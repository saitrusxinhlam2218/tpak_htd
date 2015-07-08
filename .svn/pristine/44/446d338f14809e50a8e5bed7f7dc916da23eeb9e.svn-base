 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  mad_error.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:47:27
 * @(#)  Last delta: 12/2/94 at 18:17:44
 * @(#)  SCCS File: /taxi/sccs/s.mad_error.h
 *                                                                           *
 *****************************************************************************/
#ifndef _MAD_ERROR_H_
#define _MAD_ERROR_H_
#include "STDIO.h"
#include "dec_lint.h"
#include "tmplen.h"
char trace_str[132];	/* trace file buffer */
char error_str[132];	/* error file buffer */
FILE *trace_fp;		/* trace file pointer */
extern mad_error();
/* ERROR expects arguments where x=fleet id, y=cisam file and z = a brief description of the error */
#define ERROR(x,y,z) mad_error(x,y,z,__FILE__,__LINE__);
#define TRACE(y) {fprintf(trace_fp,"%d:%s",time((time_t *)0),y); fflush(trace_fp); }
#define LEVEL0 0
#define LEVEL1 1
#define LEVEL2 2
#define LEVEL3 3
#define LEVEL4 4
#define LEVEL5 5
#define LEVEL6 6
#define LEVEL7 7
#define LEVEL8 8
#define INVALID_ARG  15
#endif
