/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Stats_usr_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:23 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Stats_usr_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _STATS_USR_DB_H
#define _STATS_USR_DB_H
#include <isam.h>
typedef struct	cisam_su	{
	int	record_nbr;	/* System assigned for redundancy only		*/
	char	fleet;		/* Fleet ID					*/
	char	user_type;	/* 1=Phone opeator 2 = dispatcher 3 = mamager	*/
	short	user_id;	/* ID of the user this record pertains to.       */
	int	from_dt_tm;	/* Beginning of the time period this record applies to (C format).*/
	int	to_dt_tm;	/* End of the time period this record applies to (C format). */
	char	from_date[10];	/* Beginning date in INFORMIX format.		   */
	char	from_time[7];	/* Beginning time.				   */
	char	to_date[10];	/* Ending date in INFORMIX format.		   */
	char	to_time[7];	/* Ending time.					   */
	short	pass_calls;	/* Number of passanger calls entered by this user during the above time period.	*/
	short	del_calls;	/* Number of deliverey calls entered by this user during the above time period. */
	short	typex_calls;	/* Number of Type "X" calls entered by this user during the above time period.  */
	short	wkup_calls;	/* Number of wake-up calls entered by this user during the above time period.  */
	short	killed;		/* Number of calls killed by this user during the above time period(this number represents the */
				/* number of the times the user pressed the CANCEL function key while on basic,extended or */
				/* wakeup call entry screen.*/
	short	callbacks;	/* Number of callbacks processed by this user during the above time period(this number represents */
				/* the number of the times the user pressed the CALLBACK function key from any screen.*/
	short	unzoned;	/* Number of unzoned calls processed by this user during the above time period(this number */
				/* represent the all calls whose unzoned bit in the exception bit structure was turned on entry. */
	short	cancelled;	/* Number of calls cancelled by this user during the above time period(this number represents */
				/* the # of the times the user pressed the CANCEL function key while on any of the search screen.*/
	short	messages;	/* Number of messages sent by this user during the above time period(this number represents the */
				/* # of times the user pressed the SEND function key while in message window/message entry mode.*/
	short	man_assigns;	/* Number of calls manually assigned by a dispatcher to a specific vehicle during the above */
				/* time period(this number represents the number of the dispatcher assigned a call */
				/* that was not assigned yet to a vehicle.  NOTE: if a dispatcher use this screen as a */
				/* scratchpad multiple assignments (overrides) assignments (so long as the screen is not refresh).*/
	short	overrides;	/* Number of calls force assigned by a dispatcher to a sperific vehicle during the above time */
				/* period (this number represents the number of times the dispatcher assigned a call that was */
				/* already assigned to a vehicle. */
	short	exceptions;	/* Number of exception processed by a dispatcher to a specific vehicle during the above time */
				/* period(this number represents the number of the execption the dispatcher performed an action */
				/* on. An action on execption is define as any action that cause the execption line to be dimmed. */
				/* NOTE: if a dispatcher execptions through the call history window each exception taken care of */
				/* in that manner also counts). */
}USER_STATS_REC;
#ifdef KEY_SRC
struct keydesc su_key1= {
         ISDUPS,
         2,
				 { {4,CHARSIZE,CHARTYPE},
					 {6,INTSIZE,INTTYPE} }
};
struct keydesc su_key2= {
         ISNODUPS,
         1,
         {0,LONGSIZE,LONGTYPE},
};
#else
extern struct keydesc su_key1;
extern struct keydesc su_key2;
#endif
#endif
