/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Tlg_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:47 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Tlg_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _TLG_DB_H
#define _TLG_DB_H
#include <isam.h>
typedef struct cisam_tlg {
  int      tlg_nbr;
  int      tlg_lo_range;
  int      tlg_hi_range;
  char     tlg_fleet;
  char     tlg_cmnt[32];
}TLG_REC;
#ifdef KEY_SRC
struct keydesc tlg_key1= {
  ISNODUPS,
  1,
  0, LONGSIZE, LONGTYPE,
};
#else
extern struct keydesc tlg_key1;
#endif
#endif
