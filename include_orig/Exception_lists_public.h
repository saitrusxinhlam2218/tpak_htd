/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Exception_lists_public.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:43:04 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Exception_lists_public.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Exception_lists_public.h,v $	$Revision: 1.2 $"

#define EXCPT_LIST_FIRST              LIST_FIRST
#define EXCPT_LIST_NEXT               LIST_NEXT

EXCPT_LIST_HNDL	                Excpt_list_create();
RET_STATUS 			Excpt_list_destroy(EXCPT_LIST_HNDL);
RET_STATUS 			Excpt_list_add(EXCPT_LIST_HNDL, int);
RET_STATUS 			Excpt_list_remove(EXCPT_LIST_HNDL, int);
int				Excpt_list_find(EXCPT_LIST_HNDL, int);

#define	Excpt_list_each_get(excpt_list, exc_nbr)	\
   { int _next; \
   for ((exc_nbr) = Excpt_list_get((excpt_list), EXCPT_LIST_FIRST),\
	_next = Excpt_list_get((excpt_list), EXCPT_LIST_NEXT);\
	(exc_nbr); \
	(exc_nbr) = _next, _next = Excpt_list_get((excpt_list), EXCPT_LIST_NEXT)) {
#define	Excpt_list_each_end }}

