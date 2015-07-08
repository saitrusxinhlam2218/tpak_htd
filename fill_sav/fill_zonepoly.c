/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_zonepoly.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2000/03/02 19:41:04 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_zonepoly.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: fill_zonepoly.c,v $	$Revision: 1.2 $"

static char rcsid[] = "$Id: fill_zonepoly.c,v 1.2 2000/03/02 19:41:04 taxiadm Exp $";


#define KEY_SRC
#include "taxipak.h"
#include "Object.h"
#include "Zonepoly_db.h"

void
fill_zonepoly_key( req_buf, buf )
     char *req_buf;
     char *buf;
{
  struct cisam_zn_poly *zone;
  register int offset = 0;

  zone = ( struct cisam_zn_poly *)req_buf;
  stchar(&zone->fleet,&buf[offset],sizeof(zone->fleet));
  offset = sizeof(zone->fleet);
  stint(zone->nbr,&buf[offset]);
  offset += INTSIZE;
  stint(zone->seq_nbr, &buf[offset]);
}  

void
fill_zonepoly(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int    action;
{
  struct  cisam_zn_poly  *zone;
  register int  offset = 0;
  register i;
  register j;
  
  zone = (struct cisam_zn_poly *)req_buf;
  
  stchar(&zone->fleet,&buf[offset],sizeof(zone->fleet));
  offset += sizeof(zone->fleet);
  
  stint(zone->nbr,&buf[offset]);
  offset += INTSIZE;

  stint(zone->seq_nbr, &buf[offset]);
  offset += INTSIZE;
  
  stdbl(zone->pointx, &buf[offset]);
  offset += DOUBLESIZE;
  
  stdbl(zone->pointy, &buf[offset]);
  offset += DOUBLESIZE;

  stchar(zone->latitude, &buf[offset], sizeof(zone->latitude)-1);
  offset += sizeof(zone->latitude)-1;

  stchar(zone->longitude, &buf[offset], sizeof(zone->longitude)-1);
  offset += sizeof(zone->longitude)-1;
  
}

void
unfill_zonepoly(req_buf, buf)
     char *req_buf;
     char *buf;
{
  struct  cisam_zn_poly  *zone;
  register int  offset = 0;
  register i;
  register j;
  short code = 0;
  
  zone = (struct cisam_zn_poly *)req_buf;
  
  zone->fleet = buf[offset];
  offset += CHARSIZE;
  
  zone->nbr = ldint(&buf[offset]);
  offset += INTSIZE;

  zone->seq_nbr = ldint(&buf[offset]);
  offset += INTSIZE;
  
  zone->pointx = lddblnull(&buf[offset], &code);
  offset += DOUBLESIZE;
  
  zone->pointy = lddblnull(&buf[offset], &code);
  offset += DOUBLESIZE;

  ldchar(&buf[offset], sizeof(zone->latitude)-1, zone->latitude);
  offset += sizeof(zone->latitude)-1;
  
  ldchar(&buf[offset], sizeof(zone->longitude)-1, zone->longitude);
  offset += sizeof(zone->longitude)-1;

}
