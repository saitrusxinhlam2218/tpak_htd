static char sz__FILE__[]="@(#)dsp_dr_sum.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  dsp_dr_sum.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:48:39
 * @(#)  Last delta: 12/2/94 at 17:56:00
 * @(#)  SCCS File: /taxi/sccs/s.dsp_dr_sum.c
 *                                                                           *
 *****************************************************************************/


#include <cursesX.h>
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "time.h"
#include "df_maint.h"
#include "ui_def.h"
#define SRCH_EXTERNS
#include "srch.h"
#include "call_entry.h"
#include "language.h"
#include "enhance.h"
#include "switch_ext.h"

#define ROOKIE '2'

char    c_string[21];
extern int page_num;
#define		SEARCHES_ON_PG	17			/* max call summary on a pg */
extern int matches_index;				/* for matches function */
extern struct fleets *fleet[];
extern	struct cisam_dr srch_dr_calls[];
extern char dr_calls_fleet[SEARCHES_ON_PG];

/* display the line info over the summary area */
disp_dr_sum(got_call)
int got_call;
{
	int   s_loop;


	attrset(A_REVERSE);
	sprintf(c_string, "%3d", page_num);
	mvaddstr(5,128,c_string);
	attrset(0);
	for (s_loop=0; s_loop<got_call; s_loop++)
	{
		dsp_dr_sum_line(s_loop);
	}
}

/* display the info for each summary line */
dsp_dr_sum_line(line_no)
int line_no;
{
char    temp_str[21];
int     indx;
long srch_time;
extern struct tm *localtime();
struct tm *srch_tp;
int     i, space;


	indx = find_findex(srch_dr_calls[line_no].fleet);
	sprintf(c_string, "%2d", line_no+1);
	mvaddstr(6+line_no,1,c_string);
	mvaddch(6+line_no,4,srch_dr_calls[line_no].fleet);
	/* DRV# */
	if (srch_dr_calls[line_no].id != 0)
	{
		sprintf(c_string, "%5d",srch_dr_calls[line_no].id);
		mvaddstr(6+line_no,7,c_string);
	}
	/* VEH# */
	if (srch_dr_calls[line_no].last_veh_nbr > 0)
	{
		sprintf(c_string, "%4d",srch_dr_calls[line_no].last_veh_nbr);
		mvaddstr(6+line_no,13,c_string);
	}
	mvaddstr(6+line_no,19,srch_dr_calls[line_no].name);
	mvaddstr(6+line_no,46,srch_dr_calls[line_no].license_nbr);
	/* License Renew */

	
	mvaddstr(6+line_no,63,srch_dr_calls[line_no].taxi_license);

        /* ATTRIBUTES */
        space = 0;
        for ( i = 0; i < 32; i++ )
	  {
	    if (srch_dr_calls[line_no].attributes[i] == YES && fleet[indx]->vd_attr_info[i].drv_attr[0] != '\0')
	      {
		mvaddstr(6+line_no,74 + space,fleet[indx]->vd_attr_info[i].drv_attr);
		space += 3;
	      }
	    if ( space + 74 > 132 )
	      break;
	  }

#ifdef FOO
	if (srch_dr_calls[line_no].class == ROOKIE)
		mvaddch(6+line_no,122,YES);
	else
		mvaddch(6+line_no,122, NO);
#endif
} /* end of dsp_dr_sum_line(line_no) */

/* hilite a line over summary area */
hilite_dr_line(detail_line)
int detail_line;
{
	/* set reverse attributes */
	attrset(A_REVERSE | A_BOLD);
	mvaddstr(6+detail_line,0,"                                                                                                                                    \0");
	dsp_dr_sum_line(detail_line); /* set reverse attributes */
	attrset(0);
}

/* unhilite a line over summary area */
unhilite_dr_line(detail_line)
int detail_line;
{
	mvaddstr(6+detail_line,0,"                                                                                                                                    \0");
	refresh();
	dsp_dr_sum_line(detail_line);
}
