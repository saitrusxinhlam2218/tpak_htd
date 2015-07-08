 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  comm_strdefs.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:44:37
 * @(#)  Last delta: 12/2/94 at 18:41:02
 * @(#)  SCCS File: /taxi/sccs/s.comm_strdefs.h
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:comm_strdefs.h	10.1.2.1"
/*
 * String indexes for language catalog (PROGRAM: common routines)
 *
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
nl_catd COMMON_m_catd;
#else
extern nl_catd COMMON_m_catd;
#endif

nl_catd catopen();
char    *catgets();
char    *m_catgets();

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

#define COMMON_SET	(1)

#define	COMMON_START	(1)
#define	COMMON_TERM_OPEN		(COMMON_START + 0)
#define	COMMON_CAT_OPEN			(COMMON_TERM_OPEN + 1)
#define	COMMON_CAT_FLAG			(COMMON_CAT_OPEN  + 1)
#define	COMMON_NOW			(COMMON_CAT_FLAG  + 1)
#define	COMMON_END			(COMMON_NOW  + 1)

#endif
