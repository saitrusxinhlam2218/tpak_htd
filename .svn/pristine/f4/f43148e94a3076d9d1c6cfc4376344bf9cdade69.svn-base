static char sz__FILE__[]="@(#)getactflts.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  getactflts.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:32:51
 * @(#)  Last delta: 12/2/94 at 11:34:24
 * @(#)  SCCS File: /taxi/sccs/s.getactflts.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "mads_types.h"
#include "mad_error.h"
#include "ex_key.h"		/* define external db keys */
#include "df_maint.h"		/* for defines */
#include "mads_isam.h"		/* for defines */
#include "path.h"
#include "language.h"

extern	char req_buf[];	/* buffer to cast as call structure */
extern	int db();

/*====================================================================
= get_act_fleets() -
=	Scans all active fleets.
=	Returns the number of active fleets and what they are.
=	What they are is returned with the argument.
=
=====================================================================*/
int get_act_fleets(fleet_id)
char fleet_id[];
{
	register int ccode, fleetno=0;
	struct	cisam_fl *fleetrec;

	fleetrec=(struct cisam_fl *)req_buf;						/* to cast req_buf */

	if(db(FLEET_FILE_ID,OPEN,&fl_key1,ISINOUT | ISMANULOCK,0) < 0){		/*  open the file */
		ERROR(' ',FLEET_FILE,"getactflts.c - cannot open file\n");
		return(-1);
	}

	fleetrec->nbr = 0;
	ccode=db(FLEET_FILE_ID,READ_KEYS,&fl_key2,ISEQUAL,0);
	while (ccode >=0) {
		if (fleetrec->active == YES) {
			if (fleetno >= FLEET_MAX) {
				ERROR(' ', "", "Fleet no. exceeds FLEET_MAX");
				db(FLEET_FILE_ID,CLOSE,&fl_key2,0,0);
				return(-1);
			}
			fleet_id[fleetno++] = fleetrec->id;
		}	
		ccode=db(FLEET_FILE_ID,READ_KEYS,&fl_key2,ISNEXT,0);
	}	

	db(FLEET_FILE_ID,CLOSE,&fl_key2,0,0);
	fleet_id[fleetno] = NULL;
	return(fleetno);
}
