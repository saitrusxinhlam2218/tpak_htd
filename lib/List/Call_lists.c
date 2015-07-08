/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call_lists.c,v $
*  @(#)  $Revision: 1.9 $
*  @(#)  $Date: 2004/05/12 12:20:05 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/List/Call_lists.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Call_lists.c,v 1.9 2004/05/12 12:20:05 jwelch Exp $";

#include "taxipak.h"
#include  "Object.h"
#include "List_public.h"
#include "calls.h"
#include "Call_public.h"
#include "Call_lists_public.h"
#include "Exception_private.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#ifdef DEBUG_OO
#include <sys/types.h>
#include <sys/times.h>		/* temp */
#endif

LinkedList *calls_list;

static int
call_reject_veh_list_compare(veh_1, veh_2)
   VEH_HNDL   veh_1;
   VEH_HNDL   veh_2;
{

  struct veh_driv * veh_1_ptr = (struct veh_driv *) veh_1;
  struct veh_driv * veh_2_ptr = (struct veh_driv *) veh_2;  

  if ( veh_1_ptr->veh_nbr <  veh_2_ptr->veh_nbr)
    return(LIST_LT);
  if (veh_1_ptr->veh_nbr == veh_2_ptr->veh_nbr) 
    return(LIST_EQ); 
  return(LIST_GT);  

}				/* end call_reject_veh_list_compare */

LinkedList *
Call_reject_veh_list_create()
{
  return((LinkedList *)LL_new());    
}				/* end Call_reject_veh_list_create */

RET_STATUS
Call_reject_veh_list_destroy(
			LinkedList *call_reject_veh_list_hndl)
{
  LL_Destroy( call_reject_veh_list_hndl );
}				/* end Call_reject_veh_list_create */

RET_STATUS
Call_reject_veh_list_add(call_reject_veh_list_hndl, veh_hndl)
   LinkedList *call_reject_veh_list_hndl;
   VEH_HNDL veh_hndl;
{
  return(LL_PriorityEnqueue( call_reject_veh_list_hndl, (void *)veh_hndl, call_reject_veh_list_compare ));
}				/* end Call_reject_veh_list_add */

RET_STATUS
Call_reject_veh_list_remove(call_reject_veh_list_hndl, veh_hndl)
   LinkedList *call_reject_veh_list_hndl;
   VEH_HNDL   veh_hndl;
{
  VEH_HNDL tmp_veh_hndl;

  if ( ( tmp_veh_hndl =LL_Remove( call_reject_veh_list_hndl, (void *)veh_hndl ) ) != HNDL_NULL )
    return(SUCCESS);
  else
    return(FAIL);  
}				/* end Call_reject_veh_list_remove */

VEH_HNDL
Call_reject_veh_list_find(LinkedList *call_reject_veh_list_hndl, short nbr)
{
  VEH_HNDL tmp_veh_hndl;
  LL_Rewind( call_reject_veh_list_hndl );

  while ((tmp_veh_hndl = (VEH_HNDL)LL_Get(call_reject_veh_list_hndl)) != HNDL_NULL)
    {
    if ( nbr == (short)Veh_get_value( tmp_veh_hndl, VEH_NBR ) )
      return (tmp_veh_hndl);      
      if (LL_Next( call_reject_veh_list_hndl ) != SUCCESS)
	break;
    }
  
  return(HNDL_NULL);
  
}				/* end Call_reject_veh_list_find */

VEH_HNDL
Call_reject_veh_list_get(
			 LinkedList *call_reject_veh_list_hndl,
			 short get_indx)
{
  switch (get_indx)
    {
    case LIST_FIRST:
      return( (VEH_HNDL)LL_GetFirst(call_reject_veh_list_hndl) );
      break;

    case LIST_NEXT:
      return( (VEH_HNDL)LL_GetNext(call_reject_veh_list_hndl) );
      break;

    default:
      return(HNDL_NULL);
      break;
    }    
}				/* end Call_reject_veh_list_get */

static int
calls_list_compare(call_1, call_2)
   CALL_HNDL call_1;
   CALL_HNDL call_2;
{
   struct calls *call_1_ptr = (struct calls *) call_1;
   struct calls *call_2_ptr = (struct calls *) call_2;


   if (call_1_ptr->call_number < call_2_ptr->call_number)
      return (LIST_LT);
   if (call_1_ptr->call_number == call_2_ptr->call_number)
      return (LIST_EQ);
   return (LIST_GT);
}				/* end calls_list_compare */

static int
calls_list_search(nbr, call_2)
   int   nbr;
   CALL_HNDL call_2;
{
   struct calls *call_2_ptr = (struct calls *) call_2;


   if (nbr < call_2_ptr->call_number)
      return (LIST_LT);
   if (nbr == call_2_ptr->call_number)
      return (LIST_EQ);
   return (LIST_GT);
}				/* end calls_list_search */

LinkedList *
Calls_list_create()
{
  return((LinkedList *)LL_new());      
}				/* end Calls_list_create */

RET_STATUS
Calls_list_destroy()
{
  LL_Destroy( calls_list );
}				/* end Calls_list_create */

RET_STATUS
Calls_list_add(call_hndl)
   CALL_HNDL call_hndl;
{
  return(LL_InsertNode( calls_list, (void *)call_hndl ));
}				/* end Calls_list_add */

RET_STATUS
Calls_list_remove(call_hndl)
   CALL_HNDL call_hndl;
{
  CALL_HNDL tmp_call_hndl;

  if ( ( tmp_call_hndl =LL_Remove( calls_list, (void *)call_hndl ) ) != HNDL_NULL )
    return(SUCCESS);
  else
    return(FAIL);
  
}				/* end Calls_list_remove */

CALL_HNDL
Calls_list_find(nbr)
   int   nbr;
{
  
  CALL_HNDL tmp_call_hndl;
  LL_Rewind( calls_list );

  while ((tmp_call_hndl = (CALL_HNDL)LL_Get(calls_list)) != HNDL_NULL)
    {
    if ( nbr == (int)Call_get_value( tmp_call_hndl, CALL_NBR ) )
      return (tmp_call_hndl);      
      if (LL_Next( calls_list ) != SUCCESS)
	break;
    }
  
  return(HNDL_NULL);
  
}				/* end Calls_list_find */

CALL_HNDL
Calls_list_get(short get_indx)
{
  switch (get_indx)
    {
    case LIST_FIRST:
      return( (CALL_HNDL)LL_GetFirst(calls_list) );
      break;

    case LIST_NEXT:
      return( (CALL_HNDL)LL_GetNext(calls_list) );
      break;

    default:
      return(HNDL_NULL);
      break;
    }      

}				/* end Calls_list_get */


LinkedList *
Call_msg_list_create()
{
  return((LinkedList *)LL_new());      
}

RET_STATUS
Call_msg_list_destroy( call_msg_list_hndl )
   LinkedList *call_msg_list_hndl;     
{
  return(LL_Destroy( call_msg_list_hndl ));
}			

RET_STATUS
Call_msg_list_add(call_msg_list_hndl, call_msg_hndl)
   LinkedList *call_msg_list_hndl;
   CALL_MSG_HNDL call_msg_hndl;
{
  return(LL_InsertNode(call_msg_list_hndl, (void *)call_msg_hndl));
}			

RET_STATUS
Call_msg_list_remove(call_msg_list_hndl, call_msg_hndl)
     LinkedList *call_msg_list_hndl;
     CALL_MSG_HNDL call_msg_hndl;
{
  CALL_MSG_HNDL tmp_cmsg_hndl;

  if ( ( tmp_cmsg_hndl =LL_Remove( call_msg_list_hndl, (void *)call_msg_hndl ) ) != HNDL_NULL )
    return(SUCCESS);
  else
    return(FAIL);    
}			

CALL_MSG_HNDL
Call_msg_list_get(LinkedList *call_msg_list_hndl, short get_indx)
{
  switch (get_indx)
    {
    case LIST_FIRST:
      return( (CALL_MSG_HNDL)LL_GetFirst(call_msg_list_hndl) );
      break;

    case LIST_NEXT:
      return( (CALL_MSG_HNDL)LL_GetNext(call_msg_list_hndl) );
      break;

    default:
      return(HNDL_NULL);
      break;
    }      
}			



