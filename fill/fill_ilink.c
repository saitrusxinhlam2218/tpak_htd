/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_ilink.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2002/03/21 14:52:04 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_ilink.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

static char rcsid[] = "$Id: fill_ilink.c,v 1.2 2002/03/21 14:52:04 taxiadm Exp $";

#define KEY_SRC

#include <isam.h>
#include "Interlink_db.h"

fill_ilink_key( char *from, char *buf )
{
  ILINK_DB_REC *il = (ILINK_DB_REC *)from;
  register int offset = 0;

  stlong(il->tpak_nbr, &buf[0]);
}

fill_ilink( req_buf, buf, action )
  char *req_buf;
char *buf;
int action;
{
  ILINK_DB_REC  *il;
  register int offset = 0;

  il = ( ILINK_DB_REC * )req_buf;
  
  stlong(il->tpak_nbr, &buf[offset]);
  offset += LONGSIZE;

  stchar(il->t800_nbr, &buf[offset], sizeof(il->t800_nbr)-1);
  offset += sizeof(il->t800_nbr)-1;

  stchar(il->status, &buf[offset], sizeof(il->status)-1);
  offset += sizeof(il->status)-1;

  stchar(il->origin, &buf[offset], sizeof(il->origin)-1);
  offset += sizeof(il->origin)-1;
}

unfill_ilink( req_buf, buf )
  char *req_buf;
char *buf;
{
  ILINK_DB_REC *il;
  register int offset = 0;

  il = (ILINK_DB_REC *)req_buf;

  il->tpak_nbr = ldlong(&buf[offset]);
  offset += LONGSIZE;

  ldchar(&buf[offset], sizeof(il->t800_nbr)-1,il->t800_nbr);
  offset += sizeof(il->t800_nbr)-1;

  ldchar(&buf[offset], sizeof(il->status)-1,il->status);
  offset += sizeof(il->status)-1;

  ldchar(&buf[offset], sizeof(il->origin)-1,il->origin);
  offset += sizeof(il->origin)-1;
}
