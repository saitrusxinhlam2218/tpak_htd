static char sz__FILE__[]="@(#)cl_dr_srch.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  cl_dr_srch.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:47:08
 * @(#)  Last delta: 12/2/94 at 18:01:53
 * @(#)  SCCS File: /taxi/sccs/s.cl_dr_srch.c
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

/*******************************************************************************/
/* dr_search: gets the next or the previous 17 (probably less) vehicle records */
/*	 from the disk (only those records that meet the selection criteria    */
/*	 are selected).		 Returns the number of the records selected.   */
/*******************************************************************************/
extern int page_num;					/* for the search page number */
extern char req_buf[];					/* buffer for r/w records to/from disk */
extern struct keydesc *ptr_srch_key; 			/* the pointer to the search key */
extern struct cisam_dr srch_dr_calls[]; 		/* the array of records that holds 17 calls */
char dr_calls_fleet[SEARCHES_ON_PG];
extern int matches_index;				/* for matches fleet */
extern int (*fnptr_matches)();				/* pointer to function used to test */
							/* if a record read meets the selection criteria.  */
call_dr_search(have_start, got_calls, srch_dir, init_page)
char have_start;					/* 'y' or 'n'; search start from current position  or not */
int *got_calls;						/* pointer to # of calls read */
int srch_dir;						/* search direction, FORWARD or BACKWORD */
char init_page;						/* 'y' or 'n'; search start for first page */
{
	struct cisam_dr *ptr;				/* pointer to be assigned to req_buf[] */
	short i=0, ccode=0;
	short driver_id;

	ptr = (struct cisam_dr *)req_buf;

	if (init_page == YES){								/* 1st page of a search request */
		matches_index = 0;							/* start with 1st fleet */
		if(forward_start_dr_srch(got_calls,ptr))				/* get 1st valid record */
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
			while (srch_dr_calls[i-1].id > 0 && i <= SEARCHES_ON_PG) {	/* find starting point */
				i++;
			}
			if(i > 0){							/* found the last call */
			
				ptr->fleet = srch_dr_calls[i-2].fleet; 			/* fleet id of last record */
				ptr->id = srch_dr_calls[i-2].id;		 	/* driver id */
				strcpy(ptr->name, srch_dr_calls[i-2].name);
				driver_id = srch_dr_calls[i-2].id;
			}

			if(db(DRIVER_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ, 0) < 0) 	/* get last record again */
				return(*got_calls);					/* no more driver records */
			else {
				if(ptr->id != driver_id){
					while(db(DRIVER_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT, 0) >= 0){ /* locate last rec dispd */
						if(ptr->id == driver_id)
							break;
						else{	if(strcmp(ptr->name,srch_dr_calls[i-2].name) != 0)
								break;
						}
					}
				}
			}
			
			*got_calls = 0;
		}


		while (*got_calls < SEARCHES_ON_PG) {						/* more records to find */
			if (db(DRIVER_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0) < 0) {		/* more records in file ? */
				matches_index++;						/* done, next fleet */
				if (forward_start_dr_srch(got_calls,ptr))			/* found a record */
					continue;
				else {
					if (*got_calls) {
						for(i= *got_calls; i<SEARCHES_ON_PG; i++)
							srch_dr_calls[i].id = 0;
						fwd_page_handler();
					}
					return(*got_calls); 
				}
			}
			if ((*fnptr_matches)(ptr, select_criteria.fleet[matches_index])) {
				memcpy(&srch_dr_calls[*got_calls],req_buf, sizeof(struct cisam_dr));
				(*got_calls)++;
			}
			else{
				if(ptr->fleet != select_criteria.fleet[matches_index]){
					matches_index++;				/* done with this fleet, next fleet */
					if (forward_start_dr_srch(got_calls,ptr))	/* found a record to continue from */
						continue;
					else {
						if (*got_calls) {
							for(i= *got_calls; i<SEARCHES_ON_PG; i++)
								srch_dr_calls[i].id = 0;
							fwd_page_handler();
						}
						return(*got_calls); 
					}
				}
			}
	
		}
		if (*got_calls) {
			for(i= *got_calls; i<SEARCHES_ON_PG; i++)
				srch_dr_calls[i].id = 0;
			fwd_page_handler();
		}
		return (*got_calls);
	}

	if ( srch_dir == BACKWARD )  {
		for(i=0; i < FLEET_MAX; i++){
			if(srch_dr_calls[0].fleet == select_criteria.fleet[i])
				break;
		}
	
		matches_index = i;
		ptr->fleet = srch_dr_calls[0].fleet; 
		ptr->id = srch_dr_calls[0].id; 
		strcpy(ptr->name,srch_dr_calls[0].name);
		driver_id = srch_dr_calls[0].id;

		*got_calls = 0;
		db(DRIVER_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ, 0);			/* get 1st record again */
		if(ptr->id != driver_id){
			while(db(DRIVER_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV, 0) >= 0){ /* locate last record displayed*/
				if(ptr->id == driver_id)
					break;
				else{	if(strcmp(ptr->name,srch_dr_calls[0].name) != 0)
						break;
				}
			}
		}
		
		ccode = db(DRIVER_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0);		/* no more for this fleet ? */
		while (*got_calls < SEARCHES_ON_PG) { 					/* get more records */
			if (ccode < 0) {
				matches_index--;					/* done, prev fleet */
				if (backward_start_dr_srch(got_calls,ptr)) {		/* found a record to continue from */
					ccode=db(DRIVER_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0);
					continue;
				}
				else {
					if (*got_calls)
						bkd_page_handler();

					if (*got_calls != SEARCHES_ON_PG && *got_calls > 0){	/* partial page of records ? */
						for (i=0; i <= *got_calls; i++)			/* copy up the records */
							memcpy(&srch_dr_calls[i], &srch_dr_calls[SEARCHES_ON_PG - (*got_calls) + i],
								sizeof(struct cisam_dr));
						for(i= *got_calls; i<SEARCHES_ON_PG; i++)
							srch_dr_calls[i].id = 0;
					}
					return(*got_calls); 
				}
			}
			if ((*fnptr_matches)(ptr, select_criteria.fleet[matches_index])) {
				memcpy(&srch_dr_calls[SEARCHES_ON_PG - (*got_calls) - 1],req_buf, sizeof(struct cisam_dr));
				(*got_calls)++;
				ccode=db(DRIVER_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0);
			}
			else {
				if(ptr->fleet != select_criteria.fleet[matches_index]){	/* still the same fleet ? */
					matches_index--;				/* done with assigned, prev fleet */
					if (backward_start_dr_srch(got_calls,ptr))	/* found a record to continue from */
						ccode=db(DRIVER_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0);
					else
						ccode = -1;
				}
				else
					ccode=db(DRIVER_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0);
			}
		}
		bkd_page_handler();
	}  								/* end if  srch_type == BACKWARDS */
	return(*got_calls); 
}

forward_start_dr_srch(got_calls,ptr) 				/* find first good record for a fleet */
int *got_calls;
struct cisam_dr *ptr;
{
	short i=0;

	if(matches_index < 0){
		matches_index = 0;
		return(NOT_DONE);
	}

	if(matches_index >= FLEET_MAX){
		matches_index = FLEET_MAX - 1;
		return(NOT_DONE);
	}

	for (i=matches_index ; i < FLEET_MAX; i++) {

		matches_index = i;

		if (select_criteria.fleet[matches_index] == '\0')
			continue;

		ptr->fleet = select_criteria.fleet[matches_index]; 		/* fleet id */
		ptr->id = atoi(select_criteria.driverid);			/* driver id */
		strcpy(ptr->name, select_criteria.drivername);

		if(db(DRIVER_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ,0) >= 0){				/* have records to read */
			if ((*fnptr_matches)(ptr, select_criteria.fleet[matches_index])) {		/* check criteria */
				memcpy(&srch_dr_calls[*got_calls],req_buf,sizeof(struct cisam_dr)); 
				(*got_calls)++;
				return(DONE);
			}
		}
	}
	return (NOT_DONE);
}


backward_start_dr_srch(got_calls,ptr) 			/* backward search through to the initial of the first fleet */
int *got_calls;
struct cisam_dr *ptr;
{
	short i=0;

	if(matches_index < 0){
		matches_index = 0;
		return(NOT_DONE);
	}

	if(matches_index >= FLEET_MAX){
		matches_index = FLEET_MAX - 1;
		return(NOT_DONE);
	}

	for ( i=matches_index; i >= 0; i--) {

		matches_index = i;

		if (select_criteria.fleet[matches_index] == '\0') 
			continue;

		ptr->fleet = select_criteria.fleet[matches_index];		/* fleet id */
		ptr->id = atoi(select_criteria.driverid); 			/* driver id */
		strcpy(ptr->name, select_criteria.drivername);

		if (db(DRIVER_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ,0) < 0)			/* have no records to read */
			db(DRIVER_FILE_ID,READ_KEYS,ptr_srch_key,ISLAST,0);

		while (db(DRIVER_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV) >= 0){			/* have records to read */
			if ((*fnptr_matches)(ptr), select_criteria.fleet[matches_index]) { 	/* check criteria */
				memcpy(&srch_dr_calls[SEARCHES_ON_PG - (*got_calls) - 1],req_buf,sizeof(struct cisam_dr)); 
				(*got_calls)++;
				return(DONE);
			}
		} 
	}
	return (NOT_DONE);
}
