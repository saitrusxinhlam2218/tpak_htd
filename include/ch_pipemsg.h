/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  ch_pipemsg.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:44:41                      *
* @(#)  Last delta: 12/2/94 at 18:54:20                                               *
* @(#)  SCCS File: /taxi/sccs/s.ch_pipemsg.h                                                       *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS                                          *
*                                                                           *
****************************************************************************/
/* ENH0002: SamPlan message types sent over the CallHandler pipe */
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
typedef struct
{
  int type;
  int nbr;
  char str[10];
} CALLHND_PIPE_MSG;
