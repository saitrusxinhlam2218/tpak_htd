static char sz__FILE__[]="@(#)ui_mesg.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_mesg.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:51:08
 * @(#)  Last delta: 12/2/94 at 17:56:59
 * @(#)  SCCS File: /taxi/sccs/s.ui_mesg.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <cursesX.h>
#include <sys/types.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "ex_key.h"
#include "call_entry.h"
#include "except.h"
#include "func_keys.h"
#include "mesg.h"
#include "screens.h"
#include "ui_def.h"
#include "ui_error.h"
#include "user.h"
#include "menu.h"
#include "srch.h"
#include "language.h"
#include "ui_strdefs.h"
#include "path.h"
#include "mad_error.h"
#include "enhance.h"
#include "switch_ext.h"

extern struct menus *cmenu[];				/* array of pointers to menus structure in * shared memory */
extern int map_array[];					/* to map scr_name to index into menus structures */
extern int uid;								/* users id # */
extern int def_fl_index;						/* fleet we are working with */

BOOLEAN  msg_box_active = FALSE;


/* * * * * * * * * * * * * * * * * * * * * * * *
*   Note: the until time for repeat messages   *
*   must be entered in 24 hour format now.     *
** * * * * * * * * * * * * * * * * * * * * * * */

char dtm_bf[15];							/* buffer for date and time */
int  cn_flg;								/* canned message flag */

extern struct fleets *fleet[];						/* fleet array in shared memory */
extern struct users *uiuser[];						/* user profile array in shared memory */
extern struct cisam_cl srch_calls[];					/* user profile array in shared memory */
extern int user_index;							/* index into user array for this user */
extern char scr_type;							/* current screen type */
extern int scr_name;							/* current screen name */
extern short end_edit_repeat;						/* global end flag */
extern short clear_err_flag;						/* global error flag */
extern WINDOW *srch_win;						/* search window used for messages */
extern WINDOW *msg_win;
extern char req_buf[];							/* request buffer for db calls */
extern char canmtxt[MAX_CANM+1][41];					/* canned message text array */

extern int help();							/* help function */
extern int global_msg_num;						/* for CALL_W_MSG function key processing */
short da_flag = 0;

extern int val_repeat(), val_fleet(), val_to1(), val_to2(),		/* validation routines */
	val_canned(), val_often(), check_rtime(),
	check_rdate(), val_text1(), val_text2(), val_drv_attr(), val_veh_attr();

struct scr_flds mesg_flds[] = {						/* screen field structure for user input fields */
	{0,0,1,1},
	{M_RPT_ROW,M_RPT_COL,M_RPT_SIZ,M_FLT},
	{M_FLT_ROW,M_FLT_COL,M_FLT_SIZ,M_TO1},
	{M_TO1_ROW,M_TO1_COL,M_TO1_SIZ,M_TO2},
	{M_TO2_ROW,M_TO2_COL,M_TO2_SIZ,M_CANM},
	{M_CANM_ROW,M_CANM_COL,M_CANM_SIZ,M_OFTN},
	{M_OFTN_ROW,M_OFTN_COL,M_OFTN_SIZ,M_UNDT},
	{M_UNDT_ROW,M_UNDT_COL,M_UNDT_SIZ,M_UNTM},
	{M_UNTM_ROW,M_UNTM_COL,M_UNTM_SIZ,M_TXT1},
	{M_TXT1_ROW,M_TXT1_COL,M_TXT1_SIZ,M_TXT2},
	{M_TXT2_ROW,M_TXT2_COL,M_TXT2_SIZ,M_DRVATTR},
	{M_DRVATTR_ROW,M_DRVATTR_COL,M_DRVATTR_SIZ,M_RPT}
};

int (* val_rout[]) () = {						/* array of validation routines */
	val_repeat,
	val_fleet,
	val_to1,
	val_to2,
	val_canned,
	val_often,
	check_rdate,
	check_rtime,
	val_text1,
	val_text2,
	val_drv_attr
};

extern short send_to_dis_flag;
extern int (*security[])();
extern short multiple_call_flag;
short can_set;			/* was a canned messaged entered */
extern char *Spaces;

/* To keep track of read messages */
int Size_of_list = 0;
int Elements_in_list = 0;
struct read_message_struct
{
  int  msg_num;
  int  deleted;		/* 1 if deleted, 0 otherwise */
} *Message_list = NULL;


/***********************************************************************************************************************************
mesgs -
This routine is called when the MESSAGE function key is pressed.  It will display an empty message screen in the lower portion
of the terminal screen for creating messages.
***********************************************************************************************************************************/
mesgs(except_highlighted, except_ptr)
int except_highlighted;
struct excepts *except_ptr[];
{
	char	temp_buf[12];
    	char mbuf[READ_BUF_SIZE];					/* buffer for read field */
    	char c;								/* temporary variable to hold character */
    	int key;							/* key for read_field */
    	int i,j;							/* temporary variables for loops */
    	int cancel = 0;							/* flag to end while loop */
    	int msg_piu = M_TO2;						/* piu value */
    	short canned_msg=0;						/* 1=canned message number entered */
    	struct cisam_rm rmesg;						/* repeat message structure to build messge */
    	int should_clear = 0;
    	short callout_exception = 0;					/* 1=callout processing, 0=regular message */
    	int callout_return_code = -1;					/* -1=did not send message, 0=sent message */
	char date_str[10];						/* for formatting date */
	int  failed;
	int  old_scr_name;
	int dup = 0; 
	char rptsave;
	short MaxAttrs;
	int   VehAttrCol = 0, DrvAttrCol = 0;

	refresh();

	old_scr_name = scr_name;
	scr_name = MESSAGE_WIN_SCR;
    	if(except_highlighted != NULL)
		callout_exception = 1;					/* this is a message to driver from callout exc */
	can_set = 0;
    	msg_win_disp();							/* display message window */
    	prt_entry_areas(msg_win,mesg_flds,MAX_PIU_MESG);			/* fill in user entry areas with underscores */
    	wattrset(msg_win,A_UNDERLINE);
    	mvwaddch(msg_win,M_RPT_ROW,M_RPT_COL,NO);
        mvwprintw(msg_win, M_DRVATTR_ROW, M_DRVATTR_COL, "  ");
	
	bzero(&rmesg, sizeof(rmesg));
	rmesg.repeat = NO;
	strcpy(rmesg.veh_attr, "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN");
	strcpy(rmesg.drv_attr, "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN");
	rmesg.veh_attr_set = NO;
	rmesg.drv_attr_set = NO;

        if ( send_to_dis_flag || multiple_call_flag) {
	    	mvwaddch(msg_win,M_TO1_ROW,M_TO1_COL,' ');
		rmesg.to_type = ' ';
	}
    	else {
	        /* Managers and dispatchers default sending
		   to Vehicles, Call takers to Users */
	        if (uiuser[user_index]->menu_perms.exceptions || uiuser[user_index]->menu_perms.system_maint)
		  {
		    mvwaddch(msg_win, M_TO1_ROW, M_TO1_COL, MSG_VEHICLE);
		    rmesg.to_type = MSG_VEHICLE;
		  }
		else
		  {
		    mvwaddch(msg_win, M_TO1_ROW, M_TO1_COL, MSG_USER);
		    rmesg.to_type = MSG_USER;
		  }
	}

    	if(callout_exception)
          {
            if (except_ptr[except_highlighted-1]->fl_ptr != NULL)
              {
                mvwaddch(msg_win,M_FLT_ROW,M_FLT_COL,(char)((except_ptr[except_highlighted-1]->fl_ptr)->fleet_id));
                rmesg.fleet = (char)((except_ptr[except_highlighted-1]->fl_ptr)->fleet_id);
              }
            else
              {
                mvwaddch(msg_win,M_FLT_ROW,M_FLT_COL,fleet[def_fl_index]->fleet_id);
		rmesg.fleet = srch_calls[except_highlighted-1].fleet;
              }
            mvwaddch(msg_win,M_TO1_ROW,M_TO1_COL,MSG_VEHICLE);
            rmesg.to_type = MSG_VEHICLE;
            mvwprintw(msg_win,M_TO2_ROW,M_TO2_COL,"%4d",except_ptr[except_highlighted-1]->veh_nbr);
            sprintf(temp_buf, "%5d", except_ptr[except_highlighted-1]->veh_nbr);
            strcpy(rmesg.to_selection, temp_buf);
            msg_piu = M_CANM;									/* piu value */
    	}
    	else {
    		mvwaddch(msg_win,M_FLT_ROW,M_FLT_COL,fleet[def_fl_index]->fleet_id);
		rmesg.fleet = fleet[def_fl_index]->fleet_id;
	}


    wrefresh(msg_win);

    while (cancel == 0) {
	if(msg_piu == M_UNDT || msg_piu == M_UNTM)
		should_clear = 1;
	else
		should_clear = 0;
	
	if(msg_piu == M_UNDT)
		da_flag = TRUE; /* allow read_field to put in slashes for lazy operators -- Tony need to redesign */
        msg_box_active = TRUE;
	i = read_field(msg_win,mesg_flds,msg_piu,mbuf,&key,&should_clear);
	da_flag = FALSE;
	if(i == CANCEL_KEY || i == MESSAGE_KEY) {
		cancel = 1;
		callout_return_code = -1;			/* message not sent to driver */
		clear_error();
		continue;
	}

	if(enter_field(msg_piu,mbuf,&rmesg) < 0)
		continue;


	if(msg_piu == 1)  {
		/* in the repeat msg field; YES entry allowed only for superervisor's */
		if(((*security[map_array[EXCEPTIONS_SCR]])() == NOT_ALLOWED ) && mbuf[0] == YES) {
			prt_error(UI_NOT_ALLOWED,
				  catgets(UI_m_catd, UI_1, UI_UI_MESG_1, "<repeat messages can only be entered by dispatchers>"));
			continue;
		}
	}  

	if (clear_err_flag) {
	    clear_error();
	    refresh();
	}

	switch (i) {
		case '\t':
		case KEY_DOWN:
		case '\r':
		case '\n':
			if (msg_piu == M_CANM)
			  {
			    wmove(msg_win,mesg_flds[M_RPT].prompt_row,mesg_flds[M_RPT].prompt_col);
			    if ((winch(msg_win) & 0x7f) == YES)
			      msg_piu = mesg_flds[msg_piu].tab_field;
			    else
			      {
				msg_piu = M_TXT1;
				canned_msg = 0;
				if(atoi(mbuf) > 0){
				  msg_piu = M_DRVATTR;
				   //msg_piu = M_TO1;
				   canned_msg = 1;
				   wattrset(msg_win, A_UNDERLINE);
				   wmove(msg_win, M_TXT1_ROW, M_TXT1_COL);
				   for (j = 0; j < mesg_flds[M_TXT1].max_chars; j++)
				     waddch(msg_win, ' ');
				   mvwaddstr(msg_win, M_TXT1_ROW, M_TXT1_COL, rmesg.line_1);
				   wattrset(msg_win, 0);
				   wrefresh(msg_win);
				}
				if (!strlen(rmesg.line_1))
				  {
				    wattrset(msg_win, A_UNDERLINE);
				    wmove(msg_win, M_TXT1_ROW, M_TXT1_COL);
				    for (j = 0; j < mesg_flds[M_TXT1].max_chars; j++)
				      waddch(msg_win, ' ');
				    wattrset(msg_win, 0);
				    wrefresh(msg_win);
				  }
			    }
			}
			else
			    msg_piu = mesg_flds[msg_piu].tab_field;
			break;

		case KEY_UP:
			if (msg_piu == M_TXT1) {
			    wmove(msg_win,mesg_flds[M_RPT].prompt_row,mesg_flds[M_RPT].prompt_col);
			    if ((winch(msg_win) & 0x7f) == YES)
				msg_piu -= 1;
			    else
				msg_piu = M_CANM;
			}
			else if ((msg_piu -= 1) < 1)
				if(canned_msg)
				   msg_piu = M_CANM;
				else
			    	   msg_piu = MAX_PIU_MESG;
			break;

		      case ALTERNATE_KEY:
#ifdef FOO
			if ( msg_piu == M_VEHATTR )
			  {
			    if ( ( rmesg.to_selection[0] != '*' ) && ( rmesg.to_type != MSG_ZONE ) )
			      {
				prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_64,
							     "Attribute Filters only valid for Broadcast Messages"));
				break;
			      }
			    if ( ( rmesg.to_type != MSG_VEHICLE ) && ( rmesg.to_type != MSG_ZONE ) )
			      {
				prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_67,
						     "Vehicle Attribute Filters only valid for Vehicle Messages"));
				break;
			      }
                            alt_attr();
                            //			    attr_select( SELECT_VEHICLE_ATTR, find_findex(rmesg.fleet), rmesg.veh_attr );
			    msg_win_disp();			    
			    prt_entry_areas(msg_win,mesg_flds,MAX_PIU_MESG);			    
			    refresh_msg_data( &rmesg );
			  }
			else if ( msg_piu == M_DRVATTR )
			  {
			    if ( ( rmesg.to_selection[0] != '*' ) && ( rmesg.to_type != MSG_ZONE ) )
			      {
				prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_64,
						     "Attribute Filters only valid for Broadcast Messages"));
				break;
			      }
			    if ( ( rmesg.to_type != MSG_DRIVER ) && ( rmesg.to_type != MSG_ZONE ) )
			      {
				prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_66,
						     "Driver Attribute Filters only valid for Driver Messages"));
				break;
			      }
                            alt_attr();
                            //			    attr_select( SELECT_DRIVER_ATTR, find_findex(rmesg.fleet), rmesg.drv_attr );
			    msg_win_disp();			    
			    prt_entry_areas(msg_win,mesg_flds,MAX_PIU_MESG);			    
			    refresh_msg_data( &rmesg );
			  }
			
			VehAttrCol = M_VEHATTR_COL + 2;
			DrvAttrCol = M_DRVATTR_COL + 2;
			if (1)
			  MaxAttrs = ATTR_MAX;
			else
			  MaxAttrs = 8;
			rmesg.veh_attr_set = NO;
			rmesg.drv_attr_set = NO;
			for ( i = 0; i < MaxAttrs; i++ )
			  {
			    if ( rmesg.veh_attr[i] == YES )
			      {
				rmesg.veh_attr_set = YES;
				if ( fleet[ find_findex(rmesg.fleet) ]->vd_attr_info[i].veh_attr != '\0' )
				  {
				    mvwaddstr( msg_win, M_VEHATTR_ROW, VehAttrCol,
					      fleet[find_findex(rmesg.fleet)]->vd_attr_info[i].veh_attr );
				    VehAttrCol += 3;
				    if ( VehAttrCol >= M_VEHATTR_COL + 12 )
				      break;
				  }
			      }
			    if ( rmesg.drv_attr[i] == YES )
			      {
				rmesg.drv_attr_set = YES;
				if ( fleet[ find_findex(rmesg.fleet) ]->vd_attr_info[i].drv_attr != '\0' )
				  {
				    mvwaddstr( msg_win, M_DRVATTR_ROW, DrvAttrCol,
					      fleet[find_findex(rmesg.fleet)]->vd_attr_info[i].drv_attr );
				    DrvAttrCol += 3;
				    if ( DrvAttrCol >= M_DRVATTR_COL + 12 )
				      break;
				  }
			      }			    
			  }
			wattrset( msg_win, A_UNDERLINE );
			mvwaddch( msg_win, M_VEHATTR_ROW, M_VEHATTR_COL, rmesg.veh_attr_set );
			mvwaddch( msg_win, M_DRVATTR_ROW, M_DRVATTR_COL, rmesg.drv_attr_set );
			wattrset( msg_win, 0 );
			wrefresh( msg_win );
#endif
			break;
			
		case SEND_DUP_KEY:
			    dup = 1;
		case SEND_KEY:
			    /* Repeat messages must have a value for how often */
			    if (rmesg.repeat == YES &&
				rmesg.how_often == 0)
			      {
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_MESG_17, " <Often>"));
				msg_piu = M_OFTN;
				break;
			      }

			    /* If there's something in the to field, it must be
			       either a '*' or a number. */
			    if ( !(1) )
			      {
				if (strcmp(rmesg.to_selection, "*") != 0) {
				  failed = 0;
				  for (i = 0; i < 6 && rmesg.to_selection[i] && rmesg.to_selection[i] != ' '; i ++)
				    if (!isdigit(rmesg.to_selection[i])) {
				      failed = 1;
				      break;
				    }
				  if (failed) {
				    prt_error(UI_NOFIELD, "<%s>", catgets(UI_m_catd, UI_1, UI_UI_MESG_31,
									  "Must enter a number or a '*'"));
				    msg_piu = M_TO2;
				    break;
				  }
				}
			      }

			    if (rmesg.fleet == '*')
			      {
				if ((rmesg.to_type == MSG_DRIVER ||
				     rmesg.to_type == MSG_VEHICLE)
				    && rmesg.to_selection[0] != '*')
				  {
				    prt_error(UI_NO_MSG,
					      catgets(UI_m_catd, UI_1, UI_TEXT_22,
						      "If sending to a single driver or taxi, you must use a single fleet"));
				    msg_piu = M_FLT;
				    break;
				  }
			      }

			    if (rmesg.to_type == MSG_ZONE &&
				rmesg.to_selection[0] == '*' && ( !(1) ))
			      {
				prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_23, "Must specify a single zone"));
				msg_piu = M_TO2;
				break;
			      }

			    rmesg.created_by = uid;
			    rptsave = rmesg.repeat;
			    if (message_send(&rmesg) == -1)
			    {
				rmesg.repeat = rptsave;
				break;
			    }
			    rmesg.repeat = rptsave;

			    if (!dup)
			    {
			    if (!clear_err_flag) {
			        bzero(&rmesg, sizeof(rmesg));
				rmesg.repeat = NO;
				rmesg.veh_attr_set = NO;
				rmesg.drv_attr_set = NO;
                                mvwaddstr( msg_win, M_DRVATTR_ROW+1, M_DRVATTR_COL + 2, "                        " ); 
				
				rmesg.to_type = MSG_VEHICLE;
				get_pkup_date(rmesg.until_date);
				rmesg.fleet = fleet[def_fl_index]->fleet_id;

				prt_entry_areas(msg_win,mesg_flds,MAX_PIU_MESG);
    				wattrset(msg_win,A_UNDERLINE);
    				mvwaddch(msg_win,M_RPT_ROW,M_RPT_COL,NO);
				mvwaddch(msg_win,M_TO1_ROW,M_TO1_COL,MSG_VEHICLE);
    				mvwaddch(msg_win,M_FLT_ROW,M_FLT_COL, rmesg.fleet);

				strcpy(rmesg.veh_attr, "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN");
				strcpy(rmesg.drv_attr, "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN");
				rmesg.veh_attr_set = NO;
				rmesg.drv_attr_set = NO;
    				wattrset(msg_win,0);
			    }
			    }
			    else dup = 0;
			    msg_piu = M_TO2;

                        if (send_to_dis_flag || multiple_call_flag)     /* called from send_to_dispatcher() fucntion */
				cancel = 1;

			bump_stat(rmesg.fleet,SENDING_MSG);		/* increment count of # of msgs send by this user */
			
			if (callout_exception) {			/* called from exception screen w-callout exception */
				cancel = 1;
				callout_return_code = 0;		/* message sent to driver */
			}

			if (!cancel)
				prt_error(UI_REC_UPDATED,catgets(UI_m_catd, UI_1, UI_UI_MESG_2, "<message sent>"));

			break;

		case COMMAND_KEY:
		case CANCEL_KEY:
		case MESSAGE_KEY:
                  msg_box_active = FALSE;
			cancel = 1;
			callout_return_code = -1;			/* message not sent to driver */
			break;

		case HELP_KEY:
			help(MESSAGE_WIN_SCR, msg_piu);
			/* redraw the srch_win */
			touchwin(msg_win);
			wrefresh(msg_win);
			break;

		case -1:
			break;

		case PRINT_SCR_KEY:	/* print screen image with window  overlay */
			print_scr(msg_win);
			break;

		case '\014':					/* control L .. refresh screen  if messed up */
			wrefresh(msg_win);			/* This is done by read_field() too. read_field */
			break;					/* will never return a ^L. */

		default:
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1, "<wrong function key pressed>"));
			break;
	}
    }
    wclear(msg_win);
    wrefresh(msg_win);
    touchwin(stdscr);
    refresh();
    scr_name = old_scr_name;
    if (callout_exception)
	return (callout_return_code);
}



/***********************************************************************************************************************************
***********************************************************************************************************************************/
msg_win_disp()
{
    werase(msg_win);

    wattrset(msg_win, 0);
/*     mvwaddstr(msg_win,0,0," "); */
    mvwaddstr(msg_win,1,1,catgets(UI_m_catd, UI_1, UI_UI_MESG_3, "Repeat [Y/N]"));
    mvwaddstr(msg_win,1,20,catgets(UI_m_catd, UI_1, UI_UI_MESG_4, "Fleet"));
    mvwaddstr(msg_win,1,28,catgets(UI_m_catd, UI_1, UI_UI_MESG_5, "   To"));
    mvwaddstr(msg_win,1,44,catgets(UI_m_catd, UI_1, UI_UI_MESG_6, "Canned  #"));
    mvwaddstr(msg_win,1,60,catgets(UI_m_catd, UI_1, UI_UI_MESG_7, "How Often"));
    mvwaddstr(msg_win,1,79,catgets(UI_m_catd, UI_1, UI_UI_MESG_8, "Until"));
    mvwaddstr(msg_win, 1, 85, catgets(UI_m_catd, UI_1, UI_UI_MESG_9, "Date"));
    mvwaddstr(msg_win, 1, 100, catgets(UI_m_catd, UI_1, UI_UI_MESG_10, "Time"));
    mvwaddstr(msg_win,2,1,catgets(UI_m_catd, UI_1, UI_UI_MESG_11, "Message Text"));

    mvwaddstr(msg_win, 4, 1, catgets(UI_m_catd, UI_1, UI_UI_DISP_S_18, "Car Needs"));
    mvwaddstr(msg_win, 4, M_DRVATTR_COL + 2, "             ");

    mvwaddstr(msg_win, 6, 1,
	      catgets(UI_m_catd, UI_1, UI_UI_MESG_29,
		      "To: * - Everyone, U - User, S - Dispatcher, C - Call Taker, M - Manager, T - Vehicle, D - Driver, Z - Drivers in a Zone"));
    tpak_box( msg_win, 0, 0 );
}


/***********************************************************************************************************************************
***********************************************************************************************************************************/
enter_field(piu,buf,rmsg_pt)
int piu;
char buf[];
struct cisam_rm *rmsg_pt;
{
    int i,j;
    int x;

    j = strlen(buf);
    for(i=0; i<j; i++) {
	if (buf[i] == '_')
	    buf[i] = ' ';
    }

    x = (*(val_rout[piu-1]))(piu,buf,rmsg_pt);
    return(x);
}

/********************************************************************************************/
/* check_rdate - verify date field							    */
/********************************************************************************************/
int 
check_rdate(piu, buf, rmsg_pt)
int piu;						/* pointer to piu */
char buf[];						/* pointer to buffer for reading field */
struct cisam_rm *rmsg_pt;
{
  char date_str[DATE_SIZE];

	if((verify_date(buf, date_str)) == -1) {
		prt_error(BAD_DATE,catgets(UI_m_catd, UI_1, UI_UI_MESG_12, "<format is mm/dd/yy>"));
		return(-1);
	}

        strcpy(rmsg_pt->until_date, date_str);

	rmsg_pt->until_dt_tm = get_internal_time(rmsg_pt->until_date, rmsg_pt->until_time);

    	wattrset(msg_win,A_UNDERLINE);
	mvwaddstr(msg_win, M_UNDT_ROW, M_UNDT_COL, rmsg_pt->until_date);
    	wattrset(msg_win,0);
    	wrefresh(msg_win);

	return(1);
}

/********************************************************************************************/
/* check_rtime - verify time field							    */
/********************************************************************************************/
int 
check_rtime(piu, buf, rmsg_pt)
int piu;						/* pointer to piu */
char buf[];						/* pointer to buffer for reading field */
struct cisam_rm *rmsg_pt;
{
  char time_str[TIME_SIZE];

	if((verify_time(buf, time_str)) == -1) {
		prt_error(BAD_TIME,catgets(UI_m_catd, UI_1, UI_UI_MESG_13, "<format is HH:MM>"));
		return(-1);
	}

        strcpy(rmsg_pt->until_time, time_str);

	rmsg_pt->until_dt_tm = get_internal_time(rmsg_pt->until_date, rmsg_pt->until_time);

    	wattrset(msg_win,A_UNDERLINE);
	mvwaddstr(msg_win, M_UNTM_ROW, M_UNTM_COL, rmsg_pt->until_time);
    	wattrset(msg_win,0);
    	wrefresh(msg_win);

	return(1);
}

val_repeat(piu,buf,rmsg_pt)
int piu;
char buf[];
struct cisam_rm *rmsg_pt;
{
  if (buf[0] == YES && rmsg_pt->repeat != YES)
    {

      /* The following don't allow repeat messages */
      if (rmsg_pt->to_type == MSG_USER ||
	  rmsg_pt->to_type == MSG_CALL_TAKER ||
	  rmsg_pt->to_type == MSG_MANAGER ||
	  rmsg_pt->to_type == MSG_SUPERVISOR ||
	  rmsg_pt->to_type == '*')
	{
	  prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_13, "Repeat messages not allowed for chosen recepient"));
	  return(-1);
	}

      rmsg_pt->repeat = YES;
      
      /* Repeat for today */
      get_pkup_date(rmsg_pt->until_date);
      
      /* Until midnight */
      strcpy(rmsg_pt->until_time, "23:59");

      rmsg_pt->until_dt_tm = get_internal_time(rmsg_pt->until_date, rmsg_pt->until_time);

      wattrset(msg_win, A_UNDERLINE);
      mvwaddstr(msg_win, M_UNDT_ROW, M_UNDT_COL, rmsg_pt->until_date);
      mvwaddstr(msg_win, M_UNTM_ROW, M_UNTM_COL, rmsg_pt->until_time);
    }
  else if (buf[0] == NO && rmsg_pt->repeat != NO)
    {
      rmsg_pt->repeat = NO;
      
      wattrset(msg_win, A_UNDERLINE);
      clear_field(msg_win, mesg_flds, M_UNDT);
      clear_field(msg_win, mesg_flds, M_UNTM);
    }
  else if (buf[0] != YES && buf[0] != NO)
    {
      prt_error(UI_NOFIELD," <%s %c>", catgets(UI_m_catd, UI_1, UI_UI_MESG_14, "Repeat"), buf[0]);
      return(-1);
    }
  wattrset(msg_win,0);
  return(1);
}

val_fleet(piu,buf,rmsg_pt)
int piu;
char buf[];
struct cisam_rm *rmsg_pt;
{
    int i;

   if (buf[0] == '*')
     {
       rmsg_pt->fleet = '*';
       return(1);
     }
    for(i=0; i<FLEET_MAX; i++)						/* try to find fleet in shared memory */
	if (fleet[i]->fleet_id == buf[0] && fleet[i]->active == YES) {
	    rmsg_pt->fleet = buf[0];
	    return(1);							/* yes */
        }
    prt_error(UI_NOFLEET,"<%c>",buf[0]);				/* no, print error */
    return(-1);
}

val_drv_attr(piu, read_buf, rmsg_ptr)
     int piu;
     char read_buf[];
     struct cisam_rm *rmsg_ptr;
{
  BOOLEAN found_it;
  int           i, fleet_index = 0;
  int pos = 0, attr_row, attr_col;

  attr_row = M_DRVATTR_ROW;
  attr_col = M_DRVATTR_COL + 2;
  
  if ( !strncmp(read_buf, "  ", 2) )
    return(1);

  for(i=0; i<FLEET_MAX; i++) {
    if (fleet[i]->fleet_id == rmsg_ptr->fleet) {
      fleet_index = i;
      break;
    }
  }

  mvwprintw(msg_win, attr_row + 1, attr_col, "                                  ");

  if (!strncmp(read_buf, "XX", 2))
    {
      rmsg_ptr->veh_attr_set = NO;
      rmsg_ptr->drv_attr_set = NO;
      for ( i = 0; i < ATTR_MAX; i++ )
        {
          rmsg_ptr->veh_attr[i] = NO;
          rmsg_ptr->drv_attr[i] = NO;
        }
    }
  
  for ( i = 0; i < ATTR_MAX; i++ )
    {
      if ((!strncmp(fleet[fleet_index]->vd_attr_info[i].veh_attr, read_buf, 2)) &&
               (strlen(read_buf)) &&
               (fleet[fleet_index]->vd_attr_info[i].veh_attr[0] != EOS))        
        {
          found_it = TRUE;
          if ( rmsg_ptr->veh_attr[i] == YES )
            rmsg_ptr->veh_attr[i] = NO;
          else
            {
              rmsg_ptr->veh_attr[i] = YES;
              rmsg_ptr->veh_attr_set = YES;
            }
        }
      
      if ((!strncmp(fleet[fleet_index]->vd_attr_info[i].drv_attr, read_buf, 2)) &&
               (strlen(read_buf)) &&
               (fleet[fleet_index]->vd_attr_info[i].drv_attr[0] != EOS))        
        {
          found_it = TRUE;
          if ( rmsg_ptr->drv_attr[i] == YES )
            rmsg_ptr->drv_attr[i] = NO;
          else
            {
              rmsg_ptr->drv_attr[i] = YES;
              rmsg_ptr->drv_attr_set = YES;
            }
        }
      if ( rmsg_ptr->drv_attr[i] == YES )
        {
          mvwprintw(msg_win, attr_row + 1, attr_col + pos, "%s ", fleet[fleet_index]->vd_attr_info[i].drv_attr);
          pos += 3;
        }
      if ( rmsg_ptr->veh_attr[i] == YES )
        {
          mvwprintw(msg_win, attr_row + 1, attr_col + pos, "%s ", fleet[fleet_index]->vd_attr_info[i].veh_attr);
          pos += 3;
        }                      
    }
  if (read_buf[0] == EOS)
    {
      return(-1);
    }
  else
    {
      mvwprintw(msg_win, M_DRVATTR_ROW, M_DRVATTR_COL, "  ");
      return(-1);
    }
}

val_veh_attr(piu, buf, rmsg_pt)
     int piu;
     char buf[];
     struct cisam_rm *rmsg_pt;
{
  
  return(1);
}

val_to1(piu,buf,rmsg_pt)
int piu;
char buf[];
struct cisam_rm *rmsg_pt;
{
    char no[2];

    if (scr_name == EDIT_REPEAT_MESSAGES)
      {
	/* The following don't allow repeat messages */
	if (buf[0] == MSG_USER ||
	    buf[0] == MSG_CALL_TAKER ||
	    buf[0] == MSG_MANAGER ||
	    buf[0] == MSG_SUPERVISOR ||
	    buf[0] == '*')
	  {
	    prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_13, "Repeat messages not allowed for chosen recepient"));
	    return(-1);
	  }
      }

    if (buf[0] == '*' ||
	buf[0] == MSG_USER ||
	buf[0] == MSG_CALL_TAKER ||
	buf[0] == MSG_MANAGER ||
	buf[0] == MSG_VEHICLE ||
	buf[0] == MSG_DRIVER ||
	buf[0] == MSG_ZONE ||
	buf[0] == MSG_SUPERVISOR) {

	rmsg_pt->to_type = buf[0];

	/* The following don't allow repeat messages, change repeat field to NO */
	if (rmsg_pt->repeat == YES &&
	    (buf[0] == MSG_USER ||
	     buf[0] == MSG_CALL_TAKER ||
	     buf[0] == MSG_MANAGER ||
	     buf[0] == MSG_SUPERVISOR ||
	     buf[0] == '*'))
	  {
	    no[0] = NO;
	    no[1] = '\0';
	    val_repeat(piu, no, rmsg_pt);

	    /* Display 'N' */
	    wattrset(msg_win,A_UNDERLINE);
	    mvwaddch(msg_win, M_RPT_ROW, M_RPT_COL, NO);
	    prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_13, "Repeat messages not allowed for chosen recepient"));
	    return(-1);
	  }

	/* '*' is the only valid value for to_selection for '*', CALL_TAKER, MANAGER, SUPERVISOR */
	if (buf[0] == '*' ||
	    buf[0] == MSG_CALL_TAKER ||
	    buf[0] == MSG_MANAGER ||
	    buf[0] == MSG_SUPERVISOR)
	  {
	    strcpy(rmsg_pt->to_selection, "*");
	    wattrset(msg_win, A_UNDERLINE);
	    mvwprintw(msg_win, M_TO2_ROW, M_TO2_COL, "%-4s", rmsg_pt->to_selection);
	  }

	return(1);
    }
    else {
	prt_error(UI_REQENTRY, "");
	return(-1);
    }
}

val_to2(piu,buf,rmsg_pt)
int piu;
char buf[];
struct cisam_rm *rmsg_pt;
{
    int i;
    char *cptr;

    /* Allow empty field (but must be filled for send) */
    if (strncmp(buf, "     ", 6) == 0)
	return(1);

    /* If there's something in the to field, it must be
       either a '*' or a number. */

    /* If the user enters an * it needs to be at the start of the line  */
    /*                                                                  */
    cptr = strchr(buf, '*');
    if ( ( cptr && strlen(buf) != 1) && ( rmsg_pt->to_type != MSG_ZONE ) )
    {
        prt_error(UI_NOFIELD, "<%s>", catgets(UI_m_catd, UI_1, UI_UI_MESG_31, "Must enter a number or a '*'"));
        return(-1);
    }

    if (buf[0] != '*')
      {
	/* '*' is the only valid value for to_selection for '*', CALL_TAKER, MANAGER, SUPERVISOR */
	if (rmsg_pt->to_type == '*' ||
	    rmsg_pt->to_type == MSG_CALL_TAKER ||
	    rmsg_pt->to_type == MSG_MANAGER ||
	    rmsg_pt->to_type == MSG_SUPERVISOR)
	  {
	    prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_24,
					 "Must enter a '*' for calls to *, Dispatchers, Call Takers, or Managers"));
	    return(-1);
	  }

      for (i = 0; i < 6 && buf[i] && buf[i] != ' '; i ++)
	{
	  if (( !isdigit(buf[i]) ) && ( rmsg_pt->to_type != MSG_ZONE ))
	    {
	      prt_error(UI_NOFIELD, "<%s>", catgets(UI_m_catd, UI_1, UI_UI_MESG_31, "Must enter a number or a '*'"));
	      return(-1);
	    }
	}
      }

    else if (rmsg_pt->to_type == MSG_ZONE)
      {
	prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_23, "Must specify a single zone"));
	return(-1);
      }


    strncpy(rmsg_pt->to_selection,buf,6);
    rmsg_pt->to_selection[6] = '\0';
    return(1);
}

val_canned(piu,buf,rmsg_pt)
int piu;
char buf[];
struct cisam_rm *rmsg_pt;
{
    int x;
    struct cisam_cm *cm_ptr;

    cm_ptr = (struct cisam_cm *)req_buf;

    if ((buf[0] == ' ') || (buf[0] == '_')) {
	can_set = 0;	/* no canned message to send */
	cn_flg = 0;
    }
    else {
	if (((x = atoi(buf)) < 0) || (x > MAX_CANM)) {
	    prt_error(UI_NOFIELD," <%s %s>", catgets(UI_m_catd, UI_1, UI_UI_MESG_15, "Canned mesg #"), buf);
	    return(-1);
	}
	else
	  {
	    if (strlen(&canmtxt[x][0]) == 0) {
	      prt_error(UI_NOFIELD," <%s %s>", catgets(UI_m_catd, UI_1, UI_UI_MESG_16, "Unable to read canned mesg #"), buf);
	      return(-1);
	    }
	    can_set = 1;		/* canned message to send */
	    strcpy(rmsg_pt->line_1,canmtxt[x]);
	    rmsg_pt->line_2[0]='\0';
	    cn_flg = 1;
	  }

    }
    return(1);
}

val_often(piu,buf,rmsg_pt)
int piu;
char buf[];
struct cisam_rm *rmsg_pt;
{
    	int i;

    	rmsg_pt->how_often = atoi(buf);
    	for(i=0; i<mesg_flds[piu].max_chars; i++) {
		if (buf[i] == ':') {
	    		rmsg_pt->how_often *= 60;
	    		rmsg_pt->how_often += atoi(&buf[i+1]);
		}
    	}

	if(rmsg_pt->how_often == 0) {
	    prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_MESG_17, " <Often>"));
	    return(-1);
	}

    	return(1);
}

val_text1(piu,buf,rmsg_pt)
int piu;
char buf[];
struct cisam_rm *rmsg_pt;
{
  if (strlen(buf) > M_TXT1_SIZ)
    buf[M_TXT1_SIZ] = '\0';
  
  strcpy(rmsg_pt->line_1,buf);
  return(1);
}

val_text2(piu,buf,rmsg_pt)
int piu;
char buf[];
struct cisam_rm *rmsg_pt;
{
  if (strlen(buf) > M_TXT2_SIZ)
    buf[M_TXT2_SIZ] = '\0';
  
  strcpy(rmsg_pt->line_2, buf);
  return(1);
}

/***********************************************************************
Below are the functions for reading messages.
************************************************************************/

void
reset_message_list()
{
  Size_of_list = 0;
  Elements_in_list = 0;
  if (Message_list)
    free(Message_list);
  Message_list = NULL;
}

void
add_to_read_message_list(msg_num)
int  msg_num;
{
  int  i;

  for (i = 0; i < Elements_in_list; i ++)
    {
      /* If already in list, no need to add it */
      if (Message_list[i].msg_num == msg_num)
	return;
    }

  /* It's not in the list */
  Elements_in_list ++;

  /* If new list, allocate space for message numbers. Start with 5 */
  if (Message_list == NULL)
    {
      Size_of_list = 5;
      Message_list = (struct read_message_struct *) malloc(Size_of_list * sizeof(struct read_message_struct));
    }

  /* If we need more room, increase by 5 */
  else if (Elements_in_list > Size_of_list)
    {
      Size_of_list += 5;
      Message_list = (struct read_message_struct *) realloc((int *) Message_list,
							    Size_of_list * sizeof(struct read_message_struct));
    }
  
  Message_list[Elements_in_list - 1].msg_num = msg_num;
  Message_list[Elements_in_list - 1].deleted = 0;
}

change_status_of_read_messages()
{
  int  i;
  int  rc;
  int  update_count;

  update_count = 0;
  for (i = 0; i < Elements_in_list; i ++)
    {
      if (Message_list[i].deleted == 0)
	{
	  rc = go_to_message_number(Message_list[i].msg_num);
	  if (rc == 0)
	    update_count += update_current_message_to_old();
	}
    }

  reset_message_list();
  return(update_count);
}

mark_deleted_message_and_get_previous(msg_num, mb_ptr)
int  msg_num;
struct cisam_mb *mb_ptr;
{
  int  i, j;
  int  rc;
  int  prev_msg_num;
  int  no_prev_msg;

  prev_msg_num = -1;
  no_prev_msg = 0;

  /* Count through the read messages until
     we find the one we're looking for. The
     message numbers in Messsage_list will
     be in the order we want to see them. */
  for (i = 0; i < Elements_in_list, prev_msg_num == -1; i ++)
    {
      if (Message_list[i].msg_num == msg_num)
	{
	  /* Mark the message as deleted,
	     and find the previous undeleted
	     message */
	  Message_list[i].deleted = 1;
	  for (j = i - 1; j >= 0, prev_msg_num == -1; j --)
	      if (Message_list[j].deleted == 0)
		prev_msg_num = Message_list[j].msg_num;
	}
    }

  if (prev_msg_num == -1)
    no_prev_msg = 1;

  /* previous message found */
  else
    {
      /* Set pointer in file to previous message */
      rc = go_to_message_number(prev_msg_num);
      if (rc == -1)
	no_prev_msg = 1;

      /* Read the current message with the key we use
	 for reading in the proper order. req_buf
	 contains the data we want already. */
      else
	{
	  rc = read_message(mb_ptr, uid, ISEQUAL);
	  if (rc == -1)
	    no_prev_msg = 1;
	}
    }

  /* Get the first message if no previous one */
  if (no_prev_msg)
    {
      rc = get_first_message(mb_ptr, uid);
      if (rc == -1)
	return(-1);
    }
  return(0);
}

void
show_message(mb_ptr)
struct cisam_mb *mb_ptr;
{
  char status[20];

  if (mb_ptr->status == NEW_MSG)
    sprintf(status, "(%s)", catgets(UI_m_catd, UI_1, UI_TEXT_26, "NEW"));
  else
    sprintf(status, "(%s)", catgets(UI_m_catd, UI_1, UI_TEXT_27, "OLD"));

  mvwprintw(msg_win, 1, 2, "%s %d  %s %6s  %8s %s      ",
	    catgets(UI_m_catd, UI_1, UI_UI_MESG_18, "From"),
	    mb_ptr->from, catgets(UI_m_catd, UI_1, UI_UI_MESG_19, "at"),
	    mb_ptr->date_sent, mb_ptr->time_sent, status);
  mvwprintw(msg_win, 2, 2, "%-100s", mb_ptr->msg_line1);
  mvwprintw(msg_win, 3, 2, "%-100s", mb_ptr->msg_line2);

  /* Keep cursor at top right corner of window */
  wmove(msg_win, 0, 0);

  wrefresh(msg_win);

  add_to_read_message_list(mb_ptr->msg_nbr);
}

void
no_messages()
{
  werase(msg_win);
  tpak_box(msg_win, 0, 0);
  center_msg(msg_win, 2, catgets(UI_m_catd, UI_1, UI_TEXT_28, "No more messages"));
  wrefresh(msg_win);
}

void
tpak_read_messages()
{
  int i;
  int end_msg = 0;
  int key;
  int rc;
  struct cisam_mb mb;
  int num_new_messages_read;
  
  num_new_messages_read = 0;

  rc = get_first_message(&mb, uid);
  if (rc == -1)
    {
      prt_error(UI_NO_MSG,"%s %d", catgets(UI_m_catd, UI_1, UI_UI_MESG_24, "No mail messages for"), uid);
      return;
    }
  
  wclear(msg_win);
  tpak_box(msg_win, 0, 0);

  msg_box_active = TRUE;
  show_message(&mb);
  
  while (end_msg == 0)
    {
      /* Keep cursor at top right corner of
	 window after an error */
      if (clear_err_flag)
	{
	  wmove(msg_win, 0, 0);
	  wrefresh(msg_win);
	}

      key = wgetkey(msg_win);
      if (clear_err_flag)
	{
	  clear_error();
	  refresh();
	}
      
      switch (key)
	{
	case NEXT_PG_KEY:
	  rc = get_next_message(&mb, uid);
	  if (rc == -1)
	    prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_29, "No next mail message"));
	  else
	    show_message(&mb);
	  break;
	  
	case PREV_PG_KEY:
	  rc = get_previous_message(&mb, uid);
	  if (rc == -1)
	    prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_30, "No previous mail message"));
	  else
	    show_message(&mb);
	  break;
	  
	case DISAPPROVE_KEY:
	  rc = delete_current_message();
	  if (rc == 0)
	    {
	      prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_MESG_23, "<message deleted>"));
	      sleep(1);

	      /* Keep track of the new messages read
		 so we can change the number of messages
		 display on the screen */
	      if (mb.status == NEW_MSG)
		num_new_messages_read ++;

	      /* Go back to previous message after deletion */
	      rc = mark_deleted_message_and_get_previous(mb.msg_nbr, &mb);
	      if (rc == -1)
		{
		  prt_error(UI_NO_MSG,"%s %d", catgets(UI_m_catd, UI_1, UI_UI_MESG_24, "No mail messages for"), uid);
		  no_messages();
		  break;
		}
		show_message(&mb);
	    }
	  break;
	  
	case CANCEL_KEY:
	case RCV_MSG_KEY:
	case COMMAND_KEY:
	  end_msg = 1;
	  break;
	  
	case PRINT_SCR_KEY:
	  print_scr(msg_win);
	  break;

        case '\014':					/* control L .. refresh screen  if messed up */
          wrefresh(curscr);      
          break;			

	default:
	  break;
	}
    }

  num_new_messages_read += change_status_of_read_messages();
  change_message_count(uid, -num_new_messages_read);
  close_messages();

  werase(msg_win);
  msg_box_active = FALSE;
  
  wrefresh(msg_win);
  touchwin(stdscr);
  refresh();

}



/***********************************************************************************************************************************
edit_repeat_msgs -
This function is called when the menu item for repeat message edit has been selected.  It is similar to a combination of the
messge_send and message_recv functions for standard mailbox messages.
***********************************************************************************************************************************/
edit_repeat_msgs()
{
  struct cisam_rm *rm_ptr;
  struct cisam_rm rm;
  char rmbuf[READ_BUF_SIZE];
  int key_val;
  int field_num;
  int last_field_num;
  int should_clear = 0;
  int rc;
  int current_msg_num = -1;
  int field_entered;
  int failed;
  int i;
  
  end_edit_repeat = 0;							/* global end flag */

  field_num = M_TO2;
  bzero(&rm, sizeof(struct cisam_rm));

  rc = db(RPTMSG_FILE_ID, OPEN, 0, ISINOUT + ISMANULOCK , 0);
  if (rc < 0)
    {
      prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_MESG_28, "No repeat messages"));
      ERROR(' ',RPTMSG_FILE,"Error opening repeat mesg file");
      sleep(3);
      menu_manager(YES, SUPERVISORY_MENU);
      return;
    }

  rm_ptr = (struct cisam_rm *) req_buf;
  bzero(rm_ptr, sizeof(struct cisam_rm));
  rc = db(RPTMSG_FILE_ID, READ_KEYS, &rm_key1, ISFIRST, 0);
  if (rc < 0)
    {
      prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_MESG_28, "No repeat messages"));
      sleep(3);
      menu_manager(YES, SUPERVISORY_MENU);
      return;
    }
  
  /* Copy out of req_buf to avoid the any problems if any other calls are made to db */
  memcpy(&rm, rm_ptr, sizeof(struct cisam_rm));
	  
  current_msg_num = rm.msg_nbr;

  disp_screen_title();
  werase(msg_win);
  msg_win_disp();
  prt_entry_areas(msg_win, mesg_flds, MAX_PIU_MESG);
  display_repeat_message(&rm);

  while (!end_edit_repeat && scr_name == EDIT_REPEAT_MESSAGES)
    {
      if (field_num > MAX_PIU_MESG)
	field_num = M_FLT;
      else if (field_num < M_FLT)
	field_num = M_TXT2;

      if (field_num == M_UNDT)
	{
	  da_flag = TRUE;
	  should_clear = 1;
	}
      else if (field_num == M_UNTM)
	should_clear = 1;
      else
	should_clear = 0;

      /* The Repeat field should not be entered while editing */
      if (field_num == M_RPT)
	{
	  if (last_field_num == M_TXT2)
	    field_num = M_FLT;
	  else if (last_field_num == M_FLT)
	    field_num = M_TXT2;

	  /* Should never fall in to this case */
	  else
	    field_num = M_FLT;
	}

      last_field_num = field_num;
      key_val = read_field(msg_win, mesg_flds, field_num, rmbuf, &field_entered, &should_clear);
      da_flag = FALSE;
      if (current_msg_num >= 0)
	{
	  rc = enter_field(field_num, rmbuf, &rm);
	  if (rc < 0)
	    continue;
	}

      if (clear_err_flag)
	{
	  clear_error();
	  refresh();
	}
      
      switch (key_val)
	{
	case KEY_DOWN:
	case '\t':
	case '\r':
	case '\n':
	  field_num ++;	
	  break;
	  
	case KEY_UP:
	  field_num --;
	  break;
	  
	case PRINT_SCR_KEY:
	  print_scr(msg_win);
	  break;

	case SEND_KEY:
	  if (current_msg_num == -1)
	    break;

	  /* Repeat messages must have a value for how often */
	  if (rm.how_often == 0)
	    {
	      prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_MESG_17, " <Often>"));
	      field_num = M_OFTN;
	      break;
	    }
	  
	  /* If there's something in the to field, it must be
	     either a '*' or a number. */
	  if (strcmp(rm.to_selection, "*") != 0) {
	    failed = 0;
	    for (i = 0; i < 5 && rm.to_selection[i] && rm.to_selection[i] != ' '; i ++)
	      if (!isdigit(rm.to_selection[i])) {
		failed = 1;
		break;
	      }
	    if (failed) {
	      prt_error(UI_NOFIELD, "<%s>", catgets(UI_m_catd, UI_1, UI_UI_MESG_31,
						    "Must enter a number or a '*'"));
	      field_num = M_TO2;
	      break;
	    }
	  }
	  
	  if (rm.fleet == '*')
	    {
	      if ((rm.to_type == MSG_DRIVER ||
		   rm.to_type == MSG_VEHICLE)
		  && rm.to_selection[0] != '*')
		{
		  prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_22,
					       "If sending to a single driver or taxi, you must use a single fleet"));
		  field_num = M_FLT;
		  break;
		}
	    }
	  
	  if (rm.to_type == MSG_ZONE &&
	      rm.to_selection[0] == '*')
	    {
	      prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_23, "Must specify a single zone"));
	      field_num = M_TO2;
	      break;
	    }
	  
	  rm_ptr = (struct cisam_rm *)req_buf;

	  /* Lock the current record */
	  rc = db(RPTMSG_FILE_ID, READ_KEYS, &rm_key1, ISCURR + ISLOCK, 0);
	  if (rc < 0)
	    {
	      prt_error(UI_NOUPDATE, catgets(UI_m_catd, UI_1, UI_UI_MESG_25, "<repeat message file>"));
	      ERROR(' ',RPTMSG_FILE,"Error reading record again with lock");
	      break;
	    }

	  if (current_msg_num != rm_ptr->msg_nbr)
	    {
	      prt_error(UI_NOUPDATE, catgets(UI_m_catd, UI_1, UI_UI_MESG_25, "<repeat message file>"));
	      ERROR(' ',RPTMSG_FILE,"Error record not same as one read previously");
	      break;
	    }

	  memcpy(rm_ptr, &rm, sizeof(struct cisam_rm));
	  rm_ptr->created_by = uid;
	  rm_ptr->created_dt_tm = time(0);
	  rc = db(RPTMSG_FILE_ID, UPDATE_REC, &rm_key1, 0, 0);
	  if (rc < 0)
	    {
	      prt_error(UI_NOUPDATE, catgets(UI_m_catd, UI_1, UI_UI_MESG_25, "<repeat message file>"));
	      ERROR(' ',RPTMSG_FILE,"Error updating record");
	      break;
	    }
	  rc = db(RPTMSG_FILE_ID,RELEASE, &rm_key1, 0,0);
	  if (rc < 0)
	    {
	      prt_error(UI_NOUPDATE, catgets(UI_m_catd, UI_1, UI_UI_MESG_25, "<repeat message file>"));
	      ERROR(' ',RPTMSG_FILE,"Error releasing record");
	      break;
	    }
	  prt_error(UI_REC_UPDATED,"");
	  field_num = M_TO2;

	  /* Tell dispatch that a message has been updated */
	  send_msg_to_dispatch(rm_ptr->msg_nbr, MSG_UPD);
	  break;
	  
	case NEXT_PG_KEY:
	  rm_ptr = (struct cisam_rm *)req_buf;
	  rc = db(RPTMSG_FILE_ID, READ_KEYS, &rm_key1, ISNEXT, 0);
	  if (rc < 0)
	    {
	      prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_MESG_26, "No more repeat messages"));
	    }
	  else
	    {
	      current_msg_num = rm_ptr->msg_nbr;
	      memcpy(&rm, rm_ptr, sizeof(struct cisam_rm));
	      prt_entry_areas(msg_win, mesg_flds, MAX_PIU_MESG);
	      display_repeat_message(&rm);
	    }
	  break;
	  
	case PREV_PG_KEY:
	  rm_ptr = (struct cisam_rm *)req_buf;                                /* initialize pointer to request buffer */
	  rc = db(RPTMSG_FILE_ID, READ_KEYS, &rm_key1, ISPREV, 0);
	  if (rc < 0)
	    {
	      prt_error(UI_NO_MSG,catgets(UI_m_catd, UI_1, UI_UI_MESG_26, "No more repeat messages"));
	    }
	  else
	    {
	      current_msg_num = rm_ptr->msg_nbr;
	      memcpy(&rm, rm_ptr, sizeof(struct cisam_rm));
	      prt_entry_areas(msg_win, mesg_flds, MAX_PIU_MESG);
	      display_repeat_message(&rm);
	    }
	  break;
	  
	case DISAPPROVE_KEY:
	  if (current_msg_num == -1)
	    break;
	  rc = db(RPTMSG_FILE_ID, DELETE, 0, 0, 0);
	  if (rc < 0)
	    {
	      ERROR(' ',RPTMSG_FILE,"Error in deleting previous rept record");
	      break;
	    }

	  else
	    {
	      prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_14, "Message deleted"));

	      /* Tell dispatch about deleted message */
	      send_msg_to_dispatch(rm.msg_nbr, MSG_DEL);

	      /* Start again from first message */
	      rc = db(RPTMSG_FILE_ID, READ_KEYS, &rm_key1, ISFIRST, 0);
	      if (rc < 0)
		{
		  prt_error(UI_NO_RECORD, ". %s", catgets(UI_m_catd, UI_1, UI_UI_MESG_28, "No repeat messages"));
		  db(RPTMSG_FILE_ID, CLOSE, 0, 0, 0);
		  sleep(3);
		  menu_manager(YES, SUPERVISORY_MENU);
		  return;
		}
	      current_msg_num = rm_ptr->msg_nbr;
	      memcpy(&rm, rm_ptr, sizeof(struct cisam_rm));
	      prt_entry_areas(msg_win, mesg_flds, MAX_PIU_MESG);
	      display_repeat_message(&rm);
	      break;
	    }
	  
	case HELP_KEY:
	  help(MESSAGE_WIN_SCR, field_num);
	  /* redraw the msg_win */
	  touchwin(msg_win);
	  wrefresh(msg_win);
	  break;
	  
	case COMMAND_KEY:
	  command_line();
	  break;
	  
	default:
	  prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_MESG_27, "<from edit_repeat_msgs screen>"));
	  break;
	}
    }	/* end while */

  db(RPTMSG_FILE_ID, CLOSE, 0, 0, 0);

}	/* end edit_repeat_msgs()  */

display_repeat_message(rm_ptr)
struct cisam_rm *rm_ptr;
{
    	wattrset(msg_win,A_UNDERLINE);
	if (rm_ptr->repeat)
	  mvwaddch(msg_win,M_RPT_ROW,M_RPT_COL,rm_ptr->repeat);
	if (rm_ptr->fleet)
	  mvwaddch(msg_win,M_FLT_ROW,M_FLT_COL,rm_ptr->fleet);
	if (rm_ptr->to_type)
	  mvwaddch(msg_win,M_TO1_ROW,M_TO1_COL,rm_ptr->to_type);
    	mvwprintw(msg_win,M_TO2_ROW,M_TO2_COL,"%s",rm_ptr->to_selection);
	mvwprintw(msg_win,M_OFTN_ROW,M_OFTN_COL,"%d",rm_ptr->how_often);
    	mvwprintw(msg_win,M_UNTM_ROW,M_UNTM_COL,"%s", rm_ptr->until_time);
    	mvwprintw(msg_win,M_UNDT_ROW,M_UNDT_COL,"%s", rm_ptr->until_date);
    	mvwprintw(msg_win,M_TXT1_ROW,M_TXT1_COL,"%s",rm_ptr->line_1);
    	mvwprintw(msg_win,M_TXT2_ROW,M_TXT2_COL,"%s",rm_ptr->line_2);
    	wattrset(msg_win,0);
    	wrefresh(msg_win);
}


/*********************************************************************************/
/* disp_screen_title: displays the first 2 lines for the repeat_edit_msgs screen */
/*********************************************************************************/
disp_screen_title()
{
	int indx;				/* index into menus structure on disk */
	int i;					/* scratch variable */
	int start_col, len;


	clear();

	attrset(A_REVERSE|A_UNDERLINE);
	mvaddstr(0,0, Spaces);

	prt_date();

	indx = map_array[scr_name];	

	len = strlen(cmenu[indx]->mf_desc); 			/* print the title of the screen */
	start_col = (COLS - len - 1 )/2;
	mvprintw(0,start_col,"%s", cmenu[indx]->mf_desc);
	mvprintw(0,start_col+len+3,"(%d)", cmenu[indx]->mf_command);
	
	mvprintw(0,UID_COL,"%d", uid);

	attrset(A_REVERSE);
	mvaddstr(1,0, Spaces);

	attrset(0);
	refresh();
}


/***********************************************************************************************************************************
call_mesgs -
This routine is called when the CALL_TO_DISPATCHER function key is pressed for call with w/o msg.  It will display an empty 
message screen in the lower portion of the terminal screen for creating messages.
***********************************************************************************************************************************/
call_mesgs()
{
	char mbuf[READ_BUF_SIZE];					/* buffer for read field */
	int key_val;							/* key for read_field */
	int field_entered;
	int cancel = 0;							/* flag to end while loop */
	int msg_piu = M_TXT1;						/* piu value */
	struct cisam_rm rmesg;						/* repeat message structure to build messge */
	int should_clear = 0;

	memset((char *)&rmesg,'\0',sizeof(struct cisam_rm));
	call_msg_win_disp();						/* display message window */
	prt_entry_areas(msg_win,&mesg_flds[8],2);			/* fill in user entry areas with underscores */
	wrefresh(msg_win);

	while (cancel == 0) {
		key_val = read_field(msg_win,mesg_flds,msg_piu,mbuf,&field_entered,&should_clear);

		if (field_entered == 0) 
			mbuf[0] = '\0';

		if (msg_piu == M_TXT1)					/* copy text entererd in structure to be written to disk */
			strcpy(rmesg.line_1,mbuf);
		else if (msg_piu == M_TXT2)
			strcpy(rmesg.line_2,mbuf);

		if (clear_err_flag &&  key_val != -1) {
			clear_error();
			refresh();
		}

		switch (key_val) {
			case '\t':
			case KEY_DOWN:
			case 0:
			case '\r':
			case '\n':
				msg_piu = mesg_flds[msg_piu].tab_field;
				break;

			case KEY_UP:
				msg_piu -= 1;
				break;

			case SEND_KEY:
			case SEND_DUP_KEY:
                                if(send_to_dis_flag || multiple_call_flag)      /* called from send_to_dispatcher() fucntion */
					global_msg_num = 0;			/* for CALL_W_MSG function key processing */

				rmesg.created_by = uid;
				rmesg.repeat = NO;

				if(rmesg.line_1[0] != '\0' || rmesg.line_2[0] != '\0')
				    if (message_send(&rmesg) == -1)
					break;

				if(!clear_err_flag) 
					prt_entry_areas(msg_win,&mesg_flds[8],2);

			        msg_piu = M_TXT1;

				bump_stat(rmesg.fleet,SENDING_MSG);		/* increment count of # of msgs send by this user */
				prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_MESG_2, "<message sent>"));

                                if(send_to_dis_flag || multiple_call_flag) {    /* called from send_to_dispatcher() fucntion */
					werase(msg_win);
					touchwin(stdscr);
					return(key_val);
				}

				break;

			case SEND_SUPERV_KEY:
				if(send_to_dis_flag) {				/* called from send_to_dispatcher() fucntion */
					global_msg_num = -1;			/* for CALL_W_MSG function key processing */
					cancel = 1;
				}
				break;

                        case MULTIPLE_KEY:
                                if(multiple_call_flag) {                        /* called from send_as_multiple_call() fucntion */
                                        global_msg_num = -1;                    /* used as flag for send processing */
                                        cancel = 1;
				}
                                 break;

			case HELP_KEY:
				help(MESSAGE_WIN_SCR, msg_piu);
				touchwin(msg_win);				/* redraw the msg_win */
				wrefresh(msg_win);
				break;

			case PRINT_SCR_KEY:					/* print screen image with window  overlay */
				print_scr(msg_win);
				break;

			default:
				break;
		}

		if (msg_piu > MAX_PIU_MESG)
			msg_piu = M_TXT1;
		else if (msg_piu < M_TXT1)
			msg_piu = M_TXT2;
	}

	werase(msg_win);
	touchwin(stdscr);
}


/* ============================================================================================================================	*
call_msg_win_disp: dispalys the msg window when the CALL_TO_DISPATCHER KEY is pressed
* ============================================================================================================================	*/
call_msg_win_disp()
{

	werase(srch_win);
	mvwaddstr(srch_win,0+1,0,catgets(UI_m_catd, UI_1, UI_UI_MESG_11, "Message Text"));
	mvwaddstr(srch_win,0+3,0," ");

}

tpak_box( hWin, dummy1, dummy2 )
     WINDOW   *hWin;
     int      dummy1, dummy2;
{

  if ( getenv("UI_NO_BOXES") )
    {
      return;
    }
  else
    {
      box( hWin, 0, 0 );
    }
}

refresh_msg_data( pRmesg )
     REPEAT_MSG_REC   *pRmesg;
{
  wattrset( msg_win, A_UNDERLINE );
  mvwaddch ( msg_win, M_RPT_ROW, M_RPT_COL, pRmesg->repeat );
  mvwaddch ( msg_win, M_FLT_ROW, M_FLT_COL, pRmesg->fleet );
  mvwaddch ( msg_win, M_TO1_ROW, M_TO1_COL, pRmesg->to_type );
  mvwaddstr( msg_win, M_TO2_ROW, M_TO2_COL, pRmesg->to_selection );
  mvwaddstr( msg_win, M_UNDT_ROW, M_UNDT_COL, pRmesg->until_date );
  mvwaddstr( msg_win, M_UNTM_ROW, M_UNTM_COL, pRmesg->until_time );
  mvwaddstr( msg_win, M_TXT1_ROW, M_TXT1_COL, pRmesg->line_1 );
  mvwaddstr( msg_win, M_TXT2_ROW, M_TXT2_COL, pRmesg->line_2 );
  wattrset( msg_win, 0 );
}