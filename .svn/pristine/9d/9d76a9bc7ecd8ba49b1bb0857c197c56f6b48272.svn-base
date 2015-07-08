/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call_private.h,v $
*  @(#)  $Revision: 1.12 $
*  @(#)  $Date: 2003/01/28 18:16:15 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Call_private.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

#ifndef _CALL_PRIVATE_H_
#define _CALL_PRIVATE_H_

#include "Zone_public.h"

#define CALL_BID_STATUS    200

#define CALLS_MAX       4000
#define TIME_CALLS_MAX  8000

#define CHRG_NOW 	0	/* charge now call */
#define CHRG_TDY 	1	/* charge today call */
#define TM_NOW   	2	/* time now calls */
#define TM_TDY   	3	/* time today calls */
#define SUBSCR_NOW 	4	/* subscription calls now */
#define SUBSCR_TDY 	5	/* subscription call today */

static BOOLEAN Call_is_open;


typedef struct cisam_cl
{
  int   nbr;                          /* System assigned call number */
  int   grouped_with;                 /* Call number of the call this call is grouped with. */
  char  pos_in_group;                 /* Calls posistioning if multiple taxis required.             */
  char  nbr_in_group;                 /* Number of calls grouped together for multiple cabs. */
  char  fleet;                        /* Fleet indicator.            */
  char  extended_type[6];             /* A combination of call types that applies to this call as it is to be */
  /*    displayed on the multi- line screen (E.g. D$ - for charge delivery).  */
  char  pckup_adr_cmnt[31];           /* Pickup address comment from the geo file.          */
  char  general_cmnt[65];            /* General comment area.       */
  int   pckup_str_nbr;                /* A six digit street number.     */
  char  pckup_str_suf;              /* Alpha street number suffix.      */
  char  pckup_pre_dir[3];             /* Pickup address pre- direction (E.g. "W" in 120 W. Main St.).               */
  char  pckup_str_name[21];           /* Pickup street name.            */
  char  pckup_str_key[5];            /* FIrst 4 chars of the pickup street name for key purpose            */ 
  char  pckup_str_type[4];           /* E.G "Ave" in 120 W. North Ave.  */
  char  pckup_post_dir[3];            /* E.g. "NW" in 120 29th St.NW */
  char  intr_pre_dir[3];              /* Intersection street pre- direction.                  */
  char  intr_str_name[21];            /* Intersection street name.   */
  char  intr_str_type[4];             /* Intersection street type.   */
  char  intr_post_dir[3];             /* Intersection post direction */
  char  pckup_apt_room[6];            /* Pickup apartment or room number.                     */
  char  pckup_city[4];                /* Abbreviation of the pickup city.                       */
  short pckup_zone;                   /* 1-999.  User entered or computer generated pickup zone number.                */
  char   passenger_name[21];          /* Name of passenger.             */
  char   phone_area[4];                /* Telephone area code.        */
  char   phone_nbr[8];                 /* Telephone number.           */
  char   phone_ext[6];                 /* Telephone extension.        */
  char   dest_adr_cmnt[31];            /* Destination address comment from the geo file.  */
  int    dest_str_nbr;                 /* A six digit street number.  */
  char   dest_str_suf;               /* Alpha street number suffix. */
  char   dest_pre_dir[3];              /* Destination address pre- direction (E.g. "W" in 120 W. Main St.).               */
  char   dest_str_name[21];            /* Destination street name.    */
  char   dest_str_type[4];             /* E.g. "Ave" in 120 W. North Ave.                        */
  char   dest_post_dir[3];             /* E.g. "NW" in 120 29th St.NW */
  char   dest_apt_room[6];             /* Destination apartment or room number.                */
  char   dest_city[4];                 /* Abbreviation of the destination city.           */
  short  dest_zone;                   /* 1-999.  User entered or computer generated destination zone number.    */
  short   personal_veh;             /* 1-9999.  Vehicle number of the vehicle specifically requested by the customer.  */
  char    personal_rqst;                /* Defines whether "O"nly the vehicle specified may pickup the customer or the */
  /*    customer would like to use that vehicle "I"f it is available, otherwise another */
  /*    vehicle is acceptble.       */
  char    veh_attr_flag;                /* "Y"/"N".  Whether a vehicle or vehicles with specific characteristics */
  /*    are required to carry out this call.                  */
  char     veh_attr[65];                 /* 32 flags.  Defines which vehicle attributes are required to carry out this call. */
  char     drv_attr_flag;               /* "Y"/"N".  Whether a driver or drivers with specific characteristics are required */
  /*    to carry out this call.     */
  char    drv_attr[65];                  /* 32 flags.  Defines which driver attributes are required to carry out this call.     */
  char    nbr_of_vehs;                  /* Number of vehicles needed to service call.            */
  short   priority;                     /* Call priority (See FLEET file).                      */
  short   pri_status;                   /* Call priority field used in keyed searches.  untitl th call  is */
  /* assigned this field will contain the same value as the cl_priority */
  /* field. as soon as the call is accepted by a driver, this field */
  /* is set to 64(assignend call).  */
  char    hold_line[5];                 /* The extension number of the line the customer is holding*/
  char    local;                        /* "Y"/"N".  Whether this call is a local run or not. */
  char    charge_nbr[19];               /* Customer account number or charge card.                */
  char    requested_by[21];             /* Name of person requesting service.                    */
  char    person_to_see[21];            /* Person to see at the destination address.        */
  char   pod_required;                 /* "Y"/"N".  Whether proof of delivery is required.       */
  char    pod_name[21];                 /* Name of person signing for delivery.                   */
  char   date_in[10];                       /* Date this call was entered.  display  format.  */
  char  time_in[7];                       /* Time this call was entered.  disp format.      call is due.  "C" format.   */
  short   pod_time;                    /* Time the person signed for delivery.                   */
  float  call_rate;                     /* Rate, from the rate table, given to customer.          */
  int    date_time_in;                  /* Date and time this call was entered.  "C" format.   */
  char   est_wait_time[8];              /* Estimated waiting time given to the customer.      */
  char   due_date[10];                  /* Date this call is due.  display format.             */
  short  entered_by;                    /* User ID of person who entered this call.          */
  short  last_changed_by;               /* User ID of the last person who changed this subscription call.          */
  int    due_date_time;                 /* The date and time this         */
  int    dispd_date_time;               /* Date and time this  call was dispatched. "C" format  */
  char   due_time[7];                      /* Time this call is due.  disp format.             */
  char   dispd_date[10];                 /* Date this call was dispatched. display  format */
  char   dispd_time[7];                    /* Time this call was dispatched.  disp format.   */
  char   pckup_date[10];                 /*  Date the customer was picked up. display  format.*/
  char   pckup_time[7];                    /* Time the customer was picked up.  disp format.    */
  char  company_name[26];                /* Company name if a charge account call.                         */
  int    pckup_date_time;               /* Date and time the customer was picked up.  "C" format. */
  int    close_date_time;               /* Date and time the customer was dropped off. "C" format.*/
  char   close_date[10];                 /* Date the customer was dropped off. display format */
  char   close_time[7];                /* Time the customer was dropped off. disp format.   */
  char  approve_code[11];               /* Credit service approval code*/
  float  fare_amount;                   /* The dollar amount of the fare charged.               */
  int    subsc_call_nbr;                /* If this call is a subscription call then the subscription number is place */
  /*    in this field.(from sc_nbr) */
  short  veh_nbr;                       /* Vehicle that serviced this call, if more then one refer to history.           */
  int  drv_id;                        /* Driver that services this call, if more then one refer to history.           */
  short  dispatched_by;                 /* -1=dispatched by system.1-9999 ID of person who manually dispatched this call */
  int  expire_date;                 /* Credit card expiratio date. */
  int   approve_date;                   /* Approval date.              */
  short approve_by;                    /* call taker number that approved this charge.       */
  char  ac_addt1_info1[26];             /* Information entered in respons to the prompt from the account file (ac_header_3). */
  char  ac_addt1_info2[26];             /* Information entered on respons to the prompt from the account file (ac_header_4).  */
  char   driver_msg[37];
  char   pckup_pmap[9];
  char   dest_dmap[9];
  char  status_bits[5];                  /* 32 flags. used by the user interface process to tag future and pending subsc */
  /* calls with any call entry problems. corresponds to the status bits being sent */
  /* with a NOW call through the named pipe. used by the TIME CALL */
  /* PROCESS for inclusion in the pipe info.                 */
  char   zone_desc[21];
  char   screen_type;
  char   status[9];
  char   pass_name_key[5];
  int    dispatch_entry_tm;			/* Time the call entered dispatch program (in C format)	*/	
  char   phone_number[19];          /* internationalization phone number */
  char  advise_arrival;
  int   ext_nbr;                      /* FTJ number used by SamPlan */
  char  offer_type;                   /* Type of trip offer to be used with SamPlan trip */
  char  detail_type;                  /* Type of trip details to be used with SamPlan trip */
  char  call_msg_nbr;                 /* Number of text messages associated with this SamPlan trip */
  char   invoice_nbr1[9];
  char   invoice_nbr2[9];
  char   invoice_nbr3[9];
  char   invoice_nbr4[9];
  char   ct_adr_cmnt[31];
  short  via_addresses;
  float  vat;
#ifdef NAMERICA
  char send_destination;
  char send_rate;
#endif
  double  gps_lat;
  double  gps_long;
  char cash_account;
  double dest_gpsx;
  double dest_gpsy;
} CALL_REC;

typedef enum { PREMIUM_INSHIFT, PREMIUM_OUTSHIFT, OPEN_INSHIFT, OPEN_OUTSHIFT, NON_PREMIUM, NON_PREMIUM_OUTSHIFT } PREMIUM_SHIFT_STATE;

typedef struct calls
{
  struct calls *next;  
  CALL_REJECT_VEH_LIST_HNDL	call_reject_veh_list;
  CALLS_EXCPT_LIST_HNDL         call_except_list_hndl;
  CALL_MULT_LIST_HNDL           call_mult_list_hndl;     /* lead call in multiple list maintains list */
  CALL_MSG_LIST_HNDL            call_msg_list_hndl;
  short call_nbr_reject;		                 /* nbr of rejects */
  int c_isam_num; 		/* record number in c-isam file */
  int call_number; 		/* System assigned call number. */
  int     msg_nbr;      		/* Message number for call to supervisor*/
  struct veh_driv *veh_ptr;	/* pointer to vehicle handling this call */
  short curr_level; 		/* level the call is currently at in 
				   the matching algorithm */
  short fleet; 			/* Fleet Indicator. */
  struct except_bits status;  	/* current status of a call */
  struct call_types call_type;
  LIST_HNDL timer_list;
  short pickup_zone; 		/* User entered or computer 
				   generated pickup zone number. */
  short dest_zone; 		/* User entered or computer 
				   generated destination zone number. */
  short personal_vehicle; 	/* Vehicle number of the vehicle 
				   specifically requested by customer. */
  char personal_request;  	/* Defines whether ONLY the vehicle 
				   specified may pickup the customer or 
				   the customer would like to use that 
				   vehicle IF it is available,  otherwise 
				   another vehicle is acceptable. */
  struct veh_attributes vehicle_attributes; 	/* Defines which vehicle attributes are required to carry out 
						   this call.  */
  struct drv_attributes driver_attributes; 	/* flags which driver attributes are 
						   required to carry out this call.  */
  short number_of_vehicles; 	/* # of vehicles needed to service call. */
  short  priority; 		/* Call priority  */
  time_t event_time;				/* time of selected call event */
  short event;					/* state of selected call event */
  time_t time_info; 		/* time  offered if offered status bit set, 
				   time dispatched if status accepted */
  time_t time_entered; 		/*  Time this call was entered. */
  time_t due_time; 		/* The time this call is due. */
  time_t accept_time;
  time_t tc_activate_time;                /* activation time of time_call */
  int grouped_with;		/* pointer to the next call in the group */
  struct excepts *except_ptr;			/* pointer to an exception */
  char pickup_pmap[9];
  char advise_arrival;
  int FTJ_nbr; /* FTJ number used by SamPlan */
  char offer_type; /* Type of trip offer to be used with SamPlan trip */
  char detail_type; /* Type of trip details to be used with SamPlan trip */
  char call_msg_nbr;  /* Number of text messages associated with this SamPlan trip */
  
  int info;
  short timeout_flag;	/* indicator of a timed out call */
  double gps_lat;
  double gps_long;
  short m_action_retry;
  PREMIUM_SHIFT_STATE premium_shift_match_state;
} CALL;

typedef struct call_ipc_rec
{
  long    u_prcs_id;   	/* Users process id*/
  char    rec_type;      	/* Record type Call */
  int     msg_nbr;      	/* Message number for call to supervisor*/
  struct except_bits status; /* Status for exceptions*/
  int     c_isam_nbr;    	/* C-ISAM record number.*/
  int     call_nbr;      	/* System assigned call number.*/
  int     grouped_with; 	/* Call # of the main call if this is a call group*/
  char    fleet;         	/* Fleet Indicator. */
  struct	call_types call_type;  /* Call type. "P"assenger, "D"elivery, "X"-Dial-A-Ride. */
  short   pickup_zone;   	/* User entered or computer generated zone number*/
  short   dest_zone;   	/* User entered or computer generated */
  short   personal_veh; 	/* Vehicle number of the vehicle specifically requested by the customer*/
  char    personal_rqst; 	/* Defines whether "O"nly the vehicle specified may pickup the customer or the
				   customer would like to use that vehicle "I"f it is available, otherwise another
				   vehicle is acceptable. */
  struct veh_attributes veh_attr; /* flags Defines which vehicle attributes are required for this call.*/
  struct drv_attributes drv_attr; /*flags Define which driver attributes are required to carry out this call.*/
  short   priority;     	/* Call priority.*/
  time_t  due_time;      	/* Time this call is due. */
  char pick_pmap[9];	/* map coordinates for call offer */
  int updated_call;	/* is this an updated call or a new call 1=updated, 0=new*/
  time_t old_due_time; /*if doing an update and time was updated this value will be used to back out F35 interval count*/
  int FTJ_nbr; /* FTJ number used by SamPlan */
  char offer_type; /* Type of trip offer to be used with SamPlan trip */
  char detail_type; /* Type of trip details to be used with SamPlan trip */
  char call_msg_nbr;  /* Number of text messages associated with this SamPlan trip */
  double  gps_lat;
  double  gps_long;
  int  ilink_timeout; // if the trip is a transfer through Interlink (Helsinki), this holds the timeout value for the trip
} CALL_IPC_REC;

typedef struct _call_screen_data
{
  char screen_type;				/* BASIC/EXTENDED/EXTENDED-SUB... */
  CALL_REC *call_p;				/* pointer to call rec structure */
  AC_HNDL account_p;			/* account handle, if extended call */
} CALL_SCREEN_DATA_T;

#endif
