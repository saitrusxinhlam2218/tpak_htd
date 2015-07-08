static char sz__FILE__[]="@(#)fill_et.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_et.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:37:41
 * @(#)  Last delta: 12/2/94 at 11:31:55
 * @(#)  SCCS File: /taxi/sccs/s.fill_et.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC
#include "Excpt_type_db.h"

#define  CONST  34

void
fill_et_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_et  *et;
	register int  offset = 0;

	et = (struct cisam_et*)req_buf;

	stint(et->nbr,&buf[offset]);
        offset = CONST;
        stchar(et->short_desc,&buf[offset],sizeof(et->short_desc) -1);       
}

void
fill_et(req_buf, buf,action)
     char *req_buf;
     char *buf;
     int   action;
{
	struct  cisam_et  *et;
	register int  offset = 0;

	et = (struct cisam_et*)req_buf;

	stint(et->nbr,&buf[offset]);
        offset += INTSIZE;

        stint(et->group_nbr,&buf[offset]);
        offset += INTSIZE;

        stchar(et->desc,&buf[offset],sizeof(et->desc)-1);
        offset += sizeof(et->desc)-1;

        stchar(et->short_desc,&buf[offset],sizeof(et->short_desc)-1);

}

/* Loet program variables from C-ISAM data record */
void
unfill_et(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_et  *et;
	register int  offset = 0;

	et = (struct cisam_et*)req_buf;

	et->nbr = ldint(&buf[offset]);
        offset += INTSIZE;

        et->group_nbr = ldint(&buf[offset]);
        offset += INTSIZE;

        ldchar(&buf[offset],sizeof(et->desc)-1,et->desc);
        offset += sizeof(et->desc)-1;

        ldchar(&buf[offset],sizeof(et->short_desc)-1,et->short_desc);

}

