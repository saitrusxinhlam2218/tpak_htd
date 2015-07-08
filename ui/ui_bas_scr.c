static char sz__FILE__[]="@(#)ui_bas_scr.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  ui_bas_scr.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:50:02
* @(#)  Last delta: 12/2/94 at 18:32:14
* @(#)  SCCS File: /taxi/sccs/s.ui_bas_scr.c
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
ERROR: "Compile time constant SCANDINAVIA or NAMERICA undefined."
#endif
#endif

#include <cursesX.h>
#include <memory.h>
#include <sys/types.h>
#include <strings.h>
#include <signal.h>
#include <setjmp.h>

#include "taxipak.h"
#include "Object.h"
#include "List_public.h"
#include "screens.h"			/* defines various screen types */
#include "call_entry.h"			/* defines for screen fields */
#include "ui_def.h"
#include "scr_flds.h"
#include "mads_types.h"
#include "mad_ipc.h"
#include "mads_isam.h"
#include "ui_ipc.h"
#include "user.h"
#include "ui_error.h"
#include "df_maint.h"
#include "func_keys.h"
#include "language.h"
#include "ui_strdefs.h"
#include "enhance.h"
#include "switch_ext.h"
#include "ui_call_msg.h" 
#include "Call_msg_public.h"
#include "Call_msg_private.h"
#include "Call_msg_db.h"
#include "Call_lists_public.h"

/* declare functions used for checking fields entered */
extern int 	check_dummy(), check_fleet(), check_type(),check_st_no(), 
		check_pre_dir(),check_st_name(),check_st_type(),check_apt_no(),
		check_city(), check_zone(), check_passenger(), check_phone(),
		check_ph_ext(), check_date(), check_time(), check_comment(), 
		check_st_no(), check_st_name(), check_post_dir(), 
		check_personel1(), check_personel2(), check_car_drv_needs(), 
		check_attr(), check_car_no(), check_priority(), check_rate(), check_holdline(),
		check_local(), check_subs_time(), check_subs_time(), check_subs_time(), 
		check_subs_time(), check_subs_time(), check_subs_time(), check_subs_time(), 
		check_subs_begin_date(), check_subs_chg_time(), check_subs_chg_time(),
		check_subs_chg_time(), check_subs_chg_time(), check_subs_chg_time(), 
		check_subs_chg_time(),
		check_subs_chg_time(), check_subs_end_date(), check_charge(), check_coname1(),
		check_coname2(), check_coname3(), check_reqby(), check_pod1(), check_podname(),
  check_podtime(), check_see(), check_advise_arrival(), check_invoice(), check_via(),
  check_cash_account();

extern BOOLEAN bExplicitPremium;
#ifdef NAMERICA
extern int      check_send_dest_addr(), check_send_rate();
#endif


/* declare array of pointers to functions which check the field entered */
/* the functions check to see if the field entered is valid or not */
int (*b_check_field[])() = {
	check_dummy,check_fleet,check_type,
#ifdef SCANDINAVIA
	check_st_name, check_st_no, check_dummy, check_dummy, check_dummy,
	check_dummy, check_st_name, check_dummy,  check_dummy,
#else
	check_st_no, check_pre_dir, check_st_name, check_st_type, check_pre_dir,
	check_pre_dir, check_st_name, check_st_type,  check_pre_dir,
#endif
	check_apt_no, check_city, check_zone,

	check_passenger, check_passenger, check_phone, check_phone, check_phone, check_ph_ext,

	check_date, check_time, check_comment,
#ifdef SCANDINAVIA
	check_st_name, check_st_no, check_dummy, check_dummy, check_dummy,
#else
	check_st_no, check_pre_dir, check_st_name, check_st_type, check_pre_dir,
#endif
	check_apt_no, check_city, 
	check_zone, check_via, check_personel1, check_personel2, check_car_drv_needs,
	// check_car_drv_needs,
	check_car_no, check_priority, check_rate, check_dummy,
	check_advise_arrival, check_holdline, check_local,
#ifdef NAMERICA
	check_send_dest_addr, check_send_rate,
#endif
	check_subs_time, check_subs_time,
	check_subs_time, check_subs_time, check_subs_time, check_subs_time, check_subs_time, 
	check_subs_begin_date, check_subs_time, check_subs_time,
	check_subs_time, check_subs_time, check_subs_time, check_subs_time, check_subs_time, check_subs_end_date
};

/* declare pointers to functions for wakeup call entry screen's field checking */
int (*w_check_field[])() = {
	check_dummy, check_fleet, check_passenger, check_date, check_time, check_phone,
    	check_phone, check_phone, check_ph_ext, check_comment, check_subs_time, check_subs_time,
	check_subs_time, check_subs_time, check_subs_time, check_subs_time, check_subs_time, 
	check_subs_begin_date, check_subs_time, check_subs_time, check_subs_time, check_subs_time,
	check_subs_time, check_subs_time, check_subs_time, check_subs_end_date
};


/* declare an array of pointers to functions which check the field entered */
/* the functions check to see if the field entered is valid or not */
int (*e_check_field[])() = {
  check_dummy,check_fleet,check_type,
#ifdef SCANDINAVIA
  check_cash_account,
#endif
	check_charge, check_coname1, check_coname2, check_coname3,
#ifdef SCANDINAVIA
	check_st_name, check_st_no, check_dummy, check_dummy, check_dummy,
	check_dummy, check_st_name, check_dummy,  check_dummy,
#else
	check_st_no, check_pre_dir, check_st_name, check_st_type, check_pre_dir,
	check_pre_dir, check_st_name, check_st_type,  check_pre_dir,
#endif	
	check_apt_no, 
	check_city, check_zone, check_passenger, check_passenger, check_phone,
	check_phone, check_phone, check_ph_ext, check_date, check_time, check_comment,
	check_invoice, check_invoice, check_invoice, check_invoice,
	check_personel1, check_personel2, check_car_drv_needs,
  //	check_car_drv_needs,
	check_car_no,
#ifdef SCANDINAVIA
	check_st_name, check_st_no, check_dummy, check_dummy, check_dummy,
#else
	check_st_no, check_pre_dir, check_st_name, check_st_type, check_pre_dir, 
#endif	
	check_apt_no, check_city, check_zone, check_passenger, check_pod1, check_passenger, check_podtime,
	check_priority, check_rate, check_dummy, check_advise_arrival, check_holdline, check_local, 
	check_via,
#ifdef NAMERICA
	check_send_dest_addr, check_send_rate,
#endif
	check_subs_time, check_subs_time,
	check_subs_time, check_subs_time, check_subs_time, check_subs_time, check_subs_time, 
	check_subs_begin_date, check_subs_time, check_subs_time, check_subs_time, check_subs_time,
	check_subs_time, check_subs_time, check_subs_time, check_subs_end_date
};

int 	saveed_prev_piu;				/* piu value saved when prev scr was saved */
int 	toggle_flag;					/* set if <TOGGLE> key is entered */
extern int gkey_val;
int 	toggle_piu;					/* value of piu saved when toggle key was entered */
int 	prev_scr_flag;					/* flag set if called due to PRESSING <PREV_SCR> key */
int 	saved_prev_piu = 3;				/* value of piu saved when prev scr key was entered */
int 	end_basic_call = 0;				/* flag for ending loop of basic_call() */
int 	end_wakeup_call = 0;				/* flag for ending loop of wakeup_call() */
int 	end_extended_call = 0;				/* flag for ending loop of extended_call() */
int	num_dr_attr_s;					/* number of driver attributes currently set */
int	num_vh_attr_s;					/* number of vehicle attributes currently set */
BOOLEAN     bPhoneFieldVisited = FALSE;
extern BOOLEAN msg_box_active;
extern BOOLEAN question_for_user_active;
extern BOOLEAN bExplicitPremimum;

char     callnbr_sav[8];
struct cisam_cl call_struct;				/* structure used to read in calls from user */
struct cisam_cl prev_call_struct;			/* for saving record of prev scr */
struct call_ipc_rec call_fifo;				/* for writing record to FIFO */
struct call_ipc_rec prev_call_fifo;				/* for saving call_fifo struct of prev scr */

extern struct offsets *offset;
extern short 	clear_err_flag;				/* set if last line has an error msg printed */
extern short 	silent;					/* is read_field() silent ? */
extern int 	scr_name;				/* name of screen on display */
extern int 	user_index;				/* index into users struture in shared memory */
extern int 	zip1, zip2;
extern int 	def_fl_index;				/* index of the fleet the user is currently working with */
extern int      cur_fl_index;
extern struct keydesc	zn_key1, veh_key1;		/* for db() calls */
extern struct cisam_sc 	subs_struct;
extern struct users 	*uiuser[];			/* array of pointers to users structure in shared memory */
extern struct fleets 	*fleet[];			/* array of pointers to fleets structure in shared memory */
extern short been_touched;
extern char req_buf[];					/* for cisam disk i/o */
extern struct keydesc ac_key1;				/* key for reading zone/rate file */
extern MANUAL_FLAT_HIST ManualFlatInfo;
extern int ui_phone;
extern int ui_tlg;
extern char *Spaces;
extern short EightyCol;
extern int doing_update;				/* is the user on the update screen */
extern char gWakeupFleet;
extern char gWakeupPassenger[];
extern char gWakeupPhone[];
extern char gWakeupComment[];
extern time_t gIntWakeupDueTime;
extern char Telephone[];
extern char AccountCode[];
extern int pid;
extern int    EventFlag;
sigjmp_buf     jmpbuf;

/********************************************************************************/
/* basic_call: 	This function displays the basic call entry of the 		*/
/*		subscription call entry screen. It accepts input from 		*/
/*		the user and calls the appropriate functions to process 	*/
/*		the call entered.						*/
/********************************************************************************/
basic_call()
{
	int	i,j;					/* Counter variable */
 	int piu;					/* prompt in use */
	int field_entered = 0;				/* whether field entered or not */
	int key_val;					/* value of the key pressed by the user */
	char read_buf[READ_BUF_SIZE];			/* buffer for reading fields on screen */
	int inchar;
	int ret_ck_fld;					/* return value of check field routine */
	short max_piu;					/* max value of piu */
	struct scr_flds *ptr_scr_fld;			/* pointer to 1st element of array of piu structures */
	int should_clear = 0;				/* should read_field()  clear the field upon entry of 1st char */
	short subs_file_processed = 0;			/* are we processing the subscription file */
	int       sav_def_fl_index;
	char AttributeSet;
	int AttrCol = 0, AttrRow = 0;
	short MaxAttrs;
	CALL_MSG_HNDL cmsg_hndl;
	char tlg_buf[READ_BUF_SIZE];
	int   tlg_line_no;
	int       done;
	struct scr_flds  tlg_scr;
        void GetClientInfo();        

	num_dr_attr_s = 0;				/* number of driver attributes currently set */
	num_vh_attr_s = 0;				/* number of vehicle attributes currently set */
	bPhoneFieldVisited = FALSE;
        question_for_user_active = FALSE;
        msg_box_active = FALSE;
	
	sav_def_fl_index = def_fl_index;
	bExplicitPremium = FALSE;
	if ((ui_tlg) && (!toggle_flag))
	  {
	    if (scr_name == BASIC_CALL_SCR)
	      {
		clear();
	      start_over_basic:
		tlg_scr.prompt_row = 12;
		tlg_scr.prompt_col = 64;
		tlg_scr.max_chars = 4;
		move(1,0);
		for (i = 0; i < COLS - 1; i++)
		  addch(' ');
		wattrset(stdscr, A_REVERSE);
		mvprintw(1, GREETING_COL + 6, "Telephone Line Entry Screen (Basic)");
	    
		wattrset(stdscr, 0);
		mvaddstr(12, 40, "Telephone Line Nbr: ");

		wattrset(stdscr, A_UNDERLINE);
		wmove(stdscr, 12, 64);
		for (j = 0; j<5;j++)
		  waddch(stdscr, ' ');
		wattrset(stdscr, 0);
		wmove(stdscr, 12, 64);
		wrefresh(stdscr);
		i = 0;
		inchar = 0;
		field_entered = 0;
		should_clear = 0;
		inchar = read_field(stdscr, &tlg_scr, 0,tlg_buf, &field_entered, &should_clear);
		if (inchar == COMMAND_KEY)
		  {
		    ftnkey_handler(&piu,inchar,0);
		    goto escape_out_basic;
		  }
		for (i = 0; i <5; i++)
		  {
		    if (tlg_buf[i] == '\0')
		      break;
		    if (!isdigit(tlg_buf[i]))
		      {
			beep();
			prt_error(UI_INVALID_NUM, " - please enter numbers only");
			goto start_over_basic;
		      }
		  }
		
		tlg_line_no = atoi(tlg_buf);
		
		switch ( tlg_buf[0] )
		  {
		  case '1':
		  case '2':
		    cur_fl_index = 0;
		    def_fl_index = 0;
		    break;
		  case '3':
		    cur_fl_index = 1;
		    def_fl_index = 1;
		    break;
		  case '6':
		    cur_fl_index = 4;
		    def_fl_index = 4;
		    break;
		  case '9':
		    cur_fl_index = 7;
		    def_fl_index = 7;
		    break;
		  default:
		    beep();
		    prt_error(UI_INVALID_NUM, "- line nbr out of range");
		    goto start_over_basic;
		    break;
		  }

		if ((cur_fl_index < 0) || (cur_fl_index > 7))
		  {
		    cur_fl_index = 0;
		    def_fl_index = 0;
		  }
	      }
	  }

	doing_update = 0;
	disp_basic_scr();				/* display screen in scr_name; labels only, no data */

	ptr_scr_fld = basic_scr_flds;			/* initialize to point to 1st element of structure */

	if (scr_name == BASIC_CALL_SCR)			/* initialize value of max_piu to # of fields on the screen */
		max_piu = MAX_PIU_BASIC;		/* its a basic call entry screen */
	else
		max_piu = MAX_PIU_BAS_SUBS;		/* its a subscription basic screen */

	prt_entry_areas(stdscr,ptr_scr_fld,max_piu);	/* print entry ares underlinded */

	if(scr_name == SUBS_BASIC_SCR) {		/* initialize the subs call struct for reading next call */
		subs_file_processed = 1;
		if (!toggle_flag)
		    memset((char *)&subs_struct, '\0', sizeof(struct cisam_sc));
	}

	if(!toggle_flag)
		inters_flds(C_CLEAR,ptr_scr_fld);		/* clear the underlining from intersection fields */

	if((prev_scr_flag) && (!ui_tlg)) {
		prev_scr_flag = 0;			/* global flag; set by prev_screen() routine */
		call_struct = prev_call_struct;		/* copy saved fields to current structure */
		prt_bold_fields(&call_struct);		/* prints values as in call_struct */
		piu = saved_prev_piu;			/* restore saved prompt position */
	}
	else if(toggle_flag) {
		toggle_flag = 0;			/* unset flag */
		piu = toggle_piu;			/* get field to be read next(this needs to be mapped) */
		prt_bold_fields(&call_struct);		/* from call struct */
	}
	else {						/* called directly */
		memset((char *)&call_struct,'\0',sizeof(struct cisam_cl));		/* initialize call structure */
		memset((char *)&call_fifo,'\0',sizeof(struct call_ipc_rec));		/* initialize fifo structure */
		zip1 = zip2 = 0;							/* for rate determinantion */

		if(b_prt_default() == -1)						/* print default values */
			piu = 1; 							/* no fleet, cursor in fleet field */
		else									/* fleet specified is valid */
			piu =  3;							/* cursor in pickup street number fld */
		if (1)
		  if ((ui_phone) && (piu == 3))
                    {
                      piu = B_PH_AREA_CODE;
                      signal(SIGCONT, GetClientInfo);
                    }
	}

	end_basic_call = 0;								/* unset flag, so that we go in loop */
	attrset(0);
	attrset(A_REVERSE|A_UNDERLINE); 		/* reprint the greeting line */
	prt_date();
	attrset(A_REVERSE);
	mvaddstr(1, 0, Spaces );

	if (fleet[def_fl_index]->greeting_line == YES)

#ifdef CANBERRA_CABS
	    mvprintw(1,GREETING_COL, "%s, %s %s.",
		     fleet[find_findex(call_struct.fleet)]->fleet_name,
		     uiuser[user_index]->greeting_name, 
                     "speaking" );
#else
	    mvprintw(1,GREETING_COL, "%s  %s, %s %s. %s",
		     catgets(UI_m_catd, UI_1, UI_BAS_SCR_1, "Good Day!"),
		     fleet[find_findex(call_struct.fleet)]->fleet_name,
		     catgets(UI_m_catd, UI_1, UI_BAS_SCR_2, "this is"),
		     uiuser[user_index]->greeting_name, catgets(UI_m_catd, UI_1, UI_BAS_SCR_3, "May I help you?") );
#endif

	attrset(0);

        sigsetjmp( jmpbuf, 1);
        if ( EventFlag == TRUE )
          {
            EventFlag = FALSE;
            kill( pid, SIGCONT );
          }
        
	while(!end_basic_call && (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR)) {
		check_mail();								/* check the users mailbox */


		if(piu == B_DATE || piu == B_TIME || piu == B_BEGIN_DATE || piu == B_END_DATE)
			should_clear = 1;		/* check if read_field should clear field upon entry of 1st char in field */


                key_val = read_field(stdscr, ptr_scr_fld, piu,read_buf, &field_entered, &should_clear);


                if ( gkey_val != 0 )
                  {
                    toggle_piu = E_CHARGE;
                    strcpy( call_struct.charge_nbr, Telephone );
                    ftnkey_handler(&piu, gkey_val, 0 );
                    gkey_val = 0;
                    continue;
                  }
                
		if(key_val == -1)			/* read_field() timed out before field was entered */
			continue;			/* so go back to the same field */

		if(clear_err_flag)			/* if last line has an error msg printed on it */
			clear_error();			/* then clear it */
			
		/* Verify that field entered is correct */
		if ( key_val == ALTERNATE_KEY && piu == B_CAR_NEEDS )
		  {
                    alt_attr();

		    get_est_wait_time(1);

		  }

	     	else if (key_val != COMMAND_KEY && 
		  (ret_ck_fld = (*b_check_field[piu])(ptr_scr_fld, &piu, read_buf, key_val, &field_entered)) == DONE) {
			if (piu > max_piu)		/* field entered, piu incremented */
				piu = 1;		/* wrap around */
			else if (piu == 0)
				piu = max_piu;		/* wrap around to last field */
			if ((ui_tlg) && (key_val == SEND_KEY) && (strlen(callnbr_sav) > 0))
			  {
			    clear();
			    prt_error(UI_SENTFOR_DISPATCH," # %s ", callnbr_sav);  /* inform user of call # */
			    callnbr_sav[0] = '\0';
                            goto start_over_basic;
			  }			
		}
		else  					/* check_field returned NOT_DONE; call ftnkey_handler()*/
		  {
		    if ((ui_tlg) && (key_val == SEND_KEY) && (strlen(callnbr_sav) > 0))
		      {
			ftnkey_handler(&piu,key_val,0);
			clear();
			prt_error(UI_SENTFOR_DISPATCH," # %s ", callnbr_sav);  /* inform user of call # */
			callnbr_sav[0] = '\0';
			goto start_over_basic;
		      }
		    else 
		      {
			ftnkey_handler(&piu, key_val, 0);
			if ((ui_tlg) && (strlen(callnbr_sav) > 0) && (key_val != SEND_DUP_KEY))
			  {
			    clear();
			    prt_error(UI_SENTFOR_DISPATCH," # %s ", callnbr_sav);  /* inform user of call # */
			    callnbr_sav[0] = '\0';
			    goto start_over_basic;
			  }
		      }
		  }			
	}
      escape_out_basic:
	if (subs_file_processed)
		close_subs_file();			/* if subscription screen, then close the subscription file */
	def_fl_index = sav_def_fl_index;
	cur_fl_index = def_fl_index;
}

/********************************************************************************/
/* wakeup_call:	This function displays the wakeup call entry of the 		*/
/*		subscription call entry screen. It accepts input from 		*/
/*		the user and calls the appropriate functions to process 	*/
/*		the call entered.						*/
/********************************************************************************/
wakeup_call()
{
	int piu;					/* prompt in use */
	int field_entered = 0;				/* whether field entered or not */
	int key_val;					/* value of the key pressed by the user */
	char read_buf[READ_BUF_SIZE];			/* buffer for reading fields on screen */
	int ret_ck_fld;					/* return value of check field routine */
	short max_piu;					/* max value of piu */
	struct scr_flds *ptr_scr_fld;			/* pointer to 1st element of array of piu structures */
	int should_clear = 0;				/* should read_field()  clear the field upon entry of 1st char */
	short subs_file_processed = 0;			/* are we processing the subscription file */
        int zone_pkup_lead_time = 0;

	disp_wakeup_scr();				/* display screen in scr_name; labels only, no data */

	ptr_scr_fld = wakeup_scr_fld;			/* initialize to point to 1st element of structure */

	if (scr_name == WAKEUP_CALL_SCR)		/* initialize value of max_piu to # of fields on the screen */
		max_piu = MAX_PIU_WAKEUP;		/* its a basic call entry screen */
	else
		max_piu = MAX_PIU_WAK_SUBS;		/* its a subscription basic screen */

	prt_entry_areas(stdscr,ptr_scr_fld,max_piu);	/* print entry ares underlinded */

	if (scr_name == SUBS_WAKEUP_SCR) {		/* initialize the subs call struct for reading next call */
		subs_file_processed = 1;
		if (!toggle_flag)
		    memset((char *)&subs_struct,'\0',sizeof(struct cisam_sc));
	}

	if (prev_scr_flag) {
		prev_scr_flag = 0;			/* global flag; set by prev_screen() routine */
		call_struct = prev_call_struct;		/* restore saved fields */
		prt_wold_fields(&call_struct);		/* prints values as in call_struct */
		piu = saved_prev_piu;			/* saved by menu_manager() */
	}
	else {						/* called directly */

		memset((char *)&call_struct,'\0',sizeof(struct cisam_cl));	/* initialize call struct for reading next call */
		memset((char *)&call_fifo,'\0',sizeof(struct call_ipc_rec));	/* initialize dispatch fifo structure */
		zip1 = zip2 = 0;

                if ( gIntWakeupDueTime > 0 )
                  {
                    w_prt_default();
                    call_struct.due_date_time = get_new_lead( call_struct.due_date_time, call_struct.due_date_time,
                                                              0, offset );
                    strcpy( call_struct.passenger_name, gWakeupPassenger );
                    strcpy( call_struct.phone_number, gWakeupPhone );
                    get_srch_date( gIntWakeupDueTime - 1800, call_struct.due_date );
                    get_srch_time( gIntWakeupDueTime - 1800, call_struct.due_time );
                    call_struct.due_date_time = gIntWakeupDueTime;
                    call_struct.fleet = gWakeupFleet;
                    strcpy( call_struct.general_cmnt, gWakeupComment );
                    strncpy(call_struct.pass_name_key,call_struct.passenger_name,4);
                    prt_wold_fields(&call_struct);
                    gIntWakeupDueTime = 0;
                    gWakeupFleet = 'A';
                    gWakeupPassenger[0] = '\0';
                    gWakeupComment[0] = '\0';
                    if ( strlen( call_struct.phone_number ) )
                      piu = W_COMMENT;
                    else
                      piu = W_PH_AREA_CODE;
                  }
                else
                  {
                    if (w_prt_default() == -1)					/* print default values */
                      piu = 1;						/* set piu to read the fleet field */
                    else								/* fleet specified is valid */
                      piu =  2;						/* to be entered = passenger's name */
                  }
                
	}

	end_wakeup_call = 0;							/* unset flag, so that we go in loop */
	attrset(A_REVERSE|A_UNDERLINE);
	prt_date();
	attrset(0);
	while (!end_wakeup_call && (scr_name==WAKEUP_CALL_SCR || scr_name ==SUBS_WAKEUP_SCR)) {
		check_mail();							/* check the users mailbox */

		refresh();

		if (piu == W_DATE || piu == W_TIME)
			should_clear = 1;					/* clear field before entry */

	      	key_val = read_field(stdscr,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);
		if (key_val == -1)						/* read_field timed out before field was entered */
			continue;						/* so go back to the same field */

		if (clear_err_flag)						/* if last line has an error msg printed on it */
			clear_error();						/* then clear it */
			
										/* Verify that the field entered is correct */
	     	if (key_val != COMMAND_KEY && (ret_ck_fld = (*w_check_field[piu])(ptr_scr_fld,&piu, read_buf,key_val,&field_entered)) == DONE) {
			if (piu > max_piu)
				piu = 1;					/* wrap around */
			else if (piu == 0)
				piu = max_piu;					/* wrap around to last field */
		}
		else								/* check_field ret NOT_DONE;call ftnkey_handler()*/
			ftnkey_handler(&piu,key_val,0);
	}

	if (subs_file_processed)				/* if subscription screen, then close the subscription file */
		close_subs_file();
}

/****************************************************************************************/
/* extended_call:This function displays the extended call entry of the 			*/
/*		subscription call entry screen. It accepts input from 			*/
/*		the user and calls the appropriate functions to process 		*/
/*		the call entered.							*/
/****************************************************************************************/
extended_call()
{
        int i,j;
	int piu;						/* prompt in use */
	int field_entered = 0;					/* whether field entered or not */
	int key_val;						/* value of the key pressed by the user */
	int inchar;
	char read_buf[READ_BUF_SIZE];				/* buffer for reading fields on screen */
	int ret_ck_fld;						/* return value of check field routine */
	short max_piu;						/* max value of piu */
	struct scr_flds *ptr_scr_fld;				/* pointer to 1st element of array of piu structures */
	int should_clear = 0;					/* should read_field()  clear the field upon entry of 1st char */
	short subs_file_processed = 0;				/* are we processing the subscription file */
	int         sav_def_fl_index;
	int         sav_cur_fl_index;
	char AttributeSet;
	int AttrCol = 0, AttrRow = 0;
	short MaxAttrs;
	char tlg_buf[READ_BUF_SIZE];
	int   tlg_line_no;
	int       done;
	struct scr_flds  tlg_scr;	

	sav_def_fl_index = def_fl_index;
	num_dr_attr_s = 0;				/* number of driver attributes currently set */
	num_vh_attr_s = 0;				/* number of vehicle attributes currently set */
	bPhoneFieldVisited = FALSE;        
	bPhoneFieldVisited = FALSE;
        question_for_user_active = FALSE;
        msg_box_active = FALSE;
        
	if ((ui_tlg) && (!toggle_flag))
	  {
	    tlg_scr.prompt_row = 12;
	    tlg_scr.prompt_col = 64;
	    tlg_scr.max_chars = 4;

	    if (scr_name == EXTENDED_CALL_SCR)
	      {
		clear();
   start_over_extended:	    
		move(1,0);
		for (i = 0; i < COLS - 1; i++)
		  addch(' ');
		wattrset(stdscr, A_REVERSE);
		mvprintw(1, GREETING_COL + 6, "Telephone Line Entry Screen (Extended)");
		
		wattrset(stdscr, 0);
		mvaddstr(12, 40, "Telephone Line Nbr: ");

		wattrset(stdscr, A_UNDERLINE);
		wmove(stdscr, 12, 64);
		for (j = 0; j<5;j++)
		  waddch(stdscr, ' ');
		wattrset(stdscr, 0);
		wmove(stdscr, 12, 64);
		wrefresh(stdscr);
		i = 0;
		inchar = 0;
		field_entered = 0;
		should_clear = 0;
		inchar = read_field(stdscr, &tlg_scr, 0,tlg_buf, &field_entered, &should_clear);
		if (inchar == COMMAND_KEY)
		  {
		    ftnkey_handler(&piu,inchar,0);
		    goto escape_out_extended;
		  }
		for (i = 0; i <5; i++)
		  {
		    if (tlg_buf[i] == '\0')
		      break;
		    if (!isdigit(tlg_buf[i]))
		      {
			beep();
			prt_error(UI_INVALID_NUM, " - please enter numbers only");
			goto start_over_extended;
		      }
		  }
		
		switch ( tlg_buf[0] )
		  {
		  case '1':
		  case '2':
		    cur_fl_index = 0;
		    def_fl_index = 0;
		    break;
		  case '3':
		    cur_fl_index = 1;
		    def_fl_index = 1;
		    break;
		  case '6':
		    cur_fl_index = 4;
		    def_fl_index = 4;
		    break;
		  case '9':
		    cur_fl_index = 7;
		    def_fl_index = 7;
		    break;
		  default:
		    beep();
		    prt_error(UI_INVALID_NUM, "- line nbr out of range");
		    goto start_over_extended;
		    break;
		  }		
		
		if ((cur_fl_index < 0) || (cur_fl_index > 7))
		  {
		    cur_fl_index = 0;
		    def_fl_index = 0;
		  }
	      }
	  }

	doing_update = 0;
	disp_extended_scr(0);                                    /* display screen in scr_name, labels only, no data */
								 /* ENH0002: 0 means that the 'number of call messages'
									     field should not be displayed */

	ptr_scr_fld = extended_scr_fld;				/* initialize to point to 1st element of structure */

	if (scr_name == EXTENDED_CALL_SCR)			/* initialize value of max_piu to # of fields on the screen */
		max_piu = MAX_PIU_EXTENDED;			/* its a basic call entry screen */
	else
		max_piu = MAX_PIU_EXT_SUBS;			/* its a subscription basic screen */

	prt_entry_areas(stdscr,ptr_scr_fld,max_piu);		/* print entry ares underlinded */

	if (scr_name == SUBS_EXTENDED_SCR) {			/* initialize the subs call struct for reading next call */
		subs_file_processed = 1;
		if (!toggle_flag)
		    memset((char *)&subs_struct,'\0',sizeof(struct cisam_sc));
	}

	if (!toggle_flag)
		inters_flds(C_CLEAR,ptr_scr_fld);			/* clear the underlining from intersection fields */
	else {
		if(call_struct.pckup_str_name[0] == '\0')
			been_touched = FALSE;
		else
			been_touched = TRUE;
		if(call_struct.charge_nbr[0] != '\0')
			redsp_charge_nbr(&call_struct);
	}

	/* Default to NO POD required if not YES */
	if (call_struct.pod_required != YES)
	  call_struct.pod_required = NO;

	if (prev_scr_flag) {
		prev_scr_flag = 0;				/* global flag; set by prev_screen() routine */
		call_struct = prev_call_struct;			/* restore saved data */
		prt_eold_fields(&call_struct);			/* prints values as in call_struct */
		piu = E_CHARGE;					/* always go to charge field */
	}
	else if (toggle_flag) {
		toggle_flag = 0;				/* unset flag */
		piu = E_CHARGE;					/* always go to charge field */
		prt_eold_fields(&call_struct);			/* from call struct */
	}
	else {							/* called directly */

		memset((char *)&call_struct,'\0',sizeof(struct cisam_cl));	/* initialize call struct for reading next call */
		memset((char *)&call_fifo,'\0',sizeof(struct call_ipc_rec));	/* initialize fifo structure */
		zip1 = zip2 = 0;

		if (e_prt_default() == -1)					/* print default values */
			piu = 1; 						/* no fleet, cursor in fleet field */
		else								/* fleet specified is valid */
		    piu = E_CHARGE;					/* always go to charge field */
	}

	end_extended_call = 0;							/* unset flag, so that we go in loop */
	attrset(A_REVERSE|A_UNDERLINE);
	prt_date();
	attrset(0);

        sigsetjmp( jmpbuf, 1);
        if ( EventFlag == TRUE )
          {
            EventFlag = FALSE;
            kill( pid, SIGCONT );
          }
        
	while (!end_extended_call && (scr_name==EXTENDED_CALL_SCR || scr_name ==SUBS_EXTENDED_SCR)) {
		check_mail();							/* check the users mailbox */
		ManualFlatInfo.ManualFlatPerformed = 0;

		if (piu == E_DATE || piu == E_TIME || piu == E_POD_TIME)
			should_clear = 1;		/* check if read_field() should clear field upon entry of 1st char in fld */
		if (piu == E_RATE)
		  should_clear = 1;

                key_val = read_field(stdscr,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);

                if ( gkey_val != 0 )  // automatic toggle based on line_mgr
                  {
                    toggle_piu = B_PH_AREA_CODE;
                    strcpy( call_struct.phone_number, Telephone );
                    ftnkey_handler(&piu, gkey_val, 0 );
                    gkey_val = 0;
                    continue;
                  }                
                
		if (key_val == -1)				/* read_field() timed out before field was entered */
                  continue;				/* so go back to the same field */


		if (clear_err_flag) 
			clear_error();

		if ( key_val == ALTERNATE_KEY && piu == E_CAR_NEEDS )
		  {
                    alt_attr();

		    get_est_wait_time(1);
		  }

	     	else if (key_val != COMMAND_KEY &&
			 (ret_ck_fld = (*e_check_field[piu])(ptr_scr_fld,&piu, read_buf,key_val,&field_entered)) == DONE)
		  {
		  if (piu > max_piu)
				piu = 1;			/* wrap around */
			else if (piu == 0)
				piu = max_piu;			/* wrap around to last field */
		}
		else						/* check_field returned NOT_DONE;call ftnkey_handler()*/
		{
			if(call_struct.extended_type[0] != DELIV && call_struct.extended_type[0] != PASS && 
				call_struct.extended_type[0] != TYPEX && key_val != CANCEL_KEY && key_val != COMMAND_KEY)
			{
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_BAS_SCR_4, "must enter call type"));
			}
			else
			{
				ftnkey_handler(&piu,key_val,0);
			}
		}
	}
      escape_out_extended:
	if (subs_file_processed)				/* if subscription screen, then close the subscription file */
		close_subs_file();
	def_fl_index = sav_def_fl_index;
}
redsp_charge_nbr(cp)
struct cisam_cl *cp;				/* pointer to call structure with info */
{
	struct cisam_ac *ac_ptr;			/* pointer to req_buf[] */
	ac_ptr = (struct cisam_ac *)req_buf;

	if (db(ACCOUNT_FILE_ID,OPEN,&ac_key1,ISINOUT+ISMANULOCK, 0) < 0) {		/* open the account file on disk */
		return;
	}

	ac_ptr->fleet = cp->fleet;							/* build key for reading account file */
	strcpy(ac_ptr->nbr,cp->charge_nbr);

	if(db(ACCOUNT_FILE_ID,READ_KEYS,&ac_key1,ISEQUAL,0) >= 0)	/* try to read the record from the account file */
	{
		mvprintw(E_CHARGE_ROW+1,3,"%-30s  %-15s",ac_ptr->header_1,ac_ptr->header_3);
		mvprintw(E_CHARGE_ROW+2,3,"%-30s  %-15s",ac_ptr->header_2,ac_ptr->header_4);
	}

	if (db(ACCOUNT_FILE_ID,CLOSE,&ac_key1,ISEQUAL,0) < 0) {			/* close the account file */
		prt_error(UI_CLOSE,catgets(UI_m_catd, UI_1, UI_BAS_SCR_5, "<account file>"));				/* db() failed to close file */
	}

}

