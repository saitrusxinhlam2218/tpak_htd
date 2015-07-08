/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_tl.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2000/05/24 14:03:12 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_tl.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: fill_tl.c,v 1.3 2000/05/24 14:03:12 taxiadm Exp $";

#define KEY_SRC

#include "Tracking_Layer_db.h"
#include "Tracking_Layer_private.h"

void
fill_tl_key( req_buf, buf )
     char *req_buf;
     char *buf;
{
  TL_STRUCT      *tl;
  register int offset = 0;

  tl = (TL_STRUCT *)req_buf;

  stchar(&tl->fleet,&buf[offset], sizeof(tl->fleet));
  offset += sizeof(tl->fleet);

  stint(tl->veh_nbr, &buf[TL_VEH_NBR_OFFSET]);

  stlong(tl->date_time,&buf[TL_DATE_TIME_OFFSET]);
}
void
fill_tl(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int  action;
{
  TL_STRUCT   *tl;
  register int offset = 0;

  tl = (TL_STRUCT *)req_buf;

  stchar(&tl->fleet,&buf[offset], sizeof(tl->fleet));
  offset += sizeof(tl->fleet);

  stint(tl->veh_nbr,&buf[offset]);
  offset += INTSIZE;

  stdbl(tl->long_x,&buf[offset]);
  offset += DOUBLESIZE;

  stdbl(tl->lat_y,&buf[offset]);
  offset += DOUBLESIZE;

  stlong(tl->gps_time,&buf[offset]);
  offset += LONGSIZE;

  stlong(tl->date_time,&buf[offset]);
  offset += LONGSIZE;

  stchar(tl->date, &buf[offset], sizeof(tl->date) - 1);
  offset += sizeof(tl->date) - 1;

  stchar(tl->time, &buf[offset], sizeof(tl->time) - 1);
  offset += sizeof(tl->time) - 1;

  stchar(&tl->receiver_status, &buf[offset], sizeof(tl->receiver_status));
  offset += sizeof(tl->receiver_status);
  
}


void
unfill_tl( req_buf, buf )
     char *req_buf;
     char *buf;
{
  TL_STRUCT *tl;
  register int offset = 0;
  short    code = 0;

  tl = (TL_STRUCT *)req_buf;

  tl->fleet = buf[offset];
  offset += sizeof(tl->fleet);

  tl->veh_nbr = ldint(&buf[offset]);
  offset += INTSIZE;

  tl->long_x = lddblnull(&buf[offset], &code);
  offset += DOUBLESIZE;

  tl->lat_y = lddblnull(&buf[offset], &code);
  offset += DOUBLESIZE;

  tl->gps_time = ldlong(&buf[offset]);
  offset += LONGSIZE;

  tl->date_time = ldlong(&buf[offset]);
  offset += LONGSIZE;

  ldchar(&buf[offset], sizeof(tl->date)-1, tl->date);
  offset += sizeof(tl->date)-1;

  ldchar(&buf[offset], sizeof(tl->time)-1, tl->time);
  offset += sizeof(tl->time)-1;

  tl->receiver_status = buf[offset];
  offset += sizeof(tl->receiver_status);
  
}
