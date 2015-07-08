/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: zn_cl.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2003/01/28 17:55:30 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/ui/zn_cl.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: zn_cl.c,v 1.3 2003/01/28 17:55:30 jwelch Exp $";

/* Make sure location constants are defined. Create a compile error if they
   are not defined or both are defined. */
#ifdef SCANDINAVIA
#ifdef NAMERICA
ERROR: "Both compile time constants SCANDINAVIA and NAMERICA are defined."
#endif
#else
#ifdef NAMERICA
#else
ERROR: Compile time constant SCANDINAVIA or NAMERICA undefined.
#endif
#endif

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


struct scr_flds srch_zn_flds[] = {
	{0,0,1,1},
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
	{SRCH_ROW1, SRCH_ZONE_NBR_COL, CHARS_ZONE_NBR, 1},
};

extern int	cl_key_on;
extern int scr_name;
extern struct cisam_cl srch_calls[SEARCHES_ON_PG]; 			/* for reading records from disk */
extern struct cisam_cl saved_srch_calls[SEARCHES_ON_PG];		/* for saving records read from disk */
extern int prev_scr_flag;						/* set if <PREV_SCR> key is entered */
extern struct keydesc zn_key1;
extern struct keydesc veh_key2;
extern short clear_err_flag;						/* for matches function */
extern WINDOW *srch_win;						/* window used for search selection criteria */
extern char req_buf[];							/* buffer for r/w records from/to disk */
extern int matches_index;						/* for matches function */
extern time_t srch_dt_tm;
extern int zn_or_vh_on[];
int matches_zn();
extern int (*fnptr_matches)();						/* pointer to function used to test */
				   					/* if a record read meets the selection criteria.    */
extern void restor_svd_cls();
extern int (*fnptr_srches)();
extern call_zn_search();
extern struct scr_flds srch_flds[];

extern struct keydesc *ptr_srch_key;

zn_calls() 								/* takes care of processing the assigned calls search */
{
	int calls_on_page;
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
	struct scr_flds line_num;	/* for piu structure for line # entry */
	int should_clear = 0;
	int first_time = 1;

start_again:
	calls_on_page = 0;
	disp_calls_scrn();
	select_criteria.zone_nbr[0] = '\0';

	end_unassigned_srch  = 0;


	for (loop=0; loop<SEARCHES_ON_PG; loop++)
		zn_or_vh_on[loop] = 0;

	if(prev_scr_flag){
		bump_resp(ALL_CALLS_SEARCH_CMD,START_TIME_CMD);	/* bump count for response time stats */
		prev_scr_flag = 0;
		restor_svd_cls(calls_on_page);
		disp_call_sum(calls_on_page);
	}
	    else {
		if (!criteria_zn(&select_criteria)) {
			return (NOT_DONE);
		}
					
		bump_resp(ALL_CALLS_SEARCH_CMD,START_TIME_CMD);	/* bump count for response time stats */
		fnptr_matches = matches_zn;
		if(calls_on_page = call_zn_search(YES, &calls_on_page, FORWARD, YES)){		/* found calls ? */
			detail_call_no = NOT_AVAILABLE;  					/* indicates no detail on display */
			disp_call_sum(calls_on_page);
		}
		else { 							/* no calls record found in search */
			prt_error(UI_NO_RECORD," "); 			/* "No record found in search" 	*/
			return;
		}
	}


	while ( scr_name == SRCH_USER_DEF1 && !end_unassigned_srch ) {
		piu = 0;						/* piu points to field number */
		line_num.prompt_row = LINE_NUM_ROW;
		line_num.prompt_col = LINE_NUM_COL;
		line_num.max_chars = LINE_NUM_MAX;

		ptr_scr_fld = &line_num;	
		
		if(first_time) {		/* force highlight of first entry of search*/
			strcpy(read_buf, "1");
			field_entered = 1;
			key_val = '\n';
			first_time = 0;
		}
		else
			key_val = read_field(stdscr,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);

		if (clear_err_flag)
			clear_error();

		if (key_val == -1)
			continue;

		if (key_val == SEND_KEY) {
		    first_time = 1;
		    goto start_again;
		}

		fnptr_srches = call_zn_search;
		ret_ck_fld = check_line_no(read_buf,key_val,field_entered, &end_unassigned_srch,&calls_on_page,
					select_criteria,&detail_call_no);
					
		if ( ret_ck_fld == DONE )  		/* user might have entered a line # or a function  */
			 				/* key like <NEXT_PG>, etc. processing of which is */
			 				/* done by check_line_no(). Check_line_no() takes */
			 				/* care of processing the <DETAIL> function key   */
			 				/* (which displays the call on the extended or the */
			 				/* basic call entry screen) */
			continue;			/* back to while loop */
		else 
			ftnkey_handler(&piu, key_val,detail_call_no); 	/* user entered a function key which is not */
			 						/* processed by the check_line_no(). So call */
			 						/* the ftnkey_handler() */

	}

}


matches_zn(ptr,fleet_id,srch_dir)
struct cisam_cl *ptr;					/* pointer to be all to req_buf[] */
char fleet_id;						/* fleet id to match */
int srch_dir;						/* search direction FORWARD or BACKWARD */
{
	short i;

	if ((ptr->fleet == fleet_id) && (ptr->due_date_time >= srch_dt_tm)
	    && ( ptr->pckup_zone == atoi(select_criteria.zone_nbr )) )
	  return(DONE);
	else
	  return(NOT_DONE);
}






