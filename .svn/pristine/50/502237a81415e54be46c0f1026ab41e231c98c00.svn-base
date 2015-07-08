/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: GeoAddr_Point_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2001/10/03 17:54:18 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/GeoAddr_Point_db.h,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ifndef _GEOADDR_POINT_DB_H_
#define _GEOADDR_POINT_DB_H_
#include <isam.h>
typedef struct _GEOADDR_POINT_REC_
{
  int point_idx;
  int line_idx;
  short seq_nbr;
  int point_x;
  int point_y;
} GEOADDR_POINT_REC;
#ifdef KEY_SRC
struct keydesc geo_addr_point_key1 =
{
  ISDUPS, 1,
  { 4, LONGSIZE, LONGTYPE }
};
struct keydesc geo_addr_point_key2 =
{
  ISDUPS, 1,
  { 8, INTSIZE, INTTYPE }
};
#else
extern struct keydesc geo_addr_point_key1;
extern struct keydesc geo_addr_point_key2;
#endif
#endif
    
