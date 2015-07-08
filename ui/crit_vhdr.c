static char sz__FILE__[]="@(#)crit_vhdr.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  crit_vhdr.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:48:03
 * @(#)  Last delta: 12/2/94 at 18:04:33
 * @(#)  SCCS File: /taxi/sccs/s.crit_vhdr.c
 *                                                                           *
 *****************************************************************************/


#include <cursesX.h>
#include <sys/types.h>
#include "ui_strdefs.h"
#include "mads_types.h"
#include <isam.h>
#include "df_maint.h"
#include "mads_isam.h"
#include <time.h>
#include "call_entry.h"
#define SRCH_EXTERNS
#include "srch.h"
#include "srch_const.h"
#include "screens.h"
#include "ui_def.h"
#include "func_keys.h"
#include "ui_error.h"

#define		START_ASSIGNED_PROMPT		11	/* place prompt on start fleet field when reading selection criteria */

extern int     cl_key_on;
extern int     scr_name;
extern struct keydesc cl_key4;
extern struct keydesc cl_key9;
extern struct keydesc *ptr_srch_key;
extern int end_unassigned_srch;
extern int matches_index;				/* for matches function */
extern struct scr_flds srch_vhdr_flds[];
extern struct fleets *fleet[];
extern char excpt_fl[];
extern WINDOW *srch_win;				/* window used for search selection criteria */

extern time_t get_internal_time();
extern int verify_date(), verify_time();

/* declare functions for checking fleet permissions */
extern int ui_fleet_perms0(), ui_fleet_perms1(), ui_fleet_perms2(), 
	ui_fleet_perms3(), ui_fleet_perms4(), ui_fleet_perms5(), 
	ui_fleet_perms6(), ui_fleet_perms7();

/* declare array of pointers to functions for checking fleet perm's */
extern int (*ui_fleet_perms[])();

extern time_t srch_dt_tm;					/* Used in actual search */
extern short clear_err_flag;

/**************************************************************************/
/* criteria_vhdr: Displays the window with for selecting the search       */
/*			criteria. Reads the selection criteria entered    */
/*			by the call taker for vhdr calls search           */
/**************************************************************************/
 
criteria_vhdr(ptr_criteria)
struct select_criterias *ptr_criteria; 				/* pointer to structure for saving selection criteria */
{
	int ck_vhdr_dummy(), ck_vhdr_fleet(), ck_vhdr_date(), ck_vhdr_time(),
	ck_vhdr_number(), ck_vhdr_id();

								/* declare pointers to functions (which check the fields entered */
	 							/* for the searches selection criteria) */
	 							/* Note: these functions are similar to the field_check routines */
	 							/* for call entry */
	static int (*check_vhdr_flds[])() = { 
		ck_vhdr_dummy, ck_vhdr_fleet, ck_vhdr_fleet, 
		ck_vhdr_fleet, ck_vhdr_fleet, ck_vhdr_fleet, 
		ck_vhdr_fleet, ck_vhdr_fleet, ck_vhdr_fleet, 
		ck_vhdr_date, ck_vhdr_time,
		ck_vhdr_number, ck_vhdr_id
		};

	int piu;						/* prompt in use; used by read_field() */
	int end_criteria_vhdr = 0; 				/* flag set if function should end */
	int key_val;						/* value of key entered (as returned by read_field() */
	int field_entered = 0;					/* 0=field not entered; 1=field_entered */
	int ii;
	int ret_ck_fld;						/* return code by check_vhdr_flds[]() functions */
	char read_buf[READ_BUF_SIZE];				/* buffer for reading fields */
	struct scr_flds *ptr_scr_fld;
	int should_clear  = 0;
	
	disp_vhdr_win(ptr_criteria); 				/*  Display the window for selection of search criteria. */

	cl_key_on = CL_KEY4;					/* default to vehicle search */
	ptr_srch_key = &cl_key4;
	strcpy(ptr_criteria->vhnumber,"9999");			/* force invalid number */

	srch_dt_tm = get_internal_time(&ptr_criteria->startdate[0],&ptr_criteria->starttime[0]);

	piu = START_ASSIGNED_PROMPT;				/* prompt is placed on start date */

	while ( scr_name == VEH_DR_CALLS_SCR && !end_criteria_vhdr && !end_unassigned_srch) {
		check_mail();
		ptr_scr_fld = srch_vhdr_flds;

		if ( piu == 9   ||  piu == 10  )
			should_clear = 1;

		key_val = read_field(srch_win,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);

		if (key_val == -1)
			continue;
	
		if(clear_err_flag){
			clear_error();
			refresh();
		}

		wattrset(srch_win, A_UNDERLINE);
		ret_ck_fld = (*check_vhdr_flds[piu])(&piu,read_buf, key_val,field_entered, &select_criteria, &end_criteria_vhdr);
		wattrset(srch_win, A_NORMAL);

		if(ret_ck_fld == INVALID)
			continue;

		if (ret_ck_fld == NOT_DONE){ 			/* user entered a function key which is not */
			 					/* processed by the check_line_no(). So call the ftnkey_handler() */
			ftnkey_handler(&piu, key_val,NOT_AVAILABLE);
			if ( scr_name ==  VEH_DR_CALLS_SCR )
				disp_vhdr_win(ptr_criteria); 	/*  Display the window for selection of search criteria. */
			if ( key_val == HELP_KEY )  {
				/* ftnkey_handler() had called help(). The help window has overlayed the
				 * search window, so repaint the search window
				 */
				touchwin(srch_win);
				wrefresh(srch_win);
			}  /* end if help_key */
		}
	}

	if ( scr_name != VEH_DR_CALLS_SCR) {
		return (NOT_DONE);
	}
	werase(srch_win);
	wrefresh(srch_win);

	refresh();
	return (DONE);
}

#include "user.h"
extern int user_index;				/* index into users structure in shared memory for this user */

#define	PROMPT_SRCH_FLEET_COL	20
#define	PROMPT_SRCH_DATE_COL	45
#define	PROMPT_SRCH_TIME_COL	70
#define	PROMPT_SRCH_VHNUMBER_COL	20
#define	PROMPT_SRCH_DRIVERID_COL	45

/*************************************************************************/
/* disp_select_win: 	displays the search selection window for the     */
/*			vhdr calls search, also prints any               */
/*			default values in the fields (from user          */
/*			structure in shared memory)                      */
/*************************************************************************/

disp_vhdr_win(ptr_criteria)
struct select_criterias *ptr_criteria; 			/* pointer to structure for saving selection criteria */
{
	long srch_time;
	extern struct tm *localtime();
	struct tm *srch_tp;
	int ii,i,first_fleet;

	wattrset(srch_win, 0);
	werase(srch_win);
	mvwprintw(srch_win,0,PROMPT_SRCH_FLEET_COL,catgets(UI_m_catd, UI_1, UI_CRIT_VHDR_1, "Fleet:")); 				/* print the labels in the window */
	mvwprintw(srch_win,0,PROMPT_SRCH_DATE_COL,catgets(UI_m_catd, UI_1, UI_CRIT_VHDR_2, "Starting Date:"));
	mvwprintw(srch_win,0,PROMPT_SRCH_TIME_COL,catgets(UI_m_catd, UI_1, UI_CRIT_VHDR_3, "Starting Time:"));
	mvwprintw(srch_win,1,PROMPT_SRCH_VHNUMBER_COL,catgets(UI_m_catd, UI_1, UI_CRIT_VHDR_4, "Vehicle #:"));
	mvwprintw(srch_win,1,PROMPT_SRCH_DRIVERID_COL,catgets(UI_m_catd, UI_1, UI_CRIT_VHDR_5, "Driver ID:"));

	fleet_srch();

	first_fleet = -1;						/* default to nothing */
	for (i=0; i<FLEET_MAX; i++) {
		if (excpt_fl[i] != NULL) {
			first_fleet = find_findex(excpt_fl[i]);		/* convert to number */
			break;
		}
	}

	wattrset(srch_win, A_UNDERLINE);
									/* get the default starting date (today's date) */
	srch_time = time(NULL) - 60*60;					/* go back 1 hour */
	get_srch_date(srch_time, ptr_criteria->startdate);		/* get date to display as default */
	mvwprintw(srch_win,0,60, "%s", ptr_criteria->startdate);
	
	get_srch_time(srch_time, ptr_criteria->starttime);
	mvwprintw(srch_win,0,85,"%s", ptr_criteria->starttime);

	/* Vehicle # */
	alter_highlight(srch_win, SRCH_ROW1, SRCH_VHNUMBER_COL, SRCH_VHNUMBER_COL + CHARS_VHNUMBER - 1, A_UNDERLINE);

	/* Driver ID */
	alter_highlight(srch_win, SRCH_ROW1, SRCH_DRIVERID_COL, SRCH_DRIVERID_COL + CHARS_DRIVERID - 1, A_UNDERLINE);

	wrefresh(srch_win);
	wattrset(srch_win,0);
}


ck_vhdr_dummy(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged;
{
}


ck_vhdr_fleet(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged;
{
    int fl_index;
	
	if ( field_entered ) { 							/* user entered fleet id */

		if(read_buf[0] == ' '){						/* user deselected a fleet */
			ptr_criteria->fleet[*piu_ptr-1] = '\0';
			*piu_ptr += 1; 						/* go to next prompt position */
			return(DONE);
		}

		if ((fl_index = find_findex(read_buf[0])) == -1 ) {		/* entered invalid fleet id, trap user in field */
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CRIT_VHDR_6, "<fleet id>"));
			return(DONE);
		}

		if ((*ui_fleet_perms[fl_index])() == NOT_ALLOWED ) {	/* has permission to use ? */
			prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_CRIT_VHDR_7, "<fleet>"));
			return(DONE);						/* trap user in field */
		}

		ptr_criteria->fleet[*piu_ptr-1] = read_buf[0];			/* valid fleet */
	}

	if ( key_val == SEND_KEY ) {
		*ptr_end_unassinged = 1;					/* end all window */
		return(DONE);
	}
	else if ( key_val == '\r' || key_val == KEY_DOWN || key_val == '\n'){
			*piu_ptr += 1;
			return(DONE);
	     }
	else if ( key_val == KEY_UP) {
	    if (*piu_ptr > 1)
		*piu_ptr -= 1;
	    else
		*piu_ptr = 12; /* go to driver id field */
	    return(DONE);
	}
	else {
	     return(NOT_DONE);						/* user pressed a function key */
	     }

}
	

ck_vhdr_date(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged;
{
	char entered_date[DATE_SIZE];				/* date entered by the user */
	
	if ( field_entered ) {					 /* user entered field */

		if((verify_date(read_buf,entered_date)) == -1)  {
			prt_error(BAD_DATE,catgets(UI_m_catd, UI_1, UI_CRIT_VHDR_8, "<format is mm/dd/yy>"));
			return(INVALID);
		}
		srch_dt_tm = get_internal_time(entered_date,&ptr_criteria->starttime[0]);
		strcpy(ptr_criteria->startdate,entered_date);
		mvwprintw(srch_win,0,60, "%s ", ptr_criteria->startdate);
	}

	if ( key_val == SEND_KEY ) {
		*ptr_end_unassinged = 1;					/* end all window */
		return(DONE);
	}
	else if ( key_val == '\r' || key_val == KEY_DOWN  || key_val == '\n' ){
		*piu_ptr += 1;
		return(DONE);
             }
	else if ( key_val == KEY_UP ){
		*piu_ptr -= 1;
		return(DONE);
	     }
	else {
	     return(NOT_DONE);					/* user hit a function key */
	}
}


ck_vhdr_time(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged;
{
short loop;
	char entered_time[TIME_SIZE];					/* time entered by the user */
	
	if ( field_entered ) {						 /* user entered field */

		if((verify_time(read_buf,entered_time)) == -1)  {
			prt_error(BAD_TIME,catgets(UI_m_catd, UI_1, UI_CRIT_VHDR_9, "<format is hh:mm>"));
			return(INVALID);
		}
		srch_dt_tm = get_internal_time(&ptr_criteria->startdate[0],entered_time);
		strcpy(ptr_criteria->starttime,entered_time);
		mvwprintw(srch_win,0,85,"%s", ptr_criteria->starttime);
	}

	if ( key_val == KEY_F(1) || key_val == SEND_KEY ){
		*ptr_end_unassinged = 1;					/* end unassigned window */
		return(DONE);
	}
	else if ( key_val == '\r' || key_val == KEY_DOWN  || key_val == '\n' ){
		*piu_ptr += 1;
		return(DONE);
	     }
	else if ( key_val == KEY_UP ){
		*piu_ptr -= 1;
		return(DONE);
	     }
	else {
	     return(NOT_DONE);					/* user hit a function key */
	     }

}


ck_vhdr_number(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged;
{
	
	if ( field_entered ) { 						/* user entered field */
		strcpy(ptr_criteria->vhnumber,read_buf);
		ptr_srch_key = &cl_key4;
		cl_key_on = CL_KEY4;
	}
	if ( key_val == SEND_KEY ) {
		*ptr_end_unassinged = 1;				/* end all window */
		return(DONE);
	}
	else  if ( key_val == '\r' || key_val == KEY_DOWN  || key_val == '\n' ){
		*piu_ptr += 1;
		return(DONE);
	     }
	else if ( key_val == KEY_UP ) {
		*piu_ptr -= 1;
		return(DONE);
	     }
	else {
	     return(NOT_DONE);					/* user hit a function key */
	     }

}


ck_vhdr_id(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged;
{
	
	if ( field_entered ) { 						/* user entered field */
		strcpy(ptr_criteria->driverid,read_buf);
		ptr_srch_key = &cl_key9;
		cl_key_on = CL_KEY9;
	}

	if ( key_val == SEND_KEY ) {
		*ptr_end_unassinged = 1;				/* end all window */
		return(DONE);
	}
	else  if ( key_val == '\r' || key_val == KEY_DOWN  || key_val == '\n' ) {
		*piu_ptr = 1;
		return(DONE);
	}
	else if ( key_val == KEY_UP ) {
		*piu_ptr -= 1;
		return(DONE);
	}
	else {
	     return(NOT_DONE);					/* user hit a function key */
	     }
}
