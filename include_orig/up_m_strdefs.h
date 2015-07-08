/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  up_m_strdefs.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:50:39
* @(#)  Last delta: 12/2/94 at 18:20:44
* @(#)  SCCS File: /taxi/sccs/s.up_m_strdefs.h
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/
#ident "@(#) head:up_m_strdefs.h	10.1.2.1"

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
nl_catd UP_MAINT_m_catd;
#else
extern nl_catd UP_MAINT_m_catd;
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

#define UP_MAINT_SET		(1)

#define	UP_MAINT_START		(1)
#define	UP_MAINT_1		(UP_MAINT_START + 0)
#define	UP_MAINT_2		(UP_MAINT_START + 1)
#define	UP_MAINT_3		(UP_MAINT_START + 2)
#define	UP_MAINT_4		(UP_MAINT_START + 3)
#define	UP_MAINT_5		(UP_MAINT_START + 4)
#define	UP_MAINT_6		(UP_MAINT_START + 5)
#define	UP_MAINT_7		(UP_MAINT_START + 6)
#define	UP_MAINT_8		(UP_MAINT_START + 7)
#define	UP_MAINT_9		(UP_MAINT_START + 8)
#define	UP_MAINT_10		(UP_MAINT_START + 9)
#define	UP_MAINT_11		(UP_MAINT_START + 10)
#define	UP_MAINT_12		(UP_MAINT_START + 11)
#define	UP_MAINT_13		(UP_MAINT_START + 12)
#define	UP_MAINT_14		(UP_MAINT_START + 13)
#define	UP_MAINT_15		(UP_MAINT_START + 14)
#define	UP_MAINT_16		(UP_MAINT_START + 15)
#define	UP_MAINT_17		(UP_MAINT_START + 16)
#define	UP_MAINT_18		(UP_MAINT_START + 17)
#define	UP_MAINT_19		(UP_MAINT_START + 18)
#define	UP_MAINT_20		(UP_MAINT_START + 19)
#define	UP_MAINT_21		(UP_MAINT_START + 20)
#define	UP_MAINT_22		(UP_MAINT_START + 21)
#define	UP_MAINT_23		(UP_MAINT_START + 22)
#define	UP_MAINT_24		(UP_MAINT_START + 23)
#define	UP_MAINT_25		(UP_MAINT_START + 24)
#define	UP_MAINT_26		(UP_MAINT_START + 25)
#define	UP_MAINT_27		(UP_MAINT_START + 26)
#define	UP_MAINT_28		(UP_MAINT_START + 27)
#define	UP_MAINT_29		(UP_MAINT_START + 28)
#define	UP_MAINT_30		(UP_MAINT_START + 29)
#define	UP_MAINT_31		(UP_MAINT_START + 30)
#define	UP_MAINT_32		(UP_MAINT_START + 31)
#define	UP_MAINT_33		(UP_MAINT_START + 32)
#define	UP_MAINT_34		(UP_MAINT_START + 33)
#define	UP_MAINT_35		(UP_MAINT_START + 34)
#define	UP_MAINT_36		(UP_MAINT_START + 35)
#define	UP_MAINT_37		(UP_MAINT_START + 36)
#define	UP_MAINT_38		(UP_MAINT_START + 37)
#define	UP_MAINT_39		(UP_MAINT_START + 38)
#define	UP_MAINT_40		(UP_MAINT_START + 39)
#define	UP_MAINT_41		(UP_MAINT_START + 40)
#define	UP_MAINT_42		(UP_MAINT_START + 41)
#define	UP_MAINT_43		(UP_MAINT_START + 42)
#define	UP_MAINT_44		(UP_MAINT_START + 43)
#define	UP_MAINT_45		(UP_MAINT_START + 44)
#define	UP_MAINT_46		(UP_MAINT_START + 45)
#define	UP_MAINT_47		(UP_MAINT_START + 46)
#define	UP_MAINT_48		(UP_MAINT_START + 47)
#define	UP_MAINT_49		(UP_MAINT_START + 48)
#define	UP_MAINT_50		(UP_MAINT_START + 49)
#define	UP_MAINT_51		(UP_MAINT_START + 50)
#define	UP_MAINT_52		(UP_MAINT_START + 51)
#define	UP_MAINT_53		(UP_MAINT_START + 52)
#define	UP_MAINT_54		(UP_MAINT_START + 53)
#define	UP_MAINT_55		(UP_MAINT_START + 54)
#define	UP_MAINT_56		(UP_MAINT_START + 55)
#define	UP_MAINT_57		(UP_MAINT_START + 56)
#define	UP_MAINT_58		(UP_MAINT_START + 57)
#define	UP_MAINT_59		(UP_MAINT_START + 58)
#define	UP_MAINT_60		(UP_MAINT_START + 59)
#define	UP_MAINT_61		(UP_MAINT_START + 60)
#define	UP_MAINT_62		(UP_MAINT_START + 61)
#define	UP_MAINT_63		(UP_MAINT_START + 62)
#define	UP_MAINT_64		(UP_MAINT_START + 63)
#define	UP_MAINT_65		(UP_MAINT_START + 64)
#define	UP_MAINT_66		(UP_MAINT_START + 65)
#define	UP_MAINT_67		(UP_MAINT_START + 66)
#define	UP_MAINT_68		(UP_MAINT_START + 67)
#define	UP_MAINT_69		(UP_MAINT_START + 68)
#define	UP_MAINT_70		(UP_MAINT_START + 69)
#define	UP_MAINT_71		(UP_MAINT_START + 70)
#define	UP_MAINT_72		(UP_MAINT_START + 71)
#define	UP_MAINT_73		(UP_MAINT_START + 72)
#define	UP_MAINT_74		(UP_MAINT_START + 73)
#define	UP_MAINT_75		(UP_MAINT_START + 74)
#define	UP_MAINT_76		(UP_MAINT_START + 75)
#define	UP_MAINT_77		(UP_MAINT_START + 76)
#define	UP_MAINT_78		(UP_MAINT_START + 77)
#define UP_MAINT_79             (UP_MAINT_START + 78)
#define UP_MAINT_80             (UP_MAINT_START + 79)
#define	UP_MAINT_END		(UP_MAINT_START + 79)

#endif
