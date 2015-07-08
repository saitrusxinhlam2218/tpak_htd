static char sz__FILE__[]="@(#)kdt_2.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  kdt_2.c; Rel: 6.12.0.0; Get date: 1/13/92 at 08:17:55
* @(#)  Last delta: 9/29/91 at 22:39:40
* @(#)  SCCS File: /taxi/sccs/s.kdt_2.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

/* Based on R3 version 5.20 */

#include <stdio.h>
#include <sys/types.h>
#include <memory.h>
#include <string.h>
#include "taxipak.h"
#include "Object.h"
#include "List_public.h"
#include "dispatch_strdefs.h"
#include "dispatch.h"
#include "language.h"
#include "mads_types.h"
#include "sprintf.h"
#include "mads_isam.h"
#include "mad_error.h"
#include "kdt.h"
#include "veh_driv.h"
#include "df_maint.h"
#include "except.h"
#include "calls.h"
#include "mad_ipc.h"
#include "msg_mmp.h"
#include "itm.h"
#include "TimeCalls_private.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "Zone_public.h"
#include "Zone_private.h"
#include "Zone_lists_public.h"
#include "enhance.h"
#include "switch_ext.h"
#include "ex_key.h"
#include "GPS_private.h"
#include "TimeCalls_public.h"
#include "writer.h"

#define PENALTY_YES	0
extern 	time_t 	mads_time;				/* system time */
extern 	struct 	offsets *offset;				/* offsets of structures in shared memory */
extern 	struct fleets *fleet[];		/* shared memory fleet structure */
extern 	short get_short_time();
extern 	char *scratch_ptr;
extern char *tmp_req_buf[];

/* Approach tariff goodies */
#define NO_APPROACH_TARIFF '1'
#define PICKUP_WITH_METER_ON '2'

char *GPS_strstr( char * );

/** Airport Zone specific defines for Stockholm **/
#define AIRPORT_ZONE  473
#define AIRPORT_ZONE_FILE "/usr/taxi/run/airport_data"

/*****************************************************************************/
/*                                                                           */
/*   <PCKUP> - KDT load     Command                                          */
/*                                                                           */
/*   k_pckup () : process KDT PCKUP (pickup) command.                        */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         is_pckup_allowed ()  is pckup usage allowed                       */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_pickup ()          pickup process (dispatch)                    */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_pckup(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{
  

  Veh_process_key((VEH_HNDL) veh_ptr, VEH_PICKUP_KEY, HNDL_NULL, HNDL_NULL);
  
  return;
}

/*****************************************************************************/
/*                                                                           */
/*   <CLOSE> - KDT Meter Off Command                                         */
/*                                                                           */
/*   k_close () : process KDT CLOSE (call close) command.               */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_call_close ()      close process (dispatch)                */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_close(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{
  if ( !(BUSY(veh_ptr)) )
    {
      if (OFFERED(veh_ptr))
	send_text_predefined (term_id, MUST_PCKP_FIRST, veh_ptr);
      else
	send_text_predefined (term_id, NO_CLS_NO_PKP, veh_ptr);
      
      return;
    }

  if ( veh_ptr->t_status.meter_on )
    {
      send_text_predefined (term_id, NO_CLOSE_ALW, veh_ptr);
      return;
    }

  if (veh_ptr->call_nbr)
    {
      if (!veh_ptr->t_status.meter_on)
	Veh_process_key((VEH_HNDL) veh_ptr, VEH_CLOSE_KEY, HNDL_NULL, HNDL_NULL);
    }
  else 
    Veh_reset_state((VEH_HNDL) veh_ptr, VEH_FLAG_STATE, HNDL_NULL, HNDL_NULL);
  
  return;
}

/*****************************************************************************/
/*                                                                           */
/*   <NSHOW> - Customer No Show Command                                      */
/*                                                                           */
/*   k_noshow () : process KDT NSHOW (no show) command.                      */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_no_show ()         dispatch nshow process                       */
/*                                                                           */
/*      Data Structure Accessed:                                             */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_noshow(msg, veh_ptr, term_id, noshow_origin)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
int     noshow_origin;
{
  char  *pGPSdata;
  char  *pNoShowReason;

  if ( ( pGPSdata = GPS_strstr( msg->text ) ) != NULL )
    {
      if ( strlen( pGPSdata ) == GPS_MESSAGE_LEN )
	{
	  pGPSdata += 2;
	  local_gps_form_hndl( veh_ptr, pGPSdata );
	  gps_msg_hndl( veh_ptr, MDT_MSG_NOTRIP );
	}
    }

  if ( ( ( pNoShowReason = strstr( msg->text, FIELD_END ) ) != NULL ) &&
       ( !(0) ) )
    {
      pNoShowReason += 2;
      Veh_process_key((VEH_HNDL) veh_ptr, VEH_NOSHOW_KEY, (HNDL)(atoi(pNoShowReason)), (HNDL)0);
    }
  else
    {
      if ( noshow_origin == ITM_NO_SHOW )
	Veh_process_key((VEH_HNDL) veh_ptr, VEH_NOSHOW_KEY, (HNDL)(0), (HNDL)(noshow_origin));
      else
	Veh_process_key((VEH_HNDL) veh_ptr, VEH_NOSHOW_KEY, (HNDL)(0), HNDL_NULL);
    }
}

/*****************************************************************************/
/*                                                                           */
/*   <ACPT> - Accept a Call Command                                          */
/*                                                                           */
/*   k_accept () : process KDT ACPT (accept) command.                        */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_accept ()          accept process (dispatch)                    */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_accept(msg, veh_ptr, term_id)
     MSG_QUEUE_HDR   *msg;     
     struct veh_driv *veh_ptr;
     char	*term_id;
{

  char   *pGPSdata;

  if ( msg != NULL )
    {
      if ( strlen( msg->text ) )
	{
	  if ( ( pGPSdata = GPS_strstr( msg->text ) ) != NULL )
	    {
	      if ( strlen( pGPSdata ) == GPS_MESSAGE_LEN )
		{
		  pGPSdata += 2;
		  local_gps_form_hndl( veh_ptr, pGPSdata );
		  gps_msg_hndl( veh_ptr, MDT_MSG_ACCEPT );	  
		}
	    }
	}
    }
  
  Veh_process_key((VEH_HNDL) veh_ptr, VEH_ACCEPT_KEY , NULL, HNDL_NULL);

}

/*****************************************************************************/
/*                                                                           */
/*   <S-OUT> - Sign Off Command                                              */
/*                                                                           */
/*   k_sign_out () : process KDT S-OUT command.                              */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_sign_out ()        sign out process (dispatch)                  */
/*                                                                           */
/*      Data Structure Accessed:                                             */
/*         vehicle             veh_driv struct (creator: dispatch)           */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_sign_out(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{
   
  Veh_process_key((VEH_HNDL) veh_ptr, VEH_SIGN_OFF_KEY , NULL, HNDL_NULL);
}    /* k_sign_out */

/*****************************************************************************/
/*                                                                           */
/*   <TALK> - Request to Talk Command                                        */
/*                                                                           */
/*   k_talk () : process KDT TALK (talk) command.                            */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_talk ()            talk process (dispatch)                      */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_talk(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{
  Veh_process_key((VEH_HNDL) veh_ptr, VEH_TALK_KEY , NULL, HNDL_NULL);
}

/*****************************************************************************/
/*                                                                           */
/*   <REJCT> - Reject a Call Command                                         */
/*                                                                           */
/*   k_reject () : process KDT REJCT (reject) command.                       */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_reject ()          reject process (dispatch)                    */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_reject(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{
  Veh_process_key((VEH_HNDL) veh_ptr, VEH_REJECT_KEY, (HNDL) VEH_REJECT_PENALTY, HNDL_NULL);
}

/*****************************************************************************/
/*                                                                           */
/*   <FLAG> - Flag Request Command                                           */
/*                                                                           */
/*   k_flag() : process KDT's FLAG (flag request) command.          */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_flag ()            flag process (dispatch)                      */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_flag(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{
  Veh_process_key((VEH_HNDL) veh_ptr, VEH_FLAG_REQUEST_KEY, NULL, HNDL_NULL);
  return;
}

/*****************************************************************************/
/*                                                                           */
/*   <C-OUT> - Call Out Customer Command                                     */
/*                                                                           */
/*   k_call_out () : process KDT's C-OUT (call out) command.                 */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_c_out ()           c_out process (dispatch)                     */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_call_out(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{
  Veh_process_key((VEH_HNDL) veh_ptr, VEH_CALL_OUT_KEY, HNDL_NULL, HNDL_NULL);
}

/*****************************************************************************/
/*                                                                           */
/*   <BREAK> - Going on Break Command                                        */
/*                                                                           */
/*   k_break () : process KDT's BREAK command.                               */
/*               (BREAK is allowed any time.   It only matters to us         */
/*                if driver is posted.  Driver can not take break when       */
/*                he is offered or accepting or serving a call.)             */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_break ()           break process (dispatch)                     */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_break(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{
   Veh_process_key((VEH_HNDL) veh_ptr, VEH_BREAK_KEY, HNDL_NULL, HNDL_NULL);	
}

/*****************************************************************************/
/*                                                                           */
/*   <POST> - Post into a Zone Command                                       */
/*                                                                           */
/*   k_post () : process KDT POST command.                                   */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         is_veh_suspend ()    is vehicle suspended                         */
/*         get_veh_susp_msg ()  get vehicle suspension message from          */
/*                             vehicle record                                */
/*         is_zone_valid ()     is zone number valid                         */
/*         check_seals          is a veh allowed to pickup in a zone         */
/*         d_post ()            post process (dispatch)                      */
/*                                                                           */
/*      Data Structure Accessed:                                             */
/*         vehicle             veh_driv struct (creator: dispatch)           */
/*         fleet               fleet struct (creator: dispatch)              */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         msg[index]               inbound struct (inbound msg from NCP)         */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_post(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{

  char  *pGPSdata;
  
  if ( ( pGPSdata = GPS_strstr( msg->text ) ) != NULL )
    {
      if ( strlen( pGPSdata ) == GPS_MESSAGE_LEN )
	{
	  pGPSdata += 2;
	  local_gps_form_hndl( veh_ptr, pGPSdata );
	  gps_msg_hndl( veh_ptr, MDT_MSG_POST );	  
	  *pGPSdata = '\0';	  
	}
    }
  
   Veh_process_key((VEH_HNDL) veh_ptr , VEH_POST_KEY, (HNDL) msg->text, HNDL_NULL);
}

k_roti(veh_ptr, term_id, roti_state)
     struct veh_driv *veh_ptr;
     char	*term_id;
{
  if ( roti_state == 0 )
    Veh_set_value((VEH_HNDL)veh_ptr, VEH_ROTI_STATE, (HNDL)0);
  else
    Veh_set_value((VEH_HNDL)veh_ptr, VEH_ROTI_STATE, (HNDL)1);
}

k_unpost(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{
  char *pGPSdata;

  if ( ( pGPSdata = GPS_strstr( msg->text ) ) != NULL )
    {
      if ( strlen( pGPSdata ) == GPS_MESSAGE_LEN )
	{
	  pGPSdata += 2;
	  local_gps_form_hndl( veh_ptr, pGPSdata );
	  gps_msg_hndl( veh_ptr, MDT_MSG_UNPOST );	  
	}
    }
  
  Veh_process_key((VEH_HNDL) veh_ptr, VEH_UNPOST_KEY , NULL, HNDL_NULL);
}

/*****************************************************************************/
/*                                                                           */
/*   <CPOST> - Conditional Post into a Zone                                  */
/*                                                                           */
/*   k_cpost () : process KDT CPOST (conditional post) command.              */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         is_veh_suspend ()    is vehicle suspended                         */
/*         get_veh_susp_msg ()  get vehicle suspension message from          */
/*                             vehicle record                                */
/*         is_zone_valid ()     is zone number valid                         */
/*         check_seals          is a veh allowed to pickup in a zone         */
/*         is_fleet_valid ()    is a fleet number valid                      */
/*         d_cpost ()           cpost process (dispatch)                     */
/*                                                                           */
/*      Data Structure Accessed:                                             */
/*         vehicle             veh_driv struct (creator: dispatch)           */
/*         fleet               fleet struct (creator: dispatch)              */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         msg[index]               inbound struct (inbound msg from NCP)         */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_cpost(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{
   short zone_nbr;
   short fleet_num;
   struct zones *tmp_zone_ptr;
   char *pGPSdata;

  if ( ( pGPSdata = GPS_strstr( msg->text ) ) != NULL )
    {
      if ( strlen( pGPSdata ) == GPS_MESSAGE_LEN )
	{
	  pGPSdata += 2;
	  local_gps_form_hndl( veh_ptr, pGPSdata );
	  gps_msg_hndl( veh_ptr, MDT_MSG_CPOST );	  
	  *pGPSdata = '\0';	  
	}
    }   
   
   strcat(msg->text, "\0");
   zone_nbr = atoi (msg->text);                       /* get bid zone number from msg */
   Veh_process_key((VEH_HNDL) veh_ptr, VEH_CPOST_KEY, (HNDL) zone_nbr, HNDL_NULL);
}

/*****************************************************************************/
/*                                                                           */
/*   <INQRY> - Zone Inquiry Command                                          */
/*                                                                           */
/*   k_inquiry () : process KDT's INQRY (inquiry) command.                   */
/*                                                                           */
/*      NOTE:                                                                */
/*            INQRY command is handled Entirely by this routine.  That       */
/*            is, it updates the inquiry time in the vehicle data            */
/*            structure, if applicable, dispatch process will never          */
/*            see it.                                                        */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         is_zone_valid ()     is zone number valid                         */
/*         send_zone_info ()    send zone info to KDT (driver)               */
/*                                                                           */
/*      Data Structure Accessed:                                             */
/*         vehicle             veh_driv struct (creator: dispatch)           */
/*                             (partially updated here)                      */
/*         fleet               fleet struct (creator: dispatch)              */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         msg[index]               inbound msg (inbound struct)                  */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_inquiry(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{
   short zone_nbr, num_z_fields, i, j, num_time_calls, num_calls = 0;
   long inq_time;
   short inv_zones=0;
   struct zones *pZone;
   ZONE_HNDL    hZone = HNDL_NULL;
   CALL_HNDL    call_hndl;
   ZONE_CALLS_LIST_HNDL calls_list_hndl;
   TC_STRUCT    *pTimeCall;
   FLEET_HNDL   fleet_hndl;
   static char temp [TMP_STR_LEN];
   static char temp1 [TMP_STR_LEN];
   char *rec_sep = {"\\"};                                   /* The KDT's record-separators */
   char *digits = {"0123456789"};                            /* The list of possible digits. */
   char *strpbrk (), *ptr, number [10];
   long str_len;
   int interval;				/*which interval to send*/
   FILE  *fpAirport;
   char  AirportBuf[512];
   char  *pNext;
   char  *pGPSdata;
   int   look_ahead_value = 0;
   struct drv_attributes tc_drv_attr;
   struct veh_attributes tc_veh_attr;
   struct cisam_cl   db_call;
  short           AttrSent = 0;
  char            ToMDT[ BUFSIZ ];
  char            ToMDT2[ BUFSIZ ];
  char            DueTime[ 9 ];  
  short           backup_zones[ZONE_INQUIRY_BACKUP_ZONES] = {0,0,0,0,0,0,0,0,0,0,0};
  ZONE_VEH_LIST_HNDL veh_list_hndl;
  VEH_HNDL        veh_hndl;
  int nbr_vehs_in_shift, nbr_vehs_out_shift;
   int pri_vehs_in_shift, pri_vehs_out_shift;
   struct veh_driv  *tmp_veh_ptr;
  
  bzero( ToMDT, BUFSIZ );
  bzero( ToMDT, BUFSIZ );
  
  if ( ( pGPSdata = GPS_strstr( msg->text ) ) != NULL )
    {
      if ( strlen( pGPSdata ) == GPS_MESSAGE_LEN )
	{
	  pGPSdata += 2;
	  local_gps_form_hndl( veh_ptr, pGPSdata );
	  *pGPSdata = '\0';
	}
    }   

   if (!(SIGNED_ON(veh_ptr))){						/* vehicle is not signed on */
      send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);               /* notify driver that vehicle has not signed in */
      return;
    }

   // Trap zone inquiry that is auto generated by MDT - doesn't contain any zone numbers
   
   /* check if request is too frequent (against fleetwide-defined interval) */
   // MDT controls this timer so don't do a check here!

   //   if ((inq_time = veh_ptr->inqry_time) != FALSE)                    /* get last inqry time; not a first time request */
   //   {
   //   if ((mads_time - inq_time) < (long) fleet [veh_ptr->fleet_nbr]->zone_inqry_freq)
   //      /* the interval between requests is smaller than fleetwide-defined interval, i.e., too frequent */
   //      {
   //      send_text_predefined (term_id, NO_INQ_TOO_FRE, veh_ptr);         /* notify driver: no inq, too frequent */

   //      return;
   //      }
   //   }


   /* Eliminate the weird record-separators from the KDT (viz., a /036) */
   ptr = msg->text;
   do
     if ((ptr = strpbrk (ptr, rec_sep)) == NULL)
       break;
     else
       for (j=0; j<=3; j++)
	 *(ptr + j) = '_';
   while (ptr < (msg->text + strlen (msg->text)));

   /* 091910 - support up to 10 zones in inbound message form
    get each zone number (up to 5) from inbound message data and put all zone info in a string */
   ptr = msg->text;                                                 /* Point to the start of the input string. */
   inv_zones = 0;
   num_z_fields = 0;
   temp [0] = '\0';

   if ((ptr = strpbrk(ptr, digits)) == NULL) // no zones in message - auto-generated
     {
       k_auto_zone_inquiry(msg, veh_ptr, term_id, backup_zones, 0);
     }
   else
     {
       pNext = ptr;
       for (j=0;j<=9;j++)
	 {
	   bzero(number, 10);
	   str_len = strspn(pNext, digits);
	   if (str_len > 3)
	     str_len = 3;
	   strncpy(number, pNext, str_len);
	   backup_zones[j] = atoi(number);
	   pNext = strpbrk(pNext + str_len, digits);
	   if (pNext == NULL)
	     break;
	 }
       if ( j == 0 ) // single zone queried
	 {
	   k_auto_zone_inquiry(msg, veh_ptr, term_id, backup_zones, backup_zones[0]);
	 }
     }

   
   
   for (j=0; j<=10; j++)
     {
       nbr_vehs_in_shift=nbr_vehs_out_shift=pri_vehs_in_shift=pri_vehs_out_shift=0;
       
       zone_nbr = backup_zones[j];
       if (zone_nbr == 0)
	 break;
       
       (void)Fleet_get_hndl(BY_NBR, veh_ptr->fleet_nbr, &fleet_hndl);

       if ( Zone_get_hndl( zone_nbr, veh_ptr->fleet_nbr, &hZone ) != FAIL )
	 {
	   veh_list_hndl = (ZONE_VEH_LIST_HNDL) Zone_get_value( hZone, ZONE_AVAIL_VEH_LIST );
	   Zone_veh_list_each_get( veh_list_hndl, veh_hndl )
	     {
	       tmp_veh_ptr = (struct veh_driv *)veh_hndl;	       
	       if (Vehicle_is_Valid_Shift2( veh_hndl ) )
		 {
		   if (tmp_veh_ptr->vehicle_attr.attr26 && tmp_veh_ptr->driver_attr.attr26)
		     pri_vehs_in_shift++;
		   else
		     nbr_vehs_in_shift++;
		 }
	       else
		 {
		   if (tmp_veh_ptr->vehicle_attr.attr26 && tmp_veh_ptr->driver_attr.attr26)
		     pri_vehs_out_shift++;
		   else
		     nbr_vehs_out_shift++;
		 }
	     } Zone_veh_list_each_end(veh_list_hndl)
		 
	   num_time_calls = 0;
	   num_z_fields++;                                             /* There is one (more) field to display */
	   
	   calls_list_hndl = (ZONE_CALLS_LIST_HNDL) Zone_get_value( hZone, ZONE_AVAIL_CALLS_LIST );
	   num_calls = 0;
	   Zone_calls_list_each_get( calls_list_hndl, call_hndl )
	     {
	       if ( ( Call_get_state( (CALL_HNDL)call_hndl, CALL_AVAILABLE_STATE ) ) &&
		    (!Call_get_state( (CALL_HNDL)call_hndl, CALL_WAIT_EPOST_STATE ) ) &&
		    ( veh_ptr->fleet_nbr == (short)Call_get_value( (CALL_HNDL)call_hndl, CALL_FLEET_NBR ) ) )
		 ++num_calls;
	     } Zone_calls_list_each_end( calls_list_hndl )

           bzero( ToMDT, BUFSIZ );                        
	   Zone_tc_list_each_get( Zone_get_value( hZone, ZONE_TC_LIST ), pTimeCall )
	     {

	       // Don't consider Personal Time Calls...
	       if (pTimeCall->personal)
		 continue;
	       
	       if (0)
		 {
		   if ((short)Zone_get_value( hZone, ZONE_TC_LOOK_AHEAD ) > 0 )
		     look_ahead_value = (int)((short)Zone_get_value( hZone, ZONE_TC_LOOK_AHEAD ) * 5 * 60);
		   else
		     look_ahead_value = (int)((short)Fleet_get_value( fleet_hndl, FLEET_TC_LOOK_AHEAD ) * 5 * 60);
		 }
	       else
		 look_ahead_value = (int)((short)Fleet_get_value( fleet_hndl, FLEET_TC_LOOK_AHEAD ) * 5 * 60);
	       
	       if ( (mads_time + look_ahead_value) > (time_t)TC_get_value( pTimeCall, TC_LEAD_TIME ) )
		 {
		   bzero( &tc_drv_attr, sizeof( struct drv_attributes ) );
		   bzero( &tc_veh_attr, sizeof( struct veh_attributes ) );
		   for ( i = 0; i < (ATTR_MAX - 1); i++ )
		     {
		       if ( ( pTimeCall->veh_attr[i] == YES ) )
			 set_bit( i, sizeof( struct veh_attributes ), &tc_veh_attr );
		       if ( ( pTimeCall->drv_attr[i] == YES ) )
			 set_bit( i, sizeof( struct drv_attributes ), &tc_drv_attr );
		     }
		   if ( !Vehicle_match_attr( (unsigned char *)&veh_ptr->vehicle_attr, (unsigned char *)&tc_veh_attr ) )
		     continue;
		   if ( !Vehicle_match_attr( (unsigned char *)&veh_ptr->driver_attr, (unsigned char *)&tc_drv_attr ) )
		     continue;
		   
		   if ( pTimeCall->zone != zone_nbr )
		     continue;
		   
		   bzero( &db_call, sizeof( struct cisam_cl ) );
		   db_call.nbr = pTimeCall->nbr;
		   if ( db_read_key( CALL_FILE_ID, &db_call, &cl_key1, DB_EQUAL ) != SUCCESS )
		     continue;
		   
		   if ( db_call.pckup_zone != pTimeCall->zone )
		     continue;
		   
		   if ( strncmp( db_call.status, PENDING, 5 ) )
		     continue;
		   
		   ++num_time_calls;
		   
		   AttrSent = 0;
		   
		   if ( ( j == 0 ) && ( backup_zones[j] == 0 ) ) // Query on a single zone. Provide some timcall details
		     {
		       get_srch_time( pTimeCall->due_time, DueTime );
		       sprintf( &ToMDT[strlen(ToMDT)], "%5s ", DueTime );
		       for ( i = 0; ( i < ATTR_MAX - 1 ) && ( AttrSent < 8 ); i++ )
			 {
			   if ( pTimeCall->drv_attr[i] == YES )
			     {
			       sprintf( &ToMDT[strlen(ToMDT)], "%2s ",
					fleet[veh_ptr->fleet_nbr]->vd_attr_info[i].drv_attr );
			       AttrSent++;
			     }
			 }
		       
		       for ( i = 0; ( i < ATTR_MAX - 1 ) && ( AttrSent < 8 ); i++ )
			 {
			   if ( pTimeCall->veh_attr[i] == YES )
			     {
			       sprintf( &ToMDT[strlen(ToMDT)], "%2s ",
					fleet[veh_ptr->fleet_nbr]->vd_attr_info[i].veh_attr );
			       AttrSent++;
			     }
			 }
		       sprintf( &ToMDT[strlen(ToMDT)], "%s", CR );
                       
		     }
		   else  // multiple zone query
		     {
		       get_srch_time( pTimeCall->due_time, DueTime );
		       if (strlen(ToMDT))
			 sprintf( &ToMDT[strlen(ToMDT)], "%5s ", DueTime );
		       else
			 sprintf( &ToMDT[strlen(ToMDT)], "%5s ", DueTime );
		     }
		 }
	       
	     } Zone_tc_list_each_end(Zone_get_value( hZone, ZONE_TC_LIST ))
		 
		 if(num_time_calls > 99)					/* sanity checks */
		   num_time_calls = 0;
	   
	   if(num_time_calls < 0)					/* sanity checks */
	     num_time_calls = 0;

	   sprintf(ToMDT2, "%d/%d %s", num_calls, num_time_calls, ToMDT);
	   
	   pZone = ( ZONE * )hZone;
	   
	   if ( strlen( ToMDT2 ) )
	     i = sprintf (&temp [strlen (temp)], "%03d: %02d/%02d/%02d %s", zone_nbr,  
			  pri_vehs_in_shift,
			  nbr_vehs_in_shift,
			  nbr_vehs_out_shift+pri_vehs_out_shift,
			  ToMDT2);
	   else
	     i = sprintf (&temp [strlen (temp)], "%03d: %02d/%02d/%02d", zone_nbr,
			  pri_vehs_in_shift,
			  nbr_vehs_in_shift,
			  nbr_vehs_out_shift+pri_vehs_out_shift);

	   
	   //if ( (num_z_fields % 2) == 0 )
	     sprintf ((&temp [strlen (temp)]), "%s", CR);
	 }
       else
	 {
	   if (inv_zones == 0)                               /* first invalid zone put header string first */
	     sprintf (&temp1 [0], "%s", INV_ZONE_HDR);
	   sprintf (&temp1 [strlen (temp1)], " %03d", zone_nbr);
	   
	   /* put all invalid zones in a string */
	   inv_zones++;                                          /* indicate there is a invalid zone*/
	   if (inv_zones == 4 && (strlen(temp1) < MAX_DISPLAY_LENGTH))
	     strcat(temp1,CR);
	 }
     }

   ptr = ptr + str_len;                                           /* Point to the rest of the input string. */

   
   if (inv_zones > 0)                                            /* if there are invalid zones */
     {
       if (num_z_fields == 1 || num_z_fields == 3)
         strcat(temp,CR);
       strcat (temp, temp1);                                          /* add invalid msg to the end of info display */
     }
   
   /* send zone info (and invalid zone, if any) to driver */
   veh_ptr->inqry_time = mads_time;
   send_zone_info (term_id, temp, veh_ptr);

   gps_msg_hndl( veh_ptr, MDT_MSG_INQUIRY );	  
   
 }

k_auto_zone_inquiry(msg, veh_ptr, term_id, backup_zones, primary_zone)
     MSG_QUEUE_HDR  *msg;
     struct veh_driv *veh_ptr;
     char *term_id;
     short backup_zones[];
     short primary_zone;
{
  short best_zone = 0;
  short (*srch_zones);
  FLEET_HNDL fl_hndl=HNDL_NULL;
  ZONE_HNDL  zn_hndl=HNDL_NULL;
  ZONE_HNDL  srch_zn_hndl=HNDL_NULL;
  ZONE       *pPrimary = NULL;
  int        i,k,nbr_zones;

  if ( veh_ptr == NULL )
    return; // error

  nbr_zones = 0;
  
  if ( primary_zone == 0 )
    {
      if ( GPS_get_best_zone( (VEH_HNDL)veh_ptr, &best_zone ) != SUCCESS )
	{
	  return; //error
	}
    }
  else
    {
      best_zone = primary_zone;
      //      backup_zones[0] = primary_zone;
      //      nbr_zones++;
    }

  (void)Fleet_get_hndl(BY_NBR, veh_ptr->fleet_nbr, &fl_hndl);
  if ( Zone_get_hndl( best_zone, veh_ptr->fleet_nbr, &zn_hndl) == FAIL )
    {
      return; //error
    }

  backup_zones[nbr_zones++] = best_zone;
  
  srch_zones = (short (*))Zone_get_value(zn_hndl, ZONE_AUTO_INQRY);

  pPrimary = (ZONE *)zn_hndl;

  if (*srch_zones == 0) // empty backup list, use backup zone list
    {
      for (i=2; i < LEVEL_MAX; i++)
	{
	  for (k=0; k < BCK_UP_MAX; k++)
	    {
	      if ( pPrimary->level_number[i].bck_up_zone[k] > 0 )
		{
		  backup_zones[nbr_zones++] = pPrimary->level_number[i].bck_up_zone[k];
		  if (nbr_zones == ZONE_INQUIRY_BACKUP_ZONES)
		    break;
		}
	    }
	  if (nbr_zones == ZONE_INQUIRY_BACKUP_ZONES)
	    break;
	}
    }
  else
    {
      for (i=0; i < ZONE_INQUIRY_BACKUP_ZONES; i++)
	{
	  if (*(srch_zones+i) != 0)
	    {
	      backup_zones[nbr_zones++] = *(srch_zones+i);
	      if (nbr_zones == ZONE_INQUIRY_BACKUP_ZONES)
		break;
	    }
	}
	  
    }
  
}

/*****************************************************************************/
/*                                                                           */
/*   <BID> - Fleetwide Call Offer Response Command                           */
/*                                                                           */
/*   k_bid () : process KDT BID (bid) command.                               */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         is_bid_offered ()    is a bid call offered                        */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_bid ()             bid process (dispatch)                       */
/*                                                                           */
/*      Data Structure Accessed:                                             */
/*         vehicle             veh_driv struct (creator: dispatch)           */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         msg[mindex]               message					*/
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_bid(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{

  
}

k_zonecheck(msg, veh_ptr, term_id)
     MSG_QUEUE_HDR  *msg;
     struct veh_driv *veh_ptr;
     char *term_id;
{
  short i;
  char nbr_zones[16];
  char sZoneNbr[8];
  char sChecksum[8];
  char sZoneChecksum[8];
  char *str_ptr, *tmp_ptr;
  ZONE_REC   zone_rec;		/* cisam  returned zone structure */
  ZONEDOWN_REC zonedown_rec;
  
  if (veh_ptr == NULL)
    return;

  str_ptr = strtok(msg->text, ",");  // ptr to first field - packet number - not very meaningful

  if ( str_ptr == NULL )
    return;
  str_ptr = strtok(NULL, ","); // 2nd field - total number of packets - not very meaningful

  if ( str_ptr == NULL )
    return;

  str_ptr = strtok(NULL, ",");  // 3rd field - number of zones in this checksum message

  if ( str_ptr == NULL )
    return;

  strcpy( nbr_zones, str_ptr );

  str_ptr = strtok(NULL, ",");  // checksum content  - length should be nbr_zones * 7 + 4 for terminators
  
  if (strlen(str_ptr) != atoi(nbr_zones) * 7 + 4)
    return;

  db_open(ZONE_FILE_ID, ISINOUT + ISMANULOCK);
  db_open(ZONEDOWN_FILE_ID, ISINOUT + ISMANULOCK);
  bzero(&zone_rec, sizeof(ZONE_REC));
  bzero(&zonedown_rec, sizeof(ZONEDOWN_REC));
  
  zone_rec.fleet = 'H';
  
  for ( i = 0; i < atoi(nbr_zones); i++ )
    {
      bzero(sZoneNbr, 8);
      bzero(sChecksum, 8);
      bzero(sZoneChecksum, 8);
      strncpy(sZoneNbr, str_ptr, 3);
      strncpy(sChecksum, str_ptr + 3, 4);
      printf("Zone %s Checksum %s\n", sZoneNbr, sChecksum);
      zone_rec.nbr = atoi(sZoneNbr);
      if ( db_read_key(ZONE_FILE_ID, &zone_rec, &zn_key1, ISEQUAL) == SUCCESS )
	{
	  // do checksums match?
	  if (strncmp(sChecksum, zone_rec.poly_checksum, 4)) // mismatch...add an entry to zonedown table
	    {
	      // do we already have an entry?
	      zonedown_rec.fleet = 'H';
	      zonedown_rec.veh_nbr = veh_ptr->veh_nbr;
	      zonedown_rec.zone_nbr = atoi(sZoneNbr);
	      if ( db_read_key(ZONEDOWN_FILE_ID, &zonedown_rec, &zonedown_key1, ISEQUAL) != SUCCESS )
		{
		  zonedown_rec.entered_datetime = mads_time;
		  db_write(ZONEDOWN_FILE_ID, &zonedown_rec);
		}
	    }
	}
      str_ptr = str_ptr + 7;
    }
  db_close(ZONE_FILE_ID);
  db_close(ZONEDOWN_FILE_ID);
}

k_stand_notify(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{
  char *pGPSdata = NULL, *pField = NULL;
  int notify_status = 0;
  int zone_nbr;
  if ( ( pGPSdata = GPS_strstr( msg->text ) ) != NULL )
    {
      if ( strlen( pGPSdata ) == GPS_MESSAGE_LEN )
	{
	  pGPSdata += 2;
	  local_gps_form_hndl( veh_ptr, pGPSdata );
	  gps_msg_hndl( veh_ptr, MDT_MSG_MSG );	  
	  *pGPSdata = '\0';	  
	}
    }

  if ( ( pField = strstr( msg->text, FIELD_END ) ) != NULL )
    {
      pField += 2;
      notify_status = atoi( pField );

      if ( atoi( msg->text ) > 0 )
	{
	  zone_nbr = atoi(msg->text);
	  Veh_process_key((VEH_HNDL) veh_ptr, VEH_STAND_NOTIFY_KEY, (HNDL)zone_nbr, (HNDL)notify_status);
	}
      
    }
}

/*****************************************************************************/
/*                                                                           */
/*   <S-IN> - Sign In Command                                                */
/*                                                                           */
/*   k_sign_in () : process KDT S-IN (sign in) command.                      */
/*                 (sign-in is allowed for suspended driver/vehicle)         */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         is_veh_suspend ()    is vehicle suspended                         */
/*         chk_driver ()        check status of a driver                     */
/*         get_veh_susp_msg ()  get vehicle suspension message from          */
/*                             vehicle record                                */
/*         d_sign_in ()         sign in process (dispatch)                   */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         msg               inbound text field from the message             */
/*        veh_ptr             pminter to a vehicle data structure            */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the                       */
/*                                                                           */
/*****************************************************************************/

k_sign_in(msg, veh_ptr, term_id, drv_validate )
char msg[];
struct veh_driv *veh_ptr;
char	*term_id;
char drv_validate;  // used to validate driver ID serial number
{
   int drv_id;
   char *pGPSdata;



   strcat (msg, "\0");
   drv_id = atoi (msg);     /* get driver's ID (drv_id) from inbound msg */
   
   veh_ptr->t_status.kdt_working = 1;
   Veh_process_key((VEH_HNDL) veh_ptr, VEH_SIGN_IN_KEY, (HNDL) drv_id, (HNDL) drv_validate );

   if ( ( pGPSdata = GPS_strstr( msg ) ) != NULL )
     {
       if ( strlen( pGPSdata ) == GPS_MESSAGE_LEN )
	 {
	   pGPSdata += 2;
	   local_gps_form_hndl( veh_ptr, pGPSdata );
	   gps_msg_hndl( veh_ptr, MDT_MSG_SIGNIN );	  
	   *pGPSdata = '\0';	  
	 }
     }   
 }

/*****************************************************************************
*                                                                           *
*   <MSG> - Message to Dispatcher Command                                   *
*                                                                           *
*   k_msg () : process KDT MSG (message) command.                       *
*                                                                           *
*      ROUTINES Called:                                                     *
*         k_talk()             handle request-to-talk
*         k_break()            handle go-on-break
*         k_call_out()         handle call-out
*         k_flag()             handle flag-request
*         send_text_predefined ()         send text to KDT (driver)                    *
*         d_msg ()             message process (dispatch)                   *
*                                                                           *
*      GLOBALS:                                                             *
*         veh_ptr             pointer to a vehicle data structure           *
*                                (veh_driv struct)                          *
*         msg[index]               inbound struct (inbound msg from NCP)         *
*         term_id         terminal id (with \0 at the end of 4th char)  *
*                                                                           *
*****************************************************************************/

k_msg(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{
  char *pGPSdata = NULL, *pField = NULL;
  short to_veh_id = 0;
  
  if ( ( pGPSdata = GPS_strstr( msg->text ) ) != NULL )
    {
      if ( strlen( pGPSdata ) == GPS_MESSAGE_LEN )
	{
	  pGPSdata += 2;
	  local_gps_form_hndl( veh_ptr, pGPSdata );
	  gps_msg_hndl( veh_ptr, MDT_MSG_MSG );	  
	  *pGPSdata = '\0';	  
	}
    }

  /**
      Messages from vehicle to dispatch center appear as <canmsg#>%D  %D%E
      Messages from vehicle to vehicle appear as <canmsg#>%D<veh#>%D%E

      Messages for special TSAB functions appear as 99%D<action#>%D%E
      
  **/
  if ( ( pField = strstr( msg->text, FIELD_END ) ) != NULL ) 
    {
      pField += 2;
      to_veh_id = atoi( pField );
      
      if (0)  // is vehicle to vehicle messaging enabled
        {
          if ( to_veh_id > 0 )
            {
              /* vehicle to vehicle message */
              Veh_process_key( (VEH_HNDL) veh_ptr, VEH_MSG_KEY, (HNDL)(atoi(msg->text)), (HNDL)to_veh_id );
              return;
            }
        }
      
      if ( ( atoi(msg->text) == 99 ) && to_veh_id > 0 )  // triggers special TSAB information services
        {
	  //          Veh_process_key( (VEH_HNDL) veh_ptr, VEH_MSG_KEY, (HNDL)(atoi(msg->text)), (HNDL)to_veh_id );
	  //          return;
        }          
    }

  /* vehicle to dispatch center message */
  if ( atoi( msg->text ) > 0 )
    Veh_set_state((VEH_HNDL) veh_ptr, VEH_MSG_STATE, (HNDL)(atoi (msg->text)));                 
}

/*****************************************************************************/
/*                                                                           */
/*   <INFO> - Vehicle Inquiry Command                                        */
/*                                                                           */
/*   k_info (zone_nbr) : process KDT's INFO (information) command.           */
/*                                                                           */
/*      NOTE:                                                                */
/*            INFO command is handled Entirely by this routine.  That        */
/*            is, it updates the vehicle info request time in the            */
/*            vehicle data structure, if applicable, and dispatch            */
/*            process will never be invoked.                                 */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         is_veh_valid ()      is vehicle ID valid                          */
/*                                                                           */
/*      Data Structure Accessed:                                             */
/*         vehicle             veh_driv struct (creator: dispatch)           */
/*                             (partially updated here)                      */
/*         fleet               fleet struct (creator: dispatch)              */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         msg[index]               inbound struct (inbound msg from NCP)         */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_info(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{
  char  *pGPSdata;

  if ( ( pGPSdata = GPS_strstr( msg->text ) ) != NULL )
    {
      if ( strlen( pGPSdata ) == GPS_MESSAGE_LEN )
	{
	  pGPSdata += 2;
	  local_gps_form_hndl( veh_ptr, pGPSdata );
	  gps_msg_hndl( veh_ptr, MDT_MSG_QPOS );	  
	}
    }
  
  Veh_process_key((VEH_HNDL) veh_ptr, VEH_Q_POSITION_KEY, HNDL_NULL, HNDL_NULL );
}

/*****************************************************************************/
/*                                                                           */
/*   <EPOST> - Enroute Post into a Zone Command                              */
/*                                                                           */
/*                                                                           */
/*   k_epost () : process KDT's EPOST (enroute post) command.                */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         is_veh_suspend ()    is vehicle suspended                         */
/*         get_veh_susp_msg ()  get vehicle suspension message from          */
/*                             vehicle record                                */
/*         is_zone_valid ()     is zone number valid                         */
/*         check_seals          is a veh allowed to pickup in a zone         */
/*         d_epost ()           epost process (dispatch)                     */
/*                                                                           */
/*      Data Structure Accessed:                                             */
/*         vehicle             veh_driv struct (creator: dispatch)           */
/*         fleet               fleet struct (creator: dispatch)              */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         msg[index]               inbound msg (inbound struct)                  */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_epost(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{
   short zone_nbr;
   long fleet_num;
   struct zones *tmp_zone_ptr;
   char *pGPSdata;

  if ( ( pGPSdata = GPS_strstr( msg->text ) ) != NULL )
    {
      if ( strlen( pGPSdata ) == GPS_MESSAGE_LEN )
	{
	  pGPSdata += 2;
	  local_gps_form_hndl( veh_ptr, pGPSdata );
	  gps_msg_hndl( veh_ptr, MDT_MSG_EPOST );	  
	  *pGPSdata = '\0';	  
	}
    }   

   Veh_process_key((VEH_HNDL) veh_ptr, VEH_EPOST_KEY, (HNDL) msg->text, HNDL_NULL);
}

/*****************************************************************************/
/*                                                                           */
/*   <ETA> - Estimated Time of Arrival Command                               */
/*                                                                           */
/*   k_eta () : process KDT's ETA (Estimated Time of Arrival) command.       */
/*             (Driver can ETA at any time.   If ETA follows a call          */
/*              offer, it implies ACPT, otherwise informational              */
/*              exception message is generated for a supervisor.             */
/*              This routine does not generate the exception.)               */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_eta ()             eta process (dispatch)                       */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         msg[index]               inbound msg (inbound struct)                  */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_eta(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{
   int est_time;
   char *pGPSdata;

  if ( ( pGPSdata = GPS_strstr( msg->text ) ) != NULL )
    {
      if ( strlen( pGPSdata ) == GPS_MESSAGE_LEN )
	{
	  pGPSdata += 2;
	  local_gps_form_hndl( veh_ptr, pGPSdata );
	  gps_msg_hndl( veh_ptr, MDT_MSG_ETA );	  
	  *pGPSdata = '\0';	  
	}
    }   

   strcat (msg->text, "\0");
   est_time = atoi (msg->text);                                  /* get estimated time from msg */
   Veh_process_key((VEH_HNDL) veh_ptr, VEH_ETA_KEY, (HNDL) est_time, HNDL_NULL);
}

/*****************************************************************************/
/*                                                                           */
/*   <M> - Emergency Command                                                 */
/*                                                                           */
/*   k_emergency () : process KDT's M (emergency) command.                   */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         emerg_ack ()         send emergency ack to KDT (driver)           */
/*         d_emergency ()       emergency process (dispatch)                 */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_emergency(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{

	if (veh_ptr->t_status.emer)
		return;
	else
	  {
	    Veh_set_state((VEH_HNDL) veh_ptr, VEH_EMERGENCY_STATE, HNDL_NULL);
	  }
}

/*****************************************************************************/
/*                                                                           */
/*     - Actual Meter On Command                                             */
/*                                                                           */
/*   k_meter_on () : process KDT meter-on (flag down) command.               */
/*                  (always allowed as long as signed on)                    */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_pickup ()         meter-on process (dispatch)                 */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_meter_on(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{
	Veh_process_key((VEH_HNDL) veh_ptr, VEH_METER_ON_KEY, HNDL_NULL, HNDL_NULL);
}

/*****************************************************************************/
/*                                                                           */
/*   k_meter_off () : process KDT meter-off (flag up) command.               */
/*                   (always allowed as long as signed on)                   */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_call_close ()      close process (dispatch)                */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_meter_off(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{
  Veh_process_key((VEH_HNDL) veh_ptr, VEH_METER_OFF_KEY, HNDL_NULL, HNDL_NULL);
}

k_mfault(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{
  Veh_process_key((VEH_HNDL) veh_ptr, VEH_METER_FAULT_KEY, HNDL_NULL, HNDL_NULL);
}



char *
GPS_strstr( pMMP_msg )
     char   *pMMP_msg;
{
  char  *pDelimiter;
  
  if ( pMMP_msg == NULL )
    return ( NULL );

  /*
     First look for the escaped '%P' text.  If this is not present,
     then we can just do a straight 'strstr' for '%P' and get the
     proper result.  Otherwise, we do a char-by-char search for '%P'
     */

  if ( ( pDelimiter = strstr( pMMP_msg, "%%P" ) ) == NULL )
    return ( strstr( pMMP_msg, GPS_DELIMITER ) );
  else
    {
      pDelimiter = pMMP_msg;
      while ( *pDelimiter != '\0' )
	{
	  if ( ( *pDelimiter == '%' ) &&
	       ( *(pDelimiter + 1) == 'P' ) &&
	       ( *(pDelimiter - 1) != '%' ) )
	    return( pDelimiter );
	  else
	    ++pDelimiter;
	}
    }
  return ( NULL );
}
	
  
