/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ui_call_msg.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/06/18 14:44:45 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/ui_call_msg.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: ui_call_msg.h,v $	$Revision: 1.3 $"

#ifndef __UI_CALL_MSG
#define __UI_CALL_MSG

typedef struct {
  long   isam_rec_nbr;
  int    call_nbr;
  char   msg_nbr;
  int    changed_by;
  int    last_changed;
  char   device;
  char   ack;
  char   message1[ 76 ];
  char   message2[ 76 ];
} CL_MSG_REC;

#define CL_MSG_DEV_ROW         2
#define CL_DEV1_ROW            2
#define CL_DEV2_ROW            5
#define CL_DEV3_ROW            8
#define CL_MSG_DEV_COL         4
#define CL_MSG_DEV_DESCRIPT_ROW 2
#define CL_MSG_DEV_DESCRIPT_COL 22
#define CL_MSG_ACK_COL         58
#define CL_MSG_COL             7
#define CL_MSG_ACK_ROW         2
#define CL_MSG1_ROW1           3
#define CL_MSG1_ROW2           4
#define CL_MSG2_ROW1           6
#define CL_MSG2_ROW2           7
#define CL_MSG3_ROW1           9
#define CL_MSG3_ROW2           10



#define CL_DEV1      1
#define CL_MSG_1_1   ( CL_DEV1 + 1 )
#define CL_MSG_1_2   ( CL_MSG_1_1 + 1 )
#define MAX_CL_MSG_PIU     CL_MSG_1_2
#define MIN_CL_MSG_PIU     CL_DEV1


#endif
