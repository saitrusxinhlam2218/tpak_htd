/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  fl_m_strdefs.h; Rel: 6.7.0.0; Get date: 12/8/92 at 09:54:42
* @(#)  Last delta: 11/27/90 at 17:32:59
* @(#)  SCCS File: /taxi/sccs/s.fl_m_strdefs.h
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/
#ident "@(#) head:fl_m_strdefs.h	6.7.0.0"

/*
 * String indexes for language catalog (PROGRAM: fl_maint)
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
nl_catd FL_MAINT_m_catd;
extern char *getenv();
#else
extern nl_catd FL_MAINT_m_catd;
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

#define FL_MAINT_SET		(1)

#define	FL_MAINT_START		(1)
#define	FL_MAINT_1		(FL_MAINT_START + 0)
#define	FL_MAINT_2		(FL_MAINT_START + 1)
#define	FL_MAINT_3		(FL_MAINT_START + 2)
#define	FL_MAINT_4		(FL_MAINT_START + 3)
#define	FL_MAINT_5		(FL_MAINT_START + 4)
#define	FL_MAINT_6		(FL_MAINT_START + 5)
#define	FL_MAINT_7		(FL_MAINT_START + 6)
#define	FL_MAINT_8		(FL_MAINT_START + 7)
#define	FL_MAINT_9		(FL_MAINT_START + 8)
#define	FL_MAINT_10		(FL_MAINT_START + 9)
#define	FL_MAINT_11		(FL_MAINT_START + 10)
#define	FL_MAINT_12		(FL_MAINT_START + 11)
#define	FL_MAINT_13		(FL_MAINT_START + 12)
#define	FL_MAINT_14		(FL_MAINT_START + 13)
#define	FL_MAINT_15		(FL_MAINT_START + 14)
#define	FL_MAINT_16		(FL_MAINT_START + 15)
#define	FL_MAINT_17		(FL_MAINT_START + 16)
#define	FL_MAINT_18		(FL_MAINT_START + 17)
#define	FL_MAINT_19		(FL_MAINT_START + 18)
#define	FL_MAINT_20		(FL_MAINT_START + 19)
#define	FL_MAINT_21		(FL_MAINT_START + 20)
#define	FL_MAINT_22		(FL_MAINT_START + 21)
#define	FL_MAINT_23		(FL_MAINT_START + 22)
#define	FL_MAINT_24		(FL_MAINT_START + 23)
#define	FL_MAINT_25		(FL_MAINT_START + 24)
#define	FL_MAINT_26		(FL_MAINT_START + 25)
#define	FL_MAINT_27		(FL_MAINT_START + 26)
#define	FL_MAINT_28		(FL_MAINT_START + 27)
#define	FL_MAINT_29		(FL_MAINT_START + 28)
#define	FL_MAINT_30		(FL_MAINT_START + 29)
#define	FL_MAINT_31		(FL_MAINT_START + 30)
#define	FL_MAINT_32		(FL_MAINT_START + 31)
#define	FL_MAINT_33		(FL_MAINT_START + 32)
#define	FL_MAINT_34		(FL_MAINT_START + 33)
#define	FL_MAINT_35		(FL_MAINT_START + 34)
#define	FL_MAINT_36		(FL_MAINT_START + 35)
#define	FL_MAINT_37		(FL_MAINT_START + 36)
#define	FL_MAINT_38		(FL_MAINT_START + 37)
#define	FL_MAINT_39		(FL_MAINT_START + 38)
#define	FL_MAINT_40		(FL_MAINT_START + 39)
#define FL_MAINT_41             (FL_MAINT_START + 40)
#define	FL_MAINT_END		(FL_MAINT_START + 41)


/*
 * The following strings were extracted from fl_scr_ary.h
 */

#define FL_SCR_FLDS_SET		(2)

#define	FL_SCR_FLDS_START	(1)
#define	FL_SCR_FLDS_1		(FL_SCR_FLDS_START + 0)
#define	FL_SCR_FLDS_2		(FL_SCR_FLDS_START + 1)
#define	FL_SCR_FLDS_3		(FL_SCR_FLDS_START + 2)
#define	FL_SCR_FLDS_4		(FL_SCR_FLDS_START + 3)
#define	FL_SCR_FLDS_5		(FL_SCR_FLDS_START + 4)
#define	FL_SCR_FLDS_6		(FL_SCR_FLDS_START + 5)
#define	FL_SCR_FLDS_7		(FL_SCR_FLDS_START + 6)
#define	FL_SCR_FLDS_8		(FL_SCR_FLDS_START + 7)
#define	FL_SCR_FLDS_9		(FL_SCR_FLDS_START + 8)
#define	FL_SCR_FLDS_10		(FL_SCR_FLDS_START + 9)
#define	FL_SCR_FLDS_11		(FL_SCR_FLDS_START + 10)
#define	FL_SCR_FLDS_12		(FL_SCR_FLDS_START + 11)
#define	FL_SCR_FLDS_13		(FL_SCR_FLDS_START + 12)
#define	FL_SCR_FLDS_14		(FL_SCR_FLDS_START + 13)
#define	FL_SCR_FLDS_15		(FL_SCR_FLDS_START + 14)
#define	FL_SCR_FLDS_16		(FL_SCR_FLDS_START + 15)
#define	FL_SCR_FLDS_17		(FL_SCR_FLDS_START + 16)
#define	FL_SCR_FLDS_18		(FL_SCR_FLDS_START + 17)
#define	FL_SCR_FLDS_19		(FL_SCR_FLDS_START + 18)
#define	FL_SCR_FLDS_20		(FL_SCR_FLDS_START + 19)
#define	FL_SCR_FLDS_21		(FL_SCR_FLDS_START + 20)
#define	FL_SCR_FLDS_22		(FL_SCR_FLDS_START + 21)
#define	FL_SCR_FLDS_23		(FL_SCR_FLDS_START + 22)
#define	FL_SCR_FLDS_24		(FL_SCR_FLDS_START + 23)
#define	FL_SCR_FLDS_25		(FL_SCR_FLDS_START + 24)
#define	FL_SCR_FLDS_26		(FL_SCR_FLDS_START + 25)
#define	FL_SCR_FLDS_27		(FL_SCR_FLDS_START + 26)
#define	FL_SCR_FLDS_28		(FL_SCR_FLDS_START + 27)
#define	FL_SCR_FLDS_29		(FL_SCR_FLDS_START + 28)
#define	FL_SCR_FLDS_30		(FL_SCR_FLDS_START + 29)
#define	FL_SCR_FLDS_31		(FL_SCR_FLDS_START + 30)
#define	FL_SCR_FLDS_32		(FL_SCR_FLDS_START + 31)
#define	FL_SCR_FLDS_33		(FL_SCR_FLDS_START + 32)
#define	FL_SCR_FLDS_34		(FL_SCR_FLDS_START + 33)
#define	FL_SCR_FLDS_35		(FL_SCR_FLDS_START + 34)
#define	FL_SCR_FLDS_36		(FL_SCR_FLDS_START + 35)
#define	FL_SCR_FLDS_37		(FL_SCR_FLDS_START + 36)
#define	FL_SCR_FLDS_38		(FL_SCR_FLDS_START + 37)
#define	FL_SCR_FLDS_39		(FL_SCR_FLDS_START + 38)
#define	FL_SCR_FLDS_40		(FL_SCR_FLDS_START + 39)
#define	FL_SCR_FLDS_41		(FL_SCR_FLDS_START + 40)
#define	FL_SCR_FLDS_42		(FL_SCR_FLDS_START + 41)
#define	FL_SCR_FLDS_43		(FL_SCR_FLDS_START + 42)
#define	FL_SCR_FLDS_44		(FL_SCR_FLDS_START + 43)
#define	FL_SCR_FLDS_45		(FL_SCR_FLDS_START + 44)
#define	FL_SCR_FLDS_46		(FL_SCR_FLDS_START + 45)
#define	FL_SCR_FLDS_47		(FL_SCR_FLDS_START + 46)
#define	FL_SCR_FLDS_48		(FL_SCR_FLDS_START + 47)
#define	FL_SCR_FLDS_49		(FL_SCR_FLDS_START + 48)
#define	FL_SCR_FLDS_50		(FL_SCR_FLDS_START + 49)
#define	FL_SCR_FLDS_51		(FL_SCR_FLDS_START + 50)
#define	FL_SCR_FLDS_52		(FL_SCR_FLDS_START + 51)
#define	FL_SCR_FLDS_53		(FL_SCR_FLDS_START + 52)
#define	FL_SCR_FLDS_54		(FL_SCR_FLDS_START + 53)
#define	FL_SCR_FLDS_55		(FL_SCR_FLDS_START + 54)
#define	FL_SCR_FLDS_56		(FL_SCR_FLDS_START + 55)
#define	FL_SCR_FLDS_57		(FL_SCR_FLDS_START + 56)
#define	FL_SCR_FLDS_58		(FL_SCR_FLDS_START + 57)
#define	FL_SCR_FLDS_59		(FL_SCR_FLDS_START + 58)
#define	FL_SCR_FLDS_60		(FL_SCR_FLDS_START + 59)
#define	FL_SCR_FLDS_61		(FL_SCR_FLDS_START + 60)
#define	FL_SCR_FLDS_62		(FL_SCR_FLDS_START + 61)
#define	FL_SCR_FLDS_63		(FL_SCR_FLDS_START + 62)
#define	FL_SCR_FLDS_64		(FL_SCR_FLDS_START + 63)
#define	FL_SCR_FLDS_65		(FL_SCR_FLDS_START + 64)
#define	FL_SCR_FLDS_66		(FL_SCR_FLDS_START + 65)
#define	FL_SCR_FLDS_67		(FL_SCR_FLDS_START + 66)
#define	FL_SCR_FLDS_68		(FL_SCR_FLDS_START + 67)
#define	FL_SCR_FLDS_69		(FL_SCR_FLDS_START + 68)
#define	FL_SCR_FLDS_70		(FL_SCR_FLDS_START + 69)
#define	FL_SCR_FLDS_71		(FL_SCR_FLDS_START + 70)
#define	FL_SCR_FLDS_72		(FL_SCR_FLDS_START + 71)
#define	FL_SCR_FLDS_73		(FL_SCR_FLDS_START + 72)
#define	FL_SCR_FLDS_74		(FL_SCR_FLDS_START + 73)
#define	FL_SCR_FLDS_75		(FL_SCR_FLDS_START + 74)
#define	FL_SCR_FLDS_76		(FL_SCR_FLDS_START + 75)
#define	FL_SCR_FLDS_77		(FL_SCR_FLDS_START + 76)
#define	FL_SCR_FLDS_78		(FL_SCR_FLDS_START + 77)
#define	FL_SCR_FLDS_79		(FL_SCR_FLDS_START + 78)
#define	FL_SCR_FLDS_80		(FL_SCR_FLDS_START + 79)
#define	FL_SCR_FLDS_81		(FL_SCR_FLDS_START + 80)
#define	FL_SCR_FLDS_82		(FL_SCR_FLDS_START + 81)
#define	FL_SCR_FLDS_83		(FL_SCR_FLDS_START + 82)
#define	FL_SCR_FLDS_84		(FL_SCR_FLDS_START + 83)
#define	FL_SCR_FLDS_85		(FL_SCR_FLDS_START + 84)
#define	FL_SCR_FLDS_86		(FL_SCR_FLDS_START + 85)
#define	FL_SCR_FLDS_87		(FL_SCR_FLDS_START + 86)
#define	FL_SCR_FLDS_88		(FL_SCR_FLDS_START + 87)
#define	FL_SCR_FLDS_89		(FL_SCR_FLDS_START + 88)
#define	FL_SCR_FLDS_90		(FL_SCR_FLDS_START + 89)
#define	FL_SCR_FLDS_91		(FL_SCR_FLDS_START + 90)
#define	FL_SCR_FLDS_92		(FL_SCR_FLDS_START + 91)
#define	FL_SCR_FLDS_93		(FL_SCR_FLDS_START + 92)
#define	FL_SCR_FLDS_94		(FL_SCR_FLDS_START + 93)
#define	FL_SCR_FLDS_95		(FL_SCR_FLDS_START + 94)
#define	FL_SCR_FLDS_96		(FL_SCR_FLDS_START + 95)
#define	FL_SCR_FLDS_97		(FL_SCR_FLDS_START + 96)
#define	FL_SCR_FLDS_98		(FL_SCR_FLDS_START + 97)
#define	FL_SCR_FLDS_99		(FL_SCR_FLDS_START + 98)
#define	FL_SCR_FLDS_100		(FL_SCR_FLDS_START + 99)
#define	FL_SCR_FLDS_101		(FL_SCR_FLDS_START + 100)
#define	FL_SCR_FLDS_102		(FL_SCR_FLDS_START + 101)
#define	FL_SCR_FLDS_103		(FL_SCR_FLDS_START + 102)
#define	FL_SCR_FLDS_104		(FL_SCR_FLDS_START + 103)
#define	FL_SCR_FLDS_105		(FL_SCR_FLDS_START + 104)
#define	FL_SCR_FLDS_106		(FL_SCR_FLDS_START + 105)
#define	FL_SCR_FLDS_107		(FL_SCR_FLDS_START + 106)
#define	FL_SCR_FLDS_108		(FL_SCR_FLDS_START + 107)
#define	FL_SCR_FLDS_109		(FL_SCR_FLDS_START + 108)
#define	FL_SCR_FLDS_110		(FL_SCR_FLDS_START + 109)
#define	FL_SCR_FLDS_111		(FL_SCR_FLDS_START + 110)
#define	FL_SCR_FLDS_112		(FL_SCR_FLDS_START + 111)
#define	FL_SCR_FLDS_113		(FL_SCR_FLDS_START + 112)
#define	FL_SCR_FLDS_114		(FL_SCR_FLDS_START + 113)
#define	FL_SCR_FLDS_115		(FL_SCR_FLDS_START + 114)
#define	FL_SCR_FLDS_116		(FL_SCR_FLDS_START + 115)
#define	FL_SCR_FLDS_117		(FL_SCR_FLDS_START + 116)
#define	FL_SCR_FLDS_118		(FL_SCR_FLDS_START + 117)
#define	FL_SCR_FLDS_119		(FL_SCR_FLDS_START + 118)
#define	FL_SCR_FLDS_120		(FL_SCR_FLDS_START + 119)
#define	FL_SCR_FLDS_121		(FL_SCR_FLDS_START + 120)
#define	FL_SCR_FLDS_122		(FL_SCR_FLDS_START + 121)
#define	FL_SCR_FLDS_123		(FL_SCR_FLDS_START + 122)
#define	FL_SCR_FLDS_124		(FL_SCR_FLDS_START + 123)
#define	FL_SCR_FLDS_125		(FL_SCR_FLDS_START + 124)
#define FL_SCR_FLDS_126		(FL_SCR_FLDS_START + 125)
#define FL_SCR_FLDS_127         (FL_SCR_FLDS_START + 126)
#define FL_SCR_FLDS_128         (FL_SCR_FLDS_START + 127)
#define FL_SCR_FLDS_129         (FL_SCR_FLDS_START + 128)
#define FL_SCR_FLDS_130         (FL_SCR_FLDS_START + 129)
#define FL_SCR_FLDS_131         (FL_SCR_FLDS_START + 130)
#define FL_SCR_FLDS_132         (FL_SCR_FLDS_START + 131)
#define FL_SCR_FLDS_133         (FL_SCR_FLDS_START + 132)
#define FL_SCR_FLDS_134         (FL_SCR_FLDS_START + 133)
#define FL_SCR_FLDS_135         (FL_SCR_FLDS_START + 134)
#define FL_SCR_FLDS_136         (FL_SCR_FLDS_START + 135)
#define FL_SCR_FLDS_137         (FL_SCR_FLDS_START + 136)
#define FL_SCR_FLDS_138         (FL_SCR_FLDS_START + 137)
#define FL_SCR_FLDS_139         (FL_SCR_FLDS_START + 138)
#define FL_SCR_FLDS_140         (FL_SCR_FLDS_START + 139)
#define FL_SCR_FLDS_141         (FL_SCR_FLDS_START + 140)
#define FL_SCR_FLDS_142         (FL_SCR_FLDS_START + 141)
#define FL_SCR_FLDS_143         (FL_SCR_FLDS_START + 142)
#define FL_SCR_FLDS_144         (FL_SCR_FLDS_START + 143)
#define FL_SCR_FLDS_145         (FL_SCR_FLDS_START + 144)
#define FL_SCR_FLDS_146         (FL_SCR_FLDS_START + 145)
#define FL_SCR_FLDS_147         (FL_SCR_FLDS_START + 146)
#define FL_SCR_FLDS_148         (FL_SCR_FLDS_START + 147)
#define	FL_SCR_FLDS_END		(FL_SCR_FLDS_START + 148)

#endif

