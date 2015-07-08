static char sz__FILE__[]="@(#)vh_cl_flds.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  vh_cl_flds.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:48:37
 * @(#)  Last delta: 12/2/94 at 11:50:05
 * @(#)  SCCS File: /taxi/sccs/s.vh_cl_flds.c
 *                                                                           *
 *****************************************************************************/

#include "vh_m_strdefs.h"
#include <cursesX.h>
#include "mad_error.h"
#include "vh_maint.h"
#include "scr.h"
#include "vh_scr_ary.h"

extern FILE *trace_fp;		/* trace file pointer */
extern char trace_str[];	/* trace file string */

/******************************************************************************
* vh_cl_flds - clear a data field on the screen.
*******************************************************************************/

vh_cl_flds(piu)
	int piu;	/* # of the field to be cleared */
{
	int i;

#ifdef DEBUG
	sprintf(trace_str,"Entering vh_cl_flds\n");
	TRACE(trace_str);
#endif

	/* move the cursor to the 1st char of the field to be cleared */
	move(vh_scr_flds[piu].prompt_row, vh_scr_flds[piu].prompt_col);

	/* clear the field */
	for (i=0; i < vh_scr_flds[piu].max_chars ; i++ )  
		addch(' ');

	mvaddstr( D_ATTR_ROW + 1, 0, "                                               " );
	mvaddstr( D_ATTR_ROW + 2, 0, "                                               " );
#ifdef DEBUG
	sprintf(trace_str,"Leaving vh_cl_flds\n");
	TRACE(trace_str);
#endif
}
