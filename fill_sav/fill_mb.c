static char sz__FILE__[]="@(#)fill_mb.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_mb.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:37:56
 * @(#)  Last delta: 12/2/94 at 11:27:21
 * @(#)  SCCS File: /taxi/sccs/s.fill_mb.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:fill_mb.c	10.1.0.0"


#define KEY_SRC

#include "Mailbox_db.h"
#include "taxi_db.h"

#define   DATE_TIME_SENT_OFFSET  10
#define   STATUS_OFFSET 214
 
void
fill_mb_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_mb  *mb;
	register int  offset = 0;

	mb = (struct cisam_mb*)req_buf;

        stlong(mb->address,&buf[offset]);

        offset = DATE_TIME_SENT_OFFSET;
        stlong(mb->date_time_sent,&buf[offset]);

        offset = STATUS_OFFSET;
	stchar(&mb->status,&buf[offset],sizeof(mb->status));

	/* msg_nbr for mb_key3 */
	offset = 4;
        stlong(mb->msg_nbr, &buf[offset]);
}

void
fill_mb(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int    action;
{
	struct  cisam_mb  *mb;
	register int  offset = 0;
	long serial_nbr = 0;
	int    rc = 0;
	mb = (struct cisam_mb*)req_buf;

        stlong(mb->address,&buf[offset]);
        offset += LONGSIZE;

	if (action == ADD_REC)
	  {
	    rc = isuniqueid(Db_files[MAILBOX_FILE_ID].fd,&serial_nbr);
	    mb->msg_nbr = (int)serial_nbr;
	  }

        stlong(mb->msg_nbr,&buf[offset]);
	offset += LONGSIZE;

        stint(mb->from,&buf[offset]);
        offset += INTSIZE;

        stlong(mb->date_time_sent,&buf[offset]);
	offset += LONGSIZE;


	stchar(mb->msg_line1,&buf[offset],sizeof(mb->msg_line1)-1);
	offset += sizeof(mb->msg_line1) -1;

	stchar(mb->msg_line2,&buf[offset],sizeof(mb->msg_line2)-1);
	offset += sizeof(mb->msg_line2) -1;


	stchar(&mb->status,&buf[offset],sizeof(mb->status));
	offset += sizeof(mb->status);

	stchar(&mb->fleet,&buf[offset],sizeof(mb->fleet));
	offset += sizeof(mb->fleet);

	stchar(&mb->to_type,&buf[offset],sizeof(mb->to_type));
	offset += sizeof(mb->to_type);

	stchar(mb->to_selection,&buf[offset],sizeof(mb->to_selection) -1);
	offset += sizeof(mb->to_selection) -1;

	stchar(mb->date_sent,&buf[offset],sizeof(mb->date_sent) -1);
	offset += sizeof(mb->date_sent) -1;

	stchar(mb->time_sent,&buf[offset],sizeof(mb->time_sent) -1);
	offset += sizeof(mb->time_sent) -1;

        stlong(mb->msg_type,&buf[offset]);

}

/* Load program variables from C-ISAM data record */
void
unfill_mb(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_mb  *mb;
	register int  offset = 0;

	mb = (struct cisam_mb*)req_buf;

        mb->address = ldlong(&buf[offset]);
        offset += LONGSIZE;

        mb->msg_nbr = ldlong(&buf[offset]);
	offset += LONGSIZE;

        mb->from = ldint(&buf[offset]);
        offset += INTSIZE;

        mb->date_time_sent = ldlong(&buf[offset]);
	offset += LONGSIZE;


	ldchar(&buf[offset],sizeof(mb->msg_line1)-1,mb->msg_line1);
	offset += sizeof(mb->msg_line1) -1;

	ldchar(&buf[offset],sizeof(mb->msg_line2)-1,mb->msg_line2);
	offset += sizeof(mb->msg_line2) -1;

	mb->status = buf[offset];
        offset += sizeof(mb->status);

	mb->fleet = buf[offset];
        offset += sizeof(mb->fleet);

	mb->to_type = buf[offset];
        offset += sizeof(mb->to_type);

	ldchar(&buf[offset],sizeof(mb->to_selection)-1,mb->to_selection);
	offset += sizeof(mb->to_selection) -1;

	ldchar(&buf[offset],sizeof(mb->date_sent)-1,mb->date_sent);
	offset += sizeof(mb->date_sent) -1;

	ldchar(&buf[offset],sizeof(mb->time_sent)-1,mb->time_sent);
	offset += sizeof(mb->time_sent) -1;

        mb->msg_type = ldlong(&buf[offset]);

}
