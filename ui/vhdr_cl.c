static char sz__FILE__[]="@(#)vhdr_cl.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  vhdr_cl.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:52:30
 * @(#)  Last delta: 12/2/94 at 18:30:07
 * @(#)  SCCS File: /taxi/sccs/s.vhdr_cl.c
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
#include "ui_def.h"
#include "screens.h"
#include "ui_error.h"
#include "func_keys.h"
#include "language.h"


#define		LINE_NUM_ROW		3
#define		LINE_NUM_COL		8
#define		LINE_NUM_MAX		2


struct scr_flds srch_vhdr_flds[] = {
	{0,0,1, 1},
	{SRCH_ROW0, SRCH_FLEET1_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET2_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET3_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET4_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET5_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET6_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET7_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET8_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_DATE_COL, CHARS_STARTDATE, 1},
	{SRCH_ROW0, SRCH_TIME_COL, CHARS_STARTTIME, 1},
	{SRCH_ROW1, SRCH_VHNUMBER_COL, CHARS_VHNUMBER, 1},
	{SRCH_ROW1, SRCH_DRIVERID_COL, CHARS_DRIVERID, 1},
};


extern int cl_key_on;
extern int scr_name;
extern time_t srch_dt_tm;							/* date/time entered by the user */
extern struct cisam_cl srch_calls[SEARCHES_ON_PG]; 			/* for reading records from disk */
struct cisam_cl saved_srch_calls[SEARCHES_ON_PG];			/* for saving records read from disk */
extern int prev_scr_flag;						/* set if <PREV_SCR> key is entered */
extern short clear_err_flag;						/* for matches function */
extern WINDOW *srch_win;						/* window used for search selection criteria */
extern char req_buf[];							/* buffer for r/w records from/to disk */
extern int matches_index;						/* for matches function */
extern int zn_or_vh_on[];
extern struct keydesc cl_key4;
extern struct keydesc cl_key9;
extern int (*fnptr_matches)();						/* pointer to function used to test */

extern int (*fnptr_srches)();	
extern struct scr_flds srch_flds[];
extern int call_vd_search( );
extern void restor_svd_cls();
extern struct keydesc *ptr_srch_key;
int matches_vhdr();
veh_drv_calls()
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
	struct scr_flds *ptr_scr_fld;
	struct scr_flds line_num;			/* for piu structure for line # entry */
	int should_clear = 0;
	int first_time = 1;

start_again:
	calls_on_page = 0;
	disp_calls_scrn(); 				/* display the veh/driver search screen */

	end_unassigned_srch  = 0;

	for (loop=0; loop<SEARCHES_ON_PG; loop++)
		zn_or_vh_on[loop] = 0;

	if(prev_scr_flag){
		prev_scr_flag = 0;
		restor_svd_cls(calls_on_page);
		disp_call_sum(calls_on_page);
    	}
	else {
		if (!criteria_vhdr(&select_criteria)) {
		/*
			werase(srch_win);
			wrefresh(srch_win);
		*/
			return (NOT_DONE);
		}
		ptr = (struct cisam_cl *)req_buf;

		fnptr_matches = matches_vhdr;
		if ( calls_on_page = call_vd_search(YES, &calls_on_page,FORWARD, YES))  {
			detail_call_no = NOT_AVAILABLE;  			/* indicats no detail on display */
			disp_call_sum(calls_on_page);				/* display calls found */
		}
		else  {
			prt_error(UI_NO_RECORD," "); 				/* "No more calls found in search" 	*/
			return;
		}
	}

	while ( scr_name == VEH_DR_CALLS_SCR && !end_unassigned_srch ) { 	/* the cursor is now on the "Line:" prompt; */
										/* Read the line number. The user can enter */
										/* a line number or press a function key */
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
			continue;

		if (clear_err_flag)
			clear_error();

		if (key_val == SEND_KEY) {
		    first_time = 1;
		    goto start_again;
		}

		fnptr_srches = call_vd_search;
		ret_ck_fld = check_line_no(read_buf,key_val,field_entered, &end_unassigned_srch,&calls_on_page,
					select_criteria,&detail_call_no);
					
		if ( ret_ck_fld == DONE )  		/* user might have entered a line # or a function key like <NEXT_PG>, */
							/* etc. processing of which is done by check_line_no(). Check_line_no() */
							/* takes care of processing the <DETAIL> function key (which displays */
							/* the call on the extended or the basic call entry screen) */
			continue;			/* back to while loop */
		else 					/* user entered a function key which is not processed by the 
							/* check_line_no(). So call * the ftnkey_handler() */
			ftnkey_handler(&piu, key_val,detail_call_no);

	}

}

matches_vhdr(ptr,fleet_id)
struct cisam_cl *ptr;					/* pointer to be all to req_buf[] */
char fleet_id;						/* fleet currently workin on */
{
int temp_num;

	switch (cl_key_on) {
		case 9:
			temp_num = atoi(select_criteria.driverid);
			if (ptr->fleet == fleet_id && ptr->drv_id==temp_num && ptr->due_date_time >= srch_dt_tm) {
				return(DONE);
			}
			break;
		case 4:
			temp_num = atoi(select_criteria.vhnumber);
			if (ptr->fleet == fleet_id && ptr->veh_nbr==temp_num && ptr->due_date_time >= srch_dt_tm) {
				return(DONE);
			}
			break;
		}
		return(NOT_DONE);
}
