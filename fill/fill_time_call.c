/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_time_call.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:48:06 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_time_call.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: fill_time_call.c,v 1.2 1998/02/23 21:48:06 taxiadm Exp $";

#define KEY_SRC
#include <time.h>
#include "Time_call_db.h"

void
fill_time_call_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  TIME_CALL_REC *tc;

  tc = ( TIME_CALL_REC * ) req_buf;

  stchar( &tc->fleet, &buf[0], sizeof( tc->fleet ) );
  stint( tc->zone, &buf[1] );
  stlong( tc->due_date_time, &buf[3] );
  stlong( tc->cl_nbr, &buf[7] );
}

void
fill_time_call( req_buf, buf, action )
     char *req_buf;
     char *buf;
     int action;
{
  TIME_CALL_REC *tc;
  register int offset = 0;

  tc = ( TIME_CALL_REC * ) req_buf;

  stchar( &tc->fleet, &buf[offset], sizeof( tc->fleet ) );
  offset += sizeof( tc->fleet );

  stint( tc->zone, &buf[offset] );
  offset += INTSIZE;

  stlong( tc->due_date_time, &buf[offset] );
  offset += LONGSIZE;

  stlong( tc->cl_nbr, &buf[offset] );
  offset += LONGSIZE;

  stchar( tc->drv_attr, &buf[offset], sizeof( tc->drv_attr ) - 1 );
  offset += sizeof( tc->drv_attr ) - 1;
  
  stchar( tc->veh_attr, &buf[offset], sizeof( tc->veh_attr ) - 1 );
  offset += sizeof( tc->veh_attr ) - 1;
}

void
unfill_time_call(req_buf, buf)
     char *req_buf;
     char *buf;
{
  TIME_CALL_REC *tc;
  register int offset = 0;

  tc = ( TIME_CALL_REC * ) req_buf;
  
  tc->fleet = buf[offset];
  offset += sizeof( tc->fleet );

  tc->zone = ldint( &buf[offset] );
  offset += INTSIZE;

  tc->due_date_time = ldlong( &buf[offset] );
  offset += LONGSIZE;

  tc->cl_nbr = ldlong( &buf[offset] );
  offset += LONGSIZE;

  ldchar(&buf[offset], sizeof( tc->drv_attr) - 1, tc->drv_attr );
  offset += sizeof( tc->drv_attr ) - 1;

  ldchar(&buf[offset], sizeof( tc->veh_attr) - 1, tc->veh_attr );
  offset += sizeof( tc->veh_attr ) - 1;
}
  
