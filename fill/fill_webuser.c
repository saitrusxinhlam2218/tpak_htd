/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_webuser.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2003/01/28 16:30:04 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_webuser.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

static char rcsid[] = "$Id: fill_webuser.c,v 1.3 2003/01/28 16:30:04 jwelch Exp $";

#define KEY_SRC

#include "TPakWeb_User.h"
#include "taxi_db.h"

void
fill_webuser_key( req_buf, buf )
    char  *req_buf;
    char  *buf;
{
    WEBUSER *webuser;

    webuser = (WEBUSER *)req_buf;

    stlong( webuser->nbr, &buf[0]);
    stchar( webuser->user_name, &buf[4], sizeof(webuser->user_name)-1);
}

void
fill_webuser( req_buf, buf, action )
    char   *req_buf;
    char   *buf;
    int    action;
{
    WEBUSER *webuser;
    register int offset = 0;
    long     serial_nbr = 0;
    
    webuser = (WEBUSER *)req_buf;
    
    if (action == ADD_REC)
	{
	    isuniqueid(Db_files[WEBUSER_FILE_ID].fd, &serial_nbr);
	    webuser->nbr = (int)serial_nbr;
	}

    stlong(webuser->nbr, &buf[offset]);
    offset += LONGSIZE;

    stchar(webuser->user_name, &buf[offset], sizeof(webuser->user_name)-1);
    offset += sizeof(webuser->user_name)-1;

    stchar(webuser->passwd, &buf[offset], sizeof(webuser->passwd)-1);
    offset += sizeof(webuser->passwd)-1;

    stchar(webuser->fullname, &buf[offset], sizeof(webuser->fullname)-1);
    offset += sizeof(webuser->fullname)-1;

    stchar(webuser->tele, &buf[offset], sizeof(webuser->tele)-1);
    offset += sizeof(webuser->tele)-1;

    stchar(webuser->email, &buf[offset], sizeof(webuser->email)-1);
    offset += sizeof(webuser->email)-1;

    stchar(webuser->service, &buf[offset], sizeof(webuser->service)-1);
    offset += sizeof(webuser->service)-1;

    stchar(webuser->cookie, &buf[offset], sizeof(webuser->cookie)-1);
    offset += sizeof(webuser->cookie)-1;
}

void
unfill_webuser( req_buf, buf )
    char     *req_buf;
    char     *buf;
{
    WEBUSER *webuser;
    register int offset = 0;

    webuser = (WEBUSER *)req_buf;

    webuser->nbr = ldlong(&buf[offset]);
    offset += LONGSIZE;

    ldchar(&buf[offset], sizeof(webuser->user_name)-1, webuser->user_name);
    offset += sizeof(webuser->user_name)-1;

    ldchar(&buf[offset], sizeof(webuser->passwd)-1, webuser->passwd);
    offset += sizeof(webuser->passwd)-1;

    ldchar(&buf[offset], sizeof(webuser->fullname)-1, webuser->fullname);
    offset += sizeof(webuser->fullname)-1;

    ldchar(&buf[offset], sizeof(webuser->tele)-1, webuser->tele);
    offset += sizeof(webuser->tele)-1;

    ldchar(&buf[offset], sizeof(webuser->email)-1, webuser->email);
    offset += sizeof(webuser->email)-1;

    ldchar(&buf[offset], sizeof(webuser->service)-1, webuser->service);
    offset += sizeof(webuser->service)-1;

    ldchar(&buf[offset], sizeof(webuser->cookie)-1, webuser->cookie);
    offset += sizeof(webuser->cookie)-1;
}

    
    



	    
