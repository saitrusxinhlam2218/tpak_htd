static char sz__FILE__[]="@(#)au_live.c	10.1.1.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  au_live.c; Rel: 10.1.1.1; Get date: 6/2/95 at 12:46:23
 * @(#)  Last delta: 12/2/94 at 16:17:46
 * @(#)  SCCS File: /taxi/sccs/s.au_live.c
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <isam.h>
#include <sys/types.h>

#include "mads_types.h"
#include "Fleet_db.h"
#include "taxi_db.h"
#include "key.h"	
#include "ui_def.h"
#include "path.h"
#define DECLARING
#include "comm_strdefs.h"
#include "rep_strdefs.h"
#include "language.h"

extern long conversion();
extern	char buf[];				/* return buffer for call record */
extern	char req_buf[];				/* buffer to cast as call structure */
extern	int db();
extern  void local_date_time();

char manage_fleets();
long seconds;					/* current date and time in seconds */
int ccode;
struct	cisam_fl *fleetrec;

char sysbuf[120];                     

#define administrator 4
#define blanks "                    "
/*====================================================================
= main parameter 
=	If parameter < 0, then start all fleet reporting processes
=			  but print no reports.
=	If parameter is in 0 .. 7, then print the particular fleet report.
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=====================================================================*/
main(argc,argv)
int argc;					/* 1 then stop all message cron atjobs and exit, else restart cron atjobs */
char *argv[];					/* argv[0] = fleet number 0 .. 7 */
						/* argv[0] < 0, then start all fleet processes */
{
	int i,option;
	char fleet_id;							/* the active fleet id */
	static char fleet_name[50];					/* the active fleet name */
	fleetrec = (struct cisam_fl*)req_buf;				/* to cast req_buf */

        /* initialize language environment */
        init_locale();
        REP_catd = init_catalog("au_live");
        COMMON_m_catd = init_catalog("common");

  	setuid(0);							/* set user id to ROOT */

	/* remove all au_live at jobs and exit */
	if (argc == 1) {
           sprintf(sysbuf, "at -r `grep -l au_live %s 2>/dev/null | sed 's/.*\\///'` 2>/dev/null ", ATJOBS);
           system(sysbuf);
	   printf(catgets(REP_catd, 1, 1, "Live Time Calls Audit Report Has Been Cancelled, Proceed\n"));
	   fflush(stdout);
	   exit(0);
	}

        option = atoi(argv[1]);
	if (option == -1) {							/* start au_live reports */
		printf(catgets(REP_catd, 1, 2, "Live Time Calls Audit Report Has Been Started, Please Wait While First Set of Reports is Produced\n"));
		fflush(stdout);
		ccode = db(FLEET_FILE_ID,OPEN,&fl_key2,ISINOUT | ISMANULOCK,0);			/* open FLEET db for read only */
		if (ccode < 0) {
			printf(catgets(REP_catd, 1, 3, "Error OPENing file %s iserrno %d\n"),FLEET_FILE_ID,iserrno);
			fflush(stdout);
			exit(-1);
		}	
		for (i = 0; i < 8; i++) {
			fleet_id = manage_fleets(i,fleet_name);
			if (fleet_id != '\0') {
				printf(catgets(REP_catd, 1, 4, "Processing Fleet %c - %s\n"),fleet_id,fleet_name);
				fflush(stdout);
				print_shift(fleet_id,fleet_name);
			}
		}
		ccode = db(FLEET_FILE_ID,CLOSE,&fl_key2,0,0);
		if (ccode < 0) {
			printf(catgets(REP_catd, 1, 5, "Error CLOSEing file %d iserrno %d\n"),FLEET_FILE_ID,iserrno);
			fflush(stdout);
			exit(-1);
		}
		exit(0);
	}
											/* process specific fleet report */
	ccode = db(FLEET_FILE_ID,OPEN,&fl_key2,ISINOUT | ISMANULOCK,0);			/* open FLEET db for read only */
	if (ccode < 0) {
		printf(catgets(REP_catd, 1, 3, "Error OPENing file %s iserrno %d\n"),FLEET_FILE_ID,iserrno);
		fflush(stdout);
		exit(-1);
	}	
	fleet_id = manage_fleets(option,fleet_name); 
	if (fleet_id != '\0')
		print_shift(fleet_id,fleet_name);
	ccode = db(FLEET_FILE_ID,CLOSE,&fl_key2,0,0);
	if (ccode < 0) {
		printf(catgets(REP_catd, 1, 5, "Error CLOSEing file %d iserrno %d\n"),FLEET_FILE_ID,iserrno);
		fflush(stdout);
		exit(-1);
	}

        /* close language catalogues */
        cleanup_catalog(REP_catd);
        cleanup_catalog(COMMON_m_catd);
}	

/************************************************************************************************/
/* manage_fleets() - gets a fleet number to run the report for.					*/
/************************************************************************************************/
char manage_fleets(fleetnumber,fleetname)
int fleetnumber;
char fleetname[];
{
	int i;
	fleetrec = (struct cisam_fl*)req_buf;				/* to cast req_buf */

	fleetrec->nbr = fleetnumber;
	ccode = db(FLEET_FILE_ID,READ_KEYS,&fl_key2,ISEQUAL,0);

#ifdef DEBUG
	printf("fleetrec->nbr %d fleet->active is %c fleetrec->id is %c\n", fleetrec->nbr,fleetrec->active,fleetrec->id);
	printf("fleetrec->name is %s fleetrec->time_calls_freq %d\n", fleetrec->name,fleetrec->time_calls_freq);
	fflush(stdout);
#endif

	if ((Toupper(fleetrec->active) == YES) && fleetrec->time_calls_freq > 0) {
		next_report(fleetrec->nbr,fleetrec->time_calls_freq);			/* reschedule this fleet process */
		strcpy(fleetname,fleetrec->name);
		i = 0;
		while(fleetname[i] != '\0' && i < 50) {						/* insert underscores for SQL */
			if(fleetname[i] == ' ')
				fleetname[i] = '_';
			i++;
		}
		return(fleetrec->id);
	}	

	return(NULL);

}	

/****************************************************************************************/
/* next_report() - Schedules a report in nexthours, using at.				*/
/****************************************************************************************/
next_report(thisfleet,nexthours)
int thisfleet;
int nexthours;
{
	char systembuff[120];
        long current_time;
        struct tm *current_tm;
        int  athr;
        int  atmin;

        /* remove any previous audit live at jobs for this fleet */
        sprintf(systembuff, "at -r `grep -l \"au_live %d\" %s 2>/dev/null | sed 's/.*\\///'` 2>/dev/null", thisfleet, ATJOBS);
        system(systembuff);

        current_time = time((long *)0);
        current_tm = localtime(&current_time);
        athr = current_tm->tm_hour+nexthours;
        if (athr >= 24) athr = athr - 24;   /* reset hours after passing midnight */
        atmin = current_tm->tm_min;
	sprintf(systembuff,"echo \"%s %d\" | at %2d%02d 2>/dev/null",AU_LIVE,
                thisfleet,athr,atmin);

#ifdef DEBUG
	printf("next_report:%s\n",systembuff);
	fflush(stdout);
#endif
 	system(systembuff);
}

/****************************************************************************************/
/* print_shift() - Prints a report for this shift.					*/
/****************************************************************************************/
print_shift(fleet_id,fleet_name)
char fleet_id, fleet_name[];
{
	int sys_day_start,sys_day_end,sys_prod_start_time;			/* variables for the system control fields */
	char sys_gen_rpt1_n[40],sys_print_rpt1_n[40];

	char systembuff[132];
	char start_date[20];
	char start_time[20];
	char end_date[20];
	char end_time[20];
	char dt_tm_buff[13];
	long start_dt_tm;							/* the initial start report time */
	long ending_dt_tm;							/* the ending start report time */
	char *getenv();
	char *mads_spooler;
        char mbuf[20];
        char dummy[40];

        mads_spooler = mbuf;
	if ((mads_spooler = getenv("MADS_SPOOLER")) == NULL) {		/* get the environment variable, MADS_SPOOLER */
		mads_spooler = PRINT_CMD;			/* use default value */
	}
/*
	date_time(start_date,start_time);
	map_dt_tm(start_date,start_time,dt_tm_buff);
	start_dt_tm = conversion(dt_tm_buff);
	ending_dt_tm = start_dt_tm + (365 * 24 * 60 * 60);	
	strcpy(end_time,start_time);
        get_srch_date(ending_dt_tm,end_date);
*/
        /* get current (start) & ending date/time */
        start_dt_tm = time( (long *) 0);
	ending_dt_tm = start_dt_tm + (365 * 24 * 60 * 60);	/* for the next year */

        /* localize date & time */
        local_date_time(start_dt_tm, start_date, start_time, dummy);
        local_date_time(ending_dt_tm, end_date, end_time, dummy);

	sprintf(systembuff,"%s \"%c\" \"%s\" \"%s\" %ld \"%s\" \"%s\" %ld \"%s\"| %s",
		DE_LMAIN,fleet_id,start_date,start_time,start_dt_tm, end_date,end_time,ending_dt_tm,fleet_name,mads_spooler);
	system(systembuff);
}
