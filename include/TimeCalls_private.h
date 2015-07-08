/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: TimeCalls_private.h,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 1998/05/13 02:30:55 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/TimeCalls_private.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _TIMECALLS_PRIVATE_H
#define _TIMECALLS_PRIVATE_H
typedef struct _tc_struct
{
  int                nbr;
  time_t             due_time;
  time_t             lead_time;
  int                fleet;
  short              zone;
  char               drv_attr[33];
  char               veh_attr[33];
  unsigned int       personal:1;
  unsigned int       mult:1;
  struct _tc_struct  *next;
} TC_STRUCT;
#define TC_DEBUG_LOAD      0
#define TC_DEBUG_LAUNCH    1
#define TC_DEBUG_REMOVE    2
#define TIME_CALLS_MAX   8000
#endif
