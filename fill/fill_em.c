static char sz__FILE__[]="@(#)fill_em.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_em.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:37:39
 * @(#)  Last delta: 12/2/94 at 11:21:41
 * @(#)  SCCS File: /taxi/sccs/s.fill_em.c
 *                                                                           *
 *****************************************************************************/


#define KEY_SRC

#include "Emergency_log_db.h"
#include "taxi_db.h"

#define   CONST   14 

void
fill_em_key( req_buf, buf )
     char *req_buf;
     char *buf;
{
	struct  cisam_em  *em;
	register int  offset = 0;

	em = (struct cisam_em*)req_buf;

	stchar(&em->fleet,&buf[offset],sizeof(em->fleet));
	offset = 2;

	stlong(em->nbr,&buf[offset]);
        offset = CONST;

	stlong(em->date_time,&buf[offset]);
}

void
fill_em(req_buf, buf,action)
     char *req_buf;
     char *buf;
     int  action;
{
	struct  cisam_em  *em;
	register int  offset = 0;
	int serial_nbr;

	em = (struct cisam_em*)req_buf;

	if (action == ADD_REC)
	  {
	    isuniqueid(Db_files[EMERG_FILE_ID].fd,&serial_nbr);
	    em->nbr = (int)serial_nbr;
	  }
	stchar(&em->fleet,&buf[offset],sizeof(em->fleet));
	offset += sizeof(em->fleet);

	stchar(&em->pad,&buf[offset],sizeof(em->pad));
	offset += sizeof(em->pad);

	stlong(em->nbr,&buf[offset]);
        offset += LONGSIZE;

	stlong(em->exc_nbr,&buf[offset]);
        offset += LONGSIZE;

	stlong(em->call_nbr,&buf[offset]);
        offset += LONGSIZE;

	stlong(em->date_time,&buf[offset]);
        offset += LONGSIZE;

	stchar(em->event_date,&buf[offset],sizeof(em->event_date)-1);
        offset += sizeof(em->event_date)-1;

	stchar(em->event_time,&buf[offset],sizeof(em->event_time)-1);
        offset += sizeof(em->event_time)-1;

	stchar(em->resolved_date,&buf[offset],sizeof(em->resolved_date)-1);
        offset += sizeof(em->resolved_date)-1;

	stchar(em->resolved_time,&buf[offset],sizeof(em->resolved_time)-1);
        offset += sizeof(em->resolved_time)-1;

	stint(em->veh_nbr,&buf[offset]);
        offset += INTSIZE;

	stlong(em->drv_id,&buf[offset]);
        offset += LONGSIZE;

	stint(em->zone_nbr,&buf[offset]);
        offset += INTSIZE;

	stlong(em->resolved_dt_tm,&buf[offset]);
        offset += LONGSIZE;

	stint(em->resolved_by,&buf[offset]);
        offset += INTSIZE;

	stchar(em->comment, &buf[offset], sizeof(em->comment) - 1);
}

/* Load program variables from C-ISAM data record */
void
unfill_em( req_buf, buf )
     char *req_buf;
     char *buf;
{
	struct  cisam_em  *em;
	register int  offset = 0;

	em = (struct cisam_em*)req_buf;


        em->fleet = buf[offset];
	offset += sizeof(em->fleet);

	em->pad = buf[offset];
	offset += sizeof(em->pad);

	em->nbr = ldlong(&buf[offset]);
        offset += LONGSIZE;

	em->exc_nbr = ldlong(&buf[offset]);
        offset += LONGSIZE;

	em->call_nbr = ldlong(&buf[offset]);
        offset += LONGSIZE;

	em->date_time = ldlong(&buf[offset]);
        offset += LONGSIZE;

	ldchar(&buf[offset],sizeof(em->event_date)-1,em->event_date);
        offset += sizeof(em->event_date)-1;

	ldchar(&buf[offset],sizeof(em->event_time)-1,em->event_time);
        offset += sizeof(em->event_time)-1;

	ldchar(&buf[offset],sizeof(em->resolved_date)-1,em->resolved_date);
        offset += sizeof(em->resolved_date)-1;

	ldchar(&buf[offset],sizeof(em->resolved_time)-1,em->resolved_time);
        offset += sizeof(em->resolved_time)-1;

	em->veh_nbr = ldint(&buf[offset]);
        offset += INTSIZE;

	em->drv_id = ldlong(&buf[offset]);
        offset += LONGSIZE;

	em->zone_nbr = ldint(&buf[offset]);
        offset += INTSIZE;

	em->resolved_dt_tm = ldlong(&buf[offset]);
        offset += LONGSIZE;

	em->resolved_by = ldint(&buf[offset]);
        offset += INTSIZE;

	ldchar(&buf[offset], sizeof(em->comment) - 1, em->comment);

}
