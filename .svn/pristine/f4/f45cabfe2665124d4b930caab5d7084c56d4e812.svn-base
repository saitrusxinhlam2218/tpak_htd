/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_attrans.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2002/03/21 14:52:04 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_attrans.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: fill_attrans.c,v $	$Revision: 1.2 $"

static char rcsid[] = "$Id: fill_attrans.c,v 1.2 2002/03/21 14:52:04 taxiadm Exp $";

#define KEY_SRC

#include <isam.h>
#include "Xfer_db.h"

fill_xfer_key( char *from, char *buf )
{
  XFER_DB_REC *xfer = (XFER_DB_REC *)from;
  register int offset = 0;

  stlong(xfer->cl_nbr, &buf[offset]);
  offset += LONGSIZE;
}

fill_xfer( req_buf, buf, action )
     char *req_buf;
     char *buf;
     int action;     
{
  
    XFER_DB_REC *xfer = (XFER_DB_REC *)req_buf;
    register int offset = 0;

    stlong(xfer->cl_nbr, &buf[offset]);
    offset += LONGSIZE;

    stlong(xfer->due_date_time, &buf[offset]);
    offset += LONGSIZE;
  }

unfill_xfer( req_buf, buf )
     char *req_buf;
     char *buf;
{
  XFER_DB_REC *xfer = (XFER_DB_REC *)req_buf;
  register int offset = 0;
  
  xfer->cl_nbr = ldlong(&buf[offset]);
  offset += LONGSIZE;
  
  xfer->due_date_time = ldlong(&buf[offset]);
  offset += LONGSIZE;
}

