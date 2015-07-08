/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: date_time.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2001/03/12 16:41:10 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/common/date_time.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

static char rcsid[] = "$Id: date_time.c,v 1.2 2001/03/12 16:41:10 taxiadm Exp $";


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
