/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: m_action.c,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 1998/02/23 21:48:37 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill_rate/m_action.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: m_action.c,v $	$Revision: 1.4 $"

static char rcsid[] = "$Id: m_action.c,v 1.4 1998/02/23 21:48:37 taxiadm Exp $";

#define KEY_SRC
#include "Meter_action_db.h"

void
fill_m_action_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  M_ACTION_STRUCT   *pMA;
  int   offset = 0;
  
  pMA = ( M_ACTION_STRUCT * ) req_buf;

  if ( pMA->trans_cat[1] == ' ' )
    pMA->trans_cat[1] = '\0';
  
  stchar( pMA->trans_cat, &buf[offset], sizeof( pMA->trans_cat ) - 1 );
  offset += sizeof( pMA->trans_cat ) - 1;
  stchar( &pMA->flat_rate, &buf[offset], sizeof( pMA->flat_rate ) );
  offset += sizeof( pMA->flat_rate );
  stchar( &pMA->account_call, &buf[offset], sizeof( pMA->account_call ) );
}

void
fill_m_action( req_buf, buf, action )
     char *req_buf;
     char *buf;
     int action;
{
  int       offset = 0;
  M_ACTION_STRUCT    *pMA;

  pMA = ( M_ACTION_STRUCT * ) req_buf;

  stchar( pMA->trans_cat, &buf[offset], sizeof( pMA->trans_cat ) - 1 );
  offset += sizeof( pMA->trans_cat ) -1;

  stchar( &pMA->flat_rate, &buf[offset], sizeof( pMA->flat_rate ) );
  offset += sizeof( pMA->flat_rate );

  stchar( &pMA->account_call, &buf[offset], sizeof( pMA->account_call ) );
  offset += sizeof( pMA->account_call );

  stchar( pMA->meter_action, &buf[offset], sizeof( pMA->meter_action ) - 1 );
  offset += sizeof( pMA->meter_action ) -1;
}

void
unfill_m_action(req_buf, buf)
     char *req_buf;
     char *buf;
{
  M_ACTION_STRUCT  *pMA;
  int offset = 0;

  pMA = ( M_ACTION_STRUCT * ) req_buf;

  ldchar( &buf[offset], sizeof( pMA->trans_cat ) - 1, pMA->trans_cat );
  offset += sizeof( pMA->trans_cat ) - 1;

  pMA->flat_rate = buf[offset];
  offset += sizeof ( pMA->flat_rate );

  pMA->account_call = buf[offset];
  offset += sizeof ( pMA->account_call );

  ldchar( &buf[offset], sizeof( pMA->meter_action ) - 1, pMA->meter_action );
  offset += sizeof( pMA->meter_action ) - 1;

  if ( pMA->trans_cat[1] == '\0' )
    {
      pMA->trans_cat[1] = ' ';
      pMA->trans_cat[2] = '\0';
    }
}
