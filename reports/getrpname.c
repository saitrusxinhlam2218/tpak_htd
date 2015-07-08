static char sz__FILE__[]="@(#)getrpname.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  getrpname.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:21:59
 * @(#)  Last delta: 12/14/89 at 17:08:12
 * @(#)  SCCS File: /taxi/sccs/s.getrpname.c
 *                                                                           *
 *****************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include "path.h"
#include "rep_strdefs.h"

/*====================================================================
= get_rp_name()
=	return a string such as: A10.880120
=	designating A=fleet_id, 10=report number, and date as yymmdd.
=====================================================================*/
char *get_rp_name (fleet_id,report_no,end_date)
char fleet_id;
int report_no;
char *end_date;
{
	static char report_name[16];

	sprintf(report_name,"%c%02d.%s",fleet_id,report_no,end_date);
	report_name[4]=end_date[0];	/* month m1 */
	report_name[5]=end_date[1];	/* month m2 */
	report_name[6]=end_date[3];	/* day d1 */
	report_name[7]=end_date[4];	/* day d2 */
	report_name[8]=end_date[6];	/* year y1 */
	report_name[9]=end_date[7];	/* year y2 */
	report_name[10]='\0';
	return(report_name);
}	/* end of report_fleet */
