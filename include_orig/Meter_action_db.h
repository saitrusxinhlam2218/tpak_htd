/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Meter_action_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:43:57 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Meter_action_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Meter_action_db.h,v $	$Revision: 1.2 $"

#ifndef _M_ACTION_DB_H
#define _M_ACTION_DB_H

#include <isam.h>
typedef struct _M_ACTION_STRUCT
{
  char          trans_cat[ 3 ];
  char          flat_rate;
  char          account_call;
  char          meter_action[ 33 ];
} M_ACTION_STRUCT;
#ifdef KEY_SRC
struct keydesc m_action_key1 =
{
  ISDUPS,
  3,
  {
    { 0, 2, CHARTYPE },
    { 2, 1, CHARTYPE },
    { 3, 1, CHARTYPE },
  }
};
#else
extern struct keydesc m_action_key1;
#endif

#endif
