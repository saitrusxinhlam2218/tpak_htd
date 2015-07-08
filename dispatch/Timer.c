/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Timer.c,v $
*  @(#)  $Revision: 1.7 $
*  @(#)  $Date: 2004/05/12 16:03:39 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Timer.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Timer.c,v 1.7 2004/05/12 16:03:39 jwelch Exp $";

/*-----------------------------------------------------------
 *	timer.c
 *-----------------------------------------------------------
 */

/*
 *	include
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <varargs.h>
#include <values.h>
#include <unistd.h>

#include "taxipak.h"
#include "mad_ipc.h"
#include "Object.h"
#include "Timer_public.h"
#include "LL.h"
#include "List_public.h"
#include "Timer_private.h"

/*
 *	fix - use ERROR from TaxiPak
 */
#define	ERROR		printf

/*
 *	globals
 */
static LinkedList 	 	*active_timers = NULL;
static TIMER			*free_timers = NULL_TIMER;
static TIMER			*timer_memory[MAX_MEMORY_BLOCKS];
static short			already_initialized = FALSE;

static short	 		timer_debug = FALSE;

extern time_t                   mads_time;
extern struct offsets *offset;				/* offsets of structures in shared memory */

/*----------------------------------------------------------
 *====>	PUBLIC FUNCTIONS
 */

/*-----------------------------------------------------------
 *==>	Timer services start
 *-----------------------------------------------------------
 */
RET_STATUS
Timer_services_start(application_time)
	long			*application_time;
{
	int				 i;

	if (already_initialized)
		return(FAIL);

	timer_debug_mode(0);

	for (i = 0; i < MAX_MEMORY_BLOCKS; i++)
		timer_memory[i] = NULL_TIMER;

	free_timers = allocate_timer(MAX_TIMERS);

	if (free_timers == NULL_TIMER)
	  {
		ERROR("Error Timer_services_start - couldn't initialize timers\n");
		return(FAIL);
	  }

	if ( (active_timers = LL_new() ) == NULL )
	  {
		ERROR("Error Timer_services_start - couldn't create list\n");
		return(FAIL);
	  }

	timer_time(application_time);

	already_initialized = TRUE;

	return(SUCCESS);
}

/*-----------------------------------------------------------
 *==>	Timer services end
 *-----------------------------------------------------------
 */
RET_STATUS
Timer_services_end()
{
	TIMER			*timer;


	if (!already_initialized)
		return(FAIL);

	if ( LL_Destroy(active_timers) != SUCCESS )
	  {
		ERROR("Error Timer_services_end - couldn't destroy list\n");
		return(FAIL);
	  }

	if (free_timer_memory() != SUCCESS)
	  {
		ERROR("Error Timer_services_end - couldn't free timers\n");
		return(FAIL);
	  }

	free_timers = NULL_TIMER;

	already_initialized = FALSE;

	return(SUCCESS);
}

/*-----------------------------------------------------------
 *==>	Timer create
 *-----------------------------------------------------------
 */
TIMER_HANDLE
Timer_create(va_alist)
	va_dcl
{
	va_list		 	ap;
	int				parameter;
	short		 	type = 0;
	int		 		expire_time = 0;
	int			 	repeat_interval = 0;
	int				(*notify_func)() = NULL;
	char			*data = NULL;
	TIMER_HANDLE            hTimer;


	if (!already_initialized)
		return(NULL_TIMER_HANDLE);

	va_start(ap);

	while((parameter = va_arg(ap, int)) != ATTRIBUTE_END)
	  {
		switch(parameter)
		  {
			case TIMER_TYPE:
				type = va_arg(ap, int);
				break;

			case TIMER_EXPIRE_TIME:
				expire_time = va_arg(ap, int);
				break;

			case TIMER_REPEAT_INTERVAL:
				repeat_interval = va_arg(ap, int);
				break;

			case TIMER_NOTIFY_FUNC:
				notify_func = (int (*)())va_arg(ap, char *);
				break;

			case TIMER_DATA:
				data = va_arg(ap, char *);
				break;

			default:
				ERROR("Error Timer_create - %s (type = %d) - %s\n", 
						"badly formed arg list", type, "timer not created");
				return(NULL_TIMER_HANDLE);
				break;
		  }
	  }

	va_end(ap);

	if (expire_time == 0)
	  {
		ERROR("Error Timer_create - no expire time (type = %d) \n", type);
		return(NULL_TIMER_HANDLE);
	  }

	if (notify_func == NULL)
	  {
		ERROR("Error Timer_create - no notify function (type = %d)\n", type);
		return(NULL_TIMER_HANDLE);
	  }

	if (timer_debug)
	  {
	    hTimer = add_timer(type, expire_time, repeat_interval, notify_func, data);
	    timer_debug_print("adding timer %x type %d expire %d secs repeat %d\n",
				hTimer, type, expire_time, repeat_interval);
	    return( hTimer );
	  }
	else
	  return(add_timer(type, expire_time, repeat_interval, notify_func, data));
}

/*-----------------------------------------------------------
 *==>	Timer destroy
 *-----------------------------------------------------------
 */
RET_STATUS
Timer_destroy(timer)
	TIMER_HANDLE	 timer;
{

	if (!already_initialized)
		return(FAIL);

	if (!timer)
	  {
		ERROR("Error Timer_destroy - NULL timer\n");
		return(FAIL);
	  }



	return(destroy_timer((TIMER *)timer));
}

/*-----------------------------------------------------------
 *==>	Timer service
 *-----------------------------------------------------------
 */
int
Timer_service(number_of_timers)
	int			 number_of_timers;
{
	int			 i,
				 count = 0;
	TIMER		*service_timer;
	long		 check_time;

	timer_debug = TRUE;

	if (!already_initialized)
		return(count);

	if (number_of_timers <= 0 && number_of_timers != TIMER_ALL_TIMERS)
	  {
		ERROR("Error Timer_service - number of timers to check <= 0\n");
		return(count);
	  }

	if ((TIMER *)LL_GetFirst(active_timers) == NULL_TIMER)
	  return(count);

	if (number_of_timers == TIMER_ALL_TIMERS)
		number_of_timers = MAXINT;

	check_time = timer_time(NULL);

	for (i = 0; i < number_of_timers; i++)
	  {
/*
 *		check here for active_timers because notify function and destroy_timer
 *			can cause active_timers to become NULL
 */

	    service_timer = (TIMER *)LL_GetFirst(active_timers);

		if (service_timer == NULL_TIMER)
			return(count);
	
		if (service_timer->expire_time <= check_time)
		  {

/*
 *			call notify function
 */
			service_timer->notify_func(service_timer->data);

/*
 *			reset or delete timer
 *				NOTE: this must take place after the notify so the 
 *					individual notify functions can get values from 
 *					the timer until they return control back here
 */
			if (service_timer->repeat_interval)
			  {
			    LL_Remove( active_timers, service_timer );
			    
			    service_timer->expire_time = (service_timer->expire_time + 
							  service_timer->repeat_interval);
			    LL_PriorityEnqueue(active_timers, (void *)service_timer, timer_compare);
			  }
			else
			  {
				destroy_timer((TIMER *)service_timer);
			  }

			count++;
		  }
		else
			return(count);
	  }
	
	return(count);
}

/*-----------------------------------------------------------
 *==>	Timer get value
 *-----------------------------------------------------------
 */
ATTR_VALUE
Timer_get_value(timer, attribute)
	TIMER_HANDLE	timer;
	ATTRIBUTE		attribute;
{

	if (!already_initialized)
		return(NULL_TIMER_HANDLE);

	if (timer == NULL)
	  {
		ERROR("Error Timer_get_value - %s\n", "no timer");
		return(ATTR_VALUE_NULL);
	  }

	if (timer_debug)
		timer_debug_print("timer get %x  attribute %d\n", timer, attribute);

	switch(attribute)
	  {
		case TIMER_TYPE:
			return((ATTR_VALUE)((TIMER *)timer)->type);
			break;

		case TIMER_EXPIRE_TIME:
			return((ATTR_VALUE)((TIMER *)timer)->expire_time);
			break;

		case TIMER_REPEAT_INTERVAL:
			return((ATTR_VALUE)((TIMER *)timer)->repeat_interval);
			break;

		case TIMER_NOTIFY_FUNC:
			return((ATTR_VALUE)((TIMER *)timer)->notify_func);
			break;

		case TIMER_DATA:
			return((ATTR_VALUE)((TIMER *)timer)->data);
			break;

		default:
			ERROR("Error Timer_get_value - %s\n", "invalid attribute");
			return(ATTR_VALUE_NULL);
			break;
	  }

	return(ATTR_VALUE_NULL);
}

/*-----------------------------------------------------------
 *==>	Timer set value
 *-----------------------------------------------------------
 */
RET_STATUS
Timer_set_value(va_alist)
	va_dcl
{
	va_list		 ap;
	int			 parameter;
	TIMER		*timer;
	long		 now_time;
	short	 	 type = -99,
				 new_value_set = FALSE;
	int			 expire_time = 0,
				 repeat_interval = -99;
	char		*data = NULL;
	int			 (*notify_function)() = NULL;


	if (!already_initialized)
		return(FAIL);

	va_start(ap);

	timer = (TIMER *)va_arg(ap, char *);

	if (!timer)
		return(FAIL);

	if (timer_debug)
		timer_debug_print("timer set %x\n", timer);

	while((parameter = va_arg(ap, int)) != ATTRIBUTE_END)
	  {
		switch(parameter)
		  {
			case TIMER_TYPE:
				type = va_arg(ap, int);
				break;

			case TIMER_EXPIRE_TIME:
				expire_time = va_arg(ap, int);
				if (expire_time <= 0)
	  			  {
					ERROR("Error Set_timer - %s %x (type = %d)\n",
							"expire time has past on",
							timer,
							timer->type);
					return(FAIL);
	  			  }
				break;

			case TIMER_NOTIFY_FUNC:
				notify_function = (int (*)())va_arg(ap, char *);
				if (notify_function == NULL)
	  			  {
					ERROR("Error Set_timer - %s (type = %d)\n",
							"can't set NULL notify func",
							timer->type);
					return(FAIL);
	  			  }
				break;

			case TIMER_REPEAT_INTERVAL:
				repeat_interval = va_arg(ap, int);
				if (repeat_interval < 0)
	  			  {
					ERROR("Error Set_timer - repeat interval must be > 0\n");
					return(FAIL);
	  			  }
				break;

			case TIMER_DATA:
				data = va_arg(ap, char *);
				break;

			default:
				ERROR("Error Timer_set_value - %s\n", "badly formed arg list");
				return(FAIL);
				break;
		  }
	  }

	va_end(ap);

	if (type != -99)
	  {
		timer->type = type;
		new_value_set = TRUE;
	  }
	if (notify_function != NULL)
	  {
		timer->notify_func = notify_function;
		new_value_set = TRUE;
	  }
	if (repeat_interval != -99)
	  {
		timer->repeat_interval = repeat_interval;
		new_value_set = TRUE;
	  }
	if (data != NULL)
	  {
		timer->data = data;
		new_value_set = TRUE;
	  }
	if (expire_time > 0)
	  {
	    LL_Remove(active_timers, (void *)timer);
	    timer->expire_time = timer_time(NULL) + expire_time;
	    LL_PriorityEnqueue(active_timers, (void *)timer, timer_compare);
	    new_value_set = TRUE;
	  }

	if (new_value_set)
		return(SUCCESS);
	else
		return(FAIL);
}

/*----------------------------------------------------------
 *====>	PRIVATE FUNCTIONS
 */

/*-----------------------------------------------------------
 *==>	add timer
 *-----------------------------------------------------------
 */
static
TIMER_HANDLE
add_timer(type, expire_time, repeat_interval, notify_func, data)
	short		type;
	int			expire_time;
	int			repeat_interval;
	int			(*notify_func)();
	char		*data;
{
	TIMER		*new_timer;

/*
 *	validate 
 */

	if (expire_time <= 0)
	  {
		ERROR("Error add_timer - expire time past (type = %d) - %s\n",
				type, "timer not created");
		return(NULL_TIMER_HANDLE);
	  }

	if (!notify_func)
		return(NULL_TIMER_HANDLE);

/*
 *	build timer
 */
	if ((new_timer = get_new_timer()) == NULL_TIMER)
	  {
		ERROR("Error add_timer - couldn't allocate timer (type = %d) - %s\n",
				type, "timer not created");
		return(NULL_TIMER_HANDLE);
	  }

	new_timer->type = type;
	new_timer->expire_time = timer_time(NULL) + expire_time;
	new_timer->repeat_interval = repeat_interval;
	new_timer->notify_func = notify_func;
	new_timer->data = data;

	if (LL_PriorityEnqueue(active_timers, (void *)new_timer, timer_compare) != SUCCESS)
	  {

		add_timer_to_free_timers(new_timer);
		return(NULL_TIMER_HANDLE);
	  }

	return((TIMER_HANDLE)new_timer);
}

/*-----------------------------------------------------------
 *==>	destroy timer
 *-----------------------------------------------------------
 */
static
RET_STATUS
destroy_timer(timer)
	TIMER		*timer;
{
	TIMER		*current_timer,
				*previous_timer = NULL_TIMER,
				*last_free_timer;


	if (!timer)
		return(FAIL);

	
	if ( LL_Remove(active_timers, (void *)timer) != timer )
	  {
		ERROR("Error destroy_timer - timer %x not found (type = %d) - %s\n",
				timer, timer->type, "timer not destroyed");
		return(FAIL);
	  }

	zero_timer(timer);

	add_timer_to_free_timers(timer);

	return(SUCCESS);
}

/*-----------------------------------------------------------
 *==>	add timer to free timers
 *-----------------------------------------------------------
 */
static
RET_STATUS
add_timer_to_free_timers(timer)
	TIMER		*timer;
{
	TIMER		*last_free_timer;


	if (!timer)
		return(FAIL);

	if (!free_timers)
	  {
		free_timers = timer;
		return(SUCCESS);
	  }

	timer->next = free_timers;
	free_timers = timer;

	return(SUCCESS);
}

/*-----------------------------------------------------------
 *==>	zero timer
 *-----------------------------------------------------------
 */
static
RET_STATUS
zero_timer(timer)
	TIMER		*timer;
{
	if (!timer)
		return(FAIL);

	timer->type = 0;
	timer->expire_time = (long)0;
	timer->repeat_interval = 0;
	timer->next = NULL_TIMER;
	timer->notify_func = NULL;
	timer->data = NULL;
	timer->unique_id = 0;

	return(SUCCESS);
}

/*-----------------------------------------------------------
 *==>	get new timer
 *-----------------------------------------------------------
 */
static
TIMER *
get_new_timer()
{
	TIMER		*new_timer;
	static int	 id_number = 0;


	if (!free_timers)
	  {
		ERROR("Error get_new_timer - allocating new block of %d timers\n",
				BLOCK_OF_TIMERS);

		free_timers = allocate_timer(BLOCK_OF_TIMERS);

		if (!free_timers)
			return(NULL_TIMER);
	  }

	new_timer = free_timers;
	free_timers = free_timers->next;
	new_timer->next = NULL_TIMER;

	id_number++;
	if (id_number >= MAXINT)
	  {
		ERROR("Error get_new_timer - reseting unique id number to 1\n");
		id_number = 1;
	  }

	new_timer->unique_id = id_number;

	return(new_timer);
}

/*-----------------------------------------------------------
 *==>	allocate timer
 *-----------------------------------------------------------
 */
static
TIMER *
allocate_timer(number)
	int				 number;
{
	TIMER			*timer,
					*previous_timer,
					*first_timer;
	int				 i;


	for (i = 0; i < MAX_MEMORY_BLOCKS; i++)
		if (timer_memory[i] == NULL_TIMER)
			break;

	timer_memory[i] = (TIMER *)malloc(sizeof(TIMER) * number);

	if (!timer_memory[i])
	  {
		ERROR("Error allocate_timer - couldn't allocate memory\n");
		return(NULL_TIMER);
	  }

	first_timer = timer_memory[i];
	timer = timer_memory[i];

	previous_timer = NULL_TIMER;

	for (i = 0; i < number; i++)
	  {
		zero_timer(timer);

		if (previous_timer)
			previous_timer->next = timer;

		previous_timer = timer;
		timer++;
	  }

	return(first_timer);
}

/*-----------------------------------------------------------
 *==>	free timer memory
 *-----------------------------------------------------------
 */
static
RET_STATUS 
free_timer_memory()
{
	int				 i;

	for (i = 0; i < MAX_MEMORY_BLOCKS; i++)
	  {
		if (timer_memory[i] == NULL_TIMER)
			break;

		free((TIMER *)timer_memory[i]);
		timer_memory[i] = NULL_TIMER;
	  }

	return(SUCCESS);
}

/*-----------------------------------------------------------
 *==>	timer compare
 *-----------------------------------------------------------
 */
int
timer_compare(timer1, timer2)
	TIMER			*timer1,
					*timer2;
{

	if (timer1->expire_time < timer2->expire_time)
		return(-1);
	else
	if (timer1->expire_time > timer2->expire_time)
		return(1);
	else
	  {
		if (timer1->unique_id < timer2->unique_id)
			return(-1);
		else
		if (timer1->unique_id > timer2->unique_id)
			return(1);
		else
			return(0);
	  }
}

/*-----------------------------------------------------------
 *==>	timer time
 *-----------------------------------------------------------
 */
static
long
timer_time(set_time)
	long			*set_time;
{
	static long		*application_time_variable = NULL;

	if (set_time)
		application_time_variable = set_time;

	if (application_time_variable != NULL)
		return(*application_time_variable);
	else
		return(time(NULL));
}

/*----------------------------------------------------------
 *====>	DEBUG FUNCTIONS
 */

/*-----------------------------------------------------------
 *==>	timer debug mode
 *-----------------------------------------------------------
 */

timer_debug_mode(on)
	short			on;
{
	timer_debug = on;

	return(TRUE);
}

/*-----------------------------------------------------------
 *==>	timer debug print
 *-----------------------------------------------------------
 */

timer_debug_print(fmt, p1, p2, p3, p4, p5, p6, p7, p8)
	char        *fmt;
	long         p1, p2, p3, p4, p5, p6, p7, p8;
{
  static FILE	*debug_fp = NULL;
	static 		 unable_to_open = FALSE;
	TIMER		*timer;	
	int count = 0;
	char      *ctime(), *stime;
	time_t    tmtime;
	char      time_string[64];
	char      new_format[512];

	if (unable_to_open)
		return(FALSE);

	tmtime = time( (time_t *) 0 );
	stime = ctime( &tmtime );
	

		if (!(debug_fp = (FILE *)fopen(TIMER_OUT_FILE, "a")))
		  {
			unable_to_open = TRUE;
			return(FALSE);
		  }


	FOR_EACH_TIMER(free_timers, timer)
	  {
		count++;
	  }
	
	sprintf( time_string, "%.15s Total timers: %d", &stime[4], count );
	sprintf( new_format, "%s %s", time_string, fmt );
	fprintf(debug_fp, new_format, p1, p2, p3, p4, p5, p6, p7, p8);


	fclose(debug_fp);

	return(TRUE);
}


/*-----------------------------------------------------------
 *==>	dump free timers
 *-----------------------------------------------------------
 */

dump_free_timers()
{
	TIMER		*timer;
	int			 count;


	if (!free_timers)
	  {
		return(FALSE);
	  }


	count = 0;
	FOR_EACH_TIMER(free_timers, timer)
	  {
		count++;
	  }


	return(TRUE);
}

/*-----------------------------------------------------------------
 *  time string
 *-----------------------------------------------------------------
 */
char *
time_string(long_time)
	time_t		long_time;
{
	struct tm   *localtime(), *timed;
	static char  time_string[10];

	if (!long_time)
		return("     ");

	timed = localtime(&long_time);

	//	sprintf(time_string, "%02d:%02d:%02d", 
	//		timed->tm_hour, timed->tm_min, timed->tm_sec);

	return(time_string);
}

#define T_REP_MSG 8
RET_STATUS
Timer_delete_rptmsg(msg_nbr)
   int msg_nbr;
/* Was del_timer_rptmsg */
{
	TIMER		*timer;

	if (!active_timers)
		return(FAIL);

	for (timer = (TIMER *)LL_GetFirst(active_timers);
	     timer != (TIMER *)HANDLE_NULL;
	     timer = (TIMER *)LL_GetNext(active_timers))
	  {
              if ((int) Timer_get_value(timer, TIMER_TYPE) == T_REP_MSG)
                 if ((int) Timer_get_value(timer, TIMER_DATA) == msg_nbr)
                 {
		   LL_Remove(active_timers, (void *)timer);
		   return(SUCCESS);
                 }
                    
	  }

	return(FAIL);
}                 /* Timer_delete_rptmsg */

void
stat_usr_drv_funcs()
{
#ifndef T_USR_DRV_STAT
#define T_USR_DRV_STAT     11
#endif
#ifndef DRV_STATS_PERIOD
#define DRV_STATS_FREQ  6    /* how many time per hr veh stats output */
#define DRV_STATS_PERIOD (3600 / DRV_STATS_FREQ)  /* how often veh stats output in sec */
#endif
  
  offset->stat_update = mads_time;
  wr_drv_stat( NULL );
  //  file_system_check();
  (void) Timer_create(
		      TIMER_TYPE, T_USR_DRV_STAT,
		      TIMER_EXPIRE_TIME, DRV_STATS_PERIOD,
		      TIMER_NOTIFY_FUNC, stat_usr_drv_funcs,
		      ATTRIBUTE_END);
}
