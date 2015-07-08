static char sz__FILE__[]="@(#)ui_zstat.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  ui_zstat.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:52:07
* @(#)  Last delta: 12/2/94 at 18:15:58
* @(#)  SCCS File: /taxi/sccs/s.ui_zstat.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

#include <stdio.h>
#include <cursesX.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#include "mad_error.h"
#include "taxipak.h"
#include "mem_mgmt.h"
#include "Object.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Zone_public.h"
#include "Zone_private.h"
#include "mads_types.h"
#include "mad_ipc.h"
#include "acc.h"
#include "veh_driv.h"
#include "calls.h"
#include "mad_error.h"
#include "screens.h"
#include "ui_def.h"
#include "ui_ipc.h"
#include "user.h"
#include "ui_error.h"
#include "df_maint.h"
#include "call_entry.h"
#include "ui_stat.h"
#include "func_keys.h"
#include "menu.h"
#include "mads_isam.h"
#include "ex_key.h"
#include "language.h"
#include "ui_strdefs.h"
#include "enhance.h"
#include "switch_ext.h"

extern struct offsets *offset;
extern struct fleets *fleet[];					/* array of pointers to fleets structure in shared memory */
extern struct cisam_cl call_struct;
extern struct users *uiuser[];					/* array of pointers to users structure in shared memory */
extern struct menus *cmenu[];					/* array of pointers to menus structure in shared memory */
extern int map_array[];						/* to map scr_name to index into menus structures */
extern int user_index;						/* index into users structure in shared memory */
extern int def_fl_index;					/* index into fleet array for user's default fleet */
extern char req_buf[];						/* buffer for r/w records from/to disk */
extern int uid;							/* users id # */
extern char scr_type;						/* current screen type */
extern int scr_name;						/* current screen name */
extern short end_zone_status;					/* flag to end zone status display */
extern int msgkey;						/* */
extern int pid;							/* process id */
extern short clear_err_flag;					/* error flag when error message displayed */
extern struct keydesc zn_key1;



struct scr_flds zone_status_scr_flds[] = {			/* screen field structure for user input fields */
	{0,0,1,1},
	{Z_FLEET_ROW,Z_FLEET_COL,CHARS_FLEET,Z_ZONE},
	{Z_ZONE_ROW,Z_ZONE_COL,CHARS_ZONE,Z_FLEET},
};

/* declare functions for checking fleet permissions */
extern int ui_fleet_perms0(), ui_fleet_perms1(), ui_fleet_perms2(), 
	ui_fleet_perms3(), ui_fleet_perms4(), ui_fleet_perms5(), 
	ui_fleet_perms6(), ui_fleet_perms7();

/* declare array of pointers to functions for checking fleet perm's */
extern int (*ui_fleet_perms[])();

int zs_chosen_fleet = -1;					/* currently chosen fleet */
int zs_chosen_zone = -1;					/* currently chosen zone */
char zs_chosen_veh_attr[ATTR_MAX];
char zs_chosen_drv_attr[ATTR_MAX];
char zs_ch_fleet;						/* currently chosen fleet id */

/***********************************************************************************************************************************
zone_status -
This function is called from the User Interface process when a user selects the Zone Status Display screen.  The routine formats
the screen for display of zone status information.  The user may enter two fields (the fleet and the zone).  A default fleet is
provided based on the information from the user profile area in shared memory.  The bottom portion of the screen is used to display
all the zones in a particular fleet.  Up to seven pages may be displayed by using <next> and <previous> keys.  The top portion is
for detailed zone display.  When a user enters a zone number, an ipc message is sent to Dispatch requesting the detailed info for
that zone.  The zone status procedure waits for a response from Dispatch to continue.
***********************************************************************************************************************************/
zone_status()
{
    int var_zone_page = 1;					/* current page for zone summary output */
    int old_time = -1;						/* last time of screen update */
    int zpiu = Z_ZONE;						/* field cursor is to start on */
    int update_screen_rate_ms;					/* How often, in millisec, to update status screen */
    int i;

#ifdef DEBUG
	sprintf(trace_str,"zone_status\n");
	TRACE(trace_str);
#endif

    zs_chosen_fleet = -1;						/* currently chosen fleet */
    zs_chosen_zone = -1;						/* currently chosen zone */
    zs_ch_fleet = '\0';							/* currently chosen fleet id */
    for ( i = 0; i < ATTR_MAX; i++ )
      {
	zs_chosen_veh_attr[i] = NO;
	zs_chosen_drv_attr[i] = NO;
      }
    
    end_zone_status = 0;						/* flag to end zone status display */

    disp_zone_status_scr();						/* output zone status display headers on screen */
    prt_entry_areas(stdscr,zone_status_scr_flds,MAX_PIU_ZONE);		/* fill in user entry areas with underscores */
    if ( def_fleet()== -1 )  
	/* no defalut fleet specified */
	zpiu =  Z_FLEET;	/* go to fleet field */
    else 
	    refresh();								/* re-draw the screen */

    if (zs_chosen_fleet >= 0)
      {
	update_screen_rate_ms = fleet[zs_chosen_fleet]->zone_status_update_freq * 1000;

	/* Rate must not be less than 10 seconds long */
	if (update_screen_rate_ms < 10000)
	  update_screen_rate_ms = 10000;
      }

    /* Default to 10 seconds */
    else
      update_screen_rate_ms = 10000;

    while ((!end_zone_status) && (scr_name == ZONE_STATUS_SCR)) {	/* repeat until user decides to end it */
	if (zs_chosen_fleet >= 0)					/* have we established a fleet yet? */
	    if (fleet[zs_chosen_fleet]->zone_update_time > old_time) {	/* yes, did dispatch update zone summaries again? */
		disp_zones(&var_zone_page);				/* yes, display zone summary again. */
		disp_specific();					/* redisplay the detailed info for the selected zone */
		refresh();
		old_time = fleet[zs_chosen_fleet]->zone_update_time;	/* change screen update time */
	    }
	read_inpt(&zpiu,&var_zone_page, update_screen_rate_ms);		/* read input from user */
    }
    return(0);								/* return to caller in User Interface */
}



/***********************************************************************************************************************************
disp_zone_status_scr -
This function displays the headers for the zone status display screen.  The reverse attributes are set for the top portion of the
screen.  The date and user id number are also displayed on the top line.
***********************************************************************************************************************************/
disp_zone_status_scr()
{
    int i, start_col, len, indx, hdls, hdln;                    /* temporary variable for loops */
    char hdl[200];

#ifdef DEBUG
	sprintf(trace_str,"disp_zone_status\n");
	TRACE(trace_str);
#endif

    clear();							/* clear screen */
    attrset(A_REVERSE | A_BOLD);				/* set bold reverse for background */
    for(i=1; i<=5; i++)						/* for lines 1-5, set reverse and bold */
	mvaddstr(i,0,"                                                                                                                                    ");
    attrset(A_REVERSE);						/* set dim reverse for lines 0 and 6 */
    mvaddstr(0,0,"                                                                                                                                    ");
    mvaddstr(6,0,"                                                                                                                                    ");
    prt_date();							/* current date */

    indx = map_array[scr_name];	

    len = strlen(cmenu[indx]->mf_desc); 			/* print the title of the screen */
    start_col = (COLS - len - 1 )/2;
    mvprintw(0,start_col,"%s", cmenu[indx]->mf_desc);
    mvprintw(0,start_col+len+3,"(%d)",cmenu[indx]->mf_command);
    mvprintw(0,UID_COL,"%d",uid);
    mvaddstr(1,0,catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_1, " Fleet  "));
    mvaddstr(1,55,catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_2, " Backup zones:  zone cl vh en zone cl vh en zone cl vh en zone cl vh en "));
    mvaddstr(2,4,catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_3, " Unassigned"));
    mvaddstr(2,21,catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_4, " Time-out"));
    mvaddstr(2,36,catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_16, " Time-calls "));
    mvaddstr(3,4,catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_5, " Booked: "));
    mvaddstr(4,4,catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_6, " Enroute: "));
    mvaddstr(5,0,catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_7, "   Zone a"));
    strcpy(hdl,catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_8, " zone cl vh en tc"));
    hdls=TO_NEXT_COL_E4;
    hdln=130/hdls;                                              /* no zones in a line */
    hdl[hdls]=0;
    for(i=0; i<hdln; i++) { mvaddstr(6,i*hdls,hdl); }
    attrset(0);							/* set normal attribute */
    for(i=1; i<=MAX_PIU_ZONE; i++) {				/* leave normal space on either side of user entered fields */
	mvaddstr(zone_status_scr_flds[i].prompt_row,zone_status_scr_flds[i].prompt_col-1," ");
	mvaddstr(zone_status_scr_flds[i].prompt_row,zone_status_scr_flds[i].prompt_col+zone_status_scr_flds[i].max_chars," ");
    }
    return(0);
}


/***********************************************************************************************************************************
def_fleet -
This function checks the user profile area in shared memory and sets the current fleet to the default fleet specified for that
user.
***********************************************************************************************************************************/
def_fleet()
{
#ifdef DEBUG
	sprintf(trace_str,"def_fleet\n");
	TRACE(trace_str);
#endif

    if ( (zs_chosen_fleet = def_fl_index) >= 0 )	{		/* set current fleet to user default */
	    zs_ch_fleet = fleet[zs_chosen_fleet]->fleet_id;		/* set current fleet id */
	    mvprintw(Z_FLEET_ROW,Z_FLEET_COL,"%c",zs_ch_fleet);		/* display it on screen */
	    return(0);
    }
    return(-1);
}



/***********************************************************************************************************************************
read_inpt -
This function waits for the user to enter a field and then processes the request.  The user may switch fields by tabbing or using
arrow keys, or switch pages of the zone summary display, or enter a new field or function key.
***********************************************************************************************************************************/
read_inpt(zpiu,zpg, update_screen_rate_ms)
int *zpiu;							/* piu of field user is currently on */
int *zpg;							/* page number that user is currently on */
int update_screen_rate_ms;
{
    char buf[READ_BUF_SIZE];					/* buffer to store users text */
    int key;							/* key set if user actually entered something */
    int i,j,x;							/* temporary variables for loops and values */
    char c;							/* used for character stripped of attribute values */
    int should_clear = 0;

#ifdef DEBUG
	sprintf(trace_str,"read_inpt - *zpiu=%d, *zpg=%d\n",*zpiu,*zpg);
	TRACE(trace_str);
#endif

    /* Put cursor at input line */
    move((zone_status_scr_flds + *zpiu)->prompt_row, (zone_status_scr_flds + *zpiu)->prompt_col);
    refresh();

    /* Wait for a character */
    if ((c = wgetsh_or_wait(stdscr, update_screen_rate_ms)) == -1) {
	/* If no char, return to update screen */
	return(0);
    }

    /* Put the char back in buffer so read_field can read it */
    ungetsh(c);

    i = read_field(stdscr,zone_status_scr_flds,*zpiu,buf,&key,&should_clear);			/* read field from user */
    if ((clear_err_flag) && (i != -1))						/* is there an error mesg displayed? */
	clear_error();								/* yes, clear it */
    switch(i) {
	case KEY_TAB:
	case KEY_DOWN:
	case KEY_UP:								/* user wants to change fields */
			rev_to_norm(stdscr,zone_status_scr_flds,*zpiu);		/* set text to normal value */
			*zpiu = zone_status_scr_flds[*zpiu].tab_field;		/* get new field piu */
			break;

	case NEXT_PG_KEY:
	case KEY_NPAGE:								/* user wants to go to next page */
			(*zpg)++;						/* increment page count */
			disp_zones(zpg);					/* display new page of zone summary */
			refresh();
			break;



	case PREV_PG_KEY:
	case KEY_PPAGE:								/* user wants to go to prev page */
			(*zpg)--;						/* decrement page count */
			disp_zones(zpg);					/* display new page of zone summary */
			refresh();
			break;

        case ALTERNATE_KEY:
			if (1)
                          {
                            for ( i = 0; i < 32; i++ )
                              {
                                call_struct.veh_attr[i] = NO;
                                call_struct.drv_attr[i] = NO;
                              }
                            alt_attr();
                            strncpy(zs_chosen_veh_attr, call_struct.veh_attr, ATTR_MAX);
                            strncpy(zs_chosen_drv_attr, call_struct.drv_attr, ATTR_MAX);
                          }
			/** intentionally falling through **/
	case 0:
	case '\r':
	case '\n':								/* user has entered something into field */
			rev_to_norm(stdscr,zone_status_scr_flds,*zpiu);		/* set field to normal attributes */
			if (key == 0) {						/* was anything actually entered? */
			    for(j=0;j<zone_status_scr_flds[*zpiu].max_chars;j++) {	/* no, use wat was already there */
				wmove(stdscr,zone_status_scr_flds[*zpiu].prompt_row,zone_status_scr_flds[*zpiu].prompt_col+j);
				c = winch(stdscr);				/* get next character */
				c = c & 0x7f;					/* strip off all attributes */
				buf[j] = c;					/* save in buffer */
			    }
			    buf[j] = '\0';					/* end with null */
			}
			    
			x = atoi(buf);						/* get number value */
			if (*zpiu==Z_FLEET) {					/* new fleet field? */
			    if (new_fleet(buf[0]) > 0) {			/* yes, set new current fleet */
				*zpg = 1;					/* move to first page of new fleet */
				zs_chosen_zone = -1;				/* clear zone */
				disp_zones(zpg);				/* display zones for this fleet */
    				attrset(A_UNDERLINE);				/* underline */
				mvprintw(Z_ZONE_ROW,Z_ZONE_COL,"   ");		/* clear old zone */
    				attrset(0);					/* normal */
				clear_zone_detail();				/* clear detail area */
				refresh();
				*zpiu = zone_status_scr_flds[*zpiu].tab_field;	/* get new field piu */
			    }
			}
			else if (*zpiu==Z_ZONE) {				/* is this new zone field? */
			    new_zone(x,buf);					/* set new zone */
			    disp_specific();					/* display detail of zone */
			    refresh();
			}
			break;

	case -1:								/* user did not enter anything */
			break;

	default:
			ftnkey_handler(zpiu,i,0);					/* user entered function key */
			refresh();						/* refresh screen after function key handler */
			break;
    }
	return(0);
}


/***********************************************************************************************************************************
disp_zones -
This function displays the zone summaries on the bottom of the screen.  A full page is displayed if there are enough zones to
fill it.  The page number is passed.
***********************************************************************************************************************************/
disp_zones(zpg)
int *zpg;								/* page number to display */
{
    int i,j,k,ctr,zpp,tnc,nop;                                              /* temporary variables for loops, etc */
    char spc[30];
    short NumTimeCalls,interval;
    time_t   now_time;

#ifdef DEBUG
	sprintf(trace_str,"disp_zones - *zpg=%d\n",*zpg);
	TRACE(trace_str);
#endif
    nop=9;
    zpp=MAX_ZONES_ON_PG_E4;
    tnc=TO_NEXT_COL_E4;

    now_time = time(0);
    
    if ((*zpg < 1) || (*zpg > nop)) {                                   /* is this a valid page? */
	prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_9, "<No more pages that way>"));		/* no, print error */
	if (*zpg < 1)							/* is it too small? */
	    *zpg = 1;							/* yes, set to page 1 */
	else if (*zpg > nop)                                            /* is it too big? */
	    *zpg = nop;                                                 /* set to page nop (enhancement depended) */
    }
    else if ((*zpg > 1) && (((*zpg-1) * zpp) >= fleet[zs_chosen_fleet]->zone_ct_ptr)) {                         /* anything on this page? */
	    prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_9, "<No more pages that way>"));		/* no, print error */
	    *zpg -= 1;							/* go back a page */
    }
    else {
	j = k = 0;                                                              /* start off at zero */
	for(i=(*zpg-1)*zpp; i<(*zpg * zpp); i++) {                              /* go for as many as will fit */
	    if (i<fleet[zs_chosen_fleet]->zone_ct_ptr) {			/* are there any more zones for this page? */
		if (fleet[zs_chosen_fleet]->zone_sumry[i].zone_num == 1000)
		  {
		    /** wakeup zone **/
		    mvprintw( START_SUM_ROW+k,START_SUM_COL+(j*tnc),"%s",
			     catgets( UI_m_catd, UI_1, UI_UI_SSTAT_42, "Wake-ups" ) );
		  }
		else if( fleet[zs_chosen_fleet]->zone_sumry[i].time_outs == YES){
			attrset(A_REVERSE);
			mvprintw(START_SUM_ROW+k,START_SUM_COL+(j*tnc),"%3.3d",fleet[zs_chosen_fleet]->zone_sumry[i].zone_num);
			mvprintw(START_SUM_ROW+k,START_SUM_COL+(j*tnc)+3,"%s","   ");
			attrset(0);
		}
		else{
			attrset(0);
			mvprintw(START_SUM_ROW+k,START_SUM_COL+(j*tnc),"%3.3d",fleet[zs_chosen_fleet]->zone_sumry[i].zone_num);
			mvprintw(START_SUM_ROW+k,START_SUM_COL+(j*tnc)+3,"%s","    ");
		}

		if ( fleet[zs_chosen_fleet]->zone_sumry[i].zone_num != 1000)
		  {
		    fleet[zs_chosen_fleet]->zone_sumry[i].no_calls ?
		      mvprintw(START_SUM_ROW+k,START_SUM_COL+(j*tnc)+4,"%2d",fleet[zs_chosen_fleet]->zone_sumry[i].no_calls) :
			mvprintw(START_SUM_ROW+k,START_SUM_COL+(j*tnc)+4,"  ");
		    fleet[zs_chosen_fleet]->zone_sumry[i].no_veh ?
		      mvprintw(START_SUM_ROW+k,START_SUM_COL+(j*tnc)+7,"%2d",fleet[zs_chosen_fleet]->zone_sumry[i].no_veh) :
			mvprintw(START_SUM_ROW+k,START_SUM_COL+(j*tnc)+7,"  ");
		    fleet[zs_chosen_fleet]->zone_sumry[i].no_enroute ?
		      mvprintw(START_SUM_ROW+k,START_SUM_COL+(j*tnc)+10,"%2d",fleet[zs_chosen_fleet]->zone_sumry[i].no_enroute) :
			mvprintw(START_SUM_ROW+k,START_SUM_COL+(j*tnc)+10,"  ");
		  }
		fleet[zs_chosen_fleet]->zone_sumry[i].no_timecalls ?
		  mvprintw(START_SUM_ROW+k,START_SUM_COL+(j*tnc)+13,"%2d", fleet[zs_chosen_fleet]->zone_sumry[i].no_timecalls) :
		    mvprintw(START_SUM_ROW+k,START_SUM_COL+(j*tnc)+13,"  ");
	      }
	    else {
		sprintf(spc,"%28s",""); spc[tnc]=0;                             /* make a blank zone */
		mvprintw(START_SUM_ROW+k,START_SUM_COL+(j*tnc),spc);            /* no, blank it out */
	    }
	    if (k < LAST_DATA_LINE-START_SUM_ROW)				/* at end of page? */
		k++;								/* no, increment row */
	    else {
		k = 0;								/* yes, start a new column */
		j++;
	    }
	}
    }
    return(0);
}



/***********************************************************************************************************************************
disp_specific -
This function displays the detailed zone information based on the zone supplied by the user.  An ipc message is sent to the
Dispatch process to request the detail on the specified zone.  A read is hung waiting for a reply to the request.
NOTE:
    If Dispatch dies or is slow in responding, this process will get stuck here waiting for a reply.
***********************************************************************************************************************************/
disp_specific()
{
    int i,j,k;								/* temporary variables for loops, etc. */
    struct zone_rqst zrq;						/* structure for requesting zone detail info */
    ZONE_RESP  *zrp;
    ZONE_RESP_HNDL hZoneResp;
    int rc, times_thru;
    int NumTimeCalls;
    

    if (zs_chosen_zone >= ZONE_MAX ||
	zs_chosen_zone <= 0 ||
	fleet[zs_chosen_fleet]->zone_ptr[zs_chosen_zone] == NULL) {
	clear_zone_detail();
	return(0);							/* if invalid zone, return */
    }

    zrq.d_prcs_id = DISP_DEST;						/* send ipc to Dispatch */
    zrq.u_prcs_id = pid;						/* user's process id */
    zrq.rec_type = ZONE_REQ;						/* zone request type */
    zrq.fleet = zs_ch_fleet;						/* current fleet */
    zrq.zone_nbr = zs_chosen_zone;					/* and zone */

    bzero(&zrq.veh_attr, sizeof(zrq.veh_attr));
    for ( i = 0; i < ATTR_MAX; i++ )
      {
	if ( zs_chosen_veh_attr[i] == YES )
	  set_bit( i, sizeof(zrq.veh_attr), &zrq.veh_attr );
      }
    bzero(&zrq.drv_attr, sizeof(zrq.drv_attr));

    for ( i = 0; i < ATTR_MAX; i++ )
      {
	if ( zs_chosen_drv_attr[i] == YES )
	  set_bit( i, sizeof(zrq.drv_attr), &zrq.drv_attr );
      }

    
    msgsnd(msgkey,(struct msgbuf *)&zrq,sizeof(struct zone_rqst), IPC_NOWAIT);	/* send ipc message */

    hZoneResp = TMALLOC( ZONE_RESP, 1 );
    zrp = ( ZONE_RESP * )hZoneResp;
    times_thru = 0;
    while (times_thru <=2 ) {

	rc = msgrcv(msgkey,(void *)zrp,sizeof(ZONE_RESP),(long) pid,0);	/* receive responses until correct one */

	if (rc == -1) {
	    
	    /* If error not due to caught signal */
	    if (errno != EINTR) {
		prt_error(UI_FATAL, "%s", catgets(UI_m_catd, UI_1, UI_UI_SSTAT_74, "Error reading from dispatch in msgrcv"));
		ERROR(' ',"User-IPC","Error reading ipc message from DISPATCH");
		TFREE( hZoneResp );
		return(0);
	    }

	    /* if error was a caught signal (expecting alarm for status line display) allow
	     * two such errors (meaning a timeout of 1 to 2 minutes since the alarm comes
	     * once a minute.) */

	    if (++times_thru == 2) {
		prt_error(UI_SYSTEM_DOWN, "");
		ERROR(' ',"User-IPC","Error reading ipc message from DISPATCH");
		TFREE( hZoneResp );
		return(0);
	    }

	}
	
	/* Found right response, exit loop */
	else if (zrp->rec_type == ZONE_REQ)
	    break;

	/* Invalid response */
	else {
	    prt_error(UI_FATAL, "%s", catgets(UI_m_catd, UI_1, UI_UI_SSTAT_74, "Error reading from dispatch in msgrcv"));
	    ERROR(' ',"User-IPC","Error reading ipc message from DISPATCH");
	    TFREE( hZoneResp );
	    return(0);
	}
    }

    if (zrp->fleet == '\0') {
	prt_error(UI_INVALID_NUM,"<%s: %d>", catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_10, "detail for zone not found"), zs_chosen_zone);
	TFREE( hZoneResp );
	return(0);
      }
    
    
    attrset(A_REVERSE | A_BOLD);					/* detail in bold reverse */
    mvprintw(2,0," %2d ",zrp->unassn_calls);				/* format detail response */
    mvprintw(2,17," %2d ",zrp->timeout_calls);
    mvprintw(2,32, " %2d", zrp->time_calls);
    
    mvprintw(START_PST_ROW,0," %2d ",zrp->veh_posted);
    mvprintw(START_ENR_ROW,0," %2d ",zrp->veh_enroute);
    for (i=0; i<VEH_IN_ZN_RESP; i++) {					/* output vehicles posted and enroute */
	if (zrp->post_veh_ids[i] != 0)
	    mvprintw(START_PST_ROW,START_PST_COL+(i*TO_NEXT_VEH),"%4d ",zrp->post_veh_ids[i]);
	else
	    mvprintw(START_PST_ROW,START_PST_COL+(i*TO_NEXT_VEH),"     ");
	if (zrp->enr_veh_ids[i] != 0)
	    mvprintw(START_ENR_ROW,START_ENR_COL+(i*TO_NEXT_VEH),"%4d ",zrp->enr_veh_ids[i]);
	else
	    mvprintw(START_ENR_ROW,START_ENR_COL+(i*TO_NEXT_VEH),"     ");
    }

    j = k = 0;								/* display backup zones */
    for(i=0; i<BCK_IN_ZN_RESP; i++) {

        if (zrp->bkup_zones[i][0])
	  mvprintw(START_BCK_ROW+k,START_BCK_COL+(j*TO_NEXT_COL),"%03d",zrp->bkup_zones[i][0]);
	else
	  mvaddstr(START_BCK_ROW+k,START_BCK_COL+(j*TO_NEXT_COL),"   ");

	if (zrp->bkup_zones[i][1])
	  mvprintw(START_BCK_ROW+k,START_BCK_COL+(j*TO_NEXT_COL)+4,"%2d",zrp->bkup_zones[i][1]);
	else
	  mvaddstr(START_BCK_ROW+k,START_BCK_COL+(j*TO_NEXT_COL)+4,"   ");

	if (zrp->bkup_zones[i][2])
	  mvprintw(START_BCK_ROW+k,START_BCK_COL+(j*TO_NEXT_COL)+7,"%2d",zrp->bkup_zones[i][2]);
	else
	  mvaddstr(START_BCK_ROW+k,START_BCK_COL+(j*TO_NEXT_COL)+7,"   ");

	if (zrp->bkup_zones[i][3])
	  mvprintw(START_BCK_ROW+k,START_BCK_COL+(j*TO_NEXT_COL)+10,"%2d",zrp->bkup_zones[i][3]);
	else
	  mvaddstr(START_BCK_ROW+k,START_BCK_COL+(j*TO_NEXT_COL)+10,"   ");

	if (++k >= COLS_IN_ZBCKUP) {
	    k = 0;
	    j++;
	}
    }
    attrset(0);								/* normal attributes */
    return(0);
}



/***********************************************************************************************************************************
clear_zone_ detail -
This routine clears the detail data from the top of the zone status screen.
***********************************************************************************************************************************/
clear_zone_detail()
{
    int i,j,k;								/* temporary variables for loops, etc. */

    attrset(A_REVERSE | A_BOLD);					/* detail in bold reverse */
    mvprintw(2,0,"    ");
    mvprintw(2,17,"    ");
    mvprintw(2,32,"    ");
    mvprintw(START_PST_ROW,0,"    ");
    mvprintw(START_ENR_ROW,0,"    ");
    for (i=0; i<VEH_IN_ZN_RESP; i++) {					/* output vehicles posted and enroute */
	    mvprintw(START_PST_ROW,START_PST_COL+(i*TO_NEXT_VEH),"     ");
	    mvprintw(START_ENR_ROW,START_ENR_COL+(i*TO_NEXT_VEH),"     ");
    }

    j = k = 0;								/* display backup zones */
    for(i=0; i<BCK_IN_ZN_RESP; i++) {
	mvprintw(START_BCK_ROW+k,START_BCK_COL+(j*TO_NEXT_COL),"   ");
	mvprintw(START_BCK_ROW+k,START_BCK_COL+(j*TO_NEXT_COL)+4,"  ");
	mvprintw(START_BCK_ROW+k,START_BCK_COL+(j*TO_NEXT_COL)+7,"  ");
	mvprintw(START_BCK_ROW+k,START_BCK_COL+(j*TO_NEXT_COL)+10,"  ");

	if (++k >= COLS_IN_ZBCKUP) {
	    k = 0;
	    j++;
	}
    }
    mvprintw(Z_ZONE_ROW,Z_ZONE_COL+6,"                    ");			/* clear old zone */
    attrset(0);								/* normal attributes */
    return(0);
}


/***********************************************************************************************************************************
new_fleet -
This routine sets the new current fleet if valid
***********************************************************************************************************************************/
new_fleet(c)
char c;									/* fleet id as entered by user */
{
    int i;								/* temporary variable */

#ifdef DEBUG
	sprintf(trace_str,"new_fleet - c=%c\n",c);
	TRACE(trace_str);
#endif

	if ((zs_chosen_fleet = find_findex(c)) == -1 ) {
		prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_11, "<fleet id>")); 			/* user entered invalid fleet id, trap user in field */
		return(0);
	}
	if (fleet[zs_chosen_fleet]->active != YES )  { 			/* check if fleet entered is active or not */
		prt_error(UI_NOT_ACTIVE,"<%s %c>", catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_12, "fleet"), c); 		/* inactive fleet */
		return(0);
	}
	if ((*ui_fleet_perms[zs_chosen_fleet])() == NOT_ALLOWED ) {	/* does user have permissions to work with this fleet */
		prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_13, "<fleet>")); 			/* no permission */
		return(0);						/* trap user in field */
	}

    	zs_ch_fleet = fleet[zs_chosen_fleet]->fleet_id;					/* and fleet id */
    	return(1);
}


/***********************************************************************************************************************************
new_zone -
This function sets the new current zone number if valid.
***********************************************************************************************************************************/
new_zone(z,buf)
int z;									/* new zone number */
char buf[];								/* buffer containing user's input */
{
	int i, rc;								/* temporary variable for loops */
	struct cisam_zn *zone_file_ptr;					/* for disk i/o */
	int time_of_day;
	short day_of_week;
	struct tm *tm;
	time_t dt_tm;
	float skew;
	short calc_lead_time;

#ifdef DEBUG
	sprintf(trace_str,"new_zone - z=%d\n",z);
	TRACE(trace_str);
#endif

	if (isdigit(buf[0]) == 0)								/* is first character OK? */
		z = -1;										/* no */

	for (i=0; i<CHARS_ZONE; i++) {								/* check fld for invalid chars */
		if ((isdigit(buf[i]) == 0) && (isspace(buf[i]) == 0) && (buf[i] != '\0'))	/* got one? */
		z = -1;										/* yes */
	}

	if ((z < ZONE_MAX) && (z > 0) && (fleet[zs_chosen_fleet]->zone_ptr[z] != NULL)) {	/* is it a valid zone? */
		zs_chosen_zone = z;								/* yes, set new current zone */
		zone_file_ptr = (struct cisam_zn *)req_buf;
		zone_file_ptr->fleet = zs_ch_fleet;
		zone_file_ptr->nbr = zs_chosen_zone;

		if (db(ZONE_FILE_ID,READ_KEYS,&zn_key1,ISEQUAL,0) < 0) {			/* invalid zone entered */
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_14, "<zone not in ZONE file>"));
			zs_chosen_zone = -1;							/* invalid zone, set no zone */
			return(0);
		}
    		attrset(A_REVERSE | A_BOLD);							/* set bold reverse */
		if (0)
		  {
		    dt_tm = time(0);
		    tm = localtime( &dt_tm );
		    day_of_week = (Holiday_is_holiday( tm, offset ) ) ? RES_HOLIDAY_IDX : tm->tm_wday;
		    day_of_week = tm->tm_wday;
		    time_2_time_of_day( time(0), &time_of_day );
		    if ( ( rc = RES_get_period( day_of_week, time_of_day, &skew, offset ) ) < 0 )
		      skew = 1.0;
		    calc_lead_time = ( short )( ( zone_file_ptr->pckup_lead_time * skew ) + .000001 );
		    calc_lead_time += fleet[zs_chosen_fleet]->time_calls_lead_supp;                     /** Add the fleet supplement **/
		    mvprintw(Z_ZONE_ROW,Z_ZONE_COL+6,"                            ");			/* clear old zone */
		    mvprintw(Z_ZONE_ROW,Z_ZONE_COL+6,"%s (%2d MIN) ",
			     zone_file_ptr->desc, calc_lead_time);			/* clear old zone */
		  }
		else
		  {
		    mvprintw(Z_ZONE_ROW,Z_ZONE_COL+6,"                    ");			/* clear old zone */
		    mvprintw(Z_ZONE_ROW,Z_ZONE_COL+6,"%s ",
			     zone_file_ptr->desc);			/* clear old zone */
		  }
    		attrset(0);									/* set bold reverse */
		return(1);
	}
	else {
    		attrset(A_REVERSE | A_BOLD);							/* set bold reverse */
		mvprintw(Z_ZONE_ROW,Z_ZONE_COL+6,"                    ");			/* clear old zone */
    		attrset(0);									/* set bold reverse */
		zs_chosen_zone = -1;								/* invalid zone, set no zone */
		prt_error(UI_INVALID_NUM,"<%s>", catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_15, "zone"));
		return(0);
    }
}


