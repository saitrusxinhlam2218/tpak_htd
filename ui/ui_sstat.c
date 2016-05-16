static char sz__FILE__[]="@(#)ui_sstat.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  ui_sstat.c; Rel: 6.9.0.0; Get date: 10/4/90 at 11:33:14
* @(#)  Last delta: 9/26/90 at 17:37:42
* @(#)  SCCS File: /taxi/sccs/s.ui_sstat.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cursesX.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "taxipak.h"
#include "mem_mgmt.h"
#include "Object.h"
#include "mads_types.h"
#include "mad_ipc.h"
#include "acc.h"
#include "veh_driv.h"
#include "mads_isam.h"
#include "mad_error.h"
#include "call_entry.h"
#include "df_maint.h"
#include "ex_key.h"
#include "func_keys.h"
#include "screens.h"
#include "ui_def.h"
#include "ui_error.h"
#include "ui_ipc.h"
#include "ui_stat.h"
#include "user.h"
#include "menu.h"
#include "language.h"
#include "ui_strdefs.h"
#include "GPS_private.h"
#include "switch_enh.h"
#include "Exception_private.h"

extern int scr_name;						/* name of screen on display */
extern struct fleets *fleet[];					/* array of pointers to fleets structure in shared memory */
extern struct users *uiuser[];					/* array of pointers to users structure in shared memory */
extern struct menus *cmenu[];					/* array of pointers to menus structure in shared memory */
extern int map_array[];						/* to map scr_name to index into menus structures */
extern int user_index;						/* index into users structure in shared memory */
extern int uid;							/* users id # */
extern char scr_type;						/* current screen type */
extern int scr_name;						/* current screen name */
extern short end_syst_status;					/* flag to end system status display */
extern int msgkey;						/* qid for ipc */
extern int pid;							/* process id */
extern short clear_err_flag;					/* error flag when error message displayed */
extern char req_buf[];
extern struct veh_index *veh_idx[ ];
extern int gps_pipe_fd;
extern char sGPS_host[];
extern long glEnhSwitch; 
extern int fifo_fd;                                    /* file descriptor for opening FIFO */
struct scr_flds syst_status_scr_flds[] = {			/* screen field structure for user input fields */
	{0,0,1,1},
	{S_FLEET_ROW,S_FLEET_COL,CHARS_FLEET,S_VEH},
	{S_VEH_ROW,S_VEH_COL,CHARS_VEH,S_DRIV},
	{S_DRIV_ROW,S_DRIV_COL,CHARS_DRIV,S_FLEET},
};
extern int def_fl_index;					/* index into fleet array for user's default fleet */

/* declare functions for checking fleet permissions */
extern int ui_fleet_perms0(), ui_fleet_perms1(), ui_fleet_perms2(), 
	ui_fleet_perms3(), ui_fleet_perms4(), ui_fleet_perms5(), 
	ui_fleet_perms6(), ui_fleet_perms7();

/* declare array of pointers to functions for checking fleet perm's */
extern int (*ui_fleet_perms[])();
extern struct offsets *offset;

int ss_chosen_fleet = -1;					/* currently chosen fleet */
char ss_ch_fleet = '\0';					/* currently chosen fleet id */
int ss_chosen_veh = -1;						/* currently chosen vehicle */
int ss_chosen_driv = -1;					/* currently chosen driver */
short ss_choice = 0;						/* 0=no choice made, 1=vehicle entered, 2=driver entered */
char drv_name[29];						/* driver name */
extern int iserrno;
extern int  LastVehUsed;
short SystemStatusView;
#define VEHICLE_VIEW    0
#define ATTRIBUTES_VIEW 1

extern  int GPS_guy;
extern char *get_asc_time();

/***********************************************************************************************************************************
system_status -
This function is called from the User Interface process when a user selects the System Status Display screen.  The routine formats
the screen for the system status.  The bottom half of the screen is used to display the fleet-wide summary information obtained
from shared memory.  The top half of the screen is used to display specific vehicle/driver information obtained through a request
to Dispatch.
***********************************************************************************************************************************/
system_status()
{
    int old_time = -1;						/* last time of screen update */
    int spiu = S_VEH;						/* field cursor is to start on */
    int update_screen_rate_ms;					/* How often, in millisec, to update status screen */

#ifdef DEBUG
	sprintf(trace_str,"system_status\n");
	TRACE(trace_str);
#endif

    ss_chosen_fleet = -1;						/* currently chosen fleet */
    ss_ch_fleet = '\0';							/* currently chosen fleet id */
    ss_chosen_veh = -1;							/* currently chosen vehicle */
    ss_chosen_driv = -1;						/* currently chosen driver */
    end_syst_status = 0;						/* flag to end system status display */

    disp_syst_status_scr();						/* output system status display headers on screen */
    SystemStatusView = VEHICLE_VIEW;
    prt_entry_areas(stdscr,syst_status_scr_flds,MAX_PIU_SYST);		/* fill in user entry areas with underscores */
    if ( ss_def_fleet()	== -1 )						/* set up default fleet */
	/* no default fleet is specified , so go fleet field */
	spiu = S_FLEET;

    if (ss_chosen_fleet >= 0)
      {
	update_screen_rate_ms = fleet[ss_chosen_fleet]->zone_status_update_freq * 1000;
	
	/* Rate must not be less than 10 seconds long */
	if (update_screen_rate_ms < 10000)
	  update_screen_rate_ms = 10000;
      }

    /* Default to 10 sec */
    else
      update_screen_rate_ms = 10000;
	  
    while ((!end_syst_status) && (scr_name == SYSTEM_STATUS_SCR)) {	/* repeat until user decides to end it */
      if (SystemStatusView == VEHICLE_VIEW)
	disp_syst();
	ss_read_inpt(&spiu, update_screen_rate_ms);			/* read input from user */
    }

    return(0);								/* return to caller in User Interface */
}



/***********************************************************************************************************************************
disp_syst_status_scr -
This function displays the headers for the system status display screen.  The reverse attributes are set for the top portion of the
screen.  The date and user id number are also displayed on the top line.
***********************************************************************************************************************************/
disp_syst_status_scr()
{
    int i, len, start_col, indx;				/* temporary variable for loops */

#ifdef DEBUG
	sprintf(trace_str,"disp_syst_status\n");
	TRACE(trace_str);
#endif

    clear();							/* clear screen */
    attrset(A_REVERSE | A_BOLD);				/* set bold reverse for background */
    for(i=1; i<=5; i++)						/* for lines 1-5, set reverse and bold */
	mvaddstr(i,0,"                                                                                                                                    ");
    attrset(A_REVERSE);						/* set dim reverse for line 0 */
    mvaddstr(0,0,"                                                                                                                                    ");
    prt_date();							/* current date */

    indx = map_array[scr_name];	
    len = strlen(cmenu[indx]->mf_desc); 			/* print the title of the screen */
    start_col = (COLS - len - 1 )/2;
    mvprintw(0,start_col,"%s", cmenu[indx]->mf_desc);
    mvprintw(0,start_col+len+3,"(%d)", cmenu[indx]->mf_command);
    mvprintw(0,UID_COL,"%d",uid);
    mvaddstr(1,0,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_1, " Fleet  "));
    mvaddstr(1,11,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_2, "   Vehicle # "));
    mvaddstr(1,30,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_3, "  Driver ID "));
    mvaddstr(1,57,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_8, " Current Status "));
    mvaddstr(2, 57, catgets(UI_m_catd, UI_1, UI_UI_SSTAT_78, " Taximeter "));
    mvaddstr(1,SS_DET_TEXT4,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_4, " Short Meter "));
    mvaddstr(1,SS_DET_TEXT5,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_5, " Req to Talk "));
    mvaddstr(2,0,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_6, " MID  "));
    mvaddstr(2,13,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_7, " Driver Name "));
    mvaddstr(2,SS_DET_TEXT4,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_9, " Late Meter  "));
    mvaddstr(2,SS_DET_TEXT5,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_10, " Messages    "));
    mvaddstr(3,0,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_11, " Veh Attributes "));
    mvaddstr(3,SS_DET_TEXT1,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_12, " Signed on "));
    mvaddstr(3,SS_DET_TEXT2,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_13, " Calls "));
    mvaddstr(3,SS_DET_TEXT3,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_14, " Book-Ins   "));
    mvaddstr(3,SS_DET_TEXT4,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_15, " No-Accepts  "));
    mvaddstr(3,SS_DET_TEXT5,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_16, " Callbacks   "));
    mvaddstr(4,0,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_17, " Drv Attributes "));
    mvaddstr(4,SS_DET_TEXT1,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_18, " Signed of "));
    mvaddstr(4,SS_DET_TEXT2,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_19, " Flags "));
    mvaddstr(4,SS_DET_TEXT3,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_20, " Cond Books "));
    mvaddstr(4,SS_DET_TEXT4,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_21, " Rejects     "));
    mvaddstr(4,SS_DET_TEXT5,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_22, " Callouts    "));
    mvaddstr(5,0,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_23, " Emergency On "));
    mvaddstr(5,23,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_24, " Off "));
    mvaddstr(5,SS_DET_TEXT1,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_25, " Assigns   "));
    mvaddstr(5,SS_DET_TEXT2,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_26, " Bids  "));
    mvaddstr(5,SS_DET_TEXT3,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_27, " Enr. Books "));
    mvaddstr(5,SS_DET_TEXT4,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_28, " Breaks      "));
    mvaddstr(5,SS_DET_TEXT5,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_29, " No-Trips    "));

    attrset(0);							/* set normal attribute */
    mvaddstr(6,0,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_30, "--------------------------------------------------F L E E T W I D E   S U M M A R Y-------------------------------------------------"));
                                                          
    mvaddstr(7,0,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_31, "              Now   Today                    Now   Today   "));
    mvaddstr(8,SS_SUM_TEXT1,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_32, "Emergencies"));
    mvaddstr(9,SS_SUM_TEXT1,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_33, "Signed On"));
    mvaddstr(10,SS_SUM_TEXT1,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_34, "Signed Off"));
    mvaddstr(11,SS_SUM_TEXT1,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_35, "Flags"));
    mvaddstr(12,SS_SUM_TEXT1,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_36, "Bid Calls"));
    mvaddstr(13,SS_SUM_TEXT1,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_37, "Books"));
    mvaddstr(14,SS_SUM_TEXT1,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_38, "Cond Books"));
    mvaddstr(15,SS_SUM_TEXT1,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_39, "Enr. Books"));
    mvaddstr(16,SS_SUM_TEXT1,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_40, "Short Meter"));
    mvaddstr(17,SS_SUM_TEXT1,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_41, "Late Meter"));
    mvaddstr(18,SS_SUM_TEXT1,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_42, "Wake_ups"));
    mvaddstr(8,SS_SUM_TEXT2,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_43, "No-Accepts"));
    mvaddstr(9,SS_SUM_TEXT2,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_44, "Rejects"));
    mvaddstr(10,SS_SUM_TEXT2,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_45, "Breaks"));
    mvaddstr(11,SS_SUM_TEXT2,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_46, "Req to Talk"));
    mvaddstr(12,SS_SUM_TEXT2,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_47, "Messages"));
    mvaddstr(13,SS_SUM_TEXT2,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_48, "Callbacks"));
    mvaddstr(14,SS_SUM_TEXT2,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_49, "Callouts"));
    mvaddstr(15,SS_SUM_TEXT2,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_50, "No-Trips"));
/*    mvaddstr(16,SS_SUM_TEXT2,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_51, "Idle")); Not supported yet */
/*    mvaddstr(17,SS_SUM_TEXT2,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_52, "Suspended")); Not supported yet */

    if (0)
      mvaddstr(17, SS_SUM_TEXT2, "Avbest");

    mvaddstr(18,SS_SUM_TEXT2,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_53, "Calls"));

    for(i=1; i<=MAX_PIU_SYST; i++) {				/* leave normal space on either side of user entered fields */
	mvaddstr(syst_status_scr_flds[i].prompt_row,syst_status_scr_flds[i].prompt_col-1," ");
	mvaddstr(syst_status_scr_flds[i].prompt_row,syst_status_scr_flds[i].prompt_col+syst_status_scr_flds[i].max_chars," ");
    }

    return(0);
}


/***********************************************************************************************************************************
ss_read_inpt -
This function waits for the user to enter a field and then processes the request.  The user may switch fields by tabbing or using
arrow keys, or enter a new field or function key.
***********************************************************************************************************************************/
ss_read_inpt(spiu, update_screen_rate_ms)
int *spiu;							/* piu of field user is currently on */
int update_screen_rate_ms;
{
    char buf[READ_BUF_SIZE];					/* buffer to store users text */
    int key;							/* key set if user actually entered something */
    int i,j,x;							/* temporary variables for loops and values */
    char c;							/* used for character stripped of attribute values */
    int should_clear = 0;

#ifdef DEBUG
	sprintf(trace_str,"ss_read_inpt - *spiu=%d\n",*spiu);
	TRACE(trace_str);
#endif
    /* Put cursor at input line */
    move((syst_status_scr_flds + *spiu)->prompt_row, (syst_status_scr_flds + *spiu)->prompt_col);
    refresh();

    /* Wait for a character */
    if ((c = wgetsh_or_wait(stdscr, update_screen_rate_ms)) == -1) {
	/* If no char, return to update screen */
	return(0);
    }

    /* Put the char back in buffer so read_field can read it */
    ungetsh(c);

    i = read_field(stdscr,syst_status_scr_flds,*spiu,buf,&key,&should_clear);		/* read field from user */
    if ((clear_err_flag) && (i != -1))						/* is there an error mesg displayed? */
	clear_error();								/* yes, clear it */
    switch(i) {
	case KEY_TAB:
	case KEY_DOWN:								/* user wants to change fields forward */
			*spiu = syst_status_scr_flds[*spiu].tab_field;		/* get new field piu */
			break;

	case KEY_UP:								/* user wants to change fields backward */
			(*spiu) -= 1;						/* go back one field */
			if (*spiu < 1)						/* too far? */
			    *spiu = MAX_PIU_SYST;				/* yes, go to last field */
			break;

        case PREV_PG_KEY:
			if ( SystemStatusView == ATTRIBUTES_VIEW )
			  {
			    SystemStatusView = VEHICLE_VIEW;
			    disp_syst_status_scr();
			    disp_syst();
			  }
			break;
        case NEXT_PG_KEY:
			if ( SystemStatusView == VEHICLE_VIEW )
			  {
			    SystemStatusView = ATTRIBUTES_VIEW;
			    disp_attr_sum();
			  }
			break;

		      case ALTERNATE_KEY:
			if ( !(1) )
			  prt_error(UI_NOFIELD, catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1, "<wrong function key pressed>"));
			else if ( ss_chosen_veh > 0 || ss_chosen_driv > 0 )
			  DoVehDrivDetail();
			else
			  prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_65, "<No Driver Found >"));			  
			break;

		      case 6:
			if ( GPS_guy )
			  GPS_zoom( MAP_APP_RQST_TYPE_ZOOM_IN );
			break;

		      case 7:
			if ( GPS_guy )
			  GPS_zoom( MAP_APP_RQST_TYPE_ZOOM_OUT );
			break;

	case 0:
	case '\r': 
	case '\n':								/* user has entered something into field */
			if (key == 0) {						/* was anything actually entered? */
			    for(j=0;j<syst_status_scr_flds[*spiu].max_chars;j++) {	/* no, use wat was already there */
				wmove(stdscr,syst_status_scr_flds[*spiu].prompt_row,syst_status_scr_flds[*spiu].prompt_col+j);
				c = winch(stdscr);				/* get next character */
				c = c & 0x7f;					/* strip off all attributes */
				buf[j] = c;					/* save in buffer */
			    }
			    buf[j] = '\0';					/* end with null */
			}

			x = atoi(buf);						/* get number value */
			if (*spiu==S_FLEET) {					/* is this a new fleet field? */
			    if (ss_new_fleet(buf[0]) > 0) {			/* yes, set new current fleet */
				ss_chosen_veh = -1;
				ss_chosen_driv = -1;
				ss_choice = 0;
				if ( SystemStatusView == VEHICLE_VIEW )
				  disp_syst();					/* display summary for this fleet */
				attrset(A_UNDERLINE);				/* underline */
    				mvprintw(S_VEH_ROW,S_VEH_COL,"    ");		/* and vehicle number */
    				mvprintw(S_DRIV_ROW,S_DRIV_COL,"     ");		/* and driver */
				attrset(0);					/* normal */
				clear_vh_drv();					/* clear vehicle/driver detail */
				*spiu = syst_status_scr_flds[*spiu].tab_field;	/* get new field piu */
			    }
			}
			else if (*spiu==S_VEH) {				/* is this new vehicle field? */
			    if (new_veh(x,buf) == 0)					/* set new vehicle */
				disp_vh_drv();					/* display detail of vehicle */
			    if ( SystemStatusView == VEHICLE_VIEW )
			      disp_syst();					/* display summary for this fleet */
			}
			else if (*spiu==S_DRIV) {				/* is this new driver field? */
			    if (new_driv(x,buf) == 0)					/* set new driver */
				disp_vh_drv();					/* display detail of driver */
			    if ( SystemStatusView == VEHICLE_VIEW )
			      disp_syst();					/* display summary for this fleet */
			}
			else
			    *spiu = S_FLEET;					/* set spiu to fleet field */
			break;

	case -1:								/* user did not enter anything */
			break;

	default:
			ftnkey_handler(spiu,i,0);					/* user entered function key */
			break;
    }
	return(0);
}


/***********************************************************************************************************************************
disp_syst -
This function displays the fleet summary on the bottom of the screen.  The information is taken from the shared memory area for
the fleet specified in chosen_fleet.
***********************************************************************************************************************************/
disp_syst()
{
	int i;								/* temporary variable for loops */

#ifdef DEBUG
	sprintf(trace_str,"disp_syst\n");
	TRACE(trace_str);
#endif

	if (ss_chosen_fleet < 0)						/* has a fleet been chosen? */
		return(0);							/* no */

	if (fleet[ss_chosen_fleet]->now_emergencies > 0)
		mvprintw(8,SS_SUM_INFO1,"%4d",fleet[ss_chosen_fleet]->now_emergencies);
	else
		mvprintw(8,SS_SUM_INFO1,"    ");

	if (fleet[ss_chosen_fleet]->emergencies > 0)
		mvprintw(8,SS_SUM_INFO2,"%5d",fleet[ss_chosen_fleet]->emergencies);
	else 
		mvprintw(8,SS_SUM_INFO2,"     ");

	if (fleet[ss_chosen_fleet]->now_signed_on > 0)
		mvprintw(9,SS_SUM_INFO1,"%4d",fleet[ss_chosen_fleet]->now_signed_on);
	else
		mvprintw(9,SS_SUM_INFO1,"    ");

	if (fleet[ss_chosen_fleet]->signed_on > 0)
		mvprintw(9,SS_SUM_INFO2,"%5d",fleet[ss_chosen_fleet]->signed_on);
	else
		mvprintw(9,SS_SUM_INFO2,"     ");

	mvprintw(10,SS_SUM_INFO1,"    ");

	if (fleet[ss_chosen_fleet]->signed_off > 0)
		mvprintw(10,SS_SUM_INFO2,"%5d",fleet[ss_chosen_fleet]->signed_off);
	else
		mvprintw(10,SS_SUM_INFO2,"     ");

	mvprintw(11,SS_SUM_INFO1,"    ");

	if (fleet[ss_chosen_fleet]->flags > 0)
		mvprintw(11,SS_SUM_INFO2,"%5d",fleet[ss_chosen_fleet]->flags);
	else
		mvprintw(11,SS_SUM_INFO2,"     ");

	mvprintw(12,SS_SUM_INFO1,"    ");

	if (fleet[ss_chosen_fleet]->now_bids > 0)
		mvprintw(12,SS_SUM_INFO1,"%5d",fleet[ss_chosen_fleet]->now_bids);
	else
		mvprintw(12,SS_SUM_INFO1,"     ");

	if (fleet[ss_chosen_fleet]->bids > 0)
		mvprintw(12,SS_SUM_INFO2,"%5d",fleet[ss_chosen_fleet]->bids);
	else
		mvprintw(12,SS_SUM_INFO2,"     ");

	if(fleet[ss_chosen_fleet]->now_posts > 0)
		mvprintw(13,SS_SUM_INFO1, "%4d", fleet[ss_chosen_fleet]->now_posts);
	else
		mvprintw(13,SS_SUM_INFO1,"    ");

	if (fleet[ss_chosen_fleet]->cond_posts > 0)
		mvprintw(14,SS_SUM_INFO2,"%5d",fleet[ss_chosen_fleet]->cond_posts);
	else
		mvprintw(14,SS_SUM_INFO2,"     ");

	if (fleet[ss_chosen_fleet]->now_enroute_posts > 0)
		mvprintw(15,SS_SUM_INFO1,"%4d",fleet[ss_chosen_fleet]->now_enroute_posts);
	else
		mvprintw(15,SS_SUM_INFO1,"    ");

	if (fleet[ss_chosen_fleet]->enroute_posts > 0)
		mvprintw(15,SS_SUM_INFO2,"%5d",fleet[ss_chosen_fleet]->enroute_posts);
	else
		mvprintw(15,SS_SUM_INFO2,"     ");

	if (fleet[ss_chosen_fleet]->short_meters > 0)
		mvprintw(16,SS_SUM_INFO2,"%5d",fleet[ss_chosen_fleet]->short_meters);
	else
		mvprintw(16,SS_SUM_INFO2,"     ");

	mvprintw(17,SS_SUM_INFO1,"    ");

	if (fleet[ss_chosen_fleet]->late_meters > 0)
		mvprintw(17,SS_SUM_INFO2,"%5d",fleet[ss_chosen_fleet]->late_meters);
	else
		mvprintw(17,SS_SUM_INFO2,"     ");

	if (fleet[ss_chosen_fleet]->no_accepts > 0)
		mvprintw(8,SS_SUM_INFO4,"%5d",fleet[ss_chosen_fleet]->no_accepts);
	else
		mvprintw(8,SS_SUM_INFO4,"     ");

	if (fleet[ss_chosen_fleet]->rejects > 0)
		mvprintw(9,SS_SUM_INFO4,"%5d",fleet[ss_chosen_fleet]->rejects);
	else
		mvprintw(9,SS_SUM_INFO4,"     ");

	mvprintw(10,SS_SUM_INFO3,"    ");

	if (fleet[ss_chosen_fleet]->breaks > 0)
		mvprintw(10,SS_SUM_INFO4,"%5d",fleet[ss_chosen_fleet]->breaks);
	else
		mvprintw(10,SS_SUM_INFO4,"     ");

	mvprintw(11,SS_SUM_INFO3,"    ");

	if (fleet[ss_chosen_fleet]->req_to_talk > 0)
		mvprintw(11,SS_SUM_INFO4,"%5d",fleet[ss_chosen_fleet]->req_to_talk);
	else
		mvprintw(11,SS_SUM_INFO4,"     ");

	mvprintw(12,SS_SUM_INFO3,"    ");

	if (fleet[ss_chosen_fleet]->messages > 0)
		mvprintw(12,SS_SUM_INFO4,"%5d",fleet[ss_chosen_fleet]->messages);
	else
		mvprintw(12,SS_SUM_INFO4,"     ");

	if(fleet[ss_chosen_fleet]->now_callbacks > 0)
		mvprintw(13,SS_SUM_INFO3,"%5d",fleet[ss_chosen_fleet]->now_callbacks);
	else
		mvprintw(13,SS_SUM_INFO3,"    ");

	if (fleet[ss_chosen_fleet]->callbacks > 0)
		mvprintw(13,SS_SUM_INFO4,"%5d",fleet[ss_chosen_fleet]->callbacks);
	else
		mvprintw(13,SS_SUM_INFO4,"     ");

	mvprintw(14,SS_SUM_INFO3,"    ");

	if (fleet[ss_chosen_fleet]->callouts > 0)
		mvprintw(14,SS_SUM_INFO4,"%5d",fleet[ss_chosen_fleet]->callouts);
	else
		mvprintw(14,SS_SUM_INFO4,"     ");

	mvprintw(15,SS_SUM_INFO3,"    ");

	if (fleet[ss_chosen_fleet]->no_shows > 0)
		mvprintw(15,SS_SUM_INFO4,"%5d",fleet[ss_chosen_fleet]->no_shows);
	else
		mvprintw(15,SS_SUM_INFO4,"     ");

	mvprintw(16,SS_SUM_INFO3,"    ");

	mvprintw(17,SS_SUM_INFO3,"    ");

	mvprintw(18,SS_SUM_INFO3,"    ");

        if (0)
          {
            if ( fleet[ss_chosen_fleet]->cancels > 0 )
              mvprintw(17, SS_SUM_INFO4, "%5d", fleet[ss_chosen_fleet]->cancels);
            else
              mvprintw(17, SS_SUM_INFO4, "     ");
          }

	
	if (fleet[ss_chosen_fleet]->calls > 0)
		mvprintw(18,SS_SUM_INFO4,"%5d",fleet[ss_chosen_fleet]->calls);
	else
		mvprintw(18,SS_SUM_INFO4,"     ");

	if (fleet[ss_chosen_fleet]->wakeups > 0)
		mvprintw(18,SS_SUM_INFO2,"%5d",fleet[ss_chosen_fleet]->wakeups);
	else
		mvprintw(18,SS_SUM_INFO2,"     ");

	return(0);
}

disp_attr_sum()
{
  int i;
  short veh_col, drv_col, veh_row, drv_row;

  for ( i = 7; i <=24; i++)
    	mvaddstr(i,0,"                                                                                                                                    ");
  mvaddstr( 6, 0, catgets(UI_m_catd, UI_1, UI_UI_SSTAT_79, "-------- B I L A T T R I B U T       ------------------------------------ F | R A R E  A T T R I B U T     -------------" ));
  mvaddstr( 7, 0, catgets(UI_m_catd, UI_1, UI_UI_SSTAT_80, "                         Now   Today                          Now   Today                           Now                   Now" ));
  veh_col = SS_SUM_TEXT1;
  drv_col = SS_SUM_INFO6 - 6;
  veh_row = 8;
  drv_row = 8;
  
  for(i=0; i<32; i++) {
		if ( strlen(fleet[ss_chosen_fleet]->vd_attr_info[i].veh_attr) )
		  {
		    mvprintw(veh_row,veh_col,"%2.2s-%-17.17s   %4d  %5d",
			     fleet[ss_chosen_fleet]->vd_attr_info[i].veh_attr,
			     fleet[ss_chosen_fleet]->vd_attr_info[i].veh_at_desc,
			     fleet[ss_chosen_fleet]->now_veh_attr[i],
			     fleet[ss_chosen_fleet]->fl_veh_attr[i]);
		    ++veh_row;
		  }

		if ( strlen(fleet[ss_chosen_fleet]->vd_attr_info[i].drv_attr) )
		  {
		    mvprintw(drv_row,drv_col,"%2.2s-%-17.17s   %4d",
			     fleet[ss_chosen_fleet]->vd_attr_info[i].drv_attr,
			     fleet[ss_chosen_fleet]->vd_attr_info[i].drv_at_desc,
			     fleet[ss_chosen_fleet]->now_driv_attr[i]);

		    ++drv_row;
		  }
		if ( veh_row > 23 )
		  {
		    veh_row = 8;
		    veh_col = SS_SUM_TEXT2 + 6;
		  }
		if ( drv_row > 23 )
		  {
		    drv_row = 8;
		    drv_col = SS_SUM_INFO7 + 6;
		  }
	      }
}
/***********************************************************************************************************************************
disp_vh_drv -
This function displays the detailed veh/driv information based on the info supplied by the user.  An ipc message is sent to the
Dispatch process to request the detail on the specified vehicle.  A read is hung waiting for a reply to the request.
NOTE:
    If Dispatch dies or is slow in responding, this process will get stuck here waiting for a reply.
***********************************************************************************************************************************/
disp_vh_drv()
{
    int i;								/* temporary variables for loops, etc. */
    unsigned char mask;							/* mask for looping through bit structures */
    unsigned char *vbpt;						/* pointer for looping through bit structures */
    unsigned char *dbpt;						/* pointer for looping through bit structures */
    unsigned char *tbpt;						/* pointer for looping through bit structures */
    char st_buf[64];							/* buffer for storing current taxi status text */
    char st_buf2[16];
    char stp[8];								/* pointer for filling in buffer */
    char dt_buf[10];							/* date buffer for conversion */
    char tm_buf[7];							/* time buffer for conversion */
    struct veh_rqst vrq;						/* structure for requesting vehicle detail info */
    VEH_RESP_REC *vrp;					/* structure for vehicle detail responses */
    VEH_RESP_HNDL  hVehResp = NULL;
    struct cisam_dr *drp;						/* pointer to driver's C-Isam record */
    int  rc, times_thru;
    char error_str[80];
    short veh_col, drv_col;
    struct veh_driv *veh_ptr;

#ifdef DEBUG
	sprintf(trace_str,"disp_vh_drv\n");
	TRACE(trace_str);
#endif

    clear_vh_drv();							/* clear detail area */
    
    if(ss_chosen_fleet < 0)
      return;								/* invalid fleet chosen */

    VEH_RESP_REC vehresp_rec;
    bzero(&vehresp_rec, sizeof(VEH_RESP_REC));
    
    if(ss_chosen_veh > 0)
      {						/* vehicle # exists for this driver ? */

	vrq.d_prcs_id = DISP_DEST;	
	vrq.u_prcs_id = pid;	
	vrq.rec_type = VEH_REQ;
	vrq.fleet = ss_ch_fleet;
	vrq.veh_nbr = ss_chosen_veh;	

	    	msgsnd(msgkey,
	       (struct msgbuf *)&vrq,sizeof(struct veh_rqst), IPC_NOWAIT);
		//		hVehResp = malloc( sizeof(VEH_RESP_REC));
		vrp = ( VEH_RESP_REC * )&vehresp_rec;
	
	times_thru = 0;
	while (times_thru <= 2) 
	  {
	    rc = msgrcv(msgkey,
			(void *)vrp,sizeof(VEH_RESP_REC),(long)pid,0);
	    
	    if (rc == -1) 
	      {
		if (errno != EINTR) 
		  {
		    prt_error(UI_FATAL, "%s", 
			      catgets(UI_m_catd, UI_1, UI_UI_SSTAT_74, 
				      "Error reading from dispatch in msgrcv"));
		    sprintf(error_str,
			    "Error %d reading ipc message from DISPATCH", errno);
		    ERROR(' ',"User-IPC", error_str);
		    if (hVehResp)
		      //		      free( hVehResp );
		    return(0);
		  }
		if (++times_thru == 2) 
		  {
		    prt_error(UI_SYSTEM_DOWN, "");
		    ERROR(' ',"User-IPC", 
			  "Timeout reading ipc message from DISPATCH");
		    if (hVehResp)
		      //		      free( hVehResp );
		    return(0);
		  }
	      }
	    else 
	      if (vrp->rec_type == VEH_REQ)
		break;
	      else 
		{
		  prt_error(UI_FATAL, "%s", 
			    catgets(UI_m_catd, UI_1, UI_UI_SSTAT_74, 
				    "Error reading from dispatch in msgrcv"));
		  ERROR(' ',"User-IPC", "Invalid msg from dispatch DISPATCH");
		  if (hVehResp);
		  //		  free( hVehResp );
		  return(0);
	    	}
	  }	    


#ifdef FOO
        if ( vrp->drv_id == 0 )
	  {
	    ss_choice = 2;
	  }
#endif
	
	if(ss_choice == 1 || (ss_choice == 2 && (ss_chosen_veh == vrp->veh_nbr) && (ss_chosen_driv == vrp->drv_id))){
	  attrset(A_REVERSE | A_BOLD);
        mvprintw(2,7,"%-5d",strtol(vrp->rf_id,(char **)NULL, 16));
	
    	vrp->short_meter ? mvprintw(1,SS_DET_INFO4,"%3d",vrp->short_meter) : mvprintw(1,SS_DET_INFO4,"   ");
    	vrp->late_meter ? mvprintw(2,SS_DET_INFO4,"%3d",vrp->late_meter) : mvprintw(2,SS_DET_INFO4,"   ");
    	vrp->no_accepts ? mvprintw(3,SS_DET_INFO4,"%3d",vrp->no_accepts) : mvprintw(3,SS_DET_INFO4,"   ");
    	vrp->rejects ? mvprintw(4,SS_DET_INFO4,"%3d",vrp->rejects) : mvprintw(4,SS_DET_INFO4,"   ");
    	vrp->breaks ? mvprintw(5,SS_DET_INFO4,"%3d",vrp->breaks) : mvprintw(5,SS_DET_INFO4,"   ");
    	vrp->req_to_talk ? mvprintw(1,SS_DET_INFO5,"%3d",vrp->req_to_talk) : mvprintw(1,SS_DET_INFO5,"   ");
    	vrp->messages ? mvprintw(2,SS_DET_INFO5,"%3d",vrp->messages) : mvprintw(2,SS_DET_INFO5,"   ");
    	vrp->callbacks ? mvprintw(3,SS_DET_INFO5,"%3d",vrp->callbacks) : mvprintw(3,SS_DET_INFO5,"   ");
    	vrp->callouts ? mvprintw(4,SS_DET_INFO5,"%3d",vrp->callouts) : mvprintw(4,SS_DET_INFO5,"   ");
    	vrp->no_shows ? mvprintw(5,SS_DET_INFO5,"%3d",vrp->no_shows) : mvprintw(5,SS_DET_INFO5,"   ");
    	vrp->calls ? mvprintw(3,SS_DET_INFO2,"%3d",vrp->calls) : mvprintw(3,SS_DET_INFO2,"   ");
    	vrp->flags ? mvprintw(4,SS_DET_INFO2,"%3d",vrp->flags) : mvprintw(4,SS_DET_INFO2,"   ");
    	vrp->bids ? mvprintw(5,SS_DET_INFO2,"%3d",vrp->bids) : mvprintw(5,SS_DET_INFO2,"   ");
	vrp->posts ? mvprintw(3,SS_DET_INFO3,"%3d",vrp->posts) : mvprintw(3,SS_DET_INFO3,"   ");
    	vrp->cond_posts ? mvprintw(4,SS_DET_INFO3,"%3d",vrp->cond_posts) : mvprintw(4,SS_DET_INFO3,"   ");
    	vrp->enroute_posts ? mvprintw(5,SS_DET_INFO3,"%3d",vrp->enroute_posts) : mvprintw(5,SS_DET_INFO3,"   ");
        vrp->emerg_on_time ? get_asc_time(vrp->emerg_on_time,dt_buf,tm_buf) : strncpy(tm_buf,"",7);
        if ( vrp->emerg_on_time != 0 )
          get_asc_time(vrp->emerg_on_time,dt_buf,tm_buf);
        else
          strncpy(tm_buf,"",7);
        
    	mvprintw(5,15,"%6.6s",tm_buf);
        
        vrp->emerg_off_time ? get_asc_time(vrp->emerg_off_time,dt_buf,tm_buf) : strncpy(tm_buf,"",7);

    	mvprintw(5,29,"%6.6s",tm_buf);
    	vrp->sign_on_dt_tm ? get_asc_time(vrp->sign_on_dt_tm,dt_buf,tm_buf) : strncpy(tm_buf,"",7);
    	mvprintw(3,SS_DET_INFO1,"%6.6s",tm_buf);
    	vrp->sign_off_dt_tm ? get_asc_time(vrp->sign_off_dt_tm,dt_buf,tm_buf) : strncpy(tm_buf,"",7);
    	mvprintw(4,SS_DET_INFO1,"%6.6s",tm_buf);
    	vrp->assigns ? mvprintw(5,SS_DET_INFO1,"%d",vrp->assigns) : mvprintw(5,SS_DET_INFO1,"   ");
		/* only display driver's name if there is a driver assoc. with the vehicle */
		vrp->drv_id ? mvprintw(2,27,"%-28s",drv_name) : mvprintw(2,27,"                            ");

	drv_col = 17;
	veh_col = 17;
    	mask = 0x80;
    	vbpt = (unsigned char *)&vrp->veh_attr;
    	dbpt = (unsigned char *)&vrp->drv_attr;
    	for(i = 0; i < 32; i++) { /* MRB */
		if (*vbpt & mask)
		  {
		    if ( veh_col < SS_DET_TEXT1 )
		      mvprintw(3,veh_col,"%.2s ",fleet[ss_chosen_fleet]->vd_attr_info[i].veh_attr);
		    veh_col += 3;
		  }
		if (*dbpt & mask)
		  {
		    if ( drv_col < SS_DET_TEXT1 )
		      mvprintw(4,drv_col,"%.2s ",fleet[ss_chosen_fleet]->vd_attr_info[i].drv_attr);
		    drv_col += 3;
		  }
		if (( mask >>= 1 ) == 0x00 )
		  {
		    mask = 0x80;
		    ++dbpt;
		    ++vbpt;
		  }
    	}
	
    	tbpt = (unsigned char *)&vrp->cur_status;
    	memset(st_buf,0,64);
	memset(stp,0,8);
	
    	if (vrp->cur_status.emer) 
	  strcat(st_buf,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_57, "DISAB/"));
	
    	if (vrp->cur_status.posted) {
	  strcat(st_buf,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_58, "BOOKED "));
	  sprintf(stp,"%3.3d/",vrp->zone_num);
	  strcat(st_buf, stp);
    	}

       if (vrp->cur_status.crse_annul)
	 {
	   strcat(st_buf,"ANNUL/");
	 }
	
    	if (vrp->cur_status.e_posted) {
	  strcat(st_buf,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_59, "EBOOKED "));
	  sprintf(stp,"%3.3d/",vrp->zone_num);
	  strcat(st_buf, stp);
    	}
	
    	if (vrp->cur_status.on_break)
	  strcat(st_buf,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_60, "BREAK/"));
	
    	if (vrp->cur_status.pckup)
	  strcat(st_buf,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_61, "PICKUP/"));
	
    	if (vrp->cur_status.accepted)
	  strcat(st_buf,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_62, "ACCEPT/"));
	
    	if (vrp->cur_status.offered) 
	  strcat(st_buf,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_63, "OFFER/"));
	
    	if (vrp->cur_status.idle)
			strcat(st_buf,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_64, "IDLE/"));


    	mvprintw(1,74,"%-18.18s",st_buf);
	if (vrp->cur_status.meter_on)
	  strcpy(st_buf, catgets(UI_m_catd, UI_1, UI_UI_SSTAT_76, "UP "));
	else
	  strcpy(st_buf, catgets(UI_m_catd, UI_1, UI_UI_SSTAT_77, "DWN"));
	mvprintw(2,74, "%-4.4s", st_buf);
    	attrset(0);
    	mvprintw(S_FLEET_ROW,S_FLEET_COL,"%c",vrp->fleet);			/* print fleet */
    	mvprintw(S_VEH_ROW,S_VEH_COL,"%-4d",vrp->veh_nbr);			/* and vehicle number */
    	mvprintw(S_DRIV_ROW,S_DRIV_COL,"%-5d",vrp->drv_id);			/* and driver */

	  if ( ( GPS_guy ) &&
	       ( vrp->gps_long != 0.0 ) &&
	       ( vrp->gps_lat != 0.0 ))
	    {
	      if ( ( vrp->gps_long > (double)fleet[ss_chosen_fleet]->gps_max_x ) ||
		   ( vrp->gps_long < (double)fleet[ss_chosen_fleet]->gps_min_x ) ||
		   ( vrp->gps_lat  > (double)fleet[ss_chosen_fleet]->gps_max_y ) ||
		   ( vrp->gps_lat  < (double)fleet[ss_chosen_fleet]->gps_min_y ) )
		prt_error(UI_NO_MSG, "TAXI POSITION NOT ON MAP");
	      //	      else if ( !vrp->cur_status.inuse )
	      //		prt_error(UI_VEH_STATUS, " ");
	      else
		{
		  GPS_position_display( vrp );
		  prt_error(UI_NO_MSG, "TAXIGEO VISUALIZATION IN PROGRESS");
		  // Send polling request
		  GPS_position_poll( vrp );
		}
	    }
	  else if ( GPS_guy )
	    prt_error(UI_NO_MSG, "GPS COORDINATES NOT AVAILABLE FOR SELECTED TAXI");


    	refresh();								/* re-draw screen */
	if (hVehResp)
	  //	TFREE( hVehResp );	  
    	return(0);
	}
      else if(ss_choice == 2){							/* get driver info from disk */
    	drp = (struct cisam_dr *)req_buf;					/* fill up request buffer with key to search on */
    	drp->fleet = ss_ch_fleet;						/* character fleet */
    	drp->id = ss_chosen_driv;
	
    	if (db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISEQUAL,0) < 0) {		/* read DRIVER record */
	  prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_65, "<No Driver Found >"));			/* no, print error */
	  return(0);							/* if error, return */
    	}
	
	attrset(A_REVERSE | A_BOLD);
	drp->short_meters ? mvprintw(1,SS_DET_INFO4,"%3d",drp->short_meters) : mvprintw(1,SS_DET_INFO4,"   ");
	drp->late_meters  ? mvprintw(2,SS_DET_INFO4,"%3d",drp->late_meters)  : mvprintw(2,SS_DET_INFO4,"   ");
	drp->no_accepts   ? mvprintw(3,SS_DET_INFO4,"%3d",drp->no_accepts)   : mvprintw(3,SS_DET_INFO4,"   ");
	drp->rejects      ? mvprintw(4,SS_DET_INFO4,"%3d",drp->rejects)      : mvprintw(4,SS_DET_INFO4,"   ");
	drp->breaks	  ? mvprintw(5,SS_DET_INFO4,"%3d",drp->breaks)	     : mvprintw(5,SS_DET_INFO4,"   ");
	drp->req_to_talk  ? mvprintw(1,SS_DET_INFO5,"%3d",drp->req_to_talk)  : mvprintw(1,SS_DET_INFO5,"   ");
	drp->messages	  ? mvprintw(2,SS_DET_INFO5,"%3d",drp->messages)     : mvprintw(2,SS_DET_INFO5,"   ");
	drp->callbacks	  ? mvprintw(3,SS_DET_INFO5,"%3d",drp->callbacks)    : mvprintw(3,SS_DET_INFO5,"   ");
	drp->callouts	  ? mvprintw(4,SS_DET_INFO5,"%3d",drp->callouts)     : mvprintw(4,SS_DET_INFO5,"   ");
	drp->no_shows	  ? mvprintw(5,SS_DET_INFO5,"%3d",drp->no_shows)     : mvprintw(5,SS_DET_INFO5,"   ");
	drp->calls	  ? mvprintw(3,SS_DET_INFO2,"%3d",drp->calls)	     : mvprintw(3,SS_DET_INFO2,"   ");
	drp->flags	  ? mvprintw(4,SS_DET_INFO2,"%3d",drp->flags)	     : mvprintw(4,SS_DET_INFO2,"   ");
	drp->bids	  ? mvprintw(5,SS_DET_INFO2,"%3d",drp->bids)	     : mvprintw(5,SS_DET_INFO2,"   ");
	drp->posts	  ? mvprintw(3,SS_DET_INFO3,"%3d",drp->posts)	     : mvprintw(3,SS_DET_INFO3,"   ");
	drp->cond_posts   ? mvprintw(4,SS_DET_INFO3,"%3d",drp->cond_posts)   : mvprintw(4,SS_DET_INFO3,"   ");
	drp->assigns      ? mvprintw(5,SS_DET_INFO1,"%d",drp->assigns)       : mvprintw(5,SS_DET_INFO1,"   ");
	drp->enroute_posts ? mvprintw(5,SS_DET_INFO3,"%3d",drp->enroute_posts) : mvprintw(5,SS_DET_INFO3,"   ");
	drp->emerg_on_dt_tm ? get_asc_time(drp->emerg_on_dt_tm,dt_buf,tm_buf) : strncpy(tm_buf,"",7);
	mvprintw(5,15,"%6.6s",tm_buf);
	drp->emerg_of_dt_tm ? get_asc_time(drp->emerg_of_dt_tm,dt_buf,tm_buf) : strncpy(tm_buf,"",7);
	mvprintw(5,29,"%6.6s",tm_buf);
	drp->signed_on_dt_tm ? get_asc_time(drp->signed_on_dt_tm,dt_buf,tm_buf) : strncpy(tm_buf,"",7);
	mvprintw(3,SS_DET_INFO1,"%6.6s",tm_buf);
	drp->signed_of_dt_tm ? get_asc_time(drp->signed_of_dt_tm,dt_buf,tm_buf) : strncpy(tm_buf,"",7);
	mvprintw(4,SS_DET_INFO1,"%6.6s",tm_buf);
	drp->id ? mvprintw(2,27,"%-28s",drp->name) : mvprintw(2,27,"                            ");
	drv_col = 17;
    	for(i=0; i<32; i++) {
	        
		if (drp->attributes[i] == YES)
		  {
		    if ( drv_col < SS_DET_TEXT1 )
		      mvprintw(4,drv_col,"%.2s ",fleet[ss_chosen_fleet]->vd_attr_info[i].drv_attr);
		    drv_col += 3;
		  }
    	}

    	memset(st_buf,0,64);
	strcpy(st_buf,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_66, "S-OUT"));							/* this guy is not sign on now */

    	mvprintw(1,74,"%-18.18s",st_buf);
    	attrset(0);
    	mvprintw(S_FLEET_ROW,S_FLEET_COL,"%c",drp->fleet);			/* print fleet */
    	mvprintw(S_VEH_ROW,S_VEH_COL,"%-4d",drp->last_veh_nbr);			/* blank out vehicle number */
    	mvprintw(S_DRIV_ROW,S_DRIV_COL,"%-5d",drp->id);				/* and driver */
    	refresh();								/* re-draw screen */
      }
	//	TFREE( hVehResp );
      }
}

/***********************************************************************************************************************************
clear_vh_drv -
This function clears the vehicle/driver detail portion of the screen.
***********************************************************************************************************************************/
clear_vh_drv()
{
    int i;							/* temporary variable for loops */

    attrset(A_REVERSE | A_BOLD);				/* blank out all detail fields */
    mvprintw(2,7,"     ");
    mvprintw(1,SS_DET_INFO4,"   ");
    mvprintw(2,SS_DET_INFO4,"   ");
    mvprintw(3,SS_DET_INFO4,"   ");
    mvprintw(4,SS_DET_INFO4,"   ");
    mvprintw(5,SS_DET_INFO4,"   ");
    mvprintw(1,SS_DET_INFO5,"   ");
    mvprintw(2,SS_DET_INFO5,"   ");
    mvprintw(3,SS_DET_INFO5,"   ");
    mvprintw(4,SS_DET_INFO5,"   ");
    mvprintw(5,SS_DET_INFO5,"   ");
    mvprintw(3,SS_DET_INFO2,"   ");
    mvprintw(4,SS_DET_INFO2,"   ");
    mvprintw(5,SS_DET_INFO2,"   ");
    mvprintw(3,SS_DET_INFO3,"   ");
    mvprintw(4,SS_DET_INFO3,"   ");
    mvprintw(5,SS_DET_INFO3,"   ");
    mvprintw(5,15,"      ");
    mvprintw(5,29,"      ");
    mvprintw(3,SS_DET_INFO1,"      ");
    mvprintw(4,SS_DET_INFO1,"      ");
    mvprintw(2,27,"                            ");
    mvprintw(2,81,"      ");

    for(i=0; i<9; i++) {
	mvprintw(3,17+(3*i),"   ");
	mvprintw(4,17+(3*i),"   ");
    }
    mvprintw(1,74,"                  ");
    mvprintw(2,74,"    ");
    attrset(0);
    refresh();								/* re-draw screen */
    return(0);
}



/***********************************************************************************************************************************
ss_def_fleet -
This function checks the user profile area in shared memory and sets the current fleet to the default fleet specified for that
user.
***********************************************************************************************************************************/
int ss_def_fleet()
{
#ifdef DEBUG
	sprintf(trace_str,"ss_def_fleet\n");
	TRACE(trace_str);
#endif

    if ( (ss_chosen_fleet = def_fl_index) >= 0 )  {
		/* set current fleet to user default */
	    ss_ch_fleet = fleet[ss_chosen_fleet]->fleet_id;			/* set current fleet id */
	    mvprintw(Z_FLEET_ROW,Z_FLEET_COL,"%c",ss_ch_fleet);			/* display it on screen */
	    return(0);
    }
    return(-1);
}



/***********************************************************************************************************************************
ss_new_fleet -
This routine sets the new current fleet if valid.
***********************************************************************************************************************************/
ss_new_fleet(c)
char c;									/* fleet id as entered by user */
{
    int i;								/* temporary variable */

#ifdef DEBUG
	sprintf(trace_str,"ss_new_fleet - c=%c\n",c);
	TRACE(trace_str);
#endif

	if ((ss_chosen_fleet = find_findex(c)) == -1 ) {
		prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_67, "<fleet id>")); 			/* user entered invalid fleet id, trap user in field */
		return(0);
	}
	if (fleet[ss_chosen_fleet]->active != YES )  { 			/* check if fleet entered is active or not */
		prt_error(UI_NOT_ACTIVE,"%s %c>", catgets(UI_m_catd, UI_1, UI_UI_SSTAT_68, "fleet"), c); 		/* inactive fleet */
		return(0);
	}
	if ((*ui_fleet_perms[ss_chosen_fleet])() == NOT_ALLOWED ) {	/* does user have permissions to work with this fleet */
		prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_69, "<fleet>")); 			/* no permission */
		return(0);						/* trap user in field */
	}

    	ss_ch_fleet = fleet[ss_chosen_fleet]->fleet_id;
    	return(1);
}



/***********************************************************************************************************************************
new_veh -
This function sets the new current vehicle number if valid.
***********************************************************************************************************************************/
new_veh(v,buf)
int v;									/* new vehicle number */
char buf[];								/* buffer containing user's input */
{
    int i;								/* temporary variable for loops */
    struct cisam_vh *vhp;						/* pointer to vehicle structure */

#ifdef DEBUG
	sprintf(trace_str,"new_veh - v=%d\n",v);
	TRACE(trace_str);
#endif

    if (ss_chosen_fleet < 0) {						/* do we have a current fleet? */
	prt_error(UI_NOFLEET,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_70, "<No fleet chosen yet>"));			/* no, print error */
	return(-1);
    }

    for (i=0; i<CHARS_VEH; i++)
	if ((isdigit(buf[i]) == 0) && (isspace(buf[i]) == 0) && (buf[i] != '\0'))
	    v = -1;

    vhp = (struct cisam_vh *)req_buf;					/* fill in key for validation */
    vhp->fleet = ss_ch_fleet;
    vhp->nbr = v;

    if (db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISEQUAL,0) < 0) {	/* read VEH record */
	prt_error(UI_NOFIELD,"<%s %s>", catgets(UI_m_catd, UI_1, UI_UI_SSTAT_71, "No Vehicle Found"), buf);	/* no, print error */
	return(-1);							/* if error, return */
    }
    ss_chosen_veh = v;							/* yes, set new current vehicle */
    ss_chosen_driv = vhp->last_driver;					/* set new current driver */
    ss_choice = 1;							/* vehicle number entered */
    get_drv_name(vhp->last_driver);					/* get driver's name */
    return(0);
}


/***********************************************************************************************************************************
new_driv -
This function sets the new current driver number if valid.
***********************************************************************************************************************************/
new_driv(d,buf)
int d;									/* new driver number */
char buf[];								/* buffer containing user's input */
{
    int i;								/* temporary variable for loops */
    struct cisam_dr *drp;						/* pointer to driver structure */

#ifdef DEBUG
	sprintf(trace_str,"new_driv - d=%d\n",d);
	TRACE(trace_str);
#endif

    if (ss_chosen_fleet < 0) {
	prt_error(UI_NOFLEET,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_70, "<No fleet chosen yet>"));			/* no, print error */
	return(-1);
    }

    for (i=0; i<CHARS_DRIV; i++)
	if ((isdigit(buf[i]) == 0) && (isspace(buf[i]) == 0) && (buf[i] != '\0'))
	    d = -1;

    drp = (struct cisam_dr *)req_buf;					/* fill up request buffer with key to search on */
    drp->fleet = ss_ch_fleet;
    drp->id = d;

    if (db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISEQUAL,0) < 0) {		/* read VEH record */
	prt_error(UI_NOFIELD,"<%s %s>", catgets(UI_m_catd, UI_1, UI_UI_SSTAT_72, "No Driver Found"), buf);		/* no, print error */
	return(-1);							/* if error, return */
    }
    ss_chosen_veh = drp->last_veh_nbr;					/* yes, set new current vehicle */
    ss_choice = 2;							/* driver number entered */

    ss_chosen_driv = d;							/* yes, set new current driver */
    strncpy(drv_name,drp->name,29);					/* get driver's name */
    return(0);
}



/***********************************************************************************************************************************
get_drv_name -
This function gets the driver's name from disk.
***********************************************************************************************************************************/
get_drv_name(id)
int id;
{
    struct cisam_dr *drp;						/* pointer to driver structure */

    drp = (struct cisam_dr *)req_buf;
    drp->fleet = ss_ch_fleet;
    drp->id = id;
    if (db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISEQUAL,0) < 0) {		/* read DRIVER record */
	prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_73, "<No Driver record Found>"));		/* no, print error */
	strncpy(drv_name,"",29);
	return(0);							/* if error, return */
    }
    strncpy(drv_name,drp->name,29);
}


GPS_zoom( zoom_dir )
     char zoom_dir;
{
  int                 res, i, sockfd;
  char                send_buf[132];
  struct sockaddr_in  serv_addr, cli_addr;
  struct hostent      *hp;

  bzero( send_buf, sizeof( send_buf ) );

  sprintf( send_buf, "%c", zoom_dir );
  
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  hp = gethostbyname(sGPS_host);
  bcopy(hp->h_addr, (char *)&serv_addr.sin_addr, hp->h_length);
  serv_addr.sin_port = htons(6232);
  if ( ( sockfd = socket(AF_INET, SOCK_DGRAM, 0 ) ) != 0 )      
    {
      bzero((char *)&cli_addr, sizeof(cli_addr));
      cli_addr.sin_family = AF_INET;
      cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      cli_addr.sin_port = htons(0);
      if ( bind( sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr) ) >= 0 )
	res = sendto(sockfd, send_buf, strlen(send_buf), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
      
      close( sockfd );
    }

}

int
GPS_position_poll( vrp )
     VEH_RESP_REC *vrp;
{
  struct excpt_rec except_fifo;

  bzero( &except_fifo, sizeof(struct excpt_rec) );
  except_fifo.u_prcs_id = pid;
  except_fifo.rec_type = GPS_POS_REQ;
  except_fifo.except_ptr = '\0';

  //Except_nbr field will be used to store fleet_nbr
  if ( vrp->fleet != 'H' ) //special HTD case where Fleet H is used for fleet 0
    except_fifo.except_nbr = vrp->fleet - 'A';
  else
    except_fifo.except_nbr = 0;
  
  except_fifo.resolve_code = R_EMERG;
  except_fifo.call_nbr = 0;
  except_fifo.grouped_with = 0;
  except_fifo.veh_nbr = vrp->veh_nbr;
  except_fifo.zone_nbr = 0;
  except_fifo.old_zone_nbr = 0;
  except_fifo.old_due_time = 0;

  if (write(fifo_fd,&except_fifo,sizeof(struct excpt_rec)) == -1)	/* Write FIFO to dispatch */
    {
      //      ERROR(' ',"Error writing to FIFO.");
    }
}
  
int
GPS_position_display( vrp )
     VEH_RESP_REC  *vrp;
{
  int                 res, i, sockfd;
  char                send_buf[132];
  struct sockaddr_in  serv_addr, cli_addr;
  struct hostent      *hp;

  if ( vrp->veh_nbr < 0 )
    return ( -1 );

  bzero( send_buf, sizeof( send_buf ) );

  sprintf( send_buf, "%c!%c!%d", MAP_APP_RQST_TYPE_SHOW, vrp->fleet, vrp->veh_nbr );
  
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  hp = gethostbyname(sGPS_host);
  if ( hp == NULL )
    {
      prt_error( UI_NO_MSG, "TaxiGEO: problem with hostname mapping (%s)", sGPS_host );
      return(-1);
    }
    
  bcopy(hp->h_addr, (char *)&serv_addr.sin_addr, hp->h_length);
  serv_addr.sin_port = htons(6232);
  if ( ( sockfd = socket(AF_INET, SOCK_DGRAM, 0 ) ) != 0 )      
    {
      bzero((char *)&cli_addr, sizeof(cli_addr));
      cli_addr.sin_family = AF_INET;
      cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      cli_addr.sin_port = htons(0);
      if ( bind( sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr) ) >= 0 )
	res = sendto(sockfd, send_buf, strlen(send_buf), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
      
      close( sockfd );
    }

  return(0);

}


DoVehDrivDetail()
{
  WINDOW   *WVehDrivDetail;
  int c;
  VEH_REC  veh_rec;
  DRIVER_REC drv_rec;
  char     scrtch_buf[132];
  int      i = 0, attr_col;

  bzero( &veh_rec, sizeof( VEH_REC ) );
  bzero( &drv_rec, sizeof( DRIVER_REC ) );

  WVehDrivDetail = newwin( 9, 110, 4, 10 );
  tpak_box( WVehDrivDetail, 0, 0 );
  
  if ( ss_chosen_veh > 0 || ss_chosen_driv > 0 )
    {
      veh_rec.fleet = fleet[ss_chosen_fleet]->fleet_id;
      veh_rec.nbr = ss_chosen_veh;
      if ( db_read_key( VEHICLE_FILE_ID, &veh_rec, &veh_key2, ISEQUAL ) == SUCCESS )
	{
	  mvwprintw( WVehDrivDetail, 2, 5, catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_8, "Veh#"));
	  mvwprintw( WVehDrivDetail, 2, 25, "%d", veh_rec.nbr );
	  mvwprintw( WVehDrivDetail, 3, 5, catgets(UI_m_catd, UI_1, UI_UI_DISP_S_43, "Driver"));
	  mvwprintw( WVehDrivDetail, 4, 5, "%s %s",
		    catgets(UI_m_catd, UI_1, UI_UI_EXEC_P_9, "vehicle file"),
		    catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_17, "Phone") );
          mvwprintw( WVehDrivDetail, 4, 25, "%s", veh_rec.telephone );

	  mvwprintw( WVehDrivDetail, 5, 5, "%s %s",
		     catgets(UI_m_catd, UI_1, UI_UI_EXEC_P_10, "driver file"),
		     catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_17, "Phone") );
	  
	  mvwprintw( WVehDrivDetail, 6, 4, catgets(UI_m_catd, UI_1, UI_UI_SSTAT_11, "Veh Attributes ") );
	  mvwprintw( WVehDrivDetail, 7, 4, catgets(UI_m_catd, UI_1, UI_UI_SSTAT_17, "Drv Attributes ") );
	  attr_col = 25;
	  for ( i = 0; i< 32; i++ )
	    {
	      if ( veh_rec.attr[i] == YES )
		{
		  mvwprintw( WVehDrivDetail, 6, attr_col, "%.2s ", fleet[ss_chosen_fleet]->vd_attr_info[i].veh_attr );
		  attr_col += 3;
		}
	    }
	}
      drv_rec.fleet = fleet[ss_chosen_fleet]->fleet_id;
      drv_rec.id = ss_chosen_driv;
      if ( db_read_key( DRIVER_FILE_ID, &drv_rec, &dr_key1, ISEQUAL ) == SUCCESS )
	{
	  mvwprintw( WVehDrivDetail, 3, 25, "%d %s", drv_rec.id, drv_rec.name );
	  if (1)
	    mvwprintw( WVehDrivDetail, 5, 25, "%s", drv_rec.license_nbr );
	  else
	    mvwprintw( WVehDrivDetail, 5, 25, "%s", drv_rec.phone );
	  attr_col = 25;
	  for ( i = 0; i< 32; i++ )
	    {
	      if ( drv_rec.attributes[i] == YES )
		{
		  mvwprintw( WVehDrivDetail, 7, attr_col, "%.2s ", fleet[ss_chosen_fleet]->vd_attr_info[i].drv_attr );
		  attr_col += 3;
		}
	    }
	}
    }  


  attrset(0);
  wrefresh( WVehDrivDetail );

  do {
    c = wgetch(WVehDrivDetail);
  } while ((c != 27) && (c != '\n'));
  
  flush_input(stdscr);
  werase(WVehDrivDetail);
  touchwin(stdscr);
  refresh();
  return(0);

}
