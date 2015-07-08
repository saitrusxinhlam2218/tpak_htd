 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  dec_lint.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:44:55
 * @(#)  Last delta: 12/2/94 at 18:09:07
 * @(#)  SCCS File: /taxi/sccs/s.dec_lint.h
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/

/* 
   Here lies the KLUDGE's for getting lint to work with the C library.
   If lint reports problems with these funcs include this file and add
   to it if needed.
 */

long	strtol();
unsigned	sleep();
void exit();
void abort();
