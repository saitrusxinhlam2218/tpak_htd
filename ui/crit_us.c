static char sz__FILE__[]="@(#)crit_us.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  crit_us.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:47:56
 * @(#)  Last delta: 12/2/94 at 18:04:29
 * @(#)  SCCS File: /taxi/sccs/s.crit_us.c
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

#define		START_ASSIGNED_PROMPT		1	/* place prompt on start fleet field when reading selection criteria */

extern int scr_name;
extern struct keydesc up_key1;
extern short clear_err_flag;				/* for matches function */
extern struct keydesc *ptr_srch_key;
extern int end_unassigned_srch;
extern int matches_index;				/* for matches function */
extern struct scr_flds srch_usr_flds[];
extern WINDOW *srch_win;				/* window used for search selection criteria */
extern int upper_case;

/******************************************************************************/
/* criteria_usrdr: Displays the window with for selecting the search          */
/*			criteria. Reads the selection criteria entered        */
/*			by the call taker for usrdr calls search 	      */
/******************************************************************************/

criteria_usr(ptr_criteria)
struct select_criterias *ptr_criteria; 			/* pointer to structure for saving selection criteria */
{
	int ck_usr_dummy(), ck_usr_id(), ck_fullname();

	static int (*check_usr_flds[])() = { 
          ck_usr_dummy, ck_usr_id, ck_fullname,
	};

	int piu;					/* prompt in use; used by read_field() */
	int end_criteria_usr = 0; 			/* flag set if function should end */
	int key_val;					/* value of key entered (as returned by read_field() */
	int field_entered = 0;				/* 0=field not entered; 1=field_entered */
	int ret_ck_fld;					/* return code by check_usr_flds[]() functions */
	char read_buf[READ_BUF_SIZE];			/* buffer for reading fields */
	struct scr_flds *ptr_scr_fld;
	int should_clear = 0;

	
	disp_usr_win(ptr_criteria); 			/*  Display the window for selection of search criteria. */

	piu = START_ASSIGNED_PROMPT;			/* prompt is placed on start date */

        upper_case = 0;
        strcpy(ptr_criteria->passengername, "");
	while ( scr_name == USER_SRCH_SCR && !end_criteria_usr && !end_unassigned_srch) {
		check_mail();
		ptr_scr_fld = srch_usr_flds;

                //		if ( piu == 9  || piu == 10  )
                //			should_clear = 1;
		key_val = read_field(srch_win,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);

		if (key_val == -1)
			continue;

		if (clear_err_flag){
			clear_error();
			refresh();
		}

		wattrset(srch_win, A_UNDERLINE);
		ret_ck_fld = (*check_usr_flds[piu])(&piu,read_buf, key_val,field_entered, &select_criteria, &end_criteria_usr);
		wattrset(srch_win, 0);

		if ( ret_ck_fld != DONE )   {
			ftnkey_handler(&piu, key_val,NOT_AVAILABLE);
			if ( scr_name == USER_SRCH_SCR )
				disp_usr_win(ptr_criteria); 	/*  Display the window for selection of search criteria. */
			if ( key_val == HELP_KEY )  {
				/* ftnkey_handler() had called help(). The help window has overlayed the
				 * search window, so repaint the search window
				 */
				touchwin(srch_win);
				wrefresh(srch_win);
			}  /* end if help_key */
		}
	}

	if(scr_name != USER_SRCH_SCR) {
          upper_case = 1;
		return (NOT_DONE);
	}
	werase(srch_win);
	wrefresh(srch_win);

	refresh();
        upper_case = 1;        
	return (DONE);
}

#include "user.h"

extern int user_index;				/* index into users structure in shared memory for this user */

#define	SRCH_USER_COL	20

/**************************************************************************/
/* disp_select_win: 	displays the search selection window for the      */
/*			usr calls search, also prints any                 */
/*			default values in the fields (from user           */
/*			structure in shared memory)                       */
/**************************************************************************/

disp_usr_win(ptr_criteria)
struct select_criterias *ptr_criteria; 			/* pointer to structure for saving selection criteria */
{
	werase(srch_win);
	wattrset(srch_win, 0);
	mvwaddstr(srch_win,0,SRCH_USER_COL,catgets(UI_m_catd, UI_1, UI_CRIT_US_1, "User ID:"));
        mvwaddstr(srch_win,0,SRCH_USER_COL+16, catgets(UI_m_catd, UI_1, UI_TEXT_68, "Fullname:"));

	wrefresh(srch_win);
	wattrset(srch_win,0);
}

ck_usr_dummy(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged;
{
}

ck_usr_id(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged;
{
	if ( field_entered ) {
		strcpy(ptr_criteria->userid,read_buf); 			/* user entered field */
		*ptr_end_unassinged = 1;				/* end all window */
		ptr_srch_key = &up_key1;
		return (DONE);
	}
	if ( key_val == SEND_KEY ) {
		ptr_srch_key = &up_key1;
		*ptr_end_unassinged = 1;				/* end usr window */
		return(DONE);
	}
	else  if ( key_val == '\r' || key_val == KEY_DOWN  || key_val == '\n' ){
		*piu_ptr = 2;
		return(DONE);
	}
	else if ( key_val == KEY_UP ){
		*piu_ptr = 2;
		return(DONE);
	}
	else {
		return(NOT_DONE);
	}
}

ck_fullname(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int field_entered;
	struct select_criterias *ptr_criteria;
	int *ptr_end_unassinged;
{
	if ( field_entered ) {
          // using passengername search criteria for fullname search
		strcpy(ptr_criteria->passengername,read_buf); 			/* user entered field */
		*ptr_end_unassinged = 1;				/* end all window */
		ptr_srch_key = &up_key2;
		return (DONE);
	}
	if ( key_val == SEND_KEY ) {
		ptr_srch_key = &up_key2;
		*ptr_end_unassinged = 1;				/* end usr window */
		return(DONE);
	}
	else  if ( key_val == '\r' || key_val == KEY_DOWN  || key_val == '\n' ){
		*piu_ptr = 1;
		return(DONE);
	}
	else if ( key_val == KEY_UP ){
		*piu_ptr = 1;
		return(DONE);
	}
	else {
		return(NOT_DONE);
	}
}
