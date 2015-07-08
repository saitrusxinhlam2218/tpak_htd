/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_gps_params.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2003/01/28 16:30:05 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_gps_params.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: fill_gps_params.c,v 1.3 2003/01/28 16:30:05 jwelch Exp $";

#define KEY_SRC
#include <isam.h>
#include "GPS_params_db.h"
#include "taxi_db.h"

void
fill_gps_params_key( char *from, char *buf )
{

}

void
fill_gps_params( req_buf, buf, action )
     char *req_buf;
     char *buf;
     int action;
{
  register int offset = 0;
  GPS_PARAMS *gps = (GPS_PARAMS *)req_buf;

  stfloat(gps->diff_latitude, &buf[offset]);
  offset += FLOATSIZE;

  stfloat(gps->diff_longitude, &buf[offset]);
  offset += FLOATSIZE;

  stlong(gps->diff_height, &buf[offset]);
  offset += LONGSIZE;

  stlong(gps->diff_frequency, &buf[offset]);
  offset += LONGSIZE;

  stchar(gps->diff_server, &buf[offset], sizeof(gps->diff_server)-1);
  offset += sizeof(gps->diff_server)-1;

  stlong(gps->diff_port, &buf[offset]);
  offset += sizeof(gps->diff_port)-1;
  
}

void
unfill_gps_params( req_buf, buf )
     char *req_buf;
     char *buf;
{
  GPS_PARAMS  *gps = (GPS_PARAMS *)req_buf;
  register int offset = 0;

  gps->diff_latitude = ldfloat(&buf[offset]);
  offset += FLOATSIZE;

  gps->diff_longitude = ldfloat(&buf[offset]);
  offset += FLOATSIZE;

  gps->diff_height = ldlong(&buf[offset]);
  offset += LONGSIZE;

  gps->diff_frequency = ldlong(&buf[offset]);
  offset += LONGSIZE;

  ldchar(&buf[offset], sizeof(gps->diff_server)-1,gps->diff_server);
  offset += sizeof(gps->diff_server)-1;

  gps->diff_port = ldlong(&buf[offset]);
  offset += LONGSIZE;

}
  
