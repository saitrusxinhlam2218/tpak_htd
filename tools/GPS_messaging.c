/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: GPS_messaging.c,v $
*  @(#)  $Revision: 1.1 $
*  @(#)  $Date: 2002/03/25 17:17:29 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/tools/GPS_messaging.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: GPS_messaging.c,v $	$Revision: 1.1 $"

static char rcsid[] = "$Id: GPS_messaging.c,v 1.1 2002/03/25 17:17:29 jwelch Exp $";

#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/termio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <math.h>
#include "GPS_params_db.h"
#include "path.h"
#include "taxi_db.h"
#include "taxipak.h"
#include "parman.h"
#include "acc.h"
#include "async.h"
#include "msg.h"
#include "mmp.h"
#include "msg_mmp.h"
#include "taxipak.h"
#include "Object.h"
#include "dispatch_strdefs.h"
#include "taxi_db.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "Vehicle_db.h"
#include "GPS_private.h"

#define PIPES
#define TERM_ID_LEN  5

char      gterm_id[16];
char      host_link;
extern char *optarg;
extern int optind, opterr;
char tmp_req_buf[1024];
char req_buf[12];
time_t	mads_time;
int	mdt_msgs_max = 75;
char	*scratch_ptr;
int	ok_stats_flg = 0;
int	mdt_char_mapping = 0;
char  mdt_map[132];
struct offsets *offset;
time_t  lost_time;
int     FirstCE = FALSE;

typedef unsigned char byte;
int i,done_with_diff,done,len,msglen,state,gps_pipe_fd,async_fd,sock_fd,disp_sys_sock,DEBUG;
unsigned char sum,buf[300],ch;
#define ITM_GPS_MSG  "200100"

int tcp_open(name,port)
char *name;
int  port;
{
	int t;
	struct timeval timeout;
	struct sockaddr_in server;
	struct hostent *hp;

	if((hp = gethostbyname(name)) == NULL) return(-1);
	bzero(&server, sizeof(server));
	bcopy(hp->h_addr, &server.sin_addr, hp->h_length);
	server.sin_port=htons(port);
	server.sin_family=hp->h_addrtype;
	if((t=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) return(-2);
	/* Set read timeout to 3 seconds */
	timeout.tv_sec=3;
	timeout.tv_usec=0;
	if(setsockopt(t, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval)) < 0) return(-4);
	if(connect(t, &server, sizeof(server)) != 0) return(-5);
	return(t);
}

int fd_open(name)
char *name;
{
	return(open(name,O_RDWR | O_APPEND,0));
}

int get_message_len(type)
char *type;
{
	int msglen;

	msglen=0;
	switch(*(type++))
	{
		case 'A' : switch(*type)
		{
			case 'A' :	msglen=8; break;
			case 'B' :	msglen=8; break;
			case 'C' :	msglen=9; break;
			case 'D' :	msglen=8; break;
			case 'E' :	msglen=8; break;
			case 'J' :	msglen=8; break;
			case 'a' :	msglen=10; break;
			case 'b' :	msglen=10; break;
			case 'c' :	msglen=11; break;
			case 'd' :	msglen=11; break;
			case 'e' :	msglen=11; break;
			case 'f' :	msglen=15; break;
			case 'g' :	msglen=8; break;
			case 'h' :	msglen=8; break;
			case 'i' :	msglen=9; break;
			case 'j' :	msglen=8; break;
			case 'k' :	msglen=9; break;
			case 'l' :	msglen=9; break;
			case 'm' :	msglen=12; break;
			case 'n' :	msglen=8; break;
			case 'o' :	msglen=25; break;
			case 'p' :	msglen=25; break;
			case 'q' :	msglen=8; break;
			case 'r' :	msglen=8; break;
			case 's' :	msglen=20; break;
			case 't' :	msglen=8; break;
			case 'u' :	msglen=12; break;
			case 'v' :	msglen=8; break;
			case 'w' :	msglen=8; break;
			case 'x' :	msglen=9; break;
			case 'y' :	msglen=11; break;
			case 'z' :	msglen=11; break;
		}
		break;

		case 'B' : switch(*type)
		{
			case 'a' :	msglen=68; break;
			case 'b' :	msglen=92; break;
			case 'c' :	msglen=82; break;
			case 'd' :	msglen=23; break;
			case 'e' :	msglen=33; break;
			case 'f' :	msglen=80; break;
			case 'g' :	msglen=122; break;
			case 'h' :	msglen=52; break;
			case 'i' :	msglen=80; break;
			case 'j' :	msglen=8; break;
			case 'k' :	msglen=69; break;
			case 'l' :	msglen=41; break;
			case 'n' :	msglen=59; break;
		}
		break;

		case 'C' : switch(*type)
		{
			case 'a' :	msglen=9; break;
			case 'b' :	msglen=9; break;
			case 'd' :	msglen=171; break;
			case 'e' :	msglen=52; break; /* could also be 7 */
			case 'f' :	msglen=7; break;
			case 'g' :	msglen=8; break;
			case 'j' :	msglen=294; break;
		}
		break;

		case 'E' : switch(*type)
		{
			case 'a' :	msglen=76; break;
			case 'c' :	msglen=100; break;
			case 'g' :	msglen=158; break;
			case 'k' :	msglen=71; break;
			case 'n' :	msglen=69; break;
		}
		break;

		case 'F' : switch(*type)
		{
			case 'a' :	msglen=9; break;
			case 'd' :	msglen=189; break;
		}
		break;
	}
	return(msglen);
}

InitOncore( pGPSParams )
     GPS_PARAMS  *pGPSParams;
{
  unsigned char   ToOncore[ 128 ];
  unsigned int    Checksum;
  unsigned char   Param;
  unsigned char   EndOfMsg[ 2 ] = { 0x0d, 0x0a };
  int             length, offset, rc;
  int             Position;
  unsigned int    j;
  double          y;

  bzero( ToOncore, sizeof( ToOncore ) );
  /* First send DATA OUTPUT MSG @@Ea */
  offset = 0;
  memcpy( ToOncore, ONCORE_DATA_OUTPUT_MSG, ONCORE_MSG_TYPE_LEN );
  offset += ONCORE_MSG_TYPE_LEN;
  Param = 0x00;          /** output position only when polled **/
  memcpy( &ToOncore[ offset ], &Param, 1 );
  ++offset;
  Checksum = 0x00;
  MsgChecksum( &Checksum, &ToOncore[2], offset - 2 );
  memcpy( &ToOncore[ offset ], &Checksum, 1 );
  ++offset;
  memcpy( &ToOncore[ offset ], EndOfMsg, 2 );
  offset += 2;

  if ( ( rc = write( sock_fd, ToOncore, offset ) ) != offset )
    {
      fprintf( stderr, "Problem writing Oncore (%s)\n", ONCORE_DATA_OUTPUT_MSG );
      close( sock_fd );
      exit( -1 );
    }

  bzero( ToOncore, sizeof( ToOncore ) );
  /** Set Oncore's Fixed Position with FIX_POSITION message **/
  offset = 0;
  memcpy( ToOncore, ONCORE_FIX_POSITION, ONCORE_MSG_TYPE_LEN );
  offset += ONCORE_MSG_TYPE_LEN;
  length = 2;

  offset += PackPosition( &ToOncore[ offset ], (int)(pGPSParams->diff_latitude * MILLIARCSECS_PER_DEGREE) );
  offset += PackPosition( &ToOncore[ offset ], (int)(pGPSParams->diff_longitude * MILLIARCSECS_PER_DEGREE) );
  offset += PackPosition( &ToOncore[ offset ], pGPSParams->diff_height );
  Param = 0x01;      /* height type: GPS elllipsoid height ref */
  memcpy( &ToOncore[ offset ], &Param, 1 );
  ++offset;
  Checksum = 0x00;
  MsgChecksum( &Checksum, &ToOncore[2], offset - 2 );
  memcpy( &ToOncore[ offset ], &Checksum, 1 );
  ++offset;
  memcpy( &ToOncore[ offset ], EndOfMsg, 2 );
  offset += 2;

  if ( ( rc = write( sock_fd, ToOncore, offset ) ) != offset )
    {
      fprintf( stderr, "Problem writing Oncore (%s)\n", ONCORE_FIX_POSITION );
      close( sock_fd );
      exit( -1 );
    }      
  
  bzero( ToOncore, sizeof( ToOncore ) );
  /* Send HOLD_POSITION Message */
  offset = 0;
  memcpy( ToOncore, ONCORE_HOLD_POSITION, ONCORE_MSG_TYPE_LEN );
  offset += ONCORE_MSG_TYPE_LEN;
  Param = 0x01;
  memcpy( &ToOncore[ offset ], &Param, 1 );
  ++offset;
  Checksum = 0x00;
  MsgChecksum( &Checksum, &ToOncore[2], offset - 2 );
  memcpy( &ToOncore[ offset ], &Checksum, 1 );
  ++offset;
  memcpy( &ToOncore[ offset ], EndOfMsg, 2 );
  offset += 2;  
  if ( ( rc = write( sock_fd, ToOncore, offset ) ) != offset )
    {
      fprintf( stderr, "Problem writing Oncore (%s)\n", ONCORE_HOLD_POSITION );
      close( sock_fd );
      exit( -1 );
    }

  /* Finally, set frequency of updates */
  bzero( ToOncore, sizeof( ToOncore ) );
  /* Send OUTPUT_PSEDORANGE_CORRECTION Message */
  offset = 0;
  memcpy( ToOncore, ONCORE_CORRECTION, ONCORE_MSG_TYPE_LEN );
  offset += ONCORE_MSG_TYPE_LEN;
  if(pGPSParams->diff_frequency > 255)
  {
    Param=255;
  }
  else if(pGPSParams->diff_frequency < 5)
  {
    Param=5;
  }
  else
  {
    Param=pGPSParams->diff_frequency;
  }
  memcpy( &ToOncore[ offset ], &Param, 1 );
  ++offset;
  Checksum = 0x00;
  MsgChecksum( &Checksum, &ToOncore[2], offset - 2 );
  memcpy( &ToOncore[ offset ], &Checksum, 1 );
  ++offset;
  memcpy( &ToOncore[ offset ], EndOfMsg, 2 );
  offset += 2;
  if ( ( rc = write( sock_fd, ToOncore, offset ) ) != offset )
    {
      fprintf( stderr, "Problem writing Oncore (%s)\n", ONCORE_CORRECTION );
      close( sock_fd );
      exit( -1 );
    }  
}

int
PackPosition( pOncoreBuf, LatLongAltitude )
     unsigned char *pOncoreBuf;
     int         LatLongAltitude;
{
  unsigned int  j;
  double        y;
  int           Position;
  int           offset = 0;

  Position = LatLongAltitude;
  y = 6.0;
  while ( y >= 0.0 )
    {
      j = (unsigned int)Position/(pow(16.0,y));
      memcpy( (pOncoreBuf + offset), &j, 1 );
      ++offset;
      Position = (Position % (int)(pow(16.0,y)));
      y = y - 2.0;
    }
  return( offset );
}
  
MsgChecksum( pChecksum, pOncoreBuf, length )
     unsigned int *pChecksum;
     unsigned char *pOncoreBuf;
     int           length;
{
  int      offset = 0;
  while ( offset < length )
    {
      *pChecksum ^= *(pOncoreBuf + offset);
      ++offset;
    }
}


void terminate_run()
{
	done=1;
	if(DEBUG) fprintf(stderr,"termination signal received\n");
}



main(argc, argv)
int argc;
char *argv[];
{
  GPS_PARAMS    diff_settings;
  fd_set readmask;
  int    maxfdpl = 0, nfound;


  mdt_init(0);

  

  if((async_fd=fd_open(TO_ASYNC_PIPE)) < 0)
    {
      fprintf(stderr,"pipe open failure (%d)\n",errno);
      close(sock_fd);
      exit(3);
    }

  memcpy( gterm_id, argv[2], 5 );
  
  config_outb_msg(10, "000", ' ', ' ', ' ', ' ');  
  
  switch ( atoi( argv[1] ) )
    {
    case 1:
      TPakGetPosition();
      printf("Getting Position\n");
      break;
    case 2:
      TPakGetCurrentTime();
      printf("Getting time\n");
      break;
    case 3:
      TPakGetSatelliteSelect();
      printf("Getting Satellite Select\n");
      break;
    case 4:
      TPakGetCurrentDate();
      printf("Getting date\n");
      break;
    default:
      break;
    }
  
  close(async_fd);
  
  if(DEBUG) fprintf(stderr,"the process has stopped running\n");
}

CEtimeout()
{
      FirstCE = FALSE;
}

TPakGetPosition( )
{
  unsigned char to_oncore[ 132 ];
  int           offset = 0,len;
  unsigned char Param;
  unsigned char Checksum;
  unsigned char   EndOfMsg[ 2 ] = { 0x0d, 0x0a };  
  unsigned char dest[ 132 ];
  char          response_buf[512];
  
  bzero( to_oncore, 132 );
  memcpy( to_oncore, ONCORE_POSITION_OUTPUT, ONCORE_MSG_TYPE_LEN );
  offset += ONCORE_MSG_TYPE_LEN;
  /* output response only once */
  Param = 0x00;
  memcpy( &to_oncore[ offset ], &Param, 1 );
  ++offset;
  Checksum = 0x00;
  MsgChecksum( &Checksum, &to_oncore[2], offset - 2 );
  memcpy( &to_oncore[ offset ], &Checksum, 1 );
  ++offset;
  memcpy( &to_oncore[ offset ], EndOfMsg, 2 );
  offset += 2;

  len = compress( dest, to_oncore, 8 );
  dest[len] = '\0';
  EscapePercent(dest);
  sprintf( response_buf, "%s", dest );

  add_outb_text( response_buf );
  send_msg_mmp( gterm_id, ITM_GPS_MSG, NULL );
  
}

TPakGetCurrentTime( )
{
  unsigned char to_oncore[ 132 ];
  int           offset = 0,len;
  unsigned char Param;
  unsigned char Checksum;
  unsigned char   EndOfMsg[ 2 ] = { 0x0d, 0x0a };  
  unsigned char dest[ 132 ];
  char          response_buf[512];
  
  bzero( to_oncore, 132 );
  bzero( dest, 132 );
  bzero( response_buf, 512 );

  memcpy( to_oncore, ONCORE_GET_TIME, ONCORE_MSG_TYPE_LEN );
  offset += ONCORE_MSG_TYPE_LEN;

  Param = 0xFF;
  for ( i = 0; i < 3; i++ )
    {
      memcpy( &to_oncore[ offset ], &Param, 1 );
      ++offset;
    }

  Checksum = 0x00;
  MsgChecksum( &Checksum, &to_oncore[2], offset - 2 );
  memcpy( &to_oncore[ offset ], &Checksum, 1 );
  ++offset;
  memcpy( &to_oncore[ offset ], EndOfMsg, 2 );
  offset += 2;

  len = compress( dest, to_oncore, 10 );
  dest[len] = '\0';
  EscapePercent(dest);
  sprintf( response_buf, "%s", dest );

  add_outb_text( response_buf );
  send_msg_mmp( gterm_id, ITM_GPS_MSG, NULL );
  
}

TPakGetCurrentDate( )
{
  unsigned char to_oncore[ 132 ];
  int           offset = 0,len;
  unsigned char Param;
  unsigned char Checksum;
  unsigned char   EndOfMsg[ 2 ] = { 0x0d, 0x0a };  
  unsigned char dest[ 132 ];
  unsigned char test_dest[ 132 ];
  char          response_buf[512];
  
  bzero( to_oncore, 132 );
  bzero( dest, 132 );
  bzero( response_buf, 512 );
  bzero( test_dest, 132 );

  memcpy( to_oncore, ONCORE_GET_DATE, ONCORE_MSG_TYPE_LEN );
  offset += ONCORE_MSG_TYPE_LEN;

  Param = 0xFF;
  for ( i = 0; i < 4; i++ )
    {
      memcpy( &to_oncore[ offset ], &Param, 1 );
      ++offset;
    }

  Checksum = 0x00;
  MsgChecksum( &Checksum, &to_oncore[2], offset - 2 );
  memcpy( &to_oncore[ offset ], &Checksum, 1 );
  ++offset;
  memcpy( &to_oncore[ offset ], EndOfMsg, 2 );
  offset += 2;

  len = compress( dest, to_oncore, 11 );
  
  expand( test_dest, dest, len );
  
  dest[len] = '\0';
  
  EscapePercent(dest);
  sprintf( response_buf, "%s", dest );

  add_outb_text( response_buf );
  send_msg_mmp( gterm_id, ITM_GPS_MSG, NULL );
  
}


TPakGetSatelliteSelect( )
{
  unsigned char to_oncore[ 132 ];
  int           offset = 0,len;
  unsigned char Param;
  unsigned char Checksum;
  unsigned char   EndOfMsg[ 2 ] = { 0x0d, 0x0a };  
  unsigned char dest[ 132 ];
  int           length;
  char          response_buf[512];
  
  bzero( to_oncore, 132 );
  bzero( dest, 132 );
  bzero( response_buf, 512 );

  memcpy( to_oncore, ONCORE_GET_SATELLITESELECT, ONCORE_MSG_TYPE_LEN );
  offset += ONCORE_MSG_TYPE_LEN;
  length = 2;

  Param = 0xFF;
  memcpy( &to_oncore[ offset ], &Param, 1 );
  ++offset;
  Checksum = 0x00;
  MsgChecksum( &Checksum, &to_oncore[2], offset - 2 );
  memcpy( &to_oncore[ offset ], &Checksum, 1 );
  ++offset;
  memcpy( &to_oncore[ offset ], EndOfMsg, 2 );
  offset += 2;

  len = compress( dest, to_oncore, 8 );
  dest[len] = '\0';
  EscapePercent(dest);
  sprintf( response_buf, "%s", dest );

  add_outb_text( response_buf );
  send_msg_mmp( gterm_id, ITM_GPS_MSG, NULL );
  
}

  


int compress(byte *dest, byte *source, int len) {

    int i = 0, j;
    byte *pntr, rem;
    pntr = dest;
    j = 4 * (len / 3);
    if(rem = len % 3) j += rem + 1;


    while(j--) {

	switch(i++ & 0x3) {

	    case 0:
		*pntr++ = *source >> 2;
		break;

	    case 1:
		*pntr = (*source++ << 4) & 0x30;
		*pntr++ |= *source >> 4;
		break;

	    case 2:
		*pntr = (*source++ << 2) & 0x3C;
		*pntr++ |= *source >> 6;
		break;

	    case 3:
		*pntr++ = *source++ & 0x3F;
		break;
	}

    }

    i = len = (int)(pntr - dest);

    while(i--) *dest++ += ' ';

    return(len);

}

EscapePercent(char *message)
/**************************************************************************
 * Name: void EscapePercent(char *message);                               *
 * Type:                                                                  *
 *                                                                        *
 * Purpose: Looks through character string for the '%'-character          *
 *          and escapes it with another '%'-character.                    *
 *                                                                        *
 * Returnvalue: None                                                      *
 **************************************************************************/
{
  int i,j,len;
  char TmpString[512];

  len = strlen(message);

  j = 0;
  for (i = 0; i < len; i++)
  {
    switch(*(message+i))
    {
      case '%' : *(TmpString+j++) = *(message+i);
		 *(TmpString+j++) = '%';
		 break;
      default:   *(TmpString+j++) = *(message+i);
		 break;
    }
  }
  TmpString[j] = 0;
  strcpy(message,TmpString);
}

int expand(byte *dest, byte *source, int len) {

    int i = 0, j;
    byte *pntr, rem;
    j = 3 * (len / 4);
    if(rem = len % 4) j += (rem-1);
    pntr = source;
    while(len--) *pntr++ -= ' ';
    pntr = dest;

    while(j--) {

	switch(i++ % 3) {

	    case 0:
		*pntr = *source++ << 2;
		*pntr++ |= (*source >> 4) & 0x3;
		break;

	    case 1:
		*pntr = *source++ << 4;
		*pntr++ |= (*source >> 2) &  0xF;
		break;

	    case 2:
		*pntr = *source++ << 6;
		*pntr++ |= *source++ & 0x3F;
		break;

	}

    }

    return((int)(pntr - dest));
}

  


