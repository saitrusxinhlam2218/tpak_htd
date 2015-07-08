/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Fleet_private.c,v $
*  @(#)  $Revision: 1.17 $
*  @(#)  $Date: 2004/06/22 17:09:22 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Fleet_private.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Fleet_private.c,v 1.17 2004/06/22 17:09:22 jwelch Exp $";

#include <sys/types.h>		/* temp */
#include <sys/times.h>		/* temp */
#include "mad_error.h"
#include "taxipak.h"
#include "Object.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Fleet_db.h"
#include "Attribute_db.h"
#include "taxi_db.h"
#include "path.h"
#include "language.h"
#include "mad_ipc.h"

extern FLEET *fleet[ ]; 
extern struct offsets *offset;	/* offsets of structures in shared memory */
extern char req_buf[];

extern HNDL fleet_value();
RET_STATUS Fleet_rec_2_obj();

RET_STATUS
Fleet_set_value(fleet_hndl, attr, hndl)
   FLEET_HNDL fleet_hndl;
   int   attr;
   HNDL  hndl;
{
   HNDL  status;

   if ((attr >= STATS_START) && (attr <= STATS_END))
   {
      printf("ERROR - attempt to set  fleet value %d that is a stat \n", attr);
      return (FAIL);		/* cant set a stat */
   }
   if (fleet_hndl == (FLEET_HNDL) NULL)
      return (FAIL);
   status = fleet_value(fleet_hndl, PUT, attr, hndl);
   return ((short) status);
}				/* end Fleet_set_value */

RET_STATUS
Fleet_increment_stats(
		      FLEET_HNDL fleet_hndl,
		      int attr,
		      short index)
{
   if ((attr >= STATS_START) && (attr <= STATS_END))
      return ((int) fleet_value(fleet_hndl, INCREMENT, attr, index));
   return (FAIL);
}				/* end fleet_increment_stats */

RET_STATUS
Fleet_decrement_stats(
		      FLEET_HNDL fleet_hndl,
		      int attr,
		      short index)
{
   if ((attr >= STATS_START) && (attr <= STATS_END))
      return ((int) fleet_value(fleet_hndl, DECREMENT, attr, index));
   return (FAIL);
}				/* end fleet_decrement_stats */

RET_STATUS
Fleet_initialize_stats(fleet_hndl, attr, index)
   FLEET_HNDL fleet_hndl;
   int   attr;
{
   if ((attr >= STATS_START) && (attr <= STATS_END))
      return ((int) fleet_value(fleet_hndl, PUT, attr, index));
   return (FAIL);
}				/* end fleet_decrement_stats */


/*******************************************************************************
* fleet_maint - Update the fleet structure in memory.
******************************************************************************/
Fleet_maint(int rec_nbr, int key_fld, char fleet_id)
{
   FLEET_REC *fl_ptr;
   int   fl_nbr;
   int   stats_compute_freq_on, inactive_retry_interval_on, zone_status_update_freq_on, zone_info_freq_on;
   FLEET_HNDL fleet_hndl;

   stats_compute_freq_on = 0;
   inactive_retry_interval_on = 0;
   zone_status_update_freq_on = 0;
   zone_info_freq_on = 0;

   if (Fleet_get_hndl(BY_ID, fleet_id, &fleet_hndl) == FAIL)
   {
      ERROR(fleet_id, FLEET_FILE, "fleet_maint - cannot find fleet id.\n");
      return;
   }
   fl_nbr = (short) Fleet_get_value(fleet_hndl, FLEET_NBR);

   fl_ptr = (FLEET_REC *) req_buf;

   if (db(FLEET_FILE_ID, OPEN, &fl_key2, ISINOUT | ISMANULOCK, 0) < 0)
   {				/* open the file */
      ERROR(' ', FLEET_FILE, "fleet_maint - cannot open file\n");
      return;
   }
   if (db(FLEET_FILE_ID, READ_NUM, &fl_key2, ISEQUAL, rec_nbr) < 0)
   {				/* read record */
      fl_ptr->nbr = fl_nbr;
      if (db(FLEET_FILE_ID, READ_KEYS, &fl_key2, ISEQUAL, 0) < 0)
      {				/* if fleet record is not returned */
	 ERROR(fleet_id, FLEET_FILE, "fleet_maint - cannot find record.\n");
	 db(FLEET_FILE_ID, CLOSE, &fl_key1, 0, 0);	/* close the file */
	 return;
      }
   }
   /*
    * For stats_compute, inactive_retry_interval, and zone_status_update, if
    * not already running, don't allow change. This means if any of these
    * values are zero when dispatch starts, it can not be changed while
    * dispatch is running. This is probably not necessary. It may be
    * necessary to init things first if things are going to be turned on.
    */
   if (fleet[fl_nbr]->stats_compute_freq != 0)
      stats_compute_freq_on = 1;

   if (fleet[fl_nbr]->inactive_retry_interval != 0)
      inactive_retry_interval_on = 1;

   if (fleet[fl_nbr]->zone_status_update_freq != 0)
      zone_status_update_freq_on = 1;

   if (fleet[fl_nbr]->zone_info_freq != 0)
      zone_info_freq_on = 1;

   Fleet_rec_2_obj(fleet[fl_nbr], fl_ptr);	/* populate fleet */

   if (fleet[fl_nbr]->active == YES)
   {
      if ((stats_compute_freq_on) && (fleet[fl_nbr]->stats_compute_freq))
	 Fleet_update_pickup_stats(fl_nbr);

      if ((inactive_retry_interval_on) && (fleet[fl_nbr]->inactive_retry_interval))
	 inactivity_logout(fl_nbr);

      /*
       * If not doing zone_info before, but doing it now, call d_search() and
       * send_srch_display() to start it off. If alredy doing zone_info the
       * next timer event call to d_search() will set the next timer to the
       * new value.
       */
      if (!zone_info_freq_on && fleet[fl_nbr]->zone_info_freq)
      {
	 Fleet_make_search_report(fl_nbr, 0);
	 send_srch_display(fl_nbr);
      }
      if ((zone_status_update_freq_on) && (fleet[fl_nbr]->zone_status_update_freq))
	 (void) Fleet_zone_summary(fl_nbr);	/* begin summary reports */

   }
   db(FLEET_FILE_ID, CLOSE, &fl_key1, 0, 0);	/* close the file */
}                    /* end Fleet_maint */

/****************************************************************************
* Fleet_rec_2_obj - Copy data from the disk file structure to the internal
* structure.
*****************************************************************************/
RET_STATUS
Fleet_rec_2_obj(dest, src)
   FLEET *dest;
   FLEET_REC *src;
{
   short  ch_nbr;		/* for reading the ATTR file */
   short j;			/* loop index */
   struct cisam_at *attr_ptr;	/* pointer to a record returned by cisam */
   char error_str[80];


#ifdef DEBUG
   if (offset->prattle >= LEVEL2)
   {
      sprintf(trace_str, "Fleet_rec_2_obj : fleet = %c src = %x dest = %x \n", src->id, src, dest);
      TRACE(trace_str);
   }
#endif

   dest->fleet_id = src->id;
   strcpy(dest->fleet_name, src->name);

   dest->active = src->active;
   dest->matching_algorithm = src->match_algorithm;
   dest->fleet_nbr = src->nbr;
   dest->drv_reg_pri = src->drv_regular_pri;
   dest->drv_nshow_pri = src->drv_no_show_pri;
   dest->drv_locl_pri = src->drv_local_pri;
   dest->drv_cncl_pri = src->drv_cancel_pri;
   dest->local_calls_time = src->local_calls_dur;
   dest->call_offer_procedure = src->call_offer_proc;
   dest->no_accept_penalty = src->no_acpt_penalt;
   dest->missing_receipt_retry_cnt = src->missing_receipt_retries;

   dest->missing_receipts_timeout = src->missing_receipt_timeout;
   if (dest->missing_receipts_timeout < 60)
      dest->missing_receipts_timeout = 60;	/* minimum time out period */
   dest->bid_action = src->bid_action;

   if (src->no_seals == YES)
      dest->criteria.suppress_seals = 1;
   else
      dest->criteria.suppress_seals = 0;

   if (src->no_personal != YES)
      dest->criteria.suppress_personal = 1;
   else
      dest->criteria.suppress_personal = 0;

   dest->auto_flag_on_stands = src->auto_stand_flag;
   dest->no_show_action = src->no_show_action;
   dest->cancel_action = src->cancel_action;
   dest->e_post_action = src->enr_post_action;

   if (src->hold_line_flag == YES)
      dest->criteria.hold_line = 1;
   else
      dest->criteria.hold_line = 0;

   dest->zone_backup_level = src->zone_backup_lev - '0';
   if (src->avl_call_q_time > 999)
      src->avl_call_q_time = 999;
   dest->unassigned_time = src->avl_call_q_time * 60;
   dest->call_offer_info = src->cal_offer_info;
   dest->call_assignment_info = src->cal_assign_info;
   dest->approach_tariff = src->approach_tariff;
   if (src->num_levels_to_compute_stats > LEVEL_MAX)
   {
       sprintf(error_str,"Fleet %d stats level reset from %d", 
                          src->nbr, src->num_levels_to_compute_stats);
       ERROR(' ',"", error_str);
       dest->num_levels_to_compute_stats = LEVEL_MAX;
   }
   else
      dest->num_levels_to_compute_stats = src->num_levels_to_compute_stats;
   dest->stats_compute_freq = src->stats_compute_freq * 60;
   dest->handle_system_assigned_calls = src->handle_system_assigned_calls;
   dest->inactive_retry_interval = src->inactive_retry_interval * 60;
   dest->inactive_time_out_period = src->inactive_time_out_period * 60;
   dest->veh_inquiry_freq = src->veh_inqry_freq;
   dest->zone_info_freq = src->zone_info_freq;
   dest->zone_inqry_freq = src->zone_inqry_freq;
   dest->posting_freq = src->post_freq;
   dest->cond_posting_freq = src->cond_post_freq;
   dest->call_accept_per = src->call_accept_per;
   dest->break_duration = src->break_duration * 60;
   dest->bid_duration = src->bid_duration;
   dest->e_post_lead_time = src->enr_post_lead * 60;
   dest->short_meter_duration = src->short_meter_dur * 60;
   dest->late_meter = src->late_meter * 60;
   dest->max_reject_count = src->max_rej_count;
   dest->pers_lead_tm = src->pers_lead_tm * 60;
   dest->when_enroute_post = src->when_enr_post;
   dest->no_accept_no_post_period = src->no_acpt_penalty * 60;
   dest->max_nbr_veh_atr = src->max_nbr_veh_atr;
   dest->max_nbr_drv_atr = src->max_nbr_drv_atr;
   dest->zone_status_update_freq = src->zone_stat_freq;
   dest->approve_msgs_to_drivers = src->drv_msg_approve;
   dest->max_cabs_per_call = src->max_vehs_call + 0x30;	/* add ascii mask */
   dest->future_call_maximum = (long) src->future_call_max * 24 * 60 * 60;	/* days  => sec */
   dest->future_call_minimum = src->future_call_min * 60;	/* min => sec */
   dest->pass_flat_rate_calc = src->pass_rate_calc;
   dest->del_flat_rate_calc = src->dlv_rate_calc;
   dest->subsc_call_review = src->subsc_review;
   dest->greeting_line = src->greeting_line;
   strcpy(dest->default_city, src->city_default);
   strcpy(dest->default_phone_area, src->phone_area_def);
   dest->default_call_type = src->call_type_def;
   dest->live_time_calls_freq = src->time_calls_freq;

   dest->level_reject_penalty[0] = src->lev1_rej_penalt;
   dest->level_reject_penalty[1] = src->main_rej_penalt;
   dest->level_reject_penalty[2] = src->lev2_rej_penalt;
   dest->level_reject_penalty[3] = src->lev3_rej_penalt;
   dest->level_reject_penalty[4] = src->lev4_rej_penalt;
   dest->level_reject_penalty[5] = src->lev5_rej_penalt;
   dest->level_reject_penalty[6] = src->lev6_rej_penalt;
   dest->level_reject_penalty[7] = src->lev7_rej_penalt;
   dest->level_reject_penalty[8] = src->lev8_rej_penalt;

   dest->level_exclusive_time[0] = src->lev1_excl_time * 60;
   dest->level_exclusive_time[1] = src->main_excl_time * 60;	/* stored in seconds */
   dest->level_exclusive_time[2] = src->lev2_excl_time * 60;
   dest->level_exclusive_time[3] = src->lev3_excl_time * 60;
   dest->level_exclusive_time[4] = src->lev4_excl_time * 60;
   dest->level_exclusive_time[5] = src->lev5_excl_time * 60;
   dest->level_exclusive_time[6] = src->lev6_excl_time * 60;
   dest->level_exclusive_time[7] = src->lev7_excl_time * 60;
   dest->level_exclusive_time[8] = src->lev8_excl_time * 60;

   strcpy(dest->audit_trail_prt_port, src->audt_trail_port);
   dest->passenger_priority_def = src->pass_pri_def;
   dest->delivery_priority_def = src->del_pri_def;
   dest->charge_priority_def = src->charge_pri_def;
   dest->time_calls_priority_def = src->calls_pri_def;
   dest->subsc_priority_def = src->subsc_pri_def;
   dest->typex_priority_def = src->typex_pri_def;
   dest->wake_up_priority_def = src->wake_up_pri_def;
   dest->highest_rookie_pri = src->high_rookie_pri;

   if (src->met_pass_call == YES)
      dest->criteria.met_pass_call = 1;
   else
      dest->criteria.met_pass_call = 0;

   if (src->met_del_call == YES)
      dest->criteria.met_del_call = 1;
   else
      dest->criteria.met_del_call = 0;

   if (src->met_typex_call == YES)
      dest->criteria.met_typex_call = 1;
   else
      dest->criteria.met_typex_call = 0;

   dest->hist_srch = src->hist_search_def;
   dest->time_call_freq = src->time_call_freq * 60;
   dest->time_call_max = src->time_call_max;
   strcpy(dest->estimate_text[0], src->estimate_text1);
   dest->range_limit[0] = src->range_limit1;
   strcpy(dest->estimate_text[1], src->estimate_text2);
   dest->range_limit[1] = src->range_limit2;
   strcpy(dest->estimate_text[2], src->estimate_text3);
   dest->range_limit[2] = src->range_limit3;
   strcpy(dest->estimate_text[3], src->estimate_text4);
   dest->range_limit[3] = src->range_limit4;
   strcpy(dest->estimate_text[4], src->estimate_text5);
   dest->range_limit[4] = src->range_limit5;
   strcpy(dest->estimate_text[5], src->estimate_text6);
   dest->range_limit[5] = src->range_limit6;
   strcpy(dest->estimate_text[6], src->estimate_text7);
   dest->range_limit[6] = src->range_limit7;
   strcpy(dest->estimate_text[7], src->estimate_text8);
   dest->range_limit[7] = src->range_limit8;
   strcpy(dest->def_est_text, src->def_est_text);
   dest->active = src->active;
   dest->wakeup_lead_time = src->wakeup_lead_time * 60;	/* save in seconds */
   dest->itm_missing_receipts_max = src->itm_missing_receipts_max;
   dest->nak_offer_penal = src->nak_offer_penal;
   dest->auto_flag_req_time = src->auto_flag_time * 60;
   dest->time_int_req = src->time_int_req;

   if ( src->time_calls_lead_supp != dest->time_calls_lead_supp )
     {
       TC_LeadTimeModify( (FLEET_HNDL) dest, src->time_calls_lead_supp, dest->time_calls_lead_supp );
     }
   
   dest->time_calls_lead_supp = src->time_calls_lead_supp;
   dest->FR_distance_filter = src->FR_distance_filter;
#ifdef NAMERICA
   dest->send_dest_addr = src->send_dest_addr;
   dest->unassgnd_look_ahead = src->unassgnd_look_ahead;
   dest->send_dest_offer = src->send_dest_offer;
   dest->send_pckup_cmt = src->send_pckup_cmt;
   dest->send_rate = src->send_rate;
#endif
   dest->gps_state = src->gps_state;
   dest->gps_time = src->gps_time;
   dest->gps_distance_ceiling = src->gps_distance_ceiling;
   Fleet_set_value( (FLEET_HNDL)dest, FLEET_SEND_CALL_TYPE, (HNDL)src->send_call_type );
   Fleet_set_value( (FLEET_HNDL)dest, FLEET_AUTO_REJECT_TIME, (HNDL)src->auto_reject_time );
   Fleet_set_value( (FLEET_HNDL)dest, FLEET_M_ACTION_TIMER, (HNDL)src->m_action_timer );
   Fleet_set_value( (FLEET_HNDL)dest, FLEET_M_ACTION_RETRY, (HNDL)src->m_action_retry );
   Fleet_set_value( (FLEET_HNDL)dest, FLEET_M_ACTION_PENALTY, (HNDL)src->m_action_penalty );
   Fleet_set_value( (FLEET_HNDL)dest, FLEET_CANX_TO_MOFF_TIME, (HNDL)src->canx_to_moff_time );
   dest->gps_max_x = src->gps_max_x;
   dest->gps_min_x = src->gps_min_x;
   dest->gps_max_y = src->gps_max_y;
   dest->gps_min_y = src->gps_min_y;
   Fleet_set_value( (FLEET_HNDL)dest, FLEET_GPS_COORD_SYS, (HNDL)src->gps_coord_sys );
   Fleet_set_value( (FLEET_HNDL)dest, FLEET_GPS_BOOK, (HNDL)src->gps_book );
  

   Fleet_set_value( (FLEET_HNDL)dest, FLEET_TC_RESTRICT_INTERVAL, (HNDL)src->tc_restrict_interval );
   Fleet_set_value( (FLEET_HNDL)dest, FLEET_NS_SHORT, (HNDL)src->ns_short );

   Fleet_set_value( (FLEET_HNDL)dest, FLEET_EMERG_RANGE, (HNDL)src->emerg_range );
   Fleet_set_value( (FLEET_HNDL)dest, FLEET_ENROUTE_BREAK, (HNDL)src->enroute_break );
   
   Fleet_set_value( (FLEET_HNDL)dest, FLEET_ILINK_ACTIVE, (HNDL)src->ilink_active );
   Fleet_set_value( (FLEET_HNDL)dest, FLEET_ILINK_TIMEOUT, (HNDL)src->ilink_timeout );

   Fleet_set_value( (FLEET_HNDL)dest, FLEET_CBOOK_TIME, (HNDL)src->cbook_time );
   Fleet_set_value( (FLEET_HNDL)dest, FLEET_STAND_NTFY_INTERVAL, (HNDL)src->stand_ntfy_interval );
   Fleet_set_value( (FLEET_HNDL)dest, FLEET_STAND_NTFY_LIFE, (HNDL)src->stand_ntfy_life);
   
   if ( src->load_unload == YES )
     offset->status.load_unload = 1;
   else
     offset->status.load_unload = 0;     
   
   /* load in attribute code matrix */
   attr_ptr = (struct cisam_at *) req_buf;	/* point to the req_buffer */
   if (db_open(ATTR_FILE_ID, ISINOUT + ISMANULOCK) < 0)
   {
      ERROR(src->id, ATTR_FILE, "Fleet_rec_2_obj - cant open attribute file\n");
      return(FAIL);
   }
   ch_nbr = 1;		/* attribute number */
   attr_ptr->fleet = src->id;
   attr_ptr->type = C_VEHICLE;	/* vehicle attributes */
   for (j = 0; j < ATTR_MAX; j++)
   {
      attr_ptr->nbr = ch_nbr;
      if (db(ATTR_FILE_ID, READ_KEYS, &at_key1, ISEQUAL, 0) < 0)
      {
	 ch_nbr++;		/* did not find record */
	 continue;		/* back to for loop */
      }
      strcpy(dest->vd_attr_info[j].veh_attr, attr_ptr->abbrev);
      strcpy(dest->vd_attr_info[j].veh_at_desc, attr_ptr->desc);
      dest->vd_attr_info[j].veh_FV = attr_ptr->factor_value;
      dest->vd_attr_info[j].veh_EV = attr_ptr->extra_value;
      ch_nbr++;
   }

   attr_ptr = (struct cisam_at *) req_buf;
   ch_nbr = 1;		/* attribute number */
   attr_ptr->fleet = src->id;
   attr_ptr->type = C_DRIVER;	/* driver attributes */
   for (j = 0; j < ATTR_MAX; j++)
   {
      attr_ptr->nbr = ch_nbr;
      if (db(ATTR_FILE_ID, READ_KEYS, &at_key1, ISEQUAL, 0) < 0)
      {
	 ch_nbr++;		/* did not find record */
	 continue;		/* back to for loop */
      }
      strcpy(dest->vd_attr_info[j].drv_attr, attr_ptr->abbrev);
      strcpy(dest->vd_attr_info[j].drv_at_desc, attr_ptr->desc);
      dest->vd_attr_info[j].drv_FV = attr_ptr->factor_value;
      dest->vd_attr_info[j].drv_EV = attr_ptr->extra_value;
      ch_nbr++;
   }
   /* close the attributes file */
   if ((db(ATTR_FILE_ID, CLOSE, &at_key1, ISEQUAL, 0)) < 0)
   {
      ERROR(src->id, ATTR_FILE, "Fleet_rec_2_obj - cant close attribute file\n");
      return(FAIL);
   }
   return (SUCCESS);
}				/* end Fleet_rec_2_obj */

/* TODO - call_stats processing */
/* something with all the arrays */
