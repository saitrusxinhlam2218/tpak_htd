/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Vehicle_lists_public.h,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2004/06/22 17:10:04 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Vehicle_lists_public.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _VEHICLE_LISTS_PUBLIC_H_
#define _VEHICLE_LISTS_PUBLIC_H_
#include "LL.h"
#define VEH_CALLS_LIST_FIRST    LIST_FIRST
#define VEH_CALLS_LIST_NEXT     LIST_NEXT
LinkedList *        Veh_calls_list_create();
RET_STATUS          Veh_calls_list_destroy( LinkedList *);
int                 Veh_calls_list_add( LinkedList *, CALL_HNDL);
int                 Veh_calls_list_remove(LinkedList *, CALL_HNDL);
CALL_HNDL           Veh_calls_list_find(LinkedList *, CALL_HNDL);
CALL_HNDL           Veh_calls_list_get(LinkedList *, int);
RET_STATUS          Veh_calls_list_clean(LinkedList *);
#define	Veh_calls_list_each_get(veh_calls_list, call_hndl)	\
   { CALL_HNDL _next; LL_Rewind(veh_calls_list); \
   for ((call_hndl) = Veh_calls_list_get((veh_calls_list), VEH_CALLS_LIST_FIRST),\
	_next = Veh_calls_list_get((veh_calls_list), VEH_CALLS_LIST_NEXT);\
	(call_hndl != HNDL_ERR); \
	(call_hndl) = _next, _next = Veh_calls_list_get((veh_calls_list), VEH_CALLS_LIST_NEXT)) {
#define	Veh_calls_list_each_end(veh_calls_list) } LL_Rewind(veh_calls_list); }
#define VEH_EXCPT_LIST_FIRST    LIST_FIRST
#define VEH_EXCPT_LIST_NEXT     LIST_NEXT
LinkedList *        Veh_excpt_list_create();
RET_STATUS          Veh_excpt_list_destroy( LinkedList *);
int                 Veh_excpt_list_add( LinkedList *, EXCPT_HNDL);
int                 Veh_excpt_list_remove(LinkedList *, EXCPT_HNDL);
EXCPT_HNDL           Veh_excpt_list_find(LinkedList *, EXCPT_HNDL);
EXCPT_HNDL           Veh_excpt_list_get(LinkedList *, int);
RET_STATUS          Veh_excpt_list_clean(LinkedList *);
#define	Veh_excpt_list_each_get(veh_excpt_list, excpt_hndl)	\
   { EXCPT_HNDL _next; LL_Rewind(veh_excpt_list); \
   for ((excpt_hndl) = Veh_excpt_list_get((veh_excpt_list), VEH_EXCPT_LIST_FIRST),\
	_next = Veh_excpt_list_get((veh_excpt_list), VEH_EXCPT_LIST_NEXT);\
	(excpt_hndl != HNDL_ERR); \
	(excpt_hndl) = _next, _next = Veh_excpt_list_get((veh_excpt_list), VEH_EXCPT_LIST_NEXT)) {
#define	Veh_excpt_list_each_end } LL_Rewind(veh_excpt_list); }
#define VEH_HTTP_LIST_FIRST    LIST_FIRST
#define VEH_HTTP_LIST_NEXT     LIST_NEXT
LinkedList *        Veh_http_list_create();
RET_STATUS          Veh_http_list_destroy( LinkedList *);
int                 Veh_http_list_add( LinkedList *, HTTP_HNDL);
int                 Veh_http_list_remove(LinkedList *, HTTP_HNDL);
HTTP_HNDL           Veh_http_list_find(LinkedList *, HTTP_HNDL);
HTTP_HNDL           Veh_http_list_get(LinkedList *, int);
RET_STATUS          Veh_http_list_clean(LinkedList *);
#define	Veh_http_list_each_get(veh_http_list, http_hndl)	\
   { HTTP_HNDL _next; LL_Rewind(veh_http_list); \
   for ((http_hndl) = Veh_http_list_get((veh_http_list), VEH_HTTP_LIST_FIRST),\
	_next = Veh_http_list_get((veh_http_list), VEH_HTTP_LIST_NEXT);\
	(http_hndl != HNDL_ERR); \
	(http_hndl) = _next, _next = Veh_http_list_get((veh_http_list), VEH_HTTP_LIST_NEXT)) {
#define	Veh_http_list_each_end(veh_http_list) } LL_Rewind(veh_http_list); }
#endif
