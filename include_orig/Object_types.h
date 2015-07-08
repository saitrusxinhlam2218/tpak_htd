/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Object_types.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/02/23 21:44:00 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Object_types.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

/*---------------------------------------------------
 *	Object types.h
 *
 *		Generic types for objects
 *---------------------------------------------------
 */

#ifndef _OBJECT_TYPES

typedef void *		HNDL;

#define	HNDL_ERR	((void *)0L)
#define	HNDL_NULL	((void *)0L)
#define HANDLE_NULL     ((HNDL)0L)
 
typedef HNDL			GENERIC;
typedef int			ATTRIBUTE;
typedef GENERIC			ATTR_VALUE;

#define _OBJECT_TYPES
#endif

