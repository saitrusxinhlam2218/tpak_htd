static char sz__FILE__[]="@(#)lock_credit.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  lock_credit.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:33:02
 * @(#)  Last delta: 12/2/94 at 11:48:53
 * @(#)  SCCS File: /taxi/sccs/s.lock_credit.c
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/

/* lock_credit.c to lock the bad_credit_file while writing or reading.
   Created 6-6-90 Doug Rudoff */

#include <stdio.h>
#include <errno.h>

#include "path.h"
#include "credit.h"

int credit_file_has_been_locked_by_me = 0;

int
credit_file_is_locked()
{
  FILE *fp;
  char buf[10];
  int  pid, rc;

  if (credit_file_has_been_locked_by_me)
    return(1);

  /* Check to see if lock file is not there */
  if ((fp = fopen(CREDIT_LOCK_FILE, "r")) == NULL)
    return(0);

  /* The lock file has the locking processes pid */
  fgets(buf, 10, fp);
  pid = atoi(buf);

  fclose(fp);

  /* Check to see if the locking process is still there.
     If the process is gone, get rid of lock file. */
  rc = kill(pid, 0);
  if (rc == -1 && errno == ESRCH)
    {
      unlink(CREDIT_LOCK_FILE);
      return(0);
    }

  /* File is locked */
  return(1);
}

int
lock_credit_file()
{
  FILE *fp;

  if (credit_file_has_been_locked_by_me)
    return(0);

  if (credit_file_is_locked())
    return(-1);

  if ((fp = fopen(CREDIT_LOCK_FILE, "w")) == NULL)
    {
      credit_file_has_been_locked_by_me = 0;
      return(-1);
    }

  /* Make sure anyone can remove the lock file if there's a problem */
  chmod(CREDIT_LOCK_FILE, 0666);

  fprintf(fp, "%d\n", getpid());
  fclose(fp);
  
  credit_file_has_been_locked_by_me = 1;

  return(0);
}

unlock_credit_file()
{
  if (!credit_file_has_been_locked_by_me)
    return(-1);

  if (unlink(CREDIT_LOCK_FILE) == -1)
    return(-1);

  credit_file_has_been_locked_by_me = 0;
  
  return(0);
}

try_to_lock_bad_credit_file()
{
  int  locked_file, iteration;

  /* Lock the current bad credit file */
  locked_file = 0;
  iteration = 0;
  do
    {
      if (lock_credit_file() == -1)
	{
	  /* Allow only 25 tries to lock credit file */
	  if (iteration == 25)
	    {
	      /* Couldn't lock credit file */
	      return(-1);
	    }
	  iteration ++;
	  
	  /* Wait 1 second and try to lock again */
	  sleep(1);
	}
      else
	locked_file = 1;
    }
  while (!locked_file);

  return(0);
}
