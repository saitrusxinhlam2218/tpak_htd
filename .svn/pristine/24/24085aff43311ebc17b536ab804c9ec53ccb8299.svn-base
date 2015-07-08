/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Shift_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/03/13 18:14:15 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Shift_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _SHIFT_DB_H
#define _SHIFT_DB_H
#include <isam.h>
#define ADD_ATTR_OFFSET   112
#define SHIFT_NBR                                      0
#define SHIFT_FLEET             SHIFT_NBR               + 10
#define SHIFT_PCKUP_ZONE        SHIFT_FLEET             + 171
#define SHIFT_CHARGE_NBR        SHIFT_PCKUP_ZONE        + 140 + ADD_ATTR_OFFSET
#define SHIFT_DUE_DATE_TIME     SHIFT_CHARGE_NBR        + 124
#define SHIFT_VEH_NBR           SHIFT_DUE_DATE_TIME     + 110
#define SHIFT_DISPATCHED_BY     SHIFT_VEH_NBR           + 6
#ifdef KEY_SRC
struct keydesc sh_key1=  {
	 ISNODUPS,
	 1,
         {SHIFT_NBR,LONGSIZE,LONGTYPE},
};
/* Composite of (fleet, due_date_time) */
struct keydesc sh_key2=  {
	 ISDUPS,
	 2,
	 {     {SHIFT_FLEET,CHARSIZE,CHARTYPE},
				 {SHIFT_DUE_DATE_TIME,LONGSIZE,LONGTYPE} }
};
/* Composite of (fleet, pckup_zone, due_date_time) */
struct keydesc sh_key3=  {
	 ISDUPS,
	 3,
	 {     {SHIFT_FLEET,CHARSIZE,CHARTYPE},
         {SHIFT_PCKUP_ZONE,INTSIZE,INTTYPE},
				 {SHIFT_DUE_DATE_TIME,LONGSIZE,LONGTYPE} }
};
/* Composite of (fleet, veh_nbr, due_date_time) */
struct keydesc sh_key4=  {
	 ISDUPS,
	 3,
	 {     {SHIFT_FLEET,CHARSIZE,CHARTYPE},
         {SHIFT_VEH_NBR,INTSIZE,INTTYPE},
				 {SHIFT_DUE_DATE_TIME,LONGSIZE,LONGTYPE} }
};
/* Composite of (fleet, dispatched_by, due_date_time) */
struct keydesc sh_key5=  {
	 ISDUPS,
	 3,
	 {     {SHIFT_FLEET,CHARSIZE,CHARTYPE},
         {SHIFT_DISPATCHED_BY,INTSIZE,INTTYPE},
				 {SHIFT_DUE_DATE_TIME,LONGSIZE,LONGTYPE} }
};
/* Composite of (fleet, charge_nbr, due_date_time) */
struct keydesc sh_key6=  {
	 ISDUPS,
	 3,
	 {     {SHIFT_FLEET,CHARSIZE,CHARTYPE},
         {SHIFT_CHARGE_NBR,18,CHARTYPE},
				 {SHIFT_DUE_DATE_TIME,LONGSIZE,LONGTYPE} }
};
#endif
#endif
