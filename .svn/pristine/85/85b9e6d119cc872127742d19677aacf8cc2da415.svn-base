/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  ccp_strdefs.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:44:34		            *			
* @(#)  Last delta: 12/2/94 at 18:53:11						    *	
* @(#)  SCCS File: /taxi/sccs/s.ccp_strdefs.h							    *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS					    *
*                                                                           *
*****************************************************************************/
#ident "@(#) head:ccp_strdefs.h	10.1.2.1"

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
nl_catd CCP_catd;
#else
extern nl_catd CCP_catd;
#endif

nl_catd catopen();
char    *catgets();
char    *m_catgets();
int     catclose();

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

#define CCP_SET	2

#define AS_DOWN_MSG	1
#define AS_UP_MSG	2

#endif
