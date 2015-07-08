/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Zone_ext.c,v $
*  @(#)  $Revision: 1.13 $
*  @(#)  $Date: 2004/08/19 12:54:12 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Zone_ext.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Zone_ext.c,v 1.13 2004/08/19 12:54:12 jwelch Exp $";

#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>

#include "taxipak.h"
#include "Object.h"
#include "mad_ipc.h"
#include "mad_error.h"
#include "acc.h"
#include "except.h"
#include "taxi_db.h"
#include "path.h"
#include "Timer_public.h"
#include "timers.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Call_public.h"
#include "Call_private.h"
#include "Call_db.h"
#include "Driver_public.h"
#include "List_public.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "Zone_lists_public.h"
#include "Zone_public.h"
#include "Zone_private.h"
#include "TimeCalls_private.h"
#include "ui_ipc.h"
#include "language.h"
#include "writer.h"
#include "enhance.h"

extern long glEnhSwitch;
extern time_t mads_time;
extern short time_ticks;
extern struct offsets *offset;
extern int dis_msgqid;
extern FLEET *fleet[ ];	/* temporary */
extern int num_tc;

VEH  *veh_array[500];
static int dec_error( ZONE *, VEH *);

/******************************************************/
/* RET_STATUS Zone_add_call(zone_nbr, call_hndl)      */
/* OP: add a call to this zone                        */
/* PRE: call hndl exists                              */
/*      call is zoned                                 */
/* POST: if call not aready in zone                   */
/*          Call added to list of avail calls in zone */
/*          Zone call stats incremented               */
/* ERROR: Zone hndl does not exist                    */
/******************************************************/

RET_STATUS
Zone_add_call(			/* WAS add_zone_call */
	      short zone_nbr,
	      CALL_HNDL call_hndl)
{
   ZONE_HNDL zone_hndl;
   ZONE_CALLS_LIST_HNDL calls_list_hndl;	/* temproary */
   ZONE *zone;
   char  class;
   CALL *cl_ptr;

   if (Call_is_zoned(call_hndl) == TRUE)
      return (SUCCESS);

   if (Zone_get_hndl(zone_nbr, (short) Call_get_value(call_hndl, CALL_FLEET_NBR), &zone_hndl) == FAIL)
      return (FAIL);
   calls_list_hndl = (ZONE_CALLS_LIST_HNDL) Zone_get_value(zone_hndl, ZONE_AVAIL_CALLS_LIST);
   Zone_calls_list_add(calls_list_hndl, call_hndl);
   Zone_increment_stats(zone_hndl, ZONE_NBR_CALLS);


   return (SUCCESS);
}				/* end Zone_add_call */

/****************************************************/
/* RET_STATUS Zone_remove_call(zone_nbr, call_hndl) */
/* OP: remove a call from this zone                 */
/* PRE: call hndl exists                            */
/*      call has pickup zone                        */
/* POST: if call on zone avail list                 */
/*          Call remoived list of avail  in zone    */
/*          Zone call stats decremented             */
/* ERROR: returns FAIL if not on list               */
/****************************************************/

RET_STATUS
Zone_remove_call(		/* WAS remove_call */
		 short zone_nbr,
		 CALL_HNDL call_hndl)
{
   ZONE_HNDL zone_hndl;
   ZONE *zone;
   ZONE_CALLS_LIST_HNDL calls_list_hndl;	/* temproary */

   int   call_nbr;
   BOOLEAN tc_found = FALSE;

   (void) Zone_get_hndl(zone_nbr, (short) Call_get_value(call_hndl, CALL_FLEET_NBR), &zone_hndl);

   /* NO ERROR CHECKING */

   calls_list_hndl = (ZONE_CALLS_LIST_HNDL) Zone_get_value(zone_hndl, ZONE_AVAIL_CALLS_LIST);
   if (calls_list_hndl == HNDL_ERR)
      return (FAIL);
   if (Zone_calls_list_find(calls_list_hndl, call_hndl) != HNDL_ERR)
   {
      if (Zone_calls_list_remove(calls_list_hndl, call_hndl) == FAIL)	/* not already done */
	 return (FAIL);
   } else
      return (FAIL);
   Zone_decrement_stats(zone_hndl, ZONE_NBR_CALLS);



   return (SUCCESS);
}				/* end Zone_remove_call */

int   granual = 30;

int
get_gran()
{
   if (granual > 0)
      granual--;
   else
      granual = 30;
   return (granual);
}

/********************************************************/
/* RET_STATUS Zone_add_veh(zone_nbr, veh_hndl)          */
/* OP: add a vehicle to the list of available taxis     */
/* PRE: vehicle exists                                  */
/*      zone_nbr != 0                                   */
/* POST: vehicle list created if none                   */
/*       vehicle added to list of veh for zone          */
/*       zone veh/posts  stats incremented              */
/*       fleet post stats devremented                   */
/*             incomplete                               */
/* ERROR:                                               */
/********************************************************/

RET_STATUS
Zone_add_veh(			/* WAS part of add_taxi */
	     short zone_nbr,
	     VEH_HNDL veh_hndl)
{
   ZONE_HNDL zone_hndl;
   ZONE_VEH_LIST_HNDL veh_list_hndl;
   time_t  ptime = 0;
   VEH_HNDL nxt_veh_hndl;
   VEH  *veh_ptr;
   DRIVER_HNDL driver_hndl = (DRIVER_HNDL) veh_hndl;
   BOOLEAN e_posted = FALSE;
   char  error_str[80];
   RET_STATUS Zone_ext_increment_veh_count();
   long     lPosAge;
   time_t   VehGPSTime = 0;
   short   fl_nbr;
   FLEET_HNDL   fl_hndl;
   struct zones *pZone = NULL;
   
   veh_ptr = (VEH *) veh_hndl;

   if (zone_nbr == 0)
   {
      ERROR(' ', "", "Zone_add_veh - zero zone nbr");
   }
   Veh_set_value(veh_hndl, VEH_ZONE_NBR, (HNDL) zone_nbr);	/* POSTED STATE ?? */
   if (Zone_get_hndl(zone_nbr, (short) Veh_get_value(veh_hndl, VEH_FLEET_NBR), &zone_hndl) != SUCCESS)
   {
      ERROR(' ', "", "Zone_add_veh - Zone_get_hndl() failed");
      return (FAIL);
   }
   veh_list_hndl = (ZONE_VEH_LIST_HNDL) Zone_get_value(zone_hndl, ZONE_AVAIL_VEH_LIST);
#ifdef ASSERT
   if (Zone_veh_list_find(veh_list_hndl, veh_hndl) != HNDL_ERR)
   {
      sprintf(error_str, "taxi %d already in zone %d", veh_ptr->veh_nbr, zone_nbr);
      ERROR(' ', "", error_str);
      return (SUCCESS);		/* already there */
   }
#endif

   if (veh_ptr->t_status.e_posted)
   {
      /* PUT IN STATE SET EPOST */
      e_posted = TRUE;
      veh_ptr->e_post_time = mads_time;	/* belongs in vehicle */
      veh_ptr->q_time = mads_time;
      Zone_increment_stats(zone_hndl, ZONE_NBR_ENROUTE);
      ++veh_ptr->enroute_posts;	/* incr driver eposts */
      ++fleet[veh_ptr->fleet_nbr]->now_enroute_posts;	/* incr # drivers now
							 * e_posted */
   } else if (veh_ptr->t_status.posted)
   {
      veh_ptr->time_info = mads_time;	/* belongs in vehicle */
      //  veh_ptr->q_time = (time_ticks++) + (((unsigned long)mads_time) << 8);
      veh_ptr->q_time = mads_time;
      Zone_increment_stats(zone_hndl, ZONE_NBR_VEH);
      ++veh_ptr->posts;		/* incr driver posts */
      ++fleet[veh_ptr->fleet_nbr]->now_posts;	/* incr # drivers now  posted */
   }
   Zone_veh_list_add(veh_list_hndl, veh_hndl);
   veh_hndl = Zone_veh_list_find(veh_list_hndl, veh_hndl);	/* to access the list */
   nxt_veh_hndl = Zone_veh_list_get(veh_list_hndl, ZONE_VEH_LIST_NEXT);
   if (nxt_veh_hndl != (VEH_HNDL) HNDL_ERR)
      ptime = (long) Veh_get_value(nxt_veh_hndl, VEH_WARM_TIME) - get_gran();

   if (!ptime)
      time(&ptime);
   Veh_set_value(veh_hndl, VEH_WARM_TIME, (HNDL) ptime);
   writer_post(veh_ptr->fleet, veh_ptr->veh_nbr, zone_nbr, veh_ptr->q_time, e_posted);

   return (SUCCESS);
}				/* end Zone_add_veh */

static RET_STATUS
zone_veh_list_recover(zone_ptr, veh_list_hndl)
   ZONE *zone_ptr;
   ZONE_VEH_LIST_HNDL veh_list_hndl;
{
   char  error_str[80];
   VEH  *veh_ptr;
   int   jj, ii = 0;
   short zone_nbr;

   if (zone_ptr == NULL)
      return(FAIL);
   if (veh_list_hndl == NULL)
      return(FAIL);
   zone_nbr = zone_ptr->zone_nbr;

   zone_ptr->num_taxis = 0;
   zone_ptr->num_enroute = 0;

   Zone_veh_list_each_get(veh_list_hndl, (VEH_HNDL) veh_ptr)
   {
      if (veh_ptr->zone_num != zone_nbr)
      {
	 sprintf(error_str, "veh %d in zone %d priority %d", veh_ptr->veh_nbr,
		 veh_ptr->zone_num, veh_ptr->q_priority);
	 ERROR(' ', "", error_str);
      } else if (veh_ptr->t_status.posted)
      {
	 veh_array[ii++] = veh_ptr;
         ++zone_ptr->num_taxis;
         
      } else if (veh_ptr->t_status.e_posted)
      {
	 veh_array[ii++] = veh_ptr;
         ++zone_ptr->num_enroute;
      }
   } Zone_veh_list_each_end(veh_list_hndl)


   if(Zone_veh_list_clear(veh_list_hndl) == FAIL)
        return(FAIL);

   for (jj = 0; jj < ii; jj++)
     Zone_veh_list_add(veh_list_hndl, (VEH_HNDL)veh_array[jj]);

   sprintf(error_str," Zone %d veh list rebuilt for %d veh", zone_ptr->zone_nbr, ii);
   ERROR(' ',"",error_str);
   return (SUCCESS);
}				/* end zone_veh_list_recover */

/********************************************************/
/* RET_STATUS Zone_remove_veh(zone_nbr, veh_hndl)       */
/* OP: removea vehicle to the list of available taxis   */
/* PRE: vehicle exists                                  */
/*      zone_nbr != 0                                   */
/* POST: vehicle removed form list of veh for zone      */
/*       vehilce list delete if free                    */
/*       zone veh/posts/epost  stats decremented        */
/*       fleet post stats decremented                   */
/*       if VEH_POSTED_STATE                            */
/*          VEH_POSTED_STATE reset                      */
/*          VEH_IDLE_STATE set                          */
/*       if VEH_EPOSTED_STATE                           */
/*          VEH_EPOSTED_STATE reset arg ??????          */
/*          VEH_IDLE_STATE set                          */
/*       db post info updated                           */
/* ERROR:                                               */
/* update */
/********************************************************/

RET_STATUS
Zone_remove_veh(		/* WAS find_taxi  REMOVE */
		short zone_nbr,
		VEH_HNDL veh_hndl)
{
   struct zones *tmp_zone_ptr;	/* pointer to the zone structure */
   ZONE_VEH_LIST_HNDL veh_list_hndl;
   ZONE_HNDL zone_hndl;
   short fl_nbr;
   VEH  *veh_ptr;
   BOOLEAN e_posted;
   DRIVER_HNDL driver_hndl = (DRIVER_HNDL) veh_hndl;
   char  error_str[80];
   RET_STATUS Zone_ext_decrement_veh_count();


   veh_ptr = (VEH *) veh_hndl;
#ifdef DEBUG
   if (offset->prattle >= LEVEL5)
   {
      sprintf(trace_str, " Zone_remove_veh (Zone_ext.c): veh_ptr = %x veh_nbr = %d \n", veh_ptr, veh_ptr->veh_nbr);
      TRACE(trace_str);
   }
#endif


   if (zone_nbr == 0)
      return (FAIL);		/* MRB new */
   tmp_zone_ptr = fleet[veh_ptr->fleet_nbr]->zone_ptr[zone_nbr];	/* get pointer to zone *
									 * used */

#ifdef DEBUG_OOx
   printf("Veh %d removed from zone %d\n", veh_ptr->veh_nbr, zone_nbr);
#endif
   fl_nbr = veh_ptr->fleet_nbr;
   (void) Zone_get_hndl(veh_ptr->zone_num, fl_nbr, &zone_hndl);
   veh_list_hndl = (ZONE_VEH_LIST_HNDL) Zone_get_value(zone_hndl, ZONE_AVAIL_VEH_LIST);
   if (veh_list_hndl == HNDL_ERR)
   {
     // veh_list list for zone is gone create a new one.
     Zone_set_value(zone_hndl, ZONE_AVAIL_VEH_LIST, (HNDL)Zone_veh_list_create());
     
      if (tmp_zone_ptr->num_taxis != 0)
      {
	 tmp_zone_ptr->num_taxis = 0;	/* make sure count is 0 */
	 ERROR(' ', "", "non zero taxis in zone");
	 return (FAIL);
      }
      
   }
   veh_hndl = HNDL_ERR;

   veh_hndl = Zone_veh_list_find(veh_list_hndl, (VEH_HNDL) veh_ptr);
   if (veh_hndl == (VEH_HNDL) HNDL_ERR)
   {
      sprintf(error_str, "Vehicle %d  not in zone %d\n", veh_ptr->veh_nbr, zone_nbr);
      ERROR(' ', "", error_str);
      return (FAIL);
   }
   veh_ptr = (VEH *) veh_hndl;
   if (Zone_veh_list_remove(veh_list_hndl, veh_hndl) == FAIL)
   {
      sprintf(error_str, "Unable to remove taxi %d prio %d  from zone %d",
	      veh_ptr->veh_nbr, veh_ptr->q_priority, veh_ptr->zone_num);
      ERROR(' ', "", error_str);
      if (zone_veh_list_recover(tmp_zone_ptr, veh_list_hndl) == FAIL)
	 return (FAIL);
   }
   /* DOESNT FOLLOWING BELONG IN VEHICLE */
   if (veh_ptr->t_status.posted)
   {
      Zone_decrement_stats(zone_hndl, ZONE_NBR_VEH);
      if (fleet[veh_ptr->fleet_nbr]->now_posts <= 0)
	 fleet[veh_ptr->fleet_nbr]->now_posts = 0;
      else
	 --fleet[veh_ptr->fleet_nbr]->now_posts;
   }
   if (veh_ptr->t_status.e_posted)
   {
      Zone_decrement_stats(zone_hndl, ZONE_NBR_ENROUTE);
      if (fleet[veh_ptr->fleet_nbr]->now_enroute_posts <= 0)
	 fleet[veh_ptr->fleet_nbr]->now_enroute_posts = 0;
      else
	 --fleet[veh_ptr->fleet_nbr]->now_enroute_posts;
   }
   /* G7 UNIQUE */
   return (SUCCESS);
}				/* end Zone_remove_veh */

/* move the vehicle to the end of the queue */
RET_STATUS
Zone_veh_end_queue(
		short zone_nbr,
		VEH_HNDL veh_hndl)
{
   struct zones *tmp_zone_ptr;	/* pointer to the zone structure */
   ZONE_VEH_LIST_HNDL veh_list_hndl;
   ZONE_HNDL zone_hndl;
   short fl_nbr;
   VEH  *veh_ptr;
   DRIVER_HNDL driver_hndl = (DRIVER_HNDL) veh_hndl;
   char  error_str[80];
   time_t  ptime = 0;
   VEH_HNDL nxt_veh_hndl;

   veh_ptr = (VEH *) veh_hndl;

   if (zone_nbr == 0)
      return (FAIL);		/* MRB new */
   tmp_zone_ptr = fleet[veh_ptr->fleet_nbr]->zone_ptr[zone_nbr];	/* get pointer to zone *
									 * used */

   fl_nbr = veh_ptr->fleet_nbr;
   (void) Zone_get_hndl(veh_ptr->zone_num, fl_nbr, &zone_hndl);
   veh_list_hndl = (ZONE_VEH_LIST_HNDL) Zone_get_value(zone_hndl, ZONE_AVAIL_VEH_LIST);
   if (veh_list_hndl == HNDL_ERR)
   {
      if (tmp_zone_ptr->num_taxis != 0)
      {
	 tmp_zone_ptr->num_taxis = 0;	/* make sure count is 0 */
	 ERROR(' ', "", "non zero taxis in zone");
	 return (FAIL);
      }
   }
   veh_hndl = HNDL_ERR;

   veh_hndl = Zone_veh_list_find(veh_list_hndl, (VEH_HNDL) veh_ptr);
   if (veh_hndl == (VEH_HNDL) HNDL_ERR)
   {
      sprintf(error_str, "Vehicle %d  not in zone %d\n", veh_ptr->veh_nbr, zone_nbr);
      ERROR(' ', "", error_str);
      return (FAIL);
   }
   veh_ptr = (VEH *) veh_hndl;
   if (Zone_veh_list_remove(veh_list_hndl, veh_hndl) == FAIL)
   {
      sprintf(error_str, "Unable to remove taxi %d prio %d  from zone %d",
	      veh_ptr->veh_nbr, veh_ptr->q_priority, veh_ptr->zone_num);
      ERROR(' ', "", error_str);
      if (zone_veh_list_recover(tmp_zone_ptr, veh_list_hndl) == FAIL)
	 return (FAIL);
   }

   if (veh_ptr->t_status.e_posted)
   {
      veh_ptr->e_post_time = mads_time;	/* belongs in vehicle */
      veh_ptr->q_time = (time_ticks++) + (mads_time >> 8);
   } else if (veh_ptr->t_status.posted)
   {
      veh_ptr->time_info = mads_time;	/* belongs in vehicle */
      veh_ptr->q_time = (time_ticks++) + (mads_time >> 8);
   }
   Zone_veh_list_add(veh_list_hndl, veh_hndl);
   veh_hndl = Zone_veh_list_find(veh_list_hndl, veh_hndl);	/* to access the list */
   nxt_veh_hndl = Zone_veh_list_get(veh_list_hndl, ZONE_VEH_LIST_NEXT);
   if (nxt_veh_hndl != (VEH_HNDL) HNDL_ERR)
      ptime = (long) Veh_get_value(nxt_veh_hndl, VEH_WARM_TIME) - get_gran();

   if (!ptime)
      time(&ptime);
   Veh_set_value(veh_hndl, VEH_WARM_TIME, (HNDL) ptime);
   writer_post(veh_ptr->fleet, veh_ptr->veh_nbr, zone_nbr, ptime, veh_ptr->t_status.e_posted);
}                             /* end Zone_veh_end_queue */

static RET_STATUS
decrement_short(s)
   short *s;
{
   if (--*s < 0)
   {
      *s = 0;
      /* ERROR(' ', "", "Attempt to decrement counter less than 0!"); */
      return (FAIL);
   }
   return (SUCCESS);
}                           /* end decrement_short */

static int
dec_error(zone_ptr, veh_ptr)
   ZONE *zone_ptr;
   VEH  *veh_ptr;
{
   char  error_str[80];
#ifdef DEBUG
   sprintf(error_str, "0 decrement zone %d, veh %d, e_posted %d, class %c",
	   zone_ptr->zone_nbr, veh_ptr->veh_nbr, veh_ptr->t_status.e_posted, veh_ptr->driver_class);
   ERROR(' ', "", error_str);
#ifdef MORE
   sprintf(error_str, " offerd %d accepted %d call_nbr %d zone_nbr %d",
	   veh_ptr->t_status.offered, veh_ptr->t_status.accepted, veh_ptr->call_nbr, veh_ptr->zone_num);
   ERROR(' ', "", error_str);
#endif
#endif
   return(SUCCESS);
}                    /* end dec_error */

/*-------------------------------------------------------
 *	Vehicle change q priority
 *-------------------------------------------------------
 */
RET_STATUS
Vehicle_change_q_priority(
	VEH_HNDL	vehicle, 
	char		priority
	)
{
	struct veh_driv		*vehicle_struct = (struct veh_driv *)vehicle;
	short				zone_number;
	ZONE_HNDL			zone_handle;
	RET_STATUS			rc;
	ZONE_VEH_LIST_HNDL 	vehicle_list;

	zone_number = vehicle_struct->zone_num;	

/*
 *	not in a zone list
 */
	if (zone_number <= 0)
	{
		vehicle_struct->q_priority = priority;
		return( SUCCESS );
	}

/*
 *	check if in a zone list
 */
	rc = Zone_get_hndl(zone_number, vehicle_struct->fleet_nbr, &zone_handle);
	if (rc != SUCCESS)
		return( FAIL );
		
	vehicle_list = 
		(ZONE_VEH_LIST_HNDL)Zone_get_value(zone_handle, ZONE_AVAIL_VEH_LIST);
		
	if (Zone_veh_list_find(vehicle_list, vehicle) != HNDL_ERR)
	{
      	ERROR(' ', "", "Veh_change_q_priority - Vehicle currently in Zone");

		rc = Zone_veh_list_remove(vehicle_list, vehicle);
		if (rc != SUCCESS)
			return( FAIL );

		vehicle_struct->q_priority = priority;

		rc = Zone_veh_list_add(vehicle_list, vehicle);
		if (rc != SUCCESS)
			return( FAIL );

		return( SUCCESS );
	}

	vehicle_struct->q_priority = priority;

	return( SUCCESS );
}



Zone_add_tc(
	       ZONE_HNDL    zone_hndl,
	       CALL_HNDL    call_hndl
	       )
{
  TC_STRUCT *pTimeCall;
  ZONE_TC_LIST_HNDL tc_list_hndl = HNDL_NULL;
  TC_HNDL tc_hndl, found_tc_hndl = HNDL_NULL;
  struct calls       *call_ptr;
  CALL_REC           *cl_ptr;
  ZONE_HNDL multi_zone_hndl = HNDL_NULL;  
  
  if ( call_hndl == HNDL_NULL )
    return( FAIL );

  if ( zone_hndl == NULL )
    if (Zone_get_hndl(WAKEUP_ZONE, (short) Call_get_value(call_hndl, CALL_FLEET_NBR), &zone_hndl) == FAIL)
      return( FAIL );

  call_ptr = ( CALL * ) call_hndl;

  /* Handle the condition where a multi-trip time call is being added */
  if ( ( call_ptr->call_type.multiple == 1 ) && ( call_ptr->grouped_with > 0 ) )
    {
      /* if the 'grouped_with' trip is already in the TC list then, we don't need
         to do anything here */
      if ( Call_rec_alloc((CALL_REC_HNDL *) & cl_ptr ) != SUCCESS )
	{
	  ERROR(' ', "", "Call_rec_alloc failure Zone_add_tc" );
	}
      else
	{
	  cl_ptr->nbr = call_ptr->grouped_with;
	  if ( Call_rec_read((CALL_REC_HNDL) cl_ptr, DB_EQUAL | DB_LOCK, CALL_KEY1 ) != SUCCESS )
	    {
	      ERROR(' ', "", "Call_rec_read failure GPS_check_zone");
              Call_release_locks();
	      Call_rec_free((CALL_REC_HNDL *) & cl_ptr );
	    }
          else /* multi-trip record exists */
            {
              /* get the zone_hndl for the multi-trip and check the TC_list for this one */
              Zone_get_hndl( cl_ptr->pckup_zone, (short)Call_get_value(call_hndl, CALL_FLEET_NBR),
                             &multi_zone_hndl );
              if ( multi_zone_hndl != HNDL_NULL )
                {
                  tc_list_hndl = (ZONE_TC_LIST_HNDL)Zone_get_value( multi_zone_hndl, ZONE_TC_LIST );
                  if ( tc_list_hndl != NULL )
                    {
                      Zone_tc_list_each_get( tc_list_hndl, tc_hndl )
                        {
                          pTimeCall = (TC_STRUCT *)tc_hndl;
                          if ( pTimeCall->nbr == cl_ptr->nbr )  /* multi-trip is already in TC_list */
                            {
                              Call_release_locks();
                              Call_rec_free((CALL_REC_HNDL *) & cl_ptr );                              
                              return(FAIL);
                            }
                        } Zone_tc_list_each_end(tc_list_hndl)

                            }
                }
            }
        }
      
      Call_release_locks();
      Call_rec_free((CALL_REC_HNDL *) & cl_ptr );                                    
    }

  
  tc_list_hndl = ( ZONE_TC_LIST_HNDL ) Zone_get_value( zone_hndl, ZONE_TC_LIST );
  if ( tc_list_hndl != NULL )
    {
      Zone_tc_list_each_get( tc_list_hndl, tc_hndl )
	{
	  pTimeCall = ( TC_STRUCT * )tc_hndl;
	  if ( pTimeCall->nbr == (int)( CALL * )call_ptr->call_number )
	    {
	      found_tc_hndl = tc_hndl;
	      break;
	    }
	} Zone_tc_list_each_end(tc_list_hndl)
    }

  if ( found_tc_hndl != HNDL_NULL )
    {
      /* enter here if we just performed an update of the due_time for TC */
      /* we'll remove it and then reenter it in the list                  */
      Zone_tc_list_remove( tc_list_hndl, found_tc_hndl );
      tc_hndl = found_tc_hndl;
      pTimeCall = tc_hndl;
      memset((char *) pTimeCall, '\0', sizeof( TC_STRUCT ) );
    }

  /* grab a new chunk from heap  */
  /* if one didn't already exist */
  if ( found_tc_hndl == HNDL_NULL )
    {
      tc_hndl = (TC_HNDL) malloc(sizeof( TC_STRUCT ) );
      ++num_tc;
      pTimeCall = ( TC_STRUCT * ) tc_hndl;      
      memset((char *) pTimeCall, '\0', sizeof( TC_STRUCT ));
    }
  

  call_ptr = ( struct calls * )call_hndl;

  if ( ( TC_load( (CALL_HNDL) call_hndl, (TC_HNDL)pTimeCall ) == SUCCESS ))
      Zone_tc_list_add( tc_list_hndl, pTimeCall );
  else
    {
      free( tc_hndl );
      --num_tc;
    }  
}

Zone_remove_tc(
	       ZONE_HNDL    zone_hndl,
	       CALL_HNDL    call_hndl
	       )
{
  ZONE_TC_LIST_HNDL    tc_list_hndl;
  TC_HNDL              tc_hndl, found_tc_hndl = HNDL_NULL;
  TC_STRUCT            *pTimeCall;
  CALL                 *pCall;

  if ( call_hndl == HNDL_NULL )
    return ( FAIL );
  
  pCall = ( CALL * ) call_hndl;
  
  if ( zone_hndl == NULL )
    if (Zone_get_hndl(WAKEUP_ZONE, (short) Call_get_value(call_hndl, CALL_FLEET_NBR), &zone_hndl) == FAIL)
      return( FAIL );
  
  tc_list_hndl = ( ZONE_TC_LIST_HNDL ) Zone_get_value( zone_hndl, ZONE_TC_LIST );
  if ( tc_list_hndl != NULL )
    {
      Zone_tc_list_each_get( tc_list_hndl, tc_hndl )
	{
	  pTimeCall = ( TC_STRUCT * )tc_hndl;
	  if ( pTimeCall->nbr == (int)( CALL * )pCall->call_number )
	    {
	      found_tc_hndl = tc_hndl;
	      break;
	    }
	} Zone_tc_list_each_end(tc_list_hndl)
	  
       if ( found_tc_hndl != HNDL_NULL )
	 {
	   Zone_tc_list_remove( tc_list_hndl, found_tc_hndl );
	   free( tc_hndl );
	   --num_tc;
	   return( SUCCESS );
	 }
    }
  
  return( FAIL );
}

