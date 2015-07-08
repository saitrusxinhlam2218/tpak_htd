/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: List_public.h,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2004/05/12 12:19:05 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/List_public.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: List_public.h,v $	$Revision: 1.5 $"

#include "LL.h"

/*-------------------------------------------------------
 *	List public.h
 *
 *		data types, function prototypes and definitions
 *		for timer object
 *-------------------------------------------------------
 */
#ifndef LIST_PUBLIC

/*------------------------------------------------------
 *	data types
 *------------------------------------------------------
 */

typedef HNDL			LIST_HANDLE;

/*------------------------------------------------------
 *	attributes
 *------------------------------------------------------
 */

/*------------------------------------------------------
 *	services
 *------------------------------------------------------
 */

RET_STATUS 	List_services_start ARGS((void));
RET_STATUS 	List_services_end ARGS((void));
LIST_HANDLE List_create ARGS((int (*compare_func )()));
RET_STATUS 	List_destroy ARGS((LIST_HANDLE list_handle));
RET_STATUS 	List_clear ARGS((LIST_HANDLE list_handle));
RET_STATUS 	List_add ARGS((LIST_HANDLE list, HNDL data));
RET_STATUS	List_remove ARGS((LIST_HANDLE list, HNDL data));
HNDL 		List_get ARGS((LIST_HANDLE list, int attribute));
HNDL 		List_find ARGS((LIST_HANDLE list, HNDL search_value,
						int (*search_func )()));

/*------------------------------------------------------
 *	defines
 *------------------------------------------------------
 */

#define LIST_FIRST				-100
#define LIST_NEXT				-101
#define LIST_PREV                               -102

#define LIST_LT					-1
#define LIST_EQ					0
#define LIST_GT					1


#define LIST_PUBLIC	
#endif
