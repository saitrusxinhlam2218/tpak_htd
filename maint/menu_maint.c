static char sz__FILE__[]="@(#)menu_maint.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  menu_maint.c; Rel: 5.4.0.0; Get date: 3/28/90 at 10:09:00
 * @(#)  Last delta: 3/23/90 at 16:07:52
 * @(#)  SCCS File: /taxi/sccs/s.menu_maint.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <isam.h>
#include <cursesX.h>
#include <signal.h>
#include <setjmp.h>

#define DECLARING
#include "comm_strdefs.h"
#include "menu_m_strdefs.h"
#include "mads_types.h"
#include "mad_error.h"
#include "key.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "menu_maint.h"
#include "tokenize.h"
#include "language.h"


/*  Declare an array of pointers to functions which check the field entered
 *  for validity.  
 */

extern int do_jmp;		/* flag used to indicate longjmp processing status	*/
extern jmp_buf jmp_env;		/* environment storage for longjmp			*/

extern int (*check_field[])();	/* Array of pointers to routines to 
				   validate the data input by the user */

extern struct scr_flds menu_scr_flds[];
extern int menu_cl_flds();
extern int read_field();
extern char buf[];		/* buffer used for interface to db routine */
extern char req_buf[];		/* buffer used for interface to db routine */
extern int iserrno;		/* C-isam error number */
extern int errno;		/* Error number returned from system calls */
extern upper_case;
char o_file[F_NAME_SIZE];	/* output file as specified by the user */
char mode[2];			/* user specifies Append or Create */
int  bAttrSelect;
int valid_req(), prt_query_hdr();
int fld_desc(), clear_desc_line();
int prt_scr(), process_intr();
void prt_hdr(), prt_labels();
char read_request();
int prompt_fname(), prompt_mode();
int cmd_flag = 0; 
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
int ret_val;			/* value returned from the check routines */
int cur_list;			/* Indicates a row is in the current list */
int record_nbr;			/* C-ISAM record number */
int error_n_fld;		/* Set if an error in field condition exists */
int file_open;			/* set when MENU file opened and check in cleanup */

struct df_maint df_maint_ipc;	/* structure for passing information 
				   to dispatch  */
struct cisam_mf *mf_ptr;	/* pointer to menu file structure */
struct cisam_mf *temp_mf_ptr;	
struct cisam_mf temp_mf_buf;	

struct offsets *offset;		/* shared memory offset for redundant db routine. */
struct offsets *get_offsets();


/*****************************************************************************
*  menu_maint - main for the menu file maintenance program.
*******************************************************************************/

main(argc,argv)
int argc;
char **argv;
{

	char request;		/*  User request : Q, N, P, A, U, R, O, or E */
        char nlspath[80];
        static char put_nlspath[80];
        int  putenv();
       /*
        * Set up for internationalization by setting the locale and opening the catalog
        */
	strcpy(nlspath, "/usr/taxi/catalogs/finnish/%N.cat");
	sprintf(put_nlspath, "NLSPATH=%s", nlspath);
	putenv(put_nlspath);	
        setlocale(LC_ALL, "");
        open_catalog(&COMMON_m_catd, "common", 0, 0);
        open_catalog(&MEN_MAINT_m_catd, "menu_maint", 0, 0);

		file_open = 0;


	init();

#ifdef DEBUG
	sprintf(trace_str,"Entering menu_maint\n");
	TRACE(trace_str);
#endif
	temp_mf_ptr = (struct cisam_mf *)req_buf;
	temp_mf_ptr = &temp_mf_buf;
	prt_hdr();
	prt_labels();

	/*  Get the first record from the menu file */

	if((db(MENU_FILE_ID,READ_KEYS,&mf_key2,ISFIRST,0)) < 0)  {
		prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_3, "No record found, ADD first!"));
#ifdef DEBUG
	sprintf(trace_str,"Leaving menu_maint - 1\n");
	TRACE(trace_str);
#endif
	}
	else  {

	/*  Display the data from the first record */

		prt_data();

		record_nbr = isrecnum; 	/* save record number for future read */
		cur_list = 1;		/* valid row in current list */
	}

	/*  Input the users request (Query, Next, Previous, Update,
	    Output, Exit).  					*/

	 while(request = read_request())  {
		cmd_flag = 0;
		error_n_fld = 0;
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

			case M_UPDATE:
				clear_err_line();
				process_update();
				if (ipc_required)
				  send_ipc();
				break;

			case M_OUTPUT:
				clear_err_line();
				process_output();
				break;
		
			case M_EXIT:
				cleanup();
				break;

			default:
				cleanup();
				break;
		}
	    }

#ifdef DEBUG
	sprintf(trace_str,"Leaving menu_maint - 2\n");
	TRACE(trace_str);
#endif
	    cleanup();

}  /* End of main */

/******************************************************************************
* process_query - search for specific record in the menu file
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
	prt_query_hdr();		/* Print the header for the query 
					   screen */

	/*  Process users input of key field(command string).  */
	ret_val = NOT_VALID;
	while(ret_val == NOT_VALID)  {
		piu = F_CMD;
		norm_to_rev(stdscr,menu_scr_flds,piu);
		if (piu == 5 || piu == 6)
			upper_case = 0;
		intr_allowed = 1;		/* interrupts allowed */
		key_val = read_field(stdscr,menu_scr_flds,piu,read_buf,&field_entered,&should_clear);
		revund_to_norm(stdscr,menu_scr_flds,piu);
		attrset(0);
		intr_allowed = 0;		/* interrupts not allowed */

		if(saw_intr)  {			/* key_value = interrupt char */
			saw_intr = 0;		/* interrup has been processed */
			cur_list = 0;		/* no row in current list */
			clear_flds();
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

		ret_val = check_field[F_CMD](&piu,read_buf,key_val,&field_entered);
		if(ret_val == NOT_VALID)
			continue;
		if ((cmd_flag) && (key_val != ESC))	{
			prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_4, "Enter ESC to execute"));
			ret_val = NOT_VALID;
			continue;
		}

		if(key_val == ESC)   {
			clear_err_line();
		/*  Using the key entered in previous step, call C-Isam routine
	    	    to retrieve specific record.  */
		if (!cmd_flag)
			mf_ptr->command = 0;		
			if((db(MENU_FILE_ID,READ_KEYS,&mf_key2,ISGTEQ,0)) < 0)  {
				if(iserrno == ENOREC)  {
					prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_5, "There are no rows satisfying the conditions"));
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
                                        prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_6, "Unable to read menu file"));
                                        prt_cisam_err(iserrno,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_6, "Unable to read menu file"));
					ERROR(' ',MENU_FILE,"Unable to read menu file");
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

	return;

}  /* End of process_query()  */

/******************************************************************************
* process_next - read the next record in the menu file and display it.
*******************************************************************************/

process_next()
{

#ifdef DEBUG
	sprintf(trace_str,"Entering process_next\n");
	TRACE(trace_str);
#endif

	clear_err_line();
	if(!cur_list)  {
		prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_7, "There are no rows in the current list"));
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_next - 1\n");
	TRACE(trace_str);
#endif
		return;
	}

	/*  Call the C-Isam routine to get the next entry.   */

	if((db(MENU_FILE_ID,READ_KEYS,&mf_key2,ISNEXT,0)) < 0)  {
		if(iserrno == EENDFILE)  {
			prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_8, "There are no more rows in the direction you are going"));
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_next - 2\n");
	TRACE(trace_str);
#endif
			return;
		}
		else  {
                        prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_9, "Unable to read next menu file record"));
			sleep(3);
                        prt_cisam_err(iserrno,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_9, "Unable to read next menu file record"));
			ERROR(' ',MENU_FILE,"Unable to read next menu file record");
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_next - 3\n");
	TRACE(trace_str);
#endif
			cleanup();
		}
	}
	record_nbr = isrecnum;	/* save the record number for future read */
        prt_data();  		/* Print retrieved data on the screen. */

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_next - 4\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of process_next()  */

/******************************************************************************
* process_previous - read the previous record in the menu file and display it.
*******************************************************************************/

process_previous()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering process_previous\n");
	TRACE(trace_str);
#endif

	clear_err_line();
	if(!cur_list)  {
		prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_7, "There are no rows in the current list"));
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_previous - 1\n");
	TRACE(trace_str);
#endif
		return;
	}

	/*  Call the C-Isam routine to get the previous entry.  */

	if((db(MENU_FILE_ID,READ_KEYS,&mf_key2,ISPREV,0)) < 0)  {
		if(iserrno == EENDFILE)  {
			prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_8, "There are no more rows in the direction you are going"));
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_previous - 2\n");
	TRACE(trace_str);
#endif
			return;
		}
		else  {
			prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_10, "Unable to read previous menu file record"));
			sleep(3);
			prt_cisam_err(iserrno,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_10, "Unable to read previous menu file record"));
			ERROR(' ',MENU_FILE,"Unable to read previous menu file record");
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_previous - 3\n");
	TRACE(trace_str);
#endif
			cleanup();
		}
	}

	record_nbr = isrecnum;	/* save the record number for future read */
        prt_data();  		/* Print retrieved data on the screen. */

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_previous - 4\n");
	TRACE(trace_str);
#endif

}  /* End of process_previous()  */

/******************************************************************************
* process_update - input data fields and update the menu file record
*******************************************************************************/

process_update()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering process_update\n");
	TRACE(trace_str);
#endif

	clear_err_line();
	if(!cur_list)  {
		prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_7, "There are no rows in the current list"));
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_update - 1\n");
	TRACE(trace_str);
#endif
		return;
	}

	prt_upd_hdr();		/* Print the header line for the update screen.  */

	if((db(MENU_FILE_ID,READ_KEYS,&mf_key2,ISEQUAL,0)) < 0) {
		prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_12, "Menu file read error"));
		prt_hdr();
		return;
	}
	process_input();	/* Process each field as input by the user */
	if(saw_intr)  {
		saw_intr = 0;	/* interrupt has been processed */
		cur_list = 0;	/* no valid row in current list */
		clear_flds();
		clear_desc_line();
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_update - 3\n");
	TRACE(trace_str);
#endif
		return;
	}

	/*  Call the C-Isam update record routine.  */
	memcpy((char *)temp_mf_ptr,req_buf,sizeof(struct cisam_mf));
	if((db(MENU_FILE_ID,READ_KEYS,&mf_key2,ISEQUAL+ISLOCK,0)) < 0) {
                prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_13, "Unable to lock current record"));
                prt_cisam_err(iserrno,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_13, "Unable to lock current record"));
		ERROR(' ',MENU_FILE,"Unable to lock current record");
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_update - 2\n");
	TRACE(trace_str);
#endif
		return;
	}
	memcpy(req_buf,(char *)temp_mf_ptr,sizeof(struct cisam_mf));
	if((db(MENU_FILE_ID,UPDATE_REC,&mf_key2,0,0)) < 0)  {
                prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_14, "Unable to update new record to menu file"));
                prt_cisam_err(iserrno,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_14, "Unable to update new record to menu file"));
		ERROR(' ',MENU_FILE,"Unable to update new record to menu file");
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_update - 4\n");
	TRACE(trace_str);
#endif
		return;
	}

	prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_15, "Record updated"));

	if((db(MENU_FILE_ID,RELEASE,&mf_key2,0,0)) < 0)  {
                prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_16, "Unable to release current record"));
                prt_cisam_err(iserrno,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_16, "Unable to release current record"));
		ERROR(' ',MENU_FILE,"Unable to release current record");
		prt_hdr();
		clear_desc_line();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_update - 5\n");
	TRACE(trace_str);
#endif
		return;
	}

	cur_list = 1;		/* valid row in current list */
        prt_hdr();		/* Display the menu file maintenance screen header line */
	clear_desc_line();

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_update - 6\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of process_update()  */

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
		prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_17, "Unable to open output file\n"));
		ERROR(' ',MENU_FILE,"Unable to open output file");
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
        char     open_brace[2],  close_brace[2];

#ifdef DEBUG
	sprintf(trace_str,"Entering prt_labels\n");
	TRACE(trace_str);
#endif

	strcpy(open_brace, catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_53, "[" ));
	strcpy(close_brace, catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_54, "]" ));
	/* print the screen labels  */
	attrset(A_BOLD);
	mvaddstr(L_M_NBR_ROW,L_M_NBR_COL,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_19, "Number"));
	mvaddstr(L_M_NBR_ROW,L_CMD_COL,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_20, "Command"));
	mvaddstr(L_M_NBR_ROW,L_LST_MENU_COL,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_21, "Last Menu Item"));
	mvaddstr(L_M_NBR_ROW,L_TYPE_COL,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_22, "Type"));
	mvaddstr(L_M_NBR_ROW,L_DESC_COL,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_23, "Description"));
	attrset(0);
	mvaddstr(L_M_NBR_ROW+1,L_M_NBR_COL,open_brace);
	mvaddstr(L_M_NBR_ROW+1,L_M_NBR_COL+MC_NBR+1,close_brace);
	mvaddstr(L_M_NBR_ROW+1,L_CMD_COL,open_brace);
	mvaddstr(L_M_NBR_ROW+1,L_CMD_COL+MC_CMD+1,close_brace);
	mvaddstr(L_M_NBR_ROW+1,L_LST_MENU_COL,open_brace);
	mvaddstr(L_M_NBR_ROW+1,L_LST_MENU_COL+MC_LST_MENU+1,close_brace);
	mvaddstr(L_M_NBR_ROW+1,L_TYPE_COL,open_brace);
	mvaddstr(L_M_NBR_ROW+1,L_TYPE_COL+MC_TYPE+1,close_brace);
	mvaddstr(L_M_NBR_ROW+1,L_DESC_COL,open_brace);
	mvaddstr(L_M_NBR_ROW+1,L_DESC_COL+MC_DESC+1,close_brace);
	attrset(A_BOLD);
	mvaddstr(L_UNIX_CMD_ROW,L_UNIX_CMD_COL,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_24, "UNIX Command"));
	mvaddstr(L_UNIX_CMD_ROW,L_PRE_SCR_COL,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_25, "Previous Screen"));
	attrset(0);
	mvaddstr(L_UNIX_CMD_ROW+1,L_UNIX_CMD_COL,open_brace);
	mvaddstr(L_UNIX_CMD_ROW+1,L_UNIX_CMD_COL+MC_UNIX_CMD+1,close_brace);
	mvaddstr(L_UNIX_CMD_ROW+1,L_PRE_SCR_COL,open_brace);
	mvaddstr(L_UNIX_CMD_ROW+1,L_PRE_SCR_COL+MC_PRE_SCR+1,close_brace);

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

#ifdef DEBUG
	sprintf(trace_str,"Entering prt_data\n");
	TRACE(trace_str);
#endif

	clear_flds();

	/* Fill in the data items on the screen */

	mvprintw(D_M_NBR_ROW,D_M_NBR_COL,"%3d",mf_ptr->nbr);
	mvprintw(D_M_NBR_ROW,D_CMD_COL,"%3d",mf_ptr->command);
	mvprintw(D_M_NBR_ROW,D_LST_MENU_COL,"%c",mf_ptr->last_menu_item);
	mvprintw(D_M_NBR_ROW,D_TYPE_COL,"%c",mf_ptr->type);
	mvprintw(D_M_NBR_ROW,D_DESC_COL,"%s",mf_ptr->desc);
	mvprintw(D_UNIX_CMD_ROW,D_UNIX_CMD_COL,"%s",mf_ptr->unix_string);
	mvprintw(D_UNIX_CMD_ROW,D_PRE_SCR_COL,"%c",mf_ptr->prev_scr_flag);

	refresh();

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_data\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_data  */

/******************************************************************************
* process_input - input data fields.
******************************************************************************/

process_input()
{
	int i;		/* loop index */
	int should_clear = 0;
#ifdef DEBUG
	sprintf(trace_str,"Entering process_input\n");
	TRACE(trace_str);
#endif

	piu = 3;
	while(1)   {
		fld_desc(piu);
		norm_to_rev(stdscr,menu_scr_flds,piu);
		if (piu == 5 || piu == 6)
			upper_case = 0;
		intr_allowed = 1;		/* interrupts allowed */
		key_val = read_field(stdscr,menu_scr_flds,piu,read_buf,&field_entered,&should_clear);
		revund_to_norm(stdscr,menu_scr_flds,piu);
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

		ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);
		if(ret_val == NOT_VALID)
			continue;

		if((key_val == ESC) && (!error_n_fld))
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
* send_ipc - send a message to dispatch that a record in the menu file changed.
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
	df_maint_ipc.which_file = MENU_FILE_ID;
	df_maint_ipc.key = mf_ptr->nbr; 
	df_maint_ipc.cisam_num = isrecnum;         
	if((qid = msgget(key,IPC_CREAT | 0666)) == -1)   {
		prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_26, "Menu file maint msgget failed "));
                prt_unix_err(errno,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_27, "Menu file maint msgget failed"));
		ERROR(' ',MENU_FILE,"Menu file maint msgget failed");
		sleep(3);
#ifdef DEBUG
	sprintf(trace_str,"Leaving send_ipc - 1\n");
	TRACE(trace_str);
#endif
		cleanup();
	}

	if((msgsnd(qid,&df_maint_ipc,sizeof(df_maint_ipc), IPC_NOWAIT)) == -1)  {
		prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_28, "Menu file maint msgsnd failed "));
                prt_unix_err(errno,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_29, "Menu file maint msgsnd failed"));
		ERROR(' ',MENU_FILE,"Menu file maint msgsnd failed");
		sleep(3);
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

} /*  End of send_ipc()  */

/******************************************************************************
* read_request - read users request for options.
*******************************************************************************/

char read_request()
{
	/*  The main header, the labels and the data from the first
	 *  record in the menu file have been displayed.  At this
	 *  point the user must request one of the eight options,
	 *  they are : Q, q, N, n, P, p, U, u, O, o, E, and e.
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

	mvprintw(HDR_ROW_1,HDR_COL_1,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_30, "QUERY: Enter command to execute or INTERRUPT aborts. "));
	refresh();

	clear_flds();

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_qurey_hdr\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_query_hdr()  */

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
	mvaddstr(HDR_ROW_1,HDR_COL_1,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_31, "Query Next Previous Update Output Exit"));
	attrset(A_BOLD);
	mvaddstr(HDR_ROW_2,HDR_COL_2,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_32, "MENU FILE MAINTENANCE"));
	attrset(0);
	refresh();
	
#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_hdr\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of prt_hdr()  */

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
	mvaddstr(HDR_ROW_1,HDR_COL_1,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_33, "UPDATE: ESC executes. INTERRUPT aborts."));
	refresh();

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_upd_hdr\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_upd_hdr() */

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
	mvaddstr(HDR_ROW_1,HDR_COL_1,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_34, "Enter output file (default is menu_scr.out): "));
	refresh();
	getstr(o_file);
	if(!strcmp(o_file,""))
		strcpy(o_file,"menu_scr.out");

#ifdef DEBUG
	sprintf(trace_str,"Leaving prompt_fname\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prompt_fname()  */

/******************************************************************************
* prompt_mode - prompt the user for file mode Create or Append.
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
		mvprintw(HDR_ROW_1,HDR_COL_1,"%s %s  %s", 
				catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_35, 
				"Output file"), o_file, 
				catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_36, 
				"Append or Create (default : C) ?") );

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
				prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET,
					MEN_MAINT_55, 
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
	sprintf(trace_str,"%s %d\n", catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_37, "C-isam Error Number ="), iserrno);
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
	sprintf(trace_str,"Entering prt_unix_err\n");
	TRACE(trace_str);

	sprintf(trace_str,"%s, ",u_err_msg);
	TRACE(trace_str);
	sprintf(trace_str,"%s %d\n", catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_38, "Unix Error Number ="), u_err);
	TRACE(trace_str);

	sprintf(trace_str,"Leaving prt_unix_err\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_unix_err()  */

/******************************************************************************
* init - open files, initialize variables, setup signals, prepare curses.
*******************************************************************************/

init()
{
#ifdef DEBUG
	if((trace_fp = fopen(TRACE_FILE,"w")) == NULL)  {
		prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_39, "Unable to open trace file "));
		ERROR(' ',MENU_FILE,"Unable to open trace file");
		sleep(3);
#ifdef DEBUG
	sprintf(trace_str,"Leaving init - 1\n");
	TRACE(trace_str);
#endif
		cleanup();
	}

	sprintf(trace_str,"Entering init\n");
	TRACE(trace_str);
#endif

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

	if((offset = get_offsets()) == 0)  {
		prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_41, "Unable to attach shared memory"));
		sleep(3);
		cleanup();
	}

	signal(SIGQUIT,SIG_IGN);
	signal(SIGINT,process_intr);
	signal(SIGTERM,cleanup);
	signal(SIGHUP,cleanup);
	signal(SIGPIPE,cleanup);
	signal(SIGTSTP, SIG_IGN);

	mf_ptr = (struct cisam_mf *) req_buf;

	if((db(MENU_FILE_ID,OPEN,&mf_key2,ISINOUT+ISMANULOCK,0)) < 0)  {   
                prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_42, "Unable to open menu file"));
		sleep(3);
                prt_cisam_err(iserrno,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_42, "Unable to open menu file"));
		ERROR(' ',MENU_FILE,"Unable to open menu file");
#ifdef DEBUG
	sprintf(trace_str,"Leaving init - 2\n");
	TRACE(trace_str);
#endif
		cleanup();
	} else
		file_open = 1;

#ifdef DEBUG
	sprintf( tracestr, "Leaving init -3\n");
	TRACE( trace_str );
#endif
	return;

}  /*  End of init()  */

/******************************************************************************
* cleanup - if an error has occured, close files and terminate curses.
*******************************************************************************/

cleanup()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering cleanup\n");
	TRACE(trace_str);
#endif

	if (file_open)
		if((db(MENU_FILE_ID,CLOSE,&mf_key2,ISEQUAL,0)) < 0)  {
			prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_43, "Unable to close menu file"));
			prt_cisam_err(iserrno,catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_43, "Unable to close menu file"));
			ERROR(' ',MENU_FILE,"Unable to close menu filel");
		}

#ifdef DEBUG
	sprintf(trace_str,"Leaving cleanup\n");
	TRACE(trace_str);

	if(fclose(trace_fp) == -1) 
		prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_45, "Unable to close trace file"));
#endif

	shmdt(offset);

	nocbreak();
	echo();
	erase();
	refresh();
	endwin();

	catclose(COMMON_m_catd);
	catclose(MEN_MAINT_m_catd);
	exit(0);

}  /*  End of cleanup()  */

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
		menu_cl_flds(i); 

#ifdef DEBUG
	sprintf(trace_str,"Leaving clear_flds\n");
	TRACE(trace_str);
#endif
}

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
}

/******************************************************************************
* process_intr - process the interrupt signal.
*******************************************************************************/

process_intr()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering process_intr\n");
	TRACE(trace_str);
#endif
	/* This routine is called whcn an interrupt signal
	   is received.  A flag is set to indicate that this
	   signal was received.  The flag end_read_field is set
	   so the read_field routine will terminate. */

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
}  /* End of process_intr()  */

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
		case F_M_NBR :
			addstr(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_46, "Sequential number used for permission cross_reference"));
			break;
		case F_CMD :
			addstr(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_47, "A command string assigned to each menu item"));
			break;
		case F_LST_MENU :
			addstr(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_48, "Y/N.  Whether this menu item is the last one on the menu or not."));
			break;
		case F_TYPE :
			addstr(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_49, "A menu item may be a \'M\'enu, \'E\'xternal executable, \'I\'nternal executable."));
			break;
		case F_DESC :
			addstr(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_50, "The actual character description that will be desplayed on the menu"));
			break;
		case F_UNIX_CMD :
			addstr(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_51, "UNIX command to be executed if menu item is external executable"));
			break;
		case F_PRE_SCR :
			addstr(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_52, "Previous screen flag"));
			break;
	}
	refresh();

#ifdef DEBUG
	sprintf(trace_str,"Leaving fld_desc \n");
	TRACE(trace_str);
#endif
}  /* End of fld_desc()  */
