/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fatal.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1997/08/09 17:54:14 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/common/fatal.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: fatal.c,v 1.2 1997/08/09 17:54:14 taxiadm Exp $";

#include <stdio.h>
#include <varargs.h>

fatal(va_alist)
va_dcl
{
 
  
    va_list args;
    char * fmt;
    extern int errno;
	 
		 
		 
    va_start(args);
    fmt = va_arg(args, char *);
    vprintf(fmt, args);
    va_end(args);
    fflush(stdout);

    exit(0);
}
					 
					 
