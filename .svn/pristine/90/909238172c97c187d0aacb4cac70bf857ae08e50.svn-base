/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Transcat_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:52 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Transcat_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _TRANSCAT_DB_H
#define _TRANSCAT_DB_H
#include <isam.h>
typedef struct _TRANSCAT_STRUCT
{
  int          tc_id;
  char         tc_name[ 3 ];
  char         tc_descript[ 33 ];
  float        tc_vat;
  float        tc_min_fare;
} TRANSCAT_STRUCT;
#ifdef KEY_SRC
struct keydesc tc_key1= {
  ISNODUPS,
  1,
  { { 4, 2, CHARTYPE },
  }
};
#else
extern struct keydesc tc_key1;
#endif
#endif
