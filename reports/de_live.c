static char sz__FILE__[]="@(#)de_live.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  de_live.c; Rel: 10.1.0.0; Get date: 4/18/95 at 17:01:40
* @(#)  Last delta: 12/2/94 at 11:47:57
* @(#)  SCCS File: /taxi/sccs/s.de_live.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <sys/types.h>
#include "machdep.h"
#include "mads_types.h"
#include "key.h"
#include "ex_key.h"
#include "mad_error.h"
#include "ui_def.h"
#include "path.h"
#define DECLARING
#include "comm_strdefs.h"
#include "rep_strdefs.h"

extern long conversion();
extern	char buf[];	/* for db put and get records       */
extern	char req_buf[];	/* buffer to cast as call structure */
extern	int db();
extern	char *get_rp_name();
struct	cisam_cl *shiftrec;
extern  void local_date_time();

FILE *fileptr;
long seconds;			/* current date and time in seconds */
int pagenumber;		/* printed page counter */
int printedlines;		/* global printed line counter */
#define blanks "                    "

#define linesperpage 53		/* lines per page */

char sysbuf[120];
/*====================================================================
= main()
=	Reports all unassigned calls per fleet.
=====================================================================*/

main(argc,argv)
int argc;
char *argv[];	
{
	register int indx;
	int goodfleets;
	char systembuff[132];
	char fleet_id[FLEET_MAX+1];
	char *fleet_name[FLEET_MAX+1];	/* will malloc room for each fleet name */
	char start_date[10];
	char start_time[10];
	char end_date[10];
	char end_time[10];
	char dt_tm_buff[13];
	long start_dt_tm;		/* the initial start report time */
	long ending_dt_tm;		/* the initial start report time */
	char *fl_name_ptr;		/* Points to fleet name */
	char *getenv();
	char *mads_spooler;
        char mbuf[20];
	short fleet_num[9];
        char dummy[40];

        /* initialize language environment */
        init_locale();
        REP_catd = init_catalog("de_live");
        COMMON_m_catd = init_catalog("common");

        mads_spooler = mbuf;
	if ((mads_spooler = getenv("MADS_SPOOLER")) == NULL) {		/* get the environment variable, MADS_SPOOLER */
		mads_spooler = PRINT_CMD;			/* use default value */
	}

	CLEAR;
	puts("\n\n\t\t==================================================================\n");
	puts(  catgets(REP_catd, 1, 1, "\n\t\t             REPORTING LIVE TIME CALLS FOR FLEETS\n\n"));

	goodfleets = choose_fleets(fleet_id,fleet_name,1,fleet_num);

	puts(  	"\n\t\t==================================================================\n");

	if (goodfleets) {
		/* prompt for the starting and ending dates and times */
		ch_dt_tminf(start_date,start_time,end_date,end_time);

		/* map the dates and times into integers */
		map_dt_tm(start_date,start_time,dt_tm_buff);
		start_dt_tm = conversion(dt_tm_buff);

		if(!isdigit(end_date[0]))
			date_time(end_date, systembuff);

		map_dt_tm(end_date,end_time,dt_tm_buff);
		ending_dt_tm = conversion(dt_tm_buff);

                /* localize date/time */
                local_date_time(start_dt_tm, start_date, start_time, dummy);
                local_date_time(ending_dt_tm, end_date, end_time, dummy);

		/* report for each fleet */
		for (indx=0; indx<goodfleets; indx++) {
			fl_name_ptr = fleet_name[indx];
			while(*fl_name_ptr != '\0') {
				if(*fl_name_ptr == ' ')
					*fl_name_ptr = '_';
				fl_name_ptr++;
			}

			sprintf(systembuff,"%s %c %s %s %ld \"%s\" %s %ld \"%s\" | %s",
				DE_LMAIN, fleet_id[indx],start_date,start_time,start_dt_tm, end_date,end_time,
				ending_dt_tm,fleet_name[indx],mads_spooler);

			puts("\n\t\t==================================================================\n");
			printf(catgets(REP_catd, 1, 2, "\t\t             PREPARING REPORT FOR FLEET %c\n"),fleet_id[indx]);
			system(systembuff);
			printf(catgets(REP_catd, 1, 3, "\t\t             REPORT FOR FLEET %c IS PRINTING\n"),fleet_id[indx]);
			puts(  "\t\t==================================================================\n");

		}
	}
	else {
		puts(catgets(REP_catd, 1, 4, "\n\t\tNO fleets will be reported.\n "));
		puts(catgets(REP_catd, 1, 5, "\t\tPlease, run this program later. <CR> to continue.."));
		fflush(stdin);
		gets(fleet_id);
		return(0);
	}

        /* close language catalogues */
        cleanup_catalog(REP_catd);
        cleanup_catalog(COMMON_m_catd);

	return(goodfleets);
}

