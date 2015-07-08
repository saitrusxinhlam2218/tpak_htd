static char sz__FILE__[]="@(#)sp_scr.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  sp_scr.c; Rel: 6.5.0.0; Get date: 1/22/93 at 10:45:54
 * @(#)  Last delta: 1/11/93 at 10:42:07
 * @(#)  SCCS File: /taxi/sccs/s.sp_scr.c
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include <time.h>
#include "screens.h"
#include <memory.h>
#include "sp_ary.h"
#include "ui_def.h"
#include "sp_scr_fld.h"
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "ui_ipc.h"
#include "user.h"
#include "ui_error.h"
#include "df_maint.h"
#include "language.h"
#include "ui_strdefs.h"

						/* declare functions used for checking fields entered */
extern int check_dummy1(), check_fleet1(), check_veh(),check_drv(),check_susp_clear(),check_susp_d_clear(),check_susp_x_clear(),
check_susp_from_date(),check_susp_from_time(),check_susp_to_date(),check_susp_to_time(),check_pass(), 
check_susp_d_from_date(),check_susp_d_from_time(),check_susp_d_to_date(),check_susp_d_to_time(), 
check_susp_x_from_date(),check_susp_x_from_time(),check_susp_x_to_date(),check_susp_x_to_time(),
check_del(),check_typex(),check_reason(),check_msg1(),check_msg2(),check_msg3();

						/* declare an array of pointers to functions which check the field entered */
						/* the functions check to see if the field entered is valid or not */
int (*sp_check_field[])() = {check_dummy1, check_fleet1, check_veh, check_drv,
			     check_susp_clear,check_susp_d_clear,check_susp_x_clear,
			     check_pass,
			     check_susp_from_date, check_susp_from_time, check_susp_to_date, check_susp_to_time,
			     check_del,
			     check_susp_d_from_date, check_susp_d_from_time, check_susp_d_to_date, check_susp_d_to_time,
			     check_typex,
			     check_susp_x_from_date, check_susp_x_from_time, check_susp_x_to_date, check_susp_x_to_time,
			     check_reason, check_msg1,
			     check_msg2, check_msg3};

extern int scr_name;
struct cisam_vh *vh_ptr;			/* for saving record of prev scr */
struct cisam_dr *dr_ptr;			/* for saving record of prev scr */
extern struct cisam_cl prev_call_struct;		/* for saving record of prev scr */
extern struct call_ipc_rec call_fifo;			/* for writing record to FIFO */
extern struct call_ipc_rec prev_call_fifo;			/* for saving call_fifo struct of prev scr */
int from_flag,to_flag,dr_flag,vh_flag,clear_susp_flag,clear_susp_d_flag,clear_susp_x_flag; 
int valid_flag = 0;
int i,nbr_day;
char sp_fleet,susp_clear_flag,susp_d_clear_flag,susp_x_clear_flag,susp_pass_call,susp_del_call,susp_typex_call;
char susp_reason[21],message[97],message1[33],message2[33],message3[33];
int susp_from_dt_tm, susp_to_dt_tm;
int susp_d_from_dt_tm, susp_d_to_dt_tm;
int susp_x_from_dt_tm, susp_x_to_dt_tm;
char p_from_date[12], p_to_date[12];
char p_from_time[10], p_to_time[10];
char d_from_date[12], d_to_date[12];
char d_from_time[10], d_to_time[10];
char x_from_date[12], x_to_date[12];
char x_from_time[10], x_to_time[10];
extern char req_buf[];
extern struct fleets *fleet[];
extern int def_fl_index;
extern struct users *uiuser[];			/* array of pointers to users structure in shared memory */
extern int user_index;				/* index into users struture in shared memory */
int end_suspend = 0;				/* flag for ending loop of sp_call() */
extern short clear_err_flag;			/* set if last line has an error msg printed */
extern struct keydesc dr_key1, veh_key2;  	/* for db() calls */

extern time_t timelocal();			/* converts tm struct to Unix time (long) */

/****************************************************************************************/
/* veh_drv_suspend: 	This function displays the suspend driver/vehicle  		*/
/*		 	screen. It accepts input from the user and calls 		*/
/*			the appropriate functions to process the call entered.  	*/
/****************************************************************************************/
veh_drv_suspend()
{
	int piu = 2;					/* prompt in use */
	int field_entered = 0;				/* whether field entered or not */
	int key_val;					/* value of the key pressed by the user */
	char read_buf[READ_BUF_SIZE]; 			/* buffer for reading fields on screen */
	int ret_ck_fld;					/* return value of check field routine */
	int should_clear = 0;

	disp_sp_scr();
	sp_print_values();
	attrset(0);
	end_suspend = 0;
	while((scr_name == SUSPEND_VEH_DR_SCR) && (!end_suspend)) {

	        if ( piu == F_SUSP_PASS_FROM_DATE ||
		     piu == F_SUSP_PASS_FROM_TIME ||
		     piu == F_SUSP_PASS_TO_DATE ||
		     piu == F_SUSP_PASS_TO_TIME ||
		     piu == F_SUSP_DEL_FROM_DATE ||
		     piu == F_SUSP_DEL_FROM_TIME ||
		     piu == F_SUSP_DEL_TO_DATE ||
		     piu == F_SUSP_DEL_TO_TIME ||
		     piu == F_SUSP_X_FROM_DATE ||
		     piu == F_SUSP_X_FROM_TIME ||
		     piu == F_SUSP_X_TO_DATE ||
		     piu == F_SUSP_X_TO_TIME )
		  should_clear = 1;
		    
	      	key_val = read_field(stdscr,sp_scr_flds,piu,read_buf, &field_entered,&should_clear); 

		if (key_val == -1) 				/* read_field() timed out */
			continue;

		if (clear_err_flag) 
			clear_error();
								/* Verify that the field entered is correct */
	     	if ( (ret_ck_fld = (*sp_check_field[piu])(&piu, read_buf,key_val, &field_entered)) == DONE )  {
			if ( piu  > MAX_PIU_SUSPEND )
				piu = 1; 			/* wrap around */
			else if ( piu == 0 )
				piu = MAX_PIU_SUSPEND;			/* wrap around to last field */
		}
		else 
			ftnkey_handler(&piu,key_val,0);
			
	}
}

/******************************************************************************/
sp_print_values()
{
struct tm tmbuf;
struct tm *tmptr;

	vh_ptr = (struct cisam_vh *)req_buf;
	dr_ptr = (struct cisam_dr *)req_buf;

	memset((char *)vh_ptr,'\0',sizeof(struct cisam_vh));
	memset((char *)dr_ptr,'\0',sizeof(struct cisam_dr));

	from_flag = 1;
	to_flag = 1;
	dr_flag = 0;
	vh_flag = 0;
	clear_susp_flag = 0;
	clear_susp_d_flag = 0;
	clear_susp_x_flag = 0;
	valid_flag = 0;
	vh_ptr->nbr = 0;
	dr_ptr->id = 0;
	nbr_day = 0;
	memset(susp_reason,'\0',21);
	memset(message,'\0',97);
	memset(message1,'\0',33);
	memset(message2,'\0',33);
	memset(message3,'\0',33);
	get_pkup_date(p_from_date);			/* default from date to NOW */
	get_pkup_time(p_from_time);			/* default from time to NOW */
	get_pkup_date(d_from_date);			/* default from date to NOW */
	get_pkup_time(d_from_time);			/* default from time to NOW */
	get_pkup_date(x_from_date);			/* default from date to NOW */
	get_pkup_time(x_from_time);			/* default from time to NOW */
	if (DATE_FORMAT == DAY_FIRST_IN_DATE){
	    sprintf(p_to_date, "31%c12%c29", DATE_SEPERATOR, DATE_SEPERATOR);
	    sprintf(d_to_date, "31%c12%c29", DATE_SEPERATOR, DATE_SEPERATOR);
	    sprintf(x_to_date, "31%c12%c29", DATE_SEPERATOR, DATE_SEPERATOR);
	}
	else if (DATE_FORMAT == YEAR_FIRST_IN_DATE){
	    sprintf(p_to_date, "29%c12%c31", DATE_SEPERATOR, DATE_SEPERATOR);
	    sprintf(d_to_date, "29%c12%c31", DATE_SEPERATOR, DATE_SEPERATOR);
	    sprintf(x_to_date, "29%c12%c31", DATE_SEPERATOR, DATE_SEPERATOR);
	}
	else{
	    sprintf(p_to_date, "12%c31%c29", DATE_SEPERATOR, DATE_SEPERATOR);
	    sprintf(d_to_date, "12%c31%c29", DATE_SEPERATOR, DATE_SEPERATOR);
	    sprintf(x_to_date, "12%c31%c29", DATE_SEPERATOR, DATE_SEPERATOR);
	}

#ifdef HR12	
	strcpy(p_to_time, "11:59p");			/* default to time to 1 minute before midnight) */
	strcpy(d_to_time, "11:59p");			/* default to time to 1 minute before midnight) */
	strcpy(x_to_time, "11:59p");			/* default to time to 1 minute before midnight) */
#else
	strcpy(p_to_time, "23:59");			/* default to time to 1 minute before midnight) */
	strcpy(d_to_time, "23:59");			/* default to time to 1 minute before midnight) */
	strcpy(x_to_time, "23:59");			/* default to time to 1 minute before midnight) */
#endif

        /* set and display default to/from date/times to "now" and "forever" */

        /* convert "now" and "forever" to Unix seconds */
	susp_from_dt_tm = time(NULL);
	susp_d_from_dt_tm = time(NULL);
	susp_x_from_dt_tm = time(NULL);
        tmptr = &tmbuf;
        tmptr->tm_sec = 0;
        tmptr->tm_min = 59;
        tmptr->tm_hour = 23;
        tmptr->tm_mday = 31;
        tmptr->tm_mon = 12;
        tmptr->tm_year = 129;
        susp_to_dt_tm = get_internal_time(p_to_date, p_to_time);
	susp_d_to_dt_tm = get_internal_time(d_to_date, d_to_time);
	susp_x_to_dt_tm = get_internal_time(x_to_date, x_to_time);
/*	susp_to_dt_tm = timelocal(tmptr);
	susp_d_to_dt_tm = timelocal(tmptr);
	susp_x_to_dt_tm = timelocal(tmptr); */

	prt_entry_areas(stdscr,sp_scr_flds,MAX_PIU_SUSPEND);
	attrset(A_UNDERLINE);

	mvprintw(D_PASS_ROW,D_SUSP_FROM_DATE_COL,"%s",p_from_date);
	mvprintw(D_PASS_ROW,D_SUSP_FROM_TIME_COL,"%s",p_from_time);
	mvprintw(D_PASS_ROW,D_SUSP_TO_DATE_COL,"%s",p_to_date);
	mvprintw(D_PASS_ROW,D_SUSP_TO_TIME_COL,"%s",p_to_time);

	mvprintw(D_DEL_ROW,D_SUSP_FROM_DATE_COL,"%s",p_from_date);
	mvprintw(D_DEL_ROW,D_SUSP_FROM_TIME_COL,"%s",p_from_time);
	mvprintw(D_DEL_ROW,D_SUSP_TO_DATE_COL,"%s",p_to_date);
	mvprintw(D_DEL_ROW,D_SUSP_TO_TIME_COL,"%s",p_to_time);

	mvprintw(D_TYPEX_ROW,D_SUSP_FROM_DATE_COL,"%s",p_from_date);
	mvprintw(D_TYPEX_ROW,D_SUSP_FROM_TIME_COL,"%s",p_from_time);
	mvprintw(D_TYPEX_ROW,D_SUSP_TO_DATE_COL,"%s",p_to_date);
	mvprintw(D_TYPEX_ROW,D_SUSP_TO_TIME_COL,"%s",p_to_time);

	susp_clear_flag = NO;
	susp_d_clear_flag = NO;
	susp_x_clear_flag = NO;
	mvprintw(D_VEH_ROW,D_CLEAR_SUSP_COL,"%c",susp_clear_flag);
	mvprintw(D_VEH_ROW,D_CLEAR_SUSP_COL+5,"%c",susp_d_clear_flag);
	mvprintw(D_VEH_ROW,D_CLEAR_SUSP_COL+10,"%c",susp_x_clear_flag);
	susp_pass_call = YES;
	mvprintw(D_PASS_ROW,D_PASS_COL,"%c",susp_pass_call);
	susp_del_call = YES;
	mvprintw(D_DEL_ROW,D_DEL_COL,"%c",susp_del_call);
	susp_typex_call = YES;
	mvprintw(D_TYPEX_ROW,D_TYPEX_COL,"%c",susp_typex_call);
	if (def_fl_index == -1)
		mvprintw(D_FLEET_ROW,D_FLEET_COL,"%c",' ');
	else	{
		valid_flag = 1;
		sp_fleet = fleet[def_fl_index]->fleet_id;
		mvprintw(D_FLEET_ROW,D_FLEET_COL,"%c",sp_fleet);
	}
}
