/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Vehicle_lists.c,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2004/05/12 12:56:39 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Vehicle_lists.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Vehicle_lists.c,v 1.5 2004/05/12 12:56:39 jwelch Exp $";

#include "taxipak.h"
#include  "Object.h"

#include "List_public.h"
#include "Call_public.h"
#include "Exception_private.h"
#include "Vehicle_lists_public.h"

LinkedList *
Veh_calls_list_create()
{
   return ((VEH_CALLS_LIST_HNDL)LL_new());
}				/* end Veh_calls_list_create */

RET_STATUS
Veh_calls_list_destroy(veh_calls_list_hndl)
   LinkedList *veh_calls_list_hndl;
{
   return (LL_Destroy((LinkedList *) veh_calls_list_hndl));
}				/* end Veh_calls_list_create */

int
Veh_calls_list_add(veh_calls_list_hndl, call_hndl)
   LinkedList *veh_calls_list_hndl;
   CALL_HNDL call_hndl;
{
  return(LL_InsertNode(veh_calls_list_hndl, (void *)call_hndl));
}				/* end Veh_calls_list_add */

int
Veh_calls_list_remove(veh_calls_list_hndl, call_hndl)
   LinkedList *veh_calls_list_hndl;
   CALL_HNDL call_hndl;
{
  CALL_HNDL tmp_call_hndl = HNDL_NULL;

  if ( ( tmp_call_hndl = LL_Remove( veh_calls_list_hndl, (void *)call_hndl ) ) != HNDL_NULL )
    return(SUCCESS);
  else
    return(FAIL);
}				/* end Veh_calls_list_remove */

CALL_HNDL
Veh_calls_list_find(veh_calls_list_hndl, call_hndl)
   LinkedList  *veh_calls_list_hndl;
   CALL_HNDL call_hndl;
{
  CALL_HNDL tmp_call_hndl;

  LL_Rewind( veh_calls_list_hndl );

  while ((tmp_call_hndl = (CALL_HNDL)LL_Get(veh_calls_list_hndl)) != HNDL_NULL)
    {
      if ( call_hndl == tmp_call_hndl )
	return( tmp_call_hndl );
      if ( LL_Next( veh_calls_list_hndl ) != SUCCESS )
	break;
    }
  return (HNDL_NULL);
}                              /* end Veh_calls_list_find */
   
CALL_HNDL
Veh_calls_list_get(veh_calls_list_hndl, get_indx)
   LinkedList *veh_calls_list_hndl;
   int   get_indx;
{
  switch (get_indx)
    {
    case LIST_FIRST:
      return( (CALL_HNDL)LL_GetFirst(veh_calls_list_hndl) );
      break;
      
    case LIST_NEXT:
      return( (CALL_HNDL)LL_GetNext(veh_calls_list_hndl) );
      break;
      
    default:
      return(HNDL_NULL);
      break;
    }
}				/* end Veh_calls_list_get */

RET_STATUS
Veh_calls_list_clear(veh_calls_list_hndl)
   LinkedList *veh_calls_list_hndl;
{
  LL_Rewind( veh_calls_list_hndl );
  do {
    LL_DeleteNode( veh_calls_list_hndl );
  } while( LL_Get( veh_calls_list_hndl ) != HNDL_NULL );

  return(SUCCESS);  
}                               /* end Veh_calls_list_clear */

static int
veh_excpt_list_compare( excpt_1, excpt_2 )
     EXCPT_HNDL  excpt_1;
     EXCPT_HNDL  excpt_2;
{
  EXCEPT *except_1_ptr = (EXCEPT *)excpt_1;
  EXCEPT *except_2_ptr = (EXCEPT *)excpt_2;

  if ( except_1_ptr->priority < except_2_ptr->priority )
    return (LIST_LT);
  if ( except_1_ptr->priority == except_2_ptr->priority )
    return (LIST_EQ);
  return (LIST_GT);
}

LinkedList *
Veh_excpt_list_create()
{
  return((LinkedList *)LL_new());
}				/* end Veh_excpt_list_create */

RET_STATUS
Veh_excpt_list_destroy(veh_excpt_list_hndl)
   LinkedList *veh_excpt_list_hndl;
{
   return (LL_Destroy(veh_excpt_list_hndl));
}				/* end Veh_excpt_list_create */

int
Veh_excpt_list_add(veh_excpt_list_hndl, excpt_hndl)
   LinkedList *veh_excpt_list_hndl;
   EXCPT_HNDL excpt_hndl;
{
  return(LL_PriorityEnqueue( veh_excpt_list_hndl, (void *)excpt_hndl, veh_excpt_list_compare ));
}				/* end Veh_excpt_list_add */

int
Veh_excpt_list_remove(veh_excpt_list_hndl, excpt_hndl)
   LinkedList *veh_excpt_list_hndl;
   EXCPT_HNDL excpt_hndl;
{
  EXCPT_HNDL tmp_excpt_hndl;

  if ( ( tmp_excpt_hndl = LL_Remove( veh_excpt_list_hndl, (void *)excpt_hndl ) ) != HNDL_NULL )
    return(SUCCESS);
  else
    return(FAIL);
}				/* end Veh_excpt_list_remove */

EXCPT_HNDL
Veh_excpt_list_find(veh_excpt_list_hndl, excpt_hndl)
   LinkedList *veh_excpt_list_hndl;
   EXCPT_HNDL excpt_hndl;
{
  EXCPT_HNDL tmp_excpt_hndl;
  LL_Rewind( veh_excpt_list_hndl );

  while ((tmp_excpt_hndl = (EXCPT_HNDL)LL_Get(veh_excpt_list_hndl)) != HNDL_NULL)
    {
      if ( tmp_excpt_hndl == excpt_hndl )
	return(tmp_excpt_hndl);
      if (LL_Next( veh_excpt_list_hndl ) != SUCCESS)
	break;
    }

  return(HNDL_NULL);

}                              /* end Veh_excpt_list_find */
   
EXCPT_HNDL
Veh_excpt_list_get(veh_excpt_list_hndl, get_indx)
   LinkedList *veh_excpt_list_hndl;
   int   get_indx;
{
  switch (get_indx)
    {
    case LIST_FIRST:
      return( (EXCPT_HNDL)LL_GetFirst(veh_excpt_list_hndl) );
      break;

    case LIST_NEXT:
      return( (EXCPT_HNDL)LL_GetNext(veh_excpt_list_hndl) );
      break;

    default:
      return(HNDL_NULL);
    }
}				/* end Veh_excpt_list_get */

RET_STATUS
Veh_excpt_list_clear(veh_excpt_list_hndl)
   VEH_EXCPT_LIST_HNDL veh_excpt_list_hndl;
{
  LL_Rewind( veh_excpt_list_hndl );
  do {
    LL_DeleteNode( veh_excpt_list_hndl );
  } while( LL_Get( veh_excpt_list_hndl ) != HNDL_NULL );

  return(SUCCESS);
}                               /* end Veh_excpt_list_clear */


static int
veh_http_list_compare( http_1, http_2 )
     HNDL  http_1;
     HNDL  http_2;
{
  return (LIST_GT);
}

LinkedList *
Veh_http_list_create()
{
  return((LinkedList *)LL_new());  
}				/* end Veh_http_list_create */

RET_STATUS
Veh_http_list_destroy(veh_http_list_hndl)
     LinkedList * veh_http_list_hndl;
{
   return (LL_Destroy(veh_http_list_hndl));  
}				/* end Veh_http_list_create */

int
Veh_http_list_add(veh_http_list_hndl, http_hndl)
   LinkedList *veh_http_list_hndl;
   HTTP_HNDL http_hndl;
{
  return(LL_AddNode(veh_http_list_hndl, (void *)http_hndl));
}				/* end Veh_http_list_add */

int
Veh_http_list_remove(veh_http_list_hndl, http_hndl)
   LinkedList *veh_http_list_hndl;
   HTTP_HNDL http_hndl;
{
  HTTP_HNDL tmp_http_hndl;

  if ( ( tmp_http_hndl = LL_Remove( veh_http_list_hndl, (void *)http_hndl ) ) != HNDL_NULL )
    return(SUCCESS);
  else
    return(FAIL);
}				/* end Veh_http_list_remove */

HTTP_HNDL
Veh_http_list_find(veh_http_list_hndl, http_hndl)
   LinkedList *veh_http_list_hndl;
   HTTP_HNDL http_hndl;
{
  HTTP_HNDL tmp_http_hndl;
  LL_Rewind( veh_http_list_hndl );
  while ((tmp_http_hndl = (HTTP_HNDL)LL_Get(veh_http_list_hndl)) != HNDL_NULL)
    {
      if ( tmp_http_hndl == http_hndl )
	return( tmp_http_hndl );
      if ( LL_Next( veh_http_list_hndl ) != SUCCESS )
	break;
    }

  return(HNDL_NULL);
}                              /* end Veh_http_list_find */
   
HTTP_HNDL
Veh_http_list_get(veh_http_list_hndl, get_indx)
   LinkedList *veh_http_list_hndl;
   int   get_indx;
{
  switch (get_indx)
    {
    case LIST_FIRST:
      return( (HTTP_HNDL)LL_GetFirst(veh_http_list_hndl) );
      break;

    case LIST_NEXT:
      return( (HTTP_HNDL)LL_GetNext(veh_http_list_hndl) );
      break;

    default:
      return(HNDL_NULL);
    }

}				/* end Veh_http_list_get */

RET_STATUS
Veh_http_list_clear(veh_http_list_hndl)
   LinkedList *veh_http_list_hndl;
{
  LL_Rewind( veh_http_list_hndl );
  do {
    LL_DeleteNode( veh_http_list_hndl );
  } while( LL_Get( veh_http_list_hndl ) != HNDL_NULL );

  return(SUCCESS);
}                               /* end Veh_http_list_clear */
