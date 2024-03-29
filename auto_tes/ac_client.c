/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ac_client.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2001/10/03 17:57:34 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/auto_tes/ac_client.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: ac_client.c,v $	$Revision: 1.2 $"

static char rcsid[] = "$Id: ac_client.c,v 1.2 2001/10/03 17:57:34 taxiadm Exp $";


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
#include "Auto_TES.h"

#ifdef TESTING
#define PORT_TES_SERVER         6989
#define SERVICE_TES_SERVER      "TES_test_svc"
#else
#define PORT_TES_SERVER         6988
#define SERVICE_TES_SERVER      "TES_svc"
#endif

#define HOST_TES_SERVER         "tyson"

main()
{
  struct hostent *pHent;
  struct servent *pSent;
  struct utsname hname;
  int num_socket;
  struct sockaddr_in sinRemote;

  char tmp_buf[12];
  int send_length, rc;
  AC_MSG       *ClientMsg;
  TES_MSG      *TESMsg;
  char         APIMsg[ BUFSIZ ];
  int          msg_length, TES_type;
  char         msgType[ TES_TYPE_LEN + 1 ];
  char         RecvBuf[ BUFSIZ ];
  char         *pRecvBuf;
  int          done = 0;
  int          got_first_one = 0, got_second_one = 0;
  char         cmdMsg[ 16 ];


  bzero( RecvBuf, sizeof( RecvBuf ) );
  
  uname( &hname );

  
  pSent = getservbyname( SERVICE_TES_SERVER, "tcp" );
  if ( pSent == NULL )
    sinRemote.sin_port = htons( PORT_TES_SERVER );
  else sinRemote.sin_port = pSent->s_port;

  if ( ( num_socket = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
      printf( "Error acquiring socket\n");
      exit(-1);
    }

  sinRemote.sin_family = AF_INET;

  pHent = gethostbyname( HOST_TES_SERVER );
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

  done = 0;
  while ( !done )
    {
      printf( "Enter AutoCall API Message --> " );
      gets( APIMsg );

      if ( atoi( APIMsg ) == 108 )
      	DoDetailedCall( APIMsg );

      strncpy( cmdMsg, APIMsg, 3 );
      SendAutoCall( num_socket, APIMsg );

    restart:
      bzero( RecvBuf, sizeof( RecvBuf ) );
      if ( ( rc = read( num_socket, RecvBuf, AC_MSG_SIZE_LEN ) ) <= 0 )
	exit ( -1 );
      
      msg_length = (int) strtol( RecvBuf, ( char ** ) 0, 16 );
      pRecvBuf = &RecvBuf[AC_MSG_SIZE_LEN];
      if ( ( rc = read( num_socket, pRecvBuf, msg_length ) ) < 0 )
	exit( -1 );

      printf("--> %s\n", RecvBuf );
      
      if ( ( ( atoi( cmdMsg ) == 211 ) || ( atoi( cmdMsg ) == 210 ) ) && ( !got_first_one ) )
	{
	  got_first_one = 1;
	  goto restart;
	}
      else if ( ( atoi( cmdMsg ) == 210 ) && ( got_first_one ) && ( !got_second_one ) )
	{
	  got_second_one = 1;
	  goto restart;
	}
      else
	{
	  got_first_one = 0;
	  got_second_one = 0;
	}
    }
      
  close( num_socket );
  
}

DoDetailedCall( char *pMsg )
{
  char Account[128],DueDate[128],DueTime[128],Phone[128],Phone_ext[128],VehAttr[128],DrvAttr[128],Passenger[128],Comment[128],
       Ref[128], StreetNbr[128], StreetType[128], StreetName[128], City[128], PostalCode[128];
  char CanCharge[128];

#ifdef FOO
  printf( "\tEnter Account: " );
  gets( Account );
  printf("\tDue Date: " );
  gets( DueDate );
  if ( !strlen(DueDate) )
    strcpy( DueDate, "    " );
  printf("\tDue Time: " );
  gets( DueTime );
  if ( !strlen(DueTime) )
    strcpy( DueTime, "    ");
  printf("\tPhone: " );
  gets( Phone );
  if ( !strlen(Phone) )
    strcpy(Phone, "          ");
  printf("\tPhone Ext: " );
  gets( Phone_ext );
  if ( !strlen(Phone_ext) )
    strcpy(Phone_ext, "    ");
  printf("\tVehicle Attr: " );
  gets( VehAttr );
  if ( !strlen(VehAttr) )
    strcpy(VehAttr, "        ");
  printf("\tDriver Attr: " );
  gets( DrvAttr );
  if ( !strlen(DrvAttr) )
    strcpy(DrvAttr, "        ");
  printf("\tPassenger: " );
  gets( Passenger );
  if ( !strlen(Passenger) )
    strcpy(Passenger, "              ");
  printf("\tComment: " );
  gets( Comment );
  if ( !strlen(Comment) )
    strcpy(Comment,"                                ");
  printf("\tCan Charge: ");
  gets( CanCharge );
  CanCharge[0] = ' ';
  printf("\tReference: ");
  gets( Ref );
  if ( !strlen(Ref) )
    strcpy(Ref, "           ");
  printf("\tStreet Nbr: ");
  gets( StreetNbr );
  if ( !strlen(StreetNbr) )
    strcpy(StreetNbr, "    ");
  printf("\tStreet Type: ");
  gets( StreetType );
  if ( !strlen(StreetType) )
    strcpy(StreetType, "         ");
  printf("\tStreet Name: ");
  gets( StreetName );
  if ( !strlen(StreetName) )
    strcpy(StreetName, "                         ");
  printf("\tCity: ");
  gets( City );
  if ( !strlen(City) )
    strcpy(City, "                         ");
  printf( "\tPostal Code: ");
  gets( PostalCode );
  if ( !strlen(PostalCode) )
    strcpy(PostalCode, "     ");
#endif

  sprintf( pMsg, "108%c%-6s%-4s%-4s%-3s%-20s%-3s%-10s%-14s%-5s%-64s%-32s%-32s",'H',"","","0010","","RUUSULANKATU","HEL",
	   "0505495437","","","","","");

}

SendAutoCall( sClient, pTESMsg )
     int       sClient;
     TES_MSG   *pTESMsg;
{
  int         msg_length,rc, i=0;
  AC_MSG      MsgAutoCall;

  msg_length = TES_TYPE_LEN;
  while ( pTESMsg->TESdata[i] != NULL )
    {
      ++i;
      ++msg_length;
    }
      
  sprintf( MsgAutoCall.MsgSize, "%03x", msg_length );
  MsgAutoCall.MsgSize[ AC_MSG_SIZE_LEN ] = '\0';
  memcpy( MsgAutoCall.MsgTES, pTESMsg->TEStype, TES_TYPE_LEN );
  memcpy( &MsgAutoCall.MsgTES[ TES_TYPE_LEN ], pTESMsg->TESdata, msg_length );

  rc = send( sClient, &MsgAutoCall, msg_length + 3, 0 );
}
