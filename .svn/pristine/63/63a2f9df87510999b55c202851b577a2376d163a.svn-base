 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  smain_strdefs.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:49:06
 * @(#)  Last delta: 12/2/94 at 18:10:25
 * @(#)  SCCS File: /taxi/sccs/s.smain_strdefs.h
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
/*
 * String indexes for language catalog (PROGRAM: system_main)
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
nl_catd SMAIN_m_catd;
#else
extern nl_catd SMAIN_m_catd;
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
#define SMAIN_SET	(1)
#define	SMAIN_START	(1)
#define	SMAIN_1		(SMAIN_START + 0)
#define	SMAIN_2		(SMAIN_START + 1)
#define	SMAIN_3		(SMAIN_START + 2)
#define	SMAIN_4		(SMAIN_START + 3)
#define	SMAIN_5		(SMAIN_START + 4)
#define	SMAIN_6		(SMAIN_START + 5)
#define	SMAIN_7		(SMAIN_START + 6)
#define	SMAIN_8		(SMAIN_START + 7)
#define	SMAIN_9		(SMAIN_START + 8)
#define	SMAIN_10	(SMAIN_START + 9)
#define	SMAIN_11	(SMAIN_START + 10)
#define	SMAIN_12	(SMAIN_START + 11)
#define	SMAIN_13	(SMAIN_START + 12)
#define	SMAIN_END	(SMAIN_START + 12)
#endif
