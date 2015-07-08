static char sz__FILE__[]="@(#)au_trail.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  au_trail.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:20:26
 * @(#)  Last delta: 12/14/89 at 16:55:15
 * @(#)  SCCS File: /taxi/sccs/s.au_trail.c
 *                                                                           *
 *****************************************************************************/

/* Make sure location constants are defined. Create a compile error if they
   are not defined or both are defined. */
#ifdef SCANDINAVIA
#ifdef NAMERICA
ERROR: "Both compile time constants SCANDINAVIA and NAMERICA are defined."
#endif
#else
#ifdef NAMERICA
#else
ERROR: "Compile time constant SCANDINAVIA or NAMERICA undefined."
#endif
#endif

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <termio.h>
#include <sys/signal.h>
#include <langinfo.h>
#include <malloc.h>
#include <isam.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sgtty.h>
#include <sys/ioctl.h>

#include "mads_types.h"
#include "Fleet_db.h"
#include "taxi_db.h"
#include "key.h"                /* using keydesc cl_key1 for call number */
#include "path.h"
#define DECLARING
#include "rep_strdefs.h"
#include "language.h"

extern  char buf[];     /* return buffer for call record */
extern  char req_buf[]; /* buffer to cast as call structure */
extern  int db();
extern  void local_date_time();

struct  cisam_cl *callrec;
char fleet_id[10]; 
int fleetno;

#define maxfleets 10    /* maximum number of fleets */
/* wait sleep_interval  seconds until reading new calls*/
#define sleep_interval  5      /* seconds */
#define blanks "                    "

/*====================================================================
= ad_trail() -
=       Print every call record.
=       Begins with the last recorded call record.
=       Then periodically polls the database ciasm_cl to print new call records.
=       Who:    Bruce Young
=       What:   Written         When: 20 February 1988
=       Opens each dedicated printer.
=       What:   Modified        When: 27 May 1988
=====================================================================*/
main(argc,argv)
int argc;       /* any argument will stop the audit_trail prpocess, without restarting */
char *argv[];
{
        extern open_time_out();
        char iobuff[132]; 
        char systembuff[132];
        char *fleet_printer[10];
        char *strptr;
        char ttybuff[6];

        register int n, indx;
        int delay_interval=0;     /* this paramter should be the sleep interval in seconds */
	int option=0;                           /* 0 all fleets, 1 choose from active fleets */
        int fleets;
        int looping=1;
        int ccode;
        int fd;

        struct stat statbuff;
        int filefd[maxfleets];
        int printer;
        struct sgttyb arg;

#ifdef DEBUG
        printf("audit_trail  starting\n");
#endif

        /* initialize language environment */
        init_locale();
        REP_catd = init_catalog("au_trail");

        /* assign delay sleep interval in seconds */
        if (argc > 1) delay_interval = atoi(argv[1]);     
        if (delay_interval<=0) delay_interval = sleep_interval;
        /* get all the fleets and the corresponding printer files */
        fleets = get_printers(fleet_printer);
        
        /* set an alarm to make non-blocking  file opens */
        signal(SIGALRM, open_time_out);         /* an alarm will process open_time_out */

        /* for every fleet */
        for (fleetno=0;fleetno<fleets;fleetno++) {
                /* for each fleet, must open the printer file which is returned from choosefleets */
		/* the default printer is a zero */
                filefd[fleetno]=0;
		if (get_fleet_tty(fleet_printer[fleetno], ttybuff)) {
			sprintf(iobuff, "/dev/%s", ttybuff);
                        alarm(5);   /* time out after five seconds */
                        filefd[fleetno]=open(iobuff,O_WRONLY);/* open the tty for writing */
                        alarm(0);                                       /* reset the timer */
			ioctl(filefd[fleetno], TIOCGETP, &arg);
			arg.sg_flags |= CRMOD;
			ioctl(filefd[fleetno], TIOCSETP, &arg);
                        /* if there are fleets, then reset the printers */
                        if (filefd[fleetno]) 
				init_printer(fleet_id[fleetno], fleet_printer[fleetno], iobuff, filefd[fleetno]);

#ifdef DEBUG
        printf("audit_trail  file descriptor for fleetno %d is %d.\n",fleetno,filefd[fleetno]);
#endif
                }       /* end if the printer file exists */
        }       /* end for each fleet */

        printer=filefd[0];                      /* the default hardcopy device */
#ifdef DEBUG
        printf("audit_trail  the default printer is %d.\n", printer);
#endif

        callrec=(struct cisam_cl*)req_buf;      /* to cast req_buf */

        /* initialize to look at the end of the cisam CALL file */
        ccode=db(CALL_FILE_ID,OPEN,&cl_key1,ISINPUT | ISMANULOCK,0);    /* open CALL db for read only */
        if (ccode<0) {
                sprintf(iobuff,catgets(REP_catd, 1, 1, "Error OPENing file %s iserrno %d\n"),CALL_FILE_ID,iserrno);
                write(printer,iobuff,strlen(iobuff));
                exit(-1);
        }       /* end if ccode */

        /* ISLAST points to the last record in the CISAM file */
        ccode=db(CALL_FILE_ID,START,&cl_key1,ISLAST,0);
        while (looping) { /* loop until a deliberate exit */
                ccode=db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISNEXT,0);
                if (ccode<0) {	/* could not read a call record */
                        if (iserrno == EENDFILE) {	/* reached the end of the call records */
#ifdef DEBUG
        printf("audit_trail: sleeping %d seconds\n",delay_interval);
#endif
                                sleep(delay_interval);  /* wait for a transaction */
                        }
                        else {	/* some problem reading the call table */
                                sprintf(iobuff,catgets(REP_catd, 1, 2, "Error READING file %s iserrno %d\n"),CALL_FILE_ID,iserrno);
                                write(printer,iobuff,strlen(iobuff));
                                sprintf(iobuff,catgets(REP_catd, 1, 3, "Current record is %d "),isrecnum);
                                write(printer,iobuff,strlen(iobuff));
                                sprintf(iobuff,"callrec->nbr %d \n",callrec->nbr);
                                write(printer,iobuff,strlen(iobuff));
                                exit(-1);
                        }
                }       /* end if ccode<0 */
                else { /* has a call record to print */
			/* This record belongs to a fleet. Match the fleet_id */
                        for (fleetno=0; callrec->fleet != fleet_id[fleetno] && fleetno<maxfleets; fleetno++);

			/* matched the fleet, print the record in buf on the filefd[printer] */
                        if (fleetno<maxfleets && filefd[fleetno]) print_record(filefd[fleetno]); 
                        else {	/* this record had an incorrect fleet_id */
                                sprintf(iobuff,catgets(REP_catd, 1, 4, "The call record %d has NO fleet id.\n"));
                                write(printer,iobuff,strlen(iobuff));
                        }       /* end else */
                }       /* end else print a record */
        }       /* end of while looping */

        ccode=db(CALL_FILE_ID,CLOSE,&cl_key1,0,0);
        /* this program ends, must be restarted by an administrator */
        sprintf(iobuff,catgets(REP_catd, 1, 5, "%sThe CALLS AUDIT TRAIL Program Is Terminating, Must Be Restarted By An Administrator.\n"),blanks);
        write(printer,iobuff,strlen(iobuff));
 
        /* close language catalogues */
        cleanup_catalog(REP_catd);

}       /* end of main */

/*====================================================================
= init_printer() -
=       Resets the printer for output.
=====================================================================*/
init_printer(fleet_name,printer_name,printer_device, printer)
char fleet_name;	/* the fleet_id */
char *printer_name;	/* the line printer name */
char *printer_device;	/* the line printer device tty name */
int printer;		/* the opened printer file descriptor */
{
        char iobuff[164];

	/* As an explanation of this procedure, the goal is to set the printer tty interface for writing records. */
	/* This is done once for each printer. All subsequent writing will be to one of these printers. */
	/* The tty interfce is a script with the same name as the printer, such as lp_1 */
	/* The tty interface may be a script to reset the printer or may call a script for this purpose */
	/* It is essential to direct the output from the script to the real tty device */
	/* Script parameters may vary, but all must be included because printer interfaces shift paramters */
	/* The paramters have these meanings: au_trail - source program name,	*/
	/* 	TAXI - first line of the printed banner */
	/* CALL-AUDIT- second line of the banner	*/
	/* 	0    - the number of copies to print, no copies are spooled, all are driectly printed */
	/*	p10  - the point size, roughly in characters per inch */
	/*sprintf(iobuff,"%s%s au_trail 'FLEET %c' CALL-AUDIT 0 p10 > %s",
		PRINTER_STTY,printer_name,fleet_name,printer_device); */
#ifdef DEBUG
        printf("audit_trail: %s\n",iobuff);
#endif
/* system(iobuff); */

        print_heading(printer);	/* write a heading on each printer */
        return(0);
}       /* end init_printer */ 

/*====================================================================
= print_heading() -
=       Prints the field definitions for this report.
=====================================================================*/
print_heading(printer)
int printer;
{
char iobuff[164];

/* line one */
sprintf(iobuff,catgets(REP_catd, 1, 6, "\n======================================================= CALLS AUDIT TRAIL REPORT ==================================================\n"));
write(printer,iobuff,strlen(iobuff));

/* line two */
sprintf(iobuff,catgets(REP_catd, 1, 7, "TYPE ....DUE TIME... ZONE CITY ...........PICKUP ADDRESS.............  APT  ......PASSENGER..... ...PHONE NUMBER... PR #C FL  CALL #\n"));
write(printer,iobuff,strlen(iobuff));

/* line three */
sprintf(iobuff,catgets(REP_catd, 1, 8, "     ..FARE.. CA1.2.3.4.5.6.7.8        ......INTERSECTION STREET.....      DA1.2.3.4.5.6.7.8  POD .....SIGNED BY...... .TIME. .WITH.\n"));
write(printer,iobuff,strlen(iobuff));

/* line four */
sprintf(iobuff,catgets(REP_catd, 1, 9, "     ..RATE.. PERSNL ZONE CITY .........DESTINATION ADDRESS..........  APT  ....REQUESTED BY.... ....PERSON TO SEE... HOLD LOC\n"));
write(printer,iobuff,strlen(iobuff));

/* line five */
sprintf(iobuff,catgets(REP_catd, 1, 10, "     ..ENTERED TIME.. .BY. CHNG ..EST.. ..DISPATCH TIME. .BY. ...PICKUP TIME.. ...CLOSE TIME... VEH# DRV# .STATUS. SUBSC#.\n"));
write(printer,iobuff,strlen(iobuff));

/* line six */
sprintf(iobuff,catgets(REP_catd, 1, 11, "     ......CHARGE #....... EXPIRES. .APPR CODE .BY. ....COMPANY NAME.... ...USER ENTERED 1... ...USER ENTERED 2...\n"));
write(printer,iobuff,strlen(iobuff));

/* line seven */
sprintf(iobuff,catgets(REP_catd, 1, 12, "     .............................................COMMENT LINE.. .............................................................\n"));
write(printer,iobuff,strlen(iobuff));

/* line eight */
sprintf(iobuff,catgets(REP_catd, 1, 13, "     .........DRIVER MESSAGE.............\n"));
write(printer,iobuff,strlen(iobuff));

/* line nine */
sprintf(iobuff,"====================================================================================================================================\n");
write(printer,iobuff,strlen(iobuff));
}       /* end of print_heading */

/*====================================================================
= print_record() -
=       Prints the lines of the current call record
=       that have alphanumeric characters.
=====================================================================*/
print_record(printer)
int printer;
{
        char iobuff[164];       /* line buffer */
        int cl_entered_by;
	char tbuff[132];	/* line buffer */
        /*struct tm *tt;*/
        char *currency_symbol;
        char dummy[40];
        char exp_date[20];
		char call_number[8];

#ifdef DEBUG
        printf("audit_trail: printer %d\n",printer);
#endif

	if ( (currency_symbol = nl_langinfo(CRNCYSTR)) == NULL)
	  currency_symbol = " ";
	else
	  ++currency_symbol;
	 

strcpy(call_number, last_six(callrec->nbr));

/* line one, 23 fields, note pckup_str_nbr and pckup_str_suf form one field*/
sprintf(iobuff,"%-5.5s %-9.9s %-5.5s %3d  %-3.3s %7d%1c %-2.2s %-20.20s %-3.3s %-2.2s %-5.5s %-20.20s ",
callrec->extended_type, callrec->due_date, callrec->due_time, callrec->pckup_zone, callrec->pckup_city,
callrec->pckup_str_nbr, callrec->pckup_str_suf, callrec->pckup_pre_dir, callrec->pckup_str_name,
callrec->pckup_str_type, callrec->pckup_post_dir, callrec->pckup_apt_room, callrec->passenger_name);
write(printer,iobuff,strlen(iobuff));

#ifdef SCANDINAVIA
sprintf(iobuff," %-18.18s  %2d %1c %1c %6s\n", callrec->phone_number, callrec->priority, 
		callrec->nbr_in_group, callrec->fleet, call_number);
#else
sprintf(iobuff," (%-3.3s)%-8.8s %-3.3s  %2d %1c %1c %6s\n", callrec->phone_area, callrec->phone_nbr, 
		callrec->phone_ext, callrec->priority, callrec->nbr_in_group, callrec->fleet, call_number);
#endif
write(printer,iobuff,strlen(iobuff));

/* line two */
if ( (Toupper(callrec->veh_attr_flag) == YES) && strlen(callrec->veh_attr)>7)
#ifdef SCANDINAVIA
sprintf(iobuff,"     %7.2f%s %1c %-32.32s ",
callrec->fare_amount, currency_symbol, callrec->veh_attr_flag,
callrec->veh_attr);
#else
sprintf(iobuff,"     %s%7.2f %1c %-32.32s ",
currency_symbol,callrec->fare_amount, callrec->veh_attr_flag,
callrec->veh_attr);
#endif
else 
#ifdef SCANDINAVIA
   sprintf(iobuff,"     %7.2f%s %1c                 ",
           callrec->fare_amount, currency_symbol, callrec->veh_attr_flag);
#else
   sprintf(iobuff,"     %s%7.2f %1c                 ",
           currency_symbol, callrec->fare_amount, callrec->veh_attr_flag);
#endif

if ( Toupper(callrec->drv_attr_flag) == YES && strlen(callrec->drv_attr)>7)
sprintf(tbuff,"       %-2.2s %-20.20s %-3.3s %-2.2s      %1c %-32.32s %1c %-20.20s   %6d %6d\n",
callrec->intr_pre_dir, callrec->intr_str_name, callrec->intr_str_type, callrec->intr_post_dir,
callrec->drv_attr_flag, 
callrec->drv_attr,
callrec->pod_required, callrec->pod_name, callrec->pod_time, callrec->grouped_with);

else
sprintf(tbuff,"       %-2.2s %-20.20s %-3.3s %-2.2s       %1c  %-20.20s   %6d %6d\n",
callrec->intr_pre_dir, callrec->intr_str_name, callrec->intr_str_type, callrec->intr_post_dir,
callrec->pod_required, callrec->pod_name, callrec->pod_time, callrec->grouped_with);

strcat(iobuff,tbuff);
if (printable_line(iobuff)) write(printer,iobuff,strlen(iobuff));

/* line three */
#ifdef SCANDINAVIA
sprintf(iobuff, "     %7.2f%s  %04d%1c %3d %-3s  %6d %1c %-2s %-20s %-3s %-2s %-5s %-20s %-20s %-4s %1c\n", callrec->call_rate, currency_symbol, callrec->personal_veh, callrec->personal_rqst, callrec->dest_zone, callrec->dest_city, callrec->dest_str_nbr, callrec->dest_str_suf, callrec->dest_pre_dir, callrec->dest_str_name, callrec->dest_str_type, callrec->dest_post_dir, callrec->dest_apt_room, callrec->requested_by, callrec->person_to_see, callrec->hold_line, callrec->local);
#else
sprintf(iobuff, "     %s%7.2f  %04d%1c %3d %-3s  %6d %1c %-2s %-20s %-3s %-2s %-5s %-20s %-20s %-4s %1c\n", currency_symbol,callrec->call_rate, callrec->personal_veh, callrec->personal_rqst, callrec->dest_zone, callrec->dest_city, callrec->dest_str_nbr, callrec->dest_str_suf, callrec->dest_pre_dir, callrec->dest_str_name, callrec->dest_str_type, callrec->dest_post_dir, callrec->dest_apt_room, callrec->requested_by, callrec->person_to_see, callrec->hold_line, callrec->local);
#endif
if (printable_line(iobuff)) write(printer,iobuff,strlen(iobuff));

/* line four */
cl_entered_by = callrec->entered_by;
if (callrec->entered_by == -1) cl_entered_by = 0;
sprintf(iobuff, "     %-9.9s %-6.6s %4d %4d %-7.7s %-9.9s %-6.6s %4d %-9.9s %-6.6s %-9.9s %-6.6s %4d %5d %-8.8s %6d\n",
callrec->date_in, callrec->time_in, cl_entered_by, callrec->last_changed_by, 
callrec->est_wait_time, callrec->dispd_date, callrec->dispd_time, callrec->dispatched_by, 
callrec->pckup_date, callrec->pckup_time, callrec->close_date, callrec->close_time, 
callrec->veh_nbr, callrec->drv_id, callrec->status, callrec->subsc_call_nbr);
if (printable_line(iobuff)) write(printer,iobuff,strlen(iobuff));

/* line five */
/* get the date_time structure */
if (callrec->expire_date >0) {
	/*tt=localtime( &(callrec->expire_date) );*/
        local_date_time(callrec->expire_date, exp_date, dummy, dummy);
	sprintf(iobuff,"     %-18.18s %s %-10.10s %d %-20.20s %-20.20s %-20.20s\n",
	callrec->charge_nbr, exp_date,
        /*callrec->charge_nbr, tt->tm_mon+1,tt->tm_mday,tt->tm_year,*/
	callrec->approve_code, callrec->approve_by, callrec->company_name, 
	callrec->ac_addt1_info1, callrec->ac_addt1_info2);
	if (printable_line(iobuff)) write(printer,iobuff,strlen(iobuff));
}	/* end if has a date to print */

/* line six */
sprintf(iobuff,"     %-120.120s\n",callrec->general_cmnt);
if (printable_line(iobuff)) write(printer,iobuff,strlen(iobuff));

/* line seven */
sprintf(iobuff,"     %-37.37s\n",callrec->driver_msg);
if (printable_line(iobuff)) write(printer,iobuff,strlen(iobuff));
write(printer,"\n",strlen("\n"));
	return(0);
}	/* end print_record */

/*====================================================================
= get_printers() -
=       Scans all active fleets.
=       Returns the number of active fleets,
=       optionally, the number of fleets choosen.
=====================================================================*/
int get_printers(fleet_printer)
char *fleet_printer[];
{
        register int indx;
        int bufflen;
        int ccode;
        int goodfleet;
        char iobuff[64];        /* operator responses */
        char *p1;

        struct  cisam_fl *fleetrec;
        fleetrec=(struct cisam_fl*)req_buf;     /* to cast req_buf */

        /* initialize to read the cisam FLEET file */
        ccode=db(FLEET_FILE_ID,OPEN,&fl_key2,ISINPUT | ISMANULOCK,0);   /* open CALL db for read only */
        if (ccode<0) {
                printf(catgets(REP_catd, 1, 1, "Error OPENing file %s iserrno %d\n"),FLEET_FILE_ID,iserrno);
                exit(-1);
        }       /* end if ccode */

        /* define the key field, for all fleets start from 0 */
        fleetrec->nbr = 0;
        fleetno=0;
        /* read the matching record */
        ccode=db(FLEET_FILE_ID,READ_KEYS,&fl_key2,ISEQUAL,0);
        while (ccode >=0) {
#ifdef DEBUG
        printf("fleet->active %c fleetrec->id %c \n", fleetrec->active,fleetrec->id);
#endif
                if (Toupper(fleetrec->active) == YES) {
                        /* choose only active fleets */
                        fleet_id[fleetno]=fleetrec->id; /* save the fleet identifier */
                        if ((p1=malloc(21))==NULL) exit(-1);
                        strcpy(p1,fleetrec->audt_trail_port); /* save the fleet name */
                        fleet_printer[fleetno]=p1;
#ifdef DEBUG
        printf("fleet_id %c fleet_printer %s \n",fleet_id[fleetno],fleet_printer[fleetno]);
#endif
                        fleetno++;
                }       /* end if fleetrec */
                /* if dispatching all fleet procedures, get the next matching record */
                ccode=db(FLEET_FILE_ID,READ_KEYS,&fl_key2,ISNEXT,0);
        }       /* end while */ 

        /* close the FLEET file */
        ccode=db(FLEET_FILE_ID,CLOSE,&fl_key2,0,0);
        if (ccode<0) {
                printf(catgets(REP_catd, 1, 14, "Error CLOSEing file %d iserrno %d\n"),FLEET_FILE_ID,iserrno);
                printf("fleetrec->id %c \n",fleetrec->id);
                exit(-1);
        }       /* end if ccode */

        goodfleet=fleetno;
        fleet_id[goodfleet]=NULL;
        fleet_printer[goodfleet]= (char *) NULL;
        return(goodfleet);
}       /* end get_printer */

/*====================================================================
= open_time_out() -
=       Prints a message about a non-working printer.
=====================================================================*/
int open_time_out()
{
        /* The operating system cannot communicate with some printer */
        printf(catgets(REP_catd, 1, 15, "\n%sThe printer for fleet %c is not hooked-up.\n\n"), blanks,fleet_id[fleetno]);
        fflush(stdout);
}       /* end open_time_out */


/*----------------------------------------------------
 *	get fleet tty
 *----------------------------------------------------
 */

get_fleet_tty(fleet_printer, fleet_tty)
	char		fleet_printer[],
				fleet_tty[];
{
	FILE    *fp;
	char     string[132],
			*slash,
			 tty[6];
	short    found_printer = 0,
			 found_tty = 0;


	if (!(fp = fopen("/etc/printcap", "r")))
	  {
		printf(catgets(REP_catd, 1, 16, "couldn't open /etc/printcap\n"));
		return(0);
	  }

	while(fgets(string, 132, fp))
	  {
		if (!found_printer &&
			strncmp(string, fleet_printer,  strlen(fleet_printer)) == 0)
			found_printer = 1;

		if (found_printer)
		  {
			if (slash = strrchr(string, '/'))
			  {
				if (strncmp(slash + 1, "tty", strlen("tty")) == 0)
				  {
					strncpy(fleet_tty, slash + 1, strlen("ttyNN"));
					found_tty = 1;
					break;
				  }
			  }
		  }
	  }

	fclose(fp);

	return(found_tty);
}

