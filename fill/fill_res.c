/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_res.c,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 1998/02/23 21:47:46 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_res.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: fill_res.c,v 1.4 1998/02/23 21:47:46 taxiadm Exp $";

#define KEY_SRC
#include "Res_periods_db.h"

void fill_rp( char *, char *, int );

void
fill_rp_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  (void)fill_rp( req_buf, buf, -1 );		/* just fill it all */
}

void
fill_rp(char *req_buf, char *buf, int action)
{
  register int  offset = 0;
  RP_REC        *from;
  
  from = ( RP_REC * ) req_buf;
  /* RP_REC.period_definition.period */
  
  stint(((RP_REC *) from)->period_definition.period, &buf[offset]);
  offset += INTSIZE;
  
  /* NOTE: Since TIME_OF_DAY is an int, we have a problem here.  Kludge offset for now... */
  
  /* RP_REC.period_definition.start_time */
  
  stint(((RP_REC *) from)->period_definition.start_time, &buf[offset]);
  offset += INTSIZE;
  
  /* RP_REC.period_definition.end_time */
  
  stint(((RP_REC *) from)->period_definition.end_time, &buf[offset]);
  offset += INTSIZE;
  
  /* RP_REC.days_of_week */
  
  stchar(((RP_REC *) from)->days_of_week, &buf[offset], sizeof(((RP_REC *) from)->days_of_week)-1);
  offset += sizeof(((RP_REC *) from)->days_of_week)-1;
  
  /* RP_REC.lead_time_skew */
  
  stfloat(((RP_REC *) from)->period_definition.lead_time_skew, &buf[offset]);
  offset += FLOATSIZE;
  
  /* RP_REC.period_load */
  
}

void
unfill_rp(req_buf, buf)
     char *req_buf;
     char *buf;
{
  int i;
  register int  offset = 0;
  RP_REC        *to;
  
  /* RP_REC.period_definition.period */
  to = ( RP_REC * ) req_buf;
  
  ((RP_REC *) to)->period_definition.period = (short) ldint(&buf[offset]);
  offset += INTSIZE;
  
  /* NOTE: Since TIME_OF_DAY is an int, we have a problem here.  Kludge offset for now... */
  
  /* RP_REC.period_definition.start_time */
  
  ((RP_REC *) to)->period_definition.start_time = (short) ldint(&buf[offset]);
  offset += INTSIZE;
  
  /* RP_REC.period_definition.end_time */
  
  ((RP_REC *) to)->period_definition.end_time = (short) ldint(&buf[offset]);
  offset += INTSIZE;
  
  /* RP_REC.days_of_week */
  
  ldchar(&buf[offset], sizeof(((RP_REC *) to)->days_of_week)-1, ((RP_REC *) to)->days_of_week);
  offset += sizeof(((RP_REC *) to)->days_of_week)-1;
  
  /* RP_REC.lead_time_skew */
  
  ((RP_REC *) to)->period_definition.lead_time_skew = ldfloat(&buf[offset]);
  offset += FLOATSIZE;
  
}
  
  
