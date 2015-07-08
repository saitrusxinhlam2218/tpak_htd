/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: credit_main.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1997/08/09 18:08:06 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/sysadmin/credit_main.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: credit_main.c,v 1.2 1997/08/09 18:08:06 taxiadm Exp $";



/* credit_main.c
   Created 6-6-90 Doug Rudoff */

#include <stdio.h>
#include <ctype.h>

#include "language.h"
#define DECLARING
#include "credit_strdefs.h"
#include "comm_strdefs.h"

extern char *tarfile;

main(argc, argv)
int argc;
char **argv;
{
  char s[10];
  int  bad_arguments = 0;
  int  new_or_update;


  /* Initialize language environment */
  init_locale();
  CREDIT_catd = init_catalog("credit_main");
  COMMON_m_catd = init_catalog("common");

  if (argc <= 1)
    bad_arguments= 1;
  else if (strcmp(argv[1], "-l") == 0)
    new_or_update = 1;
  else if (strcmp(argv[1], "-b") == 0)
    new_or_update = 0;
  else
    bad_arguments = 1;

  if (bad_arguments)
    {
      printf("%s\n%s\n",
	     catgets(CREDIT_catd, 1, 1,
		     "Wrong arguments to credit:"),
	     catgets(CREDIT_catd, 1, 2,
		     "-l for load, update or restore, -b for back up."));
      exit(0);
    }
      
  if (argc >= 3)
    {
      tarfile = argv[2];
      printf("Target tar file is '%s'\n\n", tarfile);
    }


  if (new_or_update)
    new_or_update_credit();
  else
    back_up_credit();

  printf("\n\n%s ...", catgets(CREDIT_catd, 1, 3, "Please hit return"));
  gets(s);

  /* Close language catalogs */
  cleanup_catalog(CREDIT_catd);
  cleanup_catalog(COMMON_m_catd);

  /* exit() prevents unaligned access bus error which is
     probably related to problems in using the catalogs */
  exit(0);
}
