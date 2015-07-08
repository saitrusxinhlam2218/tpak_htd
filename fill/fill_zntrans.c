/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_zntrans.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2003/01/28 16:30:04 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_zntrans.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

static char rcsid[] = "$Id: fill_zntrans.c,v 1.3 2003/01/28 16:30:04 jwelch Exp $";

#define KEY_SRC

#include <isam.h>
#include "ZoneTrans_db.h"

fill_zntrans_key( char *from, char *buf )
{
  ZONETRANS_DB_REC *zt = (ZONETRANS_DB_REC *)from;
  register int offset = 0;

  stchar(zt->t800, &buf[offset], sizeof(zt->t800)-1);
}

fill_zntrans( req_buf, buf, action )
  char *req_buf;
char *buf;
int action;
{
  ZONETRANS_DB_REC *zt;
  register int offset = 0;

  zt = (ZONETRANS_DB_REC *)req_buf;

  stchar(zt->t800,&buf[offset],sizeof(zt->t800)-1);
  offset += sizeof(zt->t800)-1;

  stint(zt->tpak,&buf[offset]);
  offset += INTSIZE;

  stchar(zt->comment,&buf[offset],sizeof(zt->comment)-1);
  offset += sizeof(zt->comment)-1;
}

unfill_zntrans( req_buf, buf )
  char *req_buf;
char *buf;
{
  ZONETRANS_DB_REC  *zt;
  register int offset = 0;

  zt = (ZONETRANS_DB_REC *)req_buf;

  ldchar(&buf[offset], sizeof(zt->t800)-1,zt->t800);
  offset += sizeof(zt->t800)-1;

  zt->tpak = ldint(&buf[offset]);
  offset += INTSIZE;

  ldchar(&buf[offset], sizeof(zt->comment)-1,zt->comment);
  offset += sizeof(zt->comment)-1;
}
