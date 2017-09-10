 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  pi_struc.h; Rel: 10.1.2.1; Get date: 4/19/95 at 08:46:16
 * @(#)  Last delta: 12/2/94 at 18:52:53
 * @(#)  SCCS File: /taxi/sccs/s.pi_struc.h
 *                                                                           *
 *****************************************************************************/
/* -------------------------------------------------------------------------------
   Copyright 1993 - TaxiCom Copenhagen, Denmark
   Copyright 1995 - 2001 Stento Danmark A/S

   System	:	PLANET - LINK
   Filename	:	pi_struc.h
   Purpose	:	This module contains the 3 basic structures for PI:
			CALL
			MESSAGE
			EXCEPTION

Modification history

Author	Date	Revision	Description
------  ------- --------------- -----------------------------------------------
LG      931125  0.0		The 3 basic PI-structures.
JW      950122  0.5             See RCS
JW      950424  0.6             See RCS
JW      970311  0.7             See RCS
JW      980620  0.8             See RCS
------------------------------------------------------------------------------- */

#ifndef _H_PI_STRUC
#define _H_PI_STRUC

/* The interface structure for a call. */
/***************************************/

typedef struct call {
  char		fleet;			/* Letter identifying current fleet (A-H) 				*/
  int		call_number;		/* Unique incremental call-number					*/
  short		priority;		/* The calls priority (6 - 63)						*/
  int		call_group;		/* Call-number of the main call of the group (0 if single)		*/
	char    call_position;    /* This calls position within the group (0 if single)     */
  char		number_of_calls;	/* Number of calls within the group (0 if single)			*/
  char		number_of_vehicles;	/* Number of vehicles needed for the current call			*/
  char		call_type[ 5 ];		/* Combination of types of call (account, future, multi etc. )		*/
  char		from_addr_street[ 21 ];	/* Streetname of pickup-address						*/
  int		from_addr_number;	/* Streetnumber of pickup-address (0 if PLACENAME)			*/
  char		from_addr_suffix;	/* Streetnumber suffix letter (ex. 15 H) of pickup-address		*/
  char		from_addr_apart[ 6 ];	/* Apartment or room-number of pickup-address				*/
  char		from_addr_city[ 4 ];	/* TaxiPak abbreviation of city of pickup-address (ex. GBG)		*/
  short		from_addr_zone;		/* Zone of pickup-address						*/
  char		from_addr_cmnt[ 31 ];	/* Pickup-address comment (also used for PLACENAME)			*/
  char		passenger[ 21 ];	/* Name of passenger to be picked up					*/
  char		phone[ 11 ];		/* Phonenumber of passenger						*/
  char		to_addr_street[ 21 ];	/* Streetname of destination-address					*/
  int		to_addr_number;		/* Streetnumber of destination-address (0 if PLACENAME)			*/
  char		to_addr_suffix;		/* Streetnumber suffix letter (ex. 15 H) of destination-address		*/
  char		to_addr_apart[ 6 ];	/* Apartment or room-number of destination-address			*/
  char		to_addr_city[ 4 ];	/* TaxiPak abbreviation of city of destination-address (ex. GBG)	*/
  short		to_addr_zone;		/* Zone of destination-address						*/
  char		to_addr_cmnt[ 31 ];	/* Destination-address comment (also used for PLACENAME)		*/
  short		car_number;		/* Eventual car connected to the call					*/
  char          car_attrib[ 32 ];

  char		driver_id[ 6 ];		/* Driver-number containing 5 digits					*/
  char		driver_attrib[ 32 ];	/* 8 driver-attributes (Y/N) analog to the ones in Taxipak		*/
  short		creator;		/* user-ID of the creator (PLANET = 9999)				*/
  char		creation_date[ 7 ];	/* Date the call is created/received (YYMMDD)				*/
  char		creation_time[ 5 ];	/* Time the call is created/received (HHMM)				*/
  char		due_date[ 7 ];		/* Date the call is to be dispatched (YYMMDD)				*/
  char		due_time[ 5 ];		/* Time the call is to be dispatched (HHMM)				*/
  char		pickup_time[ 5 ];	/* Time the customer is picked up by the taxi (HHMM)			*/
  char		close_time[ 5 ];	/* Time the trip is finished (HHMM)					*/
  char		call_comment[ 65 ];	/* General comment (also used for PICKUP/DEST-message to car)		*/
  short		call_status;		/* Status for the call (ASSIGNED, PICKUP, COMPLETE etc.)		*/
  char          gpsx[8];
  char          gpsy[8];
	} PI_CALL_REC;

// sizeof call is 380 without a GPS position
// and 396 with a GPS position
#define CALL_STRUC_GPS_SIZE   sizeof(struct call)
#define CALL_STRUC_SIZE       sizeof(struct call) - 16

typedef struct _pi_callh_rec_
{
  char   event_desc[9];
  char   status_date[10];
  char   status_time[7];
  short  veh_nbr;
  int    drv_id;
  short  user_id;
  char   action_taken[9];
  char   action_date[10];
  char   action_time[7];
  int    ch_call_number;
} PI_CALLH_REC;

	/* The interface structure for a external call. */
	/************************************************/


typedef	struct call_ext {
		char    fleet;      /* Letter identifying current fleet (A-H)         */
		int   call_number;    /* Unique incremental call-number         */
		short   priority;   /* The calls priority (6 - 63)            */
		int   call_group;   /* Call-number of the main call of the group (0 if single)    */
		char    call_position;    /* This calls position within the group (0 if single)     */
		char    number_of_calls;  /* Number of calls within the group (0 if single)     */
		char    number_of_vehicles; /* Number of vehicles needed for the current call     */
		char    call_type[ 5 ];   /* Combination of types of call (account, future, multi etc. )    */
		char    from_addr_street[ 21 ]; /* Streetname of pickup-address           */
		int   from_addr_number; /* Streetnumber of pickup-address (0 if PLACENAME)      */
		char    from_addr_suffix; /* Streetnumber suffix letter (ex. 15 H) of pickup-address    */
		char    from_addr_apart[ 6 ]; /* Apartment or room-number of pickup-address       */
		char    from_addr_city[ 4 ];  /* TaxiPak abbreviation of city of pickup-address (ex. GBG)   */
		short   from_addr_zone;   /* Zone of pickup-address           */
		char    from_addr_cmnt[ 31 ]; /* Pickup-address comment (also used for PLACENAME)     */
		char    passenger[ 21 ];  /* Name of passenger to be picked up          */
		char    phone[ 11 ];    /* Phonenumber of passenger           */
		char    to_addr_street[ 21 ]; /* Streetname of destination-address          */
		int   to_addr_number;   /* Streetnumber of destination-address (0 if PLACENAME)     */
		char    to_addr_suffix;   /* Streetnumber suffix letter (ex. 15 H) of destination-address   */
		char    to_addr_apart[ 6 ]; /* Apartment or room-number of destination-address      */
		char    to_addr_city[ 4 ];  /* TaxiPak abbreviation of city of destination-address (ex. GBG)  */
		short   to_addr_zone;   /* Zone of destination-address            */
		char    to_addr_cmnt[ 31 ]; /* Destination-address comment (also used for PLACENAME)    */
		short   car_number;   /* Eventual car connected to the call         */

		char    car_attrib[ 32 ];

		char    driver_id[ 6 ];   /* Driver-number containing 5 digits          */

		char    driver_attrib[ 32 ];

		short   creator;    /* user-ID of the creator (PLANET = 9999)       */
		char    creation_date[ 7 ]; /* Date the call is created/received (YYMMDD)       */
		char    creation_time[ 5 ]; /* Time the call is created/received (HHMM)       */
		char    due_date[ 7 ];    /* Date the call is to be dispatched (YYMMDD)       */
		char    due_time[ 5 ];    /* Time the call is to be dispatched (HHMM)       */
		char    pickup_time[ 5 ]; /* Time the customer is picked up by the taxi (HHMM)      */
		char    close_time[ 5 ];  /* Time the trip is finished (HHMM)         */
		char    call_comment[ 65 ]; /* General comment (also used for PICKUP/DEST-message to car)   */
		short   call_status;    /* Status for the call (ASSIGNED, PICKUP, COMPLETE etc.)    */
		int     ftj_number;     /* 4 byte integer - little endian( bytes-reversed ) */
		char    offer_type;     /* 0-9 trip offer type (ASCII value) */
		char    detail_type;    /* 0-9 trip detail type (ASCII value) */
		char    msg_nbr;        /* 1-3 text messages attached to call (ASCII) */
		} CALL_EXT_REC;


/* The deivce call message structure for taxi devices.          */
/*********************************************************/

/* Size of call_msg structure is 264 */

typedef struct cmsg {
	int   call_nbr;            /* Unique record-ID for every TaxiPak call */
	char  msg_nbr;             /* Number assigned externally by Ftj/Samplan for each trip/device */
	int   changed_by;          /* future use        */
	int   last_changed;        /* future use        */
	char  device;              /* device to send msg to (1-3)(prn/itm/mdt)      */
	char  msg[250];            /* data/text for device  */
} CALL_MSG_REC;


/* The interface structure for a non-repeatable message. */
/*********************************************************/

#define MESSAGE_STRUC_SIZE	544

typedef struct message {
  char		fleet;			/* Letter identifying current fleet (A-H) 				*/
  int		message_number;		/* Unique incremental number of the message				*/
  short		creator;		/* user-ID of the creator (PLANET = 9999)				*/
  char		creation_date[ 7 ];	/* Date of message creation (YYMMDD)					*/
  char		creation_time[ 5 ];	/* Time of message creation (HHMM)					*/
  char		receive_group;		/* Group to receive the message (V=Vehicle, D=Driver etc.)		*/
  char		receive_id[ 7 ];	/* Number of the receiver (Vehicle-number, driver-number etc.)		*/
  char		message_text[ 512 + 1 ];	/* The message								*/
  } MSG_REC;

/* The interface structure for an exception. 		*/
/********************************************************/

#define EXCEPTION_STRUC_SIZE	44

typedef struct _exception_ {
  long		exception_number;	/* A unique number for the exception					*/
  char		fleet;			/* Letter identifying current fleet (A-H) 				*/
  char		creation_date[ 7 ];	/* Date of message creation (YYMMDD)					*/
  char		creation_time[ 5 ];	/* Time of message creation (HHMM)					*/
  int		exception_type;		/* Enumeration-value of exception					*/
  char		approval;		/* Is the exception approved/disapproved? (Y/N)				*/
  short		zone_number;		/* Eventual zone-number							*/
  int		call_number;		/* Eventual call-number							*/
  short		car_number;		/* Eventual car-number							*/
  long		message_number;		/* Eventual message-number for a predefined message (canned ...)	*/
  char		outstanding;		/* Is the exception not handled yet? (Y/N)				*/
  } PI_EXCEPT_REC;

typedef struct _ZONE_INFO_REC
{
  char           fleet;
  short          zone_nbr;
  short          veh_primary_zone;
  short          veh_backup_zones;
  char           veh_attr[ 32 ];
  char           drv_attr[ 32 ];
  short          calls_unassigned;
} ZONE_INFO_REC;

typedef struct _ACCT_DATA_REC
{
  char          acct_nbr[16];
  char          fleet;
  char          call_number[8];
  char          priority[2];
  char          number_of_calls;
  char          call_type[5];
  char          from_addr_street[21];
  char          from_addr_number[6];
  char          from_addr_suffix;
  char          from_addr_apart[6];
  char          from_addr_city[4];
  char          from_addr_zone[3];
  char          from_addr_cmnt[31];
  char          passenger[21];
  char          requested_by[21];
  char          phone[11];
  
  // destination stuff here
  char          to_addr_street[21];
  char          to_addr_number[6];
  char          to_addr_suffix;
  char          to_addr_apart[6];
  char          to_addr_city[4];
  char          to_addr_zone[3];
  char          to_addr_cmnt[31];
  
  char          car_number[6];
  char          car_attrib[32];
  char          driver_id[6];
  char          driver_attrib[32];
  char          creator[4];
  char          creation_date[7];
  char          creation_time[5];
  char          due_date[7];
  char          due_time[5];
  char          pickup_time[5];
  char          close_time[5];
  char          call_comment[65];
  char          gpsx[8];
  char          gpsy[8];
  char          fare_amount[8];
  char          vat[8];
} ACCT_DATA_REC;
#define ACCT_DATA_REC_SIZE sizeof(ACCT_DATA_REC)

typedef struct _ACCOUNT_DATA_REC
{
  char           fleet;
  char           acct_nbr[16];
  char           phone_nbr[16];
}ACCOUNT_DATA_REC;

typedef struct _BOOK_SLOTS
{
  char          time[5];
  short         n_free;
} BOOK_SLOTS;

typedef struct _COORD_TRANSFORM_
{
  char          latitude[10];
  char          longitude[10];
  char          zone[3];
} COORD_TRANSFORM;

typedef struct _PRE_BOOK_INFO
{
  char           fleet;
  short          zone_nbr;
  char           date[ 7 ];
  BOOK_SLOTS     book_list[ 7 ];
} PRE_BOOK_INFO;

typedef struct _RTN_CMSG_REC_ {
	char   call_nbr[8];            /* Unique record-ID for every TaxiPak call */
	char   msg_nbr;             /* Number assigned externally by Ftj/Samplan for each trip/device */
	char   changed_by[4];          /* future use        */
	char   last_changed[4];        /* future use        */
	char   device;              /* device to send msg to (1-3)(prn/itm/mdt)      */
	char   msg[250];            /* data/text for device  */
} RTN_CALL_MSG_REC;

typedef struct _SND_MSG_REC_ {
  char fleet;
  char receive_group;
  char receive_id[7];
  char message_text[201];
} SND_MSG_REC;

typedef struct _FLEET_STATUS_
{
  char fleet_id;
  char fleet_name[32];
  int signed_on;
  int signed_off;
  int calls;
  int flags;
  int cond_posts;
  int enroute_posts;
  int short_meters;
  int late_meters;
  int no_accepts;
  int asigns;
  int rejects;
  int breaks;
  int req_to_talk;
  int messages;
  int callbacks;
  int callouts;
  int no_shows;
  int suspended;
  int emergencies;
  int wakeups;
  int cancels;
  int now_signed_on;
  int now_signed_off;
  int now_in_fleet;
  int now_calls;
  int now_flags;
  int now_posts;
  int now_enroute_posts;
  int now_late_meters;
  int now_breaks;
  int now_req_to_talk;
  int now_messages;
  int now_callbacks;
  int now_callouts;
  int now_no_shows;
  int now_idle;
  int now_suspended;
  int now_emergencies;
} FLEET_STATUS;

typedef struct _PI_VEH_STATUS
{
  char fleet;
  char veh_id[6];
  char current_status[32];
  char meter_status[16];
  char veh_attr[64];
  char drv_attr[64];
  short assigns;
  short calls;
  short flags;
  short posts;
  short cond_posts;
  short enroute_posts;
  short short_meters;
  short late_meters;
  short no_accepts;
  short rejects;
  short breaks;
  short req_to_talk;
  short messages;
  short callbacks;
  short callouts;
  short no_shows;
} VEH_STATUS;
  
typedef struct _PI_LINE_MGR_REC
{
  char frame_type;
  char customer_id;
  char agent_id[6];
  char a_number[20];
  char pin_number[14];
  char due_time[8];
  char due_date[8];
  char call_nbr[10];
} PI_LINE_MGR_REC;

#define STANDARD_BOOKING  '1'
#define AUTOBOOKING '2'

typedef struct _PI_CBACK_COUT_REC
{
  char   frame_type;
  char   active;
} PI_CBACK_COUT_REC;

#define CALLBACK_FRAME   '1'
#define CALLOUT_FRAME    '2'

typedef struct _PI_PHONE_DEMAND_REC
{
  char   frame_type;
  char   agent_id[6];
  char   phone_number[20];
  char   demand_data[20];
} PI_PHONE_DEMAND_REC;

typedef struct _PI_EMERG_PICTURE
{
  char cEmergency;               // 0
  char cCamTriggerSource;        // 0 + sizeof(char) = 1
  int  iMobileID;                // 1 + sizeof(char) = 2 -- 4 with padding
  double szLatitude;             // 4 + sizeof(int) = 8
  double szLongitude;            // 8 + sizeof(double) = 16
  int sTime;                     // 16 + sizeof(double) = 24
  char cGPSStatus;               // 24 + sizeof(int) = 28
  char cYear;                    // 28 + sizeof(char) = 29
  char cMonth;                   // 29 + sizeof(char) = 30
  char cDay;                     // 30 + sizeof(char) = 31
  char szVehicleID[11];           // 31 + sizeof(char) = 32
  // start of picture 30 + 11 = 41
} PI_EMERG_PICTURE_REC;

typedef struct _PI_ACCOUNT_VALIDATE
{
  char Fleet;
  char ID[18];
  char Password[16];
  char Name[25];
  char StreetNbr[8];
  char StreetSuffix;
  char StreetName[20];
  char Apart[5];
  char City[3];
  char Phone[18];
  char Attention[20];
  char Comment[40];
  char DriverMsg[36];
  char Email[64];
} PI_ACCOUNT_VALIDATE_REC;

typedef struct _PI_GET_CALLS_REC_
{
  char  Fleet;
  char  ID[18];
  char  start_date[7];
  char  start_time[5];
  char  end_date[7];
  char  end_time[5];
  int   status;
  int   nbr_of_calls;
} PI_GET_CALLS_REC;

typedef struct _PI_GET_ACCOUNT_ADDR_REC_
{
  char  Fleet;
  char  ID[18];
  int   nbr_of_addr;
} PI_GET_ACCOUNT_ADDR_REC;

typedef struct _PI_GET_ACCOUNT_INDV_REC_
{
  char  Fleet;
  char  ID[18];
  int   nbr_of_indv;
} PI_GET_ACCOUNT_INDV_REC;

typedef struct _PI_RQST_GPS_POS_
{
  int   veh_nbr;
} PI_RQST_GPS_POS;

#define PI_EMERG_PICTURE_OVERHEAD (sizeof(PI_EMERG_PICTURE_REC) -1 )



// Log stuff
#define EXTP_TO_PI  "EXTP --> PI  "
#define PI_TO_EXTP  "PI   --> EXTP"

#endif


