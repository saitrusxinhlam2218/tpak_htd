static char sz__FILE__[]="@(#)ui_uud.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_uud.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:52:03
 * @(#)  Last delta: 12/2/94 at 18:16:53
 * @(#)  SCCS File: /taxi/sccs/s.ui_uud.c
 *                                                                           *
 *****************************************************************************/
#include <string.h>
#include <cursesX.h>
#include <sys/types.h>
#include <ctype.h>
#include "func_keys.h"
#include "ui_def.h"
#include "call_entry.h"
#include "screens.h"
#include "user.h"
#include "menu.h"
#include "mads_types.h"
#include "ui_error.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "calls.h"
#include "except.h"
#include "mad_ipc.h"
#include "ui_uud.h"
#include "language.h"
#include "ui_strdefs.h"

							/* define macro for checking that default search entered is correct */
#define	CHECK_SRCH(s_)	(( (s_<UNASSIGNED_CALLS_SCR) || (s_>USER_SRCH_SCR) || (s_>EXCEPTIONS_SCR && s_<VEHICLE_SRCH_SCR) ) ? 0:1)

extern int (*ui_fleet_perms[])();			/* pointers to fleet permission checking functions */
extern char req_buf[];
extern int scr_name;					/* name of screen on display */
extern char scr_type;					/* type of screen on display */
extern short end_update_user;				/* for ending update_user_defaults() */
int uud_ck_dummy(),uud_ck_fleet(),uud_ck_type(),uud_ck_city(), uud_ck_screen(), uud_ck_search(), 
    uud_ck_ttynum(), uud_ck_zonesup(), uud_ck_allgroup(), uud_ck_group(), uud_ck_extype(),
    uud_ck_tm_est(), uud_ck_lead_tm(), uud_ck_y_n();

							/* declare pointers to field checking routines */
int (*uud_ck_fld[])() = { uud_ck_dummy, uud_ck_fleet, uud_ck_type,
			uud_ck_city, uud_ck_screen, 
			uud_ck_search, uud_ck_fleet, uud_ck_fleet,
			uud_ck_fleet, uud_ck_fleet, uud_ck_fleet,
			uud_ck_fleet, uud_ck_fleet, uud_ck_fleet,
			uud_ck_zonesup, uud_ck_zonesup, uud_ck_zonesup, uud_ck_zonesup,
			uud_ck_zonesup, uud_ck_zonesup, uud_ck_zonesup,
			uud_ck_zonesup, uud_ck_zonesup, uud_ck_zonesup,
			uud_ck_zonesup, uud_ck_zonesup, uud_ck_zonesup,
			uud_ck_zonesup, uud_ck_zonesup, uud_ck_zonesup,
			uud_ck_lead_tm, uud_ck_lead_tm, uud_ck_lead_tm, uud_ck_lead_tm, 
			uud_ck_lead_tm, uud_ck_lead_tm, uud_ck_lead_tm, uud_ck_lead_tm, 
			uud_ck_lead_tm, uud_ck_lead_tm, uud_ck_lead_tm, uud_ck_lead_tm, 
			uud_ck_lead_tm, uud_ck_lead_tm, uud_ck_lead_tm, uud_ck_lead_tm, 
			uud_ck_tm_est, uud_ck_tm_est,uud_ck_tm_est, uud_ck_tm_est,
			uud_ck_tm_est, uud_ck_tm_est,uud_ck_tm_est, uud_ck_tm_est,
			uud_ck_tm_est, uud_ck_tm_est,uud_ck_tm_est, uud_ck_tm_est,
			uud_ck_tm_est, uud_ck_tm_est,uud_ck_tm_est, uud_ck_tm_est,
			uud_ck_allgroup, uud_ck_group, uud_ck_extype,
			uud_ck_extype, uud_ck_extype, uud_ck_extype, 
			uud_ck_extype, uud_ck_extype, uud_ck_extype, uud_ck_extype,
			uud_ck_group, uud_ck_extype,
			uud_ck_extype, uud_ck_extype, uud_ck_extype, 
			uud_ck_extype, uud_ck_extype, uud_ck_extype, uud_ck_extype,
			uud_ck_group, uud_ck_extype,
			uud_ck_extype, uud_ck_extype, uud_ck_extype, 
			uud_ck_extype, uud_ck_extype, uud_ck_extype, uud_ck_extype,
			uud_ck_group, uud_ck_extype,
			uud_ck_extype, uud_ck_extype, uud_ck_extype, 
			uud_ck_extype, uud_ck_extype, uud_ck_extype, uud_ck_extype,
			uud_ck_group, uud_ck_extype,
			uud_ck_extype, uud_ck_extype, uud_ck_extype, 
			uud_ck_extype, uud_ck_extype, uud_ck_extype, uud_ck_extype,
			uud_ck_group, uud_ck_extype,
			uud_ck_extype, uud_ck_extype, uud_ck_extype, 
			uud_ck_extype, uud_ck_extype, uud_ck_extype, uud_ck_extype,
			uud_ck_group, uud_ck_extype,
			uud_ck_extype, uud_ck_extype, uud_ck_extype, 
			uud_ck_extype, uud_ck_extype, uud_ck_extype, uud_ck_extype,
			uud_ck_group, uud_ck_extype,
			uud_ck_extype, uud_ck_extype, uud_ck_extype, 
			uud_ck_extype, uud_ck_extype, uud_ck_extype, uud_ck_extype,
			uud_ck_group, uud_ck_extype,
			uud_ck_extype, uud_ck_extype, uud_ck_extype, 
			uud_ck_extype, uud_ck_extype, uud_ck_extype, uud_ck_extype,
			uud_ck_group, uud_ck_extype,
			uud_ck_extype, uud_ck_extype, uud_ck_extype, 
			uud_ck_extype, uud_ck_extype, uud_ck_extype, uud_ck_extype,
#ifndef NAMERICA
			uud_ck_group, uud_ck_extype,
			uud_ck_extype, uud_ck_extype, uud_ck_extype, 
			uud_ck_extype, uud_ck_extype, uud_ck_extype, uud_ck_extype,
			uud_ck_group, uud_ck_extype,
			uud_ck_extype, uud_ck_extype, uud_ck_extype, 
			uud_ck_extype, uud_ck_extype, uud_ck_extype, uud_ck_extype,
#else
			    uud_ck_y_n, uud_ck_y_n, uud_ck_y_n, uud_ck_y_n,
			    uud_ck_y_n, uud_ck_y_n, uud_ck_y_n, uud_ck_y_n,
			    uud_ck_y_n, uud_ck_y_n, uud_ck_y_n, uud_ck_y_n,
			    uud_ck_y_n, uud_ck_y_n, uud_ck_y_n, uud_ck_y_n,
			    uud_ck_y_n, uud_ck_y_n, uud_ck_y_n, uud_ck_y_n,
			    uud_ck_y_n, uud_ck_y_n, uud_ck_y_n, uud_ck_y_n,
			    uud_ck_y_n, uud_ck_y_n, uud_ck_y_n,
#endif			    
};

extern struct users *uiuser[];			/* array of pointers to users structure in shared memory */
extern int user_index;				/* index into users struture in shared memory */
extern short clear_err_flag;			/* set if last line has an error msg printed */
extern int map_array[];				/* maps scr_name into the index in shared memory */
extern int (*security[])();
extern short clear_err_flag;			/* flag for clearing error msg on last line */
extern int uid;					/* user's unix id # */
extern struct menus *cmenu[];			/* array of pointers to menus struct in shared memory */
extern struct fleets *fleet[];			/* array of pointers to fleets struct in shared memory */
extern int def_fl_index;			/* users default fleet index */
extern int cur_fl_index;			/* users current fleet index */
extern struct keydesc lo_key1, zn_key1, eg_key1;
char excpt_fl[FLEET_MAX];			/* to save the fleets selected for exception processing */
extern struct offsets *offset;
extern struct except_bits selected_except;
extern struct except_bits temp_selected_except;
struct new_def_structs new_def_struct;		/* structure for saving new defaults entered by user */
int tmp_fl_idx;					/* maintains fleet used until updated to def_fl_index */
int num_channels, num_superzones, num_lead_tms, num_time_ests;
extern char *Spaces;

/*****************************************************************************************/
/* ui_uud.c: contains functions for processing the update_user_defaults	screen.          */
/*****************************************************************************************/
update_user_defaults()
{
	int piu;					/* prompt in use */
	int field_entered = 0;				/* whether field entered or not */
	int key_val;					/* value of the key pressed by the user */
	char read_buf[READ_BUF_SIZE];			/* buffer for reading fields on screen */
	int ret_ck_fld;					/* return value of check field routine */
	short max_piu;					/* max value of piu */
	struct scr_flds *ptr_scr_fld;			/* pointer to 1st element of array of piu structures */
	struct excpt_groups excpt_group[EXC_GROUPS];	/* for saving info on exception groups & types */
	int should_clear = 0;

	disp_update_defs_scr(excpt_group);	/* display the update_user defaults scren */
						/* initialize ptr_scr_fld to the address of basic_scr_fld[] array of structure */
	ptr_scr_fld = uud_scr_flds;		/* points to 1st element of structure */

	if ((*security[map_array[EXCEPTIONS_SCR]])() == NOT_ALLOWED)	/* initialize value of max_piu to # of fields on screen */
		max_piu = MAX_PIU_UUD1;					/* lower value of piu - calltakers */
	else 
		max_piu = MAX_PIU_UUD2;					/* user has defaluts for exceptions also */

	prt_uud_defs(&new_def_struct,excpt_group);			/* Print the default values for the fields on the screen */

									/* at this point, screen with default/old values is on */
									/* display and prompt is in the right field */
								/*  Enter while loop which executes as long as the field defined */
	 							/*  by "piu" is being entered; and the user is still on the same */
	 							/*  screen. */
	piu = 1;						/* place cursor on the first field on the screen */
	end_update_user = 0;					/* unset flag, so that we go in loop */
	while (!end_update_user && (scr_name==UPDATE_USER_DEFAULTS)) {
	      	key_val = read_field(stdscr,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);	/* field defined by piu */
		if (key_val == -1)				/* read_field() timed out before field was entered, go back */
			continue;

		
		if (clear_err_flag)				/* if last line has an error msg printed on it, then clear it */
			clear_error();
			
								/* Verify that the field entered is correct */
	     	if ((ret_ck_fld = (*uud_ck_fld[piu])(ptr_scr_fld,&piu,read_buf,key_val,&field_entered,&new_def_struct,
					excpt_group)) == DONE) {
			if (piu > max_piu)			/* field entered, change piu accordingly to next field */
				piu = 1;			/* wrap around */
			else if (piu == 0)
				piu = max_piu;			/* wrap around to last field */
		}
		else  {
			if (key_val == SEND_KEY) {		/* user is done with this screen */
				update_new_defaults(&new_def_struct,excpt_group); 	/* place new values in shared memory */
			}
			else  
			if (key_val == HELP_KEY)
			  {
/*
 *				fix
 *				force a screen repaint when done
 */
				ftnkey_handler(&piu,key_val,0);
				touchwin(stdscr);
				wrefresh(stdscr);
			  }
			else
				ftnkey_handler(&piu,key_val,0);				/* user pressed some other function key */
		}
	}
}

prt_uud_entry_areas(win_fld, ptr_scr_fld, max_piu, new_def_ptr, excpt_group_ptr)
     WINDOW *win_fld;
     struct scr_flds *ptr_scr_fld;
     int    max_piu;
     struct new_def_structs *new_def_ptr;
     struct excpt_groups *excpt_group_ptr;
{
  int i, j;
  struct scr_flds *ptr_to_fld;

  wattrset(win_fld, A_UNDERLINE);
  for (i = 1; i <= UUD_GROUP_10; i++)
    {
      ptr_to_fld = ptr_scr_fld + i;
      wmove(win_fld, ptr_to_fld->prompt_row, ptr_to_fld->prompt_col);
      for (j = 0; j<ptr_to_fld->max_chars;j++)
	waddch(win_fld,' ');
    }
  wattrset(win_fld, 0);
}

/****************************************************************************************/
/* disp_update_defs_scr: displays the update_user_defaults screen with			*/
/*		labels, but no data							*/
/****************************************************************************************/
disp_update_defs_scr(excpt_group_ptr)
struct excpt_groups *excpt_group_ptr;
{
	int indx;				/* index into menus structure on disk */
	int i, done;					/* scratch variable */
	int num_groups;
	CHANNEL_ENTRY *channel_ptr;
	int col_index;


	clear();

	attrset(A_REVERSE|A_UNDERLINE);					/* set reverse attributes */
	mvaddstr(0,0, Spaces);

	prt_date();							/* print the date on the top left corner */
	indx = map_array[scr_name];	

	mvprintw(0,SCR_TITLE_COL,"%s", cmenu[indx]->mf_desc);		/* print the title of the screen */
	mvprintw(0,SCR_TITLE_COL+strlen(cmenu[indx]->mf_desc) +3,"(%d)", cmenu[indx]->mf_command); 			/* print the title of the screen */
	
	mvprintw(0,UID_COL,"%d", uid);					/* print the user's id number */

	attrset(0);							/* reset normal attributes */

									/* print the labels necessary for all user's */
	attron(A_BOLD);
	mvaddstr(UUD_UD_ROW-1,0,catgets(UI_m_catd, UI_1, UI_UI_UUD_1, "---------------------------------------------- Defaults For User ------------------------------------------------------------------"));
	attroff(A_BOLD);
	mvaddstr(UUD_UD_ROW,0,catgets(UI_m_catd, UI_1, UI_UI_UUD_2, "Fleet:"));
	mvaddstr(UUD_UD_ROW,UUD_DISP_TYPE_COL,catgets(UI_m_catd, UI_1, UI_UI_UUD_3, "Call Type:"));
	mvaddstr(UUD_UD_ROW,UUD_DISP_CITY_COL,catgets(UI_m_catd, UI_1, UI_UI_UUD_4, "City:"));
	mvaddstr(UUD_UD_ROW,UUD_DISP_SCR_COL,catgets(UI_m_catd, UI_1, UI_UI_UUD_5, "Screen:"));
	mvaddstr(UUD_UD_ROW,UUD_DISP_SRCH_COL,catgets(UI_m_catd, UI_1, UI_UI_UUD_6, "Search:"));
	attron(A_BOLD);
	mvaddstr(UUD_EXCEPT_ROW-2,0,catgets(UI_m_catd, UI_1, UI_UI_UUD_7, "-------------------------------------------- Exception/Searches Selection Criteria -----------------------------------------------"));
	attroff(A_BOLD);
	   mvaddstr(UUD_EXCEPT_ROW-1,0,catgets(UI_m_catd, UI_1, UI_UI_UUD_8, "Fleet(s):"));
								/* if the user has permissions to work with the exceptions */
	 							/* screen, then display the defaults for the exceptions types */
	if ((*security[map_array[EXCEPTIONS_SCR]])() == ALLOWED) {	/* user is a dispatcher, so print the rest of the labels */
		mvaddstr(UUD_EXCEPT_ROW,0,catgets(UI_m_catd, UI_1, UI_UI_UUD_9, "Zone(s)/Superzone(s):"));
		mvaddstr(UUD_EXCEPT_ROW+1,0,catgets(UI_m_catd, UI_1, UI_UI_UUD_10, "Default Lead Time %:"));
		mvaddstr(UUD_EXCEPT_ROW+2,0,catgets(UI_m_catd, UI_1, UI_UI_UUD_11, "Override Time Est:"));
		attron(A_BOLD);
		mvaddstr(UUD_EXCEPT_ROW+3,0,catgets(UI_m_catd, UI_1, UI_UI_UUD_12, "..Exception Groups..    ..................................................Exception Types....................................."));
		attroff(A_BOLD);
		mvaddstr(UUD_GROUPALL_ROW,0,catgets(UI_m_catd, UI_1, UI_UI_UUD_13, "All Groups"));

		init_except_group(excpt_group_ptr);		/* initialize structures for saving exception groups/types info */
		get_group_desc(excpt_group_ptr);		/* get the group descroioption from disk file */
		num_groups = 0;
		for (i = 0; i < EXC_GROUPS; i++)		/* print the exception group description */
		  {
		        if (strlen((excpt_group_ptr + i )->group_desc))
			  {
			    mvprintw(UUD_GROUP1_ROW+num_groups,0,"%s", (excpt_group_ptr + i )->group_desc);
			    num_groups++;
			  }
		  }

#ifdef NAMERICA
		    attron(A_BOLD);
		    mvprintw(UUD_GROUP1_ROW+num_groups, 24, "..............................................Channnel Processing.....................................");
		    attroff(A_BOLD);
		    mvprintw(UUD_GROUP1_ROW+num_groups+1,0,"CHANNELS");
		    i = 0;
		    channel_ptr = &(offset->channels[i]);
		    done = 0;
		    col_index = 0;
		    num_channels = 0;
		    while ((i < 8) && (done == 0))
		      {
			mvprintw(UUD_GROUP1_ROW+num_groups+1, UUD_DISP_TYPE1_COL+13*col_index, "%s", channel_ptr->channel_short_descript);
			i++;
			channel_ptr = &(offset->channels[i]);
			if (strlen(channel_ptr->channel_id)== 0)
			  done = 1;
			col_index++;
			num_channels++;
		      }
		    col_index = 0;
		    while ((i < 16) && (done == 0))
		      {
			mvprintw(UUD_GROUP1_ROW+num_groups+2, UUD_DISP_TYPE1_COL + 13*col_index, "%s", channel_ptr->channel_short_descript);
			i++;
			channel_ptr = &(offset->channels[i]);
			if (strlen(channel_ptr->channel_id) == 0)
			  done = 1;
			col_index++;
			num_channels++;
		      }
		    col_index = 0;
		    while ((i < 18) && (done == 0))
		      {
			mvprintw(UUD_GROUP1_ROW+num_groups+3, UUD_DISP_TYPE1_COL + 13*col_index, "%s", channel_ptr->channel_short_descript);
			i++;
			channel_ptr = &(offset->channels[i]);
			if (strlen(channel_ptr->channel_id) == 0)
			  done = 1;
			col_index++;
			num_channels++;
		      }
		    num_channels--;
#endif

		get_except_types(excpt_group_ptr);		/* print the exception types description */
	}
}

/****************************************************************************************/
/* uud_ck_dummy: a dummy routine for piu = 0 value pointer to function 			*/
/****************************************************************************************/
int uud_ck_dummy(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered,new_def_ptr,excpt_group_ptr)
struct scr_flds *ptr_scr_fld;			/* pointer to 1st piu element */
int *piu_ptr;					/* pointer to piu value */
char *read_buf;					/* pointer to buffer for reading input */
int key_val;					/* value of key pressed by user */
int *field_entered;				/* whether user entered field or not */
struct new_def_structs *new_def_ptr;		/* pointer to struct for saving new defaults */
struct excpt_groups *excpt_group_ptr;
{
	return(update_piu(ptr_scr_fld,piu_ptr,key_val));
}

/****************************************************************************************/
/* uud_ck_fleet: checks the fleet entered by the user on update_user defs scr		*/
/****************************************************************************************/
int uud_ck_fleet(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered,new_def_ptr,excpt_group_ptr)
struct scr_flds *ptr_scr_fld;				/* pointer to 1st piu element */
int *piu_ptr;						/* pointer to piu value */
char *read_buf;						/* pointer to buffer for reading input */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether user entered field or not */
struct new_def_structs *new_def_ptr;			/* pointer to struct for saving new defaults */
struct excpt_groups *excpt_group_ptr;
{
	struct scr_flds *ptr_to_fld;			/* pointer to field being processed */
	int old_piu;					/* piu value of the field being verified */
	int ret_val;					/* return value from update_piu() routine */
	char *fleet_entry;				/* points to space for saving entry */
	int i;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	switch (*piu_ptr) {						/* determine which field is being entered */
		case UUD_FLEET:
			fleet_entry = &new_def_ptr->fleet;
			break;

		case UUD_EXCPT_FLEET1:
			fleet_entry = &new_def_ptr->excpt_fleet[0];
			break;

		case UUD_EXCPT_FLEET2:
			fleet_entry = &new_def_ptr->excpt_fleet[1];
			break;

		case UUD_EXCPT_FLEET3:
			fleet_entry = &new_def_ptr->excpt_fleet[2];
			break;

		case UUD_EXCPT_FLEET4:
			fleet_entry = &new_def_ptr->excpt_fleet[3];
			break;

		case UUD_EXCPT_FLEET5:
			fleet_entry = &new_def_ptr->excpt_fleet[4];
			break;

		case UUD_EXCPT_FLEET6:
			fleet_entry = &new_def_ptr->excpt_fleet[5];
			break;

		case UUD_EXCPT_FLEET7:
			fleet_entry = &new_def_ptr->excpt_fleet[6];
			break;

		case UUD_EXCPT_FLEET8:
			fleet_entry = &new_def_ptr->excpt_fleet[7];
			break;

		default:
			prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_UI_UUD_14, "<uud_ck_fleet() called>"));
			break;
	}

	if (!*field_entered) {
		if (*piu_ptr == UUD_FLEET) {					/* users default fleet */
			if (new_def_ptr->fleet == '\0') {			/* no old entry, print default value */
				if (def_fl_index != -1) {
					attrset(A_UNDERLINE);
					mvaddch(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col,fleet[def_fl_index]->fleet_id);
					attrset(0);
				}
			}
			else {							/* there is an old entry */
				if (*fleet_entry == '\0')			/* clear the field since old entry was invalid */
					clear_field(stdscr,ptr_scr_fld,*piu_ptr);
			}
		}
	}
	else {									/*  field entered */
		if (read_buf[0] == ' ') {					/* wipe out old entry */
			*fleet_entry = '\0';
			if (*piu_ptr == UUD_FLEET) {				/* print default value here */
				if (def_fl_index != -1) {
					attrset(A_UNDERLINE);
					mvaddch(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col,fleet[def_fl_index]->fleet_id);
					attrset(0);
				}
			}
		}
		else {								/* new entry */
			if (is_fleet_valid(read_buf[0])){			/* valid entry, save it */
				*fleet_entry = read_buf[0];
				if (*piu_ptr == UUD_FLEET && (*security[map_array[EXCEPTIONS_SCR]])() == ALLOWED) {
					tmp_fl_idx = find_findex(*fleet_entry);
					attrset(A_UNDERLINE);
					for (i = 0; i < MAX_EXC_ZONES; i++) {			/* print the super zone lead perc */
					  if (fleet[tmp_fl_idx]->super_zone[i].perc_lead_tm > 0)
					    mvprintw(UUD_EXCEPT_ROW+1,UUD_ZONE1_COL+i*5,"%3d",
						     fleet[tmp_fl_idx]->super_zone[i].perc_lead_tm);
					  new_def_ptr->lead_tm_perc[tmp_fl_idx][i] = fleet[tmp_fl_idx]->super_zone[i].perc_lead_tm;
					}
			
					for (i = 0; i < MAX_EXC_ZONES; i++) {		/* print the superzone time estimates */
					  if (fleet[tmp_fl_idx]->super_zone[i].man_time_est > 0)
					    mvprintw(UUD_EXCEPT_ROW+2,UUD_ZONE1_COL+i*5,"%3d",
						     fleet[tmp_fl_idx]->super_zone[i].man_time_est);
					  new_def_ptr->tm_est[tmp_fl_idx][i] = fleet[tmp_fl_idx]->super_zone[i].man_time_est;
					}
					attrset(0);
				}
			}
			else   {						/* invalid entry, trap user in field */
				old_piu = *piu_ptr;
				ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
				*piu_ptr = old_piu;
				return(ret_val);
			}
		}
	}
		
	old_piu = *piu_ptr;
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	if (*piu_ptr == old_piu -1 &&  old_piu == UUD_FLEET)
#ifdef NAMERICA
		*piu_ptr = UUD_HIA_TYPE_1 + num_channels;;					/* go to last exception group */
#else
	        *piu_ptr = UUD_GROUP_8;
#endif
	return(ret_val);
}

/****************************************************************************************/
/* uud_ck_city: checks the type entered by the user on update_user defs scr		*/
/****************************************************************************************/
int uud_ck_city(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered,new_def_ptr,excpt_group_ptr)
struct scr_flds *ptr_scr_fld;				/* pointer to 1st piu element */
int *piu_ptr;						/* pointer to piu value */
char *read_buf;						/* pointer to buffer for reading input */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether user entered field or not */
struct new_def_structs *new_def_ptr;			/* pointer to struct for saving new defaults */
struct excpt_groups *excpt_group_ptr;
{
	struct scr_flds *ptr_to_fld;
	int old_piu, ret_val;
	struct cisam_lo *city_ptr;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (!*field_entered) {					/* field not entered; print previouly entered or default value */
		if (new_def_ptr->city[0]) 
			mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col,new_def_ptr->city);
		else						/* field was never entered; print default value */
			mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col,uiuser[user_index]->default_city);
	}
	else {							/* field entered */
		if (read_buf[0] == ' ') {			/* user wants to use default value */
			new_def_ptr->city[0] = '\0';
			mvaddstr(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col,uiuser[user_index]->default_city);
		}
		else {						/* new entry for city, verify it */
			city_ptr = (struct cisam_lo *)req_buf;
			strcpy(city_ptr->code,read_buf);
			if (db(LOCALTY_FILE_ID,READ_KEYS,&lo_key1,ISEQUAL,0) < 0) {		/* invalid city entered */
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_UUD_15, "<city>"));
				old_piu = *piu_ptr;
				ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
				*piu_ptr = old_piu;						/* trap user in city field */
				return(ret_val);						/* leave user in field */
			}
			strcpy(new_def_ptr->city,read_buf);					/* city entered is valid, save it */
		}
	}
	return(update_piu(ptr_scr_fld,piu_ptr,key_val));
}

/****************************************************************************************/
/* uud_ck_type: checks the city entered by the user on update_user defs scr		*/
/****************************************************************************************/
int uud_ck_type(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered,new_def_ptr,excpt_group_ptr)
struct scr_flds *ptr_scr_fld;				/* pointer to 1st piu element */
int *piu_ptr;						/* pointer to piu value */
char *read_buf;						/* pointer to buffer for reading input */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether user entered field or not */
struct new_def_structs *new_def_ptr;			/* pointer to struct for saving new defaults */
struct excpt_groups *excpt_group_ptr;
{
	struct scr_flds *ptr_to_fld;
	short error_flag = 0;
	int old_piu, ret_val;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (!*field_entered) {					/* field not entered; if old entry, then verify it */
		if (new_def_ptr->call_type != '\0') {		/* there is an old entry */
			if (new_def_ptr->call_type != PASS  && new_def_ptr->call_type != DELIV && new_def_ptr->call_type != TYPEX) {
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_UUD_16, "<call type>"));				/* invalid old entry */
				error_flag = 1;
			}
		}
	}
	else  {							/* field entered */
		if (read_buf[0] == ' ') {			/* use default value */
			new_def_ptr->call_type = uiuser[user_index]->default_call_type;
			attrset(A_UNDERLINE);
			mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,new_def_ptr->call_type);
			attrset(0);
		}
		if (read_buf[0] != PASS && read_buf[0] != DELIV && read_buf[0] != TYPEX) {		/* new entry verify it */
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_UUD_16, "<call type>"));						/* invalid new entry */
			error_flag = 1;
		}
		else											/* valid entry, save it */
			new_def_ptr->call_type = read_buf[0];
	}
	old_piu = *piu_ptr;							/* update piu */
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	if (error_flag)								/* invalid call type, trap user in field */
		*piu_ptr = old_piu;
	return(ret_val);
}

/************************************************************************************************/
/* uud_ck_screen: checks the scr entered by the user on update_user defs scr			*/
/************************************************************************************************/
int uud_ck_screen(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered,new_def_ptr,excpt_group_ptr)
struct scr_flds *ptr_scr_fld;				/* pointer to 1st piu element */
int *piu_ptr;						/* pointer to piu value */
char *read_buf;						/* pointer to buffer for reading input */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether user entered field or not */
struct new_def_structs *new_def_ptr;			/* pointer to struct for saving new defaults */
struct excpt_groups *excpt_group_ptr;
{
	int ret_val;					/* return value from update_piu() */
	int old_piu;					/* piu value for this field */
	struct scr_flds *ptr_to_fld;			/* pointer to this fields piu struct */
	short i;					/* scratch variable */

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (!*field_entered) {					/* old value should be valid */
		if (new_def_ptr->screen == -1) {		/* user never entered screen name, print from shared memory */
			mvprintw(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col,"%-3d",uiuser[user_index]->default_screen);
		}
		else {						/* user had previouly entered a new value for the default screen */
			mvprintw(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col,"%-3d",new_def_ptr->screen);
		}
		return(update_piu(ptr_scr_fld,piu_ptr,key_val));
	}
	else {							/* field entered */
		if (read_buf[0] == ' ') {			/* user wants old default value */
			new_def_ptr->screen = -1;
			mvprintw(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col,"%-3d",uiuser[user_index]->default_screen);
		}
		else {						/* new entry; verify it */
			i = 0;
			while (read_buf[i] != '\0' && i<ptr_to_fld->max_chars) {	/* check if all chars are valid digits */
				if (!isdigit(read_buf[i])) {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_UUD_17, "<digits only>"));
					old_piu = *piu_ptr;
					ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
					*piu_ptr = old_piu;
					return(ret_val);				/* trap user in field*/
				}
				i++;
			}
			new_def_ptr->screen = atoi(read_buf);			/* got screen name; verify it */
			if (new_def_ptr->screen < 0 || new_def_ptr->screen > MAX_MENU_LIST) {	/* invalid screen # entered */
				new_def_ptr->screen = -1;  
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_UUD_18, "<screen name>"));
				old_piu = *piu_ptr;
				ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
				*piu_ptr = old_piu;
				return(ret_val);				/* trap user in field*/
			}
		}
		return(update_piu(ptr_scr_fld,piu_ptr,key_val));
	}
}

/****************************************************************************************/
/* uud_ck_srch: checks the srch entered by the user on update_user defs scr		*/
/****************************************************************************************/
int uud_ck_search(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered,new_def_ptr,excpt_group_ptr)
struct scr_flds *ptr_scr_fld;			/* pointer to 1st piu element */
int *piu_ptr;					/* pointer to piu value */
char *read_buf;					/* pointer to buffer for reading input */
int key_val;					/* value of key pressed by user */
int *field_entered;				/* whether user entered field or not */
struct new_def_structs *new_def_ptr;		/* pointer to struct for saving new defaults */
struct excpt_groups *excpt_group_ptr;
{
	int ret_val;				/* return value from update_piu() */
	int old_piu;				/* piu value for this field */
	struct scr_flds *ptr_to_fld;		/* pointer to this fields piu struct */
	short i;				/* scratch variable */

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (!*field_entered) {				/* old value should be valid */
		if (new_def_ptr->search == -1) {	/* user never entered search screen name, print from shared memory */
			mvprintw(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col,"%-3d",uiuser[user_index]->default_search);
		}
		else {					/* user had previouly entered a new value for the default screen */
			mvprintw(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col,"%-3d",new_def_ptr->search);
		}
		return(update_piu(ptr_scr_fld,piu_ptr,key_val));
	}
	else {						/* field entered */
		if (read_buf[0] == ' ') {		/* user wants old default value */
			new_def_ptr->search = -1;
			mvprintw(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col,"%-3d",uiuser[user_index]->default_search);
		}
		else {					/* new entry; verify it */
			i = 0;
			while (read_buf[i] != '\0' && i<ptr_to_fld->max_chars) {	/* check if all chars are valid digits */
				if (!isdigit(read_buf[i])) {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_UUD_17, "<digits only>"));
					old_piu = *piu_ptr;
					ret_val = update_piu(ptr_scr_fld, piu_ptr,key_val);
					*piu_ptr = old_piu;
					return(ret_val);				/* trap user in field*/
				}
				i++;
			}
			new_def_ptr->search = atoi(read_buf);				/* got search name; verify it */
			if (CHECK_SRCH(new_def_ptr->search) == 0) {			/* invalid search # entered */
				new_def_ptr->screen = -1;  
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_UUD_19, "<search name>"));
				old_piu = *piu_ptr;
				ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
				*piu_ptr = old_piu;
				return(ret_val);					/* trap user in field*/
			}
		}
		return(update_piu(ptr_scr_fld,piu_ptr,key_val));
	}
}

/****************************************************************************************/
/* uud_ck_tty#: checks the tty# entered by the user on update_user defs scr		*/
/****************************************************************************************/
int uud_ck_ttynum(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered,new_def_ptr,excpt_group_ptr)
struct scr_flds *ptr_scr_fld;				/* pointer to 1st piu element */
int *piu_ptr;						/* pointer to piu value */
char *read_buf;						/* pointer to buffer for reading input */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether user entered field or not */
struct new_def_structs *new_def_ptr;			/* pointer to struct for saving new defaults */
struct excpt_groups *excpt_group_ptr;
{

	struct scr_flds *ptr_to_fld;


	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (!*field_entered) {				/* field not entered; so use previouly defined value */
		mvprintw(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,"%d",new_def_ptr->no_of_tty);
	}
	else {						/* field entered */
		if (read_buf[0] == ' ') {		/* use default value of 1 terminal */
			new_def_ptr->no_of_tty = 1;
			mvprintw(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,"%d",new_def_ptr->no_of_tty);
		}
		else {					/* new entry; if not valid, use 1 */
			if (read_buf[0] != '1' && read_buf[0] != '2') {
				new_def_ptr->no_of_tty = 1;
				mvprintw(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col,"%d", new_def_ptr->no_of_tty);
				prt_error(UI_NOFIELD, catgets(UI_m_catd, UI_1, UI_UI_UUD_20, "<default value '1' taken>"));
			}
			else {
				read_buf[1] = '\0';
				new_def_ptr->no_of_tty =  atoi(read_buf);
			}
		}
	}
	return(update_piu(ptr_scr_fld,piu_ptr,key_val));
}

/****************************************************************************************/
/* uud_ck_zonesup: checks the zonesup entered by the user on update_user defs scr       */
/****************************************************************************************/
int uud_ck_zonesup(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered,new_def_ptr,excpt_group_ptr)
struct scr_flds *ptr_scr_fld;				/* pointer to 1st piu element */
int *piu_ptr;						/* pointer to piu value */
char *read_buf;						/* pointer to buffer for reading input */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether user entered field or not */
struct new_def_structs *new_def_ptr;			/* pointer to struct for saving new defaults */
struct excpt_groups *excpt_group_ptr;
{
	struct scr_flds *ptr_to_fld;
	int old_piu, ret_val;
	short *zone_ptr;				/* to point to  zone field being entered */
	int indx;					/* index for shared memory, user struct zone field */

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);
	
	switch (*piu_ptr) {				/* determine which field is being entered */
		case UUD_ZONE1:
			zone_ptr = &new_def_ptr->zonesup[0];
			indx = 0;
			break;

		case UUD_ZONE2:
			zone_ptr = &new_def_ptr->zonesup[1];
			indx = 1;
			break;

		case UUD_ZONE3:
			zone_ptr = &new_def_ptr->zonesup[2];
			indx = 2;
			break;

		case UUD_ZONE4:
			zone_ptr = &new_def_ptr->zonesup[3];
			indx = 3;
			break;

		case UUD_ZONE5:
			zone_ptr = &new_def_ptr->zonesup[4];
			indx = 4;
			break;

		case UUD_ZONE6:
			zone_ptr = &new_def_ptr->zonesup[5];
			indx = 5;
			break;

		case UUD_ZONE7:
			zone_ptr = &new_def_ptr->zonesup[6];
			indx = 6;
			break;

		case UUD_ZONE8:
			zone_ptr = &new_def_ptr->zonesup[7];
			indx = 7;
			break;

		case UUD_ZONE9:
			zone_ptr = &new_def_ptr->zonesup[8];
			indx = 8;
			break;

		case UUD_ZONE10:
			zone_ptr = &new_def_ptr->zonesup[9];
			indx = 9;
			break;

		case UUD_ZONE11:
			zone_ptr = &new_def_ptr->zonesup[10];
			indx = 10;
			break;

		case UUD_ZONE12:
			zone_ptr = &new_def_ptr->zonesup[11];
			indx = 11;
			break;

		case UUD_ZONE13:
			zone_ptr = &new_def_ptr->zonesup[12];
			indx = 12;
			break;

		case UUD_ZONE14:
			zone_ptr = &new_def_ptr->zonesup[13];
			indx = 13;
			break;

		case UUD_ZONE15:
			zone_ptr = &new_def_ptr->zonesup[14];
			indx = 14;
			break;

		case UUD_ZONE16:
			zone_ptr = &new_def_ptr->zonesup[15];
			indx = 15;
			break;

		default:						/* error */
			return(update_piu(ptr_scr_fld,piu_ptr,key_val));
		
	}

	if (!*field_entered) {				/* field not entered; verify old entry if any */
		if (*zone_ptr == (short)0) {		/* no previous entry */
			*zone_ptr = 0;
		}
		else {					/* there is old entry, verify it */
			if (!is_zonesup_valid(*zone_ptr,new_def_ptr)) {			/* invalid old entry */
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_UUD_21, "<zone/superzone #>"));
				old_piu = *piu_ptr;
				ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
				*piu_ptr = old_piu;
				return(ret_val);
			}
		}
	}
	else {						/* field entered */
		if (read_buf[0] == ' ') {		/* wipe out old entry */
			*zone_ptr = 0;
			attrset(A_UNDERLINE);
			clear_field(stdscr,ptr_scr_fld,*piu_ptr);
			attrset(0);
		}
		else {					/* new entry , verify it */
			*zone_ptr = (short)atoi(read_buf);
			if (!is_zonesup_valid(*zone_ptr,new_def_ptr)) {			/* invalid new entry */
				*zone_ptr = (short)0;
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_UUD_21, "<zone/superzone #>"));
				old_piu = *piu_ptr;
				ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
				*piu_ptr = old_piu;
				return(ret_val);
			}
		}
	}
	ret_val = update_piu(ptr_scr_fld, piu_ptr, key_val);
	if (*piu_ptr > (UUD_ZONE1 + num_superzones - 1))
	    {
	      if (num_lead_tms > 0)
		*piu_ptr = UUD_LEAD_TM1;
	      else if (num_time_ests > 0)
		*piu_ptr = UUD_TM_EST1;
	      else
		*piu_ptr = UUD_GROUP_1;
	    }
	      
	return(ret_val);
}




set_groupall(struct scr_flds *ptr_scr_fld,int *piu_ptr,int key_val,int ptr_index,struct excpt_groups *excpt_group_ptr)
{
	register short i;
        struct scr_flds *ptr_to_fld;
	int old_piu;

        ptr_to_fld = ptr_scr_fld + *piu_ptr;
 

	
	attrset(A_UNDERLINE);						/* print the value */
        for (i = 0; i < (excpt_group_ptr + ptr_index)->how_many; i++) 
	{
                 (excpt_group_ptr + ptr_index)->type_y_n[i] = YES;
                 mvaddch( (ptr_to_fld + i + 1)->prompt_row, (ptr_to_fld + i + 1)->prompt_col, YES);
        }
	attrset(0);						/* print the value */
	

}




/*****************************************************************************************/
/* uud_ck_group: checks the allgroup entered by the user on update_user defs scr         */
/*****************************************************************************************/
int uud_ck_group(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered,new_def_ptr,excpt_group_ptr)
struct scr_flds *ptr_scr_fld;			/* pointer to 1st piu element */
int *piu_ptr;					/* pointer to piu value */
char *read_buf;					/* pointer to buffer for reading input */
int key_val;					/* value of key pressed by user */
int *field_entered;				/* whether user entered field or not */
struct new_def_structs *new_def_ptr;		/* pointer to struct for saving new defaults */
struct excpt_groups *excpt_group_ptr;
{
	struct scr_flds *ptr_to_fld;
	int ptr_index = 0;	
	int old_piu, ret_val;
	short i;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	switch (*piu_ptr) {					/* determine which field is being entered */
		case UUD_GROUP_1:
			ptr_index = 0;
			break;

		case UUD_GROUP_2:
			ptr_index = 1;
			break;

		case UUD_GROUP_3:
			ptr_index = 2;
			break;

		case UUD_GROUP_4:
			ptr_index = 3;
			break;

		case UUD_GROUP_5:
			ptr_index = 4;
			break;

		case UUD_GROUP_6:
			ptr_index = 5;
			break;

		case UUD_GROUP_7:
			ptr_index = 6;
			break;

		case UUD_GROUP_8:
			ptr_index = 7;
			break;

		case UUD_GROUP_9:
			ptr_index = 8;
			break;

		case UUD_GROUP_10:
			ptr_index = 9;
			break;

		case UUD_GROUP_11:
			ptr_index = 10;
			break;

		default:
			prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_UI_UUD_22, "<uud_ck_group() called>"));
			return(update_piu(ptr_scr_fld,piu_ptr,key_val));
	}

	switch (is_y_n(read_buf[0]))
	{
		case 0: 
			(excpt_group_ptr + ptr_index)->group_y_n =  NO ;
			break;
		case 1:
			(excpt_group_ptr + ptr_index)->group_y_n =  YES ;
			break;
		case 2:
			(excpt_group_ptr + ptr_index)->group_y_n =  YES ;
			mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,(excpt_group_ptr + ptr_index)->group_y_n);
			set_groupall(ptr_scr_fld,piu_ptr,key_val,ptr_index,excpt_group_ptr);
			old_piu = *piu_ptr;						/* update piu */
			ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
			
			if (*piu_ptr == old_piu + 1)
				*piu_ptr = old_piu + (UUD_GROUP_2 - UUD_GROUP_1);	/* go to next group */
			return(ret_val);
			break;
		default: break;
	}

	attrset(A_UNDERLINE);						/* print the value */
	mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,(excpt_group_ptr + ptr_index)->group_y_n);
 /* if group is not selected,then deselect all the exception types belonging to this group */
	if ((excpt_group_ptr + ptr_index)->group_y_n == NO) {
		for (i = 0; i < (excpt_group_ptr + ptr_index)->how_many; i++) {
			(excpt_group_ptr + ptr_index)->type_y_n[i] = NO;
			mvaddch( (ptr_to_fld + i + 1)->prompt_row, (ptr_to_fld + i + 1)->prompt_col, NO);
		}
	}
	attrset(0);

	old_piu = *piu_ptr;						/* update piu */
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	if (*piu_ptr == old_piu -1) {					/* go to previous group field */
		if (*piu_ptr > UUD_GROUP_1)
			*piu_ptr = old_piu - (UUD_GROUP_2 - UUD_GROUP_1);
	}
	else if ((excpt_group_ptr + ptr_index)->how_many == 0 || (excpt_group_ptr + ptr_index)->group_y_n == NO) {
		if (*piu_ptr == old_piu + 1)
			*piu_ptr = old_piu + (UUD_GROUP_2 - UUD_GROUP_1);	/* go to next group */
	}
	if ((*piu_ptr == UUD_ALL_GROUPS) && (old_piu == UUD_GROUP_1))
	  {
	    if (num_time_ests > 0)
	      *piu_ptr = UUD_TM_EST1 + num_time_ests - 1;
	    else if (num_lead_tms > 0)
	      *piu_ptr = UUD_LEAD_TM1 + num_lead_tms - 1;
	    else
	      *piu_ptr = UUD_ZONE1 + num_superzones - 1;
	  }
	      
#ifdef NAMERICA
	    if (*piu_ptr == UUD_GROUP_10)
	      *piu_ptr = UUD_HIA_TYPE_1;
#else
	  if (*piu_ptr == UUD_GROUP_9)
	    *piu_ptr = UUD_FLEET;
#endif
	return(ret_val);
}




/****************************************************************************************/
/* uud_ck_allgroup: checks the group entered by the user on update_user defs scr	*/
/****************************************************************************************/
int uud_ck_allgroup(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered,new_def_ptr,excpt_group_ptr)
struct scr_flds *ptr_scr_fld;				/* pointer to 1st piu element */
int *piu_ptr;						/* pointer to piu value */
char *read_buf;						/* pointer to buffer for reading input */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether user entered field or not */
struct new_def_structs *new_def_ptr;			/* pointer to struct for saving new defaults */
struct excpt_groups *excpt_group_ptr;
{

	register short i, j;
	int ret_val;
	int old_piu;
	int piu;

	if (!*field_entered) {				/* old entry if any was already verified */
	}
	else {						/* new entry */  
		switch (is_y_n(read_buf[0])) 
		{
		    
		   case 0:
			new_def_ptr->excpt_groupall = NO;
			break;
		   case 1:
			new_def_ptr->excpt_groupall = YES;
			break;
		   case 2:
			new_def_ptr->excpt_groupall = YES; 

			attrset(A_UNDERLINE);						/* print the value */
			for (i = 0; i < EXC_GROUPS; i++) {
				if ((strcmp((excpt_group_ptr + i)->group_desc,""))&&(strcmp((excpt_group_ptr + i)->group_desc," ")))
				{
					(excpt_group_ptr + i )->group_y_n = YES;			/* group not selected */
					mvaddch(UUD_GROUP1_ROW+i, UUD_GROUP_COL, (excpt_group_ptr + i )->group_y_n);
					for (j = 0; j < (excpt_group_ptr + i)->how_many;j++)
					{
						(excpt_group_ptr + i )->type_y_n[j] = YES;		/* group not selected */
						mvaddch(UUD_GROUP1_ROW+i, UUD_TYPE1_COL+13*j, (excpt_group_ptr + i )->type_y_n[j]);
					}
				}
			}

			attrset(0);						/* print the value */
/*
			prt_uud_defs(new_def_ptr,excpt_group_ptr);
*/
			
			break;
		   default:
			break;
		
		}
	}
	return(update_piu(ptr_scr_fld,piu_ptr,key_val));
}

/****************************************************************************************/
/* uud_ck_lead_tm: checks the lead time percent entered for a superzone
/****************************************************************************************/
int uud_ck_lead_tm(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered,new_def_ptr,excpt_group_ptr)
struct scr_flds *ptr_scr_fld;				/* pointer to 1st piu element */
int *piu_ptr;						/* pointer to piu value */
char *read_buf;						/* pointer to buffer for reading input */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether user entered field or not */
struct new_def_structs *new_def_ptr;			/* pointer to struct for saving new defaults */
struct excpt_groups *excpt_group_ptr;
{
	struct scr_flds *ptr_to_fld;			/* pointer to field being processed */
	int old_piu;					/* piu value of the field being verified */
	int ret_val;					/* return value from update_piu() routine */
	short *zone_ptr;				/* points to space for saving entry */

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);
	
	zone_ptr = &new_def_ptr->lead_tm_perc[cur_fl_index][*piu_ptr-UUD_LEAD_TM1];

	if (!*field_entered) {				/* field not entered; verify old entry if any */
		if (*zone_ptr == (short)0) {		/* no previous entry */
			*zone_ptr = 0;
		}
		else {					/* there is old entry, verify it */
			if ( *zone_ptr <0 || *zone_ptr > 999) {			/* invalid old entry */
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_UUD_23, "<lead time percentage>"));
				old_piu = *piu_ptr;
				ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
				*piu_ptr = old_piu;
				return(ret_val);
			}
		}
	}
	else {						/* field entered */
		if (read_buf[0] == ' ') {		/* wipe out old entry */
			*zone_ptr = 0;
			attrset(A_UNDERLINE);
			clear_field(stdscr,ptr_scr_fld,*piu_ptr);
			attrset(0);
		}
		else {					/* new entry , verify it */
			*zone_ptr = (short)atoi(read_buf);
			if ( *zone_ptr <0 || *zone_ptr > 999) {			/* invalid old entry */
				*zone_ptr = (short)0;
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_UUD_23, "<lead time percentage>"));
				old_piu = *piu_ptr;
				ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
				*piu_ptr = old_piu;
				return(ret_val);
			}
		}
	}
	old_piu = *piu_ptr;
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	if (*piu_ptr > (UUD_LEAD_TM1 + num_lead_tms - 1)) /* Handle case where we drop down one line */
	    *piu_ptr = UUD_TM_EST1;
	else if ((old_piu == UUD_LEAD_TM1) && (*piu_ptr > (UUD_ZONE1 + num_superzones - 1))
		 && (key_val == KEY_UP))
	  *piu_ptr = UUD_ZONE1 + num_superzones - 1;
	return(ret_val);

}

/****************************************************************************************/
/* uud_ck_tm_est: checks the manual time estimate for a superzone
/****************************************************************************************/
int uud_ck_tm_est(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered,new_def_ptr,excpt_group_ptr)
struct scr_flds *ptr_scr_fld;				/* pointer to 1st piu element */
int *piu_ptr;						/* pointer to piu value */
char *read_buf;						/* pointer to buffer for reading input */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether user entered field or not */
struct new_def_structs *new_def_ptr;			/* pointer to struct for saving new defaults */
struct excpt_groups *excpt_group_ptr;
{
	struct scr_flds *ptr_to_fld;			/* pointer to field being processed */
	int old_piu;					/* piu value of the field being verified */
	int ret_val;					/* return value from update_piu() routine */
	short *zone_ptr;				/* points to space for saving entry */

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);
	
	zone_ptr = &new_def_ptr->tm_est[cur_fl_index][*piu_ptr-UUD_TM_EST1];

	if (!*field_entered) {				/* field not entered; verify old entry if any */
		if (*zone_ptr == (short)0) {		/* no previous entry */
			*zone_ptr = 0;
		}
		else {					/* there is old entry, verify it */
			if ( *zone_ptr <0 || *zone_ptr > 999) {			/* invalid old entry */
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_UUD_24, "<manual time estimate>"));
				old_piu = *piu_ptr;
				ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
				*piu_ptr = old_piu;
				return(ret_val);
			}
		}
	}
	else {						/* field entered */
		if (read_buf[0] == ' ') {		/* wipe out old entry */
			*zone_ptr = 0;
			attrset(A_UNDERLINE);
			clear_field(stdscr,ptr_scr_fld,*piu_ptr);
			attrset(0);
		}
		else {					/* new entry , verify it */
			*zone_ptr = (short)atoi(read_buf);
			if ( *zone_ptr <0 || *zone_ptr > 999) {			/* invalid old entry */
				*zone_ptr = (short)0;
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_UUD_24, "<manual time estimate>"));
				old_piu = *piu_ptr;
				ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
				*piu_ptr = old_piu;
				return(ret_val);
			}
		}
	}
	old_piu = *piu_ptr;
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	if (*piu_ptr > (UUD_TM_EST1 + num_time_ests - 1))
	  *piu_ptr = UUD_GROUP_1;
	else if ((old_piu == UUD_TM_EST1) && (*piu_ptr > (UUD_LEAD_TM1 + num_lead_tms - 1))
		 && (key_val == KEY_UP))
	  *piu_ptr = UUD_LEAD_TM1 + num_lead_tms - 1;
	return(ret_val);

}

/***************************************************************************/
/* uud_ck_type: checks the type entered by the user on update_user defs scr*/
/***************************************************************************/
int uud_ck_extype(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered,new_def_ptr,excpt_group_ptr)
struct scr_flds *ptr_scr_fld;				/* pointer to 1st piu element */
int *piu_ptr;						/* pointer to piu value */
char *read_buf;						/* pointer to buffer for reading input */
int key_val;						/* value of key pressed by user */
int *field_entered;					/* whether user entered field or not */
struct new_def_structs *new_def_ptr;			/* pointer to struct for saving new defaults */
struct excpt_groups *excpt_group_ptr;
{
	struct scr_flds *ptr_to_fld;
	int ret_val, old_piu;
	int group_index, type_index;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);
							/* determine the field being entered */
	group_index = ((*piu_ptr - UUD_GROUP_1)/(UUD_GROUP_2 - UUD_GROUP_1));
	type_index =  ((*piu_ptr - UUD_GROUP_1)%(UUD_GROUP_2 - UUD_GROUP_1)) - 1;

	if (!*field_entered) {				/* take default value of NO */
		(excpt_group_ptr + group_index)->type_y_n[type_index] = NO;
	}
	else {						/* field entered */
		switch (is_y_n(read_buf[0]))
		{
			case 0: 
				(excpt_group_ptr + group_index)->type_y_n[type_index] =  NO ;
				break;
			case 1:
				(excpt_group_ptr + group_index)->type_y_n[type_index] =  YES ;
				break;
			case 2:
				(excpt_group_ptr + group_index)->type_y_n[type_index] =  YES ;
				break;
			default: break;
		}

	}

	attrset(A_UNDERLINE);				/* print the value */
	mvaddch(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col,(excpt_group_ptr + group_index)->type_y_n[type_index]);
	attrset(0);

	old_piu = *piu_ptr;
	ret_val = update_piu(ptr_scr_fld,piu_ptr,key_val);
	if (*piu_ptr == old_piu + 1) {							/* check if this exception type exists */
		if ((excpt_group_ptr + group_index)->how_many <= type_index + 1)	/* this exception type does not exist */
			*piu_ptr = UUD_GROUP_1 + (group_index + 1)*(UUD_GROUP_2 - UUD_GROUP_1);	/* go to next group field */
	}

#ifdef NAMERICA
	    if (*piu_ptr == UUD_GROUP_9)
	      *piu_ptr = UUD_HIA_TYPE_1;
#else
	  if (*piu_ptr == UUD_GROUP_9)
	    *piu_ptr = UUD_FLEET;
#endif

	return(ret_val);
}

int uud_ck_y_n (ptr_scr_fld,piu_ptr, read_buf, key_val, field_entered, new_def_ptr,excpt_group_ptr)
        struct scr_flds *ptr_scr_fld;
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
        struct new_def_structs *new_def_ptr;
        struct excpt_groups *excpt_group_ptr;
{
  struct scr_flds *ptr_to_fld;
  int ret_val, old_piu;
 
  int    channel_index;

#ifdef NAMERICA
  ptr_to_fld = ptr_scr_fld + (*piu_ptr);
  channel_index = ((*piu_ptr - UUD_HIA_TYPE_1));
  if (!*field_entered) {
    new_def_ptr->channels[channel_index] = 0;
  }
  else
    {
      switch (is_y_n(read_buf[0]))
	{
	case 0:
	  new_def_ptr->channels[channel_index] = 0;
	  break;
	case 1:
	  new_def_ptr->channels[channel_index] = 1;
	  break;
	case 2:
	  new_def_ptr->channels[channel_index] = 1;
	  break;
	default: break;
	}
    }
  attrset(A_UNDERLINE);
  if (new_def_ptr->channels[channel_index] == 1)
    mvaddch(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, 'Y');
  else
    mvaddch(ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, 'N');
  attrset(0);

  old_piu = *piu_ptr;
  ret_val = update_piu(ptr_scr_fld, piu_ptr, key_val);
  if (*piu_ptr > (UUD_HIA_TYPE_1 + num_channels))
    *piu_ptr = UUD_FLEET;
  if ((*piu_ptr == UUD_G11_TYPE_8) && (old_piu == UUD_HIA_TYPE_1))
    *piu_ptr = UUD_G8_TYPE_1;
  if ((*piu_ptr == UUD_GROUP_11))
    *piu_ptr = UUD_G8_TYPE_1;
  if ((*piu_ptr == UUD_G11_TYPE_8) && (old_piu == UUD_HIA_TYPE_8))
    *piu_ptr = UUD_HIA_TYPE_9;
#endif
  return(DONE);
}
  
    
  
/*******************************************************************/
/* prt_uud_defs: prints the users default parameters on the screen */
/*******************************************************************/
prt_uud_defs(new_def_ptr,excpt_group_ptr)
struct new_def_structs *new_def_ptr;					/* pointer to defaults struct */
struct excpt_groups *excpt_group_ptr;
{
	int i,j,col_indx;
	int num_groups;
	
	attrset(A_UNDERLINE);

	if (def_fl_index != -1){
		mvaddch(UUD_UD_ROW,UUD_FLEET_COL,fleet[def_fl_index]->fleet_id);
		tmp_fl_idx = def_fl_index;
	}

	mvaddch(UUD_UD_ROW,UUD_TYPE_COL,uiuser[user_index]->default_call_type);
	mvaddstr(UUD_UD_ROW,UUD_CITY_COL,uiuser[user_index]->default_city);
	mvprintw(UUD_UD_ROW,UUD_SCR_COL,"%d", uiuser[user_index]->default_screen);
	mvprintw(UUD_UD_ROW,UUD_SEARCH_COL,"%d", uiuser[user_index]->default_search);

					/* if exception types/group fields are displayed, then display the default for those also */
	for (i = 0; i < FLEET_MAX; i++) {				/* print the fleet id's */
		new_def_ptr->excpt_fleet[i] = excpt_fl[i];
		if (new_def_ptr->excpt_fleet[i] != '\0')
			mvaddch(UUD_EXCEPT_ROW-1,UUD_FLEET1_COL+2*i, new_def_ptr->excpt_fleet[i]);
	}

	if ((*security[map_array[EXCEPTIONS_SCR]])() == ALLOWED) {
		new_def_ptr->no_of_tty = 1;					/* print the # of terminals */
		new_def_ptr->scr_tty1 = 0;					/* let MAIN_MENU be the default */
                num_superzones = 0;
		for (i = 0; i < MAX_EXC_ZONES; i++) {				/* print the zones & superzones */
		        if (uiuser[user_index]->zones_sel[i] > 0)
			  {
			    mvprintw(UUD_EXCEPT_ROW,UUD_ZONE1_COL+i*5,"%d",uiuser[user_index]->zones_sel[i]);
			    num_superzones++;
			  }
			new_def_ptr->zonesup[i] = uiuser[user_index]->zones_sel[i];
		}
		num_lead_tms = 0;
		for (i = 0; i < num_superzones; i++) {				/* print the super zone lead perc */
		  mvprintw(UUD_EXCEPT_ROW+1,UUD_ZONE1_COL+i*5,"%3d",fleet[def_fl_index]->super_zone[i].perc_lead_tm);
		  num_lead_tms++;
		  new_def_ptr->lead_tm_perc[def_fl_index][i] = fleet[def_fl_index]->super_zone[i].perc_lead_tm;
		}
		num_time_ests = 0;
		for (i = 0; i < num_superzones; i++) {				/* print the superzone time estimates */
		  mvprintw(UUD_EXCEPT_ROW+2,UUD_ZONE1_COL+i*5,"%3d",fleet[def_fl_index]->super_zone[i].man_time_est);
		  num_time_ests++;
		  new_def_ptr->tm_est[def_fl_index][i] = fleet[def_fl_index]->super_zone[i].man_time_est;
		}

#ifdef NAMERICA
		    col_indx = 0;
		    for (i = 0; i < ((num_channels+1) > 8 ? 8:(num_channels+1)); i++) {
		      if (uiuser[user_index]->channels[i] == 1)
			mvaddch(UUD_HIA1_ROW, UUD_TYPE1_COL+13*col_indx,'Y');
		      else
			mvaddch(UUD_HIA1_ROW, UUD_TYPE1_COL+13*col_indx,'N');
		      new_def_ptr->channels[i] = uiuser[user_index]->channels[i];
		      col_indx++;
		    }
		    col_indx = 0;
		    for (i = 8; i < ((num_channels+1) > 16 ? 16:(num_channels+1)); i++){
		      if (uiuser[user_index]->channels[i] == 1)
			mvaddch(UUD_HIA2_ROW, UUD_TYPE1_COL+13*col_indx, 'Y');
		      else
			mvaddch(UUD_HIA2_ROW, UUD_TYPE1_COL+13*col_indx, 'N');
		      new_def_ptr->channels[i] = uiuser[user_index]->channels[i];
		      col_indx++;
		    }
		    col_indx = 0;
		    for (i = 16; i< (num_channels+1); i++) {
		      if (uiuser[user_index]->channels[i] == 1)
			mvaddch(UUD_HIA3_ROW, UUD_TYPE1_COL+13*col_indx, 'Y');
		      else
			mvaddch(UUD_HIA3_ROW, UUD_TYPE1_COL+13*col_indx, 'N');
		      new_def_ptr->channels[i] = uiuser[user_index]->channels[i];
		      col_indx++;
		    }
#endif

		get_old_types(excpt_group_ptr);			/* print the old exception group/type selection criteria */
		num_groups = 0;
		for ( i = 0; i < EXC_GROUPS; i++ )  {
			if ( (excpt_group_ptr + i )->should_process == YES )  {
			  if (strlen((excpt_group_ptr + i)->group_desc))
			    {
				  mvaddch(UUD_GROUP1_ROW+num_groups, UUD_GROUP_COL, (excpt_group_ptr + i )->group_y_n);
				  /* print types selected in this group */
				  for ( j = 0; j < (excpt_group_ptr + i )->how_many; j++ )
				    if (((excpt_group_ptr+i)->type_y_n[j] == YES) || ((excpt_group_ptr+i)->type_y_n[j] == NO))
				      mvaddch(UUD_GROUP1_ROW+num_groups, UUD_TYPE1_COL+13*j, (excpt_group_ptr + i )->type_y_n[j]);
				  num_groups++;
			    }
			}  /* end if */
		}  /* end for */

	}  /* end if ... user is a supervisor */

	attrset(0);
	
	new_def_ptr->screen = -1;
	new_def_ptr->search = -1;
	new_def_ptr->no_of_tty = 1;
	new_def_ptr->scr_tty1 = 0;	/* let MAIN_MENU be the default */

}	/* end prt_uud_defs()  */

/***************************************************************************************/
/* update_new_defaults: writes the users new default parameters in shared memory, etc  */
/***************************************************************************************/
update_new_defaults(ptr,excpt_group_ptr)
struct	new_def_structs *ptr;						/* pointer to the struct with new defaults */
struct excpt_groups *excpt_group_ptr;
{
	int findex;							/* dummy to save new default fleet index */
	int i;
	int type;
	int fleet_index;

	if (ptr->fleet != '\0') { 					/* change default fleet is required */
		if ((findex = find_findex(ptr->fleet)) == -1)		/* save new default fleet; if valid */
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_UUD_25, "<default fleet unchanged>"));
		else {
			def_fl_index = findex;
			cur_fl_index = def_fl_index;
			uiuser[user_index]->default_fleet = (short)def_fl_index;
		}
	}

	if (ptr->call_type != '\0')
		uiuser[user_index]->default_call_type = ptr->call_type;

	if (ptr->city[0]) { 						/* change default city */
		strncpy(uiuser[user_index]->default_city, ptr->city,3);
		ptr->city[3] = '\0';
	}

	if (ptr->screen != -1)						/* change default screen */
		uiuser[user_index]->default_screen = ptr->screen;

	if (ptr->search != -1)						/* change default search */
		uiuser[user_index]->default_search = ptr->search;

	for (i = 0; i < FLEET_MAX; i++) 				/* initialize the fleet selection area */
		excpt_fl[i] = '\0';

	for (i = 0; i < FLEET_MAX; i++) {					/* save fleets selected for exception processing */
		fleet_index = find_findex(ptr->excpt_fleet[i]);			/* arrive at fleet number for fleet id */
		if (fleet_index >= 0 && fleet_index < FLEET_MAX)		/* valid fleet number ? */
			excpt_fl[fleet_index] = ptr->excpt_fleet[i];
	}
	
	if (excpt_fl[0] == '\0')
		uiuser[user_index]->fleet_perms.fleet0 = 0;
	else
		uiuser[user_index]->fleet_perms.fleet0 = 1;
	
	if (excpt_fl[1] == '\0')
		uiuser[user_index]->fleet_perms.fleet1 = 0;
	else
		uiuser[user_index]->fleet_perms.fleet1 = 1;
	
	if (excpt_fl[2] == '\0')
		uiuser[user_index]->fleet_perms.fleet2 = 0;
	else
		uiuser[user_index]->fleet_perms.fleet2 = 1;
	
	if (excpt_fl[3] == '\0')
		uiuser[user_index]->fleet_perms.fleet3 = 0;
	else
		uiuser[user_index]->fleet_perms.fleet3 = 1;
	
	if (excpt_fl[4] == '\0')
		uiuser[user_index]->fleet_perms.fleet4 = 0;
	else
		uiuser[user_index]->fleet_perms.fleet4 = 1;
	
	if (excpt_fl[5] == '\0')
		uiuser[user_index]->fleet_perms.fleet5 = 0;
	else
		uiuser[user_index]->fleet_perms.fleet5 = 1;
	
	if (excpt_fl[6] == '\0')
		uiuser[user_index]->fleet_perms.fleet6 = 0;
	else
		uiuser[user_index]->fleet_perms.fleet6 = 1;
	
	if (excpt_fl[7] == '\0')
		uiuser[user_index]->fleet_perms.fleet7 = 0;
	else
		uiuser[user_index]->fleet_perms.fleet7 = 1;

	
	for (i = 0; i < MAX_EXC_ZONES; i++)				/* update zones/superzones selected for excptprocessing */
		uiuser[user_index]->zones_sel[i] = ptr->zonesup[i];

	for (i = 0; i < MAX_EXC_ZONES; i++)				/* update zones/superzones selected for excptprocessing */
		fleet[def_fl_index]->super_zone[i].perc_lead_tm = ptr->lead_tm_perc[def_fl_index][i];

	for (i = 0; i < MAX_EXC_ZONES; i++)				/* update zones/superzones selected for excptprocessing */
		fleet[def_fl_index]->super_zone[i].man_time_est = ptr->tm_est[def_fl_index][i];

					/* update shared memory with info about the new exception types selected by the user */
	for (type = 0; type < EXC_TYPES; type++)
			uiuser[user_index]->exc_grp_slct[type] = type_selected_in_group(type,excpt_group_ptr);

#ifdef NAMERICA
	for (i = 0; i < 18; i++)
	  uiuser[user_index]->channels[i] = ptr->channels[i];
#endif

					/* now save the exception types that the user has selected in the bit structure */
	memset((char *)&selected_except,'\0',sizeof(struct except_bits));			/* reinitialize it */
	for (type = 0; type < EXC_TYPES; type++) {
		if ( uiuser[user_index]->exc_grp_slct[type] > 0 ) 	/* user has selected this exception type; so set the */
			 						/* bit in the temp_selected_except bit structure */
			set_bit(type,sizeof(struct except_bits),(unsigned char *)&selected_except);
	}

	temp_selected_except = selected_except;				/* make temp selection like newly defined selection */
	
/*	write_counter(CNT_OPEN);		/* write request to counter process to recount for the 25th line */

	menu_manager(YES,uiuser[user_index]->default_screen); 			/* go to the default screen */
}

/**************************************************************************/
/* is_fleet_valid: Returns 0 ...if fleet entered is not valid.            */
/*		   Returns 1   if fleet enterd is valid 		  */
/**************************************************************************/
int is_fleet_valid(fleet_entered)
char fleet_entered;				/* fleet entered on update user defaults screen */
{
	int indx;

	if ((indx = find_findex(fleet_entered)) == -1) {			/* check if fleet id entered is valid */
		prt_error(UI_NOFIELD,"<%s: %c>", catgets(UI_m_catd, UI_1, UI_UI_UUD_26, "fleet"), fleet_entered);
		return(0);
	}
	if (fleet[indx]->active != YES) {					/* inactive fleet */
		prt_error(UI_NOT_ACTIVE,"<%s: %c>", catgets(UI_m_catd, UI_1, UI_UI_UUD_26, "fleet"), fleet_entered);
		return(0);
	}
	if ((*ui_fleet_perms[indx])() == NOT_ALLOWED) {			/* does user have permission to work with fleet */
		prt_error(UI_NOT_ALLOWED,"<%s %c>", catgets(UI_m_catd, UI_1, UI_UI_UUD_26, "fleet"), fleet_entered);		/* not allowed */
		return(0);
	}
	return(1);								/* fleet entered is valid and active */
}

/***************************************************************************************/
/* is_zonesup_valid: returns 1 if zone or superzone # entered is valid, else returns 0 */
/***************************************************************************************/
int is_zonesup_valid(zone_num,ptr)
short zone_num;							/* zone/superzone # */
struct new_def_structs *ptr;
{
	int i;
	struct cisam_zn *zone_file_ptr;

	zone_file_ptr = (struct cisam_zn *)req_buf;

	if (zone_num == 0)					/* assume that zone_num = 0 is valid */
		return(1);

	for (i = 0; i < FLEET_MAX; i++) {			/* loop thru for all the zones/superzones selected by the user */
		if (ptr->excpt_fleet[i] == '\0')
			continue;
		zone_file_ptr->fleet =  ptr->excpt_fleet[i];
		zone_file_ptr->nbr = zone_num;
		if ( db(ZONE_FILE_ID,READ_KEYS,&zn_key1,ISEQUAL,0) < 0 ) 
			continue;
		else						/* got record ... valid zone/zuperzone entered */
			return(1);
	}
	return(0);
}

/****************************************************************************************/
/* is_y_n: is entry YES or NO . Returns  1 if YES, NO returns 0, 2 returns *, -1 is none*/
/****************************************************************************************/
int is_y_n(ch)
char ch;							/* char entered */
{
	switch (ch)
	{
	case NO:
		
		return(0);
	case YES:
		return(1);
	case '*':
		return(2);

	default:							/* invalid entry, use NO */
		prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_UUD_27, "<default value NO taken>"));
		return(-1);
	}
}

/********************************************************************************************************/
/* get_group_desc: gets the description of the exception groups from exception groups file on disk 	*/
/*	Returns 	0	if failed to read description records off disk				*/
/*			1	on success								*/
/*******************************************************************************************************/
int get_group_desc(excpt_group_ptr)
struct excpt_groups *excpt_group_ptr;				/* pointer to excpt_group array of structures */
{
	int i;
	struct cisam_eg *eptr;					/* for disk i/o */

	if (db(EXC_GROUP_ID,OPEN,&eg_key1,ISINOUT+ISMANULOCK,0) < 0 ) {		/* open the exception group file */
		prt_error(UI_OPEN,catgets(UI_m_catd, UI_1, UI_UI_UUD_28, "<exception group file>"));
		return(0);							/* failed */
	}

	eptr = (struct cisam_eg *)req_buf;					/* read & save the exception group's description */
	eptr->nbr = 0;								/* start reading from first record on disk */

	if (db(EXC_GROUP_ID,READ_KEYS,&eg_key1,ISGTEQ,0) < 0) {
		prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_UI_UUD_28, "<exception group file>"));			/* close the file */
		if (db(EXC_GROUP_ID,CLOSE,&eg_key1,0,0) < 0) {
			prt_error(UI_CLOSE,catgets(UI_m_catd, UI_1, UI_UI_UUD_28, "<exception group file>"));
			return(0);						/* failed */
		}
		return(0);							/* failed */
	}

	(excpt_group_ptr + eptr->nbr - 1 )->should_process = YES;		/* save info in excpt_group[] struct */
	strcpy((excpt_group_ptr + eptr->nbr - 1 )->group_desc,eptr->name);

	i = 1;									/* read all records from disk */
	while ((db(EXC_GROUP_ID,READ_KEYS,&eg_key1,ISNEXT,0) >= 0) && (i < EXC_GROUPS)) {
		(excpt_group_ptr + eptr->nbr - 1 )->should_process = YES;	/* save info in excpt_group[] struct */
		strcpy((excpt_group_ptr + eptr->nbr - 1 )->group_desc,eptr->name);
		i++;
	}

	if (db(EXC_GROUP_ID,CLOSE,&eg_key1,0,0) < 0) {				/* close the file */
		prt_error(UI_CLOSE,catgets(UI_m_catd, UI_1, UI_UI_UUD_28, "<exception group file>"));
		return(0);							/* failed */
	}
	return(1);								/* success */
}

/****************************************************************************************/
/* init_except_group: intializes the excpt_group[] array of structures 			*/
/****************************************************************************************/
init_except_group(excpt_group_ptr)
struct excpt_groups *excpt_group_ptr;			/* array of pointers to excpt_group structures */
{
	int i,j;

	for (i = 0; i < EXC_GROUPS; i++) {
		(excpt_group_ptr + i)->should_process = NO;			/* should not process this group */
		strcpy((excpt_group_ptr + i)->group_desc," ");
		(excpt_group_ptr + i)->how_many = 0;				/* no exception belong to this group */
		(excpt_group_ptr + i)->excpt_type[0] = 0;
		(excpt_group_ptr + i )->group_y_n = NO;			/* group not selected */
		for (j = 0; j < 12; j++)
			(excpt_group_ptr + i )->type_y_n[j] = NO;		/* group not selected */
	}
}

/********************************************************************************************************/
/* get_except_types: gets and saves the exception types belonging to each of the exception groups 	*/
/********************************************************************************************************/
get_except_types(excpt_group_ptr)
struct excpt_groups *excpt_group_ptr;
{

	int i;					/* scratch variable */
	int j, k;
	int num_groups = 0;
	int type_ary[EXC_TYPES];
	struct excpt_desc *exptr;

	exptr = offset->except_desc;				/* print the exception types description */
	for (i = 0; i < EXC_GROUPS; i++) {
	        if (strlen((excpt_group_ptr+i)->group_desc))
		    {
		      for (k = 0; k < EXC_TYPES; k++)  
			type_ary[k] = -1;
		get_types_from_group(type_ary,i+1);
		j = 0;
		while (type_ary[j] != -1) {			/* save exception types belonging to this group */
			(excpt_group_ptr + i )->excpt_type[j] = type_ary[j];
			mvaddstr(UUD_GROUP1_ROW+num_groups, UUD_DISP_TYPE1_COL+13*j, (exptr+type_ary[j])->short_desc);
			j++;
		}
		(excpt_group_ptr + i )->how_many = j;		/* # of exception types belonging to group # i+1 */
		if ((excpt_group_ptr + i)->how_many > 0)	/* should this excpt group be processed during sel criteria entry */
		 	(excpt_group_ptr + i )->should_process = YES;			/* this group is to be processed */
		      num_groups++;
		    }
	      }
}

/**************************************************************************************************/
/* get_old_types: saves YES/NO for the exception types/groups selected in excpt_group structures */
/**************************************************************************************************/
get_old_types(excpt_group_ptr)
struct excpt_groups *excpt_group_ptr;
{
	int i,j,k;
	int get_out = 0;

	for (i = 0; i < EXC_TYPES; i++) {
		if (test_bit(i,sizeof(struct except_bits),(unsigned char *)&selected_except)) {
			for (j = 0; j < EXC_GROUPS; j++) {
				for (k=0; k < (excpt_group_ptr + j )->how_many; k++) {	/* this group has some exception types */
					(excpt_group_ptr + j)->should_process = YES;
					if ((excpt_group_ptr + j)->excpt_type[k] == i) {
						(excpt_group_ptr + j)->type_y_n[k] = YES;
						(excpt_group_ptr + j)->group_y_n = YES;
						get_out = 1;
						break;
					}
				}
				if (get_out) {
					get_out = 0;
					break;
				}
			}
		}
	}
}

/**********************************************************************************************************/
/* type_selected_in_group: checks whether the type specified by type+1 (argument 1) has been selected     */
/* by the user or not. Returns exception group # in which the selected exception belongs; else if         */
/* the exception is not selected by the user, then the function returns 0				  */
/**********************************************************************************************************/
type_selected_in_group(type,excpt_group_ptr)
int type;						/* exception # type ... starting from 0 */
struct excpt_groups *excpt_group_ptr;
{
	int group_num, j;

	for (group_num = 0; group_num < EXC_GROUPS; group_num++) {
		for (j =0; j < 12; j++) {
			if ((excpt_group_ptr + group_num)->excpt_type[j] == type) 
				if ((excpt_group_ptr + group_num)->type_y_n[j] == YES)	/* user has selected this exception type */
					return(group_num+1);
		}
	}
	return(0);
}
