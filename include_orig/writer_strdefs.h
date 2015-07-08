 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  writer_strdefs.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:51:21
 * @(#)  Last delta: 12/2/94 at 18:04:11
 * @(#)  SCCS File: /taxi/sccs/s.writer_strdefs.h
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:writer_strdefs.h/usr/users/brian/samplan/src/include	10.1.2.1"

/*
 * If we are compiling for internationalization, then define
 * INTERNATIONAL
 */

#ifdef INTERNATIONAL

#ifndef INTER_INCLUDES
#include <nl_types.h>
#include <locale.h>
#define INTER_INCLUDES
#endif

#ifdef DECLARING
nl_catd WRITER_catd;
#else
extern nl_catd WRITER_catd;
#endif

nl_catd catopen();
char    *catgets();
char    *m_catgets();
int     catclose();

#else
/*
 * no international text	
 */

#define catgets( catd, set_num, msg_num, s )	s
#define m_catgets( catd, set_num, msg_num, s )	s
#define catclose( a )
#define open_catalog( a, b, c, d )
#define setlocale( a, b )

#endif


/*
 * Defines for the strings start here
 */
#ifdef INTERNATIONAL
#endif
