static char sz__FILE__[]="@(#)fill_rm.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_rm.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:38:05
 * @(#)  Last delta: 12/2/94 at 12:00:34
 * @(#)  SCCS File: /taxi/sccs/s.fill_rm.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC
#include "Rptmsg_log_db.h"
#include "taxi_db.h"

void
fill_rm_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_rm  *rm;
	register int  offset = 0;

	rm = (struct cisam_rm*)req_buf;

        stlong(rm->msg_nbr,&buf[offset]);
        offset += LONGSIZE;

        stint(rm->created_by,&buf[offset]);
}

void
fill_rm(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int	action;
{
	struct cisam_rm	*rm;
	register int	offset = 0;
        int serial_nbr = 0;
        
	rm = (struct cisam_rm*)req_buf;

	if(action == ADD_REC)
          {
            isuniqueid(Db_files[RPTMSG_FILE_ID].fd, &serial_nbr);
            rm->msg_nbr = (int)serial_nbr;
          }

        stlong(rm->msg_nbr, &buf[offset]);
        offset += LONGSIZE;

        stint(rm->created_by,&buf[offset]);
        offset += INTSIZE;

        stlong(rm->created_dt_tm,&buf[offset]);
	offset += LONGSIZE;

	stchar(&rm->repeat,&buf[offset],sizeof(rm->repeat));
	offset += sizeof(rm->repeat);

	stchar(&rm->fleet,&buf[offset],sizeof(rm->fleet));
	offset += sizeof(rm->fleet);

	stchar(&rm->to_type,&buf[offset],sizeof(rm->to_type));
	offset += sizeof(rm->to_type);

	stchar(rm->to_selection, &buf[offset], sizeof(rm->to_selection) - 1);
	offset += sizeof(rm->to_selection) - 1;

        stint(rm->how_often,&buf[offset]);
	offset += INTSIZE;

        stlong(rm->until_dt_tm,&buf[offset]);
	offset += LONGSIZE;

	stchar(rm->line_1, &buf[offset], sizeof(rm->line_1) - 1);
	offset += sizeof(rm->line_1) - 1;

	stchar(rm->line_2, &buf[offset], sizeof(rm->line_2) - 1);
	offset += sizeof(rm->line_2) - 1;

	stchar(rm->cr_date, &buf[offset], sizeof(rm->cr_date) - 1);
	offset += sizeof(rm->cr_date) - 1;

	stchar(rm->cr_time, &buf[offset], sizeof(rm->cr_time) - 1);
	offset += sizeof(rm->cr_time) - 1;

	stchar(rm->until_date, &buf[offset], sizeof(rm->until_date) - 1);
	offset += sizeof(rm->until_date) - 1;

	stchar(rm->until_time, &buf[offset], sizeof(rm->until_time) - 1);
	offset += sizeof(rm->until_time) - 1;

	stchar(&rm->veh_attr_set, &buf[offset], sizeof(rm->veh_attr_set));
	offset += sizeof(rm->veh_attr_set);

	stchar(rm->veh_attr, &buf[offset], sizeof(rm->veh_attr) - 1);
	offset += sizeof(rm->veh_attr) - 1;

	stchar(&rm->drv_attr_set, &buf[offset], sizeof(rm->drv_attr_set));
	offset += sizeof(rm->drv_attr_set);

	stchar(rm->drv_attr, &buf[offset], sizeof(rm->drv_attr) - 1);
	offset += sizeof(rm->drv_attr) - 1;	
}

/* Load program variables from C-ISAM data record */
void
unfill_rm(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_rm  *rm;
	register int  offset = 0;

	rm = (struct cisam_rm*)req_buf;

        rm->msg_nbr = ldlong(&buf[offset]);
        offset += LONGSIZE;

        rm->created_by = ldint(&buf[offset]);
        offset += INTSIZE;

        rm->created_dt_tm = ldlong(&buf[offset]);
	offset += LONGSIZE;

	rm->repeat = buf[offset];
	offset += sizeof(rm->repeat);

	rm->fleet = buf[offset];
	offset += sizeof(rm->fleet);

	rm->to_type = buf[offset];
	offset += sizeof(rm->to_type);

	ldchar(&buf[offset],sizeof(rm->to_selection)-1,rm->to_selection);
	offset += sizeof(rm->to_selection) -1;

        rm->how_often = ldint(&buf[offset]);
	offset += INTSIZE;

        rm->until_dt_tm = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ldchar(&buf[offset],sizeof(rm->line_1)-1,rm->line_1);
	offset += sizeof(rm->line_1) -1;

	ldchar(&buf[offset],sizeof(rm->line_2)-1,rm->line_2);
	offset += sizeof(rm->line_2) - 1;

	ldchar(&buf[offset], sizeof(rm->cr_date) - 1, rm->cr_date);
	offset += sizeof(rm->cr_date) - 1;

	ldchar(&buf[offset], sizeof(rm->cr_time) - 1, rm->cr_time);
	offset += sizeof(rm->cr_time) - 1;

	ldchar(&buf[offset], sizeof(rm->until_date) - 1, rm->until_date);
	offset += sizeof(rm->until_date) - 1;

	ldchar(&buf[offset], sizeof(rm->until_time) - 1, rm->until_time);
	offset += sizeof(rm->until_time) - 1;

	rm->veh_attr_set = buf[offset];
	offset += sizeof(rm->veh_attr_set);

	ldchar(&buf[offset], sizeof(rm->veh_attr) - 1, rm->veh_attr);
	offset += sizeof(rm->veh_attr) - 1;

	rm->drv_attr_set = buf[offset];
	offset += sizeof(rm->drv_attr_set);

	ldchar(&buf[offset], sizeof(rm->drv_attr) - 1, rm->drv_attr);
	offset += sizeof(rm->drv_attr) - 1;	
	
}
