static char sz__FILE__[]="@(#)ui_process.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_process.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:51:17
 * @(#)  Last delta: 12/2/94 at 18:24:29
 * @(#)  SCCS File: /taxi/sccs/s.ui_process.c
 *                                                                           *
 *****************************************************************************/
#include <cursesX.h>
#include <ctype.h>
#include <string.h>

#include "ui_strdefs.h"
#include "ui_def.h"
#include "ui_error.h"
#include "screens.h"
#include "tokenize.h"
#include "language.h"

int terminate = 0;			/* flag set if process loop is to end */
char scr_type;				/* type of screen on display, UI_SCREEN('S') or UI_MENU('M') */
int scr_name;				/* name of screen on display */
extern int no_of_items;			/* # of items on display onthe menu screen */
extern int num_highlighted;		/* # of the item highlighted on the mneu */
extern char *ptr_item_desc[];		/* array of pointers to point to desc of items on the menu */
extern int map_array[];			/* to map scr_name to index into menus structure */
extern int (*security[])();		/* pointers to functions for checking users permissions */
extern short clear_err_flag;		/* set if an error msg appears on last line */

int process_dummy(); 			/* a dummy function ... not called */
/* declare functions used for processing the various user interface tasks */
extern int  update_user_defaults(), basic_call(), extended_call(), wakeup_call(), unassigned_calls(), 
subsc_fut_calls(), assigned_calls(), all_calls(), veh_drv_calls(), indiv_calls(), exceptions(), veh_drv_suspend(), 
zone_status(), system_status(), edit_repeat_msgs(), veh_srch(), drv_srch(), user_srch(), exec_user_prog(), exec_shift_s(), zn_calls();

/* define and initialize pointers to functions which process the various user interface tasks */
int (*process_func[])() = { process_dummy, process_dummy, process_dummy, process_dummy, update_user_defaults, exec_user_prog,
			process_dummy, basic_call, extended_call, wakeup_call, basic_call, extended_call, wakeup_call,
			exec_user_prog, exec_user_prog, exec_user_prog, unassigned_calls, subsc_fut_calls, assigned_calls,
			all_calls, veh_drv_calls, indiv_calls, exceptions, process_dummy, zn_calls, veh_drv_suspend, 
			zone_status, system_status, edit_repeat_msgs, exec_user_prog, exec_shift_s, exec_user_prog, 
			exec_user_prog, exec_user_prog, exec_user_prog, exec_user_prog, exec_user_prog, exec_user_prog, 
			exec_user_prog, exec_user_prog, exec_user_prog, exec_user_prog, exec_user_prog, veh_srch,
			drv_srch, user_srch, exec_user_prog,exec_user_prog, exec_user_prog, exec_user_prog, exec_user_prog,
			exec_user_prog};

/* ============================================================================================================================	*
 * process: The main processing function for the user interface. 
 * Who: Naveed Cochinwala 	What: Written		When: 1988
 * ============================================================================================================================	*/
process()
{
	int c;				/* for reading input from user */
	int num_read;			/* # entered by user */
	int i;				/* scratch variable */
	int cursor_column;
	void move_highlight();		/* moves the highlight from one line to the other */

	/* Location of cursor depends on length of last line of menu */
	cursor_column = strlen(catgets(UI_m_catd, UI_1, UI_UI_DISP_M_4, "Enter carriage return to execute selection:  "));


	/* enter a while loop which executes as long as the terminate flag is not set */
	while ( ! terminate )  {
		/*
		 * at this point the menu_manager() has already been called, and the scr_name and scr_type 
		 * variables have been set appropriately. If the screen is a UI_MENU, then it is already on 
		 * display, if the screen is a UI_SCREEN, then the process loop should navigate the program 
		 * such that the appropriate routine is called 
		 */
		if ( scr_type == UI_MENU )  {
		        display_time();
			move(CURSOR_ROW_MENU, cursor_column); 	/* move the cursor to the input position */
			refresh();

			if ( (c = getkey()) == ERR )	 		/* try to read a char from the terminal; this is a */
									/* blocked read of BLOCK_TIME/10 seconds */
				continue;				/* back to while loop */

			if ( isprint(c) )
			  c = Toupper(c);

			if ( clear_err_flag ) 				/* last line has error msg printed, clear it */
				clear_error();

						/* A char has been read. If it is a #, an up_arrow, a down_arrow key or a */
						/* space bar, then highlight the appropriate item on the menu. If it is a */
			 			/* <CR>, then call the menu_manager() with the number of the item highlighted. */
						/* If it is any other key, then call the function key handler. */
			

			if ( isascii(c) && isdigit(c) ) { 			/* char entered is a #, check its validity */
				num_read = c - '0';   				/* convert char to int */
				if (num_read < 1 || num_read > no_of_items) { 	/* # entered is not valid, prt an error msg and */
					prt_error(UI_INVALID_NUM,"%d",num_read);
					continue; 				/* continue with the while loop */
				}  						/* end if invalid # read */

										/* a valid # has been entered by the user; */
				move_highlight(num_highlighted, num_read); 	/* move highlight from the old to new position */
				continue;					/* back to while loop */
			}  							/* end if isdigit(c) */
			else {
				switch ( c ) { 					/* char entered is not a # */
				case M_EXIT:
						if ( scr_name == MAIN_MENU )			/* if on MAIN_MENU then exit */
							terminate = 1;				/* leave while loop */
						else 
							menu_manager(YES, scr_name/10);		/* Move up one menu level */
						break;

					case KEY_DOWN: 						/* move the highlight dowm by 1 # */
					case ' ':
						move_highlight(num_highlighted, num_highlighted+1);
						break;						/* leave switch */
					
					case KEY_UP: 						/* move highlight up by 1 # */
						move_highlight(num_highlighted, num_highlighted-1);
						break;						/* leave switch */

					case '\r': 					/* user has selected item on the menu, */
					case '\n': 					/* call the menu_manager() */
						menu_manager(NO, num_highlighted);
						break;					/* leave switch */

					default: 	/* a function key has been pressed, or an invalid char was entered */
						ftnkey_handler((char *)0,c,0);

				}  							/* end switch */
			}  								/* end else char entered is not a # */
		}  									/* end if scr_type == UI_MENU */

		else 			/* screen == UI_SCREEN; so call the appropriate function to do the processing */
			process_func[map_array[scr_name]]();
	}  										/* end while */
}  											/* end process() */


/******************************************************************************/
/* move_highlight: 	moves the highlight from the item specified by the    */
/*			first argument to the item specified by the second    */
/*			argument 					      */
/******************************************************************************/
void move_highlight(old_num,new_num)
	int old_num;			/* number of item which is at present highlighted */
	int new_num;			/*  # of the new item which should be highlighted */
{

	int indx;			/* index into menus structures in shared memory */

	indx = map_array[scr_name*10 + old_num];
	if ((*security[indx])() == NOT_ALLOWED ) 		/* if permission for item defined by old_num is not allowed, */
		attrset(0); 					/* then set video attributes to dim mode */
	else
		attrset(A_BOLD); 				/* else attrset is already regular mode */

	if ( old_num <= no_of_items ) {
		if ( old_num <= MAX_ITEMS_ON_1ST_COL ) 		/* print the old_num item again with the new video attribute */
								/* item desc appears on the 1st col */
			mvprintw(ROW_1ST_ITEM+(old_num-1)*2, MENU_1ST_COL, " %d. %s ", old_num, ptr_item_desc[old_num-1] );
		else 						/* item desc appears on the 2nd col */
			mvprintw(ROW_1ST_ITEM+(old_num-1-MAX_ITEMS_ON_1ST_COL)*2, MENU_2ND_COL," %d. %s ",
					old_num,ptr_item_desc[old_num-1]);
	}  /* end if */
	if ( new_num > no_of_items ) 				/* if new_num > no_of_items on display, */
		new_num = 1; 					/* then move the highlight to item # 1 */
	else if ( new_num < 1 )
		new_num = no_of_items; 				/* move highlight to last item on menu */

	attrset(A_REVERSE); 					/* print the highlight on th new item desc */
	if ( new_num <= MAX_ITEMS_ON_1ST_COL ) 
		mvprintw(ROW_1ST_ITEM+(new_num-1)*2, MENU_1ST_COL, " %d. %s ", new_num, ptr_item_desc[new_num-1]);
	else 							/* item desc is on the 2nd col on the menu */
		mvprintw(ROW_1ST_ITEM+(new_num-1-MAX_ITEMS_ON_1ST_COL)*2, MENU_2ND_COL," %d. %s ",new_num,ptr_item_desc[new_num-1]);

	num_highlighted = new_num;   				/* new item # that is now highlighted */
	attrset(0);						/* reset mode to normal */
		
								/* do not refresh screen */
}  								/* end move_highlight() */


/****************************************************/
/* process_dummy: a dummy function to be pointed to */
/****************************************************/
process_dummy()
{
}
