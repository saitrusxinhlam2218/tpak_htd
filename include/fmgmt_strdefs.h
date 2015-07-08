 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fmgmt_strdefs.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:46:01
 * @(#)  Last delta: 12/2/94 at 18:31:26
 * @(#)  SCCS File: /taxi/sccs/s.fmgmt_strdefs.h
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
/*
 * String indexes for language catalog (PROGRAM: filemgmt)
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
nl_catd FMGMT_m_catd;
#else
extern nl_catd FMGMT_m_catd;
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
#define FMGMT_SET		(1)
#define	FMGMT_START		(1)
#define	FMGMT_1			(FMGMT_START + 0)
#define	FMGMT_2			(FMGMT_START + 1)
#define	FMGMT_3			(FMGMT_START + 2)
#define	FMGMT_4			(FMGMT_START + 3)
#define	FMGMT_5			(FMGMT_START + 4)
#define	FMGMT_6			(FMGMT_START + 5)
#define	FMGMT_7			(FMGMT_START + 6)
#define	FMGMT_8			(FMGMT_START + 7)
#define	FMGMT_9			(FMGMT_START + 8)
#define	FMGMT_10		(FMGMT_START + 9)
#define	FMGMT_11		(FMGMT_START + 10)
#define	FMGMT_12		(FMGMT_START + 11)
#define	FMGMT_13		(FMGMT_START + 12)
#define	FMGMT_14		(FMGMT_START + 13)
#define	FMGMT_15		(FMGMT_START + 14)
#define	FMGMT_16		(FMGMT_START + 15)
#define	FMGMT_17		(FMGMT_START + 16)
#define	FMGMT_18		(FMGMT_START + 17)
#define	FMGMT_19		(FMGMT_START + 18)
#define	FMGMT_20		(FMGMT_START + 19)
#define	FMGMT_21		(FMGMT_START + 20)
#define	FMGMT_22		(FMGMT_START + 21)
#define	FMGMT_23		(FMGMT_START + 22)
#define	FMGMT_24		(FMGMT_START + 23)
#define	FMGMT_25		(FMGMT_START + 24)
#define	FMGMT_26		(FMGMT_START + 25)
#define	FMGMT_27		(FMGMT_START + 26)
#define	FMGMT_28		(FMGMT_START + 27)
#define	FMGMT_29		(FMGMT_START + 28)
#define	FMGMT_30		(FMGMT_START + 29)
#define	FMGMT_31		(FMGMT_START + 30)
#define	FMGMT_32		(FMGMT_START + 31)
#define	FMGMT_33		(FMGMT_START + 32)
#define	FMGMT_34		(FMGMT_START + 33)
#define	FMGMT_35		(FMGMT_START + 34)
#define	FMGMT_36		(FMGMT_START + 35)
#define	FMGMT_37		(FMGMT_START + 36)
#define	FMGMT_38		(FMGMT_START + 37)
#define	FMGMT_39		(FMGMT_START + 38)
#define	FMGMT_40		(FMGMT_START + 39)
#define	FMGMT_41		(FMGMT_START + 40)
#define	FMGMT_42		(FMGMT_START + 41)
#define	FMGMT_43		(FMGMT_START + 42)
#define	FMGMT_44		(FMGMT_START + 43)
#define	FMGMT_45		(FMGMT_START + 44)
#define	FMGMT_46		(FMGMT_START + 45)
#define	FMGMT_47		(FMGMT_START + 46)
#define	FMGMT_48		(FMGMT_START + 47)
#define	FMGMT_49		(FMGMT_START + 48)
#define	FMGMT_50		(FMGMT_START + 49)
#define	FMGMT_51		(FMGMT_START + 50)
#define	FMGMT_52		(FMGMT_START + 51)
#define	FMGMT_53		(FMGMT_START + 52)
#define	FMGMT_54		(FMGMT_START + 53)
#define	FMGMT_55		(FMGMT_START + 54)
#define	FMGMT_56		(FMGMT_START + 55)
#define	FMGMT_57		(FMGMT_START + 56)
#define	FMGMT_END		(FMGMT_START + 56)
#endif
