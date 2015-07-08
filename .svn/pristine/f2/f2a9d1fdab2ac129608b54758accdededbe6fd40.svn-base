/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Zone_lists_public.h,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2004/06/22 17:12:38 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Zone_lists_public.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Zone_lists_public.h,v $	$Revision: 1.5 $"

#include "LL.h"
/* Zone_lists_public.h */

#ifndef _ZONE_LISTS_PUBLIC_H_
#define _ZONE_LISTS_PUBLIC_H_

#define ZONE_LIST_BIDDER_MAX 32
#define ZONE_CALLS_LIST_FIRST LIST_FIRST
#define ZONE_CALLS_LIST_NEXT  LIST_NEXT
#define ZONE_TC_LIST_FIRST LIST_FIRST
#define ZONE_TC_LIST_NEXT  LIST_NEXT
#define ZONE_VEH_LIST_FIRST LIST_FIRST
#define ZONE_VEH_LIST_NEXT  LIST_NEXT

/* FUNCTION PROTOTYPES */

RET_STATUS           Zone_calls_list_add (LinkedList *, CALL_HNDL);
RET_STATUS           Zone_calls_list_clear (LinkedList *);
LinkedList *         Zone_calls_list_create ();
CALL_HNDL            Zone_calls_list_find (LinkedList *, CALL_HNDL);
CALL_HNDL            Zone_calls_list_get (LinkedList *, short);
RET_STATUS           Zone_calls_list_remove (LinkedList *, CALL_HNDL);

RET_STATUS           Zone_tc_list_add (LinkedList *, TC_HNDL);
RET_STATUS           Zone_tc_list_clear (LinkedList *);
LinkedList *         Zone_tc_list_create ();
TC_HNDL              Zone_tc_list_find (LinkedList *, TC_HNDL);
TC_HNDL              Zone_tc_list_get (LinkedList *, short);
RET_STATUS           Zone_tc_list_remove (LinkedList *, TC_HNDL);

#define	Zone_calls_list_each_get(calls_list, call_hndl)	\
   { CALL_HNDL _next; LL_Rewind(calls_list); \
   for ((call_hndl) = Zone_calls_list_get((calls_list), ZONE_CALLS_LIST_FIRST),\
	_next = Zone_calls_list_get((calls_list), ZONE_CALLS_LIST_NEXT);\
	(call_hndl); \
	(call_hndl) = _next, _next = Zone_calls_list_get((calls_list), ZONE_CALLS_LIST_NEXT)) {

#define	Zone_calls_list_each_end(calls_list) } LL_Rewind(calls_list); }

#define	Zone_tc_list_each_get(tc_list, tc_hndl)	\
   { TC_HNDL _next; LL_Rewind(tc_list); \
   for ((tc_hndl) = Zone_tc_list_get((tc_list), ZONE_TC_LIST_FIRST),\
	_next = Zone_tc_list_get((tc_list), ZONE_TC_LIST_NEXT);\
	(tc_hndl); \
	(tc_hndl) = _next, _next = Zone_tc_list_get((tc_list), ZONE_TC_LIST_NEXT)) {

#define	Zone_tc_list_each_end(tc_list) } LL_Rewind(tc_list); }

RET_STATUS           Zone_veh_list_add (LinkedList *, VEH_HNDL);
RET_STATUS           Zone_veh_list_clear (LinkedList *);
LinkedList *         Zone_veh_list_create ();
VEH_HNDL             Zone_veh_list_find (LinkedList *, VEH_HNDL);
VEH_HNDL             Zone_veh_list_get (LinkedList *, short);
RET_STATUS           Zone_veh_list_remove (LinkedList *, VEH_HNDL);

#define	Zone_veh_list_each_get(veh_list, veh_hndl)	\
   { VEH_HNDL _next; LL_Rewind(veh_list); \
   for ((veh_hndl) = Zone_veh_list_get((veh_list), ZONE_VEH_LIST_FIRST),\
	_next = Zone_veh_list_get((veh_list), ZONE_VEH_LIST_NEXT);\
	(veh_hndl); \
	(veh_hndl) = _next, _next = Zone_veh_list_get((veh_list), ZONE_VEH_LIST_NEXT)) {

#define	Zone_veh_list_each_end(veh_list) } LL_Rewind(veh_list); }

#endif

