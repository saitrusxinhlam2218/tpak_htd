static char sz__FILE__[]="@(#)ui_restrict_time_calls.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  ui_restrict_time_calls.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:51:33
* @(#)  Last delta: 12/2/94 at 18:12:04
* @(#)  SCCS File: /taxi/sccs/s.ui_restrict_time_calls.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

/*-----------------------------------------------------------------
 *	ui restrict time calls.c
 *-----------------------------------------------------------------
 */

#include <cursesX.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>

#include "taxipak.h"
#include "language.h"
#include "mem_mgmt.h"
#include "Object.h"
#include "mads_types.h"
#include "ui_strdefs.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "screens.h"
#include "call_entry.h"
#include "df_maint.h"
#include "ui_def.h"
#include "ui_error.h"
#include "func_keys.h"
#include "path.h"
#include "user.h"
#include "ui_restrict_time_calls.h"
#include "ui_ipc.h"
#include "mad_error.h"

/*
 *	externs
 */
extern char 			req_buf[];
extern char                     tmp_req_buf[];
extern struct keydesc 	zn_key1, 
						fl_key1,
						cl_key4,
                                                time_call_key2;
extern int 				doing_update;
extern int 				scr_name;
extern int 				No_status_line;
extern struct users 	*uiuser[];
extern int 				user_index;
/*
 *	globals
 */
WINDOW					*alt_times_win,
						*newwin();
struct scr_flds alt_time_field[] = 
				  {
					{UI_TCR_FIELD_ROW, UI_TCR_FIELD_COL, 
					UI_TCR_FIELD_CHARS, UI_TCR_FIELD_PIU},

					{UI_TCR_FIELD_ROW, UI_TCR_FIELD_COL, 
					UI_TCR_FIELD_CHARS, UI_TCR_FIELD_PIU}
				  };

extern int msgkey;						/* qid for ipc */
extern int pid;							/* process id */





/*-----------------------------------------------------------------
 *==>	tcr check time call restriction
 *-----------------------------------------------------------------
 */

tcr_check_time_call_restriction(call, called_from_send, key_val)
	struct cisam_cl		*call;
	int				 called_from_send;
	int					 key_val;
{
	int				 max_time_calls_for_zone;
	struct cisam_fl		 fleet;
	int          num_time_calls = 0;
	HNDL    hTCResp = HNDL_NULL;
	TC_RESP_REC  *tc_rp;
	struct tc_rqst tc_rq;
	int  rc, times_thru;
	char error_str[80];	

/*
 *	when the send key is pressed and the cursor is on the time or
 *		date or zone field, the validation for that field is executed
 *		and this function will get called twice because in the send()
 *		code a final validation is necessary.  The following check
 *		will ensure that this does not execute twice when send key is 
 *		pushed.
 */

	if ((key_val != '\n' && key_val != '\r' && key_val !='\t' &&
		 key_val != KEY_UP && key_val != KEY_DOWN) && 
		!called_from_send)
		return(TRUE);

/*
 *	if not enough information on call or 
 *		restricted time call disabled --> return FALSE
 */
	if (!call || call->due_date_time <= 0 )
	  {
		tcr_print_message(catgets(UI_m_catd, UI_1, UI_RESTRICT_TC_6, "Error:  Not enough call information"));
		return(FALSE);
	  }

	if ( call->extended_type[2] != TIMEC ) 
	    return(TRUE);
	
	if (!tcr_get_fleet(call, &fleet))
	  {
		tcr_print_message(catgets(UI_m_catd, UI_1, UI_RESTRICT_TC_7, "Time Calls must have a fleet"));
		return(FALSE);
	  }

	if (!fleet.restricted_time_call_enabled || 
		!fleet.time_call_time_period_len)
	  {
/*
 *		time call restriction not enabled
 */
		return(TRUE);
	  }

	if (!tcr_get_call_zone(call))
	  {
/*
 *		no such thing as an unzoned restricted time call
 */
		tcr_print_message(catgets(UI_m_catd, UI_1, UI_RESTRICT_TC_8, "Time Calls must be zoned"));
		return(FALSE);
	  }


/*
 *	if no max time calls for the zone exists --> return TRUE
 */
	if ((max_time_calls_for_zone = 
	     (int)tcr_get_zone_max_time_calls((int)call->pckup_zone, (char)call->fleet)) == 0)
		return(TRUE);
	else if ( max_time_calls_for_zone == -1 )   // no time calls allowed for this zone
	    {
		tcr_print_message(catgets(UI_m_catd, UI_1, UI_RESTRICT_TC_11,"Time Calls not authorized for this zone"));
		return(FALSE);
	    }

/*
 *      check how many time calls currently set for this time
 */
	tc_rq.d_prcs_id = DISP_DEST;
	tc_rq.u_prcs_id = pid;
	tc_rq.rec_type = TC_REQ;
	tc_rq.fleet = call->fleet;
	tc_rq.zone_nbr = call->pckup_zone;
	tc_rq.due_date_time = call->due_date_time;

	msgsnd( msgkey, (struct msgbuf *)&tc_rq, sizeof( struct tc_rqst ), IPC_NOWAIT );
	hTCResp = TMALLOC( TC_RESP_REC, 1 );
	tc_rp = ( TC_RESP_REC * )hTCResp;
	
	times_thru = 0;
	while ( times_thru <= 2 )
	  {
	    rc = msgrcv( msgkey, (void *)tc_rp, sizeof(TC_RESP_REC), (long)pid, 0 );
	    if ( rc == -1 )
	      {
		if ( errno != EINTR )
		  {
		    prt_error(UI_FATAL, "%s", 
			      catgets(UI_m_catd, UI_1, UI_UI_SSTAT_74, 
				      "Error reading from dispatch in msgrcv"));
		    sprintf(error_str,
			    "Error %d reading ipc message from DISPATCH", errno);
		    ERROR(' ',"User-IPC", error_str);
		    TFREE( hTCResp );
		    return(0);
		  }
		if (++times_thru == 2) 
		  {
		    prt_error(UI_SYSTEM_DOWN, "");
		    ERROR(' ',"User-IPC", 
			  "Timeout reading ipc message from DISPATCH");
		    TFREE( hTCResp );
		    return(0);
		  }
	      }
	    else 
	      if (tc_rp->rec_type == TC_REQ)
		break;
	    else
	      {
		  prt_error(UI_FATAL, "%s", 
			    catgets(UI_m_catd, UI_1, UI_UI_SSTAT_74, 
				    "Error reading from dispatch in msgrcv"));
		  ERROR(' ',"User-IPC", "Invalid msg from dispatch DISPATCH");
		  TFREE( hTCResp );
		  return(0);
	    	}
	  }
/*
 *	set valid future time with restrictions
 */
	if (!tcr_set_valid_future_time(call, tc_rp->time_slots, max_time_calls_for_zone))
	  {
	        TFREE( hTCResp );
		return(FALSE);
	  }

	TFREE( hTCResp );
	return(TRUE);
}


/*-----------------------------------------------------------------
 *==>	tcr get fleet
 *-----------------------------------------------------------------
 */

tcr_get_fleet(call, fleet)
	struct cisam_cl		*call;
	struct cisam_fl		*fleet;
{
	struct cisam_fl		*db_fleet;
	

	if (!call)
		return(FALSE);

	if (call->fleet == ' ')
		return(FALSE);

	db_fleet = (struct cisam_fl *)req_buf;

	/* Open fleet file if it is not open and leave it open */
	if (Db_files[FLEET_FILE_ID].fd == -1)
	    if (db(FLEET_FILE_ID, OPEN, &fl_key1, ISINOUT | ISMANULOCK, 0) < 0)
		return(FALSE);

	db_fleet->id = call->fleet;	

	if (db(FLEET_FILE_ID, READ_KEYS, &fl_key1, ISEQUAL, 0) < 0)
	    return(FALSE);

	memcpy(fleet, db_fleet, sizeof(struct cisam_fl));

	return(TRUE);
}


/*-----------------------------------------------------------------
 *==>	tcr get call zone
 *-----------------------------------------------------------------
 */

tcr_get_call_zone(call)
	struct cisam_cl		*call;
{
	
	if (!call)
		return(FALSE);

	return((int)call->pckup_zone);
}

/*-----------------------------------------------------------------
 *==>	tcr get zone max time calls
 *-----------------------------------------------------------------
 */

tcr_get_zone_max_time_calls(zone_number, fleet)
	int					zone_number;
	char				fleet;
{
	struct cisam_zn		*zone;
	int				 max_time_calls;

#ifdef FOO	
	if (zone_number <= 0 || fleet == ' ')
		return(FALSE);
#endif

	zone = (struct cisam_zn *)req_buf;

	zone->fleet = fleet;
	zone->nbr = (short)zone_number;

	if (db(ZONE_FILE_ID, READ_KEYS, &zn_key1, ISEQUAL, 0) < 0)
	  {
		return(FALSE);
	  }

	max_time_calls = (int)zone->max_time_calls;

#ifdef FOO
	if (max_time_calls == UI_TCR_MAX_TIME_CALLS_NO_ENTRY &&
		zone->nbr != zone->superzn_nbr)
	  {
		zone->nbr = zone->superzn_nbr;
		if (db(ZONE_FILE_ID, READ_KEYS, &zn_key1, ISEQUAL, 0) < 0)
		  {
			return(FALSE);
		  }

		max_time_calls = (int)zone->max_time_calls;
	  }
#endif

	return((int)max_time_calls);
}



/*-----------------------------------------------------------------
 *==>	tcr set valid future time
 *-----------------------------------------------------------------
 */

tcr_set_valid_future_time(call, time_slots, max_time_calls)
        struct cisam_cl                  *call;
        TIMES                            *time_slots;
	short				 max_time_calls;
{
  time_t                                 current_time;
  short				 have_alternates = FALSE,
						 new_time_index,
						 call_zone, i;



/*
 *	check for requested time slot
 */
        current_time = time(0);
	if (time_slots->count < max_time_calls)
		return(TRUE);

/*
 *	check for any alternates
 */
	for (i = 1; i <= UI_TCR_MAX_TIME_SLOTS; i++)
	  {
		if ((time_slots + i)->count < max_time_calls &&
			(time_slots + i)->date_time > current_time)
			have_alternates = TRUE;
		else
			(time_slots + i)->count = UI_TCR_SLOT_NOT_AVAILABLE;
	  }

	if (!have_alternates)
	  {
		tcr_print_message("%s", catgets(UI_m_catd, UI_1, UI_RESTRICT_TC_9, 
			"There are no alternate times available within 6 time slots.  Please choose another date/time."));
		return(FALSE);
	  }

	new_time_index = (short)tcr_get_alternate_time(time_slots, max_time_calls);

	if (!new_time_index)
	  {
		tcr_print_message("%s", catgets(UI_m_catd, UI_1, UI_RESTRICT_TC_10, 
		"Requested time slot is unavailable.  Please change before sending call"));
		return(FALSE);
	  }

	current_time = get_time();
	if ((time_slots + new_time_index)->date_time <= current_time)
	  {
		prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CHECK_FLD_42, 
				"<date/time entered has past>"));
		sleep(1);
		tcr_print_message("%s", catgets(UI_m_catd, UI_1, UI_RESTRICT_TC_10, 
		"Requested time slot is unavailable.  Please change before sending call"));
		return(FALSE);
	  }
/*
 *	set new date/time
 */
	strcpy(call->due_date, (time_slots + new_time_index)->date_string);
	strcpy(call->due_time, (time_slots + new_time_index)->time_string);
	call->due_date_time = get_internal_time(call->due_date, call->due_time);

	if (doing_update || scr_name == EXTENDED_CALL_SCR)
	  {
		wattrset(stdscr,A_UNDERLINE);
    	mvaddstr(E_PKUP_DATE_ROW, E_PKUP_DATE_COL, call->due_date);
		mvaddstr(E_PKUP_TIME_ROW, E_PKUP_TIME_COL, call->due_time);
		wattrset(stdscr,0);
	  }

	if (scr_name == BASIC_CALL_SCR)
	  {
		wattrset(stdscr,A_UNDERLINE);
    	mvaddstr(B_PKUP_DATE_ROW, B_PKUP_DATE_COL, call->due_date);
		mvaddstr(B_PKUP_DATE_ROW, B_PKUP_TIME_COL, call->due_time);
		wattrset(stdscr,0);
	  }

	return(TRUE);
}


/*-----------------------------------------------------------------
 *==>	tcr set time slot totals
 *
 *		slot 0 --> requested time
 *		slots 1 - 6 --> alternate times (3 before, 3 after)
 *-----------------------------------------------------------------
 */

tcr_set_time_slot_totals(time_slots, time_block, current_due_date_time, 
			time_call_due_date_time)
	TIMES				 time_slots[];
	short				 time_block;
	long				 current_due_date_time,
						 time_call_due_date_time;
{
	long				 current_time_block,
						 time_call_time_block;
	short				 index;


	current_time_block = current_due_date_time / time_block;
	time_call_time_block = time_call_due_date_time / time_block;
	
	if (abs(current_time_block - time_call_time_block) > 
			(UI_TCR_MAX_TIME_SLOTS / 2))
		return(TRUE);

	if (time_call_time_block == current_time_block)
	  {
		time_slots[UI_TCR_REQUESTED_SLOT].count++;
		return(TRUE);
	  }

	index = (time_call_time_block - current_time_block) + 
			(UI_TCR_MAX_TIME_SLOTS / 2);

	if (index < (UI_TCR_MAX_TIME_SLOTS / 2))
		index++;

	time_slots[index].count++;

	return(TRUE);
}


/*-----------------------------------------------------------------
 *==>	tcr set time slot strings
 *
 *		time_slots array is filled in with time strings
 *		time is converted to time_blocks so that the intervals will
 *			be one "time_block"
 *		function get_srch_time() is used to get the time string
 *			time must be converted from "time_block" to actual_C_time
 *			because get_srch_time() needs (long)seconds as input
 *-----------------------------------------------------------------
 */

tcr_set_time_slot_strings(time_slots, time_block_seconds, current_due_date_time)
	TIMES				 time_slots[];
	short				 time_block_seconds;
	long				 current_due_date_time;
{
	long				 current_time_block,
						 actual_C_time,
						 time;
	int					 i;


	current_time_block = (long)(current_due_date_time / time_block_seconds);
	
	time = current_time_block - (UI_TCR_MAX_TIME_SLOTS / 2);
	
	for (i = 1; i <= UI_TCR_MAX_TIME_SLOTS; i++)
	  {
		if (time == current_time_block)
		  {
			actual_C_time = (long)(time * time_block_seconds);
			time_slots[UI_TCR_REQUESTED_SLOT].date_time = actual_C_time;
			get_srch_time(actual_C_time, 
					time_slots[UI_TCR_REQUESTED_SLOT].time_string);
			get_srch_date(actual_C_time, 
					time_slots[UI_TCR_REQUESTED_SLOT].date_string);
			time++;
		  }

		actual_C_time = (long)(time * time_block_seconds);
		time_slots[i].date_time = actual_C_time;
		get_srch_time(actual_C_time, time_slots[i].time_string);
		get_srch_date(actual_C_time, time_slots[i].date_string);

		time++;
	  }

	return(TRUE);
}


/*-----------------------------------------------------------------
 *==>	tcr get alternate time
 *
 *		fill in window and display
 *		get user response
 *-----------------------------------------------------------------
 */

tcr_get_alternate_time(time_slots, max_number)
	TIMES				 *time_slots;
	short				 max_number;
{
	char				 string[80],
						 reponse_buffer[READ_BUF_SIZE];
	int					 count = 1,
						 i,
						 field_entered = 0,
						 clear_field = 0,
						 choice,
						 key_stroke;
	short				 alternate_selection = FALSE;


	tcr_start_alt_win();
	werase(alt_times_win);
/*
 *	fill in window strings
 */
	sprintf(string, "%s", catgets(UI_m_catd, UI_1, UI_RESTRICT_TC_1, "Requested pick up time not available.  Please select an alternate:"));
	mvwaddstr(alt_times_win, 0, 5, string);
	sprintf(string, "    %s   %s", 
		catgets(UI_m_catd, UI_1, UI_RESTRICT_TC_2, "Time"), 
		catgets(UI_m_catd, UI_1, UI_RESTRICT_TC_3, "Available"));
	mvwaddstr(alt_times_win, 1, UI_TCR_WIN_COL_1, string);
	mvwaddstr(alt_times_win, 1, UI_TCR_WIN_COL_2, string);

	for (i = 1; i <= UI_TCR_MAX_TIME_SLOTS; i++)
	  {
		if ((time_slots + i)->count != UI_TCR_SLOT_NOT_AVAILABLE)
			sprintf(string, "<%d> %s    %2d", i, 
				(time_slots+ i)->time_string, max_number - (time_slots +i )->count);	
		else
			sprintf(string, "<%d>", i);

		mvwaddstr(alt_times_win, 
					(1 + ((i <= (UI_TCR_MAX_TIME_SLOTS / 2)) ? 
							i : i - (UI_TCR_MAX_TIME_SLOTS / 2))), 
					((i <= (UI_TCR_MAX_TIME_SLOTS / 2)) ? 
							UI_TCR_WIN_COL_1 : UI_TCR_WIN_COL_2), string);
	  }

	prt_entry_areas(alt_times_win, alt_time_field, 1);

/*
 *	display window and get user response
 */
	wrefresh(alt_times_win);

	while (!alternate_selection)
	  {
		key_stroke = read_field(alt_times_win, alt_time_field, 
			UI_TCR_FIELD_PIU, reponse_buffer, &field_entered, &clear_field);
		
		switch(key_stroke)
		  {
			case '\n':

				if (reponse_buffer[0] == ' ')
				  {
					alternate_selection = TRUE;
					choice = 0;
				  }
				else
				  {
					rev_to_norm(alt_times_win, alt_time_field, UI_TCR_FIELD_PIU);
					choice = atoi(reponse_buffer);

					if (choice > 0 && choice <= UI_TCR_MAX_TIME_SLOTS)
				  	  {
						alternate_selection = TRUE;	
		
						if ((time_slots + choice)->count == UI_TCR_SLOT_NOT_AVAILABLE)
					  	  {
							tcr_print_message("%s", 
								catgets(UI_m_catd, UI_1, UI_RESTRICT_TC_4, 
						"Please select a time slot that has calls available"));	
							alternate_selection = FALSE;
					  	  }
				  	  }
					else
				  	  {
						tcr_print_message("%s",
							catgets(UI_m_catd, UI_1, UI_RESTRICT_TC_5, 
							"Please select an available time slot [1..6]"));	
				  	  }
				  }

				break;

			case PRINT_SCR_KEY:
				tcr_print_screen();
				break;

			default:
				break;
		  }
	  }

	werase(alt_times_win);
	wrefresh(alt_times_win);
	touchwin(stdscr);
	refresh();

	return(choice);
}


/*-----------------------------------------------------------------
 *==>	tcr start alt win
 *
 *		will only create window once
 *-----------------------------------------------------------------
 */

tcr_start_alt_win()
{
	static short	already_started = FALSE;


	if (already_started)
		return(TRUE);
	
	alt_times_win = newwin(UI_TCR_WIN_ROWS, UI_TCR_WIN_COLS, 
						UI_TCR_SCREEN_ROW, UI_TCR_SCREEN_COLS);

	already_started = TRUE;

	return(TRUE);
}
	

/*-----------------------------------------------------------------
 *==>	tcr print message
 *-----------------------------------------------------------------
 */

tcr_print_message(format, p1, p2, p3, p4, p5, p6, p7, p8)
	char 		*format;
	long		 p1, p2, p3, p4, p5, p6, p7, p8;
{

	prt_error(UI_NO_MSG, format, p1, p2, p3, p4, p5, p6, p7, p8);

	return(TRUE);
}


/*-----------------------------------------------------------------
 *==>	tcr print screen
 *-----------------------------------------------------------------
 */

tcr_print_screen()
{
	FILE		*popen(),
				*pptr;
	int 		 col, 
				 line,
				 wline,
				 wcol,
				 dec_la210, 
				 force_underline,
				 c;
	char 		*mads_spooler,
				*getenv(),
				 date_time[40],
				 sh_command[80];
	long 		 curr_time;


	if ((mads_spooler = getenv("MADS_SPOOLER")) == NULL) 
		mads_spooler = PRINT_CMD;	
	
	if (getenv("DECLA210") != NULL)
	    dec_la210 = 1;
	else
	    dec_la210 = 0;

	if (getenv("UNDERLINE") != NULL)
	    force_underline= 1;
	else
	    force_underline= 0;

	sprintf(sh_command,"%s -s 2> /dev/null", mads_spooler );

	if ((pptr = popen(sh_command, "w")) == NULL) 
	  {
		prt_error(UI_FATAL,
		catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_10, "<cannot open pipe for lp>"));
		return;
	  }

	curr_time = time(NULL);
	local_date_time(curr_time, NULL, NULL, date_time);
	fprintf(pptr, "%s\n", date_time);
	
	col = 0;
	for (line = 0; line < UI_TCR_SCREEN_ROW ; line++) 
	  {
		for (col = 0; col < COLS; col++) 
		  {
			move(line,col);
			c = inch();
			char_print(dec_la210, force_underline, c, pptr);
		  }
		fputc('\n',pptr);
	  }

	wcol = 0;
	for (wline = 0; wline < UI_TCR_WIN_ROWS ; wline++) 
	  {
		for (wcol = 0; wcol < UI_TCR_WIN_COLS ; wcol++)
		  {
			wmove(alt_times_win,wline,wcol);
			c = winch(alt_times_win);
			char_print(dec_la210, force_underline, c, pptr);
		  }
		fputc('\n',pptr);
	  }

	col = 0;			
	for (line = UI_TCR_SCREEN_ROW+UI_TCR_WIN_ROWS; line < LINES ; line++) 
	  {
		for (col = 0; col < COLS ; col++) 
		  {
			move(line,col);
			c = inch();
			char_print(dec_la210, force_underline, c, pptr);
		  }
		fputc('\n',pptr);
	  }

	if (!No_status_line)
	    fprintf(pptr, "%s\n", uiuser[user_index]->img_25);

	pclose(pptr);

	prt_error(UI_PRT_SCR,"<%s %s>", 
	catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_11, "on printer"), mads_spooler);

	return(TRUE);
}

