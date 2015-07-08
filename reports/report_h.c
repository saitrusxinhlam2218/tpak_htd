static char sz__FILE__[]="@(#)report_h.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  report_h.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:22:10
 * @(#)  Last delta: 12/14/89 at 17:12:18
 * @(#)  SCCS File: /taxi/sccs/s.report_h.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <time.h>
#include "rep_strdefs.h"

extern void local_date_time();
/*====================================================================
= report_heading() -
=	Prints Report Type Heading
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=====================================================================*/
report_heading(sys_dt_tm,start_date,start_time,end_date,end_time,fleetname,title2,pagenumber,fileptr)

long sys_dt_tm;                /* current time in seconds */
char *start_date;
char *start_time;
char *end_date;
char *end_time;
char *fleetname;
char *title2;
int  *pagenumber;
FILE *fileptr;

{

   char sys_date[20];             /* current date, locally formatted */
   char sys_time[20];             /* current time, locally formatted */
   char dummy[40];        

   /* get current date/time & localize */
   local_date_time(sys_dt_tm, sys_date, sys_time, dummy);

   fprintf(fileptr,catgets(REP_catd, 10, 1, "Report for Date: %8s to %8s                  %20s                  Page: %3d   Date Printed: %s\n"),
   start_date,end_date,fleetname,*pagenumber,sys_date);
	(*pagenumber)++;	

   fprintf(fileptr,catgets(REP_catd, 10, 2, "Report Coverage: %8s to %8s      %-55s       Time Printed: %s\n\n"),
   start_time,end_time,title2,sys_time);

   return(0);

}	
