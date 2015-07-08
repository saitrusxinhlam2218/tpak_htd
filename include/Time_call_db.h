/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Time_call_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:41 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Time_call_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _TIME_CALL_DB_H
#define _TIME_CALL_DB_H
#include <isam.h>
typedef struct _time_call_rec
{
  char          fleet;
  short         zone;
  time_t        due_date_time;
  int           cl_nbr;
  char          drv_attr[65];
  char          veh_attr[65];
} TIME_CALL_REC;
#ifdef KEY_SRC
struct keydesc time_call_key1 = {
  ISNODUPS,
  1,
  { 7, LONGSIZE, LONGTYPE},
};
struct keydesc time_call_key2 = {
  ISDUPS,
  3,
  {
    { 0, CHARSIZE, CHARTYPE },
    { 1, INTSIZE, INTTYPE },
    { 3, LONGSIZE, LONGTYPE },
  }
};
#else
extern struct keydesc time_call_key1;
extern struct keydesc time_call_key2;
#endif
#endif
