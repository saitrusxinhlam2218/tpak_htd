/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: displayps.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1997/08/09 18:08:08 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/sysadmin/displayps.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: displayps.c,v 1.2 1997/08/09 18:08:08 taxiadm Exp $";
#define DECLARING
#include "comm_strdefs.h"
#include "disp_strdefs.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include <isam.h>
#include <sys/types.h>
#include "machdep.h"
#include "mads_types.h"


#define LONG_PROCESS_MAX        8

static char *process[] = { "/usr/taxi/bin/ncp","/usr/taxi/bin/mirror","/usr/taxi/bin/reflect","/usr/taxi/bin/dispatch","/usr/taxi/bin/counter","ui","da_report","da_purge" };
static char *long_process[ LONG_PROCESS_MAX ] = { 0 };


#define process_entries 8
#define mirror_index 	1
#define reflect_index 	2
#define ui_index 	5
#define storage_period 	6

#define process_file 	"/usr/taxi/run/display_ps.fi"

#define BLANKS 		"                    "
#define ROOT 		0

/*      This function was added because we had to modify array  */
/*      initialization for internationalization.                */
void init_long_process_array()
{
        long_process[0] = m_catgets(DISP_m_catd, DISP_SET, DISP_13, "Network Control Processor");
        long_process[1] = m_catgets(DISP_m_catd, DISP_SET, DISP_14, "Mirror");
        long_process[2] = m_catgets(DISP_m_catd, DISP_SET, DISP_15, "Reflection");
        long_process[3] = m_catgets(DISP_m_catd, DISP_SET, DISP_16, "TAXI Dispatch");
        long_process[4] = m_catgets(DISP_m_catd, DISP_SET, DISP_17, "Counter");
        long_process[5] = m_catgets(DISP_m_catd, DISP_SET, DISP_18, "User Interface");
        long_process[6] = m_catgets(DISP_m_catd, DISP_SET, DISP_19, "Daily Report");
        long_process[7] = m_catgets(DISP_m_catd, DISP_SET, DISP_20, "Daily Purge");
}
 

/*====================================================================
= main()
=	display_ps	  reschedules da_report, displays processes
=	display status of current and future processes.
=====================================================================*/

main(argc,argv)
int argc;
char *argv[];
{ 


	/*
	 * Set up for internationalization by setting the locale and opening the catalog
	 */
	setlocale(LC_ALL, "");
	open_catalog(&COMMON_m_catd, "common", 0, 0);
	open_catalog(&DISP_m_catd, "display_ps", 0, 0);

        init_long_process_array();              /* Initialize long process      */
                                                /* array.  This had to be added */
                                                /* for internationalization.    */
	reschedule_process();			/* reschedule da_report */

	process_status();

	catclose( COMMON_m_catd );
	catclose( DISP_m_catd );
	exit(0);
}

/*====================================================================
= reschedule_process() -
=	Schedules a report using at.
=====================================================================*/

reschedule_process()
{
  	setuid(ROOT);		/* set id to ROOT to run at */

	/* reschedule da_report */
	system("/usr/taxi/bin/reports/da_report -1 2> /dev/null");
	system("at -r `grep -l da_subcall /usr/spool/cron/atjobs/*.a 2>/dev/null`");
	system("echo \"/usr/taxi/bin/reports/da_subcall \" | at 23:54 2>/dev/null");

	return;
}

/*==================================================================
=	process_status()
=		print and remove files.
==================================================================*/

process_status()
{
	FILE *process_fp; 

	char buff[164]; 
	char iobuffer[132]; 
	char *str_ptr;

	int index;

	int process_table[process_entries];
	int taxi_process_counter;
	int shadow_process_counter;

  	setuid(ROOT);				/* set id to run at */

	clear_scrn();
	printf("\n%s%s\n", BLANKS, catgets(DISP_m_catd, DISP_SET, DISP_1, "********* TAXI ADMINISTRATION *********") );
	printf(  "%s%s\n", BLANKS, catgets(DISP_m_catd, DISP_SET, DISP_2, "*********   PROCESS  TABLES   *********") );
	printf("\n%s%s\n", BLANKS, catgets(DISP_m_catd, DISP_SET, DISP_3, "            Live Processes") );
	printf(  "%s----------------------------------------\n", BLANKS);

	/* initialize the process variables */
	for(index = 0; index < process_entries; index++)
		process_table[index] = 0;
	taxi_process_counter=0;
	shadow_process_counter=0;

	/* create and read the process file */

	sprintf(buff,"ps -ax > %s ",process_file); 
	system(buff);
	process_fp = fopen(process_file,"r");
	while(fgets(iobuffer, 132, process_fp)) {
		iobuffer[strlen(iobuffer)-1] = '\0';
		str_ptr=strrchr(iobuffer,' ') + 1;

		/* the process table entries are 0 if the process is not running otherwise are 1 */
		for (index=0; index<process_entries; index++) {
			/* matching strings are counted */
			if (! strcmp(str_ptr,process[index])) {
				process_table[index]++;
			}
		}	/* post the process into the process_table */
	}
	unlink(process_file);

	/* print the process status */
	for (index=0; index<process_entries; index++) 
		/* count shadow processes running */
		if(process_table[index]) 	
			switch(index) {
		case mirror_index: 	
			if(process_table[index] > 1  && !shadow_process_counter) {
				taxi_process_counter++;
				shadow_process_counter++;
				printf("%s%s.\n", BLANKS, catgets(DISP_m_catd, DISP_SET, DISP_4, "The Shadow Processes are running") );
			}
			break;

		case reflect_index: 
			if(! shadow_process_counter) {
				taxi_process_counter++;
				shadow_process_counter++;
				printf("%s%s.\n", BLANKS, catgets(DISP_m_catd, DISP_SET, DISP_4, "The Shadow Processes are running") );
			}
			break;

		case ui_index:
			/* display all user interface processes running */
			taxi_process_counter++;
			if (process_table[index]==1)
				printf("%s%s.\n", BLANKS, catgets(DISP_m_catd, DISP_SET, DISP_5, "One person is running the User Interface process") );
			else
				printf("%s%s %d %s.\n", 
					BLANKS, catgets(DISP_m_catd, DISP_SET, DISP_6, "There are"), process_table[index], catgets(DISP_m_catd, DISP_SET, DISP_7, "User Interface processes running") );
			break;

		default:
			/* count all TAXI processes running */
			taxi_process_counter++;
			printf("%s%s %s .\n", BLANKS, catgets(DISP_m_catd, DISP_SET, DISP_8, "The"), long_process[index], catgets(DISP_m_catd, DISP_SET, DISP_9, "process is running") );
			break;
		}

	if (! taxi_process_counter)
		printf("%s%s.\n", BLANKS, catgets(DISP_m_catd, DISP_SET, DISP_10, "NO TAXI processes are running") );

	/* look for any runnable at-processes */
	printf("\n%s%s\n", BLANKS, catgets(DISP_m_catd, DISP_SET, DISP_11, "            Future Processes") );
	printf(  "%s----------------------------------------\n", BLANKS);

	/* create and read the future process file */
	sprintf(buff,"find /usr/spool/cron/atjobs -name '*.a' -print > %s",process_file);
	system(buff);
	taxi_process_counter=0;
	process_fp=fopen(process_file,"r");

	while ( fgets(iobuffer,132,process_fp) ) {
		taxi_process_counter++;
		printf("%s", BLANKS);
		fflush(stdout);
		sprintf(buff,"tail -2 %s ",iobuffer);
		system(buff);
	}

	if (! taxi_process_counter)
		printf("%s%s.\n", BLANKS, catgets(DISP_m_catd, DISP_SET, DISP_12, "NO future TAXI processes are scheduled") );
}

