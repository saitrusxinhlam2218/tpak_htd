/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Vehicle.c,v $
*  @(#)  $Revision: 1.19 $
*  @(#)  $Date: 2004/05/12 12:56:39 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Vehicle.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Vehicle.c,v 1.19 2004/05/12 12:56:39 jwelch Exp $";

/* Vehicle.c                                                               */
#include <sys/types.h>		/* temp */
#include <sys/times.h>		/* temp */
#include <sys/ipc.h>
#include "mad_error.h"
#include "taxipak.h"
#include "Object.h"
#include "Call_public.h"
#include "Call_private.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "List_public.h"
#include "Vehicle_lists_public.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Fleet_lists_public.h"
#include "Zone_public.h"
#include "Vehicle_private.h"
#include "Timer_public.h"
#include "dispatch_strdefs.h"
#include "mad_ipc.h"
#include "mads_isam.h"
#include "key.h"
#include "df_maint.h"
#include "ui_ipc.h"
#include "msg_mmp.h"
#include "f_msg_defs.h"
#include "except.h"
#include "enhance.h"
#include "switch_ext.h"
#include "kdt.h"

extern FLEET *fleet[ ];	/* shared memory fleet structure */
extern struct offsets *offset;
extern time_t mads_time;
extern int dis_msgqid;
static void veh_cvt_to_flag();

/*************************************************************/
/* RET_STATUS Veh_send_msg_text(veh_hndl, control, msg_type) */
/* OP: send a message to vehicle                             */
/* PRE: vehicle exists                                       */
/* POST: formated message sent to vehicle                    */
/* ERROR: message not sent                                   */
/*************************************************************/

RET_STATUS
Veh_send_msg_text(veh_hndl, control, message)
   VEH_HNDL veh_hndl;
   char *control;
   char *message;
{
   mk_outb_text(NULL);
   add_outb_text(message);
   if (send_msg_mmp(((VEH *) veh_hndl)->mid, control, (VEH *) veh_hndl) != SUCCESS)
      return (VEH_MESSAGE_NOT_SENT);
   else
      return (SUCCESS);
}

/************************************************************/
/* RET_STATUS Veh_get_hndl(from, value, fleet_nbr hndl_ptr) */
/* OP: get a vehicle handle from mid or veh_nbr             */
/* PRE: veh_id_list and veh_by_nbr exist                    */
/* POST:  veh_hndl returned                                 */
/* ERROR: no vehicle (HNDL_ERR)                             */
/************************************************************/

RET_STATUS
Veh_get_hndl(
	     int from,
	     int value,
	     short fleet_nbr,
	     VEH_HNDL * hndl_ptr)
{
   FLEET_HNDL fleet_hndl;
   FLEET_VEH_NBR_LIST_HNDL fleet_veh_nbr_list;

   if (fleet_nbr < 0)
   {
      *hndl_ptr = HNDL_NULL;
      return (FAIL);
   }
   switch (from)
   {
   case BY_MID:
      *hndl_ptr = Fleet_veh_id_list_find(value);
      break;
   case BY_NBR:
      (void) Fleet_get_hndl(BY_NBR, fleet_nbr, &fleet_hndl);
      fleet_veh_nbr_list = (FLEET_VEH_NBR_LIST_HNDL) Fleet_get_value(fleet_hndl, FLEET_VEH_NBR_LIST);
      *hndl_ptr = Fleet_veh_nbr_list_find(fleet_veh_nbr_list, value);
      break;
   default:
      *hndl_ptr = HNDL_NULL;
      break;
   }
   if (*hndl_ptr == HNDL_NULL)
      return (FAIL);
   else
      return (SUCCESS);
}				/* end Veh_get_hndl) */

/*********************************************************************************/
/* int Veh_check_attr(veh_hndl, call_hndl)                                       */
/* OP: checks vehicle  & driverd attributes to see if ok to distapch             */
/* PRE: call & veh exist - driver signed on                                      */
/* POST: FALSE returned if vehicle had rejected call                             */
/*       FALSE returned if vehicle suspended                                     */
/*       FALSE returned if vehicle_attributes dont match call_vehicle_attributes */
/*       otherwise TRUE returned                                                 */
/* ERROR: none                                                                   */
/*********************************************************************************/

int
Veh_check_attr(veh_hndl, call_hndl, cbook)	/* WAS check_attr(call_ptr,veh_ptr) */
   VEH_HNDL veh_hndl;
   CALL_HNDL call_hndl;
   BOOLEAN cbook;
{
   CALL_REJECT_VEH_LIST_HNDL call_reject_veh_list;
   FLEET_HNDL   fleet_hndl;
   struct calls *call_ptr;
   struct veh_driv *veh_ptr;

   call_ptr = (struct calls *) call_hndl;
   veh_ptr = (struct veh_driv *) veh_hndl;

#ifdef DEBUG
   if (offset->prattle >= LEVEL5)
   {
      sprintf(trace_str, "Veh_check_attr : veh_ptr = %x call_ptr = %x\n", veh_ptr, call_ptr);
      TRACE(trace_str);
   }
#endif

   Fleet_get_hndl( BY_ID, veh_ptr->fleet, &fleet_hndl );
   
   call_reject_veh_list = (CALL_REJECT_VEH_LIST_HNDL) Call_get_value((CALL_HNDL) call_ptr,
						      CALL_REJECT_VEH_LIST);
   if ( (call_reject_veh_list != NULL) && ( !cbook ) )
     if (Call_reject_veh_list_find(call_reject_veh_list, (short)Veh_get_value((VEH_HNDL)veh_ptr, VEH_NBR)))
	 return (FALSE);

   if (veh_ptr->driver_class == ROOKIE)
   {				/* check if too hot for rookie to handle */
      if (call_ptr->priority < fleet[veh_ptr->fleet_nbr]->highest_rookie_pri)
	 return (FALSE);
   }
   /* make sure not suspended from this type of call */
   if ((call_ptr->call_type.passenger) &&
       (Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_PASS_LIST),
					veh_ptr->veh_nbr)))
    {
	if ( veh_ptr->veh_susp_to_date < mads_time )
	  {
	    Fleet_susp_veh_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_PASS_LIST),
				       (VEH_HNDL)veh_ptr);

	    if (Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_DELIV_LIST),
					 veh_ptr->veh_nbr))
	      if ( veh_ptr->veh_susp_d_to_date < mads_time )
		Fleet_susp_veh_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_DELIV_LIST),
					   (VEH_HNDL)veh_ptr);

	    if (Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_TYPEX_LIST),
					 veh_ptr->veh_nbr))
	      if ( veh_ptr->veh_susp_x_to_date < mads_time )
		Fleet_susp_veh_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_TYPEX_LIST),
					   (VEH_HNDL)veh_ptr);	    
	  }
	else
	     return (FALSE);
    }

   if ((call_ptr->call_type.delivery) &&
       (Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_DELIV_LIST),
					veh_ptr->veh_nbr)))
	{
		if ( veh_ptr->veh_susp_d_to_date < mads_time )
		  {
		    Fleet_susp_veh_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_DELIV_LIST),
					       (VEH_HNDL)veh_ptr);
		    if (Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_PASS_LIST),
						 veh_ptr->veh_nbr))
		      if ( veh_ptr->veh_susp_to_date < mads_time )
			Fleet_susp_veh_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_PASS_LIST),
						   (VEH_HNDL)veh_ptr);
		    
		    if (Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_TYPEX_LIST),
						 veh_ptr->veh_nbr))
		      if ( veh_ptr->veh_susp_x_to_date < mads_time )
			Fleet_susp_veh_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_TYPEX_LIST),
						   (VEH_HNDL)veh_ptr);
		  }
		else
		  return (FALSE);
	}

   if ((call_ptr->call_type.xtype) &&
       (Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_TYPEX_LIST),
					veh_ptr->veh_nbr)))
	{
		if ( veh_ptr->veh_susp_x_to_date < mads_time )
		  {
		    Fleet_susp_veh_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_VEH_SUSP_TYPEX_LIST),
					       (VEH_HNDL)veh_ptr);

		    if (Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_DELIV_LIST),
						 veh_ptr->veh_nbr))
		      if ( veh_ptr->veh_susp_d_to_date < mads_time )
			Fleet_susp_veh_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_DELIV_LIST),
						   (VEH_HNDL)veh_ptr);

		    if (Fleet_susp_veh_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_PASS_LIST),
						 veh_ptr->veh_nbr))
		      if ( veh_ptr->veh_susp_to_date < mads_time )
			Fleet_susp_veh_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_VEH_SUSP_PASS_LIST),
						   (VEH_HNDL)veh_ptr);		    
		    
		  }
		else
		     return (FALSE);
	}

   if ((call_ptr->call_type.passenger) &&
       (Fleet_susp_drv_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_DRV_SUSP_PASS_LIST),
					veh_ptr->driver_id)))
     {
	if ( veh_ptr->drv_susp_to_date < mads_time )
		Fleet_susp_drv_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_DRV_SUSP_PASS_LIST),
					   (VEH_HNDL)veh_ptr);
	else
                return (FALSE);
      }
   if ((call_ptr->call_type.delivery) &&
       (Fleet_susp_drv_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_DRV_SUSP_DELIV_LIST),
					veh_ptr->driver_id)))
	{
		if ( veh_ptr->drv_susp_d_to_date < mads_time )
			Fleet_susp_drv_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_DRV_SUSP_DELIV_LIST),
						(VEH_HNDL)veh_ptr);
		else
		     return (FALSE);
	}
   if ((call_ptr->call_type.xtype) &&
       (Fleet_susp_drv_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_DRV_SUSP_TYPEX_LIST),
					veh_ptr->driver_id)))
	{
		if ( veh_ptr->drv_susp_x_to_date < mads_time )
			Fleet_susp_drv_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl,FLEET_DRV_SUSP_TYPEX_LIST),
						(VEH_HNDL)veh_ptr);
		else
		     return (FALSE);   
	}


   if ( !Vehicle_match_attr( (unsigned char *)&veh_ptr->vehicle_attr, (unsigned char *)&call_ptr->vehicle_attributes ) )
     return (FALSE);

   if ( Vehicle_match_attr( (unsigned char *)&veh_ptr->driver_attr, (unsigned char *)&call_ptr->driver_attributes ) )
     return (TRUE);   

   return (FALSE);		/* default attributes do not match */
}				/* end Veh_check_attr */

/*************************************************/
/* int Veh_check_seals(veh_hndl, zone_nbr)       */
/* OP: compares zone and vehicles for identity   */
/* PRE: vehicle exists                           */
/* POST:  TRUE returned  if FLEET_SUPPRESS_SEALS */
/*        TRUE returned  if seals match          */
/*        otherwise FALSE returned               */
/* ERROR: no zone hndl                           */
/*************************************************/

int
Veh_check_seals(		/* WAS check_seals */
		VEH_HNDL veh_hndl,
		short zone_nbr)
{
   unsigned char *veh_bit_ptr;	/* pointer to the vehicle bit fields */
   unsigned char *zone_bit_ptr;	/* pointer to the zone bit fields */
   unsigned char result;	/* result of bit operations  */
   char  error_str[80];
   struct veh_driv *veh_ptr;
   ZONE_HNDL zone_hndl;

   veh_ptr = (struct veh_driv *) veh_hndl;
#ifdef DEBUG
   if (offset->prattle >= LEVEL5)
   {
      sprintf(trace_str, "check_seals : veh_ptr = %x zone = %d \n", veh_ptr, zone_nbr);
      TRACE(trace_str);
   }
#endif

   if (fleet[veh_ptr->fleet_nbr]->criteria.suppress_seals)	/* no need to check
								 * seals */
      return (TRUE);

   veh_bit_ptr = (unsigned char *) &veh_ptr->now_seals;	/* start of vehicle seal
							 * bits */
   if (Zone_get_hndl(zone_nbr, veh_ptr->fleet_nbr, &zone_hndl) == FAIL)
   {
      sprintf(error_str, "No hndle for zone %d", zone_nbr);
      ERROR(' ', "", error_str);
      return (FALSE);
   }
   zone_bit_ptr = (unsigned char *) Zone_get_value(zone_hndl, ZONE_SEALS);
   if (*zone_bit_ptr == (*veh_bit_ptr & *zone_bit_ptr))
   {				/* check if first 8 bits match */
      ++veh_bit_ptr;		/* point to next 8 bits */
      ++zone_bit_ptr;		/* point to next 8 bits */
      if (*zone_bit_ptr == (*veh_bit_ptr & *zone_bit_ptr))	/* check if next 8 bits
								 * match */
	 return (TRUE);		/* return ok */
   }
   return (FALSE);		/* default - seals don't match */
}				/* end Veh_check_seals */

/*******************************************/
/* RET_STATUS Veh_services_start()         */
/* OP: initial set up for vehilce services */
/* PRE: List_services_start invoked        */
/* POST:                                   */
/* ERROR: none                             */
/* change doc */
/*******************************************/

RET_STATUS
Veh_services_start()
{
   ERROR(' ', "", "Veh_services_start not supported");
   return(FAIL);
}				/* end Veh_services_start */

/*******************************************/
/* RET_STATUS Veh_services_stop()          */
/* OP: initial set up for vehilce services */
/* PRE: Veh_services_start invoked         */
/* POST:  */
/*        */
/* ERROR: none                             */
/*******************************************/

RET_STATUS
Veh_services_stop()
{
   ERROR(' ', "", "Veh_services_stop not supported");
   return(FAIL);
}				/* end Veh_services_stop */

/******************************************************/
/* RET_STATUS Veh_suspend(veh_hndl, attr_list)        */
/* OP: modify  the vehicle suspend data               */
/* PRE: vehicle exists                                */
/* POST:  vehicle suspend values set                  */
/*        db updated                                  */
/* ERROR: db access error                             */
/******************************************************/

RET_STATUS
Veh_suspend(veh_hndl, attr_list)
   VEH_HNDL veh_hndl;
    /* ATTR_LIST */ int attr_list;
{
   ERROR(' ', "", "Veh_suspend not supported");
   return(FAIL);
}				/* end Veh_suspend */

/**********************************************************/
/* RET_STATUS Veh_call_cancel(veh_hndl, call_hndl)        */
/* OP: call canceled reset vehicle                        */
/* PRE: veh exists                                        */
/*      call exists                                       */
/* POST:                                                  */
/*       in need of update */
/*      if VEH_CALL_HNDL  == CALL_HNDL                    */
/*         !VEH_NO_SHOW_STATE                             */
/*         if VEH_EPOSTED_STATE                           */
/*            EPOST timer deleted                         */
/*         !VEH_NOSHOW_STATE                              */
/*         VEH_PRIORITY = FLEET_DRV_CANCEL_PRIORITY       */
/*         if VEH_LATE_METER_STATE                        */
/*            !VEH_LATE_METER_STATE                       */
/*            fleet now_late meter stats decremented      */
/*         if VEH_OFFERED_STATE && !VEH_ACCEPTED_STATE    */
/*            !VEH_OFFERED_STATE                          */
/*            VEH_CALL_NBR = 0                            */
/*            VEH_CALL_HNDL = HNDL_ERR                    */
/*         else                                           */
/*            VEH_ON_CALL_STATE reset                     */
/*         if FLEET_CANCEL_ACTION == SEMI or AUTO         */
/*            VEH_POST_STATE called for CALL_PICKUP_ZONE  */
/*                                                        */
/*     Call_send_cancel called                            */
/* ERROR:                                                 */
/**********************************************************/

 /* WAS drv_cancel */
RET_STATUS
Veh_call_cancel(veh_hndl, call_hndl)
   VEH_HNDL veh_hndl;
   CALL_HNDL call_hndl;
{
   struct veh_driv *veh_ptr;
   struct calls *call_ptr;
   short	save_zone = 0;

   if (veh_hndl == HNDL_NULL)
      return (FAIL);

   veh_ptr = (struct veh_driv *) veh_hndl;
   call_ptr = (struct calls *) call_hndl;
   if (veh_ptr->call_ptr != call_ptr)
   {
      if (veh_ptr->call_ptr != NULL)
         return(FAIL);    /* probably completed this call */
      else if ((veh_ptr->call_ptr == NULL) && (veh_ptr->t_status.offered == 0))
               return(FAIL);
   }

   if (veh_ptr != NULL)
     save_zone = veh_ptr->zone_num;

   /* send even if only offrered */
   if (veh_ptr->t_status.noshow)
      Veh_reset_state((VEH_HNDL) veh_ptr, VEH_NOSHOW_STATE, (HNDL) R_APPROVE, HNDL_NULL);
   else
      Call_send_cancel(call_hndl, veh_hndl);

   if (veh_ptr->call_ptr == (struct calls *) call_hndl)
   {
      /* Remove any EPOST timers for the vehicle */
      if (veh_ptr->t_status.e_posted)
      {

	 /* Next three lines ensure vehicle state is properly reset when ebooked vehicle has */
	 /* call cancelled.  Action should be to book vehicle into zone of the */
	 /* call that was cancelled based on cancel priority in fleet file     */
	 if (veh_ptr->t_status.meter_on)
	 {
	     Veh_reset_state((VEH_HNDL) veh_ptr, VEH_EPOSTED_STATE, HNDL_NULL, HNDL_NULL);
	     Veh_set_state((VEH_HNDL) veh_ptr, VEH_IDLE_STATE, HNDL_NULL);
	 }


	 if (veh_ptr->veh_epost_timer != NULL)
	 {
	    Timer_destroy((TIMER_HNDL) veh_ptr->veh_epost_timer);
	    veh_ptr->veh_epost_timer = NULL;
	 }
      }
      if (veh_ptr->t_status.accepted)
	veh_ptr->priority = fleet[veh_ptr->fleet_nbr]->drv_cncl_pri;
      
      if (veh_ptr->t_status.late_meter)
      {				/* did he meter on late */
	 --fleet[veh_ptr->fleet_nbr]->now_late_meters;
	 veh_ptr->t_status.late_meter = 0;
      }

      /** use VEH_CALL_OFFER_STATE here instead??? **/
      if (veh_ptr->t_status.offered && !veh_ptr->t_status.accepted)
      {
	 veh_ptr->call_ptr = NULL;
	 veh_ptr->call_nbr = 0;
	 veh_ptr->t_status.offered = 0;
      } else
      {
	Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_CALL_STATE, HNDL_ERR, HNDL_NULL);
      }

      if ((fleet[veh_ptr->fleet_nbr]->cancel_action == '2' || fleet[veh_ptr->fleet_nbr]->cancel_action == '3'))
	{
	  veh_ptr->zone_num = save_zone;	  
	  if ( veh_ptr->t_status.meter_on && (0) )
	    {
	      veh_ptr->t_status.crse_annul = 1;
	      Veh_set_value((VEH_HNDL) veh_ptr, VEH_CRSE_CANX_TIME, (HNDL) mads_time);
	      Veh_set_value((VEH_HNDL) veh_ptr, VEH_CANX_TYPE, (HNDL) CANX_TYPE_CANCEL_KEY);
	    }
	  else
	    {
	      veh_ptr->t_status.meter_on = 0;
	      Veh_reset_state((VEH_HNDL) veh_ptr, VEH_METER_ON_STATE, HNDL_NULL, HNDL_NULL );
	      if ( save_zone != 0 )
		Veh_set_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, (HNDL)save_zone);
	    }
	  
	}

   }
   return (SUCCESS);
}				/* end Veh_call_cancel */

/********************************************************/
/* RET_STATUS VEH_get_status(veh_hndl, veh_resp_buf)    */
/* OP: return veh status in addressed buffer            */
/* PRE: veh exists                                      */
/* POST: veh status added to veh_resp_buf               */
/* ERROR: none                                          */
/*          change doc                                  */
/********************************************************/

RET_STATUS
Veh_get_status(veh_hndl, veh_resp_buf)
   VEH_HNDL veh_hndl;
   struct veh_drv_info_resp *veh_resp_buf;
{
   struct veh_driv *veh_ptr;

   veh_ptr = (struct veh_driv *) veh_hndl;
   veh_resp_buf->fleet = Fleet_get_id(veh_ptr->fleet_nbr);
   veh_resp_buf->veh_nbr = veh_ptr->veh_nbr;	/* Vehicle number */
   veh_resp_buf->drv_id = veh_ptr->driver_id;	/* Driver ID */
   strcpy(veh_resp_buf->rf_id, veh_ptr->mid);	/* rf_id number */
   if (veh_ptr->t_status.posted || veh_ptr->t_status.e_posted)	/* if posted, show 'em
								 * where */
      veh_resp_buf->zone_num = veh_ptr->zone_num;
   else
      veh_resp_buf->zone_num = 0;
   memcpy(&veh_resp_buf->cur_status, &veh_ptr->t_status, sizeof(struct taxi_status));
   memcpy(&veh_resp_buf->veh_attr, &veh_ptr->vehicle_attr, sizeof(struct veh_attributes));
   memcpy(&veh_resp_buf->drv_attr, &veh_ptr->driver_attr, sizeof(struct drv_attributes));
   veh_resp_buf->emerg_on_time = veh_ptr->emergency_on_date;	/* emergency on time */
   veh_resp_buf->emerg_off_time = veh_ptr->emergency_off_date;	/* emergency off time */
   veh_resp_buf->short_meter = veh_ptr->short_meters;	/* number of short
							 * meters */
   veh_resp_buf->req_to_talk = veh_ptr->req_to_talk;	/* number of times
							 * driver requested to
							 * talk */
   veh_resp_buf->late_meter = veh_ptr->late_meters;	/* number of late meters */
   veh_resp_buf->messages = veh_ptr->messages;	/* number of messages sent to
						 * supervisors */
   veh_resp_buf->sign_on_dt_tm = veh_ptr->signed_on_date;	/* Last sign on time */
   veh_resp_buf->sign_off_dt_tm = veh_ptr->signed_off_date;	/* Last sign off time */
   veh_resp_buf->calls = veh_ptr->calls;	/* number of dispatched calls */
   veh_resp_buf->posts = veh_ptr->posts;	/* number of times driver
						 * posted */
   veh_resp_buf->no_accepts = veh_ptr->no_accepts;	/* number of no_accepts */
   veh_resp_buf->callbacks = veh_ptr->callbacks;	/* number of callbacks */
   veh_resp_buf->flags = veh_ptr->flags;	/* number of flags picked up
						 * by the driver */
   veh_resp_buf->cond_posts = veh_ptr->cond_posts;	/* number of conditional
							 * posts by the driver */
   veh_resp_buf->rejects = veh_ptr->rejects;	/* number of rejects */
   veh_resp_buf->callouts = veh_ptr->callouts;	/* number of callouts */
   veh_resp_buf->assigns = veh_ptr->assigns;	/* # of manual assigns for
						 * drivers last shift */
   veh_resp_buf->bids = veh_ptr->bids;	/* number of bids awarded to driver */
   veh_resp_buf->enroute_posts = veh_ptr->enroute_posts;	/* number of times
								 * driver enroute posted */
   veh_resp_buf->breaks = veh_ptr->breaks;	/* number of breaks taken by
						 * the driver */
   veh_resp_buf->no_shows = veh_ptr->no_shows;	
   veh_resp_buf->gps_lat =  veh_ptr->gps_lat;
   veh_resp_buf->gps_long = veh_ptr->gps_long;   
   
   /*
    * veh_resp_buf->missing_receipts =
    * veh_ptr->itm_info.nbr_missing_receipts; MOVE TO BASELINE
    */
   return (SUCCESS);
}				/* end Vehicle_get_status */

/*******************************************************/
/* RET_STATUS Veh_send_status(req_ptr)                 */
/* OP:  send veh status to ui                          */
/* PRE: none                                           */
/* POST:  veh  status send to ui proc                  */
/* ERROR: illegal veh                                  */
/*        error in transmitting msg to ui              */
/*******************************************************/

RET_STATUS
Veh_send_status(req_ptr)	/* Was veh_driv_status */
   struct veh_rqst *req_ptr;
{
   struct veh_drv_info_resp veh_resp_buf;
   struct veh_driv *veh_ptr;
   VEH_HNDL veh_hndl;
   char  error_str[80];
   short fleet_nbr;

   memset((char *) &veh_resp_buf, '\0', sizeof veh_resp_buf);
   veh_resp_buf.u_prcs_id = req_ptr->u_prcs_id;
   veh_resp_buf.rec_type = VEH_REQ;
   fleet_nbr = Fleet_get_nbr(req_ptr->fleet);
   if (Veh_get_hndl(BY_NBR, req_ptr->veh_nbr, fleet_nbr, &veh_hndl) == FAIL)
   {
      sprintf(error_str, "Veh_send_status  - %d is bad veh nbr", req_ptr->veh_nbr);
      ERROR(req_ptr->fleet, "", error_str);
      if (msgsnd(dis_msgqid, (struct msgbuf *) & veh_resp_buf, sizeof(struct veh_drv_info_resp), IPC_NOWAIT) < 0)
      {
	 ERROR(' ', "", "Veh_send_status - veh_driv status not sent.\n");
      }
      return (FAIL);
   }
   if (Veh_get_status(veh_hndl, &veh_resp_buf) == FAIL)
      return (FAIL);

   if (msgsnd(dis_msgqid, (struct msgbuf *) & veh_resp_buf, sizeof(struct veh_drv_info_resp), IPC_NOWAIT) < 0)
   {
      ERROR(' ', "", "Veh_send_status - veh_driv status not sent.\n");
      return (FAIL);
   }
   return (SUCCESS);
}				/* end Veh_send_status */

RET_STATUS
Veh_send_emerg_bcast_status(to_msgqid, req_ptr)	/* Was veh_driv_status */
  int to_msgqid;
   struct emerg_bcast_rqst *req_ptr;
{
   EMERG_BCAST_RESP bcast_resp_buf;
   struct veh_driv *veh_ptr;
   VEH_HNDL veh_hndl, test_veh_hndl;
   char  error_str[80];
   short fleet_nbr;
   CALL   pseudo_call;
   FLEET_HNDL fleet_hndl;
   FLEET_VEH_NBR_LIST_HNDL  veh_nbr_list;
   short  nbr_taxis = 0;

   memset((char *) &bcast_resp_buf, '\0', sizeof bcast_resp_buf);
   bcast_resp_buf.d_prcs_id = req_ptr->u_prcs_id;
   bcast_resp_buf.u_prcs_id = req_ptr->u_prcs_id;
   bcast_resp_buf.rec_type = EMERG_BCAST_REQ;
   fleet_nbr = Fleet_get_nbr(req_ptr->fleet);


   if (Veh_get_hndl(BY_NBR, req_ptr->veh_nbr, fleet_nbr, &veh_hndl) == FAIL)
   {
      sprintf(error_str, "Veh_send_status  - %d is bad veh nbr", req_ptr->veh_nbr);
      ERROR(req_ptr->fleet, "", error_str);
      if (msgsnd(dis_msgqid, (struct msgbuf *) & bcast_resp_buf, sizeof(EMERG_BCAST_RESP), IPC_NOWAIT) < 0)
      {
	 ERROR(' ', "", "Veh_send_emerg_bcast_status - veh_driv status not sent.\n");
      }
      return (FAIL);
   }

   Fleet_get_hndl( BY_NBR, fleet_nbr, &fleet_hndl );
   veh_nbr_list = ( FLEET_VEH_NBR_LIST_HNDL )Fleet_get_value( fleet_hndl, FLEET_VEH_NBR_LIST );
   nbr_taxis = 0;
   Fleet_veh_nbr_list_each_get( veh_nbr_list, test_veh_hndl )
     {
       if ( test_veh_hndl == veh_hndl )
         continue;
       
       veh_ptr = ( struct veh_driv * ) test_veh_hndl;

       if ( (char)Veh_get_value( test_veh_hndl, VEH_NAV_TYPE ) != STAR2000_MDT )
         continue;
       
       if ( veh_ptr->signed_on_date > 0  && veh_ptr->signed_off_date == 0 )
         {
           pseudo_call.gps_lat = veh_ptr->gps_lat;
           pseudo_call.gps_long = veh_ptr->gps_long;
           if ( ( GPS_veh_call_get_distance( (VEH_HNDL)veh_hndl, (CALL_HNDL)&pseudo_call ) < req_ptr->radius ) )
             ++nbr_taxis;
         }
     } Fleet_veh_nbr_list_each_end

   bcast_resp_buf.nbr_taxis = nbr_taxis;
   
   if (msgsnd(dis_msgqid, (struct msgbuf *) & bcast_resp_buf, sizeof(struct emerg_bcast_rqst), IPC_NOWAIT) < 0)
   {
      ERROR(' ', "", "Veh_send_emerg_bcast_status - veh_driv status not sent.\n");
      return (FAIL);
   }
   return (SUCCESS);
}				/* end Veh_send_emerg_bcast_status */

/********************************************************/
/* RET_STATUS Veh_initialize(veh_hndl)                  */
/* OP: initial the vehilce                              */
/* PRE: veh exists                                      */
/* POST:  VEH_ON_CALL_STATE reset                       */
/*        veh status set to off                         */
/*        veh exceptions (requests) initialized         */
/*        veh/driv stats initialized                    */
/* ERROR: none                                          */
/********************************************************/

RET_STATUS
Veh_initialize(veh_hndl)
   VEH_HNDL veh_hndl;
{
   register ii, jj;
   int   call_nbr;
   struct veh_driv *veh_ptr;
   VEH_REC veh_rec;

   veh_ptr = (struct veh_driv *) veh_hndl;

   memset((char *) &veh_rec, '\0', sizeof( VEH_REC ) );
   Veh_reset_state(veh_hndl, VEH_ON_CALL_STATE, HNDL_NULL, HNDL_NULL);

   /* TODO - putsome of this in driver */
   ii = veh_ptr->t_status.kdt_working;	/* push the working status */
   jj = veh_ptr->t_status.meter_working;	/* push the working status */
   memset((char *) &veh_ptr->t_status, '\0', sizeof(struct taxi_status));	/* clear out status bits */
   veh_ptr->t_status.kdt_working = ii;	/* pop the working status */
   veh_ptr->t_status.meter_working = jj;	/* pop the working status */

   veh_ptr->c_post_time = 0;	/* get rid of conditional post restriction */
   veh_ptr->e_post_time = 0;	/* get rid of enroute post restriction */
   veh_ptr->on_break_time = 0;

   if ( ( mads_time - veh_ptr->emergency_on_date ) > 86400 )   
     veh_ptr->emergency_on_date = 0;
   if ( ( mads_time - veh_ptr->emergency_off_date ) < 86400 )
     veh_ptr->emergency_off_date = 0;

   veh_ptr->time_info = 0;
   veh_ptr->calls = 0;		/* Number of dispatched calls. */
   veh_ptr->flags = 0;		/* Number of flags picked up on the street. */
   veh_ptr->bids = 0;		/* Number of bids awarded to this driver. */
   veh_ptr->posts = 0;		/* Number of times this driver posted. */
   veh_ptr->cond_posts = 0;	/* Number of times driver cond posted */
   veh_ptr->enroute_posts = 0;	/* # of times driver enroute posted. */
   veh_ptr->short_meters = 0;	/* Number of short meters. */
   veh_ptr->late_meters = 0;	/* Number of late meters. */
   veh_ptr->no_accepts = 0;	/* Number of no-accepts. */
   veh_ptr->rejects = 0;	/* Number of rejects. */
   veh_ptr->breaks = 0;		/* Number of breaks taken. */
   veh_ptr->req_to_talk = 0;	/* # of times driver requested to talk. */
   veh_ptr->messages = 0;	/* Number of messages sent to supervisors. */
   veh_ptr->callbacks = 0;	/* Number of callbacks. */
   veh_ptr->callouts = 0;	/* Number of callouts. */
   veh_ptr->no_shows = 0;	/* Number of no-shows. */
   veh_ptr->tmp_calls = 0;	/* Number of dispatched calls. */
   veh_ptr->tmp_flags = 0;	/* Number of flags picked up on the street. */
   veh_ptr->tmp_bids = 0;	/* Number of bids awarded to this driver. */
   veh_ptr->tmp_posts = 0;	/* Number of times this driver posted. */
   veh_ptr->tmp_cond_posts = 0;	/* Number of times driver cond posted */
   veh_ptr->tmp_enroute_posts = 0;	/* # of times driver enroute posted. */
   veh_ptr->tmp_short_meters = 0;	/* Number of short meters. */
   veh_ptr->tmp_late_meters = 0;/* Number of late meters. */
   veh_ptr->tmp_no_accepts = 0;	/* Number of no-accepts. */
   veh_ptr->tmp_rejects = 0;	/* Number of rejects. */
   veh_ptr->tmp_breaks = 0;	/* Number of breaks taken. */
   veh_ptr->tmp_req_to_talk = 0;/* # of times driver requested to talk. */
   veh_ptr->tmp_messages = 0;	/* Number of messages sent to supervisors. */
   veh_ptr->tmp_callbacks = 0;	/* Number of callbacks. */
   veh_ptr->tmp_callouts = 0;	/* Number of callouts. */
   veh_ptr->tmp_no_shows = 0;	/* Number of no-shows. */
   veh_ptr->reject_time = 0;	/* time of rejecting a call */
   veh_ptr->inqry_time = 0;	/* time of inquiry (zone info) */
   veh_ptr->veh_info_time = 0;	/* time of vehicle info request */
   veh_ptr->epost_zone = 0;	/* zone number for enroute posting */
   if (veh_ptr->veh_calls_list_hndl != HNDL_ERR)
      Veh_calls_list_clear(veh_ptr->veh_calls_list_hndl);

   /* Reset values in vehicle database */
   veh_rec.fleet = fleet[veh_ptr->fleet_nbr]->fleet_id;
   veh_rec.nbr = veh_ptr->veh_nbr;
   if (db_read_key(VEHICLE_FILE_ID, &veh_rec, &veh_key2, ISEQUAL + ISLOCK) == SUCCESS)
   {
     veh_rec.last_driver = veh_ptr->driver_id;
     veh_rec.assigns = veh_rec.calls = veh_rec.flags = veh_rec.bids = veh_rec.posts = veh_rec.cond_posts =
     veh_rec.enroute_posts = veh_rec.short_meters = veh_rec.late_meters = veh_rec.no_accepts = veh_rec.reject = veh_rec.breaks =
     veh_rec.req_to_talk = veh_rec.messages = veh_rec.callbacks = veh_rec.callouts = veh_rec.no_shows = 0;

     veh_ptr->term_type = veh_rec.term_type;

     db_update(VEHICLE_FILE_ID, &veh_rec);
     db_release(VEHICLE_FILE_ID);
   }

   return(SUCCESS);

}				/* end Veh_initialize */

/************************************************************/
/* RET_STATUS Veh_get_info(VEH_HNDL, attr, value_ptr)       */
/* OP: get an attribute value from a vehicle object         */
/* PRE: vehicle object exists                               */
/* POST:  attribute value retrieved and RET_STATUS returned */
/* ERROR: no value pointer given (FAIL)                     */
/************************************************************/
RET_STATUS
Veh_get_info(
	     VEH_HNDL veh_hndl,
	     int attr,
	     HNDL * value_ptr)
{
   if (value_ptr == NULL)
      return (FAIL);

   *value_ptr = (HNDL) Veh_get_value(veh_hndl, attr);

   return (SUCCESS);
}

RET_STATUS
Veh_send_to_printer(veh_hndl, message)
   VEH_HNDL veh_hndl;
   char *message;
{
   mk_outb_text(NULL);
   add_outb_text(VEH_PRINTER_HEADER);
   add_outb_text(message);
   add_outb_text(VEH_PRINTER_TRAILER);
   if (send_msg_mmp(((VEH *) veh_hndl)->mid, VEH_SEND_TO_PRINTER, (VEH *) veh_hndl) != SUCCESS)
      return (VEH_MESSAGE_NOT_SENT);
   else
      return (SUCCESS);
}

RET_STATUS
Veh_redispatch_call(veh_hndl, call_hndl, redispatch_type)
   VEH_HNDL veh_hndl;
   CALL_HNDL call_hndl;
   int       redispatch_type;
{
    VEH * veh_ptr;
    CALL * call_ptr;
    char scratch_ptr[80];
   short term_type = 0;
   
    veh_ptr = (VEH *) veh_hndl;
    call_ptr = (CALL *) call_hndl;

    if (veh_ptr->call_nbr != call_ptr->call_number)
    {
            return(FAIL);
    }

    if (!veh_ptr->t_status.pckup)
    {
      if ( redispatch_type == M_ACTION_REDISPATCH && (0) )
	{
	  sprintf( scratch_ptr, "%s %d %s %s %s",
		  "KÖRNING", veh_ptr->call_nbr, "SKICKAD TILL NY BIL,", CR,
		  "TAXAMETERN UPPTAGEN" );
	}
      else
	{
	  sprintf(scratch_ptr, "%s %d %s ",
		  CALL_, veh_ptr->call_nbr, CALL_REDISPATCH);
	}
      
      term_type = (short)Veh_get_value( (VEH_HNDL)veh_ptr, VEH_TERM_TYPE );
      if ( term_type == MDT_OPUS_2K )
        {
          mk_outb_text( scratch_ptr );
          send_text_predefined( veh_ptr->mid, TRIP_CANX, veh_ptr );
        }
      else
        send_text(veh_ptr->mid, scratch_ptr, veh_ptr, PRIORITY_3);
      
      Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_CALL_STATE, HNDL_NULL, HNDL_NULL);      
   }
   else
   {
      veh_cvt_to_flag(veh_ptr);
   }
            
}                      /* end Veh_redispatch_call */

/* call has been canceled or redispatcherd while there was */
/* a pickup - essentiall convert vehicle to flag state     */

static void
veh_cvt_to_flag(veh_ptr)
   struct veh_driv * veh_ptr;
{
   /* pretty bogus */
      Veh_reset_state((VEH_HNDL) veh_ptr, VEH_METER_ON_STATE, HNDL_NULL, HNDL_NULL);
      veh_ptr->call_nbr = 0;
      veh_ptr->call_ptr = NULL;
      veh_ptr->t_status.late_meter = 0;
      return;
}                   /* end veh_cvt_to_flag */


int 
Vehicle_match_attr( unsigned char *addr1, unsigned char *addr2 )
{
  
  if (*addr2 == (*addr1 & *addr2)) /* checks 1-8 */
    {
      ++addr1;
      ++addr2;
      if ( *addr2 == ( *addr1 & *addr2 ) ) /* checks 9-16 */
	{
	  ++addr1;
	  ++addr2;
	  if ( *addr2 == ( *addr1 & *addr2 ) ) /* checks 17-24 */
	    {
	      ++addr1;
	      ++addr2;
	      if ( *addr2 == ( *addr1 & *addr2 ) ) /* checks 25-32 */
		{
		  ++addr1;
		  ++addr2;
		  if ( *addr2 == ( *addr1 & *addr2 )) /* checkec 33 - 40 */
		    {
		      ++addr1;
		      ++addr2;
		      if ( *addr2 == ( *addr1 & *addr2 )) // checks 41 - 48
			{
			  ++addr1;
			  ++addr2;
			  if ( *addr2 == ( *addr1 & *addr2 )) // checks 49 - 56
			    {
			      ++addr1;
			      ++addr2;
			      if ( *addr2 == ( *addr1 & *addr2 )) // checkes 57 - 64
				return(TRUE);
			    }
			}
		    }
		}
	    }
	}
    }
  
  return (FALSE);
}
