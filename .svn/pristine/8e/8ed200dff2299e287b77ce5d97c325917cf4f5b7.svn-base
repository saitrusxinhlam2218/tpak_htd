/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Timer_public.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/01/25 20:41:14 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Timer_public.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Timer_public.h,v $	$Revision: 1.2 $"

/*-------------------------------------------------------
 *	Timer public.h
 *
 *		data types, function prototypes and definitions
 *		for timer object
 *-------------------------------------------------------
 */

#ifndef _TIMER_PUBLIC

/*------------------------------------------------------
 *	data types
 *------------------------------------------------------
 */

typedef GENERIC			TIMER_HANDLE;

/*------------------------------------------------------
 *	attributes
 *------------------------------------------------------
 */

enum TIMER_ATTRIBUTE
  {
	TIMER_TYPE			 	= -101,
	TIMER_EXPIRE_TIME		= -102,
	TIMER_REPEAT_INTERVAL	= -103,
	TIMER_NOTIFY_FUNC		= -104,
	TIMER_DATA				= -105
  };	


/*------------------------------------------------------
 *	services
 *------------------------------------------------------

RET_STATUS Timer_services_start ARGS((long *application_time));
RET_STATUS Timer_services_end ARGS((void));
TIMER_HANDLE Timer_create ARGS((int va_alist));
RET_STATUS Timer_destroy ARGS((TIMER_HANDLE timer));
int Timer_service ARGS((int number_of_timers));
ATTR_VALUE Timer_get_value ARGS((TIMER_HANDLE timer, ATTRIBUTE attribute));
RET_STATUS Timer_set_value ARGS((int va_alist));
int timer_compare ARGS((TIMER *timer1, TIMER *timer2));

/*------------------------------------------------------
 *	defines
 *------------------------------------------------------
 */

#define	TIMER_ALL_TIMERS		-200

#define _TIMER_PUBLIC
#endif
