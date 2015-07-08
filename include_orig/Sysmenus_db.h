/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Sysmenus_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/09/02 17:38:52 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Sysmenus_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Sysmenus_db.h,v $	$Revision: 1.2 $"

#ifndef _SYSMENUS_DB_H
#define _SYSMENUS_DB_H

#include <isam.h>

typedef struct  cisam_sysmenus
{
  char menuname[19];
  char title[61];
} SYSMENUS_REC;

typedef struct cisam_sysitems
{
  char          menuname[19];
  int           itemnum;
  char          mtext[61];
  char          mtype;
  char          progname[61];
} SYSITEMS_REC;

#ifdef KEY_SRC

struct keydesc sysmenu_key1 = {
  ISNODUPS,
  1,
  {{0, 18, CHARTYPE}}
};

struct keydesc sysitems_key1 = {
  ISNODUPS,
  2,
  {{0, 18, CHARTYPE},
     {18, 4, LONGTYPE}}
};

#else
extern struct keydesc sysmenus_key1;
extern struct keydesc sysitems_key1;
#endif
  
  

#endif
