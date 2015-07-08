/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_hist_cmt.c,v $
*  @(#)  $Revision: 1.6 $
*  @(#)  $Date: 2003/01/28 16:30:05 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_hist_cmt.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

#define KEY_SRC
#include "Hist_cmt_private.h"
#include "taxi_db.h"

void fill_hc_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	register int  offset = 0;
	HIST_CMT_REC *histcmt_rec = (HIST_CMT_REC *)req_buf;

	stlong(histcmt_rec->comment_id,&buf[offset]);
	offset += LONGSIZE;

	stlong(histcmt_rec->call_nbr, &buf[8] );
	stlong(histcmt_rec->dt_tm, &buf[42] );
}


void
fill_hc(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int action;
{
	register int  offset = 0;
	int i;
        int serial_nbr = 0;
	HIST_CMT_REC *histcmt_rec = (HIST_CMT_REC *)req_buf;;

	if (action == ADD_REC)
          {
            isuniqueid(Db_files[HIST_CMT_FILE_ID].fd, &serial_nbr);
            histcmt_rec->comment_id = (int)serial_nbr;
          }
	
	stlong(histcmt_rec->comment_id,&buf[offset]);
	offset += LONGSIZE;

	stlong(histcmt_rec->next_part,&buf[offset]);
	offset += LONGSIZE;

	stlong(histcmt_rec->call_nbr,&buf[offset]);
	offset += LONGSIZE;

	stchar(histcmt_rec->text, &buf[offset], sizeof(histcmt_rec->text)-1);
	offset += sizeof(histcmt_rec->text)-1;

	stlong(histcmt_rec->dt_tm, &buf[offset]);
	offset += LONGSIZE;
}

/* Load program variables from C-ISAM data record */

void unfill_hc(req_buf, buf)
     char *req_buf;
     char *buf;
{
	int i;
	register int  offset = 0;
	HIST_CMT_REC *histcmt_rec = (HIST_CMT_REC *)req_buf;

	histcmt_rec->comment_id = ldlong(&buf[offset]);
	offset += LONGSIZE;

	histcmt_rec->next_part = ldlong(&buf[offset]);
	offset += LONGSIZE;

	histcmt_rec->call_nbr = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ldchar(&buf[offset],sizeof(histcmt_rec->text)-1, histcmt_rec->text);
        offset += sizeof(histcmt_rec->text)-1;

	histcmt_rec->dt_tm = ldlong(&buf[offset]);
	offset += LONGSIZE;
}

