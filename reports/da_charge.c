static char sz__FILE__[]="@(#)da_charge.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  da_charge.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:20:40
 * @(#)  Last delta: 12/14/89 at 17:00:03
 * @(#)  SCCS File: /taxi/sccs/s.da_charge.c
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
#include "path.h"

extern	char buf[];	/* for db put and get records       */
extern	char req_buf[];	/* buffer to cast as call structure */
extern	int db();
extern	char *get_rp_name();

struct	cisam_cl *shiftrec;

#define blanks "                    "
/*====================================================================
= da_charge()
=	Scans the shift call file, counting charge calls and all calls.
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=====================================================================*/
da_charge(fleet_id,start_date,start_time,start_dt_tm,end_date,end_time,ending_dt_tm,fleet_name)
char fleet_id;
char *start_date;
char *start_time;
long start_dt_tm;
char *end_date;
char *end_time;
long ending_dt_tm;
char *fleet_name;
{
	char buffer[264];
	char *report_name;
	char systembuff[256];
	char working_fleet;
	char *fl_name_ptr;		/* Points to fleet name */

	int ccode;		/* returned from proc calls */
	int fleetno;
	int report_no=15;
	register int charge_counter, call_counter;

	/* for each active fleet */

	/* make one pass through the shift table for each fleet */
	/* each pass counts the charge calls and all the calls for the fleet */
	/* initialize the shiftrec structure */
	shiftrec=(struct cisam_cl*)req_buf;	/* to cast req_buf */
	working_fleet = fleet_id;
	shiftrec->fleet = working_fleet;	/* first key field */
	shiftrec->due_date_time = 0;		/* second key field */
	call_counter=0;
	charge_counter=0;

	/* open the cisam SHIFT file, using the composite key2, fleet & due_dt_tm */
	ccode = 0;
	db_open(SHIFT_FILE_ID, ISINPUT | ISMANULOCK);	/* open CALL db for read only */

	if (ccode<0) {
		printf(catgets(REP_catd, 1, 1, "Error OPENing file %s iserrno %d\n"),SHIFT_FILE_ID,iserrno);
		return(-1);
	}

	ccode=db(SHIFT_FILE_ID,READ_KEYS,&sh_key2,ISGTEQ,0);
	while (ccode>=0 && shiftrec->fleet==working_fleet) { 
		/* continue while matching fleet */

		/* count all calls */
		call_counter++;
		/* non-NULL must have a charge number */
		if (strlen(shiftrec->charge_nbr)>0) {
			charge_counter++;
		}

		ccode=db(SHIFT_FILE_ID,READ_KEYS,&sh_key2,ISNEXT,0);	/* get next record */
	}	/* end of matching fleet_id */
	ccode=db(SHIFT_FILE_ID,CLOSE,&sh_key2,ISNEXT,0);

	report_name = get_rp_name(fleet_id,report_no,end_date);

	/* Replace spaces with underscores */
	fl_name_ptr = fleet_name;
	while(*fl_name_ptr != '\0') {
		if(*fl_name_ptr == ' ')
			*fl_name_ptr = '_';
		fl_name_ptr++;
	}

	sprintf(systembuff,"%s %c %s %s %ld \"%s\" %s %ld \"%s\" %d %d > %s/%s ",
		"sacego -q da_charg",fleet_id,start_date,start_time,start_dt_tm,
		end_date,end_time,ending_dt_tm,fleet_name,charge_counter,call_counter,REPORT_DATA,report_name);
	system(systembuff);

}
