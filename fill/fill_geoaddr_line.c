/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_geoaddr_line.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2003/01/28 16:30:04 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_geoaddr_line.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

static char rcsid[] = "$Id: fill_geoaddr_line.c,v 1.3 2003/01/28 16:30:04 jwelch Exp $";

#define KEY_SRC
#include "taxipak.h"
#include "taxi_db.h"
#include "GeoAddr_Line_db.h"

void
fill_geoaddr_line_key( req_buf, buf )
  char   *req_buf;
char *buf;
{
  GEOADDR_LINE_REC  *pgeoline;
  register int offset = 0;

  pgeoline = (GEOADDR_LINE_REC *)req_buf;

  stlong(pgeoline->line_idx, &buf[0]);
  stlong(pgeoline->addr_idx, &buf[4]);
}

void
fill_geoaddr_line( req_buf, buf, action )
  char *req_buf;
char *buf;
int action;
{
  GEOADDR_LINE_REC *pgeoline;
  register int offset = 0;
  int serial_nbr = 0;

  pgeoline = (GEOADDR_LINE_REC *)req_buf;

  if ( action == ADD_REC )
    {
      isuniqueid(Db_files[GEOADDR_LINE_FILE_ID].fd, &serial_nbr);
      pgeoline->line_idx = (int)serial_nbr;
    }

  stlong(pgeoline->line_idx, &buf[offset]);
  offset += LONGSIZE;

  stlong(pgeoline->addr_idx, &buf[offset]);
  offset += LONGSIZE;
}

void
unfill_geoaddr_line( req_buf, buf )
  char *req_buf;
char *buf;
{
  GEOADDR_LINE_REC  *pgeoline;
  register int offset = 0;

  pgeoline = (GEOADDR_LINE_REC *)req_buf;

  pgeoline->line_idx = ldlong(&buf[offset]);
  offset += LONGSIZE;

  pgeoline->addr_idx = ldlong(&buf[offset]);
  offset += LONGSIZE;
}
