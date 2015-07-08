static char sz__FILE__[]="@(#)da_cabs_hr.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  da_cabs_hr.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:20:34
 * @(#)  Last delta: 12/14/89 at 16:59:53
 * @(#)  SCCS File: /taxi/sccs/s.da_cabs_hr.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "ex_key.h"
#include "rep_strdefs.h"
#include "path.h"

extern	char buf[];								/* for db put and get records       */
extern	char req_buf[];								/* buffer to cast as call structure */
extern	int db();
extern	char *get_rp_name();

struct	cisam_cl *shiftrec;
struct	cisam_vh *vehrec;

										/* the accumulators for five reports follow: */
int total_cab_hr_count[25];							/* report total counters */
int total_hr_count[25];

										/* the accumulators for five reports follow: */
#define MAXVEH  2500
int cabs_in_fleet[MAXVEH];								/* count of all cabs in this fleet */
int tot_cabs_in_fleet;
int hour_mask[24];								/* hour bit masks for bits 0..23 */
int cab_marker[10000];						/* one word per cab number with one bit per hour of the day */

FILE *fileptr;
long seconds;									/* current date and time in seconds */
int pagenumber;									/* printed page counter */
int printedlines;								/* global printed line counter */

static int no_of_cabs = 10000;
#define blanks "                    "
#ifdef SCANDINAVIA
#define LINESPERPAGE 41								/* lines per page */
#else
#define LINESPERPAGE 53
#endif

/*====================================================================
= da_cabs_hr()
=	Prints calls by cab by time.
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=====================================================================*/
da_cabs_hr(fleet_id,start_date,start_time,start_dt_tm,end_date,end_time,ending_dt_tm,fleet_name)
char fleet_id;
char *start_date;
char *start_time;
long start_dt_tm;
char *end_date;
char *end_time;
long ending_dt_tm;
char *fleet_name;
{
	struct tm *tt;

	char buffer[264];
	register int cab_no;
	char *report_name;
	char  path_name[81];
	char report_title[64];

	int ccode;							/* returned from proc calls */
	int fleetno;
	register int indx;
	int report_no;							/* will print with leading zero */
	int i;

	seconds = time( (long *) 0);					/* current date and time in seconds */
									/* each mask element has a 1 in the bit for that hour */
	for (indx=0; indx<24; indx++) hour_mask[indx] = (1<<indx);

#ifdef DEBUG
	printf("hour masks ");
	for (indx=0; indx<24; indx++)  printf("%d",hour_mask[indx]);
#endif

	pagenumber=1;							/* printed page counter */

#ifdef DEBUG
	printf("\noperating on %d fleet %c \n",fleetno,fleet_id);
#endif
									/* count the number of cabs for this fleet */
									/* initialize the shiftrec structure */
	vehrec=(struct cisam_vh*)req_buf;				/* to cast req_buf */
	vehrec->fleet = fleet_id;
	vehrec->nbr = 0; 
	for (i=0;i< MAXVEH;i++) cabs_in_fleet[i] = 0;

	if( db(VEHICLE_FILE_ID,OPEN,&veh_key2,ISINPUT | ISMANULOCK,0) < 0){ /* the composite key4, fleet & veh_nbr */
		printf(catgets(REP_catd, 4, 1, "Error OPENing file %d iserrno %d\n"),VEHICLE_FILE_ID,iserrno);
		return(-1);
	}	

	i = 0;
	tot_cabs_in_fleet = 0;
	ccode=db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISGTEQ,0);
	while (ccode>=0 && vehrec->fleet==fleet_id) { 			/* continue while matching fleet */
		cab_no=vehrec->nbr;
		cabs_in_fleet[i] = cab_no;
		tot_cabs_in_fleet++;
		i++;
		while ( ccode>=0 && cab_no==vehrec->nbr ) /* continue while matching vehicle number */
			ccode=db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISNEXT,0);	/* get next record */
	}	/* end of matching fleet_id */
	ccode=db(VEHICLE_FILE_ID,CLOSE,&veh_key2,0,0);


#ifdef DEBUG
printf("cabs_in_fleet %d \n",cabs_in_fleet);
#endif

	/* zero 10,000 cab flags, one bit per hour of the day */
	/* each bit 0 .. 23 represents an hour and is 1 if and only if this cab received a call that hour */
	for (indx=0; indx<no_of_cabs; indx++) cab_marker[indx]=0;

	/* initialize the shiftrec structure */
	shiftrec=(struct cisam_cl*)req_buf;	/* to cast req_buf */
	shiftrec->fleet = fleet_id;
	shiftrec->veh_nbr = 0; /* for every pickup zone */
	shiftrec->due_date_time = 0; 

	/* open the cisam SHIFT file, using the composite key4, fleet & pckup_zone */
	ccode=db(SHIFT_FILE_ID,OPEN,&sh_key4,ISINPUT | ISMANULOCK,0);	/* open CALL db for read only */
	if (ccode<0) {
		printf(catgets(REP_catd, 4, 1, "Error OPENing file %d iserrno %d\n"),SHIFT_FILE_ID,iserrno);
		return(-1);
	}	/* end if ccode */

	report_name = get_rp_name(fleet_id,7,end_date);
	sprintf(path_name, "%s/%s", REPORT_DATA, report_name);
	fileptr = fopen(path_name,"w");

	printedlines = print_heading_7(fleet_id,start_date,start_time,end_date,end_time,fleet_name);
	/* zero total report counters */
	for (indx=0; indx<25; indx++) total_hr_count[indx]=0;
	for (i = 0; i < MAXVEH; i++)
	{
		cab_no=cabs_in_fleet[i];	/* keep matching cab number */
		shiftrec->fleet = fleet_id;
		shiftrec->veh_nbr = cab_no;
		ccode=db(SHIFT_FILE_ID,READ_KEYS,&sh_key4,ISGTEQ,0);
		for (indx=0; indx<25; indx++) total_cab_hr_count[indx]=0;	/* report total counters */
		while ( ccode>=0 && cab_no==shiftrec->veh_nbr && cab_no != 0 ) { /* continue while matching vehicle number */
			/* count only records that were dispatched */
			if ((shiftrec->veh_nbr != 0)&&(shiftrec->close_date_time > 0)) {	/* sum all accumulators at the record level */
				tt=localtime( &(shiftrec->due_date_time) );
				cab_marker[cab_no] |= hour_mask[tt->tm_hour];	/* mark the hour this cab was busy */
				total_cab_hr_count[tt->tm_hour]++; /* counters for max_super_zone zones, 24 hours */
				total_cab_hr_count[24]++;		/* total calls for this zone */
			}	/* end if dispatched */

			ccode=db(SHIFT_FILE_ID,READ_KEYS,&sh_key4,ISNEXT,0);	/* get next record */
		}	/* end of matching vehicle number */

		/* sum for all cabs */
		for (indx=0;indx<25;indx++) {
			total_hr_count[indx] += total_cab_hr_count[indx];
		}

		/* print the cab detail line */
		if (printedlines>=LINESPERPAGE) {
			putc('\f',fileptr);
			printedlines = print_heading_7(fleet_id,start_date,start_time,end_date,end_time,fleet_name);
		}	
		print_record_7(cab_no);			/* print the transaction in buf */
	}
	ccode=db(SHIFT_FILE_ID,CLOSE,&sh_key4,ISNEXT,0);

	print_total_7();	/* the sum of all cab calls */
	fclose(fileptr);

}	/* end of report_fleet */

/*====================================================================
= print_heading_7() -
=	Prints the field definitions for this report.
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=====================================================================*/
print_heading_7(fleet_id,start_date,start_time,end_date,end_time,fleet_name)
char fleet_id;
char *start_date;
char *start_time;
char *end_date;
char *end_time;
char *fleet_name;
{
	char title2[50];
	sprintf(title2,catgets(REP_catd, 4, 2, "     CALL DISTRIBUTION by cab by due time"));

/*	putc('\f',fileptr);*/
	putc('\n',fileptr);

	/* print the standard daily report heading */
	report_heading(seconds,start_date,start_time,end_date,end_time,fleet_name,title2,&pagenumber,fileptr);

/* seperator line */
fprintf(fileptr,"====================================================================================================================================\n");

fprintf(fileptr,catgets(REP_catd, 4, 3, "      TOTAL           MIDNIGHT                       MORNING                       NOON                         AFTERNOON        \n"));

fprintf(fileptr,catgets(REP_catd, 4, 4, "CAB#  CALLS    10   11   12    1    2    3    4    5    6    7    8    9   10   11   12    1    2    3    4    5    6    7    8    9\n"));

/* seperator line */
fprintf(fileptr,"====================================================================================================================================\n");
	return(0);

}	/* end of print_heading */

/*====================================================================
= print_record_7() -
=	Prints the lines of the current call record
=	that have alphanumeric characters.
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=
=====================================================================*/
print_record_7(cab_no)
int cab_no;
{
	register int indx;
	int report_indx;

	/* skip the unassigned calls with cab_no 0 */
	if (!cab_no) return(0);

	/* print zone number */
	fprintf(fileptr,"%4d  ",cab_no);	
	/* print shift total for this zone */
	fprintf(fileptr,"%5d ",total_cab_hr_count[24]); /* shift total for zone */
	/* print hour totals for hours 10, 11, and 12 midnight */
	for (indx=21; indx<24; indx++) 
		if ( total_cab_hr_count[indx] ) fprintf(fileptr," %4d",total_cab_hr_count[indx]); 
		else fprintf(fileptr,"     "); 
	/* print hourly totals from 1am to 9pm */
	for (indx=0; indx<21; indx++) 
		if ( total_cab_hr_count[indx] ) fprintf(fileptr," %4d",total_cab_hr_count[indx]); 
		else fprintf(fileptr,"     "); 
	putc('\n',fileptr);
	printedlines++;
	return(0);
}	/* end print_record */

/*=====================================================================
= print_total_7() -
=	Prints the super zone total
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=
=====================================================================*/
print_total_7()
{
	register int indx, indx2;
	register hour_of_day;
	int cab_counter[24];

	/* print total calls each hour */
	fprintf(fileptr,"====================================================================================================================================\n");

	fprintf(fileptr,"%6s ",catgets(REP_catd, 4, 5, "Total:") ); 					/* shift total for zone */
	fprintf(fileptr,"%5d",total_hr_count[24]); 

	for(indx = 21; indx < 24; indx++) {					/* print hourly totals from 10pm to 12pm */
		if(total_hr_count[indx]) fprintf(fileptr, " %4d", total_hr_count[indx]);
		else fprintf(fileptr, "     "); 
	}

	for (indx=0; indx<21; indx++) {						/* print hourly totals from 1am to 9pm */
		if(total_hr_count[indx]) fprintf(fileptr, " %4d", total_hr_count[indx]); 
		else fprintf(fileptr, "     "); 
	}

	putc('\n',fileptr);

	/* print percentages */
	fprintf(fileptr,"%6s ",catgets(REP_catd, 4, 6, "Percent:") ); 
	fprintf(fileptr,"100");	/* must sum to 100 percent */

	if(total_hr_count[24]) {
		for(indx = 21; indx < 24; indx++) 					/* print hourly totals from 10pm to 12pm */
			fprintf(fileptr, " %4.1f", 100.0 * (float) total_hr_count[indx] / (float) total_hr_count[24]); 
		for(indx = 0; indx < 21; indx++) 
			fprintf(fileptr, " %4.1f", 100.0 * (float) total_hr_count[indx] / (float) total_hr_count[24]); 
	}

	putc('\n',fileptr);
	fprintf(fileptr,"====================================================================================================================================\n");

	/* count all busy cabs each hour */
	for (indx=0; indx<24; indx++) {
		cab_counter[indx]=0;
		hour_of_day= hour_mask[indx];
		/* scan all cabs counting all busy cabs this hour */
		for (indx2=0; indx2<no_of_cabs; indx2++)
			if (cab_marker[indx2] & hour_of_day) cab_counter[indx]++;
	}	/* sum all busy cabs for each hour */

										/* print cabs that were busy each hour */
	fprintf(fileptr,"%6s",catgets(REP_catd, 4, 7, "Active Cabs:") ); 				/* shift total for zone */

	for(indx = 21; indx < 24; indx++) {					 /* print from 10pm to 12am */
		if(cab_counter[indx]) fprintf(fileptr, " %4d", cab_counter[indx]);
		else fprintf(fileptr,"     "); 
	}

	for(indx = 0; indx < 21; indx++) {					 /* print hourly totals from 1am to 9pm */
		if(cab_counter[indx]) fprintf(fileptr, " %4d", cab_counter[indx]); 
		else fprintf(fileptr, "     "); 
	}

	putc('\n',fileptr);

	/* print percentages */
	fprintf(fileptr,"%6s ",catgets(REP_catd, 4, 8, "Percent:   ") ); 

	if(tot_cabs_in_fleet) {							 /* print from 10pm */
		for(indx = 21; indx<24; indx++) 
			fprintf(fileptr," %4.1f",100.0 * (float) cab_counter[indx] / (float) tot_cabs_in_fleet); 

		for(indx = 0; indx < 21; indx++) 
			fprintf(fileptr," %4.1f",100.0 * (float) cab_counter[indx] / (float) tot_cabs_in_fleet); 
	}

	putc('\n',fileptr);
	fprintf(fileptr,"====================================================================================================================================\n");

	printedlines += 7;
	return(0);
}	/* end print_total_7 */



