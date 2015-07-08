/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  itm_missing.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:46:53
* @(#)  Last delta: 12/2/94 at 18:40:16
* @(#)  SCCS File: /taxi/sccs/s.itm_missing.h
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/
#ifndef _ITM_MISSING_H
#define _ITM_MISSING_H
/* file itm_missing.h */
/* itm missing receipt data structures etc */
/* started 9 July 90 */
/* last update 17 Sept 90 */
/* M. R. Bradley */
typedef struct missing_receipt_bin{
   int receipt_nbr_first;    /* oldest in  bin*/
   int receipt_nbr_last;     /* newest in bin */
   int receipt_nbr_current;  /* current adddressed nbr in bin */
   int receipt_nbr_cnt;      /* how many */
   int retry_cnt;            /* how many times have we tried */
   struct missing_receipt_bin *next;
   struct missing_receipt_bin *previous;
} MISSING_RECEIPT_BIN, *MISSING_RECEIPT_BIN_HNDL;
struct itm_struct{
     int receipt_count;
     int last_receipt_nbr;        /* receieved unsolicited */
     int last_requested_receipt_nbr;  /* for ITM errors */
     int nbr_missing_receipts;
     int nbr_receipts_to_request;  /* how many reqceipts can we request */
     int request_status;
     int relative_cnt;
     short trip_indicator;        /* whether missing receipts are for current trip */
     struct timers *missing_receipt_timer;
     MISSING_RECEIPT_BIN missing_receipt_nbrs;
     int invoice_retry_count;
};
      
#ifdef STANDALONE
struct veh_driv{
     int foobar;
     struct itm_struct itm_info;
};
#endif
#define EARLIER  1
#define LATER    2
#define ADD_TO   3
#define EXISTS   4
#define CONTINUE 5
#define MAJOR(x) (x / 100)
#define SHARE(x) (x % 100)
/* for the transaction log */
#define ITM_REQUESTED 1
#define ITM_RECEIVED  2
#define ITM_LOST      3
#define ITM_NO_LOG    4
#define ITM_REQUESTED_STR  "REQUEST"
#define ITM_RECEIVED_STR   "RECEIVED"
#define ITM_LOST_STR       "LOST"
/* how many time we retry the request before we give up */
/* should be a fleet parameter */
/* #define RECEIPT_RETRY_MAX 3 */
/* how many relative receipt requests */
#define MAX_RELATIVE_RECEIPT_REQUESTS 255
/* request status (itm_info) */
#define ITM_REQUEST_STATUS_NONE  0   /* all ok */
#define ITM_REQUEST_STATUS_RETRY 1   /* keep trying */
#define ITM_REQUEST_STATUS_ERROR 2   /* cant get em */
/* itm_info.trip_indicator */
#define TRIP_COMPLETE_RECEIVED     0
#define TRIP_COMPLETE_NOT_RECEIVED 1
#endif
