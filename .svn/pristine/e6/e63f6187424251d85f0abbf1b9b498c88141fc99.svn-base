static char sz__FILE__[]="@(#)fill_zone.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_zone.c; Rel: 6.3.0.0; Get date: 4/8/92 at 16:04:41
 * @(#)  Last delta: 1/16/92 at 16:36:59
 * @(#)  SCCS File: /taxi/sccs/s.fill_zone.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC
#include "taxipak.h"
#include "Object.h"
#include "Zone_db.h"

void
fill_zn_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_zn  *zone;
	register int  offset = 0;

	zone = (struct cisam_zn*)req_buf;
	stchar(&zone->fleet,&buf[offset],sizeof(zone->fleet));
	offset = sizeof(zone->fleet) + sizeof(zone->superzn_detail);
	stint(zone->nbr,&buf[offset]);

        stchar(zone->stand_telephone, &buf[260], sizeof(zone->stand_telephone));
}

void
fill_zn(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int    action;
{
	struct  cisam_zn  *zone;
	register int  offset = 0;
	register i;
	register j;

	zone = (struct cisam_zn*)req_buf;

	stchar(&zone->fleet,&buf[offset],sizeof(zone->fleet));
	offset += sizeof(zone->fleet);
	stchar(&zone->superzn_detail,&buf[offset],sizeof(zone->superzn_detail));
	offset += sizeof(zone->superzn_detail);
	stint(zone->nbr,&buf[offset]);
	offset += INTSIZE;
	stint(zone->superzn_nbr,&buf[offset]);
	offset += INTSIZE;
	stchar(zone->desc,&buf[offset],sizeof(zone->desc)-1);
	offset += sizeof(zone->desc)-1;
	stchar(&zone->type,&buf[offset],sizeof(zone->type));
	offset += sizeof(zone->type);
	stchar(&zone->bid_allowed,&buf[offset],sizeof(zone->bid_allowed));
	offset += sizeof(zone->bid_allowed);
	stchar(zone->pckup_seals,&buf[offset],sizeof(zone->pckup_seals)-1);
	offset += sizeof(zone->pckup_seals)-1;
	stint(zone->pckup_lead_time,&buf[offset]);
	offset += INTSIZE;
	stint(zone->stand_time_from,&buf[offset]);
	offset += INTSIZE;
	stint(zone->stand_time_to,&buf[offset]);
	offset += INTSIZE;

	stint(zone->avl_call_q_time,&buf[offset]);
	offset += INTSIZE;

	stchar(&zone->enr_post_allow,&buf[offset],sizeof(zone->enr_post_allow));
	offset += sizeof(zone->enr_post_allow);

	buf[offset] = zone->lev_rej_penlty[1];		/* do for primary */
	offset += CHARSIZE;

	stint(zone->lev_excl_time[1],&buf[offset]);		/* do for primary */
	offset += INTSIZE;

	for(i=0;i<LEVEL_MAX;i++){
		if(i == 1)
			continue;
		buf[offset] = zone->lev_alg[i] ;
		offset += CHARSIZE;
	}

	for(i=0;i<LEVEL_MAX;i++){
		if(i == 1)
			continue;
		for(j=0;j<BCK_UP_MAX;j++){
			stint(zone->lev_zone_nbr[i][j],&buf[offset]);
			offset += INTSIZE;
		}
	}

	for(i=0;i<LEVEL_MAX;i++){
		if(i == 1)
			continue;
		stint(zone->lev_excl_time[i],&buf[offset]);
		offset += INTSIZE;
	}

	for(i=0;i<LEVEL_MAX;i++){
		if(i == 1)
			continue;
		buf[offset] = zone->lev_rej_penlty[i];
		offset += CHARSIZE;
	}

	stint(zone->max_time_calls,&buf[offset]);
	offset += INTSIZE;

	stint(zone->mdt_screen, &buf[offset]);
	offset += INTSIZE;

	stint(zone->enr_post_lead, &buf[offset]);
	offset += INTSIZE;

	stint(zone->break_duration, &buf[offset]);
	offset += INTSIZE;

	stint(zone->accept_duration, &buf[offset]);
	offset += INTSIZE;

	stchar(&zone->check_avail_taxis,&buf[offset],sizeof(zone->check_avail_taxis));
	offset += sizeof(zone->check_avail_taxis);

	stdbl(zone->gps_lat, &buf[offset]);
	offset += DOUBLESIZE;

	stdbl(zone->gps_long, &buf[offset]);
	offset += DOUBLESIZE;

	stlong(zone->gps_radius, &buf[offset]);
	offset += LONGSIZE;

	stlong(zone->gps_dist_ceiling, &buf[offset]);
	offset += LONGSIZE;

	buf[offset] = zone->match_algorithm;
	offset += sizeof(zone->match_algorithm);

	stint(zone->tc_look_ahead, &buf[offset]);
	offset += INTSIZE;

	stint(zone->tc_restrict_interval, &buf[offset]);
	offset += INTSIZE;

	stint(zone->ns_short, &buf[offset]);
	offset += INTSIZE;

	stint(zone->emerg_range, &buf[offset]);
	offset += INTSIZE;

	stchar(zone->stand_telephone,&buf[offset],sizeof(zone->stand_telephone) - 1);
	offset += sizeof(zone->stand_telephone) - 1;

        stint(zone->stand_hold_timer, &buf[offset]);
	offset += INTSIZE;

	stint(zone->cbook_time, &buf[offset]);
	offset += INTSIZE;

	stint(zone->roti_time, &buf[offset]);
	offset += INTSIZE;

	stint(zone->stand_nbr, &buf[offset]);
	offset += INTSIZE;

	stint(zone->inquiry_backup[0], &buf[offset]);
	offset += INTSIZE;

	stint(zone->inquiry_backup[1], &buf[offset]);
	offset += INTSIZE;
	
	stint(zone->inquiry_backup[2], &buf[offset]);
	offset += INTSIZE;
	
	stint(zone->inquiry_backup[3], &buf[offset]);
	offset += INTSIZE;
	
	stint(zone->inquiry_backup[4], &buf[offset]);
	offset += INTSIZE;
	
	stint(zone->inquiry_backup[5], &buf[offset]);
	offset += INTSIZE;
	
	stint(zone->inquiry_backup[6], &buf[offset]);
	offset += INTSIZE;
	
	stint(zone->inquiry_backup[7], &buf[offset]);
	offset += INTSIZE;
	
	stint(zone->inquiry_backup[8], &buf[offset]);
	offset += INTSIZE;
	
	stint(zone->inquiry_backup[9], &buf[offset]);
	offset += INTSIZE;	

	stchar(zone->poly_checksum,&buf[offset], sizeof(zone->poly_checksum)-1);
	offset += sizeof(zone->poly_checksum)-1;

	stint(zone->typex_lead_time, &buf[offset]);
	offset += INTSIZE;
	
}

void
unfill_zn(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_zn  *zone;
	register int  offset = 0;
	register i;
	register j;
	short code = 0;

	zone = (struct cisam_zn*)req_buf;

	zone->fleet = buf[offset];
	offset += CHARSIZE;

	zone->superzn_detail = buf[offset];
	offset += CHARSIZE;

	zone->nbr = ldint(&buf[offset]);
	offset += INTSIZE;

	zone->superzn_nbr = ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(zone->desc)-1,zone->desc);
	offset += sizeof(zone->desc)-1;

	zone->type = buf[offset];
	offset += sizeof(zone->type);

	zone->bid_allowed = buf[offset];
	offset += sizeof(zone->bid_allowed);

	ldchar(&buf[offset],sizeof(zone->pckup_seals)-1,zone->pckup_seals);
	offset += sizeof(zone->pckup_seals)-1;

	zone->pckup_lead_time = ldint(&buf[offset]);
	offset += INTSIZE;

	zone->stand_time_from = ldint(&buf[offset]);
	offset += INTSIZE;

	zone->stand_time_to = ldint(&buf[offset]);
	offset += INTSIZE;

	zone->avl_call_q_time = ldint(&buf[offset]);
	offset += INTSIZE;

	zone->enr_post_allow = buf[offset];
	offset += sizeof(zone->enr_post_allow);

	zone->lev_rej_penlty[1] = buf[offset];
	offset += CHARSIZE;

	zone->lev_excl_time[1] = ldint(&buf[offset]);
	offset += INTSIZE;

	for(i=0;i<LEVEL_MAX;i++){
		if(i == 1)
			continue;
		zone->lev_alg[i] = buf[offset];
		offset += CHARSIZE;
	}

	for(i=0;i<LEVEL_MAX;i++){
		if(i == 1)
			continue;
		for(j=0;j<BCK_UP_MAX;j++){
			zone->lev_zone_nbr[i][j] = ldint(&buf[offset]);
			offset += INTSIZE;
		}
	}

	for(i=0;i<LEVEL_MAX;i++){
		if(i == 1)
			continue;
		zone->lev_excl_time[i] = ldint(&buf[offset]);
		offset += INTSIZE;
	}

	for(i=0;i<LEVEL_MAX;i++){
		if(i == 1)
			continue;
		zone->lev_rej_penlty[i] = buf[offset];
		offset += CHARSIZE;
	}

	zone->max_time_calls = ldint(&buf[offset]);
	offset += INTSIZE;

	zone->mdt_screen = ldint(&buf[offset]);
	offset += INTSIZE;

	zone->enr_post_lead = ldint(&buf[offset]);
	offset += INTSIZE;

	zone->break_duration = ldint(&buf[offset]);
	offset += INTSIZE;

	zone->accept_duration = ldint(&buf[offset]);
	offset += INTSIZE;

	zone->check_avail_taxis = buf[offset];
	offset += CHARSIZE;

	zone->gps_lat = lddblnull(&buf[offset], &code);
	offset += DOUBLESIZE;

	zone->gps_long = lddblnull(&buf[offset], &code);
	offset += DOUBLESIZE;

	zone->gps_radius = ldlong(&buf[offset]);
	offset += LONGSIZE;

	zone->gps_dist_ceiling = ldlong(&buf[offset]);
	offset += LONGSIZE;

	zone->match_algorithm = buf[offset];
	offset += sizeof(zone->match_algorithm);

	zone->tc_look_ahead = ldint(&buf[offset]);
	offset += INTSIZE;

	zone->tc_restrict_interval = ldint(&buf[offset]);
	offset += INTSIZE;

	zone->ns_short = ldint(&buf[offset]);
	offset += INTSIZE;

	zone->emerg_range = ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(zone->stand_telephone)-1,zone->stand_telephone);
	offset += sizeof(zone->stand_telephone)-1;

        zone->stand_hold_timer = ldint(&buf[offset]);
        offset += INTSIZE;

	zone->cbook_time = ldint(&buf[offset]);
	offset += INTSIZE;

	zone->roti_time = ldint(&buf[offset]);
	offset += INTSIZE;

	zone->stand_nbr = ldint(&buf[offset]);
	offset += INTSIZE;

	zone->inquiry_backup[0] = ldint(&buf[offset]);
	offset += INTSIZE;
	zone->inquiry_backup[1] = ldint(&buf[offset]);
	offset += INTSIZE;
	zone->inquiry_backup[2] = ldint(&buf[offset]);
	offset += INTSIZE;
	zone->inquiry_backup[3] = ldint(&buf[offset]);
	offset += INTSIZE;
	zone->inquiry_backup[4] = ldint(&buf[offset]);
	offset += INTSIZE;
	zone->inquiry_backup[5] = ldint(&buf[offset]);
	offset += INTSIZE;
	zone->inquiry_backup[6] = ldint(&buf[offset]);
	offset += INTSIZE;
	zone->inquiry_backup[7] = ldint(&buf[offset]);
	offset += INTSIZE;
	zone->inquiry_backup[8] = ldint(&buf[offset]);
	offset += INTSIZE;
	zone->inquiry_backup[9] = ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(zone->poly_checksum)-1,zone->poly_checksum);
	offset += sizeof(zone->poly_checksum)-1;

	zone->typex_lead_time = ldint(&buf[offset]);
	offset += INTSIZE;
	
}




