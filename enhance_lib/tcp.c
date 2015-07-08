static char sz__FILE__[]="@(#)tcp.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  tcp.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:36:38		            *			
* @(#)  Last delta: 12/2/94 at 12:07:46						    *	
* @(#)  SCCS File: /taxi/sccs/s.tcp.c							    *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS					    *
*                                                                           *
*****************************************************************************/

/* -----------------------------------------------------------------
    Copyright 1994 - TaxiCom Copenhagen, Denmark
    

    System   : TaxiPak Enhancement Library 
    Filename : tcp.c
    Purpose  : Common routines to handle client/server funcs 

    Modification history

    Author      Date      Revision     Description
    ---------   -------   -----------  -------------------------
    KAG         940521    0.0         	Prototype 

	funcs:

	int	SetUpListenSock(int nServicePort)
  	int WriteSockData( unsigned char *buf, int buf_len, nClientFd)
	int HandleNewSocket(int nListenSock, fd_set *ReadMask, int *nClientFd  ) 
	int	SetUpListenSock()

   ----------------------------------------------------------------- */ 

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>

#include "enhance_com.h"
#include  "err_inc.h"
#include "tcp_inc.h" 

static char new_szLogMsg[5120];

  /**********************************************************************************
  * Name: int WriteSockData( unsigned char *buf, int buf_len, int nClientFd)	   	*
  * Purpose: To send buf_len bytes from buf out on socket s.               			*
  *                                                                        			*
  * Returnvalue: -1    -> Illegal socket. Socket is not open.              			*
  *              0     -> Nothing send at all.                             			*
  *              n > 0 -> n bytes have been send out on socket s.          			*
  *                                                                        			*
  * con: The connection to send data on.                                   			*
  * buf: The buffer where the data to send is stored.                      			*
  * buf_len: The amount of bytes to send from buf.                         			*                   
  ***********************************************************************************/

  int WriteSockData( unsigned char *buf, int buf_len, int nClientFd)
  {
    fd_set writeset;
    struct timeval select_timeout;
    int data_send = 0;
    unsigned char temp_string[256];
	int	nRet=0;

	if ( nClientFd == 0 )
	{
		MakeLogEntry( "Client not connected -- throwing away message" );
		return bFALSE;
	}

    if (nClientFd < 0)
	{
		sprintf( new_szLogMsg, "Invalid socket fd <con:%d>",
												nClientFd);
		MakeLogEntry( new_szLogMsg );
		MakeLogEntry( "Exiting SOCK:Invalid file descriptor" );
		ENH_ERROR("Exiting SOCK:Invalid file descriptor", CLIENT_SOCK_WRITE);
	}


    FD_SET( nClientFd, &writeset);
    select_timeout.tv_sec = 7; /* timeout on writing is 7 seconds */
    select_timeout.tv_usec = 0;

	switch (select(FD_SETSIZE, NULL, &writeset, NULL, &select_timeout))
 	{
		
		case FAIL:
			sprintf( new_szLogMsg, "Unable to write to socket <con:%d>", nClientFd);
			MakeLogEntry( new_szLogMsg );
			ENH_ERROR("Can't write to an open client socket", NO_ERROR);
			return bFALSE;

		case (0):
			sprintf( new_szLogMsg, "Timeout on a write to a socket<con:%d>", 
														nClientFd );
			MakeLogEntry( new_szLogMsg );
       		return bFALSE;

       	default:

       		nRet = write(nClientFd, buf, buf_len);
			if ( nRet != buf_len )
			{
				sprintf( new_szLogMsg, "Unable to write full message to socket <con:%d, msg sz %d>",
													nClientFd, buf_len );
				ENH_ERROR( new_szLogMsg, CLIENT_SOCK_WRITE );
				exit(CLIENT_SOCK_WRITE);
			}
			data_send = nRet;
#ifdef NDEBUG
			sprintf( new_szLogMsg, "Wrote (%d of %d) bytes to Client", nRet, buf_len);
			MakeLogEntry( new_szLogMsg );
#endif							

      } /* end switch */

    return data_send;

  } 

 /**************************************************************************
  * Name: int HandleNewSocket(int nListenSock, fd_set *ReadMask) 		   *
  * Type: Function                                                         *
  *                                                                        * 
  * Purpose: Handle connect request by On-Line AS 						   *
  *                                                                        *
  * Returnvalue: 0 -> Not done.                                            *
  *              1 -> Done.                                                *
  *                                                                        *
  * Desc.		 Update read mask with new client -- accept/send data	   *
  **************************************************************************/

int HandleNewSocket(int nListenSock, fd_set *ReadMask, int *nClientFd  ) 
{
   	int length, accept_fd, temp, connection_flags;
   	struct sockaddr incomming;
	int	nRet, i=0;


  	length = sizeof(incomming);
   	accept_fd = accept( nListenSock, (struct sockaddr*) &incomming, &length);

	if ( *nClientFd != 0 )
	{
		MakeLogEntry( "No more clients allowed to connect" ); 
		close( accept_fd );
		return bFALSE;
	}

	if ( accept_fd == FAIL )
		ENH_ERROR( "Accepting client", TCP_ERROR );

	/* Set new file descriptor */
	sprintf( new_szLogMsg, "Accepted new AS client on fd (%d)", accept_fd );
	MakeLogEntry( new_szLogMsg );
	*nClientFd = accept_fd;	
	FD_SET( accept_fd, ReadMask );
	return bTRUE;
}

 /**************************************************************************
  * Name: int GetClientData( int *ClientFd, int nTaxiPakFd)                *
  * Type: Function                                                         *
  *                                                                        * 
  * Purpose: Read socket and call message handler 						   *    
  *                                                                        *
  * Returnvalue: bFALSE -> Client closes down (removed in this function).  *
  *              bTRUE  -> Read data ( result in spBuf )				   * 
  *				 bFAIL  -> error reading data ( see error printout ) 	   *
  *               														   * 
  *	Notes: Use BufferInfo or cast pointer to struct						   * 
  *						( first fields must match )						   *
  **************************************************************************/

int GetClientData( int *ClientFd, BufInfo *spBuf )
{
    int nBytesRead;

	if ( *ClientFd < 1 )
	{
		MakeLogEntry( "Failed to read client Data( FD is invalid ) " );
		return bFAIL;
	}

    nBytesRead = read(*ClientFd, spBuf->cpOffset , spBuf->nBytesLeft); 
#ifdef NDEBUG
	sprintf( new_szLogMsg, "Read, %d bytes from client", nBytesRead );
	MakeLogEntry( new_szLogMsg );
	sprintf( new_szLogMsg, "Client Msg Data -> %.*s", nBytesRead, spBuf->cpOffset);
	MakeLogEntry( new_szLogMsg ); 
#endif

	switch ( nBytesRead )
	{
		case 0:
			sprintf( new_szLogMsg, "Client shutdown on fd (%d)", *ClientFd );
			MakeLogEntry( new_szLogMsg );
			close( *ClientFd );
			*ClientFd = 0;
			return bFALSE;

		case FAIL:
			sprintf( new_szLogMsg, "Error reading on fd (%d)", *ClientFd );
			ENH_ERROR( new_szLogMsg, NO_ERROR );
			return bFAIL;

		default:
			spBuf->nSize += nBytesRead;
			spBuf->cpOffset += nBytesRead;
			spBuf->nBytesLeft -= nBytesRead;
			break;

	}
	return bTRUE;
}

 /**************************************************************************
  * Name: int SetUpListenSock( void )                                      * 
  * Type: Function                                                         *
  *                                                                        * 
  * Purpose:  Setup a socket to listen to clients .                        *
  **************************************************************************/

int	SetUpListenSock( int nServicePort )
{
	int s;
  	struct protoent *prot_p;
  	struct sockaddr_in sin;
	struct linger xlinger;
	int intval=1;	
	int secs;

	/* Check if TCP is possible */
	if (!(prot_p = getprotobyname("ip"))) 
		ENH_ERROR( "Was not able to find the ip-protocol", RESOURCE_PROBLEM);

  	MakeLogEntry("IP-protocol found");
  	endprotoent();
 
	/* Create socket, bind and listen to it */ 
  	if ((s = socket(AF_INET, SOCK_STREAM, 0 )) < 0) 
    	ENH_ERROR( "Was not able to use socket", RESOURCE_PROBLEM);
  	else
	{
    	sprintf( new_szLogMsg, "Service (%d) have been selected.", nServicePort);
		MakeLogEntry( new_szLogMsg );
	}

	intval = sizeof(struct linger);
	if (getsockopt(s, SOL_SOCKET, SO_LINGER, &xlinger, &intval) < 0)
	  ENH_ERROR("Not able to get sockopt", RESOURCE_PROBLEM);
	intval = sizeof(struct linger);
	xlinger.l_onoff=0;
	xlinger.l_linger=0;	
	if (setsockopt(s, SOL_SOCKET, SO_LINGER, &xlinger, intval) < 0)
	  ENH_ERROR("CreateSocket -- setsockopt failed", RESOURCE_PROBLEM);
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &intval, sizeof(int)) < 0)
	  ENH_ERROR("setsockopt failed", RESOURCE_PROBLEM);
	secs = 1;
	if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &secs, sizeof(secs)) < 0)
	  ENH_ERROR("**TCP** -- setsockopt failed", RESOURCE_PROBLEM);
	secs = 20;
	if (setsockopt(s, IPPROTO_TCP, TCP_KEEPIDLE, &secs, sizeof(secs)) < 0)
	  ENH_ERROR("**TCP** -- setsockopt KEEPALIVE failed", RESOURCE_PROBLEM);
	secs = 1;
	if (setsockopt(s, IPPROTO_TCP, TCP_KEEPCNT, &secs, sizeof(secs)) < 0)
	  ENH_ERROR("**TCP** -- setsockopt KEEPALIVE failed", RESOURCE_PROBLEM);	
	secs = 20;
	if (setsockopt(s, IPPROTO_TCP, TCP_KEEPINTVL, &secs, sizeof(secs)) < 0)
	  ENH_ERROR("**TCP** -- setsockopt KEEPALIVE failed", RESOURCE_PROBLEM);

  	sin.sin_family        = AF_INET;
  	sin.sin_addr.s_addr   = htonl(INADDR_ANY);
  	sin.sin_port          = htons( nServicePort );

  	if (bind(s, (struct sockaddr*) &sin, sizeof(sin)) < 0)
	  {
	    ENH_ERROR( "Was not able to bind socket to address", RESOURCE_PROBLEM);
	    sleep(15);
	    exit(-1);
	  }
  	else
    	MakeLogEntry("Socket is bound to address.");
  
  	if ( listen(s, 5) < 0 ) 	/* set limit for waiting connections */
		ENH_ERROR( "Can't listen on socket", RESOURCE_PROBLEM);
	
	sprintf( new_szLogMsg, "Listening on (%d) socket", s );
	MakeLogEntry( new_szLogMsg );
	return s;
}


