/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: rel_time_m_strdefs.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1996/09/06 13:49:55 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/rel_time_m_strdefs.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifdef INTERNATIONAL
#ifndef INTER_INCLUDES
#include <nl_types.h>
#include <locale.h>
#define INTER_INCLUDES
#endif
#ifdef DECLARING
nl_catd  REL_MAINT_m_catd;
#else
extern nl_catd REL_MAINT_m_catd;
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
#define  REL_MAINT_SET         (1)
#define	REL_MAINT_START		(1)
#define	REL_MAINT_1		(REL_MAINT_START + 0)
#define	REL_MAINT_2		(REL_MAINT_START + 1)
#define	REL_MAINT_3		(REL_MAINT_START + 2)
#define	REL_MAINT_4		(REL_MAINT_START + 3)
#define	REL_MAINT_5		(REL_MAINT_START + 4)
#define	REL_MAINT_6		(REL_MAINT_START + 5)
#define	REL_MAINT_7		(REL_MAINT_START + 6)
#define	REL_MAINT_8		(REL_MAINT_START + 7)
#define	REL_MAINT_9		(REL_MAINT_START + 8)
#define	REL_MAINT_10		(REL_MAINT_START + 9)
#define	REL_MAINT_11		(REL_MAINT_START + 10)
#define	REL_MAINT_12		(REL_MAINT_START + 11)
#define	REL_MAINT_13		(REL_MAINT_START + 12)
#define	REL_MAINT_14		(REL_MAINT_START + 13)
#define	REL_MAINT_15		(REL_MAINT_START + 14)
#define	REL_MAINT_16		(REL_MAINT_START + 15)
#endif
