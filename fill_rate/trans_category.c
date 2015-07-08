/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: trans_category.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/02/23 21:48:48 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill_rate/trans_category.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: trans_category.c,v $	$Revision: 1.3 $"

static char rcsid[] = "$Id: trans_category.c,v 1.3 1998/02/23 21:48:48 taxiadm Exp $";

#define KEY_SRC
#include "Transcat_db.h"

void
fill_trans_category_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  TRANSCAT_STRUCT    *pTransCat;

  pTransCat = ( TRANSCAT_STRUCT * ) req_buf;
  stchar( pTransCat->tc_name, &buf[ 4 ], sizeof( pTransCat->tc_name ) - 1 );
}

void
fill_trans_category( req_buf, buf, action )
     char *req_buf;
     char *buf;
     int action;
{
  TRANSCAT_STRUCT    *pTransCat;
  int                offset = 0;

  pTransCat = ( TRANSCAT_STRUCT * ) req_buf;

  stlong( pTransCat->tc_id, &buf[ offset ] );
  offset += sizeof( pTransCat->tc_id );

  stchar( pTransCat->tc_name, &buf[ offset ], sizeof( pTransCat->tc_name ) - 1 );
  offset += sizeof( pTransCat->tc_name ) - 1;

  stchar( pTransCat->tc_descript, &buf[ offset ], sizeof( pTransCat->tc_descript ) - 1 );
  offset += sizeof( pTransCat->tc_descript ) - 1;

  stfloat( pTransCat->tc_vat, &buf[ offset ] );
  offset += sizeof( pTransCat->tc_vat );

  stfloat( pTransCat->tc_min_fare, &buf[ offset ] );
  offset += sizeof( pTransCat->tc_min_fare );

}

void
unfill_trans_category(req_buf, buf )
     char *req_buf;
     char *buf;
{
  TRANSCAT_STRUCT      *pTransCat;
  int                  offset = 0;

  pTransCat = ( TRANSCAT_STRUCT * ) req_buf;

  pTransCat->tc_id = ldlong( &buf[ offset ] );
  offset += sizeof( pTransCat->tc_id );

  ldchar( &buf[ offset ], sizeof( pTransCat->tc_name ) - 1, pTransCat->tc_name );
  offset += sizeof( pTransCat->tc_name ) - 1;

  if ( pTransCat->tc_name[1] == '\0' )
    {
      pTransCat->tc_name[1] == ' ';
      pTransCat->tc_name[2] == '\0';
    }

  ldchar( &buf[ offset ], sizeof( pTransCat->tc_descript ) - 1, pTransCat->tc_descript );
  offset += sizeof( pTransCat->tc_descript ) - 1;

  pTransCat->tc_vat = ldfloat( &buf[ offset ] );
  offset += sizeof( pTransCat->tc_vat );

  pTransCat->tc_min_fare = ldfloat( &buf[ offset ] );
  offset += sizeof( pTransCat->tc_min_fare );

}

  

  
