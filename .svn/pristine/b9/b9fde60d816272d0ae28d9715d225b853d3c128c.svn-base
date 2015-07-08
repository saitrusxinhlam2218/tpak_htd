/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Vehicle_db.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2004/05/12 13:25:43 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Vehicle_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Vehicle_db.h,v $	$Revision: 1.3 $"

#ifndef _VEH_DB_H
#define _VEH_DB_H

#include <isam.h>

#include "Vehicle_public.h"
#include "Vehicle_private.h"

#define VH_FLEET                0
#define VH_NBR                  2  
#define VH_MID                  4  
#define VH_LAST_DRIVER          267
#define VH_SUSP_PASS_CALL       55
#define VH_SUSP_PASS_START   	56
#define VH_SUSP_PASS_END     	60
#define VH_SUSP_DEL_CALL        64
#define VH_SUSP_DEL_START    	465
#define VH_SUSP_DEL_END      	469
#define VH_SUSP_TYPEX_CALL      65
#define VH_SUSP_TYPEX_START  	503
#define VH_SUSP_TYPEX_END    	507

#define VH_KEY1 1
#define VH_KEY2 2
#define VH_KEY3 3
#define VH_KEY4 4
#define VH_KEY5 5
#define VH_KEY6 6
#define VH_KEY7 7
#define VH_KEY8 8
#define VH_KEY9 9

#ifdef KEY_SRC
struct keydesc veh_key1=  {
	 ISNODUPS,
	 1,
         {VH_MID,5,CHARTYPE},
};


/* Composite of (fleet,nbr) */
struct keydesc veh_key2=  {
	 ISNODUPS,
	 2,
	 {     {VH_FLEET,CHARSIZE,CHARTYPE},
				 {VH_NBR,INTSIZE,INTTYPE} }
};

/* Composite of (fleet,last_driver) */
struct keydesc veh_key3=  {
	 ISDUPS,
	 2,
	 {     {VH_FLEET,CHARSIZE,CHARTYPE},
				 {VH_LAST_DRIVER,INTSIZE,INTTYPE} }
};

/* Composite of (susp_pass_call, susp_pass_start) */
 struct keydesc veh_key4=  {
	 ISDUPS,
	 2,
	 {     {VH_SUSP_PASS_CALL,CHARSIZE,CHARTYPE},
				 {VH_SUSP_PASS_START,LONGSIZE,LONGTYPE} }
};

/* Composite of (susp_pass_call, susp_pass_end) */
 struct keydesc veh_key5=  {
	 ISDUPS,
	 2,
	 {     {VH_SUSP_PASS_CALL,CHARSIZE,CHARTYPE},
				 {VH_SUSP_PASS_END,LONGSIZE,LONGTYPE} }
};

/* Composite of (susp_del_call, susp_del_start) */
 struct keydesc veh_key6=  {
	 ISDUPS,
	 2,
	 {     {VH_SUSP_DEL_CALL,CHARSIZE,CHARTYPE},
				 {VH_SUSP_DEL_START,LONGSIZE,LONGTYPE} }
};

/* Composite of (susp_del_call, susp_del_end) */
 struct keydesc veh_key7=  {
	 ISDUPS,
	 2,
	 {     {VH_SUSP_DEL_CALL,CHARSIZE,CHARTYPE},
				 {VH_SUSP_DEL_END,LONGSIZE,LONGTYPE} }
};

/* Composite of (susp_typex_call, susp_typex_start) */
 struct keydesc veh_key8=  {
	 ISDUPS,
	 2,
	 {     {VH_SUSP_TYPEX_CALL,CHARSIZE,CHARTYPE},
				 {VH_SUSP_TYPEX_START,LONGSIZE,LONGTYPE} }
};

/* Composite of (susp_typex_call, susp_typex_end) */
 struct keydesc veh_key9=  {
	 ISDUPS,
	 2,
	 {     {VH_SUSP_TYPEX_CALL,CHARSIZE,CHARTYPE},
				 {VH_SUSP_TYPEX_END,LONGSIZE,LONGTYPE} }
};
#else
extern struct keydesc veh_key1;
extern struct keydesc veh_key2;
extern struct keydesc veh_key3;
extern struct keydesc veh_key4;
extern struct keydesc veh_key5;
extern struct keydesc veh_key6;
extern struct keydesc veh_key7;
extern struct keydesc veh_key8;
extern struct keydesc veh_key9;
#endif

#endif
