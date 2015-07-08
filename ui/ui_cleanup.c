static char sz__FILE__[]="@(#)ui_cleanup.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  ui_cleanup.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:50:10
* @(#)  Last delta: 12/2/94 at 18:10:48
* @(#)  SCCS File: /taxi/sccs/s.ui_cleanup.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

#include <cursesX.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>

#include "mads_types.h"
#include "user.h"
#include "df_maint.h"
#include "taxi_db.h"
#include "ui_strdefs.h"
#include "comm_strdefs.h"
#include "writer_strdefs.h"
#include "ui_error.h"
#include "Line_Manager_private.h"

/* global variables */
extern int fifo_fd;			/* FIFO file descriptor */
extern char *A_shm_ptr;			/* pointer to start of shared memory segment */
extern char  *LineMgrshmptr;
extern int user_index;			/* index in users structure in shared memory */
extern struct users *uiuser[];		/* array of pointers to users structure in shared memory */
extern short mem_attached;		/* whether init() was able to attach shared memory before cleanup() is called */
extern short curses_set;		/* whether init() initialized curses or not */
extern FILE *trace_fp;			/* file pointer for trace files */
extern char logo_file[];		/* name of logo_file to be displayed, when user logs out */
extern short clear_err_flag; 		/* set if last line contains an error msg */
extern int Kill_parent_when_done;
extern char *ui_errlist[];
extern int LineMgrIndex;

void
close_all_db_files()
{
  int  i;

  /* Close any open db files */
  for (i = 0; i < DB_NUM_FILES; i ++)
    if (Db_files[i].fd != -1)
      {
	if (db(i, CLOSE, 0, 0, 0) < 0)
	  fprintf(stderr, "%s: %s (%d)\n", ui_errlist[UI_CLOSE], Db_files[i].name, iserrno);
      }
}

/********************************************************************************/
/* cleanup: 	the cleanup routine to be executed before the user interface    */
/*		program exits							*/
/********************************************************************************/
void cleanup()
{
	char shell_command[80];	/* buffer to save logo file name to cat */
	int i;
	static int called = 0;
        int rc;

	if (called) exit(1);
	called = 1;


#ifdef DEBUG
	if (trace_fp)
	    fclose(trace_fp);
#endif

	close(fifo_fd);

	/* Erase pid and uid reference. This is to mark the user memory slot as
	   available again */

	if (uiuser[user_index])
	  {
	    uiuser[user_index]->p_id = 0;
	    uiuser[user_index]->user_id = 0;
	  }

	status_line("");				/* rewrite the 25th line with spaces */

	/* generate statusr records */
	generate_statusr_recs();

	do_resp_time_stats();
	close_resp_stat_file();

	if ( mem_attached ) { 				/* reset variables in users structure for this user in shared memory */
		uiuser[user_index]->user_id = 0;
		uiuser[user_index]->p_id = 0;		/* this tells the counter process, that the user logged out */
		shmdt(A_shm_ptr); 			/* detach shared memory */
	}  /* end if mem_attached */

	if ( curses_set ) {
		nocbreak();
		endwin();
	}

	close_all_db_files();

        if ( LineMgrshmptr != NULL )
          {
            if ( LineMgrIndex > 0 )
              {
                for ( i = 0; i < MAX_PIDS; i++ )
                  {
                    if ( Lines[LineMgrIndex]->UIPid[i] == getpid() )
                      {
                        Lines[LineMgrIndex]->UIPid[i] = 0;
                        break;
                      }
                    else
                      {
                        rc = kill( Lines[LineMgrIndex]->UIPid[i], 0 );
                        if ( rc == -1 && errno == ESRCH )
                          Lines[LineMgrIndex]->UIPid[i] = 0;
                      }
                  }
                shmdt( LineMgrshmptr );            
              }
          }

	/* cat the logo_file if it exists */
	if ( mem_attached  &&  logo_file[0] != '\0' ) {
		sprintf(shell_command,"cat %s 2> /dev/null",logo_file );
		system(shell_command);
	}

	catclose(UI_m_catd);
	catclose(COMMON_m_catd);
	catclose(WRITER_catd);

	fflush(stdout);

	/* If there are any errors displayed, wait 5 seconds
	   before exiting. */
	if (clear_err_flag)
	    sleep(5);

	/* Kill parent process if passed proper start up argument */
	if (Kill_parent_when_done)
	  kill(getppid(), 9);

	exit(1);

}
