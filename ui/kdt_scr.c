static char sz__FILE__[]="@(#)kdt_scr.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  kdt_scr.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:49:12
 * @(#)  Last delta: 12/2/94 at 18:16:35
 * @(#)  SCCS File: /taxi/sccs/s.kdt_scr.c
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include "screens.h"				/* defines various screen types */
#include <memory.h>
#include "kdt_ary.h"				/* defines for screen fields */
#include "ui_def.h"
#include "kdt_scr_fd.h"
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "ui_ipc.h"
#include "user.h"
#include "ui_error.h"
#include "df_maint.h"
#include "func_keys.h"

#define	MAX_PIU_KDT	6

						/* declare functions used for checking fields entered */
extern int check_dummy2(), check_fleet2(), check_veh1(),check_action(), check_zone1(),check_driver(),check_pri2(); 

						/* declare an array of pointers to functions which check the field entered */
						/* the functions check to see if the field entered is valid or not */
int (*kdt_check_field[])() = {check_dummy2,check_fleet2,check_veh1, check_action,check_zone1,check_driver,check_pri2};

extern int scr_name;
struct cisam_vh *vh_ptr;			/* for saving record of prev scr */
struct cisam_dr *dr_ptr;			/* for saving record of prev scr */
struct cisam_zn *zn_ptr;			/* for saving record of prev scr */
struct kdt_emulate Kdt;
extern struct cisam_cl prev_call_struct;	/* for saving record of prev scr */
extern struct call_ipc_rec call_fifo;		/* for writing record to FIFO */
extern struct call_ipc_rec prev_call_fifo;		/* for saving call_fifo struct of prev scr */
int dr_flags = 0;
int vh_flags = 0;
int zn_flags = 0;
int pr_flags = 0;
int act_flags = 0;
int valid_flags = 0;
char ft_id;
extern char req_buf[];
extern struct fleets *fleet[];
extern int def_fl_index;
extern WINDOW *srch_win;
extern struct users *uiuser[];			/* array of pointers to users structure in shared memory */
extern int user_index;				/* index into users struture in shared memory */
int end_kdt = 0;				/* flag for ending loop of sp_call() */
extern short clear_err_flag;			/* set if last line has an error msg printed */
extern struct keydesc dr_key1, veh_key2, zn_key1;			/* for db() calls */

kdt_emulation()
{
	int piu = 2;				/* prompt in use */
	int field_entered = 0;			/* whether field entered or not */
	int key_val;				/* value of the key pressed by the user */
	char read_buf[READ_BUF_SIZE];		/* buffer for reading fields on screen */
	int ret_ck_fld;				/* return value of check field routine */
	short max_piu;				/* max value of piu */
	struct scr_flds *ptr_scr_fld;		/* pointer to 1st element of array of piu structures */
	int should_clear = 0;
	int old_scr_name;

	old_scr_name = scr_name;
	scr_name = KDT_EMULATION_WIN_SCR;

	vh_ptr = (struct cisam_vh *)req_buf;
	dr_ptr = (struct cisam_dr *)req_buf;
	zn_ptr = (struct cisam_zn *)req_buf;

	werase(srch_win);			/* erase what was in the window */
	disp_kdt_lab();				/* display window labels */

						/* initialize ptr_scr_fld to the address of kdt_scr_fld[] array of structure */
	ptr_scr_fld = kdt_scr_flds;		/* points to 1st element of structure */
	max_piu = MAX_PIU_KDT;

	prt_entry_areas(srch_win,ptr_scr_fld,max_piu);			/* print entry ares underlinded */
	wattrset(srch_win,A_UNDERLINE);
	if (def_fl_index == -1)
		mvwprintw(srch_win,D_FLEET_ROW,D_FLEET_COL,"%c",' ');
	else	{
		valid_flags = 1;
		ft_id = fleet[def_fl_index]->fleet_id;
		mvwprintw(srch_win,D_FLEET_ROW,D_FLEET_COL,"%c",ft_id);
	}
	wattrset(srch_win,0);
	end_kdt = 0;
	while(!end_kdt)	{
									/* Read the field defined by "piu" */
	      	key_val = read_field(srch_win,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);
		if (key_val == -1)					/* read_field() timed out before field was entered */
			continue;					/* so go back to the same field */

		if (clear_err_flag) {				/* if last line has an error msg printed on it, then clear it */
			clear_error();
			refresh();
		}
								/* Verify that the field entered is correct */
		if(key_val == COMMAND_KEY || key_val == KDT_EMULATE_KEY)
			scr_name = old_scr_name;
	     	if ((ret_ck_fld = (*kdt_check_field[piu])(&piu, read_buf,key_val, &field_entered)) == DONE) {
			if (piu > max_piu)
				piu = 1; 			/* wrap around */
			else if (piu == 0)
				piu = max_piu;			/* wrap around to last field */
		}
		if (ret_ck_fld == 2) 
			return;
		if (ret_ck_fld == NOT_DONE)			/* check_field returned NOT_DONE;call ftnkey_handler()*/
			ftnkey_handler(&piu,key_val,0);
	}

	werase(srch_win);
	wrefresh(srch_win);
	touchwin(stdscr);
	refresh();

	scr_name = old_scr_name;
}
