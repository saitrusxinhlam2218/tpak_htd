/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: crit_zn.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1997/12/13 14:36:18 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/ui/crit_zn.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: crit_zn.c,v 1.3 1997/12/13 14:36:18 taxiadm Exp $";


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
#include <sys/types.h>
#include <isam.h>
#include <time.h>
#include <ctype.h>

#include "mads_types.h"
#include "df_maint.h"
#include "mads_isam.h"
#include "call_entry.h"
#define SRCH_EXTERNS
#include "srch.h"
#include "srch_const.h"
#include "ui_def.h"
#include "screens.h"
#include "func_keys.h"
#include "ui_error.h"
#include "ui_strdefs.h"

#define		START_ASSIGNED_PROMPT		11

extern	int	basecall;
extern int     cl_key_on;
extern int     scr_name;
int     match_scr;
extern struct keydesc cl_key1;
extern struct keydesc cl_key3;
extern struct keydesc cl_key5;
extern struct keydesc cl_key6;
extern struct keydesc cl_key7;
extern struct keydesc cl_key8;
extern struct keydesc cl_key11;
extern struct keydesc pl_key1;
extern struct keydesc *ptr_srch_key;
extern char req_buf[];
extern int end_unassigned_srch;
extern int matches_index;				/* for fleet index */
extern struct scr_flds srch_zn_flds[];
extern struct cisam_cl call_struct;			/* for call record storage */
extern struct fleets *fleet[];
extern char excpt_fl[];
extern WINDOW *srch_win;				/* window used for search selection criteria */

extern time_t get_internal_time();
extern int verify_date(), verify_time();

extern int ui_fleet_perms0(), ui_fleet_perms1(), ui_fleet_perms2(),
           ui_fleet_perms3(), ui_fleet_perms4(), ui_fleet_perms5(),
           ui_fleet_perms6(), ui_fleet_perms7();

extern int (*ui_fleet_perms[])();

extern time_t srch_dt_tm;
extern short clear_err_flag;

/************************************************************************/
/* criteria_all: Displays the window with for selecting the search      */
/*		criteria. Reads the selection criteria entered          */
/*		by the call taker for all calls search                  */
/************************************************************************/
criteria_zn(ptr_criteria)
struct select_criterias *ptr_criteria; 			/* pointer to structure for saving selection criteria */
{
int ck_zn_dummy(), ck_zn_fleet(), ck_zn_date(), ck_zn_time(),
ck_zn_zone();
							/* declare pointers to functions (which check the fields entered */
 							/* for the searches selection criteria) */
 							/* Note: these functions are similar to the field_check routines */
 							/* for call entry */
static int (*check_zn_flds[])() = { 
	ck_zn_dummy, ck_zn_fleet, ck_zn_fleet, 
	ck_zn_fleet, ck_zn_fleet, ck_zn_fleet, 
	ck_zn_fleet, ck_zn_fleet, ck_zn_fleet, 
	ck_zn_date, ck_zn_time, 
	ck_zn_zone
	};

int piu;						/* prompt in use; used by read_field() */
int end_criteria_zn = 0; 				/* flag set if function should end */
int key_val;						/* value of key entered (as returned by read_field() */
int field_entered = 0;					/* 0=field not entered; 1=field_entered */
int ii;
int ret_ck_fld;						/* return code by check_all_flds[]() functions */
char read_buf[READ_BUF_SIZE];				/* buffer for reading fields */
struct scr_flds *ptr_scr_fld;
int should_clear = 0;


	disp_zn_win(ptr_criteria); 			/*  Display the window for selection of search criteria. */

	srch_dt_tm = get_internal_time(&ptr_criteria->startdate[0],&ptr_criteria->starttime[0]);

	cl_key_on = CL_KEY11;				/* default key is call number */
	ptr_srch_key = &cl_key11;

        piu = START_ASSIGNED_PROMPT;			/* prompt is placed on start date */

        match_scr = SRCH_USER_DEF1;


	while ( scr_name == match_scr && !end_criteria_zn && !end_unassigned_srch)  {
		check_mail();
		ptr_scr_fld = srch_zn_flds;

		if ( piu == 9  ||  piu == 10 )
			/* date or time field entry; should clear field on entry of 1st char */
			should_clear = 1;

		key_val = read_field(srch_win,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);

		if (key_val == -1)
			continue;

		if(clear_err_flag){
			clear_error();
			refresh();
		}

		wattrset(srch_win, A_UNDERLINE);
		ret_ck_fld = (*check_zn_flds[piu])(&piu,read_buf, key_val,field_entered, &select_criteria, &end_criteria_zn);
		wattrset(srch_win, 0);

		if(ret_ck_fld == INVALID)
			continue;

		if (ret_ck_fld != DONE) { 		/* user entered a function key which is not */
							/* processed by the check_line_no(). So call the ftnkey_handler() */
			ftnkey_handler(&piu, key_val,NOT_AVAILABLE);
			if ( scr_name == SRCH_USER_DEF1 )
				disp_zn_win(ptr_criteria); 	/*  Display the window for selection of search criteria. */
			if ( key_val == HELP_KEY )  {
				/* ftnkey_handler() had called help(). The help window has overlayed the
				 * search window, so repaint the search window
				 */
				touchwin(srch_win);
				wrefresh(srch_win);
			}  /* end if help_key */
		}
	}

	if (scr_name != match_scr) {
		return (NOT_DONE);
	}

	werase(srch_win);
	wrefresh(srch_win);

	refresh();
	return (DONE);
}

#include "user.h"

extern int user_index;			/* index into users structure in shared memory for this user */

#define	PROMPT_SRCH_FLEET_COL	20
#define	PROMPT_SRCH_DATE_COL	45
#define	PROMPT_SRCH_TIME_COL	70
#define PROMPT_SRCH_ST_COL	20
#define PROMPT_SRCH_ZN_COL      20
#define PROMPT_SRCH_NAME_COL	61
#define PROMPT_SRCH_CHRG_COL	20
#define PROMPT_SRCH_PH_COL	61
#define PROMPT_SRCH_CL_NO_COL	20

/*******************************************************************************************************/
/* disp_all_win: 	displays the search selection window for the all calls search, also prints any */
/* 			default values in the fields (from user structure in shared memory)            */
/*******************************************************************************************************/
disp_zn_win(ptr_criteria)
struct select_criterias *ptr_criteria; 		/* pointer to structure for saving selection criteria */
{
long srch_time;
struct tm *srch_tp;
int ii,i,first_fleet;


        wattrset(srch_win, 0);
	werase(srch_win);
	wrefresh(srch_win);

	mvwaddstr(srch_win,0,PROMPT_SRCH_FLEET_COL,catgets(UI_m_catd, UI_1, UI_CRIT_ALL_1, "Fleet:")); 		/* print the labels in the window */
	mvwaddstr(srch_win,0,PROMPT_SRCH_DATE_COL,catgets(UI_m_catd, UI_1, UI_CRIT_ALL_2, "Starting Date:"));
	mvwaddstr(srch_win,0,PROMPT_SRCH_TIME_COL,catgets(UI_m_catd, UI_1, UI_CRIT_ALL_3, "Starting Time:"));
	mvwaddstr(srch_win,1,PROMPT_SRCH_ZN_COL, "Zone" );

	fleet_srch();

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

	wattrset(srch_win, A_UNDERLINE);
	get_srch_date(srch_time, ptr_criteria->startdate);		/* get date to display as default */
	mvwprintw(srch_win,0,60, "%s", ptr_criteria->startdate);
	
	get_srch_time(srch_time, ptr_criteria->starttime);
	mvwprintw(srch_win,0,85,"%s", ptr_criteria->starttime);

	/* pickup street */
	alter_highlight(srch_win, SRCH_ROW1, SRCH_ZONE_NBR_COL, SRCH_ZONE_NBR_COL + CHARS_ZONE_NBR - 1, A_UNDERLINE);

	wattrset(srch_win,0);
}


ck_zn_dummy(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged;
{
	return(DONE);
}


ck_zn_fleet(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
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
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CRIT_ALL_9, "<fleet id>"));
			return(DONE);
		}

		if ((*ui_fleet_perms[fl_index])() == NOT_ALLOWED ) {	/* has permission to use ? */
			prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_CRIT_ALL_10, "<fleet>"));
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
		*piu_ptr = 11;
	    return(DONE);
	}
	else {
	     return(NOT_DONE);						/* user pressed a function key */
	     }

}

ck_zn_date(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged;
{
	char entered_date[DATE_SIZE];

	if ( field_entered ) { 						/* user entered field */
		if((verify_date(read_buf,entered_date)) == -1) {
			prt_error(BAD_DATE,catgets(UI_m_catd, UI_1, UI_CRIT_ALL_11, "<format is mm/dd/yy>"));
			return(INVALID);
		}
		srch_dt_tm = get_internal_time(entered_date,&ptr_criteria->starttime[0]);
		strcpy(ptr_criteria->startdate,entered_date);
		mvwprintw(srch_win,0,60, "%s ", ptr_criteria->startdate);
}

	if ( key_val == SEND_KEY ) {
		*ptr_end_unassinged = 1;				/* end all window */
		return(DONE);
	}

	else  	if ( key_val == '\r' || key_val == KEY_DOWN  || key_val == '\n' ){
		   *piu_ptr += 1;
		   return(DONE);
		}

	else 	if ( key_val == KEY_UP ){
		   *piu_ptr -= 1;
		   return(DONE);
		}
	else {
		return(NOT_DONE);
	}
}

ck_zn_zone(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged;
{
  if ( field_entered )
    {
      if ( atoi(read_buf) <= 0 )
	{
	  ptr_criteria->zone_nbr[0] = '\0';
	  *piu_ptr += 1;
	  return(DONE);
	}
      strcpy( ptr_criteria->zone_nbr, read_buf );
    }

  if ( key_val == SEND_KEY )
    {
      *ptr_end_unassinged = 1;	/* end all window */
      return(DONE);
    }
  else if ( key_val == '\r' || key_val == KEY_DOWN || key_val == '\n')
    {
    *piu_ptr = 1;
    return(DONE);
  }
  else if ( key_val == KEY_UP) {
    if (*piu_ptr > 1)
      *piu_ptr -= 1;
    else
      *piu_ptr = 11; /* go to zone field */
    return(DONE);
  }
  else {
    return(NOT_DONE);						/* user pressed a function key */
  }
}

ck_zn_time(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged;
{
	short loop;
	char entered_time[TIME_SIZE];

	if ( field_entered ) { 					/* user entered field */
		if((verify_time(read_buf,entered_time)) == -1) {
			prt_error(BAD_TIME,catgets(UI_m_catd, UI_1, UI_CRIT_ALL_12, "<format is HH:MM>"));
			return(INVALID);
		}
		srch_dt_tm = get_internal_time(&ptr_criteria->startdate[0],entered_time);
		strcpy(ptr_criteria->starttime,entered_time);
		mvwprintw(srch_win,0,85,"%s", ptr_criteria->starttime);
	}

	if ( key_val == SEND_KEY ) {
		*ptr_end_unassinged = 1;				/* end all window */
		return(DONE);
	}
	else  	if ( key_val == '\r' || key_val == KEY_DOWN  || key_val == '\n' ){
		   *piu_ptr += 1;
		   return(DONE);
		}
	else 	if ( key_val == KEY_UP ){
		   *piu_ptr -= 1;
		   return(DONE);
		}
	else {
		return(NOT_DONE);
	}
}


