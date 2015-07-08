static char sz__FILE__[]="@(#)ui_securty.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_securty.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:51:37
 * @(#)  Last delta: 12/2/94 at 18:28:20
 * @(#)  SCCS File: /taxi/sccs/s.ui_securty.c
 *                                                                           *
 *****************************************************************************/
#include "ui_def.h"
#include "screens.h"
#include "user.h"
#include "menu.h"
#include "ui_error.h"
#include "language.h"

extern int user_index;				/* index into users structure in shared memory */
extern int map_array[];				/* array of ints, to index into menus structure in shared memory */
extern struct users *uiuser[];			/* array of pointers to users struct in in shared memory */
extern struct menus *cmenu[];			/* array of pointers to menus structure in shared memory */
extern int cur_fl_index;			/* index of current fleet user is working with */

/********************************************************************************************************/
/* security: These functions check if the user has permission to execute/view the screen/menu/command.	*/
/* If the user does not have permission, then NOT_ALLOWED is returned.					*/
/* If the user has permission, then ALLOWED is returned.						*/
/********************************************************************************************************/

/*******************************************************/
/* security_main_menu: checks permission for main menu */
/*******************************************************/
int security_main_menu()
{
  /* Always permission for main menu */
  return(ALLOWED);
}

/*************************************************************/
/* security_call_menu: checks permission for call entry menu */
/*************************************************************/
int security_call_menu()
{
	if ( uiuser[user_index]->menu_perms.call_menu ) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/***************************************************************/
/* security_searches_menu: checks permission for searches menu */
/***************************************************************/
int security_searches_menu()
{
	if ( uiuser[user_index]->menu_perms.searches_menu ) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/********************************************************************/
/* security_superv_menu: checks permission for the supervisory menu */
/********************************************************************/
int security_superv_menu()
{
	if ( uiuser[user_index]->menu_perms.supervisory_menu ) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/***************************************************************************/
/* security_update_user: checks permission for update user defaults screen */
/***************************************************************************/
int security_update_user()
{
	if (uiuser[user_index]->menu_perms.update_user_def ) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/***************************************************************************/
/* security_system_maint: checks permission for system maintanance program */
/***************************************************************************/
int security_system_maint()
{
	if ( uiuser[user_index]->menu_perms.system_maint ) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/*******************************************************************/
/* security_user_menu: checks permission for the user defined menu */
/*******************************************************************/
int security_user_menu()
{
	int indx;					/* index into menu structure in shared memory */

	if (uiuser[user_index]->menu_perms.user_def_menu) {		/* user has permission */
		indx = map_array[USER_DEFINED_MENU];			/* check if user has defined a menu or not */
		if (cmenu[indx]->mf_type == UI_MENU) {	/* its a user defined menu */
		    	if (cmenu[indx]->mf_desc[0] == '\0' ){		/* check if user has defined it or not */
				prt_error(UI_NOT_DEFINED,"%d", cmenu[indx]->mf_command);
				return(NOT_ALLOWED);			/* user has not defined the menu */
		     	}
		     	else 
				return(ALLOWED);
		}
		else  {					/* user has not defined this to be a menu, print an error message */
			prt_error(UI_NOT_DEFINED,"%d", cmenu[indx]->mf_command);
			return(NOT_ALLOWED);
		}
	}
	return(NOT_ALLOWED);				/* user does not have permission */
}

/*************************************************************************/
/* security_basic_scr: checks permission for the basic call entry screen */
/*************************************************************************/
int security_basic_scr()
{
	if ( uiuser[user_index]->menu_perms.basic_call) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/***************************************************************************/
/* security_extended_scr: checks permission for extended call entry screen */
/***************************************************************************/
int security_extended_scr()
{
	if ( uiuser[user_index]->menu_perms.extended_call ) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/***************************************************************************/
/* security_wakeup_scr: checks permission for the wakeup call entry screen */
/***************************************************************************/
int security_wakeup_scr()
{
	if (uiuser[user_index]->menu_perms.wakeup_call)  
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/****************************************************************************/
/* security_subs_basic: checks permission for subscription basic call entry */ 
/****************************************************************************/
int security_subs_basic()
{
	if ( uiuser[user_index]->menu_perms.basic_subs) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/******************************************************************************/
/* security_subs_extended: checks permission for subscrip extended call entry */
/******************************************************************************/
int security_subs_extended()
{
	if ( uiuser[user_index]->menu_perms.extended_subs) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/*****************************************************************************/
/* security_subs_wakeup: checks permission for subscription wakeup call entry*/
/*****************************************************************************/
int security_subs_wakeup()
{
	if ( uiuser[user_index]->menu_perms.wakeup_subs) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/*********************************************************************/
/* security_unassigned: checks permission for unassigned call search */
/*********************************************************************/
int security_unassigned()
{
	if ( uiuser[user_index]->menu_perms.unassigned_cls == 1 ) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/***************************************************************************/
/* security_subs_fut: checks permission for subscript & future call search */
/***************************************************************************/
int security_subs_fut()
{
	if ( uiuser[user_index]->menu_perms.subs_fut_cls ) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/*****************************************************************/
/* security_assigned: checks permission for assigned call search */
/*****************************************************************/
int security_assigned()
{
	if ( uiuser[user_index]->menu_perms.assigned_cls ) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/**************************************************************/
/* security_all_calls: checks permission for all calls search */
/**************************************************************/
int security_all_calls()
{
	if ( uiuser[user_index]->menu_perms.all_calls ) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/****************************************************************/
/* security_veh_dr: checks permission for vehicle/driver search */
/****************************************************************/
int security_veh_dr()
{
	if (uiuser[user_index]->menu_perms.veh_dr_cls) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/********************************************************************/
/* security_indiv_cls: checks permission for individual call search */
/********************************************************************/
int security_indiv_cls()
{
	if ( uiuser[user_index]->menu_perms.individual_cls ) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/****************************************************************/
/* security_exceptions: checks permission for exceptions search */
/****************************************************************/
int security_exceptions()
{
	if ( uiuser[user_index]->menu_perms.exceptions ) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/******************************************************************/
/* security_other_srch: checks permission for other searches menu */
/******************************************************************/
int security_other_srch()
{
	if (uiuser[user_index]->menu_perms.other_searches) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/*************************************************************************/
/* security_suspend: checks permission for suspend vehicle driver screen */
/*************************************************************************/
int security_suspend()
{
	if (uiuser[user_index]->menu_perms.suspend_veh_dr)  
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/**************************************************************************/
/* security_zone_status: checks permission for zone status display search */
/**************************************************************************/
int security_zone_status()
{
	if (uiuser[user_index]->menu_perms.zone_status) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/******************************************************************************/
/* security_system_status: checks permission for system status display search */
/******************************************************************************/
int security_system_status()
{
	if (uiuser[user_index]->menu_perms.system_status) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/******************************************************************************/
/* security_edit_repeat_msgs: checks permission for edit repeat messages      */
/******************************************************************************/
int security_edit_repeat_msgs()
{
	if (uiuser[user_index]->menu_perms.edit_repeat_msgs ) 
		return(ALLOWED);
	return (NOT_ALLOWED);
}

/***************************************************************/
/* security_vehicle_srch: checks permission for vehicle search */
/***************************************************************/
int security_vehicle_srch()
{
	if (uiuser[user_index]->menu_perms.vehicle_srch ) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/*************************************************************/
/* security_driver_srch: checks permission for driver search */
/*************************************************************/
int security_driver_srch()
{
	if (uiuser[user_index]->menu_perms.driver_srch ) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/*********************************************************/
/* security_user_srch: checks permission for user search */
/*********************************************************/
int security_user_srch()
{
	if (uiuser[user_index]->menu_perms.user_srch) 
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/***************************************************************************************/
/* security_call_user1: checks permission for 1st user defined item on call entry menu */
/***************************************************************************************/
int security_call_user1()
{
	if (uiuser[user_index]->menu_perms.user_call_def1 == 0)			/* first user defined item on call entry menu */
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[CALL_USER_DEF1]));			/* check if user has defined the exectable */
}

/***************************************************************************************/
/* security_call_user2: checks permission for 2nd user defined item on call entry menu */
/***************************************************************************************/
int security_call_user2()
{
	if (uiuser[user_index]->menu_perms.user_call_def2 == 0) 
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[CALL_USER_DEF2]));			/* check if user has defined the exectable */
}

/***************************************************************************************/
/* security_call_user3: checks permission for 3rd user defined item on call entry menu */
/***************************************************************************************/
int security_call_user3()
{
	if (uiuser[user_index]->menu_perms.user_call_def3 == 0) 
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[CALL_USER_DEF3]));			/* check if user has defined the exectable */
}

/******************************************************************************************/
/* security_srch_user1: checks permission for 1st user defined item on call searches menu */
/******************************************************************************************/
int security_srch_userdef1()
{
	if (uiuser[user_index]->menu_perms.user_srch_usr_def)
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/**********************************************************************************************/
/* security_superv_def1: checks permission for 1st user defined item on call supervisory menu */
/**********************************************************************************************/
int security_superv_def1()
{
	if (uiuser[user_index]->menu_perms.superv_user_def1 == 0) 
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[SUPERV_USER_DEF1]));		/* check if user has defined the exectable */
}

/**********************************************************************************************/
/* security_shift_sched:
/**********************************************************************************************/
int security_shift_sched()
{
	if (uiuser[user_index]->menu_perms.shift_sched)
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/**********************************************************************************************/
/* security_superv_def3: checks permission for 3rd user defined item on call supervisory menu */
/**********************************************************************************************/
int security_superv_def3()
{
	if (uiuser[user_index]->menu_perms.superv_user_def3 == 0) 
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[SUPERV_USER_DEF3]));		/* check if user has defined the exectable */
}

/**********************************************************************************************/
/* security_superv_def4: checks permission for 4th user defined item on call supervisory menu */
/**********************************************************************************************/
int security_superv_def4()
{
	if (uiuser[user_index]->menu_perms.superv_user_def4 == 0) 
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[SUPERV_USER_DEF4]));		/* check if user has defined the exectable */
}

/**********************************************************************************************/
/* security_superv_def5: checks permission for 5th user defined item on call supervisory menu */
/**********************************************************************************************/
int security_superv_def5()
{
	if (uiuser[user_index]->menu_perms.superv_user_def5 == 0) 
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[SUPERV_USER_DEF5]));		/* check if user has defined the exectable */
}

/******************************************************************************/
/* security_um_def1: checks permission for 1st user defined item on user menu */
/******************************************************************************/
int security_um_def1()
{
	if (uiuser[user_index]->menu_perms.user_def_1 == 0) 
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[UM_USER_DEF_1]));			/* check if user has defined the exectable */
}

/******************************************************************************/
/* security_um_def2: checks permission for 2nd user defined item on user menu */
/******************************************************************************/
int security_um_def2()
{
	if (uiuser[user_index]->menu_perms.user_def_2 == 0)
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[UM_USER_DEF_2]));		/* check if user has defined the exectable */

}

/******************************************************************************/
/* security_um_def3: checks permission for 3rd user defined item on user menu */
/******************************************************************************/
int security_um_def3()
{
	if (uiuser[user_index]->menu_perms.user_def_3 ==0) 
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[UM_USER_DEF_3]));		/* check if user has defined the exectable */
}

/******************************************************************************/
/* security_um_def4: checks permission for 4th user defined item on user menu */
/******************************************************************************/
int security_um_def4()
{
	if (uiuser[user_index]->menu_perms.user_def_4 ==0) 
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[UM_USER_DEF_4]));		/* check if user has defined the exectable */
}

/******************************************************************************/
/* security_um_def5: checks permission for 5th user defined item on user menu */
/******************************************************************************/
int security_um_def5()
{
	if (uiuser[user_index]->menu_perms.user_def_5 ==0) 
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[UM_USER_DEF_5]));		/* check if user has defined the exectable */
}

/******************************************************************************/
/* security_um_def6: checks permission for 6th user defined item on user menu */
/******************************************************************************/
int security_um_def6()
{
	if (uiuser[user_index]->menu_perms.user_def_6 == 0)
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[UM_USER_DEF_6]));		/* check if user has defined the exectable */

}

/******************************************************************************/
/* security_um_def7: checks permission for 7th user defined item on user menu */
/******************************************************************************/
int security_um_def7()
{
	if (uiuser[user_index]->menu_perms.user_def_7 == 0 ) 
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[UM_USER_DEF_7]));		/* check if user has defined the exectable */
}

/******************************************************************************/
/* security_um_def8: checks permission for 8th user defined item on user menu */
/******************************************************************************/
int security_um_def8()
{
	if (uiuser[user_index]->menu_perms.user_def_8 == 0) 
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[UM_USER_DEF_8]));		/* check if user has defined the exectable */
}

/******************************************************************************/
/* security_um_def9: checks permission for 9th user defined item on user menu */
/******************************************************************************/
int security_um_def9()
{
	if (uiuser[user_index]->menu_perms.user_def_9 == 0) 
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[UM_USER_DEF_9]));		/* check if user has defined the exectable */
}

/********************************************************************************************/
/* security_os_userdef1: checks permission for 1st user defined item on other searches menu */
/********************************************************************************************/
int security_os_userdef1()
{
	if (uiuser[user_index]->menu_perms.srch_user_def1 == 0)
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[OS_USER_DEF2]));		/* check if user has defined the exectable */
}

/********************************************************************************************/
/* security_os_userdef2: checks permission for 2nd user defined item on other searches menu */
/********************************************************************************************/
int security_os_userdef2()
{
	if (uiuser[user_index]->menu_perms.srch_user_def2 == 0) 
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[OS_USER_DEF2]));		/* check if user has defined the exectable */
}

/********************************************************************************************/
/* security_os_userdef3: checks permission for 3rd user defined item on other searches menu */
/********************************************************************************************/
int security_os_userdef3()
{
	if (uiuser[user_index]->menu_perms.srch_user_def3 == 0) 
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[OS_USER_DEF3]));		/* check if user has defined the exectable */
}

/********************************************************************************************/
/* security_os_userdef4: checks permission for 4th user defined item on other searches menu */
/********************************************************************************************/
int security_os_userdef4()
{
	if (uiuser[user_index]->menu_perms.srch_user_def4 == 0) 
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[OS_USER_DEF4]));		/* check if user has defined the exectable */
}

/********************************************************************************************/
/* security_os_userdef5: checks permission for 5th user defined item on other searches menu */
/********************************************************************************************/
int security_os_userdef5()
{
	if (uiuser[user_index]->menu_perms.srch_user_def5 == 0) 
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[OS_USER_DEF5]));		/* check if user has defined the exectable */
}

/********************************************************************************************/
/* security_os_userdef6: checks permission for 6th user defined item on other searches menu */
/********************************************************************************************/
int security_os_userdef6()
{
	if (uiuser[user_index]->menu_perms.srch_user_def6 == 0) 
		return(NOT_ALLOWED);
	return(sec_defined_byuser(map_array[OS_USER_DEF6]));		/* check if user has defined the exectable */
}

/*********************************************************************************************/
/* sec_defined_byuser: check if the user has defined the exeternal executable program or not */
/*********************************************************************************************/
int sec_defined_byuser(indexm)
int indexm;				/* index into menus structure in shared memory */
{
	if (cmenu[indexm]->mf_type != UI_EXTERNAL) {	/* not defined as an external executable */
		prt_error(UI_NOT_DEFINED,"%d", cmenu[indexm]->mf_command);
		return(NOT_ALLOWED);
	}
	if (cmenu[indexm]->mf_unix_string[0] == '\0') {				/* user has not defined the executable program */
		prt_error(UI_NOT_DEFINED,"%d", cmenu[indexm]->mf_command);
		return(NOT_ALLOWED);
	}
	return(ALLOWED);
}

/****************************************************************************************/
/* fleet_perms: routines to check users permission of working with various fleets	*/
/*		Return:									*/
/*			ALLOWED - if user is permitted to work with a fleet		*/
/*			NOT_ALLOWEDa - if not allowed					*/
/****************************************************************************************/

/***********************************************************************/
/* ui_fleet_perms0 - verify that user has permission to access fleet 0 */
/***********************************************************************/
int ui_fleet_perms0()
{
	if (uiuser[user_index]->orig_fleet_perms.fleet0 )  
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/***********************************************************************/
/* ui_fleet_perms1 - verify that user has permission to access fleet 1 */
/***********************************************************************/
int ui_fleet_perms1()
{
	if (uiuser[user_index]->orig_fleet_perms.fleet1)  
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/***********************************************************************/
/* ui_fleet_perms2 - verify that user has permission to access fleet 2 */
/***********************************************************************/
int ui_fleet_perms2()
{
	if (uiuser[user_index]->orig_fleet_perms.fleet2)  
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/***********************************************************************/
/* ui_fleet_perms3 - verify that user has permission to access fleet 3 */
/***********************************************************************/
int ui_fleet_perms3()
{
	if (uiuser[user_index]->orig_fleet_perms.fleet3)  
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/***********************************************************************/
/* ui_fleet_perms4 - verify that user has permission to access fleet 4 */
/***********************************************************************/
int ui_fleet_perms4()
{
	if (uiuser[user_index]->orig_fleet_perms.fleet4)  
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/***********************************************************************/
/* ui_fleet_perms5 - verify that user has permission to access fleet 5 */
/***********************************************************************/
int ui_fleet_perms5()
{
	if (uiuser[user_index]->orig_fleet_perms.fleet5)  
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/***********************************************************************/
/* ui_fleet_perms6 - verify that user has permission to access fleet 6 */
/***********************************************************************/
int ui_fleet_perms6()
{
	if (uiuser[user_index]->orig_fleet_perms.fleet6)  
		return(ALLOWED);
	return(NOT_ALLOWED);
}

/***********************************************************************/
/* ui_fleet_perms7 - verify that user has permission to access fleet 7 */
/***********************************************************************/
int ui_fleet_perms7()
{
	if (uiuser[user_index]->orig_fleet_perms.fleet7)  
		return(ALLOWED);
	return(NOT_ALLOWED);
}
