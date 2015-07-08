static char sz__FILE__[]="@(#)mul_key.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  mul_key.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:49:21
 * @(#)  Last delta: 12/2/94 at 18:31:50
 * @(#)  SCCS File: /taxi/sccs/s.mul_key.c
 *                                                                           *
 *****************************************************************************/

#include <ctype.h>
#include <sys/types.h>
#include <cursesX.h>
#include "except.h"
#include "mads_types.h"
#include "mad_ipc.h"
#include "screens.h"
#include "ui_error.h"
#include "user.h"
#include "mads_isam.h"
#include "ex_key.h"
#include "df_maint.h"
#include "writer.h"
#include "srch.h"
#include "mad_error.h"
#include "path.h"
#include "language.h"
#include "ui_strdefs.h"

int	mult_call = 0;

extern	int	own_isrecnum;
extern	int 	scr_name;
extern int 	user_index;			/* index into users struct in shared memory */
extern int 	uid;				/* users system id */
extern char 	req_buf[];
extern struct 	cisam_cl 	srch_calls[];	/* saved call records from search */
extern struct 	users	 	*uiuser[];	/* array of pointers in users struct in shared memory */
extern struct	cisam_cl	call_struct;
extern struct offsets *offset;				/* pointer to offsets struct in shared memory */


multiple_key(srch_index, ptr_ptr)
int srch_index;					/* Index to searches array of calls  */
struct excepts **ptr_ptr;
{
	struct cisam_cl	*cl_ptr;						/* Ptr to call rec read from disk */
	int	indx;								/* Index for call rec in memory */
	struct excepts *ex_ptr;

	cl_ptr = (struct cisam_cl *)req_buf;					/* Set call ptr to isam buffer */
	
	if(scr_name != EXCEPTIONS_SCR) {					/* If wrong screen, get out */
		prt_error(UI_INV_KEY," ");					/* Print error */
		return;								/* Get out */
	}

	if(srch_index == NOT_AVAILABLE)  { 					/* A call must be highlighted */
		prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_MUL_KEY_1, "<no call highlighted>"));			/* If not, print error msg */
		return;								/* Get out */
	}

	if(uiuser[user_index]->menu_perms.exceptions != 1){			/* If user is not a dispatcher get out */
		prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_MUL_KEY_2, "<to assign multiple calls>"));		/* Print error */
		return;								/* Get out */
	}
	   
	if(srch_calls[srch_index].grouped_with)	 {				/* If already a mult call */
		prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_MUL_KEY_3, "<already a multiple call>"));		/* Print error */
		return;								/* Get out */
	}



	/* first find out if the value of 'mult_call' is valid on the current screen */
	ex_ptr = offset->first_except;
	while ( ex_ptr != NULL )
	  {
	    if ( ( ex_ptr->call_nbr == mult_call ) && ( ex_ptr->inuse != '1' ) )
		break;
	    else if ( ( ex_ptr->call_nbr == mult_call ) && ( ex_ptr->inuse == '1' ) )
	      {
		mult_call = 0;
		ex_ptr == NULL;
	      }
	    else
	      ex_ptr = ex_ptr->next;
	  }

	if ( ex_ptr == NULL )
	  mult_call = 0;
	
	if(mult_call == 0)  {							/* First call of group */
		cl_ptr->nbr = srch_calls[srch_index].nbr;			/* Get call # to read in */
		if((db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL+ISLOCK, 0)) < 0)  {	/* read in call */
			prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_MUL_KEY_5, "<call record>"));			/* If error, print error */
			ERROR(' ', CALL_FILE, "Unable to read");		/* Log err to disk */
			return;							/* Get out */
		}

		cl_ptr->grouped_with = cl_ptr->nbr;				/* Set first in group to itself */
		mult_call = cl_ptr->nbr;					/* Store # for future links */
		srch_calls[srch_index].grouped_with = cl_ptr->grouped_with;	/* Change call stored in memory */

		if((db(CALL_FILE_ID, UPDATE_REC, &cl_key1, 0, 0)) < 0)  {	/* Update call on disk */
			prt_error(UI_NOUPDATE,catgets(UI_m_catd, UI_1, UI_MUL_KEY_5, "<call record>"));			/* If error, print error */
			ERROR(' ', CALL_FILE, "Unable to update");		/* Log err to disk */
			return;							/* Get out */
		}

		if(db(CALL_FILE_ID, RELEASE, &cl_key1, 0, 0) < 0)
			prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_MUL_KEY_8, "<error releasing lock calls record>"));
	}
	else {
		cl_ptr->nbr = mult_call;					/* Call # of prev in group */
		if((db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL+ISLOCK, 0)) < 0)  {	/* Read in call */
			prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_MUL_KEY_5, "<call record>"));			/* If error, print error msg */
			ERROR(' ', CALL_FILE, "Unable to read");		/* Log err to disk */
			return; 						/* Get out */
		}
		
		if(cl_ptr->fleet != srch_calls[srch_index].fleet) {		/* Make sure mult calls are in same fleet */
			prt_error(UI_NOFLEET ,catgets(UI_m_catd, UI_1, UI_MUL_KEY_9, "Different fleet"));		/* If error, print error msg */
			return; 						/* Get out */
		}

		mult_call = cl_ptr->grouped_with;				/* Store current # for backward link */
		cl_ptr->grouped_with = srch_calls[srch_index].nbr;		/* Set prev call to group with curr call */

		for(indx = 0; indx < 17; indx++)				/* Find prev call in memory to update */
			if(srch_calls[indx].nbr == cl_ptr->nbr) {		/* Found it */
				srch_calls[indx].grouped_with = cl_ptr->grouped_with;	/* Store curr # in prev call */
				break;						/* Get out of loop */
			}

		if((db(CALL_FILE_ID, UPDATE_REC, &cl_key1, 0, 0)) < 0) {	/* Update call on disk */
			prt_error(UI_NOUPDATE, catgets(UI_m_catd, UI_1, UI_MUL_KEY_5, "<call record>"));		/* if error, print error msg */
			ERROR(' ', CALL_FILE, "Unable to update");		/* Log err to disk */
			return;							/* get out */
		}

		if(db(CALL_FILE_ID, RELEASE, &cl_key1, 0, 0) < 0)
			prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_MUL_KEY_8, "<error releasing lock calls record>"));

		cl_ptr->nbr = srch_calls[srch_index].nbr;			/* Get curr call # */
		if((db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL+ISLOCK, 0)) < 0)  {	/* Read in call rec */
			prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_MUL_KEY_5, "<call record>"));			/* if error, print err msg */
			ERROR(' ', CALL_FILE, "Unable to read");		/* Log err to disk */
			return;							/* Get out */
		}

		cl_ptr->grouped_with = mult_call;				/* Place prev call in group */
		mult_call = cl_ptr->nbr;					/* Save call # for future link */
		srch_calls[srch_index].grouped_with = cl_ptr->grouped_with;	/* Update call in memory */

		if((db(CALL_FILE_ID, UPDATE_REC, &cl_key1, 0, 0)) < 0)  {	/* Update call on disk */
			prt_error(UI_NOUPDATE,catgets(UI_m_catd, UI_1, UI_MUL_KEY_5, "<call record>"));			/* if error, print error msg */
			ERROR(' ', CALL_FILE, "FIFO");				/* Log err to disk */
			return;							/* get out */
		}

		if(db(CALL_FILE_ID, RELEASE, &cl_key1, 0, 0) < 0)
			prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_MUL_KEY_8, "<error releasing lock calls record>"));
	}

/*
 *    fix for multiple call
 *    call history updated when call is entered not when linked
	add_noexcpt_callh(srch_index, R_MULT, ACT_MULT, 0); 			/* Add MULT to call history *
 */

	dsp_cl_detail(srch_index + 1);						/* Re-display call detail lines */

	prt_error(UI_LINKED, "");
}

add_mult_h(call_nbr, fleet)
int	call_nbr;
char	fleet;
{
	struct cisam_ch *ch_ptr;					/* for call history file */
	char date_str[10];						/* for date in display format */
	char time_str[10];						/* for time in display format */

	ch_ptr = (struct cisam_ch *)req_buf;

	memset((char *)ch_ptr,'\0',sizeof(struct cisam_ch)); 		/* initialize the buffer for reading call history rec */

	ch_ptr->call_nbr = call_nbr;  					/* call # */
	ch_ptr->fleet = fleet;

	ch_ptr->exception_nbr = (short)CALL_CANCEL;			/* this is actually the exception type */
	ch_ptr->event_type = EVNT_REG_TYPE;				/* regular function */
	ch_ptr->status_dt_tm = (int)time(NULL);
	get_pkup_date(date_str); 					/* get the date in display format */
	strcpy(ch_ptr->status_date, date_str);
	
	get_pkup_time(time_str); 					/* get the time in display format */
	strcpy(ch_ptr->status_time, time_str);

	ch_ptr->veh_nbr = 0;
	ch_ptr->drv_id = 0;
	ch_ptr->user_id = (short)uid;

	strcpy(ch_ptr->event_desc, ACT_CANCEL);

	if(db(CALLH_FILE_ID, ADD_REC, &ch_key1, 0, 0) < 0)  		/* add the record to disk */
		prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_MUL_KEY_11, "<call history record>"));
}
