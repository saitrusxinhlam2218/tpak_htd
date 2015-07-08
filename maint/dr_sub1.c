static char sz__FILE__[]="@(#)dr_sub1.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  dr_sub1.c; Rel: 5.3.0.0; Get date: 3/26/90 at 14:02:06
 * @(#)  Last delta: 3/22/90 at 17:03:28
 * @(#)  SCCS File: /taxi/sccs/s.dr_sub1.c
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

#include "dr_m_strdefs.h"
#include "mad_error.h"
#include "mads_types.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "df_maint.h"
#include "mads_isam.h"
#include "ex_key.h"
#include "dr_maint.h"
#include "tokenize.h"
#include "literals.h"
#include "path.h"
#include "language.h"
#include "fleet.h"


extern int upper_case;
extern short skip_return;
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
extern int fleet_flag,id_flag;
extern struct cisam_dr *dr_ptr; /* pointer to user vehicle file structure */
extern struct cisam_dr *temp_dr_ptr; 
extern struct cisam_dr temp_dr_buf; 
extern int clear_flds();	/* clear data fields on the screen */
extern int doing_add;		/* performing add operation */
extern void prt_labels();
extern int clear_desc_line();	/* routine to clear the field description line */
extern short ipc_required;	/* Set if a field changed which is in shared memory */

extern struct  scr_flds dr_scr_flds[];
extern struct df_maint df_maint_ipc;	/* structure for passing information 
				   	   to dispatch  */
extern int AddUpdateDeleteAllowed();
extern MaxAttrs;
void prt_hdr();

/******************************************************************************
* process_query - search for specific record in the driver file
*******************************************************************************/

process_query()
{
	int i;				/* loop index */
	int should_clear;
#ifdef DEBUG
	sprintf(trace_str,"Entering process_query\n");
	TRACE(trace_str);
#endif

	clear_err_line();
	clear_flds();
	prt_query_hdr();		/* Print the header for the query 
					   screen */

	/*  Process users input of key fields(fleet and driver id ).  */

	piu = F_FLEET;
    	ret_val = NOT_VALID;
	while(ret_val == NOT_VALID)  {
		intr_allowed = 1;		/* interrupts allowed */
		if ( (piu == 1) || ( piu == F_ATTR_1 ) )
			skip_return = 1;
		else
			skip_return = 0;
		fld_desc(piu);

		/* Clear on dates and times */
		if (piu == F_SP_FM_DATE    ||
		    piu == F_SP_TO_DATE    ||
		    piu == F_SP_D_FM_DATE  ||
		    piu == F_SP_D_TO_DATE  ||
		    piu == F_SP_X_FM_DATE  ||
		    piu == F_SP_X_TO_DATE  ||
		    piu == F_EMRG_ON_DATE  ||
		    piu == F_EMRG_OFF_DATE ||
		    piu == F_SGN_ON_DATE   ||
		    piu == F_SGN_OFF_DATE  ||
		    piu == F_SP_FM_TIME    ||
		    piu == F_SP_TO_TIME    ||
		    piu == F_SP_D_FM_TIME  ||
		    piu == F_SP_D_TO_TIME  ||
		    piu == F_SP_X_FM_TIME  ||
		    piu == F_SP_X_TO_TIME  ||
		    piu == F_EMRG_ON_TIME  ||
		    piu == F_EMRG_OFF_TIME ||
		    piu == F_SGN_ON_TIME   ||
		    piu == F_SGN_OFF_TIME)

		    should_clear = 1;
		else
		    should_clear = 0;

		key_val = read_field(stdscr,dr_scr_flds,piu,read_buf,&field_entered,&should_clear);
		intr_allowed = 0;		/* interrupts not allowed */

		if(saw_intr)  {			/* key_value = interrupt char */
			saw_intr = 0;		/* interrup has been processed */
			cur_list = 0;		/* no row in current list */
			clear_flds();
			clear_err_line();
			prt_hdr();
			fleet_flag = 0;
			id_flag = 0;
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_query - 1\n");
	TRACE(trace_str);
#endif
			return;
		}

		/* validate input data, store it in c-isam structure, and
		   adjust the field pointer */

		ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);
		if(ret_val == NOT_VALID)
			continue;

		if ((!fleet_flag) && (key_val != ESC)) 	{
			prt_error(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_85, "You must enter value"));
			continue;
		}
		if ((!id_flag) && (key_val != ESC)) 	{
			prt_error(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_85, "You must enter value"));
			--piu;
			ret_val = NOT_VALID;
			continue;
		}
		if ((id_flag) && (key_val != ESC))	{
			prt_error(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_86, "Enter ESC to execute"));
			--piu;
			ret_val = NOT_VALID;
			continue;
		}
		if (key_val == ESC)	{
		/*  Using the key entered in previous step, call C-Isam routine
	    	    to retrieve specific record.  */
			clear_err_line();
			if (!fleet_flag)	{
				dr_ptr->fleet = 'A';
				dr_ptr->id = 0;
			}
			if (!id_flag)
				dr_ptr->id = 0;
	/*		if ((piu == 1) && (!field_entered))	{
				dr_ptr->fleet = 'A';
				dr_ptr->id = 0;
			}
*/
			if((db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISGTEQ,0)) < 0)  {
				if(iserrno == ENOREC)  {
					prt_error(NO_ROWS);
					clear_flds();
					cur_list = 0;	/* row in current list is invalid */
					prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_query - 3\n");
	TRACE(trace_str);
#endif
					return;
				}
				else  {
					prt_error(READ_DRIVER_ERR);
					sleep(3);
					prt_cisam_err(iserrno,READ_DRIVER_ERR);
					ERROR(' ',DRIVER_FILE,READ_DRIVER_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_query - 4\n");
	TRACE(trace_str);
#endif
					cleanup();
				}
			}
			record_nbr = isrecnum;	/* save record number */
			cur_list = 1;		/* valid row in current list */
        		prt_data();  		
			prt_hdr();
		    }
	    }

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_query - 5\n");
	TRACE(trace_str);
#endif

	fleet_flag = 0;
	id_flag = 0;
	return;

}  /* End of process_query()  */

/******************************************************************************
* process_next - read the next record in the driver file and display it.
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

	if((db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISNEXT,0)) < 0)  {
		if(iserrno == EENDFILE)  {
			prt_error(NO_ROWS_DIR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_next - 2\n");
	TRACE(trace_str);
#endif
			return;
		}
		else  {
			prt_error(RD_NXT_DRIVER_ERR);
			sleep(3);
			prt_cisam_err(iserrno,RD_NXT_DRIVER_ERR);
			ERROR(' ',DRIVER_FILE,RD_NXT_DRIVER_ERR);
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

	if((db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISPREV,0)) < 0)  {
		if(iserrno == EENDFILE)  {
			prt_error(NO_ROWS_DIR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_previous - 2\n");
	TRACE(trace_str);
#endif
			return;
		}
		else  {
			prt_error(RD_PREV_DRIVER_ERR);
			sleep(3);
			prt_cisam_err(iserrno,RD_PREV_DRIVER_ERR);
			ERROR(' ',DRIVER_FILE,RD_PREV_DRIVER_ERR);
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

process_add()
{
	char err[80];

	clear_err_line();
	clear_flds();

	/* reset all the members of the driver file structure */

	memset((char *)dr_ptr,'\0',sizeof(struct cisam_dr));

	store_def();		/* default seals/attributes to 'N' */

	piu = F_FLEET;		/* start with the fleet field */
	prt_add_hdr();		/* Print the header line for the the add screen.  */
	prt_labels();		/* print the screen labels  */
	prt_data();
	while(1)
	{
		doing_add = 1;
		process_input();	/* Process each field as input by the user */
		doing_add = 0;
		if(saw_intr)  {
			saw_intr = 0; 	/* interrupt has been processed */
			cur_list = 0;	/* no valid row in current list */
			clear_flds();
			clear_desc_line();
			prt_hdr();
			fleet_flag = 0;
			id_flag = 0;
			return;
		}
		/* save inputted data from process_input() in a temp struct because the next call to
			db may over write if a key is found - thus losing user entered data */
		memcpy((char *)temp_dr_ptr,req_buf,sizeof(struct cisam_dr));
		if((db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISEQUAL,0)) == 0) 
		{
			prt_error(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_87, "Driver id already exists"));
		}
		else
		{
			break;
		}
		memcpy(req_buf,(char *)temp_dr_ptr,sizeof(struct cisam_dr));
		
	}
	memcpy(req_buf,(char *)temp_dr_ptr,sizeof(struct cisam_dr));
	/*  Call the C-Isam add record routine.  */

	if((db(DRIVER_FILE_ID,ADD_REC,&dr_key1,0,0)) < 0)  {
		prt_error(ADD_DRIVER_ERR);
		sprintf(err, "%s(%d)", ADD_DRIVER_ERR, iserrno);
		ERROR(' ', DRIVER_FILE, err);
	}
	else
	{
	  /* After an ADD to db, no current record is defined.
	     Read the just added record to set the current record */
	  db(DRIVER_FILE_ID, READ_KEYS, &dr_key1, ISEQUAL, 0);
	  ipc_required = 1;
	}

	cur_list = 1;		/* valid row in current list */
	prt_hdr();		/* Display the vehicle file maintenance screen header line  */
	clear_desc_line();

	fleet_flag = 0;
	id_flag = 0;
}  /* End of process_add()  */

/******************************************************************************
* process_update - input data fields and update the driver file record
*******************************************************************************/

process_update()
{
	int dr_id,tmp_id;
	char err[80];

	if((db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISEQUAL,0)) < 0) {
		prt_error(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_88, "Driver file read error"));
		prt_hdr();
		return;
	}

	clear_err_line();
	if(!cur_list)  {
		prt_error(NO_CUR_LIST);
		return;
	}
	dr_id = dr_ptr->id;
	prt_upd_hdr();		/* Print the header line for the update screen.  */
	while(1)
	{
		piu = F_CLASS;		/* start with the license number */
		process_input();	/* Process each field as input by the user */
		if(saw_intr)  {
			saw_intr = 0;	/* interrupt has been processed */
			cur_list = 0;	/* no valid row in current list */
			clear_flds();
			clear_desc_line();
			prt_hdr();
			fleet_flag = 0;
			id_flag = 0;
			return;
		}
		memcpy((char *)temp_dr_ptr,req_buf,sizeof(struct cisam_dr));
		if(dr_id == dr_ptr->id)
		{
			break; /* never changed id */
		}
		else if((db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISEQUAL,0)) == 0) 
		{
			prt_error(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_87, "Driver id already exists"));
		}
		else
		{
			break;
	 	}
		memcpy(req_buf,(char *)temp_dr_ptr,sizeof(struct cisam_dr));

	}
	memcpy(req_buf,(char *)temp_dr_ptr,sizeof(struct cisam_dr));
	memcpy((char *)temp_dr_ptr,req_buf,sizeof(struct cisam_dr));
	tmp_id = dr_ptr->id;
	dr_ptr->id = dr_id;
	if((db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISEQUAL+ISLOCK,0)) < 0) {
		prt_error(LOCK_DRIVER_ERR);
		prt_cisam_err(iserrno,LOCK_DRIVER_ERR);
		ERROR(' ',DRIVER_FILE,LOCK_DRIVER_ERR);
		prt_hdr();
		return;
	}

	memcpy(req_buf,(char *)temp_dr_ptr,sizeof(struct cisam_dr));
	dr_ptr->id = tmp_id;
	if((db(DRIVER_FILE_ID,UPDATE_REC,&dr_key1,0,0)) < 0)  {
		prt_error(UPD_DRIVER_ERR);
		sprintf(err, "%s(%d)", UPD_DRIVER_ERR, iserrno);
		ERROR(' ',DRIVER_FILE, err);
	}
	else
	{
	  prt_error(REC_UPDATED);
	  ipc_required = 1;
	}

	if((db(DRIVER_FILE_ID,RELEASE,&dr_key1,0,0)) < 0)  {
		prt_error(RLS_DRIVER_ERR);
		cur_list = 0;
		prt_cisam_err(iserrno,RLS_DRIVER_ERR);
		ERROR(' ',DRIVER_FILE,RLS_DRIVER_ERR);
		prt_hdr();
		clear_desc_line();
		return;
	}

	cur_list = 1;		/* valid row in current list */
        prt_hdr();		/* Display the vehicle file maintenance screen header line */
	clear_desc_line();

	fleet_flag = 0;
	id_flag = 0;

}  /*  End of process_update()  */

/******************************************************************************
* process_remove - remove the current driver file record.
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

	answer = prompt_confirm();	/* make sure the user wants to remove this record */

	if(answer == NO)  {			/* user answered negatively */
		prt_hdr();
		return;
	}

	/*  If the user responded affirmatively, call C-Isam routine 
	    to delete the record.  */

	if((db(DRIVER_FILE_ID,DELETE,&dr_key1,0,0)) < 0)  {
		prt_error(DEL_DRIVER_ERR);
		sprintf(err, "%s(%d)", DEL_DRIVER_ERR, iserrno);
		ERROR(' ',DRIVER_FILE, err);
	}
	/* If one looks at df_maint.c in dispatch,
	   dispatch doesn't care if a driver has
	   been removed, so there is no need to
	   send the info over ipc.
	/******
	else
	  ipc_required = 1;
	  *******/

	cur_list = 0;		/* no ro in current list */
	clear_flds();
	prt_hdr();


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
		ERROR(' ',DRIVER_FILE,OPEN_OUT_ERR);
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
* store_def - store default values for the seals/attributes arrays.
****************************************************************************/

store_def()
{
	int i;		/* loop index */

#ifdef DEBUG
	sprintf(trace_str,"Entering store_def\n");
	TRACE(trace_str);
#endif

	for(i = 0; i < NUM_SEALS ; i++)
		dr_ptr->seals[i] = NO;

	for(i = 0; i < NUM_ATTR ; i++)
		dr_ptr->attributes[i] = NO;

#ifdef DEBUG
	sprintf(trace_str,"Leaving store_def\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of store_def()  */

/******************************************************************************
* prt_labels - print the field labels on the screen.
*******************************************************************************/

void prt_labels()
{

	char	 open_brace[2],  close_brace[2];

#ifdef DEBUG
	sprintf(trace_str,"Entering prt_labels\n");
	TRACE(trace_str);
#endif

	strcpy(open_brace, 
		catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_139, "["));
	strcpy(close_brace, 
		catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_140, "]"));

	/* print the screen labels  */

	attrset(A_BOLD);
	mvaddstr(L_FLEET_ROW,L_FLEET_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_89, "Fleet"));
	mvaddstr(L_FLEET_ROW,L_DR_ID_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_90, "ID"));
	mvaddstr(L_FLEET_ROW,L_CLASS_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_91, "Class"));
	mvaddstr(L_FLEET_ROW,L_NAME_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_92, "Name"));
	mvaddstr(L_FLEET_ROW,L_ADD_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_93, "Address"));
	mvaddstr(L_FLEET_ROW,L_CITY_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_94, "City"));
	mvaddstr(L_FLEET_ROW,L_ZIP_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_95, "Zip"));
	mvaddstr(L_FLEET_ROW,L_ZSFX_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_96, "Suffix"));
	mvaddstr(L_PHONE_ROW,L_PHONE_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_97, "Phone"));
	mvaddstr(L_PHONE_ROW,L_LIC_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_98, "License Number"));
	mvaddstr(L_PHONE_ROW,L_TXI_LIC_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_99, "Taxi Lic"));
	mvaddstr(L_PHONE_ROW,L_SEAL_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_100, "Seals"));
	mvaddstr(L_PHONE_ROW,L_SEAL_1_COL,"1");
	mvaddstr(L_PHONE_ROW,L_SEAL_2_COL,"2");
	mvaddstr(L_PHONE_ROW,L_SEAL_3_COL,"3");
	mvaddstr(L_PHONE_ROW,L_SEAL_4_COL,"4");
	mvaddstr(L_PHONE_ROW,L_SEAL_5_COL,"5");
	mvaddstr(L_PHONE_ROW,L_SEAL_6_COL,"6");
	mvaddstr(L_PHONE_ROW,L_SEAL_7_COL,"7");
	mvaddstr(L_PHONE_ROW,L_SEAL_8_COL,"8");
	mvaddstr(L_PHONE_ROW,L_SEAL_9_COL,"9");
	mvaddstr(L_PHONE_ROW,L_SEAL_10_COL,"10");
	mvaddstr(L_PHONE_ROW,L_SEAL_11_COL,"11");
	mvaddstr(L_PHONE_ROW,L_SEAL_12_COL,"12");
	mvaddstr(L_PHONE_ROW,L_SEAL_13_COL,"13");
	mvaddstr(L_PHONE_ROW,L_SEAL_14_COL,"14");
	mvaddstr(L_PHONE_ROW,L_SEAL_15_COL,"15");
	mvaddstr(L_PHONE_ROW,L_SEAL_16_COL,"16");
	attrset(0);
	mvaddstr(L_FLEET_ROW+1,L_FLEET_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_FLEET_COL+MC_FLEET+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_DR_ID_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_DR_ID_COL+MC_DR_ID+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_CLASS_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_CLASS_COL+MC_CLASS+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_NAME_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_NAME_COL+MC_NAME+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_ADD_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_ADD_COL+MC_ADDRESS+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_CITY_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_CITY_COL+MC_CITY+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_ZIP_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_ZIP_COL+MC_ZIP+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_ZSFX_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_ZSFX_COL+MC_ZIP_SFX+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_PHONE_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_PHONE_COL+MC_PHONE+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_LIC_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_LIC_COL+MC_LIC+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_TXI_LIC_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_TXI_LIC_COL+MC_TXI_LIC+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_1_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_1_COL+MC_SEAL_1+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_2_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_2_COL+MC_SEAL_2+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_3_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_3_COL+MC_SEAL_3+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_4_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_4_COL+MC_SEAL_4+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_5_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_5_COL+MC_SEAL_5+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_6_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_6_COL+MC_SEAL_6+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_7_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_7_COL+MC_SEAL_7+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_8_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_8_COL+MC_SEAL_8+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_9_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_9_COL+MC_SEAL_9+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_10_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_10_COL+MC_SEAL_10+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_11_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_11_COL+MC_SEAL_11+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_12_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_12_COL+MC_SEAL_12+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_13_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_13_COL+MC_SEAL_13+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_14_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_14_COL+MC_SEAL_14+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_15_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_15_COL+MC_SEAL_15+1,close_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_16_COL,open_brace);
	mvaddstr(L_PHONE_ROW+1,L_SEAL_16_COL+MC_SEAL_16+1,close_brace);
	attrset(A_BOLD);
	mvaddstr(L_ATTR_ROW,L_ATTR_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_101, "Attributes"));
#ifdef FOO
	mvaddstr(L_ATTR_ROW,L_ATTR_1_COL,"1");
	mvaddstr(L_ATTR_ROW,L_ATTR_2_COL,"2");
	mvaddstr(L_ATTR_ROW,L_ATTR_3_COL,"3");
	mvaddstr(L_ATTR_ROW,L_ATTR_4_COL,"4");
	mvaddstr(L_ATTR_ROW,L_ATTR_5_COL,"5");
	mvaddstr(L_ATTR_ROW,L_ATTR_6_COL,"6");
	mvaddstr(L_ATTR_ROW,L_ATTR_7_COL,"7");
	mvaddstr(L_ATTR_ROW,L_ATTR_8_COL,"8");
#endif
	mvaddstr(L_ATTR_ROW,L_SP_FM_DATE_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_102, "Suspended From"));
	mvaddstr(L_ATTR_ROW,L_SP_TO_DATE_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_103, "Suspended To"));
	mvaddstr(L_ATTR_ROW,L_SUSP_RSN_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_104, "Suspension Reason"));
	mvaddstr(L_ATTR_ROW+1,L_SUSP_PASS_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_105, "P"));
	mvaddstr(L_ATTR_ROW+2,L_SUSP_DEL_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_106, "D"));
	mvaddstr(L_ATTR_ROW+3,L_SUSP_TPX_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_107, "X"));
	attrset(0);
	mvaddstr(L_ATTR_ROW+1,L_ATTR_1_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_ATTR_1_COL+MC_ATTR_1+1,close_brace);
#ifdef FOO
	mvaddstr(L_ATTR_ROW+1,L_ATTR_2_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_ATTR_2_COL+MC_ATTR_2+1,close_brace);
	mvaddstr(L_ATTR_ROW+1,L_ATTR_3_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_ATTR_3_COL+MC_ATTR_3+1,close_brace);
	mvaddstr(L_ATTR_ROW+1,L_ATTR_4_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_ATTR_4_COL+MC_ATTR_4+1,close_brace);
	mvaddstr(L_ATTR_ROW+1,L_ATTR_5_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_ATTR_5_COL+MC_ATTR_5+1,close_brace);
	mvaddstr(L_ATTR_ROW+1,L_ATTR_6_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_ATTR_6_COL+MC_ATTR_6+1,close_brace);
	mvaddstr(L_ATTR_ROW+1,L_ATTR_7_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_ATTR_7_COL+MC_ATTR_7+1,close_brace);
	mvaddstr(L_ATTR_ROW+1,L_ATTR_8_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_ATTR_8_COL+MC_ATTR_8+1,close_brace);
#endif
	mvaddstr(L_ATTR_ROW+1,L_SP_FM_DATE_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_SP_FM_DATE_COL+MC_SUSP_FM_DATE+1,close_brace);
	mvaddstr(L_ATTR_ROW+1,L_SP_FM_TIME_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_SP_FM_TIME_COL+MC_SUSP_FM_TIME+1,close_brace);
	mvaddstr(L_ATTR_ROW+1,L_SP_TO_DATE_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_SP_TO_DATE_COL+MC_SUSP_TO_DATE+1,close_brace);
	mvaddstr(L_ATTR_ROW+1,L_SP_TO_TIME_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_SP_TO_TIME_COL+MC_SUSP_TO_TIME+1,close_brace);
	mvaddstr(L_ATTR_ROW+2,L_SP_FM_DATE_COL,open_brace);
	mvaddstr(L_ATTR_ROW+2,L_SP_FM_DATE_COL+MC_SUSP_FM_DATE+1,close_brace);
	mvaddstr(L_ATTR_ROW+2,L_SP_FM_TIME_COL,open_brace);
	mvaddstr(L_ATTR_ROW+2,L_SP_FM_TIME_COL+MC_SUSP_FM_TIME+1,close_brace);
	mvaddstr(L_ATTR_ROW+2,L_SP_TO_DATE_COL,open_brace);
	mvaddstr(L_ATTR_ROW+2,L_SP_TO_DATE_COL+MC_SUSP_TO_DATE+1,close_brace);
	mvaddstr(L_ATTR_ROW+2,L_SP_TO_TIME_COL,open_brace);
	mvaddstr(L_ATTR_ROW+2,L_SP_TO_TIME_COL+MC_SUSP_TO_TIME+1,close_brace);
	mvaddstr(L_ATTR_ROW+3,L_SP_FM_DATE_COL,open_brace);
	mvaddstr(L_ATTR_ROW+3,L_SP_FM_DATE_COL+MC_SUSP_FM_DATE+1,close_brace);
	mvaddstr(L_ATTR_ROW+3,L_SP_FM_TIME_COL,open_brace);
	mvaddstr(L_ATTR_ROW+3,L_SP_FM_TIME_COL+MC_SUSP_FM_TIME+1,close_brace);
	mvaddstr(L_ATTR_ROW+3,L_SP_TO_DATE_COL,open_brace);
	mvaddstr(L_ATTR_ROW+3,L_SP_TO_DATE_COL+MC_SUSP_TO_DATE+1,close_brace);
	mvaddstr(L_ATTR_ROW+3,L_SP_TO_TIME_COL,open_brace);
	mvaddstr(L_ATTR_ROW+3,L_SP_TO_TIME_COL+MC_SUSP_TO_TIME+1,close_brace);
	mvaddstr(L_ATTR_ROW+1,L_SUSP_RSN_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_SUSP_RSN_COL+MC_SP_RSN+1,close_brace);
	mvaddstr(L_ATTR_ROW+1,L_SUSP_PASS_COL+3,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_SUSP_PASS_COL+3+MC_SP_PASS+1,close_brace);
	mvaddstr(L_ATTR_ROW+2,L_SUSP_DEL_COL+3,open_brace);
	mvaddstr(L_ATTR_ROW+2,L_SUSP_DEL_COL+3+MC_SP_DEL+1,close_brace);
	mvaddstr(L_ATTR_ROW+3,L_SUSP_TPX_COL+3,open_brace);
	mvaddstr(L_ATTR_ROW+3,L_SUSP_TPX_COL+3+MC_SP_TPX+1,close_brace);
	attrset(A_BOLD);
	mvaddstr(L_SUSP_MSG_ROW,L_SUSP_MSG_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_108, "Suspension Message"));
	attrset(0);
	mvaddstr(L_SUSP_MSG_ROW+1,L_SUSP_MSG_COL,open_brace);
	mvaddstr(L_SUSP_MSG_ROW+1,L_SUSP_MSG_COL+MC_MSG_1+1,close_brace);
	attrset(A_BOLD);
	mvaddstr(L_CMNT_ROW,L_CMNT_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_109, "Driver Comments"));
	mvaddstr(L_CMNT_ROW,L_DRV_L_RNW_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_110, "Lic Renew"));
	mvaddstr(L_CMNT_ROW,L_TXI_L_RNW_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_111, "Taxi Lic Renew"));
	mvaddstr(L_CMNT_ROW,L_ID_SERIAL_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_142, "ID Serial Nbr"));
	//mvaddstr(L_CMNT_ROW,L_ID_SERIAL_COL,"ID Serial Nbr");
	attrset(0);
	mvaddstr(L_CMNT_ROW+1,L_CMNT_COL,open_brace);
	mvaddstr(L_CMNT_ROW+1,L_CMNT_COL+MC_COMMENT+1,close_brace);
	mvaddstr(L_CMNT_ROW+1,L_DRV_L_RNW_COL,open_brace);
	mvaddstr(L_CMNT_ROW+1,L_DRV_L_RNW_COL+MC_LIC_RNW_DATE+1,close_brace);
	mvaddstr(L_CMNT_ROW+1,L_TXI_L_RNW_COL,open_brace);
	mvaddstr(L_CMNT_ROW+1,L_TXI_L_RNW_COL+MC_TXI_LIC_RNW_DATE+1,close_brace);
	mvaddstr(L_CMNT_ROW+1,L_ID_SERIAL_COL,open_brace);
	mvaddstr(L_CMNT_ROW+1,L_ID_SERIAL_COL+MC_ID_SERIAL+1,close_brace);
	attrset(A_BOLD);
	mvaddstr(L_LST_EMRG_ROW,L_EMRG_ON_DATE_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_112, "Last Emergency On"));
	mvaddstr(L_LST_EMRG_ROW,L_EMRG_OFF_DATE_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_113, "Emergency Resolved"));
	mvaddstr(L_LST_EMRG_ROW,L_SGN_ON_DATE_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_114, "Signed On"));
	mvaddstr(L_LST_EMRG_ROW,L_SGN_OFF_DATE_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_115, "Signed Off"));
	mvaddstr(L_LST_EMRG_ROW,L_LST_VEH_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_116, "Veh #"));
	mvaddstr(L_LST_EMRG_ROW,L_LST_CALL_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_117, "Call Serviced"));
	attrset(0);
	mvaddstr(L_LST_EMRG_ROW+1,L_EMRG_ON_DATE_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_EMRG_ON_DATE_COL+MC_EMRG_ON_DATE+1,close_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_EMRG_ON_TIME_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_EMRG_ON_TIME_COL+MC_EMRG_ON_TIME+1,close_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_EMRG_OFF_DATE_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_EMRG_OFF_DATE_COL+MC_EMRG_OFF_DATE+1,close_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_EMRG_OFF_TIME_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_EMRG_OFF_TIME_COL+MC_EMRG_OFF_TIME+1,close_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_SGN_ON_DATE_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_SGN_ON_DATE_COL+MC_SGN_ON_DATE+1,close_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_SGN_ON_TIME_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_SGN_ON_TIME_COL+MC_SGN_ON_TIME+1,close_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_SGN_OFF_DATE_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_SGN_OFF_DATE_COL+MC_SGN_OFF_DATE+1,close_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_SGN_OFF_TIME_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_SGN_OFF_TIME_COL+MC_SGN_OFF_TIME+1,close_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_LST_VEH_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_LST_VEH_COL+MC_LST_VEH+1,close_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_LST_CALL_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_LST_CALL_COL+MC_LST_CALL+1,close_brace);
	attrset(A_BOLD);
	mvaddstr(L_POSTS_ROW,L_POSTS_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_118, "BOOK"));
	mvaddstr(L_POSTS_ROW,L_CND_POSTS_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_119, "CBOOK"));
	mvaddstr(L_POSTS_ROW,L_ENR_POSTS_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_120, "EBOOK"));
	mvaddstr(L_POSTS_ROW,L_N_ACPTS_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_121, "no ACPT"));
	mvaddstr(L_POSTS_ROW,L_REJECTS_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_122, "REJCT"));
	mvaddstr(L_POSTS_ROW,L_CL_OUTS_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_123, "C-OUT"));
	mvaddstr(L_POSTS_ROW,L_NO_SHWS_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_124, "NTRIP"));
	mvaddstr(L_POSTS_ROW,L_FLAGS_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_125, "FLAG"));
	mvaddstr(L_POSTS_ROW,L_BIDS_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_126, "BID"));
	mvaddstr(L_POSTS_ROW,L_RQ_TLK_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_127, "TALK"));
	mvaddstr(L_POSTS_ROW,L_MSGS_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_128, "MSG"));
	mvaddstr(L_POSTS_ROW,L_BREAKS_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_129, "BREAK"));
	mvaddstr(L_POSTS_ROW,L_ASSGN_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_130, "Assigns"));
	mvaddstr(L_POSTS_ROW,L_CALLS_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_131, "Calls"));
	mvaddstr(L_POSTS_ROW,L_SH_MTRS_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_132, "Short M"));
	mvaddstr(L_POSTS_ROW,L_LT_MTRS_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_133, "Late M"));
	mvaddstr(L_POSTS_ROW,L_CL_BCKS_COL,catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_134, "Callback"));
	attrset(0);
	mvaddstr(L_POSTS_ROW+1,L_POSTS_COL,open_brace);
	mvaddstr(L_POSTS_ROW+1,L_POSTS_COL+MC_POSTS+1,close_brace);
	mvaddstr(L_POSTS_ROW+1,L_CND_POSTS_COL,open_brace);
	mvaddstr(L_POSTS_ROW+1,L_CND_POSTS_COL+MC_CND_POSTS+1,close_brace);
	mvaddstr(L_POSTS_ROW+1,L_ENR_POSTS_COL,open_brace);
	mvaddstr(L_POSTS_ROW+1,L_ENR_POSTS_COL+MC_ENR_POSTS+1,close_brace);
	mvaddstr(L_POSTS_ROW+1,L_N_ACPTS_COL,open_brace);
	mvaddstr(L_POSTS_ROW+1,L_N_ACPTS_COL+MC_NO_ACCEPTS+1,close_brace);
	mvaddstr(L_POSTS_ROW+1,L_REJECTS_COL,open_brace);
	mvaddstr(L_POSTS_ROW+1,L_REJECTS_COL+MC_REJECTS+1,close_brace);
	mvaddstr(L_POSTS_ROW+1,L_CL_OUTS_COL,open_brace);
	mvaddstr(L_POSTS_ROW+1,L_CL_OUTS_COL+MC_CL_OUTS+1,close_brace);
	mvaddstr(L_POSTS_ROW+1,L_NO_SHWS_COL,open_brace);
	mvaddstr(L_POSTS_ROW+1,L_NO_SHWS_COL+MC_NO_SHOWS+1,close_brace);
	mvaddstr(L_POSTS_ROW+1,L_FLAGS_COL,open_brace);
	mvaddstr(L_POSTS_ROW+1,L_FLAGS_COL+MC_FLAGS+1,close_brace);
	mvaddstr(L_POSTS_ROW+1,L_BIDS_COL,open_brace);
	mvaddstr(L_POSTS_ROW+1,L_BIDS_COL+MC_BIDS+1,close_brace);
	mvaddstr(L_POSTS_ROW+1,L_RQ_TLK_COL,open_brace);
	mvaddstr(L_POSTS_ROW+1,L_RQ_TLK_COL+MC_RQ_TO_TALK+1,close_brace);
	mvaddstr(L_POSTS_ROW+1,L_MSGS_COL,open_brace);
	mvaddstr(L_POSTS_ROW+1,L_MSGS_COL+MC_MSGS+1,close_brace);
	mvaddstr(L_POSTS_ROW+1,L_BREAKS_COL,open_brace);
	mvaddstr(L_POSTS_ROW+1,L_BREAKS_COL+MC_BREAKS+1,close_brace);
	mvaddstr(L_POSTS_ROW+1,L_ASSGN_COL,open_brace);
	mvaddstr(L_POSTS_ROW+1,L_ASSGN_COL+MC_ASSIGNS+1,close_brace);
	mvaddstr(L_POSTS_ROW+1,L_CALLS_COL,open_brace);
	mvaddstr(L_POSTS_ROW+1,L_CALLS_COL+MC_CALLS+1,close_brace);
	mvaddstr(L_POSTS_ROW+1,L_SH_MTRS_COL,open_brace);
	mvaddstr(L_POSTS_ROW+1,L_SH_MTRS_COL+MC_SH_METERS+1,close_brace);
	mvaddstr(L_POSTS_ROW+1,L_LT_MTRS_COL,open_brace);
	mvaddstr(L_POSTS_ROW+1,L_LT_MTRS_COL+MC_LT_METERS+1,close_brace);
	mvaddstr(L_POSTS_ROW+1,L_CL_BCKS_COL,open_brace);
	mvaddstr(L_POSTS_ROW+1,L_CL_BCKS_COL+MC_CL_BACKS+1,close_brace);

	refresh();

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_labels\n");
	TRACE(trace_str);
#endif

}  /* End of prt_labels  */

/******************************************************************************
* prt_data - print the data on the screen.
*******************************************************************************/

prt_data()
{

	int i;			/* loop index */
	struct tm *tm_ptr;      /* pointer to a unix time structure */
	time_t tmp_time;
	char AttributesSet;
	short Fleet_nbr;
	int AttrCol = 0, AttrRow = 0;
	extern struct fleets *fleet[];

#ifdef DEBUG
	sprintf(trace_str,"Entering prt_data\n");
	TRACE(trace_str);
#endif

	/* Make sure all values are reasonable */
	if (dr_ptr->id < 0)
	    dr_ptr->id = 0;
	if (dr_ptr->susp_from_dt_tm < 0)
	    dr_ptr->susp_from_dt_tm = 0;
	if (dr_ptr->susp_to_dt_tm < 0)
	    dr_ptr->susp_to_dt_tm= 0;
	if (dr_ptr->susp_d_from_dt_tm < 0)
	    dr_ptr->susp_d_from_dt_tm = 0;
	if (dr_ptr->susp_d_to_dt_tm < 0)
	    dr_ptr->susp_d_to_dt_tm= 0;
	if (dr_ptr->susp_x_from_dt_tm < 0)
	    dr_ptr->susp_x_from_dt_tm = 0;
	if (dr_ptr->susp_x_to_dt_tm < 0)
	    dr_ptr->susp_x_to_dt_tm= 0;
	if (dr_ptr->emerg_on_dt_tm < 0)
	    dr_ptr->emerg_on_dt_tm = 0;
	if (dr_ptr->emerg_of_dt_tm < 0)
	    dr_ptr->emerg_of_dt_tm = 0;
	if (dr_ptr->signed_on_dt_tm < 0)
	    dr_ptr->signed_on_dt_tm = 0;
	if (dr_ptr->signed_of_dt_tm < 0)
	    dr_ptr->signed_of_dt_tm = 0;
	if (dr_ptr->last_veh_nbr < 0)
	    dr_ptr->last_veh_nbr = 0;
	if (dr_ptr->last_call < 0)
	    dr_ptr->last_call = 0;
	if (dr_ptr->assigns < 0)
	    dr_ptr->assigns = 0;
	if (dr_ptr->calls < 0)
	    dr_ptr->calls = 0;
	if (dr_ptr->flags < 0)
	    dr_ptr->flags = 0;
	if (dr_ptr->bids < 0)
	    dr_ptr->bids = 0;
	if (dr_ptr->posts < 0)
	    dr_ptr->posts = 0;
	if (dr_ptr->cond_posts < 0)
	    dr_ptr->cond_posts = 0;
	if (dr_ptr->enroute_posts < 0)
	    dr_ptr->enroute_posts = 0;
	if (dr_ptr->short_meters < 0)
	    dr_ptr->short_meters = 0;
	if (dr_ptr->late_meters < 0)
	    dr_ptr->late_meters = 0;
	if (dr_ptr->no_accepts < 0)
	    dr_ptr->no_accepts = 0;
	if (dr_ptr->rejects < 0)
	    dr_ptr->rejects = 0;
	if (dr_ptr->breaks < 0)
	    dr_ptr->breaks = 0;
	if (dr_ptr->req_to_talk < 0)
	    dr_ptr->req_to_talk = 0;
	if (dr_ptr->messages < 0)
	    dr_ptr->messages = 0;
	if (dr_ptr->callbacks < 0)
	    dr_ptr->callbacks = 0;
	if (dr_ptr->callouts < 0)
	    dr_ptr->callouts = 0;
	if (dr_ptr->no_shows < 0)
	    dr_ptr->no_shows = 0;


	/* Fill in the data items on the screen */

	mvprintw(D_FLEET_ROW,D_FLEET_COL,"%c",dr_ptr->fleet);
	mvprintw(D_FLEET_ROW,D_DR_ID_COL,"%6d",dr_ptr->id);
	mvprintw(D_FLEET_ROW,D_CLASS_COL,"%c",dr_ptr->class);
	mvprintw(D_FLEET_ROW,D_NAME_COL,"%s",dr_ptr->name);
	mvprintw(D_FLEET_ROW,D_ADD_COL,"%s",dr_ptr->address);
	mvprintw(D_FLEET_ROW,D_CITY_COL,"%s",dr_ptr->city);
	mvprintw(D_FLEET_ROW,D_ZIP_COL,"%s",dr_ptr->zip);
	mvprintw(D_FLEET_ROW,D_ZSFX_COL,"%s",dr_ptr->zip_suf);
	mvprintw(D_PHONE_ROW,D_PHONE_COL,"%s",dr_ptr->phone);
	mvprintw(D_PHONE_ROW,D_LIC_COL,"%s",dr_ptr->license_nbr);
	mvprintw(D_PHONE_ROW,D_TXI_LIC_COL,"%s",dr_ptr->taxi_license);
	mvprintw(D_PHONE_ROW,D_SEAL_1_COL,"%c",dr_ptr->seals[0]);
	mvprintw(D_PHONE_ROW,D_SEAL_2_COL,"%c",dr_ptr->seals[1]);
	mvprintw(D_PHONE_ROW,D_SEAL_3_COL,"%c",dr_ptr->seals[2]);
	mvprintw(D_PHONE_ROW,D_SEAL_4_COL,"%c",dr_ptr->seals[3]);
	mvprintw(D_PHONE_ROW,D_SEAL_5_COL,"%c",dr_ptr->seals[4]);
	mvprintw(D_PHONE_ROW,D_SEAL_6_COL,"%c",dr_ptr->seals[5]);
	mvprintw(D_PHONE_ROW,D_SEAL_7_COL,"%c",dr_ptr->seals[6]);
	mvprintw(D_PHONE_ROW,D_SEAL_8_COL,"%c",dr_ptr->seals[7]);
	mvprintw(D_PHONE_ROW,D_SEAL_9_COL,"%c",dr_ptr->seals[8]);
	mvprintw(D_PHONE_ROW,D_SEAL_10_COL,"%c",dr_ptr->seals[9]);
	mvprintw(D_PHONE_ROW,D_SEAL_11_COL,"%c",dr_ptr->seals[10]);
	mvprintw(D_PHONE_ROW,D_SEAL_12_COL,"%c",dr_ptr->seals[11]);
	mvprintw(D_PHONE_ROW,D_SEAL_13_COL,"%c",dr_ptr->seals[12]);
	mvprintw(D_PHONE_ROW,D_SEAL_14_COL,"%c",dr_ptr->seals[13]);
	mvprintw(D_PHONE_ROW,D_SEAL_15_COL,"%c",dr_ptr->seals[14]);
	mvprintw(D_PHONE_ROW,D_SEAL_16_COL,"%c",dr_ptr->seals[15]);
	
	AttrCol = 1;
	AttrRow = 1;
	if ( dr_ptr->fleet == 'H' )
	  Fleet_nbr = 0;
	else
	  Fleet_nbr = dr_ptr->fleet - 'A';
	
	for ( i = 0; i < MaxAttrs; i++ )
	  {
	    if ( dr_ptr->attributes[i] == YES )
	      {
		if ( fleet[Fleet_nbr]->vd_attr_info[i].drv_attr[0] != '\0' )
		  {
		    mvaddstr( D_ATTR_ROW + AttrRow, AttrCol, fleet[Fleet_nbr]->vd_attr_info[i].drv_attr );
		    AttrCol += 3;
		    if ( AttrCol >= L_SUSP_PASS_COL - 2 )
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
	
	if(!dr_ptr->susp_from_dt_tm)  {
		mvprintw(D_SUSP_ROW,D_SP_FM_DATE_COL, "00%c00%c00", DATE_SEPERATOR, DATE_SEPERATOR);
		mvprintw(D_SUSP_ROW,D_SP_FM_TIME_COL,"00:00");
	}
	else  {
	  tmp_time = (time_t)dr_ptr->susp_from_dt_tm;
	  tm_ptr = localtime(&tmp_time);
		if ( tm_ptr->tm_year >= 100 )
		  tm_ptr->tm_year -= 100;
		if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		    mvprintw(D_SUSP_ROW,D_SP_FM_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_year);
		else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
		    mvprintw(D_SUSP_ROW,D_SP_FM_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_year, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_mday);
		else
		    mvprintw(D_SUSP_ROW,D_SP_FM_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mon + 1, DATE_SEPERATOR, tm_ptr->tm_mday,
			     DATE_SEPERATOR,tm_ptr->tm_year);
#ifdef HR24
		mvprintw(D_SUSP_ROW,D_SP_FM_TIME_COL,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#else
		if(tm_ptr->tm_hour >= 12)  {
			tm_ptr->tm_hour -= 12;
			mvprintw(D_SUSP_ROW,D_SP_FM_TIME_COL,"%02d:%02dp",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
		}
		else
			mvprintw(D_SUSP_ROW,D_SP_FM_TIME_COL,"%02d:%02da",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
#endif
	}

	if(!dr_ptr->susp_to_dt_tm)  {
		mvprintw(D_SUSP_ROW,D_SP_TO_DATE_COL, "00%c00%c00", DATE_SEPERATOR, DATE_SEPERATOR);
		mvaddstr(D_SUSP_ROW,D_SP_TO_TIME_COL, "00:00");
	}

	else {
	  tmp_time = (time_t)dr_ptr->susp_to_dt_tm;
	  tm_ptr = localtime(&tmp_time);	  

		if ( tm_ptr->tm_year >= 100 )
		  tm_ptr->tm_year -= 100;		
		if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		    mvprintw(D_SUSP_ROW,D_SP_TO_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_year);
		else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
		    mvprintw(D_SUSP_ROW,D_SP_TO_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_year, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_mday);
		else
		    mvprintw(D_SUSP_ROW,D_SP_TO_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mon + 1, DATE_SEPERATOR, tm_ptr->tm_mday,
			     DATE_SEPERATOR,tm_ptr->tm_year);
#ifdef HR24
		mvprintw(D_SUSP_ROW,D_SP_TO_TIME_COL,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#else
		if(tm_ptr->tm_hour >= 12)  {
			tm_ptr->tm_hour -= 12;
			mvprintw(D_SUSP_ROW,D_SP_TO_TIME_COL,"%02d:%02dp",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
		}
		else
			mvprintw(D_SUSP_ROW,D_SP_TO_TIME_COL,"%02d:%02da",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
#endif
	}

	if(!dr_ptr->susp_d_from_dt_tm)  {
		mvprintw(D_SUSP_ROW+1,D_SP_FM_DATE_COL, "00%c00%c00", DATE_SEPERATOR, DATE_SEPERATOR);
		mvprintw(D_SUSP_ROW+1,D_SP_FM_TIME_COL,"00:00");
	}
	else  {
	  tmp_time = (time_t)dr_ptr->susp_d_from_dt_tm;
	  tm_ptr = localtime(&tmp_time);	  

		if ( tm_ptr->tm_year >= 100 )
		  tm_ptr->tm_year -= 100;		
		if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		    mvprintw(D_SUSP_ROW+1,D_SP_FM_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_year);
		else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
		    mvprintw(D_SUSP_ROW+1,D_SP_FM_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_year, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_mday);
		else
		    mvprintw(D_SUSP_ROW+1,D_SP_FM_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mon + 1, DATE_SEPERATOR, tm_ptr->tm_mday,
			     DATE_SEPERATOR,tm_ptr->tm_year);
#ifdef HR24
		mvprintw(D_SUSP_ROW+1,D_SP_FM_TIME_COL,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#else
		if(tm_ptr->tm_hour >= 12)  {
			tm_ptr->tm_hour -= 12;
			mvprintw(D_SUSP_ROW+1,D_SP_FM_TIME_COL,"%02d:%02dp",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
		}
		else
			mvprintw(D_SUSP_ROW+1,D_SP_FM_TIME_COL,"%02d:%02da",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
#endif
	}

	if(!dr_ptr->susp_d_to_dt_tm)  {
		mvprintw(D_SUSP_ROW+1,D_SP_TO_DATE_COL, "00%c00%c00", DATE_SEPERATOR, DATE_SEPERATOR);
		mvaddstr(D_SUSP_ROW+1,D_SP_TO_TIME_COL, "00:00");
	}
	else {
	  tmp_time = (time_t)dr_ptr->susp_d_to_dt_tm;
	  tm_ptr = localtime(&tmp_time);	  

		if ( tm_ptr->tm_year >= 100 )
		  tm_ptr->tm_year -= 100;		
		if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		    mvprintw(D_SUSP_ROW+1,D_SP_TO_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_year);
		else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
		    mvprintw(D_SUSP_ROW+1,D_SP_TO_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_year, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_mday);
		else
		    mvprintw(D_SUSP_ROW+1,D_SP_TO_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mon + 1, DATE_SEPERATOR, tm_ptr->tm_mday,
			     DATE_SEPERATOR,tm_ptr->tm_year);
#ifdef HR24
		mvprintw(D_SUSP_ROW+1,D_SP_TO_TIME_COL,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#else
		if(tm_ptr->tm_hour >= 12)  {
			tm_ptr->tm_hour -= 12;
			mvprintw(D_SUSP_ROW+1,D_SP_TO_TIME_COL,"%02d:%02dp",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
		}
		else
			mvprintw(D_SUSP_ROW+1,D_SP_TO_TIME_COL,"%02d:%02da",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
#endif
	}

	if(!dr_ptr->susp_x_from_dt_tm)  {
		mvprintw(D_SUSP_ROW+2,D_SP_FM_DATE_COL, "00%c00%c00", DATE_SEPERATOR, DATE_SEPERATOR);
		mvprintw(D_SUSP_ROW+2,D_SP_FM_TIME_COL,"00:00");
	}
	else  {
	  tmp_time = (time_t)dr_ptr->susp_x_from_dt_tm;
	  tm_ptr = localtime(&tmp_time);

		if ( tm_ptr->tm_year >= 100 )
		  tm_ptr->tm_year -= 100;		
		if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		    mvprintw(D_SUSP_ROW+2,D_SP_FM_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_year);
		else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
		    mvprintw(D_SUSP_ROW+2,D_SP_FM_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_year, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_mday);
		else
		    mvprintw(D_SUSP_ROW+2,D_SP_FM_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mon + 1, DATE_SEPERATOR, tm_ptr->tm_mday,
			     DATE_SEPERATOR,tm_ptr->tm_year);
#ifdef HR24
		mvprintw(D_SUSP_ROW+2,D_SP_FM_TIME_COL,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#else
		if(tm_ptr->tm_hour >= 12)  {
			tm_ptr->tm_hour -= 12;
			mvprintw(D_SUSP_ROW+2,D_SP_FM_TIME_COL,"%02d:%02dp",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
		}
		else
			mvprintw(D_SUSP_ROW+2,D_SP_FM_TIME_COL,"%02d:%02da",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
#endif
	}

	if(!dr_ptr->susp_x_to_dt_tm)  {
		mvprintw(D_SUSP_ROW+2,D_SP_TO_DATE_COL, "00%c00%c00", DATE_SEPERATOR, DATE_SEPERATOR);
		mvaddstr(D_SUSP_ROW+2,D_SP_TO_TIME_COL, "00:00");
	}
	else {
	  tmp_time = (time_t)dr_ptr->susp_x_to_dt_tm;
	  tm_ptr = localtime(&tmp_time);	  

		if ( tm_ptr->tm_year >= 100 )
		  tm_ptr->tm_year -= 100;		
		if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		    mvprintw(D_SUSP_ROW+2,D_SP_TO_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_year);
		else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
		    mvprintw(D_SUSP_ROW+2,D_SP_TO_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_year, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_mday);
		else
		    mvprintw(D_SUSP_ROW+2,D_SP_TO_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mon + 1, DATE_SEPERATOR, tm_ptr->tm_mday,
			     DATE_SEPERATOR,tm_ptr->tm_year);
#ifdef HR24
		mvprintw(D_SUSP_ROW+2,D_SP_TO_TIME_COL,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#else
		if(tm_ptr->tm_hour >= 12)  {
			tm_ptr->tm_hour -= 12;
			mvprintw(D_SUSP_ROW+2,D_SP_TO_TIME_COL,"%02d:%02dp",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
		}
		else
			mvprintw(D_SUSP_ROW+2,D_SP_TO_TIME_COL,"%02d:%02da",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
#endif
	}

	mvprintw(D_SUSP_ROW,D_SUSP_RSN_COL,"%s",dr_ptr->susp_reason);
	mvprintw(D_SUSP_ROW,D_SUSP_PASS_COL,"%c",dr_ptr->susp_pass_call);
	mvprintw(D_SUSP_ROW+1,D_SUSP_DEL_COL,"%c",dr_ptr->susp_del_call);
	mvprintw(D_SUSP_ROW+2,D_SUSP_TPX_COL,"%c",dr_ptr->susp_typex_call);

	if(dr_ptr->susp_message)
		mvprintw(D_SUSP_MSG_1_ROW,D_SUSP_MSG_1_COL,"%s",dr_ptr->susp_message);
	else  {
		move(D_SUSP_MSG_1_ROW,D_SUSP_MSG_1_COL);
		for(i = 0 ; i < MC_MSG_1 ; i++)
			addch(' ');
	}
	if(dr_ptr->comments)
		mvprintw(D_CMNT_ROW,D_CMNT_COL,"%s",dr_ptr->comments);
	else  {
		move(D_CMNT_ROW,D_CMNT_COL);
		for(i = 0 ; i < MC_COMMENT ; i++)
			addch(' ');
	}
	
	if(dr_ptr->lic_renew)
		mvprintw(D_CMNT_ROW,D_DRV_L_RNW_COL,"%s",dr_ptr->lic_renew);
	else
		mvprintw(D_CMNT_ROW,D_DRV_L_RNW_COL, "00%c00%c00", DATE_SEPERATOR, DATE_SEPERATOR);
	if(dr_ptr->taxi_lic_renew)
		mvprintw(D_CMNT_ROW,D_TXI_L_RNW_COL,"%s",dr_ptr->taxi_lic_renew);
	else
		mvprintw(D_CMNT_ROW,D_TXI_L_RNW_COL, "00%c00%c00", DATE_SEPERATOR, DATE_SEPERATOR);

	mvprintw(D_CMNT_ROW,D_ID_SERIAL_COL, "%c", dr_ptr->serial_nbr);
	
	if(!dr_ptr->emerg_on_dt_tm)  {
		mvprintw(D_EMRG_ROW,D_EMRG_ON_DATE_COL, "00%c00%c00", DATE_SEPERATOR, DATE_SEPERATOR);
		mvprintw(D_EMRG_ROW,D_EMRG_ON_TIME_COL,"00:00");
	}
	else  {
	  tmp_time = (time_t)dr_ptr->emerg_on_dt_tm;
	  tm_ptr = localtime(&tmp_time);	  	  

		if ( tm_ptr->tm_year >= 100 )
		  tm_ptr->tm_year -= 100;		
		if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		    mvprintw(D_EMRG_ROW,D_EMRG_ON_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_year);
		else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
			mvprintw(D_EMRG_ROW,D_EMRG_ON_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_year, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_mday);
		else
		    mvprintw(D_EMRG_ROW,D_EMRG_ON_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mon + 1, DATE_SEPERATOR, tm_ptr->tm_mday,
			     DATE_SEPERATOR,tm_ptr->tm_year);
#ifdef HR24
		mvprintw(D_EMRG_ROW,D_EMRG_ON_TIME_COL,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#else
		if(tm_ptr->tm_hour >= 12)  {
			tm_ptr->tm_hour -= 12;
			mvprintw(D_EMRG_ROW,D_EMRG_ON_TIME_COL,"%02d:%02dp",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
		}
		else
			mvprintw(D_EMRG_ROW,D_EMRG_ON_TIME_COL,"%02d:%02da",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
#endif
	}

	if(!dr_ptr->emerg_of_dt_tm)  {
		mvprintw(D_EMRG_ROW,D_EMRG_OFF_DATE_COL, "00%c00%c00", DATE_SEPERATOR, DATE_SEPERATOR);
		mvprintw(D_EMRG_ROW,D_EMRG_OFF_TIME_COL,"00:00");
	}
	else  {
	  tmp_time = (time_t)dr_ptr->emerg_of_dt_tm;
	  tm_ptr = localtime(&tmp_time);	  	  	  

		if ( tm_ptr->tm_year >= 100 )
		  tm_ptr->tm_year -= 100;		
		if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		    mvprintw(D_EMRG_ROW,D_EMRG_OFF_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_year);
		else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
		    mvprintw(D_EMRG_ROW,D_EMRG_OFF_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_year, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_mday);
		else
		    mvprintw(D_EMRG_ROW,D_EMRG_OFF_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mon + 1, DATE_SEPERATOR, tm_ptr->tm_mday,
			     DATE_SEPERATOR,tm_ptr->tm_year);
#ifdef HR24
		mvprintw(D_EMRG_ROW,D_EMRG_OFF_TIME_COL,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#else
		if(tm_ptr->tm_hour >= 12)  {
			tm_ptr->tm_hour -= 12;
			mvprintw(D_EMRG_ROW,D_EMRG_OFF_TIME_COL,"%02d:%02dp",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
		}
		else
			mvprintw(D_EMRG_ROW,D_EMRG_OFF_TIME_COL,"%02d:%02da",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
#endif
	}

	if(!dr_ptr->signed_on_dt_tm)  {
		mvprintw(D_EMRG_ROW,D_SGN_ON_DATE_COL, "00%c00%c00", DATE_SEPERATOR, DATE_SEPERATOR);
		mvprintw(D_EMRG_ROW,D_SGN_ON_TIME_COL,"00:00");
	}
	else  {
	  tmp_time = (time_t)dr_ptr->signed_on_dt_tm;
	  tm_ptr = localtime(&tmp_time);	  
		if ( tm_ptr->tm_year >= 100 )
		  tm_ptr->tm_year -= 100;		
		if (DATE_FORMAT == DAY_FIRST_IN_DATE)
			mvprintw(D_EMRG_ROW,D_SGN_ON_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_year);
		else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
			mvprintw(D_EMRG_ROW,D_SGN_ON_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_year, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_mday);
		else
		    mvprintw(D_EMRG_ROW,D_SGN_ON_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mon + 1, DATE_SEPERATOR, tm_ptr->tm_mday,
			     DATE_SEPERATOR,tm_ptr->tm_year);
#ifdef HR24
		mvprintw(D_EMRG_ROW,D_SGN_ON_TIME_COL,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#else
		if(tm_ptr->tm_hour >= 12)  {
			tm_ptr->tm_hour -= 12;
			mvprintw(D_EMRG_ROW,D_SGN_ON_TIME_COL,"%02d:%02dp",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
	}
	else
			mvprintw(D_EMRG_ROW,D_SGN_ON_TIME_COL,"%02d:%02da",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
#endif
	}
	
	if(!dr_ptr->signed_of_dt_tm)  {
		mvprintw(D_EMRG_ROW,D_SGN_OFF_DATE_COL, "00%c00%c00", DATE_SEPERATOR, DATE_SEPERATOR);
		mvprintw(D_EMRG_ROW,D_SGN_OFF_TIME_COL,"00:00");
	}
	else  {
	  tmp_time = (time_t)dr_ptr->signed_of_dt_tm;
	  tm_ptr = localtime(&tmp_time);
		if ( tm_ptr->tm_year >= 100 )
		  tm_ptr->tm_year -= 100;		
		if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		    mvprintw(D_EMRG_ROW,D_SGN_OFF_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_year);
		else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
		    mvprintw(D_EMRG_ROW,D_SGN_OFF_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_year, DATE_SEPERATOR, tm_ptr->tm_mon+1,
			     DATE_SEPERATOR,tm_ptr->tm_mday);
		else
		    mvprintw(D_EMRG_ROW,D_SGN_OFF_DATE_COL,
			     "%02d%c%02d%c%02d",
			     tm_ptr->tm_mon + 1, DATE_SEPERATOR, tm_ptr->tm_mday,
			     DATE_SEPERATOR,tm_ptr->tm_year);
#ifdef HR24
		mvprintw(D_EMRG_ROW,D_SGN_OFF_TIME_COL,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#else
		if(tm_ptr->tm_hour >= 12)  { 
			tm_ptr->tm_hour -= 12; 
			mvprintw(D_EMRG_ROW,D_SGN_OFF_TIME_COL,"%02d:%02dp",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
		}
		else 
			mvprintw(D_EMRG_ROW,D_SGN_OFF_TIME_COL,"%02d:%02da",
				 tm_ptr->tm_hour,tm_ptr->tm_min);
#endif
	    }

	mvprintw(D_EMRG_ROW,D_LST_VEH_COL,"%4d",dr_ptr->last_veh_nbr);
	mvprintw(D_EMRG_ROW,D_LST_CALL_COL,"%18d",dr_ptr->last_call);
	mvprintw(D_POSTS_ROW,D_POSTS_COL,"%4d",dr_ptr->posts);
	mvprintw(D_POSTS_ROW,D_CND_POSTS_COL,"%4d",dr_ptr->cond_posts);
	mvprintw(D_POSTS_ROW,D_ENR_POSTS_COL,"%4d",dr_ptr->enroute_posts);
	mvprintw(D_POSTS_ROW,D_N_ACPTS_COL,"%4d",dr_ptr->no_accepts);
	mvprintw(D_POSTS_ROW,D_REJECTS_COL,"%4d",dr_ptr->rejects);
	mvprintw(D_POSTS_ROW,D_CL_OUTS_COL,"%4d",dr_ptr->callouts);
	mvprintw(D_POSTS_ROW,D_NO_SHWS_COL,"%4d",dr_ptr->no_shows);
	mvprintw(D_POSTS_ROW,D_FLAGS_COL,"%4d",dr_ptr->flags);
	mvprintw(D_POSTS_ROW,D_BIDS_COL,"%4d",dr_ptr->bids);
	mvprintw(D_POSTS_ROW,D_RQ_TLK_COL,"%4d",dr_ptr->req_to_talk);
	mvprintw(D_POSTS_ROW,D_MSGS_COL,"%4d",dr_ptr->messages);
	mvprintw(D_POSTS_ROW,D_BREAKS_COL,"%4d",dr_ptr->breaks);
	mvprintw(D_POSTS_ROW,D_ASSGN_COL,"%4d",dr_ptr->assigns);
	mvprintw(D_POSTS_ROW,D_CALLS_COL,"%4d",dr_ptr->calls);
	mvprintw(D_POSTS_ROW,D_SH_MTRS_COL,"%4d",dr_ptr->short_meters);
	mvprintw(D_POSTS_ROW,D_LT_MTRS_COL,"%4d",dr_ptr->late_meters);
	mvprintw(D_POSTS_ROW,D_CL_BCKS_COL,"%4d",dr_ptr->callbacks);

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

send_ipc()
{
	int qid, key = MSGKEY;

#ifdef DEBUG
	sprintf(trace_str,"Entering send_ipc\n");
	TRACE(trace_str);
#endif

	df_maint_ipc.d_prcs_id = DISP_DEST;
	df_maint_ipc.u_prcs_id = getpid();
	df_maint_ipc.rec_type = DFM_REQ;		
	df_maint_ipc.which_file = DRIVER_FILE_ID;
	df_maint_ipc.fleet = dr_ptr->fleet; 
	df_maint_ipc.key = dr_ptr->id; 
	df_maint_ipc.cisam_num = isrecnum;         
	if((qid = msgget(key, IPC_CREAT | 0666)) == -1)   {
		prt_error(DRIVER_GET_ERR);
		prt_unix_err(errno,DRIVER_GET_ERR);
		sleep(3);
		ERROR(' ',DRIVER_FILE,DRIVER_GET_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving send_ipc - 1\n");
	TRACE(trace_str);
#endif
		cleanup();
	}

	if((msgsnd(qid,&df_maint_ipc,sizeof(df_maint_ipc), IPC_NOWAIT)) < 0)  {
		prt_error(DRIVER_SND_ERR);
		prt_unix_err(errno,DRIVER_SND_ERR);
		sleep(3);
		ERROR(' ',DRIVER_FILE,DRIVER_SND_ERR);
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

clear_flds()
{
	int i;		/* loop index */

#ifdef DEBUG
	sprintf(trace_str,"Entering clear_flds\n");
	TRACE(trace_str);
#endif

	for(i = 1; i <= NUM_FLDS; i++) 
		dr_cl_flds(i);
	mvaddstr( D_ATTR_ROW + 1, 0, "                                             " );
	mvaddstr( D_ATTR_ROW + 2, 0, "                                             " );

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
	  mvaddstr(HDR_ROW_1,HDR_COL_1,MAIN_HDR);
	else
	  mvaddstr(HDR_ROW_1,HDR_COL_1,"Query Next Previous Output Exit");
	attrset(A_BOLD);
	mvaddstr(HDR_ROW_2,HDR_COL_2,DRIVER_TITLE);
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

prompt_fname()
{

#ifdef DEBUG
	sprintf(trace_str,"Entering prompt_fname\n");
	TRACE(trace_str);
#endif
	
	clear_hdr_line();
	mvaddstr(HDR_ROW_1,HDR_COL_1,DRIVER_FNAME_PROMPT);
	refresh();
	getstr(o_file);
	if(!strcmp(o_file,""))
		strcpy(o_file,"dr_scr.out");

#ifdef DEBUG
	sprintf(trace_str,"Leaving prompt_fname\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prompt_fname()  */
