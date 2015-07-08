/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Xfer_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2002/03/21 14:52:16 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Xfer_db.h,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ifndef _XFER_DB_H
#define _XFER_DB_H
typedef struct _XFER_DB_REC
{
  int cl_nbr;
  int due_date_time;
} XFER_DB_REC;
#ifdef KEY_SRC
struct keydesc xfer_key1 = {
  ISNODUPS,
  1,
  { { 0, LONGSIZE, LONGTYPE } }
};
#else
extern struct keydesc xfer_key1;
#endif
#endif
