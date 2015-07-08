static char sz__FILE__[]="@(#)open_catalog.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  open_catalog.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:33:10
 * @(#)  Last delta: 12/2/94 at 11:40:04
 * @(#)  SCCS File: /taxi/sccs/s.open_catalog.c
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
#include <stdio.h>
#include <strings.h>
#include "comm_strdefs.h"
/* NLSPATH bug in ULTRIX 4.4 work-around. --- start --- */
#include <unistd.h>
#include <pwd.h>
/* NLSPATH bug in ULTRIX 4.4 work-around. --- end --- */
#include "language.h"

char	*getenv();

/*
 * open_catalog is used to open a language catalog call was put into this program to allow for the
 * redefinition of open cat to be a null function, this helps to clean the code up when Internationalization
 * is not required
 */
void open_catalog( catd, catname, oflag, eflag )
nl_catd	*catd;
char	*catname;
int	oflag;
int	eflag;
{
	char    *NLS_path;
	char    CatPathName[64];

	*catd = catopen(catname, oflag);
	catgets( *catd, 1, 1, NULL);
#ifdef FOO
	sprintf( CatPathName, "%s%s.cat", TPAK_NLS_PATH, catname );
	*catd = catopen( CatPathName, oflag );
#endif

}
