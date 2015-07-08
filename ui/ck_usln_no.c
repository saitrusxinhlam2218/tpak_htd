static char sz__FILE__[]="@(#)ck_usln_no.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ck_usln_no.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:46:58
 * @(#)  Last delta: 12/2/94 at 18:35:13
 * @(#)  SCCS File: /taxi/sccs/s.ck_usln_no.c
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include <sys/types.h>

#include "ui_strdefs.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "call_entry.h"
#define SRCH_EXTERNS
#include "srch.h"
#include "ui_def.h"
#include "func_keys.h"
#include "ui_error.h"
#include "screens.h"
#include "language.h"

#define 	FORWARD 		1
#define 	BACKWARD 		0
#define		SEARCHES_ON_PG 		17
#define		LINE_NUM_ROW		3
#define		LINE_NUM_COL		8
#define		LINE_NUM_MAX		2

extern int (*fnptr_srches)();
extern int matches_index;				/* for matches function */
extern int scr_name;
extern int bottom_of_page;
extern int page_num;

check_us_line_no(read_buf,key_ent_val, kb_field_entered,end_srch,calls_on_page, ptr_struct,pass_line_no)
char *read_buf;				/* buffer in which field entered is stored */
int key_ent_val;			/* value of "function key" entered by call taker */
int kb_field_entered;			/* 0=field not entered; 1=field entered */
int *end_srch;				/* 1=end srch; 0=continue srch */
int *calls_on_page;			/* # of calls which are on current page display */
struct select_criterias ptr_struct;	/* structure in which searches selection criteria is saved */
int *pass_line_no;			/* used to pass the current line number back to the search  */
{
	static int detail_call_no = NOT_AVAILABLE;
	struct scr_flds line_num;	/* for piu structure for line # entry */
	struct scr_flds *ptr_scr_fld;
	extern char req_buf[];		/* buffer for r/w records from/to disk */
	int got_calls;
	int zv_fld_entered;		/* whether zone or vehicle # entered */
	int fld_entered;	
	int piu;			/* prompt in use */
        int line_no;
        int ret_ck_fld;
	int key_val;
	int loop;
	WINDOW *win1;

	if ( kb_field_entered ) { 					/* if field was entered by digit */
		if ( ((line_no = atoi(read_buf)) < 1 ) || line_no > *calls_on_page) {
			prt_error(UI_WRONG_NUM," "); 					/* "Wrong number was entered" 	*/
		        mvaddstr(3,8,"    ");
			attrset(A_REVERSE);
		        mvaddstr(3,8,"    ");
			attrset(0);
			*pass_line_no = detail_call_no;
			return(DONE);
		}

		clear_user_detail_area();
		
		if (detail_call_no != NOT_AVAILABLE) 
			unhilite_us_line(detail_call_no); 
		
		detail_call_no = line_no - 1;
		hilite_us_line(detail_call_no); 			/*hilite the line on summary area */

		dsp_us_detail(detail_call_no);
				
		*pass_line_no = detail_call_no;
		return(DONE);
	}

	switch (key_ent_val)  { 			/* user entered a function key; check and process type of key */
		case '\r':
		case '\n': 				/* <CR> entered w/o entering line #; do nothing */
			*pass_line_no = detail_call_no;
			return(DONE);

		case KEY_UP:				/* display detail of previous user */
			if (detail_call_no == NOT_AVAILABLE)  {
				prt_error(UI_NO_DTL," ");
				*pass_line_no = detail_call_no;
				return(DONE);
			}

			unhilite_us_line(detail_call_no); 
			if (detail_call_no > 0)  
				detail_call_no--;
			else
				detail_call_no = (*calls_on_page) - 1;

			hilite_us_line(detail_call_no); 		/*hilite the line matched on summary area */

			clear_user_detail_area();
			dsp_us_detail(detail_call_no);
			*pass_line_no = detail_call_no;
			return(DONE);

		case KEY_DOWN: 						/* display detail of next user */
			if (detail_call_no==NOT_AVAILABLE)  {
				prt_error(UI_NO_DTL," ");
				*pass_line_no = detail_call_no;
				return(DONE);
			}

			unhilite_us_line(detail_call_no); 		/*unhilite the last hilited line */
			
			if (detail_call_no >= ((*calls_on_page) - 1))  
				detail_call_no = 0;
			else
				detail_call_no++;

			hilite_us_line(detail_call_no); 		/*hilite the line matched on summary area */
			clear_user_detail_area();
			dsp_us_detail(detail_call_no);
			*pass_line_no = detail_call_no;
			return(DONE);

		case NEXT_PG_KEY:
			got_calls = 0;
			if ( (*fnptr_srches)(YES, &got_calls, FORWARD, NO) )  {  
				if (got_calls)
					*calls_on_page = got_calls;
				detail_call_no = NOT_AVAILABLE; 
				
				clear_user_detail_area();
				erase_area(6,17);
				disp_usr_sum(*calls_on_page);
			}
			else  
				prt_error(UI_NO_RECORD,catgets(UI_m_catd, UI_1, UI_CK_USLN_NO_2, "<in the direction you are going>"));

			*pass_line_no = detail_call_no;
			return(DONE);

		case PREV_PG_KEY:
			got_calls = 0;
			if (page_num > 1 && (*fnptr_srches)(YES, &got_calls, BACKWARD, NO) )  {  
				if (got_calls)
					*calls_on_page = got_calls;
				detail_call_no = NOT_AVAILABLE; 
				clear_user_detail_area();
				erase_area(6,17);
				disp_usr_sum(*calls_on_page);
			}
			else  
				prt_error(UI_NO_RECORD,catgets(UI_m_catd, UI_1, UI_CK_USLN_NO_2, "<in the direction you are going>"));

			*pass_line_no = detail_call_no;
			return(DONE);

		case REFRESH_KEY:
			got_calls = 0;
			bottom_of_page = 1;
			if ( (*fnptr_srches)(YES, &got_calls, FORWARD, YES) )  {  
				if (got_calls)
					*calls_on_page = got_calls;
				detail_call_no = NOT_AVAILABLE; 
				clear_user_detail_area();
				erase_area(6,17);
				disp_usr_sum(*calls_on_page);
			}
			else  
				prt_error(UI_NO_RECORD,catgets(UI_m_catd, UI_1, UI_CK_USLN_NO_2, "<in the direction you are going>"));

			*pass_line_no = detail_call_no;
			return(DONE);

		case PREV_SCR_KEY:
		case COMMAND_KEY:
			detail_call_no = NOT_AVAILABLE;
			return(NOT_DONE);
	}
	return(NOT_DONE);
}
