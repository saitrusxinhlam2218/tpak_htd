 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  pi_inc.h; Rel: 10.1.2.2; Get date: 4/19/95 at 08:46:01
 * @(#)  Last delta: 12/14/94 at 10:34:58
 * @(#)  SCCS File: /taxi/sccs/s.pi_inc.h
 *                                                                           *
 *****************************************************************************/
/* -------------------------------------------------------------------------------
   Copyright 1993 - TaxiCom Copenhagen, Denmark

   System	:	PLANET - LINK
   Filename	:	pi_inc.h
   Purpose	:	Statics and Client Server 

Modification history

Author	Date	Revision	Description
------  ------- --------------- -----------------------------------------------
LG      931125  0.0		Function-paramters/return-values.

------------------------------------------------------------------------------- */

#ifndef _H_PI_INC
#define _H_PI_INC

/*STAT DEFINES */
#define	MAX_SAMPLES	10
#define	PEAK_TIME	10
#define	MIN_TIME	11
#define	SAMPLE_SPACE	12	/* Highest array slot for stats */
#define NUM_STAT_FUNCS  15
#define	ALL_FUNCS	-1
#define	STAT_FILE	"./PiStatistics"

/* SYSTEM DEFINES */
#define	FAIL	-1
#define	bTRUE	1
#define bFALSE  0

/* EXIT DEFINES */
#define	RESOURCE_PROBLEM	2
#define	CLIENT_TERMINATES	3
#define	TAXI_ERROR			4
#define CLIENT_SOCK_WRITE	5
#define	SIGNAL_EXIT			6

/* CHILD */
#define	NO_PI_CHILD		-1
#define	MAX_CHILD		17

/* License */
#define	MSGS_PER_DAY	800
#define	EXPIRE_DAY		18
#define	EXPIRE_MONTH	4

/* HeartBeats */
#define PARENT_HB_SOCK  0
#define CHILD_HB_SOCK   1
#define HB_TO_SERVER_TIME  15  /* in seconds */
#define BEAT  "BBBBEAT"


typedef struct  ci
{
	int     nPid;
	int     nFd;
	int     nHeartBeatPipe;
	time_t  lLastHeartBeat;
	short   bResponded;
}	ChildInfo;

typedef struct	tcs
{
	long	time[ SAMPLE_SPACE ];
	int		nNextSlot;
}TaxiStats;

/* Message Protocol */

#define ORDER_DATA_START   6

#define PI_SCAN_WF_HEAD    1
#define PI_SCAN_WF_LEN1    2
#define PI_SCAN_WF_LEN2    3
#define PI_SCAN_WF_PACNUM  4
#define PI_SCAN_WF_MSG1    5
#define PI_SCAN_WF_MSG2    6
#define PI_SCAN_WF_DATA    7
#define PI_SCAN_WF_TAIL    8
#define HEAD_CHAR          42
#define TAIL_CHAR          35

struct socket_inf 
{
  int fd;
  unsigned char in_data[MAX_DATA_LEN];
  unsigned char out_data[MAX_DATA_LEN];
  int  try_send_out_data;
  int  next_in_data_pos;
  int  next_out_data_pos;
  int  scan_state;
  int  internal_pacnum;
  int  last_out_pacnum;
  int  last_res_pacnum;
  int  this_pacnum;
  int  in_data_len;  /* From PacNum to Tail */
  int  out_data_len; /* All the data hold in out_data */
  int  block_type;
  int  order_num;
  int  illegal_block;
};

int ValidFtjCallRec( struct cisam_cmsg *cmsg, return_type *return_data );
int ValidFtjInfoCallRec( CALL_EXT_REC *cur_call, return_type *return_data  );
int ValidCallRecord( PI_CALL_REC *cur_call, return_type *return_data );
void MakeAckMessage( unsigned char *temp_data,struct socket_inf *Client,
								struct return_struc return_data );
void  MakePiExcept( int nPid, int nType );
void  MakeSysExcept( int nPid, int  nInfo );
void DumpBuf( char *szBuf, int nSize );
int ParseAnswerBlock( char cType, char *sData );
void  DumpExceptStruct( PI_EXCEPT_REC *Exp );
void  DumpMessageStruct( MSG_REC *Msg );
void  DumpCallStruct( PI_CALL_REC *cl );

#endif




