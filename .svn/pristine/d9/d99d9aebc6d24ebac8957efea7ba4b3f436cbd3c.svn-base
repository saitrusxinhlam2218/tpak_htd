/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: cond_type.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/02/23 21:48:31 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill_rate/cond_type.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: cond_type.c,v $	$Revision: 1.3 $"

static char rcsid[] = "$Id: cond_type.c,v 1.3 1998/02/23 21:48:31 taxiadm Exp $";

#define KEY_SRC
#include "Cond_type_db.h"
#include "taxi_db.h"

void
fill_cond_type_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  COND_TYPE_STRUCT   *pctype;

  pctype = ( COND_TYPE_STRUCT * ) req_buf;

  stint( pctype->nbr, &buf[4] );
}

void
fill_cond_type( req_buf, buf, action )
     char *req_buf;
     char *buf;
     int action;
{
  int offset = 0;
  COND_TYPE_STRUCT *pctype;

  pctype = ( COND_TYPE_STRUCT * ) req_buf;

  if ( action == ADD_REC )
    isuniqueid( Db_files[ COND_TYPE_FILE_ID ].fd, &pctype->id );
  stlong( pctype->id, &buf[offset] );
  offset += sizeof( pctype->id );

  stint( pctype->nbr, &buf[offset] );
  offset += sizeof( pctype->nbr );

  stchar( pctype->descript, &buf[offset], sizeof( pctype->descript ) - 1 );
  offset += sizeof( pctype->descript ) - 1;
}

void
unfill_cond_type( req_buf, buf)
     char *req_buf;
     char *buf;
{
  int offset = 0;
  COND_TYPE_STRUCT *pctype;

  pctype = ( COND_TYPE_STRUCT * ) req_buf;

  pctype->id = ldlong( &buf[offset] );
  offset += sizeof( pctype->id );

  pctype->nbr = ldint( &buf[offset] );
  offset += sizeof( pctype->nbr );

  ldchar( &buf[offset], sizeof( pctype->descript ) - 1, pctype->descript );
  offset += sizeof( pctype->descript ) - 1;
}
