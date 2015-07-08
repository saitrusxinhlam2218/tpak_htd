/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: tc_m_strdefs.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1996/09/06 13:51:22 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/tc_m_strdefs.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: tc_m_strdefs.h,v $	$Revision: 1.2 $"

#ifdef INTERNATIONAL

#ifndef INTER_INCLUDES
#include <nl_types.h>
#include <locale.h>
#define INTER_INCLUDES
#endif

#ifdef DECLARING
nl_catd  TC_MAINT_m_catd;
#else
extern nl_catd TC_MAINT_m_catd;
#endif

nl_catd catopen();
char    *catgets();
char    *m_catgets();

#else

#define catgets( catd, set_num, msg_num, s )	s
#define m_catgets( catd, set_num, msg_num, s )	s
#define catclose( a )
#define open_catalog( a, b, c, d )
#define setlocale( a, b )

#endif

#ifdef INTERNATIONAL

#define  TC_MAINT_SET         (1)

#define	TC_MAINT_START		(1)
#define	TC_MAINT_1		(TC_MAINT_START + 0)
#define	TC_MAINT_2		(TC_MAINT_START + 1)
#define	TC_MAINT_3		(TC_MAINT_START + 2)
#define	TC_MAINT_4		(TC_MAINT_START + 3)
#define	TC_MAINT_5		(TC_MAINT_START + 4)
#define	TC_MAINT_6		(TC_MAINT_START + 5)
#define	TC_MAINT_7		(TC_MAINT_START + 6)
#define	TC_MAINT_8		(TC_MAINT_START + 7)
#define	TC_MAINT_9		(TC_MAINT_START + 8)
#define	TC_MAINT_10		(TC_MAINT_START + 9)
#define	TC_MAINT_11		(TC_MAINT_START + 10)
#define	TC_MAINT_12		(TC_MAINT_START + 11)
#define	TC_MAINT_13		(TC_MAINT_START + 12)
#define	TC_MAINT_14		(TC_MAINT_START + 13)
#define	TC_MAINT_15		(TC_MAINT_START + 14)
#define	TC_MAINT_16		(TC_MAINT_START + 15)
#define	TC_MAINT_17		(TC_MAINT_START + 16)
#define	TC_MAINT_18		(TC_MAINT_START + 17)
#define	TC_MAINT_19		(TC_MAINT_START + 18)
#define	TC_MAINT_20		(TC_MAINT_START + 19)
#define	TC_MAINT_21		(TC_MAINT_START + 20)
#define	TC_MAINT_22		(TC_MAINT_START + 21)
#define	TC_MAINT_23		(TC_MAINT_START + 22)
#define	TC_MAINT_24		(TC_MAINT_START + 23)
#define	TC_MAINT_25		(TC_MAINT_START + 24)
#define	TC_MAINT_26		(TC_MAINT_START + 25)
#define	TC_MAINT_27		(TC_MAINT_START + 26)
#define	TC_MAINT_28		(TC_MAINT_START + 27)
#define	TC_MAINT_29		(TC_MAINT_START + 28)
#define	TC_MAINT_30		(TC_MAINT_START + 29)
#define	TC_MAINT_31		(TC_MAINT_START + 30)
#define	TC_MAINT_32		(TC_MAINT_START + 31)
#define	TC_MAINT_33		(TC_MAINT_START + 32)
#define	TC_MAINT_34		(TC_MAINT_START + 33)
#define	TC_MAINT_35		(TC_MAINT_START + 34)
#define	TC_MAINT_36		(TC_MAINT_START + 35)
#define	TC_MAINT_37		(TC_MAINT_START + 36)
#define	TC_MAINT_38		(TC_MAINT_START + 37)
#define	TC_MAINT_39		(TC_MAINT_START + 38)
#define	TC_MAINT_40		(TC_MAINT_START + 39)
#define	TC_MAINT_41		(TC_MAINT_START + 40)
#define	TC_MAINT_42		(TC_MAINT_START + 41)
#define	TC_MAINT_43		(TC_MAINT_START + 42)
#define	TC_MAINT_44		(TC_MAINT_START + 43)
#define	TC_MAINT_45		(TC_MAINT_START + 44)
#define	TC_MAINT_46		(TC_MAINT_START + 45)
#define	TC_MAINT_47		(TC_MAINT_START + 46)
#define	TC_MAINT_48		(TC_MAINT_START + 47)
#define	TC_MAINT_49		(TC_MAINT_START + 48)
#define	TC_MAINT_50		(TC_MAINT_START + 49)
#define	TC_MAINT_51		(TC_MAINT_START + 50)
#define	TC_MAINT_52		(TC_MAINT_START + 51)
#define	TC_MAINT_53		(TC_MAINT_START + 52)
#define	TC_MAINT_54		(TC_MAINT_START + 53)
#define	TC_MAINT_55		(TC_MAINT_START + 54)
#define	TC_MAINT_56		(TC_MAINT_START + 55)
#define	TC_MAINT_57		(TC_MAINT_START + 56)
#define	TC_MAINT_58		(TC_MAINT_START + 57)
#define	TC_MAINT_59		(TC_MAINT_START + 58)
#define	TC_MAINT_60		(TC_MAINT_START + 59)
#define	TC_MAINT_61		(TC_MAINT_START + 60)
#define	TC_MAINT_62		(TC_MAINT_START + 61)
#define	TC_MAINT_63		(TC_MAINT_START + 62)
#define	TC_MAINT_64		(TC_MAINT_START + 63)
#define	TC_MAINT_65		(TC_MAINT_START + 64)
#define	TC_MAINT_66		(TC_MAINT_START + 65)
#define	TC_MAINT_67		(TC_MAINT_START + 66)
#define	TC_MAINT_68		(TC_MAINT_START + 67)
#define	TC_MAINT_69		(TC_MAINT_START + 68)
#define	TC_MAINT_70		(TC_MAINT_START + 69)
#define	TC_MAINT_71		(TC_MAINT_START + 70)
#define	TC_MAINT_72		(TC_MAINT_START + 71)
#define	TC_MAINT_73		(TC_MAINT_START + 72)
#define	TC_MAINT_74		(TC_MAINT_START + 73)
#define	TC_MAINT_75		(TC_MAINT_START + 74)
#define	TC_MAINT_76		(TC_MAINT_START + 75)
#define	TC_MAINT_77		(TC_MAINT_START + 76)
#define	TC_MAINT_78		(TC_MAINT_START + 77)
#define	TC_MAINT_79		(TC_MAINT_START + 78)
#define	TC_MAINT_80		(TC_MAINT_START + 79)
#define	TC_MAINT_81		(TC_MAINT_START + 80)
#define	TC_MAINT_82		(TC_MAINT_START + 81)
#define	TC_MAINT_83		(TC_MAINT_START + 82)
#define	TC_MAINT_84		(TC_MAINT_START + 83)
#define	TC_MAINT_85		(TC_MAINT_START + 84)
#define	TC_MAINT_86		(TC_MAINT_START + 85)
#define	TC_MAINT_87		(TC_MAINT_START + 86)
#define	TC_MAINT_88		(TC_MAINT_START + 87)
#define	TC_MAINT_89		(TC_MAINT_START + 88)
#define	TC_MAINT_90		(TC_MAINT_START + 89)
#define	TC_MAINT_91		(TC_MAINT_START + 90)
#define	TC_MAINT_92		(TC_MAINT_START + 91)
#define	TC_MAINT_93		(TC_MAINT_START + 92)
#define	TC_MAINT_94		(TC_MAINT_START + 93)
#define	TC_MAINT_95		(TC_MAINT_START + 94)
#define	TC_MAINT_96		(TC_MAINT_START + 95)
#define	TC_MAINT_97		(TC_MAINT_START + 96)
#define	TC_MAINT_98		(TC_MAINT_START + 97)
#define	TC_MAINT_99		(TC_MAINT_START + 98)
#define	TC_MAINT_100		(TC_MAINT_START + 99)
#define	TC_MAINT_101		(TC_MAINT_START + 100)
#define	TC_MAINT_102		(TC_MAINT_START + 101)
#define	TC_MAINT_103		(TC_MAINT_START + 102)
#define	TC_MAINT_104		(TC_MAINT_START + 103)
#define	TC_MAINT_105		(TC_MAINT_START + 104)
#define	TC_MAINT_106		(TC_MAINT_START + 105)
#define	TC_MAINT_107		(TC_MAINT_START + 106)
#define	TC_MAINT_108		(TC_MAINT_START + 107)
#define	TC_MAINT_109		(TC_MAINT_START + 108)
#define	TC_MAINT_110		(TC_MAINT_START + 109)
#define	TC_MAINT_111		(TC_MAINT_START + 110)
#define	TC_MAINT_112		(TC_MAINT_START + 111)
#define	TC_MAINT_113		(TC_MAINT_START + 112)
#define	TC_MAINT_114		(TC_MAINT_START + 113)
#define	TC_MAINT_115		(TC_MAINT_START + 114)
#define	TC_MAINT_116		(TC_MAINT_START + 115)
#define	TC_MAINT_117		(TC_MAINT_START + 116)
#define	TC_MAINT_118		(TC_MAINT_START + 117)
#define	TC_MAINT_119		(TC_MAINT_START + 118)
#define	TC_MAINT_120		(TC_MAINT_START + 119)
#define	TC_MAINT_121		(TC_MAINT_START + 120)
#define	TC_MAINT_122		(TC_MAINT_START + 121)
#define	TC_MAINT_123		(TC_MAINT_START + 122)
#define	TC_MAINT_124		(TC_MAINT_START + 123)
#define	TC_MAINT_125		(TC_MAINT_START + 124)
#define	TC_MAINT_126		(TC_MAINT_START + 125)
#define	TC_MAINT_127		(TC_MAINT_START + 126)
#define	TC_MAINT_128		(TC_MAINT_START + 127)
#define	TC_MAINT_129		(TC_MAINT_START + 128)
#define	TC_MAINT_130		(TC_MAINT_START + 129)
#define	TC_MAINT_131		(TC_MAINT_START + 130)
#define	TC_MAINT_132		(TC_MAINT_START + 131)
#define	TC_MAINT_133		(TC_MAINT_START + 132)
#define	TC_MAINT_134		(TC_MAINT_START + 133)
#define	TC_MAINT_135		(TC_MAINT_START + 134)
#define	TC_MAINT_136		(TC_MAINT_START + 135)
#define	TC_MAINT_137		(TC_MAINT_START + 136)
#define	TC_MAINT_138		(TC_MAINT_START + 137)
#define	TC_MAINT_139		(TC_MAINT_START + 138)
#define	TC_MAINT_140		(TC_MAINT_START + 139)
#define	TC_MAINT_141		(TC_MAINT_START + 140)
#define	TC_MAINT_END		(TC_MAINT_START + 140)

#endif

