#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <isam.h>

#include "taxipak.h"
#include "Object.h"
#include "key.h"
#include "Zone_private.h"
#include "Zone_db.h"

char tmp_req_buf[5120];

main( argc, argv )
     int argc;
     char **argv;
{
  FILE  *fp;
  char  filename[64];
  char *pDispType;
  char  source_buf[1024];
  char  *pResult, *pZone1, *pZone2;
  char  sZone1[16], sZone2[16];
  int   done, rc, j, k, found_backup_zone = 0;
  ZONE_REC  zone;

  fp = fopen( "/usr/taxi/run/traces/gps_disp_trace", "r" );

  
  if ( fp == NULL )
    exit( -1 );

  if ( ( rc = db_open(ZONE_FILE_ID, ISINOUT + ISMANULOCK) ) != SUCCESS )
    exit( -1 );

  
  done = 0;
  while ( !done )
    {
      bzero( source_buf, 1024 );
      bzero( &zone, sizeof(ZONE_REC) );
      pDispType = NULL;
      found_backup_zone = 0;
      
      pResult = fgets( source_buf, 1024, fp );
      if ( pResult == NULL )
	exit(0);

      pDispType = strstr( source_buf, "VEHICLE->CALL" );
      if ( pDispType != NULL )
	{
	  pZone1 = strstr( source_buf, "Zone" );
	  if ( pZone1 != NULL )
	    {
	      strncpy( sZone1, (pZone1 + 5), 3 );
	      pResult = (pZone1 + 5);
	      pZone2 = strstr( pResult, "Zone" );
	      if ( pZone2 != NULL )
		{
		  strncpy( sZone2, (pZone2 + 5), 3 );
		  zone.fleet = 'A';
		  zone.nbr = atoi( sZone1 );
		  if ( db_read_key( ZONE_FILE_ID, &zone, &zn_key1, ISEQUAL ) == SUCCESS )
		    {
		      if ( atoi( sZone2 ) != zone.nbr )
			{
			  for ( j = 0; j < LEVEL_MAX; j++ )
			    {
			      for ( k = 0; k < BCK_UP_MAX; k++ )
				{
				  if ( atoi( sZone2 ) == zone.lev_zone_nbr[j][k] )
				    {
				      found_backup_zone = 1;
				      break;
				    }
				}
			    }
			}
		      else
			found_backup_zone = 1;
		      if ( found_backup_zone == 0 )
			printf( source_buf );
		    }
		}
	    }
	}
    }
}
