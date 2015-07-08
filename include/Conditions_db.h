/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Conditions_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:42:42 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Conditions_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _CONDITIONS_DB_H
#define _CONDITIONS_DB_H
#include <isam.h>
#define COND_SEASON          1
#define COND_RELATION        2
#define COND_VEH_ATTR        3
#define COND_DRV_ATTR        4
#define COND_VIA_ADDR        5
#define COND_FIXED  'F'
#define COND_VARIABLE 'V'
typedef struct _CONDITIONS_STRUCT
{
  int          cond_id;
  char         cond_transcat[ 3 ];
  int          cond_nbr;
  char         cond_category;
  float        cond_FV;
  float        cond_EV;
} CONDITIONS_STRUCT;
#ifdef KEY_SRC
struct keydesc cond_key1=
{
  ISDUPS,
  1,
  {
    { 4, 2, CHARTYPE },
  }
};
#else
extern struct keydesc cond_key1;
#endif
#endif
