static char sz__FILE__[]="@(#)man_veh.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  man_veh.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:22:02
 * @(#)  Last delta: 12/14/89 at 17:10:20
 * @(#)  SCCS File: /taxi/sccs/s.man_veh.c
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
#include "mad_error.h"
#include "key.h"
#include "except.h"
#include "calls.h"
#include "mad_ipc.h"
#include "rep_strdefs.h"

extern	char buf[];	/* for db put and get records       */
extern	char req_buf[];	/* buffer to cast as call structure */
extern	int db();
extern	char *get_rp_name();
struct	cisam_cl *shiftrec;

FILE *fileptr;
long seconds;			/* current date and time in seconds */
int pagenumber;			/* printed page counter */
int printedlines;		/* global printed line counter */
#define blanks "                    "

#define endofpage 44		/* check this end of page to group one record on a page */
#define linesperpage 53		/* lines per page */


main()
{
	int sys_day_start, sys_day_end, sys_prod_start_time;
	char 	sys_gen_rpt1_n[40],sys_print_rpt1_n[40];
	char 	start_date[10];
	char 	start_time[10];
	char 	end_date[10];
	char 	end_time[10];
	char fleet_name[FLEET_MAX][21];
	char fleet_id[FLEET_MAX+1];

	get_syscntl(&sys_day_start,&sys_day_end,&sys_prod_start_time,sys_gen_rpt1_n,sys_print_rpt1_n);
	get_act_fleets(fleet_id,fleet_name);
	da_manual(fleet_id,start_date,start_time,end_date,end_time,fleet_name);

}

/*********************************************************************
* da_manual()
**********************************************************************/
da_manual (fleet_id,start_date,start_time,end_date,end_time,fleet_name)
char fleet_id[];
char *start_date;
char *start_time;
char *end_date;
char *end_time;
char *fleet_name[FLEET_MAX][21];
{
	char buffer[264];
	char *report_name;

	int ccode;		/* returned from proc calls */
	register int indx;
	int report_no = 12;	/* will print with leading zero */
	int fleets=strlen(fleet_id);

	seconds = time( (long *) 0);			/* current date and time in seconds */
	shiftrec=(struct cisam_cl*)req_buf;	/* to cast req_buf */
	pagenumber=1;		/* printed page counter */

	ccode=db(SHIFT_FILE_ID,OPEN,&sh_key5,ISINPUT | ISMANULOCK,0);	/* open CALL db for read only */
	if (ccode<0) {
		printf(catgets(REP_catd, 1, 1, "Error OPENing file %d iserrno %d\n"),SHIFT_FILE_ID,iserrno);
		exit(-1);
	}	

	for (indx=0;indx<fleets;indx++) {
		report_name = get_rp_name(fleet_id[indx],report_no,end_date);
		fileptr = fopen(report_name,"w");

		/* ISGTEQ points to the first record matching fleet with time now or later */
		shiftrec->fleet = fleet_id[indx];
		shiftrec->dispatched_by = 1; 					/* the first possible dispatcher */
		shiftrec->due_date_time = 0; 					/* make the assignment */

		printedlines = print_heading_12(fleet_id[indx],start_date,start_time,end_date,end_time,fleet_name[indx]);
		ccode=db(SHIFT_FILE_ID,READ_KEYS,&sh_key5,ISGTEQ,0);

		while (ccode>=0 && shiftrec->fleet==fleet_id[indx]) { 	/* continue while matching fleet */
			if(shiftrec->dispatched_by >  0){ 		/* must be manually dispatched */
				if (printedlines>endofpage) printedlines=
					print_heading_12(fleet_id[indx],start_date,start_time,end_date,end_time,fleet_name[indx]);
				print_record_12(); 
			}
			ccode=db(SHIFT_FILE_ID,READ_KEYS,&sh_key5,ISNEXT,0);
		}

		print_ending(fileptr,fleet_id[indx],catgets(REP_catd, 1, 2, "MANUALLY Dispatched Calls"));
		fclose(fileptr);

	}
	ccode=db(SHIFT_FILE_ID,CLOSE,&sh_key5,ISNEXT,0);
}	

/*====================================================================
= print_heading_12() -
=====================================================================*/
print_heading_12(fleet_id,start_date,start_time,end_date,end_time,fleet_name)
char fleet_id;
char *start_date;
char *start_time;
char *end_date;
char *end_time;
char *fleet_name;
{
	char title2[60];
	sprintf(title2,catgets(REP_catd, 1, 3, "MANUALLY DISPATCHED CALLS by dispatcher by due time"));

	putc('\f',fileptr);
	/* print the two line heading. update the page counter */
	report_heading(seconds,start_date,start_time,end_date,end_time,fleet_name,title2,&pagenumber,fileptr);

/* seperator line */
fprintf(fileptr,"====================================================================================================================================\n");

/* line three */
fprintf(fileptr,catgets(REP_catd, 1, 4, "TYPE ....DUE TIME... ZONE CITY ...........PICKUP ADDRESS.............  APT  ......PASSENGER..... ...PHONE NUMBER... PR #C FL  CALL #\n"));

/* line four */
fprintf(fileptr,catgets(REP_catd, 1, 5, "     ..FARE.. CA1.2.3.4.5.6.7.8        ......INTERSECTION STREET.....      DA1.2.3.4.5.6.7.8  POD .....SIGNED BY...... .TIME. .WITH.\n"));

/* line five */
fprintf(fileptr,catgets(REP_catd, 1, 6, "     ..RATE.. PERSNL ZONE CITY .........DESTINATION ADDRESS..........  APT  ....REQUESTED BY.... ....PERSON TO SEE... HOLD LOC\n"));

/* line six */
fprintf(fileptr,catgets(REP_catd, 1, 7, "     ..ENTERED TIME.. .BY. CHNG ..EST.. ..DISPATCH TIME. .BY. ...PICKUP TIME.. ...CLOSE TIME... VEH# DRV# .STATUS. SUBSC#.\n"));

/* line seven */
fprintf(fileptr,catgets(REP_catd, 1, 8, "     ......CHARGE #....... EXPIRES. .APPR CODE .BY. ....COMPANY NAME.... ...USER ENTERED 1... ...USER ENTERED 2...\n"));

/* line eight */
fprintf(fileptr,catgets(REP_catd, 1, 9, "     .............................................COMMENT LINE...............................................................\n"));

/* line nine */
	fprintf(fileptr,catgets(REP_catd, 1, 10, "     .........DRIVER MESSAGE.............\n"));

/* seperator line */
fprintf(fileptr,"====================================================================================================================================\n");
	putc('\n',fileptr);
	return(0);

}	/* end of print_heading */

/*====================================================================
= print_record_12() -
=	Prints the lines of the current call record
=	that have alphanumeric characters.
=====================================================================*/
print_record_12()
{
	char iobuff[200];	/* line buffer */
	char e_date[9];
        char *currency_symbol;

if ( (currency_symbol = nl_langinfo(CRNCYSTR)) == NULL)
   currency_symbol = " "; 

/* line one, 23 fields */
sprintf(iobuff,"%-5s %-9s %-5s %3d %-3s %6d %1c %-2s %-20s %-3s %-2s %-5s %-20s ",
shiftrec->extended_type, shiftrec->due_date, shiftrec->due_time, shiftrec->pckup_zone, shiftrec->pckup_city,
shiftrec->pckup_str_nbr, shiftrec->pckup_str_suf, shiftrec->pckup_pre_dir, shiftrec->pckup_str_name,
shiftrec->pckup_str_type, shiftrec->pckup_post_dir, shiftrec->pckup_apt_room, shiftrec->passenger_name);
fprintf(fileptr,iobuff);

#ifdef SCANDINAVIA
sprintf(iobuff," %-18s  %2d %1c  %1c   %4d\n", shiftrec->phone_number, shiftrec->priority, shiftrec->nbr_in_group, shiftrec->fleet, shiftrec->nbr);
#else
sprintf(iobuff," (%-3s)%-8s %-3s  %2d %1c  %1c   %4d\n", shiftrec->phone_area, shiftrec->phone_nbr, shiftrec->phone_ext, shiftrec->priority, shiftrec->nbr_in_group, shiftrec->fleet, shiftrec->nbr);
#endif
fprintf(fileptr,iobuff);
printedlines++;

/* line two */
#ifdef SCANDINAVIA
sprintf(iobuff, "     %7.2f%s %1c %1c %1c %1c %1c %1c %1c %1c %1c %-2s %-20s %-3s %-2s %1c %1c %1c %1c %1c %1c %1c %1c %1c %1c %-20s %6d %6d\n", shiftrec->fare_amount, currency_symbol, shiftrec->veh_attr_flag, shiftrec->veh_attr[0], shiftrec->veh_attr[1], shiftrec->veh_attr[2], shiftrec->veh_attr[3], shiftrec->veh_attr[4], shiftrec->veh_attr[5], shiftrec->veh_attr[6], shiftrec->veh_attr[7], shiftrec->intr_pre_dir, shiftrec->intr_str_name, shiftrec->intr_str_type, shiftrec->intr_post_dir, shiftrec->drv_attr_flag, shiftrec->drv_attr[0], shiftrec->drv_attr[1], shiftrec->drv_attr[2], shiftrec->drv_attr[3], shiftrec->drv_attr[4], shiftrec->drv_attr[5], shiftrec->drv_attr[6], shiftrec->drv_attr[7], shiftrec->pod_required, shiftrec->pod_name, shiftrec->pod_time, shiftrec->grouped_with);
#else
sprintf(iobuff, "     %s%7.2f %1c %1c %1c %1c %1c %1c %1c %1c %1c %-2s %-20s %-3s %-2s %1c %1c %1c %1c %1c %1c %1c %1c %1c %1c %-20s %6d %6d\n", currency_symbol,shiftrec->fare_amount, shiftrec->veh_attr_flag, shiftrec->veh_attr[0], shiftrec->veh_attr[1], shiftrec->veh_attr[2], shiftrec->veh_attr[3], shiftrec->veh_attr[4], shiftrec->veh_attr[5], shiftrec->veh_attr[6], shiftrec->veh_attr[7], shiftrec->intr_pre_dir, shiftrec->intr_str_name, shiftrec->intr_str_type, shiftrec->intr_post_dir, shiftrec->drv_attr_flag, shiftrec->drv_attr[0], shiftrec->drv_attr[1], shiftrec->drv_attr[2], shiftrec->drv_attr[3], shiftrec->drv_attr[4], shiftrec->drv_attr[5], shiftrec->drv_attr[6], shiftrec->drv_attr[7], shiftrec->pod_required, shiftrec->pod_name, shiftrec->pod_time, shiftrec->grouped_with);
#endif

if (printable_line(iobuff)) {
	fprintf(fileptr,iobuff);
	printedlines++;
}	/* end if */

/* line three */
#ifdef SCANDINAVIA
sprintf(iobuff, "     %7.2f%s  %4d%c  %3d %-3s %6d %1c %-2s %-20s %-3s %-2s %-5s %-20s %-20s %-4s %1c\n", shiftrec->call_rate, currency_symbol, shiftrec->personal_veh, shiftrec->personal_rqst, shiftrec->dest_zone, shiftrec->dest_city, shiftrec->dest_str_nbr, shiftrec->dest_str_suf, shiftrec->dest_pre_dir, shiftrec->dest_str_name, shiftrec->dest_str_type, shiftrec->dest_post_dir, shiftrec->dest_apt_room, shiftrec->requested_by, shiftrec->person_to_see, shiftrec->hold_line, shiftrec->local);
#else
sprintf(iobuff, "     %s%7.2f  %4d%c  %3d %-3s %6d %1c %-2s %-20s %-3s %-2s %-5s %-20s %-20s %-4s %1c\n", currency_symbol,shiftrec->call_rate, shiftrec->personal_veh, shiftrec->personal_rqst, shiftrec->dest_zone, shiftrec->dest_city, shiftrec->dest_str_nbr, shiftrec->dest_str_suf, shiftrec->dest_pre_dir, shiftrec->dest_str_name, shiftrec->dest_str_type, shiftrec->dest_post_dir, shiftrec->dest_apt_room, shiftrec->requested_by, shiftrec->person_to_see, shiftrec->hold_line, shiftrec->local);
#endif

if (printable_line(iobuff)) {
	fprintf(fileptr,iobuff);
	printedlines++;
}	/* end if */

/* line four */
sprintf(iobuff,
"     %-9s %-6s %4d %4d  %-7s %-9s %-6s %4d %-9s %-6s %-9s %-6s %4d %5d %-8s %4d\n",
shiftrec->date_in, shiftrec->time_in, shiftrec->entered_by,
shiftrec->last_changed_by, shiftrec->est_wait_time, 
shiftrec->dispd_date, shiftrec->dispd_time, shiftrec->dispatched_by, 
shiftrec->pckup_date, shiftrec->pckup_time, shiftrec->close_date, shiftrec->close_time, 
shiftrec->veh_nbr, shiftrec->drv_id, shiftrec->status, shiftrec->subsc_call_nbr);
fprintf(fileptr,iobuff);	/* will print the dispatched_by field and possibly more */
printedlines++;

/* line five */
/* get the date_time structure */
local_date_time(shiftrec->expire_date, e_date, NULL, NULL );
sprintf(iobuff,"     %-18s %s %-10s %d %-20s %-20s %-20s\n",
shiftrec->charge_nbr, e_date,
shiftrec->approve_code, shiftrec->approve_by, shiftrec->company_name, 
shiftrec->ac_addt1_info1, shiftrec->ac_addt1_info2);
if (printable_line(iobuff)) {
	fprintf(fileptr,iobuff);
	printedlines++;
}	/* end if */

/* line six */
sprintf(iobuff,"     %-120s\n",shiftrec->general_cmnt);
if (printable_line(iobuff)) {
	fprintf(fileptr,iobuff);
	printedlines++;
}	/* end if */

/* line seven */
sprintf(iobuff,"     %-37s\n",shiftrec->driver_msg);
if (printable_line(iobuff)) {
	fprintf(fileptr,iobuff);
	printedlines++;
}	/* end if */

	putc('\n',fileptr);
	printedlines++;
	return(0);
}	/* end print_record */
