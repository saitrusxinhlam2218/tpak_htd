static char sz__FILE__[]="@(#)crit_vh.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  crit_vh.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:48:00
 * @(#)  Last delta: 12/2/94 at 18:04:31
 * @(#)  SCCS File: /taxi/sccs/s.crit_vh.c
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

#define		START_ASSIGNED_PROMPT		9	/* place prompt on start fleet field when reading selection criteria */

extern int scr_name;
extern struct keydesc veh_key2;
extern short clear_err_flag;				/* for matches function */
extern struct keydesc *ptr_srch_key;
extern int end_unassigned_srch;
extern int matches_index;				/* for matches function */
extern struct scr_flds srch_vh_flds[];
extern WINDOW *srch_win;				/* window used for search selection criteria */


/* declare functions for checking fleet permissions */
extern int ui_fleet_perms0(), ui_fleet_perms1(), ui_fleet_perms2(), 
	ui_fleet_perms3(), ui_fleet_perms4(), ui_fleet_perms5(), 
	ui_fleet_perms6(), ui_fleet_perms7();

/* declare array of pointers to functions for checking fleet perm's */
extern int (*ui_fleet_perms[])();

/**************************************************************************/
/* criteria_vhdr: Displays the window with for selecting the search       */
/*			criteria. Reads the selection criteria entered    */
/*			by the call taker for vhdr calls search           */
/**************************************************************************/

criteria_vh(ptr_criteria)
struct select_criterias *ptr_criteria; 			/* pointer to structure for saving selection criteria */
{
	int ck_vh_dummy(), ck_vh_fleet(), ck_vh_number();

	static int (*check_vh_flds[])() = { 
		ck_vh_dummy, ck_vh_fleet, ck_vh_fleet, 
		ck_vh_fleet, ck_vh_fleet, ck_vh_fleet, 
		ck_vh_fleet, ck_vh_fleet, ck_vh_fleet, ck_vh_number
	};

	int piu;					/* prompt in use; used by read_field() */
	int end_criteria_vh = 0; 			/* flag set if function should end */
	int key_val;					/* value of key entered (as returned by read_field() */
	int field_entered = 0;				/* 0=field not entered; 1=field_entered */
	int ret_ck_fld;					/* return code by check_vh_flds[]() functions */
	char read_buf[READ_BUF_SIZE];			/* buffer for reading fields */
	struct scr_flds *ptr_scr_fld;
	int should_clear = 0;

	
	disp_vh_win(ptr_criteria); 			/*  Display the window for selection of search criteria. */

	ptr_srch_key = &veh_key2;			/* default to vehicle number search */

	piu = START_ASSIGNED_PROMPT;			/* prompt is placed on start date */

	while ( scr_name == VEHICLE_SRCH_SCR && !end_criteria_vh && !end_unassigned_srch) {
		check_mail();
		ptr_scr_fld = srch_vh_flds;

		if ( piu == 9  ||  piu == 10  )
			should_clear = 1;
		key_val = read_field(srch_win,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);

		if (key_val == -1)
			continue;

		if (clear_err_flag){
			clear_error();
			refresh();
		}

		wattrset(srch_win, A_UNDERLINE);
		ret_ck_fld = (*check_vh_flds[piu])(&piu,read_buf, key_val,field_entered, &select_criteria, &end_criteria_vh);
		wattrset(srch_win, 0);

		if ( ret_ck_fld != DONE )   {
			ftnkey_handler(&piu, key_val,NOT_AVAILABLE);
			if ( scr_name == VEHICLE_SRCH_SCR )
				disp_vh_win(ptr_criteria); /*  Display the window for selection of search criteria. */
			if ( key_val == HELP_KEY )  {
				/* ftnkey_handler() had called help(). The help window has overlayed the
				 * search window, so repaint the search window
				 */
				touchwin(srch_win);
				wrefresh(srch_win);
			}  /* end if help_key */
		}
	} 

	if( scr_name != VEHICLE_SRCH_SCR)
		return (NOT_DONE);
	
	werase(srch_win);
	wrefresh(srch_win);

	refresh();
	return (DONE);
}

#include "user.h"

extern int user_index;	/* index into users structure in shared memory for 
			 * this user */

#define	PROMPT_SRCH_FLEET_COL	20
#define	PROMPT_SRCH_VHNUMBER_COL	48

/**************************************************************************/
/* disp_select_win: 	displays the search selection window for the      */
/*			vh calls search, also prints any                  */
/*			default values in the fields (from user           */
/*			structure in shared memory) 		          */
/**************************************************************************/

disp_vh_win(ptr_criteria)
struct select_criterias *ptr_criteria; 				/* pointer to structure for saving selection criteria */
{
	werase(srch_win);
	wattrset(srch_win, 0);
	mvwaddstr(srch_win,0,PROMPT_SRCH_FLEET_COL,catgets(UI_m_catd, UI_1, UI_CRIT_VH_1, "Fleet:"));
	mvwaddstr(srch_win,0,PROMPT_SRCH_VHNUMBER_COL,catgets(UI_m_catd, UI_1, UI_CRIT_VH_2, "Vehicle #:"));

	fleet_srch();

	alter_highlight(srch_win, 0, SRCH_VH_VHNUMBER_COL, SRCH_VH_VHNUMBER_COL + CHARS_VHNUMBER - 1, A_UNDERLINE);

	wrefresh(srch_win);
	wattrset(srch_win,0);
}

ck_vh_dummy(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged;
{
}

ck_vh_fleet(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
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
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CRIT_VH_3, "<fleet id>"));
			return(DONE);
		}

		if ((*ui_fleet_perms[fl_index])() == NOT_ALLOWED ) {	/* has permission to use ? */
			prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_CRIT_VH_4, "<fleet>"));
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
	else if ( key_val == KEY_UP ) {
		if (*piu_ptr > 1)
			*piu_ptr -= 1;
		else
		        *piu_ptr = 9; /* to vehicle # field */
		return(DONE);
	     }
	else {
	     return(NOT_DONE);						/* user pressed a function key */
	     }
}

ck_vh_number(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged;
{
	if ( field_entered ) { 						/* user entered field */
		strcpy(ptr_criteria->vhnumber,read_buf);
		ptr_srch_key = &veh_key2;
	}

	if ( key_val == SEND_KEY ) {
		ptr_srch_key = &veh_key2;
		*ptr_end_unassinged = 1;				/* end vh window */
		return(DONE);
	}
	else  if ( key_val == '\r' || key_val == KEY_DOWN  || key_val == '\n' ){
		*piu_ptr = 1;
		return(DONE);
	}
	else if ( key_val == KEY_UP ){
		*piu_ptr -= 1;
		return(DONE);
	}
	else {
		return(NOT_DONE);
	}
}
