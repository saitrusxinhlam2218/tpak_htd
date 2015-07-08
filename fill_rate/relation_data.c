/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: relation_data.c,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 1998/10/09 12:53:48 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill_rate/relation_data.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: relation_data.c,v $	$Revision: 1.5 $"

static char rcsid[] = "$Id: relation_data.c,v 1.5 1998/10/09 12:53:48 taxiadm Exp $";

#define KEY_SRC
#include "Relations_data_db.h"
#include "taxi_db.h"

extern char tmp_req_buf[];

void
fill_rel_data_key( req_buf, buf )
     char *req_buf;
     char *buf;
{
  REL_DATA_STRUCT            *pRelData;

  pRelData = ( REL_DATA_STRUCT * ) tmp_req_buf;

  stlong( pRelData->nbr, &buf[ 4 ] );
  stlong( pRelData->time_period, &buf[ 8 ] );
}

void
fill_rel_data( req_buf, buf, action )
     char *req_buf;
     char *buf;
     int action;
{
  int      offset = 0;
  REL_DATA_STRUCT           *pRelData;

  pRelData = ( REL_DATA_STRUCT * ) req_buf;

  if ( action == ADD_REC )
    isuniqueid( Db_files[ REL_DATA_FILE_ID ].fd, &pRelData->id );
  stlong( pRelData->id, &buf[offset] );
  offset += sizeof( pRelData->id );

  stlong( pRelData->nbr, &buf[offset] );
  offset += sizeof( pRelData->nbr );

  stlong( pRelData->time_period, &buf[offset] );
  offset += sizeof( pRelData->time_period );

  stfloat( pRelData->FV, &buf[offset] );
  offset += sizeof( pRelData->FV );

  stfloat( pRelData->EV, &buf[offset] );
  offset += sizeof( pRelData->EV );
}

void
unfill_rel_data(req_buf, buf)
     char *req_buf;
     char *buf;
{
  int offset = 0;
  REL_DATA_STRUCT   *pRelData;

  pRelData = ( REL_DATA_STRUCT * ) tmp_req_buf;

  pRelData->id = ldlong( &buf[offset] );
  offset += sizeof( pRelData->id );

  pRelData->nbr = ldlong( &buf[offset] );
  offset += sizeof( pRelData->nbr );

  pRelData->time_period = ldlong( &buf[offset] );
  offset += sizeof( pRelData->time_period );

  pRelData->FV = ldfloat( &buf[offset] );
  offset += sizeof( pRelData->FV );

  pRelData->EV = ldfloat( &buf[offset] );
  offset += sizeof( pRelData->EV );
}
 
