/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill.h,v $
*  @(#)  $Revision: 1.12 $
*  @(#)  $Date: 2004/08/23 10:20:45 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/fill.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _MYSQL_
void fill_acad(char *from, char *to, int inaction);
void unfill_acad(char *to, char *from);
void fill_acad_key(char *from, char *to);
void fill_ad(char *from, char *to, int inaction);	/* Access_Description */
void unfill_ad(char *to, char *from);
void fill_ad_key(char *from, char *to);
void fill_cl( char *, char *, int );
void unfill_cl(char *, char * );
void fill_cl_key(char *, char *);
void fill_ch(char *, char *, int );
void unfill_ch(char *from, char *to );
void fill_ch_key(char *, char *);
void fill_cmt(char *from, char *to, int inaction);	/* Comment */
void unfill_cmt(char *to, char *from);
void fill_cmt_key(char *from, char *to);
void fill_dr();
void unfill_dr();
void fill_dr_key();
void fill_vh();
void unfill_vh();
void fill_vh_key();
void fill_imsi();
void unfill_imsi();
void fill_imsi_key();
void fill_zn();
void unfill_zn();
void fill_zn_key();
void fill_fl();
void unfill_fl();
void fill_fl_key();
void fill_ad(char *from, char *to, int inaction);
void unfill_ad(char *to, char *from);
void fill_ad_key(char *from, char *to);
void fill_ac();
void unfill_ac();
void fill_ac_key();
void fill_at();
void unfill_at();
void fill_at_key();
void fill_em();
void unfill_em();
void fill_em_key();
void fill_el();
void unfill_el();
void fill_el_key();
void fill_et();
void unfill_et();
void fill_et_key();
void fill_eg();
void unfill_eg();
void fill_eg_key();
void fill_in();
void unfill_in();
void fill_in_key();
void fill_lo(char *from, char *to, int inaction);
void unfill_lo(char *to, char *from);
void fill_lo_key(char *from, char *to);
void fill_hp();
void unfill_hp();
void fill_hp_key();
void fill_hl(char *from, char *to, int inaction);	/* holiday */
void unfill_hl(char *to, char *from);
void fill_hl_key(char *from, char *to);
void fill_indv(char *from, char *to, int inaction);
void unfill_indv(char *to, char *from);
void fill_indv_key(char *from, char *to);
void fill_receipt(char *from, char *to, int inaction);
void unfill_receipt(char *from, char *to );
void fill_receipt_key(char *from, char *to );
void fill_card_type(char *from, char *to, int inaction);
void unfill_card_type(char *from, char *to );
void fill_card_type_key(char *from, char *to );
void fill_bad_credit(char *from, char *to, int inaction);
void unfill_bad_credit(char *from, char *to );
void fill_bad_credit_key(char *from, char *to );
void fill_gps_params_key(char *from, char *to );
void fill_gps_params(char *from, char *to, int inaction);
void unfill_gps_params(char *from, char *to );
void fill_cr_key(char *from, char *to);
void fill_cr(char *from, char *to, int inaction);
void unfill_cr(char *to, char *from);
void fill_pl();
void unfill_pl();
void fill_pl_key();
void fill_mf();
void unfill_mf();
void fill_mf_key();
void fill_mb();
void unfill_mb();
void fill_mb_key();
void fill_ml();
void unfill_ml();
void fill_ml_key();
void fill_rt();
void unfill_rt();
void fill_rt_key();
void fill_rm();
void unfill_rm();
void fill_rm_key();
void fill_sl();
void unfill_sl();
void fill_sl_key();
void fill_sn();
void unfill_sn();
void fill_sn_key();
void fill_st();
void unfill_st();
void fill_st_key();
void fill_sc();
void unfill_sc();
void fill_sc_key();
void fill_sz();
void unfill_sz();
void fill_sz_key();
void fill_syc();
void unfill_syc();
void fill_syc_key();
void fill_uz();
void unfill_uz();
void fill_uz_key();
void fill_up();
void unfill_up();
void fill_up_key();
void fill_cm();
void unfill_cm();
void fill_cm_key();
void fill_sd();
void unfill_sd();
void fill_sd_key();
void fill_su();
void unfill_su();
void fill_su_key();
void fill_sa();
void unfill_sa();
void fill_sa_key();
void fill_shift();
void fill_hh();
void fill_sh();
void unfill_sh();
void fill_sh_key(char *, char *);
void fill_tlg(char *from, char *to, int inaction);
void unfill_tlg(char *to, char *from);
void fill_tlg_key(char *from, char *to);
void fill_rp(char *from, char *to, int inaction);
void fill_rp_key(char *from, char *to);
void unfill_rp(char *to, char *from);
void fill_hc_key(char *, char *);
void fill_hc(char *, char *, int);
void unfill_hc(char *, char *);
void fill_cust_key(char *, char *);
void fill_cust(char *, char *, int);
void unfill_cust(char *, char *);
void fill_time_call(char *, char *,int );
void unfill_time_call(char *, char *);
void fill_time_call_key(char *, char *);
void fill_channel(char *, char *,int);
void unfill_channel(char *, char *);
void fill_channel_key(char *, char *);
void fill_cmsg(char *, char *, int);
void unfill_cmsg(char *, char *);
void fill_cmsg_key(char *, char *);
void fill_veh_cm_key( char *, char *);
void fill_veh_cm( char *, char *, int);
void unfill_veh_cm( char *, char *);
void fill_tl_key( char *, char *);
void fill_tl( char *, char *, int);
void unfill_tl( char *, char *);
void fill_subch(char *, char *, int );
void unfill_subch(char *from, char *to );
void fill_subch_key(char *, char *);
void fill_zonepoly(char *, char *, int );
void unfill_zonepoly(char *from, char *to );
void fill_zonepoly_key( char *, char * );
void fill_ftj_bad(char *, char *, int );
void unfill_ftj_bad(char *from, char *to );
void fill_ftj_bad_key( char *, char * );
void fill_line_mgr( char *, char *, int );
void unfill_line_mgr( char *from, char *to );
void fill_line_mgr_key( char *, char * );
void fill_geoaddr_addr( char *, char *, int );
void unfill_geoaddr_addr( char *from, char *to );
void fill_geoaddr_addr_key( char *, char * );
void fill_geoaddr_line( char *, char *, int );
void unfill_geoaddr_line( char *from, char *to );
void fill_geoaddr_line_key( char *, char * );
void fill_geoaddr_point( char *, char *, int );
void unfill_geoaddr_point( char *from, char *to );
void fill_geoaddr_point_key( char *, char * );
void fill_zntrans( char *, char *, int );
void unfill_zntrans( char *from, char *to );
void fill_zntrans_key( char *, char * );
void fill_attrans( char *, char *, int );
void unfill_attrans( char *from, char *to );
void fill_attrans_key( char *, char *);
void fill_ilink( char *, char *, int );
void unfill_ilink( char *from, char *to );
void fill_ilink_key( char *, char *);
void fill_webuser(char *, char *, int );
void unfill_webuser(char *from, char *to );
void fill_webuser_key( char *, char * );
void fill_webaddr(char *, char *, int );
void unfill_webaddr(char *from, char *to );
void fill_webaddr_key( char *, char * );
void fill_sysmenus(char *, char *, int);
void unfill_sysmenus(char *, char *);
void fill_sysmenus_key( char *, char *);
void fill_sysitems(char *, char *, int);
void unfill_sysitems(char *, char *);
void fill_sysitems_key( char *, char *);
void fill_ss(char *, char *, int);
void unfill_ss(char *, char *);
void fill_ss_key( char *, char *);
void fill_s_version(char *, char *, int);
void unfill_s_version(char *, char *);
void fill_s_version_key( char *, char *);
void fill_sh(char *, char *, int);
void unfill_sh(char *, char *);
void fill_sh_key( char *, char *);
void fill_zonedown(char *, char *, int);
void unfill_zonedown(char *, char *);
void fill_zonedown_key(char *, char *);
void fill_kelanode(char *, char *, int);
void unfill_kelanode(char *, char *);
void fill_kelanode_key(char *, char *);
#else
// START MYSQL DEFINITIONS
void fill_my_cl_key(char *, char *, struct keydesc *, int);
void fill_my_cl( char *, char *);
void unfill_my_cl( char *, MYSQL_ROW);
//void fill_my_acad(char *, char *);
//void unfill_my_acad(char *to, MYSQL_ROW);
//void fill_my_acad_key(char *, char *, struct keydesc *, int);
//void fill_my_ad(char *from, char *to, int inaction);	/* Access_Description */
//void unfill_my_ad(char *to, MYSQL_ROW);
//void fill_my_ad_key(char *, char *, struct keydesc *, int);
//void fill_my_ch(char *, char *, int );
//void unfill_my_ch(char *from, MYSQL_ROW);
//void fill_my_ch_key(char *, char *, struct keydesc *, int);
//void fill_my_cmt(char *from, char *to, int inaction);	/* Comment */
//void unfill_my_cmt(char *to, MYSQL_ROW);
//void fill_my_cmt_key(char *, char *, struct keydesc *, int);
void fill_my_dr();
void unfill_my_dr(char *to, MYSQL_ROW);
void fill_my_dr_key(char *, char *, struct keydesc *, int);
void fill_my_vh();
void unfill_my_vh(char *to, MYSQL_ROW);
void fill_my_vh_key(char *, char *, struct keydesc *, int);
//void fill_my_imsi();
//void unfill_my_imsi(char *to, MYSQL_ROW);
//void fill_my_imsi_key(char *, char *, struct keydesc *, int);
//void fill_my_zn();
//void unfill_my_zn(char *to, MYSQL_ROW);
//void fill_my_zn_key(char *, char *, struct keydesc *, int);
void fill_my_fl();
void unfill_my_fl(char *to, MYSQL_ROW);
void fill_my_fl_key(char *, char *, struct keydesc *, int);
//void fill_my_ad(char *from, char *to, int inaction);
//void unfill_my_ad(char *to, MYSQL_ROW);
//void fill_my_ad_key(char *, char *, struct keydesc *, int);
//void fill_my_ac();
//void unfill_my_ac(char *to, MYSQL_ROW);
//void fill_my_ac_key(char *, char *, struct keydesc *, int);
//void fill_my_at();
//void unfill_my_at(char *to, MYSQL_ROW);
//void fill_my_at_key(char *, char *, struct keydesc *, int);
//void fill_my_em();
//void unfill_my_em(char *to, MYSQL_ROW);
//void fill_my_em_key(char *, char *, struct keydesc *, int);
//void fill_my_el();
//void unfill_my_el(char *to, MYSQL_ROW);
//void fill_my_el_key(char *, char *, struct keydesc *, int);
//void fill_my_et();
//void unfill_my_et(char *to, MYSQL_ROW);
//void fill_my_et_key(char *, char *, struct keydesc *, int);
//void fill_my_eg();
//void unfill_my_eg(char *to, MYSQL_ROW);
//void fill_my_eg_key(char *, char *, struct keydesc *, int);
//void fill_my_in();
//void unfill_my_in(char *to, MYSQL_ROW);
//void fill_my_in_key(char *, char *, struct keydesc *, int);
//void fill_my_lo(char *from, char *to, int inaction);
//void unfill_my_lo(char *to, MYSQL_ROW);
//void fill_my_lo_key(char *, char *, struct keydesc *, int);
//void fill_my_hp();
//void unfill_my_hp(char *to, MYSQL_ROW);
//void fill_my_hp_key(char *, char *, struct keydesc *, int);
//void fill_my_hl(char *from, char *to, int inaction);	/* holiday */
//void unfill_my_hl(char *to, MYSQL_ROW);
//void fill_my_hl_key(char *, char *, struct keydesc *, int);
//void fill_my_indv(char *from, char *to, int inaction);
//void unfill_my_indv(char *to, MYSQL_ROW);
//void fill_my_indv_key(char *, char *, struct keydesc *, int);
//void fill_my_receipt(char *from, char *to, int inaction);
//void unfill_my_receipt(char *from, MYSQL_ROW );
//void fill_my_receipt_key(char *, char *, struct keydesc *, int);
//void fill_my_card_type(char *from, char *to, int inaction);
//void unfill_my_card_type(char *from, MYSQL_ROW );
//void fill_my_card_type_key(char *, char *, struct keydesc *, int);
//void fill_my_bad_credit(char *from, char *to, int inaction);
//void unfill_my_bad_credit(char *from, MYSQL_ROW );
//void fill_my_bad_credit_key(char *, char *, struct keydesc *, int);
//void fill_my_gps_params_key(char *, char *, struct keydesc *, int);
//void fill_my_gps_params(char *from, char *to, int inaction);
//void unfill_my_gps_params(char *from, MYSQL_ROW );
//void fill_my_cr_key(char *, char *, struct keydesc *, int);
//void fill_my_cr(char *from, char *to, int inaction);
//void unfill_my_cr(char *to, MYSQL_ROW);
//void fill_my_pl();
//void unfill_my_pl(char *to, MYSQL_ROW);
//void fill_my_pl_key(char *, char *, struct keydesc *, int);
//void fill_my_mf();
//void unfill_my_mf(char *to, MYSQL_ROW);
//void fill_my_mf_key(char *, char *, struct keydesc *, int);
//void fill_my_mb();
//void unfill_my_mb(char *to, MYSQL_ROW);
//void fill_my_mb_key(char *, char *, struct keydesc *, int);
//void fill_my_ml();
//void unfill_my_ml(char *to, MYSQL_ROW);
//void fill_my_ml_key(char *, char *, struct keydesc *, int);
//void fill_my_rt();
//void unfill_my_rt(char *to, MYSQL_ROW);
//void fill_my_rt_key(char *, char *, struct keydesc *, int);
//void fill_my_rm();
//void unfill_my_rm(char *to, MYSQL_ROW);
//void fill_my_rm_key(char *, char *, struct keydesc *, int);
//void fill_my_sl();
//void unfill_my_sl(char *to, MYSQL_ROW);
//void fill_my_sl_key(char *, char *, struct keydesc *, int);
//void fill_my_sn();
//void unfill_my_sn(char *to, MYSQL_ROW);
//void fill_my_sn_key(char *, char *, struct keydesc *, int);
//void fill_my_st();
//void unfill_my_st(char *to, MYSQL_ROW);
//void fill_my_st_key(char *, char *, struct keydesc *, int);
//void fill_my_sc();
//void unfill_my_sc(char *to, MYSQL_ROW);
//void fill_my_sc_key(char *, char *, struct keydesc *, int);
//void fill_my_sz();
//void unfill_my_sz(char *to, MYSQL_ROW);
//void fill_my_sz_key(char *, char *, struct keydesc *, int);
//void fill_my_syc();
//void unfill_my_syc(char *to, MYSQL_ROW);
//void fill_my_syc_key(char *, char *, struct keydesc *, int);
//void fill_my_uz();
//void unfill_my_uz(char *to, MYSQL_ROW);
//void fill_my_uz_key(char *, char *, struct keydesc *, int);
//void fill_my_up();
//void unfill_my_up(char *to, MYSQL_ROW);
//void fill_my_up_key(char *, char *, struct keydesc *, int);
//void fill_my_cm();
//void unfill_my_cm(char *to, MYSQL_ROW);
//void fill_my_cm_key(char *, char *, struct keydesc *, int);
//void fill_my_sd();
//void unfill_my_sd(char *to, MYSQL_ROW);
//void fill_my_sd_key(char *, char *, struct keydesc *, int);
//void fill_my_su();
//void unfill_my_su(char *to, MYSQL_ROW);
//void fill_my_su_key(char *, char *, struct keydesc *, int);
//void fill_my_sa();
//void unfill_my_sa(char *to, MYSQL_ROW);
//void fill_my_sa_key(char *, char *, struct keydesc *, int);
//void fill_my_shift();
//void fill_my_hh();
//
//void fill_my_sh();
//void unfill_my_sh(char *to, MYSQL_ROW);
//void fill_my_sh_key(char *, char *, struct keydesc *, int);
//
//void fill_my_tlg(char *from, char *to, int inaction);
//void unfill_my_tlg(char *to, MYSQL_ROW);
//void fill_my_tlg_key(char *, char *, struct keydesc *, int);
//
//void fill_my_rp(char *from, char *to, int inaction);
//void fill_my_rp_key(char *, char *, struct keydesc *, int);
//void unfill_my_rp(char *to, MYSQL_ROW);
//
//void fill_my_hc_key(char *, char *, struct keydesc *, int);
//void fill_my_hc(char *, char *, int);
//void unfill_my_hc(char *, MYSQL_ROW);
//
//void fill_my_cust_key(char *, char *, struct keydesc *, int);
//void fill_my_cust(char *, char *, int);
//void unfill_my_cust(char *, MYSQL_ROW);
//
//void fill_my_time_call(char *, char *,int );
//void unfill_my_time_call(char *, MYSQL_ROW);
//void fill_my_time_call_key(char *, char *, struct keydesc *, int);
//
//void fill_my_channel(char *, char *,int);
//void unfill_my_channel(char *, MYSQL_ROW);
//void fill_my_channel_key(char *, char *, struct keydesc *, int);
//
//void fill_my_cmsg(char *, char *, int);
//void unfill_my_cmsg(char *, MYSQL_ROW);
//void fill_my_cmsg_key(char *, char *, struct keydesc *, int);
//
//void fill_my_veh_cm_key(char *, char *, struct keydesc *, int);
//void fill_my_veh_cm( char *, char *, int);
//void unfill_my_veh_cm( char *, MYSQL_ROW);
//
//void fill_my_tl_key(char *, char *, struct keydesc *, int);
//void fill_my_tl( char *, char *, int);
//void unfill_my_tl( char *, MYSQL_ROW);
//
//void fill_my_subch(char *, char *, int );
//void unfill_my_subch(char *from, MYSQL_ROW );
//void fill_my_subch_key(char *, char *, struct keydesc *, int);
//
//void fill_my_zonepoly(char *, char *, int );
//void unfill_my_zonepoly(char *from, MYSQL_ROW );
//void fill_my_zonepoly_key(char *, char *, struct keydesc *, int);
//
//void fill_my_ftj_bad(char *, char *, int );
//void unfill_my_ftj_bad(char *from, MYSQL_ROW );
//void fill_my_ftj_bad_key(char *, char *, struct keydesc *, int);
//
//void fill_my_line_mgr( char *, char *, int );
//void unfill_my_line_mgr( char *from, MYSQL_ROW );
//void fill_my_line_mgr_key(char *, char *, struct keydesc *, int);
//
//void fill_my_geoaddr_addr( char *, char *, int );
//void unfill_my_geoaddr_addr( char *from, MYSQL_ROW );
//void fill_my_geoaddr_addr_key(char *, char *, struct keydesc *, int);
//
//void fill_my_geoaddr_line( char *, char *, int );
//void unfill_my_geoaddr_line( char *from, MYSQL_ROW );
//void fill_my_geoaddr_line_key(char *, char *, struct keydesc *, int);
//
//void fill_my_geoaddr_point( char *, char *, int );
//void unfill_my_geoaddr_point( char *from, MYSQL_ROW );
//void fill_my_geoaddr_point_key(char *, char *, struct keydesc *, int);
//
//void fill_my_zntrans( char *, char *, int );
//void unfill_my_zntrans( char *from, MYSQL_ROW );
//void fill_my_zntrans_key(char *, char *, struct keydesc *, int);
//
//void fill_my_attrans( char *, char *, int );
//void unfill_my_attrans( char *from, MYSQL_ROW );
//void fill_my_attrans_key(char *, char *, struct keydesc *, int);
//
//void fill_my_ilink( char *, char *, int );
//void unfill_my_ilink( char *from, MYSQL_ROW );
//void fill_my_ilink_key(char *, char *, struct keydesc *, int);
//
//
//void fill_my_webuser(char *, char *, int );
//void unfill_my_webuser(char *from, MYSQL_ROW );
//void fill_my_webuser_key(char *, char *, struct keydesc *, int);
//
//void fill_my_webaddr(char *, char *, int );
//void unfill_my_webaddr(char *from, MYSQL_ROW );
//void fill_my_webaddr_key(char *, char *, struct keydesc *, int);
//
//void fill_my_sysmenus(char *, char *, int);
//void unfill_my_sysmenus(char *, MYSQL_ROW);
//void fill_my_sysmenus_key(char *, char *, struct keydesc *, int);
//
//void fill_my_sysitems(char *, char *, int);
//void unfill_my_sysitems(char *, MYSQL_ROW);
//void fill_my_sysitems_key(char *, char *, struct keydesc *, int);
#endif
