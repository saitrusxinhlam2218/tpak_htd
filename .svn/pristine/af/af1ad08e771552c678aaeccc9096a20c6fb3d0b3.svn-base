/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_bad_credit.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/03/12 22:01:12 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_bad_credit.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: fill_bad_credit.c,v $	$Revision: 1.2 $"

static char rcsid[] = "$Id: fill_bad_credit.c,v 1.2 1998/03/12 22:01:12 taxiadm Exp $";

#define KEY_SRC

#include <isam.h>
#include "Bad_credit_db.h"
#include "taxi_db.h"

void
fill_bad_credit_key( char *from, char *buf )
{

  BAD_CREDIT_REC *bc = (BAD_CREDIT_REC *)from;

  stchar(bc->card_nbr, &buf[CARD_NBR_OFFSET], sizeof(bc->card_nbr)-1);
  
}

void
fill_bad_credit( req_buf, buf, action )
     char  *req_buf;
     char  *buf;
     int   action;
{
  long serial_nbr = 0;
  register int offset = 0;
  BAD_CREDIT_REC *bc = (BAD_CREDIT_REC *)req_buf;

  if ( action == ADD_REC )
    {
      isuniqueid(Db_files[BAD_CREDIT_FILE_ID].fd, &serial_nbr);
      bc->serial_nbr = serial_nbr;
    }
  stlong(bc->serial_nbr, &buf[offset]);
  offset += LONGSIZE;

  stchar(&bc->date_range, &buf[offset], sizeof(bc->date_range));
  offset += sizeof(bc->date_range);

  stchar(bc->date_start, &buf[offset], sizeof(bc->date_start)-1);
  offset += sizeof(bc->date_start)-1;

  stchar(bc->date_end, &buf[offset], sizeof(bc->date_end)-1);
  offset += sizeof(bc->date_end)-1;

  stchar(bc->card_nbr, &buf[offset], sizeof(bc->card_nbr)-1);
  offset += sizeof(bc->card_nbr)-1;

  stchar(bc->card_type, &buf[offset], sizeof(bc->card_type)-1);
  offset += sizeof(bc->card_type)-1;

  stlong(bc->entered_dt_tm, &buf[offset]);
  offset += LONGSIZE;
}

void
unfill_bad_credit( req_buf, buf )
     char *req_buf;
     char *buf;
{
  BAD_CREDIT_REC *bc = (BAD_CREDIT_REC *)req_buf;
  register int offset = 0;

  bc->serial_nbr = ldlong(&buf[offset]);
  offset += LONGSIZE;

  bc->date_range = buf[offset];
  offset += sizeof(bc->date_range);

  ldchar(&buf[offset],sizeof(bc->date_start)-1,bc->date_start);
  offset += sizeof(bc->date_start)-1;

  ldchar(&buf[offset],sizeof(bc->date_end)-1,bc->date_end);
  offset += sizeof(bc->date_end)-1;

  ldchar(&buf[offset],sizeof(bc->card_nbr)-1,bc->card_nbr);
  offset += sizeof(bc->card_nbr)-1;

  ldchar(&buf[offset],sizeof(bc->card_type)-1,bc->card_type);
  offset += sizeof(bc->card_type)-1;

  bc->entered_dt_tm = ldlong(&buf[offset]);
  offset += LONGSIZE;
}
