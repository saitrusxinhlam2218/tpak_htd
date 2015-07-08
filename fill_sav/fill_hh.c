static char sz__FILE__[]="@(#)fill_hh.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_hh.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:37:46
 * @(#)  Last delta: 12/2/94 at 11:31:57
 * @(#)  SCCS File: /taxi/sccs/s.fill_hh.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:fill_hh.c	10.1.0.0"
#ident "@(#) head:fill_hh.c	4.1.0.0"


#include "Call_hist_db.h"
#include "taxi_db.h"

#define CONST	12

void
fill_hh_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  struct cisam_ch *ch;
  register int  offset = 0;

  ch = (struct cisam_ch*)req_buf;

  stlong(ch->record_nbr,&buf[offset]);
  offset += LONGSIZE;
  
  stlong(ch->call_nbr,&buf[offset]);
  offset += LONGSIZE;
 
  stint(ch->exception_nbr,&buf[offset]);
  offset = CONST;

stlong(ch->status_dt_tm,&buf[offset]);
}

void
fill_hh(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int  action;
{
int  cc;
  struct cisam_ch *ch;
  register int  offset = 0;

  ch = (struct cisam_ch*)req_buf;

stlong(ch->record_nbr,&buf[offset]);
offset += LONGSIZE;

stlong(ch->call_nbr,&buf[offset]);
offset += LONGSIZE;

stint(ch->exception_nbr,&buf[offset]);
offset += INTSIZE;

stchar(&ch->fleet,&buf[offset],sizeof(ch->fleet));
offset += sizeof(ch->fleet); 

stchar(&ch->event_type,&buf[offset],sizeof(ch->event_type));
offset += sizeof(ch->event_type); 

stlong(ch->status_dt_tm,&buf[offset]);
offset += LONGSIZE;

stchar(ch->status_date,&buf[offset],(sizeof(ch->status_date))-1);
offset += sizeof(ch->status_date)-1; 

stchar(ch->status_time,&buf[offset],(sizeof(ch->status_time))-1);
offset += sizeof(ch->status_time)-1; 

stchar(ch->event_desc,&buf[offset],(sizeof(ch->event_desc))-1);
offset += sizeof(ch->event_desc)-1; 

stint(ch->veh_nbr,&buf[offset]);
offset += INTSIZE;

stlong(ch->drv_id,&buf[offset]);
offset += LONGSIZE;

stint(ch->user_id,&buf[offset]);
offset += INTSIZE;

stchar(ch->action_taken,&buf[offset],(sizeof(ch->action_taken))-1);
offset += sizeof(ch->action_taken)-1;
 
stchar(ch->action_date,&buf[offset],(sizeof(ch->action_date))-1);
offset += sizeof(ch->action_date)-1;

stchar(ch->action_time,&buf[offset],(sizeof(ch->action_time))-1);
offset += sizeof(ch->action_time)-1;

stlong(ch->action_dt_tm,&buf[offset]);
offset += LONGSIZE;

stint(ch->action_veh_nbr,&buf[offset]);
offset += INTSIZE;

stlong(ch->action_drv_id,&buf[offset]);
offset += LONGSIZE;

stint(ch->action_user_id,&buf[offset]);
}


void
unfill_hh( req_buf, buf)
     char *req_buf;
     char *buf;
{
  struct cisam_ch *ch;
  register int  offset = 0;

  ch = (struct cisam_ch*)req_buf;

ch->record_nbr = ldlong(&buf[offset]);
offset += LONGSIZE;

ch->call_nbr = ldlong(&buf[offset]);
offset += LONGSIZE;

ch->exception_nbr = ldint(&buf[offset]);
offset += INTSIZE;

ch->fleet = buf[offset];
offset += sizeof(ch->fleet); 

ch->event_type = buf[offset];
offset += sizeof(ch->event_type); 

ch->status_dt_tm = ldlong(&buf[offset]);
offset += LONGSIZE;

ldchar(&buf[offset],sizeof(ch->status_date)-1,ch->status_date);
offset += sizeof(ch->status_date)-1; 

ldchar(&buf[offset],sizeof(ch->status_time)-1,ch->status_time);
offset += sizeof(ch->status_time)-1; 

ldchar(&buf[offset],sizeof(ch->event_desc)-1,ch->event_desc);
offset += sizeof(ch->event_desc)-1; 

ch->veh_nbr = ldint(&buf[offset]);
offset += INTSIZE;

ch->drv_id = ldlong(&buf[offset]);
offset += LONGSIZE;

ch->user_id = ldint(&buf[offset]);
offset += INTSIZE;

ldchar(&buf[offset],sizeof(ch->action_taken)-1,ch->action_taken);
offset += sizeof(ch->action_taken)-1;
 
ldchar(&buf[offset],sizeof(ch->action_date)-1,ch->action_date);
offset += sizeof(ch->action_date)-1;

ldchar(&buf[offset],sizeof(ch->action_time)-1,ch->action_time);
offset += sizeof(ch->action_time)-1;

ch->action_dt_tm = ldlong(&buf[offset]);
offset += LONGSIZE;

ch->action_veh_nbr = ldint(&buf[offset]);
offset += INTSIZE;

ch->action_drv_id = ldlong(&buf[offset]);
offset += LONGSIZE;
 
ch->action_user_id = ldint(&buf[offset]);
}
