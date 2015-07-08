static char sz__FILE__[]="@(#)ui_exec_shift_s.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_exec_shift_s.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:50:39
 * @(#)  Last delta: 12/2/94 at 18:40:42
 * @(#)  SCCS File: /taxi/sccs/s.ui_exec_shift_s.c
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include <term.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>

#include "menu.h"
#include "ui_error.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "ui_def.h"
#include "language.h"
#include "ui_strdefs.h"
#include "screens.h"

#define 	MAX_EXEC_ARGS	20	/* max args allowed for exec call */

extern char scr_type;
extern int scr_name;
extern char *A_shm_ptr;	
extern int fifo_fd;	
extern FILE *trace_fp;
extern char *ui_errlist[];	
extern int iserrno;
extern int user_index;
extern struct menus *cmenu[];

void 	(*saved_sigint)(), 
		(*saved_sigquit)(), 
		(*saved_sighup)(), 
		(*saved_sigpipe)(), 
		(*saved_sigterm)();

extern int errno;
extern char *sys_errlist[];
extern int map_array[];	
extern char *strtok();

extern int exec_from_scr;
extern char exec_scr_type;
extern void cleanup();
extern WINDOW *srch_win;	
extern WINDOW *help_win;

short						ss_argument;

/*------------------------------------------------------------------
 *	exec_shift_s()
 *
 *		same as exec_user_prog except handles interface to 
 *		shift scheduler
 *------------------------------------------------------------------
 */

exec_shift_s()
{
	int childs_pid;
	int cisam_ret;
	int indx;	
	char prog_name[50];	
	int i;		
	int	rc;
	char system_buff[80];


	erase(); 
	refresh();
	fflush(stdout);	

	status_line("");		/* erase the 25th line */

	/* clear any windows that are on the screen... HERE */

	def_prog_mode();
#ifdef FOO
	reset_shell_mode(); 		/* reset tty mode to non_curses mode */
#endif

	indx = map_array[scr_name]; 
	strcpy(prog_name,cmenu[indx]->mf_unix_string);

	if (ss_argument)
	  {
		sprintf(system_buff, "%s %d", prog_name, ss_argument);
		ss_argument = 0;
	  }
	else
		sprintf(system_buff, "%s", prog_name);
			
	system(system_buff);		

#ifdef FOO
	reset_prog_mode(); 						/* reset tty back to curses mode */
	
	/* NOTE SHOULD ALSO RESET keypad() of all the windows */
	echo();
	keypad(help_win,FALSE);
	noecho();
#endif
	clear();

	if ((rc = get_return_from_shift_s()) != -1)
		menu_manager(YES, rc);
	else
		menu_manager(YES, exec_from_scr);

	exec_from_scr = 0;
}


get_return_from_shift_s()
{
	int		rc;
	FILE	*fp;
	char	buffer[80];

	rc = -1;

	sprintf(buffer, "%s.%d", "/tmp/ss_exit_code", getuid());

	if (fp = fopen(buffer, "r"))
	  {
		fscanf(fp, "%d", &rc);
		close(fp);
		sprintf(buffer, "%s.%d", "rm -f /tmp/ss_exit_code", getuid());
		system(buffer);
	  }

	return(rc);
}


/*------------------------------------------------------------------
 *	shift_s_command()
 *------------------------------------------------------------------
 */

shift_s_command(ss_command)
	short			ss_command;
{
	char			new_command[10],
					shift_s_menu[4];


	sprintf(new_command, "%d", ss_command);
	sprintf(shift_s_menu, "%d", SHIFT_SCHEDULER);

	if (strncmp(new_command, shift_s_menu, strlen(shift_s_menu)))
		return(0);

	if (strlen(new_command) > strlen(shift_s_menu))
		ss_argument = ss_command;
	else
		ss_argument = 0;

	return(SHIFT_SCHEDULER);
}


