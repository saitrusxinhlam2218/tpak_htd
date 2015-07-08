static char sz__FILE__[]="@(#)up_sub2.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  up_sub2.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:48:31
 * @(#)  Last delta: 12/2/94 at 11:24:11
 * @(#)  SCCS File: /taxi/sccs/s.up_sub2.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <isam.h>
#include <cursesX.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <setjmp.h>

#include "mads_types.h"
#include "mads_isam.h"
#include "mad_error.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "df_maint.h"
#include "ex_key.h"
#include "up_maint.h"
#include "literals.h"
#include "menu.h"
#include "path.h"
#include "tokenize.h"
#include "language.h"
#include "up_m_strdefs.h"

extern int do_jmp;              /* flag indicating if a longjmp is to be done when intr occurs  */ 
extern jmp_buf jmp_env;         /* jump environment for longjmp                                 */ 
 
extern int upper_case;
extern int city_flags;
extern int err_n_fld;
extern int ret_val;		/* return value */
extern int piu;			/* prompt in use */
extern int intr_allowed;	/* interrupts allowed */
extern int key_val;		/* value returned from the check routines */
extern char read_buf[];		/* buffer for reading fields */
int end_read_field;		/* flag, 0/1, set to end read_field function */
extern int field_entered;	/* field entered or not */
extern int saw_intr;		/* indicates an interrupt signal was entered */
extern int cur_list;		/* indicates a row is in the current list */
extern int (*check_field[])();	/* Array of pointers to routines to validate
				   the data input by the user */
extern int record_nbr;		/* C_ISAM record number */
extern char o_file[];		/* output file as specified by the user */
extern char mode[];		/* user specified Append or Create */	
extern int usr_id_flag;
extern int name_flag;
extern int doing_add;
extern char fl_names[FLEET_MAX][21];	/* Stores fleet names for fld_desc() */
extern char mf_names[MENU_MAX][34];	/* Stores menu names for fld_desc() */
extern char eg_names[12][8];		/* Stores exception group names for fld_desc() */
extern char et_names[32][31]; 		/* Stores exception type names for fld_desc() */
extern char channel_names[18][32];
int fld_desc();			/* field description routine */
extern struct scr_flds up_scr_flds[];
extern struct df_maint df_maint_ipc;	/* structure for passing information 
				   	   to dispatch  */
extern struct cisam_up *up_ptr;	/* pointer to user profile file structure */


/******************************************************************************
* prt_labels - print the field labels on the screen.
*******************************************************************************/

void prt_labels()
{
	char	 open_brace[2],  close_brace[2];

#ifdef DEBUG
	sprintf(trace_str,"Entering prt_labels\n");
	TRACE(trace_str);
#endif

	strcpy(open_brace, catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_76, "[" ));
	strcpy(close_brace, catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_77, "]" ));

	/* print the screen labels  */
	attrset(A_BOLD);
	mvaddstr(L_USER_ID_ROW,L_USER_ID_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_14, "User ID"));
	mvaddstr(L_USER_ID_ROW,L_USER_NAME_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_15, "Name"));
	mvaddstr(L_USER_ID_ROW,L_FULLNAME,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_16, "Full Name"));
	mvaddstr(L_USER_ID_ROW,L_GRT_NAME_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_17, "Greeting Name"));
	mvaddstr(L_USER_ID_ROW,L_FLT_PERM_1_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_18, "Fleet Permissions"));
	attrset(0);
	mvaddstr(L_USER_ID_ROW+1,L_USER_ID_COL+5,open_brace);
	mvaddstr(L_USER_ID_ROW+1,L_USER_ID_COL+5+MC_ID+1,close_brace);
	mvaddstr(L_USER_ID_ROW+1,L_USER_NAME_COL,open_brace);
	mvaddstr(L_USER_ID_ROW+1,L_USER_NAME_COL+MC_NAME+1,close_brace);
	mvaddstr(L_USER_ID_ROW+1,L_FULLNAME,open_brace);
	mvaddstr(L_USER_ID_ROW+1,L_FULLNAME+MC_FULLNAME+1,close_brace);
	mvaddstr(L_USER_ID_ROW+1,L_GRT_NAME_COL,open_brace);
	mvaddstr(L_USER_ID_ROW+1,L_GRT_NAME_COL+MC_GRT_NAME+1,close_brace);
	mvaddstr(L_USER_ID_ROW+1,L_FLT_PERM_1_COL,open_brace);
	mvaddstr(L_USER_ID_ROW+1,L_FLT_PERM_1_COL+MC_FL_PERM_1+1,close_brace);
	mvaddstr(L_USER_ID_ROW+1,L_FLT_PERM_2_COL,open_brace);
	mvaddstr(L_USER_ID_ROW+1,L_FLT_PERM_2_COL+MC_FL_PERM_2+1,close_brace);
	mvaddstr(L_USER_ID_ROW+1,L_FLT_PERM_3_COL,open_brace);
	mvaddstr(L_USER_ID_ROW+1,L_FLT_PERM_3_COL+MC_FL_PERM_3+1,close_brace);
	mvaddstr(L_USER_ID_ROW+1,L_FLT_PERM_4_COL,open_brace);
	mvaddstr(L_USER_ID_ROW+1,L_FLT_PERM_4_COL+MC_FL_PERM_4+1,close_brace);
	mvaddstr(L_USER_ID_ROW+1,L_FLT_PERM_5_COL,open_brace);
	mvaddstr(L_USER_ID_ROW+1,L_FLT_PERM_5_COL+MC_FL_PERM_5+1,close_brace);
	mvaddstr(L_USER_ID_ROW+1,L_FLT_PERM_6_COL,open_brace);
	mvaddstr(L_USER_ID_ROW+1,L_FLT_PERM_6_COL+MC_FL_PERM_6+1,close_brace);
	mvaddstr(L_USER_ID_ROW+1,L_FLT_PERM_7_COL,open_brace);
	mvaddstr(L_USER_ID_ROW+1,L_FLT_PERM_7_COL+MC_FL_PERM_7+1,close_brace);
	mvaddstr(L_USER_ID_ROW+1,L_FLT_PERM_8_COL,open_brace);
	mvaddstr(L_USER_ID_ROW+1,L_FLT_PERM_8_COL+MC_FL_PERM_8+1,close_brace);
	attrset(A_BOLD);
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_HDR_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_19, "Menu Permissions"));
	attrset(0);
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_1_COL,"1");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_2_COL,"2");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_3_COL,"3");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_4_COL,"4");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_5_COL,"5");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_6_COL,"6");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_7_COL,"7");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_8_COL,"8");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_9_COL,"9");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_11_COL,"11");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_12_COL,"12");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_13_COL,"13");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_14_COL,"14");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_15_COL,"15");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_16_COL,"16");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_17_COL,"17");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_18_COL,"18");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_19_COL,"19");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_21_COL,"21");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_22_COL,"22");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_23_COL,"23");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_24_COL,"24");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_25_COL,"25");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_26_COL,"26");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_27_COL,"27");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_28_COL,"28");
	mvaddstr(L_M_PERM_1_ROW,L_M_PERM_29_COL,"29");
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_1_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_1_COL+MC_M_PERM_1+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_2_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_2_COL+MC_M_PERM_2+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_3_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_3_COL+MC_M_PERM_3+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_4_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_4_COL+MC_M_PERM_4+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_5_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_5_COL+MC_M_PERM_5+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_6_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_6_COL+MC_M_PERM_6+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_7_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_7_COL+MC_M_PERM_7+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_8_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_8_COL+MC_M_PERM_8+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_9_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_9_COL+MC_M_PERM_9+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_11_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_11_COL+MC_M_PERM_11+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_12_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_12_COL+MC_M_PERM_12+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_13_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_13_COL+MC_M_PERM_13+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_14_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_14_COL+MC_M_PERM_14+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_15_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_15_COL+MC_M_PERM_15+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_16_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_16_COL+MC_M_PERM_16+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_17_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_17_COL+MC_M_PERM_17+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_18_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_18_COL+MC_M_PERM_18+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_19_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_19_COL+MC_M_PERM_19+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_21_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_21_COL+MC_M_PERM_21+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_22_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_22_COL+MC_M_PERM_22+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_23_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_23_COL+MC_M_PERM_23+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_24_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_24_COL+MC_M_PERM_24+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_25_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_25_COL+MC_M_PERM_25+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_26_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_26_COL+MC_M_PERM_26+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_27_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_27_COL+MC_M_PERM_27+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_28_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_28_COL+MC_M_PERM_28+1,close_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_29_COL,open_brace);
	mvaddstr(L_M_PERM_1_ROW+1,L_M_PERM_29_COL+MC_M_PERM_29+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_31_COL,"31");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_32_COL,"32");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_33_COL,"33");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_34_COL,"34");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_35_COL,"35");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_36_COL,"36");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_37_COL,"37");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_38_COL,"38");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_39_COL,"39");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_61_COL,"61");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_62_COL,"62");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_63_COL,"63");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_64_COL,"64");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_65_COL,"65");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_66_COL,"66");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_67_COL,"67");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_68_COL,"68");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_69_COL,"69");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_281_COL,"281");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_282_COL,"282");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_283_COL,"283");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_284_COL,"284");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_285_COL,"285");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_286_COL,"286");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_287_COL,"287");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_288_COL,"288");
	mvaddstr(L_M_PERM_3_ROW,L_M_PERM_289_COL,"289");
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_29_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_29_COL+MC_M_PERM_29+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_31_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_31_COL+MC_M_PERM_31+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_32_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_32_COL+MC_M_PERM_32+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_33_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_33_COL+MC_M_PERM_33+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_34_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_34_COL+MC_M_PERM_34+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_35_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_35_COL+MC_M_PERM_35+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_36_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_36_COL+MC_M_PERM_36+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_37_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_37_COL+MC_M_PERM_37+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_38_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_38_COL+MC_M_PERM_38+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_39_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_39_COL+MC_M_PERM_39+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_61_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_61_COL+MC_M_PERM_61+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_62_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_62_COL+MC_M_PERM_62+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_63_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_63_COL+MC_M_PERM_63+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_64_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_64_COL+MC_M_PERM_64+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_65_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_65_COL+MC_M_PERM_65+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_66_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_66_COL+MC_M_PERM_66+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_67_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_67_COL+MC_M_PERM_67+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_68_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_68_COL+MC_M_PERM_68+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_69_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_69_COL+MC_M_PERM_69+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_281_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_281_COL+MC_M_PERM_281+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_282_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_282_COL+MC_M_PERM_282+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_283_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_283_COL+MC_M_PERM_283+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_284_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_284_COL+MC_M_PERM_284+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_285_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_285_COL+MC_M_PERM_285+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_286_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_286_COL+MC_M_PERM_286+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_287_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_287_COL+MC_M_PERM_287+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_288_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_288_COL+MC_M_PERM_288+1,close_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_289_COL,open_brace);
	mvaddstr(L_M_PERM_3_ROW+1,L_M_PERM_289_COL+MC_M_PERM_289+1,close_brace);
	attrset(A_BOLD);
	mvaddstr(L_DF_FLEET_ROW,L_DEFAULTS_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_20, "Default"));
	mvaddstr(L_DF_FLEET_ROW,L_DF_FLEET_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_21, "Fleet"));
	mvaddstr(L_DF_FLEET_ROW,L_DF_SCR_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_22, "Screen"));
	mvaddstr(L_DF_FLEET_ROW,L_DF_SRCH_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_23, "Search"));
	mvaddstr(L_DF_FLEET_ROW,L_DF_CL_TYP_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_25, "Call Type"));
	mvaddstr(L_DF_FLEET_ROW,L_DF_CITY_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_26, "City"));
	mvaddstr(L_DF_FLEET_ROW,L_HI_CL_PRI_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_27, "Highest Pri"));
	mvaddstr(L_DF_FLEET_ROW,L_CN_ZN_CLS_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_28, "Can Zone Calls"));
	mvaddstr(L_DF_FLEET_ROW,L_LG_FILE_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_29, "Logo File"));
	mvaddstr(L_AUTO_CALL_MSG_ROW,L_AUTO_CALL_MSG_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_79, "Auto Free Text" ));
	attrset(0);
	mvaddstr(L_DF_FLEET_ROW+1,L_DF_FLEET_COL,open_brace);
	mvaddstr(L_DF_FLEET_ROW+1,L_DF_FLEET_COL+MC_DF_FLEET+1,close_brace);
	mvaddstr(L_DF_FLEET_ROW+1,L_DF_SCR_COL,open_brace);
	mvaddstr(L_DF_FLEET_ROW+1,L_DF_SCR_COL+MC_DF_SCR+1,close_brace);
	mvaddstr(L_DF_FLEET_ROW+1,L_DF_SRCH_COL,open_brace);
	mvaddstr(L_DF_FLEET_ROW+1,L_DF_SRCH_COL+MC_DF_SRCH+1,close_brace);
	mvaddstr(L_DF_FLEET_ROW+1,L_DF_CL_TYP_COL,open_brace);
	mvaddstr(L_DF_FLEET_ROW+1,L_DF_CL_TYP_COL+MC_DF_CL_TYP+1,close_brace);
	mvaddstr(L_DF_FLEET_ROW+1,L_DF_CITY_COL,open_brace);
	mvaddstr(L_DF_FLEET_ROW+1,L_DF_CITY_COL+MC_DF_CITY+1,close_brace);
	mvaddstr(L_DF_FLEET_ROW+1,L_HI_CL_PRI_COL,open_brace);
	mvaddstr(L_DF_FLEET_ROW+1,L_HI_CL_PRI_COL+MC_DF_HI_CL_PRI+1,close_brace);
	mvaddstr(L_DF_FLEET_ROW+1,L_CN_ZN_CLS_COL,open_brace);
	mvaddstr(L_DF_FLEET_ROW+1,L_CN_ZN_CLS_COL+MC_CN_ZONE_CLS+1,close_brace);
	mvaddstr(L_DF_FLEET_ROW+1,L_LG_FILE_COL,open_brace);
	mvaddstr(L_DF_FLEET_ROW+1,L_LG_FILE_COL+MC_LG_FILE+1,close_brace);
	mvaddstr(L_AUTO_CALL_MSG_ROW+1,L_AUTO_CALL_MSG_COL,open_brace);
	mvaddstr(L_AUTO_CALL_MSG_ROW+1,L_AUTO_CALL_MSG_COL+MC_AUTO_CALL_MSG+1,close_brace);
	attrset(A_BOLD);
	mvaddstr(L_SPR_ZONES_1_ROW,L_SPR_ZONES_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_30, "Superzones :"));
	attrset(0);
	mvaddstr(L_SPR_ZONES_2_ROW,L_S_ZONE_1_COL,"1");
	mvaddstr(L_SPR_ZONES_2_ROW,L_S_ZONE_2_COL,"2");
	mvaddstr(L_SPR_ZONES_2_ROW,L_S_ZONE_3_COL,"3");
	mvaddstr(L_SPR_ZONES_2_ROW,L_S_ZONE_4_COL,"4");
	mvaddstr(L_SPR_ZONES_2_ROW,L_S_ZONE_5_COL,"5");
	mvaddstr(L_SPR_ZONES_2_ROW,L_S_ZONE_6_COL,"6");
	mvaddstr(L_SPR_ZONES_2_ROW,L_S_ZONE_7_COL,"7");
	mvaddstr(L_SPR_ZONES_2_ROW,L_S_ZONE_8_COL,"8");
	mvaddstr(L_SPR_ZONES_2_ROW,L_S_ZONE_9_COL,"9");
	mvaddstr(L_SPR_ZONES_2_ROW,L_S_ZONE_10_COL,"10");
	mvaddstr(L_SPR_ZONES_2_ROW,L_S_ZONE_11_COL,"11");
	mvaddstr(L_SPR_ZONES_2_ROW,L_S_ZONE_12_COL,"12");
	mvaddstr(L_SPR_ZONES_2_ROW,L_S_ZONE_13_COL,"13");
	mvaddstr(L_SPR_ZONES_2_ROW,L_S_ZONE_14_COL,"14");
	mvaddstr(L_SPR_ZONES_2_ROW,L_S_ZONE_15_COL,"15");
	mvaddstr(L_SPR_ZONES_2_ROW,L_S_ZONE_16_COL,"16");
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_1_COL,open_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_1_COL+MC_SPZ_1+1,close_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_2_COL,open_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_2_COL+MC_SPZ_2+1,close_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_3_COL,open_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_3_COL+MC_SPZ_3+1,close_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_4_COL,open_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_4_COL+MC_SPZ_4+1,close_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_5_COL,open_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_5_COL+MC_SPZ_5+1,close_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_6_COL,open_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_6_COL+MC_SPZ_6+1,close_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_7_COL,open_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_7_COL+MC_SPZ_7+1,close_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_8_COL,open_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_8_COL+MC_SPZ_8+1,close_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_9_COL,open_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_9_COL+MC_SPZ_9+1,close_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_10_COL,open_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_10_COL+MC_SPZ_10+1,close_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_11_COL,open_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_11_COL+MC_SPZ_11+1,close_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_12_COL,open_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_12_COL+MC_SPZ_12+1,close_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_13_COL,open_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_13_COL+MC_SPZ_13+1,close_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_14_COL,open_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_14_COL+MC_SPZ_14+1,close_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_15_COL,open_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_15_COL+MC_SPZ_15+1,close_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_16_COL,open_brace);
	mvaddstr(L_SPR_ZONES_2_ROW+1,L_S_ZONE_16_COL+MC_SPZ_16+1,close_brace);
	attrset(A_BOLD);
	mvaddstr(L_GROUPS_ROW,L_GROUPS_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_31, "Exc Groups:"));
	attrset(0);
	mvaddstr(L_GROUPS_ROW,L_GRP_1_COL,"1");
	mvaddstr(L_GROUPS_ROW,L_GRP_2_COL,"2");
	mvaddstr(L_GROUPS_ROW,L_GRP_3_COL,"3");
	mvaddstr(L_GROUPS_ROW,L_GRP_4_COL,"4");
	mvaddstr(L_GROUPS_ROW,L_GRP_5_COL,"5");
	mvaddstr(L_GROUPS_ROW,L_GRP_6_COL,"6");
	mvaddstr(L_GROUPS_ROW,L_GRP_7_COL,"7");
	mvaddstr(L_GROUPS_ROW,L_GRP_8_COL,"8");
	mvaddstr(L_GROUPS_ROW,L_GRP_9_COL,"9");
	mvaddstr(L_GROUPS_ROW,L_GRP_10_COL,"10");
	mvaddstr(L_GROUPS_ROW,L_GRP_11_COL,"11");
	mvaddstr(L_GROUPS_ROW,L_GRP_12_COL,"12");
	mvaddstr(L_GROUPS_ROW+1,L_GRP_1_COL,open_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_1_COL+MC_EX_GP_1+1,close_brace); 
	mvaddstr(L_GROUPS_ROW+1,L_GRP_2_COL,open_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_2_COL+MC_EX_GP_2+1,close_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_3_COL,open_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_3_COL+MC_EX_GP_3+1,close_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_4_COL,open_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_4_COL+MC_EX_GP_4+1,close_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_5_COL,open_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_5_COL+MC_EX_GP_5+1,close_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_6_COL,open_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_6_COL+MC_EX_GP_6+1,close_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_7_COL,open_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_7_COL+MC_EX_GP_7+1,close_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_8_COL,open_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_8_COL+MC_EX_GP_8+1,close_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_9_COL,open_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_9_COL+MC_EX_GP_9+1,close_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_10_COL,open_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_10_COL+MC_EX_GP_10+1,close_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_11_COL,open_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_11_COL+MC_EX_GP_11+1,close_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_12_COL,open_brace);
	mvaddstr(L_GROUPS_ROW+1,L_GRP_12_COL+MC_EX_GP_12+1,close_brace);
	attrset(A_BOLD);
	mvaddstr(L_TYPE_ROW,L_TYPE_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_32, "Exc"));
	attrset(0);
	mvaddstr(L_TYPE_ROW,L_TYP_1_COL,"1");
	mvaddstr(L_TYPE_ROW,L_TYP_2_COL,"2");
	mvaddstr(L_TYPE_ROW,L_TYP_3_COL,"3");
	mvaddstr(L_TYPE_ROW,L_TYP_4_COL,"4");
	mvaddstr(L_TYPE_ROW,L_TYP_5_COL,"5");
	mvaddstr(L_TYPE_ROW,L_TYP_6_COL,"6");
	mvaddstr(L_TYPE_ROW,L_TYP_7_COL,"7");
	mvaddstr(L_TYPE_ROW,L_TYP_8_COL,"8");
	mvaddstr(L_TYPE_ROW,L_TYP_9_COL,"9");
	mvaddstr(L_TYPE_ROW,L_TYP_10_COL,"10");
	mvaddstr(L_TYPE_ROW,L_TYP_11_COL,"11");
	mvaddstr(L_TYPE_ROW,L_TYP_12_COL,"12");
	mvaddstr(L_TYPE_ROW,L_TYP_13_COL,"13");
	mvaddstr(L_TYPE_ROW,L_TYP_14_COL,"14");
	mvaddstr(L_TYPE_ROW,L_TYP_15_COL,"15");
	mvaddstr(L_TYPE_ROW,L_TYP_16_COL,"16");
	mvaddstr(L_TYPE_ROW,L_TYP_17_COL,"17");
	mvaddstr(L_TYPE_ROW,L_TYP_18_COL,"18");
	mvaddstr(L_TYPE_ROW,L_TYP_19_COL,"19");
	mvaddstr(L_TYPE_ROW,L_TYP_20_COL,"20");
	mvaddstr(L_TYPE_ROW,L_TYP_21_COL,"21");
	mvaddstr(L_TYPE_ROW,L_TYP_22_COL,"22");
	mvaddstr(L_TYPE_ROW,L_TYP_23_COL,"23");
	mvaddstr(L_TYPE_ROW,L_TYP_24_COL,"24");
	mvaddstr(L_TYPE_ROW,L_TYP_25_COL,"25");
	mvaddstr(L_TYPE_ROW,L_TYP_26_COL,"26");
	mvaddstr(L_TYPE_ROW,L_TYP_27_COL,"27");
	mvaddstr(L_TYPE_ROW,L_TYP_28_COL,"28");
	mvaddstr(L_TYPE_ROW,L_TYP_29_COL,"29");
	mvaddstr(L_TYPE_ROW,L_TYP_30_COL,"30");
	mvaddstr(L_TYPE_ROW,L_TYP_31_COL,"31");
	mvaddstr(L_TYPE_ROW,L_TYP_32_COL,"32");
	attrset(A_BOLD);
	mvaddstr(L_TYP_2_ROW,L_TYP_COL,catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_33, "Type"));
	attrset(0);
	mvaddstr(L_TYP_2_ROW,L_TYP_1_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_1_COL+MC_EX_TYP_1+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_2_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_2_COL+MC_EX_TYP_2+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_3_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_3_COL+MC_EX_TYP_3+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_4_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_4_COL+MC_EX_TYP_4+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_5_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_5_COL+MC_EX_TYP_5+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_6_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_6_COL+MC_EX_TYP_6+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_7_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_7_COL+MC_EX_TYP_7+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_8_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_8_COL+MC_EX_TYP_8+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_9_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_9_COL+MC_EX_TYP_9+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_10_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_10_COL+MC_EX_TYP_10+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_11_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_11_COL+MC_EX_TYP_11+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_12_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_12_COL+MC_EX_TYP_12+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_13_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_13_COL+MC_EX_TYP_13+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_14_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_14_COL+MC_EX_TYP_14+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_15_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_15_COL+MC_EX_TYP_15+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_16_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_16_COL+MC_EX_TYP_16+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_17_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_17_COL+MC_EX_TYP_17+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_18_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_18_COL+MC_EX_TYP_18+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_19_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_19_COL+MC_EX_TYP_19+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_20_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_20_COL+MC_EX_TYP_20+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_21_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_21_COL+MC_EX_TYP_21+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_22_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_22_COL+MC_EX_TYP_22+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_23_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_23_COL+MC_EX_TYP_23+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_24_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_24_COL+MC_EX_TYP_24+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_25_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_25_COL+MC_EX_TYP_25+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_26_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_26_COL+MC_EX_TYP_26+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_27_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_27_COL+MC_EX_TYP_27+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_28_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_28_COL+MC_EX_TYP_28+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_29_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_29_COL+MC_EX_TYP_29+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_30_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_30_COL+MC_EX_TYP_30+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_31_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_31_COL+MC_EX_TYP_31+1,close_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_32_COL,open_brace);
	mvaddstr(L_TYP_2_ROW,L_TYP_32_COL+MC_EX_TYP_32+1,close_brace);
#ifdef NAMERICA
	  {
	    attrset(A_BOLD);
	    mvaddstr(L_HIA_ROW, 0, "Chn");
	    attrset(0);
	    mvaddstr(L_HIA_ROW, L_HIA_1_COL, open_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_1_COL+MC_HIA_1+1,close_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_2_COL, open_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_2_COL+MC_HIA_2+1,close_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_3_COL, open_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_3_COL+MC_HIA_3+1,close_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_4_COL, open_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_4_COL+MC_HIA_4+1,close_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_5_COL,open_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_5_COL+MC_HIA_5+1,close_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_6_COL,open_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_6_COL+MC_HIA_6+1,close_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_7_COL,open_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_7_COL+MC_HIA_7+1,close_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_8_COL,open_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_8_COL+MC_HIA_8+1,close_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_9_COL,open_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_9_COL+MC_HIA_9+1,close_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_10_COL, open_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_10_COL+MC_HIA_10+1,close_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_11_COL, open_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_11_COL+MC_HIA_11+1,close_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_12_COL, open_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_12_COL+MC_HIA_12+1,close_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_13_COL, open_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_13_COL+MC_HIA_13+1,close_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_14_COL, open_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_14_COL+MC_HIA_14+1,close_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_15_COL,open_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_15_COL+MC_HIA_15+1,close_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_16_COL,open_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_16_COL+MC_HIA_16+1,close_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_17_COL,open_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_17_COL+MC_HIA_17+1,close_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_18_COL,open_brace);
	    mvaddstr(L_HIA_ROW, L_HIA_18_COL+MC_HIA_18+1,close_brace);
	  }
#endif
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

/*	clear_flds();     */

	/* Fill in the data items on the screen */

	mvprintw(D_USER_ID_ROW,D_USER_ID_COL,"%4d",up_ptr->user_id);
	mvprintw(D_USER_ID_ROW,D_USER_NAME_COL,"%s",up_ptr->user_name);
	mvprintw(D_USER_ID_ROW,D_FULLNAME,"%s",up_ptr->fullname);
	mvprintw(D_USER_ID_ROW,D_GRT_NAME_COL,"%s",up_ptr->greeting_name);
	mvprintw(D_USER_ID_ROW,D_FLT_PERM_1_COL,"%c",up_ptr->fleet_perms[0]);
	mvprintw(D_USER_ID_ROW,D_FLT_PERM_2_COL,"%c",up_ptr->fleet_perms[1]);
	mvprintw(D_USER_ID_ROW,D_FLT_PERM_3_COL,"%c",up_ptr->fleet_perms[2]);
	mvprintw(D_USER_ID_ROW,D_FLT_PERM_4_COL,"%c",up_ptr->fleet_perms[3]);
	mvprintw(D_USER_ID_ROW,D_FLT_PERM_5_COL,"%c",up_ptr->fleet_perms[4]);
	mvprintw(D_USER_ID_ROW,D_FLT_PERM_6_COL,"%c",up_ptr->fleet_perms[5]);
	mvprintw(D_USER_ID_ROW,D_FLT_PERM_7_COL,"%c",up_ptr->fleet_perms[6]);
	mvprintw(D_USER_ID_ROW,D_FLT_PERM_8_COL,"%c",up_ptr->fleet_perms[7]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_1_COL,"%c",up_ptr->menu_perms[1]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_2_COL,"%c",up_ptr->menu_perms[2]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_3_COL,"%c",up_ptr->menu_perms[3]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_4_COL,"%c",up_ptr->menu_perms[4]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_5_COL,"%c",up_ptr->menu_perms[5]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_6_COL,"%c",up_ptr->menu_perms[6]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_7_COL,"%c", NO);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_8_COL,"%c", NO);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_9_COL,"%c", NO);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_11_COL,"%c",up_ptr->menu_perms[7]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_12_COL,"%c",up_ptr->menu_perms[8]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_13_COL,"%c",up_ptr->menu_perms[9]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_14_COL,"%c",up_ptr->menu_perms[10]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_15_COL,"%c",up_ptr->menu_perms[11]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_16_COL,"%c",up_ptr->menu_perms[12]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_17_COL,"%c",up_ptr->menu_perms[13]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_18_COL,"%c",up_ptr->menu_perms[14]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_19_COL,"%c",up_ptr->menu_perms[15]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_21_COL,"%c",up_ptr->menu_perms[16]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_22_COL,"%c",up_ptr->menu_perms[17]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_23_COL,"%c",up_ptr->menu_perms[18]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_24_COL,"%c",up_ptr->menu_perms[19]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_25_COL,"%c",up_ptr->menu_perms[20]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_26_COL,"%c",up_ptr->menu_perms[21]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_27_COL,"%c",up_ptr->menu_perms[22]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_28_COL,"%c",up_ptr->menu_perms[23]);
	mvprintw(D_M_PERM_1_ROW,D_M_PERM_29_COL,"%c",up_ptr->menu_perms[24]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_31_COL,"%c",up_ptr->menu_perms[25]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_32_COL,"%c",up_ptr->menu_perms[26]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_33_COL,"%c",up_ptr->menu_perms[27]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_34_COL,"%c",up_ptr->menu_perms[28]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_35_COL,"%c",up_ptr->menu_perms[29]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_36_COL,"%c",up_ptr->menu_perms[30]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_37_COL,"%c",up_ptr->menu_perms[31]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_38_COL,"%c",up_ptr->menu_perms[32]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_39_COL,"%c",up_ptr->menu_perms[33]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_61_COL,"%c",up_ptr->menu_perms[34]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_62_COL,"%c",up_ptr->menu_perms[35]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_63_COL,"%c",up_ptr->menu_perms[36]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_64_COL,"%c",up_ptr->menu_perms[37]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_65_COL,"%c",up_ptr->menu_perms[38]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_66_COL,"%c",up_ptr->menu_perms[39]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_67_COL,"%c",up_ptr->menu_perms[40]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_68_COL,"%c",up_ptr->menu_perms[41]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_69_COL,"%c",up_ptr->menu_perms[42]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_281_COL,"%c",up_ptr->menu_perms[43]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_282_COL,"%c",up_ptr->menu_perms[44]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_283_COL,"%c",up_ptr->menu_perms[45]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_284_COL,"%c",up_ptr->menu_perms[46]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_285_COL,"%c",up_ptr->menu_perms[47]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_286_COL,"%c",up_ptr->menu_perms[48]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_287_COL,"%c",up_ptr->menu_perms[49]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_288_COL,"%c",up_ptr->menu_perms[50]);
	mvprintw(D_M_PERM_2_ROW,D_M_PERM_289_COL,"%c",up_ptr->menu_perms[51]);
	mvprintw(D_DF_FLEET_ROW,D_DF_FLEET_COL,"%c",up_ptr->default_fleet);
	mvprintw(D_DF_FLEET_ROW,D_DF_SCR_COL,"%3d",up_ptr->default_screen);
	mvprintw(D_DF_FLEET_ROW,D_DF_SRCH_COL,"%3d",up_ptr->default_search);
/* Position is not used anymore 
	mvprintw(D_DF_FLEET_ROW,D_DF_POS_COL,"%3d",up_ptr->default_pos);
*/
	mvprintw(D_DF_FLEET_ROW,D_DF_CL_TYP_COL,"%c",up_ptr->default_call_tp);
	mvprintw(D_DF_FLEET_ROW,D_DF_CITY_COL,"%s",up_ptr->default_city);
	mvprintw(D_DF_FLEET_ROW,D_DF_HI_CL_PRI_COL,"%2d",up_ptr->high_call_pri);
	mvprintw(D_DF_FLEET_ROW,D_CN_ZN_CLS_COL,"%c",up_ptr->can_zone_calls);
	mvprintw(D_DF_FLEET_ROW,D_LG_FILE_COL,"%s",up_ptr->logo_file);
	mvprintw(D_AUTO_CALL_MSG_ROW,D_AUTO_CALL_MSG_COL,"%c",up_ptr->auto_free_text);
	mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_1_COL,"%5d",up_ptr->superzone_1);
	mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_2_COL,"%5d",up_ptr->superzone_2);
	mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_3_COL,"%5d",up_ptr->superzone_3);
	mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_4_COL,"%5d",up_ptr->superzone_4);
	mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_5_COL,"%5d",up_ptr->superzone_5);
	mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_6_COL,"%5d",up_ptr->superzone_6);
	mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_7_COL,"%5d",up_ptr->superzone_7);
	mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_8_COL,"%5d",up_ptr->superzone_8);
	mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_9_COL,"%5d",up_ptr->superzone_9);
	mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_10_COL,"%5d",up_ptr->superzone_10);
	mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_11_COL,"%5d",up_ptr->superzone_11);
	mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_12_COL,"%5d",up_ptr->superzone_12);
	mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_13_COL,"%5d",up_ptr->superzone_13);
	mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_14_COL,"%5d",up_ptr->superzone_14);
	mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_15_COL,"%5d",up_ptr->superzone_15);
	mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_16_COL,"%5d",up_ptr->superzone_16);
	mvprintw(D_GROUPS_ROW,D_GRP_1_COL,"%c",up_ptr->group_1);
	mvprintw(D_GROUPS_ROW,D_GRP_2_COL,"%c",up_ptr->group_2);
	mvprintw(D_GROUPS_ROW,D_GRP_3_COL,"%c",up_ptr->group_3);
	mvprintw(D_GROUPS_ROW,D_GRP_4_COL,"%c",up_ptr->group_4);
	mvprintw(D_GROUPS_ROW,D_GRP_5_COL,"%c",up_ptr->group_5);
	mvprintw(D_GROUPS_ROW,D_GRP_6_COL,"%c",up_ptr->group_6);
	mvprintw(D_GROUPS_ROW,D_GRP_7_COL,"%c",up_ptr->group_7);
	mvprintw(D_GROUPS_ROW,D_GRP_8_COL,"%c",up_ptr->group_8);
	mvprintw(D_GROUPS_ROW,D_GRP_9_COL,"%c",up_ptr->group_9);
	mvprintw(D_GROUPS_ROW,D_GRP_10_COL,"%c",up_ptr->group_10);
	mvprintw(D_GROUPS_ROW,D_GRP_11_COL,"%c",up_ptr->group_11);
	mvprintw(D_GROUPS_ROW,D_GRP_12_COL,"%c",up_ptr->group_12);
	mvprintw(D_TYPE_ROW,D_TYP_1_COL,"%c",up_ptr->type_1);
	mvprintw(D_TYPE_ROW,D_TYP_2_COL,"%c",up_ptr->type_2);
	mvprintw(D_TYPE_ROW,D_TYP_3_COL,"%c",up_ptr->type_3);
	mvprintw(D_TYPE_ROW,D_TYP_4_COL,"%c",up_ptr->type_4);
	mvprintw(D_TYPE_ROW,D_TYP_5_COL,"%c",up_ptr->type_5);
	mvprintw(D_TYPE_ROW,D_TYP_6_COL,"%c",up_ptr->type_6);
	mvprintw(D_TYPE_ROW,D_TYP_7_COL,"%c",up_ptr->type_7);
	mvprintw(D_TYPE_ROW,D_TYP_8_COL,"%c",up_ptr->type_8);
	mvprintw(D_TYPE_ROW,D_TYP_9_COL,"%c",up_ptr->type_9);
	mvprintw(D_TYPE_ROW,D_TYP_10_COL,"%c",up_ptr->type_10);
	mvprintw(D_TYPE_ROW,D_TYP_11_COL,"%c",up_ptr->type_11);
	mvprintw(D_TYPE_ROW,D_TYP_12_COL,"%c",up_ptr->type_12);
	mvprintw(D_TYPE_ROW,D_TYP_13_COL,"%c",up_ptr->type_13);
	mvprintw(D_TYPE_ROW,D_TYP_14_COL,"%c",up_ptr->type_14);
	mvprintw(D_TYPE_ROW,D_TYP_15_COL,"%c",up_ptr->type_15);
	mvprintw(D_TYPE_ROW,D_TYP_16_COL,"%c",up_ptr->type_16);
	mvprintw(D_TYPE_ROW,D_TYP_17_COL,"%c",up_ptr->type_17);
	mvprintw(D_TYPE_ROW,D_TYP_18_COL,"%c",up_ptr->type_18);
	mvprintw(D_TYPE_ROW,D_TYP_19_COL,"%c",up_ptr->type_19);
	mvprintw(D_TYPE_ROW,D_TYP_20_COL,"%c",up_ptr->type_20);
	mvprintw(D_TYPE_ROW,D_TYP_21_COL,"%c",up_ptr->type_21);
	mvprintw(D_TYPE_ROW,D_TYP_22_COL,"%c",up_ptr->type_22);
	mvprintw(D_TYPE_ROW,D_TYP_23_COL,"%c",up_ptr->type_23);
	mvprintw(D_TYPE_ROW,D_TYP_24_COL,"%c",up_ptr->type_24);
	mvprintw(D_TYPE_ROW,D_TYP_25_COL,"%c",up_ptr->type_25);
	mvprintw(D_TYPE_ROW,D_TYP_26_COL,"%c",up_ptr->type_26);
	mvprintw(D_TYPE_ROW,D_TYP_27_COL,"%c",up_ptr->type_27);
	mvprintw(D_TYPE_ROW,D_TYP_28_COL,"%c",up_ptr->type_28);
	mvprintw(D_TYPE_ROW,D_TYP_29_COL,"%c",up_ptr->type_29);
	mvprintw(D_TYPE_ROW,D_TYP_30_COL,"%c",up_ptr->type_30);
	mvprintw(D_TYPE_ROW,D_TYP_31_COL,"%c",up_ptr->type_31);
	mvprintw(D_TYPE_ROW,D_TYP_32_COL,"%c",up_ptr->type_32);
#ifdef NAMERICA
	  {
	    mvprintw(D_HIA_ROW,D_HIA_1_COL,"%c",up_ptr->hia_1);
	    mvprintw(D_HIA_ROW,D_HIA_2_COL,"%c",up_ptr->hia_2);
	    mvprintw(D_HIA_ROW,D_HIA_3_COL,"%c",up_ptr->hia_3);
	    mvprintw(D_HIA_ROW,D_HIA_4_COL,"%c",up_ptr->hia_4);
	    mvprintw(D_HIA_ROW,D_HIA_5_COL,"%c",up_ptr->hia_5);
	    mvprintw(D_HIA_ROW,D_HIA_6_COL,"%c",up_ptr->hia_6);
	    mvprintw(D_HIA_ROW,D_HIA_7_COL,"%c",up_ptr->hia_7);
	    mvprintw(D_HIA_ROW,D_HIA_8_COL,"%c",up_ptr->hia_8);
	    mvprintw(D_HIA_ROW,D_HIA_9_COL,"%c",up_ptr->hia_9);
	    mvprintw(D_HIA_ROW,D_HIA_10_COL,"%c",up_ptr->hia_10);
	    mvprintw(D_HIA_ROW,D_HIA_11_COL,"%c",up_ptr->hia_11);
	    mvprintw(D_HIA_ROW,D_HIA_12_COL,"%c",up_ptr->hia_12);
	    mvprintw(D_HIA_ROW,D_HIA_13_COL,"%c",up_ptr->hia_13);
	    mvprintw(D_HIA_ROW,D_HIA_14_COL,"%c",up_ptr->hia_14);
	    mvprintw(D_HIA_ROW,D_HIA_15_COL,"%c",up_ptr->hia_15);
	    mvprintw(D_HIA_ROW,D_HIA_16_COL,"%c",up_ptr->hia_16);
	    mvprintw(D_HIA_ROW,D_HIA_17_COL,"%c",up_ptr->hia_17);
	    mvprintw(D_HIA_ROW,D_HIA_18_COL,"%c",up_ptr->hia_18);
	  }
#endif

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
	int should_clear = 0;

#ifdef DEBUG
	sprintf(trace_str,"Entering process_input\n");
	TRACE(trace_str);
#endif

	while(1)   {
		fld_desc(piu);
		intr_allowed = 1;		/* interrupts allowed */
		if (piu == 2)
			upper_case = 1;
		else
			upper_case = 0;
		norm_to_rev(stdscr,up_scr_flds,piu);
		key_val = read_field(stdscr,up_scr_flds,piu,read_buf,&field_entered,&should_clear); 
		revund_to_norm(stdscr,up_scr_flds,piu);
		attrset(0);
		intr_allowed = 0;		/* interrupts not allowed */
		if(saw_intr)  {
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_input - 1\n");
	TRACE(trace_str);
#endif
			return;
		}

		/* validate input data, store it in c-isam structure, and
		   adjust the field pointer */

		ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);
		if((ret_val == NOT_VALID) && (!name_flag))
			continue;
		if ((doing_add) && (!name_flag))	{
			prt_error(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_34, "You must enter value"));
			piu = 2;
			continue;
		}
		if (key_val == ESC)	{
			if (doing_add)	{
				if((!err_n_fld)&&(city_flags))
					break;
				else	{ 
					prt_error(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_35, "Must enter city code"));
					continue;
				}
			}
			if(!err_n_fld)
				break;
		}
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
* send_ipc - send a message to dispatch that a record in the user profile file changed.
*******************************************************************************/

send_ipc()
{
	int qid, key = MSGKEY;

#ifdef DEBUG
	sprintf(trace_str,"Entering send_ipc\n");
	TRACE(trace_str);
#endif

	df_maint_ipc.d_prcs_id = DISP_DEST;
	df_maint_ipc.u_prcs_id = getpid();
	df_maint_ipc.rec_type = DFM_REQ;		
	df_maint_ipc.which_file = USER_FILE_ID;
	df_maint_ipc.key = up_ptr->user_id; 
	df_maint_ipc.cisam_num = isrecnum;         
	if((qid = msgget(key,0666 | IPC_CREAT)) == -1)   {
		prt_error(PROFILE_GET_ERR);
		prt_unix_err(errno,PROFILE_GET_ERR);
		sleep(3);
		ERROR(' ',PROFILE_FILE,PROFILE_GET_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving send_ipc - 1\n");
	TRACE(trace_str);
#endif
		cleanup();
	}

	if((msgsnd(qid,&df_maint_ipc,sizeof(df_maint_ipc), IPC_NOWAIT)) == -1)  {
		prt_error(PROFILE_SND_ERR);
		prt_unix_err(errno,PROFILE_SND_ERR);
		sleep(3);
		ERROR(' ',PROFILE_FILE,PROFILE_SND_ERR);
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
	return;

} /*  End of send_ipc()  */

/******************************************************************************
* read_request - read users request for options.
*******************************************************************************/

char read_request()
{
	/*  The main header, the labels and the data from the first
	 *  record in the user profile file have been displayed.  At this
	 *  point the user must request one of the eight options,
	 *  they are : Q, q, N, n, P, p, A, a, U, u, R, r, O, o, E, and e.
	 */

	int c;		/* For reading characters */

#ifdef DEBUG
	sprintf(trace_str,"Entering read_request\n");
	TRACE(trace_str);
#endif

	move(REQ_ROW,REQ_COL);
	refresh();

	while((c = getkey()) != ERR)  {
	    c = Toupper(c);
	        if (c == M_QUERY ||
		    c == M_NEXT ||
		    c == M_PREVIOUS ||
		    c == M_ADD ||
		    c == M_UPDATE ||
		    c == M_REMOVE ||
		    c == M_OUTPUT ||
		    c == M_EXIT)
		    break;
		else  {
			move(REQ_ROW,REQ_COL);
			refresh();
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

	mvprintw(HDR_ROW_1,HDR_COL_1,QUERY_HDR);
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
* prt_hdr - print the main screen header.
*******************************************************************************/

void prt_hdr()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_hdr\n");
	TRACE(trace_str);
#endif

	clear_hdr_line();
	mvaddstr(HDR_ROW_1,HDR_COL_1,MAIN_HDR);
	attrset(A_BOLD);
	mvaddstr(HDR_ROW_2,HDR_COL_2,PROFILE_TITLE);
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
	mvaddstr(HDR_ROW_1,HDR_COL_1,UPDATE_HDR);
	refresh();

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_upd_hdr\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_upd_hdr() */

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
		if(c == YES || c == NO)
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
* prompt_fname - prompt the user for a file name to output the screen image to.
*******************************************************************************/

prompt_fname()
{

#ifdef DEBUG
	sprintf(trace_str,"Entering prompt_fname\n");
	TRACE(trace_str);
#endif
	
	clear_hdr_line();
	mvaddstr(HDR_ROW_1,HDR_COL_1,PROFILE_FNAME_PROMPT);
	refresh();
	getstr(o_file);
	if(!strcmp(o_file,""))
		strcpy(o_file,"up_scr.out");

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
		mvprintw(HDR_ROW_1,HDR_COL_1,FMODE_PROMPT,o_file);

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
				prt_error(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_78, 
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
	refresh();

	/* Reset video attributes */

	attrset(0);

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_error\n");
	TRACE(trace_str);
#endif

	beep();

	return;

}  /*  End of prt_error()  */

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

	for(i = 1; i <= NUM_FLDS; i++) 
		up_cl_flds(i); 

#ifdef DEBUG
	sprintf(trace_str,"Leaving clear_flds\n");
	TRACE(trace_str);
#endif
	return;
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
	move(ERROR_ROW,ERROR_COL);
	clrtoeol();
	refresh();  

#ifdef DEBUG
	sprintf(trace_str,"Leaving clear_err_line\n");
	TRACE(trace_str);
#endif
	return;
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
	return;
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

        if ( do_jmp ) {
		do_jmp = 0;
                longjmp(jmp_env, 1);
	}
 
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_intr\n");
	TRACE(trace_str);
#endif
	return;
}

/******************************************************************************
* clear_desc_line - clear the field description line on the screen.
*******************************************************************************/

clear_desc_line()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering clear_desc_line\n");
	TRACE(trace_str);
#endif
	move(DESC_ROW,DESC_COL);
	clrtoeol();
	refresh();  

#ifdef DEBUG
	sprintf(trace_str,"Leaving clear_desc_line\n");
	TRACE(trace_str);
#endif
	return;
}

/******************************************************************************
* fld_desc - display a description of the field being added/updated.
******************************************************************************/

fld_desc(piu)
int piu;
{
	char tmp_buf[133];

#ifdef DEBUG
	sprintf(trace_str,"Entering fld_desc \n");
	TRACE(trace_str);
#endif
	clear_desc_line();
	move(DESC_ROW,DESC_COL);
	switch(piu)  {
		case F_ID :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_36, "User ID, displayed on all screens and used for transation logging"));
			break;
		case F_NAME :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_37, "Full user name"));
			break;
		case F_GRT_NAME :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_38, "Greeting name to be displayed on the greeting line"));
			break;
		case F_FL_PERM_1 :
			sprintf(tmp_buf, "%s - %s", catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_39, "Flag indicating this user may work on fleet 1"), fl_names[0]);
			addstr(tmp_buf);
			break;
		case F_FL_PERM_2 :
			sprintf(tmp_buf, "%s - %s", catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_40, "Flag indicating this user may work on fleet 2"), fl_names[1]);
			addstr(tmp_buf);
			break;
		case F_FL_PERM_3 :
			sprintf(tmp_buf, "%s - %s", catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_41, "Flag indicating this user may work on fleet 3"), fl_names[2]);
			addstr(tmp_buf);
			break;
		case F_FL_PERM_4 :
			sprintf(tmp_buf, "%s - %s", catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_42, "Flag indicating this user may work on fleet 4"), fl_names[3]);
			addstr(tmp_buf);
			break;
		case F_FL_PERM_5 :
			sprintf(tmp_buf, "%s - %s", catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_43, "Flag indicating this user may work on fleet 5"), fl_names[4]);
			addstr(tmp_buf);
			break;
		case F_FL_PERM_6 :
			sprintf(tmp_buf, "%s - %s", catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_44, "Flag indicating this user may work on fleet 6"), fl_names[5]);
			addstr(tmp_buf);
			break;
		case F_FL_PERM_7 :
			sprintf(tmp_buf, "%s - %s", catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_45, "Flag indicating this user may work on fleet 7"), fl_names[6]);
			addstr(tmp_buf);
			break;
		case F_FL_PERM_8 :
			sprintf(tmp_buf, "%s - %s", catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_46, "Flag indicating this user may work on fleet 8"), fl_names[7]);
			addstr(tmp_buf);
			break;
		case F_M_PERM_1 :
		case F_M_PERM_2 :
		case F_M_PERM_3 :
		case F_M_PERM_4 :
		case F_M_PERM_5 :
		case F_M_PERM_6 :
		case F_M_PERM_7 :
		case F_M_PERM_8 :
		case F_M_PERM_9 :
			sprintf(tmp_buf, "%s %s", catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_47, "A flag indicating this user may select"), mf_names[piu - F_M_PERM_1]);
			addstr(tmp_buf);
			break;
		case F_M_PERM_11 :
		case F_M_PERM_12 :
		case F_M_PERM_13 :
		case F_M_PERM_14 :
		case F_M_PERM_15 :
		case F_M_PERM_16 :
		case F_M_PERM_17 :
		case F_M_PERM_18 :
		case F_M_PERM_19 :
		case F_M_PERM_21 :
		case F_M_PERM_22 :
		case F_M_PERM_23 :
		case F_M_PERM_24 :
		case F_M_PERM_25 :
		case F_M_PERM_26 :
		case F_M_PERM_27 :
		case F_M_PERM_28 :
		case F_M_PERM_29 :
		case F_M_PERM_31 :
		case F_M_PERM_32 :
		case F_M_PERM_33 :
		case F_M_PERM_34 :
		case F_M_PERM_35 :
		case F_M_PERM_36 :
		case F_M_PERM_37 :
		case F_M_PERM_38 :
		case F_M_PERM_39 :
		case F_M_PERM_61 :
		case F_M_PERM_62 :
		case F_M_PERM_63 :
		case F_M_PERM_64 :
		case F_M_PERM_65 :
		case F_M_PERM_66 :
		case F_M_PERM_67 :
		case F_M_PERM_68 :
		case F_M_PERM_69 :
		case F_M_PERM_281 :
		case F_M_PERM_282 :
		case F_M_PERM_283 :
		case F_M_PERM_284 :
		case F_M_PERM_285 :
		case F_M_PERM_286 :
		case F_M_PERM_287 :
		case F_M_PERM_288 :
		case F_M_PERM_289 :
			sprintf(tmp_buf, "%s %s", catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_47, "A flag indicating this user may select"), mf_names[piu - F_M_PERM_1 - 3]);
			addstr(tmp_buf);
			break;
		case F_DF_FLEET :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_48, "Default fleet ID"));
			break;
		case F_DF_SCR :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_49, "Default screen for this user in navigation format"));
			break;
		case F_DF_SRCH :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_50, "Default search for this user in navigation format"));
			break;
		case F_DF_POS :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_51, "Default user position number"));
			break;
		case F_DF_CL_TYP :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_52, "Default call type"));
			break;
		case F_DF_CITY :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_53, "Default city code"));
			break;
		case F_DF_HI_CL_PRI :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_54, "1 - 63. The highest priority this user may assign to a call"));
			break;
		case F_CN_ZONE_CLS :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_55, "Y/N. Indicates this user can zone calls"));
			break;
		case F_LG_FILE :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_56, "Name of a file to be displayed when the user exits"));
			break;
		      case F_AUTO_CALL_MSG:
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_80, "Whether to automatically show Free Text Message window after call entry (J/N)"));
			break;
		case F_SPZ_1 :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_57, "Superzone 1 default section"));
			break;
		case F_SPZ_2 :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_58, "Superzone 2 default section"));
			break;
		case F_SPZ_3 :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_59, "Superzone 3 default section"));
			break;
		case F_SPZ_4 :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_60, "Superzone 4 default section"));
			break;
		case F_SPZ_5 :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_61, "Superzone 5 default section"));
			break;
		case F_SPZ_6 :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_62, "Superzone 6 default section"));
			break;
		case F_SPZ_7 :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_63, "Superzone 7 default section"));
			break;
		case F_SPZ_8 :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_64, "Superzone 8 default section"));
			break;
		case F_SPZ_9 :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_65, "Superzone 9 default section"));
			break;
		case F_SPZ_10 :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_66, "Superzone 10 default section"));
			break;
		case F_SPZ_11 :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_67, "Superzone 11 default section"));
			break;
		case F_SPZ_12 :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_68, "Superzone 12 default section"));
			break;
		case F_SPZ_13 :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_69, "Superzone 13 default section"));
			break;
		case F_SPZ_14 :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_70, "Superzone 14 default section"));
			break;
		case F_SPZ_15 :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_71, "Superzone 15 default section"));
			break;
		case F_SPZ_16 :
			addstr(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_72, "Superzone 16 default section"));
			break;
		case F_EX_GP_1 :
		case F_EX_GP_2 :
		case F_EX_GP_3 :
		case F_EX_GP_4 :
		case F_EX_GP_5 :
		case F_EX_GP_6 :
		case F_EX_GP_7 :
		case F_EX_GP_8 :
		case F_EX_GP_9 :
		case F_EX_GP_10 :
		case F_EX_GP_11 :
		case F_EX_GP_12 :
			sprintf(tmp_buf, "%s %s %s", catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_73,
				"Y/N. Whether exception group"),
				eg_names[piu - F_EX_GP_1],
				catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_75, "is included in this users selection")); 
			addstr(tmp_buf);
			break;
		case F_EX_TYP_1 :
		case F_EX_TYP_2 :
		case F_EX_TYP_3 :
		case F_EX_TYP_4 :
		case F_EX_TYP_5 :
		case F_EX_TYP_6 :
		case F_EX_TYP_7 :
		case F_EX_TYP_8 :
		case F_EX_TYP_9 :
		case F_EX_TYP_10 :
		case F_EX_TYP_11 :
		case F_EX_TYP_12 :
		case F_EX_TYP_13 :
		case F_EX_TYP_14 :
		case F_EX_TYP_15 :
		case F_EX_TYP_16 :
		case F_EX_TYP_17 :
		case F_EX_TYP_18 :
		case F_EX_TYP_19 :
		case F_EX_TYP_20 :
		case F_EX_TYP_21 :
		case F_EX_TYP_22 :
		case F_EX_TYP_23 :
		case F_EX_TYP_24 :
		case F_EX_TYP_25 :
		case F_EX_TYP_26 :
		case F_EX_TYP_27 :
		case F_EX_TYP_28 :
		case F_EX_TYP_29 :
		case F_EX_TYP_30 :
		case F_EX_TYP_31 :
		case F_EX_TYP_32 :
			sprintf(tmp_buf, "%s %s %s", catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_74,
				"Y/N. Whether exception type"),
				et_names[piu - F_EX_TYP_1],
				catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_75, "is included in this users selection")); 
			addstr(tmp_buf);
			break;
#ifdef NAMERICA
	        case F_HIA_1:
	        case F_HIA_2:
		case F_HIA_3:
		case F_HIA_4:
		case F_HIA_5:
		case F_HIA_6:
		case F_HIA_7:
		case F_HIA_8:
		case F_HIA_9:
		case F_HIA_10:
		case F_HIA_11:
		case F_HIA_12:
		case F_HIA_13:
		case F_HIA_14:
		case F_HIA_15:
		case F_HIA_16:
		case F_HIA_17:
		case F_HIA_18:
			sprintf(tmp_buf, "%s %s %s", "Y/N. Whether user should see exceptions on",
				channel_names[piu - F_HIA_1],
				"radio channel");
			addstr(tmp_buf);
			break;
#endif
			
	}
	refresh();

#ifdef DEBUG
	sprintf(trace_str,"Leaving fld_desc \n");
	TRACE(trace_str);
#endif
}  /* End of fld_desc()  */
