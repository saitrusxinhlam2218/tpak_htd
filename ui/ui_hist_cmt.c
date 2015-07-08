/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ui_hist_cmt.c,v $
*  @(#)  $Revision: 1.8 $
*  @(#)  $Date: 2003/01/28 17:55:29 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/ui/ui_hist_cmt.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

#include <string.h>
#include <sys/time.h>
#include <cursesX.h>
#include <sys/types.h>
#include <isam.h>
#include "language.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "comm_strdefs.h"
#include "writer_strdefs.h"
#include "mad_error.h"
#include "mad_ipc.h"
#include "except.h"
#include "ui_strdefs.h"
#include "Hist_cmt_private.h"
#include "path.h"
#include "func_keys.h"
#include "ui_def.h"
#include "mesg.h"
#include "scr.h"
#include "screens.h"
#include "ui_error.h"
#include "writer.h"
#include "switch_enh.h"

extern struct keydesc hc_key1;
extern struct keydesc hc_key2;
extern struct keydesc ch_key1;
extern struct scr_flds mesg_flds[];
#ifdef FOO
extern BOOLEAN Scrn_error_displayed;
#endif
extern int uid;
extern struct cisam_cl srch_calls[];
  extern struct cisam_sc subs_calls[];
extern int 	scr_name;
extern char req_buf[];
extern long glEnhSwitch; 
WINDOW  *newwin();

typedef struct {
	   char buffer[HIST_TEXT_LEN+1];
	} CMT_REC;

struct scr_flds hist_cmt_flds[] = {
  {0, 0, 1, 1},
  {1, 14, 30, M_TXT1}
};

add_hist_cmt(msg_text, iCallNbr)
char *msg_text;		/* The buffer holding the 90 char message */
int iCallNbr;		/* Call number */
{
	HIST_CMT_REC *hc_rec;
	int i = 0, j = 0, no_items = 0, end_of_string = 0, next_part = 0, offset = 0;
	short rc = 0, counter = 0;
	CMT_REC cmt_array[3];
 	time_t	hist_cmt_dt_tm;	

	hc_rec = (HIST_CMT_REC *)req_buf;
	
	rc = db(HIST_CMT_FILE_ID,OPEN, &hc_key1,ISINOUT+ISMANULOCK, 0);
	if (rc < 0)
        {
                ERROR(' ', "", "Could not open histcmt table");
                return(-1);
        }

	/******* Set date_time stamps for all records ******/
	hist_cmt_dt_tm = time((time_t *) 0);

	/******* Parse msg_text into three separate records *********/
	for (i = 0; i < 3; i++)
	{
		counter = 0;
		for (j = 0; j <= HIST_TEXT_LEN - 1; j++)
		{
			if (msg_text[offset] == NULL)
			{
				cmt_array[i].buffer[counter] = NULL;
				end_of_string = 1;
				break;
			}

			cmt_array[i].buffer[counter] = msg_text[offset];
			offset++;
			counter++;
		}
		cmt_array[i].buffer[HIST_TEXT_LEN] = NULL;
		if (end_of_string)
		{
			if (j != 0)
				no_items++;
			break;
		}
		no_items++;
	}

	/******* Write each record to histcmt table *******/
	for (i = no_items-1; i >= 0; i--)
	{
		hc_rec->call_nbr = iCallNbr;
		hc_rec->next_part = next_part;
		strcpy(hc_rec->text,cmt_array[i].buffer);
		hc_rec->dt_tm = hist_cmt_dt_tm;
		if ((rc = db(HIST_CMT_FILE_ID,ADD_REC,&hc_key1, 0, 0)) != 0)
		{
			ERROR(' ',"", "Error writing to histcmt table");
			db(HIST_CMT_FILE_ID,CLOSE,&hc_key1, 0, 0);
			return(-1);
		}
		next_part = hc_rec->comment_id;
	}
	db(HIST_CMT_FILE_ID, CLOSE, &hc_key1, 0, 0);
	return(hc_rec->comment_id);

}

int recall_hist_cmt(comment_id, no_items, cmt_array)
int comment_id, *no_items;	/* The comment_id in the histcmt table */
CMT_REC cmt_array[];		/* The char buffer to hold the recalled message */
{
	short rc = 0;
	HIST_CMT_REC *hc_rec;
	int index = 0;

	hc_rec = (HIST_CMT_REC *)req_buf;

	rc = db(HIST_CMT_FILE_ID,OPEN,&hc_key1, ISINOUT+ISMANULOCK, 0);
	if (rc < 0)
	{
		ERROR(' ', "", "Could not open histcmt table");
		return(-1);
	}

	hc_rec->comment_id = comment_id;
 	while(1)
	{
		rc = db(HIST_CMT_FILE_ID,READ_KEYS,&hc_key1,ISEQUAL,0);
		if (rc < 0)
		{
			ERROR(' ',"", "Could not find specified record in histcmt table");
			db(HIST_CMT_FILE_ID,CLOSE,&hc_key1, 0, 0);
			return(-1);
		}
		strcpy(cmt_array[index].buffer, hc_rec->text);
		*no_items += 1;
		if (hc_rec->next_part == 0)
		{
			db(HIST_CMT_FILE_ID,CLOSE,&hc_key1, 0, 0);
			return(0);
		}
		else
		{
			hc_rec->comment_id = hc_rec->next_part;
			index++;
		}
	}
}

void print_hist_cmt(srch_win, row, col, ch_ptr, counter, key_val)
WINDOW *srch_win;
int *row, *col;
struct cisam_ch *ch_ptr;
int *counter, key_val;
{
	int no_items = 0, i = 0;
	char temp_buff1[READ_BUF_SIZE], temp_buff2[READ_BUF_SIZE], buffer[READ_BUF_SIZE];
	CMT_REC cmt_array[3];
	static char hold_msg[50];
	struct cisam_ch tmp_call_hist;

	buffer[0] = NULL;
	temp_buff1[0] = NULL;
	temp_buff2[0] = NULL;
	/** Need to save some callh info because recall_hist_cmt will wipe out
	    what's in req_buf **/
	strcpy(tmp_call_hist.event_desc, ch_ptr->event_desc);
	strcpy(tmp_call_hist.status_date, ch_ptr->status_date);
	strcpy(tmp_call_hist.status_time, ch_ptr->status_time);
	tmp_call_hist.user_id = ch_ptr->user_id;
	recall_hist_cmt(ch_ptr->drv_id, &no_items, cmt_array);

	for (i = 0; i < no_items; i++)
		if (i == 0)
		{
			if (no_items >= 2 && key_val == PREV_PG_KEY)
			{
				sprintf(temp_buff2,"%-8s %-9s %-6s %-4d %-30s",tmp_call_hist.event_desc,
				tmp_call_hist.status_date, tmp_call_hist.status_time, tmp_call_hist.user_id,  cmt_array[i].buffer);
			}
			else
			{
				sprintf(temp_buff1,"%-8s %-9s %-6s %-4d %-30s",tmp_call_hist.event_desc,
                                tmp_call_hist.status_date, tmp_call_hist.status_time, tmp_call_hist.user_id, cmt_array[i].buffer);
			}
			*counter += 1;
		}
		else
		{
			sprintf(buffer,"%-30s", cmt_array[i].buffer);
			if (no_items >= 2 && key_val == PREV_PG_KEY)
				strcat(temp_buff1, buffer);
			else
				strcat(temp_buff2, buffer);
		}
	mvwaddstr(srch_win,*row,*col,temp_buff1);
	if (no_items > 1)
	{
		switch(key_val)
		{
			default:

			case NEXT_PG_KEY: 	*row += 1;
						break;

			case PREV_PG_KEY: 	*row -= 1;
                                        	break;
		}
			
		if (*row >= 5 && *col == 1 && key_val != PREV_PG_KEY)
		{
			*row = 1;
			*col = 63;
		}
		else
			if (*row < 1 && *col == 63 && key_val == PREV_PG_KEY)
			{
				*row = 4;
				*col = 1;
			}
			else
				if (*row >= 5 && *col == 63)
				{
					*counter += 1;
					strcpy(hold_msg, temp_buff2);
					return;
				}
		*counter += 1;
		mvwaddstr(srch_win,*row,*col,temp_buff2);
	}
}

call_trp_cmt_disp(WHistCmt)
WINDOW *WHistCmt;
{
	char buffer[BUFSIZ];

	sprintf(buffer, catgets( UI_m_catd, UI_1, UI_UI_MISC_12, "Comment:" ));
        wclear(WHistCmt);
        mvwprintw(WHistCmt,1,3,buffer);
}

comment_key(srch_win, srch_index)
WINDOW *srch_win;
int srch_index;
{
	char mbuf[READ_BUF_SIZE];		/* buffer for read field */
	int key_val;				/* key for read_field */
	int field_entered;
	int cancel = 0;				/* flag to end while loop */
	int piu;			/* piu value */
	struct cisam_rm rmesg;			/* repeat message structure to build messge */
	int should_clear = 0;
	struct cisam_ch *ch_ptr;
	char date_str[10];
	char time_str[10];
	WINDOW  *WHistCmt;

	WHistCmt = newwin(3, 60, 14, 24);

	switch (scr_name)
	{
 	case UNASSIGNED_CALLS_SCR:
        case ASSIGNED_CALLS_SCR:
        case VEH_DR_CALLS_SCR:
        case ALL_CALLS_SCR:
        case INDIVIDUAL_CALLS_SCR:
	case EXCEPTIONS_SCR:
	case SUBS_FUTURE_CLS_SCR:
		memset((char *)&rmesg,'\0',sizeof(struct cisam_rm));
		call_trp_cmt_disp(WHistCmt);		/* display message window */
		wrefresh(WHistCmt);
		tpak_box(WHistCmt, 0, 0);
		wrefresh(WHistCmt);
		prt_entry_areas(WHistCmt,&hist_cmt_flds[0],1); /* fill in user entry areas with underscores */
		wrefresh(WHistCmt);
		refresh();

		piu = 1;
		while (cancel == 0) 
		{
			key_val = read_field(WHistCmt,hist_cmt_flds,piu,mbuf,&field_entered,&should_clear);

			if (field_entered == 0) 
				mbuf[0] = '\0';

			strcpy(rmesg.line_1,mbuf);
#ifdef FOO
			if (Scrn_error_displayed &&  key_val != -1) 
			  Scrn_clear_error();
#endif

			switch (key_val) 
			{
				case PRINT_SCR_KEY:	/* print screen image with window  overlay */
					print_scr(WHistCmt);
					break;

				case COMMENT_KEY:
					cancel = 1;
					break;

				default:
					break;
			}
		}

		if (strlen(mbuf))
		  {
		    ch_ptr = (struct cisam_ch *)req_buf;
		    memset((char *)ch_ptr,'\0',sizeof(struct cisam_ch));
		    if (ch_ptr->drv_id = (add_hist_cmt(rmesg.line_1,srch_calls[srch_index].nbr)))
		      {
			if ( scr_name == SUBS_FUTURE_CLS_SCR )
			  ch_ptr->call_nbr = subs_calls[srch_index].nbr;
			else
			  ch_ptr->call_nbr = srch_calls[srch_index].nbr;
			ch_ptr->exception_nbr = 0;
			ch_ptr->event_type = REGULAR_EVENT;
			ch_ptr->status_dt_tm = (int) time(NULL);
			get_pkup_date(date_str);
			strcpy(ch_ptr->status_date, date_str);
			get_pkup_time(time_str);
			strcpy(ch_ptr->status_time, time_str);
			strcpy(ch_ptr->event_desc, COMMENT);
			ch_ptr->veh_nbr = 0;
			ch_ptr->user_id = (short)uid;
			if ( scr_name == SUBS_FUTURE_CLS_SCR )
			  {
			    if (0)
			      {
				if(db(SUBCALLH_FILE_ID,ADD_REC,&ch_key1,0,0) < 0)  	/* add the record to disk */
				  prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_CALL_H_1, "<call history record>"));
			      }
			    else
			      prt_error(UI_NOFIELD, catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1, "<wrong function key pressed>"));
			  }
			else
			  {
			    if(db(CALLH_FILE_ID,ADD_REC,&ch_key1,0,0) < 0)  	/* add the record to disk */
			      prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_CALL_H_1, "<call history record>"));
			  }
		      }
		  }

		werase(WHistCmt);
		touchwin(stdscr);
		return;

	default:
		prt_error(UI_NOT_ALLOWED, catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1,"<wrong function key pressed>"));
         	return;
	}
}