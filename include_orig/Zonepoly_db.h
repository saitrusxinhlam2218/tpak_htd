/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Zonepoly_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2000/03/02 19:44:22 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Zonepoly_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Zonepoly_db.h,v $	$Revision: 1.2 $"

#ifndef _ZONEPOLY_DB_H
#define _ZONEPOLY_DB_H

#include <isam.h>
#include "Zone_private.h"

#ifdef KEY_SRC

struct keydesc znpoly_key1= {
  ISDUPS,
  3,
  { {0,CHARSIZE,CHARTYPE},
      {1,INTSIZE,INTTYPE},
      {3,INTSIZE,INTTYPE}}
};
#else
extern struct keydesc znpoly_key1;
#endif
#endif
