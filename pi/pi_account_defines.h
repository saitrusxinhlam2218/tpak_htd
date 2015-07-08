 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ab_defines.h; Rel: 10.1.0.0; Get date: 4/18/95 at 16:31:59
 * @(#)  Last delta: 12/2/94 at 11:17:19
 * @(#)  SCCS File: /taxi/sccs/s.ab_defines.h
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/


/*-----------------------------------------------------
 *	ab defines.h
 *-----------------------------------------------------
 */

#ifndef NULL
#define NULL						0
#endif

#ifndef FALSE
#define FALSE						0
#endif

#ifndef TRUE
#define TRUE						1
#endif

#define AB_DBX_MESSAGES				1
#define AB_DBX_NO_AB_CONNECT		2
#define AB_DBX_NO_DISPATCH			3
#define AB_DBX_NO_DB_WRITE			4
#define DEBUG(level) (debug_level >= level)

#ifdef TEST_AB
#define AB_TTY						"/dev/tty01"
#else
#define AB_TTY                                         "/dev/tty03"
#endif

#define AB_IN_MESSAGE_LEN			11
#define AB_OUT_MESSAGE_LEN			8
#define AB_NULL_FD					-1

#define AB_CALL_DISPATCHED_MSG		1
#define AB_INVALID_ACCOUNT_MSG		2
#define AB_NO_CARS_AVAIL_MSG		5
#define AB_SYSTEM_DOWN_MSG		6

#define AB_SEPARATOR_CHAR			'*'
#define AB_END_MESSAGE_CHAR			'\n'
#define AB_CR_CHAR				'\r'
#define AB_SEPARATOR_POSITION		2
#define AB_LINE_NUMBER_LEN			2
#define AB_ACCOUNT_NUMBER_LEN		6
#define AB_OUT_MESSAGE_NUMBER_LEN	3

#define AB_USER						"autobooker"

#define AB_LINE_MAX					8

// Account Call Exceptions
#define PI_INV_ACCOUNT        132

#define   TIMEC_TYPE_OFF  2
