static char sz__FILE__[]="@(#)super_lead.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  super_lead.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:33:19
 * @(#)  Last delta: 12/2/94 at 11:45:13
 * @(#)  SCCS File: /taxi/sccs/s.super_lead.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include "mads_types.h"
#include "mad_error.h"
#include "mad_ipc.h"
#include "fleet.h"
#include "Res_periods_db.h"

struct fleets *fleet[FLEET_MAX];				/* Fleet array */

time_t
get_new_lead( time_t x, time_t due_time, short fleet_nbr, struct offsets *offset )
{
  INT	i, rc, time_of_day;
  struct tm	*tm;
  short	day_of_week, time_period;
  float skew;
  
  if (offset == NULL)
    return (0);
  
  tm = localtime( &due_time );
  day_of_week = (Holiday_is_holiday( tm, offset ) ) ? RES_HOLIDAY_IDX : tm->tm_wday;
  time_2_time_of_day( time(0), &time_of_day );
  
  if ( ( rc = RES_get_period( day_of_week, time_of_day, &skew, offset ) ) < 0 )
    skew = 1.0;
  else
    skew = 1.0;
  
  x = (int)( x * skew ) + ( fleet[fleet_nbr]->time_calls_lead_supp * 60 );
  return (x);
}

get_super_lead(x,super_zn,fleet_nbr)			/* get the super zone lead time percentage */
int x;
int super_zn;
int fleet_nbr;
{
	int i,flag;
	float new_lead=0;
	float percent;

	if( super_zn >= 0 && super_zn < 64000){
		flag = 0;
		for(i=0;i< MAX_SUP_ZONES; i++){
			if( fleet[fleet_nbr]->super_zone[i].nbr == super_zn){
				flag = 1;
				break;
			}
		}
	
		if(fleet[fleet_nbr]->super_zone[i].perc_lead_tm > 0 && fleet[fleet_nbr]->super_zone[i].perc_lead_tm < 999 && flag){
			percent = ((float)fleet[fleet_nbr]->super_zone[i].perc_lead_tm + 100.0) / 100.0; 
			new_lead = (float)x * percent;
		}
	}

	if(new_lead)
		return(new_lead);
	else
		return(x);
}

