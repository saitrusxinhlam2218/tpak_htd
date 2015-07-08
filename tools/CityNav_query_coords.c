/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: CityNav_query_coords.c,v $
*  @(#)  $Revision: 1.1 $
*  @(#)  $Date: 2002/03/25 17:17:29 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/tools/CityNav_query_coords.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: CityNav_query_coords.c,v $	$Revision: 1.1 $"

static char rcsid[] = "$Id: CityNav_query_coords.c,v 1.1 2002/03/25 17:17:29 jwelch Exp $";


#include <stdio.h>
#include <time.h>
#include <cursesX.h>
#include <string.h>
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "language.h"
#include "key.h"
#include "mad_error.h"
#include "path.h"
#include "Address_db.h"
#include "cnapi.h"

char tmp_req_buf[5120];

struct keydesc null_key = {
  ISNODUPS,
  0
};

main( argc, argv)
     int argc;
     char *argv[];
{
  struct cisam_ad     adr_rec;
  int             rc = SUCCESS;
  CNargptr            mydata;
  int err_code;
  int mode;
  int counter = 0;
  

  bzero( &adr_rec, sizeof( struct cisam_ad ) );

  db_open( ADDRESS_FILE_ID, ISINOUT | ISMANULOCK );
  mode = ISFIRST;

  while ( ( rc == SUCCESS ) && ( counter < atoi(argv[1]) ) )
    {
      rc = db_read_key( ADDRESS_FILE_ID, &adr_rec, &ad_key1, mode );
      mode = ISNEXT;
      ++counter;
    }

  while ( rc == SUCCESS )
    {

      rc = db_read_key( ADDRESS_FILE_ID, &adr_rec, &ad_key1, mode );
      mode = ISNEXT;
      if ( rc == SUCCESS )
	{
	  ++counter;
	      err_code = CNconnect( 1, 0, "polly", 11300 );
	      if ( err_code > 0 )
		{
		  mydata = CNstartNewSearch();      	      
		  err_code = CNputLink( mydata,
				       adr_rec.str_index,
				       adr_rec.str_city,
				       (int)((adr_rec.low_nbr + adr_rec.high_nbr)/2),
				       1 );
		  
		  err_code = CNGetCoords( mydata );
		  
		  if ( mydata->totdrivetime > 0 )
		    adr_rec.str_x_coord = (int) mydata->pathlength;
		  if ( mydata->pathlength > 0 )
		    adr_rec.str_y_coord = (int) mydata->totdrivetime;
		  
		  db_update(ADDRESS_FILE_ID, &adr_rec);
		  CNcleanup( mydata );
		  CNdisconnect();
#ifdef FOO
		  if ( counter % 500 == 0 )
		    {
		      printf("Sleeping (%d)...\n", counter );
		      sleep(60);
		    }
#endif
		}
	  else
	    {
	      sleep(60);
	    }
	  
	}

    }


}
