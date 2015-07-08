static char sz__FILE__[]="@(#)sp_disp.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  sp_disp.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:49:43
 * @(#)  Last delta: 12/2/94 at 18:00:48
 * @(#)  SCCS File: /taxi/sccs/s.sp_disp.c
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include <sys/types.h>
#include <memory.h>

#include "ui_def.h"
#include "user.h"
#include "menu.h"
#include "mads_types.h"
#include "fleet.h"
#include "screens.h"
#include "sp_ary.h"
#include "ui_error.h"
#include "ui_strdefs.h"

extern int prt_entry_areas();
extern int scr_name;				/* name of screen on display */
extern struct menus *cmenu[];			/* array of pointers to menus structure in shared memory */
extern int map_array[];				/* to map scr_name to index into menus structures */
extern struct fleets *fleet[];			/* array of pointers to fleets structure in shared memory */
extern struct users *uiuser[];			/* array of pointers to users structure in shared memory */
extern int user_index;				/* index into users structure in shared memory */
extern int uid;					/* users id # */
extern int def_fl_index;			/* index into fleet struct in shared memory for users default fleet(from init() */
extern char *Spaces;

/***********************************************************************************/
/* disp_sp_scr:    displays the vehicle/driver suspension screen with labels       */
/* 		   and no data.                                    	           */
/***********************************************************************************/
disp_sp_scr()
{
	int indx;				/* index into menus structure on disk */
	int i;					/* scratch variable */


	clear(); 				/* erase the previous screen/data that might be on the screen */

	attrset(A_REVERSE);
	mvaddstr(0,0, Spaces);

	prt_date();

	indx = map_array[scr_name];	

	mvprintw(0,SCR_TITLE_COL,"%s", cmenu[indx]->mf_desc); 			/* print the title of the screen */
	mvprintw(0,SCR_TITLE_COL+strlen(cmenu[indx]->mf_desc) +3,"(%d)", cmenu[indx]->mf_command); 			/* print the title of the screen */
	
	mvprintw(0,UID_COL,"%d", uid); 						/* print the user's id number */

	attrset(0); 								/* reset normal attributes */

	mvaddstr(D_FLEET_ROW,1,catgets(UI_m_catd, UI_1, UI_SP_DISP_1, "Fleet")); 					/* print the labels */
	mvaddstr(D_VEH_ROW-1,D_CLEAR_SUSP_COL,catgets(UI_m_catd, UI_1, UI_SP_DISP_15, "P"));
	mvaddstr(D_VEH_ROW-1,D_CLEAR_SUSP_COL+5,catgets(UI_m_catd, UI_1, UI_SP_DISP_16, "D"));
	mvaddstr(D_VEH_ROW-1,D_CLEAR_SUSP_COL+10,catgets(UI_m_catd, UI_1, UI_SP_DISP_17, "X"));
	mvaddstr(D_VEH_ROW,1,catgets(UI_m_catd, UI_1, UI_SP_DISP_2, "Vehicle  #"));
	mvaddstr(D_VEH_ROW,26,catgets(UI_m_catd, UI_1, UI_SP_DISP_3, "Driver ID"));
	mvaddstr(D_VEH_ROW,50,catgets(UI_m_catd, UI_1, UI_SP_DISP_4, "Clear Suspension (Y/N)"));
	mvaddstr(D_SUSP_ROW+1,D_SUSP_FROM_DATE_COL,catgets(UI_m_catd, UI_1, UI_SP_DISP_6, "From Date"));
	mvaddstr(D_SUSP_ROW+1,D_SUSP_FROM_TIME_COL,catgets(UI_m_catd, UI_1, UI_SP_DISP_7, "From Time"));
	mvaddstr(D_SUSP_ROW+1,D_SUSP_TO_DATE_COL,catgets(UI_m_catd, UI_1, UI_SP_DISP_8, "To Date"));
	mvaddstr(D_SUSP_ROW+1,D_SUSP_TO_TIME_COL,catgets(UI_m_catd, UI_1, UI_SP_DISP_9, "To Time"));
	mvaddstr(D_PASS_ROW,1, catgets(UI_m_catd, UI_1, UI_SP_DISP_10, "Suspend From Servicing Passenger Calls  (Y/N)"));
	mvaddstr(D_DEL_ROW,1,  catgets(UI_m_catd, UI_1, UI_SP_DISP_11, "Suspend From Servicing Delivery Calls   (Y/N)"));
	mvaddstr(D_TYPEX_ROW,1,catgets(UI_m_catd, UI_1, UI_SP_DISP_12, "Suspend From Servicing Type 'X' Calls   (Y/N)"));
	mvaddstr(D_REASON_ROW,1,catgets(UI_m_catd, UI_1, UI_SP_DISP_13, "Manager's Reason For Suspension (Not Seen by Driver)"));
	mvaddstr(D_MSG1_ROW,1,catgets(UI_m_catd, UI_1, UI_SP_DISP_14, "Message Sent to Driver"));
}
