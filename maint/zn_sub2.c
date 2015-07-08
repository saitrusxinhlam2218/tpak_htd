static char sz__FILE__[]="@(#)zn_sub2.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  zn_sub2.c; Rel: 6.9.0.0; Get date: 4/9/92 at 09:26:33
* @(#)  Last delta: 1/16/92 at 16:44:14
* @(#)  SCCS File: /taxi/sccs/s.zn_sub2.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <isam.h>
#include <cursesX.h>
#include <signal.h>
#include <setjmp.h>

#include "zn_m_strdefs.h"
#include "mad_error.h"
#include "mads_types.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "df_maint.h"
#include "mads_isam.h"
#include "ex_key.h"
#include "zn_maint.h"
#include "zn_msg.h"
#include "tokenize.h"
#include "language.h"

extern int do_jmp;		/* flag indicating longjmp processing status	*/
extern jmp_buf jmp_env;		/* environment storage for longjmp		*/
extern int upper_case;
extern int doing_add;			/* prompt in use */
extern int ret_val;		/* return value */
extern int piu;			/* prompt in use */
extern int intr_allowed;	/* interrupts allowed */
extern int key_val;		/* value returned from the check routines */
extern int key_desc();		/* value returned from the check routines */
extern char read_buf[];		/* buffer for reading fields */
int end_read_field;		/* flag, 0/1, set to end read_field function */
extern int error_flag;
extern int field_entered;	/* field entered or not */
extern int saw_intr;		/* indicates an interrupt signal was entered */
extern int cur_list;		/* indicates a row is in the current list */
extern int (*check_field[])();	/* Array of pointers to routines to validate
				   the data input by the user */
extern int record_nbr;		/* C_ISAM record number */
extern char o_file[];		/* output file as specified by the user */
extern char mode[];		/* user specified Append or Create */	

extern struct scr_flds zone_scr_fld[];
extern struct df_maint df_maint_ipc;	/* structure for passing information 
				   	   to dispatch  */
extern struct cisam_zn *zn_ptr;	/* pointer to zone file structure */
extern struct cisam_zn *temp_zn_ptr;
extern struct cisam_zn zn_buf;	
extern int updated_zones[];
extern int range_low, range_high;
extern int scr_nbr;		/* screen number */

/******************************************************************************
* prt_labels - print the field labels on the screen.
*******************************************************************************/

void prt_labels()
{
int   i;
	char 	 open_brace[2],  close_brace[2];

#ifdef DEBUG
	sprintf(trace_str,"Entering prt_labels\n");
	TRACE(trace_str);
#endif

	strcpy(open_brace, catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_63, "[" ));
	strcpy(close_brace, catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_64, "]" ));

	/* print the screen labels  */
        if ( scr_nbr == 1 )
          {
        attrset(A_BOLD);
	mvaddstr(L_FLEET_ROW,L_FLEET_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_26, "Fleet"));
	mvaddstr(L_FLEET_ROW,L_ZONE_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_27, "Zone #"));
	mvaddstr(L_FLEET_ROW,L_DESC_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_28, "Description"));
	mvaddstr(L_FLEET_ROW,L_TYPE_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_29, "Type"));
	mvaddstr(L_FLEET_ROW,L_SPR_ZONE_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_30, "Superzone Detail"));
	mvaddstr(L_FLEET_ROW,L_SPZ_NBR_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_31, "Number"));
	mvaddstr(L_FLEET_ROW,L_MAX_TIME_CALLS_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_66, "# Time Calls"));
	mvaddstr(L_FLEET_ROW,L_TC_REST_INT_COL,"TC time period");
        attrset(0);
	mvaddstr(L_FLEET_ROW+1,L_FLEET_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_FLEET_COL+MC_FLEET+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_ZONE_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_ZONE_COL+MC_ZONE+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_DESC_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_DESC_COL+MC_DESC+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_TYPE_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_TYPE_COL+MC_TYPE+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_SPZ_DETL_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_SPZ_DETL_COL+MC_SPZ_DETL+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_SPZ_NBR_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_SPZ_NBR_COL+MC_SPZ_NBR+1,close_brace);

        attrset(A_BOLD);
	mvaddstr(L_MDT_SCREEN_ROW, L_MDT_SCREEN_COL, catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_69, "MDT Screen"));
        attrset(0);
	mvaddstr(D_MDT_SCREEN_ROW, D_MDT_SCREEN_COL - 1, open_brace);
	mvaddstr(D_MDT_SCREEN_ROW, D_MDT_SCREEN_COL + MC_MDT_SCREEN, close_brace);

	mvaddstr(L_FLEET_ROW+1,L_MAX_TIME_CALLS_COL - 1,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_MAX_TIME_CALLS_COL+MC_MAX_TIME_CALLS+1,close_brace);

	mvaddstr(L_FLEET_ROW+1,L_TC_REST_INT_COL - 1,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_TC_REST_INT_COL+MC_TC_REST_INT+1,close_brace);

        attrset(A_BOLD);
	mvaddstr(L_MX_DSP_TM_ROW,L_MX_DSP_TM_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_33, "Max Dispatch Time"));
	mvaddstr(L_MX_DSP_TM_ROW,L_PKUP_L_TM_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_34, "Lead Time"));
	mvaddstr(L_MX_DSP_TM_ROW,L_PKUP_L_TM_COL+12, "X");
	mvaddstr(L_MX_DSP_TM_ROW,L_TYPE_COL+1,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_35, "Stand Time From"));
	mvaddstr(L_MX_DSP_TM_ROW,L_ST_TM_TO_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_36, "To"));
	mvaddstr(L_MX_DSP_TM_ROW,L_EP_ALWD_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_37, "EBOOK Allowed"));
        attrset(0);
	mvaddstr(L_MX_DSP_TM_ROW+1,L_MX_DSP_TM_COL,open_brace);
	mvaddstr(L_MX_DSP_TM_ROW+1,L_MX_DSP_TM_COL+MC_MX_DSP_TM+1,close_brace);
	mvaddstr(L_MX_DSP_TM_ROW+1,L_PKUP_L_TM_COL,open_brace);
	mvaddstr(L_MX_DSP_TM_ROW+1,L_PKUP_L_TM_COL+MC_PKUP_L_TM+1,close_brace);
	mvaddstr(L_MX_DSP_TM_ROW+1,L_X_PKUP_L_TM_COL,open_brace);
	mvaddstr(L_MX_DSP_TM_ROW+1,L_X_PKUP_L_TM_COL+MC_PKUP_L_TM+1,close_brace);	
	mvaddstr(L_MX_DSP_TM_ROW+1,L_ST_TM_FRM_COL,open_brace);
	mvaddstr(L_MX_DSP_TM_ROW+1,L_ST_TM_FRM_COL+MC_ST_TM_FRM+1,close_brace);
	mvaddstr(L_MX_DSP_TM_ROW+1,L_ST_TM_TO_COL,open_brace);
	mvaddstr(L_MX_DSP_TM_ROW+1,L_ST_TM_TO_COL+MC_ST_TM_TO+1,close_brace);
	mvaddstr(L_MX_DSP_TM_ROW+1,L_EP_ALWD_COL,open_brace);
	mvaddstr(L_MX_DSP_TM_ROW+1,L_EP_ALWD_COL+MC_EP_ALWD+1,close_brace);

        attrset(A_BOLD);

	mvaddstr(L_EBOOK_LEAD_TM_ROW, L_EBOOK_LEAD_TM_COL+4, catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_70,
									 "EBOOK Lead"));
	mvaddstr(L_EBOOK_LEAD_TM_ROW, L_TC_LOOK_AHEAD_COL+5, catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_83,
								   "TC Int"));
	mvaddstr(L_EBOOK_LEAD_TM_ROW, L_CBOOK_TIME_COL, catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_88,
								"Cbook Time"));
        attrset(0);
	mvaddstr(D_EBOOK_LEAD_TM_ROW, D_EBOOK_LEAD_TM_COL - 1, open_brace);
	mvaddstr(D_EBOOK_LEAD_TM_ROW, D_EBOOK_LEAD_TM_COL + MC_EBOOK_LEAD_TM, close_brace);
	mvaddstr(D_EBOOK_LEAD_TM_ROW, D_TC_LOOK_AHEAD_COL - 1, open_brace);
	mvaddstr(D_EBOOK_LEAD_TM_ROW, D_TC_LOOK_AHEAD_COL + MC_TC_LOOK_AHEAD, close_brace);
	mvaddstr(D_EBOOK_LEAD_TM_ROW, D_CBOOK_TIME_COL - 1, open_brace);
	mvaddstr(D_EBOOK_LEAD_TM_ROW, D_CBOOK_TIME_COL + MC_CBOOK_TIME, close_brace);


        attrset(A_BOLD);
	mvaddstr(L_PKUP_SLS_ROW,L_PKUP_SLS_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_38, "Pickup Seals"));
        attrset(0);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_1_COL,open_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_1_COL+MC_P_SL_1+1,close_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_2_COL,open_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_2_COL+MC_P_SL_2+1,close_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_3_COL,open_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_3_COL+MC_P_SL_3+1,close_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_4_COL,open_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_4_COL+MC_P_SL_4+1,close_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_5_COL,open_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_5_COL+MC_P_SL_5+1,close_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_6_COL,open_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_6_COL+MC_P_SL_6+1,close_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_7_COL,open_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_7_COL+MC_P_SL_7+1,close_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_8_COL,open_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_8_COL+MC_P_SL_8+1,close_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_9_COL,open_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_9_COL+MC_P_SL_9+1,close_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_10_COL,open_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_10_COL+MC_P_SL_10+1,close_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_11_COL,open_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_11_COL+MC_P_SL_11+1,close_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_12_COL,open_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_12_COL+MC_P_SL_12+1,close_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_13_COL,open_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_13_COL+MC_P_SL_13+1,close_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_14_COL,open_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_14_COL+MC_P_SL_14+1,close_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_15_COL,open_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_15_COL+MC_P_SL_15+1,close_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_16_COL,open_brace);
	mvaddstr(L_PKUP_SLS_ROW+1,L_P_SL_16_COL+MC_P_SL_16+1,close_brace);
        attrset(A_BOLD);
	mvaddstr(L_BREAK_DURATION_ROW,L_BREAK_DURATION_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET,ZN_MAINT_72,"Break"));
        attrset(0);
	mvaddstr(D_BREAK_DURATION_ROW,D_BREAK_DURATION_COL-1,open_brace);
	mvaddstr(D_BREAK_DURATION_ROW,D_BREAK_DURATION_COL+MC_BREAK_DURATION,close_brace);
        attrset(A_BOLD);
	mvaddstr(L_ACPT_DURATION_ROW,L_ACPT_DURATION_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET,ZN_MAINT_75,"Accept"));
        mvaddstr(L_CHECK_TAXIS_ROW, L_CHECK_TAXIS_COL, catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_77, "Check Taxis" ) );
	mvaddstr(L_CHECK_TAXIS_ROW, L_NS_SHORT_COL, catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_84, "Short Notrip"));
        attrset(0);
	mvaddstr(D_ACPT_DURATION_ROW,D_ACPT_DURATION_COL-1,open_brace);
	mvaddstr(D_ACPT_DURATION_ROW,D_ACPT_DURATION_COL+MC_ACPT_DURATION,close_brace);
        mvaddstr(D_CHECK_TAXIS_ROW, D_CHECK_TAXIS_COL - 1, open_brace );
        mvaddstr(D_CHECK_TAXIS_ROW, D_CHECK_TAXIS_COL + MC_CHECK_TAXIS, close_brace );
	mvaddstr(D_CHECK_TAXIS_ROW, D_NS_SHORT_COL - 1, open_brace );
	mvaddstr(D_CHECK_TAXIS_ROW, D_NS_SHORT_COL + MC_NS_SHORT, close_brace);


        attrset(A_BOLD);
	mvaddstr(L_LEVEL_ROW,L_LEVEL_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_39, "Level"));
	mvaddstr(L_LEVEL_ROW,L_ALG_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_40, "Alg"));
	mvaddstr(L_LEVEL_ROW,L_EXC_TM_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_41, "Excl Time"));
	mvaddstr(L_LEVEL_ROW,L_REJ_PEN_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_42, "Rej Penalty"));
	mvaddstr(L_LEVEL_ROW,L_ZONE_1_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_43, "Zone 1"));
	mvaddstr(L_LEVEL_ROW,L_ZONE_2_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_44, "Zone 2"));
	mvaddstr(L_LEVEL_ROW,L_ZONE_3_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_45, "Zone 3"));
	mvaddstr(L_LEVEL_ROW,L_ZONE_4_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_46, "Zone 4"));
	mvaddstr(L_LEVEL_ROW,L_ZONE_5_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_47, "Zone 5"));
	mvaddstr(L_LEVEL_ROW,L_ZONE_6_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_48, "Zone 6"));
	mvaddstr(L_LEVEL_ROW,L_ZONE_7_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_49, "Zone 7"));
	mvaddstr(L_LEVEL_ROW,L_ZONE_8_COL,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_50, "Zone 8"));
        attrset(0);
	mvaddstr(L_LEVEL_ROW+1,L_LEVEL_COL+2,"1");
	mvaddstr(L_LEVEL_ROW+1,L_ALG_COL,open_brace);
	mvaddstr(L_LEVEL_ROW+1,L_ALG_COL+MC_L1_ALG+1,close_brace);
	mvaddstr(L_LEVEL_ROW+1,L_EXC_TM_COL,open_brace);
	mvaddstr(L_LEVEL_ROW+1,L_EXC_TM_COL+MC_L1_ETM+1,close_brace);
	mvaddstr(L_LEVEL_ROW+1,L_REJ_PEN_COL,open_brace);
	mvaddstr(L_LEVEL_ROW+1,L_REJ_PEN_COL+MC_L1_RPN+1,close_brace);
	mvaddstr(L_LEVEL_ROW+1,L_ZONE_1_COL,open_brace);
	mvaddstr(L_LEVEL_ROW+1,L_ZONE_1_COL+MC_L1_Z1+1,close_brace);
	mvaddstr(L_LEVEL_ROW+1,L_ZONE_2_COL,open_brace);
	mvaddstr(L_LEVEL_ROW+1,L_ZONE_2_COL+MC_L2_Z1+1,close_brace);
	mvaddstr(L_LEVEL_ROW+1,L_ZONE_3_COL,open_brace);
	mvaddstr(L_LEVEL_ROW+1,L_ZONE_3_COL+MC_L3_Z1+1,close_brace);
	mvaddstr(L_LEVEL_ROW+1,L_ZONE_4_COL,open_brace);
	mvaddstr(L_LEVEL_ROW+1,L_ZONE_4_COL+MC_L4_Z1+1,close_brace);
	mvaddstr(L_LEVEL_ROW+1,L_ZONE_5_COL,open_brace);
	mvaddstr(L_LEVEL_ROW+1,L_ZONE_5_COL+MC_L5_Z1+1,close_brace);
	mvaddstr(L_LEVEL_ROW+1,L_ZONE_6_COL,open_brace);
	mvaddstr(L_LEVEL_ROW+1,L_ZONE_6_COL+MC_L6_Z1+1,close_brace);
	mvaddstr(L_LEVEL_ROW+1,L_ZONE_7_COL,open_brace);
	mvaddstr(L_LEVEL_ROW+1,L_ZONE_7_COL+MC_L7_Z1+1,close_brace);
	mvaddstr(L_LEVEL_ROW+1,L_ZONE_8_COL,open_brace);
	mvaddstr(L_LEVEL_ROW+1,L_ZONE_8_COL+MC_L8_Z1+1,close_brace);
        attrset(A_BOLD);
	mvaddstr(L_LVL_1_ROW+1,L_MX_DSP_TM_COL+GPS_ADD_OFFSET,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_51, "Primary"));
        attrset(0);
	mvaddstr(L_PRM_ROW,L_EXC_TM_COL,open_brace);
	mvaddstr(L_PRM_ROW,L_EXC_TM_COL+MC_L1_ETM+1,close_brace);
	mvaddstr(L_PRM_ROW,L_REJ_PEN_COL,open_brace);
	mvaddstr(L_PRM_ROW,L_REJ_PEN_COL+MC_L1_RPN+1,close_brace);


	mvaddstr(L_LVL_2_ROW,L_LVL_2_COL,"2");
	mvaddstr(L_LVL_3_ROW,L_LVL_3_COL,"3");
	mvaddstr(L_LVL_4_ROW,L_LVL_4_COL,"4");
	mvaddstr(L_LVL_5_ROW,L_LVL_5_COL,"5");
	mvaddstr(L_LVL_6_ROW,L_LVL_6_COL,"6");
	mvaddstr(L_LVL_7_ROW,L_LVL_7_COL,"7");
	mvaddstr(L_LVL_8_ROW,L_LVL_8_COL,"8");

for (i = 0; i < 7; ++i)		{
	mvaddstr(L_LVL_2_ROW+i,L_ALG_COL,open_brace);
	mvaddstr(L_LVL_2_ROW+i,L_ALG_COL+MC_L1_ALG+1,close_brace);
	mvaddstr(L_LVL_2_ROW+i,L_EXC_TM_COL,open_brace);
	mvaddstr(L_LVL_2_ROW+i,L_EXC_TM_COL+MC_L1_ETM+1,close_brace);
	mvaddstr(L_LVL_2_ROW+i,L_REJ_PEN_COL,open_brace);
	mvaddstr(L_LVL_2_ROW+i,L_REJ_PEN_COL+MC_L1_RPN+1,close_brace);
	mvaddstr(L_LVL_2_ROW+i,L_ZONE_1_COL,open_brace);
	mvaddstr(L_LVL_2_ROW+i,L_ZONE_1_COL+MC_L1_Z1+1,close_brace);
	mvaddstr(L_LVL_2_ROW+i,L_ZONE_2_COL,open_brace);
	mvaddstr(L_LVL_2_ROW+i,L_ZONE_2_COL+MC_L2_Z1+1,close_brace);
	mvaddstr(L_LVL_2_ROW+i,L_ZONE_3_COL,open_brace);
	mvaddstr(L_LVL_2_ROW+i,L_ZONE_3_COL+MC_L3_Z1+1,close_brace);
	mvaddstr(L_LVL_2_ROW+i,L_ZONE_4_COL,open_brace);
	mvaddstr(L_LVL_2_ROW+i,L_ZONE_4_COL+MC_L4_Z1+1,close_brace);
	mvaddstr(L_LVL_2_ROW+i,L_ZONE_5_COL,open_brace);
	mvaddstr(L_LVL_2_ROW+i,L_ZONE_5_COL+MC_L5_Z1+1,close_brace);
	mvaddstr(L_LVL_2_ROW+i,L_ZONE_6_COL,open_brace);
	mvaddstr(L_LVL_2_ROW+i,L_ZONE_6_COL+MC_L6_Z1+1,close_brace);
	mvaddstr(L_LVL_2_ROW+i,L_ZONE_7_COL,open_brace);
	mvaddstr(L_LVL_2_ROW+i,L_ZONE_7_COL+MC_L7_Z1+1,close_brace);
	mvaddstr(L_LVL_2_ROW+i,L_ZONE_8_COL,open_brace);
	mvaddstr(L_LVL_2_ROW+i,L_ZONE_8_COL+MC_L8_Z1+1,close_brace);

        attrset(A_BOLD);
        mvaddstr(L_GPS_LAT_ROW,L_GPS_LAT_COL, "GPS Y" );
        mvaddstr(L_GPS_LONG_ROW,L_GPS_LONG_COL,"GPS X" );
        mvaddstr(L_GPS_RADIUS_ROW, L_GPS_RADIUS_COL, "GPS Radius" );
	mvaddstr(L_GPS_DIST_CEILING_ROW, L_GPS_DIST_CEILING_COL, "GPS Customer Radius" );
        mvaddstr(L_GPS_EMERG_RANGE_ROW, L_GPS_EMERG_RANGE_COL, "Emerg. Brcst." );
        attrset(0);
        mvaddstr(D_GPS_LAT_ROW,D_GPS_LAT_COL-1,open_brace);
        mvaddstr(D_GPS_LAT_ROW,D_GPS_LAT_COL+MC_GPS_LAT,close_brace);
        mvaddstr(D_GPS_LONG_ROW,D_GPS_LONG_COL-1,open_brace);
        mvaddstr(D_GPS_LONG_ROW,D_GPS_LONG_COL+MC_GPS_LONG,close_brace);
        mvaddstr(D_GPS_RADIUS_ROW,D_GPS_RADIUS_COL-1,open_brace);
        mvaddstr(D_GPS_RADIUS_ROW,D_GPS_RADIUS_COL+MC_GPS_RADIUS,close_brace);
	mvaddstr(D_GPS_DIST_CEILING_ROW,D_GPS_DIST_CEILING_COL-1,open_brace);
	mvaddstr(D_GPS_DIST_CEILING_ROW,D_GPS_DIST_CEILING_COL+MC_GPS_DIST_CEILING,close_brace);
        mvaddstr(D_GPS_EMERG_RANGE_ROW,D_GPS_EMERG_RANGE_COL-1,open_brace);
        mvaddstr(D_GPS_EMERG_RANGE_ROW,D_GPS_EMERG_RANGE_COL+MC_GPS_EMERG_RANGE,close_brace);
	mvaddstr(D_MATCH_ALGORITHM_ROW,D_MATCH_ALGORITHM_COL-1,open_brace);
	mvaddstr(D_MATCH_ALGORITHM_ROW,D_MATCH_ALGORITHM_COL+MC_MATCH_ALGORITHM,close_brace);

      }


      }
        if ( scr_nbr == 2 )
          {
            attrset(A_BOLD);
            mvaddstr(L_FLEET_ROW+2, L_STAND_TELE_COL, "Stand Telephone");
            mvaddstr(L_FLEET_ROW+2, L_HTTP_TIMER_COL, "HTTP Timer" );
	    mvaddstr(L_FLEET_ROW+2, L_ROTI_COL, "ROTI");
	    mvaddstr(L_FLEET_ROW+2, L_STAND_NBR_COL, "Stand Nbr");
            attrset(0);
            mvaddstr(L_FLEET_ROW+3,L_STAND_TELE_COL, open_brace);
            mvaddstr(L_FLEET_ROW+3,L_STAND_TELE_COL+MC_STAND_TELE+1,close_brace);
            mvaddstr(L_FLEET_ROW+3,L_HTTP_TIMER_COL, open_brace);
            mvaddstr(L_FLEET_ROW+3,L_HTTP_TIMER_COL+MC_HTTP_TIMER+1,close_brace);
	    mvaddstr(L_FLEET_ROW+3,L_ROTI_COL, open_brace);
	    mvaddstr(L_FLEET_ROW+3,L_ROTI_COL+MC_ROTI+1,close_brace);
	    mvaddstr(L_FLEET_ROW+3,L_STAND_NBR_COL, open_brace);
	    mvaddstr(L_FLEET_ROW+3,L_STAND_NBR_COL+MC_STAND_NBR+1,close_brace);
	    attrset(A_BOLD);
	    mvaddstr(L_FLEET_ROW+4,L_AUTO_INQRY_COL, "Auto Inquiry Zones");
	    attrset(0);
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL, open_brace);
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+MC_AUTO_INQRY+1,close_brace);
	    
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+6, open_brace);
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+6+MC_AUTO_INQRY+1,close_brace);
	    	    
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+12, open_brace);
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+12+MC_AUTO_INQRY+1,close_brace);
	    	    
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+18, open_brace);
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+18+MC_AUTO_INQRY+1,close_brace);
	    	    
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+24, open_brace);
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+24+MC_AUTO_INQRY+1,close_brace);
	    	    
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+30, open_brace);
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+30+MC_AUTO_INQRY+1,close_brace);
	    	    
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+36, open_brace);
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+36+MC_AUTO_INQRY+1,close_brace);
	    	    
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+42, open_brace);
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+42+MC_AUTO_INQRY+1,close_brace);
	    	    
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+48, open_brace);
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+48+MC_AUTO_INQRY+1,close_brace);
	    	    
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+54, open_brace);
	    mvaddstr(L_FLEET_ROW+5,L_AUTO_INQRY_COL+54+MC_AUTO_INQRY+1,close_brace);	    

	    
          }

	refresh();

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_labels\n");
	TRACE(trace_str);
#endif

}  /* End of prt_labels  */

/******************************************************************************
* prt_data - print the data on the screen.
*******************************************************************************/

prt_data()
{

	int i;			/* loop index */

#ifdef DEBUG
	sprintf(trace_str,"Entering prt_data\n");
	TRACE(trace_str);
#endif

	//clear_flds();

	/* Fill in the data items on the screen */

        if (scr_nbr == 1)
          {
	mvprintw(D_FLEET_ROW,D_FLEET_COL,"%c",zn_ptr->fleet);
	mvprintw(D_FLEET_ROW,D_ZONE_COL,"%5d",zn_ptr->nbr);
	mvprintw(D_FLEET_ROW,D_DESC_COL,"%s",zn_ptr->desc);
	mvprintw(D_FLEET_ROW,D_TYPE_COL,"%c",zn_ptr->type);
	mvprintw(D_FLEET_ROW,D_SPZ_DETL_COL,"%c",zn_ptr->superzn_detail);
	mvprintw(D_FLEET_ROW,D_SPZ_NBR_COL,"%5d",zn_ptr->superzn_nbr);
/* Bid Allowed field no longer used
	mvprintw(D_FLEET_ROW,D_BID_ALW_COL,"%c",zn_ptr->bid_allowed);
*/
	mvprintw(D_MDT_SCREEN_ROW, D_MDT_SCREEN_COL, "%d", zn_ptr->mdt_screen);
	mvprintw(D_FLEET_ROW,D_MAX_TIME_CALLS_COL,"%3d",zn_ptr->max_time_calls);
	mvprintw(D_FLEET_ROW,D_TC_REST_INT_COL,"%3d",zn_ptr->tc_restrict_interval);
	mvprintw(D_MX_DSP_TM_ROW,D_MX_DSP_TM_COL,"%5d",zn_ptr->avl_call_q_time);
	mvprintw(D_MX_DSP_TM_ROW,D_PKUP_L_TM_COL,"%5d",zn_ptr->pckup_lead_time);
	mvprintw(D_MX_DSP_TM_ROW,D_X_PKUP_L_TM_COL,"%5d",zn_ptr->typex_lead_time);	
	mvprintw(D_MX_DSP_TM_ROW,D_ST_TM_FRM_COL,"%5d",zn_ptr->stand_time_from);
	mvprintw(D_MX_DSP_TM_ROW,D_ST_TM_TO_COL,"%5d",zn_ptr->stand_time_to);
	mvprintw(D_MX_DSP_TM_ROW,D_EP_ALWD_COL,"%c",zn_ptr->enr_post_allow);
	mvprintw(D_EBOOK_LEAD_TM_ROW, D_EBOOK_LEAD_TM_COL, "%5d", zn_ptr->enr_post_lead);
	mvprintw(D_EBOOK_LEAD_TM_ROW, D_TC_LOOK_AHEAD_COL, "%3d", zn_ptr->tc_look_ahead);
	mvprintw(D_EBOOK_LEAD_TM_ROW, D_CBOOK_TIME_COL, "%5d", zn_ptr->cbook_time);
	mvprintw(D_PKUP_SLS_ROW,D_P_SL_1_COL,"%c",zn_ptr->pckup_seals[0]);
	mvprintw(D_PKUP_SLS_ROW,D_P_SL_2_COL,"%c",zn_ptr->pckup_seals[1]);
	mvprintw(D_PKUP_SLS_ROW,D_P_SL_3_COL,"%c",zn_ptr->pckup_seals[2]);
	mvprintw(D_PKUP_SLS_ROW,D_P_SL_4_COL,"%c",zn_ptr->pckup_seals[3]);
	mvprintw(D_PKUP_SLS_ROW,D_P_SL_5_COL,"%c",zn_ptr->pckup_seals[4]);
	mvprintw(D_PKUP_SLS_ROW,D_P_SL_6_COL,"%c",zn_ptr->pckup_seals[5]);
	mvprintw(D_PKUP_SLS_ROW,D_P_SL_7_COL,"%c",zn_ptr->pckup_seals[6]);
	mvprintw(D_PKUP_SLS_ROW,D_P_SL_8_COL,"%c",zn_ptr->pckup_seals[7]);
	mvprintw(D_PKUP_SLS_ROW,D_P_SL_9_COL,"%c",zn_ptr->pckup_seals[8]);
	mvprintw(D_PKUP_SLS_ROW,D_P_SL_10_COL,"%c",zn_ptr->pckup_seals[9]);
	mvprintw(D_PKUP_SLS_ROW,D_P_SL_11_COL,"%c",zn_ptr->pckup_seals[10]);
	mvprintw(D_PKUP_SLS_ROW,D_P_SL_12_COL,"%c",zn_ptr->pckup_seals[11]);
	mvprintw(D_PKUP_SLS_ROW,D_P_SL_13_COL,"%c",zn_ptr->pckup_seals[12]);
	mvprintw(D_PKUP_SLS_ROW,D_P_SL_14_COL,"%c",zn_ptr->pckup_seals[13]);
	mvprintw(D_PKUP_SLS_ROW,D_P_SL_15_COL,"%c",zn_ptr->pckup_seals[14]);
	mvprintw(D_PKUP_SLS_ROW,D_P_SL_16_COL,"%c",zn_ptr->pckup_seals[15]);
	mvprintw(D_BREAK_DURATION_ROW,D_BREAK_DURATION_COL,"%5d",zn_ptr->break_duration);
	mvprintw(D_ACPT_DURATION_ROW,D_ACPT_DURATION_COL,"%5d",zn_ptr->accept_duration);
	mvprintw(D_CHECK_TAXIS_ROW, D_CHECK_TAXIS_COL, "%c", zn_ptr->check_avail_taxis);
	mvprintw(D_CHECK_TAXIS_ROW, D_NS_SHORT_COL, "%3d", zn_ptr->ns_short);
	mvprintw(D_GPS_LAT_ROW, D_GPS_LAT_COL, "%8.02f",zn_ptr->gps_lat);
	mvprintw(D_GPS_LONG_ROW, D_GPS_LONG_COL, "%8.02f",zn_ptr->gps_long);
	mvprintw(D_GPS_RADIUS_ROW, D_GPS_RADIUS_COL, "%6d",zn_ptr->gps_radius);
	mvprintw(D_GPS_DIST_CEILING_ROW,D_GPS_DIST_CEILING_COL,"%6d",zn_ptr->gps_dist_ceiling);
        mvprintw(D_GPS_EMERG_RANGE_ROW,D_GPS_EMERG_RANGE_COL,"%6d",zn_ptr->emerg_range);
	mvprintw(D_MATCH_ALGORITHM_ROW,D_MATCH_ALGORITHM_COL,"%c",zn_ptr->match_algorithm);
	mvprintw(D_LEVEL_ROW,D_L1_ALG_COL,"%c",zn_ptr->lev_alg[0]);
	mvprintw(D_LEVEL_ROW,D_L1_ETM_COL,"%5d",zn_ptr->lev_excl_time[0]);
	mvprintw(D_LEVEL_ROW,D_L1_RPN_COL,"%c",zn_ptr->lev_rej_penlty[0]);
	mvprintw(D_LEVEL_ROW,D_L1_Z1_COL,"%3d",zn_ptr->lev_zone_nbr[0][0]);
	mvprintw(D_LEVEL_ROW,D_L2_Z2_COL,"%3d",zn_ptr->lev_zone_nbr[0][1]);
	mvprintw(D_LEVEL_ROW,D_L3_Z3_COL,"%3d",zn_ptr->lev_zone_nbr[0][2]);
	mvprintw(D_LEVEL_ROW,D_L4_Z4_COL,"%3d",zn_ptr->lev_zone_nbr[0][3]);
	mvprintw(D_LEVEL_ROW,D_L5_Z5_COL,"%3d",zn_ptr->lev_zone_nbr[0][4]);
	mvprintw(D_LEVEL_ROW,D_L6_Z6_COL,"%3d",zn_ptr->lev_zone_nbr[0][5]);
	mvprintw(D_LEVEL_ROW,D_L7_Z7_COL,"%3d",zn_ptr->lev_zone_nbr[0][6]);
	mvprintw(D_LEVEL_ROW,D_L8_Z8_COL,"%3d",zn_ptr->lev_zone_nbr[0][7]);
	mvprintw(D_PRM_ETM_ROW,D_PRM_ETM_COL,"%5d",zn_ptr->lev_excl_time[1]);
	mvprintw(D_PRM_ETM_ROW,D_PRM_RPN_COL,"%c",zn_ptr->lev_rej_penlty[1]);
	mvprintw(D_LVL_2_ROW,D_L2_ALG_COL,"%c",zn_ptr->lev_alg[2]);
	mvprintw(D_LVL_2_ROW,D_L2_ETM_COL,"%5d",zn_ptr->lev_excl_time[2]);
	mvprintw(D_LVL_2_ROW,D_L2_RPN_COL,"%c",zn_ptr->lev_rej_penlty[2]);
	mvprintw(D_LVL_2_ROW,D_L2_Z1_COL,"%3d",zn_ptr->lev_zone_nbr[2][0]);
	mvprintw(D_LVL_2_ROW,D_L2_Z2_COL,"%3d",zn_ptr->lev_zone_nbr[2][1]);
	mvprintw(D_LVL_2_ROW,D_L2_Z3_COL,"%3d",zn_ptr->lev_zone_nbr[2][2]);
	mvprintw(D_LVL_2_ROW,D_L2_Z4_COL,"%3d",zn_ptr->lev_zone_nbr[2][3]);
	mvprintw(D_LVL_2_ROW,D_L2_Z5_COL,"%3d",zn_ptr->lev_zone_nbr[2][4]);
	mvprintw(D_LVL_2_ROW,D_L2_Z6_COL,"%3d",zn_ptr->lev_zone_nbr[2][5]);
	mvprintw(D_LVL_2_ROW,D_L2_Z7_COL,"%3d",zn_ptr->lev_zone_nbr[2][6]);
	mvprintw(D_LVL_2_ROW,D_L2_Z8_COL,"%3d",zn_ptr->lev_zone_nbr[2][7]);

	mvprintw(D_LVL_3_ROW,D_L3_ALG_COL,"%c",zn_ptr->lev_alg[3]);
	mvprintw(D_LVL_3_ROW,D_L3_ETM_COL,"%5d",zn_ptr->lev_excl_time[3]);
	mvprintw(D_LVL_3_ROW,D_L3_RPN_COL,"%c",zn_ptr->lev_rej_penlty[3]);
	mvprintw(D_LVL_3_ROW,D_L3_Z1_COL,"%3d",zn_ptr->lev_zone_nbr[3][0]);
	mvprintw(D_LVL_3_ROW,D_L3_Z2_COL,"%3d",zn_ptr->lev_zone_nbr[3][1]);
	mvprintw(D_LVL_3_ROW,D_L3_Z3_COL,"%3d",zn_ptr->lev_zone_nbr[3][2]);
	mvprintw(D_LVL_3_ROW,D_L3_Z4_COL,"%3d",zn_ptr->lev_zone_nbr[3][3]);
	mvprintw(D_LVL_3_ROW,D_L3_Z5_COL,"%3d",zn_ptr->lev_zone_nbr[3][4]);
	mvprintw(D_LVL_3_ROW,D_L3_Z6_COL,"%3d",zn_ptr->lev_zone_nbr[3][5]);
	mvprintw(D_LVL_3_ROW,D_L3_Z7_COL,"%3d",zn_ptr->lev_zone_nbr[3][6]);
	mvprintw(D_LVL_3_ROW,D_L3_Z8_COL,"%3d",zn_ptr->lev_zone_nbr[3][7]);

	mvprintw(D_LVL_4_ROW,D_L4_ALG_COL,"%c",zn_ptr->lev_alg[4]);
	mvprintw(D_LVL_4_ROW,D_L4_ETM_COL,"%5d",zn_ptr->lev_excl_time[4]);
	mvprintw(D_LVL_4_ROW,D_L4_RPN_COL,"%c",zn_ptr->lev_rej_penlty[4]);
	mvprintw(D_LVL_4_ROW,D_L4_Z1_COL,"%3d",zn_ptr->lev_zone_nbr[4][0]);
	mvprintw(D_LVL_4_ROW,D_L4_Z2_COL,"%3d",zn_ptr->lev_zone_nbr[4][1]);
	mvprintw(D_LVL_4_ROW,D_L4_Z3_COL,"%3d",zn_ptr->lev_zone_nbr[4][2]);
	mvprintw(D_LVL_4_ROW,D_L4_Z4_COL,"%3d",zn_ptr->lev_zone_nbr[4][3]);
	mvprintw(D_LVL_4_ROW,D_L4_Z5_COL,"%3d",zn_ptr->lev_zone_nbr[4][4]);
	mvprintw(D_LVL_4_ROW,D_L4_Z6_COL,"%3d",zn_ptr->lev_zone_nbr[4][5]);
	mvprintw(D_LVL_4_ROW,D_L4_Z7_COL,"%3d",zn_ptr->lev_zone_nbr[4][6]);
	mvprintw(D_LVL_4_ROW,D_L4_Z8_COL,"%3d",zn_ptr->lev_zone_nbr[4][7]);

	mvprintw(D_LVL_5_ROW,D_L5_ALG_COL,"%c",zn_ptr->lev_alg[5]);
	mvprintw(D_LVL_5_ROW,D_L5_ETM_COL,"%5d",zn_ptr->lev_excl_time[5]);
	mvprintw(D_LVL_5_ROW,D_L5_RPN_COL,"%c",zn_ptr->lev_rej_penlty[5]);
	mvprintw(D_LVL_5_ROW,D_L5_Z1_COL,"%3d",zn_ptr->lev_zone_nbr[5][0]);
	mvprintw(D_LVL_5_ROW,D_L5_Z2_COL,"%3d",zn_ptr->lev_zone_nbr[5][1]);
	mvprintw(D_LVL_5_ROW,D_L5_Z3_COL,"%3d",zn_ptr->lev_zone_nbr[5][2]);
	mvprintw(D_LVL_5_ROW,D_L5_Z4_COL,"%3d",zn_ptr->lev_zone_nbr[5][3]);
	mvprintw(D_LVL_5_ROW,D_L5_Z5_COL,"%3d",zn_ptr->lev_zone_nbr[5][4]);
	mvprintw(D_LVL_5_ROW,D_L5_Z6_COL,"%3d",zn_ptr->lev_zone_nbr[5][5]);
	mvprintw(D_LVL_5_ROW,D_L5_Z7_COL,"%3d",zn_ptr->lev_zone_nbr[5][6]);
	mvprintw(D_LVL_5_ROW,D_L5_Z8_COL,"%3d",zn_ptr->lev_zone_nbr[5][7]);

	mvprintw(D_LVL_6_ROW,D_L6_ALG_COL,"%c",zn_ptr->lev_alg[6]);
	mvprintw(D_LVL_6_ROW,D_L6_ETM_COL,"%5d",zn_ptr->lev_excl_time[6]);
	mvprintw(D_LVL_6_ROW,D_L6_RPN_COL,"%c",zn_ptr->lev_rej_penlty[6]);
	mvprintw(D_LVL_6_ROW,D_L6_Z1_COL,"%3d",zn_ptr->lev_zone_nbr[6][0]);
	mvprintw(D_LVL_6_ROW,D_L6_Z2_COL,"%3d",zn_ptr->lev_zone_nbr[6][1]);
	mvprintw(D_LVL_6_ROW,D_L6_Z3_COL,"%3d",zn_ptr->lev_zone_nbr[6][2]);
	mvprintw(D_LVL_6_ROW,D_L6_Z4_COL,"%3d",zn_ptr->lev_zone_nbr[6][3]);
	mvprintw(D_LVL_6_ROW,D_L6_Z5_COL,"%3d",zn_ptr->lev_zone_nbr[6][4]);
	mvprintw(D_LVL_6_ROW,D_L6_Z6_COL,"%3d",zn_ptr->lev_zone_nbr[6][5]);
	mvprintw(D_LVL_6_ROW,D_L6_Z7_COL,"%3d",zn_ptr->lev_zone_nbr[6][6]);
	mvprintw(D_LVL_6_ROW,D_L6_Z8_COL,"%3d",zn_ptr->lev_zone_nbr[6][7]);

	mvprintw(D_LVL_7_ROW,D_L7_ALG_COL,"%c",zn_ptr->lev_alg[7]);
	mvprintw(D_LVL_7_ROW,D_L7_ETM_COL,"%5d",zn_ptr->lev_excl_time[7]);
	mvprintw(D_LVL_7_ROW,D_L7_RPN_COL,"%c",zn_ptr->lev_rej_penlty[7]);
	mvprintw(D_LVL_7_ROW,D_L7_Z1_COL,"%3d",zn_ptr->lev_zone_nbr[7][0]);
	mvprintw(D_LVL_7_ROW,D_L7_Z2_COL,"%3d",zn_ptr->lev_zone_nbr[7][1]);
	mvprintw(D_LVL_7_ROW,D_L7_Z3_COL,"%3d",zn_ptr->lev_zone_nbr[7][2]);
	mvprintw(D_LVL_7_ROW,D_L7_Z4_COL,"%3d",zn_ptr->lev_zone_nbr[7][3]);
	mvprintw(D_LVL_7_ROW,D_L7_Z5_COL,"%3d",zn_ptr->lev_zone_nbr[7][4]);
	mvprintw(D_LVL_7_ROW,D_L7_Z6_COL,"%3d",zn_ptr->lev_zone_nbr[7][5]);
	mvprintw(D_LVL_7_ROW,D_L7_Z7_COL,"%3d",zn_ptr->lev_zone_nbr[7][6]);
	mvprintw(D_LVL_7_ROW,D_L7_Z8_COL,"%3d",zn_ptr->lev_zone_nbr[7][7]);

	mvprintw(D_LVL_8_ROW,D_L8_ALG_COL,"%c",zn_ptr->lev_alg[8]);
	mvprintw(D_LVL_8_ROW,D_L8_ETM_COL,"%5d",zn_ptr->lev_excl_time[8]);
	mvprintw(D_LVL_8_ROW,D_L8_RPN_COL,"%c",zn_ptr->lev_rej_penlty[8]);
	mvprintw(D_LVL_8_ROW,D_L8_Z1_COL,"%3d",zn_ptr->lev_zone_nbr[8][0]);
	mvprintw(D_LVL_8_ROW,D_L8_Z2_COL,"%3d",zn_ptr->lev_zone_nbr[8][1]);
	mvprintw(D_LVL_8_ROW,D_L8_Z3_COL,"%3d",zn_ptr->lev_zone_nbr[8][2]);
	mvprintw(D_LVL_8_ROW,D_L8_Z4_COL,"%3d",zn_ptr->lev_zone_nbr[8][3]);
	mvprintw(D_LVL_8_ROW,D_L8_Z5_COL,"%3d",zn_ptr->lev_zone_nbr[8][4]);
	mvprintw(D_LVL_8_ROW,D_L8_Z6_COL,"%3d",zn_ptr->lev_zone_nbr[8][5]);
	mvprintw(D_LVL_8_ROW,D_L8_Z7_COL,"%3d",zn_ptr->lev_zone_nbr[8][6]);
	mvprintw(D_LVL_8_ROW,D_L8_Z8_COL,"%3d",zn_ptr->lev_zone_nbr[8][7]);
          }

        if ( scr_nbr == 2 )
          {
	    mvprintw(D_FLEET_ROW+2, D_STAND_TELE_COL, "%s", zn_ptr->stand_telephone);
            mvprintw(D_FLEET_ROW+2, D_HTTP_TIMER_COL, "%3d", zn_ptr->stand_hold_timer);
	    mvprintw(D_FLEET_ROW+2, D_ROTI_COL, "%3d", zn_ptr->roti_time);
	    mvprintw(D_FLEET_ROW+2, D_STAND_NBR_COL, "%3d", zn_ptr->stand_nbr);
	    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL, "%3d", zn_ptr->inquiry_backup[0]);
	    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+6, "%3d", zn_ptr->inquiry_backup[1]);
	    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+12, "%3d", zn_ptr->inquiry_backup[2]);
	    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+18, "%3d", zn_ptr->inquiry_backup[3]);
	    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+24, "%3d", zn_ptr->inquiry_backup[4]);
	    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+30, "%3d", zn_ptr->inquiry_backup[5]);
	    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+36, "%3d", zn_ptr->inquiry_backup[6]);
	    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+42, "%3d", zn_ptr->inquiry_backup[7]);
	    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+48, "%3d", zn_ptr->inquiry_backup[8]);
	    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+54, "%3d", zn_ptr->inquiry_backup[9]);	    
	    
		     
          }
        
	refresh();

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_data\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_data  */

/******************************************************************************
* process_input - input data fields.
******************************************************************************/

process_input()
{
	int i;		/* loop index */
	int should_clear= 0;

#ifdef DEBUG
	sprintf(trace_str,"Entering process_input\n");
	TRACE(trace_str);
#endif
	while(1)   {
		key_desc(piu);
		intr_allowed = 1;		/* interrupts allowed */
		key_val = read_field(stdscr,zone_scr_fld,piu,read_buf,&field_entered,&should_clear); 
		attrset(0);
		intr_allowed = 0;		/* interrupts not allowed */
		if(saw_intr)  {
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_input - 1\n");
	TRACE(trace_str);
#endif
			error_flag = 1;
			return;
		}

		if (piu > NUM_FLDS)
			piu = 1;
		if (piu == 0)
			piu = NUM_FLDS;

		/* validate input data, store it in c-isam structure, and
		   adjust the field pointer */

		ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);
			if ((piu == F_DESC) && (key_val != KEY_UP) && (doing_add))	{
				temp_zn_ptr->fleet = zn_ptr->fleet;
				temp_zn_ptr->nbr = zn_ptr->nbr;
				if ((db(ZONE_FILE_ID,READ_KEYS,&zn_key1,ISEQUAL,0)) >= 0)	{
					prt_error(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_52, "Duplicated index found. Please enter again! "));
					piu = F_FLEET;
					continue;
				}
			}
		if (piu > NUM_FLDS)
			piu = 1;
		if (piu == 0)
			piu = NUM_FLDS;

		/*if((key_val == ESC) && (zn_ptr->nbr != 0)) */
		if((doing_add) &&(piu == F_ZONE) &&(key_val == ESC))
			prt_error(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_53, "You must enter zone number"));
		if((key_val == ESC) && (error_flag))
			break;
	}
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_input - 2\n");
	TRACE(trace_str);
#endif

	return;
}  /*  End of process_input()  */

/******************************************************************************
* prt_scr - read the screen and output characters to a file.
*******************************************************************************/

prt_scr(o_file_fp)
FILE *o_file_fp;
{

	int col, line, c;

#ifdef DEBUG
	sprintf(trace_str,"Entering prt_scr\n");
	TRACE(trace_str);
#endif

	col = 0;
	for ( line = 0; line < LINES ; line++)  {
		for ( col = 0; col < COLS ; col++ )  {
			move(line,col);
			c = inch();
			fputc(c,o_file_fp);	
		}
		if(c != '\n')
			fputc('\n',o_file_fp);
	}
#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_scr\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_scr() */


/******************************************************************************
* send_ipc - send a message to dispatch that a record in the zone file changed.
*******************************************************************************/

send_ipc()
{
  int qid, key = MSGKEY;
  int k = 0;
  
  if((qid = msgget(key,0666 | IPC_CREAT)) == -1)   {
    prt_error(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_54, "Zone file maint msgget failed "));
    sleep(3);
    
    cleanup();
  }  

  if ( range_low > 0 )
    {
      while ( updated_zones[k] != 0 )
	{
	  df_maint_ipc.d_prcs_id = DISP_DEST;
	  df_maint_ipc.u_prcs_id = getpid();
	  df_maint_ipc.rec_type = DFM_REQ;		/* !!!!!!!!!!!  */
	  df_maint_ipc.which_file = ZONE_FILE_ID;
	  df_maint_ipc.fleet = zn_ptr->fleet; 
	  df_maint_ipc.key = updated_zones[k];
	  df_maint_ipc.cisam_num = isrecnum;         
          if ( k % 10 == 0 )
            sleep(1);
	  if(msgsnd(qid,&df_maint_ipc,sizeof(df_maint_ipc), IPC_NOWAIT) == -1)  {
	    prt_error(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_55, "Zone file maint msgsnd failed "));
	    sleep(3);
	    cleanup();
	  }
	  k++;
	}
    }
  else
    {
      df_maint_ipc.d_prcs_id = DISP_DEST;
      df_maint_ipc.u_prcs_id = getpid();
      df_maint_ipc.rec_type = DFM_REQ;		/* !!!!!!!!!!!  */
      df_maint_ipc.which_file = ZONE_FILE_ID;
      df_maint_ipc.fleet = zn_ptr->fleet; 
      df_maint_ipc.key = zn_ptr->nbr; 
      df_maint_ipc.cisam_num = isrecnum;         

      if(msgsnd(qid,&df_maint_ipc,sizeof(df_maint_ipc), IPC_NOWAIT) == -1)  {
	prt_error(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_55, "Zone file maint msgsnd failed "));
	sleep(3);
	
	cleanup();
      }
      send_ipc2();      
    }
  
} /*  End of send_ipc()  */


send_ipc2()
{
	int qid, key = MSGKEY;
	df_maint_ipc.d_prcs_id = DISP_DEST;
	df_maint_ipc.u_prcs_id = getpid();
	df_maint_ipc.rec_type = DFM_REQ;		/* !!!!!!!!!!!  */
	df_maint_ipc.which_file = ZONEPOLY_FILE_ID;
	df_maint_ipc.fleet = zn_ptr->fleet; 
	df_maint_ipc.key = zn_ptr->nbr; 
	df_maint_ipc.cisam_num = isrecnum;         
	if((qid = msgget(key,0666 | IPC_CREAT)) == -1)   {
		prt_error(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_54, "Zone file maint msgget failed "));
		sleep(3);
#ifdef DEBUG
	sprintf(trace_str,"Leaving send_ipc - 1\n");
	TRACE(trace_str);
#endif
		cleanup();
	}

	if(msgsnd(qid,&df_maint_ipc,sizeof(df_maint_ipc), IPC_NOWAIT) == -1)  {
		prt_error(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_55, "Zone file maint msgsnd failed "));
		sleep(3);
#ifdef DEBUG
	sprintf(trace_str,"Leaving send_ipc - 2\n");
	TRACE(trace_str);
#endif
		cleanup();
	}
#ifdef DEBUG
	sprintf(trace_str,"Leaving send_ipc - 3\n");
	TRACE(trace_str);
#endif

} /*  End of send_ipc()  */

/******************************************************************************
* read_request - read users request for options.
*******************************************************************************/

char read_request()
{
	/*  The main header, the labels and the data from the first
	 *  record in the zone file have been displayed.  At this
	 *  point the user must request one of the eight options,
	 *  they are : Q, q, N, n, P, p, U, u, O, o, E, and e.
	 */

	int c;		/* For reading characters */

#ifdef DEBUG
	sprintf(trace_str,"Entering read_request\n");
	TRACE(trace_str);
#endif

	move(REQ_ROW,REQ_COL);
	refresh();

	while(c = getkey())  {
	    c = Toupper(c);
	     if (c == M_QUERY ||
		 c == M_NEXT ||
		 c == M_ADD ||
		 c == M_PREVIOUS ||
		 c == M_UPDATE ||
		 c == M_REMOVE ||
		 c == M_OUTPUT ||
                 c == M_SCREEN ||
		 c == M_EXIT)
		    break;
		else  {
		  /* 014 is Control-L for clear and redraw screen */
		  if (c == '\014')
		      wrefresh(curscr);
		  else
		    {
		      move(REQ_ROW,REQ_COL);
		      refresh();
		    }
		}
	}

#ifdef DEBUG
	sprintf(trace_str,"Leaving read_request\n");
	TRACE(trace_str);
#endif

	return(c);

} /*  End of read_request() */

/******************************************************************************
* prt_query_hdr - print the header line for the query option.
*******************************************************************************/

prt_query_hdr()
{
	int i;		/* loop index */

#ifdef DEBUG
	sprintf(trace_str,"Entering prt_qurey_hdr\n");
	TRACE(trace_str);
#endif
	clear_hdr_line();

	/* Print the Query screen header */

	mvprintw(HDR_ROW_1,HDR_COL_1,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_56, "QUERY: ESC executes. INTERRUPT aborts. "));
	refresh();

	clear_flds();

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_qurey_hdr\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_query_hdr()  */


/******************************************************************************
* prt_add_hdr - print the header line for the add option.
*******************************************************************************/

prt_add_hdr()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_add_hdr\n");
	TRACE(trace_str);
#endif

	clear_hdr_line();
	mvaddstr(HDR_ROW_1,HDR_COL_1,ADD_HDR);
	refresh();
	clear_flds();

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_add_hdr\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of prt_add_hdr() */


/******************************************************************************
* prompt_confirm - confirm that the user wants to remove this record.
*******************************************************************************/

char prompt_confirm()
{
	int c;		/* For reading characters */

#ifdef DEBUG
	sprintf(trace_str,"Entering prompt_confirm\n");
	TRACE(trace_str);
#endif

	clear_hdr_line();
	mvaddstr(DEL_ROW,DEL_COL,DELETE_HDR);
	refresh();

	while(c = getkey())  {
	    c = Toupper(c);
		if (c == YES || c == NO)
			break;
		else  {
			move(DEL_ROW,DEL_COL);
			refresh();
		}
	}

#ifdef DEBUG
	sprintf(trace_str,"Leaving prompt_confirm\n");
	TRACE(trace_str);
#endif

	return(c);

}  /*  End of prompt_confirm()  */

/******************************************************************************
* prt_hdr - print the main screen header.
*******************************************************************************/

void prt_hdr()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_hdr\n");
	TRACE(trace_str);
#endif

	clear_hdr_line();
	mvaddstr(HDR_ROW_1,HDR_COL_1,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_57, "Query Next Previous Add Update Remove Output Exit"));
	attrset(A_BOLD);
	mvaddstr(HDR_ROW_2,HDR_COL_2,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_58, "ZONE FILE MAINTENANCE"));
	attrset(0);
	refresh();
	
#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_hdr\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of prt_hdr()  */

/******************************************************************************
* prt_upd_hdr - print the header line for the update option.
*******************************************************************************/

prt_upd_hdr()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_upd_hdr\n");
	TRACE(trace_str);
#endif
	clear_hdr_line();
	mvaddstr(HDR_ROW_1,HDR_COL_1,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_59, "UPDATE: ESC executes. INTERRUPT aborts."));
	refresh();

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_upd_hdr\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_upd_hdr() */

/******************************************************************************
* prompt_fname - prompt the user for a file name to output the screen image to.
*******************************************************************************/

prompt_fname()
{

#ifdef DEBUG
	sprintf(trace_str,"Entering prompt_fname\n");
	TRACE(trace_str);
#endif
	
	clear_hdr_line();
	mvaddstr(HDR_ROW_1,HDR_COL_1,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_60, "Enter output file (default is zone_scr.out): "));
	refresh();
	getstr(o_file);
	if(!strcmp(o_file,""))
		strcpy(o_file,catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_61, "zone_scr.out"));

#ifdef DEBUG
	sprintf(trace_str,"Leaving prompt_fname\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prompt_fname()  */

/******************************************************************************
* prompt_mode - prompt the user for file mode Create or Append.
*******************************************************************************/

prompt_mode()
{
	char		answer;
	short		have_valid_response;

#ifdef DEBUG
	sprintf(trace_str,"Entering prompt_mode\n");
	TRACE(trace_str);
#endif

	have_valid_response = 0;
	while(!have_valid_response)
	  {
		clear_hdr_line();
		mvprintw(HDR_ROW_1,HDR_COL_1,catgets(ZN_MAINT_m_catd, 
				ZN_MAINT_SET, ZN_MAINT_62, 
				"Output file %s  Append or Create (default : C) ?"),o_file);

		getstr(mode);
/*
 *      default to create
 */
		if (!strlen(mode))
			mode[0] = M_CREATE;

		answer = Toupper(mode[0]);

		switch (answer)
	  	  {
			case M_APPEND:
				strcpy(mode,"a");
				have_valid_response = 1;
				break;
			case M_CREATE:
				strcpy(mode,"w");
				have_valid_response = 1;
				break;
			default:
				prt_error(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_67, 
					"Invalid choice - Please enter (A/C) append or create"));
				break;
	  	  }
	  }


	clear_err_line();


#ifdef DEBUG
	sprintf(trace_str,"Leaving prompt_mode\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prompt_mode()  */
/******************************************************************************
* prt_error - print error message.
*******************************************************************************/

prt_error(err_msg)
char *err_msg;			/* error message to be printed */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_error\n");
	TRACE(trace_str);
#endif

	clear_err_line();

	/* Set reverse video attributes */
	attrset(A_REVERSE);
	mvprintw(ERROR_ROW,ERROR_COL," %s ",err_msg);
	beep();
	refresh();
	attrset(0);
	/* Reset video attributes */

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_error\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_error()  */

prt_zn_range( zn_range_msg )
     char *zn_range_msg;
{
  attrset(A_REVERSE);  
  mvprintw( 1, 90, "%s", zn_range_msg );
  refresh();
  attrset(0);
  return;
}

clear_zn_range( )
{
  move( 1, 90 );
  clrtoeol();
  refresh();
}

/******************************************************************************
* prt_desc - print description.
*******************************************************************************/
prt_desc(err_msg) 
char  *err_msg;       /* error message to be print */
{
	mvprintw(ERROR_ROW - 1,ERROR_COL,"%s",err_msg);
	refresh();
	return;
}



/******************************************************************************
* prt_cisam_err - write the cisam error message to the trace file.
*******************************************************************************/
prt_cisam_err(err_nbr,err_msg)
int err_nbr;			/* C-isam error number associated with this problem */
char *err_msg;			/* Error message associated with this problem */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_cisam_err\n");
	TRACE(trace_str);

	sprintf(trace_str,"%s, ",err_msg);
	TRACE(trace_str);
	sprintf(trace_str,"C-isam Error Number = %d\n",iserrno);
	TRACE(trace_str);

	sprintf(trace_str,"Leaving prt_cisam_err\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_cisam_err()  */

/******************************************************************************
* prt_unix_err - write the unix error message to the trace file.
*******************************************************************************/

prt_unix_err(u_err,u_err_msg)
int u_err;			/* unix error number associated with this problem */
char *u_err_msg;		/* Error message associated with this problem */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_unix_err\n");
	TRACE(trace_str);

	sprintf(trace_str,"%s, ",u_err_msg);
	TRACE(trace_str);
	sprintf(trace_str,"Unix Error Number = %d\n",u_err);
	TRACE(trace_str);

	sprintf(trace_str,"Leaving prt_unix_err\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_unix_err()  */





/******************************************************************************
* Clear the data fields on the screen. 
*******************************************************************************/

clear_flds()
{
	int i;		/* loop index */
#ifdef DEBUG
	sprintf(trace_str,"Entering clear_flds\n");
	TRACE(trace_str);
#endif

	for(i = 1; i < NUM_FLDS; i++) 
		zone_cl_flds(i); 

#ifdef DEBUG
	sprintf(trace_str,"Leaving clear_flds\n");
	TRACE(trace_str);
#endif
}

/******************************************************************************
* clear_err_line - clear the error line on the screen.
*******************************************************************************/

clear_err_line()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering clear_err_line\n");
	TRACE(trace_str);
#endif
	move(ERROR_ROW-1,ERROR_COL);
	clrtoeol();
	refresh();  
	move(ERROR_ROW,ERROR_COL);
	clrtoeol();
	refresh();  
#ifdef DEBUG
	sprintf(trace_str,"Leaving clear_err_line\n");
	TRACE(trace_str);
#endif
}

/******************************************************************************
* clear_hdr_line - clear the header line on the screen.
*******************************************************************************/

clear_hdr_line()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering clear_hdr_line\n");
	TRACE(trace_str);
#endif
	move(HDR_ROW_1,HDR_COL_1);
	clrtoeol();
	refresh();
#ifdef DEBUG
	sprintf(trace_str,"Leaving clear_hdr_line\n");
	TRACE(trace_str);
#endif
}

/******************************************************************************
* process_intr - process the interrupt signal.
*******************************************************************************/

process_intr()
{
	/* This routine is called whcn an interrupt signal
	   is received.  A flag is set to indicate that this
	   signal was received.  The flag end_read_field is set
	   so the read_field routine will terminate. */

#ifdef DEBUG
	sprintf(trace_str,"Entering process_intr\n");
	TRACE(trace_str);
#endif
	signal(SIGINT,process_intr);
	if(intr_allowed)  {
		saw_intr = 1;
		end_read_field = 1;
	}

	if ( do_jmp )
		longjmp(jmp_env, 1 );

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_intr\n");
	TRACE(trace_str);
#endif
	return;
}

