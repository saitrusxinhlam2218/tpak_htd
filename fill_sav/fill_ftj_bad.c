/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_ftj_bad.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2001/03/12 16:40:23 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_ftj_bad.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: fill_ftj_bad.c,v $	$Revision: 1.2 $"

static char rcsid[] = "$Id: fill_ftj_bad.c,v 1.2 2001/03/12 16:40:23 taxiadm Exp $";

#define KEY_SRC
#include "taxipak.h"
#include "Object.h"
#include "FTJ_bad_db.h"

void fill_ftj_bad_key( req_buf, buf )
     char *req_buf;
     char *buf;
{
    FTJ_BAD_STRUCT *ftj_bad;
    register int offset = 0;

    ftj_bad = (FTJ_BAD_STRUCT *)req_buf;
    stchar(ftj_bad->ftj_number, &buf[offset], sizeof(ftj_bad->ftj_number)-1);
}

void fill_ftj_bad( req_buf, buf, action )
     char   *req_buf;
    char   *buf;
    int    action;
{
    FTJ_BAD_STRUCT  *ftj_bad;
    register int offset = 0;
    int  serial_nbr = 0;

    ftj_bad = (FTJ_BAD_STRUCT *)req_buf;
    stchar(ftj_bad->ftj_number, &buf[offset], sizeof(ftj_bad->ftj_number)-1);
    offset += sizeof(ftj_bad->ftj_number)-1;

    stchar(ftj_bad->ftj_start_date, &buf[offset], sizeof(ftj_bad->ftj_start_date)-1);
    offset += sizeof(ftj_bad->ftj_start_date)-1;

    stchar(ftj_bad->ftj_added_date, &buf[offset], sizeof(ftj_bad->ftj_added_date)-1);
    offset += sizeof(ftj_bad->ftj_added_date)-1;
}

void
unfill_ftj_bad( req_buf, buf )
    char     *req_buf;
    char     *buf;
{
    FTJ_BAD_STRUCT *ftj_bad;
    register int offset = 0;

    ftj_bad = (FTJ_BAD_STRUCT *)req_buf;

    ldchar(&buf[offset], sizeof(ftj_bad->ftj_number)-1, ftj_bad->ftj_number);
    offset += sizeof(ftj_bad->ftj_number)-1;
    
    ldchar(&buf[offset], sizeof(ftj_bad->ftj_start_date)-1, ftj_bad->ftj_start_date);
    offset += sizeof(ftj_bad->ftj_start_date)-1;
    
    ldchar(&buf[offset], sizeof(ftj_bad->ftj_added_date)-1, ftj_bad->ftj_added_date);
    offset += sizeof(ftj_bad->ftj_added_date)-1;    
}    

