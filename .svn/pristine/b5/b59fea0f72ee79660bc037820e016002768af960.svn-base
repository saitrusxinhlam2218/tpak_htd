
#define KEY_SRC
#include "taxipak.h"
#include "Object.h"
#include "ZoneDown_db.h"

void
fill_zonedown_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  struct cisam_zonedown *zd;
  register int offset = 0;

  zd = (struct cisam_zonedown *)req_buf;
  stchar(&zd->fleet,&buf[offset],sizeof(zd->fleet));
  offset += sizeof(zd->fleet);
  stint(zd->veh_nbr,&buf[offset]);
  offset += INTSIZE;
  stint(zd->zone_nbr,&buf[offset]);
  offset += INTSIZE;

}

void fill_zonedown(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int action;

{
  struct cisam_zonedown *zd;
  register int offset = 0;
  register i;
  register j;

  zd = (struct cisam_zonedown *)req_buf;

  stchar(&zd->fleet,&buf[offset],sizeof(zd->fleet));
  offset += sizeof(zd->fleet);

  stint(zd->veh_nbr,&buf[offset]);
  offset += INTSIZE;

  stint(zd->zone_nbr,&buf[offset]);
  offset += INTSIZE;

  stlong(zd->entered_datetime,&buf[offset]);
  offset += LONGSIZE;

  stchar(&zd->flag,&buf[offset],sizeof(zd->flag));
  offset += sizeof(zd->flag);  
}

void
unfill_zonedown(req_buf, buf)
     char *req_buf;
     char *buf;
{
  struct cisam_zonedown *zd;
  register int offset = 0;
  register i;
  register j;
  short code = 0;

  zd = (struct cisam_zonedown *)req_buf;

  zd->fleet = buf[offset];
  offset += CHARSIZE;

  zd->veh_nbr = ldint(&buf[offset]);
  offset += INTSIZE;

  zd->zone_nbr = ldint(&buf[offset]);
  offset += INTSIZE;

  zd->entered_datetime = ldlong(&buf[offset]);
  offset += LONGSIZE;

  zd->flag = buf[offset];
  offset += CHARSIZE;
}
