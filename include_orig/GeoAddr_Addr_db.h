/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: GeoAddr_Addr_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2001/10/03 17:54:15 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/GeoAddr_Addr_db.h,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: GeoAddr_Addr_db.h,v $	$Revision: 1.2 $"

#ifndef _GEOADDR_ADDR_DB_H_
#define _GEOADDR_ADDR_DB_H_

#include <isam.h>


#define GEOADDR_SNAME_LEN   20
#define GEOADDR_CITY_LEN     3

typedef struct _GEOADDR_ADDR_REC
{
  int    addr_idx;
  short  from_left;
  short  to_left;
  short  from_right;
  short  to_right;
  char   sname[ GEOADDR_SNAME_LEN + 1 ];
  char   city[ GEOADDR_CITY_LEN + 1 ];
} GEOADDR_ADDR_REC;


#ifdef KEY_SRC
struct keydesc geoaddr_addr_key1 =
{
  ISDUPS, 2,
  { { 12, 20, CHARTYPE },
    { 32, 3, CHARTYPE } }
};
struct keydesc geoaddr_addr_key2 =
{
  ISNODUPS, 1,
  { 0, LONGSIZE, LONGTYPE }
};
#else
extern struct keydesc geoaddr_addr_key1;
extern struct keydesc geoaddr_addr_key2;
#endif

#endif
