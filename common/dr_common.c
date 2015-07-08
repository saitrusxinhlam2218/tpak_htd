static char sz__FILE__[]="@(#)dr_common.c        10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  dr_common.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:32:42                      *
* @(#)  Last delta: 12/2/94 at 11:48:29                                               *
* @(#)  SCCS File: /taxi/sccs/s.dr_common.c                                                       *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS                                          *
*                                                                           *
*****************************************************************************/

#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <isam.h>
#include <cursesX.h>
#include <signal.h>
#include <setjmp.h>

#include "dr_m_strdefs.h"
#include "mad_error.h"
#include "mads_types.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "mads_isam.h"
#include "ex_key.h"
#include "dr_maint.h"
#include "tokenize.h"
#include "literals.h"
#include "language.h"

extern int do_jmp;
extern jmp_buf jmp_env;

extern int doing_add;
extern int upper_case;
extern int ret_val;		/* return value */
extern int piu;			/* prompt in use */
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
extern int fleet_flag;
extern int id_flag;
extern int err_n_fld;

int fld_desc();			/* field description routine */
int clear_desc();		/* routine to clear the field description line */

extern struct scr_flds dr_scr_flds[];
extern struct cisam_dr *dr_ptr;	/* pointer to user driver file structure */

/******************************************************************************
* process_input - input data fields.
******************************************************************************/

process_input()
{
int  k = 0;
int should_clear = 1;

#ifdef DEBUG
	sprintf(trace_str,"Entering process_input\n");
	TRACE(trace_str);
#endif

	while(1)   {
		fld_desc(piu);
		intr_allowed = 1;		/* interrupts allowed */

		/* Clear on dates and times */
		if (piu == F_SP_FM_DATE       ||
		    piu == F_SP_TO_DATE       ||
		    piu == F_SP_D_FM_DATE     ||
		    piu == F_SP_D_TO_DATE     ||
		    piu == F_SP_X_FM_DATE     ||
		    piu == F_SP_X_TO_DATE     ||
		    piu == F_EMRG_ON_DATE     ||
		    piu == F_EMRG_OFF_DATE    ||
		    piu == F_SGN_ON_DATE      ||
		    piu == F_SGN_OFF_DATE     ||
		    piu == F_SP_FM_TIME       ||
		    piu == F_SP_TO_TIME       ||
		    piu == F_SP_D_FM_TIME     ||
		    piu == F_SP_D_TO_TIME     ||
		    piu == F_SP_X_FM_TIME     ||
		    piu == F_SP_X_TO_TIME     ||
			piu == F_LIC_RNW_DATE     ||
			piu == F_TXI_LIC_RNW_DATE ||
		    piu == F_EMRG_ON_TIME     ||
		    piu == F_EMRG_OFF_TIME    ||
		    piu == F_SGN_ON_TIME      ||
		    piu == F_SGN_OFF_TIME)

		    should_clear = 1;
		else
		    should_clear = 0;

		key_val = read_field(stdscr,dr_scr_flds,piu,read_buf,&field_entered,&should_clear); 
		intr_allowed = 0;		/* interrupts not allowed */
		if(saw_intr)  {
			clear_err_line();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_input - 1\n");
	TRACE(trace_str);
#endif
			return;
		}

		/* validate input data, store it in c-isam structure, and
		   adjust the field pointer */

		ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);

	if (doing_add)	{
		if((ret_val == NOT_VALID) &&(!fleet_flag))	
			continue;
		if (!fleet_flag)	{
			prt_error(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_7, "You must enter value"));
			piu = 1;
			continue;
		}
		if (k == 0)	{
			++k;
			continue;
		}

			if (!id_flag)	{
				prt_error(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_7, "You must enter value"));
				if (key_val != KEY_UP)
					piu = 2;
				continue;
			}
	}
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

prt_scr(o_file_fp)
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
	 *  they are : Q, q, N, n, P, p, A, a, U, u, R, r, O, o, E, and e.
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

#ifdef DEBUG
	sprintf(trace_str,"Leaving read_request\n");
	TRACE(trace_str);
#endif

	return(c);

} /*  End of read_request() */

/******************************************************************************
* prt_query_hdr - print the header line for the query option.
*******************************************************************************/

prt_query_hdr()
{
	int i;		/* loop index */

#ifdef DEBUG
	sprintf(trace_str,"Entering prt_qurey_hdr\n");
	TRACE(trace_str);
#endif
	clear_hdr_line();

	/* Print the Query screen header */

	mvprintw(HDR_ROW_1,HDR_COL_1, QUERY_HDR);
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

prt_add_hdr()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_add_hdr\n");
	TRACE(trace_str);
#endif

	clear_hdr_line();
	mvaddstr(HDR_ROW_1,HDR_COL_1,ADD_HDR);
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

prt_upd_hdr()
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
		if (c == YES || c == NO)
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
* prompt mode - prompt the user for file mode Create or Append.
*******************************************************************************/

prompt_mode()
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
				prt_error(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_141, 
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

prt_error(err_msg)
char *err_msg;			/* error message to be printed */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_error\n");
	TRACE(trace_str);
#endif

	clear_err_line();

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
	beep();

	return;

}  /*  End of prt_error()  */

/******************************************************************************
* prt_cisam_err - write the cisam error message to the trace file.
*******************************************************************************/

prt_cisam_err(err_nbr,err_msg)
int err_nbr;			/* C-isam error number associated with this problem */
char *err_msg;			/* Error message associated with this problem */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_cisam_err\n");
	TRACE(trace_str);
	sprintf(trace_str,"%s, ",err_msg);
	TRACE(trace_str);
	sprintf(trace_str,"C-isam Error Number = %d\n",iserrno);
	TRACE(trace_str);
	sprintf(trace_str,"Leaving prt_cisam_err\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_cisam_err()  */

/******************************************************************************
* prt_unix_err - write the unix error message to the trace file.
*******************************************************************************/

prt_unix_err(u_err,u_err_msg)
int u_err;			/* unix error number associated with this problem */
char *u_err_msg;		/* Error message associated with this problem */
{
#ifdef DEBUG
	sprintf(trace_str,"%s, ",u_err_msg);
	TRACE(trace_str);
	sprintf(trace_str,"Unix Error Number = %d\n",u_err);
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_unix_err()  */

/******************************************************************************
* clear_err_line - clear the error line on the screen.
*******************************************************************************/

clear_err_line()
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
* clear_hdr_line - clear the header line on the screen.
*******************************************************************************/

clear_hdr_line()
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

process_intr()
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
* clear_desc_line - clear the field description line on the screen.
*******************************************************************************/

clear_desc_line()
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
* fld_desc - display a description of the field being added/updated.
******************************************************************************/

fld_desc(piu)
int piu;
{
#ifdef DEBUG
	sprintf(trace_str,"Entering fld_desc \n");
	TRACE(trace_str);
#endif
	clear_desc_line();
	move(DESC_ROW,DESC_COL);
	switch(piu)  {
		case F_FLEET :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_8, "Fleet indicator"));
			break;
		case F_DRVR_ID :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_9, "Driver ID (1-99999)"));
			break;
		case F_CLASS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_10, "Driver Classification 1-Regular, 2-Rookie"));
			break;
		case F_NAME :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_11, "Driver's name"));
			break;
		case F_ADDRESS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_12, "Driver's home address"));
			break;
		case F_CITY :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_13, "Driver's home city"));
			break;
		case F_ZIP :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_14, "Driver's home zip code"));
			break;
		case F_Z_SFX :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_15, "Driver's home zip code suffix"));
			break;
		case F_PHONE :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_16, "Driver's home phone"));
			break;
		case F_LIC :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_17, "Driver's license number"));
			break;
		case F_TXI_LIC :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_18, "Driver's taxi license number"));
			break;
		case F_SEAL_1 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_19, "Flag indicating driver has seal 1"));
			break;
		case F_SEAL_2 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_20, "Flag indicating driver has seal 2"));
			break;
		case F_SEAL_3 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_21, "Flag indicating driver has seal 3"));
			break;
		case F_SEAL_4 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_22, "Flag indicating driver has seal 4"));
			break;
		case F_SEAL_5 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_23, "Flag indicating driver has seal 5"));
			break;
		case F_SEAL_6 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_24, "Flag indicating driver has seal 6"));
			break;
		case F_SEAL_7 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_25, "Flag indicating driver has seal 7"));
			break;
		case F_SEAL_8 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_26, "Flag indicating driver has seal 8"));
			break;
		case F_SEAL_9 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_27, "Flag indicating driver has seal 9"));
			break;
		case F_SEAL_10 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_28, "Flag indicating driver has seal 10"));
			break;
		case F_SEAL_11 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_29, "Flag indicating driver has seal 11"));
			break;
		case F_SEAL_12 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_30, "Flag indicating driver has seal 12"));
			break;
		case F_SEAL_13 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_31, "Flag indicating driver has seal 13"));
			break;
		case F_SEAL_14 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_32, "Flag indicating driver has seal 14"));
			break;
		case F_SEAL_15 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_33, "Flag indicating driver has seal 15"));
			break;
		case F_SEAL_16 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_34, "Flag indicating driver has seal 16"));
			break;
		case F_ATTR_1 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_35, "Flag indicating driver has attribute 1"));
			break;
		case F_ATTR_2 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_36, "Flag indicating driver has attribute 2"));
			break;
		case F_ATTR_3 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_37, "Flag indicating driver has attribute 3"));
			break;
		case F_ATTR_4 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_38, "Flag indicating driver has attribute 4"));
			break;
		case F_ATTR_5 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_39, "Flag indicating driver has attribute 5"));
			break;
		case F_ATTR_6 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_40, "Flag indicating driver has attribute 6"));
			break;
		case F_ATTR_7 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_41, "Flag indicating driver has attribute 7"));
			break;
		case F_ATTR_8 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_42, "Flag indicating driver has attribute 8"));
			break;
		case F_SP_FM_DATE :
		case F_SP_D_FM_DATE :
		case F_SP_X_FM_DATE :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_43, "Date this driver was suspended"));
			break;
		case F_SP_FM_TIME :
		case F_SP_D_FM_TIME :
		case F_SP_X_FM_TIME :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_44, "Time this driver was suspended"));
			break;
		case F_SP_TO_DATE :
		case F_SP_D_TO_DATE :
		case F_SP_X_TO_DATE :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_45, "Date this driver will be restored to service"));
			break;
		case F_SP_TO_TIME :
		case F_SP_D_TO_TIME :
		case F_SP_X_TO_TIME :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_46, "Time this driver will be restored to service"));
			break;
		case F_SP_REASON :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_47, "Reason for driver suspension"));
			break;
		case F_SP_PASS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_48, "Whether this driver is restricted from servicing passenger calls"));
			break;
		case F_SP_DEL :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_49, "Whether this driver is restricted from servicing delivery calls"));
			break;
		case F_SP_TPX :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_50, "Whether this driver is restricted from servicing typex calls"));
			break;
		case F_MSG_1 :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_51, "Suspension message to be sent to the driver"));
			break;
		case F_COMMENT :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_52, "General driver comment area"));
			break;
		case F_LIC_RNW_DATE :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_53, "Driver's license renewal date"));
			break;
		case F_TXI_LIC_RNW_DATE :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_54, "Driver's taxi license renewal date"));
			break;
		case F_EMRG_ON_DATE :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_55, "Last date this driver invoked an emergency"));
			break;
		case F_EMRG_ON_TIME :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_56, "Last time this driver invoked an emergency"));
			break;
		case F_EMRG_OFF_DATE :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_57, "Last date this driver's last emergency was taken care of"));
			break;
		case F_EMRG_OFF_TIME :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_58, "Last time this driver's last emergency was taken care of"));
			break;
		case F_SGN_ON_DATE :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_59, "Last date this driver signed on"));
			break;
		case F_SGN_ON_TIME :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_60, "Last time this driver signed on"));
			break;
		case F_SGN_OFF_DATE :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_61, "Last date this driver signed off"));
			break;
		case F_SGN_OFF_TIME :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_62, "Last time this driver signed off"));
			break;
		case F_LST_VEH :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_63, "Last vehicle this driver drove"));
			break;
		case F_LST_CALL :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_64, "Last call handled by this driver"));
			break;
		case F_POSTS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_65, "Number of times this driver booked-in during his/her last shift"));
			break;
		case F_COND_POSTS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_66, "Number of times this driver issued conditional books during his/her last shift"));
			break;
		case F_ENR_POSTS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_67, "Number of times this driver issued enroute books during his/her last shift"));
			break;
		case F_NO_ACCEPTS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_68, "Number of times this driver issued no accepts during his/her last shift"));
			break;
		case F_REJECTS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_69, "Number of times this driver issued rejects during his/her last shift"));
			break;
		case F_CL_OUTS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_70, "Number of callouts this driver had during his/her last shift"));
			break;
		case F_NO_SHOWS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_71, "Number of no trips this driver had during his/her last shift"));
			break;
		case F_FLAGS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_72, "Number of flags this driver had during his/her last shift"));
			break;
		case F_BIDS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_73, "Number of bids this driver had during his/her last shift"));
			break;
		case F_RQ_TO_TALK :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_74, "Number of requests to talk this driver issued during his/her last shift"));
			break;
		case F_MSGS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_75, "Number of messages this driver sent to the supervisor during his/her last shift"));
			break;
		case F_BREAKS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_76, "Number of breaks this driver took during his/her last shift"));
			break;
		case F_ASSIGNS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_77, "Number of calls manually assigned to this driver during his/her last shift"));
			break;
		case F_CALLS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_78, "Number of calls dispatched to this driver during his/her last shift"));
			break;
		case F_SH_METERS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_79, "Number of short meters attempted by this driver during his/her last shift"));
			break;
		case F_LT_METERS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_80, "Number of late meters that occured during this driver's last shift"));
			break;
		case F_CL_BACKS :
			addstr(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_81, "Number of callbacks this driver had during his/her last shift"));
			break;
	}
	refresh();

#ifdef DEBUG
	sprintf(trace_str,"Leaving fld_desc \n");
	TRACE(trace_str);
#endif
}  /* End of fld_desc()  */
