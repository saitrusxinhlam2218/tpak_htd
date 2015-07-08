/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: trip_data_m_strdefs.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1996/09/06 13:52:02 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/trip_data_m_strdefs.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: trip_data_m_strdefs.h,v $	$Revision: 1.2 $"

#ifdef INTERNATIONAL

#ifndef INTER_INCLUDES
#include <nl_types.h>
#include <locale.h>
#define INTER_INCLUDES
#endif

#ifdef DECLARING
nl_catd  TRIP_DATA_MAINT_m_catd;
#else
extern nl_catd TRIP_DATA_MAINT_m_catd;
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

#define  TRIP_DATA_MAINT_SET         (1)

#define	TRIP_DATA_MAINT_START		(1)
#define	TRIP_DATA_MAINT_1		(TRIP_DATA_MAINT_START + 0)
#define	TRIP_DATA_MAINT_2		(TRIP_DATA_MAINT_START + 1)
#define	TRIP_DATA_MAINT_3		(TRIP_DATA_MAINT_START + 2)
#define	TRIP_DATA_MAINT_4		(TRIP_DATA_MAINT_START + 3)
#define	TRIP_DATA_MAINT_5		(TRIP_DATA_MAINT_START + 4)
#define	TRIP_DATA_MAINT_6		(TRIP_DATA_MAINT_START + 5)
#define	TRIP_DATA_MAINT_7		(TRIP_DATA_MAINT_START + 6)
#define	TRIP_DATA_MAINT_8		(TRIP_DATA_MAINT_START + 7)
#define	TRIP_DATA_MAINT_9		(TRIP_DATA_MAINT_START + 8)
#define	TRIP_DATA_MAINT_10		(TRIP_DATA_MAINT_START + 9)
#define	TRIP_DATA_MAINT_11		(TRIP_DATA_MAINT_START + 10)
#define	TRIP_DATA_MAINT_12		(TRIP_DATA_MAINT_START + 11)
#define	TRIP_DATA_MAINT_13		(TRIP_DATA_MAINT_START + 12)
#define	TRIP_DATA_MAINT_14		(TRIP_DATA_MAINT_START + 13)
#define	TRIP_DATA_MAINT_15		(TRIP_DATA_MAINT_START + 14)
#define	TRIP_DATA_MAINT_16		(TRIP_DATA_MAINT_START + 15)

#endif
