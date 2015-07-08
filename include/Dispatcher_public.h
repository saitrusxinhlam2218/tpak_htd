/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Dispatcher_public.h,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2000/03/02 19:43:59 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Dispatcher_public.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_
/* Dispatcher_public.h */
/* FUNCTION PROTOTYPES */
RET_STATUS Dispatch_call_offer(VEH_HNDL, CALL_HNDL, int, int);
RET_STATUS Dispatcher_call_match(CALL_HNDL);
VEH_HNDL   Dispatcher_veh_select(CALL_HNDL, ZONE_HNDL, short);
RET_STATUS Dispatcher_veh_match(VEH_HNDL, int);
/*
 *	new for G7
 */
RET_STATUS Dispatcher_Nordic_veh_select(CALL_HNDL, ZONE_HNDL, VEH_HNDL*, short);
BOOLEAN Dispatcher_night_shift_compatible(CALL_HNDL, VEH_HNDL);
BOOLEAN Dispatcher_call_class_compatible(CALL_HNDL, VEH_HNDL);
BOOLEAN Dispatcher_vehicle_call_compatible(CALL_HNDL, VEH_HNDL, ZONE_HNDL);
BOOLEAN Dispatcher_fifo_by_zone_queue_order( CALL_HNDL, ZONE_HNDL, VEH_HNDL *);
BOOLEAN Dispatcher_temporal_by_zone_level( CALL_HNDL, ZONE_HNDL, VEH_HNDL *);
RET_STATUS Dispatcher_Nordic_vehicle_to_call(VEH_HNDL, short);
RET_STATUS Dispatcher_Nordic_find_public_call(VEH_HNDL, ZONE_HNDL);
RET_STATUS Dispatcher_Nordic_look_in_zone(ZONE_HNDL, VEH_HNDL, char, CALL_HNDL*);
RET_STATUS Dispatcher_Nordic_look_in_zone_for_TimeCalls(ZONE_HNDL, VEH_HNDL, CALL_HNDL *);
RET_STATUS Dispatcher_Nordic_look_in_backup_levels(VEH_HNDL, ZONE_HNDL, short, char);
RET_STATUS Dispatcher_get_zone_backup_max(ZONE_HNDL, int*); 
#endif
