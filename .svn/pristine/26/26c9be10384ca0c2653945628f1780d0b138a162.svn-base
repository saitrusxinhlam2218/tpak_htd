static char sz__FILE__[]="@(#)fill_cm.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_cm.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:37:28
 * @(#)  Last delta: 12/2/94 at 11:31:39
 * @(#)  SCCS File: /taxi/sccs/s.fill_cm.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:fill_cm.c	10.1.0.0"

#define KEY_SRC

#include "Canmsg_db.h"

void
fill_cm_key( req_buf, buf )
     char *req_buf;
     char *buf;
{
	struct  cisam_cm  *cm;
	register int  offset = 0;

	cm = (struct cisam_cm*)req_buf;

        stlong(cm->nbr,&buf[offset]);
}

void
fill_cm(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int     action;
{
	struct  cisam_cm  *cm;
	register int  offset = 0;

	cm = (struct cisam_cm*)req_buf;

        stlong(cm->nbr,&buf[offset]);
        offset += LONGSIZE;

        stchar(cm->text,&buf[offset],sizeof(cm->text)-1);
}

/* Load program variables from C-ISAM data record */
void
unfill_cm( req_buf, buf )
     char *req_buf;
     char *buf;
{
	struct  cisam_cm  *cm;
	register int  offset = 0;

	cm = (struct cisam_cm*)req_buf;

        cm->nbr = ldlong(&buf[offset]);
        offset += LONGSIZE;

	ldchar(&buf[offset],sizeof(cm->text)-1,cm->text);
}
