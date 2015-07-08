/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Zone_public.h,v $
*  @(#)  $Revision: 1.10 $
*  @(#)  $Date: 2002/03/21 14:52:16 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Zone_public.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

#ifndef _ZONE_PUBLIC_
#define _ZONE_PUBLIC_

#include <sys/time.h>

/* Zone_public.h */
#define ZONE_STAND_OPEN		1
#define ZONE_STAND_CLOSED	0
#define	ZN_STAND		'1'	/* zone is a stand */
#define	ZN_ZONE			'2'	/* zone is a regular zone */
#define	ZN_SUPER		'3'	/* zone is a super zone */

/* VALUES */
#define ZONE_AVAIL_CALLS_LIST   0   /* HNDL - available calls in zone list */
#define ZONE_AVAIL_VEH_LIST     1   /* HNDL - available taxis in zone list */
#define ZONE_DESCRIPTION        2   /* char * */
#define ZONE_EPOST_ALLOWED      3   /* char - YES & NO */
#define ZONE_FLEET_NBR          4   /* short */
#define ZONE_NBR                5   /* short */
#define ZONE_PICK_UP_LEAD_TIME  6   /* short - minutes befre due when released */
#define ZONE_SEALS              7   /* struct * seals_flags - for pickup */
#define ZONE_STAND_STATUS       8   /* short - 0 CLOSED 1 OPEN */
#define ZONE_STAND_START        9   /* short - stand start time 2400 format */
#define ZONE_STAND_STOP         10  /* short - stand stop  time 2400 format */
#define ZONE_SUPER_ZONE_NBR     11  /* short - NEW to struct zone from mads_isam */
#define ZONE_TIMEOUT_TIME       12  /* short - time before timout state occurrs */
#define ZONE_TYPE               13  /* char - 1=Stand 2=Zone 3=Superzone */
#define ZONE_MAX_TIME_CALLS	14  /* Max number of time calls */
#define ZONE_TC_LIST            15  /* List handle for time calls in the zone */
#define ZONE_LEAD_TIME          16
#define ZONE_GPS_LAT            17  /* int */
#define ZONE_GPS_LONG           18  /* int */
#define ZONE_GPS_RADIUS         19  /* int */
#define ZONE_EPOST_LEAD_TIME    20  /* short */
#define ZONE_GPS_DIST_CEILING   21  /* int */
#define ZONE_MATCH_ALGORITHM    22  /* char */
#define ZONE_TC_LOOK_AHEAD      23  /* short */
#define ZONE_TC_RESTRICT_INTERVAL 24 /* short */
#define ZONE_NS_SHORT           25  /* short */
#define ZONE_EMERG_RANGE        26  /* short */
#define ZONE_STAND_TIMER        27  /* int */
#define ZONE_ROTI               28  /* short */
#define ZONE_AUTO_INQRY         29  /* short[] */
#define ZONE_X_LEAD_TIME        30

/* STATISTICS */
#define ZONE_NBR_BID_CALLS        701   /* short  - calls up for bid */
#define ZONE_NBR_CALLS            702   /* short */
#define ZONE_NBR_ENROUTE          703   /* short - taxis in route to thsi zone */
#define ZONE_NBR_VEH              704   /* short */
#define ZONE_AVG_DISPATCH_TIME    705   /* short */
#define ZONE_NBR_CALLS_PICKED_UP  706   /* short */
#define ZONE_NBR_CALLS_WAIT_EPOST 707  /* short */
#define ZONE_NBR_ORDINAIRE_VEH_POST  708
#define ZONE_NBR_CLUB_ALL_VEH_POST       709
#define ZONE_NBR_CLUB_VEH_POST           710
#define ZONE_NBR_ORDINAIRE_VEH_EPOST     711
#define ZONE_NBR_CLUB_ALL_VEH_EPOST      712
#define ZONE_NBR_CLUB_VEH_EPOST          713

/* ZONE CONSTANTS */
#define ZONE_SLOT_NOT_AVAILABLE		-1
#define ZONE_DESC_LEN			20
#define ZONE_REQUESTED_SLOT		0
#define	ZONE_MAX_TIME_SLOTS		8
#define ZONE_CALL_NUMBER_SAVE_MAX	3
#define	ZONE_BUSY_RATING_NONE		0
#define ZONE_BUSY_RATING_EASY		1
#define ZONE_BUSY_RATING_MEDIUM		2
#define ZONE_BUSY_RATING_HARD		3

#define ZONE_ALG_VALUE_5                5
#define ZONE_ALG_VALUE_7                7
#define ZONE_ALG_VALUE_10               10
#define ZONE_ALG_VALUE_12               12
#define ZONE_ALG_VALUE_15               15
#define ZONE_ALG_VALUE_20               20
#define ZONE_ALG_VALUE_30               30

#define ZONE_MAX        		1001
#define SUBZONE_LEN			5
#define ZONES_PER_MDT_SCREEN_MAX        30      /* maximum number of zones in the search report */
#define MDT_ZONE_SCREEN_MAX             3       /* max # of screens on mdt for zone search report */

/* DB KEYS */
#define ZONE_KEY_PRIMARY		0

/* Driver IPC messages */
#define ZONE_UPDATE_RECORD		0
#define ZONE_UPDATE_TABLE		1

/* Return codes */
#define ZONE_NOT_RESTRICTED 		0
#define ZONE_MAX_TIME_CALLS_NO_ENTRY	-1
#define ZONE_NO_FLEET_BUSY_FACTOR	0
#define	ZONE_NULL_ARG			err_encode(ZONE_OBJ, NULL_ARG)
#define	ZONE_NO_MEMORY			err_encode(ZONE_OBJ, NO_MEMORY)
#define	ZONE_UNSUPPORTED_ARG		err_encode(ZONE_OBJ, UNSUPPORTED_ARG)
#define	ZONE_INVALID_ARG		err_encode(ZONE_OBJ, INVALID_ARG)
#define ZONE_BAD_ARGS			err_encode(ZONE_OBJ, INVALID_ARG)
#define	ZONE_IPC_FAILED			err_encode(ZONE_OBJ, IPC_FAILED)
#define ZONE_NO_ZONE			err_encode(ZONE_OBJ, 100)
#define ZONE_NO_MAX_FUTURE_TIME		err_encode(ZONE_OBJ, 101)
#define ZONE_NO_TIME_CALL_PERIOD	err_encode(ZONE_OBJ, 102)
#define ZONE_RESERVATION_NOT_ALLOWED    err_encode(ZONE_OBJ, 103)	
#define ZONE_NO_MAX_TIME_CALL_ENTRY     err_encode(ZONE_OBJ, 104)	
#define ZONE_NO_CALLS			err_encode(ZONE_OBJ, 105)

/* STATE */
#define ZONE_BID_STATE        100      /* Calls up for bid - set & reset */

#define Zone_set_info Zone_set_value

typedef char SUBZONE[SUBZONE_LEN+1];

typedef struct zone_id
        {
          short fleet_nbr;
          short zone_nbr;
        } ZONE_ID;

typedef struct zone_alt_time
        {
          char time_string[TIME_LEN + 1];
          char date_string[DATE_LEN + 1];
          time_t date_time;
          short call_count;
        } ZONE_ALT_TIME;

#include "Zone_private.h"
/* FUNCTION PROTOTYPES */
RET_STATUS	Zone_add(ZONE_REC_HNDL);
RET_STATUS	Zone_add_veh(short, VEH_HNDL);
RET_STATUS	Zone_alloc(ZONE_REC_HNDL *);
RET_STATUS	Zone_delete(ZONE_REC_HNDL);
void		Zone_free(ZONE_REC_HNDL *);
int             Zone_get_state(ZONE_HNDL, int);
HNDL            Zone_get_value(ZONE_HNDL, int );
RET_STATUS	Zone_notify_dispatch(short, ZONE_REC_HNDL);
RET_STATUS	Zone_read(ZONE_REC_HNDL, int, int);
RET_STATUS	Zone_release_locks();
RET_STATUS	Zone_update(ZONE_REC_HNDL);
RET_STATUS 	Zone_remove_veh(short, VEH_HNDL);
RET_STATUS      Zone_add_call(short, CALL_HNDL);
RET_STATUS      Zone_add_call_for_bid(short, CALL_HNDL);
RET_STATUS      Zone_call_bid_remove(ZONE_HNDL);
RET_STATUS      Zone_check_epost(ZONE_HNDL, VEH_HNDL);
RET_STATUS      Zone_decrement_stats(ZONE_HNDL, int);
RET_STATUS      Zone_get_hndl(short, short, ZONE_HNDL *);
RET_STATUS      Zone_get_info(ZONE_HNDL, int, HNDL *);
RET_STATUS      Zone_get_status( );
RET_STATUS      Zone_increment_stats(ZONE_HNDL, int);
RET_STATUS      Zone_initalize(ZONE_HNDL);
RET_STATUS      Zone_initialize_stats(ZONE_HNDL, int);
RET_STATUS      Zone_release_epost(ZONE_HNDL, VEH_HNDL);
RET_STATUS      Zone_remove_call(short, CALL_HNDL);
RET_STATUS      Zone_reset_state(ZONE_HNDL, int, HNDL);
RET_STATUS      Zone_set_state(ZONE_HNDL, int, HNDL);
RET_STATUS      Zone_set_value(ZONE_HNDL, int, HNDL);
RET_STATUS      Zone_veh_end_queue(short, VEH_HNDL);
ZONE_HNDL       Zone_create(short, short, ZONE_REC *);

#endif

