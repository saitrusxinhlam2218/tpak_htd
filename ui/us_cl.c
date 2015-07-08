static char sz__FILE__[]="@(#)us_cl.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  us_cl.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:52:15
 * @(#)  Last delta: 12/2/94 at 18:29:15
 * @(#)  SCCS File: /taxi/sccs/s.us_cl.c
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "call_entry.h"
#define SRCH_EXTERNS
#include "srch.h"
#include "ui_def.h"
#include "screens.h"
#include "ui_error.h"
#include "func_keys.h"
#include "language.h"
#include "ui_strdefs.h"

#define		SEARCHES_ON_PG		17 		/* max call summary on a pg */
#define		FORWARD			1
#define 	BACKWARD 		0
#define		SRCH_ROW0 		0 
#define		SRCH_USRID_COL		29
#define		CHARS_USRID		4
#define         SRCH_FULLNAME_COL       53
#define         CHARS_FULLNAME          24

#define		LINE_NUM_ROW		4
#define		LINE_NUM_COL		8
#define		LINE_NUM_MAX		2

struct scr_flds srch_usr_flds[] = {
	{0,0,1, 1},
	{SRCH_ROW0, SRCH_USRID_COL, CHARS_USRID, 1},
        {SRCH_ROW0, SRCH_FULLNAME_COL, CHARS_FULLNAME, 1},
};


extern int scr_name;
struct cisam_up srch_up_calls[SEARCHES_ON_PG]; 			/* for reading records from disk */
extern int prev_scr_flag;						/* set if <PREV_SCR> key is entered */
extern short clear_err_flag;
extern WINDOW *srch_win;						/* window used for search selection criteria */
extern char req_buf[];							/* buffer for r/w records from/to disk */
extern int matches_index;						/* for matches function */
extern struct keydesc up_key1;
extern int (*fnptr_matches)();						/* pointer to function used to test */
				   					/* if a record read meets the selection criteria.    */
extern int (*fnptr_srches)();
extern call_up_search();
extern struct scr_flds srch_flds[];

extern struct keydesc *ptr_srch_key;

user_srch()
{
	int calls_on_page;				/* # of call records read from disk, as returned from call_search() */
	struct cisam_up *ptr;				/* pointer to be assigned to req_buf[] */
	int piu;					/* prompt in use */
	int field_entered;				/* 0= field not entered; 1=field entered */
	int ret_ck_fld;					/* return code from ck_assigned_fld[]() */
	int detail_call_no;
	extern int end_unassigned_srch;
	short loop;
	WINDOW *std_win;
	int key_val;					/* value returned by function */
	char read_buf[READ_BUF_SIZE];			/* pointer to buffer for reading input */
	struct scr_flds *ptr_scr_fld;
	struct scr_flds line_num;			/* for piu structure for line # entry */
	int should_clear = 0;
	int first_time = 1;


start_again:
	calls_on_page = 0;
	disp_user_scrn();

	end_unassigned_srch = 0;

	if ( prev_scr_flag )  {
#ifdef TEMP_SKIP
/* for the time being, skip from here -----------------*/
#endif
	}
	else {
		if (!criteria_usr(&select_criteria)) { 			/* if find nothing on disk */
			/*
			werase(srch_win);
			wrefresh(srch_win);
			*/

			return (NOT_DONE);
		}
		if ( calls_on_page = call_up_search(YES,&calls_on_page,FORWARD,YES))  {
			detail_call_no = NOT_AVAILABLE;  			/* indicats no detail on display */
			disp_usr_sum(calls_on_page);
		}
		else  {
			prt_error(UI_NO_RECORD," "); 				/* "No record found in search" 	*/
			return;
		}
	}

	while ( scr_name == USER_SRCH_SCR && !end_unassigned_srch ) {
		piu = 0;
		line_num.prompt_row = LINE_NUM_ROW;
		line_num.prompt_col = LINE_NUM_COL;
		line_num.max_chars = LINE_NUM_MAX;
		line_num.tab_field = 1;

		ptr_scr_fld = &line_num;	
		
		if(first_time) {		/* force highlight of first entry of search*/
			strcpy(read_buf, "1");
			field_entered = 1;
			key_val = '\n';
			first_time = 0;
		}
		else
			key_val = read_field(stdscr,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);

		if (key_val == -1)
			continue; 			/* read_field timed out */

		if (clear_err_flag)
			clear_error();

		if (key_val == SEND_KEY) {
		    first_time = 1;
		    goto start_again;
		}

		fnptr_srches = call_up_search;
		ret_ck_fld = check_us_line_no(read_buf,key_val,field_entered,
					&end_unassigned_srch,&calls_on_page, select_criteria,&detail_call_no);
					
		if ( ret_ck_fld == DONE )  
			continue;				/* back to while loop */
		else 
			ftnkey_handler(&piu, key_val,0);

	}

}
