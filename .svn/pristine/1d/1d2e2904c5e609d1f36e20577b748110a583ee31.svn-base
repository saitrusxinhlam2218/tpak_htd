static char sz__FILE__[]="@(#)crit_dr.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  crit_dr.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:47:44
 * @(#)  Last delta: 12/2/94 at 18:04:21
 * @(#)  SCCS File: /taxi/sccs/s.crit_dr.c
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
#include "screens.h"
#include "ui_def.h"
#include "func_keys.h"
#include "ui_error.h"
#include "user.h"

#define		START_ASSIGNED_PROMPT		9	/* place prompt on start fleet field when reading selection criteria */
#define		SRCH_FLEET_COL			20
#define		SRCH_ID_COL			20
#define		SRCH_NAME_COL			42

extern int     scr_name;
extern struct keydesc dr_key1;
extern struct keydesc dr_key3;
extern short clear_err_flag;				/* for matches function */
extern struct keydesc *ptr_srch_key;
extern int end_unassigned_srch;
extern int matches_index;				/* for matches function */
extern struct scr_flds srch_dr_flds[];
extern WINDOW *srch_win;				/* window used for search selection criteria */
extern int user_index;				/* index into users structure in shared memory for this user */


/* declare functions for checking fleet permissions */
extern int ui_fleet_perms0(), ui_fleet_perms1(), ui_fleet_perms2(), 
	ui_fleet_perms3(), ui_fleet_perms4(), ui_fleet_perms5(), 
	ui_fleet_perms6(), ui_fleet_perms7();

/* declare array of pointers to functions for checking fleet perm's */
extern int (*ui_fleet_perms[])();

/**************************************************************************/
/* criteria_vhdr: Displays the window with for selecting the search       */
/*			criteria. Reads the selection criteria entered    */
/*			by the call taker for vhdr calls search 	  */
/**************************************************************************/
criteria_dr(ptr_criteria)
struct select_criterias *ptr_criteria; 			/* pointer to structure for saving selection criteria */
{
	int ck_dr_dummy(), ck_dr_fleet(), ck_dr_id(), ck_dr_name();

	static int (*check_dr_flds[])() = { 
		ck_dr_dummy, ck_dr_fleet, ck_dr_fleet, 
		ck_dr_fleet, ck_dr_fleet, ck_dr_fleet, 
		ck_dr_fleet, ck_dr_fleet, ck_dr_fleet, 
		ck_dr_id, ck_dr_name
	};

	int piu;					/* prompt in use; used by read_field() */
	int end_criteria_dr = 0; 			/* flag set if function should end */
	int key_val;					/* value of key entered (as returned by read_field() */
	int field_entered = 0;				/* 0=field not entered; 1=field_entered */
	int ret_ck_fld;					/* return code by check_dr_flds[]() functions */
	char read_buf[READ_BUF_SIZE];			/* buffer for reading fields */
	struct scr_flds *ptr_scr_fld;
	int should_clear = 0;

	disp_dr_win(ptr_criteria); 			/*  Display the window for selection of search criteria. */

	ptr_srch_key = &dr_key1;			/* default to search by driver ID */

	piu = START_ASSIGNED_PROMPT;			/* prompt is placed on start date */

	while ( scr_name == DRIVER_SRCH_SCR && !end_criteria_dr && !end_unassigned_srch) {
		check_mail();
		ptr_scr_fld = srch_dr_flds;
		if ( piu == 9  ||   piu == 10  )
			should_clear = 1;
		key_val = read_field(srch_win,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);

		if (key_val == -1)
			continue;

		if (clear_err_flag){
			clear_error();
			refresh();
		}

		wattrset(srch_win, A_UNDERLINE);
		ret_ck_fld = (*check_dr_flds[piu])(&piu,read_buf, key_val,field_entered, &select_criteria, &end_criteria_dr);
		wattrset(srch_win, 0);

		if ( ret_ck_fld != DONE )   {
			ftnkey_handler(&piu, key_val,NOT_AVAILABLE);
			if ( scr_name ==  DRIVER_SRCH_SCR )
				disp_dr_win(ptr_criteria); 	/*  Display the window for selection of search criteria. */
			if ( key_val == HELP_KEY )  {
				/* ftnkey_handler() had called help(). The help window has overlayed the
				 * search window, so repaint the search window
				 */
				touchwin(srch_win);
				wrefresh(srch_win);
			}  /* end if help_key */
		}
	}

	if ( scr_name != DRIVER_SRCH_SCR) {
		return (NOT_DONE);
	}
	werase(srch_win);
	wrefresh(srch_win);

	refresh();
	return (DONE);
}

/**************************************************************************/
/* disp_select_win: 	displays the search selection window for the      */
/*			dr calls search, also prints any                  */
/*			default values in the fields (from user           */
/*			structure in shared memory) 			  */
/**************************************************************************/
disp_dr_win(ptr_criteria)
struct select_criterias *ptr_criteria; 			/* pointer to structure for saving selection criteria */
{
	werase(srch_win);
	wattrset(srch_win, 0);
	mvwaddstr(srch_win,0,SRCH_FLEET_COL,catgets(UI_m_catd, UI_1, UI_CRIT_DR_1, "Fleet:"));
	mvwaddstr(srch_win,1,SRCH_ID_COL,catgets(UI_m_catd, UI_1, UI_CRIT_DR_2, "Driver ID:"));
	mvwaddstr(srch_win,1,SRCH_NAME_COL,catgets(UI_m_catd, UI_1, UI_CRIT_DR_3, "Driver Name:"));

	/* Driver ID */
	alter_highlight(srch_win, SRCH_ROW1, SRCH_DR_DRIVERID_COL, SRCH_DR_DRIVERID_COL + CHARS_DRIVERID - 1, A_UNDERLINE);

	/* Driver name */
	alter_highlight(srch_win, SRCH_ROW1, SRCH_DRVNAME_COL, SRCH_DRVNAME_COL + CHARS_DRIVERNAME - 1, A_UNDERLINE);

	fleet_srch();

	wrefresh(srch_win);
	wattrset(srch_win,0);
}

/********************************************************************************/
/* ck_dr_dummy - dummy check routine						*/
/********************************************************************************/
ck_dr_dummy(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged;
{
}

/********************************************************************************/
/* ck_dr_fleet - verify fleet entered						*/
/********************************************************************************/
ck_dr_fleet(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged;
{
    int fl_index;

	if (field_entered) { 							/* user entered fleet id */
		if(read_buf[0] == ' '){						/* user deselected a fleet */
			ptr_criteria->fleet[*piu_ptr-1] = '\0';
			*piu_ptr += 1; 						/* go to next prompt position */
			return(DONE);
		}

		if ((fl_index = find_findex(read_buf[0])) == -1) {		/* entered invalid fleet id, trap user in field */
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CRIT_DR_4, "<fleet id>"));
			return(DONE);
		}

		if ((*ui_fleet_perms[fl_index])() == NOT_ALLOWED) {	/* has permission to use ? */
			prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_CRIT_DR_5, "<fleet>"));
			return(DONE);						/* trap user in field */
		}

		ptr_criteria->fleet[*piu_ptr-1] = read_buf[0];			/* valid fleet */
	}

	if (key_val == SEND_KEY) {
		*ptr_end_unassinged = 1;					/* end all window */
		return(DONE);
	}
	else if (key_val == '\r' || key_val == KEY_DOWN || key_val == '\n'){
			*piu_ptr += 1;
			return(DONE);
	     }
	else if (key_val == KEY_UP) {
	    	if (*piu_ptr > 1)
			*piu_ptr -= 1;
		else
		    	*piu_ptr = 10; /* go to driver name field */
		return(DONE);
	     }
	else {
	     return(NOT_DONE);						/* user pressed a function key */
	     }
}

/********************************************************************************/
/* ck_dr_name - verify driver name						*/
/********************************************************************************/
ck_dr_name(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged;
{
	if (field_entered) { 						/* user entered field */
		strcpy(ptr_criteria->drivername,read_buf);
		ptr_srch_key = &dr_key3;
	}
	if ((key_val == SEND_KEY)) {
		*ptr_end_unassinged = 1;				/* end dr window */
		return(DONE);
	}
	else  if (key_val == '\r' || key_val == KEY_DOWN  || key_val == '\n') {
		*piu_ptr = 1;
		return(DONE);
	}
	else if (key_val == KEY_UP) {
		*piu_ptr -= 1;
		return(DONE);
	}
	else {
		return(NOT_DONE);
	}

}

/********************************************************************************/
/* ck_dr_id - verify driver id							*/
/********************************************************************************/
ck_dr_id(piu_ptr, read_buf, key_val, field_entered, ptr_criteria,
ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged;
{
	if (field_entered) {						/* user entered field */
		strcpy(ptr_criteria->driverid,read_buf);
		ptr_srch_key = &dr_key1;
	}
	if (key_val == SEND_KEY) {
		ptr_srch_key = &dr_key1;
		*ptr_end_unassinged = 1;					/* end dr window */
		return(DONE);
	}
	else  if (key_val == '\r' || key_val == KEY_DOWN  || key_val == '\n') {
			*piu_ptr += 1;
			return(DONE);
		}
	else if (key_val == KEY_UP) {
			*piu_ptr -= 1;
			return(DONE);
		}
	else
	    return(NOT_DONE);
}
