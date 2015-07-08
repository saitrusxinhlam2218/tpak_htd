/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_sysmenuitems.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/09/02 18:14:18 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_sysmenuitems.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: fill_sysmenuitems.c,v $	$Revision: 1.2 $"

static char rcsid[] = "$Id: fill_sysmenuitems.c,v 1.2 1998/09/02 18:14:18 taxiadm Exp $";


#include "Sysmenus_db.h"

void
fill_sysitems_key( req_buf, buf )
     char *req_buf;
     char *buf;
{
  struct cisam_sysitems *psys;

  psys = (struct cisam_sysitems *)req_buf;

  stchar( psys->menuname, &buf[0], sizeof(psys->menuname) - 1 );
  stlong( psys->itemnum, &buf[sizeof(psys->menuname)-1] );
}

void
fill_sysitems( req_buf, buf, action )
     char  *req_buf;
     char  *buf;
     int   action;
{
  register int offset = 0;
  struct cisam_sysitems *psys;
  psys = (struct cisam_sysitems *)req_buf;

  stchar( psys->menuname, &buf[offset], sizeof(psys->menuname) - 1 );
  offset += sizeof(psys->menuname) - 1;

  stlong( psys->itemnum, &buf[offset] );
  offset += LONGSIZE;
  
  stchar( psys->mtext, &buf[offset], sizeof(psys->mtext) - 1 );
  offset += sizeof(psys->mtext) - 1;

  stchar( &psys->mtype, &buf[offset], sizeof(psys->mtype) );
  offset += sizeof( psys->mtype );

  stchar( psys->progname, &buf[offset], sizeof(psys->progname) - 1 );
  offset += sizeof(psys->progname) - 1;  
}

void
unfill_sysitems( req_buf, buf )
     char *req_buf;
     char *buf;
{
  struct cisam_sysitems *psys;
  register int offset = 0;

  psys = (struct cisam_sysitems *)req_buf;

  ldchar(&buf[offset],sizeof(psys->menuname)-1, psys->menuname);
  offset += sizeof(psys->menuname)-1;

  psys->itemnum = ldlong( &buf[offset] );
  offset += LONGSIZE;

  ldchar(&buf[offset],sizeof(psys->mtext)-1, psys->mtext);
  offset += sizeof(psys->mtext)-1;

  psys->mtype = buf[offset];
  offset += sizeof( psys->mtype );

  ldchar(&buf[offset],sizeof(psys->progname)-1, psys->progname);
  offset += sizeof(psys->progname)-1;
}

