static char sz__FILE__[]="@(#)ui_ftnkey.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_ftnkey.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:50:47
 * @(#)  Last delta: 12/2/94 at 17:56:02
 * @(#)  SCCS File: /taxi/sccs/s.ui_ftnkey.c
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include <ctype.h>
#include <sys/types.h>
#include <isam.h>
#include <errno.h>
#include <sys/ipc.h>
#include <pwd.h>
#include <grp.h>

#include "taxipak.h"
#include "mem_mgmt.h"
#include "Object.h"
#include "List_public.h"
#include "ui_def.h"
#include "call_entry.h"
#include "tokenize.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "rate_isam.h"
#include "ui_error.h"
#include "user.h"
#include "screens.h"
#include "ui_ipc.h"
#include "ui_error.h"
#include "df_maint.h"
#include "func_keys.h"
#include "except.h"
#include "zgeo.h"
#include "srch.h"
#include "writer.h"
#include "language.h"
#include "ui_strdefs.h"
#include "path.h"
#include "calls.h"
#include "veh_driv.h"
#include "mad_ipc.h"
#include "enhance.h"
#include "switch_ext.h"
#include "ex_key.h"
#include "cnapi.h"
#include "ui_call_msg.h"
#include "Call_lists_public.h"

#define		NAVIGATION_CMD_COL	10 	/* col to read cmd on */
#define		NAVIGATION_CMD_CHARS	4	/* max chars for nav. cmd */
#define		HOLDLINE		31	/* for setting holdline bit */

MANUAL_FLAT_HIST   ManualFlatInfo =  { 0, 0.0, 0 };

int New_screen_from_command = 0;

extern struct users *uiuser[];			/* array of pointers in users struct in shared memory */
extern struct fleets *fleet[];
extern int cur_fl_index;			/* index of the fleet the user is currently working with */
extern int def_fl_index;
extern int user_index;				/* index into users struct in shared memory */
extern int end_suspend;				/* flag to end suspend vehicle driver function */
extern int end_basic_call;			/* flag for ending basic call entry */
extern int end_wakeup_call;			/* flag for ending wakeup call entry */
extern int end_extended_call;			/* flag for ending extended call entry */
extern char scr_type;				/* type of screen on display */
extern short user_knows;			/* if user knows that call is unzoned */
extern short call_zoned;			/* if call is not or not */
extern short write_dispatch;			/* if should write to FIFO */
extern short write_disk;			/* if should write to calls file on disk */
extern short write_subs;			/* if should write to subs file on disk */
extern short user_changed_dt_tm;		/* did user change date-time field */
extern int fifo_fd;				/* fiel descriptor for FIFO */
extern int b_prt_default(), e_prt_default(), w_prt_default();
extern int dim_line();
extern int dsp_sum_line();			/* display information on summary line  */
extern struct cisam_cl call_struct;		/* for writing call file */
extern struct call_ipc_rec call_fifo;		/* for call rec to FIFO */
extern int scr_name;
extern int errno;
extern char *sys_errlist[];
extern struct keydesc cl_key1;
extern char req_buf[];
extern char tmp_req_buf[];
extern struct scr_flds basic_scr_flds[], extended_scr_fld[], wakeup_scr_fld[];
extern int pid;					/* process id of this process */
extern struct offsets *offset;			/* pointer to offsets struct in shared memory */
extern int msgkey;
extern CALL_MSG_LIST_HNDL cmsg_list_hndl;
extern struct cisam_cust gCustRec;
/*
 *	fix
 */
extern int num_vh_attr_s,num_dr_attr_s;
extern int ui_phone;
extern int ui_phone_update;
extern int bPhoneFieldVisited;

short end_zone_status =0;			/* flag set if zone status() is to end */
short end_syst_status =0;			/* flag set if system status() is to end */
short end_edit_repeat =0;			/* flag set if edit_repeat_mesg() is to end */
extern int end_unassigned_srch;			/* flag set if unassigned srch is to end */
short inters_flag=0;				/* flag fet if intersection address should be read */
extern short clear_err_flag;			/* set if an error msg on the last line is to be cleared */
short end_exceptions;				/* flag set if exceptions is to end */
extern int prev_scr_name;			/* name of screen saved as previous screen */
extern int prev_scr_flag;			/* previous screen name */
short write_unzoned_flag = 0;			/* set if a record shoule be added to the unzoned address file */
extern int uid;					/* users unix id */
short end_update_user=0;			/* flag for ending update_user_defaults screen */
extern int toggle_piu;				/* piu value when toggle key is pressed */
extern int toggle_flag;				/* 1=toggle key pressed; 0=toggle key not pressed */
extern int doing_update;			/* is user on the update screen or not */
extern int doing_detail;

extern int map_array[];				/* Used to map screen names to index */
extern int (*security[])();			/* pointers to functions for checking users permissions  */

extern struct cisam_cl srch_calls[];		/* saved call records from search */
extern add_noexcpt_callh();			/* Add a non_exception call history record */
void dispatch_key();
void resend_key();
void hold_key();
void callback_key();
void cancel_key();
extern WINDOW *help_win, *srch_win, *mesg_wind;
int own_isrecnum;				/* to save the cisam record # for the call record written to disk */
int global_msg_num = 0;				/* for CALL_W_MSG function key processing */
extern int end_individual_srch;			/* for ending individual search function */
short send_to_dis_flag = 0;			/* set if mesgs() function is called from send_to_dispatcher() */
short multiple_call_flag = 0;                   /* set if mesgs() function is called from send_as_multiple_call() */
extern struct cisam_sc subs_struct;		/* for subscription calls disk i/o */
extern short zone_pkup_lead_time;		/* pickup lead time for pickup zone */
extern char callnbr_sav[];
extern int ui_tlg;
extern int zip1, zip2;
extern int No_status_line;
extern int Dispatch_pid;
extern void local_date_time();
extern int (*b_check_field[])();
extern int (*e_check_field[])();
int set_multiple_call_type( char [], short );
char *last_six();
WINDOW *newwin();
char   gWakeupPassenger[20];
char   gWakeupPhone[20];
char   gWakeupComment[32];
time_t gIntWakeupDueTime = 0;
char   gWakeupFleet = 'A';
BOOLEAN question_for_user_active = FALSE;
extern int LineMgrIndex;
extern short line_mgr_attached;

struct scr_flds vehconf_msg_flds[] = {
  { 0, 0, 1, 1 },
  { 1, 1, 1, 1 }
};

/***********************************************************************************************/
/* ftnkey_handler: called if a function key pressed by the user  is not processed internally   */
/*	by the routine (in which the key was pressed). Once the function key processing is     */
/*	is completed, the function returns. The return value is ignored 		       */
/***********************************************************************************************/
ftnkey_handler(piu_ptr,c,generic_int)
int *piu_ptr;
int c;
int generic_int;			/* used to pass misc integers */
{
	int	grp;
	int     piu_sav;

	switch(c) {
		case COMMAND_KEY:		/* execute command key function */
			command_line();
			break;

		case SEARCH_KEY:		/*  for searches */
			searches();
			break;

        case SEND_WAKEUP_KEY:  
          if (piu_ptr)
            send_call(piu_ptr,NO, NO, NO, YES);
          else
            prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1, "<wrong function key pressed>"));
          break;
          
		case SEND_KEY:			/* FOR SEND KEY */
			if (piu_ptr)
			  send_call(piu_ptr,NO, NO, NO, NO);
			else
			  prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1, "<wrong function key pressed>"));
			break;

		      case 23:   /* control-W */
			if (!(1) )
			  prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1, "<wrong function key pressed>"));
			else if ( ( call_struct.nbr_in_group != '\0' ) && ( call_struct.nbr_in_group != '1' ) )
			  prt_error(UI_NOFIELD, "SEND_HOLD only valid for single vehicle request");
			else if (piu_ptr)
			  send_call(piu_ptr, NO, YES, NO, NO);
			break;

		      case 20: /* control-T */
			if (!(1) )
			  prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1, "<wrong function key pressed>"));
			else if ( ( call_struct.nbr_in_group != '\0' ) && ( call_struct.nbr_in_group != '1' ) )
			  prt_error(UI_NOFIELD, "SEND_SWITCH only valid for single vehicle request");
			else if (piu_ptr)
			  send_call(piu_ptr, NO, NO, YES, NO);
			break;

	case 27:
		case SEND_DUP_KEY:		/* send and duplicate key, processing only for call entry screens */
			if (piu_ptr)
			  send_call(piu_ptr,YES, NO, NO, NO);
			else
			  prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1, "<wrong function key pressed>"));
			break;

		case PRINT_SCR_KEY:		/* prints the screen which is on display on the line printer */
			print_scr(NULL);
			break;

		case HELP_KEY:
			if (piu_ptr)
			  help(scr_name,*piu_ptr);
			else
			  prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1, "<wrong function key pressed>"));
			break;

		case PREV_SCR_KEY:		/* display the preiouslu saved screen */
			previous_scr();
			break;

		case MESSAGE_KEY:		/* send message key */
			mesgs(NULL);
			break;

		case RCV_MSG_KEY:		/* recv message key */
			tpak_read_messages();
			break;

		case ALTERNATE_KEY:		/* alternate address key */
			if (piu_ptr)
			  altrn_adr(piu_ptr);
			else
			  prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1, "<wrong function key pressed>"));
			break;

		case KDT_EMULATE_KEY:
			grp = getgid();
			//			if(grp == 110 || grp == 400 || grp == 500 || grp == 600 || grp == 15)	/* Programmer, Dispatcher or Manager */
				kdt_emulation();
			//			else
			//				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1, "<wrong function key pressed>"));
			break;

		case TOGGLE_KEY:		/* toggle to appropriate call entry screen */
			toggle_screen();
			break;

		case RESEND_KEY:
			resend_key(generic_int);		/* pass searches array call number */
			break;

		case HOLD_KEY:
			hold_key(generic_int);			/* pass searches array call number */
			break;

		case CALLBACK_KEY:
			callback_key(generic_int);		/* pass searches array call number */
			break;

		case CANCEL_KEY:
			if (piu_ptr)
			  cancel_key(piu_ptr,generic_int);	/* pass searches array call number */
			else
			  prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1, "<wrong function key pressed>"));
			break;

		case SEND_SUPERV_KEY:				/* send to dispatcher function key */
			if (piu_ptr)
			  send_to_dispatcher(piu_ptr);
			else
			  prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1, "<wrong function key pressed>"));
			break;

		      case COMMENT_KEY:
			if ((piu_ptr) && (1))
			  comment_key(srch_win, generic_int);
			else
			  prt_error(UI_NOFIELD, catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1, "<wrong function key pressed>"));
			break;

		      case CL_MSG_KEY:
		      case CL_MSG_KEY2:
			  searches();
			break;
			    
			
		case MULTIPLE_KEY:
			if (piu_ptr)
			  send_as_multiple_call(piu_ptr, generic_int);
			else
			  prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1, "<wrong function key pressed>"));
			break;


		case '\014':					/* control L .. refresh screen  if messed up */
			wrefresh(curscr);			/* This is done by read_field() too. read_field */
			break;					/* will never return a ^L. */

	case 255:
	  break;
	  
		default:
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1, "<wrong function key pressed>"));
			break;
	}

}

command_line()
{
	char read_buf[READ_BUF_SIZE];	/* for reading users input */
	struct scr_flds nav_cmd;	/* structure for reading input */
	int piu = 0;	/* to be passed to read_field */
	short end_nav_cmd = 0;	/* flag to ending this function */
	short error_flag = 0;	/* flag to error */
	int key_val;	/* return value from read_field() */
	int cmd_num;	/* # entered by user */
	int field_entered;	/* flag =1, if field entered; else 0 */
	int i;	/* scratch variable */
	int should_clear = 0;
	int top_line[40];	/* a 40 char string is an arbitrary value */
	int x;
	short new_ss_command;

	New_screen_from_command = 0;

	/* Save top line of screen to put back after done*/
	for (i = 0; i < 40; i ++) {
	    x = (int) mvinch(0, i);
	    top_line[i] = x;
	}

	/* clear the date area, and print the prompt */
	mvprintw(0,0,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_2, " Command: "));

	/* set up the structure for reading the field */
	nav_cmd.prompt_row = 0;
	nav_cmd.prompt_col = NAVIGATION_CMD_COL;
	nav_cmd.max_chars = NAVIGATION_CMD_CHARS;	
	piu = 0;	/* dummay to be passed to read_field() */

	while ( ! end_nav_cmd ) {
		/* read the field */
		should_clear = 1;	/* clear the entry area upon entry of 1st char in field */
		key_val=read_field(stdscr,&nav_cmd,piu,read_buf,&field_entered,&should_clear);
		
		if ( key_val == -1 )
			/* read_field() timed out */
			continue;
		
		if ( clear_err_flag )
			/* an error msg on last line should be cleared */
			clear_error();

		/* entry should be terminated by <SEND>, <CR>, newline,
		 * or reaching max chars
		 */
		if (key_val != '\r' && key_val != '\n' && key_val != COMMAND_KEY &&
			key_val != SEND_KEY) {
			/* invalid termination of field */
			prt_error(UI_NOFIELD,
				catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_3, "(should terminate with <SEND> or <CR> )"));
			continue;  /* back to while loop */
		}  /* end if ... invalid termination */
		/* terminated properly */
		if ( key_val == '\n'  && !field_entered) {
			/* field not entered; leave function */
			end_nav_cmd = 1;
			move(0, 0);
			for (i = 0; i < 40; i ++) {
			    attrset(top_line[i] & A_ATTRIBUTES);
			    addch(top_line[i]);
			}
			attrset(0);
			refresh();
			continue;  /* back to while loop */
		}  /* end if ... field not entered */
		if ( ! field_entered ) {
			/* field not entered; leave function */
			end_nav_cmd = 1;
			/* display the main menu */
			menu_manager(YES,MAIN_MENU);
			New_screen_from_command = 1;
			continue;  /* back to while loop */
		}  /* end if ... field not entered */

		/* field has been entered */
		i = 0;	
		while ( read_buf[i] != '\0' && i < NAVIGATION_CMD_CHARS) {
			/* check that chars entered are digits */
			if ( !isdigit(read_buf[i]) ) {
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_4, "<digits only>"));
				error_flag = 1;	/* set flag */
				break;	/* leave this while loop */
			}  /* end if */
			i++;
		}  

		if(error_flag) {
			error_flag = 0;	/* reset flag */
			continue;	/* wrong input */
		}

		cmd_num = atoi(read_buf);

/*
 *      check for shift schedule
 */
		if (new_ss_command = shift_s_command(cmd_num))
			cmd_num = new_ss_command;

		/* check if # entered is < 290; since menu_manager() can
		 * only check within this range
		 */
		if(cmd_num >= MAX_MENU_LIST) {
			/* invalid # entered */
			prt_error(UI_NOSCR,"<%d>", cmd_num);
			continue;	/* back to while loop */
		}

		/* call the menu manager  */
		if(menu_manager(YES,cmd_num) != DONE) 
			/* wrong # entered; error msg is printed by menu_manager() */
			continue;	/* back to while loop */

		/* everything is ok */
		end_basic_call = 1;	/* end basic call */
		end_wakeup_call = 1;	/* end wakeup call */
		end_extended_call = 1;	/* end extended call */
		end_zone_status = 1;	/* end zone status display */
		end_syst_status = 1;	/* end system status display */
		end_edit_repeat = 1;	/* end edit repeat mesg display */
		end_unassigned_srch = 1;	/* end unassigned srch */
		end_exceptions = 1;	/* end exceptions() */
		end_nav_cmd = 1;	/* set flag to leave loop */
		end_update_user = 1;	/* end update_user_defaults screen */
		end_suspend = 1;   /* end suspend vehicle/driver function */
		end_individual_srch = 1;	/* end individual search function */
		New_screen_from_command = 1;
		inters_flag = 0;	/* intersection flag not entered */

	}  /* end while */

	/* reprint the old date if the new screen is a menu */
	if ( scr_type == UI_MENU ) {
		mvprintw(0,0,"                      ");
		attrset(A_REVERSE);
		prt_date();
		attrset(0);
	}  /* end if */

}  /* end command_line() */

/* searches: 	calls the function which the users default search, when the
 *		searches function key is pressed
 */
searches()
{
	/* check if the default search specified is a # less than 300 */
	if ( uiuser[user_index]->default_search >= MAX_MENU_LIST ) {
		/* wrong default search specified */
		prt_error(UI_NOSCR,"<%d>", uiuser[user_index]->default_search);
		return;
	}  /* end if */

	/* everything is ok */
	end_basic_call = 1;	/* end basic call */
	end_wakeup_call = 1;	/* end wakeup call */
	end_extended_call = 1;	/* end extended call */
	end_zone_status = 1;	/* end zone status display */
	end_syst_status = 1;	/* end system status display */
	end_edit_repeat = 1;	/* end edit repeat mesg display */
	end_unassigned_srch = 1;	/* end unassigned srch */
	end_exceptions = 1;	/* end exceptions() */
	end_update_user = 1;	/* end update_user_defaults screen */
	end_suspend = 1;   /* end suspend vehicle/driver function */
	end_individual_srch = 1;	/* end individual search function */
	inters_flag = 0;	/* intersection flag not entered */

	/* call the menu_manager() */
	if ( menu_manager(YES,uiuser[user_index]->default_search) != DONE ) {
		/* wrong default search specified */
		prt_error(UI_NOSCR,"<%d>", uiuser[user_index]->default_search);
		return;
	}

}

/****************************************************************************************/
/* check_exceptions: checks if the call entered is zoned or not				*/
/*		Returns:								*/
/*			DONE - if zoned							*/
/*			NOT_DONE - if unzoned and user does not know 			*/
/*				   that the call is unzoned				*/
/****************************************************************************************/
int check_exceptions(piu_ptr)
int *piu_ptr;								/* pointer to piu */
{
					/* if we are on a wakeup call entry screen; then we should not try to zone the call; */
	 				/* just set the wakeup exception bit */
	if (scr_name == WAKEUP_CALL_SCR || scr_name == SUBS_WAKEUP_SCR) {		/* its a wakeup call */
		call_fifo.status.wakeup = 1;						/* generate wakeup call exception */
		set_call_bit(WAKE_UP-1);			/* set wakeup call bit in status_bits field of call_struct */
		return(DONE);					/* no need to zone call for wakeup calls */
	}

	if (call_struct.hold_line[0] != '\0') {			/* check if holdline bit should be set */
		call_fifo.status.holdline = 1;
		set_call_bit(HOLDLINE-1);
	}

	if (user_changed_dt_tm) {				/* user changed date-time fields ? */
		if (get_internal_time(call_struct.due_date,call_struct.due_time) < time(NULL)) {
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_5, "<date/time entered has past; please verify and reenter>"));
			if (scr_name == BASIC_CALL_SCR) {
				*piu_ptr = B_DATE;
			}
			else if (scr_name == EXTENDED_CALL_SCR) {
				*piu_ptr = E_DATE;
			}
			else if (scr_name == EXTENDED_CALL_SCR) {
				*piu_ptr = W_DATE;
			}
			return(NOT_DONE);
		}
	}
	else {								/* user did not touch date-time, default to now */
		get_pkup_time(call_struct.due_time); 			/* save the date & the time this call */
		get_pkup_date(call_struct.due_date); 			/* is entered, in display format */
	}

								/* must have something in pickup street name */
	if(scr_name ==BASIC_CALL_SCR || scr_name ==SUBS_BASIC_SCR || scr_name ==EXTENDED_CALL_SCR || scr_name ==SUBS_EXTENDED_SCR) {
	 	if(call_struct.pckup_str_name[0] == '\0') {
			*piu_ptr = B_PKUP_ST_NO;
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_6, "<pickup street name>"));
			return(NOT_DONE);
		}
	}

	if (!call_zoned  && (call_struct.pckup_zone == 0)) {			/* call not zoned yet */
		if (! user_knows) {						/* if user does not know about it, */

		        /* Use now knows that call is unzoned */
		        user_knows = 1;

			prt_error(UI_NOZONE, "");				/* if user has permission to zone, */
			if (uiuser[user_index]->can_zone_calls == YES) {
				switch (scr_name) {				/* then place him in the zone field */
					case BASIC_CALL_SCR:
					case SUBS_BASIC_SCR:
						*piu_ptr = B_PKUP_ZONE;
						break;
					case EXTENDED_CALL_SCR:
					case SUBS_EXTENDED_SCR:
						*piu_ptr = E_PKUP_ZONE;
						break;
				}
			}
			else {							/* user is not allowed to zone calls */
				if (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR)
					*piu_ptr = B_PKUP_ST_NO;		/* place user in street name field */
				else if (scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR)
					*piu_ptr = E_PKUP_ST_NO;
			}
			return(NOT_DONE);

		}
		else {							/* user knows that call is unzoned; set except bit */
			write_unzoned_flag = 1;				/* so as to add a record to the unzoned address file */
			call_fifo.status.unzoned = 1;
			set_call_bit(UNZONED-1);			/* set the bit in call_struct.status_bits */
			return(DONE);
		}
	}
	else  {								/* call is already zoned; so set the bit to 0 */
		call_zoned = 1;
		call_fifo.status.unzoned = 0;
		return(DONE);
	}
}

/*********************************************************************************/
/* send: sends the call for dispatching; by writing it to the named pipe,	 */
/*	 and the calls and/or subscription file on disk 			 */
/*********************************************************************************/
send_call(piu_ptr,send_dup_flag,send_hold_flag,send_switch_flag,send_wakeup_flag)
int *piu_ptr;						/* pointer to piu */
char send_dup_flag;					/* YES send_duplicate key entered; NO send key entered */
char send_hold_flag;
char send_switch_flag;
char send_wakeup_flag;
{
	char loop_count;				/* # of requetss to be written to FIFO */
	struct scr_flds *ptr_scr_fld;			/* pointer to the array of piu structs*/
	int (*func_ptr_defs)();				/* pointer to either b_prt_defaults or e_prt_defaults or w_prt_defaults */

	int cisam_ret_num;				/* cisam generated # foirom calls file */
	struct scr_flds *ptr_to_fld;			/* points to approp. piu struct */
	int i;
	int max_piu;					/* max piu value */
	struct cisam_cl *temp_cisam_ptr;		/* for assigning to req_buf */
	char no_of_vehs;				/* # of calls to service this call */
	char date_str[10];				/* date string */
	char time_str[10];				/* time string */
	int  rc;
	int  key;
	short findex = 0;
	struct veh_rqst vrq;
	struct passwd *PasswordEntry;
	struct group *ManagerGroupEntry;
	char **pGroupNames;
	uid_t UserUID;
	short done = FALSE;
	int   AddCallMsg;
	int   AvailTaxiFlag = 1;
	char  MeterAction[132];
	CALL_MSG_HNDL  cmsg_hndl;
	CL_MSG_REC     *cmsg_rec;
	char  cl_msg_nbr;
	struct cisam_ac account;

	
	
	/* Check if dispatch PID is still up. kill(pid, 0) returns
	   a -1 and an errno of ESRCH if the PID does not exist.
	   Don't want to send a call if no dispatch to take it.
	   Warn user and exit from UI when dispatch is dead. */
	callnbr_sav[0] = '\0';	
	rc = kill(Dispatch_pid, 0);
	if (rc == -1 && errno == ESRCH)
	  {
	    prt_error(UI_NOT_ALLOWED, ": %s", catgets(UI_m_catd, UI_1, UI_TEXT_3, "Dispatch is down"));
	    sleep(1);
	    beep();
	    sleep(1);
	    beep();
	    sleep(1);
	    cleanup();
	  }

	
	if (scr_name != BASIC_CALL_SCR && scr_name != SUBS_BASIC_SCR &&
	    scr_name != EXTENDED_CALL_SCR && scr_name != SUBS_EXTENDED_SCR &&
	    scr_name != WAKEUP_CALL_SCR && scr_name != SUBS_WAKEUP_SCR ) { 		/* not called from call entry screen */
		prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_8, "<send/send_dup key from this screen>"));
		return;
	}

	if ( scr_name == EXTENDED_CALL_SCR )
	  {
	    /** validate the account number **/
	    bzero( &account, sizeof( struct cisam_ac ) );
	    account.fleet = call_struct.fleet;
	    strcpy(account.nbr, call_struct.charge_nbr);
	    if ( !valid_charge_nbr(&account, &call_struct, 1) )
	      {
		prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_MISC_9, "<charge #>"));	/* invalid charge # entered */
		return;
	      }
	  }

	if (scr_name == SUBS_BASIC_SCR || scr_name == SUBS_EXTENDED_SCR || scr_name == SUBS_WAKEUP_SCR) {
		subs_send(piu_ptr,send_dup_flag);			/* call the subscription calls processing function */
		return;
	}
									/* check if call is zoned or not (for wakeup call */
									/* entry screens; set the wakeup exception bit */
	if (check_exceptions(piu_ptr) == NOT_DONE) 			/* user does not know that call is unzoned; tell him */
	  {
	    callnbr_sav[0] = '\0';
	    return;
	  }

	/* Get TaxiGEO coords for Taxi Stockholm */
	// Taken out 08-30-00 because all coordinates set in GIS system
#ifdef FOO
	if ( glEnhSwitch & ENH0002 )
	  {
	    CityNavGetCoords( &call_struct );
	  }
#endif

	loop_count = (call_struct.nbr_in_group == '\0') ? '1': call_struct.nbr_in_group; 	/* # of records to be sent to */
	no_of_vehs = '1'; 								/* dispatch = # of vehs specified for call*/
	while ( no_of_vehs <= loop_count ) {
		if (write_disk) { 							/* write to disk */
			call_struct.date_time_in = time(NULL);
			call_struct.entered_by = (short)uid;
			time_subs_call();					/* checks whether its a time or subscription call */
						 				/* (also sets call_struct.due_date_time field) */

										/* if call does not pass minimum time for time */
										/* call them make it a now call */
			if(call_struct.due_date_time < ( time(NULL) +  fleet[cur_fl_index]->future_call_minimum)){
				get_pkup_date(date_str);					/* get current date */
				get_pkup_time(time_str);					/* get current time */
				strcpy(call_struct.due_date,date_str);				/* save date in call record */
				strcpy(call_struct.due_time,time_str);				/* save time in call record */
				call_struct.due_date_time = get_internal_time(call_struct.due_date, call_struct.due_time);
				if ( call_struct.extended_type[2] != 'K' )
				  call_struct.extended_type[2] = ' ';
			}

/*
 *  restricted time calls
 */
			if ((call_struct.extended_type[2] == TIMEC) &&
				(scr_name == BASIC_CALL_SCR || scr_name == EXTENDED_CALL_SCR))
			  {
			    if (1) // Finland sets special driver attributes for TC's
			      call_struct.drv_attr[22] = YES;
				if (!tcr_check_time_call_restriction(&call_struct, TRUE, SEND_KEY))
					return;
			  }
			else if ( scr_name == BASIC_CALL_SCR || scr_name == EXTENDED_CALL_SCR )
			  {
			    if (1)
			      call_struct.drv_attr[22] = NO;
			  }



			if (call_struct.extended_type[0] == WAKEUP)				/* wakeup call ? */
				zone_pkup_lead_time = fleet[cur_fl_index]->wakeup_lead_time;	/* set lead time */

			if((call_struct.priority = get_call_pri()) == -1){	/* get the call priority and save it */
				call_struct.priority = fleet[cur_fl_index]->passenger_priority_def;	/* default to passenger */
			}
			if(!doing_update){
				if (call_struct.extended_type[2] == TIMEC && 
				    (call_struct.due_date_time > ( time(NULL) + zone_pkup_lead_time)))
				  {
				    call_struct.pri_status = 63;			/* PENDING time call */

				  }
				else {
					call_struct.pri_status = call_struct.priority;	/* not a time call, regular priority */
					strcpy(call_struct.status,UNASSGND);

				}
			}
			/* check for valid zone number */
			findex = find_findex(call_struct.fleet);
			if (((fleet[findex]->zone_ptr[call_struct.pckup_zone] == NULL) ||
			    (call_struct.pckup_zone == 0)) && (call_struct.extended_type[0] != WAKEUP))
			  {
			    if (0)
			      {
				prt_error( UI_NO_MSG, catgets( UI_m_catd, UI_1, UI_UI_FTNKEY_30, "Call must be zoned" ) );
				return;
			      }
			    else
			      {
				write_unzoned_flag = 1;
				call_struct.pckup_zone = 0;
				call_fifo.status.unzoned = 1;
				set_call_bit(UNZONED-1);
			      }
			  }

			/** CHECK FOR AVAILABLE TAXIS **/
			/** DONT FORGET TO CHECK IF ITS AN IMMEDIATE CALL **/
			if ( (0) && ( call_struct.pckup_zone != 0 ) &&
			     ( call_struct.pri_status != 63 ) )
			  {
			    if ( ( AvailTaxiFlag = CheckAvailTaxis( &call_struct, piu_ptr ) ) < 0 )
				return;
			  }
			
			get_pkup_time(call_struct.time_in); 			/* save the date & the time this call */
			get_pkup_date(call_struct.date_in); 			/* is entered, in display format */
			if (call_struct.nbr_in_group != '\0')
				call_struct.pos_in_group = no_of_vehs;
			if (strlen(call_struct.phone_nbr) <= 0)
				call_struct.phone_area[0] = '\0';		/* no phone number -> no area code */


			call_struct.ext_nbr = -1;  /* This call originated in UI so mark as a NON_EXTERNAL call */
			memcpy(req_buf,(char *)&call_struct,sizeof(struct cisam_cl));
top_of_send:

			if ((i= db(CALL_FILE_ID,ADD_REC,&cl_key1,ISEQUAL,0)) < 0){ 	/* should write record to call file */
				prt_error(UI_WRITE,"<%d>",i); 				/* db() failed to write rec. */
				return;
			}
			temp_cisam_ptr = (struct cisam_cl *)req_buf;
			if ((isrecnum == 0) || (temp_cisam_ptr->nbr == 0))
			  {
			    key = 0;
			    while (key != YES && key != NO)
			      {
				move(23, 0);
				clrtoeol();
				refresh();
				beep();
				attrset(A_REVERSE);
				mvprintw(23, 0, " %s ", "Error writing call record. Resend? (Y/N)");
				refresh();
				attrset(0);

				key = Toupper(wgetch(curscr));
				if (key == YES)
				  goto top_of_send;
				if (key == NO){
				  move(23,0);
				  clrtoeol();
				  refresh();
				  return;
				}
			      }
			  }
			own_isrecnum = (int)isrecnum; 			/* save the cisam record # for writing to dispatch */
		}
		
		temp_cisam_ptr = (struct cisam_cl *)req_buf; 		/* copy call # placed in req_buf into call_struct */
		call_struct.nbr = temp_cisam_ptr->nbr;


		
		if (write_unzoned_flag) { 		/* if call is unzoned then add a record to the unzoned address file */
			write_unzoned();  		/* add record to unzoned address file */
			write_unzoned_flag = 0;
		}

		add_callh_spec(R_ENTERED);		/* get "ENTERED" call history record into call history file */
		if ( ManualFlatInfo.ManualFlatPerformed )
		  add_callh_spec(R_MANUAL_RATE);

		generate_callh_recs(); 			/* add call history records for any exceptions associated with this call */

		bump_stat(call_struct.fleet,ENTERING_CALL);			/* update user statistics */

		if (write_dispatch) {			/* write record to FIFO, if required */
			fill_call_rec(); 		/* FILL IN CALL_REC struct fields here */
			if (write(fifo_fd,&call_fifo,sizeof(struct call_ipc_rec)) == -1) {
				prt_error(UI_WRITE,"<%s>", sys_errlist[errno]); 		/* write on fifo failed */
				return;
			}
		}


                
		no_of_vehs++;
	}

	ManualFlatInfo.PrevFlatRate = 0;
	ManualFlatInfo.ManualFlatPerformed = 0;
	ManualFlatInfo.UserID = 0;

	if ( send_hold_flag == YES )
	  {
	    VehConfirmProcess( );
	  }

	switch (scr_name) {					/* clear the fields on the screen; get the max piu value */
		case BASIC_CALL_SCR:
		  if ( (line_mgr_attached) && (LineMgrIndex >= 0) && (LineMgrIndex < MAX_PIDS) &&
		       (1) )		  
		    //if ((ui_phone) && (glEnhSwitch & ENH0005))
			  {
	                    //fill_cust_rec();
			    bPhoneFieldVisited = 0;
			  }
			else
			  fill_cust_rec2();  // just store the customer record if available
			max_piu = MAX_PIU_BASIC;
			ptr_scr_fld = basic_scr_flds;
			func_ptr_defs = b_prt_default;
			strcpy(callnbr_sav, last_six(call_struct.nbr));			
			if ((!ui_tlg) || (send_dup_flag == YES))
			  prt_error(UI_SENTFOR_DISPATCH," # %s ", last_six(call_struct.nbr));  /* inform user of call # */
                        if ( send_wakeup_flag == YES )
                          {
                            end_basic_call=1;
                            scr_name = WAKEUP_CALL_SCR;
                            strcpy(gWakeupPassenger, call_struct.passenger_name);
                            strcpy(gWakeupPhone, call_struct.phone_number);
                            gIntWakeupDueTime = call_struct.due_date_time;
                            sprintf(gWakeupComment, "%s %s",
                                    catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_21, "Call nbr"),
                                    last_six(call_struct.nbr));
                          }                        
			break;
		case SUBS_BASIC_SCR:
			if ((ui_phone) && (1))
			  {
			    fill_cust_rec();
			    bPhoneFieldVisited = 0;
			  }
			max_piu = MAX_PIU_BAS_SUBS;
			ptr_scr_fld = basic_scr_flds;
			func_ptr_defs = b_prt_default;
			break;
		case EXTENDED_CALL_SCR:
			max_piu = MAX_PIU_EXTENDED;
			ptr_scr_fld = extended_scr_fld;
			func_ptr_defs = e_prt_default;
			strcpy(callnbr_sav, last_six(call_struct.nbr));
			if ((!ui_tlg) || (send_dup_flag == YES))
			  prt_error(UI_SENTFOR_DISPATCH," # %s ", last_six(call_struct.nbr));  /* inform user of call # */
                        if ( send_wakeup_flag == YES )
                          {
                            strcpy(gWakeupPassenger, call_struct.passenger_name);
                            gIntWakeupDueTime = call_struct.due_date_time;
                          }
			break;
		case SUBS_EXTENDED_SCR:
			max_piu = MAX_PIU_EXT_SUBS;
			ptr_scr_fld = extended_scr_fld;
			func_ptr_defs = e_prt_default;
			break;
		case WAKEUP_CALL_SCR:
			max_piu = MAX_PIU_WAKEUP;
			ptr_scr_fld = wakeup_scr_fld;
			func_ptr_defs = w_prt_default;
			prt_error(UI_SENTFOR_DISPATCH," # %s ", last_six(call_struct.nbr));  /* inform user of call # */
			break;
		case SUBS_WAKEUP_SCR:
			max_piu = MAX_PIU_WAK_SUBS;
			ptr_scr_fld = wakeup_scr_fld;
			func_ptr_defs = w_prt_default;
			break;
		default:
			prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_9, "<called send()>"));
	}

	write_dispatch = 1; 				/* write to fifo */
	write_disk = 1;					/* write to call file */
	write_subs = 0;  				/* do not write to subs file */
	if (send_dup_flag == NO) 			/* if send key hit; reset time flag */
		user_changed_dt_tm = 0;			/* user did not change date-time fields */
	
	memset((char *)&call_fifo,'\0',sizeof(struct call_ipc_rec)); 	/* initialize call_fifo structures for the next call */

	if ( (send_dup_flag == NO) && (send_switch_flag == NO) ) { 					/* if send key hit; clear all structures */
		if (scr_name != WAKEUP_CALL_SCR  &&  scr_name != SUBS_WAKEUP_SCR) {
			get_zone_desc(C_CLEAR, PICKUP_ADDR, &call_struct, NULL);			/* clear the zone_desc */
			get_zone_desc(C_CLEAR, DEST_ADDR, &call_struct, NULL);	

			get_est_wait_time(0); 					/* clear the estimated time field area */
			
			print_addr_comment(C_CLEAR,PICKUP_ADDR);			/* clear the pkup and dest comments */
			print_addr_comment(C_CLEAR,DEST_ADDR);
			if (scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR)
			  {
			    call_struct.ct_adr_cmnt[0] = '\0';
			    mvaddstr( E_CT_CMNT_ROW, E_CT_CMNT_COL, "                              ");
			  }
			else if (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR )
			  {
			    call_struct.ct_adr_cmnt[0] = '\0';
			    mvaddstr( B_CT_CMNT_ROW, B_CT_CMNT_COL, "                              ");
			  }
		}

		if (scr_name == EXTENDED_CALL_SCR  ||  scr_name == SUBS_EXTENDED_SCR)
			charge_related_fields(C_CLEAR,NULL,&call_struct); 		/* clear the charge # fields, etc */

		attrset(A_UNDERLINE); 						/* clear all the fields */
		prt_entry_areas(stdscr,ptr_scr_fld,max_piu);

		if (scr_name != WAKEUP_CALL_SCR  &&  scr_name != SUBS_WAKEUP_SCR) {
			attrset(0);
			inters_flds(C_CLEAR,ptr_scr_fld);				/* clear the intersection field underlining */
			attrset(A_UNDERLINE); 					/* clear all the fields */
		}

		call_zoned = 0;						/* call not zoned */
		user_knows = 0;						/* user does not know */

		if (scr_name == WAKEUP_CALL_SCR || scr_name == SUBS_WAKEUP_SCR)
			*piu_ptr = W_NAME;				/* go to the name field */
		else if ( scr_name == EXTENDED_CALL_SCR )
                  *piu_ptr = E_CHARGE;
                else
                  *piu_ptr = 3;					/* go to pkup address field */
		
		if (1)
		  {
		    if ((line_mgr_attached) && (LineMgrIndex >= 0) && (LineMgrIndex < MAX_PIDS) && (*piu_ptr == 3))	    
		      *piu_ptr = B_PH_AREA_CODE;
		  }

		inters_flag = 0;					/* so as not to read the intersection address */
		zip1 = zip2 = 0;
		memset((char *)&call_struct,'\0',sizeof(struct cisam_cl));
/*
 *	fix
 */
		num_vh_attr_s = num_dr_attr_s = 0;
		cur_fl_index = def_fl_index;
		(*func_ptr_defs)(); 					/* print the default again */
	}
	else if ( send_switch_flag == YES )
	  {
	    switch_pickup_dest();
	    set_multiple_call_type(call_struct.extended_type, 0);
	    
	    prt_error(UI_SENTFOR_DISPATCH," "); 			/* inform user that call is being processed */
	    prt_entry_areas(stdscr,ptr_scr_fld,max_piu);
	    print_addr_comment(C_CLEAR, PICKUP_ADDR);
	    print_addr_comment(C_CLEAR, DEST_ADDR);
	    if ( scr_name == BASIC_CALL_SCR )
	      *piu_ptr = B_PKUP_CITY;
	    else
	      *piu_ptr = E_PKUP_CITY;
	    zone_call(ptr_scr_fld, *piu_ptr);
	    if ( scr_name == BASIC_CALL_SCR )
	      *piu_ptr = B_DEST_CITY;
	    else
	      *piu_ptr = E_DEST_CITY;
	    zone_call(ptr_scr_fld, *piu_ptr);
	    get_est_wait_time(0);
	    get_zone_desc(C_PRINT, PICKUP_ADDR, &call_struct, NULL);
	    get_zone_desc(C_PRINT, DEST_ADDR, &call_struct, NULL);
	    if ( scr_name == BASIC_CALL_SCR )
	      prt_bold_fields( &call_struct );
	    else if ( scr_name == EXTENDED_CALL_SCR )
	      prt_eold_fields( &call_struct );
	    get_est_wait_time(1);
	    ManualFlatInfo.PrevFlatRate = 0;
	    ManualFlatInfo.ManualFlatPerformed = 0;
	    ManualFlatInfo.UserID = 0;
            bzero( &gCustRec, sizeof( struct cisam_cust ) );
            bPhoneFieldVisited = TRUE;
            strcpy(call_struct.phone_number, "0000000000");
	    if ( scr_name == BASIC_CALL_SCR )
	      *piu_ptr = B_TIME;
	    else
	      *piu_ptr = E_TIME;
	  }
	else { 								/* send & duplicate key entered */ 
/*
 *              fix for multiple call - clear 'M' on send and dup
 */
                set_multiple_call_type(call_struct.extended_type, 0);

		prt_error(UI_SENTFOR_DISPATCH," "); 			/* inform user that call is being processed */
		ManualFlatInfo.PrevFlatRate = 0;
		ManualFlatInfo.ManualFlatPerformed = 0;
		ManualFlatInfo.UserID = 0;
	}
}  									/* end send() */

/*************************************************************************************/
/* fill_call_rec: fills the call_rec structure with info from the cisam_cl struct,   */
/*		where it is saved during call_entry 				     */
/*************************************************************************************/
fill_call_rec()
{
	int i;

	call_fifo.u_prcs_id = pid; 				/* copy the process id of the user */
	call_fifo.rec_type = CALL_REQ; 				/* copy the type of the request to be sent */
								/* copy the message # for the call to supervisor */
								/* copy the status bits ( for exceptions ) */
	call_fifo.c_isam_nbr = (int) own_isrecnum; 		/* copy the cisam_rec # */
	call_fifo.call_nbr = call_struct.nbr; 			/* copy the call # */
	call_fifo.grouped_with = call_struct.grouped_with; 	/* copy the grouped with field */
	call_fifo.fleet = call_struct.fleet; 			/* copy the fleet */
	get_call_type(); 					/* copy the call type */
	call_fifo.personal_veh = call_struct.personal_veh;	/* copy cab number if personal request */
	call_fifo.personal_rqst = call_struct.personal_rqst;	/* copy O/I */
	call_fifo.pickup_zone = call_struct.pckup_zone; 	/* copy the pickup zone */
	call_fifo.dest_zone = call_struct.dest_zone; 		/* copy the destination zone */
	call_fifo.personal_veh = call_struct.personal_veh; 	/* copy the personal veh fields */
	call_fifo.msg_nbr = global_msg_num;			/* copy msg # for call with msg exception */
	strcpy(call_fifo.pick_pmap,call_struct.pckup_pmap);	
	
	for ( i = 0; i < ATTR_MAX; i++ )  { 			/* copy the vehicle attributes */
		if ( call_struct.veh_attr[i] == YES )  {
			set_bit(i,sizeof(struct veh_attributes),&call_fifo.veh_attr); 			/* set bit */
		}
	}

	for ( i = 0; i < ATTR_MAX; i++ )  { 			/* copy the driver attributes */
		if ( call_struct.drv_attr[i] == YES )  {
			set_bit(i,sizeof(struct drv_attributes),&call_fifo.drv_attr); 			/* set bit */
		}
	}

	call_fifo.priority = call_struct.priority; 		/* copy the call priority */
	call_fifo.due_time = call_struct.due_date_time; 	/* copy the due date time */
	call_fifo.updated_call = 0; 				/* intialize to a new call */

	
	call_fifo.old_due_time = 0; 	/* and no old due time */
	
	call_fifo.call_msg_nbr = call_struct.call_msg_nbr;
	call_fifo.gps_lat = call_struct.gps_lat;
	call_fifo.gps_long = call_struct.gps_long;
	call_fifo.FTJ_nbr = -1;

}  								/* end fill_call_rec() */

fill_cust_rec()
{
  struct cisam_cust   *cust;
  int rc,i;
  int do_customer_update = FALSE;
  time_t   t;
  char key;

  /* check if we retrieved CUSTOMER record for this call */
  if ( gCustRec.made_calls > 0 )
    {
      /* have we modified contents of CUSTOMER record? */
      if ( ( strcmp(gCustRec.pckup_str_name, call_struct.pckup_str_name) ) ||
	   ( strcmp(gCustRec.passenger_name, call_struct.passenger_name) ) ||
	   ( strcmp(gCustRec.pckup_city, call_struct.pckup_city) ) ||
	   ( gCustRec.pckup_str_nbr != call_struct.pckup_str_nbr ) ||
           ( gCustRec.fleet != call_struct.fleet ) ||
           ( strncmp(gCustRec.veh_attr, call_struct.veh_attr, strlen(call_struct.veh_attr) ) ) ||
           ( strncmp(gCustRec.drv_attr, call_struct.drv_attr, strlen(call_struct.drv_attr ) ) ) ||
           ( strncmp(gCustRec.customer_comment, call_struct.general_cmnt, strlen(call_struct.general_cmnt ) ) ) ||
	   ( strcmp(gCustRec.pckup_apt_room, call_struct.pckup_apt_room) ) )
	{
	  /* ask customer if they want to modify CUSTOMER record */
          if ( ( !(0) ) || ( ui_phone_update ) )
            prt_error( UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_63, "Customer record has changed. Would you like to save these changes (Y/N)?") );
          else
	    {
	      if ( (0) && ( call_struct.extended_type[2] == TIMEC ) )
		{
		  prt_error( UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_63, "Customer record has changed. Would you like to save these changes (Y/N)?") );
		  key = 0;
		}
	      else
		key = YES;	      
	    }


	  
	  while ( key != YES && key != NO )
	    {
	      key = Toupper( wgetkey( stdscr ) );
	    }
	  if ( key == YES )
	    do_customer_update = TRUE;
	}

    }

  cust = (struct cisam_cust *)req_buf;
  bzero( cust, sizeof( struct cisam_cust ) );

#ifdef NAMERICA
  strcpy(cust->phone_nbr, call_struct.phone_nbr );
#else
  strcpy(cust->phone_nbr, call_struct.phone_number);
#endif

  // no phone number to query
  if ( !strlen(cust->phone_nbr) )
    return;

  // TSAB doesn't want mobile phone prefixed phone numbers saved
  if ( (0) && ( call_struct.phone_nbr[0] == '0' ) && ( call_struct.phone_nbr[1] == '7' ) )
    return;


  // non-numeric phone...
  for ( i = 0; i < strlen(cust->phone_nbr); i++ )
    {
      if ( !isdigit((int)cust->phone_nbr[i]) )
        return;
    }

  // Don't store a number with all '0'
  if ( atoi( call_struct.phone_nbr ) == 0 )
    return;

  
  t = time(0);
  
  if (db(CUSTOMER_FILE_ID, READ_KEYS, &cust_key1, ISEQUAL, 0) < 0)
    {
      /** New customer record to add **/
      strcpy(cust->pckup_str_name, call_struct.pckup_str_name);
      
      if ( ( !(0) ) || ( ui_phone_update ) )
        {
          strcpy(cust->passenger_name, call_struct.passenger_name);
          strcpy(cust->customer_comment, call_struct.general_cmnt);
          strcpy(cust->veh_attr, call_struct.veh_attr);
          strcpy(cust->drv_attr, call_struct.drv_attr);
        }
      
      strcpy(cust->pckup_city, call_struct.pckup_city);
      cust->pckup_str_nbr = call_struct.pckup_str_nbr;
      cust->pckup_zone = call_struct.pckup_zone;
      strcpy(cust->pckup_apt_room, call_struct.pckup_apt_room);
      cust->access_date_time = t;
      cust->made_calls = 1;
      cust->fleet = call_struct.fleet;
      if (db(CUSTOMER_FILE_ID, ADD_REC, &cust_key1, 0, 0) < 0)
	{
	  prt_error("Unable to Add New Customer Record");
	}
    }
  else
    {
      if ( do_customer_update == TRUE )
	{
	  strcpy(cust->pckup_str_name, call_struct.pckup_str_name);
          
          if ( ( !(0) ) || ( ui_phone_update ) )
            {
              strcpy(cust->passenger_name, call_struct.passenger_name);
              strcpy(cust->customer_comment, call_struct.general_cmnt);
              strcpy(cust->veh_attr, call_struct.veh_attr);
              strcpy(cust->drv_attr, call_struct.drv_attr);
            }
          
	  strcpy(cust->pckup_city,     call_struct.pckup_city);
	  strcpy(cust->pckup_apt_room, call_struct.pckup_apt_room);
	  cust->pckup_str_nbr =        call_struct.pckup_str_nbr;
	  cust->fleet =                call_struct.fleet;
	  cust->pckup_zone =           call_struct.pckup_zone;
          cust->made_calls++;
          cust->access_date_time = t;
          if (0)
            {
              if (db(CUSTOMER_FILE_ID, UPDATE_REC, &cust_key1, 0, 0) < 0)
                {
                  prt_error("Unable to Update Customer Record");
                }
            }
          else
            {
              if (db(CUSTOMER_FILE_ID, ADD_REC, &cust_key1, 0, 0) < 0)
                {
                  prt_error("Unable to Update Customer Record");
                }
            }
	}
      else
        {
          cust->made_calls++;
          cust->access_date_time = t;      
          if (db(CUSTOMER_FILE_ID, UPDATE_REC, &cust_key1, 0, 0) < 0)
            {
              prt_error("Unable to Update Customer Record");
            }
        }
    }
}


switch_pickup_dest( )
{
  struct cisam_cl tmp_call_struct;

  bzero( &tmp_call_struct, sizeof( struct cisam_cl ) );

  tmp_call_struct.pckup_str_nbr = call_struct.dest_str_nbr;
  tmp_call_struct.pckup_str_suf = call_struct.dest_str_suf;
  strcpy(tmp_call_struct.pckup_pre_dir, call_struct.dest_pre_dir);
  strcpy(tmp_call_struct.pckup_str_name, call_struct.dest_str_name);
  strncpy(tmp_call_struct.pckup_str_key, call_struct.dest_str_name, 4);
  strcpy(tmp_call_struct.pckup_str_type, call_struct.dest_str_type);
  strcpy(tmp_call_struct.pckup_post_dir, call_struct.dest_post_dir);
  strcpy(tmp_call_struct.pckup_apt_room, call_struct.dest_apt_room);
  strcpy(tmp_call_struct.pckup_city, call_struct.dest_city);
  tmp_call_struct.pckup_zone = call_struct.dest_zone;
  
  tmp_call_struct.dest_str_nbr = call_struct.pckup_str_nbr;
  tmp_call_struct.dest_str_suf = call_struct.pckup_str_suf;
  strcpy(tmp_call_struct.dest_pre_dir, call_struct.pckup_pre_dir);
  strcpy(tmp_call_struct.dest_str_name, call_struct.pckup_str_name);
  strcpy(tmp_call_struct.dest_str_type, call_struct.pckup_str_type);
  strcpy(tmp_call_struct.dest_post_dir, call_struct.pckup_post_dir);
  strcpy(tmp_call_struct.dest_apt_room, call_struct.pckup_apt_room);
  strcpy(tmp_call_struct.dest_city, call_struct.pckup_city);
  tmp_call_struct.dest_zone = call_struct.pckup_zone;


  /* put destination into global call_struct */
  call_struct.dest_str_nbr = tmp_call_struct.dest_str_nbr;
  call_struct.dest_str_suf = tmp_call_struct.dest_str_suf;
  strcpy(call_struct.dest_pre_dir, tmp_call_struct.dest_pre_dir);
  strcpy(call_struct.dest_str_name, tmp_call_struct.dest_str_name);
  strcpy(call_struct.dest_str_type, tmp_call_struct.dest_str_type);
  strcpy(call_struct.dest_post_dir, tmp_call_struct.dest_post_dir);
  strcpy(call_struct.dest_apt_room, tmp_call_struct.dest_apt_room);
  strcpy(call_struct.dest_city, tmp_call_struct.dest_city);
  call_struct.dest_zone = tmp_call_struct.dest_zone;
  
  /* now put pickup into global call_struct */
  call_struct.pckup_str_nbr = tmp_call_struct.pckup_str_nbr;
  call_struct.pckup_str_suf = tmp_call_struct.pckup_str_suf;
  strcpy(call_struct.pckup_pre_dir, tmp_call_struct.pckup_pre_dir);
  strcpy(call_struct.pckup_str_name, tmp_call_struct.pckup_str_name);
  strncpy(call_struct.pckup_str_key, tmp_call_struct.pckup_str_name, 4);
  strcpy(call_struct.pckup_str_type, tmp_call_struct.pckup_str_type);
  strcpy(call_struct.pckup_post_dir, tmp_call_struct.pckup_post_dir);
  strcpy(call_struct.pckup_apt_room, tmp_call_struct.pckup_apt_room);
  strcpy(call_struct.pckup_city, tmp_call_struct.pckup_city);
  call_struct.pckup_zone = tmp_call_struct.pckup_zone;  
}
/* Print a char to a pipe. If printer is a DEC LA210
   display characters with attributes underlined. If
   force_underline is 1, print highlighted characters
   by putting an underscore beneath them. */
void
char_print(dec_la210, force_underline, c, pptr)
int dec_la210;
int force_underline;
int c;
FILE *pptr;
{
    static int underline = 0;

    if (dec_la210) {
	if (c & A_ATTRIBUTES)  {

	    /* Turn on underline */
	    if (!underline) {
		underline = 1;
		fprintf(pptr, "\033[4m");
	    }
	}
	else {

	    /* Turn off underline */
	    if (underline) {
		underline = 0;
		fprintf(pptr, "\033[24m");
	    }
	}
	fputc(c & A_CHARTEXT, pptr);
    }
    else if (force_underline) {

	fputc(c & A_CHARTEXT, pptr);

	/* Print backspace then an underscore for highlighted text */
	if (c & A_ATTRIBUTES) 
	    fprintf(pptr, "\b_");
    }
    else
	fputc(c & A_CHARTEXT,pptr);
	    
	    
}

/********************************************************************************************************/
/* print_scr: prints the screen on display on the line printer. It uses the environment 		*/
/*	variable MADS_SPOOLER, as the spooler daemon, if this environment is not			*/
/*	declared, then the default spooler is used.
/********************************************************************************************************/
print_scr(win_fld)
WINDOW *win_fld;			/* NULL if there is no window on display, else pointer to window */
{
	FILE *popen();
	FILE *pptr;
	int col, line, c;
	char *mads_spooler;		/* program name used for sending output to printer */
	char *getenv();
	int wline, wcol;
	char sh_command[80];
	long curr_time;
        char date_time[40];
	int dec_la210, force_underline;
	int i, j, screen[25][133];


	if ((mads_spooler = getenv("MADS_SPOOLER")) == NULL) {		/* get the environment variable, MADS_SPOOLER */
		mads_spooler = PRINT_CMD;			/* use default value = "/usr/ucb/lpr" */
	}
	
	if (getenv("DECLA210") != NULL)
	    dec_la210 = 1;
	else
	    dec_la210 = 0;

	if (getenv("UNDERLINE") != NULL)
	    force_underline= 1;
	else
	    force_underline= 0;

	sprintf(sh_command,"%s 2> /dev/null", mads_spooler );		/* save command to be exected */

	if ((pptr = popen(sh_command, "w")) == NULL) {
		prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_10, "<cannot open pipe for lp>"));
		return;
	}

	curr_time = time(NULL);
        local_date_time(curr_time, NULL, NULL, date_time);
	fprintf(pptr, "%s\n", date_time);
	


	/* First get stdscr */
	for (i = 0; i < LINES ; i++)
	  for (j = 0; j < COLS ; j++)
	    screen[i][j] = mvinch(i, j);

	/* Get window passed in */
	if (win_fld != NULL)
	{
	  for (i = 0; i < win_fld->_maxy; i ++)
	    for (j = 0; j < win_fld->_maxx; j ++)
	      screen[i + win_fld->_begy][j + win_fld->_begx] = mvwinch(win_fld, i, j);
	}

	/* If mesg_wind is up (Not a passed parameter).
	   Do not show the mesg_wind if it overlaps the passed in win_fld */
	if (mesg_wind != NULL && (win_fld == NULL || win_fld->_begy > mesg_wind->_begy + mesg_wind->_maxy))
	{
	  for (i = 0; i < mesg_wind->_maxy; i ++)
	    for (j = 0; j < mesg_wind->_maxx; j ++)
	      screen[i + mesg_wind->_begy][j + mesg_wind->_begx] = mvwinch(mesg_wind, i, j);
	}


	/* Send everything to printer */
	for (i = 0; i < LINES ; i++)
	{
	  for (j = 0; j < COLS ; j++)
	    char_print(dec_la210, force_underline, screen[i][j], pptr);
	  fputc('\n',pptr);
	}

#if 0
	if (win_fld == NULL) {						/* there is no window on display */
		col = 0;
		for (line = 0; line < LINES ; line++) {
			for (col = 0; col < COLS ; col++) {
				move(line,col);
				c = inch();
				char_print(dec_la210, force_underline, c, pptr);
			}
			fputc('\n',pptr);
		}
	}
	else {								/* print the stdscr until the window is reached */
		col = 0;
		for (line = 0; line < START_SRCHWIN_ROW ; line++) {
			for (col = 0; col < COLS ; col++) {
				move(line,col);
				c = inch();
				char_print(dec_la210, force_underline, c, pptr);
			}
			fputc('\n',pptr);
		}
		wcol = 0;						/* print the window */
		for (wline = 0; wline < SRCH_WIN_LINES ; wline++) {
			for (wcol = 0; wcol < COLS ; wcol++) {
				wmove(win_fld,wline,wcol);
				c = winch(win_fld);
				char_print(dec_la210, force_underline, c, pptr);
			}
			fputc('\n',pptr);
		}
		col = 0;						/* print the stdscr portion after the window */
		for (line = START_SRCHWIN_ROW+SRCH_WIN_LINES; line < LINES ; line++) {
			for (col = 0; col < COLS ; col++) {
				move(line,col);
				c = inch();
				char_print(dec_la210, force_underline, c, pptr);
			}
			fputc('\n',pptr);
		}
	}
#endif

	/* Output status line */
	if (!No_status_line)
	    fprintf(pptr, "%s\n", uiuser[user_index]->img_25);

	pclose(pptr);

	/* inform user that screen image is being printed */	
	prt_error(UI_PRT_SCR,"<%s %s>", catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_11, "on printer"), mads_spooler);
}

/*******************************************************************/
/* previous_scr: displays the previous screen which has been saved */
/*******************************************************************/
previous_scr()
{	
	if ( prev_scr_name == -1 )  {
		prt_error(UI_NOSCR,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_12, "<no previous screen saved>"));
		return;
	}  /* end if */
		
	if ( menu_manager(YES,prev_scr_name) == DONE)    {
		/* previous screen has been set to current screen */
		prev_scr_flag = 1;
		prev_scr_name = -1;	/* undefine prev scr */
	}  /* end if */

}  /* end previous_scr() */


/* screen_toggle: called when the screen_toggle function key is pressed.
 * 	This function moves the user from the basic_call_entry screen
 *	to the extended_call_entry screen, and vice versa.
 */
toggle_screen()
{
	
	/* set the value of toggle_piu */

  toggle_piu = 3;	/* go to third field on screen */

	/* go to the appropriate screen */
	switch ( scr_name ) {
		case BASIC_CALL_SCR:
			if ( menu_manager(YES,EXTENDED_CALL_SCR) == DONE ) 
				/* set the toggle_flag */
				/*indicates that this key was pressed*/
				toggle_flag = 1;
			break;
		case SUBS_BASIC_SCR:
			if ( menu_manager(YES,SUBS_EXTENDED_SCR) == DONE )
				/* set the toggle_flag */
				/*indicates that this key was pressed*/
				toggle_flag = 1;
			break;
		case EXTENDED_CALL_SCR:
                  if ( (1) && ( ui_phone ) )
                    toggle_piu = B_PH_AREA_CODE;
                  
			if (menu_manager(YES,BASIC_CALL_SCR) == DONE )
				/* set the toggle_flag */
				/*indicates that this key was pressed*/
				toggle_flag = 1;
			break;
		case SUBS_EXTENDED_SCR:
			if (menu_manager(YES,SUBS_BASIC_SCR) == DONE )
				/* set the toggle_flag */
				/*indicates that this key was pressed*/
				toggle_flag = 1;
			break;
		default:
			/* toggle key is not allowed from any other screen */
			prt_error(UI_NOFIELD,
				catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_13, "<toggle key not allowed from this screen>"));
			break;
	}  /* end switch */

}  /* end toggle_screen() */
				
#if 0
This is gone now since the DISPATCH_KEY can only be entered
from the exceptions screen now

/********************************************************************************/
/* dispatch_key - Perform the actions of a dispatch function key.		*/
/* 		  The dispatch key is only allowed for the following searches : */
/* 		  assigned calls, all calls, calls by vehicle/driver, and       */
/*  		  individual calls.  						*/
/********************************************************************************/
void dispatch_key(srch_index)
int srch_index;					/* Index to searches array of calls  */
{
	struct excpt_rec except_fifo;		/* for writing except resolution record to FIFO */
	struct cisam_cl *cl_ptr;

	if((scr_name != UNASSIGNED_CALLS_SCR) && (scr_name != ASSIGNED_CALLS_SCR)  && (scr_name != ALL_CALLS_SCR) && 
	   (scr_name != VEH_DR_CALLS_SCR) && (scr_name != INDIVIDUAL_CALLS_SCR))  {
		prt_error(UI_INV_KEY," ");
		return;
	}
	
	if (uiuser[user_index]->menu_perms.exceptions != 1){			/* if user is not a dispatcher ?*/
		prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_14, "<to manually assign vehicles>"));
		return;
	}
	   
	if(srch_calls[srch_index].grouped_with) {
		prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_15, "<dispatch not allowed for mult call>"));
		return;
	}

	add_noexcpt_callh(srch_index,R_DISPATCH,ACT_DISPATCH,0); /* Add a call history record indicating DISPATCH key was hit */

	cl_ptr = (struct cisam_cl *)req_buf;
	cl_ptr->nbr = srch_calls[srch_index].nbr;
	if((db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0)) < 0)  {
		prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_16, "<call record>"));
		return;
	}

	cl_ptr->dispd_date_time = (long)0; 			/* Update the call records on disk and in memory */
	srch_calls[srch_index].dispd_date_time = (long)0;
	strcpy(cl_ptr->dispd_date,"");
	strcpy(srch_calls[srch_index].dispd_date,"");
	strcpy(cl_ptr->dispd_time,"");
	strcpy(srch_calls[srch_index].dispd_time,"");
	cl_ptr->pckup_date_time = (long)0;
	strcpy(cl_ptr->pckup_date,"");
	strcpy(cl_ptr->pckup_time,"");
	strcpy(srch_calls[srch_index].pckup_time,"");
	cl_ptr->close_date_time = (long)0;
	srch_calls[srch_index].close_date_time = (long)0;
	strcpy(cl_ptr->close_date,"");
	strcpy(cl_ptr->close_time,"");
	strcpy(srch_calls[srch_index].close_time,"");
	cl_ptr->veh_nbr = (short)0;
	srch_calls[srch_index].veh_nbr = (short)0;
	cl_ptr->dispatched_by = (short)0;
	srch_calls[srch_index].dispatched_by = (short)0;
	cl_ptr->drv_id = (short)0;
	srch_calls[srch_index].drv_id = (short)0;
	cl_ptr->pri_status = srch_calls[srch_index].priority;
	strcpy(cl_ptr->status,UNASSGND);
	strcpy(srch_calls[srch_index].status,UNASSGND);

	if((db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0)) < 0)  {
		prt_error(UI_NOUPDATE,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_16, "<call record>"));
		return;
	}

	if((db(CALL_FILE_ID,RELEASE,&cl_key1,0,0)) < 0)  {
		prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_17, "<cannot release lock on call record>"));
		return;
	}

	except_fifo.u_prcs_id = pid; 		/* Build an exception record to send to dispatch, so the call can be assigned.  */
	except_fifo.rec_type = EXCEPT_REQ;
	except_fifo.except_ptr = '\0';
	except_fifo.except_nbr = 0;
	except_fifo.resolve_code = R_DISPATCH;
	except_fifo.call_nbr = srch_calls[srch_index].nbr;
	except_fifo.grouped_with = srch_calls[srch_index].grouped_with;
	except_fifo.veh_nbr = 0;
	except_fifo.zone_nbr = 0;
	except_fifo.old_zone_nbr = 0;
	except_fifo.old_due_time = 0;

	if(write(fifo_fd,&except_fifo,sizeof(struct excpt_rec)) == -1)  { 		/* Write FIFO for dispatch routine */
		prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_18, "<except rec to FIFO>"));
		return;
	}

	if(scr_name != INDIVIDUAL_CALLS_SCR )  {
		dsp_sum_line(srch_index);
		dim_line(srch_index);
	}
	prt_error(UI_CALL_DISPATCHED," ");
	return;
}  /* End of dispatch_key()  */
#endif /* if 0 */

/****************************************************************************/
/* resend_key - Perform the actions of a resend function key		    */
/* 		The resend key is only allowed for the following searches : */
/*	   	assigned calls, all calls, calls by vehicle/driver, and	    */
/*	   	individual calls.  					    */
/****************************************************************************/

void resend_key(srch_index)
int srch_index;		/* Index to searches array of calls  */
{
	struct excpt_rec except_fifo;	/* for writing except resolution record to FIFO */
        struct cisam_cl *cl_ptr;
        int             call_number;

	if((scr_name != ASSIGNED_CALLS_SCR)  && (scr_name != ALL_CALLS_SCR) && (scr_name != VEH_DR_CALLS_SCR) &&
	   (scr_name != INDIVIDUAL_CALLS_SCR))  {
		prt_error(UI_INV_KEY," ");
		return;
	}
	
	if (uiuser[user_index]->menu_perms.exceptions != 1){			/* if user is not a dispatcher ?*/
		prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_14, "<to manually assign vehicles>"));
		return;
	}
	   
	add_noexcpt_callh(srch_index,R_RESEND,ACT_RESEND,0); 	/* Add a call history record indicating RESEND key was hit */

	except_fifo.u_prcs_id = pid;				/* tell dispatch to resend info */
	except_fifo.rec_type = EXCEPT_REQ;
	except_fifo.except_ptr = '\0';
	except_fifo.except_nbr = 0;
	except_fifo.resolve_code = R_RESEND;
	except_fifo.call_nbr = srch_calls[srch_index].nbr;
	except_fifo.grouped_with = srch_calls[srch_index].grouped_with;
	except_fifo.veh_nbr = 0;
	except_fifo.zone_nbr = 0;
	except_fifo.old_zone_nbr = 0;
	except_fifo.old_due_time = 0;

	if(write(fifo_fd,&except_fifo,sizeof(struct excpt_rec)) == -1)  { 		/* Write FIFO for dispatch routine */
		prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_18, "<except rec to FIFO>"));
		return;
	}

	if(scr_name != INDIVIDUAL_CALLS_SCR)  {
		dsp_sum_line(srch_index);
		dim_line(srch_index);
	}

	return;
} /* End of resend_key()  */

/* Step through the exceptions looking for one
   with the given call number and a HOLD */
hold_exists_for_call(call_num)
int call_num;
{
  struct excepts *ex;

  ex = offset->first_except;

  while (ex)
  {
    if (ex->call_nbr == call_num &&
	ex->type == SUP_HOLD)
      return(1);
    ex = ex->next;
  }

  return(0);
}

/****************************************************************************
* hold_key - Perform the actions of a hold function key
****************************************************************************/

void hold_key(srch_index)
int srch_index;							/* Index to searches array of calls  */
{
	struct excpt_rec except_fifo;				/* for writing except resolution record to FIFO */

	/* allowed only from search screen from calls file */
	if ( scr_name != UNASSIGNED_CALLS_SCR  && scr_name != ASSIGNED_CALLS_SCR &&
	     scr_name != ALL_CALLS_SCR  && scr_name != VEH_DR_CALLS_SCR )  {
		prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_19, "<hold key from this screen>"));
		return;
	}
	if((*security[map_array[EXCEPTIONS_SCR]])() == NOT_ALLOWED)  { 		/* The hold key is only allowed for supervisors */
		prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_20, "<hold key>"));
		return;
	}

	/* hold is only allowed for unassigned & assigned calls; is not allowed for
	 * completed calls, killed calls, cancelled calls for calls with status of pickup 
	 */
	if ( (strcmp(srch_calls[srch_index].status,PICKUP) == 0) || (strcmp(srch_calls[srch_index].status,COMPLETE) == 0) ||
		(strcmp(srch_calls[srch_index].status,KILLED) == 0) || (strcmp(srch_calls[srch_index].status,CANCELD) == 0) )  	{
		prt_error(UI_NOFIELD,"<%s %s %s>", 
			catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_21, "hold not allowed for calls with"), srch_calls[srch_index].status, catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_22, "status") );
		return;
	}

	if(srch_calls[srch_index].grouped_with) {
		prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_23, "<hold not allowed for mult call>"));
		return;
	}

	/* No more than one per call at a time */
	if (hold_exists_for_call(srch_calls[srch_index].nbr))
	{
	  prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_33, "There is already a HOLD on the call"));
	  return;
	}

	add_noexcpt_callh(srch_index,R_SUP_HOLD,ACT_HOLD,0); 	/* Add a call history record indicating HOLD key was hit */

	except_fifo.u_prcs_id = pid; 					/* Build an exception record to send to dispatch */
	except_fifo.rec_type = EXCEPT_REQ;
	except_fifo.except_ptr = '\0';
	except_fifo.except_nbr = 0;
	except_fifo.resolve_code = R_SUP_HOLD;
	except_fifo.call_nbr = srch_calls[srch_index].nbr;
	except_fifo.grouped_with = srch_calls[srch_index].grouped_with;
	except_fifo.veh_nbr = 0;
	except_fifo.zone_nbr = 0;
	except_fifo.old_zone_nbr = 0;
	except_fifo.old_due_time = 0;

	if(write(fifo_fd,&except_fifo,sizeof(struct excpt_rec)) == -1)  { 		/* Write FIFO for dispatch routine */
		prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_18, "<except rec to FIFO>"));
		return;
	}

	if(scr_name != INDIVIDUAL_CALLS_SCR)  {
		dsp_sum_line(srch_index);
		dim_line(srch_index);
	}

	return;
} /* End of hold_key()  */

/****************************************************************************
* callback_key - Perform the actions of a callback function key
****************************************************************************/

void callback_key(srch_index)
int srch_index;		/* Index to searches array of calls  */
{
	struct excpt_rec except_fifo;	/* for writing except resolution record to FIFO */

	/* not allowed from screens other than search screens */
	if ( scr_name != UNASSIGNED_CALLS_SCR && scr_name != ASSIGNED_CALLS_SCR && scr_name != ALL_CALLS_SCR  &&
	     scr_name != VEH_DR_CALLS_SCR  && scr_name != INDIVIDUAL_CALLS_SCR ) {
		prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_24, "<callback key from this screen>"));
		return;
	}
		
	/* A call must be highlighted */

	if(srch_index == NOT_AVAILABLE)  {
		prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_25, "<no call highlighted>"));
		return;
	}
	   
	/* Add a call history record indicating CALLBACK key was hit */

	add_noexcpt_callh(srch_index,R_CALLBACK,ACT_CALLBACK,0);

	/* Build an exception record to send to dispatch, so the call can
	   be assigned.  */

	except_fifo.u_prcs_id = pid;
	except_fifo.rec_type = EXCEPT_REQ;
	except_fifo.except_ptr = '\0';
	except_fifo.except_nbr = 0;
	except_fifo.resolve_code = R_CALLBACK;
	except_fifo.call_nbr = srch_calls[srch_index].nbr;
	except_fifo.grouped_with = srch_calls[srch_index].grouped_with;
	except_fifo.veh_nbr = 0;
	except_fifo.zone_nbr = 0;
	except_fifo.old_zone_nbr = 0;
	except_fifo.old_due_time = 0;

	/* Write FIFO for dispatch routine */

	if(write(fifo_fd,&except_fifo,sizeof(struct excpt_rec)) == -1)  {
		prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_18, "<except rec to FIFO>"));
		return;
	}

	if(scr_name != INDIVIDUAL_CALLS_SCR)  {
		dsp_sum_line(srch_index);
		dim_line(srch_index);
	}

	/* update user statistics */
	bump_stat(srch_calls[srch_index].fleet,CALLING_BACK);
	prt_error(UI_CALL_BACK," ");

	return;
}  /* End of callback_key()  */
				
/****************************************************************************
* cancel_key - Perform the actions of a cancel function key
****************************************************************************/

void 
cancel_key(piu_ptr, srch_index)
int *piu_ptr;		/* pointer to piu in use on call entry screens */
int srch_index;		/* Index to searches array of calls  */
{
	char cncl_act;
	int i;
	struct excpt_rec except_fifo;	/* for writing except resolution record to FIFO */
	struct cisam_cl *cl_ptr;
	char loop_count;		/* # of requetss to be written to FIFO */
	int	call_nbr;		/* Counter for canceling multiple calls */
	int	call_save;		/* Stores grouped with call for next search */
	int	indx;			/* Index for calls in memory */

	switch(scr_name) {						/* processing depends upon the screen on display */
		case BASIC_CALL_SCR:
			zip1 = zip2 = 0;
			call_entry_cancel(basic_scr_flds,MAX_PIU_BASIC,b_prt_default,piu_ptr);
			return;

		case SUBS_BASIC_SCR:
			zip1 = zip2 = 0;
			call_entry_cancel(basic_scr_flds,MAX_PIU_BAS_SUBS,b_prt_default,piu_ptr);
			return;

		case EXTENDED_CALL_SCR:
			zip1 = zip2 = 0;
			call_entry_cancel(extended_scr_fld,MAX_PIU_EXTENDED,e_prt_default,piu_ptr);
			charge_related_fields(C_CLEAR,NULL,&call_struct); 	
			return;

		case SUBS_EXTENDED_SCR:
			zip1 = zip2 = 0;
			charge_related_fields(C_CLEAR,NULL,&call_struct); 		/* clear the charge # fields, etc */
			call_entry_cancel(extended_scr_fld,MAX_PIU_EXT_SUBS,e_prt_default,piu_ptr);
			return;

		case WAKEUP_CALL_SCR:
			call_entry_cancel(wakeup_scr_fld,MAX_PIU_WAKEUP,w_prt_default,piu_ptr);
			return;

		case SUBS_WAKEUP_SCR:
			call_entry_cancel(wakeup_scr_fld,MAX_PIU_WAK_SUBS,w_prt_default,piu_ptr);
			return;

		case SUBS_FUTURE_CLS_SCR:
			cancel_subs(srch_index);
			return;

		case UNASSIGNED_CALLS_SCR:
		case ASSIGNED_CALLS_SCR:
		case VEH_DR_CALLS_SCR:
		case ALL_CALLS_SCR:
		case INDIVIDUAL_CALLS_SCR:
			if(srch_index == NOT_AVAILABLE)  { 				/* A call must be highlighted */
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_25, "<no call highlighted>"));
				return;
			}

			cncl_act = '1';							/* default value */
			for(i = 0; i < FLEET_MAX; i++)  {
				if(fleet[i]->fleet_id == srch_calls[srch_index].fleet)  {
					cncl_act = fleet[i]->cancel_action;
					break;
				}
			}

			call_nbr = srch_calls[srch_index].nbr;

			if (call_nbr != 0)
			  {
			    /* Check if call has already been cancelled. Note that a cancelled call can be
			       revived, so look at the last entry in call history */
			    if (get_last_exception_type_from_callh(call_nbr) == CALL_CANCEL)
			      {
				prt_error(UI_CANCELLED_CALL, "");
				return;
			      }
			  }

			while(call_nbr != 0) {
				for(indx = 0; indx < 17; indx++)
					if(call_nbr == srch_calls[indx].nbr)
						break;

				if(indx < 17)
					add_noexcpt_callh(indx,R_CANCEL,ACT_CANCEL,0);		/* add a call history record */
				else
					add_mult_h(call_nbr, srch_calls[srch_index].fleet);

				cl_ptr = (struct cisam_cl *)req_buf;
				cl_ptr->nbr = call_nbr;

				if((db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0)) < 0)  {
					prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_16, "<call record>"));
					return;
				}

				call_save = cl_ptr->grouped_with;

				if(cncl_act == '3')  {
					if(indx < 17) {
						strcpy(srch_calls[indx].status,CANCELD);
						srch_calls[indx].pri_status = (short)64;
						srch_calls[indx].grouped_with = 0L;
						if(scr_name != INDIVIDUAL_CALLS_SCR)
							dsp_sum_line(indx);
					}
					strcpy(cl_ptr->status,CANCELD);
					cl_ptr->pri_status = (short)64;
					cl_ptr->grouped_with = 0L;

					if((db(CALL_FILE_ID, UPDATE_REC, &cl_key1, 0, 0)) < 0) {	/* R2 */
						prt_error(UI_NOUPDATE, "<call record>");
						return;
					}
				}
				else 
					prt_error(UI_TO_DISPATCH,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_26, "<proceed>"));

				if((db(CALL_FILE_ID,RELEASE,&cl_key1,0,0)) < 0)  {
					prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_17, "<cannot release lock on call record>"));
					return;
				}

				/* Build an exception record to send to dispatch, so the call can be assigned */
				except_fifo.u_prcs_id = pid;
				except_fifo.rec_type = EXCEPT_REQ;
				except_fifo.except_ptr = '\0';
				except_fifo.except_nbr = 0;
				except_fifo.resolve_code = R_CANCEL;
				except_fifo.call_nbr = call_nbr;
				except_fifo.grouped_with = srch_calls[srch_index].grouped_with;
				except_fifo.veh_nbr = 0;
				except_fifo.zone_nbr = 0;
				except_fifo.old_zone_nbr = 0;
				except_fifo.old_due_time = 0;

				if(write(fifo_fd,&except_fifo,sizeof(struct excpt_rec)) == -1) {	/* Write FIFO to dispatch */
					prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_18, "<except rec to FIFO>"));
					return;
				}
				call_nbr = call_save;
				if(call_nbr == srch_calls[srch_index].nbr)		/* Don't do first call again */
					call_nbr = 0L;
			}

			if(scr_name != INDIVIDUAL_CALLS_SCR)  {
				dsp_cl_detail(srch_index + 1);
				dsp_sum_line(srch_index);
				dim_line(srch_index);
			}

			/* update user statistics */
			bump_stat(srch_calls[srch_index].fleet,CANCELLING_CALL);
			return;

		default:
			prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_27, "<cancel key from this screen>"));
			return;

	}
}

/************************************************************************************************/
/* call_entry_cancel:	processes the cancel function for call entry screens 			*/
/*			Writes a call record to disk, with status = KILLED, pri_status = 64;	*/
/*			and any other data entry fields thatwere entered by the user. It then	*/
/*			resets all the global flags for call entry processing, clears the screen*/
/*			and paints back default values						*/
/************************************************************************************************/
call_entry_cancel(ptr_scr_fld,max_piu,func_ptr_defs,piu_ptr)
struct scr_flds *ptr_scr_fld;				/* pointer to approp piu struct array */
int max_piu;						/* max piu value for the screen on display */
int (*func_ptr_defs)();					/* pointer to b_prt_defaults or e_prt_defaults() or w_prt_defaults()*/
int *piu_ptr;						/* pointer to piu in use on the call entry screen */
{
	int i;						/* scratch variable */

	strcpy(call_struct.status, KILLED);				/* write call record to disk; with KILLED status */
	call_struct.pri_status = (short)64;				/* and pri_status = 64 */
	call_struct.entered_by = (short)uid;
	strncpy(call_struct.pckup_str_key,call_struct.pckup_str_name,4);
	call_struct.pckup_str_key[4] = '\0';
	strncpy(call_struct.pass_name_key,call_struct.passenger_name,4);
	call_struct.pass_name_key[4] = '\0';
	call_struct.date_time_in = time(NULL);
	get_pkup_date(call_struct.date_in);
	get_pkup_time(call_struct.time_in);
	call_struct.due_date_time = get_internal_time(call_struct.due_date, call_struct.due_time);

	if (scr_name == SUBS_BASIC_SCR || scr_name == SUBS_EXTENDED_SCR || scr_name == SUBS_WAKEUP_SCR) {
		;	/* do nothing */
	}
	else {								/* its a regular call */
		memcpy(req_buf,(char *)&call_struct, sizeof(struct cisam_cl));
		if (db(CALL_FILE_ID,ADD_REC,&cl_key1,ISEQUAL,0) < 0) {	/* write call record to call file */
			prt_error(UI_WRITE,"<%s>", CALL_FILE);			/* db() failed to write rec. */
		}
	}

	bump_stat(call_struct.fleet,KILLING_CALL);			/* update user statistics */
	get_zone_desc(C_CLEAR, PICKUP_ADDR, &call_struct, NULL);				/* clear the zone_desc */
	get_zone_desc(C_CLEAR, DEST_ADDR, &call_struct, NULL);	
	get_est_wait_time(0);						/* clear the estimated time field area */
	print_addr_comment(C_CLEAR,PICKUP_ADDR);				/* clear the pkup and dest comments */
	print_addr_comment(C_CLEAR,DEST_ADDR);
	prt_entry_areas(stdscr,ptr_scr_fld,max_piu);			/* clear all the fields */
	attrset(0);							/* initialize flags for next call */
	inters_flds(C_CLEAR,ptr_scr_fld);					/* clear the underlining from intersection fields */

	write_dispatch = 1;						/* write to fifio */
	write_disk = 1;							/* write to call file */
	write_subs = 0;							/* do not write to subs file */
	call_zoned = 0;							/* call not zoned */
	user_knows = 0;							/* user does not know */
	inters_flag = 0;						/* set flag, not to read intersection address*/
	call_zoned = 0;							/* call not zoned */
	user_knows = 0;							/* user does not know */
	inters_flag = 0;						/* so as not to read the intersection address */

	if (scr_name == WAKEUP_CALL_SCR || scr_name == SUBS_WAKEUP_SCR)
		*piu_ptr = W_NAME;					/* go to the name field */
	else if (scr_name == EXTENDED_CALL_SCR)
		*piu_ptr = 2;
	else
		*piu_ptr = 3;						/* go to pkup address field */
	
	memset((char *)&call_struct,'\0',sizeof(struct cisam_cl));	/* initialize call_struct */

/*
 *	fix
 */
	num_vh_attr_s = num_dr_attr_s = 0;

	memset((char *)&call_fifo,'\0',sizeof(struct call_ipc_rec));	/* initialize call fifo structure */

	if (scr_name == SUBS_BASIC_SCR || scr_name == SUBS_EXTENDED_SCR || scr_name == SUBS_WAKEUP_SCR)  
		memset((char *)&subs_struct,'\0',sizeof(struct cisam_sc));	/* initialize the subscription calls structure */
			
	(*func_ptr_defs)();						/* print the default again */

	prt_error(UI_CALL_CANCEL," ");
}

/********************************************************************************************************/
/* send_to_dispatcher: processes the SENS_TO_DISPATCHER function key. Displays the msg window		*/
/*	and creates the exception of call without msg or call with msg (if processing is from		*/
/*	a call entry screen). msg # should be written toio the FIFO					*/
/********************************************************************************************************/
send_to_dispatcher(piu_ptr)
int *piu_ptr;								/* pointer to piu in use */
{
	int	ret_val;

	if (scr_name != BASIC_CALL_SCR && scr_name != EXTENDED_CALL_SCR && scr_name != WAKEUP_CALL_SCR) {
		prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_28, "<send to dispatcher key from this screen>"));		/* called from wrong screen */
		return;
	}

	send_to_dis_flag = 1;
	ret_val = call_mesgs();							/* processing inside the msg window */
	send_to_dis_flag = 0;

	if(global_msg_num != -1) {
		if(global_msg_num > 0) {		/* call with msg bit for generating exception */
			set_bit(CALL_W_MSG - 1, sizeof(struct except_bits), (unsigned char *)&call_fifo.status);
			set_call_bit(CALL_W_MSG - 1);
		}
		else {
			set_bit(CALL_NO_MSG - 1, sizeof(struct except_bits), (unsigned char *)&call_fifo.status);
			set_call_bit(CALL_NO_MSG - 1);
		}

		if(ret_val == SEND_KEY)
			send_call(piu_ptr, NO, NO, NO, NO);			/* process this call */
		else
			send_call(piu_ptr, YES, NO, NO, NO);			/* process this call */

		/* Clean out bit fields in case send failed and call is still on screen */

		if(global_msg_num > 0) {		/* call with msg bit for generating exception */
			call_fifo.status.call_msg = 0;
			unset_call_bit(CALL_W_MSG - 1);
		}
		else {
			call_fifo.status.call_no_msg = 0;
			unset_call_bit(CALL_NO_MSG - 1);
		}
	}

	global_msg_num = 0;						/* set the msg # to 0, for next processing */
}

/********************************************************************************************************/
/* send as multiple call: processes the MULT function key. Displays the msg window                      */
/*      and creates the exception of multiple call (if processing is from                               */
/*      a call entry screen). msg # should be written toio the FIFO                                     */
/*      Multiple Call processing updated by cjs 010890                                                  */
/********************************************************************************************************/

send_as_multiple_call(piu_ptr, search_index)
int *piu_ptr;                                                           /* pointer to piu in use */
int search_index;
{
        int     ret_val;
        short   basic_screen;


/*
 *      allow mult key on 24 screen to set a mult exception on CANCELD calls
 *              the exception will be set immediately (the user doesn't need
 *              to hit the <send> key)
 */
        if (scr_name == ALL_CALLS_SCR)
          {
                if(search_index == NOT_AVAILABLE)
                  {
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_25, "<no call highlighted>"));
                        return;
                  }
                if (!strncmp(srch_calls[search_index].status, CANCELD, 4))
                  {
                        generate_multiple_exception(search_index);
                        return;
                  }
          }

        if (scr_name != BASIC_CALL_SCR && scr_name != EXTENDED_CALL_SCR &&
            scr_name != SUBS_BASIC_SCR && scr_name != SUBS_EXTENDED_SCR)
          {
                prt_error(UI_NOT_ALLOWED,"<multiple call key from this screen>");       /* called from wrong screen */
                return;
          }
 
        multiple_call_flag = 1;
        ret_val = call_mesgs();                                                 /* processing inside the msg window */
        multiple_call_flag = 0;

        if(global_msg_num != -1)
          {
/*
 *              set call type and exception type
 */
                set_multiple_call_type(call_struct.extended_type, 1);

                set_bit(MULT - 1, sizeof(struct except_bits), (unsigned char *)&call_fifo.status);
                set_call_bit(MULT - 1);

                if(ret_val == SEND_KEY)
                        send_call(piu_ptr, NO, NO, NO, NO);                     /* process this call no dups */
                else
                        send_call(piu_ptr, YES, NO, NO, NO);                     /* process this call dup */

		/* Unset call type, call status bit and fifo status bit in case of send error */

                set_multiple_call_type(call_struct.extended_type, 0);
		unset_call_bit(MULT - 1);
		call_fifo.status.multiple = 0;
          }

        global_msg_num = 0;                                             /* set the msg # to 0, for next processing */
}

/*-------------------------------------------------------------------
 *      set multiple call type
 *              sets extended type to multiple 'M'
 *-------------------------------------------------------------------
 */

set_multiple_call_type(char extended_type_field[], short on)
{
        int     i;
        short   found_null_terminator;
 
        if (!on)
	  extended_type_field[4] = '\0';
	else
	  {
	    for (i=0; i < 4; i++)
	      {
		if (extended_type_field[i] == '\0')
		  extended_type_field[i] = ' ';
	      }
		
	    extended_type_field[4] = MULT_CALL;
	  }

        extended_type_field[5] = '\0';

        return(1);
}

/*-------------------------------------------------------------------
 *      generate multiple exception
 *-------------------------------------------------------------------
 */
 
generate_multiple_exception(search_index)
        int                     search_index;
{
        struct cisam_cl *cl_ptr;
 
 
        memcpy(&call_struct, (char *)&srch_calls[search_index], sizeof(struct cisam_cl));
/*
 *      add call history entry
 */
        add_noexcpt_callh(search_index, R_MULT, ACT_MULTIPLE, 0);
 
/*
 *      update call
 */
/*
 *      set db call record
 */
        cl_ptr = (struct cisam_cl *)req_buf;
        cl_ptr->nbr = srch_calls[search_index].nbr;

        if((db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0)) < 0)
          {
		prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_7, "<call>"));
                return;
          }

        cl_ptr->pri_status = srch_calls[search_index].priority;
        strcpy(cl_ptr->status,UNASSGND);
        set_multiple_call_type(cl_ptr->extended_type, 1);
        cl_ptr->pri_status = (short)64;
        cl_ptr->grouped_with = 0L;

        if((db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0)) < 0)  {
		prt_error(UI_NOUPDATE,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_7, "<call>"));
                return;
        }

        if((db(CALL_FILE_ID,RELEASE,&cl_key1,0,0)) < 0)  {
		prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_17, "<cannot release lock on call record>"));
                return;
        }

/*
 *      set current display
 */
        strcpy(srch_calls[search_index].status, UNASSGND);
        srch_calls[search_index].pri_status = (short)64;
        srch_calls[search_index].grouped_with = 0L;
        set_multiple_call_type(srch_calls[search_index].extended_type, 1);
        dsp_sum_line(search_index);

/*
 *      build exception by resending call with MULTIPLE exception bit set
 */

        set_bit(MULT - 1, sizeof(struct except_bits), (unsigned char *)&call_fifo.status);
        set_call_bit(MULT - 1);

        fill_call_rec();
        if (write(fifo_fd, &call_fifo, sizeof(struct call_ipc_rec)) == -1)
          {
                prt_error(UI_WRITE,"<%s>", sys_errlist[errno]);
                return;
          }

/*
 *      update display
 */
        dsp_cl_detail(search_index + 1);
        dsp_sum_line(search_index);
        dim_line(search_index);

        return;
}


  
/** RETURNS:
              -1   ERROR _or_ NO TAXIS AVAILABLE AND USER ANSWERS 'NO'
	      0    NO TAXIS AVAILABLE AND USER ANSWERS 'YES'
	      1    TAXIS AVAILABLE IN PICKUP ZONE
**/
int
CheckAvailTaxis( pCall, piu_ptr )
     struct cisam_cl  *pCall;
     int              *piu_ptr;
{

  struct zones *pZone, *pBackupZone;
  struct veh_driv *pVeh;
  char key;
  int nbr_levels, pos_in_level, zone_nbr, i, rc, times_thru;
  struct cisam_zn ZoneRec;
  struct zone_rqst zone_request;
  ZONE_RESP *zone_response;
  ZONE_RESP_HNDL  hZoneResp;
  
  if ( pCall == NULL )
    return ( -1 );

  ZoneRec.fleet = fleet[ cur_fl_index ]->fleet_id;
  ZoneRec.nbr = pCall->pckup_zone;

  if ( db_read_key( ZONE_FILE_ID, &ZoneRec, &zn_key1, ISEQUAL ) != SUCCESS )
      return( -1 );

  if ( ZoneRec.check_avail_taxis == NO )
    return ( 1 );

  nbr_levels = 0;
  pos_in_level = 0;

  bzero( &zone_request.veh_attr, sizeof( zone_request.veh_attr ) );
  bzero( &zone_request.drv_attr, sizeof( zone_request.drv_attr ) );
  if (1)
    {
      for ( i = 0; i < ATTR_MAX; i++ )
	{
	  if ( call_struct.veh_attr[i] == YES )
	    set_bit( i, sizeof(zone_request.veh_attr), &zone_request.veh_attr );
	  if ( call_struct.drv_attr[i] == YES )
	    set_bit( i, sizeof(zone_request.drv_attr), &zone_request.drv_attr );
	}
    }	  
  zone_request.d_prcs_id = DISP_DEST;
  zone_request.u_prcs_id = pid;
  zone_request.rec_type = ZONE_REQ;
  zone_request.fleet = pCall->fleet;
  zone_request.zone_nbr = pCall->pckup_zone;

  msgsnd( msgkey, ( struct msgbuf * )&zone_request,
	 sizeof( struct zone_rqst ), IPC_NOWAIT );
  
  times_thru = 0;
  hZoneResp = TMALLOC( ZONE_RESP, 1 );
  zone_response = ( ZONE_RESP * )hZoneResp;
  while (times_thru <=2)
    {
      rc = msgrcv(msgkey,(struct msgbuf *)zone_response,
		  sizeof(struct zone_resp), pid, 0);
      if (rc == -1)
	{
	  if (errno != EINTR)
	    {
	      TFREE( hZoneResp );
	      return(-1);
	    }
	  if (++times_thru == 2)
	    {
	      TFREE( hZoneResp );
	      return(-1);
	    }
	}
      else if (zone_response->rec_type == ZONE_REQ)
	break;
      else
	{
	  TFREE( hZoneResp );
	  return(-1);
	}
    }  

  if ( zone_response->veh_posted > 0 )
    return( 1 );
  
  for ( i = 0; i < BCK_IN_ZN_RESP; i ++ )
    {
      if ( zone_response->bkup_zones[i][1] > 0 )
	return( 1 );
#ifdef FOO
      if ( nbr_levels <= fleet[ cur_fl_index ]->zone_backup_level )
	break;

	  if ( pVeh->t_status.posted && !pVeh->t_status.offered && !pVeh->t_status.on_break )
	    {
	      if ( taxi_avail_attr( pCall, pVeh ) >= 0 )
		return( 1 );
	    }
	  pVeh = pVeh->next;
      
      /** Now get the next zone in the backup list. Drop a level if there isn't another **/
      /** zone at this level **/
      if ( ( zone_nbr = pZone->level_number[nbr_levels].bck_up_zone[pos_in_level] ) <= 0 )
	{
	  /** drop a level **/
	  ++nbr_levels;
	  pos_in_level = 0;
	  zone_nbr = pZone->level_number[ nbr_levels ].bck_up_zone[pos_in_level];
	}
      else
	pos_in_level++;
      
      if ( zone_nbr > 0 )
	pBackupZone = fleet[ cur_fl_index ]->zone_ptr[ zone_nbr ];
      else
	pBackupZone == NULL;
#endif
      
    }
  
  /** NOW GET VALIDATION FROM THE USER THAT THEY REALLY WANT TO ENTER THE **/
  /** CALL EVEN IF NO CARS ARE AVAILABLE......                            **/

  prt_error( UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_29, "No taxis available in this zone. Would you still like to send? (Y/N)" ) );
  key = 0;
  while ( key != YES && key != NO )
    {
      question_for_user_active = TRUE;
      key = Toupper( wgetkey( stdscr ) );
      if ( key == NO )
	{
	  /** KILL CALL **/
	  clear_error();
	  switch ( scr_name )
	    {
	    case BASIC_CALL_SCR:
	      call_entry_cancel( basic_scr_flds, MAX_PIU_BASIC, b_prt_default, piu_ptr );
	      break;
	    case EXTENDED_CALL_SCR:
	      call_entry_cancel( extended_scr_fld, MAX_PIU_EXTENDED, e_prt_default, piu_ptr );
	      break;
	    }
          question_for_user_active = FALSE;
	  return( -1 );
	}
      if ( key == YES )
	{
	  clear_error();
          question_for_user_active = FALSE;
	  return ( 0 );
	}
    }
}
      
taxi_avail_attr( pCall, pVeh )
     struct cisam_cl *pCall;
     struct veh_driv *pVeh;
{
  unsigned char *addr1;
  unsigned char *addr2;
  short found, i, mult;
  struct veh_attributes CallVehAttr;
  struct drv_attributes CallDrvAttr;
  

  bzero( &CallVehAttr, sizeof( struct veh_attributes ) );
  bzero( &CallDrvAttr, sizeof( struct drv_attributes ) );
	
  /** CREATE BIT FIELD CONSISTING OF CALL'S VEHICLE AND DRIVER ATTRIBUTES **/
  for ( i = 0; i < ATTR_MAX; i++ )
    {
      if ( pCall->veh_attr[i] == YES )
	set_bit( i, sizeof( struct veh_attributes), &CallVehAttr );
    }
  for ( i = 0; i < ATTR_MAX; i++ )
    {
      if ( pCall->drv_attr[i] == YES )
	set_bit( i, sizeof( struct drv_attributes ), &CallDrvAttr );
    }
	
  addr1 = (unsigned char*)&pVeh->vehicle_attr;				/* get pointer to vehicle bits */
  addr2 = (unsigned char*)&CallVehAttr;
  if( *addr2 == ( *addr1 & *addr2))
    {
      addr1 = (unsigned char*)&pVeh->driver_attr;			/* get pointer to driver bits */
      addr2 = (unsigned char*)&CallDrvAttr;
      if( *addr2 == ( *addr1 & *addr2))
	return(0);
    }
  return(-1);
}



int
AddTimeCall( pCall )
     struct cisam_cl *pCall;
{
  TIME_CALL_REC  *pTimeCall;
  int rc;

  if ( pCall == NULL )
    return ( -1 );
  
  pTimeCall = ( TIME_CALL_REC * )tmp_req_buf;

  pTimeCall->cl_nbr = pCall->nbr;

  /** First see if we've got an entry in table **/
  if ( ( rc = db( TIME_CALL_FILE_ID, READ_KEYS, &time_call_key1, ISEQUAL, 0 ) ) < 0 )
    {
      /** No entry exists so we must do an ADD **/
      pTimeCall->fleet = pCall->fleet;
      pTimeCall->zone = pCall->pckup_zone;
      pTimeCall->due_date_time = pCall->due_date_time;
      pTimeCall->cl_nbr = pCall->nbr;
      strcpy( pTimeCall->drv_attr, pCall->drv_attr );
      strcpy( pTimeCall->veh_attr, pCall->veh_attr );

      if ( ( rc = db( TIME_CALL_FILE_ID, ADD_REC, &time_call_key1, ISEQUAL, 0 ) ) < 0 )
	{
	  return( -1 );
	}
    }
  else
    {
      /** Update existing record via UPDATE **/
      pTimeCall->fleet = pCall->fleet;
      pTimeCall->zone = pCall->pckup_zone;
      pTimeCall->due_date_time = pCall->due_date_time;
      pTimeCall->cl_nbr = pCall->nbr;
      strcpy( pTimeCall->drv_attr, pCall->drv_attr );
      strcpy( pTimeCall->veh_attr, pCall->veh_attr );
      if ( ( rc = db( TIME_CALL_FILE_ID, UPDATE_REC, &time_call_key1, 0, 0 ) ) < 0 )
	{
	  return( -1 );
	}
    }
  
  return ( 1 );
}

int
CheckTimeCallCriteria( pCall )
     struct cisam_cl  *pCall;
{
  /** Checks whether CALL meets the criteria for tracking **/
  /** as a time_call in the TIME_CALL table               **/

  if ( ( pCall->pri_status == 63 ) &&
       ( pCall->personal_rqst != ONLY_PERS_REQ ) &&
       ( ( pCall->grouped_with == 0 ) || ( pCall->grouped_with == pCall->nbr ) ) )
    return( 1 );
  else
    return( -1 );
}

int
DeleteTimeCall( CallNbr )
     int CallNbr;
{
  TIME_CALL_REC *pTimeCall;
  int  rc;

  pTimeCall = ( TIME_CALL_REC * ) tmp_req_buf;
  pTimeCall->cl_nbr = CallNbr;

  if ( ( rc = db( TIME_CALL_FILE_ID, READ_KEYS, &time_call_key1, ISEQUAL, 0 ) ) < 0 )
    return( -1 );

  if ( ( rc = db( TIME_CALL_FILE_ID, DELETE, &time_call_key1, 0, 0 ) ) < 0 )
    return( -1 );

  return( 1 );
}

    

int
VehConfirmProcess()
{

  struct cisam_cl chk_call_rec;
  struct cisam_vh chk_veh_rec;
  char temp_str[256];
  WINDOW     *WVehConfirm;
  int        c;
  fd_set     readmask;
  int        maxfdpl, nfound = -1;
  struct timeval  duration;
  int        stdinfd = fileno(stdin);
  int        done = 0;
  
  WVehConfirm = newwin( 7, 90, 8, 24 );
  VehConfirm_draw( WVehConfirm );
  mvwprintw( WVehConfirm, 2, 19, catgets(UI_m_catd, UI_1, UI_TEXT_73,"Waiting for taxi to accept call %s"), last_six(call_struct.nbr) );
  wrefresh( WVehConfirm );
  bzero( &chk_call_rec, sizeof( struct cisam_cl ) );
  chk_call_rec.nbr = call_struct.nbr;
  done = 0;
  while ( !done )
    {
      
      if ( db_read_key( CALL_FILE_ID, &chk_call_rec, &cl_key1, ISEQUAL ) == SUCCESS )
	{
	  if ( chk_call_rec.veh_nbr > 0 )
	    {
	      wattrset( WVehConfirm, A_BOLD );
	      sprintf( temp_str, catgets(UI_m_catd, UI_1, UI_TEXT_74, "Call Accepted by Vehicle %d             "), chk_call_rec.veh_nbr );
	      mvwprintw( WVehConfirm, 2, 19, temp_str );
	      /* get vehicle information if available */
	      chk_veh_rec.fleet = chk_call_rec.fleet;
	      chk_veh_rec.nbr = chk_call_rec.veh_nbr;
	      if ( db_read_key( VEHICLE_FILE_ID, &chk_veh_rec, &veh_key2, ISEQUAL ) == SUCCESS )
		{
		  sprintf( temp_str, "%s %s %s", chk_veh_rec.color,
			  chk_veh_rec.make, chk_veh_rec.model );
		  mvwprintw( WVehConfirm, 3, 19, temp_str );
		}
	      mvwprintw( WVehConfirm, 5, 19, catgets(UI_m_catd, UI_1, UI_TEXT_75,"Press <RETURN> to continue..               ") );
	      wrefresh( WVehConfirm );
	      while ( ( c = getkey() ) != '\n' );
	      VehConfirm_destroy( WVehConfirm );
	      done = 1;
	    }
	}
      if ( !done )
	{
	  /* check for a key pressed by user */
	  FD_ZERO( &readmask );
	  FD_SET( stdinfd, &readmask );
	  maxfdpl = stdinfd + 1;
	  duration.tv_sec = 3;
	  duration.tv_usec = 0;
	  nfound = select( maxfdpl, &readmask, (fd_set *) 0, (fd_set *) 0,
			  ( struct timeval * ) &duration );
	  if ( ( nfound == 1 ) && ( FD_ISSET( stdinfd, &readmask ) ) )
	    {
	      mvwprintw( WVehConfirm, 5, 19, catgets(UI_m_catd, UI_1, UI_TEXT_76,"Would you like to stop the search? (Y/N)") );
	      wrefresh( WVehConfirm );
	      c = Toupper(getkey());
	      if ( c == YES )
		{
		  done = 1;
		  VehConfirm_destroy( WVehConfirm );
		}
	      else if ( c == NO )
		{
		  mvwprintw( WVehConfirm, 5, 19, "                                          " );
		  wrefresh( WVehConfirm );
		}
	    }
	}
    }
  return(DONE);
}

int
VehConfirm_draw( WVehConfirm )
     WINDOW *WVehConfirm;
{
  wattrset( WVehConfirm, 0 );
  tpak_box( WVehConfirm, 0, 0 );
  wrefresh( WVehConfirm );
  mvwprintw( WVehConfirm, 0, 19, catgets(UI_m_catd, UI_1, UI_TEXT_77,"Call Accept Confirmation Window") );
  wattrset( WVehConfirm, 0 );

  wrefresh( WVehConfirm );

  return(DONE);
}

int
VehConfirm_destroy( WVehConfirm )
     WINDOW   *WVehConfirm;
{
  werase( WVehConfirm );
  touchwin( stdscr );

  return(DONE);
}


fill_cust_rec2()
{
  struct cisam_cust   *cust;
  int rc,i;
  int do_customer_update = TRUE;
  time_t   t;
  char key;
  char TelephoneNbr[32];

  if ( line_mgr_attached == 0 )
    return;

  if ( LineMgrIndex < 0 )
    return;
  
  if (Lines[LineMgrIndex]->NewCall != '1' )
    return;

  bzero(TelephoneNbr, 32);
  if (strlen(Lines[LineMgrIndex]->TelephoneNbr) <= 5)
    {
      Lines[LineMgrIndex]->NewCall = '0' ;
      strcpy(Lines[LineMgrIndex]->TelephoneNbr," ");
      return;
    }
  else
    strcpy(TelephoneNbr, Lines[LineMgrIndex]->TelephoneNbr);

  Lines[LineMgrIndex]->NewCall = '0' ;
  strcpy(Lines[LineMgrIndex]->TelephoneNbr," ");
  
  cust = (struct cisam_cust *)req_buf;
  bzero( cust, sizeof( struct cisam_cust ) );


  strcpy(cust->phone_nbr, TelephoneNbr);

  // no phone number to query
  if ( !strlen(cust->phone_nbr) )
    return;


  // non-numeric phone...
  for ( i = 0; i < strlen(cust->phone_nbr); i++ )
    {
      if ( !isdigit((int)cust->phone_nbr[i]) )
        return;
    }

  // Don't store a number with all '0'
  if ( atoi( cust->phone_nbr ) == 0 )
    return;

  
  t = time(0);
  
  if (db(CUSTOMER_FILE_ID, READ_KEYS, &cust_key1, ISEQUAL, 0) < 0)
    {
      /** New customer record to add **/
      strcpy(cust->pckup_str_name, call_struct.pckup_str_name);
      
      if ( ( !(0) ) || ( ui_phone_update ) )
        {
          strcpy(cust->passenger_name, call_struct.passenger_name);
          strcpy(cust->customer_comment, call_struct.general_cmnt);
          strcpy(cust->veh_attr, call_struct.veh_attr);
          strcpy(cust->drv_attr, call_struct.drv_attr);
        }
      
      strcpy(cust->pckup_city, call_struct.pckup_city);
      cust->pckup_str_nbr = call_struct.pckup_str_nbr;
      cust->pckup_zone = call_struct.pckup_zone;
      strcpy(cust->pckup_apt_room, call_struct.pckup_apt_room);
      cust->access_date_time = t;
      cust->made_calls = 1;
      cust->fleet = call_struct.fleet;
      if (db(CUSTOMER_FILE_ID, ADD_REC, &cust_key1, 0, 0) < 0)
	{
	  prt_error("Unable to Add New Customer Record");
	}
    }
  else
    {
      if ( do_customer_update == TRUE )
	{
	  strcpy(cust->pckup_str_name, call_struct.pckup_str_name);
          
          if ( ( !(0) ) || ( ui_phone_update ) )
            {
              strcpy(cust->passenger_name, call_struct.passenger_name);
              strcpy(cust->customer_comment, call_struct.general_cmnt);
              strcpy(cust->veh_attr, call_struct.veh_attr);
              strcpy(cust->drv_attr, call_struct.drv_attr);
            }
          
	  strcpy(cust->pckup_city,     call_struct.pckup_city);
	  strcpy(cust->pckup_apt_room, call_struct.pckup_apt_room);
	  cust->pckup_str_nbr =        call_struct.pckup_str_nbr;
	  cust->fleet =                call_struct.fleet;
	  cust->pckup_zone =           call_struct.pckup_zone;
          cust->made_calls++;
          cust->access_date_time = t;

              if (db(CUSTOMER_FILE_ID, UPDATE_REC, &cust_key1, 0, 0) < 0)
                {
                  prt_error("Unable to Update Customer Record");
                }

	}
      else
        {
          cust->made_calls++;
          cust->access_date_time = t;      
          if (db(CUSTOMER_FILE_ID, UPDATE_REC, &cust_key1, 0, 0) < 0)
            {
              prt_error("Unable to Update Customer Record");
            }
        }
    }
}  
  
