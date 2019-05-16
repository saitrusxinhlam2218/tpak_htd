/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Vehicle_key.c,v $
*  @(#)  $Revision: 1.45 $
*  @(#)  $Date: 2004/06/22 17:08:17 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Vehicle_key.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Vehicle_key.c,v 1.45 2004/06/22 17:08:17 jwelch Exp $";


/* Vehicle_key.c                                                           */
#include <sys/types.h>		/* temp */
#include <sys/times.h>		/* temp */
#include "mad_error.h"
#include "taxipak.h"
#include "Object.h"
#include "Vehicle_public.h"
#include "List_public.h"
#include "Vehicle_lists_public.h"
#include "Fleet_lists_public.h"
#include "Driver_public.h"
#include "Call_public.h"
#include "Zone_public.h"
#include "Zone_private.h"
#include "Zone_lists_public.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "dispatch.h"
#include "msg_defs.h"
#include "msg_mmp.h"
#include "acc.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "Call_public.h"
#include "Call_private.h"
#include "language.h"
#include "GPS_private.h"
#include "dispatch_strdefs.h"
#include "f_msg_defs.h"
#include "except.h"
#include "mad_ipc.h"
#include "kdt.h"
#include "itm.h"
#include "enhance.h"
#include "switch_ext.h"
#include "Veh_canmsg_db.h"
#include "S_Shift_db.h"
#include "S_Sched_db.h"
#include "taxi_db.h"
#include "KelaNode_db.h"

extern FLEET *fleet[ ];	/* shared memory fleet structure */
extern time_t mads_time;
extern short debug_state;
extern struct offsets *offset;

static LIST_HNDL key_list;

#define ZONE_BACKUP_MAX			BCK_UP_MAX
typedef short BACKUP_ZONE[ZONE_BACKUP_MAX];

extern BOOLEAN Vehicle_is_Valid_Shift2( VEH_HNDL );
BOOLEAN Time_in_Shift( int, char, int );

/*************************************************************************/
/*                                                                       */
/* is_suspended ()                                                       */
/*                                                                       */
/* check if the vehicle or the driver is suspended                       */
/* send a message of notification and a reason if any                    */
/*                                                                       */
/*************************************************************************/

int
is_suspended(id, veh_ptr, type, do_msg)
   char *id;
   struct veh_driv *veh_ptr;
   int   type;			/* who to check */
   int   do_msg;
{
   char *suspend_msg;

   switch (type)
   {
   case DRIVER:
   case EITHER:
      /* if driver suspended send notification and reason */
      if (Driver_get_state((DRIVER_HNDL) veh_ptr, DRIVER_SUSPENDED_STATE) == TRUE)
      {				/* check driver status: suspended ? */
	 if (do_msg)
	 {

	    suspend_msg = (char *) Driver_get_value((DRIVER_HNDL) veh_ptr, DRIVER_SUSP_DRV_MSG);
	    if (suspend_msg != NULL)
	       if (*suspend_msg != '\0')
		  send_text(id, suspend_msg, veh_ptr, PRIORITY_1);
	    send_text_predefined(id, SUSPND_DRV, veh_ptr);
	 }
	 return (TRUE);
      }
      if (type != EITHER)
	 break;

   case VEHICLE:
      /* if vehicle suspended send notification and reason */
      if (Veh_get_state((VEH_HNDL) veh_ptr, VEH_SUSPENDED_STATE) == TRUE)
      {
	 if (do_msg)
	 {
	    suspend_msg = (char *) Veh_get_value((VEH_HNDL) veh_ptr, VEH_SUSP_MSG);
	    if (suspend_msg != NULL)
	       if (*suspend_msg != '\0')
		  send_text(id, suspend_msg, veh_ptr, PRIORITY_1);
	    send_text_predefined(id, SUSPND_VEH, veh_ptr);
	 }
	 return (TRUE);
      }
      break;
   }				/* end switch */
   return (FALSE);
}				/* end is_suspended */

/***********************************************************/
/* RET_STATUS Veh_process_key(mid, key_type, argument)     */
/* OP: process a key input (or ack\nak)                    */
/* PRE: none                                               */
/* POST: key procesing complete                            */
/* ERROR: no vehicle                                       */
/***********************************************************/

RET_STATUS
Veh_process_key(
   VEH_HNDL veh_hndl,
   short key_type,
   HNDL  argument,
   HNDL  argument2)
{
   struct veh_driv *veh_ptr, *to_veh_ptr;
   struct calls *call_ptr;
   struct calls *tmp_call_ptr;
   CALL_HNDL tmp_call_hndl;
   VEH_HNDL  tmp_veh_hndl;
   BOOLEAN     veh_found_for_flag = FALSE;
   short got_call = 0;
   short zone_nbr;
   ZONE_HNDL zone_hndl, backup_zone_hndl;
   FLEET_HNDL fleet_hndl;
   struct zones *zone_ptr;	/* temp */
   char  error_str[80];
   char  penalty;
   CALL *call_list_rec;
   short term_type = 0;
   time_t  tmp_time;
   BACKUP_ZONE backup_zone;
   int i, k, j;
   short level_offered;   
   short timeout;
   VEH_CANMSG_REC  veh_cmsg;
   char            msg_date[16];
   char            msg_time[16];
   char            to_veh_msg[256];
   time_t          time_sent = 0;
   char            tmp_str[256];
   char            DueTime[ 9 ];
   unsigned char mask;
   unsigned char *addr;
   int jj,lines_sent = 0;
   BOOLEAN msg_to_send = FALSE, first_msg = TRUE;
   char            tmp_buf[ 512 ];
   struct cisam_sh s_sched;
   struct cisam_ss s_shift;
   struct cisam_s_version sv;
   int mode = 0;
   struct tm *tm_ptr;
   int curTime = 0;
  char    *stime, *ctime();
  time_t tmtime;
  KELANODE_REC kelanode_rec;  
   
   veh_ptr = (struct veh_driv *) veh_hndl;

   /** Assertion check to make validate the call_ptr referenced by vehicle **/
   if ( veh_ptr->call_ptr != NULL )
     {
       if ( veh_ptr != veh_ptr->call_ptr->veh_ptr )
	 {
	   if ( veh_ptr->call_ptr->veh_ptr != NULL )
	     {
#ifdef FOO
	       sprintf( error_str, "ASSERT: invalid call reference vh%d -> cl%d ->vh%d",
		       veh_ptr->veh_nbr, veh_ptr->call_ptr->call_number, veh_ptr->call_ptr->veh_ptr->veh_nbr );
	       ERROR(' ', "", error_str );
#endif
	     }
	   else
	     {
	       sprintf( error_str, "ASSERT: invalid call reference vh%d -> cl%d -> NULL",
		       veh_ptr->veh_nbr, veh_ptr->call_ptr->call_number );
	       ERROR(' ', "", error_str );
	     }
	   veh_ptr->call_ptr = NULL;
	   return(FAIL);
	 }
     }
   
   switch (key_type)
   {

   case VEH_STAND_NOTIFY_KEY:
     if (!(SIGNED_ON(veh_ptr)))
       {
	 send_text_predefined(veh_ptr->mid, NOT_SIGN_IN, veh_ptr);
	 return (FAIL);
      }
     if (Fleet_get_hndl(BY_NBR, veh_ptr->fleet_nbr, &fleet_hndl) == FAIL)
       {
	 ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", "Fleet nbr is out of boundary");
	 return (FAIL);
       }
     if (Zone_get_hndl((short)argument, veh_ptr->fleet_nbr, &zone_hndl) == FAIL)
       {
	 return (FAIL);
       }
     //Look for the zone in the stand_bcast_report
     for (i=0;i<MAX_STAND_BCAST_REPORT;i++)
       {
	 if (fleet[veh_ptr->fleet_nbr]->bcast_report[i].zone_num == (short)argument)
	   {
	     if (fleet[veh_ptr->fleet_nbr]->bcast_report[i].stand_bcast_type < (short)argument2)
	       {
		 fleet[veh_ptr->fleet_nbr]->bcast_report[i].stand_bcast_type = (short)argument2;
		 fleet[veh_ptr->fleet_nbr]->bcast_report[i].bcast_time = 0;
	       }
	     fleet[veh_ptr->fleet_nbr]->bcast_report[i].last_report_time = mads_time;
	     break;
	   }
       }
     if ( i == MAX_STAND_BCAST_REPORT ) // didn't find it
       {
	 // find first open slot
	 for (i=0;i<MAX_STAND_BCAST_REPORT;i++)
	   {
	     if (fleet[0]->bcast_report[i].zone_num == 0)
	       {
		 fleet[veh_ptr->fleet_nbr]->bcast_report[i].zone_num = (short)argument;
		 fleet[veh_ptr->fleet_nbr]->bcast_report[i].stand_bcast_type = (short)argument2;
		 fleet[veh_ptr->fleet_nbr]->bcast_report[i].last_report_time = mads_time;
		 break;
	       }
	   }
       }
     
     break;
     
   case VEH_ACCEPT_KEY:
/******************************************************************/
/* VEH_ACCEPT_KEY                                                 */
/*  OP: pre process the accept msg                                */
/*  ARG:     none                                                 */
/*  PRE: veh exists                                               */
/*  POST:                                                         */
/*        VEH_ACCEPT_STATE set                                    */
/*  ERROR: if !VEH_SIGN_IN_STATE                                  */
/*                send NOT_SIGN_IN msg to vehicle                 */
/*         if VEH_METER_ON_STATE                                  */
/*             && VEH_ACCEPTED_STATE                              */
/*               send NO_POST_MTR_ON msg to vehicle               */
/*         if !VEH_CALL_OFFERED_STATE                             */
/*                 send NO_ACPT_NO_OFFR  msg to vehicle           */
/*         if VEH_ACCEPTED_STATE &&                               */
/*                 send NO_ACPT_ACPT  msg to vehicle              */
/******************************************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_ACCEPT_KEY %d\n", veh_ptr->veh_nbr);
#endif
      if (!(SIGNED_ON(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NOT_SIGN_IN, veh_ptr);
	 return (FAIL);
      }
      if (veh_ptr->t_status.accepted)
      {
	if (veh_ptr->call_ptr != NULL)
	  {
	    call_ptr = veh_ptr->call_ptr;
	    Call_send_assign((CALL_HNDL)call_ptr, (VEH_HNDL)veh_ptr, FALSE);
	  }
	return(SUCCESS);
      }
      if (veh_ptr->t_status.meter_on)
      {

	 if (veh_ptr->t_status.accepted)	/* SMS P.1859 */
	 {
	    send_text_predefined(veh_ptr->mid, NO_POST_MTR_ON, veh_ptr);
	    return (FAIL);
	 }
      }
      if (!(OFFERED(veh_ptr)))
      {
	send_text_predefined(veh_ptr->mid, NO_ACPT_NO_OFFR, veh_ptr);
	return (FAIL);
      }
      //      if (veh_ptr->t_status.accepted)
      //      {
      //	send_text_predefined(veh_ptr->mid, NO_ACPT_ACPT, veh_ptr);
      //	return (FAIL);
      //      }
      
      call_ptr = veh_ptr->call_ptr;

      return (Veh_set_state(veh_hndl, VEH_ACCEPT_STATE, (HNDL) call_ptr));

   case VEH_ACK_KEY:
/********************************************************/
/* VEH_ACK_KEY                                          */
/*  OP: process the acknowledge message                 */
/*  ARG:    none                                        */
/*  PRE: none                                           */
/*  POST: reset_pkt_entry                               */
/*  ERROR: if no vehicle                                */
/*              nak_error(msg, "No Vehicle Ptr") called */
/********************************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_ACK_KEY\n");
#endif
      if (debug_state)
	 ERROR(' ', "", "VEH_ACK_KEY not implemented");
      break;

   case VEH_BREAK_KEY:
/****************************************************/
/* VEH_BREAK_KEY                                    */
/*  OP: preproces break request msg                 */
/*  ARG: none                                       */
/*  PRE: veh exists                                 */
/*  POST:                                           */
/*        if VEH_POSTED_STATE                       */
/*             VEH_ON_BREAK_STATE set               */
/*        else                                      */
/*              break time message sent to vehicle  */
/*  ERROR: if !VEH_SIGN_IN_STATE                    */
/*              NOT_SIGN_IN send to vehicle         */
/*         if VEH_ON_CALL_STATE                     */
/*              NO_BRK_MTR_ON send to vehicle       */
/*         if VEH_ACCEPTED_STATE                    */
/*              NO_BRK_ACPT send to vehicle         */
/*         if VEH_CALL_OFFERED_STATE                */
/*              NO_BRK_OFFRD send to vehicle        */
/*         if VEH_ON_BREAK_STATE                    */
/*              ALREADY_ON_BREAK send to vehicle    */
/****************************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_BREAK_KEY\n");
#endif
      if (!(SIGNED_ON(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NOT_SIGN_IN, veh_ptr);
	 return (FAIL);
      }
      if (!POSTED(veh_ptr))
      {
	 send_text_predefined(veh_ptr->mid, NOT_POST, veh_ptr);
	 return (FAIL);
      }

      if (veh_ptr->t_status.crse_annul && (0))
	{
	  send_text_predefined(veh_ptr->mid, NO_BRK_MTR_ON, veh_ptr);
	  return (FAIL);
	}
      
      if (BUSY(veh_ptr))
      {
	 send_text_predefined(veh_ptr->mid, NO_BRK_MTR_ON, veh_ptr);
	 return (FAIL);
      }
      if ((OFFERED(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NO_BRK_OFFRD, veh_ptr);
	 return (FAIL);
      }
      if (veh_ptr->t_status.on_break)
      {
	 send_text_predefined(veh_ptr->mid, ALREADY_ON_BREAK, veh_ptr);
	 return (FAIL);
      }
      if (!veh_ptr->t_status.posted)
      {
	 sprintf(error_str, "%s %d %s", BREAK_TIME, fleet[veh_ptr->fleet_nbr]->break_duration / 60, MINUTES);
	 (void) send_text(veh_ptr->mid, error_str, veh_ptr, PRIORITY_2);	/* send it out */
	 return (SUCCESS);
      } else
	 Veh_set_state((VEH_HNDL) veh_ptr, VEH_ON_BREAK_STATE, HNDL_NULL);
      break;

   case VEH_CALL_OUT_KEY:
/**************************************************/
/* VEH_CALL_OUT_KEY                               */
/*  OP: process the call out request              */
/*  ARG:  none                                    */
/*  PRE: veh exists                               */
/*  POST: fleet stats incremented                 */
/*        driver stats incremented                */
/*        Call_call_out called                    */
/*  ERROR: if !VEH_SIGN_IN                        */
/*              NOT_SIGN_IN msg sent to vehicle   */
/*         if VEH_ACCEPTED_STATE                  */
/*              NO_C_OUT msg sent to vehicle      */
/*         no call                                */
/**************************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_CALL_OUT_KEY\n");
#endif
      if (!(SIGNED_ON(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NOT_SIGN_IN, veh_ptr);
	 return (FAIL);
      }
      if (!(ACCEPTED(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NO_C_OUT, veh_ptr);
	 return (FAIL);
      }
      fleet[veh_ptr->fleet_nbr]->callouts += 1;
      veh_ptr->callouts += 1;
      Call_call_out((CALL_HNDL) veh_ptr->call_ptr);
      break;

   case VEH_CLOSE_KEY:
/*************************************************************/
/* VEH_CLOSE_KEY                                             */
/*  OP: preprocess the close                                 */
/*  ARG: none                                                */
/*  PRE: veh exists                                          */
/*  POST: VEH_FLAG_STATE reset                               */
/*    meter workin drivvel */
/*  ERROR: if !VEH_SIGN_IN_STATE                             */
/*              NOT_SIGN_IN msg sent to vehicle              */
/*         if !VEH_ON_CALL_STATE && VEH_CALL_OFFERED_STATE   */
/*              NO_MUST_PCKP_FIRST msg sent to vehicle       */
/*         if !VEH_ON_CALL_STATE && !VEH_CALL_OFFERED_STATE  */
/*              NO_CLS_NO_PKP msg sent to vehicle            */
/*         if VEH_METER_ON_STATE                             */
/*              NO_CLOSE_ALW msg sent to vehicle             */
/*************************************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_CLOSE_KEY\n");
#endif
      if (!(SIGNED_ON(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NOT_SIGN_IN, veh_ptr);
	 return (FAIL);
      }
      if (!(BUSY(veh_ptr)))
      {
	 if (!OFFERED(veh_ptr))
	    send_text_predefined(veh_ptr->mid, NO_CLS_NO_PKP, veh_ptr);
	 return (FAIL);
      }

      if ( offset->status.load_unload == 0 )
	{
	  send_text_predefined(veh_ptr->mid, NO_CLOSE_ALW, veh_ptr);
	  return (FAIL);
	}
      
      if (veh_ptr->t_status.meter_on)
      {
	 send_text_predefined(veh_ptr->mid, NO_CLOSE_ALW, veh_ptr);
	 return (FAIL);
      }
      if (veh_ptr->call_ptr != NULL)
      {
           return (Veh_reset_state((VEH_HNDL) veh_ptr, VEH_PICKUP_STATE, HNDL_NULL, HNDL_NULL));
      }

      return (Veh_reset_state((VEH_HNDL) veh_ptr, VEH_FLAG_STATE, HNDL_NULL, HNDL_NULL));

   case VEH_CPOST_KEY:
/****************************************************************/
/* VEH_CPOST_KEY                                                */
/*  OP: process the conditional post request                    */
/*  ARG: zone_nbr                                               */
/*       veh_exists                                             */
/*  PRE: veh exists                                             */
/*  POST:                                                       */
/*        if call available in zone                             */
/*           if Veh_check_seals && Veh_check_attr               */
/*              Dispatcher_call_offer called                    */
/*              fleet cpost stats incremented                   */
/*              driver cpost stats incremented                  */
/*        else                                                  */
/*           VEH_CPOST_TIME set                                 */
/*           NO_CALL_AVAILABLE msg sent to vehicle              */
/*  ERROR: if VEH_NOSHOW_STATE                                  */
/*             NO_SHOW_WAIT msg sent to vehicle                 */
/*         if !VEH_SIGN_IN_STATE                                */
/*             NOT_SIGN_IN msg sent to vehicle                  */
/*         if (VEH_SUSPENDED_STATE || DRIVER_SUSPENDED_STATE    */
/*             suspeneded msg sent msg sent to vehicle          */
/*         if VEH_ACCEPTED_STATE                                */
/*             NO_CPOST_ACPT msg sent to vehicle                */
/*         if VEH_CALL_OFFERED_STATE                            */
/*             NO_CPOST_OFF msg sent to vehicle                 */
/*         if VEH_PICKUP_STATE                                  */
/*             NO_CPOST_MTR_ON msg sent to vehicle              */
/*         if cposted to frequently (VEH_CPOST_TIME)            */
/*             NO_CPOST_TOO_FRE msg sent to vehicle             */
/*         if !VEH_POSTED_STATE                                 */
/*             NO_CPOST_NO_POST msg sent to vehicle             */
/*         if no zone_hndl                                      */
/*             INV_ZONE_NO msg sent to vehicle                  */
/*         if Veh_check_seals == FAIL                           */
/*             NO_CPOST_NOT_ALW msg sent to vehicle             */
/*         if ZONE_TYPE == STAND && ZONE_STAND_STATUS == CLOSED */
/*             STAND_CLOSED msg sent to vehicle                 */
/*         no zone_hndl                                         */
/****************************************************************/

      /* includes d_cpost */

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_CPOST_KEY\n");
#endif

      zone_nbr = (short) argument;
      if (veh_ptr->t_status.noshow)
      {
	 send_text_predefined(veh_ptr->mid, NO_SHOW_WAIT, veh_ptr);
	 return (FAIL);
      }
      if (!(SIGNED_ON(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NOT_SIGN_IN, veh_ptr);
	 return (FAIL);
      }

      if (veh_ptr->t_status.on_break)
      {
	 return (FAIL); /* disable CBOOK key if vehicle/driver is on break */
      }

      if (is_suspended(veh_ptr->mid, veh_ptr, EITHER, MSG))
	 return (FAIL);

      if (ACCEPTED(veh_ptr))
      {
	 send_text_predefined(veh_ptr->mid, NO_CPOST_ACPT, veh_ptr);
	 return (FAIL);
      }
      if (OFFERED(veh_ptr))
      {
	 send_text_predefined(veh_ptr->mid, NO_CPOST_OFF, veh_ptr);
	 return (FAIL);
      }
      if (BUSY(veh_ptr))
      {
	 send_text_predefined(veh_ptr->mid, NO_CPOST_MTR_ON, veh_ptr);
	 return (FAIL);
      }
      if (Fleet_get_hndl(BY_NBR, veh_ptr->fleet_nbr, &fleet_hndl) == FAIL)
	 ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", "Fleet nbr is out of boundary");

      if ((mads_time - veh_ptr->c_post_time) < (long) fleet[veh_ptr->fleet_nbr]->cond_posting_freq)
      {
	    send_text_predefined(veh_ptr->mid, NO_CPOST_TOO_FRE, veh_ptr);
	    return (FAIL);
      }
      if (!(POSTED(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NO_CPOST_NO_POST, veh_ptr);
	 return (FAIL);
      }
      if (Zone_get_hndl(zone_nbr, veh_ptr->fleet_nbr, &zone_hndl) == FAIL)
      {
	 send_text_predefined(veh_ptr->mid, INV_ZONE_NO, veh_ptr);
	 return (FAIL);
      }
      if (!Veh_check_seals((VEH_HNDL) veh_ptr, zone_nbr))
      {
	 send_text_predefined(veh_ptr->mid, NO_CPOST_NOT_ALW, veh_ptr);
	 return (FAIL);
      }
#ifdef FOO  //JGW - 022513 - disable shift check for CBOOK/FRI requests
      if (!Vehicle_is_Valid_Shift2((VEH_HNDL)veh_ptr))
	{
	  send_text_predefined(veh_ptr->mid, NO_CPOST_NOT_ALW, veh_ptr);
	  return (FAIL);
	}
#endif

      zone_ptr = fleet[veh_ptr->fleet_nbr]->zone_ptr[zone_nbr];
      if (zone_ptr->type == ZN_STAND)
      {
	 if (zone_ptr->stand_time_from > 0 || zone_ptr->stand_time_to > 0)
	 {
	    if (get_short_time(mads_time) < zone_ptr->stand_time_from ||
		get_short_time(mads_time) > zone_ptr->stand_time_to)
	    {
	       send_text_predefined(veh_ptr->mid, STAND_CLOSED, veh_ptr);
	       return (FAIL);
	    }
	 }
      }
      (void) Zone_get_hndl(zone_nbr, (short) veh_ptr->fleet_nbr, &zone_hndl);
      key_list = (ZONE_CALLS_LIST_HNDL) Zone_get_value(zone_hndl, ZONE_AVAIL_CALLS_LIST);
      Zone_calls_list_each_get(key_list, tmp_call_hndl)
      {
	 call_list_rec = (CALL *)tmp_call_hndl;

	
	 if (Call_get_state(tmp_call_hndl, CALL_AVAILABLE_STATE))
	 {
	   if ( zone_ptr->cbook_time > 0 )
	     timeout = zone_ptr->cbook_time;
	   else
	     timeout = fleet[veh_ptr->fleet_nbr]->cbook_time;
	   
	   //	    timeout = (short)Zone_get_value((ZONE_HNDL)zone_ptr, ZONE_TIMEOUT_TIME);
#ifdef NAMERICA
	    {
#else	    
	    if ( ( (mads_time - (time_t)Call_get_value( tmp_call_hndl, CALL_TIME_ENTERED ) ) >= timeout ) ||
		 (0) )
	      {
#endif		
		/* make sure zones match */
		if ( (short)Call_get_value( tmp_call_hndl, CALL_PICKUP_ZONE ) != zone_nbr )
		  continue;

                // don't offer a trip that has been held for STC
                if ( Call_get_state( tmp_call_hndl, CALL_WAIT_EPOST_STATE ) )
                  continue;

		// Don't mess with Interlink trips
		if ((short)Call_get_value( tmp_call_hndl, CALL_TYPE_ILINK ) == TRUE )
		  continue;
			 
		if (Veh_check_attr(veh_hndl, tmp_call_hndl, TRUE) && Veh_check_seals(veh_hndl, zone_nbr))
		  {
		    
		    if ( (short)Call_get_value( tmp_call_hndl, CALL_FLEET_NBR ) !=
			(short)Veh_get_value( veh_hndl, VEH_FLEET_NBR ) )
		      {
			sprintf(tmp_str, "Veh/Call mismatch-prevented dispatch(cl_nbr %d cl_fleet %d/vh_nbr %d vh_fleet %c)",
				(int)Call_get_value(tmp_call_hndl, CALL_NBR), (char)Call_get_value(tmp_call_hndl, CALL_FLEET_NBR ),
				(int)Veh_get_value(veh_hndl, VEH_NBR), (char)Veh_get_value(veh_hndl, VEH_FLEET));
			ERROR(' ',"",tmp_str);
			return (FAIL);
		      }		    

                    // TSAB gets different trip offer format when originating from CPOST
                    if (0)
                      Dispatcher_cbook_call_offer( veh_hndl, tmp_call_hndl, PRIMARY_TYPE, 1 );
                    else
                      Dispatcher_call_offer(veh_hndl, tmp_call_hndl, PRIMARY_TYPE, 1);
                    
		    fleet[veh_ptr->fleet_nbr]->cond_posts += 1;
		    veh_ptr->cond_posts += 1;
		    return (SUCCESS);
		  }
	      }
	 }
	 } Zone_calls_list_each_end(key_list)

	 veh_ptr->c_post_time = mads_time;
      send_text_predefined(veh_ptr->mid, NO_CALL_AVAIL, veh_ptr);
      return (FAIL);

   case VEH_CREDIT_KEY:
/*******************************************************/
/* VEH_CREDIT_KEY                                      */
/*  OP: process a credit request                       */
/*  ARG:   credit  msg                                 */
/*  PRE: veh_exists                                    */
/*  POST: verify_acct() called                         */
/*  ERROR: iff !VEH_SIGN_IN_STATE                      */
/*            NOT_SIGN_IN msg sent to vehicle          */
/*************************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_CREDIT_KEY\n");
#endif
      ERROR(' ', "", "VEH_CREDIT_KEY not implemented");
      break;

   case VEH_EPOST_KEY:
/****************************************************************/
/* VEH_EPOST_KEY                                                */
/*  OP: preprocesses the epost request                          */
/*  ARG: zone_nbr                                               */
/*  PRE: veh exists                                             */
/*  POST: if zone_nbr == 0 && VEH_EPOSTED_STATE                 */
/*           VEH_EPOSTED_STATE reset                            */
/*        else                                                  */
/*           VEH_EPOSTED_STATE set                              */
/*  ERROR: if VEH_NOSHOW_STATE                                  */
/*            NO_SHOW_WAIT msg sent to vehicle                  */
/*         if !VEH_SIGN_IN_STATE                                */
/*            NOT_SIGN_IN msg sent to vehicle                   */
/*         if VEH_SUSPENDED_STATE || DRIVER_SUSPENDED_STATE     */
/*            suspended msg sent to vehicle                     */
/*         if zone does not exist                               */
/*            INV_ZONE_NO msg sent to vehicle                   */
/*         if !ZONE_EPOST_CONDITION_ALLOWED                     */
/*            NO_EPOST_NOT_ALW msg set to vehicle               */
/*         if !Veh_check_seals                                  */
/*            NO_EPOST_NOT_ALW msg set to vehicle               */
/*         if ZONE_TYPE == STAND && ZONE_STAND_STATUS == CLOSED */
/*             STAND_CLOSED msg sent to vehicle                 */
/*         switch on FLEET_WHEN_ENROUTE_POST                    */
/*            case METER_ON:                                    */
/*              if !VEH_PICKUP_STATE                            */
/*                 NO_EPOST_NOT_MTR_ON msg sent to vehicle      */
/*            case METER_ON & SYSTEM CALL:                      */
/*              if !!VEH_PICKUP_STATE || !VEH_ACCEPTED_STATE    */
/*                 NO_EPOST_SYS_ERR msg sent to vehicle         */
/****************************************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_EPOST_KEY\n");
#endif

      if (Fleet_get_hndl(BY_NBR, veh_ptr->fleet_nbr, &fleet_hndl) == FAIL)
        return(FAIL);
      
      if ( ((char)Fleet_get_value( fleet_hndl, FLEET_ENROUTE_BREAK )) == YES )
        {
          if ( (SIGNED_ON(veh_ptr)) &&
               (!veh_ptr->t_status.meter_on) &&
               (!veh_ptr->t_status.on_break) &&
               (!veh_ptr->t_status.e_posted) &&
               (!veh_ptr->t_status.pckup ) &&
               (!veh_ptr->t_status.accepted ) )
            {
              
              if (is_suspended(veh_ptr->mid, veh_ptr, EITHER, MSG))
                return (FAIL);
              
              strcat(argument, "\0");
              zone_nbr = atoi(argument);
              if (Zone_get_hndl(zone_nbr, veh_ptr->fleet_nbr, &zone_hndl) == FAIL)
                {
                  send_text_predefined(veh_ptr->mid, INV_ZONE_NO, veh_ptr);
                  return (FAIL);
                }	  
              Veh_set_state((VEH_HNDL) veh_ptr, VEH_ON_BREAK_STATE, (HNDL) zone_nbr);
              return (SUCCESS);
            }
        }
      
      if (veh_ptr->t_status.noshow)
      {
	 send_text_predefined(veh_ptr->mid, NO_SHOW_WAIT, veh_ptr);
	 return (FAIL);
      }
      if ((veh_ptr->t_status.on_break))
	{
	  send_text_predefined(veh_ptr->mid, ALREADY_ON_BREAK, veh_ptr);
	  return (FAIL);
	}
      if ((!veh_ptr->t_status.e_posted) && (!veh_ptr->t_status.meter_on) &&
	   (fleet[veh_ptr->fleet_nbr]->approach_tariff == PICKUP_WITH_METER_ON))
	{
	  send_text_predefined(veh_ptr->mid, MUST_PCKP_FIRST, veh_ptr);
	  return (FAIL);
	}
      if (!(SIGNED_ON(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NOT_SIGN_IN, veh_ptr);
	 return (FAIL);
      }
      
      if (veh_ptr->t_status.crse_annul && (0))
	{
	  send_text_predefined(veh_ptr->mid, NO_C_OUT, veh_ptr);
	  return (FAIL);
	}      

      if (is_suspended(veh_ptr->mid, veh_ptr, EITHER, MSG))
	 return (FAIL);



      strcat(argument, "\0");
      zone_nbr = atoi(argument);

      /** attempt to epost successively into same zone */
      if ( ( veh_ptr->t_status.e_posted ) &&
	   ( veh_ptr->epost_zone == zone_nbr ) )
	return(FAIL);
      
      if (zone_nbr == 0)
      {
	 if (veh_ptr->t_status.e_posted)
	    return (Veh_reset_state((VEH_HNDL) veh_ptr, VEH_EPOSTED_STATE, HNDL_NULL, HNDL_NULL));
      }
      
      if (Zone_get_hndl(zone_nbr, veh_ptr->fleet_nbr, &zone_hndl) == FAIL)
      {
	 send_text_predefined(veh_ptr->mid, INV_ZONE_NO, veh_ptr);
	 return (FAIL);
      }
      if (fleet[veh_ptr->fleet_nbr]->zone_ptr[zone_nbr]->enr_post_allow == NO)
      {
	 send_text_predefined(veh_ptr->mid, NO_EPOST_NOT_ALW, veh_ptr);
	 return (FAIL);
      }
      if (!Veh_check_seals((VEH_HNDL) veh_ptr, zone_nbr))
      {
	 send_text_predefined(veh_ptr->mid, NO_PCKUP_NOT_ALW, veh_ptr);
	 return (FAIL);
      }
      zone_ptr = fleet[veh_ptr->fleet_nbr]->zone_ptr[zone_nbr];
      if (zone_ptr->type == ZN_STAND)
      {
	 if (zone_ptr->stand_time_from > 0 || zone_ptr->stand_time_to > 0)
	 {
	    if (get_short_time(mads_time) < zone_ptr->stand_time_from
		|| get_short_time(mads_time) > zone_ptr->stand_time_to)
	    {
	       send_text_predefined(veh_ptr->mid, STAND_CLOSED, veh_ptr);
	       return (FAIL);
	    }
	 }
      }
      switch (fleet[veh_ptr->fleet_nbr]->when_enroute_post)
      {
      case '1':
	 break;
      case '2':
	 if (veh_ptr->t_status.pckup)
	    break;
	 send_text_predefined(veh_ptr->mid, MUST_PCKP_FIRST, veh_ptr);
	 return (FAIL);
      case '3':
	 if (veh_ptr->t_status.pckup && veh_ptr->t_status.accepted)
	    break;
	 send_text_predefined(veh_ptr->mid, NO_EPOST_NOT_ASGN_CALL, veh_ptr);
	 return (FAIL);
      default:
	 ERROR(' ', "", "epost");
	 send_text_predefined(veh_ptr->mid, NO_EPOST_SYS_ERR, veh_ptr);
	 return (FAIL);
      }

      return (Veh_set_state((VEH_HNDL) veh_ptr, VEH_EPOSTED_STATE, (HNDL) zone_nbr));

   case VEH_ETA_KEY:
/*****************************************************/
/* VEH_ETA_KEY                                       */
/*  OP: process the eta message                      */
/*  ARG:   eta time                                  */
/*  PRE: vehicle exists                              */
/*  POST: VEH_ETA_REQUEST made                       */
/*        if VEH_CALL_OFFERED_STATE &&               */
/*           !(VEH_PICKUP_STATE || VEH_ACCEPT_STATE) */
/*               VEH_ACCEPT_STATE set                */
/*        if VEH_CALL_HNDL                           */
/*               Call_eta called                     */
/*  ERROR: if !VEH_SIGN_IN_STATE                     */
/*            NOT_SIGN_IN msg sent to vehicle        */
/*****************************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_ETA_KEY\n");
#endif
      if (!(SIGNED_ON(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NOT_SIGN_IN, veh_ptr);
	 return (FAIL);
      }

      if ( veh_ptr->call_ptr != NULL )
	mk_except((struct calls *) veh_ptr->call_ptr, veh_ptr, ETA, (long) argument, -1);
      else
	mk_except((struct calls *) NULL, veh_ptr, ETA, (long) argument, -1);

      if (veh_ptr->t_status.offered && !(veh_ptr->t_status.pckup || veh_ptr->t_status.accepted))
      {				/* vehicle responding to a offered call */
	 Veh_set_state((VEH_HNDL) veh_ptr, VEH_ACCEPT_STATE, (HNDL) veh_ptr->call_ptr);
      }
      if (veh_ptr->call_nbr > NULL)
	 Call_eta((CALL_HNDL) veh_ptr->call_ptr, (VEH_HNDL) veh_ptr, (int) argument);
      break;

   case VEH_FLAG_REQUEST_KEY:
/*******************************************************/
/* VEH_FLAG_REQUEST_KEY                                */
/*  OP: preprocess the flag request key                */
/*  ARG: none                                          */
/*  PRE: vehicle exists                                */
/*  POST:                                              */
/*       if VEH_IDLE_STATE && !VEH_CALL_OFFERED_STATE  */
/*            fleet stats incremented                  */
/*            driver stats incremented                 */
/*            OK_FLAG sent to vehicle                  */
/*       else                                          */
/*         if ZONE_TYPE == STAND &&                    */
/*            FLEET_AUTO_FLAG_ON_STANDS == SEMI        */
/*              VEH_FLAG_REQUEST_STATE set             */
/*         if ZONE_TYPE == STAND &&                    */
/*            FLEET_AUTO_                              */
/*              if CALL_FLAG_REQUEST set == SUCCESS    */
/*                  fleet stats incremented            */
/*                  driver stats incremented           */
/*                  OK_FLAG sent to vehicle            */
/*                  VEH_FLAG_REQUEST_STATE set         */
/*         if ZONE_TYPE != STAND                       */
/*            VEH_FLAG_REQUEST_STATE set               */
/*  ERROR: if !VEH_SIGN_IN_STATE                       */
/*              NOT_SIGN_IN msg sent to vehicle        */
/*         if VEH_PICKUP_STATUS                        */
/*              NO_FLAG msg sent to vehicle            */
/*         if ZONE_TYPE == STAND &&                    */
/*            FLEET_AUTO_FLAG_ON_STANDS == NONE        */
/*              NO_FLAG msg sent to vehicle            */
/*         if ZONE_TYPE == STAND &&                    */
/*            FLEET_AUTO_                              */
/*              if CALL_FLAG_REQUEST set == FAIL       */
/*                   NO_FLAG msg sent to vehicle       */
/*         zone does not exist                         */
/*******************************************************/

      if (!(SIGNED_ON(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NOT_SIGN_IN, veh_ptr);
	 return (FAIL);
      }
      if (veh_ptr->t_status.pckup)
      {
	 send_text_predefined(veh_ptr->mid, NO_FLAG, veh_ptr);
	 return (FAIL);
      }
      
      if ( (OFFERED(veh_ptr)) && (!ACCEPTED(veh_ptr)) )
	{
	  send_text_predefined(veh_ptr->mid, CALL_OFFERED_ACPT);
	  return (FAIL);
	}
	  
      if (veh_ptr->t_status.idle && !veh_ptr->t_status.offered)
      {
	 ++fleet[veh_ptr->fleet_nbr]->flags;
	 ++veh_ptr->flags;
	 send_text_predefined(veh_ptr->mid, OK_FLAG, veh_ptr);
	 return (SUCCESS);
      } else
      {

	 if (veh_ptr->zone_num != 0)
	    (void) Zone_get_hndl(veh_ptr->zone_num, veh_ptr->fleet_nbr, &zone_hndl);
	 else
	 {
	    if (veh_ptr->call_ptr != NULL)
	       (void) Zone_get_hndl(veh_ptr->call_ptr->pickup_zone, veh_ptr->fleet_nbr, &zone_hndl);
	    else
	    {
	       ERROR(' ', "", "vehicle with no zone");
	       zone_hndl = HNDL_NULL;
	    }
	 }
	 if (zone_hndl != NULL)
	       switch ((char)Fleet_get_value( (FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_FLAG_ACTION ) )
	       {
	       case '1':
		  send_text_predefined(veh_ptr->mid, NO_FLAG, veh_ptr);
		  return (FAIL);
	       case '2':
		  Veh_set_state((VEH_HNDL) veh_ptr, VEH_FLAG_REQUEST_STATE, HNDL_NULL);
		  break;
		  
		  /* automatically approve if
		     - there is an available vehicle in the zone
		     - time criteria is met
		  */
	       case '3':
		  Zone_veh_list_each_get( Zone_get_value( zone_hndl, ZONE_AVAIL_VEH_LIST ), tmp_veh_hndl )
		    {
		      if ( !Veh_get_state( tmp_veh_hndl, VEH_ON_BREAK_STATE ) )
			{
			  veh_found_for_flag = TRUE;
			  break;
			}
		    } Zone_veh_list_each_end(Zone_get_value(zone_hndl,ZONE_AVAIL_VEH_LIST))

		  if ( (short) Call_get_value( (CALL_HNDL)veh_ptr->call_ptr, CALL_TYPE_TIME_CALL ) != TRUE )
		    tmp_time = (time_t) Call_get_value( (CALL_HNDL)veh_ptr->call_ptr, CALL_TIME_DUE );
		  else
		    tmp_time = (time_t) Call_get_value( (CALL_HNDL)veh_ptr->call_ptr, CALL_PICKUP_LEAD_TIME );
		  
		  if ( ( veh_found_for_flag == TRUE ) &&
		       (tmp_time >
		       ( mads_time - (int)Fleet_get_value( (FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_AUTO_FLAG_TIME ) ) ) )
		    {
		      ++fleet[veh_ptr->fleet_nbr]->now_flags;
		      veh_ptr->t_status.flag = 1;
		      Call_add_history( (CALL_HNDL)veh_ptr->call_ptr, (VEH_HNDL)veh_ptr, FLAG_REQ, TAKEN, 0L, 0 );
		      Veh_reset_state((VEH_HNDL) veh_ptr, VEH_FLAG_REQUEST_STATE, (HNDL)R_APPROVE, HNDL_NULL);
		    } 
		  else
		    {
		      send_text_predefined(veh_ptr->mid, NO_FLAG, veh_ptr);
		      Call_add_history( (CALL_HNDL)veh_ptr->call_ptr, (VEH_HNDL)veh_ptr, FLAG_REQ, 33, 0L, 0 );
		      return (FAIL);
		    }
		  break;
		}
       }
      return (SUCCESS);

   case VEH_ITM_KEY:
/************************************/
/* VEH_ITM_KEY                      */
/*  OP: process an itm message      */
/*  ARG: msg                        */
/*  PRE: veh exists                 */
/*  POST: itm msg processng invoked */
/*  ERROR: tbd                      */
/************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_ITM_KEY\n");
#endif
      if (debug_state)
	 ERROR(' ', "", "VEH_ITM_KEY not implemented");
      break;

   case VEH_METER_FAULT_KEY:
/**************************************/
/* VEH_METER_FAULT_KEY                */
/*  OP: process a meter fault message */
/*  ARG: none                         */
/*  PRE: veh exists                   */
/*       call exists                  */
/*  POST: VEH_MFAULT_REUEST made      */
/*        call history updated        */
/*  ERROR: none                       */
/**************************************/

      if (veh_ptr->call_ptr != NULL)
	{
	  mk_except(veh_ptr->call_ptr, veh_ptr, MFAULT, 0, -1);
	  Call_add_history((CALL_HNDL)veh_ptr->call_ptr, veh_hndl, MFAULT, 0, 0L, 0);
	}

      break;
      
   case VEH_METER_OFF_KEY:
/****************************************************************/
/* VEH_METER_OFF_KEY                                            */
/*  OP: preprocessing for meter_off                             */
/*  ARG: none                                                   */
/*  PRE: veh exists                                             */
/*       VEH_METER_ON_STATE                                     */
/*  POST:                                                       */
/*        if !VEH_SIGNED_IN_STATE                               */
/*           VEH_METER_ON_STATE reset                           */
/*        else                                                  */
/*          if VEH_PICKUP_STATE                                 */
/*             if FLEET_APPROACH_TARIFF == PICKUP_WITH_METER_ON */
/*                if VEH_ACCEPTED_STATE                         */
/*                   VEH_PICKUP_STATE set                       */
/*                 else if !VEH_ON_CALL_STATE                   */
/*                         VEH_METER_ON_STATE reset             */
/*          else                                                */
/*              if !VEH_ON_CALL_STATE                           */
/*                 VEH_PICKUP_STATE reset                       */
/*                 return                                       */
/*          VEH_FLAG_STATE reset                                */
/*  ERROR: none                                                 */
/****************************************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_METER_OFF_KEY\n");
#endif
      if (!(SIGNED_ON(veh_ptr)))/* vehicle is not signed on */
      {
	 Veh_reset_state((VEH_HNDL) veh_ptr, VEH_METER_ON_STATE, HNDL_NULL, HNDL_NULL);
	 return (FAIL);
      }
      if (veh_ptr->t_status.meter_on == 0)	/* DONT THINK IT SHOULD BE ON */
      {
	 if (veh_ptr->call_ptr == NULL)
	   {
	     if (POSTED(veh_ptr))
	       Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL);
	     return (SUCCESS);
	   }
	 if (veh_ptr->t_status.offered)
	   return (FAIL);
      }
      if (!veh_ptr->t_status.pckup)
      {
	 if (fleet[veh_ptr->fleet_nbr]->approach_tariff == PICKUP_WITH_METER_ON)
	 {
	    if (veh_ptr->t_status.accepted)
	       Veh_set_state((VEH_HNDL) veh_ptr, VEH_PICKUP_STATE, HNDL_NULL);
	    else if (veh_ptr->call_ptr == NULL)	/* meter on and shouldnt be */
	    {
	       Veh_reset_state((VEH_HNDL) veh_ptr, VEH_METER_ON_STATE, HNDL_NULL, HNDL_NULL);
	       return (SUCCESS);
	    }
	 }

      } else if (veh_ptr->call_ptr == NULL)
	 return (Veh_reset_state((VEH_HNDL) veh_ptr, VEH_METER_ON_STATE, HNDL_NULL, HNDL_NULL));
      return (Veh_reset_state((VEH_HNDL) veh_ptr, VEH_METER_ON_STATE, HNDL_NULL, HNDL_NULL));

#ifndef EXT_VEH_METER_ON_KEY
   case VEH_METER_ON_KEY:
/************************************************************************/
/* VEH_METER_ON_KEY                                                     */
/*  OP: pre processing for meter on                                     */
/*  ARG: none                                                           */
/*  PRE: veh exists                                                     */
/*  POST:  VEH_METER_ON_STATE set                                       */
/*        if VEH_SIGNED_IN_STATE                                        */
/*          if FLEET_APPROACH_TARRIFF == NO_APPROACH_TARRIFF            */
/*             VEHPICKUP_STATE set                                      */
/*          if FLEET_APPROACH_TARRIFF == PICKUP_WITH_METER_ON &&        */
/*             VEH_ACCEPTED_STATE                                       */
/*               return                                                 */
/*           if VEH_OFFERED_STATE &&                                    */
/*              FLEET_HANDLE_SYSTEM_ASSIGNED_CALLS == IMPLIED_REJECT    */
/*                VEH_POSTED_STATE reset                                */
/*                if VEH_ON_CALL_STATE                                  */
/*                   Veh_process_key (REJECT) called                    */
/*                else                                                  */
/*                   !VEH_OFFERED_STATE                                 */
/*                VEH_FLAG_STATE set                                    */
/*  ERROR: if VEH_OFFER_STATE &&                                        */
/*            FLEET_HANDLE_SYSTEM_ASSIGNED_CALLS == FORCE_STYSTEM_CALLS */
/*              CALL_OFFERED_ACPT msg sent to vehicle                   */
/************************************************************************/

      Veh_set_state((VEH_HNDL) veh_ptr, VEH_METER_ON_STATE, HNDL_NULL);

      if ( veh_ptr->t_status.crse_annul && (0) )
	return (SUCCESS);
      
      if (!(SIGNED_ON(veh_ptr)))
	 return (FAIL);

      if (veh_ptr->call_ptr == NULL)
	 return (Veh_set_state((VEH_HNDL) veh_ptr, VEH_FLAG_STATE, HNDL_NULL));

      if ((fleet[veh_ptr->fleet_nbr]->approach_tariff == PICKUP_WITH_METER_ON) &&
	  (veh_ptr->t_status.accepted))	/* normal */
	 return (SUCCESS);

      /* NOTE COMMON CODE WITH PICKUP_KEY - DO SOMETHING */
      if (veh_ptr->t_status.offered && !veh_ptr->t_status.accepted)
	 switch (fleet[veh_ptr->fleet_nbr]->handle_system_assigned_calls)
	 {
	 case FORCE_SYSTEM_CALLS:
	    send_text_predefined(veh_ptr->mid, CALL_OFFERED_ACPT, veh_ptr);
	    return (FAIL);

	 case IMPLIED_REJECT:
	 default:
	    Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL);
	    if (veh_ptr->call_ptr != NULL)
	       Veh_process_key((VEH_HNDL) veh_ptr, VEH_REJECT_KEY, (HNDL) PENALTY_NO, HNDL_NULL);
	    else
	       veh_ptr->t_status.offered = 0;
	    Veh_set_state((VEH_HNDL) veh_ptr, VEH_PICKUP_STATE, HNDL_NULL);
	    return (Veh_set_state((VEH_HNDL) veh_ptr, VEH_FLAG_STATE, HNDL_NULL));
	 }			/* end case */
      if (fleet[veh_ptr->fleet_nbr]->approach_tariff == NO_APPROACH_TARIFF)
	 return (Veh_set_state((VEH_HNDL) veh_ptr, VEH_PICKUP_STATE, HNDL_NULL));

      ERROR(' ', "", "Unresolved meter on condition");
      return (FAIL);
#endif

   case VEH_MSG_KEY:
/*************************************************/
/* VEH_MSG_KEY                                   */
/*  OP: preprocess a vehicle msg to dispatcher   */
/*  ARG: veh_hndl                                */
/*       msg                                     */
/*  PRE: veh exists                              */
/*  POST: VEH_MSG_STAT set                       */
/*  ERROR: if !VEH_SIGNED_IN_STATE               */
/*              NOT_SIGN_IN msg sent to vehicle  */
/*************************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_MSG_KEY\n");
#endif
      if (!(SIGNED_ON(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NOT_SIGN_IN, veh_ptr);
	 return (FAIL);
      }

      if (0)
        {
          (void)Veh_get_hndl(BY_NBR,(int)argument2,(short)Veh_get_value((VEH_HNDL)veh_ptr, VEH_FLEET_NBR),(VEH_HNDL *)&to_veh_ptr);
          if ( to_veh_ptr == NULL )
            {
              sprintf( error_str, catgets(DISPATCH_catd, 1, 46, "VEHICLE NUMBER %d NOT VALID"), (int)argument2 );
              send_text( veh_ptr->mid, error_str, veh_ptr, PRIORITY_2 );
              return (FAIL);
            }
          
          if ( !SIGNED_ON(to_veh_ptr) )
            {
              sprintf( error_str, catgets(DISPATCH_catd, 1, 47, "VEHICLE %d NOT SIGNED IN"), (int)argument2 );
              send_text( veh_ptr->mid, error_str, veh_ptr, PRIORITY_2 );
              return( FAIL );
            }
          
          bzero( &veh_cmsg, sizeof( VEH_CANMSG_REC ) );
          veh_cmsg.nbr = (int)argument;
          if ( db_read_key( VEH_CMSG_FILE_ID, &veh_cmsg, &veh_cm_key1, DB_EQUAL ) != SUCCESS )
            {
              sprintf( error_str, catgets(DISPATCH_catd, 1, 48, "INVALID MESSAGE NUMBER %d"), (int)argument );
              send_text( veh_ptr->mid, error_str, veh_ptr, PRIORITY_2 );
              return( FAIL );
            }
          
          time_sent = time((time_t)0);
          get_srch_date( time_sent, msg_date );
          get_srch_time( time_sent, msg_time );
          sprintf( to_veh_msg, "%s%c%s%c %d%s%c %d %s%s%s%s %s",
                   FLEET_HDR,veh_ptr->fleet,FROM_HDR, MSG_VEHICLE, veh_ptr->veh_nbr, TO_HDR,
                   MSG_VEHICLE, to_veh_ptr->veh_nbr, CR,
                   veh_cmsg.text, CR, msg_date, msg_time );
          send_text( to_veh_ptr->mid, to_veh_msg, to_veh_ptr, PRIORITY_2 );
          return( SUCCESS );
        }

      if ( ( (int)argument == 97 ) )
	{
	  // Arrive at stop
	  // timestamp arrival
	  if (veh_ptr->call_ptr != NULL && veh_ptr->kela_node > 0)
	    {
	      db_open(KELANODE_FILE_ID, ISINOUT | ISMANULOCK);
	      bzero(&kelanode_rec, sizeof(KELANODE_REC));
	      kelanode_rec.tpak_id = veh_ptr->call_ptr->call_number;
	      kelanode_rec.nbr = veh_ptr->kela_node;
	      kelanode_rec.version = 0;
	      if ( db_read_key(KELANODE_FILE_ID, &kelanode_rec, &kelanode_key3, ISEQUAL) == SUCCESS )
		{
		  tmtime = time((time_t)0);
		  stime = (char *)ctime(&tmtime);
		  sprintf(kelanode_rec.arrival_time, "%.8s", &stime[11]);
		  db_update(KELANODE_FILE_ID, &kelanode_rec);
		}
	    }

	  
	  
	}
      if (( (int)argument == 98 ) )
	{
	  // Depart from stop
	  // timestamp departure
	  // send details for next stop - address
	}
      if ( ( (int)argument == 99 ) )
        {
          switch ( (int)argument2 )
            {
            case 10:   // TRIP DETAILS
              tmp_buf[0] = '\0';
              mk_outb_text("");
              if ( veh_ptr->call_ptr == NULL ) // NO TRIP ASSIGNED
                {
                  send_text_predefined(veh_ptr->mid, NO_CALL_AVAIL, veh_ptr);
                  return( SUCCESS );
                }
              else
                {
                  if ( !veh_ptr->t_status.accepted ) // TRIP NOT ACCEPTED
                    {
                      send_text_predefined(veh_ptr->mid, NO_ACPT_NO_OFFR, veh_ptr);
                      return( SUCCESS );
                    }
                  else
                    {
                      send_assign( veh_ptr->mid, veh_ptr->call_ptr, veh_ptr, FALSE );
                      return( SUCCESS );
                    }                  
                }
              break;

            case 20:  // SHOW ATTRIBUTES
              tmp_buf[0] = '\0';
              mk_outb_text("");
              strcpy( tmp_buf, MDT_VEH_ATTR );
              for ( i = 0; i < ATTR_MAX; i++ )
                {
                  if ( test_bit( i, sizeof( veh_ptr->vehicle_attr ), &veh_ptr->vehicle_attr ) )
                    {
                      if ( strlen(fleet[veh_ptr->fleet_nbr]->vd_attr_info[i].veh_attr) < 2 )
                        {
                          strcat( tmp_buf, " " );
                          strcat( tmp_buf, fleet[veh_ptr->fleet_nbr]->vd_attr_info[i].veh_attr );
                        }
                      else
                        strcat( tmp_buf, fleet[veh_ptr->fleet_nbr]->vd_attr_info[i].veh_attr );
                      strcat( tmp_buf, " " );
                    }
                }
              mk_outb_text( tmp_buf );
              add_outb_text( CR );
              tmp_buf[0] = '\0';
              strcpy( tmp_buf, MDT_DRV_ATTR );
              for ( i = 0; i < ATTR_MAX; i++ )
                {
                  if ( test_bit( i, sizeof( veh_ptr->driver_attr ), &veh_ptr->driver_attr ) )
                    {
                      if ( strlen(fleet[veh_ptr->fleet_nbr]->vd_attr_info[i].drv_attr) < 2 )
                        {
                          strcat( tmp_buf, " " );
                          strcat( tmp_buf, fleet[veh_ptr->fleet_nbr]->vd_attr_info[i].drv_attr );
                        }
                      else
                        strcat( tmp_buf, fleet[veh_ptr->fleet_nbr]->vd_attr_info[i].drv_attr );
                      strcat( tmp_buf, " " );
                    }
                }
              add_outb_text( tmp_buf );
              send_msg_mmp(veh_ptr->mid, TEXT_DISPLAY, veh_ptr);
              break;

            case 30:   // Show GPS zones
              Veh_send_gps_zones( veh_ptr );
              break;
              
            case 600:  // BIG_CAR_TRIP_LIST
             tmp_buf[0] = '\0';
             mk_outb_text("");
             msg_to_send = FALSE;
             lines_sent = 0;
             first_msg = TRUE;
             Fleet_big_car_trip_list_each_get((FLEET_BIG_CAR_TRIP_LIST_HNDL)fleet[veh_ptr->fleet_nbr]->big_car_trip_list_hndl, tmp_call_hndl)
               {

                 // don't send ACCEPTED trips or trips older than 2 hours..
                 if ( ( Call_get_state( tmp_call_hndl, CALL_ACCEPT_STATE ) )||
                      ( mads_time - (time_t)Call_get_value( tmp_call_hndl, CALL_TIME_DUE ) > 7200 ) )
                   continue;

                 // don't send if zones are mismatched
                 if ( (short)( Call_get_value( tmp_call_hndl, CALL_PICKUP_ZONE ) ) !=
                      (short)( Veh_get_value( (VEH_HNDL)veh_ptr, VEH_ZONE_NBR ) ) )
                   continue;

                 // don't send if the trip doesn't have big trip attributes
                 call_ptr = (CALL *)tmp_call_hndl;
                 if ( ( !call_ptr->vehicle_attributes.attr3 ) && ( !call_ptr->vehicle_attributes.attr4 ) &&
                      ( !call_ptr->vehicle_attributes.attr5) )
                   continue;
                 
                 // only send info if all attributes match                 
                 if ( Veh_check_attr((VEH_HNDL)veh_ptr, tmp_call_hndl, FALSE ) )
                   {
                     get_srch_time( (time_t)Call_get_value(tmp_call_hndl, CALL_TIME_DUE), DueTime );
                     sprintf(tmp_buf, "ZON %d %5s ", (int)Call_get_value(tmp_call_hndl, CALL_PICKUP_ZONE),
                             DueTime );
                     call_ptr = (CALL *)tmp_call_hndl;
                     addr = (unsigned char *) &call_ptr->vehicle_attributes;
                     if ( *addr )
                       {
                         for ( i = 0; i < 4; i++ )
                           {
                             mask = 0x01;
                             for ( jj = 0; jj <= 8; jj++ )
                               {
                                 if ( ( *addr & mask ) && ( strlen(tmp_buf) < 29 ) )
                                   {
                                     strcat( tmp_buf, " " );                                     
                                     strcat( tmp_buf, fleet[veh_ptr->fleet_nbr]->vd_attr_info[jj+(i*8)].veh_attr );
                                   }
                                 mask <<= 1;
                               }
                             ++addr;
                           }
                       }
                     msg_to_send = TRUE;
                     add_outb_text( tmp_buf );
                     add_outb_text( CR );
                     ++lines_sent;
                     if ( lines_sent == 10 )
                       {
                         lines_sent = 0;
                         if ( first_msg == TRUE )
                           {
                             send_msg_mmp(veh_ptr->mid, TEXT_DISPLAY, veh_ptr);
                             first_msg = FALSE;
                           }
                         else
                           send_msg_mmp(veh_ptr->mid, ASSIGN_DISPLAY_P1, veh_ptr);
                         mk_outb_text("");
                       }
                   }

               } Fleet_big_car_trip_list_each_end((FLEET_BIG_CAR_TRIP_LIST_HNDL)fleet[veh_ptr->fleet_nbr]->big_car_trip_list_hndl)
                   
             if ( msg_to_send )
               {
                 if ( first_msg == TRUE )
                   send_msg_mmp(veh_ptr->mid, TEXT_DISPLAY, veh_ptr);
                 else
                   send_msg_mmp(veh_ptr->mid, ASSIGN_DISPLAY_P1, veh_ptr);
               }
             else
               send_text_predefined(veh_ptr->mid, NO_CALL_AVAIL, veh_ptr);             

                    
                    return( SUCCESS );
              break;

            default:
              break;
            }
        }


      break;

   case VEH_NAK_KEY:
/*********************************************************/
/* VEH_NAK_KEY                                           */
/*  OP: preprocess a nak msg                             */
/*  ARG: MMP nak msg                                     */
/*  PRE: none                                            */
/*  POST:                                                */
/*        if msg->pkt_id == 0                            */
/*             nak_error called                          */
/*        else                                           */
/*           reset_pkt_entry called                      */
/*           mk_nak_error called                         */
/*           if MSG_PKT_TYPE                             */
/*                get_undev() called                     */
/*           if ASSIGN_PKT_TYPE && veh exists            */
/*                VEH_ASSIGN_ERROR_REQUEST made          */
/*           if OFFER_PKT_TYPE                           */
/*               if VEH_CALL_HNDL == call_hndl from pkt  */
/*                   CALL_OFFER_STATE reset              */
/*  ERROR: if OFFER_PKT_TYPE && call_nbr != VEH_CALL_NBR */
/*            ERROR msg recorded                         */
/*********************************************************/
      if (debug_state)
	 printf("VEH_NAK_KEY not implemented\n");
      break;

   case VEH_NOSHOW_KEY:
/**************************************************************/
/* VEH_NOSHOW_KEY                                             */
/*  OP: noshow key processing for call                        */
/*  ARG: msg (not supported)                                  */
/*  PRE: vehicle exists                                       */
/*       call exists                                          */
/*  POST: VEH_NOSHOR_STATE set                                */
/*  ERROR: if !VEH_SIGN_ON_STATE                              */
/*               NOT_SIGN_IN msg sent to vehicle              */
/*         if !VEH_ACCEPTED_STATE                             */
/*               NO_NSHW_NO_ACPT msg sent to vehicle          */
/*         if FLEET_APPROACH_TARRIFF == PICKUP_WITH_METER_ON  */
/*               NO_EPOST_SYS_ERR sent to vehicle             */
/*         if VEH_ON_CALL_STATE                               */
/*                send NO_NSHW_MTR_ON msg to vehicle          */
/**************************************************************/

      /* WAS k_noshow & validate_no_show */

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_NOSHOW_KEY\n");
#endif
      if (!(SIGNED_ON(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NOT_SIGN_IN, veh_ptr);
	 return (FAIL);
      }
#ifdef FOO
      if ((!veh_ptr->t_status.meter_on) && (ACCEPTED(veh_ptr)))
	{
	  send_text_predefined(veh_ptr->mid, NO_BRK_ACPT, veh_ptr);
	  return (FAIL);
	}
#endif
      if (!(ACCEPTED(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NO_NSHW_NO_ACPT, veh_ptr);
	 return (FAIL);
      }
      
      if ( (int) argument2 != ITM_NO_SHOW )
	{
	  if (BUSY(veh_ptr) &&
              (fleet[veh_ptr->fleet_nbr]->approach_tariff != PICKUP_WITH_METER_ON) &&
              (!(0) ) )
	    {
	      send_text_predefined(veh_ptr->mid, NO_NSHW_MTR_ON, veh_ptr);
	      return (FAIL);
	    }
	}
      
      if ((fleet[veh_ptr->fleet_nbr]->approach_tariff == PICKUP_WITH_METER_ON) &&
	  veh_ptr->t_status.late_meter)
      {
	 send_text_predefined(veh_ptr->mid, NO_EPOST_SYS_ERR, veh_ptr);
	 return (FAIL);
      }

      if ( (int)argument2 == ITM_NO_SHOW )
	return (Veh_set_state((VEH_HNDL) veh_ptr, VEH_NOSHOW_STATE, (HNDL)3000));
      else
	return (Veh_set_state((VEH_HNDL) veh_ptr, VEH_NOSHOW_STATE, (HNDL)argument));

   case VEH_PICKUP_KEY:	/* was k_pickup, validate_pickup */
/************************************************************************/
/* VEH_PICKUP_KEY                                                       */
/*  OP: pickup key processing  indicating one of following:             */
/*           picking up customer for system assigned call               */
/*           picking up flag trip                                       */
/*  ARG: none                                                           */
/*  PRE: vehicle exists                                                 */
/*  POST:                                                               */
/*        if FLEET_APPROACH_TARRIFF != PICKUP_WITH_METER_ON             */
/*             VEH_PICKUP_STATE set                                     */
/*        else                                                          */
/*           if VEH_METER_ON_STATE                                      */
/*              VEH_PICKUP_STATE set                                    */
/*              return                                                  */
/*           if VEH_CALL_OFFERED_STATE && VEH_ACCEPTED_STATE            */
/*              VEH_PICKUP_STATE set                                    */
/*              return                                                  */
/*            if VEH_CALL_OFFERED_STATE &&                              */
/*               FLEET_HANDLE_SYSTEM_ASSIGNED_CALLS == IMPLIED_REJECT   */
/*                 VEH_RESET_POSTED_STATE reset                         */
/*                 if VEH_ON_CALL_STATE                                 */
/*                    VEH_REJECT_KEY processed                          */
/*                 else                                                 */
/*                    !VEH_CALL_OFFERED_STATE                           */
/*                 VEH_FLAG_STATE set                                   */
/*  ERROR: if !VEH_SIGN_IN_STATE                                        */
/*                NOT_SIGN_IN msg sent to vehicle                       */
/*         if VEH_PICKUP_STATE                                          */
/*                 NO_ACPT_ACPT msg sent to vehicle                     */
/*         if !VEH_OK_TO_PICKUP_STATE                                   */
/*                NO_PCKUP_ALW msg sent to vehicle                      */
/*         if VEH_ACCEPTED_STATE && !VEH_METER_ON_STATE                 */
/*                NO_CLOSE_ALW msg sent to vehicle                      */
/*         if VEH_CALL_OFFERED_STATE &&                                 */
/*            FLEET_HANDLE_SYSTEM_ASSIGNED_CALLS == FORCE_STYSTEM_CALLS */
/*              CALL_OFFERED_ACPT msg sent to vehicle                   */
/************************************************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_PICKUP_KEY\n");
#endif
      /* WAS handle_a_call_offer, k_pickup validate_pickup */
      if ( offset->status.load_unload == 0 )
	{
	  mk_outb_text("INTE TILLÅTET");
	  send_msg_mmp( veh_ptr->mid, TEXT_DISPLAY, veh_ptr );	  
	}
      
      if (!(SIGNED_ON(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NOT_SIGN_IN, veh_ptr);
	 return (FAIL);
      }

      if (veh_ptr->t_status.pckup)
      {
	 /* Send "ALREADY ON TRIP" */
	 send_text_predefined(veh_ptr->mid, NO_ACPT_ACPT, veh_ptr);
	 return (FAIL);
      }

      if (veh_ptr->t_status.crse_annul && (0) )
	{
	  send_text_predefined(veh_ptr->mid, NO_C_OUT, veh_ptr);
	  return (FAIL);
	}
      
#ifdef FOO
      if (veh_ptr->t_status.accepted && !veh_ptr->t_status.meter_on)
	{
	  send_text_predefined(veh_ptr->mid, NO_EPOST_SYS_ERR, veh_ptr);
	  return (FAIL);	/* Can't load until meter is on */
	}
      if (veh_ptr->t_status.offered && !veh_ptr->t_status.meter_on)
	{
	  send_text_predefined(veh_ptr->mid, NO_EPOST_SYS_ERR, veh_ptr);
	  return (FAIL);
	}
#endif

#ifdef NAMERICA
      if (is_pckup_allowed( veh_ptr ) == FALSE )
	{
	  send_text_predefined( veh_ptr->mid, NO_PCKUP_ALW, veh_ptr );
	  return;
	}
#endif

      if (veh_ptr->call_ptr == NULL)
	{
	  Veh_set_state((VEH_HNDL) veh_ptr, VEH_FLAG_STATE, HNDL_NULL );	  
	  return (FAIL);
	}
      
      switch (fleet[veh_ptr->fleet_nbr]->approach_tariff)
      {
      case NO_APPROACH_TARIFF:
      default:
	 return (Veh_set_state((VEH_HNDL) veh_ptr, VEH_PICKUP_STATE, HNDL_NULL));

      case PICKUP_WITH_METER_ON:
	 if (veh_ptr->t_status.meter_on)
	    return (Veh_set_state((VEH_HNDL) veh_ptr, VEH_PICKUP_STATE, HNDL_NULL));
	 if (veh_ptr->t_status.offered && veh_ptr->t_status.accepted)
	    return (Veh_set_state((VEH_HNDL) veh_ptr, VEH_PICKUP_STATE, HNDL_NULL));

	 if (veh_ptr->t_status.offered && !veh_ptr->t_status.accepted)
	    switch (fleet[veh_ptr->fleet_nbr]->handle_system_assigned_calls)
	    {
	    case FORCE_SYSTEM_CALLS:
	       send_text_predefined(veh_ptr->mid, CALL_OFFERED_ACPT, veh_ptr);
	       return (FAIL);

	    case IMPLIED_REJECT:
	    default:
	       Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL);
	       if (veh_ptr->call_ptr != NULL)
		  Veh_process_key((VEH_HNDL) veh_ptr, VEH_REJECT_KEY, (HNDL) PENALTY_NO, HNDL_NULL);
	       else
		  veh_ptr->t_status.offered = 0;

	       return (Veh_set_state((VEH_HNDL) veh_ptr, VEH_FLAG_STATE, HNDL_NULL));
	    }			/* end case */
	 if (debug_state)
	    printf("Unresolved pickup condition \n");
	 ERROR(' ', "", "Unresolved pickup condition");
	 return (FAIL);
      }				/* end case */

   case VEH_POST_KEY:
/*******************************************************************/
/* VEH_POST_KEY                                                    */
/*  OP: preprocess post key request                                */
/*  ARG: zone_nbr                                                  */
/*  PRE: vehicle exists                                            */
/*  POST:                                                          */
/*       if VEH_POSTED_STATE                                       */
/*          if ARGUMENT == VEH_ZONE_NBR                            */
/*             if VEH_ON_BERAK_STATE reset == FAIL                 */
/*                return(FAIL)                                     */
/*             if (Dispatcher_call_match == FAIL                   */
/*                q_position msg set to vehicle                    */
/*             return                                              */
/*       if  VEH_ON_BREAK_STATE                                    */
/*           if VEH_ON_BERAK_STATE reset == FAIL                   */
/*                return(FAIL)                                     */
/*       if VEH_EPOSTED_STATE                                      */
/*           VEH_EPOSTED_STATE reset                               */
/*       else                                                      */
/*           VEH_POSTED_STATE reset                                */
/*  ERROR: if !VEH_SIGNED_IN_STATE                                 */
/*                NOT_SIGN_IN msg sent to vehicle                  */
/*         if VEH_SUSPENDED_STATE or DRIVER_SUSPENDED_STATE        */
/*                suspension msg sent to vehicle                   */
/*         if VEH_METER_ON_STATE                                   */
/*                NO_POST_MTR_ON msg sent to vehicle               */
/*         if VEH_PICKUP_STATE                                     */
/*                NO_POST_MTR_ON msg sent to vehicle               */
/*         if VEH_ACCEPTED_STATE                                   */
/*                NO_POST_ACPT msg sent to vehicle                 */
/*         if VEH_CALL_OFFERED_STATE                               */
/*                NO_POST_OFF msg sent to to vehicle               */
/*         if VEH_POSTED_STATE && posting too frequent             */
/*                NO_POST_TOO_FRE msg sent to vehicle              */
/*         if VEH_ZONE_NBR == ZONE_NBR && !VEH_ON_BREAK_STATE      */
/*                ALREADY_POST  msg sent to vehicle                */
/*         if ZONE_GET_HNDL == HNDL_ERR                            */
/*                INV_ZONE_NO  msg sent to vehicle                 */
/*         if Veh_check_seal == FAIL                               */
/*                NO_PCKUP_NOT_ALW msg sent to vehicle             */
/*         if ZONE_TYPE == STAND && ZONE_STAND_STATUS == CLOSED    */
/*                STAND_CLOSED msg sent to vehicle                 */
/*******************************************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_POST_KEY %d\n", veh_ptr->veh_nbr);
#endif

      if (veh_ptr->t_status.noshow)
      {
	 send_text_predefined(veh_ptr->mid, NO_SHOW_WAIT, veh_ptr);
	 return (FAIL);
      }
      if (!(SIGNED_ON(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NOT_SIGN_IN, veh_ptr);
	 return (FAIL);
      }
      if (is_suspended(veh_ptr->mid, veh_ptr, EITHER, MSG))
	 return (FAIL);

      if (veh_ptr->t_status.meter_on)
      {

	 send_text_predefined(veh_ptr->mid, NO_POST_MTR_ON, veh_ptr);
	 return (FAIL);

      }
      if (BUSY(veh_ptr))
      {
	 send_text_predefined(veh_ptr->mid, NO_POST_MTR_ON, veh_ptr);
	 return (FAIL);
      }
      if (ACCEPTED(veh_ptr))
      {
	 send_text_predefined(veh_ptr->mid, NO_POST_ACPT, veh_ptr);
	 return (FAIL);
      }
      if (OFFERED(veh_ptr))
      {
	 send_text_predefined(veh_ptr->mid, NO_POST_OFF, veh_ptr);
	 return (FAIL);
      }
      strcat(argument, "\0");
      zone_nbr = atoi(argument);

      if (POSTED(veh_ptr))
      {
	 if ((mads_time - veh_ptr->time_info) < fleet[veh_ptr->fleet_nbr]->posting_freq)
	 {
	    send_text_predefined(veh_ptr->mid, NO_POST_TOO_FRE, veh_ptr);
	    return (FAIL);
	 } else
	 {
	   if (zone_nbr == veh_ptr->zone_num)
	     {
	       if (veh_ptr->t_status.on_break)
		 {
		   if (Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_BREAK_STATE, (HNDL) key_type, HNDL_NULL) == FAIL)
		     {
		       return (FAIL);
		     }
		   
		   if (Dispatcher_veh_match((VEH_HNDL) veh_ptr, zone_nbr) == FAIL)
		     send_qp(zone_nbr, veh_ptr);
		   return (SUCCESS);
	       } else
		 {
		   send_text_predefined(veh_ptr->mid, ALREADY_POST, veh_ptr);
		   return (FAIL);
		   
		 }
	     }
	 }
       }

      /* GPS active and empty form....find best zone based on GPS */
      if ( ((char)Fleet_get_value( (FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_GPS_STATE ) == YES ) &&
	  (zone_nbr == 0 ) )
	{
	  if ( ( veh_ptr->gps_long < (double)fleet[veh_ptr->fleet_nbr]->gps_min_x ) ||
	       ( veh_ptr->gps_long > (double)fleet[veh_ptr->fleet_nbr]->gps_max_x ) ||
	       ( veh_ptr->gps_lat  < (double)fleet[veh_ptr->fleet_nbr]->gps_min_y ) ||
	       ( veh_ptr->gps_lat  > (double)fleet[veh_ptr->fleet_nbr]->gps_max_y ) )
	    {
	      send_text( veh_ptr->mid, catgets(DISPATCH_catd, 1, 40, "GPS SAYS: NOT IN ZONE"), veh_ptr, PRIORITY_3 );
	      return (FAIL);
	    }
	  else if ( (time_t)Veh_get_value( (VEH_HNDL) veh_ptr, VEH_GPS_TIME ) <= 0 )
	    {
	      send_text( veh_ptr->mid, catgets(DISPATCH_catd, 1, 41,"GPS SAYS: NO CURRENT POSITION"), veh_ptr, PRIORITY_3 );
	      return (FAIL);
	    }
	  else
	    {
	      if ( GPS_get_best_zone( (VEH_HNDL) veh_ptr, &zone_nbr ) != SUCCESS )
		{
		  send_text( veh_ptr->mid, catgets(DISPATCH_catd, 1, 40, "GPS SAYS: NOT IN ZONE"), veh_ptr, PRIORITY_3 );
		  return (FAIL);
		}
	    }
	}
	      
      if (Zone_get_hndl(zone_nbr, veh_ptr->fleet_nbr, &zone_hndl) == FAIL)
      {
	 send_text_predefined(veh_ptr->mid, INV_ZONE_NO, veh_ptr);
	 return (FAIL);
      }
      if (!Veh_check_seals((VEH_HNDL) veh_ptr, zone_nbr))
      {
	 send_text_predefined(veh_ptr->mid, NO_PCKUP_NOT_ALW, veh_ptr);
	 return (FAIL);
      }
      if (( (short) Zone_get_value(zone_hndl, ZONE_TYPE) == ZN_STAND) ||
          (0) )
      {
	 if ((short) Zone_get_value(zone_hndl, ZONE_STAND_STATUS) == ZONE_STAND_CLOSED)
	 {
           if (0)
             {
               mk_outb_text("ZON STÄNGD");
               send_msg_mmp( veh_ptr->mid, TEXT_DISPLAY, veh_ptr );
             }
           else
             send_text_predefined(veh_ptr->mid, STAND_CLOSED, veh_ptr);
	    return (FAIL);
	 }
      }

      /* what about interaction between on_break and epost ?? */
      if (veh_ptr->t_status.on_break)
	 if (Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_BREAK_STATE, HNDL_NULL, HNDL_NULL) == FAIL)
	    return (FAIL);
      if (veh_ptr->t_status.e_posted)
	 return (Veh_reset_state((VEH_HNDL) veh_ptr, VEH_EPOSTED_STATE, (HNDL) zone_nbr, HNDL_NULL));
      return (Veh_set_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, (HNDL) zone_nbr));

   case VEH_Q_POSITION_KEY:
/*******************************************************/
/* VEH_Q_POSITION_KEY                                  */
/*  OP: porcess the q_position request                 */
/*  ARG: none                                          */
/*  PRE: veh  exists                                   */
/*  POST: queue position msg sent to vehicle           */
/*        VEH_INFO_TIME set                            */
/*  ERROR: if !VEH_SIGNED_IN_STATE                     */
/*                NOT_SIGN_IN msg sent to vehicle      */
/*         if VEH_CALL_OFFERED_STATE                   */
/*                NO_POST msg sent to vehicle          */
/*         if requesting too frequently                */
/*                NO_INFO_TOO_FRE msg sent to vehicle  */
/*         msg tranmission error                       */
/*******************************************************/
      /* Was k_info  & VEH_Q_POSITION */
#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_Q_POSITION_KEY\n");
#endif
      if (!(SIGNED_ON(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NOT_SIGN_IN, veh_ptr);
	 return (FAIL);
      }
      
      if (0)
	{
	  if (!POSTED(veh_ptr) && !EPOSTED(veh_ptr))
	    {
	      send_text_predefined(veh_ptr->mid, NOT_POST, veh_ptr);
	      return (FAIL);
	    }
	}
      else if ( !POSTED(veh_ptr) )
	{
	  send_text_predefined(veh_ptr->mid, NOT_POST, veh_ptr);
	  return (FAIL);
	}
      
      if (veh_ptr->veh_info_time != 0)
      {

	 if ((mads_time - veh_ptr->veh_info_time) < (long) fleet[veh_ptr->fleet_nbr]->veh_inquiry_freq)
	 {
	    send_text_predefined(veh_ptr->mid, NO_INFO_TOO_FRE, veh_ptr);
	    return (FAIL);
	 }
      }
      veh_ptr->veh_info_time = mads_time;
      return (send_qp(veh_ptr->zone_num, veh_ptr));

   case VEH_REJECT_KEY:
/**********************************************************************/
/* VEH_REJECT_KEY                                                     */
/*  OP: preprocess the reject key msg                                 */
/*  ARG: VEH_REJECT_PENALTY or NO_PENALTY                             */
/*  PRE: veh exists                                                   */
/*       call exists                                                  */
/*  POST:                                                             */
/*       if ARGUMENT == VEH_REJECT_PENALTY                            */
/*          if not a personal call                                    */
/*            if ZONE_REJECT_PENALTY f0r level offered                */
/*                penalty = ZONE_REJECT_PENALTY for level offered     */
/*            else                                                    */
/*                penalty = FLEET_REJECT_PENALTY for level offered    */
/*       else                                                         */
/*          penalty = none                                            */
/*      call determined                                               */
/*      CALL_OFFERED_STATE reset with penalty                         */
/*      VEH_CALL_OFFER_STATE reset with penalty                       */
/*  ERROR: if ARGUMENT == VEH_REJECT_PENALTY                          */
/*            if !VEH_SIGNED_IN_STATE                                 */
/*                   NOT_SIGN_IN msg sent to vehicle                  */
/*            if !VEH_CALL_OFFERED_STATE                              */
/*                   NO_RJCT_NOT_OFF msg sent to vehicle              */
/*            if VEH_ACCEPTED_STATE                                   */
/*                   NO_RJCT_NOT_OFF msg sent to vehicle              */
/*         if VEH_ACCEPTED_STATE || VEH_PICKUP_STATE                  */
/*                NO_REJECT msg sent to vehicle                       */
/*         if VEH_CALL_HNDL == HNDL_ERR                               */
/*                NO_REJECT msg sent to vehicle                       */
/**********************************************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_REJECT_KEY\n");
#endif
      if ((short) argument == VEH_REJECT_PENALTY)	/* from k_reject */
      {
	 if (!(SIGNED_ON(veh_ptr)))
	 {
	    send_text_predefined(veh_ptr->mid, NOT_SIGN_IN, veh_ptr);
	    return (FAIL);
	 }
	 if (!(OFFERED(veh_ptr)))
	 {
	   send_text_predefined(veh_ptr->mid, NO_RJCT_NOT_OFF, veh_ptr);
	   return (FAIL);
	 }
	 if (ACCEPTED(veh_ptr))
	 {
	   send_text_predefined(veh_ptr->mid, NO_RJCT_NOT_OFF, veh_ptr);
	   return (FAIL);
	 }
      }
      call_ptr = NULL;
      if (veh_ptr->t_status.accepted || veh_ptr->t_status.pckup)
      {

	 if (veh_ptr->veh_calls_list_hndl != HNDL_ERR)
	 {
	    Veh_calls_list_each_get(veh_ptr->veh_calls_list_hndl, tmp_call_hndl)
	    {
	       tmp_call_ptr = (struct calls *) tmp_call_hndl;
	       if (((tmp_call_ptr->personal_request == ONLY_PERS_REQ) ||
		    (tmp_call_ptr->personal_request == IF_PERS_REQ)) && !tmp_call_ptr->status.available)
		  call_ptr = tmp_call_ptr;
	       else
		  call_ptr = NULL;
	    }
	    Veh_calls_list_each_end(veh_ptr->veh_calls_list_hndl)
	 }
	 if (call_ptr == NULL)
	 {
	    send_text_predefined(veh_ptr->mid, NO_REJECT, veh_ptr);
	    return (FAIL);
	 }
      }
      if (call_ptr == NULL)
      {
	 if (ACCEPTED(veh_ptr) || BUSY(veh_ptr))
	 {
	    send_text_predefined(veh_ptr->mid, NO_REJECT, veh_ptr);
	    return (FAIL);
	 }
	 call_ptr = veh_ptr->call_ptr;
	 if (call_ptr == NULL)
	 {
	    send_text_predefined(veh_ptr->mid, NO_REJECT, veh_ptr);
	    return (FAIL);
	 }
      }
      penalty = '0';
      if ((short) argument == VEH_REJECT_PENALTY)
      {
	// determine level offered for the taxi if not a personal request
	if ( call_ptr->personal_request != ONLY_PERS_REQ )
	  {
	    zone_ptr = fleet[veh_ptr->fleet_nbr]->zone_ptr[call_ptr->pickup_zone];	
	    level_offered = -1;
	    for ( i = 0; i < LEVEL_MAX; i++ )
	      {
		for ( j = 0; j < BCK_UP_MAX; j++ )
		  {
		    if ( zone_ptr->level_number[i].bck_up_zone[j] == veh_ptr->zone_num )
		      {
			level_offered = i;
			break;
		      }
		  }
		if ( level_offered != -1 )
		  break;
	      }
	    
	    if ( level_offered == -1 )
	      {
		send_text_predefined(veh_ptr->mid, NO_REJECT, veh_ptr);
		return (FAIL);
	      }
	  }
		
	 if (!(call_ptr->personal_request == ONLY_PERS_REQ || call_ptr->personal_request == IF_PERS_REQ))
	    if ((penalty = fleet[veh_ptr->fleet_nbr]->zone_ptr[call_ptr->pickup_zone]->level_reject_penalty[level_offered])
		<= '0')
	    {
	       penalty = fleet[veh_ptr->fleet_nbr]->level_reject_penalty[level_offered];
	    }
      }
      
      if (penalty == '1')
      {
	 send_text_predefined(veh_ptr->mid, NO_REJECT, veh_ptr);
	 return (FAIL);
      }
      
      if ( ( penalty == '4' ) && (0) )
	{
	  Zone_get_hndl( call_ptr->pickup_zone, veh_ptr->fleet_nbr, &zone_hndl );
	  veh_found_for_flag = FALSE;
	  if ( zone_hndl != HNDL_NULL )
	    {
	      for ( k = 0; k < 5; k++ )
		{
		  if ( veh_found_for_flag == TRUE )
		    break;
		  for ( i=0; i< ZONE_BACKUP_MAX; i++ )
		    backup_zone[i] = 0;		  
		  Zone_get_backup_zone( zone_hndl, k, &backup_zone[0] );
		  for ( i = 0; i < ZONE_BACKUP_MAX; i++ )
		    {
		      if ( veh_found_for_flag == TRUE )
			break;
		      zone_nbr = backup_zone[i];
		      if ( zone_nbr <= 0 )
			continue;
		      Zone_get_hndl( zone_nbr, veh_ptr->fleet_nbr, &backup_zone_hndl );
		      if ( backup_zone_hndl == HNDL_ERR )
			continue;
		      Zone_veh_list_each_get( Zone_get_value( backup_zone_hndl, ZONE_AVAIL_VEH_LIST ), tmp_veh_hndl )
			{
			  if ( !Veh_get_state( tmp_veh_hndl, VEH_ON_BREAK_STATE ) &&
			      ( tmp_veh_hndl != (VEH_HNDL)veh_ptr ) )
			    {
			      veh_found_for_flag = TRUE;
			      break;
			    }
			} Zone_veh_list_each_end(Zone_get_value(backup_zone_hndl,ZONE_AVAIL_VEH_LIST))
		    }
		}
	      if ( (short)Call_get_value( (CALL_HNDL)veh_ptr->call_ptr, CALL_TYPE_TIME_CALL ) != TRUE )
		tmp_time = (time_t) Call_get_value( (CALL_HNDL)veh_ptr->call_ptr, CALL_TIME_DUE );
	      else
		tmp_time = (time_t)Call_get_value ( (CALL_HNDL)veh_ptr->call_ptr, CALL_PICKUP_LEAD_TIME );
	      if ( ( veh_found_for_flag == TRUE ) &&
		  ( tmp_time >
		   ( mads_time - (int)Fleet_get_value((FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_AUTO_REJECT_TIME ) ) ) )
		{
		  Call_reset_state((CALL_HNDL) call_ptr, CALL_OFFERED_STATE, (HNDL) penalty);
		  Veh_reset_state((VEH_HNDL) veh_ptr, VEH_CALL_OFFER_STATE, (HNDL) penalty, (HNDL) call_ptr);
		  return ( SUCCESS );
		}
	      else
		{
		  send_text_predefined( veh_ptr->mid, NO_REJECT, veh_ptr );
		  return ( FAIL );
		}
	    }
	  else
	    {
	      send_text_predefined( veh_ptr->mid, NO_REJECT, veh_ptr );
	      return ( FAIL );
	    }
	}
      
      Call_reset_state((CALL_HNDL) call_ptr, CALL_OFFERED_STATE, (HNDL) penalty);
      Veh_reset_state((VEH_HNDL) veh_ptr, VEH_CALL_OFFER_STATE, (HNDL) penalty, (HNDL) call_ptr);
      break;

   case VEH_SIGN_IN_KEY:
/****************************************************************/
/* VEH_SIGN_IN_KEY                                              */
/*  OP: preprocess the sign in msg                              */
/*  ARG: driver_id                                              */
/*  PRE: veh_exists                                             */
/*  POST: VEH_SIGN_IN_STATE set                                 */
/*        DRIVER_SIGN_IN_STATE set                              */
/*        if either suspeneded                                  */
/*          suspend msg sent to vehicle                         */
/*  ERROR: if VEH_SIGN_IN_STATE && driver_id != VEH_DRIVER_ID   */
/*               SIGN_IN_SIGN_OUT msg sent to vehicle           */
/*         if VEH_SIGNED_ON_STATE && driver_id == VEH_DRIVER_ID */
/*               authorize msg sent to vehicle                  */
/*               SIGN_IN_ALR msg sent to vehicle                */
/*         if Driver_validate_id == FAIL                        */
/*               INV_DRVR_ID msg sent to vehicle                */
/*         if DRIVER_SIGN_IN_STATE                              */
/*               SIGN_IN_DIF_CAB msg sent to vehicle            */
/*         if VEH_METER_ON_STATE                                */
/*               authorize msg sent to vehcicle                 */
/*               NO_POST_MTR_ON msg sent to vehcile             */
/*               deauthorize msg sent to vehicle                */
/****************************************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_SIGN_IN_KEY\n");
#endif
      if (SIGNED_ON(veh_ptr))
      {
	 if ((int) argument == veh_ptr->driver_id)
	 {
	    authorize(veh_ptr);
	    send_text_predefined(veh_ptr->mid, SIGN_IN_ALR, veh_ptr);

            if (veh_ptr->t_status.posted) /* no cheating */
                Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL);

	    return (FAIL);
	 } else
	 {
	    send_text_predefined(veh_ptr->mid, SIGN_IN_SIGN_OUT, veh_ptr);
	    return (FAIL);
	 }
      }
      (void) Fleet_get_hndl(BY_NBR, veh_ptr->fleet_nbr, &fleet_hndl);
      key_list = (FLEET_DRIVER_VEH_LIST_HNDL) Fleet_get_value(fleet_hndl, FLEET_DRIVER_VEH_LIST);
      if ((int) argument != 0)
         if (Fleet_driver_veh_list_find(key_list, (int) argument) != HNDL_NULL)
         {
	    authorize(veh_ptr);
	    send_text_predefined(veh_ptr->mid, SIGN_IN_DIF_CAB, veh_ptr);
	    term_type = (short)Veh_get_value( (VEH_HNDL) veh_ptr, VEH_TERM_TYPE );
	    if ( ( term_type == MDT_V03_TMAT ) || ( term_type == MDT_420_TMAT ) || ( term_type == MDT_OPUS_2K ) )
	      itm_send_msg( veh_ptr->mid, veh_ptr, DRIVER_VALIDNESS_NO, " " );	 	    
	    deauthorize(veh_ptr);
	    return (FAIL);
         }

      if (Driver_initialize((int) argument, (VEH_HNDL) veh_ptr, (char) argument2) == FAIL)
      {
	 authorize(veh_ptr);
	 send_text_predefined(veh_ptr->mid, INV_DRVR_ID, veh_ptr);
	 term_type = (short)Veh_get_value( (VEH_HNDL) veh_ptr, VEH_TERM_TYPE );
	 if ( ( term_type == MDT_V03_TMAT ) || ( term_type == MDT_420_TMAT ) || ( term_type == MDT_OPUS_2K ) )
	   itm_send_msg( veh_ptr->mid, veh_ptr, DRIVER_VALIDNESS_NO, " " );	 
	 deauthorize(veh_ptr);
	 return (FAIL);
      }

      // For HTD check if vehicle is OK according to the shift schedule

      if ( veh_ptr->t_status.meter_on )
	veh_ptr->t_status.meter_on = 0;
      
      Veh_set_state((VEH_HNDL) veh_ptr, VEH_SIGN_IN_STATE, (HNDL) argument);
      Driver_set_state((DRIVER_HNDL) veh_ptr, DRIVER_SIGN_IN_STATE, (HNDL) argument);
      is_suspended(veh_ptr->mid, veh_ptr, EITHER, MSG);

      term_type = (short)Veh_get_value( (VEH_HNDL) veh_ptr, VEH_TERM_TYPE );
      if ( ( term_type == MDT_V03_TMAT ) || ( term_type == MDT_420_TMAT ) || ( term_type == MDT_OPUS_2K ) )
	itm_send_msg( veh_ptr->mid, veh_ptr, DRIVER_VALIDNESS_YES, " " );

      break;

   case VEH_SIGN_OFF_KEY:
/*******************************************************/
/* VEH_SIGN_OFF_KEY                                    */
/*  OP: preprocess the sign out msg                    */
/*  ARG: none                                          */
/*  PRE: veh exists                                    */
/*  POST: VEH_SIGN_IN_STATE reset                      */
/*        DRIVER_SIGN_IN_STATE reset                   */
/*  ERROR: if !VEH_SIGN_IN_STATE                       */
/*              NOT_SIGN_IN msg sent to vehicle        */
/*         if VEH_ACCEPTED_STATE                       */
/*              NO_CPOST_ACPT msg sent to vehicle      */
/*         if VEH_PICKUP_STATE                         */
/*              NO_CPOST_MTR_ON msg sent to vehicle    */
/*******************************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_SIGN_OFF_KEY\n");
#endif
      if (!(SIGNED_ON(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NOT_SIGN_IN, veh_ptr);
	 return (FAIL);
      }
      if (ACCEPTED(veh_ptr))
      {
	 send_text_predefined(veh_ptr->mid, NO_CPOST_ACPT, veh_ptr);
	 return (FAIL);
      }

      if ( veh_ptr->t_status.crse_annul && (0) )
	{
	  send_text_predefined(veh_ptr->mid, NO_C_OUT, veh_ptr);
	  return (FAIL);
	}
      
      if (BUSY(veh_ptr))
      {
	 send_text_predefined(veh_ptr->mid, NO_CPOST_MTR_ON, veh_ptr);
	 return (FAIL);
      }
      if ((veh_ptr->t_status.on_break) && (veh_ptr->t_status.meter_on))
	{
	  send_text_predefined(veh_ptr->mid, NO_POST_MTR_ON, veh_ptr);
	  return (FAIL);
	}
/*
 *	G7 fix
 *	added the following call... the comments above talk about doing
 *	this but I guess it was never implemented- so here's the call
 *	This call is important to decrement the fleet->now_drv_attr[]
 */
	  (void)Driver_reset_state((DRIVER_HNDL)veh_ptr, 
			DRIVER_SIGN_IN_STATE, HNDL_NULL);

      return(Veh_reset_state((VEH_HNDL) veh_ptr, 
			VEH_SIGN_IN_STATE, HNDL_NULL, HNDL_NULL));

   case VEH_TALK_KEY:
/*************************************************/
/* VEH_TALK_KEY                                  */
/*  OP: preprocess a request to talk message     */
/*  ARG: none                                    */
/*  PRE: veh exists                              */
/*  POST: VEH_TALK_STATE set                     */
/*  ERROR: if !VEH_SIGN_IN_STATE                 */
/*              NOT_SIGN_IN msg sent to vehicle  */
/*************************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_TALK_KEY\n");
#endif
      if (!(SIGNED_ON(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NOT_SIGN_IN, veh_ptr);
	 return (FAIL);
      }
      Veh_set_state((VEH_HNDL) veh_ptr, VEH_TALK_STATE, HNDL_NULL);
      break;

   case VEH_UNPOST_KEY:
/*************************************************************/
/* VEH_UNPOST_KEY                                            */
/*  OP: preprocessing of unpost message                      */
/*  ARG: none                                                */
/*  PRE: veh_exists                                          */
/*  POST:                                                    */
/*       if VEH_ON_BREAK_STATE                               */
/*          VEH_ON_BEAK_STATE reset                          */
/*       VEH_POSTED_STATE reset                              */
/*  ERROR: if !VEH_SIGNED_IN_STATE                           */
/*                NOT_SIGN_IN msg sent to vehicle            */
/*         if VEH_SUSPENDED_STATE or DRIVER_SUSPENDED_STATE  */
/*                suspension msg sent to vehicle             */
/*         if VEH_NOSHOW_STATE                               */
/*                 send NO_SHOW_WAIT msg to vehicle          */
/*                NO_POST_MTR_ON msg sent to vehicle         */
/*         if VEH_ACCEPTED_STATE                             */
/*                NO_POST_ACPT msg sent to vehicle           */
/*         if VEH_CALL_OFFERED_STATE                         */
/*                NO_POST_OFF msg sent to to vehicle         */
/*         if !VEH_POSTED_STATE                              */
/*                NOT_POST msg sent to vehicle               */
/*         VEH_BREAK_STATE reset failure                     */
/*************************************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_UNPOST_KEY\n");
#endif
      if (veh_ptr->t_status.noshow)
      {
	 send_text_predefined(veh_ptr->mid, NO_SHOW_WAIT, veh_ptr);
	 return (FAIL);
      }
      if (!(SIGNED_ON(veh_ptr)))
      {
	 send_text_predefined(veh_ptr->mid, NOT_SIGN_IN, veh_ptr);
	 return (FAIL);
      }
      if (is_suspended(veh_ptr->mid, veh_ptr, EITHER, MSG))
	 return (FAIL);

      if (BUSY(veh_ptr))
      {
	 send_text_predefined(veh_ptr->mid, NO_POST_MTR_ON, veh_ptr);
	 return (FAIL);
      }
      if ((veh_ptr->t_status.on_break) && (veh_ptr->t_status.meter_on))
	{
	  send_text_predefined(veh_ptr->mid, NO_POST_MTR_ON, veh_ptr);
	  return (FAIL);
	}
      if (ACCEPTED(veh_ptr))
      {
	 send_text_predefined(veh_ptr->mid, NO_POST_ACPT, veh_ptr);
	 return (FAIL);
      }
      if (OFFERED(veh_ptr))
      {
	 send_text_predefined(veh_ptr->mid, NO_POST_OFF, veh_ptr);
	 return (FAIL);
      }
      if (!POSTED(veh_ptr))
      {
	 send_text_predefined(veh_ptr->mid, NOT_POST, veh_ptr);
	 return (FAIL);
      }
      if (veh_ptr->t_status.on_break)
	 if (Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_BREAK_STATE, HNDL_NULL, HNDL_NULL) == FAIL)
	    return (FAIL);
      Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, (HNDL) MSG, HNDL_NULL);
      break;

   case VEH_ZONE_INQUIRY_KEY:
/**********************************************************************/
/* VEH_ZONE_INQUIRY_KEY                                               */
/*  OP: process zone request for zone status                          */
/*  ARG: list of zones                                                */
/*  PRE: veh exists                                                   */
/*  POST: zones status msg sent to vehicle                            */
/*        VEH_INQRY_TIME set                                          */
/*  ERROR: if !VEH_SIGNED_IN_STATE                                    */
/*                NOT_SIGN_IN msg sent to vehicle                     */
/*         if (current time - VEH_INQRY_TIME) < FLEET_ZONE_INQRY_FREQ */
/*                NO_INQ_TOO_FRE msg sent to vehicle                  */
/**********************************************************************/

#ifdef DEBUG_OO
      if (debug_state)
	 printf("VEH_ZONE_INQUIRY_KEY\n");
#endif
      if (debug_state)
	 printf("VEH_ZONE_INQUIRY_KEY not implemented\n");
      break;

   default:
         sprintf(error_str, "ILLEGAL KEY %d VEH %d", key_type, veh_ptr->veh_nbr);
         ERROR(' ', "", error_str);
         return(FAIL);
   }				/* end case */
   return (SUCCESS);
}

   Veh_send_gps_zones( VEH_HNDL veh_hndl )
     {
       FLEET_HNDL fleet_hndl = HNDL_NULL;
       ZONE_HNDL  zone_hndl = HNDL_NULL;
       ZONE       *zone_ptr = NULL;
       int        i, distance;
       BOOLEAN    found_zone = FALSE;
       struct veh_driv *veh_ptr = NULL;
       short  sZoneNbr = 0;
       char            tmp_buf[ 512 ];
       
       if ( veh_hndl == HNDL_NULL )
         return (FAIL);

       veh_ptr = (struct veh_driv *)veh_hndl;
       Fleet_get_hndl( BY_NBR, (short)Veh_get_value( veh_hndl, VEH_FLEET_NBR ), &fleet_hndl );

       if ( fleet_hndl == HNDL_NULL )
         return (FAIL);

       mk_outb_text("");       
       add_outb_text( "ZON: " );

       for ( i = 0; i < ZONE_MAX; i++ )
         {
           if ( ( Zone_get_hndl( i, (short)Fleet_get_value(fleet_hndl, FLEET_NBR), &zone_hndl ) ) == FAIL )
             continue;
           
           zone_ptr = (ZONE *)zone_hndl;
           
           /* the zone is defined as a circle */
           if ( ( zone_ptr->gps_long > 0.0 ) &&
                ( zone_ptr->gps_lat  > 0.0 ) &&
                ((int)Zone_get_value( zone_hndl, ZONE_GPS_RADIUS) > 0 ) )
             {
               if ( ( distance = GPS_veh_zone_get_distance( veh_hndl, zone_hndl ) ) <
                    (int)Zone_get_value( zone_hndl, ZONE_GPS_RADIUS) )
                 {
                   found_zone = TRUE;
                   sprintf(tmp_buf, "%d ", i);
                   add_outb_text( tmp_buf );
                 }
             }
           /** The zone is defined by a polygon **/
           else if ( zone_ptr->poly_points > 0 )
             {
               if ( VehInZonePoly( veh_hndl, zone_hndl ) == 1 )
                 {
                   found_zone = TRUE;
                   sprintf(tmp_buf, "%d ", i);
                   add_outb_text( tmp_buf );
                 }
             }
         }
       
       if ( found_zone == TRUE )
         send_msg_mmp( veh_ptr->mid, TEXT_DISPLAY, veh_ptr );
       else
	  send_text( veh_ptr->mid, catgets(DISPATCH_catd, 1, 41,"GPS SAYS: NO CURRENT POSITION"), veh_ptr, PRIORITY_3 );
     }
       
   BOOLEAN Time_in_Shift( int skedNbr, char fleetID, int version )
     {
       struct tm *tm_ptr;
       struct cisam_sh s_sched;
       struct cisam_ss s_shift1;
       struct cisam_ss s_shift2;
       int curTime = 0;
       time_t tmp_time = 0;
       
       tm_ptr = localtime(&mads_time);
       curTime = tm_ptr->tm_hour * 100 + tm_ptr->tm_min;
       
       bzero(&s_sched, sizeof(struct cisam_sh));
       bzero(&s_shift1, sizeof(struct cisam_ss));
       bzero(&s_shift2, sizeof(struct cisam_ss));

       // Retrieve shift information for the present calendar day
       s_sched.schedule_num = skedNbr;
       s_sched.month = tm_ptr->tm_mon + 1;
       s_sched.fleet = fleetID;

       if ( db_read_key( S_SCHED_FILE_ID, &s_sched, &s_sh_key2, ISEQUAL ) == SUCCESS )
	 {       
	   //Valid schedule entry for this month and sked_nbr
	   s_shift1.schedule_key = s_sched.key;
	   s_shift1.day = tm_ptr->tm_mday;
	   s_shift1.vers_id = version;

	   if ( db_read_key( S_SHIFT_FILE_ID, &s_shift1, &ss_key3, ISEQUAL ) == SUCCESS )
	     {
	       // Shift for current calendar date/schedule number defined
	       if ( curTime < s_shift1.end_1 && curTime >= s_shift1.start_1 )
		 return (TRUE);
	       else if ( s_shift1.end_1 < s_shift1.start_1 )
		 {
		   s_shift1.end_1 += 2400;
		   if ( curTime < s_shift1.end_1 && curTime >= s_shift1.start_1 )
		     return (TRUE);
		 }
	       // Test the Second Shift Range
	       if ( curTime < s_shift1.end_2 && curTime >= s_shift1.start_2 )
		 return (TRUE);
	       else if ( s_shift1.end_2 < s_shift1.start_2 )
		 {
		   s_shift1.end_2 += 2400;
		   if ( curTime < s_shift1.end_2 && curTime >= s_shift1.start_2 )
		     return (TRUE);
		 }
	       // Test the Third Shift Range
	       if ( curTime < s_shift1.end_3 && curTime >= s_shift1.start_3 )
		 return (TRUE);
	       else if ( s_shift1.end_3 < s_shift1.start_3 )
		 {
		   s_shift1.end_3 += 2400;
		   if ( curTime < s_shift1.end_3 && curTime >= s_shift1.start_3 )
		     return (TRUE);
		 }	       
	     }
	   else if ( iserrno == ENOREC ) // no shift defined. Allow the shift.
	     return (TRUE);
	 }
       else
	 return (TRUE); // Problem with schedule definition...allow
       
       //If the current time is before 06:00, we may be working a shift
       //that started in the previous calendar day
       if ( curTime < 600 )
	 {
	   tmp_time = mads_time - 86400;
	   tm_ptr = localtime(&tmp_time);
	   bzero(&s_sched, sizeof(struct cisam_sh));
	   s_sched.schedule_num = skedNbr;
	   s_sched.month = tm_ptr->tm_mon + 1;
	   s_sched.fleet = fleetID;

	   if ( db_read_key( S_SCHED_FILE_ID, &s_sched, &s_sh_key2, ISEQUAL ) == SUCCESS )
	     {       
	       //Valid schedule entry for this month and sked_nbr
	       s_shift2.schedule_key = s_sched.key;
	       s_shift2.day = tm_ptr->tm_mday;
	       s_shift2.vers_id = version;

	       if ( db_read_key( S_SHIFT_FILE_ID, &s_shift2, &ss_key3, ISEQUAL ) == SUCCESS )
		 {
		   // Previous day's shift is defined. Only look at overlap shifts here
		   if ( s_shift2.end_1 < s_shift2.start_1 )
		     {
		       curTime += 2400;
		       s_shift2.end_1 += 2400;
		       if ( curTime < s_shift2.end_1 && curTime >= s_shift2.start_1 )
			 return (TRUE);		       
		     }
		   // Second Shift..
		   if ( s_shift2.end_2 < s_shift2.start_2 )
		     {
		       curTime += 2400;
		       s_shift2.end_2 += 2400;
		       if ( curTime < s_shift2.end_2 && curTime >= s_shift2.start_2 )
			 return (TRUE);
		     }
		   // Third Shift...
		   if ( s_shift2.end_3 < s_shift2.start_3 )
		     {
		       curTime += 2400;
		       s_shift2.end_3 += 2400;
		       if ( curTime < s_shift2.end_3 && curTime >= s_shift2.start_3 )
			 return (TRUE);
		     }		   
		 }
	     }
	 }
       
       return (FALSE);
     }

       
