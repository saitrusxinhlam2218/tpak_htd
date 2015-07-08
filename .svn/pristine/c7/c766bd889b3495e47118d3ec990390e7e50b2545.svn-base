/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Channel_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:42:38 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Channel_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Channel_db.h,v $	$Revision: 1.2 $"

#ifndef _CHANNEL_DB_H
#define _CHANNEL_DB_H
#include <isam.h>

#ifdef KEY_SRC
struct keydesc channel_key1= {
  ISNODUPS,
  1,
  0, 3, CHARTYPE,
};
#else
extern struct keydesc channel_key1;
#endif

typedef struct cisam_channel {
  char     channel_id[3];
  int      channel_nbr;
  char     channel_short_descript[4];
  char     channel_descript[32];
}CHANNEL_REC;

#endif
