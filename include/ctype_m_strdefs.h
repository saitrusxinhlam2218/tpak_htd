/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ctype_m_strdefs.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1996/09/06 13:46:55 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/ctype_m_strdefs.h,v $
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
nl_catd  COND_TYPE_MAINT_m_catd;
#else
extern nl_catd COND_TYPE_MAINT_m_catd;
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
#define  COND_TYPE_MAINT_SET         (1)
#define	COND_TYPE_MAINT_START		(1)
#define	COND_TYPE_MAINT_1		(COND_TYPE_MAINT_START + 0)
#define	COND_TYPE_MAINT_2		(COND_TYPE_MAINT_START + 1)
#define	COND_TYPE_MAINT_3		(COND_TYPE_MAINT_START + 2)
#define	COND_TYPE_MAINT_4		(COND_TYPE_MAINT_START + 3)
#define	COND_TYPE_MAINT_5		(COND_TYPE_MAINT_START + 4)
#define	COND_TYPE_MAINT_6		(COND_TYPE_MAINT_START + 5)
#define	COND_TYPE_MAINT_7		(COND_TYPE_MAINT_START + 6)
#define	COND_TYPE_MAINT_8		(COND_TYPE_MAINT_START + 7)
#define	COND_TYPE_MAINT_9		(COND_TYPE_MAINT_START + 8)
#define	COND_TYPE_MAINT_10		(COND_TYPE_MAINT_START + 9)
#define	COND_TYPE_MAINT_11		(COND_TYPE_MAINT_START + 10)
#define	COND_TYPE_MAINT_12		(COND_TYPE_MAINT_START + 11)
#define	COND_TYPE_MAINT_13		(COND_TYPE_MAINT_START + 12)
#define	COND_TYPE_MAINT_14		(COND_TYPE_MAINT_START + 13)
#define	COND_TYPE_MAINT_15		(COND_TYPE_MAINT_START + 14)
#define	COND_TYPE_MAINT_16		(COND_TYPE_MAINT_START + 15)
#endif
