/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Cond_type_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:42:40 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Cond_type_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Cond_type_db.h,v $	$Revision: 1.2 $"

#ifndef _COND_TYPE_DB_H
#define _COND_TYPE_DB_H

#include <isam.h>
typedef struct _COND_TYPE_STRUCT
{
  long          id;
  short         nbr;
  char          descript[ 33 ];
} COND_TYPE_STRUCT;
#ifdef KEY_SRC
struct keydesc cond_type_key1 =
{
  ISNODUPS,
  1,
  {
    { 4, INTSIZE,  INTTYPE },
  }
};
#else
extern struct keydesc cond_type_key1;
#endif

#endif
