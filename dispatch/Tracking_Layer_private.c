/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Tracking_Layer_private.c,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2004/05/12 12:58:59 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Tracking_Layer_private.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Tracking_Layer_private.c,v 1.5 2004/05/12 12:58:59 jwelch Exp $";

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "taxipak.h"
#include "Object.h"
#include "taxi_db.h"
#include "language.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "Fleet_public.h"
#include "Tracking_Layer_private.h"
#include "Tracking_Layer_db.h"

Tracking_Layer_db_add( VEH_HNDL veh_hndl )
{
  VEH           *veh_ptr;
  TL_STRUCT     tl_struct;
  time_t        delete_threshold;
  int           rc, done = 0, mode;
  char          sToTlayer[255];
  int                 res, i, sockfd;
  struct sockaddr_in  serv_addr, cli_addr;
  struct hostent      *hp;  

  bzero( &tl_struct, sizeof( TL_STRUCT ) );
  veh_ptr = ( VEH * ) veh_hndl;

  tl_struct.date_time = time(0);
  tl_struct.fleet = veh_ptr->fleet;
  tl_struct.lat_y = veh_ptr->gps_lat;
  tl_struct.long_x = veh_ptr->gps_long;
  tl_struct.veh_nbr = (short)Veh_get_value(veh_hndl, VEH_NBR);
  get_pkup_date( tl_struct.date );
  get_pkup_time( tl_struct.time );
  tl_struct.receiver_status = (char)Veh_get_value(veh_hndl, VEH_GPS_STATE);

  if ( db_write(TL_FILE_ID, &tl_struct) != SUCCESS )
    {
      db_open( TL_FILE_ID, ISINOUT | ISMANULOCK ); // we've had some cases where the file handle for this gets 'lost'
      db_write(TL_FILE_ID, &tl_struct);
    }

  // Send message to TLAYER server
  bzero( sToTlayer, 255 );
  sprintf( sToTlayer, "%c!%d!%6.0f!%6.0f", veh_ptr->fleet,
	   (short)Veh_get_value(veh_hndl, VEH_NBR),
	   veh_ptr->gps_long,
	   veh_ptr->gps_lat );

  serv_addr.sin_family = AF_INET;
  hp = gethostbyname("TAXIGEO");
  if ( hp != NULL )
    {
      bcopy(hp->h_addr, (char *)&serv_addr.sin_addr, hp->h_length);
      serv_addr.sin_port = htons(6243);
      if ( ( sockfd = socket(AF_INET, SOCK_DGRAM, 0 ) ) != 0 )      
	{
	  bzero((char *)&cli_addr, sizeof(cli_addr));
	  cli_addr.sin_family = AF_INET;
	  cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	  cli_addr.sin_port = htons(0);
	  if ( bind( sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr) ) >= 0 )
	    res = sendto(sockfd, sToTlayer, strlen(sToTlayer), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	  
	  close( sockfd );
	}
    }
  
}
  
  
