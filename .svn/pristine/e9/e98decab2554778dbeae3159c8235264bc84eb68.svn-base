static char sz__FILE__[]="@(#)up_sub1.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  up_sub1.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:48:29
 * @(#)  Last delta: 12/2/94 at 11:36:54
 * @(#)  SCCS File: /taxi/sccs/s.up_sub1.c
 *                                                                           *
 *****************************************************************************/
#include <cursesX.h>
#include "up_m_strdefs.h"
#include "mad_error.h"
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include "mads_types.h"
#include <sys/ipc.h>
#include <isam.h>
#include "df_maint.h"
#include "mads_isam.h"
#include "ex_key.h"

#include <signal.h>
#include "up_maint.h"
#include "literals.h"
#include <memory.h>
#include "path.h"
#include "tokenize.h"
#include "language.h"

extern int upper_case;
extern int ret_val;		/* return value */
extern int piu;			/* prompt in use */
extern int intr_allowed;	/* interrupts allowed */
extern int key_val;		/* value returned from the check routines */
extern char read_buf[];		/* buffer for reading fields */
extern int field_entered;	/* field entered or not */
extern int saw_intr;		/* indicates an interrupt signal was entered */
extern int cur_list;		/* indicates a row is in the current list */
extern int (*check_field[])();	/* Array of pointers to routines to validate
				   the data input by the user */
extern int record_nbr;		/* C_ISAM record number */
extern char o_file[];		/* output file as specified by the user */
extern char mode[];		/* user specified Append or Create */	
extern char req_buf[];
extern struct scr_flds up_scr_flds[]; 
extern struct cisam_up *up_ptr; /* pointer to user profile file structure */
extern struct cisam_up *temp_up_ptr; 
extern struct cisam_up temp_up_buf; 
extern int clear_flds();	/* clear data fields on the screen */
extern int doing_add;		/* performing add operation */
extern int name_flag;	
extern int usr_id_flag;

/******************************************************************************
* process_query - search for specific record in the user profile file
*******************************************************************************/

process_query()
{
	int i;				/* loop index */
	int should_clear = 0;

#ifdef DEBUG
	sprintf(trace_str,"Entering process_query\n");
	TRACE(trace_str);
#endif

	clear_err_line();
	clear_flds();
	prt_query_hdr();		/* Print the header for the query 
					   screen */

	/*  Process users input of key field(command string).  */

    	ret_val = NOT_VALID;
	while(ret_val == NOT_VALID)  {
		piu = F_ID;
		intr_allowed = 1;		/* interrupts allowed */
		if (piu == 2)
			upper_case = 0;
		else
			upper_case = 1;
		norm_to_rev(stdscr,up_scr_flds,piu);
		key_val = read_field(stdscr,up_scr_flds,piu,read_buf,&field_entered,&should_clear);
		revund_to_norm(stdscr,up_scr_flds,piu);
		attrset(0);
		intr_allowed = 0;		/* interrupts not allowed */

		if(saw_intr)  {			/* key_value = interrupt char */
			saw_intr = 0;		/* interrup has been processed */
			cur_list = 0;		/* no row in current list */
			clear_flds();
			clear_err_line();
			usr_id_flag = 0;
			prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_query - 1\n");
	TRACE(trace_str);
#endif
			return;
		}

		/* validate input data, store it in c-isam structure, and
		   adjust the field pointer */

		ret_val = check_field[F_ID](&piu,read_buf,key_val,&field_entered);
		if(ret_val == NOT_VALID)
			continue;
		if((usr_id_flag) && (key_val != ESC))	{
			prt_error(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_12, "Enter ESC to execute"));
			ret_val = NOT_VALID;
			continue;
		}
	/*	if(((key_val == ESC) || (key_val == VALID) ||(key_val == CR)) && (field_entered))  {	*/

		if (key_val == ESC)	{
			clear_err_line();
			if (!usr_id_flag)
				up_ptr->user_id = 0;
	/*		if (!field_entered)	
				up_ptr->user_id = 0;
	*/			
			if((db(USER_FILE_ID,READ_KEYS,&up_key1,ISGTEQ,0)) < 0)  {
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
					prt_error(READ_PROFILE_ERR);
					prt_cisam_err(iserrno,READ_PROFILE_ERR);
					ERROR(' ',PROFILE_FILE,READ_PROFILE_ERR);
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
	usr_id_flag = 0;
	return;

}  /* End of process_query()  */

/******************************************************************************
* process_next - read the next record in the user profile file and display it.
*******************************************************************************/

process_next()
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

	if((db(USER_FILE_ID,READ_KEYS,&up_key1,ISNEXT,0)) < 0)  {
		if(iserrno == EENDFILE)  {
			prt_error(NO_ROWS_DIR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_next - 2\n");
	TRACE(trace_str);
#endif
			return;
		}
		else  {
			prt_error(RD_NXT_PROFILE_ERR);
			prt_cisam_err(iserrno,RD_NXT_PROFILE_ERR);
			ERROR(' ',PROFILE_FILE,RD_NXT_PROFILE_ERR);
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
* process_previous - read the previous record in the user profile file and display it.
*******************************************************************************/

process_previous()
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

	if((db(USER_FILE_ID,READ_KEYS,&up_key1,ISPREV,0)) < 0)  {
		if(iserrno == EENDFILE)  {
			prt_error(NO_ROWS_DIR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_previous - 2\n");
	TRACE(trace_str);
#endif
			return;
		}
		else  {
			prt_error(RD_PREV_PROFILE_ERR);
			prt_cisam_err(iserrno,RD_PREV_PROFILE_ERR);
			ERROR(' ',PROFILE_FILE,RD_PREV_PROFILE_ERR);
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
* process_add - input data fields and add the new record to the user profile file 
*******************************************************************************/

process_add()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering process_add\n");
	TRACE(trace_str);
#endif

	clear_err_line();
	clear_flds();

	/* reset all the members of the user profile file structure */

	memset((char *)up_ptr,'\0',sizeof(struct cisam_up));

	store_def();		/* default fleet/menu permissions to 'N' */

	piu = F_NAME;		/* start with the user id */
	prt_add_hdr();		/* Print the header line for the the add screen.  */
	prt_labels();		/* print the screen labels  */
	prt_data();
	doing_add = 1;
	process_input();	/* Process each field as input by the user */
	doing_add = 0;
	if(saw_intr)  {
		saw_intr = 0; 	/* interrupt has been processed */
		cur_list = 0;	/* no valid row in current list */
		clear_flds();
		name_flag = 0;
		clear_desc_line();
		clear_err_line();
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_add - 1\n");
	TRACE(trace_str);
#endif
		return;
	}

	/*  Call the C-Isam add record routine.  */

	if((db(USER_FILE_ID,ADD_REC,&up_key1,0,0)) < 0)  {
		prt_error(ADD_PROFILE_ERR);
		clear_flds();
		clear_desc_line();
		clear_err_line();
		prt_hdr();
		return;
	}

	cur_list = 1;		/* valid row in current list */
	prt_hdr();		/* Display the user profile file maintenance screen header line  */
	clear_desc_line();

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_add - 3\n");
	TRACE(trace_str);
#endif
	name_flag = 0;
	return;

}  /* End of process_add()  */

/******************************************************************************
* process_update - input data fields and update the user profile file record
*******************************************************************************/

process_update()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering process_update\n");
	TRACE(trace_str);
#endif

	clear_err_line();
	piu = F_GRT_NAME;			/* start with the user name */
	if(!cur_list)  {
		prt_error(NO_CUR_LIST);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_update - 1\n");
	TRACE(trace_str);
#endif
		return;
	}

	prt_upd_hdr();		/* Print the header line for the update screen.  */

	if((db(USER_FILE_ID,READ_KEYS,&up_key1,ISEQUAL,0)) < 0) {
		prt_error(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_13, "User profile read error"));
		clear_err_line();
		prt_hdr();
		return;
	}
	process_input();	/* Process each field as input by the user */
	if(saw_intr)  {
		saw_intr = 0;	/* interrupt has been processed */
		cur_list = 0;	/* no valid row in current list */
		clear_flds();
		clear_desc_line();
		clear_err_line();
		name_flag = 0;
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_update - 3\n");
	TRACE(trace_str);
#endif
		return;
	}

	/*  Call the C-Isam update record routine.  */
	memcpy((char *)temp_up_ptr,req_buf,sizeof(struct cisam_up));
	if((db(USER_FILE_ID,READ_KEYS,&up_key1,ISEQUAL+ISLOCK,0)) < 0) {
		prt_error(LOCK_PROFILE_ERR);
		prt_cisam_err(iserrno,LOCK_PROFILE_ERR);
		ERROR(' ',PROFILE_FILE,LOCK_PROFILE_ERR);
		clear_err_line();
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_update - 2\n");
	TRACE(trace_str);
#endif
		return;
	}

	memcpy(req_buf,(char *)temp_up_ptr,sizeof(struct cisam_up));
	if((db(USER_FILE_ID,UPDATE_REC,&up_key1,0,0)) < 0)  {
		prt_error(UPD_PROFILE_ERR);
		prt_cisam_err(iserrno,UPD_PROFILE_ERR);
		ERROR(' ',PROFILE_FILE,UPD_PROFILE_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_update - 4\n");
	TRACE(trace_str);
#endif
		clear_desc_line();
		clear_err_line();
		prt_hdr();
	}

	prt_error(REC_UPDATED);
	if((db(USER_FILE_ID,RELEASE,&up_key1,0,0)) < 0)  {
		prt_error(RLS_PROFILE_ERR);
		cur_list = 0;
		prt_cisam_err(iserrno,RLS_PROFILE_ERR);
		ERROR(' ',PROFILE_FILE,RLS_PROFILE_ERR);
		prt_hdr();
		clear_desc_line();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_update - 5\n");
	TRACE(trace_str);
#endif
		return;
	}

	cur_list = 1;		/* valid row in current list */
        prt_hdr();		/* Display the user profile file maintenance screen header line */
	clear_desc_line();

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_update - 6\n");
	TRACE(trace_str);
#endif
	name_flag = 0;
	return;

}  /*  End of process_update()  */

/******************************************************************************
* process_remove - remove the current user profile file record.
*******************************************************************************/

process_remove()
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

	if((db(USER_FILE_ID,DELETE,&up_key1,0,0)) < 0)  {
		prt_error(DEL_PROFILE_ERR);
		prt_cisam_err(iserrno,DEL_PROFILE_ERR);
		ERROR(' ',PROFILE_FILE,DEL_PROFILE_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_remove - 3\n");
	TRACE(trace_str);
#endif
		cleanup();
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

process_output()
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
		ERROR(' ',PROFILE_FILE,OPEN_OUT_ERR);
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

/*****************************************************************************
* store_def - store default values for the fleet/menu permission arrays.
****************************************************************************/

store_def()
{
	int i;		/* loop index */

#ifdef DEBUG
	sprintf(trace_str,"Entering store_def\n");
	TRACE(trace_str);
#endif

	for(i = 0; i < NUM_FLT_PERMS ; i++)
		up_ptr->fleet_perms[i] = NO;

	up_ptr->menu_perms[0] = YES;
	for(i = 1; i < NUM_MENU_PERMS ; i++)
		up_ptr->menu_perms[i] = NO;

#ifdef DEBUG
	sprintf(trace_str,"Leaving store_def\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of store_def()  */
