static char sz__FILE__[]="@(#)map_dt_tm.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  map_dt_tm.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:22:03
 * @(#)  Last delta: 12/14/89 at 17:10:24
 * @(#)  SCCS File: /taxi/sccs/s.map_dt_tm.c
 *                                                                           *
 *****************************************************************************/

#include "rep_strdefs.h"

/*====================================================================
= map_dt_tm() -
=	Convert date and time strings to YYMMDDhhmmss.
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=====================================================================*/
int map_dt_tm(start_date,start_time,dt_tm_buff)
char *start_date;
char *start_time;
char *dt_tm_buff;	/* date and time buffer */
{
	/* prepare the date_time buffer */
	dt_tm_buff[0]=start_date[6]; /* yy */
	dt_tm_buff[1]=start_date[7];
	dt_tm_buff[2]=start_date[0]; /* mm */
	dt_tm_buff[3]=start_date[1];
	dt_tm_buff[4]=start_date[3]; /* dd */
	dt_tm_buff[5]=start_date[4];
	dt_tm_buff[6]=start_time[0]; /* hh */
	dt_tm_buff[7]=start_time[1];
	dt_tm_buff[8]=start_time[3]; /* mm */
	dt_tm_buff[9]=start_time[4];
	dt_tm_buff[10]=start_time[6]; /* ss */
	dt_tm_buff[11]=start_time[7];
	dt_tm_buff[12]='\0';
	return(1);
} /* end map_dt_tm */
