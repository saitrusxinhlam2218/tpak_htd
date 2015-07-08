/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: cnvdttm.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2001/03/12 16:41:08 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/common/cnvdttm.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

static char rcsid[] = "$Id: cnvdttm.c,v 1.2 2001/03/12 16:41:08 taxiadm Exp $";

#include <stdio.h>
#include <time.h>
#include "literals.h"
#define DECLARING
#include "rep_strdefs.h"

/******************************************************************************
* convert YYMMDDhhmmss to a long.
******************************************************************************/

long convert_dt_tm(date_time) 
char date_time[13];
{
	long	tm;
	int	i,n,days,tzone;
	char	s[3], *getenv(), *tz;
	short	isLeapyear;
	extern	struct	tm *Localtime();
	char    *temp_ptr;
	char    temp_str[3];
	char    temp_str2[3];
	int	nn;

	if (strlen(date_time) != 12) {
		printf(catgets(REP_catd, 3, 1, "INVALID_FORMAT for date_time\n"));
		sleep(3);
		return(0);
	}
	for(i=0; i<12; i++)
		if (date_time[i] < '0' || date_time[i] > '9')	{
			printf(catgets(REP_catd, 3, 2, "NON_NUMERIC character in date_time\n"));
			sleep(3);
			return(0);
		}

	if ((tz=getenv("TZ")) == NULL)
		tzone = 5;	/* Eatern Time Zone */
	else
		tzone = atoi(tz);
	s[2] = '\0';

	for (i=0; i<12; i+=2)	{
		s[0] = date_time[i];
		s[1] = date_time[i+1];
		n = atoi(s);

		switch(i)	{
		case 0:	/* YY:	year */
			isLeapyear = n != 0 && n % 4 == 0; /* 2000 isn't */
			if (n<70)
				n+=100; /* years after 2000 */
			days = (n-70L) * 365L;
			days += ((n-69L) / 4); /* previous year's leap days */
			continue;
		case 2:	/* MM: month */
			switch (n)	{
			case 12:
				days += 30;	/* Nov */
			case 11:
				days += 31;	/* Oct */
			case 10:
				days += 30;	/* Sep */
			case 9:
				days += 31;	/* Aug */
			case 8:
				days += 31;	/* Jul */
			case 7:
				days += 30;	/* Jun */
			case 6:
				days += 31;	/* May */
			case 5:
				days += 30;	/* Apr */
			case 4:
				days += 31;	/* Mar */
			case 3:
				days += (isLeapyear ? 29 : 28); /* Feb */
			case 2:
				days += 31;	/* Jan */
			case 1:
				break;
			default:
				if(n < 0)  {
					printf(catgets(REP_catd, 3, 3, "INVALID_MONTH in date_time\n"));
					sleep(3);
				}
				return(0);
			}
			continue;
		case 4: /* DD:day */
			if (n>31)	{
				printf(catgets(REP_catd, 3, 4, "INVALID_DAY in date_time\n"));
				sleep(3);
				return(0);
			}
			tm = (days+n-1) * 24L * 60L * 60L;
			continue;
		case 6: /* hh:hour */
			if (n>23)	{
				printf(catgets(REP_catd, 3, 5, "INVALID_HOUR in date_time\n"));
				sleep(3);
				return(0);
			}
			n += tzone; /* correct for time zone */
			tm += n * 60L * 60L;
			continue;
		case 8: /* mm:minute */
			if (n>59)	{
				printf(catgets(REP_catd, 3, 6, "INVALID_MIN in date_time\n"));
				sleep(3);
				return(0);
			}
			tm += n * 60L;
			continue;
		case 10: /* ss:second */
			if (n>59)	{
				printf(catgets(REP_catd, 3, 7, "INVALID_SEC in date_time\n"));
				sleep(3);
				return(0);
			}
			tm += n;
		}
		if (localtime(&tm)->tm_isdst)
		tm -= 60L * 60L; /* adjust for daylight saving time */
		return(tm);
	}
} /* end of convert_dt_tm */
