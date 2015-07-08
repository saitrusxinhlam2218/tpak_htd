static char sz__FILE__[]="@(#)da_shift.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  da_shift.c; Rel: 5.2.0.0; Get date: 2/27/90 at 12:20:56
 * @(#)  Last delta: 1/16/90 at 15:23:35
 * @(#)  SCCS File: /taxi/sccs/s.da_shift.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <isam.h>
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "ex_key.h"
#include "mad_error.h"
#include "path.h"
#include "rep_strdefs.h"

extern	char buf[];	/* for db put and get records       */
extern	char req_buf[];	/* buffer to cast as call structure */
extern	int db();

#define blanks "                    "

struct	cisam_cl *callrec;
struct	cisam_ch *histrec;

char sysbuff[80];

/* the shift and shifth table names are coded in-line.
   This should be no problem as the names are used only once,
   and this method removes errors of define statements. */

/*====================================================================
= da_shift()
=	Each day, the previous shift calls and call history records
=	should be moved to a file, called SHIFT.
=	da_shift returns 0 if all proceeded ok
=			-1 otherwise
=	What:   Copy .dat and .idx files at last option for fault tolerance
=		Echo program status to the screen.
=		Currently this proceedure does NOT use table redundancy proceedures.
=====================================================================*/
da_shift(fleet_id,start_date,start_time,start_dt_tm,end_date,end_time,ending_dt_tm,fleet_name, option)
char fleet_id[];
char *start_date;
char *start_time;
long start_dt_tm;
char *end_date;
char *end_time;
long ending_dt_tm;
char *fleet_name[];
int option;		/* option to print to screen  1 prints messages to screen */
{
	char buffer[132];
	register char current_fleet;
	register int ccode;		/* returned from proc calls */
	register int indx;
	register int call_number;
	register long save_dt_tm;
	int found_calls=0;	/* 1 if calls to process else 0 */
	int records=0;
	int fleets=strlen(fleet_id);
	int start_fleet;
	int open_f1=0, open_f2=0, open_f3=0, open_f4=0;

	struct cisam_cl call_struct;

	/* cast the buffers as the request buffer */
	callrec=(struct cisam_cl*)req_buf;
	histrec=(struct cisam_ch*)req_buf;

	/* open the call table that consists of the previous shift calls and any current calls */
	/*  open the cisam CALL file, using the composite key3: fleet, due_date_time */
	if (db(CALL_FILE_ID,OPEN,&cl_key3,ISINPUT | ISMANULOCK,0)<0) {
		ERROR(' ', CALL_FILE, "Error OPENing call file")
		exit(-1);
	}
	else open_f1++;

	/* copy empty shift and shift history tables to the system directory */
	sprintf(buffer,"/bin/cp %s %s",SHIFT_DAT,MADS_DBS_DIR); system(buffer);
	sprintf(buffer,"/bin/cp %s %s",SHIFT_IDX,MADS_DBS_DIR); system(buffer);
	sprintf(buffer,"/bin/cp %s %s",SHIFTH_DAT,MADS_DBS_DIR); system(buffer);
	sprintf(buffer,"/bin/cp %s %s",SHIFTH_IDX,MADS_DBS_DIR); system(buffer);

	/* open the call history, shift, and shift history tables once */
	/*  open the cisam CALL HISTORY file using key1 = call_nbr, exception_nbr */
	if (db(CALLH_FILE_ID,OPEN,&ch_key1,ISINPUT | ISMANULOCK,0)<0) {
		ERROR(' ', CALLH_FILE, "Error OPENing call history file")
		exit(-1);
	}
	else open_f2++;

	/* open two new tables: SHIFT and SHIFT HISTORY */
	/* open the cisam SHIFT file, assume it is empty. */
	/* The key does not matter because records are only added to the shift table */
	if (db(SHIFT_FILE_ID,OPEN,&sh_key1,ISOUTPUT | ISMANULOCK,0)<0) {
		ERROR(' ', SHIFT_FILE, "Error OPENing shift file")
		exit(-1);
	}
	else open_f3++;

	/*  open the cisam SHIFT HISTORY file, assume it is empty. */
	/* The key does not matter because records are only added to the shift history table */
	if (db(SHIFTH_FILE_ID,OPEN,&hh_key1,ISOUTPUT | ISMANULOCK,0)<0) {
		ERROR(' ', SHIFTH_FILE, "Error OPENing shift history")
		exit(-1);
	}
	else open_f4++;

	/* 
		Write calls from the previous shift to the call table.
		For each call, write all corresponding call history reccords to the shift history table.
		This proceedure locks only the new shift tables.
		Print status messages to the screen, because this proceedure runs slowly.
	*/
	for (indx=0;indx<fleets;indx++) {
		current_fleet=fleet_id[indx];	/* is easy to manage and is fail-safe */
		if(option == 1)
			printf(catgets(REP_catd, 9, 5, "%sSaving records for fleet %c \n"),blanks,current_fleet); /* print status on the screen */

		/* ISGTEQ points to the first record matching fleet with time now or later */
		callrec->fleet = current_fleet;
		callrec->due_date_time = start_dt_tm;
		ccode=db(CALL_FILE_ID,READ_KEYS,&cl_key3,ISGTEQ,0);	/* key3 = fleet, due_date_time */
		while ((ccode>=0) && (callrec->fleet==current_fleet) && (callrec->due_date_time<ending_dt_tm)) { 
			/* save the current due_date_time, since the req_buf will be overwritten */
			save_dt_tm=callrec->due_date_time;
			call_number=callrec->nbr;

			/* add this record to the shift table */
			if(db(SHIFT_FILE_ID,ADD_REC,&sh_key1,0,0)<0) 
				ERROR(' ', SHIFT_FILE, "shift: error writing")
			else {
				records++;

				/* define the call history fields */
				histrec->call_nbr = call_number;
				histrec->exception_nbr = 0;
				ccode=db(CALLH_FILE_ID,READ_KEYS,&ch_key1,ISGTEQ,0);	/* key1 = call_nbr, exception_nbr */

				while ((ccode>=0) && (histrec->call_nbr==call_number)) { /* while matching call numbers */
					/* continue while matching call_number */
					if (db(SHIFTH_FILE_ID,ADD_REC,&hh_key1,0,0)<0) 
						ERROR(' ', SHIFTH_FILE, "shift_history: error writing")
					ccode=db(CALLH_FILE_ID,READ_KEYS,&ch_key1,ISNEXT,0);
				}
			}

			/* restore the call record key fields to read the next record */
			callrec->fleet=current_fleet;
			callrec->due_date_time=save_dt_tm;
			ccode=db(CALL_FILE_ID,READ_KEYS,&cl_key3,ISNEXT,0);	/* key3 = fleet, due_date_time */
		}
	}

	/*  The previous shift calls were written to the SHIFT file. Now, close that file. */
	if(open_f1) 
		if(db(SHIFT_FILE_ID,CLOSE,&sh_key1,0,0)<0) 
			ERROR(' ', SHIFT_FILE, "Error CLOSEing shift file")

	/*  The previous shift calls were written to the SHIFT file. Now, close that file. */
	if(open_f2) 
		if(db(SHIFTH_FILE_ID,CLOSE,&hh_key1,0,0)<0) 
			ERROR(' ', SHIFTH_FILE, "Error CLOSEing shift history file")

	/* it remains to purge the calls from the CALL and the CALL HISTORY tables */
	if(open_f3) 
		if(db(CALL_FILE_ID,CLOSE,&cl_key1,0,0)<0) 
			ERROR(' ',CALL_FILE, "Error CLOSEing call file")

	if(open_f4) 
		if(db(CALLH_FILE_ID,CLOSE,&ch_key1,0,0)<0)
			ERROR(' ', CALLH_FILE, "Error CLOSEing call history")

	if(option == 1)
		printf(catgets(REP_catd, 9, 12, "%sSTATUS: Done copying call records.\n"),blanks);
	return(0);
}
