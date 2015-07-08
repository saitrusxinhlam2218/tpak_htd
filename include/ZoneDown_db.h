/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ZoneDown_db.h,v $
*  @(#)  $Revision: 1.1 $
*  @(#)  $Date: 2007/01/24 14:52:14 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/ZoneDown_db.h,v $
*
*  Copyright (c) 2007 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ifndef _ZONEDOWN_DB_H
#define _ZONEDOWN_DB_H
#include <isam.h>
#define ZONEDOWN_KEY 0
typedef struct cisam_zonedown
{
  char fleet;
  short veh_nbr;
  short zone_nbr;
  int  entered_datetime;
  char flag;
} ZONEDOWN_REC;
#ifdef KEY_SRC
struct keydesc zonedown_key1={
  ISNODUPS,
  3,
  { { 0, CHARSIZE, CHARTYPE },
    { 1, INTSIZE, INTTYPE },
    { 3, INTSIZE, INTTYPE } }
};
#else
extern struct keydesc zonedown_key1;
#endif
#endif
