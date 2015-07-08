/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: termid.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1997/08/09 18:08:22 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/sysadmin/termid.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: termid.c,v 1.2 1997/08/09 18:08:22 taxiadm Exp $";

#include <stdio.h>
#include <termio.h>
#include <sys/signal.h>

struct termio termsave, termstate;

#define blanks "                    "
/*====================================================================
= termid() -
= This main program determines what type of terminal is in use.
= Terminals currently supported are:
= wyse60, motorola tm220
=====================================================================*/
main()
{
	int time_out();
	char stringback[16];

	/* looking for a wyse60 with esc space */
	signal(SIGALRM,time_out);
	printf("\033\040\n");
	fflush(stdout);
	fflush(stdin);
	alarm(2);
	fgets(stringback,3,stdin);
	stringback[3]='\0';
	if (atoi(stringback)==60) {
		printf("\033~;"); /* put the terminal into vt100 personality */
		fflush(stdout);
		fflush(stdin);
		exit (6);
	}	/* end if wyse60 */

	else { 
	/* assume a motorola tm220 */
		fflush(stdout);
		fflush(stdin);
		exit(1);	/* make this the default terminal type */
		printf("\n\n\033[>c\n");
		signal(SIGALRM,time_out);
		alarm(2);
		fgets(stringback,10,stdin);
		stringback[6]='\0';
		/* the user interface program expects a tm220-w terminal */
		exit(1);
	}	/* end else */

}  /* end main */

/*====================================================================
= initialize()
=	Puts the terminal into raw mode. 
=	If several terminal types are supported, 
=	then the Motorola terminals are best identified in raw mode.
=====================================================================*/
initialize()
{
	/* put the terminal into raw mode, saving the original state */
	if (ioctl(0,TCGETA,&termsave) == -1) {
		printf("%sIOCTL ERROR: (termid).\n",blanks );;
		exit(-1);
	}	/* enf if */
	if (ioctl(0,TCGETA,&termstate) == -1) {
		printf("%sIOCTL ERROR: (termid).\n",blanks );;
		exit(-1);
	}	/* end if */
	termstate.c_lflag &= ~ICANON;
	termstate.c_lflag &= ~ECHO;
	termstate.c_cc[VMIN]  =  1;
	termstate.c_cc[VTIME] =  1;
	if (ioctl(0,TCSETA,&termstate)==-1)  exit(-1);

}	/* end initialize */

/*====================================================================
= time_out()
=	Processes the alarm. 
=	This stub returns to the next line in the calling proceedure.
=====================================================================*/
time_out() { }

/*====================================================================
= clean_up()
=	Restores the terminal state when the ioctl call was made.
=====================================================================*/
clean_up()
{
	ioctl(0,TCSETA,&termsave);
	return;
}	/* end close */
