static char sz__FILE__[]="@(#)check_fld.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  check_fld.c; Rel: 6.26.0.0; Get date: 10/29/92 at 12:22:38
* @(#)  Last delta: 4/25/91 at 13:55:04
* @(#)  SCCS File: /taxi/sccs/s.check_fld.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
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
ERROR: Compile time constant SCANDINAVIA or NAMERICA undefined.
#endif
#endif

#include <langinfo.h>
#include <locale.h>
#include <math.h>
#include <cursesX.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <langinfo.h>

#include "ui_strdefs.h"
#include "ui_def.h"
#include "ui_error.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "rate_isam.h"
#include "ex_key.h"
#include "ui_ipc.h"
#include "call_entry.h"
#include "screens.h"
#include "calls.h"
#include "user.h"
#include "df_maint.h"
#include "zgeo.h"
#include "func_keys.h"
#include "language.h"
#include "enhance.h"
#include "switch_ext.h"
#include "Scrn.h"
#include "Account.h"
#include "Account_Address.h"

#define		MAX_DIRS	8			/* for st pre and post dir's */
#define		KEY_LENGTH	4			/* for key flds */

							/* define abbreviations for charge card #'s used in check_charge() */
#define		MASTERCARD		catgets(UI_m_catd, UI_1, UI_CHECK_FLD_1, "MC")
#define		VISA			catgets(UI_m_catd, UI_1, UI_CHECK_FLD_2, "VI")
#define		DISCOVER		catgets(UI_m_catd, UI_1, UI_CHECK_FLD_3, "DS")
#define		AMERICAN_EXPRESS	catgets(UI_m_catd, UI_1, UI_CHECK_FLD_4, "AE")
#define		DINERS_CLUB		catgets(UI_m_catd, UI_1, UI_CHECK_FLD_5, "DC")
#define		CARTE_BLANCHE		catgets(UI_m_catd, UI_1, UI_CHECK_FLD_6, "CB")

							/* macro for checking if charge # entered is a credit card # */
#define		IS_CHARGE_CARD_NUM(ptr)	 ( ( (strncmp(MASTERCARD,ptr,2)==0) || (strncmp(VISA,ptr,2)==0) || \
					   (strncmp(DISCOVER,ptr,2)==0)  || (strncmp(AMERICAN_EXPRESS,ptr,2)==0) || \
					   (strncmp(DINERS_CLUB,ptr,2)==0) || (strncmp(CARTE_BLANCHE,ptr,2)==0) ) ? 1:0 )

extern struct cisam_cl call_struct;			/* for disk i/o */
extern struct call_ipc_rec call_fifo;			/* for writing to FIFO */
extern int scr_name;					/* name of screen on display */
extern int cur_fl_index;				/* index of the fleet the user is currently working with */
extern int user_index;					/* index into user's struct in shared mem */
extern struct fleets *fleet[];				/* array of pointers to fleets struct */
extern struct users *uiuser[];				/* array of pointers to users struct */
extern char req_buf[];
extern short inters_flag;				/* flag set if user wants to enter inters address */
extern struct keydesc st_key1, lo_key1, zn_key1, veh_key2, cust_key1;
extern int def_fl_index;				/* index for the users default fleet */
extern int zip1,zip2;					/* pickup zip and destintation zip */
extern int num_vh_attr_s,num_dr_attr_s;
extern int bPhoneFieldVisited;
extern int    EventFlag;
extern int ui_phone;
extern short line_mgr_attached;
extern int LineMgrIndex;
extern char Telephone[];

extern int (*ui_fleet_perms[])();
short user_knows=0;			/* whether user knows if call is unzoned */
short call_zoned=0;			/* if call is not or not */
short write_disk=1;			/* if call should be added to call rec */
short write_subs=0;			/* if call should be written to subs file */
short write_dispatch=1;			/* should write rec to FIFO or not */
short user_changed_dt_tm=0;		/* did user change date/time fields 0=did not, 1=he did */
extern double atof();			/* library function */
extern int doing_update;		/* Indicates the update function key is being processed. */
struct cisam_sc subs_struct;		/* structure for writing subscription record to disk */
short been_touched;
extern char *Spaces;
extern char tmp_req_buf[];
BOOLEAN bExplicitPremium;
struct cisam_cust  gCustRec;

/* to and from date for subscriptions */
char from_date[7];
char to_date[7];

int tcr_check_time_call_restriction( struct cisam_cl *, int, int );

struct scr_flds cust_flds[] = {						/* piu fields, 1 for choice selection */
	{0,0,1,1},
	{A_SLCT_ROW, A_SLCT_COL, A_SLCT_SIZ, A_SLCT}
};

struct cisam_cust  cust_choice[8];
extern WINDOW *srch_win;
extern short silent;
extern short clear_err_flag;

/****************************************************************************/
/* update_piu: updates the value of piu, for the next field which is        */
/*		is to be read, called by field checking routines 	    */
/****************************************************************************/
int update_piu(ptr_scr_fld,piu_ptr,key_val)
struct scr_flds *ptr_scr_fld;			/* pointer to array of piu structures */
int *piu_ptr;					/* pointer to piu */
int key_val;					/* value of key pressed */
{
	struct scr_flds *ptr_to_fld;		/* pointer to this field piu struct */
	int done;
	int orig_piu;
	int i, j, attr_start, skip_attr;
	int on_basic_screen, on_extended_screen, on_wakeup_screen;
	int row, col;
	int on_attribute;
	char *attr_abreviation, *attr_description;
	static int attr_description_displayed = 0, old_scr_name = 0;

	/* No attribute description displayed if new screen */
	if (old_scr_name != scr_name)
	  attr_description_displayed = 0;

	old_scr_name = scr_name;

	on_basic_screen = 0;
	on_extended_screen = 0;
	on_wakeup_screen = 0;

	orig_piu = *piu_ptr;

	if (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR)
	  on_basic_screen = 1;
	else if (scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR)
	  on_extended_screen = 1;
	else if (scr_name == WAKEUP_CALL_SCR || scr_name == SUBS_WAKEUP_SCR)
	  on_wakeup_screen = 1;


	switch(key_val) {
		case OK:			/* max char's were entered by user */
		case '\r':
		case '\n':
		case KEY_DOWN:
			*piu_ptr += 1;		/* move to next field */
			break;

		case '\t':			/* move the tab field specified in the piu structure */
			ptr_to_fld = ptr_scr_fld + (*piu_ptr);
			*piu_ptr = ptr_to_fld->tab_field;
			break;

		case KEY_UP:
			*piu_ptr -= 1;		/* move up 1 field */
			break;

		default:			/* some other function key entered */
			return(NOT_DONE);
	}

	/*
	 * In the Subscription screens, we skip date & time fields.
	 */
	if (scr_name == SUBS_BASIC_SCR) {
	    if (*piu_ptr == B_DATE) /* Must have advanced from Phone to Date */
		*piu_ptr = B_COMMENT;
	    else if (*piu_ptr == B_TIME) /* Must have gone back from Comment to Time */
		*piu_ptr = B_PH_EXT;
	}

	else if (scr_name == SUBS_EXTENDED_SCR) {
	    if (*piu_ptr == E_DATE) /* Must have advanced from Phone to Date */
		*piu_ptr = E_COMMENT;
	    else if (*piu_ptr == E_TIME) /* Must have gone back from Comment to Time */
		*piu_ptr = E_PH_EXT;
	}

	else if (scr_name == SUBS_WAKEUP_SCR) {
	    if (*piu_ptr == W_DATE) /* Must have advanced from Name to Date */
		*piu_ptr = W_PH_AREA_CODE;
	    else if (*piu_ptr == W_TIME) /* Must have gone back from Phone to Time */
		*piu_ptr = W_NAME;
	}

#ifdef SCANDINAVIA
	/*
	 * Check if going in to a now "forbidden" field that is
	 * not used.
	 */
	done = 0;
	if (on_basic_screen) {
	    do {
	      if (*piu_ptr == B_DUMMY_1 ||
		  *piu_ptr == B_DUMMY_2 ||
		  *piu_ptr == B_DUMMY_3 ||
		  *piu_ptr == B_DUMMY_4 ||
		  *piu_ptr == B_DUMMY_5 ||
		  *piu_ptr == B_DUMMY_6 ||
		  *piu_ptr == B_DUMMY_7 ||
		  *piu_ptr == B_DUMMY_8 ||
		  *piu_ptr == B_DUMMY_9 ||
		  *piu_ptr == B_PH_NUM1 ||
		  *piu_ptr == B_PH_NUM2 ||
		  ( *piu_ptr == B_RATE && ( 0 ) )   ||
		  *piu_ptr == B_VAT     ||
		  *piu_ptr == B_PERSONEL2 ||
		  *piu_ptr == B_PH_EXT) {


		    if (orig_piu < *piu_ptr) {
		    /* Go to field after */
			(*piu_ptr) ++;

			if ( (line_mgr_attached) && (LineMgrIndex >= 0) && (LineMgrIndex < MAX_PIDS) &&
			     (orig_piu == B_PH_NUM1) && (*piu_ptr == B_DATE) )
			  //			if ( (ui_phone) && (orig_piu == B_PH_NUM1) && (*piu_ptr == B_DATE))
			  {
			    *piu_ptr = B_PH_NUM2;
			    done = 1;
			  }
			if ( (line_mgr_attached) && (LineMgrIndex >= 0) && (LineMgrIndex < MAX_PIDS) &&
			     !bPhoneFieldVisited && !doing_update )
			  {
			    *piu_ptr = B_PKUP_ST_NAME;
			    bPhoneFieldVisited = 1;
			    done = 1;
			  }
			/* Skip Intersection fields if no intersections */
			if (*piu_ptr ==  B_INTERS_ST_NAME && !inters_flag)
			    *piu_ptr = B_PKUP_APT_NO;
		    }
		    else {
			/* Go to field before */
			(*piu_ptr) --;
			
			/* Skip Intersection fields if no intersections */
			if (*piu_ptr ==  B_INTERS_ST_NAME && !inters_flag)
			    *piu_ptr = B_PKUP_ST_NO;
		    }
		    
		}
		else
		  done = 1;
	    } while (!done);
	}
	else if (on_extended_screen) {
	    do {
	      if (*piu_ptr == E_DUMMY_1 ||
		  *piu_ptr == E_DUMMY_2 ||
		  *piu_ptr == E_DUMMY_3 ||
		  *piu_ptr == E_DUMMY_4 ||
		  *piu_ptr == E_DUMMY_5 ||
		  *piu_ptr == E_DUMMY_6 ||
		  *piu_ptr == E_DUMMY_7 ||
		  *piu_ptr == E_DUMMY_8 ||
		  *piu_ptr == E_DUMMY_9 ||
		  *piu_ptr == E_PH_NUM1 ||
		  *piu_ptr == E_PH_NUM2 ||
                  ( *piu_ptr == E_CASH_ACCT && !( 0 ) ) ||
		  ( *piu_ptr == E_RATE && ( 0 ) )    ||
		  *piu_ptr == E_VAT     ||
		  *piu_ptr == E_PERSONEL2 ||
		  *piu_ptr == E_PH_EXT) {
		    
		    if (orig_piu < *piu_ptr) {
		    /* Go to field after */
			(*piu_ptr) ++;
			
			/* Skip Intersection fields if no intersections */
			if (*piu_ptr ==  E_INTERS_ST_NAME && !inters_flag)
			    *piu_ptr = E_PKUP_APT_NO;
		    }
		    else {
			/* Go to field before */
			(*piu_ptr) --;
			
			/* Skip Intersection fields if no intersections */
			if (*piu_ptr ==  E_INTERS_ST_NAME && !inters_flag)
			    *piu_ptr = E_PKUP_ST_NO;
		    }
		    
		}
		else
		done = 1;
	    } while (!done);
	}
	else if (on_wakeup_screen) {
	    do {
		if (*piu_ptr == W_PH_NUM1 ||
		    *piu_ptr == W_PH_NUM2 ||
		    *piu_ptr == W_PH_EXT) {
		    
		    if (orig_piu < *piu_ptr) {
		    /* Go to field after */
			(*piu_ptr) ++;
		    }
		    else {
			/* Go to field before */
			(*piu_ptr) --;
		    }
		    
		}
		else
		done = 1;
	    } while (!done);
	}

	/*
	 * In the Subscription screens, we skip date & time fields.
	 * We did this before, but the skipping of fields can make things
	 * wrong, so we do it again.
	 */
	if (scr_name == SUBS_BASIC_SCR) {
	    if (*piu_ptr == B_DATE) /* Must have advanced from Phone to Date */
		*piu_ptr = B_COMMENT;
	    else if (*piu_ptr == B_TIME) /* Must have gone back from Comment to Time */
		*piu_ptr = B_PH_AREA_CODE;
	}
	else if (scr_name == SUBS_EXTENDED_SCR) {
	    if (*piu_ptr == E_DATE) /* Must have advanced from Phone to Date */
		*piu_ptr = E_COMMENT;
	    else if (*piu_ptr == E_TIME) /* Must have gone back from Comment to Time */
		*piu_ptr = E_PH_AREA_CODE;
	}
	else if (scr_name == SUBS_WAKEUP_SCR) {
	    if (*piu_ptr == W_DATE) /* Must have advanced from Name to Date */
		*piu_ptr = W_PH_AREA_CODE;
	    else if (*piu_ptr == W_TIME) /* Must have gone back from Phone to Time */
		*piu_ptr = W_NAME;
	}



#endif	    

	/* Only stop on rate field if allowed to enter rate */
	if ( (*piu_ptr == B_RATE && on_basic_screen) ||
	     (*piu_ptr == E_RATE && on_extended_screen) )
	  {
	    /* The arguments for get_rate() will return 0 if rate cannot be entered */
	    if (get_rate(C_PRINT, NO, NULL, NULL) == 0)
	      {
		if (orig_piu < *piu_ptr)
		  (*piu_ptr) ++;
		else
		  (*piu_ptr) --;
	      }
	  }

	/* Skip 2nd personnel column if 1st one is empty and
	   you're going backwards */
	else if ( ((*piu_ptr == B_PERSONEL2 && on_basic_screen) ||
		   (*piu_ptr == E_PERSONEL2 && on_extended_screen)) &&
		 orig_piu > *piu_ptr)
	{
	    if (call_struct.personal_veh == 0)
		  (*piu_ptr) --;
	  }

#ifdef FOO
        if (scr_name == BASIC_CALL_SCR)
        {
           if (orig_piu < *piu_ptr)
           { 
              /* Check if heading forward */
              if (*piu_ptr == B_PKUP_PRE_DIR)
                 *piu_ptr =  B_PKUP_ST_NAME;
              else if (*piu_ptr == B_PKUP_POST_DIR)
                 *piu_ptr = B_PASSENGER;
              else if (*piu_ptr == B_PH_AREA_CODE)
                 *piu_ptr = B_TIME;       
              else if (*piu_ptr == B_COMMENT)
                 *piu_ptr =   B_DEST_ST_NAME;
              else if (*piu_ptr == B_DEST_PRE_DIR)
                 *piu_ptr = B_DEST_ST_NAME;
              else if (*piu_ptr == B_DEST_POST_DIR)
                 *piu_ptr = B_CAR_NEEDS;
              else if (*piu_ptr == B_PKUP_CITY)
                 *piu_ptr = B_PASSENGER;
           } /* if */  
           else 
           {
              if (*piu_ptr == B_PKUP_PRE_DIR)
                 *piu_ptr = B_PKUP_ST_NO;
              else if (*piu_ptr == 11)
                 *piu_ptr = B_PKUP_ST_TYPE;
              else if (*piu_ptr == B_DEST_PRE_DIR) 
                 *piu_ptr = B_DEST_ST_NO;          
              else if (*piu_ptr == B_DEST_POST_DIR)
                 *piu_ptr = B_DEST_ST_TYPE;      
           } /* else */

        } /* if */	
#endif
	return(DONE);
}


int check_dummy(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered)
	struct scr_flds *ptr_scr_fld;	/* pointer to array of piu structures */
	int *piu_ptr;  	/* pointer to piu */
	char *read_buf;	/* pointer to buffer for reading field */
	int key_val;	/* value of key pressed by user */
	int *field_entered;	/* whether field entered =1; or not=0 */
{
	return(update_piu(ptr_scr_fld,piu_ptr,key_val));
}

/**************************************************************************************************/
int check_fleet(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered)
struct scr_flds *ptr_scr_fld;			/* pointer to array of piu structures */
int *piu_ptr;  					/* pointer to piu */
char *read_buf;					/* pointer to buffer for reading field */
int key_val;					/* value of key pressed by user */
int *field_entered;				/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;
	int i;					/* scratch variable */
	int ret_val;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (!*field_entered) { 						/* if a previous valid entry is not avaible */
		ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val); 	/* trap user in field */
		if (call_struct.fleet == '\0') { 			/* no entry made so far */
			*piu_ptr = 1;
			prt_error(UI_REQENTRY,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_7, "<fleet id>"));
			return(ret_val);
		}
		else { 							/* valid previous entry avaibale */
			attrset(A_UNDERLINE); 				/* print it */
			mvaddch(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col,call_struct.fleet);
			attrset(0);
			return(ret_val);
		}
	}  								/* end if field not entered */
	else  { 							/* user has entered the field */
	        /* No update fleet while in update mode */
	        if(doing_update && read_buf[0] != call_struct.fleet)
		  {
		    prt_error(UI_NO_MSG, "%s %s",
			      catgets(UI_m_catd, UI_1, UI_CHECK_FLD_7, "<fleet id>"),
			      catgets(UI_m_catd, UI_1, UI_MUL_KEY_7, "Unable to update"));
		    return(update_piu(ptr_scr_fld,piu_ptr,key_val));
		  }

		if (read_buf[0] == ' ') { 				/* if user entered a space, then take the default fleet */
			if (def_fl_index != -1) { 			/* default value of fleet is valid, print and save it */
				call_fifo.fleet = fleet[def_fl_index]->fleet_id;
				attrset(A_UNDERLINE);
				mvaddch(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col,call_struct.fleet);
				attrset(0);
				return(update_piu(ptr_scr_fld,piu_ptr,key_val));
			}  						/* end right default fleet */
			else { 						/* wrong default fleet */
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_8, "<wrong default fleet>"));
				return(DONE);
			}
		}  							/* end if ...user wants default fleet id */

									/* new entry of fleet */
		if ((cur_fl_index = find_findex(read_buf[0])) == -1) { 	/* user entered invalid fleet id, trap user in field */
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_7, "<fleet id>"));
			return(DONE);
		}
		if (fleet[cur_fl_index]->active != YES) { 		/* check if fleet entered is not active */
			prt_error(UI_NOT_ACTIVE,"<%s %c>", catgets(UI_m_catd, UI_1, UI_CHECK_FLD_9, "fleet"), read_buf[0]);
			return(DONE);
		}
		 
		if ((*ui_fleet_perms[cur_fl_index])() == NOT_ALLOWED) {		/* is user allowed to work with this fleet */
			prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_10, "<fleet>")); 			/* no permission */
			return(DONE);						/* trap user in field */
		}

		if (call_struct.fleet == read_buf[0])				/* same fleet ? */
			return(update_piu(ptr_scr_fld,piu_ptr,key_val));	/* update piu and return */
			
		call_struct.fleet = read_buf[0]; 				/* copy it in call struct */
		call_fifo.fleet = call_struct.fleet;

		/* New fleet may have different zones; clear the old zones, and read new ones */
		call_struct.pckup_zone = 0;
		call_struct.dest_zone = 0;
		zone_addr(&call_struct, PICKUP_ADDR, &zip1);
		if (scr_name == EXTENDED_CALL_SCR)
		  {
		    print_addr_comment(C_PRINT, PICKUP_ADDR);
		  }
		zone_addr(&call_struct, DEST_ADDR, &zip2);
		if (scr_name == EXTENDED_CALL_SCR)
		  {
		    print_addr_comment(C_PRINT, DEST_ADDR);
		  }

		attrset(A_REVERSE); 						/* reprint the greeting line */
		move(1,0);
		for (i=0; i < COLS-1; i++) 
			addch(' ');
		if (fleet[cur_fl_index]->greeting_line == YES)  /* print it */
#ifdef NAMERICA
		if (fleet[cur_fl_index]->greeting_line == YES)  /* print it */
			mvprintw(1,GREETING_COL, "%s, %s %s.",
                                 fleet[cur_fl_index]->fleet_name, 
				 uiuser[user_index]->greeting_name, 
                                 "speaking");
#else
			mvprintw(1,GREETING_COL, "%s  %s, %s %s. %s",
				 catgets(UI_m_catd, UI_1, UI_CHECK_FLD_11, "Good Day!"), fleet[cur_fl_index]->fleet_name,
				 catgets(UI_m_catd, UI_1, UI_CHECK_FLD_12, "this is"), 
				uiuser[user_index]->greeting_name, catgets(UI_m_catd, UI_1, UI_CHECK_FLD_13, "May I help you?") );
#endif
		attrset(A_UNDERLINE);

		if((call_struct.priority = get_call_pri()) == -1) { 			/* get the call priority and save it */
			call_struct.priority = fleet[cur_fl_index]->passenger_priority_def;	/* default to passenger */
		}
		call_fifo.priority = call_struct.priority;
		get_rate(C_CLEAR,NO,&zip1,&zip2);				/* sets rate = 0.0 */
		get_rate(C_PRINT,YES,&zip1,&zip2); 				/* print the rate from the rate file, if applicable */
		

		switch (scr_name)  { 		/* print the rest of the default values depending upon the screen on display */
		   case BASIC_CALL_SCR:
		   case SUBS_BASIC_SCR:
			clear_field(stdscr,ptr_scr_fld,B_PRIORITY); 		/* print the call priority */
			mvprintw(B_PRIORITY_ROW,B_PRIORITY_COL,"%d", call_struct.priority);
			
			if (uiuser[user_index]->default_city[0]=='\0') {		/* has user defined a default city ? */
				clear_field(stdscr,ptr_scr_fld, B_PKUP_CITY);	/* no, take from fleet structure */
				mvaddstr(B_PKUP_CITY_ROW, B_PKUP_CITY_COL, fleet[cur_fl_index]->default_city);
				strcpy(call_struct.pckup_city, fleet[cur_fl_index]->default_city);
			}

										/* get the default phone area code for this fleet */
		  	if ((fleet[cur_fl_index]->default_phone_area[0]!='\0')
		   	   	&& (strcmp(fleet[cur_fl_index]->default_phone_area,"000") != 0)){
				mvaddstr(B_PHONE_ROW,B_PH_AREA_CODE_COL, fleet[cur_fl_index]->default_phone_area);
				strcpy(call_struct.phone_area, fleet[cur_fl_index]->default_phone_area);
			 }

			attrset(A_UNDERLINE);
			if (call_struct.pckup_zone > 0)
			  mvprintw(B_PKUP_ZONE_ROW, B_PKUP_ZONE_COL, "%-03d", call_struct.pckup_zone);
			else
			  mvprintw(B_PKUP_ZONE_ROW, B_PKUP_ZONE_COL, "   ");
			if (call_struct.dest_zone > 0)
			  mvprintw(B_DEST_ZONE_ROW, B_DEST_ZONE_COL, "%-03d", call_struct.dest_zone);
			else
			  mvprintw(B_DEST_ZONE_ROW, B_DEST_ZONE_COL, "   ");
			attrset(A_NORMAL);
			get_zone_desc(C_PRINT, PICKUP_ADDR, &call_struct, NULL);
			get_zone_desc(C_PRINT, DEST_ADDR, &call_struct, NULL);

		 	break;							/* leave switch */

		   case EXTENDED_CALL_SCR:
		   case SUBS_EXTENDED_SCR:
			clear_field(stdscr,ptr_scr_fld,E_PRIORITY); 			/* print the call priority */
			mvprintw(E_PRIORITY_ROW,E_PRIORITY_COL,"%d", call_struct.priority);
			
											/* copy the default city */
			if (uiuser[user_index]->default_city[0]=='\0') {			/* if user did not define */
				clear_field(stdscr,ptr_scr_fld, E_PKUP_CITY);		/* take from fleet */
				mvaddstr(E_PKUP_CITY_ROW, E_PKUP_CITY_COL, fleet[cur_fl_index]->default_city);
				strcpy(call_struct.pckup_city, fleet[cur_fl_index]->default_city);
			}

											/* get phone are code */
		  	if ((fleet[cur_fl_index]->default_phone_area[0]!='\0')
		   	     && (strcmp(fleet[cur_fl_index]->default_phone_area, "000") != 0)) {
				mvaddstr(E_PHONE_ROW,E_PH_AREA_CODE_COL, fleet[cur_fl_index]->default_phone_area);
				strcpy(call_struct.phone_area, fleet[cur_fl_index]->default_phone_area);
			 }

			attrset(A_UNDERLINE);
			if (call_struct.pckup_zone > 0)
			  mvprintw(E_PKUP_ZONE_ROW, E_PKUP_ZONE_COL, "%-03d", call_struct.pckup_zone);
			else
			  mvprintw(E_PKUP_ZONE_ROW, E_PKUP_ZONE_COL, "   ");
			if (call_struct.dest_zone > 0)
			  mvprintw(E_DEST_ZONE_ROW, E_DEST_ZONE_COL, "%-03d", call_struct.dest_zone);
			else
			  mvprintw(E_DEST_ZONE_ROW, E_DEST_ZONE_COL, "   ");
			attrset(A_NORMAL);
			get_zone_desc(C_PRINT, PICKUP_ADDR, &call_struct, NULL);
			get_zone_desc(C_PRINT, DEST_ADDR, &call_struct, NULL);
		 	break;							/* leave switch */


		   case WAKEUP_CALL_SCR:
		   case SUBS_WAKEUP_SCR:					/* print the default area code this fleet */
		  	if ((fleet[cur_fl_index]->default_phone_area[0]!='\0')
		   	     && (strcmp(fleet[cur_fl_index]->default_phone_area, "000") != 0)) {
				mvaddstr(W_PHONE_ROW,W_PH_AREA_CODE_COL, fleet[cur_fl_index]->default_phone_area);
				strcpy(call_struct.phone_area, fleet[cur_fl_index]->default_phone_area);
			 }
		 	break;							/* leave switch */
		  }  								/* end switch */
		  return(update_piu(ptr_scr_fld,piu_ptr,key_val)); 		/* update piu and return */
		}  								/* end else ... field entered */
}  										/* end check_fleet() */

/**********************************************************************************************/
/* check_type - verify entry or modification of the call type field			      */
/**********************************************************************************************/
int check_type(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered)
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
	int ret_val;
	struct scr_flds *ptr_to_fld;
	int old_piu;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (!*field_entered) {				/* field not entered; check if previous field is there */
		old_piu = *piu_ptr;
		ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
		if (call_struct.extended_type[0] == '\0') {		/* no previous entry,trap user in field */
			prt_error(UI_REQENTRY,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_14, "<call type>"));
			*piu_ptr = old_piu;				/* trap user in field */
			return(ret_val);
		}
		else {							/* previous entry */
			attrset(A_UNDERLINE);
			mvprintw(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, "%c%c",call_struct.extended_type[0], call_struct.extended_type[1]);
			attrset(0);
		}
		return(ret_val);
	}
	else  {								/* user has entered the field */
		if (read_buf[0] == ' ') {				/* if first char entered is space, then use default value */
			call_struct.extended_type[0] = uiuser[user_index]->default_call_type;
			call_struct.extended_type[1] = ' ';
			call_struct.extended_type[2] = '\0';
			call_struct.extended_type[3] = '\0';
			call_struct.extended_type[4] = '\0';
			call_struct.extended_type[5] = '\0';
			get_call_type();
			attrset(A_UNDERLINE);
			mvprintw(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.extended_type);
			attrset(0);
			return(update_piu(ptr_scr_fld,piu_ptr,key_val));
		}

									/* new entry , so verify it */
		if (read_buf[0] != PASS && read_buf[0] != PASS2 && read_buf[0] != DELIV && read_buf[0] != TYPEX) {
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_15, "<call type1>"));
			return(DONE);
		}

		call_struct.extended_type[0] = read_buf[0];
		if (read_buf[1] == '\0')
		  read_buf[1] = INVOICE_CALL;

		if ( read_buf[0] == PASS2 )
		  {
		    read_buf[1] = 'E';
		    if ( scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR )
		      {
			attrset(A_UNDERLINE);
			mvprintw(B_CL_TYPE_ROW, B_CL_TYPE_COL + 1, "%c", read_buf[1]);
			attrset(0);
		      }
		  }
		
		call_struct.extended_type[1] = (read_buf[1] == '\0') ? ' ' : read_buf[1];
		call_struct.extended_type[2] = (read_buf[2] == '\0') ? ' ' : read_buf[2];


		time_subs_call();
		get_call_type();					/* copy info in call_fifo */
		
		if((call_struct.priority = get_call_pri()) == -1){ 			/* get the call priority and save it */
			call_struct.priority = fleet[cur_fl_index]->passenger_priority_def;	/* default to passenger */
		}
		call_fifo.priority = call_struct.priority;

		attrset(A_UNDERLINE);							/* print new call priority on screen */
		if (scr_name == BASIC_CALL_SCR || scr_name==SUBS_BASIC_SCR) {
			clear_field(stdscr,ptr_scr_fld,B_PRIORITY);
			mvprintw(B_PRIORITY_ROW,B_PRIORITY_COL,"%d", call_struct.priority);
		}
		else if (scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR) {
			clear_field(stdscr,ptr_scr_fld,E_PRIORITY);
			mvprintw(E_PRIORITY_ROW,E_PRIORITY_COL,"%d", call_struct.priority);
		}
		attrset(0);
		return(update_piu(ptr_scr_fld,piu_ptr,key_val));			/* update piu */
	}
}

/*********************************************************************************************/
/* check_st_no - verify entry of street number						     */
/*********************************************************************************************/
int check_st_no(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered)
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;			/* pointer to field */
	int *str_nbr_ptr;					/* pointer to call_struct field */
	char *suf_ptr;					/* pointer to suffix */
	int  i, len;
	
	ptr_to_fld = ptr_scr_fld + (*piu_ptr);
	
	if (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR) {
		switch (*piu_ptr) {
			case B_PKUP_ST_NO:
				str_nbr_ptr = &call_struct.pckup_str_nbr;
				suf_ptr = &call_struct.pckup_str_suf;
				break;

			case B_DEST_ST_NO:
				str_nbr_ptr = &call_struct.dest_str_nbr;
				suf_ptr = &call_struct.dest_str_suf;
				break;

			default:
				prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_16, "<check_st_no() called>"));
		}
	}
	else if (scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR) {
		switch (*piu_ptr) {
			case E_PKUP_ST_NO:
				str_nbr_ptr = &call_struct.pckup_str_nbr;
				suf_ptr = &call_struct.pckup_str_suf;
				break;

			case E_DEST_ST_NO:
				str_nbr_ptr = &call_struct.dest_str_nbr;
				suf_ptr = &call_struct.dest_str_suf;
				break;

			default:
				prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_16, "<check_st_no() called>"));
		}
	}

	if (*field_entered) {			/* field entered, if 1st char entered is a space, then wipe out the old entry */
		if (read_buf[0] == ' ') {
			*str_nbr_ptr = 0;
			*suf_ptr = '\0';
		}
		else {				/* user has entered the street #; 7th char if entered should be an alpha. */
			been_touched = TRUE;
			len = strlen(read_buf);

#ifdef NAMERICA
			/* Check last char (must be digit or alpha */
			if (!isalnum(read_buf[len - 1])) {
			    prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_17, "<street number>"));
			    return(DONE);
			}

			/* Everything except the last char is digit */
			for (i = 0; i < len - 1; i ++)
			    if (!isdigit(read_buf[i])) {
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_17, "<street number>"));
				return(DONE);
			    }

			/* Check to see if there is a alpha suffix */
			if (isalpha(read_buf[len - 1]))
			    *suf_ptr = read_buf[len - 1];

			/* If full length, last char must be a an alpha for suffix */
			else if (len == CHARS_ST_NO)
			  {
			    prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_17, "<street number>"));
			    return(DONE);
			  }

			/* Assign street number to call_struct */
			*str_nbr_ptr = atoi(read_buf);
#else
			/* All chars must be digits */
			for (i = 0; i < len; i ++)
			    if (!isdigit(read_buf[i])) {
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_17, "<street number>"));
				return(DONE);
			    }

			*str_nbr_ptr = atoi(read_buf);
#endif
		}
		zone_call(ptr_scr_fld,*piu_ptr);
	}
	return(update_piu(ptr_scr_fld,piu_ptr,key_val));			/* update piu and return */
}

/*****************************************************************************************/
/* check_pre_dir - verifies pre-direction and post-direction fields on all screens       */
/*****************************************************************************************/
int check_pre_dir(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
/* Finland doesn't have pre or post dir, so this function is not called */
#ifdef NAMERICA

	static char *directions[] = {"N","S","E","W","NW","NE","SW","SE"};
	char *temp_ptr;					/* to be assigned to the field in call_struct */
	int i;						/* scratch variable */
	struct scr_flds *ptr_to_fld;			/* pointer to the approp. piu struct */
	int check_inters_flag=0;			/* if should check inters_flag for updating piu */
	int old_piu;
	int ret_val;


	if (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR) {	/* determine which field is being entered */
		switch (*piu_ptr) {
			case B_PKUP_PRE_DIR:
				temp_ptr = call_struct.pckup_pre_dir;
				break;
			case B_PKUP_POST_DIR:
				temp_ptr = call_struct.pckup_post_dir;
				check_inters_flag = 1;				
				break;
			case B_INTERS_PRE_DIR:
				temp_ptr = call_struct.intr_pre_dir;
				break;
			case B_INTERS_POST_DIR:
				temp_ptr = call_struct.intr_post_dir;
				break;
			case B_DEST_PRE_DIR:
				temp_ptr = call_struct.dest_pre_dir;
				break;
			case B_DEST_POST_DIR:
				temp_ptr = call_struct.dest_post_dir;
				break;
			default:
				prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_18, "<check_pre_dir called>"));
		}
	}
	else if (scr_name==EXTENDED_CALL_SCR || scr_name==SUBS_EXTENDED_SCR) {
		switch (*piu_ptr) {
			case E_PKUP_PRE_DIR:
				temp_ptr = call_struct.pckup_pre_dir;
				break;
			case E_PKUP_POST_DIR:
				temp_ptr = call_struct.pckup_post_dir;
				check_inters_flag = 1;				
				break;
			case E_INTERS_PRE_DIR:
				temp_ptr = call_struct.intr_pre_dir;
				break;
			case E_INTERS_POST_DIR:
				temp_ptr = call_struct.intr_post_dir;
				break;
			case E_DEST_PRE_DIR:
				temp_ptr = call_struct.dest_pre_dir;
				break;
			case E_DEST_POST_DIR:
				temp_ptr = call_struct.dest_post_dir;
				break;
			default:
				prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_18, "<check_pre_dir called>"));
		}
	}

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);			/* initialize pointer this piu structure */

	if (!*field_entered) {					/* field not entered */
		old_piu = *piu_ptr;
		ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
		if ((*piu_ptr == old_piu+1) && (check_inters_flag)) 
			if (!inters_flag)			/* do not read intersection ad */
				*piu_ptr = old_piu + 5;
		return(ret_val);
	}
	else {							/* field entered */
		been_touched = TRUE;
		if (read_buf[0] == ' ') {			/* if 1st char is space, then wipe out old entry */
			*temp_ptr = '\0';
			zone_call(ptr_scr_fld,*piu_ptr);
			old_piu = *piu_ptr;
			ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
			if ((*piu_ptr == old_piu+1) && (check_inters_flag)) 
				if (!inters_flag)		/* do not read intersection ad */
					*piu_ptr = old_piu + 5;
			return(ret_val);
		}
		if (strcmp(temp_ptr,read_buf) == 0) {		/* user did not change previous entry */
			old_piu = *piu_ptr;
			ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
			if ((*piu_ptr == old_piu+1) && (check_inters_flag)) 
				if (!inters_flag)			/* do not read intersection ad */
					*piu_ptr = old_piu + 5;
			return(ret_val);
		}

		for (i=0; i < MAX_DIRS; i++) {			/* field entered; verify and copy */
			if (strcmp(directions[i],read_buf) == 0) {		/* valid entry */
				strcpy(temp_ptr,read_buf);
				zone_call(ptr_scr_fld,*piu_ptr);
				old_piu = *piu_ptr;
				ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
				if ((*piu_ptr == old_piu+1) && (check_inters_flag)) 
					if (!inters_flag)			/* do not read intersection ad */
						*piu_ptr = old_piu + 5;
				return(ret_val);
			}
		}

		/* field entered is not valid */
		prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_19, "<street direction>"));
		zone_call(ptr_scr_fld,*piu_ptr);

/*		old_piu = *piu_ptr;
/*		ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
/*		if ((*piu_ptr == old_piu+1) && (check_inters_flag)) 
/*			if (!inters_flag)					/* do not read intersection ad */
/*				*piu_ptr = old_piu + 5;
/*		return(ret_val); */
		return(DONE);
	}
#else
        return(DONE);
#endif
}

/****************************************************************************************/
/* check_st_name - verify entry into the street name field				*/
/****************************************************************************************/
int check_st_name(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;			/* pointer to array of piu structures */
int *piu_ptr;					/* pointer to piu */
char *read_buf;					/* pointer to buffer for reading field */
int key_val;					/* value of key pressed by user */
int *field_entered;				/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;		/* pointer to the piu struct */
	char *temp_ptr;				/* pointer to field in call_struct */
	int len_st_name;			/* length of st name entered */
	int is_pkup=0;			/* is address being read the pkup address */
	int old_piu;				/* piu value of this field */
	int ret_val;				/* return code from update_piu() */

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);	/* check which field is being entered */

	if (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR) {
		switch (*piu_ptr) {
			case B_PKUP_ST_NAME:
				temp_ptr = call_struct.pckup_str_name;
				is_pkup = 1;
				break;
			case B_INTERS_ST_NAME:
				temp_ptr = call_struct.intr_str_name;
				break;
			case B_DEST_ST_NAME:
				temp_ptr = call_struct.dest_str_name;
				break;
			default:
				prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_20, "<check_str_name called>"));
		}
	}
	else if (scr_name==EXTENDED_CALL_SCR || scr_name==SUBS_EXTENDED_SCR) {
		switch (*piu_ptr) {
			case E_PKUP_ST_NAME:
				temp_ptr = call_struct.pckup_str_name;
				is_pkup = 1;
				break;
			case E_INTERS_ST_NAME:
				temp_ptr = call_struct.intr_str_name;
				break;
			case E_DEST_ST_NAME:
				temp_ptr = call_struct.dest_str_name;
				break;
			default:
				prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_20, "<check_str_name called>"));
		}
	}
	
	if (!*field_entered) {
		old_piu = *piu_ptr;
		ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);

		return(ret_val);
	}
	else {				/* field entered; if 1st char entered is a space, then wipe out old entry if any */
		been_touched = TRUE;
		if (read_buf[0] == ' ') {
			*temp_ptr = '\0';
			if (is_pkup) {
				inters_flag = 0;
				inters_flds(C_CLEAR,ptr_scr_fld);		/* clear the intersection fields */
			}
			if (is_pkup)
			  print_addr_comment(C_CLEAR, PICKUP_ADDR);
			else
			  print_addr_comment(C_CLEAR, DEST_ADDR);
			zone_call(ptr_scr_fld,*piu_ptr);
			return(update_piu(ptr_scr_fld,piu_ptr,key_val));
		}

		len_st_name = strlen(read_buf);				/* new st name entered */
		if (is_pkup) {
			if (*(read_buf+len_st_name-1) != '/') {
				inters_flag = 0;
				inters_flds(C_CLEAR,ptr_scr_fld);		/* clear the intersection fields */
			}
			if (*(read_buf+len_st_name-1) == '/') {
				if (!inters_flag) {			/* first time thru, set flag */
					inters_flag = 1;
			inters_flds(C_PRINT,ptr_scr_fld);	/* print the intersection fields */
				}
				*(read_buf+len_st_name-1) = '\0';
			}
		}
		if (strcmp(temp_ptr,read_buf) != 0) {			/* user entered something different */
			strcpy(temp_ptr,read_buf);			/* copy new entry */
			if (is_pkup)
			  print_addr_comment(C_CLEAR, PICKUP_ADDR);
			else
			  print_addr_comment(C_CLEAR, DEST_ADDR);
			zone_call(ptr_scr_fld,*piu_ptr);		/* zone the call */
			if (is_pkup) {					/* copy the pckup_str_key in call_struct */
				strncpy(call_struct.pckup_str_key,temp_ptr,KEY_LENGTH);
				call_struct.pckup_str_key[KEY_LENGTH] = '\0';
			}
		}
		return(update_piu(ptr_scr_fld,piu_ptr,key_val));
	}
}

/*******************************************************************************************/
/* check_st_type - verify correctness of the street type field				   */
/*******************************************************************************************/
int check_st_type(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
/* Finland doesn't have street_type, so this function is not called */
#ifdef NAMERICA

	struct scr_flds *ptr_to_fld;			/* pointer to the approp. piu struct */
	char *temp_ptr;					/* pointer to the string in call_struct */
	struct cisam_st *str_type_ptr;			/* pointer to street type struct for for reading cisam rec's */
	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR) {
		switch (*piu_ptr) {
			case B_PKUP_ST_TYPE:
				temp_ptr = call_struct.pckup_str_type;
				break;
			case B_INTERS_ST_TYPE:
				temp_ptr = call_struct.intr_str_type;
				break;
			case B_DEST_ST_TYPE:
				temp_ptr = call_struct.dest_str_type;
				break;
			default:
				prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_22, "<check_str_type called>"));
		}
	}
	else if (scr_name==EXTENDED_CALL_SCR || scr_name==SUBS_EXTENDED_SCR) {
		switch (*piu_ptr) {
			case E_PKUP_ST_TYPE:
				temp_ptr = call_struct.pckup_str_type;
				break;
			case E_INTERS_ST_TYPE:
				temp_ptr = call_struct.intr_str_type;
				break;
			case E_DEST_ST_TYPE:
				temp_ptr = call_struct.dest_str_type;
				break;
			default:
				prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_22, "<check_str_type called>"));
		}
	}
	if (*field_entered) {					/* field entered */
		been_touched = TRUE;
		if (read_buf[0] == ' ') 			/* if 1st char entered is space, then wipe out old entry */
			*temp_ptr = '\0';			/* erase old entry */
		else {						/* field entered; verify it; by reading the street type file */
			if (strcmp(temp_ptr,read_buf) == 0)	/* user did not modify an existing entry ? */
				return(update_piu(ptr_scr_fld,piu_ptr,key_val));
				
			str_type_ptr = (struct cisam_st *)req_buf;
			strcpy(str_type_ptr->abbrev,read_buf);
			if (db(STRTYPE_FILE_ID,READ_KEYS,&st_key1,ISEQUAL,0) < 0) {		/* invalid street type entered */
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_23, "<street type>"));
				zone_call(ptr_scr_fld,*piu_ptr);
				return(DONE);
			}
			strcpy(temp_ptr,str_type_ptr->actual);			/* got right street type; get actual street type */
			attrset(A_UNDERLINE);					/* display actual street on screen */
			clear_field(stdscr,ptr_scr_fld,*piu_ptr);
			mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, temp_ptr);
		}
		zone_call(ptr_scr_fld,*piu_ptr);
		return(update_piu(ptr_scr_fld,piu_ptr,key_val));
	}
	else
		return(update_piu(ptr_scr_fld,piu_ptr,key_val));
#else
        return(DONE);
#endif
}

/**********************************************************************************************/
/* check_apt_no - verify apartment number field						      */
/**********************************************************************************************/
int check_apt_no(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;			/* points to piu field */
	char *temp_ptr;					/* pointer to call_struct field */
	int old_piu;
	int ret_val;
	int check_inters_flag = 0;	/* should check inters_flag, since should only read dest address if inters_flag is set */


	ptr_to_fld = ptr_scr_fld + (*piu_ptr);
									/* assign temp_ptr to call_struct field being read */
	if (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR) {
		switch (*piu_ptr) {
			case B_PKUP_APT_NO:
				temp_ptr = call_struct.pckup_apt_room;
				check_inters_flag = 1;
				break;

			case B_DEST_APT_NO:
				temp_ptr = call_struct.dest_apt_room;
				break;

			default:
				prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_24, "<check_apt_no() called>"));
		}
	}
	else if (scr_name==EXTENDED_CALL_SCR || scr_name== SUBS_EXTENDED_SCR) {
		switch (*piu_ptr) {
			case E_PKUP_APT_NO:
				temp_ptr = call_struct.pckup_apt_room;
				check_inters_flag = 1;
				break;

			case E_DEST_APT_NO:
				temp_ptr = call_struct.dest_apt_room;
				break;

			default:
				prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_24, "<check_apt_no() called>"));
		}
	}
	
	if (*field_entered) {						/* field entered */
		been_touched = TRUE;
		if (read_buf[0] == ' ') 				/* if 1st char is a space then wipe out old entry */
			*temp_ptr = '\0';
		else							/* new entry */
			strcpy(temp_ptr,read_buf);
	}

	old_piu = *piu_ptr;						/* update piu */
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	if ((*piu_ptr == (old_piu -1)) && (check_inters_flag)) {	/* should read dest address only if inters_flag is set */
		if (!inters_flag)					/* skip dest address fields */
			*piu_ptr = old_piu - 5;
	}
	return(ret_val);
}

/**************************************************************************************************/
/* check_city - verify entry in the city field							  */
/**************************************************************************************************/
int check_city(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;  						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;
	char *temp_ptr;
	short *zone_temp;				/* pointer to call_struct zone # field */
	short zone_num;
	struct cisam_lo *city_ptr;			/* for disk i/o */
	int old_piu;					/* to save this piu value */
	int ret_val;					/* return code from update_piu() */
	int cannot_zone_fld=1;				/* field to move user to if not allowed to zone calls */
	int pkup_or_dest;				/* which address should be zoned */
	int i;					/* scratch variable */

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR) {
		switch (*piu_ptr) {
			case B_PKUP_CITY:
				temp_ptr = call_struct.pckup_city;
				zone_temp = (short *)&call_struct.pckup_zone;
				cannot_zone_fld = B_PASSENGER;
				pkup_or_dest =  PICKUP_ADDR;
				break;

			case B_DEST_CITY:
				temp_ptr = call_struct.dest_city;
				zone_temp = (short *)&call_struct.dest_zone;
				cannot_zone_fld = B_PERSONEL1;
				pkup_or_dest = DEST_ADDR;
				break;

			default:
				prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_25, "<check_city() called>"));
		}
	}
	else if(scr_name==EXTENDED_CALL_SCR || scr_name==SUBS_EXTENDED_SCR) {
		switch (*piu_ptr) {
			case E_PKUP_CITY:
				temp_ptr = call_struct.pckup_city;
				zone_temp = (short *)&call_struct.pckup_zone;
				cannot_zone_fld = E_PASSENGER;
				pkup_or_dest =  PICKUP_ADDR;
				break;

			case E_DEST_CITY:
				temp_ptr = call_struct.dest_city;
				zone_temp = (short *)&call_struct.dest_zone;
				cannot_zone_fld = E_SEE;
				pkup_or_dest = DEST_ADDR;
				break;

			default:
				prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_25, "<check_city() called>"));
		}
	}

	if (!*field_entered) {						/* city not entered; use previouly entered city */
		if (*temp_ptr != '\0') {
			attrset(A_UNDERLINE);
			mvaddstr(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,temp_ptr);
			attrset(0);
		}
	}
	else { 								/* city entered; check entry */
		been_touched = TRUE;
		city_ptr = (struct cisam_lo *)req_buf;
		strcpy(city_ptr->code,read_buf);
		if ( (db(LOCALTY_FILE_ID,READ_KEYS,&lo_key1,ISEQUAL,0) < 0)  &&
                     ((*piu_ptr == E_PKUP_CITY) || (*piu_ptr == B_PKUP_CITY)) )
                {
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_26, "<city>")); 		/* invalid city entered */
			old_piu = *piu_ptr;
			ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
			*piu_ptr = old_piu; 				/* trap user in city field */
			return(ret_val);				/* leave user in field */
		}
		
		if ( strcmp( city_ptr->code, temp_ptr ) )
		  {
                    if ( pkup_or_dest == PICKUP_ADDR )
                      {
                        call_struct.gps_lat = 0.0;
                        call_struct.gps_long = 0.0;
                      }
                    else
                      {
                        call_struct.dest_gpsx = 0.0;
                        call_struct.dest_gpsy = 0.0;
                      }
		  }
		
		strcpy(temp_ptr,read_buf);				/* copy validated city into call structure */
	}

	if ((pkup_or_dest == PICKUP_ADDR && strlen(call_struct.pckup_str_name) > 0) ||
	    (pkup_or_dest == DEST_ADDR && strlen(call_struct.dest_str_name) > 0)){		/* do we have an address to zone */
		if (*field_entered && city_ptr->subzone[0] != '\0'){			/* no addresses for this city? */
			if(pkup_or_dest == PICKUP_ADDR)
				zip1 = city_ptr->zip;
			else if(pkup_or_dest == DEST_ADDR)
				zip2 = city_ptr->zip;
			zone_num = sz_to_zone(call_struct.fleet, city_ptr->subzone);	/* get zone for city */
		}
		else{
			if(pkup_or_dest == PICKUP_ADDR)
			  {
			    zone_num = zone_addr(&call_struct,pkup_or_dest,&zip1);	/* regular address, zone it */
			    if (scr_name == EXTENDED_CALL_SCR)
			      {
				print_addr_comment(C_PRINT, PICKUP_ADDR);
			      }
			  }
			else if(pkup_or_dest == DEST_ADDR)
			  {
			    zone_num = zone_addr(&call_struct,pkup_or_dest,&zip2);	/* regular address, zone it */
			    if (scr_name == EXTENDED_CALL_SCR)
			      {
				print_addr_comment(C_PRINT, DEST_ADDR);
			      }
			  }
		}

		if (zone_num < 0) { 								/* cannot zone call ? */

		        /* While updating don't change a unzoned call */
		        if (doing_update && uiuser[user_index]->can_zone_calls == YES)
			  {
			    ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
			    return(ret_val);
			  }

			if (pkup_or_dest == PICKUP_ADDR)
				call_zoned = 0;							/* call not zoned */
			*zone_temp = (short)0;							/* reset zone # to 0 = unzoned */
			attrset(A_UNDERLINE); 			/* clear any old entry which might be there in the zone field */
			move((ptr_to_fld+1)->prompt_row,(ptr_to_fld+1)->prompt_col);
			for (i = 0; i < (ptr_to_fld+1)->max_chars; i++)
				addch(' ');
			attrset(0);
			prt_error(UI_NOZONE, NULL);
			user_knows = 1;					/* user knows that call is not zoned */
			get_zone_desc(C_CLEAR, pkup_or_dest, &call_struct, NULL); 	/* clear old entry */
			print_addr_comment(C_CLEAR,pkup_or_dest);
			get_rate(C_CLEAR,NO,&zip1,&zip2);				/* sets rate = 0.0 */
			if (pkup_or_dest == PICKUP_ADDR) 
				get_est_wait_time(0);
			if (uiuser[user_index]->can_zone_calls == YES) {
				old_piu = *piu_ptr; 			/* user can zone calls, place him in the zone field */
				ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
				return(ret_val);
			}
			else  { 					/* user cannot zone calls; place user in the approp field */
				old_piu = *piu_ptr;
				ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
				if (*piu_ptr == old_piu + 1)
					*piu_ptr = cannot_zone_fld;
				return(ret_val);
			}
		}
		else { 							/* call zoned; print zone number, etc on screen */
		        attrset(A_UNDERLINE);
			mvprintw((ptr_to_fld+1)->prompt_row,(ptr_to_fld+1)->prompt_col, "%3d", zone_num);
			attrset(0);
			*zone_temp = (short)zone_num;
			get_zone_desc(C_PRINT, pkup_or_dest, &call_struct, NULL); /* get the zone description from the zone file */
			print_addr_comment(C_PRINT,pkup_or_dest);
			if ( pkup_or_dest == PICKUP_ADDR )  {
				get_est_wait_time(1); 			/* print estimated ewait time on scr, and save it */
				call_zoned = 1;				/* set flag that call is zoned */
			}

			get_rate(C_PRINT,YES,&zip1,&zip2); 			/* print the rate from the rate file, if applicable */

			old_piu = *piu_ptr;
			ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
/******
/*			if (*piu_ptr == old_piu + 1)
/*				*piu_ptr += 1; 				/* now on zone field; no need to skip it */
/*****/
			return(ret_val);
		}  							/* end else ... call zoned */
	}
	else { 								/* no need to zone -> no street name */
		if (uiuser[user_index]->can_zone_calls == YES || uiuser[user_index]->can_zone_calls == YES) {
			old_piu = *piu_ptr; 				/* user can zone calls, place him in the zone field */
			ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
			return(ret_val);
		}
		else  { 						/* user cannot zone calls; place user in the approp field */
			old_piu = *piu_ptr;
			ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
			if (*piu_ptr == old_piu + 1)
				*piu_ptr = cannot_zone_fld;
			return(ret_val);
		}
	}
}

/*****************************************************************************************/
/* check_zone - verify entry of the zone field						 */
/*****************************************************************************************/
int check_zone(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;
	short *temp_zone;
	short *fifo_temp;				/* pointer to call_fifo zone # field */
	short zone_num;
	struct cisam_zn *zone_file_ptr;	/* for disk i/o */
	struct fleets *fl_ptr;
	int i;
	int ret_val;					/* return code from update_piu() */
	int old_piu;
	char pkup_or_dest;				/* whether pkup or dest address being zoned */

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR) {
		switch (*piu_ptr) {
			case B_PKUP_ZONE:
				temp_zone = (short *)&call_struct.pckup_zone;
				fifo_temp = (short *)&call_fifo.pickup_zone;
				pkup_or_dest = PICKUP_ADDR;
				break;

			case B_DEST_ZONE:
				temp_zone = (short *)&call_struct.dest_zone;
				fifo_temp = (short *)&call_fifo.dest_zone;
				pkup_or_dest = DEST_ADDR;
				break;

			default:
				prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_27, "<check_zone() called>"));
		}
	}
	else if(scr_name==EXTENDED_CALL_SCR || scr_name==SUBS_EXTENDED_SCR) {
		switch (*piu_ptr) {
			case E_PKUP_ZONE:
				temp_zone = (short *)&call_struct.pckup_zone;
				fifo_temp = (short *)&call_fifo.pickup_zone;
				pkup_or_dest = PICKUP_ADDR;
				break;

			case E_DEST_ZONE:
				temp_zone = (short *)&call_struct.dest_zone;
				fifo_temp = (short *)&call_fifo.dest_zone;
				pkup_or_dest = DEST_ADDR;
				break;

			default:
				prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_27, "<check_zone() called>"));
		}
	}

	if (!*field_entered) {						/* zone not entered */
		return(update_piu(ptr_scr_fld,piu_ptr,key_val));
	}
	else {								/* zone entered; check if has permissins to zone a call */
		if (uiuser[user_index]->can_zone_calls != YES) {
			prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_28, "<to zone calls>"));
			return(update_piu(ptr_scr_fld,piu_ptr,key_val));
		}
		i = 0;							/* user can zone calls; verify zone # entered */
		while (i < ptr_to_fld->max_chars && read_buf[i] != '\0') {
			if (!isdigit(read_buf[i]) && read_buf[i] != ' ') {
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_29, "<zone #>"));
				get_zone_desc(C_CLEAR, pkup_or_dest, &call_struct, NULL);	/* clear the zone desc area */
				print_addr_comment(C_CLEAR,pkup_or_dest);
				if (pkup_or_dest == PICKUP_ADDR) 
					get_est_wait_time(0);
				return(DONE);
			}
			i++;
		}

		if (*temp_zone == (short)atoi(read_buf)) {		/* zone number did not change ? */
			return(update_piu(ptr_scr_fld,piu_ptr,key_val));
		}
		get_rate(C_CLEAR, NO, NULL, NULL );
		zone_num = (short)atoi(read_buf);
		
		if ( zone_num != *temp_zone )
		  {
		    call_struct.gps_lat = 0.0;
		    call_struct.gps_long = 0.0;
		  }
		
		zone_num = (short)atoi(read_buf);
		zone_file_ptr = (struct cisam_zn *)req_buf;
		zone_file_ptr->fleet = call_struct.fleet;
		zone_file_ptr->nbr = zone_num;

		if (db(ZONE_FILE_ID,READ_KEYS,&zn_key1,ISEQUAL,0) < 0) {			/* invalid zone entered */
		  prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_29, "<zone #>"));
		  get_zone_desc(C_CLEAR, pkup_or_dest, &call_struct, NULL);			/* get zone desc */
		  print_addr_comment(C_CLEAR,pkup_or_dest);
		  get_rate(C_CLEAR,NO,&zip1,&zip2);						/* clear the rate field */
		  if (pkup_or_dest == PICKUP_ADDR) 
		    get_est_wait_time(0);
		  old_piu = *piu_ptr;
		  ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
		  *piu_ptr = old_piu;
		  return(ret_val);							/* leave user in field */
		}

		if ( ( zone_file_ptr->gps_lat != 0.0 ) && ( zone_file_ptr->gps_long != 0.0 ) )
		  {
		    call_struct.gps_lat = zone_file_ptr->gps_lat;
		    call_struct.gps_long = zone_file_ptr->gps_long;
		  }
		
		call_zoned = 1;
		*temp_zone = zone_num;
		*fifo_temp = zone_num;
		get_zone_desc(C_PRINT, pkup_or_dest, &call_struct, zone_file_ptr);
		print_addr_comment(C_PRINT,pkup_or_dest);
		if (pkup_or_dest == PICKUP_ADDR) 
			get_est_wait_time(1);
		get_rate(C_PRINT,YES,&zip1,&zip2);			/* print the rate from the rate file, if applicable */

/*
 *	restricted time calls
 */
		if ((scr_name == BASIC_CALL_SCR || scr_name == EXTENDED_CALL_SCR))
		    tcr_check_time_call_restriction(&call_struct, FALSE, key_val);
		
		return(update_piu(ptr_scr_fld,piu_ptr,key_val));
      }
} 

int check_passenger(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
	struct scr_flds *ptr_scr_fld;	/* pointer to array of piu structures */
	int *piu_ptr;  	/* pointer to piu */
	char *read_buf;	/* pointer to buffer for reading field */
	int key_val;	/* value of key pressed by user */
	int *field_entered;	/* whether field entered =1; or not=0 */
{


	struct scr_flds *ptr_to_fld;	/* pointer to approp. piu struct */
	char *temp_ptr;	/* pointer to approp. field in call_struct */
	char *ptr_pass_key;	/* used for saving key */
	int old_piu, ret_val;
	
	ptr_to_fld = ptr_scr_fld + (*piu_ptr);
	ptr_pass_key = NULL;

	if ( scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR ||
		scr_name == WAKEUP_CALL_SCR || scr_name == SUBS_WAKEUP_SCR)  {
          switch ( *piu_ptr ) {
          case B_PASSENGER:
            temp_ptr = call_struct.passenger_name;
            ptr_pass_key = call_struct.pass_name_key;
            break;
          case W_NAME:
            temp_ptr = call_struct.passenger_name;
            ptr_pass_key = call_struct.pass_name_key;            
            break;
          case B_REQBY:
            temp_ptr = call_struct.requested_by;
            break;
          default:
            prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_30, "<check_passenger()called>"));
            break;
          }
	}
	else {
		/* scr is extended type */
		switch ( *piu_ptr ) {
			case E_PASSENGER:
				temp_ptr = call_struct.passenger_name;
				ptr_pass_key = call_struct.pass_name_key;
				break;
			case E_REQ_BY:
				temp_ptr = call_struct.requested_by;
				break;
			case E_SEE:
				temp_ptr = call_struct.person_to_see;
				break;
			case E_PODNAME:
				temp_ptr = call_struct.pod_name;
				break;
			default:
				prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_30, "<check_passenger()called>"));
		}  /* end switch */
	}  /* end else ... extended scr */

	if ( *field_entered ) {
		/* user entered field */
		/* if 1st char is a space, then wipe out previous entry */
		if ( *read_buf == ' ')  {
			/* user wants to erase previous entry */
			*temp_ptr = '\0';
			if ( ptr_pass_key != NULL) 
				*ptr_pass_key = '\0';
		}
		else  {
			/* new entry */
			strcpy(temp_ptr, read_buf);
			if ( ptr_pass_key != NULL ) {
				strncpy(ptr_pass_key,temp_ptr,KEY_LENGTH);
				ptr_pass_key[KEY_LENGTH] = '\0';
			}
		}  /* end else */

	} /* end if... field entered  */

	old_piu = *piu_ptr;
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	if ( *piu_ptr == B_PKUP_ZONE || *piu_ptr == B_DEST_ZONE  ||  *piu_ptr == E_PKUP_ZONE  ||  *piu_ptr == E_DEST_ZONE )  {
		/* user is trying to go to the zone field, check he has permissions for zoning calls */
		if ( uiuser[user_index]->can_zone_calls != YES ) 
			/* cannot zone calls; skip this field and go to city field, */
			*piu_ptr = old_piu - 2;
	}  /* end if */
	else if ( *piu_ptr ==W_DATE && scr_name == SUBS_WAKEUP_SCR )
		/* not a data entry field on subs screens, so skip it */
		*piu_ptr = W_PH_AREA_CODE;	/* go to phone area number field */

	return(ret_val);

}  /* end check_passenger() */


int check_phone(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered)
struct scr_flds *ptr_scr_fld;			/* pointer to array of piu structures */
int *piu_ptr;					/* pointer to piu */
char *read_buf;					/* pointer to buffer for reading field */
int key_val;					/* value of key pressed by user */
int *field_entered;				/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;		/* pointer to approp. piu struct */
	char *temp_ptr;				/* pointer to approp. field in call_struct */
	char temp_num[5];			/* for temp phone # */
	char temp_char[2];			/* single temporary character for phone number translation */
	int i;
	int check_area_code=0;		/* should check area code, if phone # is entered */
	int is_area_code=0;			/* is this area code field */
	int old_piu, ret_val, rc;



	int j, len;

#ifdef SCANDINAVIA
	  {
	    /* Strip out any non alphanumeric characters */
	    len = strlen(read_buf);
	    for (i = 0, j = 0; i < len; i ++)
	      if (isalnum(read_buf[i]))
		call_struct.phone_number[j ++] = read_buf[i];
	    call_struct.phone_number[j] = '\0';
	    
	    /* Copy first 7 digits of Scandinavian phone # into keyed phone # location */
	    strncpy(call_struct.phone_nbr, call_struct.phone_number, 7);
	    call_struct.phone_nbr[7] = '\0';

	    ptr_to_fld = ptr_scr_fld + (*piu_ptr);            
            attrset(A_UNDERLINE); 								/* print entry on screen */
            if ( strcmp( read_buf, "0" ) )
	      {
		for (i=0; i < ptr_to_fld->max_chars; i++)
		  addch(' ');
		mvaddstr(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,read_buf);
	      }
	    
            attrset(0);
              
	    if ((line_mgr_attached) && (LineMgrIndex >= 0) && (LineMgrIndex < MAX_PIDS) &&
		(1) && (scr_name != WAKEUP_CALL_SCR) &&
                (scr_name != EXTENDED_CALL_SCR) && (scr_name != SUBS_WAKEUP_SCR))
	      {
		if ( (!doing_update) && (!bPhoneFieldVisited) && ( strlen(call_struct.phone_nbr) ) )
		  {
                    if ( !strncmp( call_struct.phone_number, "0000000000", 10 ) )
                      {
                        attrset(A_REVERSE);
                        mvprintw(23, 0, " %s ", catgets(UI_m_catd, UI_1, UI_TEXT_72, "A-Number Unavailable"));
                        attrset(0);
                        bzero( call_struct.phone_number, sizeof(call_struct.phone_number) );
                      }
                    else if ( !strcmp( call_struct.phone_number, "0" ) )
                      {
			attrset(A_REVERSE);
			mvprintw(23, 0," %s " , catgets(UI_m_catd, UI_1, UI_TEXT_72, "A-Number Unavailable"));
                        bzero( call_struct.phone_number, sizeof(call_struct.phone_number) );
			*piu_ptr = B_FLEET + 1; /* Move to street number field if no profile */
			attrset(0);
		      }                        
		    else if ((rc = DoCustomerProfile(ptr_scr_fld, piu_ptr) == 1))
		      {
			attrset(A_REVERSE);
			mvprintw(23, 0," %s ", catgets(UI_m_catd, UI_1, UI_TEXT_71, "Repeat Customer"));
			attrset(0);
                        if ( call_struct.pckup_zone > 0 )
                          *piu_ptr = B_DATE;
                        else
                          *piu_ptr = B_TYPE;                        
		      }
		    else
		      {
			attrset(A_REVERSE);
			mvprintw(23, 0," %s " , catgets(UI_m_catd, UI_1, UI_TEXT_70, "Ny kund"));
			*piu_ptr = B_FLEET + 1; /* Move to street number field if no profile */
			attrset(0);
		      }
		    bPhoneFieldVisited = 1;
                    EventFlag = FALSE;
		  }
	      }
	    ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	    return(ret_val);
	    
	  }
#else
	  {

	    ptr_to_fld = ptr_scr_fld + (*piu_ptr);
	    
	    if ( scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR ) {
	      switch ( *piu_ptr ) {
	      case B_PH_AREA_CODE:
		temp_ptr = call_struct.phone_area;
		is_area_code = 1;
		break;
	      case B_PH_NUM1:
		temp_ptr = call_struct.phone_nbr;
		check_area_code = 1;
		break;
	      case B_PH_NUM2:
		temp_ptr = &call_struct.phone_nbr[3];
		break;
	      default:
		prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_31, "<check_phone()called>"));
	      }  									/* end switch */
	    }  										/* end else ... basic scr */
	    else if (scr_name== EXTENDED_CALL_SCR || scr_name== SUBS_EXTENDED_SCR){
	      switch ( *piu_ptr ) {
	      case E_PH_AREA_CODE:
		temp_ptr = call_struct.phone_area;
		is_area_code = 1;
		break;
	      case E_PH_NUM1:
		temp_ptr = call_struct.phone_nbr;
		check_area_code = 1;
		break;
	      case E_PH_NUM2:
		temp_ptr = &call_struct.phone_nbr[3];
		break;
	      default:
		prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_31, "<check_phone()called>"));
	      }  								/* end switch */
	    }  									/* end else ... extended scr */
	    else if (scr_name== WAKEUP_CALL_SCR || scr_name== SUBS_WAKEUP_SCR ) {
	      switch ( *piu_ptr ) {
	      case W_PH_AREA_CODE:
		temp_ptr = call_struct.phone_area;
		is_area_code = 1;
		break;
	      case W_PH_NUM1:
		temp_ptr = call_struct.phone_nbr;
		check_area_code = 1;
		break;
	      case W_PH_NUM2:
		temp_ptr = &call_struct.phone_nbr[3];
		break;
	      default:
		prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_31, "<check_phone()called>"));
	      }								/* end switch */
	    }									/* end else ... wakeup scr */
	    
	    if (*field_entered) { 						/* user entered field */
	      if (read_buf[0] == ' ') { 		/* if 1st char is a space, then wipe out old phone # and area code */
		if (is_area_code) { 		/* print default area code */
		  if (fleet[cur_fl_index]->default_phone_area != '\0' && strcmp(fleet[cur_fl_index]->default_phone_area,"000")) 
		    mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col,fleet[cur_fl_index]->default_phone_area);
		}
		else  {
		  call_struct.phone_area[0] = '\0';
		  *temp_ptr = '\0';
		}
		old_piu = *piu_ptr;
		ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
		if ( *piu_ptr == W_TIME  && scr_name == SUBS_WAKEUP_SCR ) 	/* not a data entry field, so skip it */
		  *piu_ptr = old_piu - 3;  				/* go to name field */
		return(ret_val);
	      }  									/* end if .. wipe out old entry */
	      
	      if (call_struct.phone_area[0] == '\0') { 			/* new entry, copy area code in the call_struct */
		if(fleet[cur_fl_index]->default_phone_area != '\0' && strcmp(fleet[cur_fl_index]->default_phone_area,"000"))
		  strcpy(call_struct.phone_area,fleet[cur_fl_index]->default_phone_area);		/* save area code */
	      }  												/* end if */
	      
	      i = 0;
	      while ( i < ptr_to_fld->max_chars && read_buf[i] != '\0' ) {
		if ( !isalpha(read_buf[i]) && !isdigit(read_buf[i]) ) { 		/* wrong entry, still save it */
		  strcpy(temp_ptr,read_buf);
		  prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_32, "<phone #>"));
		  return(DONE);
		  /*				old_piu = *piu_ptr;
						/*				ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
										/*				if ( *piu_ptr == W_TIME  && scr_name == SUBS_WAKEUP_SCR ) /* not a data entry field, so skip it */
										  /*					*piu_ptr = old_piu - 3;  			/* go to name field */
		  /*				return(ret_val); */
		}  								/* end if */
		
		if (isalpha(read_buf[i])) {
		  if((read_buf[i] - 64) > 16) 				/* convert alpha to phone digits */
		    sprintf(temp_char, "%c", (((4+(read_buf[i]-64))/3)+'0'));	/* R - Y */
		  else
		    sprintf(temp_char, "%c", (((5+(read_buf[i]-64))/3)+'0'));	/* A - P */
		  
		  if(read_buf[i] == 'Q' || read_buf[i] == 'Z') {
		    prt_error(UI_NOFIELD, catgets(UI_m_catd, UI_1, UI_CHECK_FLD_33,
						  "<letters Q and Z are not valid in a phone number>"));
		    return(DONE);
		  }
		  temp_num[i] = temp_char[0];
		}
		else 									/* its a digit */
		  temp_num[i] = read_buf[i];
		
		i++;
	      }  										/* end while */
	      temp_num[i] = '\0';
	      
	      if ( temp_ptr == call_struct.phone_nbr )  					/* save entry */
		strncpy(temp_ptr,temp_num,3);
	      else
		strcpy(temp_ptr,temp_num);
	      
	      switch(*piu_ptr)
		{
		case B_PH_NUM2:
		  if ((line_mgr_attached) && (LineMgrIndex >= 0) && (LineMgrIndex < MAX_PIDS) && (1))
		    {
                      //signal(SIGCONT, SIG_IGN);
		      if (!bPhoneFieldVisited && !doing_update)
			{
                          if ( !strncmp( call_struct.phone_number, "0000000000", 10 ) )
                            {
                              attrset(A_REVERSE);
                              //mvprintw(23, 0, " %s ", catgets(UI_m_catd, UI_1, UI_TEXT_72, "A-Number Unavailable"));
                              attrset(0);
                              bzero( call_struct.phone_number, sizeof(call_struct.phone_number) );
			      *piu_ptr = B_PKUP_ST_NAME - 1; /* Move to street number field if no profile */
                            }
			  else if ((rc = DoCustomerProfile(ptr_scr_fld, piu_ptr) > 0))
			    {
			      //attrset(A_REVERSE);
			      //mvprintw(23, 0," %s ", catgets(UI_m_catd, UI_1, UI_TEXT_71,"Repeat Customer"));
			      //attrset(0);
                              if ( call_struct.pckup_zone > 0 )
                                *piu_ptr = B_TIME;
                              else
                                *piu_ptr = B_COMMENT - 1;
			    }
			  else
			    {
			      //attrset(A_REVERSE);
			      //mvprintw(23, 0," %s " ,catgets(UI_m_catd, UI_1, UI_TEXT_70,"Ny kund"));
			      *piu_ptr = B_PKUP_ST_NAME - 1; /* Move to street number field if no profile */
			      //attrset(0);
			    }
			  bPhoneFieldVisited = 1;
			}
		    }
		  break;
		default:
		  attrset(A_UNDERLINE); 								/* print entry on screen */
		  mvaddstr(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,temp_num);
		  attrset(0);
		  break;
		}
	      
	      attrset(A_UNDERLINE); 								/* print entry on screen */
	      mvaddstr(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,temp_num);
	      attrset(0);
	      
	    }  											/* end else ... field entered */
	    
	    old_piu = *piu_ptr; 									/* update piu */
	    ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	    if ( *piu_ptr == W_TIME  && scr_name == SUBS_WAKEUP_SCR ) 			/* not a data entry field, so skip it */
	      *piu_ptr = old_piu - 3;  						/* go to name field */
	    return(ret_val);
	  }
#endif
}  											/* end check_phone() */

	
int check_ph_ext(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
	struct scr_flds *ptr_scr_fld;	/* pointer to array of piu structures */
	int *piu_ptr;  	/* pointer to piu */
	char *read_buf;	/* pointer to buffer for reading field */
	int key_val;	/* value of key pressed by user */
	int *field_entered;	/* whether field entered =1; or not=0 */
{

	struct scr_flds *ptr_to_fld;	/* pointer to approp. piu struct */
	int i;
	int old_piu, ret_val;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if ( *field_entered )  {

		if ( read_buf[0] == ' ')  
			/* user wants to wipe out old entry, if any */
			memset(call_struct.phone_ext,'\0',ptr_to_fld->max_chars+1);

		else {

		    for (i = 0; i < strlen(read_buf); i ++) {

			if (!isdigit(read_buf[i])) {

			    if (read_buf[i] == 'Q' || read_buf[i] == 'Z') {
				prt_error(UI_NOFIELD, catgets(UI_m_catd, UI_1, UI_CHECK_FLD_33,
							      "<letters Q and Z are not valid in a phone number>"));
				return(DONE);
			    }

			    else if (!isalpha(read_buf[i])) {
				prt_error(UI_NOFIELD, catgets(UI_m_catd, UI_1, UI_CHECK_FLD_32, "<phone #>"));
				return(DONE);
			    }

			    else if (read_buf[i] <= 'P')
				read_buf[i] = (((5+(read_buf[i]-64))/3)+'0');	/* A - P */
			    else
				read_buf[i] = (((4+(read_buf[i]-64))/3)+'0');	/* R - Y */
			}

			strcpy(call_struct.phone_ext,read_buf);

			/* Print on screen */
			attrset(A_UNDERLINE);
			mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, read_buf);
			attrset(0);
		    }

		}  /* end else ... new entry */

	}  /* end if .... field entered */

	old_piu = *piu_ptr;
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	if ( (*piu_ptr == B_DATE && scr_name == SUBS_BASIC_SCR)  || (*piu_ptr == E_DATE && scr_name == SUBS_EXTENDED_SCR) ) 
		/* on subscription the pkup date & time fields are not data entry fields */
		*piu_ptr = old_piu + 3;	/* go to comment field */
	return(ret_val);

}	/* end check_ph_ent() */

/********************************************************************************************/
/* check_date - verify date field							    */
/********************************************************************************************/
int check_date(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered)
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;			/* pointer to piu struct being processed */
	int old_piu;
	int ret_val;					/* return code from update_piu() */
	char temp_date[12];				/* to save format of time entered */
	struct tm *pDateTime;
	char  *pDateDescript;
	time_t tmp_datetime;
	
	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

							/* cannot be called from subscription screens */
	if (scr_name == SUBS_BASIC_SCR || scr_name == SUBS_EXTENDED_SCR || scr_name == SUBS_WAKEUP_SCR) {
		prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_34, "<check_date() called>"));						/* should not be called */
		return(update_piu(ptr_scr_fld,piu_ptr,key_val));
	}

	strncpy(temp_date,read_buf,11);
	temp_date[11] = '\0';

	if (!*field_entered) {					/* field not entered; if no previous date, then trap user */
		if (call_struct.due_date[0] == '\0') {		/* no previous date */
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_35, "<date>"));
			return(DONE);
		}
	}
	else {										/* new entry; verify it */
		if (strcmp(read_buf, call_struct.due_date) != 0)
		  {
		    user_changed_dt_tm = 1;					/* user changed time field */
		  }

		if (user_changed_dt_tm) {					/* user did change date-time ? */
			if (verify_date(read_buf,call_struct.due_date) == -1) {	/* invalid date entry */
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_36, "<date field>"));
				return(DONE);
			}
			else {				/* valid date entered; it is already saved in call_struct.due-date */
				 			/* by verify_date() .. format in which date is saved is mm dd yy */
				
				if (strncmp(temp_date,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_37, "NOW"),3) == 0 ||
				    strcmp(temp_date, " ")  == 0)
				{
					get_pkup_date(call_struct.due_date);
					get_pkup_time(call_struct.due_time);
					user_changed_dt_tm = 0;					/* default time desired */
					attrset(A_UNDERLINE);
					mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, call_struct.due_date);
					mvaddstr((ptr_to_fld+1)->prompt_row, (ptr_to_fld+1)->prompt_col, call_struct.due_time);
					attrset(0);
				}
				if (has_date_passed(call_struct.due_date)) { 			/* check if date has past or not */
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_38, "<date entered has past>"));	/* date entered is past */
					return(DONE);
				}
				else {								/* valid date */
					if (get_internal_time(call_struct.due_date,call_struct.due_time) > 
					    (time(NULL) + fleet[cur_fl_index]->future_call_maximum)) {
						prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_39, "<date entered too far into the future>"));
						return(DONE);
					}

					time_subs_call();					/* print this date in the field */
/*
 *	restricted time calls
 */
				if (scr_name == BASIC_CALL_SCR || scr_name == EXTENDED_CALL_SCR)
					tcr_check_time_call_restriction(&call_struct, FALSE, key_val);
                                
					if (!doing_update) {
                                          if ( call_struct.priority <= get_call_pri() )
                                            {
                                              if ( call_struct.extended_type[2] == TIMEC )
                                                call_struct.pri_status = 63;
                                            }
                                          else
                                            {
                                              call_struct.priority = 0;				/* don't care about last priority */
                                              if((call_struct.priority = get_call_pri()) == -1) 	/* get call priority and save it */
                                                call_struct.priority = fleet[cur_fl_index]->passenger_priority_def;
                                            }
                                              
                                          if (call_struct.extended_type[2] == TIMEC)
                                            call_struct.pri_status = 63;			/* PENDING time call */
                                          else
                                            call_struct.pri_status = call_struct.priority;

					}
                                        else {
                                          if ( ( call_struct.extended_type[2] == TIMEC ) ||
                                               ( ( call_struct.extended_type[2] == SUBS ) && ( call_struct.pri_status == 63 ) ) )
                                            {
                                              if ( call_struct.priority < get_call_pri() )
                                                call_struct.pri_status = 63;
                                              else
                                                {
                                                  call_struct.pri_status = 63;
                                                  call_struct.priority = get_call_pri();
                                                }
                                            }
                                        }
                                        
					attrset(A_UNDERLINE);
					print_priority();					/* prints call priority */
					mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, call_struct.due_date);
					attrset(0);
				}
			}
		}
		else {							/* user did not change the date, default it */
			if (!doing_update) {
				get_pkup_date(call_struct.due_date);
				attrset(A_UNDERLINE);
				mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, call_struct.due_date);
				attrset(0);
			}
		}
	}
	if ( scr_name == BASIC_CALL_SCR || scr_name == EXTENDED_CALL_SCR )
	  {

	    tmp_datetime = (time_t)call_struct.due_date_time;
	    pDateTime = localtime( &tmp_datetime );
	    if ( pDateTime != NULL )
	      {
		pDateDescript = nl_langinfo( DAY_1 + pDateTime->tm_wday );
		if ( pDateDescript != NULL )
		  {
		    if ( scr_name == BASIC_CALL_SCR )
		      mvprintw( B_PKUP_DATE_ROW, B_PKUP_DATE_COL + 24, "%s  ", pDateDescript );
		    else
		      mvprintw( E_PKUP_DATE_ROW, E_PKUP_DATE_COL + 24, "%s  ", pDateDescript );
		  }
	      }

	  }
	
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);

	/* should skip rate field, if user is not allowed to enter rate */
	if (*piu_ptr == B_RATE && (scr_name == BASIC_CALL_SCR ||  scr_name == SUBS_BASIC_SCR)) {
		if (!get_rate(C_PRINT,NO,&zip1,&zip2))			/* cannot enter rate */
			*piu_ptr = B_HOLDLINE;
	}

	if (*piu_ptr == E_RATE && ( scr_name == EXTENDED_CALL_SCR ||  scr_name == SUBS_EXTENDED_SCR)) {
#ifdef FOO
		if (!get_rate(C_PRINT,NO,&zip1,&zip2))				/* cannot enter rate */
			*piu_ptr = E_HOLDLINE;
#endif
	}
	get_est_wait_time(1);
	return(ret_val);
}

/**************************************************************************************/
/* check_time - verify time entered field					      */
/**************************************************************************************/
int check_time(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;			/* pointer to array of piu structures */
int *piu_ptr;					/* pointer to piu */
char *read_buf;					/* pointer to buffer for reading field */
int key_val;					/* value of key pressed by user */
int *field_entered;				/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;		/* pointer to piu struct being processed */
	int old_piu, ret_val;
	char temp_time[10];			/* to save time format entered */
	int invalid_time=0;			/* 1=invalid time entered */

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

										/* cannot be called from subscription screens */
	if (scr_name == SUBS_BASIC_SCR || scr_name == SUBS_EXTENDED_SCR || scr_name == SUBS_WAKEUP_SCR) {
		prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_40, "<check_time() called>")); 			/* should not be called */
		return(update_piu(ptr_scr_fld,piu_ptr,key_val));
	}

	strncpy(temp_time,read_buf,9);
	temp_time[9] = '\0';

#ifdef HR12
	/* Must enter NOON or MIDN for AM/PM time */

	if(strncmp(read_buf,"12:00",5) == 0 || strncmp(read_buf,"1200",4) == 0) {
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_41, "<time field>"));
			return(DONE);
	}
#endif

	if (!*field_entered) 	{ 						/* field not entered */
		if (call_struct.due_time[0] == '\0') {				/* no previous time */
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_41, "<time field>"));
			invalid_time = 1;
		}
	}
	else {									/* field entered  */
		if (strcmp(read_buf, call_struct.due_time) != 0)
		  {
		    user_changed_dt_tm = 1;					/* user changed time field */
		  }

		if (user_changed_dt_tm) {					/* user did change date-time ? */
			if (verify_time(read_buf, call_struct.due_time) == -1) {	/* invalid time entry */
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_41, "<time field>"));
				invalid_time = 1;
				return(DONE);
			}
			else {				/* valid time entered; it is already saved in call_struct.due-time */
				 			/* by verify_date() .. format in which date is saved is hh:mm (24 hr) */
			        /* display properly formatted time */
			        attrset(A_UNDERLINE);
				mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, call_struct.due_time);
				attrset(0);
				if (strncmp(temp_time,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_37, "NOW"), 3) == 0 ||
				    strcmp(temp_time, " ") == 0)
				{
				    /* set the date to current date. We have the now time, we need now date */
					get_pkup_date(call_struct.due_date);
					get_pkup_time(call_struct.due_time);
					user_changed_dt_tm = 0;					/* default time desired */
					attrset(A_UNDERLINE);
					mvaddstr((ptr_to_fld-1)->prompt_row, (ptr_to_fld-1)->prompt_col, call_struct.due_date);
					mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, call_struct.due_time);
					attrset(0);
				}
										/* check if date/time entered is past or not */
				else if (get_internal_time(call_struct.due_date,call_struct.due_time) < (time(0)-60)) {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_42, "<date/time entered has past>"));	/* date/time entered is past */
					invalid_time = 1;
				}

				if (!invalid_time) {
					time_subs_call(); 					/* date/time entered is ok */

/*
 *	restricted time calls
 */
				if (scr_name == BASIC_CALL_SCR || scr_name == EXTENDED_CALL_SCR)
					tcr_check_time_call_restriction(&call_struct, FALSE, key_val);



					if (!doing_update) {

                                          if ( call_struct.priority <= get_call_pri() )
                                            {
                                              if ( call_struct.extended_type[2] == TIMEC )
                                                call_struct.pri_status = 63;
                                            }
                                          else
                                            {
                                              call_struct.priority = 0;				/* don't care about last priority */
                                              if((call_struct.priority = get_call_pri()) == -1) 	/* get call priority and save it */
                                                call_struct.priority = fleet[cur_fl_index]->passenger_priority_def;
                                            }

                                          if (call_struct.extended_type[2] == TIMEC)
                                            call_struct.pri_status = 63;			/* PENDING time call */
                                          else
                                            call_struct.pri_status = call_struct.priority;	/* not time call,reg pri */
					}
                                        else {
                                          if ( ( call_struct.extended_type[2] == TIMEC ) ||
                                               ( ( call_struct.extended_type[2] == SUBS ) && ( call_struct.pri_status == 63 ) ) )
                                            {
                                              // doing an update for a time call
                                              // keep the call's priority if it is of a higher priority
                                              if (call_struct.priority < get_call_pri())
                                                {
                                                  call_struct.pri_status = 63;
                                                }
                                              else
                                                {
                                                  call_struct.pri_status = 63;
                                                  call_struct.priority = get_call_pri();
                                                }
                                            }
                                          else // doing an update on an immediate call
                                            {
                                              call_struct.priority = 0;
                                              if((call_struct.priority = get_call_pri()) == -1) 	/* get call priority and save it */
                                                call_struct.priority = fleet[cur_fl_index]->passenger_priority_def;
                                              call_struct.pri_status = call_struct.priority;
                                            }
                                        }
                                                
					attrset(A_UNDERLINE); 					/* print this time in the field */
					mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, call_struct.due_time);
					print_priority();		/* prints the call priority in the priority field */
					attrset(0);
				}
			}
		}
		else {							/* user did not change time, default it */
			if(!doing_update) {
                          if ( !strlen(call_struct.due_time) )
                            get_pkup_time(call_struct.due_time);
				attrset(A_UNDERLINE);
				mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, call_struct.due_time);
				attrset(0);
			}
		}
	}

	
	if (!invalid_time)
	    ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	else
	    ret_val = DONE;
	if (ret_val == NOT_DONE && invalid_time) {			/* user possibly pressed SEND with invalid time */
		ret_val = DONE;						/* pretend he pushed RETURN key */
	}

	/* skip rate field, if user cannot enter rate */
	if (*piu_ptr == B_RATE && (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR)) {
		if (!get_rate(C_PRINT,NO,&zip1,&zip2))					/* cannot enter rate */
			*piu_ptr = B_HOLDLINE;
	}

	if (*piu_ptr == E_RATE && (scr_name == EXTENDED_CALL_SCR ||  scr_name == SUBS_EXTENDED_SCR)) {
#ifdef FOO
		if (!get_rate(C_PRINT,NO,&zip1,&zip2))					/* cannot enter rate */
			*piu_ptr = E_HOLDLINE;
#endif
	}

	get_est_wait_time(1);
	return(ret_val);

}

/******************************************************************************************/
/* check_comment - verify comment field							  */
/******************************************************************************************/
int check_comment(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
	struct scr_flds *ptr_scr_fld;	/* pointer to array of piu structures */
	int *piu_ptr;  	/* pointer to piu */
	char *read_buf;	/* pointer to buffer for reading field */
	int key_val;	/* value of key pressed by user */
	int *field_entered;	/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;
	int old_piu, ret_val;
	int len, all_spaces, i;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if ( *field_entered )
	  {
	    len = strlen(read_buf);
	    all_spaces = 1;
	    for (i = 0; i < len && i < CHARS_COMMENT; i ++) {
	      if (read_buf[i] != ' ') {
		all_spaces = 0;
		break;
	      }
	    }
	    if (all_spaces)
	      call_struct.general_cmnt[0] = '\0';
	    else
	      /* i is the number of leading spaces (don't copy them) */
	      strcpy(call_struct.general_cmnt, read_buf + i);

	    /* If leading spaces, have to print result of removing them */
	    if (i > 0)
	      {
		attrset(A_UNDERLINE);
		mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, call_struct.general_cmnt);
		len = strlen(call_struct.general_cmnt);
		/* Fill out the rest of the field with spaces to erase the characters we shifted to the right while
		   removing the leading spaces */
		if (len < CHARS_COMMENT)
		  mvprintw(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col + len,
			   "%*c", CHARS_COMMENT - len, ' ');
		attrset(0);
	      }
	  }

	/* update piu */
	old_piu = *piu_ptr;
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
#if 0
	if ( (*piu_ptr == B_TIME && scr_name == SUBS_BASIC_SCR)  || (*piu_ptr == E_TIME && scr_name == SUBS_EXTENDED_SCR) )
		/* on subscription the pkup date & time fields are not data entry fields */
		*piu_ptr = old_piu - 3;	/* go to field before pkup date field */
#endif
	return(ret_val);

}

/**************************************************************************************/
/* check_car_drv_needs - verifies entry into the car-driver need flag field           */
/**************************************************************************************/
int check_car_drv_needs(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
  struct scr_flds *ptr_to_fld;			/* pointer to approp. piu struct */
  char *temp_ptr;					/* pointer to field in call_struct */
  int old_piu;					/* piu value of this field */
  int ret_val;					/* return code from update_piu() */
  int attr_index;					/* index for attr's fields */
  int basic_screen, car_needs, i;		/* temporary variables */
  int start_row, start_col;
  char original;
  int pos = 0, attr_row, attr_col, count_veh_yes = 0, count_drv_yes = 0;
  BOOLEAN found_it;

  
  ptr_to_fld = ptr_scr_fld + (*piu_ptr) ;

  if ( !strncmp(read_buf,"  ", 2) )
    {
      ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
      return(ret_val);
    }
  
  if ( ( scr_name == BASIC_CALL_SCR ) || ( scr_name == SUBS_BASIC_SCR ) )
    {
      attr_row = B_CAR_ROW;
      attr_col = B_CAR_ATTR1_COL;
    }
  else
    {
      attr_row = E_CAR_ROW;
      attr_col = E_CAR_ATTR1_COL;
    }
  
  mvwprintw(stdscr, attr_row + 1, attr_col, "                                  ");

  if (!strncmp(read_buf, "LA", 2))
    bExplicitPremium = TRUE;
  
  if (!strncmp(read_buf, "XX", 2))
    {
      call_struct.veh_attr_flag = NO;
      call_struct.drv_attr_flag = NO;
      for ( i = 0; i < ATTR_MAX; i++ )
        {
          call_struct.veh_attr[i] = NO;
          call_struct.drv_attr[i] = NO;
        }
    }        
  
  pos = 0;
  
  found_it = FALSE;
  
  for ( i = 0; i < ATTR_MAX; i++ )
    {
      if ((!strncmp(fleet[cur_fl_index]->vd_attr_info[i].veh_attr, read_buf, 2)) &&
          (fleet[cur_fl_index]->vd_attr_info[i].veh_attr[0] != EOS))
        {
          found_it = TRUE;
          
          if ( ( call_struct.veh_attr[i] == YES ) &&
               ( strlen(read_buf) ) ) // if already set toggle to NO
            call_struct.veh_attr[i] = NO;
          else
            {
              if ( count_yeses(call_struct.veh_attr) >= (fleet[cur_fl_index]->max_nbr_veh_atr) )
                {
                  Scrn_print_error("Too many vehicle attributes");
                  continue;
                }
              else
                {
                  call_struct.veh_attr[i] = YES;
                  call_struct.veh_attr_flag = YES;
                }
            }
        }
      
      if ((!strncmp(fleet[cur_fl_index]->vd_attr_info[i].drv_attr, read_buf, 2)) &&
               (strlen(read_buf)) &&
               (fleet[cur_fl_index]->vd_attr_info[i].drv_attr[0] != EOS))        
        {
          found_it = TRUE;
          if ( call_struct.drv_attr[i] == YES )
            call_struct.drv_attr[i] = NO;
          else
            {
              if ( count_yeses(call_struct.drv_attr) >= (fleet[cur_fl_index]->max_nbr_drv_atr) )
                {
                  Scrn_print_error("Too many driver attributes");
                  continue;
                }
              else
                {
                  call_struct.drv_attr[i] = YES;
                  call_struct.drv_attr_flag = YES;
                }
              
            }
          
        }
      
      if ( call_struct.veh_attr[i] == YES )
        {
          mvwprintw(stdscr, attr_row + 1, attr_col + pos, "%s ", fleet[cur_fl_index]->vd_attr_info[i].veh_attr);
          pos += 3;
        }
      if ( call_struct.drv_attr[i] == YES )
        {
          mvwprintw(stdscr, attr_row + 1, attr_col + pos, "%s ", fleet[cur_fl_index]->vd_attr_info[i].drv_attr);
          pos += 3;
        }          
    }
  
  if ( ( found_it == FALSE ) &&
       ( strlen(read_buf) ) &&
       ( strncmp(read_buf, "XX", 2) ) )
    Scrn_print_error("ATTRIBUTE NOT AVAILABLE");

  if (read_buf[0] == EOS)
    {
      ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);	/* should directly skip to the next avaible field, and should */
      return(ret_val);
    }
  else
    return(DONE);
  

}

/*********************************************************************************/
/* check_attr - verifies entry of attributes, both car and driver                */
/*********************************************************************************/
check_attr(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;			/* pointer to approp piu struct */
	char *temp_ptr;					/* pointer to field in call_struct */
	int indx;					/* index to be used in->vd_attr_info struct */
	char v_or_d;					/* veh or drv attr */
	int b;					/* flag for scr name */
	int old_piu;					/* piu value of this field */
	int ret_val;					/* return code from update_piu() */

#ifdef FOO
	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR) {		/* determine which field is being read */
		b = 1;
		switch (*piu_ptr) {
			case B_CAR_ATTR1:
				temp_ptr = &call_struct.veh_attr[0];
				v_or_d = C_VEHICLE;
				indx = 0;
				break;
			case B_CAR_ATTR2:
				temp_ptr = &call_struct.veh_attr[1];
				v_or_d = C_VEHICLE;
				indx = 1;
				break;
			case B_CAR_ATTR3:
				temp_ptr = &call_struct.veh_attr[2];
				v_or_d = C_VEHICLE;
				indx = 2;
				break;
			case B_CAR_ATTR4:
				temp_ptr = &call_struct.veh_attr[3];
				v_or_d = C_VEHICLE;
				indx = 3;
				break;
			case B_CAR_ATTR5:
				temp_ptr = &call_struct.veh_attr[4];
				v_or_d = C_VEHICLE;
				indx = 4;
				break;
			case B_CAR_ATTR6:
				temp_ptr = &call_struct.veh_attr[5];
				v_or_d = C_VEHICLE;
				indx = 5;
				break;
			case B_CAR_ATTR7:
				temp_ptr = &call_struct.veh_attr[6];
				v_or_d = C_VEHICLE;
				indx = 6;
				break;
			case B_CAR_ATTR8:
				temp_ptr = &call_struct.veh_attr[7];
				v_or_d = C_VEHICLE;
				indx = 7;
				break;
			case B_DRV_ATTR1:
				temp_ptr = &call_struct.drv_attr[0];
				v_or_d = C_DRIVER;
				indx = 0;
				break;
			case B_DRV_ATTR2:
				temp_ptr = &call_struct.drv_attr[1];
				v_or_d = C_DRIVER;
				indx = 1;
				break;
			case B_DRV_ATTR3:
				temp_ptr = &call_struct.drv_attr[2];
				v_or_d = C_DRIVER;
				indx = 2;
				break;
			case B_DRV_ATTR4:
				temp_ptr = &call_struct.drv_attr[3];
				v_or_d = C_DRIVER;
				indx = 3;
				break;
			case B_DRV_ATTR5:
				temp_ptr = &call_struct.drv_attr[4];
				v_or_d = C_DRIVER;
				indx = 4;
				break;
			case B_DRV_ATTR6:
				temp_ptr = &call_struct.drv_attr[5];
				v_or_d = C_DRIVER;
				indx = 5;
				break;
			case B_DRV_ATTR7:
				temp_ptr = &call_struct.drv_attr[6];
				v_or_d = C_DRIVER;
				indx = 6;
				break;
			case B_DRV_ATTR8:
				temp_ptr = &call_struct.drv_attr[7];
				v_or_d = C_DRIVER;
				indx = 7;
				break;
		}
	}
	else if (scr_name==EXTENDED_CALL_SCR || scr_name== SUBS_EXTENDED_SCR) {	
		b = 0;
		switch ( *piu_ptr ) {
			case E_CAR_ATTR1:
				temp_ptr = &call_struct.veh_attr[0];
				v_or_d = C_VEHICLE;
				indx = 0;
				break;
			case E_CAR_ATTR2:
				temp_ptr = &call_struct.veh_attr[1];
				v_or_d = C_VEHICLE;
				indx = 1;
				break;
			case E_CAR_ATTR3:
				temp_ptr = &call_struct.veh_attr[2];
				v_or_d = C_VEHICLE;
				indx = 2;
				break;
			case E_CAR_ATTR4:
				temp_ptr = &call_struct.veh_attr[3];
				v_or_d = C_VEHICLE;
				indx = 3;
				break;
			case E_CAR_ATTR5:
				temp_ptr = &call_struct.veh_attr[4];
				v_or_d = C_VEHICLE;
				indx = 4;
				break;
			case E_CAR_ATTR6:
				temp_ptr = &call_struct.veh_attr[5];
				v_or_d = C_VEHICLE;
				indx = 5;
				break;
			case E_CAR_ATTR7:
				temp_ptr = &call_struct.veh_attr[6];
				v_or_d = C_VEHICLE;
				indx = 6;
				break;
			case E_CAR_ATTR8:
				temp_ptr = &call_struct.veh_attr[7];
				v_or_d = C_VEHICLE;
				indx = 7;
				break;
			case E_DRV_ATTR1:
				temp_ptr = &call_struct.drv_attr[0];
				v_or_d = C_DRIVER;
				indx = 0;
				break;
			case E_DRV_ATTR2:
				temp_ptr = &call_struct.drv_attr[1];
				v_or_d = C_DRIVER;
				indx = 1;
				break;
			case E_DRV_ATTR3:
				temp_ptr = &call_struct.drv_attr[2];
				v_or_d = C_DRIVER;
				indx = 2;
				break;
			case E_DRV_ATTR4:
				temp_ptr = &call_struct.drv_attr[3];
				v_or_d = C_DRIVER;
				indx = 3;
				break;
			case E_DRV_ATTR5:
				temp_ptr = &call_struct.drv_attr[4];
				v_or_d = C_DRIVER;
				indx = 4;
				break;
			case E_DRV_ATTR6:
				temp_ptr = &call_struct.drv_attr[5];
				v_or_d = C_DRIVER;
				indx = 5;
				break;
			case E_DRV_ATTR7:
				temp_ptr = &call_struct.drv_attr[6];
				v_or_d = C_DRIVER;
				indx = 6;
				break;
			case E_DRV_ATTR8:
				temp_ptr = &call_struct.drv_attr[7];
				v_or_d = C_DRIVER;
				indx = 7;
				break;
		}
	}

	if(v_or_d == C_DRIVER && read_buf[0] == YES && *temp_ptr != YES) 
	{
		if(num_dr_attr_s >= (fleet[cur_fl_index]->max_nbr_drv_atr - '0'))
		{
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_45, "too many driver attributes"));
			return(DONE);
		}
	}
	else if(v_or_d == C_VEHICLE && read_buf[0] == YES && *temp_ptr != YES)
	{
		if(num_vh_attr_s >= (fleet[cur_fl_index]->max_nbr_veh_atr - '0'))
		{		
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_46, "too many vehicle attributes"));
			return(DONE);
		}
	}	

	if (*field_entered) {				/* if field not entered; take previouly entered or default value */
		if (read_buf[0] == YES && *temp_ptr != YES)			/* field entered */
		{
			if(v_or_d == C_VEHICLE)
				 num_vh_attr_s++;
			else
				 num_dr_attr_s++;
			*temp_ptr = YES;		/* copy it, and leave field highlighted */
		}
	 	else if (read_buf[0] == NO && *temp_ptr != NO) 
		{
			if(v_or_d == C_VEHICLE)
				 num_vh_attr_s--;
			else
				 num_dr_attr_s--;
			*temp_ptr = NO;
		}
		else 
			if (*temp_ptr == YES)
				*temp_ptr = YES;
		else 
			*temp_ptr = NO;
	}

	if (*temp_ptr == YES)						/* if field value is set to YES */
		 attrset(A_REVERSE);					/* reprint the attribute desc in reverse video */
	else
		attrset(A_NORMAL);					/* else reprint the desc in normal mode */

	if (v_or_d == C_VEHICLE)
		mvaddstr(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,fleet[cur_fl_index]->vd_attr_info[indx].veh_attr);
	else 
		mvaddstr(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,fleet[cur_fl_index]->vd_attr_info[indx].drv_attr);

	attrset(0);

	old_piu = *piu_ptr;
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);

	return(ret_val);
#endif
#if 0
	if (*piu_ptr == old_piu + 1) {				/* if we are on the attr field, then it should have a desc */
		indx++;
		if ((b==1 && *piu_ptr>B_CAR_NEEDS && *piu_ptr<B_DRV_NEEDS) || (b==0 && *piu_ptr>E_CAR_NEEDS && *piu_ptr<E_DRV_NEEDS)) {
		     if(fleet[cur_fl_index]->vd_attr_info[indx].veh_attr[0]=='\0')	/* check that veh sttr field has a desc */
			 *piu_ptr = (b==1) ? B_DRV_NEEDS : E_DRV_NEEDS;			/* skip attr's field, go to driver needs */
		}
		else 
			if((b==1 && *piu_ptr>B_DRV_NEEDS && *piu_ptr<=B_DRV_ATTR8) || (b==0 && *piu_ptr>E_DRV_NEEDS && *piu_ptr <=E_DRV_ATTR8)) {
		    		if (fleet[cur_fl_index]->vd_attr_info[indx].drv_attr[0]== '\0')
					*piu_ptr = (b==1) ? B_CAR_NO : E_CAR_NO;	/* skip drv attrs fields; go to car # fld*/
			}
	}
	else				 /* on subscription screen pkup date and time are not data entry fields, so skip 'em */
		if (scr_name == SUBS_BASIC_SCR && *piu_ptr == B_DATE)  
		*piu_ptr = B_PH_AREA_CODE;
#endif
}

/**************************************************************************************************/
int check_car_no(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;					/* pointer to array of piu structures */
int *piu_ptr;  							/* pointer to piu */
char *read_buf;							/* pointer to buffer for reading field */
int key_val;							/* value of key pressed by user */
int *field_entered;						/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;	
	char *temp_ptr;	
	int ret_val;						/* return code from update_piu() */
	int indx;
	int old_piu;						/* piu value of this field */

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);
	temp_ptr = &call_struct.nbr_in_group;

	if ( ! *field_entered ) { 				/* field not entered; take previouly entered value */
		if ( call_struct.nbr_in_group == '1' ) {
			attrset(A_UNDERLINE);
			mvprintw(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,"%c", call_struct.nbr_in_group);
			attrset(0);
		}
	}
	else  { 						/* field entered */
	    if (doing_update)
	      {
		prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_MUL_KEY_7, "Unable to update"));
		attrset(A_UNDERLINE);
		mvprintw(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, "%c", call_struct.nbr_of_vehs);
		attrset(0);
		ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
		return(ret_val);
	      }
		
	    if ( ! isdigit(read_buf[0]) || read_buf[0] == '0' ) {
		prt_error(UI_NOFIELD, catgets(UI_m_catd, UI_1, UI_CHECK_FLD_48, "<should be digit>"));
		return(DONE);
	    }
	    else { 					/* entry is valid */
		call_struct.nbr_in_group = read_buf[0]; 
		if (call_struct.nbr_in_group > fleet[cur_fl_index]->max_cabs_per_call) {	/* more then allow*/
		    call_fifo.status.extra_taxis = 1; 			/* set extra taxi bit */
		    prt_error(UI_EXTRA_CABS," ");
		}
		else 
		    /* turn off extra taxis exception bit */
		    call_fifo.status.extra_taxis = 0; 			/* set extra taxi bit */
		
	    }
	}

	old_piu = *piu_ptr; 		/* update piu; but if going to the driver attr field, then the attr desc should exist */
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	
#if 0
	if ( *piu_ptr == old_piu - 1 ) { 				/* we are on the driver attr field # 8 */
	    if ( call_struct.drv_attr_flag == NO )
		*piu_ptr -= ATTR_MAX; 				/* go to drv needs field */
	    else {
		indx = ATTR_MAX - 1;
		while ( indx >= 0 && fleet[cur_fl_index]->vd_attr_info[indx].drv_attr[0]== 0 ) {
		    indx--;
		    *piu_ptr -= 1;
		}
	    }
	}
	else if ( *piu_ptr == B_DATE && scr_name == SUBS_BASIC_SCR )
	    *piu_ptr = B_PH_AREA_CODE;
#endif	
	return(ret_val);

}									/* end check_car_no()  */

/***********************************************************************************************/
int check_rate(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;  						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;			/* pointer to the piu field being processed */
	int old_piu, ret_val, i;
	char rate_string[15];	/* to print the rate entered */
	char *radixptr;
	float call_rate, fSubTotal;


	if ( ! *field_entered ) { 			/* field not entered; do nothing for now */
		if ( call_struct.call_rate == 0.0 ) { 	/* clear the area */
			attrset(A_UNDERLINE);
			clear_field(stdscr,ptr_scr_fld,*piu_ptr);
			attrset(0);
		}
	}
	else { 						/* field entered */
		if ( read_buf[0] == ' ' )  { 		/* wipe out old entry */
			call_struct.call_rate = 0.0;
			call_struct.vat = 0.0;
			attrset(A_UNDERLINE);
			clear_field(stdscr,ptr_scr_fld,*piu_ptr);
			attrset(0);
		}
		else { 					/* new entry save it */
		  	/* Get rid of leading -'s */
		        i = 0;
			while (read_buf[i] == '-')
			    read_buf[i ++] = ' ';

			if (radixptr = strchr(read_buf, *nl_langinfo(RADIXCHAR)))
			  *radixptr = '.';

			call_rate = (float) atof(read_buf);

			/* add the epsilon value to correct inherent rounding problems */
			call_rate += 0.000001;

			/* Although max value is 9999.99, the above correction means we must check above 9999.999 */
			if (call_rate > 9999.999)
			  {
			    prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_8, "Maximum value is 9999.99"));
			    return(DONE);
			  }
			
			call_struct.call_rate = call_rate;
			
			sprintf(rate_string,"%7.2f", call_struct.call_rate);
			rate_string[CHARS_RATE] = '\0';
			attrset(A_UNDERLINE); 							/* print it on the screen */
			if ( scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR ) 
				mvprintw(B_RATE_ROW,B_RATE_COL,"%s",rate_string);
			else if ( scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR )
				mvprintw(E_RATE_ROW,E_RATE_COL,"%s",rate_string);

			if ( scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR ) 
			  mvprintw(B_VAT_ROW,B_VAT_COL,"%5.2f",call_struct.vat);
			else if ( scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR )
			  mvprintw(E_VAT_ROW,E_VAT_COL,"%5.2f",call_struct.vat);
			
			attrset(0);
		}
	}
	
	/* update piu */
	old_piu = *piu_ptr;
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
#if 0
This is done in update_piu now
	if ( *piu_ptr == E_DATE && scr_name == SUBS_EXTENDED_SCR ) 
		/*  on extended screen pkup date field is not a data entry fiedl, so skip it */
		*piu_ptr = E_PH_AREA_CODE;	
	else if ( *piu_ptr == B_DATE  &&  scr_name == SUBS_BASIC_SCR )
		*piu_ptr = B_PH_AREA_CODE;
#endif

	return(ret_val);

}	/* check_rate() */


int check_holdline(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered)
	struct scr_flds *ptr_scr_fld;	/* pointer to array of piu structures */
	int *piu_ptr;  	/* pointer to piu */
	char *read_buf;	/* pointer to buffer for reading field */
	int key_val;	/* value of key pressed by user */
	int *field_entered;	/* whether field entered =1; or not=0 */
{

	struct scr_flds *ptr_to_fld;
	int i;
	int ret_val, old_piu;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (  *field_entered ) {
		/* if 1st char enterd is a space, then user wants to 
		 * wipe out old entry
		 */
		if ( read_buf[0] == ' ') 
			memset(call_struct.hold_line,'\0',ptr_to_fld->max_chars+1);
		else {
			/* new entry */
			/* field entered; verify that all chars are digits, copy
			 * entry in call_struct regardless
			 */
			i = 0;
			while ( i < ptr_to_fld->max_chars && read_buf[i] != '\0' ) {
				if ( ! isdigit(read_buf[i]) ) {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_49, "<digits only>"));
					return(DONE);
				}  /* end if */
				i++;
			} /* end while */
			/* save entry in call_struct */
			strcpy(call_struct.hold_line,read_buf);
		}  /* end else ... new entry */

	}  /* end if */

	/* update piu */
	old_piu = *piu_ptr;
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
#if 0
	if ( *piu_ptr == B_DATE  &&  scr_name == SUBS_BASIC_SCR )
		*piu_ptr = B_PH_AREA_CODE;
	else if ( *piu_ptr == E_DATE  &&  scr_name == SUBS_EXTENDED_SCR )
		*piu_ptr = E_PH_AREA_CODE;
	else if ( *piu_ptr == B_RATE && (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR) ) {
		/* if user not allowed to enter rate, then skip it */
		if ( ! get_rate(C_PRINT,NO,&zip1,&zip2) )
			/* user cannot enter rate */
			*piu_ptr = B_PRIORITY;
	}  /* end else if */
	else if ( *piu_ptr == E_RATE && (scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR) ) {
		/* if user not allowed to enter rate, then skip it */
#ifdef FOO
		if ( ! get_rate(C_PRINT,NO,&zip1,&zip2) )
			/* user cannot enter rate */
			*piu_ptr = E_PRIORITY;
#endif
	}  /* end else if */
#endif
	return(ret_val);

}  	/* check_holdline() */


/***************************************************************************************/
int check_local(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;  						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;			/* pointer to approp. piu struct */
	int old_piu, ret_val;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	attrset(A_UNDERLINE);
	if ( ! *field_entered ) { 			/* field not entered; use previouly entered value */
		mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.local);
	}
	else { 						/* field entered; if entry is not valid, then take default value = NO */
		if ( read_buf[0] == YES ) {
			call_struct.local = YES;
			mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.local);
			call_fifo.status.local = 1; 			/* set the local call bit flag in except_bits */
		}
		else 	if ( read_buf[0] == NO ){
				call_struct.local = NO;
				mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.local);
				call_fifo.status.local = 0; 		/* unset the local bit flag in the except_bits struct */
			}
		else  { 						/* invalid field entry */
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_50, "<default value NO taken>"));
			mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.local);
			call_fifo.status.local = 0; 			/* unset the local bit flag in the except_bits struct */
		}  /* end else */
	}

	attrset(0);
	old_piu = *piu_ptr; 						/* update piu */
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	if ( *piu_ptr == B_DATE  &&  scr_name == SUBS_BASIC_SCR )
		*piu_ptr = B_PH_AREA_CODE;
	else if ( *piu_ptr == E_DATE  &&  scr_name == SUBS_EXTENDED_SCR )
		*piu_ptr = E_PH_AREA_CODE;

	return(ret_val);

}									/* check_local() */


int check_advise_arrival(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;  						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;			/* pointer to approp. piu struct */
	int old_piu, ret_val;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	attrset(A_UNDERLINE);
	if ( ! *field_entered ) { 			/* field not entered; use previouly entered value */
		mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.advise_arrival);
	}
	else { 						/* field entered; if entry is not valid, then take default value = NO */
		if ( read_buf[0] == YES ) {
			call_struct.advise_arrival = YES;
			mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.advise_arrival);
		}
		else 	if ( read_buf[0] == NO ){
				call_struct.advise_arrival = NO;
				mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.advise_arrival);
			}
		else  { 						/* invalid field entry */
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_50, "<default value NO taken>"));
			call_struct.advise_arrival = NO;
			mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.advise_arrival);
		}  /* end else */
	}

	attrset(0);
	old_piu = *piu_ptr; 						/* update piu */
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
#if 0
This is done by update piu
	if ( *piu_ptr == B_DATE  &&  scr_name == SUBS_BASIC_SCR )
		*piu_ptr = B_PH_AREA_CODE;
	else if ( *piu_ptr == E_DATE  &&  scr_name == SUBS_EXTENDED_SCR )
		*piu_ptr = E_PH_AREA_CODE;
#endif
	return(ret_val);

}									/* check_advise_arrival() */

int check_cash_account(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;  						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;			/* pointer to approp. piu struct */
	int old_piu, ret_val;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	attrset(A_UNDERLINE);
	if ( ! *field_entered ) { 			/* field not entered; use previouly entered value */
		mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.advise_arrival);
	}
	else { 						/* field entered; if entry is not valid, then take default value = NO */
		if ( read_buf[0] == YES ) {
			call_struct.cash_account = YES;
			mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.cash_account);
		}
		else 	if ( read_buf[0] == NO ){
				call_struct.cash_account = NO;
				mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.cash_account);
			}
		else  { 						/* invalid field entry */
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_50, "<default value NO taken>"));
			call_struct.cash_account = NO;
			mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.cash_account);
		}  /* end else */
	}

	attrset(0);
	old_piu = *piu_ptr; 						/* update piu */
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);

        return(ret_val);
}

/************************************************************************************************/
/* check_subs_time - check time field for accuracy						*/
/************************************************************************************************/
int check_subs_time(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{

	struct scr_flds *ptr_to_fld;
	int old_piu, ret_val;
	short *time_ptr;
	char formatted_time[10], time_entry[10], temp_time[10];
	int error_flag = 0;
	int invalid_time=0;				/* 1=invalid time entered */
	int all_spaces, len, i;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);
	time_ptr = NULL;
	switch (scr_name) {
		case SUBS_BASIC_SCR:
			switch (*piu_ptr) {
				case B_MON_TIME:
					time_ptr = &subs_struct.mon_time;
					break;
				case B_TUE_TIME:
					time_ptr = &subs_struct.tue_time;
					break;
				case B_WED_TIME:
					time_ptr = &subs_struct.wed_time;
					break;
				case B_THUR_TIME:
					time_ptr = &subs_struct.thu_time;
					break;
				case B_FRI_TIME:
					time_ptr = &subs_struct.fri_time;
					break;
				case B_SAT_TIME:
					time_ptr = &subs_struct.sat_time;
					break;
				case B_SUN_TIME:
					time_ptr = &subs_struct.sun_time;
					break;
				case B_MON_CHG:
					time_ptr = &subs_struct.mon_time_change;
					break;
				case B_TUE_CHG:
					time_ptr = &subs_struct.tue_time_change;
					break;
				case B_WED_CHG:
					time_ptr = &subs_struct.wed_time_change;
					break;
				case B_THUR_CHG:
					time_ptr = &subs_struct.thu_time_change;
					break;
				case B_FRI_CHG:
					time_ptr = &subs_struct.fri_time_change;
					break;
				case B_SAT_CHG:
					time_ptr = &subs_struct.sat_time_change;
					break;
				case B_SUN_CHG:
					time_ptr = &subs_struct.sun_time_change;
					break;
				default:
					prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_51, "<check_subs_time() called>"));
					break;
			}
			break;							/* leave larger switch */

		case SUBS_EXTENDED_SCR:
			switch (*piu_ptr) {
				case E_MON_TIME:
					time_ptr = &subs_struct.mon_time;
					break;
				case E_TUE_TIME:
					time_ptr = &subs_struct.tue_time;
					break;
				case E_WED_TIME:
					time_ptr = &subs_struct.wed_time;
					break;
				case E_THUR_TIME:
					time_ptr = &subs_struct.thu_time;
					break;
				case E_FRI_TIME:
					time_ptr = &subs_struct.fri_time;
					break;
				case E_SAT_TIME:
					time_ptr = &subs_struct.sat_time;
					break;
				case E_SUN_TIME:
					time_ptr = &subs_struct.sun_time;
					break;
				case E_MON_CHG:
					time_ptr = &subs_struct.mon_time_change;
					break;
				case E_TUE_CHG:
					time_ptr = &subs_struct.tue_time_change;
					break;
				case E_WED_CHG:
					time_ptr = &subs_struct.wed_time_change;
					break;
				case E_THUR_CHG:
					time_ptr = &subs_struct.thu_time_change;
					break;
				case E_FRI_CHG:
					time_ptr = &subs_struct.fri_time_change;
					break;
				case E_SAT_CHG:
					time_ptr = &subs_struct.sat_time_change;
					break;
				case E_SUN_CHG:
					time_ptr = &subs_struct.sun_time_change;
					break;
				default:
					prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_51, "<check_subs_time() called>"));
					break;
			}
			break;								/* leave larger switch */

		case SUBS_WAKEUP_SCR:
			switch (*piu_ptr) {
				case W_MON_TIME:
					time_ptr = &subs_struct.mon_time;
					break;
				case W_TUW_TIME:
					time_ptr = &subs_struct.tue_time;
					break;
				case W_WED_TIME:
					time_ptr = &subs_struct.wed_time;
					break;
				case W_THUR_TIME:
					time_ptr = &subs_struct.thu_time;
					break;
				case W_FRI_TIME:
					time_ptr = &subs_struct.fri_time;
					break;
				case W_SAT_TIME:
					time_ptr = &subs_struct.sat_time;
					break;
				case W_SUN_TIME:
					time_ptr = &subs_struct.sun_time;
					break;
				case W_MON_CHG:
					time_ptr = &subs_struct.mon_time_change;
					break;
				case W_TUW_CHG:
					time_ptr = &subs_struct.tue_time_change;
					break;
				case W_WED_CHG:
					time_ptr = &subs_struct.wed_time_change;
					break;
				case W_THUR_CHG:
					time_ptr = &subs_struct.thu_time_change;
					break;
				case W_FRI_CHG:
					time_ptr = &subs_struct.fri_time_change;
					break;
				case W_SAT_CHG:
					time_ptr = &subs_struct.sat_time_change;
					break;
				case W_SUN_CHG:
					time_ptr = &subs_struct.sun_time_change;
					break;
				default:
					prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_51, "<check_subs_time() called>"));
					break;
			}
			break;								/* leave larger switch */

		default:
			prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_51, "<check_subs_time() called>"));
			break;
	}

	    
	len = strlen(read_buf);
	all_spaces = 1;
	for (i = 0; i < len && i < CHARS_TIME; i ++) {
	    if (read_buf[i] != ' ') {
		all_spaces = 0;
		break;
	    }
	}
	
	if (len == 0 || all_spaces) {
	    *time_ptr = 0;
	    ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	    return(ret_val);
	}

	if ( ( atoi(read_buf) != 9999 ) && ( strncmp(read_buf,"99:99", 5) ) )
	  {
	    if (verify_time(read_buf, time_entry) == -1) {
	      prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_41, "<time field>"));		/* invalid time entry */
	      return(DONE);
	    }
	
	    attrset(A_UNDERLINE);
	    mvprintw(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, "%s", time_entry);
	    attrset(0);	    
	  }
	else
	  {
	    strcpy( time_entry, "9999" );
	    attrset(A_UNDERLINE);
	    mvprintw(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, "%s", "99:99");
	    attrset(0);
	  }


	/* Shift over the ':' to erase it */
	for (i = 2; i <= 6; i ++)
	    time_entry[i] = time_entry[i + 1];

	*time_ptr = (short) atoi(time_entry);

#ifdef HR12
	if (time_entry[4] == 'P' || time_entry[4] == 'p')
	    *time_ptr += 1200;
#endif

	if (*time_ptr == 0) {
	    prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_CHECK_FLD_65,
					 "To avoid confusion, do not enter MIDNIGHT. Enter 23:59"));
	    return(DONE);
	}

	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	return(ret_val);
}


/****************************************************************************************/
/* check_subs_begin_date - verify entry of beginning date 				*/
/****************************************************************************************/
int check_subs_begin_date(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;
	int ret_val;
	int *date_ptr;					/* pointer to the field in subs_struct being read */
	char date_entry[10];
	int  i, all_spaces, len;
	time_t t;
	struct tm *tm_ptr;
	char todays_date[7];
	
	ptr_to_fld = ptr_scr_fld + (*piu_ptr);
	date_ptr = &subs_struct.from_date;

	if (scr_name != SUBS_BASIC_SCR && scr_name != SUBS_EXTENDED_SCR && scr_name != SUBS_WAKEUP_SCR) {
		prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_52, "<check_subs_begin_date() called>"));
		return(DONE);
	}

	/* If field is empty, leave it empty. verify_date() would return current date */
	len = strlen(read_buf);
	all_spaces = 1;
	for (i = 0; i < len && i < CHARS_DATE; i ++) {
	    if (read_buf[i] != ' ') {
		all_spaces = 0;
		break;
	    }
	}
	
	if (len == 0 || all_spaces) {
	    subs_struct.from_date = 0;
            from_date[0] = '\0';
	    ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	    return(ret_val);
	}

	if (verify_date(read_buf, date_entry) == -1) {
            from_date[0] = '\0';
	    prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_36, "<date field>"));		/* invalid time entry */
	    return(DONE);
	}

	/* Date entered is with date seperators. subs.from_date is a
	 * int consisisting of the dates in digits. Pretty stupid
	 * design if you ask me since it's in ascending order only
	 * if the day comes first in the date.
	 */

	subs_struct.from_date = (TO_DIGIT(date_entry[0]) * 100000 + TO_DIGIT(date_entry[1]) * 10000 +
				 TO_DIGIT(date_entry[3]) * 1000 + TO_DIGIT(date_entry[4]) * 100 +
				 TO_DIGIT(date_entry[6]) * 10 + TO_DIGIT(date_entry[7]));

	if ( ( DATE_FORMAT == MONTH_FIRST_IN_DATE ) && ( date_entry[6] != '9' ) )
	  subs_struct.from_date += 1000000;
	else if ( ( DATE_FORMAT == YEAR_FIRST_IN_DATE ) && ( date_entry[0] != '9' ) )
	  subs_struct.from_date += 1000000;
	else if ( ( DATE_FORMAT == DAY_FIRST_IN_DATE ) && ( date_entry[6] != '9' ) )
	  subs_struct.from_date += 1000000;	

	t = time(0);
	tm_ptr = localtime(&t);
	sprintf(todays_date, "%02d%02d%02d", tm_ptr->tm_year % 100, tm_ptr->tm_mon + 1, tm_ptr->tm_mday);


	attrset(A_UNDERLINE);								/* print formatted date on the screen */
	mvaddstr(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, date_entry);
	attrset(0);
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	return(ret_val);

}

/****************************************************************************************/
/* check_subs_end_date - verify ending date						*/
/****************************************************************************************/
int check_subs_end_date(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered)
struct scr_flds *ptr_scr_fld;						/* pointer to array of piu structures */
int *piu_ptr;								/* pointer to piu */
char *read_buf;								/* pointer to buffer for reading field */
int key_val;								/* value of key pressed by user */
int *field_entered;							/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;
	int ret_val;
	char date_entry[10];
	int  i, all_spaces, len;
	time_t t;
	struct tm *tm_ptr;
	char todays_date[7];

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (scr_name != SUBS_BASIC_SCR && scr_name != SUBS_EXTENDED_SCR && scr_name != SUBS_WAKEUP_SCR) {
		prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_54, "<check_subs_end_date() called>"));
		return(DONE);
	}

	/* If field is empty, leave it empty. verify_date() would return current date */
	len = strlen(read_buf);
	all_spaces = 1;
	for (i = 0; i < len && i < CHARS_DATE; i ++) {
	    if (read_buf[i] != ' ') {
		all_spaces = 0;
		break;
	    }
	}
	
	if (len == 0 || all_spaces) {
	    subs_struct.to_date = 0;
            to_date[0] = '\0';
	    ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	    return(ret_val);
	}

	if (verify_date(read_buf, date_entry) == -1) {
            to_date[0] = '\0';
	    prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_36, "<date field>"));		/* invalid time entry */
	    return(DONE);
	}

	/* Date entered is with date seperators. subs.to_date is a
	 * int consisisting of the dates in digits. Pretty stupid
	 * design if you ask me since it's in ascending order only
	 * if the day comes first in the date.
	 */
	subs_struct.to_date = (TO_DIGIT(date_entry[0]) * 100000 + TO_DIGIT(date_entry[1]) * 10000 +
			       TO_DIGIT(date_entry[3]) * 1000 + TO_DIGIT(date_entry[4]) * 100 +
			       TO_DIGIT(date_entry[6]) * 10 + TO_DIGIT(date_entry[7]));


	if ( ( DATE_FORMAT == MONTH_FIRST_IN_DATE ) && ( date_entry[6] != '9' ) )
	  subs_struct.to_date += 1000000;
	else if ( ( DATE_FORMAT == YEAR_FIRST_IN_DATE ) && ( date_entry[0] != '9' ) )
	  subs_struct.to_date += 1000000;
	else if ( ( DATE_FORMAT == DAY_FIRST_IN_DATE ) && ( date_entry[6] != '9' ) )
	  subs_struct.to_date += 1000000;



	t = time(0);
	tm_ptr = localtime(&t);
	sprintf(todays_date, "%02d%02d%02d", tm_ptr->tm_year % 100, tm_ptr->tm_mon + 1, tm_ptr->tm_mday);

	attrset(A_UNDERLINE);								/* print formatted date on the screen */
	mvaddstr(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, date_entry);
	attrset(0);
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	return(ret_val);

}

/**************************************************************************************/
/* check_personel1 - verifys entry of personal field (vehicle number)		      */
/**************************************************************************************/
int check_personel1(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered)
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;
	int veh_num, ret_val, ret_cisam, old_piu;
	int i, error_flag;
	struct cisam_vh *veh_file_ptr;
	
	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (*field_entered) {					/* if 1st char entered is a space, then wipe out old entry */
			
		if (read_buf[0] == ' ') {
			call_struct.personal_veh = (short)0;
			call_fifo.personal_veh = (short)0;
			call_struct.personal_rqst = '\0';			/* wipe out entry for personel2 field also */
			call_fifo.personal_rqst = '\0';
			attrset(A_UNDERLINE);
			mvprintw(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,"    ");
			mvaddch((ptr_to_fld+1)->prompt_row,(ptr_to_fld+1)->prompt_col,' ');
			attrset(0);
			old_piu = *piu_ptr;
			ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
			if (*piu_ptr == old_piu+1)
				*piu_ptr += 1;					/* skip the 2nd personal field */
			return(ret_val);
		}

					/* field entered; verify it, if invalid veh # specified then do not save it, else save it */
		error_flag = 0;
		if (fleet[cur_fl_index]->criteria.suppress_personal){
			attrset(A_UNDERLINE);
			mvprintw(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,"    ");
			mvaddch((ptr_to_fld+1)->prompt_row,(ptr_to_fld+1)->prompt_col,' ');
			attrset(0);
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_56, "<Entry of personal calls is not allowed>"));
			error_flag = 1;
			ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
			*piu_ptr += 1;							/* skip 2nd personal field */
			return(ret_val);
		}

		i = 0;
		while (i < ptr_to_fld->max_chars && read_buf[i] != '\0') {
			if (!isdigit(read_buf[i])) {
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_57, "<chars only>"));
				error_flag = 1;
				break;
			}
			i++;
		}
		if (!error_flag) {						/* error has not been caught so far */
			veh_num = atoi(read_buf);
			veh_file_ptr = (struct cisam_vh *) req_buf;
			veh_file_ptr->fleet = call_struct.fleet;
			veh_file_ptr->nbr = (short)veh_num;
			if ((ret_cisam=db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISEQUAL,0)) < 0 ) {
				prt_error(UI_NOFIELD,"%d <%s>", iserrno, catgets(UI_m_catd, UI_1, UI_CHECK_FLD_58, "non-existent veh # "));	/* invalid veh # specified */
				error_flag = 1;
			}
			else {								/* got right veh # */
				call_struct.personal_veh = (short)veh_num;
				call_fifo.personal_veh = (short)veh_num;
			}
		}
	}

	else { /* field not entered */
	    if (read_buf[0] == ' ') {
		call_struct.personal_veh = (short)0;
		call_fifo.personal_veh = (short)0;
		call_struct.personal_rqst = '\0';			/* wipe out entry for personel2 field also */
		call_fifo.personal_rqst = '\0';
		attrset(A_UNDERLINE);
		mvprintw(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,"    ");
		attrset(0);
		old_piu = *piu_ptr;
		ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
		if (*piu_ptr == old_piu+1)
		    *piu_ptr += 1;					/* skip the 2nd personal field */
		return(ret_val);
	    }
	}

	old_piu = *piu_ptr;								/* update piu */
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);

	if (error_flag)
		*piu_ptr = old_piu;							/* wrong entry, trap user in field */

	if (*piu_ptr == old_piu + 1)
		if (call_struct.personal_veh == 0)					/* not entered or wrong entry */
			*piu_ptr += 1;							/* skip 2nd personal field */

	if (call_struct.personal_veh != 0)
	  {
	    call_struct.personal_rqst = C_PERSONAL_ONLY;
	    call_fifo.personal_rqst = C_PERSONAL_ONLY;
	  }
	
	return(ret_val);
}

/****************************************************************************************/
/* check_personel2 - verify user entry of the I or O flag for personal specification    */
/****************************************************************************************/
int check_personel2(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered)
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;
	int old_piu, ret_val;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (!*field_entered) {							/* take previously entered value */
		if (call_struct.personal_veh != 0) {				/* need entry for this field */
			if (call_struct.personal_rqst == '\0') {
				call_struct.personal_rqst = C_PERSONAL_ONLY;
				call_fifo.personal_rqst = C_PERSONAL_ONLY;
			}  
			attrset(A_UNDERLINE);					/* print field on screen */
			mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, C_PERSONAL_ONLY);
			attrset(0);
		}
	}
	else {								/* field entered; if wrong entry, then take C_PERSONAL_IF */
	      if (call_struct.personal_veh == 0)
		{
		  prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_32, "Need to enter the vehicle number first"));
		  attrset(A_UNDERLINE);
		  mvaddch(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, ' ');
		  attrset(0);
		  
		  /* Go back one to the vehicle number field */
		  (*piu_ptr)--;
		  return(DONE);
		}

		if (read_buf[0] == ' ') {
			call_struct.personal_rqst = C_PERSONAL_IF;
			call_fifo.personal_rqst = C_PERSONAL_IF;
			attrset(A_UNDERLINE);
			mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,C_PERSONAL_IF);
			attrset(0);
		}
		else if (read_buf[0] != C_PERSONAL_ONLY && read_buf[0] != C_PERSONAL_IF) {
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_59, "<wrong entry;default 'I' taken>"));
			call_struct.personal_rqst = C_PERSONAL_IF;
			attrset(A_UNDERLINE);
			mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,C_PERSONAL_IF);
			attrset(0);
		}
		else  {
			call_struct.personal_rqst = read_buf[0];
			call_fifo.personal_rqst = read_buf[0];
		}
	}

	old_piu = *piu_ptr;								/* update piu */
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	return(ret_val);

}

clear_charge_fields_only(ptr_scr_flds)
struct scr_flds *ptr_scr_flds;				/* pointer to array of piu structures */
{
  int i;

  call_struct.company_name[0] = '\0';
  call_struct.ac_addt1_info1[0] = '\0';
  call_struct.ac_addt1_info2[0] = '\0';
  call_struct.driver_msg[0] = '\0';

  if ( call_struct.veh_attr_flag != YES )
    {
      call_struct.veh_attr_flag = NO;
      memset(call_struct.veh_attr, '\0', sizeof(call_struct.veh_attr));
    }
  if ( call_struct.drv_attr_flag != YES )
    {
      call_struct.drv_attr_flag = NO;
      memset(call_struct.drv_attr, '\0', sizeof(call_struct.drv_attr));
    }
  
  attrset(A_UNDERLINE);			
  for (i = E_CO_NAME1; i < E_CO_NAME3; i++) {
    clear_field(stdscr, ptr_scr_flds, i);
  }

  //  mvaddch(E_CAR_ROW,E_CAR_COL,NO);
  //  mvaddch(E_DRIVER_ROW, E_DRIVER_COL, NO);
  
  attrset(0);
  mvaddstr(E_CHARGE_ROW + 1, 3, "                                               "); /* 47 spaces */
  mvaddstr(E_CHARGE_ROW + 2, 3, "                                               "); /* 47 spaces */
  mvaddstr(E_CHARGE_ROW + 3, 22, "                                    "); /* 36 spaces */

}

/********************************************************************************/
/* check_charge - verify entry of the charge account number			*/
/********************************************************************************/
int check_charge(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered)
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
  int old_piu, ret_val;
  struct cisam_ac AccountRec;
  int charge_nbr_changed = 0;			/* has charge # entered changed ? */
  static char blank_s[] = "                                                    ";
  static char blk_s1[] = "                         ";
  STREET_ADDRESS street_address;
  PHONE phone;
  ACCOUNT_ID account_id;  
  ACAD_REC  *acad;
  INDV_REC  *indv;
  int       answer_key = 0;
  char      city_sav[8];
  
  bzero( &AccountRec, sizeof( struct cisam_ac ) );
  bzero( city_sav, sizeof( city_sav ) );

  if ( strlen(Telephone) )
    {
      bzero( call_struct.charge_nbr, sizeof(call_struct.charge_nbr) );
      Telephone[0] = '\0';
      bPhoneFieldVisited = TRUE;
    }
  attrset(A_UNDERLINE);
  mvwprintw( stdscr, E_CHARGE_ROW, E_CHARGE_COL, "%s", read_buf );
  attrset(0);
  refresh();
  if (!*field_entered  &&  call_struct.charge_nbr[0] == '\0') {	/* there is no old entry, do not trap user in field */
    old_piu = *piu_ptr;
    ret_val = update_piu(ptr_scr_fld, piu_ptr, key_val );
    call_struct.extended_type[3] = ' ';
    if(!doing_update && key_val != TOGGLE_KEY)
      prt_error(UI_REQENTRY,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_60, "<charge a/c>"));
    return(ret_val);
  }
  else {								/* either new entry or old entry exists; verify it */
    if (*field_entered) {
      if (read_buf[0] == ' ') {
	call_struct.charge_nbr[0] = '\0';
	attrset(A_UNDERLINE);
	clear_field(stdscr,ptr_scr_fld,*piu_ptr);
	attrset(0);
	clear_charge_fields_only(ptr_scr_fld);
	call_struct.extended_type[3] = ' ';			/* No charge */
	if (!doing_update && key_val != TOGGLE_KEY )
	  {
	    prt_error(UI_REQENTRY,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_60, "<charge a/c>"));
	    return( *piu_ptr );
	  }
	return(update_piu(ptr_scr_fld,piu_ptr,key_val));
      }
      else								/* new entry; save it */
	charge_nbr_changed = get_charge_nbr(read_buf);		/* strip off extra chars */
    }
    
    if (IS_CHARGE_CARD_NUM(read_buf))
      return(update_piu(ptr_scr_fld,piu_ptr,key_val));
    
    if ((valid_charge_nbr(&AccountRec,&call_struct,0)) && (charge_nbr_changed))
      {
	if ( 0 )
	  {
	    if (AccountRec.account_charge_enabled != YES)
	      {
		charge_nbr_changed = 1;
		clear_charge_fields_only(ptr_scr_fld);
		prt_error(UI_NO_MSG,
			  "Ej tillåtet att göra kontobeställning för angivet kontonummer!");
		return(DONE);
	      }
	  }
	

    
      acad = ( ACAD_REC * ) tmp_req_buf;
      ret_val = GetAccountAddress( &AccountRec, acad );

      if ( ret_val == 1 )
	{
#ifdef NAMERICA
	  if ( strlen(call_struct.pckup_str_name) )
	    {
	      while ( answer_key != YES && answer_key != NO )
		{
		  move(23, 0);
		  clrtoeol();
		  refresh();
		  attrset(A_REVERSE);
		  mvprintw(23, 0, " %s ", "Do you want to retrieve account address information? (Y/N)");
		  refresh();
		  attrset(0);
		  answer_key = Toupper(wgetch(curscr));
		  if ( answer_key == NO )
		    {
		      move(23,0);
		      clrtoeol();
		      refresh();
		    }
		  else
		    {
		      AccountAddress_to_Account( &AccountRec, acad );
		      move(23,0);
		      clrtoeol();
		      refresh();
		    }
		}
	    }
#else
	  AccountAddress_to_Account( &AccountRec, acad );
#endif
	}

      indv = ( INDV_REC * ) tmp_req_buf;
      ret_val = GetIndv( &AccountRec, indv );
      if ( ret_val == 1 )
	strcpy( call_struct.passenger_name, indv->individual_id.name );

      if (AccountRec.show_pckup_adr == YES)	 /*do we show address for this account auto */
	{
#ifdef NAMERICA
	  if ( strlen(call_struct.pckup_str_name) )
	    {
	      while ( answer_key != YES && answer_key != NO )
		{
		  move(23, 0);
		  clrtoeol();
		  refresh();
		  beep();
		  attrset(A_REVERSE);
		  mvprintw(23, 0, " %s ", "Do you want to retrieve account address information? (Y/N)");
		  refresh();
		  attrset(0);
		  answer_key = Toupper(wgetch(curscr));
		  if ( answer_key == NO )
		    {
		      move(23,0);
		      clrtoeol();
		      refresh();
		    }
		  else
		    {
		      clear_charge_fields_only(ptr_scr_fld);
		      charge_related_fields(C_CLEAR,&AccountRec,&call_struct);
		      charge_related_fields(C_PRINT,&AccountRec,&call_struct);
		      move(23,0);
		      clrtoeol();
		      refresh();
		    }
		}
	    }
	  else
	    {
	      clear_charge_fields_only(ptr_scr_fld);
	      charge_related_fields(C_CLEAR,&AccountRec,&call_struct);
	      charge_related_fields(C_PRINT,&AccountRec,&call_struct);
	    }
#else
	  clear_charge_fields_only(ptr_scr_fld);
          
          if ( !strlen(call_struct.pckup_str_name) )
            charge_related_fields(C_CLEAR,&AccountRec,&call_struct);
          
	  charge_related_fields(C_PRINT,&AccountRec,&call_struct); 	/* charge # entered is valid; display fields */
#endif
	}
      else
	{
	  clear_charge_fields_only(ptr_scr_fld);
	  if ( strlen(call_struct.pckup_city) )
	    strcpy(city_sav, call_struct.pckup_city);
	  if ( !strlen(call_struct.pckup_str_name) )
	    charge_related_fields(C_CLEAR,&AccountRec,&call_struct);
	  if ( strlen( city_sav ) )
	    strcpy( call_struct.pckup_city, city_sav );
	  charge_related_fields(C_PRINT, &AccountRec, &call_struct );
	}
      
      if (call_struct.extended_type[1] == '\0')  
	call_struct.extended_type[1] = ' ';		/* set the call type */
      if (call_struct.extended_type[2] == '\0')  
	call_struct.extended_type[2] = ' ';		/* set the call type */
      call_struct.extended_type[3] = CHARGE;			/* '$' */
      if(call_struct.extended_type[4] != MULT_CALL)
	call_struct.extended_type[4] = '\0';
      call_struct.extended_type[5] = '\0';
#ifdef SCANDINAVIA
      if ( !strlen(call_struct.pckup_str_name) )
        charge_related_fields(C_CLEAR,&AccountRec,&call_struct);
      
      //      charge_related_fields(C_PRINT,&AccountRec,&call_struct); 	/* charge # entered is valid; display fields */
#endif
      return(update_piu(ptr_scr_fld,piu_ptr,key_val));	/* update piu */
    }
    else {								/* charge a/c # entered is not valid */
      if ((valid_charge_nbr(&AccountRec, &call_struct)))
	 return(update_piu(ptr_scr_fld,piu_ptr,key_val));
      else
	{
	  charge_nbr_changed = 1;
	  clear_charge_fields_only(ptr_scr_fld);
	}
      return(DONE);		/* don't go to next field without valid acct # */
    }
  }
}

/****************************************************************************************/
/* check_coname - verify entry of company name field					*/
/****************************************************************************************/
int check_coname1(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered)
	struct scr_flds *ptr_scr_fld;	/* pointer to array of piu structures */
	int *piu_ptr;  	/* pointer to piu */
	char *read_buf;	/* pointer to buffer for reading field */
	int key_val;	/* value of key pressed by user */
	int *field_entered;	/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (  *field_entered )  {
		/* field entered */
		if ( read_buf[0] == ' ' )  {
			attrset(A_UNDERLINE);
			clear_field(stdscr,ptr_scr_fld,*piu_ptr);
			attrset(0);
		}  /* end if .. wipe out old entry */
		else 
			/* new entry */
	               	strcpy(call_struct.company_name,read_buf);
	}  /* end if .. field entered */
			
	return(update_piu(ptr_scr_fld,piu_ptr,key_val));

}	/* end check_coname1() */


int check_coname2(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered)
	struct scr_flds *ptr_scr_fld;	/* pointer to array of piu structures */
	int *piu_ptr;  	/* pointer to piu */
	char *read_buf;	/* pointer to buffer for reading field */
	int key_val;	/* value of key pressed by user */
	int *field_entered;	/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (  *field_entered )  {
		/* field entered */
		if ( read_buf[0] == ' ' )  {
			attrset(A_UNDERLINE);
			clear_field(stdscr,ptr_scr_fld,*piu_ptr);
			attrset(0);
		}  /* end if .. wipe out old entry */
		else 
			/* new entry */
			strcpy(call_struct.ac_addt1_info1,read_buf);
	}  /* end if .. field entered */
			
	return(update_piu(ptr_scr_fld,piu_ptr,key_val));

}	/* end check_coname2() */


int check_coname3(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered)
	struct scr_flds *ptr_scr_fld;	/* pointer to array of piu structures */
	int *piu_ptr;  	/* pointer to piu */
	char *read_buf;	/* pointer to buffer for reading field */
	int key_val;	/* value of key pressed by user */
	int *field_entered;	/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (  *field_entered )  {
		/* field entered */
		if ( read_buf[0] == ' ' )  {
			attrset(A_UNDERLINE);
			clear_field(stdscr,ptr_scr_fld,*piu_ptr);
			attrset(0);
		}  /* end if .. wipe out old entry */
		else 
			/* new entry */
			strcpy(call_struct.ac_addt1_info2,read_buf);
	}  /* end if .. field entered */
			
	return(update_piu(ptr_scr_fld,piu_ptr,key_val));

}	/* end check_coname3() */


int check_pod1(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered)
	struct scr_flds *ptr_scr_fld;	/* pointer to array of piu structures */
	int *piu_ptr;  	/* pointer to piu */
	char *read_buf;	/* pointer to buffer for reading field */
	int key_val;	/* value of key pressed by user */
	int *field_entered;	/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;
	int ret_val, old_piu;


	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if ( ! *field_entered )  {
		/* field not entered */
		if ( call_struct.pod_required == '\0' )
			call_struct.pod_required = NO;
	}  /* end field not entered */
	else {
		/* field entered; if YES, else NO */
		if ( read_buf[0] == ' ' || read_buf[0] == NO )  
			call_struct.pod_required = NO;
		else if ( read_buf[0] == YES )
			call_struct.pod_required = YES;
		else  {
			/* invalid entry, use NO */
			call_struct.pod_required = NO;
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_50, "<default value NO taken>"));
		}  /* end else */
	}  /* end else .. field entered */

	/* print data in the field */
	attrset(A_UNDERLINE);
	mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,call_struct.pod_required);
	attrset(0);

	/* update piu */
	old_piu = *piu_ptr;
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	if ( call_struct.pod_required == NO )
		if ( *piu_ptr == old_piu + 1 )
			/* skip the pod name & time fields */
			*piu_ptr += 2;

	return(ret_val);

}	/* check_pod1()  */


int check_podtime(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered)
	struct scr_flds *ptr_scr_fld;	/* pointer to array of piu structures */
	int *piu_ptr;  	/* pointer to piu */
	char *read_buf;	/* pointer to buffer for reading field */
	int key_val;	/* value of key pressed by user */
	int *field_entered;	/* whether field entered =1; or not=0 */
{

	struct scr_flds *ptr_to_fld;	/* pointer to piu struct being processed */
	int old_piu, ret_val;
	char temp_time[9];	/* for saving formatted time */
	char formatted_time[9];	/* for saving formatted time */


	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	/* cannot be called from subscription screens */
	if ( scr_name != EXTENDED_CALL_SCR && scr_name != SUBS_EXTENDED_SCR ) {
		/* should not be called */
		prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_61, "<check_podtime() called>"));
		return(update_piu(ptr_scr_fld,piu_ptr,key_val));
	}  /* end if ... called from subscription screen */

	if ( ! *field_entered )  {
		/* field not entered */
	}  /* end if .. field not entered */

	else {
		if ( read_buf[0] == ' ') {
			/* wipe out old entry */
			call_struct.pod_time = (short)0;
			attrset(A_UNDERLINE);
			clear_field(stdscr,ptr_scr_fld,*piu_ptr);
			attrset(0);
		}  /* end if */
		/* new entry; verify it */
		if ( verify_time(read_buf,temp_time) == -1 ) {
			/* invalid time entry */
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_41, "<time field>"));
			return(DONE);
		}
		else {
			/* entry is valid, so save it in hhmm formatted "short" variable */
			sprintf(formatted_time,"%02d%02d", atoi(&temp_time[0]), atoi(&temp_time[3]));
			call_struct.pod_time = (short)atoi(formatted_time);
		}  /* end else */

		/* print the time in display format on the screen */
		attrset(A_UNDERLINE);
		mvprintw(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,"%-6s",temp_time);
		attrset(0);
	}  /* end else ... field_entered */

	return(update_piu(ptr_scr_fld,piu_ptr,key_val));

}	/* check_podtime()  */


/*******************************************************************************************/
int check_priority(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;					/* pointer to array of piu structures */
int *piu_ptr;  							/* pointer to piu */
char *read_buf;							/* pointer to buffer for reading field */
int key_val;							/* value of key pressed by user */
int *field_entered;						/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;
	int old_piu;
	int ret_val;
	short call_priority;					/* entered by user */
	int error_flag=0; 					/* set if invalid priority entered by user */
	int i;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if ( ! *field_entered ) { 		/* field not entered; unless a valid entry is there trap user in field */
		if ( call_struct.priority == 0 ) { 				/* user previously enterd an invalid field */
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_62, "<need a valid call priority>"));
			error_flag = 1;
		}  
	}
	else {
		if ( read_buf[0] == ' ')  { 					/* take default value */
			if((call_struct.priority = get_call_pri()) == -1){ 	/* get the call priority and save it */
				call_struct.priority = fleet[cur_fl_index]->passenger_priority_def;	/* default to pass*/
			}
			if(!doing_update)
				call_struct.pri_status = call_struct.priority;
			attrset(A_UNDERLINE);
			mvprintw(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,"%d", call_struct.priority);
			attrset(0);
		}
		else { 								/* new value entered */
			i = 0;
			while ( i < ptr_to_fld->max_chars && read_buf[i] != '\0' )  {
				if ( ! isdigit(read_buf[i]) ) {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_49, "<digits only>"));
					error_flag = 1;
					break;					/* leave while loop */
				}
				i++;
			}
			if ( ! error_flag )  {
				 call_priority = (short)atoi(read_buf);
				 if ( call_priority < uiuser[user_index]->highest_call_pri || call_priority > 63 )  {
					prt_error(UI_NOFIELD,"<%s %d %s>",
						  catgets(UI_m_catd, UI_1, UI_CHECK_FLD_63, "priority must be between"), 
						  (int) uiuser[user_index]->highest_call_pri,
						  catgets(UI_m_catd, UI_1, UI_CHECK_FLD_64, "and 63"));
					error_flag = 1;
				 }
			}
			if ( ! error_flag )  { 				/* value enterd is valid */
				call_struct.priority = call_priority;
				if(!doing_update)
					call_struct.pri_status = call_struct.priority;
			}
		}  							/* end else ... new entry */
	}  								/* end else .. field enteerd */

	old_piu = *piu_ptr; 						/* update piu */
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	if ( error_flag ) {
		call_struct.priority = 0;
		if(!doing_update)
			call_struct.pri_status = 0;			/* save for next time */
		*piu_ptr = old_piu;					/* trap user in field */
	}
	else if ( *piu_ptr == B_DATE  &&   scr_name == SUBS_BASIC_SCR )
		*piu_ptr = B_PH_AREA_CODE;
	else if ( *piu_ptr == old_piu -1  && ( scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR) ) {
		if ( call_struct.pod_required == NO ) 			/* check if should go to the pod time field or not */
			*piu_ptr = E_POD; 				/* skip the pod fields */
	}
	else if ( *piu_ptr == B_RATE && (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR) )  {
		if ( ! get_rate(C_PRINT,NO,&zip1,&zip2) )
			*piu_ptr = B_HOLDLINE; 				/* cannot enter rate, skip rate field */
	}
	else if ( *piu_ptr == E_RATE && (scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR) )  {
#ifdef FOO
		if ( ! get_rate(C_PRINT,NO,&zip1,&zip2) )
			*piu_ptr = E_HOLDLINE; 				/* cannot enter rate, skip rate field */
#endif
	}

	return(ret_val);

}									/* check_priority() */

int check_via( ptr_scr_fld, piu_ptr, read_buf, key_val, field_entered )
     struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
  struct scr_flds *ptr_to_fld;			/* pointer to field */
  int *str_nbr_ptr;					/* pointer to call_struct field */
  char *suf_ptr;					/* pointer to suffix */
  int  i, len;
  short new_via = 0;
  int  error_flag, old_piu, ret_val;
  
  ptr_to_fld = ptr_scr_fld + (*piu_ptr);
  old_piu = *piu_ptr;
  error_flag = 0;
  
  switch ( *piu_ptr )
    {
    case B_VIA_ADDRESS:
    case E_VIA_ADDRESS:
      if ( ! isdigit(read_buf[0]) ) {
	prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_49, "<digits only>"));
	error_flag = 1;
	break;
      }
      new_via = atoi( read_buf );
      call_struct.via_addresses = new_via;
      break;
    default:
      break;
    }
  
  ret_val = update_piu(ptr_scr_fld, piu_ptr, key_val);

  return(ret_val);

}
      
int check_invoice( ptr_scr_fld, piu_ptr, read_buf, key_val, field_entered )
     struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
  struct scr_flds *ptr_to_fld;			/* pointer to field */
  int *str_nbr_ptr;					/* pointer to call_struct field */
  char *suf_ptr;					/* pointer to suffix */
  int  i, len;
  
  ptr_to_fld = ptr_scr_fld + (*piu_ptr);
  
  switch ( *piu_ptr )
    {
    case E_INV1:
      strcpy( call_struct.invoice_nbr1, read_buf );
      break;
    case E_INV2:
      strcpy( call_struct.invoice_nbr2, read_buf );
      break;
    case E_INV3:
      strcpy( call_struct.invoice_nbr3, read_buf );
      break;
    case E_INV4:
      strcpy( call_struct.invoice_nbr4, read_buf );
      break;
    default:
      break;
    }
  return( update_piu(ptr_scr_fld, piu_ptr, key_val) );
}

int
DoCustomerProfile(ptr_scr_fld, piu_ptr)
     struct scr_flds *ptr_scr_fld;
     int    *piu_ptr;
{
  int    i;
  struct scr_flds   *ptr_to_fld;
  char              sStrNbr[8];
  char              sStrNameSave[32];
  char              read_buf[8];
  int               field_entered = 1;
  int AttrCol = 0, AttrRow = 0;
  short MaxAttrs;
  
  bzero( &gCustRec, sizeof( struct cisam_cust ) );

  strcpy(gCustRec.phone_nbr, call_struct.phone_number );
  if ( GetCustomerAddress( &gCustRec ) == 0 )
    //  if (db_read_key(CUSTOMER_FILE_ID, &gCustRec, &cust_key1, ISEQUAL) != SUCCESS)
    {
      return(-1);
    }
  else
    {

      if (isalpha(gCustRec.fleet))
	{
	  sprintf(read_buf, "%c", gCustRec.fleet);
	  *piu_ptr = B_FLEET;
	  attrset( A_UNDERLINE );
	  ptr_to_fld = ptr_scr_fld + (*piu_ptr);
	  mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, read_buf);
	  attrset( 0 );
	  check_fleet( ptr_scr_fld, piu_ptr, read_buf, '\n', &field_entered );
	}
      if (strlen(gCustRec.pckup_str_name))
	{
	  strcpy(call_struct.pckup_str_name, gCustRec.pckup_str_name);
	  *piu_ptr = B_PKUP_ST_NAME;
	  attrset(A_UNDERLINE);
	  ptr_to_fld = ptr_scr_fld + (*piu_ptr);
	  mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, gCustRec.pckup_str_name);
	  attrset(0);
	}

      if (strlen(gCustRec.pckup_apt_room))
	{
	  strcpy( call_struct.pckup_apt_room, gCustRec.pckup_apt_room );
	  *piu_ptr = B_PKUP_APT_NO;
	  attrset(A_UNDERLINE);
	  ptr_to_fld = ptr_scr_fld + (*piu_ptr);
	  mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, gCustRec.pckup_apt_room);
	  attrset(0);
	}

      if (strlen(gCustRec.customer_comment))
        {
          strcpy(call_struct.general_cmnt, gCustRec.customer_comment);
          *piu_ptr = B_COMMENT;
          ptr_to_fld = ptr_scr_fld + (*piu_ptr);
          attrset(A_UNDERLINE);
          mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, gCustRec.customer_comment);
          attrset(0);
        }
      
      if (gCustRec.pckup_str_nbr > 0)
	{
	  call_struct.pckup_str_nbr = gCustRec.pckup_str_nbr;
	  *piu_ptr = B_PKUP_ST_NO;
	  ptr_to_fld = ptr_scr_fld + (*piu_ptr);
	  if (isalpha(gCustRec.pckup_str_suf))
	    {
	      call_struct.pckup_str_suf = gCustRec.pckup_str_suf;
	      sprintf(sStrNbr, "%d%c", gCustRec.pckup_str_nbr, gCustRec.pckup_str_suf);
	    }
	  else
	    sprintf(sStrNbr, "%d", gCustRec.pckup_str_nbr);
	  attrset(A_UNDERLINE);
	  mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, sStrNbr);
	  attrset(0);
	}
      if (strlen(gCustRec.passenger_name))
	{
	  strcpy(call_struct.passenger_name, gCustRec.passenger_name);
	  *piu_ptr = B_PASSENGER;
	  ptr_to_fld = ptr_scr_fld + (*piu_ptr);
	  attrset(A_UNDERLINE);
	  mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, gCustRec.passenger_name);
	  attrset(0);
	}
      if (strlen(gCustRec.pckup_city))
	{
	  strcpy(call_struct.pckup_city, gCustRec.pckup_city);
	  *piu_ptr = B_PKUP_CITY;
	  ptr_to_fld = ptr_scr_fld + (*piu_ptr);
	  attrset(A_UNDERLINE);
	  mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, gCustRec.pckup_city);
	  attrset(0);
	}
      *piu_ptr = B_PKUP_CITY;

      strcpy( sStrNameSave, gCustRec.pckup_str_name );
      zone_call(ptr_scr_fld, *piu_ptr);
      gCustRec.pckup_zone = call_struct.pckup_zone;
      strncpy(call_struct.pckup_str_key, sStrNameSave, KEY_LENGTH);
      call_struct.pckup_str_key[KEY_LENGTH] = '\0';

      for ( i = 0; i < ATTR_MAX; i++ )
        {
          if ( gCustRec.veh_attr[i] == YES )
            {
              call_struct.veh_attr[i] = YES;
              call_struct.veh_attr_flag = YES;
            }
          else
            call_struct.veh_attr[i] = NO;

          if ( gCustRec.drv_attr[i] == YES )
            {
              call_struct.drv_attr[i] = YES;
              call_struct.drv_attr_flag = YES;
            }
          else
            call_struct.drv_attr[i] = NO;
        }
      
      attrset(A_NORMAL);
      AttrCol = B_CAR_ATTR1_COL;
      AttrRow = B_CAR_ROW;
      if ( 1 )
        MaxAttrs = ATTR_MAX;
      else
        MaxAttrs = 8;
      for ( i = 0; i < MaxAttrs; i++ )
        {
          if ( call_struct.veh_attr[i] == YES )
            {
              if ( fleet[ find_findex(call_struct.fleet) ]->vd_attr_info[i].veh_attr[0] != '\0' )
                {
                  mvaddstr( AttrRow + 1, AttrCol, fleet[ find_findex(call_struct.fleet) ]->vd_attr_info[i].veh_attr );
                  AttrCol += 3;
                  if ( AttrCol >= B_CAR_ATTR8_COL + 2 )
                    break;
                }
            }
          if ( call_struct.drv_attr[i] == YES )
            {
              if ( fleet[ find_findex(call_struct.fleet) ]->vd_attr_info[i].drv_attr[0] != '\0' )
                {
                  mvaddstr( AttrRow + 1, AttrCol, fleet[ find_findex(call_struct.fleet) ]->vd_attr_info[i].drv_attr );
                  AttrCol += 3;
                  if ( AttrCol >= B_DRIVER_ATTR8_COL + 2 )
                    break;
                }
            }
        }

      attrset(A_NORMAL);


    }
  gCustRec.made_calls = 1;
  return(1);
}	    

#ifdef NAMERICA
int check_send_dest_addr(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;  						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;			/* pointer to approp. piu struct */
	int old_piu, ret_val;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	attrset(A_UNDERLINE);
	if ( ! *field_entered ) { 			/* field not entered; use previouly entered value */
		mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.send_destination);
	}
	else { 						/* field entered; if entry is not valid, then take default value = NO */
		if ( read_buf[0] == YES ) {
			call_struct.send_destination = YES;
			mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.send_destination);
		}
		else 	if ( read_buf[0] == NO ){
				call_struct.send_destination = NO;
				mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.send_destination);
			}
		else  { 						/* invalid field entry */
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_50, "<default value NO taken>"));
			call_struct.send_destination = NO;
			mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.advise_arrival);
		}  /* end else */
	}

	attrset(0);
	old_piu = *piu_ptr; 						/* update piu */
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);

	return(ret_val);

}

int check_send_rate(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered) 
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int *piu_ptr;  						/* pointer to piu */
char *read_buf;						/* pointer to buffer for reading field */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether field entered =1; or not=0 */
{
	struct scr_flds *ptr_to_fld;			/* pointer to approp. piu struct */
	int old_piu, ret_val;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	attrset(A_UNDERLINE);
	if ( ! *field_entered ) { 			/* field not entered; use previouly entered value */
		mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.send_destination);
	}
	else { 						/* field entered; if entry is not valid, then take default value = NO */
		if ( read_buf[0] == YES ) {
			call_struct.send_rate = YES;
			mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.send_rate);
		}
		else 	if ( read_buf[0] == NO ){
				call_struct.send_rate = NO;
				mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.send_rate);
			}
		else  { 						/* invalid field entry */
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_50, "<default value NO taken>"));
			call_struct.send_rate = NO;
			mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, call_struct.send_rate);
		}  /* end else */
	}

	attrset(0);
	old_piu = *piu_ptr; 						/* update piu */
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);

	return(ret_val);

}
#endif

GetCustomerAddress( pCustomer )
  struct cisam_cust *pCustomer;
{
  int   rc = 0,choices, i,x;
  char cust_buf[READ_BUF_SIZE];
  int select = 0, select_made = 0;
  int rkey = 0;
  int should_clear = 0;
  WINDOW *wAlt;
  struct cisam_cust  cust_rec;  


  wAlt = srch_win;
  if ( pCustomer == NULL )
    return ( FAIL );

  bzero( &cust_rec, sizeof( struct cisam_cust ) );
  strcpy( cust_rec.phone_nbr, pCustomer->phone_nbr );
  
  choices = 0;
  if ( db_read_key( CUSTOMER_FILE_ID, &cust_rec, &cust_key1, ISEQUAL ) != SUCCESS )
    return ( 0 );
  else
    {
      memcpy( &cust_choice[ choices ], &cust_rec, sizeof( struct cisam_cust ) );
      ++choices;
      while ( rc == SUCCESS )
        {
          rc = db_read_key( CUSTOMER_FILE_ID, &cust_rec, &cust_key1, ISNEXT );
          if ( strcmp( cust_rec.phone_nbr, pCustomer->phone_nbr ) )
            rc = FAIL;
          if ( rc == SUCCESS )
            {
              memcpy( &cust_choice[ choices ], &cust_rec, sizeof( struct cisam_cust ) );
              ++choices;
            }
        }
    }

  if ( choices > 0 )
    {
      alt_win_disp();
      prt_entry_areas( wAlt, cust_flds, A_MAX );
      cust_ch_disp( choices, cust_choice );
      wrefresh( wAlt );
      
      while ( select == 0 )
        {
          i = read_field( wAlt, cust_flds, A_SLCT, cust_buf, &rkey, &should_clear );
          if ( ( clear_err_flag ) && ( i != -1 ) )
            {
              clear_error();
              refresh();
            }
          switch (i)
            {
            case 0:
            case '\n':
              rev_to_norm( wAlt, cust_flds, A_SLCT );
              if ( ( x = atoi( cust_buf ) ) < 1 )
                {
                  select = 1;
                  break;
                }
              if ( ( ( x = atoi( cust_buf ) ) < 1 ) || ( x > choices ) )
                {
                  prt_error( UI_NO_RECORD, catgets( UI_m_catd, UI_1, UI_ALTRN_2, "Customer record not found" ) );
                }
              else
                {
                  memcpy( pCustomer, &cust_choice[ x - 1 ], sizeof( struct cisam_cust ) );
                  select_made = 1;
                  select = 1;
                }
              break;
            default:
              break;
            }
        }
      
      werase( wAlt );
      wrefresh( wAlt );
      touchwin( stdscr );      
    }
  else
    {
      memcpy( pCustomer, &cust_choice[0], sizeof( struct cisam_cust ) );
      select_made = 1;
      select = 1;
    }
  

  refresh();
  
  if ( select_made == 1 )
    return( choices );
  else
    return( choices );
}
  
  
cust_ch_disp( count, altr_ch )
     int count;
     struct cisam_cust  altr_ch[];
{
    int i;									/* temporary variable */
    int row,col;								/* row and column to print data */
    char av_buf[132];								/* temporary buffer for formatting text */

    if (count == 0) {								/* is there at least on alternate choice */
	prt_error(UI_NO_RECORD,catgets(UI_m_catd, UI_1, UI_ALTRN_1, "<for alternates>"));				/* no, print error */
	return;
    }

    for(i=0; i<MAXALT; i++) {							/* print out all choices */
	row = i;								/* get row */
	col = ALT_DATA_COL1;
	if ( !( 0 ) )
	  {
	    if(row > 3){
	      row = row - 4;							/* second set of columns */
	      col = ALT_DATA_COL2;
	    }
	  }
	if (count > i) {							/* is this a valid choice? */
#ifdef NAMERICA

#else
          sprintf(av_buf,"(%d) %20s %6d    %3s %20s",
                  i+1,altr_ch[i].pckup_str_name, altr_ch[i].pckup_str_nbr,
                  altr_ch[i].pckup_city, altr_ch[i].passenger_name );
#endif
	  
	}
	else								/* out of range of choices */
	  sprintf(av_buf,"");						/* clear segment */
        mvwprintw(srch_win,row+1,col,"%-62.62s",av_buf);			/* print out choice */
    }
}
