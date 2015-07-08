static char sz__FILE__[]="@(#)de_subsc.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  de_subsc.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:21:47
 * @(#)  Last delta: 12/14/89 at 17:01:51
 * @(#)  SCCS File: /taxi/sccs/s.de_subsc.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <isam.h>
#include <sys/types.h>
#include "machdep.h"
#include "mads_types.h"
#include "key.h"
#include "path.h"
#define DECLARING
#include "rep_strdefs.h"

extern  void local_date_time();

/*====================================================================
= main()
=	Reports all unassigned calls per fleet.
=====================================================================*/

main(argc,argv)
int argc;
char *argv[];	
{
	register int indx;
	int goodfleets;
	char systembuff[132];
	char fleet_id[FLEET_MAX+1];
	char *fleet_name[FLEET_MAX+1];	/* will malloc room for each fleet name */
	char *fl_name_ptr;		/* Points to fleet name */
	short fleet_num[9];

        /* initialize language environment */
        init_locale();
        REP_catd = init_catalog("de_subsc");

	CLEAR;
	puts("\n\n\t\t==================================================================\n");
	printf(  catgets(REP_catd, 1, 1, "\n\t\t             REPORTING SUBSCRIPTION CALLS FOR FLEETS\n\n"));

	goodfleets = choose_fleets(fleet_id,fleet_name,1,fleet_num);

	puts(  	"\n\t\t==================================================================\n");

	if (goodfleets) {
		/* report for each fleet */

		for (indx=0; indx<goodfleets; indx++) {
			fl_name_ptr = fleet_name[indx];
			while(*fl_name_ptr != '\0') {
				if(*fl_name_ptr == ' ')
					*fl_name_ptr = '_';
				fl_name_ptr++;
			}

			sprintf(systembuff,"sacego -q %s %c \"%s\" ", 
				DE_SUBSC, fleet_id[indx], fleet_name[indx]);

			puts("\n\t\t==================================================================\n");
			printf(catgets(REP_catd, 1, 2, "\t\t             PREPARING REPORT FOR FLEET %c\n"),fleet_id[indx]);

			system(systembuff);

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

        /* close language catalogues */
        cleanup_catalog(REP_catd);

	return(goodfleets);
}
