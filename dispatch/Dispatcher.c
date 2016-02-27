/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Dispatcher.c,v $
*  @(#)  $Revision: 1.42 $
*  @(#)  $Date: 2005/04/21 06:44:48 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Dispatcher.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Dispatcher.c,v 1.42 2005/04/21 06:44:48 jwelch Exp $";

// this is a comment

#include <stdio.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <values.h>
#include "taxipak.h"
#include "taxi_db.h"

#include "Object.h"
#include "List_public.h"
#include "Call_public.h"
#include "Call_private.h"
#include "Call_timers_public.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Fleet_db.h"
#include "Fleet_lists_public.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "Driver_public.h"
#include "Zone_public.h"
#include "Zone_private.h"
#include "Zone_db.h"
#include "Zone_lists_public.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Dispatcher_public.h"
#include "Vehicle_lists_public.h"
#include "language.h"
#include "GPS_private.h"
#include "Timer_public.h"
#include "mad_ipc.h"
#include "dispatch.h"		/* for writer.h */
#include "writer.h"
#include "timers.h"
#include "mad_error.h"
#include "except.h"
#include "enhance.h"
#include "switch_ext.h"


extern struct offsets *offset;
extern FLEET *fleet[ ];	/* shared memory fleet structure */
extern time_t mads_time;
extern short debug_state;
RET_STATUS Interlink_transfer_verify( CALL_HNDL );

#define CALL_CLASS_PUBLIC 'G'

int Dispatcher_distance_to_client( double, double, VEH_HNDL );
/*
 *	G7 change
 *	fix this
 */
/*----------------------------------------------------------
 *==>	G7 declarations
 *		these belong in other objects or files
 *----------------------------------------------------------
 */

#define ZONE_BACKUP_MAX			BCK_UP_MAX
#define ZONE_LEVEL_MAX			LEVEL_MAX

typedef short BACKUP_ZONE[ZONE_BACKUP_MAX];
typedef short INV_BACKUP_ZONE[INV_BCK_UP_MAX];

/****************************************************************************/
/* RET_STATUS Dispatcher_call_offer(veh_hndl, call_hndl, type, offer_level) */
/* OP: call_offer processing                                                */
/* PRE: call exists                                                         */
/*     vehicle exists                                                       */
/* POST: using type and the FLEET_CALL_OFFER_PROCEDURE (option)             */
/*       or if !VEH_TERM_WORKING_STATE option = '2'                         */
/*       option == '2' || type == BID_TYPE  || (option == 1 &&              */
/*       (type == PRIMARY_TYPE) || (type == STAND_TYPE))                    */
/*         Call_set_state(call_hndl, CALL_ACCEPT_STATE, NULL) called        */
/*         Veh_set_state(veh_hndl, VEH_ACCEPT_STATE, NULL) called           */
/*       option == '3' || type == EXPLICIT_ACCEPT || (option == 1 &&        */
/*       (type != PRIMARY_TYPE) || (type != STAND_TYPE))                    */
/*         Call_set_state(call_hndl, CALL_OFFERED_STATE, NULL) called       */
/*         Veh_set_state(veh_hndl, VEH_CALL_OFFER_STATE, (HNDL) offer_level)*/
/* ERROR: illegal state change                                              */
/****************************************************************************/

RET_STATUS
Dispatcher_call_offer(veh_hndl, call_hndl, type, offer_level)		/* WAS call_offer */
   VEH_HNDL veh_hndl;
   CALL_HNDL call_hndl;
   int   type;
   int   offer_level;
{
   char  option;
   char  scratch_ptr[80];
   FLEET_HNDL fl_hndl;
   struct calls *call_ptr;	/* temp */
   struct veh_driv *veh_ptr;	/* temp */

   G7_print("%.11s\tDispatcher_call_offer call %d vehicle %d level %d \n",
	    (int) Call_get_value(call_hndl, CALL_NBR),
	    (int) Veh_get_value(veh_hndl, VEH_NBR),
	    offer_level);

   switch (type)
   {
   case EXPLICIT_ACCEPT:
      option = '3';
      break;
   default:
      (void) Fleet_get_hndl(BY_NBR, (int) Call_get_value(call_hndl, CALL_FLEET_NBR), &fl_hndl);
      option = (char) Fleet_get_value(fl_hndl, FLEET_CALL_OFFER_PROCEDURE);
      break;
   }				/* end case */

   if (Veh_get_state(veh_hndl, VEH_MDT_WORKING_STATE) == FALSE)
   {
      if (Call_set_state(call_hndl, CALL_OFFERED_STATE, (HNDL) veh_hndl) == SUCCESS)
      {
	 Veh_set_state(veh_hndl, VEH_CALL_OFFER_STATE, (HNDL) call_hndl);
	 Veh_set_value(veh_hndl, VEH_CALL_OFFER_LEVEL, (HNDL) offer_level);
      }
      return (SUCCESS);
   }

   switch (option)
   {
   case '1':			/* if on a stand or in a primary zone */
      if ((type == PRIMARY_TYPE) || (type == STAND_TYPE))
      {				/* do implicit accept */
#ifdef TODO
	 Call_set_value(call_hndl, CALL_VEH_HNDL, veh_hndl);
	 Veh_set_value(veh_hndl, VEH_CALL_HNDL, call_hndl);
	 veh_ptr->call_nbr = call_ptr->call_number;
	 Veh_set_state(veh_hndl, VEH_ACCEPT_STATE, call_hndl);
#else
	 call_ptr = (struct calls *) call_hndl;
	 veh_ptr = (struct veh_driv *) veh_hndl;
	 if (call_ptr->veh_ptr != NULL)
	    ERROR(' ', "", "Call already has a vehicle");	/* return ?? */

	 call_ptr->status.available = 0;
	 call_ptr->status.offered = 1;
	 veh_ptr->t_status.offered = 1;
	 Call_set_value(call_hndl, CALL_VEH_HNDL, (HNDL) veh_hndl);	/* TEMP */
	 Veh_set_value(veh_hndl, VEH_CALL_HNDL, (HNDL) call_hndl);	/* TEMP */
	 veh_ptr->call_nbr = call_ptr->call_number;
	 Veh_set_state((VEH_HNDL) veh_ptr, VEH_ACCEPT_STATE, (HNDL) veh_ptr->call_ptr);
#endif
      } else
      {
	 if (Call_set_state(call_hndl, CALL_OFFERED_STATE, (HNDL) veh_hndl) == SUCCESS)
	 {
	    Veh_set_state(veh_hndl, VEH_CALL_OFFER_STATE, (HNDL) call_hndl);
	    Veh_set_value(veh_hndl, VEH_CALL_OFFER_LEVEL, (HNDL) offer_level);
	 }
      }
      break;
   case '2':			/* implied accept */
      call_ptr = ( struct calls * ) call_hndl;
      Call_set_value(call_hndl, CALL_VEH_HNDL, veh_hndl);
      Veh_set_value(veh_hndl, VEH_CALL_HNDL, call_hndl);
      Veh_set_value(veh_hndl, VEH_CALL_NBR, (HNDL)call_ptr->call_number);
      Veh_set_state(veh_hndl, VEH_ACCEPT_STATE, call_hndl);
      break;
   case '3':			/* explicit accept */
      if (Call_set_state(call_hndl, CALL_OFFERED_STATE, (HNDL) veh_hndl) == SUCCESS)
      {
	 Veh_set_state(veh_hndl, VEH_CALL_OFFER_STATE, (HNDL) call_hndl);
	 Veh_set_value(veh_hndl, VEH_CALL_OFFER_LEVEL, (HNDL) offer_level);
      }
      break;
   default:
      ERROR(' ', "", "bad call_offer option");
      return (FAIL);
   }
   return (SUCCESS);
}				/* end Dispatcher_call_offer */


RET_STATUS
Dispatcher_cbook_call_offer(veh_hndl, call_hndl, type, offer_level)		/* WAS call_offer */
   VEH_HNDL veh_hndl;
   CALL_HNDL call_hndl;
   int   type;
   int   offer_level;
{
   char  option;
   char  scratch_ptr[80];
   FLEET_HNDL fl_hndl;
   struct calls *call_ptr;	/* temp */
   struct veh_driv *veh_ptr;	/* temp */

   G7_print("%.11s\tDispatcher_cbook_call_offer call %d vehicle %d level %d \n",
	    (int) Call_get_value(call_hndl, CALL_NBR),
	    (int) Veh_get_value(veh_hndl, VEH_NBR),
	    offer_level);

   (void) Fleet_get_hndl(BY_NBR, (int) Call_get_value(call_hndl, CALL_FLEET_NBR), &fl_hndl);
   option = (char) Fleet_get_value(fl_hndl, FLEET_CALL_OFFER_PROCEDURE);


   if (Veh_get_state(veh_hndl, VEH_MDT_WORKING_STATE) == FALSE)
   {
      if (Call_set_state(call_hndl, CALL_OFFERED_STATE, (HNDL) veh_hndl) == SUCCESS)
      {
	 Veh_set_state(veh_hndl, VEH_CALL_OFFER_STATE, (HNDL) call_hndl);
	 Veh_set_value(veh_hndl, VEH_CALL_OFFER_LEVEL, (HNDL) offer_level);
      }
      return (SUCCESS);
   }


   if (Call_set_state(call_hndl, CALL_CBOOK_OFFERED_STATE, (HNDL) veh_hndl) == SUCCESS)
     {
       Veh_set_state(veh_hndl, VEH_CALL_OFFER_STATE, (HNDL) call_hndl);
       Veh_set_value(veh_hndl, VEH_CALL_OFFER_LEVEL, (HNDL) offer_level);
     }

   return (SUCCESS);
}				


is_backup_zones(bz)		/* part of Zone ?? */
   struct level *bz;
{
   int ii;

   for (ii = 0; ii < ZONE_BACKUP_MAX; ii++)
   {
      if (bz->bck_up_zone[ii])
	 return (TRUE);
   }
   return (FALSE);
}				/* end is_backup_zone */

/********************************************************/
/* RET_STATUS Dispatcher_call_match(call_hndl)          */
/* OP: attemps to find a vehicle to take the call       */
/*       details TBD                                    */
/* PRE: call exists and is available                    */
/* POST: Dispatcher_call_offer called if vehicle found  */
/*       If no vehicle found increments level of call   */
/* ERROR:  TBD                                          */
/********************************************************/

RET_STATUS
Dispatcher_call_match(call_hndl)	/* WAS call_match */
   CALL_HNDL call_hndl;
{
   struct calls *call_ptr;
   struct zones *tmp_zone_ptr;	/* pointer to a zone structure */
   short exclusive_time;	/* exclusive matching time */
   int   timeout;		/* timeout duration, fleet or zone */
   FLEET *fl_ptr;	/* pointer to fleet being used */
   short call_zone;		/* zone of call */
   short offer_type;
   register i;
   VEH_HNDL veh_hndl;
   struct veh_driv *veh_ptr;
   ZONE_HNDL zone_hndl;
   CALL_HNDL tmp_call_hndl;
  CALL_REC           *cl_ptr;        
   BOOLEAN found_big_car_trip = FALSE;
   time_t gps_time;

   call_ptr = (struct calls *) call_hndl;	/* temp */

   fl_ptr = fleet[call_ptr->fleet];
   call_zone = call_ptr->pickup_zone;
   if (call_zone == 0)
   {
      ERROR(' ', "", "Dispatcher call match for unzoned call");
      return (FAIL);
   }
   tmp_zone_ptr = fl_ptr->zone_ptr[call_zone];

#ifdef VPUx

   if (Veh_get_hndl(BY_NBR, 9996, 0, &veh_hndl) != FAIL)
     {
       veh_ptr = (struct veh_driv *)veh_hndl;
       if (!SIGNED_ON(veh_ptr))
	 {
	   Driver_initialize((int)1, (VEH_HNDL)veh_hndl, (char)0);
	   Veh_set_state((VEH_HNDL) veh_ptr, VEH_SIGN_IN_STATE, (HNDL) 1);
	   Driver_set_state((DRIVER_HNDL) veh_ptr, DRIVER_SIGN_IN_STATE, (HNDL) 1);
	   (void) Timer_create( TIMER_TYPE, T_ILINK_TRANSFER_VERIFY,
				TIMER_EXPIRE_TIME, 15,
				TIMER_DATA, call_hndl,
				TIMER_NOTIFY_FUNC, Interlink_transfer_verify,
				ATTRIBUTE_END );
	   return;
	 } else
	 {
	   (void) Timer_create( TIMER_TYPE, T_ILINK_TRANSFER_VERIFY,
				TIMER_EXPIRE_TIME, 15,
				TIMER_DATA, call_hndl,
				TIMER_NOTIFY_FUNC, Interlink_transfer_verify,
				ATTRIBUTE_END );
	   return;
	 }

     }

#endif
   if (call_ptr->curr_level == 0)
   {
     
      if (tmp_zone_ptr->avl_call_q_time > 0)	/* Zone_get_value(ZONE_TIMEOUT
						 * _TIME) */
	 timeout = tmp_zone_ptr->avl_call_q_time;
      else
	 timeout = fl_ptr->unassigned_time;

      if ( timeout > 0 )
	{
	  if (Call_add_timer((CALL_HNDL) call_ptr, T_TIMEOUT, timeout,
			     (int (*)())call_timeout_notify_func) != SUCCESS)
	      ERROR(' ', " ", "adding T_TIMEOUT timer");
	}
      else
        {
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
        }
   }
   if (call_ptr->status.available)
   {
      for (i = 0; i <= call_ptr->curr_level - 1; i++)
      {
	 if (call_ptr->status.wait_epost && (call_ptr->curr_level >= 1))
	    break;
	 if ((veh_hndl = Dispatcher_veh_select(call_hndl, (ZONE_HNDL) tmp_zone_ptr, i)) != HNDL_ERR)
	 {
	    if (call_ptr->curr_level == 0)
	       offer_type = STAND_TYPE;
	    else if (call_ptr->curr_level == 1)
	       offer_type = PRIMARY_TYPE;
	    else
	       offer_type = BACKUP_TYPE;

	    Dispatcher_call_offer(veh_hndl, (CALL_HNDL) call_ptr, offer_type, i);
	    return (SUCCESS);
	 }
         if ( i == 1 )  // if no match on first level, hold for a STC taxi
           {
             (void) Zone_get_hndl(call_zone, call_ptr->fleet, &zone_hndl);
             Zone_check_epost_vehs(zone_hndl, (CALL_HNDL)call_ptr);
           }         
         
      }
   }
   /* no match made, schedule to match again */

   do
   {
      /* only offer held calls in levels 0 & 1 */
      if ((call_ptr->status.available) && !(call_ptr->status.wait_epost && (call_ptr->curr_level >= 1)))
      {
	 if ((veh_hndl = Dispatcher_veh_select(call_hndl, (ZONE_HNDL) tmp_zone_ptr, call_ptr->curr_level)) != NULL)
	 {
	    if (call_ptr->curr_level == 0)
	       offer_type = STAND_TYPE;
	    else if (call_ptr->curr_level == 1)
	       offer_type = PRIMARY_TYPE;
	    else
	       offer_type = BACKUP_TYPE;
	    Dispatcher_call_offer(veh_hndl, (CALL_HNDL) call_ptr, offer_type, call_ptr->curr_level);
	    return (SUCCESS);
	 }

      }

      if (tmp_zone_ptr->level_exclusive_time[call_ptr->curr_level] != 0)
	 exclusive_time = tmp_zone_ptr->level_exclusive_time[call_ptr->curr_level];
      else
	 exclusive_time = fl_ptr->level_exclusive_time[call_ptr->curr_level];

      /* if call is offered at level greater than fleet level */
      if ((fl_ptr->zone_backup_level <= call_ptr->curr_level) || (call_ptr->curr_level >= ZONE_BACKUP_MAX))
	{
	  if ( call_ptr->FTJ_nbr == -1 ) //haven't done check for out-of-shift vehicles yet
	    {
	      call_ptr->FTJ_nbr = 0;
	      call_ptr->curr_level = 0; // reset search level
	      continue;
	    }
	  else if ( call_ptr->FTJ_nbr == 0 ) // haven't passed to Interlink yet
	    {
	      call_ptr->FTJ_nbr = 1;
	      call_ptr->curr_level = 0; // reset search level to primary
	      continue;
	    }
	    return (FAIL);		/* do nothing */
	}

      if ( ( (short)Call_get_value( call_hndl, CALL_TYPE_ILINK ) == TRUE ) )
	{
	  if ( Call_rec_alloc((CALL_REC_HNDL *) & cl_ptr ) != SUCCESS )
	    {
	      ERROR(' ', "", "Call_rec_alloc failure InterLink_transfer_notify");
	    }
	  else
	    {
	      cl_ptr->nbr = (int)Call_get_value( call_hndl, CALL_NBR );
	      if ( Call_rec_read((CALL_REC_HNDL) cl_ptr, DB_EQUAL | DB_LOCK, CALL_KEY1 ) != SUCCESS )
		{
		  ERROR(' ', "", "Call_rec_read failure InterLink_transfer_notify");
		  Call_release_locks();
		  Call_rec_free((CALL_REC_HNDL *) & cl_ptr );
		  return;
		}
	      else
		{
		  if (strncmp(cl_ptr->extended_type, "M8",2)) // TaxiPak trip that was transferred to Interlink - stop dispatch
		    {
		      Call_release_locks();
		      Call_rec_free((CALL_REC_HNDL *) & cl_ptr );		      		      
		      return;
		    }
		  Call_release_locks();
		  Call_rec_free((CALL_REC_HNDL *) & cl_ptr );		      
		}
	    }	      
	}
      
      /* If Interlink trip and we couldn't find a match...cancel the trip here */      
          if ( ( ( (short)Call_get_value( call_hndl, CALL_TYPE_ILINK ) == TRUE ) && ((short)Call_get_value(call_hndl, CALL_NBR_REJECT) >= 1)) ||
	       ( (call_ptr->status.available) && ( call_ptr->curr_level == (fl_ptr->zone_backup_level - 1))) )
            {
	      // Verify that the trip originated from T800
	      if ( Call_rec_alloc((CALL_REC_HNDL *) & cl_ptr ) != SUCCESS )
		{
		  ERROR(' ', "", "Call_rec_alloc failure InterLink_transfer_notify");
		}
	      else
		{
		  cl_ptr->nbr = (int)Call_get_value( call_hndl, CALL_NBR );
		  if ( Call_rec_read((CALL_REC_HNDL) cl_ptr, DB_EQUAL | DB_LOCK, CALL_KEY1 ) != SUCCESS )
		    {
		      ERROR(' ', "", "Call_rec_read failure InterLink_transfer_notify");
		      Call_release_locks();
		      Call_rec_free((CALL_REC_HNDL *) & cl_ptr );
		      return;
		    }
		  else
		    {
		      if (!strncmp(cl_ptr->extended_type, "M8",2)) //
			{
			  InterLink_timeout_notify( call_hndl );
			  Call_cancel( call_hndl, HNDL_NULL, TRUE, FALSE );
			}
		      Call_release_locks();
		      Call_rec_free((CALL_REC_HNDL *) & cl_ptr );		      
		    }
		}
            }
      
      if (exclusive_time != 0)
      {
	 if (is_backup_zones((struct level *) & tmp_zone_ptr->level_number[call_ptr->curr_level]))
	 {

	    /* add an event to restart the matching */

	    if (Call_add_timer((CALL_HNDL) call_ptr, T_ZONE_TIMEOUT,
		  exclusive_time, (int (*)())call_zone_timeout_notify_func) != SUCCESS)
	    {
	       ERROR(' ', " ", "adding T_ZONE_TIMEOUT timer");
	    }
	 } else
	    exclusive_time = 0;	/* must trick loop to go to next */

      }
      
      // hold for STC taxi after we get past primary zone
      if ( call_ptr->curr_level == 1 )
        {
          (void) Zone_get_hndl(call_zone, call_ptr->fleet, &zone_hndl);
          Zone_check_epost_vehs(zone_hndl, (CALL_HNDL)call_ptr);
        }
      
      call_ptr->curr_level += 1;/* increase match level */
   } while (exclusive_time == 0);


   
   return (FAIL);
}				/* end Dispatcher_call_match */

/*******************************************************************/
/* VEH_HNDL Dispatcher_veh_select(call_hndl, zone_hndl level)      */
/* OP: attempts to select a vehicle to take the call               */
/*     one of three alogrithm used selected base on                */
/*     the algorithm specified for the zone at the specified level */
/*     if none the defaaut algorithm for the fleet is used         */
/*     Algorithm details are:                                      */
/*         TBD                                                     */
/* PRE: match algorith avaialble                                   */
/*      call exists                                                */
/* POST:  slects a best vehicle if there is one else returns NULL  */
/* ERROR: TBD                                                      */
/*******************************************************************/

VEH_HNDL
Dispatcher_veh_select(CALL_HNDL call_hndl, ZONE_HNDL zone_hndl, short level)
{
   struct calls *call_ptr;
   struct zones *zone_ptr;
   struct veh_driv *veh_ptr;
   struct veh_driv *best_match;	/* best matched cab using specified algorithm */
   register i, j;
   time_t best_time;		/* earliest time of a posted cab */
   int   zone_nbr;		/* zone number */
   int   call_zone;		/* zone number where call is */
   int   fleet_nbr;		/* fleet number */
   int   q_pos = 0;		/* queue position being looked at */
   short flag = 1;
   int   option;		/* taxi algorithm type */
   ZONE_VEH_LIST_HNDL veh_list_hndl;
   VEH_HNDL veh_hndl;
  CALL_REC           *cl_ptr;           
   FLEET *fl_ptr;	/* pointer to fleet being used */
   
   call_ptr = (struct calls *) call_hndl;
   if (call_ptr == NULL)
   {
      ERROR(' ', "", "Null call");
      return (HNDL_ERR);
   }
   zone_ptr = (struct zones *) zone_hndl;
   if (zone_ptr == NULL)
   {
      ERROR(' ', "", "Null zone");
      return (HNDL_ERR);
   }

   best_match = NULL;
   best_time = mads_time;	/* default time */
   fleet_nbr = call_ptr->fleet;	/* get fleet number to work in */

   if ((option = zone_ptr->level_algorithm[level]) <= '0')	/* see if a zone alg
								 * exists */
      option = fleet[fleet_nbr]->matching_algorithm;	/* by default use fleet
							 * alg */

   fl_ptr = fleet[call_ptr->fleet];       
   if ( ( ( (short)Call_get_value( call_hndl, CALL_TYPE_ILINK ) == TRUE ) && ((short)Call_get_value(call_hndl, CALL_NBR_REJECT) >= 1)) ||
	( (call_ptr->status.available) && ( call_ptr->curr_level == (fl_ptr->zone_backup_level - 1))) )
     {
       // Verify that the trip originated from T800

       if ( Call_rec_alloc((CALL_REC_HNDL *) & cl_ptr ) != SUCCESS )
	 {
	   ERROR(' ', "", "Call_rec_alloc failure InterLink_transfer_notify");
	 }
       else
	 {
	   cl_ptr->nbr = (int)Call_get_value( call_hndl, CALL_NBR );
	   if ( Call_rec_read((CALL_REC_HNDL) cl_ptr, DB_EQUAL | DB_LOCK, CALL_KEY1 ) != SUCCESS )
	     {
	       ERROR(' ', "", "Call_rec_read failure InterLink_transfer_notify");
	       Call_release_locks();
	       Call_rec_free((CALL_REC_HNDL *) & cl_ptr );
	       return;
	     }
	   else
	     {
	       if (!strncmp(cl_ptr->extended_type, "M8",2)) //
		 {
		   InterLink_timeout_notify( call_hndl );
		   Call_cancel( call_hndl, HNDL_NULL, TRUE, FALSE );
		 }
	       Call_release_locks();
	       Call_rec_free((CALL_REC_HNDL *) & cl_ptr );		      
	     }
	 }
     }
   
   
	      
   call_zone = zone_ptr->level_number[1].bck_up_zone[0];

   Dispatcher_Nordic_veh_select(call_hndl, zone_hndl,&best_match, level);

   return ((VEH_HNDL) best_match);	/* if veh_hndl  not found, return
					 * NULL pointer */
}				/* end  Dispatcher_veh_select */

/*****************************************************************/
/* RET_STATUS Dispatcher_veh_match(veh_hndl, zone_nbr)           */
/* OP:                                                           */
/* PRE:                                                          */
/* POST:                                                         */
/* ERROR: TBD - FAIL if no match                                 */
/*****************************************************************/
 /* ADD ABILITY TO MATCH WITH CALLS UP FFOR BID */
RET_STATUS
Dispatcher_veh_match(veh_hndl, zone_nbr)	/* WAS Dispatcher_veh_match */
   VEH_HNDL veh_hndl;
   short    zone_nbr;
{
   return (Dispatcher_Nordic_vehicle_to_call(veh_hndl, zone_nbr));

}


/*---------------------------------------------------------
 *==>	G7 Functions
 *---------------------------------------------------------
 */

/*---------------------------------------------------------
 *==>	Dispatcher G7 veh select
 *
 *	OP:
 *		Call is entered into a zone.
 *		This function gets called for each search level of
 *		the backup levels for the primary zone.
 *		If an appropriate vehicle is found, then it is returned.
 *		Each backup zone in the level is checked for compatible
 *		vehicles, in the order of the backup zones.
 *		Returns FAIL is not vehicle is found.
 *
 *	PRE:
 *	POST:
 *	ERROR:
 *---------------------------------------------------------
 */
RET_STATUS
Dispatcher_Nordic_veh_select(
			 CALL_HNDL call_hndl,
			 ZONE_HNDL zone_hndl,
			 VEH_HNDL * veh_hndl,
			 short level)
{
   int   i, zone_nbr, fleet_nbr, vehicles_in_zone;
   FLEET_HNDL  fleet_hndl;
   BACKUP_ZONE backup_zones;
   ZONE_HNDL backup_zone;
   ZONE_VEH_LIST_HNDL
      zone_vehicle_list;
   VEH_HNDL vehicle_in_zone;
   VEH      *veh_ptr;
   ZONE     *zone;
   CALL *CalltoMatch;
   FILE      *fpGPSDispatchTrace;
   char      *ctime(), *stime;
   time_t    tmtime;
   char      match_algorithm;

   G7_print("%.11s\tDispatcher_Nordic_veh_select: call %d level %d\n",
	    (int) Call_get_value(call_hndl, CALL_NBR), level);

   CalltoMatch = ( CALL * ) call_hndl;

  if ( Fleet_get_hndl( BY_NBR, (short)Call_get_value(call_hndl,CALL_FLEET_NBR), &fleet_hndl ) == FAIL )
      return(FAIL);

   /* Get Zone Value for matching algorithm as this overrides the fleet setting */
   match_algorithm = (char)Zone_get_value( zone_hndl, ZONE_MATCH_ALGORITHM );
   if ( ( match_algorithm == FLEET_ALG_CLOSEST_TAXI ) || ( match_algorithm == FLEET_ALG_ZONE_QUEUE_PRIMARY_CLOSEST_BACKUP ) ||
        ( match_algorithm == FLEET_ALG_ZONE_FIFO ) )
     {
       switch ( match_algorithm )
	 {
	 case FLEET_ALG_CLOSEST_TAXI:
	   if ( Dispatcher_closest_match( (CALL_HNDL)call_hndl, (ZONE_HNDL)zone_hndl, &vehicle_in_zone  ) == SUCCESS )
	     {
	       *veh_hndl = vehicle_in_zone;	   
	       return ( SUCCESS );
	     }
	   break;
	 case FLEET_ALG_ZONE_QUEUE_PRIMARY_CLOSEST_BACKUP:
	   if ( Dispatcher_fifo_primary_closest_backup( (CALL_HNDL)call_hndl, (ZONE_HNDL)zone_hndl, &vehicle_in_zone ) == SUCCESS )
	     {
	       *veh_hndl = vehicle_in_zone;
	       return ( SUCCESS );
	     }
	   break;
	 default:
	   break;
	 }
     }
   else
     {
       match_algorithm = (char)Fleet_get_value( (FLEET_HNDL)fleet_hndl, FLEET_MATCH_ALGORITHM );
       switch ( match_algorithm )
	 {
	 case FLEET_ALG_CLOSEST_TAXI:
	   if ( Dispatcher_closest_match( (CALL_HNDL)call_hndl, (ZONE_HNDL)zone_hndl, &vehicle_in_zone  ) == SUCCESS )
	     {
	       *veh_hndl = vehicle_in_zone;	   
	       return ( SUCCESS );
	     }
	   break;
	 case FLEET_ALG_ZONE_QUEUE_PRIMARY_CLOSEST_BACKUP:
	   if ( Dispatcher_fifo_primary_closest_backup( (CALL_HNDL)call_hndl, (ZONE_HNDL)zone_hndl, &vehicle_in_zone ) == SUCCESS )
	     {
	       *veh_hndl = vehicle_in_zone;
	       return ( SUCCESS );
	     }
	   break;
	 default:
	   break;
	 }
     }

   /** note: if other taxi matching algorithms fails, fall through to traditional FIFO queue method **/

   // Checking all zones at stand level and sorting by queue order
   zone = (ZONE *)zone_hndl;
   if ( zone != NULL )
     {
       if ( zone->level_algorithm[0] == FLEET_ALG_ZONE_FIFO )
	 {
	   if ( Dispatcher_fifo_by_zone_queue_order( (CALL_HNDL)call_hndl, (ZONE_HNDL)zone_hndl, &vehicle_in_zone ) == SUCCESS )
	     {
	       *veh_hndl = vehicle_in_zone;
	       return (SUCCESS);
	     }
	 }
       else if ( zone->level_algorithm[0] == '1' )
	 {
	   if ( Dispatcher_temporal_by_zone_level( (CALL_HNDL) call_hndl, (ZONE_HNDL)zone_hndl, &vehicle_in_zone ) == SUCCESS )
	     {
	       *veh_hndl = vehicle_in_zone;
	       return (SUCCESS);
	     }
	 }
     }
   
   /*
    * get call FLEET_NBR
    */
   Zone_get_info(zone_hndl, ZONE_FLEET_NBR, (HNDL *) (&fleet_nbr));

   /*
    * get backup zones
    */
   for ( i=0; i< ZONE_BACKUP_MAX; i++ )
     backup_zones[i] = 0;
   
   Zone_get_backup_zone(zone_hndl, level, &backup_zones[0]);

   /*
    * for each backup zone
    */
   for (i = 0; i < ZONE_BACKUP_MAX; i++)
   {
      zone_nbr = backup_zones[i];
      if (zone_nbr <= 0)
	 continue;


      // HELSINKI ONLY - if we retrieve zone 888 this is the Interlink transfer zone
      // send the order to T800 and deactivate in TaxiPak
      if ( ( zone_nbr == 888 ) &&
           ( (char)Fleet_get_value( fleet_hndl, FLEET_ILINK_ACTIVE ) == YES ) &&
           ( (short)Call_get_value( call_hndl, CALL_TYPE_ILINK_REJECT ) == FALSE ) &&
           ( (short)Call_get_value( call_hndl, CALL_TYPE_ILINK ) == FALSE ) &&
	   ( (short)Call_get_value( call_hndl, CALL_TYPE_TIME_CALL ) == FALSE ) &&
	   ( CalltoMatch->FTJ_nbr == 1 ) )
        {
          InterLink_transfer_notify( call_hndl );
	  Call_set_value( call_hndl, CALL_TYPE_ILINK, (HNDL)TRUE );
	  (void) Timer_create(
			      TIMER_TYPE, T_ILINK_TRANSFER_VERIFY,
			      TIMER_EXPIRE_TIME, 30,
			      TIMER_DATA, call_hndl,
			      TIMER_NOTIFY_FUNC, Interlink_transfer_verify,
			      ATTRIBUTE_END );
	  
	  //          Call_deactivate( call_hndl );
          return( FAIL );
        }
      
      if ( zone_nbr == 888 )
        continue;
      
      G7_print("%.11s\t  ...checking zone %d for call %d\n", zone_nbr, (int) Call_get_value(call_hndl, CALL_NBR));

      backup_zone = HNDL_ERR;
      Zone_get_hndl(zone_nbr, fleet_nbr, (ZONE_HNDL *) & backup_zone);
      if (backup_zone == HNDL_ERR)
	 continue;



      /*
       * for each vehicle in this zone
       */
      Zone_get_info(backup_zone, ZONE_AVAIL_VEH_LIST, &zone_vehicle_list);

      Zone_veh_list_each_get(zone_vehicle_list, vehicle_in_zone)
      {

	if (Dispatcher_vehicle_call_compatible(call_hndl, vehicle_in_zone, zone_hndl))
	  {
	    if ((CalltoMatch->premium_shift_match_state == PREMIUM_INSHIFT ||
                 CalltoMatch->premium_shift_match_state == NON_PREMIUM ||
                 CalltoMatch->premium_shift_match_state == OPEN_INSHIFT) &&
		Vehicle_is_Valid_Shift( vehicle_in_zone, call_hndl ))
	      {
		if ( ( fpGPSDispatchTrace = fopen( GPS_TRACE_FILE, "a" ) ) != NULL )
		  {
		    tmtime = time( (time_t *) 0 );
		    stime = ctime( &tmtime );
		    veh_ptr = (VEH *)vehicle_in_zone;
		    fprintf( fpGPSDispatchTrace, "%.8s Call %08d Zone %03d CALL->VEHICLE (FIFO-x) ",
			    &stime[11], CalltoMatch->call_number, CalltoMatch->pickup_zone);
		    
		    fprintf( fpGPSDispatchTrace, "Veh %04d Zone %03d QP %02d Level %d INSHIFT PR %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n",
			    (short)Veh_get_value( vehicle_in_zone, VEH_NBR ),
			    (short)Veh_get_value( vehicle_in_zone, VEH_ZONE_NBR ),
			    (short)Veh_get_value( vehicle_in_zone, VEH_Q_POSITION ),
			     level,
			     (short)CalltoMatch->driver_attributes.attr16,
			     (short)CalltoMatch->driver_attributes.attr17,
			     (short)CalltoMatch->driver_attributes.attr18,
			     (short)CalltoMatch->driver_attributes.attr19,
			     (short)CalltoMatch->driver_attributes.attr20,
			     (short)CalltoMatch->driver_attributes.attr21,
			     (short)CalltoMatch->driver_attributes.attr22,
			     (short)CalltoMatch->driver_attributes.attr23,
			     (short)CalltoMatch->driver_attributes.attr24,
			     (short)CalltoMatch->driver_attributes.attr25,
			     (short)CalltoMatch->driver_attributes.attr26,
			     (short)CalltoMatch->driver_attributes.attr27,
			     (short)CalltoMatch->driver_attributes.attr28,
			     (short)CalltoMatch->driver_attributes.attr29,
			     (short)CalltoMatch->driver_attributes.attr30,
			     (short)CalltoMatch->driver_attributes.attr31,
			     (short)CalltoMatch->driver_attributes.attr32);

		    
		    
		    fclose( fpGPSDispatchTrace );
		  }	   
		*veh_hndl = vehicle_in_zone;
		return (SUCCESS);
	      }
	    else if ((CalltoMatch->premium_shift_match_state == PREMIUM_OUTSHIFT ||
		      CalltoMatch->premium_shift_match_state == NON_PREMIUM_OUTSHIFT ||
		      CalltoMatch->premium_shift_match_state == OPEN_OUTSHIFT))
	      {
		if ( ( fpGPSDispatchTrace = fopen( GPS_TRACE_FILE, "a" ) ) != NULL )
		  {
		    tmtime = time( (time_t *) 0 );
		    stime = ctime( &tmtime );
		    veh_ptr = (VEH *)vehicle_in_zone;
		    fprintf( fpGPSDispatchTrace, "%.8s Call %08d Zone %03d CALL->VEHICLE (FIFO-x) ",
			    &stime[11], CalltoMatch->call_number, CalltoMatch->pickup_zone);
		    
		    fprintf( fpGPSDispatchTrace, "Veh %04d Zone %03d QP %02d Level %d OUTSHIFT PR %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n",
			    (short)Veh_get_value( vehicle_in_zone, VEH_NBR ),
			    (short)Veh_get_value( vehicle_in_zone, VEH_ZONE_NBR ),
			    (short)Veh_get_value( vehicle_in_zone, VEH_Q_POSITION ),
			     level,
			     (short)CalltoMatch->driver_attributes.attr16,
			     (short)CalltoMatch->driver_attributes.attr17,
			     (short)CalltoMatch->driver_attributes.attr18,
			     (short)CalltoMatch->driver_attributes.attr19,
			     (short)CalltoMatch->driver_attributes.attr20,
			     (short)CalltoMatch->driver_attributes.attr21,
			     (short)CalltoMatch->driver_attributes.attr22,
			     (short)CalltoMatch->driver_attributes.attr23,
			     (short)CalltoMatch->driver_attributes.attr24,
			     (short)CalltoMatch->driver_attributes.attr25,
			     (short)CalltoMatch->driver_attributes.attr26,
			     (short)CalltoMatch->driver_attributes.attr27,
			     (short)CalltoMatch->driver_attributes.attr28,
			     (short)CalltoMatch->driver_attributes.attr29,
			     (short)CalltoMatch->driver_attributes.attr30,
			     (short)CalltoMatch->driver_attributes.attr31,
			     (short)CalltoMatch->driver_attributes.attr32);			     

		    
		    fclose( fpGPSDispatchTrace );
		  }	   
		*veh_hndl = vehicle_in_zone;
		return (SUCCESS);		
	      }
	  }
      } Zone_veh_list_each_end(zone_vehicle_list)

    }


   /* state transitions */
   switch (CalltoMatch->premium_shift_match_state) {
   case PREMIUM_INSHIFT:
     if (level == 2)
       {
	 CalltoMatch->premium_shift_match_state = PREMIUM_OUTSHIFT;
	 CalltoMatch->curr_level = 0;
       }
     break;
   case PREMIUM_OUTSHIFT:
     if (level == 2)
       {
	 CalltoMatch->premium_shift_match_state = OPEN_INSHIFT;
	 CalltoMatch->vehicle_attributes.attr26=0;CalltoMatch->driver_attributes.attr26=0;
	 CalltoMatch->curr_level = 0;	 
       }
     break;
   case OPEN_INSHIFT:
     if (level == 8)
       {
	 CalltoMatch->premium_shift_match_state = OPEN_OUTSHIFT;
	 CalltoMatch->curr_level = 0;
       }
     break;
   case NON_PREMIUM: 
     if (level == 8)
       {
	 CalltoMatch->premium_shift_match_state = NON_PREMIUM_OUTSHIFT;
	 CalltoMatch->curr_level = 0;
       }
     break;
   case OPEN_OUTSHIFT:
   case NON_PREMIUM_OUTSHIFT:
   default:
     ;
   }
   
   return (FAIL);
}


/*---------------------------------------------------------
 *==>	Dispatcher vehicle call compatible
 *
 *	OP:
 *		Checks the state and attributes of a vehicle and a call
 *		for compatibility for matching.
 *		Returns TRUE if compatible and FALSE if not.
 *
 *	PRE:
 *	POST:
 *	ERROR:
 *---------------------------------------------------------
 */
BOOLEAN
Dispatcher_vehicle_call_compatible(
				   CALL_HNDL call_hndl,
				   VEH_HNDL veh_hndl,
				   ZONE_HNDL zone_hndl)
{
   int   zone_nbr;
   char  call_match_class, driver_class;
   struct veh_driv  *veh_ptr;
   char tmp_str[512];

   G7_print("%.11s\tDispatcher_vehicle_call_compatible call %d vehicle %d\n",
	    (int) Call_get_value(call_hndl, CALL_NBR),(int) Veh_get_value(veh_hndl, VEH_NBR));

   /*
    * check call state
    */

   veh_ptr = (struct veh_driv *)veh_hndl;

   if ( (short)Call_get_value( call_hndl, CALL_FLEET_NBR ) !=
        (short)Veh_get_value( veh_hndl, VEH_FLEET_NBR ) )
     {
       sprintf(tmp_str, "Veh/Call mismatch-prevented dispatch(cl_nbr %d cl_fleet %d/vh_nbr %d vh_fleet %c)",
	       (int)Call_get_value(call_hndl, CALL_NBR), (char)Call_get_value( call_hndl, CALL_FLEET_NBR ),
	       (int)Veh_get_value(veh_hndl, VEH_NBR), (char)Veh_get_value(veh_hndl, VEH_FLEET));
       ERROR(' ',"",tmp_str);
       return (FALSE);
     }
   
   if ( (!Call_get_state(call_hndl, CALL_AVAILABLE_STATE)) ||
        (Call_get_state(call_hndl, CALL_HOLD_STATE)) )
   {
      G7_print("%.11s\t  ...call state not available. call %d vehicle %d\n",
	       (int)Call_get_value(call_hndl, CALL_NBR),
	       (int)Veh_get_value(veh_hndl, VEH_NBR));
      return (FALSE);
   }
   /*
    * check vehicle state
    */


   if (!Veh_get_state(veh_hndl, VEH_OK_TO_MATCH_STATE))
   {
      G7_print("%.11s\t  ...vehicle state not ok to match. call %d vehicle %d\n",
	       (int)Call_get_value(call_hndl, CALL_NBR),
	       (int)Veh_get_value(veh_hndl, VEH_NBR));
      return (FALSE);
   }
   if (Veh_get_state(veh_hndl, VEH_ON_BREAK_STATE))
   {
      G7_print("%.11s\t  ...vehicle state on break. call %d vehicle %d\n",
	       (int)Call_get_value(call_hndl, CALL_NBR),
	       (int)Veh_get_value(veh_hndl, VEH_NBR));
      return (FALSE);
   }
   /*
    * check vehicle attributes
    */
   if (!Veh_check_attr(veh_hndl, call_hndl, FALSE))
   {
      G7_print("%.11s\t  ...vehicle wrong attributes. call %d vehicle %d\n",
	       (int)Call_get_value(call_hndl, CALL_NBR),
	       (int)Veh_get_value(veh_hndl, VEH_NBR));
      return (FALSE);
   }
   /*
    * check vehicle seals G7 does not check for seals
    * */
    Zone_get_info(zone_hndl, ZONE_NBR, (HNDL *)&zone_nbr);
    //    if (!Veh_check_seals(veh_hndl, zone_nbr))
    //      return(FALSE);


   G7_print("%.11s\t  ...call and vehicle compatible. call %d vehicle %d\n",
	    (int)Call_get_value(call_hndl, CALL_NBR),
	    (int)Veh_get_value(veh_hndl, VEH_NBR));

   return (TRUE);
}

/*---------------------------------------------------------
 *==>	Dispatcher epost vehicle call compatible
 *
 *	OP:
 *		Checks the state and attributes of an epost vehicle and a call
 *		for compatibility for matching.
 *		Returns TRUE if compatible and FALSE if not.
 *
 *	PRE:
 *	POST:
 *	ERROR:
 *---------------------------------------------------------
 */
BOOLEAN
Dispatcher_epost_vehicle_call_compatible(
				   CALL_HNDL call_hndl,
				   VEH_HNDL veh_hndl,
				   ZONE_HNDL zone_hndl)
{
   int   zone_nbr;
   char  call_match_class, driver_class;
   struct veh_driv  *veh_ptr;
   char tmp_str[512];

   G7_print("%.11s\tDispatcher_vehicle_call_compatible call %d vehicle %d\n",
	    (int) Call_get_value(call_hndl, CALL_NBR),(int) Veh_get_value(veh_hndl, VEH_NBR));

   /*
    * check call state
    */

   veh_ptr = (struct veh_driv *)veh_hndl;

   if ( (short)Call_get_value( call_hndl, CALL_FLEET_NBR ) !=
        (short)Veh_get_value( veh_hndl, VEH_FLEET_NBR ) )
     {
       sprintf(tmp_str, "Veh/Call mismatch-prevented dispatch(cl_nbr %d cl_fleet %d/vh_nbr %d vh_fleet %c)",
	       (int)Call_get_value(call_hndl, CALL_NBR), (char)Call_get_value( call_hndl, CALL_FLEET_NBR ),
	       (int)Veh_get_value(veh_hndl, VEH_NBR), (char)Veh_get_value(veh_hndl, VEH_FLEET));
       ERROR(' ',"",tmp_str);
       return (FALSE);
     }
   
   if ( (!Call_get_state(call_hndl, CALL_AVAILABLE_STATE)) ||
        (Call_get_state(call_hndl, CALL_HOLD_STATE)) )
   {
      G7_print("%.11s\t  ...call state not available. call %d vehicle %d\n",
	       (int)Call_get_value(call_hndl, CALL_NBR),
	       (int)Veh_get_value(veh_hndl, VEH_NBR));
      return (FALSE);
   }

   if (Veh_get_state(veh_hndl, VEH_ON_BREAK_STATE))
   {
      G7_print("%.11s\t  ...vehicle state on break. call %d vehicle %d\n",
	       (int)Call_get_value(call_hndl, CALL_NBR),
	       (int)Veh_get_value(veh_hndl, VEH_NBR));
      return (FALSE);
   }
   /*
    * check vehicle attributes
    */
   if (!Veh_check_attr(veh_hndl, call_hndl, FALSE))
   {
      G7_print("%.11s\t  ...vehicle wrong attributes. call %d vehicle %d\n",
	       (int)Call_get_value(call_hndl, CALL_NBR),
	       (int)Veh_get_value(veh_hndl, VEH_NBR));
      return (FALSE);
   }
   /*
    * check vehicle seals G7 does not check for seals
    * */
    Zone_get_info(zone_hndl, ZONE_NBR, (HNDL *)&zone_nbr);
    //    if (!Veh_check_seals(veh_hndl, zone_nbr))
    //      return(FALSE);


   G7_print("%.11s\t  ...call and vehicle compatible. call %d vehicle %d\n",
	    (int)Call_get_value(call_hndl, CALL_NBR),
	    (int)Veh_get_value(veh_hndl, VEH_NBR));

   return (TRUE);
}



/*---------------------------------------------------------
 *==>	Dispatcher G7 vehicle to call
 *
 *	OP:
 *		A vehicle books into a zone.
 *		Search for a compatible call using the G7 defined
 *		algorithm for seaching backup zone levels
 *		First check call class, then call appropriate find()
 *		function to find compatible call.
 *		If a compatible call is found, an offer is made to the
 *		vehicle, and return SUCCESS.  If not, then return FAIL.
 *
 *	PRE:
 *	POST:
 *	ERROR:
 *---------------------------------------------------------
 */
RET_STATUS
Dispatcher_Nordic_vehicle_to_call(
			      VEH_HNDL veh_hndl,
			      short    zone_nbr)
{
   char  driver_class;
   short   fleet_nbr;
   ZONE_HNDL zone_hndl;
   RET_STATUS rc;




   fleet_nbr = (short)Veh_get_value( veh_hndl, VEH_FLEET_NBR );

   Zone_get_hndl(zone_nbr, fleet_nbr, (ZONE_HNDL *) & zone_hndl);
   if (zone_hndl == HNDL_ERR)
      return (FAIL);

   G7_print("%.11s\tDispatcher_Nordic_vehicle_to_call vehicle %d zone %d\n",
	    (int) Veh_get_value(veh_hndl, VEH_NBR), zone_nbr );
   
   rc = Dispatcher_Nordic_find_public_call(veh_hndl, zone_hndl);

   return (rc);
}

/*---------------------------------------------------------
 *==>	Dispatcher Nordic find public call
 *
 *	OP:
 *		Given a vehicle, and a PRIMARY zone, search for a general public
 *		compatible call using the NORDIC defined algorithm.
 *
 *		First check the STAND for the PRIMARY zone for SUBSCRIBER call.
 *		Next check the PRIMARY zone for SUBSCRIBER call.
 *		Next check the 1st level BACKUP zone for SUBSCRIBER call.
 *
 *		Next check the STAND for the PRIMARY zone for PUBLIC call.
 *		Next check the PRIMARY zone for PUBLIC call.
 *		Next check the 1st level BACKUP zone for PUBLIC call.
 *
 *		Next check the levels 2-7 BACKUP zones for SUBSCRIBER call.
 *		Next check the levels 2-7 BACKUP zones for PUBLIC call.
 *
 *		If a compatible call is found, an offer is made to the
 *		vehicle, and return SUCCESS.  If not, then return FAIL.
 *
 *	PRE:
 *	POST:
 *	ERROR:
 *---------------------------------------------------------
 */
RET_STATUS
Dispatcher_Nordic_find_public_call(
			       VEH_HNDL veh_hndl,
			       ZONE_HNDL zone_hndl)
{
   RET_STATUS rc;
   CALL_HNDL call_in_zone = HNDL_NULL;
   int   backup_max;
   short level;
   FLEET_HNDL    fleet_hndl;
   ZONE_HNDL     veh_zone_hndl = HNDL_NULL;
   char      tmp_str[512];

   Zone_get_hndl( (short)Veh_get_value( veh_hndl, VEH_ZONE_NBR ),
		  (short)Zone_get_value( zone_hndl, ZONE_FLEET_NBR ), &veh_zone_hndl );

   if ( veh_zone_hndl == HNDL_NULL )
     return( FAIL );

   zone_hndl = veh_zone_hndl;
   
   G7_print("%.11s\tDispatcher_Nordic_find_public_call zone %d  vehicle's zone %d\n",
	    (int)Zone_get_value( zone_hndl, ZONE_NBR ), (int)Zone_get_value( veh_zone_hndl, ZONE_NBR ));
   
   Fleet_get_hndl( BY_NBR, (short)Zone_get_value( zone_hndl, ZONE_FLEET_NBR ), &fleet_hndl );
   
   /*
    * get fleet backup levels
    */
   rc = Dispatcher_get_zone_backup_max(zone_hndl, &backup_max);
   if (rc != SUCCESS)
   {
      G7_print("%.11s\t  ...error getting backup max\n");
      return (rc);
   }
   /*
    * check STAND (level 0) backups for CALL_CLASS_PUBLIC
    */
   rc = Dispatcher_Nordic_look_in_backup_levels(
				 veh_hndl, zone_hndl, 0, CALL_CLASS_PUBLIC);
   if (rc == SUCCESS)
   {
      G7_print("%.11s\t  ...found class = %c in stand\n", CALL_CLASS_PUBLIC);
      return (rc);
   }
   /*
    * check PRIMARY (internal level 1) for CALL_CLASS_PUBLIC
    */
   if (Dispatcher_Nordic_look_in_zone(
	  zone_hndl, veh_hndl, CALL_CLASS_PUBLIC, &call_in_zone) == SUCCESS)
   {
     /* verify that vehicle's zone is a backup of the call's zone using zone table */
     if ( (short)Veh_get_value(veh_hndl, VEH_ZONE_NBR) != (short)Call_get_value(call_in_zone, CALL_PICKUP_ZONE) )
       {
	 sprintf( tmp_str, "Veh_disp prob: cl_nbr %d cl_zone %d vh_nbr %d vh_zn %d",
		 (int)Call_get_value(call_in_zone, CALL_NBR),
		 (short)Call_get_value(call_in_zone, CALL_PICKUP_ZONE),			       
		 (int)Veh_get_value( veh_hndl, VEH_NBR ),
		 (short)Veh_get_value( veh_hndl, VEH_ZONE_NBR ) );
#ifdef FOO
	 ERROR(' ', "", tmp_str);
#endif
	 Zone_maint( 0, (short)Call_get_value(call_in_zone, CALL_PICKUP_ZONE),
		    (char)Veh_get_value( veh_hndl, VEH_FLEET ) );
       }
     else
       {
	 Dispatcher_trace_call_offer( veh_hndl, call_in_zone, (short)1 );
	 Dispatcher_call_offer(veh_hndl, call_in_zone, PRIMARY_TYPE, 1);
	 G7_print("%.11s\t  ...found class = %c in primary\n", CALL_CLASS_PUBLIC);
	 return (SUCCESS);	 
       }
   }
   /*
    * check first level backup (internal level 2) for CALL_CLASS_PUBLIC
    */
   if (backup_max > 0)
   {
      rc = Dispatcher_Nordic_look_in_backup_levels(
				 veh_hndl, zone_hndl, (short)2, CALL_CLASS_PUBLIC);
      if (rc == SUCCESS)
      {
	 G7_print("%.11s\t  ...found class = %c in 1st level backup\n",
		  CALL_CLASS_PUBLIC);
	 return (rc);
      }
   }

   /*
    * check second level and beyond (internal levels 3-8) for
    * CALL_CLASS_PUBLIC
    */
   for (level = 3; level <= backup_max; level++)
   {
      rc = Dispatcher_Nordic_look_in_backup_levels(
			     veh_hndl, zone_hndl, level, CALL_CLASS_PUBLIC);
      if (rc == SUCCESS)
      {
	 G7_print("%.11s\t  ...found class = %c in %d level backup\n",
		  CALL_CLASS_PUBLIC, level);
	 return (rc);
      }
   }

   return (FAIL);
}

/*---------------------------------------------------------
 *==>	Dispatcher G7 look in backup levels
 *
 *	OP:
 *		Given a zone and level, search for a call that is compatible
 *		with the supplied vehicle and call_match_class.
 *		For this zone backup level, check each backup zone.
 *
 *		If a compatible call is found, an offer is made to the
 *		vehicle, and return SUCCESS.  If not, then return FAIL.
 *
 *	PRE:
 *	POST:
 *	ERROR:
 *---------------------------------------------------------
 */
RET_STATUS
Dispatcher_Nordic_look_in_backup_levels(
				    VEH_HNDL veh_hndl,
				    ZONE_HNDL zone_hndl,
				    short level,
				    char call_match_class)
{
   INV_BACKUP_ZONE backup_zones;
   RET_STATUS rc;
   int    offer_type, i, j, k;
   short fleet_nbr, zone_nbr;
   ZONE_HNDL backup_zone;
   CALL_HNDL call_in_zone = HNDL_NULL;
   char      error_str[ 132 ];
   ZONE_REC  zone_rec;
   short     veh_zone_nbr;
   BOOLEAN   found_backup_zone;
   char      tmp_str[512];

   G7_print("%.11s\tDispatcher_Nordic_look_in_backup_levels vehicle %d zone %d level %d\n",
	    (int) Veh_get_value(veh_hndl, VEH_NBR), (int)Zone_get_value(zone_hndl, ZONE_NBR), level);

   /*
    * get zone FLEET_NBR
    */
   fleet_nbr = (short)Zone_get_value(zone_hndl, ZONE_FLEET_NBR);



       
   /*
    * get backup zones
    */

   for ( i=0; i< INV_BCK_UP_MAX; i++ )
     backup_zones[i] = 0;
   
   /* This enhancement determines if time calls in the primary and backup chain should have priority
      over all other calls */
   if ( (0) && ( level == 0 ) )
     {
       rc = Zone_get_all_inverse_backup_zones(zone_hndl, fleet[fleet_nbr]->zone_backup_level, backup_zones);
       for ( i = 0; i < INV_BCK_UP_MAX; i++ )
	 {
	   zone_nbr = backup_zones[i];
	   if ( zone_nbr <= 0 )
	     break;
	   backup_zone = HNDL_ERR;
	   Zone_get_hndl(zone_nbr, fleet_nbr, (ZONE_HNDL *) & backup_zone);
	   if ( backup_zone == HNDL_ERR )
	     continue;

	   if ( Dispatcher_Nordic_look_in_zone_for_TimeCalls( backup_zone, veh_hndl, &call_in_zone ) == SUCCESS )
	     {
	       if ( (short)Call_get_value( call_in_zone, CALL_CURRENT_LEVEL ) < level )
		 continue;	       
	       if ( level == 0 )
		 offer_type = STAND_TYPE;
	       else if (level == 1)
		 offer_type = PRIMARY_TYPE;
	       else
		 offer_type = BACKUP_TYPE;

	       /* verify that vehicle's zone is a backup of the call's zone using zone table */
	       bzero( &zone_rec, sizeof( zone_rec ) );
	       zone_rec.fleet = (char)Veh_get_value(veh_hndl, VEH_FLEET);
	       zone_rec.nbr = (short)Call_get_value(call_in_zone, CALL_PICKUP_ZONE);
	       found_backup_zone = FALSE;
	       veh_zone_nbr = (short)Veh_get_value( veh_hndl, VEH_ZONE_NBR );
	       if ( veh_zone_nbr != (short)Call_get_value(call_in_zone, CALL_PICKUP_ZONE) )
		 {
		   db_open( ZONE_FILE_ID, ISINOUT | ISMANULOCK );
		   if ( db_read_key( ZONE_FILE_ID, &zone_rec, &zn_key1, ISEQUAL ) == SUCCESS )
		     {
		       for ( j = 0; j < LEVEL_MAX; j++ )
			 {
			   for ( k = 0; k < BCK_UP_MAX; k++ )
			     {
			       if ( veh_zone_nbr == zone_rec.lev_zone_nbr[j][k] )
				 {
				   found_backup_zone = TRUE;
				   break;
				 }
			       if ( zone_rec.lev_zone_nbr[j][k] == 0 )
				 break;
			     }
			   if ( found_backup_zone == TRUE )
			     break;
			 }
		     }
		 }
	       else
		 found_backup_zone = TRUE;
		       
	       if ( found_backup_zone == TRUE )
		 {
		   Dispatcher_trace_call_offer( veh_hndl, call_in_zone, level );
		   Dispatcher_call_offer(veh_hndl, call_in_zone, offer_type, level);
		   db_close(ZONE_FILE_ID);
		   return (SUCCESS);
		 }
	       else  /* Identified problem with backup zone chain for the call's zone */
		 {
		   sprintf( tmp_str, "Veh_disp prob: cl_nbr %d cl_zone %d vh_nbr %d vh_zn %d",
			   (int)Call_get_value(call_in_zone, CALL_NBR),
			   (short)Call_get_value(call_in_zone, CALL_PICKUP_ZONE),			       
			   (int)Veh_get_value( veh_hndl, VEH_NBR ),
			   (short)Veh_get_value( veh_hndl, VEH_ZONE_NBR ) );
#ifdef FOO
		   ERROR(' ', "", tmp_str);
#endif
		   Zone_maint( 0, (short)Call_get_value(call_in_zone, CALL_PICKUP_ZONE),
			      (char)Veh_get_value( veh_hndl, VEH_FLEET ) );		   
		 }
	     }
	 }
     }       
   
   for ( i=0; i< INV_BCK_UP_MAX; i++ )
     backup_zones[i] = 0;
   
   rc = Zone_get_inverse_backup_zone(zone_hndl,
				     level, backup_zones);
   if (rc != SUCCESS)
      return (FAIL);
   
   /*
    * for backup zone at this level
    */
   for (i = 0; i < INV_BCK_UP_MAX; i++)
   {
      zone_nbr = backup_zones[i];
      if (zone_nbr <= 0)
	 break;

      G7_print("%.11s\t  ...checking inverse backup zone %d\n", zone_nbr);

      backup_zone = HNDL_ERR;
      Zone_get_hndl(zone_nbr, fleet_nbr, (ZONE_HNDL *) & backup_zone);
      if (backup_zone == HNDL_ERR)
	 continue;

      if (Dispatcher_Nordic_look_in_zone(backup_zone, veh_hndl,
				call_match_class, &call_in_zone) == SUCCESS)
      {
	 if ( (short)Call_get_value( call_in_zone, CALL_CURRENT_LEVEL ) < level )
	   continue;

	 if (level == 0)
	    offer_type = STAND_TYPE;
	 else if (level == 1)
	    offer_type = PRIMARY_TYPE;
	 else
	    offer_type = BACKUP_TYPE;

         /* verify that vehicle's zone is a backup of the call's zone using zone table */
	 bzero( &zone_rec, sizeof( zone_rec ) );
	 zone_rec.fleet = (char)Veh_get_value(veh_hndl, VEH_FLEET);
	 zone_rec.nbr = (short)Call_get_value( call_in_zone, CALL_PICKUP_ZONE );
	 found_backup_zone = FALSE;
	 veh_zone_nbr = (short)Veh_get_value( veh_hndl, VEH_ZONE_NBR );
	 if ( veh_zone_nbr != (short)Call_get_value(call_in_zone, CALL_PICKUP_ZONE) )
	   {
	     db_open( ZONE_FILE_ID, ISINOUT | ISMANULOCK );
	     if ( db_read_key( ZONE_FILE_ID, &zone_rec, &zn_key1, ISEQUAL ) == SUCCESS )
	       {
		 for ( j = 0; j < LEVEL_MAX; j++ )
		   {
		     for ( k = 0; k < BCK_UP_MAX; k++ )
		       {
			 if ( veh_zone_nbr == zone_rec.lev_zone_nbr[j][k] )
			   {
			     found_backup_zone = TRUE;
			     break;
			   }
			 if ( zone_rec.lev_zone_nbr[j][k] == 0 )
			   break;
		       }
		     if ( found_backup_zone == TRUE )
		       break;
		   }
	       }
	   }
	 else
	   found_backup_zone = TRUE;
	 
	 if ( found_backup_zone == TRUE )
	   {
	     Dispatcher_trace_call_offer( veh_hndl, call_in_zone, level );
	     Dispatcher_call_offer(veh_hndl, call_in_zone, offer_type, level);
	     db_close(ZONE_FILE_ID);
	     return (SUCCESS);
	   }
	 else  /* Identified problem with backup zone chain for the call's zone */
	   {
	     sprintf( tmp_str, "Veh_disp prob: cl_nbr %d cl_zone %d vh_nbr %d vh_zn %d",
		     (int)Call_get_value(call_in_zone, CALL_NBR),
		     (short)Call_get_value(call_in_zone, CALL_PICKUP_ZONE),			       
		     (int)Veh_get_value( veh_hndl, VEH_NBR ),
		     (short)Veh_get_value( veh_hndl, VEH_ZONE_NBR ) );
#ifdef FOO
	     ERROR(' ', "", tmp_str);
#endif
	     Zone_maint( 0, (short)Call_get_value(call_in_zone, CALL_PICKUP_ZONE),
			 (char)Veh_get_value( veh_hndl, VEH_FLEET ) );
	   }
       }
    }


   db_close(ZONE_FILE_ID);
   return (FAIL);
}

/*---------------------------------------------------------
 *==>   Dispatcher Nordic Look in zone for Time Calls
 *
 *
 *---------------------------------------------------------
 */
RET_STATUS
Dispatcher_Nordic_look_in_zone_for_TimeCalls(
					     ZONE_HNDL zone_hndl,
					     VEH_HNDL veh_hndl,
					     CALL_HNDL *call_found )
{
  ZONE_CALLS_LIST_HNDL zone_call_list;
  CALL_HNDL call_in_zone = HNDL_NULL;

  *call_found = HNDL_NULL;

  Zone_get_info( zone_hndl, ZONE_AVAIL_CALLS_LIST, &zone_call_list );
  Zone_calls_list_each_get( zone_call_list, call_in_zone )
    {
      if ( call_in_zone == NULL )
	continue;

      //transferred to Interlink?
      if ( (short)Call_get_value( (CALL_HNDL)call_in_zone, CALL_TYPE_ILINK ) == TRUE )
	continue;
      
      if ( (short)Call_get_value( call_in_zone, CALL_TYPE_TIME_CALL ) == FALSE )
	continue;

      
      if ( Call_get_state( (CALL_HNDL)call_in_zone, CALL_WAIT_EPOST_STATE ) )
        {
          // continue to hold trip if taxi is not booking into primary zone
          if ( (short)Zone_get_value( zone_hndl, ZONE_NBR ) ==
               (short)Veh_get_value( veh_hndl, VEH_ZONE_NBR ) )
            {
              // release the trip
              Call_reset_state( call_in_zone, CALL_WAIT_EPOST_STATE, NULL );
              // need to add call history here??
            }
          else
            continue;
        }

      if ( Dispatcher_vehicle_call_compatible(
					      call_in_zone, veh_hndl, zone_hndl ) == TRUE )
	{
	  *call_found = call_in_zone;
	  break;
	}
    } Zone_calls_list_each_end(zone_call_list)

    if ( *call_found != HNDL_NULL )
      return( SUCCESS );
    else
      return( FAIL );
}

/*---------------------------------------------------------
 *==>	Dispatcher G7 look in zone
 *
 *	OP:
 *		Given a zone, check each call in the zone for compatibility
 *		with the supplied vehicle and call_match_class.
 *		If a call is found then it is set and return SUCCESS. Otherwise
 *		return FAIL.
 *
 *	PRE:
 *	POST:
 *	ERROR:
 *---------------------------------------------------------
 */
RET_STATUS
Dispatcher_Nordic_look_in_zone(
			   ZONE_HNDL zone_hndl,
			   VEH_HNDL veh_hndl,
			   char call_match_class,
			   CALL_HNDL * call_found)
{
   ZONE_CALLS_LIST_HNDL
      zone_call_list;
   ZONE_VEH_LIST_HNDL zone_vehicle_list = HNDL_NULL;
   VEH_HNDL   veh_in_zone;
   CALL_HNDL call_in_zone = HNDL_NULL;
   char  zone_call_class;
   int  rc;


   *call_found = HNDL_NULL;
   
   G7_print("%.11s\tDispatcher_Nordic_look_in_zone zone %d vehicle %d\n",
	    (int)Zone_get_value(zone_hndl, ZONE_NBR),(int) Veh_get_value(veh_hndl, VEH_NBR));

   /*
    * for each call in this zone find a call that matches call_match_class
    */
   Zone_get_info(zone_hndl, ZONE_AVAIL_CALLS_LIST, &zone_call_list);
   Zone_calls_list_each_get(zone_call_list, call_in_zone)
   {
      if (call_in_zone == NULL)
	 continue;

      //transferred to Interlink?
      if ( (short)Call_get_value( (CALL_HNDL)call_in_zone, CALL_TYPE_ILINK ) == TRUE )
	continue;
      
      if ( Call_get_state( (CALL_HNDL)call_in_zone, CALL_WAIT_EPOST_STATE ))
        {
          // continue to hold trip if taxi is not booking into primary zone
          if ( (short)Zone_get_value( zone_hndl, ZONE_NBR ) ==
               (short)Veh_get_value( veh_hndl, VEH_ZONE_NBR ) )
            {
              // release the trip
              Call_reset_state( call_in_zone, CALL_WAIT_EPOST_STATE, NULL );
              // need to add call history here??
            }
          else
            continue;
        }

      
      if ( Dispatcher_vehicle_call_compatible(
					 call_in_zone, veh_hndl, zone_hndl) == TRUE )
      {
	/** pick the call with the higher priority **/
	 if ( *call_found != HNDL_NULL )
	   {
	     if ( (short)Call_get_value( call_in_zone, CALL_PRIORITY ) <
		  (short)Call_get_value( *call_found, CALL_PRIORITY ) )
	       *call_found = call_in_zone;
	   }
	 else
	   *call_found = call_in_zone;
      }
   } Zone_calls_list_each_end(zone_call_list)

       
  if ( *call_found != HNDL_NULL )
    {
      // check if a STC veh is linked to this trip
      Zone_get_info( zone_hndl, ZONE_AVAIL_VEH_LIST, &zone_vehicle_list );
      Zone_veh_list_each_get( zone_vehicle_list, veh_in_zone )
        {
          if ( Veh_get_value( veh_in_zone, VEH_HELD_EPOST_CALL ) == *call_found )
            {
              Veh_set_value( veh_in_zone, VEH_HELD_EPOST_CALL, HNDL_NULL );
              Call_add_history( *call_found, veh_in_zone, RELEASE_EPOST, 0, 0L, 0 );
              break;
            }
        } Zone_veh_list_each_end(zone_vehicle_list)
      return( SUCCESS);
    }
  else
    return (FAIL);
}

/*---------------------------------------------------------
 *==>	Dispatcher get zone backup max
 *---------------------------------------------------------
 */
RET_STATUS
Dispatcher_get_zone_backup_max(
			       ZONE_HNDL zone_hndl,
			       int *backup_max)
{
   int   fleet_nbr, backup_level_max;
   FLEET *fleet;
   FLEET_HNDL fleet_hndl;

   Zone_get_info(zone_hndl, ZONE_FLEET_NBR, (HNDL *) (&fleet_nbr));

   if (Fleet_get_hndl(BY_NBR, fleet_nbr, &fleet_hndl) != SUCCESS)
      return (FAIL);
   if (fleet_hndl == HNDL_NULL)
      return (FAIL);
   fleet = (FLEET *) fleet_hndl;
   backup_level_max = fleet->zone_backup_level;

   *backup_max = backup_level_max;

   return (SUCCESS);
}

G7_print(fmt, s1, s2, s3, s4, s5, s6, s7, s8)
   char *fmt;
   int  s1, s2, s3, s4, s5, s6, s7, s8;
{
  FILE  *fpTrace;
  char *ctime(), *stime;
  time_t tmtime;  

  return(0);

  if ( ( fpTrace = fopen("/usr/taxi/run/traces/disp_details", "a") ) != NULL )
    {
      tmtime = time((time_t *) 0);
      stime = ctime(&tmtime);
      fprintf(fpTrace, fmt, &stime[8], s1, s2, s3, s4, s5, s6, s7, s8);
      fclose( fpTrace );
    }
  
}

Dispatcher_closest_match(
			 CALL_HNDL call_hndl,
			 ZONE_HNDL zone_hndl,
			 VEH_HNDL *veh_hndl )
{
  CALL         *CalltoMatch;
  FLEET_HNDL   fleet_hndl;
  FLEET        *fl_ptr;
  BACKUP_ZONE  backup_zones;
  ZONE_HNDL    backup_zone;
  struct zones *zone_ptr;  
  ZONE_VEH_LIST_HNDL  zone_vehicle_list;  
  FLEET_VEH_NBR_LIST_HNDL  fl_veh_nbr_list;
  VEH_HNDL     vehicle_in_zone;
  VEH          *veh_ptr;
  short        zone_nbr, i, jj, ii, fleet_nbr;
  short        zone;
  int          check_distance = 0, best_distance = 0, GPS_distance_ceiling = 0;
  short        best_qpos = 0, best_level = 0;
  FILE      *fpGPSDispatchTrace;
  char      *ctime(), *stime;
  time_t    tmtime;

  if ( Fleet_get_hndl( BY_NBR, (short)Call_get_value(call_hndl,CALL_FLEET_NBR), &fleet_hndl ) == FAIL )
      return(FAIL);

  *veh_hndl = HNDL_NULL;
  CalltoMatch = ( CALL * ) call_hndl;

  Zone_get_info( zone_hndl, ZONE_FLEET_NBR, (HNDL *) (&fleet_nbr ) );
  zone_ptr = (struct zones *) zone_hndl;
  fl_ptr = (FLEET *)fleet_hndl;
  
  /* if the call does not have GPS coordinates use the Zone center coordinates */
  if ( CalltoMatch->gps_long <= 0 || CalltoMatch->gps_lat <= 0 )
    {
      // zone defined by circle
      if ( zone_ptr->gps_long > 0 )
	{
	  CalltoMatch->gps_long = zone_ptr->gps_long;
	  CalltoMatch->gps_lat = zone_ptr->gps_lat;
	}
      // zone defined by polygon
      else if ( zone_ptr->poly_centroid_x > 0 )
	{
	  CalltoMatch->gps_long = zone_ptr->poly_centroid_x;
	  CalltoMatch->gps_lat  = zone_ptr->poly_centroid_y;
	}
    }

  if ( !(0) )
    {
      best_distance = 99999999;
      for ( ii = 0; ii < LEVEL_MAX; ii++ )
	{
	  if ( ii > fl_ptr->zone_backup_level )
	    break;
	  
	  if ( ii > CalltoMatch->curr_level )
	    break;
	  
	  for ( jj = 0; jj < BCK_UP_MAX; jj++ )
	    {
	      zone_nbr = zone_ptr->level_number[ii].bck_up_zone[jj];
	      if ( zone_nbr != 0 )
		{
		  Zone_get_hndl( zone_nbr, fleet_nbr, (ZONE_HNDL *) &backup_zone );
		  
		  Zone_get_info( backup_zone, ZONE_AVAIL_VEH_LIST, &zone_vehicle_list );  
		  Zone_veh_list_each_get( zone_vehicle_list, vehicle_in_zone )
		    {
		      if ( Dispatcher_vehicle_call_compatible( call_hndl, vehicle_in_zone, backup_zone ) && 
			   Vehicle_is_Valid_Shift( veh_hndl, call_hndl ) )
			{
			  if ( ( check_distance = Dispatcher_distance_to_client(
										CalltoMatch->gps_long,
										CalltoMatch->gps_lat,
										vehicle_in_zone )  ) >= 0 )
			    {
			      if ( check_distance < best_distance )
				best_distance = check_distance;
			    }
			}
		    } Zone_veh_list_each_end(zone_vehicle_list)
			}
	    }
	}
      
      if ( best_distance == 99999999 )
	return( FAIL );
    }
  
  /* add GPS_DISTANCE_CEILING to best distance in order to perform sort of available taxis list */

  if ( (int)Zone_get_value( zone_hndl, ZONE_GPS_DIST_CEILING ) <= 0 )
    {
      if (0)
	GPS_distance_ceiling = (int) Fleet_get_value( fleet_hndl, FLEET_GPS_DISTANCE_CEILING );
      else
	GPS_distance_ceiling = best_distance + (int) Fleet_get_value( fleet_hndl, FLEET_GPS_DISTANCE_CEILING );
    }
  else
    {
      if (0)
	GPS_distance_ceiling = (int) Zone_get_value( zone_hndl, ZONE_GPS_DIST_CEILING );
      else
	GPS_distance_ceiling = best_distance + (int) Zone_get_value( zone_hndl, ZONE_GPS_DIST_CEILING );
    }

  if ( ( fpGPSDispatchTrace = fopen( GPS_TRACE_FILE, "a" ) ) != NULL )
    {
      tmtime = time( (time_t *) 0 );
      stime = ctime( &tmtime );
      fprintf( fpGPSDispatchTrace, "%.8s\tCall %08d\tZone %03d\tX %05.2f\tY %05.2f Ceiling %06d CALL->VEHICLE (GPS)\n",
	      &stime[11], CalltoMatch->call_number, CalltoMatch->pickup_zone,
	      CalltoMatch->gps_long,
	      CalltoMatch->gps_lat,
	      GPS_distance_ceiling );
    }

  /* check primary zone followed by all backup levels - if taxi found on stand, distribute trip
     to this taxi */
  best_distance = 0;
  *veh_hndl = HNDL_NULL;
  for ( ii = 0; ii < LEVEL_MAX; ii++ )
    {
      if ( ii > fl_ptr->zone_backup_level )
	break;

      if ( ii > CalltoMatch->curr_level )
	break;
      
      for ( jj = 0; jj < BCK_UP_MAX; jj++ )
	{
	  zone_nbr = zone_ptr->level_number[ii].bck_up_zone[jj];
	  if ( zone_nbr != 0 )
	    {
	      Zone_get_hndl( zone_nbr, fleet_nbr, (ZONE_HNDL *) &backup_zone );
	      
	      Zone_get_info( backup_zone, ZONE_AVAIL_VEH_LIST, &zone_vehicle_list );  
	      Zone_veh_list_each_get( zone_vehicle_list, vehicle_in_zone )
		{
		  if ( Dispatcher_vehicle_call_compatible( call_hndl, vehicle_in_zone, backup_zone )&&
		       Vehicle_is_Valid_Shift( veh_hndl, call_hndl ) )
		    {
		      /* is this taxi on the stand...if so, distribute the trip to this one directly */
		      if ( ii == 0 )
			{
			  best_distance = Dispatcher_distance_to_client(
									CalltoMatch->gps_long,
									CalltoMatch->gps_lat,
									vehicle_in_zone );
			  *veh_hndl = vehicle_in_zone;
			  veh_ptr = (VEH *)vehicle_in_zone;
			  fprintf( fpGPSDispatchTrace, "Veh %04d Zone %03d QP %02d X %05.2f Y %05.2f Level %d Dist %06d) STAND\n",
			      (short)Veh_get_value( vehicle_in_zone, VEH_NBR ),
			      (short)Veh_get_value( vehicle_in_zone, VEH_ZONE_NBR ),
			      (short)Veh_get_value( vehicle_in_zone, VEH_Q_POSITION ),
			      veh_ptr->gps_long,
			      veh_ptr->gps_lat, ii,
			      best_distance );
			  fclose( fpGPSDispatchTrace );
			  return (SUCCESS);
			}
		      if ( *veh_hndl == HNDL_NULL )
			{
			  if ( ( check_distance = Dispatcher_distance_to_client(
				      CalltoMatch->gps_long,
				      CalltoMatch->gps_lat,
				      vehicle_in_zone ) ) >= 0 )
			    {
			      *veh_hndl = vehicle_in_zone;
			      best_distance = check_distance;
			      best_qpos = (short) Veh_get_value( vehicle_in_zone, VEH_Q_POSITION );
			      best_level = ii;
			    }
			}
		      else
			{
			  check_distance = Dispatcher_distance_to_client(
				      CalltoMatch->gps_long,
				      CalltoMatch->gps_lat,
				      vehicle_in_zone );
			    if ( check_distance >= 0 )
			      {
				/**
				  Case 1: check_time < GPS_time_ceiling AND best_time > GPS_time_ceiling
				  Result - select new vehicle
				  Case 2: check_time < GPS_time_ceiling AND best_time < GPS_time_ceiling
				  Result - choose vehicle with lowest queue position
				  Case 3: check_time > GPS_time_ceiling AND best_time > GPS_time_ceiling
				  Result - choose vehicle with shortest arrival time
				  Case 4: check_time > GPS_time_ceiling AND best_time < GPS_time_ceiling
				  Result - no change to selected vehicle
				  **/
				
				if ( ( check_distance <= GPS_distance_ceiling ) &&
				     ( best_distance > GPS_distance_ceiling ) )
				  {
				    best_distance = check_distance;
				    best_qpos = (short) Veh_get_value( vehicle_in_zone, VEH_Q_POSITION );
				    best_level = ii;
				    *veh_hndl = vehicle_in_zone;
				  }
				else if ( ( check_distance > GPS_distance_ceiling ) &&
					  ( best_distance > GPS_distance_ceiling ) )
				  {
				    if ( check_distance < best_distance )
				      {
					best_distance = check_distance;
					best_qpos = (short) Veh_get_value( vehicle_in_zone, VEH_Q_POSITION );
					best_level = ii;
					*veh_hndl = vehicle_in_zone;
				      }
				  }			      
			      }
			  
			}
		      veh_ptr = (VEH *)vehicle_in_zone;
		      fprintf( fpGPSDispatchTrace, "Veh %04d Zone %03d QP %02d X %05.2f Y %05.2f Level %d Dist %06d)\n",
			      (short)Veh_get_value( vehicle_in_zone, VEH_NBR ),
			      (short)Veh_get_value( vehicle_in_zone, VEH_ZONE_NBR ),
			      (short)Veh_get_value( vehicle_in_zone, VEH_Q_POSITION ),
			      veh_ptr->gps_long,
			      veh_ptr->gps_lat, ii,
			      check_distance );

		    }
		} Zone_veh_list_each_end(zone_vehicle_list)
	    }
	}
    }
  

  if ( *veh_hndl != HNDL_NULL )
    {

      fprintf( fpGPSDispatchTrace, "Offer Veh %04d\n",(int)Veh_get_value( *veh_hndl, VEH_NBR ) );
      fclose( fpGPSDispatchTrace );

      return (SUCCESS);
    }
  else
    {
      fclose( fpGPSDispatchTrace );
      return (FAIL);
    }
}

Dispatcher_distance_to_client( double call_gps_x, double call_gps_y,
				  VEH_HNDL  veh_hndl )
{
  double       dist = 0.0;
  double       veh_gps_x, veh_gps_y;
  VEH          *veh_ptr;
  double       GPS_xmax, GPS_xmin, GPS_ymax, GPS_ymin;

  veh_ptr = (VEH *)veh_hndl;
  veh_gps_x = veh_ptr->gps_long;
  veh_gps_y = veh_ptr->gps_lat;

  if (0)
    {
      GPS_xmax = (double)GPS_XMAX;
      GPS_xmin = (double)GPS_XMIN;
      GPS_ymax = (double)GPS_YMAX;
      GPS_ymin = (double)GPS_YMIN;
    }
  else
    {
      GPS_xmax = fleet[veh_ptr->fleet_nbr]->gps_max_x;
      GPS_xmin = fleet[veh_ptr->fleet_nbr]->gps_min_x;
      GPS_ymax = fleet[veh_ptr->fleet_nbr]->gps_max_y;
      GPS_ymin = fleet[veh_ptr->fleet_nbr]->gps_min_y;
    }
  
  /* make sure all positions are on the map */
  if ( ( veh_gps_x > GPS_xmax ) || ( call_gps_x > GPS_xmax ) ||
       ( veh_gps_x < GPS_xmin ) || ( call_gps_x < GPS_xmin ) ||
       ( veh_gps_y > GPS_ymax ) || ( call_gps_y > GPS_ymax ) ||
       ( veh_gps_y < GPS_ymin ) || ( call_gps_y < GPS_ymin ) )
    return ( -1 );
  
  dist = sqrt( pow(((double)veh_gps_x - (double)call_gps_x), 2.0 ) +
	       pow(((double)veh_gps_y - (double)call_gps_y), 2.0 ) );

  return((int)dist);
  
}

Dispatcher_trace_call_offer( vehicle_in_zone, call_hndl, level )
     VEH_HNDL vehicle_in_zone;
     CALL_HNDL call_hndl;
     short     level;
{
  FILE      *fpGPSDispatchTrace;
  char      *ctime(), *stime;
  time_t    tmtime;
  CALL      *CalltoMatch;
  VEH       *veh_ptr;

  CalltoMatch = ( CALL * ) call_hndl;
  veh_ptr = (VEH *)vehicle_in_zone;
  
  if ( ( fpGPSDispatchTrace = fopen( GPS_TRACE_FILE, "a" ) ) != NULL )
    {
      tmtime = time( (time_t *) 0 );
      stime = ctime( &tmtime );
      fprintf( fpGPSDispatchTrace, "%.8s\tCall %08d\tZone %03d\tX %05.2f\tY %05.2f VEHICLE->CALL ",
	      &stime[11], CalltoMatch->call_number, CalltoMatch->pickup_zone,
	      CalltoMatch->gps_long,
	      CalltoMatch->gps_lat );
      fprintf( fpGPSDispatchTrace, "Veh %04d Zone %03d QP %02d X %05.2f Y %05.2f Level %d \n",
	      (short)Veh_get_value( vehicle_in_zone, VEH_NBR ),
	      (short)Veh_get_value( vehicle_in_zone, VEH_ZONE_NBR ),
	      (short)Veh_get_value( vehicle_in_zone, VEH_Q_POSITION ),
	      veh_ptr->gps_long,
	      veh_ptr->gps_lat, level );
      fclose( fpGPSDispatchTrace );
    }
}

BOOLEAN Dispatcher_temporal_by_zone_level( CALL_HNDL call_hndl,
					   ZONE_HNDL zone_hndl,
					   VEH_HNDL *veh_hndl )
{
  CALL         *CalltoMatch;
  FLEET_HNDL   fleet_hndl;
  FLEET        *fl_ptr;
  BACKUP_ZONE  backup_zones;
  ZONE_HNDL    backup_zone;
  ZONE_HNDL    stand_zone;
  struct zones *zone_ptr;
  ZONE_VEH_LIST_HNDL  zone_vehicle_list;
  short jj, zone_nbr;
  VEH_HNDL     vehicle_in_zone, best_vehicle;
  time_t       best_time = MAXINT;

  if ( Fleet_get_hndl( BY_NBR, (short)Call_get_value(call_hndl,CALL_FLEET_NBR), &fleet_hndl ) == FAIL )
    return(FAIL);

  best_vehicle = HNDL_NULL;
  CalltoMatch = ( CALL * ) call_hndl;
  fl_ptr = ( FLEET * ) fleet_hndl;
  zone_ptr = (struct zones *)zone_hndl;

  for ( jj = 0; jj < BCK_UP_MAX; jj++ )
    {
      zone_nbr = zone_ptr->level_number[0].bck_up_zone[jj];
      if ( zone_nbr != 0 )
	{
	  Zone_get_hndl( zone_nbr, (short)Zone_get_value(zone_hndl, ZONE_FLEET_NBR), (ZONE_HNDL *)&stand_zone );
	  Zone_get_info( stand_zone, ZONE_AVAIL_VEH_LIST, &zone_vehicle_list );
	  Zone_veh_list_each_get( zone_vehicle_list, vehicle_in_zone )
	    {
	      if ( Dispatcher_vehicle_call_compatible( call_hndl, vehicle_in_zone, stand_zone ) &&
		   Vehicle_is_Valid_Shift( vehicle_in_zone, call_hndl ) ) 
		{
		  if ( (time_t)vehicle_in_zone->time_info < best_time )
		    {
		      best_vehicle = vehicle_in_zone;
		      best_time = (time_t)vehicle_in_zone->time_info;
		    }
		}
	    } Zone_veh_list_each_end( zone_vehicle_list )
	}
    }
  
  if ( best_vehicle != HNDL_NULL )
    {
      *veh_hndl = best_vehicle;
      return (SUCCESS);
    }
  else
    {
      *veh_hndl = HNDL_NULL;
      return (FAIL);
    }
}


BOOLEAN Dispatcher_fifo_by_zone_queue_order( CALL_HNDL call_hndl,
				     ZONE_HNDL zone_hndl,
				     VEH_HNDL *veh_hndl )
{
  CALL         *CalltoMatch;
  FLEET_HNDL   fleet_hndl;
  FLEET        *fl_ptr;
  BACKUP_ZONE  backup_zones;
  ZONE_HNDL    backup_zone;
  ZONE_HNDL    stand_zone;  
  struct zones *zone_ptr;  
  ZONE_VEH_LIST_HNDL  zone_vehicle_list;  
  FLEET_VEH_NBR_LIST_HNDL  fl_veh_nbr_list;
  VEH_HNDL     vehicle_in_zone;
  VEH          *veh_ptr;
  short        zone_nbr, i, jj, ii, fleet_nbr;
  short        zone;
  short     qpos = 0;
  int          done = 0;

  if ( Fleet_get_hndl( BY_NBR, (short)Call_get_value(call_hndl,CALL_FLEET_NBR), &fleet_hndl ) == FAIL )
    return(FAIL);

  CalltoMatch = ( CALL * ) call_hndl;
  fl_ptr = ( FLEET * ) fleet_hndl;
  zone_ptr = (struct zones *)zone_hndl;

  // Checking stands at level 0 - cab queue order within this level with current setting for queue position - start with qpos 1
  while (!done)
    {
      for ( jj = 0; jj < BCK_UP_MAX; jj++ )
	{
	  zone_nbr = zone_ptr->level_number[0].bck_up_zone[jj];
	  if ( zone_nbr != 0 )
	    {
	      Zone_get_hndl( zone_nbr, (short)Zone_get_value(zone_hndl, ZONE_FLEET_NBR), (ZONE_HNDL *)&stand_zone );
	      Zone_get_info( stand_zone, ZONE_AVAIL_VEH_LIST, &zone_vehicle_list );
	      
	      LL_Rewind(zone_vehicle_list); // reset to start of list
	      
	      vehicle_in_zone = Zone_veh_list_get( zone_vehicle_list, ZONE_VEH_LIST_FIRST ); // get first veh if present
	      if ( vehicle_in_zone != HNDL_NULL )
		{
		  for ( i = 0; i < qpos; i++ )
		    {
		      vehicle_in_zone = Zone_veh_list_get( zone_vehicle_list, ZONE_VEH_LIST_NEXT );
		      if ( vehicle_in_zone == HNDL_NULL )
			break; //no taxi at this qpos in the zone chain
		    }
		  if ( vehicle_in_zone != HNDL_NULL )
		    {
		      if ( Dispatcher_vehicle_call_compatible( call_hndl, vehicle_in_zone, stand_zone ) &&
			   Vehicle_is_Valid_Shift( vehicle_in_zone, call_hndl ) )
			{
			  *veh_hndl = vehicle_in_zone;
			  return (SUCCESS);
			}
		    }
		  
		}
	    }
	  // moving to next zone at this level and checking this queue position
	  
	}

      // moving to the next queue position to check - if our last qpos yielded nothing then our search should be done
      if ( vehicle_in_zone == HNDL_NULL )
	{
	  *veh_hndl = HNDL_NULL;
	  return ( FAIL );
	}
      qpos++;
    }
}


Dispatcher_fifo_primary_closest_backup(
			 CALL_HNDL call_hndl,
			 ZONE_HNDL zone_hndl,
			 VEH_HNDL *veh_hndl )
{
  CALL         *CalltoMatch;
  FLEET_HNDL   fleet_hndl;
  FLEET        *fl_ptr;
  BACKUP_ZONE  backup_zones;
  ZONE_HNDL    backup_zone;
  ZONE_HNDL    stand_zone;  
  struct zones *zone_ptr;  
  ZONE_VEH_LIST_HNDL  zone_vehicle_list;  
  FLEET_VEH_NBR_LIST_HNDL  fl_veh_nbr_list;
  VEH_HNDL     vehicle_in_zone;
  VEH          *veh_ptr;
  short        zone_nbr, i, jj, ii, fleet_nbr;
  short        zone;
  int          check_distance = 0, best_distance = 0, GPS_distance_ceiling = 0;
  short        best_qpos = 0, best_level = 0;
  FILE      *fpGPSDispatchTrace;
  char      *ctime(), *stime;
  time_t    tmtime;

  if ( Fleet_get_hndl( BY_NBR, (short)Call_get_value(call_hndl,CALL_FLEET_NBR), &fleet_hndl ) == FAIL )
    return(FAIL);

  CalltoMatch = ( CALL * ) call_hndl;
  fl_ptr = ( FLEET * ) fleet_hndl;
  zone_ptr = (struct zones *)zone_hndl;

  /* if the call does not have GPS coordinates use the Zone center coordinates */
  if ( CalltoMatch->gps_long <= 0 || CalltoMatch->gps_lat <= 0 )
    {
      // zone defined by circle
      if ( zone_ptr->gps_long > 0 )
	{      
	  CalltoMatch->gps_long = zone_ptr->gps_lat;
	  CalltoMatch->gps_lat = zone_ptr->gps_long;
	}
      // zone defined by polygon
      else if ( zone_ptr->poly_centroid_x > 0 )
	{
	  CalltoMatch->gps_long = zone_ptr->poly_centroid_x;
	  CalltoMatch->gps_lat  = zone_ptr->poly_centroid_y;
	}
      else
	{
	  CalltoMatch->gps_long = 0;
	  CalltoMatch->gps_lat = 0;
	}
    }

  /** First check the stand zones and offer to first qualified in queue **/
  for ( jj = 0; jj < BCK_UP_MAX; jj++ )
    {
      zone_nbr = zone_ptr->level_number[0].bck_up_zone[jj];
      if ( zone_nbr != 0 )
	{
	  Zone_get_hndl( zone_nbr, (short)Zone_get_value(zone_hndl, ZONE_FLEET_NBR), (ZONE_HNDL *)&stand_zone );
	  Zone_get_info( stand_zone, ZONE_AVAIL_VEH_LIST, &zone_vehicle_list );
	  Zone_veh_list_each_get( zone_vehicle_list, vehicle_in_zone )
	    {
	      if ( Dispatcher_vehicle_call_compatible( call_hndl,
						       vehicle_in_zone, stand_zone ) )
		{
		  *veh_hndl = vehicle_in_zone;
		  return (SUCCESS);
		}
	    } Zone_veh_list_each_end(zone_vehicle_list)
	}
    }

  
  /** Second check the primary zone and offer to first qualified in queue **/
  Zone_get_info( zone_hndl, ZONE_AVAIL_VEH_LIST, &zone_vehicle_list );
  Zone_veh_list_each_get(zone_vehicle_list, vehicle_in_zone)
    {
      if (Dispatcher_vehicle_call_compatible(call_hndl,
					     vehicle_in_zone, zone_hndl))
	{
	  *veh_hndl = vehicle_in_zone;
	  return (SUCCESS);
	}
    } Zone_veh_list_each_end(zone_vehicle_list)
  
  *veh_hndl = HNDL_NULL;
  CalltoMatch = ( CALL * ) call_hndl;

  // don't continue if this search is limited to stands/primary zones
  // this will ensure that the trip will be held for a STC veh in primary
  if (CalltoMatch->curr_level <= 1)
    return(FAIL);
  
  Zone_get_info( zone_hndl, ZONE_FLEET_NBR, (HNDL *) (&fleet_nbr ) );
  zone_ptr = (struct zones *) zone_hndl;

  best_distance = 99999999;
  for ( ii = 2; ii < LEVEL_MAX; ii++ )
    {
      if ( ii > fl_ptr->zone_backup_level )
	break;
      
      for ( jj = 0; jj < BCK_UP_MAX; jj++ )
	{
	  zone_nbr = zone_ptr->level_number[ii].bck_up_zone[jj];
	  if ( zone_nbr != 0 )
	    {
	      Zone_get_hndl( zone_nbr, fleet_nbr, (ZONE_HNDL *) &backup_zone );
	      
	      Zone_get_info( backup_zone, ZONE_AVAIL_VEH_LIST, &zone_vehicle_list );  
	      Zone_veh_list_each_get( zone_vehicle_list, vehicle_in_zone )
		{
		  if ( Dispatcher_vehicle_call_compatible( call_hndl, vehicle_in_zone, backup_zone ) )
		    {
		      if ( ( check_distance = Dispatcher_distance_to_client(
					      CalltoMatch->gps_long,
					      CalltoMatch->gps_lat,
					      vehicle_in_zone )  ) >= 0 )
			{
			  if ( check_distance < best_distance )
			    best_distance = check_distance;
			}
		    }
		} Zone_veh_list_each_end(zone_vehicle_list)
	    }
	}
    }

  if ( best_distance == 99999999 )
    return( FAIL );

  /* add GPS_DISTANCE_CEILING to best distance in order to perform sort of available taxis list */

  if ( (int)Zone_get_value( zone_hndl, ZONE_GPS_DIST_CEILING ) <= 0 )
    GPS_distance_ceiling = best_distance + (int) Fleet_get_value( fleet_hndl, FLEET_GPS_DISTANCE_CEILING );
  else
    GPS_distance_ceiling = best_distance + (int) Zone_get_value( zone_hndl, ZONE_GPS_DIST_CEILING );

  if ( ( fpGPSDispatchTrace = fopen( GPS_TRACE_FILE, "a" ) ) != NULL )
    {
      tmtime = time( (time_t *) 0 );
      stime = ctime( &tmtime );
      fprintf( fpGPSDispatchTrace, "%.8s\tCall %08d\tZone %03d\tX %05.2f\tY %05.2f Ceiling %06d CALL->VEHICLE (GPS)\n",
	      &stime[11], CalltoMatch->call_number, CalltoMatch->pickup_zone,
	      CalltoMatch->gps_long,
	      CalltoMatch->gps_lat,
	      GPS_distance_ceiling );
    }

  /* check primary zone followed by all backup levels - ignore stand level 0 */
  best_distance = 0;
  *veh_hndl = HNDL_NULL;
  for ( ii = 0; ii < LEVEL_MAX; ii++ )
    {

      if ( ii > fl_ptr->zone_backup_level )
	break;
      
      for ( jj = 0; jj < BCK_UP_MAX; jj++ )
	{
	  zone_nbr = zone_ptr->level_number[ii].bck_up_zone[jj];
	  if ( zone_nbr != 0 )
	    {
	      Zone_get_hndl( zone_nbr, fleet_nbr, (ZONE_HNDL *) &backup_zone );
	      
	      Zone_get_info( backup_zone, ZONE_AVAIL_VEH_LIST, &zone_vehicle_list );  
	      Zone_veh_list_each_get( zone_vehicle_list, vehicle_in_zone )
		{
		  if ( Dispatcher_vehicle_call_compatible( call_hndl, vehicle_in_zone, backup_zone ) )
		    {
		      if ( *veh_hndl == HNDL_NULL )
			{
			  if ( ( check_distance = Dispatcher_distance_to_client(
				      CalltoMatch->gps_long,
				      CalltoMatch->gps_lat,
				      vehicle_in_zone ) ) >= 0 )
			    {
			      *veh_hndl = vehicle_in_zone;
			      best_distance = check_distance;
			      best_qpos = (short) Veh_get_value( vehicle_in_zone, VEH_Q_POSITION );
			      best_level = ii;
			    }
			}
		      else
			{
			  check_distance = Dispatcher_distance_to_client(
				      CalltoMatch->gps_long,
				      CalltoMatch->gps_lat,
				      vehicle_in_zone );
			    if ( check_distance >= 0 )
			      {
				
				if ( check_distance < best_distance )
				  {
				    best_distance = check_distance;
				    best_level = ii;
				    *veh_hndl = vehicle_in_zone;
				  }
			      }
			  
			}

		      veh_ptr = (VEH *)vehicle_in_zone;
		      fprintf( fpGPSDispatchTrace, "Veh %04d Zone %03d QP %02d X %05.2f Y %05.2f Level %d Dist %06d)\n",
			      (short)Veh_get_value( vehicle_in_zone, VEH_NBR ),
			      (short)Veh_get_value( vehicle_in_zone, VEH_ZONE_NBR ),
			      (short)Veh_get_value( vehicle_in_zone, VEH_Q_POSITION ),
			      veh_ptr->gps_long,
			      veh_ptr->gps_lat, ii,
			      check_distance );

		    }
		} Zone_veh_list_each_end(zone_vehicle_list)
	    }
	}
    }
  

  if ( *veh_hndl != HNDL_NULL )
    {

      fprintf( fpGPSDispatchTrace, "Offer Veh %04d (CLOSEST BACKUP)\n",(int)Veh_get_value( *veh_hndl, VEH_NBR ) );
      fclose( fpGPSDispatchTrace );
      return (SUCCESS);
    }
  else
    {
      fclose( fpGPSDispatchTrace );
      return (FAIL);
    }
}  
