/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call_public.h,v $
*  @(#)  $Revision: 1.14 $
*  @(#)  $Date: 2003/01/28 18:16:14 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Call_public.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _CALL_PUBLIC_H_
#define _CALL_PUBLIC_H_
/* Call_public.h */
typedef int CALL_ID;
/* defines */
/* db related defines */
#define	CALL_CHARGE_APPROVE_LEN	10
#define	CALL_DISPLAY_DATE_LEN	DATE_LEN
#define	CALL_DISPLAY_TIME_LEN	TIME_LEN
#define	CALL_EXT_TYPE_LEN	5
#define	CALL_GEN_COMMENT_LEN	64
#define	CALL_HOLD_LINE_LEN	4
#define	CALL_MAP_DATA_LEN	8
#define	CALL_STATUS_BITS_LEN	4
#define	CALL_STATUS_LEN		8
#define CALL_PASSENGER_KEY_LEN	4
#define CALL_STREET_NAME_KEY_LEN	4
#define CALL_KEY_ABBR_LEN	4
#define NOT_PERS             0
#define IF_PERS              1
#define ONLY_PERS            2
#define APPR_PERS_ONLY       3
#define CALL_AVAILABLE      77
#define CALL_NOT_AVAILABLE  78
#define CALL_REJECTED       91
#define CALL_NO_ACCEPTED    92
#define NONE                0  /* for Call_add_history */
/* for call counting */
#define INCREMENT_NOW       0
#define INCREMENT_BOTH      1
#define DECREMENT_BOTH      2
#define DECREMENT_NOW       3
#define DECREMENT_TODAY     4
/* for timer monotoring */
#define TIMEOUT_START      2
#define TIMEOUT_COMPLETE   3
#define BID_START          4
#define BID_COMPLETE       5
/* for no accepts */
#define CALL_NO_ACCEPT_NO_PENALTY ((HNDL) 0)
#define CALL_NO_ACCEPT_PENALTY ((HNDL) 1)
#define CALL_NAK_ON_CALL_OFFER ((HNDL) 2)
/* values  - some may become private*/
#define CALL_ADVISE_ARRIVAL       0   /* short was char - YES/NO */
#define CALL_CALL_VEH_LIST        1   /* CALL_VEH_LIST_HNDL - vehicles rejecting call */
#define CALL_CURRENT_LEVEL        2   /* short - current call matching level */
#define CALL_DB_INDEX             3   /* int - db number - cisam_nbr */
#define CALL_DEST_ZONE            4   /* short */
#define CALL_GROUPED_WITH         5   /* int - call nbr */
#define CALL_ETA_TIME             6   /* time_t */
#define CALL_EVENT_TIME           7   /* time of selected event */
#define CALL_EVENT                8   /* event name */
#define CALL_FLEET_NBR            9   /* short */
#define CALL_MULT_CALLS_LIST      10  /* LIST_HNDL multiple call grouping */
#define CALL_NBR                  11  /* int */
#define CALL_PICKUP_LEAD_TIME     12  /* int - get from Zone */
#define CALL_PICKUP_MAP_STR       13  /* char * - map location */
#define CALL_PICKUP_ZONE          14  /* short */
#define CALL_PERSONAL_REQUEST_TYPE 15  /* short was char - NOT_PERS, IF_PERS, ONLY_PERS, APPR_ONLY*/
#define CALL_PRIORITY             16  /* short */
#define CALL_REJECT_VEH_LIST      17  /* CALL_REJECT_VEH_LIST_HNDL - rejected by list */
#define CALL_TIME_DUE             18  /* time_t */
#define CALL_TIME_ENTERED         19  /* time_t */
#define CALL_TIME_INFO            20  /* time_t time offered or dispatched*/
#define CALL_TYPE_CHARGE          30  /* short TRUE/FALSE */
#define CALL_TYPE_DELIVERY        31  /* short TRUE/FALSE */
#define CALL_TYPE_MULTIPLE        32  /* short TRUE/FALSE MAY DELETE */
#define CALL_TYPE_PASSENGER       33  /* short TRUE/FALSE */
#define CALL_TYPE_SUBSCRIPTION    34  /* short TRUE/FALSE */
#define CALL_TYPE_TIME_CALL       35  /* short TRUE/FALSE */
#define CALL_TYPE_WAKEUP          36  /* short TRUE/FALSE */
#define CALL_VEH_DRIV_ATTR        37  /* short - TRUE/FALSE are veh_driv attr specified */
#define CALL_VEH_ATTR_HNDL        38  /* VEH_ATTR_HNDL - vehicle attributes */
#define CALL_DRIVER_ATTR_HNDL     39  /* VEH_DRIVER_ATTR_HNDL - vehicle attributes */
#define CALL_VEH_HNDL             40  /* VEH_HNDL */
#define CALL_EXCPT_LIST           41
#define CALL_GPS_LAT              42  /* int */
#define CALL_GPS_LONG             43  /* int */
#define CALL_MSG_LIST             44  /* CALL_MSG_LIST_HNDL */
#define CALL_MSG_NBRS             45  /* char */
#define CALL_PERSONAL_VEH_NBR     46  /* short */
#define CALL_TIMER_LIST           47  /* LIST_HNDL */
#define CALL_TC_ACTIVATE_TIME     48  /* time_t    */
#define CALL_M_ACTION_RETRY       49  /* short     */
#define CALL_ACCEPT_TIME          50  /* time_t    */
#define CALL_TYPE_ILINK           51  /* short TRUE/FALSE */
#define CALL_TYPE_ILINK_REJECT    52  /* short TRUE/FALSE */
/* db record values */
#define CALL_FLEET 50
/* statisitcs */
#define CALL_NBR_REJECT           715  /* short - number of rejects */
#define Call_set_info Call_set_value
/* state */
#define CALL_ACCEPT_STATE            100
#define CALL_AVAILABLE_STATE         101
#define CALL_BID_STATE               102
#define CALL_CLOSED_STATE            103    /* really PICKUP rset - for Call_add_history */
#define CALL_FLAG_REQUEST_STATE      104
#define CALL_HOLD_STATE              105
#define CALL_LATE_METER_STATE        106
#define CALL_MULT_CALL_STATE         107
#define CALL_NOSHOW_STATE            109
#define CALL_OFFERED_STATE           110
/* #define CALL_PERS_STATE              11x  NEEDED ?? */
/* #define CALL_ONLY_PERS_STATE         11x  NEEDED ?? */
#define CALL_PICKUP_STATE            111
#define CALL_SHORT_METER_STATE       112
#define CALL_TIMEOUT_STATE           114
#define CALL_WAIT_EPOST_STATE        115
#define CALL_PERS_ATTR_OVERRIDE      116
#define CALL_CBOOK_OFFERED_STATE     117
/* DB KEYS */
#define CALL_KEY1        1
#define CALL_KEY2        2
#define CALL_KEY3        3
#define CALL_KEY4        4
#define CALL_KEY5        5
#define CALL_KEY6        6
#define CALL_KEY7        7
#define CALL_KEY8        8
#define CALL_KEY9        9
#define CALL_KEY10       10
#define CALL_KEY_NBR			CALL_KEY1
#define CALL_KEY_FLEET_DUE_DATE_TIME	CALL_KEY3
#define CALL_PRIMARY_KEY		CALL_KEY1
/* Offset into extended type field */
#define CALL_TYPE_OFFSET	0
/* 1 not used */
#define CALL_TIME_OFFSET	2
#define CALL_CHARGE_OFFSET	3
#define CALL_MULT_OFFSET	4
/* 5 not used */
/* Priority status modes */
#define CALL_PENDING_PRI_STATUS	63
/* Redispatch cause codes */
#define NORMAL_REDISPATCH        1
#define M_ACTION_REDISPATCH      2
#define ILINK_REDISPATCH         3
/* FUNCTION PROTOTYPES */
CALL_HNDL   Call_activate();
RET_STATUS  Call_accept(CALL_HNDL);
RET_STATUS  Call_add_history(CALL_HNDL, VEH_HNDL, short, short, char, int);
RET_STATUS  Call_call_out(CALL_HNDL);
RET_STATUS  Call_cancel(CALL_HNDL, VEH_HNDL, short, int);
RET_STATUS  Call_cancel_bid(CALL_HNDL);
RET_STATUS  Call_create(CALL_REC_HNDL);
RET_STATUS  Call_db_add(CALL_REC_HNDL hndl);
RET_STATUS  Call_db_delete(CALL_REC_HNDL hndl);
RET_STATUS  Call_db_update(CALL_REC_HNDL hndl);
RET_STATUS  Call_deactivate(CALL_HNDL);
RET_STATUS  Call_decrement_stats(CALL_HNDL, int);
RET_STATUS  Call_destroy(CALL_HNDL);
#ifndef EXT_CALL_ETA
RET_STATUS  Call_eta(CALL_HNDL, VEH_HNDL, int);
#endif
RET_STATUS  Call_get_hndl(int , CALL_HNDL *);
RET_STATUS  Call_get_info(CALL_HNDL, int, HNDL *);
RET_STATUS  Call_get_rec_info(CALL_REC_HNDL, int, HNDL *);
CALL_REC_HNDL Call_get_record(int, int);
int         Call_get_state(CALL_HNDL, int);
HNDL        Call_get_value(CALL_HNDL, int);
RET_STATUS  Call_get_status();
RET_STATUS  Call_increment_stats(CALL_HNDL, int);
CALL_HNDL   Call_initialize();
RET_STATUS  Call_initialize_stats(CALL_HNDL, int);
RET_STATUS  Call_ipc_rec_load();
int         Call_is_zoned(CALL_HNDL);
RET_STATUS  Call_load();
RET_STATUS  Call_match(CALL_HNDL);
RET_STATUS  Call_rec_alloc(CALL_REC_HNDL * hndl);
RET_STATUS  Call_rec_free(CALL_REC_HNDL * hndl);
RET_STATUS  Call_rec_read( CALL_REC_HNDL hndl, int mode, int key_type);
RET_STATUS  Call_redispatch(CALL_HNDL, int);
RET_STATUS  Call_release_locks(void);
void        Call_remove_reject_veh(int, int);
RET_STATUS  Call_reset_state(CALL_HNDL, int, HNDL);
RET_STATUS  Call_send_assign(CALL_HNDL, VEH_HNDL);
RET_STATUS  Call_send_cancel(CALL_HNDL, VEH_HNDL);
RET_STATUS  Call_send_offer(CALL_HNDL, VEH_HNDL);
RET_STATUS  Call_set_rec_info(CALL_REC_HNDL, int, HNDL);
RET_STATUS  Call_set_state(CALL_HNDL, int, HNDL);
RET_STATUS  Call_set_value(CALL_HNDL, int, HNDL);
RET_STATUS  Call_update(CALL_HNDL, CALL_REC_HNDL);
RET_STATUS  Call_update_counts(CALL_HNDL, short);
#endif
