static char sz__FILE__[]="@(#)au_message.c	10.1.1.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  au_message.c; Rel: 10.1.1.1; Get date: 6/2/95 at 12:46:26
* @(#)  Last delta: 12/2/94 at 16:17:42
* @(#)  SCCS File: /taxi/sccs/s.au_message.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <isam.h>
#include <time.h>
#include <sys/types.h>

#include "mads_types.h"
#include "Fleet_db.h"
#include "taxi_db.h"
#include "key.h"		/* using keydesc cl_key1 for call number */
#include "ui_def.h"
#include "path.h"
#define DECLARING
#include "rep_strdefs.h"
#include "language.h"

extern	char buf[];	/* return buffer for call record */
extern	char req_buf[];	/* buffer to cast as call structure */
extern	int db();
extern  void local_date_time();

#define blanks "                    "

char sysbuf[120];                     

char get_fleet();
long  start_dt_tm;
/*====================================================================
= main parameter 
=	If parameter < 0, then start all fleet reporting processes
=			  but print no reports.
=	If parameter is in 0 .. 7, then print the particular fleet repoprt.
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=====================================================================*/
main(argc,argv)
int argc;	/* 1 then stop all message cron atjobs and exit, else restart cron atjobs */
char *argv[];	/* argv[0] = fleet number 0 .. 7 */
		/* argv[0] < 0, then start all fleet processes */
{
	int fleetnumber;
	char fleet_id;	/* the active fleet id */
	char fleet_name[50];	/* the active fleet name */

        /* initialize language environment */
        init_locale();
        /*REP_catd = init_catalog("au_message"); - presently, no useful msgs*/

  	setuid(0);

	/* remove all au_live at jobs and exit */
	if (argc <= 1) { 
           sprintf(sysbuf,"at -r `grep -l au_message %s 2>/dev/null | sed 's/.*\\///' ` 2>/dev/null", ATJOBS);
           system(sysbuf);
	   exit(0);
	}

	if (argc>1)
           fleetnumber=atoi(argv[1]);
	if (argc>2) start_dt_tm=atol(argv[2]);

	/* print all active fleet identifiers */
	if (fleet_id=get_fleet(fleetnumber,fleet_name)) print_fleet(fleet_id,fleet_name); 

        /* close language catalogues */
        /*cleanup_catalog(REP_catd);*/

}	/* end main */

/*====================================================================
= get_fleet() -
=	If fleetnumber <0 starts all fleet message processes and returns NULL;
=	Else restarts itself, return a single fleet id and name.
=====================================================================*/
char get_fleet(fleetnumber,fleet_name)
/* get_fleet returns the char fleet_id */
int fleetnumber;	/* one cell for each active fleet, 50 should be very large */
char fleet_name[];	/* one pointer for each active fleet name */
{
	int ccode;
	char returncode;
	struct	cisam_fl *fleetrec;
	fleetrec=(struct cisam_fl*)req_buf;	/* to cast req_buf */

	/* initialize to read the cisam FLEET file */
	ccode=db(FLEET_FILE_ID,OPEN,&fl_key2,ISINPUT | ISMANULOCK,0);	/* open CALL db for read only */
	if (ccode<0) {
		printf(catgets(REP_catd, 1, 1, "Error OPENing file %s iserrno %d\n"),FLEET_FILE_ID,iserrno);
		exit(-1);
	}	/* end if ccode */

	/* define the key field, for all fleets start from 0 */
	fleetrec->nbr = fleetnumber;
	if (fleetnumber<0) fleetrec->nbr = 0;
	returncode=NULL;

#ifdef DEBUG
	printf("fleetrec->nbr is %d fleetnumber is %d\n",
	fleetrec->nbr,fleetnumber);
#endif

	/* read the matching record */
	ccode=db(FLEET_FILE_ID,READ_KEYS,&fl_key2,ISEQUAL,0);
	while (ccode >=0) {

#ifdef DEBUG
	printf("fleet->active is %c fleetrec->id is %c\n",
	fleetrec->active,fleetrec->id);
	printf("fleetrec->name is %s fleetrec->time_calls_freq %d\n",
	fleetrec->name,fleetrec->time_calls_freq);
#endif

		if (Toupper(fleetrec->active) == YES) {
			returncode=fleetrec->id; /* save the fleet identifier */
			strcpy(fleet_name,fleetrec->name); /* save the fleet name */
			/* reschedule this fleet process */
			next_report(fleetrec->nbr,fleetrec->msg_rpt_freq);
		}	/* end if fleetrec */
	
		fleetrec->nbr++;		/* next fleet */
		ccode = -1;
		/* if dispatching all fleet procedures, get the next matching record */
		if (fleetnumber<0) ccode=db(FLEET_FILE_ID,READ_KEYS,&fl_key2,ISEQUAL,0);
	}	/* end while */	

	/* close the FLEET file */
	ccode=db(FLEET_FILE_ID,CLOSE,&fl_key2,0,0);
	if (ccode<0) {
		printf(catgets(REP_catd, 1, 2, "Error CLOSEing file %d iserrno %d\n"),FLEET_FILE_ID,iserrno);
		printf("fleetrec->id %c \n",fleetrec->id);
		exit(-1);
	}	/* end if ccode */

	/* the default return code is NULL fleet identifier */
	if (fleetnumber<0) return(NULL);	/* do not print a report */
	return(returncode);

}	/* end get_fleet */

/*====================================================================
= next_report() -
=	Schedules a report in nexthours, using at.
=====================================================================*/
next_report(thisfleet,nexthours)
int thisfleet;
int nexthours;
{
	char systembuff[120];
	char *getenv();
	char *mads_spooler;
        char mbuf[20];
        long current_time;
        struct tm *current_tm;
        int  athr;
        int  atmin;

        mads_spooler = mbuf;
	if ((mads_spooler = getenv("MADS_SPOOLER")) == NULL) {
		mads_spooler = PRINT_CMD;
	}

	/* first remove any previous audit message at job for this fleet */
        sprintf(systembuff,"at -r `grep -l \"au_message %d\" %s 2>/dev/null | sed 's/.*\\///' ` 2>/dev/null", thisfleet, ATJOBS);
        system(systembuff);

        current_time = time((long *)0);
        current_tm = localtime(&current_time);
        athr = current_tm->tm_hour+nexthours;
        if (athr >= 24) athr = athr - 24;        /* reset hours after passing midnight */
        atmin = current_tm->tm_min;
	sprintf(systembuff,"echo \"%s %d %ld|%s\"|at %2d%02d 2>/dev/null", AU_MESSAGE,thisfleet,current_time,mads_spooler,athr,atmin);
#ifdef DEBUG 
printf("next_report: systembuff %s\n",systembuff);
#endif
	system(systembuff);
}	/* end of next_report */

/*====================================================================
= print_fleet() -
=	Print messages for the particular fleet.
=====================================================================*/
int print_fleet(fleet_id,fleet_name)
char fleet_id;
char fleet_name[];
{
	char buffer[264];
	char start_date[20];
        char start_time[20];
        char start_date_time[40];

        local_date_time(start_dt_tm, start_date, start_time, start_date_time);

	sprintf(buffer,"sacego -q %s %c %s %s %ld \"%s\" ",
              MESSAGES, fleet_id, start_date, start_time, start_dt_tm, fleet_name);

#ifdef DEBUG
printf("print_fleet: %s\n",buffer);
#endif
	system(buffer);
	return(0);
}	/* end of print_fleet */
