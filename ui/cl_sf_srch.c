static char sz__FILE__[]="@(#)cl_sf_srch.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  cl_sf_srch.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:47:19
 * @(#)  Last delta: 12/2/94 at 18:02:00
 * @(#)  SCCS File: /taxi/sccs/s.cl_sf_srch.c
 *                                                                           *
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
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
#include "srch_const.h"
#include "language.h"

extern int sc_key_on;					/* number of the key being used for search*/
extern int page_num;					/* for the search page number */
extern char req_buf[];					/* buffer for r/w records to/from disk */
extern struct keydesc *ptr_srch_key; 			/* the pointer to the search key */
extern struct cisam_sc subs_calls[];  			/* the array of calls that holds 17 calls */
extern int matches_index;				/* for matches fleet */
extern int (*fnptr_matches)();				/* pointer to function used to test */
							/* if a record read meets the selection criteria.  */

/**************************************************************************************/
/* call_subs_search: gets the next or the previous 17 (probably less) calls records   */
/*	 from the disk (only those records that meet the selection criteria           */
/*	 are selected).		 Returns the number of the records selected.          */
/**************************************************************************************/
call_subs_search(have_start, got_calls, srch_dir, init_page)
char have_start;					/* 'y' or 'n'; search start from current position  or not */
int *got_calls;						/* pointer to # of calls read */
int srch_dir;						/* search direction, FORWARD or BACKWORD */
char init_page;						/* 'y' or 'n'; search start for first page */
{
	struct cisam_sc *ptr;				/* pointer to be assigned to req_buf[] */
	long call_number;				/* temporary storage for call number */
	short i=0, j=0, ccode=0;

	ptr = (struct cisam_sc *)req_buf;

	if (init_page == YES){								/* 1st page of a search request */
		matches_index = 0;							/* start with 1st fleet */
		if(forward_start_subs_srch(got_calls,ptr))				/* get 1st valid record */
			page_num = 0; 							/* initialize the search page number */
		else{
		  	*got_calls = 0;							/* could not find any call records */
			return(*got_calls);
		}
	}
							/* either the search has to start from a current record; or this */
	 						/* is the first time that this search is being done, in which case */
	 						/* the first 17 records should be grabbed */
	if (srch_dir == FORWARD ){  
		if (init_page != YES){							/* not the first page */
			i = 1;
			while (subs_calls[i-1].nbr > 0 && i <= SEARCHES_ON_PG) {	/* find starting point */
				i++;
			}
			if(i > 0){							/* found the last call */
				for(j=0; j < FLEET_MAX; j++){
					if(subs_calls[i-2].fleet == select_criteria.fleet[j])
					break;
				}
				matches_index = j;

				ptr->fleet = subs_calls[i-2].fleet; 				/* fleet id of last record */
				ptr->nbr = subs_calls[i-2].nbr;					/* last call number used */
				strncpy(ptr->pckup_str_key,subs_calls[i-2].pckup_str_key,4);	/* last street name retrieved */
				strncpy(ptr->pass_name,subs_calls[i-2].pass_name,4);		/* last pass name retrieved */
				strcpy(ptr->charge_nbr,subs_calls[i-2].charge_nbr);		/* last charge number retrieved */
				strcpy(ptr->phone_nbr,subs_calls[i-2].phone_nbr);		/* last phone number used */
				call_number = subs_calls[i-2].nbr;				/* call number */
			}

			if(db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ, 0) < 0) 	/* get last record again */
				return(*got_calls);					/* no more calls in call file */
			else {
				if(sc_key_on != 1 && ptr->nbr != call_number){
					while(db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0) >= 0){
						if(ptr->nbr == call_number)
							break;
					}
				}
			}
			*got_calls = 0;
		}

		ccode = db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0);		/* more calls in file ? */
		while (*got_calls < SEARCHES_ON_PG) {					/* more calls to find */
			if (ccode < 0) {
				if (sc_key_on != 1) {					/* other then sc_nbr key */
					matches_index++;				/* done with this fleet, next fleet */
					if (forward_start_subs_srch(got_calls,ptr)) {	/* found a record to continue from */
						ccode=db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0);
						continue;
					}
				}
				if (*got_calls){
					for(i= *got_calls; i<SEARCHES_ON_PG; i++)
						subs_calls[i].nbr = 0;
					fwd_page_handler();
				}
				return(*got_calls); 
			}
			if ((*fnptr_matches)(ptr, select_criteria.fleet[matches_index])) {
				memcpy(&subs_calls[*got_calls],req_buf, sizeof(struct cisam_sc));
				(*got_calls)++;
				ccode=db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0);
			}
			else{
				if (sc_key_on == 1) {						/* search by call # */
					ccode=db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0);
					continue;
				}
				if((sc_key_on == 3 && (strncmp(ptr->pckup_str_key,select_criteria.streetname,MIN(strlen(select_criteria.streetname),4))) != 0) ||
				   (sc_key_on == 4 && (strncmp(ptr->pass_name,select_criteria.passengername,MIN(strlen(select_criteria.passengername),4))) != 0) ||
				    ptr->fleet != select_criteria.fleet[matches_index]){
					matches_index++;				/* done with this fleet, next fleet */
					if (forward_start_subs_srch(got_calls,ptr))	/* found a record to continue from */
						ccode=db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0);
					else
						ccode = -1;
				}
				else
					ccode=db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0);
			}
	
		}
		if (*got_calls){
			for(i= *got_calls; i<SEARCHES_ON_PG; i++)
				subs_calls[i].nbr = 0;
			fwd_page_handler();
		}
		return (*got_calls);
	}

	if ( srch_dir == BACKWARD )  {
		for(i=0; i < FLEET_MAX; i++){
			if(subs_calls[0].fleet == select_criteria.fleet[i])
				break;
		}
	
		matches_index = i;
		ptr->fleet = subs_calls[0].fleet; 				/* fleet id of last record */
		ptr->nbr = subs_calls[0].nbr;					/* last call number used */
		strncpy(ptr->pckup_str_key,subs_calls[0].pckup_str_key,4);	/* last street name retrieved */
		strncpy(ptr->pass_name,subs_calls[0].pass_name,4);		/* last pass name retrieved */
		strcpy(ptr->charge_nbr,subs_calls[0].charge_nbr);		/* last charge number retrieved */
		strcpy(ptr->phone_nbr,subs_calls[0].phone_nbr);			/* last phone number used */
		call_number = subs_calls[0].nbr;				/* call number */

		*got_calls = 0;
		db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ, 0);    		/* get 1st record again */
		if(sc_key_on != 1 && ptr->nbr != call_number){
			while(db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0) >= 0){
				if(ptr->nbr == call_number)
					break;
			}
		}

		ccode = db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0);
		while (*got_calls < SEARCHES_ON_PG) { 					/* get more records */
			if (ccode < 0) {
				if (sc_key_on != 1) {					/* other then sc_nbr key */
					matches_index--;				/* done with this fleet, prev fleet */
					if (backward_start_subs_srch(got_calls,ptr)) {	/* found a record to continue from */
						ccode=db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0);
						continue;
					}
				}
				if (*got_calls)
					bkd_page_handler();

				if (*got_calls != SEARCHES_ON_PG && *got_calls > 0){	/* partial page of calls ? */
					for (i=0; i <= *got_calls; i++)			/* copy up the records */
						memcpy(&subs_calls[i], &subs_calls[SEARCHES_ON_PG - (*got_calls) + i],sizeof(struct cisam_sc));
					for(i= *got_calls; i<SEARCHES_ON_PG; i++)
						subs_calls[i].nbr = 0;
				}
				return(*got_calls); 
			}
			if ((*fnptr_matches)(ptr, select_criteria.fleet[matches_index])) {
				memcpy(&subs_calls[SEARCHES_ON_PG - (*got_calls) - 1],req_buf, sizeof(struct cisam_sc));
				(*got_calls)++;
				ccode=db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0);
			}
			else {
				if (sc_key_on == 1) {						/* search by call # */
					ccode=db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0);
					continue;
				}
				if((sc_key_on == 3 && (strncmp(ptr->pckup_str_key,select_criteria.streetname,MIN(strlen(select_criteria.streetname),4))) != 0) ||
				   (sc_key_on == 4 && (strncmp(ptr->pass_name,select_criteria.passengername,MIN(strlen(select_criteria.passengername),4))) != 0) ||
				    ptr->fleet != select_criteria.fleet[matches_index]){	/* still the same fleet ? */
					matches_index--;				/* done with this fleet, prev fleet */
					if (backward_start_subs_srch(got_calls,ptr))	/* found a record to continue from */
						ccode=db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0);
					else {
						ccode = -1;
					}	
				}
				else
					ccode=db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0);
			}
		}
		bkd_page_handler();
	}
	return(*got_calls); 
}


forward_start_subs_srch(got_calls,ptr) 				/* find first good record for a fleet */
int *got_calls;
struct cisam_sc *ptr;
{
	short i=0, ccode=0;

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
		ptr->nbr = atoi(select_criteria.callnumber);
		strncpy(ptr->pckup_str_key,select_criteria.streetname,4);
		strncpy(ptr->pass_name,select_criteria.passengername,4);
		strcpy(ptr->charge_nbr,select_criteria.chargenumber);
		strcpy(ptr->phone_nbr,select_criteria.phonenumber);

		if(db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ,0) >= 0){				/* have records to read */
			if ((*fnptr_matches)(ptr, select_criteria.fleet[matches_index])) {		/* check criteria */
				memcpy(&subs_calls[*got_calls],req_buf,sizeof(struct cisam_sc)); 
				(*got_calls)++;
				return(DONE);
			}
			else{
				if((sc_key_on == 3 && (strncmp(ptr->pckup_str_key,select_criteria.streetname,MIN(strlen(select_criteria.streetname),4))) != 0) ||
				   (sc_key_on == 4 && (strncmp(ptr->pass_name,select_criteria.passengername,MIN(strlen(select_criteria.passengername),4))) != 0) ||
				   (sc_key_on != 1 && ptr->fleet != select_criteria.fleet[matches_index]))
					continue;							/* next fleet */
			}
		}
		ccode = db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ,0);				/* have records to read */
		while (ccode >= 0){									/* have records to read */
			if ((*fnptr_matches)(ptr, select_criteria.fleet[matches_index])) {		/* check criteria */
				memcpy(&subs_calls[*got_calls],req_buf,sizeof(struct cisam_sc)); 
				(*got_calls)++;
				return(DONE);
			}
			else{
				if (sc_key_on == 1) {						/* search by call # */
					ccode=db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0);
					continue;
				}
				if((sc_key_on == 3 && (strncmp(ptr->pckup_str_key,select_criteria.streetname,MIN(strlen(select_criteria.streetname),4))) != 0) ||
				   (sc_key_on == 4 && (strncmp(ptr->pass_name,select_criteria.passengername,MIN(strlen(select_criteria.passengername),4))) != 0) ||
				    ptr->fleet != select_criteria.fleet[matches_index])
					break;							/* done with all for fleet*/
				else
					ccode=db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0);
				
			}
		} 
	}
	return (NOT_DONE);
}


backward_start_subs_srch(got_calls,ptr) 			/* backward search through to the initial of the first fleet */
int *got_calls;
struct cisam_sc *ptr;
{
	short i=0, ccode=0, key_length=0;

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
		ptr->nbr = atoi(select_criteria.callnumber);
		strncpy(ptr->pckup_str_key,select_criteria.streetname,4);
		if (sc_key_on == 3) {							/* street name key */
			key_length = strlen(select_criteria.streetname);		/* how full is the key ? */
			while (key_length < 4){						/* while a partial key */
				ptr->pckup_str_key[key_length] = LAST_UPCASE_LETTER_OF_ALPHABET; /* high order fill */
				key_length++;						/* next position */
			}
		}
		strncpy(ptr->pass_name,select_criteria.passengername,4);
		if (sc_key_on == 4) {							/* passenger name key */
			key_length = strlen(select_criteria.streetname);		/* how full is the key ? */
			while (key_length < 4){						/* while a partial key */
				ptr->pass_name[key_length] = LAST_UPCASE_LETTER_OF_ALPHABET;	/* high order fill */
				key_length++;						/* next position */
			}
		}
		strcpy(ptr->charge_nbr,select_criteria.chargenumber);
		strcpy(ptr->phone_nbr,select_criteria.phonenumber);

		if (db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ,0) < 0)			/* have no records to read */
			db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISLAST,0);

		db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV);
		while (ccode >= 0) {
			if ((*fnptr_matches)(ptr, select_criteria.fleet[matches_index])) {		/* check criteria */
				memcpy(&subs_calls[SEARCHES_ON_PG - (*got_calls) - 1],req_buf,sizeof(struct cisam_sc)); 
				(*got_calls)++;
				return(DONE);
			}
			else {
				if (sc_key_on == 1) {						/* search by call # */
					ccode=db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0);
					continue;
				}
				if((sc_key_on == 3 && (strncmp(ptr->pckup_str_key,select_criteria.streetname,MIN(strlen(select_criteria.streetname),4))) != 0) ||
				   (sc_key_on == 4 && (strncmp(ptr->pass_name,select_criteria.passengername,MIN(strlen(select_criteria.passengername),4))) != 0) ||
			 	   ptr->fleet != select_criteria.fleet[matches_index])
					break;
				else 
					ccode=db(SUBCALL_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0);
			}
		}
	} 
	return (NOT_DONE);
}
