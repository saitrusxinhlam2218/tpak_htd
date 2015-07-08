/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Excpt_group_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:43:08 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Excpt_group_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Excpt_group_db.h,v $	$Revision: 1.2 $"

#ifndef _EXCPT_GROUP_DB_H
#define _EXCPT_GROUP_DB_H

#include <isam.h>

typedef struct    cisam_eg
{
  short    nbr;  
  char     name[8];
}EXCEPT_GROUP_REC;

#ifdef KEY_SRC
struct keydesc eg_key1= {
         ISNODUPS,
         1,
         {0,INTSIZE,INTTYPE},
};

/* name */
struct keydesc eg_key2= {
         ISDUPS,
         1,
         {2,7,CHARTYPE},
};
#else
extern struct keydesc eg_key1;
extern struct keydesc eg_key2;
#endif

#endif
