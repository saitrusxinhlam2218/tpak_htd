static char sz__FILE__[]="@(#)ui_disp_s.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_disp_s.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:50:17
 * @(#)  Last delta: 12/2/94 at 18:12:16
 * @(#)  SCCS File: /taxi/sccs/s.ui_disp_s.c
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
#include <memory.h>
#include <strings.h>

#include "ui_def.h"
#include "user.h"
#include "menu.h"
#include "mads_types.h"
#include "fleet.h"
#include "screens.h"
#include "call_entry.h"
#include "ui_error.h"
#include "language.h"
#include "ui_strdefs.h"
/* #define LIBRARY 1 */
#include "enhance.h"
#include "switch_ext.h"
#include "ui_call_msg.h"

#define		B_FLD2_COL1	22	/* for printing field 2 in col 1 */
#define		B_DISP_COL2	78	/* for printing labels in col 2 */

extern int scr_name;	/* name of screen on display */
extern struct menus *cmenu[];	/* array of pointers to menus structure in
				 * shared memory */
extern int map_array[];	/* to map scr_name to index into menus structures */
extern struct fleets *fleet[];	/* array of pointers to fleets structure in
				 * shared memory */
extern struct users *uiuser[];	/* array of pointers to users structure in
				 * shared memory */
extern int user_index;	/* index into users structure in shared memory */
extern int uid;	/* users id # */
extern int def_fl_index;	/* index into fleets structure in shared 
 				 * memory for users default fleet(from init() */
extern short been_touched;			/* has the screen been modified */

extern int doing_update;		/* Indicates the update function key is being processed. */
extern int doing_detail;		/* Indicates the detail function key is being processed. */

extern short inters_flag;
extern char *Spaces;

/* disp_basic_scr: displays the basic call entry or the subscription
* 		basic call entry screen with labels and no data
*/
disp_basic_scr()
{

	int indx;	/* index into menus structure on disk */
	int i;	/* scratch variable */
	int findex;	/* index into fleets structure */
	int start_col, len;

	/* erase the previous screen/data that might be on the screen */
	clear(); 

	/* set reverse attributes */
	attrset(A_REVERSE|A_UNDERLINE);
	mvaddstr(0,0, Spaces);

	/* print the date on the top left corner */
	prt_date();

	indx = map_array[scr_name];	

	/* print the title of the screen */
	len = strlen(cmenu[indx]->mf_desc);
	start_col = (COLS - len - 1 )/2;
	mvprintw(0,start_col,"%s", cmenu[indx]->mf_desc);
	mvprintw(0,start_col+len+3,"(%d)", cmenu[indx]->mf_command);
	
	/* print the user's id number */
	mvprintw(0,UID_COL,"%d", uid);

	/* print the second line in reverse video */
	attrset(A_REVERSE);
	mvaddstr(1,0, Spaces);

	/* print the greeting line if the greeting line field in this users
 	 * default fleet is set to YES
	 */
	if ((findex = def_fl_index ) == -1 ) 
		/* users default fleet does not exist */
		prt_error(UI_NOFLEET,"<%c>", uiuser[user_index]->default_fleet);
	else {
		/* users default fleet exists */
		if ( fleet[findex]->greeting_line == YES )
			/* greeting line should be displayed */
			mvprintw(1,GREETING_COL,
			"%s  %s, %s %s. %s",
			catgets(UI_m_catd, UI_1, UI_UI_DISP_S_1, "Good Day!"), fleet[findex]->fleet_name, 
			catgets(UI_m_catd, UI_1, UI_UI_DISP_S_2, "this is"), uiuser[user_index]->greeting_name, catgets(UI_m_catd, UI_1, UI_UI_DISP_S_3, "May I help you?") );
	}  /* end else */

	/* reset normal attributes */
	attrset(0);

	/* print the labels */

	/* print the labels for fleet and type */
	mvaddstr(B_FLEET_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_4, "Fleet"));
	mvaddstr(B_CL_TYPE_ROW,B_FLD2_COL1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_5, "Type"));

	/* print the address label */
	mvaddstr(B_PKUP_ADDRESS_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_6, "Address"));

	mvaddstr(B_PKUP_APT_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_7, "Apt/Room"));
	mvaddstr(B_PKUP_CITY_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_8, "City"));
	mvaddstr(B_PKUP_ZONE_ROW,B_FLD2_COL1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_9, "Zone"));
	mvaddstr(B_PASSENGER_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_10, "Passenger"));

        mvaddstr(B_REQBY_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_44,"Requested by"));

	mvaddstr(B_PHONE_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_11, "Phone"));
#ifdef NAMERICA
	mvaddstr(B_PHONE_ROW,B_PH_AREA_CODE_COL-1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_12, "(   )    -     ext"));
#endif
	if ( scr_name == BASIC_CALL_SCR )
		mvaddstr(B_PKUP_DATE_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_13, "PU Date/Time"));
	else {
		/* its the basic subscription screen */
		mvaddstr(B_PKUP_DATE_ROW, 1, catgets(UI_m_catd, UI_1, UI_UI_DISP_S_14, "Date Entered"));
		mvprintw(B_PKUP_DATE_ROW, B_FLD2_COL1+7, "%s", catgets(UI_m_catd, UI_1, UI_UI_DISP_S_15, "Entered By"));
		mvprintw(B_PKUP_DATE_ROW, B_ENTERED_BY_COL, "%d", uid);
	}  /* end else */

	/* print labels in the 2nd column */
	mvaddstr(B_DEST_ADDRESS_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_16, "Dest Address"));
	mvaddstr(B_DEST_APT_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_7, "Apt/Room"));
	mvaddstr(B_DEST_CITY_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_8, "City"));
	mvaddstr(B_DEST_ZONE_ROW,B_DEST_ZONE_COL - 5,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_9, "Zone"));
	mvaddstr(B_VIA_ADDRESS_ROW, B_DISP_COL2, catgets(UI_m_catd, UI_1, UI_TEXT_57,"Via Address"));
	mvaddstr(B_PERSONNEL_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_17, "Personal"));
	mvaddstr(B_CAR_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_18, "Car Needs"));
        //	mvaddstr(B_DRIVER_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_19, "Driver Needs"));
	mvaddstr(B_CAR_NO_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_20, "# of cars"));
	mvaddstr(B_PRIORITY_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_21, "Priority"));
	mvaddstr(B_RATE_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_22, "Rate"));
	mvaddstr(B_VAT_ROW,B_DISP_COL2+25, catgets(UI_m_catd, UI_1, UI_TEXT_56, "VAT"));
	mvaddstr(B_HOLDLINE_ROW,B_DISP_COL2+25,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_23, "Hold Line"));
	mvaddstr(B_LOCAL_ROW,B_DISP_COL2+41,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_24, "Local"));

	/* print the comment label */
	mvaddstr(B_COMMENT_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_25, "Comment"));
	mvaddstr(B_COMMENT_ROW,B_DISP_COL2, catgets(UI_m_catd, UI_1, UI_UI_DISP_S_47, "Advise Arrival"));
#ifdef NAMERICA
	mvaddstr(B_SEND_DEST_ADDR_ROW,B_DISP_COL2, "Send Dest");
	mvaddstr(B_SEND_RATE_ROW, B_DISP_COL2 +25, "Send Rate" );
#endif

	if ( scr_name == SUBS_BASIC_SCR )
		/* its a subscription screen, print the subscription lines */
		disp_subs_lines();

	/* do not refresh the screen */

}	/* end disp_basic_scr()  */


/* disp_subs_lines: displays the labels for the subscription lines */
disp_subs_lines()
{
	/* print the labels forthe subscription lines */
	mvaddstr(SUBS_ROW1,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_26, "Day"));
	mvaddstr(SUBS_ROW1,SUBS_MON_COL+2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_27, "Mon"));
	mvaddstr(SUBS_ROW1,SUBS_TUE_COL+2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_28, "Tue"));
	mvaddstr(SUBS_ROW1,SUBS_WED_COL+2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_29, "Wed"));
	mvaddstr(SUBS_ROW1,SUBS_THUR_COL+2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_30, "Thu"));
	mvaddstr(SUBS_ROW1,SUBS_FRI_COL+2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_31, "Fri"));
	mvaddstr(SUBS_ROW1,SUBS_SAT_COL+2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_32, "Sat"));
	mvaddstr(SUBS_ROW1,SUBS_SUN_COL+2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_33, "Sun"));

	mvaddstr(SUBS_ROW2,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_34, "Time"));
	mvaddstr(SUBS_ROW2,SUBS_SUN_COL+10,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_35, "Beginning Date"));

	mvaddstr(SUBS_ROW3,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_36, "One Time Change"));
	mvaddstr(SUBS_ROW3,SUBS_SUN_COL+10,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_37, "Ending Date"));

}  /* end disp_subs_lines() */



/* prt_entry_areas: prts spaces in underline  mode for entry areas on all the fields of a call entry screen */
prt_entry_areas(win_fld, ptr_scr_fld, max_piu)
	WINDOW *win_fld;	/* window used for printing of these entry areas */
	struct scr_flds *ptr_scr_fld;	/* pointer to piu structure to be used */
	int max_piu;	/* # of fields to be filled */
{

	int i, j;	
	struct scr_flds *ptr_to_fld;
	int basic_screen, extended_screen, wakeup_screen;

	basic_screen = 0;
	extended_screen = 0;
	wakeup_screen = 0;


	if ( (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR) && ( max_piu > MAX_CL_MSG_PIU ) )
	  basic_screen = 1;
	else if ((scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR || scr_name == INDIVIDUAL_CALLS_SCR) && ( max_piu > MAX_CL_MSG_PIU ) )
	  extended_screen = 1;
	else if (scr_name == WAKEUP_CALL_SCR || scr_name == SUBS_WAKEUP_SCR)
	  wakeup_screen = 1;

	wattrset(win_fld,A_UNDERLINE);
	for ( i=1; i <= max_piu; i++ )  {

	    
	  if ((scr_name == SUBS_BASIC_SCR && (i == B_DATE || i == B_TIME) )  || 
		(scr_name == SUBS_EXTENDED_SCR && (i == E_DATE || i == E_TIME) )   ||
		(scr_name == SUBS_WAKEUP_SCR && (i == W_DATE || i == W_TIME) ) )
	      continue;

		/* Do not print intersection underlines if no intersection */
#ifdef SCANDINAVIA
	  if ( basic_screen &&
		 (!inters_flag && i == B_INTERS_ST_NAME))
	    continue;
#endif

#ifdef NAMERICA
	  
          if ( basic_screen && (!inters_flag && (i == B_INTERS_PRE_DIR ||
				  i == B_INTERS_ST_NAME ||
				  i == B_INTERS_ST_TYPE ||
				  i == B_INTERS_POST_DIR)))
				  
	    continue;




          //	    if ( basic_screen && ( i == B_PKUP_PRE_DIR ||
          //	                           i == B_PKUP_POST_DIR ||
          //				   i == B_DEST_PRE_DIR ||
          //				   i == B_DEST_POST_DIR ) )
          //				   continue;
#endif
				   


#ifdef SCANDINAVIA
	  if ( extended_screen &&
	      ( (!inters_flag && i == E_INTERS_ST_NAME) ||
                ( ( i == E_CASH_ACCT ) && !(0) ) ) )
	    continue;
#endif
	  /*
	  if (!inters_flag && (i == E_INTERS_PRE_DIR ||
				     i == E_INTERS_ST_NAME ||
				     i == E_INTERS_ST_TYPE ||
				     i == E_INTERS_POST_DIR))
	    continue;
*/


#ifdef SCANDINAVIA
	  if (( basic_screen &&
		 (i == B_DUMMY_1 ||
		  i == B_DUMMY_2 ||
		  i == B_DUMMY_3 ||
		  i == B_DUMMY_4 ||
		  i == B_DUMMY_5 ||
		  i == B_DUMMY_6 ||
		  i == B_DUMMY_7 ||
		  i == B_DUMMY_8 ||
		  i == B_DUMMY_9 ||
		  i == B_PERSONEL2 ||
		  (i == B_INTERS_ST_NAME && !inters_flag) ||
		  i == B_PH_NUM1 ||
		  i == B_PH_NUM2 ||
		  i == B_PH_EXT)) ||
		( extended_screen &&
		 (i == E_DUMMY_1 ||
		  i == E_DUMMY_2 ||
		  i == E_DUMMY_3 ||
		  i == E_DUMMY_4 ||
		  i == E_DUMMY_5 ||
		  i == E_DUMMY_6 ||
		  i == E_DUMMY_7 ||
		  i == E_DUMMY_8 ||
		  i == E_DUMMY_9 ||
		  (i == E_INTERS_ST_NAME && !inters_flag) ||
		  i == E_PH_NUM1 ||
		  i == E_PH_NUM2 ||
		  i == E_PERSONEL2 ||
		  i == E_PH_EXT)) ||
		( wakeup_screen &&
		 (i == W_PH_NUM1 ||
		  i == W_PH_NUM2 ||
		  i == W_PH_EXT)))
			continue;
#endif

		/* initialize pointer for accessing the filelds */
            ptr_to_fld = ptr_scr_fld + i;	

	    if ( basic_screen )
	      {
		wattrset(win_fld, 0);
		mvaddstr( B_DRIVER_ROW, B_DRIVER_ATTR1_COL, "                               " );
		mvaddstr( B_CAR_ROW, B_CAR_ATTR1_COL, "                               " );
		wattrset(win_fld,A_UNDERLINE);
	      }
	    else if ( extended_screen )
	      {
		wattrset( win_fld, 0 );
		mvaddstr( E_CAR_ROW, E_CAR_ATTR1_COL, "                               " );
		mvaddstr( E_DRIVER_ROW, E_DRIVER_ATTR1_COL, "                               " );
		wattrset(win_fld,A_UNDERLINE);
	      }

#ifdef SCANDINAVIA
	    if ( (extended_screen) && (i == E_CASH_ACCT) && (doing_detail) )
              wmove(win_fld,ptr_to_fld->prompt_row - 1, ptr_to_fld->prompt_col);
            else
              wmove(win_fld,ptr_to_fld->prompt_row, ptr_to_fld->prompt_col);
#else
            wmove(win_fld,ptr_to_fld->prompt_row, ptr_to_fld->prompt_col);
#endif
		for (j=0; j<ptr_to_fld->max_chars; j++ )
			waddch(win_fld,' ');
	}
	wattrset(win_fld,0);

}


/* disp_wakeup_scr: displays the wakeup call entry or the subscription
* 		wakeup call entry screen with labels and no data
*/
disp_wakeup_scr()
{

	int indx;	/* index into menus structure on disk */
	int i;	/* scratch variable */
	int findex;	/* index into fleets structure */
	int start_col, len;

	/* erase the previous screen/data that might be on the screen */
	clear(); 

	/* set reverse attributes */
	attrset(A_REVERSE|A_UNDERLINE);
	mvaddstr(0,0, Spaces);

	/* print the date on the top left corner */
	prt_date();

	/* print the title of the screen */
	if (!doing_detail && !doing_update)
	  {
	    indx = map_array[scr_name];	
	    len = strlen(cmenu[indx]->mf_desc);
	    start_col = (COLS - len - 1 )/2;
	    mvprintw(0,start_col,"%s", cmenu[indx]->mf_desc);
	    mvprintw(0,start_col+len+3,"(%d)", cmenu[indx]->mf_command);

	    /* print the user's id number */
	    mvprintw(0,UID_COL,"%d", uid);
	    
	    /* print the second line in reverse video */
	    attrset(A_REVERSE);
	    mvaddstr(1,0, Spaces);
	  }

	/* print the greeting line if the greeting line field in this users
 	 * default fleet is set to YES
	 */
	if ((findex = def_fl_index ) == -1 ) 
		/* users default fleet does not exist */
		prt_error(UI_NOFLEET,"<%c>", uiuser[user_index]->default_fleet);
	else {
		/* users default fleet exists */
		if ( scr_name == WAKEUP_CALL_SCR && fleet[findex]->greeting_line == YES
		    && !doing_update && !doing_detail)
			/* greeting line should be displayed */
			mvprintw(1,GREETING_COL,
			"%s  %s, %s %s. %s",
			catgets(UI_m_catd, UI_1, UI_UI_DISP_S_38, "Good day!"), fleet[findex]->fleet_name, 
			catgets(UI_m_catd, UI_1, UI_UI_DISP_S_2, "this is"), uiuser[user_index]->greeting_name,
			catgets(UI_m_catd, UI_1, UI_UI_DISP_S_3, "May I help you?"));
	}  /* end else */

	/* reset normal attributes */
	attrset(0);

	/* print the labels */

	/* print the labels for fleet and type */
	mvaddstr(W_FLEET_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_4, "Fleet"));
	mvaddstr(W_NAME_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_39, "Name"));
	mvaddstr(W_PHONE_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_11, "Phone"));
#ifdef NAMERICA
	mvaddstr(W_PHONE_ROW,W_PH_AREA_CODE_COL-1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_12, "(   )    -     ext"));
#endif
	/* print the comment label */
	mvaddstr(B_COMMENT_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_25, "Comment"));

	if ( scr_name == WAKEUP_CALL_SCR)
		mvaddstr(W_PKUP_DATE_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_40, "Date/Time"));
	else {
		/* its the wakeup subscription screen */
		mvaddstr(W_PKUP_DATE_ROW, 1, catgets(UI_m_catd, UI_1, UI_UI_DISP_S_14, "Date Entered"));
		mvprintw(W_PKUP_DATE_ROW, B_FLD2_COL1+7, "%s", catgets(UI_m_catd, UI_1, UI_UI_DISP_S_15, "Entered By"));
		mvprintw(W_PKUP_DATE_ROW, B_ENTERED_BY_COL, "%d", uid);
	}  /* end else */

	if (scr_name == INDIVIDUAL_CALLS_SCR)
	  mvaddstr(E_FLEET_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_21, "Call #"));
	else if (doing_detail)
	  {
	    if (scr_name == SUBS_WAKEUP_SCR)
	      mvaddstr(E_FLEET_ROW, B_DISP_COL2, catgets(UI_m_catd, UI_1, UI_TEXT_12, "Sub Call #"));
	    else
	      mvaddstr(E_FLEET_ROW, B_DISP_COL2, catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_21, "Call #"));
	  }


	if ( scr_name == SUBS_WAKEUP_SCR )
		/* its a subscription screen, print the subscription lines */
		disp_subs_lines();

	/* do not refresh the screen */

}	/* end disp_wakeup_scr()  */


/* disp_extended_scr: displays the extended call entry or the subscription
* 		extended call entry screen with labels and no data
*  FTJ_call is a BOOL indicating whether the number of call messages field
*  should be displayed.
*  if FTJ_call == 1 display field
*  if FTJ_call == 0 don't display field
*
*/

disp_extended_scr(int FTJ_call)
{

	int indx;	/* index into menus structure on disk */
	int i;	/* scratch variable */
	int findex;	/* index into fleets structure */
	int start_col, len;
	struct cisam_ac  *ac_ptr;

	been_touched = FALSE;

	/* erase the previous screen/data that might be on the screen */
	clear(); 

	/* set reverse attributes */
	attrset(A_REVERSE|A_UNDERLINE);
	mvaddstr(0,0, Spaces);

	/* print the date on the top left corner */
	prt_date();

	if (!doing_detail && !doing_update)
	  {
	    indx = map_array[scr_name];	
	    
	    /* print the title of the screen */
	    len = strlen(cmenu[indx]->mf_desc);
	    start_col = (COLS - len - 1 )/2;
	    mvprintw(0,start_col,"%s", cmenu[indx]->mf_desc);
	    mvprintw(0,start_col+len+3,"(%d)", cmenu[indx]->mf_command);
	
	    /* print the user's id number */
	    mvprintw(0,UID_COL,"%d", uid);
	    
	    /* print the second line in reverse video */
	    attrset(A_REVERSE);
	    mvaddstr(1,0, Spaces);
	  }

	/* print the greeting line if the greeting line field in this users
 	 * default fleet is set to YES
	 */
	if ((findex = def_fl_index ) == -1 ) 
		/* users default fleet does not exist */
		prt_error(UI_NOFLEET,"<%c>", uiuser[user_index]->default_fleet);
	else {
		/* users default fleet exists */
		if ( fleet[findex]->greeting_line == YES
		    && !doing_update && !doing_detail && scr_name != INDIVIDUAL_CALLS_SCR)
			/* greeting line should be displayed */
			mvprintw(1,GREETING_COL,
			"%s  %s, %s %s. %s",
			catgets(UI_m_catd, UI_1, UI_UI_DISP_S_38, "Good day!"), fleet[findex]->fleet_name, 
			catgets(UI_m_catd, UI_1, UI_UI_DISP_S_2, "this is"), uiuser[user_index]->greeting_name,
			catgets(UI_m_catd, UI_1, UI_UI_DISP_S_3, "May I help you?"));
	}  /* end else */

	/* reset normal attributes */
	attrset(0);

	/* print the labels */

	/* print the labels for fleet and type */
	mvaddstr(E_FLEET_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_4, "Fleet"));
	mvaddstr(E_CL_TYPE_ROW,E_CL_TYPE_COL-6,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_5, "Type"));
        
        if ((0) &&
            (FTJ_call == 1) &&
            (0))
          mvaddstr(E_CASH_ACCT_ROW - 1,E_CASH_ACCT_COL-9,catgets(UI_m_catd, UI_1, UI_TEXT_69,"Cash Acct"));
        else if (0)
          mvaddstr(E_CASH_ACCT_ROW,E_CASH_ACCT_COL-9,catgets(UI_m_catd, UI_1, UI_TEXT_69,"Cash Acct"));
        
	mvaddstr(E_INV_ROW, E_INV1_COL-14,catgets(UI_m_catd, UI_1, UI_TEXT_58,"Invoice Nbr"));

	if ((0) && (FTJ_call == 1))
		mvaddstr(E_SP_CL_MSG_ROW,E_SP_CL_MSG_COL-13, catgets(UI_m_catd, UI_1, UI_TEXT_54, "Call Msgs:"));

	mvaddstr(E_CHARGE_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_41, "Charge #"));
	mvaddstr(E_CONAME1_ROW,E_CONAME_COL-9,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_42, "Co. Name"));
	mvaddstr(E_CHARGE_ROW+3,3,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_43, "Driver"));

	/* print the address label */
	mvaddstr(E_PKUP_ADDRESS_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_6, "Address"));

	mvaddstr(E_PKUP_APT_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_7, "Apt/Room"));
	mvaddstr(E_PKUP_CITY_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_8, "City"));
	mvaddstr(E_PKUP_ZONE_ROW,E_PKUP_ZONE_COL-5,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_9, "Zone"));
	mvaddstr(E_PASSENGER_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_10, "Passenger"));
#ifdef NAMERICA
	mvaddstr(E_REQBY_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_44, "Req'd By"));
#else
	mvaddstr(E_REQBY_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_44, "Best{lld av"));
#endif
	mvaddstr(E_PHONE_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_11, "Phone"));
#ifdef NAMERICA
	mvaddstr(E_PHONE_ROW,E_PH_AREA_CODE_COL-1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_12, "(   )    -     ext"));
#endif
	if ( scr_name != SUBS_EXTENDED_SCR )
		mvaddstr(E_PKUP_DATE_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_13, "PU Date/Time"));
	else {
		/* its the extended subscription screen */
		mvaddstr(E_PKUP_DATE_ROW, 1, catgets(UI_m_catd, UI_1, UI_UI_DISP_S_14, "Date Entered"));
		mvprintw(E_PKUP_DATE_ROW, B_FLD2_COL1+7, "%s", catgets(UI_m_catd, UI_1, UI_UI_DISP_S_15, "Entered By"));
		mvprintw(E_PKUP_DATE_ROW, B_ENTERED_BY_COL, "%d", uid);
	}  /* end else */

	/* print labels in the 2nd column */
	mvaddstr(E_DEST_ADDRESS_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_16, "Dest Address"));
	mvaddstr(E_DEST_APT_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_7, "Apt/Room"));
	mvaddstr(E_DEST_CITY_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_8, "City"));
	mvaddstr(E_DEST_ZONE_ROW,B_DEST_ZONE_COL - 6,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_9, "Zone"));
	mvaddstr(E_PERSONEL_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_17, "Personal"));
        //	mvaddstr(E_DRIVER_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_19, "Driver Needs"));
	mvaddstr(E_CAR_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_18, "Car Needs"));
	mvaddstr(E_CAR_NO_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_20, "# of cars"));
	mvaddstr(E_PRIORITY_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_21, "Priority"));
	mvaddstr(E_RATE_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_22, "Rate"));
	mvaddstr(E_VAT_ROW, B_DISP_COL2+25, catgets(UI_m_catd, UI_1, UI_TEXT_56, "VAT"));
	mvaddstr(E_HOLDLINE_ROW,B_DISP_COL2+25,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_23, "Hold Line"));
	mvaddstr(E_LOCAL_ROW,B_DISP_COL2+41,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_24, "Local"));
	mvaddstr(E_SEE_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_45, "See"));
	mvaddstr(E_POD_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_46, "P.O.D.?"));

	/* print the comment label */
	mvaddstr(E_COMMENT_ROW,1,catgets(UI_m_catd, UI_1, UI_UI_DISP_S_25, "Comment"));
	mvaddstr(E_COMMENT_ROW,B_DISP_COL2, catgets(UI_m_catd, UI_1, UI_UI_DISP_S_47, "Advise Arrival"));

	mvaddstr(E_VIA_ADDRESS_ROW, B_DISP_COL2, catgets(UI_m_catd, UI_1, UI_TEXT_57,"Via Address"));

#ifdef NAMERICA
	mvaddstr(E_SEND_DEST_ADDR_ROW,B_DISP_COL2+25, "Send Dest");
	mvaddstr(E_SEND_RATE_ROW, B_DISP_COL2+41, "Send Rate");
#endif
	
	if (scr_name == INDIVIDUAL_CALLS_SCR)
	  mvaddstr(E_FLEET_ROW,B_DISP_COL2,catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_21, "Call #"));
	else if (doing_detail)
	  {
	    if (scr_name == SUBS_EXTENDED_SCR)
	      mvaddstr(E_FLEET_ROW - 1, B_DISP_COL2, catgets(UI_m_catd, UI_1, UI_TEXT_12, "Sub Call #"));
	    else
	      mvaddstr(E_FLEET_ROW - 1, B_DISP_COL2, catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_21, "Call #"));
	  }

	if ( scr_name == SUBS_EXTENDED_SCR )
		/* its a subscription screen, print the subscription lines */
		disp_subs_lines();

	/* do not refresh the screen */

}	/* end disp_extended_scr()  */