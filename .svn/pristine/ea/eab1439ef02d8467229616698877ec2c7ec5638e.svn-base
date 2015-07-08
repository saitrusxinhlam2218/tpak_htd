static char sz__FILE__[]="@(#)rst_sv_cls.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  rst_sv_cls.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:49:28
 * @(#)  Last delta: 12/2/94 at 18:21:56
 * @(#)  SCCS File: /taxi/sccs/s.rst_sv_cls.c
 *                                                                           *
 *****************************************************************************/

#include <sys/types.h>
#include "srch_const.h"
#include "srch.h"
#include "mads_types.h"
#include "mads_isam.h"

extern struct cisam_cl srch_calls[SEARCHES_ON_PG];
extern struct cisam_cl saved_srch_calls[SEARCHES_ON_PG];
extern struct select_criterias select_criteria;
extern struct select_criterias saved_srch_criteria;

extern int page_num;		/* page number for search screens */
extern int saved_pg_no;		/* page number saved by menu manager */

void restor_svd_cls(num_calls)
int num_calls;
{
	int i;

	for(i = 0; i < num_calls ; i++)
		srch_calls[i] = saved_srch_calls[i];

	page_num = saved_pg_no;

	memcpy(&select_criteria,&saved_srch_criteria,sizeof(struct select_criterias));
}
