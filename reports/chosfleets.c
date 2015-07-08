static char sz__FILE__[]="@(#)chosfleets.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  chosfleets.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:20:30
 * @(#)  Last delta: 12/14/89 at 16:55:57
 * @(#)  SCCS File: /taxi/sccs/s.chosfleets.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <isam.h>
#include <malloc.h>
#include <sys/types.h>
#include <ctype.h>

#include "mads_types.h"
#include "ex_key.h"		/* define external db keys */
#include "df_maint.h"		/* for defines */
#include "mads_isam.h"		/* for defines */
#include "mad_error.h"		/* for defines */
#include "rep_strdefs.h"
#include "language.h"

extern	char req_buf[];	/* buffer to cast as call structure */
extern	int db();

#define blanks "                    "
/*====================================================================
= choose_fleets() -
=	Scans all active fleets.
=	Returns the number of active fleets,
=	optionally, the number of fleets choosen.
=	Who:	Bruce Young
=	What:	Written		When: 2 March 1988
=
=====================================================================*/
int choose_fleets(fleet_id,fleet_name,option,fleet_num)
char fleet_id[], *fleet_name[];
short fleet_num[];
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
	ccode=db(FLEET_FILE_ID,OPEN,&fl_key2,ISINPUT | ISMANULOCK,0);	/* open CALL db for read only */
	if (ccode<0) {
		printf(catgets(REP_catd, 2, 1, "Error OPENing file %s iserrno %d\n"),FLEET_FILE_ID,iserrno);
		exit(-1);
	}	/* end if ccode */

	fleetno=0;	/* count active fleets */
	/* define the key field, for all fleets start from 0 */
	fleetrec->nbr = 0;
	/* read the matching record */
	ccode=db(FLEET_FILE_ID,READ_KEYS,&fl_key2,ISEQUAL,0);
	while (ccode >=0) {

#ifdef DEBUG
	sprintf(trace_str,"fleet->active %c fleetrec->id %c fleetrec->name %s\n",
	fleetrec->active,fleetrec->id,fleetrec->name);
	TRACE(trace_str);
#endif

		if (Toupper(fleetrec->active) == YES) {
			/* choose only active fleets */
			fleet_id[fleetno]=fleetrec->id; /* save the fleet identifier */
			fleet_num[fleetno] = fleetrec->nbr;
			p1=(char *)malloc(21);
			strcpy(p1,fleetrec->name); /* save the fleet name */
			fleet_name[fleetno]=p1;
#ifdef DEBUG
	sprintf(trace_str,"fleet_id %c fleet_name %s\n",fleet_id[fleetno],fleet_name[fleetno]);
	TRACE(trace_str);
#endif
			fleetno++;
		}	/* end if fleetrec */
	
		/* if dispatching all fleet procedures, get the next matching record */
		ccode=db(FLEET_FILE_ID,READ_KEYS,&fl_key2,ISNEXT,0);
	}	/* end while */	

	/* close the FLEET file */
	ccode=db(FLEET_FILE_ID,CLOSE,&fl_key2,0,0);
	if (ccode<0) {
		printf(catgets(REP_catd, 2, 2, "Error CLOSEing file %d iserrno %d\n"),FLEET_FILE_ID,iserrno);
		printf("fleetrec->id %c \n",fleetrec->id);
		exit(-1);
	}	/* end if ccode */

	goodfleet=fleetno;
	/* choose the fleets to report */
	if (option) {
		printf(catgets(REP_catd, 2, 3, "%sThe following fleets are active: "),blanks);
		for (indx=0; indx<fleetno; indx++)  printf(" %c",fleet_id[indx]);
		printf(catgets(REP_catd, 2, 4, "\n\n%sChoose the fleets to report. "),blanks);
		printf(catgets(REP_catd, 2, 5, "<CR> to quit, * for all fleets: "));
		fflush(stdin);
		gets(iobuff);
		if (! (bufflen=strlen(iobuff)) ) return(0); /* return when no reply */

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
					fleet_num[goodfleet] = fleet_num[indx];
					fleet_name[goodfleet]=fleet_name[indx];
					goodfleet++;	/* count of fleets to report */
				}	/* end if */
		} /* end else */
	}	/* end if option */

	fleet_id[goodfleet]=NULL;
	fleet_name[goodfleet]= (char *) NULL;
	fleet_num[goodfleet] = 99;
#ifdef DEBUG
	sprintf(trace_str,catgets(REP_catd, 2, 6, "fleets chosen %s\n"), fleet_id);
	TRACE(trace_str);
#endif

	return(goodfleet);
}	/* end choose_fleet */
