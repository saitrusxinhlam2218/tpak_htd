/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: TLayer_purge.c,v $
*  @(#)  $Revision: 1.1 $
*  @(#)  $Date: 2002/03/25 17:17:29 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/tools/TLayer_purge.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: TLayer_purge.c,v $	$Revision: 1.1 $"

static char rcsid[] = "$Id: TLayer_purge.c,v 1.1 2002/03/25 17:17:29 jwelch Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mads_types.h"
#include "mads_isam.h"
#include "language.h"
#include "key.h"
#include "mad_error.h"
#include "path.h"
#include "Tracking_Layer_private.h"
#include "Tracking_Layer_db.h"

char tmp_req_buf[1024];

main()
{
  time_t     current_time = 0;
  TL_STRUCT  tl_struct;
  int        recs_purged = 0;
  int        done = 0, mode = 0, rec_count = 0, rc;
  FILE       *tlayer_purge_file;

  
  current_time = time(0);

  // go back one day
  current_time -= 86400;

  tl_struct.date_time = 0;
  mode = ISFIRST;
  done = 0;

  if (db_open( TL_FILE_ID, ISINOUT | ISMANULOCK ) != SUCCESS )
    {
      printf("Error opening TLAYER file\n");
      exit(-1);
    }
  
  while ( !done )
    {
      rc = db_read_key( TL_FILE_ID, &tl_struct, &tl_key1, mode );
      mode = ISNEXT;
      if ( ( rc == 0 ) && ( tl_struct.date_time < current_time ) )
	{
	  db_delete( TL_FILE_ID, &tl_struct );
	  ++recs_purged;
	}
      else if ( rc != 0 )
	done = 1;
      ++rec_count;
    }
  db_close(TL_FILE_ID);

  system("rm /tmp/tlayer_purgestatus");
  if ( (tlayer_purge_file = fopen( "/tmp/tlayer_purgestatus", "w+")) != NULL )
    {
      fprintf( tlayer_purge_file, "Track Layer purged in %d secs.\n", time(0) - (current_time + 86400 ) );
      fprintf( tlayer_purge_file, "Records searched - %d Records purged %d\n",
	       rec_count, recs_purged );
      fclose(tlayer_purge_file);
    }
}
  
