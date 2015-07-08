/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Tperiods_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:51 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Tperiods_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _TPERIODS_DB_H
#define _TPERIODS_DB_H
#include <isam.h>
typedef struct _TPERIODS_STRUCT
{
  int          tp_id;
  char         tp_day_of_week;
  char         tp_start_time[ 7 ];
  char         tp_end_time[ 7 ];
  short        tp_nbr;
  char         tp_descript[ 33 ];
} TPERIODS_STRUCT;
#ifdef KEY_SRC
struct keydesc tp_key1= {
  ISNODUPS,
  1,
  { { 0, LONGSIZE, LONGTYPE },
  }
};
struct keydesc tp_key2= {
  ISDUPS,
  1,
  { { 4, 1, CHARTYPE },
  }
};
#else
extern struct keydesc tp_key1;
extern struct keydesc tp_key2;
#endif
#endif
