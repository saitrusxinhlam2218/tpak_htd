static char sz__FILE__[]="@(#)choosedttm.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  choosedttm.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:59:53
* @(#)  Last delta: 12/2/94 at 11:53:02
* @(#)  SCCS File: /taxi/sccs/s.choosedttm.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "rep_strdefs.h"

#define blanks "                    "

extern void local_date_time();
/*====================================================================
= choose_dt_tm() 
=	Prompt for date and time to report.
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=
=====================================================================*/
choose_dt_tm(start_date,start_time,end_date,end_time)
char *start_date, *start_time;
char *end_date, *end_time;
{
        long current_dt_tm;
        char dummy[40];

	/* read the starting date and time */
        current_dt_tm = time ( (long *) 0);
        local_date_time(current_dt_tm, start_date, dummy, dummy);
	printf(catgets(REP_catd, 11, 1, "\n%sEnter the starting date (i.e., %s) or <CR> for today:"),blanks,start_date); 
	fflush(stdout);
	fflush(stdin);
	gets(start_date);
	if (strlen(start_date)<8)  {
          /* date_time(start_date,start_time; */
           current_dt_tm = time ( (long *) 0);
           local_date_time(current_dt_tm, start_date, start_time, dummy);
        }
	printf(catgets(REP_catd, 11, 2, "\n%sEnter the starting time HH:MM  or <CR> for first record:"),blanks); 
	fflush(stdout);
	fflush(stdin);
	gets(start_time);
	if(strlen(start_time) < 5) 
		strcpy(start_time,"00:00:00");
	else {		/* fill out the seconds with zeros */
		start_time[5]=':';
		start_time[6]='0';
		start_time[7]='0';
	}

	/* read the ending date and time */
	printf(catgets(REP_catd, 11, 3, "\n%sEnter the ending date (i.e., %s) or <CR> for all current days:"),blanks,end_date); 
	fflush(stdout);
	fflush(stdin);
	gets(end_date);
	if (strlen(end_date)<8) {
		strcpy(end_date,"all days");
		strcpy(end_time,"23:59:59");
	}
	else { 		/* prompt for the ending time */
		printf(catgets(REP_catd, 11, 4, "\n%sEnter the ending time as HH:MM  or <CR> for all records:"),blanks); 
	        fflush(stdout);
		fflush(stdin);
		gets(end_time);
		if (strlen(end_time)<5) strcpy(end_time,"23:59:59");
		else {	/* fill out the seconds with zeros */
			end_time[5]=':';
			end_time[6]='0';
			end_time[7]='0';
		}
	}
	return(1);
}

/*====================================================================
= choose2_dt_tm() 
=	Prompt for date and time to report.
=====================================================================*/

choose2_dt_tm(start_date,start_time,end_date,end_time)
char *start_date, *start_time;
char *end_date, *end_time;
{

	char temp_date[10];
	char temp_time[10];
        long current_dt_tm;
        char dummy[40];

        current_dt_tm = time ( (long *) 0);
        local_date_time(current_dt_tm, start_date, dummy, dummy);
	printf(catgets(REP_catd, 11, 5, "\n%sEnter the starting date (i.e., %s) or hit <CR> for today:"),blanks,start_date); 
	fflush(stdout);
	fflush(stdin);
	gets(temp_date);
	if (verify_date(temp_date,start_date) == -1) { /* if invalid date, default to NOW */
	   /*get_srch_date(time(NULL),start_date);*/
           current_dt_tm = time ( (long *) 0);
           local_date_time(current_dt_tm, start_date, dummy, dummy);
        }

        current_dt_tm = time ( (long *) 0);
        local_date_time(current_dt_tm-(60*60), dummy, start_time, dummy);
        start_time[5] = '\0';
	printf(catgets(REP_catd, 11, 6, "\n%sEnter the starting time as HH:MM or hit <CR> for 60 minutes back:"),blanks); 
	fflush(stdout);
	fflush(stdin);
	gets(temp_time);
        if (temp_time[0] != '\0') {
           if (verify_time(temp_time,start_time) == -1) {/* if invalid time or <CR>, default to NOW-60 min*/
	       /*get_srch_time(time(NULL)-60*60,start_time);*/
               current_dt_tm = time ( (long *) 0);
               local_date_time(current_dt_tm-(60*60), dummy, start_time, dummy);
           }
        }

        current_dt_tm = time ( (long *) 0);
        local_date_time(current_dt_tm, end_date, dummy, dummy);
	printf(catgets(REP_catd, 11, 7, "\n%sEnter the ending date (i.e., %s) or hit <CR> for today:"),blanks,end_date); 
	fflush(stdout);
	fflush(stdin);
	gets(temp_date);
	if (verify_date(temp_date,end_date) == -1) {/* if invalid date entered, default to NOW */
	   /*get_srch_date(time(NULL),end_date);*/
           current_dt_tm = time ( (long *) 0);
           local_date_time(current_dt_tm, end_date, dummy, dummy);
        }

	printf(catgets(REP_catd, 11, 8, "\n%sEnter the ending time as HH:MM or hit <CR> for NOW:"),blanks); 
	fflush(stdout);
	fflush(stdin);
	gets(temp_time);
	if (verify_time(temp_time,end_time) == -1) { /* if invalid time, default to NOW */
	   /*get_srch_time(time(NULL),end_time)*/
           current_dt_tm = time ( (long *) 0);
           local_date_time(current_dt_tm, dummy, end_time, dummy);
        }

	return(1);
}
ch_dt_tminf(start_date,start_time,end_date,end_time)
char *start_date, *start_time;
char *end_date, *end_time;
{
        char *day;
        char *month;
        char *year;
        long current_dt_tm;
        char dummy[40];

	/* read the starting date and time */
        current_dt_tm = time ( (long *) 0);
        local_date_time(current_dt_tm, start_date, dummy, dummy);
	printf(catgets(REP_catd, 11, 1, "\n%sEnter the starting date (i.e., %s) or <CR> for today:"),blanks,start_date); 
	fflush(stdout);
	fflush(stdin);
	gets(start_date);
	if (strlen(start_date)<8)  {
           date_time(start_date,start_time);
           /*current_dt_tm = time ( (long *) 0);
           local_date_time(current_dt_tm, start_date, start_time, dummy);*/
        }
	printf(catgets(REP_catd, 11, 9, "\n%sEnter the starting time as HH:MM or <CR> for first record:"),blanks); 
	fflush(stdout);
	fflush(stdin);
	gets(start_time);
	if(strlen(start_time) < 5) 
		strcpy(start_time,"00:00:00");
	else {		/* fill out the seconds with zeros */
		start_time[5]=':';
		start_time[6]='0';
		start_time[7]='0';
	}

	/* read the ending date and time */
        current_dt_tm = time ( (long *) 0);
        local_date_time(current_dt_tm, end_date, dummy, dummy);
	printf(catgets(REP_catd, 11, 10, "\n%sEnter the ending date (i.e., %s) or <CR> for infinite:"),blanks,end_date); 
	fflush(stdout);
	fflush(stdin);
	gets(end_date);
	if (strlen(end_date)<8) {
		strcpy(end_date,"12/31/99");
		strcpy(end_time,"23:59:59");
	}
	else { 		/* prompt for the ending time */
		printf(catgets(REP_catd, 11, 11, "\n%sEnter the ending time as HH:MM  or <CR> for all records:"),blanks); 
	        fflush(stdout);
		fflush(stdin);
		gets(end_time);
		if (strlen(end_time)<5) strcpy(end_time,"23:59:59");
		else {	/* fill out the seconds with zeros */
			end_time[5]=':';
			end_time[6]='0';
			end_time[7]='0';
		}
	}
	return(1);
}
