static char sz__FILE__[]="@(#)fl_common.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fl_common.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:47:32
 * @(#)  Last delta: 12/2/94 at 12:10:08
 * @(#)  SCCS File: /taxi/sccs/s.fl_common.c
 *                                                                           *
 *****************************************************************************/
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <isam.h>
#include <curses.h>
#include <signal.h>
#include <setjmp.h>

#include "fl_m_strdefs.h"
#include "mad_error.h"
#include "mads_types.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "mads_isam.h"
#include "ex_key.h"
#include "fl_maint.h"
#include "literals.h"
#include "fl_scr.h"
#include "fl_scr_ary.h"
#include "tokenize.h"
#include "language.h"

extern int do_jmp;
extern jmp_buf jmp_env;

extern int err_n_fld;
extern int ret_val;		/* return value */
extern int piu;			/* prompt in use */
extern int intr_allowed;	/* interrupts allowed */
extern int key_val;		/* value returned from the check routines */
extern char read_buf[];		/* buffer for reading fields */
int end_read_field;		/* flag, 0/1, set to end read_field function */
extern int field_entered;	/* field entered or not */
extern int saw_intr;		/* indicates an interrupt signal was entered */
extern int cur_list;		/* indicates a row is in the current list */

extern int upper_case;
extern int record_nbr;		/* C_ISAM record number */
extern char o_file[];		/* output file as specified by the user */
extern char mode[];		/* user specified Append or Create */	
extern int fl_flag;
extern struct scr_flds fl_scr_flds[];

void fld_desc();			/* field description routine the data input by the user */
void prt_error(char* err_msg);
void clear_hdr_line();
void clear_err_line();

/******************************************************************************
* process_input - input data fields.
******************************************************************************/

void process_input()
{
	int i;		/* loop index */
	int should_clear = 0;

#ifdef DEBUG
	sprintf(trace_str,"Entering process_input\n");
	TRACE(trace_str);
#endif

	while(1)   {
		fld_desc(piu);
		intr_allowed = 1;		/* interrupts allowed */
		if ((piu >= 89 && piu <= 93) ||
		    piu == F_AU_TL_PT)
			upper_case = 0;
		else
		    upper_case = 1;
		key_val = read_field(stdscr,fl_scr_flds,piu,read_buf,&field_entered,&should_clear); 
		attrset(0);
		intr_allowed = 0;		/* interrupts not allowed */
		if(saw_intr)  {
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_input - 1\n");
	TRACE(trace_str);
#endif
			return;
		}

		/* validate input data, store it in c-isam structure, and
		   adjust the field pointer */

		ret_val = (fl_scr_flds[piu].val_rtn)(&piu,read_buf,key_val,&field_entered);

		if((key_val == ESC) && (!err_n_fld))
			break;
	}
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_input - 2\n");
	TRACE(trace_str);
#endif

	return;
}  /*  End of process_input()  */

/******************************************************************************
* prt_scr - read the screen and output characters to a file.
*******************************************************************************/

void prt_scr(o_file_fp)
FILE *o_file_fp;
{

	int col, line, c;

#ifdef DEBUG
	sprintf(trace_str,"Entering prt_scr\n");
	TRACE(trace_str);
#endif

	col = 0;
	for ( line = 0; line < LINES ; line++)  {
		for ( col = 0; col < COLS ; col++ )  {
			move(line,col);
			c = inch();
			fputc(c,o_file_fp);	
		}
		if(c != '\n')
			fputc('\n',o_file_fp);
	}
#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_scr\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_scr() */

/******************************************************************************
* read_request - read users request for options.
*******************************************************************************/

char read_request()
{
	/*  The main header, the labels and the data from the first
	 *  record in the user profile file have been displayed.  At this
	 *  point the user must request one of the eight options,
	 *  they are : Q, q, N, n, P, p, U, u, R, s, O, o, E, 
	 *  and e.
	 */

	int c;		/* For reading characters */

#ifdef DEBUG
	sprintf(trace_str,"Entering read_request\n");
	TRACE(trace_str);
#endif

	move(REQ_ROW,REQ_COL);
	refresh();

	while(c = getkey())  {
	        c = Toupper(c);
		if (c == M_QUERY ||
		    c == M_NEXT ||
		    c == M_PREVIOUS ||
		    c == M_UPDATE ||
		    c == M_SCREEN ||
		    c == M_OUTPUT ||
		    c == M_EXIT)
		    break;
		else  {
			move(REQ_ROW,REQ_COL);
			refresh();
		}
	}

#ifdef DEBUG
	sprintf(trace_str,"Leaving read_request\n");
	TRACE(trace_str);
#endif

	return(c);

} /*  End of read_request() */

/******************************************************************************
* prt_query_hdr - print the header line for the query option.
*******************************************************************************/

void prt_query_hdr()
{
	int i;		/* loop index */

#ifdef DEBUG
	sprintf(trace_str,"Entering prt_qurey_hdr\n");
	TRACE(trace_str);
#endif
	clear_hdr_line();

	/* Print the Query screen header */

	mvprintw(HDR_ROW_1,HDR_COL_1,FLEET_QUERY_HDR);
	refresh();

	clear_flds();

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_qurey_hdr\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_query_hdr()  */

/******************************************************************************
* prt_add_hdr - print the header line for the add option.
*******************************************************************************/

void prt_add_hdr()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_add_hdr\n");
	TRACE(trace_str);
#endif

	clear_hdr_line();
	mvaddstr(HDR_ROW_1,HDR_COL_1,FLEET_ADD_HDR);
	refresh();
	clear_flds();

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_add_hdr\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of prt_add_hdr() */

/******************************************************************************
* prt_upd_hdr - print the header line for the update option.
*******************************************************************************/

void prt_upd_hdr()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_upd_hdr\n");
	TRACE(trace_str);
#endif
	clear_hdr_line();
	mvaddstr(HDR_ROW_1,HDR_COL_1,UPDATE_HDR);
	refresh();

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_upd_hdr\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_upd_hdr() */

/******************************************************************************
* prompt_confirm - confirm that the user wants to remove this record.
*******************************************************************************/

char prompt_confirm()
{
	int c;		/* For reading characters */

#ifdef DEBUG
	sprintf(trace_str,"Entering prompt_confirm\n");
	TRACE(trace_str);
#endif

	clear_hdr_line();
	mvaddstr(DEL_ROW,DEL_COL,DELETE_HDR);
	refresh();

	while(c = getkey())  {
	    c = Toupper(c);
		if(c == YES || c == NO)
			break;
		else  {
			move(DEL_ROW,DEL_COL);
			refresh();
		}
	}

#ifdef DEBUG
	sprintf(trace_str,"Leaving prompt_confirm\n");
	TRACE(trace_str);
#endif

	return(c);

}  /*  End of prompt_confirm()  */

/******************************************************************************
* prompt_mode - prompt the user for file mode Create or Append.
*******************************************************************************/

void prompt_mode()
{
	char		answer;
	short		have_valid_response;

#ifdef DEBUG
	sprintf(trace_str,"Entering prompt_mode\n");
	TRACE(trace_str);
#endif

	have_valid_response = 0;
	while(!have_valid_response)
	  {
		clear_hdr_line();
		mvprintw(HDR_ROW_1,HDR_COL_1,FMODE_PROMPT,o_file);

		getstr(mode);
/*
 *      default to create
 */
		if (!strlen(mode))
			mode[0] = M_CREATE;

		answer = Toupper(mode[0]);

		switch (answer)
	  	  {
			case M_APPEND:
				strcpy(mode,"a");
				have_valid_response = 1;
				break;
			case M_CREATE:
				strcpy(mode,"w");
				have_valid_response = 1;
				break;
			default:
				prt_error(catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_39, 
					"Invalid choice - Please enter (A/C) append or create"));
				break;
	  	  }
	  }


	clear_err_line();


#ifdef DEBUG
	sprintf(trace_str,"Leaving prompt_mode\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prompt_mode()  */

/******************************************************************************
* prt_error - print error message.
*******************************************************************************/

void prt_error(err_msg)
char *err_msg;			/* error message to be printed */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_error\n");
	TRACE(trace_str);
#endif

	clear_err_line();
	beep();

	/* Set reverse video attributes */

	attrset(A_REVERSE);

	mvprintw(ERROR_ROW,ERROR_COL," %s ",err_msg);

	refresh();

	/* Reset video attributes */

	attrset(0);

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_error\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_error()  */

/******************************************************************************
* prt_cisam_err - write the cisam error message to the trace file.
*******************************************************************************/

void prt_cisam_err(err_nbr,err_msg)
int err_nbr;			/* C-isam error number associated with this problem */
char *err_msg;			/* Error message associated with this problem */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_cisam_err\n");
	TRACE(trace_str);
#endif

	sprintf(trace_str,"%s, ",err_msg);
	TRACE(trace_str);
	sprintf(trace_str,"%s %d\n", catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_2, "C-isam Error Number ="), iserrno);
	TRACE(trace_str);

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_cisam_err\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_cisam_err()  */

/******************************************************************************
* prt_unix_err - write the unix error message to the trace file.
*******************************************************************************/

void prt_unix_err(u_err,u_err_msg)
int u_err;			/* unix error number associated with this problem */
char *u_err_msg;		/* Error message associated with this problem */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_unix_err\n");
	TRACE(trace_str);
#endif

	sprintf(trace_str,"%s, ",u_err_msg);
	TRACE(trace_str);
	sprintf(trace_str,"%s %d\n", catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_3, "Unix Error Number ="), u_err);
	TRACE(trace_str);

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_unix_err\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_unix_err()  */

/******************************************************************************
* clear_err_line - clear the error line on the screen.
*******************************************************************************/

void clear_err_line()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering clear_err_line\n");
	TRACE(trace_str);
#endif
	move(ERROR_ROW,ERROR_COL);
	clrtoeol();
	refresh();  

#ifdef DEBUG
	sprintf(trace_str,"Leaving clear_err_line\n");
	TRACE(trace_str);
#endif
	return;
}

/******************************************************************************
* clear_desc_line - clear the field description line on the screen.
*******************************************************************************/

void clear_desc_line()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering clear_desc_line\n");
	TRACE(trace_str);
#endif
	move(DESC_ROW,DESC_COL);
	clrtoeol();
	refresh();  

#ifdef DEBUG
	sprintf(trace_str,"Leaving clear_desc_line\n");
	TRACE(trace_str);
#endif
	return;
}


/******************************************************************************
* clear_hdr_line - clear the header line on the screen.
*******************************************************************************/

void clear_hdr_line()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering clear_hdr_line\n");
	TRACE(trace_str);
#endif
	move(HDR_ROW_1,HDR_COL_1);
	clrtoeol();
	refresh();

#ifdef DEBUG
	sprintf(trace_str,"Leaving clear_hdr_line\n");
	TRACE(trace_str);
#endif
	return;
}

/******************************************************************************
* process_intr - process the interrupt signal.
*******************************************************************************/

void process_intr(int sig_num)
{
	/* This routine is called whcn an interrupt signal
	   is received.  A flag is set to indicate that this
	   signal was received.  The flag end_read_field is set
	   so the read_field routine will terminate. */

#ifdef DEBUG
	sprintf(trace_str,"Entering process_intr\n");
	TRACE(trace_str);
#endif
	signal(SIGINT,process_intr);
	if(intr_allowed)  {
		saw_intr = 1;
		end_read_field = 1;
	}

        if ( do_jmp ) {
                do_jmp = 0;
                longjmp(jmp_env, 1);
        }
 

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_intr\n");
	TRACE(trace_str);
#endif
	return;
}

/******************************************************************************
* fld_desc - display a description of the field being added/updated.
******************************************************************************/

void fld_desc(piu)
int piu;
{
#ifdef DEBUG
	sprintf(trace_str,"Entering fld_desc \n");
	TRACE(trace_str);
#endif
	clear_desc_line();
	move(DESC_ROW,DESC_COL);
	printw("%s",fl_scr_flds[piu].help);

} 
