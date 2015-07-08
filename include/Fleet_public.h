/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Fleet_public.h,v $
*  @(#)  $Revision: 1.16 $
*  @(#)  $Date: 2004/06/22 17:11:18 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Fleet_public.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _FLEET_PUBLIC_H_
#define _FLEET_PUBLIC_H_
#define FLEET_MAX 8
#define RANGE_MAX	8	/* arrays of ranges for pickup times */
#define EST_LEN		8	/* length of the estimate text */
#define MAX_SUP_ZONES	16	/* maximum number of super zones */
#define FLEET_NAME_LEN  20
#define NO_APPROACH_TARIFF   '1'
#define PICKUP_WITH_METER_ON '2'
#define FORCE_SYSTEM_CALLS    1
#define IMPLIED_REJECT	      2
#define PENALTY_NO            1
#define FLEET_EPOST_ALWAYS			'1'
#define FLEET_EPOST_METER_ON			'2'
#define FLEET_EPOST_METER_ON_AND_SYSTEM_CALL	'3'
#define FLEET_ALG_ZONE_FIFO                          '3'
#define FLEET_ALG_CLOSEST_TAXI                       '4'
#define FLEET_ALG_ZONE_QUEUE_PRIMARY_CLOSEST_BACKUP  '5'
#define FLEET_MIN_BUSY_FACTOR   0.1     /* was 1.0 - 1.5, changed 1/10/92 */
#define FLEET_MAX_BUSY_FACTOR   9.9     /*     per request by G7 */
#define ATTRIB_DESC_LEN		20
#define ATTRIB_ABBREV_LEN	2
/*STATISTICS */
#define FLEET_NBR_ASSIGNS 		701
#define FLEET_NBR_BIDS 			702
#define FLEET_NBR_BREAKS 		703
#define FLEET_NBR_CALLBACKS 		704
#define FLEET_NBR_CALLOUTS 		705
#define FLEET_NBR_CALLS 		706
#define FLEET_NBR_COND_POSTS 		707
#define FLEET_NBR_EMERGENCIES 		708
#define FLEET_NBR_ENROUTE_POSTS 	709
#define FLEET_NBR_FLAGS 		710
#define FLEET_NBR_FL_VEH_ATTR 		711
#define FLEET_NBR_LATE_METERS 		712
#define FLEET_NBR_MESSAGES 		713
#define FLEET_NBR_NO_ACCEPTS 		714
#define FLEET_NBR_NO_SHOWS 		715
#define FLEET_NBR_REJECTS 		716
#define FLEET_NBR_REQ_TO_TALK 		717
#define FLEET_NBR_SHORT_METERS 		718
#define FLEET_NBR_SIGNED_OFF 		719
#define FLEET_NBR_SIGNED_ON 		720
#define FLEET_NBR_SUSPENDED 		721
#define FLEET_NBR_WAKEUPS 		722
#define FLEET_NBR_NOW_BIDS 		723
#define FLEET_NBR_NOW_BREAKS 		724
#define FLEET_NBR_NOW_CALLBACKS 	725
#define FLEET_NBR_NOW_CALLOUTS 		726
#define FLEET_NBR_NOW_CALLS 		727
#define FLEET_NBR_NOW_DRIV_ATTR         728
#define FLEET_NBR_NOW_EMERGENCIES 	729
#define FLEET_NBR_NOW_ENROUTE_POSTS	730
#define FLEET_NBR_NOW_FLAGS 		731
#define FLEET_NBR_NOW_IDLE 		732
#define FLEET_NBR_NOW_IN_FLEET 		733
#define FLEET_NBR_NOW_LATE_METERS 	734
#define FLEET_NBR_NOW_MESSAGES 		735
#define FLEET_NBR_NOW_NO_SHOWS 		736
#define FLEET_NBR_NOW_POSTS 		737
#define FLEET_NBR_NOW_REQ_TO_TALK 	738
#define FLEET_NBR_NOW_SIGNED_OFF 	739
#define FLEET_NBR_NOW_SIGNED_ON 	740
#define FLEET_NBR_NOW_SUSPENDED 	741
#define FLEET_NBR_NOW_VEH_ATTR          742
#define FLEET_NBR_NOW_WAKEUPS 		743
#define FLEET_ZONE_BID_COUNT    	750
#define FLEET_NBR_CONTROL_MSGS		751
#define FLEET_NBR_CONTROL_MSGS_NO_CHANGE 752
#define FLEET_NBR_CANCELS               753
/* VALUES  - ver incomplete */
#define FLEET_APPROVE_MSGS_TO_DRIVERS 0
#define FLEET_CALL_ACCEPT_TIME        1
#define FLEET_CALL_OFFER_PROCEDURE    2
#define FLEET_DRIVER_VEH_LIST         3
#define FLEET_ID                      5
#define FLEET_NBR                     7
#define FLEET_TIME_CALL_MAX_INTERVAL  8
#define FLEET_UNASSIGNED_TIME         9
#define FLEET_VEH_NBR_LIST            11
#define FLEET_ZONE_CALLS_LIST         13
#define	FLEET_FUTURE_CALL_MINIMUM     14
#define FLEET_ACTIVE                  15
#define FLEET_WAKEUP_LEAD_TIME        16
#define FLEET_TC_LEAD_TIME            17
#define FLEET_TIME_CALL_LEAD_SUPP     18
#define FLEET_EXCPT_LIST              19
#define FLEET_FLAG_ACTION             20
#define FLEET_GPS_STATE               21
#define FLEET_VEH_SUSP_PASS_LIST      22
#define FLEET_VEH_SUSP_DELIV_LIST     23
#define FLEET_VEH_SUSP_TYPEX_LIST     24
#define FLEET_DRV_SUSP_PASS_LIST      25
#define FLEET_DRV_SUSP_DELIV_LIST     26
#define FLEET_DRV_SUSP_TYPEX_LIST     27
#define FLEET_MATCH_ALGORITHM         28
#define FLEET_TC_LOOK_AHEAD           29
#define FLEET_AUTO_FLAG_TIME          30
#define FLEET_PKT_ID_LIST             31
#define FLEET_GPS_POS_AGE                32
#define FLEET_OFFER_RETRIES              33
#define FLEET_TRS_CEILING                34
#define FLEET_GPS_DISTANCE_CEILING       35
#define FLEET_SEND_CALL_TYPE             36
#define FLEET_AUTO_REJECT_TIME           37
#define	FLEET_FUTURE_CALL_MAXIMUM        38
#define FLEET_M_ACTION_TIMER             39   /* short */
#define FLEET_M_ACTION_RETRY             40   /* short */
#define FLEET_M_ACTION_PENALTY           41   /* short */
#define FLEET_CANX_TO_MOFF_TIME          42   /* short */
#define FLEET_GPS_MAX_X                  43   /* float */
#define FLEET_GPS_MIN_X                  44   /* float */
#define FLEET_GPS_MAX_Y                  45   /* float */
#define FLEET_GPS_MIN_Y                  46   /* float */
#define FLEET_GPS_COORD_SYS              47   /* short */
#define FLEET_GPS_BOOK                   48   /* char  */
#define FLEET_GPS_TIME                   49   /* short */
#define FLEET_TC_RESTRICT_INTERVAL       50   /* short */
#define FLEET_NS_SHORT                   51   /* short */
#define FLEET_BIG_CAR_TRIP_LIST          52   /* FLEET_BIG_CAR_TRIP_LIST_HNDL */
#define FLEET_EMERG_RANGE                53   /* short */
#define FLEET_ENROUTE_BREAK              54   /* char */
#define FLEET_ILINK_ACTIVE               55   /* char */
#define FLEET_ILINK_TIMEOUT              56   /* short */
#define FLEET_LOAD_UNLOAD                57   /* char  */
#define FLEET_CBOOK_TIME                 58   /* short */
#define FLEET_STAND_NTFY_INTERVAL        59   /* short */
#define FLEET_STAND_NTFY_LIFE            60   /* short */
#ifndef BY_NBR
#define BY_NBR -1
#endif
#define BY_ID 1
RET_STATUS    Fleet_init();      /* initializes the global fleet pointers in shmem */
FLEET_HNDL    Fleet_create(short);
RET_STATUS    Fleet_get_hndl(int, int, FLEET_HNDL *);
char          Fleet_get_id(short);
short         Fleet_get_nbr(char);
HNDL          Fleet_get_value(FLEET_HNDL, int );
RET_STATUS    Fleet_initialize(short);
RET_STATUS    Fleet_make_search_report(short, short);
RET_STATUS    Fleet_make_stand_notify_report(short);
RET_STATUS    Fleet_zone_summary(short);
RET_STATUS    Fleet_update_pickup_stats(short);
RET_STATUS    Fleet_decrement_stats(FLEET_HNDL, int, short);
int           Fleet_get_state(FLEET_HNDL, int);
HNDL          Fleet_get_value(FLEET_HNDL, int );
RET_STATUS    Fleet_increment_stats(FLEET_HNDL, int, short);   
RET_STATUS    Fleet_reset_state(FLEET_HNDL, int, HNDL);
RET_STATUS    Fleet_set_state(FLEET_HNDL, int, HNDL);
RET_STATUS    Fleet_set_value(FLEET_HNDL, int, HNDL);
/* errors */
#define FLEET_NULL_ARG	        err_encode(FLEET_OBJ, NULL_ARG)
#define FLEET_INVALID_ARG	err_encode(FLEET_OBJ, INVALID_ARG)
#endif
