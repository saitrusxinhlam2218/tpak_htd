static char sz__FILE__[]="@(#)da_usest.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  da_usest.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:21:19
 * @(#)  Last delta: 12/14/89 at 17:00:49
 * @(#)  SCCS File: /taxi/sccs/s.da_usest.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "rep_strdefs.h"
#include "path.h"

extern char *get_rp_name();

/*====================================================================
= da_usest()
=	Print user statistics.
=	Who:	Bruce Young
=	What:	Written		When: 20 August 1988
=====================================================================*/
da_usest (fleet_id,start_date,start_time,start_dt_tm,end_date,end_time,ending_dt_tm,fleet_name)
char fleet_id;
char *start_date;
char *start_time;
long start_dt_tm;
char *end_date;
char *end_time;
long ending_dt_tm;
char *fleet_name;
{
	int report_no = 3;	/* will print with leading zero */

	char *report_name;
	char systembuff[256];
	char *fl_name_ptr;		/* Points to fleet name */

	report_name = get_rp_name(fleet_id,report_no,end_date);
	/* Replace spaces with underscores */
	fl_name_ptr = fleet_name;
	while(*fl_name_ptr != '\0') {
		if(*fl_name_ptr == ' ')
			*fl_name_ptr = '_';
		fl_name_ptr++;
	}

	/* print the user statistics report */
	sprintf(systembuff,"%s %c %s %s %ld \"%s\" %s %ld \"%s\" > %s/%s ",
		"sacego -q da_usest",fleet_id,start_date,start_time,start_dt_tm,
		end_date,end_time,ending_dt_tm,fleet_name,REPORT_DATA,report_name);
	system(systembuff);
	/* run a brief ace program to compute daily user statistics */
	/* and a program to write the daily statistics to the statistics table */
/* 		sprintf(systembuff,"%s %c %s %s %ld \"%s\" %s %ld \"%s\" | %s ",
			"sacego -q da_usum",fleet_id,start_date,start_time,start_dt_tm,
			end_date,end_time,ending_dt_tm,fleet_name,
			"da_useput",fleet_id,start_date,start_time,start_dt_tm);
		system(systembuff);
*/
}	/* end of report_fleet */
