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
#include "Place_db.h"

char tmp_req_buf[5120];

struct keydesc null_key = {
  ISNODUPS,
  0
};

main()
{
  struct cisam_ad     adr_rec;
  struct cisam_pl     pl_rec;
  int             rc = SUCCESS;
  int             rc2 = SUCCESS;
  int             mode, mode2;

  db_open( PLACENM_FILE_ID, ISINOUT | ISMANULOCK );
  db_open( ADDRESS_FILE_ID, ISINOUT | ISMANULOCK );
  mode = ISFIRST;
  bzero( &pl_rec, sizeof( PLACENAME_REC ) );
  pl_rec.index = 0;
  while ( rc == SUCCESS )
    {
      rc = db_read_key( PLACENM_FILE_ID, &pl_rec, &null_key, mode );
      mode = ISNEXT;
      if ( rc == SUCCESS )
	{
	  bzero( &adr_rec, sizeof(ADDRESS_REC) );
	  adr_rec.str_index = pl_rec.index;
	  mode2 = ISGTEQ;
	  rc2 = SUCCESS;
	  while ( rc2 == SUCCESS )
	    {
	      rc2 = db_read_key( ADDRESS_FILE_ID, &adr_rec, &ad_key2, mode2 );
	      mode2 = ISNEXT;
	      if ( rc2 == SUCCESS )
		{
		  if ( ( pl_rec.street_nbr >= adr_rec.low_nbr ) &&
		       ( pl_rec.street_nbr <= adr_rec.high_nbr ) &&
		       ( !strcmp(pl_rec.str_city, adr_rec.str_city ) ) &&
		       ( (pl_rec.odd_even == adr_rec.odd_even) || ( adr_rec.odd_even == 'B' ) ) )
		    {
		      if ( pl_rec.str_x_coord == 0 )
			{
			  pl_rec.str_x_coord = adr_rec.str_x_coord;
			  pl_rec.str_y_coord = adr_rec.str_y_coord;
			  db_update( PLACENM_FILE_ID, &pl_rec );
			}
		      rc2 = -1;
		    }
		}
	    }
	}
    }
}
