 static char sz__FILE__[]="@(#)indiv_cl.c        10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  indiv_cl.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:49:01
 * @(#)  Last delta: 12/2/94 at 18:04:51
 * @(#)  SCCS File: /taxi/sccs/s.indiv_cl.c
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#define SRCH_EXTERNS
#include "srch.h"
#include "ui_def.h"
#include "ui_error.h"
#include "call_entry.h"
#include "screens.h"
#include "func_keys.h"
#include "language.h"
#include "ui_ipc.h"
#include "enhance.h"
#include "switch_ext.h"

#define		LINE_NUM_ROW		1
#define		LINE_NUM_COL		1
#define		LINE_NUM_MAX		2
#define		FORWARD			1
#define		BACKWORD		0

extern int scr_name;
extern short clear_err_flag;	/* for matches function */
extern WINDOW *srch_win;	/* window used for search selection criteria */
extern struct cisam_cl srch_calls[];
extern char req_buf[];
extern int (*fnptr_matches)();	/* pointer to function used to test if a record */
				/* read meets the selection criteria. */
extern int matches_all();
extern int prev_scr_flag;	/* Indicates this routine was called because of a previous screen function key */
extern int crit_all_on;
extern struct scr_flds extended_scr_fld[];
extern int end_unassigned_srch;
extern short silent;

int end_individual_srch;

short max_piu;

indiv_calls()
{
	int field_entered;
	int piu;			/* prompt in use */
	int got_calls;			/* a call was found by the search */
	int key_val;			/* value returned by function */
	char read_buf[133];		/* used for reading user input */
	struct scr_flds *ptr_scr_fld;
	struct scr_flds line_num;	/* for piu structure for line # entry */
	int ret_local_keys;
	int should_clear = 0;


start_again:
	fnptr_matches = matches_all;	/* use the matches all routine for verifying the selection criteria */

	/* initialize value of max_piu to # of fields on the screen */
	max_piu = MAX_PIU_EXTENDED;
	ptr_scr_fld = extended_scr_fld;

	/* end_unassigned_srch is really a global flag used to terminate
	 * all the searches.  It must be reset so the criteria routine will work correctly.  */
	 end_unassigned_srch = 0;

	/* Display the individual call search screen */
	scr_name = INDIVIDUAL_CALLS_SCR;

	if ((0) && (srch_calls[0].call_msg_nbr > '0')) /* ENH0002: Determine whether the 'number of call messages' field should
						       be displayed */
		disp_extended_scr(1);
	else
		disp_extended_scr(0);

	prt_entry_areas(stdscr,ptr_scr_fld,max_piu);
	refresh();

	if (prev_scr_flag)  {
		restor_svd_cls(1);	/* restor calls saved when prevoius individual call search was interrupted  */
		scr_name = INDIVIDUAL_CALLS_SCR;
		prt_eold_fields(&srch_calls[0]);
	}
	else {
		crit_all_on = 0;
		memset(&select_criteria,'\0',sizeof(struct select_criterias));

		if (!criteria_all(&select_criteria))   /* Invalid criteria was entered */
			return(NOT_DONE);
	
		get_call(&got_calls,FORWARD, YES);
		if (got_calls)  {
			scr_name = INDIVIDUAL_CALLS_SCR;
			prt_eold_fields(&srch_calls[0]);
		}
		else { 							/* no calls record found in search */
			prt_error(UI_NO_RECORD," "); 			/* "No record found in search" 	*/
			return(NOT_DONE);
		}

	} /* end else */

	/* while work continues on the individual call screen */
	end_individual_srch = NOT_DONE;
	while(scr_name==INDIVIDUAL_CALLS_SCR && end_individual_srch==NOT_DONE)  {

		/* prepare to read the first field */
		piu = 0;	
		silent = 1;
		line_num.prompt_row = LINE_NUM_ROW;
		line_num.prompt_col = LINE_NUM_COL;
		line_num.max_chars = LINE_NUM_MAX;
		ptr_scr_fld = &line_num;	
		key_val = read_field(stdscr,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);

		if (clear_err_flag) clear_error();

		if (key_val == -1) continue; 	/* Nothing was entered by the user */

		if (key_val == SEND_KEY) {
		    goto start_again;
		}

		if (local_keys(key_val)==DONE) continue; /* local function key was processed */

		else /* There are two ways to get out of individual call search :
		 	1) The search function key routine changes the screen name.
		 	2) The command function key routine sets end_individual_srch.  */
			ftnkey_handler(&piu, key_val,0);

	} /* end while */
}  /* end indiv_calls() */
