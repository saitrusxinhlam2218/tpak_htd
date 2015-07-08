static char sz__FILE__[]="@(#)vh_dr_no.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  vh_dr_no.c; Rel: 6.3.0.0; Get date: 1/22/93 at 10:47:03
 * @(#)  Last delta: 11/30/92 at 15:48:18
 * @(#)  SCCS File: /taxi/sccs/s.vh_dr_no.c
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include <sys/types.h>

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
#include "ui_strdefs.h"

#define 	FORWARD 		1
#define 	BACKWARD 		0
#define		SEARCHES_ON_PG 		17
#define		LINE_NUM_ROW		3
#define		LINE_NUM_COL		8
#define		LINE_NUM_MAX		2
#define		NUM_SEALS		16

extern int (*fnptr_srches)();
extern int matches_index;					/* for matches function */
extern int scr_name;
extern int bottom_of_page;
extern struct keydesc zn_key1;
extern struct keydesc veh_key2;
extern char seal[NUM_SEALS][4];
extern char *Spaces;
extern int page_num;

/********************************************************************************/
/* check_vh_line_no: reads the line number entered by the call taker. Displays  */
/*			the detail of the line # entered in the detail area     */
/*			(lines 2-6). This routine also processes the 		*/
/*			<DETAIL>, <NEXT_PG>, <PREV_PG>, etc. function keys,     */
/*			which are specific to this searches                     */
/********************************************************************************/

check_vh_dr_line_no(read_buf,key_ent_val, kb_field_entered,end_srch,calls_on_page,ptr_struct,pass_line_no)
char *read_buf;						/* buffer in which field entered is stored */
int key_ent_val;					/* value of "function key" entered by call taker */
int kb_field_entered;					/* 0=field not entered; 1=field entered */
int *end_srch;						/* 1=end srch; 0=continue srch */
int *calls_on_page;					/* # of calls which are on current page display */
struct select_criterias ptr_struct;			/* structure in which searches selection criteria is saved */
int *pass_line_no;					/* used to pass the current line number back to the search */
{
	static int detail_call_no = NOT_AVAILABLE;
	struct scr_flds line_num;			/* for piu structure for line # entry */
	struct scr_flds *ptr_scr_fld;
	extern char req_buf[];				/* buffer for r/w records from/to disk */
	int got_calls;
	int zv_fld_entered;				/* whether zone or vehicle # entered */
	int fld_entered;	
	int piu;					/* prompt in use */
        int line_no;
        int ret_ck_fld;
	int key_val;
	int loop;
	WINDOW *win1;

	if ( kb_field_entered ) { 							/* if field was entered by digit */
		if ( ((line_no = atoi(read_buf)) < 1 ) || line_no > *calls_on_page) { 	/* check if line# entered is within range */
			prt_error(UI_WRONG_NUM," "); 					/* "Wrong number was entered" 	*/ 
		        mvaddstr(3,8,"    ");
			attrset(A_REVERSE);
		        mvaddstr(3,8,"    ");
			attrset(0);
			*pass_line_no = detail_call_no;
			return(DONE);
		}

		clear_vhdr_detail_area();
		if (detail_call_no != NOT_AVAILABLE) { 				/*unhilite the line matched on summary area */
			if (scr_name == VEHICLE_SRCH_SCR)
				unhilite_vh_line(detail_call_no); 
			else 	if (scr_name == DRIVER_SRCH_SCR)
					unhilite_dr_line(detail_call_no); 
		}
		detail_call_no = line_no - 1;
		if (scr_name == VEHICLE_SRCH_SCR){
			hilite_vh_line(detail_call_no); 	/*hilite the line on summary area */
			dsp_vh_detail(detail_call_no); 		/* line # entered is valid; display detail of this call */
		}
		else  	if (scr_name == DRIVER_SRCH_SCR){
				hilite_dr_line(detail_call_no); 
				dsp_dr_detail(detail_call_no);
		}

		*pass_line_no = detail_call_no;
		return(DONE);
	}

	switch (key_ent_val)  { 				/* user entered a function key; check and process type of key */
		case '\r':
		case '\n':
			*pass_line_no = detail_call_no; 	/* <CR> entered w/o entering line #; do nothing */
			return(DONE);

		case KEY_UP:					/* display detail of previous call */
			if (detail_call_no==NOT_AVAILABLE)  {
				prt_error(UI_NO_DTL," "); 	/* "No detail line number available" 	*/
				*pass_line_no = detail_call_no;
				return(DONE);
			}

			if (scr_name == VEHICLE_SRCH_SCR)
				unhilite_vh_line(detail_call_no); 
			else 	if (scr_name == DRIVER_SRCH_SCR)
					unhilite_dr_line(detail_call_no); 

			if (detail_call_no > 0)  
				detail_call_no--;
			else
				detail_call_no = (*calls_on_page) - 1;

			if (scr_name == VEHICLE_SRCH_SCR)
				hilite_vh_line(detail_call_no); 	/*hilite the line matched on summary area */
			else 	if (scr_name == DRIVER_SRCH_SCR)
					hilite_dr_line(detail_call_no); 

			clear_vhdr_detail_area();
			if (scr_name == VEHICLE_SRCH_SCR)
				dsp_vh_detail(detail_call_no);
			else	if (scr_name == DRIVER_SRCH_SCR)
					dsp_dr_detail(detail_call_no);

			*pass_line_no = detail_call_no;
			return(DONE);

		case KEY_DOWN: 						/* display detail of next call */
			if (detail_call_no == NOT_AVAILABLE)  {
				prt_error(UI_NO_DTL," "); 		/* "No detail line number available" 	*/
				*pass_line_no = detail_call_no;
				return(DONE);
			}

			if (scr_name == VEHICLE_SRCH_SCR)
				unhilite_vh_line(detail_call_no); 	/*unhilite the last hilited line */
			else 	if (scr_name == DRIVER_SRCH_SCR)
					unhilite_dr_line(detail_call_no); 
			
			if (detail_call_no >= ((*calls_on_page) - 1))  
				detail_call_no = 0;
			else
				detail_call_no++;

			if (scr_name == VEHICLE_SRCH_SCR)
				hilite_vh_line(detail_call_no); 		/*hilite the line matched on summary area */
			else	if (scr_name == DRIVER_SRCH_SCR)
					hilite_dr_line(detail_call_no); 

			clear_vhdr_detail_area();
			if (scr_name == VEHICLE_SRCH_SCR)
				dsp_vh_detail(detail_call_no);
			else	if (scr_name == DRIVER_SRCH_SCR)
					dsp_dr_detail(detail_call_no);

			*pass_line_no = detail_call_no;
			return(DONE);

		case NEXT_PG_KEY: 				/* display summary of the next 17 calls */
			got_calls = 0;
			if ( (*fnptr_srches)(YES, &got_calls, FORWARD, NO) )  {  
				if (got_calls)
					*calls_on_page = got_calls;
				detail_call_no = NOT_AVAILABLE; 		/* erase message on detail area */
				clear_vhdr_detail_area();
				erase_area(6,17); 				/* erase_sum_area */
				if (scr_name == VEHICLE_SRCH_SCR)
					disp_vh_sum(*calls_on_page);
				else	if (scr_name == DRIVER_SRCH_SCR)
						disp_dr_sum(*calls_on_page);
			}
			else  {
				prt_error(UI_NO_RECORD,catgets(UI_m_catd, UI_1, UI_VH_DR_NO_1, "<in the direction you are going>"));
			}
			*pass_line_no = detail_call_no;
			return(DONE);

		case PREV_PG_KEY: 
			got_calls = 0;
			if (page_num > 1 && (*fnptr_srches)(YES, &got_calls, BACKWARD, NO) )  {  
				if (got_calls)
					*calls_on_page = got_calls;
				detail_call_no = NOT_AVAILABLE; 
				clear_vhdr_detail_area();
				erase_area(6,17);
				if (scr_name == VEHICLE_SRCH_SCR)
					disp_vh_sum(*calls_on_page);
				else	if (scr_name == DRIVER_SRCH_SCR)
						disp_dr_sum(*calls_on_page);
			}
			else  {
				prt_error(UI_NO_RECORD,catgets(UI_m_catd, UI_1, UI_VH_DR_NO_1, "<in the direction you are going>"));
			}
			*pass_line_no = detail_call_no;
			return(DONE);

		case REFRESH_KEY:	
			got_calls = 0;
			bottom_of_page = 1;
			if ( (*fnptr_srches)(YES, &got_calls, FORWARD, YES) )  {  
				if (got_calls)
					*calls_on_page = got_calls;
				detail_call_no = NOT_AVAILABLE; 
				clear_vhdr_detail_area();
				erase_area(6,17);
				if (scr_name == VEHICLE_SRCH_SCR)
					disp_vh_sum(*calls_on_page);
				else	if (scr_name == DRIVER_SRCH_SCR)
						disp_dr_sum(*calls_on_page);
			}
			else  {
				prt_error(UI_NO_RECORD,catgets(UI_m_catd, UI_1, UI_VH_DR_NO_1, "<in the direction you are going>"));
			}
			*pass_line_no = detail_call_no;
			return(DONE);

		case PREV_SCR_KEY:
		case COMMAND_KEY:
			detail_call_no = NOT_AVAILABLE;
			return(NOT_DONE);
	}
	return(NOT_DONE);
}


clear_vhdr_detail_area()
{
	int x;

	attrset(A_REVERSE | A_BOLD);

	mvaddstr(1,0, Spaces);
	mvaddstr(2,0, Spaces);
	mvaddstr(3,0, Spaces);
	mvaddstr(4,0, Spaces);

	attrset(A_REVERSE);
	mvaddstr(1,0,catgets(UI_m_catd, UI_1, UI_VH_DR_NO_2, " Sign-On  "));
	mvaddstr(2,0,catgets(UI_m_catd, UI_1, UI_VH_DR_NO_3, " Sign-Off "));
	mvaddstr(1,31,catgets(UI_m_catd, UI_1, UI_VH_DR_NO_4, " Emerg On  "));
	mvaddstr(2,31,catgets(UI_m_catd, UI_1, UI_VH_DR_NO_5, " Emerg Off "));
	mvaddstr(1,59,catgets(UI_m_catd, UI_1, UI_VH_DR_NO_6, " Suspended From "));
	mvaddstr(1,77,catgets(UI_m_catd, UI_1, UI_VH_DR_NO_7, " Suspended To   "));
	mvaddstr(2,58,"P");
	mvaddstr(1,96,"L");
	mvaddstr(2,96,"X");
	mvaddstr(3,0,catgets(UI_m_catd, UI_1, UI_VH_DR_NO_12, " Line:    "));
	mvaddstr(3,31," ");

	for(x = 0; x < NUM_SEALS; x++) {
		mvprintw(3, 32 + (x * 6), "%3s-", seal[x]);
	}

	attrset(0);
}
