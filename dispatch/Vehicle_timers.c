/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Vehicle_timers.c,v $
*  @(#)  $Revision: 1.6 $
*  @(#)  $Date: 2004/06/22 17:08:17 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Vehicle_timers.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Vehicle_timers.c,v 1.6 2004/06/22 17:08:17 jwelch Exp $";
#include <stdio.h>
#include <sys/types.h>
#include <sys/times.h>

#include "taxipak.h"
#include "Object.h"
#include "Timer_public.h"
#include "List_public.h"
#include "List_private.h"
#include "dispatch.h"
#include "kdt.h"
#include "veh_driv.h"
#include "Vehicle_timers_public.h"
#include "Call_public.h"
#include "fleet.h"
#include "timers.h"
#include "mad_error.h"

extern time_t mads_time;
static char error_buf[80];

static RET_STATUS remove_timer_from_list();
TIMER_HANDLE veh_find_timer( LIST_HANDLE, int);

#define Veh_timer_list_each_get( timer_list, timer_hndl) \
   { TIMER_HANDLE _next; LL_Rewind( timer_list); \
   for ((timer_hndl) = LL_GetFirst( timer_list ),\
        _next = LL_GetNext( timer_list );\
        (timer_hndl); \
        (timer_hndl) = _next, _next = LL_GetNext(timer_list)) {

#define Veh_timer_list_each_end } LL_Rewind( timer_list ); }

/*--------------------------------------------
 *	timer list functions
 *--------------------------------------------
 */
/*
 *	NOTE
 *	the following macro should be replaced by a
 *		real veh to veh_get_value() when that
 *		gets implemented
 */

#define Veh_get_value(veh, attribute)     ((struct veh_driv *)veh)->timer_list
#define Veh_set_value(veh, attribute, value) \
			((struct veh_driv *)veh)->timer_list = value

/*************************************************************************/
/* RET_STATUS Veh_add_timer(veh_hndl, type, expiration, notify_func)   */
/* OP: create a timer of teh indicated type                              */
/* PRE: Timer_services started                                           */
/*      veh exists                                                      */
/* POST: timer created                                                   */
/* ERROR: cant create timer list if necessary                            */
/*        unable to create timer                                         */
/*************************************************************************/

RET_STATUS
Veh_add_timer(veh_hndl, type, expiration, notify_func)
   VEH_HNDL veh_hndl;
   int   type;
   int   expiration;
   int   (*notify_func) ();
{
   /* COULD PROBEBLY DEFAULD THE NOTIFY_FUNC */
   TIMER_HANDLE timer;
   LIST_HANDLE timer_list;
   int   rc;
   struct veh_driv *veh_ptr;
   char  error_str[80];
   int   bNewTimerList = FALSE;

   if (veh_hndl == HNDL_NULL)
   {
      sprintf(error_str, "Error veh_add_timer - no veh specified (type = %d)", type);
      ERROR(' ', "", error_str);
      return (FAIL);
   }
/*
 *	if veh doesn't have a timer list yet, create one
 *		this strategy needs some more thought after the veh object
 *		has been completed
 *	NOTE this group of code will change when vehs are implemented due
 *		to the veh_get_value and the timer_list dynamivehy created here
 */
   if ((timer_list = (LIST_HANDLE) Veh_get_value(veh_hndl, VEH_TIMER_LIST)) ==
       NULL)
   {
      if ((timer_list = (LIST_HANDLE) LL_new()) == HANDLE_NULL)
      {
	 sprintf(error_str, "Error Veh_add_timer - no timer list (type = %d)", type);
	 ERROR(' ', "", error_str);
	 return (FAIL);
      }
      Veh_set_value(veh_hndl, VEH_TIMER_LIST, timer_list);
      bNewTimerList = TRUE;
   }
   switch (type)
   {
   default:
      timer = (TIMER_HANDLE) Timer_create(
					  TIMER_TYPE, type,
					  TIMER_EXPIRE_TIME, expiration,
					  TIMER_DATA, veh_hndl,
					  TIMER_NOTIFY_FUNC, notify_func,
					  ATTRIBUTE_END);

      if (timer == (TIMER_HANDLE) NULL)
      {
	 sprintf(error_str, "Error veh_add_timer - couldn't create timer (type = %d)",
		 type);
	 ERROR(' ', "", error_str);
	 return (FAIL);
      }
      rc = LL_InsertNode(timer_list, (void *)timer);
   }				/* end case */

   return (rc);
}				/* end Call_add_timer */

/*********************************************************/
/* RET_STATUS Veh_delete_timer(veh_hndl, type)         */
/* OP: deletes a timer of specified type                 */
/* PRE: veh exists                                      */
/* POST: timer of type removed                           */
/* ERROR: unable to remove timer or could not find       */
/*********************************************************/

RET_STATUS
Veh_delete_timer(veh_hndl, type)
   VEH_HNDL veh_hndl;
   int   type;
{
   LIST_HANDLE timer_list;
   char  error_str[80];

   if (veh_hndl == HNDL_NULL)
   {
      return (FAIL);
   }
   if ((timer_list = (LIST_HANDLE) Veh_get_value(veh_hndl, VEH_TIMER_LIST)) == NULL)
   {
      return (FAIL);
   }
   return (remove_timer_from_list(timer_list, type, TRUE));
}

/*--------------------------------------------------
 *	Call remove from  timer list
 *--------------------------------------------------
 */
static RET_STATUS
Veh_remove_from_timer_list(veh, type)
   VEH_HNDL veh;
   int   type;
{
   LIST_HANDLE timer_list;

   if (((struct veh_driv *) veh)->veh_nbr == 0)
   {
      ERROR(' ', "", "Veh not in memory");
      return (FAIL);
   }
   if (veh == NULL)
   {
      sprintf(error_buf, "Error Veh_remove_from_timer_list - no veh (type = %d)",
	      type);
      ERROR(' ', "", error_buf);
      return (FAIL);
   }
   if ((timer_list = (LIST_HANDLE) Veh_get_value(veh, VEH_TIMER_LIST)) ==
       NULL)
   {
      sprintf(error_buf, "Error Veh_remove_from_timer_list - no timer list (type = %d)",
	      type);
      ERROR(' ', "", error_buf);
      return (FAIL);
   }
   return (remove_timer_from_list(timer_list, type, FALSE));
}

/*--------------------------------------------------
 *	remove timer from list
 *--------------------------------------------------
 */
static RET_STATUS
remove_timer_from_list(timer_list, type, delete_timer)
   LIST_HANDLE timer_list;
   int   type;
   short delete_timer;
{
   TIMER_HANDLE timer, prev_timer;


   
   short target_timers_remain = TRUE, timer_found = FALSE;

   prev_timer = HANDLE_NULL;

   if (timer_list == HANDLE_NULL)
   {
      sprintf(error_buf, "Error remove_timer_from_list - no list");
      ERROR(' ', "", error_buf);
      return (FAIL);
   }
   if (type == VEH_ALL_TIMERS)
   {
     Veh_timer_list_each_get( timer_list, timer )
       {
	 Timer_destroy( timer );
	 timer_found = TRUE;
       } Veh_timer_list_each_end
   } else
   {
	 Veh_timer_list_each_get( timer_list, timer )
	   {
	     if ((int) Timer_get_value(timer, TIMER_TYPE) == type)
	       {
		 Timer_destroy(timer);
		 timer_found = TRUE;
	       }
	   } Veh_timer_list_each_end
   }

   if (timer_found)
      return (SUCCESS);
   else
      return (FAIL);
}

/*--------------------------------------------------
 *	Notify functions
 *		these functions might not eventually belong
 *		in this file
 */
/*--------------------------------------------------
 *	call accept notify func T_ACCPT
 *--------------------------------------------------
 */
RET_STATUS
Veh_wait_epost_notify_func(veh)
   VEH_HNDL veh;
{
  struct veh_driv *veh_ptr;
  ZONE_HNDL  hZone;
  CALL_HNDL  tmp_call_hndl;

  veh_ptr = ( struct veh_driv * ) veh;
  if ((veh_ptr != NULL) && (veh_ptr->t_status.e_posted))
    {
      Zone_get_hndl( veh_ptr->zone_num, veh_ptr->fleet_nbr, &hZone );
      Veh_reset_state((VEH_HNDL) veh_ptr, VEH_EPOSTED_STATE, HNDL_NULL, HNDL_NULL);
      send_text_predefined(veh_ptr->mid,LATE_EPOST,veh_ptr);          /* tell driver he is late*/
      tmp_call_hndl = (CALL_HNDL)Veh_get_value((VEH_HNDL)veh_ptr, VEH_HELD_EPOST_CALL);      
      Zone_release_epost( hZone, (VEH_HNDL)veh_ptr );
      if ( tmp_call_hndl != HNDL_NULL )
	{
	  Call_reset_state(tmp_call_hndl, CALL_WAIT_EPOST_STATE, NULL);
	  Zone_check_epost_vehs( hZone, tmp_call_hndl );
	}      
    }
  return (SUCCESS);
}

/* find a timer of a gever type */
TIMER_HANDLE
veh_find_timer(
		LIST_HANDLE timer_list,
		int type)
{
   TIMER_HANDLE timer;
   char  error_str[80];

   if (timer_list == NULL)
      return (HNDL_NULL);

   Veh_timer_list_each_get( timer_list, timer )
     {
       if ((int) Timer_get_value(timer, TIMER_TYPE) == type)
	 return (timer);
     } Veh_timer_list_each_end
	 
   return (HNDL_NULL);
}				/* end veh_find_timer */
