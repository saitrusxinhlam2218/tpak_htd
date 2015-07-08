/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ac_main.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2002/03/25 17:48:26 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/auto_tes/ac_main.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

static char rcsid[] = "$Id: ac_main.c,v 1.3 2002/03/25 17:48:26 jwelch Exp $";


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <netdb.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <locale.h>
#include <math.h>
#include "language.h"
#include "taxipak.h"
#include "Object.h"
#include "mads_isam.h"
#include "key.h"
#include "mads_types.h"
#include "taxi_error.h"
#include "mad_ipc.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Auto_TES.h"
#define DECLARING
#include "ui_strdefs.h"
#include "comm_strdefs.h"
#include "writer_strdefs.h"

#ifdef TESTING
#define TES_SVC   "TES_test_svc"
#else
#define TES_SVC   "TES_svc"
#endif

#define MKEY1    2048

#define PERMS    0666

int newsockfd;
int readid;  // message queue id
struct offsets *offset;
int death_process();
void Zone_mem_init();

int sh_id;
char *sh_pt;
extern char *shmat();
FLEET *fleet[FLEET_MAX];
char tmp_req_buf[BUFSIZ];
MEM_ZONE *fleetMemZones[FLEET_MAX][ZONE_MAX];
short GetCustomerZone(int, double, double );

int LostConnect()
{
  exit(-1);
  close( newsockfd );
}

main ( argc, argv)
     int  argc;
     char **argv;
{
  int                   sockfd, clilen, childpid, fd, rc;
  struct servent        *sp;
  struct sockaddr_in    cli_addr, serv_addr;
  char                  RecvBuf[132];
  int                   NbrConnections = 1;
  struct linger xlinger;
  int intval;
  int session = 0;
  char nlspath[80];
  static char put_nlspath[80];
  int  putenv();
  MEM_ZONE *pZone = NULL;
  int zn_nbr = 0;
  
  init_mgr_locale();

  Zone_mem_init();

  zn_nbr = GetCustomerZone(7, 385186.0, 6671682.0);
  printf("Found zone %d\n", zn_nbr);

  open_catalog(&COMMON_m_catd, "common", 0, 0);
  open_catalog(&UI_m_catd, "ui", 0, 0);
  open_catalog(&WRITER_catd, "writer", 0, 0);


  if ((sp = getservbyname( TES_SVC, "tcp")) == NULL)
    {
      perror("Unable to get TES service");
      exit(-1);
    }
  
  if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    perror("server: can't open stream socket");

  intval = sizeof(struct linger);
  if (getsockopt(sockfd, SOL_SOCKET, SO_LINGER, &xlinger, &intval) < 0)
    {
      perror("Get socket option failed: LINGER ");
      return(-1);
    }
  
  intval = sizeof(struct linger);
  xlinger.l_onoff=0;
  xlinger.l_linger=0;
  if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &xlinger, intval) < 0)
    {
      perror( "Set socket option failed: LINGER ");
      return(-1);
    }
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &intval, sizeof(int)) < 0)
    {
      perror( "Set socket option failed: REUSEADDR ");
      return(-1);
    }
  
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = sp->s_port;
  
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
      perror("server: can't bind local address");
      exit( -1 );
    }
  
  listen(sockfd, NbrConnections);

  clilen = sizeof( cli_addr );
  
  while ( 1 )
    {
      signal(SIGCHLD, death_process);
      newsockfd = accept( sockfd, ( struct sockaddr * ) &cli_addr, &clilen );
      
      if ( newsockfd <= 0 )
	{
	  perror( "Error on accept" );
	  exit ( -1 );
	}
      ++session;
      if ( session >= 1000 )
	session = 1;
      
      rc = fork();
      
      if ( rc == -1 )
	{
	  perror( "Error forking..." );
	  exit( -1 );
	}
      
      /** Child  **/
      if ( rc == 0 )
	{
	  close( sockfd );
	  sigaction(SIGCHLD, SA_NOCLDWAIT);
	  signal(SIGCHLD, SIG_IGN);	  
	  signal( SIGURG, LostConnect );
	  /** Attach to shared memory **/
	  TPak_shm_init();
	  ManageRequests( newsockfd, session );
	  Client_to_AutoCall( newsockfd, NULL, TES_CLIENT_DISCONNECT, session );	  
	  exit( -1 );
	}
      /** Parent **/
      else
	{
	  close( newsockfd );
	}
    }

  TerminateConnection( newsockfd );

  exit(1);
}

TerminateConnection( sClient )
{
  close( sClient );
  exit(-1);
}

ManageRequests( sClient, session )
     int    sClient;
     int    session;
{
  AC_MSG       *ClientMsg;
  TES_MSG      *TESMsg;
  int          msg_length, TES_type;
  char         msgType[ TES_TYPE_LEN + 1 ];
  char         RecvBuf[ BUFSIZ ];
  char         *pRecvBuf;
  int          done = 0, rc;
  
  while ( !done )
    {
      bzero( RecvBuf, sizeof( RecvBuf ) );
      /** First read first three bytes to get message size **/
      if ( ( rc = read( sClient, RecvBuf, AC_MSG_SIZE_LEN ) ) <= 0 )
	return ( -1 );
      
      msg_length = (int) strtol( RecvBuf, ( char ** ) 0, 16 );
      
      if ( ( msg_length > 1024 ) || ( msg_length <= 0 ) )
	continue;
      
      pRecvBuf = &RecvBuf[AC_MSG_SIZE_LEN];
      if ( ( rc = read( sClient, pRecvBuf, msg_length ) ) < 0 )
	return( -1 );
      
      ClientMsg = ( AC_MSG * ) RecvBuf;
      
      TESMsg = ( TES_MSG * ) ClientMsg->MsgTES;
      strncpy( msgType, TESMsg->TEStype, TES_TYPE_LEN );
      TES_type = atoi(msgType);
      Client_to_AutoCall( sClient, TESMsg, TES_type, session );      
      switch ( TES_type )
	{
	case TES_STARTED:
	  StartedRqst( sClient, session );
	  break;
	case TES_TERMINATE:
	  TerminateRqst( sClient, session );
	  done = 1;
	  break;
	case TES_CANCEL_CALL:
	  CancelCall( sClient, TESMsg, session );
	  break;
	case TES_GEN_EXCEPTION:
	  GenKOReply( sClient, TES_KO_BAD_FORMAT , session );
	  break;
	case TES_UPDATE_CALL:
	  GenKOReply( sClient, TES_KO_BAD_FORMAT , session );
	  break;
	case TES_REQ_ACCT_CALL:
	  AccountCallRqst( sClient, TESMsg, session );
	  break;
	case TES_RQST_ADDR:
	  AddressValidRqst( sClient, TESMsg, session );
	  break;

	case TES_REQ_DETAILED_CALL:
	    DispatchDetailedCall( sClient, TESMsg, session );
	  break;

        case TES_MONITOR_CALL:
          MonitorCall( sClient, TESMsg, session );
          break;

        case TES_REQ_STAND_TAXI:
          ReqStandTaxi( sClient, TESMsg, session );
          break;

	case TES_REQ_STAND_TAXIv2:
	  ReqStandTaxiv2( sClient, TESMsg, session );
	  break;

	case TES_REQ_CALL_DETAILS:
	    ReqCallDetails( sClient, TESMsg, session );
	    break;

	default:
	  GenKOReply( sClient, TES_KO_BAD_FORMAT , session );
	  break;
	}

    }
}
	  

death_process() 
{
        int pid;
	long msgtype = 0;
	int rc = 0;
        int status; 
        struct rusage usage; 
	char   message_on_queue[5120];

        while ((pid=wait3(&status,WNOHANG,&usage)) > 0) 
	{
	  //  bzero( message_on_queue, 5120 );
	  //msgtype = (long)pid;
	  //while ( rc != -1 )
	  //  rc = msgrcv( readid, message_on_queue, 1024, msgtype, MSG_NOERROR | IPC_NOWAIT );
	}
			
}


TPak_shm_init()
 {
      int     i;
      sh_id = shmget(SHMKEY, 0, 0);
      if (!(sh_pt = shmat(sh_id, 0x1000000, 0)))
	{
	  exit(-1);
	}

      offset = (struct offsets *) sh_pt;
      fleet[0] = offset->fleet;
      for (i = 1; i < FLEET_MAX; i++ )
	fleet[i] = fleet[0] + i;
}

void Zone_mem_init()
{
  int fl_idx = 0, zone_nbr = 42, point_index = 0, k;
  ZONE_POLY_REC zonepoly_rec;
  int mode = ISEQUAL, done = 0;
  short zn_nbr = 1;
  MEM_ZONE *pZone;

  db_open( ZONEPOLY_FILE_ID, DB_READ_ONLY|DB_MANUAL_LOCK );
  for ( fl_idx = 7; fl_idx >= 0; fl_idx-- )
    {

      for ( k = 1; k < ZONE_MAX; k++ )
	{
	  mode = ISEQUAL; done = 0; zonepoly_rec.fleet = 0x41 + fl_idx; zonepoly_rec.nbr = k; zonepoly_rec.seq_nbr = 1;point_index = 0;
	  while ( !done )
	    {
	      if ( db_read_key(ZONEPOLY_FILE_ID, &zonepoly_rec, &znpoly_key1, mode ) == SUCCESS )
		{
		  if ( mode == ISEQUAL ) // first point in zone...malloc
		    {
		      pZone = (MEM_ZONE *)malloc( sizeof( MEM_ZONE ) );
		      fleetMemZones[fl_idx][k] = (MEM_ZONE *)pZone;
		    }
		  
		  mode = ISNEXT;
		  if ( zonepoly_rec.nbr != k )
		    done = TRUE;
		  else
		    {
		      pZone->zone_poly[point_index].x = zonepoly_rec.pointx;
		      pZone->zone_poly[point_index].y = zonepoly_rec.pointy;
		      ++point_index;
		      
		      if ( point_index > MAX_POLY_POINTS )
			break;
		    }
		}
	      else
		done = 1;
	    }  // loop zonepoly point
	  pZone->poly_points = point_index;
	  if ( point_index > 0 )
	    ZonePolyCalculateCentroid( pZone );
	} // next zone
      
    } // next fleet
  

  db_close( ZONEPOLY_FILE_ID );
}

short GetCustomerZone( int fl_nbr, double cust_x, double cust_y )
{
  MEM_ZONE *pZone;
  POINT_POLY cust_point, p1, p2;
  POINT_POLY *zone_poly;
  int  nbr_points, i, j;
  double xinters;
  int counter = 0;

  cust_point.x = cust_x;
  cust_point.y = cust_y;
  
  for ( j = 1; j < ZONE_MAX; j++ )
    {
      if ( ( pZone = fleetMemZones[fl_nbr][j] ) != HNDL_NULL )
	{
	  zone_poly = (POINT_POLY *)pZone->zone_poly;
	  p1 = zone_poly[0];
	  nbr_points = pZone->poly_points;
	  
	  for ( i = 1; i <= nbr_points; i++ )
	    {
	      p2 = zone_poly[i % nbr_points];
	      if ( cust_point.y > MIN(p1.y,p2.y) )
		{
		  if ( cust_point.y <= MAX(p1.y,p2.y) )
		    {
		      if ( cust_point.x <= MAX(p1.x,p2.x) )
			{
			  if ( p1.y != p2.y )
			    {
			      xinters = (cust_point.y - p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
			      if ( p1.x == p2.x || cust_point.x <= xinters )
				counter++;
			    }
			}
		    }
		}
	      p1 = p2;
	    }
	  if ( counter % 2 == 0 )
	    continue;
	  else
	    {
	      printf("Found zone %d\n", j);
	      return(j);
	    }
	}
    }
      
  return 0;
}
ZonePolyCalculateCentroid( MEM_ZONE *zone_ptr )
{
  int   N, i;
  double x, y;
  double running_sumx, running_sumy;
  POINT_POLY  *zone_poly;

  N = zone_ptr->poly_points;
  zone_poly = (POINT_POLY *) zone_ptr->zone_poly;
  running_sumx = 0.0;
  running_sumy = 0.0;
  
  for ( i = 0; i < N; i++ )
    {
      running_sumx += zone_poly[i].x;
      running_sumy += zone_poly[i].y;
    }

  x = running_sumx / N;
  y = running_sumy / N;

  zone_ptr->poly_centroid_x = x;
  zone_ptr->poly_centroid_y = y;
  
}
