static char sz__FILE__[]="@(#)da_calls.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  da_calls.c; Rel: 6.3.0.0; Get date: 8/12/92 at 10:42:38
* @(#)  Last delta: 8/6/92 at 17:17:14
* @(#)  SCCS File: /taxi/sccs/s.da_calls.c
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
#include <langinfo.h>
#include <isam.h>
#include <sys/types.h>

#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "ex_key.h"
#include "rep_strdefs.h"
#include "language.h"
#include "path.h"

extern	char buf[];	/* for db put and get records       */
extern	char req_buf[];	/* buffer to cast as call structure */
extern	int db();
extern	char *get_rp_name();
struct	cisam_cl *shiftrec;

FILE *fileptr;
long seconds;			/* current date and time in seconds */
int pagenumber;		/* printed page counter */
int printedlines;		/* global printed line counter */
#define blanks "                    "

#ifdef SCANDINAVIA
#define LINESPERPAGE	41
#else
#define LINESPERPAGE 53		/* lines per page */
#endif
#ifdef SCANDINAVIA
#define ENDOFPAGE	32
#else
#define ENDOFPAGE	44
#endif
/*====================================================================
= da_calls()
=	Print messages for the particular fleet.
=	Is quite specific to this report.
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=====================================================================*/
da_calls(fleet_id,start_date,start_time,start_dt_tm,end_date,end_time,ending_dt_tm,fleet_name)
char fleet_id;
char *start_date;
char *start_time;
long start_dt_tm;
char *end_date;
char *end_time;
long ending_dt_tm;
char *fleet_name;
{
	char *report_name;
	int ccode;		/* returned from proc calls */
	register int indx;
	int report_no = 1;	/* will print with leading zero */
	int endofpage;
	char 	path_name[81];				/* Holds name of report file */

	seconds = time( (long *) 0);			/* current date and time in seconds */
	shiftrec=(struct cisam_cl*)req_buf;	/* to cast req_buf */

	/* open the cisam CALL file, using the composite key3 */
	ccode=db(SHIFT_FILE_ID,OPEN,&sh_key2,ISINPUT | ISMANULOCK,0);	/* open CALL db for read only */
	if (ccode<0) {
		printf(catgets(REP_catd, 6, 1, "Error OPENing file %d iserrno %d\n"),SHIFT_FILE_ID,iserrno);
		return(-1);
	}	/* end if ccode */

		pagenumber=1;		/* printed page counter */

		report_name = get_rp_name(fleet_id,report_no,end_date);
		sprintf(path_name, "%s/%s", REPORT_DATA, report_name);
		fileptr = fopen(path_name,"w");

		/* ISGTEQ points to the first record matching fleet with time now or later */
		shiftrec->fleet = fleet_id;
		shiftrec->due_date_time = 0; /* make the assignment */

		printedlines = print_heading_1(fleet_id,start_date,start_time,end_date,end_time,fleet_name);
		ccode=db(SHIFT_FILE_ID,READ_KEYS,&sh_key2,ISGTEQ,0);
#ifdef DEBUG
printf("da_calls_t: output to %s shiftrec->fleet %c \n",report_name,shiftrec->fleet);
#endif
		endofpage = ENDOFPAGE;		/* check this end of page to group one record on a page */
		while (ccode>=0 && shiftrec->fleet==fleet_id) { 
			/* continue while matching fleet */
			if (printedlines>endofpage )
			{
			    if(endofpage == ENDOFPAGE)/* only print the header first time through save 12 lines */
			    {
				printedlines=0;
				endofpage = LINESPERPAGE;
			     }
			    printedlines = 0;
			    putc('\f',fileptr);
			}
			print_record_1();			/* print the transaction in buf */
			ccode=db(SHIFT_FILE_ID,READ_KEYS,&sh_key2,ISNEXT,0);
		}	/* end of while 1 */

		if (shiftrec->fleet != fleet_id || iserrno == EENDFILE) 
			print_ending(fileptr,fleet_id,catgets(REP_catd, 6, 2, "CALL DISTRIBUTION by cab"));

		else {	/* print an error reading file message */
			fprintf(fileptr,catgets(REP_catd, 6, 3, "%sThere Were No CALL Records.\n"),blanks);
			print_ending(fileptr,fleet_id,catgets(REP_catd, 6, 2, "CALL DISTRIBUTION by cab"));
		}
		fclose(fileptr);

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
	sprintf(title2,catgets(REP_catd, 6, 4, "           CALLS by call due time"));

/*	putc('\f',fileptr);*/
	/* print the two line heading. update the page counter */
	report_heading(seconds,start_date,start_time,end_date,end_time,fleet_name,title2,&pagenumber,fileptr);

/* seperator line */
fprintf(fileptr,"====================================================================================================================================\n");

/* line three */
fprintf(fileptr,catgets(REP_catd, 6, 5, "TYPE ....DUE TIME... ZONE CITY ...........PICKUP ADDRESS.............  APT  ......PASSENGER..... ...PHONE NUMBER... PR #C FL  CALL #\n"));

/* line four */
fprintf(fileptr,catgets(REP_catd, 6, 6, "     ..FARE.. CA1.2.3.4.5.6.7.8        ......INTERSECTION STREET.....      DA1.2.3.4.5.6.7.8  POD .....SIGNED BY...... .TIME. .WITH.\n"));

/* line five */
fprintf(fileptr,catgets(REP_catd, 6, 7, "     ..RATE.. PERSNL ZONE CITY .........DESTINATION ADDRESS..........  APT  ....REQUESTED BY.... ....PERSON TO SEE... HOLD LOC\n"));

/* line six */
fprintf(fileptr,catgets(REP_catd, 6, 8, "     ..ENTERED TIME.. .BY. CHNG ..EST.. ..DISPATCH TIME. .BY. ...PICKUP TIME.. ...CLOSE TIME... VEH# DRV# .STATUS. SUBSC#.\n"));

/* line seven */
fprintf(fileptr,catgets(REP_catd, 6, 9, "     ......CHARGE #....... EXPIRES. .APPR CODE .BY. ....COMPANY NAME.... ...USER ENTERED 1... ...USER ENTERED 2...\n"));

/* line eight */
fprintf(fileptr,catgets(REP_catd, 6, 10, "     .............................................COMMENT LINE........................................................\n"));

/* line nine */
	fprintf(fileptr,catgets(REP_catd, 6, 11, "     .........DRIVER MESSAGE.............\n"));

/* seperator line */
fprintf(fileptr,"====================================================================================================================================\n");
	putc('\n',fileptr);
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


if ( (currency_symbol = nl_langinfo(CRNCYSTR)) == NULL)
   currency_symbol = " "; 

/* line one, is always printed */
/* line one, 23 fields, note pckup_str_nbr and pckup_str_suf form one field*/
sprintf(iobuff,"%-5.5s %-9.9s %-6.6s %3d  %-3.3s ",
shiftrec->extended_type, shiftrec->due_date, shiftrec->due_time, shiftrec->pckup_zone, shiftrec->pckup_city);
fprintf(fileptr,iobuff);
if(shiftrec->pckup_str_nbr == 0)
	sprintf(tbuff,"      ");
else
#ifdef NAMERICA
	sprintf(tbuff,"%6d",shiftrec->pckup_str_nbr);
sprintf(iobuff,"%s%c %-2.2s %-20.20s %-3.3s %-2.2s %-5.5s %-20.20s ",
tbuff, shiftrec->pckup_str_suf, shiftrec->pckup_pre_dir, shiftrec->pckup_str_name,
shiftrec->pckup_str_type, shiftrec->pckup_post_dir, shiftrec->pckup_apt_room, shiftrec->passenger_name);
#else
	sprintf(tbuff,"%-6d",shiftrec->pckup_str_nbr);
sprintf(iobuff,"   %20s %s        %-5.5s %-20.20s ",
shiftrec->pckup_str_name, tbuff, shiftrec->pckup_apt_room, shiftrec->passenger_name);
#endif

fprintf(fileptr,iobuff);
#ifdef SCANDINAVIA
sprintf(iobuff," %-18.18s %2d %c %c %7d\n", shiftrec->phone_number, shiftrec->priority, shiftrec->nbr_in_group, shiftrec->fleet, shiftrec->nbr);
#else
sprintf(iobuff," (%-3.3s)%-8.8s %-3.3s %2d %1c %1c %7d\n", shiftrec->phone_area, shiftrec->phone_nbr, shiftrec->phone_ext, shiftrec->priority, shiftrec->nbr_in_group, shiftrec->fleet, shiftrec->nbr);
#endif
fprintf(fileptr,iobuff);
printedlines++;

/* lines two thru seven are printed only if they have data */
/* a floating point number is stored as the null string. Print only positive numbers */
if (shiftrec->fare_amount > 0.0) 
#ifdef SCANDINAVIA
   sprintf(iobuff,"     %7.2f%s ", shiftrec->fare_amount, currency_symbol);
#else
   sprintf(iobuff,"     %s%7.2f ", currency_symbol, shiftrec->fare_amount);
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
sprintf(tbuff,"       %-2.2s %-20.20s %-3.3s %-2.2s      %1c %1c %1c %1c %1c %1c %1c %1c %1c %1c %-20.20s   %6d %6d\n",
shiftrec->intr_pre_dir, shiftrec->intr_str_name, shiftrec->intr_str_type, shiftrec->intr_post_dir,
shiftrec->drv_attr_flag, 
shiftrec->drv_attr[0], shiftrec->drv_attr[1], shiftrec->drv_attr[2], shiftrec->drv_attr[3], 
shiftrec->drv_attr[4], shiftrec->drv_attr[5], shiftrec->drv_attr[6], shiftrec->drv_attr[7], 
shiftrec->pod_required, shiftrec->pod_name, shiftrec->pod_time, shiftrec->grouped_with);
else
sprintf(tbuff,"       %-2.2s %-20.20s %-3.3s %-2.2s      %1c                 %-20.20s   %6d %6d\n",
shiftrec->intr_pre_dir, shiftrec->intr_str_name, shiftrec->intr_str_type, shiftrec->intr_post_dir, shiftrec->pod_required, shiftrec->pod_name, shiftrec->pod_time, shiftrec->grouped_with);
/*shiftrec->drv_attr_flag, 
shiftrec->drv_attr[0], shiftrec->drv_attr[1], shiftrec->drv_attr[2], shiftrec->drv_attr[3], 
shiftrec->drv_attr[4], shiftrec->drv_attr[5], shiftrec->drv_attr[6], shiftrec->drv_attr[7], 
*/
strcat(iobuff,tbuff);

/* print this line only if it contains data */
if (printable_line(iobuff)) { fprintf(fileptr,iobuff); printedlines++; }	/* end if */

/* line three */
/* a floating point number is stored as the null string. Print only positive numbers */
if (shiftrec->call_rate > 0.0) 
#ifdef SCANDINAVIA
   sprintf(iobuff,"     %7.2f%s  " ,shiftrec->call_rate, currency_symbol);
#else
   sprintf(iobuff,"     %s%7.2f  " ,currency_symbol, shiftrec->call_rate);
#endif
else 
   sprintf(iobuff,"               ");

sprintf(tbuff,"%04d%1c ",
shiftrec->personal_veh, shiftrec->personal_rqst);
if( shiftrec->dest_zone == 0)
	sprintf(tbuff,"     ");
else
	sprintf(tbuff,"%3d ", shiftrec->dest_zone); 
strcat(iobuff,tbuff);
sprintf(tbuff,"%-3s  ",shiftrec->dest_city);
strcat(iobuff,tbuff);
if (printable_line(iobuff))
  fprintf(fileptr,iobuff);

if(shiftrec->dest_str_nbr == 0)
	sprintf(tbuff,"      ");
else
#ifdef NAMERICA
	sprintf(tbuff,"%6d",shiftrec->dest_str_nbr);
sprintf(iobuff,"%s%c %-2s %-20s %-3s %-2s %-5s %-20s %-20s %-4s %1c\n",
tbuff, shiftrec->dest_str_suf, shiftrec->dest_pre_dir, shiftrec->dest_str_name, 
shiftrec->dest_str_type, shiftrec->dest_post_dir, 
shiftrec->dest_apt_room, shiftrec->requested_by, shiftrec->person_to_see, shiftrec->hold_line, shiftrec->local);
#else
	sprintf(tbuff,"%-6d",shiftrec->dest_str_nbr);
sprintf(iobuff,"   %20s %s        %-5s %-20s %-20s %-4s %1c\n",
shiftrec->dest_str_name, tbuff,
shiftrec->dest_apt_room, shiftrec->requested_by, shiftrec->person_to_see, shiftrec->hold_line, shiftrec->local);
#endif
/* print this line only if it contains data */
if (printable_line(iobuff))
  {
    fprintf(fileptr,iobuff);
    printedlines++;
  }

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
if (printable_line(iobuff)) { fprintf(fileptr,iobuff); printedlines++; }	/* end if */

/* line five */
/* get the date_time structure */
if (shiftrec->charge_nbr != '\0') {
        local_date_time(shiftrec->expire_date, e_date, NULL, NULL);
	sprintf(iobuff,"     %-18.18s %c %-10.10s %d %-20.20s %-20.20s %-20.20s\n",
	shiftrec->charge_nbr, ' ',
	shiftrec->approve_code, shiftrec->approve_by, shiftrec->company_name, 
	shiftrec->ac_addt1_info1, shiftrec->ac_addt1_info2);
	/* print this line only if it contains data */
	if (printable_line(iobuff)) { fprintf(fileptr,iobuff); printedlines++; }	/* end if */
}	/* end if has a date to print */

/* line six */
sprintf(iobuff,"     %-120.120s\n",shiftrec->general_cmnt);
/* print this line only if it contains data */
if (printable_line(iobuff)) { fprintf(fileptr,iobuff); printedlines++; }	/* end if */

/* line seven */
sprintf(iobuff,"     %-37.37s\n",shiftrec->driver_msg);
/* print this line only if it contains data */
if (printable_line(iobuff)) { fprintf(fileptr,iobuff); printedlines++; }	/* end if */

	fputc('\n',fileptr);
	printedlines++;
	return(0);
}	/* end print_record */

/* pad_null() - puts a blank into a field if and only if that field was null */
pad_null(stringptr)
char *stringptr;
{ if (! strlen(stringptr)) stringptr=" "; }	/* end pad_null */

