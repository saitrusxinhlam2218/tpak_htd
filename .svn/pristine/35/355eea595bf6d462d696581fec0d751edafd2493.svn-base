static char sz__FILE__[]="@(#)dp_us_scrn.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  dp_us_scrn.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:48:22
 * @(#)  Last delta: 12/2/94 at 18:11:06
 * @(#)  SCCS File: /taxi/sccs/s.dp_us_scrn.c
 *                                                                           *
 *****************************************************************************/

#include <string.h>
#include <cursesX.h> 
#include <time.h>
#include "ui_strdefs.h"
#include "screens.h"
#include "menu.h"
#include "ui_def.h"

extern int scr_name;
extern struct menus *cmenu[];		/* array of pointers to menus structure in shared memory */
extern int map_array[];			/* to map scr_name to index into menus structures */
extern int uid;				/* users id # */
extern char *Spaces;

disp_user_scrn()
{
int     line_offset;
int     i, indx, len, start_col;

	erase_search_data(); 					/* erase the old screen */

	attrset(A_REVERSE);

	for (i = 0; i < 6; i ++)
	    mvaddstr(i, 0, Spaces);

	prt_date(); 					/* print the date on the top left corner */

	indx = map_array[scr_name];	
	len = strlen(cmenu[indx]->mf_desc); 		/* print the title of the screen */
	start_col = (COLS - len - 1 )/2;
	mvprintw(0,start_col,"%s", cmenu[indx]->mf_desc);
	mvprintw(0,start_col+len+3,"(%d)", cmenu[indx]->mf_command);

	mvprintw(0,UID_COL,"%d", uid); 			/* print the user's id number */
	
	check_mail(); 					/* if have message, print the numbers of message */

	clear_user_detail_area();

	attrset(A_REVERSE);
	mvaddstr(5,0,catgets(UI_m_catd, UI_1, UI_DP_US_SCRN_1, "    USER          USER NAME         GREETING NAME  .....FLEETS....                                                                  "));
	mvaddstr(4,0,catgets(UI_m_catd, UI_1, UI_DP_US_SCRN_2, " Line:    "));
	attrset(0);
	refresh();
}

clear_user_detail_area()
{
	char		 blank_string[132];
	int			 i;


	sprintf(blank_string, "%132s", " ");
	attrset(A_REVERSE);
	for (i = 2; i < 5; i ++)
	    mvaddstr(i, 0, blank_string);

	attrset(A_REVERSE | A_BOLD);
	mvaddstr(1,0,catgets(UI_m_catd, UI_1, UI_DP_US_SCRN_3, " ..............DEFAULTS............................. "));
	mvaddstr(1,59,catgets(UI_m_catd, UI_1, UI_DP_US_SCRN_4, " ..................MENU ITEMS THIS USER MAY EXECUTE.................... "));
	attrset(A_REVERSE);
	mvaddstr(2,0,catgets(UI_m_catd, UI_1, UI_DP_US_SCRN_5, " Screen "));
	mvaddstr(2,12,catgets(UI_m_catd, UI_1, UI_DP_US_SCRN_6, " Zone Calls "));
	mvaddstr(2,29,catgets(UI_m_catd, UI_1, UI_DP_US_SCRN_7, " Fleet "));
	mvaddstr(2,43,catgets(UI_m_catd, UI_1, UI_DP_US_SCRN_8, " City "));
	mvaddstr(3,0,catgets(UI_m_catd, UI_1, UI_DP_US_SCRN_9, " Search "));
	mvaddstr(3,12,catgets(UI_m_catd, UI_1, UI_DP_US_SCRN_10, " Highest Pri "));
	mvaddstr(3,29,catgets(UI_m_catd, UI_1, UI_DP_US_SCRN_11, " Call Type "));
	mvaddstr(4,0,catgets(UI_m_catd, UI_1, UI_DP_US_SCRN_2, " Line:    "));
	attrset(0);
}
