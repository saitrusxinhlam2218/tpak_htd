static char sz__FILE__[]="@(#)print_end.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  print_end.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:22:05
 * @(#)  Last delta: 12/14/89 at 17:11:28
 * @(#)  SCCS File: /taxi/sccs/s.print_end.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include "rep_strdefs.h"

/*====================================================================
= print_ending() -
=	Completes the current report.
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=
=====================================================================*/
print_ending(fileptr,fleet_id,report_title)
FILE *fileptr;
char fleet_id;
char *report_title;
{
	int i;

	/* a few blank lines */
	putc('\n',fileptr);
	for(i=1; i< (112 - strlen(report_title))/2 ;i++)
		fprintf(fileptr,"=");	
        fprintf(fileptr, catgets(REP_catd, 12, 1, " END OF %s FOR FLEET %c "),
                report_title,fleet_id);
	for(i=1; i< (112 - strlen(report_title))/2 ;i++)
		fprintf(fileptr,"=");	
	putc('\n',fileptr);
	return(0);
}	/* end of print_ending */
