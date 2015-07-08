 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  sp_ary.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:49:17
 * @(#)  Last delta: 12/2/94 at 18:22:49
 * @(#)  SCCS File: /taxi/sccs/s.sp_ary.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:sp_ary.h	10.1.2.1"

#define 	MAX_PIU_SUSPEND		25
						/* declare structure for saving field values */
struct scr_flds {
	int prompt_row;				/* row on which prompt appears */
	int prompt_col;				/* col on which prompt appears */
	int max_chars;				/* max chars allowed on a field */
	int tab_field;				/* piu value to move to if tab key pressed */
};

#include "sp_fld_name.h"
