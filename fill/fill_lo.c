static char sz__FILE__[]="@(#)fill_lo.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_lo.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:37:54
 * @(#)  Last delta: 12/2/94 at 11:32:08
 * @(#)  SCCS File: /taxi/sccs/s.fill_lo.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC

#include "Locality_db.h"

void
fill_lo_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_lo  *lo;
	register int  offset = 0;

	lo = (struct cisam_lo*)req_buf;

	stchar(lo->code,&buf[offset],sizeof(lo->code)-1);
        stchar(lo->subzone, &buf[6],sizeof(lo->subzone)-1);
}

void
fill_lo(req_buf, buf,action)
     char *req_buf;
     char *buf;
     int   action;
{
	struct  cisam_lo  *lo;
	register int  offset = 0;

	lo = (struct cisam_lo*)req_buf;

	stchar(lo->code,&buf[offset],sizeof(lo->code)-1);
	offset += sizeof(lo->code)-1;

	stchar(lo->act_city_code,&buf[offset],sizeof(lo->act_city_code)-1);
	offset += sizeof(lo->act_city_code) -1;

	stchar(lo->subzone,&buf[offset],sizeof(lo->subzone)-1);
	offset += sizeof(lo->subzone) - 1;

	stlong(lo->zip,&buf[offset]);
	offset += LONGSIZE;

	stchar(lo->full_name,&buf[offset],sizeof(lo->full_name)-1);

}

/* Load program variables from C-ISAM data record */
void
unfill_lo(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_lo  *lo;
	register int  offset = 0;

	lo = (struct cisam_lo*)req_buf;

	ldchar(&buf[offset],sizeof(lo->code)-1,lo->code);
	offset += sizeof(lo->code) -1;

	ldchar(&buf[offset],sizeof(lo->act_city_code)-1,lo->act_city_code);
	offset += sizeof(lo->act_city_code) -1;

	ldchar(&buf[offset],sizeof(lo->subzone)-1,lo->subzone);
	offset += sizeof(lo->subzone) - 1;

	lo->zip = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ldchar(&buf[offset],sizeof(lo->full_name)-1,lo->full_name);
}
