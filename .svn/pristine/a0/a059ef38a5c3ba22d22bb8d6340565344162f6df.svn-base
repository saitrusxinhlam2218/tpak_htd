static char sz__FILE__[]="@(#)fill_hl.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_hl.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:37:48
 * @(#)  Last delta: 12/2/94 at 11:32:02
 * @(#)  SCCS File: /taxi/sccs/s.fill_hl.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:fill_hl.c	10.1.0.0"

#define KEY_SRC

#include "Holiday_db.h"

void
fill_hl_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_hl  *hl;
	register int  offset = 0;

	hl = (struct cisam_hl*)req_buf;

        stlong(hl->mon,&buf[offset]);
        offset += LONGSIZE;

        stlong(hl->day,&buf[offset]);
        offset += LONGSIZE;

        stlong(hl->year,&buf[offset]);
}

void
fill_hl(req_buf,buf, action)
     char *req_buf;
     char *buf;
     int   action;
{
	struct  cisam_hl  *hl;
	register int  offset = 0;

	hl = (struct cisam_hl*)req_buf;

        stlong(hl->mon,&buf[offset]);
        offset += LONGSIZE;

        stlong(hl->day,&buf[offset]);
        offset += LONGSIZE;

        stlong(hl->year,&buf[offset]);
        offset += LONGSIZE;

	stchar(hl->desc,&buf[offset],sizeof(hl->desc)-1);
}

/* Load program variables from C-ISAM data record */
void
unfill_hl(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_hl  *hl;
	register int  offset = 0;

	hl = (struct cisam_hl*)req_buf;

	hl->mon = ldlong(&buf[offset]);
        offset += LONGSIZE;

	hl->day = ldlong(&buf[offset]);
        offset += LONGSIZE;

	hl->year = ldlong(&buf[offset]);
        offset += LONGSIZE;

	ldchar(&buf[offset],sizeof(hl->desc)-1,hl->desc);
}
