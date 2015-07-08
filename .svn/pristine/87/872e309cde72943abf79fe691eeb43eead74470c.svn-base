/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: res_per_sub1.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1996/09/06 14:09:11 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/maint/res_per_sub1.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: res_per_sub1.c,v 1.2 1996/09/06 14:09:11 jwelch Exp $";


#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <isam.h>
#include <cursesX.h>
#include <signal.h>
#include <memory.h>

#include "res_per_m_strdefs.h"
#include "mad_error.h"
#include "mads_types.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "df_maint.h"
#include "mads_isam.h"
#include "ex_key.h"
#include "res_per_maint.h"
#include "literals.h"
#include "tokenize.h"
#include "path.h"
#include "language.h"

extern int upper_case;
extern short skip_return;
extern int ret_val;				/* return value */
extern int piu;					/* prompt in use */
extern int intr_allowed;			/* interrupts allowed */
extern int key_val;				/* value returned from the check routines */
extern char read_buf[];				/* buffer for reading fields */
extern int field_entered;			/* field entered or not */
extern int saw_intr;				/* indicates an interrupt signal was entered */
extern int cur_list;				/* indicates a row is in the current list */
extern int (*check_field[])();			/* Array of pointers to routines to validate the data input by the user */
extern int record_nbr;				/* C_ISAM record number */
extern char o_file[];				/* output file as specified by the user */
extern char mode[];				/* user specified Append or Create */	
extern char req_buf[];
extern RP_REC *pResPer;
extern RP_REC *pTempResPer;
extern RP_REC  ResPer_Buf;
extern int clear_flds();			/* clear data fields on the screen */
extern int doing_add;				/* performing add operation */
extern int err_n_fld;				/* Indicates that an error exists in a field */
extern void prt_labels();
extern int clear_desc_line();
extern struct scr_flds res_per_scr_flds[];
extern struct df_maint df_maint_ipc;		/* structure for passing information to dispatch  */
extern short ipc_required;			/* Set if a variable which resides */
void prt_hdr();
extern char zero_date[];

/******************************************************************************
* process_query - search for specific record in the vehicle file
*******************************************************************************/
process_query()
{
	int i;				/* loop index */
	int should_clear;
	
	key_val = 0;
	clear_err_line();
	clear_flds();
	prt_query_hdr();		/* Print the header for the query screen */

					/*  Process users input of key fields(fleet and vehicle number).  */
	piu = F_PERIOD;
    	ret_val = NOT_VALID;
	while(key_val != ESC) {
		intr_allowed = 1;		/* interrupts allowed */
		if (piu == 1)
			skip_return = 1;
		else
			skip_return = 0;

		should_clear = 0;

		key_val = read_field(stdscr,res_per_scr_flds,piu,read_buf,&field_entered,&should_clear);
		intr_allowed = 0;		/* interrupts not allowed */

		if(saw_intr) {			/* key_value = interrupt char */
			saw_intr = 0;		/* interrup has been processed */
			cur_list = 0;		/* no row in current list */
			clear_flds();
			clear_err_line();
			prt_hdr();
			return;
		}

					/* validate input data, store it in c-isam structure, and adjust the field pointer */
		ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);
		if(ret_val == NOT_VALID)
			continue;	

		if(key_val == ESC) {		/* Using key entered in prev step, call C-Isam routine to retrieve the record. */
			clear_err_line();
			if(db(RP_FILE_ID, READ_KEYS, &rp_key1, ISGTEQ, 0) < 0) {
				if(iserrno == ENOREC) {
					prt_error(NO_ROWS);
					clear_flds();
					cur_list = 0;				/* row in current list is invalid */
					prt_hdr();
					return;
				}
				else  {
					prt_error(READ_RES_PER_ERR);
					sleep(3);
					ERROR(' ',RP_FILE,READ_RES_PER_ERR);
					cleanup();
				}
			}
			record_nbr = isrecnum;					/* save record number */
			cur_list = 1;						/* valid row in current list */
        		prt_data();  		
			prt_hdr();
		}
	}
	key_val = 0;
	return;
}

/******************************************************************************
* process_next - read the next record in the vehicle file and display it.
*******************************************************************************/
process_next()
{
	clear_err_line();
	if(!cur_list)  {
		prt_error(NO_CUR_LIST);
		return;
	}

									/*  Call the C-Isam routine to get the next entry.   */
	if(db(RP_FILE_ID, READ_KEYS, &rp_key1, ISNEXT, 0) < 0) {
		if(iserrno == EENDFILE) {
			prt_error(NO_ROWS_DIR);
			return;
		}
		else  {
			prt_error(RD_NXT_RES_PER_ERR);
			sleep(3);
			ERROR(' ',RP_FILE,RD_NXT_RES_PER_ERR);
			cleanup();
		}
	}
	cur_list = 1;
	record_nbr = isrecnum;					/* save the record number for future read */
	clear_flds();
        prt_data();				  		/* Print retrieved data on the screen. */

	return;
}

/******************************************************************************
* process_previous - read the previous record in the vehicle file and display it.
*******************************************************************************/
process_previous()
{
	clear_err_line();
	if(!cur_list) {
		prt_error(NO_CUR_LIST);
		return;
	}

									/*  Call the C-Isam routine to get the previous entry.  */
	if(db(RP_FILE_ID, READ_KEYS, &rp_key1, ISPREV, 0) < 0) {
		if(iserrno == EENDFILE) {
			prt_error(NO_ROWS_DIR);
			return;
		}
		else  {
			prt_error(RD_PREV_RES_PER_ERR);
			sleep(3);
			ERROR(' ',RP_FILE,RD_PREV_RES_PER_ERR);
			cleanup();
		}
	}
	cur_list = 1;
	record_nbr = isrecnum;						/* save the record number for future read */
	clear_flds();
        prt_data();					  		/* Print retrieved data on the screen. */
 	return;
}

/******************************************************************************
* process_add - input data fields and add the new record to the vehicle file 
*******************************************************************************/
process_add()
{
	char err[80];

	clear_err_line();
	clear_flds();

	memset((char *)pResPer, '\0', sizeof(RP_REC));

	piu = F_PERIOD;
	prt_add_hdr();					/* Print the header line for the the add screen.  */
	prt_labels();					/* print the screen labels  */
	prt_data();
	while( key_val != ESC ){
		doing_add = 1;
		process_input();				/* Process each field as input by the user */
		doing_add = 0;
		if(saw_intr) {
			saw_intr = 0;			 	/* interrupt has been processed */
			cur_list = 0;				/* no valid row in current list */
			clear_flds();
			clear_desc_line();
			prt_hdr();
			return;
		}
		memcpy((char *)pTempResPer,req_buf,sizeof(RP_REC));

	}							/*  Call the C-Isam add record routine.  */
	memcpy(req_buf,(char *)pTempResPer,sizeof(RP_REC));
	if(db(RP_FILE_ID, ADD_REC, &rp_key1, 0, 0) < 0) {
		prt_error(ADD_RES_PER_ERR);
		sprintf(err, "%s(%d)", ADD_RES_PER_ERR, iserrno);
		ERROR(' ',RP_FILE, err);
	}
	else
	{
	  /* After an ADD to db, no current record is defined.
	     Read the just added record to set the current record */
	  db(RP_FILE_ID, READ_KEYS, &rp_key1, ISEQUAL, 0);
	  ipc_required = 1;
	}

	cur_list = 1;					/* valid row in current list */
	prt_hdr();					/* Display the vehicle file maintenance screen header line  */
	clear_desc_line();
}

/******************************************************************************
* process_update - input data fields and update the vehicle file record
*******************************************************************************/
process_update()
{
	char err[80];

	if(db(RP_FILE_ID,READ_KEYS,&rp_key1,ISEQUAL,0) < 0) {
		prt_error(catgets(RES_PER_MAINT_m_catd, 1, RES_PER_MAINT_8, "Period file read error"));
		prt_hdr();
		return;
	}
	clear_err_line();
	if(!cur_list) {
		prt_error(NO_CUR_LIST);
		return;
	}

	prt_upd_hdr();					/* Print the header line for the update screen.  */
	while(1)		/*user cannot enter a duplicate RFID */
	{
		piu = F_START;					/* start with the license number */
		process_input();				/* Process each field as input by the user */
			/* If the user hit ESC while an error condition existed in a field, the error in field flag must be reset */
		if(key_val == ESC)
			err_n_fld = 0;

		if(saw_intr) {
			saw_intr = 0;					/* interrupt has been processed */
			cur_list = 0;					/* no valid row in current list */
			clear_flds();
			clear_desc_line();
			prt_hdr();
			return;
 		}
		memcpy((char *)pTempResPer,req_buf,sizeof(RP_REC));
		break	;
	}
	memcpy(req_buf,(char *)pTempResPer,sizeof(RP_REC));
	memcpy((char *)pTempResPer,req_buf,sizeof(RP_REC));
	if(db(RP_FILE_ID,UPDATE_REC,&rp_key1,0,0) < 0) {
		prt_error(UPD_RES_PER_ERR);
		sprintf(err, "%s(%d)", UPD_RES_PER_ERR, iserrno);
		ERROR(' ',RP_FILE, err);
	}
	else
	    prt_error(REC_UPDATED);

	if(db(RP_FILE_ID,RELEASE,&rp_key1,0,0) < 0) {
		prt_error(RLS_RES_PER_ERR);
		cur_list = 0;
		ERROR(' ',RP_FILE,RLS_RES_PER_ERR);
		prt_hdr();
		clear_desc_line();
		return;
	}

	cur_list = 1;						/* valid row in current list */
        prt_hdr();						/* Display the vehicle file maintenance screen header line */
	clear_desc_line();
	ipc_required = 1;
} 
/******************************************************************************
* process_remove - remove the current vehicle file record.
*******************************************************************************/
process_remove()
{
	int i;				/* loop index */
	char answer;			/* response from user */
	char err[80];

	clear_err_line();
	if(!cur_list)  {
		prt_error(NO_CUR_LIST);
		return;
	}

	answer = prompt_confirm();				/* make sure the user wants to remove this record */

	if(answer == NO)  {						/* user answered negatively */
		prt_hdr();
		return;
	}

					/*  If the user responded affirmatively, call C-Isam routine to delete the record.  */
	if(db(RP_FILE_ID,DELETE,&rp_key1,0,0) < 0) {
		prt_error(DEL_RES_PER_ERR);
		sprintf(err, "%s(%d)", DEL_RES_PER_ERR, iserrno);
		ERROR(' ',RP_FILE, err);
	}
	else
	  ipc_required = 1;

	cur_list = 0;			/* no row in current list */
	clear_flds();
	prt_hdr();
}

/******************************************************************************
* process_output - open an output file and call prt_scr.
*******************************************************************************/
process_output()
{
	FILE *o_file_fp;		/* output file pointer */
	FILE *fopen();

	intr_allowed = 1;		/* interrupts are allowed */
	clear_err_line();
	echo();
	prompt_fname();			/* prompt the user for the output file name */
	if(saw_intr)  {
		noecho();
		saw_intr = 0;		/* interrupt was processed */
		intr_allowed = 0; 	/* interrupts not allowed */
		prt_hdr();
		return;
	}

	prompt_mode();		/* Ask the user: Append to existing file or Create a new one?  */

	noecho();
	if(saw_intr)  {
		saw_intr = 0;		/* interrupt was processed */
		intr_allowed = 0; 	/* interrupts not allowed */
		prt_hdr();
		return;
	}

	if((o_file_fp = fopen(o_file,mode)) == NULL)  {
		prt_error(OPEN_OUT_ERR);
		ERROR(' ',RP_FILE,OPEN_OUT_ERR);
		prt_hdr();
		return;
	}

        prt_scr(o_file_fp);		/*  Write the screen information to the file.  */
	fclose(o_file_fp);
	prt_hdr();

	return;
}

void prt_labels()
{
  char           open_brace[2], close_brace[2];
  strcpy(open_brace, "(");
  strcpy(close_brace, ")");

  attrset( A_BOLD );
  mvaddstr( 4, D_PERIOD_COL - 2 , catgets( RES_PER_MAINT_m_catd, 1, RES_PER_MAINT_1, "Period" ) );
  mvaddstr( 4, D_START_COL, catgets( RES_PER_MAINT_m_catd, 1, RES_PER_MAINT_2, "Start" ) );
  mvaddstr( 4, D_END_COL, catgets( RES_PER_MAINT_m_catd, 1, RES_PER_MAINT_3, "End" ) );
  mvaddstr( 3, D_DAYS_COL - 1, catgets( RES_PER_MAINT_m_catd, 1, RES_PER_MAINT_4, "Active Days" ) );
  mvaddstr( 4, D_DAYS_COL, catgets( RES_PER_MAINT_m_catd, 1, RES_PER_MAINT_5, "SMTWTFSH" ) );
  mvaddstr( 4, D_FACTOR_COL - 2, catgets( RES_PER_MAINT_m_catd, 1, RES_PER_MAINT_6, "Lead Time Factor" ) );
  attrset(0);
  mvaddstr( 5, D_PERIOD_COL - 1, open_brace );
  mvaddstr( 5, D_PERIOD_COL + MC_PERIOD + 1, close_brace );
  mvaddstr( 5, D_START_COL - 1, open_brace );
  mvaddstr( 5, D_START_COL + MC_START + 1, close_brace );
  mvaddstr( 5, D_END_COL - 1, open_brace );
  mvaddstr( 5, D_END_COL + MC_END + 1, close_brace );
  mvaddstr( 5, D_DAYS_COL - 1, open_brace );
  mvaddstr( 5, D_DAYS_COL + MC_DAYS, close_brace );
  mvaddstr( 5, D_FACTOR_COL - 1, open_brace );
  mvaddstr( 5, D_FACTOR_COL + MC_FACTOR + 1, close_brace );
  refresh();
}

prt_data()
{
  int i;

  mvprintw( 5, D_PERIOD_COL, "%2d", pResPer->period_definition.period );
  mvprintw( 5, D_START_COL, "%4d", pResPer->period_definition.start_time );
  mvprintw( 5, D_END_COL, "%4d", pResPer->period_definition.end_time );
  mvprintw( 5, D_DAYS_COL, "%s", pResPer->days_of_week );
  mvprintw( 5, D_FACTOR_COL, "%2.2f", pResPer->period_definition.lead_time_skew );
  refresh();
  
  return;
}
	   
clear_flds()
{
	int i;		/* loop index */

	for(i = 1; i <= NUM_FLDS; i++) 
		res_per_cl_flds(i); 
	return;
}

/******************************************************************************
* prt_hdr - print the main screen header.
*******************************************************************************/

void prt_hdr()
{
	clear_hdr_line();
	mvaddstr(HDR_ROW_1, HDR_COL_1, "Query Next Previous Update Output Exit");
	attrset(A_BOLD);
	mvaddstr(HDR_ROW_2,HDR_COL_2,RES_PER_TITLE);
	attrset(0);
	refresh();
	
	return;
}

/******************************************************************************
* prompt_fname - prompt the user for a file name to output the screen image to.
*******************************************************************************/

prompt_fname()
{
	clear_hdr_line();
	mvaddstr(HDR_ROW_1,HDR_COL_1,RES_PER_FNAME_PROMPT);
	refresh();
	getstr(o_file);
	if(!strcmp(o_file,""))
		strcpy(o_file,catgets(RES_PER_MAINT_m_catd, 1, RES_PER_MAINT_7, "res_per_scr.out"));

	return;
} 


/******************************************************************************
* send_ipc - send a message to dispatch that a record in the user profile file changed.
*******************************************************************************/

send_ipc()
{
	int qid, key = MSGKEY;

	df_maint_ipc.d_prcs_id = DISP_DEST;
	df_maint_ipc.u_prcs_id = getpid();
	df_maint_ipc.rec_type = DFM_REQ;		
	df_maint_ipc.which_file = RP_FILE_ID;
	if((qid = msgget(key,0666 | IPC_CREAT)) == -1)   { 
		prt_error(RES_PER_GET_ERR); 
		sleep(3); 
		ERROR(' ',RP_FILE,RES_PER_GET_ERR);
		cleanup();
	}

	if((msgsnd(qid,&df_maint_ipc,sizeof(df_maint_ipc), IPC_NOWAIT)) == -1)  {
		prt_error(RES_PER_SND_ERR);
		sleep(3);
		ERROR(' ',RP_FILE,RES_PER_SND_ERR);
		cleanup();
	}
	return;
}

