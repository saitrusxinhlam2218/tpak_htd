static char sz__FILE__[]="@(#)sbsc_ft_cl.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  sbsc_ft_cl.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:49:32
 * @(#)  Last delta: 12/2/94 at 18:21:59
 * @(#)  SCCS File: /taxi/sccs/s.sbsc_ft_cl.c
 *                                                                           *
 *****************************************************************************/


/* Make sure location constants are defined. Create a compile error if they
   are not defined or both are defined. */
#ifdef SCANDINAVIA
#ifdef NAMERICA
ERROR: "Both compile time constants SCANDINAVIA and NAMERICA are defined."
#endif
#else
#ifdef NAMERICA
#else
ERROR: "Compile time constant SCANDINAVIA or NAMERICA undefined."
#endif
#endif

#include <cursesX.h>
#include <string.h>
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


struct scr_flds srch_sf_flds[] = {
	{0,0,1,1},
	{SRCH_ROW0, SRCH_FLEET1_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET2_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET3_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET4_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET5_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET6_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET7_COL, CHARS_FLEET, 1},
	{SRCH_ROW0, SRCH_FLEET8_COL, CHARS_FLEET, 1},
	{SRCH_ROW1, SRCH_STREET_COL, CHARS_STREETNAME, 1},
	{SRCH_ROW1, SRCH_PASSENGER_COL, CHARS_PASSENGER, 1},
	{SRCH_ROW2,SRCH_CHARGE_COL,CHARS_CHARGENUMBER, 1},
#ifdef SCANDINAVIA
	{SRCH_ROW2,SRCH_PHONE_COL1,CHARS_PHONE_NUMBER, 1},
#else
	{SRCH_ROW2,SRCH_PHONE_COL1,CHARS_PHONE_NUM1, 1},
#endif
	{SRCH_ROW2,SRCH_PHONE_COL2,CHARS_PHONE_NUM2, 1},
	{SRCH_ROW3,SRCH_CALLNUM_COL,CHARS_CALLNUMBER, 1},
};

int sc_key_on;
extern int scr_name;
struct cisam_sc subs_calls[SEARCHES_ON_PG];	 			/* for reading records from disk */
extern int prev_scr_flag;						/* set if <PREV_SCR> key is entered */
extern short clear_err_flag;						/* for matches function */
extern WINDOW *srch_win;						/* window used for search selection criteria */
extern char req_buf[];							/* buffer for r/w records from/to disk */
extern int matches_index;						/* for matches function */
int matches_subs();
extern int (*fnptr_matches)();						/* pointer to function used to test */
				   					/* if a record read meets the selection criteria.    */
extern int (*fnptr_srches)();	
extern struct scr_flds srch_flds[];

extern struct keydesc *ptr_srch_key;

subsc_fut_calls()							/* takes care of processing the assigned calls search */
{
	int calls_on_page;				/* # of call records read from disk, as returned from call_search() */
	struct cisam_sc *ptr;				/* pointer to be assigned to req_buf[] */
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

	end_unassigned_srch  = 0;

	if (!criteria_sf(&select_criteria)) {
		close_subs_file();
		return (NOT_DONE);
	}
					
	fnptr_matches = matches_subs;
	if(calls_on_page = call_subs_search(YES, &calls_on_page, FORWARD, YES)){		/* found calls ? */
		detail_call_no = NOT_AVAILABLE;  					/* indicates no detail on display */
		disp_call_sum(calls_on_page);
	}
	else { 							/* no calls record found in search */
		prt_error(UI_NO_RECORD," "); 			/* "No record found in search" 	*/
		close_subs_file();
		return;
	}

	while ( scr_name == SUBS_FUTURE_CLS_SCR && !end_unassigned_srch ) {
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

		fnptr_srches = call_subs_search;
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
	close_subs_file();

}

/**************************************************************************************/
/* matches_subs - filter calls submitted for criteria used in search		      */
/**************************************************************************************/
matches_subs(ptr,fleet_id,srch_dir)
struct cisam_sc *ptr;					/* pointer to be all to req_buf[] */
char fleet_id;						/* fleet id to match */
int srch_dir;						/* search direction FORWARD or BACKWARD */
{
	short i;

	switch (sc_key_on) {
		case 1: 				/* The composite of sc_key1 is {nbr} */
			for (i=0; i<FLEET_MAX; i++){
				if(select_criteria.fleet[i] == ptr->fleet)
					return(DONE);
			}
			break;
		case 3: 				/* The composite of sc_key3 is {fleet, pckup_str_key} */
			if (ptr->fleet == fleet_id &&
			    (strncmp(ptr->pckup_str_key,select_criteria.streetname,MIN(strlen(select_criteria.streetname),4)) == 0))
				return(DONE);
			break;
		case 4: 				/* The composite of sc_key4 is {fleet, pass_name_key} */
			if (ptr->fleet == fleet_id &&
			    (strncmp(ptr->pass_name,select_criteria.passengername,MIN(strlen(select_criteria.passengername),4)) == 0))
				return(DONE);
			break;
		case 5: 				/* The composite of sc_key5 is {fleet, phone_nbr} */
			if (ptr->fleet == fleet_id)
				return(DONE);
			break;
		case 6: 				/* The composite of sc_key6 is {fleet, charge_number} */
			if (ptr->fleet == fleet_id)
				return(DONE);
			break;
	}
	return(NOT_DONE);
}
