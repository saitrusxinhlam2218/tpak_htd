/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Interlink_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2002/03/21 14:52:15 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Interlink_db.h,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ifndef _ILINK_DB_H
#define _ILINK_DB_H
#define ILINK_T800_NBR_LEN   16
#define ILINK_STATUS_LEN     20
#define ILINK_ORIGIN_LEN      5
typedef struct _ILINK_DB_REC
{
  int          tpak_nbr;
  char         t800_nbr[ ILINK_T800_NBR_LEN + 1 ];
  char         status[ ILINK_STATUS_LEN + 1 ];
  char         origin[ ILINK_ORIGIN_LEN + 1 ];
} ILINK_DB_REC;
#ifdef KEY_SRC
struct keydesc ilink_key1= {
  ISNODUPS,
  1,
  { { 0, LONGSIZE, LONGTYPE } }
};
#else
extern struct keydesc ilink_key1;
#endif
#endif
