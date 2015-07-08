 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  menu_m_strdefs.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:47:52
 * @(#)  Last delta: 12/2/94 at 18:14:49
 * @(#)  SCCS File: /taxi/sccs/s.menu_m_strdefs.h
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
/*
 * String indexes for language catalog (PROGRAM: mf_maint)
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
nl_catd MEN_MAINT_m_catd;
extern char *getenv();
#else
extern nl_catd MEN_MAINT_m_catd;
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
#define MEN_MAINT_SET		(1)
#define	MEN_MAINT_START		(1)
#define	MEN_MAINT_1		(MEN_MAINT_START + 0)
#define	MEN_MAINT_2		(MEN_MAINT_START + 1)
#define	MEN_MAINT_3		(MEN_MAINT_START + 2)
#define	MEN_MAINT_4		(MEN_MAINT_START + 3)
#define	MEN_MAINT_5		(MEN_MAINT_START + 4)
#define	MEN_MAINT_6		(MEN_MAINT_START + 5)
#define	MEN_MAINT_7		(MEN_MAINT_START + 6)
#define	MEN_MAINT_8		(MEN_MAINT_START + 7)
#define	MEN_MAINT_9		(MEN_MAINT_START + 8)
#define	MEN_MAINT_10		(MEN_MAINT_START + 9)
#define	MEN_MAINT_11		(MEN_MAINT_START + 10)
#define	MEN_MAINT_12		(MEN_MAINT_START + 11)
#define	MEN_MAINT_13		(MEN_MAINT_START + 12)
#define	MEN_MAINT_14		(MEN_MAINT_START + 13)
#define	MEN_MAINT_15		(MEN_MAINT_START + 14)
#define	MEN_MAINT_16		(MEN_MAINT_START + 15)
#define	MEN_MAINT_17		(MEN_MAINT_START + 16)
#define	MEN_MAINT_18		(MEN_MAINT_START + 17)
#define	MEN_MAINT_19		(MEN_MAINT_START + 18)
#define	MEN_MAINT_20		(MEN_MAINT_START + 19)
#define	MEN_MAINT_21		(MEN_MAINT_START + 20)
#define	MEN_MAINT_22		(MEN_MAINT_START + 21)
#define	MEN_MAINT_23		(MEN_MAINT_START + 22)
#define	MEN_MAINT_24		(MEN_MAINT_START + 23)
#define	MEN_MAINT_25		(MEN_MAINT_START + 24)
#define	MEN_MAINT_26		(MEN_MAINT_START + 25)
#define	MEN_MAINT_27		(MEN_MAINT_START + 26)
#define	MEN_MAINT_28		(MEN_MAINT_START + 27)
#define	MEN_MAINT_29		(MEN_MAINT_START + 28)
#define	MEN_MAINT_30		(MEN_MAINT_START + 29)
#define	MEN_MAINT_31		(MEN_MAINT_START + 30)
#define	MEN_MAINT_32		(MEN_MAINT_START + 31)
#define	MEN_MAINT_33		(MEN_MAINT_START + 32)
#define	MEN_MAINT_34		(MEN_MAINT_START + 33)
#define	MEN_MAINT_35		(MEN_MAINT_START + 34)
#define	MEN_MAINT_36		(MEN_MAINT_START + 35)
#define	MEN_MAINT_37		(MEN_MAINT_START + 36)
#define	MEN_MAINT_38		(MEN_MAINT_START + 37)
#define	MEN_MAINT_39		(MEN_MAINT_START + 38)
#define	MEN_MAINT_40		(MEN_MAINT_START + 39)
#define	MEN_MAINT_41		(MEN_MAINT_START + 40)
#define	MEN_MAINT_42		(MEN_MAINT_START + 41)
#define	MEN_MAINT_43		(MEN_MAINT_START + 42)
#define	MEN_MAINT_44		(MEN_MAINT_START + 43)
#define	MEN_MAINT_45		(MEN_MAINT_START + 44)
#define	MEN_MAINT_46		(MEN_MAINT_START + 45)
#define	MEN_MAINT_47		(MEN_MAINT_START + 46)
#define	MEN_MAINT_48		(MEN_MAINT_START + 47)
#define	MEN_MAINT_49		(MEN_MAINT_START + 48)
#define	MEN_MAINT_50		(MEN_MAINT_START + 49)
#define	MEN_MAINT_51		(MEN_MAINT_START + 50)
#define	MEN_MAINT_52		(MEN_MAINT_START + 51)
#define	MEN_MAINT_53		(MEN_MAINT_START + 52)
#define	MEN_MAINT_54		(MEN_MAINT_START + 53)
#define	MEN_MAINT_55		(MEN_MAINT_START + 54)
#define	MEN_MAINT_END		(MEN_MAINT_START + 54)
#endif
