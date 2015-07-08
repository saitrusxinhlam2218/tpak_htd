static char sz__FILE__[]="@(#)de_autob.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  de_autob.c; Rel: 10.1.0.0; Get date: 4/18/95 at 17:01:31
* @(#)  Last delta: 12/2/94 at 12:01:55
* @(#)  SCCS File: /taxi/sccs/s.de_autob.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <malloc.h>

#include <isam.h>
#include <sys/types.h>
#include "machdep.h"
#include "mads_types.h"
#include "key.h"
#include "path.h"
#define DECLARING
#include "rep_strdefs.h"


/*====================================================================
= main parameter 
=	Reports all active vehicles for all active fleets.
=====================================================================*/
main(argc,argv)
int argc;
char *argv[];	
{
	register int indx;
	long seconds;			/* current date and time in seconds */
	int goodfleets;
	char fleet_id[FLEET_MAX+1];	/* plenty of fleets */
	char *fleet_name[FLEET_MAX+1];	/* will malloc room for each fleet name */
	char systembuff[133];
	char *fl_name_ptr;		/* Points to fleet name */
	short fleet_num[9];

	 init_locale();
	 REP_catd = init_catalog("de_drive");
        
	/* save, only once, the current date and time as seconds */
	seconds = time ((long *) 0);


	goodfleets=choose_fleets(fleet_id,fleet_name,1,fleet_num);

	if (goodfleets) {			/* report for each fleet */
		for (indx=0; indx<goodfleets; indx++) {
			fl_name_ptr = fleet_name[indx];
			while(*fl_name_ptr != '\0') {
				if(*fl_name_ptr == ' ')
					*fl_name_ptr = '_';
				fl_name_ptr++;
			}

			sprintf(systembuff,"sacego -q %s %c \"%s\"", DE_AUTOB,
				fleet_id[indx], fleet_name[indx]);

			puts("\n\t\t==================================================================\n");
			printf(catgets(REP_catd, 1, 2, "\t\t             PREPARING REPORT FOR FLEET %c\n"),fleet_id[indx]);

			system(systembuff);			/* report for one fleet */

			printf(catgets(REP_catd, 1, 3, "\t\t             REPORT FOR FLEET %c IS PRINTING\n"),fleet_id[indx]);
			puts(  "\t\t==================================================================\n");

		}
	}
	else {
		printf(catgets(REP_catd, 1, 4, "\n\t\tNO fleets will be reported.\n "));
		printf(catgets(REP_catd, 1, 5, "\t\tPlease, run this program later. <CR> to continue.."));
		fflush(stdin);
		gets(fleet_id);
		return(0);
	}

	cleanup_catalog(REP_catd);
	return(goodfleets);
}
