static char sz__FILE__[]="@(#)vh_cl.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  vh_cl.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:52:19
* @(#)  Last delta: 12/2/94 at 18:02:40
* @(#)  SCCS File: /taxi/sccs/s.vh_cl.c
*                                                                           *
* Copyright (c) 1991 - MDI Seattle                                          *
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

struct scr_flds srch_vh_flds[] = {
	{0,0,1, 1},
	{SRCH_ROW0, SRCH_FLEET1_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET2_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET3_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET4_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET5_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET6_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET7_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET8_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_VH_VHNUMBER_COL, CHARS_VHNUMBER, 1},
};


extern int scr_name;
struct cisam_vh srch_vh_calls[SEARCHES_ON_PG]; 			/* for reading records from disk */
extern struct cisam_cl saved_srch_calls[SEARCHES_ON_PG];	/* for saving records read from disk */
extern int prev_scr_flag;					/* set if <PREV_SCR> key is entered */
extern short clear_err_flag;					/* for matches function */
extern WINDOW *srch_win;					/* window used for search selection criteria */
extern char req_buf[];						/* buffer for r/w records from/to disk */
extern int matches_index;					/* for matches function */
extern struct keydesc veh_key2;
extern int (*fnptr_matches)();					/* pointer to function used to test if a record read meets */
								/* the selection criteria.    */
extern int (*fnptr_srches)();
extern call_vh_search();
extern struct scr_flds srch_flds[];
int matches_vh();

extern struct keydesc *ptr_srch_key;

/*****************************************************************/
/* takes care of processing the assigned calls search            */
/*****************************************************************/
veh_srch()
{
	int calls_on_page;			/* # of call records read from disk, as returned from call_search() */
	struct cisam_vh *ptr;			/* pointer to be assigned to req_buf[] */
	int piu;				/* prompt in use */
	int field_entered;			/* 0= field not entered; 1=field entered */
	int ret_ck_fld;				/* return code from ck_assigned_fld[]() */
	int detail_call_no;
	extern int end_unassigned_srch;
	short loop;
	WINDOW *std_win;
	int key_val;				/* value returned by function */
	char read_buf[READ_BUF_SIZE];		/* pointer to buffer for reading input */
	struct scr_flds *ptr_scr_fld;
	struct scr_flds line_num;		/* for piu structure for line # entry */
	int should_clear = 0;
	int first_time = 1;

start_again:
	calls_on_page = 0;
	disp_vh_dr_dtl_scrn();

	end_unassigned_srch = 0;

	if ( prev_scr_flag )  {
#ifdef TEMP_SKIP
/* for the time being, skip from here -----------------*/
#endif
	}
	else {
	        bzero(&select_criteria, sizeof(select_criteria));
		if (!criteria_vh(&select_criteria)) { 			/* if find nothing on disk */
			/*
			werase(srch_win);
			wrefresh(srch_win);
			*/
			return (NOT_DONE);
		}
		fnptr_matches = matches_vh;
		if(calls_on_page = call_vh_search(YES, &calls_on_page,FORWARD, YES)) { 	/* some records found in the search */
			detail_call_no = NOT_AVAILABLE;  				/* indicates no detail on display */
			disp_vh_sum(calls_on_page); 					/* display summary of these calls */
		}
		else  {
			prt_error(UI_NO_RECORD," "); 					/* "No record found in search" 	*/
			return;
		}
	}

	while(scr_name == VEHICLE_SRCH_SCR && !end_unassigned_srch ) { 		/* the cursor is now on the "Line:" prompt; */
										/* read the line number. The user can enter */
										/* a line number or he/she can press a function */
										/* key */
		piu = 0;							/* piu is a local int */
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

		fnptr_srches = call_vh_search;
		ret_ck_fld = check_vh_dr_line_no(read_buf,key_val,field_entered,
					&end_unassigned_srch,&calls_on_page, select_criteria,&detail_call_no);

		if ( ret_ck_fld == DONE )  
			continue;
		else 
			ftnkey_handler(&piu, key_val,detail_call_no);

	}


}


matches_vh(ptr)
struct cisam_vh *ptr;					/* pointer to be all to req_buf[] */
{
	if (ptr->fleet == select_criteria.fleet[matches_index])
		return 1;
	else
		return 0;
}
