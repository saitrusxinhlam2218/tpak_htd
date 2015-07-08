/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Seasons_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:18 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Seasons_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Seasons_db.h,v $	$Revision: 1.2 $"

#ifndef _SEASONS_DB_H
#define _SEASONS_DB_H

#include <isam.h>

typedef struct _SEASONS_STRUCT
{
  int          season_id;
  char         season_start[ 9 ];
  char         season_end[ 9 ];
  char         season_descript[ 17 ];
  float        season_FV;
  float        season_EV;
} SEASONS_STRUCT;

#ifdef KEY_SRC
struct keydesc seasons_key1 =
{
  ISNODUPS,
  1,
  {
    { 4, 8, CHARTYPE },
  }
};

#else
extern struct keydesc seasons_key1;
#endif

#endif
