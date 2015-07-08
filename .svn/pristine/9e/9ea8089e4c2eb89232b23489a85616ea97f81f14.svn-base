static char sz__FILE__[]="@(#)kdt_disp.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  kdt_disp.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:49:08
* @(#)  Last delta: 12/2/94 at 17:59:51
* @(#)  SCCS File: /taxi/sccs/s.kdt_disp.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

#include <cursesX.h>
#include "ui_def.h"
#include "user.h"
#include "menu.h"
#include <sys/types.h>
#include "mads_types.h"
#include "fleet.h"
#include <memory.h>
#include "screens.h"
#include "kdt_ary.h"
#include "ui_error.h"
#include "ui_strdefs.h"

extern WINDOW *srch_win;

/*****************************************************************************/
/* disp_basic_scr: displays the basic call entry or the subscription         */
/*		basic call entry screen with labels and no data              */
/*****************************************************************************/
disp_kdt_lab()
{
        tpak_box(srch_win, 0, 0 );
	mvwaddstr(srch_win,0,0," ");			/* print the labels */
	mvwaddstr(srch_win,1,51,catgets(UI_m_catd, UI_1, UI_KDT_DISP_1, "MDT EMULATION WINDOW"));			/* print the labels */
	mvwaddstr(srch_win,L_FLEET_ROW,L_FLEET_COL,catgets(UI_m_catd, UI_1, UI_KDT_DISP_2, "Fleet ID: "));
	mvwaddstr(srch_win,L_FLEET_ROW,L_VEH_COL,catgets(UI_m_catd, UI_1, UI_KDT_DISP_3, "Vehicle Number: "));
	mvwaddstr(srch_win,L_FLEET_ROW,L_ACTION_COL,catgets(UI_m_catd, UI_1, UI_KDT_DISP_4, "Action Code: "));
	mvwaddstr(srch_win,L_FLEET_ROW,L_ZONE_COL,catgets(UI_m_catd, UI_1, UI_KDT_DISP_5, "Zone Number: "));
	mvwaddstr(srch_win,L_DRV_ROW,L_ZONE_COL,catgets(UI_m_catd, UI_1, UI_KDT_DISP_6, "Driver   ID: "));
	mvwaddstr(srch_win,L_FLEET_ROW,L_PRI_COL,catgets(UI_m_catd, UI_1, UI_KDT_DISP_7, "Taxi Queue Priority: "));
	mvwaddstr(srch_win,L_LAB_ROW,L_FLEET_COL,catgets(UI_m_catd, UI_1, UI_KDT_DISP_8, "ACTION CODES: 1=BOOK 2=CBOOK 3=EBOOK 4=S-IN 5=S-OUT 6=UNBOOK 7=MDT AUTHORIZE 8=TRIP ACCEPT 9=TRIP REJECT 10=METER ON 11=METER OFF"));
}	
