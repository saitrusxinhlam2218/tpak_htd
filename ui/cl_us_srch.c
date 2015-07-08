static char sz__FILE__[]="@(#)cl_us_srch.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  cl_us_srch.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:47:26
 * @(#)  Last delta: 12/2/94 at 18:02:04
 * @(#)  SCCS File: /taxi/sccs/s.cl_us_srch.c
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

extern int page_num;					/* for the search page number */
extern short clear_err_flag;
extern char req_buf[];					/* buffer for r/w records to/from disk */
extern struct keydesc *ptr_srch_key; 			/* the pointer to the search key */
extern struct cisam_up srch_up_calls[];	 		/* the array of records that holds 17 calls */
char vh_calls_fleet[SEARCHES_ON_PG];

/*******************************************************************************/
/* up_search: gets the next or the previous 17 (probably less) vehicle records */
/*	 from the disk (only those records that meet the selection criteria    */
/*	 are selected).		 Returns the number of the records selected.   */
/*******************************************************************************/
call_up_search(have_start, got_calls, srch_dir, init_page)
char have_start;					/* 'y' or 'n'; search start from current position  or not */
int *got_calls;						/* pointer to # of calls read */
int srch_dir;						/* search direction, FORWARD or BACKWORD */
char init_page;						/* 'y' or 'n'; search start for first page */
{
    struct cisam_up *ptr;				/* pointer to be assigned to req_buf[] */
    short i=0;
    
    ptr = (struct cisam_up *)req_buf;
    
    if (init_page == YES){								/* 1st page of a search request */
	if(forward_start_up_srch(got_calls,ptr))				/* get 1st valid record */
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
	    while (srch_up_calls[i-1].user_id > 0 && i <= SEARCHES_ON_PG) {	/* find starting point */
		i++;
	    }
	    if(i > 0){							/* found the last call */
		ptr->user_id = srch_up_calls[i-2].user_id;	 	/* last user retrieved */
	    }
	    
	    if(db(USER_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ, 0) < 0) 	/* get last record again */
		return(*got_calls);					/* no more records in call file */
	    
	    *got_calls = 0;
	}
	
	
	while ( *got_calls < SEARCHES_ON_PG ){						/* more records to find */
	    if(db(USER_FILE_ID,READ_KEYS,ptr_srch_key,ISNEXT,0) < 0){		/* more records in file ? */
		
		if (*got_calls) {
		    for(i= *got_calls; i<SEARCHES_ON_PG; i++)
			srch_up_calls[i].user_id = 0;
		    fwd_page_handler();
		}
		
		return(*got_calls); 
	    }
	    memcpy(&srch_up_calls[*got_calls],req_buf,sizeof(struct cisam_up));
	    (*got_calls)++;
	}
	if (*got_calls) {
	    for(i= *got_calls; i<SEARCHES_ON_PG; i++)
		srch_up_calls[i].user_id = 0;
	    fwd_page_handler();
	}
	return (*got_calls);
    }
    
    if ( srch_dir == BACKWARD )  {
	ptr->user_id = srch_up_calls[0].user_id; 
	
	*got_calls = 0;
	if (db(USER_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ, 0) < 0)		/* get 1st record again */
	    return(*got_calls);						/* no more records in file */
	
	while ( *got_calls < SEARCHES_ON_PG ) { 				/* get more records */
	    if(db(USER_FILE_ID,READ_KEYS,ptr_srch_key,ISPREV,0) < 0){		/* no more records */
		
		if (*got_calls)
		    bkd_page_handler();
		
		if (*got_calls != SEARCHES_ON_PG && *got_calls > 0){	/* partial page of records ? */
		    for (i=0; i <= *got_calls; i++)			/* copy up the records */
			memcpy(&srch_up_calls[i], &srch_up_calls[SEARCHES_ON_PG - (*got_calls) + i],
			       sizeof(struct cisam_up));
		    for(i= *got_calls; i<SEARCHES_ON_PG; i++)
			srch_up_calls[i].user_id = 0;
		}
		return(*got_calls); 
		
	    }
	    memcpy(&srch_up_calls[SEARCHES_ON_PG - (*got_calls) - 1],req_buf,sizeof(struct cisam_up));
	    (*got_calls)++;
	}
	
	if (*got_calls)
	    bkd_page_handler();
    }  								/* end if  srch_type == BACKWARDS */
    return(*got_calls); 
}


forward_start_up_srch(got_calls,ptr) 				/* find first good record for a fleet */
int *got_calls;
struct cisam_up *ptr;
{
	short i=0;

	ptr->user_id = atoi(select_criteria.userid);				/* user id to start from */
        strcpy(ptr->fullname, select_criteria.passengername);
	if(db(USER_FILE_ID,READ_KEYS,ptr_srch_key,ISGTEQ,0) >= 0){				/* have records to read */
		memcpy(&srch_up_calls[*got_calls],req_buf,sizeof(struct cisam_up));
		(*got_calls)++;
		return(DONE);
	}
	else {
		return(NOT_DONE);
	}
}
