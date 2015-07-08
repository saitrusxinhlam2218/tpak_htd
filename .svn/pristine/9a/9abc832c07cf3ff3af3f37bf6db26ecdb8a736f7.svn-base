static char sz__FILE__[]="@(#)fill_st.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_st.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:38:24
 * @(#)  Last delta: 12/2/94 at 11:32:39
 * @(#)  SCCS File: /taxi/sccs/s.fill_st.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:fill_st.c	10.1.0.0"

#define KEY_SRC
#include "Street_type_db.h"

void
fill_st_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_st  *st;
	register int  offset = 0;

	st = (struct cisam_st*)req_buf;

	stchar(st->abbrev,&buf[offset],sizeof(st->abbrev)-1);

}

void
fill_st(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int    action;
{
	struct  cisam_st  *st;
	register int  offset = 0;

	st = (struct cisam_st*)req_buf;


	stchar(st->abbrev,&buf[offset],sizeof(st->abbrev)-1);
        offset += sizeof(st->abbrev)-1;

	stchar(st->actual,&buf[offset],sizeof(st->actual)-1);
	offset += sizeof(st->actual) -1;

	stchar(st->desc,&buf[offset],sizeof(st->desc)-1);
}

/* Load program variables from C-ISAM data record */
void
unfill_st(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_st  *st;
	register int  offset = 0;

	st = (struct cisam_st*)req_buf;

	ldchar(&buf[offset],sizeof(st->abbrev)-1,st->abbrev);
        offset += sizeof(st->abbrev)-1;

	ldchar(&buf[offset],sizeof(st->actual)-1,st->actual);
	offset += sizeof(st->actual) -1;

	ldchar(&buf[offset],sizeof(st->desc)-1,st->desc);
}
