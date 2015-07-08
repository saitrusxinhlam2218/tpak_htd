/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Holiday_rate_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:43:32 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Holiday_rate_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _HOL_RATE_DB_H
#define _HOL_RATE_DB_H
#include <isam.h>
typedef struct _HOLIDAY_STRUCT
{
  int          id;
  char         start_dt[ 9 ];
  char         end_dt[ 9 ];
  char         start_tm[ 7 ];
  char         end_tm[ 7 ];
  char         descript[ 17 ];
  short        tp_nbr;
} HOLIDAY_STRUCT;
#ifdef KEY_SRC
struct keydesc holiday_key1 =
{
  ISDUPS,
  1,
  {
    { 4, 8, CHARTYPE },
  }
};
#else
extern struct keydesc holiday_key1;
#endif
#endif
