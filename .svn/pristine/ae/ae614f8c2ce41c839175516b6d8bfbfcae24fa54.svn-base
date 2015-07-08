/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Zone_db.h,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 2002/03/21 14:52:16 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Zone_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _ZONE_DB_H
#define _ZONE_DB_H
#include <isam.h>
#include "Zone_public.h"
#include "Zone_private.h"
#ifdef KEY_SRC
struct keydesc zn_key1=  {
	 ISNODUPS,
	 2,
	 {     {0,CHARSIZE,CHARTYPE},
				 {2,INTSIZE,INTTYPE} }
};
struct keydesc zn_key2= {
  ISDUPS,
  1,
  { {260,18,CHARTYPE} }
};
#else
extern struct keydesc zn_key1;
extern struct keydesc zn_key2;
#endif
#endif
