/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ZoneTrans_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2002/03/21 14:52:16 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/ZoneTrans_db.h,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ifndef _ZONETRANS_DB_H
#define _ZONETRANS_DB_H
#define ZONETRANS_T800_ID_LEN   5
#define ZONETRANS_COMMENT_LEN   64
typedef struct _ZONETRANS_DB_REC
{
  char         t800[ ZONETRANS_T800_ID_LEN + 1 ];
  short        tpak;
  char         comment[ ZONETRANS_COMMENT_LEN + 1 ];
} ZONETRANS_DB_REC;
#ifdef KEY_SRC
struct keydesc zntrans_key1= {
  ISDUPS,
  1,
  { { 0, 5, CHARTYPE } }
};
#else
extern struct keydesc zntrans_key1;
#endif
#endif
