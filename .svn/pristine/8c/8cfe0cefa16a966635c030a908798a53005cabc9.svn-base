static char sz__FILE__[]="@(#)crit_sf.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  crit_sf.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:47:48
 * @(#)  Last delta: 12/2/94 at 18:04:23
 * @(#)  SCCS File: /taxi/sccs/s.crit_sf.c
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
#include <sys/types.h>
#include <isam.h>

#include "mads_types.h"
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
#include "ui_strdefs.h"

#define		START_ASSIGNED_PROMPT		11

int	crit_sf_on;
extern int     sc_key_on;
extern int     scr_name;
extern struct keydesc sc_key1;
extern struct keydesc sc_key3;
extern struct keydesc sc_key4;
extern struct keydesc sc_key5;
extern struct keydesc sc_key6;
extern struct keydesc pl_key1;
extern struct keydesc *ptr_srch_key;
extern char req_buf[];
extern int end_unassigned_srch;
extern int matches_index;				/* for fleet index */
extern struct scr_flds srch_sf_flds[];
extern struct cisam_cl call_struct;
extern WINDOW *srch_win;				/* window used for search selection criteria */


extern int ui_fleet_perms0(), ui_fleet_perms1(), ui_fleet_perms2(),
           ui_fleet_perms3(), ui_fleet_perms4(), ui_fleet_perms5(),
           ui_fleet_perms6(), ui_fleet_perms7();

extern int (*ui_fleet_perms[])();

extern short clear_err_flag;

/************************************************************************/
/* criteria_sf: Displays the window with for selecting the search       */
/*		criteria. Reads the selection criteria entered          */
/*		by the call taker for all calls search                  */
/************************************************************************/
criteria_sf(ptr_criteria)
struct select_criterias *ptr_criteria; 			/* pointer to structure for saving selection criteria */
{
int ck_sf_dummy(), ck_sf_fleet(), ck_sf_street(), ck_sf_name(), ck_sf_charge(), ck_sf_phone1(), ck_sf_phone2(), ck_sf_number();

							/* declare pointers to functions (which check the fields entered */
 							/* for the searches selection criteria) */
 							/* Note: these functions are similar to the field_check routines */
 							/* for call entry */
static int (*check_sf_flds[])() = { 
	ck_sf_dummy, ck_sf_fleet, ck_sf_fleet, 
	ck_sf_fleet, ck_sf_fleet, ck_sf_fleet, 
	ck_sf_fleet, ck_sf_fleet, ck_sf_fleet, 
	ck_sf_street, ck_sf_name, 
	ck_sf_charge, ck_sf_phone1,
	ck_sf_phone2, ck_sf_number
	};

int piu;						/* prompt in use; used by read_field() */
int end_criteria_sf = 0; 				/* flag set if function should end */
int key_val;						/* value of key entered (as returned by read_field() */
int field_entered = 0;					/* 0=field not entered; 1=field_entered */
int ii;
int ret_ck_fld;						/* return code by check_all_flds[]() functions */
char read_buf[READ_BUF_SIZE];				/* buffer for reading fields */
struct scr_flds *ptr_scr_fld;
int should_clear = 0;

	disp_sf_win(ptr_criteria); 			/*  Display the window for selection of search criteria. */

	sc_key_on = SC_KEY1;				/* default key is call number */
	ptr_srch_key = &sc_key1;

	piu = START_ASSIGNED_PROMPT - 2;		/* prompt is placed on pickup street name */

	while ( scr_name == SUBS_FUTURE_CLS_SCR && !end_criteria_sf && !end_unassigned_srch)  {
		check_mail();
		ptr_scr_fld = srch_sf_flds;

		key_val = read_field(srch_win,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);

		if (key_val == -1)
			continue;

		if(clear_err_flag){
			clear_error();
			refresh();
		}

		wattrset(srch_win, A_UNDERLINE);
		ret_ck_fld = (*check_sf_flds[piu])(&piu,read_buf, key_val,field_entered, &select_criteria, &end_criteria_sf);
		wattrset(srch_win, 0);

		if(ret_ck_fld == INVALID)
			continue;

		if (ret_ck_fld != DONE) { 		/* user entered a function key which is not */
							/* processed by the check_line_no(). So call the ftnkey_handler() */
			crit_sf_on = 0;
			ftnkey_handler(&piu, key_val,NOT_AVAILABLE);
			if ( scr_name == SUBS_FUTURE_CLS_SCR )
				disp_sf_win(ptr_criteria); 	/*  Display the window for selection of search criteria. */
			if ( key_val == HELP_KEY )  { 	/* ftnkey_handler() had called help(). The help window has overlayed the */
				 			/* search window, so repaint the search window */
				touchwin(srch_win);
				wrefresh(srch_win);
			}
		}
	}

	if (scr_name != SUBS_FUTURE_CLS_SCR) {
		crit_sf_on = 0;
		return (NOT_DONE);
	}

	werase(srch_win);
	wrefresh(srch_win);

	refresh();
	return (DONE);
}

#include "user.h"

extern int user_index;			/* index into users structure in shared memory for this user */

#define	SRCH_FLEET_COL	20
#define SRCH_ST_COL	20
#define SRCH_NAME_COL	61
#define SRCH_CHRG_COL	20
#define SRCH_PH_COL	61
#define SRCH_CL_NO_COL	20

/*******************************************************************************************************/
/* disp_sf_win: 	displays the search selection window for the all calls search, also prints any */
/* 			default values in the fields (from user structure in shared memory)            */
/*******************************************************************************************************/
disp_sf_win(ptr_criteria)
struct select_criterias *ptr_criteria; 		/* pointer to structure for saving selection criteria */
{
long srch_time;
extern struct tm *localtime();
struct tm *srch_tp;
int ii;

	werase(srch_win);
	wrefresh(srch_win);
	wattrset(srch_win, 0);
	mvwaddstr(srch_win,0,SRCH_FLEET_COL,catgets(UI_m_catd, UI_1, UI_CRIT_SF_1, "Fleet:")); 		/* print the labels in the window */
	mvwaddstr(srch_win,1,SRCH_ST_COL,catgets(UI_m_catd, UI_1, UI_CRIT_SF_2, "Pickup Street Name:"));
	mvwaddstr(srch_win,1,SRCH_NAME_COL,catgets(UI_m_catd, UI_1, UI_CRIT_SF_3, "Passenger Name:"));
	mvwaddstr(srch_win,2,SRCH_CHRG_COL,catgets(UI_m_catd, UI_1, UI_CRIT_SF_4, "Charge Number:"));
	mvwaddstr(srch_win,2,SRCH_PH_COL,catgets(UI_m_catd, UI_1, UI_CRIT_SF_5, "Phone Number:     -"));
#ifdef SCANDINAVIA
	/* Kludge to wipe out the '-' in phone # for international version */
	mvwaddstr(srch_win,2,SRCH_PH_COL + 18, " ");
#endif
	mvwaddstr(srch_win,3,SRCH_CL_NO_COL,catgets(UI_m_catd, UI_1, UI_CRIT_SF_6, "Call Number:"));

	fleet_srch();
									/* get the default starting date (today's date) */
	sprintf(&(ptr_criteria->callnumber[0]),"01");			/* for default search */

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

	wattrset(srch_win,0);
}


ck_sf_dummy(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged;
{
	return(DONE);
}


ck_sf_fleet(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
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
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CRIT_SF_7, "<fleet id>"));
			return(DONE);
		}

		if ((*ui_fleet_perms[fl_index])() == NOT_ALLOWED ) {	/* has permission to use ? */
			prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_CRIT_SF_8, "<fleet>"));
			return(DONE);						/* trap user in field */
		}

		ptr_criteria->fleet[*piu_ptr-1] = read_buf[0];			/* valid fleet */
	}

	if ( key_val == SEND_KEY ) {
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
		*piu_ptr = 14; /* go to call number field */
	    return(DONE);
	}
	else {
	     return(NOT_DONE);						/* user pressed a function key */
	     }
}


ck_sf_street(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
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
			memcpy(&tmp_call_struct,&call_struct,sizeof(struct cisam_cl));    /* save call struct */
			strcpy(call_struct.pckup_str_name,read_buf);				/* fake in street name typed */
			if (scr_name == SUBS_FUTURE_CLS_SCR || scr_name == ALL_CALLS_SCR || scr_name == INDIVIDUAL_CALLS_SCR)
				/* in these cases we want to look up alternative street names, not place names */
				call_struct.pckup_str_nbr = 1;            /* fake a street name */
			else {
				call_struct.pckup_str_nbr = 0;            /* fake a place name */
			}
			if (altrn_adr(NULL) > 0) {						/* display and choose alternates */
				strcpy(ptr_criteria->streetname,call_struct.pckup_str_name);
				ptr_srch_key = &sc_key3;
				sc_key_on = SC_KEY3;
				crit_sf_on = 1;
				memcpy(&call_struct,&tmp_call_struct,sizeof(struct cisam_cl));	/* restore call struct */
				*ptr_end_unassinged = 1;  /* usr made his choise */
				return(DONE);
			}
			memcpy(&tmp_call_struct,&call_struct,sizeof(struct cisam_cl));		/* save call struct */
		}
    		pl_ptr = (struct cisam_pl *)req_buf;		/* pointer to placename structure */
		strcpy(pl_ptr->name,read_buf);			/* fill key to retrieve placename record */
		if (db(PLACENM_FILE_ID,READ_KEYS,&pl_key1,ISEQUAL,0) < 0)	/* read a record */
			strcpy(ptr_criteria->streetname,read_buf);		/* not a place name; user what was typed in */
		else
			strcpy(ptr_criteria->streetname,pl_ptr->str_name);	/* is a place name; use street name */

		ptr_srch_key = &sc_key3;
		sc_key_on = SC_KEY3;
		crit_sf_on = 1;
	}
	if ( key_val == SEND_KEY ) {
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
			memcpy(&tmp_call_struct,&call_struct,sizeof(struct cisam_cl));		/* save call struct */
			call_struct.pckup_str_name[0] = '\0';					/* fake in nothing */
			call_struct.pckup_str_nbr = 0;						/* fake a place name */
			if (altrn_adr(NULL) > 0) {						/* display and choose alternates */
				strcpy(ptr_criteria->streetname,call_struct.pckup_str_name);
				*ptr_end_unassinged = 1;			/* end all window */
				ptr_srch_key = &sc_key3;
				sc_key_on = SC_KEY3;
				memcpy(&call_struct,&tmp_call_struct,sizeof(struct cisam_cl));	/* restore call struct */
				return(DONE);
			}
			memcpy(&call_struct,&tmp_call_struct,sizeof(struct cisam_cl));		/* restore call struct */
		} else {
			*ptr_end_unassinged = 1;  /* end all window */
			return(DONE);
		}
	}
	return(NOT_DONE);
}


ck_sf_name(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged; 
{
	
	if ( field_entered ) { 						/* user entered field */
		strcpy(ptr_criteria->passengername,read_buf);
		ptr_srch_key = &sc_key4;
		sc_key_on = SC_KEY4;
		crit_sf_on = 1;
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

ck_sf_phone1(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged;
{
	
	if ( field_entered ) { 					/* user entered field */
#ifdef SCANDINAVIA
		strcpy(ptr_criteria->phonenumber, read_buf);
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

ck_sf_phone2(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
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
		ptr_srch_key = &sc_key5;
		sc_key_on = SC_KEY5;
		crit_sf_on = 1;
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

ck_sf_charge(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
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
		ptr_srch_key = &sc_key6;
		sc_key_on = SC_KEY6;
		crit_sf_on = 1;
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


ck_sf_number(piu_ptr, read_buf, key_val, field_entered, ptr_criteria, ptr_end_unassinged)
int *piu_ptr;
char *read_buf;
int key_val;
int field_entered;
struct select_criterias *ptr_criteria;
int *ptr_end_unassinged;
{
	
	if ( field_entered ) { 					/* user entered field */
		strcpy(ptr_criteria->callnumber,read_buf);
		ptr_srch_key = &sc_key1;
		sc_key_on = SC_KEY1;
		crit_sf_on = 1;
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
