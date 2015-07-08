 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fl_scr_ary.h; Rel: 6.7.0.0; Get date: 4/14/92 at 15:59:21
 * @(#)  Last delta: 5/23/91 at 10:04:54
 * @(#)  SCCS File: /taxi/sccs/s.fl_scr_ary.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:fl_scr_ary.h	6.7.0.0"


/*  The following global array of structures is used by the "read_field()"
 *  routine to plact the cursor/prompt to the correct row and column of
 *  the field being read.  The index to the array is the field number.
 *
 * NOTE: The inititalization of the help string has been moved to init_scr_flds in
 *       fl_maint.c, this was done for internationalization.
 */

extern int check_dummy(), check_fleet(), check_nbr(), check_1to3();
extern int check_y_n(), check_1to5(), check_1to4(), check_1or2();
extern int  check_1to999(), check_1to8(), check_0to8(), check_anything(), check_1to63();
extern int check_1to9(), check_6to63(), check_call_typ(), check_short();
extern int check_area_code(), check_0or1(), check_0to4(), check_missing_receipts(), check_time_call(), check_float();

struct scr_flds fl_scr_flds[] = {
	{ 0, 0, 1,0,	check_dummy, "%c",
	 ""},
	{ ROW_3, D_ID_COL, 1,0 ,	check_fleet, "%c", NULL},
	{ ROW_3, D_NBR_COL, 1,0 ,	check_nbr, "%d", NULL},
	{ ROW_3, D_ACT_COL, 1,0,	check_y_n,"%c", NULL},
	{ ROW_3, D_NAME_COL, 20,0 ,	check_anything,"%s", NULL},
	{ ROW_4, D_COMMENT_COL, 40,0,	check_anything,"%s", NULL},
	{ ROW_3, D_REJ_LVL_1_COL, 1,0,	check_1to4,"%c", NULL},
	{ ROW_3, D_REJ_PRM_COL, 1,0,	check_1to4,"%c", NULL},
	{ ROW_3, D_REJ_LVL_2_COL, 1,0,	check_1to4,"%c", NULL},
	{ ROW_3, D_REJ_LVL_3_COL, 1,0,	check_1to4,"%c", NULL},
	{ ROW_3, D_REJ_LVL_4_COL, 1,0,	check_1to4,"%c", NULL},
	{ ROW_3, D_REJ_LVL_5_COL, 1,0,	check_1to4,"%c", NULL},
	{ ROW_3, D_REJ_LVL_6_COL, 1,0,	check_1to4,"%c", NULL},
	{ ROW_3, D_REJ_LVL_7_COL, 1,0,	check_1to4,"%c", NULL},
	{ ROW_3, D_REJ_LVL_8_COL, 1,0,	check_1to4,"%c", NULL},
	{ ROW_4, D_EXC_LVL_1_COL, 2,0,	check_short,"%2d", NULL},
	{ ROW_4, D_EXC_PRM_COL, 2,0,	check_short,"%2d", NULL},
	{ ROW_4, D_EXC_LVL_2_COL, 2,0,	check_short,"%2d", NULL},
	{ ROW_4, D_EXC_LVL_3_COL, 2,0,	check_short,"%2d", NULL},
	{ ROW_4, D_EXC_LVL_4_COL, 2,0,	check_short,"%2d", NULL},
	{ ROW_4, D_EXC_LVL_5_COL, 2,0,	check_short,"%2d", NULL},
	{ ROW_4, D_EXC_LVL_6_COL, 2,0,	check_short,"%2d", NULL},
	{ ROW_4, D_EXC_LVL_7_COL, 2,0,	check_short,"%2d", NULL},
	{ ROW_4, D_EXC_LVL_8_COL, 2,0,	check_short,"%2d", NULL},
	{ ROW_7, D_POST_1_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_8, D_CPOST_COL+1, 5,0, check_short,"%5d", NULL},
	{ ROW_9, D_INQRY_2_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_10, D_SRCH_COL+1, 5,0, check_short,"%5d", NULL},
	{ ROW_11, D_INFO_COL+1, 5,0, check_short,"%5d", NULL},
	{ ROW_12, D_ZN_STAT_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_13, D_TM_CALL_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_7, D_N_SHOW_1_COL+1, 1,0,	check_1to5,"%c", NULL},
	{ ROW_8, D_CNCL_1_COL+1, 1,0,	check_1to5,"%c", NULL},
	{ ROW_9, D_REG_PRI_COL+1, 1,0,	check_1to5,"%c", NULL},
	{ ROW_10, D_LOCAL_PRI_COL+1, 1,0,	check_1to5,"%c", NULL},
	{ ROW_11, D_ROOKIE_COL+1, 2,0,	check_1to63,"%d", NULL},
	{ ROW_7, D_N_SHOW_2_COL+1, 1,0,	check_1to4,"%c", NULL},
	{ ROW_8, D_CNCL_2_COL+1, 1,0,	check_1to3,"%c", NULL},
	{ ROW_9, D_EPOST_ACT_COL+1, 1,0,	check_y_n,"%c", NULL},
	{ ROW_10, D_M_ACTION_RETRY_COL+1, 1,0,	check_0to8,"%d", NULL},
	{ ROW_13, D_C_OFFER_COL+1, 1,0,	check_1to3,"%c", NULL},
	{ ROW_7,D_ACPT_COL+1,5,0,	check_short,"%5d", NULL},
	{ ROW_8, D_BREAK_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_9, D_SH_METER_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_10, D_M_ACTION_TIMER_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_11, D_LOCAL_DUR_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_12, D_C_MATCH_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_13, D_CANX_TO_MOFF_COL+1, 5, 0, check_short, "%5d", NULL},
	{ ROW_14, D_NS_SHORT_COL+1, 3, 0, check_1to999, "%3d", NULL},
        { ROW_15, D_TC_INT_COL+1, 5, 0, check_short, "%5d", NULL},
	{ ROW_7, D_N_ACPT_TP_COL+1, 1,0,	check_1to3,"%c", NULL},
	{ ROW_8, D_N_ACPT_BT_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_9, D_NAK_OFFER_COL+1, 1,0,        check_1to3,"%c",NULL},
	{ ROW_11, D_EPOST_LT_COL+4, 5,0,	check_short,"%5d", NULL},
	{ ROW_12, D_TIME_C_COL+4, 5,0,	check_short,"%5d", NULL},
	{ ROW_13, D_LATE_M_COL+4, 5,0,	check_short,"%5d", NULL},
	{ ROW_14, D_LATE_M_COL+4, 5, 0, check_short,"%5d", NULL},
	{ ROW_7, D_C_ASSIGN_COL+4, 1, 0, check_0to4, "%c", NULL},
	{ ROW_8, D_C_ASSIGN_COL + 4, 1, 0, check_0to4, "%c", NULL},
#ifdef NAMERICA
	{ ROW_9, D_C_ASSIGN_COL + 4, 1, 0, check_y_n, "%c", NULL},
#endif
	{ ROW_11, D_EPOST_AL_COL+4, 1,0,	check_1to3,"%c", NULL},
	{ ROW_12, D_PRSNLS_COL+4, 1,0,	check_y_n,"%c", NULL},
	{ ROW_7, D_REJ_CNT_COL+1, 3,0,	check_1to999,"%3d", NULL},
	{ ROW_8, D_D_ATTR_COL+1, 1,0,	check_0to8,"%d", NULL},
	{ ROW_9, D_VEH_ATTR_COL+1, 1,0,	check_0to8,"%d", NULL},
	{ ROW_10, D_VEH_CALL_COL+1, 3,0,	check_1to9,"%3d", NULL},
	{ ROW_11, D_FUT_CALL_COL+1, 2,0, check_time_call,"%2d", NULL}, /* Note that fleet.future_call_max is a float,
									  but displayed as an int. */
	{ ROW_12, D_N_KDT_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_13, D_TM_C_PRC_COL+1, 5,0,	check_short,"%5d", NULL},
        { ROW_14, D_TIME_CLS_INT_REQ_COL+1, 2,0,check_short,"%2d", NULL},
	{ ROW_15, D_TIME_CLS_LEAD_SUPP_COL+1, 2, 0, check_short, "%2d", NULL},
	{ ROW_17, D_CK_SEALS_COL+1, 1,0,	check_y_n,"%c", NULL},
	{ ROW_18, D_H_LINE_COL+1, 1,0,	check_y_n,"%c", NULL},
	{ ROW_19, D_GRT_LINE_COL+1, 1,0,	check_y_n,"%c", NULL},
#ifdef NAMERICA
	{ ROW_20, D_UNASSGND_LK_AHD+1, 3, 0,    check_short, "%3d", NULL},
	{ ROW_21, D_SEND_DEST_OFFER_COL+1, 1,0, check_y_n,"%c", NULL },
	{ ROW_22, D_SEND_PCKUP_CMT_COL+1, 1, 0, check_y_n,"%c", NULL },
#else
	{ ROW_20, D_GPS_COL+1, 1, 0, check_y_n, "%c", NULL },
	{ ROW_21, D_GPS_COL-1, 3, 0, check_short, "%3d", NULL },

#endif
	{ ROW_17, D_ST_FLG_P_COL+1, 1,0,	check_1to3,"%c", NULL},
	{ ROW_18, D_Z_BK_LVL_COL+1, 1,0,	check_1to8,"%c", NULL},
	{ ROW_19, D_D_MSG_AP_COL+1, 1,0,	check_1or2,"%c", NULL},
	{ ROW_20, D_S_CL_RVW_COL+1, 1,0,	check_1or2,"%c", NULL},
	{ ROW_21, D_FLG_TIME_COL+1, 3,0,	check_short,"%3d", NULL},
#ifdef NAMERICA
	{ ROW_22, D_SEND_RATE_COL+1, 1, 0,      check_y_n,"%c", NULL},
#endif
	{ ROW_17, D_CHARGE_COL+1, 2,0,	check_6to63,"%2d", NULL},
	{ ROW_18, D_S_CALLS_COL+1, 2,0,	check_6to63,"%2d", NULL},
	{ ROW_19, D_T_CALLS_COL+1, 2,0,	check_6to63,"%2d", NULL},
#ifdef NAMERICA
	{ ROW_21, D_GPS_COL+1, 1, 0, check_y_n, "%c", NULL },
	{ ROW_22, D_GPS_COL+1, 3, 0, check_short, "%3d", NULL },
#endif
	{ ROW_17, D_PASS_COL+1, 2,0,	check_6to63,"%2d", NULL},
	{ ROW_18, D_DEL_COL+1, 2,0,	check_6to63,"%2d", NULL},
	{ ROW_19, D_X_TYPE_COL+1, 2,0,	check_6to63,"%2d", NULL},
	{ ROW_20, D_W_KUP_COL+1, 2,0,	check_6to63,"%2d", NULL},
	{ ROW_17, D_MAN_MET_1_COL+1, 1,0,	check_y_n,"%c", NULL},
	{ ROW_18, D_MAN_MET_2_COL+1, 1,0,	check_y_n,"%c", NULL},
	{ ROW_19, D_MAN_MET_3_COL+1, 1,0,	check_y_n,"%c", NULL},
	{ ROW_17, D_R_CALC_1_COL+1, 1,0,	check_1to4,"%c", NULL},
	{ ROW_18, D_R_CALC_2_COL+1, 1,0,	check_1to4,"%c", NULL},
	{ ROW_3, D_AU_TR_PT_COL+1, 20,0,	check_anything,"%s", NULL},
	{ ROW_3, D_T_CL_RPT_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_4, D_MSG_RPT_COL+1, 5,0,	check_short,"%5d", NULL},
        { ROW_3, D_ILINK_ACTIVE_COL, 1, 0, check_y_n, "%c", NULL},
        { ROW_4, D_ILINK_TIMEOUT_COL, 3, 0, check_short, "%3d", NULL},
	{ ROW_10, D_CITY_COL+1, 3,0,	check_anything,	"%3s", NULL},
	{ ROW_11, D_A_CODE_COL+1, 3,0,	check_area_code,"%s", NULL},
	{ ROW_12, D_C_TYPE_COL+1, 1,0,	check_call_typ,"%c", NULL},
	{ ROW_13, D_H_SRCH_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_14, D_MATCH_ALG_COL+1, 1,0,	check_1to5,"%c", NULL},
	{ ROW_15, D_GPS_MAXMIN_COL+1, 10,0,	check_float,"%10.2f", NULL},
	{ ROW_16, D_GPS_MAXMIN_COL+1, 10,0,	check_float,"%10.2f", NULL},
	{ ROW_17, D_GPS_MAXMIN_COL+1, 10, 0,    check_float,"%10.2f", NULL},
	{ ROW_18, D_GPS_MAXMIN_COL+1, 10, 0,    check_float,"%10.2f", NULL},
	{ ROW_19, D_GPS_MAXMIN_COL+1, 1, 0, check_1or2,"%1d", NULL},
	{ ROW_20, D_CITY_COL+1, 5, 0, check_short, "%5d", NULL},
	{ ROW_21, D_C_TYPE_COL+1, 1, 0, check_y_n, "%c", NULL},
	{ ROW_10, D_1ST_EST_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_10, D_1ST_TXT_COL+1, 7,0,	check_anything,"%s", NULL},
	{ ROW_11, D_2ND_EST_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_11, D_2ND_TXT_COL+1, 7,0,	check_anything,"%s", NULL},
	{ ROW_12, D_3RD_EST_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_12, D_3RD_TXT_COL+1, 7,0,	check_anything,"%s", NULL},
	{ ROW_13, D_4TH_EST_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_13, D_4TH_TXT_COL+1, 7,0,	check_anything,"%s", NULL},
	{ ROW_14, D_5TH_EST_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_14, D_5TH_TXT_COL+1, 7,0,	check_anything,"%s", NULL},
	{ ROW_15, D_6TH_EST_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_15, D_6TH_TXT_COL+1, 7,0,	check_anything,"%s", NULL},
	{ ROW_16, D_7TH_EST_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_16, D_7TH_TXT_COL+1, 7,0,	check_anything,"%s", NULL},
	{ ROW_17, D_8TH_EST_COL+1, 5,0,	check_short,"%5d", NULL},
	{ ROW_17, D_8TH_TXT_COL+1, 7,0,	check_anything,"%s", NULL},
	{ ROW_18, D_DEFAULT_COL+1, 7,0,	check_anything,"%s", NULL},
	{D_INACTIVE_RETRY_INTERVAL_ROW,	D_INACTIVE_RETRY_INTERVAL_COL,
	 CHARS_INACTIVE_RETRY_INTERVAL, 0, check_short, "%3d", NULL},
	{D_INACTIVE_TIME_OUT_PERIOD_ROW, D_INACTIVE_TIME_OUT_PERIOD_COL,
	 CHARS_INACTIVE_TIME_OUT_PERIOD, 0, check_short, "%3d", NULL},
	{D_APPROACH_TARIFF_ROW, D_APPROACH_TARIFF_COL,
	 CHARS_APPROACH_TARIFF, 0, check_1or2, "%c", NULL},
	{D_NUM_LEVELS_TO_COMPUTE_STATS_ROW, D_NUM_LEVELS_TO_COMPUTE_STATS_COL,
	 CHARS_NUM_LEVELS_TO_COMPUTE_STATS, 0, check_1to8, "%d", NULL},
	{D_STATS_COMPUTE_FREQ_ROW, D_STATS_COMPUTE_FREQ_COL,
	 CHARS_STATS_COMPUTE_FREQ, 0, check_short, "%3d", NULL},
	{D_HANDLE_SYSTEM_ASSIGNED_CALLS_ROW, D_HANDLE_SYSTEM_ASSIGNED_CALLS_COL,
	 CHARS_HANDLE_SYSTEM_ASSIGNED_CALLS, 0, check_1or2, "%d", NULL},
#ifdef SCANDINAVIA
	{D_FLAT_RATE_FILTER_ROW, D_FLAT_RATE_FILTER_COL,
	 CHARS_FLAT_RATE_FILTER, 0, check_y_n, "%c", NULL },
#endif
	{D_SEND_CALL_TYPE_ROW, D_SEND_CALL_TYPE_COL, CHARS_SEND_CALL_TYPE, 0, check_y_n,
	   "%c", NULL},
	{D_AUTO_REJECT_TIME_ROW, D_AUTO_REJECT_TIME_COL, CHARS_AUTO_REJECT_TIME, 0, check_short,
         "%3d", NULL},
        { D_AUTO_REJECT_TIME_ROW + 1, D_EMERG_RANGE_COL+1, 5, 0, check_short, "%5d", NULL },
        { D_AUTO_REJECT_TIME_ROW + 2, D_ENROUTE_BREAK_COL, 1, 0, check_y_n, "%c", NULL },
	{ D_AUTO_REJECT_TIME_ROW + 3, D_LOAD_UNLOAD_COL, 1, 0, check_y_n, "%c", NULL },
};	
