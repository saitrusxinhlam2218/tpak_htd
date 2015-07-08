/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: GPS_track_server.c,v $
*  @(#)  $Revision: 1.1 $
*  @(#)  $Date: 2002/03/25 17:17:29 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/tools/GPS_track_server.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: GPS_track_server.c,v $	$Revision: 1.1 $"

static char rcsid[] = "$Id: GPS_track_server.c,v 1.1 2002/03/25 17:17:29 jwelch Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cursesX.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <langinfo.h>
#include <locale.h>
#include <math.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "language.h"
#include "key.h"
#include "mad_error.h"
#include "path.h"
#include "Vehicle_db.h"

char tmp_req_buf[5120];

struct keydesc null_key = {
  ISNODUPS,
  0
};

main()
{
  struct sockaddr_in  serv_addr, cli_addr;
  int             sockfd;
  
  db_open( VEHICLE_FILE_ID, ISINOUT | ISMANULOCK );

  if ( ( sockfd = socket( AF_INET, SOCK_DGRAM, 0 )) < 0 )
    exit(-1);

  bzero((char *) &serv_addr, sizeof( serv_addr ) );
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );
  serv_addr.sin_port = htons(6932);

  if ( bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 )
    exit(-1);

  dg_echo(sockfd, ( struct sockaddr *) &cli_addr, sizeof(cli_addr) );
}

char *field(char *string, int num)
{
   static char tmp_str[512];
   static char *ret;
   int ant;
   
   ant = 1;
   strncpy(tmp_str,string,512);
   ret = strtok(tmp_str,"!");
   while ((ret != NULL) && (num > ant++))
      ret = strtok(NULL,"!");
   return(ret);
}

dg_echo( sockfd, pcli_addr, maxclilen )
     int sockfd;
     struct sockaddr *pcli_addr;
     int maxclilen;
{
  int n, clilen;
  int add_track = 0;
  char mesg[1024];
  char *pGPSPos, *pFleet, *pVehNbr;
  struct cisam_vh veh_rec;
  char pos_buf[32];
  TL_STRUCT  tl_struct;

  for ( ; ; )
    {
      clilen = maxclilen;
      bzero( mesg, 1024 );
      n = recvfrom(sockfd, mesg, 1024, 0, pcli_addr, &clilen );
      if (n<0)
	exit(-1);

      pFleet = field( mesg, 1 );
      veh_rec.fleet = *pFleet;
      pVehNbr = field( mesg, 2 );
      veh_rec.nbr = atoi( pVehNbr );
      if ( db_read_key( VEHICLE_FILE_ID, &veh_rec, &veh_key2, ISEQUAL ) == SUCCESS )
	{
	  pGPSPos = field( mesg, 3 );
	  strcpy( pos_buf, pGPSPos );
	  if ( abs(((int)atoi( pos_buf ) - (int)veh_rec.gps_lat)) > 50 )
	    add_track = 1;
	  veh_rec.gps_lat = (double)atoi( pGPSPos );
	  pGPSPos = field( mesg, 4 );
	  if ( abs(((int)atoi( pGPSPos ) - (int)veh_rec.gps_long)) > 50.0 )
	    add_track = 1;
	  veh_rec.gps_long = (double)atoi( pGPSPos );
	  pGPSPos = field( mesg, 5 );
	  veh_rec.gps_state = (int)atoi( pGPSPos );
	  pGPSPos = field( mesg, 6 );
	  veh_rec.last_signed_on = (int)atoi( pGPSPos );
	  pGPSPos = field( mesg, 7 );
	  veh_rec.last_signed_off = (int)atoi( pGPSPos );
	  db_update( VEHICLE_FILE_ID, &veh_rec );
          pGPSPos = field( mesg, 8 );
          veh_rec.last_driver = (int)atoi( pGPSPos );
	}

      if ( add_track == 1 )
	{
	  db_open( TL_FILE_ID, ISINOUT | ISMANULOCK );
	  add_track = 0;
	  bzero( &tl_struct, sizeof( TL_STRUCT ) );
	  tl_struct.date_time = time(0);
	  tl_struct.fleet = veh_rec.fleet;
	  tl_struct.lat_y = veh_rec.gps_lat;
	  tl_struct.long_x = veh_rec.gps_long;
	  tl_struct.veh_nbr = veh_rec.nbr;
	  get_pkup_date( tl_struct.date );
	  get_pkup_time( tl_struct.time );
	  tl_struct.receiver_status = '@';
	  db_write(TL_FILE_ID, &tl_struct);
	  db_close( TL_FILE_ID );
	}
    }
}
