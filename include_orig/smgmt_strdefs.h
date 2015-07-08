 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  smgmt_strdefs.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:49:10
 * @(#)  Last delta: 12/2/94 at 18:22:32
 * @(#)  SCCS File: /taxi/sccs/s.smgmt_strdefs.h
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:smgmt_strdefs.h	10.1.2.1"
/*
 * String indexes for language catalog (PROGRAM: system_mgmt)
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
nl_catd SMGMT_m_catd;
#else
extern nl_catd SMGMT_m_catd;
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
#define	catclose( a )
#define open_catalog( a, b, c, d )
#define setlocale( a, b )

#endif


/*
 * Defines for the strings start here
 */
#ifdef INTERNATIONAL

#define SMGMT_SET	(1)

#define	SMGMT_START	(1)
#define	SMGMT_1		(SMGMT_START + 0)
#define	SMGMT_2		(SMGMT_START + 1)
#define	SMGMT_3		(SMGMT_START + 2)
#define	SMGMT_4		(SMGMT_START + 3)
#define	SMGMT_5		(SMGMT_START + 4)
#define	SMGMT_6		(SMGMT_START + 5)
#define	SMGMT_7		(SMGMT_START + 6)
#define	SMGMT_8		(SMGMT_START + 7)
#define	SMGMT_9		(SMGMT_START + 8)
#define	SMGMT_10		(SMGMT_START + 9)
#define	SMGMT_11		(SMGMT_START + 10)
#define	SMGMT_12		(SMGMT_START + 11)
#define	SMGMT_13		(SMGMT_START + 12)
#define	SMGMT_14		(SMGMT_START + 13)
#define	SMGMT_15		(SMGMT_START + 14)
#define	SMGMT_16		(SMGMT_START + 15)
#define	SMGMT_17		(SMGMT_START + 16)
#define	SMGMT_18		(SMGMT_START + 17)
#define	SMGMT_19		(SMGMT_START + 18)
#define	SMGMT_20		(SMGMT_START + 19)
#define	SMGMT_21		(SMGMT_START + 20)
#define	SMGMT_22		(SMGMT_START + 21)
#define	SMGMT_23		(SMGMT_START + 22)
#define	SMGMT_24		(SMGMT_START + 23)
#define	SMGMT_25		(SMGMT_START + 24)
#define	SMGMT_26		(SMGMT_START + 25)
#define	SMGMT_27		(SMGMT_START + 26)
#define	SMGMT_28		(SMGMT_START + 27)
#define	SMGMT_29		(SMGMT_START + 28)
#define	SMGMT_30		(SMGMT_START + 29)
#define	SMGMT_31		(SMGMT_START + 30)
#define	SMGMT_32		(SMGMT_START + 31)
#define	SMGMT_33		(SMGMT_START + 32)
#define	SMGMT_34		(SMGMT_START + 33)
#define	SMGMT_35		(SMGMT_START + 34)
#define	SMGMT_36		(SMGMT_START + 35)
#define	SMGMT_37		(SMGMT_START + 36)
#define	SMGMT_38		(SMGMT_START + 37)
#define	SMGMT_39		(SMGMT_START + 38)
#define	SMGMT_40		(SMGMT_START + 39)
#define	SMGMT_41		(SMGMT_START + 40)
#define	SMGMT_42		(SMGMT_START + 41)
#define	SMGMT_43		(SMGMT_START + 42)
#define	SMGMT_44		(SMGMT_START + 43)
#define	SMGMT_45		(SMGMT_START + 44)
#define	SMGMT_46		(SMGMT_START + 45)
#define	SMGMT_47		(SMGMT_START + 46)
#define	SMGMT_48		(SMGMT_START + 47)
#define	SMGMT_49		(SMGMT_START + 48)
#define	SMGMT_50		(SMGMT_START + 49)
#define	SMGMT_51		(SMGMT_START + 50)
#define	SMGMT_52		(SMGMT_START + 51)
#define	SMGMT_53		(SMGMT_START + 52)
#define	SMGMT_54		(SMGMT_START + 53)
#define	SMGMT_55		(SMGMT_START + 54)
#define	SMGMT_56		(SMGMT_START + 55)
#define	SMGMT_57		(SMGMT_START + 56)
#define	SMGMT_58		(SMGMT_START + 57)
#define	SMGMT_59		(SMGMT_START + 58)
#define	SMGMT_60		(SMGMT_START + 59)
#define	SMGMT_61		(SMGMT_START + 60)
#define	SMGMT_62		(SMGMT_START + 61)
#define SMGMT_63                (SMGMT_START + 62)
#define SMGMT_64                (SMGMT_START + 63)
#define	SMGMT_END		(SMGMT_START + 63)

#endif
