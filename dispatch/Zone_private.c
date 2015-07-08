/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Zone_private.c,v $
*  @(#)  $Revision: 1.19 $
*  @(#)  $Date: 2004/06/22 17:08:17 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Zone_private.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Zone_private.c,v 1.19 2004/06/22 17:08:17 jwelch Exp $";

/* Zone_private.c */
#include <sys/types.h>		/* temp */
#include <sys/times.h>		/* temp */
#include "mad_error.h"
#include "taxipak.h"
#include "Object.h"
#include "Vehicle_public.h"
#include "List_public.h"
#include "Call_public.h"
#include "Zone_lists_public.h"
#include "Zone_public.h"
#include "Zone_private.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Fleet_lists_public.h"
#include "mad_ipc.h"
#include "timers.h" 
#include "Timer_public.h"
#include "path.h"
#include "mads_isam.h"
#include "key.h"
#include "language.h"
#include "dispatch.h"
#include "enhance.h"
#include "switch_ext.h"

extern FLEET *fleet[ ]; /* temporary */
extern int state_debug;
extern time_t mads_time;
extern int debug_state;
extern char req_buf[];
extern struct offsets *offset;	/* pointer to offset structure */

RET_STATUS Zone_rec_2_obj();

#ifdef DEBUG_OO
int   zone_state_names_switch = FALSE;
char *zone_state_name[20];

#endif
/* ADD HEADER */
RET_STATUS
Zone_get_hndl(
   short zone_nbr,
   short fleet_nbr,
   ZONE_HNDL * hndl_ptr)
{
   FLEET *fl_ptr;
   struct zones *zone_ptr;
   int ii;
   char error_str[80];

   *hndl_ptr = (ZONE_HNDL) HNDL_ERR; /* in case we fail */
   if (fleet_nbr > FLEET_MAX || fleet_nbr < 0)
   {
      sprintf(error_str, "Fleet nbr %d wrong", fleet_nbr);
      ERROR(' ',"",error_str);
      return(FAIL);
   }
   if (zone_nbr >= ZONE_MAX)
   {
      sprintf(error_str,"Zone nbr %d too large", zone_nbr);
      ERROR(' ',"",error_str);
      return(FAIL);
   }
   if (zone_nbr <=  0)
   {
#ifdef NO_ZONE_ERROR 
      if (zone_nbr == 0)
      {
         ERROR(' ',"","Zone nbr zero");
      }
      else
      {
         ERROR(' ',"","Zone nbr too small");
      }
#endif
      return(FAIL);
   }
   fl_ptr = fleet[fleet_nbr];
   if (fl_ptr->zone_ptr[zone_nbr] != NULL)
   {
      *hndl_ptr = (ZONE_HNDL) fl_ptr->zone_ptr[zone_nbr];
      return (SUCCESS);
   }
   return(FAIL);
}                               /* end Zone_get_hndl */

struct zones *
zone_hndl_to_ptr(zone_hndl)
   ZONE_HNDL zone_hndl;
{
   return ((struct zones *) zone_hndl);
}				/* end zone_hndl_to_ptr */

HNDL
zone_value(zone_hndl, op, attr, value)
   ZONE_HNDL zone_hndl;
   int   op;
   int   attr;
   HNDL  value;
{
   struct zones *zone_ptr = NULL;
   FLEET_HNDL fl_hndl;
   char eror_str[80];
   ZONE_VEH_LIST_HNDL veh_list_hndl;
   VEH_HNDL   veh_hndl;
   int  return_count = 0;

   if (zone_hndl != NULL)
       zone_ptr = zone_hndl_to_ptr(zone_hndl);
   else
   {
       if (op == PUT)
          return((HNDL) FAIL);
       else
          return(HNDL_ERR);
   }

   if (zone_ptr == NULL)
   {
       if (op == PUT) 
          return((HNDL) FAIL); 
       else
          return(HNDL_ERR);
   }

   switch (attr)
   {
   case ZONE_AVAIL_CALLS_LIST:
      if (op == PUT)
      {
#ifdef ASSERT
         if (value == (HNDL) NULL)
            printf("Setting zone %d call_list to NULL\n", zone_ptr->zone_nbr);
#endif
	 zone_ptr->calls_hndl = value;  
      }
      else
      {
	 return((HNDL) zone_ptr->calls_hndl);
      }
      break;

    case ZONE_TC_LIST:
      if (op == PUT)
	zone_ptr->time_calls_hndl = value;  
      else
	return((HNDL) zone_ptr->time_calls_hndl);
      break;

   case ZONE_AVAIL_VEH_LIST:
      if (op == PUT)
      {
#ifdef ASSERT
         if (value == (HNDL) NULL)
            printf("Setting zone %d veh_list to NULL\n", zone_ptr->zone_nbr);
#endif
	 zone_ptr->veh_hndl = value;  
      }
      else
      {
	 return((HNDL) zone_ptr->veh_hndl);
      }
      break;
   case ZONE_DESCRIPTION:
      if (op == PUT)
      {
         ERROR(' ',"","ZONE_DESCRIPTION PUT not implemented");
         return((HNDL) FAIL);
      }
      else
          return((HNDL) zone_ptr->description);
   case ZONE_NBR:
      if (op == PUT)
      {
#ifdef ASSERT
         if (value == (HNDL) 0)
             printf("Setting zone nbr to 0\n");
#endif
	 zone_ptr->zone_nbr = (short) value;
      }
      else
      {
	 return((HNDL) zone_ptr->zone_nbr);
      }
      break;
   case ZONE_FLEET_NBR:
      if (op == PUT)
	 zone_ptr->fleet_nbr = (short) value;
      else
      {
	 return((HNDL) zone_ptr->fleet_nbr);
      }
      break;

    case ZONE_GPS_RADIUS:
      if ( op == PUT )
	zone_ptr->gps_radius = (int) value;
      else
	return((HNDL) zone_ptr->gps_radius);
      break;

    case ZONE_GPS_DIST_CEILING:
      if ( op == PUT )
	zone_ptr->gps_dist_ceiling = (int) value;
      else
	return((HNDL) zone_ptr->gps_dist_ceiling);
      break;

   case ZONE_EMERG_RANGE:
     if ( op == PUT )
       zone_ptr->emerg_range = (int) value;
     else
       return((HNDL) zone_ptr->emerg_range);
     break;

   case ZONE_STAND_TIMER:
     if ( op == PUT )
       zone_ptr->stand_hold_timer = (short) value;
     else
       return((HNDL) zone_ptr->stand_hold_timer);
     break;

   case ZONE_ROTI:
     if ( op == PUT )
       zone_ptr->roti_time = (short)value;
     else
       return((HNDL)zone_ptr->roti_time);
     break;
     

    case ZONE_MATCH_ALGORITHM:
      if ( op == PUT )
	zone_ptr->match_algorithm = (char) value;
      else
	return ((HNDL) zone_ptr->match_algorithm);
      break;

   case ZONE_TC_LOOK_AHEAD:
     if ( op == PUT )
       zone_ptr->tc_look_ahead = (short) value;
     else
       return ((HNDL) zone_ptr->tc_look_ahead);
     break;
	       
   case ZONE_TC_RESTRICT_INTERVAL:
     if ( op == PUT )
       zone_ptr->tc_restrict_interval = (short) value;
     else
       return ((HNDL) zone_ptr->tc_restrict_interval);
     break;

   case ZONE_NS_SHORT:
     if ( op == PUT )
       zone_ptr->ns_short = (short) value;
     else
	 {
	     if ( zone_ptr->ns_short > 0 )
		 return ((HNDL) zone_ptr->ns_short);
	     else
		 {
		     (void)Fleet_get_hndl(BY_NBR, zone_ptr->fleet_nbr, &fl_hndl);
		     return ((HNDL) Fleet_get_value(fl_hndl, FLEET_NS_SHORT));
		 }
	 }
     break;

    case ZONE_EPOST_LEAD_TIME:
      if ( op == PUT )
	zone_ptr->e_post_lead_time = (short) value;
      else
	return((HNDL) zone_ptr->e_post_lead_time);
      break;
	       

   case ZONE_NBR_CALLS_WAIT_EPOST:
      switch (op)
      {
      case GET:
         return((HNDL) zone_ptr->nbr_calls_epost_wait);
      case PUT:
         zone_ptr->nbr_calls_epost_wait = (int) value;
         break;
      case DECREMENT:
         zone_ptr->nbr_calls_epost_wait--;
         break;
      case INCREMENT:
         zone_ptr->nbr_calls_epost_wait++;
         break;
      default:
         printf("ERROR - ILLEGAL op %d for ZONE_NBR_BID_CALLS\n", op);
         return((HNDL)FAIL);
      }   /* end case */
      break;

   case ZONE_NBR_ENROUTE:
      switch (op)
      {
      case GET:
         return((HNDL) zone_ptr->num_enroute);
      case PUT:
         zone_ptr->num_enroute = (int) value;
         break;
      case DECREMENT:
         zone_ptr->num_enroute--;
         break;
      case INCREMENT:
         zone_ptr->num_enroute++;
         break;
      default:
         printf("ERROR - ILLEGAL op %d for ZONE_NBR_ENROUTE\n", op);
         return((HNDL)FAIL);
      }   /* end case */
      if (zone_ptr->num_enroute < 0)
      {
#ifdef ASSERT
          printf("ASSERT ERROR negative ZONE_NBR_ENROUTE\n");
#endif
          ERROR(' ',"","negative ZONE_NBR_ENROUTE");
          zone_ptr->num_enroute = 0;
          return((HNDL) HNDL_ERR);
      }
      break;
   case ZONE_NBR_VEH:
      switch (op)
      {
      case GET:
        return_count = 0;
        veh_list_hndl = (ZONE_VEH_LIST_HNDL) Zone_get_value( zone_hndl, ZONE_AVAIL_VEH_LIST );
        Zone_veh_list_each_get( veh_list_hndl, veh_hndl )
          {
            if ( (!Veh_get_state( veh_hndl, VEH_ON_BREAK_STATE ) ) &&
                 (!Veh_get_state( veh_hndl, VEH_EPOSTED_STATE ) ) )
              ++return_count;
          } Zone_veh_list_each_end(veh_list_hndl)
              
         return((HNDL) return_count);
      case PUT:
         zone_ptr->num_taxis = (int) value;
         break;
      case DECREMENT:
         zone_ptr->num_taxis--;
         break;
      case INCREMENT:
         zone_ptr->num_taxis++;
         break;
      default:
         printf("ERROR - ILLEGAL op %d for ZONE_NBR_VEH\n", op);
         return((HNDL)FAIL);
      }   /* end case */
      if (zone_ptr->num_taxis < 0)
      {
#ifdef ASSERT
          printf("ASSERT ERROR negative ZONE_NBR_VEH\n");
#endif
          ERROR(' ',"","negative ZONE_NBR_VEH");
          zone_ptr->num_taxis = 0;
          return((HNDL) HNDL_ERR);
      }
      break;
   case ZONE_NBR_CALLS:
      switch (op)
      {
        case PUT:
           if ((zone_ptr->num_calls == 0) && value)
              Fleet_zone_calls_list_add(fleet[zone_ptr->fleet_nbr]->fleet_zone_calls_list_hndl, (ZONE_HNDL) zone_ptr);
           else
             if ((zone_ptr->num_calls) && !value)
                Fleet_zone_calls_list_remove(fleet[zone_ptr->fleet_nbr]->fleet_zone_calls_list_hndl, (ZONE_HNDL) zone_ptr);
           zone_ptr->num_calls = (short) value;
           break;
        case GET:
           return((HNDL) zone_ptr->num_calls);
        case INCREMENT:
           if (zone_ptr->num_calls == 0)
              Fleet_zone_calls_list_add(fleet[zone_ptr->fleet_nbr]->fleet_zone_calls_list_hndl, (ZONE_HNDL) zone_ptr);
           zone_ptr->num_calls++; 
           break;
        case DECREMENT:
           zone_ptr->num_calls--;
           if (zone_ptr->num_calls == 0) 
                Fleet_zone_calls_list_remove(fleet[zone_ptr->fleet_nbr]->fleet_zone_calls_list_hndl, (ZONE_HNDL) zone_ptr);
           break;
        default: 
           return(HNDL_ERR);
      }
#ifdef ASSERT
      if (zone_ptr->num_calls < 0)
      {
          printf("ASSERT ERROR negative ZONE_NBR_CALLS\n");
          return((HNDL)FAIL);
      }
#endif
      break;

   case ZONE_AUTO_INQRY:
     if (op == GET)
       return ((HNDL) zone_ptr->inquiry_backup);
     else
       {
            ERROR(' ',"","ZONE_AUTO_INQRY PUT not implemented");
            if (op == PUT)
               return((HNDL) FAIL);
            else
               return((HNDL)FAIL);
        }
     
   case ZONE_SEALS:
        if (op == GET)
           return((HNDL) &zone_ptr->pckup_seals);
        else
        {
            ERROR(' ',"","ZONE_SEALS PUT not implemented");
            if (op == PUT)
               return((HNDL) FAIL);
            else
               return((HNDL)FAIL);
        }
      
    case ZONE_STAND_STATUS: /* time dependent */
      if (op == GET)
        {
	  if ( (zone_ptr->type != ZN_STAND) && !(0) )
            return((HNDL) ZONE_STAND_OPEN);
	  if (zone_ptr->stand_time_from > 0 || zone_ptr->stand_time_to > 0)
	    {
	      if ((short)get_short_time(mads_time) < zone_ptr->stand_time_from || 
		  (short) get_short_time(mads_time) > zone_ptr->stand_time_to)
		return((HNDL)ZONE_STAND_CLOSED);
	    }
	  
	  return((HNDL)ZONE_STAND_OPEN);
        }
      else
	{
	  ERROR(' ',"","Illegal ZONE_STAND_STATUS operation");
	  if (op == PUT)
	    return((HNDL) FAIL);
	  else
	    return(HNDL_ERR);
	}
    case ZONE_TYPE:
      if (op == GET)
	return((HNDL) zone_ptr->type);
      else
	{ 
	  ERROR(' ',"","Illegal ZONE_TYPE operation"); 
	  if (op == PUT) 
	    return((HNDL) FAIL); 
	  else 
	    return(HNDL_ERR); 
	}
      

    case ZONE_X_LEAD_TIME:
      if ( op == GET )
	{
	  if ( zone_ptr->zone_nbr == WAKEUP_ZONE )
	    {
	      Fleet_get_hndl(BY_NBR, zone_ptr->fleet_nbr, &fl_hndl);
	      return(Fleet_get_value(fl_hndl, FLEET_WAKEUP_LEAD_TIME));
	    }
	  else if ( zone_ptr->typex_lead_time >= 0 )
	    return((HNDL) zone_ptr->typex_lead_time );
	}
      
    case ZONE_LEAD_TIME:
      if ( op == GET )
	{
	  if ( zone_ptr->zone_nbr == WAKEUP_ZONE )
	    {
	      Fleet_get_hndl(BY_NBR, zone_ptr->fleet_nbr, &fl_hndl);
	      return(Fleet_get_value(fl_hndl, FLEET_WAKEUP_LEAD_TIME));
	    }
	  else if ( zone_ptr->pickup_lead_time >= 0 )
	    return((HNDL) zone_ptr->pickup_lead_time );
	}
      
    case ZONE_TIMEOUT_TIME: /* WAS avl_call_q_time */
      if (op == GET)
	{
        if(zone_ptr->avl_call_q_time >0 )
	  return((HNDL) zone_ptr->avl_call_q_time);
        else
	  {
	    Fleet_get_hndl(BY_NBR, zone_ptr->fleet_nbr, &fl_hndl);
           return(Fleet_get_value(fl_hndl, FLEET_UNASSIGNED_TIME)); 
        }
      }
   default:
      sprintf(error_str, "ILLEGAL VALUE FOR zone_access %d", attr);
      ERROR(' ',"",error_str);
      if (op == PUT)
         return((HNDL) FAIL);
      else
         return(HNDL_ERR);
   }				/* end case */
   return((HNDL) SUCCESS);
}				/* end zone_value */

HNDL
Zone_get_value(zone_hndl, attr)
   ZONE_HNDL zone_hndl;
   int   attr;
{
  return (zone_value(zone_hndl, GET, attr, NULL));
}				/* end Zone_get_value */

RET_STATUS
Zone_get_info(zone_hndl, attr, value_ptr)
   ZONE_HNDL zone_hndl;
   int   attr;
   HNDL * value_ptr;
{
   if (value_ptr == NULL)
      return(FAIL);
   
   *value_ptr = zone_value(zone_hndl, GET, attr, NULL);
   
   return(SUCCESS);
}                              /* end Zone_get_info */

RET_STATUS
Zone_set_value(zone_hndl, attr, hndl)
   ZONE_HNDL zone_hndl;
   int   attr;
   HNDL  hndl;
{
   HNDL  status;

   if ((attr >= STATS_START) && (attr <= STATS_END))
   {
      printf("ERROR - attempt to set  zone value %d that is a stat \n", attr);
      return (FAIL);		/* cant set a stat */
   }
   if (zone_hndl == (ZONE_HNDL) NULL)
      return (FAIL);
   
   status = zone_value(zone_hndl, PUT, attr, hndl);
   
   return ((short) status);
}				/* end Zone_set_value */

RET_STATUS      
Zone_increment_stats(zone_hndl, attr)   
   ZONE_HNDL zone_hndl;
   int   attr;
{
   HNDL status;

   /* TODO ext stats */
   if ((attr >= STATS_START) && (attr <= STATS_END))
   {
    status = zone_value(zone_hndl, INCREMENT, attr, NULL);
    if (status != HNDL_ERR)
         return(SUCCESS);
   }
   return(FAIL);
}                               /* end zone_increment_stats */

RET_STATUS      
Zone_decrement_stats(zone_hndl, attr)   
   ZONE_HNDL zone_hndl;
   int   attr;
{
   HNDL status;

   /* TODO ext stats */
   if ((attr >= STATS_START) && (attr <= STATS_END))
   {
      status = zone_value(zone_hndl, DECREMENT, attr, NULL);
      if (status != HNDL_ERR)
         return(SUCCESS);
   }
   return(FAIL);
}                               /* end zone_decrement_stats */

RET_STATUS      
Zone_initialize_stats(zone_hndl, attr)    
   ZONE_HNDL zone_hndl;
   int   attr;
{
   HNDL status;

   /* TODO ext stats */
   if ((attr >= STATS_START) && (attr <= STATS_END))
   {
      status = zone_value(zone_hndl, PUT, attr, 0);  /* MAY WANT INITIALIZE dir */
      if (status == HNDL_ERR)
           return(FAIL);
      return(SUCCESS);
   }
   return(FAIL);
}                               /* end zone_decrement_stats */


/*******************************************************************************
* Zone_maint - Update the zone structure in memory.
******************************************************************************/
Zone_maint(rec_nbr, zone_nbr, fleet_id)
   int   rec_nbr;
   int   zone_nbr;
   char  fleet_id;
{
   struct zones *zone_tmp_ptr;	/* pointer to a zone structure */
   ZONE_REC *zn_ptr;		/* pointer to a cisam zone structure */
   int   fl_nbr;
   FLEET_HNDL fleet_hndl;
   char error_str[80];

#ifdef DEBUG
   if (offset->prattle >= LEVEL3)
   {
      sprintf(trace_str, "zone_maint : rec_nbr = %d  zone_nbr = %d fleet = %c \n", rec_nbr, zone_nbr, fleet_id);
      TRACE(trace_str);
   }
#endif

   zn_ptr = (ZONE_REC *) req_buf;

   db(ZONE_FILE_ID, OPEN, &zn_key1, ISINOUT | ISMANULOCK, 0);

   zn_ptr->fleet = fleet_id;
   zn_ptr->nbr = zone_nbr;
   if (db(ZONE_FILE_ID, READ_KEYS, &zn_key1, ISEQUAL, 0) < 0)
   {
      if (iserrno == ENOREC)
      {
	 sprintf(error_str, "zone_maint; Zone %d removed from db?", zone_nbr);
	 ERROR(' ', ZONE_FILE, error_str);
      } else
      {
	 sprintf(error_str, "zone_maint; C-isam error %d", iserrno);
	 ERROR(' ', ZONE_FILE, error_str);
      }
      db(ZONE_FILE_ID, CLOSE, 0, 0, 0);
      return;
   }
   if (zn_ptr->type == '3')
   {				/* don't touch superzones */
      return;
   }
   if (Fleet_get_hndl(BY_ID, fleet_id, &fleet_hndl) == FAIL)
   {
      ERROR(' ', "", "zone_maint - cannot find fleet.\n");
      db(ZONE_FILE_ID, CLOSE, &zn_key1, 0, 0);	/* close the file */
      return;
   }
   fl_nbr = (short) Fleet_get_value(fleet_hndl, FLEET_NBR);

   if (fleet[fl_nbr]->zone_ptr[zone_nbr] == NULL)
     {				/* zone does not exist */
       fleet[fl_nbr]->zone_ptr[zn_ptr->nbr] = (struct zones *) Zone_create(zn_ptr->nbr,fl_nbr, zn_ptr);
       
   } else
     Zone_rec_2_obj(fleet[fl_nbr]->zone_ptr[zone_nbr], zn_ptr);
   
   build_inv_zones(fl_nbr);	/* rebuild the inverted zone structure */

   db(ZONE_FILE_ID, CLOSE, &zn_key1, 0, 0);	/* close the file */
   return;
}

/****************************************************************************
* Zone_rec_2_obj - Copy data from the disk file structure to the internal
* structure.
*****************************************************************************/
RET_STATUS
Zone_rec_2_obj(dest, src)
   ZONE *dest;
   ZONE_REC *src;
{
   register i, j;

#ifdef DEBUG
   if (offset->prattle >= LEVEL2)
   {
      sprintf(trace_str, "Zone_rec_2_obj : zone = %d fleet = %c src = %x dest = %x \n", src->nbr, src->fleet, src, dest);
      TRACE(trace_str);
   }
#endif

   dest->super_zone = src->superzn_nbr;
   dest->type = src->type;
   strcpy(dest->description, src->desc);
   dest->fleet_call_offer = src->bid_allowed;

   if (src->pckup_seals[0] == YES)
      dest->pckup_seals.s1 = 1;
   else
      dest->pckup_seals.s1 = 0;

   if (src->pckup_seals[1] == YES)
      dest->pckup_seals.s2 = 1;
   else
      dest->pckup_seals.s2 = 0;

   if (src->pckup_seals[2] == YES)
      dest->pckup_seals.s3 = 1;
   else
      dest->pckup_seals.s3 = 0;

   if (src->pckup_seals[3] == YES)
      dest->pckup_seals.s4 = 1;
   else
      dest->pckup_seals.s4 = 0;

   if (src->pckup_seals[4] == YES)
      dest->pckup_seals.s5 = 1;
   else
      dest->pckup_seals.s5 = 0;

   if (src->pckup_seals[5] == YES)
      dest->pckup_seals.s6 = 1;
   else
      dest->pckup_seals.s6 = 0;

   if (src->pckup_seals[6] == YES)
      dest->pckup_seals.s7 = 1;
   else
      dest->pckup_seals.s7 = 0;

   if (src->pckup_seals[7] == YES)
      dest->pckup_seals.s8 = 1;
   else
      dest->pckup_seals.s8 = 0;

   if (src->pckup_seals[8] == YES)
      dest->pckup_seals.s9 = 1;
   else
      dest->pckup_seals.s9 = 0;

   if (src->pckup_seals[9] == YES)
      dest->pckup_seals.s10 = 1;
   else
      dest->pckup_seals.s10 = 0;

   if (src->pckup_seals[10] == YES)
      dest->pckup_seals.s11 = 1;
   else
      dest->pckup_seals.s11 = 0;

   if (src->pckup_seals[11] == YES)
      dest->pckup_seals.s12 = 1;
   else
      dest->pckup_seals.s12 = 0;

   if (src->pckup_seals[12] == YES)
      dest->pckup_seals.s13 = 1;
   else
      dest->pckup_seals.s13 = 0;

   if (src->pckup_seals[13] == YES)
      dest->pckup_seals.s14 = 1;
   else
      dest->pckup_seals.s14 = 0;

   if (src->pckup_seals[14] == YES)
      dest->pckup_seals.s15 = 1;
   else
      dest->pckup_seals.s15 = 0;

   if (src->pckup_seals[15] == YES)
      dest->pckup_seals.s16 = 1;
   else
      dest->pckup_seals.s16 = 0;

   dest->pickup_lead_time = src->pckup_lead_time * 60;
   dest->stand_time_from = src->stand_time_from;
   dest->stand_time_to = src->stand_time_to;
   if (src->avl_call_q_time > 999)
       src->avl_call_q_time = 999;
   dest->avl_call_q_time = src->avl_call_q_time * 60;
   dest->enr_post_allow = src->enr_post_allow;
   dest->e_post_lead_time = src->enr_post_lead;
   dest->mdt_screen = src->mdt_screen;

   dest->gps_lat = src->gps_lat;
   dest->gps_long = src->gps_long;
   dest->gps_radius = src->gps_radius;
   dest->gps_dist_ceiling = src->gps_dist_ceiling;
   dest->match_algorithm = src->match_algorithm;
   
   dest->break_duration = src->break_duration;
   dest->accept_duration = src->accept_duration;
   dest->stand_time_from = src->stand_time_from;
   dest->stand_time_to = src->stand_time_to;
   dest->check_avail_taxis = src->check_avail_taxis;
   
   for (i = 0; i < LEVEL_MAX; i++)
   {
      dest->level_exclusive_time[i] = src->lev_excl_time[i] * 60;
      dest->level_reject_penalty[i] = src->lev_rej_penlty[i];
      if (i == 1)
      {
	 dest->level_algorithm[i] = SEQ_ZONE_TEMP_CAB;	/* stuff with first cab
							 * first zone alg */
	 for (j = 0; j < BCK_UP_MAX; j++)
	 {
	    dest->level_number[i].bck_up_zone[j] = 0;	/* make primary with no
							 * backups */
	 }
	 dest->level_number[1].bck_up_zone[0] = src->nbr;	/* make primary backup
								 * itself */
      } else
      {
	 dest->level_algorithm[i] = src->lev_alg[i];
	 for (j = 0; j < BCK_UP_MAX; j++)
	 {
	    dest->level_number[i].bck_up_zone[j] = src->lev_zone_nbr[i][j];
	 }
      }
   }
   dest->tc_look_ahead = src->tc_look_ahead;
   dest->tc_restrict_interval = src->tc_restrict_interval;
   dest->ns_short = src->ns_short;
   dest->emerg_range = src->emerg_range;
   dest->stand_hold_timer = src->stand_hold_timer;
   dest->cbook_time = src->cbook_time * 60;
   dest->roti_time = src->roti_time;
   for (j=0;j<ZONE_INQUIRY_BACKUP_ZONES;j++)
     dest->inquiry_backup[j] = src->inquiry_backup[j];
   dest->typex_lead_time = src->typex_lead_time * 60;
   return (SUCCESS);
}				/* end Zone_rec_2_obj */

ZonePoly_maint( zone_nbr, fleet_id )
     int zone_nbr;
     char fleet_id;
{
  FLEET_HNDL  fleet_hndl;
  struct zones *zone_ptr;
  int         i;
  short       fl_nbr;
  short       zn_nbr;
  ZONE_POLY_REC zonepoly_rec;
  int         mode, point_index, done = 0;

  zn_nbr = (short)zone_nbr;
  
  if ( Fleet_get_hndl( BY_ID, fleet_id, &fleet_hndl ) == FAIL )
    {
      ERROR(fleet_id, ZONEPOLY_FILE, "zonepoly_maint -- no fleet\n");
      return;
    }

  fl_nbr = (short) Fleet_get_value( fleet_hndl, FLEET_NBR );

  if ( (zone_ptr = fleet[ fl_nbr ]->zone_ptr[ zn_nbr ]) == NULL )
    {
      ERROR(fleet_id, ZONEPOLY_FILE, "zonepoly_maint -- no zone\n");
      return;
    }

  for ( i = 0; i < zone_ptr->poly_points; i++ )
    {
      if ( i >= MAX_POLY_POINTS )
        break;
      
      zone_ptr->zone_poly[i].x = 0.0;
      zone_ptr->zone_poly[i].y = 0.0;
    }
  zone_ptr->poly_points = 0;

  /* no reread the points */
  db_open(ZONEPOLY_FILE_ID, ISINOUT + ISMANULOCK);  
  point_index = 0;
  mode = ISEQUAL;
  zonepoly_rec.fleet = fleet_id;
  zonepoly_rec.nbr = zn_nbr;
  zonepoly_rec.seq_nbr = 1;
  done = 0;
  while ( !done )
    {
      if ( db_read_key(ZONEPOLY_FILE_ID, &zonepoly_rec, &znpoly_key1, mode ) == SUCCESS )
	{
	  mode = ISNEXT;
	  if ( zonepoly_rec.nbr != zn_nbr )
	    done = TRUE;
	  else
	    {
	      zone_ptr->zone_poly[point_index].x = zonepoly_rec.pointx;
	      zone_ptr->zone_poly[point_index].y = zonepoly_rec.pointy;
	      ++point_index;
	    }
	}
      else
	done = 1;
    }
  if ( point_index > 0 )
    {
      zone_ptr->poly_points = point_index;
      ZonePolyCalculateCentroid( (ZONE_HNDL)zone_ptr );
    }
  db_close(ZONEPOLY_FILE_ID);
}
  
ZonePolyCalculateCentroid( zone_hndl )
     ZONE_HNDL  zone_hndl;
{
  int   N, i;
  double x, y;
  ZONE   *zone_ptr;
  double running_sumx, running_sumy;
  POINT_POLY  *zone_poly;

  zone_ptr = (ZONE *) zone_hndl;
  N = zone_ptr->poly_points;
  zone_poly = (POINT_POLY *) zone_ptr->zone_poly;
  running_sumx = 0.0;
  running_sumy = 0.0;
  
  for ( i = 0; i < N; i++ )
    {
      running_sumx += zone_poly[i].x;
      running_sumy += zone_poly[i].y;
    }

  x = running_sumx / N;
  y = running_sumy / N;

  zone_ptr->poly_centroid_x = x;
  zone_ptr->poly_centroid_y = y;
  
}
