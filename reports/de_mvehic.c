static char sz__FILE__[]="@(#)de_mvehic.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  de_mvehic.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:21:45
 * @(#)  Last delta: 12/14/89 at 17:01:47
 * @(#)  SCCS File: /taxi/sccs/s.de_mvehic.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <isam.h>
#include <sys/types.h>
#include "machdep.h"
#include "mads_types.h"
#include "key.h"
#include "ui_def.h"
#include "path.h"
#define DECLARING
#include "comm_strdefs.h"
#include "rep_strdefs.h"
#include "language.h"

extern  void local_date_time();

char sysbuf[120];
/*====================================================================
= main()
=	Front end to the manifest process
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
	int driver_id;
	long get_internal_time(),choose_driver();
	char *getenv();
	char *mads_spooler;
	short fleet_num[9];
	char mbuf[20];
	char dummy[40];

        /* initialize language environment */
        init_locale();
        REP_catd = init_catalog("de_mvehic");
        COMMON_m_catd = init_catalog("common");

	mads_spooler = mbuf;
	if ((mads_spooler = getenv("MADS_SPOOLER")) == NULL) {		/* get the environment variable, MADS_SPOOLER */
		mads_spooler = PRINT_CMD;			/* use default value */
	}

	CLEAR;

	goodfleets = choose_fleets(fleet_id,fleet_name,1,fleet_num);

	if (goodfleets) {			/* prompt for the starting and ending dates and times */

		driver_id = (int)(choose_driver());

		choose3_dt_tm(start_date,start_time,end_date,end_time);

		start_dt_tm = get_internal_time(start_date,start_time);

		ending_dt_tm = get_internal_time(end_date,end_time);
		for(indx=0;indx<goodfleets;indx++)
		{

                local_date_time(start_dt_tm, start_date, start_time, dummy);
                local_date_time(ending_dt_tm, end_date, end_time, dummy);

		sprintf(systembuff,"%s %c \"%d\" \"%s\" \"%s\" %ld \"%s\" \"%s\" %ld \"%s\" | %s",
		DE_MANIFST,fleet_id[indx],driver_id,start_date,start_time,start_dt_tm, 
		end_date,end_time,ending_dt_tm,fleet_name[indx],mads_spooler);

		system(systembuff);
		}
	}
	else {
		puts(catgets(REP_catd, 1, 1, "\n\t\tNO fleets will be reported.\n "));
		puts(catgets(REP_catd, 1, 2, "\t\tPlease, run this program later. <CR> to continue.."));
		fflush(stdin);
		gets(fleet_id);
		return(0);
	}
        /* close language catalogues */
        cleanup_catalog(REP_catd);
        cleanup_catalog(COMMON_m_catd);

	return(goodfleets);
}
/**************************************************************************************/
/* get_internal_time: gets the time in internal C format; from the display formats    */
/*	of date & time								      */
/*	Returns 	0  if invalid date/time arg is passed to function	      */
/*			time in unix format 					      */
/**************************************************************************************/
long get_internal_time(date_copy,time_copy)
char *date_copy;			/* assumed to be in mm/dd/yy format */
char *time_copy;			/* assumed to be in 24hr or 12hr format; hh:mm{a/p} */
{
	char date_time[13];			/* date & time formatted in YYMMDDhhmmss format */
	char scratch_ptr[3];			/* scratch area */
	short hours=0;

	if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
	{
		date_time[0] = date_copy[0];
		date_time[1] = date_copy[1];
		date_time[2] = date_copy[3];
		date_time[3] = date_copy[4];
		date_time[4] = date_copy[6];
		date_time[5] = date_copy[7];
	} else
	{
		date_time[0] = date_copy[6]; 			/* copy year from date */
		date_time[1] = date_copy[7];

        	if (DATE_FORMAT == DAY_FIRST_IN_DATE) {
	   	date_time[2] = date_copy[3]; 			/* copy month */
   	   	date_time[3] = date_copy[4];

	   	date_time[4] = date_copy[0]; 			/* copy day */
	   	date_time[5] = date_copy[1];
        	}
        	else {
	   	date_time[2] = date_copy[0]; 			/* copy month */
	   	date_time[3] = date_copy[1];

	   	date_time[4] = date_copy[3]; 			/* copy day */
	   	date_time[5] = date_copy[4];
        	}
	}

	/* copy the hour */
	date_time[6] = time_copy[0];
	date_time[7] = time_copy[1];

#ifdef HR12
	strncpy(scratch_ptr,time_copy,2);			/* extract just the hours */
	scratch_ptr[2] = '\0';					/* null terminate it */
	hours = atoi(scratch_ptr);				/* convert to integer hours */
	if (time_copy[5] == 'p' || time_copy[5] == 'P'){	/* PM time; take care of 1:00p - 11:59p */
		if(hours < 12)
			hours += 12;
	}
	else							/* AM time; take care of 12:01a - 12:59a */
		if(hours == 12)
			hours -= 12;
	sprintf(scratch_ptr, "%02d", hours);			/* convert back to ascii format */
	date_time[6] = scratch_ptr[0];
	date_time[7] = scratch_ptr[1];
#endif

	date_time[8] = time_copy[3]; 			/* copy the min from time */
	date_time[9] = time_copy[4];
	
	date_time[10] = '0'; 				/* copy sec's to be 0 secs */
	date_time[11] = '0';
	date_time[12] = '\0';
	
	return(conversion(date_time)); 			/* now get the internal time */

}

/*====================================================================
= choose_driver() 
=	Prompts for driver id.             
=====================================================================*/
long choose_driver()

{
	char dr_id[10];

	printf(catgets(REP_catd, 1, 3, "\n                    Please enter driver ID number:")); 
	fflush(stdin);
	gets(dr_id);
	return(atol(dr_id));
}

/*====================================================================
= choose3_dt_tm() 
=	Prompt for date and time to report.
=====================================================================*/
int choose3_dt_tm(start_date,start_time,end_date,end_time)
char *start_date, *start_time;
char *end_date, *end_time;
{
	char temp_date[10];
	char temp_time[10];
        long current_dt_tm;
        char dummy[40];

	printf(catgets(REP_catd, 1, 4, "\n                    Choose the starting date (call entry format; hit Return for today):")); 
	fflush(stdin);
	gets(temp_date);
	if (verify_date(temp_date,start_date) == -1){ /* if invalid date entered, default to NOW */
	   /*get_srch_date(time(NULL),start_date);*/
           current_dt_tm = time ( (long *) 0);
           local_date_time(current_dt_tm, start_date, dummy, dummy);
        }

	printf(catgets(REP_catd, 1, 5, "\n                    Choose the starting time (call entry format):")); 
	fflush(stdin);
	gets(temp_time);
	if (verify_time(temp_time,start_time) == -1){	/* if invalid time entered, default to NOW */
	   /*get_srch_time(time(NULL),start_time);*/
           current_dt_tm = time ( (long *) 0);
           local_date_time(current_dt_tm, dummy, start_time, dummy);
        }

	printf(catgets(REP_catd, 1, 6, "\n                    Choose the ending date (call entry format; hit Return for today):")); 
	fflush(stdin);
	gets(temp_date);
	if (verify_date(temp_date,end_date) == -1) { /* if invalid date entered, default to NOW */
	   /*get_srch_date(time(NULL),end_date);*/
           current_dt_tm = time ( (long *) 0);
           local_date_time(current_dt_tm, end_date, dummy, dummy);
        }

	printf(catgets(REP_catd, 1, 7, "\n                    Choose the ending time (call entry format; hit Return for NOW):")); 
	fflush(stdin);
	gets(temp_time);
	if (verify_time(temp_time,end_time) == -1) { /* if invalid time entered, default to NOW */
	   /*get_srch_time(time(NULL),end_time);*/
           current_dt_tm = time ( (long *) 0);
           local_date_time(current_dt_tm, dummy, end_time, dummy);
        }
	return(1);
}
