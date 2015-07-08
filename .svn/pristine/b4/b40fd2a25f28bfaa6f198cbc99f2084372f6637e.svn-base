/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: hol_maint.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1996/09/06 14:08:31 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/maint/hol_maint.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: hol_maint.c,v 1.2 1996/09/06 14:08:31 jwelch Exp $";


#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <isam.h>
#include <cursesX.h>
#include <signal.h>

#define	DECLARING
#include "comm_strdefs.h"
#include "holiday_per_m_strdefs.h"
#include "mad_error.h"
#include "mads_types.h"
#include "key.h"
#include "holiday_per_maint.h"
#include "literals.h"
#include "language.h"

/* Declare an array of pointers to functions which check the field entered for validity. */

extern int (*check_field[])();	/* Array of pointers to routines to validate the data input by the user */

extern struct scr_flds hol_scr_flds[];
extern int hol_cl_flds();
extern int read_field();
extern char buf[];		/* buffer used for interface to db routine */
extern char req_buf[];		/* buffer used for interface to db routine */
extern int iserrno;		/* C-isam error number */
extern int errno;		/* Error number returned from system calls */

char o_file[F_NAME_SIZE];	/* output file as specified by the user */
char mode[2];			/* user specifies Append or Create */

extern int prt_query_hdr(), prt_add_hdr();
extern int prt_scr(), process_intr();
extern void prt_hdr(), prt_labels();
extern char read_request(), prompt_confirm();
extern int prompt_fname(), prompt_mode();
extern int prt_data(), process_input(), send_ipc();
extern int prt_upd_hdr(), prt_error();
extern int clear_flds();
extern int clear_err_line(), clear_hdr_line();
extern int process_query(), process_next();
extern int process_previous(), process_add();
extern int process_update(), process_remove(), process_output();

int ret_val;			/* return value  */
int intr_allowed;		/* interrupts allowed */
int saw_intr;			/* indicates an interrupt signal was entered */
char intr_char;			/* interrupt character */
int end_read_field;		/* flag, 0/1, set to end read_field function */
short ipc_required;		/* Set if a field changed which is in shared memory */
char read_buf[BUFFER_SIZE];	/* buffer for reading fields */
int piu = 3;			/* field number to start reading input from */
int field_entered;		/* field entered or not */
int key_val;			/* value of key entered */
int cur_list;			/* Indicates a row is in the current list */
int record_nbr;			/* C-ISAM record number */
int err_n_fld;			/* set if an error in field condition exists */
int doing_add;			/* performing add operation */
int	file_open;
struct df_maint df_maint_ipc;
struct cisam_hl *pHoliday;
struct cisam_hl *pTempHoliday;
struct cisam_hl  Holiday_Buf;
char zero_date[9];

#define  HOLIDAY_OPEN   1


/*****************************************************************************
*  
*******************************************************************************/

main(argc,argv)
int argc;
char **argv;
{
	char request;		/*  User request : Q, N, P, A, U, R, O, or E */
	short  done = FALSE;

	/*
	 * Set up for internationalization by setting the locale and opening the catalog
	 */
	setlocale(LC_ALL, "");
	open_catalog(&COMMON_m_catd, "common", 0, 0);
	open_catalog(&HOL_MAINT_m_catd, "holiday_per_maint", 0, 0);
	open_catalog(&LITERAL_m_catd, "literals", 0, 0);

	file_open = 0;


	init();

	pTempHoliday = &Holiday_Buf;
	prt_hdr();
	prt_labels();

	/*  Get the first record from the vehicle file */

	if((db(HOLIDAY_FILE_ID,READ_KEYS,&hl_key1,ISFIRST,0)) < 0) 
		prt_error(catgets(HOL_MAINT_m_catd, 1, HOL_MAINT_4,
				  "No record found, ADD first !"));
	else  {
		prt_data();		/* Display the data from the first record */
		record_nbr = isrecnum; 	/* save record number for future read */
		cur_list = 1;		/* valid row in current list */
	}

	/*  Input the users request (Query, Next, Previous, Add, Update, Remove, Output, Exit).	*/

	 while(request = read_request())  {
		err_n_fld = 0;
		ipc_required = 0;
	    	switch(request)  {
			case M_QUERY:
				clear_err_line();
				process_query();
				break;

			    case M_NEXT:
				clear_err_line();
				process_next();
				break;

			case M_PREVIOUS:
				clear_err_line();
				process_previous();
				break;

			case M_ADD:
				clear_err_line();
				process_add();
				if ( ipc_required )
				  send_ipc();
				break;

			case M_UPDATE:
				clear_err_line();
				process_update();
				if ( ipc_required )
				  send_ipc();
				break;

			case M_REMOVE:
				clear_err_line();
				process_remove();
				if ( ipc_required )
				  send_ipc();
				break;

			case M_OUTPUT:
				clear_err_line();
				process_output();
				break;
		
			case M_EXIT:
				cleanup();
				break;

			case '\014':					/* ^L, refresh screen */
				wrefresh(curscr);
				break;

			default:
				cleanup();
				break;
		}

	}

	cleanup();
	catclose(COMMON_m_catd);
	catclose(HOL_MAINT_m_catd);
	catclose(LITERAL_m_catd);
}

/******************************************************************************
* init - open files, initialize variables, setup signals, prepare curses.
*******************************************************************************/

init()
{
	intr_allowed = 0;	/* interrupts not allowed */
	saw_intr = 0;		/* an interrupt signal has not been entered */
	cur_list = 0;		/* no row in current list */
	initscr();
	noecho();
/*
	keypad(stdscr,TRUE);
*/
        term_setup(getenv("TERM"));
        config_tty();


	cbreak();

	signal(SIGQUIT,SIG_IGN);
	signal(SIGINT,process_intr);
	signal(SIGTERM,cleanup);
	signal(SIGHUP,cleanup);
	signal(SIGPIPE,cleanup);
	signal(SIGTSTP, SIG_IGN);

	pHoliday = (struct cisam_hl *) req_buf;

	if((db(HOLIDAY_FILE_ID,OPEN,&hl_key1,ISINOUT+ISMANULOCK,0)) < 0)  {   
		prt_error(HOL_OPEN_ERR);
		sleep(3);
		cleanup();
	} else
		file_open |= HOLIDAY_OPEN;

	sprintf(zero_date, "00%c00%c00", DATE_SEPERATOR, DATE_SEPERATOR);
}

/******************************************************************************
* cleanup - if an error has occured, close files and terminate curses.
*******************************************************************************/

cleanup()
{
	if(file_open & HOLIDAY_OPEN)
		if((db(HOLIDAY_FILE_ID,CLOSE,&hl_key1,ISEQUAL,0)) < 0)  {
			prt_error(HOL_CLOSE_ERR);
			ERROR(' ',HOL_FILE,HOL_CLOSE_ERR);
		}

	nocbreak();
	echo();
	erase();
	refresh();
	endwin();

	exit(0);
}
