static char sz__FILE__[]="@(#)ui_exec_p.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_exec_p.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:50:35
 * @(#)  Last delta: 12/2/94 at 18:39:29
 * @(#)  SCCS File: /taxi/sccs/s.ui_exec_p.c
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include <term.h>
#include <signal.h>
#include <sys/types.h>

#include "menu.h"
#include "ui_error.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "ui_def.h"
#include "language.h"
#include "ui_strdefs.h"

#define 	MAX_EXEC_ARGS	20	/* max args allowed for exec call */

extern char scr_type;			/* type of screen */
extern int scr_name;			/* name of screen/prog on display/to be execed */
extern char *A_shm_ptr;			/* pointer to beginning of shared memory segment */
extern int fifo_fd;			/* file descriptor for FIFO */
extern FILE *trace_fp;			/* file pointer for trace file, declared in init() */
extern char *ui_errlist[];		/* error list for ui */
extern int iserrno;			/* cisam error # */
extern int user_index;			/* index into users struiuucture in shared memory */
extern struct menus *cmenu[];		/* array of pointers to menus structure in shared memory */

/* declare pointers to functions for saving the old values for the signal handlers */
void (*saved_sigint)(), (*saved_sigquit)(), (*saved_sighup)(), (*saved_sigpipe)(), (*saved_sigterm)();

extern int errno;
extern char *sys_errlist[];
extern int map_array[];			/* to map scr_name to index into menus structure in shared memory */
extern char *strtok();			/* library function */

extern int exec_from_scr;	/* screen from which this program is execed */
extern char exec_scr_type;	/* type of screen from which this prog execd */
extern void cleanup();
extern WINDOW *srch_win;	/* pointer to the searches window */
extern WINDOW *help_win;	/* pointer to the help window */

/***********************************************************************************/
/* exec_user_prog: 	in this routine the user interface forks a child	   */
/*			process, which then execs the desired program		   */
/*			(the program which the user selected). The routine	   */
/*			takes care of setting tty to non-curses mode		   */
/*			before execing, and then resetting it back to              */
/*			curses mode after the child goes away. Also all 	   */
/*			signals are handled, and all open files are closed	   */
/*			before the exec call is made.  				   */
/***********************************************************************************/
exec_user_prog()
{
	int childs_pid;			/* childs process id in parent */
	int cisam_ret;			/* return value from db() call */
	int indx;			/* index into menus */
	char prog_name[50];		/* complete name of program to be execed */
	char *argexec[MAX_EXEC_ARGS];	/* pointers to arguments for call to exec call */
	int i;				/* scratch variable */


	erase(); 			/* clear the screen */
	refresh();
	fflush(stdout);			/* flushn out any extra char's entered */

	status_line("");		/* erase the 25th line */

	/* clear any windows that are on the screen... HERE */

	def_prog_mode(); 		/* save these tty settings */
					/* nocbreak(); echo(); keypad(stdscr,FALSE); endwin(); */
	reset_shell_mode(); 		/* reset tty mode to non_curses mode */

					/* parent should ignore all signals until the child terminates */
	saved_sigint = signal(SIGINT,SIG_IGN);
	saved_sigquit = signal(SIGQUIT,SIG_IGN);
	saved_sighup = signal(SIGHUP,SIG_IGN);
	saved_sigpipe = signal(SIGPIPE,SIG_IGN);
	saved_sigterm = signal(SIGTERM,SIG_IGN);

	if ( (childs_pid = fork()) == 0 )  { 		/* fork a child process */

		/* this is the child */

							/* set all signals to default values in the child */
		signal(SIGINT,SIG_DFL);
		signal(SIGQUIT,SIG_DFL);
		signal(SIGHUP,SIG_DFL);
		signal(SIGPIPE,SIG_DFL);
		signal(SIGTERM,SIG_DFL);
		signal(SIGTSTP, SIG_DFL);
		signal(SIGUSR1, SIG_DFL);

		endwin(); 				/* get rid of curses in child */

		indx = map_array[scr_name]; 		/* get program name before detaching shared memory */
		strcpy(prog_name,cmenu[indx]->mf_unix_string);

		close(fifo_fd);				/* close FIFO */

		close_all_db_files();
								/* get the complete desired name of the program to be execed */

		argexec[0] = strtok(prog_name, " "); 		/* break up the program name into arguments */
		i = 1;
		while ((i<MAX_EXEC_ARGS-1) && ((argexec[i]=strtok(NULL," "))!=NULL))
			i++;					/* continue until args are exhausted */
		argexec[MAX_EXEC_ARGS-1] = NULL;  		/* will be used if user has 20 args inc. prog name */
		
#ifdef DEBUG
		if ( fclose(trace_fp) == EOF ) { 		/* close the trace file */
								/* cannot close trace file */
			fprintf(stderr,"%s %s <%s>",ui_errlist[UI_CLOSE], UI_TRACE_FILE, sys_errlist[errno]);
		}
#endif		
		shmdt(A_shm_ptr); 				/* detach shared memory */

		execvp(argexec[0],argexec);  			/* exec desired program */

		fprintf(stderr,"%s %s <%s>", ui_errlist[UI_NOEXEC], argexec[0] , sys_errlist[errno]); 		/* exec failed */
		fflush(stdout);
		fflush(stderr);
		sleep(3);
		exit(1);					/* leave program */

	}  /* end child */

	/*signal(SIGINT,saved_sigint); */ 				/* reset all signals */
	signal(SIGQUIT,saved_sigquit);
	signal(SIGHUP,saved_sighup);
	signal(SIGPIPE,saved_sigpipe);
	signal(SIGTERM,cleanup);

	while ( wait(NULL) != childs_pid ) 				/* this is the parent; wait until child terminates  */
		;  							/* go in the loop */

	signal(SIGINT,saved_sigint); 

	reset_prog_mode(); 						/* reset tty back to curses mode */
	
	/* NOTE SHOULD ALSO RESET keypad() of all the windows */
	echo();/* keypad(stdscr,FALSE); nocbreak(); keypad(srch_win,FALSE); */
	keypad(help_win,FALSE);
	noecho(); /* keypad(stdscr,TRUE); halfdelay(BLOCK_TIME); */
	/*keypad(srch_win,TRUE); keypad(help_win,TRUE); */

	/*initscr(); noecho(); keypad(stdscr,TRUE);halfdelay(BLOCK_TIME); */

							/* note that the scr_name is set to the program/screen execed; */
	 						/* so in order for the parent to continuw with the user interface */
	 						/* it should get the name of the screen from which this program */
	 						/* was execed */

							/* display the same old screen again; this should be done by */
	 						/* tricking curses into thinking that the stdscr/curscr has */
	 						/* changed */
	clear();

	menu_manager(YES, exec_from_scr);
	exec_from_scr = 0;
}


