/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: dist_filter.c,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 1998/09/15 12:38:24 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill_rate/dist_filter.c,v $
*
*  Copyright (c) 1996 - MobileSoft Consulting, Inc. Dublin, OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: dist_filter.c,v $	$Revision: 1.5 $"

#define KEY_SRC
#include "Dist_filter_db.h"

void
fill_dist_filter_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  DIST_FILTER_STRUCT        *pDF;

  pDF = ( DIST_FILTER_STRUCT * ) req_buf;

  if ( pDF->trans_cat[1] == ' ' )
    pDF->trans_cat[1] = '\0';
  stchar( pDF->trans_cat, &buf[ 0 ], sizeof( pDF->trans_cat ) - 1 );
}

void
fill_dist_filter( req_buf, buf, action )
     char *req_buf;
     char *buf;
     int action;
{
  DIST_FILTER_STRUCT    *pDF;
  int                   offset = 0;

  pDF = ( DIST_FILTER_STRUCT * ) req_buf;

  if ( pDF->trans_cat[1] == ' ' )
    pDF->trans_cat[1] = '\0';
  stchar( pDF->trans_cat, &buf[offset], sizeof( pDF->trans_cat ) - 1 );
  offset += sizeof( pDF->trans_cat ) - 1;

  stfloat( pDF->distance, &buf[ offset ] );
  offset += sizeof( pDF->distance );
  
  stfloat( pDF->factor_val, &buf[ offset ] );
  offset += sizeof( pDF->factor_val );

  stfloat( pDF->extra_val, &buf[ offset ] );
  offset += sizeof( pDF->extra_val );
}

void
unfill_dist_filter(req_buf, buf)
     char *req_buf;
     char *buf;
{
  DIST_FILTER_STRUCT            *pDF;
  int                           offset = 0;

  pDF = ( DIST_FILTER_STRUCT * ) req_buf;
  
  ldchar( &buf[offset], sizeof( pDF->trans_cat ) - 1, pDF->trans_cat );
  offset += sizeof( pDF->trans_cat ) - 1;
  if ( pDF->trans_cat[1] == '\0' )
    {
      pDF->trans_cat[1] = ' ';
      pDF->trans_cat[2] = '\0';
    }

  pDF->distance = ldfloat( &buf[offset] );
  offset += sizeof( pDF->distance );

  pDF->factor_val = ldfloat( &buf[offset] );
  offset += sizeof( pDF->factor_val );

  pDF->extra_val = ldfloat( &buf[offset] );
  offset += sizeof( pDF->extra_val );  
}

  
