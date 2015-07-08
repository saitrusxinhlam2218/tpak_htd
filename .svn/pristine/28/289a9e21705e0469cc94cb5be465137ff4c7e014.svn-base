static char sz__FILE__[]="@(#)dp_vh_dtl.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  dp_vh_dtl.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:48:25
 * @(#)  Last delta: 12/2/94 at 18:10:14
 * @(#)  SCCS File: /taxi/sccs/s.dp_vh_dtl.c
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "time.h"
#include "df_maint.h"
#include "call_entry.h"
#include "ui_def.h"

extern	struct cisam_vh srch_vh_calls[];
dsp_vh_detail(line_no) 					/* display the line info over the detail area */
int	line_no;
{
    	char dt_buf[10];							/* date buffer for conversion */
    	char tm_buf[7];								/* time buffer for conversion */

	attrset(A_REVERSE | A_BOLD);

	if(srch_vh_calls[line_no].last_signed_on > 0) {
		get_asc_time(srch_vh_calls[line_no].last_signed_on, dt_buf, tm_buf);
		mvprintw(1,10,"%8.8s", dt_buf);
		mvprintw(1,20,"%6.6s", tm_buf);
	}

	if(srch_vh_calls[line_no].last_emerg_on > 0) {
		get_asc_time(srch_vh_calls[line_no].last_emerg_on, dt_buf, tm_buf);
		mvprintw(1,42,"%8.8s", dt_buf);
		mvprintw(1,51,"%6.6s", tm_buf);
	}

	if(srch_vh_calls[line_no].susp_from_dt_tm > 0) {
		mvaddstr(2,60,srch_vh_calls[line_no].susp_from_date);
		mvaddstr(2,70,srch_vh_calls[line_no].susp_from_time);
	}

	if(srch_vh_calls[line_no].susp_d_from_dt_tm > 0) {
		mvaddstr(1,98,srch_vh_calls[line_no].susp_d_from_date);
		mvaddstr(1,108,srch_vh_calls[line_no].susp_d_from_time);
	}

	if(srch_vh_calls[line_no].susp_x_from_dt_tm > 0) {
		mvaddstr(2,98,srch_vh_calls[line_no].susp_x_from_date);
		mvaddstr(2,108,srch_vh_calls[line_no].susp_x_from_time);
	}

	if(srch_vh_calls[line_no].last_signed_off > 0) {
		get_asc_time(srch_vh_calls[line_no].last_signed_off, dt_buf, tm_buf);
		mvprintw(2,10,"%8.8s", dt_buf);
		mvprintw(2,20,"%6.6s", tm_buf);
	}

	if(srch_vh_calls[line_no].last_emerg_off > 0) {
		get_asc_time(srch_vh_calls[line_no].last_emerg_off, dt_buf, tm_buf);
		mvprintw(2,42,"%8.8s", dt_buf);
		mvprintw(2,51,"%6.6s", tm_buf);
	}

	if(srch_vh_calls[line_no].susp_to_dt_tm > 0) {
		mvaddstr(2,78,srch_vh_calls[line_no].susp_to_date);
		mvaddstr(2,88,srch_vh_calls[line_no].susp_to_time);
	}

	if(srch_vh_calls[line_no].susp_d_to_dt_tm > 0) {
		mvaddstr(1,115,srch_vh_calls[line_no].susp_d_to_date);
		mvaddstr(1,125,srch_vh_calls[line_no].susp_d_to_time);
	}

	if(srch_vh_calls[line_no].susp_x_to_dt_tm > 0) {
		mvaddstr(2,115,srch_vh_calls[line_no].susp_x_to_date);
		mvaddstr(2,125,srch_vh_calls[line_no].susp_x_to_time);
	}

	mvaddch(3,36,srch_vh_calls[line_no].seals[0]);
	mvaddch(3,42,srch_vh_calls[line_no].seals[1]);
	mvaddch(3,48,srch_vh_calls[line_no].seals[2]);
	mvaddch(3,54,srch_vh_calls[line_no].seals[3]);
	mvaddch(3,60,srch_vh_calls[line_no].seals[4]);
	mvaddch(3,66,srch_vh_calls[line_no].seals[5]);
	mvaddch(3,72,srch_vh_calls[line_no].seals[6]);
	mvaddch(3,78,srch_vh_calls[line_no].seals[7]);
	mvaddch(3,84,srch_vh_calls[line_no].seals[8]);
	mvaddch(3,90,srch_vh_calls[line_no].seals[9]);
	mvaddch(3,96,srch_vh_calls[line_no].seals[10]);
	mvaddch(3,102,srch_vh_calls[line_no].seals[11]);
	mvaddch(3,108,srch_vh_calls[line_no].seals[12]);
	mvaddch(3,114,srch_vh_calls[line_no].seals[13]);
	mvaddch(3,120,srch_vh_calls[line_no].seals[14]);
	mvaddch(3,126,srch_vh_calls[line_no].seals[15]);

	attrset(0);
}
