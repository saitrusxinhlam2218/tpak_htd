/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  credit_strdefs.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:44:48
* @(#)  Last delta: 12/2/94 at 18:41:00
* @(#)  SCCS File: /taxi/sccs/s.credit_strdefs.h
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/
#ident "@(#) head:credit_strdefs.h/usr/users/brian/samplan/src/include	10.1.2.1"
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
nl_catd CREDIT_catd;
extern  nl_catd init_catalog();
extern  int cleanup_catalog();
extern  void init_locale();
extern  void init_mgr_locale();
#else
extern nl_catd CREDIT_catd;
#endif

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
#endif
