static char sz__FILE__[]="@(#)dp_al_scrn.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  dp_al_scrn.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:48:11
 * @(#)  Last delta: 12/2/94 at 18:34:58
 * @(#)  SCCS File: /taxi/sccs/s.dp_al_scrn.c
 *                                                                           *
 *****************************************************************************/
#include <cursesX.h>
#include <string.h>
#include <time.h>
#include "ui_strdefs.h"
#include "menu.h"
#include "ui_def.h"
#include "screens.h"

extern int scr_name;				/* screen name */
extern int uid;					/* users id # */
extern struct menus *cmenu[];			/* array of pointers to menus structure in shared memory */
extern int map_array[];				/* to map scr_name to index into menus structures */

/* Full screen width of spaces */
char  *Spaces = "                                                                                                                                    ";

erase_search_data()
{
    int row;


    attrset(0);
    /* Erase from the 5th row all but the last row -- it might have an error on it */
    for (row = 5; row < LINES - 1; row ++)
	mvaddstr(row, 0, Spaces);
}


/***************************************************************************/
/* disp_calls_scrn:  Display an empty search screen for call searches      */
/***************************************************************************/
disp_calls_scrn()
{
	char    c_string[READ_BUF_SIZE + 1];
	int     line_offset;
	int     indx, len, start_col;
	int     row;
    
	erase_search_data();

	attrset(A_REVERSE);
	mvaddstr(0,0, Spaces); 
	prt_date(); 						/* print the date on the top left corner */

	indx = map_array[scr_name];	
	len = strlen(cmenu[indx]->mf_desc); 			/* print the title of the screen */
	start_col = (COLS - len - 1 )/2;
	mvprintw(0,start_col,"%s", cmenu[indx]->mf_desc);
	mvprintw(0,start_col+len+3,"(%d)", cmenu[indx]->mf_command);
	
	mvprintw(0,UID_COL,"%d", uid); 				/* print the user's id number */
	
	check_mail(); 						/* if have message, print the numbers of message */

        attrset(A_REVERSE | A_BOLD);
	mvaddstr(1,0,Spaces);
	mvaddstr(2,0,Spaces);
	mvaddstr(3,0,Spaces);
	mvaddstr(4,0,Spaces);

	attrset(0); 						/* for detail area */
	mvaddstr(3,17,"                                                     ");
	attrset(A_REVERSE);
	mvaddstr(3,17,catgets(UI_m_catd, UI_1, UI_DP_AL_SCRN_1, " Due  Disp'd Veh# Pickup Close  Pri   Date     Attr  "));
	sprintf(c_string,catgets(UI_m_catd, UI_1, UI_DP_AL_SCRN_2, "    Fl  Status    Due   Type  Veh# Zon Cty             Pickup Address                    Passenger        Place/Address Comment     \0"));
	mvaddstr(5,0,c_string);
	attrset(0);

	mvaddstr(3,1,"          ");
	attrset(A_REVERSE);
	mvaddstr(3,1,catgets(UI_m_catd, UI_1, UI_DP_AL_SCRN_3, " Line:    "));
	attrset(0);

	refresh();
}
