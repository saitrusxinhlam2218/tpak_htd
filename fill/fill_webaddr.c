/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_webaddr.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2001/02/01 19:38:06 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_webaddr.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

static char rcsid[] = "$Id: fill_webaddr.c,v 1.2 2001/02/01 19:38:06 taxiadm Exp $";


#define KEY_SRC

#include "TPakWeb_UserAddress.h"
#include "taxi_db.h"

void
fill_webaddr_key( req_buf, buf )
    char  *req_buf;
    char  *buf;
{
    WEBADDR *webaddr;

    webaddr = (WEBADDR *)req_buf;

    stlong( webaddr->webuser_nbr, &buf[0]);
}

void fill_webaddr( req_buf, buf, action )
    char    *req_buf;
    char    *buf;
    int     action;
{
    WEBADDR *webaddr;
    register int offset = 0;
    int      serial_nbr = 0;

    webaddr = (WEBADDR *)req_buf;

    stlong(webaddr->webuser_nbr, &buf[offset]);
    offset += LONGSIZE;

    stchar(webaddr->str_pre_dir, &buf[offset], sizeof(webaddr->str_pre_dir)-1);
    offset += sizeof(webaddr->str_pre_dir)-1;

    stchar(webaddr->str_name, &buf[offset], sizeof(webaddr->str_name)-1);
    offset += sizeof(webaddr->str_name)-1;

    stchar(webaddr->str_type, &buf[offset], sizeof(webaddr->str_type)-1);
    offset += sizeof(webaddr->str_type)-1;

    stchar(webaddr->str_post_dir, &buf[offset], sizeof(webaddr->str_post_dir)-1);
    offset += sizeof(webaddr->str_post_dir)-1;

    stchar(webaddr->str_city, &buf[offset], sizeof(webaddr->str_city)-1);
    offset += sizeof(webaddr->str_city)-1;        

    stlong(webaddr->str_nbr, &buf[offset]);
    offset += LONGSIZE;

    stchar(webaddr->str_apartment, &buf[offset], sizeof(webaddr->str_apartment)-1);
    offset += sizeof(webaddr->str_apartment)-1;            

    stchar(webaddr->str_comment, &buf[offset], sizeof(webaddr->str_comment)-1);
    offset += sizeof(webaddr->str_comment)-1;

    stchar(webaddr->str_ct_comment, &buf[offset], sizeof(webaddr->str_ct_comment)-1);
    offset += sizeof(webaddr->str_ct_comment)-1;

    stchar(&webaddr->place_name, &buf[offset], sizeof(webaddr->place_name));
    offset += sizeof(webaddr->place_name);
    
    stlong(webaddr->place_index, &buf[offset]);
    offset += LONGSIZE;
    
}

void
unfill_webaddr( req_buf, buf )
    char   *req_buf;
    char   *buf;
{
    WEBADDR     *webaddr;
    register int offset = 0;

    webaddr = (WEBADDR *)req_buf;

    webaddr->webuser_nbr = ldlong(&buf[offset]);
    offset += LONGSIZE;

    ldchar(&buf[offset], sizeof(webaddr->str_pre_dir) -1, webaddr->str_pre_dir);
    offset += sizeof(webaddr->str_pre_dir)-1;

    ldchar(&buf[offset], sizeof(webaddr->str_name) -1, webaddr->str_name);
    offset += sizeof(webaddr->str_name)-1;

    ldchar(&buf[offset], sizeof(webaddr->str_type) -1, webaddr->str_type);
    offset += sizeof(webaddr->str_type)-1;

    ldchar(&buf[offset], sizeof(webaddr->str_post_dir) -1, webaddr->str_post_dir);
    offset += sizeof(webaddr->str_post_dir)-1;

    ldchar(&buf[offset], sizeof(webaddr->str_city) -1, webaddr->str_city);
    offset += sizeof(webaddr->str_city)-1;    

    webaddr->str_nbr = ldlong(&buf[offset]);
    offset += LONGSIZE;

    ldchar(&buf[offset], sizeof(webaddr->str_apartment) -1, webaddr->str_apartment);
    offset += sizeof(webaddr->str_apartment)-1;
    
    ldchar(&buf[offset], sizeof(webaddr->str_comment) -1, webaddr->str_comment);
    offset += sizeof(webaddr->str_comment)-1;

    ldchar(&buf[offset], sizeof(webaddr->str_ct_comment) -1, webaddr->str_ct_comment);
    offset += sizeof(webaddr->str_ct_comment)-1;

    webaddr->place_name = buf[offset];
    offset += sizeof(webaddr->place_name);

    webaddr->place_index = ldlong(&buf[offset]);
    offset += LONGSIZE;    
    
}

    
