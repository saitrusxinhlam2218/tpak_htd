/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call_msg_private.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/11/06 02:04:52 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Call_msg_private.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _CALL_MSG_PRIVATE_H
#define _CALL_MSG_PRIVATE_H
#define MAX_CALL_MSG      10000
#define RQ_TRIP_DETAILS 1
#define ASSIGN_RESEND 2
#define CAN_CALL 3
#define CM_ACK 4
#define CM_NACK 5
#define DISP_OVRR 6
#define CM_UPDATE 7
/* ENH0002: devices used for Call Messages */
#define MDT     '0'
#define METER   '1'
#define PRINTER '2'
typedef struct _call_msg_struct
{
  char             type;
  int              call_nbr;
  char             msg_nbr;
  char             ack_nak;
  struct _call_msg_struct *next;
} CALL_MSG_STRUCT;
#endif
