 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  user.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:50:51
 * @(#)  Last delta: 12/2/94 at 18:29:17
 * @(#)  SCCS File: /taxi/sccs/s.user.h
 *                                                                           *
 *****************************************************************************/
#define EXC_TYPES	32		/* Number of exception types defined */
#define EXC_GROUPS	12		/* Number of exception groups defined */
#define USER_MAX	128		/* Number of ports on the machine */
#define MAX_EXC_ZONES	16		/* Max number of (super)zones selected */
					/* The structure for the count increments is defined as: */
#define	CNT_MESG	'M'		/* 'M' = modify message count. */
#define	CNT_EXCEPT	'E'		/* 'E' = modify exception count. */
#define	CNT_CLOSE	'C'		/* 'C' = clear counts and 25th line. */
#define	CNT_OPEN	'O'		/* 'O' = set counts and write 25th line. */
struct inc_cnt {
        char	inc_type;		/* see above CNT_ defines */
        short	amount;			/* amount to modify by (- or +) */
        short	user_id;		/* User id to modify count for */
        short	fleet;			/* Fleet of exception */
        short	zone;			/* Zone of exception */
        short	super_zone;		/* Super zone of exception */
        short	exc_type;		/* Exception type (corresponds to #define in DISPATCH) */
	short	term_num;		/* Terminal being signed on/off */
};
					/* fleet permissions */
struct up_fleet_perms {
	unsigned fleet0:1;
	unsigned fleet1:1;
	unsigned fleet2:1;
	unsigned fleet3:1;
	unsigned fleet4:1;
	unsigned fleet5:1;
	unsigned fleet6:1;
	unsigned fleet7:1;
};
 						/* user menu permissions, corresponds to flags on disk, 1=allowed, 0=not allowed */
struct up_menu_perms {
	unsigned int main_menu:1;		/* permission for MAIN MENU */
	unsigned int call_menu:1;		/* permission for CALL_ENTRY_MENU */
	unsigned int searches_menu:1;		/* permission for SEARCHES_MENU */
	unsigned int supervisory_menu:1;	/* permission for SUPERVISORY_MENU */
	unsigned int update_user_def:1;		/* permission for UPDATE_USER_DEFAULTS  routine */
	unsigned int system_maint:1;		/* permission for SYSTEM_MAINTANANCE */
	unsigned int user_def_menu:1;		/* permission for the USER_DEFINED_MENU*/
	unsigned int basic_call:1;		/* permission for BASIC_CALL_ENTRY */
	unsigned int extended_call:1;		/* permission for EXTENDED_CALL_ENTRY */
	unsigned int wakeup_call:1;		/* permission for WAKEUP_CALL_ENTRY */
	unsigned int basic_subs:1;		/* permission for BASIC_SUBSCRIPTION CALL */
	unsigned int extended_subs:1;		/* permission for EXTENDED_SUBSCRIPTION CALL */
	unsigned int wakeup_subs:1;		/* permission for WAKEUP_SUBSCRIPTION CALL*/
	unsigned int user_call_def1:1;		/* permission for first user defined item on call entry menu */
	unsigned int user_call_def2:1;		/* permission for 2nd user defined item  on call entry menu */
	unsigned int user_call_def3:1;		/* permission for 3rd user defined item on call entry menu */
	unsigned int unassigned_cls:1;		/* permission for UNASSIGNED_CALLS */
	unsigned int subs_fut_cls:1;		/* permission for SUBSCRIPTION & FUTURE  CALLS */
	unsigned int assigned_cls:1;		/* permission for ASSIGNED_CALLS */
	unsigned int all_calls:1;		/* permission for ALL_CALLS */
	unsigned int veh_dr_cls:1;		/* permission for VEHICLE_DRIVER CALLS */
	unsigned int individual_cls:1;		/* permission for INDIVIDUAL_CALLS */
	unsigned int exceptions:1;		/* permission for EXCEPTIONS SCREEN */
	unsigned int other_searches:1;		/* permission for OTHER_SEARCHES MENU */
	unsigned int user_srch_usr_def:1;	/* permission for user defined item on search menu */
	unsigned int vehicle_srch:1;		/* permission for VEHICLE_SEARCH */
	unsigned int driver_srch:1;		/* permission for DRIVER_SEARCH */
	unsigned int user_srch:1;		/* permission for USER_SEARCH */
	unsigned int srch_user_def1:1;		/* permission for 1st user defined  item on other searches menu*/
	unsigned int srch_user_def2:1;		/* permission for 2nd user defined  item on "other searches menu" */ 
	unsigned int srch_user_def3:1;		/* permission for 3rd user defined  item on "other searches menu" */
	unsigned int srch_user_def4:1;		/* permission for 4th user defined  item on "other searches menu" */
	unsigned int srch_user_def5:1;		/* permission for 5th user defined  item on "other searches menu" */
	unsigned int srch_user_def6:1;		/* permission for 6th user defined  item on "other searches menu" */
	unsigned int suspend_veh_dr:1;		/* permission for SUSPEND_VEHICLE  DRIVER screen */
	unsigned int zone_status:1;		/* permission for ZONE_STATUS screen */
	unsigned int system_status:1;		/* permission for SYSTEM_STATUS screen */
	unsigned int edit_repeat_msgs:1;	/* permission for edit repeat messages */
	unsigned int superv_user_def1:1;	/* permission for 1st user defined item on supervisor menu */
	unsigned int shift_sched:1;	
	unsigned int superv_user_def3:1;	/* permission for 3rd user defined item  on supervisory menu */
	unsigned int superv_user_def4:1;	/* permission for 4th user defined item  on supervisory menu */
	unsigned int superv_user_def5:1;	/* permission for 5th  user defined item  on supervisory menu */
	unsigned int user_def_1:1;		/* permission for 1st user defined item  on user defined menu */
	unsigned int user_def_2:1;		/* permission for 2nd user defined item on  user defined menu */
	unsigned int user_def_3:1;		/* permission for 3rd user defined item on  user defined menu */
	unsigned int user_def_4:1;		/* permission for 4th user defined item on  user defined menu */
	unsigned int user_def_5:1;		/* permission for 5th user defined item on  user defined menu */
	unsigned int user_def_6:1;		/* permission for 6th user defined item on  user defined menu */
	unsigned int user_def_7:1;		/* permission for 7th user defined item on  user defined menu */
	unsigned int user_def_8:1;		/* permission for 8th user defined item on  user defined menu */
	unsigned int user_def_9:1;		/* permission for 9th user defined item on  user defined menu */
	unsigned int not_used_1:1;		/* not used ... placed to complete 7 bytes */
	unsigned int not_used_2:1;		/* not used ... placed to complete 7 bytes */
	unsigned int not_used_3:1;		/* not used ... placed to complete 7 bytes */
	unsigned int not_used_4:1;		/* not used ... placed to complete 7 bytes */
};
struct users {
	short	user_id;			/* User ID.  Displayed on all screens and used for transaction logging. */
	char	greeting_name[13];		/* User name to be displayed on the greeting line, if used. */
	short	user_pos;			/* position of user in list */
	short	exc_grp[EXC_TYPES];		/* one for each exception type */
	struct	up_menu_perms menu_perms;	/* A flag for each menu item; 1=allowed to access, 0=not allowed to access */
	struct	up_fleet_perms fleet_perms;	/* Flag for each fleet this user may work on - possibly was changed on CMD 4 */
	struct	up_fleet_perms orig_fleet_perms;/* Flag for each fleet this user may work on - permanent sel from profile file. */
	short	default_screen;			/* Default screen for this user in navigation format. */
	short	default_search;			/* Default search for this user in navigation format. */
	short	default_fleet;			/* Default fleet id. */
	char	default_call_type;		/* Default call type. */
	char	default_city[4];		/* Default city code. */
	char	can_zone_calls;			/* "Y"/"N".  Can this user zone calls.*/
	short	highest_call_pri;		/* 1-63.  The highest priority this user may assign to a call. */
	short	zones_sel[MAX_EXC_ZONES];	/* max of 16 selected super(zones) */
	short	cnt_msg;			/* count of unread messages */
	short	cnt_exc[EXC_GROUPS];		/* count of exceptions in groups */
	short	exc_grp_slct[EXC_TYPES];	/* selected types contain the groups they belong to */
	char	img_25[133];			/* shared memory image of 25th line */
	long	last_25_time;			/* last time 25th line was updated */
	int	p_id;				/* process id of USER INTERFACE proc */
	char    auto_free_text;
#ifdef NAMERICA
	int     channels[18];                   /* radio channels in  */
#endif
}; 
