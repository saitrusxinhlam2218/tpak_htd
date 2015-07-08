/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_vh_cmsg.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1999/02/01 12:46:08 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_vh_cmsg.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: fill_vh_cmsg.c,v 1.2 1999/02/01 12:46:08 taxiadm Exp $";

#define KEY_SRC

#include "Veh_canmsg_db.h"

void
fill_veh_cm_key( req_buf, buf )
     char   *req_buf;
     char   *buf;
{
  struct cisam_veh_cm *cm;
  register int  offset = 0;
  
  cm = (struct cisam_veh_cm*)req_buf;
  
  stlong(cm->nbr,&buf[offset]);
}  

void
fill_veh_cm(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int     action;
{
	struct  cisam_veh_cm  *cm;
	register int  offset = 0;

	cm = (struct cisam_veh_cm*)req_buf;

        stlong(cm->nbr,&buf[offset]);
        offset += LONGSIZE;

        stchar(cm->text,&buf[offset],sizeof(cm->text)-1);
}

/* Load program variables from C-ISAM data record */
void
unfill_veh_cm( req_buf, buf )
     char *req_buf;
     char *buf;
{
	struct  cisam_veh_cm  *cm;
	register int  offset = 0;

	cm = (struct cisam_veh_cm*)req_buf;

        cm->nbr = ldlong(&buf[offset]);
        offset += LONGSIZE;

	ldchar(&buf[offset],sizeof(cm->text)-1,cm->text);
}
