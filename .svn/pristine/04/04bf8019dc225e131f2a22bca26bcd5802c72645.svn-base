static char sz__FILE__[]="@(#)fill_eg.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_eg.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:37:35
 * @(#)  Last delta: 12/2/94 at 11:31:44
 * @(#)  SCCS File: /taxi/sccs/s.fill_eg.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC
#include "Excpt_group_db.h"

void
fill_eg_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_eg  *eg;
	register int  offset = 0;

	eg = (struct cisam_eg*)req_buf;

	stint(eg->nbr,&buf[offset]);
        offset += INTSIZE;

        stchar(eg->name,&buf[offset],sizeof(eg->name) -1);       
}


fill_eg(req_buf, buf,action)
     char *req_buf;
     char *buf;
     int   action;
{
	struct  cisam_eg  *eg;
	register int  offset = 0;

	eg = (struct cisam_eg*)req_buf;

	stint(eg->nbr,&buf[offset]);
        offset += INTSIZE;

        stchar(eg->name,&buf[offset],sizeof(eg->name)-1);

}

/* Loeg program variables from C-ISAM data record */

unfill_eg(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_eg  *eg;
	register int  offset = 0;

	eg = (struct cisam_eg*)req_buf;

	eg->nbr = ldint(&buf[offset]);
        offset += INTSIZE;

        ldchar(&buf[offset],sizeof(eg->name)-1,eg->name);

}

