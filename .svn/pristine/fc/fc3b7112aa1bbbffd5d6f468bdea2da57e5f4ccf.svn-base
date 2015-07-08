static char sz__FILE__[]="@(#)fill_sh.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_sh.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:38:16
 * @(#)  Last delta: 12/2/94 at 11:32:30
 * @(#)  SCCS File: /taxi/sccs/s.fill_sh.c
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
#define KEY_SRC

#include <stdio.h>
#include <sys/types.h>
#include "taxipak.h"
#include "mads_types.h"
#include "Object.h"
#include "taxi_db.h"
#include "S_Sched_db.h"


void
fill_sh_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct cisam_sh *sh;

	sh = (struct cisam_sh*)req_buf;
  
	stlong(sh->key, &buf[SH_KEY]);
	stlong(sh->schedule_num, &buf[SH_SCHEDULE_NUM]);
	stint(sh->month, &buf[SH_MONTH]);
 	stchar(&sh->fleet, &buf[SH_FLEET], sizeof(sh->fleet));
}

void
fill_sh(req_buf, buf,action)
     char *req_buf;
     char *buf;
     int  action;
{
	struct cisam_sh *sh;

	sh = (struct cisam_sh*)req_buf;

	if(action == ADD_REC)
	  {
	    long tmp_long;
	    isuniqueid(Db_files[S_SCHED_FILE_ID].fd,&tmp_long);
	    sh->key = (int)tmp_long;
	  }

	fill_sh_key(req_buf, buf);
}

void
unfill_sh(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct cisam_sh *sh;

	sh = (struct cisam_sh*)req_buf;

	sh->key = ldlong(&buf[SH_KEY]);
	sh->schedule_num = ldlong(&buf[SH_SCHEDULE_NUM]);
	sh->month = ldint(&buf[SH_MONTH]);
	sh->fleet = buf[SH_FLEET];
}
