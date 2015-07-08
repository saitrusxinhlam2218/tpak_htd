static char sz__FILE__[]="@(#)usprintf.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  usprintf.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:33:25
 * @(#)  Last delta: 12/2/94 at 11:49:55
 * @(#)  SCCS File: /taxi/sccs/s.usprintf.c
 *                                                                           *
 *****************************************************************************/

#include <string.h>
#include <varargs.h>

int usprintf(s, va_alist)
char * s;
va_dcl
{
    va_list args;
    char * fmt;



    va_start(args);
    fmt = va_arg(args, char *);
    vsprintf(s,fmt, args);
    va_end(args);
    return(strlen(s));
}
 
