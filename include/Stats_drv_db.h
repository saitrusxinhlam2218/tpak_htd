/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Stats_drv_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:21 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Stats_drv_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _STATS_DRV_DB_H
#define _STATS_DRV_DB_H
#include <isam.h>
typedef struct	cisam_sd	{
	int	record_nbr;	/* System assigned for redundancy only		*/
	char	fleet;		/* Fleet ID					*/
	char	pad;
	int	drv_id;		/* ID of the driver to who this record	pertains to */
	short	veh_nbr;	/* Veh number of the vehicle the above driver was driving while accumulating these statistics.	*/
	int	from_dt_tm;	/* Beginning of the time period this record applies to (C format).  */
	int	to_dt_tm;	/* End of the time period this record applies to (C format).   */
	char	from_date[10];	/* Beginning date in INFORMIX format.		   */
	char	from_time[7];	/* Beginning time.				   */
	char	to_date[10];	/* Ending date in INFORMIX format.		   */
	char	to_time[7];	/* Ending time.					   */
	short	calls;		/* Number of calls assigned this driver by the system or by a dispatch.	   */
	short	flags;		/* Number of flags picked up on the street (This number represets the number of times the driver */
				/* pressed the FLAG key and was approved by the dispatcher or the system).  */
	short	bids;		/* Number of bids awarded to this driver.	    */
	short	posts;		/* Number of the times this driver posted.	    */
	short	cond_posts;	/* Number of the times this driver conditionally posted. */
	short	enroute_posts;	/* Number of the times this driver enroute posts.   */
	short	short_meters;	/* Numbere of short meters.			    */
	short	late_meters;	/* Number of late meters.			    */
	short	no_accepts;	/* Number of no-accepts.			    */
	short	rejects;	/* Number of rejects.				    */
	short	breaks;		/* Number of breaks taken.			    */
	short	req_to_talk;	/* Number of times this driver requested to talk.   */
	short	messages;	/* Number of messages sent.			    */
	short	callbacks;	/* Number of callbacks.				    */
	short	callouts;	/* Number of callouts.				    */
	short	no_shows;	/* Number of no-shows.				    */
	short	emergencies;	/* Number of emergencies.			    */
	short	sign_ins;	/* Number of sign ins.				    */
	short	sign_offs;	/* Number of sign offs.				    */
}DRIVER_STATS_REC;
#ifdef KEY_SRC
struct keydesc sd_key1= {
         ISDUPS,
         2,
				 { {4,CHARSIZE,CHARTYPE},
					 {6,LONGSIZE,LONGTYPE} }
};
/* Composite of (fleet,veh_nbr) */
struct keydesc sd_key2= {
         ISDUPS,
         2,
				 { {4,CHARSIZE,CHARTYPE},
					 {10,INTSIZE,INTTYPE} }
};
/* record_nbr system assigned for redundancy only */
struct keydesc sd_key3= {
         ISNODUPS,
         1,
         {0,LONGSIZE,LONGTYPE},
};
#else
extern struct keydesc sd_key1;
extern struct keydesc sd_key2;
extern struct keydesc sd_key3;
#endif
#endif
