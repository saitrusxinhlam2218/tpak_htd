/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Indv_db.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/05/12 19:17:36 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Indv_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _INDV_DB_H
#define _INDV_DB_H
#include <isam.h>
#include "Individual.h"
#include "Individual_private.h"
#ifdef KEY_SRC
struct keydesc indv_key1 =
{
  ISDUPS, 2,
  {{ 0, CHARSIZE, CHARTYPE },
   { 1, AC_NUMBER_LEN, CHARTYPE }}
};
struct keydesc INDV_key =
{
	ISNODUPS, 3,
	{{ 0, CHARSIZE, CHARTYPE },
	{ 1, AC_NUMBER_LEN, CHARTYPE },
	{ 1+AC_NUMBER_LEN, INDV_NAME_LEN, CHARTYPE}}
};
struct keydesc INDV_subacct_key =
{
	ISDUPS, 3,
	{{ 0, CHARSIZE, CHARTYPE },
	{ 1, AC_NUMBER_LEN, CHARTYPE },
	{ 1+AC_NUMBER_LEN+INDV_NAME_LEN, INDV_SUBACCOUNT_LEN, CHARTYPE}}
};
struct keydesc *Individual_keys[] =
{
	&INDV_key,
	&INDV_subacct_key,
	&indv_key1
};
#else
extern struct keydesc *Individual_keys[];
extern struct keydesc INDV_key;
extern struct keydesc INDV_subacct_key;
extern struct keydesc indv_key1;
#endif
#endif
