/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: holidays.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/02/23 21:48:35 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill_rate/holidays.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: holidays.c,v $	$Revision: 1.3 $"

static char rcsid[] = "$Id: holidays.c,v 1.3 1998/02/23 21:48:35 taxiadm Exp $";

#define KEY_SRC
#include "Holiday_rate_db.h"
#include "taxi_db.h"

void
fill_holiday_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  HOLIDAY_STRUCT    *pHol;

  pHol = ( HOLIDAY_STRUCT * ) req_buf;

  stchar( pHol->start_dt, &buf[ 4 ], sizeof( pHol->start_dt ) - 1 );
}

void
fill_holiday( req_buf, buf, action )
     char *req_buf;
     char *buf;
     int      action;
{
  int         offset = 0;
  HOLIDAY_STRUCT   *pHol;

  pHol = ( HOLIDAY_STRUCT * ) req_buf;

  if ( action == ADD_REC )
    isuniqueid( Db_files[ HOL_FILE_ID ].fd, &pHol->id );
  stlong( pHol->id, &buf[offset] );
  offset += sizeof( pHol->id );

  stchar( pHol->start_dt, &buf[offset], sizeof( pHol->start_dt ) - 1 );
  offset += sizeof( pHol->start_dt ) - 1;

  stchar( pHol->end_dt, &buf[offset], sizeof( pHol->end_dt ) - 1 );
  offset += sizeof( pHol->end_dt ) - 1;

  stchar( pHol->start_tm, &buf[offset], sizeof( pHol->start_tm ) - 1 );
  offset += sizeof( pHol->start_tm ) - 1;

  stchar( pHol->end_tm, &buf[offset], sizeof( pHol->end_tm ) - 1 );
  offset += sizeof( pHol->end_tm ) - 1;

  stchar( pHol->descript, &buf[offset], sizeof( pHol->descript ) - 1 );
  offset += sizeof( pHol->descript ) - 1;

  stint( pHol->tp_nbr, &buf[offset] );
  offset += sizeof( pHol->tp_nbr );
}
void
unfill_holiday(req_buf, buf )
     char *req_buf;
     char *buf;
{
  int        offset = 0;
  HOLIDAY_STRUCT     *pHol;

  pHol = ( HOLIDAY_STRUCT * ) req_buf;

  pHol->id = ldlong( &buf[offset] );
  offset += sizeof( pHol->id );

  ldchar( &buf[offset], sizeof( pHol->start_dt ) - 1, pHol->start_dt );
  offset += sizeof( pHol->start_dt ) - 1;

  ldchar( &buf[offset], sizeof( pHol->end_dt ) - 1, pHol->end_dt );
  offset += sizeof( pHol->end_dt ) - 1;

  ldchar( &buf[offset], sizeof( pHol->start_tm ) - 1, pHol->start_tm );
  offset += sizeof( pHol->start_tm ) - 1;
  
  ldchar( &buf[offset], sizeof( pHol->end_tm ) - 1, pHol->end_tm );
  offset += sizeof( pHol->end_tm ) - 1;

  ldchar( &buf[offset], sizeof( pHol->descript ) - 1, pHol->descript );
  offset += sizeof( pHol->descript ) - 1;

  pHol->tp_nbr = ldint( &buf[offset] );
  offset += sizeof( pHol->tp_nbr );
}

  

  
