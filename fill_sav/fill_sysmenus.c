/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_sysmenus.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/09/02 18:14:20 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_sysmenus.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: fill_sysmenus.c,v $	$Revision: 1.2 $"

static char rcsid[] = "$Id: fill_sysmenus.c,v 1.2 1998/09/02 18:14:20 taxiadm Exp $";

#define KEY_SRC

#include "Sysmenus_db.h"

void
fill_sysmenus_key( req_buf, buf )
     char *req_buf;
     char *buf;
{
  struct cisam_sysmenus *psys;

  psys = (struct cisam_sysmenus *)req_buf;

  stchar( psys->menuname, &buf[0], sizeof(psys->menuname) - 1 );
}

void
fill_sysmenus( req_buf, buf, action )
     char  *req_buf;
     char  *buf;
     int   action;
{
  register int offset = 0;
  struct cisam_sysmenus *psys;
  psys = (struct cisam_sysmenus *)req_buf;

  stchar( psys->menuname, &buf[offset], sizeof(psys->menuname) - 1 );
  offset += sizeof(psys->menuname) - 1;

  stchar( psys->title, &buf[offset], sizeof(psys->title) - 1 );
  offset += sizeof(psys->title) - 1;
}

void
unfill_sysmenus( req_buf, buf )
     char *req_buf;
     char *buf;
{
  struct cisam_sysmenus *psys;
  register int offset = 0;

  psys = (struct cisam_sysmenus *)req_buf;

  ldchar(&buf[offset],sizeof(psys->menuname)-1, psys->menuname);
  offset += sizeof(psys->menuname)-1;

  ldchar(&buf[offset],sizeof(psys->title)-1, psys->title);
  offset += sizeof(psys->title)-1;
}

  

  
