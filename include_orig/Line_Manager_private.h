/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Line_Manager_private.h,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2002/09/10 19:25:21 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Line_Manager_private.h,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Line_Manager_private.h,v $	$Revision: 1.5 $"

#ifndef _LINE_MGR_PRIVATE_H
#define _LINE_MGR_PRIVATE_H

#include      <sys/types.h>
#include      <signal.h>

#define       CSTA_MSG_SIZE_SZ        2
#define       CSTA_MSG_TYPE_SZ        2
#define       CSTA_MSG_REF_SZ         4
#define       CSTA_MSG_CODE_SZ        3

/** Message Type Definitions **/
#define       MT_COMMAND              "hc" /* Line_Manager --> TAO */
#define       MT_RESPONSE_ACK         "hr" /* TAO --> Line_Manager */
#define       MT_RESPONSE_NAK         "he" /* TAO --> Line_Manager */
#define       MT_EVENT                "ev" /* TAO --> Line_Manager */

/** CSTA Object Definitions **/
#define       CSTA_OBJ_DEVICE         'P'
#define       CSTA_OBJ_QUEUE          'Q'
#define       CSTA_OBJ_CALL           'C'
#define       CSTA_OBJ_MONITOR        'M'
#define       CSTA_OBJ_AGENT          'N'
#define       CSTA_OBJ_TELE_NBR       '#'
#define       CSTA_OBJ_SZ              5

/** CSTA Command Codes **/
#define       CSTA_CMD_DIVERT_CALL     "08"
#define       CSTA_CMD_MAKE_CALL       "10"
#define       CSTA_CMD_PARK_CALL       "18"
#define       CSTA_CMD_MONITOR_START   "71"
#define       CSTA_CMD_MONITOR_STOP    "73"

/** CSTA Events **/
#define       CSTA_EVENT_CONFERENCED      "23"
#define       CSTA_EVENT_CONNECT_CLEAR    "24"
#define       CSTA_EVENT_DELIVERED        "25"
#define       CSTA_EVENT_DIVERTED         "26"
#define       CSTA_EVENT_CALL_ESTABLISHED "27"
#define       CSTA_EVENT_FAILED           "28"
#define       CSTA_EVENT_ORIGINATED       "31"
#define       CSTA_EVENT_CALL_QUEUED      "32"
#define       CSTA_EVENT_RETRIEVED        "33"
#define       CSTA_EVENT_TRANSFERRED      "35"
#define       CSTA_EVENT_AGENT_BUSY       "56"
#define       CSTA_EVENT_LOGGED_ON        "51"
#define       CSTA_EVENT_LOGGED_OFF       "52"
#define       CSTA_EVENT_NOT_READY        "53"
#define       CSTA_EVENT_READY            "54"
#define       CSTA_EVENT_WORKING_AFTER_CALL "55"


/** CSTA Causes **/
#define       CSTA_CAUSE_NEW_CALL              "C22"
#define       CSTA_CAUSE_NORMAL_CLEARING       "C48"
#define       CSTA_CAUSE_REDIRECTED            "C28"
#define       CSTA_CAUSE_ACTIVE_MONITOR        "C1"
#define       CSTA_CAUSE_BUSY                  "C3"
#define       CSTA_CAUSE_CALL_NOT_ANSWERED     "C10"
#define       CSTA_CAUSE_CALL_PICKUP           "C11"
#define       CSTA_CAUSE_DEST_NOT_OBTAINABLE   "C13"
#define       CSTA_CAUSE_FORCED_PAUSE          "C43"
#define       CSTA_CAUSE_MAKE_CALL             "C44"
#define       CSTA_CAUSE_NETWORK_CONGESTION    "C20"
#define       CSTA_CAUSE_NETWORK_NOT_OBTAINABLE "C21"
#define       CSTA_CAUSE_NO_AVAILABLE_AGENTS   "C23"
#define       CSTA_CAUSE_NUMBER_CHANGED        "C50"
#define       CSTA_CAUSE_PARK                  "C25"
#define       CSTA_CAUSE_RES_NOT_AVAIL         "C30"
#define       CSTA_CAUSE_TIMEOUT               "C56"
#define       CSTA_CAUSE_TRANSFER              "C32"


#define       TTY_NAME_SZ               2
#define       SERVER_NAME_SZ            31
#define       USER_NAME_SZ              32
#define       MAX_TTYS                  200
#define       MAX_CALLS                 100

#define       QUEUE_SIZE                5000

#define       HANGUP_LINE_SUPPLEMENT    5000

#define       BUF_SIZ                   512
#define       TELEPHONE_NBR_LEN         20
#define       AGENT_LEN                 4

#define       LINE_MGR_SHMEM_KEY        1172

typedef struct _LINE_MGR_IPC
{
  pid_t       UIPid;
  int         index;
  int         Command;
  long        CallNbr;
  char        CallObject[ CSTA_OBJ_SZ + 1 ];
  char        TelephoneNbr[ TELEPHONE_NBR_LEN + 1 ];
} LINE_MGR_IPC;

#define       UI_IPC_CMD_REGISTER       1000
#define       UI_IPC_CMD_DEREGISTER     1001
#define       UI_IPC_CMD_DIVERT         1002
#define       UI_IPC_CMD_PARK           1003
#define       UI_IPC_CMD_MAKE_CALL      1004

/** Maximum Number of Calls in Queue **/
#define MAX_CLS        100

typedef struct _CALL_LIST_STRUCT
{
  time_t             EstablishedTime;
  char               CallID[CSTA_OBJ_SZ + 1];
  long               TPakCallNbr;
  short              CallState;
} CALL_LIST_STRUCT;

#define MAX_PIDS 10

typedef struct _LINE_MGR_STRUCT
{
  char                    TTYName[SERVER_NAME_SZ + 1];
  char                    UserName[ USER_NAME_SZ + 1 ];
  int                     UserID;
  CALL_LIST_STRUCT        CallAssigned;
  pid_t                   UIPid[MAX_PIDS];
  char                    NewCall;
  char                    MonitorName[CSTA_OBJ_SZ + 1];
  char                    DDSName[CSTA_OBJ_SZ + 1];
  char                    AgentName[ AGENT_LEN + 1];
  char                    EventPending[BUF_SIZ];
  char                    TelephoneNbr[TELEPHONE_NBR_LEN + 1];
  time_t                  Established;
} LINE_MGR_STRUCT;

LINE_MGR_STRUCT   *Lines[ MAX_TTYS ];

typedef struct _QUEUE_STRUCT
{
  time_t                  QueueTime;
  long                    TPakCallNbr;
  char                    CallID[ CSTA_OBJ_SZ + 1 ];
  char                    TLG;
  short                   uid;
} TAO_QUEUE_STRUCT;

TAO_QUEUE_STRUCT              *TaoCallQueue[ QUEUE_SIZE ];

#define CALL_NEW_ESTABLISHED_STATE        1000
#define CALL_DIVERT_ESTABLISHED_STATE     1001
#define CALL_QUEUED_STATE                 1002
#define CALL_MAKECALL_ESTABLISHED_STATE   1003

#define CALLOUT_NO                        1001
#define CALLOUT_YES                       1002

struct LineMgr_offsets {
  time_t             init_time;
  LINE_MGR_STRUCT    *Line_Mgr_Array;
  CALL_LIST_STRUCT   *Call_List;
};

struct LineMgr_offsets    *pLineMgr_offset;

typedef struct _ACK_CMD_ELEMENT
{
  int        RefCmdNbr;
  char       CommandType[ 2 ];
  int        LineIndex;
  int        Status;
} ACK_CMD_ELEMENT;

ACK_CMD_ELEMENT   CommandList[ 1000 ];

#define STATUS_COMPLETE     0
#define STATUS_PENDING      1




  
#define TAO_HOST  "CSEE_2"
#define TAO_SERVICE "tao_2"
#define LINE_MGR_PIPE "/usr/taxi/run/line_mgr_pipe"
#define LINE_MGR_TRACE "/usr/taxi/run/traces/line_mgr_trace"


#endif





