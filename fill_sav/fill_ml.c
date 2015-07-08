static char sz__FILE__[]="@(#)fill_ml.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_ml.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:38:01
 * @(#)  Last delta: 12/2/94 at 11:22:01
 * @(#)  SCCS File: /taxi/sccs/s.fill_ml.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:fill_ml.c	10.1.0.0"

#define KEY_SRC
#include "Message_log_db.h"
#include "taxi_db.h"

void
fill_ml_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_ml  *ml;
	register int  offset = 0;

	ml = (struct cisam_ml*)req_buf;

        stlong(ml->nbr,&buf[offset]);
        offset = sizeof(ml->nbr) + sizeof(ml->from);

        stlong(ml->date_time_sent,&buf[offset]);

}

void
fill_ml(req_buf, buf,action)
     char *req_buf;
     char *buf;
     int    action;
{
	struct  cisam_ml  *ml;
	register int  offset = 0;
	int offset2 = 0;
	long serial_nbr = 0;

	bzero(buf, sizeof(struct cisam_ml));
	ml = (struct cisam_ml*)req_buf;

	if (action == ADD_REC)
	  {
	    isuniqueid(Db_files[MSGLOG_FILE_ID].fd,&serial_nbr);
	    ml->nbr = (long)serial_nbr;
	  }

        stlong(ml->nbr,&buf[offset]);
        offset += LONGSIZE;

        stlong(ml->from,&buf[offset]);
        offset += LONGSIZE;

        stlong(ml->date_time_sent,&buf[offset]);
	offset += LONGSIZE;

        stint(ml->supervisor_id,&buf[offset]);
        offset += INTSIZE;


	stchar(ml->date_sent,&buf[offset],sizeof(ml->date_sent)-1);
	offset += sizeof(ml->date_sent) -1;

	stchar(ml->time_sent,&buf[offset],sizeof(ml->time_sent)-1);
	offset += sizeof(ml->time_sent) -1;

	stchar(&ml->supervisor_saw,&buf[offset],sizeof(ml->supervisor_saw));
	offset += sizeof(ml->supervisor_saw);

	stchar(&ml->repeat,&buf[offset],sizeof(ml->repeat));
	offset += sizeof(ml->repeat);

	stchar(&ml->fleet,&buf[offset],sizeof(ml->fleet));
	offset += sizeof(ml->fleet);

	stchar(&ml->to_type,&buf[offset],sizeof(ml->to_type));
	offset += sizeof(ml->to_type);
	stchar(ml->to_selection,&buf[offset],sizeof(ml->to_selection)-1);
	offset += sizeof(ml->to_selection) -1;

	stchar(ml->msg_line1,&buf[offset],sizeof(ml->msg_line1)-1);
	offset += sizeof(ml->msg_line1) -1;

	stchar(ml->msg_line2,&buf[offset],sizeof(ml->msg_line2)-1);
	offset += sizeof(ml->msg_line2) - 1;

	stchar(&ml->veh_attr_set, &buf[offset], sizeof(ml->veh_attr_set));
	offset += sizeof(ml->veh_attr_set);

	stchar(ml->veh_attr, &buf[offset], sizeof(ml->veh_attr));
	offset += sizeof(ml->veh_attr);

	stchar(&ml->drv_attr_set, &buf[offset], sizeof(ml->drv_attr_set));
	offset += sizeof(ml->drv_attr_set);

	stchar(ml->drv_attr, &buf[offset], sizeof(ml->drv_attr));
	offset += sizeof(ml->drv_attr);

	stchar(ml->msgtext, &buf[offset], sizeof(ml->msgtext));
	if ( strstr(&buf[offset], "%.L") )
	  {
	    offset2 = (char *)strstr(&buf[offset], "%.L") - (char *)(&buf[offset]) + 10;
	    buf[offset + offset2] = '\0';
	  }

	if ( strstr(&buf[offset], "==REITTI LOPPUUN SUORITETTU==") )
	  {
	    offset2 = (char *)strstr(&buf[offset], "==REITTI LOPPUUN SUORITETTU==") - (char *)(&buf[offset]) + 28;
	    buf[offset + offset2] = '\0';
	  }

	if ( strstr(&buf[offset], "                                ") )
	  {
	    offset2 = (char *)strstr(&buf[offset], "                                ") - (char *)(&buf[offset]);
	    buf[offset + offset2] = '\0';
	  }
	
	offset += sizeof(ml->msgtext);


}

/* Load program variables from C-ISAM data record */
void
unfill_ml(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_ml  *ml;
	register int  offset = 0;

	bzero(req_buf, sizeof(BUFSIZ));
	ml = (struct cisam_ml*)req_buf;

        ml->nbr = ldlong(&buf[offset]);
        offset += LONGSIZE;

        ml->from = ldlong(&buf[offset]);
        offset += LONGSIZE;

        ml->date_time_sent = ldlong(&buf[offset]);
	offset += LONGSIZE;

        ml->supervisor_id = ldint(&buf[offset]);
        offset += INTSIZE;

	ldchar(&buf[offset],sizeof(ml->date_sent)-1,ml->date_sent);
	offset += sizeof(ml->date_sent) -1;

	ldchar(&buf[offset],sizeof(ml->time_sent)-1,ml->time_sent);
	offset += sizeof(ml->time_sent) -1;

	ml->supervisor_saw = buf[offset];
	offset += sizeof(ml->supervisor_saw);

	ml->repeat = buf[offset];
	offset += sizeof(ml->repeat);

	ml->fleet = buf[offset];
	offset += sizeof(ml->fleet);

	ml->to_type = buf[offset];
	offset += sizeof(ml->to_type);

	ldchar(&buf[offset],sizeof(ml->to_selection)-1,ml->to_selection);
	offset += sizeof(ml->to_selection) -1;

	ldchar(&buf[offset],sizeof(ml->msg_line1)-1,ml->msg_line1);
	offset += sizeof(ml->msg_line1) -1;

	ldchar(&buf[offset],sizeof(ml->msg_line2)-1,ml->msg_line2);
	offset += sizeof(ml->msg_line2) - 1;

	ml->veh_attr_set = buf[offset];
	offset += sizeof(ml->veh_attr_set);

	ldchar(&buf[offset], sizeof(ml->veh_attr), ml->veh_attr);
	offset += sizeof(ml->veh_attr);

	ml->drv_attr_set = buf[offset];
	offset += sizeof(ml->drv_attr_set);

	ldchar(&buf[offset], sizeof(ml->drv_attr), ml->drv_attr);
	offset += sizeof(ml->drv_attr);

	ldchar(&buf[offset], sizeof(ml->msgtext), ml->msgtext);
	offset += sizeof(ml->msgtext);			
}
