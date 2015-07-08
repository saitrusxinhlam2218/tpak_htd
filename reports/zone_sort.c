static char sz__FILE__[]="@(#)zone_sort.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  zone_sort.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:22:13
 * @(#)  Last delta: 12/14/89 at 17:19:00
 * @(#)  SCCS File: /taxi/sccs/s.zone_sort.c
 *                                                                           *
 *****************************************************************************/

#include <string.h>
#include "rep_strdefs.h"

/*====================================================================
= zone_sort() -
=	Sorts an array of integers in ascending order.
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=====================================================================*/
zone_sort(array_len, sort_this, sort_with)
register int array_len;
int sort_this[];
char sort_with[][];
{
	register int i, j, temp;
	char	temp2[64];

	if (array_len <= 0) return(0); /* must have positive length */
	--array_len;			/* index from zero to array_len-1 */
	for(i=array_len; i; --i)
		for(j=0; j<i; j++)
			if( sort_this[j]>sort_this[j+1] ) {

				/* sort the super zone numbers */
				temp = sort_this[j];
				sort_this[j] = sort_this[j+1];
				sort_this[j+1] = temp;

				/* also sort the super zone names */
				strcpy(temp2, sort_with[j]);
				strcpy(sort_with[j], sort_with[j+1]);
				strcpy(sort_with[j+1], temp2);
			}	/* end if must reorder */
	return(1);
}
