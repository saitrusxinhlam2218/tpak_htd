/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: GeoAddr_Line_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2001/10/03 17:54:17 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/GeoAddr_Line_db.h,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: GeoAddr_Line_db.h,v $	$Revision: 1.2 $"

#ifndef _GEOADDR_LINE_DB_H_
#define _GEOADDR_LINE_DB_H_

#include <isam.h>

typedef struct _GEOADDR_LINE_REC_
{
  int   line_idx;
  int   addr_idx;
} GEOADDR_LINE_REC;

#ifdef KEY_SRC
struct keydesc geoaddr_line_key1 =
{
  ISDUPS, 1,
  { 4, LONGSIZE, LONGTYPE }
};
struct keydesc geoaddr_line_key2 =
{
  ISNODUPS, 1,
  { 0, LONGSIZE, LONGTYPE }
};
#else
extern struct keydesc geoaddr_line_key1;
extern struct keydesc geoaddr_line_key2;
#endif

#endif
