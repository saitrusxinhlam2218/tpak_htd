static char sz__FILE__[]="@(#)fill_fl.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_fl.c; Rel: 6.6.0.0; Get date: 2/26/92 at 21:05:45
 * @(#)  Last delta: 5/27/91 at 14:00:12
 * @(#)  SCCS File: /taxi/sccs/s.fill_fl.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:fill_fl.c	6.6.0.0"

#define KEY_SRC

#include "taxipak.h"
#include "mads_types.h"
#include "Object.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "taxi_db.h"
#include "Fleet_db.h"

void
fill_fl_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_fl  *ft;

	ft = (struct cisam_fl*)req_buf;

	stchar(&ft->id,&buf[FL_ID],sizeof(ft->id));

	stint(ft->nbr,&buf[FL_NBR]);
}

void
fill_fl(req_buf, buf,action)
     char *req_buf;
     char *buf;
     int   action;
{
	register int   offset = 0;
	struct  cisam_fl  *ft;

	ft = (struct cisam_fl*)req_buf;

	stchar(&ft->id,&buf[offset],sizeof(ft->id));
	offset += sizeof(ft->id);

	stchar(&ft->match_algorithm,&buf[offset],sizeof(ft->match_algorithm));
	offset += sizeof(ft->match_algorithm);

	stint(ft->nbr,&buf[offset]);
	offset += INTSIZE;

	stchar(ft->name,&buf[offset],sizeof(ft->name)-1);
	offset += sizeof(ft->name)-1;

	stchar(&ft->drv_regular_pri,&buf[offset],sizeof(ft->drv_regular_pri));
	offset += sizeof(ft->drv_regular_pri);
	stchar(&ft->drv_no_show_pri,&buf[offset],sizeof(ft->drv_no_show_pri));
	offset += sizeof(ft->drv_no_show_pri);
	stchar(&ft->drv_local_pri,&buf[offset],sizeof(ft->drv_local_pri));
	offset += sizeof(ft->drv_local_pri);
	stchar(&ft->drv_cancel_pri,&buf[offset],sizeof(ft->drv_cancel_pri));
	offset += sizeof(ft->drv_cancel_pri);
	stint(ft->local_calls_dur,&buf[offset]);
	offset += INTSIZE;
	stchar(&ft->call_offer_proc,&buf[offset],sizeof(ft->call_offer_proc));
	offset += sizeof(ft->call_offer_proc);
	stchar(&ft->no_acpt_penalt,&buf[offset],sizeof(ft->no_acpt_penalt));
	offset += sizeof(ft->no_acpt_penalt);
	stchar(&ft->lev1_rej_penalt,&buf[offset],sizeof(ft->lev1_rej_penalt));
	offset += sizeof(ft->lev1_rej_penalt);
	stchar(&ft->main_rej_penalt,&buf[offset],sizeof(ft->main_rej_penalt));
	offset += sizeof(ft->main_rej_penalt);
	stchar(&ft->lev2_rej_penalt,&buf[offset],sizeof(ft->lev2_rej_penalt));
	offset += sizeof(ft->lev2_rej_penalt);
	stchar(&ft->lev3_rej_penalt,&buf[offset],sizeof(ft->lev3_rej_penalt));
	offset += sizeof(ft->lev3_rej_penalt);
	stchar(&ft->lev4_rej_penalt,&buf[offset],sizeof(ft->lev4_rej_penalt));
	offset += sizeof(ft->lev4_rej_penalt);
	stchar(&ft->lev5_rej_penalt,&buf[offset],sizeof(ft->lev5_rej_penalt));
	offset += sizeof(ft->lev5_rej_penalt);
	stchar(&ft->lev6_rej_penalt,&buf[offset],sizeof(ft->lev6_rej_penalt));
	offset += sizeof(ft->lev6_rej_penalt);
	stchar(&ft->lev7_rej_penalt,&buf[offset],sizeof(ft->lev7_rej_penalt));
	offset += sizeof(ft->lev7_rej_penalt);
	stchar(&ft->lev8_rej_penalt,&buf[offset],sizeof(ft->lev8_rej_penalt));
	offset += sizeof(ft->lev8_rej_penalt);
	stchar(&ft->bid_action,&buf[offset],sizeof(ft->bid_action));
	offset += sizeof(ft->bid_action);
	stint(ft->lev1_excl_time,&buf[offset]);
	offset += INTSIZE;
	stint(ft->main_excl_time,&buf[offset]);
	offset += INTSIZE;
	stint(ft->lev2_excl_time,&buf[offset]);
	offset += INTSIZE;
	stint(ft->lev3_excl_time,&buf[offset]);
	offset += INTSIZE;
	stint(ft->lev4_excl_time,&buf[offset]);
	offset += INTSIZE;
	stint(ft->lev5_excl_time,&buf[offset]);
	offset += INTSIZE;
	stint(ft->lev6_excl_time,&buf[offset]);
	offset += INTSIZE;
	stint(ft->lev7_excl_time,&buf[offset]);
	offset += INTSIZE;
	stint(ft->lev8_excl_time,&buf[offset]);
	offset += INTSIZE;
	stchar(&ft->no_personal,&buf[offset],sizeof(ft->no_personal));
	offset += sizeof(ft->no_personal);
	stchar(&ft->no_seals,&buf[offset],sizeof(ft->no_seals));
	offset += sizeof(ft->no_seals);
	stchar(&ft->auto_stand_flag,&buf[offset],sizeof(ft->auto_stand_flag));
	offset += sizeof(ft->auto_stand_flag);
	stchar(&ft->no_show_action,&buf[offset],sizeof(ft->no_show_action));
	offset += sizeof(ft->no_show_action);
	stchar(&ft->cancel_action,&buf[offset],sizeof(ft->cancel_action));
	offset += sizeof(ft->cancel_action);
	stchar(&ft->enr_post_action,&buf[offset],sizeof(ft->enr_post_action));
	offset += sizeof(ft->enr_post_action);
	stchar(&ft->hold_line_flag,&buf[offset],sizeof(ft->hold_line_flag));
	offset += sizeof(ft->hold_line_flag);
	stchar(&ft->zone_backup_lev,&buf[offset],sizeof(ft->zone_backup_lev));
	offset += sizeof(ft->zone_backup_lev);
	stint(ft->avl_call_q_time,&buf[offset]);
	offset += INTSIZE;

	stint(ft->veh_inqry_freq,&buf[offset]);
	offset += INTSIZE;

	stchar(&ft->cal_assign_info,&buf[offset],sizeof(ft->cal_assign_info));
	offset += sizeof(ft->cal_assign_info);

	stint(ft->zone_info_freq,&buf[offset]);
	offset += INTSIZE;

	stint(ft->zone_inqry_freq,&buf[offset]);
	offset += INTSIZE;

	stint(ft->post_freq,&buf[offset]);
	offset += INTSIZE;

	stint(ft->cond_post_freq,&buf[offset]);
	offset += INTSIZE;

	stint(ft->call_accept_per,&buf[offset]);
	offset += INTSIZE;
	stint(ft->break_duration,&buf[offset]);
	offset += INTSIZE;
	stint(ft->bid_duration,&buf[offset]);
	offset += INTSIZE;
	stint(ft->enr_post_lead,&buf[offset]);
	offset += INTSIZE;
	stint(ft->short_meter_dur,&buf[offset]);
	offset += INTSIZE;
	stint(ft->late_meter,&buf[offset]);
	offset += INTSIZE;
	stint(ft->max_rej_count,&buf[offset]);
	offset += INTSIZE;
	stint(ft->pers_lead_tm,&buf[offset]);
	offset += INTSIZE;
	stint(ft->no_acpt_penalty,&buf[offset]);
	offset += INTSIZE;


	stchar(&ft->when_enr_post,&buf[offset],sizeof(ft->when_enr_post));
	offset += sizeof(ft->when_enr_post);
	stint(ft->zone_stat_freq,&buf[offset]);
	offset += INTSIZE;
	stint(ft->max_nbr_drv_atr,&buf[offset]);
	offset += INTSIZE;
	stchar(&ft->drv_msg_approve,&buf[offset],sizeof(ft->drv_msg_approve));
	offset += sizeof(ft->drv_msg_approve);
	stint(ft->max_vehs_call,&buf[offset]);
	offset += INTSIZE;
	stfloat(ft->future_call_max,&buf[offset]);
	offset += FLOATSIZE;
	stint(ft->future_call_min,&buf[offset]);
	offset += INTSIZE;
	stchar(&ft->pass_rate_calc,&buf[offset],sizeof(ft->pass_rate_calc));
	offset += sizeof(ft->pass_rate_calc);
	stchar(&ft->dlv_rate_calc,&buf[offset],sizeof(ft->dlv_rate_calc));
	offset += sizeof(ft->dlv_rate_calc);
	stchar(&ft->subsc_review,&buf[offset],sizeof(ft->subsc_review));
	offset += sizeof(ft->subsc_review);
	stchar(&ft->greeting_line,&buf[offset],sizeof(ft->greeting_line));
	offset += sizeof(ft->greeting_line);
	stchar(ft->city_default,&buf[offset],sizeof(ft->city_default)-1);
	offset += sizeof(ft->city_default)-1;
	stchar(ft->phone_area_def,&buf[offset],sizeof(ft->phone_area_def)-1);
	offset += sizeof(ft->phone_area_def)-1;
	stint(ft->time_calls_freq,&buf[offset]);
	offset += INTSIZE;
	stint(ft->msg_rpt_freq,&buf[offset]);
	offset += INTSIZE;
	stchar(ft->audt_trail_port,&buf[offset],sizeof(ft->audt_trail_port)-1);
	offset += sizeof(ft->audt_trail_port)-1;
	stchar(ft->comments,&buf[offset],sizeof(ft->comments)-1);
	offset += sizeof(ft->comments)-1;
	stint(ft->pass_pri_def,&buf[offset]);
	offset += INTSIZE;
	stint(ft->del_pri_def,&buf[offset]);
	offset += INTSIZE;
	stint(ft->charge_pri_def,&buf[offset]);
	offset += INTSIZE;
	stint(ft->calls_pri_def,&buf[offset]);
	offset += INTSIZE;
	stint(ft->subsc_pri_def,&buf[offset]);
	offset += INTSIZE;
	stint(ft->typex_pri_def,&buf[offset]);
	offset += INTSIZE;
	stint(ft->wake_up_pri_def,&buf[offset]);
	offset += INTSIZE;
	stint(ft->high_rookie_pri,&buf[offset]);
	offset += INTSIZE;
	stchar(&ft->call_type_def,&buf[offset],sizeof(ft->call_type_def));
	offset += sizeof(ft->call_type_def);
	stchar(&ft->met_pass_call,&buf[offset],sizeof(ft->met_pass_call));
	offset += sizeof(ft->met_pass_call);
	stchar(&ft->met_del_call,&buf[offset],sizeof(ft->met_del_call));
	offset += sizeof(ft->met_del_call);
	stchar(&ft->met_typex_call,&buf[offset],sizeof(ft->met_typex_call));
	offset += sizeof(ft->met_typex_call);
	stint(ft->hist_search_def,&buf[offset]);
	offset += INTSIZE;

	stint(ft->time_call_freq,&buf[offset]);
	offset += INTSIZE;

	stint(ft->time_call_max,&buf[offset]);
	offset += INTSIZE;

	stint(ft->range_limit1,&buf[offset]);
	offset += INTSIZE;

	stchar(ft->estimate_text1,&buf[offset],sizeof(ft->estimate_text1)-1);
	offset += sizeof(ft->estimate_text1)-1;

	stint(ft->range_limit2,&buf[offset]);
	offset += INTSIZE;

	stchar(ft->estimate_text2,&buf[offset],sizeof(ft->estimate_text2)-1);
	offset += sizeof(ft->estimate_text2)-1;

	stint(ft->range_limit3,&buf[offset]);
	offset += INTSIZE;

	stchar(ft->estimate_text3,&buf[offset],sizeof(ft->estimate_text3)-1);
	offset += sizeof(ft->estimate_text3)-1;

	stint(ft->range_limit4,&buf[offset]);
	offset += INTSIZE;

	stchar(ft->estimate_text4,&buf[offset],sizeof(ft->estimate_text4)-1);
	offset += sizeof(ft->estimate_text4)-1;

	stint(ft->range_limit5,&buf[offset]);
	offset += INTSIZE;

	stchar(ft->estimate_text5,&buf[offset],sizeof(ft->estimate_text5)-1);
	offset += sizeof(ft->estimate_text5)-1;

	stint(ft->range_limit6,&buf[offset]);
	offset += INTSIZE;

	stchar(ft->estimate_text6,&buf[offset],sizeof(ft->estimate_text6)-1);
	offset += sizeof(ft->estimate_text6)-1;

	stint(ft->range_limit7,&buf[offset]);
	offset += INTSIZE;

	stchar(ft->estimate_text7,&buf[offset],sizeof(ft->estimate_text7)-1);
	offset += sizeof(ft->estimate_text7)-1;

	stint(ft->range_limit8,&buf[offset]);
	offset += INTSIZE;

	stchar(ft->estimate_text8,&buf[offset],sizeof(ft->estimate_text8)-1);
	offset += sizeof(ft->estimate_text8)-1;

	stchar(ft->def_est_text,&buf[offset],sizeof(ft->def_est_text)-1);
	offset += sizeof(ft->def_est_text)-1;

	stint(ft->max_nbr_veh_atr,&buf[offset]);
	offset += INTSIZE;

	buf[offset] = ft->active;
	offset += sizeof(ft->active);

	stint(ft->wakeup_lead_time,&buf[offset]);
	offset += INTSIZE;

	stint(ft->missing_receipt_timeout,&buf[offset]);
	offset += INTSIZE;

	stint(ft->restricted_time_call_enabled,&buf[offset]);
	offset += INTSIZE;

	stint(ft->time_call_time_period_len,&buf[offset]);
	offset += INTSIZE;

	stint(ft->missing_receipt_retries,&buf[offset]);
	offset += INTSIZE;

	stint(ft->inactive_retry_interval,&buf[offset]);
	offset += INTSIZE;

	stint(ft->inactive_time_out_period,&buf[offset]);
	offset += INTSIZE;

	buf[offset] = ft->cal_offer_info;
	offset += sizeof(ft->cal_offer_info);

	buf[offset] = ft->approach_tariff;
	offset += sizeof(ft->approach_tariff);

	stint(ft->num_levels_to_compute_stats,&buf[offset]);
	offset += INTSIZE;

	stint(ft->stats_compute_freq,&buf[offset]);
	offset += INTSIZE;

	stint(ft->handle_system_assigned_calls,&buf[offset]);
	offset += INTSIZE;

	stint(ft->itm_missing_receipts_max,&buf[offset]);
	offset += INTSIZE;

	stchar(&ft->nak_offer_penal,&buf[offset],sizeof(ft->nak_offer_penal));
	offset += sizeof(ft->nak_offer_penal);

	stint(ft->auto_flag_time,&buf[offset]);
	offset += INTSIZE;

	stint(ft->time_int_req,&buf[offset]);
	offset += INTSIZE;

	stint(ft->time_calls_lead_supp, &buf[offset]);
	offset += INTSIZE;

	stchar(&ft->FR_distance_filter,&buf[offset],sizeof(ft->FR_distance_filter));
	offset += sizeof(ft->FR_distance_filter);

#ifdef NAMERICA

	stchar(&ft->send_dest_addr, &buf[offset], sizeof(ft->send_dest_addr));
	offset += sizeof(ft->send_dest_addr);
	
	stint(ft->unassgnd_look_ahead,&buf[offset]);
	offset += INTSIZE;

	stchar(&ft->send_dest_offer,&buf[offset],sizeof(ft->send_dest_offer));
	offset += sizeof(ft->send_dest_offer);

	stchar(&ft->send_pckup_cmt,&buf[offset],sizeof(ft->send_pckup_cmt));
	offset += sizeof(ft->send_pckup_cmt);
	
	stchar(&ft->send_rate,&buf[offset],sizeof(ft->send_rate));
	offset += sizeof(ft->send_rate);
#endif
	stchar(&ft->gps_state,&buf[offset],sizeof(ft->gps_state));
	offset += sizeof(ft->gps_state);

	stint(ft->gps_time,&buf[offset]);
	offset += INTSIZE;

	stlong(ft->gps_distance_ceiling, &buf[offset]);
	offset += LONGSIZE;

	stchar(&ft->send_call_type,&buf[offset],sizeof(ft->send_call_type));
	offset += sizeof(ft->send_call_type);

	stint(ft->auto_reject_time, &buf[offset]);
	offset += INTSIZE;

	stint(ft->m_action_timer, &buf[offset]);
	offset += INTSIZE;

	stint(ft->m_action_retry, &buf[offset]);
	offset += INTSIZE;

	stint(ft->m_action_penalty, &buf[offset]);
	offset += INTSIZE;

	stint(ft->canx_to_moff_time, &buf[offset]);
	offset += INTSIZE;

	stdbl(ft->gps_max_x, &buf[offset]);
	offset += DOUBLESIZE;

	stdbl(ft->gps_min_x, &buf[offset]);
	offset += DOUBLESIZE;

	stdbl(ft->gps_max_y, &buf[offset]);
	offset += DOUBLESIZE;

	stdbl(ft->gps_min_y, &buf[offset]);
	offset += DOUBLESIZE;

	stint(ft->gps_coord_sys, &buf[offset]);
	offset += INTSIZE;

	stchar(&ft->gps_book,&buf[offset],sizeof(ft->gps_book));
	offset += sizeof(ft->gps_book);


	stint(ft->tc_restrict_interval, &buf[offset]);
	offset += INTSIZE;

	stint(ft->ns_short, &buf[offset]);
	offset += INTSIZE;

        stint(ft->emerg_range, &buf[offset]);
        offset += INTSIZE;

        stchar(&ft->enroute_break,&buf[offset],sizeof(ft->enroute_break));
        offset += sizeof(ft->enroute_break);

        stchar(&ft->ilink_active,&buf[offset],sizeof(ft->ilink_active));
        offset += sizeof(ft->ilink_active);

        stint(ft->ilink_timeout, &buf[offset]);
        offset += INTSIZE;

        stchar(&ft->load_unload,&buf[offset],sizeof(ft->load_unload));
        offset += sizeof(ft->load_unload);	

	stint(ft->cbook_time, &buf[offset]);
        offset += INTSIZE;

	stint(ft->stand_ntfy_interval, &buf[offset]);
        offset += INTSIZE;
	
	stint(ft->stand_ntfy_life, &buf[offset]);
        offset += INTSIZE;

	
	
}

/* Load program variables from C-ISAM data record */
void
unfill_fl(req_buf, buf)
     char *req_buf;
     char *buf;
{
	register int   offset = 0;
	struct  cisam_fl  *ft;

	ft = (struct cisam_fl*)req_buf;

	ft->id = buf[offset];
	offset += CHARSIZE;

	ft->match_algorithm = buf[offset];
	offset += CHARSIZE;

	ft->nbr = ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(ft->name)-1,ft->name);
	offset += sizeof(ft->name)-1;

	ft->drv_regular_pri = buf[offset];
	offset += sizeof(ft->drv_regular_pri);

	ft->drv_no_show_pri = buf[offset];
	offset += sizeof(ft->drv_no_show_pri);

	ft->drv_local_pri = buf[offset];
	offset += sizeof(ft->drv_local_pri);

	ft->drv_cancel_pri = buf[offset];
	offset += sizeof(ft->drv_cancel_pri);

	ft->local_calls_dur = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->call_offer_proc = buf[offset];
	offset += sizeof(ft->call_offer_proc);

	ft->no_acpt_penalt = buf[offset];
	offset += sizeof(ft->no_acpt_penalt);

	ft->lev1_rej_penalt = buf[offset];
	offset += sizeof(ft->lev1_rej_penalt);

	ft->main_rej_penalt = buf[offset];
	offset += sizeof(ft->main_rej_penalt);

	ft->lev2_rej_penalt = buf[offset];
	offset += sizeof(ft->lev3_rej_penalt);

	ft->lev3_rej_penalt = buf[offset];
	offset += sizeof(ft->lev3_rej_penalt);

	ft->lev4_rej_penalt = buf[offset];
	offset += sizeof(ft->lev4_rej_penalt);

	ft->lev5_rej_penalt = buf[offset];
	offset += sizeof(ft->lev5_rej_penalt);

	ft->lev6_rej_penalt = buf[offset];
	offset += sizeof(ft->lev6_rej_penalt);

	ft->lev7_rej_penalt = buf[offset];
	offset += sizeof(ft->lev7_rej_penalt);

	ft->lev8_rej_penalt = buf[offset];
	offset += sizeof(ft->lev8_rej_penalt);

	ft->bid_action = buf[offset];
	offset += sizeof(ft->bid_action);

	ft->lev1_excl_time = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->main_excl_time = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->lev2_excl_time = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->lev3_excl_time = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->lev4_excl_time = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->lev5_excl_time = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->lev6_excl_time = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->lev7_excl_time= ldint(&buf[offset]);
	offset += INTSIZE;

	ft->lev8_excl_time= ldint(&buf[offset]);
	offset += INTSIZE;

	ft->no_personal = buf[offset];
	offset += sizeof(ft->no_personal);

	ft->no_seals = buf[offset];
	offset += sizeof(ft->no_seals);

	ft->auto_stand_flag = buf[offset];
	offset += sizeof(ft->auto_stand_flag);

	ft->no_show_action = buf[offset];
	offset += sizeof(ft->no_show_action);

	ft->cancel_action = buf[offset];
	offset += sizeof(ft->cancel_action);

	ft->enr_post_action = buf[offset];
	offset += sizeof(ft->enr_post_action);

	ft->hold_line_flag = buf[offset];
	offset += sizeof(ft->hold_line_flag);

	ft->zone_backup_lev = buf[offset];
	offset += sizeof(ft->zone_backup_lev);

	ft->avl_call_q_time = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->veh_inqry_freq = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->cal_assign_info = buf[offset];
	offset += sizeof(ft->cal_assign_info);

	ft->zone_info_freq = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->zone_inqry_freq = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->post_freq = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->cond_post_freq = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->call_accept_per = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->break_duration = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->bid_duration = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->enr_post_lead = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->short_meter_dur = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->late_meter = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->max_rej_count = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->pers_lead_tm = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->no_acpt_penalty = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->when_enr_post = buf[offset];
	offset += sizeof(ft->when_enr_post);

	ft->zone_stat_freq = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->max_nbr_drv_atr = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->drv_msg_approve = buf[offset];
	offset += sizeof(ft->drv_msg_approve);

	ft->max_vehs_call = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->future_call_max = ldfloat(&buf[offset]); 
	offset += FLOATSIZE;

	ft->future_call_min = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->pass_rate_calc = buf[offset];
	offset += sizeof(ft->pass_rate_calc);

	ft->dlv_rate_calc = buf[offset];
	offset += sizeof(ft->dlv_rate_calc);

	ft->subsc_review = buf[offset];
	offset += sizeof(ft->subsc_review);

	ft->greeting_line = buf[offset];
	offset += sizeof(ft->greeting_line);

	ldchar(&buf[offset],sizeof(ft->city_default)-1,ft->city_default);
	offset += sizeof(ft->city_default)-1;

	ldchar(&buf[offset],sizeof(ft->phone_area_def)-1,ft->phone_area_def);
	offset += sizeof(ft->phone_area_def)-1;

	ft->time_calls_freq = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->msg_rpt_freq = ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(ft->audt_trail_port)-1,ft->audt_trail_port);
	offset += sizeof(ft->audt_trail_port)-1;

	ldchar(&buf[offset],sizeof(ft->comments)-1,ft->comments);
	offset += sizeof(ft->comments)-1;

	ft->pass_pri_def = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->del_pri_def = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->charge_pri_def = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->calls_pri_def = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->subsc_pri_def = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->typex_pri_def = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->wake_up_pri_def = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->high_rookie_pri = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->call_type_def = buf[offset];
	offset += sizeof(ft->call_type_def);

	ft->met_pass_call = buf[offset];
	offset += sizeof(ft->met_pass_call);

	ft->met_del_call = buf[offset];
	offset += sizeof(ft->met_del_call);

	ft->met_typex_call = buf[offset];
	offset += sizeof(ft->met_typex_call);

	ft->hist_search_def = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->time_call_freq = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->time_call_max = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->range_limit1= ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(ft->estimate_text1)-1,ft->estimate_text1);
	offset += sizeof(ft->estimate_text1)-1;

	ft->range_limit2= ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(ft->estimate_text2)-1,ft->estimate_text2);
	offset += sizeof(ft->estimate_text2)-1;

	ft->range_limit3= ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(ft->estimate_text3)-1,ft->estimate_text3);
	offset += sizeof(ft->estimate_text3)-1;

	ft->range_limit4= ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(ft->estimate_text4)-1,ft->estimate_text4);
	offset += sizeof(ft->estimate_text4)-1;

	ft->range_limit5= ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(ft->estimate_text5)-1,ft->estimate_text5);
	offset += sizeof(ft->estimate_text5)-1;

	ft->range_limit6= ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(ft->estimate_text6)-1,ft->estimate_text6);
	offset += sizeof(ft->estimate_text6)-1;

	ft->range_limit7= ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(ft->estimate_text7)-1,ft->estimate_text7);
	offset += sizeof(ft->estimate_text7)-1;

	ft->range_limit8= ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(ft->estimate_text8)-1,ft->estimate_text8);
	offset += sizeof(ft->estimate_text8)-1;

	ldchar(&buf[offset],sizeof(ft->def_est_text)-1,ft->def_est_text);
	offset += sizeof(ft->def_est_text)-1;

	ft->max_nbr_veh_atr = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->active = buf[offset];
	offset += sizeof(ft->active);

	ft->wakeup_lead_time = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->missing_receipt_timeout = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->restricted_time_call_enabled = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->time_call_time_period_len = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->missing_receipt_retries = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->inactive_retry_interval = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->inactive_time_out_period = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->cal_offer_info = buf[offset];
	offset += sizeof(ft->cal_offer_info);

	ft->approach_tariff = buf[offset];
	offset += sizeof(ft->approach_tariff);

	ft->num_levels_to_compute_stats = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->stats_compute_freq = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->handle_system_assigned_calls = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->itm_missing_receipts_max = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->nak_offer_penal = buf[offset];
	offset += sizeof(ft->nak_offer_penal);

	ft->auto_flag_time= ldint(&buf[offset]);
	offset += INTSIZE;

	ft->time_int_req = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->time_calls_lead_supp = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->FR_distance_filter = buf[offset];
	offset += sizeof(ft->FR_distance_filter);

#ifdef NAMERICA
	ft->send_dest_addr = buf[offset];
	offset += sizeof(ft->send_dest_addr);

	ft->unassgnd_look_ahead = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->send_dest_offer = buf[offset];
	offset += sizeof(ft->send_dest_offer);

	ft->send_pckup_cmt = buf[offset];
	offset += sizeof(ft->send_pckup_cmt);

	ft->send_rate = buf[offset];
	offset += sizeof(ft->send_rate);		
#endif
	ft->gps_state = buf[offset];
	offset += sizeof(ft->gps_state);

	ft->gps_time = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->gps_distance_ceiling = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ft->send_call_type = buf[offset];
	offset += sizeof(ft->send_call_type);

	ft->auto_reject_time = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->m_action_timer = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->m_action_retry = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->m_action_penalty = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->canx_to_moff_time = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->gps_max_x = lddbl(&buf[offset]);
	offset += DOUBLESIZE;

	ft->gps_min_x = lddbl(&buf[offset]);
	offset += DOUBLESIZE;

	ft->gps_max_y = lddbl(&buf[offset]);
	offset += DOUBLESIZE;

	ft->gps_min_y = lddbl(&buf[offset]);
	offset += DOUBLESIZE;	

	ft->gps_coord_sys = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->gps_book = buf[offset];
	offset += CHARSIZE;


	ft->tc_restrict_interval = ldint(&buf[offset]);
	offset += INTSIZE;

	ft->ns_short = ldint(&buf[offset]);
	offset += INTSIZE;
        
        ft->emerg_range = ldint(&buf[offset]);
        offset += INTSIZE;

        ft->enroute_break = buf[offset];
        offset += CHARSIZE;

        ft->ilink_active = buf[offset];
        offset += CHARSIZE;

        ft->ilink_timeout = ldint(&buf[offset]);
        offset += INTSIZE;

        ft->load_unload = buf[offset];
        offset += CHARSIZE;

        ft->cbook_time = ldint(&buf[offset]);
        offset += INTSIZE;

        ft->stand_ntfy_interval = ldint(&buf[offset]);
        offset += INTSIZE;

        ft->stand_ntfy_life = ldint(&buf[offset]);
        offset += INTSIZE;		
	
}

