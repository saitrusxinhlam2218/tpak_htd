/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Object_defines.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/02/06 17:22:11 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Object_defines.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
/*---------------------------------------------------
 *	Object defines.h
 *
 *		Generic defines for objects
 *---------------------------------------------------
 */
#ifndef _OBJECT_DEFINES
#define GET_STR "get"
#define PUT_STR "set"
#define TAKEN 44               /* for VEH & CALL FALG_REQUEST_STATE */
/* operations on values - statistics */
#define PUT             (0)
#define GET             (1)
#define INCREMENT	(3)
#define DECREMENT       (4)
#ifdef FOO
#define ON		(1)
#endif
#define _ON            ((HNDL) 1)
#define OFF		(0)
#define _OFF		((HNDL)0)
#define ATTR_VALUE_NULL	((ATTR_VALUE)0)
#define STATE_SET    (GET + 5)
#define STATE_RESET  (GET + 6)
/* #define U_IMT_ERR (-1) */
#define STATS_START 700
#define STATS_END   799
#define STATE_BASE 100
#define EXT_STATE_BASE 1100
#define EXT_VALUE_BASE 1000
#define _OBJECT_DEFINES
#endif
