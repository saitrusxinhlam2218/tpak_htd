/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_tlg.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/02/23 21:48:07 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_tlg.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: fill_tlg.c,v $	$Revision: 1.3 $"

static char rcsid[] = "$Id: fill_tlg.c,v 1.3 1998/02/23 21:48:07 taxiadm Exp $";

#define KEY_SRC
#include "Tlg_db.h"

void
fill_tlg_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  struct cisam_tlg      *tlg;
  register int offset = 0;

  tlg = (struct cisam_tlg *)req_buf;
  stlong(tlg->tlg_nbr, &buf[offset]);
}

void
fill_tlg(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int action;
{
  struct cisam_tlg   *tlg;
  register int offset = 0;

  tlg = (struct cisam_tlg *)req_buf;

  stlong(tlg->tlg_nbr, &buf[offset]);
  offset += sizeof(tlg->tlg_nbr);

  stlong(tlg->tlg_lo_range, &buf[offset]);
  offset += sizeof(tlg->tlg_lo_range);

  stlong(tlg->tlg_hi_range, &buf[offset]);
  offset += sizeof(tlg->tlg_hi_range);

  stchar(&tlg->tlg_fleet, &buf[offset], sizeof(tlg->tlg_fleet));
  offset += sizeof(tlg->tlg_fleet);

  stchar(tlg->tlg_cmnt, &buf[offset], sizeof(tlg->tlg_cmnt));
  offset += sizeof(tlg->tlg_cmnt);
}

void
unfill_tlg( req_buf, buf)
     char *req_buf;
     char *buf;
{
  struct cisam_tlg   *tlg;
  register int index  = 0;

  tlg = (struct cisam_tlg *)req_buf;

  tlg->tlg_nbr = ldlong(&buf[index]);
  index += sizeof(tlg->tlg_nbr);

  tlg->tlg_lo_range = ldlong(&buf[index]);
  index += sizeof(tlg->tlg_lo_range);

  tlg->tlg_hi_range = ldlong(&buf[index]);
  index += sizeof(tlg->tlg_hi_range);

  ldchar(&buf[index], sizeof(tlg->tlg_fleet), &tlg->tlg_fleet);
  index += sizeof(tlg->tlg_fleet);

  ldchar(&buf[index], sizeof(tlg->tlg_cmnt), tlg->tlg_cmnt);
  index += sizeof(tlg->tlg_cmnt);
}
	 

  
