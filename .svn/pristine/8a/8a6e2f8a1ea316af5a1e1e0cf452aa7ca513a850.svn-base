static char sz__FILE__[]="@(#)fill_el.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_el.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:37:37
 * @(#)  Last delta: 12/2/94 at 11:31:46
 * @(#)  SCCS File: /taxi/sccs/s.fill_el.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:fill_el.c	10.1.0.0"

#define KEY_SRC

#include "Error_db.h"
#include "taxi_db.h"

void
fill_el_key( req_buf, buf )
     char *req_buf;
     char *buf;
{
	struct  cisam_el  *el;
	register int  offset = 0;

	el = (struct cisam_el*)req_buf;
	stlong(el->nbr,&buf[offset]);
}

void
fill_el( req_buf, buf, action)
     char *req_buf;
     char *buf;
     int    action;
{
	struct  cisam_el  *el;
	register int  offset = 0;
        long serial_nbr = 0;

	el = (struct cisam_el*)req_buf;

	if (action == ADD_REC)
          {
            isuniqueid(Db_files[ERROR_FILE_ID].fd,&serial_nbr);
            el->nbr = (int)serial_nbr;
          }
        
	stlong(el->nbr,&buf[offset]);
        offset += LONGSIZE;

        stchar(&el->fleet,&buf[offset],sizeof(el->fleet));
        offset += sizeof(el->fleet);

        stchar(el->file,&buf[offset],sizeof(el->file)-1);
        offset += sizeof(el->file)-1;

        stchar(el->date,&buf[offset],sizeof(el->date)-1);
        offset += sizeof(el->date)-1;

        stchar(el->time,&buf[offset],sizeof(el->time)-1);
        offset += sizeof(el->time)-1;

        stchar(el->source_file,&buf[offset],sizeof(el->source_file)-1);
        offset += sizeof(el->source_file)-1;

	stint(el->line_nbr,&buf[offset]);
        offset += INTSIZE;

        stchar(el->error_msg,&buf[offset],sizeof(el->error_msg)-1);
}

/* Loel program variables from C-ISAM data record */
void
unfill_el( req_buf, buf )
     char *req_buf;
     char *buf;
{
	struct  cisam_el  *el;
	register int  offset = 0;

	el = (struct cisam_el*)req_buf;

	el->nbr = ldlong(&buf[offset]);
        offset += LONGSIZE;

        el->fleet = buf[offset];
        offset += sizeof(el->fleet);

        ldchar(&buf[offset],sizeof(el->file)-1,el->file);
        offset += sizeof(el->file)-1;

        ldchar(&buf[offset],sizeof(el->date)-1,el->date);
        offset += sizeof(el->date)-1;

        ldchar(&buf[offset],sizeof(el->time)-1,el->time);
        offset += sizeof(el->time)-1;

        ldchar(&buf[offset],sizeof(el->source_file)-1,el->source_file);
        offset += sizeof(el->source_file)-1;

	el->line_nbr = ldint(&buf[offset]);
        offset += INTSIZE;

        ldchar(&buf[offset],sizeof(el->error_msg)-1,el->error_msg);
}

