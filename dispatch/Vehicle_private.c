/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Vehicle_private.c,v $
*  @(#)  $Revision: 1.68 $
*  @(#)  $Date: 2004/06/22 17:08:17 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Vehicle_private.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Vehicle_private.c,v 1.68 2004/06/22 17:08:17 jwelch Exp $";

#include <sys/types.h>		/* temp */
#include <sys/times.h>		/* temp */
#include <signal.h>
#include "mad_error.h"
#include "taxipak.h"
#include "Object.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "Vehicle_lists_public.h"
#include "Call_public.h"
#include "Call_private.h"
#include "List_public.h"
#include "Zone_public.h"
#include "Zone_lists_public.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Fleet_lists_public.h"
#include "dispatch.h"
#include "msg_defs.h"
#include "acc.h"
#include "kdt.h"
#include "msg_mmp.h"
#include "msg.h"
#include "udp.h"
#include "Driver_private.h"
#include "mad_ipc.h"
#include "path.h"
#include "except.h"
#include "timers.h"
#include "Call_timers_public.h"
#include "dispatch_strdefs.h"
#include "f_msg_defs.h"
#include "user.h"
#include "language.h"
#include "mads_isam.h"
#include "key.h"
#include "df_maint.h"
#include "Timer_public.h"
#include "GPS_private.h"
#include "enhance.h"
#include "switch_ext.h"
#include "itm.h"
#include "S_Shift_db.h"
#include "S_Sched_db.h"

#define NO_POST_CHECK ((HNDL) 10)

extern char req_buf[];
extern struct offsets *offset;
extern time_t mads_time;
extern FLEET *fleet[ ];	/* shared memory fleet structure */
extern int veh_used;
extern int warm_start;

extern BOOLEAN Time_in_Shift( int, char, int );
extern short debug_state;
int   veh_state_names_switch = FALSE;
char *veh_state_name[40];
int get_out_text_size();
RET_STATUS Veh_check_suspensions( VEH_HNDL );

/****************************************************************************
* Veh_rec_2_obj - Copy data from the disk file structure to the internal
* structure.
*
* !!! The bit techniques used here are possibly non-portable.
*****************************************************************************/
RET_STATUS
Veh_rec_2_obj(VEH *dest, VEH_REC *src)
{
   unsigned char mask;		/* holds the bit mask value */
   unsigned char *addr;		/* used as a pointer to a bit structure */
   register i, j, ii;
   FLEET_HNDL   fleet_hndl;
   FLEET_SUSP_LIST_HNDL  fleet_susp_list_hndl;
   char         susp_msg[1024];
   unsigned tmp_susp_pass_call;		/* Temporary suspension flag (passenger) */
   unsigned tmp_susp_del_call;		/* Temporary suspension flag (delivery) */
   unsigned tmp_susp_typex_call;		/* Temporary suspension flag (typex) */      

   mk_outb_text("");
   
   dest->fleet_nbr = src->fleet_nbr;
   dest->veh_nbr = src->nbr;
   dest->fleet = src->fleet;

   for (ii = 0; ii < MID_SIZE; ii++)
   {
      if (islower(src->mobile_rf_id[ii]))
	 dest->mid[ii] = Toupper(src->mobile_rf_id[ii]);
      else
	 dest->mid[ii] = src->mobile_rf_id[ii];
   }

   dest->rf_id = (unsigned long) strtol(src->mobile_rf_id, (char **) 0, 16);

   dest->veh_susp_from_date = src->susp_from_dt_tm;
   dest->veh_susp_to_date = src->susp_to_dt_tm;
   dest->veh_susp_d_from_date = src->susp_d_from_dt_tm;
   dest->veh_susp_d_to_date = src->susp_d_to_dt_tm;
   dest->veh_susp_x_from_date = src->susp_x_from_dt_tm;
   dest->veh_susp_x_to_date = src->susp_x_to_dt_tm;

   if (Fleet_get_hndl(BY_ID, dest->fleet, &fleet_hndl) == FAIL)
     {
       sprintf(error_str, "Bad fleet id='%c'. Vehicle=%d", dest->fleet, dest->veh_nbr);
       ERROR(dest->fleet, "",error_str);
     }

   if ( ( dest->veh_susp_from_date < mads_time ) && 
        ( mads_time < dest->veh_susp_to_date )   &&
        ( src->susp_pass_call == YES ) )
     {
       dest->vehicle_attr.susp_pass_call = 1;
       if ( Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_PASS_LIST),
					    dest->veh_nbr) == HNDL_NULL )
	 Fleet_susp_veh_list_add( (FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_PASS_LIST),
				 (VEH_HNDL)dest );
       if ( dest->t_status.inuse )
	 {
	   strcpy( susp_msg, MDT_MSG_VEH_SUSP);
	   add_outb_text( susp_msg );
	   strcpy(susp_msg, MDT_MSG_PASS_TRIP);
	   strcat(susp_msg, src->susp_to_date );
	   strcat(susp_msg, " " );
	   strcat(susp_msg, src->susp_to_time );
	   add_outb_text( susp_msg );
	   add_outb_text(CR);
           //	   send_msg_mmp( dest->mid, TEXT_DISPLAY, dest ); 
	 }
     }
   else
     {
       if ( Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_PASS_LIST),
					    dest->veh_nbr) != HNDL_NULL )
	 {
	   Fleet_susp_veh_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_PASS_LIST),
					     (VEH_HNDL)dest);
	   if ( dest->t_status.inuse )
	     {
	       strcpy( susp_msg, MDT_MSG_VEH_SUSP_END);
	       add_outb_text( susp_msg );
	       add_outb_text( CR );
	       strcpy(susp_msg, MDT_MSG_PASS_TRIP_END);
	       add_outb_text( susp_msg );
	       add_outb_text( CR );
               //	       send_msg_mmp( dest->mid, TEXT_DISPLAY, dest );
	     }
	 }
	       
     }
       
   if ( ( dest->veh_susp_d_from_date < mads_time ) &&
        ( mads_time < dest->veh_susp_d_to_date )   &&
        ( src->susp_del_call == YES ) )
     {
       dest->vehicle_attr.susp_del_call = 1;
       if ( Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_DELIV_LIST),
				     dest->veh_nbr) == HNDL_NULL )       
	 Fleet_susp_veh_list_add( (FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_DELIV_LIST),
				 (VEH_HNDL)dest );
       if ( dest->t_status.inuse )
	 {
	   strcpy( susp_msg, MDT_MSG_VEH_SUSP);
	   add_outb_text( susp_msg );
	   strcpy(susp_msg, MDT_MSG_DELIV_TRIP);
	   strcat(susp_msg, src->susp_d_to_date );
	   strcat(susp_msg, " " );
	   strcat(susp_msg, src->susp_d_to_time );
	   add_outb_text( susp_msg );
	   add_outb_text(CR);
           //	   send_msg_mmp( dest->mid, TEXT_DISPLAY, dest ); 
	 }       
     }
   else
     {
       if ( Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_DELIV_LIST),
					    dest->veh_nbr) != HNDL_NULL )
	 {
	   Fleet_susp_veh_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_DELIV_LIST),
					     (VEH_HNDL)dest);
	   if ( dest->t_status.inuse )
	     {
	       strcpy( susp_msg, MDT_MSG_VEH_SUSP_END);
	       add_outb_text( susp_msg );
	       add_outb_text( CR );
	       strcpy(susp_msg, MDT_MSG_DELIV_TRIP_END);
	       add_outb_text( susp_msg );
	       add_outb_text( CR );
               //	       send_msg_mmp( dest->mid, TEXT_DISPLAY, dest );
	     }
	 }
     }
   
   if ( ( dest->veh_susp_x_from_date < mads_time ) &&
        ( mads_time < dest->veh_susp_x_to_date )   &&
        ( src->susp_typex_call == YES ) )
     {
       dest->vehicle_attr.susp_typex_call = 1;
       if ( Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_TYPEX_LIST),
				     dest->veh_nbr) == HNDL_NULL )       
	 Fleet_susp_veh_list_add( (FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_TYPEX_LIST),
				 (VEH_HNDL)dest );
       if ( dest->t_status.inuse )
	 {
	   strcpy( susp_msg, MDT_MSG_VEH_SUSP);
	   add_outb_text( susp_msg );
	   strcpy(susp_msg, MDT_MSG_TYPEX_TRIP);
	   strcat(susp_msg, src->susp_x_to_date );
	   strcat(susp_msg, " " );
	   strcat(susp_msg, src->susp_x_to_time );
	   add_outb_text( susp_msg );
	   add_outb_text(CR);
           //	   send_msg_mmp( dest->mid, TEXT_DISPLAY, dest ); 
	 }       
     }
   else
     {
       if ( Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_TYPEX_LIST),
					    dest->veh_nbr) != HNDL_NULL )
	 {
	   Fleet_susp_veh_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_TYPEX_LIST),
					     (VEH_HNDL)dest);
	   if ( dest->t_status.inuse )
	     {
	       strcpy( susp_msg, MDT_MSG_VEH_SUSP_END);
	       add_outb_text( susp_msg );
	       add_outb_text( CR );
	       strcpy(susp_msg, MDT_MSG_TYPEX_TRIP_END);
	       add_outb_text( susp_msg );
	       add_outb_text( CR );
               //	       send_msg_mmp( dest->mid, TEXT_DISPLAY, dest );
	     }
	 }
     }

   if ( get_outb_text_size() )
     send_msg_mmp( dest->mid, TEXT_DISPLAY, dest );

   if (src->kdt_working == YES)
      dest->t_status.kdt_working = 1;
   else
      dest->t_status.kdt_working = 0;

/*
 *	fix
 *	to allow MID's > MID_MAX for Aerotaxi Vehicles (vehicles with no MDT)
 */
	if (dest->rf_id > MID_MAX)
      dest->t_status.kdt_working = 0;

   if (src->meter_working == YES)
      dest->t_status.meter_working = 1;
   else
      dest->t_status.meter_working = 0;

   dest->term_type = src->term_type;

   addr = (unsigned char *) &dest->vehicle_seals;	/* get pointer to start
							 * of seals struct */
   *addr = 0;
   for (i = 0; i < 2; i++)
   {
      mask = 0x80;		/* set the bit mask */
      for (j = 0; j <= 7; j++)
      {				/* read high bit to low */
	 if (src->seals[j + (i * 8)] == YES)	/* check if bit should be set */
	    *addr |= mask;
	 mask >>= 1;		/* shift the mask for next attribute */
      }
      if (src->seals[j + (i * 8)] == '\0')
	 break;
      ++addr;			/* point to the next 8 bits */
      *addr = 0;
   }
   /* place the bits into the veh attribute bit struct */
   /* save these because attribute code will clear these settings */
   tmp_susp_pass_call = dest->vehicle_attr.susp_pass_call;
   tmp_susp_del_call = dest->vehicle_attr.susp_del_call;
   tmp_susp_typex_call = dest->vehicle_attr.susp_typex_call;

   addr = (unsigned char*)&dest->vehicle_attr;		            
     for(i=0;i<8;i++)
       {
	 mask = 0x80;
	 for (j=0; j<=7;j++)
	   {
	     if(src->attr[j + (i*8)] == YES)
	       {
		 if ( *addr & mask )  /** is attr bit currently set in memory - no action needed **/
		   {
		     mask >>= 1;
		     continue;
		   }
		 else
		   {
		     /** set the bit in the in memory attr structure **/
		     *addr |= mask;					
		     ++fleet[src->fleet_nbr]->fl_veh_attr[j + (i*8)]; /** update total counts **/
		     if(dest->t_status.inuse)
		       ++fleet[dest->fleet_nbr]->now_veh_attr[j + (i*8)]; 
		   }
	       }
	     else 
	       {
		 if ( *addr & mask )  /** is attr bit currently set in memory - clear it **/
		   {
		     *addr ^= mask;
		     --fleet[src->fleet_nbr]->fl_veh_attr[j + (i*8)]; /** update total counts **/
		     if(dest->t_status.inuse)
		       --fleet[dest->fleet_nbr]->now_veh_attr[j + (i*8)];
		   }
	       }
	     mask >>= 1;			       
	   }
	 ++addr;
       }            
      /* now reset suspensions settings */
      dest->vehicle_attr.susp_pass_call = tmp_susp_pass_call;
      dest->vehicle_attr.susp_del_call = tmp_susp_del_call;
      dest->vehicle_attr.susp_typex_call = tmp_susp_typex_call;

   if ( ( mads_time - src->last_emerg_on ) < 86400 )
     dest->emergency_on_date = src->last_emerg_on;
   if ( ( mads_time - src->last_emerg_off ) < 86400 )
     dest->emergency_off_date = src->last_emerg_off;

#ifdef NAMERICA
   dest->radio_channel = -1;
#endif
   
   init_itm_info(dest, src);	/* initialze the itm information */
   dest->CanxType = 0;
   dest->crse_canx_time = 0;
   dest->gps_lat = src->gps_lat;
   dest->gps_long = src->gps_long;
   dest->last_action_time = mads_time;
   dest->veh_epost_timer = HNDL_NULL;
   dest->held_call = HNDL_NULL;
   dest->send_gps_not_in_zone = TRUE;
   dest->roti = FALSE;
   dest->schedule_num = src->schedule_num;
   return(SUCCESS);
}				/* end Veh_rec_2_obj */


void
init_veh_state_names()
{
   veh_state_name[VEH_ACCEPT_STATE - STATE_BASE] = "ACCEPT";
   veh_state_name[VEH_BID_STATE - STATE_BASE] = "BID";
   veh_state_name[VEH_CALL_OFFER_STATE - STATE_BASE] = "CALL_OFFER";
   veh_state_name[VEH_EMERGENCY_STATE - STATE_BASE] = "EMERGENCY";
   veh_state_name[VEH_EPOSTED_STATE - STATE_BASE] = "EPOSTED";
   veh_state_name[VEH_FLAG_STATE - STATE_BASE] = "FLAG";
   veh_state_name[VEH_FLAG_REQUEST_STATE - STATE_BASE] = "FLAG_REQUEST";
   veh_state_name[VEH_IDLE_STATE - STATE_BASE] = "IDLE";
   veh_state_name[VEH_IN_USE_STATE - STATE_BASE] = "IN USE";
   veh_state_name[VEH_LATE_METER_STATE - STATE_BASE] = "LATE_METER";
   veh_state_name[VEH_LOCAL_STATE - STATE_BASE] = "LOCAL";
   veh_state_name[VEH_MDT_WORKING_STATE - STATE_BASE] = "METER WORKING";
   veh_state_name[VEH_METER_ON_STATE - STATE_BASE] = "METER ON";
   veh_state_name[VEH_MSG_STATE - STATE_BASE] = "MSG";
   veh_state_name[VEH_NOSHOW_STATE - STATE_BASE] = "NOSHOW";
   veh_state_name[VEH_OK_TO_MATCH_STATE - STATE_BASE] = "OK_TO_MATCH";
   veh_state_name[VEH_OK_TO_PICKUP_STATE - STATE_BASE] = "OK_TO_PICKUP";
   veh_state_name[VEH_ON_BREAK_STATE - STATE_BASE] = "ON_BREAK";
   veh_state_name[VEH_ON_CALL_STATE - STATE_BASE] = "ON_CALL";
   veh_state_name[VEH_PICKUP_STATE - STATE_BASE] = "PICKUP";
   veh_state_name[VEH_POSTED_STATE - STATE_BASE] = "POSTED";
   veh_state_name[VEH_TALK_STATE - STATE_BASE] = "TALK";
   veh_state_name[VEH_SHORT_METER_STATE - STATE_BASE] = "SHORT_METER";
   veh_state_name[VEH_SIGN_IN_STATE - STATE_BASE] = "SIGN_IN";
   veh_state_name[VEH_SUSPENDED_STATE - STATE_BASE] = "SUSPENDED_VEH";
}				/* end init_veh_state_names() */


struct veh_driv *
veh_hndl_to_ptr(veh_hndl)
   VEH_HNDL veh_hndl;
{
   return ((struct veh_driv *) veh_hndl);
}				/* end veh_hndl_to_ptr */


veh_http_break_notify(veh_ptr)
  struct veh_driv *veh_ptr;
{

  // timer expired for a vehicle that was not on break
  if ( veh_ptr->t_status.on_break == 0 )
    return;
  
  if ( veh_ptr != NULL )
    {
      veh_ptr->on_break_time = 0;
      veh_ptr->t_status.on_break = 0;
    }
}

static void
veh_break_notify(veh_ptr)
     struct veh_driv *veh_ptr;
{
  ZONE_HNDL   zone_hndl;

  // timer expired for a vehicle that was not on break
  if ( veh_ptr->t_status.on_break == 0 )
    return;
  
  if ( veh_ptr != NULL )
    {
      veh_ptr->on_break_time = 0;
      (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_BREAK_STATE, (HNDL)VEH_POST_KEY, (HNDL)1);
    }
  //Need to do some additional 'magic' here so this car does not get automatically booked into
  //zone by the GPS processor. We are creating a special state here IDLE/BREAK
  veh_ptr->t_status.on_break = 1;
}

static void
veh_epost_notify(veh_ptr)
   struct veh_driv *veh_ptr;
{
  ZONE_HNDL   hZone;
  CALL_HNDL   tmp_call_hndl;
  
  if (veh_ptr != NULL)
    {
      veh_ptr->veh_epost_timer = NULL;
      
      Zone_get_hndl( veh_ptr->zone_num, veh_ptr->fleet_nbr, &hZone );
      
      tmp_call_hndl = (CALL_HNDL)Veh_get_value((VEH_HNDL)veh_ptr, VEH_HELD_EPOST_CALL);    
      Zone_release_epost( hZone, (VEH_HNDL)veh_ptr );
      
      if ( tmp_call_hndl != HNDL_NULL )
	Call_reset_state(tmp_call_hndl, CALL_WAIT_EPOST_STATE, NULL);
      
      (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_EPOSTED_STATE, HNDL_NULL, HNDL_NULL);
      
      if ( tmp_call_hndl != HNDL_NULL )
	Zone_check_epost_vehs( hZone, tmp_call_hndl );
      
    }
  else
    {
      ERROR(' ', "", "null vehicle ptr");
      printf("null vehicle ptr in veh_epost_notify\n");
    }
}				/* end veh_epost_notify */

static HNDL
veh_value(veh_hndl, op, attr, value)
   VEH_HNDL veh_hndl;
   int   op;
   int   attr;
   HNDL  value;
{
   struct veh_driv *veh_ptr;
   short q_position;
   ZONE_VEH_LIST_HNDL veh_list_hndl;
   ZONE_HNDL zone_hndl;
   short fl_nbr;
   VEH_HNDL tmp_veh_hndl;
   char  str_buf[80];
   int   cc;
   VEH_REC *veh;
   VEH *vehicle_list_ptr;

   veh_ptr = veh_hndl_to_ptr(veh_hndl);

   switch (attr)
   {
   case VEH_CALL_HNDL:
      if (op == PUT)
	 veh_ptr->call_ptr = (struct calls *) value;
      else
	 return ((HNDL) veh_ptr->call_ptr);
      break;
   case VEH_CALL_OFFER_LEVEL:
      if (op == PUT)
	 veh_ptr->level_offered = (short) value;
      else
	 return ((HNDL) veh_ptr->level_offered);
      break;
    case VEH_CALL_NBR:
      if ( op == PUT )
	veh_ptr->call_nbr = (int) value;
      else
	return ((HNDL) veh_ptr->call_nbr);
      break;
   case VEH_DRIVER_HNDL:
      if (op == GET)
	 return ((HNDL) veh_hndl);
      printf("VEH_DRIVER_HNDL not implemented\n");
      break;
   case VEH_FLEET_NBR:
      if (op == PUT)
	 veh_ptr->fleet_nbr = (short) value;
      else
	 return ((HNDL) veh_ptr->fleet_nbr);
      break;
      
    case VEH_FLEET:
      if ( op == PUT )
	veh_ptr->fleet = (char) value;
      else
	return ((HNDL) veh_ptr->fleet);
      break;
	       
   case VEH_INFO_TIME:
      if (op == PUT)
	 veh_ptr->time_info = (time_t) value;
      else
	 return ((HNDL) veh_ptr->time_info);
      break;
   case VEH_MID:
      if (op == PUT)
	 strncpy(veh_ptr->mid, (char *) value, MID_SIZE);
      else
	 return ((HNDL) veh_ptr->mid);
      break;

   case VEH_IP_ADDRESS:
     if ( op == PUT )
       strncpy(veh_ptr->ip_address, (char *) value, IP_ADDR_SIZE);
     else
       return ((HNDL)veh_ptr->ip_address);
     break;
     
   case VEH_NBR:
      if (op == PUT)
	 veh_ptr->veh_nbr = (short) value;
      else
	 return ((HNDL) veh_ptr->veh_nbr);
      break;

   case VEH_DRV_ID:
     if ( op == PUT )
       veh_ptr->driver_id = (int) value;
     else
       return ((HNDL) veh_ptr->driver_id);
     break;

   case VEH_RF_ID:
     if ( op == PUT )
       veh_ptr->rf_id = (unsigned long)value;
     else
       return ((HNDL) veh_ptr->rf_id);
     break;
	       
   case VEH_Q_POSITION:
      if (op != GET)
      {
	 ERROR(' ', "", "Illegal VEH_Q_POSITION operation");
	 return ((HNDL) FAIL);
      }
      if (veh_ptr == NULL)
	 return ((HNDL) FAIL);

      if (veh_ptr->t_status.posted || veh_ptr->t_status.e_posted)
      {
	 q_position = 0;
	 if (Zone_get_hndl(veh_ptr->zone_num, veh_ptr->fleet_nbr, &zone_hndl) == FAIL)
	 {
	    sprintf(error_str, "bad zone nbr %d", veh_ptr->zone_num);
	    ERROR(' ', "", error_str);
	    return ((HNDL) FAIL);
	 }
	 veh_list_hndl = (ZONE_VEH_LIST_HNDL) Zone_get_value(zone_hndl, ZONE_AVAIL_VEH_LIST);
	 Zone_veh_list_each_get(veh_list_hndl, tmp_veh_hndl)
	 {
	 	vehicle_list_ptr = (VEH *)tmp_veh_hndl;
		q_position++;

                if (vehicle_list_ptr->veh_nbr == veh_ptr->veh_nbr)
                        return ((HNDL) q_position);
	 } Zone_veh_list_each_end(veh_list_hndl)
      }
      sprintf(str_buf, "could not find veh %d zn %d", veh_ptr->veh_nbr, veh_ptr->zone_num);
      ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_buf);
      return ((HNDL) FAIL);
   case VEH_SUSP_MSG:
      if (op == PUT)
	 return ((HNDL) FAIL);
      if (op == GET)
      {
	 veh = (VEH_REC *) req_buf;
	 veh->fleet = fleet[veh_ptr->fleet_nbr]->fleet_id;
	 veh->nbr = veh_ptr->veh_nbr;
	 if ((cc = db(VEHICLE_FILE_ID, READ_KEYS, &veh_key2, ISEQUAL, 0)) < 0)
	 {
	    ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, VEHICLE_FILE,
		  "Cannot find vehicle in C-ISAM data base")
	       return (HNDL_NULL);
	 } else
	    return ((HNDL) veh->susp_message);
      }
      return (HNDL_ERR);
   case VEH_WARM_TIME:
      if (op == PUT)
	 veh_ptr->warm_time = (time_t) value;
      else
	 return ((HNDL) veh_ptr->warm_time);
      break;
   case VEH_ZONE_NBR:
      if (op == PUT)
	 veh_ptr->zone_num = (short) value;
      else
	 return ((HNDL) veh_ptr->zone_num);
      break;			/* INCOMPLETE                */


    case VEH_GPS_TIME:
      if ( op == PUT )
	veh_ptr->gps_time = (time_t) value;
      else
	return ((HNDL) veh_ptr->gps_time);
      break;

    case VEH_GPS_STATE:
      if ( op == PUT )
	veh_ptr->gps_receiver_status = (char)value;
      else
	return ((HNDL) veh_ptr->gps_receiver_status);
      break;
      
    case VEH_HOST_LINK:
      if ( op == PUT )
	veh_ptr->host_link = (char)value;
      else
	return((HNDL)veh_ptr->host_link);
      break;

   case VEH_LINK_TYPE:
     if ( op == PUT )
       veh_ptr->link_type = (char)value;
     else
       return((HNDL)veh_ptr->link_type);
     break;

   case VEH_NAV_TYPE:
     if ( op == PUT )
       veh_ptr->nav_type = (char)value;
     else
       return((HNDL)veh_ptr->nav_type);
     break;

   case VEH_ROTI_STATE:
     if ( op == PUT )
       veh_ptr->roti = (BOOLEAN)value;
     else
       return((HNDL)veh_ptr->roti);
     break;
     
   case VEH_SEND_GPS_NOT_IN_ZONE:
     if ( op == PUT )
       veh_ptr->send_gps_not_in_zone = (BOOLEAN)value;
     else
       return((HNDL)veh_ptr->send_gps_not_in_zone);
     break;
     
   case VEH_HELD_EPOST_CALL:
     if ( op == PUT )
       veh_ptr->held_call = (CALL_HNDL)value;
     else
       return((HNDL)veh_ptr->held_call);
     break;
      
    case VEH_LAST_MSG_NBR:
      if ( op == PUT )
	veh_ptr->last_msg_nbr = (int) value;
      else
	return ((HNDL) veh_ptr->last_msg_nbr);
      break;
    case VEH_TERM_TYPE:
      if ( op == PUT )
	veh_ptr->term_type = (short) value;
      else
	return ((HNDL) veh_ptr->term_type);
      break;

    case VEH_CRSE_CANX_TIME:
      if (op == PUT)
	veh_ptr->crse_canx_time = (time_t) value;
      else
	return ((HNDL) veh_ptr->crse_canx_time);
      break;

    case VEH_CANX_TYPE:
      if (op == PUT)
	veh_ptr->CanxType = (short) value;
      else
	return ((HNDL) veh_ptr->CanxType);
      break;      
		
   default:
      printf("ILLEGAL VALUE FOR veh_access %d\n", attr);
      if (op == PUT)
	 return ((HNDL) FAIL);
      else
	 return ((HNDL) NULL);
      break;
   }				/* end case */
   return ((HNDL) SUCCESS);
}				/* end veh_value */

HNDL
Veh_get_value(veh_hndl, attr)
   VEH_HNDL veh_hndl;
   int   attr;
{
  return ((HNDL) veh_value(veh_hndl, GET, attr, HNDL_NULL));
}				/* end Veh_get_value */

RET_STATUS
Veh_set_value(veh_hndl, attr, hndl)
   VEH_HNDL veh_hndl;
   int   attr;
   HNDL  hndl;
{
   HNDL  status;

   if ((attr >= STATS_START) && (attr <= STATS_END))
   {
      printf("ERROR - attempt to set  veh value %d that is a stat \n", attr);
      return (FAIL);		/* cant set a stat */
   }
   if (veh_hndl == (VEH_HNDL) NULL)
      return (FAIL);

   status = (HNDL) veh_value(veh_hndl, PUT, attr, hndl);
   
   return ((short) status);
}				/* end Veh_set_value */

#ifdef ASSERT
static int
assert_check_op(op)
   int   op;
{
   /* TODO */
   return (FALSE);
}				/* end assert_check_op */
#endif

RET_STATUS
veh_bit_update(veh_ptr, direction)	/* WAS part of d_sign_in */
   struct veh_driv *veh_ptr;
   short direction;
{
   unsigned char *veh_bit_ptr;
   unsigned char *now_bit_ptr;
   unsigned char *drv_bit_ptr;
   short tmp;
   short ii,j;

   drv_bit_ptr = (unsigned char *) &veh_ptr->driver_attr;

   veh_bit_ptr = (unsigned char *) &veh_ptr->vehicle_attr;
   for (ii = 0; ii < 4; ii++)
   {
      tmp = 0x80;
      for (j=0; j<=7; j++)
	{
	  if (direction == STATE_SET)
	    {
	      if (*veh_bit_ptr & tmp)
		++fleet[veh_ptr->fleet_nbr]->now_veh_attr[j + (ii*8)];
	    }
	  else
	    {
	      if (*veh_bit_ptr & tmp)
		{
		  if ((veh_ptr->fleet_nbr < 0) || (veh_ptr->fleet_nbr > 7))
		    return(SUCCESS);
		  if (fleet[veh_ptr->fleet_nbr]->now_veh_attr[j + (ii*8)] > 0)
		    --fleet[veh_ptr->fleet_nbr]->now_veh_attr[j + (ii*8)];
		}
	    }
	  tmp >>= 1;
	}
      ++veh_bit_ptr;
    }



   return (SUCCESS);
}				/* end veh_bit_update */

RET_STATUS
veh_suspend(veh_ptr, call_ptr)		/* WAS part of no_accept */
   struct veh_driv *veh_ptr;
   CALL *call_ptr;
 /* TO DO make part of Veh_suspend */
{
   VEH_REC *vh_ptr;
   FLEET_HNDL  fleet_hndl;
   char susp_msg[1024];
   int modify_passenger_susp = FALSE, modify_delivery_susp = FALSE, modify_typex_susp = FALSE;

   if ( ( call_ptr == NULL ) || ( veh_ptr == NULL ) )
     return (FAIL);

   if (Fleet_get_hndl(BY_ID, veh_ptr->fleet, &fleet_hndl) == FAIL)
     {
       sprintf(error_str, "Bad fleet id='%c'. Vehicle=%d", veh_ptr->fleet, veh_ptr->veh_nbr);
       ERROR(veh_ptr->fleet, "",error_str);
     }   

   if ( fleet[ veh_ptr->fleet_nbr ]->no_accept_no_post_period == 0 )
     return( SUCCESS );
   
   if ( Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_PASS_LIST),
				 veh_ptr->veh_nbr) == HNDL_NULL )
     {
       Fleet_susp_veh_list_add( (FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_PASS_LIST),
			       (VEH_HNDL)veh_ptr );
       modify_passenger_susp = TRUE;
       veh_ptr->vehicle_attr.susp_pass_call = 1;
       veh_ptr->veh_susp_from_date = mads_time;
       veh_ptr->veh_susp_to_date = fleet[veh_ptr->fleet_nbr]->no_accept_no_post_period + mads_time;
     }
   else if ( veh_ptr->veh_susp_to_date < ( fleet[veh_ptr->fleet_nbr]->no_accept_no_post_period + mads_time ) )
     {
       veh_ptr->vehicle_attr.susp_pass_call = 1;
       veh_ptr->veh_susp_from_date = mads_time;
       veh_ptr->veh_susp_to_date = fleet[veh_ptr->fleet_nbr]->no_accept_no_post_period + mads_time;
       modify_passenger_susp = TRUE;
     }   

   if ( Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_DELIV_LIST),
				 veh_ptr->veh_nbr) == HNDL_NULL )
     {
       Fleet_susp_veh_list_add( (FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_DELIV_LIST),
			       (VEH_HNDL)veh_ptr );
       veh_ptr->vehicle_attr.susp_del_call = 1;       
       veh_ptr->veh_susp_d_from_date = mads_time;
       veh_ptr->veh_susp_d_to_date = fleet[veh_ptr->fleet_nbr]->no_accept_no_post_period + mads_time;       
       modify_delivery_susp = TRUE;
     }
   else if ( veh_ptr->veh_susp_d_to_date < ( fleet[veh_ptr->fleet_nbr]->no_accept_no_post_period + mads_time ) )
     {
       veh_ptr->vehicle_attr.susp_del_call = 1;
       veh_ptr->veh_susp_d_from_date = mads_time;
       veh_ptr->veh_susp_d_to_date = fleet[veh_ptr->fleet_nbr]->no_accept_no_post_period + mads_time;
       modify_delivery_susp = TRUE;
     }   
   
   if ( Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_TYPEX_LIST),
				 veh_ptr->veh_nbr) == HNDL_NULL )
     {
       Fleet_susp_veh_list_add( (FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_TYPEX_LIST),
			       (VEH_HNDL)veh_ptr );
       veh_ptr->vehicle_attr.susp_typex_call = 1;       
       veh_ptr->veh_susp_x_from_date = mads_time;
       veh_ptr->veh_susp_x_to_date = fleet[veh_ptr->fleet_nbr]->no_accept_no_post_period + mads_time;       
       modify_typex_susp = TRUE;
     }
   else if ( veh_ptr->veh_susp_x_to_date < ( fleet[veh_ptr->fleet_nbr]->no_accept_no_post_period + mads_time ) )
     {
       veh_ptr->vehicle_attr.susp_typex_call = 1;
       veh_ptr->veh_susp_x_from_date = mads_time;
       veh_ptr->veh_susp_x_to_date = fleet[veh_ptr->fleet_nbr]->no_accept_no_post_period + mads_time;
       modify_typex_susp = TRUE;
     }
   
   

   vh_ptr = (VEH_REC *) req_buf;
   vh_ptr->fleet = veh_ptr->fleet;
   vh_ptr->nbr = veh_ptr->veh_nbr;

   if ((db(VEHICLE_FILE_ID, READ_KEYS, &veh_key2, ISEQUAL + ISLOCK, 0)) < 0)
   {
      ERROR(veh_ptr->fleet, VEHICLE_FILE, "no_accept - can't read record ");
      return (FAIL);
   }

   /* PASSENGER */
   if ( modify_passenger_susp == TRUE )
     {
       vh_ptr->susp_from_dt_tm = veh_ptr->veh_susp_from_date;
       vh_ptr->susp_to_dt_tm = veh_ptr->veh_susp_to_date;
       get_srch_date(veh_ptr->veh_susp_from_date, vh_ptr->susp_from_date);
       get_srch_time(veh_ptr->veh_susp_from_date, vh_ptr->susp_from_time);
       get_srch_date(veh_ptr->veh_susp_to_date, vh_ptr->susp_to_date);
       get_srch_time(veh_ptr->veh_susp_to_date, vh_ptr->susp_to_time);
     }
   
   /** DELIV **/
   if ( modify_delivery_susp == TRUE )
     {
       vh_ptr->susp_d_from_dt_tm = veh_ptr->veh_susp_d_from_date;
       vh_ptr->susp_d_to_dt_tm = veh_ptr->veh_susp_d_to_date;
       get_srch_date(veh_ptr->veh_susp_d_from_date, vh_ptr->susp_d_from_date);
       get_srch_time(veh_ptr->veh_susp_d_from_date, vh_ptr->susp_d_from_time);
       get_srch_date(veh_ptr->veh_susp_d_to_date, vh_ptr->susp_d_to_date);
       get_srch_time(veh_ptr->veh_susp_d_to_date, vh_ptr->susp_d_to_time);
     }
   
   /** TYPEX **/
   if ( modify_typex_susp == TRUE )
     {
       vh_ptr->susp_x_from_dt_tm = veh_ptr->veh_susp_x_from_date;
       vh_ptr->susp_x_to_dt_tm = veh_ptr->veh_susp_x_to_date;
       get_srch_date(veh_ptr->veh_susp_x_from_date, vh_ptr->susp_x_from_date);
       get_srch_time(veh_ptr->veh_susp_x_from_date, vh_ptr->susp_x_from_time);
       get_srch_date(veh_ptr->veh_susp_x_to_date, vh_ptr->susp_x_to_date);
       get_srch_time(veh_ptr->veh_susp_x_to_date, vh_ptr->susp_x_to_time);
     }
       
   if (fleet[veh_ptr->fleet_nbr]->no_accept_no_post_period)
   {
     if ( modify_passenger_susp == TRUE )
       vh_ptr->susp_pass_call = YES;
     if ( modify_delivery_susp == TRUE )
       vh_ptr->susp_del_call = YES;
     if ( modify_typex_susp == TRUE )
       vh_ptr->susp_typex_call = YES;
   }

   sprintf(trace_str, "%s%d", MDT_MSG_NOACT_TRIPNO, call_ptr->call_number);
   bzero(vh_ptr->susp_reason, (sizeof vh_ptr->susp_reason) - 1);
   strcpy(vh_ptr->susp_reason, trace_str);
   bzero(vh_ptr->susp_message, (sizeof vh_ptr->susp_message) - 1);
   strcpy(vh_ptr->susp_message, trace_str);

   if ((db(VEHICLE_FILE_ID, UPDATE_REC, &veh_key2, 0, 0)) < 0)
   {
      ERROR(veh_ptr->fleet, VEHICLE_FILE, "no_accept - can't update record ");
      return (FAIL);
   }
   if ((db(VEHICLE_FILE_ID, RELEASE, &veh_key2, 0, 0)) < 0)
   {
      ERROR(veh_ptr->fleet, VEHICLE_FILE, "no_accept - can't release record ");
      return (FAIL);
   }
   if ( veh_ptr->t_status.inuse )
     {
#ifdef SCANDINAVIA
       if ( (short)Veh_get_value((VEH_HNDL)veh_ptr, VEH_TERM_TYPE) == MDT_OPUS_2K )
         send_text_predefined( veh_ptr->mid, OPUS2K_SUSPENSION, veh_ptr );
       mk_outb_text("");
       strcpy( susp_msg, MDT_MSG_VEH_SUSP);
       add_outb_text( susp_msg );
       strcpy(susp_msg, MDT_MSG_PASS_TRIP);
       strcat(susp_msg, vh_ptr->susp_to_date );
       strcat(susp_msg, " " );
       strcat(susp_msg, vh_ptr->susp_to_time );
       add_outb_text( susp_msg );
       add_outb_text(CR);
       //       send_msg_mmp( veh_ptr->mid, TEXT_DISPLAY, veh_ptr );
       strcpy( susp_msg, MDT_MSG_VEH_SUSP);
       add_outb_text( susp_msg );
       strcpy(susp_msg, MDT_MSG_DELIV_TRIP);
       strcat(susp_msg, vh_ptr->susp_d_to_date );
       strcat(susp_msg, " " );
       strcat(susp_msg, vh_ptr->susp_d_to_time );
       add_outb_text( susp_msg );
       add_outb_text(CR);
       //       send_msg_mmp( veh_ptr->mid, TEXT_DISPLAY, veh_ptr );
       strcpy( susp_msg, MDT_MSG_VEH_SUSP);
       add_outb_text( susp_msg );
       strcpy(susp_msg, MDT_MSG_TYPEX_TRIP);
       strcat(susp_msg, vh_ptr->susp_x_to_date );
       strcat(susp_msg, " " );
       strcat(susp_msg, vh_ptr->susp_x_to_time );
       add_outb_text( susp_msg );
       add_outb_text(CR);
       send_msg_mmp( veh_ptr->mid, TEXT_DISPLAY, veh_ptr );
#endif
     }
}				/* end veh_suspend */

RET_STATUS
veh_call_close(veh_ptr)
   struct veh_driv *veh_ptr;
 /* common to reset of VEH_PICKUP_STATE & VEH_FLAG_STATE */
{
   /* WHY DONT WE RESET ON CALL STATE UNCONDITIONALLY ?? */
   (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_CALL_STATE, VEH_NO_POST_CHECK, HNDL_NULL);
   if (veh_ptr->t_status.e_posted)
   {
      veh_ptr->zone_num = veh_ptr->epost_zone;	/* SMS P.1911 */
      if (Veh_reset_state((VEH_HNDL) veh_ptr, VEH_EPOSTED_STATE, (HNDL) veh_ptr->zone_num, HNDL_NULL) == FAIL)
      {
/*	 (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_CALL_STATE, HNDL_NULL, HNDL_NULL); */
	 return (SUCCESS);
      }

   }
   veh_ptr->c_post_time = 0;
   veh_ptr->t_status.flag = 0;
   veh_ptr->t_status.local = 0;

   return (SUCCESS);
}				/* end veh_call_close */

static void
veh_tell_ui_about_emerg()
{
   struct users *ui_user;
   int   ii;

   ui_user = offset->user;

   for (ii = 0; ii < USER_MAX; ii++, ui_user++)
   {
      if (ui_user->p_id != 0 &&
	  ui_user->menu_perms.exceptions)
	 kill(ui_user->p_id, SIGUSR2);
   }

}				/* end veh_tell_ui_about_emerg */

static RET_STATUS
veh_check_state(veh_ptr, state, op)
   struct veh_driv *veh_ptr;
   int   state;
   int   op;
{
    char error_str[80];
#ifdef DEBUG_OO
   if (!veh_state_names_switch)
   {
      init_veh_state_names();
      veh_state_names_switch = TRUE;
   }
   if (debug_state)
   {
      if ((op ==STATE_SET) || (op == STATE_RESET))
	 printf("Veh %d %s %s\n", veh_ptr->veh_nbr,
		(op == STATE_RESET) ? "STATE_RESET from" : "STATE_SET to",
		veh_state_name[state - STATE_BASE]);
   }
#endif

   switch (state)
   {
   case VEH_ACCEPT_STATE:
      if (veh_ptr->t_status.emer == 1)
	 return (FAIL);
      break;
   case VEH_CALL_OFFER_STATE:
      if (veh_ptr->t_status.emer == 1)
	 return (FAIL);
      break;
   case VEH_EMERGENCY_STATE:
      break;
   case VEH_EPOSTED_STATE:
      if (veh_ptr->t_status.emer == 1)
	 return (FAIL);
      break;
   case VEH_FLAG_REQUEST_STATE:
      if (veh_ptr->t_status.emer == 1)
	 return (FAIL);
      break;
   case VEH_FLAG_STATE:
      if (veh_ptr->t_status.emer == 1)
	 return (FAIL);
      break;
   case VEH_IDLE_STATE:
      if (veh_ptr->t_status.emer == 1)
	 return (FAIL);
   case VEH_IN_USE_STATE:
      break;
   case VEH_LATE_METER_STATE:
      if (veh_ptr->t_status.emer == 1)
	 return (FAIL);
      break;
   case VEH_LOCAL_STATE:
      if (veh_ptr->t_status.emer == 1)
	 return (FAIL);
      break;
   case VEH_MDT_WORKING_STATE:
      break;
   case VEH_METER_ON_STATE:
      if (veh_ptr->t_status.emer == 1)
	 return (FAIL);
      break;
   case VEH_MSG_STATE:
      if (veh_ptr->t_status.emer == 1)
	 return (FAIL);
      break;
   case VEH_NOSHOW_STATE:
      if (veh_ptr->t_status.emer == 1)
	 return (FAIL);
      break;
   case VEH_OK_TO_MATCH_STATE:
      if (op != GET)
	 return (FAIL);
      break;
   case VEH_ON_CALL_STATE:
      if (veh_ptr->t_status.emer == 1)
	 return (FAIL);
      if (op == STATE_SET)
	 return (FAIL);
      if (veh_ptr->t_status.offered || veh_ptr->t_status.accepted ||
	  veh_ptr->t_status.pckup || veh_ptr->t_status.flag)
	 return (SUCCESS);
      if (veh_ptr->driver_id == 0)	/* for sign_out */
	 return (SUCCESS);
      return (FAIL);
   case VEH_OK_TO_PICKUP_STATE:
      if (op != GET)
	 return (FAIL);
      break;
   case VEH_ON_BREAK_STATE:
      if (veh_ptr->t_status.emer == 1)
	 return (FAIL);
      break;
   case VEH_PICKUP_STATE:
      if (veh_ptr->t_status.emer == 1)
	 return (FAIL);
      if (op == STATE_SET)
	 if (veh_ptr->t_status.pckup == 1)
	    return (FAIL);
      break;
   case VEH_POSTED_STATE:
      if (veh_ptr->t_status.emer == 1)
	 return (FAIL);
      if (op == STATE_RESET)
      {
	 if (veh_ptr->t_status.posted == 0)
	 {
#ifdef FOO
            sprintf(error_str,"Attempt to unpost from %d by veh #%d when not posted",
                    veh_ptr->zone_num, veh_ptr->veh_nbr);
	    ERROR(' ', "", error_str);
#endif
            return(SUCCESS); /* still may be in zone */
	 }
	 return (SUCCESS);
      }
      if (op == STATE_SET)
	 if (veh_ptr->t_status.idle == 0)
	 {
#ifdef FOO
	    ERROR(' ', "", "ATTEMPT TO POST WHEN not idle");
#endif
	 }
      break;
   case VEH_SHORT_METER_STATE:
      break;
   case VEH_SIGN_IN_STATE:
      if (veh_ptr->t_status.emer == 1)
	 return (FAIL);
      if ((op == STATE_RESET) && (veh_ptr->t_status.inuse == 0))
	 return (FAIL);		/* already signed out */
      if ((op == STATE_SET) && veh_ptr->t_status.inuse)
	 return (FAIL);		/* already signed in */
      break;
   case VEH_SUSPENDED_STATE:
      if (veh_ptr->t_status.emer == 1)
	 return (FAIL);
      break;
   case VEH_TALK_STATE:
      if (veh_ptr->t_status.emer == 1)
	 return (FAIL);
      break;
    case VEH_EMERGENCY_RESOLVED_STATE:
      break;
   default:
      return (FAIL);
   }
   return (SUCCESS);
}				/* end veh_check_state */

int
veh_state(veh_ptr, op, state, argument, argument2)
   struct veh_driv *veh_ptr;
   int   op;
   int   state;
   HNDL  argument;
   HNDL  argument2;
{
  int   iMoffDelay, j;
  short tmp = 0, call_zone = 0;
   short zone_nbr, break_time = 0;
   short newer_zone_nbr = 0;
   FLEET_HNDL fleet_hndl;
   DRIVER_HNDL driver_hndl;
   HTTP_HNDL  http_hndl;
   ZONE_HNDL zone_hndl;
   CALL_HNDL tmp_call_hndl;
   ZONE_VEH_LIST_HNDL veh_list_hndl;
   struct zones *zone_ptr;	/* temp */
   char  str_buf[80];
   VEH_HNDL veh_hndl, tmp_veh_hndl;
   struct calls *call_ptr;
   char penalty, dr_type;
   char   resp_buf[128];
   VEH_REC  veh_rec;
   FILE    *fpEmergVehNum;
   BOOLEAN new_zone = FALSE, msg_to_send = FALSE, first_msg = TRUE, stc_break = FALSE;
  char            tmp_buf[ 512 ];
  char            DueTime[ 9 ];
   unsigned char mask;
   unsigned char *addr;
   int i,jj,lines_sent = 0;
   struct cisam_sh s_sched;
   struct cisam_ss s_shift;
   struct cisam_s_version sv;
   struct cisam_cl *cl_ptr;
   int mode = 0;
   struct tm *tm_ptr;
   int curTime = 0;
  char    *stime, *ctime();
  time_t tmtime;
  KELANODE_REC kelanode_rec;
  struct calls *tmp_call_ptr;
  short level_offered = -1;

#ifdef ASSERT
   if (assert_check_op(op))
   {
      ERROR(' ', "", "Illegal Operation");
      return (FAIL);
   }
#endif

   driver_hndl = (DRIVER_HNDL) Veh_get_value((VEH_HNDL) veh_ptr, VEH_DRIVER_HNDL);
   veh_hndl = (VEH_HNDL) veh_ptr;
#ifdef DEBUG
   if (offset->prattle >= LEVEL1)
   {
      if (op != GET)
      {
         if (op == STATE_RESET)
         {
             switch (state)
             {
                 case VEH_PICKUP_STATE:
                 case VEH_CALL_OFFER_STATE:
                      if (argument2 != NULL)
                         {
                            sprintf(trace_str, "V#%d st %d op %d c#%d arg %x c#%d\n",
                                    veh_ptr->veh_nbr, state, op,veh_ptr->call_nbr,  argument, ((CALL *)argument2)->call_number);
                            break;
                         }
                 default:
                      sprintf(trace_str, "V#%d st %d op %d c#%d arg %x arg %x\n",
                              veh_ptr->veh_nbr, state, op, veh_ptr->call_nbr, argument, argument2);
                      break;
             }
         }
         else
         {
             switch (state)
             {
                 case VEH_ACCEPT_STATE:
                 case VEH_CALL_OFFER_STATE:
                      if (argument != NULL)
                         {
                            sprintf(trace_str, "V#%d st %d op %d c#%d c#%d\n",
                                    veh_ptr->veh_nbr, state, op, veh_ptr->call_nbr, ((CALL *)argument)->call_number);
                            break;
                         }
                 default:
                      sprintf(trace_str, "V#%d st %d op %d c#%d arg %x\n",
                              veh_ptr->veh_nbr, state, op, veh_ptr->call_nbr, argument);
                      break;
             }
         }
         TRACE(trace_str);
      }
   }
#endif
   if (veh_check_state(veh_ptr, state, op) != SUCCESS)
      return (FAIL);

   switch (state)
   {
   case VEH_ACCEPT_STATE:
/***************************************************/
/* VEH_ACCEPT_STATE                                */
/* STATE_SET                                       */
/*   OP: Vehicle has accepted a call               */
/*   ARG:call_hndl                                 */
/*   PRE: veh exists                               */
/*        call exists                              */
/*   POST:                                         */
/*        if ARGUMENT == VEH_CALL_HNDL             */
/*            if VEH_POSTED_STATE                  */
/*                 VEH_POSTED_STATE reset          */
/*            if VEH_EPOSTED_STATE                 */
/*                 VEH_EPOSTED_STATE reset         */
/*            !VEH_IDLE_STATE                      */
/*            VEH_TIME_INFO set                    */
/*            Driver call stats incremented        */
/*            CALL_ACCEPTED_STATE set              */
/*            if VEH_MDT_WORKING_STATE             */
/*                 Call_send_assign called         */
/*        else                                     */
/*           if                                    */
/*              !VEH_MDT_WORKING_STATE             */
/*                  CALL_ACCEPT_STATE set          */
/*   ERROR: illegal state transition               */
/*                CALL_HNDL != VEH_CALL_HNDL &&    */
/* STATE_RESET                                     */
/*   OP:  reset the accepted state                 */
/*   ARG:  none                                    */
/*   PRE:  vehicle exists                          */
/*   POST:  !VEH_ACCEPTED_STATE                    */
/*   ERROR:  none                                  */
/***************************************************/

      switch (op)
      {
      case STATE_SET:
	 if (((struct calls *) argument == veh_ptr->call_ptr) && (veh_ptr->call_ptr != NULL))
	 {
	    if (POSTED(veh_ptr))
	       (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL);
	    else if (veh_ptr->t_status.e_posted)
	       (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_EPOSTED_STATE, HNDL_NULL, HNDL_NULL);
	    veh_ptr->t_status.idle = 0;
	    veh_ptr->t_status.accepted = 1;
	    veh_ptr->time_info = mads_time;

	    ++veh_ptr->calls;	/* DRIVER ?? */
	    if (Call_set_state((CALL_HNDL) argument, CALL_ACCEPT_STATE, NULL) == SUCCESS)
	    {
	       if (veh_ptr->t_status.kdt_working)
		 {
		   if (0)
		     {
		       if ( ( (short)Fleet_get_value( (FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_M_ACTION_TIMER ) > 0 ) &&
			    ( (CALL_MSG_LIST_HNDL)Call_get_value((CALL_HNDL)argument, CALL_MSG_LIST) != HNDL_NULL ) )
			 {
			   Call_set_value((CALL_HNDL)argument, CALL_M_ACTION_RETRY,
					  (HNDL)((short)Fleet_get_value((FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_M_ACTION_RETRY )));
			   Call_msg_send_m_action( (CALL_MSG_LIST_HNDL)Call_get_value((CALL_HNDL)argument, CALL_MSG_LIST),
						  (VEH_HNDL)veh_ptr );
			   if ( Call_add_timer((CALL_HNDL)argument, T_M_ACTION,
					  (short)Fleet_get_value( (FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_M_ACTION_TIMER ),
					  (int (*)())call_m_action_notify_func) != SUCCESS )
			     {
			       sprintf(error_str, "Could not add T_M_ACTION for call nbr %d",
				       (int)Call_get_value((CALL_HNDL)argument, CALL_NBR));
			       ERROR(' ',"", error_str);
			     }
			   /* increment M_ACTION retry counter */
			 }
		       else
			 Call_send_assign((CALL_HNDL)argument, (VEH_HNDL) veh_ptr, FALSE);
		     }
		   else
		     Call_send_assign((CALL_HNDL) argument, (VEH_HNDL) veh_ptr, FALSE);
		 }
	       
	       /* Update Vehicle's GPS state for TaxiGEO */
	       writer_gps_state( (VEH_HNDL) veh_ptr, VEH_GPS_STATE_ACCEPT );

	       Fleet_big_car_trip_list_remove(
                                              (FLEET_BIG_CAR_TRIP_LIST_HNDL)fleet[veh_ptr->fleet_nbr]->big_car_trip_list_hndl,
                                              (CALL_HNDL)argument
                                              );
               
	       return (SUCCESS);
	    }
	 } else
	 {
	    if ((!veh_ptr->t_status.kdt_working)
		&& (veh_ptr->call_ptr != NULL))
	       return (Call_set_state((CALL_HNDL) argument, CALL_ACCEPT_STATE, NULL));
	    else
	       ERROR(' ', "", "VEH_ACCEPT_STATE set call mismatch");
	 }
	 return (FAIL);

      case STATE_RESET:
	 veh_ptr->t_status.accepted = 0;
	 return (SUCCESS);

      default:
	 return ((int) veh_ptr->t_status.accepted ? SUCCESS : FAIL);
      }				/* end case */
      break;

   case VEH_CALL_OFFER_STATE:
/************************************************************/
/* VEH_CALL_OFFER_STATE                                     */
/* STATE_SET                                                */
/*   OP:  call offer sent to vehicle                        */
/*   ARG:  call_hndl                                        */
/*   PRE:  CALL_OFFER_STATE set                             */
/*         call_hndl exists                                 */
/*   POST: state  set                                       */
/*         VEH_CALL_NBR set to call  nbr                    */
/*         VEH_CALL_HNDL set to  argument                   */
/*   ERROR: illegal state transition                        */
/* STATE_RESET                                              */
/*   OP:  no accept or reject processing                    */
/*   ARG1: type - one of:                                   */
/*                NO_ACCEPT_PENALTY                         */
/*                NO_ACCEPT_NO_PENALTY                      */
/*                REJECT_PENALTY                            */
/*   ARGT: call_hndl                                        */
/*   PRE: veh exists                                        */
/*        VEH_CALL_HNDL  exists                             */
/*        driver  exists                                    */
/*        call  in  OFFER  state                            */
/*   POST:                                                  */
/*      switch on argument1                                 */
/*      NO_ACCEPT_PENALTY:                                  */
/*      NO_ACCEPT_NO_PENALTY:                               */
/*         driver no_accepts incremented                    */
/*         fleet no_accepts incremented                     */
/*         if ARGUMENT == NO_ACCEPT_NO_PENALTY              */
/*            !VEH_OFFERED_STATE                            */
/*            VEH_CALL_HNDL set to NULL                     */
/*            VEH_CALL_NBR set to 0                         */
/*         if ARGUMENT == NO_ACCEPT_PENALTY                 */
/*            switch on FLEET_NO_ACCEPT_PENALTY             */
/*                  NONE:  VEH_CALL_HNDLset to NULL         */
/*                         VEH_CALL_NBR set to 0            */
/*                         !VEH_OFFERED_STATE               */
/*                         if VEH_ZONE_NBR                  */
/*                            Dispatcher_veh_match called   */
/*                  ALLOWED:                                */
/*                        VEH_ON_CALL_STATE reset           */
/*                        Zone_add_vehcalled                */
/*                        VEH_Q_PRIORITY = VEH_PRIORITY     */
/*                        VEH_POSTED_STATE                  */
/*                        Dispatcher_veh_match called       */
/*                  PENALIZE: VEH_ON_CALL_STATE reset       */
/*                        Veh_suspend called                */
/*      REJECT_PENALTY:                                     */
/*         fleet reject cnts incremented                    */
/*         driver reject cnts incremented                   */
/*            VEH_CALL_HNDL = HNDL_NULL                     */
/*            VEH_CALL_NBR = 0                              */
/*            !VEH_OFFERED_STATE                            */
/*            if REJECT_PENALTY == ALLOWED                  */
/*                Dispatcher_veh_match called               */
/*            else if REJECT_PENALTY == ALLOWED_PENALIZE    */
/*                    VEH_POSTED_STATE reset                */
/*                    VEH_Q_PRIORITY = VEH_PRIORITY         */
/*                    Zone_add_veh called                   */
/*    ERROR: illegal state transition                       */
/************************************************************/

      switch (op)
      {
      case STATE_SET:
	 veh_ptr->t_status.offered = 1;
	 veh_ptr->call_nbr = (int) Call_get_value((CALL_HNDL) argument, CALL_NBR);
	 veh_ptr->call_ptr = (struct calls *) argument;
	 return (SUCCESS);

      case STATE_RESET:
	 /********* CALL REJECT HANDLING AREA **************/
	tmp_call_ptr = veh_ptr->call_ptr;
	 if (((char) argument >= '0') && ((char) argument <= '4'))     
	 {
	    fleet[veh_ptr->fleet_nbr]->rejects += 1;
	    veh_ptr->rejects += 1;
	    veh_ptr->call_ptr = NULL;
	    veh_ptr->call_nbr = 0;
	    veh_ptr->t_status.offered = 0;

	    if ((char)argument == '3')
	      {
		call_ptr = (struct calls *) argument2;
		call_zone = call_ptr->pickup_zone;
		tmp = veh_ptr->zone_num;
		if ((tmp == call_zone))
		  {
		    (void)Veh_reset_state((VEH_HNDL)veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL);
		    veh_ptr->t_status.posted = 1;
		    Vehicle_change_q_priority((VEH_HNDL)veh_ptr, veh_ptr->priority);
		    Zone_add_veh(veh_ptr->zone_num, (VEH_HNDL)veh_ptr);
		  }
	      }

	    return (SUCCESS);
	  /******* END CALL REJECT HANDLING AREA *****************/
	 } else
	 {
	   /*****  NAK ON CALL OFFER / NO ACCEPT HANDLING AREA ****/
	   veh_ptr->call_ptr = NULL;
	   veh_ptr->call_nbr = 0;
	   veh_ptr->t_status.offered = 0;	   
	   
	    Zone_get_hndl(veh_ptr->zone_num, veh_ptr->fleet_nbr, &zone_hndl );
	    if ( zone_hndl != HNDL_NULL )
	      {
#ifdef FOO
		if ( (char)Zone_get_value(zone_hndl, ZONE_TYPE) != ZN_STAND )
		  {
		    // Book into zone 995
		    (short)argument = 995;
		    (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL);
		    (void) Veh_set_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, (HNDL) argument);
		  }
#endif
		if (tmp_call_ptr != NULL)
		  {
		    zone_ptr = fleet[veh_ptr->fleet_nbr]->zone_ptr[tmp_call_ptr->pickup_zone];
		    level_offered = -1;
		    for ( i = 0; i < LEVEL_MAX; i++ )
		      {
			for ( j = 0; j < BCK_UP_MAX; j++ )
			  {
			    if (zone_ptr->level_number[i].bck_up_zone[j] == veh_ptr->zone_num )
			      {
				level_offered = i;
				break;
			      }
			  }
			if (level_offered != -1)
			  break;
		      }
		    if (level_offered != -1)
		      {
#ifdef FOO
			// attempt to move vehicle to end of queue on penalty '3' setting
			if (zone_ptr->level_reject_penalty[level_offered] == '3')
			  {
			    (void)Veh_reset_state((VEH_HNDL)veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL);
			    if (veh_ptr->zone_num > 0)
			      {
				veh_ptr->t_status.posted = 1;
				Vehicle_change_q_priority((VEH_HNDL)veh_ptr, veh_ptr->priority);
				Zone_add_veh(veh_ptr->zone_num, (VEH_HNDL)veh_ptr);
			      }
			  }
#endif
		      }
		  }
	      }
	    
	    call_ptr = (struct calls *) argument2;

	    if (argument == CALL_NO_ACCEPT_PENALTY)
	    {
	       fleet[veh_ptr->fleet_nbr]->no_accepts += 1;
	       veh_ptr->no_accepts += 1;
	    }

	    // 11-25-15: unbook vehicle and book into zone 995
	    // if vehicle booked into same zone as call
	    // and call zone is a non-stand zone
	    // If booked into a stand zone and trip is offered from same
	    // stand zone, move taxi to end of queue
	    tmp = veh_ptr->zone_num;
	    call_zone = call_ptr->pickup_zone;
	    if ((tmp == call_zone) &&
		((char)Zone_get_value(zone_hndl, ZONE_TYPE) != ZN_STAND))
	      {
		(short)argument = 995;
		(void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL);
		(void) Veh_set_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, (HNDL) argument);
	      }
	    else if ((tmp == call_zone) &&
		     ((char)Zone_get_value(zone_hndl, ZONE_TYPE) == ZN_STAND))
	      {
		(void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_CALL_STATE, HNDL_NULL, HNDL_NULL);
		(void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL);
		veh_ptr->t_status.posted = 1;
		Vehicle_change_q_priority((VEH_HNDL)veh_ptr, veh_ptr->priority);
		(void) Zone_add_veh(tmp, (VEH_HNDL) veh_ptr);
		//(void) Dispatcher_veh_match((VEH_HNDL) veh_ptr, tmp);
	      }
	    /* veh_ptr->t_status.offered = 0; */
	    /* veh_ptr->call_ptr = NULL; */

#ifdef FOO
	    if ((int) argument)
	    {

	       if (argument == CALL_NO_ACCEPT_PENALTY)
		 penalty = fleet[veh_ptr->fleet_nbr]->no_accept_penalty;

	       /* NAK on call offer */
	       else
		 penalty = fleet[veh_ptr->fleet_nbr]->nak_offer_penal;

	       switch (penalty)
	       {
	       case '1':
		  /* NONE */
		  veh_ptr->call_ptr = NULL;
		  veh_ptr->call_nbr = 0;
		  veh_ptr->t_status.offered = 0;
		  if (veh_ptr->zone_num)
		     (void) Dispatcher_veh_match((VEH_HNDL) veh_ptr, veh_ptr->zone_num);
		  break;

	       case '2':
		  /* ALLOWED */
		  tmp = veh_ptr->zone_num;
		  (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_CALL_STATE, HNDL_NULL, HNDL_NULL);

		  if ( ( tmp > 0 ) )// && ( argument == CALL_NO_ACCEPT_PENALTY ) )
		    {
		      veh_ptr->t_status.posted = 1;
		      Vehicle_change_q_priority((VEH_HNDL)veh_ptr, veh_ptr->priority);
		      (void) Zone_add_veh(tmp, (VEH_HNDL) veh_ptr);
		      (void) Dispatcher_veh_match((VEH_HNDL) veh_ptr, tmp);
		    }
		  break;

	       case '3':
		  /* PENALIZE */
		  (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_CALL_STATE, HNDL_NULL, HNDL_NULL);
		  /* Only suspend for no accept, not on NAK */
		  if (argument == CALL_NO_ACCEPT_PENALTY)
		  {
                        //Need to do some additional 'magic' here so this car does not get automatically booked into
                        //zone by the GPS processor. We are creating a special state here IDLE/BREAK
                    if (0)
                      veh_ptr->t_status.on_break = 1;

		    if (veh_suspend(veh_ptr, call_ptr) == FAIL)
                      {

                        return (FAIL);
                      }
		  }
		  break;

		case '4':
		  /* Conditionally penalize  */
	          veh_ptr->call_ptr = NULL;
		  veh_ptr->call_nbr = 0;
	          veh_ptr->t_status.offered = 0;
		  break;
		 
	       }
	    } else
	    {
	       veh_ptr->call_ptr = NULL;
	       veh_ptr->call_nbr = 0;
	       veh_ptr->t_status.offered = 0;
               if (veh_ptr->zone_num)
                  (void) Dispatcher_veh_match((VEH_HNDL) veh_ptr, veh_ptr->zone_num);
	    }
#endif

	    return (SUCCESS);
	 }

      default:
	 return ((int) veh_ptr->t_status.offered ? SUCCESS : FAIL);
      }				/* end case */
      break;

    case VEH_EMERGENCY_RESOLVED_STATE:
      switch (op)
	{
	case STATE_SET:
	  veh_ptr->t_status.emer_clear = 1;
	  break;

	case STATE_RESET:
	  veh_ptr->t_status.emer_clear = 0;
	  break;

	default:
	  return ((int)veh_ptr->t_status.emer_clear ? SUCCESS : FAIL );
	}
      break;
      
   case VEH_EMERGENCY_STATE:
/*************************************************/
/* VEH_EMERGENCY_STATE                           */
/* STATE_SET                                     */
/*   OP: vehicle has an emergency                */
/*   ARG: none                                   */
/*   PRE: vehicle exists                         */
/*   POST: fleet emergency stats incremented     */
/*      vehicle history updated                  */
/*      if VEH_EPOSTED_STATE                     */
/* 	   remove the epost timer                */
/*      vehicle emergency on time set            */
/*      system emergency procedure invoked       */
/*      EMERGENCY request sent to UI             */
/*      state set                                */
/*   ERROR: already in emergency state           */
/* STATE_RESET                                   */
/*   OP: Emergency acknowledged                  */
/*   ARG: none                                   */
/*   PRE: in state - vehicle exists              */
/*   POST: fleet statistics decremented          */
/*      vehicle emergency off time set           */
/*      VEH_SIGN_IN state reset                  */
/*   ERROR: none                                 */
/*************************************************/

      switch (op)
      {
      case STATE_SET:
	 veh_ptr->t_status.emer = 1;
	 Fleet_increment_stats((FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_NBR_EMERGENCIES, 0);
	 Fleet_increment_stats((FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_NBR_NOW_EMERGENCIES, 0);
	 veh_ptr->emergency_on_date = mads_time;
	 veh_ptr->emergency_off_date = 0;
	 if (veh_ptr->call_ptr != NULL)
	    mk_except(veh_ptr->call_ptr, veh_ptr, EMERGENCY, 0, -1);
	 else
	    mk_except((struct calls *) NULL, veh_ptr, EMERGENCY, 0, -1);

	 writer_emer(veh_ptr->fleet, veh_ptr->veh_nbr, veh_ptr->driver_id, veh_ptr->call_nbr,
		     mads_time, veh_ptr->zone_num);

	 if (veh_ptr->t_status.e_posted)
	 {
	    if (veh_ptr->veh_epost_timer != NULL)
	    {
	       Timer_destroy((TIMER_HNDL) veh_ptr->veh_epost_timer);
	       veh_ptr->veh_epost_timer = NULL;
	    }
	 }


	 if ( ( fpEmergVehNum = fopen( EMERG_VEH_FILE, "w" )) == NULL )
	   {
	     ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id,"", "Unable to open Emergency Vehicle File");
	   }
	 else
	   {
	     fprintf(fpEmergVehNum, "%d\t%c", veh_ptr->veh_nbr, veh_ptr->fleet);
	     fclose( fpEmergVehNum );
	   }

	 veh_tell_ui_about_emerg();
	 
	 /** send message to GPS display system **/
	 if ( (char)Fleet_get_value( (FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_GPS_STATE ) == YES )
           {
             GPS_send_display_system( (VEH_HNDL) veh_hndl, MAP_APP_RQST_TYPE_EMERG );
           }
	 
	 return (SUCCESS);

      case STATE_RESET:
	Fleet_decrement_stats((FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_NBR_NOW_EMERGENCIES, 0);
	 veh_ptr->emergency_off_date = mads_time;
	 veh_ptr->t_status.emer = 0;
	 (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_SIGN_IN_STATE, HNDL_NULL, HNDL_NULL);
	 return (SUCCESS);

      default:
	 return ((int) veh_ptr->t_status.emer ? SUCCESS : FAIL);
      }				/* end case */
      break;

   case VEH_EPOSTED_STATE:	/* WAS d_epost - e_post_post plus bits */
/*********************************************************/
/* VEH_EPOSTED_STATE                                     */
/* STATE_SET                                             */
/*   OP: epost the vehicle                               */
/*   ARG: epost zone nbr                                 */
/*   PRE: epost allowed                                  */
/*       vehicle exists                                  */
/*       zone exists                                     */
/*   POST:                                               */
/*       if VEH_EPOSTED_SSTATE                           */
/*           delete EPOST timer                          */
/*           Zone_remove_veh called                      */
/*       else                                            */
/*          if VEH_POSTED_STATE                          */
/*             VEH_POSTED_STATE reset                    */
/*       new EPOST timer started                         */
/*       Zone_add_veh(epost zone) called                 */
/*       Zone_check_epost_calls called                   */
/*       q position  message sent to vehicle             */
/*   ERROR: illegal state transition                     */
/*          unable to set timer                          */
/* STATE_RESET                                           */
/*   OP: epost termination has occurred                  */
/*   ARG: post zone or 0                                 */
/*   PRE: vehicle exists                                 */
/*        VEH_EPOSTED_STATE                              */
/*        Driver exists                                  */
/*  POST: VEH_IDLE_STATE set                             */
/*     if ARGUMENT (post zone)                           */
/*        if epost expired                               */
/*           send LATE_EPOST msg to driver               */
/*           Zone_remove_veh called                      */
/*           Zone_check_epost_calls called               */
/*           return                                      */
/*        if (post_zone != VEH_ZONE_NBR)                 */
/*           Zone_remove_veh called                      */
/*           Zone_set_state(EPOSTED) called              */
/*           VEH_POSTED_STATE set                        */
/*        if (post_zone == VEH_ZONE_NBR)                 */
/*           Zone_reset_state(EPOSTED) called            */
/*           Fleet_stats incremented and decremented     */
/*           Driver stats incremented                    */
/*           else                                        */
/*              Dispartcher_veh_match called             */
/*              if no match                              */
/*                q position msg sent to vehicle         */
/*             EPOST timer deleted                       */
/*      if ARGUMENT == 0                                 */
/*         if no EPOST timer                             */
/*            Zone_remove_veh called                     */
/*            Zone_check_epost_calls called              */
/*            VEH_ZONE_NBR = 0                           */
/*            LATE_EPOST msg sent to vehicle             */
/*   ERROR: none                                         */
/*********************************************************/

      /* remember to remove from zone if there */
      switch (op)
      {
      case STATE_SET:

	 if ((short) argument == 0)
	 {
	    ERROR(' ', "", "EPOST_SET with no zone");
	    return (FAIL);
	 }
	 /* If already e_posted, remove from timer list */
	 /* also free a epost_wait call                 */
	 if (veh_ptr->t_status.e_posted)
	 {
	    if (veh_ptr->veh_epost_timer != NULL)
	    {
	       Timer_destroy((TIMER_HNDL) veh_ptr->veh_epost_timer);
	       veh_ptr->veh_epost_timer = NULL;
	    }
	    if (Zone_remove_veh(veh_ptr->zone_num, (VEH_HNDL) veh_ptr) == FAIL)
	       ERROR(' ', "", "Zone_remove_veh failure");
	    Zone_get_hndl(veh_ptr->zone_num, veh_ptr->fleet_nbr, &zone_hndl );
	    tmp_call_hndl = (CALL_HNDL)Veh_get_value((VEH_HNDL)veh_ptr, VEH_HELD_EPOST_CALL);
	    Zone_release_epost( zone_hndl, (VEH_HNDL)veh_ptr );
	    if ( tmp_call_hndl != HNDL_NULL )
	      {
		Call_reset_state(tmp_call_hndl, CALL_WAIT_EPOST_STATE, NULL);
		Zone_check_epost_vehs( zone_hndl, tmp_call_hndl );
	      }
	 } else
	 {
	    if (veh_ptr->t_status.posted)
	       (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL);
	 }

	 if (veh_ptr->veh_epost_timer != NULL)
	   {
	     Timer_destroy((TIMER_HNDL) veh_ptr->veh_epost_timer);
	     veh_ptr->veh_epost_timer = NULL;
	   }
	 else
	   veh_ptr->veh_epost_timer = NULL;
	 
	 veh_ptr->t_status.e_posted = 1;
	 veh_ptr->epost_zone = (short) argument;
	 
	 Vehicle_change_q_priority((VEH_HNDL)veh_ptr, veh_ptr->priority);

	 Zone_add_veh((short) argument, (VEH_HNDL) veh_ptr);
	 if (Zone_get_hndl(veh_ptr->zone_num, veh_ptr->fleet_nbr, &zone_hndl) == FAIL)
	 {
	    sprintf(error_str, "bad zone nbr %d", veh_ptr->zone_num);
	    ERROR(' ', "", error_str);
	    return (FAIL);
	 }
	 Zone_check_epost_calls(zone_hndl, (VEH_HNDL)veh_ptr);
	 if ( (short)Zone_get_value( zone_hndl, ZONE_EPOST_LEAD_TIME ) <= 0 )
	   veh_ptr->veh_epost_timer =
	     (char *) Timer_create(
				   TIMER_TYPE, T_WAIT_EPOST,
				   TIMER_NOTIFY_FUNC, veh_epost_notify,	/* TEMPORARY */
				   TIMER_EXPIRE_TIME, fleet[veh_ptr->fleet_nbr]->e_post_lead_time,
				   TIMER_DATA, (HNDL) veh_ptr,
				   ATTRIBUTE_END);
	 else
	   veh_ptr->veh_epost_timer =
	     (char *) Timer_create(
				   TIMER_TYPE, T_WAIT_EPOST,
				   TIMER_NOTIFY_FUNC, veh_epost_notify,	/* TEMPORARY */
				   TIMER_EXPIRE_TIME, ((short)Zone_get_value(zone_hndl, ZONE_EPOST_LEAD_TIME)) * 60,
				   TIMER_DATA, (HNDL) veh_ptr,
				   ATTRIBUTE_END);

	 send_qp((short) argument, veh_ptr);
	 return (SUCCESS);

      case STATE_RESET:
	 if (Zone_get_hndl(veh_ptr->zone_num, veh_ptr->fleet_nbr, &zone_hndl) == FAIL)
	 {
	    sprintf(error_str, "bad zone nbr %d", veh_ptr->zone_num);
	    ERROR(' ', "", error_str);
	    return (FAIL);
	 }
	 /*
	  * Veh_set_state(veh_hndl, VEH_IDLE_STATE, HNDL_NULL); *//* KEEP
	  * ????
	  */
	 if ( veh_ptr->veh_epost_timer == NULL )
	   tmp = 1;
	 if ( (short)Zone_get_value( zone_hndl, ZONE_EPOST_LEAD_TIME ) <= 0 )
	   {
	     if ((fleet[veh_ptr->fleet_nbr]->e_post_lead_time < (mads_time - veh_ptr->e_post_time)))
	       tmp = 1;
	   }
	 else
	   {
	     if ( ((short)Zone_get_value( zone_hndl, ZONE_EPOST_LEAD_TIME ) * 60) < (mads_time - veh_ptr->e_post_time ) )
	       tmp = 1;
	   }
	 
	 if ((argument == HNDL_NULL) || tmp || ((short) argument != veh_ptr->zone_num))
	 {
	   zone_nbr = veh_ptr->epost_zone;
	    if (Zone_remove_veh(veh_ptr->epost_zone, (VEH_HNDL) veh_ptr) == FAIL)
            {
               sprintf(error_str,"Zone_remove_veh %d ezone %d zone %d arg %d",
                                  veh_ptr->veh_nbr, veh_ptr->epost_zone, veh_ptr->zone_num, (short) argument);
	       ERROR(' ', "", error_str);
            }
	    veh_ptr->t_status.e_posted = 0;

	    if (Zone_get_hndl(veh_ptr->zone_num, veh_ptr->fleet_nbr, &zone_hndl) == FAIL)
	    {
	       sprintf(error_str, "bad zone nbr %d", veh_ptr->zone_num);
	       ERROR(' ', "", error_str);
	       return (FAIL);
	    }

	    if (argument == HNDL_NULL)
	       veh_ptr->zone_num = 0;
	    else
	       /* this cant happen boys - remove */
	       Veh_set_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, argument);	/* really */
	    if (veh_ptr->veh_epost_timer != NULL)
	    {
	       Timer_destroy((TIMER_HNDL) veh_ptr->veh_epost_timer);
	       veh_ptr->veh_epost_timer = NULL;
	    }
	    if (tmp)
	    {
	       send_text_predefined(veh_ptr->mid, LATE_EPOST, veh_ptr);
	       Zone_release_epost( zone_hndl, (VEH_HNDL)veh_ptr );
	       return (FAIL);
	    }
	    return (SUCCESS);
	 }
	 if (Zone_get_hndl(veh_ptr->zone_num, veh_ptr->fleet_nbr, &zone_hndl) == FAIL)
	 {
	    sprintf(error_str, "bad zone nbr %d", veh_ptr->zone_num);
	    ERROR(' ', "", error_str);
	    return (FAIL);
	 }
         tmp_call_hndl = HNDL_NULL;
         tmp_call_hndl = (CALL_HNDL)Veh_get_value((VEH_HNDL)veh_ptr, VEH_HELD_EPOST_CALL);
	 Zone_release_epost(zone_hndl, (VEH_HNDL)veh_ptr);
	 /* below should be in zone - how to do */
	 ++fleet[veh_ptr->fleet_nbr]->now_posts;
	 Zone_increment_stats(zone_hndl, ZONE_NBR_VEH);
	 Zone_decrement_stats(zone_hndl, ZONE_NBR_ENROUTE);
	 --fleet[veh_ptr->fleet_nbr]->now_enroute_posts;

	 veh_ptr->t_status.e_posted = 0;
	 veh_ptr->t_status.posted = 1;

	 if (Zone_get_hndl((short) argument, veh_ptr->fleet_nbr, &zone_hndl) == FAIL)
	 {
	    sprintf(error_str, "bad zone nbr %d", (short) argument);
	    ERROR(' ', "", error_str);
	    return (FAIL);
	 }

	 if (veh_ptr->veh_epost_timer != NULL)
	 {
	    Timer_destroy((TIMER_HNDL) veh_ptr->veh_epost_timer);
	    veh_ptr->veh_epost_timer = NULL;
	 }

	 /* check GPS position against zone coordinates */
	 if ( ((char)Fleet_get_value( (FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_GPS_STATE ) == YES ) &&
	      ((char)Veh_get_value( (VEH_HNDL) veh_ptr, VEH_NAV_TYPE ) == STAR2000_MDT ) &&
	      (GPS_check_zone( zone_hndl, veh_ptr ) != SUCCESS ))
	   {
	     /**
	       GPS_check_zone will remove the vehicle from zone so don't do it here
	     **/
	     
	     if ( ( GPS_get_best_zone( (VEH_HNDL) veh_ptr, &argument ) != SUCCESS ) ||
		  ( (short)argument <= 0 ) )
	       {
		 send_text( veh_ptr->mid, catgets(DISPATCH_catd, 1, 40, "GPS SAYS: NOT IN ZONE"), veh_ptr, PRIORITY_3 );
		 if ( POSTED(veh_ptr) )
		   Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL );
		 return (FAIL);
		 
	       }

	     sprintf( error_str, "%s%s%s %d",
		      "VÄÄRÄ KV RUUTU", CR,
		      "OLET RUUDUSSA",
		      (short)argument );
	     send_text( veh_ptr->mid, error_str, veh_ptr, PRIORITY_3 );	     
	     new_zone = TRUE;
             
	     Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL );

             // Did this veh have a held call? If so, initiate call-->veh matching
             if ( tmp_call_hndl != HNDL_NULL )
               Dispatcher_call_match(tmp_call_hndl);

             // Now put the taxi into the proper zone for GPS localization
	     Veh_set_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, (HNDL) argument );
             
             return (SUCCESS);
	   }


	 if ( (Dispatcher_veh_match((VEH_HNDL) veh_ptr, (short) argument) == FAIL) &&
	      ( new_zone == FALSE ) )
	   send_qp((short) argument, veh_ptr);

	 return (SUCCESS);

      default:
	 return ((int) veh_ptr->t_status.e_posted ? SUCCESS : FAIL);
      }				/* end case */
      break;

   case VEH_FLAG_REQUEST_STATE:/* WAS d_flag & res-except */
/***********************************************************/
/* VEH_FLAG_REQUEST_STATE                                  */
/* STATE_SET                                               */
/*   OP: flag request processing                           */
/*   ARG: none                                             */
/*   PRE: tbd                                              */
/*   POST: VEH_FLAG_REQUEST made                           */
/*         fleet now_flags stats incremented               */
/*         CALL_FLAG_REQUEST_STATE set                     */
/*   ERROR: illegal state transition                       */
/* STATE_RESET                                             */
/*   OP: FLAG_REQUEST approved or disapproved              */
/*   ARG: R_APPROVE or R_DISAPPROVE                        */
/*   PRE: veh exists                                       */
/*   POST:                                                 */
/*        if VEH_CALL_HNDL                                 */
/*           CALL_FLAG_REQUEST_STATE reset with ARGUMENT   */
/*        switch on argument                               */
/*          R_APPROVED:                                    */
/*               driver flag stats incremented             */
/*               fleet flag stats modified                 */
/*               VEH_ON_CALL_STATE reset                   */
/*               OK_FLAG msg sent to vehicle               */
/*          R_DISAPPROVED:                                 */
/*               NO_FLAG msg sent to vehicle               */
/*               fleet flag stats decremented              */
/*   ERROR: illegal state transition                       */
/***********************************************************/

      switch (op)
	{
      case STATE_SET:
	  mk_except(veh_ptr->call_ptr, veh_ptr, FLAG_REQ, 0, -1);
	  ++fleet[veh_ptr->fleet_nbr]->now_flags;
	  Call_set_state((CALL_HNDL) veh_ptr->call_ptr, CALL_FLAG_REQUEST_STATE, HNDL_NULL);
	  veh_ptr->t_status.flag = 1;
	  return (SUCCESS);
	  
      case STATE_RESET:
	 if (veh_ptr->call_ptr)
	    Call_reset_state((CALL_HNDL) veh_ptr->call_ptr, CALL_FLAG_REQUEST_STATE, argument);
	 if ((short) argument == R_APPROVE)
	 {
	    (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_CALL_STATE, HNDL_NULL, HNDL_NULL);
	    ++veh_ptr->flags;
	    --fleet[veh_ptr->fleet_nbr]->now_flags;
	    ++fleet[veh_ptr->fleet_nbr]->flags;
	    send_text_predefined(veh_ptr->mid, OK_FLAG, veh_ptr);
	 } else
	 {
	    --fleet[veh_ptr->fleet_nbr]->now_flags;
	    send_text_predefined(veh_ptr->mid, NO_FLAG, veh_ptr);
	 }
	 return (SUCCESS);

      default:
	 return ((int) veh_ptr->t_status.flag ? SUCCESS : FAIL);
      }				/* end case */

   case VEH_FLAG_STATE:	/* WAS part of d_pickup */
/*********************************************************/
/* VEH_FLAG_STATE                                        */
/* STATE_SET                                             */
/*   OP: veh has picked up flag call                     */
/*   ARG: none                                           */
/*   PRE: vehicle exists                                 */
/* VEH_CALL_HNDL == HNDL_ERR                             */
/*   POST:                                               */
/*        if VEH_POSTED_STATE                            */
/*           VEH_POSTED_STATE reset                      */
/*        else if VEH_EPOSTED_STATE                      */
/*             VEH_EPOSTED_STATE reset                   */
/*        VEH_PICKUP_STATE                               */
/*        !VEH_IDLE_STATE                                */
/*        VEH_TIME_INFO set                              */
/*        VEH_ON_CALL_STATE set                          */
/*   ERROR: none                                         */
/* STATE_RESET                                           */
/*   OP: call closed called from unload key              */
/*   ARG: none                                           */
/*   PRE: veh exists                                     */
/*   POST:                                               */
/*        VEH_ON_CALL_STATE reset (VEH_NO_POST_CHECK)    */
/*        if VEH_METER_ON_STATE                          */
/*           VEH_METER_ON_STATE reset                    */
/*        if VEH_EPOSTED_STATE                           */
/*           VEH_ZONE_NBR = VEH_EPOST_NBR                */
/*           if VEH_EPOSTED_STATE reset == FAIL          */
/*              VEH_ON_CALL_STATE reset                  */
/*               return                                  */
/*   ERROR: none                                         */
/*********************************************************/

      switch (op)
      {
      case STATE_SET:
	if (!veh_ptr->t_status.on_break)
	  {
	    if (veh_ptr->t_status.posted)	
	      (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL);
	    else if (veh_ptr->t_status.e_posted)	/* MRB NEW */
	      (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_EPOSTED_STATE, HNDL_NULL, HNDL_NULL);
	    veh_ptr->t_status.pckup = 1;	/* mark the taxi as having a fare */
	  }
	veh_ptr->t_status.idle = 0;	/* mark status as busy */
	veh_ptr->time_info = mads_time;	/* update the accept time */
	++fleet[veh_ptr->fleet_nbr]->now_flags;
	++fleet[veh_ptr->fleet_nbr]->flags;
	++veh_ptr->flags;	/* DRIVER ?? */

	writer_gps_state( (VEH_HNDL) veh_ptr, VEH_GPS_STATE_FLAG );
	
	 return (SUCCESS);

      case STATE_RESET:
         --fleet[veh_ptr->fleet_nbr]->now_flags;
	 veh_call_close(veh_ptr);
	 if (veh_ptr->t_status.meter_on)
	    (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_METER_ON_STATE, HNDL_NULL, HNDL_NULL);
	 return (SUCCESS);

      default:
	 return ((int) veh_ptr->t_status.offered ? SUCCESS : FAIL);
      }				/* end case */
      break;

   case VEH_IDLE_STATE:	/* reset WAS idle_post set is RESET_VEH */
/****************************************/
/* VEH_IDLE_STATE                       */
/* STATE_SET                            */
/*   OP: vehicle idle                   */
/*   ARG: none                          */
/*   PRE: vehicle exists                */
/*   POST: !VEH_ACCEPT_STATE            */
/*         !VEH_PICKUP_STATE            */
/*         !VEH_OFFERED_STATE           */
/*         !VEH_ACCEPTED_STATE          */
/*         !VEH_METER_ON_STATE          */
/* ERROR: illegal state transition      */
/* STATE_RESET                          */
/*   OP: vheicle has posted             */
/*   ARG: none                          */
/*   PRE: VEH_IDLE_STATE                */
/*        vehicle exists                */
/*   POST: !VEH_IDLE_STATE              */
/*   ERROR: none                        */
/****************************************/

      /* REMOVE FROM ZONE ?? */
      switch (op)
      {
      case STATE_SET:
	 veh_ptr->t_status.idle = 1;
	 veh_ptr->t_status.pckup = 0;
	 veh_ptr->t_status.accepted = 0;
	 veh_ptr->t_status.offered = 0;
	 veh_ptr->t_status.meter_on = 0;

	 /* set vehicle's GPS state */
	 writer_gps_state( (VEH_HNDL) veh_hndl, VEH_GPS_STATE_IDLE );
	 
	 return (SUCCESS);

      case STATE_RESET:
	 veh_ptr->t_status.idle = 0;
	 return (SUCCESS);

      default:
	 return ((int) veh_ptr->t_status.idle ? SUCCESS : FAIL);
      }				/* end case */
      break;

   case VEH_IN_USE_STATE:
/************************************/
/* VEH_IN_USE_STATE                 */
/* STATE_SET                        */
/*   OP: vehicle inuse              */
/*   ARG: none                      */
/*   PRE: vehicle exists            */
/*   POST: state set                */
/* ERROR: none                      */
/* STATE_RESET                      */
/*   OP: driver has signed off      */
/*   ARG: none                      */
/*   PRE: vehicle exists            */
/*   POST: state reset              */
/*   ERROR: none                    */
/************************************/
      switch (op)
      {
      case STATE_SET:
	 veh_ptr->t_status.inuse = 1;
	 return (SUCCESS);

      case STATE_RESET:
	 veh_ptr->t_status.inuse = 0;
	 return (SUCCESS);

      default:
	 return ((int) veh_ptr->t_status.inuse ? SUCCESS : FAIL);
      }				/* end case */

   case VEH_LATE_METER_STATE:
/*************************************************/
/* VEH_LATE_METER_STATE                           */
/* STATE_SET                                      */
/*   OP: late meter has occurred                  */
/*   ARG: none                                    */
/*   PRE: veh exists                              */
/*   POST: driver late meter stats incremented    */
/*   ERROR: VEH_LATE_METER_STATE                  */
/* STATE_RESET                                    */
/*   OP: late meter condition rescinded           */
/*   ARG: none                                    */
/*   PRE: VEH_LATE_METER_STATE                    */
/*        veh_exists                              */
/*   POST: !VEH_LATE_METER_STATE                  */
/*   ERROR: !VEH_LATE_METER_STATE                 */
/**************************************************/
      switch (op)
      {
      case STATE_SET:
	 ++veh_ptr->late_meters;
	 veh_ptr->t_status.late_meter = 1;
	 return (SUCCESS);

      case STATE_RESET:
	 veh_ptr->t_status.late_meter = 0;
	 return (SUCCESS);

      default:
	 return ((int) veh_ptr->t_status.late_meter ? SUCCESS : FAIL);
      }				/* end case */
      break;

   case VEH_LOCAL_STATE:
/*************************************************/
/* VEH_LOCAL_STATE                                */
/* STATE_SET                                      */
/*   OP: veh on local call                        */
/*   ARG: none                                    */
/*   PRE: veh exists                              */
/*   POST: state set                              */
/*   ERROR: none                                  */
/* STATE_RESET                                    */
/*   OP: no longer on call                        */
/*   ARG: none                                    */
/*   PRE: veh_exists                              */
/*   POST: !VEH_LOCAL_STATE                       */
/*   ERROR: none                                  */
/**************************************************/
      switch (op)
      {
      case STATE_SET:
	 veh_ptr->t_status.local = 1;
	 return (SUCCESS);

      case STATE_RESET:
	 veh_ptr->t_status.local = 0;
	 return (SUCCESS);

      default:
	 return ((int) veh_ptr->t_status.local ? SUCCESS : FAIL);
      }				/* end case */
      break;

   case VEH_MDT_WORKING_STATE:
/*****************************************/
/* VEH_MDT_WORKING_STATE                 */
/* STATE_SET                             */
/*   OP: mdt terminal working            */
/*   ARG: none                           */
/*   PRE: veh exists                     */
/*   POST: VEH_MDT_WORKING_STATE         */
/*   ERROR: VEH_MDT_WORKING_STATE        */
/* STATE_RESET                           */
/*   OP: late meter condition recended   */
/*   ARG: none                           */
/*   PRE: VEH_MDT_WORKING_STATE          */
/*        veh_exists                     */
/*   POST: !VEH_MDT_WORKING_STATE        */
/*   ERROR: !VEH_MDT_WORKING_STATE       */
/*****************************************/
      switch (op)
      {
      case STATE_SET:
	 veh_ptr->t_status.kdt_working = 1;
	 return (SUCCESS);
      case STATE_RESET:
	 veh_ptr->t_status.kdt_working = 0;
	 return (SUCCESS);

      default:
	 return ((int) veh_ptr->t_status.kdt_working ? SUCCESS : FAIL);
      }				/* end case */
      break;

   case VEH_METER_ON_STATE:	/* WAS part of pickup */
/*******************************************************/
/* VEH_METER_ON_STATE                                  */
/* STATE_SET                                           */
/*   OP: meter is on                                   */
/*   ARG: none                                         */
/*   PRE: veh exists                                   */
/*   POST: state set                                   */
/*   ERROR: illegal state transition                   */
/* STATE_RESET                                         */
/*   OP: meter is off                                  */
/*   ARG: none                                         */
/*   PRE: veh exists                                   */
/*   POST: state reset                                 */
/*   ERROR: illegal state transition                   */
/******************************************************/
      switch (op)
      {
      case STATE_SET:
	 veh_ptr->t_status.meter_on = 1;
	 veh_ptr->time_info = mads_time;    /* for short meter calc */
	 veh_ptr->last_action_time = mads_time; /* activity indicator */	 

	 /* don't let driver do pickup from a break state and retain queue position */
	 if ( veh_ptr->t_status.on_break )
	   {
	     Veh_reset_state((VEH_HNDL)veh_ptr, VEH_ON_BREAK_STATE, HNDL_NULL, HNDL_NULL);
	     Veh_reset_state((VEH_HNDL)veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL);
	   }
	 
	 if (veh_ptr->call_ptr != NULL)
	   Call_delete_timer((CALL_HNDL)veh_ptr->call_ptr, T_LATE_METER);
	 return (SUCCESS);

      case STATE_RESET:

	if ((veh_ptr->call_ptr != NULL) &&
	    ((cl_ptr = get_call_record(veh_ptr->call_ptr->c_isam_num, veh_ptr->call_ptr->call_number)) != NULL)) {	
	  if ((!strncmp(cl_ptr->extended_type, "TH", 2)))
	    {
	      mk_outb_text("");
	      add_outb_text("%SZ490000%SZ1E000A%SZ2000C8%SZ210190"); //%SZ2000C8%SZ230190");
	      send_msg_mmp(veh_ptr->mid, TEXT_DISPLAY, veh_ptr);
	    }
	}

	 writer_gps_state( veh_hndl, VEH_GPS_STATE_IDLE );
	 
	 if (veh_ptr->t_status.crse_annul == 1)
	   {
	     veh_ptr->t_status.meter_on = 0;
	     veh_ptr->t_status.crse_annul = 0;
	     if ((short)Veh_get_value((VEH_HNDL)veh_ptr, VEH_CANX_TYPE) == CANX_TYPE_CANCEL_KEY)
	       iMoffDelay = (short)Fleet_get_value( (FLEET_HNDL)fleet[veh_ptr->fleet_nbr],
						   FLEET_CANX_TO_MOFF_TIME );
	     Veh_set_value((VEH_HNDL) veh_ptr, VEH_CANX_TYPE, (HNDL) 0);
	     if (((time_t)mads_time -
		  (time_t)Veh_get_value((VEH_HNDL)veh_ptr, VEH_CRSE_CANX_TIME)) < iMoffDelay)
	       {
		 Veh_set_state(veh_hndl, VEH_POSTED_STATE, (HNDL) veh_ptr->zone_num);
	       }
	     else
	       {
		 veh_ptr->priority = fleet[veh_ptr->fleet_nbr]->drv_reg_pri;
		 Veh_set_state(veh_hndl, VEH_IDLE_STATE, HNDL_NULL);
		 send_text_predefined(veh_ptr->mid, NOT_POST, veh_ptr);
	       }
	     Veh_set_value((VEH_HNDL) veh_ptr, VEH_CRSE_CANX_TIME, (HNDL) 0);
	   }
	 
	 if ((Veh_get_state(veh_hndl, VEH_PICKUP_STATE)))
	   {
	     Veh_reset_state((VEH_HNDL)veh_ptr, VEH_PICKUP_STATE, HNDL_NULL, HNDL_NULL);
	     veh_ptr->t_status.meter_on = 0;
	     if ((time_t) Veh_get_value((VEH_HNDL) veh_ptr, VEH_INFO_TIME) +
		 fleet[veh_ptr->fleet_nbr]->short_meter_duration > mads_time)
	       {
		 if (offset->status.ncp && Veh_get_state((VEH_HNDL) veh_ptr, VEH_MDT_WORKING_STATE) &&
		     !Veh_get_state((VEH_HNDL) veh_ptr, VEH_CALL_OFFER_STATE))
		   {
		     if ( veh_ptr->call_ptr != NULL )
		       {
			 if ( veh_ptr != veh_ptr->call_ptr->veh_ptr )
			   {
			     call_ptr = veh_ptr->call_ptr;
			     if ( call_ptr->veh_ptr != NULL )
			       sprintf( error_str, "ASSERT: METER_ON RESET_STATE error veh %d call %d call_veh %d",
				       veh_ptr->veh_nbr, call_ptr->call_number, call_ptr->veh_ptr->veh_nbr );
			     else
			       sprintf( error_str, "ASSERT: METER_ON RESET_STATE error veh %d call %d call_veh NULL",
				       veh_ptr->veh_nbr, call_ptr->call_number );
			     ERROR(veh_ptr->fleet, "", error_str);
			   }
			 else
			   (void) Call_set_state((CALL_HNDL) veh_ptr->call_ptr, CALL_SHORT_METER_STATE, HNDL_NULL);
		       }
		   }
	       }
             if ( ( (char)Fleet_get_value( (FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_GPS_BOOK ) == YES ) &&
                  ( (char)Fleet_get_value( (FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_GPS_STATE ) == YES ) &&
		  ( (char)Veh_get_value( (VEH_HNDL) veh_ptr, VEH_NAV_TYPE ) == STAR2000_MDT ) &&
                  ( !( Veh_get_state( (VEH_HNDL)veh_ptr, VEH_POSTED_STATE ) ) ) )
               {
                 if ( GPS_get_best_zone( (VEH_HNDL)veh_ptr, &zone_nbr ) == SUCCESS )
                   {
                     Veh_set_state((VEH_HNDL)veh_ptr, VEH_POSTED_STATE, (HNDL)zone_nbr);
                   }
               }
	     return (SUCCESS);
	   }
	 veh_ptr->t_status.meter_on = 0;
	 return (SUCCESS);

      default:
	 return ((int) veh_ptr->t_status.meter_on ? SUCCESS : FAIL);
      }				/* end case */
      break;

   case VEH_MSG_STATE:		/* was d_msg */
/***********************************************************/
/* VEH_MSG_STATE                                           */
/* STATE_SET                                               */
/*   OP: msg from veh processing                           */
/*   ARG: message nbr                                      */
/*   PRE: veh exists                                       */
/*   POST: fleet and driver stats incremented              */
/*         VEH_MSG_REQUEST made                            */
/*         if VEH_CALL_HNDL                                */
/*            Call_add_history (MESSAGE)                   */
/*   ERROR:  none                                          */
/* STATE_RESET                                             */
/*   OP: VEH_MSG_REQUEST answer                            */
/*   ARG: approve or disapprove                            */
/*   PRE: veh exists                                       */
/*   POST: no action at preset                             */
/*   ERROR: no request                                     */
/***********************************************************/

      switch (op)
      {
      case STATE_SET:
	 if ( argument == HNDL_NULL )
	    return (SUCCESS);


	 if (veh_ptr->call_ptr != NULL)
	   Call_add_history((CALL_HNDL) veh_ptr->call_ptr,
			    (VEH_HNDL) veh_ptr, MESSAGE, (short) argument, 0L, 0);	 

	 // Special HTD codes for MPK trip processing. These are automatically
	 // being managed by the MPKBridge software so don't generate exceptions
	 if ( (short)argument != 99 && (short)argument != 97 && (short)argument != 98 )
	   {
	     if (veh_ptr->call_ptr != NULL)
	       mk_except((struct calls *) veh_ptr->call_ptr, veh_ptr, MESSAGE, (short) argument, -1);
	     else
	       mk_except((struct calls *) NULL, veh_ptr, MESSAGE, (short) argument, -1);
	   }
	 else if ((short)argument == 99)
	   {
	     if (veh_ptr->call_ptr != NULL && veh_ptr->kela_node > 0)
	       {
		 db_open(KELANODE_FILE_ID, ISINOUT | ISMANULOCK);
		 bzero(&kelanode_rec, sizeof(KELANODE_REC));
		 kelanode_rec.tpak_id = veh_ptr->call_ptr->call_number;
		 kelanode_rec.nbr = veh_ptr->kela_node;
		 kelanode_rec.nbr = 0;
		 if ( db_read_key(KELANODE_FILE_ID, &kelanode_rec, &kelanode_key3, ISEQUAL) == SUCCESS )
		   {
		     mk_outb_text("");
		     sprintf(tmp_buf,"KELA: %s", kelanode_rec.booking_id);
		     mk_outb_text(tmp_buf);
		     send_msg_mmp(veh_ptr->mid, TEXT_DISPLAY, veh_ptr);
		   }
		 db_close(KELANODE_FILE_ID);
	       }
	   }
	 else if ((short)argument == 97)
	   {
	     // Arrive at stop
	     // timestamp arrival 
	     if (veh_ptr->call_ptr != NULL && veh_ptr->kela_node > 0)
	       {
		 db_open(KELANODE_FILE_ID, ISINOUT | ISMANULOCK);
		 bzero(&kelanode_rec, sizeof(KELANODE_REC));
		 kelanode_rec.tpak_id = veh_ptr->call_ptr->call_number;
		 kelanode_rec.nbr = veh_ptr->kela_node;
		 kelanode_rec.version = 0;
		 if ( db_read_key(KELANODE_FILE_ID, &kelanode_rec, &kelanode_key3, ISEQUAL) == SUCCESS )
		   {
		     tm_ptr = localtime(&mads_time);
		     strftime(kelanode_rec.arrival_time, 25, "%Y-%m-%dT%H:%M:%S", tm_ptr);
		     //sprintf(kelanode_rec.arrival_time, "%.8s", &stime[11]);
		     db_update(KELANODE_FILE_ID, &kelanode_rec);
		   }
		 db_close(KELANODE_FILE_ID);
	       }
	   }
	 
	 fleet[veh_ptr->fleet_nbr]->messages += 1;
	 veh_ptr->messages += 1;/* DRIVER */
	 return (SUCCESS);

      case STATE_RESET:
	 return (SUCCESS);

      default:
	 ERROR(' ', "", "VEH_MSG_STATE GET not presently implemented");
	 return (FAIL);
      }				/* end case */
      break;

   case VEH_NOSHOW_STATE:
/********************************************************************/
/* VEH_NOSHOW_STATE                                                 */
/* STATE_SET                                                        */
/*   OP: process a noshow request                                   */
/*   ARG: none                                                      */
/*   PRE: vehicle exists                                            */
/*   POST:                                                          */
/*         if VEH_EPOSTED_STATE                                     */
/*            if EPOST_TIMER                                        */
/*               EPOST_TIMER deleted                                */
/*         CALL_NOSHOW_STATE set                                    */
/*         switch on FLEET_NO_SHOW_ACTION                           */
/*            MANUAL: state set                                     */
/*            SEMI: state set                                       */
/*            AUTO: VEH_METER_ON_STATE reset                        */
/*                  VEH_ON_CALL_STATE reset                         */
/*                  VEH_ZONE_NBR set                                */
/*                  VEH_PRIORITY set to FLEET_NOSHOW_PRIORITY       */
/*                  driver stats incremented                        */
/*                     VEH_POSTED_STATE set                         */
/*           AUTO2:                                                 */
/*                  VEH_ON_CALL_STATE reset                         */
/*                  driver stats incremented                        */
/*                  VEH_PRIORITY set to FLEET_NOSHOW_PRIORITY       */
/*                     NO_SHOW_APP_POST (OK) msg sent to vehicle    */
/*   ERROR: illegal state transition (DENIED msg)                   */
/*          illegal fleet option                                    */
/* STATE_RESET                                                      */
/*   OP: noshow request either approved or disaproved               */
/*   ARG: R_APPROVE or R_DISAPPROVE                                 */
/*   PRE: vehicle exists                                            */
/*   POST: if ARGUMENT == R_APPROIVED                               */
/*           if FLEET_NO_SHOW_ACTION == AUTO2                       */
/*              return                                              */
/*           !VEH_NOSHOE_STATE                                      */
/*           VEH_PRIORITY set to FLEET_NOSHOW_PRIORITY              */
/*           VEH_ZONE_NBR set                                       */
/*           VEH_ON_CALL_STATE reset                                */
/*           driver noshow stats incremented                        */
/*              if FLEET_NO_SHOW_ACTION == SEMI                     */
/*                 VEH_POSTED_STATE set                             */
/*              else                                                */
/*                 if FLEET_NO_SHOW_ACTION == MANUAL                */
/*                    VEH_ZONE_NBR = 0                              */
/*                    NO_SHOW_APP_POST (OK) msg sent to vehicle     */
/*         if R_ARGUMENT == R_DISAPPROIVED                          */
/*            if FLEET_NO_SHOW_ACTION == MANUAL ||                  */
/*               FLEET_NO_SHOW_ACTION == SEMI                       */
/*                   NO_SHOW_DENIED  msg sent to vehicle            */
/*   ERROR: state transition                                        */
/********************************************************************/

      switch (op)
      {
      case STATE_SET:
	 if (veh_ptr == NULL)
	 {
	    ERROR(' ', "", "Null vehicle ptr");
	    return (FAIL);	/* an error */
	 }
	 if (veh_ptr->call_ptr == NULL)
	 {
	    sprintf(str_buf, "NOSHOW request from %d with no call", veh_ptr->veh_nbr);
	    ERROR(' ', "", str_buf);
	    return (FAIL);	/* an error */
	 }
	 if (veh_ptr->call_ptr->call_number != veh_ptr->call_nbr)	/* WHY DO THIS */
	 {
	    sprintf(str_buf, "call %d doesn't match veh %d call %d",
		    veh_ptr->call_ptr->call_number, veh_ptr->veh_nbr, veh_ptr->call_nbr);
	    ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_buf);
	    return (FAIL);
	 }
	 if (veh_ptr->t_status.e_posted)
	    if (veh_ptr->veh_epost_timer != NULL)
	    {
	       Timer_destroy((TIMER_HNDL) veh_ptr->veh_epost_timer);
	       veh_ptr->veh_epost_timer = NULL;
	    }


	 if ( (int)argument == 3000 )
	   Call_set_state((CALL_HNDL) veh_ptr->call_ptr, CALL_NOSHOW_STATE, (HNDL)3000);
	 else
	   Call_set_state((CALL_HNDL) veh_ptr->call_ptr, CALL_NOSHOW_STATE, (HNDL)argument);

	 /* automatically take actions if noshow originated at ITM */
	 if ( (int)argument == 3000 &&
              (veh_ptr->call_ptr->status.ns_short == 0) )
	   {
	     tmp = veh_ptr->zone_num;
	     if (tmp == 0)
	       tmp = veh_ptr->call_ptr->pickup_zone;
	     (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_CALL_STATE, HNDL_NULL, HNDL_NULL);
	     veh_ptr->zone_num = tmp;
	     veh_ptr->priority = fleet[veh_ptr->fleet_nbr]->drv_nshow_pri;
	     send_text_predefined( veh_ptr->mid, NO_SHOW_APP_POST, veh_ptr );
	     if ( !veh_ptr->t_status.meter_on )
	       (void) Veh_set_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, (HNDL)veh_ptr->zone_num);
	     ++veh_ptr->no_shows;
	     return(SUCCESS);
	   }

	 switch (fleet[veh_ptr->fleet_nbr]->no_show_action)
	 {
	 case '1':
	 case '2':
	    veh_ptr->t_status.noshow = 1;
	    break;

	 case '3':
	    /** Meter OFF is a physical action that shouldn't be handled in code **/
#ifdef FOO
	    (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_METER_ON_STATE, HNDL_NULL, HNDL_NULL);
#endif
	    tmp = veh_ptr->zone_num;
	    if (tmp == 0)
	       tmp = veh_ptr->call_ptr->pickup_zone;
	    (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_CALL_STATE, HNDL_NULL, HNDL_NULL);
	    veh_ptr->zone_num = tmp;
	    veh_ptr->priority = fleet[veh_ptr->fleet_nbr]->drv_nshow_pri;
	    send_text_predefined( veh_ptr->mid, NO_SHOW_APP_POST, veh_ptr );	    
	    (void) Veh_set_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, (HNDL)veh_ptr->zone_num);
	    ++veh_ptr->no_shows;
	    break;

	 case '4':
	    (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_CALL_STATE, HNDL_NULL, HNDL_NULL);
	    ++veh_ptr->no_shows;
	    veh_ptr->priority = fleet[veh_ptr->fleet_nbr]->drv_nshow_pri;
	    send_text_predefined(veh_ptr->mid, NO_SHOW_APP_POST, veh_ptr);
	    break;

	 default:
	    ERROR(' ', "", "invalid noshow option");	/* option does not exist */
	    return (FAIL);
	 }
	 return (SUCCESS);

      case STATE_RESET:
	 if (fleet[veh_ptr->fleet_nbr]->no_show_action == '4')
	    return (SUCCESS);
	 if ((short) argument == R_APPROVE)
	 {
	    veh_ptr->priority = fleet[veh_ptr->fleet_nbr]->drv_nshow_pri;
	    veh_ptr->t_status.noshow = 0;
	    if (veh_ptr->call_ptr == NULL)
	    {
	       ERROR(' ', "", "NULL call ptr");
	       tmp = veh_ptr->zone_num;
	    } else
	    {
	       if (veh_ptr->call_ptr->pickup_zone != 0)
		  tmp = veh_ptr->call_ptr->pickup_zone;
	       else
		  tmp = veh_ptr->zone_num;
	    }
	    send_text_predefined( veh_ptr->mid, NO_SHOW_APP_POST, veh_ptr );
	    (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_CALL_STATE, HNDL_NULL, HNDL_NULL);
	    if ( ( tmp != 0 ) &&
		 ( ( fleet[veh_ptr->fleet_nbr]->no_show_action == '2' ) ||
		   ( fleet[veh_ptr->fleet_nbr]->no_show_action == '3' ) ) )
	      {
		Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL );
		veh_ptr->zone_num = tmp;		
		Veh_set_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, (HNDL)tmp );
	      }

	    ++veh_ptr->no_shows;/* DRIVER */
	    
	 } else
	 {			/* R_DISAPPROVE */
	    if (fleet[veh_ptr->fleet_nbr]->no_show_action == '1' ||
		fleet[veh_ptr->fleet_nbr]->no_show_action == '2')
	    {
	       veh_ptr->t_status.noshow = 0;
	       send_text_predefined(veh_ptr->mid, NO_SHOW_DENIED, veh_ptr);
	    }
	 }
	 return (SUCCESS);
      default:
	 return ((int) veh_ptr->t_status.noshow ? SUCCESS : FAIL);
      }				/* end case */
      break;

   case VEH_OK_TO_MATCH_STATE:	/* GET only */
/***************************************************************/
/* VEH_OK_TO_MATCH_STATE                                       */
/* GET_ONLY: is the vehicle in a state where it can be matched */
/***************************************************************/

      if (op != GET)
      {
#ifdef ASSERT
	 printf("Error - attempt to PUT VEH_OK_TO_MATCH\n");
#endif
	 return (FAIL);
      } else
	 return ((veh_ptr->t_status.pckup ||
		  veh_ptr->t_status.offered ||
		  veh_ptr->t_status.emer ||
		  veh_ptr->t_status.accepted) ? FAIL : SUCCESS);
      break;

   case VEH_OK_TO_PICKUP_STATE:/* WAS is_pickup_allowed */
/***************************************************************/
/* VEH_OK_TO_PICKUP_STATE:                                     */
/* GET_ONLY: is the vehicle in a state where pickup is allowed */
/***************************************************************/

      if (op != GET)
	 return (FAIL);

      if (veh_ptr->t_status.meter_working == FALSE)	/* meter NOT WORKING */
	 return (SUCCESS);

      if (veh_ptr->call_ptr == NULL)
	 return (SUCCESS);

      if (veh_ptr->call_ptr->call_type.passenger &&
	  fleet[veh_ptr->fleet_nbr]->criteria.met_pass_call)
	 return (SUCCESS);

      if (veh_ptr->call_ptr->call_type.delivery &&
	  fleet[veh_ptr->fleet_nbr]->criteria.met_del_call)
	 return (SUCCESS);

      if (veh_ptr->call_ptr->call_type.xtype &&
	  fleet[veh_ptr->fleet_nbr]->criteria.met_typex_call)
	 return (SUCCESS);

      return (FAIL);
      break;

   case VEH_ON_BREAK_STATE:	/* RESET WAS d_post & on_break_post */
/***************************************************/
/* VEH_ON_BREAK_STATE                              */
/* STATE_SET                                       */
/*   OP: veh want to take a break                  */
/*   ARG: none                                     */
/*   PRE: veh exists                               */
/*        driver exists                            */
/*        VEH_POSTED_STATE                         */
/*   POST: VEH_ON_BREAK_TIME set                   */
/*         fleet break stats incremented           */
/*         break time message sent to veh          */
/*         driver break stats incremented          */
/*   ERROR: illegal state transition               */
/* STATE_RESET                                     */
/*   OP: vehicle has posted while on a break       */
/*   ARG: key used                                 */
/*   PRE: ON_BREAK set                             */
/*        vehilce exists                           */
/*        driver exists                            */
/*   POST: fleet now breaks stats decremented      */
/*        if exceeded break time                   */
/*            Zone_remove_veh called               */
/*            VEH_ZONE_NBR = 0                     */
/*            LATE_BREAK msg sent to vehicle       */
/*   ERROR: none                                   */
/***************************************************/

      switch (op)
      {
      case STATE_SET:
        stc_break = FALSE;
        if ( (short) argument > 0 )
	  {
	    veh_ptr->on_break_time = mads_time;
	    veh_ptr->t_status.on_break = 1;
	    Veh_set_state( (VEH_HNDL) veh_ptr, VEH_POSTED_STATE, (HNDL) argument );
            stc_break = TRUE;  // this is used so the STC break time is set according
            // to the STC timer for the destination zone
	  }
	 veh_ptr->on_break_time = mads_time;
	 veh_ptr->t_status.on_break = 1;
	 fleet[veh_ptr->fleet_nbr]->breaks += 1;
	 ++fleet[veh_ptr->fleet_nbr]->now_breaks;
	 veh_ptr->breaks += 1;

         if ( stc_break == TRUE )
           {
             if (Zone_get_hndl((short) argument, veh_ptr->fleet_nbr, &zone_hndl) == FAIL)
               {
                 sprintf(error_str, "bad zone nbr %d", (short) argument);
                 ERROR(' ', "", error_str);
                 return (FAIL);
               }
             break_time = (short)Zone_get_value( zone_hndl, ZONE_EPOST_LEAD_TIME ) * 60;
             if ( break_time == 0 )
               break_time = fleet[veh_ptr->fleet_nbr]->e_post_lead_time;
           }
         else
           break_time = fleet[veh_ptr->fleet_nbr]->break_duration;

	 sprintf(str_buf, "%s %d %s", BREAK_TIME,
		 break_time / 60, MINUTES);
	 (void) send_text(veh_ptr->mid, str_buf, veh_ptr, PRIORITY_2);

	 // Set break timer for R4.4 Customers
	 if (0)
           {
             Veh_add_timer( (VEH_HNDL) veh_ptr, T_BREAK,
                            break_time,
                            veh_break_notify );
           }
	 
	 return (SUCCESS);

      case STATE_RESET:
	if ((short) argument2 != 1 )
          Veh_delete_timer((VEH_HNDL)veh_ptr, T_BREAK);
        
	 veh_ptr->t_status.on_break = 0;
	 --fleet[veh_ptr->fleet_nbr]->now_breaks;

         
         if ((short) argument != VEH_POST_KEY)
            return(SUCCESS);

	 if (fleet[veh_ptr->fleet_nbr]->break_duration < mads_time - veh_ptr->on_break_time)
	 {
            if (veh_ptr->zone_num && veh_ptr->t_status.posted)
            {
	       (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL);
	       veh_ptr->zone_num = 0;
	       send_text_predefined(veh_ptr->mid, LATE_BREAK, veh_ptr);
            }
	    return (FAIL);
	 }
         if ((short)argument2 == 1)
           {
             if (veh_ptr->zone_num && veh_ptr->t_status.posted)
               {
                 (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL);
                 veh_ptr->zone_num = 0;
                 send_text_predefined(veh_ptr->mid, LATE_BREAK, veh_ptr);
               }
             return (FAIL);
           }
         
	 if ( (char)Fleet_get_value((FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_GPS_STATE ) == YES )
	   {
	     Zone_get_hndl( veh_ptr->zone_num, veh_ptr->fleet_nbr, &zone_hndl );
	     if ( zone_hndl != HNDL_NULL )
	       {
		 if ( GPS_check_zone( zone_hndl, veh_ptr ) != SUCCESS )
		   {
		     sprintf( error_str, "%s%s",
			      catgets(DISPATCH_catd, 1, 42, "NOT IN REQUESTED ZONE"), CR );
		     
		     (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL);
		     veh_ptr->zone_num = 0;
		     send_text( veh_ptr->mid, error_str, veh_ptr, PRIORITY_3 );		     
		     return (FAIL);
		   }
	       }
	   }
		     
	 return (SUCCESS);
      default:
	 return ((int) veh_ptr->t_status.on_break ? SUCCESS : FAIL);
      }				/* end case */

   case VEH_ON_CALL_STATE:	/* WAS reset  d_call_close */
/****************************************************************/
/* VEH_ON_CALL_STATE                                            */
/* STATE_SET                                                    */
/*   OP:  vehicle is on a call                                  */
/*   ARG: none                                                  */
/*   PRE: vehicel exists                                        */
/*   POST: state set                                            */
/*   ERROR: illegal state transition                            */
/* STATE_RESET                                                  */
/*   OP:  vehicle no longer on call                             */
/*   ARG: NULL or VEH_NO_POST_CHECK or VEH_STAKCED_CALLS_CHECK  */
/*   PRE: VEH_ON_CALL_STATE                                     */
/*        vehicle exists                                        */
/*        call exists                                           */
/*   POST:                                                      */
/*         if ARGUMENT != VEH_NO_POST_CHECK                     */
/*            if VEH_EPOSTED_STATE                              */
/*               EPOST timer deleted                            */
/*               Zone_remove_veh called                         */
/*            else if VEH_POSTED_STATE                          */
/*                    Zone_remove_veh called                    */
/*         VEH_INDL_STATE                                       */
/*         !VEH_ACCEPTED_STATE                                  */
/*         !VEH_OFFERED_STATE                                   */
/*         !VEH_POSTED_STATE                                    */
/*         !VEH_PICKUP_STATE                                    */
/*         !VEH_LATE_METER_STATE                                */
/*         !VEH_FLAG_REQUEST_STATE                              */
/*         VEH_CALL_NBR = 0                                     */
/*         VEH_CALL_HNDL = HNDL_NULL                            */
/*         VEH_WARM_TIME = 0                                    */
/*         VEH_TIME_INFO = 0                                    */
/*         !VEH_METER_ON_STATE                                  */
/*   ERROR: illegal state transition                            */
/****************************************************************/

      switch (op)
      {
      case STATE_SET:
         if(veh_ptr->t_status.on_break)
              (void) Veh_reset_state(veh_hndl, VEH_ON_BREAK_STATE, HNDL_NULL, HNDL_NULL);
	 return (SUCCESS);

      case STATE_RESET:	/* WAS RESET_VEH & call of Zone_remove_veh */
	 if (argument != VEH_NO_POST_CHECK)
	 {
	    if (veh_ptr->t_status.e_posted)
	    {
	       if (veh_ptr->veh_epost_timer != NULL)
	       {
		  Timer_destroy((TIMER_HNDL) veh_ptr->veh_epost_timer);
		  veh_ptr->veh_epost_timer = NULL;
	       }
	       if (Zone_remove_veh(veh_ptr->epost_zone, (VEH_HNDL) veh_ptr) == FAIL)
		  ERROR(' ', "", "Zone_remove_veh failure");
	    } else if (veh_ptr->t_status.posted)
	    {
	       if (Zone_remove_veh(veh_ptr->zone_num, (VEH_HNDL) veh_ptr) == FAIL)
		  ERROR(' ', "", "Zone_remove_veh failure");
	    }
	    veh_ptr->t_status.e_posted = 0;
	 }
	 veh_ptr->t_status.idle = 1;
	 veh_ptr->t_status.accepted = 0;
	 veh_ptr->t_status.offered = 0;
	 veh_ptr->t_status.posted = 0;
	 veh_ptr->t_status.pckup = 0;
	 veh_ptr->t_status.late_meter = 0;
         veh_ptr->t_status.noshow = 0;
	 veh_ptr->t_status.flag = 0;
#ifdef FOO
	 veh_ptr->t_status.meter_on = 0;	/* MRB 6 Sept 91 */
#endif
	 veh_ptr->call_nbr = 0;
	 veh_ptr->call_ptr = NULL;
	 veh_ptr->zone_num = 0;
	 veh_ptr->warm_time = 0;
	 veh_ptr->time_info = 0;/* !CALL_CLOSE */
	 /* NOSHOW ???? */

	 /*
	  * may want to generalize thsi to remove other checks for stacked
	  * calls
	  */

	 return (SUCCESS);

      default:
	 return ((int) (veh_ptr->t_status.offered ||
			veh_ptr->t_status.accepted ||
			veh_ptr->t_status.pckup ||
			veh_ptr->t_status.flag));
      }				/* end case */
      break;

   case VEH_PICKUP_STATE:	/* WAS d_pickup (part of)  & d_call_close */
/*****************************************************************/
/* VEH_PICKUP_STATE                                              */
/* STATE_SET                                                     */
/*   OP: veh has picked up customer on system assigned call      */
/*   ARG: optional call_hndl                                     */
/*   PRE: call exists                                            */
/*        vehicle exists                                         */
/*   POST:                                                       */
/*         if VEH_LATE_METER_STATE                               */
/*            fleet now late_meter stats decremented             */
/*            !VEH_LATE_METER_STATE                              */
/*         if ARGUMENT != HNDL_NULL                              */
/*            CALL_PICKUP_STATE set for ARGUMENT                 */
/*         else                                                  */
/*            CALL_PICKUP_STATE for VEH_CALL_HNDL                */
/*         !VEH_NO_SHOW                                          */
/*         if CALL_LOCAL_STATE                                   */
/*            VEH_LOCAL_STATE set                                */
/*         if VEH_FLAG_REQUEST_STATE                             */
/*            fleet & driver flag stats incremented              */
/*         VEH_INFO_TIME set                                     */
/*         VEH_ZONE_NBR = CALL_PICKUP_ZONE                       */
/*   ERROR: illegl state transition                              */
/* STATE_RESET                                                   */
/*   OP: part of close                                           */
/*   ARG: none                                                   */
/*   PRE: veh exists                                             */
/*   POST:                                                       */
/*        if VEH_LOCAL_STATE                                     */
/*           VEH_PRIORITY = FLEET_DRV_LOCAL_PRIORITY             */
/*        else                                                   */
/*           VEH_PRIORITY = FLEET_DRV_REG_RPIORITY               */
/*        if ARGUMENT != HNDL_NULL                               */
/*           CALL_PICKUP_STATE reset                             */
/*        if ARGUMENT == VEH_CALL_HNDL || ARGUMENT == HNDL_NULL  */
/*           VEH_ON_CALL_STATE reset (VEH_NO_POST_CHECK)         */
/*           if VEH_EPOSTED_STATE                                */
/*              VEH_ZONE_NBR = VEH_EPOST_NBR                     */
/*              if VEH_EPOSTED_STATE reset == FAIL               */
/*                 VEH_ON_CALL_STATE reset                       */
/*              else if !VEH_ACCEPTED_STATE &&                   */
/*                         Dispatcher_call_match called          */
/*                         if no match                           */
/*                            q_position mesg sent to veh        */
/*        VEH_METER_ON_STATE reset                               */
/*  ERROR: none                                                  */
/*****************************************************************/

      switch (op)
      {
      case STATE_SET:
	 /* put in check state */
	 if (veh_ptr->call_nbr > 0 && !veh_ptr->t_status.accepted)
	 {
	   if ( veh_ptr->call_ptr != NULL )
	     {
	       Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL );
	       Veh_set_state((VEH_HNDL) veh_ptr, VEH_FLAG_STATE, HNDL_NULL );	       
	       Call_reset_state((CALL_HNDL) veh_ptr->call_ptr, CALL_OFFERED_STATE, HNDL_NULL );
	     }
	   return(0);
	 }
	 
	 if (veh_ptr->t_status.pckup)
	 {
	   send_text_predefined(veh_ptr->mid, NO_ACPT_ACPT, veh_ptr);
	   return (FAIL);
	 }
	 
	 if ( veh_ptr->t_status.accepted
	     && !veh_ptr->t_status.meter_on
	     &&  fleet[veh_ptr->fleet_nbr]->approach_tariff == PICKUP_WITH_METER_ON )
	   {
	     send_text_predefined(veh_ptr->mid,NO_CLOSE_ALW,veh_ptr);
	     return(FAIL);
	   }
	 /* CALL ?? */
	 if (veh_ptr->t_status.late_meter)
	 {
	    --fleet[veh_ptr->fleet_nbr]->now_late_meters;
	    veh_ptr->t_status.late_meter = 0;
	 }
	 if ( veh_ptr->call_ptr != NULL )
	   {
	     if (veh_ptr->call_ptr->status.local)
	       {
		 veh_ptr->t_status.local = 1;
	       }
	     if (argument != HNDL_NULL)
	       Call_set_state((CALL_HNDL) argument, CALL_PICKUP_STATE, (HNDL)veh_hndl);
	     else
	       Call_set_state((CALL_HNDL) veh_ptr->call_ptr, CALL_PICKUP_STATE, (HNDL) veh_hndl);
	     veh_ptr->zone_num =
	       (short) Call_get_value((CALL_HNDL) veh_ptr->call_ptr, CALL_PICKUP_ZONE);	     
	   }

	 veh_ptr->t_status.pckup = 1;
	 veh_ptr->t_status.noshow = 0;

	  // VPU trips send 'fast location update' message
	  call_ptr = veh_ptr->call_ptr;
	  if ( call_ptr != NULL)
	    {
	      if ((cl_ptr = get_call_record(call_ptr->c_isam_num, call_ptr->call_number)) != NULL)
		{
		  if ((!strncmp(cl_ptr->extended_type, "KV",2)))
		    {
		      mk_outb_text("");
		      add_outb_text("%SZ2000C8%SZ230190");
		      send_msg_mmp(veh_ptr->mid, TEXT_DISPLAY, veh_ptr);
		    }
		}
	    }
	 
	 /* FLAG REQUEST HNDLING ??? */
	 if (veh_ptr->t_status.flag)
	 {
	    /* SHOULDNT GET HERE ? */
	    ++fleet[veh_ptr->fleet_nbr]->flags;
	    ++fleet[veh_ptr->fleet_nbr]->now_flags;
	    ++veh_ptr->flags;	/* DRIVER ?? */
	 }

	 return (SUCCESS);

      case STATE_RESET:
	 if (veh_ptr->call_ptr == NULL)
	 {
	    sprintf(error_str, "Null ptr call %d VEH_PICKUP_STATE reset veh #%d arg %x",
                    veh_ptr->call_nbr, veh_ptr->veh_nbr, argument);
	 }
	 if (veh_ptr->t_status.local)
	 {
	    if (veh_ptr->time_info + fleet[veh_ptr->fleet_nbr]->local_calls_time < mads_time)
	       veh_ptr->priority = fleet[veh_ptr->fleet_nbr]->drv_locl_pri;
	 } else
	 {
	    veh_ptr->priority = fleet[veh_ptr->fleet_nbr]->drv_reg_pri;
	 }

	 veh_ptr->last_call_handled = veh_ptr->call_nbr;	/* G7 thanks - right
								 * place MRB */
	 if (argument != HNDL_NULL)
	 {
	    Call_reset_state((CALL_HNDL) argument, CALL_PICKUP_STATE, HNDL_NULL);
	    if (argument == (HNDL) veh_ptr->call_ptr)
	       veh_call_close(veh_ptr);
	 } else
	 {
            if (veh_ptr->call_ptr != NULL)
	      {
		if ( veh_ptr != veh_ptr->call_ptr->veh_ptr )
		  {
		    call_ptr = veh_ptr->call_ptr;
		    if ( veh_ptr->call_ptr->veh_ptr != NULL )
		      sprintf( error_str, "ASSERT: VEH_PICKUP RESET_STATE error veh %d call %d call_veh %d",
			      veh_ptr->veh_nbr, call_ptr->call_number, call_ptr->veh_ptr->veh_nbr );
		    else
		      sprintf( error_str, "ASSERT: METER_ON RESET_STATE error veh %d call %d call_veh NULL",
			      veh_ptr->veh_nbr, veh_ptr->call_ptr->call_number );
		    ERROR(veh_ptr->fleet, "", error_str);
		  }
		else
		  Call_reset_state((CALL_HNDL) veh_ptr->call_ptr, CALL_PICKUP_STATE, HNDL_NULL);
	      }
	    veh_call_close(veh_ptr);
	 }
	 return (SUCCESS);

      default:
	 return ((int) veh_ptr->t_status.pckup ? SUCCESS : FAIL);
      }				/* end case */
      break;

   case VEH_POSTED_STATE:	/* WAS d_post */
/***********************************************************/
/* VEH_POSTED_STATE                                        */
/* STATE_SET                                               */
/*   OP: vehicle post request processing                   */
/*   ARG: zone number                                      */
/*   PRE: zone exists                                      */
/*        vehicle exists                                   */
/*   POST:                                                 */
/*         if VEH_POSTED_STATE                             */
/*            if ARGUMENT == VEH_ZONE_NBR                  */
/*               if veh not in Zone_veh_list               */
/*                  Zone_add_veh called                    */
/*                  VEH_Q_PRIORITY = VEH_PRIORITY          */
/*                  return                                 */
/*            else                                         */
/*               Zone_remove_veh                           */
/*          VEH_Q_PRIORITY = VEH_PRIORITY                  */
/*          Zone_add_veh_called                            */
/*          VEH_PRIORITY = FLEET_DRIV_REG_PRIORITY         */
/*          Dispatcher_veh_match called                    */
/*          if not matched                                 */
/*             q_posuition msg sent to vehicle             */
/*   ERROR: illegal state transition                       */
/* STATE_RESET                                             */
/*   OP: unpost the vehicle                                */
/*   ARG: none or MSG                                      */
/*   PRE: veh exists                                       */
/*   POST: Zone_remove_veh called                          */
/*         writer_unpost called                            */
/*         if ARGUMENT == MSG                              */
/*            POST_OFF msg set to vehicle                  */
/*            if driver of vehicle suspended               */
/*               suspend msg sent to vehicle               */
/*   ERROR: illegal state transition                       */
/***********************************************************/

      switch (op)
      {
	 /* argument is post zone nbr */
      case STATE_RESET:
	 Zone_remove_veh(veh_ptr->zone_num, (VEH_HNDL) veh_ptr);
	 writer_unpost(veh_ptr->fleet, veh_ptr->veh_nbr);

	 veh_ptr->t_status.posted = 0;

         // Remove any HTTP queries from the list
         if ( veh_ptr->http_list != HNDL_NULL )
           {
             Veh_http_list_each_get( veh_ptr->http_list, http_hndl )
               {
                 Veh_http_list_remove( veh_ptr->http_list, http_hndl );
                 free( (char *)http_hndl );
               } Veh_http_list_each_end(veh_ptr->http_list)
           }

	 if (argument == (HNDL) MSG)
	 {
	    send_text_predefined(veh_ptr->mid, POST_OFF, veh_ptr);
	    (void) is_suspended(veh_ptr->mid, veh_ptr, EITHER, MSG);
	 }
	 return (SUCCESS);

      case STATE_SET:
	 /* could maybe move some of this to KEY */
	 if (Zone_get_hndl((short) argument, veh_ptr->fleet_nbr, &zone_hndl) == FAIL)
	   {
	     sprintf(error_str, "bad zone nbr %d", (short) argument);
	     ERROR(' ', "", error_str);
	     return (FAIL);
	   }	 


	 /* is there a 'future' suspension that has become active ?*/
	 if ( Veh_check_suspensions( (VEH_HNDL)veh_ptr ) == SUCCESS )
	   {
	     return (FAIL);
	   }

	 
	 /** is taxi within limits of the zone **/
	 /** don't perform the check if warm   **/
	 /** start is in progress.             **/
	 if ( ((char)Fleet_get_value( (FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_GPS_STATE ) == YES ) &&
              (GPS_check_zone( zone_hndl, veh_ptr ) != SUCCESS ) &&
              (!warm_start) &&
              ( veh_ptr->t_status.on_break != 1 ))
	   {
	     if ( (int)Fleet_get_value( (FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_GPS_TIME ) > 0 )
	       {
		 if ( abs( (time_t )Veh_get_value( (VEH_HNDL) veh_ptr, VEH_GPS_TIME ) - time(0) ) >=
		      (int)Fleet_get_value( (FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_GPS_TIME ) )
		   return(FAIL);
	       }
		 
	     
	     if ( ( GPS_get_best_zone( (VEH_HNDL) veh_ptr, &newer_zone_nbr ) != SUCCESS ) ||
		  ( (short) newer_zone_nbr <= 0 ) )
	       {
		 send_text( veh_ptr->mid, catgets(DISPATCH_catd, 1, 40, "GPS SAYS: NOT IN ZONE"), veh_ptr, PRIORITY_3 );
		 return (FAIL);
	       }
	     sprintf( error_str, "%s%s%s %d",
		      "VÄÄRÄ RUUTU",
		      CR,
		      "OLET RUUDUSSA",
		      (short)newer_zone_nbr );
	     send_text( veh_ptr->mid, error_str, veh_ptr, PRIORITY_3 );
	     (short)argument = newer_zone_nbr;
	     if (Zone_get_hndl((short) newer_zone_nbr, veh_ptr->fleet_nbr, &zone_hndl) == FAIL)
	       {
		 sprintf(error_str, "bad zone nbr %d", (short) argument);
		 ERROR(' ', "", error_str);
		 return (FAIL);
	       }	 	     
	     new_zone = TRUE;
	   }	 	       

	 if (POSTED(veh_ptr))
	 {
	    if (veh_ptr->zone_num == (short) argument)
	    {
	       veh_list_hndl = (ZONE_VEH_LIST_HNDL) Zone_get_value(zone_hndl, ZONE_AVAIL_VEH_LIST);
	       if ((veh_hndl = Zone_veh_list_find(veh_list_hndl, (VEH_HNDL) veh_ptr)) == (VEH_HNDL) HNDL_NULL)
	       {
		Vehicle_change_q_priority((VEH_HNDL)veh_ptr, veh_ptr->priority);

		(void) Zone_add_veh((short) argument, (VEH_HNDL) veh_ptr);
	       }
	       return (FAIL);
	    } else
	    {
	       /* different zone */
	       if (Zone_remove_veh(veh_ptr->zone_num, (VEH_HNDL) veh_ptr) == FAIL)
               {
                  sprintf(error_str, "Zone_remove_veh %d failure zn %d new zn %d",
                                      veh_ptr->veh_nbr, veh_ptr->zone_num, (short) argument);
		  ERROR(' ', "", error_str);
               }
	    }
	 } else
	 {
	    /* not posted */
	    if ((short) argument == 0)	/* ERROR */
	    {
	       ERROR(' ', "", "Attempt to post to zone 0");
	       return (FAIL);
	    }
	 }
	 veh_ptr->t_status.posted = 1;

	 Vehicle_change_q_priority((VEH_HNDL)veh_ptr, veh_ptr->priority);
	 tmp_veh_hndl = HNDL_NULL;
	 veh_list_hndl = (ZONE_VEH_LIST_HNDL) Zone_get_value(zone_hndl, ZONE_AVAIL_VEH_LIST);	 
	 if ((tmp_veh_hndl = Zone_veh_list_find(veh_list_hndl, (VEH_HNDL) veh_ptr)) == (VEH_HNDL) veh_ptr )
	   Zone_remove_veh( (short)argument, (VEH_HNDL) veh_ptr );
			   
	 (void) Zone_add_veh((short) argument, (VEH_HNDL) veh_ptr);
	 veh_ptr->priority = fleet[veh_ptr->fleet_nbr]->drv_reg_pri;
	 if (Dispatcher_veh_match((VEH_HNDL) veh_ptr, (short) argument) == FAIL)
	    send_qp((short) argument, veh_ptr);

	 veh_ptr->last_action_time = mads_time;
         Veh_set_value( (VEH_HNDL) veh_ptr, VEH_SEND_GPS_NOT_IN_ZONE, (HNDL) TRUE );
	 return (SUCCESS);
      default:
	 return ((int) veh_ptr->t_status.posted ? SUCCESS : FAIL);
      }				/* end case */
      break;

   case VEH_SHORT_METER_STATE:
/****************************************************/
/* VEH_SHORT_METER_STATE                            */
/* STATE_SET                                        */
/*   OP: short meter has occurred                   */
/*   ARG: none                                      */
/*   PRE: veh exists                                */
/*   POST: VEH_SHORT_METER_STATE                    */
/*   ERROR: VEH_SHORT_METER_STATE                   */
/* STATE_RESET                                      */
/*   OP: short meter condition recended             */
/*   ARG: none                                      */
/*   PRE: VEH_SHORT_METER_STATE                     */
/*        veh_exists                                */
/*   POST: !VEH_SHORT_METER_STATE                   */
/*   ERROR: !VEH_SHORT_METER_STATE                  */
/****************************************************/

/* really not a state now - butcould be             */

      switch (op)
      {
      case STATE_SET:
	 ++veh_ptr->short_meters;
	 return (SUCCESS);
      case STATE_RESET:
	 return (SUCCESS);
      default:
	 /* CANT GET */
	 return (FAIL);
      }				/* end case */
      break;

   case VEH_SIGN_IN_STATE:	/* d_sign_in - d_sign_out */
/*********************************************************/
/* VEH_SIGN_IN_STATE                                     */
/* STATE_SET                                             */
/*   OP: sign in the vehicle                             */
/*   ARG: driver id                                      */
/*   PRE: vehicle exists                                 */
/*   POST:  veh_driver values set                        */
/*          Veh_initialize called                        */
/*          VEH_DRIVER_ID = ARGUMENT                     */
/*          VEH_IN_USE state set                         */
/*          VEH_IDLE_STATE                               */
/*          VEH_STATE_START_TIME set                     */
/*          VEH_SIGNED_ON_TIME set                       */
/*          VEH_SIGNED_OFF_TIME = 0                      */
/*          PRIORITY set to FLEET_DRV_PRIORITY           */
/*          fleet statistic incremented                  */
/*          vehicle history updated                      */
/*          authorize msg sent to vehicle                */
/*          SIGN_IN_OK msg sent to vehicle               */
/*          vehicle stats incremented                    */
/*   ERROR: none                                         */
/* STATE_RESET                                           */
/*   OP: sign out the vehicle                            */
/*   PRE: veh exists                                     */
/*   POST: VEH_IN_USE_STATE reset                        */
/*         VEH_STGNED_OFF_TIME set                       */
/*         writer_sign_off called                        */
/*         driver sign_oof stats incremented             */
/*         vehicle driver stats writen to db             */
/*         VEH_DRIVER_ID = 0                             */
/*         veh driver attributes reset                   */
/*         if VEH_ON_BREAK_STATE                         */
/*            fleet now on break stats decremented       */
/*         if VEH_LATE_METER_STATE                       */
/*            !VEH_LATE_METER_STATE                      */
/*            fleet now late meter stats decremented     */
/*         if VEH_FLAG_REQUEST_STATE                     */
/*            fleet now flag request stats decremented   */
/*         VEH_ON_CALL_STATE reset                       */
/*         fleet no_signed_on stats decremented          */
/*         fleet signed_off stats incremented            */
/*         SIGN_OUT msg sent to vehicle                  */
/*         deauthorize msg sent to vehicle               */
/*   ERROR: none                                         */
/*********************************************************/

      switch (op)
      {
      case STATE_SET:
	 Veh_initialize(veh_hndl);
	 veh_ptr->signed_on_date = mads_time;
	 veh_ptr->t_status.inuse = 1;
	 veh_ptr->t_status.idle = 1;
	 veh_ptr->driver_id = (int) argument;
	 veh_ptr->stat_start_tm = mads_time;
	 veh_bit_update(veh_ptr, STATE_SET);

	 veh_ptr->priority = fleet[veh_ptr->fleet_nbr]->drv_reg_pri;
	 veh_ptr->signed_on_date = mads_time;
	 veh_ptr->signed_off_date = 0;


	 if (writer_sign_on(veh_ptr->fleet, veh_ptr->veh_nbr, veh_ptr->driver_id) < 0)
	    ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", "writer_sign_on: failed");

	 authorize(veh_ptr);
	 
#ifdef NAMERICA
	 send_text_predefined(veh_ptr->mid, SIGN_IN_OK, veh_ptr);
#endif

	 ++fleet[veh_ptr->fleet_nbr]->signed_on;
	 ++fleet[veh_ptr->fleet_nbr]->now_signed_on;
	 ++veh_ptr->sign_ins;

         //veh_ptr->gps_lat = 0.0;
         //veh_ptr->gps_long = 0.0;
	 writer_gps_state( veh_hndl, VEH_GPS_STATE_OFF );
	 
	 return (SUCCESS);
	 
      case STATE_RESET:
	 if ((veh_ptr->fleet_nbr < 0) || (veh_ptr->fleet_nbr > 7))
	    return(FAIL);
	 veh_ptr->signed_off_date = mads_time;
	 veh_ptr->t_status.inuse = 0;
	 veh_ptr->t_status.meter_on = 0;
	 writer_sign_off(veh_ptr);
	 ++veh_ptr->sign_offs;

	 /** Clear any driver suspensions from the Fleet lists **/
	 if ( Fleet_get_hndl(BY_ID, veh_ptr->fleet, &fleet_hndl) != FAIL )
	   {
	     if (Fleet_susp_drv_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_PASS_LIST),
					  veh_ptr->driver_id) != HNDL_NULL)
	       {
		 Fleet_susp_drv_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_PASS_LIST),
					    (VEH_HNDL)veh_ptr);
	       }
	     
	     if (Fleet_susp_drv_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_DELIV_LIST),
					  veh_ptr->driver_id) != HNDL_NULL)
	       {
		 Fleet_susp_drv_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_DELIV_LIST),
					    (VEH_HNDL)veh_ptr);
	       }
	     
	     if (Fleet_susp_drv_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_TYPEX_LIST),
					  veh_ptr->driver_id) != HNDL_NULL)
	       {
		 Fleet_susp_drv_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_TYPEX_LIST),
					    (VEH_HNDL)veh_ptr);
	       }	 
	   }
	 
	 veh_ptr->driver_id = 0;
	 veh_bit_update(veh_ptr, STATE_RESET);

	 if (veh_ptr->t_status.on_break)
	    --fleet[veh_ptr->fleet_nbr]->now_breaks;
	 if (veh_ptr->t_status.late_meter)
	 {
	    --fleet[veh_ptr->fleet_nbr]->now_late_meters;
	    veh_ptr->t_status.late_meter = 0;
	 }
	 if (veh_ptr->t_status.flag)
	 {
	    --fleet[veh_ptr->fleet_nbr]->now_flags;
	 }
	 (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_CALL_STATE, HNDL_NULL, HNDL_NULL);
	 --fleet[veh_ptr->fleet_nbr]->now_signed_on;
	 ++fleet[veh_ptr->fleet_nbr]->signed_off;
	 send_text_predefined(veh_ptr->mid, SIGN_OUT, veh_ptr);
	 deauthorize(veh_ptr);
	 
	 writer_gps_state( veh_hndl, VEH_GPS_STATE_OFF );
	 return (SUCCESS);
      default:
	 return ((int) veh_ptr->t_status.inuse ? SUCCESS : FAIL);
      }				/* end case */
      break;

   case VEH_SUSPENDED_STATE:
/*****************************************************************/
/* VEH_SUSPENDED_STATE                                           */
/* GET ONLY: returns status - may call STATE_SET or STATE_RESET  */
/* STATE_SET (not directly callable)                             */
/*   OP: vehicle is suspened                                     */
/*   ARG: type of suspension (optional)                          */
/*   PRE: vehicle exists                                         */
/*        Veh_suspend called                                     */
/*        Veh_get_state(SUSPEND) called                          */
/*   POST: vehicle susp attributes set                           */
/*         fleet susp stats incremented                          */
/*         vehicle unabe to take calls as defined by arg         */
/*   ERROR: none                                                 */
/* STATE_RESET (Not directly callable)                           */
/*   OP: vehicle is nolonger suspended                           */
/*   ARG: none                                                   */
/*   PRE:  dirver exists                                         */
/*        Veh_suspend called                                     */
/*        Veh_get_state(SUSPEND) called                          */
/*   POST: vehicle susp attributes reset                         */
/*         fleet susp stats decremented                          */
/*         vehicle able to take calls                            */
/*  ERROR: none                                                  */
/*****************************************************************/

      switch (op)
      {
      case GET:
	(void)Fleet_get_hndl( BY_NBR, veh_ptr->fleet_nbr, &fleet_hndl );
	if ( fleet_hndl == HNDL_NULL )
	  return( FAIL );

	/* first clear any expired suspensions for the vehicle */
	 if (Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_PASS_LIST),veh_ptr->veh_nbr)
	     != HNDL_NULL )
	   {
	     if ( veh_ptr->veh_susp_to_date < mads_time )
	       Fleet_susp_veh_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_PASS_LIST),
					  (VEH_HNDL)veh_ptr);
	   }
	 if (Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_DELIV_LIST),veh_ptr->veh_nbr)
	     != HNDL_NULL )
	   {
	     if ( veh_ptr->veh_susp_d_to_date < mads_time )
		 Fleet_susp_veh_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_DELIV_LIST),
					    (VEH_HNDL)veh_ptr);
	   }
	 if (Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_TYPEX_LIST),veh_ptr->veh_nbr)
	     != HNDL_NULL )
	   {
	     if ( veh_ptr->veh_susp_x_to_date < mads_time )
		 Fleet_susp_veh_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_TYPEX_LIST),
					    (VEH_HNDL)veh_ptr);
	   }

	/* now check for suspensions */
	if (Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_PASS_LIST),veh_ptr->veh_nbr)
	    != HNDL_NULL )
	  {
	    if (Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_DELIV_LIST),veh_ptr->veh_nbr)
		!= HNDL_NULL )
	      {
		if (Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_TYPEX_LIST),veh_ptr->veh_nbr)
		    != HNDL_NULL )
		  return( SUCCESS );    /* vehicle suspended for all 3 call types */
	      }
	  }
	 return (FAIL);   /* vehicle allowed for at least one call type */

      default:
	 {
	    ERROR(' ', "", "VEH_SUSPEND_STATE illegal op");
	    return (FAIL);
	 }
      }				/* end case */
      break;


   case VEH_TALK_STATE:	/* was d_talk & except.c */
/***********************************************/
/* VEH_TALK_STATE                              */
/* STATE_SET                                   */
/*   OP: talk request processed                */
/*   ARG: none                                 */
/*   PRE: vehcle exists                        */
/*   POST: VEH_TALK_REQUEST made               */
/*         veh & fleet stats incremented       */
/*         veh history updated                 */
/*   ERROR: state already set                  */
/* STATE_RESET                                 */
/*   OP: process talk request response         */
/*   ARG: approved or disapproved              */
/*        veh exists                           */
/*   PRE: state set                            */
/*   POST: decrement fleet stats               */
/*         if approved msg GO_TO_VOICE send    */
/*         if disapproved GO_TO_VOICE_DEN sent */
/*   ERROR: message tansmission error          */
/***********************************************/

      switch (op)
      {
      case STATE_SET:
	if (veh_ptr->call_ptr != NULL)
	  mk_except((struct calls *) veh_ptr->call_ptr, veh_ptr, REQ_TO_TLK, 0, -1);
	else
	  mk_except((struct calls *) NULL, veh_ptr, REQ_TO_TLK, 0, -1);
	fleet[veh_ptr->fleet_nbr]->req_to_talk += 1;
	fleet[veh_ptr->fleet_nbr]->now_req_to_talk += 1;
	veh_ptr->req_to_talk += 1;	/* driver */
	
	if (veh_ptr->call_ptr != NULL)
	  {
	    if (veh_ptr->call_ptr->veh_ptr != veh_ptr)
	      {
		sprintf(error_str, "TALK error veh #%d call %d, veh1 %x",
			veh_ptr->veh_nbr,veh_ptr->call_ptr->call_number, veh_ptr->call_ptr->veh_ptr);
		ERROR(' ', "", error_str);
	    } else
	      Call_add_history((CALL_HNDL) veh_ptr->call_ptr,
			       (VEH_HNDL) veh_ptr, REQ_TO_TLK, NULL, 0L, 0);
	  }
	return (SUCCESS);

      case STATE_RESET:
	 --fleet[veh_ptr->fleet_nbr]->now_req_to_talk;
	 if ((short) argument == R_APPROVE)
	    send_text_predefined(veh_ptr->mid, GO_TO_VOICE, veh_ptr);
	 else			/* R_DISAPPROVE */
	    send_text_predefined(veh_ptr->mid, GO_TO_VOICE_DEN, veh_ptr);
	 return (SUCCESS);


      default:
	 ERROR(' ', "", "GET TALK STATE NOT IMPLEMENTED");
	 return (FAIL);
      }				/* end case */
      break;
     
   default:
     printf("ILLEGAL STATE FOR veh_state %d\n", state);
     return (FAIL);
   }				/* end case */
   return (SUCCESS);
}				/* end veh_state */


RET_STATUS
Veh_set_state(veh_hndl, state, argument)
   VEH_HNDL veh_hndl;
   int   state;
   HNDL  argument;
{
  if (veh_hndl == HNDL_NULL)
    return (FALSE);
  
  return ((RET_STATUS) veh_state(veh_hndl, STATE_SET, state, argument, HNDL_NULL));
}				/* end Veh_set_state */

RET_STATUS
Veh_reset_state(veh_hndl, state, argument, argument2)
   VEH_HNDL veh_hndl;
   int   state;
   HNDL  argument;
   HNDL  argument2;
{
  if (veh_hndl == HNDL_NULL)
    return (FALSE);
  
  return ((RET_STATUS) veh_state(veh_hndl, STATE_RESET, state, argument, argument2));
}				/* end Veh_reset_state */

int
Veh_get_state(veh_hndl, state)
   VEH_HNDL veh_hndl;
   int   state;
{
   int   rc;

   if (veh_hndl == HNDL_NULL)
     return (FALSE);
   
   rc = veh_state(veh_hndl, GET, state, NULL, NULL);

   if (rc == SUCCESS)
      return (TRUE);
   else
      return (FALSE);
}				/* end Veh_set_state */

/*******************************************************************************
* Veh_maint - Update the veh_driv structure in memory.
******************************************************************************/
Veh_maint(rec_num, veh_nbr, fleet_id)
   int   rec_num;
   int   veh_nbr;
   char  fleet_id;
{
   int   fl_nbr;
   short tmp_rf_id;
   struct veh_driv *veh_ptr;	/* pointer to a vehicle/driver record */
   struct veh_driv *pVehicles;
   VEH_REC *vh_ptr;		/* pointer to a cisam vehicle record */
   DRIVER_REC *dr_ptr;		/* pointer to a cisam driver record */
   unsigned char mask;
   unsigned char *addr;							/* used as a pointer to a bit structure */   
   unsigned char *now_bit_ptr;	/* pointer to the now bit structure */
   unsigned char *veh_bit_ptr;	/* pointer to the veh bit structure */
   register i, j, rfidx, vhidx;
   FLEET_HNDL fleet_hndl;
   FLEET_VEH_NBR_LIST_HNDL fleet_veh_nbr_list;
   FLEET_DRIVER_VEH_LIST_HNDL fleet_driver_list;
   char  error_str[80];

   
#ifdef DEBUG
   if (offset->prattle >= LEVEL3)
   {
      sprintf(trace_str, "veh_maint : rec_num = %d  veh_nbr = %d fleet = %c \n", rec_num, veh_nbr, fleet_id);
      TRACE(trace_str);
   }
#endif

   vh_ptr = (VEH_REC *) req_buf;

   if (Fleet_get_hndl(BY_ID, fleet_id, &fleet_hndl) == FAIL)
   {
      sprintf(error_str, "Bad fleet id='%c'. Vehicle=%d", fleet_id, veh_nbr);
      ERROR(fleet_id, VEHICLE_FILE, error_str);
      return;
   }
   fleet_veh_nbr_list = (FLEET_VEH_NBR_LIST_HNDL) Fleet_get_value(fleet_hndl, FLEET_VEH_NBR_LIST);
   fl_nbr = (short) Fleet_get_value(fleet_hndl, FLEET_NBR);
   (void) Veh_get_hndl(BY_NBR, veh_nbr, fl_nbr, (VEH_HNDL *) & veh_ptr);

   vh_ptr->fleet = fleet_id;
   vh_ptr->nbr = veh_nbr;
   if (db(VEHICLE_FILE_ID, READ_KEYS, &veh_key2, ISEQUAL, 0) < 0)
   {				/* read record */
      if (iserrno == ENOREC)
      {
	 if ((veh_ptr = (struct veh_driv *) Fleet_veh_nbr_list_find(fleet_veh_nbr_list, vh_ptr->nbr)) == NULL)
	 {
	    sprintf(error_str, "veh_maint - can't find vehicle %d\n", vh_ptr->nbr);
	    ERROR(fleet_id, VEHICLE_FILE, error_str);
	    return;
	 } else
	 {
	    if (veh_ptr == NULL)
	    {
	       ERROR(fleet_id, VEHICLE_FILE, "veh_maint - NULL veh_ptr");
	       return;
	    }
	    if ((veh_ptr = (struct veh_driv *) Fleet_veh_id_list_find(veh_ptr->rf_id)) == NULL)
	    {
	       sprintf(error_str, "veh_maint - can't find vehicle %d\n", veh_nbr);
	       ERROR(fleet_id, VEHICLE_FILE, error_str);
	       return;
	    }
	    /* This is nuts, but we should say something anyway. */
	    if (veh_ptr->t_status.offered || veh_ptr->t_status.accepted)
	    {
	       sprintf(error_str, "veh #%d db record deleted while either offered or accepted",
		       veh_ptr->veh_nbr);
	       ERROR(fleet_id, VEHICLE_FILE, error_str);
	    }
	    /*
	     * Make sure he is at least removed from zone lists and timers
	     * are cleaned up.
	     */
	    if (veh_ptr->t_status.posted)
	       (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL);
	    if (veh_ptr->t_status.e_posted)
	       (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_EPOSTED_STATE, HNDL_NULL, HNDL_NULL);

	    /* Blow the car away. */
	    if (Fleet_veh_nbr_list_remove(fleet_veh_nbr_list, (VEH_HNDL) veh_ptr) == FAIL)
            {
                 sprintf(error_str,"veh_nbr remove failure veh %d", veh_ptr->veh_nbr);
                 ERROR(' ',"",error_str);
            }
	    if (Fleet_veh_id_list_remove((VEH_HNDL) veh_ptr) == FAIL)
            {
                 sprintf(error_str,"veh_id remove failure veh %d", veh_ptr->veh_nbr);
                 ERROR(' ',"",error_str);
            }
            if (SIGNED_ON(veh_ptr))
            {
                 fleet_driver_list = (FLEET_DRIVER_VEH_LIST_HNDL) Fleet_get_value(fleet_hndl, FLEET_DRIVER_VEH_LIST);
                 if(Fleet_driver_veh_list_remove(fleet_driver_list, (VEH_HNDL) veh_ptr) == FAIL)
                 {
                    sprintf(error_str,"remove failure driver %d veh %d list %x",
                                   veh_ptr->driver_id,  veh_ptr->veh_nbr, fleet_driver_list);
                     ERROR(veh_ptr->fleet,"", error_str);
                 }

            }
	    bzero( veh_ptr, sizeof( struct veh_driv ) );
	    --veh_used;		/* save his spot to be used later */
	    offset->veh_used = veh_used;

	    return;
	 }
      } else
      {
	 sprintf(error_str, "veh_maint: C-isam error %d", iserrno);
	 ERROR(fleet_id, VEHICLE_FILE, error_str);
	 return;
      }
   }
   if (veh_ptr == NULL)
   {				/* if vehicle doesn't exist */
      if ((veh_ptr = (struct veh_driv *) malloc(sizeof(struct veh_driv))) == NULL)
      {
	 ERROR(' ', "", "build_veh_driv - couldn't allocate memory for vehicle.");
	 return;
      }     
      bzero( veh_ptr, sizeof( struct veh_driv ) );
      up_veh_idx(veh_ptr);	/* make sure nothing hoses up req_buf */

#ifndef	RANDOM			/* If RANDOM is defined, these tasks are done
				 * in up_veh_idx() for us */
      /* Add the new vehicle to the fleet lists. */
      if (Fleet_veh_id_list_add((VEH_HNDL) veh_ptr) != SUCCESS)
      {
	 sprintf(error_str, "Veh_maint - Fleet_veh_id_list_add() for fleet %c vehicle #%d failed!\n",
		 veh_ptr->fleet, veh_ptr->veh_nbr);
	 ERROR(fleet_id, VEHICLE_FILE, error_str);
      }
      if (Fleet_veh_nbr_list_add(fleet_veh_nbr_list, (VEH_HNDL) veh_ptr) != SUCCESS)
      {
	 sprintf(error_str, "Veh_maint - Fleet_veh_nbr_list_add() for fleet %c vehicle #%d failed!\n",
		 veh_ptr->fleet, veh_ptr->veh_nbr);
	 ERROR(fleet_id, VEHICLE_FILE, error_str);
      }
#endif
   } else
   {				/* vehicle exists */
      
     tmp_rf_id = (unsigned long) strtol(vh_ptr->mobile_rf_id, (char **) 0, 16);
     if (tmp_rf_id != veh_ptr->rf_id)
       {
	 if ((veh_ptr = (struct veh_driv *) Fleet_veh_id_list_find(veh_ptr->rf_id)) == NULL)
	   {
	     sprintf(error_str, "veh_maint - can't find vehicle %d\n", vh_ptr->nbr);
	     ERROR(fleet_id, VEHICLE_FILE, error_str);
	     return;
	   }
	 
	 Fleet_veh_id_list_remove((VEH_HNDL) veh_ptr);
	 Veh_rec_2_obj((VEH *)veh_ptr, (VEH_REC *)vh_ptr);
	 
	 Fleet_veh_id_list_add((VEH_HNDL) veh_ptr);	/* with new mid */
     } else
	 Veh_rec_2_obj(veh_ptr, vh_ptr);


     if ( veh_ptr->t_status.inuse )
       {
	 dr_ptr = (DRIVER_REC *) req_buf;
	 dr_ptr->fleet = fleet_id;
	 dr_ptr->id = veh_ptr->driver_id;
	 
	 if (db(DRIVER_FILE_ID, READ_KEYS, &dr_key1, ISEQUAL, 0) < 0)
	   {			/* read record */
	     ERROR(fleet_id, DRIVER_FILE, "veh_maint - cannot find record.\n");
	     return;
	   }
	 now_bit_ptr = (unsigned char *) &veh_ptr->now_seals;	/* set pointer to the
								 * structure address */
	 *now_bit_ptr = 0;
	 veh_bit_ptr = (unsigned char *) &veh_ptr->vehicle_seals;	/* set pointer to the
									 * structure address */
	 
	 for (i = 0; i < 2; i++)
	   {			/* do it for two loops */
	     mask = 0x80;	/* set the bit mask */
	     for (j = 0; j <= 7; j++)
	       {			/* read high bit to low */
		 if ((dr_ptr->seals[j + (i * 8)] == YES) && veh_bit_ptr)	/* check if bit should
										 * be set */
		   *now_bit_ptr |= mask;	/* set the bit if YES */
		 mask >>= 1;	/* shift the mask for next attribute */
	       }
	     ++now_bit_ptr;	/* point to the next 8 bits */
	     *now_bit_ptr = 0;
	     ++veh_bit_ptr;	/* point to the next 8 bits */
	   }
	 
       }
   }
 }



RET_STATUS
Veh_warm_start_call_close(veh_hndl, call_rec_ptr)
   VEH_HNDL veh_hndl;
   CALL_REC *call_rec_ptr;
{
   VEH  *veh_ptr;

   veh_ptr = (VEH *) veh_hndl;

   veh_ptr->last_call_handled = call_rec_ptr->nbr;
   veh_ptr->t_status.meter_on = 0;
   if (veh_ptr->t_status.e_posted)
      (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_EPOSTED_STATE, (HNDL) veh_ptr->zone_num, HNDL_NULL);
   (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_CALL_STATE, HNDL_NULL, HNDL_NULL);
   return (SUCCESS);

}				/* end Veh_warm_start_call_close */

RET_STATUS
Veh_warm_start_epost(veh_hndl, zone_nbr)
   VEH_HNDL veh_hndl;
   short zone_nbr;
{
    VEH  *veh_ptr;

    veh_ptr = (VEH *) veh_hndl;
    switch (fleet[veh_ptr->fleet_nbr]->when_enroute_post)
    {
    case FLEET_EPOST_METER_ON_AND_SYSTEM_CALL:
           /* Veh_set_state((VEH_HNDL) veh_ptr, VEH_ACCEPT_STATE, HNDL_NULL); */
           veh_ptr->t_status.accepted = 1;

     case FLEET_EPOST_METER_ON:
           veh_ptr->t_status.idle     = 0;
           veh_ptr->t_status.meter_on = 1;
           veh_ptr->t_status.pckup    = 1;
           /* Veh_set_state((VEH_HNDL) veh_ptr, VEH_IDLE_STATE, HNDL_NULL); */
           /* Veh_set_state((VEH_HNDL) veh_ptr, VEH_METER_ON_STATE, HNDL_NULL); */
           /* Veh_set_state((VEH_HNDL) veh_ptr, VEH_PICKUP_STATE, HNDL_NULL); */
           break;

     case FLEET_EPOST_ALWAYS:
		/* do not know meter state */
    default :
           break;
    }
    Veh_set_state((VEH_HNDL) veh_ptr, VEH_EPOSTED_STATE, (HNDL) zone_nbr);
    return(SUCCESS);
}           /* end Veh_warm_start_epost */


/**
  Checks for Future suspensions by reading the database vehicle record.

  Returns SUCCESS if future suspension was activated
  Returns FAIL    if no suspension was activated
  
**/
RET_STATUS
Veh_check_suspensions( veh_hndl )
     VEH_HNDL veh_hndl;
{
  VEH_REC  veh_rec;
  FLEET_HNDL  fleet_hndl;
  struct veh_driv *veh_ptr;
  char         susp_msg[1024];  
  BOOLEAN bSuspended_pass = FALSE, bSuspended_deliv = FALSE, bSuspended_typex = FALSE;
  
  if ( veh_hndl == HNDL_NULL )
    return(FAIL);

  veh_ptr = ( struct veh_driv * ) veh_hndl;

  if ( Fleet_get_hndl( BY_ID, veh_ptr->fleet, &fleet_hndl ) == FAIL )
    return(FAIL);
  
  bzero( &veh_rec, sizeof( VEH ) );
  veh_rec.fleet = (char)Veh_get_value( veh_hndl, VEH_FLEET );
  veh_rec.nbr = (short)Veh_get_value( veh_hndl, VEH_NBR );

  if ( db_read_key( VEHICLE_FILE_ID, &veh_rec, &veh_key2, ISEQUAL ) != SUCCESS )
    return( FAIL );


  /* Vehicle is suspended for Passenger calls */
  if ( ( veh_rec.susp_from_dt_tm <= mads_time ) && ( veh_rec.susp_to_dt_tm > mads_time ) )
    {
      /* first remove any instances in the list */
      Fleet_susp_veh_list_remove( (FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_PASS_LIST),
				  (VEH_HNDL)veh_hndl );

      /* now add the vehicle to the list */
      Fleet_susp_veh_list_add( (FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_PASS_LIST),
			       (VEH_HNDL) veh_hndl );

      /* set the in-memory suspension periods */
      veh_ptr->vehicle_attr.susp_pass_call = 1;
      veh_ptr->veh_susp_from_date = veh_rec.susp_from_dt_tm;
      veh_ptr->veh_susp_to_date = veh_rec.susp_to_dt_tm;

      /* send the messages to alert the driver */
      strcpy( susp_msg, MDT_MSG_VEH_SUSP);
      mk_outb_text( susp_msg );
      strcpy(susp_msg, MDT_MSG_PASS_TRIP);
      strcat(susp_msg, veh_rec.susp_to_date );
      strcat(susp_msg, " " );
      strcat(susp_msg, veh_rec.susp_to_time );
      add_outb_text( susp_msg );
      add_outb_text(CR);
      send_msg_mmp( veh_ptr->mid, TEXT_DISPLAY, veh_ptr );
      
      bSuspended_pass = TRUE;
    }
  else
    {
      Fleet_susp_veh_list_remove( (FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_PASS_LIST),
				  (VEH_HNDL)veh_hndl );
      veh_ptr->vehicle_attr.susp_pass_call = 0;
      veh_ptr->veh_susp_from_date = 0;
      veh_ptr->veh_susp_to_date = 0;
      bSuspended_pass = FALSE;
    }

  /* delivery calls */
  if ( ( veh_rec.susp_d_from_dt_tm <= mads_time ) && ( veh_rec.susp_d_to_dt_tm > mads_time ) )
    {
      /* first remove any instances in the list */
      Fleet_susp_veh_list_remove( (FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_DELIV_LIST),
				  (VEH_HNDL)veh_hndl );

      /* now add the vehicle to the list */
      Fleet_susp_veh_list_add( (FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_DELIV_LIST),
			       (VEH_HNDL) veh_hndl );

      /* set the in-memory suspension periods */
      veh_ptr->vehicle_attr.susp_del_call = 1;
      veh_ptr->veh_susp_d_from_date = veh_rec.susp_d_from_dt_tm;
      veh_ptr->veh_susp_d_to_date = veh_rec.susp_d_to_dt_tm;

      /* send the messages to alert the driver */
      strcpy( susp_msg, MDT_MSG_VEH_SUSP);
      mk_outb_text( susp_msg );
      strcpy(susp_msg, MDT_MSG_DELIV_TRIP);
      strcat(susp_msg, veh_rec.susp_d_to_date );
      strcat(susp_msg, " " );
      strcat(susp_msg, veh_rec.susp_d_to_time );
      add_outb_text( susp_msg );
      add_outb_text(CR);
      send_msg_mmp( veh_ptr->mid, TEXT_DISPLAY, veh_ptr );       

      bSuspended_deliv = TRUE;
    }
  else
    {
      Fleet_susp_veh_list_remove( (FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_DELIV_LIST),
				  (VEH_HNDL)veh_hndl );
      veh_ptr->vehicle_attr.susp_del_call = 0;
      veh_ptr->veh_susp_d_from_date = 0;
      veh_ptr->veh_susp_d_to_date = 0;
      bSuspended_deliv = FALSE;
    }

  /* typex calls */
  if ( ( veh_rec.susp_x_from_dt_tm <= mads_time ) && ( veh_rec.susp_x_to_dt_tm > mads_time ) )
    {
      /* first remove any instances in the list */
      Fleet_susp_veh_list_remove( (FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_TYPEX_LIST),
				  (VEH_HNDL)veh_hndl );

      /* now add the vehicle to the list */
      Fleet_susp_veh_list_add( (FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_TYPEX_LIST),
			       (VEH_HNDL) veh_hndl );

      /* set the in-memory suspension periods */
      veh_ptr->vehicle_attr.susp_typex_call = 1;
      veh_ptr->veh_susp_x_from_date = veh_rec.susp_x_from_dt_tm;
      veh_ptr->veh_susp_x_to_date = veh_rec.susp_x_to_dt_tm;

      /* send the messages to alert the driver */
      strcpy( susp_msg, MDT_MSG_VEH_SUSP);
      mk_outb_text( susp_msg );
      strcpy(susp_msg, MDT_MSG_TYPEX_TRIP);
      strcat(susp_msg, veh_rec.susp_x_to_date );
      strcat(susp_msg, " " );
      strcat(susp_msg, veh_rec.susp_x_to_time );
      add_outb_text( susp_msg );
      add_outb_text(CR);
      send_msg_mmp( veh_ptr->mid, TEXT_DISPLAY, veh_ptr );       

      bSuspended_typex = TRUE;
    }
  else
    {
      Fleet_susp_veh_list_remove( (FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_TYPEX_LIST),
				  (VEH_HNDL)veh_hndl );
      veh_ptr->vehicle_attr.susp_typex_call = 0;
      veh_ptr->veh_susp_x_from_date = 0;
      veh_ptr->veh_susp_x_to_date = 0;
      bSuspended_typex = FALSE;
    }    

  if ( ( bSuspended_pass == TRUE ) && ( bSuspended_deliv == TRUE ) && ( bSuspended_typex == TRUE ) )
    {
      if ( Veh_get_state( veh_hndl, VEH_POSTED_STATE ) )
	Veh_reset_state( veh_hndl, VEH_POSTED_STATE, HNDL_NULL, HNDL_NULL );
      return( SUCCESS );
    }
  else
    return( FAIL );
}
      
BOOLEAN Vehicle_is_Valid_Shift( VEH_HNDL veh_hndl, CALL_HNDL call_hndl )
{
  struct cisam_sh s_sched;
  struct cisam_ss s_shift;
  struct cisam_s_version sv;
  struct veh_driv *veh_ptr;
  int mode = 0;
  CALL *theCall;

  theCall = (CALL *)call_hndl;
  if ( ( theCall != NULL )  && ( theCall->FTJ_nbr == 1 ) )
    return (TRUE);

  veh_ptr = (struct veh_driv *)veh_hndl;
  bzero(&sv, sizeof(struct cisam_s_version));
  sv.vers_id = 0;
  mode = ISFIRST;
  while ( db_read_key( S_VERSION_FILE_ID, &sv, &sv_key1, mode ) == SUCCESS )
    {
      mode = ISNEXT;
      if ( sv.active == YES )
	break;
    }
  
  if ( sv.active == YES ) // Found a valid version record
    {
      if ( !Time_in_Shift( (int)veh_ptr->schedule_num,
			   (char)fleet[veh_ptr->fleet_nbr]->fleet_id,
			   sv.vers_id ) )
	{
	  return(FALSE);
	}
    }
  
  return(TRUE);
									      
}

BOOLEAN Vehicle_is_Valid_Shift2( VEH_HNDL veh_hndl )
{
  struct cisam_sh s_sched;
  struct cisam_ss s_shift;
  struct cisam_s_version sv;
  struct veh_driv *veh_ptr;
  int mode = 0;

  veh_ptr = (struct veh_driv *)veh_hndl;
  bzero(&sv, sizeof(struct cisam_s_version));
  sv.vers_id = 0;
  mode = ISFIRST;
  while ( db_read_key( S_VERSION_FILE_ID, &sv, &sv_key1, mode ) == SUCCESS )
    {
      mode = ISNEXT;
      if ( sv.active == YES )
	break;
    }
  
  if ( sv.active == YES ) // Found a valid version record
    {
      if ( !Time_in_Shift( (int)veh_ptr->schedule_num,
			   (char)fleet[veh_ptr->fleet_nbr]->fleet_id,
			   sv.vers_id ) )
	{
	  return(FALSE);
	}
    }
  
  return(TRUE);
									      
}

  
