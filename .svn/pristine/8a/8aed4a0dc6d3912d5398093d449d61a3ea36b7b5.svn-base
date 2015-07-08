/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: List_private.h,v $
*  @(#)  $Revision: 1.7 $
*  @(#)  $Date: 2003/01/28 16:38:43 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/List_private.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: List_private.h,v $	$Revision: 1.7 $"

#include "List_public.h"
/*-----------------------------------------------------
 *	List private.h
 *-----------------------------------------------------
 */

#ifndef LIST_PRIVATE

/*
 *	structures
 */
typedef struct list_elements
  {
	GENERIC					 data;
	struct list_elements	*prev,
							*next,
							*left,
							*right;
	short					 balance;
  } LIST_ELEMENT;

typedef struct list_struct
  {
	LIST_ELEMENT			*tree,
							*list,
							*current_element;
	int					 	(*compare_func)();
	struct list_struct		*next;
  } LIST;


/*
 *	defines
 */
#define NULL_LIST			((LIST *)NULL)
#define NULL_ELEMENT		((LIST_ELEMENT *)NULL)

#define MAX_LISTS			5000
#define BLOCK_OF_LISTS		1000
#define MAX_ELEMENTS		30000
#define BLOCK_OF_ELEMENTS	10000
#define MAX_MEMORY_BLOCKS	1000

#define FOR_EACH_LIST_OBJ(start, work) \
			for (work = start; work; work = work->next)

#define ORDERED_LIST(list) \
			(list->compare_func != NULL)
#define UNORDERED_LIST(list) \
			(!(ORDERED_LIST(list)))	
/*
 *	functions
 */
static RET_STATUS 
add_to_list ARGS((LIST_HANDLE, HNDL));
static RET_STATUS 
add_to_except_list ARGS((LIST_HANDLE, HNDL));
static RET_STATUS 
insert_into_ordered_list ARGS((LIST *, LIST_ELEMENT *, int (*compare_func )()));
static RET_STATUS 
insert_into_unordered_list ARGS((LIST *, LIST_ELEMENT *));
static RET_STATUS 
remove_from_list ARGS((LIST_HANDLE, HNDL));
static RET_STATUS 
remove_from_except_list ARGS((LIST_HANDLE, HNDL));
static LIST_ELEMENT *
remove_from_ordered_list ARGS((LIST *, HNDL, int (*compare_func )()));
static LIST_ELEMENT *
remove_from_unordered_list ARGS((LIST *, HNDL));
static HNDL 
find_in_list ARGS((LIST_HANDLE, HNDL, int (*compare_func )()));
static LIST_ELEMENT *
find_element_in_tree ARGS((LIST *, HNDL, int (*compare_func )()));
static LIST_ELEMENT *
find_element_in_list ARGS((LIST *, HNDL, int (*compare_fund )()));
static HNDL 
get_from_list ARGS((LIST *, int));
static HNDL
get_current_element_from_list ARGS((LIST *));
static RET_STATUS 
traverse_list ARGS((LIST *, int (*action_func )()));
static RET_STATUS 
add_list_to_free_lists ARGS((LIST *));
static RET_STATUS 
add_list_to_free_except_lists ARGS((LIST *));
static RET_STATUS 
add_element_to_free_elements ARGS((LIST_ELEMENT *));
static RET_STATUS 
add_element_to_free_except_elements ARGS((LIST_ELEMENT *));
static RET_STATUS 
zero_list ARGS((LIST *));
static RET_STATUS 
zero_element ARGS((LIST_ELEMENT *));
static LIST *
get_new_list ARGS((void));
static LIST *
get_new_except_list ARGS((void));
static LIST_ELEMENT *
get_new_element ARGS((void));
static LIST_ELEMENT *
get_new_except_element ARGS((void));
static LIST *
allocate_list ARGS((int));
static LIST_ELEMENT *
allocate_element ARGS((int));

#define LIST_PRIVATE
#endif
