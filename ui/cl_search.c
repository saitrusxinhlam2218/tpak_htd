static char sz__FILE__[]="@(#)cl_search.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  cl_search.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:47:15
 * @(#)  Last delta: 12/2/94 at 18:01:58
 * @(#)  SCCS File: /taxi/sccs/s.cl_search.c
 *                                                                           *
 *****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <cursesX.h>
#include <sys/types.h>
#include "ui_strdefs.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "call_entry.h"
#define SRCH_EXTERNS
#include "srch.h"
#include "language.h"

#define FORWARD 1
#define BACKWARD 0
#define SEARCHES_ON_PG 17

/* call_search: gets the next or the previous 17 (probably less) calls records 
 *	 from the disk (only those records that meet the selection criteria
 *	 are selected).		 Returns the number of the records selected.
 */

int bottom_of_page;
int top_of_page;
extern char req_buf[];	/* buffer for r/w records to/from disk */
extern struct keydesc *ptr_srch_key;
extern struct cisam_cl srch_calls[SEARCHES_ON_PG];  /* the array suppose for 17 */
extern int matches_index;	/* for matches function */
extern int (*fnptr_matches)();	/* pointer to function used to test
				   if a record read meets the selection
				   criteria.    */

call_search(have_start, got_calls, srch_dir)
char have_start;	/* 'y' or 'n'; search start from current
						 position  or not */
int *got_calls;		/* pointer to # of calls read */
int srch_dir;		/* search direction, FORWARD or BACKWORD */
{
	int	got_start;
	int	ii;
	struct cisam_cl *ptr;	/* pointer to be assigned to req_buf[] */
	struct cisam_cl *temp_req_buf_ptr; 

	/* check if the record from which the search should start is current */
	if ( have_start == NO )
	{
		/* we do not have the record from which the search should 
		 * start; since the search was invoked by pressing the
		 * <PREV_SCR> key. So get the record from which the search 
		 * should start
		 */
		/* display the record which saved on srch_calls[] */
		/* try to let current_position go to the srch_calls[16] */
	}

	ptr = (struct cisam_cl *)req_buf;

	/* either the search has to start from a current record; or this
	 * is the first time that this search is being done, in which case
	 * the first 17 records should be grabbed
	 */
	if ( srch_dir == FORWARD )  
	{
		if(bottom_of_page)
		{
			while ( *got_calls < SEARCHES_ON_PG )
			{
				/* call db routine to extract the calls record
				 * with key being defined by ptr_srch_key.
				 */
				ptr->fleet = select_criteria.fleet[matches_index]; 
				ptr->nbr = atoi(select_criteria.callnumber);
				strncpy(ptr->pckup_str_key, select_criteria.streetname, 4);
				strncpy(ptr->pass_name_key, select_criteria.passengername, 4);
				strncpy(ptr->charge_nbr, select_criteria.chargenumber, strlen(select_criteria.chargenumber));
				ptr->due_date_time = 0;
				if(db(CALL_FILE_ID,READ_KEYS,(struct keydesc *)ptr_srch_key,ISNEXT,0)<0)
				/* cannot get any more records from disk */
				{
					mvprintw(23,0,catgets(UI_m_catd, UI_1, UI_CL_SEARCH_1, "NO CALLS RECORD FOUND"));
					bottom_of_page = 0;
					return (*got_calls);
				}

				/* check if this call, meets other selection criteria */
				if ( (*fnptr_matches)(ptr) == 1 )  
				{
					/* call read matches selection criteria */
					memcpy(&srch_calls[*got_calls],req_buf, sizeof(struct cisam_cl));
					(*got_calls)++;
				}  /* end if */
			}/* end while */
		}/* end bottom of page */
		else  /* not bottom of page */
		{
			ptr->fleet = srch_calls[16].fleet; 
			ptr->due_date_time = srch_calls[16].due_date_time; 
			ptr->veh_nbr = srch_calls[16].veh_nbr; 
			ptr->drv_id = srch_calls[16].drv_id; 
			ptr->pri_status = srch_calls[16].pri_status; 
			strcpy(ptr->pckup_str_key,srch_calls[16].pckup_str_key); 
			strcpy(ptr->pass_name_key,srch_calls[16].pass_name_key); 
			strcpy(ptr->charge_nbr,srch_calls[16].charge_nbr); 
			ptr->nbr = srch_calls[16].nbr; 

			if (db(CALL_FILE_ID,READ_KEYS,(struct keydesc *)ptr_srch_key,ISEQUAL, 0) < 0)
			{
				mvprintw(23,0,catgets(UI_m_catd, UI_1, UI_CL_SEARCH_2, "CAN NOT FIND THE RECORD WHICH HAVE THE SAME KEY WITH BOTTOM OF PAGE"));
				return(0);
			}
			while ( ptr->nbr != srch_calls[16].nbr ) 
			{
				if (db(CALL_FILE_ID,READ_KEYS,(struct keydesc *)ptr_srch_key,ISNEXT, 0) < 0)
				{
					mvprintw(23,0,catgets(UI_m_catd, UI_1, UI_CL_SEARCH_3, "CAN NOT FIND THE RECORD OF BOTTOM OF PAGE"));
					return(0);
				}
			}
			*got_calls = 0;
			while ( *got_calls < SEARCHES_ON_PG )
			{
				if(db(CALL_FILE_ID,READ_KEYS,(struct keydesc *)ptr_srch_key,ISNEXT,0)<0)
				{
					mvprintw(23,0,catgets(UI_m_catd, UI_1, UI_CL_SEARCH_4, "CANNOT GET ANY MORE RECORDS FROM DISK"));
					return(0);
				}

				/* check if this call, meets other selection criteria */
				if ( (*fnptr_matches)(ptr) == 1 )  
				{
					/* call read matches selection criteria */
					memcpy(&srch_calls[*got_calls],req_buf,
							sizeof(struct cisam_cl));
					(*got_calls)++;
				}  /* end if */
			}
		}
		top_of_page = 0;
		bottom_of_page = 1;
		return(*got_calls);
	}
	else	/* search direction is BACKWARDS; eg: <PREV_PG> */
	{
		if (top_of_page)
		{
			ii = SEARCHES_ON_PG;
			*got_calls = 0;
			while ( *got_calls < SEARCHES_ON_PG )
			{
				/* get to the first record which is on display on
				 * this screen; then from there on save the 17 older
				 * records 
				 */
				if ( db(CALL_FILE_ID,READ_KEYS,(struct keydesc *)ptr_srch_key, ISPREV,0) < 0 ) 
				{
					mvprintw(23,0,catgets(UI_m_catd, UI_1, UI_CL_SEARCH_5, "NO MORE CALLS RECORDS FOUND"));
					bottom_of_page = 0;
					return(0);
				}
				ptr = (struct cisam_cl *)req_buf;
				ptr->fleet = select_criteria.fleet[matches_index]; 

				/* if selection criteria matched */  
				if ( (*fnptr_matches)(ptr) == 1 )  
				{
					memcpy(&srch_calls[ii-1],req_buf,
						sizeof(struct cisam_cl));
					ii--;
					(*got_calls)++;
				}
			}  /* end while */
		}
		else  /* not top of page */
		{
			ptr = (struct cisam_cl *)req_buf;
			ptr->fleet = srch_calls[0].fleet; 
			ptr->due_date_time = srch_calls[0].due_date_time; 
			ptr->veh_nbr = srch_calls[0].veh_nbr; 
			ptr->drv_id = srch_calls[0].drv_id; 
			ptr->pri_status = srch_calls[0].pri_status; 
			strcpy(ptr->pckup_str_key,srch_calls[0].pckup_str_key); 
			strcpy(ptr->pass_name_key,srch_calls[0].pass_name_key); 
			strcpy(ptr->charge_nbr,srch_calls[0].charge_nbr); 
			ptr->nbr = srch_calls[0].nbr; 

			if (db(CALL_FILE_ID,READ_KEYS,(struct keydesc *)ptr_srch_key,ISEQUAL, 0) < 0)
			{
				prt_error(0,catgets(UI_m_catd, UI_1, UI_CL_SEARCH_6, "no calls record found"));
				return(0);
			}
			while ( ptr->nbr != srch_calls[0].nbr) 
			{
				if (db(CALL_FILE_ID,READ_KEYS,(struct keydesc *)ptr_srch_key,ISNEXT, 0) < 0)
				{
					prt_error(0,catgets(UI_m_catd, UI_1, UI_CL_SEARCH_6, "no calls record found"));
					return(0);
				}
			}
			*got_calls = 0;
			while ( *got_calls < SEARCHES_ON_PG )
			{
				if(db(CALL_FILE_ID,READ_KEYS,(struct keydesc *)ptr_srch_key,ISPREV,0)<0)
				/* cannot get any more records from disk */
				{
					mvprintw(23,0,catgets(UI_m_catd, UI_1, UI_CL_SEARCH_5, "NO MORE CALLS RECORD FOUND"));
					return(0);
				}

				/* check if this call, meets other selection criteria */
				if ( (*fnptr_matches)(ptr) == 1 )  
				{
					/* call read matches selection criteria */
					memcpy(&srch_calls[16 - (*got_calls)],req_buf,
							sizeof(struct cisam_cl));
					(*got_calls)++;
				}  /* end if */
			}
		}
		top_of_page = 1;
		bottom_of_page = 0;
		return(*got_calls);
	}  /* end if  srch_type == BACKWARDS */

}  /* end if search_calls() */
