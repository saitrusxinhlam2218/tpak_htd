/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ZoneDown.c,v $
*  @(#)  $Revision: 1.26 $
*  @(#)  $Date: 2004/06/22 17:08:17 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/ZoneDown.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: ZoneDown.c,v 1.26 2004/06/22 17:08:17 jwelch Exp $";


#include <stdio.h>
#include <math.h>
#include <sys/param.h>
#include "mad_error.h"
#include "taxipak.h"
#include "Object.h"
#include "Zone_public.h"
#include "Zone_private.h"
#include "ZoneDown_db.h"
#include "mads_isam.h"
#include "key.h"
#include "language.h"
#include "dispatch.h"
#include "enhance.h"
#include "switch_ext.h"
#include "msg_mmp.h"
#include <locale.h>
#include <langinfo.h>

//===========================================================================
//  APP_ZONE DEFINITIONS
//===========================================================================
#define MAX_VERTICES       256   // maximum number of vertices per polygon
#define MAX_ZONES          1000  // maximum number of zones supported 1 to 999
#define MAX_TIMED_OUT_ZONES 100
extern time_t mads_time;	/* system time */
time_t last_zonedown_time = 0;

typedef struct
{
    double lat;
    double lon;
} VERTEX;


int ZoneDownCheck()
{
  char szDebug[10000];
  char szCoord[20];
  char szCenterLat[16];
  char szCenterLon[16];
  bool overflow = FALSE;
  long delta_lat;
  long delta_lon;
  int  start_idx;
  int  stop_idx;
  int  packet_idx;
  double wgsCenterLat, wgsCenterLon, wgsLat, wgsLon;
  int  idx;
  int  num_vertices;
  VERTEX *pVertices;
  ZONE_HNDL zone_hndl;
  ZONE    *zone_ptr;
  VEH_HNDL veh_hndl;
  struct veh_driv *veh_ptr;
  unsigned long checksum = 0;
  ZONE_POLY_REC zonepoly_rec;
  ZONEDOWN_REC zonedown_rec;
  int mode;
  int zd_mode = ISFIRST;
  int count = 0;

  if (mads_time - last_zonedown_time < 60*5) // every five minutes for now
    return;

  last_zonedown_time = mads_time;
    
  db_open(ZONEDOWN_FILE_ID, ISINOUT + ISMANULOCK);
  
  while ( db_read_key(ZONEDOWN_FILE_ID, &zonedown_rec, &zonedown_key1, zd_mode) == SUCCESS )
    {
      overflow = FALSE;
      count++;
      if (count == 10)
	break;
      Zone_get_hndl( (short)zonedown_rec.zone_nbr, (short)0, &zone_hndl );
      if ( zone_hndl == HNDL_NULL )
	continue;
      
      zone_ptr = (ZONE *)zone_hndl;

      Veh_get_hndl( BY_NBR, zonedown_rec.veh_nbr, 0, (VEH_HNDL *) & veh_ptr );
      if ( veh_ptr == NULL )
	continue;
      if ( !SIGNED_ON(veh_ptr) )  // don't send polygon download msg until veh signed on
	continue;
      
      num_vertices = zone_ptr->poly_points;

      // stand = 0 until understand more
      checksum += 0;
      db_open(ZONEPOLY_FILE_ID, ISINOUT + ISMANULOCK);
      mode = ISEQUAL;
      zonepoly_rec.fleet = 'H';
      zonepoly_rec.nbr = zonedown_rec.zone_nbr;
      zonepoly_rec.seq_nbr = 1;
      
      for (idx = 0; idx < num_vertices; idx += 34)
	{
	  bzero(szDebug, 10000);      
	  start_idx = idx + 1;
	  mode = ISEQUAL;
	  zonepoly_rec.seq_nbr = start_idx;
	  stop_idx = MIN(start_idx + 33, num_vertices);
	  db_read_key(ZONEPOLY_FILE_ID, &zonepoly_rec, &znpoly_key1, mode);
	  if (idx == 0)
	    {
	      bzero(szCenterLat, 16);bzero(szCenterLon, 16);
	      strcpy(szCenterLat, zonepoly_rec.latitude);
	      strcpy(szCenterLon, zonepoly_rec.longitude);
	      zonepoly_rec.longitude[3] = *nl_langinfo(RADIXCHAR);
	      zonepoly_rec.latitude[2] = *nl_langinfo(RADIXCHAR);
	      wgsCenterLon = atof(zonepoly_rec.longitude);
	      wgsCenterLat = atof(zonepoly_rec.latitude);
	      
	      sprintf(szDebug, "%03d,%03d,%02X,%02X,%02X,%+s,%+s,",
		      zone_ptr->zone_nbr, zone_ptr->gps_radius, zone_ptr->poly_points,
		      start_idx,
		      stop_idx,
		      szCenterLon,
		      szCenterLat);	      
	    }
	  else
	    {
	      sprintf(szDebug, "%03d,%03d,%02X,%02X,%02X,%+s,%+s,",
		      zone_ptr->zone_nbr, zone_ptr->gps_radius, zone_ptr->poly_points,
		      start_idx,
		      stop_idx,
		      szCenterLon,
		      szCenterLat);
	    }
	  

		  

	  checksum += ((unsigned long) (wgsCenterLat * 100000.0)) % 10000L;
	  checksum += ((unsigned long) (wgsCenterLon * 100000.0)) % 10000L;

	  for (packet_idx = start_idx - 1; packet_idx <= stop_idx - 1; packet_idx++)
	    {
	      mode = ISNEXT;
	      bzero(szCoord, 20);

	      zonepoly_rec.longitude[3] = *nl_langinfo(RADIXCHAR);
	      zonepoly_rec.latitude[2] = *nl_langinfo(RADIXCHAR);	  
	      wgsLat = atof(zonepoly_rec.latitude);
	      wgsLon = atof(zonepoly_rec.longitude);

	      delta_lon = (long)(wgsLon * 100000.0) - (long)(wgsCenterLon * 100000.0);
	      delta_lat = (long)(wgsLat * 100000.0) - (long)(wgsCenterLat * 100000.0);
	      if ((delta_lon > 524287)||(delta_lon < -524288) || (delta_lat > 524287) || (delta_lon < - 524288))
		overflow = TRUE;
	      
	      sprintf(szCoord, "%05X%05X", delta_lon & 0xFFFFF, delta_lat & 0xFFFFF);
	      strcat(szDebug, szCoord);

	      checksum += ((unsigned long) (wgsLat * 100000.0)) % 10000L;
	      checksum += ((unsigned long) (wgsLon * 100000.0)) % 10000L;

	      printf("Zone %d point %d Lat %s Long %s Checksum %d\n",
		     zonepoly_rec.nbr, zonepoly_rec.seq_nbr,
		     zonepoly_rec.latitude, zonepoly_rec.longitude,
		     checksum);
	      
	      db_read_key(ZONEPOLY_FILE_ID, &zonepoly_rec, &znpoly_key1, mode);
	      if (zonepoly_rec.nbr != zone_ptr->zone_nbr)
		break;	      
	    }
	  if (!overflow)
	    {
	      mk_outb_text(szDebug);
	      send_msg_mmp(veh_ptr->mid, POLY_DOWNLOAD, veh_ptr);
	    }
	  else
	    printf("Polygon zone too big - %d\n", zone_ptr->zone_nbr);

	}
      db_delete(ZONEDOWN_FILE_ID, &zonedown_rec);
    }
  
  db_close(ZONEDOWN_FILE_ID);
  
}
