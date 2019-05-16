/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: pi_test.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1999/04/12 18:49:07 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/pi/pi_test.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: pi_test.c,v $	$Revision: 1.3 $"

static char rcsid[] = "$Id: pi_test.c,v 1.3 1999/04/12 18:49:07 taxiadm Exp $";

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <netdb.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include "../pi/pi.h"
#include "../pi/pi_struc.h"

#define PORT_PI         3000
#define SERVICE_PI      "pi"
#define HOST_PI         "localhost"
char szLogMsg[512];
char PacNum = '0';

void	MakeLogEntry( char *szMsg )
{
	{
		fprintf(stderr, "%s",szMsg);
		fprintf(stderr,"\n" );
	}
}

main()
{
  struct hostent *pHent;
  struct servent *pSent;
  struct utsname hname;
  int num_socket;
  struct sockaddr_in sinRemote;

  char tmp_buf[12];
  int send_length, rc;
  char         APIMsg[ BUFSIZ ];
  int          msg_length, TES_type;
  char         msgType[ BUFSIZ ];
  char         RecvBuf[ BUFSIZ ];
  char         *pRecvBuf;
  int          done = 0;


  bzero( RecvBuf, sizeof( RecvBuf ) );
  
  uname( &hname );

  
  pSent = getservbyname( SERVICE_PI, "tcp" );
  if ( pSent == NULL )
    {
      printf( "Error acquiring tcb network service\n");
      exit(-1 );
    }
  else sinRemote.sin_port = pSent->s_port;

  if ( ( num_socket = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
      printf( "Error acquiring socket\n");
      exit(-1);
    }

  sinRemote.sin_family = AF_INET;

  pHent = gethostbyname( HOST_PI );
  if ( pHent == NULL )
    {
      close( num_socket );
      printf("Error acquiring host name \n" );
      exit( -1 );
    }

  memcpy( &sinRemote.sin_addr, pHent->h_addr_list[0], sizeof( struct in_addr ) );

  if ( connect( num_socket,
	       ( struct sockaddr * ) &sinRemote,
	       sizeof( struct sockaddr ) ) == -1 )
    {
      close( num_socket );
      printf( "Error performing connect\n" );
      return ( -1 );
    }

  pi_test_zone_address( num_socket );
  
  //pi_test_zone_info( num_socket );

  
}

pi_test_zone_address( int sockfd )
{
  char      pi_buf[ BUFSIZ ];
  int       rc = 0;
  int       nMsgType;
  short     MsgSize;
  int       cl_nbr;


  PI_CALL_REC cl_rec;

  bzero( &cl_rec, sizeof(PI_CALL_REC) );
  bzero( pi_buf, BUFSIZ );

  /** fill in some test call values **/
  pi_test_fill_call( &cl_rec );

  pi_buf[0] = '*';
  memcpy( &pi_buf[6], &cl_rec, sizeof( PI_CALL_REC ) );
  MsgSize = sizeof( PI_CALL_REC ) + 4;
  memcpy( &pi_buf[1], &MsgSize, sizeof(short) );
  memcpy( &pi_buf[3], &PacNum, sizeof(char));
  ++PacNum;
  pi_buf[4] = 0x00;
  nMsgType = 1;
  memcpy( &pi_buf[5], &nMsgType, sizeof(char) );
  memcpy( &pi_buf[MsgSize+2], "#", 1);

  rc = send( sockfd, pi_buf, MsgSize + 4 );

  bzero( pi_buf, BUFSIZ );
  rc = ReceivePIdata( sockfd, pi_buf );

  cl_nbr = *(int *)&pi_buf[8];

  bzero( pi_buf, BUFSIZ );
  pi_buf[0] = '*';
  memcpy( &pi_buf[6], &cl_nbr, sizeof( int ) );
  MsgSize = sizeof( int ) + 4;
  memcpy( &pi_buf[1], &MsgSize, sizeof(short) );
  memcpy( &pi_buf[3], &PacNum, sizeof(char));
  ++PacNum;
  pi_buf[4] = 0x00;
  nMsgType = 4;
  memcpy( &pi_buf[5], &nMsgType, sizeof(char) );
  memcpy( &pi_buf[MsgSize+2], "#", 1);

  rc = send( sockfd, pi_buf, MsgSize + 4 );

  bzero( pi_buf, BUFSIZ );
  rc = ReceivePIdata( sockfd, pi_buf );
}

pi_test_call( int sockfd )
{
  char      pi_buf[ BUFSIZ ];
  int       rc = 0;
  int       nMsgType;
  short     MsgSize;
  int       cl_nbr;


  PI_CALL_REC cl_rec;

  bzero( &cl_rec, sizeof(PI_CALL_REC) );
  bzero( pi_buf, BUFSIZ );

  /** fill in some test call values **/
  pi_test_fill_call( &cl_rec );

  pi_buf[0] = '*';
  memcpy( &pi_buf[6], &cl_rec, sizeof( PI_CALL_REC ) );
  MsgSize = sizeof( PI_CALL_REC ) + 4;
  memcpy( &pi_buf[1], &MsgSize, sizeof(short) );
  memcpy( &pi_buf[3], &PacNum, sizeof(char));
  ++PacNum;
  pi_buf[4] = 0x00;
  nMsgType = 2;
  memcpy( &pi_buf[5], &nMsgType, sizeof(char) );
  memcpy( &pi_buf[MsgSize+2], "#", 1);

  rc = send( sockfd, pi_buf, MsgSize + 4 );

  bzero( pi_buf, BUFSIZ );
  rc = ReceivePIdata( sockfd, pi_buf );

  cl_nbr = *(int *)&pi_buf[8];

  bzero( pi_buf, BUFSIZ );
  pi_buf[0] = '*';
  memcpy( &pi_buf[6], &cl_nbr, sizeof( int ) );
  MsgSize = sizeof( int ) + 4;
  memcpy( &pi_buf[1], &MsgSize, sizeof(short) );
  memcpy( &pi_buf[3], &PacNum, sizeof(char));
  ++PacNum;
  pi_buf[4] = 0x00;
  nMsgType = 4;
  memcpy( &pi_buf[5], &nMsgType, sizeof(char) );
  memcpy( &pi_buf[MsgSize+2], "#", 1);

  rc = send( sockfd, pi_buf, MsgSize + 4 );

  bzero( pi_buf, BUFSIZ );
  rc = ReceivePIdata( sockfd, pi_buf );
}

pi_test_ext_call( int sockfd )
{

  char      pi_buf[ BUFSIZ ];
  int       rc = 0;
  int       nMsgType;
  short     MsgSize;
  int       cl_nbr;
  CALL_EXT_REC cl_rec;
  CALL_MSG_REC cl_msg_rec;

  bzero( &cl_msg_rec, sizeof( CALL_MSG_REC ) );
  bzero( &cl_rec, sizeof(CALL_EXT_REC) );
  bzero( pi_buf, BUFSIZ );

  /** fill in some test call values **/
  pi_test_fill_ext_call( &cl_rec );
  /* put in some call_msg values */
  cl_msg_rec.call_nbr = 0;
  cl_msg_rec.msg_nbr = '1';
  cl_msg_rec.changed_by = 0;
  cl_msg_rec.last_changed = 0;
  cl_msg_rec.device = '2';
  strcpy( cl_msg_rec.msg, "MESSAGE TO THE ITM" );

  pi_buf[0] = '*';
  memcpy( &pi_buf[6], &cl_rec, sizeof( CALL_EXT_REC ) );
  memcpy( &pi_buf[6+sizeof(CALL_EXT_REC)], &cl_msg_rec, sizeof(CALL_MSG_REC) );
  MsgSize = sizeof( CALL_EXT_REC ) + 4 + 4 + sizeof( CALL_MSG_REC );
  memcpy( &pi_buf[1], &MsgSize, sizeof(short) );
  memcpy( &pi_buf[3], &PacNum, sizeof(char));
  ++PacNum;
  pi_buf[4] = 0x00;
  nMsgType = 13;
  memcpy( &pi_buf[5], &nMsgType, sizeof(char) );
  memcpy( &pi_buf[MsgSize+2], "#", 1);

  rc = send( sockfd, pi_buf, MsgSize + 4 );

  bzero( pi_buf, BUFSIZ );
  rc = ReceivePIdata( sockfd, pi_buf );

  cl_nbr = *(int *)&pi_buf[8];

  bzero( pi_buf, BUFSIZ );
  pi_buf[0] = '*';
  memcpy( &pi_buf[6], &cl_nbr, sizeof( int ) );
  MsgSize = sizeof( int ) + 4;
  memcpy( &pi_buf[1], &MsgSize, sizeof(short) );
  memcpy( &pi_buf[3], &PacNum, sizeof(char));
  ++PacNum;
  pi_buf[4] = 0x00;
  nMsgType = 4;
  memcpy( &pi_buf[5], &nMsgType, sizeof(char) );
  memcpy( &pi_buf[MsgSize+2], "#", 1);

  rc = send( sockfd, pi_buf, MsgSize + 4 );

  bzero( pi_buf, BUFSIZ );
  rc = ReceivePIdata( sockfd, pi_buf );

}

  
pi_test_zone_info( int sockfd )
{
  char      pi_buf[ BUFSIZ ];
  int       rc = 0;
  int       nMsgType;
  short     MsgSize;
  ZONE_INFO_REC zone_info;
  typedef struct _ZONE_INFO_REPLY
    {
      short        return_value;
      char       fleet;
      short      zone_nbr;
      short      veh_primary_zone;
      short      veh_backup_zones;
      char       veh_attr[32];
      char       drv_attr[32];
    } ZONE_INFO_REPLY;
  ZONE_INFO_REPLY   *pZoneInfoReply;
  order             *pOrder;

  bzero( &zone_info, sizeof( ZONE_INFO_REC ) );

  zone_info.fleet = 'A';
  zone_info.zone_nbr = 172;
  zone_info.veh_primary_zone = 0;
  zone_info.veh_backup_zones = 0;
  strcpy(zone_info.veh_attr, "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN");
  strcpy(zone_info.drv_attr, "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN");

  pi_buf[0] = '*';
  memcpy( &pi_buf[6], &zone_info, sizeof(ZONE_INFO_REC) );
  MsgSize = sizeof( ZONE_INFO_REC ) + 4;
  memcpy( &pi_buf[1], &MsgSize, sizeof(short) );
  memcpy( &pi_buf[3], &PacNum, sizeof(char));
  ++PacNum;
  pi_buf[4] = 0x00;
  nMsgType = 20;
  memcpy( &pi_buf[5], &nMsgType, sizeof(char) );
  memcpy( &pi_buf[MsgSize + 2], "#", 1 );
  rc = send( sockfd, pi_buf, MsgSize + 4 );
  bzero( pi_buf, BUFSIZ );
  ReceivePIdata(sockfd, pi_buf );
}

pi_test_exception( int sockfd )
{
  char      pi_buf[ BUFSIZ ];
  int       rc = 0;
  int       nMsgType;
  short     MsgSize;
  GenExcept excpt;

  bzero( &excpt, sizeof(GenExcept) );
  bzero( pi_buf, BUFSIZ );

  excpt.nGroup = 1;
  excpt.nNumber = 33;

  pi_buf[0] = '*';
  memcpy( &pi_buf[6], &excpt, sizeof( GenExcept ) );
  MsgSize = sizeof( GenExcept ) + 4;
  memcpy( &pi_buf[1], &MsgSize, sizeof(short) );
  memcpy( &pi_buf[3], &PacNum, sizeof(char));
  ++PacNum;
  pi_buf[4] = 0x00;
  nMsgType = 14;
  memcpy( &pi_buf[5], &nMsgType, sizeof(char) );
  memcpy( &pi_buf[MsgSize+2], "#", 1);
  
  rc = send( sockfd, pi_buf, MsgSize + 4 );
  bzero( pi_buf, BUFSIZ );
  ReceivePIdata( sockfd, pi_buf );
}

pi_test_message( int sockfd )
{
  char      pi_buf[ BUFSIZ ];
  MSG_REC   tm;
  typedef struct _MSG_REPLY_
    {
      short   return_val;
      int     message_nbr;
    } MSG_REPLY;
  MSG_REPLY   *pMsgReply;
  order       *pOrder;
  int       rc = 0;
  int       nMsgType;
  short     MsgSize;

  bzero( &tm, sizeof( MSG_REC ) );
  bzero( pi_buf, BUFSIZ );

  pi_buf[0] = '*';


  tm.fleet = 'A';
  tm.message_number = 99998;
  tm.creator = 9999;
#ifdef FOO
  strcpy( tm.creation_date, "98-06-22" );
  strcpy( tm.creation_time, "1230" );
#endif
  tm.receive_group = 'F';
  strcpy(tm.receive_id, "1" );
  strcpy(tm.message_text, "A TEST MESSAGE TO A 6 DIGIT DRIVER");

  memcpy( &pi_buf[6], &tm, sizeof( MSG_REC ) );
  MsgSize = sizeof( MSG_REC ) + 4;
  memcpy( &pi_buf[1], &MsgSize, sizeof(short) );
  memcpy( &pi_buf[3], &PacNum, sizeof(char));
  ++PacNum;
  pi_buf[4] = 0x00;
  nMsgType = 9;
  memcpy( &pi_buf[5], &nMsgType, sizeof(char) );
  memcpy( &pi_buf[MsgSize+2], "#", 1);
  
  rc = send( sockfd, pi_buf, MsgSize + 4 );
  bzero( pi_buf, BUFSIZ );
  ReceivePIdata( sockfd, pi_buf );

  pOrder = (order *)pi_buf;
  pMsgReply = (MSG_REPLY *)&pOrder->data[1];
  
}

pi_test_fill_call( PI_CALL_REC *pcl )
{
  pcl->fleet = 'H';
  pcl->call_number = 0;
  pcl->priority = 20;
  pcl->call_group = 0;
  pcl->call_position = '0';
  pcl->number_of_calls = '0';
  pcl->number_of_vehicles = 0x01;
  strcpy(pcl->call_type, "M    ");
  strcpy(pcl->from_addr_street, "SILTASAARENKATU");
  pcl->from_addr_number = 8;
  strcpy(pcl->from_addr_city,"HEL");
  pcl->from_addr_zone = 814;
  strcpy(pcl->from_addr_cmnt, "HELLO I'M A COMMENT");
  strcpy(pcl->passenger, "MICKEY \\\\\\\\" );
  strcpy(pcl->phone, "911" );
  strcpy(pcl->car_attrib,"NNNNNNNN");
  strcpy(pcl->driver_attrib,"NNNNNNNN");
#ifdef FOO
  strcpy(pcl->creation_date,"980624");
  strcpy(pcl->creation_time,"1620");
  strcpy(pcl->due_date,"980624");
  strcpy(pcl->due_time,"1624");
#endif
  strcpy(pcl->call_comment, "CALL COMMENT HERE");
}

pi_test_fill_ext_call( CALL_EXT_REC *pcl )
{
  pcl->fleet = 'A';
  pcl->call_number = 0;
  pcl->priority = 20;
  pcl->call_group = 0;
  pcl->call_position = '0';
  pcl->number_of_calls = '0';
  pcl->number_of_vehicles = 0x01;
  strcpy(pcl->call_type, "P    ");
  strcpy(pcl->from_addr_street, "STAMGATAN");
  pcl->from_addr_number = 6;
  strcpy(pcl->from_addr_city,"STO");
  pcl->from_addr_zone = 814;
  strcpy(pcl->from_addr_cmnt, "HELLO I'M A COMMENT");
  strcpy(pcl->passenger, "MICKEY \\\\\\\\" );
  strcpy(pcl->phone, "911" );
  strcpy(pcl->car_attrib,"NNNNNNNN");
  strcpy(pcl->driver_attrib,"NNNNNNNN");
#ifdef FOO
  strcpy(pcl->creation_date,"980624");
  strcpy(pcl->creation_time,"1620");
#endif
  strcpy(pcl->due_date,"980924");
  strcpy(pcl->due_time,"1624");
  strcpy(pcl->call_comment, "CALL COMMENT HERE");
  pcl->ftj_number = 334455;
  pcl->offer_type = '0';
  pcl->detail_type = '2';
  pcl->msg_nbr = '1';
}

ReceivePIdata( int sockfd, char *pi_buf )
{
        char  sData[BUFSIZ];
	int   nBytes;
	int   nMsgSize;
	int   x;
	char  *ptr;

	nBytes = read( sockfd, sData, BUFSIZ-1 );

	sprintf( szLogMsg, "Read (%d) bytes from PI", nBytes ); 
	MakeLogEntry( szLogMsg );


	if (nBytes == 0 )
	{
		MakeLogEntry( "Server has closed down, exiting" );
		exit(-1);
	}

	if ( nBytes == -1 )
	{
		MakeLogEntry( "Failed reading server data" );
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
	memcpy( pi_buf, sData, BUFSIZ );

}

