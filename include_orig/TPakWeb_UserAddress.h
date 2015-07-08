/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: TPakWeb_UserAddress.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2001/02/01 19:39:55 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/TPakWeb_UserAddress.h,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: TPakWeb_UserAddress.h,v $	$Revision: 1.2 $"

#ifndef _TPAKWEB_USERADDRESS_H
#define _TPAKWEB_USERADDRESS_H


#include <isam.h>


#ifdef KEY_SRC
struct keydesc webaddr_key1 = {
    ISDUPS,
    1,
    0, 4, LONGTYPE,
};
#else
extern struct keydesc webaddr_key1;
#endif

typedef struct cisam_webaddr {
    int          webuser_nbr;
    char         str_pre_dir[3];
    char         str_name[21];
    char         str_type[4];
    char         str_post_dir[3];
    char         str_city[4];
    int          str_nbr;
    char         str_apartment[6];
    char         str_comment[31];
    char         str_ct_comment[31];
    char         place_name;
    int          place_index;
} WEBADDR;



#endif




