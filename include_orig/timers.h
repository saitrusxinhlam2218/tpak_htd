 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  timers.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:49:57
 * @(#)  Last delta: 12/2/94 at 18:11:56
 * @(#)  SCCS File: /taxi/sccs/s.timers.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:timers.h	10.1.2.1"

/* based on R3 version 5.1 */

#define TIMER_MARGIN	5				/* Value to allow for a margin because of deletions. */
#define TIMERS_PER_CALL 4				/* maximum timers per call */
#define MAX_TIMERS       CALLS_MAX * TIMERS_PER_CALL	/* total number of timers in the system at any one time */
#define MAX_SEQ_SRCH     100
#define MAX_INT_PTR      MAX_TIMERS / MAX_SEQ_SRCH + TIMER_MARGIN

#define DRV_STATS_FREQ  6    /* how many time per hr veh stats output */
#define DRV_STATS_PERIOD (3600 / DRV_STATS_FREQ)  /* how often veh stats output in sec */


#define T_DEL_ALL	-1	/* Delete all timers for this call
			 	 when it expires */
#define T_DEL_ONLY	0	/* 0  Delete only this timer when it expires */
#define T_TIMEOUT	1	/* 1  Call timeout timer */
#define T_ACCPT		2  	/* Call acceptance timer */
#define T_LATE_METER	3  	/* Late meter timer */
#define T_ZONE_TIMEOUT	4  	/* Time-out by zone level timer */
#define T_BID_CLOSE	5  	/* Bid close timer */
#define T_SRCH		6  	/* Search report cycle timer */
#define T_ZN_SUMMARY	7  	/* Zone summary display cycle timer */
#define T_REP_MSG	8  	/* Repeat message timer */
#define T_TM_TO_PCKUP	9  	/* Time to pickup statistics */
#define T_DAY_END	10	/* End of the day */
#define T_USR_DRV_STAT	11	/* User and driver statistics */
#define T_WAIT_EPOST    12	/* Epost timer */
#define T_KDT_STATS	13	/* Count kdt key presses */
#define T_DR_ONLY_SRCH	14	/* Driver only search report cycle timer id */
#define T_MISSING_RECEIPT 15    /* missing receipt time_out */
#define T_INACTIVITY 	16      /* inactivity time_out */
#define T_STAGGER_CALLS	17      /* stagger call time_out */
#define T_RECALC_INTVLS	18      /* recalculate the interval counters for future time calls */
#define T_HOURLY_TASKS  19      /* tasks that should be performed each hour */
#define T_BIZ_STATUS    20      /* Business status level reporting for Stockholm */
#define T_TIMECALLS     21      /* scan TC lists for time calls that are due */
#define T_ZONE_REBUILD  22      /* periodically rebuild the zone calls list */
#define T_M_ACTION      23      /* meter action message timer */
#define T_TEST_DRIVE    24      /* used to move vehicles on a map during testing */
#define T_BREAK         25      /* duration of vehicle break */
#define T_HTTP_BREAK    26      /* duration of IN-HTTP imposed break */
#define T_ILINK_TIMEOUT 27      /* duration of matching for an Interlink transfer trip */
#define T_ILINK_TRANSFER_VERIFY 28

/* record structure of an individual timer event */
struct timers {
	int    exp_date_time;      	/* Expiration date and time */
	int    msg_num;       		/* message number (repeat messages only)*/
	int    veh_num;				/* Vehicle number for timer */
	struct calls *call_ptr;		/* pointer to a call record */
	short  purpose;            	/* Purpose of timer, from list */
	int    fleet;			/* fleet for searches */
    	char   pointed_to;         	/* Flag if interval pointer set for this item */
	struct timers *prev;            /* Previous item in queue */
  	struct timers *next;            /* Next item in queue */
	struct timers *next_call_msg;   /* Next item in call chain or repeat message chain */
};

	
