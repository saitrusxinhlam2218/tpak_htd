static char sz__FILE__[]="@(#)de_manifst.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  de_manifst.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:21:43
 * @(#)  Last delta: 12/14/89 at 17:01:44
 * @(#)  SCCS File: /taxi/sccs/s.de_manifst.c
 *                                                                           *
 *****************************************************************************/

/* Make sure location constants are defined. Create a compile error if they
   are not defined or both are defined. */
#ifdef SCANDINAVIA
#ifdef NAMERICA
ERROR: "Both compile time constants SCANDINAVIA and NAMERICA are defined."
#endif
#else
#ifdef NAMERICA
#else
ERROR: "Compile time constant SCANDINAVIA or NAMERICA undefined."
#endif
#endif

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <isam.h>
#include <sys/types.h>
#include <langinfo.h>

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
struct  cisam_cl save_cl;
struct	cisam_dr *drvrec;
struct	cisam_dr saved_dr;			/* saved driver structure */

long seconds;					/* number of seconds to back from current time */
int pagenumber=1;				/* printed page counter */
int printedlines;				/* global printed line counter */
#define blanks					"                    "

#define endofpage 52				/* check this end of page to group one record on a page */
#define linesperpage 53				/* lines per page */

char *currency_symbol;
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
	long driver_id=atol(argv[2]);
	char *start_date=argv[3];
	char *start_time=argv[4];
	long start_dt_tm=atol(argv[5]);				/* the initial start report time */
	char *end_date=argv[6];
	char *end_time=argv[7];
	long ending_dt_tm=atol(argv[8]);			/* the initial start report time */
	char *fleet_name=argv[9];
 
	int ccode;						/* returned from proc calls */
	int total_calls = 0;
	float total_fare = 0;
	float total_rate = 0;
	callrec=(struct cisam_cl*)req_buf;			/* to cast req_buf */
	drvrec=(struct cisam_dr*)req_buf;			/* to cast req_buf */
	seconds = time( (long *) 0);				/* current date and time in seconds */

        /* initialize language environment */
        init_locale();
        REP_catd = init_catalog("de_manifst");

        if ( (currency_symbol = nl_langinfo(CRNCYSTR)) == NULL)
           currency_symbol = " "; 

	if(db(DRIVER_FILE_ID,OPEN,&veh_key1,ISINPUT | ISMANULOCK,0) < 0) {			/* open DRIVER db for read only */
		printf(catgets(REP_catd, 1, 1, "Error OPENing DRIVER file %d iserrno %d\n"),DRIVER_FILE_ID,iserrno);
		exit(-1);
	}

	if(db(CALL_FILE_ID,OPEN,&cl_key4,ISINPUT | ISMANULOCK,0) < 0) {			/* open CALL db for read only */
		printf(catgets(REP_catd, 1, 2, "Error OPENing CALL file %d iserrno %d\n"),CALL_FILE_ID,iserrno);
		exit(-1);
	}


	drvrec->fleet = fleet_id;
	drvrec->id = driver_id;
	ccode=db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISGTEQ,0);

	memcpy(&saved_dr,req_buf,sizeof(struct cisam_dr));	/* save driver variables */

	printedlines = print_heading(fleet_id,start_date,start_time,end_date,end_time,fleet_name);

	callrec->fleet = fleet_id;					/* fleet id */
	callrec->pri_status = 64;					/* assigned calls only */
	callrec->drv_id = driver_id;					/* driver ID */
	callrec->due_date_time = start_dt_tm;				/* date/time entered by user */

	ccode = db(CALL_FILE_ID,READ_KEYS,&cl_key9,ISGTEQ,0);		/* got calls ? */
	if(driver_id > 0 && ccode >= 0) {				/* got calls ? */
		printf("HH %d   %d   %d   %d  %dhhh\n",driver_id,callrec->drv_id,callrec->dispd_date_time,start_dt_tm,ending_dt_tm);

		while (ccode >= 0 && callrec->drv_id == driver_id) {
			if (callrec->drv_id == driver_id && callrec->dispd_date_time >= start_dt_tm &&
			    callrec->dispd_date_time <= ending_dt_tm) {
				if (printedlines > endofpage) 
					printedlines = print_heading(fleet_id,start_date,start_time,end_date,end_time,fleet_name);
				print_record();					/* print the transaction in buf */
				total_calls += 1;
				total_fare += callrec->fare_amount;
				total_rate += callrec->call_rate;
			}
			else
				break;						/* done with this driver */

			ccode = db(CALL_FILE_ID,READ_KEYS,&cl_key9,ISNEXT,0);
		}
	}

	if (total_calls > 0) {
		puts("                                                                                         --------  --------");
		printf(catgets(REP_catd, 1, 3, "TOTALS                                                                                    $%6.2f  $%6.2f\n"),
			total_rate,total_fare);
	}
	printf(catgets(REP_catd, 1, 4, "\nTOTAL CALLS - %d\n"),total_calls);

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
	char title2[50];
	sprintf(title2,catgets(REP_catd, 1, 5, "                     MANIFEST"));

	report_heading(seconds,start_date,start_time,end_date,end_time,fleet_name,title2,&pagenumber);

	printf(catgets(REP_catd, 1, 6, "Driver Name - %s    Driver ID - %d    Last Vehicle - %d\n\n"),saved_dr.name,saved_dr.id,saved_dr.last_veh_nbr);
	/* printf("Last Sign-on Time - %s %s    Last Sign-off Time - %s %s\n\n"); */

	puts("====================================================================================================================================");

	puts(catgets(REP_catd, 1, 7, "                                                     FROM"));

	puts(catgets(REP_catd, 1, 8, "TYPE .........STREET ADDRESS............... CTY ZON  ZONE ..DUE.  DISPD.  PCKUP.  CLOSE. .FLAT RATE..FARE.."));

	puts("====================================================================================================================================");

	putchar('\n');
	return(0);

}

/************************************************************************/
/* print_record() -							*/
/*	Prints the info on an active cab				*/
/************************************************************************/
print_record()
{
	char iobuff[132];					/* line buffer */
	int val;

	memcpy(&save_cl,req_buf,sizeof(struct cisam_cl));	 /*save driver variables */
	val = dofrom_to(callrec->nbr);
	memcpy(req_buf,&save_cl,sizeof(struct cisam_cl));	/* save driver variables */
	if(!val)
		val = callrec->pckup_zone;

#ifdef SCANDINAVIA
	sprintf(iobuff, "%-5s %7d %-2s %-20s %-3s %-2s %-3s %3d  %3d  %-6s  %-6s  %-6s  %-6s  %6.2f%s  %6.2f%s", callrec->extended_type, callrec->pckup_str_nbr, callrec->pckup_pre_dir, callrec->pckup_str_name, callrec->pckup_str_type, callrec->pckup_post_dir, callrec->pckup_city, callrec->pckup_zone,val, callrec->due_time, callrec->dispd_time, callrec->pckup_time, callrec->close_time, callrec->call_rate, currency_symbol, callrec->fare_amount, currency_symbol);
#else
	sprintf(iobuff, "%-5s %7d %-2s %-20s %-3s %-2s %-3s %3d  %3d  %-6s  %-6s  %-6s  %-6s  %s%6.2f  %s%6.2f", callrec->extended_type, callrec->pckup_str_nbr, callrec->pckup_pre_dir, callrec->pckup_str_name, callrec->pckup_str_type, callrec->pckup_post_dir, callrec->pckup_city, callrec->pckup_zone,val, callrec->due_time, callrec->dispd_time, callrec->pckup_time, callrec->close_time, currency_symbol,callrec->call_rate, currency_symbol,callrec->fare_amount);
#endif

	if (printable_line(iobuff)) {
		puts(iobuff);
		printedlines++;
	}
	return(0);
}

/*=====================================================================
= report_heading() -
=	Prints Report Type Heading
=====================================================================*/
report_heading(seconds,start_date,start_time,end_date,end_time,fleetname,title2,pagenumber)
long seconds;
char *start_date;
char *start_time;
char *end_date;
char *end_time;
char *fleetname;
char *title2;
int  *pagenumber;
{
	char current_date[20], current_time[20];

	/* get the date_time structure */

        local_date_time(seconds, current_date, current_time, NULL);

	printf(catgets(REP_catd, 1, 9,
		       "Report for Date: %8s to %8s                  %20s                  Page: %3d   Date Printed: %s\n"),
		start_date,end_date,fleetname,*pagenumber, current_date);
	(*pagenumber)++;	/* increment the pagenumber */

	printf(catgets(REP_catd, 1, 10, "Report Coverage: %8s to %8s      %-55s       Time Printed: %s\n\n"),
		start_time,end_time,title2, current_time);

	fflush(stdout);
	return(0);
}	/* end of report_heading */

dofrom_to(call_num)
int call_num;
{

	struct	cisam_ch *histrec;
	int ccode1;

	histrec=(struct cisam_ch*)req_buf;

	if (db(CALLH_FILE_ID,OPEN,&ch_key1,ISINPUT | ISMANULOCK,0)<0) {
		printf(catgets(REP_catd, 1, 11, "Error OPENing CALLH file %d iserrno %d\n"),CALLH_FILE_ID,iserrno);
		exit(-1);
	}
	histrec->call_nbr = call_num;
	histrec->exception_nbr = 0;
	ccode1=db(CALLH_FILE_ID,READ_KEYS,&ch_key1,ISGTEQ,0);	/* key1 = call_nbr, exception_nbr */
	while(ccode1 >=0 && histrec->call_nbr == call_num)
	{
		if(strncmp(histrec->event_desc,"ACCE",4) == 0)
		{
			return(histrec->user_id);
		}
		ccode1=db(CALLH_FILE_ID,READ_KEYS,&ch_key1,ISNEXT,0);	/* key1 = call_nbr, exception_nbr */
	}
	return(0);
}
