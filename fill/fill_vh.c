static char sz__FILE__[]="@(#)fill_vh.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_vh.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:38:37
 * @(#)  Last delta: 12/2/94 at 11:35:14
 * @(#)  SCCS File: /taxi/sccs/s.fill_vh.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC
#include "taxipak.h"
#include "Object.h"
#include "mads_types.h"
#include "Vehicle_db.h"

void
fill_vh_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_vh  *veh;

	veh = (struct cisam_vh*)req_buf;

	stchar(&veh->fleet,&buf[VH_FLEET],sizeof(veh->fleet)); /* fleet indicator */

	stint(veh->nbr, &buf[VH_NBR]);  /* vehicle number */

	stchar(veh->mobile_rf_id, &buf[VH_MID], sizeof(veh->mobile_rf_id)-1);

	stlong(veh->last_driver, &buf[VH_LAST_DRIVER]); /* last driver */

	stchar(&veh->susp_pass_call,&buf[VH_SUSP_PASS_CALL],sizeof(veh->susp_pass_call)); /* Suspension key - passenger */
	stlong(veh->susp_from_dt_tm,&buf[VH_SUSP_PASS_START]);
	stlong(veh->susp_to_dt_tm,&buf[VH_SUSP_PASS_END]);

	stchar(&veh->susp_del_call,&buf[VH_SUSP_DEL_CALL],sizeof(veh->susp_del_call)); /* Suspension key - delivery */
	stlong(veh->susp_d_from_dt_tm,&buf[VH_SUSP_DEL_START]);
	stlong(veh->susp_d_to_dt_tm,&buf[VH_SUSP_DEL_END]);

	stchar(&veh->susp_typex_call,&buf[VH_SUSP_TYPEX_CALL],sizeof(veh->susp_typex_call)); /* Suspension key - typex */
	stlong(veh->susp_x_from_dt_tm,&buf[VH_SUSP_TYPEX_START]);
	stlong(veh->susp_x_to_dt_tm,&buf[VH_SUSP_TYPEX_END]);
}

void
fill_vh(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int  action;
{
	struct  cisam_vh  *veh;
	register int  offset = 0;

	veh = (struct cisam_vh*)req_buf;

	stchar(&veh->fleet,&buf[offset],sizeof(veh->fleet));
	offset += sizeof(veh->fleet);
	stchar(&veh->kdt_working,&buf[offset],sizeof(veh->kdt_working));
	offset += sizeof(veh->kdt_working);
	stint(veh->nbr,&buf[offset]);
	offset += INTSIZE;
	stchar(veh->mobile_rf_id,&buf[offset],sizeof(veh->mobile_rf_id)-1);
	offset += sizeof(veh->mobile_rf_id)-1;
	stchar(veh->license_nbr,&buf[offset],sizeof(veh->license_nbr)-1);
	offset += sizeof(veh->license_nbr)-1;
	stchar(veh->make,&buf[offset],sizeof(veh->make)-1);
	offset += sizeof(veh->make)-1;
	stchar(veh->model,&buf[offset],sizeof(veh->model)-1);
	offset += sizeof(veh->model)-1;
	stchar(veh->seals,&buf[offset],sizeof(veh->seals)-1);
	offset += sizeof(veh->seals)-1;
	stchar(&veh->susp_pass_call,&buf[offset],sizeof(veh->susp_pass_call));
	offset += sizeof(veh->susp_pass_call);
	stlong(veh->susp_from_dt_tm,&buf[offset]);
	offset += LONGSIZE;
	stlong(veh->susp_to_dt_tm,&buf[offset]);
	offset += LONGSIZE;
	stchar(&veh->susp_del_call,&buf[offset],sizeof(veh->susp_del_call));
	offset += sizeof(veh->susp_del_call);
	stchar(&veh->susp_typex_call,&buf[offset],sizeof(&veh->susp_typex_call));
	offset += sizeof(veh->susp_typex_call);
	stchar(veh->susp_reason,&buf[offset],sizeof(veh->susp_reason)-1);
	offset += sizeof(veh->susp_reason)-1;
	stchar(veh->susp_message,&buf[offset],sizeof(veh->susp_message)-1);
	offset += sizeof(veh->susp_message)-1;
	stchar(veh->attr,&buf[offset],sizeof(veh->attr)-1);
	offset += sizeof(veh->attr)-1;
	stchar(&veh->meter_working,&buf[offset],sizeof(&veh->meter_working));
	offset += sizeof(veh->meter_working);
	stlong(veh->last_emerg_on,&buf[offset]);
	offset += LONGSIZE;
	stlong(veh->last_emerg_off,&buf[offset]);
	offset += LONGSIZE;
	stlong(veh->last_signed_on,&buf[offset]);
	offset += LONGSIZE;
	stlong(veh->last_signed_off,&buf[offset]);
	offset += LONGSIZE;
	stlong(veh->last_driver,&buf[offset]);
	offset += LONGSIZE;
	stlong(veh->last_call,&buf[offset]);
	offset += LONGSIZE;
	stint(veh->assigns,&buf[offset]);
	offset += INTSIZE;
	stint(veh->calls,&buf[offset]);
	offset += INTSIZE;
	stint(veh->flags,&buf[offset]);
	offset += INTSIZE;
	stint(veh->bids,&buf[offset]);
	offset += INTSIZE;
	stint(veh->posts,&buf[offset]);
	offset += INTSIZE;
	stint(veh->cond_posts,&buf[offset]);
	offset += INTSIZE;
	stint(veh->enroute_posts,&buf[offset]);
	offset += INTSIZE;
	stint(veh->short_meters,&buf[offset]);
	offset += INTSIZE;
	stint(veh->late_meters,&buf[offset]);
	offset += INTSIZE;
	stint(veh->no_accepts,&buf[offset]);
	offset += INTSIZE;
	stint(veh->reject,&buf[offset]);
	offset += INTSIZE;

	stint(veh->breaks,&buf[offset]);
	offset += INTSIZE;
	stint(veh->req_to_talk,&buf[offset]);
	offset += INTSIZE;
	stint(veh->messages,&buf[offset]);
	offset += INTSIZE;
	stint(veh->callbacks,&buf[offset]);
	offset += INTSIZE;
	stint(veh->callouts,&buf[offset]);
	offset += INTSIZE;
	stint(veh->no_shows,&buf[offset]);
	offset += INTSIZE;

	stint(veh->fleet_nbr,&buf[offset]);
	offset += INTSIZE;

	stchar(veh->susp_from_date,&buf[offset],sizeof(veh->susp_from_date)-1);
	offset += sizeof(veh->susp_from_date)-1;

	stchar(veh->susp_from_time,&buf[offset],sizeof(veh->susp_from_time)-1);
	offset += sizeof(veh->susp_from_time)-1;

	stchar(veh->susp_to_date,&buf[offset],sizeof(veh->susp_to_date)-1);
	offset += sizeof(veh->susp_to_date)-1;

	stchar(veh->susp_to_time,&buf[offset],sizeof(veh->susp_to_time)-1);
	offset += sizeof(veh->susp_to_time)-1;

	stchar(veh->comments,&buf[offset],sizeof(veh->comments)-1);
	offset += sizeof(veh->comments)-1;


	stint(veh->status,&buf[offset]);
	offset += INTSIZE;

	stint(veh->last_zone_posted,&buf[offset]);
	offset += INTSIZE;


	stlong(veh->last_time_posted,&buf[offset]); 
		offset += LONGSIZE;

	stint(veh->schedule_num,&buf[offset]);
	offset += INTSIZE;

	stint(veh->term_type,&buf[offset]);
	offset += INTSIZE;

		stlong(veh->itm_last_receipt_nbr,&buf[offset]);
		offset += LONGSIZE;

		stlong(veh->itm_nbr_missing_receipts,&buf[offset]);
		offset += LONGSIZE;

		stlong(veh->itm_request_status,&buf[offset]);
		offset += LONGSIZE;

		stlong(veh->itm_receipt_nbr_first,&buf[offset]);
		offset += LONGSIZE;

		stlong(veh->itm_receipt_nbr_last,&buf[offset]);
		offset += LONGSIZE;

		stlong(veh->itm_receipt_nbr_current,&buf[offset]);
		offset += LONGSIZE;

		stlong(veh->itm_receipt_nbr_cnt,&buf[offset]);
		offset += LONGSIZE;

		stlong(veh->itm_retry_cnt,&buf[offset]);
		offset += LONGSIZE;

	stlong(veh->susp_d_from_dt_tm,&buf[offset]);
	offset += LONGSIZE;

	stlong(veh->susp_d_to_dt_tm,&buf[offset]);
	offset += LONGSIZE;

	stchar(veh->susp_d_from_date,&buf[offset],sizeof(veh->susp_d_from_date)-1);
	offset += sizeof(veh->susp_d_from_date)-1;

	stchar(veh->susp_d_from_time,&buf[offset],sizeof(veh->susp_d_from_time)-1);
	offset += sizeof(veh->susp_d_from_time)-1;

	stchar(veh->susp_d_to_date,&buf[offset],sizeof(veh->susp_d_to_date)-1);
	offset += sizeof(veh->susp_d_to_date)-1;

	stchar(veh->susp_d_to_time,&buf[offset],sizeof(veh->susp_d_to_time)-1);
	offset += sizeof(veh->susp_d_to_time)-1;
 
	stlong(veh->susp_x_from_dt_tm,&buf[offset]);
	offset += LONGSIZE;
 
	stlong(veh->susp_x_to_dt_tm,&buf[offset]);
	offset += LONGSIZE;
 
	stchar(veh->susp_x_from_date,&buf[offset],sizeof(veh->susp_x_from_date)-1);
	offset += sizeof(veh->susp_x_from_date)-1;
 
	stchar(veh->susp_x_from_time,&buf[offset],sizeof(veh->susp_x_from_time)-1);
	offset += sizeof(veh->susp_x_from_time)-1;
 
	stchar(veh->susp_x_to_date,&buf[offset],sizeof(veh->susp_x_to_date)-1);
	offset += sizeof(veh->susp_x_to_date)-1;
 
	stchar(veh->susp_x_to_time,&buf[offset],sizeof(veh->susp_x_to_time)-1);
	offset += sizeof(veh->susp_x_to_time)-1;

	stchar(veh->color, &buf[offset], sizeof(veh->color) -1);
	offset += sizeof(veh->color) -1;

	stchar(veh->telephone, &buf[offset], sizeof(veh->telephone) -1);
	offset += sizeof(veh->telephone) -1;

	stchar(&veh->host_link, &buf[offset], sizeof(veh->host_link));
	offset += sizeof(veh->host_link);
	
	stdbl(veh->gps_lat, &buf[offset]);
	offset += DOUBLESIZE;

	stdbl(veh->gps_long, &buf[offset]);
	offset += DOUBLESIZE;

	stint(veh->gps_state, &buf[offset]);
	offset += INTSIZE;

	stchar(veh->mdt_tele, &buf[offset], sizeof(veh->mdt_tele) -1);
	offset += sizeof(veh->mdt_tele) -1;	
}

/* Load program variables from C-ISAM data record */
void
unfill_vh(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_vh  *veh;
	register int  offset = 0;
	char temp_attr[65];
	short code = 0;

	veh = (struct cisam_vh*)req_buf;

	veh->fleet = buf[offset];
	offset += sizeof(veh->fleet);

	veh->kdt_working = buf[offset];
	offset += sizeof(veh->kdt_working);

	veh->nbr = ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(veh->mobile_rf_id)-1,veh->mobile_rf_id);
	offset += sizeof(veh->mobile_rf_id)-1;

	ldchar(&buf[offset],sizeof(veh->license_nbr)-1,veh->license_nbr);
	offset += sizeof(veh->license_nbr)-1;
	ldchar(&buf[offset],sizeof(veh->make)-1,veh->make);
	offset += sizeof(veh->make) -1;
	ldchar(&buf[offset],sizeof(veh->model)-1,veh->model);
	offset += sizeof(veh->model)-1;
	ldchar(&buf[offset],sizeof(veh->seals)-1,veh->seals);
	offset += sizeof(veh->seals)-1;
	ldchar(&buf[offset],sizeof(veh->susp_pass_call),&veh->susp_pass_call);
	offset += sizeof(veh->susp_pass_call);
	veh->susp_from_dt_tm = ldlong(&buf[offset]);
	offset += LONGSIZE;
	veh->susp_to_dt_tm = ldlong(&buf[offset]);
	offset += LONGSIZE;
	ldchar(&buf[offset],sizeof(veh->susp_del_call),&veh->susp_del_call);
	offset += sizeof(veh->susp_del_call);
	ldchar(&buf[offset],sizeof(veh->susp_typex_call),&veh->susp_typex_call);
	offset += sizeof(veh->susp_typex_call);
	ldchar(&buf[offset],sizeof(veh->susp_reason)-1,veh->susp_reason);
	offset += sizeof(veh->susp_reason)-1;
	ldchar(&buf[offset],sizeof(veh->susp_message)-1,veh->susp_message);
	offset += sizeof(veh->susp_message)-1;
	ldchar(&buf[offset],sizeof(veh->attr)-1,temp_attr);
	sprintf(veh->attr,"%-64.64s",temp_attr);             /* as all 64 characters are needed in all TaxiPak routines */
	offset += sizeof(veh->attr)-1;
	ldchar(&buf[offset],sizeof(veh->meter_working),&veh->meter_working);
	offset += sizeof(veh->meter_working);
	veh->last_emerg_on = ldlong(&buf[offset]);
	offset += LONGSIZE;
	veh->last_emerg_off = ldlong(&buf[offset]);
	offset += LONGSIZE;
	veh->last_signed_on = ldlong(&buf[offset]);
	offset += LONGSIZE;
	veh->last_signed_off = ldlong(&buf[offset]);
	offset += LONGSIZE;
	veh->last_driver = ldlong(&buf[offset]);
	offset += LONGSIZE;
	veh->last_call = ldlong(&buf[offset]);
	offset += LONGSIZE;
	veh->assigns = ldint(&buf[offset]);
	offset += INTSIZE;
	veh->calls = ldint(&buf[offset]);
	offset += INTSIZE;
	veh->flags = ldint(&buf[offset]);
	offset += INTSIZE;
	veh->bids = ldint(&buf[offset]);
	offset += INTSIZE;
	veh->posts = ldint(&buf[offset]);
	offset += INTSIZE;
	veh->cond_posts = ldint(&buf[offset]);
	offset += INTSIZE;
	veh->enroute_posts = ldint(&buf[offset]);
	offset += INTSIZE;
	veh->short_meters = ldint(&buf[offset]);
	offset += INTSIZE;
	veh->late_meters = ldint(&buf[offset]);
	offset += INTSIZE;
	veh->no_accepts = ldint(&buf[offset]);
	offset += INTSIZE;

	veh->reject = ldint(&buf[offset]);
	offset += INTSIZE;

	veh->breaks = ldint(&buf[offset]);
	offset += INTSIZE;

	veh->req_to_talk = ldint(&buf[offset]);
	offset += INTSIZE;
	veh->messages = ldint(&buf[offset]);
	offset += INTSIZE;
	veh->callbacks = ldint(&buf[offset]);
	offset += INTSIZE;
	veh->callouts = ldint(&buf[offset]);
	offset += INTSIZE;
	veh->no_shows = ldint(&buf[offset]);
	offset += INTSIZE;

	veh->fleet_nbr = ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(veh->susp_from_date)-1,veh->susp_from_date);
	offset += sizeof(veh->susp_from_date)-1;

	ldchar(&buf[offset],sizeof(veh->susp_from_time)-1,veh->susp_from_time);
	offset += sizeof(veh->susp_from_time)-1;

	ldchar(&buf[offset],sizeof(veh->susp_to_date)-1,veh->susp_to_date);
	offset += sizeof(veh->susp_to_date)-1;

	ldchar(&buf[offset],sizeof(veh->susp_to_time)-1,veh->susp_to_time);
	offset += sizeof(veh->susp_to_time)-1;

	ldchar(&buf[offset],sizeof(veh->comments)-1,veh->comments);
	offset += sizeof(veh->comments)-1;
	

	veh->status = ldint(&buf[offset]);
	offset += INTSIZE;

	veh->last_zone_posted = ldint(&buf[offset]);
	offset += INTSIZE;

	veh->last_time_posted = ldlong(&buf[offset]);
		offset += LONGSIZE;

	veh->schedule_num = ldint(&buf[offset]);
	offset += INTSIZE;

	veh->term_type = ldint(&buf[offset]);
	offset += INTSIZE;

		veh->itm_last_receipt_nbr = ldlong(&buf[offset]);
		offset += LONGSIZE;

		veh->itm_nbr_missing_receipts = ldlong(&buf[offset]);
		offset += LONGSIZE;

		veh->itm_request_status = ldlong(&buf[offset]);
		offset += LONGSIZE;

		veh->itm_receipt_nbr_first = ldlong(&buf[offset]);
		offset += LONGSIZE;

		veh->itm_receipt_nbr_last = ldlong(&buf[offset]);
		offset += LONGSIZE;

		veh->itm_receipt_nbr_current = ldlong(&buf[offset]);
		offset += LONGSIZE;

		veh->itm_receipt_nbr_cnt = ldlong(&buf[offset]);
		offset += LONGSIZE;

		veh->itm_retry_cnt = ldlong(&buf[offset]);
		offset += LONGSIZE;

	veh->susp_d_from_dt_tm = ldlong(&buf[offset]);
	offset += LONGSIZE;
 
	veh->susp_d_to_dt_tm = ldlong(&buf[offset]);
	offset += LONGSIZE;
 
	ldchar(&buf[offset],sizeof(veh->susp_d_from_date)-1,veh->susp_d_from_date);
	offset += sizeof(veh->susp_d_from_date)-1;
 
	ldchar(&buf[offset],sizeof(veh->susp_d_from_time)-1,veh->susp_d_from_time);
	offset += sizeof(veh->susp_d_from_time)-1;
 
	ldchar(&buf[offset],sizeof(veh->susp_d_to_date)-1,veh->susp_d_to_date);
	offset += sizeof(veh->susp_d_to_date)-1;
 
	ldchar(&buf[offset],sizeof(veh->susp_d_to_time)-1,veh->susp_d_to_time);
	offset += sizeof(veh->susp_d_to_time)-1;
 
	veh->susp_x_from_dt_tm = ldlong(&buf[offset]);
	offset += LONGSIZE;
 
	veh->susp_x_to_dt_tm = ldlong(&buf[offset]);
	offset += sizeof(veh->susp_x_to_dt_tm);
 
	ldchar(&buf[offset],sizeof(veh->susp_x_from_date)-1,veh->susp_x_from_date);
	offset += sizeof(veh->susp_x_from_date)-1;

	ldchar(&buf[offset],sizeof(veh->susp_x_from_time)-1,veh->susp_x_from_time);
	offset += sizeof(veh->susp_x_from_time)-1;

	ldchar(&buf[offset],sizeof(veh->susp_x_to_date)-1,veh->susp_x_to_date);
	offset += sizeof(veh->susp_x_to_date)-1;

	ldchar(&buf[offset],sizeof(veh->susp_x_to_time)-1,veh->susp_x_to_time);
	offset += sizeof(veh->susp_x_to_time)-1;

	ldchar(&buf[offset], sizeof(veh->color) -1, veh->color);
	offset += sizeof(veh->color) -1;

	ldchar(&buf[offset], sizeof(veh->telephone) -1, veh->telephone);
	offset += sizeof(veh->telephone) -1;

	veh->host_link = buf[offset];
	offset += sizeof(veh->host_link);
	
	veh->gps_lat = lddblnull(&buf[offset], &code);
	offset += DOUBLESIZE;

	veh->gps_long = lddblnull(&buf[offset], &code);
	offset += DOUBLESIZE;

	veh->gps_state = ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset], sizeof(veh->mdt_tele) -1, veh->mdt_tele);
	offset += sizeof(veh->mdt_tele) -1;	
}

