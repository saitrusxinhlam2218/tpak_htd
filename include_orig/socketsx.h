/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: socketsx.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2002/03/25 19:32:49 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/socketsx.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: socketsx.h,v $	$Revision: 1.2 $"

#ifndef  TRANS_DATA_SIZE 
#define MAX_TCP_DATA    1460
#define  SND_DATA_SZ    MAX_TCP_DATA  /* max sending data size/packet */
#define  RCV_DATA_SZ    MAX_TCP_DATA  /* max rcv'ing data size/packet */
#define  MAX_TRANS_SIZE  RCV_DATA_SZ         /* max. transaction size */
#define  TRANS_HDR_SIZE  (4 * sizeof(int))
#define  TRANS_DATA_SIZE MAX_TRANS_SIZE-TRANS_HDR_SIZE
typedef struct transact       /* transaction struct for mirror/reflection proc */ 
{ 	int size;                                /* size of the transaction */ 
	int action;                               /* action type */ 
	int file;                                 /* type of file */ 
	int rec_no;                               /* record no */ 
	char data[TRANS_DATA_SIZE];               /* actual data */
} REQHDR;
#else
typedef struct transact REQHDR;
#endif
 
#define MAXDATA         TRANS_DATA_SIZE
#define MAXREQ		sizeof(REQHDR)
#define HDRSIZE		(MAXREQ-(MAXDATA))

#define TSUCCESS	0
#define TFAIL		1
#define THANDSHAKE 	2
#define TGETTIME	3
#define TGETPWD		4
#define TRANDOM		5

#define NETUP 	1
#define NETDOWN 0
#define MSERVICE 1


extern int NetStat;

fatal();
CloseConnection();
int death();
int MakeConnection();
CreateSocket();
OpenSocket();
HandleConnection();
REQHDR * RcvRequest();
SndResponse();
int MakeRequest();
REQHDR * AllocRequest();
FreeRequest();
GetConnectName();
