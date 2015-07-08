static char sz__FILE__[]="@(#)fl_cl_flds.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fl_cl_flds.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:47:30
 * @(#)  Last delta: 12/2/94 at 11:32:56
 * @(#)  SCCS File: /taxi/sccs/s.fl_cl_flds.c
 *                                                                           *
 *****************************************************************************/

#include <curses.h>
#include "mad_error.h"
#include "fl_maint.h"
#include "fl_scr.h"

extern FILE *trace_fp;		/* trace file pointer */
extern char trace_str[];	/* trace file string */
extern int scr_nbr;		/* screen number */
extern struct scr_flds fl_scr_flds[];

/******************************************************************************
* fl_cl_flds - clear a data field on the screen.
*******************************************************************************/

fl_cl_flds(piu)
	int piu;	/* # of the field to be cleared */
{
	int i;

#ifdef DEBUG
	sprintf(trace_str,"Entering fl_cl_flds\n");
	TRACE(trace_str);
#endif

	/* move the cursor to the 1st char of the field to be cleared */
/*	if((F_POST_1 <= piu) && (piu <= F_SCR_2_MAX))  
		move(fl_scr_flds[piu].prompt_row, fl_scr_flds[piu].prompt_col+1);
	else
*/
		move(fl_scr_flds[piu].prompt_row, fl_scr_flds[piu].prompt_col);

	/* clear the field */
	for (i=0; i < fl_scr_flds[piu].max_chars ; i++ )  
		addch(' ');

#ifdef DEBUG
	sprintf(trace_str,"Leaving fl_cl_flds\n");
	TRACE(trace_str);
#endif
}
