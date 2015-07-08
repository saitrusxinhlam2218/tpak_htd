static char sz__FILE__[]="@(#)ui_disp_m.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_disp_m.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:50:14
 * @(#)  Last delta: 12/2/94 at 18:27:22
 * @(#)  SCCS File: /taxi/sccs/s.ui_disp_m.c
 *                                                                           *
 *****************************************************************************/
#include <cursesX.h>
#include <string.h>
#include "screens.h"
#include "ui_def.h"
#include "user.h"
#include "menu.h"
#include "language.h"
#include "ui_strdefs.h"

#define		PROJ_TITLE_LENGTH	36   /* length of proj. title length */
#define		SIZE_FIRST_LINE		132	/* # of cols in a line */
#define		MAX_ITEMS_MM		6	/* max items on main menu */
#define		CM_TITLE_COL		32   /* col to print call menu title */


extern int uid;	/* users id # */
extern char sTelnetHost[];
extern char sLatDescript[];
extern int LineMgrIndex;
extern char sGPS_host[];
extern int map_array[];	/* to convert scr_name into index into menus structure
			 * in shared memory */
extern int user_index;	/* index into users structure in shared memory */
char *ptr_item_desc[MAX_MENU_ITEMS];	/* pointers to description of items 
					 * to appear on a menu screen */
char item_desc[BUF_ITEM_DESC];	/* buffer to save item descriptions to appear
				 * on a menu screen */
int no_of_items;	/* # of items to be displayed on a menu screen */
int num_highlighted = 1;   /* # of the item highlighted on the menu screen */
extern struct menus *cmenu[]; 	 /* array of pointers to menus structures 
				  * in shared memory */
extern int (*security[])();	/* array of pointers to functions for 
				 * checking users permissions */
extern int prev_scr_flag;

/* disp_menu:	 	displays the menu specified by the argument scr_name
 *			on the terminal screen. For
 *			items for which the user does not have permission
 *			the item is displayed in dim mode, otherwise it
 *			is displayed in regular mode
 */
int display_menu(scr_name)
	int scr_name;	/* name of screen to be displayed */
{


	static char blank_string[] = "               ";	/* printed if item desc
							 * not defined */
	int i;	/* scratch variable */
	short highlight;	/* flag to print 1st allowed item highlighted */
	int max_menu_items;	/* indicates # of items on a menu screen */
	int indx;	/* index into menus structure */
	int dim[MAX_MENU_ITEMS];	/* 0=dim display, 1=regular display */
	int length;	/* length of menu title to be printed */
	int diff;	/* diff. b/w project title length & menu title length */

	/* erase the previous screen */
	/*erase(); */
	clear();

	if ( prev_scr_flag ) 
		prev_scr_flag = 0;	/* called by pressinf PREV_SCR KEY */
	/* make the first two lines on the screen in reverse video */
	attrset(A_REVERSE);

	/* Print the date on the top left corner of the screen. This
	 * is done by calling prt_date().
	 */
	prt_date();

	mvaddstr(0,PROJ_TITLE_COL,catgets(UI_m_catd, UI_1, UI_UI_DISP_M_1, " TaxiPak Computerized Taxi Dispatch System "));

	/* Print the user's id.  */

	mvprintw(0,UID_COL," %d ", uid);

	/* Print the title of the screen. If the user has not specified
	 * the description of the title, then display a blank string,
	 * to center the menu title properly, some calculation is
	 * required
	 */
	if ( cmenu[map_array[scr_name]]->mf_desc[0] == '\0' )  
		/* user has not specified a screen title for display */
		mvaddstr(MENU_TITLE_ROW,MENU_TITLE_COL,blank_string);
	else {
		/* user has specified a screen title for display */
		/* get length of menu title */
		length = strlen(cmenu[map_array[scr_name]]->mf_desc) + 2 ;
		diff = length - PROJ_TITLE_LENGTH;
		mvprintw(MENU_TITLE_ROW,(PROJ_TITLE_COL-diff/2)," %s ",
			cmenu[map_array[scr_name]]->mf_desc);
	}   /* end else */

	/* if the menu to be displayed is MAIN_MENU, then the max 
	 * # of items that can be displayed is 6, otherwise the
	 * max # of items that can be displayed is 9. Set the
	 * variable max_menu_items to this #
	 */
	if ( scr_name == MAIN_MENU ) 
		max_menu_items = MAX_ITEMS_MM;
	else
		max_menu_items = MAX_MENU_ITEMS; 
		
	/* enter a for loop to determine how many items are to be displayed,
	 * and also the permissions and descriptions
	 */
	for (i=0; i< max_menu_items; i++)  {
		/* check if the user has defined the description of
		 * the item to be displayed
		 */
		indx = map_array[scr_name*10+i+1];
		if ( cmenu[indx]->mf_desc[0] == '\0' )
			/* user has not defined the description of the 
			 * item to be displayed, so a blank string 
			 * should be displayed
			 */
			strcpy(ptr_item_desc[i],blank_string);
		else
			/* user has defined the description of the item
			 * to be displayed
			 */
			strcpy(ptr_item_desc[i],cmenu[indx]->mf_desc);
		
		/* check whether the user has permission to execute this
		 * item. If the user does not have permission, then set
		 * dim[i] to 0, otherwise set dim[i] to 1. 
		 */
		if ( (*security[indx])() == NOT_ALLOWED )  
			/* user does not have permission */
			dim[i] = 1;
		else 
			/* user has permission */
			dim[i] = 0;
		
		no_of_items = i + 1;	/* so far # of items to display */

		/* check if this is to be the last item on the screen */
		if ( cmenu[indx]->mf_last_menu_item == YES)
			/* this is the last item to be displayed on the menu */
			break;	/* leave for loop */
	

	}  /* end for */

	/* for the items for which the user does not permission, display
	 * in dim mode 
	 */
	attrset(0);
	for ( i = 0; i < no_of_items; i++ )  {
		if ( dim[i] ) {
			/* print the lines for which the user does not
			 * have permission, in dim mode
			 */
			if ( i <  MAX_ITEMS_ON_1ST_COL )
				/* print on 1st col */
				mvprintw(ROW_1ST_ITEM+i*2,MENU_1ST_COL,
					" %d. %s ", i+1,ptr_item_desc[i]);
			else 
				/* print it in the 2nd col */
				mvprintw(ROW_1ST_ITEM+(i-7)*2,MENU_2ND_COL,
					" %d. %s ",i+1, ptr_item_desc[i]);
		}  /* end if dim */

	}  /* end for */
	
	/* print the lines for which the user has permission, in regular
	 * mode. The first such line should be highlighted
	 */
	highlight = 1; 	/* so that 1st allowed item is highlighted */
	attrset(A_REVERSE);
	for (i=0; i < no_of_items; i++ )  {
		if ( ! dim[i] ) {
			if ( i < MAX_ITEMS_ON_1ST_COL ) 
				mvprintw(ROW_1ST_ITEM+i*2,MENU_1ST_COL,
					" %d. %s ", i+1, ptr_item_desc[i]);
			else 
				/* print it in the 2nd col */
				mvprintw(ROW_1ST_ITEM+(i-7)*2,MENU_2ND_COL,
					" %d. %s ",i+1, ptr_item_desc[i]);
				
			if ( highlight ) {
				num_highlighted = i+1; /* this item is 
							* highlighted */
				highlight = 0;
				attrset(A_BOLD);
			}
		}  /* end if !dim */
	}  /* end for */
	attrset(0);

	/* Print the last few lines (similar to those on informix screens),
	 * and place the cursor at the appropriate position.
	 */
	mvprintw(LINES-5,0,catgets(UI_m_catd, UI_1, UI_UI_DISP_M_2, "Use space bar, arrow keys, or type number to make selection"));
   	mvprintw(LINES-4,0,catgets(UI_m_catd, UI_1, UI_UI_DISP_M_3, "Enter 'e' to return to previous menu or exit."));
   	mvprintw(LINES-3,0,catgets(UI_m_catd, UI_1, UI_UI_DISP_M_4, "Enter carriage return to execute selection:  "));

	attrset(A_BOLD);
	if ( strlen( sTelnetHost ) )
	  mvprintw(23, 131 - strlen(sTelnetHost) - strlen( sGPS_host ) - 3, "%s-->%s", sTelnetHost, sGPS_host);
        else if ( strlen( sLatDescript ) )
          mvprintw(23, 131 - strlen(sLatDescript) - 5, "%s-->%02d", sLatDescript, LineMgrIndex);
	attrset(0);
	
 	touchwin(stdscr);

        return(1);
}  /* end disp_call_menu() */


