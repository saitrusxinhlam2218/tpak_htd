static char sz__FILE__[]="@(#)date_time.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  date_time.c; Rel: 10.1.0.0; Get date: 4/18/95 at 17:01:24
* @(#)  Last delta: 12/2/94 at 11:26:25
* @(#)  SCCS File: /taxi/sccs/s.date_time.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

#include <stdio.h>
#include <time.h>
#include "rep_strdefs.h"

/*
   user_defined_date_time()
*/
int user_defined_date_time( current_date, current_time )
     char *current_date;
     char *current_time;
{
  	long dateandtime = time ( (long *) 0);
	struct tm *tm_buf;

	/* Print messages from the date and time at was scheduled */
	tm_buf=localtime(&dateandtime);	/* map the long integer onto a time struct */

	/* prepare the time as hh:mm:ss */
	sprintf(current_time,"%02d:%02d:%02d",tm_buf->tm_hour,tm_buf->tm_min,tm_buf->tm_sec);
	return(1);
} /* end date_time */


/*====================================================================
= date_time() -
=	Return the current date and time.
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=====================================================================*/
int date_time(current_date,current_time)
char *current_date;
char *current_time;
{
	long dateandtime = time ( (long *) 0);
	struct tm *tm_buf;

	/* Print messages from the date and time at was scheduled */
	tm_buf=localtime(&dateandtime);	/* map the long integer onto a time struct */
	/* prepare the date as mm/dd/yy */
	sprintf(current_date,"%02d/%02d/%02d",tm_buf->tm_mon+1,tm_buf->tm_mday,tm_buf->tm_year % 100);
	/* prepare the time as hh:mm:ss */
	sprintf(current_time,"%02d:%02d:%02d",tm_buf->tm_hour,tm_buf->tm_min,tm_buf->tm_sec);
	return(1);
} /* end date_time */
