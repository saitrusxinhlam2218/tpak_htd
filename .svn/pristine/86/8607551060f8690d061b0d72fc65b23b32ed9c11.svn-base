static char sz__FILE__[]="@(#)dsp_us_sum.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  dsp_us_sum.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:48:43
 * @(#)  Last delta: 12/2/94 at 18:02:23
 * @(#)  SCCS File: /taxi/sccs/s.dsp_us_sum.c
 *                                                                           *
 *****************************************************************************/


#include <cursesX.h>
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "ui_def.h"
#define SRCH_EXTERNS
#include "srch.h"
#include "call_entry.h"
#include "language.h"

extern int matches_index;	/* for matches function */
extern struct fleets *fleet[];
extern int page_num;	
extern	struct cisam_up srch_up_calls[];
extern  char *Spaces;

/* display the line info over the summary area */
disp_usr_sum(got_call)
int got_call;
{
        int   s_loop;

	attrset(A_REVERSE);
	mvprintw(5,128, "%3d", page_num);
	attrset(0);
	for (s_loop=0; s_loop<got_call; s_loop++)
	{
		dsp_us_sum_line(s_loop);
	}
}

/* display the info for each sumary line */
dsp_us_sum_line(line_no)
int line_no;
{

	mvprintw(6+line_no,1, "%2d", line_no+1);
	mvprintw(6+line_no,4, "%4d",srch_up_calls[line_no].user_id);
	mvaddstr(6+line_no,10,srch_up_calls[line_no].user_name);
        mvaddstr(6+line_no,37,srch_up_calls[line_no].fullname); 
	mvaddstr(6+line_no,61,srch_up_calls[line_no].greeting_name);

	if (srch_up_calls[line_no].fleet_perms[0] == YES)
		mvaddch(6+line_no,75, fleet[0]->fleet_id);
	if (srch_up_calls[line_no].fleet_perms[1] == YES)
		mvaddch(6+line_no,77, fleet[1]->fleet_id);
	if (srch_up_calls[line_no].fleet_perms[2] == YES)
		mvaddch(6+line_no,79, fleet[2]->fleet_id);
	if (srch_up_calls[line_no].fleet_perms[3] == YES)
		mvaddch(6+line_no,81, fleet[3]->fleet_id);
	if (srch_up_calls[line_no].fleet_perms[4] == YES)
		mvaddch(6+line_no,83, fleet[4]->fleet_id);
	if (srch_up_calls[line_no].fleet_perms[5] == YES)
		mvaddch(6+line_no,85, fleet[5]->fleet_id);
	if (srch_up_calls[line_no].fleet_perms[6] == YES)
		mvaddch(6+line_no,87, fleet[6]->fleet_id);
	if (srch_up_calls[line_no].fleet_perms[7] == YES)
		mvaddch(6+line_no,89, fleet[7]->fleet_id);

} /* end of dsp_usr_sum_line(line_no) */

/* hilite a line over summary area */
hilite_us_line(detail_line)
int detail_line;
{
	/* set reverse attributes */
	attrset(A_REVERSE | A_BOLD);
	mvaddstr(6+detail_line, 0 , Spaces);
	dsp_us_sum_line(detail_line); /* set reverse attributes */
	attrset(0);
}

/* unhilite a line over summary area */
unhilite_us_line(detail_line)
int detail_line;
{
	mvaddstr(6+detail_line, 0, Spaces);
	refresh();
	dsp_us_sum_line(detail_line);
}
