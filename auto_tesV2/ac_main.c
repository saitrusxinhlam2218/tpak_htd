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
#include <setjmp.h>
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
extern int msgkey;
struct offsets *offset;
int death_process();
static sigjmp_buf  xts_jmpbuf;
void Zone_mem_init();
void LatLong_to_UTM(double, double, double *, double *);
int sh_id;
char *sh_pt;
extern char *shmat();
FLEET *fleet[FLEET_MAX];
char tmp_req_buf[BUFSIZ];
MEM_ZONE *fleetMemZones[FLEET_MAX][ZONE_MAX];
short GetCustomerZone(int, double, double );
short GetCustomerZoneCentroid( int, double, double );

#define deg2rad (M_PI/180.0)
#define rad2deg (180.0/M_PI)
double MYMIN(double, double);
double MYMAX(double, double);

int LostConnect()
{
  close( newsockfd );
  exit(-1);  
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

  MEM_ZONE *pZone = NULL;
    double degreeVal = 0.0;
    double minuteVal = 0.0;
    double secondsVal = 0.0;
    char latClient[16];
    char lonClient[16];
    double custY, custX;
    double Northing, Easting;

	char sLocale[80];
        char nlspath[80];
        static char put_nlspath[80];
        int  putenv();
  struct sigaction action;
  sigset_t sig_set;
  int ret_setmask=0;
  char   message_on_queue[5120];
  long pid = 0;long msgtype = 0;
  
	//  init_mgr_locale();

	 strcpy(nlspath, "/usr/taxi/catalogs/finnish/%N.cat");
	 sprintf(put_nlspath, "NLSPATH=%s", nlspath);
	 putenv(put_nlspath);  
  setlocale(LC_ALL, "");
  Zone_mem_init();



  strcpy(latClient, "601324.1");
  strcpy(lonClient, "245614.7");
  custY = (double)atof(latClient);
  custY /= 10000.0;
  minuteVal = modf(custY, &degreeVal);
  minuteVal *= 100.0;
  secondsVal = modf(minuteVal, &minuteVal);
  secondsVal *= 100.0;
  minuteVal = (minuteVal + secondsVal/60.0)/60.0;
  custY = degreeVal + minuteVal;

  custX = (double)atof(lonClient);
  custX /= 10000.0;
  minuteVal = modf(custX, &degreeVal);
  minuteVal *= 100.0;
  secondsVal = modf(minuteVal, &minuteVal);
  secondsVal *= 100.0;
  minuteVal = (minuteVal + secondsVal/60.0)/60.0;
  custX = degreeVal + minuteVal;
  

  LatLong_to_UTM( custY, custX, &Northing, &Easting );
    
  //printf("397540, 6676263 - %d\n", GetCustomerZone( 7, Easting, Northing ) );
  //printf("394996, 6677257 - %d\n", GetCustomerZone( 7, 394996.0, 6677257.0 ) );
  

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
      sigsetjmp(xts_jmpbuf, 1);
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

      sigemptyset(&sig_set);
      sigaddset(&sig_set, SIGCHLD);
      ret_setmask=sigprocmask(SIG_SETMASK, &sig_set, NULL);
      
      rc = fork();

      sigemptyset(&sig_set);
      ret_setmask=sigprocmask(SIG_SETMASK, &sig_set, NULL);
      
      if ( rc == -1 )
	{
	  perror( "Error forking..." );
	  exit( -1 );
	}
      
      /** Child  **/
      if ( rc == 0 )
	{
	  close( sockfd );
	  //	  sigaction(SIGCHLD, SA_NOCLDWAIT);
	  //	  signal(SIGCHLD, SIG_IGN);	  
	  //	  signal( SIGURG, LostConnect );
	  /** Attach to shared memory **/
	  action.sa_handler = SIG_IGN;
	  action.sa_flags = SA_NOCLDWAIT;
	  sigaction(SIGCHLD, &action, NULL);
	  signal(SIGCHLD, SIG_IGN);
	  signal(SIGURG, LostConnect);
	  
	  TPak_shm_init();
	  ManageRequests( newsockfd, session );
	  Client_to_AutoCall( newsockfd, NULL, TES_CLIENT_DISCONNECT, session );
	  //Clean up message queue
	  bzero( message_on_queue, 5120 );
	  msgtype = (long)getpid();
	  rc = 0;
	  while ( rc != -1 )
	    rc = msgrcv( msgkey, message_on_queue, 1024, msgtype, MSG_NOERROR | IPC_NOWAIT );	  
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
	  ReqStandTaxiV2( sClient, TESMsg, session );
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
        long pid;
	long msgtype = 0;
	int rc = 0;
        int status; 
        struct rusage usage;
	char   message_on_queue[5120];	

	bzero( message_on_queue, 5120 );
	msgtype = (long)getpid();
	rc = 0;
	while ( rc != -1 )
	  rc = msgrcv( msgkey, message_on_queue, 1024, msgtype, MSG_NOERROR | IPC_NOWAIT );
	
        while ((pid=wait3(&status,WNOHANG,&usage)) > 0) 
	{
	  bzero( message_on_queue, 5120 );
	  msgtype = (long)pid;
	  rc = 0;
	  while ( rc != -1 )
	    rc = msgrcv( msgkey, message_on_queue, 1024, msgtype, MSG_NOERROR | IPC_NOWAIT );
	}
	siglongjmp(xts_jmpbuf, 1);	
			
}


TPak_shm_init()
 {
      int     i;
      sh_id = shmget(SHMKEY, 0, 0);
      if (!(sh_pt = shmat(sh_id, (const void *)0x0a00000010000000, 0)) == (char *)-1 )
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
  ZONE_REC zone_rec;
  int mode = ISEQUAL, done = 0;
  short zn_nbr = 1;
  MEM_ZONE *pZone;
  BOOL StandZone = FALSE;

  db_open( ZONEPOLY_FILE_ID, DB_READ_ONLY|DB_MANUAL_LOCK );
  db_open( ZONE_FILE_ID, DB_READ_ONLY|DB_MANUAL_LOCK );
  for ( fl_idx = 7; fl_idx >= 0; fl_idx-- )
    {

      for ( k = 1; k < ZONE_MAX; k++ )
	{
	  mode = ISEQUAL; done = 0; zonepoly_rec.fleet = 0x41 + fl_idx; zonepoly_rec.nbr = k; zonepoly_rec.seq_nbr = 1;point_index = 0;
	  pZone = NULL;
	  while ( !done )
	    {
	      if ( db_read_key(ZONEPOLY_FILE_ID, &zonepoly_rec, &znpoly_key1, mode ) == SUCCESS )
		{
		  // Check to see if the zone is a taxi stand...if it is, we don't want to consider it
		  zone_rec.fleet = 0x41 + fl_idx; zone_rec.nbr = k;
		  if ( db_read_key(ZONE_FILE_ID, &zone_rec, &zn_key1, ISEQUAL ) != SUCCESS )
		    {
		      StandZone = TRUE;
		      done = 1;
		      break;
		    }
		  else if ( zone_rec.type == ZN_STAND )
		    {
		      StandZone = TRUE;
		      done = 1;
		      break;
		    }
		  
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
	  if ( !StandZone && (pZone != NULL) )
	    {
	      pZone->poly_points = point_index;
	      if ( point_index > 0 )
		ZonePolyCalculateCentroid( pZone );
	    }
	  StandZone = FALSE;
	} // next zone
      
    } // next fleet
  

  db_close( ZONEPOLY_FILE_ID );
  db_close( ZONE_FILE_ID );
}

short GetCustomerZoneCentroid( int fl_nbr, double cust_x, double cust_y )
{
  MEM_ZONE *pZone;
  double best_dist = 0.0, dist = 0.0;
  short best_zone;
  double centroid_x = 0.0, centroid_y = 0.0;
  int j;
  double myCust_x, myCust_y;
  
  myCust_x = cust_x; myCust_y = cust_y;

  for ( j = 1; j < ZONE_MAX; j++ )
    {
      if ( ( pZone = fleetMemZones[fl_nbr][j] ) != HNDL_NULL )
	{
	  centroid_x = pZone->poly_centroid_x;
	  centroid_y = pZone->poly_centroid_y;
	  dist = sqrt( pow(((double)myCust_x - (double)centroid_x), 2.0 ) +
		       pow(((double)myCust_y - (double)centroid_y), 2.0 ) );
	  if ( best_dist == 0 )
	    {
	      best_dist = dist;
	      best_zone = j;
	    }
	  else if ( dist < best_dist )
	    {
	      best_dist = dist;
	      best_zone = j;
	    }
	}
    }

    return best_zone;

}

	  
short GetCustomerZone( int fl_nbr, double cust_x, double cust_y )
{
  MEM_ZONE *pZone;
  POINT_POLY cust_point, p1, p2;
  POINT_POLY *zone_poly;
  int  nbr_points, i, j;
  double xinters;
  int counter = 0;
  double myCust_x, myCust_y;

  myCust_x = cust_x;
  myCust_y = cust_y;

  bzero(&cust_point, sizeof(POINT_POLY));
  bzero(&p1, sizeof(POINT_POLY));
  bzero(&p2, sizeof(POINT_POLY));
  cust_point.x = myCust_x;
  cust_point.y = myCust_y;
  
  for ( j = 1; j < ZONE_MAX; j++ )
    {
      if ( ( pZone = fleetMemZones[fl_nbr][j] ) != HNDL_NULL )
	{
	  zone_poly = (POINT_POLY *)pZone->zone_poly;
	  p1 = zone_poly[0];
	  counter = 0;
	  nbr_points = pZone->poly_points;
	  
	  for ( i = 1; i <= nbr_points; i++ )
	    {
	      if ( i == 1 )
		printf("Searching..%d\n", j);
	      p2 = zone_poly[i % nbr_points];
	      if ( cust_point.y > MYMIN(p1.y,p2.y) )
		{
		  if ( cust_point.y <= MYMAX(p1.y,p2.y) )
		    {
		      if ( cust_point.x <= MYMAX(p1.x,p2.x) )
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
	    return(j); 
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

void LatLong_to_UTM( double Lat, double Long, double *pNorthing, double *pEasting )
{
  int   iLong;
  double a = 6378137.0;
  double eccSquared = 0.00669439;
  double k0 = 0.9996013;

  double LongOrigin;
  double eccPrimeSquared;
  double N, T, C, A, M;



  if ( !strcmp(LANGUAGE, "Danish") )
    LongOrigin = 9.0;
  else
    {
      if ( Long > 6.0 )
	LongOrigin = (int)(Long/6)*6 + 3*(int)(Long/6)/abs((int)(Long/6));
      else
	LongOrigin = 0.0;
    }
  
  eccPrimeSquared = (eccSquared)/(1.0-eccSquared);

  N = a/sqrt(1.0-eccSquared*sin(Lat*deg2rad)*sin(Lat*deg2rad));
  T = tan(Lat*deg2rad)*tan(Lat*deg2rad);
  C = eccPrimeSquared*cos(Lat*deg2rad)*cos(Lat*deg2rad);
  A = cos(Lat*deg2rad)*(Long-LongOrigin)*deg2rad;

  M = 111132.9520 * Lat - 16038.5200*sin(2.000*Lat*deg2rad) +
      16.8300*sin(4.0*Lat*deg2rad) - 0.0200*sin(6.000*Lat*deg2rad);

  *pEasting = k0*N*(A+(1.0-T+C)*A*A*A/6.0
	            + (5.0-18.0*T+T*T+72.0*C-58.0*eccPrimeSquared)*A*A*A*A*A/120.0)
                    + 500000.0;
  *pNorthing = k0*(M+N*tan(Lat*deg2rad)*(A*A/2.0+(5.0-T+9.0*C+4.0*C*C)*A*A*A*A/24.0
					 +(61.0-58.0*T+T*T+600.0*C-330.0*eccPrimeSquared)*A*A*A*A*A*A/720.0));
}

double MYMIN(double d1, double d2)
{
  if ( d1 <= d2 )
    return d1;
  else
    return d2;
}

double MYMAX(double d1, double d2)
{
  if ( d1 >= d2 )
    return d1;
  else
    return d2;
}
