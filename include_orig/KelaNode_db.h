/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Trip_data_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:54 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Trip_data_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Trip_data_db.h,v $	$Revision: 1.2 $"

#ifndef _KELANODE_DB_H
#define _KELANODE_DB_H
#include <isam.h>


typedef struct _KELANODE_REC
{
  char   rte_id[65];
  short  nbr;
  int    tpak_id;
  char   type;
  char   street[65];
  int    streetnbr;
  char   streetletter[6];
  char   city[33];
  char   pickuptime[21];
  char   passenger[65];
  char   phone[33];
  float  gps_lat;
  float  gps_lon;
  char   arrival_time[33];
  char   departure_time[33];
  char   eta[33];
  char   booking_id[21];
  char   full_booking_id[65];
  char   manual_descript[256];
} KELANODE_REC;

#ifdef KEY_SRC
struct keydesc kelanode_key1 = {
  ISNODUPS,
  2,
  {{ 0, 64, CHARTYPE},
   { 64, INTSIZE, INTTYPE } }
};

struct keydesc kelanode_key2= {
  ISNODUPS,
  2,
  {{ 66, LONGSIZE, LONGTYPE},
   { 64, INTSIZE, INTTYPE } }
};

#else
extern struct keydesc kelanode_key1;
extern struct keydesc kelanode_key2;
#endif


#endif
