static char sz__FILE__[]="@(#)chmdwn.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  chmdwn.c; Rel: 10.1.2.1; Get date: 4/20/95 at 09:47:43
 * @(#)  Last delta: 12/2/94 at 18:01:24
 * @(#)  SCCS File: /taxi/sccs/s.chmdwn.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <pwd.h>
#include <errno.h>
#include "mad_error.h"

#define OWNER      "taxiII"       /* Owner for the MADS files */
#define PERM_MODE  00666          /* mode for MADS files */

chg_own(file)
char *file;
{
struct passwd *passw, *getpwnam();

if ((passw = getpwnam(OWNER)) == 0)
   {
   sprintf(trace_str, "%s is NOT a valid user", OWNER);
   ERROR(' ', "", trace_str);
   return(-1);
   }

if (chown(file, passw->pw_uid, passw->pw_gid) == -1)
   {
   if (errno == EPERM)     /* because of permission, ignore it */ 
      return(0);

   sprintf(trace_str, "Can't chown %s - %d", file, errno);
   ERROR(' ', "", trace_str);
   return(-1);
   }

#ifdef DEBUG
sprintf(trace_str, "chown %s to %s\n", file, OWNER);
TRACE(trace_str);
#endif

return(0);
}

chg_mod(file)
char *file;
{
int mode = PERM_MODE;

if (chmod(file, mode) == -1)
   {
   if (errno == EPERM)     /* because of permission, ignore it */ 
      return(0);

   sprintf(trace_str, "Can't chmod %s - %d", file, errno);
   ERROR(' ', "", trace_str);
   return(-1);
   }

#ifdef DEBUG
sprintf(trace_str, "chmod %s to %.4o\n", file, mode);
TRACE(trace_str);
#endif

return(0);
}
