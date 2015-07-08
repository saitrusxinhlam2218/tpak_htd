static char sz__FILE__[]="@(#)dt_tm.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  dt_tm.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:32:45
 * @(#)  Last delta: 12/2/94 at 11:35:23
 * @(#)  SCCS File: /taxi/sccs/s.dt_tm.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include "taxipak.h"
#include "language.h"


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
	long nowtime;				/* to store systime */
	
	nowtime = time(NULL);
	tm_ptr = localtime(&nowtime);

	if ( tm_ptr != NULL )   {
#ifdef HR24	 									/* compile in the 24hr time format */
		sprintf(timestr,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#endif

#ifdef HR12	 									/* compile in the 12hr time format */
		if ( tm_ptr->tm_hour >= 12){						/* time is pm */
			if(tm_ptr->tm_hour > 12)
				tm_ptr->tm_hour -= 12;

			sprintf(timestr,"%02d:%02dp",tm_ptr->tm_hour,tm_ptr->tm_min);
		}
		else{ 									/* time is am */
			if(tm_ptr->tm_hour == 0)
				tm_ptr->tm_hour = 12;

			sprintf(timestr,"%02d:%02da",tm_ptr->tm_hour,tm_ptr->tm_min);
		}
#endif
	}  										/* end if .. tm_ptr != NULL */
}  											/* end get_pkup_time() */
	
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
#ifdef HR24	 									/* compile in the 24hr time format */
		sprintf(timestr,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#endif

#ifdef HR12	 									/* compile in the 12hr time format */
		if ( tm_ptr->tm_hour >= 12){						/* time is pm */
			if(tm_ptr->tm_hour > 12)
				tm_ptr->tm_hour -= 12;

			sprintf(timestr,"%02d:%02dp",tm_ptr->tm_hour,tm_ptr->tm_min);
		}
		else{ 									/* time is am */
			if(tm_ptr->tm_hour == 0)
				tm_ptr->tm_hour = 12;

			sprintf(timestr,"%02d:%02da",tm_ptr->tm_hour,tm_ptr->tm_min);
		}
#endif
	}  										/* end if .. tm_ptr != NULL */
}  											/* end get_srch_time() */


get_hhmmss_time(srch_time,timestr)
	time_t srch_time;				/* "C" formatted date/time */
	char *timestr;				/* for storing the time */
{
	
	extern struct tm *localtime();
	struct tm *tm_ptr;			/* pointer to formatted time structure */
	
	tm_ptr = localtime(&srch_time);

	if ( tm_ptr != NULL )   {
#ifdef HR24	 									/* compile in the 24hr time format */
		sprintf(timestr,"%02d:%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min, tm_ptr->tm_sec);
#endif

#ifdef HR12	 									/* compile in the 12hr time format */
		if ( tm_ptr->tm_hour >= 12){						/* time is pm */
			if(tm_ptr->tm_hour > 12)
				tm_ptr->tm_hour -= 12;

			sprintf(timestr,"%02d:%02d:%02dp",tm_ptr->tm_hour,tm_ptr->tm_min,tm_ptr->tm_sec);
		}
		else{ 									/* time is am */
			if(tm_ptr->tm_hour == 0)
				tm_ptr->tm_hour = 12;

			sprintf(timestr,"%02d:%02d:%02da",tm_ptr->tm_hour,tm_ptr->tm_min,tm_ptr->tm_sec);
		}
#endif
	}  										/* end if .. tm_ptr != NULL */
}  		

get_time()
{
  time_t tvec;

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
#ifdef HR24	 									/* compile in the 24hr time format */
		sprintf(timestr,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#endif

#ifdef HR12	 									/* compile in the 12hr time format */
		if ( tm_ptr->tm_hour >= 12){						/* time is pm */
			if(tm_ptr->tm_hour > 12)
				tm_ptr->tm_hour -= 12;

			sprintf(timestr,"%02d:%02dp",tm_ptr->tm_hour,tm_ptr->tm_min);
		}
		else{ 									/* time is am */
			if(tm_ptr->tm_hour == 0)
				tm_ptr->tm_hour = 12;

			sprintf(timestr,"%02d:%02da",tm_ptr->tm_hour,tm_ptr->tm_min);
		}
#endif
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

#ifdef HR24	 									/* compile in the 24hr time format */
		sprintf(timestr,"%02d:%02d",hour,min);
#endif

#ifdef HR12	 									/* compile in the 12hr time format */
		if ( hour >= 12){						/* time is pm */
			if(hour > 12)
				hour -= 12;

			sprintf(timestr,"%02d:%02dp",hour,min);
		}
		else{ 									/* time is am */
			if(hour == 0)
				hour = 12;

			sprintf(timestr,"%02d:%02da",hour,min);
		}
#endif

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

/** Takes a time value in format "23:14" and converts it to **/
/** numeric value 2314 for comparisons                      **/
int
get_numeric_time( char *pNumberTime, int *iNumberTime )
{
  int i;
  char *pTmp;
  
  /** Validate incoming number **/
  pTmp = pNumberTime;
  
  for ( i = 0; i < 2; i++ )
    {
      if ( !isdigit( *pTmp ) )
	return( -1 );
      ++pTmp;
    }

  if ( *pTmp != ':' )
    return( -1 );
  else
    ++pTmp;

  for ( i = 3; i < 5; i++ )
    {
      if ( !isdigit( *pTmp ) )
	return( -1 );
      ++pTmp;
    }

  /** Have a good time value **/
  pTmp = pNumberTime + 3;
  *( pTmp + 2 ) = '\0';
  *( pNumberTime + 2 ) = '\0';

  *iNumberTime = 100 * atoi( pNumberTime ) + atoi( pTmp );
  *( pNumberTime + 2 ) = ':';
  return( 0 );
}

int
get_day_of_week( char *date_copy )
{
  char  date_time[13];
  struct tm *tm_ptr;
  time_t cDateTime;
  
  if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
    {
      date_time[0] = date_copy[0];
      date_time[1] = date_copy[1];
      date_time[2] = date_copy[3];
      date_time[3] = date_copy[4];
      date_time[4] = date_copy[6];
      date_time[5] = date_copy[7];
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
  date_time[6] = '0';
  date_time[7] = '0';
  date_time[8] = '0';
  date_time[9] = '1';
  
  date_time[10] = '0'; 				/* copy sec's to be 0 secs */
  date_time[11] = '0';
  date_time[12] = '\0';

  cDateTime = conversion( date_time );
  tm_ptr = localtime( &cDateTime );

  return( tm_ptr->tm_wday );
}

get_tm_from_date( char *date_copy )
{
  
  char  date_time[13];
  struct tm *tm_ptr;
  time_t cDateTime;
  
  if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
    {
      date_time[0] = date_copy[0];
      date_time[1] = date_copy[1];
      date_time[2] = date_copy[3];
      date_time[3] = date_copy[4];
      date_time[4] = date_copy[6];
      date_time[5] = date_copy[7];
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
  date_time[6] = '0';
  date_time[7] = '0';
  date_time[8] = '0';
  date_time[9] = '1';
  
  date_time[10] = '0'; 				/* copy sec's to be 0 secs */
  date_time[11] = '0';
  date_time[12] = '\0';

  cDateTime = conversion( date_time );
  return( cDateTime );
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
      date_time[2] = date_copy[3];
      date_time[3] = date_copy[4];
      date_time[4] = date_copy[6];
      date_time[5] = date_copy[7];
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
  date_time[8] = time_copy[3];
  date_time[9] = time_copy[4];
  
  date_time[10] = '0'; 				/* copy sec's to be 0 secs */
  date_time[11] = '0';
  date_time[12] = '\0';

  cDateTime = conversion( date_time );
  return( cDateTime );
}

int
time_2_time_of_day(time_t time, int * tod)
{
   struct tm *moment;

   moment = localtime(&time);
   *tod = moment->tm_hour * 100 +
         moment->tm_min;
   return(0);
}                       /* end_time time_2_time_of_day */


get_text_from_date_time(time_t date_time, char *date_str, char *time_str)
{
  if (date_str)
    get_srch_date(date_time, date_str);

  if (time_str)
    get_srch_time(date_time, time_str);
}

get_date_time_from_text(time_t *date_time, char *date_str, char *time_str)
{
  char yymmddhhmmss[13];	/* date & time formatted in YYMMDDhhmmss format */
  char scratch_ptr[3];			/* scratch area */
  int hours=0;
  time_t tmp_date_time;
  
  if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
  {
    yymmddhhmmss[0] = date_str[0];
    yymmddhhmmss[1] = date_str[1];
    yymmddhhmmss[2] = date_str[3];
    yymmddhhmmss[3] = date_str[4];
    yymmddhhmmss[4] = date_str[6];
    yymmddhhmmss[5] = date_str[7];
  }
  
  else
  {
    /* Year */
    yymmddhhmmss[0] = date_str[6];
    yymmddhhmmss[1] = date_str[7];
    
    if (DATE_FORMAT == DAY_FIRST_IN_DATE)
    {
      /* Day */
      yymmddhhmmss[4] = date_str[0];
      yymmddhhmmss[5] = date_str[1];
      
      /* Month */
      yymmddhhmmss[2] = date_str[3];
      yymmddhhmmss[3] = date_str[4];
    }
    else {
      /* Month */
      yymmddhhmmss[2] = date_str[0];
      yymmddhhmmss[3] = date_str[1];
      
      /* Day */
      yymmddhhmmss[4] = date_str[3];
      yymmddhhmmss[5] = date_str[4];
    }
  }
  
  /* Hour */
  yymmddhhmmss[6] = time_str[0];
  yymmddhhmmss[7] = time_str[1];
  
#ifdef HR12
  strncpy(scratch_ptr,time_str,2);			/* extract just the hours */
  scratch_ptr[2] = '\0';					/* null terminate it */
  hours = atoi(scratch_ptr);				/* convert to integer hours */
  if (time_str[5] == 'p' || time_str[5] == 'P'){	/* PM time; take care of 1:00p - 11:59p */
    if(hours < 12)
      hours += 12;
  }
  else							/* AM time; take care of 12:01a - 12:59a */
    if(hours == 12)
      hours -= 12;
  sprintf(scratch_ptr, "%02d", hours);			/* convert back to ascii format */
  yymmddhhmmss[6] = scratch_ptr[0];
  yymmddhhmmss[7] = scratch_ptr[1];
#endif
  
  /* Minute */
  yymmddhhmmss[8] = time_str[3];
  yymmddhhmmss[9] = time_str[4];
  
  /* Seconds set to 0 */
  yymmddhhmmss[10] = '0';
  yymmddhhmmss[11] = '0';
  yymmddhhmmss[12] = EOS;
  
  tmp_date_time = conversion(yymmddhhmmss);
  if (tmp_date_time == -1)
    return(FAIL);

  *date_time = tmp_date_time;
  return(SUCCESS);
}
