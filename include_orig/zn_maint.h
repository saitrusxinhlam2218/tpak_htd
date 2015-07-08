 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  zn_maint.h; Rel: 6.3.0.0; Get date: 4/8/92 at 17:01:42
 * @(#)  Last delta: 1/16/92 at 16:41:07
 * @(#)  SCCS File: /taxi/sccs/s.zn_maint.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:zn_maint.h	6.3.0.0"
#ident "@(#) head:zn_maint.h	4.1.0.0"


/*  This file defines ths cursor positions for the labels and data fields
 * as well as the maximum characters allowed in a particular field and the
 * field number on the Zone file maintenance screen.
 */

/* Define row and columns for misc fields for the zone file
 * maintenance screen.
 */
#define		HDR_ROW_1	0
#define		HDR_COL_1	0
#define		HDR_ROW_2	1
#define		HDR_COL_2	56	
#define		REQ_ROW		0
#define		REQ_COL		54
#define		ERROR_ROW	23
#define		ERROR_COL	0
#define		ERROR_COL1	70
#define		DEL_ROW		0
#define		DEL_COL		0
/* Define row and columns for the labels for the zone file maintenance screen */

#define         GPS_ADD_OFFSET  18

#define		L_FLEET_ROW	3
#define		L_FLEET_COL	1
#define		L_ZONE_COL	8
#define		L_DESC_COL	17
#define         L_STAND_TELE_COL 17
#define		L_TYPE_COL	40
#define         L_HTTP_TIMER_COL 43
#define         L_ROTI_COL       56
#define         L_STAND_NBR_COL  62
#define         L_AUTO_INQRY_COL 17
#define		L_SPR_ZONE_COL	47
#define		L_SPZ_DETL_COL	57
#define		L_SPZ_NBR_COL	65
#define		L_BID_ALW_COL	79
#define		L_MAX_TIME_CALLS_COL	95
#define         L_TC_REST_INT_COL   105
#define		L_MX_DSP_TM_ROW 6
#define		L_MX_DSP_TM_COL 1
#define		L_PKUP_L_TM_COL 21
#define         L_X_PKUP_L_TM_COL 28
#define		L_ST_TM_FRM_COL	52
#define		L_ST_TM_TO_COL	61
#define		L_EP_ALWD_COL 	73
#define		L_MDT_SCREEN_COL 81
#define		L_MDT_SCREEN_ROW 3
#define		L_EBOOK_LEAD_TM_COL 90
#define		L_EBOOK_LEAD_TM_ROW 6
#define         L_TC_LOOK_AHEAD_COL  100
#define		L_PKUP_SLS_ROW	9
#define		L_PKUP_SLS_COL	1
#define		L_P_SL_1_COL	14
#define		L_P_SL_2_COL	18
#define		L_P_SL_3_COL	22
#define		L_P_SL_4_COL 	26
#define		L_P_SL_5_COL	30
#define		L_P_SL_6_COL	34
#define		L_P_SL_7_COL	38
#define		L_P_SL_8_COL	42
#define		L_P_SL_9_COL	46
#define		L_P_SL_10_COL	50
#define		L_P_SL_11_COL	54
#define		L_P_SL_12_COL	58
#define		L_P_SL_13_COL	62
#define		L_P_SL_14_COL	66
#define		L_P_SL_15_COL	70
#define		L_P_SL_16_COL	74
#define		L_BREAK_DURATION_COL	81
#define		L_BREAK_DURATION_ROW	9
#define		L_ACPT_DURATION_COL	92
#define		L_ACPT_DURATION_ROW	9
#define         L_CHECK_TAXIS_ROW       9
#define         L_CHECK_TAXIS_COL       100
#define         L_NS_SHORT_COL          113
#define         L_CBOOK_TIME_COL        115
#define         L_GPS_LAT_ROW           12
#define         L_GPS_LAT_COL           2
#define         L_GPS_LONG_ROW          14
#define         L_GPS_LONG_COL          2
#define         L_GPS_RADIUS_ROW        16
#define         L_GPS_RADIUS_COL        2
#define         L_GPS_DIST_CEILING_ROW  18
#define         L_GPS_DIST_CEILING_COL  2
#define         L_GPS_EMERG_RANGE_ROW   20
#define         L_GPS_EMERG_RANGE_COL   2

#define		L_LEVEL_ROW	12
#define		L_LEVEL_COL	19
#define		L_ALG_COL	29
#define		L_EXC_TM_COL	35
#define		L_REJ_PEN_COL	47
#define		L_ZONE_1_COL	61
#define		L_ZONE_2_COL	68
#define		L_ZONE_3_COL	75
#define		L_ZONE_4_COL	82
#define		L_ZONE_5_COL	89
#define		L_ZONE_6_COL	96
#define		L_ZONE_7_COL	103
#define		L_ZONE_8_COL	110
#define		L_LVL_1_ROW	13
#define		L_LVL_1_COL	21
#define         L_MATCH_ALGORITHM_ROW   14
#define         L_MATCH_ALGORITHM_COL   29
#define		L_PRM_ROW	14
#define		L_PRM_COL	19
#define		L_LVL_2_ROW	15
#define		L_LVL_2_COL	21
#define		L_LVL_3_ROW 	16
#define		L_LVL_3_COL	21
#define		L_LVL_4_ROW	17
#define		L_LVL_4_COL	21
#define		L_LVL_5_ROW	18
#define		L_LVL_5_COL	21
#define		L_LVL_6_ROW	19
#define		L_LVL_6_COL	21
#define		L_LVL_7_ROW	20
#define		L_LVL_7_COL	21
#define		L_LVL_8_ROW	21
#define		L_LVL_8_COL	21

/*  Define row and columns for the data fields of the zone
 *  file maintenance screen.
 */


#define		D_FLEET_ROW	4
#define		D_FLEET_COL	2
#define		D_ZONE_COL	9
#define		D_DESC_COL	18
#define         D_STAND_TELE_COL 18
#define		D_TYPE_COL	41
#define         D_HTTP_TIMER_COL 44
#define         D_ROTI_COL       57
#define         D_STAND_NBR_COL  63
#define         D_AUTO_INQRY_COL 18
#define		D_SPZ_DETL_COL	58
#define		D_SPZ_NBR_COL	66
#define		D_BID_ALW_COL	80
#define		D_MAX_TIME_CALLS_COL	96
#define         D_TC_REST_INT_COL 106
#define         D_CBOOK_TIME_COL 114
#define		D_MX_DSP_TM_ROW 7
#define		D_MX_DSP_TM_COL 2
#define		D_PKUP_L_TM_COL 22
#define         D_X_PKUP_L_TM_COL 29
#define		D_ST_TM_FRM_COL	53
#define		D_ST_TM_TO_COL	62
#define		D_EP_ALWD_COL 	74
#define		D_MDT_SCREEN_COL 82
#define		D_MDT_SCREEN_ROW 4
#define		D_EBOOK_LEAD_TM_COL 96
#define		D_EBOOK_LEAD_TM_ROW 7
#define         D_TC_LOOK_AHEAD_COL 106
#define		D_BREAK_DURATION_ROW 10
#define		D_BREAK_DURATION_COL 82
#define		D_ACPT_DURATION_ROW 10
#define		D_ACPT_DURATION_COL 93
#define         D_CHECK_TAXIS_ROW 10
#define         D_CHECK_TAXIS_COL 103
#define         D_NS_SHORT_COL    114
#define         D_GPS_LAT_ROW    13
#define         D_GPS_LAT_COL    3
#define         D_GPS_LONG_ROW   15
#define         D_GPS_LONG_COL   3
#define         D_GPS_RADIUS_ROW 17
#define         D_GPS_RADIUS_COL 3
#define         D_GPS_DIST_CEILING_ROW 19
#define         D_GPS_DIST_CEILING_COL 3
#define         D_GPS_EMERG_RANGE_ROW 21
#define         D_GPS_EMERG_RANGE_COL 3

#define		D_PKUP_SLS_ROW	10
#define		D_P_SL_1_COL	15
#define		D_P_SL_2_COL	19
#define		D_P_SL_3_COL	23
#define		D_P_SL_4_COL 	27
#define		D_P_SL_5_COL	31
#define		D_P_SL_6_COL	35
#define		D_P_SL_7_COL	39
#define		D_P_SL_8_COL	43
#define		D_P_SL_9_COL	47
#define		D_P_SL_10_COL	51
#define		D_P_SL_11_COL	55
#define		D_P_SL_12_COL	59
#define		D_P_SL_13_COL	63
#define		D_P_SL_14_COL	67
#define		D_P_SL_15_COL	71
#define		D_P_SL_16_COL	75


#define		D_LEVEL_ROW	13
#define		D_L1_ALG_COL	30
#define		D_L1_ETM_COL	36
#define		D_L1_RPN_COL	48
#define		D_L1_Z1_COL	62
#define		D_L1_Z2_COL	69
#define		D_L1_Z3_COL	76
#define		D_L1_Z4_COL	83
#define		D_L1_Z5_COL	90
#define		D_L1_Z6_COL	97
#define		D_L1_Z7_COL	104
#define		D_L1_Z8_COL	111
#define         D_MATCH_ALGORITHM_ROW 14
#define         D_MATCH_ALGORITHM_COL 30
#define		D_PRM_ETM_ROW	14
#define		D_PRM_ETM_COL	36
#define		D_PRM_RPN_COL	48
#define		D_LVL_2_ROW	15
#define		D_L2_ALG_COL	30
#define		D_L2_ETM_COL	36
#define		D_L2_RPN_COL	48
#define		D_L2_Z1_COL	62
#define		D_L2_Z2_COL	69
#define		D_L2_Z3_COL	76
#define		D_L2_Z4_COL	83
#define		D_L2_Z5_COL	90
#define		D_L2_Z6_COL	97
#define		D_L2_Z7_COL	104
#define		D_L2_Z8_COL	111
#define		D_LVL_3_ROW	16
#define		D_L3_ALG_COL	30
#define		D_L3_ETM_COL	36
#define		D_L3_RPN_COL	48
#define		D_L3_Z1_COL	62
#define		D_L3_Z2_COL	69
#define		D_L3_Z3_COL	76
#define		D_L3_Z4_COL	83
#define		D_L3_Z5_COL	90
#define		D_L3_Z6_COL	97
#define		D_L3_Z7_COL	104
#define		D_L3_Z8_COL	111
#define		D_LVL_4_ROW	17
#define		D_L4_ALG_COL	30
#define		D_L4_ETM_COL	36
#define		D_L4_RPN_COL	48
#define		D_L4_Z1_COL	62
#define		D_L4_Z2_COL	69
#define		D_L4_Z3_COL	76
#define		D_L4_Z4_COL	83
#define		D_L4_Z5_COL	90
#define		D_L4_Z6_COL	97
#define		D_L4_Z7_COL	104
#define		D_L4_Z8_COL	111
#define		D_LVL_5_ROW	18
#define		D_L5_ALG_COL	30
#define		D_L5_ETM_COL	36
#define		D_L5_RPN_COL	48
#define		D_L5_Z1_COL	62
#define		D_L5_Z2_COL	69
#define		D_L5_Z3_COL	76
#define		D_L5_Z4_COL	83
#define		D_L5_Z5_COL	90
#define		D_L5_Z6_COL	97
#define		D_L5_Z7_COL	104
#define		D_L5_Z8_COL	111
#define		D_LVL_6_ROW	19
#define		D_L6_ALG_COL	30
#define		D_L6_ETM_COL	36
#define		D_L6_RPN_COL	48
#define		D_L6_Z1_COL	62
#define		D_L6_Z2_COL	69
#define		D_L6_Z3_COL	76
#define		D_L6_Z4_COL	83
#define		D_L6_Z5_COL	90
#define		D_L6_Z6_COL	97
#define		D_L6_Z7_COL	104
#define		D_L6_Z8_COL	111
#define		D_LVL_7_ROW	20
#define		D_L7_ALG_COL	30
#define		D_L7_ETM_COL	36
#define		D_L7_RPN_COL	48
#define		D_L7_Z1_COL	62
#define		D_L7_Z2_COL	69
#define		D_L7_Z3_COL	76
#define		D_L7_Z4_COL	83
#define		D_L7_Z5_COL	90
#define		D_L7_Z6_COL	97
#define		D_L7_Z7_COL	104
#define		D_L7_Z8_COL	111
#define		D_LVL_8_ROW	21
#define		D_L8_ALG_COL	30
#define		D_L8_ETM_COL	36
#define		D_L8_RPN_COL	48
#define		D_L8_Z1_COL	62
#define		D_L8_Z2_COL	69
#define		D_L8_Z3_COL	76
#define		D_L8_Z4_COL	83
#define		D_L8_Z5_COL	90
#define		D_L8_Z6_COL	97
#define		D_L8_Z7_COL	104
#define		D_L8_Z8_COL	111

/*  Define maximum characters that are allowed for each
 *  field on the zone file maintenance screen.
 */

#define		MC_FLEET	1
#define		MC_ZONE		5
#define		MC_DESC		20
#define         MC_STAND_TELE   18
#define		MC_TYPE		1
#define         MC_HTTP_TIMER   3
#define         MC_ROTI         3
#define         MC_STAND_NBR    3
#define         MC_AUTO_INQRY   3
#define		MC_SPZ_DETL	1
#define		MC_SPZ_NBR	5
#define		MC_BID_ALW	1
#define		MC_MAX_TIME_CALLS	3
#define         MC_TC_REST_INT 3
#define		MC_MX_DSP_TM 	5
#define		MC_PKUP_L_TM 	5
#define		MC_ST_TM_FRM	5
#define		MC_ST_TM_TO	5
#define		MC_EP_ALWD 	1
#define		MC_MDT_SCREEN	1
#define		MC_EBOOK_LEAD_TM	5
#define         MC_CBOOK_TIME   5
#define         MC_TC_LOOK_AHEAD  3
#define		MC_BREAK_DURATION	5
#define		MC_ACPT_DURATION	5
#define         MC_CHECK_TAXIS  1
#define         MC_NS_SHORT     3
#define         MC_GPS_LAT      10
#define         MC_GPS_LONG     11
#define         MC_GPS_RADIUS   6
#define         MC_GPS_DIST_CEILING 6
#define         MC_GPS_EMERG_RANGE 6
#define         MC_MATCH_ALGORITHM 1
#define		MC_P_SL_1	1
#define		MC_P_SL_2	1
#define		MC_P_SL_3	1
#define		MC_P_SL_4 	1
#define		MC_P_SL_5	1
#define		MC_P_SL_6	1
#define		MC_P_SL_7	1
#define		MC_P_SL_8	1
#define		MC_P_SL_9	1
#define		MC_P_SL_10	1
#define		MC_P_SL_11	1
#define		MC_P_SL_12	1
#define		MC_P_SL_13	1
#define		MC_P_SL_14	1
#define		MC_P_SL_15	1
#define		MC_P_SL_16	1
#define		MC_L1_ALG	1
#define		MC_L1_ETM	5
#define		MC_L1_RPN	1
#define		MC_L1_Z1	3
#define		MC_L1_Z2	3
#define		MC_L1_Z3	3
#define		MC_L1_Z4	3
#define		MC_L1_Z5	3
#define		MC_L1_Z6	3
#define		MC_L1_Z7	3
#define		MC_L1_Z8	3
#define		MC_PRM_ETM	5
#define		MC_PRM_RPN	1
#define		MC_L2_ALG	1
#define		MC_L2_ETM	5
#define		MC_L2_RPN	1
#define		MC_L2_Z1	3
#define		MC_L2_Z2	3
#define		MC_L2_Z3	3
#define		MC_L2_Z4	3
#define		MC_L2_Z5	3
#define		MC_L2_Z6	3
#define		MC_L2_Z7	3
#define		MC_L2_Z8	3
#define		MC_L3_ALG	1
#define		MC_L3_ETM	5
#define		MC_L3_RPN	1
#define		MC_L3_Z1	3
#define		MC_L3_Z2	3
#define		MC_L3_Z3	3
#define		MC_L3_Z4	3
#define		MC_L3_Z5	3
#define		MC_L3_Z6	3
#define		MC_L3_Z7	3
#define		MC_L3_Z8	3
#define		MC_L4_ALG	1
#define		MC_L4_ETM	5
#define		MC_L4_RPN	1
#define		MC_L4_Z1	3
#define		MC_L4_Z2	3
#define		MC_L4_Z3	3
#define		MC_L4_Z4	3
#define		MC_L4_Z5	3
#define		MC_L4_Z6	3
#define		MC_L4_Z7	3
#define		MC_L4_Z8	3
#define		MC_L5_ALG	1
#define		MC_L5_ETM	5
#define		MC_L5_RPN	1
#define		MC_L5_Z1	3
#define		MC_L5_Z2	3
#define		MC_L5_Z3	3
#define		MC_L5_Z4	3
#define		MC_L5_Z5	3
#define		MC_L5_Z6	3
#define		MC_L5_Z7	3
#define		MC_L5_Z8	3
#define		MC_L6_ALG	1
#define		MC_L6_ETM	5
#define		MC_L6_RPN	1
#define		MC_L6_Z1	3
#define		MC_L6_Z2	3
#define		MC_L6_Z3	3
#define		MC_L6_Z4	3
#define		MC_L6_Z5	3
#define		MC_L6_Z6	3
#define		MC_L6_Z7	3
#define		MC_L6_Z8	3
#define		MC_L7_ALG	1
#define		MC_L7_ETM	5
#define		MC_L7_RPN	1
#define		MC_L7_Z1	3
#define		MC_L7_Z2	3
#define		MC_L7_Z3	3
#define		MC_L7_Z4	3
#define		MC_L7_Z5	3
#define		MC_L7_Z6	3
#define		MC_L7_Z7	3
#define		MC_L7_Z8	3
#define		MC_L8_ALG	1
#define		MC_L8_ETM	5
#define		MC_L8_RPN	1
#define		MC_L8_Z1	3
#define		MC_L8_Z2	3
#define		MC_L8_Z3	3
#define		MC_L8_Z4	3
#define		MC_L8_Z5	3
#define		MC_L8_Z6	3
#define		MC_L8_Z7	3
#define		MC_L8_Z8	3

/*  Define the field number of each field on the zone file 
 *  maintenance screen.
 */

#define         F_SCR_1_MIN             1
#define		F_FLEET			F_SCR_1_MIN
#define		F_ZONE			(F_FLEET + 1)
#define		F_DESC			(F_ZONE + 1)
#define		F_TYPE			(F_DESC + 1)
#define		F_SPZ_DETL		(F_TYPE + 1)
#define		F_SPZ_NBR		(F_SPZ_DETL + 1)
#define		F_BID_ALW		(F_SPZ_NBR + 1)
#define		F_MDT_SCREEN		(F_BID_ALW + 1)
#define		F_MAX_TIME_CALLS	(F_MDT_SCREEN + 1)
#define         F_TC_REST_INT           (F_MAX_TIME_CALLS + 1)
#define		F_MX_DSP_TM 		(F_TC_REST_INT + 1)
#define		F_PKUP_L_TM 		(F_MX_DSP_TM + 1)
#define         F_X_PKUP_L_TM           (F_PKUP_L_TM + 1)
#define		F_ST_TM_FRM		(F_X_PKUP_L_TM + 1)
#define		F_ST_TM_TO		(F_ST_TM_FRM + 1)
#define		F_EP_ALWD 		(F_ST_TM_TO + 1)
#define		F_EBOOK_LEAD_TM		(F_EP_ALWD + 1)
#define         F_TC_LOOK_AHEAD         (F_EBOOK_LEAD_TM + 1)
#define         F_CBOOK_TIME            (F_TC_LOOK_AHEAD + 1)
#define		F_P_SL_1		(F_CBOOK_TIME + 1)
#define		F_P_SL_2		(F_P_SL_1 + 1)
#define		F_P_SL_3		(F_P_SL_2 + 1)
#define		F_P_SL_4 		(F_P_SL_3 + 1)
#define		F_P_SL_5		(F_P_SL_4 + 1)
#define		F_P_SL_6		(F_P_SL_5 + 1)
#define		F_P_SL_7		(F_P_SL_6 + 1)
#define		F_P_SL_8		(F_P_SL_7 + 1)
#define		F_P_SL_9		(F_P_SL_8 + 1)
#define		F_P_SL_10		(F_P_SL_9 + 1)
#define		F_P_SL_11		(F_P_SL_10 + 1)
#define		F_P_SL_12		(F_P_SL_11 + 1)
#define		F_P_SL_13		(F_P_SL_12 + 1)
#define		F_P_SL_14		(F_P_SL_13 + 1)
#define		F_P_SL_15		(F_P_SL_14 + 1)
#define		F_P_SL_16		(F_P_SL_15 + 1)
#define		F_BREAK_DURATION	(F_P_SL_16 + 1)
#define		F_ACPT_DURATION		(F_BREAK_DURATION + 1)
#define         F_CHECK_TAXIS           (F_ACPT_DURATION + 1)
#define         F_NS_SHORT              (F_CHECK_TAXIS + 1) 
#define         F_GPS_LAT               (F_NS_SHORT + 1)
#define         F_GPS_LONG              (F_GPS_LAT     + 1)
#define         F_GPS_RADIUS            (F_GPS_LONG + 1)
#define         F_GPS_DIST_CEILING      (F_GPS_RADIUS + 1)
#define         F_GPS_EMERG_RANGE       (F_GPS_DIST_CEILING + 1)
#define		F_L1_ALG		(F_GPS_EMERG_RANGE + 1)
#define		F_L1_ETM		(F_L1_ALG + 1)
#define		F_L1_RPN		(F_L1_ETM + 1)
#define		F_L1_Z1			(F_L1_RPN + 1)
#define		F_L1_Z2			(F_L1_Z1 + 1)
#define		F_L1_Z3			(F_L1_Z2 + 1)
#define		F_L1_Z4			(F_L1_Z3 + 1)
#define		F_L1_Z5			(F_L1_Z4 + 1)
#define		F_L1_Z6			(F_L1_Z5 + 1)
#define		F_L1_Z7			(F_L1_Z6 + 1)
#define		F_L1_Z8			(F_L1_Z7 + 1)
#define         F_MATCH_ALGORITHM       (F_L1_Z8 + 1)
#define		F_PRM_ETM		(F_MATCH_ALGORITHM + 1)
#define		F_PRM_RPN		(F_PRM_ETM + 1)
#define		F_L2_ALG		(F_PRM_RPN + 1)
#define		F_L2_ETM		(F_L2_ALG + 1)
#define		F_L2_RPN		(F_L2_ETM + 1)
#define		F_L2_Z1			(F_L2_RPN + 1)
#define		F_L2_Z2			(F_L2_Z1 + 1)
#define		F_L2_Z3			(F_L2_Z2 + 1)
#define		F_L2_Z4			(F_L2_Z3 + 1)
#define		F_L2_Z5			(F_L2_Z4 + 1)
#define		F_L2_Z6			(F_L2_Z5 + 1)
#define		F_L2_Z7			(F_L2_Z6 + 1)
#define		F_L2_Z8			(F_L2_Z7 + 1)
#define		F_L3_ALG		(F_L2_Z8 + 1)
#define		F_L3_ETM		(F_L3_ALG + 1)
#define		F_L3_RPN		(F_L3_ETM + 1)
#define		F_L3_Z1			(F_L3_RPN + 1)
#define		F_L3_Z2			(F_L3_Z1 + 1)
#define		F_L3_Z3			(F_L3_Z2 + 1)
#define		F_L3_Z4			(F_L3_Z3 + 1)
#define		F_L3_Z5			(F_L3_Z4 + 1)
#define		F_L3_Z6			(F_L3_Z5 + 1)
#define		F_L3_Z7			(F_L3_Z6 + 1)
#define		F_L3_Z8			(F_L3_Z7 + 1)
#define		F_L4_ALG		(F_L3_Z8 + 1)
#define		F_L4_ETM		(F_L4_ALG + 1)
#define		F_L4_RPN		(F_L4_ETM + 1)
#define		F_L4_Z1			(F_L4_RPN + 1)
#define		F_L4_Z2			(F_L4_Z1 + 1)
#define		F_L4_Z3			(F_L4_Z2 + 1)
#define		F_L4_Z4			(F_L4_Z3 + 1)
#define		F_L4_Z5			(F_L4_Z4 + 1)
#define		F_L4_Z6			(F_L4_Z5 + 1)
#define		F_L4_Z7			(F_L4_Z6 + 1)
#define		F_L4_Z8			(F_L4_Z7 + 1)
#define		F_L5_ALG		(F_L4_Z8 + 1)
#define		F_L5_ETM		(F_L5_ALG + 1)
#define		F_L5_RPN		(F_L5_ETM + 1)
#define		F_L5_Z1			(F_L5_RPN + 1)
#define		F_L5_Z2			(F_L5_Z1 + 1)
#define		F_L5_Z3			(F_L5_Z2 + 1)
#define		F_L5_Z4			(F_L5_Z3 + 1)
#define		F_L5_Z5			(F_L5_Z4 + 1)
#define		F_L5_Z6			(F_L5_Z5 + 1)
#define		F_L5_Z7			(F_L5_Z6 + 1)
#define		F_L5_Z8			(F_L5_Z7 + 1)
#define		F_L6_ALG		(F_L5_Z8 + 1)
#define		F_L6_ETM		(F_L6_ALG + 1)
#define		F_L6_RPN		(F_L6_ETM + 1)
#define		F_L6_Z1			(F_L6_RPN + 1)
#define		F_L6_Z2			(F_L6_Z1 + 1)
#define		F_L6_Z3			(F_L6_Z2 + 1)
#define		F_L6_Z4			(F_L6_Z3 + 1)
#define		F_L6_Z5			(F_L6_Z4 + 1)
#define		F_L6_Z6			(F_L6_Z5 + 1)
#define		F_L6_Z7			(F_L6_Z6 + 1)
#define		F_L6_Z8			(F_L6_Z7 + 1)
#define		F_L7_ALG		(F_L6_Z8 + 1)
#define		F_L7_ETM		(F_L7_ALG + 1)
#define		F_L7_RPN		(F_L7_ETM + 1)
#define		F_L7_Z1			(F_L7_RPN + 1)
#define		F_L7_Z2			(F_L7_Z1 + 1)
#define		F_L7_Z3			(F_L7_Z2 + 1)
#define		F_L7_Z4			(F_L7_Z3 + 1)
#define		F_L7_Z5			(F_L7_Z4 + 1)
#define		F_L7_Z6			(F_L7_Z5 + 1)
#define		F_L7_Z7			(F_L7_Z6 + 1)
#define		F_L7_Z8			(F_L7_Z7 + 1)
#define		F_L8_ALG		(F_L7_Z8 + 1)
#define		F_L8_ETM		(F_L8_ALG + 1)
#define		F_L8_RPN		(F_L8_ETM + 1)
#define		F_L8_Z1			(F_L8_RPN + 1)
#define		F_L8_Z2			(F_L8_Z1 + 1)
#define		F_L8_Z3			(F_L8_Z2 + 1)
#define		F_L8_Z4			(F_L8_Z3 + 1)
#define		F_L8_Z5			(F_L8_Z4 + 1)
#define		F_L8_Z6			(F_L8_Z5 + 1)
#define		F_L8_Z7			(F_L8_Z6 + 1)
#define		F_L8_Z8			(F_L8_Z7 + 1)
#define         F_SCR_1_MAX             (F_L8_Z7 + 1)

#define         F_SCR_2_MIN             (F_SCR_1_MAX + 1)
#define         F_STAND_TELE            (F_SCR_2_MIN)
#define         F_HTTP_TIMER            (F_STAND_TELE + 1)
#define         F_ROTI                  (F_HTTP_TIMER + 1)
#define         F_STAND_NBR             (F_ROTI + 1)
#define         F_AUTO_INQRY_1          (F_STAND_NBR + 1)
#define         F_AUTO_INQRY_2          (F_AUTO_INQRY_1 + 1)
#define         F_AUTO_INQRY_3          (F_AUTO_INQRY_2 + 1)
#define         F_AUTO_INQRY_4          (F_AUTO_INQRY_3 + 1)
#define         F_AUTO_INQRY_5          (F_AUTO_INQRY_4 + 1)
#define         F_AUTO_INQRY_6          (F_AUTO_INQRY_5 + 1)
#define         F_AUTO_INQRY_7          (F_AUTO_INQRY_6 + 1)
#define         F_AUTO_INQRY_8          (F_AUTO_INQRY_7 + 1)
#define         F_AUTO_INQRY_9          (F_AUTO_INQRY_8 + 1)
#define         F_AUTO_INQRY_10         (F_AUTO_INQRY_9 + 1)
#define         F_SCR_2_MAX             (F_AUTO_INQRY_10)
#define		NUM_FLDS		F_AUTO_INQRY_10

#define		BUFFER_SIZE		133	/* size of buffer for reading input fields */
#define		VALID			1	/* Input field was valid */
#define		NOT_VALID		-1	/* Input field was not valid */
#define		CR			015	/* Return character */
#define		NEW_LINE		012	/* New line character */
#define		F_NAME_SIZE		15	/* Maximum file name size */
