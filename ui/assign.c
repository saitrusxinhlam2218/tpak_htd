static char sz__FILE__[]="@(#)assign.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  assign.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:46:42
 * @(#)  Last delta: 12/2/94 at 18:00:00
 * @(#)  SCCS File: /taxi/sccs/s.assign.c
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
#include "srch_const.h"
#include "writer.h"
#include "ui_def.h"
#include "screens.h"
#include "ui_error.h"
#include "func_keys.h"
#include "language.h"


#define		LINE_NUM_ROW		3
#define		LINE_NUM_COL		8
#define		LINE_NUM_MAX		2

struct scr_flds srch_asgn_flds[] = {
	{0,0,1,1},
	{SRCH_ROW0, SRCH_FLEET1_COL, CHARS_FLEET,1},
	{SRCH_ROW0, SRCH_FLEET2_COL, CHARS_FLEET,1},
	{SRCH_ROW0, SRCH_FLEET3_COL, CHARS_FLEET,1},
	{SRCH_ROW0, SRCH_FLEET4_COL, CHARS_FLEET,1},
	{SRCH_ROW0, SRCH_FLEET5_COL, CHARS_FLEET,1},
	{SRCH_ROW0, SRCH_FLEET6_COL, CHARS_FLEET,1},
	{SRCH_ROW0, SRCH_FLEET7_COL, CHARS_FLEET,1},
	{SRCH_ROW0, SRCH_FLEET8_COL, CHARS_FLEET,1},
	{SRCH_ROW0, SRCH_DATE_COL, CHARS_STARTDATE,1},
	{SRCH_ROW0, SRCH_TIME_COL, CHARS_STARTTIME,1},
	{SRCH_ROW1, SRCH_STREET_COL, CHARS_STREETNAME,1},
	{SRCH_ROW1, SRCH_PASSENGER_COL, CHARS_PASSENGER,1},
	{SRCH_ROW2,SRCH_PHONE_COL1,CHARS_PHONE_NUM1,1},
	{SRCH_ROW2,SRCH_PHONE_COL2,CHARS_PHONE_NUM1,1},
	{SRCH_ROW2,SRCH_CHARGE_COL,CHARS_CHARGENUMBER,1},
	{SRCH_ROW3,SRCH_CALLNUM_COL,CHARS_CALLNUMBER,1},
};

extern int (*fnptr_srches)();
extern call_as_search();
extern struct cisam_cl srch_calls[SEARCHES_ON_PG]; 			/* for reading records from disk */
extern struct cisam_cl saved_srch_calls[SEARCHES_ON_PG];		/* for saving records read from disk */
extern int prev_scr_flag;						/* set if <PREV_SCR> key is entered */
extern short clear_err_flag;
extern int scr_name;
int matches_as();
extern WINDOW *srch_win;						/* window used for search selection criteria */
extern time_t srch_dt_tm;							/* search criteria date/time */
extern int zn_or_vh_on[];
extern char req_buf[];							/* buffer for r/w records from/to disk */
extern int matches_index;						/* for matches function */
extern int (*fnptr_matches)();						/* pointer to function used to test if a record */
									/* read meets the selection criteria.    */
extern struct scr_flds srch_flds[];

extern void restor_svd_cls();
extern struct keydesc *ptr_srch_key;
assigned_calls() 							/* takes care of processing the assigned calls search */
{
	int calls_on_page;				/* # of call records read from disk, as returned from call_search() */
	struct cisam_cl *ptr;				/* pointer to be assigned to req_buf[] */
	int piu;					/* prompt in use */
	int field_entered;				/* 0= field not entered; 1=field entered */
	int ret_ck_fld;					/* return code from ck_assigned_fld[]() */
	int detail_call_no;
	extern int end_unassigned_srch;
	short loop;
	WINDOW *std_win;
	int key_val;					/* value returned by function */
	char read_buf[READ_BUF_SIZE];			/* pointer to buffer for reading input */
	long get_c_time();
	struct scr_flds *ptr_scr_fld;
	struct scr_flds line_num;			/* for piu structure for line # entry */
	int should_clear = 0;

start_again:
	calls_on_page = 0;
	disp_calls_scrn();

	end_unassigned_srch  = 0;

	for (loop=0; loop<SEARCHES_ON_PG; loop++)
		zn_or_vh_on[loop]=0;

	if(prev_scr_flag){
		prev_scr_flag = 0;
		restor_svd_cls(calls_on_page);
		disp_call_sum(calls_on_page);
	}
	else {
		if (!criteria_assigned(&select_criteria)) {
		/*
			werase(srch_win);
			wrefresh(srch_win);
		*/
			return (NOT_DONE);
		}

		fnptr_matches = matches_as;
		if (calls_on_page = call_as_search(YES, &calls_on_page,FORWARD, YES))  { /* found calls ? */
			detail_call_no = NOT_AVAILABLE;  				/* indicats no detail on display */
			disp_call_sum(calls_on_page); 					/* from array srch_calls[] */
		}
		else  {
			prt_error(UI_NO_RECORD," "); 					/* "No record found in search" 	*/
			return;
		}
	}

	while ( scr_name == ASSIGNED_CALLS_SCR && !end_unassigned_srch ) { 	/* the cursor is now on the "Line:" prompt; */
										/* read the line number. The user can enter */
										/* a line number or press a function key */
		check_mail();							/* check users mail */
		piu = 0;							/* piu is a pointer to current field */
		line_num.prompt_row = LINE_NUM_ROW;
		line_num.prompt_col = LINE_NUM_COL;
		line_num.max_chars = LINE_NUM_MAX;

		ptr_scr_fld = &line_num;	
		
		key_val = read_field(stdscr,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);
		if (key_val == -1)
			continue;
		if(clear_err_flag)
			clear_error();						/* clear 24th line */

		if (key_val == SEND_KEY) {
		    goto start_again;
		}

		fnptr_srches = call_as_search;
		ret_ck_fld = check_line_no(read_buf,key_val,field_entered, &end_unassigned_srch,&calls_on_page,
					select_criteria,&detail_call_no);
					
		if ( ret_ck_fld == DONE )  			/* local processing was performed */
			continue;
		else 						/* user entered a function key which is not */
			 					/* processed by the check_line_no(). So call */
			 					/* the ftnkey_handler() */
			ftnkey_handler(&piu, key_val,detail_call_no);

	} /* end while */

}  /* end assigned_calls() */

matches_as(ptr,fleet_id)
struct cisam_cl *ptr;					/* pointer to be assigned to req_buf[] */
char fleet_id;
{
	if (strcmp(ptr->status, ASSIGNED) == 0) { 
		if(ptr->pri_status == 64 && ptr->due_date_time >= srch_dt_tm && ptr->fleet == fleet_id )
			return 1;
		else
			return 0;
	}
	else
		return 0;
}
