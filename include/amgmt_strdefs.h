 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  amgmt_strdefs.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:44:15
 * @(#)  Last delta: 12/2/94 at 18:00:04
 * @(#)  SCCS File: /taxi/sccs/s.amgmt_strdefs.h
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
/*
 * String index include file for the program adminmgmt.c
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
nl_catd AMGMT_m_catd;
#else
extern nl_catd AMGMT_m_catd;
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
#define AMGMT_SET		(1)
#define	AMGMT_START		(1)
#define	AMGMT_1			(AMGMT_START + 0)
#define	AMGMT_2			(AMGMT_START + 1)
#define	AMGMT_3			(AMGMT_START + 2)
#define	AMGMT_4			(AMGMT_START + 3)
#define	AMGMT_5			(AMGMT_START + 4)
#define	AMGMT_6			(AMGMT_START + 5)
#define	AMGMT_7			(AMGMT_START + 6)
#define	AMGMT_8			(AMGMT_START + 7)
#define	AMGMT_9			(AMGMT_START + 8)
#define	AMGMT_10		(AMGMT_START + 9)
#define	AMGMT_11		(AMGMT_START + 10)
#define	AMGMT_12		(AMGMT_START + 11)
#define	AMGMT_13		(AMGMT_START + 12)
#define	AMGMT_14		(AMGMT_START + 13)
#define	AMGMT_15		(AMGMT_START + 14)
#define	AMGMT_16		(AMGMT_START + 15)
#define	AMGMT_17		(AMGMT_START + 16)
#define	AMGMT_18		(AMGMT_START + 17)
#define	AMGMT_19		(AMGMT_START + 18)
#define	AMGMT_20		(AMGMT_START + 19)
#define	AMGMT_21		(AMGMT_START + 20)
#define	AMGMT_22		(AMGMT_START + 21)
#define	AMGMT_23		(AMGMT_START + 22)
#define	AMGMT_24		(AMGMT_START + 23)
#define	AMGMT_25		(AMGMT_START + 24)
#define	AMGMT_26		(AMGMT_START + 25)
#define	AMGMT_27		(AMGMT_START + 26)
#define	AMGMT_28		(AMGMT_START + 27)
#define	AMGMT_29		(AMGMT_START + 28)
#define	AMGMT_30		(AMGMT_START + 29)
#define	AMGMT_31		(AMGMT_START + 30)
#define	AMGMT_32		(AMGMT_START + 31)
#define	AMGMT_33		(AMGMT_START + 32)
#define	AMGMT_34		(AMGMT_START + 33)
#define	AMGMT_35		(AMGMT_START + 34)
#define	AMGMT_36		(AMGMT_START + 35)
#define	AMGMT_37		(AMGMT_START + 36)
#define	AMGMT_38		(AMGMT_START + 37)
#define	AMGMT_39		(AMGMT_START + 38)
#define	AMGMT_40		(AMGMT_START + 39)
#define	AMGMT_41		(AMGMT_START + 40)
#define	AMGMT_42		(AMGMT_START + 41)
#define	AMGMT_43		(AMGMT_START + 42)
#define	AMGMT_44		(AMGMT_START + 43)
#define	AMGMT_45		(AMGMT_START + 44)
#define	AMGMT_46		(AMGMT_START + 45)
#define	AMGMT_47		(AMGMT_START + 46)
#define	AMGMT_48		(AMGMT_START + 47)
#define	AMGMT_49		(AMGMT_START + 48)
#define	AMGMT_50		(AMGMT_START + 49)
#define	AMGMT_51		(AMGMT_START + 50)
#define	AMGMT_52		(AMGMT_START + 51)
#define	AMGMT_53		(AMGMT_START + 52)
#define	AMGMT_54		(AMGMT_START + 53)
#define	AMGMT_55		(AMGMT_START + 54)
#define	AMGMT_56		(AMGMT_START + 55)
#define	AMGMT_57		(AMGMT_START + 56)
#define	AMGMT_58		(AMGMT_START + 57)
#define	AMGMT_59		(AMGMT_START + 58)
#define	AMGMT_60		(AMGMT_START + 59)
#define	AMGMT_61		(AMGMT_START + 60)
#define	AMGMT_62		(AMGMT_START + 61)
#define	AMGMT_63		(AMGMT_START + 62)
#define	AMGMT_64		(AMGMT_START + 63)
#define	AMGMT_65		(AMGMT_START + 64)
#define	AMGMT_66		(AMGMT_START + 65)
#define	AMGMT_67		(AMGMT_START + 66)
#define	AMGMT_68		(AMGMT_START + 67)
#define	AMGMT_END		(AMGMT_START + 67)
#endif
