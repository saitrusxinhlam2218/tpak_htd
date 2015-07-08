static char sz__FILE__[]="@(#)crit_asgn.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  crit_asgn.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:47:41
 * @(#)  Last delta: 12/2/94 at 18:04:19
 * @(#)  SCCS File: /taxi/sccs/s.crit_asgn.c
 *                                                                           *
 *****************************************************************************/


#include <cursesX.h>
#include <sys/types.h>
#include "ui_strdefs.h"
#include "mads_types.h"
#include <isam.h>
#include "df_maint.h"
#include "mads_isam.h"
#include "time.h"
#include "call_entry.h"
#define SRCH_EXTERNS
#include "srch.h"
#include "srch_const.h"
#include "ui_def.h"
#include "screens.h"
#include "func_keys.h"
#include "ui_error.h"

#define		START_ASSIGNED_PROMPT		9	/* place prompt on start fleet field when reading selection criteria */

extern struct scr_flds srch_asgn_flds[];
extern struct keydesc cl_key3;
extern short clear_err_flag;
extern int     scr_name;
extern struct keydesc *ptr_srch_key;
extern int matches_index;				/* index into the fleet selection array */
extern struct fleets *fleet[];
extern char excpt_fl[];
extern int end_unassigned_srch;
extern WINDOW *srch_win;				/* window used for search selection criteria */

extern time_t get_internal_time();
extern int verify_date(), verify_time();

extern int ui_fleet_perms0(), ui_fleet_perms1(), ui_fleet_perms2(),
           ui_fleet_perms3(), ui_fleet_perms4(), ui_fleet_perms5(),
           ui_fleet_perms6(), ui_fleet_perms7();

extern int (*ui_fleet_perms[])();

extern time_t srch_dt_tm;
/**************************************************************************/
/* criteria_assigned: Displays the window with for selecting the search   */
/*			criteria. Reads the selection criteria entered    */
/*			by the call taker for assigned calls search       */
/**************************************************************************/
criteria_assigned(ptr_criteria)
struct select_criterias *ptr_criteria; 				/* pointer to structure for saving selection criteria */
{
	int ck_assigned_dummy(), ck_assigned_fleet(), 
	ck_assigned_date(), ck_assigned_time(),
	ck_assigned_street(), ck_assigned_name(),
	ck_assigned_phone1(), ck_assigned_phone2(),
	ck_assigned_charge();

								/* declare pointers to functions (which check the fields entered */
	 							/* for the searches selection criteria) */
	 							/* Note: these functions are similar to the field_check routines */
	 							/* for call entry */
	static int (*check_assigned_flds[])() = { 
		ck_assigned_dummy, ck_assigned_fleet, ck_assigned_fleet, 
		ck_assigned_fleet, ck_assigned_fleet, ck_assigned_fleet, 
		ck_assigned_fleet, ck_assigned_fleet, ck_assigned_fleet, 
		ck_assigned_date, ck_assigned_time, 
		ck_assigned_street, ck_assigned_name, ck_assigned_phone1,
		ck_assigned_phone2, ck_assigned_charge};

	int piu;						/* prompt in use; used by read_field() */
	int end_criteria_assigned = 0; 				/* flag set if function should end */
	int key_val;						/* value of key entered (as returned by read_field() */
	int field_entered = 0;					/* 0=field not entered; 1=field_entered */
	int ii;
	int ret_ck_fld;						/* return code by check_assigned_flds[]() functions */
	char read_buf[READ_BUF_SIZE];				/* buffer for reading fields */
	struct scr_flds *ptr_scr_fld;
	int should_clear = 0;

	
	disp_select_win(ptr_criteria); 				/*  Display the window for selection of search criteria. */

	srch_dt_tm = get_internal_time(&ptr_criteria->startdate[0],&ptr_criteria->starttime[0]);

	piu = START_ASSIGNED_PROMPT;				/* prompt is placed on start date */

	while ( scr_name == ASSIGNED_CALLS_SCR && !end_criteria_assigned && !end_unassigned_srch) {
		check_mail();
		ptr_scr_fld = srch_asgn_flds;

		if ( piu == 9  || piu == 10  )
			should_clear = 1;

		key_val = read_field(srch_win,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);

		if (key_val == -1)
			continue;

		if(clear_err_flag) {
			clear_error();
			refresh();
		}

		wattrset(srch_win, A_UNDERLINE);

		ret_ck_fld = (*check_assigned_flds[piu])(&piu,read_buf, key_val,field_entered, &select_criteria,
					&end_criteria_assigned);
		wattrset(srch_win, 0);

		if(ret_ck_fld == INVALID)			/* bad date/time format */
			continue;

		if ( ret_ck_fld != DONE )  {  				/* user entered a function key which is not processed by */
			ftnkey_handler(&piu, key_val,NOT_AVAILABLE); 	/* check_line_no(). So call the ftnkey_handler() */
			if ( scr_name ==  ASSIGNED_CALLS_SCR )
				disp_select_win(ptr_criteria); 	/*  Display the window for selection of search criteria. */
			if ( key_val == HELP_KEY )  {
				/* ftnkey_handler() had called help(). The help window has overlayed the
				 * search window, so repaint the search window
				 */
				touchwin(srch_win);
				wrefresh(srch_win);
			}  /* end if help_key */
		}
	}

	if ( scr_name != ASSIGNED_CALLS_SCR )
		return (NOT_DONE);
	 
	ptr_srch_key = &cl_key3;			/* use key 3 (fleet, due_date_time) for this search */

	werase(srch_win);
	wrefresh(srch_win);

	return (DONE);
}

#include "user.h"

extern int user_index;	/* index into users structure in shared memory for 
			 * this user */

#define	PROMPT_SRCH_FLEET_COL	20
#define	PROMPT_SRCH_DATE_COL	45
#define	PROMPT_SRCH_TIME_COL	70

/************************************************************************************************************/
/* disp_select_win: 	displays the search selection window for the assigned calls search, also prints any */
/*			default values in the fields (from user structure in shared memory) 		    */
/************************************************************************************************************/
disp_select_win(ptr_criteria)
struct select_criterias *ptr_criteria; 		/* pointer to structure for saving selection criteria */
{
	long srch_time;
	extern struct tm *localtime();
	struct tm *srch_tp;
	int ii,i,first_fleet;

	werase(srch_win);
	wattrset(srch_win, 0);
	mvwprintw(srch_win,0,PROMPT_SRCH_FLEET_COL,catgets(UI_m_catd, UI_1, UI_CRIT_ASGN_1, "Fleet:")); 			/* print the labels in the window */
	mvwprintw(srch_win,0,PROMPT_SRCH_DATE_COL,catgets(UI_m_catd, UI_1, UI_CRIT_ASGN_2, "Starting Date:"));
	mvwprintw(srch_win,0,PROMPT_SRCH_TIME_COL,catgets(UI_m_catd, UI_1, UI_CRIT_ASGN_3, "Starting Time"));

	fleet_srch();
									/* get the default starting date (today's date) */
	first_fleet = -1;						/* default to nothing */
	for (i=0; i<FLEET_MAX; i++) {
		if (excpt_fl[i] != NULL) {
			first_fleet = find_findex(excpt_fl[i]);		/* convert to number */
			break;
		}
	}
									/* get the default starting date (today's date) */
	if (first_fleet >= 0)
		srch_time = time(NULL) - fleet[first_fleet]->hist_srch*60;	/* valid fleet, figure out how far back */
	else
		srch_time = time(NULL) - 60*60;				/* invalid fleet, go back 1 hour */
	get_srch_date(srch_time, ptr_criteria->startdate);		/* get date to display as default */

	wattrset(srch_win, A_UNDERLINE);

	mvwprintw(srch_win,0,60, "%s", ptr_criteria->startdate);
	
	get_srch_time(srch_time, ptr_criteria->starttime);
	mvwprintw(srch_win,0,85,"%s", ptr_criteria->starttime);

	wrefresh(srch_win);
	wattrset(srch_win,0);
}


/* ck_assigned_dummy: 			*/
ck_assigned_dummy(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged;
{
}


ck_assigned_fleet(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
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
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CRIT_ASGN_4, "<fleet id>"));
			return(DONE);
		}

		if ((*ui_fleet_perms[fl_index])() == NOT_ALLOWED ) {	/* has permission to use ? */
			prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_CRIT_ASGN_5, "<fleet>"));
			return(DONE);						/* trap user in field */
		}

		ptr_criteria->fleet[*piu_ptr-1] = read_buf[0];			/* valid fleet */
	}

	if ( key_val == SEND_KEY )
	{
		*ptr_end_unassinged = 1;	/* end all window */
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
		*piu_ptr = 10; /* go to time field */
	    return(DONE);
	}
	else {
	     return(NOT_DONE);						/* user pressed a function key */
	     }

}


ck_assigned_date(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged;
{

	char entered_date[DATE_SIZE];				/* date entered by the user */
	
	if (field_entered ) {					 /* user entered field */

		if((verify_date(read_buf,entered_date)) == -1)  {
			prt_error(BAD_DATE,catgets(UI_m_catd, UI_1, UI_CRIT_ASGN_6, "<format is mm/dd/yy>"));
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


ck_assigned_time(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged;
{
short loop;
	char entered_time[TIME_SIZE];					/* time entered by the user */
	
	if (field_entered) {						 /* user entered field */

		if((verify_time(read_buf,entered_time)) == -1)  {
			prt_error(BAD_TIME,catgets(UI_m_catd, UI_1, UI_CRIT_ASGN_7, "<format is HH:MM>"));
			return(INVALID);
		}
		srch_dt_tm = get_internal_time(&ptr_criteria->startdate[0],entered_time);
		strcpy(ptr_criteria->starttime,entered_time);
		mvwprintw(srch_win,0,85,"%s", ptr_criteria->starttime);
	}

	if ( key_val == SEND_KEY ){
		*ptr_end_unassinged = 1;					/* end unassigned window */
		return(DONE);
	}
	else if ( key_val == '\r' || key_val == KEY_DOWN  || key_val == '\n' ){
		*piu_ptr = 1;
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


ck_assigned_street(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged;
{
}


ck_assigned_name(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged; 
{
}

ck_assigned_phone1(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged;
{
}

ck_assigned_charge(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged;
{
	
}

 
ck_assigned_phone2(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged;
{
}
