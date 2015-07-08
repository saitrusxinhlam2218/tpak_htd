#if ( ENGLISH )
static char szTP_LANG[]="@(#)LANG = English" ;
#elif ( DANISH  )
static char szTP_LANG[]="@(#)LANG = Danish" ;
#elif ( FINNISH )
static char szTP_LANG[]="@(#)LANG = Finnish" ;
#elif ( GERMAN )
static char szTP_LANG[]="@(#)LANG = German" ;
#elif ( FRENCH )
static char szTP_LANG[]="@(#)LANG = French" ;
#elif ( SWEDISH )
static char szTP_LANG[]="@(#)LANG = Swedish" ;
#endif

static char sz__FILE__[]="@(#)ui_main.c  10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  ui_main.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:51:01
* @(#)  Last delta: 12/2/94 at 18:03:25
* @(#)  SCCS File: /taxi/sccs/s.ui_main.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/


/* Make sure location constants are defined. Create a compile error if they
   are not defined or both are defined. */
#ifdef SCANDINAVIA
#ifdef NAMERICA
ERROR: "Both compile time constants SCANDINAVIA and NAMERICA are defined."
#endif
#else
#ifdef NAMERICA
#else
ERROR: "Compile time constant SCANDINAVIA or NAMERICA undefined."
#endif
#endif

#include <stdio.h>
#include <string.h>


#include "ui_def.h" 	/* user interface program definitions */
#include "path.h"
#include "language.h"
#define DECLARING
#include "ui_strdefs.h"
#include "comm_strdefs.h"
#include "writer_strdefs.h"
#include "enhance.h"
#include "ch_pipemsg.h"

int No_status_line = 0;
int No_time_update = 0;
int No_exception_beep = 0;
int FlatTrace = 0;
int Kill_parent_when_done = 0;
int Dont_check_for_dispatch = 0;

long glEnhSwitch; /* Enhancement switch for enhancement checking */
int ch_pipe_hndl;

char LAH[2*sizeof(CALLHND_PIPE_MSG)];
#define Log_File "ui_log"
char szProcName[] = "ui";
char szLogMsg[256];
cBOOL bSilent = 0;
FILE *log_fd;

/* Have a variable that we can do a "strings ui | grep 'version'
   and find out what version of the ui we have running. */
#ifdef SCANDINAVIA
char *country_name = "version:scandinavia";
#endif
#ifdef NAMERICA
char *country_name = "version:north america";
#endif
int taxipak_process = UI;
time_t mads_time = 0;  /* for the call library */
int num_tc, free_calls_nbr, pkt_mem, free_call_msg_nbr;  /* for the LIST library */
extern char sTelnetHost[];
extern int ui_acdcli;

/* ============================================================================================================================	*
 * main.c: 	The main function for the user interface program. The program calls init(), process() and cleanup() 		*
 * WHO: Naveed Cochinwala	WHAT: Written		WHEN: 1988
 * ============================================================================================================================	*/
main(argc, argv)
int argc;
char **argv;
{
        int  i;
	char sLocale[80];
        char nlspath[80];
        static char put_nlspath[80];
        int  putenv();
	
        for (i = 1; i < argc; i ++)
	  {
	    /* Don't write to status line of terminal */
	    if (strcmp(argv[i], "-nsl") == 0)
	      No_status_line = 1;

	    if (strcmp(argv[i], "-nbeep") == 0)
	      No_exception_beep = 1;

	    if (strcmp(argv[i], "-ftrace") == 0)
	      FlatTrace = 1;

            if (strcmp(argv[i], "-acdcli") == 0)
              {
                strcpy( sTelnetHost, argv[i+1] );
                ui_acdcli = 1;
              }
	
	    /* Don't update the time every minute */
	    else if (strcmp(argv[i], "-ntu") == 0)
	      No_time_update = 1;

	    /* Kill parent process when done to keep users from
	       accessing the unix shell */
	    else if (strcmp(argv[i], "-die") == 0)
	      Kill_parent_when_done = 1;

	    /* For testing without dispatch running */


	      else if (strcmp(argv[i], "-nd") == 0)
	      Dont_check_for_dispatch = 1;

	  }
	/*
	 * Initialize the enhancement switch
	 */
	 if (enh(&glEnhSwitch) < 0)
	 {
		 /* Error logging */
		 cleanup();
	 }


	/*
	 * Set up for internationalization by setting the locale and opening the catalog
	 */
	 
	 strcpy(nlspath, "/usr/taxi/catalogs/finnish/%N.cat");
	 sprintf(put_nlspath, "NLSPATH=%s", nlspath);
	 putenv(put_nlspath);
	 setlocale(LC_ALL,"");

	open_catalog(&COMMON_m_catd, "common", 0, 0);
	open_catalog(&UI_m_catd, "ui", 0, 0);
	open_catalog(&WRITER_catd, "writer", 0, 0);

	/* initialize */
	if ( init() == NOT_DONE ) 
		/* cannot initialize properly. error message should be  printed by init(); so call cleanup() to exit */
		cleanup();

	/*
	 * call process; this is actually the user interface work horse function; it controls the flow of the program as
 	 * as the user moves from screen to screen
	 */
	process();

	/* cleanup and exit */
	cleanup();

}	/* end main()  */

