static char sz__FILE__[]="@(#)pi_dt_tm.c	10.1.2.1(ver) Date Release 12/2/94";
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  pi_dt_tm.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:45:50
 * @(#)  Last delta: 12/2/94 at 18:52:35
 * @(#)  SCCS File: /taxi/sccs/s.pi_dt_tm.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include "language.h"

#ifndef WRONG
#define WRONG   (-1)
#endif /* !WRONG */

#define SECS_PER_MIN	60
#define MINS_PER_HOUR	60
#define HOURS_PER_DAY	24
#define DAYS_PER_WEEK	7
#define DAYS_PER_NYEAR	365
#define DAYS_PER_LYEAR	366
#define SECS_PER_HOUR	(SECS_PER_MIN * MINS_PER_HOUR)
#define SECS_PER_DAY	((long) SECS_PER_HOUR * HOURS_PER_DAY)
#define MONS_PER_YEAR	12

#define TM_SUNDAY	0
#define TM_MONDAY	1
#define TM_TUESDAY	2
#define TM_WEDNESDAY	3
#define TM_THURSDAY	4
#define TM_FRIDAY	5
#define TM_SATURDAY	6

#define TM_JANUARY	0
#define TM_FEBRUARY	1
#define TM_MARCH	2
#define TM_APRIL	3
#define TM_MAY		4
#define TM_JUNE		5
#define TM_JULY		6
#define TM_AUGUST	7
#define TM_SEPTEMBER	8
#define TM_OCTOBER	9
#define TM_NOVEMBER	10
#define TM_DECEMBER	11
#define TM_SUNDAY	0

#define TM_YEAR_BASE	1900

#define EPOCH_YEAR	1970
#define EPOCH_WDAY	TM_THURSDAY

time_t timelocal(timeptr)
struct tm *     timeptr;
{
  return mktime(timeptr);
#ifdef FOO
        return timemk(timeptr, localtime, 0L);
#endif
}
/******************************************************************************
*******************************************************************************
   NOTE NOTE NOTE:

   The functions get_arg_time() and get_arg_date() are redundant. But to avoid
   changing the application code, they will remain for now

*******************************************************************************
******************************************************************************/



/*************************************************************************/
/* get_pkup_date: gets todays date, and stores it in the argument string */
/*************************************************************************/
get_pkup_date(datestr)
	char *datestr;				/* for storing todays date */
{
	static char *month_string[] = {"Jan", "Feb", "Mar", "Apr", "May",
					"Jun", "Jul", "Aug", "Sep", "Oct",
					"Nov", "Dec"};

	extern struct tm *localtime();
	struct tm *tm_ptr;			/* pointer to formatted time structure */
	long nowtime;				/* to store systime */
	
	nowtime = time(NULL);
	tm_ptr = localtime(&nowtime);

	if ( tm_ptr != NULL ){  	/* compile the date format, depending upon the compilation option  selected by the site */
	    if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		sprintf(datestr,"%02d%c%02d%c%02d", tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon + 1,
			DATE_SEPERATOR, tm_ptr->tm_year % 100);
	    else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
		sprintf(datestr,"%02d%c%02d%c%02d", tm_ptr->tm_year % 100, DATE_SEPERATOR, tm_ptr->tm_mon + 1,
			DATE_SEPERATOR, tm_ptr->tm_mday);
	    else
		sprintf(datestr,"%02d%c%02d%c%02d", tm_ptr->tm_mon + 1, DATE_SEPERATOR, tm_ptr->tm_mday,
			DATE_SEPERATOR, tm_ptr->tm_year % 100);
	}

	/** Ignore #ifdefs below */

#ifdef STUPIDITY

#ifdef DATE1								/* compile date format = day month_desc year */
		sprintf(datestr,"%02d %s %02d", tm_ptr->tm_mday, month_string[tm_ptr->tm_mon], tm_ptr->tm_year % 100);
#endif

#ifdef DATE2								/* compile date format = day/month_desc/year */
		sprintf(datestr,"%02d/%s/%02d", tm_ptr->tm_mday, month_string[tm_ptr->tm_mon], tm_ptr->tm_year % 100);
#endif

#ifdef DATE3								/* compile date format = day month year */
		sprintf(datestr,"%02d %02d %02d", tm_ptr->tm_mday, tm_ptr->tm_mon+1, tm_ptr->tm_year % 100);
#endif

#ifdef DATE4								/* compile date format = day/month/year */
		sprintf(datestr,"%02d/%02d/%02d", tm_ptr->tm_mday, tm_ptr->tm_mon+1, tm_ptr->tm_year % 100);
#endif

#ifdef DATE5								/* compile date format = month_desc day year */
		sprintf(datestr,"%s %02d %02d", month_string[tm_ptr->mon], tm_ptr->tm_mday, tm_ptr->tm_year % 100);
#endif

#ifdef DATE6								/* compile date format = month_desc day year */
		sprintf(datestr,"%s %02d %02d", month_string[tm_ptr->mon], tm_ptr->tm_mday, tm_ptr->tm_year % 100);
#endif

#ifdef DATE7								/* compile date format = month day year */
		sprintf(datestr,"%02d %02d %02d", tm_ptr->tm_mon+1, tm_ptr->tm_mday, tm_ptr->tm_year % 100);
#endif

#ifdef DATE8								/* compile date format = month/day/year */
		sprintf(datestr,"%02d/%02d/%02d", tm_ptr->tm_mon+1, tm_ptr->tm_mday, tm_ptr->tm_year % 100);
#endif

#ifdef DATE9								/* compile date format = day.month.year */
		sprintf(datestr,"%02d.%02d.%02d", tm_ptr->tm_mday, tm_ptr->tm_mon+1, tm_ptr->tm_year % 100);
#endif

	}
#endif /* STUPIDITY */

}  									/* end get_pkup_date() */

/*************************************************************************/
/* get_srch_date: accepts a "C" date/time and returns a display date 	 */
/*                NOTE: now used only by searches.  Basically the same   */
/*			as get_pkup_date except this routine takes "C"   */
/*			date/time as a parameter 			 */
/*************************************************************************/
get_srch_date(srch_time,datestr)
	long srch_time;				/* "C" formatted date/time */
	char *datestr;				/* for storing todays date */
{
	static char *month_string[] = {"Jan", "Feb", "Mar", "Apr", "May",
					"Jun", "Jul", "Aug", "Sep", "Oct",
					"Nov", "Dec"};

	extern struct tm *localtime();
	struct tm *tm_ptr;			/* pointer to formatted time structure */
	
	tm_ptr = localtime(&srch_time);

	if ( tm_ptr != NULL ){  	/* compile the date format, depending upon the compilation option  selected by the site */
	    if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		sprintf(datestr,"%02d%c%02d%c%02d", tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon + 1,
			DATE_SEPERATOR, tm_ptr->tm_year % 100);
	    else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
		sprintf(datestr,"%02d%c%02d%c%02d", tm_ptr->tm_year % 100, DATE_SEPERATOR, tm_ptr->tm_mon + 1,
			DATE_SEPERATOR, tm_ptr->tm_mday);
	    else
		sprintf(datestr,"%02d%c%02d%c%02d", tm_ptr->tm_mon + 1, DATE_SEPERATOR, tm_ptr->tm_mday,
			DATE_SEPERATOR, tm_ptr->tm_year % 100);
	}

	/** Ignore #ifdefs below */

#ifdef STUPIDITY

#ifdef DATE1								/* compile date format = day month_desc year */
		sprintf(datestr,"%02d %s %02d", tm_ptr->tm_mday, month_string[tm_ptr->tm_mon], tm_ptr->tm_year % 100);
#endif

#ifdef DATE2								/* compile date format = day/month_desc/year */
		sprintf(datestr,"%02d/%s/%02d", tm_ptr->tm_mday, month_string[tm_ptr->tm_mon], tm_ptr->tm_year % 100);
#endif

#ifdef DATE3								/* compile date format = day month year */
		sprintf(datestr,"%02d %02d %02d", tm_ptr->tm_mday, tm_ptr->tm_mon+1, tm_ptr->tm_year % 100);
#endif

#ifdef DATE4								/* compile date format = day/month/year */
		sprintf(datestr,"%02d/%02d/%02d", tm_ptr->tm_mday, tm_ptr->tm_mon+1, tm_ptr->tm_year % 100);
#endif

#ifdef DATE5								/* compile date format = month_desc day year */
		sprintf(datestr,"%s %02d %02d", month_string[tm_ptr->mon], tm_ptr->tm_mday, tm_ptr->tm_year % 100);
#endif

#ifdef DATE6								/* compile date format = month_desc day year */
		sprintf(datestr,"%s %02d %02d", month_string[tm_ptr->mon], tm_ptr->tm_mday, tm_ptr->tm_year % 100);
#endif

#ifdef DATE7								/* compile date format = month day year */
		sprintf(datestr,"%02d %02d %02d", tm_ptr->tm_mon+1, tm_ptr->tm_mday, tm_ptr->tm_year % 100);
#endif

#ifdef DATE8								/* compile date format = month/day/year */
		sprintf(datestr,"%02d/%02d/%02d", tm_ptr->tm_mon+1, tm_ptr->tm_mday, tm_ptr->tm_year % 100);
#endif

#ifdef DATE9								/* compile date format = day.month.year */
		sprintf(datestr,"%02d.%02d.%02d", tm_ptr->tm_mday, tm_ptr->tm_mon+1, tm_ptr->tm_year % 100);
#endif

	}
#endif /* STUPIDITY */

}  									/* end get_srch_date() */

/************************************************************************/
/* get_pkup_time: stores the current time in the argumnet string passed */
/************************************************************************/
get_pkup_time(timestr)
	char *timestr;				/* for storing the time */
{
	
	extern struct tm *localtime();
	struct tm *tm_ptr;			/* pointer to formatted time structure */
	time_t nowtime;				/* to store systime */
	
	nowtime = time(NULL);
	tm_ptr = localtime(&nowtime);

	if ( tm_ptr != NULL )
	  {
	    
	    sprintf(timestr,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
	    
	    
	  }
}  		
	
/************************************************************************/
/* get_srch_time: accepts a "C" date/time and returns a display time    */
/*                NOTE: now used only by searches.  Basically the same  */
/*			as get_pkup_date except this routine takes "C"  */
/*			date/time as a parameter 		        */
/************************************************************************/
get_srch_time(srch_time,timestr)
	long srch_time;				/* "C" formatted date/time */
	char *timestr;				/* for storing the time */
{
	
	extern struct tm *localtime();
	struct tm *tm_ptr;			/* pointer to formatted time structure */
	
	tm_ptr = localtime(&srch_time);

	if ( tm_ptr != NULL )   {
		sprintf(timestr,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);

	}
}  		

get_time()
{
  static long tvec;

  time(&tvec);
  return(tvec);
}


get_asc_time(c_time,cur_dt,cur_tm)
long c_time;
char cur_dt[];							/* date in ascii format */
char cur_tm[];							/* time in ascii format */
{
	get_srch_date(c_time,cur_dt);
	get_srch_time(c_time,cur_tm);
}



short get_short_time(c_time)
long c_time;
{
    char t_date[10];
    char t_time[8];
    char tmp[5];
    short stime;

    get_asc_time(c_time,t_date,t_time);
    tmp[0] = t_time[0];
    tmp[1] = t_time[1];
    tmp[2] = t_time[3];
    tmp[3] = t_time[4];
    tmp[4] = '\0';

    stime = atoi(tmp);
    if ((t_time[5] == 'p') && (stime < 1200))
	stime += 1200;

    return(stime);

}

/*************************************************************************/
/* get_date: gets the date from the time as specified by the 1st argumenu, and stores it in the argument string */
/*************************************************************************/
get_arg_date(nowtime,datestr)
	long nowtime;				/* to store systime */
	char *datestr;				/* for storing todays date */
{
	static char *month_string[] = {"Jan", "Feb", "Mar", "Apr", "May",
					"Jun", "Jul", "Aug", "Sep", "Oct",
					"Nov", "Dec"};

	extern struct tm *localtime();
	struct tm *tm_ptr;			/* pointer to formatted time structure */
	
	tm_ptr = localtime(&nowtime);

	if ( tm_ptr != NULL ){  	/* compile the date format, depending upon the compilation option  selected by the site */
	    if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		sprintf(datestr,"%02d%c%02d%c%02d", tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon + 1,
			DATE_SEPERATOR, tm_ptr->tm_year % 100);
	    else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
		sprintf(datestr,"%02d%c%02d%c%02d", tm_ptr->tm_year % 100, DATE_SEPERATOR, tm_ptr->tm_mon + 1,
			DATE_SEPERATOR, tm_ptr->tm_mday);
	    else
		sprintf(datestr,"%02d%c%02d%c%02d", tm_ptr->tm_mon + 1, DATE_SEPERATOR, tm_ptr->tm_mday,
			DATE_SEPERATOR, tm_ptr->tm_year % 100);
	}

	/** Ignore #ifdefs below */

#ifdef STUPIDITY

#ifdef DATE1								/* compile date format = day month_desc year */
		sprintf(datestr,"%02d %s %02d", tm_ptr->tm_mday, month_string[tm_ptr->tm_mon], tm_ptr->tm_year % 100);
#endif

#ifdef DATE2								/* compile date format = day/month_desc/year */
		sprintf(datestr,"%02d/%s/%02d", tm_ptr->tm_mday, month_string[tm_ptr->tm_mon], tm_ptr->tm_year % 100);
#endif

#ifdef DATE3								/* compile date format = day month year */
		sprintf(datestr,"%02d %02d %02d", tm_ptr->tm_mday, tm_ptr->tm_mon+1, tm_ptr->tm_year % 100);
#endif

#ifdef DATE4								/* compile date format = day/month/year */
		sprintf(datestr,"%02d/%02d/%02d", tm_ptr->tm_mday, tm_ptr->tm_mon+1, tm_ptr->tm_year % 100);
#endif

#ifdef DATE5								/* compile date format = month_desc day year */
		sprintf(datestr,"%s %02d %02d", month_string[tm_ptr->mon], tm_ptr->tm_mday, tm_ptr->tm_year % 100);
#endif

#ifdef DATE6								/* compile date format = month_desc day year */
		sprintf(datestr,"%s %02d %02d", month_string[tm_ptr->mon], tm_ptr->tm_mday, tm_ptr->tm_year % 100);
#endif

#ifdef DATE7								/* compile date format = month day year */
		sprintf(datestr,"%02d %02d %02d", tm_ptr->tm_mon+1, tm_ptr->tm_mday, tm_ptr->tm_year % 100);
#endif

#ifdef DATE8								/* compile date format = month/day/year */
		sprintf(datestr,"%02d/%02d/%02d", tm_ptr->tm_mon+1, tm_ptr->tm_mday, tm_ptr->tm_year % 100);
#endif

#ifdef DATE9								/* compile date format = day.month.year */
		sprintf(datestr,"%02d.%02d.%02d", tm_ptr->tm_mday, tm_ptr->tm_mon+1, tm_ptr->tm_year % 100);
#endif

	}
#endif /* STUPIDITY */

}  									/* end get_date() */


/************************************************************************/
/* get_time: stores the current time in the argumnet string passed */
/************************************************************************/
get_arg_time(nowtime,timestr)
	long nowtime;				/* to store systime */
	char *timestr;				/* for storing the time */
{
	
	extern struct tm *localtime();
	struct tm *tm_ptr;			/* pointer to formatted time structure */
	
	tm_ptr = localtime(&nowtime);

	if ( tm_ptr != NULL )   {

		sprintf(timestr,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);



	}  										/* end if .. tm_ptr != NULL */
}  											/* end get_pkup_time() */
	

/************************************************************************/
/* get_time_from_short: stores the current time in the argumnet string passed */
/************************************************************************/
get_time_from_short(shorttime,timestr)
	short shorttime;
	char *timestr;				/* for storing the time */
{
	
	int hour, min;

	hour = shorttime/100;
	min = shorttime % 100;


	sprintf(timestr,"%02d:%02d",hour,min);



}  											/* end get_pkup_time() */
	

/********************************************************************/
/* NAME                                                             */
/*      tm_to_time - convert broken-down time into time_t           */
/*                                                                  */
/* SYNOPSIS                                                         */
/*      #include <sys/types.h>                                      */
/*      #include <sys/time.h>                                       */
/*                                                                  */
/*      time_t tm_to_time (tp);                                     */
/*      struct tm *tp;                                              */
/*                                                                  */
/* DESCRIPTION                                                      */
/*      Tm_to_time converts a broken-down time pointed to by tp     */
/*      (such as returned by localtime) into a time_t (that is, a   */
/*      number of seconds since 00:00:00 GMT, Jan 1, 1970).  Thus,  */
/*      if tm_to_time is called with a broken-down time returned by */
/*      a call to localtime with some time_t argument, the value    */
/*      obtained by tm_to_time is identical to the original time_t. */
/********************************************************************/

/* Return 1 if `y' is a leap year, 0 otherwise.
 */

static int leap (y) int y; {
    y += 1900;
    if (y % 400 == 0)
	return (1);
    if (y % 100 == 0)
	return (0);
    if (y == 100)
        return (0);
    return (y % 4 == 0);
}

/* Return the number of days between Jan 1, 1970 and the given
 * broken-down time.
 */

static int ndays (p) struct tm *p; {
    register n = p->tm_mday;
    register m, y;
    register char *md = "\37\34\37\36\37\36\37\37\36\37\36\37";

    for (y = 70; y < p->tm_year; ++y) {
	n += 365;
	if (leap (y)) ++n;
    }
    for (m = 0; m < p->tm_mon; ++m)
	n += md[m] + (m == 1 && leap (y));
    return (n);
}

/* Convert a broken-down time (such as returned by localtime())
 * back into a `time_t'.
 */

time_t tm_to_time (tp) struct tm *tp; {
    register int m1, m2;
    time_t t;
    struct tm otm;

    t = (ndays (tp) - 1) * 86400L + tp->tm_hour * 3600L
	+ tp->tm_min * 60 + tp->tm_sec;
    /*
     * Now the hard part -- correct for the time zone:
     */
    otm = *tp;
    tp = localtime (&t);
    m1 = tp->tm_hour * 60 + tp->tm_min;
    m2 = otm.tm_hour * 60 + otm.tm_min;
    t -= ((m1 - m2 + 720 + 1440) % 1440 - 720) * 60L;
    return (t);
}

/* ----------------------------------------------------------------------------
/* FUNCTION: GET_LOCAL_TIME
/*
/* DESCRIPTION: Gets the C-formatted time/date.
/*
------------------------------------------------------------------------------ */

time_t get_local_time()
  {
    long	calender_time;
    struct tm	*tm_time;
    char	time_string[13];

    calender_time = time(NULL);
    tm_time = localtime(&calender_time);

    return(timelocal(tm_time));
  }

static time_t timemk(timeptr, funcp, offset)
struct tm *     timeptr;
struct tm * (*  funcp)();
long            offset;
{
        register int    direction;
        register int    bits;
        time_t          t;
        struct tm       yourtm, mytm;

        yourtm = *timeptr;
        /*
        ** Correct the tm supplied, in case some of its values are
        ** out of range.
        */
        while (yourtm.tm_sec >= SECS_PER_MIN)
                ++yourtm.tm_min, yourtm.tm_sec -= SECS_PER_MIN;
        while (yourtm.tm_sec < 0)
                --yourtm.tm_min, yourtm.tm_sec += SECS_PER_MIN;
        while (yourtm.tm_min >= MINS_PER_HOUR)
                ++yourtm.tm_hour, yourtm.tm_min -= MINS_PER_HOUR;
        while (yourtm.tm_min < 0)
                --yourtm.tm_hour, yourtm.tm_min += MINS_PER_HOUR;
        while (yourtm.tm_hour >= HOURS_PER_DAY)
                ++yourtm.tm_mday, yourtm.tm_hour -= HOURS_PER_DAY;
        while (yourtm.tm_hour < 0)
                --yourtm.tm_mday, yourtm.tm_hour += HOURS_PER_DAY;
        while (yourtm.tm_mday > 31)             /* trust me [kre] */
                ++yourtm.tm_mon, yourtm.tm_mday -= 31;
        while (yourtm.tm_mday <= 0)
                --yourtm.tm_mon, yourtm.tm_mday += 31;
        while (yourtm.tm_mon >= MONS_PER_YEAR)
                ++yourtm.tm_year, yourtm.tm_mon -= MONS_PER_YEAR;
        while (yourtm.tm_mon < 0)
                --yourtm.tm_year, yourtm.tm_mon += MONS_PER_YEAR;
        /*
        ** Calcluate the number of magnitude bits in a time_t
        ** (this works regardless of whether time_t is
        ** signed or unsigned, though lint complains if unsigned).
        */
        for (bits = 0, t = 1; t > 0; ++bits, t <<= 1)
                ;
        /*
        ** If time_t is signed, then 0 is the median value,
        ** if time_t is unsigned, then 1 << bits is median.
        */
        t = (t < 0) ? 0 : ((time_t) 1 << bits);
        for ( ; ; ) {
                mytm = *((*funcp)(&t));
                if ((direction = (mytm.tm_year - yourtm.tm_year)) == 0 &&
                        (direction = (mytm.tm_mon - yourtm.tm_mon)) == 0 &&
                        (direction = (mytm.tm_mday - yourtm.tm_mday)) == 0 &&
                        (direction = (mytm.tm_hour - yourtm.tm_hour)) == 0 &&
                        (direction = (mytm.tm_min - yourtm.tm_min)) == 0)
                                direction = mytm.tm_sec - yourtm.tm_sec;
                if (direction == 0) {
                        *timeptr = mytm;
                        return t;
                }
                if (bits-- < 0) {
                        *timeptr = yourtm;      /* restore "original" value */
                        if (yourtm.tm_mday == 31) {
                                timeptr->tm_mday = 1;
                                ++(timeptr->tm_mon);
                                t = timemk(timeptr, funcp, offset);
                                if (t != WRONG)
                                        return t;
                                *timeptr = yourtm;
                        } else if (yourtm.tm_mon == TM_FEBRUARY &&
                                yourtm.tm_mday > 28) {
                                        timeptr->tm_mday -= 28;
                                        ++(timeptr->tm_mon);
                                        t = timemk(timeptr, funcp, offset);
                                        if (t != WRONG)
                                                return t;
                                        *timeptr = yourtm;
                        }
                        return WRONG;
                }
                if (bits < 0)
                        --t;
                else if (direction > 0)
                        t -= (time_t) 1 << bits;
                else    t += (time_t) 1 << bits;
        }
}



/* ----------------------------------------------------------------------------
/* FUNCTION: PI_to_TP_time
/*
/* DESCRIPTION: Convert PI-time (HHMM) into TaxiPak-time (HH:MM).
/*
------------------------------------------------------------------------------ */

PI_to_TP_time( char *pi_time, char *tp_time )
  {
    tp_time[ 0 ] = pi_time[ 0 ];
    tp_time[ 1 ] = pi_time[ 1 ];
    tp_time[ 2 ] = ':';
    tp_time[ 3 ] = pi_time[ 2 ];
    tp_time[ 4 ] = pi_time[ 3 ];
    tp_time[ 5 ] = '\0';
  }

/* ----------------------------------------------------------------------------
/* FUNCTION: TP_to_PI_time
/*
/* DESCRIPTION: Convert TaxiPak-time (HH:MM) into PI-time (HHMM).
/*
------------------------------------------------------------------------------ */

TP_to_PI_time( char *tp_time, char *pi_time )
  {
    pi_time[ 0 ] = tp_time[ 0 ];
    pi_time[ 1 ] = tp_time[ 1 ];
    pi_time[ 2 ] = tp_time[ 3 ];
    pi_time[ 3 ] = tp_time[ 4 ];
    pi_time[ 4 ] = '\0';
  }

/* ----------------------------------------------------------------------------
/* FUNCTION: PI_to_TP_date
/*
/* DESCRIPTION: Convert PI-date (YYMMDD) into TaxiPak-date (YY-MM-DD).
/*
------------------------------------------------------------------------------ */

PI_to_TP_date( char *pi_date, char *tp_date )
  {
    tp_date[ 0 ] = pi_date[ 0 ];
    tp_date[ 1 ] = pi_date[ 1 ];
    tp_date[ 2 ] = '.';
    tp_date[ 3 ] = pi_date[ 2 ];
    tp_date[ 4 ] = pi_date[ 3 ];
    tp_date[ 5 ] = '.';
    tp_date[ 6 ] = pi_date[ 4 ];
    tp_date[ 7 ] = pi_date[ 5 ];
    tp_date[ 8 ] = '\0';
  }

/* ----------------------------------------------------------------------------
/* FUNCTION: TP_to_PI_date
/*
/* DESCRIPTION: Convert TaxiPak-date (YY-MM-DD) into PI-date (YYMMDD).
/*
------------------------------------------------------------------------------ */

TP_to_PI_date( char *tp_date, char *pi_date )
{
  pi_date[ 0 ] = tp_date[ 0 ];
  pi_date[ 1 ] = tp_date[ 1 ];
  pi_date[ 2 ] = tp_date[ 3 ];
  pi_date[ 3 ] = tp_date[ 4 ];
  pi_date[ 4 ] = tp_date[ 6 ];
  pi_date[ 5 ] = tp_date[ 7 ];
  pi_date[ 6 ] = '\0';
}

get_tm_from_date_time( char *date_copy, char *time_copy )
{
  
  char  date_time[13];
  struct tm *tm_ptr;
  time_t cDateTime;
  
  if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
    {
      date_time[0] = date_copy[0];
      date_time[1] = date_copy[1];
      date_time[2] = date_copy[2];
      date_time[3] = date_copy[3];
      date_time[4] = date_copy[4];
      date_time[5] = date_copy[5];
    }
  else
    {
      date_time[0] = date_copy[6]; 			/* copy year from date */
      date_time[1] = date_copy[7];
      
      if (DATE_FORMAT == DAY_FIRST_IN_DATE)
	{
	  date_time[4] = date_copy[0]; 			/* copy the day */
	  date_time[5] = date_copy[1];
	  
	  date_time[2] = date_copy[3]; 			/* copy month from date */
	  date_time[3] = date_copy[4];
	}
      else
	{
	  date_time[2] = date_copy[0]; 			/* copy month from date */
	  date_time[3] = date_copy[1];
	  
	  date_time[4] = date_copy[3]; 			/* copy the day */
	  date_time[5] = date_copy[4];
	  
	}
    }
  date_time[6] = time_copy[0];
  date_time[7] = time_copy[1];
  date_time[8] = time_copy[2];
  date_time[9] = time_copy[3];
  
  date_time[10] = '0'; 				/* copy sec's to be 0 secs */
  date_time[11] = '0';
  date_time[12] = '\0';

  cDateTime = conversion( date_time );
  return( cDateTime );
}

get_alt_tm_from_date_time( char *date_copy, char *time_copy )
{
  
  char  date_time[13];
  struct tm *tm_ptr;
  time_t cDateTime;
  

  date_time[0] = date_copy[4];
  date_time[1] = date_copy[5];
  date_time[2] = date_copy[2];
  date_time[3] = date_copy[3];
  date_time[4] = date_copy[0];
  date_time[5] = date_copy[1];
  
  date_time[6] = time_copy[0];
  date_time[7] = time_copy[1];
  date_time[8] = time_copy[2];
  date_time[9] = time_copy[3];
  
  date_time[10] = '0'; 				/* copy sec's to be 0 secs */
  date_time[11] = '0';
  date_time[12] = '\0';

  cDateTime = conversion( date_time );
  return( cDateTime );
}
