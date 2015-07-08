/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Driver_public.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:42:57 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Driver_public.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
/* Driver_public.h */
#ifndef _DRIVER_PUBLIC_
#define _DRIVER_PUBLIC_
/* CONSTANTS */
#define DRIVER_NAME_LEN		25
#define DRIVER_LICENSE_NUMBER_LEN	15
#define DRIVER_TAXI_LICENSE_LEN	8
#define DRIVER_SEALS_LEN		16
#define DRIVER_ATTRIBUTES_LEN	8
#define DRIVER_SUSPEND_REASON_LEN	20
#define DRIVER_SUSPEND_MESSAGE_LEN	96
#define DRIVER_COMMENTS_LEN		80
#define DRIVER_DATE_LEN		9
#define DRIVER_TIME_LEN		6
/* VALUES */
#define DRIVER_ATTR 		0	/* DRIVER_ATTR_HNDL - vehicle intibutes */
#define DRIVER_CLASS 		1	/* char - DRIVER CLASSIFICATION.  e.g. C=Club Affaire, I=Inspector */
#define DRIVER_ID 		2	/* int */
#define DRIVER_Q_POSITION       3       /* short */
#define DRIVER_PRIORITY         4       /* short */
#define DRIVER_SIGNED_ON_DATE   5       /* time_t - time driver last signed on */
#define DRIVER_SUSP_DRV_MSG     6       /* MSG_HNDL - suspension message for driver*/
#define DRIVER_SUSP_SUP_MSG     7       /* MSG_HNDL - suspension message for supervisor*/
#define DRIVER_SUSP_FROM_DATE 	8	/* time_t */
#define DRIVER_SUSP_TO_DATE 	9	/* time_t */
#define DRIVER_SUSP_PASS_TYPE   20      /* short - passenger suspension */
#define DRIVER_SUSP_DELV_TYPE   21      /* short - delievery  suspension */
#define DRIVER_SUSP_X_TYPE      22      /* short - x  suspension */
#define DRIVER_SEALS    	23	/* DRIVER_SEALS_HNDL */
#define DRIVER_VEH_HNDL         24      /* VEH_HNDL if signed in */
#define	DRIVER_FLEET_NBR	25	/* short */
#define DRIVER_PHONE_NBR	26
#define DRIVER_FLEET_ID		27	/* char  */
#define DRIVER_NAME		28
#define DRIVER_ADDRESS		29
#define DRIVER_LICENSE_NUMBER	30
#define DRIVER_TAXI_LICENSE 	31
#define DRIVER_SUSPEND_REASON	32
#define DRIVER_COMMENTS		33
#define DRIVER_LICENSE_RENEWAL	34
#define DRIVER_TAXI_LICENSE_RENEWAL	35
#define DRIVER_SHIFT		37
/* STATISTICS */
#define DRIVER_NBR_ASSIGNS 	700	/* short */
#define DRIVER_NBR_BIDS 	701	/* short */
#define DRIVER_NBR_BREAKS 	702	/* short */
#define DRIVER_NBR_CALLBACKS 	703	/* short */
#define DRIVER_NBR_CALLOUTS 	704	/* short */
#define DRIVER_NBR_CALLS 	705	/* short */
#define DRIVER_NBR_COND_POSTS 	706	/* short */
#define DRIVER_NBR_EMERGENCIES	707	/* short */
#define DRIVER_NBR_ENROUTE_POSTS 708	/* short */
#define DRIVER_NBR_FLAGS 	709	/* short */
#define DRIVER_NBR_GOING_HOMES 	720	/* short */
#define DRIVER_NBR_LATE_METERS 	721	/* short */
#define DRIVER_NBR_MESSAGES 	722	/* short */
#define DRIVER_NBR_NO_ACCEPTS 	723	/* short */
#define DRIVER_NBR_NO_SHOWS 	724	/* short */
#define DRIVER_NBR_POSTS 	725	/* short */
#define DRIVER_NBR_REJECTS 	726	/* short */
#define DRIVER_NBR_REQ_TO_TALK 	727	/* short */
#define DRIVER_NBR_SHORT_METERS	728     /* short */
#define DRIVER_NBR_SIGN_INS	729	/* short */
#define DRIVER_NBR_SIGN_OFFS	740	/* short */
#define DRIVER_EMERGENCY_ON_DT_TM	751
#define	DRIVER_EMERGENCY_OFF_DT_TM	752
#define DRIVER_SIGNED_OFF_DT_TM		753
#define DRIVER_LAST_VEHICLE	754
#define	DRIVER_LAST_CALL	755
#define DRIVER_SUSPEND_FROM_DATE	756
#define DRIVER_SUSPEND_FROM_TIME	757
#define DRIVER_SUSPEND_TO_DATE	758
#define DRIVER_SUSPEND_TO_TIME	759
/* STATE */
#define DRIVER_SUSPENDED_STATE 	100
#define DRIVER_SIGN_IN_STATE 	101
/* DB KEYS */
#define DRIVER_KEY_PRIMARY		0
#define DRIVER_KEY_FLEET_LAST_VEH	1
#define DRIVER_KEY_FLEET_NAME		2
/* Driver IPC messages */
#define	DRIVER_UPDATE_RECORD		0
#define	DRIVER_UPDATE_TABLE		1
/* Error codes */
#define	DRIVER_NULL_ARG			err_encode(DRIVER_OBJ, NULL_ARG)
#define	DRIVER_NO_MEMORY		err_encode(DRIVER_OBJ, NO_MEMORY)
#define	DRIVER_UNSUPPORTED_ARG		err_encode(DRIVER_OBJ, UNSUPPORTED_ARG)
#define	DRIVER_INVALID_ARG		err_encode(DRIVER_OBJ, INVALID_ARG)
#define	DRIVER_IPC_FAILED		err_encode(DRIVER_OBJ, IPC_FAILED)
/* FUNCTION PROTOTYPES */
HNDL		Driver_get_value(DRIVER_HNDL, int);
RET_STATUS	Driver_set_value(DRIVER_HNDL, int, HNDL);
RET_STATUS	Driver_set_state(DRIVER_HNDL, int, HNDL);
RET_STATUS	Driver_reset_state(DRIVER_HNDL, int, HNDL);
int             Driver_get_state(DRIVER_HNDL, int);
RET_STATUS      Driver_get_hndl(int, short, DRIVER_HNDL *);
RET_STATUS      Driver_initalize(int, VEH_HNDL); 
RET_STATUS      Driver_increment_stats(DRIVER_HNDL, int);
RET_STATUS      Driver_decrement_stats(DRIVER_HNDL, int);
RET_STATUS      Driver_initialize_stats(DRIVER_HNDL, int);
RET_STATUS      Driver_send_msg(DRIVER_HNDL, int);
RET_STATUS      Driver_udpate_stats(DRIVER_HNDL);
RET_STATUS      Driver_send_status(DRIVER_HNDL);
int             Driver_check_attr(DRIVER_HNDL, CALL_HNDL);
int             Driver_check_seals(DRIVER_HNDL, int);
RET_STATUS	Driver_alloc(DRIVER_REC_HNDL *);
void		Driver_free(DRIVER_REC_HNDL *);
RET_STATUS	Driver_read(DRIVER_REC_HNDL, int, int);
RET_STATUS	Driver_add(DRIVER_REC_HNDL);
RET_STATUS	Driver_update(DRIVER_REC_HNDL);
RET_STATUS	Driver_delete(DRIVER_REC_HNDL);
RET_STATUS	Driver_release_locks();
RET_STATUS	Driver_notify_dispatch(short, DRIVER_REC_HNDL);
#endif
