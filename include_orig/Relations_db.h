/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Relations_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:12 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Relations_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Relations_db.h,v $	$Revision: 1.2 $"

#ifndef _RELATIONS_DB_H
#define _RELATION_DB_H

#include <isam.h>

typedef struct _RELATIONS_STRUCT
{
  int          id;
  int          nbr;
  char         transcat[ 3 ];
  char         pckup_zone[ 4 ];
  char         pckup_sname[ 21 ];
  char         pckup_nbr[ 4 ];
  char         dst_zone[ 4 ];
  char         dst_sname[ 21 ];
  char         dst_nbr[ 4 ];
} RELATIONS_STRUCT;

#ifdef KEY_SRC
struct keydesc relations_key1 =
{
  ISDUPS,
  1,
  {
    { 8, 2, CHARTYPE },
  }
};

#else
extern struct keydesc relations_key1;
#endif


#endif
