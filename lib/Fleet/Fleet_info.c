/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Fleet_info.c,v $
*  @(#)  $Revision: 1.19 $
*  @(#)  $Date: 2004/06/22 17:16:23 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/Fleet/Fleet_info.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Fleet_info.c,v 1.19 2004/06/22 17:16:23 jwelch Exp $";

#include <sys/types.h>		/* temp */
#include <sys/times.h>		/* temp */
#include "mad_error.h"
#include "taxipak.h"
#include "Object.h"
#include "Fleet_public.h"
#include "Fleet_private.h"

extern struct fleets *fleet[FLEET_MAX];
HNDL fleet_ext_value();

HNDL
fleet_value(fleet_hndl, op, attr, value)
   FLEET_HNDL fleet_hndl;
   int   op;			/* GET PUT INCREMENT DECREMENT */
   int   attr;
   HNDL  value;
{
   struct fleets *fleet_ptr;
   int   ii;
   char  error_buf[80];
   extern struct fleets * fleet_hndl_to_ptr();


   if ( fleet_hndl == HNDL_NULL )
     {
       if ( op != GET )
         return ((HNDL) FAIL);
       else
         return (HNDL_NULL);
     }
     
   fleet_ptr = fleet_hndl_to_ptr(fleet_hndl);

   switch (attr)
   {
   case FLEET_CALL_OFFER_PROCEDURE:
      if (op == GET)
	 return ((HNDL) fleet_ptr->call_offer_procedure);
      if (op == PUT)
	 fleet_ptr->call_offer_procedure = (char) value;
      break;
   case FLEET_NBR:
      if (op == PUT)
	 return ((HNDL) FAIL);	/* sorry predefined */

      if (op == GET)
	 return ((HNDL) fleet_ptr->fleet_nbr);
   case FLEET_ID:
      if (op == PUT)
	 return ((HNDL) FAIL);	/* sorry predefined */

      if (op == GET)
	 return ((HNDL) fleet_ptr->fleet_id);

   case FLEET_CALL_ACCEPT_TIME:
      if (op == GET)
	 return ((HNDL) fleet_ptr->call_accept_per);
      if (op == PUT)
	 fleet_ptr->call_accept_per = (short) value;
      break;

   case FLEET_CBOOK_TIME:
     if (op == GET)
       return ((HNDL)fleet_ptr->cbook_time);
     if (op == PUT)
       fleet_ptr->cbook_time = (short)value * 60;
     break;

   case FLEET_STAND_NTFY_INTERVAL:
     if (op == GET)
       return ((HNDL)fleet_ptr->stand_ntfy_interval);
     if (op == PUT)
       fleet_ptr->stand_ntfy_interval = (short)value;
     break;

   case FLEET_STAND_NTFY_LIFE:
     if (op == GET)
       return ((HNDL)fleet_ptr->stand_ntfy_life);
     if (op == PUT)
       fleet_ptr->stand_ntfy_life = (short)value;
     break;          
     
   case FLEET_APPROVE_MSGS_TO_DRIVERS:
      /* temp char short problem */
      if (op == GET)
	 return ((HNDL) ((fleet_ptr->approve_msgs_to_drivers == '1') ? 1 : 2));
      if (op == PUT)
	 fleet_ptr->call_accept_per = ((short) value == 1 ? '1' : '2');
      break;
   case FLEET_DRIVER_VEH_LIST:
      if (op == GET)
	 return ((HNDL) fleet_ptr->fleet_driver_veh_list_hndl);
      else
      {
	 fleet_ptr->fleet_driver_veh_list_hndl = (FLEET_DRIVER_VEH_LIST_HNDL) value;
	 return ((HNDL) SUCCESS);
      }
      break;

    case FLEET_EXCPT_LIST:
      if ( op == GET )
	return ((HNDL) fleet_ptr->fleet_excpt_list_hndl );
      else
	return ((HNDL) FAIL );
      break;

    case FLEET_VEH_SUSP_PASS_LIST:
      if ( op == GET )
	return ((HNDL) fleet_ptr->veh_susp_pass_list_hndl );
      else
	return ((HNDL) FAIL );
      break;

    case FLEET_VEH_SUSP_DELIV_LIST:
      if ( op == GET )
	return ((HNDL) fleet_ptr->veh_susp_deliv_list_hndl );
      else
	return ((HNDL) FAIL );
      break;

    case FLEET_VEH_SUSP_TYPEX_LIST:
      if ( op == GET )
	return ((HNDL) fleet_ptr->veh_susp_typex_list_hndl );
      else
	return ((HNDL) FAIL );
      break;

    case FLEET_DRV_SUSP_PASS_LIST:
      if ( op == GET )
	return ((HNDL) fleet_ptr->drv_susp_pass_list_hndl );
      else
	return ((HNDL) FAIL );
      break;

    case FLEET_DRV_SUSP_DELIV_LIST:
      if ( op == GET )
	return ((HNDL) fleet_ptr->drv_susp_deliv_list_hndl );
      else
	return ((HNDL) FAIL );
      break;

    case FLEET_DRV_SUSP_TYPEX_LIST:
      if ( op == GET )
	return ((HNDL) fleet_ptr->drv_susp_typex_list_hndl );
      else
	return ((HNDL) FAIL );
      break;

   case FLEET_BIG_CAR_TRIP_LIST:
     if ( op == GET )
       return ((HNDL) fleet_ptr->big_car_trip_list_hndl );
     else
       return ((HNDL) FAIL );
     break;

   case FLEET_TIME_CALL_MAX_INTERVAL:
      if (op == GET)
         return ((HNDL) fleet_ptr->time_call_freq);
      else
      {
         ERROR(' ',"","Attempt to PUT FLEET_TIME_CALL_MAX_INTERVAL");
         return((HNDL) FAIL);
      }

    case FLEET_TIME_CALL_LEAD_SUPP:
      if ( op == GET )
	return ((HNDL) fleet_ptr->time_calls_lead_supp );
      else
	return((HNDL) FAIL);

    case FLEET_TC_LOOK_AHEAD:
      if ( op == GET )
	return ((HNDL) fleet_ptr->time_int_req );
      else
	return ((HNDL) FAIL);



    case FLEET_WAKEUP_LEAD_TIME:
      if ( op == GET )
	return((HNDL) fleet_ptr->wakeup_lead_time );
      else
	return((HNDL) FAIL );

   case FLEET_UNASSIGNED_TIME:
      if (op == GET)
	 return ((HNDL) fleet_ptr->unassigned_time);
      else
      {
	 fleet_ptr->unassigned_time = (short) value;
	 return ((HNDL) SUCCESS);
      }

   case FLEET_VEH_NBR_LIST:
      if (op == GET)
	 return ((HNDL) fleet_ptr->fleet_veh_nbr_list_hndl);
      else
      {
	 fleet_ptr->fleet_veh_nbr_list_hndl = (FLEET_VEH_NBR_LIST_HNDL) value;
	 return ((HNDL) SUCCESS);
      }
   case FLEET_ZONE_CALLS_LIST:
      if (op == GET)
	 return ((HNDL) fleet_ptr->fleet_zone_calls_list_hndl);
      else
      {
	 fleet_ptr->fleet_zone_calls_list_hndl = (FLEET_VEH_NBR_LIST_HNDL) value;
	 return ((HNDL) SUCCESS);
      }
   case FLEET_FUTURE_CALL_MINIMUM:
      if (op == GET)
         return ((HNDL) fleet_ptr->future_call_minimum);
      else
      {
         fleet_ptr->future_call_minimum = (long) value;
         return((HNDL) SUCCESS);
      }
    case FLEET_FUTURE_CALL_MAXIMUM:
      if (op == GET)
         return ((HNDL) fleet_ptr->future_call_maximum);
      else
      {
         fleet_ptr->future_call_maximum = (long) value;
         return((HNDL) SUCCESS);
      }      
    case FLEET_ACTIVE:
      if ( op == GET )
	return ((HNDL) fleet_ptr->active );
      else
	return((HNDL)FAIL);

    case FLEET_FLAG_ACTION:
      if ( op == GET )
	return ((HNDL) fleet_ptr->auto_flag_on_stands );
      else
	{
	  fleet_ptr->auto_flag_on_stands = ( char ) value;
	  return((HNDL) SUCCESS);
	}

    case FLEET_AUTO_FLAG_TIME:
      if ( op == GET )
	return ((HNDL) fleet_ptr->auto_flag_req_time );
      else
	{
	  fleet_ptr->auto_flag_req_time = (short) value;
	  return ((HNDL) SUCCESS);
	}

    case FLEET_GPS_STATE:
      if ( op == GET )
	return ((HNDL) fleet_ptr->gps_state );
      else
	{
	  fleet_ptr->gps_state = ( char ) value;
	  return((HNDL) SUCCESS);
	}

    case FLEET_GPS_POS_AGE:
      if ( op == GET )
	return ((HNDL) fleet_ptr->gps_time );
      else
	{
	  fleet_ptr->gps_time = ( short ) value;
	  return((HNDL) SUCCESS);
	}


   case FLEET_TC_RESTRICT_INTERVAL:
     if ( op == GET )
       return ((HNDL) fleet_ptr->tc_restrict_interval );
     else
       {
	 fleet_ptr->tc_restrict_interval = (short) value;
	 return ((HNDL) SUCCESS );
       }

   case FLEET_NS_SHORT:
      if ( op == GET )
	return ((HNDL) fleet_ptr->ns_short );
      else
	  {
	      fleet_ptr->ns_short = (short) value;
	      return ((HNDL) SUCCESS );
	  }
      
   case FLEET_EMERG_RANGE:
     if ( op == GET )
       return ((HNDL) fleet_ptr->emerg_range );
     else
       {
         fleet_ptr->emerg_range = (short) value;
         return ((HNDL) SUCCESS );
       }

   case FLEET_ENROUTE_BREAK:
     if ( op == GET )
       return ((HNDL) fleet_ptr->enroute_break );
     else
       {
         fleet_ptr->enroute_break = (char ) value;
         return ((HNDL) SUCCESS );
       }

   case FLEET_ILINK_ACTIVE:
     if ( op == GET )
       return ((HNDL) fleet_ptr->ilink_active );
     else
       {
         fleet_ptr->ilink_active = (char ) value;
         return ((HNDL) SUCCESS );
       }     

   case FLEET_ILINK_TIMEOUT:
      if ( op == GET )
	return ((HNDL) fleet_ptr->ilink_timeout );
      else
	  {
	      fleet_ptr->ilink_timeout = (short) value;
	      return ((HNDL) SUCCESS );
	  }     

    case FLEET_SEND_CALL_TYPE:
      if ( op == GET )
	return ((HNDL) fleet_ptr->send_call_type );
      else
	{
	  fleet_ptr->send_call_type = ( char ) value;
	  return((HNDL) SUCCESS);
	}

    case FLEET_AUTO_REJECT_TIME:
      if ( op == GET )
	return ((HNDL) fleet_ptr->auto_reject_time);
      else
	{
	  fleet_ptr->auto_reject_time = ( short ) value;
	  return((HNDL) SUCCESS);
	}

    case FLEET_M_ACTION_TIMER:
      if ( op == GET )
	return ((HNDL) fleet_ptr->m_action_timer );
      else
	{
	  fleet_ptr->m_action_timer = (short) value;
	  return((HNDL) SUCCESS);
	}

    case FLEET_M_ACTION_RETRY:
      if ( op == GET )
	return ((HNDL) fleet_ptr->m_action_retry );
      else
	{
	  fleet_ptr->m_action_retry = (short) value;
	  return((HNDL) SUCCESS);
	}

    case FLEET_M_ACTION_PENALTY:
      if ( op == GET )
	return ((HNDL) fleet_ptr->m_action_penalty );
      else
	{
	  fleet_ptr->m_action_penalty = (short) value;
	  return((HNDL) SUCCESS);
	}

    case FLEET_CANX_TO_MOFF_TIME:
      if ( op == GET )
	return ((HNDL) fleet_ptr->canx_to_moff_time );
      else
	{
	  fleet_ptr->canx_to_moff_time = (short) value;
	  return((HNDL) SUCCESS);
	}            

    case FLEET_GPS_COORD_SYS:
      if ( op == GET )
	return((HNDL)fleet_ptr->gps_coord_sys );
      else
	{
	  fleet_ptr->gps_coord_sys = (short) value;
	  return((HNDL) SUCCESS);
	}

    case FLEET_GPS_BOOK:
      if ( op == GET )
	return((HNDL)fleet_ptr->gps_book );
      else
	{
	  fleet_ptr->gps_book = (char) value;
	  return((HNDL) SUCCESS);
	}

   case FLEET_GPS_TIME:
     if ( op == GET )
       return((HNDL)fleet_ptr->gps_time );
     else
       {
	 fleet_ptr->gps_time = (short)value;
	 return((HNDL) SUCCESS);
       }
      
    case FLEET_MATCH_ALGORITHM:
      if ( op == GET )
	return ((HNDL) fleet_ptr->matching_algorithm );
      else
	{
	  fleet_ptr->matching_algorithm = ( char ) value;
	  return ((HNDL) SUCCESS);
	}
      
      /* STATISTICS **************** */

   case FLEET_NBR_ASSIGNS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->asigns++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->asigns);
      case PUT:
	 fleet_ptr->asigns = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->asigns--;
	 if (fleet_ptr->asigns < 0)
	 {
	    fleet_ptr->asigns = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_BIDS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->bids++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->bids);
      case PUT:
	 fleet_ptr->bids = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->bids--;
	 if (fleet_ptr->bids < 0)
	 {
	    fleet_ptr->bids = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_BREAKS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->breaks++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->breaks);
      case PUT:
	 fleet_ptr->breaks = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->breaks--;
	 if (fleet_ptr->breaks < 0)
	 {
	    fleet_ptr->breaks = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_CANCELS:
     switch (op)
       {
       case INCREMENT:
	 fleet_ptr->cancels++;
	 return ((HNDL) SUCCESS);
       case GET:
	 return ((HNDL)fleet_ptr->cancels);
       case PUT:
	 fleet_ptr->cancels = 0;
	 return ((HNDL) SUCCESS);
       case DECREMENT:
	 fleet_ptr->cancels--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_CALLBACKS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->callbacks++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->callbacks);
      case PUT:
	 fleet_ptr->callbacks = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->callbacks--;
	 if (fleet_ptr->callbacks < 0)
	 {
	    fleet_ptr->callbacks = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_CALLOUTS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->callouts++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->callouts);
      case PUT:
	 fleet_ptr->callouts = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->callouts--;
	 if (fleet_ptr->callouts < 0)
	 {
	    fleet_ptr->callouts = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_CALLS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->calls++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->calls);
      case PUT:
	 fleet_ptr->calls = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->calls--;
	 if (fleet_ptr->calls < 0)
	 {
	    fleet_ptr->calls = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_COND_POSTS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->cond_posts++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->cond_posts);
      case PUT:
	 fleet_ptr->cond_posts = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->cond_posts--;
	 if (fleet_ptr->cond_posts < 0)
	 {
	    fleet_ptr->cond_posts = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_EMERGENCIES:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->emergencies++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->emergencies);
      case PUT:
	 fleet_ptr->emergencies = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->emergencies--;
	 if (fleet_ptr->emergencies < 0)
	 {
	    fleet_ptr->emergencies = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_ENROUTE_POSTS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->enroute_posts++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->enroute_posts);
      case PUT:
	 fleet_ptr->enroute_posts = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->enroute_posts--;
	 if (fleet_ptr->enroute_posts < 0)
	 {
	    fleet_ptr->enroute_posts = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_FLAGS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->flags++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->flags);
      case PUT:
	 fleet_ptr->flags = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->flags--;
	 if (fleet_ptr->flags < 0)
	 {
	    fleet_ptr->flags = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_FL_VEH_ATTR:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->fl_veh_attr[(short) value]++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->fl_veh_attr[(short) value]);
      case PUT:
	 fleet_ptr->fl_veh_attr[(short) value] = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->fl_veh_attr[(short) value]--;
	 if (fleet_ptr->fl_veh_attr[(short) value] < 0)
	 {
	    fleet_ptr->fl_veh_attr[(short) value] = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_LATE_METERS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->late_meters++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->late_meters);
      case PUT:
	 fleet_ptr->late_meters = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->late_meters--;
	 if (fleet_ptr->late_meters < 0)
	 {
	    fleet_ptr->late_meters = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_MESSAGES:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->messages++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->messages);
      case PUT:
	 fleet_ptr->messages = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->messages--;
	 if (fleet_ptr->messages < 0)
	 {
	    fleet_ptr->messages = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NO_ACCEPTS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->no_accepts++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->no_accepts);
      case PUT:
	 fleet_ptr->no_accepts = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->no_accepts--;
	 if (fleet_ptr->no_accepts < 0)
	 {
	    fleet_ptr->no_accepts = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NO_SHOWS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->no_shows++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->no_shows);
      case PUT:
	 fleet_ptr->no_shows = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->no_shows--;
	 if (fleet_ptr->no_shows < 0)
	 {
	    fleet_ptr->no_shows = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_REJECTS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->rejects++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->rejects);
      case PUT:
	 fleet_ptr->rejects = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->rejects--;
	 if (fleet_ptr->rejects < 0)
	 {
	    fleet_ptr->rejects = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_REQ_TO_TALK:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->req_to_talk++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->req_to_talk);
      case PUT:
	 fleet_ptr->req_to_talk = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->req_to_talk--;
	 if (fleet_ptr->req_to_talk < 0)
	 {
	    fleet_ptr->req_to_talk = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_SHORT_METERS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->short_meters++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->short_meters);
      case PUT:
	 fleet_ptr->short_meters = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->short_meters--;
	 if (fleet_ptr->short_meters < 0)
	 {
	    fleet_ptr->short_meters = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_SIGNED_OFF:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->signed_off++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->signed_off);
      case PUT:
	 fleet_ptr->signed_off = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->signed_off--;
	 if (fleet_ptr->signed_off < 0)
	 {
	    fleet_ptr->signed_off = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_SIGNED_ON:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->signed_on++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->signed_on);
      case PUT:
	 fleet_ptr->signed_on = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->signed_on--;
	 if (fleet_ptr->signed_on < 0)
	 {
	    fleet_ptr->signed_on = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_SUSPENDED:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->suspended++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->suspended);
      case PUT:
	 fleet_ptr->suspended = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->suspended--;
	 if (fleet_ptr->suspended < 0)
	 {
	    fleet_ptr->suspended = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_WAKEUPS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->wakeups++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->wakeups);
      case PUT:
	 fleet_ptr->wakeups = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->wakeups--;
	 if (fleet_ptr->wakeups < 0)
	 {
	    fleet_ptr->wakeups = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_BIDS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_bids++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_bids);
      case PUT:
	 fleet_ptr->now_bids = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_bids--;
	 if (fleet_ptr->now_bids < 0)
	 {
	    fleet_ptr->now_bids = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_BREAKS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_breaks++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_breaks);
      case PUT:
	 fleet_ptr->now_breaks = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_breaks--;
	 if (fleet_ptr->breaks < 0)
	 {
	    fleet_ptr->now_breaks = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_CALLBACKS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_callbacks++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_callbacks);
      case PUT:
	 fleet_ptr->now_callbacks = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_callbacks--;
	 if (fleet_ptr->now_callbacks < 0)
	 {
	    fleet_ptr->now_callbacks = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_CALLOUTS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_callouts++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_callouts);
      case PUT:
	 fleet_ptr->now_callouts = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_callouts--;
	 if (fleet_ptr->now_callouts < 0)
	 {
	    fleet_ptr->now_callouts = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_CALLS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_calls++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_calls);
      case PUT:
	 fleet_ptr->now_calls = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_calls--;
	 if (fleet_ptr->now_calls < 0)
	 {
	    fleet_ptr->now_calls = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_DRIV_ATTR:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_driv_attr[(short) value]++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_driv_attr[(short) value]);
      case PUT:
	 fleet_ptr->now_driv_attr[(short) value] = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_driv_attr[(short) value]--;
	 if (fleet_ptr->now_driv_attr[(short) value] < 0)
	 {
	    fleet_ptr->now_driv_attr[(short) value] = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_EMERGENCIES:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_emergencies++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_emergencies);
      case PUT:
	 fleet_ptr->now_emergencies = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_emergencies--;
	 if (fleet_ptr->now_emergencies < 0)
	 {
	    fleet_ptr->now_emergencies = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_ENROUTE_POSTS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_enroute_posts++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_enroute_posts);
      case PUT:
	 fleet_ptr->now_enroute_posts = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_enroute_posts--;
	 if (fleet_ptr->now_enroute_posts < 0)
	 {
	    fleet_ptr->now_enroute_posts = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_FLAGS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_flags++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_flags);
      case PUT:
	 fleet_ptr->now_flags = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_flags--;
	 if (fleet_ptr->now_flags < 0)
	 {
	    fleet_ptr->now_flags = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_IDLE:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_idle++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_idle);
      case PUT:
	 fleet_ptr->now_idle = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_idle--;
	 if (fleet_ptr->now_idle < 0)
	 {
	    fleet_ptr->now_idle = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_IN_FLEET:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_in_fleet++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_in_fleet);
      case PUT:
	 fleet_ptr->now_in_fleet = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_in_fleet--;
	 if (fleet_ptr->now_in_fleet < 0)
	 {
	    fleet_ptr->now_in_fleet = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_LATE_METERS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_late_meters++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_late_meters);
      case PUT:
	 fleet_ptr->now_late_meters = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_late_meters--;
	 if (fleet_ptr->now_late_meters < 0)
	 {
	    fleet_ptr->now_late_meters = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_MESSAGES:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_messages++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_messages);
      case PUT:
	 fleet_ptr->now_messages = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_messages--;
	 if (fleet_ptr->now_messages < 0)
	 {
	    fleet_ptr->now_messages = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_NO_SHOWS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_no_shows++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_no_shows);
      case PUT:
	 fleet_ptr->now_no_shows = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_no_shows--;
	 if (fleet_ptr->now_no_shows < 0)
	 {
	    fleet_ptr->now_no_shows = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_POSTS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_posts++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_posts);
      case PUT:
	 fleet_ptr->now_posts = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_posts--;
	 if (fleet_ptr->now_posts < 0)
	 {
	    fleet_ptr->now_posts = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_REQ_TO_TALK:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_req_to_talk++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_req_to_talk);
      case PUT:
	 fleet_ptr->now_req_to_talk = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_req_to_talk--;
	 if (fleet_ptr->now_req_to_talk < 0)
	 {
	    fleet_ptr->now_req_to_talk = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_SIGNED_OFF:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_signed_off++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_signed_off);
      case PUT:
	 fleet_ptr->now_signed_off = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_signed_off--;
	 if (fleet_ptr->now_signed_off < 0)
	 {
	    fleet_ptr->now_signed_off = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_SIGNED_ON:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_signed_on++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_signed_on);
      case PUT:
	 fleet_ptr->now_signed_on = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_signed_on--;
	 if (fleet_ptr->now_signed_on < 0)
	 {
	    fleet_ptr->now_signed_on = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_SUSPENDED:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_suspended++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_suspended);
      case PUT:
	 fleet_ptr->now_suspended = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_suspended--;
	 if (fleet_ptr->now_suspended < 0)
	 {
	    fleet_ptr->now_suspended = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_VEH_ATTR:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_veh_attr[(short) value]++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_veh_attr[(short) value]);
      case PUT:
	 fleet_ptr->now_veh_attr[(short) value] = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_veh_attr[(short) value]--;
	 if (fleet_ptr->now_veh_attr[(short) value] < 0)
	 {
	    fleet_ptr->now_veh_attr[(short) value] = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

   case FLEET_NBR_NOW_WAKEUPS:
      switch (op)
      {
      case INCREMENT:
	 fleet_ptr->now_wakeups++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) fleet_ptr->now_wakeups);
      case PUT:
	 fleet_ptr->now_wakeups = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 fleet_ptr->now_wakeups--;
	 if (fleet_ptr->now_wakeups < 0)
	 {
	    fleet_ptr->now_wakeups = 0;
	    return ((HNDL) FAIL);
	 }
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

    case FLEET_GPS_DISTANCE_CEILING:
      {
	switch (op)
	  {
	  case GET:
	    return ((HNDL)fleet_ptr->gps_distance_ceiling);
	  case PUT:
	    fleet_ptr->gps_distance_ceiling = (int) value;
	    return ((HNDL) SUCCESS);
	  default:
	    return ((HNDL)FAIL );
	  }
      }
      
   default:
      sprintf(error_buf, "ILLEGAL VALUE FOR fleet_access %d\n", attr);
      ERROR(' ', "", error_buf);
      if (op != GET)
	 return ((HNDL) FAIL);
      else
	 return (HNDL_NULL);
      break;
   }				/* end case */
   return ((HNDL) SUCCESS);
}				/* end fleet_value */

HNDL
Fleet_get_value(fleet_hndl, attr)
   FLEET_HNDL fleet_hndl;
   int   attr;
{
  return (fleet_value(fleet_hndl, GET, attr, NULL));
}				/* end Fleet_get_value */

RET_STATUS
Fleet_get_info(fleet_hndl, attr, value_ptr)
   FLEET_HNDL fleet_hndl;
   int   attr;
   HNDL * value_ptr;
{
   if (value_ptr == NULL)
      return(FAIL);
   *value_ptr = fleet_value(fleet_hndl, GET, attr, NULL);
   return(SUCCESS);
}                              /* end Fleet_get_info */
