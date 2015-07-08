 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  disp_strdefs.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:45:06
 * @(#)  Last delta: 12/2/94 at 18:40:58
 * @(#)  SCCS File: /taxi/sccs/s.disp_strdefs.h
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:disp_strdefs.h	10.1.2.1"
/*
 * String indexes for language catalog (PROGRAM: displayps)
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
nl_catd DISP_m_catd;
#else
extern nl_catd DISP_m_catd;
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
#define open_catalog(a, b, c, d)
#define setlocale(a, b)

#endif


/*
 * Defines for the strings start here
 */
#ifdef INTERNATIONAL

#define DISP_SET	(1)

#define	DISP_START	(1)
#define	DISP_1		(DISP_START + 0)
#define	DISP_2		(DISP_START + 1)
#define	DISP_3		(DISP_START + 2)
#define	DISP_4		(DISP_START + 3)
#define	DISP_5		(DISP_START + 4)
#define	DISP_6		(DISP_START + 5)
#define	DISP_7		(DISP_START + 6)
#define	DISP_8		(DISP_START + 7)
#define	DISP_9		(DISP_START + 8)
#define	DISP_10		(DISP_START + 9)
#define	DISP_11		(DISP_START + 10)
#define	DISP_12		(DISP_START + 11)
#define	DISP_13		(DISP_START + 12)
#define	DISP_14		(DISP_START + 13)
#define	DISP_15		(DISP_START + 14)
#define	DISP_16		(DISP_START + 15)
#define	DISP_17		(DISP_START + 16)
#define	DISP_18		(DISP_START + 17)
#define	DISP_19		(DISP_START + 18)
#define	DISP_20		(DISP_START + 19)
#define	DISP_END	(DISP_START + 19)

#endif
