static char sz__FILE__[]="@(#)fill_sz.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_sz.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:38:31
 * @(#)  Last delta: 12/2/94 at 11:32:45
 * @(#)  SCCS File: /taxi/sccs/s.fill_sz.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:fill_sz.c	10.1.0.0"

#define KEY_SRC
#include "Subzone_db.h"

void
fill_sz_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_sz  *sz;
	register int  offset = 0;

	sz = (struct cisam_sz*)req_buf;

	stchar(sz->nbr,&buf[offset],sizeof(sz->nbr)-1);

}

void
fill_sz(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int     action;
{
	struct  cisam_sz  *sz;
	register int  offset = 0;

	sz = (struct cisam_sz*)req_buf;


	stchar(sz->nbr,&buf[offset],sizeof(sz->nbr)-1);
        offset += sizeof(sz->nbr)-1;

        stint(sz->zone_fleet1,&buf[offset]);
        offset += INTSIZE;

        stint(sz->zone_fleet2,&buf[offset]);
        offset += INTSIZE;

        stint(sz->zone_fleet3,&buf[offset]);
        offset += INTSIZE;

        stint(sz->zone_fleet4,&buf[offset]);
        offset += INTSIZE;

        stint(sz->zone_fleet5,&buf[offset]);
        offset += INTSIZE;

        stint(sz->zone_fleet6,&buf[offset]);
        offset += INTSIZE;

        stint(sz->zone_fleet7,&buf[offset]);
        offset += INTSIZE;

        stint(sz->zone_fleet8,&buf[offset]);
}


/* Load program variables from C-ISAM data record */
void
unfill_sz(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_sz  *sz;
	register int  offset = 0;

	sz = (struct cisam_sz*)req_buf;

	ldchar(&buf[offset],sizeof(sz->nbr)-1,sz->nbr);
        offset += sizeof(sz->nbr)-1;

        sz->zone_fleet1 = ldint(&buf[offset]);
        offset += INTSIZE;

        sz->zone_fleet2 = ldint(&buf[offset]);
        offset += INTSIZE;

        sz->zone_fleet3 = ldint(&buf[offset]);
        offset += INTSIZE;

        sz->zone_fleet4 = ldint(&buf[offset]);
        offset += INTSIZE;

        sz->zone_fleet5 = ldint(&buf[offset]);
        offset += INTSIZE;

        sz->zone_fleet6 = ldint(&buf[offset]);
        offset += INTSIZE;

        sz->zone_fleet7 = ldint(&buf[offset]);
        offset += INTSIZE;

        sz->zone_fleet8 = ldint(&buf[offset]);
}

