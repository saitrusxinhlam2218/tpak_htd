static char sz__FILE__[]="@(#)local_date_time.c	10.1(ver) Date Release 12/2/94" ;
/***************************************************************************
*
*                           S C C S   I N F O                               
*                                                                          
*       @(#)  local_date_time.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:33:00
*       @(#)  Last delta: 12/2/94 at 11:37:15
*       @(#)  SCCS File: /taxi/sccs/s.local_date_time.c
*                                                                  
****************************************************************************
*
*       history:  date       author        revision
*                 03/22/90   r.barnett     added for internationalization
*
****************************************************************************
*
*       this file contains the following routines:
*
*       local_date_time()      formats date &/or time according to locale
*
***************************************************************************/

#include <time.h>

/***************************************************************************
*
*    local_date_time()  POSIX time formatting routine strftime() will take
*                       a tm structure, and, based on the Language Support
*                       Data Base's String Table, format the date &/or time
*                       according to local (national language) conventions.
*
*    inputs:  dt_tm
*
*    outputs: local_date
*             local_time
*             local_dt_tm
*
*    usage:   local_date_time(dt_tm, local_date, local_time, local_dt_tm)
*             no returned values
*
*    restrictions: the locale's date and time formats must be declared in
*                  the Language Support Data Base's String Table, compiled
*                  by ic and announced to the system by setlocale().
*
*    written: r.barnett  3/22/90
*
***************************************************************************/
void local_date_time(dt_tm, local_date, local_time, local_dt_tm)
time_t dt_tm;
char *local_date;
char *local_time;
char *local_dt_tm;
{

   char *date_format = "%d.%m.%y";
   char *date_time_format = "%c";
   int maxsize = 40;
   char *time_format = "%X";
   struct tm *tm_nl; 
   int x;

   tm_nl = localtime(&dt_tm);
   if (local_date)
     x = strftime(local_date, maxsize, date_format, tm_nl); 
   if (local_time)
     x = strftime(local_time, maxsize, time_format, tm_nl); 
   if (local_dt_tm)
     x = strftime(local_dt_tm, maxsize, date_time_format, tm_nl); 

}
