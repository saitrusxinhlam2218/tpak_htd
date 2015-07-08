/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Driver_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:42:54 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Driver_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

#ifndef _DRIVER_DB_H
#define _DRIVER_DB_H

#include <isam.h>
#include "Driver_private.h"


#define DR_FLEET                                    0
#define DR_ID                2
#define DR_NAME              6
#define DR_LAST_VEH_NBR      441
#define DR_SUSP_PASS_CALL    208
#define DR_SUSP_PASS_START   200
#define DR_SUSP_PASS_END     204
#define DR_SUSP_DEL_CALL     209
#define DR_SUSP_DEL_START    529
#define DR_SUSP_DEL_END      533
#define DR_SUSP_TYPEX_CALL   210
#define DR_SUSP_TYPEX_START  567
#define DR_SUSP_TYPEX_END    571

#define DR_KEY1 1
#define DR_KEY2 2
#define DR_KEY3 3
#define DR_KEY4 4
#define DR_KEY5 5
#define DR_KEY6 6
#define DR_KEY7 7
#define DR_KEY8 8
#define DR_KEY9 9

#ifdef KEY_SRC
struct keydesc dr_key1=  {
				 ISNODUPS,
				 2,
				 { {DR_FLEET,CHARSIZE,CHARTYPE},
					 {DR_ID,LONGSIZE,LONGTYPE} }
};

/* Composite of (fleet,last_veh_nbr) */
struct keydesc dr_key2 = {
	 ISDUPS,
	 2,
	 {     {DR_FLEET,CHARSIZE,CHARTYPE},
				 {DR_LAST_VEH_NBR,INTSIZE,INTTYPE} }
};

/* Composite of (fleet,name) */
 struct keydesc dr_key3=  {
	 ISDUPS,
	 2,
	 {     {DR_FLEET,CHARSIZE,CHARTYPE},
				 {DR_NAME,25,CHARTYPE} }
};

/* Composite of (susp_pass_call, susp_pass_start) */
 struct keydesc dr_key4=  {
	 ISDUPS,
	 2,
	 {     {DR_SUSP_PASS_CALL,CHARSIZE,CHARTYPE},
				 {DR_SUSP_PASS_START,LONGSIZE,LONGTYPE} }
};

/* Composite of (susp_pass_call, susp_pass_end) */
 struct keydesc dr_key5=  {
	 ISDUPS,
	 2,
	 {     {DR_SUSP_PASS_CALL,CHARSIZE,CHARTYPE},
				 {DR_SUSP_PASS_END,LONGSIZE,LONGTYPE} }
};

/* Composite of (susp_del_call, susp_del_start) */
 struct keydesc dr_key6=  {
	 ISDUPS,
	 2,
	 {     {DR_SUSP_DEL_CALL,CHARSIZE,CHARTYPE},
				 {DR_SUSP_DEL_START,LONGSIZE,LONGTYPE} }
};

/* Composite of (susp_del_call, susp_del_end) */
 struct keydesc dr_key7=  {
	 ISDUPS,
	 2,
	 {     {DR_SUSP_DEL_CALL,CHARSIZE,CHARTYPE},
				 {DR_SUSP_DEL_END,LONGSIZE,LONGTYPE} }
};

/* Composite of (susp_typex_call, susp_typex_start) */
 struct keydesc dr_key8=  {
	 ISDUPS,
	 2,
	 {     {DR_SUSP_TYPEX_CALL,CHARSIZE,CHARTYPE},
				 {DR_SUSP_TYPEX_START,LONGSIZE,LONGTYPE} }
};

/* Composite of (susp_typex_call, susp_typex_end) */
 struct keydesc dr_key9=  {
	 ISDUPS,
	 2,
	 {     {DR_SUSP_TYPEX_CALL,CHARSIZE,CHARTYPE},
				 {DR_SUSP_TYPEX_END,LONGSIZE,LONGTYPE} }
};
#else
extern struct keydesc dr_key1;
extern struct keydesc dr_key2;
extern struct keydesc dr_key3;
extern struct keydesc dr_key4;
extern struct keydesc dr_key5;
extern struct keydesc dr_key6;
extern struct keydesc dr_key7;
extern struct keydesc dr_key8;
extern struct keydesc dr_key9;
#endif



#endif
