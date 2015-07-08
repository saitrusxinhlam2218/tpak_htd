/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Relations_data_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:10 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Relations_data_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _RELATIONS_DATA_DB_H
#define _RELATIONS_DATA_DB_H
#include <isam.h>
typedef struct _REL_DATA_STRUCT
{
  int          id;
  int          nbr;
  int          time_period;
  float        FV;
  float        EV;
} REL_DATA_STRUCT;
#ifdef KEY_SRC
struct keydesc rel_data_key1 =
{
  ISDUPS,
  1,
  {
    { 4, LONGSIZE, LONGTYPE },
  }
};
struct keydesc rel_data_key2 =
{
  ISDUPS,
  2,
  {
    { 4, LONGSIZE, LONGTYPE },
    { 8, LONGSIZE, LONGTYPE },
  }
};
#else
extern struct keydesc rel_data_key1;
extern struct keydesc rel_data_key2;
#endif
#endif
