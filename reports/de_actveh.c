static char sz__FILE__[]="@(#)de_actveh.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  de_actveh.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:21:33
 * @(#)  Last delta: 12/14/89 at 17:01:32
 * @(#)  SCCS File: /taxi/sccs/s.de_actveh.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <isam.h>
#include <sys/types.h>
#include "mads_types.h"
#include "key.h"
#include "srch_const.h"
#define DECLARING
#include "rep_strdefs.h"

extern	char buf[];				/* for db put and get records       */
extern	char req_buf[];				/* buffer to cast as call structure */
extern	int db();
extern  void local_date_time();

struct	cisam_cl *callrec;
struct	cisam_vh *vehrec;
struct	cisam_vh saved_vh;			/* saved vehicle structure */

long seconds;					/* number of seconds to back from current time */
int pagenumber=1;				/* printed page counter */
int printedlines;				/* global printed line counter */
int cabs[800];

#define endofpage 52				/* check this end of page to group one record on a page */

/************************************************************************/
/* main()								*/
/*	Print messages for the particular fleet.			*/
/*	Is quite specific to this report.				*/
/************************************************************************/
main(argc,argv)
int argc;
char *argv[];	
{
	char fleet_id=argv[1][0];
	char *start_date=argv[2];
	char *start_time=argv[3];
	long start_dt_tm=atol(argv[4]);				/* the initial start report time */
	char *end_date=argv[5];
	char *end_time=argv[6];
	long ending_dt_tm=atol(argv[7]);			/* the initial start report time */
	char *fleet_name=argv[8];
 
	int ccode;						/* returned from proc calls */
	short saved_veh;
	short numrows;
	int numcabs, k, i = 0;

	int total_active = 0;
	int total_inactive = 0;
	float dispd_times = 0;					/* summation of all dispd_date_time - date_time_in */
	int dispd_calls = 0;					/* how many calls were dispatched */
	float pckup_times = 0;					/* summation of all pckup_date_time - date_time_in */
	int pckup_calls = 0;					/* how many calls were picked up */
	float close_times = 0;					/* summation of all close_date_time - pckup_date_time */
	int close_calls = 0;					/* how many calls were closed */
	int total;

	memset(cabs,0,sizeof(cabs));
	callrec=(struct cisam_cl*)req_buf;      /* to cast req_buf */
	vehrec=(struct cisam_vh*)req_buf;			/* to cast req_buf */
	seconds = time((long *)0);				/* current date and time in seconds */

        /* initialize language environment */
        init_locale();
        REP_catd = init_catalog("de_actveh");

	if(db(VEHICLE_FILE_ID,OPEN,&veh_key1,ISINPUT | ISMANULOCK,0) < 0) {			/* open CALL db for read only */
		printf(catgets(REP_catd, 1, 1, "Error OPENing VEHICLE file %d iserrno %d\n"),VEHICLE_FILE_ID,iserrno);
		exit(-1);
	}

	if(db(CALL_FILE_ID,OPEN,&cl_key4,ISINPUT | ISMANULOCK,0) < 0) {			/* open CALL db for read only */
		printf(catgets(REP_catd, 1, 2, "Error OPENing CALL file %d iserrno %d\n"),CALL_FILE_ID,iserrno);
		exit(-1);
	}

	printedlines = print_heading(fleet_id,start_date,start_time,end_date,end_time,fleet_name);

	vehrec->fleet = fleet_id;
	vehrec->nbr = 0;
	ccode=db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISGTEQ,0);

	while (ccode >= 0 && vehrec->fleet == fleet_id) {		/* continue while matching fleet */
		saved_veh = vehrec->nbr;
		memcpy(&saved_vh,req_buf,sizeof(struct cisam_vh));	/* save vehicle variables */

		if (printedlines > endofpage) 
			printedlines = print_heading(fleet_id,start_date,start_time,end_date,end_time,fleet_name);

		callrec->fleet = fleet_id;					/* fleet id */
		callrec->pri_status = 64;					/* assigned calls only */
		callrec->veh_nbr = saved_veh;					/* vehicle number */
		callrec->due_date_time = start_dt_tm;				/* date/time entered by user */

		if(db(CALL_FILE_ID,READ_KEYS,&cl_key4,ISGTEQ,0) >= 0){		/* found a record */
			if (callrec->veh_nbr == saved_veh && callrec->dispd_date_time >= start_dt_tm &&
			    callrec->dispd_date_time <= ending_dt_tm) {
				cabs[i++] = saved_veh;
				total_active += 1;
				if (callrec->dispd_date_time > 0 && callrec->dispatch_entry_tm > 0) {
					dispd_times += (callrec->dispd_date_time - callrec->dispatch_entry_tm);
					dispd_calls += 1;
				}
				if (callrec->pckup_date_time > 0) {
					pckup_times += (callrec->pckup_date_time - callrec->dispatch_entry_tm);
					pckup_calls += 1;
				}
				if (callrec->pckup_date_time > 0 && callrec->close_date_time > 0) {
					close_times += (callrec->close_date_time - callrec->pckup_date_time);
					close_calls += 1;
				}
			}
			else {
				cabs[i++] = 0;
				total_inactive += 1;
			}
		}
		else {
			cabs[i++] = 0;
			total_inactive += 1;
		}

		memcpy(req_buf,&saved_vh,sizeof(struct cisam_vh));		/* restore vehicle variables */

		ccode = db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISNEXT,0);
	}

	/* now print all records */
	
	numcabs = i;
	numrows = (numcabs / 22) + (numcabs % 22 ? 1 : 0);
/*	numcabs += numrows;*/
	total = numrows * 22;
	k=0;
	for(i = 0; i < total; i++) {
		if(cabs[k + (numrows * (i % 22))])
			printf("%4d  ", cabs[k + (numrows * (i % 22))]);
		else
			if((k + (numrows * (i%22))) < numcabs)
				printf("      ");

		if(((i + 1) % 22) == 0) {
			putchar('\n');
			k++;
		}
	}

	putchar('\n');
	
	puts("------------------------------------------------------------------------------------------------------------------------------------");
	printf(catgets(REP_catd, 1, 3, "Total Active Cabs..............%d\n"),total_active);
	printf(catgets(REP_catd, 1, 4, "Total Inactive Cabs............%d\n"),total_inactive);
	if (dispd_calls > 0)
		printf(catgets(REP_catd, 1, 5, "Last Call Avg. Dispatch Time...%5.2f Minutes\n"), (dispd_times/dispd_calls)/60.0);
	else
		printf(catgets(REP_catd, 1, 6, "Last Call Avg. Dispatch Time...No Calls Dispatched During the Period\n"));
	if (pckup_calls > 0)
		printf(catgets(REP_catd, 1, 7, "Last Call Avg. Pickup Time.....%5.2f Minutes\n"), (pckup_times/pckup_calls)/60.0);
	else
		printf(catgets(REP_catd, 1, 8, "Last Call Avg. Pickup Time.....No Calls Picked Up During the Period\n"));
	if (close_calls > 0)
		printf(catgets(REP_catd, 1, 9, "Last Call Avg. Fare Duration...%5.2f Minutes\n"), (close_times/close_calls)/60.0);
	else
		printf(catgets(REP_catd, 1, 10, "Last Call Avg. Fare Duration...No Calls Completed During the Period\n"));

	fflush(stdout);

        /* close language catalogues */
        cleanup_catalog(REP_catd);

}


/************************************************************************/
/* print_heading() -							*/
/*	Prints the field definitions for this report.			*/
/************************************************************************/
print_heading(fleet_id,start_date,start_time,end_date,end_time,fleet_name)
char fleet_id;
char *start_date;
char *start_time;
char *end_date;
char *end_time;
char *fleet_name;
{
	char title2[60];
	sprintf(title2,catgets(REP_catd, 1, 11, "          ACTIVE VEHICLES by vehicle number"));

	/* putchar('\f'); */

	report_heading(seconds,start_date,start_time,end_date,end_time,fleet_name,title2,&pagenumber);

	puts("====================================================================================================================================");


	puts(catgets(REP_catd, 1, 12, "VEH#  VEH#  VEH#  VEH#  VEH#  VEH#  VEH#  VEH#  VEH#  VEH#  VEH#  VEH#  VEH#  VEH#  VEH#  VEH#  VEH#  VEH#  VEH#  VEH#  VEH#  VEH#"));

	puts("====================================================================================================================================");

	putchar('\n');
	return(0);
}

/*=====================================================================
= report_heading() -
=	Prints Report Type Heading
=====================================================================*/
report_heading(sys_dt_tm,start_date,start_time,end_date,end_time,fleetname,title2,pagenumber)
long sys_dt_tm;
char *start_date;
char *start_time;
char *end_date;
char *end_time;
char *fleetname;
char *title2;
int  *pagenumber;
{
   char sys_date[20];             /* current date, locally formatted */
   char sys_time[20];             /* current time, locally formatted */
   char dummy[40];        

   /* get current date/time & localize */
   local_date_time(sys_dt_tm, sys_date, sys_time, dummy);

	printf(catgets(REP_catd, 1, 13, "Report for Date: %8s to %8s                  %20s                  Page: %3d   Date Printed: %s\n"),
		start_date,end_date,fleetname,*pagenumber,sys_date);
	(*pagenumber)++;	/* increment the pagenumber */

	printf(catgets(REP_catd, 1, 14, "Report Coverage: %8s to %8s      %-55s       Time Printed: %s\n\n"),
		start_time,end_time,title2,sys_time);

	fflush(stdout);
	return(0);
}	/* end of report_heading */
