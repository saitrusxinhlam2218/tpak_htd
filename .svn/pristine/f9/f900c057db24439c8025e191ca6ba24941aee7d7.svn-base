/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Zone_lists.c,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2004/05/12 12:20:05 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/List/Zone_lists.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Zone_lists.c,v 1.5 2004/05/12 12:20:05 jwelch Exp $";


#include "taxipak.h"
#include  "Object.h"
#include "List_public.h"
#include "Call_public.h"
#include "Call_private.h"
#include "Zone_lists_public.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "TimeCalls_private.h"
#ifdef DEBUG_OO
#include <sys/types.h>
#include <sys/times.h>		/* temp */
#endif

extern short debug_state;

static int
zone_calls_list_compare(call_1, call_2)
   CALL_HNDL call_1;
   CALL_HNDL call_2;
{
    struct calls * call_1_ptr = (struct calls *) call_1;
    struct calls * call_2_ptr = (struct calls *) call_2;

   
    if ( call_1_ptr->priority <  call_2_ptr->priority)
        return(LIST_LT);
    if (call_1_ptr->priority == call_2_ptr->priority) 
        return(LIST_EQ); 
     return(LIST_GT);
}          /* end zone_calls_list_compare */

static int
zone_calls_list_search(call_1, call_2)
   CALL_HNDL call_1;
   CALL_HNDL call_2;
{
    struct calls * call_1_ptr = (struct calls *) call_1;
    struct calls * call_2_ptr = (struct calls *) call_2;

   
    if ( call_1_ptr->priority <  call_2_ptr->priority)
        return(LIST_LT);
    if (call_1_ptr == call_2_ptr)
        return(LIST_EQ); 
     return(LIST_GT);
}          /* end zone_calls_list_search */
   
LinkedList *
Zone_calls_list_create()
{
  return((LinkedList *)LL_new());    
}				

CALL_HNDL
Zone_calls_list_get(LinkedList *calls_list_hndl, short get_indx)
{
  switch (get_indx)
    {
    case LIST_FIRST:
      return( (CALL_HNDL)LL_GetFirst(calls_list_hndl) );
      break;

    case LIST_NEXT:
      return( (CALL_HNDL)LL_GetNext(calls_list_hndl) );
      break;

    default:
      return(HNDL_NULL);
      break;
    }  

}				/* end Zone_calls_list_get */

RET_STATUS
Zone_calls_list_add(calls_list_hndl, call_hndl)
   LinkedList *calls_list_hndl;
   CALL_HNDL call_hndl;
{
  return(LL_PriorityEnqueue((LinkedList *)calls_list_hndl, (void *)call_hndl, zone_calls_list_compare));
}				/* end Zone_calls_list_add */

RET_STATUS
Zone_calls_list_remove(calls_list_hndl, call_hndl)
   LinkedList *calls_list_hndl;
   CALL_HNDL call_hndl;
{
  CALL_HNDL tmp_call_hndl = HNDL_NULL;

  if ( ( tmp_call_hndl = LL_Remove( calls_list_hndl, (void *)call_hndl ) ) != HNDL_NULL )
    return(SUCCESS);
  else
    return(FAIL);
}				/* end Zone_calls_list_remove */

CALL_HNDL
Zone_calls_list_find(calls_list_hndl, call_hndl)
   LinkedList  *calls_list_hndl;
   CALL_HNDL call_hndl;
{
  CALL_HNDL tmp_call_hndl;
  
  LL_Rewind( calls_list_hndl );

  while ((tmp_call_hndl = (CALL_HNDL)LL_Get(calls_list_hndl)) != HNDL_NULL)
    {
      if ( call_hndl == tmp_call_hndl )
	return( tmp_call_hndl );
      if ( LL_Next( calls_list_hndl ) != SUCCESS )
	break;
    }

  return(HNDL_NULL);
  //   return ((CALL_HNDL) List_find((LIST_HNDL) calls_list_hndl, call_hndl, zone_calls_list_search));
}                              /* end Zone_calls_list_find */

RET_STATUS
Zone_calls_list_clear(calls_list_hndl)
   LinkedList *calls_list_hndl;
{

  LL_Rewind( calls_list_hndl );
  do {
    LL_DeleteNode( calls_list_hndl );
  } while( LL_Get( calls_list_hndl ) != HNDL_NULL );

  return(SUCCESS);

}                 /* end Zone_calls_list_clear */

static int
zone_tc_list_compare(call_1, call_2)
   TC_HNDL call_1;
   TC_HNDL call_2;
{
    TC_STRUCT * call_1_ptr = (TC_STRUCT *) call_1;
    TC_STRUCT * call_2_ptr = (TC_STRUCT *) call_2;

   
    if ( call_1_ptr->due_time <  call_2_ptr->due_time)
        return(LIST_LT);
    if (call_1_ptr->due_time == call_2_ptr->due_time) 
        return(LIST_EQ); 
     return(LIST_GT);
}          /* end zone_tc_list_compare */

static int
zone_tc_list_search(call_1, call_2)
   TC_HNDL call_1;
   TC_HNDL call_2;
{
    TC_STRUCT * call_1_ptr = (TC_STRUCT *) call_1;
    TC_STRUCT * call_2_ptr = (TC_STRUCT *) call_2;

    if ( call_1_ptr->due_time <  call_2_ptr->due_time)
        return(LIST_LT);
    if (call_1_ptr == call_2_ptr)
        return(LIST_EQ); 
     return(LIST_GT);
}          /* end zone_tc_list_search */
   
LinkedList *
Zone_tc_list_create()
{
  return((LinkedList *)LL_new());    
}

TC_HNDL
Zone_tc_list_get(LinkedList *tc_list_hndl, short get_indx)
{
  switch (get_indx)
    {
    case LIST_FIRST:
      return( (TC_HNDL)LL_GetFirst(tc_list_hndl) );
      break;

    case LIST_NEXT:
      return( (TC_HNDL)LL_GetNext(tc_list_hndl) );
      break;

    default:
      return(HNDL_NULL);
      break;
    }  
  
}				/* end Zone_calls_list_get */

RET_STATUS
Zone_tc_list_add(tc_list_hndl, tc_hndl)
   LinkedList *tc_list_hndl;
   TC_HNDL tc_hndl;
{
  return( LL_InsertNode( tc_list_hndl, (void *)tc_hndl ) );

}				/* end Zone_calls_list_add */

RET_STATUS
Zone_tc_list_remove(tc_list_hndl, tc_hndl)
   LinkedList *tc_list_hndl;
   TC_HNDL tc_hndl;
{
  TC_HNDL tmp_tc_hndl = HNDL_NULL;

  if ( ( tmp_tc_hndl = LL_Remove( tc_list_hndl, (void *)tc_hndl ) ) != HNDL_NULL )
    return(SUCCESS);
  else
    return(FAIL);

}				/* end Zone_calls_list_remove */

TC_HNDL
Zone_tc_list_find(tc_list_hndl, tc_hndl)
   LinkedList *tc_list_hndl;
   TC_HNDL tc_hndl;
{
  TC_HNDL tmp_tc_hndl;
  
  LL_Rewind( tc_list_hndl );

  while ((tmp_tc_hndl = (TC_HNDL)LL_Get(tc_list_hndl)) != HNDL_NULL)
    {
      if ( tc_hndl == tmp_tc_hndl )
	return( tmp_tc_hndl );
      if ( LL_Next( tc_list_hndl ) != SUCCESS )
	break;
    }

  return(HNDL_NULL);
  
}                              /* end Zone_calls_list_find */

RET_STATUS
Zone_tc_list_clear(tc_list_hndl)
   LinkedList *tc_list_hndl;
{

  LL_Rewind( tc_list_hndl );
  do {
    LL_DeleteNode( tc_list_hndl );
  } while( LL_Get( tc_list_hndl ) != HNDL_NULL );

  return(SUCCESS);
  
}                 /* end Zone_calls_list_clear */
   
static int
zone_veh_list_compare(veh_1, veh_2)
   VEH_HNDL veh_1;
   VEH_HNDL veh_2;
{
    struct veh_driv * veh_1_ptr = (struct veh_driv *) veh_1;
    struct veh_driv * veh_2_ptr = (struct veh_driv *) veh_2;

#ifdef DEBUG_ME
    printf(" comparing %d(%c)(%lu) with %d(%c)(%lu)\n", 
                veh_1_ptr->veh_nbr, 
                veh_1_ptr->priority,
                veh_1_ptr->q_time,
                veh_2_ptr->veh_nbr, 
                veh_2_ptr->priority,
                veh_2_ptr->q_time);
#endif

    if ( veh_1_ptr->q_priority <  veh_2_ptr->q_priority)
        return(LIST_LT);
    if (veh_1_ptr->q_priority == veh_2_ptr->q_priority) 
    {
        if (veh_1_ptr->q_time < veh_2_ptr->q_time)
           return(LIST_LT);
        if (veh_1_ptr->q_time == veh_2_ptr->q_time)
           return(LIST_EQ); 
    }
    return(LIST_GT);
}          /* end Zone_veh_list_compare */

static int
zone_veh_list_search(veh_1, veh_2)
   VEH_HNDL veh_1;
   VEH_HNDL veh_2;
{
    struct veh_driv * veh_1_ptr = (struct veh_driv *) veh_1;
    struct veh_driv * veh_2_ptr = (struct veh_driv *) veh_2;

   
    if ( veh_1_ptr->q_priority <  veh_2_ptr->q_priority)
        return(LIST_LT);
    if (veh_1_ptr == veh_2_ptr)
        return(LIST_EQ); 
     return(LIST_GT);
}          /* end zone_veh_list_search */
   
LinkedList *
Zone_veh_list_create()
{
  return((LinkedList *)LL_new());      
}				/* end Zone_veh_list_create */

VEH_HNDL
Zone_veh_list_get(LinkedList *veh_list_hndl, short get_indx)
{
  switch (get_indx)
    {
    case LIST_FIRST:
      return( (VEH_HNDL)LL_GetFirst(veh_list_hndl) );
      break;

    case LIST_NEXT:
      return( (VEH_HNDL)LL_GetNext(veh_list_hndl) );
      break;

    default:
      return(HNDL_NULL);
      break;
    }    
}				/* end Zone_veh_list_get */

RET_STATUS
Zone_veh_list_add(veh_list_hndl, veh_hndl)
   LinkedList *veh_list_hndl;
   VEH_HNDL veh_hndl;
{
 
  return(LL_PriorityEnqueue((LinkedList *)veh_list_hndl, (void *)veh_hndl, zone_veh_list_compare));
}				/* end Zone_veh_list_add */

RET_STATUS
Zone_veh_list_remove(veh_list_hndl, veh_hndl)
   LinkedList *veh_list_hndl;
   VEH_HNDL veh_hndl;
{

  VEH_HNDL tmp_veh_hndl = HNDL_NULL;

  if ( ( tmp_veh_hndl = LL_Remove( veh_list_hndl, (void *)veh_hndl ) ) != HNDL_NULL )
    return(SUCCESS);
  else
    return(FAIL);
  
}				/* end Zone_veh_list_remove */

VEH_HNDL
Zone_veh_list_find(veh_list_hndl, veh_hndl)
   LinkedList *veh_list_hndl;
   VEH_HNDL veh_hndl;
{
  VEH_HNDL tmp_veh_hndl;
  
  LL_Rewind( veh_list_hndl );

  while ((tmp_veh_hndl = (VEH_HNDL)LL_Get(veh_list_hndl)) != HNDL_NULL)
    {
      if ( veh_hndl == tmp_veh_hndl )
	return( tmp_veh_hndl );
      if ( LL_Next( veh_list_hndl ) != SUCCESS )
	break;
    }

  return(HNDL_NULL);

}                              /* end Zone_veh_list_find */
   
RET_STATUS
Zone_veh_list_clear(veh_list_hndl)
   LinkedList *veh_list_hndl;
{
  LL_Rewind( veh_list_hndl );
  do {
    LL_DeleteNode( veh_list_hndl );
  } while( LL_Get( veh_list_hndl ) != HNDL_NULL );

  return(SUCCESS);
  
}                 /* end Zone_veh_list_clear */

