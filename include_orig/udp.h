/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: udp.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2001/10/03 17:54:35 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/udp.h,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: udp.h,v $	$Revision: 1.2 $"

#ifndef _UDP_H_
#define _UDP_H_

#define IP_ADDR_SIZE   24
#define MESG_SIZE_LEN 3
#define MESG_TYPE_LEN 3
#define MESG_DATA_LEN 256

typedef struct _UDP_IN_ {
  char     MesgSize[ MESG_SIZE_LEN ];
  char     MesgType[ MESG_TYPE_LEN ];
  char     MesgData[ MESG_DATA_LEN ];
} UDP_IN;

typedef struct _UDP_OUT_ {
  char     dest_ip_addr[IP_ADDR_SIZE];    
  MSG_QUEUE_HDR  tpak_msg;
} UDP_OUT;

// Form Messages 100 series
#define UDP_SIGN_IN             100
#define UDP_BOOK_IN             101
#define UDP_EBOOK               102
#define UDP_MSG_FROM_DRV        103
#define UDP_CBOOK               104
#define UDP_UNBOOK              105
#define UDP_ETA                 106
#define UDP_ZONE_INQ            107
#define UDP_TEST                109

// Canned Messages 200 series
#define UDP_TRIP_ACCEPT         201
#define UDP_TRIP_REJECT         202
#define UDP_FLAG_REQ            203
#define UDP_REQ_TALK            204
#define UDP_SIGN_OFF            205
#define UDP_QP                  206
#define UDP_BREAK               207
#define UDP_LOAD                208
#define UDP_UNLOAD              209
#define UDP_CALLOUT             210
#define UDP_ADV_ARRIVAL         211

// Meter Status Messages 300 series
#define UDP_METER_ON            300
#define UDP_METER_OFF           301

#endif
