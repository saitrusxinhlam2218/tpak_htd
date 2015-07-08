static char sz__FILE__[]="@(#)geo_cl_fld.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  geo_cl_fld.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:47:45
 * @(#)  Last delta: 12/2/94 at 11:33:47
 * @(#)  SCCS File: /taxi/sccs/s.geo_cl_fld.c
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include "mad_error.h"
#include "geo_scr.h"
#include "geo_ary.h"

extern FILE *trace_fp;		/* trace file pointer */
extern char trace_str[];	/* trace file string */

/******************************************************************************
* geo_cl_flds - clear a data field on the screen.
*******************************************************************************/

geo_cl_flds(piu)
	int piu;	/* # of the field to be cleared */
{
	int i;

#ifdef DEBUG
	sprintf(trace_str,"Entering geo_cl_flds\n");
	TRACE(trace_str);
#endif

	/* move the cursor to the 1st char of the field to be cleared */
	move(geo_fld[piu].prompt_row, geo_fld[piu].prompt_col);

	/* clear the field */
	for (i=0; i < geo_fld[piu].max_chars ; i++ )  
		addch(' ');

#ifdef DEBUG
	sprintf(trace_str,"Leaving geo_cl_flds\n");
	TRACE(trace_str);
#endif
	return;
}
