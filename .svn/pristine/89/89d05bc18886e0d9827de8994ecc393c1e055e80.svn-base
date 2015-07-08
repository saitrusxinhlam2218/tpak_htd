/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Dist_filter_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:42:51 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Dist_filter_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Dist_filter_db.h,v $	$Revision: 1.2 $"

#ifndef _DIST_FILTER_DB_H
#define _DIST_FILTER_DB_H

#include <isam.h>
typedef struct _DIST_FILTER_STRUCT
{
  char          trans_cat[ 3 ];
  float         distance;
  float         factor_val;
  float         extra_val;
} DIST_FILTER_STRUCT;
#ifdef KEY_SRC
struct keydesc dist_filter_key1 =
{
  ISDUPS,
  1,
  {
    { 0, 2, CHARTYPE },
  }
};

struct keydesc dist_filter_key2 =
{
  ISDUPS,
  2,
  {
    { 0, 2, CHARTYPE },
    { 2, 4, FLOATTYPE },
  }
};
#else
extern struct keydesc dist_filter_key1;
extern struct keydesc dist_filter_key2;
#endif

#endif
