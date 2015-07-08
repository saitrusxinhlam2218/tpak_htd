static char sz__FILE__[]="@(#)ui_menu.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_menu.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:51:04
 * @(#)  Last delta: 12/2/94 at 18:27:59
 * @(#)  SCCS File: /taxi/sccs/s.ui_menu.c
 *                                                                           *
 *****************************************************************************/
#include <sys/types.h>
#include "mads_types.h"
#include "calls.h"
#include "screens.h"
#include "ui_def.h"
#include "ui_ipc.h"
#include "mads_isam.h"
#include "ui_error.h"
#include "menu.h"
#include "srch.h"
#include "language.h"

#define		SEARCHES_ON_PG	17


/* global and external variables */
extern int scr_name;	/* name of screen currently on display */
extern char scr_type;	/* type of screen currently on display */
int prev_scr_name;	/* for saving name of previous screen */
char prev_scr_type;	/* for saving previous screen type */
extern struct cisam_cl call_struct;  /* for reading in fields on call entry */
extern struct cisam_cl prev_call_struct;	/* for saving call structure */
int current_prompt_pos = 3; /* current cursor position on call entry scr */
int saved_prev_piu;	/* for saving field position of cursor of prev_scr */
extern struct call_ipc_rec call_fifo;	/* for writing calls to FIFO */
extern struct call_ipc_rec prev_call_fifo;	/* for saving calls structure for prev_scr */
extern int map_array[];	/* maps the scr_names to the indexes used in shared
			 * memory; initizlized in init() */
extern struct menus *cmenu[];	/* array of pointers to menus structure in
				 * shared memory */
int exec_from_scr;	/* screen from which other programs to be execed are
			 * called */
char exec_scr_type;	/* type of screen from which prog is execed */

/* declare functions used to check the users permissions for the various
 * screens 
 */
extern int security_main_menu(), security_call_menu(), security_searches_menu(), 
	security_superv_menu(), security_update_user(), security_system_maint(),
	security_user_menu(), security_basic_scr(), security_extended_scr(), 
	security_wakeup_scr(), security_subs_basic(), security_subs_extended(), 
	security_subs_wakeup(),security_call_user1(), security_call_user2(), 
	security_call_user3(), security_unassigned(),security_subs_fut(),
	security_assigned(), security_all_calls(), security_veh_dr(), 
	security_indiv_cls(), security_exceptions(), security_other_srch(),
	security_srch_userdef1(), security_suspend(), security_zone_status(), 
	security_system_status(), security_edit_repeat_msgs(), 
	security_superv_def1(), security_shift_sched(), security_superv_def3(),
   	security_superv_def4(), security_superv_def5(), security_um_def1(), 
	security_um_def2(), security_um_def3(), security_um_def4(), 
	security_um_def5(), security_um_def6(), security_um_def7(), 
	security_um_def8(), security_um_def9(), security_vehicle_srch(), 
	security_driver_srch(), security_user_srch(),
	security_os_userdef1(), security_os_userdef2(), security_os_userdef3(), 
	security_os_userdef4(), security_os_userdef5(), security_os_userdef6();

/* declare pointers to the functions which check the permission
 * of the user for the various screens
 */
int (*security[])() = { security_main_menu, security_call_menu,
			security_searches_menu, security_superv_menu,
			security_update_user, security_system_maint,
			security_user_menu, security_basic_scr, 
			security_extended_scr, security_wakeup_scr, 
			security_subs_basic, security_subs_extended, 
			security_subs_wakeup,security_call_user1,
			security_call_user2, security_call_user3,
			security_unassigned,security_subs_fut,
			security_assigned, security_all_calls,
			security_veh_dr, security_indiv_cls, 
			security_exceptions, security_other_srch,
			security_srch_userdef1, security_suspend,
			security_zone_status, security_system_status,
			security_edit_repeat_msgs,security_superv_def1, 
			security_shift_sched, security_superv_def3, 
			security_superv_def4, security_superv_def5, 
			security_um_def1, security_um_def2,
			security_um_def3, security_um_def4,
			security_um_def5, security_um_def6,
			security_um_def7, security_um_def8,
			security_um_def9, security_vehicle_srch,
			security_driver_srch, security_user_srch,
			security_os_userdef1, security_os_userdef2,
			security_os_userdef3, security_os_userdef4,
			security_os_userdef5, security_os_userdef6 };

struct select_criterias saved_srch_criteria;
extern struct select_criterias select_criteria;
extern int page_num;
int saved_pg_no;
extern struct cisam_cl srch_calls[], saved_srch_calls[];

/* menu_manager: This routine updates the global variables scr_name, 
 * 		prev_scr, and scr_type. Scr_name is a global short 
 * 		variable which defines which screen is currently 
 *		on display. The different kinds on screens are
 *		defined in the header file "screens.h". Prev_scr 
 *		is a global short variable which defines which was 
 *		the last screen which the user was working with, and 
 *		consequently saved so that it could be later 
 *		accessed  by pressing the <PREVSCR> function key. 
 *		Returns:
 *			DONE -	if functions completes process
 *			NOT_DONE -	if scr_name is not changed 
 *					(eg. if not allowed)
 *			RET_ERROR -	if a processing error occured
 *
 */
menu_manager(cmdkey_flag,cmd_num)
	char cmdkey_flag;	/* YES, or NO, whether the <CMD> 
				 * key was pressed */
	short cmd_num;	/* number entered by the user after pressing the
			 * <CMD> key or when on a UI_MENU screen */
{
		
	static int first_time = 1;	/* is code being excuted the 
					 * first time */

	char save_screen;   /* YES= save screen as prev_scr; n=do not save */
	int menu_index;	/* index into menu structure in shared memory for 
			 * current scr */
	int new_menu_index;	/* index into menu structure in shared memory
				 * for the new screen to be displayed */
	int i;	


	/*  If the <CMD> key was not pressed then the cmd_num passed
	 *  to menu_manager() is a number (we'll call this relative
	 *   cmd_num), which needs to be converted to the absolute
	 *   cmd_num (corresponding to what we have in the menu file)
	 */
	if ( cmdkey_flag == NO )  
		cmd_num += (scr_name * 10 );
	
	/*  If the new screen to be displayed is identical to the current
	 *  screen on display (eg: if the user presses <CMD> then 1 when
	 *  on a CALL_ENTRY_MENU); then the routine should return redisplay
	 *  the same screen again (refresh it again). 
	 */
	if ( cmd_num == scr_name )
		/* new screen and current screens are identical */
		return(DONE);

	/* check if the user has permission for the newly speified screen/menu.
	 * This function also checks that the number entered by the user
	 *  is a valid number.
	 */
	new_menu_index = map_array[cmd_num];
	if ( new_menu_index == -1 )   {
		/* the cmd_num specified does not correspond to an 
		 * existing screen/menu
		 */
		prt_error(UI_NOSCR,"<%d>", cmd_num);
		return(RET_ERROR);
	} 
	if ( (*security[new_menu_index])() == NOT_ALLOWED ) {
		prt_error(UI_NOT_ALLOWED,"%d",cmd_num);
		return(NOT_DONE);
	}

	/* Get the index of the record in the array of menu structures
	 * which corresponds to the screen on display.
	 */
	if ( first_time )  {
		/* the first time, there is no screen on display, so */
		first_time = 0;
		save_screen = NO;
	}	/* end if first_time */
	else {
		menu_index = map_array[scr_name];

		/* Ignore 'fake' screens (e.g. detail) */
		if (menu_index < MENU_MAX) {
		    if ( cmenu[menu_index]->mf_command != scr_name )  {
			/* got wrong index */
			prt_error(UI_MENU_INDEX,"%d", 
				  cmenu[menu_index]->mf_command);
			return(RET_ERROR);
		    }
		    /* Check if the previous screen should be saved or not. */
		    if ( cmenu[menu_index]->mf_prev_scr_flag == YES )
			/* save the old screen */
			save_screen = YES;
		    else 
			save_screen = NO;
		}
		else
		    save_screen = NO;
	}	/* end else */

	/* Get the index of the new screen to be displayed, so that we can
	 * access its structure from the menu file in shared memory.
	 */
	if ( cmenu[new_menu_index]->mf_command != cmd_num )  {
		/* got wrong index */
		prt_error(UI_MENU_INDEX,"%d", cmenu[new_menu_index]->mf_command);
		return(RET_ERROR);
   	}

	/* save the info required for the previous screen */
	if ( save_screen == YES ) {
		/* save previous screen with info */
 		prev_scr_name = scr_name;
	   	prev_scr_type = scr_type;
	   	switch ( prev_scr_name )  {
			case BASIC_CALL_SCR:
			case EXTENDED_CALL_SCR:
			case SUBS_BASIC_SCR:
			case SUBS_EXTENDED_SCR:
			case INDIVIDUAL_CALLS_SCR:
				/* save the call structure with information */
				prev_call_struct = call_struct;
				/* save the position of the cursor */
				saved_prev_piu = current_prompt_pos;
				/* note that both saved_prev_piu and 
				 * current_prompt_pos are global integer
				 * variables. Current_prompt_pos is a
				 * number indicating the field number 
				 * which is to be read next, it is 
				 * set by the routines which check 
				 * the fields entered
				 */
				/* save the call_fifo structure in 
				 * prev_call_struct
				 */
				break;
			case WAKEUP_CALL_SCR:				
			case SUBS_WAKEUP_SCR:
				break;
			case EXCEPTIONS_SCR:
				break;	/* save in exceptions() */
			case UNASSIGNED_CALLS_SCR:
			case SUBS_FUTURE_CLS_SCR:
			case ASSIGNED_CALLS_SCR:
			case ALL_CALLS_SCR:
			case VEH_DR_CALLS_SCR:
				/* save the data structures */
				for ( i = 0; i < SEARCHES_ON_PG; i++) 
					saved_srch_calls[i] = srch_calls[i];
				saved_pg_no = page_num;	/* the searches pg # */
				saved_srch_criteria = select_criteria;
				break;
			case SUSPEND_VEH_DR_SCR:
			case ZONE_STATUS_SCR:
			case SYSTEM_STATUS_SCR:
			case EDIT_REPEAT_MESSAGES:
			case VEHICLE_SRCH_SCR:
			case DRIVER_SRCH_SCR:
			case USER_SRCH_SCR:
				/* depends on how we implement it */
				break;

			default:
				/* for all other screens; just saving the
				 * scr_name and scr_type is sufficient to
				 * go back to it when the <PREV_SCR> key
				 * is pressed
				 */
				break;

		   } /* end switch */
	   }  /* end if save_screen == YES */

	   /* set parameters for the new screen to be displayed */
	   if ( cmenu[new_menu_index]->mf_type == UI_MENU)
		scr_type = UI_MENU;
	   else if ( cmenu[new_menu_index]->mf_type == UI_EXTERNAL) {
		/* save the name of the screen from which the new program
		 * will be execed 
		 */
		exec_from_scr = scr_name;
		exec_scr_type = scr_type;
		scr_type = UI_EXTERNAL;
	   }  /* end else 'E' */
	   else
		/* its an internal executable */
	   	scr_type = UI_SCREEN;

	   scr_name = cmd_num;

	/* If the new screen to be displayed is a UI_MENU, then call the
	 * appropriate routine to display this menu, if it is a screen
	 * then do not display it now.
	 * Display_menu() is an functions for displaying the menus 
	 * on the terminal.
	 */
	if ( scr_type == UI_MENU )
		display_menu(scr_name);

	return(DONE);

}  /* end menu_manager() */


