/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Account_Address_db.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/05/12 19:17:19 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Account_Address_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _ACAD_ADDRESS_DB_H
#define _ACAD_ADDRESS_DB_H
#include <isam.h>
#include "Account_Address.h"
#include "Account_Address_private.h"
#include "Account.h"
#ifdef KEY_SRC
struct keydesc ACAD_key = {
  ISDUPS,
  3,
  {
    { 0, CHARSIZE, CHARTYPE },
    { 1, 18, CHARTYPE },
    {19, LONGSIZE, LONGTYPE}
  }
};
struct keydesc *Account_Address_keys[] =
{
	&ACAD_key
};
#else
extern struct keydesc *Account_Address_keys[];
extern struct keydesc ACAD_key;
#endif
#endif
