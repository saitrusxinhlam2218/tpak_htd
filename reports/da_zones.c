static char sz__FILE__[]="@(#)da_zones.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  da_zones.c; Rel: 10.1.0.0; Get date: 4/18/95 at 17:01:19
* @(#)  Last delta: 12/2/94 at 11:16:21
* @(#)  SCCS File: /taxi/sccs/s.da_zones.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>

#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "ex_key.h"
#include "rep_strdefs.h"
#include "language.h"
#include "path.h"

#define max_super_zone 1000

extern	char buf[];	/* for db put and get records       */
extern	char req_buf[];	/* buffer to cast as call structure */
extern	int db();
extern	char *get_rp_name();

struct	cisam_cl *shiftrec;
struct	cisam_zn *zonerec;
struct	cisam_vh *vehrec;

/* builds reports 6, 7 , 8, 17, 18, 19 */

/* the accumulators for five reports follow: */
/* array size max_super_zone has an upper bound of one zone to each super zone */
int zone_to_super[max_super_zone];		/* linked lists for super zones */
int super_zones_sorted[max_super_zone];		/* ascending super zones */
char super_zones_names[max_super_zone][20];	/* sorted super zone names */

/* counters for reports 9.1.3.6, 9.1.3.8, 9.1.4.1, 9.1.4.2, and 9.1.4.3 */
/* the entry[0] represents events between 12 midnight and 1am, entry[23] between 11pm and 12 midnight */

/*
  "all" counters are for reports 6 and 8 (index 0 and 1) which are for all calls.
   "accepted" counters are for reports 17, 18 and 19 (index 2, 3, 4) which are for calls that are accepted by drivers.
 */

int all_zone_hr_count[max_super_zone][25];	/* counters for max_super_zone zones, 24 hours, for all reports all calls*/
int accepted_zone_hr_count[max_super_zone][25];	/* counters for max_super_zone zones, 24 hours, for all reports accepted calls */

/* accumulators for reports 9.1.4.1, 9.1.4.2, and 9.1.4.3 */
/* the entry[0] represents events between 12 midnight and 1am, entry[23] between 11pm and 12 midnight */
int zone_hr_accum[max_super_zone][25][3];	/* accumulators for max_super_zone zones, 24 hours, 3 time deltas */
int zone_info[max_super_zone][6];		/* call type counters for report 9.1.3.8 */

/* super zone accumulators */
int all_super_zone_hr_count[max_super_zone][25];	/* super zone counters */
int accepted_super_zone_hr_count[max_super_zone][25];	/* super zone counters */
int super_accum;				/* points to the first row of zone_hr_accum of the current super zone */

/* all zone accumulators */
long int all_total_zone_hr_count[25];	/* report total counters */
long int accepted_total_zone_hr_count[25];	/* report total counters */
long int total_zone_hr_accum[25];
long int total_zone_info[6];

/* the accumulators for five reports follow: */
int cabs_in_fleet;		/* count of all cabs in this fleet */
int hour_mask[24];		/* hour bit masks for bits 0..23 */
int cab_marker[10000];	/* one word per cab number with one bit per hour of the day */

static int no_of_cabs = 10000;

FILE *fileptr;
long seconds;			/* current date and time in seconds */
int pagenumber;			/* printed page counter */
int printedlines;		/* global printed line counter */
long total_time, total_entries;
int times;

#define ZONE_LIMIT 29
#ifdef NAMERICA
#define LINESPERPAGE 53		/* lines per page */
#else
#define LINESPERPAGE 41        /*  lines per page for SCANDINAVIA */
#endif

/*====================================================================
= da_zones()
=	Manages five daily reports: 9.1.3.6, 9.1.3.8, 9.1.4.1, 9.1.4.2, 9.1.4.3
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=====================================================================*/
da_zones(fleet_id,start_date,start_time,start_dt_tm,end_date,end_time,ending_dt_tm,fleet_name)
     char fleet_id;
     char *start_date;
     char *start_time;
     long start_dt_tm;
     char *end_date;
     char *end_time;
     long ending_dt_tm;
     char *fleet_name;
{
  struct tm *tt;
  
  char buffer[264];
  int ccode;		/* returned from proc calls */
  int fleetno;
  register int zone_no;
  register int indx;
  register int indx2;
  int cab_no;		/* will print with leading zero */
  int next_super_zone;	/* pointer to empty cell of super_zones_sorted */
  
  seconds = time( (long *) 0);			/* current date and time in seconds */
  
  initialize();		/* zero all accumulators */
  
  /* count the number of cabs for this fleet */
  /* initialize the shiftrec structure */
  vehrec=(struct cisam_vh*)req_buf;	/* to cast req_buf */
  vehrec->fleet = fleet_id;
  vehrec->nbr = 0; /* for every pickup zone */
  cabs_in_fleet = 0;
  
  /* open the cisam VEHICLE file, using the composite key4, fleet & pckup_zone */
  ccode=db(VEHICLE_FILE_ID,OPEN,&veh_key2,ISINPUT | ISMANULOCK,0);	/* open CALL db for read only */
  if (ccode<0) {
    printf(catgets(REP_catd, 8, 1, "Error OPENing file %d iserrno %d\n"),VEHICLE_FILE_ID,iserrno);
    return(-1);
  }	/* end if ccode */
  
  ccode=db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISGTEQ,0);
  while (ccode>=0 && vehrec->fleet==fleet_id){ /* continue while matching fleet */
    cab_no=vehrec->nbr;
    cabs_in_fleet++;
    while ( ccode>=0 && cab_no==vehrec->nbr ) /* continue while matching vehicle number */
      ccode=db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISNEXT,0);	/* get next record */
  }	/* end of matching fleet_id */
  ccode=db(VEHICLE_FILE_ID,CLOSE,&veh_key2,0,0);
#ifdef DEBUG
  printf("cabs_in_fleet %d \n",cabs_in_fleet);
#endif
  /* each fleet uniquely aggregates zones into super zones */
  /* define the zonerec structure */
  zonerec =(struct cisam_zn*)req_buf;	/* to cast req_buf */
  zonerec->fleet = fleet_id;
  zonerec->nbr = 0; /* for every pickup zone */
  next_super_zone=0; /* is a pointer into the sorted super zone array */
  
  /* open the cisam ZONE file, using the composite key1, fleet & nbr */
  ccode=db(ZONE_FILE_ID,OPEN,&zn_key1,ISINPUT | ISMANULOCK,0);	/* open CALL db for read only */
  if (ccode<0) {
    printf(catgets(REP_catd, 8, 1, "Error OPENing file %d iserrno %d\n"),ZONE_FILE_ID,iserrno);
    return(-1);
  }	/* end if ccode */
  ccode=db(ZONE_FILE_ID,READ_KEYS,&zn_key1,ISGTEQ,0);
  while (ccode>=0 && zonerec->fleet==fleet_id && zonerec->nbr<max_super_zone) { 
#ifdef DEBUG
    printf("superzone: type %c zonerec %d name %s\n", zonerec->type,zonerec->nbr,zonerec->desc);
#endif
    /* 1 is a stand, 2 is a zone */
    if (zonerec->type == '1' || zonerec->type == '2') 
      zone_to_super[zonerec->nbr] = zonerec->superzn_nbr;	/* save the super zone */
    else if (zonerec->type == '3') { /* 3 is a superzone */
      indx=0; 
      while (indx<next_super_zone && super_zones_sorted[indx] < zonerec->nbr) indx++;
#ifdef DEBUG
      printf("superzone: next_super %d indx %d sorted %d zonerec %d\n",next_super_zone,indx, super_zones_sorted[indx], zonerec->nbr);
#endif
      
      if (indx == next_super_zone) {
	/* add the super zone to the end of the list */
	super_zones_sorted[next_super_zone]=zonerec->nbr;
	strcpy(super_zones_names[next_super_zone],zonerec->desc);
	super_zones_sorted[++next_super_zone]=0;
      }	/* end if new super zone */
      else if ( super_zones_sorted[indx] != zonerec->nbr) {
	/* add the superzone sorting the superzone number and name lists */
	while (indx<next_super_zone) {
	  indx2=super_zones_sorted[indx];
	  strcpy(buffer,super_zones_names[indx]);
	  super_zones_sorted[indx]=zonerec->nbr;
	  strcpy(super_zones_names[indx],zonerec->desc);
	  zonerec->nbr=indx2;
	  strcpy(zonerec->desc,buffer);
	  indx++;
	}	/* end while */
	super_zones_sorted[++next_super_zone]=0;
      }	/* end else if no match */
      
    }	/* end else if a superzone */
    
    /* get the next zone record */
    ccode=db(ZONE_FILE_ID,READ_KEYS,&zn_key1,ISNEXT,0);
  }	/* end while all zones in zonerec */
  ccode=db(ZONE_FILE_ID,CLOSE,&zn_key1,ISGTEQ,0);
  
  /* zero 10,000 cab flags, one bit per hour of the day */
  /* each bit 0 .. 23 represents an hour and is 1 if and only if this cab received a call that hour */
  for (indx=0; indx<no_of_cabs; indx++) cab_marker[indx]=0;
  
  /* initialize the shiftrec structure */
  shiftrec=(struct cisam_cl*)req_buf;				/* to cast req_buf */
  shiftrec->fleet = fleet_id;
  shiftrec->pckup_zone = 1; 					/* for every pickup zone */
  shiftrec->due_date_time = 0; 
  
  ccode = 0;
  /* open the cisam SHIFT file, using the composite key3, fleet & pckup_zone */
  db_open(SHIFT_FILE_ID, ISINOUT | ISMANULOCK );	/* open CALL db for read only */
  if (ccode<0) {
    printf(catgets(REP_catd, 8, 1, "Error OPENing file %d iserrno %d\n"),SHIFT_FILE_ID,iserrno);
    return(-1);
  }	
  
  printedlines = LINESPERPAGE+1; 					/* read all the previous shift calls */
  ccode=db(SHIFT_FILE_ID,READ_KEYS,&sh_key3,ISGTEQ,0);
  while (ccode>=0 && shiftrec->fleet==fleet_id) { 	/* continue while matching fleet */
    
    zone_no=shiftrec->pckup_zone;				/* keep matching cab number */
    while ( ccode>=0 && zone_no==shiftrec->pckup_zone ) { 	/* continue while matching zone number */
      if ((shiftrec->due_date_time) && (shiftrec->veh_nbr > 0) && (shiftrec->close_date_time > 0)) {			/* this record was dispatched */
	
	tt=localtime( &(shiftrec->due_date_time) ); /* sum all accumulators at the record level */
	/* report # 7 */	if(shiftrec->veh_nbr != 0)
	  cab_marker[shiftrec->veh_nbr] |= hour_mask[tt->tm_hour];
	
	/* the hour counters are used in reports 9.1.3.6, 9.1.4.1, 9.1.4.2, and 9.1.4.3 */
	all_zone_hr_count[zone_no][tt->tm_hour]++; /* counters for max_super_zone zones, 24 hours */
	all_zone_hr_count[zone_no][24]++;		/* total calls for this zone */
	
	/* report # 8 */
	/* Count four types of calls */
	if ( shiftrec->extended_type[0] == PASS )
	{
		if (shiftrec->extended_type[1] == 'D')
			zone_info[zone_no][1] ++;
		else
	        	zone_info[zone_no][0]++;
	}
	else if ( shiftrec->extended_type[0] == DELIV )
	  zone_info[zone_no][2]++;
	else if ( shiftrec->extended_type[0] == TYPEX )
	  zone_info[zone_no][3]++;
	else if ( shiftrec->extended_type[0] == WAKEUP )
	  zone_info[zone_no][4]++;
	
	/* Time calls are TIMEC, Subcription calls are SUBS */
	/* Now calls = Total calls  - TIMEC - SUBS */
	if ( strchr(shiftrec->extended_type,TIMEC) ) 	zone_info[zone_no][4]++;
	else if ( strchr(shiftrec->extended_type,SUBS))	zone_info[zone_no][5]++;
	
	/* accumulators for reports 9.1.4.1 (index 0), 9.1.4.2 (index 1), and 9.1.4.3 (index 2) */
	
	/* Only count calls accepted by a vehicle */
	if (shiftrec->dispatch_entry_tm > 0)
	  {
	    
	    accepted_zone_hr_count[zone_no][tt->tm_hour]++; /* for max_super_zone zones, 24 hours */
	    accepted_zone_hr_count[zone_no][24]++;		/* total calls for this zone */
	    
	    /* report # 17: dispatched - call received times */
	    if ((indx2=(shiftrec->dispd_date_time - shiftrec->dispatch_entry_tm))<0 )
	      indx2=0;
	    if(shiftrec->dispatch_entry_tm <= 0)
	      indx2=0;
	    zone_hr_accum[zone_no][tt->tm_hour][0] += indx2;
	    zone_hr_accum[zone_no][24][0] += indx2;
	    
	    /* report # 18: customer picked up - call received times */
	    if ((indx2=(shiftrec->pckup_date_time - shiftrec->dispatch_entry_tm))<0 )
	      indx2=0;
	    if(shiftrec->dispatch_entry_tm <= 0)
	      indx2=0;
	    zone_hr_accum[zone_no][tt->tm_hour][1] += indx2;
	    zone_hr_accum[zone_no][24][1] +=  indx2;
	    
	    /* report # 19: customer delivered - picked up times */
	    if ((indx2=(shiftrec->close_date_time - shiftrec->pckup_date_time))<0 )
	      indx2=0;
	    if(shiftrec->pckup_date_time <= 0)
	      indx2=0;
	    zone_hr_accum[zone_no][tt->tm_hour][2] += indx2;
	    zone_hr_accum[zone_no][24][2] +=  indx2;
	  }
	
      }
      ccode=db(SHIFT_FILE_ID,READ_KEYS,&sh_key3,ISNEXT,0);	/* get next record */
    }	
    /* sum the zones within the super zone and grand totals */
    for (indx=0;indx<25;indx++) {
      if (zone_no < 1000) {
      all_super_zone_hr_count[zone_to_super[zone_no]][indx] += all_zone_hr_count[zone_no][indx];
      all_total_zone_hr_count[indx] += all_zone_hr_count[zone_no][indx];
      accepted_super_zone_hr_count[zone_to_super[zone_no]][indx] += accepted_zone_hr_count[zone_no][indx];
      accepted_total_zone_hr_count[indx] += accepted_zone_hr_count[zone_no][indx];
      }
    }
  }
  ccode=db(SHIFT_FILE_ID,CLOSE,&sh_key3,ISNEXT,0);
  
  /* print five reports per fleet */
  print_report_6(fleet_id,start_date,start_time,end_date,end_time,fleet_name);
  
  return(0);
}	

/*====================================================================
= initialize() -
=	zero all accumulators
=====================================================================*/
initialize()
{
	register int zone, indx;

	/* each mask element has a 1 in the bit for that hour */
	for (indx=0; indx<24; indx++) hour_mask[indx] = (1<<indx);

#ifdef DEBUG
printf("hour masks ");
for (indx=0; indx<24; indx++)  printf("%d",hour_mask[indx]);
#endif

	/* initialize the sorted super zones array as zero, NULL */

	for (indx=0; indx<25; indx++)
	  {
	    all_total_zone_hr_count[indx]=(long) 0;	/* report total counters */
	    accepted_total_zone_hr_count[indx]=(long) 0;	/* report total counters */
	  }

	for (indx=0; indx<6; indx++)
	  total_zone_info[indx]=(long) 0;

	/* zero all zone counters */
	for (zone=0; zone<max_super_zone; zone++) {	/* for each zone */
		for (indx=0; indx<25; indx++) {	/* for every hour */
			all_zone_hr_count[zone][indx] = 0; 
			all_super_zone_hr_count[zone][indx]=0;	/* super zone counters */
			accepted_zone_hr_count[zone][indx] = 0; 
			accepted_super_zone_hr_count[zone][indx]=0;	/* super zone counters */
			zone_hr_accum[zone][indx][0] = 0;
			zone_hr_accum[zone][indx][1] = 0;
			zone_hr_accum[zone][indx][2] = 0;
		}	/* end for every hour */
		for (indx=0; indx<6; indx++) /* for each type of zone */
			zone_info[zone][indx] = 0;
		zone_to_super[zone] = 0;
		super_zones_sorted[zone] = 0;
		super_zones_names[zone][0] = '\0';
	}	/* end for every zone */
	return(0);
}	/* end initialize */

/*====================================================================
= print_report_6() -
=	manages five report printouts for reports:
=		9.1.3.6, 9.1.3.8, 9.1.4.1, 9.1.4.2, 9.1.4.3
=	Each report consists of the following parts:
=	report heading, zone detail lines, super_zone subtotals, grand_totals,
=	super_zone summary information, and super_zone maps of contained zones.
=====================================================================*/
print_report_6(fleet_id,start_date,start_time,end_date,end_time,fleet_name)
char fleet_id;
char *start_date;
char *start_time;
char *end_date;
char *end_time;
char *fleet_name;
{
	int super_zone;
	register int zone_no;
	int report, report_no;
	int indx;
	char title2[60];
	char *report_name;
	char 	path_name[81];				/* Holds name of report file */
	char report_title[64];

	for (report=0; report<5; report++) { 	/* for each report */
		total_time = total_entries = times = 0;

		switch (report) {
		case 0:	/* report 9.1.3.6 zones by hour */
			report_no=6;
			sprintf(title2,catgets(REP_catd, 8, 2, "CALL DISTRIBUTION by zone by due time"));
			break;
		case 1:	/* report 9.1.3.8 zone call type information */
			report_no=8;
			sprintf(title2,catgets(REP_catd, 8, 3, "CALL DISTRIBUTION by call type by zone"));
			break;
		case 2:	/* report 9.1.4.1 zone average dispatched calls */
			report_no=17;
			sprintf(title2,catgets(REP_catd, 8, 4, "AVERAGE DISPATCH TIME by zone by time (in minutes)"));
			break;
		case 3:	/* report 9.1.4.2 zone average pick up times */
			report_no=18;
			sprintf(title2,catgets(REP_catd, 8, 5, "AVERAGE PICKUP TIME by zone by time (in minutes)"));
			break;
		case 4:	/* report 9.1.4.3 zone average passenger delivery time */
			report_no=19;
			sprintf(title2,catgets(REP_catd, 8, 6, "AVERAGE FARE DURATION TIME by zone by time (in minutes)"));
			break;
		default:
			break;
		}	/* end switch */

		/* get and open the next report file */
		report_name = get_rp_name(fleet_id,report_no,end_date);
		sprintf(path_name, "%s/%s", REPORT_DATA, report_name);
		fileptr = fopen(path_name,"w");
		if (fileptr == NULL)
		  {
		    fprintf(stderr, "%s %s\n", catgets(REP_catd, 13, 1, "Can't write to file:"), path_name);
		    continue;
		  }

#ifdef DEBUG
printf("print_report: report %d  report_no %d end_date %d report_name %s\n", report,report_no,end_date,report_name);
#endif
		/* zero hourly counters */
		for (indx=0; indx<25; indx++) total_zone_hr_accum[indx]=(long) 0;

		pagenumber=1;		/* printed page counter */
		printedlines = 0;
		print_heading_6(report,"     ",fleet_id,start_date,start_time,end_date,end_time,fleet_name);

		super_zone=0;	/* 0 is NOT a valid super zone id */
		if (! super_zones_sorted[super_zone]) super_zone++;
		while (super_zones_sorted[super_zone]) { /* for each super zone */

#ifdef DEBUG
fprintf(fileptr,"\n");
for (indx=21; indx<24; indx++) fprintf(fileptr," %4.1f",(float) total_zone_hr_accum[indx]);
for (indx=0; indx<21; indx++)  fprintf(fileptr," %4.1f",(float) total_zone_hr_accum[indx]);
#endif
			/* print all zone records within this super zone */
			zone_no=0;	/* find the first zone in the super zone */
			while (zone_no<max_super_zone && 
				zone_to_super[zone_no] != super_zones_sorted[super_zone]) zone_no++;
			super_accum=zone_no;	/* points to the first zone with the current super zone */
			while (zone_no<max_super_zone && zone_to_super[zone_no] == super_zones_sorted[super_zone]) { 

				/* for all zones in this super zone */
				if (printedlines>=LINESPERPAGE)
				  {
				    print_heading_6(report,"     ",fleet_id,start_date,start_time,end_date,end_time,fleet_name);
				    printedlines = 0;
				  }
				print_record_6(report,zone_no);			/* print the transaction in buf */

				/* get the next zone */
				zone_no++;
				while (zone_no<max_super_zone && 
					zone_to_super[zone_no] != super_zones_sorted[super_zone]) zone_no++;
			}	/* until next_zone is zero */

			/* print the super zone sub-totals */
			print_super_zone(report,super_zones_sorted[super_zone],0); /* 0 = print dashed lines */

			super_zone++;
		}	/* while super_zone increment */
		/* print the grand totals */
		print_grand_total(report);

		/* print super zone summary table */
		print_supersum(report,fleet_id,start_date,start_time,end_date,end_time,fleet_name);

		/* print zone into super zone mapping table */
		print_tran_table(report,fleet_id,start_date,start_time,end_date,end_time,fleet_name);
		/* print super zone totals */
		print_ending(fileptr,fleet_id,title2);
		fclose(fileptr);

	}	/* end for report */
	return(0);
}	/* end print_report */

/*====================================================================
= print_heading_6() -
=	Prints the field definitions for this report.
=====================================================================*/
print_heading_6(report,column_type,fleet_id,start_date,start_time,end_date,end_time,fleet_name)
int report;
char *column_type;
char fleet_id;
char *start_date;
char *start_time;
char *end_date;
char *end_time;
char *fleet_name;
{
	char title2[60];
	sprintf(title2,"0        1         2         3         4         5");

	/* form feed */
	if (pagenumber>1) putc('\f',fileptr);
	putc('\n',fileptr);

	/* print the two line heading. update the page counter */
	switch (report) {	/* for each report */
	case 0:	
		sprintf(title2,catgets(REP_catd, 8, 7, "    CALL DISTRIBUTION by zone by due time"));
		break;
	case 1:	
		sprintf(title2,catgets(REP_catd, 8, 8, "    CALL DISTRIBUTION by call type by zone"));
		break;
	case 2:	
		sprintf(title2,catgets(REP_catd, 8, 4, "AVERAGE DISPATCH TIME by zone by time (in minutes)"));
		break;
	case 3:	
		sprintf(title2,catgets(REP_catd, 8, 5, "AVERAGE PICKUP TIME by zone by time (in minutes)"));
		break;
	case 4:	
		sprintf(title2,catgets(REP_catd, 8, 6, "AVERAGE FARE DURATION TIME by zone by time (in minutes)"));
		break;
	default:
		break;
	}	/* end switch */

	report_heading(seconds,start_date,start_time,end_date,end_time,fleet_name,title2,&pagenumber,fileptr);

/* seperator line */
fprintf(fileptr,"====================================================================================================================================\n");

	switch (report) {	/* for each report */
	case 0:	
fprintf(fileptr,catgets(REP_catd, 8, 9, "%5s TOTAL           MIDNIGHT                       MORNING                       NOON                         AFTERNOON        \n"),column_type);
fprintf(fileptr,catgets(REP_catd, 8, 10, "ZONE  CALLS    10   11   12    1    2    3    4    5    6    7    8    9   10   11   12    1    2    3    4    5    6    7    8    9\n"));
		break;
	case 2:	
	case 3:	
	case 4:	
fprintf(fileptr,catgets(REP_catd, 8, 11, "%5s                 MIDNIGHT                       MORNING                       NOON                         AFTERNOON        \n"),column_type);
fprintf(fileptr,catgets(REP_catd, 8, 12, "ZONE  AVERAGE  10   11   12    1    2    3    4    5    6    7    8    9   10   11   12    1    2    3    4    5    6    7    8    9\n"));
		break;
	case 1:	
fprintf(fileptr,catgets(REP_catd, 8, 13, "ZONE   Pass. + AB  + Delivery + Xtra + Wakeup ==> Total Calls  Percent of Total <== Now Calls + Time Calls + Subscription Calls\n"));
		break;
	default:
		break;
	}	/* end switch */	

/* seperator line */
fprintf(fileptr,"====================================================================================================================================\n");
	return(0);
}	/* end of print_heading */

/*====================================================================
= print_record_6() -
=	Prints the lines of the current call record
=	that have alphanumeric characters.
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=
=====================================================================*/
print_record_6(report,zone_no)
int report, zone_no;
{
	register int indx;
	int report_indx;
	int zone_info_total=0;
	float temp_float;

	/* printedlines must be incremented BEFORE returning from this procedure */
	switch (report) {	/* for each report */
	case 0:	/* report 9.1.3.6 - counters */
		/* print zone number */
		fprintf(fileptr,"%4d  ",zone_no);	
		/* print shift total for this zone */
		fprintf(fileptr,"%5d ", all_zone_hr_count[zone_no][24]); /* shift total for zone */
		/* print late hours first */
		for (indx=21; indx<24; indx++) {
			if ( all_zone_hr_count[zone_no][indx] )
			  fprintf(fileptr," %4d", all_zone_hr_count[zone_no][indx]); 
			else
			  fprintf(fileptr,"     "); 
		}	/* end for */
		/* print hourly totals from 1am to 11pm */
		for (indx=0; indx<21; indx++) {
			if ( all_zone_hr_count[zone_no][indx] )
			  fprintf(fileptr," %4d",all_zone_hr_count[zone_no][indx]); 
			else
			  fprintf(fileptr,"     "); 
		}	/* end for */
		break;


	case 1:	/* report 9.1.3.8 - zone information is printeded as counters */
		/* print zone number */
		fprintf(fileptr,"%4d ",zone_no);	
		/* print information counters */
		for (indx=0; indx<4; indx++) {
			zone_info_total += zone_info[zone_no][indx];	/* count total types of calls */
			if ( zone_info[zone_no][indx] ) fprintf(fileptr,"%6d",zone_info[zone_no][indx]); 
			else fprintf(fileptr,"      "); 
		}	/* end for */
		/* print the total calls for this zone */
		if ( zone_info_total ) fprintf(fileptr,"                %10d",zone_info_total); 
		else break;	/* no calls to print */

		/* Changed to percentage of total calls, see total_zone_hr_count */
		/* print the percentage of calls for this zone divided by the super zone */
		/* if ( all_super_zone_hr_count[zone_to_super[zone_no]][24] )
			fprintf(fileptr,"      %10.1f     ", 100.0 * (float) zone_info_total 
				/ (float) all_super_zone_hr_count[zone_to_super[zone_no]][24]); 	*/
		/* print the percentage of calls for this zone divided by the total calls for all zones */
		if ( all_total_zone_hr_count[24] )
			fprintf(fileptr,"      %10.1f     ", 100.0 * (float) zone_info_total 
				/ (float) all_total_zone_hr_count[24]); 	
		else fprintf(fileptr,"                     ");

		fprintf(fileptr," %10d",zone_info_total - zone_info[zone_no][4] - zone_info[zone_no][5]); 
		for (indx=4; indx<6; indx++) if ( zone_info[zone_no][indx] ) 
				fprintf(fileptr," %6d",zone_info[zone_no][indx]); 
			else fprintf(fileptr,"           "); 

		/* sum super zone totals and all zone totals */
		for (indx=0;indx<6;indx++) {
			/* sum all zone values into the grand total */
			total_zone_info[indx] += (long) zone_info[zone_no][indx];
			/* sum all zone values into the first zone from this super zone */
			if (zone_no != super_accum) zone_info[super_accum][indx] += zone_info[zone_no][indx];
		}	/* end for */
		break;

	case 2:	
	case 3:	
	case 4:	/* report 9.1.4.1, 2, and 3 - delta time accumulators are printed as averages */
		report_indx = report - 2;	/* information is in array cells 0, 1, and 2 */
		/* print zone number */
		fprintf(fileptr,"%4d  ",zone_no);	
		/* print shift total for this zone */
		if ( accepted_zone_hr_count[zone_no][24] ) 
			fprintf(fileptr,"%5.1f ",(float) zone_hr_accum[zone_no][24][report_indx]
				/ ( (float) accepted_zone_hr_count[zone_no][24]* 60.0) ); 
		else break;	/* should not divide by zero */

		/* print from 10pm to midnight */
		for (indx=21; indx<24; indx++) 
			if ( accepted_zone_hr_count[zone_no][indx] ) {
				temp_float =(float) zone_hr_accum[zone_no][indx][report_indx]
					/ ( (float) accepted_zone_hr_count[zone_no][indx]* 60.0); 
				if (temp_float<99.00)
				  /* print the correct precision */
				  fprintf(fileptr," %4.1f",temp_float);
				else fprintf(fileptr," ****");
			}
			else fprintf(fileptr,"     "); 
		/* print hourly totals from 12 midnight to 10pm */
		for (indx=0; indx<21; indx++) 
			if ( accepted_zone_hr_count[zone_no][indx] ) {
				temp_float =(float) zone_hr_accum[zone_no][indx][report_indx]
					/ ( (float) accepted_zone_hr_count[zone_no][indx]* 60.0); 
				if (temp_float<99.00)
				  /* print the correct precision */
				  fprintf(fileptr," %4.1f",temp_float);
				else fprintf(fileptr," ****");
			}
			else fprintf(fileptr,"     "); 

		/* sum all zone values into the first zone from this super zone */
		for (indx=0;indx<25;indx++) /* sum all zone values into the grand total */
			total_zone_hr_accum[indx] += (long) zone_hr_accum[zone_no][indx][report_indx];
#ifdef DEBUG
fprintf(fileptr,"\n");
for (indx=21; indx<24; indx++) fprintf(fileptr," %4.1f",(float) total_zone_hr_accum[indx]);
for (indx=0; indx<21; indx++)  fprintf(fileptr," %4.1f",(float) total_zone_hr_accum[indx]);
#endif
		/* sum all zone values into the first zone from this super zone */
		for (indx=0;indx<25;indx++) {
			if (zone_no != super_accum) 
			zone_hr_accum[super_accum][indx][report_indx] += zone_hr_accum[zone_no][indx][report_indx];
		}	/* end for */
		break;

	default:
		break;
	}	/* end switch */	

	putc('\n',fileptr);
	printedlines++;
	return(0);
}	/* end print_record */

/*====================================================================
= print_super_zone() -
=	Prints the super zone total lines.
=====================================================================*/
print_super_zone(report,super_no,flag_type)
int report;		/* the report numeber to generate */
int super_no;		/* the actual super zone number */
int flag_type;		/* 0 = detail level break, 1 = super zone report */
{
	register int indx;
	int super_info_total=0;
	float temp_float;

	if (! flag_type)
	fprintf(fileptr,"------------------------------------------------------------------------------------------------------------------------------------\n");

	switch (report) {	/* for each report */
	case 0:	
		/* print super zone total */
		fprintf(fileptr,"%4d  ",super_no);
		/* print shift total for this zone */
		if ( all_super_zone_hr_count[super_no][24] ) 
			fprintf(fileptr,"%5d ",all_super_zone_hr_count[super_no][24]); /* shift total for zone */
		else fprintf(fileptr,"     "); 
		/* print from 10pm to midnight */
		for (indx=21; indx<24; indx++) {
			if ( all_super_zone_hr_count[super_no][indx] ) 
				fprintf(fileptr," %4d",all_super_zone_hr_count[super_no][indx]); 
			else fprintf(fileptr,"     "); 
		}	/* end for */
		/* print hourly totals from 12 midnight to 10pm */
		for (indx=0; indx<21; indx++) {
			if ( all_super_zone_hr_count[super_no][indx] ) 
				fprintf(fileptr," %4d",all_super_zone_hr_count[super_no][indx]); 
			else fprintf(fileptr,"     "); 
		}	/* end for */
		break;

	case 1:	/* report 9.1.3.8 - zone information */
		for (super_accum=0; super_accum<max_super_zone && zone_to_super[super_accum] != super_no; super_accum++);
		/* print superzone number */
		fprintf(fileptr,"%4d ",super_no);	
		/* print information counters */
		for (indx=0; indx<4; indx++) {
			super_info_total += zone_info[super_accum][indx];	/* count total types of calls */
			if ( zone_info[super_accum][indx] ) fprintf(fileptr,"%6d",zone_info[super_accum][indx]); 
			else fprintf(fileptr,"      "); 
		}	/* end for */
		/* print the total calls for this zone */
		if (super_info_total) fprintf(fileptr,"                %10d",super_info_total); 
		else break;	/* no calls to print */

		/* print the percentage of calls for this zone divided by the super zone */
		if ( all_total_zone_hr_count[24] ) 	
			fprintf(fileptr,"      %10.1f     ",100.0 * (float)super_info_total/(float)all_total_zone_hr_count[24]);
		else fprintf(fileptr,"                     ");

		fprintf(fileptr," %10d",super_info_total - zone_info[super_accum][4] - zone_info[super_accum][5]); 
		for (indx=4; indx<6; indx++) {
			if ( zone_info[super_accum][indx] ) fprintf(fileptr," %10d",zone_info[super_accum][indx]); 
			else fprintf(fileptr,"           "); 
		}	/* end for */
		break;

	case 2:	
	case 3:	
	case 4:	/* must accumulate the super zone sub-totals as the zone detail lines are printed */
		report -= 2;	/* information is in array cells 0, 1, and 2 */
		/* the super zone accumulators are in the first zone record matching this super zone */
		for (super_accum=0; super_accum<max_super_zone && zone_to_super[super_accum] != super_no; super_accum++);

		/* print super zone total */
		fprintf(fileptr,"%4d  ",super_no);
		/* print shift total for this zone */
		
		if ( accepted_super_zone_hr_count[super_no][24] ) 
		{
			fprintf(fileptr,"%5.1f ",(float) zone_hr_accum[super_accum][24][report]
				/ ( (float) accepted_super_zone_hr_count[super_no][24]* 60.0) ); 
			total_time += zone_hr_accum[super_accum][24][report];
			total_entries += accepted_super_zone_hr_count[super_no][24]*60;
			times++;
		}

		else break;

		/* print from 10pm to midnight */
		for (indx=21; indx<24; indx++) {
			if ( accepted_super_zone_hr_count[super_no][indx] ) {
				temp_float = (float) zone_hr_accum[super_accum][indx][report]
					/ ( (float) accepted_super_zone_hr_count[super_no][indx]* 60.0); 
				if (temp_float<99.00)	/* print the correct precision */ fprintf(fileptr," %4.1f",temp_float);
				else fprintf(fileptr," ****");
			}
			else fprintf(fileptr,"     "); 
		}	/* end for */
		/* print hourly totals from 12 midnight to 10pm */
		for (indx=0; indx<21; indx++) {
			if ( accepted_super_zone_hr_count[super_no][indx] ) {
				temp_float = (float) zone_hr_accum[super_accum][indx][report]
					/ ( (float) accepted_super_zone_hr_count[super_no][indx]* 60.0); 
				if (temp_float<99.00)	/* print the correct precision */ fprintf(fileptr," %4.1f",temp_float);
				else fprintf(fileptr," ****");
			}
			else fprintf(fileptr,"     "); 
		}	/* end for */
		break;

	}	/* end switch */	

	putc('\n',fileptr);	/* end the printable line with a carriage return */
	if (! flag_type)
	fprintf(fileptr,"------------------------------------------------------------------------------------------------------------------------------------\n");
	else putc('\n',fileptr);
	printedlines += 3;
	return(0);
}	/* end print_super_zone */

/*====================================================================
= print_grand_total() -
=	Prints the super zone grand total.
=====================================================================*/
print_grand_total(report)
int report;		/* the report number to generate */
{
	register int indx, indx2;
	register hour_of_day;
	int cab_counter[24];
	int info_total;
	float temp_float;

	fprintf(fileptr,"====================================================================================================================================\n");

	switch (report) {	/* for each report */
	case 0:	
		/* print grand total */
		fprintf(fileptr,"%6s ",catgets(REP_catd, 8, 14, "Total:") ); /* shift total for zone */
		if ( all_total_zone_hr_count[24] )
		  fprintf(fileptr,"%5d",all_total_zone_hr_count[24]); 
		else
		  fprintf(fileptr,"     "); 

		/* print from 10pm to midnight */
		for (indx=21; indx<24; indx++) {
			if ( all_total_zone_hr_count[indx] )
			  fprintf(fileptr," %4d",all_total_zone_hr_count[indx]); 
			else
			  fprintf(fileptr,"     "); 
		}
		/* print hourly totals from 12 midnight to 10pm */
		for (indx=0; indx<21; indx++) {
			if ( all_total_zone_hr_count[indx] )
			  fprintf(fileptr," %4d",all_total_zone_hr_count[indx]); 
			else
			  fprintf(fileptr,"     "); 
		}
		putc('\n',fileptr);
		/* print percentages */
		/* must sum to 100 percent */
		fprintf(fileptr,"%12s",catgets(REP_catd, 8, 15, "Percent: 100") ); 

		/* print from 10pm to midnight */
		if ( all_total_zone_hr_count[24] ) {
			for (indx=21; indx<24; indx++) {
				fprintf(fileptr," %4.1f",100.0 * (float) all_total_zone_hr_count[indx] 
					/ (float) all_total_zone_hr_count[24] ); 
			}	/* end for */
			/* print hourly totals from 12 midnight to 10pm */
			for (indx=0; indx<21; indx++) 
				fprintf(fileptr," %4.1f",100.0 * (float) all_total_zone_hr_count[indx] 
					/ (float) all_total_zone_hr_count[24] ); 
		}	/* end if */

		putc('\n',fileptr);
		fprintf(fileptr,"====================================================================================================================================\n");

		/* count all busy cabs each hour */
		for (indx=0; indx<24; indx++) {
			cab_counter[indx]=0;
			hour_of_day= hour_mask[indx];
			/* scan all cabs counting all busy cabs this hour */
			for (indx2=0; indx2<no_of_cabs; indx2++)
				if (cab_marker[indx2] & hour_of_day) cab_counter[indx]++;
		}	/* sum all busy cabs for each hour */
	
		/* print cabs that were busy each hour */
		fprintf(fileptr,catgets(REP_catd, 8, 16, "Active Cabs:")); /* shift total for zone */
		/* print from 10pm to midnight */
		for (indx=21; indx<24; indx++)
			if ( cab_counter[indx] ) fprintf(fileptr," %4d",cab_counter[indx]); 
			else fprintf(fileptr,"     "); 
		/* print hourly totals from 12 midnight to 10pm */
		for (indx=0; indx<21; indx++) 
			if ( cab_counter[indx] ) fprintf(fileptr," %4d",cab_counter[indx]); 
			else fprintf(fileptr,"     "); 
		putc('\n',fileptr);
	
		/* print percentages */
		fprintf(fileptr,catgets(REP_catd, 8, 17, "Percent:    ") ); 
		/* print from 10pm to midnight */
		if ( cabs_in_fleet ) {
			for (indx=21; indx<24; indx++)
				fprintf(fileptr," %4.1f",100.0 * (float) cab_counter[indx] / (float) cabs_in_fleet); 
			/* print hourly totals from 12 midnight to 10pm */
			for (indx=0; indx<21; indx++) 
				fprintf(fileptr," %4.1f",100.0 * (float) cab_counter[indx] / (float) cabs_in_fleet); 
		}	/* end if */
		printedlines += 7;
		break;

	case 1:	/* must accumulate the super zone sub-totals as the zone detail lines are printed */
		/* print grand total */
		fprintf(fileptr,catgets(REP_catd, 8, 18, "Total:  ") ); /* shift total for zone */
		info_total = 0;
		for (indx=0; indx<4; indx++) {
			info_total += total_zone_info[indx];	/* count total types of calls */
			if ( total_zone_info[indx] ) fprintf(fileptr,"%7d   ", total_zone_info[indx]);
			else fprintf(fileptr,"          "); 
		}	/* end for */
		/* print the total calls for this zone */
		fprintf(fileptr,"%10d",info_total); 

		/* print the percentage of calls for this zone divided by the super zone */
		if (all_total_zone_hr_count[24]) 
			fprintf(fileptr,"      %10.1f     ", 100.0 * (float) info_total / (float) all_total_zone_hr_count[24]);
		else
			fprintf(fileptr,"                     ");

		/* print the three remaining classes of calls */
		fprintf(fileptr," %10d",info_total - total_zone_info[4] - total_zone_info[5]); 
		for (indx=4; indx<6; indx++) {
			if ( total_zone_info[indx] ) fprintf(fileptr," %10d",total_zone_info[indx]); 
			else fprintf(fileptr,"           "); 
		}	/* end for */
		putc('\n',fileptr);
		
		/* print percentages */
		fprintf(fileptr,catgets(REP_catd, 8, 19, "Percent:")); 
		if ( info_total ) {
			for (indx=0; indx<4; indx++) 
				fprintf(fileptr,"%7.1f   ",100.0 * (float) total_zone_info[indx] / (float) info_total); 
			fprintf(fileptr,"%10.1f",100.0 );	/* total calls */
			fprintf(fileptr,"      %10.1f",100.0 );	/* percent of total */

			/* print the three remaining classes of calls */
			fprintf(fileptr,"      %10.1f", 100.0 * 
				(float) (info_total - total_zone_info[4] - total_zone_info[5]) / (float) info_total); 
			for (indx=4; indx<6; indx++) 
				fprintf(fileptr," %10.1f", 100.0 * (float) total_zone_info[indx] / (float) info_total); 
		}	/* end if */
		printedlines += 4;
		break;
	case 2:	
	case 3:	
	case 4:	/* must accumulate the super zone sub-totals as the zone detail lines are printed */
		/* print grand total */
		if(total_entries != 0)
			fprintf(fileptr,catgets(REP_catd, 8, 20, "Total: %4.1f"),(float)total_time / (float)total_entries);
		else
			fprintf(fileptr,catgets(REP_catd, 8, 21, "Total:  0.0"));
		/* print from 10pm to midnight */
		for (indx=21; indx<24; indx++)
			if ( accepted_total_zone_hr_count[indx] ) {
				temp_float = (float) total_zone_hr_accum[indx]
				                      / ((float) accepted_total_zone_hr_count[indx] * 60.0); 
				if (temp_float<99.00)
				  /* print the correct precision */
				  fprintf(fileptr," %4.1f",temp_float);
				else
				  fprintf(fileptr," ****");
			}
			else fprintf(fileptr,"     "); 

		/* print hourly totals from 12 midnight to 10pm */
		for (indx=0; indx<21; indx++) 
			if ( accepted_total_zone_hr_count[indx] ) {
				temp_float = (float) total_zone_hr_accum[indx]
				                    / ((float) accepted_total_zone_hr_count[indx] * 60.0); 
				if (temp_float<99.00)
				  /* print the correct precision */
				  fprintf(fileptr," %4.1f",temp_float);
				else
				  fprintf(fileptr," ****");
			}
			else fprintf(fileptr,"     "); 
		printedlines += 4;
		break;
	}	/* end switch */

	putc('\n',fileptr);
	fprintf(fileptr,"====================================================================================================================================\n");
	return(0);
}	/* end print_grand_total */

/*====================================================================
= print_supersum() -
=	Prints the super zone sub totals and report totals.
=====================================================================*/
print_supersum(report,fleet_id,start_date,start_time,end_date,end_time,fleet_name)
char fleet_id;
char *start_date;
char *start_time;
char *end_date;
char *end_time;
char *fleet_name;
{
	/* print super zone summary table */
	register int super_no=0;
	if (! super_zones_sorted[super_no] ) super_no++; 
	
	/* print each super zone aggregate of zones */
	printedlines = 0;
	print_heading_6(report,catgets(REP_catd, 8, 22, "SUPER"),
			fleet_id,start_date,start_time,end_date,end_time,fleet_name);
	while (super_zones_sorted[super_no]) {
		/* for all zones in this super zone */
		if (printedlines>=LINESPERPAGE)
		  {
		    printedlines = 0;
		    print_heading_6(report,catgets(REP_catd, 8, 22, "SUPER"),
				    fleet_id,start_date,start_time,end_date,end_time,fleet_name);
		  }
		print_super_zone(report,super_zones_sorted[super_no],1); /* flag 1 supresses dashed lines */
		super_no++;
	}	/* end while super zone to print */
	print_grand_total(report);
	return(0);

}	/* end print_supersum */

/*====================================================================
= print_tran_table() -
=	For each report, is the same translation table of zones for superzones.
=====================================================================*/
print_tran_table(report,fleet_id,start_date,start_time,end_date,end_time,fleet_name)
int report;	/* the report is not used, but may be in future revisions */
char fleet_id;
char *start_date;
char *start_time;
char *end_date;
char *end_time;
char *fleet_name;
{
	register int super_zone, zone_no;
	register zone_counter;

	/* If the super zone map breaks on a page, then start with a new page. */
	putc('\n',fileptr);
	putc('\n',fileptr);
	printedlines += 2;
	for(super_zone=1; super_zones_sorted[super_zone]; super_zone++);	/* find count of super zones */
	if (printedlines+super_zone>LINESPERPAGE)  putc('\f',fileptr);	/* form feed */

	fprintf(fileptr,catgets(REP_catd, 8, 23, "Super-Zone Translation Chart:\n"));		/* print the translation map heading */
	fprintf(fileptr,catgets(REP_catd, 8, 24, "Super-Zone     ................................ Zones Included In This Super-Zone ...........................\n"));

	super_zone=0;
	if (! super_zones_sorted[super_zone] ) super_zone++; 
	while (super_zones_sorted[super_zone]) { /* for each super zone */

		zone_counter=0;
		/* the super_zone name will print even with no zones */
		fprintf(fileptr,"\n\n%-20s\n",super_zones_names[super_zone]);	/* the super zone name */
		fprintf(fileptr,"%6d  ",super_zones_sorted[super_zone]);	/* the super zone number */

		/* print all super zone totals */
		for (zone_no=0; zone_no<max_super_zone ; zone_no++)
			if (zone_to_super[zone_no] == super_zones_sorted[super_zone]) {
				/* may have to count columns per line */
				zone_counter++;
				if (zone_counter>ZONE_LIMIT) {	/* break the zone line */
					fprintf(fileptr,"\n        ");
					zone_counter=0;
				}	/* end if zone counter */
				fprintf(fileptr," %-3d",zone_no);
			}	/* end if super zone */

		super_zone++;
	}	/* while super_zone increment */
	fprintf(fileptr,"\n");
	return(0);

}	/* end print_tran_table */

#if 0
/*====================================================================
= major_debug() -
=	Prints arrays to the standard output. 
=====================================================================*/
major_debug(option,zone_no)
int option;
int zone_no;
{
	register int i, j;
	switch (option) {
	case 5:	/* print accumulators */
		printf(catgets(REP_catd, 8, 25, "\n\nPrinting accumulators:\n"));
		printf(catgets(REP_catd, 8, 26, "Zone %3d: hours 0..23 and total\n"),zone_no);
		for (j=0;j<25;j++) printf("%d:%d ",j,zone_hr_accum[zone_no][j][0]);
		break;

	case 4:
		printf(catgets(REP_catd, 8, 27, "\n\nPrinting counters:\n"));
		printf(catgets(REP_catd, 8, 26, "Zone %3d: hours 0..23 and total\n"),zone_no);
		for (j=0;j<25;j++)  printf("%d:%d ",j,zone_hr_count[zone_no][j]);
		break;

	case 3:
		printf(catgets(REP_catd, 8, 28, "\n\nPrinting extended type information:\n"));
		printf(catgets(REP_catd, 8, 29, "Zone %3d: items 0..4 and total\n"),zone_no);
		for (j=0;j<6;j++)  printf("%d:%d ",j,zone_info[zone_no][j]);
		break;

	case 2:
		printf(catgets(REP_catd, 8, 30, "\n\nPrinting zone:super map:\n"));
		for (i=100;i<120;i++)  printf("%d:%d ",i,zone_to_super[i]);
		break;

	case 1:
		printf(catgets(REP_catd, 8, 31, "\n\nPrinting super zone sorted\n"));
		for (i=0;i<20;i++) printf("%d ",super_zones_sorted[i]);
		break;

	default: break;
	}	/* end switch */
	return(0);
}	/* end major debug */

#endif /* if 0 */
