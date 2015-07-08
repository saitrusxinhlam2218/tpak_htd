/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  tcp_inc.h; Rel: 10.1.0.0; Get date: 4/18/95 at 16:36:40		            *			
* @(#)  Last delta: 12/2/94 at 12:16:04						    *	
* @(#)  SCCS File: /taxi/sccs/s.tcp_inc.h							    *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS					    *
*                                                                           *
*****************************************************************************/
/* -----------------------------------------------------------------

    Copyright 1994 - TaxiCom Copenhagen, Denmark
    

    System   : TaxiPak Enhancement Library 
    Filename : tcp_inc.h
    Purpose  : Header info to tcp object 

    Modification history

    Author      Date      Revision     Description
    ---------   -------   -----------  -------------------------
    KAG         940521    0.0         	Prototype 

	Prototypes for external linking

   ----------------------------------------------------------------- */ 

#include <sys/types.h>

typedef struct bi
{
	UCHAR 	*cpStart;
	UCHAR	*cpOffset;
	int		nSize;
	int		nBytesLeft;
} BufInfo;

int WriteSockData( unsigned char *buf, int buf_len, int nClientFd);
int HandleNewSocket(int nListenSock, fd_set *ReadMask, int *nClientFd  ) ;
int GetClientData( int *ClientFd, BufInfo *spBuf );
int	SetUpListenSock( int nServicePort );
extern char szLogMsg[];
