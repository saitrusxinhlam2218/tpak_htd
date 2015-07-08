static char sz__FILE__[]="@(#)fill_hp.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_hp.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:37:50
 * @(#)  Last delta: 12/2/94 at 11:32:04
 * @(#)  SCCS File: /taxi/sccs/s.fill_hp.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:fill_hp.c	10.1.0.0"

#define KEY_SRC

#include "Help_db.h"

void
fill_hp_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_hp  *hp;
	register int  offset = 0;

	hp = (struct cisam_hp*)req_buf;

        stint(hp->nav_string,&buf[offset]);
        offset += INTSIZE;

        stint(hp->field_nbr,&buf[offset]);
        offset += INTSIZE;

        stint(hp->line_nbr,&buf[offset]);
}

void
fill_hp(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int    action;
{
	struct  cisam_hp  *hp;
	register int  offset = 0;

	hp = (struct cisam_hp*)req_buf;

        stint(hp->nav_string,&buf[offset]);
        offset += INTSIZE;

        stint(hp->field_nbr,&buf[offset]);
        offset += INTSIZE;

        stint(hp->line_nbr,&buf[offset]);
	offset += INTSIZE;

        stint(hp->like_nav_string,&buf[offset]);
        offset += INTSIZE;

        stint(hp->like_field_nbr,&buf[offset]);
        offset += INTSIZE;

	stchar(hp->help_line,&buf[offset],sizeof(hp->help_line)-1);
}

/* Load program variables from C-ISAM data record */
void
unfill_hp( req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_hp  *hp;
	register int  offset = 0;

	hp = (struct cisam_hp*)req_buf;

        hp->nav_string = ldint(&buf[offset]);
        offset += INTSIZE;

        hp->field_nbr = ldint(&buf[offset]);
        offset += INTSIZE;

       hp->line_nbr = ldint(&buf[offset]);
	offset += INTSIZE;

        hp->like_nav_string = ldint(&buf[offset]);
        offset += INTSIZE;

        hp->like_field_nbr = ldint(&buf[offset]);
        offset += INTSIZE;

	ldchar(&buf[offset],sizeof(hp->help_line)-1,hp->help_line);
}
