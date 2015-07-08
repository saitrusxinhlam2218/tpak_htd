static char sz__FILE__[]="@(#)unassign.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  unassign.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:52:12
 * @(#)  Last delta: 12/2/94 at 18:28:42
 * @(#)  SCCS File: /taxi/sccs/s.unassign.c
 *                                                                           *
 *****************************************************************************/


#include <cursesX.h>
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "call_entry.h"
#define SRCH_DCLS
#include "srch.h"
#include "srch_const.h"
#include "ui_def.h"
#include "screens.h"
#include "ui_error.h"
#include "user.h"
#include "func_keys.h"
#include "language.h"
#include "writer.h"

#define		LINE_NUM_ROW		3
#define		LINE_NUM_COL		8
#define		LINE_NUM_MAX		2

struct scr_flds srch_un_flds[] = {
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
	{SRCH_ROW1, SRCH_VHNUMBER_COL, CHARS_VHNUMBER, 1},
	{SRCH_ROW1, SRCH_DRIVERID_COL, CHARS_DRIVERID, 1},
};

struct cisam_cl srch_calls[SEARCHES_ON_PG]; 			/* for reading records from disk */
int saved_srch_calls_flag=0;
struct cisam_cl saved_srch_calls[SEARCHES_ON_PG];		/* for saving records read from disk */
int prev_scr_flag;						/* set if <PREV_SCR> key is entered */
extern short clear_err_flag;					/* for matches function */
int matches_un();
extern struct users *uiuser[];
extern struct fleets *fleet[];
extern char excpt_fl[];						/* fleets chosen by the user for selection purposes */
extern int user_index;
time_t srch_dt_tm;
int crit_all_on;
int crit_vhdr_on;
int cl_key_on;
int calls_on_page; 						/* # of call recs read from disk, as returned from call_search() */
int zn_or_vh_on[SEARCHES_ON_PG];
struct keydesc *ptr_srch_key;
extern int scr_name;
extern WINDOW *srch_win;					/* window used for search selection criteria */
int end_unassigned_srch;
extern char req_buf[];						/* buffer for r/w records from/to disk */
int matches_index;						/* for matches function */
int (*fnptr_matches)();						/* pointer to function used to test if a record read meets 
								/* the selection criteria.    */
int (*fnptr_srches)();	
extern call_un_search();
extern void restor_svd_cls();
extern struct keydesc cl_key4;
extern struct keydesc zn_key1;
extern struct keydesc veh_key2;
extern struct keydesc *ptr_srch_key;
struct select_criterias select_criteria;

/***********************************************************************/
/* takes care of processing the unassigned calls search               */
/***********************************************************************/
unassigned_calls()
{
	struct cisam_cl *ptr;					/* pointer to be assigned to req_buf[] */
	int piu;						/* prompt in use */
	int field_entered;					/* 0= field not entered; 1=field entered */
	int ret_ck_fld;						/* return code from ck_unassigned_fld[]() */
	int detail_call_no;
	short loop;
	WINDOW *std_win;
	int key_val;						/* value returned by function */
	char read_buf[READ_BUF_SIZE];				/* pointer to buffer for reading input */
	struct scr_flds *ptr_scr_fld;
	struct scr_flds line_num;				/* for piu structure for line # entry */
	int should_clear = 0;
	int first_time = 1;

start_again:
	disp_calls_scrn();

	end_unassigned_srch  = 0;

	if ( prev_scr_flag )  { 				/* unassigned calls search has been invoked by pressing the */
		 						/* <PREV_SCR> function key this global flag gets set by prev_scr()*/
		prev_scr_flag = 0;				/* reset previous screen flag */
		 
		restor_svd_cls(calls_on_page);			/* restor saved calls, page number and search criteria */

		disp_call_sum(calls_on_page);			/* display the summary of the calls on the screen */
	}
	else {
		calls_on_page = 0;

		for (loop=0; loop<SEARCHES_ON_PG; loop++)
			zn_or_vh_on[loop] = 0;

		if (!criteria_unassigned(&select_criteria)) {
		/*
			werase(srch_win);
			wrefresh(srch_win);
		*/
			return (NOT_DONE);
		}
					
		fnptr_matches = matches_un; 				/* now try to search for first page */
		if ( calls_on_page = call_un_search(YES,&calls_on_page,FORWARD,YES))  {
			detail_call_no = NOT_AVAILABLE;  		/* indicates no detail on display */
			disp_call_sum(calls_on_page);
		}
		else  {
			prt_error(UI_NO_RECORD," "); 			/* "No record found in search" 	*/
			return;

		}
	}

	while ( scr_name == UNASSIGNED_CALLS_SCR && !end_unassigned_srch ) { 	/* the cursor is now on the "Line:" prompt; */
										/* read the line number. The user can enter */
										/* a line number or he/she can press */
										/* a function key */
		piu = 0;							/* piu is a local int */
		line_num.prompt_row = LINE_NUM_ROW;
		line_num.prompt_col = LINE_NUM_COL;
		line_num.max_chars = LINE_NUM_MAX;

		ptr_scr_fld = &line_num;	
		
		if(first_time) {		/* force highlight of first entry of search*/
			read_buf[0] = '1';
			read_buf[1] = '\0';
			field_entered = 1;
			key_val = '\n';
			first_time = 0;
		}
		else
			key_val = read_field(stdscr,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);

		if (key_val == SEND_KEY) {
		    first_time = 1;
		    goto start_again;
		}

		if (key_val == -1)
			continue;

		if (clear_err_flag)
			clear_error();

		fnptr_srches = call_un_search;

		ret_ck_fld = check_line_no(read_buf,key_val,field_entered, &end_unassigned_srch,&calls_on_page,
					select_criteria,&detail_call_no);
					
		if ( ret_ck_fld == DONE )  
			continue;	/* back to while loop */
		else 
			ftnkey_handler(&piu, key_val,detail_call_no);

	}

}

/**********************************************************************************/
/* get the fleet from fleet structure                                             */
/**********************************************************************************/
fleet_srch()
{
	wattrset(srch_win, A_UNDERLINE);
	select_criteria.fleet[0] = excpt_fl[0];
	if (excpt_fl[0] != '\0')
		mvwaddch(srch_win,0,26,select_criteria.fleet[0]);
	else
		mvwaddch(srch_win,0,26, ' ');

	select_criteria.fleet[1] = excpt_fl[1];
	if (excpt_fl[1] != '\0')
		mvwaddch(srch_win,0,28,select_criteria.fleet[1]);
	else
		mvwaddch(srch_win,0,28, ' ');

	select_criteria.fleet[2] = excpt_fl[2];
	if (excpt_fl[2] != '\0')
		mvwaddch(srch_win,0,30,select_criteria.fleet[2]);
	else
		mvwaddch(srch_win,0,30, ' ');

	select_criteria.fleet[3] = excpt_fl[3];
	if (excpt_fl[3] != '\0')
		mvwaddch(srch_win,0,32,select_criteria.fleet[3]);
	else
		mvwaddch(srch_win,0,32, ' ');

	select_criteria.fleet[4] = excpt_fl[4];
	if (excpt_fl[4] != '\0')
		mvwaddch(srch_win,0,34,select_criteria.fleet[4]);
	else
		mvwaddch(srch_win,0,34, ' ');

	select_criteria.fleet[5] = excpt_fl[5];
	if (excpt_fl[5] != '\0')
		mvwaddch(srch_win,0,36,select_criteria.fleet[5]);
	else
		mvwaddch(srch_win,0,36, ' ');

	select_criteria.fleet[6] = excpt_fl[6];
	if (excpt_fl[6] != '\0')
		mvwaddch(srch_win,0,38,select_criteria.fleet[6]);
	else
		mvwaddch(srch_win,0,38, ' ');

	select_criteria.fleet[7] = excpt_fl[7];
	if (excpt_fl[7] != '\0')
		mvwaddch(srch_win,0,40,select_criteria.fleet[7]);
	else
		mvwaddch(srch_win,0,40, ' ');
	wattrset(srch_win, 0);
}


/***********************************************************************/
/* matches the requirements of the unassigned search                   */
/***********************************************************************/
matches_un(ptr, fleet_id)
struct cisam_cl *ptr;					/* pointer to be all to req_buf[] */
char fleet_id;						/* current valid fleet id */
{

  
  if ((ptr->pri_status) < 64  && (ptr->due_date_time >= srch_dt_tm) && (ptr->fleet == fleet_id))
    return 1;
  else
    return 0;
}
