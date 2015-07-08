static char sz__FILE__[]="@(#)sort_ints.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  sort_ints.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:22:12
 * @(#)  Last delta: 12/14/89 at 17:12:55
 * @(#)  SCCS File: /taxi/sccs/s.sort_ints.c
 *                                                                           *
 *****************************************************************************/

#include "rep_strdefs.h"

/*====================================================================
= bubble_sort_ints() -
=	Sorts an array of integers in ascending order.
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=====================================================================*/
bubble_sort_ints(array_len, sort_this)
register int array_len;
int sort_this[];
{
	register int i, j, temp;
	if (array_len <= 0) return(0); /* must have positive length */
	--array_len;			/* index from zero to array_len-1 */
	for(i=array_len; i; --i)
		for(j=0; j<i; j++)
			if( sort_this[j]>sort_this[j+1] ) {
				temp = sort_this[j];
				sort_this[j] = sort_this[j+1];
				sort_this[j+1] = temp;
			}	/* end if must reorder */
	return(1);
}
