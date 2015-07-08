static char sz__FILE__[]="@(#)chk_midnite.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  chk_midnite.c; Rel: 6.3.0.0; Get date: 3/23/92 at 09:29:06
* @(#)  Last delta: 3/23/92 at 09:25:36
* @(#)  SCCS File: /taxi/sccs/s.chk_midnite.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

/****************************************************************************/
/* file chk_midnite.c 			                                    */
/* routines to check for midnight passing do special things for files.      */
/* TaxiPak R4                                                               */
/* started 25 Dec 1991                                                      */
/* Gary McCartney                                                           */
/****************************************************************************/

#include <stdio.h>
#include <time.h>
#include <sys/types.h>

#include "path.h"

FILE *missing_receipt_fp;
char missing_receipt_file_name[64] = "";

FILE *credit_fp = NULL;
char credit_file_name[64] = "";

char file_name[64] = "";
FILE *receipt_fp = NULL;
time_t	midnight = (0l);

chk_midnite()
{
	if ( time((time_t *)0) > midnight )
	{
		if ( receipt_fp != NULL )
		{
			fclose( receipt_fp );
			receipt_fp = NULL;
			rename( RECEIPT_TRANSACTION_FILE, file_name );
		}

		if ( missing_receipt_fp != NULL )
		{
			fclose( missing_receipt_fp );
			missing_receipt_fp = NULL;
			rename( MISSING_RECEIPT_TRANSACTION_FILE, missing_receipt_file_name );
		}

		if ( credit_fp != NULL )
		{  
			fclose( credit_fp );
			credit_fp = NULL;
			rename( CREDIT_TRANSACTION_FILE, credit_file_name );
		}  

		set_file_names();
	}
}

set_file_names()
{
	extern struct tm *localtime();
	extern time_t tm_to_time();
	struct tm *tm_ptr;		/* pointer to formatted time structure */
	long   nowtime;


	/* get the current time and build the file name   */
	/*                                                */
	nowtime = (time_t)time((time_t *)0);
	tm_ptr = localtime(&nowtime);
	sprintf(file_name,"%s.%02d%02d%02d", RECEIPT_TRANSACTION_FILE,
		tm_ptr->tm_year % 100, tm_ptr->tm_mon + 1, tm_ptr->tm_mday);
	sprintf(missing_receipt_file_name,"%s.%02d%02d%02d", MISSING_RECEIPT_TRANSACTION_FILE,
		tm_ptr->tm_year % 100, tm_ptr->tm_mon + 1, tm_ptr->tm_mday);
	sprintf(credit_file_name,"%s.%02d%02d%02d", CREDIT_TRANSACTION_FILE,
		tm_ptr->tm_year % 100, tm_ptr->tm_mon + 1, tm_ptr->tm_mday);

	tm_ptr->tm_sec  = 59;
	tm_ptr->tm_min  = 59;
	tm_ptr->tm_hour = 23;

	midnight = tm_to_time( tm_ptr );
	midnight += 1;			/* increment the time by one second to get to midnight     */
					/* this is much easier than trying to set the tm structure */
					/* up for the next day                                     */

}
 

