/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  rep_strdefs.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:48:51
* @(#)  Last delta: 12/2/94 at 18:18:32
* @(#)  SCCS File: /taxi/sccs/s.rep_strdefs.h
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/
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
nl_catd REP_catd;
extern  nl_catd init_catalog();
extern  int cleanup_catalog();
extern  void init_locale();
extern  void init_mgr_locale();
#else
extern nl_catd REP_catd;
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
