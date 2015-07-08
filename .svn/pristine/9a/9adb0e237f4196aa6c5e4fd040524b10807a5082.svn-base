/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Trip_data_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:54 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Trip_data_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Trip_data_db.h,v $	$Revision: 1.2 $"

#ifndef _TRIP_DATA_DB_H
#define _TRIP_DATA_DB_H

#include <isam.h>

typedef struct _TRIPDATA_STRUCT
{
  int          td_id;
  char         td_transcat[ 3 ];
  int          td_timeperiod;
  float        td_triplength_FV;
  float        td_triptime_FV;
  float        td_triplength_BR;
  float        td_triptime_BR;
} TRIPDATA_STRUCT;

#ifdef KEY_SRC
struct keydesc td_key1=
{
  ISNODUPS,
  2,
  {
    { 4, 2, CHARTYPE },
    { 6, LONGSIZE, LONGTYPE }
  }
};
#else
extern struct keydesc td_key1;
#endif

#endif
