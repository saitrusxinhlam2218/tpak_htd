/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: conditions.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/02/23 21:48:32 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill_rate/conditions.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: conditions.c,v $	$Revision: 1.3 $"

static char rcsid[] = "$Id: conditions.c,v 1.3 1998/02/23 21:48:32 taxiadm Exp $";

#define KEY_SRC
#include "Conditions_db.h"
#include "taxi_db.h"

void
fill_conditions_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  CONDITIONS_STRUCT   *pConditions;

  pConditions = ( CONDITIONS_STRUCT * ) req_buf;

  if ( pConditions->cond_transcat[1] == ' ' )
    pConditions->cond_transcat[1] = '\0';
  stchar( pConditions->cond_transcat, &buf[ 4 ], sizeof( pConditions->cond_transcat ) - 1 );
}

void
fill_conditions( req_buf, buf, action )
     char *req_buf;
     char *buf;
     int action;
{
  int offset = 0;
  CONDITIONS_STRUCT   *pCond;

  pCond = ( CONDITIONS_STRUCT * ) req_buf;

  if ( action == ADD_REC )
    isuniqueid( Db_files[ CONDITIONS_FILE_ID ].fd, &pCond->cond_id );
  stlong( pCond->cond_id, &buf[offset] );
  offset += sizeof( pCond->cond_id );

  if ( pCond->cond_transcat[1] == ' ' )
    pCond->cond_transcat[1] = '\0';
  stchar( pCond->cond_transcat, &buf[offset], sizeof( pCond->cond_transcat ) - 1 );
  offset += sizeof( pCond->cond_transcat ) - 1;

  stlong( pCond->cond_nbr, &buf[offset] );
  offset += sizeof( pCond->cond_nbr );

  stchar( &pCond->cond_category, &buf[offset], sizeof( pCond->cond_category ) );
  offset += sizeof( pCond->cond_category );
  
  stfloat( pCond->cond_FV, &buf[offset] );
  offset += sizeof( pCond->cond_FV );

  stfloat( pCond->cond_EV, &buf[offset] );
  offset += sizeof( pCond->cond_EV );
}

void
unfill_conditions(req_buf, buf)
     char *req_buf;
     char *buf;
{
  CONDITIONS_STRUCT      *pCond;
  int offset = 0;

  pCond = ( CONDITIONS_STRUCT * ) req_buf;

  pCond->cond_id = ldlong( &buf[offset] );
  offset += sizeof( pCond->cond_id );

  ldchar( &buf[offset], sizeof( pCond->cond_transcat ) - 1, pCond->cond_transcat );
  offset += sizeof( pCond->cond_transcat ) - 1;
  if ( pCond->cond_transcat[1] == '\0' )
    {
      pCond->cond_transcat[1] = ' ';
      pCond->cond_transcat[2] = '\0';
    }

  pCond->cond_nbr = ldlong( &buf[offset] );
  offset += sizeof( pCond->cond_nbr );

  pCond->cond_category = buf[offset];
  offset += sizeof( pCond->cond_category );

  pCond->cond_FV = ldfloat( &buf[offset] );
  offset += sizeof( pCond->cond_FV );

  pCond->cond_EV = ldfloat( &buf[offset] );
  offset += sizeof( pCond->cond_EV );
}
