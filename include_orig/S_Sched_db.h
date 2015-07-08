/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: S_Sched_db.h,v $
*  @(#)  $Revision: 1.1 $
*  @(#)  $Date: 2007/01/24 14:52:14 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/S_Sched_db.h,v $
*
*  Copyright (c) 2007 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: S_Sched_db.h,v $	$Revision: 1.1 $"

#ifndef _S_SCHED_DB_H
#define _S_SCHED_DB_H

#define SH_KEY 0
#define SH_SCHEDULE_NUM 4
#define SH_MONTH 8
#define SH_FLEET 10

typedef struct cisam_sh
{
  int key;
  int schedule_num;
  short month;
  char fleet;
} S_SCHED_REC;

#ifdef KEY_SRC
struct keydesc s_sh_key1={
  ISNODUPS,
  1,
  { { SH_KEY, LONGSIZE, LONGTYPE } }
};

struct keydesc s_sh_key2={
  ISNODUPS,
  3,
  { { SH_SCHEDULE_NUM, LONGSIZE, LONGTYPE },
    { SH_MONTH, INTSIZE, INTTYPE },
    { SH_FLEET, CHARSIZE, CHARTYPE }
  }
};

struct keydesc s_sh_key3={
  ISDUPS,
  2,
  { { SH_SCHEDULE_NUM, LONGSIZE, LONGTYPE },
    { SH_FLEET, CHARSIZE, CHARTYPE }
  }
};

#else
extern struct keydesc s_sh_key1;
extern struct keydesc s_sh_key2;
extern struct keydesc s_sh_key3;
#endif

#endif
