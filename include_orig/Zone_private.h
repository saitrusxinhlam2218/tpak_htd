/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Zone_private.h,v $
*  @(#)  $Revision: 1.13 $
*  @(#)  $Date: 2004/06/22 17:11:18 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Zone_private.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

#ifndef _ZONE_PRIVATE_
#define _ZONE_PRIVATE_

#define BCK_UP_MAX 	8	/* maximum number of backup zones in a level */
#define INV_BCK_UP_MAX  64
#define LEVEL_MAX 	9	/* maximum number of levels in a zone */
#define ZONE_MAX 	1001	/* maximum number of zones in a fleet is 999  */
                                /* zone designated 1000 is reserved for wakeup calls */
#define WAKEUP_ZONE     1000
#define ZONE_INQUIRY_BACKUP_ZONES 11  // number of surrounding zones to define for the auto zone inquiry feature
#define STAND_TELEPHONE_LEN 18

/* PRIVATE VALUES */
#define ZONE_BID_TIMER 200

typedef struct  cisam_zn   {
  char    fleet;                       /* Fleet indicator.          */
  char    superzn_detail;              /* Applies to superzone records only.  Used in the superzone reports. */
  /* "Y" -create a superzone description from a generated list of individual zones. */
  /* "N" -print the English description (zn_desc field) */
  short   nbr;                         /* A zone number (1-999) or a superzone (1-64000) specific to this fleet.    */
  short   superzn_nbr;                 /* A superzone number that this zone belongs to.  (1 - 64,000).  If this record */ 
  /* is a super zone record then this field will repeat the superzone number */
  char    desc[21];                    /* English description of this zone or superzone contents (E.g. FOGGY */
  /*    BOTTOM for a zone or Prince George County for superzone).                 */
  char   type;                         /* Type of zone.  Valid types are 1=Stand, 2=Zone, 3=Superzone, 4=Backup Zone. */
  char   bid_allowed;                  /* "Y"/"N".  Defines whether fleetwide call offer is to be utilized if the call */
  /* cannot be matched to cabs in any specific stands, zones or superzones.       */
  char   pckup_seals[17];              /* A set of 16 flags indicating the seals required in this zone for customer pickup.*/
  /* Seals are fleet specific and are numbered 1 - 16 for cross- reference to the */
  /* vehicle & driver files.             */
  short   pckup_lead_time;               /* Number of minutes before due time of time calls that those calls are released. */
  short   stand_time_from;               /* In 24hr format, beginning time of stand availability.  Only applies to stands */
  short   stand_time_to;                 /* in 24hr time format, ending time of stand availability.  Only applies to stands */
  short  avl_call_q_time;                /* Amount of time a call remains in dispatch before it times out and */
  /* becomes an exception for supervis if not entered,entry in the fleet file is used. */
  char   enr_post_allow;                 /* "Y"/"N".  Whether enroute posting is allowed into this zone.                */
  
  /*******************************************************************************
   *          Dispatch algorithm choices are:                                     *
   *                1)Temporal by Zone Type                                       *
   *                2)Sequential by Zone, Temporal by Cab                         *
   *                3)Sequential by Zone, Cab Queue Order                         *
   *          Reject Penalties/Choices Are:                                       *
   *                1) Not Allowed                                                *
   *                2) Allowed, No Penalty                                        *
   *                3) Allowed, Bottom of Queue                                   *
   *******************************************************************************/
  char    lev_alg[LEVEL_MAX];                   /* Defines the dispatch algorithm used to match calls to cabs within this zone.*/
  short   lev_zone_nbr[LEVEL_MAX][BCK_UP_MAX]; 
  short   lev_excl_time[LEVEL_MAX];             /* Defines the interval (in the cabs on stands will be considered for dispatch */
  char    lev_rej_penlty[LEVEL_MAX];            /* Reject penalty/choices for level 1 - max                        */
  short   max_time_calls;			  /* for restrict time calls */
  short   mdt_screen;				  /* 1 - 3, for screen # the zone appears on MDT */
  short   enr_post_lead;                        /* Minutes.  Amount of time given a driver from the time he/she enroute posts */
  /* to the time he/she actually posts into the same zone.  If this value is */
  /* zero for the zone then the fleet value is used */
  short   break_duration;
  short   accept_duration;                      /* Seconds.  How much time is given to the driver to accept a call (i.e. to */
  /* press the <ACPT> key).  (Default -30) */
  char    check_avail_taxis;                  /* Whether to look for available taxis in this zone **/
  double   gps_lat;  /* expressed in UTM or LAT/LONG */
  double   gps_long; /* expressed in UTM or LAT/LONG */
  int     gps_radius; /* in meters */
  int    gps_dist_ceiling;
  char   match_algorithm;
  short  tc_look_ahead;
  short  tc_restrict_interval;
    short ns_short;    // short no-show determinant
  short   emerg_range;
  char    stand_telephone[STAND_TELEPHONE_LEN + 1];
  short   stand_hold_timer;
  short   cbook_time;
  short   roti_time;
  short   stand_nbr;
  short   inquiry_backup[ZONE_INQUIRY_BACKUP_ZONES];
  char     poly_checksum[5];
  short   typex_lead_time;  // when order is typex this will override the standard lead time
} ZONE_REC;

typedef struct cisam_zn_poly
{
  char    fleet;
  short   nbr;
  short   seq_nbr;
  double  pointx;
  double  pointy;
  char  latitude[13];
  char  longitude[13];
} ZONE_POLY_REC;

typedef struct _POINT_POLY
{
  double x,y;
} POINT_POLY;

#define MAX_POLY_POINTS 500
 

typedef	struct	level {
        short bck_up_zone[BCK_UP_MAX];     	/* backup zones for this zone */
        short inv_bck_up_zone[INV_BCK_UP_MAX]; 	/* inverted backup zones for this zone */
        short num_in_inv;                       /* number of zones in the inverted backup zone list */
} ZONE_LEVEL;

typedef struct zones {
	struct level level_number[LEVEL_MAX];	/* level information for this zone */
	char level_algorithm[LEVEL_MAX];	/* find taxi algorithm for this level */
	short level_exclusive_time[LEVEL_MAX]; 	/* time spent looking for a cab at this level */
	char level_reject_penalty[LEVEL_MAX];	/* penalty if a call is rejected at this level */
	char description[21];			/* zone description */
	char type;				/* 1=Stand 2=Zone 3=Superzone */
	short super_zone;			/* to be used in future  systems */
	struct veh_driv *avail_taxi; 	/* pointer to first available taxi */
	short num_taxis;  		/* number of available taxis */
	struct calls *avail_call; 	/* pointer to first available call */
	short num_calls;  		/* number of available calls */
	short num_enroute;  		/* number of vehicles enroute posted to this zone  */
	short avg_time;			/* average time to dispatch */
	short calls_pckd_up;		/* number of calls picked up in a fixed period */
	time_t tm_to_pckup;		/* cumulative time per period to pickup calls */
	char fleet_call_offer; 		/* Defines whether fleetwide call offer is to be utilized if the call cannot be
				  	 matched to cabs in any specific stands,  zones or superzones. */
	struct seals_flags pckup_seals; 	/*  A set of flags indicating the seals required in this zone for customer 
					 pickup.  Seals are fleet specific and are numbered 1 - 16 for cross-reference to the 
				 	 vehicle file.  */
	short pickup_lead_time; 	/*Number of minutes before due time of time calls that those calls are released. */
	short typex_lead_time;
	char enr_post_allow;		/* enroute post allowed to this zone */
	short stand_time_from;		/* beginning stand availability in 2400 format */
	short stand_time_to;		/* end stand availability in 2400 format */
	short avl_call_q_time;		/* time before call becomes an exception */
	ZONE_VEH_LIST_HNDL veh_hndl;
	ZONE_CALLS_LIST_HNDL calls_hndl;
	ZONE_TC_LIST_HNDL  time_calls_hndl;
	short zone_nbr;
	short fleet_nbr;
	short mdt_screen;		/* The MDT screen # for search report this zone is placed in */
        short e_post_lead_time;         /*   minutes   Amount of time given a driver from the time he/she enroute posts to
                                         the time he/she actually posts into the same zone.   */
	short break_duration;
	short accept_duration; 		/*  seconds How much time is given to the driver to accept a call 
					 (i.e. to press the <ACPT> key).  */
	char check_avail_taxis;
	short nbr_calls_epost_wait;
	double gps_lat;       /* UTM-32 coordinates */
	double gps_long;      /* UTM-32 coordinates */
	int  gps_radius;
	int    gps_dist_ceiling;
	char   match_algorithm;
	POINT_POLY  zone_poly[ MAX_POLY_POINTS ];
	int    poly_points;
	double poly_centroid_x;
	double poly_centroid_y;
    short  tc_look_ahead;
    short  tc_restrict_interval;
    short ns_short;    // short no-show determinant
  short   emerg_range;
  short   stand_hold_timer;
  short   cbook_time;
  short   roti_time;
  short   inquiry_backup[ZONE_INQUIRY_BACKUP_ZONES];  
} ZONE;

typedef struct shmem_zone {
  POINT_POLY  zone_poly[ MAX_POLY_POINTS ];
  int poly_points;
  double poly_centroid_x;
  double poly_centroid_y;  
} MEM_ZONE;

#define ZN_SHMKEY 8

typedef struct _memZones {
  
  MEM_ZONE zones[ZONE_MAX];
} MEM_ZONES;


#endif
