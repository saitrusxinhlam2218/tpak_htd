/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: udp_link.h,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 2002/09/11 13:40:34 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/udp_link.h,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: udp_link.h,v $	$Revision: 1.4 $"

#ifndef _UDP_LINK_H_
#define _UDP_LINK_H_


//
//
// Define Message types - UDP_MSG_STRUCT.MesgType
//
//

// Form Messages 100 series
#define MDT_SIGN_IN             100
#define MDT_BOOK_IN             101
#define MDT_EBOOK               102
#define MDT_MSG_FROM_DRV        103
#define MDT_CBOOK               104
#define MDT_UNBOOK              105
#define MDT_ETA                 106
#define MDT_ZONE_INQ            107
#define MDT_RELEVE              108
#define MDT_NOSHOW              109
#define MDT_DUPE_RELEVE         110    // G7 specific
#define MDT_ACCEPT_NEW          111    // G7 specific
#define MDT_QP_NEW              112    // G7 specific
#define MDT_CHARGE_NEW          113    // G7 specific
#define MDT_STATS               114    // G7 specific
#define MDT_G7_SUR_PLACE        115    // G7 specific



// Canned Messages 200 series
#define MDT_TRIP_ACCEPT         201
#define MDT_TRIP_REJECT         202
#define MDT_FLAG_REQ            203
#define MDT_REQ_TALK            204
#define MDT_SIGN_OFF            205
#define MDT_QP                  206
#define MDT_BREAK               207
#define MDT_LOAD                208
#define MDT_UNLOAD              209
#define MDT_CALLOUT             210
#define MDT_ADV_ARRIVAL         211
#define MDT_TOGGLE              212    // G7 specific
#define MDT_EMERGENCY           213
#define MDT_ROTI                214

// Meter Status Messages 300 series
#define MDT_METER_ON            300
#define MDT_METER_OFF           301
#define MDT_GPS_UPDATE          302
#define MDT_ITM_FUNCTION        303
#define MDT_METER_OFF_NEW       304    // G7 specific

// ACK Message
#define MDT_ACK                 900
#define MDT_NOACK               901

// Outbound Terminal Control Codes 400 series
#define TPAK_AUTHORIZE           400
#define TPAK_DEAUTHORIZE         401
#define TPAK_ASSIGN_GROUP_ID     402

// Outbound Filled Forms 500 series
#define TPAK_ITM_OUTPUT          500
#define TPAK_GPS_OUTPUT          501
#define TPAK_FILLED_STATUS_LINE  502
#define TPAK_FILLED_MSG_DISPLAY  503
#define TPAK_FILLED_PROMPT_LINE  504
#define TPAK_TRIP_OFFER          505
#define TPAK_TRIP_ASSIGN         506
#define TPAK_QUEUE_POSITION      507
#define TPAK_FLAG_OK             508
#define TPAK_TRIP_CANX           509
#define TPAK_NOSHOW_OK           510

// Outbound Canned Messages 600 series
#define TPAK_CANNED_STATUS_LINE  600
#define TPAK_CANNED_MSG_DISPLAY  600
#define TPAK_CANNED_PROMPT_LINE  602

// Outbound ITM Messages 700 series
#define TPAK_ITM_OUT             700

// Broadcast Messages 800 series
#define TPAK_BCAST_F31  800
#define TPAK_BCAST_F32  801
#define TPAK_BCAST_F33  802
#define TPAK_BCAST_F34  803
#define TPAK_EMERG_BCAST 815

#define TPAK_DEFOBJ_NVRAM 900

#define MMP_EOM "%E"

#endif

