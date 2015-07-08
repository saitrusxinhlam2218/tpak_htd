/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Customer_db.h,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2001/10/03 17:54:10 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Customer_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

#ifndef _CUSTOMER_DB_H
#define _CUSTOMER_DB_H

#include <isam.h>

typedef struct cisam_cust
{
  int       nbr;
  char      phone_nbr[11];
  char      phone_ext[6];
  char      fleet;
  int       pckup_str_nbr;
  char      pckup_str_suf;
  char      pckup_pre_dir[3];
  char      pckup_str_name[21];
  char      pckup_str_key[5];
  char      pckup_str_type[4];
  char      pckup_post_dir[3];
  char      pckup_apt_room[6];
  char      pckup_city[4];
  short     pckup_zone;
  char      passenger_name[21];
  long      access_date_time;
  long      made_calls;
  char   drv_attr[65];
  char   veh_attr[65];
#ifdef NAMERICA
  int       weekly_calls;
  char      customer_comment[65];
  int       dest_str_nbr;
  char      dest_str_suf;
  char      dest_pre_dir[3];
  char      dest_str_name[21];
  char      dest_str_key[5];
  char      dest_str_type[4];
  char      dest_post_dir[3];
  char      dest_apt_room[6];
  char      dest_city[4];
  short     dest_zone;
#else
  char      customer_comment[65];
#endif

} CUSTOMERS;

#ifdef KEY_SRC
struct keydesc cust_key1 = {
  ISNODUPS,
  1,
  4, 10, CHARTYPE,
};
#else
extern struct keydesc cust_key1;
#endif

#endif

