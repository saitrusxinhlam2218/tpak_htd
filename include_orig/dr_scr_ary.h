 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  dr_scr_ary.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:45:25
 * @(#)  Last delta: 12/2/94 at 18:10:35
 * @(#)  SCCS File: /taxi/sccs/s.dr_scr_ary.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:dr_scr_ary.h	10.1.2.1"
#ident "@(#) head:dr_scr_ary.h	4.1.0.0"


/*  The following global array of structures is used by the "read_field()"
 *  routine to plact the cursor/prompt to the correct row and column of
 *  the field being read.  The index to the array is the field number.
 */

struct scr_flds dr_scr_flds[] = {
	{ 0, 0, 1,0},
	{ D_FLEET_ROW, D_FLEET_COL, MC_FLEET,1},
	{ D_FLEET_ROW, D_DR_ID_COL, MC_DR_ID,0},
	{ D_FLEET_ROW, D_CLASS_COL, MC_CLASS,0},
	{ D_FLEET_ROW, D_NAME_COL, MC_NAME,1},
	{ D_FLEET_ROW, D_ADD_COL, MC_ADDRESS,0},
	{ D_FLEET_ROW, D_CITY_COL, MC_CITY,1},
	{ D_FLEET_ROW, D_ZIP_COL, MC_ZIP,0},
	{ D_FLEET_ROW, D_ZSFX_COL, MC_ZIP_SFX,0},
	{ D_PHONE_ROW, D_PHONE_COL, MC_PHONE,1},
	{ D_PHONE_ROW, D_LIC_COL, MC_LIC,0},
	{ D_PHONE_ROW, D_TXI_LIC_COL, MC_TXI_LIC,0},
	{ D_PHONE_ROW, D_SEAL_1_COL, MC_SEAL_1,0},
	{ D_PHONE_ROW, D_SEAL_2_COL, MC_SEAL_2,0},
	{ D_PHONE_ROW, D_SEAL_3_COL, MC_SEAL_3,0},
	{ D_PHONE_ROW, D_SEAL_4_COL, MC_SEAL_4,0},
	{ D_PHONE_ROW, D_SEAL_5_COL, MC_SEAL_5,0},
	{ D_PHONE_ROW, D_SEAL_6_COL, MC_SEAL_6,0},
	{ D_PHONE_ROW, D_SEAL_7_COL, MC_SEAL_7,0},
	{ D_PHONE_ROW, D_SEAL_8_COL, MC_SEAL_8,0},
	{ D_PHONE_ROW, D_SEAL_9_COL, MC_SEAL_9,0}, 
	{ D_PHONE_ROW, D_SEAL_10_COL, MC_SEAL_10,0},
	{ D_PHONE_ROW, D_SEAL_11_COL, MC_SEAL_11,0},
	{ D_PHONE_ROW, D_SEAL_12_COL, MC_SEAL_12,0},
	{ D_PHONE_ROW, D_SEAL_13_COL, MC_SEAL_13,0},
	{ D_PHONE_ROW, D_SEAL_14_COL, MC_SEAL_14,0},
	{ D_PHONE_ROW, D_SEAL_15_COL, MC_SEAL_15,0},
	{ D_PHONE_ROW, D_SEAL_16_COL, MC_SEAL_16,0},
	{ D_ATTR_ROW, D_ATTR_1_COL, MC_ATTR_1,1},
#ifdef FOO
	{ D_ATTR_ROW, D_ATTR_2_COL, MC_ATTR_2,1},
	{ D_ATTR_ROW, D_ATTR_3_COL, MC_ATTR_3,1},
	{ D_ATTR_ROW, D_ATTR_4_COL, MC_ATTR_4,1},
	{ D_ATTR_ROW, D_ATTR_5_COL, MC_ATTR_5,0},
	{ D_ATTR_ROW, D_ATTR_6_COL, MC_ATTR_6,0},
	{ D_ATTR_ROW, D_ATTR_7_COL, MC_ATTR_7,0},
	{ D_ATTR_ROW, D_ATTR_8_COL, MC_ATTR_8,0},
#endif
	{ D_SUSP_ROW, D_SUSP_PASS_COL, MC_SP_PASS,0},
	{ D_SUSP_ROW, D_SP_FM_DATE_COL, MC_SUSP_FM_DATE,1},
	{ D_SUSP_ROW, D_SP_FM_TIME_COL, MC_SUSP_FM_TIME,1},
	{ D_SUSP_ROW, D_SP_TO_DATE_COL, MC_SUSP_TO_DATE,1},
	{ D_SUSP_ROW, D_SP_TO_TIME_COL, MC_SUSP_TO_TIME,1},
	{ D_SUSP_ROW, D_SUSP_RSN_COL, MC_SP_RSN,1},
	{ D_SUSP_ROW+1, D_SUSP_DEL_COL, MC_SP_DEL,0},
	{ D_SUSP_ROW+1, D_SP_FM_DATE_COL, MC_SUSP_FM_DATE,1},
	{ D_SUSP_ROW+1, D_SP_FM_TIME_COL, MC_SUSP_FM_TIME,1},
	{ D_SUSP_ROW+1, D_SP_TO_DATE_COL, MC_SUSP_TO_DATE,1},
	{ D_SUSP_ROW+1, D_SP_TO_TIME_COL, MC_SUSP_TO_TIME,1},
	{ D_SUSP_ROW+2, D_SUSP_TPX_COL, MC_SP_TPX,0},
	{ D_SUSP_ROW+2, D_SP_FM_DATE_COL, MC_SUSP_FM_DATE,1},
	{ D_SUSP_ROW+2, D_SP_FM_TIME_COL, MC_SUSP_FM_TIME,1},
	{ D_SUSP_ROW+2, D_SP_TO_DATE_COL, MC_SUSP_TO_DATE,1},
	{ D_SUSP_ROW+2, D_SP_TO_TIME_COL, MC_SUSP_TO_TIME,1},
	{ D_SUSP_MSG_1_ROW, D_SUSP_MSG_1_COL, MC_MSG_1,0},
	{ D_CMNT_ROW, D_CMNT_COL, MC_COMMENT,0},
	{ D_CMNT_ROW, D_DRV_L_RNW_COL, MC_LIC_RNW_DATE,0},
	{ D_CMNT_ROW, D_TXI_L_RNW_COL, MC_TXI_LIC_RNW_DATE,0},
	{ D_CMNT_ROW, D_ID_SERIAL_COL, MC_ID_SERIAL, 0},
	{ D_EMRG_ROW, D_EMRG_ON_DATE_COL, MC_EMRG_ON_DATE,1},
	{ D_EMRG_ROW, D_EMRG_ON_TIME_COL, MC_EMRG_ON_TIME,1},
	{ D_EMRG_ROW, D_EMRG_OFF_DATE_COL, MC_EMRG_OFF_DATE,0},
	{ D_EMRG_ROW, D_EMRG_OFF_TIME_COL, MC_EMRG_OFF_TIME,0},
	{ D_EMRG_ROW, D_SGN_ON_DATE_COL, MC_SGN_ON_DATE,0},
	{ D_EMRG_ROW, D_SGN_ON_TIME_COL, MC_SGN_ON_TIME,0},
	{ D_EMRG_ROW, D_SGN_OFF_DATE_COL, MC_SGN_OFF_DATE,0},
	{ D_EMRG_ROW, D_SGN_OFF_TIME_COL, MC_SGN_OFF_TIME,1},
	{ D_EMRG_ROW, D_LST_VEH_COL, MC_LST_VEH,0},
	{ D_EMRG_ROW, D_LST_CALL_COL, MC_LST_CALL,0},
	{ D_POSTS_ROW, D_POSTS_COL, MC_POSTS,0}, 
	{ D_POSTS_ROW, D_CND_POSTS_COL, MC_CND_POSTS,0},
	{ D_POSTS_ROW, D_ENR_POSTS_COL, MC_ENR_POSTS,0},
	{ D_POSTS_ROW, D_N_ACPTS_COL, MC_NO_ACCEPTS,0},
	{ D_POSTS_ROW, D_REJECTS_COL, MC_REJECTS,1},
	{ D_POSTS_ROW, D_CL_OUTS_COL, MC_CL_OUTS,1},
	{ D_POSTS_ROW, D_NO_SHWS_COL, MC_NO_SHOWS,0},
	{ D_POSTS_ROW, D_FLAGS_COL, MC_FLAGS,0},
	{ D_POSTS_ROW, D_BIDS_COL, MC_BIDS,0},
	{ D_POSTS_ROW, D_RQ_TLK_COL, MC_RQ_TO_TALK,0},
	{ D_POSTS_ROW, D_MSGS_COL, MC_MSGS,0},
	{ D_POSTS_ROW, D_BREAKS_COL, MC_BREAKS,1},
	{ D_POSTS_ROW, D_ASSGN_COL, MC_ASSIGNS,1},
	{ D_POSTS_ROW, D_CALLS_COL, MC_CALLS,1},
	{ D_POSTS_ROW, D_SH_MTRS_COL, MC_SH_METERS,0},
	{ D_POSTS_ROW, D_LT_MTRS_COL, MC_LT_METERS,0},
	{ D_POSTS_ROW, D_CL_BCKS_COL, MC_CL_BACKS,0}
};