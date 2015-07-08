static char sz__FILE__[]="@(#)fill_sa.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_sa.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:38:09
 * @(#)  Last delta: 12/2/94 at 11:32:23
 * @(#)  SCCS File: /taxi/sccs/s.fill_sa.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:fill_sa.c	10.1.0.0"

#define KEY_SRC
#include "Stats_db.h"

void
fill_sa_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_sa  *sa;
	register int  offset = 0;

	sa = (struct cisam_sa*)req_buf;

	stchar(&sa->fleet,&buf[offset],sizeof(sa->fleet));      /* fleet indicator */
        offset += sizeof(sa->fleet);
	stchar(sa->date,&buf[offset],sizeof(sa->date)-1);
        
}

void
fill_sa(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int    action;
{
	struct  cisam_sa  *sa;
	register int  offset = 0;

	sa = (struct cisam_sa*)req_buf;

	stchar(&sa->fleet,&buf[offset],sizeof(sa->fleet));
	offset += sizeof(sa->fleet);

	stchar(sa->date,&buf[offset],sizeof(sa->date)-1);
	offset += sizeof(sa->date) -1;

	stlong(sa->sort_date,&buf[offset]);
	offset += LONGSIZE;

	stint(sa->day_start,&buf[offset]);
	offset += INTSIZE;

	stint(sa->day_end,&buf[offset]);
	offset += INTSIZE;

	stint(sa->u_pass_calls,&buf[offset]);
	offset += INTSIZE;

	stint(sa->u_del_calls,&buf[offset]);
	offset += INTSIZE;

	stint(sa->u_typex_calls,&buf[offset]);
	offset += INTSIZE;

	stint(sa->u_wakeup_calls,&buf[offset]);
	offset += INTSIZE;

	stint(sa->u_killed_calls,&buf[offset]);
	offset += INTSIZE;

	stfloat(sa->u_call_rate,&buf[offset]);
	offset += FLOATSIZE;

	stint(sa->u_callbacks,&buf[offset]);
	offset += INTSIZE;

	stint(sa->u_unzoned,&buf[offset]);
	offset += INTSIZE;

	stint(sa->u_cancelled,&buf[offset]);
	offset += INTSIZE;

	stint(sa->u_messages,&buf[offset]);
	offset += INTSIZE;

	stint(sa->u_assigns,&buf[offset]);
	offset += INTSIZE;

	stint(sa->u_overrides,&buf[offset]);
	offset += INTSIZE;

	stlong(sa->u_exceptions,&buf[offset]);
	offset += LONGSIZE;

	stfloat(sa->u_hours_worked,&buf[offset]);
	offset += FLOATSIZE;

	stint(sa->d_calls,&buf[offset]);
	offset += INTSIZE;

	stlong(sa->d_posts,&buf[offset]);
	offset += LONGSIZE;

	stlong(sa->d_cposts,&buf[offset]);
	offset += LONGSIZE;

	stint(sa->d_eposts,&buf[offset]);
	offset += INTSIZE;

	stint(sa->d_flags,&buf[offset]);
	offset += INTSIZE;

	stint(sa->d_bids,&buf[offset]);
	offset += INTSIZE;

	stint(sa->d_no_accepts,&buf[offset]);
	offset += INTSIZE;

	stint(sa->d_rejects,&buf[offset]);
	offset += INTSIZE;

	stint(sa->d_short_meters,&buf[offset]);
	offset += INTSIZE;

	stint(sa->d_late_meters,&buf[offset]);
	offset += INTSIZE;

	stint(sa->d_callbacks,&buf[offset]);
	offset += INTSIZE;

	stint(sa->d_callb_mon,&buf[offset]);
	offset += INTSIZE;

	stint(sa->d_callb_noshow,&buf[offset]);
	offset += INTSIZE;

	stint(sa->d_callouts,&buf[offset]);
	offset += INTSIZE;

	stint(sa->d_noshows,&buf[offset]);
	offset += INTSIZE;

	stint(sa->d_breaks,&buf[offset]);
	offset += INTSIZE;

	stint(sa->d_talks,&buf[offset]);
	offset += INTSIZE;

	stint(sa->d_messages,&buf[offset]);
	offset += INTSIZE;

	stint(sa->d_emergencies,&buf[offset]);
	offset += INTSIZE;

	stint(sa->d_sign_ins,&buf[offset]);
	offset += INTSIZE;

	stint(sa->d_sign_outs,&buf[offset]);
	offset += INTSIZE;

	stfloat(sa->d_hours_worked,&buf[offset]);
	offset += FLOATSIZE;

	stfloat(sa->d_avg_act_cabs,&buf[offset]);
}

/* Load program variables from C-ISAM data record */
void
unfill_sa(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_sa  *sa;
	register int  offset = 0;

	sa = (struct cisam_sa*)req_buf;

	sa->fleet = buf[offset];
	offset += FLOATSIZE;

	ldchar(&buf[offset],sizeof(sa->date)-1,sa->date);
	offset += sizeof(sa->date) -1;

	sa->sort_date = ldlong(&buf[offset]);
	offset += LONGSIZE;

	sa->day_start = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->day_end = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->u_pass_calls = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->u_del_calls = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->u_typex_calls = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->u_wakeup_calls = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->u_killed_calls = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->u_call_rate = ldfloat(&buf[offset]);
	offset += FLOATSIZE;

	sa->u_callbacks = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->u_unzoned = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->u_cancelled = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->u_messages = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->u_assigns = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->u_overrides = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->u_exceptions = ldlong(&buf[offset]);
	offset += LONGSIZE;

	sa->u_hours_worked = ldfloat(&buf[offset]);
	offset += FLOATSIZE;

	sa->d_calls = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_posts = ldlong(&buf[offset]);
	offset += LONGSIZE;

	sa->d_cposts = ldlong(&buf[offset]);
	offset += LONGSIZE;

	sa->d_eposts = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_flags = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_bids = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_no_accepts = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_rejects = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_short_meters = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_late_meters = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_callbacks = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_callb_mon = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_callb_noshow = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_callouts = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_noshows = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_breaks = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_talks = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_messages = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_emergencies = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_sign_ins = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_sign_outs = ldint(&buf[offset]);
	offset += INTSIZE;

	sa->d_hours_worked = ldfloat(&buf[offset]);
	offset += FLOATSIZE;

	sa->d_avg_act_cabs = ldfloat(&buf[offset]);
}

