/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  sim.h; Rel: 10.1.2.1; Get date: 4/19/95 at 08:46:34                      *
* @(#)  Last delta: 12/2/94 at 18:54:33                                               *
* @(#)  SCCS File: /taxi/sccs/s.sim.h                                                       *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS                                          *
*                                                                           *
*****************************************************************************/

/**********************************
  System defines
***********************************/
typedef unsigned char UCHAR;
#define TCP_ERROR   1
#define NO_ERROR    0
#define MAX_DATA_BUF  4096
#define FAIL  -1
#define IPADDR  "192.009.220.120"
#define ANS_BLK 1
#define ACK     2
#define NAK     3


/*************************************
  File scope globals for Simulator
*************************************/
char  szProcName[] = "AS_SIM : ";
char  szLogFile[]  = "sim.log";
int   bSendTestData = 0;
int   bSendHellos = 0;
int   nServicePort=3000;
int   nDelay=0;
int   nSendApiTestData=0;
int   bSilent = 0;
char  sTestBuf[MAX_DATA_BUF];

FILE  *log_fd;
char szLogMsg[100];

/******************************
  FORMAT SYNTAX
*******************************/

#define CALL_MSG    "CALL_MSG"
#define MULTI_CALL  "MULT_CALL"
#define APPEND_NEXT     '+'

/*********************************
  structs
*********************************/

/* Saved return parameters */
typedef struct cist
{
  int   zone[3];
  int   call_number;
  int   msg_number;
  long  excpt_number;
} CallInfoStuff;

/* GetException message */
typedef struct gecp
{
  int nType;
  int nNumber;
} GetExcpt;

/* Report Exception message */
typedef struct recp
{
  int   nType;
  int   nNumber;
  char  bExcptSwitch;
} RepExcpt;

/* Accept exception message */
typedef struct aecp
{
  long  nNumber;
  char  cApproval;
} AccptExcpt;

/* Return exception status */
typedef struct cs
{
  int   nCallNumber;
  short nCallStatus;
} CallStat;

/* Return car info */
typedef struct gc
{
  int   nCallNumber;
  short nCarNumber;
} CarInfo;

/* Return excp info */
typedef struct ei
{
  char  cOnOff;
  int   nExpType;
} ExcpRepInfo;

/* Return excp stat */
typedef struct es
{
  int   nNumber;
  int   cStat;
} ExcpStat;


/************************************
  prototypes
*************************************/
void ProcCleanup(int code);
int  MakeAPIStruct( char **sSrcBuf, char *sDestBuf,  int nLine, FILE *fTestFile );
int  MakeGenExcptStruct( char **sBuf, char *sDestBuf  );
int  MakeAllExtDispatchStructs( char **ptr, char *sDestBuf, FILE *fTestFile );
int  MakeCallMsgStruct( char **sBuf, char *sDestBuf );
int  MakeExtDispatchStruct( char **sBuf, char *sDestBuf );
int  MakeDispatchStruct( char **sBuf, char *sDestBuf );
void GetDispatchStruct( struct call *c, char **sBuf );
int  SendApiTestData( int CCPfd );
void  ReceiveNewCCPdata( int CCPfd );
char *GetField(char **str);
char  *kevfgets( char *sBuf, int nSize, FILE *fSrcFile  );









