/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Fleet_lists.c,v $
*  @(#)  $Revision: 1.10 $
*  @(#)  $Date: 2004/05/12 12:20:05 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/List/Fleet_lists.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Fleet_lists.c,v 1.10 2004/05/12 12:20:05 jwelch Exp $";


#include "taxipak.h"
#include  "Object.h"
#include "List_public.h"
#include "LL.h"
#include "Fleet_lists_public.h"
#include <sys/types.h>
#include <sys/times.h>		/* temp */
#include "Call_private.h"
#include "Call_public.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "Fleet_public.h"
#include "Exception_private.h"
#include "Packet_ID_private.h"


LinkedList *fleet_pkt_id_list;
LinkedList *fleet_veh_id_list;


static int
fleet_driver_veh_list_compare(veh_1, veh_2)
   VEH_HNDL veh_1;
   VEH_HNDL veh_2;
{
    struct veh_driv * veh_1_ptr = (struct veh_driv *) veh_1;
    struct veh_driv * veh_2_ptr = (struct veh_driv *) veh_2;

   
    if ( veh_1_ptr->driver_id <  veh_2_ptr->driver_id)
        return(LIST_LT);
    if (veh_1_ptr->driver_id == veh_2_ptr->driver_id) 
        return(LIST_EQ); 
     return(LIST_GT);
}          /* end fleet_driver_veh_list_compare */

static int
fleet_susp_veh_list_search(veh_nbr, veh_2)  
   short veh_nbr;
   VEH_HNDL veh_2;
{
    struct veh_driv * veh_2_ptr = (struct veh_driv *) veh_2;
   
    if ( veh_nbr <  veh_2_ptr->veh_nbr)
        return(LIST_LT);
    if (veh_nbr == veh_2_ptr->veh_nbr )
        return(LIST_EQ); 
     return(LIST_GT);
}          /* end fleet_susp_driver_veh_list_search */

static int
fleet_susp_veh_list_compare(veh_1, veh_2)
   VEH_HNDL veh_1;
   VEH_HNDL veh_2;
{
    struct veh_driv * veh_1_ptr = (struct veh_driv *) veh_1;
    struct veh_driv * veh_2_ptr = (struct veh_driv *) veh_2;
   
    if ( veh_1_ptr->veh_nbr <  veh_2_ptr->veh_nbr )
        return(LIST_LT);
    if (veh_1_ptr->veh_nbr == veh_2_ptr->veh_nbr) 
        return(LIST_EQ); 
     return(LIST_GT);
}          /* end fleet_susp_veh_list_compare */

static int
fleet_susp_drv_list_search(drv_nbr, veh_2)  
   int drv_nbr;
   VEH_HNDL veh_2;
{
    struct veh_driv * veh_2_ptr = (struct veh_driv *) veh_2;
   
    if ( drv_nbr <  veh_2_ptr->driver_id )
        return(LIST_LT);
    if (drv_nbr == veh_2_ptr->driver_id )
        return(LIST_EQ); 
     return(LIST_GT);
}          /* end fleet_susp__list_search */

static int
fleet_susp_drv_list_compare(veh_1, veh_2)
   VEH_HNDL veh_1;
   VEH_HNDL veh_2;
{
    struct veh_driv * veh_1_ptr = (struct veh_driv *) veh_1;
    struct veh_driv * veh_2_ptr = (struct veh_driv *) veh_2;
   
    if ( veh_1_ptr->driver_id <  veh_2_ptr->driver_id )
        return(LIST_LT);
    if (veh_1_ptr->driver_id == veh_2_ptr->driver_id) 
        return(LIST_EQ); 
     return(LIST_GT);
}          /* end fleet_susp_drv_list_compare */

static int
fleet_driver_veh_list_search(driver_id, veh_2)   /* BOGUS */
   unsigned long driver_id;
   VEH_HNDL veh_2;
{
    struct veh_driv * veh_2_ptr = (struct veh_driv *) veh_2;

    if ( driver_id <  veh_2_ptr->driver_id)
        return(LIST_LT);
    if (driver_id == veh_2_ptr->driver_id )
        return(LIST_EQ); 
     return(LIST_GT);
}

LinkedList *
Fleet_driver_veh_list_create()
{
  return ((LinkedList *)LL_new());
}				/* end Fleet_driver_veh_list_create */

RET_STATUS
Fleet_driver_veh_list_destroy(fleet_driver_veh_list)
LinkedList  *fleet_driver_veh_list;
{
  VEH_HNDL veh_hndl;
  
  LL_Rewind(fleet_driver_veh_list);
  do {
    veh_hndl = (VEH_HNDL)LL_DeleteNode(fleet_driver_veh_list);
  } while (LL_Next(fleet_driver_veh_list) == 0);
  return(SUCCESS);
  
}				/* end Fleet_driver_veh_list_create */

RET_STATUS
Fleet_driver_veh_list_add(fleet_driver_veh_list, veh_hndl)
  LinkedList *fleet_driver_veh_list;
 VEH_HNDL veh_hndl;
{
  return(LL_PriorityEnqueue( (LinkedList *)fleet_driver_veh_list, (void *)veh_hndl, fleet_driver_veh_list_compare ));
}				/* end Fleet_driver_veh_list_add */

RET_STATUS
Fleet_driver_veh_list_remove(fleet_driver_veh_list, veh_hndl)
   LinkedList *fleet_driver_veh_list;
   VEH_HNDL veh_hndl;
{
  VEH_HNDL tmp_veh_hndl = HNDL_NULL;
  
  if ( ( tmp_veh_hndl =LL_Remove( fleet_driver_veh_list, (void *)veh_hndl ) ) != HNDL_NULL )
    return(SUCCESS);
  else
    return(FAIL);

}				/* end Fleet_driver_veh_list_remove */

VEH_HNDL
Fleet_driver_veh_list_find(fleet_driver_veh_list, driver_id)
   LinkedList *fleet_driver_veh_list;
   int driver_id;
{
  VEH_HNDL tmp_veh_hndl;
  LL_Rewind( fleet_driver_veh_list );
  while ((tmp_veh_hndl = (VEH_HNDL)LL_Get(fleet_driver_veh_list)) != HNDL_NULL)
    {
      if ( driver_id == (int)Veh_get_value( tmp_veh_hndl, VEH_DRV_ID ) )
	return (tmp_veh_hndl);
      if (LL_Next( fleet_driver_veh_list ) != SUCCESS)
	break;
    }

  return(HNDL_NULL);
  
  //return ((VEH_HNDL) List_find((LIST_HNDL) fleet_driver_veh_list, (HNDL) driver_id, fleet_driver_veh_list_search)); 
}                              /* end Fleet_driver_veh_list_find */

VEH_HNDL
Fleet_driver_veh_list_get(LinkedList *fleet_driver_veh_list, short get_indx)
{

  switch ( get_indx )
    {
    case LIST_FIRST:
      return( (VEH_HNDL)LL_GetFirst( fleet_driver_veh_list ) );
      break;

    case LIST_NEXT:
      return( (VEH_HNDL)LL_GetNext( fleet_driver_veh_list ) );
      break;

    default:
      return(HNDL_NULL);
    }
  
  //   return ((VEH_HNDL) List_get((LIST_HNDL) fleet_driver_veh_list, get_indx));
}                               /* end Fleet_driver_veh_list_get */


static int
veh_id_list_compare(veh_1, veh_2)
   VEH_HNDL veh_1;
   VEH_HNDL veh_2;
{
    struct veh_driv * veh_1_ptr = (struct veh_driv *) veh_1;
    struct veh_driv * veh_2_ptr = (struct veh_driv *) veh_2;

   
    if ( veh_1_ptr->rf_id <  veh_2_ptr->rf_id)
        return(LIST_LT);
    if (veh_1_ptr->rf_id == veh_2_ptr->rf_id) 
        return(LIST_EQ); 
     return(LIST_GT);
}          /* end veh_id_list_compare */

static int
veh_id_list_search(rf_id, veh_2)   /* BOGUS */
   unsigned long rf_id;
   VEH_HNDL veh_2;
{
    struct veh_driv * veh_2_ptr = (struct veh_driv *) veh_2;

   
    if ( rf_id <  veh_2_ptr->rf_id)
        return(LIST_LT);
    if (rf_id == veh_2_ptr->rf_id )
        return(LIST_EQ); 
     return(LIST_GT);
}          /* end veh_id_list_search */
   
LinkedList *
Fleet_veh_id_list_create()
{
   return ((LinkedList *)LL_new());
}				/* end Fleet_veh_id_list_create */

RET_STATUS
Fleet_veh_id_list_destroy()
{
  VEH_HNDL veh_hndl;
  
  LL_Rewind(fleet_veh_id_list);
  do {
    veh_hndl = (VEH_HNDL)LL_DeleteNode(fleet_veh_id_list);
  } while (LL_Next(fleet_veh_id_list) == 0 );
  return(SUCCESS);
}				/* end Fleet_veh_id_list_create */

RET_STATUS
Fleet_veh_id_list_add(veh_hndl)
   VEH_HNDL veh_hndl;
{
  //   return (List_add((LIST_HNDL) fleet_veh_id_list, (HNDL) veh_hndl));
  return(LL_PriorityEnqueue( (LinkedList *)fleet_veh_id_list, (void *) veh_hndl, veh_id_list_compare ) );
}				/* end Fleet_veh_id_list_add */

RET_STATUS
Fleet_veh_id_list_remove( veh_hndl)
   VEH_HNDL veh_hndl;
{
  VEH_HNDL tmp_veh_hndl = HNDL_NULL;
  
  if ( ( tmp_veh_hndl =LL_Remove( fleet_veh_id_list, (void *)veh_hndl ) ) != HNDL_NULL )
    return(SUCCESS);
  else
    return(FAIL);
  
  //   return (List_remove((LIST_HNDL) fleet_veh_id_list, (HNDL) veh_hndl));
}				/* end Fleet_veh_id_list_remove */

VEH_HNDL
Fleet_veh_id_list_find(rf_idx)
   unsigned long rf_idx;
{
  VEH_HNDL tmp_veh_hndl;

  LL_Rewind( fleet_veh_id_list );
  while ((tmp_veh_hndl = (VEH_HNDL)LL_Get(fleet_veh_id_list)) != HNDL_NULL)
    {
    if ( rf_idx == (unsigned long)Veh_get_value( tmp_veh_hndl, VEH_RF_ID ) )
      return( tmp_veh_hndl );      
      if (LL_Next( fleet_veh_id_list ) != SUCCESS)
	break;
    }
  return (HNDL_NULL);
  // return ((VEH_HNDL) List_find((LIST_HNDL) fleet_veh_id_list, (HNDL) rf_idx, veh_id_list_search)); 
}                              /* end Fleet_veh_id_list_find */
   
VEH_HNDL
Fleet_veh_id_list_get(short get_indx)
{
  switch ( get_indx )
    {
    case LIST_FIRST:
      return( (VEH_HNDL)LL_GetFirst( fleet_veh_id_list ) );
      break;

    case LIST_NEXT:
      return( (VEH_HNDL)LL_GetNext( fleet_veh_id_list ) );
      break;

    default:
      return(HNDL_NULL);
    }
  //   return ((VEH_HNDL) List_get((LIST_HNDL) fleet_veh_id_list, get_indx));
}				/* end Fleet_veh_id_list_get */

static int
veh_nbr_list_compare(veh_1, veh_2)
   VEH_HNDL veh_1;
   VEH_HNDL veh_2;
{
    struct veh_driv * veh_1_ptr = (struct veh_driv *) veh_1;
    struct veh_driv * veh_2_ptr = (struct veh_driv *) veh_2;

   
    if ( veh_1_ptr->veh_nbr <  veh_2_ptr->veh_nbr)
        return(LIST_LT);
    if (veh_1_ptr->veh_nbr == veh_2_ptr->veh_nbr) 
        return(LIST_EQ); 
     return(LIST_GT);
}          /* end veh_nbr_list_compare */

static int
veh_nbr_list_search(nbr, veh_2)  
   short nbr;
   VEH_HNDL veh_2;
{
    struct veh_driv * veh_2_ptr = (struct veh_driv *) veh_2;

   
    if ( nbr <  veh_2_ptr->veh_nbr)
        return(LIST_LT);
    if (nbr == veh_2_ptr->veh_nbr )
        return(LIST_EQ); 
     return(LIST_GT);
}          /* end veh_nbr_list_search */

static int
bct_list_search( nbr, call_2)
  short nbr;
CALL_HNDL call_2;
{
  struct calls * call_2_ptr = (struct calls *) call_2;

  if ( nbr < call_2_ptr->pickup_zone )
    return(LIST_LT);
  if ( nbr == call_2_ptr->pickup_zone )
    return(LIST_EQ);
  return(LIST_GT);
}

static int
bct_list_compare(call_1, call_2)
   CALL_HNDL call_1;
   CALL_HNDL call_2;
{
    struct calls * call_1_ptr = (struct calls *) call_1;
    struct calls * call_2_ptr = (struct calls *) call_2;

   
    if ( call_1_ptr->pickup_zone <  call_2_ptr->pickup_zone)
        return(LIST_LT);
    if (call_1_ptr->pickup_zone == call_2_ptr->pickup_zone) 
        return(LIST_EQ); 
     return(LIST_GT);
}


   
LinkedList *
Fleet_veh_nbr_list_create()
{
  return((LinkedList *)LL_new());
  //   return ((FLEET_VEH_NBR_LIST_HNDL)List_create(veh_nbr_list_compare));
}				/* end Fleet_veh_nbr_list_create */

RET_STATUS
Fleet_veh_nbr_list_destroy(veh_nbr_list_hndl)
   LinkedList *veh_nbr_list_hndl;
{
  VEH_HNDL tmp_veh_hndl;
  LL_Rewind( veh_nbr_list_hndl );
  do {
    tmp_veh_hndl = (VEH_HNDL)LL_DeleteNode(veh_nbr_list_hndl);
  } while( LL_Next( veh_nbr_list_hndl ) == 0 );
  
  return (SUCCESS);
}				/* end Fleet_veh_nbr_list_create */

RET_STATUS
Fleet_veh_nbr_list_add(veh_nbr_list_hndl, veh_hndl)
   LinkedList *veh_nbr_list_hndl;
   VEH_HNDL veh_hndl;
{
  return ( LL_PriorityEnqueue( veh_nbr_list_hndl, (void *)veh_hndl, veh_nbr_list_compare ) );
  //   return (List_add((LIST_HNDL) veh_nbr_list_hndl, (HNDL) veh_hndl));
}				/* end Fleet_veh_nbr_list_add */

RET_STATUS
Fleet_veh_nbr_list_remove(veh_nbr_list_hndl, veh_hndl)
   LinkedList *veh_nbr_list_hndl;
   VEH_HNDL veh_hndl;
{
  VEH_HNDL tmp_veh_hndl = HNDL_NULL;
  
  if ( ( tmp_veh_hndl =(VEH_HNDL)LL_Remove( veh_nbr_list_hndl, (void *)veh_hndl ) ) != HNDL_NULL )
    return(SUCCESS);
  else
    return(FAIL);
  //   return (List_remove((LIST_HNDL) veh_nbr_list_hndl, (HNDL) veh_hndl));
}				/* end Fleet_veh_nbr_list_remove */

VEH_HNDL
Fleet_veh_nbr_list_find(
   LinkedList *veh_nbr_list_hndl,
   short nbr)
{
  VEH_HNDL tmp_veh_hndl;
  LL_Rewind( veh_nbr_list_hndl );

  while ((tmp_veh_hndl = (VEH_HNDL)LL_Get(veh_nbr_list_hndl)) != HNDL_NULL)
    {
    if ( nbr == (short)Veh_get_value( tmp_veh_hndl, VEH_NBR ) )
      return (tmp_veh_hndl);      
      if (LL_Next( veh_nbr_list_hndl ) != SUCCESS)
	break;
    }
  
  return(HNDL_NULL);
  
  //   return ((VEH_HNDL) List_find((LIST_HNDL) veh_nbr_list_hndl, (HNDL) nbr, veh_nbr_list_search)); 
}                              /* end Fleet_veh_nbr_list_find */
   


LinkedList *
Fleet_zone_calls_list_create()
{
  return ((LinkedList *)LL_new());
}				/* end Fleet_zone_calls_list_create */

ZONE_HNDL
Fleet_zone_calls_list_get(LinkedList *fleet_zone_calls_hndl, short get_indx)
{
  switch (get_indx)
    {
    case LIST_FIRST:
      return( (ZONE_HNDL)LL_GetFirst(fleet_zone_calls_hndl) );
      break;

    case LIST_NEXT:
      return( (ZONE_HNDL)LL_GetNext(fleet_zone_calls_hndl) );
      break;

    default:
      return(HNDL_NULL);
      break;
    }
  
  // return ((ZONE_HNDL) List_get((LIST_HNDL) fleet_zone_calls_hndl, get_indx));
}				/* end Fleet_zone_calls_list_get */

RET_STATUS
Fleet_zone_calls_list_add(fleet_zone_calls_hndl, zone_hndl)
   LinkedList *fleet_zone_calls_hndl;
   ZONE_HNDL zone_hndl;
{
  return( LL_InsertNode( fleet_zone_calls_hndl, (void *)zone_hndl ) );
  //   return (List_add((LIST_HNDL) fleet_zone_calls_hndl, (HNDL) zone_hndl));
}				/* end Fleet_zone_calls_list_add */

RET_STATUS
Fleet_zone_calls_list_remove(fleet_zone_calls_hndl, zone_hndl)
   LinkedList *fleet_zone_calls_hndl;
   ZONE_HNDL zone_hndl;
{
  ZONE_HNDL tmp_zone_hndl = HNDL_NULL;

  if ( ( tmp_zone_hndl = (ZONE_HNDL)LL_Remove( fleet_zone_calls_hndl, (void *)zone_hndl ) ) != HNDL_NULL )
    return (SUCCESS);
  else
    return (FAIL);
  
  //   return (List_remove((LIST_HNDL) fleet_zone_calls_hndl, (HNDL) zone_hndl));
}				/* end Fleet_zone_calls_list_remove */

static int
fleet_excpt_list_compare( excpt1, excpt2 )
     EXCPT_HNDL  excpt1;
     EXCPT_HNDL  excpt2;
{
  EXCEPT *excpt_1_ptr = (EXCEPT *)excpt1;
  EXCEPT *excpt_2_ptr = (EXCEPT *)excpt2;

  if (  excpt_1_ptr->priority < excpt_2_ptr->priority )
    return (LIST_LT);
  if ( excpt_1_ptr->priority == excpt_2_ptr->priority )
    return (LIST_EQ);
  return (LIST_GT);
}

static int
fleet_excpt_list_search( call_hndl, excpt )
     CALL_HNDL  call_hndl;
     EXCPT_HNDL excpt;
{
  EXCEPT *excpt_ptr = (EXCEPT *)excpt;

  if ( call_hndl != (CALL_HNDL)excpt_ptr->call_ptr )
    return (LIST_GT);
  else
    return (LIST_EQ);
}

LinkedList *
Fleet_excpt_list_create()
{
  return((LinkedList *)LL_new());
}				/* end Fleet_excpt_list_create */

EXCPT_HNDL
Fleet_excpt_list_get(LinkedList *fleet_excpt_list, short get_indx)
{
  switch ( get_indx )
    {
    case LIST_FIRST:
      return( (EXCPT_HNDL)LL_GetFirst( fleet_excpt_list ) );
      break;

    case LIST_NEXT:
      return( (EXCPT_HNDL)LL_GetNext( fleet_excpt_list ) );
      break;

    default:
      return(HNDL_NULL);
    }
  
}				/* end Fleet_excpt_list_get */

RET_STATUS
Fleet_excpt_list_add(fleet_excpt_hndl, excpt_hndl)
   LinkedList *fleet_excpt_hndl;
   EXCPT_HNDL excpt_hndl;
{
  return(LL_PriorityEnqueue( (LinkedList *)fleet_excpt_hndl, (void *)excpt_hndl, fleet_excpt_list_compare ));
}				/* end Fleet_excpt_list_add */

RET_STATUS
Fleet_excpt_list_remove(fleet_excpt_hndl, excpt_hndl)
   LinkedList *fleet_excpt_hndl;
   EXCPT_HNDL excpt_hndl;
{
  EXCPT_HNDL tmp_excpt_hndl = HNDL_NULL;

  if ( ( tmp_excpt_hndl = LL_Remove(fleet_excpt_hndl, (void *)excpt_hndl ) ) != HNDL_NULL )
    return(SUCCESS);
  else
    return(FAIL);
}				/* end Fleet_excpt_list_remove */

EXCPT_HNDL
Fleet_excpt_list_find(
   LinkedList  *fleet_excpt_hndl,
   CALL_HNDL  call_hndl)
{
  EXCEPT     *pExcept;
  
  LL_Rewind( fleet_excpt_hndl );
  while ((pExcept = (EXCEPT *)LL_Get(fleet_excpt_hndl)) != HNDL_NULL)
    {
      if ( call_hndl == (CALL_HNDL)pExcept->call_ptr )
	return ((EXCPT_HNDL)pExcept);      
      if (LL_Next( fleet_excpt_hndl ) != SUCCESS)
	break;
    }

  return(HNDL_NULL);
  
}                              /* end Fleet_veh_nbr_list_find */


/** SUSPENSION LIST PROCESSING **/

LinkedList *
Fleet_susp_veh_list_create()
{
  return((LinkedList *)LL_new());
}				/* end Fleet_susp_veh_list_create */

RET_STATUS
Fleet_susp_veh_list_destroy(fleet_susp_list)
LinkedList *fleet_susp_list;
{
  
  LL_Rewind(fleet_susp_list);
  do {
    LL_DeleteNode(fleet_susp_list);
  } while (LL_Next(fleet_susp_list) == 0);
  return(SUCCESS);
  
}				/* end Fleet_susp_driver_veh_list_create */

RET_STATUS
Fleet_susp_veh_list_add(fleet_susp_list, veh_hndl)
    LinkedList *fleet_susp_list;
   VEH_HNDL veh_hndl;
{
  return(LL_PriorityEnqueue( (LinkedList *)fleet_susp_list, (void *)veh_hndl, fleet_susp_veh_list_compare ));  
}				/* end Fleet_susp_driver_veh_list_add */

RET_STATUS
Fleet_susp_veh_list_remove(fleet_susp_list, veh_hndl)
   LinkedList *fleet_susp_list;
   VEH_HNDL veh_hndl;
{
  VEH_HNDL tmp_veh_hndl = HNDL_NULL;
  
  if ( ( tmp_veh_hndl =LL_Remove( fleet_susp_list, (void *)veh_hndl ) ) != HNDL_NULL )
    return(SUCCESS);
  else
    return(FAIL);  
}				/* end Fleet_susp_veh_list_remove */

   
VEH_HNDL
Fleet_susp_veh_list_get(LinkedList  *fleet_susp_list, short get_indx)
{
  switch (get_indx)
    {
    case LIST_FIRST:
      return( (VEH_HNDL)LL_GetFirst(fleet_susp_list) );
      break;

    case LIST_NEXT:
      return( (VEH_HNDL)LL_GetNext(fleet_susp_list) );
      break;

    default:
      return(HNDL_NULL);
      break;
    }
  
}				/* end Fleet_susp_veh_list_get */

VEH_HNDL
Fleet_susp_veh_list_find(LinkedList *fleet_susp_veh_list, short veh_nbr)
{
  VEH_HNDL tmp_veh_hndl;
  LL_Rewind( fleet_susp_veh_list );
  while ((tmp_veh_hndl = (VEH_HNDL)LL_Get(fleet_susp_veh_list)) != HNDL_NULL)
    {
      if ( veh_nbr == (short)Veh_get_value( tmp_veh_hndl, VEH_NBR ) )
	return (tmp_veh_hndl);
      if (LL_Next( fleet_susp_veh_list ) != SUCCESS)
	break;
    }
  
  return(HNDL_NULL);
  
}

LinkedList *
Fleet_susp_drv_list_create()
{
  return((LinkedList *)LL_new());  
}				/* end Fleet_susp_drv_list_create */

RET_STATUS
Fleet_susp_drv_list_destroy(fleet_susp_list)
LinkedList *fleet_susp_list;
{
  
  LL_Rewind(fleet_susp_list);
  do {
    LL_DeleteNode(fleet_susp_list);
  } while (LL_Next(fleet_susp_list) == 0);
  return(SUCCESS);
  
}		

RET_STATUS
Fleet_susp_drv_list_add(fleet_susp_list, veh_hndl)
   LinkedList *fleet_susp_list;
   VEH_HNDL veh_hndl;
{
  return(LL_PriorityEnqueue( (LinkedList *)fleet_susp_list, (void *)veh_hndl, fleet_susp_drv_list_compare ));    
}		

RET_STATUS
Fleet_susp_drv_list_remove(fleet_susp_list, veh_hndl)
   LinkedList *fleet_susp_list;
   VEH_HNDL veh_hndl;
{
  VEH_HNDL tmp_veh_hndl = HNDL_NULL;
  
  if ( ( tmp_veh_hndl =LL_Remove( fleet_susp_list, (void *)veh_hndl ) ) != HNDL_NULL )
    return(SUCCESS);
  else
    return(FAIL);
  
}				/* end Fleet_susp_veh_list_remove */

   
VEH_HNDL
Fleet_susp_drv_list_get(LinkedList *fleet_susp_list, short get_indx)
{
  switch (get_indx)
    {
    case LIST_FIRST:
      return( (VEH_HNDL)LL_GetFirst(fleet_susp_list) );
      break;

    case LIST_NEXT:
      return( (VEH_HNDL)LL_GetNext(fleet_susp_list) );
      break;

    default:
      return(HNDL_NULL);
      break;
    }
  
}		

VEH_HNDL
Fleet_susp_drv_list_find(fleet_susp_drv_list, driver_id)
   LinkedList *fleet_susp_drv_list;
   int driver_id;
{
  VEH_HNDL tmp_veh_hndl;
  LL_Rewind( fleet_susp_drv_list );

  while ((tmp_veh_hndl = (VEH_HNDL)LL_Get(fleet_susp_drv_list)) != HNDL_NULL)
    {
      if ( driver_id == (int)Veh_get_value( tmp_veh_hndl, VEH_DRV_ID ) )
	return (tmp_veh_hndl);      
      if (LL_Next( fleet_susp_drv_list ) != SUCCESS)
	break;
    }

  return(HNDL_NULL);
  
}                              /* end Fleet_driver_veh_list_find */

static int
pkt_id_list_search( veh_id, pkt_id_hndl )
     short    veh_id;
     PKT_ID_HNDL pkt_id_hndl;
{
  struct pkt_id_struct *pkt_id_ptr = ( struct pkt_id_struct * )pkt_id_hndl;

  if ( veh_id < pkt_id_ptr->veh_nbr )
    return (LIST_LT);
  if ( veh_id == pkt_id_ptr->veh_nbr )
    return (LIST_EQ);
  return (LIST_GT);
}

LinkedList *
Fleet_pkt_id_list_create()
{
  return((LinkedList *)LL_new());  
}

RET_STATUS
Fleet_pkt_id_list_add( pkt_id_hndl )
     PKT_ID_HNDL pkt_id_hndl;
{
  return( LL_InsertNode( fleet_pkt_id_list, (void *)pkt_id_hndl ) );
}

PKT_ID_HNDL
Fleet_pkt_id_list_get( short get_indx )
{
  switch ( get_indx )
    {
    case LIST_FIRST:
      return( (PKT_ID_HNDL)LL_GetFirst( fleet_pkt_id_list ) );
      break;

    case LIST_NEXT:
      return( (PKT_ID_HNDL)LL_GetNext( fleet_pkt_id_list ) );
      break;

    default:
      return(HNDL_NULL);
    }  
}

RET_STATUS
Fleet_pkt_id_list_remove( pkt_id_hndl )
     PKT_ID_HNDL pkt_id_hndl;
{
  PKT_ID_HNDL tmp_pkt_id_hndl = HNDL_NULL;

  if ( ( tmp_pkt_id_hndl = LL_Remove(fleet_pkt_id_list, (void *)pkt_id_hndl ) ) != HNDL_NULL )
    return(SUCCESS);
  else
    return(FAIL);
  
}


LinkedList *
Fleet_big_car_trip_list_create()
{
  return((LinkedList *)LL_new());
}

RET_STATUS
Fleet_big_car_trip_list_add( bct_list_hndl, call_hndl )
  LinkedList *bct_list_hndl;
CALL_HNDL  call_hndl;
{
  return(LL_PriorityEnqueue( (LinkedList *)bct_list_hndl, (void *)call_hndl, bct_list_compare ));  
}

RET_STATUS
Fleet_big_car_trip_list_remove(
                               LinkedList *bct_list_hndl, 
                               CALL_HNDL call_hndl
                               )
{
  CALL_HNDL tmp_call_hndl = HNDL_NULL;

  if ( ( tmp_call_hndl = LL_Remove(bct_list_hndl, (void *)call_hndl ) ) != HNDL_NULL )
    return(SUCCESS);
  else
    return(FAIL);

}

CALL_HNDL
Fleet_big_car_trip_list_find(
                             LinkedList *bct_list_hndl,
                             short nbr
                             )
{
  CALL_HNDL tmp_call_hndl;
  
  LL_Rewind( bct_list_hndl );

  while ((tmp_call_hndl = (CALL_HNDL)LL_Get(bct_list_hndl)) != HNDL_NULL)
    {
      if ( nbr == (int)Call_get_value( tmp_call_hndl, CALL_NBR ) )
	return (tmp_call_hndl);      

      if (LL_Next( bct_list_hndl ) != SUCCESS)
	break;
    }
  
  return (HNDL_NULL);

}
CALL_HNDL
Fleet_big_car_trip_list_get(
                            LinkedList *bct_list_hndl,
                            short get_indx
                            )
{
  switch (get_indx)
    {
    case LIST_FIRST:
      return( (CALL_HNDL)LL_GetFirst(bct_list_hndl) );
      break;

    case LIST_NEXT:
      return( (CALL_HNDL)LL_GetNext(bct_list_hndl) );
      break;

    default:
      return(HNDL_NULL);
      break;
    }
  
}
