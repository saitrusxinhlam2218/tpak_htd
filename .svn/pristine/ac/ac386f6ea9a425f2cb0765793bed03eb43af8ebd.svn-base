/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: tperiods.c,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 1998/02/23 21:48:46 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill_rate/tperiods.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: tperiods.c,v $	$Revision: 1.4 $"

static char rcsid[] = "$Id: tperiods.c,v 1.4 1998/02/23 21:48:46 taxiadm Exp $";

#define KEY_SRC
#include "Tperiods_db.h"

void
fill_tperiods_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  TPERIODS_STRUCT      *pTperiods;

  pTperiods = ( TPERIODS_STRUCT * ) req_buf;

  stlong( pTperiods->tp_id, &buf[ 0 ] );

  stchar( &pTperiods->tp_day_of_week, &buf[ 4 ], sizeof( pTperiods->tp_day_of_week ) );
  
}

void
fill_tperiods( req_buf, buf, action )
     char *req_buf;
     char *buf;
     int       action;
{
  TPERIODS_STRUCT      *pTperiods;
  int                  offset = 0;

  pTperiods = ( TPERIODS_STRUCT * ) req_buf;

  stlong( pTperiods->tp_id, &buf[ offset ] );
  offset += LONGSIZE;

  stchar( &pTperiods->tp_day_of_week, &buf[ offset ], sizeof( pTperiods->tp_day_of_week ) );
  offset += sizeof( pTperiods->tp_day_of_week );

  stchar( pTperiods->tp_start_time, &buf[ offset ], sizeof( pTperiods->tp_start_time ) - 1 );
  offset += sizeof( pTperiods->tp_start_time ) - 1;

  stchar( pTperiods->tp_end_time, &buf[ offset ], sizeof( pTperiods->tp_end_time ) - 1 );
  offset += sizeof( pTperiods->tp_end_time ) - 1;

  stint( pTperiods->tp_nbr, &buf[ offset ] );
  offset += INTSIZE;

  stchar( pTperiods->tp_descript, &buf[ offset ], sizeof( pTperiods->tp_descript ) - 1 );
  offset += sizeof( pTperiods->tp_descript ) - 1;
}

void
unfill_tperiods( req_buf, buf )
     char *req_buf;
     char *buf;
{
  TPERIODS_STRUCT            *pTperiods;
  int                        offset = 0;

  pTperiods = ( TPERIODS_STRUCT * ) req_buf;

  pTperiods->tp_id = ldlong( &buf[ offset ] );
  offset += LONGSIZE;

  pTperiods->tp_day_of_week = buf[ offset ];
  offset += sizeof( pTperiods->tp_day_of_week );

  ldchar( &buf[ offset ], sizeof( pTperiods->tp_start_time ) - 1, pTperiods->tp_start_time );
  offset += sizeof( pTperiods->tp_start_time ) - 1;

  ldchar( &buf[ offset ], sizeof( pTperiods->tp_end_time ) - 1, pTperiods->tp_end_time );
  offset += sizeof( pTperiods->tp_end_time ) - 1;

  pTperiods->tp_nbr = ldint( &buf[ offset ] );
  offset += INTSIZE;

  ldchar( &buf[ offset ], sizeof( pTperiods->tp_descript ) - 1, pTperiods->tp_descript );
  offset += sizeof( pTperiods->tp_descript ) - 1;
}


  

  
