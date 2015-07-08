/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Vehicle_timers_public.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:45:07 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Vehicle_timers_public.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _VEH_TIMERS_PUBLIC_H_
#define _VEH_TIMERS_PUBLIC_H_
/*-------------------------------------------------------
 *	Vehicle timers public.h
 *-------------------------------------------------------
 */
#include "Object_functions.h"
#define VEH_ALL_TIMERS		-99
RET_STATUS Veh_add_timer ARGS((VEH_HNDL veh, int type, int expiration, int (*notify_func )()));
RET_STATUS Veh_delete_timer ARGS((VEH_HNDL veh, int type));
RET_STATUS dump_veh_timer_list ARGS((VEH_HNDL veh));
RET_STATUS Veh_wait_epost_notify_func ARGS((VEH_HNDL veh ));
#endif
