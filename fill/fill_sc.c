static char sz__FILE__[]="@(#)fill_sc.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_sc.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:38:11
 * @(#)  Last delta: 12/2/94 at 11:28:05
 * @(#)  SCCS File: /taxi/sccs/s.fill_sc.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC
#include "Subcall_db.h"
#include "taxi_db.h"

void
fill_sc_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_sc  *sc;

	sc = (struct cisam_sc*)req_buf;

	stlong(sc->nbr,&buf[SC_NBR]);      

	stchar(&sc->fleet,&buf[SC_FLEET],sizeof(sc->fleet));  

  	stchar(sc->phone_nbr,&buf[SC_PHONE_NBR],sizeof(sc->phone_nbr)-1);

  	stchar(sc->charge_nbr,&buf[SC_CHARGE_NBR],sizeof(sc->charge_nbr)-1);

	stlong(sc->to_date,&buf[SC_TO_DATE]);

  	stchar(sc->pckup_str_key,&buf[SC_PCKUP_STR_KEY],sizeof(sc->pckup_str_key)-1);

  	stchar(sc->pass_name,&buf[SC_PASS_NAME],sizeof(sc->pass_name)-1);
}

void
fill_sc(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int    action;
{
	struct  cisam_sc  *sc;
	register int  offset = 0;
	int serial_nbr = 0;
	
	sc = (struct cisam_sc*)req_buf;

	if (action == ADD_REC)
	  {
	    isuniqueid(Db_files[SUBCALL_FILE_ID].fd,&serial_nbr);
	    sc->nbr = (int)serial_nbr;
	  }

	stlong(sc->nbr,&buf[offset]);
	offset += LONGSIZE;


	stchar(&sc->fleet,&buf[offset],sizeof(sc->fleet));
	offset += sizeof(sc->fleet);

	stchar(sc->extended_type,&buf[offset],sizeof(sc->extended_type)-1);
	offset += sizeof(sc->extended_type) -1;

	stchar(sc->pckup_adr_cmnt,&buf[offset],sizeof(sc->pckup_adr_cmnt)-1);
	offset += sizeof(sc->pckup_adr_cmnt) -1;

	stchar(&sc->local,&buf[offset],sizeof(sc->local));
	offset += sizeof(sc->local);

	stlong(sc->pckup_str_nbr,&buf[offset]);
	offset += LONGSIZE;

	stchar(&sc->pckup_str_suf,&buf[offset],sizeof(sc->pckup_str_suf));
	offset += sizeof(sc->pckup_str_suf);

	stchar(sc->pckup_pre_dir,&buf[offset],sizeof(sc->pckup_pre_dir)-1);
	offset += sizeof(sc->pckup_pre_dir) -1;

	stchar(sc->pckup_str_name,&buf[offset],sizeof(sc->pckup_str_name)-1);
	offset += sizeof(sc->pckup_str_name) -1;

	stchar(sc->pckup_str_type,&buf[offset],sizeof(sc->pckup_str_type)-1);
	offset += sizeof(sc->pckup_str_type) -1;

	stchar(sc->pckup_post_dir,&buf[offset],sizeof(sc->pckup_post_dir)-1);
	offset += sizeof(sc->pckup_post_dir) -1;

	stchar(sc->intr_pre_dir,&buf[offset],sizeof(sc->intr_pre_dir)-1);
	offset += sizeof(sc->intr_pre_dir) -1;

	stchar(sc->intr_str_name,&buf[offset],sizeof(sc->intr_str_name)-1);
	offset += sizeof(sc->intr_str_name) -1;

	stchar(sc->intr_str_type,&buf[offset],sizeof(sc->intr_str_type)-1);
	offset += sizeof(sc->intr_str_type) -1;

	stchar(sc->intr_post_dir,&buf[offset],sizeof(sc->intr_post_dir)-1);
	offset += sizeof(sc->intr_post_dir) -1;

	stchar(sc->pckup_apt_room,&buf[offset],sizeof(sc->pckup_apt_room)-1);
	offset += sizeof(sc->pckup_apt_room) -1;

	stchar(sc->pckup_city,&buf[offset],sizeof(sc->pckup_city)-1);
	offset += sizeof(sc->pckup_city) -1;

	stchar(sc->passenger_name,&buf[offset],sizeof(sc->passenger_name)-1);
	offset += sizeof(sc->passenger_name) -1;

	stint(sc->pckup_zone,&buf[offset]);
	offset += INTSIZE;

	stchar(sc->phone_area,&buf[offset],sizeof(sc->phone_area)-1);
	offset += sizeof(sc->phone_area) -1;

	stchar(sc->phone_nbr,&buf[offset],sizeof(sc->phone_nbr)-1);
	offset += sizeof(sc->phone_nbr) -1;

	stchar(sc->phone_ext,&buf[offset],sizeof(sc->phone_ext)-1);
	offset += sizeof(sc->phone_ext) -1;

	stchar(sc->general_cmnt,&buf[offset],sizeof(sc->general_cmnt)-1);
	offset += sizeof(sc->general_cmnt) -1;

	stchar(sc->dest_adr_cmnt,&buf[offset],sizeof(sc->dest_adr_cmnt)-1);
	offset += sizeof(sc->dest_adr_cmnt) -1;

	stlong(sc->dest_str_nbr,&buf[offset]);
	offset += LONGSIZE;

	stchar(&sc->dest_str_suf,&buf[offset],sizeof(sc->dest_str_suf));
	offset += sizeof(sc->dest_str_suf);

	stchar(sc->dest_pre_dir,&buf[offset],sizeof(sc->dest_pre_dir)-1);
	offset += sizeof(sc->dest_pre_dir)-1;

	stchar(sc->dest_str_name,&buf[offset],sizeof(sc->dest_str_name)-1);
	offset += sizeof(sc->dest_str_name) -1;

	stchar(sc->dest_str_type,&buf[offset],sizeof(sc->dest_str_type)-1);
	offset += sizeof(sc->dest_str_type) -1;

	stchar(sc->dest_post_dir,&buf[offset],sizeof(sc->dest_post_dir)-1);
	offset += sizeof(sc->dest_post_dir) -1;

	stchar(sc->dest_apt_room,&buf[offset],sizeof(sc->dest_apt_room)-1);
	offset += sizeof(sc->dest_apt_room) -1;

	stchar(sc->dest_city,&buf[offset],sizeof(sc->dest_city)-1);
	offset += sizeof(sc->dest_city) -1;

	stint(sc->dest_zone,&buf[offset]);
	offset += INTSIZE;

	stint(sc->personal_veh,&buf[offset]);
	offset += INTSIZE;

	stchar(&sc->personal_rqst,&buf[offset],sizeof(sc->personal_rqst));
	offset += sizeof(sc->personal_rqst);

	stchar(&sc->veh_attr_flag,&buf[offset],sizeof(sc->veh_attr_flag));
	offset += sizeof(sc->veh_attr_flag);

	stchar(sc->veh_attr,&buf[offset],sizeof(sc->veh_attr)-1);
	offset += sizeof(sc->veh_attr)-1;

	stchar(&sc->drv_attr_flag,&buf[offset],sizeof(sc->drv_attr_flag));
	offset += sizeof(sc->drv_attr_flag);

	stchar(sc->drv_attr,&buf[offset],sizeof(sc->drv_attr)-1);
	offset += sizeof(sc->drv_attr)-1;

	stchar(&sc->nbr_of_vehs,&buf[offset],sizeof(sc->nbr_of_vehs));
	offset += sizeof(sc->nbr_of_vehs);

	stint(sc->priority,&buf[offset]);
	offset += INTSIZE;

	stchar(sc->charge_nbr,&buf[offset],sizeof(sc->charge_nbr)-1);
	offset += sizeof(sc->charge_nbr)-1;

	stchar(sc->requested_by,&buf[offset],sizeof(sc->requested_by)-1);
	offset += sizeof(sc->requested_by)-1;

	stchar(sc->person_to_see,&buf[offset],sizeof(sc->person_to_see)-1);
	offset += sizeof(sc->person_to_see)-1;

	stchar(&sc->pod_required,&buf[offset],sizeof(sc->pod_required));
	offset += sizeof(sc->pod_required);

	stfloat(sc->call_rate,&buf[offset]);
	offset += FLOATSIZE;

	stlong(sc->from_date,&buf[offset]);
	offset += LONGSIZE;

	stlong(sc->to_date,&buf[offset]);
	offset += LONGSIZE;

        stint(sc->mon_time,&buf[offset]);
        offset += INTSIZE;

        stint(sc->mon_time_change,&buf[offset]);
        offset += INTSIZE;

        stint(sc->tue_time,&buf[offset]);
        offset += INTSIZE;

        stint(sc->tue_time_change,&buf[offset]);
        offset += INTSIZE;

        stint(sc->wed_time,&buf[offset]);
        offset += INTSIZE;

        stint(sc->wed_time_change,&buf[offset]);
        offset += INTSIZE;

        stint(sc->thu_time,&buf[offset]);
        offset += INTSIZE;

        stint(sc->thu_time_change,&buf[offset]);
        offset += INTSIZE;

        stint(sc->fri_time,&buf[offset]);
        offset += INTSIZE;

        stint(sc->fri_time_change,&buf[offset]);
        offset += INTSIZE;

        stint(sc->sat_time,&buf[offset]);
        offset += INTSIZE;

        stint(sc->sat_time_change,&buf[offset]);
        offset += INTSIZE;

        stint(sc->sun_time,&buf[offset]);
        offset += INTSIZE;

        stint(sc->sun_time_change,&buf[offset]);
        offset += INTSIZE;

        stlong(sc->date_time_in,&buf[offset]);
        offset += LONGSIZE;

        stint(sc->entered_by,&buf[offset]);
        offset += INTSIZE;

        stint(sc->last_changed_by,&buf[offset]);
        offset += INTSIZE;

        stlong(sc->call_nbr1,&buf[offset]);
        offset += LONGSIZE;

        stlong(sc->call_nbr2,&buf[offset]);
        offset += LONGSIZE;

	stchar(sc->date_in,&buf[offset],sizeof(sc->date_in)-1);
	offset += sizeof(sc->date_in)-1;

	stchar(sc->time_in,&buf[offset],sizeof(sc->time_in)-1);
	offset += sizeof(sc->time_in)-1;

	stchar(sc->ac_addt1_info1,&buf[offset],sizeof(sc->ac_addt1_info1)-1);
	offset += sizeof(sc->ac_addt1_info1)-1;

	stchar(sc->ac_addt1_info2,&buf[offset],sizeof(sc->ac_addt1_info2)-1);
	offset += sizeof(sc->ac_addt1_info2)-1;

	stchar(sc->status_bits,&buf[offset],sizeof(sc->status_bits)-1);
	offset += sizeof(sc->status_bits)-1;

	stchar(sc->company_name,&buf[offset],sizeof(sc->company_name)-1);
	offset += sizeof(sc->company_name)-1;

	stchar(sc->driver_msg,&buf[offset],sizeof(sc->driver_msg)-1);
	offset += sizeof(sc->driver_msg)-1;
	
	stchar(sc->pckup_pmap,&buf[offset],sizeof(sc->pckup_pmap)-1);
	offset += sizeof(sc->pckup_pmap)-1;

	stchar(sc->dest_dmap,&buf[offset],sizeof(sc->dest_dmap)-1);
	offset += sizeof(sc->dest_dmap)-1;

	stchar(sc->pckup_str_key,&buf[offset],sizeof(sc->pckup_str_key)-1);
	offset += sizeof(sc->pckup_str_key)-1;

	stchar(sc->pass_name,&buf[offset],sizeof(sc->pass_name)-1);
	offset += sizeof(sc->pass_name)-1;

	stchar(sc->phone_number,&buf[offset],sizeof(sc->phone_number)-1);
	offset += sizeof(sc->phone_number)-1;

	stchar(&sc->advise_arrival,&buf[offset],sizeof(sc->advise_arrival));
	offset += sizeof(sc->advise_arrival);

#ifdef NAMERICA
	stchar(&sc->send_dest, &buf[offset], sizeof(sc->send_dest));
	offset += sizeof(sc->send_dest);

	stchar(&sc->send_rate, &buf[offset], sizeof(sc->send_rate));
	offset += sizeof(sc->send_rate);
#endif
	
}
/* Load program variables from C-ISAM data record */
void
unfill_sc(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_sc  *sc;
	register int  offset = 0;

	sc = (struct cisam_sc*)req_buf;

	sc->nbr = ldlong(&buf[offset]);
	offset += LONGSIZE;

	sc->fleet = buf[offset];
	offset += sizeof(sc->fleet);

	ldchar(&buf[offset],sizeof(sc->extended_type)-1,sc->extended_type);
	offset += sizeof(sc->extended_type) -1;

	ldchar(&buf[offset],sizeof(sc->pckup_adr_cmnt)-1,sc->pckup_adr_cmnt);
	offset += sizeof(sc->pckup_adr_cmnt) -1;

	sc->local = buf[offset];
	offset += sizeof(sc->local);

	sc->pckup_str_nbr = ldlong(&buf[offset]);
	offset += LONGSIZE;

	sc->pckup_str_suf = buf[offset];
	offset += sizeof(sc->pckup_str_suf);

	ldchar(&buf[offset],sizeof(sc->pckup_pre_dir)-1,sc->pckup_pre_dir);
	offset += sizeof(sc->pckup_pre_dir) -1;

	ldchar(&buf[offset],sizeof(sc->pckup_str_name)-1,sc->pckup_str_name);
	offset += sizeof(sc->pckup_str_name) -1;

	ldchar(&buf[offset],sizeof(sc->pckup_str_type)-1,sc->pckup_str_type);
	offset += sizeof(sc->pckup_str_type) -1;

	ldchar(&buf[offset],sizeof(sc->pckup_post_dir)-1,sc->pckup_post_dir);
	offset += sizeof(sc->pckup_post_dir) -1;

	ldchar(&buf[offset],sizeof(sc->intr_pre_dir)-1,sc->intr_pre_dir);
	offset += sizeof(sc->intr_pre_dir) -1;

	ldchar(&buf[offset],sizeof(sc->intr_str_name)-1,sc->intr_str_name);
	offset += sizeof(sc->intr_str_name) -1;

	ldchar(&buf[offset],sizeof(sc->intr_str_type)-1,sc->intr_str_type);
	offset += sizeof(sc->intr_str_type) -1;

	ldchar(&buf[offset],sizeof(sc->intr_post_dir)-1,sc->intr_post_dir);
	offset += sizeof(sc->intr_post_dir) -1;

	ldchar(&buf[offset],sizeof(sc->pckup_apt_room)-1,sc->pckup_apt_room);
	offset += sizeof(sc->pckup_apt_room) -1;

	ldchar(&buf[offset],sizeof(sc->pckup_city)-1,sc->pckup_city);
	offset += sizeof(sc->pckup_city) -1;

	ldchar(&buf[offset],sizeof(sc->passenger_name)-1,sc->passenger_name);
	offset += sizeof(sc->passenger_name) -1;

	sc->pckup_zone = ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(sc->phone_area)-1,sc->phone_area);
	offset += sizeof(sc->phone_area) -1;

	ldchar(&buf[offset],sizeof(sc->phone_nbr)-1,sc->phone_nbr);
	offset += sizeof(sc->phone_nbr) -1;

	ldchar(&buf[offset],sizeof(sc->phone_ext)-1,sc->phone_ext);
	offset += sizeof(sc->phone_ext) -1;

	ldchar(&buf[offset],sizeof(sc->general_cmnt)-1,sc->general_cmnt);
	offset += sizeof(sc->general_cmnt) -1;

	ldchar(&buf[offset],sizeof(sc->dest_adr_cmnt)-1,sc->dest_adr_cmnt);
	offset += sizeof(sc->dest_adr_cmnt) -1;

	sc->dest_str_nbr= ldlong(&buf[offset]);
	offset += LONGSIZE;

	sc->dest_str_suf = buf[offset];
	offset += sizeof(sc->dest_str_suf);

	ldchar(&buf[offset],sizeof(sc->dest_pre_dir)-1,sc->dest_pre_dir);
	offset += sizeof(sc->dest_pre_dir)-1;

	ldchar(&buf[offset],sizeof(sc->dest_str_name)-1,sc->dest_str_name);
	offset += sizeof(sc->dest_str_name) -1;

	ldchar(&buf[offset],sizeof(sc->dest_str_type)-1,sc->dest_str_type);
	offset += sizeof(sc->dest_str_type) -1;

	ldchar(&buf[offset],sizeof(sc->dest_post_dir)-1,sc->dest_post_dir);
	offset += sizeof(sc->dest_post_dir) -1;

	ldchar(&buf[offset],sizeof(sc->dest_apt_room)-1,sc->dest_apt_room);
	offset += sizeof(sc->dest_apt_room) -1;

	ldchar(&buf[offset],sizeof(sc->dest_city)-1,sc->dest_city);
	offset += sizeof(sc->dest_city) -1;

	sc->dest_zone = ldint(&buf[offset]);
	offset += INTSIZE;

	sc->personal_veh = ldint(&buf[offset]);
	offset += INTSIZE;

	sc->personal_rqst = buf[offset];
	offset += sizeof(sc->personal_rqst);

	sc->veh_attr_flag = buf[offset];
	offset += sizeof(sc->veh_attr_flag);

	ldchar(&buf[offset],sizeof(sc->veh_attr)-1,sc->veh_attr);
	offset += sizeof(sc->veh_attr)-1;

	sc->drv_attr_flag = buf[offset];
	offset += sizeof(sc->drv_attr_flag);

	ldchar(&buf[offset],sizeof(sc->drv_attr)-1,sc->drv_attr);
	offset += sizeof(sc->drv_attr)-1;

	sc->nbr_of_vehs = buf[offset];
	offset += sizeof(sc->nbr_of_vehs);

	sc->priority = ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(sc->charge_nbr)-1,sc->charge_nbr);
	offset += sizeof(sc->charge_nbr)-1;

	ldchar(&buf[offset],sizeof(sc->requested_by)-1,sc->requested_by);
	offset += sizeof(sc->requested_by)-1;

	ldchar(&buf[offset],sizeof(sc->person_to_see)-1,sc->person_to_see);
	offset += sizeof(sc->person_to_see)-1;

	sc->pod_required = buf[offset];
	offset += sizeof(sc->pod_required);

	sc->call_rate = ldfloat(&buf[offset]);
	offset += FLOATSIZE;

	sc->from_date = ldlong(&buf[offset]);
	offset += LONGSIZE;

	sc->to_date = ldlong(&buf[offset]);
	offset += LONGSIZE;

        sc->mon_time = ldint(&buf[offset]);
        offset += INTSIZE;

        sc->mon_time_change = ldint(&buf[offset]);
        offset += INTSIZE;

        sc->tue_time = ldint(&buf[offset]);
        offset += INTSIZE;

        sc->tue_time_change = ldint(&buf[offset]);
        offset += INTSIZE;

        sc->wed_time = ldint(&buf[offset]);
        offset += INTSIZE;

        sc->wed_time_change = ldint(&buf[offset]);
        offset += INTSIZE;

        sc->thu_time = ldint(&buf[offset]);
        offset += INTSIZE;

        sc->thu_time_change = ldint(&buf[offset]);
        offset += INTSIZE;

        sc->fri_time = ldint(&buf[offset]);
        offset += INTSIZE;

        sc->fri_time_change = ldint(&buf[offset]);
        offset += INTSIZE;

        sc->sat_time = ldint(&buf[offset]);
        offset += INTSIZE;

        sc->sat_time_change = ldint(&buf[offset]);
        offset += INTSIZE;

        sc->sun_time = ldint(&buf[offset]);
        offset += INTSIZE;

        sc->sun_time_change = ldint(&buf[offset]);
        offset += INTSIZE;

        sc->date_time_in = ldlong(&buf[offset]);
        offset += LONGSIZE;

        sc->entered_by = ldint(&buf[offset]);
        offset += INTSIZE;

        sc->last_changed_by = ldint(&buf[offset]);
        offset += INTSIZE;

        sc->call_nbr1 = ldlong(&buf[offset]);
        offset += LONGSIZE;

        sc->call_nbr2 = ldlong(&buf[offset]);
        offset += LONGSIZE;

	ldchar(&buf[offset],sizeof(sc->date_in)-1,sc->date_in);
	offset += sizeof(sc->date_in)-1;

	ldchar(&buf[offset],sizeof(sc->time_in)-1,sc->time_in);
	offset += sizeof(sc->time_in)-1;

	ldchar(&buf[offset],sizeof(sc->ac_addt1_info1)-1,sc->ac_addt1_info1);
	offset += sizeof(sc->ac_addt1_info1)-1;

	ldchar(&buf[offset],sizeof(sc->ac_addt1_info2)-1,sc->ac_addt1_info2);
	offset += sizeof(sc->ac_addt1_info2)-1;

	ldchar(&buf[offset],sizeof(sc->status_bits)-1,sc->status_bits);
	offset += sizeof(sc->status_bits)-1;

	ldchar(&buf[offset],sizeof(sc->company_name)-1,sc->company_name);
	offset += sizeof(sc->company_name)-1;

	ldchar(&buf[offset],sizeof(sc->driver_msg)-1,sc->driver_msg);
	offset += sizeof(sc->driver_msg)-1;

	ldchar(&buf[offset],sizeof(sc->pckup_pmap)-1,sc->pckup_pmap);
	offset += sizeof(sc->pckup_pmap)-1;

	ldchar(&buf[offset],sizeof(sc->dest_dmap)-1,sc->dest_dmap);
	offset += sizeof(sc->dest_dmap)-1;

	ldchar(&buf[offset],sizeof(sc->pckup_str_key)-1,sc->pckup_str_key);
	offset += sizeof(sc->pckup_str_key)-1;

	ldchar(&buf[offset],sizeof(sc->pass_name)-1,sc->pass_name);
	offset += sizeof(sc->pass_name)-1;

	ldchar(&buf[offset],sizeof(sc->phone_number)-1,sc->phone_number);
	offset += sizeof(sc->phone_number)-1;

	sc->advise_arrival = buf[offset];
	offset += sizeof(sc->advise_arrival);

#ifdef NAMERICA	
	sc->send_dest = buf[offset];
	offset += sizeof(sc->send_dest);

	sc->send_rate = buf[offset];
	offset += sizeof(sc->send_rate);
#endif
}
