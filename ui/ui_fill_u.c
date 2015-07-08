static char sz__FILE__[]="@(#)ui_fill_u.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_fill_u.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:50:43
 * @(#)  Last delta: 12/2/94 at 18:27:42
 * @(#)  SCCS File: /taxi/sccs/s.ui_fill_u.c
 *                                                                           *
 *****************************************************************************/
#include "user.h"
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "except.h"
#include "mad_ipc.h"
#include "ui_def.h"
#include "language.h"

extern char req_buf[];				/* buffer for reading/writing cisam records */
extern int user_index;				/* index into users structure in shared memory */
extern struct users *uiuser[];			/* array of pointers to users structures in in shared memory */
extern int pid;					/* process id of this user interface program */
extern struct fleets *fleet[];			/* array of pointers to fleets structure in shared memory */
int def_fl_index;				/* index into fleets struct in shared memory corresponding to users default fleet */
int cur_fl_index;				/* index of the fleet, the user is currently working with */
struct offsets *offset;
struct except_bits selected_except;		/* describes which exception types the user has selected */
extern char excpt_fl[];				/* fleets selected for exception processing */
extern int (*ui_fleet_perms[])();
char logo_file[22];				/* name of logo_file to be displayed, when user logs out */

/************************************************************************************************/
/* fill_user: 	fills the appropriate users structure in shared memory from			*/
/*		information from the ciasam file on disk					*/
/************************************************************************************************/
void fill_user()
{
	char *memset();
	int i;	/* scratch variable */
	struct cisam_up *user_ptr;	/* pointer to user profile record 
					 * read from disk  */
	
	struct excpt_desc *exptr;	/* pointer to 1st element in array 
					 * of structs */
	int type_ary[EXC_TYPES];	/* for saving execption types 
					 * associated with each group */
	int type;	/* for loops */

	/* initialize the users struct in shared memory for this user */
	memset((char *)uiuser[user_index],'\0',sizeof(struct users));

	/* initialize poinetr to 1st element of excpt_desc structure */
	exptr = offset->except_desc;

	user_ptr = (struct cisam_up *)req_buf;

	/* copy info from req_buf[] into appropriate slot in users array
	 * of structure in shared memory
	 */
	
	uiuser[user_index]->user_id = user_ptr->user_id;
	strcpy(uiuser[user_index]->greeting_name, user_ptr->greeting_name);
	uiuser[user_index]->user_pos = user_ptr->default_pos;

	uiuser[user_index]->default_screen = user_ptr->default_screen;

	uiuser[user_index]->default_search = user_ptr->default_search;

	/* save the index of the fleet which corresponds to the users 
	 * default fleet */
	def_fl_index = find_findex(user_ptr->default_fleet);
	
	/* set cur_fl_index to the default fleet index */
	cur_fl_index = def_fl_index;
		
	uiuser[user_index]->default_call_type = user_ptr->default_call_tp;
	strcpy(uiuser[user_index]->default_city,user_ptr->default_city);
	uiuser[user_index]->can_zone_calls = user_ptr->can_zone_calls;
	uiuser[user_index]->highest_call_pri = user_ptr->high_call_pri;

	/* copy zones/superzones */
	uiuser[user_index]->zones_sel[0] = user_ptr->superzone_1;
	uiuser[user_index]->zones_sel[1] = user_ptr->superzone_2;
	uiuser[user_index]->zones_sel[2] = user_ptr->superzone_3;
	uiuser[user_index]->zones_sel[3] = user_ptr->superzone_4;
	uiuser[user_index]->zones_sel[4] = user_ptr->superzone_5;
	uiuser[user_index]->zones_sel[5] = user_ptr->superzone_6;
	uiuser[user_index]->zones_sel[6] = user_ptr->superzone_7;
	uiuser[user_index]->zones_sel[7] = user_ptr->superzone_8;
	uiuser[user_index]->zones_sel[8] = user_ptr->superzone_9;
	uiuser[user_index]->zones_sel[9] = user_ptr->superzone_10;
	uiuser[user_index]->zones_sel[10] = user_ptr->superzone_11;
	uiuser[user_index]->zones_sel[11] = user_ptr->superzone_12;
	uiuser[user_index]->zones_sel[12] = user_ptr->superzone_13;
	uiuser[user_index]->zones_sel[13] = user_ptr->superzone_14;
	uiuser[user_index]->zones_sel[14] = user_ptr->superzone_15;
	uiuser[user_index]->zones_sel[15] = user_ptr->superzone_16;

	uiuser[user_index]->p_id = pid;							/* copy process id */
											/* copy fleet permissions */
	if (user_ptr->fleet_perms[0] == YES) {
		uiuser[user_index]->fleet_perms.fleet0 = 1;
		uiuser[user_index]->orig_fleet_perms.fleet0 = 1;
	}
	else {
		uiuser[user_index]->fleet_perms.fleet0 = 0;
		uiuser[user_index]->orig_fleet_perms.fleet0 = 0;
	}

	if (user_ptr->fleet_perms[1] == YES) {
		uiuser[user_index]->fleet_perms.fleet1 = 1;
		uiuser[user_index]->orig_fleet_perms.fleet1 = 1;
	}
	else {
		uiuser[user_index]->fleet_perms.fleet1 = 0;
		uiuser[user_index]->orig_fleet_perms.fleet1 = 0;
	}

	if (user_ptr->fleet_perms[2] == YES) {
		uiuser[user_index]->fleet_perms.fleet2 = 1;
		uiuser[user_index]->orig_fleet_perms.fleet2 = 1;
	}
	else {
		uiuser[user_index]->fleet_perms.fleet2 = 0;
		uiuser[user_index]->orig_fleet_perms.fleet2 = 0;
	}

	if (user_ptr->fleet_perms[3] == YES) {
		uiuser[user_index]->fleet_perms.fleet3 = 1;
		uiuser[user_index]->orig_fleet_perms.fleet3 = 1;
	}
	else {
		uiuser[user_index]->fleet_perms.fleet3 = 0;
		uiuser[user_index]->orig_fleet_perms.fleet3 = 0;
	}

	if (user_ptr->fleet_perms[4] == YES) {
		uiuser[user_index]->fleet_perms.fleet4 = 1;
		uiuser[user_index]->orig_fleet_perms.fleet4 = 1;
	}
	else {
		uiuser[user_index]->fleet_perms.fleet4 = 0;
		uiuser[user_index]->orig_fleet_perms.fleet4 = 0;
	}

	if (user_ptr->fleet_perms[5] == YES) {
		uiuser[user_index]->fleet_perms.fleet5 = 1;
		uiuser[user_index]->orig_fleet_perms.fleet5 = 1;
	}
	else {
		uiuser[user_index]->fleet_perms.fleet5 = 0;
		uiuser[user_index]->orig_fleet_perms.fleet5 = 0;
	}

	if (user_ptr->fleet_perms[6] == YES) {
		uiuser[user_index]->fleet_perms.fleet6 = 1;
		uiuser[user_index]->orig_fleet_perms.fleet6 = 1;
	}
	else {
		uiuser[user_index]->fleet_perms.fleet6 = 0;
		uiuser[user_index]->orig_fleet_perms.fleet6 = 0;
	}

	if (user_ptr->fleet_perms[7] == YES) {
		uiuser[user_index]->fleet_perms.fleet7 = 1;
		uiuser[user_index]->orig_fleet_perms.fleet7 = 1;
	}
	else {
		uiuser[user_index]->fleet_perms.fleet7 = 0;
		uiuser[user_index]->orig_fleet_perms.fleet7 = 0;
	}

											/* copy menu permissions */
	if ( user_ptr->menu_perms[0] == YES)
		uiuser[user_index]->menu_perms.main_menu = 1;
	else
		uiuser[user_index]->menu_perms.main_menu = 0;
		
	/* on the main menu */
	if ( user_ptr->menu_perms[1] == YES)
		uiuser[user_index]->menu_perms.call_menu = 1;
	else
		uiuser[user_index]->menu_perms.call_menu = 0;

	if ( user_ptr->menu_perms[2] == YES)
		uiuser[user_index]->menu_perms.searches_menu = 1;
	else
		uiuser[user_index]->menu_perms.searches_menu = 0;

	if ( user_ptr->menu_perms[3] == YES)
		uiuser[user_index]->menu_perms.supervisory_menu = 1;
	else
		uiuser[user_index]->menu_perms.supervisory_menu = 0;

	if ( user_ptr->menu_perms[4] == YES)
		uiuser[user_index]->menu_perms.update_user_def = 1;
	else
		uiuser[user_index]->menu_perms.update_user_def = 0;

	if ( user_ptr->menu_perms[5] == YES)
		uiuser[user_index]->menu_perms.system_maint = 1;
	else
		uiuser[user_index]->menu_perms.system_maint = 0;

	if ( user_ptr->menu_perms[6] == YES)
		uiuser[user_index]->menu_perms.user_def_menu = 1;
	else
		uiuser[user_index]->menu_perms.user_def_menu = 0;

	if ( user_ptr->menu_perms[7] == YES)
		uiuser[user_index]->menu_perms.basic_call = 1;
	else
		uiuser[user_index]->menu_perms.basic_call = 0;

	if ( user_ptr->menu_perms[8] == YES)
		uiuser[user_index]->menu_perms.extended_call = 1;
	else
		uiuser[user_index]->menu_perms.extended_call = 0;
		
	if ( user_ptr->menu_perms[9] == YES)
		uiuser[user_index]->menu_perms.wakeup_call = 1;
	else
		uiuser[user_index]->menu_perms.wakeup_call = 0;

	if ( user_ptr->menu_perms[10] == YES)
		uiuser[user_index]->menu_perms.basic_subs = 1;
	else
		uiuser[user_index]->menu_perms.basic_subs = 0;

	if ( user_ptr->menu_perms[11] == YES)
		uiuser[user_index]->menu_perms.extended_subs = 1;
	else
		uiuser[user_index]->menu_perms.extended_subs = 0;

	if ( user_ptr->menu_perms[12] == YES)
		uiuser[user_index]->menu_perms.wakeup_subs = 1;
	else
		uiuser[user_index]->menu_perms.wakeup_subs = 0;

	if ( user_ptr->menu_perms[13] == YES)
		uiuser[user_index]->menu_perms.user_call_def1 = 1;
	else
		uiuser[user_index]->menu_perms.user_call_def1 = 0;

	if ( user_ptr->menu_perms[14] == YES)
		uiuser[user_index]->menu_perms.user_call_def2 = 1;
	else
		uiuser[user_index]->menu_perms.user_call_def2 = 0;

	if ( user_ptr->menu_perms[15] == YES)
		uiuser[user_index]->menu_perms.user_call_def3 = 1;
	else
		uiuser[user_index]->menu_perms.user_call_def3 = 0;

	if ( user_ptr->menu_perms[16] == YES)
		uiuser[user_index]->menu_perms.unassigned_cls = 1;
	else
		uiuser[user_index]->menu_perms.unassigned_cls = 0;
		
	if ( user_ptr->menu_perms[17] == YES)
		uiuser[user_index]->menu_perms.subs_fut_cls = 1;
	else
		uiuser[user_index]->menu_perms.subs_fut_cls = 0;

	if ( user_ptr->menu_perms[18] == YES)
		uiuser[user_index]->menu_perms.assigned_cls = 1;
	else
		uiuser[user_index]->menu_perms.assigned_cls = 0;

	if ( user_ptr->menu_perms[19] == YES)
		uiuser[user_index]->menu_perms.all_calls = 1;
	else
		uiuser[user_index]->menu_perms.all_calls = 0;

	if ( user_ptr->menu_perms[20] == YES)
		uiuser[user_index]->menu_perms.veh_dr_cls = 1;
	else
		uiuser[user_index]->menu_perms.veh_dr_cls = 0;

	if ( user_ptr->menu_perms[21] == YES)
		uiuser[user_index]->menu_perms.individual_cls = 1;
	else
		uiuser[user_index]->menu_perms.individual_cls = 0;

	if ( user_ptr->menu_perms[22] == YES)
		uiuser[user_index]->menu_perms.exceptions = 1;
	else
		uiuser[user_index]->menu_perms.exceptions = 0;

	if ( user_ptr->menu_perms[23] == YES)
		uiuser[user_index]->menu_perms.other_searches = 1;
	else
		uiuser[user_index]->menu_perms.other_searches = 0;

	if ( user_ptr->menu_perms[24] == YES)
		uiuser[user_index]->menu_perms.user_srch_usr_def = 1;
	else
		uiuser[user_index]->menu_perms.user_srch_usr_def = 0;
		
	/* on the dispatchers menu */
	if ( user_ptr->menu_perms[25] == YES)
		uiuser[user_index]->menu_perms.suspend_veh_dr = 1;
	else
		uiuser[user_index]->menu_perms.suspend_veh_dr = 0;

	if ( user_ptr->menu_perms[26] == YES)
		uiuser[user_index]->menu_perms.zone_status = 1;
	else
		uiuser[user_index]->menu_perms.zone_status = 0;

	if ( user_ptr->menu_perms[27] == YES)
		uiuser[user_index]->menu_perms.system_status = 1;
	else
		uiuser[user_index]->menu_perms.system_status = 0;

	if ( user_ptr->menu_perms[28] == YES)
		uiuser[user_index]->menu_perms.edit_repeat_msgs = 1;
	else
		uiuser[user_index]->menu_perms.edit_repeat_msgs = 0;

	if ( user_ptr->menu_perms[29] == YES)
		uiuser[user_index]->menu_perms.superv_user_def1 = 1;
	else
		uiuser[user_index]->menu_perms.superv_user_def1 = 0;
		
	if ( user_ptr->menu_perms[30] == YES)
		uiuser[user_index]->menu_perms.shift_sched = 1;
	else
		uiuser[user_index]->menu_perms.shift_sched = 0;

	if ( user_ptr->menu_perms[31] == YES)
		uiuser[user_index]->menu_perms.superv_user_def3 = 1;
	else
		uiuser[user_index]->menu_perms.superv_user_def3 = 0;

	if ( user_ptr->menu_perms[32] == YES)
		uiuser[user_index]->menu_perms.superv_user_def4 = 1;
	else
		uiuser[user_index]->menu_perms.superv_user_def4 = 0;

	if ( user_ptr->menu_perms[33] == YES)
		uiuser[user_index]->menu_perms.superv_user_def5 = 1;
	else
		uiuser[user_index]->menu_perms.superv_user_def5 = 0;

	/* on the user defined menu */
	if ( user_ptr->menu_perms[34] == YES)
		uiuser[user_index]->menu_perms.user_def_1 = 1;
	else
		uiuser[user_index]->menu_perms.user_def_1 = 0;

	if ( user_ptr->menu_perms[35] == YES)
		uiuser[user_index]->menu_perms.user_def_2 = 1;
	else
		uiuser[user_index]->menu_perms.user_def_2 = 0;

	if ( user_ptr->menu_perms[36] == YES)
		uiuser[user_index]->menu_perms.user_def_3 = 1;
	else
		uiuser[user_index]->menu_perms.user_def_3 = 0;


	if ( user_ptr->menu_perms[37] == YES)
		uiuser[user_index]->menu_perms.user_def_4 = 1;
	else
		uiuser[user_index]->menu_perms.user_def_4 = 0;
		
	if ( user_ptr->menu_perms[38] == YES)
		uiuser[user_index]->menu_perms.user_def_5 = 1;
	else
		uiuser[user_index]->menu_perms.user_def_5 = 0;

	if ( user_ptr->menu_perms[39] == YES)
		uiuser[user_index]->menu_perms.user_def_6 = 1;
	else
		uiuser[user_index]->menu_perms.user_def_6 = 0;

	if ( user_ptr->menu_perms[40] == YES)
		uiuser[user_index]->menu_perms.user_def_7 = 1;
	else
		uiuser[user_index]->menu_perms.user_def_7 = 0;

	if ( user_ptr->menu_perms[41] == YES)
		uiuser[user_index]->menu_perms.user_def_8 = 1;
	else
		uiuser[user_index]->menu_perms.user_def_8 = 0;

	if ( user_ptr->menu_perms[42] == YES)
		uiuser[user_index]->menu_perms.user_def_9 = 1;
	else
		uiuser[user_index]->menu_perms.user_def_9 = 0;

	/* on other seraches menu */
	if ( user_ptr->menu_perms[43] == YES)
		uiuser[user_index]->menu_perms.vehicle_srch = 1;
	else
		uiuser[user_index]->menu_perms.vehicle_srch = 0;

	if ( user_ptr->menu_perms[44] == YES)
		uiuser[user_index]->menu_perms.driver_srch = 1;
	else
		uiuser[user_index]->menu_perms.driver_srch = 0;

	if ( user_ptr->menu_perms[45] == YES)
		uiuser[user_index]->menu_perms.user_srch = 1;
	else
		uiuser[user_index]->menu_perms.user_srch = 0;

	if ( user_ptr->menu_perms[46] == YES)
		uiuser[user_index]->menu_perms.srch_user_def1 = 1;
	else
		uiuser[user_index]->menu_perms.srch_user_def1 = 0;

	if ( user_ptr->menu_perms[47] == YES)
		uiuser[user_index]->menu_perms.srch_user_def2 = 1;
	else
		uiuser[user_index]->menu_perms.srch_user_def2 = 0;

	if ( user_ptr->menu_perms[48] == YES)
		uiuser[user_index]->menu_perms.srch_user_def3 = 1;
	else
		uiuser[user_index]->menu_perms.srch_user_def3 = 0;

	if ( user_ptr->menu_perms[49] == YES)
		uiuser[user_index]->menu_perms.srch_user_def4 = 1;
	else
		uiuser[user_index]->menu_perms.srch_user_def4 = 0;

	if ( user_ptr->menu_perms[50] == YES)
		uiuser[user_index]->menu_perms.srch_user_def5  =  1;
	else
		uiuser[user_index]->menu_perms.srch_user_def5 = 0;
		
	if ( user_ptr->menu_perms[51] == YES)
		uiuser[user_index]->menu_perms.srch_user_def6 = 1;
	else
		uiuser[user_index]->menu_perms.srch_user_def6 = 0;

	/* not used bits */
	if ( user_ptr->menu_perms[52] == YES)
		uiuser[user_index]->menu_perms.not_used_1 = 1;
	else
		uiuser[user_index]->menu_perms.not_used_1 = 0;

	if ( user_ptr->menu_perms[53] == YES)
		uiuser[user_index]->menu_perms.not_used_2 = 1;
	else
		uiuser[user_index]->menu_perms.not_used_2 = 0;

	if ( user_ptr->menu_perms[54] == YES)
		uiuser[user_index]->menu_perms.not_used_3 = 1;
	else
		uiuser[user_index]->menu_perms.not_used_3 = 0;

	if ( user_ptr->menu_perms[55] == YES)
		uiuser[user_index]->menu_perms.not_used_4 = 1;
	else
		uiuser[user_index]->menu_perms.not_used_4 = 0;


	/* set up exception types */
	if ( user_ptr->type_1 == YES )
		uiuser[user_index]->exc_grp_slct[0] = (exptr+0)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[0] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_2 == YES )
		uiuser[user_index]->exc_grp_slct[1] = (exptr+1)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[1] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_3 == YES )
		uiuser[user_index]->exc_grp_slct[2] = (exptr+2)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[2] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_4 == YES )
		uiuser[user_index]->exc_grp_slct[3] = (exptr+3)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[3] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_5 == YES )
		uiuser[user_index]->exc_grp_slct[4] = (exptr+4)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[4] = 0;	/* exception type #1 not sel */

	if ( user_ptr->type_6 == YES )
		uiuser[user_index]->exc_grp_slct[5] = (exptr+5)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[5] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_7 == YES )
		uiuser[user_index]->exc_grp_slct[6] = (exptr+6)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[6] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_8 == YES )
		uiuser[user_index]->exc_grp_slct[7] = (exptr+7)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[7] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_9 == YES )
		uiuser[user_index]->exc_grp_slct[8] = (exptr+8)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[8] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_10 == YES )
		uiuser[user_index]->exc_grp_slct[9] = (exptr+9)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[9] = 0;	/* exception type #1 not sel */

	if ( user_ptr->type_11 == YES )
		uiuser[user_index]->exc_grp_slct[10] = (exptr+10)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[10] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_12 == YES )
		uiuser[user_index]->exc_grp_slct[11] = (exptr+11)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[11] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_13 == YES )
		uiuser[user_index]->exc_grp_slct[12] = (exptr+12)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[12] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_14 == YES )
		uiuser[user_index]->exc_grp_slct[13] = (exptr+13)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[13] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_15 == YES )
		uiuser[user_index]->exc_grp_slct[14] = (exptr+14)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[14] = 0;	/* exception type #1 not sel */

	if ( user_ptr->type_16 == YES )
		uiuser[user_index]->exc_grp_slct[15] = (exptr+15)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[15] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_17 == YES )
		uiuser[user_index]->exc_grp_slct[16] = (exptr+16)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[16] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_18 == YES )
		uiuser[user_index]->exc_grp_slct[17] = (exptr+17)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[17] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_19 == YES )
		uiuser[user_index]->exc_grp_slct[18] = (exptr+18)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[18] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_20 == YES )
		uiuser[user_index]->exc_grp_slct[19] = (exptr+19)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[19] = 0;	/* exception type #1 not sel */

	if ( user_ptr->type_21 == YES )
		uiuser[user_index]->exc_grp_slct[20] = (exptr+20)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[20] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_22 == YES )
		uiuser[user_index]->exc_grp_slct[21] = (exptr+21)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[21] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_23 == YES )
		uiuser[user_index]->exc_grp_slct[22] = (exptr+22)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[22] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_24 == YES )
		uiuser[user_index]->exc_grp_slct[23] = (exptr+23)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[23] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_25 == YES )
		uiuser[user_index]->exc_grp_slct[24] = (exptr+24)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[24] = 0;	/* exception type #1 not sel */

	if ( user_ptr->type_26 == YES )
		uiuser[user_index]->exc_grp_slct[25] = (exptr+25)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[25] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_27 == YES )
		uiuser[user_index]->exc_grp_slct[26] = (exptr+26)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[26] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_28 == YES )
		uiuser[user_index]->exc_grp_slct[27] = (exptr+27)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[27] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_29 == YES )
		uiuser[user_index]->exc_grp_slct[28] = (exptr+28)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[28] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_30 == YES )
		uiuser[user_index]->exc_grp_slct[29] = (exptr+29)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[29] = 0;	/* exception type #1 not sel */

	if ( user_ptr->type_31 == YES )
		uiuser[user_index]->exc_grp_slct[30] = (exptr+30)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[30] = 0;	/* exception type #1 not sel */
	if ( user_ptr->type_32 == YES )
		uiuser[user_index]->exc_grp_slct[31] = (exptr+31)->group_nbr;
	else
		uiuser[user_index]->exc_grp_slct[31] = 0;	/* exception type #1 not sel */

	
	for (i = 0; i < FLEET_MAX; i++) {				/* set the fleets selected for exceptions */
		if ((*ui_fleet_perms[i])() == ALLOWED && fleet[i]->active == YES)
			excpt_fl[i] = fleet[i]->fleet_id;
		else 
			excpt_fl[i] = '\0';
	}

	/* initialize type_ary */
	for ( i = 0; i < EXC_TYPES; i++ )
		type_ary[i] = -1;

	if (user_ptr->group_1 == NO) {			/* user deselected all the exception types which belong to group # 1 */
		get_types_from_group(type_ary,1);
		i = 0;
		while (type_ary[i] != -1) {
			uiuser[user_index]->exc_grp_slct[type_ary[i]] = 0;
			i++;
		}
	}

	if (user_ptr->group_2 == NO) {			/* user deselected all the exception types which belong to group # 2 */
		get_types_from_group(type_ary,2);
		i = 0;
		while (type_ary[i] != -1) {
			uiuser[user_index]->exc_grp_slct[type_ary[i]] = 0;
			i++;
		}
	}

	if (user_ptr->group_3 == NO) {			/* user deselected all the exception types which belong to group # 3 */
		get_types_from_group(type_ary,3);
		i = 0;
		while (type_ary[i] != -1) {
			uiuser[user_index]->exc_grp_slct[type_ary[i]] = 0;
			i++;
		}
	}

	if (user_ptr->group_4 == NO) {			/* user deselected all the exception types which belong to group # 4 */
		get_types_from_group(type_ary,4);
		i = 0;
		while (type_ary[i] != -1) {
			uiuser[user_index]->exc_grp_slct[type_ary[i]] = 0;
			i++;
		}
	}

	if (user_ptr->group_5 == NO) {			/* user deselected all the exception types which belong to group # 5 */
		get_types_from_group(type_ary,5);
		i = 0;
		while (type_ary[i] != -1) {
			uiuser[user_index]->exc_grp_slct[type_ary[i]] = 0;
			i++;
		}
	}

	if (user_ptr->group_6 == NO) {			/* user deselected all the exception types which belong to group # 6 */
		get_types_from_group(type_ary,6);
		i = 0;
		while (type_ary[i] != -1) {
			uiuser[user_index]->exc_grp_slct[type_ary[i]] = 0;
			i++;
		}
	}

	if (user_ptr->group_7 == NO) {			/* user deselected all the exception types which belong to group # 7 */
		get_types_from_group(type_ary,7);
		i = 0;
		while (type_ary[i] != -1) {
			uiuser[user_index]->exc_grp_slct[type_ary[i]] = 0;
			i++;
		}
	}

	if (user_ptr->group_8 == NO) {			/* user deselected all the exception types which belong to group # 8 */
		get_types_from_group(type_ary,8);
		i = 0;
		while (type_ary[i] != -1) {
			uiuser[user_index]->exc_grp_slct[type_ary[i]] = 0;
			i++;
		}
	}

	if (user_ptr->group_9 == NO) {			/* user deselected all the exception types which belong to group # 9 */
		get_types_from_group(type_ary,9);
		i = 0;
		while (type_ary[i] != -1) {
			uiuser[user_index]->exc_grp_slct[type_ary[i]] = 0;
			i++;
		}
	}

	if (user_ptr->group_10 == NO) {		/* user deselected all the exception types which belong to group # 10 */
		get_types_from_group(type_ary,10);
		i = 0;
		while (type_ary[i] != -1) {
			uiuser[user_index]->exc_grp_slct[type_ary[i]] = 0;
			i++;
		}
	}

	if (user_ptr->group_11== NO) {			/* user deselected all the exception types which belong to group # 11 */
		get_types_from_group(type_ary,11);
		i = 0;
		while (type_ary[i] != -1) {
			uiuser[user_index]->exc_grp_slct[type_ary[i]] = 0;
			i++;
		}
	}

	if (user_ptr->group_12== NO) {			/* user deselected all the exception types which  belong to group # 12 */
		get_types_from_group(type_ary,12);
		i = 0;
		while (type_ary[i] != -1) {
			uiuser[user_index]->exc_grp_slct[type_ary[i]] = 0;
			i++;
		}
	}
						/* now save the exception types that the user has selected in the bit structure */
	for (type = 0; type < EXC_TYPES; type++) {
		if (uiuser[user_index]->exc_grp_slct[type] > 0)				/* user has selected this exception type */
			set_bit(type,sizeof(struct except_bits),(unsigned char *)&selected_except);	/* set the bit on */
	}
	
	strcpy(logo_file,user_ptr->logo_file);		/* copy name of the logo_file to be displayed, whne the user logs out */

	uiuser[user_index]->auto_free_text = user_ptr->auto_free_text;

#ifdef NAMERICA
	if ( user_ptr->hia_1 == YES )
	  uiuser[user_index]->channels[0] = 1;
	else
	  uiuser[user_index]->channels[0] = 0;
	if ( user_ptr->hia_2 == YES )
	  uiuser[user_index]->channels[1] = 1;
	else
	  uiuser[user_index]->channels[1] = 0;
	if ( user_ptr->hia_3 == YES )
	  uiuser[user_index]->channels[2] = 1;
	else
	  uiuser[user_index]->channels[2] = 0;
	if ( user_ptr->hia_4 == YES )
	  uiuser[user_index]->channels[3] = 1;
	else
	  uiuser[user_index]->channels[3] = 0;
	if ( user_ptr->hia_5 == YES )
	  uiuser[user_index]->channels[4] = 1;
	else
	  uiuser[user_index]->channels[4] = 0;
	if ( user_ptr->hia_6 == YES )
	  uiuser[user_index]->channels[5] = 1;
	else
	  uiuser[user_index]->channels[5] = 0;
	if ( user_ptr->hia_7 == YES )
	  uiuser[user_index]->channels[6] = 1;
	else
	  uiuser[user_index]->channels[6] = 0;
	if ( user_ptr->hia_8 == YES )
	  uiuser[user_index]->channels[7] = 1;
	else
	  uiuser[user_index]->channels[7] = 0;
	if ( user_ptr->hia_9 == YES )
	  uiuser[user_index]->channels[8] = 1;
	else
	  uiuser[user_index]->channels[8] = 0;
	if ( user_ptr->hia_10 == YES )
	  uiuser[user_index]->channels[9] = 1;
	else
	  uiuser[user_index]->channels[9] = 0;
	if ( user_ptr->hia_11 == YES )
	  uiuser[user_index]->channels[10] = 1;
	else
	  uiuser[user_index]->channels[10] = 0;
	if ( user_ptr->hia_12 == YES )
	  uiuser[user_index]->channels[11] = 1;
	else
	  uiuser[user_index]->channels[11] = 0;
	if ( user_ptr->hia_13 == YES )
	  uiuser[user_index]->channels[12] = 1;
	else
	  uiuser[user_index]->channels[12] = 0;
	if ( user_ptr->hia_14 == YES )
	  uiuser[user_index]->channels[13] = 1;
	else
	  uiuser[user_index]->channels[13] = 0;
	if ( user_ptr->hia_15 == YES )
	  uiuser[user_index]->channels[14] = 1;
	else
	  uiuser[user_index]->channels[14] = 0;
	if ( user_ptr->hia_16 == YES )
	  uiuser[user_index]->channels[15] = 1;
	else
	  uiuser[user_index]->channels[15] = 0;
	if ( user_ptr->hia_17 == YES )
	  uiuser[user_index]->channels[16] = 1;
	else
	  uiuser[user_index]->channels[16] = 0;
	if ( user_ptr->hia_18 == YES )
	  uiuser[user_index]->channels[17] = 1;
	else
	  uiuser[user_index]->channels[17] = 0;
#endif

}



/* find_findex: finds the index into the fleets structure in shared memory.
 *	Returns: the index into the fleets structure in shared memory.
 *		 -1 if the fleet specified by the argument does not exist
 */
int find_findex(input_fleet)
	char input_fleet;  /* fleet whose index into shared memory is needed */
{
	int i;

	for ( i=0; i < FLEET_MAX; i++ ) {
		if ( fleet[i]->fleet_id == input_fleet ) 
			return(i);
	} /* end for */
	return(-1);	 /* error */
}  /* end find_findex() */


/* get_types_from_group: gets the exception types belonging to the
 *		group specified bu its argument
 */
get_types_from_group(type_ary, group_num)
	int *type_ary; /* arryty to save exception types in */
	short group_num;	/* group fro which types are to be found */
{

	int type, j;
	struct excpt_desc *exptr;

	/* initialize exptr to start of except desc struct in shared memory */
	exptr = offset->except_desc;

	j = 0;
	for ( type = 0; type < EXC_TYPES; type++)  {
		if ( (exptr+type)->group_nbr == (short)group_num ) {
			type_ary[j] = type;
			j++;
		}
	}  /* end for */

}  /* end get_types_from_group()  */

