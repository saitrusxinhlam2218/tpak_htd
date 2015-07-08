static char sz__FILE__[]="@(#)dsp_cl_sum.c	10.1.2.2(ver) Date Release 1/30/95" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  dsp_cl_sum.c; Rel: 10.1.2.1; Get date: 12/6/94 at 14:38:30
 * @(#)  Last delta: 12/2/94 at 18:02:29
 * @(#)  SCCS File: /taxi/sccs/s.dsp_cl_sum.c
 *                                                                           *
 *****************************************************************************/

/* Make sure location constants are defined. Create a compile error if they
	 are not defined or both are defined. */
#ifdef SCANDINAVIA
#ifdef NAMERICA
ERROR: "Both compile time constants SCANDINAVIA and NAMERICA are defined."
#endif
#else
#ifdef NAMERICA
#else
ERROR: "Compile time constant SCANDINAVIA or NAMERICA undefined."
#endif
#endif

#include <cursesX.h>
#include <sys/types.h>

#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "call_entry.h"
#include "screens.h"
#include "enhance.h"
#include "switch_ext.h"

int page_num;
extern  struct cisam_cl srch_calls[];
extern  struct cisam_sc subs_calls[];
extern  int scr_name;
extern  char *Spaces;

/*--------------------------------------------------------------------
 *  display the line info over the summary area
 *--------------------------------------------------------------------
 */

disp_call_sum(got_call)
	int   got_call;
{
	int   s_loop;


	attrset(A_REVERSE);
	mvprintw(5,128, "%3d", page_num);
	attrset(0);

	for (s_loop = 0; s_loop < got_call; s_loop++)
		{
		dsp_sum_line(s_loop);
		}
}

/*--------------------------------------------------------------------
 *  display the info for each sumary line
 *--------------------------------------------------------------------
 */

dsp_sum_line(line_no)
	int   line_no;
{
	char    temp_str[21];
	char    temp_call_nbr[7];
	short i;


/*
 *  fix
 */
	if (scr_name == SUBS_FUTURE_CLS_SCR)
		{
		if (subs_calls[line_no].nbr <= 0)
			return;
		}
	else
	  {
		if (srch_calls[line_no].nbr < 0)
			return;
		}

	mvprintw(6+line_no,1, "%2d", line_no+1);

	if (scr_name == SUBS_FUTURE_CLS_SCR)
		{
		mvaddch(6+line_no,4,subs_calls[line_no].fleet);
		mvaddstr(6+line_no,24,subs_calls[line_no].extended_type);
		if (subs_calls[line_no].pckup_zone)
			{
			mvprintw(6+line_no,35, "%3d",subs_calls[line_no].pckup_zone);
			}
		mvaddstr(6+line_no,39,subs_calls[line_no].pckup_city);

#ifdef SCANDINAVIA
		if (0)
			mvprintw(6 + line_no, 43, "%20s %-6d ",
					subs_calls[line_no].pckup_str_name,
					subs_calls[line_no].pckup_str_nbr);
		else
			mvprintw(6 + line_no, 43, "%20s %-6d%c",
					subs_calls[line_no].pckup_str_name,
					subs_calls[line_no].pckup_str_nbr,
					subs_calls[line_no].pckup_str_suf);
#else
		mvprintw(6+line_no,43, "%d",subs_calls[line_no].pckup_str_nbr);
		if ( subs_calls[line_no].pckup_str_suf && !(0) )
			mvaddch(6+line_no,49,subs_calls[line_no].pckup_str_suf);
		mvaddstr(6+line_no,51,subs_calls[line_no].pckup_pre_dir);
		mvaddstr(6+line_no,54,subs_calls[line_no].pckup_str_name);
		mvaddstr(6+line_no,75,subs_calls[line_no].pckup_str_type);
		mvaddstr(6+line_no,79,subs_calls[line_no].pckup_post_dir);
#endif

		mvaddstr(6+line_no,85,subs_calls[line_no].passenger_name);
		strncpy(temp_str,subs_calls[line_no].pckup_adr_cmnt,20);
		temp_str[20] = '\0';
		mvaddstr(6+line_no,106,temp_str);

		/* get last six digits of the call number */
		sprintf(temp_str,"%d",subs_calls[line_no].nbr);

		for(i=0; temp_str[i] != '\0'; i++);

		if(i > 5)
			strcpy(temp_call_nbr, &temp_str[i-6]);
		else
			strcpy(temp_call_nbr, temp_str);

		mvprintw(6+line_no,126, "%6s",temp_call_nbr);
		}
	else
		{
		mvaddch(6+line_no,4,srch_calls[line_no].fleet);
		mvaddstr(6+line_no,7,srch_calls[line_no].status);
		mvaddstr(6+line_no,16,srch_calls[line_no].due_time);
		mvaddstr(6+line_no,24,srch_calls[line_no].extended_type);
		if (srch_calls[line_no].veh_nbr)
			{
			mvprintw(6+line_no,30, "%4d",srch_calls[line_no].veh_nbr);
			}
		if (srch_calls[line_no].pckup_zone)
			{
			mvprintw(6+line_no,35, "%3d",srch_calls[line_no].pckup_zone);
			}
		mvaddstr(6+line_no,39,srch_calls[line_no].pckup_city);

#ifdef SCANDINAVIA
		if (0)
			mvprintw(6 + line_no, 43, "%20s %-6d ",
				srch_calls[line_no].pckup_str_name,
				srch_calls[line_no].pckup_str_nbr);
		else
			mvprintw(6 + line_no, 43, "%20s %-6d%c",
					srch_calls[line_no].pckup_str_name,
					srch_calls[line_no].pckup_str_nbr,
					srch_calls[line_no].pckup_str_suf);
#else
		mvprintw(6+line_no,43, "%d",srch_calls[line_no].pckup_str_nbr);
		if ( srch_calls[line_no].pckup_str_suf && !(0) )
			mvaddch(6+line_no,49,srch_calls[line_no].pckup_str_suf);
		mvaddstr(6+line_no,51,srch_calls[line_no].pckup_pre_dir);
		mvaddstr(6+line_no,54,srch_calls[line_no].pckup_str_name);
		mvaddstr(6+line_no,75,srch_calls[line_no].pckup_str_type);
		mvaddstr(6+line_no,79,srch_calls[line_no].pckup_post_dir);
#endif

		mvaddstr(6+line_no,85,srch_calls[line_no].passenger_name);
		strncpy(temp_str,srch_calls[line_no].pckup_adr_cmnt,20);
		temp_str[20] = '\0';
		mvaddstr(6+line_no,106,temp_str);

		/* get last six digits of the call number */
		sprintf(temp_str,"%d",srch_calls[line_no].nbr);

		for(i=0; temp_str[i] != '\0'; i++);

		if(i > 5)
			strcpy(temp_call_nbr, &temp_str[i-6]);
		else
			strcpy(temp_call_nbr, temp_str);
		mvprintw(6+line_no,126, "%6s",temp_call_nbr);
	}
}

/***********************************************************************************/
/* hilite a line over summary area                                                 */
/***********************************************************************************/
hilite_line(detail_line)
int detail_line;
{
	/* set reverse attributes */
	attrset(A_REVERSE | A_BOLD);
	mvaddstr(6+detail_line,0, Spaces);
	dsp_sum_line(detail_line);        /* set reverse attributes */
	attrset(0);
}

/***********************************************************************************/
/* dim a line over summary area                                                    */
/***********************************************************************************/
dim_line(detail_line)
int detail_line;
{
	/* set reverse dim */
	attrset(A_REVERSE);
	mvaddstr(6+detail_line, 0, Spaces);
	refresh();
	dsp_sum_line(detail_line);
	attrset(0);
}

/***********************************************************************************/
/* unhilite a line over summary area                                               */
/***********************************************************************************/
unhilite_line(detail_line)
int detail_line;
{
	mvaddstr(6+detail_line, 0, Spaces);
	dsp_sum_line(detail_line);
}


