/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Veh_canmsg_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1999/02/01 12:46:45 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Veh_canmsg_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _VEH_CANMSG_DB_H
#define _VEH_CANMSG_DB_H
#include <isam.h>
typedef struct cisam_veh_cm
{
  int          nbr;
  char         text[65];
} VEH_CANMSG_REC;
#ifdef KEY_SRC
struct keydesc veh_cm_key1= {
         ISNODUPS,
         1,
				 { {0,LONGSIZE,LONGTYPE} }
};
#else
extern struct keydesc veh_cm_key1;
#endif
#endif
