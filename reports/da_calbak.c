static char sz__FILE__[]="@(#)da_calbak.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  da_calbak.c; Rel: 5.4.0.0; Get date: 3/6/90 at 14:34:52
 * @(#)  Last delta: 3/5/90 at 13:26:55
 * @(#)  SCCS File: /taxi/sccs/s.da_calbak.c
 *                                                                           *
 *****************************************************************************/

/********************************************************************************/
/*										*/
/*  Function: 	da_calbak() - Run daily callback report.			*/
/*										*/
/********************************************************************************/
										
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <isam.h>
#include <sys/types.h>

#include "mads_types.h"
#include "ex_key.h"
#include "mads_isam.h"
#include "mad_error.h"
#include "acc.h"
#include "veh_driv.h"
#include "df_maint.h"
#include "writer.h"
#include "path.h"
#include "rep_strdefs.h"
#include "writer_strdefs.h"

#ifdef SCANDINAVIA
#define LINESPERPAGE 43
#else
#define	LINESPERPAGE	55
#endif
#define C_CALLBACK	'1' 
#define C_MFAULT	'2'
#define C_PICKUP	'3'
#define C_NOSHOW	'4'
#define TRUE  1
#define FALSE 0

extern	char buf[];				/* for db put and get records       */
extern	char req_buf[];				/* buffer to cast as call structure */
extern	int db();

struct	cisam_ch *shifthrec;

struct	cb_rec {				/* Holds info from call history */
	long 	time;
	char	event;
	int	veh;
	long	drv;
};	
struct	cb_rec	cb_ary[64];

struct	pr_rec {				/* Holds info to be printed */
	int	veh;
	long	drv;
	int	cb;
	int	mfault;
	int	pick;	
	int	nshow;
};	
struct	pr_rec	pr_ary[VEH_DRIV_MAX];
struct	pr_rec	total;

int	num_ex;					/* Number of indexes in cb_ary */
int	num_pr;					/* Number of indexes in pr_rec */
int	pagenumber;				/* Current page number */
int	numlines;				/* Current line number on page */
FILE 	*fileptr;				/* Ptr to output file */


/********************************************************************************/
/*										*/
/*  Function: 	da_calbak() - Run daily callback report.			*/
/*										*/
/*  Calls:	db() - Routine to access database.				*/
/*         	report() - Report daily callbacks from shift file.		*/
/*	   	ERROR() - Macro to handle error messages.			*/
/*		strlen() - Returns the length of a string.			*/
/*		exit() - Exit to UNIX.						*/
/*										*/
/*  Returns: None.								*/
/*										*/
/********************************************************************************/

da_calbak(fleet_id, start_date, start_time, start_dt_tm, end_date, end_time, ending_dt_tm, fleet_name)
char 	fleet_id;			/* Array of fleets for report */
char 	*start_date;			/* Start date of report */
char 	*start_time;			/* Start time of report */
long 	start_dt_tm;			/* Start date and time in C format */
char 	*end_date;			/* End date of report */
char 	*end_time;			/* End time of report */
long 	ending_dt_tm;			/* End date and time in C format */
char 	*fleet_name;			/* Array of fleet names for report */
{

	if(db(SHIFTH_FILE_ID, OPEN, &hh_key1, ISINPUT | ISMANULOCK, 0) < 0) {
		ERROR(' ', SHIFTH_FILE, "Error OPENING")
		return(-1);
	}

	shifthrec = (struct cisam_ch*)req_buf;

	report(fleet_id, start_date, start_time, start_dt_tm, end_date, end_time, ending_dt_tm, fleet_name);

	if(db(SHIFTH_FILE_ID, CLOSE, &hh_key1, 0, 0) < 0) {
		ERROR(' ', SHIFTH_FILE, "Error CLOSING")
		return(-1);
	}
}


/********************************************************************************/
/*										*/
/*  Function:	report() - Report daily callbacks from shift file.		*/
/*										*/
/*  Calls:	get_rp_name() - Returns file name for report.			*/
/*		fopen() - Open a file.						*/
/*		memset() - Set memory range to a char value.			*/
/*        	db() - Routine to access database.				*/
/*		sort() - Sort callback recs in ascending time.			*/
/*		proc_rec() - Get info for report from callback recs.		*/
/*		prt_rec() - Print out callback info.				*/
/*		fclose() - close file						*/
/*										*/
/********************************************************************************/

report(fleet_id, start_date, start_time, start_dt_tm, end_date, end_time, ending_dt_tm, fleet_name)
char 	fleet_id;
char 	*start_date;
char 	*start_time;
long 	start_dt_tm;
char 	*end_date;
char 	*end_time;
long 	ending_dt_tm;
char 	*fleet_name;
{
	long	tmp_call_nbr;
	int	pre_mfault, pre_pick, pre_nshow;
	int	rd_type;
	char 	*report_name;				/* Holds name of report file */
	char 	path_name[81];				/* Holds name of report file */

	extern	char *get_rp_name();			/* Returns name for disk file */

	report_name = get_rp_name(fleet_id, 16, end_date);
	sprintf(path_name, "%s/%s", REPORT_DATA, report_name);
	fileptr = fopen(path_name, "w");
	pagenumber = 1;

	memset((char *)shifthrec, '\0', sizeof(struct cisam_ch));	/* clear shift history space */	
	shifthrec->call_nbr = 0;
	tmp_call_nbr = 0;
	num_ex = 0;
	num_pr = 0;

	numlines = print_heading(start_date, start_time, start_dt_tm, end_date, end_time, ending_dt_tm, fleet_name);
	rd_type = ISGTEQ;
	while(db(SHIFTH_FILE_ID, READ_KEYS, &hh_key1, rd_type, 0) >= 0) {
		rd_type = ISNEXT;

		if(shifthrec->fleet != fleet_id)
			continue;
	
		if(shifthrec->call_nbr != tmp_call_nbr) {
			sort();
			proc_rec();
			num_ex = 0;
			tmp_call_nbr = shifthrec->call_nbr;
		}

		if(strncmp(shifthrec->event_desc, ACT_CALLBACK, strlen(ACT_CALLBACK)) == 0)
			cb_ary[num_ex].event = C_CALLBACK;
		else if(strncmp(shifthrec->event_desc,EVNT_MFAULT ,strlen(EVNT_MFAULT) ) == 0)
			cb_ary[num_ex].event = C_MFAULT;
		else if(strncmp(shifthrec->event_desc, EVNT_PCKUP,strlen( EVNT_PCKUP)) == 0)
			cb_ary[num_ex].event = C_PICKUP;
		else if(strncmp(shifthrec->event_desc, EVNT_NOSHOW , strlen(EVNT_NOSHOW)) == 0)
			cb_ary[num_ex].event = C_NOSHOW;
		else 
			cb_ary[num_ex].event = 'X';

		cb_ary[num_ex].time = shifthrec->status_dt_tm;
		cb_ary[num_ex].veh = shifthrec->veh_nbr;
		cb_ary[num_ex].drv = shifthrec->drv_id;
		num_ex++;
	}
	prt_rec(fleet_id, start_date, start_time, start_dt_tm, end_date, end_time, ending_dt_tm, fleet_name);
	fprintf(fileptr, "------------------------------------------------------------------------------------------------------------------------------------\n");
	fprintf(fileptr, catgets(REP_catd, 5, 3, " TOTALS: Drivers %5d          %4d               %4d                    %4d                  %4d\n"),
		total.drv,  total.cb, total.mfault, total.pick, total.nshow);
	fprintf(fileptr, "====================================================================================================================================\n");
	print_ending(fileptr, fleet_id, catgets(REP_catd, 5, 4, "CALLBACK CALLS by driver"));

	fclose(fileptr);

}


sort()
{
	int	y, x;
	struct	cb_rec	tmp_rec;

	for(y = 0; y < num_ex - 1; y++)
		for(x = 0; x < num_ex - 1; x++) 
			if(cb_ary[x].time > cb_ary[x + 1].time) {
				tmp_rec.time = cb_ary[x + 1].time;
				tmp_rec.event = cb_ary[x + 1].event;
				tmp_rec.veh = cb_ary[x + 1].veh;
				tmp_rec.drv = cb_ary[x + 1].drv;
				cb_ary[x + 1].time = cb_ary[x].time;
				cb_ary[x + 1].event = cb_ary[x].event;
				cb_ary[x + 1].veh = cb_ary[x].veh;
				cb_ary[x + 1].drv = cb_ary[x].drv;
				cb_ary[x].time = tmp_rec.time;
				cb_ary[x].event = tmp_rec.event;
				cb_ary[x].veh = tmp_rec.veh;
				cb_ary[x].drv = tmp_rec.drv;
			}
}


proc_rec()
{
	int	x;
	int	drv, veh;
	int	mfault, pick, nshow;

	drv = 0; veh = 0;
	mfault = 0; pick = 0; nshow = 0;

	for(x = 0; x < num_ex; x++) {
		if(cb_ary[x].drv != drv && cb_ary[x].drv != 0)
			drv = cb_ary[x].drv;	
		if(cb_ary[x].veh != veh && cb_ary[x].veh != 0)
			veh = cb_ary[x].veh;	

		switch(cb_ary[x].event) {
			case C_MFAULT:
				mfault = 1;
				break;
	
			case C_NOSHOW:
				nshow = 1;
				break;

			case C_CALLBACK:
				if(drv != 0)
					upd_rec(drv, veh, mfault, pick, nshow);

				mfault = 0; pick = 0; nshow = 0;
				break;

			case 'X':
				break;
		}
	}
}


upd_rec(drv, veh, mfault, pick, nshow)
int	drv, veh, mfault, pick, nshow;
{
	int	x;

	for(x = 0; x < num_pr; x++) 
		if(pr_ary[x].drv == drv) {
			if(veh != 0)
				pr_ary[x].veh = veh;
			pr_ary[x].drv = drv;
			pr_ary[x].cb += 1;
			pr_ary[x].mfault += mfault;
			pr_ary[x].pick += pick;
			pr_ary[x].nshow += nshow;
			break;
		}
				
	if(x == num_pr) {
		pr_ary[x].veh = veh;
		pr_ary[x].drv = drv;
		pr_ary[x].cb = 1;
		pr_ary[x].mfault = mfault;
		pr_ary[x].pick = pick;
		pr_ary[x].nshow = nshow;
		num_pr++;
	}
}


prt_rec(fl_id, start_date, start_time, start_dt_tm, end_date, end_time, ending_dt_tm, fleet_name)
char	fl_id;
char 	*start_date;
char 	*start_time;
long 	start_dt_tm;
char 	*end_date;
char 	*end_time;
long 	ending_dt_tm;
char 	*fleet_name;
{
	int	x;


	bub_sort();
	for(x = 0; x< num_pr; x++) {
		fprintf(fileptr, " %5d  %4d                     %3d                %3d                     %3d                   %3d\n",
			pr_ary[x].drv, pr_ary[x].veh, pr_ary[x].cb, pr_ary[x].mfault, pr_ary[x].pick, pr_ary[x].nshow);

		total.drv += 1;
		total.cb += pr_ary[x].cb;
		total.mfault += pr_ary[x].mfault;
		total.pick += pr_ary[x].pick;
		total.nshow += pr_ary[x].nshow;

		numlines++;
		if(numlines > LINESPERPAGE) {
			putc('\f',fileptr);
			numlines = print_heading(start_date, start_time, start_dt_tm, end_date, end_time, ending_dt_tm, fleet_name);
		}	
	}

}

/************************************************************************/
/* print_heading() -							*/
/*	Prints the field definitions for this report.			*/
/************************************************************************/
print_heading(start_date, start_time, start_dt_tm, end_date, end_time, ending_dt_tm, fleet_name)
char 	*start_date;
char 	*start_time;
long 	start_dt_tm;
char	*end_date;
char 	*end_time;
long 	ending_dt_tm;
char 	*fleet_name;
{
	long 	seconds;						/* number of seconds to back from current time */
	char 	title[50];
	char    current_date[20], current_time[20];
	sprintf(title,catgets(REP_catd, 5, 5, "          CALLBACK CALLS by driver"));

	seconds = time((long *)0);				/* current date and time in seconds */
	local_date_time(seconds, current_date, current_time, NULL);
	

	fprintf(fileptr, 
		catgets(REP_catd, 5, 6,
			"Report for Date: %8s to %8s                  %20s                  Page: %3d   Date Printed: %s\n"),
		start_date, end_date, fleet_name, pagenumber, current_date);
	pagenumber++;			/* increment the pagenumber */

	fprintf(fileptr, catgets(REP_catd, 5, 7, "Report Coverage: %8s to %8s      %-55s       Time Printed: %s\n\n"),
		start_time, end_time, title, current_time);

	fprintf(fileptr, "====================================================================================================================================\n");

	fprintf(fileptr, catgets(REP_catd, 5, 8, " DRV#   VEH#                  CALLBACKS   PRECEDED BY MFAULT   PRECEDED BY PICKUP   PRECEDED BY NTRIP\n"));

	fprintf(fileptr, "====================================================================================================================================\n");

	return(5);
}
bub_sort()   /* bubble sort */
{

	int	y, x,SORTED;
	struct	pr_rec	tmp_rec;

	SORTED = FALSE;
	while(!SORTED && num_pr > 1) {
		SORTED = TRUE;
		for(x = 0; x < num_pr - 1; x++) { 
			if(pr_ary[x].drv > pr_ary[x + 1].drv) {
				printf("DID SW    ***");
				SORTED = FALSE;
				tmp_rec.drv = pr_ary[x + 1].drv;
				tmp_rec.cb = pr_ary[x + 1].cb;
				tmp_rec.veh = pr_ary[x + 1].veh;
				tmp_rec.mfault = pr_ary[x + 1].mfault;
				tmp_rec.pick = pr_ary[x + 1].pick;
				tmp_rec.nshow = pr_ary[x + 1].nshow;

				pr_ary[x + 1].drv = pr_ary[x].drv;
				pr_ary[x + 1].cb = pr_ary[x].cb;
				pr_ary[x + 1].veh = pr_ary[x].veh;
				pr_ary[x + 1].mfault = pr_ary[x].mfault;
				pr_ary[x + 1].pick = pr_ary[x].pick;
				pr_ary[x + 1].nshow = pr_ary[x].nshow;

				pr_ary[x].drv = tmp_rec.drv;
				pr_ary[x].cb = tmp_rec.cb;
				pr_ary[x].veh = tmp_rec.veh;
				pr_ary[x].mfault = tmp_rec.mfault;
				pr_ary[x].pick = tmp_rec.pick;
				pr_ary[x].nshow = tmp_rec.nshow;
			}
		}
	}
}
