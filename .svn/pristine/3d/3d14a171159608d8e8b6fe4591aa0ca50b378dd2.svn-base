static char sz__FILE__[]="@(#)vh_dr_scrn.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  vh_dr_scrn.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:52:27
 * @(#)  Last delta: 12/2/94 at 17:55:51
 * @(#)  SCCS File: /taxi/sccs/s.vh_dr_scrn.c
 *                                                                           *
 *****************************************************************************/

#include <string.h>
#include <cursesX.h> 
#include <time.h>

#include "screens.h"
#include "ui_def.h"
#include "menu.h"
#include "ui_strdefs.h"

extern int scr_name;
extern struct menus *cmenu[];		/* array of pointers to menus structure in shared memory */
extern int map_array[];			/* to map scr_name to index into menus structures */
extern int uid;				/* users id # */
extern char *Spaces;

disp_vh_dr_dtl_scrn()
{
int     line_offset;
int     loop, indx, len, start_col;

	erase_search_data();

	attrset(A_REVERSE | A_BOLD); 			/* set reverse attributes */
	for (loop=1; loop<5; loop++)
		mvaddstr(loop, 0, Spaces); 
	attrset(0);

	attrset(A_REVERSE);
	mvaddstr(0,0, Spaces); 
	prt_date(); 					/* print the date on the top left corner */
	
	indx = map_array[scr_name];	

	len = strlen(cmenu[indx]->mf_desc); 		/* print the title of the screen */
	start_col = (COLS - len - 1 )/2;
	mvprintw(0,start_col,"%s", cmenu[indx]->mf_desc);
	mvprintw(0,start_col+len+3,"(%d)", cmenu[indx]->mf_command);

	mvprintw(0,UID_COL,"%d", uid); 		/* print the user's id number */
	
	check_mail(); 					/* if have message, print the numbers of message */

	clear_vhdr_detail_area();

	attrset(A_REVERSE); 			/* print the labels */
	if (scr_name == VEHICLE_SRCH_SCR)
		mvaddstr(5,0,catgets(UI_m_catd, UI_1, UI_VH_DR_SCRN_1, "    Fl VEH#  DRV#  MID    LICENSE #     MAKE       MODEL    MDT MET  ......ATTRIBUTES.......                                        "));
	if (scr_name == DRIVER_SRCH_SCR)
	  mvaddstr(5, 0,
		                                                       "    Yh AJAJ  AUTO         AJAJAN NIMI          REKIST. NRO     TAKSILUP    ......OMINAISUUDET.....                                  ");
#ifdef FOO	
		mvaddstr(5,0,catgets(UI_m_catd, UI_1, UI_VH_DR_SCRN_2, "    Fl DRV#  VEH#         DRIVER NAME            LICENSE #       RENEW    TAXI LIC    RENEW    ......ATTRIBUTES.......  ROOKIE      "));
#endif
	mvaddstr(3,0,catgets(UI_m_catd, UI_1, UI_VH_DR_SCRN_3, " Line:    "));
	attrset(0);
	refresh();
}
