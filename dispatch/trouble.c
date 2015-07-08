/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: trouble.c,v $
*  @(#)  $Revision: 1.21 $
*  @(#)  $Date: 2004/06/22 17:08:17 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/trouble.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: trouble.c,v 1.21 2004/06/22 17:08:17 jwelch Exp $";

#include <stdio.h>
#include <sys/types.h>
#include <memory.h>
#include <string.h>
#include "dispatch_strdefs.h"
#include "dispatch.h"
#include "mads_types.h"
#include "sprintf.h"
#include "mads_isam.h"
#include "mad_error.h"
#include "kdt.h"
#include "veh_driv.h"
#include "df_maint.h"
#include "ex_key.h"
#include "except.h"
#include "timers.h"
#include "Timer_public.h"
#include "calls.h"
#include "mad_ipc.h"
#include "language.h"
#include "path.h"
#include "List_public.h"
#include "Zone_lists_public.h"
#include "Call_lists_public.h"
#include "enhance.h"
#include "switch_ext.h"
#include "TimeCalls_public.h"
#include "TimeCalls_private.h"

extern struct offsets *offset;
extern time_t mads_time;
extern char buf [];
extern char req_buf [];
extern struct fleets *fleet[FLEET_MAX];
extern struct veh_index *veh_idx[VEH_DRIV_MAX];
extern char *scratch_ptr;
extern int num_tc;

CALLS_LIST_HNDL calls_list;

void
dump_data()
{
  int  i;
  struct calls *pCall;
  struct zones *pZone;
  char    bufCallType[6];

  for ( i = 1; i < ZONE_MAX; i++ )
    {
      if ( fleet[0]->zone_ptr[i] != NULL && fleet[0]->zone_ptr[i]->avail_call != NULL )
	{
	  pZone = fleet[0]->zone_ptr[i];
	  pCall = pZone->avail_call;
	  printf( "ZN %3d %15s CALLS %3d TAXIS %2d ENROUTE %2d\n", i,
		 pZone->description, pZone->num_calls, pZone->num_taxis, pZone->num_enroute );
	  while ( pCall != NULL )
	    {
	      bufCallType[0] = '\0';
	      if ( pCall->call_type.passenger == 1 )
		strcat( bufCallType, "P" );
	      else if ( pCall->call_type.xtype == 1 )
		strcat( bufCallType, "X" );
	      else if ( pCall->call_type.delivery == 1 )
		strcat( bufCallType, "L" );
	      
	      if ( pCall->call_type.multiple == 1 )
		strcat( bufCallType, "M" );
	      
	      printf( "NBR %8d FL %c TYP %3s PZONE %d ALG_LEV %1d\n",
		     pCall->call_number, pCall->fleet, bufCallType, pCall->pickup_zone, pCall->curr_level );
	      pCall = pCall->next;
	    }
	}
    }
}

zone_rebuild( ZONE_HNDL zone_hndl )
{
  ZONE         *zone_ptr;
  FLEET_HNDL   fleet_hndl;
  CALL_HNDL    call_hndl;
  CALL         *call;
  TC_HNDL      tc_hndl;
  TC_STRUCT    *pTimeCall;
  ZONE_TC_LIST_HNDL tc_list_hndl;
  ZONE_CALLS_LIST_HNDL zone_calls_list_hndl;
  char         fleet_id;
  int          i, done;
  short        zone_nbr = 0, fl_nbr = 0, timeout, zone_list_unassigned = 0, zone_list_timeouts = 0;
  short        call_list_unassigned = 0, call_list_timeouts = 0;
  CALL_REC     callrec;
  time_t       tmp_time, lead_time;

  fl_nbr = (short)Zone_get_value( zone_hndl, ZONE_FLEET_NBR );
  Fleet_get_hndl( BY_NBR, fl_nbr, &fleet_hndl );
  fleet_id = (char)Fleet_get_value( fleet_hndl, FLEET_ID );
  zone_nbr = (short)Zone_get_value( zone_hndl, ZONE_NBR );

  if ( !(0) )
    {
      /* Retabulate the counters for unassigned/timed-out calls */
      zone_ptr = (ZONE *)zone_hndl;
      
      if ( (short)Zone_get_value( zone_hndl, ZONE_TIMEOUT_TIME ) > 0 )
	timeout = (short)Zone_get_value( zone_hndl, ZONE_TIMEOUT_TIME );
      else
	timeout = (short)Fleet_get_value( fleet_hndl, FLEET_UNASSIGNED_TIME );
      
      /* Determine what the zone list says for unassigned and timeouts */
      zone_calls_list_hndl = (ZONE_CALLS_LIST_HNDL) Zone_get_value( zone_hndl, ZONE_AVAIL_CALLS_LIST );
      Zone_calls_list_each_get( zone_calls_list_hndl, call_hndl )
	{
	  ++zone_list_unassigned;
	  if ((mads_time - (time_t) Call_get_value(call_hndl, CALL_TIME_ENTERED)) >= timeout)
	    ++zone_list_timeouts;
	} Zone_calls_list_each_end(zone_calls_list_hndl)
	  
	  /* Determine what the master call list says for unassigned and timeouts */
	  Calls_list_each_get( call_hndl )
	    {
	      if ( ( (short)Call_get_value( call_hndl, CALL_PICKUP_ZONE ) == zone_nbr ) &&
		  ( !Call_get_state( call_hndl, CALL_ACCEPT_STATE) ) )
		{
		  /** one more check for MULT and PERSONAL calls **/
		  call = ( CALL * )call_hndl;
		  if ( call->except_ptr != NULL )
		    {
		      if ( call->except_ptr->type == MULT || call->except_ptr->type == PERSONAL )
			continue;
		    }
		  ++call_list_unassigned;
		  if ((mads_time - (time_t) Call_get_value(call_hndl, CALL_TIME_ENTERED)) >= timeout)
		    ++call_list_timeouts;
		}
	    } Calls_list_each_end
	      
	      /* if our counters don't match, rebuild the zone_calls_list based on master calls list */
	      if ( ( call_list_unassigned == zone_list_unassigned ) &&
		  ( call_list_timeouts == zone_list_timeouts ) )
		zone_ptr->num_calls = call_list_unassigned;
	      else
		{
		  Zone_calls_list_clear( zone_calls_list_hndl );
		  Calls_list_each_get( call_hndl )
		    {
		      if ( ( (short)Call_get_value( call_hndl, CALL_PICKUP_ZONE ) == zone_nbr ) &&
			  ( !Call_get_state( call_hndl, CALL_ACCEPT_STATE) ) )
			Zone_calls_list_add( zone_calls_list_hndl, call_hndl );
		    } Calls_list_each_end
		      zone_ptr->num_calls = call_list_unassigned;
		}
    }

  /* get the zone_hndl for the next timer event */
  zone_hndl = HNDL_NULL;
  for ( i = ++zone_nbr; i < ZONE_MAX - 1; i++ )
    {
      Zone_get_hndl( i, fl_nbr, &zone_hndl );
      if ( zone_hndl != HNDL_NULL )
	break;
    }
  if ( zone_hndl == HNDL_NULL )
    {
      for ( i = 1; i < ZONE_MAX - 1; i++ )
	{
	  Zone_get_hndl( i, fl_nbr, &zone_hndl );
	  if ( zone_hndl != HNDL_NULL )
	    break;
	}
    }
  
  (void) Timer_create(
		      TIMER_TYPE, T_ZONE_REBUILD,
		      TIMER_EXPIRE_TIME, 20,
		      TIMER_NOTIFY_FUNC, zone_rebuild,
		      TIMER_DATA, zone_hndl,
		      ATTRIBUTE_END );


}

  
  
hourly_tasks()
{
  struct excepts *pExcpt, *pNextExcpt;

  pExcpt = offset->first_except;
  while (pExcpt != NULL)
    {
      pNextExcpt = pExcpt->next;
      if ( pExcpt->nbr != 0 )
	{
	  if ( ( ( pExcpt->time_occur + 3600 ) < mads_time ) &&
	      ( pExcpt->inuse == '1' ))
	    lock_ex( NULL, pExcpt, LOCK_REMOVE );
	}
      pExcpt = pNextExcpt;
    }
  
  (void) Timer_create(
		      TIMER_TYPE, T_HOURLY_TASKS,
		      TIMER_EXPIRE_TIME, 3600,
		      TIMER_NOTIFY_FUNC, hourly_tasks,
		      ATTRIBUTE_END);
}
