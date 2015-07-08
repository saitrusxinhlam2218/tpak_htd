/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_card_type.c,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 1998/11/16 13:28:39 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_card_type.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: fill_card_type.c,v $	$Revision: 1.4 $"

static char rcsid[] = "$Id: fill_card_type.c,v 1.4 1998/11/16 13:28:39 taxiadm Exp $";

#define KEY_SRC

#include <isam.h>
#include "Card_type_db.h"
#include "taxi_db.h"

void
fill_card_type_key( char *from, char *buf )
{
  CARD_TYPE_REC *ct = (CARD_TYPE_REC *)from;

  stchar(ct->card_prefix, &buf[4], sizeof(ct->card_prefix)-1);
  
}

void
fill_card_type( req_buf, buf, action )
     char  *req_buf;
     char  *buf;
     int   action;
{
  long serial_nbr = 0;
  register int offset = 0;

  CARD_TYPE_REC *ct = (CARD_TYPE_REC *)req_buf;

  if ( action == ADD_REC )
    {
      isuniqueid(Db_files[CARD_TYPE_FILE_ID].fd, &serial_nbr);
      ct->serial_nbr = serial_nbr;
    }
  stlong(ct->serial_nbr, &buf[offset]);
  offset += LONGSIZE;

  stchar(ct->card_prefix, &buf[offset], sizeof(ct->card_prefix)-1);
  offset += sizeof(ct->card_prefix)-1;

  stchar(&ct->accept_reject, &buf[offset], sizeof(ct->accept_reject));
  offset += sizeof(ct->accept_reject);

  stchar(ct->comment, &buf[offset], sizeof(ct->comment)-1);
  offset += sizeof(ct->comment)-1;

  stlong(ct->entered_dt_tm, &buf[offset]);
  offset += LONGSIZE;

  stchar(ct->card_type_low, &buf[offset], sizeof(ct->card_type_low)-1);
  offset += sizeof(ct->card_type_low)-1;

  stchar(ct->card_type_high, &buf[offset],sizeof(ct->card_type_high)-1);
  offset += sizeof(ct->card_type_high)-1;

  stint(ct->service_index, &buf[offset]);
  offset += INTSIZE;

  stchar(ct->service_nbr, &buf[offset], sizeof(ct->service_nbr) - 1);
  offset += sizeof(ct->service_nbr) - 1;

  stint(ct->type_index, &buf[offset]);
  offset += INTSIZE;
  
  stchar(ct->type_nbr, &buf[offset], sizeof(ct->type_nbr)-1);
  offset += sizeof(ct->type_nbr) - 1;

  stchar(&ct->check_mod, &buf[offset], sizeof(ct->check_mod));
  offset += sizeof(ct->check_mod);  
  
}

void
unfill_card_type( req_buf, buf )
     char *req_buf;
     char *buf;
{
  CARD_TYPE_REC *ct = (CARD_TYPE_REC *) req_buf;
  register int offset = 0;
  
  ct->serial_nbr = ldlong(&buf[offset]);
  offset += LONGSIZE;

  ldchar(&buf[offset],sizeof(ct->card_prefix)-1,ct->card_prefix);
  offset += sizeof(ct->card_prefix)-1;

  ct->accept_reject = buf[offset];
  offset += sizeof( ct->accept_reject );

  ldchar(&buf[offset],sizeof(ct->comment)-1,ct->comment);
  offset += sizeof(ct->comment)-1;

  ct->entered_dt_tm = ldlong(&buf[offset]);
  offset += LONGSIZE;

  ldchar(&buf[offset],sizeof(ct->card_type_low)-1,ct->card_type_low);
  offset += sizeof(ct->card_type_low)-1;

  ldchar(&buf[offset], sizeof(ct->card_type_high)-1,ct->card_type_high);
  offset += sizeof(ct->card_type_high)-1;

  ct->service_index = ldint(&buf[offset]);
  offset += INTSIZE;

  ldchar(&buf[offset], sizeof(ct->service_nbr)-1,ct->service_nbr);
  offset += sizeof(ct->service_nbr)-1;

  ct->type_index = ldint(&buf[offset]);
  offset += INTSIZE;

  ldchar(&buf[offset], sizeof(ct->type_nbr)-1,ct->type_nbr);
  offset += sizeof(ct->service_nbr)-1;

  ct->check_mod = buf[offset];
  offset += sizeof( ct->check_mod );  
}
