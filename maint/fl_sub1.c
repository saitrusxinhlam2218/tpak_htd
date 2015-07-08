static char sz__FILE__[]="@(#)fl_sub1.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fl_sub1.c; Rel: 6.10.0.0; Get date: 4/14/92 at 16:11:46
 * @(#)  Last delta: 1/16/92 at 16:06:32
 * @(#)  SCCS File: /taxi/sccs/s.fl_sub1.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <curses.h>
#include <signal.h>
#include <memory.h>

#include "fl_m_strdefs.h"
#include "mad_error.h"
#include "mads_types.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "df_maint.h"
#include "mads_isam.h"
#include "ex_key.h"
#include "fl_maint.h"
#include "literals.h"
#include "fl_scr.h"
#include "path.h"
#include "tokenize.h"
#include "language.h"


#define	PRINTW(x, y)		mvprintw(fl_scr_flds[x].prompt_row,fl_scr_flds[x].prompt_col,fl_scr_flds[x].fmt_str, y)

extern int upper_case;
extern int ret_val;		/* return value */
extern int piu;			/* prompt in use */
extern int intr_allowed;	/* interrupts allowed */
extern int key_val;		/* value returned from the check routines */
extern char read_buf[];		/* buffer for reading fields */
extern char req_buf[];		
extern int field_entered;	/* field entered or not */
extern int saw_intr;		/* indicates an interrupt signal was entered */
extern int cur_list;		/* indicates a row is in the current list */
extern int record_nbr;		/* C_ISAM record number */
extern char o_file[];		/* output file as specified by the user */
extern char mode[];		/* user specified Append or Create */	
extern struct cisam_fl *fl_ptr; /* pointer to user fleet file structure */
extern struct cisam_fl *temp_fl_ptr; 
extern struct cisam_fl temp_fl_buf; 
extern int doing_add;		/* performing add operation */
extern int fl_flag;
extern int scr_nbr;		/* screen number */
extern void prt_labels();
extern short ipc_required;	/* Set if a variable which resides in 
				   shared memory changes */
extern int clear_desc_line();	/* Clear the field description line on the screen */
extern struct scr_flds fl_scr_flds[];
extern struct df_maint df_maint_ipc;	/* structure for passing information */
extern int AddUpdateDeleteAllowed();

void prt_hdr();
void prt_labels();
void prt_data();
void prompt_fname();
void clear_flds();

/******************************************************************************
* process_query - search for specific record in the driver file
*******************************************************************************/

void process_query()
{
	int i;				/* loop index */
	int should_clear = 0;

#ifdef DEBUG
	sprintf(trace_str,"Entering process_query\n");
	TRACE(trace_str);
#endif

	clear_err_line();
	if(scr_nbr == 2)  {
		clear();
		scr_nbr = 1;
		prt_labels();
	}
	else
		clear_flds();

	prt_query_hdr();		/* Print the header for the query 
					   screen */

	/*  Process users input of key field(fleet id ).  */

    	ret_val = NOT_VALID;
	while(ret_val == NOT_VALID)  {
		piu = F_ID;
		intr_allowed = 1;		/* interrupts allowed */
		key_val = read_field(stdscr, fl_scr_flds,piu,read_buf,&field_entered,&should_clear);
		attrset(0);
		/*intr_allowed = 0;	*/ 	/* interrupts not allowed */

		if(saw_intr)  {			/* key_value = interrupt char */
			saw_intr = 0;		/* interrup has been processed */
			cur_list = 0;		/* no row in current list */
			clear_flds();
			fl_flag = 0;
			clear_err_line();
			prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_query - 1\n");
	TRACE(trace_str);
#endif
			return;
		}

		/* validate input data, store it in c-isam structure, and
		   adjust the field pointer */

		ret_val = (fl_scr_flds[F_ID].val_rtn)(&piu,read_buf,key_val,&field_entered);
		if(ret_val == NOT_VALID)
			continue;

		if ((fl_flag) && (key_val != ESC))	{
			prt_error(catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_5, "Enter ESC to execute"));
			ret_val = NOT_VALID;
			continue;
		}
		if (key_val == ESC)	{
			clear_err_line();
			if (!fl_flag)
				fl_ptr->id = 'A';
		/*  Using the key entered in previous step, call C-Isam routine
	    	    to retrieve specific record.  */

			if((db(FLEET_FILE_ID,READ_KEYS,&fl_key1,ISGTEQ,0)) < 0)  {
				if(iserrno == ENOREC)  {
					prt_error(NO_ROWS);
					clear_flds();
					cur_list = 0;	/* row in current list is invalid */
					prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_query - 2\n");
	TRACE(trace_str);
#endif
					return;
				}
				else  {
					prt_error(READ_FLEET_ERR); 
					sleep(3);
					prt_cisam_err(iserrno,READ_FLEET_ERR);
					ERROR(' ',FLEET_FILE,READ_FLEET_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_query - 3\n");
	TRACE(trace_str);
#endif
					cleanup();
				}
			}
			record_nbr = isrecnum;	/* save record number */
			cur_list = 1;		/* valid row in current list */
        		prt_data();  		
			prt_hdr();
			return;
		}
			cur_list = 0;
			prt_hdr();
	}

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_query - 4\n");
	TRACE(trace_str);
#endif
	fl_flag = 0;
	return;

}  /* End of process_query()  */

/******************************************************************************
* process_next - read the next record in the fleet file and display it.
*******************************************************************************/

void process_next()
{

#ifdef DEBUG
	sprintf(trace_str,"Entering process_next\n");
	TRACE(trace_str);
#endif

	clear_err_line();
	if(!cur_list)  {
		prt_error(NO_CUR_LIST);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_next - 1\n");
	TRACE(trace_str);
#endif
		return;
	}

	/*  Call the C-Isam routine to get the next entry.   */

	if((db(FLEET_FILE_ID,READ_KEYS,&fl_key1,ISNEXT,0)) < 0)  {
		if(iserrno == EENDFILE)  {
			prt_error(NO_ROWS_DIR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_next - 2\n");
	TRACE(trace_str);
#endif
			return;
		}
		else  {
			prt_error(RD_NXT_FLEET_ERR);
			sleep(3);
			prt_cisam_err(iserrno,RD_NXT_FLEET_ERR);
			ERROR(' ',FLEET_FILE,RD_NXT_FLEET_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_next - 3\n");
	TRACE(trace_str);
#endif
			cleanup();
		}
	}
	cur_list = 1;
	record_nbr = isrecnum;	/* save the record number for future read */
	clear_flds();
        prt_data();  		/* Print retrieved data on the screen. */

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_next - 4\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of process_next()  */

/******************************************************************************
* process_previous - read the previous record in the driver file and display it.
*******************************************************************************/

void process_previous()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering process_previous\n");
	TRACE(trace_str);
#endif

	clear_err_line();
	if(!cur_list)  {
		prt_error(NO_CUR_LIST);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_previous - 1\n");
	TRACE(trace_str);
#endif
		return;
	}

	/*  Call the C-Isam routine to get the previous entry.  */

	if((db(FLEET_FILE_ID,READ_KEYS,&fl_key1,ISPREV,0)) < 0)  {
		if(iserrno == EENDFILE)  {
			prt_error(NO_ROWS_DIR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_previous - 2\n");
	TRACE(trace_str);
#endif
			return;
		}
		else  {
			prt_error(RD_PREV_FLEET_ERR);
			sleep(3);
			prt_cisam_err(iserrno,RD_PREV_FLEET_ERR);
			ERROR(' ',FLEET_FILE,RD_PREV_FLEET_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_previous - 3\n");
	TRACE(trace_str);
#endif
			cleanup();
		}
	}
	cur_list = 1;
	record_nbr = isrecnum;	/* save the record number for future read */
	clear_flds();
        prt_data();  		/* Print retrieved data on the screen. */

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_previous - 4\n");
	TRACE(trace_str);
#endif
 	return;

}  /* End of process_previous()  */

/******************************************************************************
* process_add - input data fields and add the new record to the driver file 
*******************************************************************************/

void process_add()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering process_add\n");
	TRACE(trace_str); 
#endif
	clear_err_line();
	if(scr_nbr == 2)  {
		clear();
		scr_nbr = 1;
		prt_labels();
	}
	else
		clear_flds();

	/* reset all the members of the fleet file structure */

	memset((char *)fl_ptr,'\0',sizeof(struct cisam_fl));

	piu = F_ID;		/* start with the fleet field */
	scr_nbr = 1;		/* on the first screen */
	prt_add_hdr();		/* Print the header line for the the add screen.  */
	prt_labels();		/* print the screen labels  */

	doing_add = 1;
	process_input();	/* Process each field as input by the user */
	doing_add = 0;
	if(saw_intr)  {
		saw_intr = 0; 	/* interrupt has been processed */
		cur_list = 0;	/* no valid row in current list */
		clear_flds();
		clear_desc_line();
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_add - 1\n");
	TRACE(trace_str);
#endif
		return;
	}

	/*  Call the C-Isam add record routine.  */

	ipc_required = 1;
	if((db(FLEET_FILE_ID,ADD_REC,&fl_key1,0,0)) < 0)  {
		prt_error(ADD_FLEET_ERR);
		prt_cisam_err(iserrno,ADD_FLEET_ERR);
		ERROR(' ',FLEET_FILE,ADD_FLEET_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_add - 2\n");
	TRACE(trace_str);
#endif
		return;
	}

	cur_list = 1;		/* valid row in current list */
	prt_hdr();		/* Display the vehicle file maintenance screen header line  */
	clear_desc_line();

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_add - 3\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of process_add()  */

/******************************************************************************
* process_update - input data fields and update the driver file record
*******************************************************************************/

void process_update()
{
        char err[80];

#ifdef DEBUG
	sprintf(trace_str,"Entering process_update\n");
	TRACE(trace_str);
#endif
	if((db(FLEET_FILE_ID,READ_KEYS,&fl_key1,ISEQUAL,0)) < 0) {
		prt_error(catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_6, "Fleet file read error"));
		prt_hdr();
		return;
	}


	clear_err_line();
	if(!cur_list)  {
		prt_error(NO_CUR_LIST);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_update - 2\n");
	TRACE(trace_str);
#endif
		return;
	}

	if(scr_nbr == 2)  {
		clear();
		scr_nbr = 1;
		prt_labels();
		prt_data();
	}

	prt_upd_hdr();		/* Print the header line for the update screen.  */

	piu = F_ACT;		/* start with the active field */
	process_input();	/* Process each field as input by the user */
	if(saw_intr)  {
		saw_intr = 0;	/* interrupt has been processed */
		cur_list = 0;	/* no valid row in current list */
		clear_flds();
		clear_desc_line();
		fl_flag = 0;
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_update - 3\n");
	TRACE(trace_str);
#endif
		return;
	}
	memcpy((char *)temp_fl_ptr,req_buf,sizeof(struct cisam_fl));
	if((db(FLEET_FILE_ID,READ_KEYS,&fl_key1,ISEQUAL+ISLOCK,0)) < 0) {
		prt_error(LOCK_FLEET_ERR);
		sprintf(err, "%s(%d)", LOCK_FLEET_ERR, iserrno);
		ERROR(' ',FLEET_FILE, err);
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_update - 1\n");
	TRACE(trace_str);
#endif
		return;
	}

	memcpy(req_buf,(char *)temp_fl_ptr,sizeof(struct cisam_fl));
	if((db(FLEET_FILE_ID,UPDATE_REC,&fl_key1,0,0)) < 0)  {
		prt_error(UPD_FLEET_ERR);
		sprintf(err, "%s(%d)", UPD_FLEET_ERR, iserrno);
		ERROR(' ',FLEET_FILE, err);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_update - 4\n");
	TRACE(trace_str);
#endif
		return;
	}

	prt_error(REC_UPDATED);
	if((db(FLEET_FILE_ID,RELEASE,&fl_key1,0,0)) < 0)  {
		prt_error(RLS_FLEET_ERR);
		cur_list = 0;
		sprintf(err, "%s(%d)", RLS_FLEET_ERR, iserrno);
		ERROR(' ',FLEET_FILE, err);
		prt_hdr();
		clear_desc_line();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_update - 5\n");
	TRACE(trace_str);
#endif
		return;
	}

	ipc_required = 1;
	cur_list = 1;		/* valid row in current list */
        prt_hdr();		/* Display the vehicle file maintenance screen header line */
	clear_desc_line();

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_update - 6\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of process_update()  */

/******************************************************************************
* process_remove - remove the current driver file record.
*******************************************************************************/

void process_remove()
{

	int i;				/* loop index */
	char answer;			/* response from user */

#ifdef DEBUG
	sprintf(trace_str,"Entering process_remove\n");
	TRACE(trace_str);
#endif

	clear_err_line();
	if(!cur_list)  {
		prt_error(NO_CUR_LIST);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_remove - 1\n");
	TRACE(trace_str);
#endif
		return;
	}

	answer = prompt_confirm();	/* make sure the user wants to remove this record */

	if(answer == NO)  {			/* user answered negatively */
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_remove - 2\n");
	TRACE(trace_str);
#endif
		return;
	}

	/*  If the user responded affirmatively, call C-Isam routine 
	    to delete the record.  */

	if((db(FLEET_FILE_ID,DELETE,&fl_key1,0,0)) < 0)  {
		prt_error(DEL_FLEET_ERR);
		prt_cisam_err(iserrno,DEL_FLEET_ERR);
		ERROR(' ',FLEET_FILE,DEL_FLEET_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_remove - 3\n");
	TRACE(trace_str);
#endif
		return;
	}

	cur_list = 0;		/* no ro in current list */
	clear_flds();
	prt_hdr();

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_remove - 4\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of process_remove()  */

/******************************************************************************
* process_output - open an output file and call prt_scr.
*******************************************************************************/

void process_output()
{

	FILE *o_file_fp;		/* output file pointer */
	FILE *fopen();

#ifdef DEBUG
	sprintf(trace_str,"Entering process_output\n");
	TRACE(trace_str);
#endif

	intr_allowed = 1;		/* interrupts are allowed */
	clear_err_line();
	echo();
	prompt_fname();			/* prompt the user for the output file name */
	if(saw_intr)  {
		noecho();
		saw_intr = 0;		/* interrupt was processed */
		intr_allowed = 0; 	/* interrupts not allowed */
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_output - 1\n");
	TRACE(trace_str);
#endif
		return;
	}

	prompt_mode();		/* Ask the user: Append to existing file or Create a
					   new one?  */

	noecho();
	if(saw_intr)  {
		saw_intr = 0;		/* interrupt was processed */
		intr_allowed = 0; 	/* interrupts not allowed */
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_output - 2\n");
	TRACE(trace_str);
#endif
		return;
	}

	if((o_file_fp = fopen(o_file,mode)) == NULL)  {
		prt_error(OPEN_OUT_ERR);
		ERROR(' ',FLEET_FILE,OPEN_OUT_ERR);
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_output - 3\n");
	TRACE(trace_str);
#endif
		return;
	}

        prt_scr(o_file_fp);		/*  Write the screen information to the file.  */
	fclose(o_file_fp);
	prt_hdr();

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_output - 4\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of process_output() */

/******************************************************************************
* prt_labels - print the field labels on the screen.
*******************************************************************************/

void prt_labels()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_labels\n");
	TRACE(trace_str);
#endif

	/* print the screen labels  */

	if(scr_nbr == 1)  {
attrset(A_BOLD);
mvaddstr(2,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_7,
"ID   #    Active    Name                                                  Lev1 Primary Lev2 Lev3 Lev4 Lev5 Lev6 Lev7 Lev8"));
attrset(0);
mvaddstr(3,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_8,
"[ ]  [ ]  [ ]       [                    ]               Reject Penalties [ ]  [ ]     [ ]  [ ]  [ ]  [ ]  [ ]  [ ]  [ ]"));
mvaddstr(4,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_9,
"Comments    [                                          ] Exclusive Time   [  ] [  ]    [  ] [  ] [  ] [  ] [  ] [  ] [  ]"));
mvaddstr(5,1,
"");
attrset(A_BOLD);
mvaddstr(6,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_10,
"         Frequency       Priority         Action          Duration         Penalty             Info                  Maximum"));
attrset(0);
mvaddstr(7,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_11,
"  BOOK      [     ]   NTRIP   [ ]     NTRIP  [ ]    ACPT    [     ]   No ACPT [ ]       Trip Offer   [ ]  REJCT Count    [   ]"));
mvaddstr(8,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_12,
"  CBOOK     [     ]   CANCEL  [ ]     CANCEL [ ]    BREAK   [     ]   No ACPT [     ]                     Driver Attr    [ ]"));
mvaddstr(8, 89, catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_122,
                                                                                        "Trip Assign  [ ]"));
#ifdef SCANDINAVIA
mvaddstr(9,1,  catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_13, 
"  TRIP QRY  [     ]   Regular [ ]     EBOOK  [ ]    Short M [     ]   NAK Ofr [ ]                         Vehicle Attr   [ ]"));
#else
mvaddstr(9,1,  
"  TRIP QRY  [     ]   Regular [ ]     EBOOK  [ ]    Short M [     ]   NAK Ofr [ ]       Send Dest    [ ]  Vehicle Attr   [  ]");
#endif
mvaddstr(10,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_14,
"  Zone Info [     ]   Local   [ ]    M Action[ ]    M Action[     ]          Lead Time          Allowed   Vehs/Call      [   ]"));
mvaddstr(11,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_15,
"  QP        [     ]   Rookie  [  ]                  Local   [     ]      EBOOK   [     ]   EBOOK     [ ]  Time Call Entry[  ]"));
mvaddstr(12,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_16,
"  Zone Stat [     ]                              Call Match [     ]   Time Calls [     ]   Personals [ ]  Pers. Lead Time[     ]"));
mvaddstr(13,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_17,
"  Time Call [     ]   Response to Call Offer [ ] CNX to MOFF[     ]   Late Meter [     ]                  Time Call Proc [     ]"));
mvaddstr(14,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_18,
"                                                Short NoTrip  [   ]   Wakeup     [     ]                  Time Call Intvl[  ]"));
mvaddstr(15,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_41,
"                                                                                                          Lead Supplement[  ]"));
attrset(A_BOLD);
mvaddstr(16,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_19,
"              Features                     Features            Default Pri.       Default Pri.    Man Meter    Rate Calc."));
attrset(0);
mvaddstr(17,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_20,
"  Check Seals   [ ]     Stand Flag Processing [ ]      Charge Call [  ]     'P'assenger [  ]        [ ]           [ ] "));
mvaddstr(18,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_21,
"  Hold Line     [ ]     Zone Backup Level     [ ]      Subsc. Call [  ]     'D'elivery  [  ]        [ ]           [ ]"));
mvaddstr(19,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_22,
"  Greeting Line [ ]     Driver Msg Approval   [ ]      Time Call   [  ]     'X'type     [  ]        [ ]"));
mvaddstr(20,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_23,
"  GPS state     [ ]     Subsc. Call Review    [ ]                           'W'ake-up   [  ]"));
mvaddstr(21,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_40,
"  GPS time    [   ]     Auto Flag Time      [   ]"));
#ifdef NAMERICA
mvaddstr(22,1,
"Send Pckup Cmnt [ ]     Send Rate             [ ]      GPS Time    [   ]");
#endif

	}
	if(scr_nbr == 2)  {
attrset(A_BOLD);
mvaddstr(2,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_24,
"                       System Information                         Report Freq.           "));
attrset(0);
mvaddstr(3,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_25,
" Audit Trail Port    [                    ]       Time Calls Report    [     ]           "));
mvaddstr(4,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_26,
"                                                  Messages Report      [     ]           "));
mvaddstr(5,1,
"                                                                                                                            ");
mvaddstr(6,1,
"                                                                                                                            ");
mvaddstr(7,1,
"                                                                                                                            ");
mvaddstr(8,1,
"                                                                                                                            ");
attrset(A_BOLD);
mvaddstr(9,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_27,
"              Defaults                              High Range  Est. Text                                                    "));
attrset(0);
mvaddstr(10,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_28,
"City Code         [   ]             1st Estimate        [     ]   [       ]                                                  "));
mvaddstr(11,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_29,
"Area Code         [   ]             2nd Estimate        [     ]   [       ]                                                  "));
mvaddstr(12,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_30,
"Call Type         [ ]               3rd Estimate        [     ]   [       ]                                                  "));
mvaddstr(13,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_31,
"Hist. Search      [     ]           4th Estimate        [     ]   [       ]                                                  "));
mvaddstr(14,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_32,
"Match Algorithm   [ ]               5th Estimate        [     ]   [       ]                                                  "));
mvaddstr(15,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_33,
"GPS Max X         [          ]      6th Estimate        [     ]   [       ]                                                  "));
mvaddstr(16,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_34,
"GPS Min X         [          ]      7th Estimate        [     ]   [       ]                                                  "));
mvaddstr(17,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_35,
"GPS Max Y         [          ]      8th Estimate        [     ]   [       ]                                                   "));
mvaddstr(18,1, catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_36,
"GPS Min Y         [          ]      Default                       [       ]                                                   "));
mvaddstr(19,1,
"GPS Coord System  ( )");
mvaddstr(20,1,
"GPS Distance      (     )");
mvaddstr(21,1,
"GPS Auto Booking  ( )");
 mvaddstr(3, 92,                                                                           "Interlink Active            ( )");
 mvaddstr(4, 92,                                                                           "Interlink Timeout           (   )");
mvaddstr(10, 92, catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_119,                "Inactive Retry              [   ]"));
mvaddstr(11, 92, catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_114,                "Inactive Timeout            [   ]"));
mvaddstr(12, 92, catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_115,                "Approach Tariff             [ ]"));
mvaddstr(13, 92, catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_116,                "Levels for Estimation       [ ]"));
mvaddstr(14, 92, catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_117,                "Frequency for Estimation    [   ]"));
mvaddstr(15, 92, catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_118,                "System Calls                [ ]"));
#ifdef SCANDINAVIA
mvaddstr(16, 92, catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_128,                "Flat Rate Distance Filter   [ ]"));
#endif
mvaddstr(17, 92, catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_133,                "Send Call Type              [ ]"));
mvaddstr(18, 92, catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_134,                "Auto Reject Time            [   ]"));
 mvaddstr(19, 92, catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_145,               "Emergency Range             [     ]"));
 mvaddstr(20, 92, catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_146,               "Enroute Break               [ ]"));
 mvaddstr(21, 92,                                                                          "Load-Unload                 ( )");
	}
	refresh();
	
#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_labels\n");
	TRACE(trace_str);
#endif

}  /* End of prt_labels  */

/******************************************************************************
* prt_data - print the data on the screen.
*******************************************************************************/

void prt_data()
{

	int i;			/* loop index */
	struct tm *tm_ptr;      /* pointer to a unix time structure */
	extern	struct scr_flds fl_scr_flds[];

#ifdef DEBUG
	sprintf(trace_str,"Entering prt_data\n");
	TRACE(trace_str);
#endif

	/* Fill in the data items on the screen */

	if(scr_nbr == 1)  {
		PRINTW(F_ID, fl_ptr->id);
		PRINTW(F_NBR, fl_ptr->nbr);
		PRINTW(F_ACT, fl_ptr->active);
		PRINTW(F_NAME, fl_ptr->name);
		PRINTW(F_COMMENT, fl_ptr->comments);
		PRINTW(F_REJ_LVL_1, fl_ptr->lev1_rej_penalt); 
		PRINTW(F_REJ_PRM, fl_ptr->main_rej_penalt); 
		PRINTW(F_REJ_LVL_2, fl_ptr->lev2_rej_penalt); 
		PRINTW(F_REJ_LVL_3,fl_ptr->lev3_rej_penalt);
		PRINTW(F_REJ_LVL_4,fl_ptr->lev4_rej_penalt);
		PRINTW(F_REJ_LVL_5,fl_ptr->lev5_rej_penalt);
		PRINTW(F_REJ_LVL_6,fl_ptr->lev6_rej_penalt);
		PRINTW(F_REJ_LVL_7,fl_ptr->lev7_rej_penalt);
		PRINTW(F_REJ_LVL_8,fl_ptr->lev8_rej_penalt);
		PRINTW(F_EXC_LVL_1,fl_ptr->lev1_excl_time);
		PRINTW(F_EXC_PRM,fl_ptr->main_excl_time);
		PRINTW(F_EXC_LVL_2,fl_ptr->lev2_excl_time);
		PRINTW(F_EXC_LVL_3,fl_ptr->lev3_excl_time);
		PRINTW(F_EXC_LVL_4,fl_ptr->lev4_excl_time);
		PRINTW(F_EXC_LVL_5,fl_ptr->lev5_excl_time);
		PRINTW(F_EXC_LVL_6,fl_ptr->lev6_excl_time);
		PRINTW(F_EXC_LVL_7,fl_ptr->lev7_excl_time);
		PRINTW(F_EXC_LVL_8,fl_ptr->lev8_excl_time);
		PRINTW(F_POST_1,fl_ptr->post_freq);
		PRINTW(F_CPOST,fl_ptr->cond_post_freq);
		PRINTW(F_INQRY_2,fl_ptr->zone_inqry_freq);
		PRINTW(F_SRCH,fl_ptr->zone_info_freq);
		PRINTW(F_INFO,fl_ptr->veh_inqry_freq);
		PRINTW(F_ZN_STAT,fl_ptr->zone_stat_freq);
		PRINTW(F_TIME_CALL,fl_ptr->time_call_freq);
		PRINTW(F_N_SH_1,fl_ptr->drv_no_show_pri);
		PRINTW(F_CNCL_1,fl_ptr->drv_cancel_pri);
		PRINTW(F_REG_PRI,fl_ptr->drv_regular_pri);
		PRINTW(F_LOCAL_PRI,fl_ptr->drv_local_pri);
		PRINTW(F_ROOKIE,fl_ptr->high_rookie_pri);
		PRINTW(F_N_SH_2,fl_ptr->no_show_action);
		PRINTW(F_CNCL_2,fl_ptr->cancel_action);
		PRINTW(F_EPOST_ACT,fl_ptr->enr_post_action);
		PRINTW(F_M_ACTION_RETRY,fl_ptr->m_action_retry);
		PRINTW(F_C_OFFER,fl_ptr->call_offer_proc);
		PRINTW(F_ACPT,fl_ptr->call_accept_per);
		PRINTW(F_BREAK,fl_ptr->break_duration);
		PRINTW(F_SH_METER,fl_ptr->short_meter_dur);
		PRINTW(F_M_ACTION_TIMER,fl_ptr->m_action_timer);
		PRINTW(F_LOCAL_DUR,fl_ptr->local_calls_dur);
		PRINTW(F_C_MATCH,fl_ptr->avl_call_q_time);
		PRINTW(F_CANX_TO_MOFF,fl_ptr->canx_to_moff_time);
		PRINTW(F_NS_SHORT,fl_ptr->ns_short);
                PRINTW(F_TC_INT, fl_ptr->cbook_time);
		PRINTW(F_N_ACPT_TP,fl_ptr->no_acpt_penalt);
		PRINTW(F_N_ACPT_BT,fl_ptr->no_acpt_penalty);
		PRINTW(F_NAK_OFFER,fl_ptr->nak_offer_penal);
		PRINTW(F_EPOST_LT,fl_ptr->enr_post_lead);
		PRINTW(F_TIME_C,fl_ptr->future_call_min);
		PRINTW(F_LATE_M,fl_ptr->late_meter);
		PRINTW(F_WAKEUP_LTM,fl_ptr->wakeup_lead_time);
		PRINTW(F_CALL_OFFER, fl_ptr->cal_offer_info);
		PRINTW(F_CALL_ASSIGN, fl_ptr->cal_assign_info);
		PRINTW(F_EPOST_AL,fl_ptr->when_enr_post);
#ifdef NAMERICA
		PRINTW(F_SEND_DEST_ADDR, fl_ptr->send_dest_addr);
		PRINTW(F_SEND_RATE, fl_ptr->send_rate);
#endif
		PRINTW(F_GPS, fl_ptr->gps_state);
		PRINTW(F_GPS_TIME, fl_ptr->gps_time);
		PRINTW(F_PRSNLS,fl_ptr->no_personal);
		PRINTW(F_REJ_CNT,fl_ptr->max_rej_count);
		PRINTW(F_D_ATTR,fl_ptr->max_nbr_drv_atr);
		PRINTW(F_V_ATTR,fl_ptr->max_nbr_veh_atr);
		PRINTW(F_V_CLS,fl_ptr->max_vehs_call);
		PRINTW(F_FUT_CALL, (int)fl_ptr->future_call_max);
		PRINTW(F_N_KDT,fl_ptr->pers_lead_tm);
		PRINTW(F_TM_C_PRC,fl_ptr->time_call_max);
		PRINTW(F_CK_SEALS, fl_ptr->no_seals);
		PRINTW(F_H_LINE,fl_ptr->hold_line_flag);
		PRINTW(F_GRT_LINE,fl_ptr->greeting_line);
		PRINTW(F_ST_FLG_P,fl_ptr->auto_stand_flag);
		PRINTW(F_FLG_TIME,fl_ptr->auto_flag_time);
		PRINTW(F_TIME_CLS_INT_REQ,fl_ptr->time_int_req);
#ifdef NAMERICA
		PRINTW(F_UNASSGND_LK_AHD,fl_ptr->unassgnd_look_ahead);
		PRINTW(F_SEND_DEST_OFFER, fl_ptr->send_dest_offer);
		PRINTW(F_SEND_PCKUP_CMT, fl_ptr->send_pckup_cmt);
#endif
		PRINTW(F_TIME_CLS_LEAD_SUPP,fl_ptr->time_calls_lead_supp);
		PRINTW(F_Z_BK_LVL,fl_ptr->zone_backup_lev);
		PRINTW(F_D_MSG_AP,fl_ptr->drv_msg_approve);
		PRINTW(F_S_CL_RVW,fl_ptr->subsc_review);
		PRINTW(F_CHARGE,fl_ptr->charge_pri_def);
		PRINTW(F_S_CALL,fl_ptr->subsc_pri_def);
		PRINTW(F_TM_CLS,fl_ptr->calls_pri_def);
		PRINTW(F_PASS,fl_ptr->pass_pri_def);
		PRINTW(F_DEL,fl_ptr->del_pri_def);
		PRINTW(F_X_TYPE,fl_ptr->typex_pri_def);
		PRINTW(F_W_KUP,fl_ptr->wake_up_pri_def);
		PRINTW(F_MAN_MET_1,fl_ptr->met_pass_call);
		PRINTW(F_MAN_MET_2,fl_ptr->met_del_call);
		PRINTW(F_MAN_MET_3,fl_ptr->met_typex_call);
		PRINTW(F_R_CALC_1,fl_ptr->pass_rate_calc);
		PRINTW(F_R_CALC_2,fl_ptr->dlv_rate_calc);
	}
	if(scr_nbr == 2)  {
		PRINTW(F_AU_TL_PT,fl_ptr->audt_trail_port);
		PRINTW(F_T_CL_RPT,fl_ptr->time_calls_freq);
		PRINTW(F_MSG_RPT,fl_ptr->msg_rpt_freq);
                PRINTW(F_ILINK_ACTIVE, fl_ptr->ilink_active);
                PRINTW(F_ILINK_TIMEOUT, fl_ptr->ilink_timeout);
		PRINTW(F_CITY,fl_ptr->city_default); 
		PRINTW(F_A_CODE,fl_ptr->phone_area_def);
		PRINTW(F_CL_TYP,fl_ptr->call_type_def);
		PRINTW(F_H_SRCH,fl_ptr->hist_search_def);
		PRINTW(F_MTCH_ALG,fl_ptr->match_algorithm);
		PRINTW(F_GPS_MAX_X,fl_ptr->gps_max_x);
		PRINTW(F_GPS_MIN_X,fl_ptr->gps_min_x);
		PRINTW(F_GPS_MAX_Y,fl_ptr->gps_max_y);
		PRINTW(F_GPS_MIN_Y,fl_ptr->gps_min_y);
		PRINTW(F_GPS_COORD_SYS,fl_ptr->gps_coord_sys);
		PRINTW(F_GPS_DISTANCE_CEILING, fl_ptr->gps_distance_ceiling);
		PRINTW(F_GPS_BOOK, fl_ptr->gps_book);
		PRINTW(F_1ST_EST,fl_ptr->range_limit1);
		PRINTW(F_1ST_TXT,fl_ptr->estimate_text1);
		PRINTW(F_2ND_EST,fl_ptr->range_limit2);
		PRINTW(F_2ND_TXT,fl_ptr->estimate_text2);
		PRINTW(F_3RD_EST,fl_ptr->range_limit3);
		PRINTW(F_3RD_TXT,fl_ptr->estimate_text3);
		PRINTW(F_4TH_EST,fl_ptr->range_limit4);
		PRINTW(F_4TH_TXT,fl_ptr->estimate_text4);
		PRINTW(F_5TH_EST,fl_ptr->range_limit5);
		PRINTW(F_5TH_TXT,fl_ptr->estimate_text5);
		PRINTW(F_6TH_EST,fl_ptr->range_limit6);
		PRINTW(F_6TH_TXT,fl_ptr->estimate_text6);
		PRINTW(F_7TH_EST,fl_ptr->range_limit7);
		PRINTW(F_7TH_TXT,fl_ptr->estimate_text7);
		PRINTW(F_8TH_EST,fl_ptr->range_limit8);
		PRINTW(F_8TH_TXT,fl_ptr->estimate_text8);
		PRINTW(F_DEFAULT,fl_ptr->def_est_text);
		PRINTW(F_INACTIVE_RETRY_INTERVAL, fl_ptr->inactive_retry_interval);
		PRINTW(F_INACTIVE_TIME_OUT_PERIOD, fl_ptr->inactive_time_out_period);
		PRINTW(F_APPROACH_TARIFF, fl_ptr->approach_tariff);
		PRINTW(F_NUM_LEVELS_TO_COMPUTE_STATS, fl_ptr->num_levels_to_compute_stats);
		PRINTW(F_STATS_COMPUTE_FREQ, fl_ptr->stats_compute_freq);
		PRINTW(F_HANDLE_SYSTEM_ASSIGNED_CALLS, fl_ptr->handle_system_assigned_calls);
#ifdef SCANDINAVIA
		PRINTW(F_FLAT_RATE_FILTER, fl_ptr->FR_distance_filter);
#endif
		PRINTW(F_SEND_CALL_TYPE, fl_ptr->send_call_type);
		PRINTW(F_AUTO_REJECT_TIME, fl_ptr->auto_reject_time);
                PRINTW(F_EMERG_RANGE, fl_ptr->emerg_range);
                PRINTW(F_ENROUTE_BREAK, fl_ptr->enroute_break);
		PRINTW(F_LOAD_UNLOAD, fl_ptr->load_unload);
	}
	refresh();

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_data\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_data  */



/******************************************************************************
* send_ipc - send a message to dispatch that a record in the driver file changed.
*******************************************************************************/

void send_ipc()
{
	int qid, key = MSGKEY;

#ifdef DEBUG
	sprintf(trace_str,"Entering send_ipc\n");
	TRACE(trace_str);
#endif

	df_maint_ipc.d_prcs_id = DISP_DEST;
	df_maint_ipc.u_prcs_id = getpid();
	df_maint_ipc.rec_type = DFM_REQ;		
	df_maint_ipc.which_file = FLEET_FILE_ID;
	df_maint_ipc.fleet = fl_ptr->id;
	df_maint_ipc.key = fl_ptr->nbr; 
	df_maint_ipc.cisam_num = isrecnum;         
	if((qid = msgget(key,IPC_CREAT | 0666)) < 0)   {
		prt_error(FLEET_GET_ERR);
		prt_unix_err(errno,FLEET_GET_ERR);
		sleep(3);
		ERROR(' ',FLEET_FILE,FLEET_GET_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving send_ipc - 1\n");
	TRACE(trace_str);
#endif
		cleanup();
	}

	if((msgsnd(qid,&df_maint_ipc,sizeof(df_maint_ipc), IPC_NOWAIT)) < 0)  {
		prt_error(FLEET_SND_ERR);
		prt_unix_err(errno,FLEET_SND_ERR);
		sleep(3);
		ERROR(' ',FLEET_FILE,FLEET_SND_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving send_ipc - 2\n");
	TRACE(trace_str);
#endif
		cleanup();
	}
#ifdef DEBUG
	sprintf(trace_str,"Leaving send_ipc - 3\n");
	TRACE(trace_str);
#endif
	return;

} /*  End of send_ipc()  */

/******************************************************************************
* Clear the data fields on the screen. 
*******************************************************************************/

void clear_flds()
{
	int i;		/* loop index */

#ifdef DEBUG
	sprintf(trace_str,"Entering clear_flds\n");
	TRACE(trace_str);
#endif

	if(scr_nbr == 1)
		for(i = 1; i <= F_SCR_1_MAX; i++)
			fl_cl_flds(i);
	if(scr_nbr == 2)
		for(i = F_SCR_2_MIN; i <= F_SCR_2_MAX; i++) 
			fl_cl_flds(i); 

#ifdef DEBUG
	sprintf(trace_str,"Leaving clear_flds\n");
	TRACE(trace_str);
#endif
	return;
}

/******************************************************************************
* prt_hdr - print the main screen header.
*******************************************************************************/

void prt_hdr()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_hdr\n");
	TRACE(trace_str);
#endif

	clear_hdr_line();
	if (AddUpdateDeleteAllowed())
	  mvaddstr(HDR_ROW_1,HDR_COL_1,FLEET_MAIN_HDR);
	else
	  mvaddstr(HDR_ROW_1,HDR_COL_1,"Query Next Previous Screen Output Exit");
	attrset(A_BOLD);
	mvaddstr(HDR_ROW_2,HDR_COL_2,FLEET_TITLE);
	attrset(0);
	refresh();
	
#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_hdr\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of prt_hdr()  */

/******************************************************************************
* prompt_fname - prompt the user for a file name to output the screen image to.
*******************************************************************************/

void prompt_fname()
{

#ifdef DEBUG
	sprintf(trace_str,"Entering prompt_fname\n");
	TRACE(trace_str);
#endif
	
	clear_hdr_line();
	mvaddstr(HDR_ROW_1,HDR_COL_1,FLEET_FNAME_PROMPT);
	refresh();
	getstr(o_file);
	if(!strcmp(o_file,""))
		strcpy(o_file,"fl_scr.out");

#ifdef DEBUG
	sprintf(trace_str,"Leaving prompt_fname\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prompt_fname()  */
