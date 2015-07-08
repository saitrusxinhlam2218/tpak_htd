/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: taxi_db.h,v $
*  @(#)  $Revision: 1.15 $
*  @(#)  $Date: 2004/08/23 10:20:45 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/taxi_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: taxi_db.h,v $	$Revision: 1.15 $"

#include <isam.h>
#ifdef _MYSQL_
#include <mysql.h>
#endif

#ifndef _TAXI_DB_H_
#define _TAXI_DB_H_

#include "db.h"
#include "fill.h"
#include "mads_types.h"

#define FILE_NAME_LEN	40
#define SUCCEED         0
#define WHOLEKEY        0
#define READ_KEYS       0
#define READ_NUM        1
#define UPDATE_UNIQ     2
#define UPDATE_REC      3
#define WRITE           4
#define RE_WRITE        5
#define START           6
#define ADD_REC         7
#define LOCK            8
#define UNLOCK          9
#define RELEASE         10
#define OPEN            11 
#define CLOSE           12
#define DELETE_UNIQ     13
#define DELETE          14
#define BUILD           15
#define ADDINX          16
#define REDN_ADD	17		/* For add on reduntant unit only */
#define FILE_UPDATE     99              /* For flat file renduncy */


#define CALL_FILE_ID  	0	/* Call file */
#define CALLH_FILE_ID   1       /* Call history file*/
#define DRIVER_FILE_ID 	2	/* Driver file */
#define VEHICLE_FILE_ID 3	/* Vehicle file */
#define ZONE_FILE_ID 	4	/* Zone file */
#define FLEET_FILE_ID 	5	/* Fleet file */
#define ADDRESS_FILE_ID 6	/* Address file */
#define ACCOUNT_FILE_ID 7       /* Account file */
#define ATTR_FILE_ID    8       /* Attributes file */
#define EMERG_FILE_ID   9       /* Emergency log file */
#define ERROR_FILE_ID   10      /* Error log file */
#define EXC_TYPE_ID     11	/* Exception type file */
#define EXC_GROUP_ID 	12	/* Exception group file */
#define INT_FILE_ID     13      /* Intersection file */
#define LOCALITY_FILE_ID 14     /* City file */
#define LOCALTY_FILE_ID 14      /* City file  - for backwards compatibility */
#define HELP_FILE_ID	15	/* Help file */
#define HOLIDAY_FILE_ID	16	/* Holiday file */
#define PLACENM_FILE_ID	17 	/* Place name file */ 
#define MENU_FILE_ID 	18	/* Menu file */
#define MAILBOX_FILE_ID	19	/* Mailbox file */
#define MSGLOG_FILE_ID	20	/* Message log file */
#define RATE_FILE_ID	21	/* rate file */
#define RPTMSG_FILE_ID	22	/* Repeat message */
#define SEAL_FILE_ID	23	/* Seal file */
#define STRNAME_FILE_ID	24	/*Street name file */
#define STRTYPE_FILE_ID	25	/* Street type file */
#define SUBCALL_FILE_ID	26	/* Sub call file */		
#define SUBZONE_FILE_ID	27	/* Sub call file */		
#define SYSCNTL_FILE_ID	28	/* Sub call file */		
#define UNZONE_FILE_ID	29	/* Sub call file */		
#define USER_FILE_ID  	30	/* User Profile file */ 
#define CANMSG_FILE_ID 	31	/* Canned message file */ 
#define STATDRV_FILE_ID	32	/* Stat driver file */ 
#define STATUSR_FILE_ID 33	/* Stat user file */ 
#define STATS_FILE_ID 	34	/* Stat user file */ 
#define SHIFT_FILE_ID 	35	/* Daily call file */ 
#define SHIFTH_FILE_ID 	36	/* Daily call file */ 

/* The two files below are for the shift scheduler. Do not get
   them confused with the above SHIFT files. The above SHIFT
   files are one day's worth of calls that are copied from the
   CALL and CALLH files */
#define S_SHIFT_FILE_ID (SHIFTH_FILE_ID + 1)
#define S_SCHED_FILE_ID (S_SHIFT_FILE_ID + 1)
#define CALLMSG_FILE_ID (S_SCHED_FILE_ID + 1)  /* Call message records for Samplan device messages */
#define HIST_CMT_FILE_ID (CALLMSG_FILE_ID + 1)
#define CUSTOMER_FILE_ID (HIST_CMT_FILE_ID + 1)
#define RP_FILE_ID      (CUSTOMER_FILE_ID + 1)
#define TIME_CALL_FILE_ID (RP_FILE_ID + 1)
#define TLG_FILE_ID        (TIME_CALL_FILE_ID + 1)      /* Telephone line group file  */
#define CHANNEL_FILE_ID (TLG_FILE_ID + 1)      /* Channel file       */
#define ACAD_FILE_ID    (CHANNEL_FILE_ID + 1)
#define INDV_FILE_ID    (ACAD_FILE_ID + 1)
#define RECEIPT_DATA_FILE_ID (INDV_FILE_ID + 1)
#define BAD_CREDIT_FILE_ID   (RECEIPT_DATA_FILE_ID + 1)
#define CARD_TYPE_FILE_ID    (BAD_CREDIT_FILE_ID + 1)
#define GPS_PARAMS_FILE_ID   (CARD_TYPE_FILE_ID + 1)
#define SYSMENUS_FILE_ID     (GPS_PARAMS_FILE_ID + 1)
#define SYSITEMS_FILE_ID     (SYSMENUS_FILE_ID + 1)
#define VEH_CMSG_FILE_ID     (SYSITEMS_FILE_ID + 1)
#define TL_FILE_ID           (VEH_CMSG_FILE_ID + 1)
#define SUBCALLH_FILE_ID     (TL_FILE_ID + 1)
#define ZONEPOLY_FILE_ID     (SUBCALLH_FILE_ID + 1)
#define WEBUSER_FILE_ID      (ZONEPOLY_FILE_ID + 1)
#define WEBADDR_FILE_ID      (WEBUSER_FILE_ID + 1)
#define FTJ_BAD_FILE_ID      (WEBADDR_FILE_ID + 1)
#define LINE_MGR_FILE_ID     (FTJ_BAD_FILE_ID + 1)
#define GEOADDR_ADDR_FILE_ID (LINE_MGR_FILE_ID + 1)
#define GEOADDR_LINE_FILE_ID (GEOADDR_ADDR_FILE_ID + 1)
#define GEOADDR_POINT_FILE_ID (GEOADDR_LINE_FILE_ID + 1)
#define ZONETRANS_FILE_ID    (GEOADDR_POINT_FILE_ID + 1)
#define ATTRANS_FILE_ID      (ZONETRANS_FILE_ID + 1)
#define ILINK_FILE_ID        (ATTRANS_FILE_ID + 1)
#define IMSI_FILE_ID         (ILINK_FILE_ID + 1)
#define S_VERSION_FILE_ID    (IMSI_FILE_ID + 1)
#define ZONEDOWN_FILE_ID     (S_VERSION_FILE_ID + 1)
#define KELANODE_FILE_ID     (ZONEDOWN_FILE_ID + 1)
#define DB_NUM_FILES	KELANODE_FILE_ID + 1



#ifdef DB_SRC

#ifdef _MYSQL_
DB_MYSQL_FILES Db_mysql_files[] =
{
{CALL_FILE,	fill_my_cl, unfill_my_cl, fill_my_cl_key, -1, NULL, sizeof(CALL_REC), (MYSQL_RES *)NULL, 0},
{ CALLH_FILE, NULL, NULL, NULL, -1, NULL, sizeof(CALL_HIST_REC), (MYSQL_RES *)NULL, 0},
{ DRIVER_FILE, fill_my_dr, unfill_my_dr, fill_my_dr_key, -1, NULL, sizeof(DRIVER_REC), (MYSQL_RES *)NULL, 0},  
//{CALLH_FILE,	fill_my_ch, unfill_my_ch, fill_my_ch_key, -1, NULL, sizeof(CALL_HIST_REC),(MYSQL_RES *)NULL,0},
{DRIVER_FILE,	fill_my_dr, unfill_my_dr, fill_my_dr_key, -1, NULL, sizeof(DRIVER_REC),(MYSQL_RES *)NULL,0},
{VEHICLE_FILE, 	fill_my_vh, unfill_my_vh, fill_my_vh_key, -1, NULL, sizeof(VEH_REC),(MYSQL_RES *)NULL,0},
{ ZONE_FILE, NULL, NULL, NULL, -1, NULL, sizeof(ZONE_REC), (MYSQL_RES *)NULL, 0},    
  //{ZONE_FILE, 	fill_my_zn, unfill_my_zn, fill_my_zn_key, -1, NULL, sizeof(ZONE_REC),(MYSQL_RES *)NULL,0},
{FLEET_FILE, 	fill_my_fl, unfill_my_fl, fill_my_fl_key, -1, NULL, sizeof(FLEET_REC),(MYSQL_RES *)NULL,0},
#ifdef FOO	  
{ADDRESS_FILE, 	fill_my_ad, unfill_my_ad, fill_my_ad_key, -1, NULL, sizeof(ADDRESS_REC),(MYSQL_RES *)NULL,0},
{ACCOUNT_FILE, 	fill_my_ac, unfill_my_ac, fill_my_ac_key, -1, NULL, sizeof(ACCOUNT_REC),(MYSQL_RES *)NULL,0},
{ATTR_FILE, 	fill_my_at, unfill_my_at, fill_my_at_key, -1, NULL, sizeof(ATTRIB_REC),(MYSQL_RES *)NULL,0},
{EMERG_FILE, 	fill_my_em, unfill_my_em, fill_my_em_key, -1, NULL, sizeof(EMERG_REC),(MYSQL_RES *)NULL,0},
{ERROR_FILE, 	fill_my_el, unfill_my_el, fill_my_el_key, -1, NULL, sizeof(ERROR_REC),(MYSQL_RES *)NULL,0},
{EXCTYPE_FILE, 	fill_my_et, unfill_my_et, fill_my_et_key, -1, NULL, sizeof(EXCEPT_TYPE_REC),(MYSQL_RES *)NULL,0},
{EXCGRP_FILE, 	fill_my_eg, unfill_my_eg, fill_my_eg_key, -1, NULL, sizeof(EXCEPT_GROUP_REC),(MYSQL_RES *)NULL,0},
{INTERST_FILE, 	fill_my_in, unfill_my_in, fill_my_in_key, -1, NULL, sizeof(INTERSECT_REC),(MYSQL_RES *)NULL,0},
{LOCALTY_FILE, 	fill_my_lo, unfill_my_lo, fill_my_lo_key, -1, NULL, sizeof(LOC_REC),(MYSQL_RES *)NULL,0},
{HELP_FILE, 	fill_my_hp, unfill_my_hp, fill_my_hp_key, -1, NULL, sizeof(HELP_REC),(MYSQL_RES *)NULL,0},
{HOLIDAY_FILE, 	fill_my_hl, unfill_my_hl, fill_my_hl_key, -1, NULL, sizeof(HOLIDAY_REC),(MYSQL_RES *)NULL,0},
{PLACENM_FILE, 	fill_my_pl, unfill_my_pl, fill_my_pl_key, -1, NULL, sizeof(PLACENAME_REC),(MYSQL_RES *)NULL,0},
{MENU_FILE, 	fill_my_mf, unfill_my_mf, fill_my_mf_key, -1, NULL, sizeof(MENU_REC),(MYSQL_RES *)NULL,0},
{MAILBOX_FILE, 	fill_my_mb, unfill_my_mb, fill_my_mb_key, -1, NULL, sizeof(MAILBOX_REC),(MYSQL_RES *)NULL,0},
{MSGLOG_FILE, 	fill_my_ml, unfill_my_ml, fill_my_ml_key, -1, NULL, sizeof(MSGLOG_REC),(MYSQL_RES *)NULL,0},
{RATE_FILE, 	fill_my_rt, unfill_my_rt, fill_my_rt_key, -1, NULL, sizeof(RATE_REC),(MYSQL_RES *)NULL,0},
{RPTMSG_FILE, 	fill_my_rm, unfill_my_rm, fill_my_rm_key, -1, NULL, sizeof(REPEAT_MSG_REC),(MYSQL_RES *)NULL,0},
{SEAL_FILE, 	fill_my_sl, unfill_my_sl, fill_my_sl_key, -1, NULL, sizeof(SEAL_REC),(MYSQL_RES *)NULL,0},
{STRNAME_FILE, 	fill_my_sn, unfill_my_sn, fill_my_sn_key, -1, NULL, sizeof(STREET_NAME_REC),(MYSQL_RES *)NULL,0},
{STRTYPE_FILE, 	fill_my_st, unfill_my_st, fill_my_st_key, -1, NULL, sizeof(STREET_TYPE_REC),(MYSQL_RES *)NULL,0},
{SUBCALL_FILE, 	fill_my_sc, unfill_my_sc, fill_my_sc_key, -1, NULL, sizeof(SUBCALL_REC),(MYSQL_RES *)NULL,0},
{SUBZONE_FILE, 	fill_my_sz, unfill_my_sz, fill_my_sz_key, -1, NULL, sizeof(SUBZONE_REC),(MYSQL_RES *)NULL,0},
{SYSCNTL_FILE, 	fill_my_syc, unfill_my_syc, fill_my_syc_key, -1, NULL, sizeof(SYSCONTROL_REC),(MYSQL_RES *)NULL,0},
{UNZONE_FILE, 	fill_my_uz, unfill_my_uz, fill_my_uz_key, -1, NULL, sizeof(UNZONE_REC),(MYSQL_RES *)NULL,0},
{PROFILE_FILE, 	fill_my_up, unfill_my_up, fill_my_up_key, -1, NULL, sizeof(USER_REC),(MYSQL_RES *)NULL,0},
{CANMSG_FILE, 	fill_my_cm, unfill_my_cm, fill_my_cm_key, -1, NULL, sizeof(CANMSG_REC),(MYSQL_RES *)NULL,0},
{STATDRV_FILE, 	fill_my_sd, unfill_my_sd, fill_my_sd_key, -1, NULL, sizeof(DRIVER_STATS_REC),(MYSQL_RES *)NULL,0},
{STATUSR_FILE, 	fill_my_su, unfill_my_su, fill_my_su_key, -1, NULL, sizeof(USER_STATS_REC),(MYSQL_RES *)NULL,0},
{STATS_FILE, 	fill_my_sa, unfill_my_sa, fill_my_sa_key, -1, NULL, sizeof(STATS_REC),(MYSQL_RES *)NULL,0},
{SHIFT_FILE,	fill_my_shift, unfill_my_cl, fill_my_cl_key, -1, NULL, sizeof(CALL_REC),(MYSQL_RES *)NULL,0},
{SHIFTH_FILE,	fill_my_hh, unfill_my_ch, fill_my_ch_key, -1, NULL, sizeof(CALL_HIST_REC),(MYSQL_RES *)NULL,0},
{CALLMSG_FILE,  fill_my_cmsg, unfill_my_cmsg, fill_my_cmsg_key, -1, NULL, sizeof(CMSG_REC),(MYSQL_RES *)NULL,0},
{HIST_CMT_FILE, fill_my_hc, unfill_my_hc, fill_my_hc_key, -1, NULL, sizeof(HIST_CMT_REC),(MYSQL_RES *)NULL,0},
{CUSTOMER_FILE, fill_my_cust, unfill_my_cust, fill_my_cust_key,  -1, NULL, sizeof(CUSTOMERS),(MYSQL_RES *)NULL,0},
{RP_FILE, fill_my_rp, unfill_my_rp, fill_my_rp_key,  -1, NULL, sizeof(RP_REC),(MYSQL_RES *)NULL,0},
{TIME_CALL_FILE, fill_my_time_call, unfill_my_time_call, fill_my_time_call_key, -1, NULL, sizeof( TIME_CALL_REC ),(MYSQL_RES *)NULL,0},
{TLG_FILE, fill_my_tlg, unfill_my_tlg, fill_my_tlg_key,  -1, NULL, sizeof(TLG_REC),(MYSQL_RES *)NULL,0},
{CHANNEL_FILE, fill_my_channel, unfill_my_channel, fill_my_channel_key,  -1, NULL, sizeof (CHANNEL_REC),(MYSQL_RES *)NULL,0},
{ACAD_FILE, fill_my_acad, unfill_my_acad, fill_my_acad_key, -1, NULL, sizeof( ACAD_REC ),(MYSQL_RES *)NULL,0},
{INDV_FILE, fill_my_indv, unfill_my_indv, fill_my_indv_key, -1, NULL, sizeof( INDV_REC ),(MYSQL_RES *)NULL,0},
{RECEIPT_DATA_FILE, fill_my_receipt, unfill_my_receipt, fill_my_receipt_key, -1, NULL, sizeof(RECEIPT_DATA_REC),(MYSQL_RES *)NULL,0},
{BAD_CRED_FILE, fill_my_bad_credit, unfill_my_bad_credit, fill_my_bad_credit_key, -1, NULL, sizeof(BAD_CREDIT_REC),(MYSQL_RES *)NULL,0},
{CARD_TYPE_FILE, fill_my_card_type, unfill_my_card_type, fill_my_card_type_key, -1, NULL, sizeof(CARD_TYPE_REC),(MYSQL_RES *)NULL,0},
{GPS_PARAMS_FILE, fill_my_gps_params, unfill_my_gps_params, fill_my_gps_params_key, -1, NULL, sizeof(GPS_PARAMS),(MYSQL_RES *)NULL,0},
{SYSMENUS_FILE, fill_my_sysmenus, unfill_my_sysmenus, fill_my_sysmenus_key, -1, NULL, sizeof(SYSMENUS_REC),(MYSQL_RES *)NULL,0},
{SYSITEMS_FILE, fill_my_sysitems, unfill_my_sysitems, fill_my_sysitems_key, -1, NULL, sizeof(SYSITEMS_REC),(MYSQL_RES *)NULL,0},
{VEH_CMSG_FILE, fill_my_veh_cm, unfill_my_veh_cm, fill_my_veh_cm_key, -1, NULL, sizeof(VEH_CANMSG_REC),(MYSQL_RES *)NULL,0},
{TL_FILE, fill_my_tl, unfill_my_tl, fill_my_tl_key, -1, NULL, sizeof(TL_STRUCT),(MYSQL_RES *)NULL,0},
{SUBCALLH_FILE, fill_my_subch, unfill_my_subch, fill_my_subch_key, -1, NULL, sizeof(SUBCALL_HIST_REC),(MYSQL_RES *)NULL,0},
{ZONEPOLY_FILE, fill_my_zonepoly, unfill_my_zonepoly, fill_my_zonepoly_key, -1, NULL, sizeof(ZONE_POLY_REC),(MYSQL_RES *)NULL,0},
{WEBUSER_FILE, fill_my_webuser, unfill_my_webuser, fill_my_webuser_key, -1, NULL, sizeof(WEBUSER),(MYSQL_RES *)NULL,0},
{WEBADDR_FILE, fill_my_webaddr, unfill_my_webaddr, fill_my_webaddr_key, -1, NULL, sizeof(WEBADDR),(MYSQL_RES *)NULL,0},
{FTJ_BAD_FILE, fill_my_ftj_bad, unfill_my_ftj_bad, fill_my_ftj_bad_key, -1, NULL, sizeof(FTJ_BAD_STRUCT),(MYSQL_RES *)NULL,0},
{LINE_MGR_FILE, fill_my_line_mgr, unfill_my_line_mgr, fill_my_line_mgr_key, -1, NULL, sizeof(LINE_MGR_DB_REC),(MYSQL_RES *)NULL,0},
{GEOADDR_ADDR_FILE, fill_my_geoaddr_addr, unfill_my_geoaddr_addr, fill_my_geoaddr_addr_key, -1, NULL, sizeof(GEOADDR_ADDR_REC),(MYSQL_RES *)NULL,0},
{GEOADDR_LINE_FILE, fill_my_geoaddr_line, unfill_my_geoaddr_line, fill_my_geoaddr_line_key, -1, NULL, sizeof(GEOADDR_LINE_REC),(MYSQL_RES *)NULL,0},
{GEOADDR_POINT_FILE, fill_my_geoaddr_point, unfill_my_geoaddr_point, fill_my_geoaddr_point_key, -1, NULL, sizeof(GEOADDR_POINT_REC),(MYSQL_RES *)NULL,0},
{ZONETRANS_FILE, fill_my_zntrans, unfill_my_zntrans, fill_my_zntrans_key, -1, NULL, sizeof(ZONETRANS_DB_REC),(MYSQL_RES *)NULL,0},
{ATTRANS_FILE, fill_my_attrans, unfill_my_attrans, fill_my_attrans_key, -1, NULL, sizeof(ATTRANS_DB_REC),(MYSQL_RES *)NULL,0},
{ILINK_FILE, fill_my_ilink, unfill_my_ilink, fill_my_ilink_key, -1, NULL, sizeof(ILINK_DB_REC),(MYSQL_RES *)NULL,0},
{IMSI_FILE, fill_my_imsi, unfill_my_imsi, fill_my_imsi_key, -1, NULL, sizeof(IMSI_REC),(MYSQL_RES *)NULL,0},
#endif
};
#endif

#ifndef _MYSQL_
DB_FILES Db_files[] =
{
{CALL_FILE,	fill_cl, unfill_cl, fill_cl_key, -1, NULL, sizeof(CALL_REC)},
{CALLH_FILE,	fill_ch, unfill_ch, fill_ch_key, -1, NULL, sizeof(CALL_HIST_REC)},
{DRIVER_FILE,	fill_dr, unfill_dr, fill_dr_key, -1, NULL, sizeof(DRIVER_REC)},
{VEHICLE_FILE, 	fill_vh, unfill_vh, fill_vh_key, -1, NULL, sizeof(VEH_REC)},
{ZONE_FILE, 	fill_zn, unfill_zn, fill_zn_key, -1, NULL, sizeof(ZONE_REC)},
{FLEET_FILE, 	fill_fl, unfill_fl, fill_fl_key, -1, NULL, sizeof(FLEET_REC)},
{ADDRESS_FILE, 	fill_ad, unfill_ad, fill_ad_key, -1, NULL, sizeof(ADDRESS_REC)},
{ACCOUNT_FILE, 	fill_ac, unfill_ac, fill_ac_key, -1, NULL, sizeof(ACCOUNT_REC)},
{ATTR_FILE, 	fill_at, unfill_at, fill_at_key, -1, NULL, sizeof(ATTRIB_REC)},
{EMERG_FILE, 	fill_em, unfill_em, fill_em_key, -1, NULL, sizeof(EMERG_REC)},
{ERROR_FILE, 	fill_el, unfill_el, fill_el_key, -1, NULL, sizeof(ERROR_REC)},
{EXCTYPE_FILE, 	fill_et, unfill_et, fill_et_key, -1, NULL, sizeof(EXCEPT_TYPE_REC)},
{EXCGRP_FILE, 	fill_eg, unfill_eg, fill_eg_key, -1, NULL, sizeof(EXCEPT_GROUP_REC)},
{INTERST_FILE, 	fill_in, unfill_in, fill_in_key, -1, NULL, sizeof(INTERSECT_REC)},
{LOCALTY_FILE, 	fill_lo, unfill_lo, fill_lo_key, -1, NULL, sizeof(LOC_REC)},
{HELP_FILE, 	fill_hp, unfill_hp, fill_hp_key, -1, NULL, sizeof(HELP_REC)},
{HOLIDAY_FILE, 	fill_hl, unfill_hl, fill_hl_key, -1, NULL, sizeof(HOLIDAY_REC)},
{PLACENM_FILE, 	fill_pl, unfill_pl, fill_pl_key, -1, NULL, sizeof(PLACENAME_REC)},
{MENU_FILE, 	fill_mf, unfill_mf, fill_mf_key, -1, NULL, sizeof(MENU_REC)},
{MAILBOX_FILE, 	fill_mb, unfill_mb, fill_mb_key, -1, NULL, sizeof(MAILBOX_REC)},
{MSGLOG_FILE, 	fill_ml, unfill_ml, fill_ml_key, -1, NULL, sizeof(MSGLOG_REC)},
{RATE_FILE, 	fill_rt, unfill_rt, fill_rt_key, -1, NULL, sizeof(RATE_REC)},
{RPTMSG_FILE, 	fill_rm, unfill_rm, fill_rm_key, -1, NULL, sizeof(REPEAT_MSG_REC)},
{SEAL_FILE, 	fill_sl, unfill_sl, fill_sl_key, -1, NULL, sizeof(SEAL_REC)},
{STRNAME_FILE, 	fill_sn, unfill_sn, fill_sn_key, -1, NULL, sizeof(STREET_NAME_REC)},
{STRTYPE_FILE, 	fill_st, unfill_st, fill_st_key, -1, NULL, sizeof(STREET_TYPE_REC)},
{SUBCALL_FILE, 	fill_sc, unfill_sc, fill_sc_key, -1, NULL, sizeof(SUBCALL_REC)},
{SUBZONE_FILE, 	fill_sz, unfill_sz, fill_sz_key, -1, NULL, sizeof(SUBZONE_REC)},
{SYSCNTL_FILE, 	fill_syc, unfill_syc, fill_syc_key, -1, NULL, sizeof(SYSCONTROL_REC)},
{UNZONE_FILE, 	fill_uz, unfill_uz, fill_uz_key, -1, NULL, sizeof(UNZONE_REC)},
{PROFILE_FILE, 	fill_up, unfill_up, fill_up_key, -1, NULL, sizeof(USER_REC)},
{CANMSG_FILE, 	fill_cm, unfill_cm, fill_cm_key, -1, NULL, sizeof(CANMSG_REC)},
{STATDRV_FILE, 	fill_sd, unfill_sd, fill_sd_key, -1, NULL, sizeof(DRIVER_STATS_REC)},
{STATUSR_FILE, 	fill_su, unfill_su, fill_su_key, -1, NULL, sizeof(USER_STATS_REC)},
{STATS_FILE, 	fill_sa, unfill_sa, fill_sa_key, -1, NULL, sizeof(STATS_REC)},
{SHIFT_FILE,	fill_shift, unfill_cl, fill_cl_key, -1, NULL, sizeof(CALL_REC)},
{SHIFTH_FILE,	fill_hh, unfill_ch, fill_ch_key, -1, NULL, sizeof(CALL_HIST_REC)},
{S_SHIFT_FILE,  fill_ss, unfill_ss, fill_ss_key, -1, NULL, sizeof(S_SHIFT_REC)},
{S_SCHED_FILE,  fill_sh, unfill_sh, fill_sh_key, -1, NULL, sizeof(S_SCHED_REC)},
{CALLMSG_FILE,  fill_cmsg, unfill_cmsg, fill_cmsg_key, -1, NULL, sizeof(CMSG_REC)},
{HIST_CMT_FILE, fill_hc, unfill_hc, fill_hc_key, -1, NULL, sizeof(HIST_CMT_REC)},
{CUSTOMER_FILE, fill_cust, unfill_cust, fill_cust_key,  -1, NULL, sizeof(CUSTOMERS)},
{RP_FILE, fill_rp, unfill_rp, fill_rp_key,  -1, NULL, sizeof(RP_REC)},
{TIME_CALL_FILE, fill_time_call, unfill_time_call, fill_time_call_key, -1, NULL, sizeof( TIME_CALL_REC )},
{TLG_FILE, fill_tlg, unfill_tlg, fill_tlg_key,  -1, NULL, sizeof(TLG_REC)},
{CHANNEL_FILE, fill_channel, unfill_channel, fill_channel_key,  -1, NULL, sizeof (CHANNEL_REC)},
{ACAD_FILE, fill_acad, unfill_acad, fill_acad_key, -1, NULL, sizeof( ACAD_REC )},
{INDV_FILE, fill_indv, unfill_indv, fill_indv_key, -1, NULL, sizeof( INDV_REC )},
{RECEIPT_DATA_FILE, fill_receipt, unfill_receipt, fill_receipt_key, -1, NULL, sizeof(RECEIPT_DATA_REC)},
{BAD_CRED_FILE, fill_bad_credit, unfill_bad_credit, fill_bad_credit_key, -1, NULL, sizeof(BAD_CREDIT_REC)},
{CARD_TYPE_FILE, fill_card_type, unfill_card_type, fill_card_type_key, -1, NULL, sizeof(CARD_TYPE_REC)},
{GPS_PARAMS_FILE, fill_gps_params, unfill_gps_params, fill_gps_params_key, -1, NULL, sizeof(GPS_PARAMS)},
{SYSMENUS_FILE, fill_sysmenus, unfill_sysmenus, fill_sysmenus_key, -1, NULL, sizeof(SYSMENUS_REC)},
{SYSITEMS_FILE, fill_sysitems, unfill_sysitems, fill_sysitems_key, -1, NULL, sizeof(SYSITEMS_REC)},
{VEH_CMSG_FILE, fill_veh_cm, unfill_veh_cm, fill_veh_cm_key, -1, NULL, sizeof(VEH_CANMSG_REC)},
{TL_FILE, fill_tl, unfill_tl, fill_tl_key, -1, NULL, sizeof(TL_STRUCT)},
{SUBCALLH_FILE, fill_subch, unfill_subch, fill_subch_key, -1, NULL, sizeof(SUBCALL_HIST_REC)},
{ZONEPOLY_FILE, fill_zonepoly, unfill_zonepoly, fill_zonepoly_key, -1, NULL, sizeof(ZONE_POLY_REC)},
{WEBUSER_FILE, fill_webuser, unfill_webuser, fill_webuser_key, -1, NULL, sizeof(WEBUSER)},
{WEBADDR_FILE, fill_webaddr, unfill_webaddr, fill_webaddr_key, -1, NULL, sizeof(WEBADDR)},
{FTJ_BAD_FILE, fill_ftj_bad, unfill_ftj_bad, fill_ftj_bad_key, -1, NULL, sizeof(FTJ_BAD_STRUCT)},
{LINE_MGR_FILE, fill_line_mgr, unfill_line_mgr, fill_line_mgr_key, -1, NULL, sizeof(LINE_MGR_DB_REC)},
{GEOADDR_ADDR_FILE, fill_geoaddr_addr, unfill_geoaddr_addr, fill_geoaddr_addr_key, -1, NULL, sizeof(GEOADDR_ADDR_REC)},
{GEOADDR_LINE_FILE, fill_geoaddr_line, unfill_geoaddr_line, fill_geoaddr_line_key, -1, NULL, sizeof(GEOADDR_LINE_REC)},
{GEOADDR_POINT_FILE, fill_geoaddr_point, unfill_geoaddr_point, fill_geoaddr_point_key, -1, NULL, sizeof(GEOADDR_POINT_REC)},
{ZONETRANS_FILE, fill_zntrans, unfill_zntrans, fill_zntrans_key, -1, NULL, sizeof(ZONETRANS_DB_REC)},
{ATTRANS_FILE, fill_attrans, unfill_attrans, fill_attrans_key, -1, NULL, sizeof(ATTRANS_DB_REC)},
{ILINK_FILE, fill_ilink, unfill_ilink, fill_ilink_key, -1, NULL, sizeof(ILINK_DB_REC)},
{IMSI_FILE, fill_imsi, unfill_imsi, fill_imsi_key, -1, NULL, sizeof(IMSI_REC)},
{S_VERSION_FILE, fill_s_version, unfill_s_version, fill_s_version_key, -1, NULL, sizeof(S_VERSION_REC)},
{ZONEDOWN_FILE, fill_zonedown, unfill_zonedown, fill_zonedown_key, -1, NULL, sizeof(ZONEDOWN_REC)},
{KELANODE_FILE, fill_kelanode, unfill_kelanode, fill_kelanode_key, -1, NULL, sizeof(KELANODE_REC)},  
};
#endif

#else
#ifdef _MYSQL_
extern DB_MYSQL_FILES Db_mysql_files[];
#else
extern DB_FILES Db_files[];
#endif

#endif

extern RET_STATUS db_open(int, int);
extern RET_STATUS db_close(int);
extern void db_done();
extern RET_STATUS db_read(int, void *, int);
extern RET_STATUS db_setkey(int, struct keydesc *);
extern RET_STATUS db_read_key(int, void *, struct keydesc *, int);
extern RET_STATUS db_read_by_record_number(int, int, void *);
extern RET_STATUS db_write(int, void *);
extern RET_STATUS db_delete(int, void *);
extern RET_STATUS db_release(int);
extern RET_STATUS db_update(int, void *);

#endif 
