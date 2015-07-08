static char sz__FILE__[]="@(#)vh_common.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  vh_common.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:48:39
 * @(#)  Last delta: 12/2/94 at 11:25:10
 * @(#)  SCCS File: /taxi/sccs/s.vh_common.c
 *                                                                           *
 *****************************************************************************/

#include "vh_m_strdefs.h"
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
#include "vh_maint.h"
#include "literals.h"
#include "tokenize.h"
#include "language.h"
#include "func_keys.h"

extern int do_jmp;
extern jmp_buf jmp_env;

extern int fleet_flag;
extern int doing_add;
extern int vh_flag;
extern int rf_flag;
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

extern struct scr_flds vh_scr_flds[];
extern struct cisam_vh *vh_ptr;	/* pointer to user profile file structure */
extern int MaxAttrs;

/*---------------------------------------------------------------------------
= process_input - input data fields.
---------------------------------------------------------------------------*/

process_input()
{
	int k = 0, i = 0;		/* loop index */
	int should_clear;
	char AttributesSet;
	int AttrCol = 0, AttrRow = 0;
	int findex;
	extern struct fleets *fleet[];

	while(1) {
		fld_desc(piu,&should_clear);	/* display the help message and set the erase flag */
		intr_allowed = 1;		/* interrupts allowed */

		/* Clear on dates and times */
		if (piu == F_SUSP_FM_1 ||
		    piu ==F_SUSP_TO_1  ||
		    piu == F_SUSP_D_FM_1 ||
		    piu ==F_SUSP_D_TO_1  ||
		    piu == F_SUSP_X_FM_1 ||
		    piu ==F_SUSP_X_TO_1  ||
		    piu == F_L_EMRG_1  ||
		    piu == F_E_RES_1   ||
		    piu == F_SGN_ON_1  ||
		    piu == F_SGN_OFF_1 ||
		    piu == F_SUSP_FM_2 ||
		    piu ==F_SUSP_TO_2  ||
		    piu == F_SUSP_D_FM_2 ||
		    piu ==F_SUSP_D_TO_2  ||
		    piu == F_SUSP_X_FM_2 ||
		    piu ==F_SUSP_X_TO_2  ||
		    piu == F_L_EMRG_2  ||
		    piu == F_E_RES_2   ||
		    piu == F_SGN_ON_2  ||
		    piu == F_SGN_OFF_2)

		    should_clear = 1;
		else
		    should_clear = 0;

		key_val = read_field(stdscr,vh_scr_flds,piu,read_buf,&field_entered,&should_clear); 

		if ( (key_val == ALTERNATE_KEY) && (piu == F_ATTR_1) )
		  {
		    attr_select( SELECT_VEHICLE_ATTR, vh_ptr->fleet_nbr, vh_ptr->attr );
		    mvaddstr( D_ATTR_ROW + 1, 0, "                                               " );
		    mvaddstr( D_ATTR_ROW + 2, 0, "                                               " );
		    AttrCol = 1;
		    AttrRow = 1;
		    for ( i = 0; i < MaxAttrs; i++ )
		      {
			if ( vh_ptr->attr[i] == YES )
			  {
			    if ( fleet[vh_ptr->fleet_nbr]->vd_attr_info[i].veh_attr[0] != '\0' )
			      {
				mvaddstr( D_ATTR_ROW + AttrRow, AttrCol, fleet[vh_ptr->fleet_nbr]->vd_attr_info[i].veh_attr );
				AttrCol += 3;
				if ( AttrCol >= L_PASS_COL - 2 )
				  {
				    AttrCol = 1;
				    AttrRow = 2;
				  }
				AttributesSet = YES;
			      }
			  }
		      }
#ifdef FOO
		    if ( AttributesSet == YES )
		      mvprintw(D_ATTR_ROW,D_ATTR_1_COL,"%c", YES );
		    else
		      mvprintw(D_ATTR_ROW,D_ATTR_1_COL,"%c", NO );
#endif
		  }

		intr_allowed = 0;		/* interrupts not allowed */
		if(saw_intr)  {
			clear_err_line();
			return;
		}

		/* validate input data, store it in c-isam structure, and adjust the field pointer */
		ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);

		if(doing_add) {
			if(ret_val == NOT_VALID && !fleet_flag) 
				continue;
			if(!fleet_flag)	{
				prt_error(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_7, "You must enter value"));
				piu = 1;
				continue;
			}
			if(k == 0) {
				++k;
				continue;
			}
			if(!vh_flag) {
				prt_error(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_8, "You must enter  a value [1 - 9999]"));
				if(key_val == KEY_UP) 
					piu = 1;
				else piu = 3;
				continue;
			}
			if(!rf_flag) {
				prt_error(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_9, "You must enter a value [1 - 3839]"));
				if(key_val != KEY_UP) 
					piu = 4;
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
			prt_error(catgets(VH_MAINT_m_catd, VH_MAINT_SET, 
					VH_MAINT_136, "Invalid entry, please enter a Y or N"));
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
				prt_error(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_141, 
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

/*-----------------------------------------------------------------------------
= fld_desc - display a description of the field being added/updated.
-----------------------------------------------------------------------------*/
fld_desc(piu,erase_field) 
int piu;
int *erase_field;	/* 0 = do not erase when reading, 1 = erase after first character input */
{
	clear_desc_line();
	move(DESC_ROW,DESC_COL);
	*erase_field=0;		/* set no-erase for completeness */
	switch(piu)  {
		case F_FLEET_1 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_10, "Fleet indicator"));
			break;
		case F_FLEET_2 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_11, "Fleet number corresponding to this vehicles fleet"));
			break;
		case F_VEH_NBR :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_12, "Vehicle number"));
			break;
		case F_RFID :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_13, "MID of the MDT terminal in this vehicle"));
			break;
		case F_LIC_NBR :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_14, "Vehicle license plate number"));
			break;
		case F_MAKE :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_15, "Vehicle make"));
			break;
		case F_MODEL :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_16, "Vehicle model"));
			break;
		case F_KDT_OK :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_17, "Y/N. Whether the MDT terminal is functional in this vehicle"));
			break;
		case F_MET_OK :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_18, "Y/N. Whether this vehicles meter is functional or not"));
			break;
			
		      case F_COLOR:
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_142, "Vehicle Color"));
			break;
		      case F_TELEPHONE:
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_143, "Telephone Number"));
			break;

	case F_MDT_TELE:
	  addstr("MDT's telephone number");
	  break;
			
		case F_SCHEDULE :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_133, "Schedule number (1..25) that this vehicle is assigned to, or (0) for unscheduled"));
			break;
		case F_SEAL_1 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_19, "Indicates that this vehicle has seal 1"));
			break;
		case F_SEAL_2 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_20, "Indicates that this vehicle has seal 2"));
			break;
		case F_SEAL_3 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_21, "Indicates that this vehicle has seal 3"));
			break;
		case F_SEAL_4 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_22, "Indicates that this vehicle has seal 4"));
			break;
		case F_SEAL_5 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_23, "Indicates that this vehicle has seal 5"));
			break;
		case F_SEAL_6 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_24, "Indicates that this vehicle has seal 6"));
			break;
		case F_SEAL_7 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_25, "Indicates that this vehicle has seal 7"));
			break;
		case F_SEAL_8 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_26, "Indicates that this vehicle has seal 8"));
			break;
		case F_SEAL_9 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_27, "Indicates that this vehicle has seal 9"));
			break;
		case F_SEAL_10 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_28, "Indicates that this vehicle has seal 10"));
			break;
		case F_SEAL_11 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_29, "Indicates that this vehicle has seal 11"));
			break;
		case F_SEAL_12 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_30, "Indicates that this vehicle has seal 12"));
			break;
		case F_SEAL_13 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_31, "Indicates that this vehicle has seal 13"));
			break;
		case F_SEAL_14 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_32, "Indicates that this vehicle has seal 14"));
			break;
		case F_SEAL_15 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_33, "Indicates that this vehicle has seal 15"));
			break;
		case F_SEAL_16 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_34, "Indicates that this vehicle has seal 16"));
			break;
		case F_TERM_TYPE :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_137, "1 - MDT with an ITM, 2 - MDT V03 with no ITM, 3 - MDT V02"));
			break;
		case F_NUM_MISS_RECEIPT :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_138, "Number of missing receipts"));
            break;
		case F_ATTR_1 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_35, "Indicates that this vehicle has attributes"));
			break;
#ifdef FOO
		case F_ATTR_2 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_36, "Indicates that this vehicle has attribute 2"));
			break;
		case F_ATTR_3 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_37, "Indicates that this vehicle has attribute 3"));
			break;
		case F_ATTR_4 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_38, "Indicates that this vehicle has attribute 4"));
			break;
		case F_ATTR_5 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_39, "Indicates that this vehicle has attribute 5"));
			break;
		case F_ATTR_6 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_40, "Indicates that this vehicle has attribute 6"));
			break;
		case F_ATTR_7 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_41, "Indicates that this vehicle has attribute 7"));
			break;
		case F_ATTR_8 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_42, "Indicates that this vehicle has attribute 8"));
			break;
#endif
		case F_SUSP_FM_1 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_43, "Date this vehicle was last suspended (MM/DD/YY)"));
			*erase_field=1;
			break;
		case F_SUSP_FM_2 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_44, "Time this vehicle was last suspended (hh:mm[a/p])"));
			*erase_field=1;
			break;
		case F_SUSP_TO_1 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_45, "Date this vehicle was last restored to service (MM/DD/YY)"));
			*erase_field=1;
			break;
		case F_SUSP_TO_2 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_46, "Time this vehicle was last restored to service (hh:mm[a/p])"));
			*erase_field=1;
			break;
		case F_SUSP_D_FM_1 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_43, "Date this vehicle was last suspended (MM/DD/YY)"));
			*erase_field=1;
			break;
		case F_SUSP_D_FM_2 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_44, "Time this vehicle was last suspended (hh:mm[a/p])"));
			*erase_field=1;
			break;
		case F_SUSP_D_TO_1 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_45, "Date this vehicle was last restored to service (MM/DD/YY)"));
			*erase_field=1;
			break;
		case F_SUSP_D_TO_2 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_46, "Time this vehicle was last restored to service (hh:mm[a/p])"));
			*erase_field=1;
			break;
		case F_SUSP_X_FM_1 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_43, "Date this vehicle was last suspended (MM/DD/YY)"));
			*erase_field=1;
			break;
		case F_SUSP_X_FM_2 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_44, "Time this vehicle was last suspended (hh:mm[a/p])"));
			*erase_field=1;
			break;
		case F_SUSP_X_TO_1 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_45, "Date this vehicle was last restored to service (MM/DD/YY)"));
			*erase_field=1;
			break;
		case F_SUSP_X_TO_2 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_46, "Time this vehicle was last restored to service (hh:mm[a/p])"));
			*erase_field=1;
			break;
		case F_SUSP_RSN :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_47, "Reason for vehicles suspension(internal)"));
			break;
		case F_PASS :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_48, "N not suspended, Y is suspended from servicing passenger trips"));
			break;
		case F_DEL :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_49, "N not suspended, Y is suspended from servicing delivery trips"));
			break;
		case F_XTYP :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_50, "N not suspended, Y is suspended from servicing type X trips"));
			break;
		case F_SP_MSG_1 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_51, "Suspension message to be sent to the vehicle"));
			break;
		case F_DRVR_CMNT :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_52, "Any comments relating to this vehicle"));
			break;
		case F_L_EMRG_1 : 
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_53, "Last date this vehicle invoked an emergency (MM/DD/YY)"));
			*erase_field=1;
			break;
		case F_L_EMRG_2 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_54, "Last time this vehicle invoked an emergency (hh:mm[a/p])"));
			*erase_field=1;
			break;
		case F_E_RES_1 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_55, "Date the last emergency was taken care of (MM/DD/YY)"));
			*erase_field=1;
			break;
		case F_E_RES_2 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_56, "Time the last emergency was taken care of (hh:mm[a/p])"));
			*erase_field=1;
			break;
		case F_SGN_ON_1 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_57, "Last date this vehicle signed on (MM/DD/YY)"));
			*erase_field=1;
			break;
		case F_SGN_ON_2 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_58, "Last time this vehicle signed on (hh:mm[a/p])"));
			*erase_field=1;
			break;
		case F_SGN_OFF_1 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_59, "Last date this vehicle signed off (MM/DD/YY)"));
			*erase_field=1;
			break;
		case F_SGN_OFF_2 :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_60, "Last time this vehicle signed off (hh:mm[a/p])"));
			*erase_field=1;
			break;
		case F_DRIVER :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_61, "Last driver of this vehicle"));
			break;
		case F_CL_SRV :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_62, "Last trip handled in this vehicle"));
			break;
		case F_POST :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_63, "Number of times this vehicle booked during the last shift"));
			break;
		case F_CPOST :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_64, "Number of times this vehicle issued conditional books during the last shift"));
			break;
		case F_EPOST :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_65, "Number of times this vehicle issued enroute books during the last shift"));
			break;
		case F_N_ACPT :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_66, "Number of times this vehicle issued no accepts during the last shift"));
			break;
		case F_REJCT :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_67, "Number of times this vehicle issued rejects during the last shift"));
			break;
		case F_C_OUT :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_68, "Number of callouts this vehicle had during the last shift"));
			break;
		case F_NSHOW :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_69, "Number of no shows this vehicle had during the last shift"));
			break;
		case F_FLAG :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_70, "Number of flags this vehicle had during the last shift"));
			break;
		case F_BID :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_71, "Number of bids this vehicle had during the last shift"));
			break;
		case F_TALK :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_72, "Number of requests to talk this vehicle issued during the last shift"));
			break;
		case F_MSG :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_73, "Number of messages this vehicle sent to the supervisor during the last shift"));
			break;
		case F_BREAK :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_74, "Number of breaks this vehicle took during the last shift"));
			break;
		case F_ASSIGNS :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_75, "Number of trips manually assigned to this vehicle during the last shift"));
			break;
		case F_CALLS :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_76, "Number of trips dispatched to this vehicle during the last shift"));
			break;
		case F_SH_MET :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_77, "Number of short meters attempted by this vehicle during the last shift"));
			break;
		case F_LT_MET :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_78, "Number of late meters that occured during this vehicle's last shift"));
			break;
		case F_CL_BACK :
			addstr(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_79, "Number of callbacks this vehicle had during the last shift"));
			break;
	}
	refresh();
}
