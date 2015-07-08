/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: AttrTrans_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2002/03/21 14:52:14 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/AttrTrans_db.h,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: AttrTrans_db.h,v $	$Revision: 1.2 $"

#ifndef _ATTRANS_DB_H
#define _ATTRANS_DB_H

#define T800_ATTR_LEN 32
#define TPAK_ATTR_LEN 2

typedef struct _ATTRANS_DB_REC
{
  char         attr_t800[ T800_ATTR_LEN + 1 ];
  char         attr_tpak[ TPAK_ATTR_LEN + 1 ];
} ATTRANS_DB_REC;

#ifdef KEY_SRC
struct keydesc attrans_key1= {
  ISNODUPS,
  1,
  { { 0, 32, CHARTYPE } }
};
#else
extern struct keydesc attrans_key1;
#endif

#endif
