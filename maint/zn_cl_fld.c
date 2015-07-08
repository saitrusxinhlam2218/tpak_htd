static char sz__FILE__[]="@(#)zn_cl_fld.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  zn_cl_fld.c; Rel: 5.2.0.0; Get date: 3/26/90 at 14:02:59
 * @(#)  Last delta: 12/27/89 at 18:03:56
 * @(#)  SCCS File: /taxi/sccs/s.zn_cl_fld.c
 *                                                                           *
 *****************************************************************************/

#include "zn_m_strdefs.h"
#include <cursesX.h>
#include "mad_error.h"
#include "zn_maint.h"
#include "scr.h"
#include "zn_ary.h"
extern FILE *trace_fp;		/* trace file pointer */
extern char trace_str[];	/* trace file string */

/******************************************************************************
* zone_cl_flds - clear a data field on the screen.
*******************************************************************************/

zone_cl_flds(piu)
	int piu;	/* # of the field to be cleared */
{
	int i;

#ifdef DEBUG
	sprintf(trace_str,"Entering zone_cl_flds\n");
	TRACE(trace_str);
#endif

	/* move the cursor to the 1st char of the field to be cleared */
	move(zone_scr_fld[piu].prompt_row, zone_scr_fld[piu].prompt_col);

	/* clear the field */
	for (i=0; i < zone_scr_fld[piu].max_chars ; i++ )  
		addch(' ');

#ifdef DEBUG
	sprintf(trace_str,"Leaving zone_cl_flds\n");
	TRACE(trace_str);
#endif
	return;
}
