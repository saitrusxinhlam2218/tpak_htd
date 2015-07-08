/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: S_Shift_db.h,v $
*  @(#)  $Revision: 1.1 $
*  @(#)  $Date: 2007/01/24 14:52:14 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/S_Shift_db.h,v $
*
*  Copyright (c) 2007 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: S_Shift_db.h,v $	$Revision: 1.1 $"

#ifndef _S_SHIFT_DB_H
#define _S_SHIFT_DB_H

#define SS_SCHEDULE_KEY 0
#define SS_DAY 4
#define SS_VERSION 32

typedef struct cisam_s_version
{
  int vers_id;
  char active;
  char desc[65];
} S_VERSION_REC;

typedef struct cisam_ss
{
  int schedule_key;
  int day;
  int start_1;
  int end_1;
  int start_2;
  int end_2;
  int start_3;
  int end_3;
  int vers_id;
} S_SHIFT_REC;

#ifdef KEY_SRC
struct keydesc ss_key1={
  ISDUPS,
  1,
  { { SS_SCHEDULE_KEY, LONGSIZE, LONGTYPE } }
};

struct keydesc ss_key2={
  ISNODUPS,
  2,
  { { SS_SCHEDULE_KEY, LONGSIZE, LONGTYPE },
    { SS_DAY, LONGSIZE, LONGTYPE }
  }
};

struct keydesc ss_key3={
  ISDUPS,
  3,
  { { SS_SCHEDULE_KEY, LONGSIZE, LONGTYPE },
    { SS_DAY, LONGSIZE, LONGTYPE },
    { SS_VERSION, LONGSIZE, LONGTYPE }
  }
};

struct keydesc sv_key1={
  ISNODUPS,
  1,
  { { 0, LONGSIZE, LONGTYPE } }
};

#else
extern struct keydesc ss_key1;
extern struct keydesc ss_key2;
extern struct keydesc ss_key3;
extern struct keydesc sv_key1;
#endif
  
#endif


  
