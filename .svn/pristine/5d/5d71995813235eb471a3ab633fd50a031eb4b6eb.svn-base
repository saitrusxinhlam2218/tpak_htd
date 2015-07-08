static char sz__FILE__[]="@(#)fill_sl.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_sl.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:38:18
 * @(#)  Last delta: 12/2/94 at 11:32:32
 * @(#)  SCCS File: /taxi/sccs/s.fill_sl.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC
#include "Seal_db.h"
 
void
fill_sl_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_sl  *sl;
	register int  offset = 0;

	sl = (struct cisam_sl*)req_buf;

        stint(sl->seal_nbr,&buf[offset]);
        offset += INTSIZE;

}

void
fill_sl(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int    action;
{
	struct  cisam_sl  *sl;
	register int  offset = 0;

	sl = (struct cisam_sl*)req_buf;


        stint(sl->seal_nbr,&buf[offset]);
        offset += INTSIZE;

	stchar(sl->desc,&buf[offset],sizeof(sl->desc)-1);
	offset += sizeof(sl->desc) -1;

	stchar(sl->abbrev,&buf[offset],sizeof(sl->abbrev)-1);

}

/* Load program variables from C-ISAM data record */
void
unfill_sl(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_sl  *sl;
	register int  offset = 0;

	sl = (struct cisam_sl*)req_buf;

        sl->seal_nbr = ldint(&buf[offset]);
        offset += INTSIZE;

	ldchar(&buf[offset],sizeof(sl->desc)-1,sl->desc);
	offset += sizeof(sl->desc) -1;

	ldchar(&buf[offset],sizeof(sl->abbrev)-1,sl->abbrev);
}
