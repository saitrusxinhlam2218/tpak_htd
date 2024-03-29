static char sz__FILE__[]="@(#)itm_receipt.c  10.1.2.2(ver) Date Release 5/24/95" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  itm_receipt.c; Rel: 10.1.2.2; Get date: 5/24/95 at 11:39:04                      *
* @(#)  Last delta: 5/24/95 at 11:38:59                                               *
* @(#)  SCCS File: /taxi/sccs/s.itm_receipt.c                                                       *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS                                          *
*                                                                           *
*****************************************************************************/

/****************************************************************************/
/* file itm_receipt.c 			                                    */
/* routines to handle itm receipt processing                                */
/* TaxiPak R4                                                               */
/* started 3 July 1990                                                      */
/* last update 14 Sept 1990                                                 */
/* M. R. Bradley 			                                    */
/****************************************************************************/

#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>

#include "acc.h"
#include "mads_types.h"
#include "language.h"
#include "mad_error.h"
#include "veh_driv.h"
#include "itm.h"
#include "path.h"
#include "msg_mmp.h"
#include "timers.h"
#include "except.h"
#include "mmp.h"
#include "tmplen.h"
#include "enhance.h"
#include "switch_ext.h"
#include "Receipt_data_db.h"
#include "taxi_db.h"
#include "GPS_private.h"

extern int itm_flag;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifdef DEBUG_R4
char last_receipt_nbr[RECEIPT_NBR_SIZE + TMP_STR_LEN];
int   mdebug = 0;		/* to simulate missing receipts */
#endif

extern char file_name[64];
extern FILE *receipt_fp;
extern time_t	midnight;

extern struct fleets *fleet[];	/* Global fleet structure */
extern struct veh_driv *get_veh_ptr();
extern int warm_start;
extern char req_buf[];
extern char *scratch_ptr;

char str_temp[TMP_STR_LEN];



log_itm_receipt_db( veh_num, receipt_buf )
     int veh_num;
     char *receipt_buf;
{
  RECEIPT_DATA_REC   receipt_data;
  int rc = 0;
  char tmp_rcpt[ 255 ];
  char *pGPSdata;

  bzero( &receipt_data, sizeof( RECEIPT_DATA_REC ) );
  strncpy( tmp_rcpt, (receipt_buf + TD_RCPT_NBR_OFFSET), TD_RCPT_NBR_SIZE );
  tmp_rcpt[ TD_RCPT_NBR_SIZE ] = '\0';
  receipt_data.itm_trans_nbr = atoi( tmp_rcpt );
  receipt_data.veh_nbr = veh_num;

  /** check if already logged in database **/
  if ( db_read_key( RECEIPT_DATA_FILE_ID, &receipt_data, &rd_key1, ISEQUAL ) == SUCCESS )
    return( -1 );
  
  receipt_data.trans_dt_tm = time(0);
  get_srch_date( receipt_data.trans_dt_tm, receipt_data.date );
  get_hhmmss_time( receipt_data.trans_dt_tm, receipt_data.time );


  /** Amount **/
  strncpy( tmp_rcpt, (receipt_buf + TD_AMOUNT_OFFSET), TD_AMOUNT_SIZE );
  tmp_rcpt[ TD_AMOUNT_SIZE ] = '\0';
  receipt_data.amount = atoi( tmp_rcpt );
  /** Extras **/
  strncpy( tmp_rcpt, (receipt_buf + TD_EXTRAS_OFFSET), TD_EXTRAS_SIZE );
  tmp_rcpt[ TD_EXTRAS_SIZE ] = '\0';
  receipt_data.extras = atoi( tmp_rcpt );
  /** Distance **/
  strncpy( tmp_rcpt, (receipt_buf + TD_DISTANCE_OFFSET), TD_DISTANCE_SIZE );
  tmp_rcpt[ TD_DISTANCE_SIZE ] = '\0';
  receipt_data.distance = atoi( tmp_rcpt );
  /** Start Time **/
  strncpy( tmp_rcpt, (receipt_buf + TD_START_TIME_OFFSET), TD_START_TIME_SIZE );
  tmp_rcpt[ TD_START_TIME_SIZE ] = '\0';
  strcpy( receipt_data.start_time, tmp_rcpt );
  /** Stop Time **/
  strncpy( tmp_rcpt, (receipt_buf + TD_STOP_TIME_OFFSET), TD_STOP_TIME_SIZE );
  tmp_rcpt[ TD_STOP_TIME_SIZE ] = '\0';
  strcpy( receipt_data.stop_time, tmp_rcpt );
  /** Trip Type **/
  strncpy( tmp_rcpt, (receipt_buf + TD_TRIP_TYPE_OFFSET), TD_TRIP_TYPE_SIZE );
  tmp_rcpt[ TD_TRIP_TYPE_SIZE ] = '\0';
  strcpy( receipt_data.trip_type, tmp_rcpt );
  /** Driver Nbr **/
  strncpy( tmp_rcpt, (receipt_buf + TD_DRIVER_NBR_OFFSET), TD_DRIVER_NBR_SIZE );
  tmp_rcpt[ TD_DRIVER_NBR_SIZE ] = '\0';
  strcpy( receipt_data.driver_nbr, tmp_rcpt );
  /** Car Nbr **/
  strncpy( tmp_rcpt, (receipt_buf + TD_CAR_NBR_OFFSET), TD_CAR_NBR_SIZE );
  tmp_rcpt[ TD_CAR_NBR_SIZE ] = '\0';
  strcpy( receipt_data.car_nbr, tmp_rcpt );
  /** Biz Nbr **/
  strncpy( tmp_rcpt, (receipt_buf + TD_BIZ_NBR_OFFSET), TD_BIZ_NBR_SIZE );
  tmp_rcpt[ TD_BIZ_NBR_SIZE ] = '\0';
  strcpy( receipt_data.biz_nbr, tmp_rcpt );
  /** Credit Card **/
  strncpy( tmp_rcpt, (receipt_buf + TD_CREDIT_CARD_OFFSET), TD_CREDIT_CARD_SIZE );
  tmp_rcpt[ TD_CREDIT_CARD_SIZE ] = '\0';
  strcpy( receipt_data.credit_card, tmp_rcpt );
  /** Call Nbr **/
  strncpy( tmp_rcpt, (receipt_buf + TD_CALL_NBR_OFFSET), TD_CALL_NBR_SIZE );
  tmp_rcpt[ TD_CALL_NBR_SIZE ] = '\0';
  receipt_data.call_nbr = atoi(tmp_rcpt);                
  
  receipt_data.veh_nbr = veh_num;
  receipt_data.processed = NO;
  
  if ( ( pGPSdata = strstr( receipt_buf, GPS_DELIMITER ) ) != NULL )
    *pGPSdata = '\0';

  if ( ( pGPSdata = strstr( receipt_buf, FIELD_END ) ) != NULL )
    *pGPSdata = '\0';
  
  strcpy(receipt_data.itm_text, receipt_buf);
  if ( strstr( receipt_data.itm_text, "%E" ) )
    strcpy( strstr( receipt_data.itm_text, "%E"), "\0");	  
  /* Payment Type */
  receipt_data.payment_type = *(receipt_buf + TD_RCPT_PAY_OFFSET);

  if ( ( rc = db_write( RECEIPT_DATA_FILE_ID, &receipt_data ) ) != SUCCESS )
    {
      ERROR(' ', "", "Error writing RECEIPT_DATA_FILE" );
      return(FAIL);
    }
  else
    return(SUCCESS);
}

/*******************************************************************/
/* log_itm_receipt()                                               */
/* accept the trip data                                            */
/* log it                                                          */
/* if there are access problems make an exception                  */
/*******************************************************************/

int log_itm_receipt(veh_num, term_id, receipt,  call_number)
   short veh_num;
   char *term_id;
   char *receipt;
   int call_number;

{
   int   result,i,j;
   char exp_str[512];
   char data_str[512];
   char text[512];

   strcpy( text, receipt );
   while (text[i] != 0)
     i++;

   i = expand( exp_str, text, i - 4 );

  /******************
   * get BCD string *
   ******************/
  j = 0;
  while (i && j< 200)
  {
    sprintf(&data_str[2*j],"%c%c", (exp_str[j] & 0xF0)/16+'0',(exp_str[j] & 0x0F)+'0');
    i--;
    j++;
  }
  data_str[2*j]=0;       /* terminate string */
  
   /* check to see if it is time to close the current archive	*/
   chk_midnite();

   if (receipt_fp == NULL)
   {
      if ((receipt_fp = fopen(RECEIPT_TRANSACTION_FILE, "a")) == NULL)
      {
	 ERROR(' ', "", "Can't open RECEIPT_TRANSACTION_FILE");
	 /* make an exception */
	 return(-1);
      }
   }

   if ((0) && (call_number == -1))
	sprintf(str_temp, "%04d %*s %11s %s", (int)veh_num, MID_SIZE, term_id, "*", receipt);
   else
	sprintf(str_temp, "%04d %*s %11d %s", (int)veh_num, MID_SIZE, term_id, call_number, receipt);

   result = fprintf(receipt_fp,"%s", str_temp); /* ENH0002. Added "%s" to allow str_temp to include the '%' character */

   /* Check errno to see if write was successful */
   /* if not we want to make sure that the ACK   */
   /* is not sent                                */
   if ( errno == ENOSPC )
     return( -1 );
   
   fflush(receipt_fp);
   strcpy(req_buf, str_temp);


   return(0);
}				/* end log_itm_receipt */

/********************************************************/
/* void                                                 */
/* init_veh_itm_info                                    */
/* initialize the itm unique data in the vehicle struct */
/********************************************************/

void
init_itm_info(veh_ptr, veh_db)
   struct veh_driv *veh_ptr;
   struct cisam_vh *veh_db;
{
   veh_ptr->invoice_retry_count = 0;
}				/* end init_itm_info */
