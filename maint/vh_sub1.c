static char sz__FILE__[]="@(#)vh_sub1.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  vh_sub1.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:48:44
* @(#)  Last delta: 12/2/94 at 11:40:10
* @(#)  SCCS File: /taxi/sccs/s.vh_sub1.c
*                                                                           *
* Copyright (c) 1991 - Motorola Seattle                                     *
*                                                                           *
*****************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <isam.h>
#include <cursesX.h>
#include <signal.h>
#include <memory.h>

#include "vh_m_strdefs.h"
#include "mad_error.h"
#include "mads_types.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "df_maint.h"
#include "mads_isam.h"
#include "ex_key.h"
#include "vh_maint.h"
#include "literals.h"
#include "tokenize.h"
#include "path.h"
#include "language.h"
#include "fleet.h"

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
extern struct cisam_vh *vh_ptr;			 /* pointer to user vehicle file structure */
extern struct cisam_vh *temp_vh_ptr; 
extern struct cisam_vh temp_vh_buf;
extern int clear_flds();			/* clear data fields on the screen */
extern int doing_add;				/* performing add operation */
extern int err_n_fld;				/* Indicates that an error exists in a field */
extern fleet_flag,vh_flag,rf_flag;
extern void prt_labels();
extern int clear_desc_line();
extern struct scr_flds vh_scr_flds[];
extern struct df_maint df_maint_ipc;		/* structure for passing information to dispatch  */
extern short ipc_required;			/* Set if a variable which resides */
void prt_hdr();
extern char zero_date[];
extern int AddUpdateDeleteAllowed();
extern MaxAttrs;

/******************************************************************************
* process_query - search for specific record in the vehicle file
*******************************************************************************/
process_query()
{
	int i;				/* loop index */
	int should_clear;

	clear_err_line();
	clear_flds();
	prt_query_hdr();		/* Print the header for the query screen */

					/*  Process users input of key fields(fleet and vehicle number).  */
	piu = F_FLEET_1;
    	ret_val = NOT_VALID;
	while(ret_val == NOT_VALID) {
		intr_allowed = 1;		/* interrupts allowed */
		if (piu == 1)
			skip_return = 1;
		else
			skip_return = 0;

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

		if(!fleet_flag && key_val != ESC) {
			prt_error(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_83, "You must enter value"));
			continue;
		}
		if(!vh_flag && key_val != ESC) {
			prt_error(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_83, "You must enter value"));
			if(key_val == KEY_UP)
				piu = 1;
			else
				piu = 3;
			ret_val = NOT_VALID;
			continue;
		}
		if(vh_flag && key_val != ESC) {
			prt_error(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_84, "Enter ESC to execute"));
			if (key_val == KEY_UP)
				piu = 1;
			else
				piu = 3;
			ret_val = NOT_VALID;
			continue;
		}
		if(key_val == ESC) {		/* Using key entered in prev step, call C-Isam routine to retrieve the record. */
			clear_err_line();
			if(!fleet_flag) {
				vh_ptr->fleet = 'A';
				vh_ptr->nbr = 0;
			}
			if(!vh_flag)
				vh_ptr->nbr = 0;
			if(db(VEHICLE_FILE_ID, READ_KEYS, &veh_key2, ISGTEQ, 0) < 0) {
				if(iserrno == ENOREC) {
					prt_error(NO_ROWS);
					clear_flds();
					cur_list = 0;				/* row in current list is invalid */
					prt_hdr();
					return;
				}
				else  {
					prt_error(READ_VEHICLE_ERR);
					sleep(3);
					ERROR(' ',VEHICLE_FILE,READ_VEHICLE_ERR);
					cleanup();
				}
			}
			record_nbr = isrecnum;					/* save record number */
			cur_list = 1;						/* valid row in current list */
        		prt_data();  		
			prt_hdr();
		}
	}
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
	if(db(VEHICLE_FILE_ID, READ_KEYS, &veh_key2, ISNEXT, 0) < 0) {
		if(iserrno == EENDFILE) {
			prt_error(NO_ROWS_DIR);
			return;
		}
		else  {
			prt_error(RD_NXT_VEHICLE_ERR);
			sleep(3);
			ERROR(' ',VEHICLE_FILE,RD_NXT_VEHICLE_ERR);
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
	if(db(VEHICLE_FILE_ID, READ_KEYS, &veh_key2, ISPREV, 0) < 0) {
		if(iserrno == EENDFILE) {
			prt_error(NO_ROWS_DIR);
			return;
		}
		else  {
			prt_error(RD_PREV_VEHICLE_ERR);
			sleep(3);
			ERROR(' ',VEHICLE_FILE,RD_PREV_VEHICLE_ERR);
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

	memset((char *)vh_ptr, '\0', sizeof(struct cisam_vh));		/* reset all the members of the vehicle file structure */

	store_def();					/* default seals/attributes to 'N' */
	store_term_type_def();			/* default for terminal type */

	piu = F_FLEET_1;				/* start with the fleet field */
	prt_add_hdr();					/* Print the header line for the the add screen.  */
	prt_labels();					/* print the screen labels  */
	prt_data();
	while(1){
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
		memcpy((char *)temp_vh_ptr,req_buf,sizeof(struct cisam_vh));
		if(db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISEQUAL,0) == 0)
		{
			prt_error(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_85, "Vehicle already exists"));
			memcpy(req_buf,(char *)temp_vh_ptr,sizeof(struct cisam_vh));
			continue;
		}

		memcpy(req_buf,(char *)temp_vh_ptr,sizeof(struct cisam_vh));
		if(db(VEHICLE_FILE_ID,READ_KEYS,&veh_key1,ISEQUAL,0) == 0)
			prt_error(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_86, "RF ID already exists"));
		else
			break;

		memcpy(req_buf,(char *)temp_vh_ptr,sizeof(struct cisam_vh));
	}							/*  Call the C-Isam add record routine.  */
	/* set GPS coordinates to valid default */
	temp_vh_ptr->gps_long = 0.0;
	temp_vh_ptr->gps_lat = 0.0;
	temp_vh_ptr->gps_state = 0;
	memcpy(req_buf,(char *)temp_vh_ptr,sizeof(struct cisam_vh));
	if(db(VEHICLE_FILE_ID, ADD_REC, &veh_key2, 0, 0) < 0) {
		prt_error(ADD_VEHICLE_ERR);
		sprintf(err, "%s(%d)", ADD_VEHICLE_ERR, iserrno);
		ERROR(' ',VEHICLE_FILE, err);
	}
	else
	{
	  /* After an ADD to db, no current record is defined.
	     Read the just added record to set the current record */
	  db(VEHICLE_FILE_ID, READ_KEYS, &veh_key2, ISEQUAL, 0);
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
	char rf_id[6];
	char err[80];

	if(db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISEQUAL,0) < 0) {
		prt_error(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_87, "Vehicle file read error"));
		prt_hdr();
		return;
	}
	strcpy(rf_id,vh_ptr->mobile_rf_id);
	clear_err_line();
	if(!cur_list) {
		prt_error(NO_CUR_LIST);
		return;
	}

	prt_upd_hdr();					/* Print the header line for the update screen.  */
	while(1)		/*user cannot enter a duplicate RFID */
	{
		piu = F_RFID;					/* start with the license number */
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
		memcpy((char *)temp_vh_ptr,req_buf,sizeof(struct cisam_vh));
		if(strcmp(rf_id,vh_ptr->mobile_rf_id) == 0)
		{
			break	;/* user never touched RFID */
		}
		else if(db(VEHICLE_FILE_ID,READ_KEYS,&veh_key1,ISEQUAL,0) == 0)
		{
			prt_error(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_86, "RF ID already exists"));
		}
		else
		{
			break;
		}
		memcpy(req_buf,(char *)temp_vh_ptr,sizeof(struct cisam_vh));

	}
	memcpy(req_buf,(char *)temp_vh_ptr,sizeof(struct cisam_vh));
	memcpy((char *)temp_vh_ptr,req_buf,sizeof(struct cisam_vh));
	if(db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISEQUAL+ISLOCK,0) < 0) {
		prt_error(LOCK_VEHICLE_ERR);
		ERROR(' ',VEHICLE_FILE,LOCK_VEHICLE_ERR);
		prt_hdr();
		return;
	}

	memcpy(req_buf,(char *)temp_vh_ptr,sizeof(struct cisam_vh));
	if(db(VEHICLE_FILE_ID,UPDATE_REC,&veh_key2,0,0) < 0) {
		prt_error(UPD_VEHICLE_ERR);
		sprintf(err, "%s(%d)", UPD_VEHICLE_ERR, iserrno);
		ERROR(' ',VEHICLE_FILE, err);
	}
	else
	    prt_error(REC_UPDATED);

	if(db(VEHICLE_FILE_ID,RELEASE,&veh_key2,0,0) < 0) {
		prt_error(RLS_VEHICLE_ERR);
		cur_list = 0;
		ERROR(' ',VEHICLE_FILE,RLS_VEHICLE_ERR);
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
	if(db(VEHICLE_FILE_ID,DELETE,&veh_key2,0,0) < 0) {
		prt_error(DEL_VEHICLE_ERR);
		sprintf(err, "%s(%d)", DEL_VEHICLE_ERR, iserrno);
		ERROR(' ',VEHICLE_FILE, err);
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
		ERROR(' ',VEHICLE_FILE,OPEN_OUT_ERR);
		prt_hdr();
		return;
	}

        prt_scr(o_file_fp);		/*  Write the screen information to the file.  */
	fclose(o_file_fp);
	prt_hdr();

	return;
}

/*****************************************************************************
* store_def - store default values for the seals/attributes arrays.
****************************************************************************/
store_def()
{
	int i;		/* loop index */

	for(i = 0; i < NUM_SEALS ; i++)
		vh_ptr->seals[i] = NO;

	for(i = 0; i < NUM_ATTR ; i++)
		vh_ptr->attr[i] = NO;

	vh_ptr->schedule_num = 0;

	return;
}

store_term_type_def()
{
	vh_ptr->term_type = 1;
}

/******************************************************************************
* prt_labels - print the field labels on the screen.
*******************************************************************************/
void prt_labels()
{
        char     open_brace[2],  close_brace[2];

	strcpy(open_brace, catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_134,"["));
	strcpy(close_brace, catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_135,"]"));

	attrset(A_BOLD);
	mvaddstr(L_FLEET_ROW,L_FLEET_1_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_88, "Fleet"));
	mvaddstr(L_FLEET_ROW,L_VEH_NBR_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_89, "Veh #"));
	mvaddstr(L_FLEET_ROW,L_RFID_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_90, "MID"));
	mvaddstr(L_FLEET_ROW,L_LIC_NBR_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_91, "License #"));
	mvaddstr(L_FLEET_ROW,L_MAKE_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_92, "Make"));
	mvaddstr(L_FLEET_ROW,L_MODEL_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_93, "Model"));
	mvaddstr(L_FLEET_ROW,L_COLOR_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_144, "Color"));
	mvaddstr(L_FLEET_ROW,L_TELEPHONE_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_145, "Tele"));
	mvaddstr(L_FLEET_ROW,L_KDT_OK_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_94, "MDT OK"));
	mvaddstr(L_FLEET_ROW,L_MET_OK_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_95, "Meter OK"));
	mvaddstr(L_FLEET_ROW,L_SCHEDULE_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_132, "Schedule"));
	attrset(0);
	mvaddstr(L_FLEET_ROW+1,L_FLEET_1_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_FLEET_1_COL+MC_FLEET_1+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_FLEET_2_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_FLEET_2_COL+MC_FLEET_2+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_VEH_NBR_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_VEH_NBR_COL+MC_VEH_NBR+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_RFID_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_RFID_COL+MC_RFID+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_LIC_NBR_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_LIC_NBR_COL+MC_LIC_NBR+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_MAKE_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_MAKE_COL+MC_MAKE+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_MODEL_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_MODEL_COL+MC_MODEL+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_COLOR_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_COLOR_COL+MC_COLOR+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_TELEPHONE_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_TELEPHONE_COL+MC_TELEPHONE+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_KDT_OK_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_KDT_OK_COL+MC_KDT_OK+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_MET_OK_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_MET_OK_COL+MC_MET_OK+1,close_brace);
	mvaddstr(L_FLEET_ROW+1,L_SCHEDULE_COL,open_brace);
	mvaddstr(L_FLEET_ROW+1,L_SCHEDULE_COL+MC_SCHEDULE+1,close_brace);
	attrset(A_BOLD);
	mvaddstr(L_SEALS_ROW,L_SEALS_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_96, "Seals"));
	mvaddstr(L_SEALS_ROW,L_SEAL_1_COL,"1");
	mvaddstr(L_SEALS_ROW,L_SEAL_2_COL,"2");
	mvaddstr(L_SEALS_ROW,L_SEAL_3_COL,"3");
	mvaddstr(L_SEALS_ROW,L_SEAL_4_COL,"4");
	mvaddstr(L_SEALS_ROW,L_SEAL_5_COL,"5");
	mvaddstr(L_SEALS_ROW,L_SEAL_6_COL,"6");
	mvaddstr(L_SEALS_ROW,L_SEAL_7_COL,"7");
	mvaddstr(L_SEALS_ROW,L_SEAL_8_COL,"8");
	mvaddstr(L_SEALS_ROW,L_SEAL_9_COL,"9");
	mvaddstr(L_SEALS_ROW,L_SEAL_10_COL,"10");
	mvaddstr(L_SEALS_ROW,L_SEAL_11_COL,"11");
	mvaddstr(L_SEALS_ROW,L_SEAL_12_COL,"12");
	mvaddstr(L_SEALS_ROW,L_SEAL_13_COL,"13");
	mvaddstr(L_SEALS_ROW,L_SEAL_14_COL,"14");
	mvaddstr(L_SEALS_ROW,L_SEAL_15_COL,"15");
	mvaddstr(L_SEALS_ROW,L_SEAL_16_COL,"16");
	mvaddstr(L_SEALS_ROW,L_TERM_TYPE_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_139,"Terminal Type"));
	mvaddstr(L_SEALS_ROW,L_NUM_MISS_RECEIPTS_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_140,"Missing Receipts"));
	mvaddstr(L_SEALS_ROW,L_MDT_TELE_COL,"MDT Puhelin");
	attrset(0);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_1_COL,open_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_1_COL+MC_SEAL_1+1,close_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_2_COL,open_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_2_COL+MC_SEAL_2+1,close_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_3_COL,open_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_3_COL+MC_SEAL_3+1,close_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_4_COL,open_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_4_COL+MC_SEAL_4+1,close_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_5_COL,open_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_5_COL+MC_SEAL_5+1,close_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_6_COL,open_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_6_COL+MC_SEAL_6+1,close_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_7_COL,open_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_7_COL+MC_SEAL_7+1,close_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_8_COL,open_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_8_COL+MC_SEAL_8+1,close_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_9_COL,open_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_9_COL+MC_SEAL_9+1,close_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_10_COL,open_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_10_COL+MC_SEAL_10+1,close_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_11_COL,open_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_11_COL+MC_SEAL_11+1,close_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_12_COL,open_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_12_COL+MC_SEAL_12+1,close_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_13_COL,open_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_13_COL+MC_SEAL_13+1,close_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_14_COL,open_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_14_COL+MC_SEAL_14+1,close_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_15_COL,open_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_15_COL+MC_SEAL_15+1,close_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_16_COL,open_brace);
	mvaddstr(L_SEALS_ROW+1,L_SEAL_16_COL+MC_SEAL_16+1,close_brace);
	mvaddstr(L_SEALS_ROW+1,L_TERM_TYPE_COL+3,open_brace);
	mvaddstr(L_SEALS_ROW+1,L_TERM_TYPE_COL+9, close_brace);
	mvaddstr(L_SEALS_ROW+1,L_NUM_MISS_RECEIPTS_COL,open_brace); 
	mvaddstr(L_SEALS_ROW+1,L_NUM_MISS_RECEIPTS_COL+15, close_brace);
	mvaddstr(L_SEALS_ROW+1,L_MDT_TELE_COL,open_brace); 
	mvaddstr(L_SEALS_ROW+1,L_MDT_TELE_COL+15, close_brace);	
	attrset(A_BOLD);
	mvaddstr(L_ATTR_ROW,L_ATTR_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_97, "Attributes"));

	mvaddstr(L_ATTR_ROW,L_SUSP_FM_1_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_98, "Suspended From"));
	mvaddstr(L_ATTR_ROW,L_SUSP_TO_1_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_99, "Suspended To"));
	mvaddstr(L_ATTR_ROW,L_SUSP_RSN_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_100, "Suspension Reason"));
	mvaddstr(L_ATTR_ROW+1,L_PASS_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_101, "P"));
	mvaddstr(L_ATTR_ROW+2,L_DEL_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_102, "D"));
	mvaddstr(L_ATTR_ROW+3,L_XTYP_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_103, "X"));
	attrset(0);
	mvaddstr(L_ATTR_ROW+1,L_ATTR_1_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_ATTR_1_COL+MC_ATTR_1+1,close_brace);

	mvaddstr(L_ATTR_ROW+1,L_SUSP_FM_1_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_SUSP_FM_1_COL+MC_SUSP_FM_1+1,close_brace);
	mvaddstr(L_ATTR_ROW+1,L_SUSP_FM_2_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_SUSP_FM_2_COL+MC_SUSP_FM_2+1,close_brace);
	mvaddstr(L_ATTR_ROW+1,L_SUSP_TO_1_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_SUSP_TO_1_COL+MC_SUSP_TO_1+1,close_brace);
	mvaddstr(L_ATTR_ROW+1,L_SUSP_TO_2_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_SUSP_TO_2_COL+MC_SUSP_TO_2+1,close_brace);
	mvaddstr(L_ATTR_ROW+2,L_SUSP_FM_1_COL,open_brace);
	mvaddstr(L_ATTR_ROW+2,L_SUSP_FM_1_COL+MC_SUSP_FM_1+1,close_brace);
	mvaddstr(L_ATTR_ROW+2,L_SUSP_FM_2_COL,open_brace);
	mvaddstr(L_ATTR_ROW+2,L_SUSP_FM_2_COL+MC_SUSP_FM_2+1,close_brace);
	mvaddstr(L_ATTR_ROW+2,L_SUSP_TO_1_COL,open_brace);
	mvaddstr(L_ATTR_ROW+2,L_SUSP_TO_1_COL+MC_SUSP_TO_1+1,close_brace);
	mvaddstr(L_ATTR_ROW+2,L_SUSP_TO_2_COL,open_brace);
	mvaddstr(L_ATTR_ROW+2,L_SUSP_TO_2_COL+MC_SUSP_TO_2+1,close_brace);
	mvaddstr(L_ATTR_ROW+3,L_SUSP_FM_1_COL,open_brace);
	mvaddstr(L_ATTR_ROW+3,L_SUSP_FM_1_COL+MC_SUSP_FM_1+1,close_brace);
	mvaddstr(L_ATTR_ROW+3,L_SUSP_FM_2_COL,open_brace);
	mvaddstr(L_ATTR_ROW+3,L_SUSP_FM_2_COL+MC_SUSP_FM_2+1,close_brace);
	mvaddstr(L_ATTR_ROW+3,L_SUSP_TO_1_COL,open_brace);
	mvaddstr(L_ATTR_ROW+3,L_SUSP_TO_1_COL+MC_SUSP_TO_1+1,close_brace);
	mvaddstr(L_ATTR_ROW+3,L_SUSP_TO_2_COL,open_brace);
	mvaddstr(L_ATTR_ROW+3,L_SUSP_TO_2_COL+MC_SUSP_TO_2+1,close_brace);
	mvaddstr(L_ATTR_ROW+1,L_SUSP_RSN_COL,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_SUSP_RSN_COL+MC_SUSP_RSN+1,close_brace);
	mvaddstr(L_ATTR_ROW+1,L_PASS_COL+2,open_brace);
	mvaddstr(L_ATTR_ROW+1,L_PASS_COL+2+MC_PASS+1,close_brace);
	mvaddstr(L_ATTR_ROW+2,L_DEL_COL+2,open_brace);
	mvaddstr(L_ATTR_ROW+2,L_DEL_COL+2+MC_DEL+1,close_brace);
	mvaddstr(L_ATTR_ROW+3,L_XTYP_COL+2,open_brace);
	mvaddstr(L_ATTR_ROW+3,L_XTYP_COL+2+MC_XTYP+1,close_brace);
	attrset(A_BOLD);
	mvaddstr(L_SUSP_MSG_ROW,L_SUSP_MSG_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_104, "Suspension Message"));
	attrset(0);
	mvaddstr(L_SUSP_MSG_ROW+1,L_SUSP_MSG_COL,open_brace);
	mvaddstr(L_SUSP_MSG_ROW+1,L_SUSP_MSG_COL+MC_SP_MSG_1+1,close_brace);
	attrset(A_BOLD);
	mvaddstr(L_DRVR_CMNT_ROW,L_DRVR_CMNT_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_105, "Driver Comments"));
	attrset(0);
	mvaddstr(L_DRVR_CMNT_ROW+1,L_DRVR_CMNT_COL,open_brace);
	mvaddstr(L_DRVR_CMNT_ROW+1,L_DRVR_CMNT_COL+MC_DRVR_CMNT+1,close_brace);
	attrset(A_BOLD);
	mvaddstr(L_LST_EMRG_ROW,L_LST_EMRG_1_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_106, "Last Emergency On"));
	mvaddstr(L_LST_EMRG_ROW,L_EMRG_RES_1_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_107, "Emergency Resolved"));
	mvaddstr(L_LST_EMRG_ROW,L_SGN_ON_1_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_108, "Signed On"));
	mvaddstr(L_LST_EMRG_ROW,L_SGN_OFF_1_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_109, "Signed Off"));
	mvaddstr(L_LST_EMRG_ROW,L_DRIVER_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_110, "Driver"));
	mvaddstr(L_LST_EMRG_ROW,L_CL_SRV_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_111, "Call Serviced"));
	attrset(0);
	mvaddstr(L_LST_EMRG_ROW+1,L_LST_EMRG_1_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_LST_EMRG_1_COL+MC_L_EMRG_1+1,close_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_LST_EMRG_2_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_LST_EMRG_2_COL+MC_L_EMRG_2+1,close_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_EMRG_RES_1_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_EMRG_RES_1_COL+MC_E_RES_1+1,close_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_EMRG_RES_2_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_EMRG_RES_2_COL+MC_E_RES_2+1,close_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_SGN_ON_1_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_SGN_ON_1_COL+MC_SGN_ON_1+1,close_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_SGN_ON_2_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_SGN_ON_2_COL+MC_SGN_ON_2+1,close_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_SGN_OFF_1_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_SGN_OFF_1_COL+MC_SGN_OFF_1+1,close_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_SGN_OFF_2_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_SGN_OFF_2_COL+MC_SGN_OFF_2+1,close_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_DRIVER_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_DRIVER_COL+MC_DRIVER+1,close_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_CL_SRV_COL,open_brace);
	mvaddstr(L_LST_EMRG_ROW+1,L_CL_SRV_COL+MC_CL_SRV+1,close_brace);
	attrset(A_BOLD);
	mvaddstr(L_POST_ROW,L_POST_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_112, "BOOK"));
	mvaddstr(L_POST_ROW,L_CPOST_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_113, "CBOOK"));
	mvaddstr(L_POST_ROW,L_EPOST_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_114, "EBOOK"));
	mvaddstr(L_POST_ROW,L_N_ACPT_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_115, "no ACPT"));
	mvaddstr(L_POST_ROW,L_REJCT_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_116, "REJCT"));
	mvaddstr(L_POST_ROW,L_C_OUT_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_117, "C-OUT"));
	mvaddstr(L_POST_ROW,L_NSHOW_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_118, "NTRIP"));
	mvaddstr(L_POST_ROW,L_FLAG_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_119, "FLAG"));
	mvaddstr(L_POST_ROW,L_BID_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_120, "BID"));
	mvaddstr(L_POST_ROW,L_TALK_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_121, "TALK"));
	mvaddstr(L_POST_ROW,L_MSG_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_122, "MSG"));
	mvaddstr(L_POST_ROW,L_BREAK_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_123, "BREAK"));
	mvaddstr(L_POST_ROW,L_ASSIGNS_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_124, "Assigns"));
	mvaddstr(L_POST_ROW,L_CALLS_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_125, "Trips"));
	mvaddstr(L_POST_ROW,L_SH_MET_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_126, "Short M"));
	mvaddstr(L_POST_ROW,L_LT_MET_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_127, "Late M"));
	mvaddstr(L_POST_ROW,L_CL_BACK_COL,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_128, "Callback"));
	attrset(0);
	mvaddstr(L_POST_ROW+1,L_POST_COL,open_brace);
	mvaddstr(L_POST_ROW+1,L_POST_COL+MC_POST+1,close_brace);
	mvaddstr(L_POST_ROW+1,L_CPOST_COL,open_brace);
	mvaddstr(L_POST_ROW+1,L_CPOST_COL+MC_CPOST+1,close_brace);
	mvaddstr(L_POST_ROW+1,L_EPOST_COL,open_brace);
	mvaddstr(L_POST_ROW+1,L_EPOST_COL+MC_EPOST+1,close_brace);
	mvaddstr(L_POST_ROW+1,L_N_ACPT_COL,open_brace);
	mvaddstr(L_POST_ROW+1,L_N_ACPT_COL+MC_N_ACPT+1,close_brace);
	mvaddstr(L_POST_ROW+1,L_REJCT_COL,open_brace);
	mvaddstr(L_POST_ROW+1,L_REJCT_COL+MC_REJCT+1,close_brace);
	mvaddstr(L_POST_ROW+1,L_C_OUT_COL,open_brace);
	mvaddstr(L_POST_ROW+1,L_C_OUT_COL+MC_C_OUT+1,close_brace);
	mvaddstr(L_POST_ROW+1,L_NSHOW_COL,open_brace);
	mvaddstr(L_POST_ROW+1,L_NSHOW_COL+MC_NSHOW+1,close_brace);
	mvaddstr(L_POST_ROW+1,L_FLAG_COL,open_brace);
	mvaddstr(L_POST_ROW+1,L_FLAG_COL+MC_FLAG+1,close_brace);
	mvaddstr(L_POST_ROW+1,L_BID_COL,open_brace);
	mvaddstr(L_POST_ROW+1,L_BID_COL+MC_BID+1,close_brace);
	mvaddstr(L_POST_ROW+1,L_TALK_COL,open_brace);
	mvaddstr(L_POST_ROW+1,L_TALK_COL+MC_TALK+1,close_brace);
	mvaddstr(L_POST_ROW+1,L_MSG_COL,open_brace);
	mvaddstr(L_POST_ROW+1,L_MSG_COL+MC_MSG+1,close_brace);
	mvaddstr(L_POST_ROW+1,L_BREAK_COL,open_brace);
	mvaddstr(L_POST_ROW+1,L_BREAK_COL+MC_BREAK+1,close_brace);
	mvaddstr(L_POST_ROW+1,L_ASSIGNS_COL,open_brace);
	mvaddstr(L_POST_ROW+1,L_ASSIGNS_COL+MC_ASSIGNS+1,close_brace);
	mvaddstr(L_POST_ROW+1,L_CALLS_COL,open_brace);
	mvaddstr(L_POST_ROW+1,L_CALLS_COL+MC_CALLS+1,close_brace);
	mvaddstr(L_POST_ROW+1,L_SH_MET_COL,open_brace);
	mvaddstr(L_POST_ROW+1,L_SH_MET_COL+MC_SH_MET+1,close_brace);
	mvaddstr(L_POST_ROW+1,L_LT_MET_COL,open_brace);
	mvaddstr(L_POST_ROW+1,L_LT_MET_COL+MC_LT_MET+1,close_brace);
	mvaddstr(L_POST_ROW+1,L_CL_BACK_COL,open_brace);
	mvaddstr(L_POST_ROW+1,L_CL_BACK_COL+MC_CL_BACK+1,close_brace);
	refresh();
}

/******************************************************************************
* prt_data - print the data on the screen.
*******************************************************************************/
prt_data()
{
	int i;						/* loop index */
	struct tm *tm_ptr;				/* pointer to a unix time structure */
	char AttributesSet;
	int AttrCol = 0, AttrRow = 0;
	time_t tmp_time;	
	extern struct fleets *fleet[];

	mvprintw(D_FLEET_ROW,D_FLEET_1_COL,"%c",vh_ptr->fleet);
	mvprintw(D_FLEET_ROW,D_FLEET_2_COL,"%d",vh_ptr->fleet_nbr);
	mvprintw(D_FLEET_ROW,D_VEH_NBR_COL,"%4d",vh_ptr->nbr);
	mvprintw(D_FLEET_ROW,D_RFID_COL,"%5d",strtol(vh_ptr->mobile_rf_id, (char **)NULL, 16));
	mvprintw(D_FLEET_ROW,D_LIC_NBR_COL,"%s",vh_ptr->license_nbr);
	mvprintw(D_FLEET_ROW,D_MAKE_COL,"%s",vh_ptr->make);
	mvprintw(D_FLEET_ROW,D_MODEL_COL,"%s",vh_ptr->model);
	mvprintw(D_FLEET_ROW,D_COLOR_COL,"%s",vh_ptr->color);
	mvprintw(D_FLEET_ROW,D_TELEPHONE_COL, "%s", vh_ptr->telephone);
	mvprintw(D_FLEET_ROW,D_KDT_OK_COL,"%c",vh_ptr->kdt_working);
	mvprintw(D_FLEET_ROW,D_MET_OK_COL,"%c",vh_ptr->meter_working);
	mvprintw(D_FLEET_ROW,D_SCHEDULE_COL,"%2d",vh_ptr->schedule_num);
	mvprintw(D_SEALS_ROW,D_SEAL_1_COL,"%c",vh_ptr->seals[0]);
	mvprintw(D_SEALS_ROW,D_SEAL_2_COL,"%c",vh_ptr->seals[1]);
	mvprintw(D_SEALS_ROW,D_SEAL_3_COL,"%c",vh_ptr->seals[2]);
	mvprintw(D_SEALS_ROW,D_SEAL_4_COL,"%c",vh_ptr->seals[3]);
	mvprintw(D_SEALS_ROW,D_SEAL_5_COL,"%c",vh_ptr->seals[4]);
	mvprintw(D_SEALS_ROW,D_SEAL_6_COL,"%c",vh_ptr->seals[5]);
	mvprintw(D_SEALS_ROW,D_SEAL_7_COL,"%c",vh_ptr->seals[6]);
	mvprintw(D_SEALS_ROW,D_SEAL_8_COL,"%c",vh_ptr->seals[7]);
	mvprintw(D_SEALS_ROW,D_SEAL_9_COL,"%c",vh_ptr->seals[8]);
	mvprintw(D_SEALS_ROW,D_SEAL_10_COL,"%c",vh_ptr->seals[9]);
	mvprintw(D_SEALS_ROW,D_SEAL_11_COL,"%c",vh_ptr->seals[10]);
	mvprintw(D_SEALS_ROW,D_SEAL_12_COL,"%c",vh_ptr->seals[11]);
	mvprintw(D_SEALS_ROW,D_SEAL_13_COL,"%c",vh_ptr->seals[12]);
	mvprintw(D_SEALS_ROW,D_SEAL_14_COL,"%c",vh_ptr->seals[13]);
	mvprintw(D_SEALS_ROW,D_SEAL_15_COL,"%c",vh_ptr->seals[14]);
	mvprintw(D_SEALS_ROW,D_SEAL_16_COL,"%c",vh_ptr->seals[15]);
	if ( vh_ptr->term_type < 1 || vh_ptr->term_type > 9 )
		vh_ptr->term_type = 1;		/* set the default value */
	mvprintw(D_SEALS_ROW,D_TERM_TYPE_COL,"%5d",vh_ptr->term_type);
	mvprintw(D_SEALS_ROW,D_NUM_MISS_RECEIPTS_COL,"%14d",vh_ptr->itm_nbr_missing_receipts);
	mvprintw(D_SEALS_ROW,D_MDT_TELE_COL,"%s",vh_ptr->mdt_tele);

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

	if(!vh_ptr->susp_from_dt_tm)  {
		mvprintw(D_ATTR_ROW,D_SUSP_FM_1_COL, zero_date);
		mvprintw(D_ATTR_ROW,D_SUSP_FM_2_COL, "00:00");
	}
	else  {
		mvprintw(D_ATTR_ROW,D_SUSP_FM_1_COL,"%s",vh_ptr->susp_from_date);
		mvprintw(D_ATTR_ROW,D_SUSP_FM_2_COL,"%s",vh_ptr->susp_from_time);
	}

	if(!vh_ptr->susp_to_dt_tm)  {
		mvprintw(D_ATTR_ROW,D_SUSP_TO_1_COL, zero_date);
		mvprintw(D_ATTR_ROW,D_SUSP_TO_2_COL, "00:00");
	}
	else {
		mvprintw(D_ATTR_ROW,D_SUSP_TO_1_COL,"%s",vh_ptr->susp_to_date);
		mvprintw(D_ATTR_ROW,D_SUSP_TO_2_COL,"%s",vh_ptr->susp_to_time);
	}

	if(!vh_ptr->susp_d_from_dt_tm)  {
		mvprintw(D_ATTR_ROW+1,D_SUSP_FM_1_COL, zero_date);
		mvprintw(D_ATTR_ROW+1,D_SUSP_FM_2_COL, "00:00");
	}
	else  {
		mvprintw(D_ATTR_ROW+1,D_SUSP_FM_1_COL,"%s",vh_ptr->susp_d_from_date);
		mvprintw(D_ATTR_ROW+1,D_SUSP_FM_2_COL,"%s",vh_ptr->susp_d_from_time);
	}

	if(!vh_ptr->susp_d_to_dt_tm)  {
		mvprintw(D_ATTR_ROW+1,D_SUSP_TO_1_COL, zero_date);
		mvprintw(D_ATTR_ROW+1,D_SUSP_TO_2_COL, "00:00");
	}
	else {
		mvprintw(D_ATTR_ROW+1,D_SUSP_TO_1_COL,"%s",vh_ptr->susp_d_to_date);
		mvprintw(D_ATTR_ROW+1,D_SUSP_TO_2_COL,"%s",vh_ptr->susp_d_to_time);
	}

	if(!vh_ptr->susp_x_from_dt_tm)  {
		mvprintw(D_ATTR_ROW+2,D_SUSP_FM_1_COL, zero_date);
		mvprintw(D_ATTR_ROW+2,D_SUSP_FM_2_COL, "00:00");
	}
	else  {
		mvprintw(D_ATTR_ROW+2,D_SUSP_FM_1_COL,"%s",vh_ptr->susp_x_from_date);
		mvprintw(D_ATTR_ROW+2,D_SUSP_FM_2_COL,"%s",vh_ptr->susp_x_from_time);
	}

	if(!vh_ptr->susp_x_to_dt_tm)  {
		mvprintw(D_ATTR_ROW+2,D_SUSP_TO_1_COL, zero_date);
		mvprintw(D_ATTR_ROW+2,D_SUSP_TO_2_COL, "00:00");
	}
	else {
		mvprintw(D_ATTR_ROW+2,D_SUSP_TO_1_COL,"%s",vh_ptr->susp_x_to_date);
		mvprintw(D_ATTR_ROW+2,D_SUSP_TO_2_COL,"%s",vh_ptr->susp_x_to_time);
	}

	mvprintw(D_ATTR_ROW,D_SUSP_RSN_COL,"%s",vh_ptr->susp_reason);
	mvprintw(D_ATTR_ROW,D_PASS_COL,"%c",vh_ptr->susp_pass_call);
	mvprintw(D_ATTR_ROW+1,D_DEL_COL,"%c",vh_ptr->susp_del_call);
	mvprintw(D_ATTR_ROW+2,D_XTYP_COL,"%c",vh_ptr->susp_typex_call);

	if(vh_ptr->susp_message)
		mvprintw(D_SP_MSG_1_ROW,D_SP_MSG_1_COL,"%s",vh_ptr->susp_message);
	else  {
		move(D_SP_MSG_1_ROW,D_SP_MSG_1_COL);
		for(i = 0; i < MC_SP_MSG_1; i++)
			addch(' ');
	}

	if(vh_ptr->comments)
		mvprintw(D_DRVR_CMNT_ROW,D_DRVR_CMNT_COL,"%s",vh_ptr->comments);
	else  {
		move(D_DRVR_CMNT_ROW,D_DRVR_CMNT_COL);
		for(i = 0; i < MC_DRVR_CMNT; i++)
			addch(' ');
	}
	
	if(!vh_ptr->last_emerg_on)  {
		mvprintw(D_LST_EMRG_ROW,D_L_EMRG_1_COL, zero_date);
		mvprintw(D_LST_EMRG_ROW,D_L_EMRG_2_COL, "00:00");
	}
	else  {
	  tmp_time = (time_t)vh_ptr->last_emerg_on;
	  tm_ptr = localtime(&tmp_time);
		
		if ( tm_ptr->tm_year >= 100 )
		  tm_ptr->tm_year -= 100;
		if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		    mvprintw(D_LST_EMRG_ROW,D_L_EMRG_1_COL,"%02d%c%02d%c%02d",
			     tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_year);
		else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
			mvprintw(D_LST_EMRG_ROW,D_L_EMRG_1_COL,"%02d%c%02d%c%02d",
			     tm_ptr->tm_year, DATE_SEPERATOR, tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_mday);
		else
		    mvprintw(D_LST_EMRG_ROW,D_L_EMRG_1_COL,"%02d%c%02d%c%02d",
			     tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_year);
#ifdef HR24
		mvprintw(D_LST_EMRG_ROW,D_L_EMRG_2_COL,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#else
		if(tm_ptr->tm_hour >= 12)  {
			tm_ptr->tm_hour -= 12;
			mvprintw(D_LST_EMRG_ROW,D_L_EMRG_2_COL,"%02d:%02dp",tm_ptr->tm_hour,tm_ptr->tm_min);
		}
		else
			mvprintw(D_LST_EMRG_ROW,D_L_EMRG_2_COL,"%02d:%02da",tm_ptr->tm_hour,tm_ptr->tm_min);
#endif
	}

	if(!vh_ptr->last_emerg_off)  {
		mvprintw(D_LST_EMRG_ROW,D_E_RES_1_COL, zero_date);
		mvprintw(D_LST_EMRG_ROW,D_E_RES_2_COL, "00:00");
	}
	else  {
	  tmp_time = (time_t)vh_ptr->last_emerg_off;
	  tm_ptr = localtime(&tmp_time);
	  
		if ( tm_ptr->tm_year >= 100 )
		  tm_ptr->tm_year -= 100;		
		if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		    mvprintw(D_LST_EMRG_ROW,D_E_RES_1_COL,"%02d%c%02d%c%02d",
			     tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_year);
		else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
			mvprintw(D_LST_EMRG_ROW,D_E_RES_1_COL,"%02d%c%02d%c%02d",
			     tm_ptr->tm_year, DATE_SEPERATOR, tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_mday);
		else
		    mvprintw(D_LST_EMRG_ROW,D_E_RES_1_COL,"%02d%c%02d%c%02d",
			     tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_year);
#ifdef HR24
		mvprintw(D_LST_EMRG_ROW,D_E_RES_2_COL,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#else
		if(tm_ptr->tm_hour >= 12)  {
			tm_ptr->tm_hour -= 12;
			mvprintw(D_LST_EMRG_ROW,D_E_RES_2_COL,"%02d:%02dp",tm_ptr->tm_hour,tm_ptr->tm_min);
		}
		else
			mvprintw(D_LST_EMRG_ROW,D_E_RES_2_COL,"%02d:%02da",tm_ptr->tm_hour,tm_ptr->tm_min);
#endif
	}

	if(!vh_ptr->last_signed_on)  {
		mvprintw(D_LST_EMRG_ROW,D_SGN_ON_1_COL, zero_date);
		mvprintw(D_LST_EMRG_ROW,D_SGN_ON_2_COL, "00:00");
	}
	else  {
	  tmp_time = (time_t)vh_ptr->last_signed_on;
	  tm_ptr = localtime(&tmp_time);
	  
		if ( tm_ptr->tm_year >= 100 )
		  tm_ptr->tm_year -= 100;		
		if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		    mvprintw(D_LST_EMRG_ROW,D_SGN_ON_1_COL,"%02d%c%02d%c%02d",
			     tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_year);
		else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
		    mvprintw(D_LST_EMRG_ROW,D_SGN_ON_1_COL,"%02d%c%02d%c%02d",
			     tm_ptr->tm_year, DATE_SEPERATOR, tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_mday);
		else
		    mvprintw(D_LST_EMRG_ROW,D_SGN_ON_1_COL,"%02d%c%02d%c%02d",
			     tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_year);
#ifdef HR24
		mvprintw(D_LST_EMRG_ROW,D_SGN_ON_2_COL,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#else
		if(tm_ptr->tm_hour >= 12)  {
			tm_ptr->tm_hour -= 12;
			mvprintw(D_LST_EMRG_ROW,D_SGN_ON_2_COL,"%02d:%02dp",tm_ptr->tm_hour,tm_ptr->tm_min);
		}
		else
			mvprintw(D_LST_EMRG_ROW,D_SGN_ON_2_COL,"%02d:%02da",tm_ptr->tm_hour,tm_ptr->tm_min);
#endif
	}
	
	if(!vh_ptr->last_signed_off)  {
		mvprintw(D_LST_EMRG_ROW,D_SGN_OFF_1_COL, zero_date);
		mvprintw(D_LST_EMRG_ROW,D_SGN_OFF_2_COL, "00:00");
	}
	else  {
	  tmp_time = (time_t)vh_ptr->last_signed_off;
	  tm_ptr = localtime(&tmp_time);
	  
		if ( tm_ptr->tm_year >= 100 )
		  tm_ptr->tm_year -= 100;		
		if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		    mvprintw(D_LST_EMRG_ROW,D_SGN_OFF_1_COL,"%02d%c%02d%c%02d",
			     tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_year);
		else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
		    mvprintw(D_LST_EMRG_ROW,D_SGN_OFF_1_COL,"%02d%c%02d%c%02d",
			     tm_ptr->tm_year, DATE_SEPERATOR, tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_mday);
		else
		    mvprintw(D_LST_EMRG_ROW,D_SGN_OFF_1_COL,"%02d%c%02d%c%02d",
			     tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_year);
#ifdef HR24
		mvprintw(D_LST_EMRG_ROW,D_SGN_OFF_2_COL,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#else
		if(tm_ptr->tm_hour >= 12)  {
			tm_ptr->tm_hour -= 12;
			mvprintw(D_LST_EMRG_ROW,D_SGN_OFF_2_COL,"%02d:%02dp",tm_ptr->tm_hour,tm_ptr->tm_min);
		}
		else
			mvprintw(D_LST_EMRG_ROW,D_SGN_OFF_2_COL,"%02d:%02da",tm_ptr->tm_hour,tm_ptr->tm_min);
#endif
	}

	mvprintw(D_LST_EMRG_ROW,D_DRIVER_COL,"%5d",vh_ptr->last_driver);
	mvprintw(D_LST_EMRG_ROW,D_CL_SRV_COL,"%18d",vh_ptr->last_call);
	mvprintw(D_POST_ROW,D_POST_COL,"%4d",vh_ptr->posts);
	mvprintw(D_POST_ROW,D_CPOST_COL,"%4d",vh_ptr->cond_posts);
	mvprintw(D_POST_ROW,D_EPOST_COL,"%4d",vh_ptr->enroute_posts);
	mvprintw(D_POST_ROW,D_N_ACPT_COL,"%4d",vh_ptr->no_accepts);
	mvprintw(D_POST_ROW,D_REJCT_COL,"%4d",vh_ptr->reject);
	mvprintw(D_POST_ROW,D_C_OUT_COL,"%4d",vh_ptr->callouts);
	mvprintw(D_POST_ROW,D_NSHOW_COL,"%4d",vh_ptr->no_shows);
	mvprintw(D_POST_ROW,D_FLAG_COL,"%4d",vh_ptr->flags);
	mvprintw(D_POST_ROW,D_BID_COL,"%4d",vh_ptr->bids);
	mvprintw(D_POST_ROW,D_TALK_COL,"%4d",vh_ptr->req_to_talk);
	mvprintw(D_POST_ROW,D_MSG_COL,"%4d",vh_ptr->messages);
	mvprintw(D_POST_ROW,D_BREAK_COL,"%4d",vh_ptr->breaks);
	mvprintw(D_POST_ROW,D_ASSIGNS_COL,"%4d",vh_ptr->assigns);
	mvprintw(D_POST_ROW,D_CALLS_COL,"%4d",vh_ptr->calls);
	mvprintw(D_POST_ROW,D_SH_MET_COL,"%4d",vh_ptr->short_meters);
	mvprintw(D_POST_ROW,D_LT_MET_COL,"%4d",vh_ptr->late_meters);
	mvprintw(D_POST_ROW,D_CL_BACK_COL,"%4d",vh_ptr->callbacks);

	refresh();

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
	df_maint_ipc.which_file = VEHICLE_FILE_ID;
	df_maint_ipc.key = vh_ptr->nbr; 
	df_maint_ipc.fleet = vh_ptr->fleet;
	df_maint_ipc.cisam_num = isrecnum;         
	if((qid = msgget(key,0666 | IPC_CREAT)) == -1)   { 
		prt_error(VEHICLE_GET_ERR); 
		sleep(3); 
		ERROR(' ',VEHICLE_FILE,VEHICLE_GET_ERR);
		cleanup();
	}

	if((msgsnd(qid,&df_maint_ipc,sizeof(df_maint_ipc), IPC_NOWAIT)) == -1)  {
		prt_error(VEHICLE_SND_ERR);
		sleep(3);
		ERROR(' ',VEHICLE_FILE,VEHICLE_SND_ERR);
		cleanup();
	}
	return;
}

/******************************************************************************
* Clear the data fields on the screen. 
*******************************************************************************/

clear_flds()
{
	int i;		/* loop index */

	for(i = 1; i <= NUM_FLDS; i++) 
		vh_cl_flds(i); 
	return;
}

/******************************************************************************
* prt_hdr - print the main screen header.
*******************************************************************************/

void prt_hdr()
{
	clear_hdr_line();
	if (AddUpdateDeleteAllowed())
	  mvaddstr(HDR_ROW_1,HDR_COL_1,MAIN_HDR);
	else
	  mvaddstr(HDR_ROW_1, HDR_COL_1, "Query Next Previous Output Exit");
	attrset(A_BOLD);
	mvaddstr(HDR_ROW_2,HDR_COL_2,VEHICLE_TITLE);
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
	mvaddstr(HDR_ROW_1,HDR_COL_1,VEHICLE_FNAME_PROMPT);
	refresh();
	getstr(o_file);
	if(!strcmp(o_file,""))
		strcpy(o_file,catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_131, "vh_scr.out"));

	return;
} 

