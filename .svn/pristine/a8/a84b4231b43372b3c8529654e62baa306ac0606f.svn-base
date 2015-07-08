static char sz__FILE__[]="@(#)fill_dr.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_dr.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:37:32
 * @(#)  Last delta: 12/2/94 at 11:31:41
 * @(#)  SCCS File: /taxi/sccs/s.fill_dr.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC

#include "Driver_db.h"

void
fill_dr_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_dr  *drv;

	drv = (struct cisam_dr*)req_buf;

	stchar(&drv->fleet,&buf[DR_FLEET],sizeof(drv->fleet));      /* fleet indicator */
	stlong(drv->id,&buf[DR_ID]);      /* driver id */
	stchar(drv->name,&buf[DR_NAME],sizeof(drv->name)-1);
	stint(drv->last_veh_nbr,&buf[DR_LAST_VEH_NBR]); /* last vehicle number */
	stchar(&drv->susp_pass_call,&buf[DR_SUSP_PASS_CALL],sizeof(drv->susp_pass_call));
	stlong(drv->susp_from_dt_tm,&buf[DR_SUSP_PASS_START]);
	stlong(drv->susp_to_dt_tm,&buf[DR_SUSP_PASS_END]);
	stchar(&drv->susp_del_call,&buf[DR_SUSP_DEL_CALL],sizeof(drv->susp_del_call));
	stlong(drv->susp_d_from_dt_tm,&buf[DR_SUSP_DEL_START]);
	stlong(drv->susp_d_to_dt_tm,&buf[DR_SUSP_DEL_END]);
	stchar(&drv->susp_typex_call,&buf[DR_SUSP_TYPEX_CALL],sizeof(drv->susp_typex_call));
	stlong(drv->susp_x_from_dt_tm,&buf[DR_SUSP_TYPEX_START]);
	stlong(drv->susp_x_to_dt_tm,&buf[DR_SUSP_TYPEX_END]);
}

void
fill_dr(req_buf, buf,action)
     char *req_buf;
     char *buf;
     int    action;
{
	struct  cisam_dr  *drv;
	register int  offset = 0;

	drv = (struct cisam_dr*)req_buf;

	stchar(&drv->fleet,&buf[offset],sizeof(drv->fleet));
	offset += sizeof(drv->fleet);
	stchar(&drv->class,&buf[offset],sizeof(drv->class));
	offset += sizeof(drv->class);
	stlong(drv->id,&buf[offset]);
	offset += LONGSIZE;
	stchar(drv->name,&buf[offset],sizeof(drv->name)-1);
	offset += sizeof(drv->name) -1;
	stchar(drv->address,&buf[offset],sizeof(drv->address)-1);
	offset += sizeof(drv->address) -1;
	stchar(drv->city,&buf[offset],sizeof(drv->city)-1);
	offset += sizeof(drv->city) - 1;
	stchar(drv->zip,&buf[offset],sizeof(drv->zip)-1);
	offset += sizeof(drv->zip) -1;
	stchar(drv->zip_suf,&buf[offset],sizeof(drv->zip_suf)-1);
	offset += sizeof(drv->zip_suf) -1;
	stchar(drv->phone,&buf[offset],sizeof(drv->phone)-1);
	offset += sizeof(drv->phone) -1;
	stchar(drv->license_nbr,&buf[offset],sizeof(drv->license_nbr)-1);
	offset += sizeof(drv->license_nbr) -1;
	stchar(drv->taxi_license,&buf[offset],sizeof(drv->taxi_license)-1);
	offset += sizeof(drv->taxi_license) -1;
	stchar(drv->seals,&buf[offset],sizeof(drv->seals)-1);
	offset += sizeof(drv->seals) -1;
	stchar(drv->attributes,&buf[offset],sizeof(drv->attributes)-1);
	offset += sizeof(drv->attributes) -1;
	stlong(drv->susp_from_dt_tm,&buf[offset]);
	offset += LONGSIZE;
	stlong(drv->susp_to_dt_tm,&buf[offset]);
	offset += LONGSIZE;
	stchar(&drv->susp_pass_call,&buf[offset],sizeof(drv->susp_pass_call));
	offset += sizeof(drv->susp_pass_call);
	stchar(&drv->susp_del_call,&buf[offset],sizeof(drv->susp_del_call));
	offset += sizeof(drv->susp_del_call);
	stchar(&drv->susp_typex_call,&buf[offset],sizeof(drv->susp_typex_call));
	offset += sizeof(drv->susp_typex_call);
	stchar(drv->susp_reason,&buf[offset],sizeof(drv->susp_reason)-1);
	offset += sizeof(drv->susp_reason)-1;
	stchar(drv->susp_message,&buf[offset],sizeof(drv->susp_message)-1);
	offset += sizeof(drv->susp_message)-1;
	stchar(drv->comments,&buf[offset],sizeof(drv->comments)-1);
	offset += sizeof(drv->comments)-1;
	stchar(drv->lic_renew,&buf[offset],sizeof(drv->lic_renew)-1);
	offset += sizeof(drv->lic_renew)-1;

	stchar(drv->taxi_lic_renew,&buf[offset],sizeof(drv->taxi_lic_renew)-1);
	offset += sizeof(drv->taxi_lic_renew)-1;

	stlong(drv->emerg_on_dt_tm,&buf[offset]);
	offset += LONGSIZE;
	stlong(drv->emerg_of_dt_tm,&buf[offset]);
	offset += LONGSIZE;
	stlong(drv->signed_on_dt_tm,&buf[offset]);
	offset += LONGSIZE;
	stlong(drv->signed_of_dt_tm,&buf[offset]);
	offset += LONGSIZE;
	stint(drv->last_veh_nbr,&buf[offset]);
	offset += INTSIZE;
	stlong(drv->last_call,&buf[offset]);
	offset += LONGSIZE;
	stint(drv->assigns,&buf[offset]);
	offset += INTSIZE;
	stint(drv->calls,&buf[offset]);
	offset += INTSIZE;
	stint(drv->flags,&buf[offset]);
	offset += INTSIZE;
	stint(drv->bids,&buf[offset]);
	offset += INTSIZE;
	stint(drv->posts,&buf[offset]);
	offset += INTSIZE;
	stint(drv->cond_posts,&buf[offset]);
	offset += INTSIZE;
	stint(drv->enroute_posts,&buf[offset]);
	offset += INTSIZE;
	stint(drv->short_meters,&buf[offset]);
	offset += INTSIZE;
	stint(drv->late_meters,&buf[offset]);
	offset += INTSIZE;
	stint(drv->no_accepts,&buf[offset]);
	offset += INTSIZE;
	stint(drv->rejects,&buf[offset]);
	offset += INTSIZE;
	stint(drv->breaks,&buf[offset]);
	offset += INTSIZE;
	stint(drv->req_to_talk,&buf[offset]);
	offset += INTSIZE;
	stint(drv->messages,&buf[offset]);
	offset += INTSIZE;
	stint(drv->callbacks,&buf[offset]);
	offset += INTSIZE;
	stint(drv->callouts,&buf[offset]);
	offset += INTSIZE;

	stint(drv->no_shows,&buf[offset]);
	offset += INTSIZE;

	stchar(drv->susp_from_date,&buf[offset],sizeof(drv->susp_from_date)-1);
	offset += sizeof(drv->susp_from_date)-1;

	stchar(drv->susp_from_time,&buf[offset],sizeof(drv->susp_from_time)-1);
	offset += sizeof(drv->susp_from_time)-1;

	stchar(drv->susp_to_date,&buf[offset],sizeof(drv->susp_to_date)-1);
	offset += sizeof(drv->susp_to_date)-1;

	stchar(drv->susp_to_time,&buf[offset],sizeof(drv->susp_to_time)-1);
	offset += sizeof(drv->susp_to_time)-1;

	stchar(drv->phone_number,&buf[offset],sizeof(drv->phone_number)-1);
	offset += sizeof(drv->phone_number)-1;

	stlong(drv->susp_d_from_dt_tm,&buf[offset]);
	offset += LONGSIZE;

	stlong(drv->susp_d_to_dt_tm,&buf[offset]);
	offset += LONGSIZE;

	stchar(drv->susp_d_from_date,&buf[offset],sizeof(drv->susp_d_from_date)-1);
	offset += sizeof(drv->susp_d_from_date)-1;

	stchar(drv->susp_d_from_time,&buf[offset],sizeof(drv->susp_d_from_time)-1);
	offset += sizeof(drv->susp_d_from_time)-1;

	stchar(drv->susp_d_to_date,&buf[offset],sizeof(drv->susp_d_to_date)-1);
	offset += sizeof(drv->susp_d_to_date)-1;

	stchar(drv->susp_d_to_time,&buf[offset],sizeof(drv->susp_d_to_time)-1);
	offset += sizeof(drv->susp_d_to_time)-1;

	stlong(drv->susp_x_from_dt_tm,&buf[offset]);
	offset += LONGSIZE;

	stlong(drv->susp_x_to_dt_tm,&buf[offset]);
	offset += LONGSIZE;

	stchar(drv->susp_x_from_date,&buf[offset],sizeof(drv->susp_x_from_date)-1);
	offset += sizeof(drv->susp_x_from_date)-1;

	stchar(drv->susp_x_from_time,&buf[offset],sizeof(drv->susp_x_from_time)-1);
	offset += sizeof(drv->susp_x_from_time)-1;

	stchar(drv->susp_x_to_date,&buf[offset],sizeof(drv->susp_x_to_date)-1);
	offset += sizeof(drv->susp_x_to_date)-1;

	stchar(drv->susp_x_to_time,&buf[offset],sizeof(drv->susp_x_to_time)-1);
	offset += sizeof( drv->susp_x_to_time ) -1;

	stchar(drv->telephone, &buf[offset], sizeof(drv->telephone) -1);
	offset += sizeof(drv->telephone) -1;

	stchar(&drv->serial_nbr,&buf[offset],sizeof(drv->serial_nbr));
	offset += sizeof(drv->serial_nbr);
}

/* Load program variables from C-ISAM data record */
void
unfill_dr( req_buf, buf )
     char *req_buf;
     char *buf;
{
	struct  cisam_dr  *drv;
	register int  offset = 0;
	char temp_attr[65];
	
	drv = (struct cisam_dr*)req_buf;

	ldchar(&buf[offset],sizeof(drv->fleet),&drv->fleet);
	offset += sizeof(drv->fleet);
	ldchar(&buf[offset],sizeof(drv->class),&drv->class);
	offset += sizeof(drv->class);
	drv->id = ldlong(&buf[offset]);
	offset += LONGSIZE;
	ldchar(&buf[offset],sizeof(drv->name)-1,drv->name);
	offset += sizeof(drv->name)-1;
	ldchar(&buf[offset],sizeof(drv->address)-1,drv->address);
	offset += sizeof(drv->address)-1;
	ldchar(&buf[offset],sizeof(drv->city)-1,drv->city);
	offset += sizeof(drv->city)-1;
	ldchar(&buf[offset],sizeof(drv->zip)-1,drv->zip);
	offset += sizeof(drv->zip)-1;
	ldchar(&buf[offset],sizeof(drv->zip_suf)-1,drv->zip_suf);
	offset += sizeof(drv->zip_suf)-1;
	ldchar(&buf[offset],sizeof(drv->phone)-1,drv->phone);
	offset += sizeof(drv->phone)-1;
	ldchar(&buf[offset],sizeof(drv->license_nbr)-1,drv->license_nbr);
	offset += sizeof(drv->license_nbr)-1;
	ldchar(&buf[offset],sizeof(drv->taxi_license)-1,drv->taxi_license);
	offset += sizeof(drv->taxi_license)-1;
	ldchar(&buf[offset],sizeof(drv->seals)-1,drv->seals);
	offset += sizeof(drv->seals)-1;
	ldchar(&buf[offset],sizeof(drv->attributes)-1,temp_attr);
	sprintf(drv->attributes,"%-64.64s",temp_attr);             /* as all 64 characters are needed in all TaxiPak routines */
	offset += sizeof(drv->attributes)-1;
	drv->susp_from_dt_tm = ldlong(&buf[offset]);
	offset += LONGSIZE;
	drv->susp_to_dt_tm = ldlong(&buf[offset]);
	offset += LONGSIZE;
	ldchar(&buf[offset],sizeof(drv->susp_pass_call),&drv->susp_pass_call);
	offset += sizeof(drv->susp_pass_call);
	ldchar(&buf[offset],sizeof(drv->susp_del_call),&drv->susp_del_call);
	offset += sizeof(drv->susp_del_call);
	ldchar(&buf[offset],sizeof(drv->susp_typex_call),&drv->susp_typex_call);
	offset += sizeof(drv->susp_typex_call);
	ldchar(&buf[offset],sizeof(drv->susp_reason)-1,drv->susp_reason);
	offset += sizeof(drv->susp_reason)-1;
	ldchar(&buf[offset],sizeof(drv->susp_message)-1,drv->susp_message);
	offset += sizeof(drv->susp_message)-1;
	ldchar(&buf[offset],sizeof(drv->comments)-1,drv->comments);
	offset += sizeof(drv->comments)-1;

	ldchar(&buf[offset],sizeof(drv->lic_renew)-1,drv->lic_renew);
	offset += sizeof(drv->lic_renew)-1;

	ldchar(&buf[offset],sizeof(drv->taxi_lic_renew) -1,drv->taxi_lic_renew);
	offset += sizeof(drv->taxi_lic_renew)-1;

	drv->emerg_on_dt_tm = ldlong(&buf[offset]);
	offset += LONGSIZE;
	drv->emerg_of_dt_tm = ldlong(&buf[offset]);
	offset += LONGSIZE;
	drv->signed_on_dt_tm = ldlong(&buf[offset]);
	offset += LONGSIZE;
	drv->signed_of_dt_tm = ldlong(&buf[offset]);
	offset += LONGSIZE;
	drv->last_veh_nbr = ldint(&buf[offset]);
	offset += INTSIZE;
	drv->last_call = ldlong(&buf[offset]);
	offset += LONGSIZE;
	drv->assigns = ldint(&buf[offset]);
	offset += INTSIZE;
	drv->calls = ldint(&buf[offset]);
	offset += INTSIZE;
	drv->flags = ldint(&buf[offset]);
	offset += INTSIZE;
	drv->bids = ldint(&buf[offset]);
	offset += INTSIZE;
	drv->posts = ldint(&buf[offset]);
	offset += INTSIZE;
	drv->cond_posts = ldint(&buf[offset]);
	offset += INTSIZE;
	drv->enroute_posts = ldint(&buf[offset]);
	offset += INTSIZE;
	drv->short_meters = ldint(&buf[offset]);
	offset += INTSIZE;
	drv->late_meters = ldint(&buf[offset]);
	offset += INTSIZE;
	drv->no_accepts = ldint(&buf[offset]);
	offset += INTSIZE;
	drv->rejects = ldint(&buf[offset]);
	offset += INTSIZE;
	drv->breaks = ldint(&buf[offset]);
	offset += INTSIZE;
	drv->req_to_talk = ldint(&buf[offset]);
	offset += INTSIZE;
	drv->messages = ldint(&buf[offset]);
	offset += INTSIZE;
	drv->callbacks = ldint(&buf[offset]);
	offset += INTSIZE;
	drv->callouts = ldint(&buf[offset]);
	offset += INTSIZE;

	drv->no_shows = ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(drv->susp_from_date)-1,drv->susp_from_date);
	offset += sizeof(drv->susp_from_date)-1;

	ldchar(&buf[offset],sizeof(drv->susp_from_time)-1,drv->susp_from_time);
	offset += sizeof(drv->susp_from_time)-1;

	ldchar(&buf[offset],sizeof(drv->susp_to_date)-1,drv->susp_to_date);
	offset += sizeof(drv->susp_to_date)-1;

	ldchar(&buf[offset],sizeof(drv->susp_to_time)-1,drv->susp_to_time);
	offset += sizeof(drv->susp_to_time)-1;

	ldchar(&buf[offset],sizeof(drv->phone_number)-1,drv->phone_number);
	offset += sizeof(drv->phone_number)-1;

	drv->susp_d_from_dt_tm = ldlong(&buf[offset]);
	offset += LONGSIZE;

	drv->susp_d_to_dt_tm = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ldchar(&buf[offset],sizeof(drv->susp_d_from_date)-1,drv->susp_d_from_date);
	offset += sizeof(drv->susp_d_from_date)-1;

	ldchar(&buf[offset],sizeof(drv->susp_d_from_time)-1,drv->susp_d_from_time);
	offset += sizeof(drv->susp_d_from_time)-1;

	ldchar(&buf[offset],sizeof(drv->susp_d_to_date)-1,drv->susp_d_to_date);
	offset += sizeof(drv->susp_d_to_date)-1;

	ldchar(&buf[offset],sizeof(drv->susp_d_to_time)-1,drv->susp_d_to_time);
	offset += sizeof(drv->susp_d_to_time)-1;

	drv->susp_x_from_dt_tm = ldlong(&buf[offset]);
	offset += LONGSIZE;

	drv->susp_x_to_dt_tm = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ldchar(&buf[offset],sizeof(drv->susp_x_from_date)-1,drv->susp_x_from_date);
	offset += sizeof(drv->susp_x_from_date)-1;

	ldchar(&buf[offset],sizeof(drv->susp_x_from_time)-1,drv->susp_x_from_time);
	offset += sizeof(drv->susp_x_from_time)-1;

	ldchar(&buf[offset],sizeof(drv->susp_x_to_date)-1,drv->susp_x_to_date);
	offset += sizeof(drv->susp_x_to_date)-1;

	ldchar(&buf[offset],sizeof(drv->susp_x_to_time)-1,drv->susp_x_to_time);
	offset += sizeof(drv->susp_x_to_time)-1;

	ldchar(&buf[offset], sizeof(drv->telephone) -1, drv->telephone);
	offset += sizeof(drv->telephone) -1;

	ldchar(&buf[offset],sizeof(drv->serial_nbr),&drv->serial_nbr);
	offset += sizeof(drv->serial_nbr);
}
