static char sz__FILE__[]="@(#)hist.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  hist.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:48:54
 * @(#)  Last delta: 12/2/94 at 18:46:28
 * @(#)  SCCS File: /taxi/sccs/s.hist.c
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include <memory.h>
#include <sys/types.h>
#include <ctype.h>

#include "call_entry.h"
#include "screens.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "ui_def.h"
#include "ui_ipc.h"
#include "user.h"
#include "ui_error.h"
#include "df_maint.h"
#include "func_keys.h"
#include "writer.h"

#define GB  gbox();
extern int scr_name;
extern short silent;
extern short clear_err_flag;
extern struct keydesc	ch_key3;
extern char req_buf[];
extern WINDOW *srch_win;
extern char *Spaces;
struct scr_flds ch_scr_fld[] = {
	{0,0,1,1},
	{0,1,1,1},
};

/***********************************************************************************/
/* call_history - this function will display the call history window		   */
/***********************************************************************************/
call_history(call_num)
int call_num;
{
	struct cisam_ch	*ch_ptr;
	struct	scr_flds  *ptr_scr_fld;
	int key_val = 0, no_displayed = 0;
	int end_help = 0;
	int field_entered = 0;
	char read_buf[READ_BUF_SIZE];
	char	temp_buff[65];
	int  row = 1;
	int  col = 1;
	int  counter,m,n;
	int  k,j = 1;
	int  piu = 1;
	int  line_flags = 1;
	int  check_flags = 0;
	int  temp_flags = 1;
	int  prev_flags = 0;
	int i;
	int rc = 0;
	int should_clear = 0;

	ptr_scr_fld = ch_scr_fld;
	ch_ptr = (struct cisam_ch *)req_buf;

	ch_ptr->call_nbr = call_num;
	ch_ptr->status_dt_tm = 0;

	if (db(CALLH_FILE_ID,READ_KEYS,&ch_key3,ISGTEQ,0) < 0) {
		prt_error(UI_NOHISTORY," ");
		return(0);
	}
	if (ch_ptr->call_nbr != call_num) {
		prt_error(UI_NOHISTORY," ");
		return(0);
	}

	werase(srch_win);

	/* curses needs touch_win() to really erase window.
	   For some reason it doesn't work without it. */

	touchwin(srch_win);
	tpak_box( srch_win, 0, 0 );

	wattrset(srch_win, A_NORMAL);
	for (counter=1; counter<=8; ++counter) {
		if (counter == 5) {
			row = 1;
			col = 63;
		}
		if (strcmp(ch_ptr->event_desc, COMMENT) == 0)
		  print_hist_cmt(srch_win, &row, &col, ch_ptr, &no_displayed, key_val);
		else
		  {
		    sprintf(temp_buff,"%-8s %-9s %-6s %-4d %-4d %-8s %-6s %-4d %-4d",ch_ptr->event_desc,ch_ptr->status_date,
			    ch_ptr->status_time, ch_ptr->veh_nbr,ch_ptr->user_id,ch_ptr->action_taken,ch_ptr->action_time,
			    ch_ptr->action_veh_nbr,ch_ptr->action_user_id);
		    no_displayed++;
		    mvwaddstr(srch_win,row,col,temp_buff);
		  }
		++row;
		if (db(CALLH_FILE_ID,READ_KEYS,&ch_key3,ISNEXT,0) < 0) {
			line_flags = 0;
			break;
		}
		if (ch_ptr->call_nbr != call_num) {
			line_flags = 0;
			break;
		}
	}
	wrefresh(srch_win);

	db(CALLH_FILE_ID,READ_KEYS,&ch_key3,ISPREV,0); 

	while(!end_help) {
	        wattroff( srch_win, A_BLINK );
		wrefresh( srch_win );
		silent = 1;
		key_val = read_field(srch_win,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);

		if (key_val == -1)
			continue;

		clear_error();
		refresh();

		switch(key_val)	{
			case	NEXT_PG_KEY:
				if (!line_flags) {
					prt_error(UI_NOHISTORY," ");
					break;
				}
				if (!temp_flags) {
					prt_error(UI_NOHISTORY," ");
					break;
				}
				if (prev_flags)	{
					for (k = 1; k < 8; ++k)
						db(CALLH_FILE_ID,READ_KEYS,&ch_key3,ISNEXT,0);
				} 
	
				if (counter == 1)
					counter = 9;

				row = 1;
				col = 1;

				if ((counter % 8) != 1)
					prt_error(UI_NOHISTORY," ");
				else {
					++j;
					if ((counter <= (j * 8)) && (check_flags != 0))
					  {
					    werase(srch_win);
					    tpak_box( srch_win, 0, 0 );
					  }
					while (counter <= (j * 8)) {
						if (db(CALLH_FILE_ID,READ_KEYS,&ch_key3,ISNEXT,0) < 0) {
							if (check_flags == 0) {
								prt_error(UI_NOHISTORY," ");
							}
							if (temp_flags == 0) {
								prt_error(UI_NOHISTORY," ");
							}
							break;
						}		
						if (ch_ptr->call_nbr != call_num) {
							temp_flags = 0;
							db(CALLH_FILE_ID,READ_KEYS,&ch_key3,ISPREV,0);
							break;
						}

						if (counter == 9) {
							werase(srch_win);
							tpak_box( srch_win, 0, 0 );
						}
						if ((counter % 8) == 5)	{
							row = 1;
							col = 63;
						}
						if (strcmp(ch_ptr->event_desc, COMMENT) == 0)
						  print_hist_cmt(srch_win, &row, &col, ch_ptr, &no_displayed, key_val);
						else
						  {
						    sprintf(temp_buff,"%-8s %-9s %-6s %-4d %-4d %-8s %-6s %-4d %-4d",ch_ptr->event_desc,
							    ch_ptr->status_date,ch_ptr->status_time, ch_ptr->veh_nbr,ch_ptr->user_id,
							    ch_ptr->action_taken,ch_ptr->action_time,ch_ptr->action_veh_nbr,
							    ch_ptr->action_user_id);
						    no_displayed++;
						    mvwaddstr(srch_win,row,col,temp_buff);
						  }
						++row;
						++counter;
						++check_flags;
						prev_flags = 0;
					}
					wrefresh(srch_win);

					if ((counter % 8) == 1)	{
						if (db(CALLH_FILE_ID,READ_KEYS,&ch_key3,ISNEXT,0) < 0)	{
							temp_flags = 0;
						}
						else {
							if (ch_ptr->call_nbr != call_num)	
								temp_flags = 0;
							db(CALLH_FILE_ID,READ_KEYS,&ch_key3,ISPREV,0);
						}
					}
				}
				break;

			case PREV_PG_KEY:
				while(1) {
					if (line_flags == 0) {
						prt_error(UI_NOPREV," ");
						break;
					}
					if ((counter == 9)) {
						prt_error(UI_NOPREV," ");
						break;
					}
					row = 4;
					col = 63;
					if ((counter % 8) == 1)	{
						if (!prev_flags) {
							for (k = 1; k <= 7; ++k)
								db(CALLH_FILE_ID,READ_KEYS,&ch_key3,ISPREV,0);
						}
					}
					else {
						if (!prev_flags)
							counter += 8;
						n = counter % 8;
						if (n == 0) {
							counter -= 7;	
							if (!prev_flags) {
								for (k = 1; k <= 6; ++k)
									db(CALLH_FILE_ID,READ_KEYS,&ch_key3,ISPREV,0);
							}
						} 
						else {
							counter = counter-(n-1);
							if (!prev_flags) {
								for (k = 1; k < (n-1); ++k)
									db(CALLH_FILE_ID,READ_KEYS,&ch_key3,ISPREV,0);
							}
						} 
					}
					if (counter == 0)
						counter = 8;
					m = counter - 8;
					if (counter > m)
					  {
					    werase(srch_win);
					    tpak_box( srch_win, 0, 0 );
					  }
					wrefresh(srch_win);
					while (counter > m) {
						if (db(CALLH_FILE_ID,READ_KEYS,&ch_key3,ISPREV,0) < 0)	{
							break;
						}		
						if ((counter % 8) == 5)	{
							row = 4;
							col = 1;
						}
						sprintf(temp_buff,"%-8s %-9s %-6s %-4d %-4d %-8s %-6s %-4d %-4d",ch_ptr->event_desc,
							ch_ptr->status_date,ch_ptr->status_time, ch_ptr->veh_nbr,ch_ptr->user_id,
							ch_ptr->action_taken,ch_ptr->action_time,ch_ptr->action_veh_nbr,
							ch_ptr->action_user_id);
						mvwaddstr(srch_win,row,col,temp_buff);
						--row;
						--counter;
					}
					wrefresh(srch_win);
					--j;
					prev_flags = 1;
					temp_flags = 1;
					break;
				}
				break;		

			case HISTORY_KEY:
				end_help = 1;
				break;

			case COMMAND_KEY:
				end_help = 1;
				werase(srch_win);
				wrefresh(srch_win);
				touchwin(stdscr);
				refresh();
				command_line();
				rc = 1;
				break;

			case PRINT_SCR_KEY:
				print_scr(srch_win);
				break;

			default:
				prt_error(UI_INV_KEY," ");
				break;
		}
	}
	werase(srch_win);
	wrefresh(srch_win);
	touchwin(stdscr);
	refresh();

	/* 0, go back to previous screen, 1 command key weas entered */
	return(rc);
}

/***********************************************************************************/
/* subcall_history - this function will display the subcall history window		   */
/***********************************************************************************/
subcall_history(call_num)
int call_num;
{
	struct cisam_subch	*ch_ptr;
	struct	scr_flds  *ptr_scr_fld;
	int key_val = 0, no_displayed = 0;
	int end_help = 0;
	int field_entered = 0;
	char read_buf[READ_BUF_SIZE];
	char	temp_buff[65];
	int  row = 1;
	int  col = 1;
	int  counter,m,n;
	int  k,j = 1;
	int  piu = 1;
	int  line_flags = 1;
	int  check_flags = 0;
	int  temp_flags = 1;
	int  prev_flags = 0;
	int i;
	int rc = 0;
	int should_clear = 0;

	ptr_scr_fld = ch_scr_fld;
	ch_ptr = (struct cisam_subch *)req_buf;

	ch_ptr->call_nbr = call_num;
	ch_ptr->status_dt_tm = 0;

	if (db(SUBCALLH_FILE_ID,READ_KEYS,&ch_key3,ISGTEQ,0) < 0) {
		prt_error(UI_NOHISTORY," ");
		return(0);
	}
	if (ch_ptr->call_nbr != call_num) {
		prt_error(UI_NOHISTORY," ");
		return(0);
	}

	werase(srch_win);

	/* curses needs touch_win() to really erase window.
	   For some reason it doesn't work without it. */

	touchwin(srch_win);
	tpak_box( srch_win, 0, 0 );

	wattrset(srch_win, A_NORMAL);
	for (counter=1; counter<=8; ++counter) {
		if (counter == 5) {
			row = 1;
			col = 63;
		}
		if (strcmp(ch_ptr->event_desc, COMMENT) == 0)
		  print_hist_cmt(srch_win, &row, &col, ch_ptr, &no_displayed, key_val);
		else
		  {
		    sprintf(temp_buff,"%-8s %-9s %-6s %-4d %-4d %-8s %-6s %-4d %-4d",ch_ptr->event_desc,ch_ptr->status_date,
			    ch_ptr->status_time, ch_ptr->veh_nbr,ch_ptr->user_id,ch_ptr->action_taken,ch_ptr->action_time,
			    ch_ptr->action_veh_nbr,ch_ptr->action_user_id);
		    no_displayed++;
		    mvwaddstr(srch_win,row,col,temp_buff);
		  }
		++row;
		if (db(SUBCALLH_FILE_ID,READ_KEYS,&ch_key3,ISNEXT,0) < 0) {
			line_flags = 0;
			break;
		}
		if (ch_ptr->call_nbr != call_num) {
			line_flags = 0;
			break;
		}
	}
	wrefresh(srch_win);

	db(SUBCALLH_FILE_ID,READ_KEYS,&ch_key3,ISPREV,0); 

	while(!end_help) {
	        wattroff( srch_win, A_BLINK );
		wrefresh( srch_win );
		silent = 1;
		key_val = read_field(srch_win,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);

		if (key_val == -1)
			continue;

		clear_error();
		refresh();

		switch(key_val)	{
			case	NEXT_PG_KEY:
				if (!line_flags) {
					prt_error(UI_NOHISTORY," ");
					break;
				}
				if (!temp_flags) {
					prt_error(UI_NOHISTORY," ");
					break;
				}
				if (prev_flags)	{
					for (k = 1; k < 8; ++k)
						db(SUBCALLH_FILE_ID,READ_KEYS,&ch_key3,ISNEXT,0);
				} 
	
				if (counter == 1)
					counter = 9;

				row = 1;
				col = 1;

				if ((counter % 8) != 1)
					prt_error(UI_NOHISTORY," ");
				else {
					++j;
					if ((counter <= (j * 8)) && (check_flags != 0))
					  {
					    werase(srch_win);
					    tpak_box( srch_win, 0, 0 );
					  }
					while (counter <= (j * 8)) {
						if (db(SUBCALLH_FILE_ID,READ_KEYS,&ch_key3,ISNEXT,0) < 0) {
							if (check_flags == 0) {
								prt_error(UI_NOHISTORY," ");
							}
							if (temp_flags == 0) {
								prt_error(UI_NOHISTORY," ");
							}
							break;
						}		
						if (ch_ptr->call_nbr != call_num) {
							temp_flags = 0;
							db(SUBCALLH_FILE_ID,READ_KEYS,&ch_key3,ISPREV,0);
							break;
						}

						if (counter == 9) {
							werase(srch_win);
							tpak_box( srch_win, 0, 0 );
						}
						if ((counter % 8) == 5)	{
							row = 1;
							col = 63;
						}
						if (strcmp(ch_ptr->event_desc, COMMENT) == 0)
						  print_hist_cmt(srch_win, &row, &col, ch_ptr, &no_displayed, key_val);
						else
						  {
						    sprintf(temp_buff,"%-8s %-9s %-6s %-4d %-4d %-8s %-6s %-4d %-4d",ch_ptr->event_desc,
							    ch_ptr->status_date,ch_ptr->status_time, ch_ptr->veh_nbr,ch_ptr->user_id,
							    ch_ptr->action_taken,ch_ptr->action_time,ch_ptr->action_veh_nbr,
							    ch_ptr->action_user_id);
						    no_displayed++;
						    mvwaddstr(srch_win,row,col,temp_buff);
						  }
						++row;
						++counter;
						++check_flags;
						prev_flags = 0;
					}
					wrefresh(srch_win);

					if ((counter % 8) == 1)	{
						if (db(SUBCALLH_FILE_ID,READ_KEYS,&ch_key3,ISNEXT,0) < 0)	{
							temp_flags = 0;
						}
						else {
							if (ch_ptr->call_nbr != call_num)	
								temp_flags = 0;
							db(SUBCALLH_FILE_ID,READ_KEYS,&ch_key3,ISPREV,0);
						}
					}
				}
				break;

			case PREV_PG_KEY:
				while(1) {
					if (line_flags == 0) {
						prt_error(UI_NOPREV," ");
						break;
					}
					if ((counter == 9)) {
						prt_error(UI_NOPREV," ");
						break;
					}
					row = 4;
					col = 63;
					if ((counter % 8) == 1)	{
						if (!prev_flags) {
							for (k = 1; k <= 7; ++k)
								db(SUBCALLH_FILE_ID,READ_KEYS,&ch_key3,ISPREV,0);
						}
					}
					else {
						if (!prev_flags)
							counter += 8;
						n = counter % 8;
						if (n == 0) {
							counter -= 7;	
							if (!prev_flags) {
								for (k = 1; k <= 6; ++k)
									db(SUBCALLH_FILE_ID,READ_KEYS,&ch_key3,ISPREV,0);
							}
						} 
						else {
							counter = counter-(n-1);
							if (!prev_flags) {
								for (k = 1; k < (n-1); ++k)
									db(SUBCALLH_FILE_ID,READ_KEYS,&ch_key3,ISPREV,0);
							}
						} 
					}
					if (counter == 0)
						counter = 8;
					m = counter - 8;
					if (counter > m)
					  {
					    werase(srch_win);
					    tpak_box( srch_win, 0, 0 );
					  }
					wrefresh(srch_win);
					while (counter > m) {
						if (db(SUBCALLH_FILE_ID,READ_KEYS,&ch_key3,ISPREV,0) < 0)	{
							break;
						}		
						if ((counter % 8) == 5)	{
							row = 4;
							col = 1;
						}
						sprintf(temp_buff,"%-8s %-9s %-6s %-4d %-4d %-8s %-6s %-4d %-4d",ch_ptr->event_desc,
							ch_ptr->status_date,ch_ptr->status_time, ch_ptr->veh_nbr,ch_ptr->user_id,
							ch_ptr->action_taken,ch_ptr->action_time,ch_ptr->action_veh_nbr,
							ch_ptr->action_user_id);
						mvwaddstr(srch_win,row,col,temp_buff);
						--row;
						--counter;
					}
					wrefresh(srch_win);
					--j;
					prev_flags = 1;
					temp_flags = 1;
					break;
				}
				break;		

			case HISTORY_KEY:
				end_help = 1;
				break;

			case COMMAND_KEY:
				end_help = 1;
				werase(srch_win);
				wrefresh(srch_win);
				touchwin(stdscr);
				refresh();
				command_line();
				rc = 1;
				break;

			case PRINT_SCR_KEY:
				print_scr(srch_win);
				break;

			default:
				prt_error(UI_INV_KEY," ");
				break;
		}
	}
	werase(srch_win);
	wrefresh(srch_win);
	touchwin(stdscr);
	refresh();

	/* 0, go back to previous screen, 1 command key weas entered */
	return(rc);
}
