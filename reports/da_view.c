static char sz__FILE__[]="@(#)da_view.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  da_view.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:21:24
 * @(#)  Last delta: 12/14/89 at 17:00:58
 * @(#)  SCCS File: /taxi/sccs/s.da_view.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <isam.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>
#include <isam.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mads_types.h"
#include "Fleet_db.h"
#include "taxi_db.h"
#include "key.h"
#include "mad_error.h"		/* for defines */
#include "path.h"
#define DECLARING
#include "rep_strdefs.h"
#include "language.h"

#define MAXREPORTS 21
#define blanks "                    "

static char *command[]={MORE_CMD, PRINT_CMD};
extern	char req_buf[];	/* buffer to cast as call structure */
extern	int db();
extern void local_date_time();

/*====================================================================
= main()
=	Prompts for the reports to view.
=	Prompts to view on the screen or to print as hard copy.
=	Who:	Bruce Young 	When: 20 April 1988	What: Wrote
=====================================================================*/
main(argc,argv)
int argc;
char *argv[];	
{
	char fleet_id[50];	/* plenty of fleets */
	char *fleet_name[50];	/* will malloc room for each fleet name */
	int goodfleets;
	int option=1;

        /* initialize language environment */
        init_locale();
        REP_catd = init_catalog("da_view");

	/* mark all the active fleets, option 1 is interactive */
	goodfleets = pick_fleets(fleet_id,fleet_name,option);

	/* when active fleets, print daily reports */
	if (goodfleets) print_reports(goodfleets,fleet_id,fleet_name);

        /* close language catalogues */
        cleanup_catalog(REP_catd);

}	/* end main */

/*====================================================================
= print_reports() -
=	Prints reports for the choosen fleets to the screen or as printer output.
=====================================================================*/
print_reports(goodfleets,fleet_id,fleet_name)
int goodfleets;
char *fleet_id, *fleet_name[];
{
	register int fleetindx;
	int report_number;
	int fleets=strlen(fleet_id);
	int iodev;
	char filename[81];
	char systembuffer[132];
	char report_date[20];
	struct stat statbuff;
	char reply[60];		/* reply buffer */

	chdir(REPORT_DATA); 
	while (1) { 	/* as long as reports are choosen */

		/* Prompt for the reports to view.  */
		report_number=report_io(goodfleets,fleet_id,report_date,&iodev); 
		if (! report_number) return(0);	 /* no reports to view */
		/* Print choosen reports for choosen fleets on the screen or as printed output.  */
		for (fleetindx=0;fleetindx<fleets;fleetindx++) { /* for each fleet */

			clear_scrn();
/*			printf(catgets(REP_catd, 1, 2, "\n\n%s      TAXI SYSTEMS\n"),blanks); */

			if (report_number<0) /* print all reports */
				for (report_number=1;report_number<=MAXREPORTS;report_number++) {
					sprintf(filename,"%s/%c%02d.%s",REPORT_DATA,fleet_id[fleetindx],report_number,report_date);
				if (stat(filename,&statbuff)) {	/* there is NO such file */
                                        printf(catgets(REP_catd, 1, 3, "\n%sReport %s is unavailable\n"), blanks, filename);
					printf(catgets(REP_catd, 1, 4, "%s<CR> to continue..... "),blanks);
					fflush(stdin);
					gets(systembuffer);
				}	/* end if NO such file */	
                                else {
					sprintf(systembuffer,"%s %s",command[iodev],filename);
					system(systembuffer);
/* if display, (not print), prompt for <CR> to proceed to another report */
if (iodev == 0) {
   printf(catgets(REP_catd, 1, 4, "%s<CR> to continue..... "),blanks);
   fflush(stdin);
   gets(reply);
}
                                 }
				}	/* end for printing all choosen reports */
			
			else if (report_number>0) {		/* print one report */
				sprintf(filename,"%s/%c%02d.%s",REPORT_DATA,fleet_id[fleetindx],report_number,report_date);
				if (stat(filename,&statbuff)) {	/* there is NO such file */
                                        printf(catgets(REP_catd, 1, 3, "\n%sReport %s is unavailable\n"), blanks, filename);
					printf(catgets(REP_catd, 1, 4, "%s<CR> to continue..... "),blanks);
					fflush(stdin);
					gets(systembuffer);
				}	/* end if NO such file */	
				else {		/* has a file to view */
					sprintf(systembuffer,"%s %s",command[iodev],filename);
					system(systembuffer); 
/* if display, (not print), prompt for <CR> to proceed to another report */
if (iodev == 0) {
   printf(catgets(REP_catd, 1, 4, "%s<CR> to continue..... "),blanks);
   fflush(stdin);
   gets(reply);
}
				}	/* end else has a file to print */
			}	/* end else print one report */
		}	/* end for each fleet */
	}	/* end while 1 loop until done */
}	/* end print_reports */

/*====================================================================
= report_io() -
=	Prompt for dates of reports and report names to print.
=	Prompts to print to the screen or to the printer.
=====================================================================*/
int report_io(fleets,fleet_id,report_date,iodev)
int fleets;
char *fleet_id;
char *report_date;
int *iodev;
{
	char systembuff[64];
	char reply[60];		/* reply buffer */
	int bufflen, choice;
	int report_number,ok,tempvalue;
	int fleetindx;
        long current_dt_tm;
        struct tm *current_tm;
        char local_date[20];
        char dummy[40];

	while (1) { /* loop until a deliberate exit */

		/* Get a report to view.  */
		/* Wait until something entered, -1 will print all files */
		while ((report_number=choose_report()) == 0);
		/* -2 is exit */
		if (report_number == -2)
		  return(0);

		/* Prompt for the date of the report to view. */
		clear_scrn();
		if (report_number>0) {
			printf(catgets(REP_catd, 1, 5, "\n\n%sThe following reports are available:\n\n"),blanks);
			for (fleetindx=0;fleetindx<fleets;fleetindx++) { /* for each fleet */
				sprintf(systembuff," ls %s/%c%02d.*",REPORT_DATA,fleet_id[fleetindx],report_number);
				system(systembuff);
			}	/* end for */
		}	/* end if */
		else if (report_number == -1) {
			printf(catgets(REP_catd, 1, 5, "\n\n%sThe following reports are available:\n\n"),blanks);
			for (fleetindx=0;fleetindx<fleets;fleetindx++) { /* for each fleet */
				sprintf(systembuff," ls %s/%c*.*",REPORT_DATA,fleet_id[fleetindx]);
				system(systembuff);
			}	/* end for */
		}	/* end if */

                current_dt_tm = time( (long *) 0);
                local_date_time(current_dt_tm,local_date,dummy,dummy);
		printf(catgets(REP_catd, 1, 6, "\n\n\n\n%sEnter the date in the following format: %s\n%sor 'e' to exit.\n"),blanks, local_date, blanks);
                fflush(stdout);
                fflush(stdin);

		/* The reply 'e' is an exit, will print nothing */
		gets(reply); 
		if (! strlen(reply)) continue;
		if (Toupper(reply[0])== M_EXIT)
		  continue; 

                report_date[0] = reply[0];
                report_date[1] = reply[1];
                report_date[2] = reply[3];
                report_date[3] = reply[4];
                report_date[4] = reply[6];
                report_date[5] = reply[7];
                report_date[6] = '\0';

                /* get the output device */
		printf(catgets(REP_catd, 1, 7, "\n\n%sPRINT OUTPUT ON \n\n"),blanks);
		printf(catgets(REP_catd, 1, 8, "%s1. The Screen\n"),blanks);
		printf(catgets(REP_catd, 1, 9, "%s2. The Printer\n\n"),blanks);
		printf(catgets(REP_catd, 1, 10, "%sOr 'e' to exit: "),blanks);
                fflush(stdout);
                fflush(stdin);
		gets(reply);
		if (! strlen(reply)) continue; /* will print nothing */
		if (Toupper(reply[0]) == M_EXIT)
		  continue; /* will print nothing */
                ok=1;
		if ( (*iodev=atoi(reply))<1 || *iodev >2) ok=0; /* look for 1 or 2 */
		else (*iodev)--;	/* will return 0 or 1 */
		if (! ok) { /* this is not a reasonable output device */
			printf(catgets(REP_catd, 1, 11, "\n%sPlease type a valid output device.\n"),blanks);
			printf(catgets(REP_catd, 1, 4, "%s<CR> to continue..... "),blanks);
			fflush(stdin);
			gets(reply);
		}	/* end if not ok */

		if (ok)	return(report_number);
	}	/* end while */
}	/* end report_io */

/*====================================================================
= choose_report() -
=	Display report titles.
=	Returns the choice: * for all, nn for one report, 0 for NO reports 
=====================================================================*/
int choose_report()
{
	char systembuff[64];
	char reply[60];		/* reply buffer */
	int bufflen, choice;

	sprintf(systembuff,"cat %s ",REPORT_CHOICES);
	while (1) { /* loop until a deliberate exit */

		clear_scrn();
		fflush(stdout);
		printf(catgets(REP_catd, 1, 12, "%s               THESE REPORTS MAY BE PRINTED\n"),blanks);
		system(systembuff);

		printf(catgets(REP_catd, 1, 13, "\n%sEnter a number 1..%d to select a particular report,\n"),blanks,MAXREPORTS);
		printf(catgets(REP_catd, 1, 14, "%sor 'e' to exit, '*' for all reports: "),blanks);
		fflush(stdout);
		fflush(stdin);
		gets(reply);
		if (! (bufflen=strlen(reply)) ) return(0); /* return when no reply */
		if (Toupper(reply[0]) ==M_EXIT)
		  return(-2); /* return when no reply */

		if (reply[0] == '*') return(-1);	/* -1 for all reports */
		if ((choice=atoi(reply)) > 0 && choice <= MAXREPORTS) return(choice); 	/* print report */

		printf(catgets(REP_catd, 1, 15, "\n%sThat reply was NOT valid.  "),blanks);
		printf(catgets(REP_catd, 1, 16, "<CR> to continue .... "));
		fflush(stdin);
		gets(reply);
	}	/* end while */

}

/*====================================================================
= pick_fleets() -
=	Returns the fleets choosen from the active fleets.
=====================================================================*/
int pick_fleets(fleet_id,fleet_name,option)
char *fleet_id, *fleet_name[];
int option;	/* 0 all fleets, 1 choose from active fleets */
{
	register int indx, fleetno;
	int bufflen;
	int ccode;
	int goodfleet;
	char iobuff[64];	/* operator responses */
	char *p1;

	struct	cisam_fl *fleetrec;
	fleetrec=(struct cisam_fl*)req_buf;	/* to cast req_buf */

	/* initialize to read the cisam FLEET file */
	if(db(FLEET_FILE_ID,OPEN,&fl_key2,ISINPUT | ISMANULOCK,0) < 0) {	/* open CALL db for read only */
		printf(catgets(REP_catd, 1, 17, "Error OPENing file %s iserrno %d\n"),FLEET_FILE_ID,iserrno);
		exit(-1);
	}

	/* define the key field, for all fleets start from 0 */
	fleetrec->nbr = 0;
	fleetno=0;
	/* read the matching record */
	ccode=db(FLEET_FILE_ID,READ_KEYS,&fl_key2,ISEQUAL,0);

	while (ccode >=0) {
		if (Toupper(fleetrec->active) == YES) {
			/* choose only active fleets */
			fleet_id[fleetno]=fleetrec->id; /* save the fleet identifier */
			p1=malloc(21);
			strcpy(p1,fleetrec->name); /* save the fleet name */
			fleet_name[fleetno]=p1;
			fleetno++;
		}
	
		/* if dispatching all fleet procedures, get the next matching record */
		ccode=db(FLEET_FILE_ID,READ_KEYS,&fl_key2,ISNEXT,0);
	}

	/* close the FLEET file */
	if(db(FLEET_FILE_ID,CLOSE,&fl_key2,0,0) < 0) {
		printf(catgets(REP_catd, 1, 18, "Error CLOSEing file %d iserrno %d\n"),FLEET_FILE_ID,iserrno);
		printf("fleetrec->id %c \n",fleetrec->id);
		exit(-1);
	}

	goodfleet=fleetno;
	/* choose the fleets to report */
	if (option) {
		clear_scrn();
		printf(catgets(REP_catd, 1, 19, "%sTAXI SYSTEM ACTIVE FLEETS\n"),blanks);
		printf("%s=========================\n\n",blanks);
		printf(catgets(REP_catd, 1, 20, "%sIdentifier    Fleet\n\n"),blanks);
		for (indx=0; indx<fleetno; indx++)  
			printf("%s%c             %s\n\n",blanks,fleet_id[indx],fleet_name[indx]);
		printf(catgets(REP_catd, 1, 21, "%sChoose the identifiers of the fleets to report.\n"),blanks);
		printf(catgets(REP_catd, 1, 22, "%sFor example: AB\n"),blanks);
		printf(catgets(REP_catd, 1, 23, "%sor 'e' to exit, '*' to report for all fleets: "),blanks);
		fflush(stdout);
		fflush(stdin);
		gets(iobuff);

		if (! (bufflen=strlen(iobuff)) ) return(0); /* return when no reply */
		if (Toupper(iobuff[0]) == M_EXIT)
		  return(0); /* return when no reply */
		if (iobuff[0] == '*') goodfleet=fleetno;
		else {
			/* save only the active and chosen fleets */
			for (indx=0; indx<bufflen; indx++) 
				iobuff[indx]=Toupper(iobuff[indx]);
			goodfleet=0;
			for (indx=0; indx<fleetno; indx++)
				if ( strchr(iobuff,fleet_id[indx]) ) {
					/* will report for this fleet */
					fleet_id[goodfleet]=fleet_id[indx];
					fleet_name[goodfleet]=fleet_name[indx];
					goodfleet++;	/* count of fleets to report */
				}
		}
	}

	fleet_id[goodfleet]=NULL;
	fleet_name[goodfleet]= (char *) NULL;
	return(goodfleet);
}

