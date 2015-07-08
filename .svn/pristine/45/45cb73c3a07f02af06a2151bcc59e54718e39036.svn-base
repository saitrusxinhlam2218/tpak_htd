/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call_msg_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:42:30 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Call_msg_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _CALL_MSG_DB_H
#define _CALL_MSG_DB_H
#include <isam.h>
typedef struct    cisam_cmsg
{
					int   call_nbr;            /* Unique record-ID for every TaxiPak call */
					char  msg_nbr;             /* Number assigned externally by Ftj/Samplan for each trip/device */
					int   changed_by;          /* future use        */
					int   last_changed;        /* future use        */
					char  device;              /* device to send msg to (1-3)(prn/itm/mdt)      */
					char  ack;                 /* language dependent (j/n) field if device ack rcvd */
					char  msg[251];            /* data/text for device  */
} CMSG_REC;
#define CMSG_CALL_NBR 0
#define CMSG_MSG_NBR 4
#define CMSG_DEVICE 13
#ifdef KEY_SRC
struct keydesc cmsg_key1= {
				 ISNODUPS,
				 2,
				 {{CMSG_CALL_NBR,LONGSIZE,LONGTYPE},
				  {CMSG_MSG_NBR,CHARSIZE,CHARTYPE}}
};
/* Composite of (call_nbr, device) */
struct keydesc cmsg_key2= {
				 ISNODUPS,
				 2,
				 {{CMSG_CALL_NBR,LONGSIZE,LONGTYPE},
				  {CMSG_DEVICE,CHARSIZE,CHARTYPE}}
};
#else
extern struct keydesc cmsg_key1;
extern struct keydesc cmsg_key2;
#endif
#endif
