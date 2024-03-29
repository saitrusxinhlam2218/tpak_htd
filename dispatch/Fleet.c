/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Fleet.c,v $
*  @(#)  $Revision: 1.26 $
*  @(#)  $Date: 2004/06/22 17:08:17 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Fleet.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Fleet.c,v 1.26 2004/06/22 17:08:17 jwelch Exp $";

/* Fleet.c */
#include <sys/types.h>		/* temp */
#include <sys/times.h>		/* temp */
#include "mad_error.h"
#include "taxipak.h"
#include "Object.h"
#include "acc.h"
#include "List_public.h"
#include "Zone_public.h"
#include "Zone_private.h"
#include "Zone_lists_public.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Fleet_lists_public.h"
#include "Vehicle_private.h"
#include "Call_private.h"
#include "TimeCalls_public.h"
#include "TimeCalls_private.h"

#include "timers.h"
#include "Timer_public.h"
#include "Call_public.h"
#include "mad_ipc.h"
#include "dispatch.h"
#include "language.h"

#include "STD.h"
#include "acc.h"
#include "msg.h"
#include "msg_mmp.h"
#include "kdt.h"
#include "enhance.h"

extern FLEET *fleet[ ];
extern struct offsets *offset;
extern time_t mads_time;	/* system time */
int    giNumCalls = 0;
long glEnhSwitch;

/************************************************/
/* RET_STATUS Fleet_zone_summary(fl_nbr)        */
/* OP: update the fleets zone summary           */
/* PRE: fleet exists                            */
/* POST: fleet zone summary updated             */
/*       foe each zone following updated        */
/*          nbr_calls                           */
/*          nbr_veh                             */
/*          nbr_enroute                         */
/*          avg_time                            */
/*       T_ZN_SUMMARY timer started             */
/* ERROR: Runable to create timer               */
/************************************************/

RET_STATUS
Fleet_zone_summary(           /* Was zone_summary */
   short   fl_nbr)
{
   FLEET *fleet_ptr;	/* pointer to a fleet structure */
   struct zones *zone_tmp_ptr;	/* pointer to a zone structure */
   register jj, cnt = 0;		/* counter variables */
   int timeout;
   short time_calls = 0;
   short num_calls = 0;
   ZONE_CALLS_LIST_HNDL calls_list_hndl;
   CALL_HNDL call_hndl;
   ZONE_HNDL zone_hndl;
   TC_STRUCT *pTimeCall;
   int   tc_slots = 0;   

   fleet_ptr = fleet[fl_nbr];	/* get a pointer to that fleet */
   

   for (jj = 1; jj < ZONE_MAX; jj++)
   {				/* for all the zones in the fleet */
      if (fleet_ptr->zone_ptr[jj] != NULL)
      {				/* if the zone is being used */
	 zone_tmp_ptr = fleet_ptr->zone_ptr[jj];	/* get a pointer to that zone */
	 fleet_ptr->zone_sumry[cnt].time_outs = SPACE;	/* assume no timed out
							 * calls */
	 if (zone_tmp_ptr->cbook_time > 0)
	    timeout = zone_tmp_ptr->cbook_time;
	 else
	    timeout = fleet_ptr->cbook_time;

	 /* see also zone_status */
	 (void) Zone_get_hndl(jj, (short) fl_nbr, &zone_hndl);
	 calls_list_hndl = (ZONE_CALLS_LIST_HNDL) Zone_get_value(zone_hndl, ZONE_AVAIL_CALLS_LIST);
	 num_calls = 0;
	 Zone_calls_list_each_get(calls_list_hndl, call_hndl)
	 {
	    if ( ( Call_get_state( (CALL_HNDL)call_hndl, CALL_AVAILABLE_STATE ) ) &&
		 ( fl_nbr == (short)Call_get_value( (CALL_HNDL)call_hndl, CALL_FLEET_NBR ) ) )
	      {
		++num_calls;
		if ( ((mads_time - (time_t) Call_get_value(call_hndl, CALL_TIME_ENTERED)) >= timeout) &&
                     (!Call_get_state( (CALL_HNDL)call_hndl, CALL_WAIT_EPOST_STATE ) ) )
		  fleet_ptr->zone_sumry[cnt].time_outs = YES;
	      }
	 } Zone_calls_list_each_end(calls_list_hndl)

         if (0)
	     {
		 // TSAB wants call takers to ALWAYS see at least 30 minutes of time calls on screen 32
		 if ( ( tc_slots = (short)Fleet_get_value( (FLEET_HNDL)fleet_ptr, FLEET_TC_LOOK_AHEAD ) ) < 6 )
		     tc_slots = 6;
	     }
	 else
	     tc_slots = (short)Fleet_get_value( (FLEET_HNDL)fleet_ptr, FLEET_TC_LOOK_AHEAD );	       

	 Zone_tc_list_each_get( Zone_get_value( zone_hndl, ZONE_TC_LIST ), pTimeCall )
	   {
	     if ( (mads_time + (tc_slots * 5 * 60) ) >
		 (time_t)TC_get_value( (TC_HNDL)pTimeCall, TC_LEAD_TIME ) )	     
	       time_calls++;
	   } Zone_tc_list_each_end(Zone_get_value( zone_hndl, ZONE_TC_LIST ))
	     
	 fleet_ptr->zone_sumry[cnt].zone_num = jj;
	 fleet_ptr->zone_sumry[cnt].no_calls = num_calls;
	 fleet_ptr->zone_sumry[cnt].no_veh = (short) Zone_get_value( zone_hndl, ZONE_NBR_VEH );
	 fleet_ptr->zone_sumry[cnt].no_enroute = (short) Zone_get_value( zone_hndl, ZONE_NBR_ENROUTE );
	 fleet_ptr->zone_sumry[cnt].no_timecalls = time_calls;
	 fleet_ptr->zone_sumry[cnt].avg_time = zone_tmp_ptr->avg_time;	/* put in avg_time */
	 ++cnt;
      }
      time_calls = 0;
   }

   fleet_ptr->zone_ct_ptr = cnt;/* fill  in the number of zones */
   fleet_ptr->zone_update_time = mads_time;	/* set time of summary */
   if (fleet_ptr->zone_status_update_freq > 0)	/* start summary  timer */

     // ZoneDown processing
     if (offset->status.ncp == 1)
       ZoneDownCheck();

      if ((TIMER_HANDLE) Timer_create(
				      TIMER_TYPE, T_ZN_SUMMARY,
				      TIMER_NOTIFY_FUNC, Fleet_zone_summary,
		                      TIMER_EXPIRE_TIME, fleet_ptr->zone_status_update_freq,
				      TIMER_DATA, fl_nbr,
				      ATTRIBUTE_END) == HANDLE_NULL)
      {
	 ERROR(' ', "", "add timer T_ZN_SUMMARY");
         return(FAIL);
      }
      return(SUCCESS);
}          /* end Fleet_zone_summary */

/******************************************************/
/* RET_STATUS Fleet_create(fleet_nbr)                 */
/* OP:  create a fleet                                */
/* PRE: none                                          */
/* POST:  fleet lists created                         */
/* ERROR: illegal fleet nbr                           */
/******************************************************/
FLEET_HNDL
Fleet_create(
   short fleet_nbr)
{
   /* partial */
   if (fleet_nbr < 0 || fleet_nbr >= FLEET_MAX)
       return(HNDL_ERR);
   fleet[fleet_nbr]->fleet_driver_veh_list_hndl = Fleet_driver_veh_list_create();
   fleet[fleet_nbr]->fleet_veh_nbr_list_hndl = Fleet_veh_nbr_list_create();
   fleet[fleet_nbr]->fleet_zone_calls_list_hndl = Fleet_zone_calls_list_create();
   fleet[fleet_nbr]->veh_susp_pass_list_hndl = Fleet_susp_veh_list_create();
   fleet[fleet_nbr]->veh_susp_deliv_list_hndl = Fleet_susp_veh_list_create();
   fleet[fleet_nbr]->veh_susp_typex_list_hndl = Fleet_susp_veh_list_create();
   fleet[fleet_nbr]->drv_susp_pass_list_hndl = Fleet_susp_drv_list_create();
   fleet[fleet_nbr]->drv_susp_typex_list_hndl = Fleet_susp_drv_list_create();
   fleet[fleet_nbr]->drv_susp_deliv_list_hndl = Fleet_susp_drv_list_create();
   fleet[fleet_nbr]->big_car_trip_list_hndl = Fleet_big_car_trip_list_create();

   return((FLEET_HNDL) fleet[fleet_nbr]);
}               /* end Fleet_create */

Fleet_bcast_emergency( FLEET_HNDL fleet_hndl, VEH_HNDL veh_hndl, short radius )
{
  char  bcast_buf[512];
  double veh_inx = 0.0, veh_iny = 0.0, veh_outx = 0.0, veh_outy = 0.0;
  struct veh_driv *veh_ptr;
  struct fleets *fl_ptr;
  char  term_id[MID_SIZE + 1];  

  bzero( bcast_buf, 512 );
  veh_ptr = (struct veh_driv *)veh_hndl;
  
  veh_inx = veh_ptr->gps_long;
  veh_iny = veh_ptr->gps_lat;

  if ( veh_inx <= 0.0 || veh_iny <= 0.0 )
    return;

  UTM_to_LatLong( veh_iny, veh_inx, &veh_outy, &veh_outx );

  sprintf( bcast_buf, "%%.XS%04d+%07d+%08d%03d.",
           veh_ptr->veh_nbr,
           (int)(veh_outy*100000), (int)(veh_outx*100000), radius/100 );

  fl_ptr = (struct fleets *)fleet_hndl;

  memcpy( term_id, GROUP_MID, MID_SIZE - 1 );
  sprintf( term_id + MID_SIZE - 1, "%d", fl_ptr->fleet_nbr );
  config_outb_msg(10, "000", ' ', ' ', 'M', ' ');
  mk_outb_text( bcast_buf );
  send_msg_mmp( term_id, SEND_EMERG_BCAST, (struct veh_driv *) NULL );
  config_outb_msg(10, "000", ' ', ' ', ' ', ' ');
  
}

stand_bcast_notify_func(fleet_nbr)
{
  send_stand_bcast( fleet_nbr );
  Fleet_make_stand_notify_report(fleet_nbr); // create the next timer
  return (SUCCESS);
}

search_report_notify_func(fleet_nbr)
   short   fleet_nbr;
{
   Fleet_make_search_report(fleet_nbr, 0);
   send_srch_display(fleet_nbr);

   send_bid_display( fleet_nbr );

   /* put it here for now            */
   /* later create a new debug timer */
   LL_debug();
   
   return (SUCCESS);
}


/*********************************************************************/
/*  comparison routine for qsort() in d_search(). We need to */
/*   sort from high to low, so the routine values are the opposite */
/*  of what would be used for sorting low to high. Note that although */
/*  it's not necessary to return 0 on the case where the two values */
/*  are equal, it improves sorting performance by more than 50% if */
/*  0 is returned. */
/*********************************************************************/
int
compare_call_count(sc1, sc2)
   struct search_count *sc1, *sc2;
{
   if (sc1->num_calls < sc2->num_calls)
      return (1);
   else if (sc1->num_calls > sc2->num_calls)
      return (-1);
   return (0);
}

/**********************************************************************/
/* RET_STATUS Fleet_make_search_report(fleet_nbr, delay)              */
/* OP: if any calls available make up a serarch report for broadcast  */
/* PRE: fleet exists                                                  */
/* POST: report created for each zone group                           */
/*       for each zone in fleet                                       */
/*          if avail calls in zone                                    */
/*            number of vehicles in zone added to report              */
/*            number of calls available in zone added to report       */
/*       reported sorted by nbr_calls                                 */
/*       T_SRCH timer statted at ZONE_INFO_FREQ + delay               */
/* ERROR: timer creation error                                        */
/**********************************************************************/

/********************************************************************************/
/* making a Search report*/
/**/
/* Build the search report so MDTs can read it when a search report is needed.*/
/**/
/*Each MDT has MDT_ZONE_SCREEN_MAX (now set to 3) screens that store the info*/
/*on the 30 busiest zones. Each zone is assigned a screen number 1, 2 or 3*/
/*for which MDT screen it will appear on. What d_search() does is find all the*/
/*valid zones for a fleet, and if the zone has calls in it, determine which screen*/
/*the info will appear on, keep track of 30 busiest zones for each screen number,*/
/*and sort the results for each screen. There are up to 999 zones per screen*/
/*and only 30 displayed on a screen. This is the algorith used to get the 30*/
/*busiest zones:*/
/**/
/*for each zone with calls:*/
/**/
/*  If the busiest zone array is not full, add the zone info to the end of the array.*/
/**/
/*  If the array is full then if the current zone is busier than least busy zone in the*/
/*  array, replace the least busy zone in the array with the current zone. Search the*/
/*  array for the new least busy zone.*/
/**/
/*Sort the array in the order from most to least busiest zone.*/
/**/
/***************************************************************************************/

RET_STATUS
Fleet_make_stand_notify_report(
			       short fleet_nbr)
{
   FLEET *fl_ptr;
   
   fl_ptr = fleet[fleet_nbr];
   
  if ((TIMER_HANDLE) Timer_create( TIMER_TYPE, T_SRCH, TIMER_EXPIRE_TIME,
				   fl_ptr->stand_ntfy_interval,
				   TIMER_DATA, fleet_nbr,
				   TIMER_NOTIFY_FUNC, stand_bcast_notify_func,
				   ATTRIBUTE_END) == HANDLE_NULL )
    return(FAIL);
  
}


RET_STATUS
Fleet_make_search_report(                   /* WAS d_search */
   short   fleet_nbr,
   short   delay)
{
   FLEET *fl_ptr;
   CALL_HNDL call_hndl;
   ZONE_CALLS_LIST_HNDL calls_list_hndl;
   int   low_slot[MDT_ZONE_SCREEN_MAX];
   int   low_value[MDT_ZONE_SCREEN_MAX];
   int   screen_count[MDT_ZONE_SCREEN_MAX];
   short   zone_nbr;
   int   nbr_avail_calls;
   int   mdt_screen;
   int   compare_call_count();
   ZONE_HNDL zone_hndl;
   struct zones *zone_ptr;
   short kk,jj,ll;
   BOOLEAN dup_found;
   FLEET_ZONE_CALLS_LIST_HNDL fleet_zone_calls_list;


   fl_ptr = fleet[fleet_nbr];
   if (fl_ptr->zone_info_freq == 0)
      return(FAIL);

   bzero( &fl_ptr->search_report, sizeof( struct search_report ) );
   fl_ptr->search_report.search_time = mads_time;

   for (mdt_screen = 0; mdt_screen < MDT_ZONE_SCREEN_MAX; mdt_screen++)
   {
      /* 9999999 is some arbitrary high value */
      low_value[mdt_screen] = 99999999;
      screen_count[mdt_screen] = 0;
   }

   if (0)
     {
       for (jj = 1; jj < ZONE_MAX - 1; jj++ )
	 {
	   if ( fl_ptr->zone_ptr[jj] != NULL )
	     {
	       (void)Zone_get_hndl( jj, (short)fleet_nbr, &zone_hndl );
	       zone_ptr = (struct zones *) zone_hndl;
	   
	       /* mdt_screen value of 0 means do not broadcast */
	       if ( zone_ptr->mdt_screen == 0 )
		 continue;
	       
	       mdt_screen = zone_ptr->mdt_screen - 1;
	       nbr_avail_calls = zone_ptr->num_calls;
	       
	       
	       /* if out of range, put it on the first screen. */
	       if (mdt_screen < 0 || mdt_screen >= MDT_ZONE_SCREEN_MAX)
		 mdt_screen = 0;
	       
	       /* If the array for an MDT screen is full, then if the new number of */
	       /* calls is more that the lowest number in the array, replace the */
	       /* lowest with the new. Then a search of the array must be done to find */
	       /* the lowest again. */
	       if (screen_count[mdt_screen] == ZONES_PER_MDT_SCREEN_MAX)
		 {
		   if (nbr_avail_calls > low_value[mdt_screen])
		     {
		       fl_ptr->search_report.screen[mdt_screen].count[low_slot[mdt_screen]].zone_num = zone_ptr->zone_nbr;
		       fl_ptr->search_report.screen[mdt_screen].count[low_slot[mdt_screen]].num_calls = nbr_avail_calls;
		       fl_ptr->search_report.screen[mdt_screen].count[low_slot[mdt_screen]].num_cabs = zone_ptr->num_taxis;
		       
		       /*
			* Reset the low value to slot 0. Most likely it is not in slot
			* 0, but use it as a starting point.
			*/
		       low_value[mdt_screen] = fl_ptr->search_report.screen[mdt_screen].count[0].num_calls;
		       low_slot[mdt_screen] = 0;
		       
		       /*
			* It's necessary to search the entire array to find the lowest
			* slot
			*/
		       for (kk = 1; kk < ZONES_PER_MDT_SCREEN_MAX; kk++)
			 if (fl_ptr->search_report.screen[mdt_screen].count[kk].num_calls < low_value[mdt_screen])
			   {
			     low_value[mdt_screen] = fl_ptr->search_report.screen[mdt_screen].count[kk].num_calls;
			     low_slot[mdt_screen] = kk;
			   }
		     }
		 }
	       /* Since the array is not full, just place it in the next slot */
	       else
		 {
		   fl_ptr->search_report.screen[mdt_screen].count[screen_count[mdt_screen]].zone_num = zone_ptr->zone_nbr;
		   fl_ptr->search_report.screen[mdt_screen].count[screen_count[mdt_screen]].num_calls = nbr_avail_calls;
		   fl_ptr->search_report.screen[mdt_screen].count[screen_count[mdt_screen]].num_cabs = zone_ptr->num_taxis;
		   
		   /*
		    * Keep track of the low value so it can be used if the array gets
		    * full
		    */
		   if (nbr_avail_calls < low_value[mdt_screen])
		     {
		       low_value[mdt_screen] = nbr_avail_calls;
		       low_slot[mdt_screen] = screen_count[mdt_screen];
		     }
		   screen_count[mdt_screen]++;
		 }
	     }
	 }
     }
   else
     {
       fleet_zone_calls_list = fleet[fleet_nbr]->fleet_zone_calls_list_hndl;
       Fleet_zone_calls_list_each_get(fleet_zone_calls_list, zone_hndl)
	 {
	   zone_ptr = (struct zones *) zone_hndl;

	   /* don't broadcast wakeup zone info */
	   if ( zone_ptr->zone_nbr == WAKEUP_ZONE )
	     continue;
	   
	   /* mdt_screen value of 0 means do not broadcast */
	   if ( zone_ptr->mdt_screen == 0 )
	     continue;
	   
	   mdt_screen = zone_ptr->mdt_screen - 1;
	   nbr_avail_calls = 0;
	   calls_list_hndl = (ZONE_CALLS_LIST_HNDL) Zone_get_value(zone_hndl, ZONE_AVAIL_CALLS_LIST);
	   Zone_calls_list_each_get(calls_list_hndl, call_hndl)
	     {
	       if ( ( Call_get_state( (CALL_HNDL)call_hndl, CALL_AVAILABLE_STATE ) ) &&
                    (!Call_get_state( (CALL_HNDL)call_hndl, CALL_WAIT_EPOST_STATE ) ) &&
		   ( fleet_nbr == (short)Call_get_value( (CALL_HNDL)call_hndl, CALL_FLEET_NBR ) ) )
		   ++nbr_avail_calls;
	     } Zone_calls_list_each_end(calls_list_hndl)
	   
	   if (!nbr_avail_calls)
	     continue;
	   
	   /* if out of range, put it on the first screen. */
	   if (mdt_screen < 0 || mdt_screen >= MDT_ZONE_SCREEN_MAX)
	     mdt_screen = 0;
	   
	   /* If the array for an MDT screen is full, then if the new number of */
	   /* calls is more that the lowest number in the array, replace the */
	   /* lowest with the new. Then a search of the array must be done to find */
	   /* the lowest again. */
	   if (screen_count[mdt_screen] == ZONES_PER_MDT_SCREEN_MAX)
	     {
	       if (nbr_avail_calls > low_value[mdt_screen])
		 {
		   dup_found = FALSE;
		   for ( ll = 0; ll <= screen_count[mdt_screen]; ll++ )
		     {
		       if ( fl_ptr->search_report.screen[mdt_screen].count[ll].zone_num == zone_ptr->zone_nbr )
			 dup_found = TRUE;
		     }
		   if ( !dup_found )
		     {
		       fl_ptr->search_report.screen[mdt_screen].count[low_slot[mdt_screen]].zone_num = zone_ptr->zone_nbr;
		       fl_ptr->search_report.screen[mdt_screen].count[low_slot[mdt_screen]].num_calls = nbr_avail_calls;
		       fl_ptr->search_report.screen[mdt_screen].count[low_slot[mdt_screen]].num_cabs = zone_ptr->num_taxis;
		     }
		   
		   /*
		    * Reset the low value to slot 0. Most likely it is not in slot
		    * 0, but use it as a starting point.
		    */
		   low_value[mdt_screen] = fl_ptr->search_report.screen[mdt_screen].count[0].num_calls;
		   low_slot[mdt_screen] = 0;
		   
		   /*
		    * It's necessary to search the entire array to find the lowest
		    * slot
		    */
		   for (kk = 1; kk < ZONES_PER_MDT_SCREEN_MAX; kk++)
		     if (fl_ptr->search_report.screen[mdt_screen].count[kk].num_calls < low_value[mdt_screen])
		       {
			 low_value[mdt_screen] = fl_ptr->search_report.screen[mdt_screen].count[kk].num_calls;
			 low_slot[mdt_screen] = kk;
		       }
		 }
	     }
	   /* Since the array is not full, just place it in the next slot */
	   else
	     {
	       dup_found = FALSE;
	       for ( ll = 0; ll <= screen_count[mdt_screen]; ll++ )
		 {
		   if ( fl_ptr->search_report.screen[mdt_screen].count[ll].zone_num == zone_ptr->zone_nbr )
		     dup_found = TRUE;
		 }
	       if ( !dup_found )
		 {
		   fl_ptr->search_report.screen[mdt_screen].count[screen_count[mdt_screen]].zone_num = zone_ptr->zone_nbr;
		   fl_ptr->search_report.screen[mdt_screen].count[screen_count[mdt_screen]].num_calls = nbr_avail_calls;
		   fl_ptr->search_report.screen[mdt_screen].count[screen_count[mdt_screen]].num_cabs = zone_ptr->num_taxis;
		   /*
		    * Keep track of the low value so it can be used if the array gets
		    * full
		    */
		   if (nbr_avail_calls < low_value[mdt_screen])
		     {
		       low_value[mdt_screen] = nbr_avail_calls;
		       low_slot[mdt_screen] = screen_count[mdt_screen];
		     }
		   screen_count[mdt_screen]++;		   
		 }
	     }
	 }
       Fleet_zone_calls_list_each_end(fleet_zone_calls_list)
     }



   /*
    * Sort the zone numbers on each mdt screen using the library routine
    * qsort(). Sort in order of number of calls per zone
    */
   for (mdt_screen = 0; mdt_screen < MDT_ZONE_SCREEN_MAX; mdt_screen++)
   {
      fl_ptr->search_report.screen[mdt_screen].num_in_search = screen_count[mdt_screen];

      /* No point in sorting if 1 or 0 entries */
      if (screen_count[mdt_screen] > 1)
	 qsort(fl_ptr->search_report.screen[mdt_screen].count, screen_count[mdt_screen],
	       sizeof(struct search_count), compare_call_count);
   }

   /* Start the search report timer so this function gets executed again */

   if ((TIMER_HANDLE) Timer_create(
				   TIMER_TYPE, T_SRCH,
			  TIMER_EXPIRE_TIME, fl_ptr->zone_info_freq + delay,
				   TIMER_DATA, fleet_nbr,
			       TIMER_NOTIFY_FUNC, search_report_notify_func,
				   ATTRIBUTE_END) == HANDLE_NULL)
   {
      ERROR(' ', "", "timer create T_SRCH");
      return(FAIL);
   }
   return(SUCCESS);
}				/* end Fleet_make_search_report  */

/************************************************/
/* RET_STATUS Fleet_initialize(fleet_nbr)       */
/* OP: initialize a fleet                       */
/* PRE: fleet exists                            */
/* POST: fleet call stats initoialized to zero  */
/*       fleet statistics initialized to zero   */
/* ERROR: none                                  */
/************************************************/

RET_STATUS
Fleet_initialize(
   short fleet_nbr)
{
      FLEET * fl_ptr;
      short jj, kk;

      /* no check for bad number - beware */
      fl_ptr = fleet[fleet_nbr];

      for (jj = 0; jj < 3; jj++)
      {
	 for (kk = 0; kk < 6; kk++)
	 {
	    fl_ptr->call_stats[jj][kk] = 0;
	 }
      }

      /* what about clearing out the lists ?? */

      fl_ptr->signed_on = 0;
      fl_ptr->signed_off = 0;
      fl_ptr->calls = 0;
      fl_ptr->cancels = 0;
      fl_ptr->flags = 0;
      fl_ptr->bids = 0;
      fl_ptr->cond_posts = 0;
      fl_ptr->enroute_posts = 0;
      fl_ptr->short_meters = 0;
      fl_ptr->late_meters = 0;
      fl_ptr->no_accepts = 0;
      fl_ptr->asigns = 0;
      fl_ptr->rejects = 0;
      fl_ptr->breaks = 0;
      fl_ptr->req_to_talk = 0;
      fl_ptr->messages = 0;
      fl_ptr->callbacks = 0;
      fl_ptr->callouts = 0;
      fl_ptr->no_shows = 0;
      fl_ptr->suspended = 0;
      fl_ptr->emergencies = 0;
      fl_ptr->wakeups = 0;
      return(SUCCESS);
}                              /* end Fleet_initialize */

/**************************************************************/
/* RET_STATUS Fleet_update_pickup_stats(fl_nbr)     */
/* OP: update the pickup stats for each zone        */
/* PRE: fleet exists                                */
/* POST:  for each zone in fleet following computed & recorded */
/*            avg time to pickup                               */
/*         T_TM_TO_PCKUP timer started                         */
/* ERROR: unable to create timer                              */
/***************************************************************/
   
RET_STATUS
Fleet_update_pickup_stats(              /* WAS stats_to_pickup */
   short   fl_nbr)
{
   FLEET *fl_ptr;
   struct zones *zone_tmp_ptr;
   register i, j;
   int   z;
   time_t tmp_time = 0;
   int   pickups;
   int   new_avg_time;
   CALL_HNDL call_hndl;
   ZONE_HNDL zone_hndl;
   ZONE_CALLS_LIST_HNDL calls_list_hndl;
   time_t time_entered;

#ifdef DEBUG
   if (offset->prattle >= LEVEL2)
   {
      sprintf(trace_str, "stats_to_pickup : fleet %d \n", fl_nbr);
      TRACE(trace_str);
   }
#endif

   fl_ptr = fleet[fl_nbr];

   for (i = 1; i < ZONE_MAX; i++)
   {
      pickups = 0;
      tmp_time = 0;
      new_avg_time = 0;
      if (fl_ptr->zone_ptr[i] != NULL)
      {
	 zone_tmp_ptr = fl_ptr->zone_ptr[i];
	 for (z = 2; z < fl_ptr->num_levels_to_compute_stats; z++)
	 {
	    for (j = 0; j < BCK_UP_MAX; j++)
	    {
	       if ((zone_tmp_ptr->level_number[z].bck_up_zone[j] > 0) &&
		    /*level # z*/ (zone_tmp_ptr->level_number[z].bck_up_zone[j] < ZONE_MAX))
	       {
                 if ( zone_tmp_ptr->level_number[z].bck_up_zone[j] == 888 )
                   continue;
                 
		  tmp_time +=
		     fl_ptr->zone_ptr[zone_tmp_ptr->level_number[z].bck_up_zone[j]]->tm_to_pckup;
		  pickups +=
		     fl_ptr->zone_ptr[zone_tmp_ptr->level_number[z].bck_up_zone[j]]->calls_pckd_up;
#ifdef DEBUG

		  if (offset->prattle == LEVEL7)
		  {
		     sprintf(trace_str, "stats_to_pickup : fleet %d bk2zone %d calls %d time to pick %d\n",
		       fl_nbr, zone_tmp_ptr->level_number[2].bck_up_zone[j],
			     fl_ptr->zone_ptr[zone_tmp_ptr->level_number[2].bck_up_zone[j]]->calls_pckd_up,
			     fl_ptr->zone_ptr[zone_tmp_ptr->level_number[2].bck_up_zone[j]]->tm_to_pckup);
		     TRACE(trace_str);
		  }
#endif
	       }
	    }
	 }

	 if (zone_tmp_ptr->calls_pckd_up > 0)
	 {			/* if any pickups in the primary zone */
	    pickups += zone_tmp_ptr->calls_pckd_up;
	    tmp_time += zone_tmp_ptr->tm_to_pckup;
	 }
	 if (pickups < 3 && pickups > 0 && zone_tmp_ptr->pickup_lead_time > 0)
	 {			/* need at least 3 calls to compute */
	    tmp_time += get_super_lead(zone_tmp_ptr->pickup_lead_time, zone_tmp_ptr->super_zone, fl_nbr);
	    pickups++;
	 }
	 if (pickups)		/* avoid floating point except */
	    new_avg_time = (tmp_time / 60) / pickups;	/* get average pickup
							 * time in minutes */
	 else
	 {
	    /* if no calls available check activity */
	    if (!pickups && !tmp_time && zone_tmp_ptr->num_calls)
	    {
/*
                                new_avg_time = get_super_lead(zone_tmp_ptr->pickup_lead_time ,zone_tmp_ptr->super_zone,fl_nbr);
*/
	       (void) Zone_get_hndl(i, (short) fl_nbr, &zone_hndl);
	       calls_list_hndl = (ZONE_CALLS_LIST_HNDL) Zone_get_value(zone_hndl, ZONE_AVAIL_CALLS_LIST);
	       Zone_calls_list_each_get(calls_list_hndl, call_hndl)
	       {
		  time_entered = (time_t) Call_get_value(call_hndl, CALL_TIME_ENTERED);
		  if (time_entered > 0)
		  {
		     if ((mads_time - time_entered) > new_avg_time)
		     {

			if (offset->prattle == LEVEL7)
			{
			   sprintf(trace_str, "stats_to_pickup : call %d entered at %d older than %d \n",
				   (int) Call_get_value(call_hndl, CALL_NBR), time_entered, new_avg_time);
			   TRACE(trace_str);
			}
			new_avg_time = mads_time - time_entered;
		     }
		  }
	       } Zone_calls_list_each_end(calls_list_hndl)
		  new_avg_time = new_avg_time / 60;
	    }
	 }

/*

					
                        if( new_avg_time > 0)
*/
	 zone_tmp_ptr->avg_time = new_avg_time;
#ifdef DEBUG
	 if (offset->prattle == LEVEL7)
	 {
	    sprintf(trace_str, "stats_to_pickup :fleet %d primary zone %d calls %d time topick %d total avg time %d percent lead %d\n",
		    fl_nbr, i, zone_tmp_ptr->calls_pckd_up, zone_tmp_ptr->tm_to_pckup, new_avg_time,
		    get_super_lead(zone_tmp_ptr->pickup_lead_time, zone_tmp_ptr->super_zone, fl_nbr));
	    TRACE(trace_str);
	 }
#endif
      }
   }
   for (i = 1; i < ZONE_MAX; i++)
   {				/* Purge all zone info every
				 * fl_ptr->stats_compute_freq minutes and
				 * restart per TGI/LA */
      if (fl_ptr->zone_ptr[i] != NULL)
      {
	 fl_ptr->zone_ptr[i]->calls_pckd_up = 0;
	 fl_ptr->zone_ptr[i]->tm_to_pckup = 0;
      }
   }
   if (fl_ptr->stats_compute_freq)
      if ((TIMER_HANDLE) Timer_create(
				      TIMER_TYPE, T_TM_TO_PCKUP,
				      TIMER_NOTIFY_FUNC, Fleet_update_pickup_stats, 
			      TIMER_EXPIRE_TIME, fl_ptr->stats_compute_freq,
				      TIMER_DATA, fl_nbr,
				      ATTRIBUTE_END) == HANDLE_NULL)
      {
	 ERROR(' ', "", "create timer T_TM_TO_PCKUP");
      }
   return(SUCCESS);
}                       /* end Fleet_update_pickup_stats */

biz_status_report()
{
  FILE *biz_stat_file;
  char biz_stat_path[256];
  time_t  iNowTime;
  struct tm *tm_ptr;
  int      iCentury, iYear;
  int      iVehBookNobreak = 0, iVehBookBreak = 0, iVehSignOn = 0;
  int      iVehSTC = 0, iNumCalls = 0, i;
  struct veh_driv *pVehicle;
  VEH_HNDL        hVehicle = HNDL_NULL;
  FLEET_VEH_NBR_LIST_HNDL  veh_nbr_list = HNDL_NULL;
  FLEET_HNDL  fleet_hndl = HNDL_NULL;
  
#define BIZ_STATUS_FILE "/usr/taxi/run/traces/baromtern"

  if (0)
    {
      /** First get name of the file based on the current date **/
      /** File should be of the form: baromtern.YYMMDD         **/
      /** New file is created upon recording the first record  **/
      /** for the new day                                      **/
      iNowTime = (time_t)time( 0 );
      tm_ptr = localtime( &iNowTime );


      sprintf( biz_stat_path, "%s.%02d%02d%02d", BIZ_STATUS_FILE,
	                                         tm_ptr->tm_year % 100,
	                                         tm_ptr->tm_mon + 1,
	                                         tm_ptr->tm_mday );

      if ((biz_stat_file = fopen( biz_stat_path, "a")) == NULL )
	{
	  fprintf( stderr, "Error opening biz status file \n" );
	  return ( -1 );
	}

      if ( tm_ptr->tm_year < 100 )
	{
	  iCentury = 19;
	  iYear = tm_ptr->tm_year;
	}
      else
	{
	  iCentury = 20;
	  iYear = tm_ptr->tm_year - 100;
	}
      
      /* default to fleet A for stockholm only */
      Fleet_get_hndl( BY_NBR, 0, &fleet_hndl );
      veh_nbr_list = ( FLEET_VEH_NBR_LIST_HNDL )Fleet_get_value( fleet_hndl, FLEET_VEH_NBR_LIST );
      Fleet_veh_nbr_list_each_get( veh_nbr_list, hVehicle )
	{
	  pVehicle = ( struct veh_driv * ) hVehicle;
	  if ( pVehicle->signed_on_date > 0 && pVehicle->signed_off_date == 0 )
	    {
	      ++iVehSignOn;
	      if ( pVehicle->t_status.posted && pVehicle->t_status.on_break )
		++iVehBookBreak;
	      if ( pVehicle->t_status.posted && !pVehicle->t_status.on_break )
		++iVehBookNobreak;
	      if ( pVehicle->t_status.e_posted )
		++iVehSTC;
	    }
	} Fleet_veh_nbr_list_each_end

      iNumCalls = fleet[0]->calls - giNumCalls;

      /** Fleet Call counter is zeroed at midnight **/
      if ( iNumCalls < 0 )
	iNumCalls = 0;
      
      giNumCalls = fleet[0]->calls;
      
      fprintf( biz_stat_file, "%02d%02d%02d%02d:%02d%02d:%04d:%04d:%04d:%04d:%04d\n",
	                      iCentury, iYear, tm_ptr->tm_mon + 1,
	                      tm_ptr->tm_mday, tm_ptr->tm_hour,
	                      tm_ptr->tm_min, iVehBookNobreak, iVehBookBreak,
	                      iVehSignOn, iVehSTC, iNumCalls );
      fclose( biz_stat_file );
    }

  (void) Timer_create(
		      TIMER_TYPE, T_BIZ_STATUS,
		      TIMER_EXPIRE_TIME, 60,
		      TIMER_NOTIFY_FUNC, biz_status_report,
		      ATTRIBUTE_END);
}
