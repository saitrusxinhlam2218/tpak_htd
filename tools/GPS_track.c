/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: GPS_track.c,v $
*  @(#)  $Revision: 1.1 $
*  @(#)  $Date: 2002/03/25 17:17:29 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/tools/GPS_track.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: GPS_track.c,v $	$Revision: 1.1 $"

static char rcsid[] = "$Id: GPS_track.c,v 1.1 2002/03/25 17:17:29 jwelch Exp $";

#include <stdio.h>
#include <time.h>
#include <cursesX.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
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
  struct cisam_vh   veh_rec;
  int             rc = SUCCESS;
  int mode;

  db_open( VEHICLE_FILE_ID, ISINOUT | ISMANULOCK );

  while (1)
    {
      veh_rec.nbr = 0;
      veh_rec.fleet = 'A';
      mode = ISFIRST;
      rc = SUCCESS;
      while ( rc == SUCCESS )
	{
	  rc = db_read_key( VEHICLE_FILE_ID, &veh_rec, &veh_key2, mode );
	  mode = ISNEXT;
	  if ( rc == SUCCESS )
	    {
	      if ( veh_rec.gps_lat > 0.0 && veh_rec.gps_long > 0.0 )
		{
		  Send_GPS_pos( &veh_rec );
		}
	    }
	}
      sleep(120);
    }
}

Send_GPS_pos( pVeh )
     struct cisam_vh *pVeh;
{
  int                 res, i, sockfd;
  char                send_buf[132];
  struct sockaddr_in  serv_addr, cli_addr;
  struct hostent      *hp;

  bzero( send_buf, sizeof( send_buf ) );

  sprintf( send_buf, "%c!%d!%5.2f!%5.2f!%d!%d!%d!%d",
	   pVeh->fleet, pVeh->nbr, pVeh->gps_lat, pVeh->gps_long, pVeh->gps_state,
	   pVeh->last_signed_on, pVeh->last_signed_off, pVeh->last_driver );
  
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  hp = gethostbyname("speedy.mbsoft.com");
  bcopy(hp->h_addr, (char *)&serv_addr.sin_addr, hp->h_length);
  serv_addr.sin_port = htons(6932);
  if ( ( sockfd = socket(AF_INET, SOCK_DGRAM, 0 ) ) != 0 )      
    {
      bzero((char *)&cli_addr, sizeof(cli_addr));
      cli_addr.sin_family = AF_INET;
      cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      cli_addr.sin_port = htons(0);
      if ( bind( sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr) ) >= 0 )
	res = sendto(sockfd, send_buf, strlen(send_buf), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
      
      close( sockfd );
    }

}  
