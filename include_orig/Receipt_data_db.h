/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Receipt_data_db.h,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 1998/11/16 13:28:57 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Receipt_data_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Receipt_data_db.h,v $	$Revision: 1.4 $"

#ifndef _RECEIPT_DATA_DB_H
#define _RECEIPT_DATA_DB_H

#include <isam.h>

#define RECEIPT_ITM_TEXT_LEN  256
#define RECEIPT_DATE_LEN 8     /** 08.08.98 **/
#define RECEIPT_TIME_LEN 8     /** 02:45:00 **/


/** TRIP_DATA message structure defines **/
#define TD_RCPT_PAY_SIZE       1
#define TD_RCPT_NBR_SIZE       4
#define TD_RCPT_DATE_SIZE      6
#define TD_AMOUNT_SIZE         5
#define TD_EXTRAS_SIZE         4
#define TD_DISTANCE_SIZE       5
#define TD_START_TIME_SIZE     4
#define TD_STOP_TIME_SIZE      4
#define TD_TRIP_TYPE_SIZE      2
#define TD_DRIVER_NBR_SIZE     6
#define TD_CAR_NBR_SIZE        8
#define TD_BIZ_NBR_SIZE        7
#define TD_CREDIT_CARD_SIZE    40
#define TD_CALL_NBR_SIZE       8

#define TD_RCPT_PAY_OFFSET     0
#define TD_RCPT_NBR_OFFSET     (TD_RCPT_PAY_OFFSET + TD_RCPT_PAY_SIZE)
#define TD_RCPT_DATE_OFFSET    (TD_RCPT_NBR_OFFSET + TD_RCPT_NBR_SIZE)
#define TD_AMOUNT_OFFSET       (TD_RCPT_DATE_OFFSET + TD_RCPT_DATE_SIZE)
#define TD_EXTRAS_OFFSET       (TD_AMOUNT_OFFSET + TD_AMOUNT_SIZE)
#define TD_DISTANCE_OFFSET     (TD_EXTRAS_OFFSET + TD_EXTRAS_SIZE)
#define TD_START_TIME_OFFSET   (TD_DISTANCE_OFFSET + TD_DISTANCE_SIZE)
#define TD_STOP_TIME_OFFSET    (TD_START_TIME_OFFSET + TD_START_TIME_SIZE)
#define TD_TRIP_TYPE_OFFSET    (TD_STOP_TIME_OFFSET + TD_STOP_TIME_SIZE)
#define TD_DRIVER_NBR_OFFSET   (TD_TRIP_TYPE_OFFSET + TD_TRIP_TYPE_SIZE)
#define TD_CAR_NBR_OFFSET      (TD_DRIVER_NBR_OFFSET + TD_DRIVER_NBR_SIZE)
#define TD_BIZ_NBR_OFFSET      (TD_CAR_NBR_OFFSET + TD_CAR_NBR_SIZE)
#define TD_CREDIT_CARD_OFFSET  (TD_BIZ_NBR_OFFSET + TD_BIZ_NBR_SIZE)
#define TD_CALL_NBR_OFFSET     (TD_CREDIT_CARD_OFFSET + TD_CREDIT_CARD_SIZE)


typedef struct cisam_rd
{
  int       serial_nbr;
  int       trans_dt_tm;
  int       veh_nbr;
  int       itm_trans_nbr;
  char      date[ RECEIPT_DATE_LEN + 1 ];
  char      time[ RECEIPT_TIME_LEN + 1 ];
  int       amount;
  int       extras;
  int       distance;
  char      start_time[ TD_START_TIME_SIZE + 1 ];
  char      stop_time[ TD_STOP_TIME_SIZE + 1 ];
  char      trip_type[ TD_TRIP_TYPE_SIZE + 1 ];
  char      driver_nbr[ TD_DRIVER_NBR_SIZE + 1 ];
  char      car_nbr[ TD_CAR_NBR_SIZE + 1 ];
  char      biz_nbr[ TD_BIZ_NBR_SIZE + 1 ];
  char      credit_card[ TD_CREDIT_CARD_SIZE + 1 ];
  char      itm_text[RECEIPT_ITM_TEXT_LEN + 1];
  int       call_nbr;
  char      processed;
  char      payment_type;
} RECEIPT_DATA_REC;

#ifdef KEY_SRC
struct keydesc rd_key1 = {
  ISNODUPS,
  2,
  { { 8, LONGSIZE, LONGTYPE },
      { 12, LONGSIZE, LONGTYPE } }
};

struct keydesc rd_key2 = {
  ISNODUPS,
  1,
  { { 0, LONGSIZE, LONGTYPE } }
};

#else
extern struct keydesc rd_key1;
extern struct keydesc rd_key2;
#endif

#endif
