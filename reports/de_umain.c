static char sz__FILE__[]="@(#)de_umain.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  de_umain.c; Rel: 10.1.0.0; Get date: 4/18/95 at 17:01:53
 * @(#)  Last delta: 12/2/94 at 11:17:30
 * @(#)  SCCS File: /taxi/sccs/s.de_umain.c
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
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
#include <malloc.h>
#include <sys/types.h>
#include <langinfo.h>

#include "mads_types.h"
#include "key.h"
#include "ex_key.h"
#include "mad_error.h"
#define DECLARING
#include "comm_strdefs.h"
#include "rep_strdefs.h"
#include "language.h"

extern	char buf[];	/* for db put and get records       */
extern	char req_buf[];	/* buffer to cast as call structure */
extern	int db();
extern	char *get_rp_name();
extern  void local_date_time();

#define blanks "                    "
#define linesperpage 53		/* lines per page */

struct	cisam_cl *shiftrec;

long seconds;			/* current date and time in seconds */
int pagenumber;		/* printed page counter */
int printedlines;		/* global printed line counter */

/*====================================================================
= main()
=	Reports all unassigned calls per fleet.
=====================================================================*/

main(argc,argv)
int argc;
char *argv[];
{
	char fleet_id = argv[1][0];
	char *start_date = argv[2];
	char *start_time = argv[3];
	long start_dt_tm = atol(argv[4]);
	char *end_date = argv[5];
	char *end_time = argv[6];
	long ending_dt_tm = atol(argv[7]);
	char *fleet_name = argv[8];

	int ccode;		/* returned from proc calls */
	register int indx;
	int report_no = 1;	/* will print with leading zero */
	int fleets=1;  /*strlen(fleet_id);*/
	int endofpage;

	seconds = time((long *)0);
	shiftrec=(struct cisam_cl *)req_buf;

        /* initialize language environment */
        init_locale();
        REP_catd = init_catalog("de_umain");
        COMMON_m_catd = init_catalog("common");

	if(db(CALL_FILE_ID, OPEN, &cl_key9, ISINPUT | ISMANULOCK, 0) < 0) {
		ERROR(' ', CALL_FILE, "Error opening");
		exit(-1);
	}
	for (indx=0;indx<fleets;indx++) {
		pagenumber=1;		/* printed page counter */

/*		report_name = get_rp_name(fleet_id[indx],report_no,end_date);*/

		/* ISGTEQ points to the first record matching fleet with time now or later */
		shiftrec->fleet = fleet_id;
		shiftrec->due_date_time = 0; /* make the assignment */
		shiftrec->pri_status = 0;
		shiftrec->drv_id = 0;
		printedlines = print_heading_1(fleet_id,start_date,start_time,end_date,end_time,fleet_name);


		ccode=db(CALL_FILE_ID,READ_KEYS,&cl_key9,ISGTEQ,0);
#ifdef DEBUG
/*printf("da_calls_t: output to %s shiftrec->fleet %c \n",report_name,shiftrec->fleet);*/
#endif
		endofpage = 44;		/* check this end of page to group one record on a page */
		while (ccode>=0 && shiftrec->fleet==fleet_id) { 
			/* continue while matching fleet */
			if (printedlines>endofpage )
			{
			    if(endofpage == 44)/* only print the header first time through save 12 lines */
			    {
				printedlines=0;
				endofpage = 56;
			     }
			    printedlines = 0;
			    putchar('\f');
			}
			if(shiftrec->pri_status == 64 )
				break;
					if( shiftrec->due_date_time < ending_dt_tm && shiftrec->due_date_time > start_dt_tm)
				print_record_1();			/* print the transaction in buf */

			ccode=db(CALL_FILE_ID,READ_KEYS,&cl_key9,ISNEXT,0);
		}	/* end of while 1 */

		print_ending(fleet_id,catgets(REP_catd, 1, 2, "UNASSIGNED CALLS"));


	}	/* end for each fleet */

        /* close language catalogues */
        cleanup_catalog(REP_catd);
        cleanup_catalog(COMMON_m_catd);

}	/* end of report_fleet */

/*====================================================================
= print_heading_1() -
=	Prints the field definitions for this report.
=====================================================================*/
print_heading_1(fleet_id,start_date,start_time,end_date,end_time,fleet_name)
char fleet_id;
char *start_date;
char *start_time;
char *end_date;
char *end_time;
char *fleet_name;
{
	char title2[50];
	sprintf(title2,catgets(REP_catd, 1, 3, "     UNASSIGNED CALLS by call due time"));

	/* print the two line heading. update the page counter */
	report_heading(seconds,start_date,start_time,end_date,end_time,fleet_name,title2,&pagenumber);
/* seperator line */
printf("====================================================================================================================================\n");

/* line three */
printf(catgets(REP_catd, 1, 4, "TYPE ....DUE TIME... ZONE CITY ...........PICKUP ADDRESS.............  APT  ......PASSENGER..... ...PHONE NUMBER... PR #C FL  CALL #\n"));

/* line four */
printf(catgets(REP_catd, 1, 5, "     ..FARE.. CA1.2.3.4.5.6.7.8        ......INTERSECTION STREET.....      DA1.2.3.4.5.6.7.8  POD .....SIGNED BY...... .TIME. .WITH.\n"));

/* line five */
printf(catgets(REP_catd, 1, 6, "     ..RATE.. PERSNL ZONE CITY .........DESTINATION ADDRESS..........  APT  ....REQUESTED BY.... ....PERSON TO SEE... HOLD LOC\n"));

/* line six */
printf(catgets(REP_catd, 1, 7, "     ..ENTERED TIME.. .BY. CHNG ..EST.. ..DISPATCH TIME. .BY. ...PICKUP TIME.. ...CLOSE TIME... VEH# DRV# .STATUS. SUBSC#.\n"));

/* line seven */
printf(catgets(REP_catd, 1, 8, "     ......CHARGE #....... EXPIRES. .APPR CODE .BY. ....COMPANY NAME.... ...USER ENTERED 1... ...USER ENTERED 2...\n"));

/* line eight */
printf(catgets(REP_catd, 1, 9, "     .............................................COMMENT LINE........................................................\n"));

/* line nine */
	printf(catgets(REP_catd, 1, 10, "     .........DRIVER MESSAGE.............\n"));

/* seperator line */
printf("====================================================================================================================================\n");
	putchar('\n');
	return(0);

}	/* end of print_heading */

/*====================================================================
= print_record_1() -
=	Prints the lines of the current call record
=	that have alphanumeric characters.
=====================================================================*/
print_record_1()
{
	char iobuff[164];	/* print buffer */
	char tbuff[132];	/* temporary buffer */
	char e_date[9];
        char *currency_symbol;
	char *last_six();

if ( (currency_symbol = nl_langinfo(CRNCYSTR)) == NULL)
   currency_symbol = " "; 

/* line one, is always printed */
/* line one, 23 fields, note pckup_str_nbr and pckup_str_suf form one field*/
sprintf(iobuff,"%-5.5s %-9.9s %-6.6s %3d  %-3.3s ",
shiftrec->extended_type, shiftrec->due_date, shiftrec->due_time, shiftrec->pckup_zone, shiftrec->pckup_city);
printf("%s",iobuff);

sprintf(iobuff,"%-20.20s ", shiftrec->pckup_str_name);
printf(iobuff);

sprintf(iobuff,"%1c %-2.2s ", shiftrec->pckup_str_suf, shiftrec->pckup_pre_dir);
printf(iobuff);
if(shiftrec->pckup_str_nbr == 0)
	sprintf(iobuff,"       ");
else
	sprintf(iobuff,"%7d",shiftrec->pckup_str_nbr);
printf(iobuff);

sprintf(iobuff,"%-3.3s %-2.2s %-5.5s %-20.20s ",
shiftrec->pckup_str_type, shiftrec->pckup_post_dir, shiftrec->pckup_apt_room, shiftrec->passenger_name);
printf(iobuff);

#ifdef SCANDINAVIA
sprintf(iobuff," %-18.18s %2d %1c %1c %6s\n", shiftrec->phone_number, shiftrec->priority, shiftrec->nbr_of_vehs, shiftrec->fleet,last_six( shiftrec->nbr));
#else
sprintf(iobuff," (%-3.3s)%-8.8s %-3.3s %2d %1c %1c %6s\n", shiftrec->phone_area, shiftrec->phone_nbr, shiftrec->phone_ext, shiftrec->priority, shiftrec->nbr_of_vehs, shiftrec->fleet,last_six(shiftrec->nbr));
#endif
printf(iobuff);
printedlines++;

/* lines two thru seven are printed only if they have data */
/* a floating point number is stored as the null string. Print only positive numbers */
if (shiftrec->fare_amount > 0.0) 
#ifdef SCANDINAVIA
   sprintf(iobuff,"     %7.2f%s ", shiftrec->fare_amount, currency_symbol);
#else
   sprintf(iobuff,"     %s%7.2f ", currency_symbol,shiftrec->fare_amount);
#endif
else 
   sprintf(iobuff,"              ");

if ( Toupper(shiftrec->veh_attr_flag) == YES && strlen(shiftrec->veh_attr)>7)
sprintf(tbuff,"%1c %1c %1c %1c %1c %1c %1c %1c %1c ", shiftrec->veh_attr_flag,
shiftrec->veh_attr[0], shiftrec->veh_attr[1], shiftrec->veh_attr[2], shiftrec->veh_attr[3], 
shiftrec->veh_attr[4], shiftrec->veh_attr[5], shiftrec->veh_attr[6], shiftrec->veh_attr[7]);
else sprintf(tbuff,"%1c                 ",shiftrec->veh_attr_flag);
strcat(iobuff,tbuff);

if ( Toupper(shiftrec->drv_attr_flag) == YES && strlen(shiftrec->drv_attr)>7)
sprintf(tbuff,"     %-2.2s %-20.20s %-3.3s %-2.2s      %1c:%1c %1c %1c %1c %1c %1c %1c %1c   %1c %-20.20s   %6d %6s\n",
shiftrec->intr_pre_dir, shiftrec->intr_str_name, shiftrec->intr_str_type, shiftrec->intr_post_dir,
shiftrec->drv_attr_flag, 
shiftrec->drv_attr[0], shiftrec->drv_attr[1], shiftrec->drv_attr[2], shiftrec->drv_attr[3], 
shiftrec->drv_attr[4], shiftrec->drv_attr[5], shiftrec->drv_attr[6], shiftrec->drv_attr[7], 
shiftrec->pod_required, shiftrec->pod_name, shiftrec->pod_time, last_six(shiftrec->grouped_with));
else
sprintf(tbuff,"     %-2.2s %-20.20s %-3.3s %-2.2s      %1c:                                  %1c %-20.20s   %6d %6s\n",
shiftrec->intr_pre_dir, shiftrec->intr_str_name, shiftrec->intr_str_type, shiftrec->intr_post_dir,
shiftrec->drv_attr_flag, 
shiftrec->pod_required, shiftrec->pod_name, shiftrec->pod_time, last_six(shiftrec->grouped_with));
strcat(iobuff,tbuff);

/* print this line only if it contains data */
if (printable_line(iobuff)) { printf(iobuff); printedlines++; }	/* end if */

/* line three */
/* a floating point number is stored as the null string. Print only positive numbers */
if (shiftrec->call_rate > 0.0) 
#ifdef SCANDINAVIA
   sprintf(iobuff,"     %7.2f%s  " , shiftrec->call_rate, currency_symbol);
#else
   sprintf(iobuff,"     %s%7.2f  " , currency_symbol,shiftrec->call_rate);
#endif
else 
   sprintf(iobuff,"               ");

sprintf(tbuff,"%04d%1c ",
shiftrec->personal_veh, shiftrec->personal_rqst);
strcat(iobuff,tbuff);
if( shiftrec->dest_zone == 0)
	sprintf(tbuff,"     ");
else
	sprintf(tbuff,"%3d ", shiftrec->dest_zone); 
strcat(iobuff,tbuff);
sprintf(tbuff,"%-3s  ",shiftrec->dest_city);
strcat(iobuff,tbuff);
if(shiftrec->dest_str_nbr == 0)
	sprintf(tbuff,"      ");
else
	sprintf(tbuff,"%6d",shiftrec->dest_str_nbr);
strcat(iobuff,tbuff);
sprintf(tbuff," %1c %-2s %-20s %-3s %-2s %-5s %-20s %-20s %-4s %1c\n",
shiftrec->dest_str_suf, shiftrec->dest_pre_dir, shiftrec->dest_str_name, 
shiftrec->dest_str_type, shiftrec->dest_post_dir, 
shiftrec->dest_apt_room, shiftrec->requested_by, shiftrec->person_to_see, shiftrec->hold_line, shiftrec->local);
strcat(iobuff,tbuff);

/* print this line only if it contains data */
if (printable_line(iobuff)) { printf(iobuff); printedlines++; }	/* end if */

/* line four, put a blank into null fields 
pad_null(shiftrec->est_wait_time);
pad_null(shiftrec->dispd_date); 
pad_null(shiftrec->dispd_time); 
pad_null(shiftrec->pckup_date);
pad_null(shiftrec->pckup_time);
pad_null(shiftrec->close_date);
pad_null(shiftrec->close_time); */
sprintf(iobuff,
"     %-9.9s %-6.6s %4d ",
shiftrec->date_in, shiftrec->time_in, shiftrec->entered_by);
if(shiftrec->last_changed_by == 0) 
	sprintf(tbuff,"    ");
else
	sprintf(tbuff,"%4d",shiftrec->last_changed_by);
strcat(iobuff,tbuff);
sprintf(tbuff,
" %-7.7s %-9.9s %-6.6s %4d %-9.9s %-6.6s %-9.9s %-6.6s %4d %5d %-8.8s ",
shiftrec->est_wait_time, shiftrec->dispd_date, shiftrec->dispd_time, shiftrec->dispatched_by, 
shiftrec->pckup_date, shiftrec->pckup_time, shiftrec->close_date, shiftrec->close_time, 
shiftrec->veh_nbr, shiftrec->drv_id, shiftrec->status);
strcat(iobuff,tbuff);
if(shiftrec->subsc_call_nbr == 0)
	sprintf(tbuff,"    \n");
else
	sprintf(tbuff,"%4d\n",shiftrec->subsc_call_nbr);
strcat(iobuff,tbuff);

/* print this line only if it contains data */
if (printable_line(iobuff)) { printf(iobuff); printedlines++; }	/* end if */

/* line five */
/* get the date_time structure */
if (shiftrec->expire_date >0) {
	local_date_time(shiftrec->expire_date, e_date, NULL, NULL );
	sprintf(iobuff,"     %-18.18s %s %-10.10s %d %-20.20s %-20.20s %-20.20s\n",
	shiftrec->charge_nbr, e_date,
	shiftrec->approve_code, shiftrec->approve_by, shiftrec->company_name, 
	shiftrec->ac_addt1_info1, shiftrec->ac_addt1_info2);
	/* print this line only if it contains data */
	if (printable_line(iobuff)) { printf(iobuff); printedlines++; }	/* end if */
}	/* end if has a date to print */

/* line six */
sprintf(iobuff,"     %-120.120s\n",shiftrec->general_cmnt);
/* print this line only if it contains data */
if (printable_line(iobuff)) { printf(iobuff); printedlines++; }	/* end if */

/* line seven */
sprintf(iobuff,"     %-37.37s\n",shiftrec->driver_msg);
/* print this line only if it contains data */
if (printable_line(iobuff)) { printf(iobuff); printedlines++; }	/* end if */

	putchar('\n');
	printedlines++;
	return(0);
}	/* end print_record */

/* pad_null() - puts a blank into a field if and only if that field was null */
pad_null(stringptr)
char *stringptr;
{ if (! strlen(stringptr)) stringptr=" "; }	/* end pad_null */

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

	printf(catgets(REP_catd, 1, 11, "Report for Date: %8s to %8s                  %20s                  Page: %3d   Date Printed: %s\n"), start_date,end_date,fleetname,*pagenumber, current_date);
	(*pagenumber)++;	/* increment the pagenumber */

	printf(catgets(REP_catd, 1, 12, "Report Coverage: %8s to %8s      %-55s       Time Printed: %s\n\n"), start_time,end_time,title2, current_time);

	return(0);
}	/* end of report_heading */

print_ending(fleet_id,report_title)
char fleet_id;
char *report_title;
{
	int i;

	/* a few blank lines */
	putchar('\n');
	for(i=1; i< (112 - strlen(report_title))/2 ;i++)
		printf("=");	
	printf(
catgets(REP_catd, 1, 13, " END OF %s FOR FLEET %c "),
	report_title,fleet_id);
	for(i=1; i< (112 - strlen(report_title))/2 ;i++)
		printf("=");	
	putchar('\n');
	fflush(stdout);
	return(0);
}	/* end of print_ending */
