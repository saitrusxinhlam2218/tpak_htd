/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: trip_data.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/02/23 21:48:49 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill_rate/trip_data.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: trip_data.c,v $	$Revision: 1.3 $"

static char rcsid[] = "$Id: trip_data.c,v 1.3 1998/02/23 21:48:49 taxiadm Exp $";

#define KEY_SRC
#include "Trip_data_db.h"
#include "taxi_db.h"

void
fill_tripdata_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  TRIPDATA_STRUCT     *pTripData;

  pTripData = ( TRIPDATA_STRUCT * ) req_buf;

  stchar( pTripData->td_transcat, &buf[ 4 ], sizeof( pTripData->td_transcat ) - 1 );
  stlong( pTripData->td_timeperiod, &buf[ 6 ] );
}

void
fill_tripdata( req_buf, buf, action )
     char *req_buf;
     char *buf;
     int action;
{
  TRIPDATA_STRUCT         *pTripData;
  int                     offset = 0;
  
  pTripData = ( TRIPDATA_STRUCT * ) req_buf;

  if ( action == ADD_REC )
    isuniqueid( Db_files[ TRIPDATA_FILE_ID ].fd, &pTripData->td_id );
  stlong( pTripData->td_id, &buf[ offset ] );
  offset += sizeof( pTripData->td_id );

  stchar( pTripData->td_transcat, &buf[ offset ], sizeof( pTripData->td_transcat ) - 1 );
  offset += sizeof( pTripData->td_transcat ) - 1;

  stlong( pTripData->td_timeperiod, &buf[ offset ] );
  offset += sizeof( pTripData->td_timeperiod );

  stfloat( pTripData->td_triplength_FV, &buf[ offset ] );
  offset += sizeof( pTripData->td_triplength_FV );

  stfloat( pTripData->td_triptime_FV, &buf[ offset ] );
  offset += sizeof( pTripData->td_triptime_FV );

  stfloat( pTripData->td_triplength_BR, &buf[ offset ] );
  offset += sizeof( pTripData->td_triplength_BR );

  stfloat( pTripData->td_triptime_BR, &buf[ offset ] );
  offset += sizeof( pTripData->td_triptime_BR );

}

void
unfill_tripdata(req_buf, buf)
     char *req_buf;
     char *buf;
{
  TRIPDATA_STRUCT             *pTripData;
  int                         offset = 0;

  pTripData = ( TRIPDATA_STRUCT * ) req_buf;

  pTripData->td_id = ldlong( &buf[offset] );
  offset += sizeof( pTripData->td_id );

  ldchar( &buf[offset], sizeof( pTripData->td_transcat ) - 1, pTripData->td_transcat );
  offset += sizeof( pTripData->td_transcat ) - 1;

  pTripData->td_timeperiod = ldlong( &buf[offset] );
  offset += sizeof( pTripData->td_timeperiod );

  pTripData->td_triplength_FV = ldfloat( &buf[offset] );
  offset += sizeof( pTripData->td_triplength_FV );

  pTripData->td_triptime_FV = ldfloat( &buf[offset] );
  offset += sizeof( pTripData->td_triptime_FV );
  
  pTripData->td_triplength_BR = ldfloat( &buf[offset] );
  offset += sizeof( pTripData->td_triplength_BR );
  
  pTripData->td_triptime_BR = ldfloat( &buf[offset] );
  offset += sizeof( pTripData->td_triptime_BR );
}

  

