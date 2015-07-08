/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Stats_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:20 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Stats_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _STATS_DB_H
#define _STATS_DB_H
#include <isam.h>
typedef struct		cisam_sa
{
          char	fleet;  		/*  Fleet ID			*/
          char	date[9];     		/* The date these statistics apply to.  In MM/DD/YY format.	*/
          int	sort_date;          	/* "C" version of the above date for sort purposes. 		*/
          short	day_start;		/* Beginning of the day in 24 hour format. */
          short	day_end;         	/* End of the day in 24 hour format.*/
          /* The following fields are user statistics: */
          short	u_pass_calls; 		/* Number of passenger calls entered into the system by users. */
          short	u_del_calls;		/* Number of delivery calls entered into the system by users. */
          short	u_typex_calls;		/* Number of type "X" calls entered into the system by users.		*/
          short	u_wakeup_calls;     	/*  Number of wake-up calls entered into the system by users. 		*/
          short	u_killed_calls;		/*  Number of calls killed by the users. */
          float	u_call_rate;		/*  Number of calls per hour entering the system.		*/
          short	u_callbacks;   		/*  Number of callbacks processed by the users. */
          short	u_unzoned;       	/*  Number of unzoned calls entered into the system. */
          short	u_cancelled;		/*  Number of calls cancelled by the users.  */
          short	u_messages; 		/*  Number of messages sent by users to other users or drivers. 		*/
          short	u_assigns;		/* # of calls that were unassigned and were assigned manually by dispatcher to a cab.	*/
          short	u_overrides; 		/* Number of calls that were dispatched to a specific cab by the system and later */
                                        /* manually reassigned by a dispatcher to another cab.		*/
          int	u_exceptions;		/* Total number of exceptions handled.  */
          float	u_hours_worked; 	/* Total hours the system was in use by all the users (hours worked). 	*/
         /* The following statistics are for drivers:	*/
          short	d_calls;         	/* Number of calls ACPTed by drivers.	*/
          int	d_posts;		/* Number of post requests processed by the system. 			*/
          int	d_cposts;		/* Number of conditional post requests processed by the system.		*/
          short	d_eposts;		/* Number of enroute post requests processed by the system.		*/
          short	d_flags; 		/* Number of flag requests processed by the system.*/
          short	d_bids;			/* Number of bid responses processed by the system.*/
          short	d_no_accepts;		/* Number of no_accepts processed by the system.   */
          short	d_rejects;		/* Number of reject requests processed by the system.*/
          short	d_short_meters;  	/* Number of short meters caused by the drivers.   */
	  short	d_late_meters;  	/* Number of late meters caused by the drivers.    */
          short	d_callbacks;		/* Number of callbacks caused by the drivers.      */
          short	d_callb_mon;		/* Number of callbacks caused by drivers who turned on their meter prior to callback. */
          short	d_callb_noshow;		/* Number of callbacks caused by drivers who claimed a NOSHOW prior to the callback. */
          short	d_callouts;		/* Number of callout requests processed by the system.*/
          short	d_noshows; 		/* Number of no show requests processed by the system.*/
          short	d_breaks; 		/* Number of break requests processed by the system.   */
          short	d_talks; 		/* Number of request to talk requests processed by the system.			*/
          short	d_messages;		/* Number of messages sent by the drivers to dispatchers.			*/
          short	d_emergencies;		/* Number of emergency requests processed by the system. */
          short	d_sign_ins;		/* Number of sign in requests processed by the system.*/
          short	d_sign_outs;		/* Number of sign out requests processed by the system. */
          float	d_hours_worked; 	/* Number of hours worked by all the drivers (sign-in to sign-out).	*/
          float	d_avg_act_cabs;		/* Avg # of vehicles active (have accepted a call) on the streets during any one hour. */
}STATS_REC;
#ifdef KEY_SRC
struct keydesc sa_key1= {
         ISNODUPS,
         2,
				 { {0,CHARSIZE,CHARTYPE},
					 {1,8,CHARTYPE} }
};
#else
extern struct keydesc sa_key1;
#endif
#endif
