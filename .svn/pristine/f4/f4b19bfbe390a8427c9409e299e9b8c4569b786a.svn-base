/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:42:25 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Call_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _CALL_DB_H
#define _CALL_DB_H
#include <isam.h>
#include "Call_public.h"
#include "Call_private.h"
#define CL_ADD_ATTR_OFFSET   112
#define CL_NBR                                      0
#define CL_GROUPED_WITH      CL_NBR               + 4 /** 4 **/
#define CL_FLEET             CL_GROUPED_WITH      + 6 /** 10 **/
#define CL_PCKUP_STR_KEY     CL_FLEET             + 127 /** 137 **/
#define CL_PCKUP_ZONE        CL_PCKUP_STR_KEY     + 44 /** 181 **/
#define CL_PHONE_NBR         CL_PCKUP_ZONE        + 25 /** 206 **/
#define CL_PRI_STATUS        CL_PHONE_NBR         + 108 + CL_ADD_ATTR_OFFSET /** 426 **/
#define CL_CHARGE_NBR        CL_PRI_STATUS        + 7 /** 433 **/
#define CL_DUE_DATE_TIME     CL_CHARGE_NBR        + 124 /** 557 **/
#define CL_VEH_NBR           CL_DUE_DATE_TIME     + 110 /** 667 **/
#define CL_DRV_ID            CL_VEH_NBR           + 2   /** 669 **/
#define CL_PASS_NAME_KEY     CL_DRV_ID            + 151 /** 820 **/
#define CL_EXT_NBR           CL_PASS_NAME_KEY     + 27  /** 847 **/
#define CL_SHIFT_NBR                                      0
#define CL_SHIFT_FLEET             CL_SHIFT_NBR               + 10
#define CL_SHIFT_PCKUP_ZONE        CL_SHIFT_FLEET             + 171
#define CL_SHIFT_CHARGE_NBR        CL_SHIFT_PCKUP_ZONE        + 140 + CL_ADD_ATTR_OFFSET
#define CL_SHIFT_DUE_DATE_TIME     CL_SHIFT_CHARGE_NBR        + 124
#define CL_SHIFT_VEH_NBR           CL_SHIFT_DUE_DATE_TIME     + 110
#define CL_SHIFT_DISPATCHED_BY     CL_SHIFT_VEH_NBR           + 6
#ifdef KEY_SRC
struct keydesc cl_key1=  {
	 ISNODUPS,
	 1,
			 { {CL_NBR,LONGSIZE,LONGTYPE} }
};
/* group_with */
struct keydesc cl_key2=  {
	 ISDUPS,
	 1,
	 { {CL_GROUPED_WITH,LONGSIZE,LONGTYPE} }
};
/* Composite of (fleet,due_date_time) */
struct keydesc cl_key3=  {
	 ISDUPS,
	 2,
	 { {CL_FLEET,CHARSIZE,CHARTYPE},
		 {CL_DUE_DATE_TIME,LONGSIZE,LONGTYPE} }
};
/* Composite of (fleet,pri_status,veh_nbr,due_date_time) */
struct keydesc cl_key4=  {
	 ISDUPS,
	 4,
	 {     {CL_FLEET,CHARSIZE,CHARTYPE},
         {CL_PRI_STATUS,INTSIZE,INTTYPE},
         {CL_VEH_NBR,INTSIZE,INTTYPE},
				 {CL_DUE_DATE_TIME,LONGSIZE,LONGTYPE}  }
};
/* Composite of (fleet,pckup_str_key,due_date_time) */
struct keydesc cl_key5=  {
	 ISDUPS,
	 3,
	 {     {CL_FLEET,CHARSIZE,CHARTYPE},
		 {CL_PCKUP_STR_KEY,4,CHARTYPE},
	 {CL_DUE_DATE_TIME,LONGSIZE,LONGTYPE}
 }
};
/* Composite of (fleet,pass_name_key,due_date_time) */
struct keydesc cl_key6=  {
	 ISDUPS,
	 3,
	 {     {CL_FLEET,CHARSIZE,CHARTYPE},
         {CL_PASS_NAME_KEY,4,CHARTYPE},
				 {CL_DUE_DATE_TIME,LONGSIZE,LONGTYPE} }
};
/* Composite of (fleet,phone_nbr,due_date_time) */
struct keydesc cl_key7=  {
	 ISDUPS,
	 3,
	 {     {CL_FLEET,CHARSIZE,CHARTYPE},
         {CL_PHONE_NBR,7,CHARTYPE},
				 {CL_DUE_DATE_TIME,LONGSIZE,LONGTYPE} }
};
/* Composite of (fleet,charge_nbr,due_date_time) */
struct keydesc cl_key8=  {
	 ISDUPS,
	 3,
	 {     {CL_FLEET,CHARSIZE,CHARTYPE},
         {CL_CHARGE_NBR,18,CHARTYPE},
				 {CL_DUE_DATE_TIME,LONGSIZE,LONGTYPE} }
};
/* Composite of (fleet,pri_status,drv_id,due_date_time) */
struct keydesc cl_key9=  {
	 ISDUPS,
	 4,
	 {     {CL_FLEET,CHARSIZE,CHARTYPE},
         {CL_PRI_STATUS,INTSIZE,INTTYPE},
         {CL_DRV_ID,LONGSIZE,LONGTYPE},
				 {CL_DUE_DATE_TIME,LONGSIZE,LONGTYPE} }
};
/* nbr */
struct keydesc cl_key10=  {
	 ISDUPS,
	 1,
				 {CL_EXT_NBR,LONGSIZE,LONGTYPE},
};
/** fleet,due_date_time,pckup_zone **/
struct keydesc cl_key11= {
  ISDUPS,
  3,
  {
    {CL_FLEET, CHARSIZE, CHARTYPE},
    {CL_DUE_DATE_TIME,LONGSIZE,LONGTYPE},
    {CL_PCKUP_ZONE,INTSIZE,INTTYPE}
  }
};
#else
extern struct keydesc cl_key1;
extern struct keydesc cl_key2;
extern struct keydesc cl_key3;
extern struct keydesc cl_key4;
extern struct keydesc cl_key5;
extern struct keydesc cl_key6;
extern struct keydesc cl_key7;
extern struct keydesc cl_key8;
extern struct keydesc cl_key9;
extern struct keydesc cl_key10;
extern struct keydesc cl_key11;
#endif
#endif
