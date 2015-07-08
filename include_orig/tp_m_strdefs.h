/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: tp_m_strdefs.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1996/09/06 13:51:47 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/tp_m_strdefs.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: tp_m_strdefs.h,v $	$Revision: 1.2 $"


#ifdef INTERNATIONAL

#ifndef INTER_INCLUDES
#include <nl_types.h>
#include <locale.h>
#define INTER_INCLUDES
#endif

#ifdef DECLARING
nl_catd  TP_MAINT_m_catd;
#else
extern nl_catd TP_MAINT_m_catd;
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

#define  TP_MAINT_SET         (1)

#define	TP_MAINT_START		(1)
#define	TP_MAINT_1		(TP_MAINT_START + 0)
#define	TP_MAINT_2		(TP_MAINT_START + 1)
#define	TP_MAINT_3		(TP_MAINT_START + 2)
#define	TP_MAINT_4		(TP_MAINT_START + 3)
#define	TP_MAINT_5		(TP_MAINT_START + 4)
#define	TP_MAINT_6		(TP_MAINT_START + 5)
#define	TP_MAINT_7		(TP_MAINT_START + 6)
#define	TP_MAINT_8		(TP_MAINT_START + 7)
#define	TP_MAINT_9		(TP_MAINT_START + 8)
#define	TP_MAINT_10		(TP_MAINT_START + 9)
#define	TP_MAINT_11		(TP_MAINT_START + 10)
#define	TP_MAINT_12		(TP_MAINT_START + 11)
#define	TP_MAINT_13		(TP_MAINT_START + 12)
#define	TP_MAINT_14		(TP_MAINT_START + 13)
#define	TP_MAINT_15		(TP_MAINT_START + 14)
#define	TP_MAINT_16		(TP_MAINT_START + 15)
#define	TP_MAINT_17		(TP_MAINT_START + 16)
#define	TP_MAINT_18		(TP_MAINT_START + 17)
#define	TP_MAINT_19		(TP_MAINT_START + 18)
#define	TP_MAINT_20		(TP_MAINT_START + 19)
#define	TP_MAINT_21		(TP_MAINT_START + 20)
#define	TP_MAINT_22		(TP_MAINT_START + 21)
#define	TP_MAINT_23		(TP_MAINT_START + 22)
#define	TP_MAINT_24		(TP_MAINT_START + 23)
#define	TP_MAINT_25		(TP_MAINT_START + 24)
#define	TP_MAINT_26		(TP_MAINT_START + 25)
#define	TP_MAINT_27		(TP_MAINT_START + 26)
#define	TP_MAINT_28		(TP_MAINT_START + 27)
#define	TP_MAINT_29		(TP_MAINT_START + 28)
#define	TP_MAINT_30		(TP_MAINT_START + 29)
#define	TP_MAINT_31		(TP_MAINT_START + 30)
#define	TP_MAINT_32		(TP_MAINT_START + 31)
#define	TP_MAINT_33		(TP_MAINT_START + 32)
#define	TP_MAINT_34		(TP_MAINT_START + 33)
#define	TP_MAINT_35		(TP_MAINT_START + 34)
#define	TP_MAINT_36		(TP_MAINT_START + 35)
#define	TP_MAINT_37		(TP_MAINT_START + 36)
#define	TP_MAINT_38		(TP_MAINT_START + 37)
#define	TP_MAINT_39		(TP_MAINT_START + 38)
#define	TP_MAINT_40		(TP_MAINT_START + 39)
#define	TP_MAINT_41		(TP_MAINT_START + 40)
#define	TP_MAINT_42		(TP_MAINT_START + 41)
#define	TP_MAINT_43		(TP_MAINT_START + 42)
#define	TP_MAINT_44		(TP_MAINT_START + 43)
#define	TP_MAINT_45		(TP_MAINT_START + 44)
#define	TP_MAINT_46		(TP_MAINT_START + 45)
#define	TP_MAINT_47		(TP_MAINT_START + 46)
#define	TP_MAINT_48		(TP_MAINT_START + 47)
#define	TP_MAINT_49		(TP_MAINT_START + 48)
#define	TP_MAINT_50		(TP_MAINT_START + 49)
#define	TP_MAINT_51		(TP_MAINT_START + 50)
#define	TP_MAINT_52		(TP_MAINT_START + 51)
#define	TP_MAINT_53		(TP_MAINT_START + 52)
#define	TP_MAINT_54		(TP_MAINT_START + 53)
#define	TP_MAINT_55		(TP_MAINT_START + 54)
#define	TP_MAINT_56		(TP_MAINT_START + 55)
#define	TP_MAINT_57		(TP_MAINT_START + 56)
#define	TP_MAINT_58		(TP_MAINT_START + 57)
#define	TP_MAINT_59		(TP_MAINT_START + 58)
#define	TP_MAINT_60		(TP_MAINT_START + 59)
#define	TP_MAINT_61		(TP_MAINT_START + 60)
#define	TP_MAINT_62		(TP_MAINT_START + 61)
#define	TP_MAINT_63		(TP_MAINT_START + 62)
#define	TP_MAINT_64		(TP_MAINT_START + 63)
#define	TP_MAINT_65		(TP_MAINT_START + 64)
#define	TP_MAINT_66		(TP_MAINT_START + 65)
#define	TP_MAINT_67		(TP_MAINT_START + 66)
#define	TP_MAINT_68		(TP_MAINT_START + 67)
#define	TP_MAINT_69		(TP_MAINT_START + 68)
#define	TP_MAINT_70		(TP_MAINT_START + 69)
#define	TP_MAINT_71		(TP_MAINT_START + 70)
#define	TP_MAINT_72		(TP_MAINT_START + 71)
#define	TP_MAINT_73		(TP_MAINT_START + 72)
#define	TP_MAINT_74		(TP_MAINT_START + 73)
#define	TP_MAINT_75		(TP_MAINT_START + 74)
#define	TP_MAINT_76		(TP_MAINT_START + 75)
#define	TP_MAINT_77		(TP_MAINT_START + 76)
#define	TP_MAINT_78		(TP_MAINT_START + 77)
#define	TP_MAINT_79		(TP_MAINT_START + 78)
#define	TP_MAINT_80		(TP_MAINT_START + 79)
#define	TP_MAINT_81		(TP_MAINT_START + 80)
#define	TP_MAINT_82		(TP_MAINT_START + 81)
#define	TP_MAINT_83		(TP_MAINT_START + 82)
#define	TP_MAINT_84		(TP_MAINT_START + 83)
#define	TP_MAINT_85		(TP_MAINT_START + 84)
#define	TP_MAINT_86		(TP_MAINT_START + 85)
#define	TP_MAINT_87		(TP_MAINT_START + 86)
#define	TP_MAINT_88		(TP_MAINT_START + 87)
#define	TP_MAINT_89		(TP_MAINT_START + 88)
#define	TP_MAINT_90		(TP_MAINT_START + 89)
#define	TP_MAINT_91		(TP_MAINT_START + 90)
#define	TP_MAINT_92		(TP_MAINT_START + 91)
#define	TP_MAINT_93		(TP_MAINT_START + 92)
#define	TP_MAINT_94		(TP_MAINT_START + 93)
#define	TP_MAINT_95		(TP_MAINT_START + 94)
#define	TP_MAINT_96		(TP_MAINT_START + 95)
#define	TP_MAINT_97		(TP_MAINT_START + 96)
#define	TP_MAINT_98		(TP_MAINT_START + 97)
#define	TP_MAINT_99		(TP_MAINT_START + 98)
#define	TP_MAINT_100		(TP_MAINT_START + 99)
#define	TP_MAINT_101		(TP_MAINT_START + 100)
#define	TP_MAINT_102		(TP_MAINT_START + 101)
#define	TP_MAINT_103		(TP_MAINT_START + 102)
#define	TP_MAINT_104		(TP_MAINT_START + 103)
#define	TP_MAINT_105		(TP_MAINT_START + 104)
#define	TP_MAINT_106		(TP_MAINT_START + 105)
#define	TP_MAINT_107		(TP_MAINT_START + 106)
#define	TP_MAINT_108		(TP_MAINT_START + 107)
#define	TP_MAINT_109		(TP_MAINT_START + 108)
#define	TP_MAINT_110		(TP_MAINT_START + 109)
#define	TP_MAINT_111		(TP_MAINT_START + 110)
#define	TP_MAINT_112		(TP_MAINT_START + 111)
#define	TP_MAINT_113		(TP_MAINT_START + 112)
#define	TP_MAINT_114		(TP_MAINT_START + 113)
#define	TP_MAINT_115		(TP_MAINT_START + 114)
#define	TP_MAINT_116		(TP_MAINT_START + 115)
#define	TP_MAINT_117		(TP_MAINT_START + 116)
#define	TP_MAINT_118		(TP_MAINT_START + 117)
#define	TP_MAINT_119		(TP_MAINT_START + 118)
#define	TP_MAINT_120		(TP_MAINT_START + 119)
#define	TP_MAINT_121		(TP_MAINT_START + 120)
#define	TP_MAINT_122		(TP_MAINT_START + 121)
#define	TP_MAINT_123		(TP_MAINT_START + 122)
#define	TP_MAINT_124		(TP_MAINT_START + 123)
#define	TP_MAINT_125		(TP_MAINT_START + 124)
#define	TP_MAINT_126		(TP_MAINT_START + 125)
#define	TP_MAINT_127		(TP_MAINT_START + 126)
#define	TP_MAINT_128		(TP_MAINT_START + 127)
#define	TP_MAINT_129		(TP_MAINT_START + 128)
#define	TP_MAINT_130		(TP_MAINT_START + 129)
#define	TP_MAINT_131		(TP_MAINT_START + 130)
#define	TP_MAINT_132		(TP_MAINT_START + 131)
#define	TP_MAINT_133		(TP_MAINT_START + 132)
#define	TP_MAINT_134		(TP_MAINT_START + 133)
#define	TP_MAINT_135		(TP_MAINT_START + 134)
#define	TP_MAINT_136		(TP_MAINT_START + 135)
#define	TP_MAINT_137		(TP_MAINT_START + 136)
#define	TP_MAINT_138		(TP_MAINT_START + 137)
#define	TP_MAINT_139		(TP_MAINT_START + 138)
#define	TP_MAINT_140		(TP_MAINT_START + 139)
#define	TP_MAINT_141		(TP_MAINT_START + 140)
#define	TP_MAINT_END		(TP_MAINT_START + 140)

#endif
