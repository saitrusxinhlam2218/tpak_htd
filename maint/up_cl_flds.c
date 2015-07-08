static char sz__FILE__[]="@(#)up_cl_flds.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  up_cl_flds.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:48:24
 * @(#)  Last delta: 12/2/94 at 11:49:17
 * @(#)  SCCS File: /taxi/sccs/s.up_cl_flds.c
 *                                                                           *
 *****************************************************************************/
#include "up_m_strdefs.h"
#include <cursesX.h>
#include "mad_error.h"
#include "up_maint.h"
#include "scr.h"
#include "up_scr_ary.h"

extern FILE *trace_fp;		/* trace file pointer */
extern char trace_str[];	/* trace file string */

/******************************************************************************
* up_cl_flds - clear a data field on the screen.
*******************************************************************************/

up_cl_flds(piu)
	int piu;	/* # of the field to be cleared */
{
	int i;

#ifdef DEBUG
	sprintf(trace_str,"Entering up_cl_flds\n");
	TRACE(trace_str);
#endif

	/* move the cursor to the 1st char of the field to be cleared */
	move(up_scr_flds[piu].prompt_row, up_scr_flds[piu].prompt_col);

	/* clear the field */
	for (i=0; i < up_scr_flds[piu].max_chars ; i++ )  
		addch(' ');

#ifdef DEBUG
	sprintf(trace_str,"Leaving up_cl_flds\n");
	TRACE(trace_str);
#endif
}
