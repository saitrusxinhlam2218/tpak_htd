/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ASSERT.h,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 1995/08/09 18:07:55 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/ASSERT.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: ASSERT.h,v $	$Revision: 1.4 $"


 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ASSERT.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:43:43
 * @(#)  Last delta: 12/2/94 at 17:57:24
 * @(#)  SCCS File: /taxi/sccs/s.ASSERT.h
 *              dfdf                                                              *
 *****************************************************************************/
#ident "@(#) head:ASSERT.h	10.1.2.1"


/* HEADER */
/************************************************************************
**
** NAME:	Assert.h
**	
**	
** DESCRIPTION:
**
**     ASSERT is a macro that indicates expression is expected to
**     be true at this point in the program.  It causes an exit(2)
**     with a diagnostic comment on the standard error when
**     expression is false (0).  Compiling with the cc(1) option
**     -DNASSERT effectively deletes assert from the program.
**
**     The diagnostic message is as follows:
**
**     "Assertion 'EX' failed: file 'FI' line LI"
**
**     where:
**
**     EX is the text of the expression that failed.
**     FI is the name of the file that the ASSERT macro appeared in.
**     LI is the line number in the file that the ASSERT macro that 
**        failed is on.
**
**     ASSERT_P is identical to ASSERT in testing mode.
**     When a program goes into production, the ASSERT macro may
**     dissappear(become undefined), however the ASSERT_P macro will
**     remain.
**
**     ASSERT_B is identical to ASSERT in testing mode.
**     However the ASSERT_B macro is guaranteed to disappear in
**     production mode.
**
**     ASSERT_S is identical to ASSERT_P except that it also prints out
**     the value of errno and its associated sys_errlist[errno] message.
**     In addition, it prints out a message string.
**     This macro is useful when calling system and library functions.
**     It prints to stderr:
**
**     "Assertion 'EX' failed: file 'FI' line LI"
**     "system error: MSG (errno=ERRNO; SYSTEMMSG.)"
**
**     where:
**          MSG       is the message passed to ASSERT_S(MSG,EX)
**          ERRNO     is the value of the system global int errno
**          SYSTEMMSG is sys_errlist[errno]
**
** BUGS:
**
**     EX can not contain a literal string. There may be a way around
**     this but it's not immediatly obvious.
**	
**	
** NOTES:
**	
**	
** UPDATE HISTORY:
**	
**	Darko Vranesic	July 21, 1987		Original.
**	
**	
**	
**		Copyright (c), 1987
**		All rights reserved
**
**	MOBILE DATA INTERNATIONAL, INC.
**		RICHMOND, BRITISH COLUMBIA
**		CANADA
**
*************************************************************************/
/* END HEADER */

#ifndef	Assert_h
#define	Assert_h

#include "STDIO.h"

#ifndef NASSERT
#define ASSERT(ex) \
    if (!(ex)) {\
        fprintf(stderr,"Assertion '%s' failed: file '%s' line %d\n",\
            "ex", __FILE__, __LINE__);\
    }

#define ASSERT_P(ex) \
    if (!(ex)) {\
        fprintf(stderr,"Assertion '%s' failed: file '%s' line %d\n",\
            "ex", __FILE__, __LINE__);\
        exit(2);\
    }

#define S_ASSERT(msg,ex) \
    if (!(ex)) {\
		extern int errno, sys_nerr;\
		extern char *sys_errlist[];\
\
        fprintf(stderr,"Assertion '%s' failed: file '%s' line %d\n",\
            "ex", __FILE__, __LINE__);\
		fprintf(stderr,"system error: %s (errno=%d; ", (msg),errno);\
		if(  errno > 0  &&  errno < sys_nerr  )\
			fprintf(stderr,"%s.)\n", sys_errlist[errno]);\
		else\
			fprintf(stderr,"unknown errno value).\n");\
        exit(2);\
    }


#define ASSERT_B(ex) ASSERT(ex)

#define	ASSERT_S(msg,ex) S_ASSERT(msg,ex)

#endif

#endif
