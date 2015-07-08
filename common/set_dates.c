static char sz__FILE__[]="@(#)set_dates.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  set_dates.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:33:17
 * @(#)  Last delta: 12/2/94 at 11:42:24
 * @(#)  SCCS File: /taxi/sccs/s.set_dates.c
 *                                                                           *
 *****************************************************************************/

#include <time.h>

#define SECONDSPERDAY 86400

/*====================================================================
= set_dates() -
=	Returns the starting and ending dates and times 
=====================================================================*/
set_dates(sys_day_end, sys_prod_start_time, start_date, start_time, start_dt_tm, end_date, end_time, ending_dt_tm)
int sys_day_end, sys_prod_start_time;
char start_date[10];
char start_time[10];
char end_date[10];
char end_time[10];
time_t *start_dt_tm, *ending_dt_tm;		/* report start and stop time stamps */
{
	char dt_tm_buff[13];
	struct tm *tt;

	/* get the current time and date. format the time hh:mm:ss */
	date_time(end_date,end_time);

	/* get the production time in ascii. */
	/* This must be the current day and the ending day of the shift. */
	sprintf(dt_tm_buff,"%04d",sys_day_end);
	/* adjust the ending time */
	end_time[0]=dt_tm_buff[0];	/* the production hour */
	end_time[1]=dt_tm_buff[1];
	end_time[3]=dt_tm_buff[2];	/* the production minutes */
	end_time[4]=dt_tm_buff[3];
	end_time[6]='0';		/* round seconds to zero */
	end_time[7]='0';
	end_time[8]='\0';
	/* compact the date and time into a buffer */
	map_dt_tm(end_date,end_time,dt_tm_buff);
	/* map the buffer into a time stamp integer */
	*ending_dt_tm = convert_dt_tm(dt_tm_buff);

	/* compute the real ending time */
	if (sys_prod_start_time < sys_day_end)  *ending_dt_tm -= SECONDSPERDAY;
	*start_dt_tm  = *ending_dt_tm - SECONDSPERDAY;	/* shift starting time is one day (in seconds) earlier */

	/* this release produces reports up to the last shift */
	/* -----|----- reporting for shift ------|----- previous 24 hour shift -----|----- time now	*/
	/* ^^^^^^^^^^^^purging interval^^^^^^^^^^|<<<<<<<reporting interval>>>>>>>>>     		*/
	/*                                       ^start_dt_tm                        ^ending_dt_tm	*/

	/* use localtime to retrieve the ascii dates and times */
	tt = localtime(ending_dt_tm);
	sprintf(end_date,"%02d/%02d/%02d",tt->tm_mon+1,tt->tm_mday,tt->tm_year);
	sprintf(dt_tm_buff,"%04d",sys_day_end);

	/* store the ending time */
	end_time[0]=dt_tm_buff[0];	/* the production hour */
	end_time[1]=dt_tm_buff[1];
	end_time[3]=dt_tm_buff[2];	/* the production minutes */
	end_time[4]=dt_tm_buff[3];
	end_time[6]='0';		/* round seconds to zero */
	end_time[7]='0';
	end_time[8]='\0';

	/* compute and store the starting time */
	tt = localtime(start_dt_tm);
	sprintf(start_date,"%02d/%02d/%02d",tt->tm_mon+1,tt->tm_mday,tt->tm_year);
	strcpy(start_time,end_time);
	return;
}
