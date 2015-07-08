/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Res_periods_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:14 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Res_periods_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _RES_PERIODS_DB_H
#define _RES_PERIODS_DB_H
#include <isam.h>
#define DAYS_PER_WEEK          7
#define MAX_TIME_OF_DAY        2359
#define RES_HOLIDAY_IDX        7
#define RP_DAYS_PER_WEEK       (DAYS_PER_WEEK + 1)
#define RES_MAX_PERIODS        20
#define RES_PERIOD_MEM_SIZE    (sizeof(RES_PERIODS) * RP_DAYS_PER_WEEK)
#define RP_PERIOD_NOT_APPLIES  '0'
#define RP_PERIOD_APPLIES      '1'
typedef int TIME_OF_DAY;
typedef struct rp_def
{
  short         period;
  TIME_OF_DAY   start_time;
  TIME_OF_DAY   end_time;
  float         lead_time_skew;
} RP_DEF;
typedef struct rp_rec
{
  RP_DEF        period_definition;
  char          days_of_week[ RP_DAYS_PER_WEEK + 1];
} RP_REC;
typedef struct res_periods
{
  short         end;
  RP_DEF        info[RES_MAX_PERIODS];
} RES_PERIODS;
#ifdef KEY_SRC
struct keydesc rp_key1 =
{
  ISDUPS,
  1,
  { 0, INTSIZE, INTTYPE }
};
#else
extern struct keydesc rp_key1;
#endif
#endif
