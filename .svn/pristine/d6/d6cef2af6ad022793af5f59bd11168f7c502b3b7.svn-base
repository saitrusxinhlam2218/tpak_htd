/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Timer_private.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2004/05/12 16:25:38 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Timer_private.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Timer_private.h,v $	$Revision: 1.3 $"

typedef struct timers 
  {
	short			type;
	long			expire_time;
	int				repeat_interval;
  	struct timers 	*next; 
	int				(*notify_func)();
	char			*data;
	int				unique_id;
  } TIMER;

	
#define NULL_TIMER			(TIMER *)0
#define NULL_TIMER_HANDLE	(TIMER_HANDLE)0

#define	MAX_TIMERS			10000
#define	BLOCK_OF_TIMERS		1000
#define	MAX_MEMORY_BLOCKS	10

#define	FOR_EACH_TIMER(start, work) \
			for (work = start; work; work = work->next)

#define TIMER_OUT_FILE	"/usr/taxi/run/traces/timers.log"

/*
 *	functions
 */

static TIMER_HANDLE add_timer ARGS((int type, int expire_time, int repeat_interval, int (*notify_func )(), char *data));
static RET_STATUS destroy_timer ARGS((TIMER *timer));
static RET_STATUS add_timer_to_free_timers ARGS((TIMER *timer));
static RET_STATUS zero_timer ARGS((TIMER *timer));
static TIMER *get_new_timer ARGS((void));
static TIMER *allocate_timer ARGS((int number));
static RET_STATUS free_timer_memory ARGS((void));
static long timer_time ARGS((long *set_time));
int timer_compare ARGS((TIMER *timer1, TIMER *timer2));
char *time_string ARGS((long long_time));
