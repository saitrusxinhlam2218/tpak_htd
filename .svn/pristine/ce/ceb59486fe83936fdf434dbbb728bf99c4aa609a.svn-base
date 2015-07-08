static char sz__FILE__[]="@(#)men_cl_fld.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  men_cl_fld.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:48:12
 * @(#)  Last delta: 12/2/94 at 11:38:05
 * @(#)  SCCS File: /taxi/sccs/s.men_cl_fld.c
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include "mad_error.h"
#include "menu_maint.h"
#include "scr.h"
#include "menscr_ary.h"

extern FILE *trace_fp;		/* trace file pointer */
extern char trace_str[];	/* trace file string */

/******************************************************************************
* menu_cl_flds - clear a data field on the screen.
*******************************************************************************/

menu_cl_flds(piu)
	int piu;	/* # of the field to be cleared */
{
	int i;

#ifdef DEBUG
	sprintf(trace_str,"Entering menu_cl_flds\n");
	TRACE(trace_str);
#endif

	/* move the cursor to the 1st char of the field to be cleared */
	move(menu_scr_flds[piu].prompt_row, menu_scr_flds[piu].prompt_col);

	/* clear the field */
	for (i=0; i < menu_scr_flds[piu].max_chars ; i++ )  
		addch(' ');

#ifdef DEBUG
	sprintf(trace_str,"Leaving menu_cl_flds\n");
	TRACE(trace_str);
#endif
	return;
}
