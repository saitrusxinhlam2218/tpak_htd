static char sz__FILE__[]="@(#)itm.c  10.1.2.4(ver) Date Release 5/24/95" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  itm.c; Rel: 10.1.2.4; Get date: 5/24/95 at 11:36:07                      *
* @(#)  Last delta: 5/24/95 at 11:36:03                                               *
* @(#)  SCCS File: /taxi/sccs/s.itm.c                                                       *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS                                          *
*                                                                           *
*****************************************************************************/

/****************************************************************************/
/* file itm.c 			                                            */
/* routines for handile message to and from the ITM                         */
/* TaxiPak R4                                                               */
/* encapsulates all the itm format information 			            */
/* started 11 June 1990                                                     */
/* last update 18 Sept 1990                                                 */
/* M. R. Bradley 			                                    */
/****************************************************************************/

/****************************************************************************/
/* GENERAL USER ROUTINES                                                    */
/* INTERNAL ROUTINES                                                        */
/****************************************************************************/

#include <memory.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <langinfo.h>
#include <locale.h>

#include "taxipak.h"
#include "Object.h"
#include "Call_lists_public.h"
#include "dispatch_strdefs.h"
#include "path.h"
#include "sprintf.h"
#include "mads_types.h"
#include "mad_error.h"
#include "mads_isam.h"
#include "dispatch.h"
#include "language.h"
#include "kdt.h"
#include "veh_driv.h"
#include "df_maint.h"
#include "timers.h"
#include "writer.h"
#include "ex_key.h"
#include "except.h"
#include "calls.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "msg.h"
#include "msg_mmp.h"
#include "itm.h"
#include "credit.h"
#define  LIBRARY 1
#include "enhance.h"
#include "switch_ext.h"
#include "tmplen.h"
#include "Receipt_data_db.h"
#include "GPS_private.h"
#include "IMSI_db.h"

char *GPS_strstr( char * );
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

extern struct offsets *offset;	/* pointer to shared memory offsets */
extern struct fleets *fleet[FLEET_MAX];	/* shared memory fleet structure */
extern struct drvr_info driver_info;
extern char req_buf[];
char  credit_card_delimeter = ':';
char  trip_data_delimeter_str[] = "!BCDEFGHIJKLMPQRSTUVWZ%";

void  itm_send_msg();
extern struct cisam_ch writer_buf;
int itm_flag = FALSE; /* debug msg*/
#ifdef DEBUG_R4
extern char last_receipt_nbr[];
extern int mdebug;
#endif

extern int from_aux_device_fd;
extern int use_aux_pipes;
extern char *scratch_ptr;
void get_itm_receipt_nbr(char *, char *);

short
hex2int(text, digits)
/* convert one or two characters of hex to integer */
   char  text[];
int   digits;
{
   int   val1 = 0;
   int   val2 = 0;

   if (digits > 2)
      digits = 2;
   else if (digits < 1)
      digits = 1;
   if (strlen(text) < digits)
   {
     /*  fprintf(stderr, "Illegal string in hex2int\n"); */
      return (0);
   }
   /* no checking for illegal digits */
   if (digits > 1)
   {
      if (text[1] > '9')
	 val1 += text[1] - 'A' + 10;
      else
	 val1 += text[1] - '0';
   }
   if (text[0] > '9')
      val2 += text[0] - 'A' + 10;
   else
      val2 += text[0] - '0';
   if (digits > 1)
      val2 *= 16;
   return (val1 + val2);
}				/* end hex2int */

/*************************************************************/
/* itm_credit_verify()                                       */
/* process an itm originated credit verification request     */
/* return status to terminal                                 */
/*************************************************************/

void
itm_credit_verify(text, veh_ptr, term_id)
   char *text;
   struct veh_driv *veh_ptr;
   char term_id[];
{
   char *str_ptr;
   int   length;
   int   rc;

#ifdef DEBUG
   if (offset->prattle >= LEVEL6)
   {
      sprintf(trace_str,
	      "ENTERING itm_credit_verify (term_id=%s,  veh_nbr=%x)\n",
	      term_id, veh_ptr);
      TRACE(trace_str);
   }
#endif

   if (!(SIGNED_ON(veh_ptr)))
   {
      /* notify driver that vehicle has not signed in */
      send_text_predefined(term_id, NOT_SIGN_IN, veh_ptr);
      return;
   }
 
#ifdef TODO
   /* determine credit catd type */
#endif

   rc = verify_acct(MDT_ITM, veh_ptr, UNKNOWN_ACCT_TYPE, text);
   if (rc != BAD_ACCOUNT)
      /* send the text for now with payment as the authorization # */
      itm_send_msg(term_id, veh_ptr, CARD_VALIDNESS_YES, text);
   else
      itm_send_msg(term_id, veh_ptr, CARD_VALIDNESS_NO, text);

#ifdef DEBUG
   if (offset->prattle >= LEVEL6)
      TRACE("LEAVING itm_credit_verify\n");
#endif
}				/* end itm_credit_verify */

/*************************************************************/
/* itm_trip_complete()                                       */
/* accept the trip complete msg from the ITM                 */
/* call for the meter off  processing                        */
/* check for missing receipts                                */
/*************************************************************/

void
itm_trip_complete(text, veh_ptr, term_id)
   char *text;
   struct veh_driv *veh_ptr;
   char *term_id;
{
   char  receipt_nbr_str[RECEIPT_NBR_SIZE + 1];
   int   len;

   len = get_mmp_msg_field(text, receipt_nbr_str, RECEIPT_NUM_FIELD);

   k_meter_off(veh_ptr, term_id);

}				/* end itm_trip_complete */

/*************************************************************/
/* itm_trip_data()                                           */
/* accept the trip data from the ITM                         */
/* archive                                                   */
/* check the receipt  nbrber and request missing receipts    */
/*************************************************************/

void
itm_trip_data(text, veh_ptr, term_id)
   char *text;
   struct veh_driv *veh_ptr;
   char *term_id;
{
  static char  str_temp[TMP_STR_LEN];
  char *str_ptr;
  int call_number;
   char   rcpt_nbr[ TD_RCPT_NBR_SIZE + 1 ];
  
  if (0)
    {

      
      if ( log_itm_receipt_db( veh_ptr->veh_nbr, text ) == 0 )
	{
	  if ( strstr( text, "%E" ) )
	    strcpy( strstr( text, "%E"), "\n");	  
          //	  log_itm_receipt(veh_ptr->veh_nbr, term_id,  text , -1);
	  strncpy( rcpt_nbr, (text + TD_RCPT_NBR_OFFSET), TD_RCPT_NBR_SIZE );
	  rcpt_nbr[ TD_RCPT_NBR_SIZE ] = '\0';
	  itm_send_msg(term_id, veh_ptr, TRIP_DATA, rcpt_nbr);
	}
      else
	{
	  /** transaction already in receipts database - just ACK receipt **/
	  strncpy( rcpt_nbr, (text + TD_RCPT_NBR_OFFSET), TD_RCPT_NBR_SIZE );
	  rcpt_nbr[ TD_RCPT_NBR_SIZE ] = '\0';
	  itm_send_msg(term_id, veh_ptr, TRIP_DATA, rcpt_nbr);
	}
    }
  else
    {
      get_itm_receipt_nbr(&text[2], str_temp);
      
      /* save the receipt */
      str_ptr = (char *) strchr(text, CTRL_START);
      if (str_ptr != NULL)
	{
	  *str_ptr = '\n';
	  str_ptr++;
	  *str_ptr = '\0';
	  call_number = 0; /* initialize call number */
	  if (veh_ptr->call_ptr !=0) call_number = veh_ptr->call_ptr->call_number;
	  log_itm_receipt(veh_ptr->veh_nbr, veh_ptr->mid, &text[2], call_number);
	}
    }
}				/* end itm_trip_data */

/*************************************************************/
/* itm_receipt_data()                                        */
/* accept the receipt date requested by host from the itm    */
/* archive                                                   */
/*************************************************************/

void
itm_receipt_data(text, veh_ptr, term_id)
   char *text;
   struct veh_driv *veh_ptr;
   char *term_id;
{
	 static char  str_temp[TMP_STR_LEN];
   char  flag[RECEIPT_REQUEST_FLAG_SIZE + 1];
   char *str_ptr;
   int receipt_nbr;
   int call_number;

   strncpy(flag, &text[2], RECEIPT_REQUEST_FLAG_SIZE);
   flag[RECEIPT_REQUEST_FLAG_SIZE] == '\0';

   get_trip_date(&text[2], str_temp);
   if (atoi(str_temp) == 0)
   {
#ifdef ASSERT
      if (itm_flag)
         printf("ASSERT ERROR - bad receipt returned %s\n", text);
#endif
      get_itm_receipt_nbr(&text[2], str_temp);
      receipt_nbr = atoi(str_temp);
      if (receipt_nbr)
      {
         sprintf(str_temp,"Receipt nbr %d reported lost from veh #%s", receipt_nbr, veh_ptr->mid);
         ERROR(' ', "", str_temp);
         return;
      }
      else                
      {
         sprintf(str_temp,"Bad requested receipt nbr from veh #%s",veh_ptr->mid);
         ERROR(' ', "", str_temp);
         return;
      }
   }

   get_itm_receipt_nbr(&text[2], str_temp);

   /* save the receipt */
   str_ptr = (char *) strchr(text, CTRL_START);
   if (str_ptr != NULL)
   {
        *str_ptr = '\n';
        str_ptr++;
        *str_ptr = '\0';

        /* dont log receipt if its a duplicate */
	{
		call_number = 999999999; /* initialize call number to designate a missing receipt */
	}
   }
}				/* end itm_receipt_data */


/*************************************************************/
/* itm_sign_on()                                             */
/* process the itm originated sign on message                */
/* call the regular sign on routine k_sign_in                */
/*************************************************************/

void
itm_sign_on(text, veh_ptr, term_id)
   char  text[];
struct veh_driv *veh_ptr;
char *term_id;
{
   char *pGPSdata = NULL;
   char *pIMSI = NULL;
   char IMSI_nbr[24];
   VEH_REC    veh_rec;
   IMSI_REC   imsi_rec;
   /* get the driver id from the message ignore the ITM id */
   char  drv_id[ITM_DRV_ID_SIZE + 1];
   int   iDrvID, call_number;

   struct calls *call_ptr = NULL;
   static struct call_ipc_rec call_buf;
   struct cisam_cl *cl_ptr;
   short fl_num;
   int   num;
   char pDate[16];
   LIST_HNDL  tmp_list;
   short term_type;

   bzero(IMSI_nbr, 24);
   bzero(&veh_rec, sizeof(VEH_REC));
   
   term_type = (short)Veh_get_value( (VEH_HNDL) veh_ptr, VEH_TERM_TYPE );

   switch ( term_type )
     {
     case MDT_V03_TMAT:
     case MDT_420_TMAT:
     case MDT_OPUS_2K:
       if ( strlen( text ) < TMAT_DRV_ID_SIZE )
	 return;
       strncpy( drv_id, &text[ TMAT_DRV_ID_OFFSET ], TMAT_DRV_ID_SIZE );
       drv_id[ TMAT_DRV_ID_SIZE ] = '\0';
       break;
       
     default:
       /** probably just Stockholm with ITM **/
       strcpy( pDate, catgets( DISPATCH_catd, 2, 29, "QP..." ));
       
       memcpy(drv_id, &text[ITM_DRIV_SHIFT_START], ITM_DRV_ID_SIZE);
       drv_id[ITM_DRV_ID_SIZE] = '\0';
       
       
     }
   
   k_sign_in(drv_id, veh_ptr, term_id, 0);

   if ( ( pGPSdata = (char *)GPS_strstr( text ) ) != NULL )
     *(pGPSdata+2) = '\0';
   
   if ( strlen( text ) == IMSI_ITM_SHIFT_START_LEN )
     {
       pIMSI = (char *)(text + IMSI_ITM_SHIFT_START_BEGIN);
       strncpy( IMSI_nbr, pIMSI, IMSI_LEN );

       imsi_rec.fleet = fleet[veh_ptr->fleet_nbr]->fleet_id;
       strcpy(imsi_rec.imsi, IMSI_nbr);
       if ( db_read_key(IMSI_FILE_ID, &imsi_rec, &imsi_key2, DB_EQUAL) == SUCCESS )
	 {
	   veh_rec.fleet = fleet[veh_ptr->fleet_nbr]->fleet_id;
	   veh_rec.nbr = veh_ptr->veh_nbr;
	   if (db_read_key(VEHICLE_FILE_ID, &veh_rec, &veh_key2, DB_EQUAL + DB_LOCK) == SUCCESS)
	     {
	       strncpy(veh_rec.mdt_tele, imsi_rec.telephone, 17);
	       db_update(VEHICLE_FILE_ID, &veh_rec);
	       db_release(VEHICLE_FILE_ID);
	     }
	 }
       else if ( iserrno == ENOTOPEN )
	 {
	   db_open(IMSI_FILE_ID, ISINOUT|ISMANULOCK);
	   if ( db_read_key(IMSI_FILE_ID, &imsi_rec, &imsi_key2, DB_EQUAL) == SUCCESS )
	     {
	       veh_rec.fleet = fleet[veh_ptr->fleet_nbr]->fleet_id;
	       veh_rec.nbr = veh_ptr->veh_nbr;
	       if (db_read_key(VEHICLE_FILE_ID, &veh_rec, &veh_key2, DB_EQUAL + DB_LOCK) == SUCCESS)
		 {
		   strncpy(veh_rec.mdt_tele, imsi_rec.telephone, 17);
		   db_update(VEHICLE_FILE_ID, &veh_rec);
		   db_release(VEHICLE_FILE_ID);
		 }
	     }
	 }
     }

   if ( pGPSdata != NULL )
     *(pGPSdata + 2) = '@';
   
   if ( ( pGPSdata = GPS_strstr( text ) ) != NULL )
     {
       if ( strlen( pGPSdata ) == GPS_MESSAGE_LEN )
	 {
	   pGPSdata += 2;
	   local_gps_form_hndl( veh_ptr, pGPSdata );
	   gps_msg_hndl( veh_ptr, MDT_MSG_SIGNIN );	  
	   *pGPSdata = '\0';	  
	 }
     }      

}				/* end itm_sign_on */

/*************************************************************/
/*itm_sign_on_validity()				     */
/*verify that the inbound itm driver id that is sent as a    */
/*response to an ITM query matches that of the driver id     */
/*that was entered into the MDT front panel.                 */
/*************************************************************/
itm_sign_on_validity (text, veh_ptr, term_id)
	char text[];
	struct veh_driv *veh_ptr;
	char *term_id;
{
	char temp_id[ITM_DRV_ID_SIZE_VERS2 + 1];
	int  iDrvID, call_number;
	char drv_id_serial;
	char IMSI_nbr[24];
	char *pIMSI;
	IMSI_REC   imsi_rec;	
	VEH_REC    veh_rec;
	char  *pGPSdata;	
	
	bzero(IMSI_nbr, 24);
	bzero(&veh_rec, sizeof(VEH_REC));	

	if ( (short)Veh_get_value((VEH_HNDL)veh_ptr, VEH_TERM_TYPE) == MDT_V03_ITM_VERS2 )
	  {
	    memcpy(temp_id, &text[ITM_DRIV_START_VERS2], ITM_DRV_ID_SIZE_VERS2);
	    temp_id[ITM_DRV_ID_SIZE_VERS2] = '\0';
	  }
	else
	  {
	    memcpy(temp_id, &text[ITM_DRIV_START_VERS1], ITM_DRV_ID_SIZE_VERS1);
	    temp_id[ITM_DRV_ID_SIZE_VERS1] = '\0';
	  }

	drv_id_serial = text[ITM_DRIV_ID_SERIAL_NBR_START];
	iDrvID = atoi( temp_id );

	

	if (SIGNED_ON(veh_ptr))
	  {
	    if (iDrvID != veh_ptr->driver_id)
	      {
		if ((veh_ptr->call_ptr) && (ACCEPTED(veh_ptr)))
		  {
		    call_number = veh_ptr->call_ptr->call_number;
		    writer_signon_cancel(call_number);
		    Call_cancel((CALL_HNDL)veh_ptr->call_ptr, (VEH_HNDL)veh_ptr, 1, 0);
		    writer_buf.call_nbr = call_number;
		    writer_buf.fleet = veh_ptr->fleet;
		    writer_buf.event_type = EVNT_REG_TYPE;
		    strcpy(writer_buf.event_desc, CALL_CANCEL_SIGNON);
		    writer_buf.drv_id = veh_ptr->driver_id;
		    writer_buf.action_veh_nbr = 0;
		    writer_buf.action_drv_id = 0;
		    writer_buf.exception_nbr = 0;
		    writer_add_hist(&writer_buf);
		  }
		Veh_process_key((VEH_HNDL) veh_ptr, VEH_SIGN_OFF_KEY , HNDL_NULL, HNDL_NULL);
	      }
	  }
   
	k_sign_in(temp_id, veh_ptr, term_id, drv_id_serial);

	if ( ( pGPSdata = (char *)GPS_strstr( text ) ) != NULL )
	  *(pGPSdata+2) = '\0';
	
	if ( strlen( text ) == IMSI_ITM_SIGN_ON_INFO_LEN )
	  {
	    pIMSI = (char *)(text + IMSI_ITM_SIGN_ON_INFO_BEGIN);
	    strncpy( IMSI_nbr, pIMSI, IMSI_LEN );

	    imsi_rec.fleet = fleet[veh_ptr->fleet_nbr]->fleet_id;
	    strcpy(imsi_rec.imsi, IMSI_nbr);
	    if ( db_read_key(IMSI_FILE_ID, &imsi_rec, &imsi_key2, DB_EQUAL) == SUCCESS )
	      {
		veh_rec.fleet = fleet[veh_ptr->fleet_nbr]->fleet_id;
		veh_rec.nbr = veh_ptr->veh_nbr;
		if (db_read_key(VEHICLE_FILE_ID, &veh_rec, &veh_key2, DB_EQUAL + DB_LOCK) == SUCCESS)
		  {
		    strncpy(veh_rec.mdt_tele, imsi_rec.telephone, 17);
		    db_update(VEHICLE_FILE_ID, &veh_rec);
		    db_release(VEHICLE_FILE_ID);
		  }
	      }
	  }	
	
	if ( pGPSdata != NULL )
	  {
	    *(pGPSdata + 2) = '@';
	    if ( strlen( pGPSdata ) == GPS_MESSAGE_LEN )
	      {
		pGPSdata += 2;
		local_gps_form_hndl( veh_ptr, pGPSdata );
		gps_msg_hndl( veh_ptr, MDT_MSG_SIGNIN );
		*pGPSdata = '\0';
	      }
	  }
}


/*************************************************************/
/* itm_msg_hndl()                                            */
/* dispatch all inbound itm msg to appropriate routine       */
/*************************************************************/

void
itm_msg_hndl(msg, veh_ptr, term_id)
   MSG_QUEUE_HDR *msg;
   struct veh_driv *veh_ptr;
   char *term_id;
{
   short itm_msg_type;
   short itm_seq_nbr;
   static char  str_temp[TMP_STR_LEN];
   int i,j,rc;
   char data_str[512];
   char card_type_comment[64];
#ifdef DEBUG_R4
   void  itm_special_request();
   int   request_code = 0;
   char  *str_ptr;
#endif
   char  *pGPSdata;
   double TaxiX = 0.0, TaxiY = 0.0;
   char  Lat[ LAT_LEN ];
   char  Long[ LONG_LEN ];
   VEH_REC  veh_rec;
   char gps_time[12];
  double Northing, Easting;
   
   itm_msg_type = hex2int(&msg->text[ITM_TEXT_START], 2);

   if ( itm_msg_type == TRIP_STOP )
     {
       if ( ( pGPSdata = GPS_strstr( msg->text ) ) != NULL )
	 {
	   if ( strlen( pGPSdata ) == GPS_MESSAGE_LEN )
	     {
	       pGPSdata += 2;
	       local_gps_form_hndl( veh_ptr, pGPSdata );
	       *pGPSdata = '\0';
	     }
	 }
     }

   /* OLD STYLE ITM SOFTWARE */
   if (msg->text[0] == CTRL_START)
      itm_msg_type = BOGUS_METER_OFF;


   switch (itm_msg_type)
   {
#ifdef DEBUG_R4
   case METER_STATUS:
      (void) get_mmp_msg_field(&msg->text[ITM_MSG_START], str_temp, METER_STATUS_FIELD);
      if (itm_flag)
         fprintf(stderr, "METER STATUS - %s\n", str_temp);
      break;
   case TX_PARAMETERS:
      print_itm_parameters(&msg->text[ITM_MSG_START], veh_ptr, term_id);
      break;

#endif

   case TRIPFIFO_DATA:
   case MULT_TRIPFIFO_DATA:
      if (1)
	{
	  send_ccp_msg(msg,veh_ptr);
	}
      else if (0)
	{
	  /* Log receipt even if logged before */
	  
	  if (strstr(&msg->text[ITM_MSG_START], "%D%E"))
	    {
	      strcpy( strstr(&msg->text[ITM_MSG_START], "%D%E"),"\n"); /* The '\' in "%D\%E" is used to prevent the sccs
									   system from interpreting the first 3 characters */
	      if (log_itm_receipt(veh_ptr->veh_nbr, term_id,  &msg->text[ITM_MSG_START] , -1) == 0) /* Log to receipt file */
		itm_send_msg(term_id, veh_ptr, ACK_FIFO, " ");                                        /* send ACK_FIFO to vehicle */
	    }
	  else if (strstr(&msg->text[ITM_MSG_START], "%D%P"))
	    {
	      strcpy( strstr(&msg->text[ITM_MSG_START], "%D%P"), "\n");
	      if (log_itm_receipt(veh_ptr->veh_nbr, term_id,  &msg->text[ITM_MSG_START] , -1) == 0) 
		itm_send_msg(term_id, veh_ptr, ACK_FIFO, " ");                                      
	    }	      
	  else
	    {
	      sprintf(str_temp,"MID#%s, Wrong TRIPFIFO_DATA:%s",term_id,&msg->text[ITM_MSG_START]);
	      ERROR(' ',"",str_temp);
	    }
	}

      break;

   case CCP_MSG_FROM_ITM:
     if ( 1 )
       // this comes later when MDXS is ready from Semel
	 send_ccp_msg(msg,veh_ptr);
     break;
     
   case RQ_CARDVAL:
   case RQ_CARD:
      /* Tampere system for validation */
      if (1)
	{
	  send_ccp_msg(msg,veh_ptr);
	}
      /* Stockholm system for validation */
      else if (0)
	{
	  get_card_validity_reply(data_str, &msg->text[ITM_MSG_START],veh_ptr);
	  itm_send_msg(term_id,veh_ptr,CARD_VALIDITY,data_str);
	}

      break;
      
   case FTJ_VALID:
       if (0)
	   {
	       get_ftj_card_validity_reply(data_str, &msg->text[ITM_MSG_START],veh_ptr);
	       itm_send_msg(term_id,veh_ptr,CARD_VALIDITY,data_str);
	   }
       break;
       
   case ACK_KEYS:
      if (1) {
	 send_ccp_msg(msg,veh_ptr);
      }
      break;
    case METER_ACTION_ACK :
      if (veh_ptr->call_ptr == NULL)
	{
	  ERROR(' ', " ", "No call_ptr in veh_ptr (itm METER_ACTION_ACK)");
	  return;
	}
      if (msg->text[ITM_MSG_START] == 'Y')
	  Call_msg_itm_ack( (VEH_HNDL) veh_ptr );
      else
	{
	  /** ITM NAK'd message **/
	  /** generete INVOICE exception if non-FTJ call, otherwise generate SYSERR exception **/
	  if (veh_ptr->call_ptr->FTJ_nbr == -1)
	    {	      
	      if (veh_ptr->call_ptr != NULL)
		{

		  if (0)
		    Call_msg_itm_nak((VEH_HNDL)veh_ptr);
		}
	    }
	  else
	    {
	      if (0)
		Call_msg_itm_nak( (VEH_HNDL) veh_ptr );
	    }
	}
      break;

   case PRINT_TEXT_ACK :
      if (0)
	  Call_msg_printer_ack( (VEH_HNDL) veh_ptr );
      break;
      
   case RQ_CARD_VALIDNESS_1:
   case RQ_CARD_VALIDNESS_2:
      /* TaxiMat meter system for validation */
      if (0)
	{
	  card_type_comment[0] = '\0';
	  if ( ( rc = verify_taximat_card_type( &msg->text[ITM_MSG_START], card_type_comment ) ) == CARD_TYPE_TEST_SUCCESS )
	    {
	       if ( verify_taximat_card_nbr ( &msg->text[ITM_MSG_START] ) == 1 )
		 itm_send_msg(term_id, veh_ptr, CARD_VALIDNESS_YES, card_type_comment );
	       else
		 {
		   sprintf( card_type_comment, "%s %d",
			   catgets(DISPATCH_catd, DISPATCH_SET, DISPATCH_2, "BAD"), abs(CARD_TYPE_BAD_CRED_FAIL) );
		   itm_send_msg(term_id, veh_ptr, CARD_VALIDNESS_NO, card_type_comment);
		 }
	     }
	  else
	    {
	      sprintf( card_type_comment, "%s %d", catgets(DISPATCH_catd, DISPATCH_SET, DISPATCH_2, "BAD"), abs(rc) );
	      itm_send_msg( term_id, veh_ptr, CARD_VALIDNESS_NO, card_type_comment );
	    }
	}
      else
	itm_credit_verify(&msg->text[ITM_MSG_START], veh_ptr, term_id);
      break;
   case SHIFT_START:
      itm_sign_on(&msg->text[ITM_MSG_START], veh_ptr, term_id);
      break;
    case SIGN_ON_INFO_VERS1:
    case SIGN_ON_INFO_VERS2:
      itm_sign_on_validity (&msg->text [ITM_MSG_START], veh_ptr, term_id);
      break;
   case TRIP_DATA:
      itm_trip_data(&msg->text[ITM_MSG_START], veh_ptr, term_id);
      break;
#ifdef DEBUG_R4
   case BOGUS_METER_OFF:
      /* phony up a trip complete message */
      strcpy(&msg->text[ITM_MSG_START], last_receipt_nbr);
      strcat(msg->text, FIELD_END);
#endif
   case TRIP_STOP:
      if (0) /* Don't check for missing receipts on TRIP_STOP */
	k_meter_off(veh_ptr,term_id);
      else
	itm_trip_complete(&msg->text[ITM_MSG_START], veh_ptr, term_id);
      break;
   case SHIFT_STOP:
        k_sign_out(veh_ptr, term_id);
	break;
   case NON_SUPPORTED_1:
        break;
   case BREAK_START:
        k_break(veh_ptr, term_id);
	break;
    case NOSHOW:
        k_noshow( msg, veh_ptr, term_id, ITM_NO_SHOW );
        break;
   case BREAK_STOP:
	if (0)
	  {
	    if (veh_ptr->t_status.on_break)              /* If vehicle previously on break */
	      {
		if (Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_BREAK_STATE, (HNDL) VEH_POST_KEY, HNDL_NULL) != FAIL)
		  {
		    if (Dispatcher_veh_match((VEH_HNDL) veh_ptr, veh_ptr->zone_num) == FAIL)
		      send_qp(veh_ptr->zone_num, veh_ptr);
		  }
	      }
	  }

      break;
	  
   case METER_FAULT:		/* no action */
      break;

   case CONTROL_REGS:

     /* Used at HTD to pass a GPS position after being polled. Format is as follows:
	05 00005A0000 HH:MM:SS +LLlllllll +YYyyyyyyy %D%E
	1234567890123456789012345678901234567890123456789 - 49 bytes  */
	if ( strlen( msg->text ) > 37 )
	  {
	    memcpy( Lat, (msg->text + DDS_LAT_OFFSET), 2 );
	    memcpy( Long, (msg->text + DDS_LONG_OFFSET), 2);
	    Lat[2] = *nl_langinfo(RADIXCHAR);
	    Long[2] = *nl_langinfo(RADIXCHAR);
	    memcpy( (Lat + 3), (msg->text + DDS_LAT_OFFSET + 3), 6 );
	    memcpy( (Long + 3), (msg->text + DDS_LONG_OFFSET + 3), 6 );
	    Lat[9] = '\0';
	    Long[9] = '\0';
	    TaxiX = atof( Long );
	    TaxiY = atof( Lat );
	    LatLong_to_UTM( TaxiY, TaxiX, &Northing, &Easting );
	    TaxiX = Easting;
	    TaxiY = Northing;
	    veh_ptr->gps_long = TaxiX;
	    veh_ptr->gps_lat = TaxiY;
	    memcpy( gps_time, (msg->text + DDS_TIME_OFFSET), 8 );
	    gps_time[8] = '\0';
	    Veh_set_value((VEH_HNDL)veh_ptr, VEH_GPS_TIME, (HNDL)gps_time);

	    bzero(&veh_rec, sizeof(VEH_REC));
	    veh_rec.fleet = veh_ptr->fleet;
	    veh_rec.nbr = veh_ptr->veh_nbr;
	    if ( db_read_key(VEHICLE_FILE_ID, &veh_rec, &veh_key2, ISEQUAL) == SUCCESS )
	      {
		veh_rec.gps_lat = veh_ptr->gps_lat;
		veh_rec.gps_long = veh_ptr->gps_long;
		db_update( VEHICLE_FILE_ID, &veh_rec );
	      }
	    
	    /* add a new record to the tracking layer */
	    Tracking_Layer_db_add( (VEH_HNDL) veh_ptr );
	    
	  }
	break;
      
   default:
      if ( use_aux_pipes == TRUE )
	{
	  strcat( msg->text, CHECK_CHAR_STR );
	  write( from_aux_device_fd, msg, msg->size + sizeof(msg->size) );
	}
      break;

   } /* end switch */
#ifdef DEBUG_R4_LOTS
   /* RQ_RECEIPT_DATA      0x08 */
   /* RQ_STATUS            0x07 */
   /* RQ_PARAMETERS        0x04 */
   /* RQ_CONTROL_REGS      0x05 */
   if (request_code)
      (void) itm_special_request(request_code, veh_ptr, term_id);
#endif
#ifdef DEBUG
   if (offset->prattle >= LEVEL6)
      TRACE("LEAVING itm_msg_hndl\n");
#endif
} /* end itm_msg_hndl() */

/*************************************************************/
/* itm_msg_send()                                            */
/* send a message to the itm                                 */
/*************************************************************/

void
itm_send_msg(term_id, veh_ptr, itm_msg_type, msg)
   char  term_id[];		/* rf id */
   int   itm_msg_type;
   char  msg[];
struct veh_driv *veh_ptr;
{
  static char  str_temp[TMP_STR_LEN];
  enum {
    T1, T2 } tstate;
  int text_len = 0;
  int i;
  extern MSG_QUEUE_HDR outb;
   
  if ( itm_msg_type == CCP_MSG_TO_ITM )
    {
      for ( tstate = T1;; )
	{
	  if ( tstate == T1 )
	    {
	      if ( msg[text_len] == '%' )
		tstate = T2;
	      ++text_len;
	    }
	  else
	    {
	      if ( msg[text_len] == 'E' )
		{
		  ++text_len;
		  break;
		}
	      else
		{
		  ++text_len;
		  tstate = T1;
		}
	    }
	}
      sprintf(str_temp, "%02X", itm_msg_type);
      memcpy(str_temp+2, msg, text_len);

      bzero(outb.text, MMP_TEXT_SIZE);
      for ( i = 0; i < text_len+2; i++ )
	outb.text[i] = str_temp[i];
      
    }
  else
    {
      sprintf(str_temp, "%02X%s", itm_msg_type, msg);
      mk_outb_text(str_temp);      
    }
  
   send_msg_mmp(term_id, ITM_MSG, veh_ptr);


}				/* end itm_send_msg */

get_trip_date(char *src_ptr, char *dest_ptr)
/* get the date time string from the source */
{
 if (src_ptr == NULL)
    return;
 strncpy(dest_ptr, &src_ptr[TRIP_DATE_INDX], TRIP_DATE_SIZE);
 dest_ptr[TRIP_DATE_SIZE] = '\0';
}                    /* end get_trip_date */

int
get_itm_acct_nbr(char *text, char *acct_nbr)
/* get the itm credit verification account number */
{
   char *str_ptr;
	 static char str_temp[TMP_STR_LEN];

   if (strlen(text) < ITM_ACCOUNT_FIELD_LEN)
        return (-1);

   strncpy(str_temp, text, ITM_ACCOUNT_FIELD_LEN);

   str_ptr = (char *) strchr(str_temp, credit_card_delimeter);
   if (str_ptr != NULL)
      *str_ptr = '\0';
   else
   {
      str_ptr = (char *) strchr(str_temp, '=');
      if (str_ptr != NULL)
         *str_ptr = '\0';
      else
         str_temp[ITM_ACCOUNT_FIELD_LEN] = '\0';
   }
   strcpy(acct_nbr, str_temp);
   return(0);
}                              /* end get_itm_acct_nbr */

void
get_itm_acct_info(text, acct_info_ptr)
    char * text;
    struct verify_acct_info *acct_info_ptr;
/* get the account infor from the text field of the ITM message */
{
   char *str_ptr;

   get_itm_acct_nbr(text, acct_info_ptr->acct_nbr);
   acct_info_ptr->acct_type = UNKNOWN_ACCT_TYPE;   /* FIX THIS WHEN WE KNWO HOW */

   str_ptr = (char *) strchr(text, credit_card_delimeter);
   if (str_ptr != NULL)
   {
      str_ptr++;
      strncpy(acct_info_ptr->expire_date, str_ptr, EXPIRE_LEN);      /* why isnt it 6 ?? */
      acct_info_ptr->expire_date[EXPIRE_LEN] = '\0';
   }
   else
   {
      str_ptr = (char *) strchr(text, '=');
      acct_info_ptr->expire_date[EXPIRE_LEN] = '\0';
   }
   strncpy(acct_info_ptr->amount, &text[ITM_ACCOUNT_FIELD_LEN], ITM_AMOUNT_LEN);
   acct_info_ptr->amount[ITM_AMOUNT_LEN] = '\0';
}                          /* end get_itm_acct_info */

void
get_itm_receipt_nbr(char *src_ptr, char *dest_ptr)
{
   char delimeter;
   char get_itm_msg_field();

   if (src_ptr == NULL)
      return;

   delimeter = get_itm_msg_field(src_ptr, dest_ptr, trip_data_delimeter_str);
   while ((delimeter != 'B') && (delimeter != '\0'))
   {
      delimeter = get_itm_msg_field(&src_ptr[strlen(dest_ptr)], dest_ptr, strchr(trip_data_delimeter_str, delimeter) + 1);
   }
}                    /* end get_itm_receipt_nbr*/

char
get_itm_msg_field(char *src_ptr, char *dest_ptr, char *delimeter_str)
/* get the first field matching on of the delimeters */
/* special case for FIRST_FIELD - no delimeter */
{
   char *ptr_field_end = src_ptr;
   char *ptr_field_start;
   char  delimeter;
   int   field_length;

   delimeter = '\0';
   dest_ptr[0] = '\0';

   if (delimeter_str == NULL)
      return (delimeter);

   if (delimeter_str[0] == NO_DELIMETER)
      ptr_field_start = ptr_field_end;
   else
      ptr_field_start = (char *) strpbrk(ptr_field_end, delimeter_str);

   if (ptr_field_start == NULL)
      return (delimeter);

   if (ptr_field_start[0] == CTRL_START)
      return (delimeter);

   if (delimeter_str[0] == NO_DELIMETER)
      delimeter = NO_DELIMETER;
   else
   {
      delimeter = ptr_field_start[0];
      ptr_field_start++;	/* over the field delimeter */
   }

   ptr_field_end = (char *) strpbrk(ptr_field_start, delimeter_str);
   if (ptr_field_end != ptr_field_start)	/* if not null field */
   {
      ptr_field_end--;		/* end of the data field */
      field_length = ptr_field_end - ptr_field_start + 1;
      strncpy(dest_ptr, ptr_field_start, field_length);
      dest_ptr[field_length] = '\0';
   } else			/* null field */
      dest_ptr[0] = '\0';

   return (delimeter);
}				/* end get_itm_msg_field */
