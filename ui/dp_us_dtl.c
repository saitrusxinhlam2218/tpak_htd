static char sz__FILE__[]="@(#)dp_us_dtl.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  dp_us_dtl.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:48:18
 * @(#)  Last delta: 12/2/94 at 18:10:09
 * @(#)  SCCS File: /taxi/sccs/s.dp_us_dtl.c
 *                                                                           *
 *****************************************************************************/

#include <string.h>
#include <cursesX.h>
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "time.h"
#include "df_maint.h"
#include "call_entry.h"
#include "language.h"

extern	struct cisam_up srch_up_calls[];
/****************************************************************************************/
/* display the line info over the detail area 						*/
/****************************************************************************************/
dsp_us_detail(line_no)
int line_no;
{
	char all_perms[180];
	char temp_perms[180];
	int i, screen_row, split_point;

	attrset(A_REVERSE | A_BOLD);

	mvprintw(2,8, "%2d",srch_up_calls[line_no].default_screen);
	mvaddch(2,26,srch_up_calls[line_no].can_zone_calls);
	mvaddch(2,41,srch_up_calls[line_no].default_fleet);
	mvaddstr(2,50,srch_up_calls[line_no].default_city);
	if (srch_up_calls[line_no].default_search > 0) {
		mvprintw(3,8, "%2d",srch_up_calls[line_no].default_search);
	}
	if(srch_up_calls[line_no].high_call_pri > 0){
		mvprintw(3,25, "%2d", srch_up_calls[line_no].high_call_pri);
	}
	mvaddch(3,41,srch_up_calls[line_no].default_call_tp);

	all_perms[0] = '\0';							/*   menu permissions */
	if (srch_up_calls[line_no].menu_perms[1] == YES)
		strncat(all_perms,"1 ",2);

	if (srch_up_calls[line_no].menu_perms[2] == YES)
		strncat(all_perms,"2 ",2);

	if (srch_up_calls[line_no].menu_perms[3] == YES)
		strncat(all_perms,"3 ",2);

	if (srch_up_calls[line_no].menu_perms[4] == YES)
		strncat(all_perms,"4 ",2);

	if (srch_up_calls[line_no].menu_perms[5] == YES)
		strncat(all_perms,"5 ",2);

	if (srch_up_calls[line_no].menu_perms[6] == YES)
		strncat(all_perms,"6 ",2);

	if (srch_up_calls[line_no].menu_perms[7] == YES)
		strncat(all_perms,"11 ",3);

	if (srch_up_calls[line_no].menu_perms[8] == YES)
		strncat(all_perms,"12 ",3);
		
	if (srch_up_calls[line_no].menu_perms[9] == YES)
		strncat(all_perms,"13 ",3);

	if (srch_up_calls[line_no].menu_perms[10] == YES)
		strncat(all_perms,"14 ",3);

	if (srch_up_calls[line_no].menu_perms[11] == YES)
		strncat(all_perms,"15 ",3);

	if (srch_up_calls[line_no].menu_perms[12] == YES)
		strncat(all_perms,"16 ",3);

	if (srch_up_calls[line_no].menu_perms[13] == YES)
		strncat(all_perms,"17 ",3);

	if (srch_up_calls[line_no].menu_perms[14] == YES)
		strncat(all_perms,"18 ",3);

	if (srch_up_calls[line_no].menu_perms[15] == YES)
		strncat(all_perms,"19 ",3);

	if (srch_up_calls[line_no].menu_perms[16] == YES)
		strncat(all_perms,"21 ",3);
		
	if (srch_up_calls[line_no].menu_perms[17] == YES)
		strncat(all_perms,"22 ",3);

	if (srch_up_calls[line_no].menu_perms[18] == YES)
		strncat(all_perms,"23 ",3);

	if (srch_up_calls[line_no].menu_perms[19] == YES)
		strncat(all_perms,"24 ",3);

	if (srch_up_calls[line_no].menu_perms[20] == YES)
		strncat(all_perms,"25 ",3);

	if (srch_up_calls[line_no].menu_perms[21] == YES)
		strncat(all_perms,"26 ",3);

	if (srch_up_calls[line_no].menu_perms[22] == YES)
		strncat(all_perms,"27 ",3);

	if (srch_up_calls[line_no].menu_perms[23] == YES)
		strncat(all_perms,"28 ",3);

	if (srch_up_calls[line_no].menu_perms[24] == YES)
		strncat(all_perms,"29 ",3);
		
	if (srch_up_calls[line_no].menu_perms[25] == YES)
		strncat(all_perms,"31 ",3);

	if (srch_up_calls[line_no].menu_perms[26] == YES)
		strncat(all_perms,"32 ",3);

	if (srch_up_calls[line_no].menu_perms[27] == YES)
		strncat(all_perms,"33 ",3);

	if (srch_up_calls[line_no].menu_perms[28] == YES)
		strncat(all_perms,"34 ",3);

	if (srch_up_calls[line_no].menu_perms[29] == YES)
		strncat(all_perms,"35 ",3);
		
	if (srch_up_calls[line_no].menu_perms[30] == YES)
		strncat(all_perms,"36 ",3);

	if (srch_up_calls[line_no].menu_perms[31] == YES)
		strncat(all_perms,"37 ",3);

	if (srch_up_calls[line_no].menu_perms[32] == YES)
		strncat(all_perms,"38 ",3);

	if (srch_up_calls[line_no].menu_perms[33] == YES)
		strncat(all_perms,"39 ",3);

	if (srch_up_calls[line_no].menu_perms[34] == YES)
		strncat(all_perms,"61 ",3);

	if (srch_up_calls[line_no].menu_perms[35] == YES)
		strncat(all_perms,"62 ",3);

	if (srch_up_calls[line_no].menu_perms[36] == YES)
		strncat(all_perms,"63 ",3);

	if (srch_up_calls[line_no].menu_perms[37] == YES)
		strncat(all_perms,"64 ",3);
		
	if (srch_up_calls[line_no].menu_perms[38] == YES)
		strncat(all_perms,"65 ",3);

	if (srch_up_calls[line_no].menu_perms[39] == YES)
		strncat(all_perms,"66 ",3);

	if (srch_up_calls[line_no].menu_perms[40] == YES)
		strncat(all_perms,"67 ",3);

	if (srch_up_calls[line_no].menu_perms[41] == YES)
		strncat(all_perms,"68 ",3);

	if (srch_up_calls[line_no].menu_perms[42] == YES)
		strncat(all_perms,"69 ",3);

	if (srch_up_calls[line_no].menu_perms[43] == YES)
		strncat(all_perms,"281 ",4);

	if (srch_up_calls[line_no].menu_perms[44] == YES)
		strncat(all_perms,"282 ",4);

	if (srch_up_calls[line_no].menu_perms[45] == YES)
		strncat(all_perms,"283 ",4);

	if (srch_up_calls[line_no].menu_perms[46] == YES)
		strncat(all_perms,"284 ",4);

	if (srch_up_calls[line_no].menu_perms[47] == YES)
		strncat(all_perms,"285 ",4);

	if (srch_up_calls[line_no].menu_perms[48] == YES)
		strncat(all_perms,"286 ",4);

	if (srch_up_calls[line_no].menu_perms[49] == YES)
		strncat(all_perms,"287 ",4);

	if (srch_up_calls[line_no].menu_perms[50] == YES)
		strncat(all_perms,"288 ",4);
		
	if (srch_up_calls[line_no].menu_perms[51] == YES)
		strncat(all_perms,"289 ",4);

	attrset(A_REVERSE);
	for (i = 2; i < 5; i ++)
	    /* Add 60 spaces */
	    mvaddstr(i, 59, "                                                            ");

	attrset(A_REVERSE | A_BOLD);

	screen_row = 2;
	while (1) {
		if (strlen(all_perms) > 60) {

		    /* Go back to find a space where split the line */
		    for (split_point = 60; split_point > 0 && all_perms[split_point] != ' '; split_point --);

		    /* This shouldn't happen */
		    if (split_point == 0)
			break;
		    
		    /* Copy only the screens that will fit on one line */
		    strncpy(temp_perms, all_perms, split_point);
		    temp_perms[split_point] = '\0';

		    mvaddstr(screen_row, 59, temp_perms);
		    strcpy(temp_perms, all_perms + split_point + 1);
		    strcpy(all_perms,temp_perms);
		    screen_row ++;
		}
		else {
		    mvaddstr(screen_row, 59, all_perms);
		    break;
		}
	}
}
