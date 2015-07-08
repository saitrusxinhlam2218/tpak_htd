static char sz__FILE__[]="@(#)fill_ss.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_ss.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:38:22
 * @(#)  Last delta: 12/2/94 at 11:32:36
 * @(#)  SCCS File: /taxi/sccs/s.fill_ss.c
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:fill_ss.c	10.1.0.0"

#define KEY_SRC
#include <stdio.h>
#include <sys/types.h>
#include "taxipak.h"
#include "mads_types.h"
#include "Object.h"
#include "taxi_db.h"
#include "S_Shift_db.h"

void
fill_s_version_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  struct cisam_s_version *sv;
  sv = (struct cisam_s_version *)req_buf;

  stlong(sv->vers_id, &buf[0]);
}

void fill_s_version(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int action;
{
  struct cisam_s_version *sv;
  register int offset = 0;

  sv = (struct cisam_s_version *)req_buf;

  stlong(sv->vers_id, &buf[offset]);
  offset += LONGSIZE;

  stchar(&sv->active,&buf[offset],sizeof(sv->active));
  offset += sizeof(sv->active);

  stchar(sv->desc, &buf[offset], sizeof(sv->desc)-1);
  offset += sizeof(sv->desc)-1;
}

void
unfill_s_version( req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct cisam_s_version *sv;
	register int  offset = 0;

	sv = (struct cisam_s_version*)req_buf;

	sv->vers_id = ldlong(&buf[offset]);
	offset += LONGSIZE;

	sv->active = buf[offset];
	offset += sizeof(sv->active);
	
	ldchar(&buf[offset], sizeof(sv->desc)-1, sv->desc);
	offset += sizeof(sv->desc)-1;
}
  
void
fill_ss_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  
	struct cisam_ss *ss;

	ss = (struct cisam_ss*)req_buf;
  
	stlong(ss->schedule_key, &buf[SS_SCHEDULE_KEY]);
	stlong(ss->day, &buf[SS_DAY]);
	stlong(ss->vers_id, &buf[SS_VERSION]);
}

void
fill_ss(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int  action;
{
	struct cisam_ss *ss;
	register int  offset = 0;

	ss = (struct cisam_ss*)req_buf;

	stlong(ss->schedule_key, &buf[offset]);
	offset += LONGSIZE;

	stlong(ss->day, &buf[offset]);
	offset += LONGSIZE;

	stlong(ss->start_1, &buf[offset]);
	offset += LONGSIZE;

	stlong(ss->end_1, &buf[offset]);
	offset += LONGSIZE;

	stlong(ss->start_2, &buf[offset]);
	offset += LONGSIZE;

	stlong(ss->end_2, &buf[offset]);
	offset += LONGSIZE;

	stlong(ss->start_3, &buf[offset]);
	offset += LONGSIZE;

	stlong(ss->end_3, &buf[offset]);
	offset += LONGSIZE;

	stlong(ss->vers_id, &buf[offset]);
	offset += LONGSIZE;
}

void
unfill_ss( req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct cisam_ss *ss;
	register int  offset = 0;

	ss = (struct cisam_ss*)req_buf;

	ss->schedule_key = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ss->day = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ss->start_1 = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ss->end_1 = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ss->start_2 = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ss->end_2 = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ss->start_3 = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ss->end_3 = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ss->vers_id = ldlong(&buf[offset]);
	offset += LONGSIZE;
};
