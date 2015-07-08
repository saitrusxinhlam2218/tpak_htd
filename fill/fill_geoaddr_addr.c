/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_geoaddr_addr.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2003/01/28 16:30:04 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_geoaddr_addr.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

static char rcsid[] = "$Id: fill_geoaddr_addr.c,v 1.3 2003/01/28 16:30:04 jwelch Exp $";

#define KEY_SRC
#include "taxipak.h"
#include "taxi_db.h"
#include "GeoAddr_Addr_db.h"

void
fill_geoaddr_addr_key( req_buf, buf )
  char   *req_buf;
char *buf;
{
  GEOADDR_ADDR_REC   *pgeoaddr;
  register int       offset = 0;
   
  pgeoaddr = (GEOADDR_ADDR_REC *)req_buf;

  stlong(pgeoaddr->addr_idx, &buf[0]);
  stchar(pgeoaddr->sname, &buf[12],sizeof(pgeoaddr->sname)-1);
  stchar(pgeoaddr->city, &buf[32],sizeof(pgeoaddr->city)-1);
}

void
fill_geoaddr_addr( req_buf, buf, action )
  char *req_buf;
char *buf;
int action;
{
  GEOADDR_ADDR_REC  *pgeoaddr;
  register int offset = 0;
  int serial_nbr = 0;

  pgeoaddr = (GEOADDR_ADDR_REC *)req_buf;
  if ( action == ADD_REC )
    {
      isuniqueid(Db_files[GEOADDR_ADDR_FILE_ID].fd, &serial_nbr);
      pgeoaddr->addr_idx = (int)serial_nbr;
    }

  stlong(pgeoaddr->addr_idx, &buf[offset]);
  offset += LONGSIZE;

  stint(pgeoaddr->from_left, &buf[offset]);
  offset += INTSIZE;

  stint(pgeoaddr->to_left, &buf[offset]);
  offset += INTSIZE;

  stint(pgeoaddr->from_right, &buf[offset]);
  offset += INTSIZE;

  stint(pgeoaddr->to_right, &buf[offset]);
  offset += INTSIZE;

  stchar(pgeoaddr->sname, &buf[offset], sizeof(pgeoaddr->sname)-1);
  offset += sizeof(pgeoaddr->sname)-1;

  stchar(pgeoaddr->city, &buf[offset], sizeof(pgeoaddr->city)-1);
  offset += sizeof(pgeoaddr->city)-1;
}

void
unfill_geoaddr_addr( req_buf, buf )
  char  *req_buf;
char *buf;
{
  GEOADDR_ADDR_REC  *pgeoaddr;
  register int offset = 0;

  pgeoaddr = (GEOADDR_ADDR_REC *)req_buf;

  pgeoaddr->addr_idx = ldlong(&buf[offset]);
  offset += LONGSIZE;

  pgeoaddr->from_left = ldint(&buf[offset]);
  offset += INTSIZE;

  pgeoaddr->to_left = ldint(&buf[offset]);
  offset += INTSIZE;

  pgeoaddr->from_right = ldint(&buf[offset]);
  offset += INTSIZE;

  pgeoaddr->to_right = ldint(&buf[offset]);
  offset += INTSIZE;

  ldchar(&buf[offset], sizeof(pgeoaddr->sname)-1, pgeoaddr->sname);
  offset += sizeof(pgeoaddr->sname)-1;

  ldchar(&buf[offset], sizeof(pgeoaddr->city)-1, pgeoaddr->city);
  offset += sizeof(pgeoaddr->city)-1;
}
  

  
  
