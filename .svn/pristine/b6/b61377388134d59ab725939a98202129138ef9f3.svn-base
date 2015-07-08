/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call_timers_public.h,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 2003/01/28 18:16:15 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Call_timers_public.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Call_timers_public.h,v $	$Revision: 1.4 $"

#ifndef _CALL_TIMERS_PUBLIC_H_
#define _CALL_TIMERS_PUBLIC_H_

/*-------------------------------------------------------
 *	Call timers public.h
 *-------------------------------------------------------
 */

#include "Object_functions.h"

#define CALL_ALL_TIMERS		-99

RET_STATUS Call_add_timer ARGS((CALL_HNDL call, int type, int expiration, int (*notify_func )()));
RET_STATUS Call_delete_timer ARGS((CALL_HNDL call, int type));
RET_STATUS dump_call_timer_list ARGS((CALL_HNDL call));
RET_STATUS call_accept_notify_func ARGS((CALL_HNDL call));
RET_STATUS call_zone_timeout_notify_func ARGS((CALL_HNDL call));
RET_STATUS call_timeout_notify_func ARGS((CALL_HNDL call));
RET_STATUS call_latemeter_notify_func ARGS((CALL_HNDL call));
RET_STATUS call_m_action_notify_func ARGS((CALL_HNDL call));
RET_STATUS call_ilink_timeout_notify_func ARGS((CALL_HNDL call));

#endif
