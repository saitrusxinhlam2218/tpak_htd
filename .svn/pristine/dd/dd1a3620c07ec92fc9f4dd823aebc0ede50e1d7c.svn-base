/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Vehicle_private.h,v $
*  @(#)  $Revision: 1.16 $
*  @(#)  $Date: 2004/06/22 17:11:18 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Vehicle_private.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Vehicle_private.h,v $	$Revision: 1.16 $"

#ifndef _VEHICLE_PRIVATE_
#define _VEHICLE_PRIVATE_

#ifndef __alpha
#include <sys/time.h>
#endif


#define  CANX_TYPE_CANCEL_KEY   1
#define  CANX_TYPE_NO_TRIP      2

/* structure defining an in memory vehicle record */
typedef struct veh_driv {
	short veh_nbr;  		/*  vehicle number */
	char fleet; 			/*  Fleet Indicator. */
	short fleet_nbr;		/* index of fleet */
	int driver_id; 			/* id of driver currently using cab */	
	char mid[MID_SIZE + 1];		/* 1-FFFF RF ID of the KDT  */
	unsigned long rf_id;
	LIST_HNDL timer_list;
	time_t event_time;				/* time of selected call event */
	short event;					/* state of selected call event */
	time_t drv_susp_from_date; 	/* Date this driver was last suspended. */
	time_t drv_susp_to_date; 	/* Date driver was last restored to service. */
	time_t veh_susp_from_date; 	/* Date this vehicle was last suspended. */
	time_t veh_susp_to_date; 	/* Date vehicle was last restored to service. */
	time_t drv_susp_d_from_date; 	/* Date this driver was last suspended. */
	time_t drv_susp_d_to_date; 	/* Date driver was last restored to service. */
	time_t veh_susp_d_from_date; 	/* Date this vehicle was last suspended. */
	time_t veh_susp_d_to_date; 	/* Date vehicle was last restored to service. */
	time_t drv_susp_x_from_date; 	/* Date this driver was last suspended. */
	time_t drv_susp_x_to_date; 	/* Date driver was last restored to service. */
	time_t veh_susp_x_from_date; 	/* Date this vehicle was last suspended. */
	time_t veh_susp_x_to_date; 	/* Date vehicle was last restored to service. */
	char driver_class; 		/* Driver classification.
				  	 Choices: 1 - Regular
					   	  2 - Rookie */
	struct drv_attributes driver_attr; /* flags   restrictions on type of service.
				  	 These flags correspond to entries
				  	 in the attribute file and define whether
				  	 the driver possesses the related attribute. */
	struct veh_attributes vehicle_attr; /* flags Define unique attributes of this 
				  	vehicle.  These flags correspond to entries
				  	in the attribute file and define whether
				  	the vehicle has the related attribute. */
	struct seals_flags vehicle_seals;  /* flags  Defines which seals this
				 	vehicle has.  
				  	Generally seals determine which cities or
				  	areas a vehicle may pickup passengers.  These
				  	flcorrespond to those in the zone file
				  	thus defining seal(s) required to pickup in
				  	each zone. */
	struct seals_flags now_seals;  /* flags  Define which seals the current
	           			driver in this vehicle possess */
	VEH_CALLS_LIST_HNDL             veh_calls_list_hndl;
	short zone_num;			/* zone # for posting and calls*/
	short epost_zone;		/* zone number for enroute posting */
	short held_epost_zone;		/* zone number for showing which zone is holding an epost call for backup zone cab*/
        char q_priority;                /* actual priority used to queue position  G7 new */
	char priority;  		/* priority of a driver for queue positioning */
	time_t time_info; 		/* time taxi posted to zone, accepted or metered on */
	time_t on_break_time; 		/* time of on break */
	time_t reject_time; 		/* time of rejecting a call */
	time_t c_post_time; 		/* time of conditional post */
	time_t e_post_time; 		/* time of enroute post */
	time_t inqry_time; 		/* time of inquiry (zone info) */
	time_t veh_info_time; 		/* time of vehicle info request */
	unsigned long q_time;           /* unique time veh in q - g7 new - not real time */
	struct calls *call_ptr; 	/* call ptr to offer or current fare */
	int call_nbr; 			/* call ptr to offer or current fare */
	short level_offered; 		/* level number offered at */
	struct taxi_status t_status; 	/* flags . posted, idle, enroute etc. */
	time_t emergency_on_date; 	/* Last date this driver invoked emergency. */
	time_t emergency_off_date; 	/* Date the last emergency was taken care of.*/
	time_t signed_on_date; 		/* Last date this driver was on the street. */
	time_t signed_off_date; 	/* Last date this driver became idle. */
	int last_call_handled; 		/* Last call handled by this driver. */
	short assigns; 			/* Number of calls manually assigned to this
				  	 driver on his/her last shift. */
	short calls; 			/* Number of dispatched calls. */
	short flags; 			/* Number of flags picked up on the street. */
	short bids; 			/* Number of bids awarded to this driver. */
	short posts; 			/* Number of times this driver posted. */
	short cond_posts; 		/* Number of times driver cond posted */
	short enroute_posts; 		/* # of times driver enroute posted. */
	short short_meters; 		/* Number of short meters. */
	short late_meters; 		/* Number of late meters. */
	short no_accepts; 		/* Number of no-accepts. */
	short rejects; 			/* Number of rejects. */
	short breaks; 			/* Number of breaks taken. */
	short req_to_talk; 		/* # of times driver requested to talk. */
	short messages; 		/* Number of messages sent to supervisors. */
	short callbacks; 		/* Number of callbacks. */
	short callouts; 		/* Number of callouts. */
	short no_shows; 		/* Number of no-shows. */
	short going_homes; 		/* Number of going homes. */
	short sign_ins;
	short sign_offs;
	short emergencies;

	time_t stat_start_tm;		/* start time of a statistics interval */	
	short tmp_assigns; 		/* Number of calls manually assigned to this
				  	 driver on his/her last shift. */
	short tmp_calls; 		/* Number of dispatched calls. */
	short tmp_flags; 		/* Number of flags picked up on the street. */
	short tmp_bids; 		/* Number of bids awarded to this driver. */
	short tmp_posts; 		/* Number of times this driver posted. */
	short tmp_cond_posts; 		/* Number of times driver cond posted */
	short tmp_enroute_posts; 	/* # of times driver enroute posted. */
	short tmp_short_meters; 	/* Number of short meters. */
	short tmp_late_meters; 		/* Number of late meters. */
	short tmp_no_accepts; 		/* Number of no-accepts. */
	short tmp_rejects; 		/* Number of rejects. */
	short tmp_breaks; 		/* Number of breaks taken. */
	short tmp_req_to_talk; 		/* # of times driver requested to talk. */
	short tmp_messages; 		/* Number of messages sent to supervisors. */
	short tmp_callbacks; 		/* Number of callbacks. */
	short tmp_callouts; 		/* Number of callouts. */
	short tmp_no_shows; 		/* Number of no-shows. */
	short tmp_going_homes; 		/* Number of going homes. */
	short tmp_sign_ins;
	short tmp_sign_offs;
	short tmp_emergencies;
	time_t warm_time;		/* special purpose field for warm standby */
	short	last_zone;		/* Last zone vehicle was booked into */
        short term_type;                /* what knd of terminal it got */
	short invoice_retry_count;
#ifdef NAMERICA
	int     radio_channel;
#endif
	char *veh_epost_timer;
	double   gps_lat;
	double   gps_long;
	time_t gps_time;
	short last_msg_nbr;
	char  host_link;
	short CanxType;
	time_t crse_canx_time;
	int   gps_state;
	char  gps_receiver_status;
	time_t last_action_time;  
  CALL_HNDL held_call;
  char  ip_address[ 24 ];
  char  link_type;
  char  nav_type;
  VEH_HTTP_LIST_HNDL http_list;
  BOOLEAN send_gps_not_in_zone;
  BOOLEAN roti;  // whether to extend the trip offer time for this taxi (remote offer trip...)
  short schedule_num;
  short kela_node;
} VEH; 


typedef struct  cisam_vh {
  char   fleet;                          /* Fleet indicator.          */
  char   kdt_working;                     /* "Y"/"N".  Whether the KDT- 440 terminal is functional in this vehicle.         */
  short  nbr;                            /* Vehicle number.           */
  char   mobile_rf_id[MID_SIZE + 1];     /* RF ID of the KDT terminal in this vehicle.          */
  char   license_nbr[11];                /* Vehicle license plate number.                   */
  char   make[11];                       /* Vehicle make.             */
  char   model[11];                      /* Vehicle model.            */
  char   seals[17];                      /* Defines which seals this vehicle has.  Generally seals determine which */
  /* cities or areas a vehicle may pickup passengers.  These flags correspond to */
  /* those in the zone file thus defining seal(s) required to pickup in each zone. */
  char   susp_pass_call;                  /* "Y"/"N". whether the vehicle is restricted from servicing passenger calls. */
  int    susp_from_dt_tm;                 /* Date & time this vehicle was last suspended.       */
  int    susp_to_dt_tm;                   /* Date & time this vehicle was last restored to   service.                  */
  char   susp_del_call;                  /* "Y"/"N". whether the vehicle is restricted from servicing delivery calls.   */
  char   susp_typex_call;                 /* "Y"/"N". whether the vehicle is restricted from servicing dial_a_ride calls.  */
  char   susp_reason[25];                 /* Reason for vehicles suspension (internal).         */
  char   susp_message[97];               /* Suspension message to be sent to the vehcile when a driver signs on while still */
  /* suspended.                */
  char   attr[65];                         /* 32 flags.  Define any unique attributes of this vehicle.  These flags */
  /* correspond to entries in the attribute file and define whether the vehicle */
  /* possesses the related attribute.               */
  char    meter_working;                   /* "Y"/"N".  Whether this vehicles meter is functional or not.  If not */
  /* functional, the driver may use the <PCKUP> and <CLOSE> functions for any */
  /* type of call.              */
  int     last_emerg_on;                    /* Last date & time this vehi invoked emergency.       */
  int     last_emerg_off;                   /* Date & time the last emergency was taken care of.                      */
  int     last_signed_on;                   /* Last date & time this vehic was on the street.       */
  int     last_signed_off;                  /* Last date & time this vehicle became idle.     */
  int     last_driver;                      /* Last driver that drove this vehicle.            */
  int     last_call;                        /* Last call handled in this vehicle.                 */
  short   assigns;                          /* Number of calls manually assigned to this vehicle on its last shift.       */
  short   calls;                            /* Number of dispatched calls.                   */
  short   flags;                            /* Number of flags picked up on the street.           */
  short   bids;                             /* Number of bids awarded to this vehicle.            */
  short   posts;                            /* Number of times this vehicle posted.          */
  short   cond_posts;                       /* Number of times this vehicle conditionally posted.                  */
  short   enroute_posts;                    /* Number of times this vehicle enroute posted.  */
  short   short_meters;                     /* Number of short meters.  */
  short   late_meters;                      /* Number of late meters.   */
  short   no_accepts;                       /* Number of no-accepts.    */
  short   reject;
  short   breaks;                           /* Number of breaks taken.  */
  short   req_to_talk;                      /* Number of times this vehic requested to talk.       */
  short   messages;                         /* Number of messages sent to supervisors.             */
  short   callbacks;                        /* Number of callbacks.     */
  short   callouts;                         /* Number of callouts.      */
  short   no_shows;                         /* Number of no-shows.      */
  short   fleet_nbr;                        /* Fleet number corresponding this vehicles fleet.     */
  char    susp_from_date[10];	
  char    susp_from_time[7];
  char    susp_to_date[10];
  char    susp_to_time[7];
  char    comments[81];                     /* Any comments relating to this vehicle.            */
  short   status;
  short   last_zone_posted;
  long    last_time_posted;
  short	 schedule_num;
  short	term_type;
  int		itm_last_receipt_nbr;
  int		itm_nbr_missing_receipts;
  int		itm_request_status;
  int		itm_receipt_nbr_first;
  int		itm_receipt_nbr_last;
  int		itm_receipt_nbr_current;
  int		itm_receipt_nbr_cnt;
  int		itm_retry_cnt;
  
  int    susp_d_from_dt_tm;  
  int    susp_d_to_dt_tm;    
  char    susp_d_from_date[10];	
  char    susp_d_from_time[7];
  char    susp_d_to_date[10];
  char    susp_d_to_time[7];
  int    susp_x_from_dt_tm;
  int    susp_x_to_dt_tm;   
  char    susp_x_from_date[10];	
  char    susp_x_from_time[7];
  char    susp_x_to_date[10];
  char    susp_x_to_time[7];
  char    color[11];
  char    telephone[17];
  char    host_link;
  double  gps_lat;
  double  gps_long;
  int     gps_state;
  char    mdt_tele[17];
} VEH_REC;

typedef struct _HTTP_HOLD {
  time_t  hold_time;
  char    query_id[32];
} HTTP_HOLD;

#endif
