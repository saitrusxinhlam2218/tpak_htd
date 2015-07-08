/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Object.h,v $
*  @(#)  $Revision: 1.10 $
*  @(#)  $Date: 2003/01/28 18:16:14 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Object.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Object_types.h"
#include "Object_defines.h"
#include "Object_functions.h"
#include "taxi_error.h"

typedef HNDL AC_HNDL;
typedef HNDL AT_HNDL;	/* Account_Type */
#ifdef NO_DEBUG
typedef HNDL CALL_HNDL;
typedef HNDL TC_HNDL;
typedef HNDL DRIVER_HNDL;
typedef HNDL FLEET_HNDL;
typedef HNDL LIST_HNDL;
typedef HNDL MSG_HNDL;
typedef HNDL SCR_HNDL;
typedef HNDL TLG_HNDL;
typedef HNDL VEH_HNDL;
typedef HNDL ZONE_HNDL;
typedef HNDL ZONE_RESP_HNDL;
typedef HNDL VEH_RESP_HNDL;
typedef HNDL EXCPT_HNDL;
typedef HNDL CALL_MSG_HNDL;
typedef HNDL EMERG_BCAST_HNDL;
typedef HNDL ZONE_STAND_HOLD_HNDL;
#else
typedef struct call_ptr * CALL_HNDL;
typedef struct _tc_struct * TC_HNDL;
typedef HNDL DRIVER_HNDL;
typedef struct fleets * FLEET_HNDL;
typedef HNDL LIST_HNDL;
typedef HNDL MSG_HNDL;
typedef HNDL SCR_HNDL;
typedef HNDL TLG_HNDL;
typedef struct veh_driv * VEH_HNDL;
typedef struct zones * ZONE_HNDL;
typedef HNDL  EXCPT_HNDL;
typedef HNDL CALL_MSG_HNDL;
typedef HNDL HTTP_HNDL;
#endif

typedef HNDL AC_REC_HNDL;	/* Account */
typedef HNDL ACAD_REC_HNDL;	/* Account_Address */
typedef HNDL AD_REC_HNDL;	/* Access_Description */
typedef HNDL ADR_REC_HNDL;      /* Address */
typedef HNDL AL_REC_HNDL;	/* Alias */
typedef HNDL AT_REC_HNDL;	/* Account_Type */
typedef	HNDL DRIVER_REC_HNDL;	/* Driver */
typedef HNDL CALL_REC_HNDL;
typedef HNDL CMT_REC_HNDL;	/* Comment */
typedef HNDL FLEET_REC_HNDL;
typedef HNDL HL_REC_HNDL;	/* Holiday */
typedef HNDL INDV_REC_HNDL;	/* Individual */
typedef HNDL LOC_REC_HNDL;	/* Locality */
typedef HNDL PL_REC_HNDL;	/* Place */
typedef	HNDL RES_REC_HNDL;	/* Reservation */
typedef	HNDL RC_REC_HNDL;	/* Reservation Cost */
typedef	HNDL RP_REC_HNDL;	/* Reservation Period */
typedef HNDL STR_REC_HNDL;	/* Street_Name */
typedef HNDL TLG_REC_HNDL;	/* Telephone_Line_Group */
typedef HNDL VEH_REC_HNDL;	/* Vehicle */
typedef HNDL ZONE_REC_HNDL;
typedef HNDL CRED_TRANS_REC_HNDL; /* Credit Transactions */
typedef HNDL TCBMAINT_REC_HNDL;
typedef HNDL CB_CONTROL_REC_HNDL;
typedef HNDL ZONE_RESP_HNDL;
typedef HNDL VEH_RESP_HNDL;
typedef HNDL TRS_CALC_HNDL;
typedef HNDL PKT_ID_HNDL;
typedef HNDL EMERG_BCAST_HNDL;
typedef HNDL ZONE_STAND_HOLD_HNDL;

typedef LIST_HNDL BIDDERS_LIST_HNDL;
typedef LIST_HNDL BID_LIST_HNDL;
typedef LIST_HNDL CALLS_LIST_HNDL;
typedef LIST_HNDL CALL_TIMER_LIST_HNDL;
typedef LIST_HNDL CALL_MULT_CALLS_LIST_HNDL;
typedef LIST_HNDL CALL_REJECT_VEH_LIST_HNDL;
typedef LIST_HNDL FLEET_DRIVER_VEH_LIST_HNDL;
typedef LIST_HNDL FLEET_VEH_ID_LIST_HNDL;
typedef LIST_HNDL FLEET_VEH_NBR_LIST_HNDL;
typedef LIST_HNDL FLEET_ZONE_CALLS_LIST_HNDL;
typedef LIST_HNDL VEH_CALLS_LIST_HNDL;
typedef LIST_HNDL ZONE_CALLS_LIST_HNDL;
typedef LIST_HNDL ZONE_TC_LIST_HNDL;
typedef LIST_HNDL ZONE_VEH_LIST_HNDL;
typedef LIST_HNDL ZONE_LAST_CALLS_LIST_HNDL;
typedef LIST_HNDL TRS_VEH_LIST_HNDL;
typedef LIST_HNDL TRS_CALC_LIST_HNDL;
typedef LIST_HNDL FLEET_PKT_ID_LIST_HNDL;
typedef LIST_HNDL FLEET_BIG_CAR_TRIP_LIST_HNDL;
typedef LIST_HNDL CALL_MULT_LIST_HNDL;
typedef LIST_HNDL FLEET_EXCPT_LIST_HNDL;
typedef HNDL FLEET_SUSP_LIST_HNDL;
typedef LIST_HNDL VEH_EXCPT_LIST_HNDL;
typedef LIST_HNDL CALLS_EXCPT_LIST_HNDL;
typedef LIST_HNDL CALL_MSG_LIST_HNDL;
typedef LIST_HNDL VEH_HTTP_LIST_HNDL;

#define TIMER_HNDL TIMER_HANDLE

 /*
 *
 *  This defines object identifiers.  Each object
 *  will be assigned an ID.  In adition, each
 *  object will have an associated prefix, which will
 *  be a string of 2-4 characters, as identified by the
 *  the distinguishing feature of the ID symbol.
 */

#define TAXI_OBJ   0 /* the Orphanage, if something has no real home */
#define AD_OBJ     1 /* Access Description */
#define AC_OBJ     2 /* Account */
#define ACAD_OBJ   3 /* Account Address */
#define AT_OBJ     4 /* Account Type */
#define ADR_OBJ    5 /* Address */
#define AL_OBJ     6 /* Alias */
#define CALL_OBJ   7 /* Call */
#define CH_OBJ     8 /* Call History */
#define CMT_OBJ    9 /* Comments */
#define DIS_OBJ   10 /* Dispatcher */
#define DRIVER_OBJ    11 /* Driver */
#define EL_OBJ    12 /* Error Log */
#define EX_OBJ    13 /* Exceptions */
#define FLD_OBJ   14 /* Field */
#define FLEET_OBJ    15 /* Fleet */
#define GEO_OBJ   16 /* Geo */
#define HOL_OBJ   17 /* Holiday */
#define INDV_OBJ  18 /* Individual */
#define LOC_OBJ   19 /* Localty */
#define MESS_OBJ  20 /* Messages */
#define PL_OBJ    21 /* Place names */
#define RTP_OBJ   22 /* Report Manager */
#define RES_OBJ   23 /* Reservation */
#define RC_OBJ    24 /* Reservation Cost */
#define RP_OBJ    25 /* Reservation Period */
#define SCR_OBJ   26 /* Screen */
#define SH_OBJ    27 /* Shift */
#define STR_OBJ   28 /* Street Name */
#define SUB_OBJ   29 /* Subscription Call */
#define TLG_OBJ   30 /* Telephone Line Group */
#define XFR_OBJ   31 /* Transfer File */
#define avail_1   32 /* Trip Transfer File */
#define UZ_OBJ    33 /* Unzoned Address */
#define avail_2   34 /* Validation Transfer File */
#define VEH_OBJ   35 /* Vehicle */
#define ZONE_OBJ  36 /* Zone */
#define HL_OBJ    37 /* Holiday */
#define	MT_OBJ    38 /* Minitel */
#define	TES_OBJ   39 /* TaxiPak External Services */
#define CRED_OBJ  40 /* Credit Transactions */
#define TCBMAINT_OBJ 41
#define CB_CONTROL_OBJ 42
#define EXCPT_OBJ 43


#define DB_OBJ   254 /* database category */
#define UNIX_OBJ 255 /* useful for error encoding within TP err scheme */

#define LAST_OBJ 255 /* This is the upper bound, must fit in 8 bits */


#endif    /* must be last line of file */
