/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: res_periods.c,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 2001/03/12 16:41:16 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/common/res_periods.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: res_periods.c,v 1.4 2001/03/12 16:41:16 taxiadm Exp $";

#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include "path.h"
#include "language.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "mad_ipc.h"
#include "mad_error.h"
#include "df_maint.h"
#include "ex_key.h"
#include "timers.h"
#include "enhance.h"
#include "switch_ext.h"



int
RES_get_period( day_of_week, time_of_day, skew, offset)
     int day_of_week;
     TIME_OF_DAY time_of_day;
     float *skew;
     struct offsets *offset;
{
  int i;
  RES_PERIODS *day;
  RES_PERIODS *res_periods;
  int rs;
  RP_REC rp_rec;
  
  if ( skew == NULL )
    return( -1 );
  
  if ( ( day_of_week < 0 ) ||
      ( day_of_week >= RP_DAYS_PER_WEEK ))
    return( -1 );
  
  if ( offset )
    {
      res_periods = offset->res_period;
      day = &res_periods[day_of_week];
      for ( i = 0; i < day->end; i++ )
	{
	  if ( ( day->info[i].start_time <= time_of_day ) &&
	      ( time_of_day <= day->info[i].end_time ) )
	    {
	      *skew = day->info[i].lead_time_skew;
	      return( 1 );
	    }
	}
    }
  return ( -1);  /** No matching period defined **/
}


int
Holiday_is_holiday( struct tm *tm, struct offsets *offset )
{

  HOLIDAYS   *holidays;
  int        i;
  
  if ( offset )
    {
      holidays = offset->Holidays;

      for ( i = 0; i < MAX_HOLIDAYS; i++ )
	{
	  if ( tm->tm_year >= 100 )
	    tm->tm_year -= 100;
	  if ( holidays[i].year == tm->tm_year )
	    if ( holidays[i].mon == tm->tm_mon + 1 )
	      if ( holidays[i].day == tm->tm_mday )
		return ( 1 );
	}
    }
    return( 0 );
}

