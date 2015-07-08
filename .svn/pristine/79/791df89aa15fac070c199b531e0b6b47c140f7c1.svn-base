static char sz__FILE__[]="@(#)fill_at.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_at.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:37:21
 * @(#)  Last delta: 12/2/94 at 11:31:31
 * @(#)  SCCS File: /taxi/sccs/s.fill_at.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:fill_at.c	10.1.0.0"
#ident "@(#) head:fill_at.c	4.1.0.0"

#define KEY_SRC

#include <isam.h>
#include "Attribute_db.h"


fill_at_key( char *from, char *buf )
{
	ATTRIB_REC  *at = (ATTRIB_REC *)from;
	register int  offset = 0;

	stchar(&at->fleet,&buf[offset],sizeof(at->fleet));
        offset += sizeof(at->fleet);
	stchar(&at->type,&buf[offset],sizeof(at->type));
        offset += sizeof(at->type);
	stint(at->nbr,&buf[offset]);
}


fill_at(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int   action;
{
	struct  cisam_at  *at;
	register int  offset = 0;

	at = (struct cisam_at*)req_buf;

	stchar(&at->fleet,&buf[offset],sizeof(at->fleet));
        offset += sizeof(at->fleet);

	stchar(&at->type,&buf[offset],sizeof(at->type));
        offset += sizeof(at->type);

	stint(at->nbr,&buf[offset]);
        offset += sizeof(at->nbr);
        
	stchar(at->desc,&buf[offset],sizeof(at->desc)-1);
        offset += sizeof(at->desc)-1;

	stchar(at->abbrev,&buf[offset],sizeof(at->abbrev)-1);
	offset += sizeof(at->abbrev) - 1;

	stfloat( at->factor_value, &buf[ offset ] );
	offset += FLOATSIZE;

	stint( at->extra_value, &buf[ offset ] );
	offset += INTSIZE;
}

unfill_at( req_buf, buf )
     char *req_buf;
     char *buf;
{
	struct  cisam_at  *at;
	register int  offset = 0;

	at = (struct cisam_at*)req_buf;

	at->fleet = buf[offset];
        offset += sizeof(at->fleet);

	at->type = buf[offset];
        offset += sizeof(at->type);

	at->nbr = ldint(&buf[offset]);
        offset += sizeof(at->nbr);
        
	ldchar(&buf[offset],sizeof(at->desc)-1,at->desc);
        offset += sizeof(at->desc)-1;

	ldchar(&buf[offset],sizeof(at->abbrev)-1,at->abbrev);
	offset += sizeof( at->abbrev ) - 1;

	at->factor_value = ldfloat( &buf[ offset ] );
	offset += FLOATSIZE;

	at->extra_value = ldint( &buf[ offset ] );
	offset += INTSIZE;
}
