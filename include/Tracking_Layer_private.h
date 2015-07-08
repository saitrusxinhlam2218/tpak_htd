/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Tracking_Layer_private.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2000/05/24 14:02:58 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Tracking_Layer_private.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _TL_PRIVATE_H
#define _TL_PRIVATE_H
#include <sys/time.h>
typedef struct _TL_STRUCT_
{
  char         fleet;
  short        veh_nbr;
  double       lat_y;
  double       long_x;
  int          gps_time;
  time_t       date_time;
  char         date[10];
  char         time[7];
  char         receiver_status;
} TL_STRUCT;
#endif
