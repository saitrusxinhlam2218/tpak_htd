static char sz__FILE__[]="@(#)sim.c  10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  sim.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:46:30                      *
* @(#)  Last delta: 12/2/94 at 18:54:31                                               *
* @(#)  SCCS File: /taxi/sccs/s.sim.c                                                       *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS                                          *
*                                                                           *
*****************************************************************************/
		/* -----------------------------------------------------------------

    Copyright 1994 - TaxiCom Copenhagen, Denmark
    
		System   : TaxiPak
		Filename : sim.C
    Purpose  : Simulate Application Server -
				Accept test suite of AS messages to ITM
				Dump out received messages from TP 

    Modification history

    Author      Date      Revision     Description
    ---------   -------   -----------  -------------------------
		KAG         940716    0.0           Prototype

	funcs:

   ----------------------------------------------------------------- */ 

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>

#include  "mads_types.h"
#include  "mads_isam.h"
#include  "pi.h"
#include  "pi_struc.h"
#include  "sim.h"
#include  "pi_inc.h"

/* Globals */

CallInfoStuff  gsCall;
char  sDataFile[15] = "ApiTestData";

#define ERROR( x, y) ( printf( "Err %s, num (%d)\n", x,y))
void	MakeLogEntry( char *szMsg )
{
	if ( !bSilent )
	{
		printf( szProcName );
		printf(szMsg);
		printf( "\n" );
	}
	if ( log_fd != NULL )
	{
		fwrite(szProcName, strlen(szProcName), 1, log_fd );
		fwrite(szMsg, strlen(szMsg), 1, log_fd );
		fwrite("\n", 1, 1, log_fd );
		fflush( log_fd );
	}
}

  int set_options(int argc, char **argv)
  {
		int c;
		int nOptions=0;
		extern char *optarg;
		extern int optind, opterr;


		while ((c = getopt(argc, argv, "dpasf0123456789")) != EOF)
		{
      switch(c)
      {
				case 'd':
					bSendTestData = 1;
					break;

				case 'p':
					sscanf( argv[optind], "-p%d", &nServicePort );
					optarg = argv[optind+1];
					break;
case 'a':
					nSendApiTestData = 1;
					break;

				case 'f':
					sscanf( argv[optind], "-f%s",sDataFile );
					optarg = argv[optind+1];
					break;

				case 's':
					sscanf( argv[optind], "-s%d", &nDelay );
					optarg = argv[optind+1];
					break;

			} /* end switch */
		++nOptions;
		}

	if ( !nOptions )
	{
		printf("usage %s [<switch> ....]\n", argv[0] );
		printf("-d        --read&send AsTestData file to Server\n");
		printf("-p port   --service port of Server\n");
		printf("-a        --read&send AITestData file to Server\n");
		printf("-s delay  --Set delay between msgs (seconds)\n");
		printf("-i        --interpret APITestData use previous return value(call_msg) in next msg\n");
		exit(0);
	}

} /* end set_options */



 /*************************************************************************
  * Name: int InitSigHndlrs( void )                                       * 
  * Type: Function                                                        *
  *                                                                       * 
  * Purpose:  Register sig handler funcs. 								  *
 **************************************************************************/
void	InitSigHndlrs()
{
	int temp;

	for (temp=1; temp < NSIG; temp++)
		signal(temp, SIG_IGN);

	if ( signal(SIGTERM, ProcCleanup) == SIG_ERR)
		perror( "Can't install SIGUSR2\n " );

	if ( signal(SIGQUIT, ProcCleanup) == SIG_ERR)
		perror( "Can't install SIGUSR2\n " );

}

void	LogErrorMsg( int nLine, char *szFile, char *szComment, int nErrLevel )
{
	fprintf( log_fd, "%s File %s, Line %d, %s\n Desc %s\n", szProcName, szFile, nLine, szComment, strerror(errno) );
	fprintf( stderr, "%s File %s, Line %d, %s\n Desc %s\n", szProcName, szFile, nLine, szComment, strerror(errno) );
	if ( nErrLevel > NO_ERROR )
		fwrite("\n", 1, 1, log_fd );
	{
		ProcCleanup(0);
		exit( nErrLevel );
	}
}
 
void ProcInit()
{
	unlink( szLogFile );
	log_fd = fopen( szLogFile, "a");
	if ( log_fd == NULL )
	{
		sprintf( szLogMsg, "Can't open %s\n", szLogFile );
		ERROR( szLogMsg, NO_ERROR );
	}
}

void ProcCleanup(int code)
{
	fclose( log_fd );
	exit(0);
	/* Unix closes all file desciptors for us */
}

int	MakeConnect2CCP()
{
	struct	sockaddr_in	bba;
	int 	s,c;
	long	lCCPaddress;

	bzero( &bba, sizeof(bba) );
	bba.sin_family = AF_INET;
	bba.sin_port = htons( nServicePort );
	lCCPaddress = inet_addr( "127.0.0.1" );
	bcopy( (char*) &lCCPaddress, &bba.sin_addr );

	s = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( s == FAIL )
		ERROR( "Failed making socket", NO_ERROR );

	c = connect( s, &bba, sizeof(bba) );
	if ( c == FAIL )
		ERROR( "Failed to connect on AS service port", TCP_ERROR );

	sprintf( szLogMsg, "Made connection to CCP port(%d) on fd %d",
							nServicePort, s );
	MakeLogEntry( szLogMsg );		
	return s;
}


void	SendSemelTestData( int CCPfd )
{
	FILE	*fTestFile;
	char  sTestBuf[MAX_DATA_BUF];
	int   nRet;
	int 	nLine=0;

	fTestFile = fopen( sDataFile, "r" );
	if ( fTestFile == NULL )
	{
		printf( "Can't open data file (%s)\n", sDataFile );
		ERROR( "Can't open test data file (AsTestData) ", NO_ERROR );
		exit(0);
		return;
	}
	printf( "Opened data file (%s)\n", sDataFile );
	exit(0);
	printf("Sending AsTestData contents to server\n");
	while ( fgets( sTestBuf, MAX_DATA_BUF-1, fTestFile ) != NULL )
	{
		if ( strlen(sTestBuf) == 1 )
			continue;
		++nLine;
		nRet = write( CCPfd , sTestBuf, 16 );
	}
}


/********************************************
	Make binary structures for API simultation
**********************************************/

int  MakeAPIStruct( char **sSrcBuf, char *sDestBuf,  int nLine, FILE *fTestFile )
{
	short shSize;
	char  cPacNum, *ptr;
	int   nMsgType;
	int   nRet=1;

	sDestBuf[0] = '*';
	nMsgType = atoi( GetField(sSrcBuf) );
	if ( !nMsgType )
	{
		printf("Invalid msg format line (%d), type (%d)\n", nLine, nMsgType);
		return 0;
	}

	switch (nMsgType)
	{
		case PI_ZONE_ADDRESS:
			shSize = sizeof(struct call)+4;
			nRet = MakeZoneStruct( sSrcBuf, &sDestBuf[6] );
			break;

		case PI_GENERATE_EXCEPTION:
			shSize = sizeof(GenExcept)+4;
			nRet = MakeGenExcptStruct( sSrcBuf, &sDestBuf[6] );
			break;

		case PI_DISP_EXT_CALL:
			shSize = nRet = MakeAllExtDispatchStructs( sSrcBuf, &sDestBuf[6], fTestFile );
			shSize += 4;
			break;

		case PI_DISPATCH_CALL:
			shSize = sizeof(struct call)+4;
			MakeDispatchStruct( sSrcBuf, &sDestBuf[6] );
			break;

		case PI_CANCEL_CALL:
			MakeLogEntry( "Cancelling call\n" );
		case PI_GET_CALL:
		case PI_GET_CAR:
			memcpy( &sDestBuf[6], &gsCall.call_number, sizeof(gsCall.call_number) );
			shSize = sizeof( gsCall.call_number )+4;
			break;


		case PI_GET_EXCEPTIONS:
			shSize = MakeGetExceptStruct( sSrcBuf, &sDestBuf[6] );
			shSize += 4;
			break;

		case PI_REPORT_EXCEPTIONS:
			shSize = MakeRepExceptStruct( sSrcBuf, &sDestBuf[6] );
			shSize += 4;
			break;

		case PI_ACCEPT_EXCEPTION:
			shSize = MakeAccptExceptStruct( sSrcBuf, &sDestBuf[6] );
			shSize += 4;
			sprintf( szLogMsg, "Accpt excptn (%d)\n", gsCall.excpt_number );
			MakeLogEntry( szLogMsg );
			break;

		case PI_SEND_MESSAGE:
			shSize = MakeSendMsgStruct( sSrcBuf, &sDestBuf[6] );
			shSize += 4;
			break;

		case PI_GET_MESSAGE:
			memcpy( &sDestBuf[6], &gsCall.msg_number, sizeof(gsCall.msg_number) );
			shSize = sizeof( gsCall.msg_number );
			shSize += 4;
			break;

		case PI_ALIVE:
		case PI_QUIT:
			shSize = 4;
			break;

		default:
			printf("Invalid msg format line (%d), type (%d)\n", nLine, nMsgType);
			return 0;
	}

	if ( !nRet )
	{
		printf("Invalid msg format line (%d), type (%d)\n", nLine, nMsgType);
		return 0;
	}

	cPacNum = nLine;
	memcpy( &sDestBuf[1], &shSize, sizeof(short) );
	memcpy( &sDestBuf[3], &cPacNum, sizeof(char) ); /* write pac-num */
	sDestBuf[4] = 0x00;  /* Order block */
	memcpy( &sDestBuf[5], &nMsgType, sizeof(char) );  /* write msg type # */
	memcpy( &sDestBuf[shSize+2], "#", 1 );

	return shSize+3;
}

/******************************************************
	Make Report exception structure for API simultation
*******************************************************/

int  MakeGetExceptStruct( char **sSrcBuf, char *sDestBuf )
{
	GetExcpt  GetExp;

	GetExp.nType = atoi( GetField( sSrcBuf ));
	GetExp.nNumber = atoi( GetField( sSrcBuf ));
	memcpy( sDestBuf, &GetExp, sizeof(GetExp) );  /* write msg data # */
	return sizeof(GetExp);

}

/******************************************************
	Make Report exception structure for API simultation
*******************************************************/

int  MakeRepExceptStruct( char **sSrcBuf, char *sDestBuf )
{
	RepExcpt  RepExp;
	char  *sPtr = sDestBuf;

	RepExp.nType = atoi( GetField( sSrcBuf ));
	RepExp.nNumber = atoi( GetField( sSrcBuf ));
	RepExp.bExcptSwitch = atoi( GetField( sSrcBuf ) );

	memcpy( sPtr, &RepExp.nType, sizeof(int) );
	sPtr += sizeof(int);
	memcpy( sPtr, &RepExp.nNumber, sizeof(int) );
	sPtr += sizeof(int);
	memcpy( sPtr, &RepExp.bExcptSwitch, sizeof(char) );

	return sizeof(int)*2+sizeof(char);

}

/****************************************************
	Make Accept Exception structure for API simultation
******************************************************/

int MakeAccptExceptStruct( char **sSrcBuf, char *sDestBuf )
{
	AccptExcpt  AcptExp;
	char  *sPtr = sDestBuf;

	AcptExp.nNumber = atoi( GetField( sSrcBuf ));
	AcptExp.cApproval = *GetField( sSrcBuf );
	AcptExp.nNumber = gsCall.excpt_number;

	memcpy( sPtr, &AcptExp.nNumber, sizeof(long));
	sPtr += sizeof(long);
	memcpy( sPtr, &AcptExp.cApproval, sizeof(char));


	return sizeof(long)+sizeof(char);

}


/****************************************************
	Make Send Message structure for API simultation
*****************************************************/

int MakeSendMsgStruct( char **sSrcBuf, char *sDestBuf )
{
	struct message  Msg;

	Msg.fleet = *GetField( sSrcBuf );
	Msg.message_number = atoi(GetField( sSrcBuf ));
	Msg.creator = (short) atoi(GetField(sSrcBuf ));
	strcpy( Msg.creation_date, GetField(sSrcBuf) );
	strcpy( Msg.creation_time, GetField(sSrcBuf) );
	Msg.receive_group = *GetField( sSrcBuf );
	strcpy( Msg.receive_id, GetField(sSrcBuf) );
	strcpy( Msg.message_text, GetField(sSrcBuf) );

	memcpy( sDestBuf, &Msg, sizeof(Msg) );  /* write msg data # */
	return sizeof(Msg);

}


/*****************************************************
	Make Generate except structures for API simultation
******************************************************/

int  MakeGenExcptStruct( char **sBuf, char *sDestBuf  )
{
	GenExcept Excp;
	int nRv;

	Excp.nGroup = atoi(GetField(sBuf));
	Excp.nNumber = atoi(GetField(sBuf));
	memcpy( sDestBuf, &Excp, sizeof(GenExcept) );  /* write msg data # */
	return 1;
}


int  MakeAllExtDispatchStructs( char **ptr, char *sDestBuf, FILE *fTestFile )
{
	int nSize=0;
	char  *spStart;
	char  *spDest = sDestBuf;
	char  cChar;
	char  *temp_ptr;
	struct  call_ext *spCall;
	struct  cisam_cmsg *cpCmsg;
	int nMultiCalls = 0;
	int nCmsgs[3];
	char  *spAppendType;

	memset( nCmsgs, 0 , sizeof( nCmsgs ) );
	MakeExtDispatchStruct( ptr, sDestBuf );
	nSize += sizeof( struct call_ext );
	spDest += sizeof( struct call_ext );

	while( (cChar = getc(fTestFile)) == APPEND_NEXT  )
	{
		if ( kevfgets( sTestBuf, MAX_DATA_BUF-1, fTestFile ) == NULL )
			return FALSE;

		spStart = sTestBuf;
		spAppendType = GetField(&spStart);
		if ( strncmp( spAppendType, CALL_MSG, strlen(CALL_MSG)  ) == 0 )
		{
			MakeCallMsgStruct( &spStart, spDest );
			spDest += sizeof( struct cisam_cmsg );
			nSize += sizeof( struct cisam_cmsg );
			++nCmsgs[nMultiCalls];
			continue;
		}

		if ( strncmp( spAppendType, MULTI_CALL, strlen(MULTI_CALL)  ) == 0 )
		{
			MakeExtDispatchStruct( &spStart, spDest );
			spDest += sizeof( struct call_ext );
			nSize += sizeof( struct call_ext );
			++nMultiCalls;
			continue;
		}

		sprintf( szLogMsg, "Don't recognize append type (%4.4s)\n", spAppendType );
		MakeLogEntry( szLogMsg );
		exit(0);
		break;
	}

	printf( "Calls (%d), call_msg recs(%d)\n", nMultiCalls+1, nCmsgs[0]+nCmsgs[1]+nCmsgs[2] );
	printf( "Call 0: (%d) call_msgs\n", nCmsgs[0] );
	printf( "Call 1: (%d) call_msgs\n", nCmsgs[1] );
	printf( "Call 2: (%d) call_msgs\n", nCmsgs[2] );
	printf( "Saved zones ( %d, %d, %d )\n", gsCall.zone[0],
																					gsCall.zone[1], gsCall.zone[2] );

	/* Lay down saved zones in dispatch call structures! */
	/* Enter zoned values only if parameter set to 0 */
	spCall = (struct call_ext *) sDestBuf;
	temp_ptr = ( char *)sDestBuf;
	switch ( nMultiCalls )
	{
		case 0:   /* no multi calls, 1 normal call */
			if ( spCall->from_addr_zone == 0  )
				spCall->from_addr_zone = gsCall.zone[0];
			break;

		case 1:   /* 2 multi calls */
			if ( spCall->from_addr_zone == 0 )
				spCall->from_addr_zone = gsCall.zone[1];
			temp_ptr += sizeof( struct call_ext ) + sizeof( struct cisam_cmsg )*nCmsgs[0];
			spCall = ( struct call_ext *) temp_ptr;
			if ( spCall->from_addr_zone == 0 )
				spCall->from_addr_zone = gsCall.zone[0];
			break;

		case 2:   /* 3 multi calls */
			if ( spCall->from_addr_zone == 0 )
				spCall->from_addr_zone = gsCall.zone[2];

			temp_ptr += sizeof( struct call_ext ) + sizeof( struct cisam_cmsg )*nCmsgs[0];
			spCall = ( struct call_ext *) temp_ptr;
			if ( spCall->from_addr_zone == 0 )
				spCall->from_addr_zone = gsCall.zone[1];

			temp_ptr += sizeof( struct call_ext ) + sizeof( struct cisam_cmsg )*nCmsgs[1];
			spCall = ( struct call_ext *) temp_ptr;
			if ( spCall->from_addr_zone == 0 )
				spCall->from_addr_zone = gsCall.zone[0];
			break;
	}

	ungetc(cChar, fTestFile);
	return nSize;

}


int  MakeCallMsgStruct( char **sBuf, char *sDestBuf )
{
	struct cisam_cmsg c;

	c.call_nbr = atoi(GetField(sBuf));
	c.msg_nbr = *GetField(sBuf);
	c.changed_by = atoi(GetField(sBuf));
	c.last_changed = atoi(GetField(sBuf));
	c.device = *GetField(sBuf);
	c.ack = *GetField(sBuf);
	strcpy( c.msg, GetField(sBuf) );

	memcpy( sDestBuf, &c, sizeof( struct cisam_cmsg ) );
	return 1;
}

/*****************************************************
	Make Generate except structures for API simultation
******************************************************/

int  MakeExtDispatchStruct( char **sBuf, char *sDestBuf )
{
	struct call_ext c;
	int nRv;

	GetDispatchStruct( (struct call*) &c, sBuf );
	c.ftj_number = atoi(GetField(sBuf));
	c.offer_type = *GetField(sBuf);
	c.detail_type = *GetField(sBuf);
	c.msg_nbr = *GetField(sBuf);
	memcpy( sDestBuf, &c, sizeof(struct call_ext) );
	return 1;
}


int MakeZoneStruct( char **sBuf, char *sDestBuf )
{
	struct call c;
	GetDispatchStruct( &c, sBuf );
	memcpy( sDestBuf, &c, sizeof(struct call) );
	return 1;
}


int MakeDispatchStruct( char **sBuf, char *sDestBuf )
{
	struct call c;
	GetDispatchStruct( &c, sBuf );
	if ( c.from_addr_zone == 0 )
		c.from_addr_zone = gsCall.zone[0];

	memcpy( sDestBuf, &c, sizeof(struct call) );
	return 1;
}

void  GetDispatchStruct( struct call *c, char **sBuf )
{
	c->fleet = *(GetField( sBuf ));
	c->call_number = atoi( GetField( sBuf ) );
	c->priority = (short) atoi( GetField( sBuf ) );
	c->call_group = atoi( GetField( sBuf ) );
	c->call_position  = *GetField( sBuf );
	c->number_of_calls = *GetField( sBuf );
	c->number_of_vehicles = (char) atoi( GetField( sBuf ) );
	strcpy( c->call_type, GetField(sBuf) );
	strcpy( c->from_addr_street, GetField(sBuf) );
	c->from_addr_number = atoi( GetField(sBuf) );
	c->from_addr_suffix = *GetField( sBuf );
	strcpy( c->from_addr_apart, GetField(sBuf) );
	strcpy( c->from_addr_city, GetField(sBuf) );
	c->from_addr_zone = (short ) atoi( GetField(sBuf) );
	strcpy( c->from_addr_cmnt, GetField(sBuf) );
	strcpy( c->passenger, GetField(sBuf) );
	strcpy( c->phone, GetField(sBuf) );
	strcpy( c->to_addr_street, GetField(sBuf) );
	c->to_addr_number = atoi( GetField(sBuf) );
	c->to_addr_suffix = * GetField(sBuf);
	strcpy( c->to_addr_apart, GetField(sBuf) );
	strcpy( c->to_addr_city, GetField(sBuf) );
	c->to_addr_zone = atoi( GetField(sBuf) );
	strcpy( c->to_addr_cmnt, GetField(sBuf) );

	c->car_number = (short) atoi( GetField(sBuf) );
	strcpy( c->car_attrib, GetField(sBuf) );
	strcpy( c->driver_id, GetField(sBuf) );
	strcpy( c->driver_attrib, GetField(sBuf) );
	c->creator = (short) atoi( GetField(sBuf) );
	strcpy( c->creation_date, GetField(sBuf) );
	strcpy( c->creation_time, GetField(sBuf) );
	strcpy( c->due_date, GetField(sBuf) );
	strcpy( c->due_time, GetField(sBuf) );
	strcpy( c->pickup_time, GetField(sBuf) );
	strcpy( c->close_time, GetField(sBuf) );
	strcpy( c->call_comment, GetField(sBuf) );
	c->call_status = (short) atoi(GetField(sBuf));

}

char  *kevfgets( char *sBuf, int nSize, FILE *fSrcFile  )
{
	char  *spGetBuf = sBuf;
	char  *spResult;
	int   nRemainSize= nSize;


	while ( 1 )
	{
		spResult = fgets( spGetBuf, nRemainSize, fSrcFile );

		if ( spResult == NULL )   /* EOF */
			return NULL;

		if ( *sBuf == '\n' && ( nSize == nRemainSize)  )       /* empty line */
			return NULL;

		if ( sBuf[strlen(sBuf)-2] != '\\'  ) /* NO concat lines - normal fgets */
			return spResult;

		nRemainSize = nSize - strlen(sBuf) +2;
		spGetBuf += strlen(sBuf)-2;
	}

}

/*************************************************
 *  Class:    Test Data Scripts...
 *  Name:     void ReceiveNewCCPdata( int CCPfd )
 *  Function: Send Simulator scripts to API                                                                        *
 **************************************************/


int  SendApiTestData( int CCPfd )
{
	static FILE  *fTestFile;
	char  sDestBuf[MAX_DATA_LEN];
	int   nRet;
	static int   nLine=0;
	int   nSize;
	char  *ptr = sTestBuf;

	if ( nLine == 0 )
	{
		printf("Sending ApiTestData contents to server\n");
		fTestFile = fopen( "ApiTestData", "r" );
		if ( fTestFile == NULL )
		{
			ERROR( "Can't open test data file (ApiTestData) ", NO_ERROR );
			return;
		}
	}

	memset( sDestBuf, 0x00, MAX_DATA_LEN );
	ptr = kevfgets( sTestBuf, MAX_DATA_BUF-1, fTestFile );
	if ( ptr == NULL )
	{
		fclose( fTestFile );
		return 0;
	}

	if ( ptr != NULL )
	{
		if ( strlen(sTestBuf) == 1 )
			return 0;

		ptr = sTestBuf;
		nSize = MakeAPIStruct( &ptr, sDestBuf, nLine, fTestFile);
		if ( nSize )
		{
			nRet = write( CCPfd , sDestBuf, nSize );
			sprintf( szLogMsg, "Wrote test data line (%d) size (%d)",
										nLine, nSize );
			MakeLogEntry( szLogMsg );
		}

		nDelay = nDelay ? nDelay : 1;
		sleep(nDelay);

		ptr = sTestBuf;
		++nLine;
		memset( sTestBuf, 0x00, sizeof(sTestBuf) );
	}

	return 1;
}

/**************************************************************************
 * Name: void ReceiveNewCCPdata( int CCPfd )                              *
 * Type: Function                                                         *
 *                                                                        *
 **************************************************************************/

void	ReceiveNewCCPdata( int CCPfd )
{
	char  sData[MAX_DATA_BUF];
	int   nBytes;
	int   nMsgSize;
	int   x;
	char  *ptr;

	nBytes = read( CCPfd, sData, MAX_DATA_BUF-1 );
#ifdef KEVIN
	sprintf( szLogMsg, "Read (%d) bytes from CCP", nBytes ); 
	MakeLogEntry( szLogMsg );
#endif

	if (nBytes == 0 )
	{
		MakeLogEntry( "Server has closed down, exiting" );
		ProcCleanup(0);
	}

	if ( nBytes == FAIL )
	{
		ERROR( "Failed reading server data", NO_ERROR );
		return;
	}

	while( nBytes )
	{

		nMsgSize = *((short*) &sData[1]);
		nMsgSize += 3;
		if ( nBytes < nMsgSize )
		{
			sprintf( szLogMsg, "Msg size from API(%d), bytes in buf(%d)\n",
																		nMsgSize, nBytes );
			MakeLogEntry( szLogMsg );
			break;
		}

		switch( sData[4] )
		{
			case ANSWER_BLOCK_TO_CLIENT:

				if ( *((short*) &sData[6]) != 0 )  /* Error in input paramter to API */
				{
					sprintf(szLogMsg, "Received ANS_BLK data PIMSG(%d) ERROR(%d)\n",
																						sData[5], (short) sData[6] );
					break;
				}

				sprintf(szLogMsg, "Received ANS_BLK data\nPIMSG(%d) ReturnCode(%d)\n",
																					 sData[5], (short) sData[6] );
				MakeLogEntry( szLogMsg );
				ParseAnswerBlock( sData[5], &sData[8] );
				break;

			case ACK_BLOCK_TO_CLIENT:
				sprintf(szLogMsg, "Received ACK_MSG data(%d)\n", sData[5] );
				nMsgSize = 7;
				MakeLogEntry( szLogMsg );
				break;

			case NAK_BLOCK_TO_CLIENT:
				sprintf(szLogMsg, "Received NAK_MSG data(%d)\n", sData[5] );
				nMsgSize = 7;
				MakeLogEntry( szLogMsg );
				break;

			default:
				sprintf(szLogMsg, "Unknown msg (%d) data (%d)\n", sData[5], (short)sData[6] );
				MakeLogEntry( szLogMsg );
				break;

		}
		if ( nBytes < 0 )
			break;
		nBytes -= nMsgSize;
		memmove(sData, &sData[nMsgSize], nBytes );

	} /* End while - parsing buffer */

}

/**********************************************
 * Class: Data Display
 * Name: DumpMessageStruct( message *Msg )
 *
 **********************************************/
#ifdef FOO
void  DumpExceptStruct( struct exception *Exp )
{
	MakeLogEntry( "Exception Structure:\n\n" );

	sprintf( szLogMsg, "Excp Number: %ld", Exp->exception_number );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Fleet: %c", Exp->fleet );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Creation Date: %6.6s", Exp->creation_date );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Creation Time: %4.4s", Exp->creation_time );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Excp type: %d", Exp->exception_type );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Approval: %c", Exp->approval );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Zone: %d", Exp->zone_number );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Call_number: %d", Exp->call_number );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Car Number: %c", Exp->car_number );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Msg Number: %d", Exp->message_number );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Outstanding: %c", Exp->outstanding );
	MakeLogEntry( szLogMsg );
}
#endif

/**********************************************
 * Class: Data Display
 * Name: DumpMessageStruct( message *Msg )
 *
 **********************************************/

void  DumpMessageStruct( struct message *Msg )
{
	MakeLogEntry( "Message Structure:\n\n" );

	sprintf( szLogMsg, "Fleet: %c", Msg->fleet );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Message nbr: %d", Msg->message_number );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Creator: %d", Msg->creator );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Creation Date: %6.6s", Msg->creation_date );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Creation Time: %4.4s", Msg->creation_time );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Receive Group: %c", Msg->receive_group );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Receive_id: %5.5s", Msg->receive_id );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Message: %s", Msg->message_text );
	MakeLogEntry( szLogMsg );
}

/**********************************************
 * Class: Data Display
 * Name: DumpCallStruct( message *Msg )
 *
 **********************************************/

void  DumpCallStruct( struct call *cl)
{
	MakeLogEntry( "Call Structure:\n\n" );

	sprintf( szLogMsg, "Fleet: %c", cl->fleet );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Call nbr: %d", cl->call_number );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Priority: %d", cl->priority );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Call Group: %d", cl->call_group );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Call Position: %c", cl->call_position );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Num calls: %c", cl->number_of_calls );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Num Vehicles: %c", cl->number_of_vehicles );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Call_type: %5.5s", cl->call_type );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "From Street: %20.20s", cl->from_addr_street );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "From City: %3.3s", cl->from_addr_city );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "From zone: %d", cl->from_addr_zone );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "From addr_cmnt: %20.20s", cl->from_addr_cmnt );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Car Number: %d", cl->car_number );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Driver attrib: %7.7s", cl->driver_attrib );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Car attrib: %7.7s", cl->car_attrib );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Create date: %6.6s", cl->creation_date );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Create time: %6.6s", cl->creation_time );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Due date: %6.6s", cl->due_date );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Due time: %6.6s", cl->due_time );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Car attrib: %7.7s", cl->car_attrib );
	MakeLogEntry( szLogMsg );

	sprintf( szLogMsg, "Call Status: %d", cl->call_status );
	MakeLogEntry( szLogMsg );

}

/******************************************
 * Class: Data Parser                     *
 * Name: char *GetField( char **str )     *
 * Function: Parse and display returned   *
 *           data                         *
 ******************************************/

int ParseAnswerBlock( char cType, char *sData )
{
	struct call *spCallStruct;
	CarInfo     *spCarInfo;
	CallStat    *spCallSts;
	ExcpRepInfo sExcpRep;
	struct exception  *spExcept;

	int         nExcpNumber;
	int         nNumberOfExcps;
	int         x;



	switch ( cType )
	{
		case PI_ZONE_ADDRESS:
			spCallStruct = ( struct call *)sData;
			gsCall.zone[2] = gsCall.zone[1];  /* shift down in saved zone list */
			gsCall.zone[1] = gsCall.zone[0];  /* most recent in slot 0 */
			gsCall.zone[0] = spCallStruct->from_addr_zone;
			sprintf( szLogMsg, "Zone num rcvd (%d)\n", spCallStruct->from_addr_zone );
			MakeLogEntry( szLogMsg );
			break;

		case PI_GET_CALL:
			DumpCallStruct( (struct call *)sData );
			break;

		case PI_GET_CAR:
			spCarInfo = ( CarInfo *)sData;
			sprintf( szLogMsg, "Call (%d), car (%d)\n", spCarInfo->nCallNumber,
																								spCarInfo->nCarNumber );
			MakeLogEntry( szLogMsg );
			break;

		case PI_CANCEL_CALL:
			spCallSts = ( CallStat *)sData;
			sprintf( szLogMsg, "Call (%d), status(%d)\n", spCallSts->nCallNumber,
																								spCallSts->nCallStatus );
			MakeLogEntry( szLogMsg );
			break;

		case PI_DISPATCH_CALL:
		case PI_DISP_EXT_CALL:
			gsCall.call_number = *( ( int* )sData );
			sprintf( szLogMsg, "Call num rcvd (%d)\n", gsCall.call_number );
			MakeLogEntry( szLogMsg );
			break;

		case PI_SEND_MESSAGE:
			gsCall.msg_number = *( (int*) sData );
			sprintf( szLogMsg, "Msg num rcvd (%d)\n", gsCall.msg_number );
			MakeLogEntry( szLogMsg );
			break;

		case PI_GET_MESSAGE:
			DumpMessageStruct( (struct message *)sData );
			break;

		case PI_REPORT_EXCEPTIONS:
#ifdef FOO	
			sExcpRep.cOnOff = sData[0];
			sExcpRep.nExpType = *( (int*)&sData[1] );

			if (sExcpRep.cOnOff == 0 )
			{
				sprintf( szLogMsg, "Exception type (%d) is being reported for client\n",
																				sExcpRep.nExpType );

				MakeLogEntry( szLogMsg );
			}
			else
			{
				spExcept = (struct exception *)&sData[1];
				gsCall.excpt_number = spExcept->exception_number;
				DumpExceptStruct( (struct exception *)&sData[1] );
			}
#endif
			break;

		case PI_ACCEPT_EXCEPTION:
			nExcpNumber = *( (int*) sData );
			sprintf( "Exception (%d)Accepted", nExcpNumber );
			MakeLogEntry( szLogMsg );
			break;

		case PI_GET_EXCEPTIONS:
#ifdef FOO
			nNumberOfExcps = *( (int*) sData );
			sprintf( szLogMsg, "Number of exceptions (%d)\n", nNumberOfExcps );
			MakeLogEntry( szLogMsg );

			spExcept = ( struct exception *)( sData+sizeof(int) );
			gsCall.excpt_number = spExcept->exception_number;
			for( x=0; x < nNumberOfExcps; x++, spExcept++)
				DumpExceptStruct( spExcept );
#endif
			break;

		default:
			break;
	} /* end switch */
}


/******************************************
 * Class: Data Parser                     *
 * Name: char *GetField( char **str )     *
 * Type: Function                         *
 *                                        *
 ******************************************/

char *GetField(char **str)
{
	char *ptr,*par;


	for(par= *str; *par==' '; par++);
	if(*par=='"')
	{
		for(ptr=par+1; *ptr && *ptr!='"'; ptr++);
		if(*ptr) *(ptr++)=0;
		*str=ptr; return(par+1);
	}
	else
	{
		for(ptr=par; *ptr && *ptr!=' '; ptr++);
		if(*ptr) *(ptr++)=0;
		*str=ptr; return(par);
	}
}


 /**************************************************************************
  * Name: void main(int argc, char **argv)                                 * 
  * Type: Function                                                         *
  *                                                                        * 
  **************************************************************************/

void main(int argc, char **argv) 
{
	int nCCPFd;
	fd_set  ReadMask, fd_set_read;
	short bApiDataLeft = 1;

	ProcInit();
	InitSigHndlrs();
    set_options(argc, argv);

	nCCPFd = MakeConnect2CCP();
	if ( bSendTestData )
		SendSemelTestData(nCCPFd);



	FD_ZERO( &ReadMask );
	FD_SET( nCCPFd, &ReadMask );

	MakeLogEntry("Starting the AS_sim process for use on TaxiPak.");

	while(1) 
	{
		fd_set_read = ReadMask;
		if ( nSendApiTestData && bApiDataLeft )
			bApiDataLeft = SendApiTestData(nCCPFd);


    	switch(select(FD_SETSIZE, &fd_set_read, NULL, NULL, NULL))
    	{
      	case (-1):
			MakeLogEntry( "Failure on select loop" ); 
			break;

		case (0):
			break;

		default: /* A socket is ready for reading */

       		if ( FD_ISSET(nCCPFd, &fd_set_read) ) 
       			ReceiveNewCCPdata(nCCPFd);
	
      } /* end switch */ 

   } /* end while(1) */
} /* end main() */

