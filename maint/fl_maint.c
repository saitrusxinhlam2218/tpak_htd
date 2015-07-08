static char sz__FILE__[]="@(#)fl_maint.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fl_maint.c; Rel: 6.12.0.0; Get date: 4/14/92 at 16:11:42
 * @(#)  Last delta: 5/23/91 at 15:43:08
 * @(#)  SCCS File: /taxi/sccs/s.fl_maint.c
 *                                                                           *
 *****************************************************************************/

#define DECLARING
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <isam.h>
#include <curses.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>

#include "mads_types.h"
#include "comm_strdefs.h"
#include "fl_m_strdefs.h"
#include "mad_error.h"
#include "key.h"
#include "Fleet_db.h"
#include "fl_maint.h"
#include "fl_scr.h"
#include "literals.h"
#include "language.h"

/*  Declare an array of pointers to functions which check the field entered
 *  for validity.  
 */

extern int fl_cl_flds();
extern int read_field();
extern char buf[];		/* buffer used for interface to db routine */
extern char req_buf[];		/* buffer used for interface to db routine */
extern int iserrno;		/* C-isam error number */
extern int errno;		/* Error number returned from system calls */

char o_file[F_NAME_SIZE];	/* output file as specified by the user */
char mode[2];			/* user specifies Append or Create */

extern int prt_query_hdr(), prt_add_hdr();
extern void prt_scr(), process_intr();
extern void prt_hdr(), prt_labels();
extern char read_request(), prompt_confirm();
extern int prompt_fname(), prompt_mode();
extern void prt_data(), process_input(), send_ipc();
extern void prt_upd_hdr(), prt_error(), prt_cisam_err();
extern void prt_unix_err(), clear_flds();
extern void clear_err_line(), clear_hdr_line();
extern int process_query(), process_next();
extern int process_previous(), process_add();
extern int process_update(), process_remove(), process_output();
void cleanup();
void Cleanup(int sig_num);
void init();

int fl_flag = 0;
int ret_val;			/* return value  */
int intr_allowed;		/* interrupts allowed */
int saw_intr;			/* indicates an interrupt signal was entered */
char intr_char;			/* interrupt character */
int end_read_field;		/* flag, 0/1, set to end read_field function */
short ipc_required;		/* Set if a field changed which is in shared memory */
short up_disp = 1;		/* Set if unable to get shared memory -- dont try to send to dispatch */
char read_buf[BUFFER_SIZE];	/* buffer for reading fields */
int piu = 3;			/* field number to start reading input from */
int field_entered;		/* field entered or not */
int key_val;			/* value of key entered */
int cur_list;			/* Indicates a row is in the current list */
int record_nbr;			/* C-ISAM record number */
int err_n_fld;			/* set if an error in field condition exists */
int doing_add;			/* performing add operation */
int scr_nbr;			/* screen number */
int file_open;			/* set when fleet file is opened and check in cleanup */

struct df_maint df_maint_ipc;	/* structure for passing information 
				   to dispatch  */
struct cisam_fl *fl_ptr;	/* pointer to driver file structure */
struct cisam_fl *temp_fl_ptr;	/* pointer to driver file structure */
struct cisam_fl temp_fl_buf;	/* pointer to driver file structure */

struct offsets *offset;		/* shared memory offset for redundant db routine */
struct offsets *get_offsets();


/*****************************************************************************
*  fl_maint - main for the fleet file maintenance program.
*******************************************************************************/

main(argc,argv)
int argc;
char **argv;
{

	char request;		/*  User request : Q, N, P, A, U, R, S, O, or E */
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
	open_catalog(&FL_MAINT_m_catd, "fl_maint", 0, 0);
	open_catalog(&LITERAL_m_catd, "literals", 0, 0);

	file_open = 0;

	temp_fl_ptr = &temp_fl_buf;
	init_scr_flds();
	init();

	//	db_init();

#ifdef DEBUG
	sprintf(trace_str,"Entering fl_maint\n");
	TRACE(trace_str);
#endif

	prt_hdr();
	prt_labels();

	/*  Get the first record from the driver file */

	fl_ptr->id = ' ';
	if((db(FLEET_FILE_ID,READ_KEYS,&fl_key1,ISGTEQ,0)) < 0)  {
		prt_error(catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_4, "No record found, ADD first !"));
#ifdef DEBUG
	sprintf(trace_str,"Leaving fl_maint - 1\n");
	TRACE(trace_str);
#endif
	}
	else  {

	/*  Display the data from the first record */

		prt_data();

		record_nbr = isrecnum; 	/* save record number for future read */
		cur_list = 1;		/* valid row in current list */
	}

	/*  Input the users request (Query, Next, Previous, Add, Update,
	    Remove, Screen, Output, Exit).  					*/

	 while(request = read_request())  {
			fl_flag = 0;
			err_n_fld = 0;
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

/*** Not in the initial product
*
*			case 'A':	   Add   
*			case 'a':	   Add   
*			clear_err_line();
*				process_add();
*				break;
*								***/

		      case M_UPDATE:
			if (AddUpdateDeleteAllowed())
			  {
			    clear_err_line();
			    process_update();
			  }
			break;

/*** Not in the initial product
*
*			case 'R':	   Remove   
*			case 'r':	   Remove   
*			clear_err_line();
*				process_remove();
*				break;
*								***/

			case M_SCREEN:
			clear_err_line();
				if(scr_nbr == 1)  {
					scr_nbr = 2;
					clear();
					prt_hdr();
					prt_labels();
					if(cur_list)
						prt_data();
				}
				else if(scr_nbr == 2)  {
					scr_nbr = 1;
					clear();
					prt_hdr();
					prt_labels();
					if(cur_list)
						prt_data();
				}
				break;

			case M_OUTPUT:
			clear_err_line();
				process_output();
				break;
		
			case M_EXIT:
#ifdef DEBUG
	sprintf(trace_str,"Leaving fl_maint - 2\n");
	TRACE(trace_str);
#endif
				cleanup();
				break;

			default:
#ifdef DEBUG
	sprintf(trace_str,"Leaving fl_maint - 3\n");
	TRACE(trace_str);
#endif
				cleanup();
				break;
		}
	        if(ipc_required && up_disp)  {
		    send_ipc();
		    ipc_required = 0;
		}
	    }

#ifdef DEBUG
	sprintf(trace_str,"Leaving fl_maint - 4\n");
	TRACE(trace_str);
#endif
	    cleanup();


}  /* End of main */


/******************************************************************************
* init_scr_flds - initialize the strings used in the structure fl_scr_flds
  this struture is declared in fl_scr.h and all other variable initialization is
  done in fl_scr_ary.h, storage space is established in fl_common.c.
*******************************************************************************/


extern struct scr_flds fl_scr_flds[];

init_scr_flds()
{


	fl_scr_flds[0].help = "";
	fl_scr_flds[F_ID].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_1, "Fleet ID");
	fl_scr_flds[F_NBR].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_2, "Fleet number (0-7) assigned by the system, used for internal cross-reference");
	fl_scr_flds[F_ACT].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_3, "Y-Fleet is active, N-Fleet is inactive");
	fl_scr_flds[F_NAME].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_4, "Fleet name");
	fl_scr_flds[F_COMMENT].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_5, "General comment");
	fl_scr_flds[F_REJ_LVL_1].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_6, "Level 1 reject penalty: 1 - not allowed; 2 - no penalty; 3 - bottom of queue");
	fl_scr_flds[F_REJ_PRM].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_7, "Primary reject penalty: 1 - not allowed; 2 - no penalty; 3 - bottom of queue");
	fl_scr_flds[F_REJ_LVL_2].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_8, "Level 2 reject penalty: 1 - not allowed; 2 - no penalty; 3 - bottom of queue");
	fl_scr_flds[F_REJ_LVL_3].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_9, "Level 3 reject penalty: 1 - not allowed; 2 - no penalty; 3 - bottom of queue");
	fl_scr_flds[F_REJ_LVL_4].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_10, "Level 4 reject penalty: 1 - not allowed; 2 - no penalty; 3 - bottom of queue");
	fl_scr_flds[F_REJ_LVL_5].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_11, "Level 5 reject penalty: 1 - not allowed; 2 - no penalty; 3 - bottom of queue");
	fl_scr_flds[F_REJ_LVL_6].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_12, "Level 6 reject penalty: 1 - not allowed; 2 - no penalty; 3 - bottom of queue");
	fl_scr_flds[F_REJ_LVL_7].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_13, "Level 7 reject penalty: 1 - not allowed; 2 - no penalty; 3 - bottom of queue");
	fl_scr_flds[F_REJ_LVL_8].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_14, "Level 8 reject penalty: 1 - not allowed; 2 - no penalty; 3 - bottom of queue");
	fl_scr_flds[F_EXC_LVL_1].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_15, "Level 1 Exclusive Time (In Minutes)");
	fl_scr_flds[F_EXC_PRM].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_16, "Primary Exclusive Time (In Minutes)");
	fl_scr_flds[F_EXC_LVL_2].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_17, "Level 2 Exclusive Time (In Minutes)");
	fl_scr_flds[F_EXC_LVL_3].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_18, "Level 3 Exclusive Time (In Minutes)");
	fl_scr_flds[F_EXC_LVL_4].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_19, "Level 4 Exclusive Time (In Minutes)");
	fl_scr_flds[F_EXC_LVL_5].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_20, "Level 5 Exclusive Time (In Minutes)");
	fl_scr_flds[F_EXC_LVL_6].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_21, "Level 6 Exclusive Time (In Minutes)");
	fl_scr_flds[F_EXC_LVL_7].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_22, "Level 7 Exclusive Time (In Minutes)");
	fl_scr_flds[F_EXC_LVL_8].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_23, "Level 8 Exclusive Time (In Minutes)");
	fl_scr_flds[F_POST_1].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_24, "How often a driver may press the BOOK-IN key. (In Seconds)");
	fl_scr_flds[F_CPOST].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_25, "How often a driver may press the CBOOK key. (In Seconds)");
	fl_scr_flds[F_INQRY_2].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_26, "How often the driver may press the ZONE QUERY key. (In Seconds)");
	fl_scr_flds[F_SRCH].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_27, "How often the zone and bid information is sent to the MDT. (In Seconds)");
	fl_scr_flds[F_INFO].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_28, "How often a driver may press the QP key. (In Seconds)");
	fl_scr_flds[F_ZN_STAT].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_29, "How often the zone status display screen is refreshed. (In Seconds)");
	fl_scr_flds[F_TIME_CALL].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_30, "How often the system checks if any time calls are due. (In Minutes)");
	fl_scr_flds[F_N_SH_1].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_31, "Priority assigned to drivers who pressed the NTRIP key. (1-highest, 5-lowest)");
	fl_scr_flds[F_CNCL_1].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_32, "Priority assigned to drivers whose call was cancelled. (1-highest, 5-lowest)");
	fl_scr_flds[F_REG_PRI].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_33, "Priority assigned to regular drivers. (1-highest, 5-lowest)");
	fl_scr_flds[F_LOCAL_PRI].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_34, "Priority assigned to drivers who completed a local call. (1-highest, 5-lowest)");
	fl_scr_flds[F_ROOKIE].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_35, "Highest priority call that a rookie may handle. (1-highest, 63-lowest)");
	fl_scr_flds[F_N_SH_2].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_36, "1-disp. appr;2-as 1 & auto book;3-auto cancel & book;4-auto cancel, show disp.");
	fl_scr_flds[F_CNCL_2].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_37, "1-dispatcher approval needed;2-as 1 & auto book;3-auto cancel & book");
	fl_scr_flds[F_EPOST_ACT].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_38, "Should system hold calls on primary level if cabs are enroute");
	fl_scr_flds[F_M_ACTION_RETRY].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_39, "Number of M_ACTION retries");
	fl_scr_flds[F_C_OFFER].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_40, "1-implied ACPT on stand & primary;2-implied ACPt;3-explicit ACPT");
	fl_scr_flds[F_ACPT].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_41, "Number of seconds given a driver to press the ACPT key. (In Seconds)");
	fl_scr_flds[F_BREAK].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_42, "Maximum length of a valid break period. (In Minutes)");
	fl_scr_flds[F_SH_METER].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_43, "Shortest allowable meter-on/meter-off interval. (In Minutes)");
	fl_scr_flds[F_M_ACTION_TIMER].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_44, "Duration of M_ACTION timer (In seconds)");
	fl_scr_flds[F_LOCAL_DUR].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_45, "Shortest allowable meter-on/meter-off interval for local calls. (In Minutes)");
	fl_scr_flds[F_C_MATCH].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_46, "Time the system is given to match a call before a TIMEOUT. (In Minutes)");
	fl_scr_flds[F_CANX_TO_MOFF].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_143, "Number of seconds to give driver to turn off meter when a trip has been cancelled");
	fl_scr_flds[F_NS_SHORT].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_143, "Minimum number of minutes required for no show request");
        fl_scr_flds[F_TC_INT].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_144, "Required duration between time call intervals");
	fl_scr_flds[F_N_ACPT_TP].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_47, "1-none;2-bottom of queue;3-unbook and penalize the driver");
	fl_scr_flds[F_N_ACPT_BT].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_48, "Amount of time after a no-accept the driver is disallowed to book. (In Minutes)");
	fl_scr_flds[F_NAK_OFFER].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_124 ,"1-none;2-unbook;3-sign off");
	fl_scr_flds[F_EPOST_LT].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_49, "Maximum allowable time between EBOOK and actual BOOK. (In Minutes)");
	fl_scr_flds[F_TIME_C].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_50, "Amount of notice required for entry of a future call. (In Minutes)");
	fl_scr_flds[F_LATE_M].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_51, "Maximum allowable time between ACPT and PCKUP/M-ON. (In Minutes)");
	fl_scr_flds[F_WAKEUP_LTM].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_52, "Time before due time a WAKE UP call is shown as an exception. (IN Minutes)"); 
	fl_scr_flds[F_CALL_OFFER].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_120, "0-zone only; 1-zone and zone description; 2-as 1 plus map; 3-as 1 plus 100 block; 4-complete address");
	fl_scr_flds[F_CALL_ASSIGN].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_121, "Trip assignment format: 0-Helisinki; 1-Stockholm");
#ifdef NAMERICA
	fl_scr_flds[F_SEND_DEST_ADDR].help = "Should trip destination details be sent in assignment";
	fl_scr_flds[F_SEND_RATE].help = "Should rate information be sent in assignment";
#endif
	fl_scr_flds[F_GPS].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_130, "GPS functions active (Y/N)");
	fl_scr_flds[F_GPS_TIME].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_131, "GPS time: how old can a GPS position be?");
        fl_scr_flds[F_EMERG_RANGE].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_147, "Range for sending emergency broadcast message (meters)");
        fl_scr_flds[F_ENROUTE_BREAK].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_148, "Should Enroute Breaks be allowed (Y/N)");
	fl_scr_flds[F_LOAD_UNLOAD].help = "Should the MDT's LOAD/UNLOAD key be enabled (J/N)"; 
	fl_scr_flds[F_EPOST_AL].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_54, "1-always;2-after PCKUP/M-ON;3=after PCKUP/M-ON on system assigned call");
	fl_scr_flds[F_PRSNLS].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_55, "Whether or not to allow personal cab requests");
	fl_scr_flds[F_REJ_CNT].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_56, "Maximum number of times a call may be rejected before it becomes an exception");
	fl_scr_flds[F_D_ATTR].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_57, "Maximum number of driver attributes a call taker may specify for a call");
	fl_scr_flds[F_V_ATTR].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_58, "Maximum number of vehicle attributes a call taker may specify for a call");
	fl_scr_flds[F_V_CLS].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_59, "Maximum number of vehicles that may be specified to service one call");
	fl_scr_flds[F_FUT_CALL].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_60, "How many days in advance, without approval, a calltaker may enter a time call");
	fl_scr_flds[F_N_KDT].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_61, "Lead time before a Personal call is brought to the Dispatcher's attention. (minutes)");
	fl_scr_flds[F_TM_C_PRC].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_62, "How far into the future should we go to select time calls. (In Minutes)");
	fl_scr_flds[F_CK_SEALS].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_63, "Whether or not to suppress seals checking");
	fl_scr_flds[F_H_LINE].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_64, "Whether the hold line feature is enabled for this fleet");
	fl_scr_flds[F_GRT_LINE].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_65, "Whether or not the greeting lines are to be displayed on the call entry screen");
	fl_scr_flds[F_ST_FLG_P].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_66, "1-not allowed;2-disp. appr;3-auto approval and call reassignment");
	fl_scr_flds[F_FLG_TIME].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_125, "Number of minutes past call due time to allow flag request.");
#ifdef NAMERICA
	fl_scr_flds[F_UNASSGND_LK_AHD].help = "How far into the future should Screen 21 display calls (hours) - '0' for disabled";
	fl_scr_flds[F_SEND_DEST_OFFER].help = "Whether to send destination zone information as component of trip offers";
	fl_scr_flds[F_SEND_PCKUP_CMT].help = "Whether to send pickup address comment information as component of trip offers";
#endif
	fl_scr_flds[F_TIME_CLS_INT_REQ].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_126, "Number of time call intervals to send on F35 requests (1 - MIN, 12 - MAX)");
	fl_scr_flds[F_TIME_CLS_LEAD_SUPP].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_127, "Time call lead time supplement");
	fl_scr_flds[F_Z_BK_LVL].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_67, "How many levels of zones are to be considered when matching a call");
	fl_scr_flds[F_D_MSG_AP].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_68, "1-dispatcher approval required;2-messages automatically submitted");
	fl_scr_flds[F_S_CL_RVW].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_69, "1-dispatcher approval required;2-calls automatically submitted");
	fl_scr_flds[F_CHARGE].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_70, "Default priority for charge calls");
	fl_scr_flds[F_S_CALL].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_71, "Default priority for subscription calls");
	fl_scr_flds[F_TM_CLS].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_72, "Default priority for future calls");
	fl_scr_flds[F_PASS].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_73, "Default priority for passenger calls");
	fl_scr_flds[F_DEL].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_74, "Default priority for delivery calls");
	fl_scr_flds[F_X_TYPE].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_75, "Default priority for type \'X\' calls");
	fl_scr_flds[F_W_KUP].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_76, "Default priority for wakeup calls");
	fl_scr_flds[F_MAN_MET_1].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_77, "Whether a manual meter on/off is allowed on passenger calls");
	fl_scr_flds[F_MAN_MET_2].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_78, "Whether a manual meter on/off is allowed on delivery calls");
	fl_scr_flds[F_MAN_MET_3].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_79, "Whether a manual meter on/off is allowed on typex calls");
	fl_scr_flds[F_R_CALC_1].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_80, "1-none;2-manual;3-zone to zone table;4-zip to zip table");
	fl_scr_flds[F_R_CALC_2].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_80, "1-none;2-manual;3-zone to zone table;4-zip to zip table");
	fl_scr_flds[F_AU_TL_PT].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_81, "Which port the audit trail printer is connected to for this fleet");
	fl_scr_flds[F_T_CL_RPT].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_82, "How often the time calls report will be printed");
	fl_scr_flds[F_MSG_RPT].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_83, "How often the message report will be printed");
        fl_scr_flds[F_ILINK_ACTIVE].help = "Allow transfer of orders from/to Interlink (Y/N)";
        fl_scr_flds[F_ILINK_TIMEOUT].help = "How long the helping system has to find a taxi for an order (0=unlimited)";
	fl_scr_flds[F_CITY].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_84, "Default city code to be used if the user profile of this user does not have one");
	fl_scr_flds[F_A_CODE].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_85, "Default phone area code to be used if this user's profile does not have one");
	fl_scr_flds[F_CL_TYP].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_86, "Default call type to be used if the user's profile does not have one");
	fl_scr_flds[F_H_SRCH].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_87, "Default for historical call searches. (In Minutes)");
	fl_scr_flds[F_GPS_MAX_X].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_137, "Maximum X coordinate allowed for GPS");
	fl_scr_flds[F_GPS_MIN_X].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_138, "Minimum X coordinate allowed for GPS");
	fl_scr_flds[F_GPS_MAX_Y].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_139, "Maximum Y coordinate allowed for GPS");
	fl_scr_flds[F_GPS_MIN_Y].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_140, "Minimum  coordinate allowed for GPS");
	fl_scr_flds[F_GPS_COORD_SYS].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_141, "Coordinate system used for GPS: 1 - Lat/Long ; 2 - UTM");
	fl_scr_flds[F_GPS_BOOK].help = m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_142, "Should system automatically book taxi? (J/N)");
	fl_scr_flds[F_MTCH_ALG].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_88, "1-seq. by zone,temp by cab;2-seq. by zone,cab queue order;3-temp by zone type;4-closest taxi dispatch(GPS);5-zone dispatch in primary/GPS dispatch in backups");
	fl_scr_flds[F_1ST_EST].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_89, "First upper limit range. (In Minutes)");
	fl_scr_flds[F_1ST_TXT].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_90, "Text corresponding to first upper limit range(e.g. 10-15)");
	fl_scr_flds[F_2ND_EST].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_91, "Second upper limit range. (In Minutes)");
	fl_scr_flds[F_2ND_TXT].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_92, "Text corresponding to second upper limit range(e.g. 10-15)");
	fl_scr_flds[F_3RD_EST].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_93, "Third upper limit range. (In Minutes)");
	fl_scr_flds[F_3RD_TXT].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_94, "Text corresponding to third upper limit range(e.g. 10-15)");
	fl_scr_flds[F_4TH_EST].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_95, "Fourth upper limit range. (In Minutes)");
	fl_scr_flds[F_4TH_TXT].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_96, "Text corresponding to fourth upper limit range(e.g. 10-15)");
	fl_scr_flds[F_5TH_EST].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_97, "Fifth upper limit range. (In Minutes)");
	fl_scr_flds[F_5TH_TXT].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_98, "Text corresponding to fifth upper limit range(e.g. 10-15)");
	fl_scr_flds[F_6TH_EST].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_99, "Sixth upper limit range. (In Minutes)");
	fl_scr_flds[F_6TH_TXT].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_100, "Text corresponding to sixth upper limit range(e.g. 10-15)");
	fl_scr_flds[F_7TH_EST].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_101, "Seventh upper limit range. (In Minutes)");
	fl_scr_flds[F_7TH_TXT].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_102, "Text corresponding to seventh upper limit range(e.g. 10-15)");
	fl_scr_flds[F_8TH_EST].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_103, "Eighth upper limit range. (In Minutes)");
	fl_scr_flds[F_8TH_TXT].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_104, "Text corresponding to eighth upper limit range(e.g. 10-15)");
	fl_scr_flds[F_DEFAULT].help =  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_105, "Default estimate text, if no ranges entered");
	fl_scr_flds[F_INACTIVE_RETRY_INTERVAL].help =
	  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_108,
		    "How often to check for inactive vehicles (minutes)");

	fl_scr_flds[F_INACTIVE_TIME_OUT_PERIOD].help =
	  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_109,
		    "Number of minutes that a vehicle may be inactive before being logged out");

	fl_scr_flds[F_APPROACH_TARIFF].help =
	  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_110,
		    "1-No approach tariff; 2-Meter on before pickup");

	fl_scr_flds[F_NUM_LEVELS_TO_COMPUTE_STATS].help =
	  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_111,
		    "Number of backup levels to compute pickup time estimation (1 - 8)");

	fl_scr_flds[F_STATS_COMPUTE_FREQ].help =
	  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_112,
		    "How often to compute pickup time estimates (minutes)");

	fl_scr_flds[F_HANDLE_SYSTEM_ASSIGNED_CALLS].help =
	  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_113,
		    "How to handle system calls: 1-Force acceptance of system assignments; 2-Ignore system assignments");
#ifdef SCANDINAVIA
	fl_scr_flds[F_FLAT_RATE_FILTER].help =
	  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_129,
		    "Flat Rate Distance Filter Active (Y/N)");
#endif
	fl_scr_flds[F_SEND_CALL_TYPE].help =
	  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_135,
		    "Whether the call type will be sent in the trip offer (Y/N)");

	fl_scr_flds[F_AUTO_REJECT_TIME].help =
	  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_136,
		    "Allowed aging on a call for determination of auto-reject approval (secs)");

	fl_scr_flds[F_GPS_DISTANCE_CEILING].help =
	  m_catgets(FL_MAINT_m_catd, FL_SCR_FLDS_SET, FL_SCR_FLDS_132,
		    "GPS distance ceiling used in matching algorithm (meters)");
	
}



/******************************************************************************
* init - open files, initialize variables, setup signals, prepare curses.
*******************************************************************************/

void init()
{
#ifdef DEBUG
	if((trace_fp = fopen(TRACE_FILE,"w")) == NULL)  {
		prt_error(TRACE_OPEN_ERR);
		sleep(3);
		ERROR(' ',FLEET_FILE,TRACE_OPEN_ERR);
		cleanup();
	}

	sprintf(trace_str,"Entering init\n");
	TRACE(trace_str);
#endif

	intr_allowed = 0;	/* interrupts not allowed */
	saw_intr = 0;		/* an interrupt signal has not been entered */
	cur_list = 0;		/* no row in current list */
	scr_nbr = 1;		/* start with screen number 1 */
	initscr();
	noecho();
/*
        keypad(stdscr,TRUE);
*/
        term_setup(getenv("TERM"));
        config_tty();

	cbreak();

	if((offset = get_offsets()) == 0)  {
		up_disp = 0;
		prt_error(NO_ATTACH);
		sleep(3);
/*		cleanup(); */
	}

	signal(SIGQUIT,SIG_IGN);
	signal(SIGINT,process_intr);
	signal(SIGTERM,Cleanup);
	signal(SIGHUP,Cleanup);
	signal(SIGPIPE,Cleanup);
	signal(SIGTSTP, SIG_IGN);

	fl_ptr = (struct cisam_fl *) req_buf;

	if((db(FLEET_FILE_ID,OPEN,&fl_key1,ISINOUT+ISMANULOCK,0)) < 0)  {   
		prt_error(FLEET_OPEN_ERR);
		sleep(3);
		prt_cisam_err(iserrno,FLEET_OPEN_ERR);
		ERROR(' ',FLEET_FILE,FLEET_OPEN_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving init - 1\n");
	TRACE(trace_str);
#endif
		cleanup();
	} else
		file_open = 1;

#ifdef DEBUG
	sprintf(trace_str,"Leaving init - 2\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of init()  */

int
AddUpdateDeleteAllowed()
{
  struct passwd *PasswordEntry;
  struct group  *ManagerGroupEntry;
  char   **pGroupNames;
  uid_t  UserUID;
  short  done = FALSE;

  return(TRUE);
  
  UserUID = getuid();
  PasswordEntry = getpwuid(UserUID);
  ManagerGroupEntry = getgrgid((gid_t)500);
  pGroupNames = (char **)ManagerGroupEntry->gr_mem;
  done = FALSE;
  while ((*pGroupNames != NULL) && (!done))
    {
      if (!strcmp(PasswordEntry->pw_name, *pGroupNames))
	{
	  done = TRUE;
	}
      else
	pGroupNames++;

    }
  return (done);
}

/******************************************************************************
* cleanup - if an error has occured, close files and terminate curses.
*******************************************************************************/

void cleanup()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering cleanup\n");
	TRACE(trace_str);
#endif

	if (file_open)
		if((db(FLEET_FILE_ID,CLOSE,&fl_key1,ISEQUAL,0)) < 0)  {
			prt_error(FLEET_CLOSE_ERR);
			prt_cisam_err(iserrno,FLEET_CLOSE_ERR);
			ERROR(' ',FLEET_FILE,FLEET_CLOSE_ERR);
		}

#ifdef DEBUG
	sprintf(trace_str,"Leaving cleanup\n");
	TRACE(trace_str);

	if(fclose(trace_fp) == -1)   {
		prt_error(TRACE_CLOSE_ERR);
		sleep(3);
		ERROR(' ',FLEET_FILE,TRACE_CLOSE_ERR);
	}
#endif

	shmdt(offset);

	nocbreak();
	echo();
	erase();
	refresh();
	endwin();

	catclose(COMMON_m_catd);
	catclose(FL_MAINT_m_catd);
	catclose(LITERAL_m_catd);

	exit(0);

}  /*  End of cleanup()  */

void Cleanup(int sig_num) {
	cleanup();
}
