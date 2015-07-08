static char sz__FILE__[]="@(#)dr_cl_flds.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  dr_cl_flds.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:47:17
 * @(#)  Last delta: 12/2/94 at 11:29:36
 * @(#)  SCCS File: /taxi/sccs/s.dr_cl_flds.c
 *                                                                           *
 *****************************************************************************/

#include <curses.h>
#include "mad_error.h"
#include "dr_maint.h"
#include "scr.h"
#include "dr_scr_ary.h"

extern FILE *trace_fp;		/* trace file pointer */
extern char trace_str[];	/* trace file string */

/******************************************************************************
* dr_cl_flds - clear a data field on the screen.
*******************************************************************************/

dr_cl_flds(piu)
	int piu;	/* # of the field to be cleared */
{
	int i;

#ifdef DEBUG
	sprintf(trace_str,"Entering dr_cl_flds\n");
	TRACE(trace_str);
#endif

	/* move the cursor to the 1st char of the field to be cleared */
        move(dr_scr_flds[piu].prompt_row, dr_scr_flds[piu].prompt_col);

	/* clear the field */
	for (i=0; i < dr_scr_flds[piu].max_chars ; i++ )  
		addch(' ');
	
	mvaddstr( D_ATTR_ROW + 1, 0, "                                          " );
	mvaddstr( D_ATTR_ROW + 2, 0, "                                          " );

#ifdef DEBUG
	sprintf(trace_str,"Leaving dr_cl_flds\n");
	TRACE(trace_str);
#endif
}
