static char sz__FILE__[]="@(#)cl_vd_srch.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  cl_vd_srch.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:47:30
 * @(#)  Last delta: 12/2/94 at 18:02:08
 * @(#)  SCCS File: /taxi/sccs/s.cl_vd_srch.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <cursesX.h>
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "call_entry.h"
#include "ui_def.h"
#define SRCH_EXTERNS
#include "srch.h"
#include "ui_error.h"
#include "language.h"

#define FORWARD 	1
#define BACKWARD 	0
#define SEARCHES_ON_PG 	17
#define MAX_INT 	2147483647

/*******************************************************************************/
/* call_search: gets the next or the previous 17 (probably less) calls records */
/*	 from the disk (only those records that meet the selection criteria    */
/*	 are selected).		 Returns the number of the records selected.   */
/*******************************************************************************/

extern time_t srch_dt_tm;					/* starting date time entered by the user */
extern int page_num;					/* for the search page number */
extern char req_buf[];					/* buffer for r/w records to/from disk */
extern struct keydesc *ptr_srch_key; 			/* the pointer to the search key */
extern struct cisam_cl srch_calls[];  			/* the array of calls that holds 17 calls */
extern int matches_index;				/* for matches fleet */
extern int (*fnptr_matches)();				/* pointer to function used to test */
							/* if a record read meets the selection criteria.  */
call_vd_search(have_start, got_calls, srch_dir, init_page)
char have_start;					/* 'y' or 'n'; search start from current position  or not */
int *got_calls;						/* pointer to # of calls read */
int srch_dir;						/* search direction, FORWARD or BACKWORD */
char init_page;						/* 'y' or 'n'; search start for first page */
{
	struct cisam_cl *ptr;				/* pointer to be assigned to req_buf[] */
	short i=0;
	long call_number;

	ptr = (struct cisam_cl *)req_buf;

	if (init_page == YES){								/* 1st page of a search request */
          for (i=0; i < SEARCHES_ON_PG; i++)
            bzero( &srch_calls[i], sizeof( struct cisam_cl ) );
          
		matches_index = 0;							/* start with 1st fleet */
		if(forward_start_vd_srch(got_calls,ptr))				/* get 1st valid record */
			page_num = 0; 							/* initialize the search page number */
		else{
		  	*got_calls = 0;							/* could not find any call records */
			return(*got_calls);
		}
	}

							/* either the search has to start from a current record; or this */
	 						/* is the first time that this search is being done, in which case */
	 						/* the first 17 records should be grabbed */
	if ( srch_dir == FORWARD ){  
	
		if (init_page != YES){							/* not the first page */
			i = 1;
			while (srch_calls[i-1].nbr > 0 && i <= SEARCHES_ON_PG) {	/* find starting point */
				i++;
			}
			if(i > 0){							/* found the last call */
			
				ptr->fleet = srch_calls[i-2].fleet; 			/* fleet id of last record */
				ptr->pri_status = srch_calls[i-2].pri_status;		/* priority of last rec */
				ptr->veh_nbr = srch_calls[i-2].veh_nbr;			/* veh number of last rec */
				ptr->drv_id = srch_calls[i-2].drv_id;			/* veh number of last rec */
				ptr->due_date_time = srch_calls[i-2].due_date_time; 	/* last date/time retrieved */
				call_number = srch_calls[i-2].nbr;			/* call number */
			}

			if(db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ, 0) < 0) 	/* get last record again */
				return(*got_calls);					/* no more calls in call file */
			else {
				if(ptr->nbr != call_number){				/* locate last record */
					while(db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV, 0) >= 0){
						if(ptr->nbr == call_number)
							break;
						else{	if(ptr->due_date_time != srch_calls[i-2].due_date_time)
								break;
						}
					}
				}
			}
			*got_calls = 0;
		}

		while (*got_calls < SEARCHES_ON_PG){					/* more calls to find */
			if(db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0)<0){		/* more calls in file ? */
				matches_index++;					/* done with this fleet, next fleet */
				if (forward_start_vd_srch(got_calls,ptr))		/* found a record to continue from */
					continue;
				else {
					if (*got_calls)
						fwd_page_handler();

					return(*got_calls); 
				}
			}
			if ((*fnptr_matches)(ptr, select_criteria.fleet[matches_index])) {
				memcpy(&srch_calls[*got_calls],req_buf, sizeof(struct cisam_cl));
				(*got_calls)++;
			}
			else{
				matches_index++;				/* done with this fleet, next fleet */
				if (forward_start_vd_srch(got_calls,ptr))	/* found a record to continue from */
					continue;
				else {
					if (*got_calls)
						fwd_page_handler();

					return(*got_calls); 
				}
			}
	
		}
		if (*got_calls)
			fwd_page_handler();
		return (*got_calls);
	}

	if ( srch_dir == BACKWARD )  {
		for(i=0; i < FLEET_MAX; i++){
			if(srch_calls[0].fleet == select_criteria.fleet[i])
				break;
		}
	
		matches_index = i;
		ptr->fleet = srch_calls[0].fleet; 
		ptr->pri_status = srch_calls[0].pri_status;
		ptr->veh_nbr = srch_calls[0].veh_nbr;
		ptr->drv_id = srch_calls[0].drv_id;
		ptr->due_date_time = srch_calls[0].due_date_time; 
		call_number = srch_calls[0].nbr;

		*got_calls = 0;
		db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ, 0);		/* get 1st record again */
		if(ptr->nbr != call_number){
			while(db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV, 0) >= 0){	/* locate last record displayed*/
				if(ptr->nbr == call_number)
					break;
				else{	if(ptr->due_date_time != srch_calls[0].due_date_time)
						break;
				}
			}
		}
		
		while (*got_calls < SEARCHES_ON_PG) {		 				/* get more records */
			if(db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0) < 0){		/* no more for this fleet ? */
				matches_index--;						/* done, prev fleet */
				if (backward_start_vd_srch(got_calls,ptr))			/* found a record */
					continue;
				else {
					if (*got_calls)
						bkd_page_handler();

					if (*got_calls != SEARCHES_ON_PG && *got_calls > 0){	/* partial page of calls ? */
						for (i=0; i <= *got_calls; i++)			/* copy up the records */
							memcpy(&srch_calls[i], &srch_calls[SEARCHES_ON_PG - (*got_calls) + i],
								sizeof(struct cisam_cl));
					}
					return(*got_calls); 
				}
			}
			if ((*fnptr_matches)(ptr, select_criteria.fleet[matches_index])) {
				memcpy(&srch_calls[SEARCHES_ON_PG - (*got_calls) - 1],req_buf, sizeof(struct cisam_cl));
				(*got_calls)++;
			}
			else {
				matches_index--;				/* done with assigned, prev fleet */
				if (backward_start_vd_srch(got_calls,ptr))	/* found a record to continue from */
					continue;
				else {
					if (*got_calls)
						bkd_page_handler();
					return(*got_calls); 
				}	
			}
		}

		bkd_page_handler();
	}  								/* end if  srch_type == BACKWARDS */
	return(*got_calls); 
}


forward_start_vd_srch(got_calls,ptr) 				/* find first good record for a fleet */
int *got_calls;
struct cisam_cl *ptr;
{
	short i=0;

	if(matches_index < 0){
		matches_index = 0;
		return(NOT_DONE);
	}

	if(matches_index > FLEET_MAX){
		matches_index = FLEET_MAX - 1;
		return(NOT_DONE);
	}

	for (i=matches_index ; i < FLEET_MAX; i++) {

		matches_index = i;

		if (select_criteria.fleet[matches_index] == '\0')
			continue;

		ptr->fleet = select_criteria.fleet[matches_index]; 		/* fleet id */
		ptr->pri_status = 64;						/* assigned calls only */
		ptr->veh_nbr = atoi(select_criteria.vhnumber);			/* vehicle number */
		ptr->drv_id = atoi(select_criteria.driverid);			/* driver id */
		ptr->due_date_time = MAX_INT;					/* date/time entered by user */

		if(db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ,0) < 0) {	/* at end of file ? */
			db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISLAST,0);	/* get last record */
			if ((*fnptr_matches)(ptr, select_criteria.fleet[matches_index])) {		/* check criteria */
				memcpy(&srch_calls[*got_calls],req_buf,sizeof(struct cisam_cl)); 
				(*got_calls)++;
				return(DONE);
			}
		}
		else {
			if(db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0) >= 0){			/* have records to read */
				if ((*fnptr_matches)(ptr, select_criteria.fleet[matches_index])) {	/* check criteria */
					memcpy(&srch_calls[*got_calls],req_buf,sizeof(struct cisam_cl)); 
					(*got_calls)++;
					return(DONE);
				}
			}
		} 
	}
	return (NOT_DONE);
}


backward_start_vd_srch(got_calls,ptr) 			/* backward search through to the initial of the first fleet */
int *got_calls;
struct cisam_cl *ptr;
{
	short i=0;

	if(matches_index < 0){
		matches_index = 0;
		return(NOT_DONE);
	}

	if(matches_index > FLEET_MAX){
		matches_index = FLEET_MAX - 1;
		return(NOT_DONE);
	}

	for ( i=matches_index; i >= 0; i--) {

		matches_index = i;

		if (select_criteria.fleet[matches_index] == '\0') 
			continue;

		ptr->fleet = select_criteria.fleet[matches_index];		/* fleet id */
		ptr->pri_status = 64;						/* assigned calls only */
		ptr->veh_nbr = atoi(select_criteria.vhnumber);			/* vehicle number */
		ptr->drv_id = atoi(select_criteria.driverid);			/* driver id */
		ptr->due_date_time = srch_dt_tm;				/* due date/time entered by user */

		db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ,0);		/* prime the search */
		if (db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT) >= 0){			/* have records to read */
			if ((*fnptr_matches)(ptr, select_criteria.fleet[matches_index])) {		/* check criteria */
				memcpy(&srch_calls[*got_calls],req_buf,sizeof(struct cisam_cl)); 
				(*got_calls)++;
				return(DONE);
			}
		} 
	}
	return (NOT_DONE);
}
