static char sz__FILE__[]="@(#)fill_cmsg.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_cmsg.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:37:30
 * @(#)  Last delta: 12/2/94 at 12:16:46
 * @(#)  SCCS File: /taxi/sccs/s.fill_cmsg.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC

#include "Call_msg_db.h"

void
fill_cmsg_key( req_buf, buf )
     char *req_buf;
     char *buf;
{
	struct  cisam_cmsg  *cmsg;

	cmsg =(struct cisam_cmsg *)req_buf;

	stlong(cmsg->call_nbr,&buf[CMSG_CALL_NBR]);                         /* call nbr in TaxiPak */
	stchar(&cmsg->msg_nbr,&buf[CMSG_MSG_NBR],sizeof(cmsg->msg_nbr));    /* message number (1-9) */
	stchar(&cmsg->device,&buf[CMSG_DEVICE],sizeof(cmsg->device));       /* device (1-3) */

}

void
fill_cmsg( req_buf, buf, action)
     char *req_buf;
     char *buf;
     int  action;
{
	struct  cisam_cmsg  *cmsg;
	register int  offset = 0;

	cmsg = (struct cisam_cmsg *)req_buf;

	stlong(cmsg->call_nbr,&buf[offset]);
	offset += LONGSIZE;
	stchar(&cmsg->msg_nbr,&buf[offset],sizeof(cmsg->msg_nbr));
	offset += sizeof(cmsg->msg_nbr);
	stlong(cmsg->changed_by,&buf[offset]);
	offset += LONGSIZE;
	stlong(cmsg->last_changed,&buf[offset]);
	offset += LONGSIZE;
	stchar(&cmsg->device,&buf[offset],sizeof(cmsg->device));
	offset += sizeof(cmsg->device);
	stchar(&cmsg->ack,&buf[offset],sizeof(cmsg->ack));
	offset += sizeof(cmsg->ack);
	stchar(cmsg->msg,&buf[offset],sizeof(cmsg->msg)-1);
}

/* Load program variables from C-ISAM data record */
void
unfill_cmsg( req_buf, buf )
     char *req_buf;
     char *buf;
{
	struct  cisam_cmsg  *cmsg;
	register int  offset = 0;

	cmsg = (struct cisam_cmsg *)req_buf;

	cmsg->call_nbr = ldlong(&buf[offset]);
	offset += LONGSIZE;
	ldchar(&buf[offset],sizeof(cmsg->msg_nbr),&cmsg->msg_nbr);
	offset += sizeof(cmsg->msg_nbr);
	cmsg->changed_by = ldlong(&buf[offset]);
	offset += LONGSIZE;
	cmsg->last_changed = ldlong(&buf[offset]);
	offset += LONGSIZE;
	ldchar(&buf[offset],sizeof(cmsg->device),&cmsg->device);
	offset += sizeof(cmsg->device);
	ldchar(&buf[offset],sizeof(cmsg->ack),&cmsg->ack);
	offset += sizeof(cmsg->ack);
	ldchar(&buf[offset],sizeof(cmsg->msg)-1,cmsg->msg);
}
