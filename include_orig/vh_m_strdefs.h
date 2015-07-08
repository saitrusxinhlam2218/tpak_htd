 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  vh_m_strdefs.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:51:02
 * @(#)  Last delta: 12/2/94 at 18:19:47
 * @(#)  SCCS File: /taxi/sccs/s.vh_m_strdefs.h
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:vh_m_strdefs.h	10.1.2.1"

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
nl_catd VH_MAINT_m_catd;
#else
extern nl_catd VH_MAINT_m_catd;
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

#define VH_MAINT_SET		(1)

#define	VH_MAINT_START		(1)
#define	VH_MAINT_1		(VH_MAINT_START + 0)
#define	VH_MAINT_2		(VH_MAINT_START + 1)
#define	VH_MAINT_3		(VH_MAINT_START + 2)
#define	VH_MAINT_4		(VH_MAINT_START + 3)
#define	VH_MAINT_5		(VH_MAINT_START + 4)
#define	VH_MAINT_6		(VH_MAINT_START + 5)
#define	VH_MAINT_7		(VH_MAINT_START + 6)
#define	VH_MAINT_8		(VH_MAINT_START + 7)
#define	VH_MAINT_9		(VH_MAINT_START + 8)
#define	VH_MAINT_10		(VH_MAINT_START + 9)
#define	VH_MAINT_11		(VH_MAINT_START + 10)
#define	VH_MAINT_12		(VH_MAINT_START + 11)
#define	VH_MAINT_13		(VH_MAINT_START + 12)
#define	VH_MAINT_14		(VH_MAINT_START + 13)
#define	VH_MAINT_15		(VH_MAINT_START + 14)
#define	VH_MAINT_16		(VH_MAINT_START + 15)
#define	VH_MAINT_17		(VH_MAINT_START + 16)
#define	VH_MAINT_18		(VH_MAINT_START + 17)
#define	VH_MAINT_19		(VH_MAINT_START + 18)
#define	VH_MAINT_20		(VH_MAINT_START + 19)
#define	VH_MAINT_21		(VH_MAINT_START + 20)
#define	VH_MAINT_22		(VH_MAINT_START + 21)
#define	VH_MAINT_23		(VH_MAINT_START + 22)
#define	VH_MAINT_24		(VH_MAINT_START + 23)
#define	VH_MAINT_25		(VH_MAINT_START + 24)
#define	VH_MAINT_26		(VH_MAINT_START + 25)
#define	VH_MAINT_27		(VH_MAINT_START + 26)
#define	VH_MAINT_28		(VH_MAINT_START + 27)
#define	VH_MAINT_29		(VH_MAINT_START + 28)
#define	VH_MAINT_30		(VH_MAINT_START + 29)
#define	VH_MAINT_31		(VH_MAINT_START + 30)
#define	VH_MAINT_32		(VH_MAINT_START + 31)
#define	VH_MAINT_33		(VH_MAINT_START + 32)
#define	VH_MAINT_34		(VH_MAINT_START + 33)
#define	VH_MAINT_35		(VH_MAINT_START + 34)
#define	VH_MAINT_36		(VH_MAINT_START + 35)
#define	VH_MAINT_37		(VH_MAINT_START + 36)
#define	VH_MAINT_38		(VH_MAINT_START + 37)
#define	VH_MAINT_39		(VH_MAINT_START + 38)
#define	VH_MAINT_40		(VH_MAINT_START + 39)
#define	VH_MAINT_41		(VH_MAINT_START + 40)
#define	VH_MAINT_42		(VH_MAINT_START + 41)
#define	VH_MAINT_43		(VH_MAINT_START + 42)
#define	VH_MAINT_44		(VH_MAINT_START + 43)
#define	VH_MAINT_45		(VH_MAINT_START + 44)
#define	VH_MAINT_46		(VH_MAINT_START + 45)
#define	VH_MAINT_47		(VH_MAINT_START + 46)
#define	VH_MAINT_48		(VH_MAINT_START + 47)
#define	VH_MAINT_49		(VH_MAINT_START + 48)
#define	VH_MAINT_50		(VH_MAINT_START + 49)
#define	VH_MAINT_51		(VH_MAINT_START + 50)
#define	VH_MAINT_52		(VH_MAINT_START + 51)
#define	VH_MAINT_53		(VH_MAINT_START + 52)
#define	VH_MAINT_54		(VH_MAINT_START + 53)
#define	VH_MAINT_55		(VH_MAINT_START + 54)
#define	VH_MAINT_56		(VH_MAINT_START + 55)
#define	VH_MAINT_57		(VH_MAINT_START + 56)
#define	VH_MAINT_58		(VH_MAINT_START + 57)
#define	VH_MAINT_59		(VH_MAINT_START + 58)
#define	VH_MAINT_60		(VH_MAINT_START + 59)
#define	VH_MAINT_61		(VH_MAINT_START + 60)
#define	VH_MAINT_62		(VH_MAINT_START + 61)
#define	VH_MAINT_63		(VH_MAINT_START + 62)
#define	VH_MAINT_64		(VH_MAINT_START + 63)
#define	VH_MAINT_65		(VH_MAINT_START + 64)
#define	VH_MAINT_66		(VH_MAINT_START + 65)
#define	VH_MAINT_67		(VH_MAINT_START + 66)
#define	VH_MAINT_68		(VH_MAINT_START + 67)
#define	VH_MAINT_69		(VH_MAINT_START + 68)
#define	VH_MAINT_70		(VH_MAINT_START + 69)
#define	VH_MAINT_71		(VH_MAINT_START + 70)
#define	VH_MAINT_72		(VH_MAINT_START + 71)
#define	VH_MAINT_73		(VH_MAINT_START + 72)
#define	VH_MAINT_74		(VH_MAINT_START + 73)
#define	VH_MAINT_75		(VH_MAINT_START + 74)
#define	VH_MAINT_76		(VH_MAINT_START + 75)
#define	VH_MAINT_77		(VH_MAINT_START + 76)
#define	VH_MAINT_78		(VH_MAINT_START + 77)
#define	VH_MAINT_79		(VH_MAINT_START + 78)
#define	VH_MAINT_80		(VH_MAINT_START + 79)
#define	VH_MAINT_81		(VH_MAINT_START + 80)
#define	VH_MAINT_82		(VH_MAINT_START + 81)
#define	VH_MAINT_83		(VH_MAINT_START + 82)
#define	VH_MAINT_84		(VH_MAINT_START + 83)
#define	VH_MAINT_85		(VH_MAINT_START + 84)
#define	VH_MAINT_86		(VH_MAINT_START + 85)
#define	VH_MAINT_87		(VH_MAINT_START + 86)
#define	VH_MAINT_88		(VH_MAINT_START + 87)
#define	VH_MAINT_89		(VH_MAINT_START + 88)
#define	VH_MAINT_90		(VH_MAINT_START + 89)
#define	VH_MAINT_91		(VH_MAINT_START + 90)
#define	VH_MAINT_92		(VH_MAINT_START + 91)
#define	VH_MAINT_93		(VH_MAINT_START + 92)
#define	VH_MAINT_94		(VH_MAINT_START + 93)
#define	VH_MAINT_95		(VH_MAINT_START + 94)
#define	VH_MAINT_96		(VH_MAINT_START + 95)
#define	VH_MAINT_97		(VH_MAINT_START + 96)
#define	VH_MAINT_98		(VH_MAINT_START + 97)
#define	VH_MAINT_99		(VH_MAINT_START + 98)
#define	VH_MAINT_100		(VH_MAINT_START + 99)
#define	VH_MAINT_101		(VH_MAINT_START + 100)
#define	VH_MAINT_102		(VH_MAINT_START + 101)
#define	VH_MAINT_103		(VH_MAINT_START + 102)
#define	VH_MAINT_104		(VH_MAINT_START + 103)
#define	VH_MAINT_105		(VH_MAINT_START + 104)
#define	VH_MAINT_106		(VH_MAINT_START + 105)
#define	VH_MAINT_107		(VH_MAINT_START + 106)
#define	VH_MAINT_108		(VH_MAINT_START + 107)
#define	VH_MAINT_109		(VH_MAINT_START + 108)
#define	VH_MAINT_110		(VH_MAINT_START + 109)
#define	VH_MAINT_111		(VH_MAINT_START + 110)
#define	VH_MAINT_112		(VH_MAINT_START + 111)
#define	VH_MAINT_113		(VH_MAINT_START + 112)
#define	VH_MAINT_114		(VH_MAINT_START + 113)
#define	VH_MAINT_115		(VH_MAINT_START + 114)
#define	VH_MAINT_116		(VH_MAINT_START + 115)
#define	VH_MAINT_117		(VH_MAINT_START + 116)
#define	VH_MAINT_118		(VH_MAINT_START + 117)
#define	VH_MAINT_119		(VH_MAINT_START + 118)
#define	VH_MAINT_120		(VH_MAINT_START + 119)
#define	VH_MAINT_121		(VH_MAINT_START + 120)
#define	VH_MAINT_122		(VH_MAINT_START + 121)
#define	VH_MAINT_123		(VH_MAINT_START + 122)
#define	VH_MAINT_124		(VH_MAINT_START + 123)
#define	VH_MAINT_125		(VH_MAINT_START + 124)
#define	VH_MAINT_126		(VH_MAINT_START + 125)
#define	VH_MAINT_127		(VH_MAINT_START + 126)
#define	VH_MAINT_128		(VH_MAINT_START + 127)
#define	VH_MAINT_129		(VH_MAINT_START + 128)
#define	VH_MAINT_130		(VH_MAINT_START + 129)
#define	VH_MAINT_131		(VH_MAINT_START + 130)
#define	VH_MAINT_132		(VH_MAINT_START + 131)
#define	VH_MAINT_133		(VH_MAINT_START + 132)
#define	VH_MAINT_134		(VH_MAINT_START + 133)
#define	VH_MAINT_135		(VH_MAINT_START + 134)
#define	VH_MAINT_136		(VH_MAINT_START + 135)
#define	VH_MAINT_137		(VH_MAINT_START + 136)
#define	VH_MAINT_138		(VH_MAINT_START + 137)
#define	VH_MAINT_139		(VH_MAINT_START + 138)
#define	VH_MAINT_140		(VH_MAINT_START + 139)
#define	VH_MAINT_141		(VH_MAINT_START + 140)
#define VH_MAINT_142            (VH_MAINT_START + 141)
#define VH_MAINT_143            (VH_MAINT_START + 142)
#define VH_MAINT_144            (VH_MAINT_START + 143)
#define VH_MAINT_145            (VH_MAINT_START + 144)
#define	VH_MAINT_END		(VH_MAINT_START + 144)

#endif
