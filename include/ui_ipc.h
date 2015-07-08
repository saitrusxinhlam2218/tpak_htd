 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_ipc.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:50:16
 * @(#)  Last delta: 12/2/94 at 18:10:58
 * @(#)  SCCS File: /taxi/sccs/s.ui_ipc.h
 *                                                                           *
 *****************************************************************************/
#ifndef _UI_IPC_H
#define _UI_IPC_H
#include "ui_restrict_time_calls.h"
/* request rec_type 's */
#define CALL_REQ  '1'     /* call request */
#define EXCEPT_REQ  '2'   /* exception request */
#define ZONE_REQ  '3'     /* zone status display request */
#define VEH_REQ   '4'     /* vehicle status display request */
#define DRV_REQ   '5'     /* send message to driver request */
#define DFM_REQ   '6'     /* disk file maintenance request */
#define KDT_REQ   '7'     /* emulate a kdt */
#define DBG_REQ   '8'     /* emulate a kdt */
#define SP_CALL_REQ '9'   /* SamPlan FTJ-call request (ENH0002) */
#define SP_EXCEPT_REQ 'A' /* SamPlan request for exception generation */
#define SP_ITM_MSG    'B' /* Call Handler request for sendisg messages to itm & printer */
#define SP_MDT_MSG    'C' /* Call Handler request for sendisg messages to mdt */
#define TC_REQ        'D'
#define PI_ZONE_INFO_REQ 'E'
#define EMERG_BCAST_REQ 'F'
#define ZONE_STAND_HOLD 'G'
#define ILINK_REQ  'I'
#define GPS_POS_REQ 'J'
#define ZONE_STAND_HOLDv2 'K'
#define DISP_DEST	1	/* used for ID of dispatch for ipc */
#define VEH_IN_ZN_RESP 10	/* number of vehicles in the zone status ipc response */
#define BCK_IN_ZN_RESP 16	/* number of backups in the zone status ipc response */
#define ITEMS_IN_ZN_RESP 4	/* number of items for each backup in the zone status ipc response */
struct  zone_rqst {
	long d_prcs_id;		     /* Id of dispatch (for destination) */
	long u_prcs_id;		     /* message type = users process ID */
	char rec_type;		     /* application type code */
	char fleet;		     /* Fleet Indicator  */
	short zone_nbr; 	     /* zone number */
	struct veh_attributes veh_attr;
	struct drv_attributes drv_attr;
}  ;
struct zone_stand_hold {
  long d_prcs_id;
  long u_prcs_id;
  char rec_type;
  char fleet;
  short zone_nbr;
  short veh_nbr;
  char  veh_tele[19];
  char query_id[32];
};
struct  veh_rqst  {
	long d_prcs_id;		/* Id of dispatch (for destination) */
	long u_prcs_id;		/* message type = users process ID */
	char rec_type;		/* application type code */
	char fleet;		/* Fleet Indicator  */
	short veh_nbr; 		/* vehicle ID */
}  ;
struct tc_rqst {
        long d_prcs_id;
	long u_prcs_id;
	char rec_type;
	char fleet;
	short zone_nbr;
	time_t due_date_time;
      } ;
typedef struct  tc_resp  {
	long u_prcs_id;		/* message type = requestors process ID */
	char rec_type; 		/* application type code */
	char fleet;		/* Fleet Indicator */
	short zone_nbr;		/* zone number */
	TIMES time_slots[UI_TCR_MAX_TIME_SLOTS + 1];
      } TC_RESP_REC;
typedef struct  zone_resp  {
	long u_prcs_id;		/* message type = requestors process ID */
	char rec_type; 		/* application type code */
	char fleet;		/* Fleet Indicator */
	short zone_nbr;		/* zone number */
	short unassn_calls; 	/* number of unassigned calls */
	short timeout_calls; 	/* number of time_out calls */
	short time_calls;
	short bkup_zones[BCK_IN_ZN_RESP][ITEMS_IN_ZN_RESP];/* first sixteen backup zones including
			      	zone number, number of calls, vehicles
			      	posted and enroute vehicles  */
	short veh_posted; 	/* number of vehicles posted in zone */
	short post_veh_ids[VEH_IN_ZN_RESP]; /* specific posted vehicle IDs */
	short veh_enroute; 	/* number of vehicles enroute in zone */
	short enr_veh_ids[VEH_IN_ZN_RESP]; 	/* specific enroute vehicle IDs */
} ZONE_RESP  ;
typedef struct  veh_drv_info_resp  {
	long u_prcs_id;		/* message type = requestors process ID */
	char rec_type;		/* application type code  */
	char fleet;		/* Fleet Indicator */
	short veh_nbr;		/* Vehicle number */
	int drv_id;  		/* Driver ID */
	char rf_id[5]; 		/* rf_id number */
	struct taxi_status cur_status; 	/* current driver status */
	int zone_num;		/* zone number vehicle is posted into */
	struct veh_attributes veh_attr;	/* Vehicle attributes code */
	struct drv_attributes drv_attr; /* Driver attributes code */
	time_t emerg_on_time; 	/* emergency on time */
	time_t emerg_off_time; 	/* emergency off time */
	short short_meter; 	/* number of short meters */
	short req_to_talk; 	/* number of times driver requested to talk */
	short late_meter; 	/* number of late meters */
	short messages;		/* number of messages sent to supervisors */
	time_t sign_on_dt_tm; 	/* Last sign on time */
	time_t sign_off_dt_tm; 	/* Last sign off time */
	short calls;		/* number of dispatched calls */
	short posts;		/* number of times driver posted */
	short no_accepts;	/* number of no_accepts */
	short callbacks; 	/* number of callbacks */
	short flags;		/* number of flags picked up by the driver */
	short cond_posts; 	/* number of conditional posts by the driver */
	short rejects;		/* number of rejects */
	short callouts; 	/* number of callouts */
	short assigns;		/* # of manual assigns for drivers last shift */
	short bids;		/* number of bids awarded to driver */
	short enroute_posts; 	/* number of times driver enroute posted */
	short breaks;		/* number of breaks taken by the driver */
	short no_shows; 	/* number of no_shows the driver had */
	int	missing_receipts;	/* number of missing itm receipts	*/
	double  gps_lat;
	double  gps_long;
} VEH_RESP_REC;
/*    The purpose of this structure is to provide a buffer used 
 *    for passing messages from call takers and supervisors to 
 *    dispatch through a named pipe to be sent to a driver. 
 */
struct drv_msg {
       long    u_prcs_id;      		/* Users process ID */
       char    rec_type;       		/* Record type  Driver msg*/
       int     msg_nbr;        		/* Message number */
       int     dispatch_flag;		/* Generate an exception if message
					   is not from a dispatcher */
       char    msg_flag;		/* MSG_LOG, MSG_RPT, MSG_UPD, MSG_DEL */
       
} ;
typedef struct emerg_bcast_rqst {
  long d_prcs_id;
  long u_prcs_id;
  char rec_type;
  char fleet;
  short veh_nbr;
  short radius;
  short nbr_taxis;
} EMERG_BCAST_RQST, EMERG_BCAST_RESP ;
typedef struct ilink_req {
  long u_prcs_id;
    char rec_type;
  char ilink_type[4];
    char  call_nbr[12];
} ILINK_RQST;
/*   The purpose of this structure is to provide a buffer for passing
 *   call related information necessary for matching calls and drivers
 *   through a named pipe between user interface and dispatch.
 */
#include "Call_private.h"
#define EMUL_POST 	1	/* emulate a driver posting */
#define EMUL_CPOST 	2	/* emulate a driver conditional posting */
#define EMUL_EPOST 	3	/* emulate a driver enroute posting */
#define EMUL_S_IN 	4	/* emulate a driver signing in posting */
#define EMUL_S_OUT 	5	/* emulate a driver signing out posting */
#define EMUL_RESET	6	/* reset the drivers status */
#define EMUL_DWNLD	7	/* download to kdt terminal */
#define EMUL_ACPT	8	/* accept a call offer */
#define EMUL_REJ	9	/* reject a call offer */
#define EMUL_NOSHOW	10	/* request a no show */
#define EMUL_FLAG	11      /* request a flag */
#define EMUL_PCKUP	12	/* meter on */
#define EMUL_CLOSE	13	/* meter off */
#define EMUL_MSG97      14
#define EMUL_MSG98      15
#define EMUL_MSG99      16
struct kdt_emulate {
       long    u_prcs_id;   	/* Users process id*/
       char    rec_type;      	/* Record type KDT */
       char    fleet_id;      	/* Fleet ID */
       short	veh_nbr;       	/* Vehicle number */
       short	action_code;   	/* Action code 1 - 7 */
       short	zone_nbr;      	/* Zone number */
       int	drv_id;       	/* Driver ID */
       short	priority;      	/* Priority */
};
/* SamPlan call types */
#define NOT_EXTERNAL -1  /* Trip is not a SamPlan EXTERNAL call */
struct sp_excpt_rec {
			long u_prcs_id;   /* Users process id (NOT USED, YET)*/
			char rec_type;    /* Record type SP_EXECPT_REQ */
			char cmsg_nbr;    /* Call message number to be used with NO_ACK exceptions */
			char cmsg_device; /* Call message device to be used with NO_ACK exceptions */
			int call_nbr;     /* Call number */
			int except_type;  /* Type of exception */
			int except_info;  /* Exception information */
};
struct sp_itm_msg {
			long u_prcs_id;   /* Users process id (NOT USED, YET)*/
			char rec_type;    /* Record type SP_EXECPT_REQ */
			char term_id[6];  /* Terminal ID the message is sent to */
			int itm_msg_type; /* Type of the ITM-message sent */
			char msg[256];    /* Message string */
};
struct sp_mdt_msg {
			long u_prcs_id;   /* Users process id (NOT USED, YET)*/
			char rec_type;    /* Record type SP_EXECPT_REQ */
			char term_id[6];  /* Terminal ID the message is sent to */
			char msg[256];    /* Message string */
};
#endif
