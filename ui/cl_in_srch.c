static char sz__FILE__[]="@(#)cl_in_srch.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  cl_in_srch.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:47:12
 * @(#)  Last delta: 12/2/94 at 18:01:55
 * @(#)  SCCS File: /taxi/sccs/s.cl_in_srch.c
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

/**************************************************************************************/
/* get_call:        gets the next or the previous 17 (probably less) calls records    */
/*	 from the disk (only those records that meet the selection criteria           */
/*	 are selected).		 Returns the number of the records selected.          */
/**************************************************************************************/
 
extern time_t srch_dt_tm;					/* starting date time entered by the user */
extern int cl_key_on;					/* number of the key being used for search*/
extern char req_buf[];					/* buffer for r/w records to/from disk */
extern struct keydesc *ptr_srch_key; 			/* the pointer to the search key */
extern struct cisam_cl srch_calls[];  			/* the array of calls that holds 17 calls */
extern int matches_index;				/* for matches fleet */
extern int (*fnptr_matches)();				/* pointer to function used to test */
							/* if a record read meets the selection criteria.  */

get_call(got_calls, srch_dir, init_page)
int *got_calls;						/* pointer to # of calls read */
int srch_dir;						/* search direction, FORWARD or BACKWORD */
char init_page;						/* 'y' or 'n'; search start for first page */
{
	struct cisam_cl *ptr;				/* pointer to be assigned to req_buf[] */
	long call_number;				/* temporary storage for call number */
	short i=0, ccode=0;

	ptr = (struct cisam_cl *)req_buf;

	if (init_page == YES){								/* 1st page of a search request */
		matches_index = 0;							/* start with 1st fleet */
		if(forward_start_in_srch(got_calls,ptr))  {				/* get 1st valid record */
			memcpy(&srch_calls[0],req_buf, sizeof(struct cisam_cl));
			return(*got_calls);
		}
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
			for(i=0; i < FLEET_MAX; i++){
				if(srch_calls[0].fleet == select_criteria.fleet[i])
					break;
			}
			matches_index = i;

			ptr->fleet = srch_calls[0].fleet; 				/* fleet id of last record */
			ptr->nbr = srch_calls[0].nbr;			/* last call number used */
			ptr->due_date_time = srch_calls[0].due_date_time; 		/* last date/time retrieved */
			strncpy(ptr->pckup_str_key,srch_calls[0].pckup_str_key,4);	/* last street name retrieved */
			strncpy(ptr->pass_name_key,srch_calls[0].pass_name_key,4);	/* last pass name retrieved */
			strcpy(ptr->charge_nbr,srch_calls[0].charge_nbr);		/* last charge number retrieved */
			strcpy(ptr->phone_nbr,srch_calls[0].phone_nbr);		/* last phone number used */
			call_number = srch_calls[0].nbr;				/* call number */

			*got_calls = 0;
			if(db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ, 0) < 0) 	/* get last record again */
				return(*got_calls);					/* no more calls in call file */
			else {
				if(cl_key_on != 1 && ptr->nbr != call_number){
					while(db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0) >= 0){
						if(ptr->nbr == call_number)
							break;
						else{
							if(ptr->due_date_time != srch_calls[0].due_date_time)
								break;	
						}
					}
				}
			}
			*got_calls = 0;
		}


		ccode=db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0);
		while ( *got_calls == 0 ) {					/* more calls to find */
			if(ccode < 0) {	
				if (cl_key_on != 1) {				/* not a call number search */
					matches_index++;				/* done, next fleet */
					forward_start_in_srch(got_calls,ptr);		/* found a recordm */
				}
				return(*got_calls); 
			}

			if ((*fnptr_matches)(ptr, select_criteria.fleet[matches_index])) {
				memcpy(&srch_calls[0],req_buf, sizeof(struct cisam_cl));
				*got_calls = 1;
				return(*got_calls);
			}
			else{
				if (cl_key_on == 1) {						/* search by call # ? */
					ccode=db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0);
					continue;
				}
				if((cl_key_on == 5 && (strncmp(ptr->pckup_str_key,select_criteria.streetname,MIN(strlen(select_criteria.streetname),4))) != 0) ||
				   (cl_key_on == 6 && (strncmp(ptr->pass_name_key,select_criteria.passengername,MIN(strlen(select_criteria.passengername),4))) != 0) ||
				   (ptr->fleet != select_criteria.fleet[matches_index])){
					matches_index++;				/* done with unassigned, next fleet */
					forward_start_in_srch(got_calls,ptr);		/* found a record to continue from */
					return(*got_calls); 
				}
				else {
					ptr->due_date_time = srch_dt_tm;
					ccode=db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ,0);
				}
			}
	
		}
		return (*got_calls);
	}

	if ( srch_dir == BACKWARD )  {
		for(i=0; i < FLEET_MAX; i++){
			if(srch_calls[0].fleet == select_criteria.fleet[i])
				break;
		}
		matches_index = i;

		ptr->fleet = srch_calls[0].fleet; 				/* fleet id of last record */
		ptr->due_date_time = srch_calls[0].due_date_time; 		/* last date/time retrieved */
		ptr->nbr = srch_calls[0].nbr;					/* last call number used */
		strncpy(ptr->pckup_str_key,srch_calls[0].pckup_str_key,4);	/* last street name retrieved */
		strncpy(ptr->pass_name_key,srch_calls[0].pass_name_key,4);	/* last pass name retrieved */
		strcpy(ptr->charge_nbr,srch_calls[0].charge_nbr);		/* last charge number retrieved */
		strcpy(ptr->phone_nbr,srch_calls[0].phone_nbr);			/* last phone number used */
		call_number = srch_calls[0].nbr;				/* call number */

		*got_calls = 0;
		db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ, 0);    		/* get 1st record again */
		if(cl_key_on != 1 && ptr->nbr != call_number){
			while(db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0) >= 0){
				if(ptr->nbr == call_number)
					break;
				else{
					if(ptr->due_date_time != srch_calls[0].due_date_time)
						break;	
				}
			}
		}

		ccode=db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0);
		while ( *got_calls == 0) {	 				/* get more records */
			if(ccode < 0) {						/* no more for this fleet ? */
				if (cl_key_on != 1) {				/* not a call number search */
					matches_index--;				/* done with fleet, prev fleet */
					backward_start_in_srch(got_calls,ptr);		/* found a record */
				}
				return(*got_calls); 
			}

			if ((*fnptr_matches)(ptr, select_criteria.fleet[matches_index])) {
				memcpy(&srch_calls[0],req_buf, sizeof(struct cisam_cl));
				*got_calls = 1;
				return(*got_calls);
			}
			else {
				if (cl_key_on == 1) {						/* search by call # ? */
					ccode=db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0);
					continue;
				}
				if((cl_key_on == 5 && (strncmp(ptr->pckup_str_key,select_criteria.streetname,MIN(strlen(select_criteria.streetname),4))) != 0) ||
				   (cl_key_on == 6 && (strncmp(ptr->pass_name_key,select_criteria.passengername,MIN(strlen(select_criteria.passengername),4))) != 0) ||
				   (ptr->fleet != select_criteria.fleet[matches_index])){
					matches_index--;				/* done with fleet, prev fleet */
					backward_start_in_srch(got_calls,ptr);		/* found a record to continue from */
					return(*got_calls); 
				}
				else {
					ptr->due_date_time = 0;
					ccode=db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ,0);
					ccode=db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0);
				}
			}
		}
	}  								/* end if  srch_type == BACKWARDS */
	return(*got_calls); 
}


forward_start_in_srch(got_calls,ptr) 				/* find first good record for a fleet */
int *got_calls;
struct cisam_cl *ptr;
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
		ptr->due_date_time = srch_dt_tm;				/* date/time entered by user */
		ptr->nbr = atoi(select_criteria.callnumber);
		strncpy(ptr->pckup_str_key,select_criteria.streetname,4);
		strncpy(ptr->pass_name_key,select_criteria.passengername,4);
		strcpy(ptr->charge_nbr,select_criteria.chargenumber);
		strcpy(ptr->phone_nbr,select_criteria.phonenumber);

		if(db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ,0) >= 0){				/* have records to read */
			if ((*fnptr_matches)(ptr, select_criteria.fleet[matches_index])) {		/* check criteria */
				memcpy(&srch_calls[0],req_buf,sizeof(struct cisam_cl)); 
				*got_calls = 1;
				return(DONE);
			}
			else {
				if((cl_key_on == 5 && (strncmp(ptr->pckup_str_key,select_criteria.streetname,MIN(strlen(select_criteria.streetname),4))) != 0) ||
				   (cl_key_on == 6 && (strncmp(ptr->pass_name_key,select_criteria.passengername,MIN(strlen(select_criteria.passengername),4))) != 0) ||
				   (cl_key_on != 1 && ptr->fleet != select_criteria.fleet[matches_index]))
					continue;
			}
		}
		ccode = db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0);				/* have records to read */
		while (ccode >= 0){									/* have records to read */
			if ((*fnptr_matches)(ptr, select_criteria.fleet[matches_index])) {		/* check criteria */
				memcpy(&srch_calls[0],req_buf,sizeof(struct cisam_cl)); 
				*got_calls = 1;
				return(DONE);
			}
			else{
				if (cl_key_on == 1) {						/* search by call # ? */
					ccode = db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0);
					continue;
				}
				if((cl_key_on == 5 && (strncmp(ptr->pckup_str_key,select_criteria.streetname,MIN(strlen(select_criteria.streetname),4))) != 0) ||
				   (cl_key_on == 6 && (strncmp(ptr->pass_name_key,select_criteria.passengername,MIN(strlen(select_criteria.passengername),4))) != 0) ||
				   (ptr->fleet != select_criteria.fleet[matches_index]))
					break;							/* done with all for fleet*/
				else {
					ptr->due_date_time = srch_dt_tm;			/* continue from bottom */
					ccode = db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ,0);
				}
			}
		} 
	}
	return (NOT_DONE);
}


backward_start_in_srch(got_calls,ptr) 			/* backward search through to the initial of the first fleet */
int *got_calls;
struct cisam_cl *ptr;
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
		ptr->due_date_time = MAX_INT;			 			/* due date/time entered by user */
		ptr->nbr = atoi(select_criteria.callnumber);
		strncpy(ptr->pckup_str_key,select_criteria.streetname,4);
		if (cl_key_on == 5) {							/* street name key */
			key_length = strlen(select_criteria.streetname);		/* how full is the key ? */
			while (key_length < 4){						/* while a partial key */
				ptr->pckup_str_key[key_length] = LAST_UPCASE_LETTER_OF_ALPHABET;	/* high order fill */
				key_length++;						/* next position */
			}
		}
		strncpy(ptr->pass_name_key,select_criteria.passengername,4);
		if (cl_key_on == 6) {							/* passenger name key */
			key_length = strlen(select_criteria.streetname);		/* how full is the key ? */
			while (key_length < 4){						/* while a partial key */
				ptr->pass_name_key[key_length] = LAST_UPCASE_LETTER_OF_ALPHABET;	/* high order fill */
				key_length++;						/* next position */
			}
		}
		strcpy(ptr->charge_nbr,select_criteria.chargenumber);
		strcpy(ptr->phone_nbr,select_criteria.phonenumber);

		if (db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ,0) < 0)			/* have no records to read */
			db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISLAST,0);

		ccode=db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV);				/* have records to read */
		while (ccode >= 0){
			if ((*fnptr_matches)(ptr, select_criteria.fleet[matches_index])) {		/* check criteria */
				memcpy(&srch_calls[0],req_buf,sizeof(struct cisam_cl)); 
				*got_calls = 1;
				return(DONE);
			}
			else{
				if (cl_key_on == 1) {						/* search by call # ? */
					ccode = db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0);
					continue;
				}
				if((cl_key_on == 5 && (strncmp(ptr->pckup_str_key,select_criteria.streetname,MIN(strlen(select_criteria.streetname),4))) != 0) ||
				   (cl_key_on == 6 && (strncmp(ptr->pass_name_key,select_criteria.passengername,MIN(strlen(select_criteria.passengername),4))) != 0) ||
				   (ptr->fleet != select_criteria.fleet[matches_index])){
					break;
				}
				else {
					ptr->due_date_time = 0;					/* date/time in by user */
					ccode=db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ);
					ccode = db(CALL_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0);
				}
			}
		} 
	}
	return (NOT_DONE);
}

