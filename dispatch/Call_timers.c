/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call_timers.c,v $
*  @(#)  $Revision: 1.15 $
*  @(#)  $Date: 2004/06/22 17:08:17 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Call_timers.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Call_timers.c,v 1.15 2004/06/22 17:08:17 jwelch Exp $";

#include <stdio.h>
#include <sys/types.h>
#include <sys/times.h>

#include "taxipak.h"
#include "Object.h"
#include "Timer_public.h"
#include "List_public.h"
#include "List_private.h"
#include "Call_public.h"
#include "Call_timers_public.h"
#include "Call_private.h"
#include "fleet.h"
#include "timers.h"
#include "mad_error.h"
#include "msg_mmp.h"
#include "Fleet_lists_public.h"
#include "Vehicle_public.h"
#include "Packet_ID_public.h"
#include "Packet_ID_private.h"
#include "enhance.h"
#include "switch_ext.h"
#include "Fleet_public.h"
#include "except.h"
#include "writer.h"
#include "Call_hist_db.h"
#include "Driver_public.h"
#include "Ilink_private.h"
#include "ui_ipc.h"

extern int ilink_pipe_fd;
extern time_t mads_time;
static char error_buf[80];

static RET_STATUS remove_timer_from_list();
TIMER_HANDLE call_find_timer( LIST_HANDLE, int);

#define Call_timer_list_each_get( timer_list, timer_hndl) \
   { TIMER_HANDLE _next; LL_Rewind( timer_list); \
   for ((timer_hndl) = LL_GetFirst( timer_list ),\
        _next = LL_GetNext( timer_list );\
        (timer_hndl); \
        (timer_hndl) = _next, _next = LL_GetNext(timer_list)) {

#define Call_timer_list_each_end } LL_Rewind(timer_list); }

/*--------------------------------------------
 *	timer list functions
 *--------------------------------------------
 */


/*************************************************************************/
/* RET_STATUS Call_add_timer(call_hndl, type, expiration, notify_func)   */
/* OP: create a timer of teh indicated type                              */
/* PRE: Timer_services started                                           */
/*      call exists                                                      */
/* POST: timer created                                                   */
/* ERROR: cant create timer list if necessary                            */
/*        unable to create timer                                         */
/*************************************************************************/

RET_STATUS
Call_add_timer(call_hndl, type, expiration, notify_func)
   CALL_HNDL call_hndl;
   int   type;
   int   expiration;
   int   (*notify_func) ();
{
   /* COULD PROBEBLY DEFAULD THE NOTIFY_FUNC */
   TIMER_HANDLE timer;
   CALL_TIMER_LIST_HNDL timer_list;
   int   rc;
   struct calls *call_ptr;
   char  error_str[80];
   int   bNewTimerList = FALSE;

   if (call_hndl == HNDL_NULL)
   {
      sprintf(error_str, "Error Call_add_timer - no call specified (type = %d)", type);
      ERROR(' ', "", error_str);
      return (FAIL);
   }
/*
 *	if call doesn't have a timer list yet, create one
 *		this strategy needs some more thought after the call object
 *		has been completed
 *	NOTE this group of code will change when calls are implemented due
 *		to the Call_get_value and the timer_list dynamically created here
 */
   if ((timer_list = (CALL_TIMER_LIST_HNDL) Call_get_value(call_hndl, CALL_TIMER_LIST)) ==
       HNDL_NULL)
   {
     if ((timer_list = (CALL_TIMER_LIST_HNDL) LL_new()) == HNDL_NULL)
      {
	 sprintf(error_str, "Error Call_add_timer - no timer list (type = %d)", type);
	 ERROR(' ', "", error_str);
	 return (FAIL);
      }
      Call_set_value(call_hndl, CALL_TIMER_LIST, timer_list);
      //      ((LIST *)timer_list)->list = NULL;
      bNewTimerList = TRUE;
   }
   switch (type)
   {
   case T_TIMEOUT:
   case T_ZONE_TIMEOUT:
      /* dont want more than one */
     /* No need to look for existing timer if we just initialized the timer list */
     if (bNewTimerList == FALSE)
	 if (call_find_timer(timer_list, type) != (TIMER_HNDL) HNDL_NULL)
	   return (FALSE);

   default:
      timer = (TIMER_HANDLE) Timer_create(
					  TIMER_TYPE, type,
					  TIMER_EXPIRE_TIME, expiration,
					  TIMER_DATA, call_hndl,
					  TIMER_NOTIFY_FUNC, notify_func,
					  ATTRIBUTE_END);

      if (timer == (TIMER_HANDLE) NULL)
      {
	 sprintf(error_str, "Error Call_add_timer - couldn't create timer (type = %d)",
		 type);
	 ERROR(' ', "", error_str);
	 return (FAIL);
      }
      rc = LL_InsertNode((LinkedList *)timer_list, timer);
   }				/* end case */

   if (type == T_TIMEOUT)
   {
      call_ptr = (struct calls *) call_hndl;
      call_ptr->event_time = mads_time;
      call_ptr->event = TIMEOUT_START;
      return (rc);
   }
   return (rc);
}				/* end Call_add_timer */

/*********************************************************/
/* RET_STATUS Call_delete_timer(call_hndl, type)         */
/* OP: deletes a timer of specified type                 */
/* PRE: call exists                                      */
/* POST: timer of type removed                           */
/* ERROR: unable to remove timer or could not find       */
/*********************************************************/

RET_STATUS
Call_delete_timer(call_hndl, type)
   CALL_HNDL call_hndl;
   int   type;
{
   CALL_TIMER_LIST_HNDL timer_list;
   char  error_str[80];

   if (call_hndl == HNDL_NULL)
     return (FAIL);

   if ((timer_list = (CALL_TIMER_LIST_HNDL) Call_get_value(call_hndl, CALL_TIMER_LIST)) == NULL)
     return (FAIL);

   return (remove_timer_from_list(timer_list, type, TRUE));
}

/*--------------------------------------------------
 *	Call remove from  timer list
 *--------------------------------------------------
 */
static RET_STATUS
Call_remove_from_timer_list(call, type)
   CALL_HNDL call;
   int   type;
{
   CALL_TIMER_LIST_HNDL timer_list;

   if (((struct calls *) call)->call_number == 0)
   {
      ERROR(' ', "", "Call not in memory");
      return (FAIL);
   }
   if (call == NULL)
   {
      sprintf(error_buf, "Error Call_remove_from_timer_list - no call (type = %d)",
	      type);
      ERROR(' ', "", error_buf);
      return (FAIL);
   }
   if ((timer_list = (CALL_TIMER_LIST_HNDL) Call_get_value(call, CALL_TIMER_LIST)) ==
       NULL)
   {
      sprintf(error_buf, "Error Call_remove_from_timer_list - no timer list (type = %d)",
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
   CALL_TIMER_LIST_HNDL timer_list;
   int   type;
   short delete_timer;
{
   TIMER_HANDLE timer, prev_timer;


   
   short target_timers_remain = TRUE, timer_found = FALSE;

   prev_timer = HANDLE_NULL;

   if (timer_list == HNDL_NULL)
   {
      sprintf(error_buf, "Error remove_timer_from_list - no list");
      ERROR(' ', "", error_buf);
      return (FAIL);
   }
   if (type == CALL_ALL_TIMERS)
   {
     Call_timer_list_each_get( timer_list, timer )
       {
	 LL_Remove( timer_list, (void *)timer );
	 timer_found = TRUE;
	 if ( delete_timer )
	   Timer_destroy( timer );
       } Call_timer_list_each_end
   } else
   {
	 Call_timer_list_each_get( timer_list, timer )
	   {
	     if ((int) Timer_get_value(timer, TIMER_TYPE) == type)
	       {
		 LL_Remove( timer_list, (void *)timer );		 
		 timer_found = TRUE;
		 if ( delete_timer )
		   Timer_destroy( timer );	 
	       }
	   } Call_timer_list_each_end
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
call_accept_notify_func(call)
   CALL_HNDL call;
{
  VEH_HNDL   veh_hndl;
  PKT_ID_HNDL pkt_id_hndl;
  PKT_ID_ARRAY_PTR  pkt_id_array_ptr;
  
  if (Call_remove_from_timer_list(call, T_ACCPT) != SUCCESS)
    {
      sprintf(error_buf, "Error call_accept_notify_func - timer not in list");
      ERROR(' ', "", error_buf);
    }

  if ( ( ( (short)Call_get_value( call, CALL_TYPE_ILINK ) == TRUE ) ) )
    (void) Call_increment_stats((CALL_HNDL) call, CALL_NBR_REJECT);
  
  if (0)
    {
      /** check for Stockholm - the ACC can't be relied on to always return a ACK/NAK
	which clears the accept_timer. Put check here to see if there is still a
	pkt_id for this vehicle/call_nbr combination.
	
	If pkt_id exists reset the CALL_OFFERED_STATE but do not penalize the driver
	**/
      veh_hndl = (VEH_HNDL)Call_get_value( call, CALL_VEH_HNDL );
      if ( veh_hndl == HNDL_NULL )
	{
	  sprintf(error_buf, "Error call_accept_notify - NULL veh_hndl");
	  ERROR(' ', "", error_buf);
	  return (FAIL);
	}
      if ( (pkt_id_array_ptr = (PKT_ID_ARRAY_PTR)find_pkt_id_by_veh((int)Veh_get_value( veh_hndl, VEH_NBR ),
								    OFFER_PKT_TYPE)) != NULL )
	{
	  if ( ( pkt_id_array_ptr->type == OFFER_PKT_TYPE ) &&
	      ( pkt_id_array_ptr->nbr == (int)Call_get_value( call, CALL_NBR ) ) &&
	       ( pkt_id_array_ptr->veh_nbr == (int)Veh_get_value( veh_hndl, VEH_NBR ) ) )
	    {
	      Call_reset_state( call, CALL_OFFERED_STATE, (HNDL)0 );
	      return (SUCCESS);
	    }
	}
    }
 
  Call_reset_state(call, CALL_OFFERED_STATE, (HNDL)1);
  return (SUCCESS);

}


RET_STATUS
call_ilink_timeout_notify_func(call)
  CALL_HNDL call;
{
  ILINK_RQST ilink;
  
  if ( ( Call_remove_from_timer_list(call, T_ILINK_TIMEOUT) != SUCCESS ) ||
       ( (int)Call_get_value( call, CALL_NBR ) <= 0 ) )
    {
      return (SUCCESS);
    }
  
  sprintf( ilink.ilink_type, "%03d", ILINK_DISP_TIMEOUT );
  sprintf( ilink.call_nbr, "%010d", (int)Call_get_value( call, CALL_NBR ) );

  if ( write( ilink_pipe_fd, &ilink, sizeof(ILINK_RQST) ) == -1 )
    {
      ERROR(' ',"","Error writing to ILINK pipe");
    }

  Call_cancel( call, (VEH_HNDL)Call_get_value( call, CALL_VEH_HNDL ), TRUE, 0 );

  return (SUCCESS);
  
}  
  
/*--------------------------------------------------
 *	call zone timeout notify func T_ZONE_TIMEOUT
 *--------------------------------------------------
 */
RET_STATUS
call_zone_timeout_notify_func(call)
   CALL_HNDL call;
{
   if (Call_remove_from_timer_list(call, T_ZONE_TIMEOUT) != SUCCESS)
   {
      sprintf(error_buf, "Error call_zone_timeout_notify_func - timer not in list");
      ERROR(' ', "", error_buf);
   }
   
  Dispatcher_call_match( call );
  return (SUCCESS);
}


/*--------------------------------------------------
 *	call timeout notify func T_TIMEOUT
 *--------------------------------------------------
 */
RET_STATUS
call_timeout_notify_func(call_hndl)
   CALL_HNDL call_hndl;
{
  struct calls *call_ptr;
  
  if (Call_remove_from_timer_list(call_hndl, T_TIMEOUT) != SUCCESS)
    {
      sprintf(error_buf, "Error call_timeout_notify_func - timer not in list");
      ERROR(' ', "", error_buf);
    }
  call_ptr = (struct calls *) call_hndl;

  Call_set_state( call_hndl, CALL_TIMEOUT_STATE, HNDL_NULL );

  return (SUCCESS);
}

/*--------------------------------------------------
 *	call latemeter notify func T_LATE_METER
 *--------------------------------------------------
 */
RET_STATUS
call_latemeter_notify_func(call_hndl)
   CALL_HNDL call_hndl;
{
  if (Call_remove_from_timer_list(call_hndl, T_LATE_METER) != SUCCESS)
    {
      sprintf(error_buf, "Error call_latemeter_notify_func - timer not in list");
      ERROR(' ', "", error_buf);
    }

  Call_set_state(call_hndl, CALL_LATE_METER_STATE, HNDL_NULL);

  return( SUCCESS );
}

/******************************************************************************/
/* call_m_action_timer                                                        */
/******************************************************************************/
RET_STATUS
call_m_action_notify_func(call_hndl)
     CALL_HNDL call_hndl;
{
  VEH_HNDL   veh_hndl;
  FLEET_HNDL fleet_hndl;
  CALL_HIST_REC ch_writer_buf;
  
  if ( Call_remove_from_timer_list(call_hndl, T_M_ACTION) != SUCCESS )
    {
      sprintf(error_buf,"Error call_m_action_notify_func - call %d timer not in list",(int)Call_get_value(call_hndl, CALL_NBR));
      ERROR(' ',"", error_buf);
    }

  Fleet_get_hndl( BY_NBR, (short)Call_get_value(call_hndl, CALL_FLEET_NBR), &fleet_hndl );
  
  if ( (veh_hndl = (VEH_HNDL)Call_get_value( call_hndl, CALL_VEH_HNDL ) ) != HNDL_NULL )
    {
      Call_msg_reset_m_action_ack( veh_hndl );
      if ( (short)Call_get_value( call_hndl, CALL_M_ACTION_RETRY ) > 0 )
	{
	  Call_msg_send_m_action((CALL_MSG_LIST_HNDL)Call_get_value(call_hndl, CALL_MSG_LIST), veh_hndl);
	  if ( Call_add_timer(call_hndl, T_M_ACTION,
			      (short)Fleet_get_value( fleet_hndl, FLEET_M_ACTION_TIMER ),
			      (int (*)())call_m_action_notify_func) != SUCCESS )
	    {
	      sprintf(error_str, "Could not add T_M_ACTION for call nbr %d",
		      (int)Call_get_value((CALL_HNDL)call_hndl, CALL_NBR));
	      ERROR(' ',"", error_str);
	    }
	  Call_set_value( call_hndl, CALL_M_ACTION_RETRY, (HNDL)((short)Call_get_value(call_hndl, CALL_M_ACTION_RETRY) - 1) );
	}
      else
	{
	  bzero( &ch_writer_buf, sizeof( CALL_HIST_REC ) );
	  ch_writer_buf.call_nbr = (int)Call_get_value(call_hndl, CALL_NBR);
	  ch_writer_buf.fleet = (char)Fleet_get_value( fleet_hndl, FLEET_ID );
	  ch_writer_buf.exception_nbr = ITM_INVOICE_NAK;
	  ch_writer_buf.event_type = EVNT_REG_TYPE;
	  strcpy( ch_writer_buf.event_desc, "TXM BUSY" );
	  ch_writer_buf.veh_nbr = (short)Veh_get_value( veh_hndl, VEH_NBR );
	  ch_writer_buf.drv_id = (int)Driver_get_value( ((DRIVER_HNDL)Veh_get_value(veh_hndl, VEH_DRIVER_HNDL)), DRIVER_ID );
	  ch_writer_buf.user_id = -1;
	  writer_add_hist( &ch_writer_buf );
	  Call_msg_reset_ack( veh_hndl );
	  Call_redispatch( call_hndl, M_ACTION_REDISPATCH );
	}
    }
  return(SUCCESS);
}
						  

/*******************************************************************************/
/* call_extend_timeout_timer -                                                 */
/*      extend the timeout timer to just after the accept timer                */
/*******************************************************************************/
int
call_extend_timeout_timer(call_ptr)	/* was extend_timeout_timer */
   struct calls *call_ptr;	/* Call pointer associated with event */
{
   CALL_TIMER_LIST_HNDL timer_list;
   TIMER_HANDLE timer;
   int   delta_time;
   time_t timer_time;
   char  error_str[80];

   if ((timer_list = (CALL_TIMER_LIST_HNDL) Call_get_value((CALL_HNDL) call_ptr, CALL_TIMER_LIST)) ==
       NULL)
   {
      ERROR(' ', "", "extend call timer T_TIMEOUT - no list");
      return (FALSE);

   }

   Call_timer_list_each_get( timer_list, timer )
     {
       if ((int) Timer_get_value(timer, TIMER_TYPE) == T_ACCPT)
	 {
	   timer_time = (time_t) Timer_get_value(timer, TIMER_EXPIRE_TIME);
	   delta_time = timer_time - mads_time + 5;
	   if (delta_time <= 0)
	     {
	       /* accept also ready to go off */
	       delta_time = 1;
	     }
	   break;
	 }
     } Call_timer_list_each_end

   Call_timer_list_each_get( timer_list, timer )
     {
       if ((int)Timer_get_value(timer, TIMER_TYPE) == T_TIMEOUT)
	 {
	   LL_Remove( timer_list, (void *)timer );	   
	   if (Call_add_timer((CALL_HNDL) call_ptr, T_TIMEOUT,
			      timer_time + 1,
			      (int (*)())call_timeout_notify_func) != SUCCESS)
	     {
	       ERROR(' ', "", "add call timer T_TIMEOUT");
	       return (FALSE);
	     } else
	       return (TRUE);
	   break;
	 }
     } Call_timer_list_each_end
	 

   sprintf(error_str, "Cant Extend timeout for call %d ",
	   call_ptr->call_number);
   ERROR(' ', "", error_str);
   return (FALSE);
}				/* end call_extend_timeout_timer */

/* find a timer of a gever type */
TIMER_HANDLE
call_find_timer(
		LIST_HANDLE timer_list,
		int type)
{
   TIMER_HANDLE timer;
   char  error_str[80];

   if (timer_list == NULL)
      return (HNDL_NULL);

   Call_timer_list_each_get( timer_list, timer )
     {
       if ((int) Timer_get_value(timer, TIMER_TYPE) == type)
	 return (timer);
     } Call_timer_list_each_end

   return (HNDL_NULL);
}				/* end call_find_timer */

