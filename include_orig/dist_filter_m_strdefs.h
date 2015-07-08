/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: dist_filter_m_strdefs.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1996/11/19 01:53:05 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/dist_filter_m_strdefs.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: dist_filter_m_strdefs.h,v $	$Revision: 1.2 $"


#ifdef INTERNATIONAL

#ifndef INTER_INCLUDES
#include <nl_types.h>
#include <locale.h>
#define INTER_INCLUDES
#endif

#ifdef DECLARING
nl_catd  DF_MAINT_m_catd;
#else
extern nl_catd DF_MAINT_m_catd;
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

#define  DF_MAINT_SET         (1)

#define	DF_MAINT_START		(1)
#define	DF_MAINT_1		(DF_MAINT_START + 0)
#define	DF_MAINT_2		(DF_MAINT_START + 1)
#define	DF_MAINT_3		(DF_MAINT_START + 2)
#define	DF_MAINT_4		(DF_MAINT_START + 3)
#define	DF_MAINT_5		(DF_MAINT_START + 4)
#define	DF_MAINT_6		(DF_MAINT_START + 5)
#define	DF_MAINT_7		(DF_MAINT_START + 6)
#define	DF_MAINT_8		(DF_MAINT_START + 7)
#define	DF_MAINT_9		(DF_MAINT_START + 8)
#define	DF_MAINT_10		(DF_MAINT_START + 9)
#define	DF_MAINT_11		(DF_MAINT_START + 10)
#define	DF_MAINT_12		(DF_MAINT_START + 11)
#define	DF_MAINT_13		(DF_MAINT_START + 12)
#define	DF_MAINT_14		(DF_MAINT_START + 13)
#define	DF_MAINT_15		(DF_MAINT_START + 14)
#define	DF_MAINT_16		(DF_MAINT_START + 15)
#define	DF_MAINT_17		(DF_MAINT_START + 16)
#define	DF_MAINT_18		(DF_MAINT_START + 17)
#define	DF_MAINT_19		(DF_MAINT_START + 18)
#define	DF_MAINT_20		(DF_MAINT_START + 19)
#define	DF_MAINT_21		(DF_MAINT_START + 20)
#define	DF_MAINT_22		(DF_MAINT_START + 21)
#define	DF_MAINT_23		(DF_MAINT_START + 22)
#define	DF_MAINT_24		(DF_MAINT_START + 23)
#define	DF_MAINT_25		(DF_MAINT_START + 24)
#define	DF_MAINT_26		(DF_MAINT_START + 25)
#define	DF_MAINT_27		(DF_MAINT_START + 26)
#define	DF_MAINT_28		(DF_MAINT_START + 27)
#define	DF_MAINT_29		(DF_MAINT_START + 28)
#define	DF_MAINT_30		(DF_MAINT_START + 29)
#define	DF_MAINT_31		(DF_MAINT_START + 30)
#define	DF_MAINT_32		(DF_MAINT_START + 31)
#define	DF_MAINT_33		(DF_MAINT_START + 32)
#define	DF_MAINT_34		(DF_MAINT_START + 33)
#define	DF_MAINT_35		(DF_MAINT_START + 34)
#define	DF_MAINT_36		(DF_MAINT_START + 35)
#define	DF_MAINT_37		(DF_MAINT_START + 36)
#define	DF_MAINT_38		(DF_MAINT_START + 37)
#define	DF_MAINT_39		(DF_MAINT_START + 38)
#define	DF_MAINT_40		(DF_MAINT_START + 39)
#define	DF_MAINT_41		(DF_MAINT_START + 40)
#define	DF_MAINT_42		(DF_MAINT_START + 41)
#define	DF_MAINT_43		(DF_MAINT_START + 42)
#define	DF_MAINT_44		(DF_MAINT_START + 43)
#define	DF_MAINT_45		(DF_MAINT_START + 44)
#define	DF_MAINT_46		(DF_MAINT_START + 45)
#define	DF_MAINT_47		(DF_MAINT_START + 46)
#define	DF_MAINT_48		(DF_MAINT_START + 47)
#define	DF_MAINT_49		(DF_MAINT_START + 48)
#define	DF_MAINT_50		(DF_MAINT_START + 49)
#define	DF_MAINT_51		(DF_MAINT_START + 50)
#define	DF_MAINT_52		(DF_MAINT_START + 51)
#define	DF_MAINT_53		(DF_MAINT_START + 52)
#define	DF_MAINT_54		(DF_MAINT_START + 53)
#define	DF_MAINT_55		(DF_MAINT_START + 54)
#define	DF_MAINT_56		(DF_MAINT_START + 55)
#define	DF_MAINT_57		(DF_MAINT_START + 56)
#define	DF_MAINT_58		(DF_MAINT_START + 57)
#define	DF_MAINT_59		(DF_MAINT_START + 58)
#define	DF_MAINT_60		(DF_MAINT_START + 59)
#define	DF_MAINT_61		(DF_MAINT_START + 60)
#define	DF_MAINT_62		(DF_MAINT_START + 61)
#define	DF_MAINT_63		(DF_MAINT_START + 62)
#define	DF_MAINT_64		(DF_MAINT_START + 63)
#define	DF_MAINT_65		(DF_MAINT_START + 64)
#define	DF_MAINT_66		(DF_MAINT_START + 65)
#define	DF_MAINT_67		(DF_MAINT_START + 66)
#define	DF_MAINT_68		(DF_MAINT_START + 67)
#define	DF_MAINT_69		(DF_MAINT_START + 68)
#define	DF_MAINT_70		(DF_MAINT_START + 69)
#define	DF_MAINT_71		(DF_MAINT_START + 70)
#define	DF_MAINT_72		(DF_MAINT_START + 71)
#define	DF_MAINT_73		(DF_MAINT_START + 72)
#define	DF_MAINT_74		(DF_MAINT_START + 73)
#define	DF_MAINT_75		(DF_MAINT_START + 74)
#define	DF_MAINT_76		(DF_MAINT_START + 75)
#define	DF_MAINT_77		(DF_MAINT_START + 76)
#define	DF_MAINT_78		(DF_MAINT_START + 77)
#define	DF_MAINT_79		(DF_MAINT_START + 78)
#define	DF_MAINT_80		(DF_MAINT_START + 79)
#define	DF_MAINT_81		(DF_MAINT_START + 80)
#define	DF_MAINT_82		(DF_MAINT_START + 81)
#define	DF_MAINT_83		(DF_MAINT_START + 82)
#define	DF_MAINT_84		(DF_MAINT_START + 83)
#define	DF_MAINT_85		(DF_MAINT_START + 84)
#define	DF_MAINT_86		(DF_MAINT_START + 85)
#define	DF_MAINT_87		(DF_MAINT_START + 86)
#define	DF_MAINT_88		(DF_MAINT_START + 87)
#define	DF_MAINT_89		(DF_MAINT_START + 88)
#define	DF_MAINT_90		(DF_MAINT_START + 89)
#define	DF_MAINT_91		(DF_MAINT_START + 90)
#define	DF_MAINT_92		(DF_MAINT_START + 91)
#define	DF_MAINT_93		(DF_MAINT_START + 92)
#define	DF_MAINT_94		(DF_MAINT_START + 93)
#define	DF_MAINT_95		(DF_MAINT_START + 94)
#define	DF_MAINT_96		(DF_MAINT_START + 95)
#define	DF_MAINT_97		(DF_MAINT_START + 96)
#define	DF_MAINT_98		(DF_MAINT_START + 97)
#define	DF_MAINT_99		(DF_MAINT_START + 98)
#define	DF_MAINT_100		(DF_MAINT_START + 99)
#define	DF_MAINT_101		(DF_MAINT_START + 100)
#define	DF_MAINT_102		(DF_MAINT_START + 101)
#define	DF_MAINT_103		(DF_MAINT_START + 102)
#define	DF_MAINT_104		(DF_MAINT_START + 103)
#define	DF_MAINT_105		(DF_MAINT_START + 104)
#define	DF_MAINT_106		(DF_MAINT_START + 105)
#define	DF_MAINT_107		(DF_MAINT_START + 106)
#define	DF_MAINT_108		(DF_MAINT_START + 107)
#define	DF_MAINT_109		(DF_MAINT_START + 108)
#define	DF_MAINT_110		(DF_MAINT_START + 109)
#define	DF_MAINT_111		(DF_MAINT_START + 110)
#define	DF_MAINT_112		(DF_MAINT_START + 111)
#define	DF_MAINT_113		(DF_MAINT_START + 112)
#define	DF_MAINT_114		(DF_MAINT_START + 113)
#define	DF_MAINT_115		(DF_MAINT_START + 114)
#define	DF_MAINT_116		(DF_MAINT_START + 115)
#define	DF_MAINT_117		(DF_MAINT_START + 116)
#define	DF_MAINT_118		(DF_MAINT_START + 117)
#define	DF_MAINT_119		(DF_MAINT_START + 118)
#define	DF_MAINT_120		(DF_MAINT_START + 119)
#define	DF_MAINT_121		(DF_MAINT_START + 120)
#define	DF_MAINT_122		(DF_MAINT_START + 121)
#define	DF_MAINT_123		(DF_MAINT_START + 122)
#define	DF_MAINT_124		(DF_MAINT_START + 123)
#define	DF_MAINT_125		(DF_MAINT_START + 124)
#define	DF_MAINT_126		(DF_MAINT_START + 125)
#define	DF_MAINT_127		(DF_MAINT_START + 126)
#define	DF_MAINT_128		(DF_MAINT_START + 127)
#define	DF_MAINT_129		(DF_MAINT_START + 128)
#define	DF_MAINT_130		(DF_MAINT_START + 129)
#define	DF_MAINT_131		(DF_MAINT_START + 130)
#define	DF_MAINT_132		(DF_MAINT_START + 131)
#define	DF_MAINT_133		(DF_MAINT_START + 132)
#define	DF_MAINT_134		(DF_MAINT_START + 133)
#define	DF_MAINT_135		(DF_MAINT_START + 134)
#define	DF_MAINT_136		(DF_MAINT_START + 135)
#define	DF_MAINT_137		(DF_MAINT_START + 136)
#define	DF_MAINT_138		(DF_MAINT_START + 137)
#define	DF_MAINT_139		(DF_MAINT_START + 138)
#define	DF_MAINT_140		(DF_MAINT_START + 139)
#define	DF_MAINT_141		(DF_MAINT_START + 140)
#define	DF_MAINT_END		(DF_MAINT_START + 140)

#endif