/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: relations.c,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 1998/02/23 21:48:42 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill_rate/relations.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: relations.c,v $	$Revision: 1.4 $"

static char rcsid[] = "$Id: relations.c,v 1.4 1998/02/23 21:48:42 taxiadm Exp $";

#define KEY_SRC
#include "Relations_db.h"
#include "taxi_db.h"

void
fill_relations_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  RELATIONS_STRUCT    *pRelations;

  pRelations = ( RELATIONS_STRUCT * ) req_buf;

  if ( pRelations->transcat[1] == ' ' )
    pRelations->transcat[1] = '\0';
  stchar( pRelations->transcat, &buf[ 8 ], sizeof( pRelations->transcat ) - 1 );
}

void
fill_relations( req_buf, buf, action )
     char *req_buf;
     char *buf;
     int    action;

{
  int       offset = 0;
  RELATIONS_STRUCT   *pRelations;

  pRelations = ( RELATIONS_STRUCT * ) req_buf;

  if ( action == ADD_REC )
    isuniqueid( Db_files[ RELATIONS_FILE_ID].fd, &pRelations->id );
  stlong( pRelations->id, &buf[offset] );
  offset += sizeof( pRelations->id );

  stlong( pRelations->nbr, &buf[offset] );
  offset += sizeof( pRelations->nbr );

  if ( pRelations->transcat[1] == ' ' )
    pRelations->transcat[1] = '\0';
  stchar( pRelations->transcat, &buf[offset], sizeof( pRelations->transcat ) - 1 );
  offset += sizeof( pRelations->transcat ) - 1;

  stchar( pRelations->pckup_zone, &buf[offset], sizeof( pRelations->pckup_zone ) - 1 );
  offset += sizeof( pRelations->pckup_zone ) - 1;

  stchar( pRelations->pckup_sname, &buf[offset], sizeof( pRelations->pckup_sname ) - 1 );
  offset += sizeof( pRelations->pckup_sname ) - 1;

  stchar( pRelations->pckup_nbr, &buf[offset], sizeof( pRelations->pckup_nbr ) - 1 );
  offset += sizeof( pRelations->pckup_nbr ) - 1;

  stchar( pRelations->dst_zone, &buf[offset], sizeof( pRelations->dst_zone ) - 1 );
  offset += sizeof( pRelations->dst_zone ) - 1;
  
  stchar( pRelations->dst_sname, &buf[offset], sizeof( pRelations->dst_sname ) - 1 );
  offset += sizeof( pRelations->pckup_sname ) - 1;

  stchar( pRelations->dst_nbr, &buf[offset], sizeof( pRelations->dst_nbr ) - 1 );
  offset += sizeof( pRelations->dst_nbr ) - 1;

}

void
unfill_relations(req_buf, buf)
     char *req_buf;
     char *buf;
{
  int   offset = 0;
  RELATIONS_STRUCT       *pRelations;

  pRelations = ( RELATIONS_STRUCT * ) req_buf;

  pRelations->id = ldlong( &buf[offset] );
  offset += sizeof( pRelations->id );

  pRelations->nbr = ldlong( &buf[offset] );
  offset += sizeof( pRelations->nbr );

  ldchar( &buf[offset], sizeof( pRelations->transcat ) - 1, pRelations->transcat );
  offset += sizeof( pRelations->transcat ) - 1;
  if ( pRelations->transcat[1] == '\0' )
    {
      pRelations->transcat[1] = ' ';
      pRelations->transcat[2] = '\0';
    }

  ldchar( &buf[offset], sizeof( pRelations->pckup_zone ) - 1, pRelations->pckup_zone );
  offset += sizeof( pRelations->pckup_zone ) - 1;
  
  ldchar( &buf[offset], sizeof( pRelations->pckup_sname ) - 1, pRelations->pckup_sname );
  offset += sizeof( pRelations->pckup_sname ) - 1;
  
  ldchar( &buf[offset], sizeof( pRelations->pckup_nbr ) - 1, pRelations->pckup_nbr );
  offset += sizeof( pRelations->pckup_nbr ) - 1;
  
  ldchar( &buf[offset], sizeof( pRelations->dst_zone ) - 1, pRelations->dst_zone );
  offset += sizeof( pRelations->pckup_zone ) - 1;
  
  ldchar( &buf[offset], sizeof( pRelations->dst_sname ) - 1, pRelations->dst_sname );
  offset += sizeof( pRelations->dst_sname ) - 1;
  
  ldchar( &buf[offset], sizeof( pRelations->dst_nbr ) - 1, pRelations->dst_nbr );
  offset += sizeof( pRelations->dst_nbr ) - 1;

}

  
