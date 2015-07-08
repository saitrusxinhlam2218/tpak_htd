static char sz__FILE__[]="@(#)cls.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  cls.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:32:36
 * @(#)  Last delta: 12/2/94 at 11:19:48
 * @(#)  SCCS File: /taxi/sccs/s.cls.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>

/*
 * clear_scrn() is used by programs that don't use curses but
 * need to clear the screen.
 */
clear_scrn()
{
    static int first_time = 1;
    static int have_to_use_system_call;
    static char cls[80];
    char file_name[80];
    char system_call[100];
    FILE *fp;
    char c;
    int  i;


    /* Redirect the output of "clear" into a file.
       Read the file and use it as the string to
       clear the terminal. If unsuccesful in getting
       the clear string, just use the "clear" command
       all the time. */

#ifdef FOO
    if (first_time) {

	sprintf(file_name, "/tmp/%d%ld", getuid(), time(0));
	sprintf(system_call, "clear > %s", file_name);
	system(system_call);

	if ((fp = fopen(file_name, "r")) == NULL) {
	    have_to_use_system_call = 1;
	}
	else {
	    have_to_use_system_call = 0;
	    i = 0;
	    while ((c = fgetc(fp)) != EOF)
		cls[i ++] = c;
	    if (i == 0)
		have_to_use_system_call = 1;
	    else
		cls[i] = '\0';
	}
	fclose(fp);
	unlink(file_name);
	first_time = 0;
    }

#endif
    //    if (have_to_use_system_call)
	system("clear");
	//    else {
	//	printf(cls);
	//    }
    fflush(stdout);
}
