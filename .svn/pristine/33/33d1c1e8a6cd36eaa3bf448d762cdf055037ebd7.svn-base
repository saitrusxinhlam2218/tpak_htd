static char sz__FILE__[]="@(#)fill_cl.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_cl.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:37:26
 * @(#)  Last delta: 12/2/94 at 11:16:10
 * @(#)  SCCS File: /taxi/sccs/s.fill_cl.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:fill_cl.c	10.1.0.0"
#define KEY_SRC

#include <time.h>
#include "taxipak.h"
#include "mads_types.h"
#include "Object.h"
#include "Call_public.h"
#include "Call_private.h"
#include "taxi_db.h"
#include "Call_db.h"
#include "Shift_db.h"

void
fill_cl_key( req_buf, buf )
     char *req_buf;
     char *buf;
{
  struct cisam_cl *cl;

  cl = (struct cisam_cl*)req_buf;

  stlong(cl->nbr, &buf[CL_NBR]);

  stlong(cl->grouped_with, &buf[CL_GROUPED_WITH]);

  stchar(&cl->fleet,&buf[CL_FLEET],sizeof(cl->fleet));

  stchar(cl->pckup_str_key,&buf[CL_PCKUP_STR_KEY],sizeof(cl->pckup_str_key)-1);

  stint(cl->pckup_zone,&buf[CL_SHIFT_PCKUP_ZONE]);  /* SHIFT file */

  stchar(cl->phone_nbr,&buf[CL_PHONE_NBR],sizeof(cl->phone_nbr)-1);

  stint(cl->pri_status,&buf[CL_PRI_STATUS]);

  stchar(cl->charge_nbr,&buf[CL_CHARGE_NBR],sizeof(cl->charge_nbr)-1);

  stlong(cl->due_date_time,&buf[CL_DUE_DATE_TIME]);

  stint(cl->veh_nbr,&buf[CL_VEH_NBR]);

  stlong(cl->drv_id,&buf[CL_DRV_ID]);

  stint(cl->dispatched_by,&buf[CL_SHIFT_DISPATCHED_BY]);

  stchar(cl->pass_name_key,&buf[CL_PASS_NAME_KEY],sizeof(cl->pass_name_key)-1);

	stlong(cl->ext_nbr, &buf[CL_EXT_NBR]);
}

void
fill_cl( req_buf, buf, action)
     char *req_buf;
     char *buf;
     int   action;
{
  struct cisam_cl *cl;
  register int  offset = 0;
  long serial_nbr = 0;

  cl = (struct cisam_cl*)req_buf;
  
  if(action == ADD_REC)
    {
      isuniqueid(Db_files[CALL_FILE_ID].fd,&serial_nbr);

      if ( ( serial_nbr == 0 ) && ( action == ADD_REC ) )
	{
	  sleep(1);
	  isuniqueid(Db_files[CALL_FILE_ID].fd, &serial_nbr);
	}
      cl->nbr = (int)serial_nbr;
    }
  
  fill_shift(req_buf, buf, action);
}

void
fill_shift(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int   action;
{
int  cc;
  struct cisam_cl *cl;
  register int  offset = 0;

cl = (struct cisam_cl*)req_buf;

stlong(cl->nbr,&buf[offset]);
offset += LONGSIZE;

stlong(cl->grouped_with,&buf[offset]);
offset += LONGSIZE;

stchar(&cl->pos_in_group,&buf[offset],sizeof(cl->pos_in_group));
offset += sizeof(cl->pos_in_group); 

stchar(&cl->nbr_in_group,&buf[offset],sizeof(cl->nbr_in_group));
offset += sizeof(cl->nbr_in_group); 

stchar(&cl->fleet,&buf[offset],sizeof(cl->fleet));
offset += sizeof(cl->fleet); 

stchar(cl->extended_type,&buf[offset],sizeof(cl->extended_type)-1);
offset += sizeof(cl->extended_type)-1; 

stchar(cl->pckup_adr_cmnt,&buf[offset],sizeof(cl->pckup_adr_cmnt)-1);
offset += sizeof(cl->pckup_adr_cmnt)-1; 

stchar(cl->general_cmnt,&buf[offset],sizeof(cl->general_cmnt)-1);
offset += sizeof(cl->general_cmnt)-1; 

stlong(cl->pckup_str_nbr,&buf[offset]);
offset += LONGSIZE;

stchar(&cl->pckup_str_suf,&buf[offset],sizeof(cl->pckup_str_suf));
offset += sizeof(cl->pckup_str_suf); 

stchar(cl->pckup_pre_dir,&buf[offset],sizeof(cl->pckup_pre_dir)-1);
offset += sizeof(cl->pckup_pre_dir)-1; 

stchar(cl->pckup_str_name,&buf[offset],sizeof(cl->pckup_str_name)-1);
offset += sizeof(cl->pckup_str_name)-1; 

stchar(cl->pckup_str_key,&buf[offset],sizeof(cl->pckup_str_key)-1);
offset += sizeof(cl->pckup_str_key)-1; 

stchar(cl->pckup_str_type,&buf[offset],sizeof(cl->pckup_str_type)-1);
offset += sizeof(cl->pckup_str_type)-1; 

stchar(cl->pckup_post_dir,&buf[offset],sizeof(cl->pckup_post_dir)-1);
offset += sizeof(cl->pckup_post_dir)-1; 

stchar(cl->intr_pre_dir,&buf[offset],sizeof(cl->intr_pre_dir)-1);
offset += sizeof(cl->intr_pre_dir)-1; 

stchar(cl->intr_str_name,&buf[offset],sizeof(cl->intr_str_name)-1);
offset += sizeof(cl->intr_str_name)-1; 

stchar(cl->intr_str_type,&buf[offset],sizeof(cl->intr_str_type)-1);
offset += sizeof(cl->intr_str_type)-1; 

stchar(cl->intr_post_dir,&buf[offset],sizeof(cl->intr_post_dir)-1);
offset += sizeof(cl->intr_post_dir)-1; 

stchar(cl->pckup_apt_room,&buf[offset],sizeof(cl->pckup_apt_room)-1);
offset += sizeof(cl->pckup_apt_room)-1; 

stchar(cl->pckup_city,&buf[offset],sizeof(cl->pckup_city)-1);
offset += sizeof(cl->pckup_city)-1; 

stint(cl->pckup_zone,&buf[offset]);
offset += INTSIZE;

stchar(cl->passenger_name,&buf[offset],sizeof(cl->passenger_name)-1);
offset += sizeof(cl->passenger_name)-1; 

stchar(cl->phone_area,&buf[offset],sizeof(cl->phone_area)-1);
offset += sizeof(cl->phone_area)-1; 

stchar(cl->phone_nbr,&buf[offset],sizeof(cl->phone_nbr)-1);
offset += sizeof(cl->phone_nbr)-1;
 
stchar(cl->phone_ext,&buf[offset],sizeof(cl->phone_ext)-1);
offset += sizeof(cl->phone_ext)-1; 

stchar(cl->dest_adr_cmnt,&buf[offset],sizeof(cl->dest_adr_cmnt)-1);
offset += sizeof(cl->dest_adr_cmnt)-1; 

stlong(cl->dest_str_nbr,&buf[offset]);
offset += LONGSIZE;

stchar(&cl->dest_str_suf,&buf[offset],sizeof(cl->dest_str_suf));
offset += sizeof(cl->dest_str_suf); 

stchar(cl->dest_pre_dir,&buf[offset],sizeof(cl->dest_pre_dir)-1);
offset += sizeof(cl->dest_pre_dir)-1; 

stchar(cl->dest_str_name,&buf[offset],sizeof(cl->dest_str_name)-1);
offset += sizeof(cl->dest_str_name)-1; 

stchar(cl->dest_str_type,&buf[offset],sizeof(cl->dest_str_type)-1);
offset += sizeof(cl->dest_str_type)-1; 

stchar(cl->dest_post_dir,&buf[offset],sizeof(cl->dest_post_dir)-1);
offset += sizeof(cl->dest_post_dir)-1; 

stchar(cl->dest_apt_room,&buf[offset],sizeof(cl->dest_apt_room)-1);
offset += sizeof(cl->dest_apt_room)-1; 

stchar(cl->dest_city,&buf[offset],sizeof(cl->dest_city)-1);
offset += sizeof(cl->dest_city)-1; 

stint(cl->dest_zone,&buf[offset]);
offset += INTSIZE;

stint(cl->personal_veh,&buf[offset]);
offset += INTSIZE;

stchar(&cl->personal_rqst,&buf[offset],sizeof(cl->personal_rqst));
offset += sizeof(cl->personal_rqst); 

stchar(&cl->veh_attr_flag,&buf[offset],sizeof(cl->veh_attr_flag));
offset += sizeof(cl->veh_attr_flag); 

stchar(cl->veh_attr,&buf[offset],sizeof(cl->veh_attr)-1);
offset += sizeof(cl->veh_attr)-1; 

stchar(&cl->drv_attr_flag,&buf[offset],sizeof(cl->drv_attr_flag));
offset += sizeof(cl->drv_attr_flag); 

stchar(cl->drv_attr,&buf[offset],sizeof(cl->drv_attr)-1);
offset += sizeof(cl->drv_attr)-1; 

stchar(&cl->nbr_of_vehs,&buf[offset],sizeof(cl->nbr_of_vehs));
offset += sizeof(cl->nbr_of_vehs);

stint(cl->priority,&buf[offset]);
offset += INTSIZE;

stint(cl->pri_status,&buf[offset]);
offset += INTSIZE;

stchar(cl->hold_line,&buf[offset],sizeof(cl->hold_line)-1);
offset += sizeof(cl->hold_line)-1; 

stchar(&cl->local,&buf[offset],sizeof(cl->local));
offset += sizeof(cl->local);

stchar(cl->charge_nbr,&buf[offset],sizeof(cl->charge_nbr)-1);
offset += sizeof(cl->charge_nbr)-1; 

stchar(cl->requested_by,&buf[offset],sizeof(cl->requested_by)-1);
offset += sizeof(cl->requested_by)-1; 

stchar(cl->person_to_see,&buf[offset],sizeof(cl->person_to_see)-1);
offset += sizeof(cl->person_to_see)-1; 

stchar(&cl->pod_required,&buf[offset],sizeof(cl->pod_required));
offset += sizeof(cl->pod_required);

stchar(cl->pod_name,&buf[offset],sizeof(cl->pod_name)-1);
offset += sizeof(cl->pod_name)-1; 

stchar(cl->date_in,&buf[offset],sizeof(cl->date_in)-1);
offset += sizeof(cl->date_in)-1; 

stchar(cl->time_in,&buf[offset],sizeof(cl->time_in)-1);
offset += sizeof(cl->time_in)-1; 

stint(cl->pod_time,&buf[offset]);
offset += INTSIZE;

stfloat(cl->call_rate,&buf[offset]);
offset += FLOATSIZE;

stlong(cl->date_time_in,&buf[offset]);
offset += LONGSIZE;

stchar(cl->est_wait_time,&buf[offset],sizeof(cl->est_wait_time)-1);
offset += sizeof(cl->est_wait_time)-1; 

stchar(cl->due_date,&buf[offset],sizeof(cl->due_date)-1);
offset += sizeof(cl->due_date)-1; 

stint(cl->entered_by,&buf[offset]);
offset += INTSIZE;

stint(cl->last_changed_by,&buf[offset]);
offset += INTSIZE;

stlong(cl->due_date_time,&buf[offset]);
offset += LONGSIZE;

stlong(cl->dispd_date_time,&buf[offset]);
offset += LONGSIZE;

stchar(cl->due_time,&buf[offset],sizeof(cl->due_time)-1);
offset += sizeof(cl->due_time)-1; 

stchar(cl->dispd_date,&buf[offset],sizeof(cl->dispd_date)-1);
offset += sizeof(cl->dispd_date)-1; 

stchar(cl->dispd_time,&buf[offset],sizeof(cl->dispd_time)-1);
offset += sizeof(cl->dispd_time)-1; 

stchar(cl->pckup_date,&buf[offset],sizeof(cl->pckup_date)-1);
offset += sizeof(cl->pckup_date)-1; 

stchar(cl->pckup_time,&buf[offset],sizeof(cl->pckup_time)-1);
offset += sizeof(cl->pckup_time)-1; 

stchar(cl->company_name,&buf[offset],sizeof(cl->company_name)-1);
offset += sizeof(cl->company_name)-1; 

stlong(cl->pckup_date_time,&buf[offset]);
offset += LONGSIZE;

stlong(cl->close_date_time,&buf[offset]);
offset += LONGSIZE;

stchar(cl->close_date,&buf[offset],sizeof(cl->close_date)-1);
offset += sizeof(cl->close_date)-1; 

stchar(cl->close_time,&buf[offset],sizeof(cl->close_time)-1);
offset += sizeof(cl->close_time)-1; 

stchar(cl->approve_code,&buf[offset],sizeof(cl->approve_code)-1);
offset += sizeof(cl->approve_code)-1; 

stfloat(cl->fare_amount,&buf[offset]);
offset += FLOATSIZE;

stlong(cl->subsc_call_nbr,&buf[offset]);
offset += LONGSIZE;

stint(cl->veh_nbr,&buf[offset]);
offset += INTSIZE;

stlong(cl->drv_id,&buf[offset]);
offset += LONGSIZE;

stint(cl->dispatched_by,&buf[offset]);
offset += INTSIZE;

stlong(cl->expire_date,&buf[offset]);
offset += LONGSIZE;

stlong(cl->approve_date,&buf[offset]);
offset += LONGSIZE;

stint(cl->approve_by,&buf[offset]);
offset += INTSIZE;

stchar(cl->ac_addt1_info1,&buf[offset],sizeof(cl->ac_addt1_info1)-1);
offset += sizeof(cl->ac_addt1_info1)-1; 

stchar(cl->ac_addt1_info2,&buf[offset],sizeof(cl->ac_addt1_info2)-1);
offset += sizeof(cl->ac_addt1_info2)-1; 

stchar(cl->driver_msg,&buf[offset],sizeof(cl->driver_msg)-1);
offset += sizeof(cl->driver_msg)-1; 

stchar(cl->pckup_pmap,&buf[offset],sizeof(cl->pckup_pmap)-1);
offset += sizeof(cl->pckup_pmap)-1; 

stchar(cl->dest_dmap,&buf[offset],sizeof(cl->dest_dmap)-1);
offset += sizeof(cl->dest_dmap)-1; 

stchar(cl->status_bits,&buf[offset],sizeof(cl->status_bits)-1);
offset += sizeof(cl->status_bits)-1; 

stchar(cl->zone_desc,&buf[offset],sizeof(cl->zone_desc)-1);
offset += sizeof(cl->zone_desc)-1; 

stchar(&cl->screen_type,&buf[offset],sizeof(cl->screen_type));
offset += sizeof(cl->screen_type); 

stchar(cl->status,&buf[offset],sizeof(cl->status)-1);
offset += sizeof(cl->status)-1; 

stchar(cl->pass_name_key,&buf[offset],sizeof(cl->pass_name_key)-1);
offset += sizeof(cl->pass_name_key)-1; 

stlong(cl->dispatch_entry_tm,&buf[offset]);
offset += LONGSIZE;

stchar(cl->phone_number,&buf[offset],sizeof(cl->phone_number)-1);
offset += sizeof(cl->phone_number)-1; 

stchar(&cl->advise_arrival,&buf[offset],sizeof(cl->advise_arrival));
offset += sizeof(cl->advise_arrival);

#ifndef NAMERICA
stlong(cl->ext_nbr,&buf[offset]);
offset += LONGSIZE;

stchar(&cl->offer_type,&buf[offset],sizeof(cl->offer_type));
offset += sizeof(cl->offer_type);

stchar(&cl->detail_type,&buf[offset],sizeof(cl->detail_type));
offset += sizeof(cl->detail_type);

stchar(&cl->call_msg_nbr,&buf[offset],sizeof(cl->call_msg_nbr));
offset += sizeof(cl->call_msg_nbr);

#else
stchar(&cl->send_destination,&buf[offset],sizeof(cl->send_destination));
offset += sizeof(cl->send_destination);

stchar(&cl->send_rate,&buf[offset],sizeof(cl->send_rate));
offset += sizeof(cl->send_rate);

#endif

stchar(cl->invoice_nbr1,&buf[offset], sizeof(cl->invoice_nbr1)-1);
offset += sizeof(cl->invoice_nbr1) - 1;

stchar(cl->invoice_nbr2,&buf[offset], sizeof(cl->invoice_nbr2)-1);
offset += sizeof(cl->invoice_nbr2) - 1;

stchar(cl->invoice_nbr3,&buf[offset], sizeof(cl->invoice_nbr3)-1);
offset += sizeof(cl->invoice_nbr3) - 1;

stchar(cl->invoice_nbr4,&buf[offset], sizeof(cl->invoice_nbr4)-1);
offset += sizeof(cl->invoice_nbr4) - 1;

stchar(cl->ct_adr_cmnt, &buf[offset], sizeof(cl->ct_adr_cmnt)-1);
offset += sizeof(cl->ct_adr_cmnt) - 1;

#ifdef NAMERICA
 cl->via_addresses = 0;
#endif

stint(cl->via_addresses,&buf[offset]);
offset += INTSIZE;

stfloat(cl->vat, &buf[offset]);
offset += FLOATSIZE;

stdbl(cl->gps_lat, &buf[offset]);
offset += DOUBLESIZE;

stdbl(cl->gps_long, &buf[offset]);
offset += DOUBLESIZE;

 stchar(&cl->cash_account, &buf[offset],sizeof(cl->cash_account));
 offset += sizeof(cl->cash_account);

stdbl(cl->dest_gpsx, &buf[offset]);
offset += DOUBLESIZE;

stdbl(cl->dest_gpsy, &buf[offset]);
offset += DOUBLESIZE;

}


void
unfill_cl( req_buf, buf )
     char *req_buf;
     char *buf;
{
  struct cisam_cl *cl;
  register int  offset = 0;
  char temp_attr[65];
  short code = 0;

  cl = (struct cisam_cl*)req_buf;

cl->nbr = ldlong(&buf[offset]);
offset += LONGSIZE;

cl->grouped_with = ldlong(&buf[offset]);
offset += LONGSIZE;

cl->pos_in_group = buf[offset];
offset += sizeof(cl->pos_in_group); 

cl->nbr_in_group = buf[offset];
offset += sizeof(cl->nbr_in_group); 

cl->fleet = buf[offset];
offset += sizeof(cl->fleet); 

ldchar(&buf[offset],sizeof(cl->extended_type)-1,cl->extended_type);
offset += sizeof(cl->extended_type)-1; 

ldchar(&buf[offset],sizeof(cl->pckup_adr_cmnt)-1,cl->pckup_adr_cmnt);
offset += sizeof(cl->pckup_adr_cmnt)-1; 

ldchar(&buf[offset],sizeof(cl->general_cmnt)-1,cl->general_cmnt);
offset += sizeof(cl->general_cmnt)-1; 

cl->pckup_str_nbr = ldlong(&buf[offset]);
offset += LONGSIZE;

cl->pckup_str_suf = buf[offset];
offset += sizeof(cl->pckup_str_suf); 

ldchar(&buf[offset],sizeof(cl->pckup_pre_dir)-1,cl->pckup_pre_dir);
offset += sizeof(cl->pckup_pre_dir)-1; 

ldchar(&buf[offset],sizeof(cl->pckup_str_name)-1,cl->pckup_str_name);
offset += sizeof(cl->pckup_str_name)-1; 

ldchar(&buf[offset],sizeof(cl->pckup_str_key)-1,cl->pckup_str_key);
offset += sizeof(cl->pckup_str_key)-1; 

ldchar(&buf[offset],sizeof(cl->pckup_str_type)-1,cl->pckup_str_type);
offset += sizeof(cl->pckup_str_type)-1; 

ldchar(&buf[offset],sizeof(cl->pckup_post_dir)-1,cl->pckup_post_dir);
offset += sizeof(cl->pckup_post_dir)-1; 

ldchar(&buf[offset],sizeof(cl->intr_pre_dir)-1,cl->intr_pre_dir);
offset += sizeof(cl->intr_pre_dir)-1; 

ldchar(&buf[offset],sizeof(cl->intr_str_name)-1,cl->intr_str_name);
offset += sizeof(cl->intr_str_name)-1; 

ldchar(&buf[offset],sizeof(cl->intr_str_type)-1,cl->intr_str_type);
offset += sizeof(cl->intr_str_type)-1; 

ldchar(&buf[offset],sizeof(cl->intr_post_dir)-1,cl->intr_post_dir);
offset += sizeof(cl->intr_post_dir)-1; 

ldchar(&buf[offset],sizeof(cl->pckup_apt_room)-1,cl->pckup_apt_room);
offset += sizeof(cl->pckup_apt_room)-1; 

ldchar(&buf[offset],sizeof(cl->pckup_city)-1,cl->pckup_city);
offset += sizeof(cl->pckup_city)-1; 

cl->pckup_zone = ldint(&buf[offset]);
offset += INTSIZE;

ldchar(&buf[offset],sizeof(cl->passenger_name)-1,cl->passenger_name);
offset += sizeof(cl->passenger_name)-1; 

ldchar(&buf[offset],sizeof(cl->phone_area)-1,cl->phone_area);
offset += sizeof(cl->phone_area)-1; 

ldchar(&buf[offset],sizeof(cl->phone_nbr)-1,cl->phone_nbr);
offset += sizeof(cl->phone_nbr)-1;
 
ldchar(&buf[offset],sizeof(cl->phone_ext)-1,cl->phone_ext);
offset += sizeof(cl->phone_ext)-1; 

ldchar(&buf[offset],sizeof(cl->dest_adr_cmnt)-1,cl->dest_adr_cmnt);
offset += sizeof(cl->dest_adr_cmnt)-1; 

cl->dest_str_nbr= ldlong(&buf[offset]);
offset += LONGSIZE;

cl->dest_str_suf = buf[offset];
offset += sizeof(cl->dest_str_suf); 

ldchar(&buf[offset],sizeof(cl->dest_pre_dir)-1,cl->dest_pre_dir);
offset += sizeof(cl->dest_pre_dir)-1; 

ldchar(&buf[offset],sizeof(cl->dest_str_name)-1,cl->dest_str_name);
offset += sizeof(cl->dest_str_name)-1; 

ldchar(&buf[offset],sizeof(cl->dest_str_type)-1,cl->dest_str_type);
offset += sizeof(cl->dest_str_type)-1; 

ldchar(&buf[offset],sizeof(cl->dest_post_dir)-1,cl->dest_post_dir);
offset += sizeof(cl->dest_post_dir)-1; 

ldchar(&buf[offset],sizeof(cl->dest_apt_room)-1,cl->dest_apt_room);
offset += sizeof(cl->dest_apt_room)-1; 

ldchar(&buf[offset],sizeof(cl->dest_city)-1,cl->dest_city);
offset += sizeof(cl->dest_city)-1; 

cl->dest_zone =ldint(&buf[offset]);
offset += INTSIZE;

cl->personal_veh = ldint(&buf[offset]);
offset += INTSIZE;

cl->personal_rqst = buf[offset];
offset += sizeof(cl->personal_rqst); 

cl->veh_attr_flag = buf[offset];
offset += sizeof(cl->veh_attr_flag); 

ldchar(&buf[offset],sizeof(cl->veh_attr)-1,temp_attr); /* Fill attribute string with spaces */
sprintf(cl->veh_attr,"%-64.64s",temp_attr);             /* as all 64 characters are needed in all TaxiPak routines */
offset += sizeof(cl->veh_attr)-1; 

cl->drv_attr_flag = buf[offset];
offset += sizeof(cl->drv_attr_flag); 

ldchar(&buf[offset],sizeof(cl->drv_attr)-1,temp_attr); /* Fill attribute string with spaces */
sprintf(cl->drv_attr,"%-64.64s",temp_attr);             /* as all 64 characters are needed in all TaxiPak routines */
offset += sizeof(cl->drv_attr)-1;

cl->nbr_of_vehs = buf[offset];
offset += sizeof(cl->nbr_of_vehs);

cl->priority = ldint(&buf[offset]);
offset += INTSIZE;

cl->pri_status = ldint(&buf[offset]);
offset += INTSIZE;

ldchar(&buf[offset],sizeof(cl->hold_line)-1,cl->hold_line);
offset += sizeof(cl->hold_line)-1; 

cl->local = buf[offset];
offset += sizeof(cl->local);

ldchar(&buf[offset],sizeof(cl->charge_nbr)-1,cl->charge_nbr);
offset += sizeof(cl->charge_nbr)-1; 

ldchar(&buf[offset],sizeof(cl->requested_by)-1,cl->requested_by);
offset += sizeof(cl->requested_by)-1; 

ldchar(&buf[offset],sizeof(cl->person_to_see)-1,cl->person_to_see);
offset += sizeof(cl->person_to_see)-1; 

cl->pod_required = buf[offset];
offset += sizeof(cl->pod_required);

ldchar(&buf[offset],sizeof(cl->pod_name)-1,cl->pod_name);
offset += sizeof(cl->pod_name)-1; 

ldchar(&buf[offset],sizeof(cl->date_in)-1,cl->date_in);
offset += sizeof(cl->date_in)-1; 

ldchar(&buf[offset],sizeof(cl->time_in)-1,cl->time_in);
offset += sizeof(cl->time_in)-1; 

cl->pod_time = ldint(&buf[offset]);
offset += INTSIZE;

cl->call_rate = ldfltnull(&buf[offset], &code);
offset += FLOATSIZE;

cl->date_time_in = ldlong(&buf[offset]);
offset += LONGSIZE;

ldchar(&buf[offset],sizeof(cl->est_wait_time)-1,cl->est_wait_time);
offset += sizeof(cl->est_wait_time)-1; 

ldchar(&buf[offset],sizeof(cl->due_date)-1,cl->due_date);
offset += sizeof(cl->due_date)-1; 

cl->entered_by = ldint(&buf[offset]);
offset += INTSIZE;

cl->last_changed_by = ldint(&buf[offset]);
offset += INTSIZE;

cl->due_date_time = ldlong(&buf[offset]);
offset += LONGSIZE;

cl->dispd_date_time = ldlong(&buf[offset]);
offset += LONGSIZE;

ldchar(&buf[offset],sizeof(cl->due_time)-1,cl->due_time);
offset += sizeof(cl->due_time)-1; 

ldchar(&buf[offset],sizeof(cl->dispd_date)-1,cl->dispd_date);
offset += sizeof(cl->dispd_date)-1; 

ldchar(&buf[offset],sizeof(cl->dispd_time)-1,cl->dispd_time);
offset += sizeof(cl->dispd_time)-1; 

ldchar(&buf[offset],sizeof(cl->pckup_date)-1,cl->pckup_date);
offset += sizeof(cl->pckup_date)-1; 

ldchar(&buf[offset],sizeof(cl->pckup_time)-1,cl->pckup_time);
offset += sizeof(cl->pckup_time)-1; 

ldchar(&buf[offset],sizeof(cl->company_name)-1,cl->company_name);
offset += sizeof(cl->company_name)-1; 

cl->pckup_date_time = ldlong(&buf[offset]);
offset += LONGSIZE;

cl->close_date_time = ldlong(&buf[offset]);
offset += LONGSIZE;

ldchar(&buf[offset],sizeof(cl->close_date)-1,cl->close_date);
offset += sizeof(cl->close_date)-1; 

ldchar(&buf[offset],sizeof(cl->close_time)-1,cl->close_time);
offset += sizeof(cl->close_time)-1; 

ldchar(&buf[offset],sizeof(cl->approve_code)-1,cl->approve_code);
offset += sizeof(cl->approve_code)-1; 

cl->fare_amount = ldfltnull(&buf[offset], &code);
offset += FLOATSIZE;

cl->subsc_call_nbr = ldlong(&buf[offset]);
offset += LONGSIZE;

cl->veh_nbr = ldint(&buf[offset]);
offset += INTSIZE;

cl->drv_id = ldlong(&buf[offset]);
offset += LONGSIZE;

cl->dispatched_by = ldint(&buf[offset]);
offset += INTSIZE;

cl->expire_date = ldlong(&buf[offset]);
offset += LONGSIZE;

cl->approve_date = ldlong(&buf[offset]);
offset += LONGSIZE;

cl->approve_by = ldint(&buf[offset]);
offset += INTSIZE;

ldchar(&buf[offset],sizeof(cl->ac_addt1_info1)-1,cl->ac_addt1_info1);
offset += sizeof(cl->ac_addt1_info1)-1; 

ldchar(&buf[offset],sizeof(cl->ac_addt1_info2)-1,cl->ac_addt1_info2);
offset += sizeof(cl->ac_addt1_info2)-1; 

ldchar(&buf[offset],sizeof(cl->driver_msg)-1,cl->driver_msg);
offset += sizeof(cl->driver_msg)-1; 

ldchar(&buf[offset],sizeof(cl->pckup_pmap)-1,cl->pckup_pmap);
offset += sizeof(cl->pckup_pmap)-1; 

ldchar(&buf[offset],sizeof(cl->dest_dmap)-1,cl->dest_dmap);
offset += sizeof(cl->dest_dmap)-1; 

ldchar(&buf[offset],sizeof(cl->status_bits)-1,cl->status_bits);
offset += sizeof(cl->status_bits)-1; 

ldchar(&buf[offset],sizeof(cl->zone_desc)-1,cl->zone_desc);
offset += sizeof(cl->zone_desc)-1; 

cl->screen_type = buf[offset];
offset += sizeof(cl->screen_type); 

ldchar(&buf[offset],sizeof(cl->status)-1,cl->status);
offset += sizeof(cl->status)-1; 

ldchar(&buf[offset],sizeof(cl->pass_name_key)-1,cl->pass_name_key);
offset += sizeof(cl->pass_name_key)-1; 

cl->dispatch_entry_tm = ldlong(&buf[offset]);
offset += LONGSIZE;

ldchar(&buf[offset],sizeof(cl->phone_number)-1,cl->phone_number);
offset += sizeof(cl->phone_number)-1; 

  cl->advise_arrival = buf[offset];
  offset += sizeof(cl->advise_arrival); 

#ifndef NAMERICA
  cl->ext_nbr = ldlong(&buf[offset]);
  offset += LONGSIZE;

  cl->offer_type = buf[offset];
  offset += sizeof(cl->offer_type);

  cl->detail_type = buf[offset];
  offset += sizeof(cl->detail_type);

  cl->call_msg_nbr = buf[offset];
  offset += sizeof(cl->call_msg_nbr);
#else
  cl->send_destination = buf[offset];
  offset += sizeof( cl->send_destination );

  if ( ( cl->send_destination != 'Y' ) && ( cl->send_destination != 'N' ) )
    cl->send_destination = 'N';
  
  cl->send_rate = buf[offset];
  offset += sizeof( cl->send_rate );

#endif
  
  ldchar(&buf[offset], sizeof(cl->invoice_nbr1)-1, cl->invoice_nbr1);
  offset += sizeof(cl->invoice_nbr1)-1;

  ldchar(&buf[offset], sizeof(cl->invoice_nbr2)-1, cl->invoice_nbr2);
  offset += sizeof(cl->invoice_nbr2)-1;

  ldchar(&buf[offset], sizeof(cl->invoice_nbr3)-1, cl->invoice_nbr3);
  offset += sizeof(cl->invoice_nbr3)-1;

  ldchar(&buf[offset], sizeof(cl->invoice_nbr4)-1, cl->invoice_nbr4);
  offset += sizeof(cl->invoice_nbr4)-1;
  
  ldchar(&buf[offset], sizeof(cl->ct_adr_cmnt)-1, cl->ct_adr_cmnt);
  offset += sizeof(cl->ct_adr_cmnt)-1;

  cl->via_addresses = ldint(&buf[offset]);
  offset += INTSIZE;

#ifdef NAMERICA
  cl->call_msg_nbr = cl->via_addresses + 0x30;
  if ( (cl->call_msg_nbr > '9') || (cl->call_msg_nbr < '0') )
    cl->call_msg_nbr = '0';
  cl->via_addresses = 0;
#endif

  cl->vat = ldfltnull(&buf[offset], &code);
  offset += FLOATSIZE;

  cl->gps_lat = lddblnull(&buf[offset], &code);
  offset += DOUBLESIZE;

  cl->gps_long = lddblnull(&buf[offset], &code);
  offset += DOUBLESIZE;

  cl->cash_account = buf[offset];
  offset += sizeof(cl->cash_account);

  cl->dest_gpsx = lddblnull(&buf[offset], &code);
  offset += DOUBLESIZE;

  cl->dest_gpsy = lddblnull(&buf[offset], &code);
  offset += DOUBLESIZE;

}

