/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Fleet_private.h,v $
*  @(#)  $Revision: 1.15 $
*  @(#)  $Date: 2004/06/22 17:11:18 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Fleet_private.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

#ifndef _FLEET_PRIVATE_H_
#define _FLEET_PRIVATE_H_

#include "Zone_public.h"
#include "Zone_private.h"

#define SCR_ATTR_MAX    16       /* Maximum number of attributes on scroll screens */
#define SELECT_DRIVER_ATTR 0
#define SELECT_VEHICLE_ATTR 1
#define ATTR_WIN_ROWS   18
#define ATTR_WIN_COLS   32
#define ATTR_SCR_ROW    3
#define ATTR_SCR_COL    20

struct bits {
	unsigned met_pass_call : 1; 	/*manual meter on allowed on passenger calls*/
	unsigned met_del_call : 1; 	/* manual meter on/off allowed on delivery calls*/
	unsigned met_typex_call : 1; 	/* manual meter on/off allowed on dial a ride */
	unsigned hold_line : 1;		/* Whether the hold line feature is enabled for this fleet. */
	unsigned suppress_seals : 1;	/* Whether to suppress seal checking or not.*/
	unsigned suppress_personal : 1;	/* Whether to suppress personal checking */
	unsigned ncp_on : 1;		/* ncp_on for this fleet */
	unsigned s8 : 1;
};

/* The MDT has three screens storing information about 30 zones each.
   The zone info has zone #, # of calls, and # of cabs. */
struct search_report
{		
  time_t search_time;		/* time of this search report */
  struct search_info
    {
      short num_in_search; 		/* number of zones in this search report */
      struct search_count
	{
	  short zone_num; 	/* zones in this search report */
	  short num_calls; 	/* calls in this zone */
	  short num_cabs; 	/* cabs in this zone */
	} count[ZONES_PER_MDT_SCREEN_MAX];
    } screen[MDT_ZONE_SCREEN_MAX];
};

typedef struct stand_bcast_report
{
  time_t  bcast_time;
  time_t  last_report_time;
  short zone_num;
  short  stand_bcast_type;  // 1 2 or 3 (one customer, couple of customers, many customers
} STAND_BCAST_REPORT;
#define MAX_STAND_BCAST_REPORT 10

/* information about what is happening in a given zone */
struct zone_summary  {
		short zone_num;		/* zone number */
		short no_calls;		/* no. of calls awaiting dispatch in this zone */
		short no_veh;		/* no. of vehicles posted in this zone */
		short no_enroute;	/* no. of vehicles enroute to this zone */
		short no_timecalls;     /* no. of timecalls in this zone  */
		short avg_time;		/* average time to dispatch */
		char time_outs;		/* are there any timeouts in this zone */
};

/* define a structure containing vehicle and driver attributes and descriptions */
struct vd_attr_struct {
	char veh_attr[3];	/* abbreviation for vehicle attribute */
	char veh_at_desc[21];	/* description for vehicle attribute */
	char drv_attr[3];	/* abbreviation for driver attribute */
	char drv_at_desc[21];	/* description for driver attribute */
	float veh_FV;
	short   veh_EV;
	float drv_FV;
	short drv_EV;
};

struct super_zones {		/* super zone information */
	short nbr;
	short perc_lead_tm;			/* percent of SUPERZONE lead time */
	short man_time_est;		/*  manual time estimate for super zone */
};


/* zone information for a given fleet */
#include "Zone_private.h"

typedef struct _FLEET_STATS_
{
  short signed_on;		/* numer of drivers who signed on */
  short signed_off;		/* numer of drivers who signed off */
  short calls; 			/* Number of dispatched calls. */
  short flags; 			/* Number of flags picked up on the street. */
  short bids; 			/* Number of bids offered  by the system. */
  short cond_posts; 		/* Number of times driver cond posted */
  short enroute_posts; 		/* Number of drivers  enroute posted */
  short short_meters; 		/* Number of short meters. */
  short late_meters; 		/* Number of late meters. */
  short no_accepts; 		/* Number of no-accepts. */
  short asigns; 			/* Number of manual assigns. */
  short rejects; 			/* Number of rejects. */
  short breaks; 			/* Number of breaks taken. */
  short req_to_talk; 		/* # of times driver requested to talk. */
  short messages; 		/* Number of messages sent to supervisors. */
  short callbacks; 		/* Number of callbacks. */
  short callouts; 		/* Number of callouts. */
  short no_shows; 		/* Number of no-shows. */
  short suspended;		/* Number of drivers suspended */
  short d_suspended;		/* Number of drivers suspended */
  short x_suspended;		/* Number of drivers suspended */
  short emergencies;		/* Number of emergencies */
  short wakeups;			/* number of wakeup calls */
  short cancels;                        // number of cancelled calls
  short now_signed_on;		/* numer of drivers who signed on */
  short now_signed_off;		/* numer of drivers who signed off */
  short now_in_fleet;		/* numer of drivers in the fleet */
  short now_calls; 		/* Number of dispatched calls. */
  short now_flags; 		/* Number of flags picked up on the street. */
  short now_bids; 		/* Number of bids offered  by the system. */
  short now_posts; 		/* Number of times driver posted */
  short now_enroute_posts; 	/* Number of drivers  enroute posted */
  short now_late_meters; 		/* Number of late meters. */
  short now_breaks; 		/* Number of breaks taken. */
  short now_req_to_talk; 		/* # of times driver requested to talk. */
  short now_messages; 		/* Number of messages sent to supervisors. */
  short now_callbacks; 		/* Number of callbacks. */
  short now_callouts; 		/* Number of callouts. */
  short now_no_shows; 		/* Number of no-shows. */
  short now_idle; 		/* Number of drivers idle */
  short now_suspended;		/* Number of drivers suspended */
  short now_emergencies;		/* Number of emergencies */
} FLEET_STATS;

/* fleet information */
typedef struct fleets {      	
	struct zones *zone_ptr[ZONE_MAX];	/* pointer to the zones being used by this fleet */	
	short call_stats[3][6]; 	/* addressed by defines  */
	struct search_report search_report; 	/* search report structure */
	char fleet_id;			/* Fleet Indicator.  */
	short fleet_nbr;		/* fleet number */
	char fleet_name[21];  		/* Fleet name, will appear on the greeting line if selected. */
	char active;			/* whether this fleet is active or not */
	struct bits criteria;		/* bits pertaining to matching criteria or indicating manual meter is allowed to be used */
	int zone_ct_ptr;		/* pointer to count of zone summary structures */
	long zone_update_time;		/* pointer to time at which zone summary structures  were updated in shared memory 
					 by dispatch */
	struct zone_summary zone_sumry[ZONE_MAX]; 	/* array of zone info */
	struct super_zones super_zone[MAX_SUP_ZONES];	/* super zone information */
	short wakeup_lead_time; 	/*Number of minutes before due time of wakeup calls that those calls are released. */ 
	char matching_algorithm; 	/* The way calls will be matched to cabs.    
				  	Choices: 1) Seq. by Zone, Temporal by Cab
					   	 2) Seq. by Zone, Cab Queue Order
					   	 3) Temporal by Zone Category */
	char drv_reg_pri; 		/* Priority given to normal drivers. */
	char drv_nshow_pri; 		/* Priority given to no-show drivers. */
	char drv_locl_pri; 		/* Priority assigned to drivers that just completed a local call. */
	char drv_cncl_pri; 		/* Priority assigned to drivers whose call has just been cancelled. */
	int local_calls_time; 		/* Amount of time from pickup to close that cannot be exceeded on a local call for it
				  	 to qualify as a local call. */
	char call_offer_procedure; 	/* How the driver responds to a call offer.
				  	 Choices: 1) Implied <ACPT> if on stand or in primary zone.
					   	  2) Implied <ACPT> regardless of zone.
					   	  3) Explicit <ACPT> required. */
	char no_accept_penalty; 	/* Penalty levied on drivers who reject a call by not pressing the <ACPT> key within
				  	 the maximum allowable time.
				  	 Choices: 1) None
					   	  2) Allowed, Bottom of Queue
					   	  3) Unpost and Penalize the Driver */
	char level_reject_penalty[LEVEL_MAX]; /* Reject Penalties - Fleetwide Defaults, Used if zone level reject values are not
						specified. 
       						Choices: 1) Not Allowed 
   	        				 	 2) Allowed, no Penalty
	        				 	 3) Allowed, Bottom of Queue */
	short level_exclusive_time[LEVEL_MAX]; /*  Exclusive Time - Fleetwide Defaults, Used if zone level exclusive times not 
						specified. */
	char bid_action; 			/* How the system puts calls up for bid.
				  			Choices: 1) Manual - supervisor approves 
				       	   			 2) Auto - no supervisor involved */
	char auto_flag_on_stands; 	/* How a flag will be handled on stands.
				  	 Choices: 1) None - Not allowed.
				 	   	  2) Semi - Needs supervisor approval.
					   	  3) Auto - may pickup flag, auto approval, auto reassign of call. */
	char no_show_action; 		/* What happens when a driver declares no-show.
					 Choices: 1) Manual - supervisor approves, driver posts.
					 2) Semi   - supervisor approves, automatically posted.
					 3) Auto   - cancel call, auto post.
					 4) Auto2  - cancel call, show sup, no post*/
	char cancel_action; 		/* What happens when a call taker cancels a call.
				  	Choices: 1) Manual - supervisor approves, driver posts.
					   	 2) Semi   - supervisor approves, automatically posted.
					   	 3) Auto   - cancel call, auto post.*/
	char e_post_action; 	/*  If a call comes into a zone w/enroute posted calls then hold it or not. */
	short zone_backup_level; 	/* 1 - LEVEL_MAX How many categories of zones to be considered when matching a call.
				  	 Choices: 1) Level 1 - stands only.
					  	  2) Level 2 - stands, primary and 1st level of backups
					   	  3-8) Adds one level of backup zones for each level chosen */
	short unassigned_time; 		/* Amount of time a call remains in dispatch before it times out and becomes an exception
				  	 for supervisor.  */
	char call_assignment_info; 	/* Call information presented to the driver when a call details are sent
                                         Choices: 0) Helsinki style
                                                  1) Stockholm style */
	char call_offer_info;   	/* Call information presented to the driver when a call is offered
				  	 Choices: 0) Zone only
				  	          1) Zone and zone description
                                                  2) as 1 plus map
					   	  3) as 1 plus 100 Block of Call
					   	  4) Complete Pickup Address */
	short veh_inquiry_freq; 	/*  seconds How often a driver may inquire about his own or another's vehicle's 
					 status.  */
	short zone_info_freq; 		/*  seconds How often a driver will get an updated set of information for the 
					 busiest zones */
	short zone_inqry_freq; 		/*  seconds How often a driver may inquire about zone (or up to 5 zones). */ 
					 
	short posting_freq; 		/*  secs time interval between posts */
	short cond_posting_freq; 	/* seconds How often a driver may conditionally post looking for calls.  */ 
	short call_accept_per; 		/*  seconds How much time is given to the driver to accept a call 
					 (i.e. to press the <ACPT> key).  */
	short break_duration; 		/*  minutes   How long of a time a driver may go on break and not lose his/her 
					 queue position  */
	short bid_duration; 		/*  seconds   How much time are the drivers given to bid on a call.   */
	struct calls *bid_ptr; 		/* pointer to call currently up for bid */
	short e_post_lead_time; 	/*   minutes   Amount of time given a driver from the time he/she enroute posts to 
			         	 the time he/she actually posts into the same zone.   */
	short short_meter_duration; 	/*  minutes If the time between a meter-on and a meter-off is shorter then this 
					 parameter the supervisor is informed.  */
	short late_meter; 		/*  minutes How soon the driver is considered to be late for customer pickup from the 
					 time of call acceptance.   */
	short max_reject_count; 	/*  Maximum number of times a call may be rejected before it becomes an exception. */
	short pers_lead_tm; 		/* amount of time a before due time an exception is created to show dispatcher that */
					/* a personal call for a driver is coming */
	char when_enroute_post; 	/* Choice: 1) Always
					   	   2) Only time after meter on
					   	   3) Only after meter on and a system assigned call */
	short no_accept_no_post_period;  /*  minutes  Amount of time following a no-accept the driver is disallowed to post, 
					  conditionally post or enroute post.  Only valid if drivers are penalized for no-accepts*/
	char max_nbr_veh_atr; 		/* Maximum number of vehicle attributes that a call taker may specify for a call. */ 
	char max_nbr_drv_atr; 		/* Maximum number of driver attributes that a call taker may specify for a call. */ 
	short zone_status_update_freq; /*  seconds  How often the zone status update screen is updated with new 
					information.  */
	char approve_msgs_to_drivers; 	/* What happens when a call taker sends a message to a driver.
				  	Choices: 1) Manual - supervisor approves
						 2) Auto   - message sent */
	char max_cabs_per_call; 	/* Maximum number of vehicles that may be specified by a call taker to service one
				  	 call.  */
	long future_call_maximum; 	/* How many seconds, in advance, without supervisor approval, a call taker may 
					 enter a future call.  */
	long future_call_minimum;	/* Seconds Amount of notice required by this fleet for  entry of a future call. */ 
	char pass_flat_rate_calc; 	/* How the flat rate is calculated for a passenger call.
				  	 Choices: 1) None   - no flat rate allowed.
					   	   2) Manual - operator enters.
					   	   3) Zone   - zone to zone lookup.
					   	   4) Zip    - zip to zip lookup. */
	char del_flat_rate_calc; 	/* How the flat rate is calculated for a delivery call.
				  	 Choices: 1) None   - no flat rate allowed.
					   	  2) Manual - operator enters.
					   	  3) Zone   - zone to zone lookup.
					   	  4) Zip    - zip to zip lookup. */
	char subsc_call_review;  	/* What happens to subscription calls before they are enabled.
				  	 Choices: 1) Manual - supervisor approves
					   	  2) Semi   - supervisor approves all holidays. */
	char greeting_line; 		/* flag Whether or not the greeting lines are to be displayed on the call entry screens. */
	char default_city[4]; 		/*  Default city code to be used if the user profile of this user does not have one. */
	char default_phone_area[4]; 	/* Default phone area code to be used if the user profile of this user does not have one. */
	char default_call_type; 	/* Default call type to be used if the user profile of this user does not have one. */
	char live_time_calls_freq; 	/* hours How often the live time calls report will be printed.  */
	char audit_trail_print; 	/* flag Whether the MADS system should print audit trail reports. */
	char audit_trail_prt_port[21]; /* Which port the audit trail printer is connected to for this fleet. */
	short passenger_priority_def; 	/* Default for passenger calls. */ 
	short delivery_priority_def; 	/* Default for delivery calls. */
	short charge_priority_def; 	/* Default for charge calls. */
	short time_calls_priority_def; /* Default for time calls. */
	short subsc_priority_def; 	/* Default for subscription calls. */
	short typex_priority_def; 	/* Default for dial a ride  calls. */
	short wake_up_priority_def; 	/* Default for wake up calls. */
	short highest_rookie_pri; 	/* Highest priority call that a rookie may handle. */

	short now_veh_attr[ATTR_MAX]; 	/* counts of vehicle attributes currently on the system */
	short fl_veh_attr[ATTR_MAX]; 	/* counts of vehicle attributes  in the fleet*/
	short now_driv_attr[ATTR_MAX]; 	/* counts of driver attributes currently on the system */
	struct vd_attr_struct vd_attr_info[ATTR_MAX];	/* veh/drv attribute codes and descriptions */

	short signed_on;		/* numer of drivers who signed on */
	short signed_off;		/* numer of drivers who signed off */
	short calls; 			/* Number of dispatched calls. */
	short flags; 			/* Number of flags picked up on the street. */
	short bids; 			/* Number of bids offered  by the system. */
	short cond_posts; 		/* Number of times driver cond posted */
	short enroute_posts; 		/* Number of drivers  enroute posted */
	short short_meters; 		/* Number of short meters. */
	short late_meters; 		/* Number of late meters. */
	short no_accepts; 		/* Number of no-accepts. */
	short asigns; 			/* Number of manual assigns. */
	short rejects; 			/* Number of rejects. */
	short breaks; 			/* Number of breaks taken. */
	short req_to_talk; 		/* # of times driver requested to talk. */
	short messages; 		/* Number of messages sent to supervisors. */
	short callbacks; 		/* Number of callbacks. */
	short callouts; 		/* Number of callouts. */
	short no_shows; 		/* Number of no-shows. */
	short suspended;		/* Number of drivers suspended */
	short d_suspended;		/* Number of drivers suspended */
	short x_suspended;		/* Number of drivers suspended */
	short emergencies;		/* Number of emergencies */
	short wakeups;			/* number of wakeup calls */

	short now_signed_on;		/* numer of drivers who signed on */
	short now_signed_off;		/* numer of drivers who signed off */
	short now_in_fleet;		/* numer of drivers in the fleet */
	short now_calls; 		/* Number of dispatched calls. */
	short now_flags; 		/* Number of flags picked up on the street. */
	short now_bids; 		/* Number of bids offered  by the system. */
	short now_posts; 		/* Number of times driver posted */
	short now_enroute_posts; 	/* Number of drivers  enroute posted */
	short now_late_meters; 		/* Number of late meters. */
	short now_breaks; 		/* Number of breaks taken. */
	short now_req_to_talk; 		/* # of times driver requested to talk. */
	short now_messages; 		/* Number of messages sent to supervisors. */
	short now_callbacks; 		/* Number of callbacks. */
	short now_callouts; 		/* Number of callouts. */
	short now_no_shows; 		/* Number of no-shows. */
	short now_idle; 		/* Number of drivers idle */
	short now_suspended;		/* Number of drivers suspended */
	short now_emergencies;		/* Number of emergencies */
	short now_wakeups;		/* number of wakeup calls */

	short time_call_freq; 		/* time interval between readings of the subscription file , in minutes*/
	short hist_srch;		/* look back number of minutes for history search */
	short time_call_max;		/* look ahead maximum minutes */

	char estimate_text[RANGE_MAX][EST_LEN];	/* ranges for pickup times */
	char def_est_text[EST_LEN];		/* ranges for pickup times */
	int range_limit[RANGE_MAX];	/* maximum time to be in this range */

        short missing_receipts_timeout;  /* time out period in min after request for exception */
        short missing_receipt_retry_cnt; /* how many time we will .... */
							/* TEK below */
        short   inactive_retry_interval;                /* Check every "inactive_retry_interval" minutes */
        short   inactive_time_out_period;               /* Log vehicles out after "inactive_time_out_period" hours */
        char    approach_tariff;                        /* how to handel "approach tariff" 1) No approach tariff
                                                           2) Meter on before pickup 3) Reserved
                                                        */
        short   num_levels_to_compute_stats;            /* number of backup levels to compute pickup time estimation (1 - 8) */
        short   stats_compute_freq;                     /* Frequency to compute pickup time estimates (in minutes) */
        short   handle_system_assigned_calls;           /* How to handle system calls 1) Force  acceptance of  system assignments
                                                           2) Ignore system assignments
                                                           3) RESERVED
                                                        */
	short	itm_missing_receipts_max;		/* Max number of missing receipts to request from itm. This is to prevent
							   asking for way too many receipts if there is corrupted data sent
							   from itm. */
	char	nak_offer_penal;

	short auto_flag_req_time;	/* The lead time for allowing a flag request to be accepted
					If the flag request is within this many minutes of the due time allow flag request*/
	short	time_int_req;
	short   time_calls_lead_supp;
	char    FR_distance_filter;
#ifdef NAMERICA
	char    send_dest_addr;
	short   unassgnd_look_ahead;
	char    send_dest_offer;
	char    send_pckup_cmt;
	char    send_rate;
#endif
	FLEET_VEH_NBR_LIST_HNDL fleet_veh_nbr_list_hndl;
        FLEET_DRIVER_VEH_LIST_HNDL fleet_driver_veh_list_hndl;
        FLEET_ZONE_CALLS_LIST_HNDL fleet_zone_calls_list_hndl;
	FLEET_EXCPT_LIST_HNDL      fleet_excpt_list_hndl;
	FLEET_SUSP_LIST_HNDL       veh_susp_pass_list_hndl;
	FLEET_SUSP_LIST_HNDL       veh_susp_deliv_list_hndl;
	FLEET_SUSP_LIST_HNDL       veh_susp_typex_list_hndl;
	FLEET_SUSP_LIST_HNDL       drv_susp_pass_list_hndl;
	FLEET_SUSP_LIST_HNDL       drv_susp_deliv_list_hndl;
	FLEET_SUSP_LIST_HNDL       drv_susp_typex_list_hndl;
        FLEET_BIG_CAR_TRIP_LIST_HNDL big_car_trip_list_hndl;
	char    gps_state;
	short   gps_time;
	int     gps_distance_ceiling;
	char    send_call_type;
	short   auto_reject_time;
	short   m_action_timer;
	short   m_action_retry;
	short   m_action_penalty;
	short   canx_to_moff_time;
	double  gps_max_x;
	double  gps_min_x;
	double  gps_max_y;
	double  gps_min_y;
	short   gps_coord_sys;
	char    gps_book;
    short tc_restrict_interval;
    short cancels;                        // number of cancelled calls
    short ns_short;  // short no_show determinant    
  short emerg_range;
  char  enroute_break;
  char  ilink_active;
  short ilink_timeout;
  short  cbook_time;
  short  stand_ntfy_interval;
  short  stand_ntfy_life;
  STAND_BCAST_REPORT bcast_report[MAX_STAND_BCAST_REPORT];   // max 10 stands to report at a time
} FLEET;

typedef struct  cisam_fl  {
        char     id;                       /* Fleet indicator.             */
        char    match_algorithm;           /* The way calls will be match the cabs.  Choices: */
                                           /*   1) Seq. by Zone, Temporal by Cab */
                                           /*   2) Seq. by Zone, Cab Queue Order */
                                           /*   3) Temporal by Zone Category     */
        short    nbr;                      /* 0-7.  Assigned by the system, non-modifiable.  Used for internal cross- reference. */
        char     name[21];                 /* Fleet name, will appear on the greeting line if select  */
        char   drv_regular_pri;            /* 1-9.  Priority assigned to normal drivers.              */
        char   drv_no_show_pri;            /* 1-9.  Priority assigned to no-show drivers.             */
        char   drv_local_pri;              /*  1-9.  Priority assigned to drivers that just completed a local call.     */
        char   drv_cancel_pri;             /* 1-9.  Priority assigned to drivers whose call has just been cancelled.         */
        short  local_calls_dur;            /* Amount of time from pickup to close that cannot be exceeded on a local call */
                                           /* for it to qualify as a local call.                  */
        char  call_offer_proc;             /* How the driver responds to a call offer. Choices: */
                                           /*   1) Implied <ACPT> if on stand or in primary zone. */
                                           /*   2) Implied <ACPT> regardless of zone. */
                                           /*   3) Explicit <ACPT> required. */
       char  no_acpt_penalt;               /* Penalty levied on drivers who reject a call by not pressing the <ACPT> key */
                                           /* within the maximum allowable time.  Choices: */
                                           /*    1) None */
                                           /*    2) Allowed, Bottom of queue */
                                           /*    3) Unpost and Penalize the Driver */

/* Reject Penalties - Fleetwide Defaults, Used if zone level reject values are not specified.  */
/* Choices: 1) Not Allowed */
/*          2) Allowed, no Penalty */
/*          3) Allowed, Bottom of Queue */

      char   lev1_rej_penalt;               /* Default for all zones on level 1.                 */
      char   main_rej_penalt;               /* Default for primary zone.   */
      char   lev2_rej_penalt;               /* Default for all zones on level 2.                    */
      char   lev3_rej_penalt;               /* Default for all zones on level 3.                    */
      char   lev4_rej_penalt;               /* Default for all zones on level 4.                    */
      char   lev5_rej_penalt;               /* Default for all zones on level 5.                    */
      char   lev6_rej_penalt;               /* Default for all zones on level 6.                    */
      char   lev7_rej_penalt;               /* Default for all zones on level 7.                    */
      char   lev8_rej_penalt;               /* Default for all zones on level 8.                    */

/* Exclusive Time - Fleetwide Defaults, Used if zone level exclusive time values not specified.  */

      char   bid_action;                    /* How the system puts calls up for bid.  Choices: */
                                            /*    1) Manual - supervisor approves */
                                            /*    2) Auto   - no supervisor involved */
      short  lev1_excl_time;                /* Default for all zones on level 1.                    */
      short  main_excl_time;                /* Default for primary zone.   */
      short  lev2_excl_time;                /* Default for all zones on level 2.                    */
      short  lev3_excl_time;                /* Default for all zones on level 3.                    */
      short  lev4_excl_time;                /* Default for all zones on level 4.                    */
      short  lev5_excl_time;                /* Default for all zones on level 5.                    */
      short  lev6_excl_time;                /* Default for all zones on level 6.                    */
      short  lev7_excl_time;                /* Default for all zones on level 7.                    */
      short  lev8_excl_time;                /* Default for all zones on level 8.                    */
     char    no_personal;                   /* "Y"/"N".  Whether to allow or not allow entry of personal cab requests.   */
     char    no_seals;                      /* "Y"/"N".  Whether to suppress seals checking or not.                        */
    char     auto_stand_flag;               /* How a flag will be handled on stands.  Choices: */
                                            /*    1) None - Not allowed. */
                                            /*    2) Semi - Needs supervisor approval. */
                                            /*    3) Auto - may pickup flag, auto approval, auto reassign of call.          */
    char   no_show_action;                 /* What happens when a driver declares no-show. Choices: */
                                           /*    1) Manual -supervisor approves, driver posts. */
                                           /*    2) Semi   -supervisor approves, automatically posted. */
                                           /*    3) Auto   - cancel call, auto post. */
                                           /*    4) Auto 2 - cancel call, show supervisor (he may clear), no automatic post.*/
    char   cancel_action;                  /* What happens when a call taker cancels a call.  Choices: */
                                           /*    1) Manual -supervisor approves, driver posts. */
                                           /*    2) Semi   -supervisor approves, automatically posted. */
                                           /*    3) Auto   - cancel call, auto post.                */
   char   enr_post_action;                  /*"Y"/"N".  Should the system hold calls on the primary level if there are */
                                            /*   any cabs that are enroute posted into the zone.        */
   char  hold_line_flag;                    /* "Y"/"N".  Whether the hold line feature is enabled for this fleet.             */
   char  zone_backup_lev;                   /* 1-8.  How many categories of zones to be considered when matching a call. Choices: */
                                            /*    1) Level 1 -stands only. */
                                            /*    2) Level 2 -stands, primary and 1st level of backups */
                                            /*    3-8) Adds one level of backup zone for each level chosen.                 */
   short avl_call_q_time;                   /* Amount of time a call remains in dispatch before it times out and becomes */
                                            /* an exception for supervisor. (Default -5).    */
   short  veh_inqry_freq;                   /* How often a driver may inquire about his own or another's vehicle's */
                                            /*    status.  (Default - 30)     */
   char cal_assign_info;      	            /* Call information presented to the driver when a call details are sent
                                                  Choices: 0) Helsinki style
                                                           1) Stockholm style */
   char cal_offer_info;   	            /* Call information presented to the driver when a call is offered
				  	          Choices: 0) Zone only
				  	                   1) Zone and zone description
                                                           2) as 1 plus map
					   	           3) as 1 plus 100 Block of Call
					   	           4) Complete Pickup Address */
   short  zone_info_freq;                   /* Seconds.  How often a driver will receive an updated set of information */
                                            /*    for the busiest zones.  (Default -30)               */
   short zone_inqry_freq;                   /* Seconds.  How often a driver may inquire about zone (up to 5 zones).(Default -30)*/
   short  post_freq;                        /* Seconds.  How often a driver may post him/her self into a zone. (Default -180) */
   short  cond_post_freq;                   /* Seconds.  How often driver may conditionally post looking for calls.(Default -120) */
   short  call_accept_per;                  /* Seconds.  How much time is given to the driver to accept a call (i.e. to */
                                            /* press the <ACPT> key).  (Default -30) */
  short   break_duration;                   /* Minutes.  How long of a time a driver may go on break and not lose his/her */
                                            /*    queue position.  (Default - 10)  */
  short   bid_duration;                     /* Seconds.  How much time are the drivers given to bid on a call.  (Default -15) */
  short  enr_post_lead;                     /* Minutes.  Amount of time given a driver from the time he/she enroute posts */
                                            /* to the time he/she actually posts into the same zone.  (Default - 8)   */
  short  short_meter_dur;                   /* Minutes.  If the time between a meter-on and a meter-off is shorter then */
                                            /* this parameter the supervisor is informed.  (Default -2)                */
 short  late_meter;                         /* Minutes.  How soon the driver is considered to be late for customer pickup */
                                            /* from the time of call acceptance.  (Default -6)   */
 short  max_rej_count;                      /* 1-999.  Maximum number of times a call may be rejected before it becomes */
                                            /*    an exception. (Default -3)  */
 short  pers_lead_tm;  			    /* lead time to personal calls */
 short  no_acpt_penalty;                    /* Minutes.  Amount of time following a no-accept the driver is disallowed to */
                                            /* post, conditionally post or enroute post.  Only valid if drivers are */
                                            /* penalized for no-accepts.  (Default - 10)               */
 char   when_enr_post;                      
                                            
                                            
                                            
  short zone_stat_freq;                    
  short max_nbr_drv_atr;                   /* Max number of driver attrs that call taker may specify for a call.  (Default - 2)  */
  char  drv_msg_approve;                   /* What happens when a call taker sends a message to a driver.  Choices: */
                                           /*   1) Manual -supervisor approves */
                                           /*   2) Auto   - message sent    */
   short max_vehs_call;                     /* 1-9.  Maximum number of vehicles that may be specified by a call taker */
                                            /* to service one call.  (Default - 4)               */
  float  future_call_max;                   /*  How many days, in advance, without supervisor approval, a call taker may */
                                            /*  enter a future call.  Multiples of 24 hours.  (Default - 14)             */
  short	 future_call_min;                   /* Minutes.  Notice required by this fleet for entry of future call.  (Default -45) */
  char   pass_rate_calc;                    /* How the flat rate is calculated for a passenger call.  Choices: */
                                            /*   1) None   - no flat rate allowed. */
                                            /*   2) Manual -operator enters. */
                                            /*   3) Zone   - zone to zone lookup. */
                                            /*   4) Zip    - zip to zip lookup.                  */
  char  dlv_rate_calc;                      /* How the flat rate is calculated for a delivery call.  Choices:  */
                                            /*   1) None   - no flat rate allowed. */
                                            /*   2) Manual -operator enters. */
                                            /*   3) Zone   - zone to zone lookup. */
                                            /*   4) Zip    - zip to zip lookup.                 */
  char subsc_review;                        /* what happens to subscription calls before they are enabled.  Choices: */
                                            /*    1) Manual -supervisor approves */
                                            /*    2) Semi   -supervisor approves all holidays.  */
  char greeting_line;                       /* "Y"/"N".  Whether or not the greeting lines are to be displayed on the call */
                                            /* entry screens.             */
  char  city_default[4];                    /* Default city code to be used if the user profile of this user does not have one. */
  char  phone_area_def[4];                  /* Default area code to be used if the user profile of this user does not have one. */
  short time_calls_freq;                    /* Hours.  How often the live time calls report will be printed.  (Default - 2)    */
  short msg_rpt_freq;                       /* Hours.  How often the message report will be printed.  (Default - 2)    */
  char  audt_trail_port[21];                /* Which port the audit trail printer is connected to for this fleet.           */
   char  comments[41];                        /* Comments.                 */
   short pass_pri_def;                        /* 5-62.  Default priority for passenger calls.      */
   short del_pri_def;                         /* 5-62.  Default priority for delivery calls.       */
   short charge_pri_def;                      /* 5-62.  Default priority for charge calls.         */
   short calls_pri_def;                       /* 5-62.  Default priority for time calls.           */
   short subsc_pri_def;                       /* 5-62.  Default priority for subscription calls.   */
   short typex_pri_def;                
   short wake_up_pri_def;
   short high_rookie_pri;                     /* 1-62.  Highest priority call that a rookie may handle.                   */
   char  call_type_def;                      /* Default call type to be used if the user profile of this user does not have one. */
   char  met_pass_call;                     /* "Y"/"N".  Whether a manual meter on/off is allowed on passenger calls.          */
   char  met_del_call;                      /* "Y"/"N".  Whether a manual meter on/off is allowed on delivery calls.           */
   char  met_typex_call;                       
   short hist_search_def;                     /* Default for historical call searches.            */
   short time_call_freq;                       /* Minutes. how often should the system check whether any time calls (future or */
                                               /*    subscription) had reached its zone configurable pick up lead time.   */
   short time_call_max;                        /* Minutes. how far into the future should we go to select above time calls. */
   short range_limit1;			/* Minutes. Upper limit of the range used to determine the estimate */
					/* text to appear on the calltaker terminal after call is entered.		*/
	char	estimate_text1[8]; 	/* Text corresponding to previous field's range only the range needs to be specified */
					/* (e.g."10-15"); UI will append "Minutes" when displaying on the screen. */
	short	range_limit2;		 
	char	estimate_text2[8]; 	
	short	range_limit3;		 
	char	estimate_text3[8]; 	
	short	range_limit4;		 
	char	estimate_text4[8]; 	
	short	range_limit5;		 
	char	estimate_text5[8]; 	
	short	range_limit6;		 
	char	estimate_text6[8]; 	
	short	range_limit7;		 
	char	estimate_text7[8]; 	
	short	range_limit8;		 
	char	estimate_text8[8]; 	
	char	def_est_text[8];	/* Default estimate text if the average arrived at does not fall into any specified  */
					/* range or if all ranges are zero	*/			 
	short   max_nbr_veh_atr;        /* Max number of veh attrs that a call taker may specify for a call. (Default - 2)  */
	char 	active;
     	short   wakeup_lead_time;        /* Number of minutes before due time of wake up calls that those calls are released. */
	short	missing_receipt_timeout;	/* for ITM support */
	short	restricted_time_call_enabled;
	short	time_call_time_period_len;	/* for restrict time calls */
	short	missing_receipt_retries;	
	short	inactive_retry_interval;		/* Check every "inactive_retry_interval" minutes */
	short	inactive_time_out_period;		/* Log vehicles out after "inactive_time_out_period" hours */	
	char	approach_tariff;			/* how to handel "approach tariff" 1) No approach tariff
							   2) Meter on before pickup 3) Reserved
							*/
	short	num_levels_to_compute_stats;		/* number of backup levels to compute pickup time estimation (1 - 8) */
	short	stats_compute_freq;			/* Frequency to compute pickup time estimates (in minutes) */
	short	handle_system_assigned_calls;		/* How to handle system calls 1) force acceptance of system assignments 
							   2) Ignore system assignments
							   3) RESERVED
							*/
	short	itm_missing_receipts_max;		/* Max number of missing receipts to request from itm. This is to prevent
							   asking for way too many receipts if there is corrupted data sent
							   from itm. */
	char	nak_offer_penal;

	short auto_flag_time;	/* The lead time for allowing a flag request to be accepted
					If the flag request is within this many minutes of the due time allow flag request*/
	short   time_int_req;
	short   time_calls_lead_supp;
	char    FR_distance_filter;
#ifdef NAMERICA
	char    send_dest_addr;
	short   unassgnd_look_ahead;
	char    send_dest_offer;
	char    send_pckup_cmt;
	char    send_rate;
#endif
	char    gps_state;
	short     gps_time;
	int     gps_distance_ceiling;
	char    send_call_type;
	short   auto_reject_time;
	short   m_action_timer;
	short   m_action_retry;
	short   m_action_penalty;
	short   canx_to_moff_time;
	double  gps_max_x;
	double  gps_min_x;
	double  gps_max_y;
	double  gps_min_y;
	short   gps_coord_sys;
	char    gps_book;

        short   tc_restrict_interval;
    short ns_short;  // short no_show determinant
  short emerg_range;
  char  enroute_break;
  char  ilink_active;
  short ilink_timeout;
  char  load_unload;
  short cbook_time;
  short stand_ntfy_interval;
  short stand_ntfy_life;
}FLEET_REC;

#endif
