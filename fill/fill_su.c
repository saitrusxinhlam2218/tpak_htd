static char sz__FILE__[]="@(#)fill_su.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_su.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:38:26
 * @(#)  Last delta: 12/2/94 at 11:32:41
 * @(#)  SCCS File: /taxi/sccs/s.fill_su.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC
#include "Stats_usr_db.h"
#include "taxi_db.h"

void
fill_su_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_su  *su;
	register int  offset = 0;

	su = (struct cisam_su*)req_buf;

	stint(su->record_nbr,&buf[offset]);
	offset += INTSIZE;

	stchar(&su->fleet,&buf[offset],sizeof(su->fleet)); /* fleet indicator */
        offset = sizeof(su->fleet) + sizeof(su->user_type);

	stint(su->user_id,&buf[offset]);  /* User ID */
}

void
fill_su(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int  action;
{
	struct  cisam_su  *su;
	register int  offset = 0;
	int serial_nbr = 0;

	su = (struct cisam_su*)req_buf;

	if (action == ADD_REC)
	  {
	    isuniqueid(Db_files[STATUSR_FILE_ID].fd,&serial_nbr);
	    su->record_nbr = (int)serial_nbr;
	  }
	
	stlong(su->record_nbr,&buf[offset]);
	offset += LONGSIZE;

	stchar(&su->fleet,&buf[offset],sizeof(su->fleet));
	offset += sizeof(su->fleet);

	stchar(&su->user_type,&buf[offset],sizeof(su->user_type));
	offset += sizeof(su->user_type);

	stint(su->user_id,&buf[offset]);
	offset += INTSIZE;

	stlong(su->from_dt_tm,&buf[offset]);
	offset += LONGSIZE;

	stlong(su->to_dt_tm,&buf[offset]);
	offset += LONGSIZE;

	stchar(su->from_date,&buf[offset],sizeof(su->from_date)-1);
	offset += sizeof(su->from_date)-1;

	stchar(su->from_time,&buf[offset],sizeof(su->from_time)-1);
	offset += sizeof(su->from_time)-1;

	stchar(su->to_date,&buf[offset],sizeof(su->to_date)-1);
	offset += sizeof(su->to_date)-1;

	stchar(su->to_time,&buf[offset],sizeof(su->to_time)-1);
	offset += sizeof(su->to_time)-1;

	stint(su->pass_calls,&buf[offset]);
	offset += INTSIZE;

	stint(su->del_calls,&buf[offset]);
	offset += INTSIZE;

	stint(su->typex_calls,&buf[offset]);
	offset += INTSIZE;

	stint(su->wkup_calls,&buf[offset]);
	offset += INTSIZE;

	stint(su->killed,&buf[offset]);
	offset += INTSIZE;

	stint(su->callbacks,&buf[offset]);
	offset += INTSIZE;

	stint(su->unzoned,&buf[offset]);
	offset += INTSIZE;

	stint(su->cancelled,&buf[offset]);
	offset += INTSIZE;

	stint(su->messages,&buf[offset]);
	offset += INTSIZE;

	stint(su->man_assigns,&buf[offset]);
	offset += INTSIZE;

	stint(su->overrides,&buf[offset]);
	offset += INTSIZE;

	stint(su->exceptions,&buf[offset]);
}
/* Load program variables from C-ISAM data record */

void
unfill_su( req_buf, buf )
     char *req_buf;
     char *buf;
{
	struct  cisam_su  *su;
	register int  offset = 0;

	su = (struct cisam_su*)req_buf;

	su->record_nbr = ldlong(&buf[offset]);
	offset += LONGSIZE;

	su->fleet = buf[offset];
	offset += sizeof(su->fleet);

	su->user_type= buf[offset];
	offset += sizeof(su->user_type);

	su->user_id = ldint(&buf[offset]);
	offset += INTSIZE;

	su->from_dt_tm = ldint(&buf[offset]);
	offset += INTSIZE;

	su->to_dt_tm= ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(su->from_date)-1,su->from_date);
	offset += sizeof(su->from_date)-1;

	ldchar(&buf[offset],sizeof(su->from_time)-1,su->from_time);
	offset += sizeof(su->from_time)-1;

	ldchar(&buf[offset],sizeof(su->to_date)-1,su->to_date);
	offset += sizeof(su->to_date)-1;

	ldchar(&buf[offset],sizeof(su->to_time)-1,su->to_time);
	offset += sizeof(su->to_time)-1;

	su->pass_calls = ldint(&buf[offset]);
	offset += INTSIZE;

	su->del_calls = ldint(&buf[offset]);
	offset += INTSIZE;

	su->typex_calls = ldint(&buf[offset]);
	offset += INTSIZE;

	su->wkup_calls= ldint(&buf[offset]);
	offset += INTSIZE;

	su->killed= ldint(&buf[offset]);
	offset += INTSIZE;

	su->callbacks= ldint(&buf[offset]);
	offset += INTSIZE;

	su->unzoned= ldint(&buf[offset]);
	offset += INTSIZE;

	su->cancelled= ldint(&buf[offset]);
	offset += INTSIZE;

	su->messages= ldint(&buf[offset]);
	offset += INTSIZE;

	su->man_assigns= ldint(&buf[offset]);
	offset += INTSIZE;

	su->overrides= ldint(&buf[offset]);
	offset += INTSIZE;

	su->exceptions= ldint(&buf[offset]);
}
