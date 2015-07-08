static char sz__FILE__[]="@(#)au_spawn.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  au_spawn.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:20:24
 * @(#)  Last delta: 12/14/89 at 16:55:12
 * @(#)  SCCS File: /taxi/sccs/s.au_spawn.c
 *                                                                           *
 *****************************************************************************/

#include "path.h"

#define sleep_interval 5	/* sleep interval when no calls in seconds */

/*====================================================================
= make_trail() -
=	Kills all audit_trail processes.
=	When called withouot arguments, restarts audit_trail processes.
??????      bruce, it restarts w/arg; w/o args it just stops !!!       ????????
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=====================================================================*/
main(argc,argv)
int argc;	/* any argument will stop the audit_trail prpocess, without restarting */
char *argv[];
{
	char systembuffer[132];

	/* if run from the audit_trail process, then kills itself, hence must be run from a separate process */
	/* first any audit_trail process is stopped */
	sprintf(systembuffer,"kill -9 `ps ax|grep au_trail|grep -v grep|grep -v sh|awk '{print $1}'`");
        system(systembuffer);

	if (argc==1) exit (0);

	sprintf(systembuffer,"nohup %s %d >/dev/null &", AU_TRAIL, sleep_interval);
	system(systembuffer);
}	/* end of main */
