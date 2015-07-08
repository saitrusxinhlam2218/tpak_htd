/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: res_per_common.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1996/09/06 14:08:56 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/maint/res_per_common.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: res_per_common.c,v 1.2 1996/09/06 14:08:56 jwelch Exp $";

#include "res_per_m_strdefs.h"
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <isam.h>
#include <cursesX.h>
#include <signal.h>
#include <setjmp.h>
#include "mad_error.h"
#include "mads_types.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "mads_isam.h"
#include "ex_key.h"
#include "res_per_maint.h"
#include "literals.h"
#include "tokenize.h"
#include "language.h"

extern int do_jmp;
extern jmp_buf jmp_env;

extern int doing_add;
extern int ret_val;		/* return value */
extern int piu;			/* prompt in use */
extern int err_n_fld;		/* Set if an error exists in a field */
extern int intr_allowed;	/* interrupts allowed */
extern int key_val;		/* value returned from the check routines */
extern char read_buf[];		/* buffer for reading fields */
int end_read_field;		/* flag, 0/1, set to end read_field function */
extern int field_entered;	/* field entered or not */
extern int saw_intr;		/* indicates an interrupt signal was entered */
extern int cur_list;		/* indicates a row is in the current list */
extern int (*check_field[])();	/* Array of pointers to routines to validate
				   the data input by the user */
extern int record_nbr;		/* C_ISAM record number */
extern char o_file[];		/* output file as specified by the user */
extern char mode[];		/* user specified Append or Create */	
extern int upper_case;

extern struct scr_flds res_per_scr_flds[];
extern RP_REC *pResPer;

/*---------------------------------------------------------------------------
= process_input - input data fields.
---------------------------------------------------------------------------*/

process_input()
{
	int k = 0;		/* loop index */
	int should_clear;

	while(1) {
		fld_desc(piu,&should_clear);	/* display the help message and set the erase flag */
		intr_allowed = 1;		/* interrupts allowed */

		if (piu == F_FACTOR)
		  should_clear = 1;
		else
		  should_clear = 0;
		
		key_val = read_field(stdscr,res_per_scr_flds,piu,read_buf,&field_entered,&should_clear); 

		intr_allowed = 0;		/* interrupts not allowed */
		if(saw_intr)  {
			clear_err_line();
			return;
		}

		/* validate input data, store it in c-isam structure, and adjust the field pointer */
		ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);

		if(doing_add) {
			if(ret_val == NOT_VALID )
				continue;
			if(k == 0) {
				++k;
				continue;
			}
		}

		if(key_val == ESC && !err_n_fld) 
			break;
	}
	return;
}

/*-----------------------------------------------------------------------------
= prt_scr - read the screen and output characters to a file.
-----------------------------------------------------------------------------*/

prt_scr(o_file_fp)
FILE *o_file_fp;
{
	int col, line, c;

	col = 0;
	for(line = 0; line < LINES; line++) {
		for(col = 0; col < COLS; col++) {
			move(line,col);
			c = inch();
			fputc(c,o_file_fp);	
		}
		if(c != '\n')
			fputc('\n',o_file_fp);
	}
	return;
}

/*-----------------------------------------------------------------------------
= read_request - read users request for options.
-----------------------------------------------------------------------------*/

char 
read_request()
{
	/*  The main header, the labels and the data from the first record in the user profile file have been displayed.  At this
	 *  point the user must request one of the eight options, they are : Q, q, N, n, P, p, A, a, U, u, R, r, O, o, E, and e.
	 */

	int c;		/* For reading characters */

	move(REQ_ROW,REQ_COL);
	refresh();

	while(c = getkey())  {
	    c = Toupper(c);
	    if (c == M_QUERY ||
		c == M_NEXT ||
		c == M_PREVIOUS ||
		c == M_ADD ||
		c == M_UPDATE ||
		c == M_REMOVE ||
		c == M_OUTPUT ||
		c == M_EXIT)
			break;
		else  {
			move(REQ_ROW,REQ_COL);
			refresh();
		}
	}

	return(c);
}

/*------------------------------------------------------------------------------
= prt_query_hdr - print the header line for the query option.
------------------------------------------------------------------------------*/

prt_query_hdr()
{
	int i;		/* loop index */

	clear_hdr_line();

	/* Print the Query screen header */

	mvprintw(HDR_ROW_1,HDR_COL_1,QUERY_HDR);
	refresh();

	clear_flds();

	return;
} 

/*-----------------------------------------------------------------------------
= prt_add_hdr - print the header line for the add option.
------------------------------------------------------------------------------*/
prt_add_hdr()
{
	clear_hdr_line();
	mvaddstr(HDR_ROW_1,HDR_COL_1,ADD_HDR);
	refresh();
	clear_flds();

	return;
}

/*------------------------------------------------------------------------------
= prt_upd_hdr - print the header line for the update option.
------------------------------------------------------------------------------*/
prt_upd_hdr()
{
	clear_hdr_line();
	mvaddstr(HDR_ROW_1,HDR_COL_1,UPDATE_HDR);
	refresh();

	return;
}

/******************************************************************************
* prompt_confirm - confirm that the user wants to remove this record.
*******************************************************************************/
char 
prompt_confirm()
{
	int c;		/* For reading characters */

	clear_hdr_line();
	mvaddstr(DEL_ROW,DEL_COL,DELETE_HDR);
	refresh();

	while(c = getkey())  {
	    c = Toupper(c);
		if(c == YES || c == NO)
		  {
			clear_err_line();
			break;
		  }
		else  {
			prt_error(catgets(RES_PER_MAINT_m_catd, 1, 
					3, "Invalid entry, please enter a Y or N"));
			mvaddstr(DEL_ROW,DEL_COL,DELETE_HDR);
			refresh();
		}
	}

	return(c);
}

/******************************************************************************
* prompt_mode - prompt the user for file mode Create or Append.
*******************************************************************************/

prompt_mode()
{
	char		answer;
	short		have_valid_response;

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
				prt_error(catgets(RES_PER_MAINT_m_catd, 1, 10, 
					"Invalid choice - Please enter (A/C) append or create"));
				break;
	  	  }
	  }


	clear_err_line();

	return;

}  /*  End of prompt_mode()  */

/******************************************************************************
* prt_error - print error message.
*******************************************************************************/
prt_error(err_msg)
char *err_msg;			/* error message to be printed */
{
	clear_err_line();

	/* Set reverse video attributes */

	attrset(A_REVERSE);

	mvprintw(ERROR_ROW,ERROR_COL," %s ",err_msg);
	refresh();

	/* Reset video attributes */

	attrset(0);

	beep();

	return;
}

/******************************************************************************
* clear_err_line - clear the error line on the screen.
*******************************************************************************/
clear_err_line()
{
	move(ERROR_ROW,ERROR_COL);
	clrtoeol();
	refresh();  

	return;
}

/******************************************************************************
* clear_hdr_line - clear the header line on the screen.
*******************************************************************************/
clear_hdr_line()
{
	move(HDR_ROW_1,HDR_COL_1);
	clrtoeol();
	refresh();

	return;
}

/******************************************************************************
* process_intr - process the interrupt signal.
*******************************************************************************/
process_intr()
{
	/* This routine is called whcn an interrupt signal is received.  A flag is set to indicate that this
	   signal was received.  The flag end_read_field is set so the read_field routine will terminate. */

	signal(SIGINT,process_intr);
	if(intr_allowed)  {
		saw_intr = 1;
		end_read_field = 1;
	}

        if ( do_jmp ) {
                do_jmp = 0;
                longjmp(jmp_env, 1);
        }
 
	return;
}

/******************************************************************************
* clear_desc_line - clear the field description line on the screen.
*******************************************************************************/
clear_desc_line()
{
	move(DESC_ROW,DESC_COL);
	clrtoeol();
	refresh();  

	return;
}

fld_desc( piu, erase_field )
     int piu;
     int *erase_field;
{
  clear_desc_line();
  move( DESC_ROW, DESC_COL );
  *erase_field = 0;
  switch( piu )
    {
    case F_PERIOD:
      addstr( catgets( RES_PER_MAINT_m_catd, 1, RES_PER_MAINT_10, "Period Number" ) );
      break;
    case F_START:
      addstr( catgets( RES_PER_MAINT_m_catd, 1, RES_PER_MAINT_11, "Period Start Time" ) );
      break;
    case F_END:
      addstr( catgets( RES_PER_MAINT_m_catd, 1, RES_PER_MAINT_12, "Period End Time" ) );
      break;
    case F_DAYS:
      addstr( catgets( RES_PER_MAINT_m_catd, 1, RES_PER_MAINT_13, "Active Days" ) );
      break;
    case F_FACTOR:
      addstr( catgets( RES_PER_MAINT_m_catd, 1, RES_PER_MAINT_14, "Time Calls Lead Time Factor" ));
      break;
    }
  refresh();
}

