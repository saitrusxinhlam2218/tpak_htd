static char sz__FILE__[]="@(#)ui_prt_err.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_prt_err.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:51:25
 * @(#)  Last delta: 12/2/94 at 18:28:14
 * @(#)  SCCS File: /taxi/sccs/s.ui_prt_err.c
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include "ui_error.h"

#define		ERROR_ROW	(LINES-1)
#define		ERROR_COL	0

extern char *ui_errlist[];
short clear_err_flag=0; 	/* set if last line contains an error msg */
extern curses_set;

/* prt_error: prints an error message on the 22nd line of the screen,
 *		in reverse video
 */
prt_error(error_no, format, p1, p2, p3, p4, p5, p6, p7, p8)
int error_no;	/* ui error # */
char *format;	/* format of the error msg */
long p1, p2, p3, p4, p5, p6, p7, p8;
{
	char new_format[132];	/* buffer for complete format of error msg */
	char prt_string[200];	/* buffer for string to be printed */
	long now_time;
	static char last_prt_string[200];
	static long last_time = 0;

	if (error_no >= 0 && error_no <= UI_LAST_ERR) {
	    
	    /* add the %s for ui_errlist */
	    strcpy(new_format,"%s");
	    if (format && *format) {
		strcat(new_format, " ");
		strcat(new_format, format);
	    }
	    
	    sprintf(prt_string, new_format, ui_errlist[error_no], p1, p2, p3, p4, p5, p6, p7, p8);
	    
	    /* Guarantee end of string */
	    prt_string[131] = '\0';
	}
	
	/* Don't use ui_errlist */
	else if (error_no == UI_NO_MSG) {
	    sprintf(prt_string, format, p1, p2, p3, p4, p5, p6, p7, p8);
	}

	/* error_no out of bounds */
	else
	    return;

	/* If curses hasn't come up yet, print to stderr */
	if (!curses_set) {
	    fprintf(stderr, "%s\n", prt_string);
	    clear_err_flag = 1;
	    return;
	}

	attrset(A_REVERSE);
	mvaddstr(ERROR_ROW,ERROR_COL,prt_string);
	attrset(0);

	/* Erase the rest of error line */
	clrtoeol();
	clear_err_flag = 1;

	/* If last time an error was displayed was between
	   0 and 1 second, and not the same error as before,
	   wait 1 second for next error display. The checking of
	   the same error as before is to avoid the annoying
	   problem of a whole bunch of the same errors appearing
	   after another, with 1 second between each. */
	now_time = time(0);
	if (now_time - last_time <= 1 && strcmp(prt_string, last_prt_string) != 0)
	    sleep(1);

	beep();
	refresh();
	last_time = now_time;
	strcpy(last_prt_string, prt_string);
}

/* clear_error: clears the error message printed on the last line */
clear_error()
{
	clear_err_flag = 0;
	move(ERROR_ROW,ERROR_COL);
	clrtoeol();
}  /* end clear_error() */


