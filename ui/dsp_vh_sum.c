static char sz__FILE__[]="@(#)dsp_vh_sum.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  dsp_vh_sum.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:48:46
 * @(#)  Last delta: 12/2/94 at 18:10:03
 * @(#)  SCCS File: /taxi/sccs/s.dsp_vh_sum.c
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

extern int page_num;
#define	SEARCHES_ON_PG	17 		/* max call summary on a pg */
extern int matches_index;		/* for matches function */
extern struct fleets *fleet[];
extern	struct cisam_vh srch_vh_calls[];
extern char vh_calls_fleet[SEARCHES_ON_PG];
char    c_string[21];

disp_vh_sum(got_call) 			/* display the line info over the summary area */
int got_call;
{
	int   s_loop;

	attrset(A_REVERSE);
	sprintf(c_string, "%3d", page_num);
	mvaddstr(5,128,c_string);
	attrset(0);
	for (s_loop=0; s_loop<got_call; s_loop++) {
		dsp_vh_sum_line(s_loop);
	}
}

dsp_vh_sum_line(line_no) 			/* display the info for each sumary line */
int line_no;
{
char    temp_str[21];
int     indx;
int     i, space;

	indx = find_findex(srch_vh_calls[line_no].fleet);
	sprintf(c_string, "%2d", line_no+1);
	mvaddstr(6+line_no,1,c_string);
	mvaddch(6+line_no,4,srch_vh_calls[line_no].fleet);
	if (srch_vh_calls[line_no].nbr) {
		sprintf(c_string, "%4d",srch_vh_calls[line_no].nbr);
		mvaddstr(6+line_no,7,c_string);
	}
	if (srch_vh_calls[line_no].last_driver) {
		sprintf(c_string, "%5d",srch_vh_calls[line_no].last_driver);
		mvaddstr(6+line_no,12,c_string);
	}
	mvprintw(6 + line_no,19,"%-5d",strtol(srch_vh_calls[line_no].mobile_rf_id,(char **)NULL, 16));
	mvaddstr(6+line_no,25,srch_vh_calls[line_no].license_nbr);
	mvaddstr(6+line_no,37,srch_vh_calls[line_no].make);
	mvaddstr(6+line_no,49,srch_vh_calls[line_no].model);
        mvaddstr(6+line_no,61,srch_vh_calls[line_no].color);
        mvaddstr(6+line_no,72,srch_vh_calls[line_no].telephone);
	mvaddch(6+line_no,88,srch_vh_calls[line_no].kdt_working);
	mvaddch(6+line_no,90,srch_vh_calls[line_no].meter_working);

        /* ATTRIBUTES */
        space = 0;
        for ( i = 0; i < ATTR_MAX; i++ )
	  {
	    if (srch_vh_calls[line_no].attr[i] == YES && fleet[indx]->vd_attr_info[i].veh_attr[i] != '0')
	      {
		mvaddstr(6+line_no,94 + space,fleet[indx]->vd_attr_info[i].veh_attr);
		space += 3;
	      }
	    if ( space + 94 > 132 )
	      break;
	  }
}

/* hilite a line over summary area */
hilite_vh_line(detail_line)
int detail_line;
{
	/* set reverse attr */
	attrset(A_REVERSE | A_BOLD);
	mvaddstr(6+detail_line,0,"                                                                                                                                    \0");
	dsp_vh_sum_line(detail_line); /* set reverse attr */
	attrset(0);
}

/* unhilite a line over summary area */
unhilite_vh_line(detail_line)
int detail_line;
{
	mvaddstr(6+detail_line,0,"                                                                                                                                    \0");
	refresh();
	dsp_vh_sum_line(detail_line);
}
