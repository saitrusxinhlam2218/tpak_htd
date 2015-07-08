/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call_except.c,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 2004/06/22 17:08:17 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Call_except.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Call_except.c,v 1.4 2004/06/22 17:08:17 jwelch Exp $";

#include <stdio.h>
#include <sys/types.h>
#include <sys/times.h>

#include "taxipak.h"
#include "Object.h"
#include "List_public.h"
#include "List_private.h"
#include "Exception_private.h"
#include "Exception_lists_public.h"
#include "Call_private.h"
#include "fleet.h"
#include "mad_error.h"

extern time_t mads_time;
static char error_buf[80];

RET_STATUS remove_excpt_from_list();
EXCPT_HNDL call_find_excpt( LIST_HANDLE, int);

#define Call_get_excpt_list_value(call, attribute)     ((struct calls *)call)->excpt_list
#define Call_set_excpt_list_value(call, attribute, value) \
			((struct calls *)call)->excpt_list = value

RET_STATUS
Call_add_excpt(call_hndl, type, expiration, notify_func)
   CALL_HNDL call_hndl;
   int   type;
   int   expiration;
   int   (*notify_func) ();
{
   /* COULD PROBEBLY DEFAULD THE NOTIFY_FUNC */
   TIMER_HANDLE timer;
   LIST_HANDLE timer_list;
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
   if ((timer_list = (LIST_HANDLE) Call_get_value(call_hndl, CALL_TIMER_LIST)) ==
       NULL)
   {
      if ((timer_list = (LIST_HANDLE) List_create(NULL)) == HANDLE_NULL)
      {
	 sprintf(error_str, "Error Call_add_timer - no timer list (type = %d)", type);
	 ERROR(' ', "", error_str);
	 return (FAIL);
      }
      Call_set_value(call_hndl, CALL_TIMER_LIST, timer_list);
      //((LIST *)timer_list)->list = NULL;
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
      rc = LL_AddNode(timer_list, (void *)timer);
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
