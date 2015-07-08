static char sz__FILE__[]="@(#)crit_all.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  crit_all.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:47:37
 * @(#)  Last delta: 12/2/94 at 18:26:50
 * @(#)  SCCS File: /taxi/sccs/s.crit_all.c
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
extern int	crit_all_on;
extern int     cl_key_on;
extern int     scr_name;
int     match_scr;
extern struct keydesc cl_key1;
extern struct keydesc cl_key3;
extern struct keydesc cl_key5;
extern struct keydesc cl_key6;
extern struct keydesc cl_key7;
extern struct keydesc cl_key8;
extern struct keydesc pl_key1;
extern struct keydesc *ptr_srch_key;
extern char req_buf[];
extern int end_unassigned_srch;
extern int matches_index;				/* for fleet index */
extern struct scr_flds srch_all_flds[];
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
criteria_all(ptr_criteria)
struct select_criterias *ptr_criteria; 			/* pointer to structure for saving selection criteria */
{
int ck_all_dummy(), ck_all_fleet(), ck_all_date(), ck_all_time(),
ck_all_street(), ck_all_name(), ck_all_charge(), ck_all_phone1(),
ck_all_phone2(), ck_all_number();

							/* declare pointers to functions (which check the fields entered */
 							/* for the searches selection criteria) */
 							/* Note: these functions are similar to the field_check routines */
 							/* for call entry */
static int (*check_all_flds[])() = { 
	ck_all_dummy, ck_all_fleet, ck_all_fleet, 
	ck_all_fleet, ck_all_fleet, ck_all_fleet, 
	ck_all_fleet, ck_all_fleet, ck_all_fleet, 
	ck_all_date, ck_all_time, 
	ck_all_street, ck_all_name, 
	ck_all_charge, ck_all_phone1,
	ck_all_phone2, ck_all_number
	};

int piu;						/* prompt in use; used by read_field() */
int end_criteria_all = 0; 				/* flag set if function should end */
int key_val;						/* value of key entered (as returned by read_field() */
int field_entered = 0;					/* 0=field not entered; 1=field_entered */
int ii;
int ret_ck_fld;						/* return code by check_all_flds[]() functions */
char read_buf[READ_BUF_SIZE];				/* buffer for reading fields */
struct scr_flds *ptr_scr_fld;
int should_clear = 0;


	disp_all_win(ptr_criteria); 			/*  Display the window for selection of search criteria. */

	srch_dt_tm = get_internal_time(&ptr_criteria->startdate[0],&ptr_criteria->starttime[0]);

	cl_key_on = CL_KEY3;				/* default key is call number */
	ptr_srch_key = &cl_key3;

	piu = START_ASSIGNED_PROMPT;			/* prompt is placed on start date */

	if (scr_name == ALL_CALLS_SCR)
		match_scr = ALL_CALLS_SCR;
	else if (scr_name == INDIVIDUAL_CALLS_SCR)
		match_scr = INDIVIDUAL_CALLS_SCR;

	while ( scr_name == match_scr && !end_criteria_all && !end_unassigned_srch)  {
		check_mail();
		ptr_scr_fld = srch_all_flds;

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
		ret_ck_fld = (*check_all_flds[piu])(&piu,read_buf, key_val,field_entered, &select_criteria, &end_criteria_all);
		wattrset(srch_win, 0);

		if(ret_ck_fld == INVALID)
			continue;

		if (ret_ck_fld != DONE) { 		/* user entered a function key which is not */
							/* processed by the check_line_no(). So call the ftnkey_handler() */
			crit_all_on = 0;
			ftnkey_handler(&piu, key_val,NOT_AVAILABLE);
			if ( scr_name == ALL_CALLS_SCR || scr_name == INDIVIDUAL_CALLS_SCR )
				disp_all_win(ptr_criteria); 	/*  Display the window for selection of search criteria. */
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
		crit_all_on = 0;
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
#define PROMPT_SRCH_NAME_COL	61
#define PROMPT_SRCH_CHRG_COL	20
#define PROMPT_SRCH_PH_COL	61
#define PROMPT_SRCH_CL_NO_COL	20

/*******************************************************************************************************/
/* disp_all_win: 	displays the search selection window for the all calls search, also prints any */
/* 			default values in the fields (from user structure in shared memory)            */
/*******************************************************************************************************/
disp_all_win(ptr_criteria)
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
	mvwaddstr(srch_win,1,PROMPT_SRCH_ST_COL,catgets(UI_m_catd, UI_1, UI_CRIT_ALL_4, "Pickup Street Name:"));
	mvwaddstr(srch_win,1,PROMPT_SRCH_NAME_COL,catgets(UI_m_catd, UI_1, UI_CRIT_ALL_5, "Passenger Name:"));
	mvwaddstr(srch_win,2,PROMPT_SRCH_CHRG_COL,catgets(UI_m_catd, UI_1, UI_CRIT_ALL_6, "Charge Number:"));
	mvwaddstr(srch_win,2,PROMPT_SRCH_PH_COL,catgets(UI_m_catd, UI_1, UI_CRIT_ALL_7, "Phone Number:      "));

	mvwaddstr(srch_win,3,PROMPT_SRCH_CL_NO_COL,catgets(UI_m_catd, UI_1, UI_CRIT_ALL_8, "Call Number:"));

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
	alter_highlight(srch_win, SRCH_ROW1, SRCH_STREET_COL, SRCH_STREET_COL + CHARS_STREETNAME - 1, A_UNDERLINE);

	/* Passenger */
	alter_highlight(srch_win, SRCH_ROW1, SRCH_PASSENGER_COL, SRCH_PASSENGER_COL + CHARS_PASSENGER - 1, A_UNDERLINE);

	/* Charge # */
	alter_highlight(srch_win, SRCH_ROW2, SRCH_CHARGE_COL, SRCH_CHARGE_COL + CHARS_CHARGENUMBER - 1, A_UNDERLINE);

	/* Phone # */
#ifdef SCANDINAVIA
	alter_highlight(srch_win, SRCH_ROW2, SRCH_PHONE_COL1, SRCH_PHONE_COL1 + CHARS_PHONE_NUMBER - 1, A_UNDERLINE);
#else
	alter_highlight(srch_win, SRCH_ROW2, SRCH_PHONE_COL1, SRCH_PHONE_COL1 + CHARS_PH_NUM1 - 1, A_UNDERLINE);
	alter_highlight(srch_win, SRCH_ROW2, SRCH_PHONE_COL2, SRCH_PHONE_COL2 + CHARS_PH_NUM2 - 1, A_UNDERLINE);
#endif
	/* Call number */
	alter_highlight(srch_win, SRCH_ROW3, SRCH_CALLNUM_COL, SRCH_CALLNUM_COL + CHARS_CALLNUMBER - 1, A_UNDERLINE);

	sprintf(&(ptr_criteria->callnumber[0]),"01");		/* for default search */
	wattrset(srch_win,0);
}


ck_all_dummy(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged;
{
	return(DONE);
}


ck_all_fleet(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
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
		*piu_ptr = 16; /* go to call number field */
	    return(DONE);
	}
	else {
	     return(NOT_DONE);						/* user pressed a function key */
	     }

}

ck_all_date(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
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


ck_all_time(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
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


ck_all_street(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged;
{
	struct cisam_pl *pl_ptr;
	struct cisam_cl tmp_call_struct;

	if (field_entered) { 					/* user entered field */
		if (key_val == ALTERNATE_KEY) {
			memcpy(&tmp_call_struct,&call_struct,sizeof(struct cisam_cl));		/* save call struct */
			strcpy(call_struct.pckup_str_name,read_buf);				/* fake in street name typed */
			if (scr_name == SUBS_FUTURE_CLS_SCR || scr_name == ALL_CALLS_SCR || scr_name == INDIVIDUAL_CALLS_SCR)
				call_struct.pckup_str_nbr = 1;            /* fake a street name */
			else {
				call_struct.pckup_str_nbr = 0;            /* fake a place name */
			}
			if (altrn_adr(NULL) > 0) {						/* display and choose alternates */
/*
/*				strcpy(ptr_criteria->streetname,call_struct.pckup_str_name);
/*				*ptr_end_unassinged = 1;					/* end all window */
/*				ptr_srch_key = &cl_key5;					/* user chose one */
/*				cl_key_on = CL_KEY5;
/*				memcpy(&call_struct,&tmp_call_struct,sizeof(struct cisam_cl));	/* restore call struct */
/*				return(DONE);
*/
			    strcpy(read_buf, call_struct.pckup_str_name);
			    mvwaddstr(srch_win, SRCH_ROW1, SRCH_STREET_COL, read_buf);

			}
			memcpy(&tmp_call_struct,&call_struct,sizeof(struct cisam_cl));		/* save call struct */
		}
    		pl_ptr = (struct cisam_pl *)req_buf;		/* pointer to placename structure */
		bzero(pl_ptr, sizeof(struct cisam_pl));
		strcpy(pl_ptr->name,read_buf);			/* fill key to retrieve placename record */

		/* If user entered an exact place name, use the street of that place name. Otherwise
		   assume user meant street name. */
		  
		/* Since place name key1 has city (and we don't) we need to to a ISGTEQ, and then check if
		   the place name returned matches the place name entered */
		if (db(PLACENM_FILE_ID,READ_KEYS,&pl_key1,ISGTEQ,0) >= 0)	/* read a record */
		  {
		    if (strcmp(pl_ptr->name, read_buf) == 0)
		      strcpy(ptr_criteria->streetname,pl_ptr->str_name);	/* is a place name; use its name */
		    else
		      /* Not an exact place name, assume user meant street name */
		      strcpy(ptr_criteria->streetname, read_buf);
		  }
		else
		  /* Not a place name, assume user meant street name */
		  strcpy(ptr_criteria->streetname,read_buf);

		ptr_srch_key = &cl_key5;
		cl_key_on = CL_KEY5;
		if ( key_val == SEND_KEY ) {
			*ptr_end_unassinged = 1;	/* end all window */
			return(DONE);
		}
	}

	if ( key_val == SEND_KEY )
	{
		*ptr_end_unassinged = 1;	/* end all window */
		return(DONE);
	}
	else  	if ( key_val == '\r' || key_val == KEY_DOWN  || key_val == '\n' ) {
		   *piu_ptr += 1;
		   return(DONE);
	}
	else 	if ( key_val == KEY_UP ) {
		   *piu_ptr -= 1;
		   return(DONE);
	}
	else	if (key_val == ALTERNATE_KEY) {
					if ( !field_entered ) {
						memcpy(&tmp_call_struct,&call_struct,sizeof(struct cisam_cl));    /* save call struct */
						call_struct.pckup_str_name[0] = '\0';         /* fake in nothing */
						call_struct.pckup_str_nbr = 0;            /* fake a place name */
						if (altrn_adr(NULL) > 0) {            /* display and choose alternates */
							strcpy(ptr_criteria->streetname,call_struct.pckup_str_name);
							*ptr_end_unassinged = 1;          /* end all window */
							ptr_srch_key = &cl_key5;          /* user chose one */
							cl_key_on = CL_KEY5;
							memcpy(&call_struct,&tmp_call_struct,sizeof(struct cisam_cl));  /* restore call struct */
							return(DONE);
						}
						memcpy(&call_struct,&tmp_call_struct,sizeof(struct cisam_cl));    /* restore call struct */
					} else {
						*ptr_end_unassinged = 1;  /* end all window */
						return(DONE);
					}
				}
	return(NOT_DONE);
}


ck_all_name(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged; 
{
	
	if ( field_entered ) { 						/* user entered field */
		strcpy(ptr_criteria->passengername,read_buf);
		ptr_srch_key = &cl_key6;
		cl_key_on = CL_KEY6;
		crit_all_on = 1;
	}

	if ( key_val == SEND_KEY ) {
		*ptr_end_unassinged = 1;				/* end all window */
		return(DONE);
	}
	else  	if ( key_val == '\r' || key_val == KEY_DOWN  || key_val == '\n' ) {
		   *piu_ptr += 1;
		   return(DONE);
		}
	else 	if ( key_val == KEY_UP ) {
		   *piu_ptr -= 1;
		   return(DONE);
		}
	else {
		return(NOT_DONE);
	}
}

ck_all_phone1(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged;
{
	int  i, j, len;
	
	if ( field_entered ) { 					/* user entered field */
#ifdef SCANDINAVIA

	  /* Strip out any non alphanumeric characters */
	  len = strlen(read_buf);
	  for (i = 0, j = 0; i < len; i ++)
	    if (isalnum(read_buf[i]))
	      ptr_criteria->phonenumber[j ++] = read_buf[i];
	  ptr_criteria->phonenumber[j] = '\0';
	  ptr_srch_key = &cl_key7;
	  cl_key_on = CL_KEY7;
	  crit_all_on = 1;
#else
		strcpy(ptr_criteria->phonenumber1,read_buf);
#endif
	}

	if ( key_val == SEND_KEY ) {
		*ptr_end_unassinged = 1;			/* end all window */
		return(DONE);
	}
	else  	if ( key_val == '\r' || key_val == KEY_DOWN  || key_val == '\n' ) {
#ifdef SCANDINAVIA
		   *piu_ptr += 2; /* skip phone2 field */
#else
		   *piu_ptr += 1;
#endif
		   return(DONE);
		}
	else 	if ( key_val == KEY_UP ) {
		   *piu_ptr -= 1;
		   return(DONE);
		}
	else if ( key_val == OK )
		return(DONE);
	else {
		return(NOT_DONE);
	}
}

ck_all_phone2(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged;
{
char temp_phonenumber[20];

	if ( field_entered ) { 						/* user entered field */
		strcpy(ptr_criteria->phonenumber2,read_buf);
		ptr_srch_key = &cl_key7;
		cl_key_on = CL_KEY7;
		crit_all_on = 1;
		strncpy(temp_phonenumber,ptr_criteria->phonenumber1,3);
		strcpy(&(temp_phonenumber[3]),ptr_criteria->phonenumber2);
		temp_phonenumber[7] = '\0';
		strcpy(ptr_criteria->phonenumber, temp_phonenumber);
	}

	if ( key_val == SEND_KEY ) {
		*ptr_end_unassinged = 1;				/* end all window */
		return(DONE);
	}
	else  	if ( key_val == '\r' || key_val == KEY_DOWN  || key_val == '\n' ) {
		   *piu_ptr += 1;
		   return(DONE);
		}
	else 	if ( key_val == KEY_UP ) {
		   *piu_ptr -= 1;
		   return(DONE);
		}
	else {
		return(NOT_DONE);
	}
}

ck_all_charge(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged;
{
char temp_buf[21];
char *temp_buf_ptr;
	
	if ( field_entered ) { 					/* user entered field */
		temp_buf_ptr = &(temp_buf[0]);
		while (*read_buf != '\0') {
			if (*read_buf != '-' && *read_buf != ' ') {
				*temp_buf_ptr = *read_buf;
				temp_buf_ptr++;
			}
			read_buf++;
		}
		strcpy(ptr_criteria->chargenumber,temp_buf);
		ptr_srch_key = &cl_key8;
		cl_key_on = CL_KEY8;
		crit_all_on = 1;
	}

	if ( key_val == SEND_KEY ) {
		*ptr_end_unassinged = 1;			/* end all window */
		return(DONE);
	}
	else  	if ( key_val == '\r' || key_val == KEY_DOWN  || key_val == '\n' ) {
		   *piu_ptr += 1;
		   return(DONE);
		}
	else 	if ( key_val == KEY_UP ) {
		   *piu_ptr -= 1;
		   return(DONE);
		}
	else {
		return(NOT_DONE);
	}
}


ck_all_number(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged;
{
	int	callnbr;
	if(field_entered) { 					/* user entered field */
		callnbr = atoi(read_buf);
		if(basecall)
			if((basecall % 1000000) > 900000 && (callnbr % 1000000) < 100000)
				callnbr += ((basecall / 1000000) * 1000000) + 1000000;
			else if((basecall % 1000000) < 100000 && (callnbr % 1000000) > 900000)
				callnbr += ((basecall / 1000000) * 1000000) - 1000000;
			else
				callnbr += (basecall / 1000000) * 1000000;
			
		sprintf(ptr_criteria->callnumber,"%d", callnbr);
		ptr_srch_key = &cl_key1;
		cl_key_on = CL_KEY1;
		crit_all_on = 1;
	}

	if ( key_val == SEND_KEY ) {
		*ptr_end_unassinged = 1;			/* end all window */
		return(DONE);
	}
	else  	if ( key_val == '\r' || key_val == KEY_DOWN  || key_val == '\n' ) {
		   *piu_ptr = 1;
		   return(DONE);
		}
	else 	if ( key_val == KEY_UP ) {
#ifdef SCANDINAVIA
		   *piu_ptr -= 2; /* skip past phone2 field */
#else
		   *piu_ptr -= 1;
#endif
		   return(DONE);
		}
	else {
		return(NOT_DONE);
	}
}
