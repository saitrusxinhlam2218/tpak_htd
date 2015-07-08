/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: seasons.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/02/23 21:48:43 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill_rate/seasons.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: seasons.c,v $	$Revision: 1.3 $"

static char rcsid[] = "$Id: seasons.c,v 1.3 1998/02/23 21:48:43 taxiadm Exp $";

#define KEY_SRC
#include "Seasons_db.h"

void
fill_seasons_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  SEASONS_STRUCT    *pSeasons;

  pSeasons = ( SEASONS_STRUCT * ) req_buf;

  stchar( pSeasons->season_start, &buf[ 4 ], sizeof( pSeasons->season_start ) - 1 );
}

void
fill_seasons( req_buf, buf, action )
     char *req_buf;
     char *buf;
     int action;
{
  int   offset = 0;
  SEASONS_STRUCT   *pSeasons;

  pSeasons = ( SEASONS_STRUCT * ) req_buf;

  stlong( pSeasons->season_id, &buf[offset] );
  offset += sizeof( pSeasons->season_id );

  stchar( pSeasons->season_start, &buf[offset], sizeof( pSeasons->season_start ) - 1 );
  offset += sizeof( pSeasons->season_start ) - 1;

  stchar( pSeasons->season_end, &buf[offset], sizeof( pSeasons->season_end ) - 1 );
  offset += sizeof( pSeasons->season_end ) - 1;

  stchar( pSeasons->season_descript, &buf[offset], sizeof( pSeasons->season_descript ) - 1 );
  offset += sizeof( pSeasons->season_descript ) - 1;

  stfloat( pSeasons->season_FV, &buf[offset] );
  offset += sizeof( pSeasons->season_FV );

  stfloat( pSeasons->season_EV, &buf[offset] );
  offset += sizeof( pSeasons->season_EV );
}

void
unfill_seasons( req_buf, buf )
     char *req_buf;
     char *buf;
{
  SEASONS_STRUCT   *pSeasons;
  int    offset = 0;
  
  pSeasons = ( SEASONS_STRUCT * ) req_buf;

  pSeasons->season_id = ldlong( &buf[offset] );
  offset += sizeof( pSeasons->season_id );

  ldchar( &buf[offset], sizeof( pSeasons->season_start ) - 1, pSeasons->season_start );
  offset += sizeof( pSeasons->season_start ) - 1;

  ldchar( &buf[offset], sizeof( pSeasons->season_end ) - 1, pSeasons->season_end );
  offset += sizeof( pSeasons->season_end ) - 1;

  ldchar( &buf[offset], sizeof( pSeasons->season_descript ) - 1, pSeasons->season_descript );
  offset += sizeof( pSeasons->season_descript ) - 1;

  pSeasons->season_FV = ldfloat( &buf[offset] );
  offset += sizeof( pSeasons->season_FV );

  pSeasons->season_EV = ldfloat( &buf[offset] );
  offset += sizeof( pSeasons->season_EV );
}

  

  
