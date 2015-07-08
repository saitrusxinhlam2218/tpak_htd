/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_attrans.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2002/03/21 14:52:04 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_attrans.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

static char rcsid[] = "$Id: fill_attrans.c,v 1.2 2002/03/21 14:52:04 taxiadm Exp $";

#define KEY_SRC

#include <isam.h>
#include "AttrTrans_db.h"

fill_attrans_key( char *from, char *buf )
{
  ATTRANS_DB_REC *at = (ATTRANS_DB_REC *)from;
  register int offset = 0;

  stchar(at->attr_t800, &buf[offset],sizeof(at->attr_t800)-1);
}

fill_attrans(req_buf,buf,action)
  char *req_buf;
char *buf;
int action;
{
  ATTRANS_DB_REC  *at;
  register int offset = 0;

  at = (ATTRANS_DB_REC *)req_buf;

  stchar(at->attr_t800,&buf[offset],sizeof(at->attr_t800)-1);
  offset += sizeof(at->attr_t800)-1;

  stchar(at->attr_tpak,&buf[offset],sizeof(at->attr_tpak)-1);
  offset += sizeof(at->attr_tpak)-1;
}

unfill_attrans( req_buf, buf )
  char *req_buf;
char *buf;
{
  ATTRANS_DB_REC *at;
  register int offset = 0;

  at = (ATTRANS_DB_REC *)req_buf;

  ldchar(&buf[offset], sizeof(at->attr_t800)-1,at->attr_t800);
  offset += sizeof(at->attr_t800)-1;

  ldchar(&buf[offset], sizeof(at->attr_tpak)-1,at->attr_tpak);
  offset += sizeof(at->attr_tpak)-1;
}

