/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call_private.c,v $
*  @(#)  $Revision: 1.35 $
*  @(#)  $Date: 2005/04/21 06:44:14 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Call_private.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Call_private.c,v 1.35 2005/04/21 06:44:14 jwelch Exp $";

#include <sys/types.h>		/* temp */
#include <sys/times.h>		/* temp */
#include "mad_error.h"
#include "taxipak.h"
#include "Object.h"
#include "timers.h"		/* temp */
#include "except.h"		/* temp */
#include "veh_driv.h"
#include "List_public.h"
#include "Call_public.h"
#include "Call_timers_public.h"
#include "Call_lists_public.h"
#include "Call_private.h"
#include "Fleet_lists_public.h"
#include "Zone_public.h"
#include "Zone_private.h"
#include "Zone_lists_public.h"
#include "fleet.h"
#include "language.h"
#include "mad_ipc.h"
#include "itm.h"
#include "enhance.h"
#include "switch_ext.h"
#include "Ilink_private.h"
#include "ui_ipc.h"

extern struct offsets *offset;
extern FLEET *fleet[ ];	/* shared memory fleet structure */

extern time_t mads_time;
extern short debug_state;
extern HNDL call_value();

int   state_debug = TRUE;
#ifdef DEBUG_OO
int   state_names_switch = FALSE;
char *call_state_name[21];

void
init_call_state_names()
{
   call_state_name[CALL_ACCEPT_STATE - STATE_BASE] = "ACCEPT";
   call_state_name[CALL_AVAILABLE_STATE - STATE_BASE] = "AVAILABLE";
   call_state_name[CALL_BID_STATE - STATE_BASE] = "BID";
   call_state_name[CALL_FLAG_REQUEST_STATE - STATE_BASE] = "FLAG_REQUEST";
   call_state_name[CALL_HOLD_STATE - STATE_BASE] = "HOLD";
   call_state_name[CALL_LATE_METER_STATE - STATE_BASE] = "LATE_METER";
   call_state_name[CALL_MULT_CALL_STATE - STATE_BASE] = "MULT_CALL";
   call_state_name[CALL_NOSHOW_STATE - STATE_BASE] = "NOSHOW";
   call_state_name[CALL_OFFERED_STATE - STATE_BASE] = "OFFERED";
   call_state_name[CALL_PICKUP_STATE - STATE_BASE] = "PICKUP";
   call_state_name[CALL_SHORT_METER_STATE - STATE_BASE] = "SHORT_METER";
   call_state_name[CALL_TIMEOUT_STATE - STATE_BASE] = "TIMEOUT";
   call_state_name[CALL_WAIT_EPOST_STATE - STATE_BASE] = "WAIT_EPOST";
   call_state_name[CALL_PERS_ATTR_OVERRIDE - STATE_BASE] = "PERS_ATTR_OVERRIDE";
}				/* end init_call_state_names() */

#endif

struct calls *
call_hndl_to_ptr(call_hndl)
   CALL_HNDL call_hndl;
{
   return ((struct calls *) call_hndl);
}				/* end call_hndl_to_ptr */

static RET_STATUS
call_check_state(call_ptr, state, op)
   struct calls *call_ptr;
   int   state;
   int   op;
{
   char  error_str[80];

   switch (state)
   {
   case CALL_ACCEPT_STATE:
      /* how to remember state */
      break;
   case CALL_AVAILABLE_STATE:
      if (op == STATE_SET)
      {
	 if ((call_ptr->status.sup_hold))
	 {
	    sprintf(error_str, "Attempt to set call %d to AVAIL when SUP_HOLD", call_ptr->call_number);
	    ERROR(' ', "", error_str);
	    return (FAIL);
	 }
	 if (call_ptr->status.available && call_ptr->status.offered)
	 {
	    ERROR(' ', "", "ASSERT ERROR2 CALL_AVAILABLE_STATE");
	    return (FAIL);
	 }
      }
      break;

   case CALL_FLAG_REQUEST_STATE:
      break;
   case CALL_HOLD_STATE:
      break;
   case CALL_LATE_METER_STATE:
      break;
   case CALL_MULT_CALL_STATE:
      break;
   case CALL_NOSHOW_STATE:
      break;
   case CALL_OFFERED_STATE:
      switch (op)
      {
      case STATE_SET:
	 if (call_ptr->status.offered)
	 {
	    if (call_ptr->veh_ptr != NULL)
	       sprintf(error_str, "call %d already offered to %d, cant dispatch",
		       call_ptr->call_number,
		(int) Veh_get_value((VEH_HNDL) call_ptr->veh_ptr, VEH_NBR));
	    else
	    {
	       if (call_ptr->status.accepted == 0)
	       {
		  ERROR(' ', "", "Offer of call already in offer state no veh_ptr");
		  return (SUCCESS);	/* problem but let it go thur - 11
					 * Dec 91 */
	       }
	       sprintf(error_str, "call %d already offered, cant dispatch", call_ptr->call_number);
	    }
	    printf("%s\n", error_str);
	    ERROR(call_ptr->fleet, " ", error_str);
	    return (FAIL);
	 }
	 if (!(call_ptr->status.available))
	 {
	    sprintf(error_str, "call %d not available, cant dispatch", call_ptr->call_number);
	    printf("%s\n", error_str);
	    ERROR(call_ptr->fleet, " ", error_str);
	    return (FAIL);
	 }
	 break;

      default:
	 break;
      }
      break;

   case CALL_PICKUP_STATE:
      break;
   case CALL_SHORT_METER_STATE:
      break;
   case CALL_TIMEOUT_STATE:
#ifdef FOO
      if (call_ptr->status.available != 1)
	 return (FAIL);
#endif
      break;
   case CALL_WAIT_EPOST_STATE:
      break;
   default:
      break;
   }				/* end case */
   return (SUCCESS);
}				/* end call_check_state */


RET_STATUS
call_state(call_hndl, op, state, argument)
   CALL_HNDL call_hndl;
   int   op;
   int   state;
   HNDL  argument;
{
   struct calls *call_ptr;
   CALL_HNDL tmp_call_hndl;
   VEH_HNDL veh_hndl;
   LIST_HNDL tmp_list;
   CALL_REC  *tmp_cl_ptr = NULL;
   struct veh_driv *veh_ptr;
   int   fl_nbr;
   FLEET_HNDL fleet_hndl;
   char  error_str[80];
   int   acpt_time;
   ZONE_HNDL zone_hndl;
   ZONE_VEH_LIST_HNDL veh_list_hndl;
   int   timeout_value;
   int   type;
   int   tmp;
   EXCEPT *except_ptr;
   short fleet_sav;
   int   cl_nbr_sav;
   BOOLEAN found_big_car_trip = FALSE, bCallIlink = FALSE;


   if (call_hndl == HNDL_NULL)
      return (FAIL);

   call_ptr = call_hndl_to_ptr(call_hndl);
   veh_ptr = call_ptr->veh_ptr;
#ifdef DEBUG
   if (offset->prattle >= LEVEL1)
   {
      if (op != GET)
      {
	 if (op == STATE_RESET)
	    sprintf(trace_str, "C#%d st %d op %d arg %x\n",
		    call_ptr->call_number, state, op, argument);
	 else
	 {
	    switch (state)
	    {
	       /* case CALL_HOLD_STATE:  */
	    case CALL_OFFERED_STATE:
	    case CALL_PICKUP_STATE:
	       if (argument != NULL)
	       {
		  sprintf(trace_str, "C#%d st %d op %d v#%d \n",
			  call_ptr->call_number, state, op, ((VEH *) argument)->veh_nbr);
		  break;
	       }
	    default:
	       sprintf(trace_str, "C#%d st %d op %d arg %x\n",
		       call_ptr->call_number, state, op, argument);
	       break;
	    }
	 }
	 TRACE(trace_str);
      }
   }
#endif


   if (call_check_state(call_ptr, state, op) != SUCCESS)
      return (FAIL);
   switch (state)
   {

   case CALL_ACCEPT_STATE:	/* for STATE_SET - call part of d_accept */
/*****************************************************/
/* CALL_ACCEPT_STATE                                 */
/* STATE_SET                                         */
/*   ARG: none                                       */
/*   OP: call has been accepted by vehicle           */
/*   PRE: call in CALL_OFFER_STATE                   */
/*        veh_hndl exists in call                    */
/*        call_exists                                */
/*   POST: call history updated                      */
/*         Zone_remove_call called                   */
/*         delete any call timers                    */
/*         Call_update_counts (DECREMENT_NOW) called */
/*         LATE_METER timer started                  */
/*         if call has CALL_TIMEOUT ||               */
/*                      BID || PERSONAL exception    */
/*             except_post_process called            */
/*  ERROR: illegal state transition                  */
/* STATE_RESET                                       */
/*   ARG: none                                       */
/*   OP: reset state                                 */
/*   PRE: call exists                                */
/*        CALL_ACCEPT_STATE                          */
/*  POST: !CALL_ACCEPT_STATE                         */
/*  ERROR: none                                      */
/*****************************************************/

      switch (op)
      {
      case STATE_SET:
	 if (call_ptr->veh_ptr == NULL)	/* safety */
	 {
	    sprintf(error_str, "No vehicle for call %d accept", call_ptr->call_number);
	    ERROR(' ', "", error_str);
	    return (FAIL);
	 }
	 Call_update_counts((CALL_HNDL) call_ptr, DECREMENT_NOW);
	 /* no timers if manual assign */
	 Call_delete_timer((CALL_HNDL) call_ptr, CALL_ALL_TIMERS);

	 if (fleet[call_ptr->fleet]->late_meter)
	    if (Call_add_timer((CALL_HNDL) call_ptr, T_LATE_METER,
			       fleet[call_ptr->fleet]->late_meter,
			       (int (*)())call_latemeter_notify_func) != SUCCESS)

	      ERROR(' ', "", "add call timer T_LATE_METER");

	 if ( call_ptr->except_ptr != NULL )
	   {
	     type = call_ptr->except_ptr->type;
	     bit_flip(CALL_TIMEOUT, call_ptr, 0);
	     bit_flip(PERSONAL, call_ptr, 0);
	     if (type == CALL_TIMEOUT || type == PERSONAL)
	       {
		 lock_ex(call_ptr, call_ptr->except_ptr, LOCK_REMOVE);
		 except_post_process(call_ptr, 0);
	       }
	   }	     
	 call_ptr->status.accepted = 1;
	 Zone_remove_call(call_ptr->pickup_zone, (CALL_HNDL) call_ptr);
	 call_ptr->accept_time = mads_time;
	 writer_acpt(call_ptr->veh_ptr->veh_nbr, call_ptr->veh_ptr->driver_id, call_ptr->call_number,
		     call_ptr->c_isam_num, 0, call_ptr->time_entered);
	 Call_add_history(call_hndl, (VEH_HNDL) veh_ptr, NONE, CALL_ACCEPT_STATE, 0L, 0);

         // Notify Interlink if this was an order transferred from a remote system
         if ( (short)Call_get_value( call_hndl, CALL_TYPE_ILINK ) == TRUE )
           InterLink_accept_notify( call_hndl );
         
	 return (SUCCESS);

      case STATE_RESET:
	 call_ptr->status.accepted = 0;
	 return (SUCCESS);

      default:
	 return (call_ptr->status.accepted ? SUCCESS : FAIL);
      }				/* end case */

   case CALL_AVAILABLE_STATE:
/**************************************************/
/* CALL_AVAILABLE_STATE                           */
/* STATE_SET                                      */
/*   OP: call available for matching              */
/*   PRE: call exists                             */
/*   POST: ! CALL_OFFERED_STATE                   */
/*         ! CALL_BID_STATE                       */
/*         CALL_AVAILABLE_STATE                   */
/*         no call timers                         */
/*   ERROR: call in illegal state (HOLD)          */
/* STATE_RESET                                    */
/*   OP: call no longer available for dispatching */
/*   PRE:  CALL_AVAILABLE_STATE                   */
/*   POST: ! CALL_AVAILABLE_STATE                 */
/*  ERROR: none                                   */
/**************************************************/

      switch (op)
      {
      case STATE_SET:
	 call_ptr->status.available = 1;
	 call_ptr->status.offered = 0;
	 return (SUCCESS);

      case STATE_RESET:
	 call_ptr->status.available = 0;
	 return (SUCCESS);

      case GET:
	return (call_ptr->status.available ? SUCCESS : FAIL);

      default:
	 break;
      }
      return (FAIL);

   case CALL_FLAG_REQUEST_STATE:
/**********************************************************/
/* CALL_FLAG_REQUEST_STATE                                */
/* STATE_SET                                              */
/*   OP: vehicle has made a flag request                  */
/*   ARG: PENDING or TAKEN                                */
/*   PRE: call exists                                     */
/*        call offerd or accepted                         */
/*   POST: if ARGUMENT == TAKEN                           */
/*            Dispatcher_call_match called                */
/*            if not matched                              */
/*               CALL_CURRENT_LEVEL restored              */
/*               returns FAIL                             */
/*            else                                        */
/*               CALL_CURRENT_LEVEL restored              */
/*               CALL_AVAILABLE set                       */
/*         call history updated                           */
/*   ERROR: illegal state transition                      */
/* STATE_RESET                                            */
/*   OP: flag request APPROVED or DISAPPROVED             */
/*   ARG: APPROVED of DISAPPROVED                         */
/*   PRE: call exists                                     */
/*   POST:                                                */
/*        if argument == APPROVED                         */
/*           if CALL_ACCEPTED_STATE                       */
/*              T_LATE_METER deleted                      */
/*           else                                         */
/*              T_ACCEPT timer deleted                    */
/*           if call not zone                             */
/*              Zone_add_call called                      */
/*              CALL_VEH_HNDL null                        */
/*              CALL_AVAILABLE_STATE                      */
/*              !CALL_OFFERED_STATE                       */
/*              writer_unassign called                    */
/*              Call_update_counts(INCREMENT_NOW) called  */
/*        switch on CALL_EVENT                            */
/*          TIMEOUT_START:                                */
/*            if still within timeout time                */
/*               T_TIMEOUT timer started                  */
/*            else                                        */
/*               CALL_TIMEOUT_STATE called                */
/*          BID_START:                                    */
/*            BID request made                            */
/*            Call_add_history (BID) called               */
/*            CALL_EVENT = BID_COMPLETE                   */
/*          TIMEOUT_COMPLETE:                             */
/*            CALL_TIMEOUT exception made                 */
/*        except_post_process called                      */
/*  ERROR: not in stae                                    */
/**********************************************************/
      switch (op)
      {
      case STATE_SET:
	 veh_hndl = (VEH_HNDL) call_ptr->veh_ptr;
	 if ((short) argument == TAKEN)
	 {
	    tmp = call_ptr->curr_level;
	    call_ptr->curr_level = 1;
	    call_ptr->status.offered = 0;	/* STATE SET *??? */
	    call_ptr->status.available = 1;
	    if (Dispatcher_call_match((CALL_HNDL) call_ptr) == SUCCESS)
	    {
	       call_ptr->curr_level = tmp;
	       Call_add_history(call_hndl, veh_hndl, FLAG_REQ, TAKEN, 0L, 0);
	       return (SUCCESS);
	    } else
	    {
	       call_ptr->curr_level = tmp;
	       call_ptr->status.available = 0;
	       return (FAIL);
	    }
	 }
	 Call_add_history(call_hndl, veh_hndl, FLAG_REQ, 0, 0L, 0);
	 return (SUCCESS);

      case STATE_RESET:
	 if ((short) argument == R_APPROVE)
	 {
	    if (call_ptr->status.accepted)
	       Call_delete_timer((CALL_HNDL) call_ptr, T_LATE_METER);
	    else
	       Call_delete_timer((CALL_HNDL) call_ptr, T_ACCPT);
	    if (!Call_is_zoned((CALL_HNDL) call_ptr))
	       (void) Zone_add_call(call_ptr->pickup_zone, (CALL_HNDL) call_ptr);
	    call_ptr->veh_ptr = NULL;
	    call_ptr->status.available = 1;	/* mark the call as available */
	    call_ptr->status.offered = 0;
	    call_ptr->status.accepted = 0;
	    writer_unassgn(call_ptr->call_number);
	    Call_update_counts((CALL_HNDL) call_ptr, INCREMENT_NOW);
	 }
	 if (fleet[call_ptr->fleet]->zone_ptr[call_ptr->pickup_zone]->avl_call_q_time > 0)
	   tmp = fleet[call_ptr->fleet]->zone_ptr[call_ptr->pickup_zone]->avl_call_q_time;
	 else
	   tmp = fleet[call_ptr->fleet]->unassigned_time;

	 if ( tmp > 0 )
	   {
	     if (Call_add_timer((CALL_HNDL) call_ptr, T_TIMEOUT,
				tmp, (int (*)())call_timeout_notify_func) != SUCCESS)
	       ERROR(' ', "", "add call timer T_TIMEOUT");
	   }

	 except_post_process(call_ptr, FLAG_REQ);	/* Will call
							 * dispatcher_call_match */
	 return (SUCCESS);

      default:
	 return (call_ptr->veh_ptr->t_status.flag ? SUCCESS : FAIL);
      }				/* end case */
      break;

   case CALL_HOLD_STATE:
/************************************************************************/
/* CALL_HOLD_STATE                                                      */
/* STATE_SET                                                            */
/*   OP: call is held for some external action                          */
/*   ARG: veh_hndl                                                      */
/*   PRE: call exists                                                   */
/*        veh_exists                                                    */
/*   POST: SUP_HOLD request made                                        */
/*         Call_add_history (SUP_HOLD) called                           */
/*   ERROR: none                                                        */
/* STATE_RESET                                                          */
/*   OP: process the HOLD_REQUEST response - call portion               */
/*   ARG: R_APPROVE or R_DISAPPROVE                                     */
/*   PRE: call exists                                                   */
/*        if ARG == R_DISAPPROVE                                        */
/*           CALL_PERSONAL_REQUEST_TYPE == ONLY_PERS_REQ                */
/*                         or                                           */
/*           CALL_PERSONAL_REQUEST_TYPE == APR_ONLY                     */
/*   POST: switch ARGUMENT                                              */
/*          APPROVED:                                                   */
/*            if call not zoned                                         */
/*               Zone_add_call called                                   */
/*            CALL_PRESONAL_REQUEST = APPR_ONLY                         */
/*            CALL_PERSONAL_VEHICLE = 0                                 */
/*            except_post_process called (Dispathcer_call_match called) */
/*          DISAPPROVED:                                                */
/*            writer_cancel called                                      */
/*            Call_cancel called                                        */
/*  ERROR: Unable to zone call                                          */
/************************************************************************/
      switch (op)
      {
      case STATE_SET:
	 mk_except(call_ptr, veh_ptr, SUP_HOLD, 0, -1);
	 Call_add_history(call_hndl, (VEH_HNDL) veh_ptr, SUP_HOLD, 1, 0L, 0);
	 return (SUCCESS);

      case STATE_RESET:
	 if ((short) argument == R_APPROVE)
	   {
	     call_ptr->status.sup_hold = 0;
	     except_post_process(call_ptr, SUP_HOLD);
	   }
	 return (SUCCESS);

      case GET:
	 return (call_ptr->status.sup_hold ? SUCCESS : FAIL);

      default:
	 break;
      }
      return (FAIL);

   case CALL_LATE_METER_STATE:	/* WAS do_latemeter */
/***********************************************************/
/* CALL_LATE_METER_STATE                                   */
/* STATE_SET                                               */
/*   ARG: none                                             */
/*   OP: late meter timer notify event                     */
/*   PRE: call exists                                      */
/*   POST: call history updated                            */
/*         fleet late meter statistics incremented         */
/*         VEH_LATE_METER_STATE set                        */
/*         LATE_METER request generated                    */
/*   ERROR: illegal state transition                       */
/* STATE_RESET                                             */
/*   OP: reset state - call customer picked up             */
/*   PRE: call exists                                      */
/*        CALL_LATE_METER_STATE                            */
/*   POST: none                                            */
/*   ERROR: none                                           */
/***********************************************************/
      switch (op)
      {
      case STATE_SET:
	 if ((call_ptr->veh_ptr == NULL))
	 {
	    sprintf(error_str, "Late meter with null veh_ptr, call %d", call_ptr->call_number);
	    ERROR(' ', "", error_str);
	    return (FAIL);
	 }
	 mk_except(call_ptr, (struct veh_driv *) NULL, LATE_METER, 0, -1);
	 ++fleet[call_ptr->fleet]->late_meters;
	 ++fleet[call_ptr->fleet]->now_late_meters;
	 Veh_set_state((VEH_HNDL) call_ptr->veh_ptr, VEH_LATE_METER_STATE, HNDL_NULL);
	 /* NOTE: no special handling for multiple calls */
	 Call_add_history(call_hndl, (VEH_HNDL) call_ptr->veh_ptr, LATE_METER, 0, 0L, 0);
	 return (SUCCESS);

      case STATE_RESET:
	 /* NO ACTION FOR NOW */
	 /*
	  * NOTE fleet now late_meter stats decremented by vehicle - probably
	  * inconsistent
	  */
	 return (SUCCESS);

      default:
	 return ((int) Veh_get_state((VEH_HNDL) call_ptr->veh_ptr, VEH_LATE_METER_STATE) ? SUCCESS : FAIL);
      }				/* end case */

   case CALL_MULT_CALL_STATE:	/* WAS call_ptr->grouped_with field */
/*************************************************************************/
/* CALL_MULT_CALL_STATE                                                  */
/* STATE_SET                                                             */
/*   OP: group calls                                                     */
/*   ARG: none for first                                                 */
/*       first memeber for others                                        */
/*   PRE: no calls                                                       */
/*        if arg == NULL call exists                                     */
/*        if arg == exists call_                                         */
/*                            call_multiples_calls_list exists           */
/*   POST: arg == NULL  - call_multiples_calls_list  created             */
/*                    call_hndl first on list                            */
/*         arg == exists - call_                                         */
/*                         call_hndl added to call_multiples_calls_list  */
/*   ERROR: no call_multiple_calls_list for arg != NULL                  */
/* STATE_RESET (NOT PRESENTLY AVAILABLE IN UI)                           */
/*   OP: remove this call                                                */
/*   PRE: delete from list                                               */
/*        if call_multiple_calls_list empty destroy it                   */
/*        No call_multiple_list_ptr                                      */
/*        Call_initialize called                                         */
/*   ERROR:   not a multiple call                                        */
/*************************************************************************/
      ERROR(' ', "", "CALL_MULT_CALL_STATE not implemented");
      return (FAIL);

   case CALL_NOSHOW_STATE:	/* WAS part of d_no_show  */
/*******************************************************/
/* CALL_NOSHOW_STATE                                   */
/* STATE_SET                                           */
/*   OP: vehicle has requested no show                 */
/*   ARG: none                                         */
/*   PRE: call exists                                  */
/*        vehicle exists                               */
/*   POST: Call_add_history (NO_SHOW) called           */
/*         switch on FLEET_NO_SHOW_ACTION              */
/*            MANUAL: NOSHOW request made              */
/*            SEMI: NOSHOW request made                */
/*            AUTO: Call_cancel called                 */
/*                  fleet stats incremented            */
/*            AUTO2: NOSHOW request made               */
/*                   Zone_remove_call called           */
/*                   fleet stats incremented           */
/*   ERROR: illegal state transition                   */
/* STATE_RESET                                         */
/*   OP: process the NO_SHOW request response          */
/*   ARG: R_APPROVE or R_DISAPPROVE                    */
/*   PRE:  call exists                                 */
/*         vehicle_exists                              */
/*         VEH_NO_SHOW_STATE                           */
/*   POST: VEH_NO_SHOW_STATE rset with ARGUMENT        */
/*         if ARGUMENT == R_APPROVE                    */
/*            fleet_stats updated                      */
/*            writer_cancel called                     */
/*            Call_update_counts (NO_SHOW) called      */
/*            Call_cancel called                       */
/*         if ARGUMENT == R_DISAPROVE                  */
/*            except_post_process called               */
/*   ERROR: none                                       */
/*******************************************************/
      switch (op)
      {
      case STATE_SET:		/* was d_noshow */

	    
	 switch (fleet[call_ptr->fleet]->no_show_action)
	 {
	 case '1':
	 case '2':

	     if (0)
		 {
		     (void)Fleet_get_hndl( BY_NBR, call_ptr->fleet, &fleet_hndl );
		     (void)Zone_get_hndl( call_ptr->pickup_zone, (short)Fleet_get_value( fleet_hndl, FLEET_NBR ), &zone_hndl );
		     if ( zone_hndl != HNDL_NULL )
			 {
			     if ( (mads_time - (time_t)Call_get_value((CALL_HNDL) call_ptr, CALL_ACCEPT_TIME)) >
				  ((time_t)Zone_get_value( zone_hndl, ZONE_NS_SHORT ) * 60) )
				 {
                                   if ( (int)argument == 3000 )
                                     {
                                       // call_ptr->status.call_cancel = 1;	/* NEVER USED */
                                       fleet_sav = call_ptr->fleet;
                                       cl_nbr_sav = call_ptr->call_number;
                                       Call_add_history(call_hndl, (VEH_HNDL) veh_ptr, NO_SHOW,
                                                        (short) fleet[fleet_sav]->no_show_action, 0L, 3000);	    
                                       Call_cancel((CALL_HNDL) call_ptr, HNDL_NULL, FALSE, 3000);
                                       ++fleet[fleet_sav]->no_shows;
                                       writer_noshow_itm( cl_nbr_sav );
                                       return( SUCCESS );
                                     }
                                   
				     mk_except(call_ptr, call_ptr->veh_ptr, NO_SHOW, 0, -1);
				     Call_add_history(call_hndl, (VEH_HNDL) veh_ptr, NO_SHOW,
						      (short) fleet[call_ptr->fleet]->no_show_action, 0L, (int)argument);
				 }
			     else
				 {
				     mk_except(call_ptr, call_ptr->veh_ptr, NS_SHORT, 0, -1);
                                     if ( (int)argument == 3000 )
                                       Call_add_history(call_hndl, (VEH_HNDL) veh_ptr, NS_SHORT,
                                                        (short) fleet[call_ptr->fleet]->no_show_action, 0L, (int)veh_ptr->veh_nbr);
                                     else
                                       Call_add_history(call_hndl, (VEH_HNDL) veh_ptr, NS_SHORT,
                                                        (short) fleet[call_ptr->fleet]->no_show_action, 0L, (int)argument);
				 }
			 }
		     else
			 {
			     mk_except(call_ptr, call_ptr->veh_ptr, NO_SHOW, 0, -1);
			     Call_add_history(call_hndl, (VEH_HNDL) veh_ptr, NO_SHOW,
					      (short) fleet[call_ptr->fleet]->no_show_action, 0L, (int)argument);
			 }
		 }
	     else
		 {
		     mk_except(call_ptr, call_ptr->veh_ptr, NO_SHOW, 0, -1);
		     Call_add_history(call_hndl, (VEH_HNDL) veh_ptr, NO_SHOW,
				      (short) fleet[call_ptr->fleet]->no_show_action, 0L, (int)argument);
		 }
	     break;
	 case '3':
	    call_ptr->status.call_cancel = 1;	/* NEVER USED */
	    Call_cancel((CALL_HNDL) call_ptr, HNDL_NULL, TRUE, 0L);
	    ++fleet[call_ptr->fleet]->no_shows;
	    Call_add_history(call_hndl, (VEH_HNDL) veh_ptr, NO_SHOW,
			     (short) fleet[call_ptr->fleet]->no_show_action, 0L, (int)argument);	    
	    break;
	 case '4':
	    mk_except(call_ptr, call_ptr->veh_ptr, NO_SHOW, 0, -1);
	    Zone_remove_call(call_ptr->pickup_zone, (CALL_HNDL) call_ptr);
	    ++fleet[call_ptr->fleet]->no_shows;
	    Call_add_history(call_hndl, (VEH_HNDL) veh_ptr, NO_SHOW,
			     (short) fleet[call_ptr->fleet]->no_show_action, 0L, (int)argument);	    
	    break;
	 default:
	    ERROR(' ', "", "invalid noshow option");	/* option does not exist */
	    return(SUCCESS);
	 }

	 return (SUCCESS);

      case STATE_RESET:
	 if ((short) argument == R_APPROVE)
	   {
	     /** Stockholm patch - several incidents of trips being cancelled on NO_SHOW approval
	       by the system without user intervention or NO_SHOW messages from car.
	       Seems to be triggered by ACK on offer **/
	     if ( Call_get_state((CALL_HNDL)call_ptr, CALL_NOSHOW_STATE) == FALSE )
	       {
		 sprintf( error_str, "phantom NOSHOW - call %d", call_ptr->call_number );
		 ERROR(' ', "", error_str);
		 return (SUCCESS);
	       }
	   }

	 if ( Veh_get_state((VEH_HNDL)(call_ptr->veh_ptr), VEH_NOSHOW_STATE) == FALSE )
	   {
	     sprintf( error_str, "phantom NOSHOW - call %d", call_ptr->call_number );
	     ERROR( ' ', "", error_str);
	     return (SUCCESS);
	   }
	 
	 Veh_reset_state((VEH_HNDL) call_ptr->veh_ptr, VEH_NOSHOW_STATE, argument, HNDL_NULL);
	 if ((short) argument == R_APPROVE)
	 {
	    if (fleet[call_ptr->fleet]->no_show_action != '4')
	       ++fleet[call_ptr->fleet]->no_shows;

            if (0)
              {
                writer_noshow_mdt(call_ptr->call_number);
                Call_update_counts((CALL_HNDL) call_ptr, INCREMENT_NOW);
                Call_cancel((CALL_HNDL) call_ptr, HNDL_NULL, FALSE, 3000);
              } 
            else
              {
                writer_cancel(call_ptr->call_number);
                Call_update_counts((CALL_HNDL) call_ptr, INCREMENT_NOW);
                Call_cancel((CALL_HNDL) call_ptr, HNDL_NULL, TRUE, 0L);
              }
	    return (SUCCESS);
	 } else
	 {
	    /* put in line ?? */
	    except_post_process(call_ptr, NO_SHOW);
	 }
	 return (SUCCESS);
      default:
	 if (call_ptr->veh_ptr)
	    return ((int) call_ptr->veh_ptr->t_status.noshow ? SUCCESS : FAIL);
	 else
	    return (FAIL);
      }				/* end case */
      break;

      // CBOOK_OFFERED STATE is special state for TSAB to
      // send a special trip offer format
   case CALL_CBOOK_OFFERED_STATE:
      switch (op)
      {
      case STATE_SET:

	 veh_hndl = (VEH_HNDL) argument;
	 fl_nbr = (int) Veh_get_value(veh_hndl, VEH_FLEET_NBR);
	 (void) Fleet_get_hndl(BY_NBR, fl_nbr, &fleet_hndl);
	 call_ptr->veh_ptr = (struct veh_driv *) argument;
	 call_ptr->status.available = 0;
	 call_ptr->status.offered = 1;
	 call_ptr->status.accepted = 0;
	 if (Veh_get_state(veh_hndl, VEH_MDT_WORKING_STATE))
	 {
	   acpt_time = (int) Fleet_get_value(fleet_hndl, FLEET_CALL_ACCEPT_TIME);
	   if (Call_add_timer((CALL_HNDL) call_ptr, T_ACCPT,
			      acpt_time,
			      (int (*)())call_accept_notify_func) != SUCCESS)
	     ERROR(' ', "", "add call timer T_ACCPT");
	   Call_cbook_send_offer(call_hndl, veh_hndl);
	 } else
	 {
	    mk_except(call_ptr, veh_ptr, MANUAL, 0, -1);
	    Call_add_history(call_hndl, veh_hndl, MANUAL, 0, 0L, 0);
	 }
	 return (SUCCESS);
         //no RESET necessary
      default:
        return (call_ptr->status.offered ? SUCCESS : FAIL);
      }				/* end case */
      break;
     
   case CALL_OFFERED_STATE:	/* WAS reset - d_reject  & no_accept */
/*************************************************************************/
/* CALL_OFFERED_STATE                                                    */
/* STATE_SET argument veh_hndl;                                          */
/*   ARG: veh_hndl                                                       */
/*   OP:                                                                 */
/*   PRE: veh_hndl legal                                                 */
/*   POST:                                                               */
/*                                                                       */
/*           d                                                           */
/*        CALL_VEH_HNDL = ARGUMENT                                       */
/*        if VEH_MDT_WORKING_STATE                                       */
/*           T_ACCEPT timer started                                      */
/*           Call_send_offer called                                      */
/*        else                                                           */
/*           MANUAL request generated                                    */
/*   ERROR: call in offered state                                        */
/* STATE_RESET                                                           */
/*   OP: call no accept or reject processing                             */
/*   ARG: call reject penalty                                            */
/*        CALL_NO_ACCEPT_PENALTY                                         */
/*        CALL_NO_ACCEPT_NO_PENALTY                                      */
/*   PRE: veh exists                                                     */
/*        call exists                                                    */
/*   POST:                                                               */
/*         if CALL_EVENT == BID_START                                    */
/*            CALL_EVENT = BID_COMPLETE                                  */
/*            BID request made                                           */
/*            Call_add_history (BID, 0) called                           */
/*                                                                       */
/*         if ARGUMENT == reject                                         */
/*             if ARGUMENT == reject penalty > '0'                       */
/*                Call_add_history (REJECT) called                       */
/*                Veh put on CALL_ERJECT_VEH_LIST                        */
/*                CALL_NBR_REJECTS incremented                           */
/*                if CALL_NBR_REJECTS > FLEET_MAX_REJECT_COUNT           */
/*                   REJECT request made                                 */
/*                   Call_add_history (BID, 1) made                      */
/*             !CALL_OFFERED_STATE                                       */
/*             CALL_VEH_HNDL = HNDL_NULL                                 */
/*             if CALL_ONLY_PERS_REQUEST                                 */
/*                CALL_HOLD_STATE set                                    */
/*             else                                                      */
/*                CALL_AVAILABLE_STATE                                   */
/*                Zone_add_call called                                   */
/*                Dispatcher_call_match called                           */
/*            T_ACCEPT timer deleted                                     */
/*                                                                       */
/*         if ARGUMENT == CALL_NO_ACCEPT...                              */
/*            T_ACCEPT_TIMER deleted                                     */
/*            CALL_VEH_HNDL = HNDL_NULL                                  */
/*            if CALL_ONLY_PERS_REQ                                      */
/*               CALL_HOLD_STATE set                                     */
/*               CALL_PRESONAL_VEH = NULL                                */
/*               CALL_PERSONAL_REQUEST = APPR_ONLY                       */
/*            if CALL_IF_PERS_REQ                                        */
/*               CALL_AVAILABLE_STATE                                    */
/*               Zone_add_call called                                    */
/*               Dispatcher_call_match called                            */
/*            if not a personal call                                     */
/*               CALL_AVAILABLE_STATE                                    */
/*               Dispatcher_call_match called                            */
/*            if VEH_CALL_NBR != CALL_NBR                                */
/*               return                                                  */
/*            Veh added to CALL_REJECT_VEH_LIST                          */
/*            VEH_CALL_OFFERED_STATE (ARGUMENT) reset                    */
/*   ERROR: cannot add call to zone                                      */
/*          cannot delete T_ACCEPT timer (reject only)                   */
/*************************************************************************/
      switch (op)
      {
      case STATE_SET:

	 veh_hndl = (VEH_HNDL) argument;
	 fl_nbr = (int) Veh_get_value(veh_hndl, VEH_FLEET_NBR);
	 (void) Fleet_get_hndl(BY_NBR, fl_nbr, &fleet_hndl);
	 call_ptr->veh_ptr = (struct veh_driv *) argument;
	 call_ptr->status.available = 0;
	 call_ptr->status.offered = 1;
	 call_ptr->status.accepted = 0;
	 if (Veh_get_state(veh_hndl, VEH_MDT_WORKING_STATE))
	 {
	   acpt_time = (int) Fleet_get_value(fleet_hndl, FLEET_CALL_ACCEPT_TIME);
	   if (Call_add_timer((CALL_HNDL) call_ptr, T_ACCPT,
			      acpt_time,
			      (int (*)())call_accept_notify_func) != SUCCESS)
	     ERROR(' ', "", "add call timer T_ACCPT");
	   Call_send_offer(call_hndl, veh_hndl);
	 } else
	 {
	    mk_except(call_ptr, veh_ptr, MANUAL, 0, -1);
	    Call_add_history(call_hndl, veh_hndl, MANUAL, 0, 0L, 0);
	 }
	 return (SUCCESS);

      case STATE_RESET:
	bCallIlink = (BOOLEAN)Call_get_value(call_hndl, CALL_TYPE_ILINK);
	 if ((char) argument >= '0')	/* REJECT not NO_ACCEPT */
	 {
	    veh_ptr = call_ptr->veh_ptr;
	    if ((char) argument != '0')	/* REJECT - PENALTY */
	    {
	       Call_add_history(call_hndl, (VEH_HNDL) veh_ptr, REJECT, 0, 0L, 0);

	       if ((tmp_list = Call_get_value((CALL_HNDL) call_ptr, CALL_REJECT_VEH_LIST)) == HNDL_ERR)
	       {
		  tmp_list = Call_reject_veh_list_create();
		  (void) Call_set_value((CALL_HNDL) call_ptr, CALL_REJECT_VEH_LIST, (HNDL) tmp_list);
	       }
	       /** Fix problem with reaching this point with NULL vehicle pointer **/
	       if (veh_ptr == NULL)
		 return(SUCCESS);
	       Call_reject_veh_list_add(tmp_list, (VEH_HNDL)veh_ptr);
	       tmp = (short) Call_get_value((CALL_HNDL) call_ptr, CALL_NBR_REJECT);
	       (void) Call_increment_stats((CALL_HNDL) call_ptr, CALL_NBR_REJECT);

	       if ((tmp + 1) >= fleet[veh_ptr->fleet_nbr]->max_reject_count)
	       {
		  mk_except(call_ptr, veh_ptr, REJECT, 0, -1);
		  if ((tmp + 1) == fleet[veh_ptr->fleet_nbr]->max_reject_count)
		  {
		     Call_add_history(call_hndl, (VEH_HNDL) call_ptr->veh_ptr, REJECT, 1, 0L, 0);
		  }
	       }
	    }
	    call_ptr->status.offered = 0;
	    call_ptr->veh_ptr = NULL;
	    if (Call_delete_timer((CALL_HNDL) call_ptr, T_ACCPT) != SUCCESS)
	    {
	       sprintf(error_str, "cant delete call %d T_ACCPT timer", call_ptr->call_number);
	       ERROR(' ', "", error_str);
	    }
	    if (call_ptr->personal_request == ONLY_PERS_REQ)
	       mk_except( call_ptr, veh_ptr, PERSONAL, 0, -1 );
	    else if (call_ptr->personal_request == IF_PERS_REQ)
	    {
	       call_ptr->status.available = 1;
	       if (Zone_add_call(call_ptr->pickup_zone, (CALL_HNDL) call_ptr) == FAIL)
	       {
		  call_ptr->pickup_zone = 0;
		  sprintf(error_str, "no_accept - zoned/unzoned call %d",
			  call_ptr->call_number);
		  ERROR(fleet[call_ptr->fleet]->fleet_id, "", error_str);
		  mk_except(call_ptr, (struct veh_driv *) NULL, UNZONED, call_ptr->msg_nbr, -1);
	       } else
	       {
		  (void) Dispatcher_call_match((CALL_HNDL) call_ptr);
	       }
	    } else
	    {
	       call_ptr->status.available = 1;
	       (void) Dispatcher_call_match((CALL_HNDL) call_ptr);
	    }

	 } else
	 {			/* NO_ACCEPT */
	    if ((call_ptr == NULL) || (call_ptr->veh_ptr == NULL))
	       return (FAIL);

	    (void) Call_delete_timer((CALL_HNDL) call_ptr, T_ACCPT);

	    veh_ptr = call_ptr->veh_ptr;
#ifdef DEBUG
	    if (veh_ptr != NULL)
	       if (veh_ptr->t_status.accepted)
		  if ((call_ptr->personal_request != ONLY_PERS_REQ) &&
		      (call_ptr->personal_request != IF_PERS_REQ))
		  {
		     sprintf(error_str, "bad no accept veh %d, pen %d, call %d veh_call %d",
			     veh_ptr->veh_nbr, (int) argument, call_ptr->call_number, veh_ptr->call_nbr);
		     ERROR(' ', "", error_str);
		     return (SUCCESS);	/* FAIL */
		  }
#endif

	    call_ptr->veh_ptr = NULL;
	    call_ptr->status.offered = 0;

	    if (call_ptr->personal_request == ONLY_PERS_REQ)
	    {
	      mk_except(call_ptr, veh_ptr, PERSONAL, 0, -1);
	      Veh_reset_state((VEH_HNDL) veh_ptr, VEH_CALL_OFFER_STATE, argument, (HNDL) call_ptr);
	      return(SUCCESS);
	    } else
	    {
	       call_ptr->status.available = 1;
	       (void) Dispatcher_call_match((CALL_HNDL) call_ptr);
	    }

	    if (veh_ptr->call_nbr != call_ptr->call_number)
	      {
		if ( (call_ptr->call_number == 0) ) //Cancelled interlink trip
		  {
		    // Need to free vehicle 'manually'
		    veh_ptr->t_status.offered = 0;
		    veh_ptr->call_ptr = NULL;
		    veh_ptr->call_nbr = 0;
		    fleet[veh_ptr->fleet_nbr]->no_accepts += 1;
		    veh_ptr->no_accepts += 1;
		  }
	       return (SUCCESS);
	      }

	    if ((tmp_list = Call_get_value((CALL_HNDL) call_ptr, CALL_REJECT_VEH_LIST)) == HNDL_ERR)
	    {
	       tmp_list = Call_reject_veh_list_create();
	       (void) Call_set_value((CALL_HNDL) call_ptr, CALL_REJECT_VEH_LIST, (HNDL) tmp_list);
	    }
	    Call_reject_veh_list_add(tmp_list, (VEH_HNDL)veh_ptr);
	    (void) Call_increment_stats((CALL_HNDL) call_ptr, CALL_NBR_REJECT);
	    return (Veh_reset_state((VEH_HNDL) veh_ptr, VEH_CALL_OFFER_STATE, argument, (HNDL) call_ptr));

	 }
	 return (SUCCESS);

      default:
	 return (call_ptr->status.offered ? SUCCESS : FAIL);
      }				/* end case */
      break;

      /*
       * case CALL_ONLY_PERS_STATE: printf("CALL_ONLY_PERS_STATE not
       * implemented\n"); return(FAIL);
       */

   case CALL_PICKUP_STATE:
/*******************************************************/
/* CALL_PICKUP_STATE                                   */
/* STATE_SET                                           */
/*   OP: customer picked up                            */
/*   ARG: none                                         */
/*   PRE: call exists                                  */
/*        veh_exists                                   */
/*   POST: zone pickup time stats updated              */
/*         zone call pickup stats incremented          */
/*         writer_pickup called                        */
/*         Call_add_history (PICKUP) called            */
/*         if outstanding request                      */
/*            cancel NO_SHOW request                   */
/*            except_post_process called               */
/*            T_LATE_METER timer deleted               */
/*   ERROR: illegal call zone                          */
/* STATE_RESET                                         */
/*   OP: call is complete                              */
/*   ARG: none                                         */
/*   PRE: call exists                                  */
/*        vehicle exists                               */
/*   POST:                                             */
/*          if short meter                             */
/*            CALL_SHORT_METER state set               */
/*         writer_close called                         */
/*         Call_add_history (CALL_CLOSE) called        */
/*         Call_deactivate called                      */
/*   ERROR: illegal state transition                   */
/*******************************************************/
      switch (op)
      {
      case STATE_SET:
	 if (call_ptr == NULL)
	 {
	    sprintf(error_str, "null call_ptr ");
	    ERROR(' ', "", error_str)
	       return (FAIL);
	 }
	 if (call_ptr->veh_ptr == NULL)
	 {
	    if (argument == NULL)
	       sprintf(error_str, "Call %d pickup with no veh, pstatus %d",
		       call_ptr->call_number, call_ptr->status.pickup);
	    else
	       sprintf(error_str, "Call %d pickup no veh, pstatus %d , arg %d",
		       call_ptr->call_number, call_ptr->status.pickup, ((VEH *) argument)->veh_nbr);
	    ERROR(' ', "", error_str);
	    return (FAIL);
	 }
	 if ((call_ptr->pickup_zone < 0) || (call_ptr->pickup_zone >= ZONE_MAX))
	 {
	    sprintf(error_str, "d_pickup:zone<0|>=Z_MAX call #=%d", call_ptr->pickup_zone);
	    ERROR(' ', "", error_str);
	    return (0);
	 }
	 fleet[call_ptr->fleet]->zone_ptr[call_ptr->pickup_zone]->tm_to_pckup += mads_time - call_ptr->time_entered;
	 fleet[call_ptr->fleet]->zone_ptr[call_ptr->pickup_zone]->calls_pckd_up += 1;

	 writer_pckup(call_ptr->veh_ptr->call_nbr,
	              call_ptr->c_isam_num, 
                      Veh_get_value((VEH_HNDL)call_ptr->veh_ptr, VEH_NBR));

	 Call_add_history(call_hndl, (VEH_HNDL) call_ptr->veh_ptr, NONE, CALL_PICKUP_STATE, 0L, 0);
	 call_ptr->status.pickup = 1;
	 if (call_ptr->except_ptr != NULL)
	 {
	    bit_flip(NO_SHOW, call_ptr, 0);
	    if (call_ptr->except_ptr->type == NO_SHOW)
	    {
	       lock_ex(call_ptr, call_ptr->except_ptr, LOCK_REMOVE);
	       except_post_process(call_ptr, 0);
	       /* CALL_NO_SHOW reset ??? */
	    }
	 }	 
	 return (SUCCESS);

      case STATE_RESET:

	 if (call_ptr == NULL)
	 {
	    ERROR(' ', "", "Null call");
	    return (FAIL);
	 }
	 if (call_ptr->veh_ptr == NULL)
	 {
	    sprintf(error_str, "Null vehicle ptr call %d, pstatus %d",
		    call_ptr->call_number, call_ptr->status.pickup);
	    ERROR(' ', "", error_str);
	    return (FAIL);
	 }
	 if ((call_ptr->veh_ptr->call_ptr != call_ptr) &&
	 Veh_get_state((VEH_HNDL) call_ptr->veh_ptr, VEH_MDT_WORKING_STATE))
	 {
	    ERROR(' ', "", "Call veh mismatch");
	    return (FAIL);
	 }
	 if (call_ptr->call_number == 0)
	 {
	    sprintf(error_str, "Attempt to close callnbr 0 - next is %x", call_ptr->next);
	    ERROR(' ', "", error_str);
	 }

	 writer_close(call_ptr->call_number, 0.0);	/* write out the call record */

	 Call_add_history(call_hndl, (VEH_HNDL) call_ptr->veh_ptr, NONE, CALL_CLOSED_STATE, 0L, 0);

	 if ((time_t) Veh_get_value((VEH_HNDL) call_ptr->veh_ptr, VEH_INFO_TIME) +
	     fleet[call_ptr->veh_ptr->fleet_nbr]->short_meter_duration > mads_time)
	 {			/* if a short meter */
	    if (offset->status.ncp && Veh_get_state((VEH_HNDL) call_ptr->veh_ptr, VEH_MDT_WORKING_STATE))
	       (void) Call_set_state(call_hndl, CALL_SHORT_METER_STATE, HNDL_NULL);
	 }

         // Notify Interlink if this was an order transferred from a remote system
         if ( (short)Call_get_value( call_hndl, CALL_TYPE_ILINK ) == TRUE )
           InterLink_complete_notify( call_hndl );
         
	 Call_deactivate((CALL_HNDL) call_ptr);
	 /* call_ptr->status.pickup = 0; */
	 return (SUCCESS);

      default:
	 return ((int) call_ptr->status.pickup ? SUCCESS : FAIL);
      }				/* end case */

   case CALL_SHORT_METER_STATE:
/***********************************************************/
/* CALL_SHORT_METER_STATE                                  */
/* STATE_SET                                               */
/*   ARG: none                                             */
/*   OP: short meter timer notify event                    */
/*   PRE: call exists                                      */
/*   POST: call history updated                            */
/*         fleet short meter statistics incremented        */
/*         VEH_SHORT_METER_STATE set                       */
/*         SHORT_METER request generated                   */
/*   ERROR: illegal state transition                       */
/* STATE_RESET                                             */
/*   OP: reset state - call customer picked up             */
/*   ARG: none                                             */
/*   PRE: call exists                                      */
/*        CALL_SHORT_METER_STATE                           */
/*   POST: none                                            */
/*   ERROR: none                                           */
/***********************************************************/
      switch (op)
      {
      case STATE_SET:
	 if ((call_ptr == NULL) || (call_ptr->veh_ptr == NULL))
	    return (FAIL);

	 Call_add_history(call_hndl, (VEH_HNDL) call_ptr->veh_ptr, SHORT_METER, 0, 0L, 0);
	 mk_except(call_ptr->veh_ptr->call_ptr, call_ptr->veh_ptr, SHORT_METER, 0, -1);
	 ++fleet[call_ptr->fleet]->short_meters;
	 Veh_set_state((VEH_HNDL) call_ptr->veh_ptr, VEH_SHORT_METER_STATE, HNDL_NULL);
	 return (SUCCESS);

      case STATE_RESET:
	 /* NO ACTION FOR NOW */
	 return (SUCCESS);

      default:
	 /* CANT GET */
	 return ((int) FAIL);
      }				/* end case */

   case CALL_TIMEOUT_STATE:	/* WAS do_timeout */
/*************************************************************************/
/* CALL_TIMEOUT_STATE                                                    */
/* STATE_SET                                                             */
/*   OP: timer                                                           */
/*   ARG: none                                                           */
/*   PRE: call exists and is available:                                  */
/*   POST: call history updated                                          */
/*         if CALL_OFFERED_STATE extend timer                            */
/*         if (fleet bid_action == '2' & fleet bid_duration == 0)        */
/*            Set call CALL_AVAILABLE_STATE                              */
/*            Dispatcher_call_match called                               */
/*            new TIMEOUT timer started                                  */
/*         if (fleet bid_action == '2' & fleet bid_duration != 0)        */
/*            Set call to CALL_BID_STATE                                 */
/*         if fleet bid_action != '2')                                   */
/*            make a TIMEOUT request;                                    */
/*   ERROR: illegal state transition                                     */
/* STATE_RESET                                                           */
/*   OP: not used                                                        */
/*   ERROR: if called                                                    */
/*************************************************************************/
      switch (op)
      {
      case STATE_SET:

	 if ( call_ptr->call_number == 0 )
	   return ( SUCCESS );

	 if ( ( (short)Call_get_value( call_hndl, CALL_TYPE_ILINK ) == TRUE ) )
	   {
	     Call_deactivate( call_hndl );
	     
	     sprintf( error_str, "Attempt to TIMEOUT Interlink call #%d",
		      (int)Call_get_value( call_hndl, CALL_NBR ) );
	     ERROR(' ', "", error_str);
	     return(FAIL);
	   }
	 
	 if ( Call_get_state(call_hndl, CALL_ACCEPT_STATE) )
	   {
	     sprintf( error_str, "Attempt to TIMEOUT accepted call #%d",
		     (int)Call_get_value( call_hndl, CALL_NBR ) );
	     ERROR(' ', "", error_str);
	     return(FAIL);
	   }

	 Zone_get_hndl( (short)Call_get_value( call_hndl, CALL_PICKUP_ZONE ),
			(short)Call_get_value( call_hndl, CALL_FLEET_NBR ), &zone_hndl );
	 if ( zone_hndl == HNDL_NULL )
	   {
	     sprintf( error_str, "Null zone_hndl for CALL_TIMEOUT_STATE call %d",
		      (int)Call_get_value( call_hndl, CALL_NBR ) );
	     ERROR(' ', "", error_str);
	   }
	 
	 // don't timeout a trip held for a valid STC taxi
	 if ( Call_get_state( call_hndl, CALL_WAIT_EPOST_STATE ) )
	   {

	     veh_list_hndl = (ZONE_VEH_LIST_HNDL) Zone_get_value( zone_hndl, ZONE_AVAIL_VEH_LIST );

	     if ( veh_list_hndl == HNDL_NULL )
	       {
		 sprintf( error_str, "Null veh_list_hndl for CALL_WAIT_EPOST_STATE call %d",
			  (int)Call_get_value( call_hndl, CALL_NBR ) );
		 ERROR(' ', "", error_str);
	       }
	     
	     Zone_veh_list_each_get( veh_list_hndl, veh_hndl )
	       {
		 if ( ( Veh_get_state( veh_hndl, VEH_EPOSTED_STATE ) ) &&
		      ( Veh_get_value( veh_hndl, VEH_HELD_EPOST_CALL ) == call_hndl ) )
		   {
		     //valid STC veh found for the held STC call
		     //add a timer event for checking in the future
		     if ( (short)Zone_get_value( zone_hndl, ZONE_TIMEOUT_TIME ) > 0 )
		       timeout_value = (int)Zone_get_value( zone_hndl, ZONE_TIMEOUT_TIME );
		     else
		       timeout_value = (int)fleet[call_ptr->fleet]->unassigned_time;
		     if ( timeout_value > 0 )
		       {
			 if (Call_add_timer((CALL_HNDL) call_ptr, T_TIMEOUT,
					    timeout_value,
					    (int (*)())call_timeout_notify_func) != SUCCESS)
			   ERROR(' ', "", "add call timer T_TIMEOUT");
		       }		     
		     return(FAIL);
		   }
	       }
	     Zone_veh_list_each_end(veh_list_hndl)
	    // falls through when the call is being held for a STC veh erroneously
	    // should we release the call here??
	   }
	 
	 Call_add_history(call_hndl, HNDL_NULL, CALL_TIMEOUT, 0, 0L, 0);


         // Stockholm adds trip to big car trip list if trip has
         // attr3,attr4,attr5 vehicle attribute set
         if ( ( ( call_ptr->vehicle_attributes.attr3 ) || ( call_ptr->vehicle_attributes.attr4 ) ||
                ( call_ptr->vehicle_attributes.attr5 ) ) && (0) )
           {
             found_big_car_trip = FALSE;
             Fleet_big_car_trip_list_each_get( (FLEET_BIG_CAR_TRIP_LIST_HNDL)fleet[call_ptr->fleet]->big_car_trip_list_hndl, tmp_call_hndl )
               {
                 if ( tmp_call_hndl == call_hndl )
                   found_big_car_trip = TRUE;
                 break;
               } Fleet_big_car_trip_list_each_end((FLEET_BIG_CAR_TRIP_LIST_HNDL)fleet[call_ptr->fleet]->big_car_trip_list_hndl)

               if ( found_big_car_trip == FALSE )
                 Fleet_big_car_trip_list_add(
                                             (FLEET_BIG_CAR_TRIP_LIST_HNDL)fleet[call_ptr->fleet]->big_car_trip_list_hndl,
                                             call_hndl
                                             );
           }

	 call_ptr->event = TIMEOUT_COMPLETE;
	 call_ptr->event_time = mads_time;

	 mk_except(call_ptr, (struct veh_driv *) NULL, CALL_TIMEOUT, 0, -1);

	 (void) Fleet_get_hndl(BY_NBR, (short)Call_get_value( call_hndl, CALL_FLEET_NBR ), &fleet_hndl);
	 Zone_maint( 0, (short)Zone_get_value( zone_hndl, ZONE_NBR ), (char)Fleet_get_value( fleet_hndl, FLEET_ID ) );
	 
	 return (SUCCESS);
	 
      case STATE_RESET:
	 {
	   if (!Call_get_state((CALL_HNDL)call_ptr, CALL_OFFERED_STATE))
	     {
	       call_ptr->status.available = 1;
	       call_ptr->status.offered = 0;
	       call_ptr->status.accepted = 0;
	       Dispatcher_call_match((CALL_HNDL) call_ptr );
	     }


	   (void)Zone_get_hndl( (short)Call_get_value((CALL_HNDL)call_ptr, CALL_PICKUP_ZONE),
			        (short) Call_get_value((CALL_HNDL)call_ptr, CALL_FLEET_NBR),
			        &zone_hndl );
	   if ( (short)Zone_get_value( zone_hndl, ZONE_TIMEOUT_TIME ) > 0 )
	     timeout_value = (int)Zone_get_value( zone_hndl, ZONE_TIMEOUT_TIME );
	   else
	     timeout_value = (int)fleet[call_ptr->fleet]->unassigned_time;
	   
	   if ( timeout_value > 0 )
	     {
	       if (Call_add_timer((CALL_HNDL) call_ptr, T_TIMEOUT,
				  timeout_value,
				  (int (*)())call_timeout_notify_func) != SUCCESS)
		 ERROR(' ', "", "add call timer T_TIMEOUT");
	     }

	 }
	 return (SUCCESS);
      default:
	 switch (call_ptr->event)
	 {
	 case TIMEOUT_COMPLETE:
	    return (SUCCESS);
	 case TIMEOUT_START:
	 default:
	    return (FAIL);
	 }
      }				/* end case */
      break;

   case CALL_WAIT_EPOST_STATE:
/*******************************************************/
/* CALL_WAIT_EPOST_STATE                               */
/* STATE_SET                                           */
/*   OP: epost state set                               */
/*   ARG: none                                         */
/*   PRE: call exists                                  */
/*        zone exists                                  */
/*   POST: WAIT_EPOST set                              */
/*         Zone epsost stats incremented               */
/*   ERROR: illegal state transition                   */
/* STATE_RESET                                         */
/*   OP: epost state released                          */
/*   ARG: none                                         */
/*   PRE: in state                                     */
/*        call exists                                  */
/*        zone exists                                  */
/*   POST: WAIT_EPOST reset                            */
/*         Zone epost stats decremented                */
/*   ERROR: not in state                               */
/*******************************************************/

      switch (op)
      {
      case STATE_SET:
	 (void) Zone_get_hndl(call_ptr->pickup_zone, call_ptr->fleet, &zone_hndl);
         //	 Zone_increment_stats(zone_hndl, ZONE_NBR_CALLS_WAIT_EPOST);
	 call_ptr->status.wait_epost = 1;
         // clear the TIMEOUT exception if set
         if ( call_ptr->except_ptr != NULL )
           {
             type = call_ptr->except_ptr->type;
             bit_flip( CALL_TIMEOUT, call_ptr, 0 );
             if ( type == CALL_TIMEOUT )
               {
                 lock_ex( call_ptr, call_ptr->except_ptr, LOCK_REMOVE );
                 except_post_process( call_ptr, 0 );
               }
           }
	 return (SUCCESS);

      case STATE_RESET:
	 (void) Zone_get_hndl(call_ptr->pickup_zone, call_ptr->fleet, &zone_hndl);
         //	 Zone_decrement_stats(zone_hndl, ZONE_NBR_CALLS_WAIT_EPOST);
	 call_ptr->status.wait_epost = 0;
	 return (SUCCESS);
      default:
	 return ((int) call_ptr->status.wait_epost ? SUCCESS : FAIL);
      }				/* end case */
      break;

    case CALL_PERS_ATTR_OVERRIDE:
      switch (op)
	{
	case STATE_SET:
	  call_ptr->status.pers_attr_override = 1;
	  return (SUCCESS);

	case STATE_RESET:
	  call_ptr->status.pers_attr_override = 0;
	  return (SUCCESS);

	default:
	  return ((int) call_ptr->status.pers_attr_override ? SUCCESS : FAIL );
	}
      break;

   default:
      printf("ILLEGAL STATE FOR call_state %d\n", state);
      sprintf(error_str, "ILLEGAL STATE FOR call_state %d\n", state);
      ERROR(' ', "", error_str);
      return (FAIL);
   }				/* end case */
   return (FAIL);
}				/* end call_state */

RET_STATUS
Call_set_state(call_hndl, state, argument)
   CALL_HNDL call_hndl;
   int   state;
   HNDL  argument;
{
  return ((RET_STATUS) call_state(call_hndl, STATE_SET, state, argument));
}				/* end Call_set_state */

RET_STATUS
Call_reset_state(call_hndl, state, argument)
   CALL_HNDL call_hndl;
   int   state;
   HNDL  argument;
{
      return ((RET_STATUS) call_state(call_hndl, STATE_RESET, state, argument));
}				/* end Call_reset_state */

void
call_fill_bits(type_bits, extended_type, len)	/* WAS fill_call_bits */
   struct call_types *type_bits;
   char  extended_type[];
   short len;
{
   register i;

   for (i = 0; i < len; i++)
   {
      if (extended_type[i] == '\0')	/* no more valid type chars */
	 break;
      switch (extended_type[i])
      {
      case TIMEC:
	 type_bits->time = 1;
	 break;
      case SUBS:
	 type_bits->subscription = 1;
	 type_bits->time = 1;
	 break;
      case CHARGE:
	 type_bits->charge = 1;
	 break;
      case WAKEUP:
	 type_bits->wakeup = 1;
	 break;
      case PASS:
	 type_bits->passenger = 1;
	 break;
      case DELIV:
	 type_bits->delivery = 1;
	 break;
      case TYPEX:
	 type_bits->xtype = 1;
	 break;
      case MULT_CALL:
	 type_bits->multiple = 1;
	 break;
      }
   }
}				/* end call_fill_bits */

RET_STATUS
Call_set_value(call_hndl, attr, hndl)
   CALL_HNDL call_hndl;
   int   attr;
   HNDL  hndl;
{
   HNDL  status;

   if ((attr >= STATS_START) && (attr <= STATS_END))
   {
      printf("ERROR - attempt to set  call value %d that is a stat \n", attr);
      return (FAIL);		/* cant set a stat */
   }
   if (call_hndl == (CALL_HNDL) NULL)
      return (FAIL);
   status = call_value(call_hndl, PUT, attr, hndl);
   return ((short) status);
}				/* end Call_set_value */

RET_STATUS
Call_increment_stats(call_hndl, attr)
   CALL_HNDL call_hndl;
   int   attr;
{
   HNDL  status;

   if ((attr >= STATS_START) && (attr <= STATS_END))
   {
      status = call_value(call_hndl, INCREMENT, attr, NULL);
      if (status != HNDL_ERR)
	 return (SUCCESS);
   }
   return (FAIL);
}				/* end call_increment_stats */

RET_STATUS
Call_decrement_stats(call_hndl, attr)
   CALL_HNDL call_hndl;
   int   attr;
{
   HNDL  status;

   if ((attr >= STATS_START) && (attr <= STATS_END))
   {
      status = call_value(call_hndl, DECREMENT, attr, NULL);
      if (status != HNDL_ERR)
	 return (SUCCESS);
   }
   return (FAIL);
}				/* end call_decrement_stats */

RET_STATUS
Call_initialize_stats(call_hndl, attr)
   CALL_HNDL call_hndl;
   int   attr;
{
   HNDL  status;

   status = call_value(call_hndl, PUT, attr, 0);	/* MAY WANT INITIALIZE
							 * dir */
   if (status == HNDL_ERR)
      return (FAIL);
   return (SUCCESS);
}				/* end call_decrement_stats */

/* calld from do_warm - assuming call is closed */
RET_STATUS
Call_warm_start_call_close(call_rec_ptr, veh_ptr)
   CALL_REC *call_rec_ptr;
   VEH  *veh_ptr;
{
   CALL  call;

   memset(&call, '\0', sizeof(struct calls));
   call.call_number = call_rec_ptr->nbr;
   call.fleet = Fleet_get_nbr(call_rec_ptr->fleet);


   Call_add_history((CALL_HNDL) & call, (VEH_HNDL) veh_ptr, NONE, CALL_CLOSED_STATE, 0L, 0);
   return (SUCCESS);
}				/* Call_warm_start_call_close */
