 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  calls.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:44:26
 * @(#)  Last delta: 12/2/94 at 17:55:01
 * @(#)  SCCS File: /taxi/sccs/s.calls.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:calls.h	10.1.2.1"


#include "Call_private.h"

/* call index into call node pool, asc. by call number */
struct call_index {
	int	nbr;			/* call number */
	struct calls *call_ptr; 	/* pointer to a call record */
};


