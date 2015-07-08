/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: TPakWeb_User.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2001/02/01 19:39:54 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/TPakWeb_User.h,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ifndef _TPAKWEB_USER_H
#define _TPAKWEB_USER_H
#define  USER_NAME_LEN    16
#define  USER_PASSWD_LEN   12
#define  USER_FULLNAME_LEN 20
#define  USER_TELE_LEN     16
#define  USER_EMAIL_LEN    32
#define  USER_SERVICE_LEN  32
#define  USER_COOKIE_LEN   30
#include <isam.h>
typedef struct cisam_webuser
{
    int        nbr;
    char       user_name[USER_NAME_LEN + 1];
    char       passwd[USER_PASSWD_LEN + 1];
    char       fullname[USER_FULLNAME_LEN + 1];
    char       tele[USER_TELE_LEN + 1];
    char       email[USER_EMAIL_LEN + 1];
    char       service[USER_SERVICE_LEN + 1];
    char       cookie[USER_COOKIE_LEN + 1];
} WEBUSER;
#ifdef KEY_SRC
struct keydesc webuser_key1 = {
    ISNODUPS,
    1,
    0, 4, LONGTYPE,
};
struct keydesc webuser_key2 = {
    ISNODUPS,
    1,
    4, 16, CHARTYPE,
};
    
#else
extern struct keydesc webuser_key1;
extern struct keydesc webuser_key2;
#endif
#endif
