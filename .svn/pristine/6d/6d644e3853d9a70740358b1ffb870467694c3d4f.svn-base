static char sz__FILE__[]="@(#)inactivity_logout.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  inactivity_logout.c; Rel: 10.1.2.1; Get date: 5/11/95 at 10:14:31
* @(#)  Last delta: 12/2/94 at 17:57:17
* @(#)  SCCS File: /taxi/sccs/s.inactivity_logout.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <time.h>

#define   MID_SIZE	5       /*I know this is dangerous but so is the choochoo train
				  effect of including one more header just to get this */
#include "taxipak.h"
#include "Object.h"
#include "List_public.h"
#include "Driver_public.h"
#include "Fleet_public.h"
#include "Fleet_lists_public.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "timers.h"
#include "Fleet_private.h"
#include "Timer_public.h"

#define         MAX(x,y)        ( (x) > (y) ? (x):(y) )   /* define macro for getting max of two values */

/* These variables should become part of the fleet structure and added to fleet 
   maintainence to be adjustable by the user.
*/

extern FLEET *fleet[FLEET_MAX];                /* shared memory fleet structure */


/*******************************************************************
 * get_last_activity_time() 
 * get the most recent time stamp from vehicle structure
 ******************************************************************/
time_t get_last_activity_time(tmp_ptr)
struct veh_driv *tmp_ptr;
{
	time_t max_time;

	max_time = MAX(tmp_ptr->time_info,tmp_ptr->veh_info_time);
	max_time = MAX(max_time,tmp_ptr->on_break_time);
	max_time = MAX(max_time,tmp_ptr->reject_time);
	max_time = MAX(max_time,tmp_ptr->c_post_time);
	max_time = MAX(max_time,tmp_ptr->e_post_time);
	max_time = MAX(max_time,tmp_ptr->inqry_time);
	max_time = MAX(max_time,tmp_ptr->emergency_on_date);
	max_time = MAX(max_time,tmp_ptr->emergency_off_date);
	max_time = MAX(max_time,tmp_ptr->signed_on_date);
	max_time = MAX(max_time,tmp_ptr->last_action_time);
        max_time = MAX(max_time,tmp_ptr->time_info);
	return(max_time);
}


/*******************************************************************
 * inactivity_logout()
 * sign off vehicles that are signed in and have done nothing 
 * for <max_duration_without_activity> time.  Check all vehicles
 * every <duration_before_checking>.
 *********************************************************************/

inactivity_logout(fleet_nbr)
int fleet_nbr;
{
        register int i;
	struct veh_driv *tmp_ptr;
	time_t last_time;	
	time_t time_now = time(0);
	VEH_HNDL veh_hndl;
	FLEET_DRIVER_VEH_LIST_HNDL fleet_driver_veh_list;
	FLEET_HNDL fleet_hndl;
	
	if (fleet[fleet_nbr]->inactive_time_out_period > 0)
	  {
	    (void) Fleet_get_hndl(BY_NBR, fleet_nbr, &fleet_hndl);
	    fleet_driver_veh_list = (FLEET_DRIVER_VEH_LIST_HNDL) Fleet_get_value(fleet_hndl, FLEET_DRIVER_VEH_LIST);
	    Fleet_driver_veh_list_each_get(fleet_driver_veh_list, veh_hndl)
	      {
		tmp_ptr = (struct veh_driv *) veh_hndl;
		if (last_time = get_last_activity_time(tmp_ptr))
		  if (last_time < (time_now - fleet[fleet_nbr]->inactive_time_out_period))
		    if (SIGNED_ON(tmp_ptr))
		      {
			(void) Driver_reset_state((DRIVER_HNDL) tmp_ptr, DRIVER_SIGN_IN_STATE, HNDL_NULL);
			
			/* deauthorize is now done inside of Veh_reset_state() */
			Veh_reset_state((VEH_HNDL) tmp_ptr, VEH_SIGN_IN_STATE, HNDL_NULL, HNDL_NULL);
		      }
	      }
	    Fleet_driver_veh_list_each_end(fleet_driver_veh_list)
	  }
	
	if (fleet[fleet_nbr]->inactive_retry_interval > 0)
	  Timer_create( TIMER_TYPE, T_INACTIVITY,
		       TIMER_EXPIRE_TIME, fleet[fleet_nbr]->inactive_retry_interval,
		       TIMER_DATA, fleet_nbr,
		       TIMER_NOTIFY_FUNC, inactivity_logout,
		       ATTRIBUTE_END );

}
