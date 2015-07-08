static char sz__FILE__[]="@(#)fill_rt.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_rt.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:38:07
 * @(#)  Last delta: 12/2/94 at 11:32:21
 * @(#)  SCCS File: /taxi/sccs/s.fill_rt.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC
#include "Rate_db.h"

void
fill_rt_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_rt  *rt;
	register int  offset = 0;

	rt = (struct cisam_rt*)req_buf;

	stchar(&rt->fleet,&buf[offset],sizeof(rt->fleet));
	offset += sizeof(rt->fleet)+sizeof(rt->pad);

        stlong(rt->zone_zip_nbr1,&buf[offset]);
        offset += LONGSIZE;

        stlong(rt->zone_zip_nbr2,&buf[offset]);

}

void
fill_rt(req_buf, buf,action)
     char *req_buf;
     char *buf;
     int     action;
{
	struct  cisam_rt  *rt;
	register int  offset = 0;

	rt = (struct cisam_rt*)req_buf;


	stchar(&rt->fleet,&buf[offset],sizeof(rt->fleet));
	offset += sizeof(rt->fleet);

	stchar(&rt->pad,&buf[offset],sizeof(rt->pad));
	offset += sizeof(rt->pad);

        stlong(rt->zone_zip_nbr1,&buf[offset]);
        offset += LONGSIZE;

        stlong(rt->zone_zip_nbr2,&buf[offset]);
        offset += LONGSIZE;

        stfloat(rt->flat_rate,&buf[offset]);
}

/* Load program variables from C-ISAM data record */
void
unfill_rt(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_rt  *rt;
	register int  offset = 0;

	rt = (struct cisam_rt*)req_buf;

	rt->fleet = buf[offset];
	offset += sizeof(rt->fleet);

	rt->pad = buf[offset];
	offset += sizeof(rt->pad);

        rt->zone_zip_nbr1 = ldlong(&buf[offset]);
        offset += LONGSIZE;

        rt->zone_zip_nbr2 = ldlong(&buf[offset]);
        offset += LONGSIZE;

        rt->flat_rate = ldfloat(&buf[offset]);
}
