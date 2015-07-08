 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  geo_m_strdefs.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:46:12
 * @(#)  Last delta: 12/2/94 at 18:47:20
 * @(#)  SCCS File: /taxi/sccs/s.geo_m_strdefs.h
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:geo_m_strdefs.h	10.1.2.1"

/*
 * String indexes for language catalog (PROGRAM: geo_maint)
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
nl_catd GEO_MAINT_m_catd;
extern char *getenv();
#else
extern nl_catd GEO_MAINT_m_catd;
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

#define GEO_MAINT_SET		(1)


#define	GEO_MAINT_START		(1)
#define	GEO_MAINT_1		(GEO_MAINT_START + 0)
#define	GEO_MAINT_2		(GEO_MAINT_START + 1)
#define	GEO_MAINT_3		(GEO_MAINT_START + 2)
#define	GEO_MAINT_4		(GEO_MAINT_START + 3)
#define	GEO_MAINT_5		(GEO_MAINT_START + 4)
#define	GEO_MAINT_6		(GEO_MAINT_START + 5)
#define	GEO_MAINT_7		(GEO_MAINT_START + 6)
#define	GEO_MAINT_8		(GEO_MAINT_START + 7)
#define	GEO_MAINT_9		(GEO_MAINT_START + 8)
#define	GEO_MAINT_10		(GEO_MAINT_START + 9)
#define	GEO_MAINT_11		(GEO_MAINT_START + 10)
#define	GEO_MAINT_12		(GEO_MAINT_START + 11)
#define	GEO_MAINT_13		(GEO_MAINT_START + 12)
#define	GEO_MAINT_14		(GEO_MAINT_START + 13)
#define	GEO_MAINT_15		(GEO_MAINT_START + 14)
#define	GEO_MAINT_16		(GEO_MAINT_START + 15)
#define	GEO_MAINT_17		(GEO_MAINT_START + 16)
#define	GEO_MAINT_18		(GEO_MAINT_START + 17)
#define	GEO_MAINT_19		(GEO_MAINT_START + 18)
#define	GEO_MAINT_20		(GEO_MAINT_START + 19)
#define	GEO_MAINT_21		(GEO_MAINT_START + 20)
#define	GEO_MAINT_22		(GEO_MAINT_START + 21)
#define	GEO_MAINT_23		(GEO_MAINT_START + 22)
#define	GEO_MAINT_24		(GEO_MAINT_START + 23)
#define	GEO_MAINT_25		(GEO_MAINT_START + 24)
#define	GEO_MAINT_26		(GEO_MAINT_START + 25)
#define	GEO_MAINT_27		(GEO_MAINT_START + 26)
#define	GEO_MAINT_28		(GEO_MAINT_START + 27)
#define	GEO_MAINT_29		(GEO_MAINT_START + 28)
#define	GEO_MAINT_30		(GEO_MAINT_START + 29)
#define	GEO_MAINT_31		(GEO_MAINT_START + 30)
#define	GEO_MAINT_32		(GEO_MAINT_START + 31)
#define	GEO_MAINT_33		(GEO_MAINT_START + 32)
#define	GEO_MAINT_34		(GEO_MAINT_START + 33)
#define	GEO_MAINT_35		(GEO_MAINT_START + 34)
#define	GEO_MAINT_36		(GEO_MAINT_START + 35)
#define	GEO_MAINT_37		(GEO_MAINT_START + 36)
#define	GEO_MAINT_38		(GEO_MAINT_START + 37)
#define	GEO_MAINT_39		(GEO_MAINT_START + 38)
#define	GEO_MAINT_40		(GEO_MAINT_START + 39)
#define	GEO_MAINT_41		(GEO_MAINT_START + 40)
#define	GEO_MAINT_42		(GEO_MAINT_START + 41)
#define	GEO_MAINT_43		(GEO_MAINT_START + 42)
#define	GEO_MAINT_44		(GEO_MAINT_START + 43)
#define	GEO_MAINT_45		(GEO_MAINT_START + 44)
#define	GEO_MAINT_46		(GEO_MAINT_START + 45)
#define	GEO_MAINT_47		(GEO_MAINT_START + 46)
#define	GEO_MAINT_48		(GEO_MAINT_START + 47)
#define	GEO_MAINT_49		(GEO_MAINT_START + 48)
#define	GEO_MAINT_50		(GEO_MAINT_START + 49)
#define	GEO_MAINT_51		(GEO_MAINT_START + 50)
#define	GEO_MAINT_52		(GEO_MAINT_START + 51)
#define	GEO_MAINT_53		(GEO_MAINT_START + 52)
#define	GEO_MAINT_54		(GEO_MAINT_START + 53)
#define	GEO_MAINT_55		(GEO_MAINT_START + 54)
#define	GEO_MAINT_56		(GEO_MAINT_START + 55)
#define	GEO_MAINT_57		(GEO_MAINT_START + 56)
#define	GEO_MAINT_58		(GEO_MAINT_START + 57)
#define	GEO_MAINT_59		(GEO_MAINT_START + 58)
#define	GEO_MAINT_60		(GEO_MAINT_START + 59)
#define	GEO_MAINT_61		(GEO_MAINT_START + 60)
#define	GEO_MAINT_62		(GEO_MAINT_START + 61)
#define	GEO_MAINT_63		(GEO_MAINT_START + 62)
#define	GEO_MAINT_64		(GEO_MAINT_START + 63)
#define	GEO_MAINT_65		(GEO_MAINT_START + 64)
#define	GEO_MAINT_66		(GEO_MAINT_START + 65)
#define	GEO_MAINT_67		(GEO_MAINT_START + 66)
#define	GEO_MAINT_68		(GEO_MAINT_START + 67)
#define	GEO_MAINT_69		(GEO_MAINT_START + 68)
#define	GEO_MAINT_70		(GEO_MAINT_START + 69)
#define	GEO_MAINT_71		(GEO_MAINT_START + 70)
#define	GEO_MAINT_72		(GEO_MAINT_START + 71)
#define	GEO_MAINT_73		(GEO_MAINT_START + 72)
#define	GEO_MAINT_74		(GEO_MAINT_START + 73)
#define	GEO_MAINT_75		(GEO_MAINT_START + 74)
#define	GEO_MAINT_76		(GEO_MAINT_START + 75)
#define	GEO_MAINT_77		(GEO_MAINT_START + 76)
#define	GEO_MAINT_78		(GEO_MAINT_START + 77)
#define	GEO_MAINT_79		(GEO_MAINT_START + 78)
#define	GEO_MAINT_80		(GEO_MAINT_START + 79)
#define	GEO_MAINT_81		(GEO_MAINT_START + 80)
#define	GEO_MAINT_82		(GEO_MAINT_START + 81)
#define	GEO_MAINT_83		(GEO_MAINT_START + 82)
#define	GEO_MAINT_84		(GEO_MAINT_START + 83)
#define	GEO_MAINT_85		(GEO_MAINT_START + 84)
#define	GEO_MAINT_86		(GEO_MAINT_START + 85)
#define	GEO_MAINT_87		(GEO_MAINT_START + 86)
#define	GEO_MAINT_88		(GEO_MAINT_START + 87)
#define	GEO_MAINT_89		(GEO_MAINT_START + 88)
#define	GEO_MAINT_90		(GEO_MAINT_START + 89)
#define	GEO_MAINT_91		(GEO_MAINT_START + 90)
#define	GEO_MAINT_92		(GEO_MAINT_START + 91)
#define	GEO_MAINT_93		(GEO_MAINT_START + 92)
#define	GEO_MAINT_94		(GEO_MAINT_START + 93)
#define	GEO_MAINT_95		(GEO_MAINT_START + 94)
#define	GEO_MAINT_96		(GEO_MAINT_START + 95)
#define	GEO_MAINT_97		(GEO_MAINT_START + 96)
#define	GEO_MAINT_98		(GEO_MAINT_START + 97)
#define	GEO_MAINT_99		(GEO_MAINT_START + 98)
#define	GEO_MAINT_100		(GEO_MAINT_START + 99)
#define	GEO_MAINT_101		(GEO_MAINT_START + 100)
#define	GEO_MAINT_102		(GEO_MAINT_START + 101)
#define	GEO_MAINT_103		(GEO_MAINT_START + 102)
#define	GEO_MAINT_104		(GEO_MAINT_START + 103)
#define	GEO_MAINT_105		(GEO_MAINT_START + 104)
#define	GEO_MAINT_106		(GEO_MAINT_START + 105)
#define	GEO_MAINT_107		(GEO_MAINT_START + 106)
#define	GEO_MAINT_108		(GEO_MAINT_START + 107)
#define	GEO_MAINT_109		(GEO_MAINT_START + 108)
#define	GEO_MAINT_110		(GEO_MAINT_START + 109)
#define	GEO_MAINT_111		(GEO_MAINT_START + 110)
#define	GEO_MAINT_112		(GEO_MAINT_START + 111)
#define	GEO_MAINT_113		(GEO_MAINT_START + 112)
#define	GEO_MAINT_114		(GEO_MAINT_START + 113)
#define	GEO_MAINT_115		(GEO_MAINT_START + 114)
#define	GEO_MAINT_116		(GEO_MAINT_START + 115)
#define	GEO_MAINT_117		(GEO_MAINT_START + 116)
#define	GEO_MAINT_118		(GEO_MAINT_START + 117)
#define	GEO_MAINT_119		(GEO_MAINT_START + 118)
#define	GEO_MAINT_120		(GEO_MAINT_START + 119)
#define	GEO_MAINT_121		(GEO_MAINT_START + 120)
#define	GEO_MAINT_END		(GEO_MAINT_START + 120)


#define	GEO_MSG_SET		(2)

#define	GEO_MSG_START		(1)
#define	GEO_MSG_1		(GEO_MSG_START + 0)
#define	GEO_MSG_2		(GEO_MSG_START + 1)
#define	GEO_MSG_3		(GEO_MSG_START + 2)
#define	GEO_MSG_4		(GEO_MSG_START + 3)
#define	GEO_MSG_5		(GEO_MSG_START + 4)
#define	GEO_MSG_6		(GEO_MSG_START + 5)
#define	GEO_MSG_7		(GEO_MSG_START + 6)
#define	GEO_MSG_8		(GEO_MSG_START + 7)
#define	GEO_MSG_9		(GEO_MSG_START + 8)
#define	GEO_MSG_10		(GEO_MSG_START + 9)
#define	GEO_MSG_11		(GEO_MSG_START + 10)
#define	GEO_MSG_12		(GEO_MSG_START + 11)
#define	GEO_MSG_13		(GEO_MSG_START + 12)
#define	GEO_MSG_14		(GEO_MSG_START + 13)
#define	GEO_MSG_15		(GEO_MSG_START + 14)
#define	GEO_MSG_16		(GEO_MSG_START + 15)
#define	GEO_MSG_17		(GEO_MSG_START + 16)
#define	GEO_MSG_18		(GEO_MSG_START + 17)
#define	GEO_MSG_19		(GEO_MSG_START + 18)
#define	GEO_MSG_20		(GEO_MSG_START + 19)
#define	GEO_MSG_21		(GEO_MSG_START + 20)
#define	GEO_MSG_22		(GEO_MSG_START + 21)
#define	GEO_MSG_23		(GEO_MSG_START + 22)
#define	GEO_MSG_24		(GEO_MSG_START + 23)
#define	GEO_MSG_25		(GEO_MSG_START + 24)
#define	GEO_MSG_26		(GEO_MSG_START + 25)
#define	GEO_MSG_27		(GEO_MSG_START + 26)
#define	GEO_MSG_28		(GEO_MSG_START + 27)
#define	GEO_MSG_29		(GEO_MSG_START + 28)
#define	GEO_MSG_30		(GEO_MSG_START + 29)
#define	GEO_MSG_31		(GEO_MSG_START + 30)
#define	GEO_MSG_32		(GEO_MSG_START + 31)
#define	GEO_MSG_33		(GEO_MSG_START + 32)
#define	GEO_MSG_END		(GEO_MSG_START + 32)

#endif
