/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_geoaddr_point.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2003/01/28 16:30:04 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_geoaddr_point.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: fill_geoaddr_point.c,v $	$Revision: 1.3 $"

static char rcsid[] = "$Id: fill_geoaddr_point.c,v 1.3 2003/01/28 16:30:04 jwelch Exp $";

#define KEY_SRC
#include "taxipak.h"
#include "taxi_db.h"
#include "GeoAddr_Point_db.h"

void
fill_geoaddr_point_key( req_buf, buf )
  char   *req_buf;
char *buf;
{
  GEOADDR_POINT_REC   *pgeopoint;
  register int       offset = 0;

  pgeopoint = (GEOADDR_POINT_REC *)req_buf;

  stlong(pgeopoint->line_idx, &buf[4]);
  stint(pgeopoint->seq_nbr, &buf[8]);
}

void
fill_geoaddr_point( req_buf, buf, action )
  char *req_buf;
char *buf;
int action;
{
  GEOADDR_POINT_REC *pgeopoint;
  register int offset = 0;
  long serial_nbr = 0;

  pgeopoint = (GEOADDR_POINT_REC *)req_buf;

  if ( action == ADD_REC )
    {
      isuniqueid(Db_files[GEOADDR_POINT_FILE_ID].fd, &serial_nbr);
      pgeopoint->point_idx = (int)serial_nbr;
    }

  stlong(pgeopoint->point_idx, &buf[offset]);
  offset += LONGSIZE;

  stlong(pgeopoint->line_idx, &buf[offset]);
  offset += LONGSIZE;

  stint(pgeopoint->seq_nbr, &buf[offset]);
  offset += INTSIZE;

  stlong(pgeopoint->point_x, &buf[offset]);
  offset += LONGSIZE;

  stlong(pgeopoint->point_y, &buf[offset]);
  offset += LONGSIZE;
}

void
unfill_geoaddr_point( req_buf, buf )
  char *req_buf;
char *buf;
{
  GEOADDR_POINT_REC  *pgeopoint;
  register int offset = 0;

  pgeopoint = (GEOADDR_POINT_REC *)req_buf;

  pgeopoint->point_idx = ldlong(&buf[offset]);
  offset += LONGSIZE;

  pgeopoint->line_idx = ldlong(&buf[offset]);
  offset += LONGSIZE;

  pgeopoint->seq_nbr = ldint(&buf[offset]);
  offset += INTSIZE;

  pgeopoint->point_x = ldlong(&buf[offset]);
  offset += LONGSIZE;

  pgeopoint->point_y = ldlong(&buf[offset]);
  offset += LONGSIZE;
}

