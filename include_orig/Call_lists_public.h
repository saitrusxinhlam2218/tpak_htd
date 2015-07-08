/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call_lists_public.h,v $
*  @(#)  $Revision: 1.8 $
*  @(#)  $Date: 2004/06/22 17:10:04 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Call_lists_public.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Call_lists_public.h,v $	$Revision: 1.8 $"

#include "LL.h"

#define CALL_REJECT_VEH_LIST_FIRST    LIST_FIRST
#define CALL_REJECT_VEH_LIST_NEXT     LIST_NEXT
#define CALLS_LIST_FIRST              LIST_FIRST
#define CALLS_LIST_NEXT               LIST_NEXT
#define CALLS_EXCPT_LIST_FIRST              LIST_FIRST
#define CALLS_EXCPT_LIST_NEXT               LIST_NEXT
#define CALL_MSG_LIST_FIRST           LIST_FIRST
#define CALL_MSG_LIST_NEXT            LIST_NEXT

LinkedList *	                Call_reject_veh_list_create();
RET_STATUS 			Call_reject_veh_list_destroy(LinkedList *);
RET_STATUS 			Call_reject_veh_list_add(LinkedList *, VEH_HNDL);
RET_STATUS 			Call_reject_veh_list_remove(LinkedList *, VEH_HNDL);
VEH_HNDL			Call_reject_veh_list_find(LinkedList *, short);
VEH_HNDL			Call_reject_veh_list_get(LinkedList *, short);

#define	Call_reject_veh_list_each_get(call_reject_veh_list, veh_hndl)	\
   { VEH_HNDL  _next; LL_Rewind(call_reject_veh_list); \
   for ((veh_hndl) = Call_reject_veh_list_get((call_reject_veh_list), CALL_REJECT_VEH_LIST_FIRST),\
	_next = Call_reject_veh_list_get((call_reject_veh_list), CALL_REJECT_VEH_LIST_NEXT);\
	((veh_hndl) != HNDL_ERR); \
	(veh_hndl) = _next, _next = Call_reject_veh_list_get((call_reject_veh_list), CALL_REJECT_VEH_LIST_NEXT)) {
#define	Call_reject_veh_list_each_end } LL_Rewind(call_reject_veh_list); }

LinkedList *    		Calls_list_create();
RET_STATUS 			Calls_list_destroy();
RET_STATUS			Calls_list_add(CALL_HNDL);
RET_STATUS			Callls_list_remove(CALL_HNDL);
CALL_HNDL 			Calls_list_find(int);
CALL_HNDL 			Calls_list_get(short);

#define	Calls_list_each_get(call_hndl)	\
   { CALL_HNDL _next; LL_Rewind(calls_list); \
   for ((call_hndl) = Calls_list_get( CALLS_LIST_FIRST),\
	_next = Calls_list_get(CALLS_LIST_NEXT);\
	((call_hndl) != HNDL_ERR); \
	(call_hndl) = _next, _next = Calls_list_get(CALLS_LIST_NEXT)) {
#define	Calls_list_each_end } LL_Rewind(calls_list); }



LinkedList *    	        Call_msg_list_create();
RET_STATUS 			Call_msg_list_destroy(LinkedList *);
RET_STATUS 			Call_msg_list_add(LinkedList *, CALL_MSG_HNDL);
RET_STATUS 			Call_msg_list_remove(LinkedList *, CALL_MSG_HNDL);
CALL_MSG_HNDL			Call_msg_list_get(LinkedList *, short);

#define	Call_msg_list_each_get(call_msg_list, call_msg_hndl)	\
   { CALL_MSG_HNDL _next; LL_Rewind(call_msg_list); \
   for ((call_msg_hndl) = Call_msg_list_get((call_msg_list), CALL_MSG_LIST_FIRST),\
	_next = Call_msg_list_get((call_msg_list), CALL_MSG_LIST_NEXT);\
	(call_msg_hndl); \
	(call_msg_hndl) = _next, _next = Call_msg_list_get((call_msg_list), CALL_MSG_LIST_NEXT)) {
#define	Call_msg_list_each_end(call_msg_list) } LL_Rewind(call_msg_list); }
