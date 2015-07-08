/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Vehicle_public.h,v $
*  @(#)  $Revision: 1.15 $
*  @(#)  $Date: 2004/05/12 12:19:05 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Vehicle_public.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _VEHICLE_PUBLIC_
#define _VEHICLE_PUBLIC_
#include "acc.h"
#define SIGNED_ON(veh_ptr)  ((veh_ptr != NULL) && (veh_ptr->t_status.inuse))
#define POSTED(veh_ptr) 	veh_ptr->t_status.posted 
#define OFFERED(veh_ptr)	veh_ptr->t_status.offered
#define ACCEPTED(veh_ptr)	veh_ptr->t_status.accepted
#define BUSY(veh_ptr)		veh_ptr->t_status.pckup
#define EPOSTED(veh_ptr)        veh_ptr->t_status.e_posted
#define VEH_DRIV_MAX 2600
#define ROOKIE '2'  /* DRIVER *??? */
/* Vehicle_public.h */
#define BY_MID (0)
#ifndef BY_NBR
#define BY_NBR (-1)
#endif
#define VEH_NO_POST (-10)
#define VEH_REJECT_PENALTY (0)
#define VEH_STACKED_CALLS_CHECK ((HNDL)1)
#define VEH_NO_POST_CHECK ((HNDL)2)
/* Size Constants */
#define VEH_QP_MAX_LEN		16
#define VEH_MID_LEN			MID_SIZE
#define VEH_LICENSE_NBR_LEN		10
#define VEH_SEALS_LEN		16
#define VEH_SUSP_REASON_LEN		20
#define VEH_SUSP_MESSAGE_LEN	96
#define VEH_ATTR_LEN		8
#define VEH_SUSP_FROM_DATE_LEN	9
#define VEH_SUSP_FROM_TIME_LEN	6
#define VEH_SUSP_TO_DATE_LEN		9
#define VEH_SUSP_TO_TIME_LEN		6
#define VEH_COMMENT_LEN		80
#define MID_MAX				3839
/* VALUES */
#define VEH_ATTR_HNDL	 	0	/* VEH_ATTR_HNDL - vehicle attributes */
#define VEH_BID_ZONE            1      /* short */
#define VEH_C_POST_TIME 	2	/* time_t */
#define VEH_CALL_NBR 		3	/* int */
#define VEH_CALL_HNDL		4	/* CALL_HNDL */
#define VEH_CALL_OFFER_LEVEL 	5 	/* short -   LEVEL NUMBER OFFERED AT */
#define VEH_DRIVER_HNDL         6       /* DRIVER_HNDL */
#define VEH_E_POST_TIME 	21	/* time_t */
#define VEH_EMERGENCY_OFF_DATE 	22	/* time_t */
#define VEH_EMERGENCY_ON_DATE 	23	/* time_t */
#define VEH_EPOST_ZONE		24	/* short -   ZONE NUMBER FOR ENROUTE POSTING */
#define VEH_FLEET 		25	/* char - fleet indicator character */
#define VEH_FLEET_NBR		26	/* short */
#define VEH_INFO_TIME		27	/* time_t - time last q position inqry made should HIDE MDT does*/
#define VEH_INQRY_TIME		28	/* time_t - time last zone inqry made should HIDE */
#define VEH_ITM_INFO     	29	/* struct itm_struct * -  FOR MISSING RECEIPTS */
#define VEH_MID              	41	/* char */
#define VEH_NBR  		42	/* short */
#define VEH_CALL_LIST_HNDL	43	/* LIST_HNDL - list of stacked calls */
#define VEH_ON_BREAK_TIME 	44	/* time_t */
#define VEH_PRIORITY  		45	/* short - was char PRIORITY OF A DRIVER FOR QUEUE POSITIONING */
#define VEH_Q_POSITION          46      /* short - get only - position in zone queue */
#define VEH_REJECT_TIME 	48	/* time_t   TIME OF REJECTING A CALL */
#define VEH_SEALS_HNDL 		49	/* VEH_SEALS_HNDL */
#define VEH_SIGNED_OFF_DATE 	50	/* time_t   LAST DATE THIS DRIVER BECAME IDLE. */
#define VEH_SIGNED_ON_DATE 	60	/* time_t   LAST DATE THIS DRIVER WAS ON THE STREET. */
#define VEH_STAT_START_TM	61	/* time_t   START TIME OF A STATISTICS  int -ERVAL */	
#define VEH_SUSP_FROM_DATE 	62	/* time_t   DATE THIS VEHICLE WAS LAST SUSPENDED. */
#define VEH_SUSP_TO_DATE 	63	/* time_t   DATE VEHICLE WAS LAST RESTORED TO SERVICE. */
#define VEH_SUSP_MSG            64      /* char * suspension message re vehicle */
#define VEH_TERM_TYPE          	65	/* short -   WHAT KND OF TERMINAL IT GOT */
#define VEH_TIME_INFO 		66	/* time_t   TIME TAXI POSTED TO ZONE, ACCEPTED OR METERED ON */
#define VEH_WARM_TIME		67	/* time_t   SPECIAL PURPOSE FIELD FOR WARM STANDBY used by ??*/
#define VEH_ZONE_NBR		68	/* short -   ZONE # FOR POSTING AND CALLS*/
#define VEH_EXCPT_LIST          69      /* LIST_HNDL */
#define VEH_GPS_X               70      /* long      */
#define VEH_GPS_Y               71      /* long      */
#define VEH_GPS_TIME            72      /* time_t    */
#define VEH_LAST_MSG_NBR        73      /* short     */
#define VEH_HOST_LINK           74      /* char      */
#define VEH_CRSE_CANX_TIME      75      /* time_t    */
#define VEH_CANX_TYPE           76      /* short     */
#define VEH_GPS_STATE           77      /* char      */
#define VEH_HELD_EPOST_CALL     78      /* CALL_HNDL */
#define VEH_IP_ADDRESS          79      /* char *    */
#define VEH_LINK_TYPE           80      /* char      */
#define VEH_NAV_TYPE            81      /* char 'S' - star2000 */
#define VEH_SEND_GPS_NOT_IN_ZONE 82     /* BOOLEAN   */
#define VEH_DRV_ID              83      /* int       */
#define VEH_RF_ID               84      /* unsigned long */
#define VEH_ROTI_STATE          85      /* BOOLEAN   */
/* STATISTICS */
/* STATE */
#define VEH_ACCEPT_STATE           100
#define VEH_BID_STATE              101
#define VEH_CALL_OFFER_STATE       102
#define VEH_EMERGENCY_STATE        103
#define VEH_EPOSTED_STATE          104
#define VEH_FLAG_STATE             105
#define VEH_FLAG_REQUEST_STATE     106
#define VEH_IDLE_STATE             107
#define VEH_IN_USE_STATE           108
#define VEH_LATE_METER_STATE       109
#define VEH_LOCAL_STATE            110
#define VEH_METER_ON_STATE         111
#define VEH_MDT_WORKING_STATE      112
#define VEH_MSG_STATE              113
#define VEH_NOSHOW_STATE           114
#define VEH_OK_TO_MATCH_STATE      121
#define VEH_OK_TO_PICKUP_STATE     122
#define VEH_ON_BREAK_STATE         123
#define VEH_ON_CALL_STATE          124
#define VEH_PICKUP_STATE           125
#define VEH_POSTED_STATE           126
#define VEH_SHORT_METER_STATE      129
#define VEH_SIGN_IN_STATE          130
#define VEH_STACKED_CALLS_STATE    131
#define VEH_SUSPENDED_STATE        132
#define VEH_TALK_STATE             133
#define VEH_TCB_MAINT_STATE        134
#define VEH_EMERGENCY_RESOLVED_STATE 135
/* KEYS */
#define VEH_ACCEPT_KEY		 800
#define VEH_ACK_KEY    	         801
#define VEH_BID_KEY		 802
#define VEH_BREAK_KEY		 803
#define VEH_CALL_OUT_KEY	 804
#define VEH_CLOSE_KEY		 805
#define VEH_CPOST_KEY		 806
#define VEH_CREDIT_KEY		 807
#define VEH_EPOST_KEY	         808
#define VEH_ETA_KEY		 809
#define VEH_FLAG_REQUEST_KEY	 810
#define VEH_ITM_KEY    	         815
#define VEH_METER_FAULT_KEY      816
#define VEH_METER_OFF_KEY        817 
#define VEH_METER_ON_KEY         818
#define VEH_MSG_KEY		 820
#define VEH_NAK_KEY    	         821
#define VEH_NOSHOW_KEY		 822
#define VEH_PICKUP_KEY		 823
#define VEH_POST_KEY		 824
#define VEH_Q_POSITION_KEY	 825
#define VEH_REJECT_KEY		 826
#define VEH_SIGN_IN_KEY		 827
#define VEH_SIGN_OFF_KEY	 828
#define VEH_TALK_KEY		 829
#define VEH_UNPOST_KEY		 830
#define VEH_ZONE_INQUIRY_KEY 	 831
#define VEH_STAND_NOTIFY_KEY     832
/* Vehicle Constants */
#define	VEH_AUTHORIZE_AND_OPEN_PORT	"021001"
#define VEH_OPEN_MDT_SERIAL_PORT 	"000001"
#define VEH_SEND_TO_PRINTER 		"200000"
#define VEH_PRINTER_HEADER 		"["
#define VEH_PRINTER_TRAILER 		"]"
#define VEH_DRIVER_TOGGLE_TOO_SOON	1
#define VEH_NOT_VALID_DRIVER_CLASS	2
#define VEH_MESSAGE_NOT_SENT		3
#define VEH_BAD_ARGS			4
#define VEH_WARM_STATUS_SIGNOFF     	0
#define VEH_WARM_STATUS_SIGNON      	1
#define VEH_WARM_STATUS_POST        	2
#define VEH_WARM_STATUS_EPOST       	3
/* DB KEYS */
#define	VEH_KEY_PRIMARY		1
#define	VEH_KEY_MID		0
#define	VEH_KEY_FLEET_VEH_NBR	1
#define	VEH_KEY_FLEET_LAST_DRIV	2
/* VEH IPC MESSAGES */
#define	VEH_UPDATE_RECORD	0
#define	VEH_UPDATE_TABLE	1
/* ERROR CODES */
#define	VEH_NULL_ARG			err_encode(VEH_OBJ, NULL_ARG)
#define	VEH_NO_MEMORY			err_encode(VEH_OBJ, NO_MEMORY)
#define VEH_UNSUPPORTED_ARG		err_encode(VEH_OBJ, UNSUPPORTED_ARG)
#define VEH_INVALID_ARG			err_encode(VEH_OBJ, INVALID_ARG)
#define VEH_IPC_FAILED			err_encode(VEH_OBJ, IPC_FAILED)
/* FUNCTION PROTOTYPES */
RET_STATUS      Veh_call_cancel(VEH_HNDL, CALL_HNDL);
int             Veh_check_attr(VEH_HNDL, CALL_HNDL, BOOLEAN); 
int             Veh_check_seals(VEH_HNDL, short); 
int             Veh_get_state(VEH_HNDL, int);
HNDL		Veh_get_value(VEH_HNDL, int);
RET_STATUS	Veh_reset_state(VEH_HNDL, int, HNDL, HNDL);
RET_STATUS	Veh_set_state(VEH_HNDL, int, HNDL);
RET_STATUS	Veh_set_value(VEH_HNDL, int, HNDL);
RET_STATUS      Veh_get_status();
RET_STATUS      Veh_initalize(VEH_HNDL); /* TBD */
RET_STATUS      Veh_process_key(VEH_HNDL, short, HNDL, HNDL);
RET_STATUS      Veh_redispatch_call(VEH_HNDL, CALL_HNDL, int);
RET_STATUS      Veh_send_status();
RET_STATUS	Veh_send_msg_text(VEH_HNDL, char *, char *);
RET_STATUS	Veh_send_to_printer(VEH_HNDL, char *);
RET_STATUS      Veh_services_start();
RET_STATUS      Veh_services_stop();
RET_STATUS      Veh_stacked_calls_add(VEH_HNDL, CALL_HNDL);
RET_STATUS      Veh_stacked_calls_delete(VEH_HNDL, CALL_HNDL);
RET_STATUS      Veh_suspend(VEH_HNDL, int);
RET_STATUS      Veh_get_hndl(int, int, short, VEH_HNDL *);
RET_STATUS	Veh_alloc(VEH_REC_HNDL *);
void		Veh_free(VEH_REC_HNDL *);
RET_STATUS	Veh_read(VEH_REC_HNDL, int, int);
RET_STATUS	Veh_add(VEH_REC_HNDL);
RET_STATUS	Veh_update(VEH_REC_HNDL);
RET_STATUS	Veh_delete(VEH_REC_HNDL);
RET_STATUS	Veh_release_locks();
RET_STATUS	Veh_notify_dispatch(short, VEH_REC_HNDL);
#define         VEH_GPS_STATE_OFF            0
#define         VEH_GPS_STATE_IDLE           1
#define         VEH_GPS_STATE_ACCEPT         2
#define         VEH_GPS_STATE_FLAG           3
#endif
