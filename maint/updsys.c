static char sz__FILE__[]="@(#)updsys.c        10.1(ver) Date Release 2/24/95" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  updsys.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:48:19
 * @(#)  Last delta: 2/24/95 at 14:56:37
 * @(#)  SCCS File: /taxi/sccs/s.updsys.c
 *                                                                           *
 *****************************************************************************/



#include <stdio.h>
#include <sys/stat.h>


/*
    The functions in updsys are by the non privileged programs
    up_maint, admin_mgmt and copy_users to do some restricted
    privileged things.

    Updsys changes is uid to root and do one of the following:

        updsys <file> -UpdatePasswordFile
            does a system("/bin/cp <file> /etc/passwd") unless <file> is empty

        updsys <file> -UpdateGroupFile
            does a system("/bin/cp <file> /etc/group") unless <file> is empty

        updsys <user> -EnterPassword
            does a system("/bin/passwd <user>").

        updsys <host> -CopyPasswordAndGroupToRemote
            does a system("rcp /etc/passwd /etc/group <host>:/etc");

    The second parameter is case depended.
*/


main(argc,argv)
int argc;
char *argv[];
{
  int size;
  char buffer[1000];
  struct stat filestat;

  if(argc==3)
  {
    setuid(0); setgid(1);    /* change to root (the program must be owned by root and has the suid bit set) */
    if(!strcmp(argv[2],"-UpdatePasswordFile"))
    {
      if(!stat(argv[1],&filestat))     /* check for empty file */
      {
        if(filestat.st_size)
        {
          sprintf(buffer,"exec /bin/cp %s /etc/passwd",argv[1]);
          system(buffer);
          exit(0);
        }
      }
    }
    if(!strcmp(argv[2],"-UpdateGroupFile"))
    {
      if(!stat(argv[1],&filestat))     /* check for empty file */
      {
        if(filestat.st_size)
        {
          sprintf(buffer,"exec /bin/cp %s /etc/group",argv[1]);
          system(buffer);
          exit(0);
        }
      }
    }
    if(!strcmp(argv[2],"-EnterPassword"))
    {
      sprintf(buffer,"exec /bin/passwd %s",argv[1]);
      system(buffer);
      exit(0);
    }
    if(!strcmp(argv[2],"-CopyPasswordAndGroupToRemote"))
    {
      sprintf(buffer,"rcp /etc/passwd /etc/group %s:/etc",argv[1]);
      system(buffer);
      exit(0);
    }
  }
  exit(1);
}
