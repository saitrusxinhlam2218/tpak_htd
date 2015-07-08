static char sz__FILE__[]="@(#)sp_ck_fld.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  sp_ck_fld.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:49:39
* @(#)  Last delta: 12/2/94 at 18:28:09
* @(#)  SCCS File: /taxi/sccs/s.sp_ck_fld.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

#include <cursesX.h>
#include <time.h>
#include <sys/types.h>
#include <ctype.h>

#include "ui_def.h"
#include "ui_error.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "ui_ipc.h"
#include "sp_ary.h"
#include "screens.h"
#include "user.h"
#include "df_maint.h"
#include "func_keys.h"
#include "mad_ipc.h"
#include "language.h"
#include "ui_strdefs.h"

extern struct cisam_vh *vh_ptr;				/* for disk i/o */
extern struct cisam_dr *dr_ptr;				/* for disk i/o */
extern int msgkey;					/* IPC message key */
extern int pid;						/* ui process id */
extern int scr_name;					/* name of screen on display */
extern int cur_fl_index;				/* index of the fleet the user is currently working with */
extern int valid_flag;
extern int dr_flag;
extern int vh_flag;
extern int clear_susp_flag;
extern int clear_susp_d_flag;
extern int clear_susp_x_flag;
extern	struct scr_flds sp_scr_flds[];
extern int prt_entry_areas();
extern get_time();
extern get_asc_time();
extern int find_findex();
extern int from_flag;
extern int to_flag;
extern int user_index;					/* index into user's struct in shared mem */
extern struct fleets *fleet[];				/* array of pointers to fleets struct */
extern struct users *uiuser[];				/* array of pointers to users struct */
extern struct veh_drv_attrs veh_drv_attr[];		/* struct's for veh/drv attrs*/
extern char req_buf[];
extern short inters_flag;				/* flag set if user wants to enter inters address */
extern struct keydesc dr_key1, veh_key2;
extern int def_fl_index;				/* index for the users default fleet */
extern int	nbr_day;
extern int susp_from_dt_tm,susp_to_dt_tm;
extern int susp_d_from_dt_tm,susp_d_to_dt_tm;
extern int susp_x_from_dt_tm,susp_x_to_dt_tm;
extern char p_from_date[12],p_to_date[12];
extern char p_from_time[10],p_to_time[10];
extern char d_from_date[12],d_to_date[12];
extern char d_from_time[10],d_to_time[10];
extern char x_from_date[12],x_to_date[12];
extern char x_from_time[10],x_to_time[10];
int send_dr_ipc(), send_vh_ipc();
extern char sp_fleet,susp_clear_flag,susp_d_clear_flag,susp_x_clear_flag,susp_pass_call,susp_del_call,susp_typex_call;
extern char	susp_reason[],message[],message1[], message2[],message3[];

							/* declare functions for checking fleet permissions */
 extern int ui_fleet_perms0(), ui_fleet_perms1(), ui_fleet_perms2(), ui_fleet_perms3(), ui_fleet_perms4(), ui_fleet_perms5(), 
			ui_fleet_perms6(), ui_fleet_perms7(); 
extern int (*ui_fleet_perms[])(); 

/**********************************************************************************/
/* updt_piu: updates the value of piu, for the next field which is 		  */
/*	     is to be read, called by field checking routines 			  */
/**********************************************************************************/
int updt_piu(key_val,piu_ptr) 
int key_val; 
int *piu_ptr;
{ 
switch(key_val) { 
		case OK:					/* max char's were entered by user */ 
		case '\r': 
		case '\n': 
		case '\t':
	        case KEY_DOWN:
			*piu_ptr += 1; 				/* move to next field */
			return(DONE);

		case KEY_UP:
			if ((*piu_ptr == 3) && (dr_flag))
				*piu_ptr = 1;
			else
				*piu_ptr -= 1;			/* move up 1 field */
			return(DONE);

		case SEND_KEY:
			send_to_disk(); 
			*piu_ptr = 2;
			return(DONE);

		default:					/* some other function key entered */
			return(NOT_DONE);
	}
}

/***************************************************************************************/
int check_dummy1(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
  return(DONE);
}

/***************************************************************************************/
int check_fleet1(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
int	old_piu,ret_code;
int    in_fleet;
	if ( *field_entered ) {
		if ((in_fleet = find_findex(read_buf[0])) == -1) {
			prt_error(UI_NOFIELD," ");
			valid_flag = 0;
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		 }
		if ((*ui_fleet_perms[in_fleet])() == NOT_ALLOWED) {
			prt_error(UI_NOT_ALLOWED," ");
			valid_flag = 0;
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		if (fleet[in_fleet]->active != YES) {
			prt_error(UI_NOT_ACTIVE," ");
			valid_flag = 0;
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
			sp_fleet = read_buf[0];
			attrset(A_UNDERLINE);
			mvprintw(D_FLEET_ROW,D_FLEET_COL,"%c",sp_fleet);
			attrset(0);
			valid_flag = 1;

		if ((dr_flag) && (key_val != KEY_UP))
			++*piu_ptr;

		return(updt_piu(key_val,piu_ptr));	
	}
	else {
		if (!valid_flag) {
			prt_error(UI_NOFIELD," ");
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		if ((dr_flag) && (key_val != KEY_UP))
			++*piu_ptr;

		return(updt_piu(key_val,piu_ptr));	
	}
}

/**********************************************************************************/
int check_veh(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int	i,number,old_piu,ret_code;
	if (*field_entered) {
			vh_ptr->fleet = sp_fleet; 
			if (!numeric(read_buf))	{
				vh_flag = 0;
				prt_error(UI_NOFIELD," ");
				old_piu = *piu_ptr;
				ret_code = updt_piu(key_val,piu_ptr);	
				*piu_ptr = old_piu;
				return(ret_code);
			}

			number = atoi(read_buf);
			vh_ptr->nbr = (short)number;
			if ((db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISEQUAL,0)) < 0) {
				vh_flag = 0;
				prt_error(UI_INVALID_NUM,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_1, "<for vehicle>"));
				old_piu = *piu_ptr;
				ret_code = updt_piu(key_val,piu_ptr);	
				*piu_ptr = old_piu;
				return(ret_code);
			}
			vh_flag = 1;
			attrset(A_UNDERLINE);
			clear_fld(D_VEH_ROW,D_VEH_COL,CHARS_VEH);
			clear_fld(D_VEH_ROW,D_DRV_COL,CHARS_DRV);
			mvprintw(D_VEH_ROW,D_VEH_COL,"%4d",vh_ptr->nbr);
			attrset(0);
			++*piu_ptr;
			
			return(updt_piu(key_val,piu_ptr));	
	}
	else {
		if ((vh_flag) && (key_val != KEY_UP))
			++*piu_ptr;
		attrset(0);
		return(updt_piu(key_val,piu_ptr));	
	}
} 
		
/*************************************************************************************/
int check_drv(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
int	i,id_number,old_piu,ret_code;
	if (*field_entered) {
			dr_ptr->fleet = sp_fleet;
			if (!numeric(read_buf))	{
				dr_flag = 0;
				prt_error(UI_NOFIELD," ");
				old_piu = *piu_ptr;
				ret_code = updt_piu(key_val,piu_ptr);	
				*piu_ptr = old_piu;
				return(ret_code);
			}
			id_number = atoi(read_buf);
			dr_ptr->id = (int)id_number;
			if ((db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISEQUAL,0)) < 0) {
				dr_flag = 0;
				prt_error(UI_INVALID_NUM,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_2, "<for driver id>"));
				dr_ptr->id = 0;
				old_piu = *piu_ptr;
				ret_code = updt_piu(key_val,piu_ptr);	
				*piu_ptr = old_piu;
				return(ret_code);
			}
			dr_flag = 1;
			attrset(A_UNDERLINE);
			clear_fld(D_VEH_ROW,D_VEH_COL,CHARS_VEH);
			clear_fld(D_VEH_ROW,D_DRV_COL,CHARS_DRV);
			mvprintw(D_VEH_ROW,D_DRV_COL,"%4d",dr_ptr->id);
			attrset(0);
			return(updt_piu(key_val,piu_ptr));	
	}

	else  	{
		if ((!vh_flag) &&(!dr_flag)&& (key_val != KEY_UP)) {
			prt_error(UI_ENTERVALUE," ");
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		if ((dr_flag) && (key_val == KEY_UP))	
			--*piu_ptr;
			attrset(0);
			return(updt_piu(key_val,piu_ptr));	
	}
}

/***********************************************************************************/
int check_susp_clear(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int   old_piu,ret_code;
	char  rb;

	rb = Toupper(read_buf[0]);  /* Make sure it's upper case */

	if (*field_entered) {
		if (rb == YES || rb == NO) {
			if (rb == YES)
				clear_susp_flag = 1;
			else
				clear_susp_flag = 0;

			susp_clear_flag = rb;
			attrset(A_UNDERLINE);
			mvprintw(D_VEH_ROW,D_CLEAR_SUSP_COL,"%c",susp_clear_flag);
			attrset(0);
			return(updt_piu(key_val,piu_ptr));
		}
		else {
			prt_error(UI_NOFIELD," ");
			attrset(A_UNDERLINE);
			mvprintw(D_VEH_ROW,D_CLEAR_SUSP_COL,"%c",susp_clear_flag);
			attrset(0);
			old_piu = *piu_ptr;
			/*ret_code = updt_piu(key_val,piu_ptr);	*/
			ret_code = DONE;
			*piu_ptr = old_piu;
			return(ret_code);
		}
	}
	else {
		if ((key_val == KEY_UP) && (vh_flag))	{
			*piu_ptr = 2;
			return(updt_piu(key_val,piu_ptr));
		}
		else
			return(updt_piu(key_val,piu_ptr));
	}
}

/***********************************************************************************/
int check_susp_d_clear(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int   old_piu,ret_code;
	char  rb;

	rb = Toupper(read_buf[0]);  /* Make sure it's upper case */

	if (*field_entered) {
		if (rb == YES || rb == NO) {
			if (rb == YES)
				clear_susp_d_flag = 1;
			else
				clear_susp_d_flag = 0;

			susp_d_clear_flag = rb;
			attrset(A_UNDERLINE);
			mvprintw(D_VEH_ROW,D_CLEAR_SUSP_COL+5,"%c",susp_d_clear_flag);
			attrset(0);
			return(updt_piu(key_val,piu_ptr));
		}
		else {
			prt_error(UI_NOFIELD," ");
			attrset(A_UNDERLINE);
			mvprintw(D_VEH_ROW,D_CLEAR_SUSP_COL+5,"%c",susp_d_clear_flag);
			attrset(0);
			old_piu = *piu_ptr;
			/*ret_code = updt_piu(key_val,piu_ptr);	*/
			ret_code = DONE;
			*piu_ptr = old_piu;
			return(ret_code);
		}
	}
	else {
		return(updt_piu(key_val,piu_ptr));
	}
}

/***********************************************************************************/
int check_susp_x_clear(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int   old_piu,ret_code;
	char  rb;

	rb = Toupper(read_buf[0]);  /* Make sure it's upper case */

	if (*field_entered) {
		if (rb == YES || rb == NO) {
			if (rb == YES)
				clear_susp_x_flag = 1;
			else
				clear_susp_x_flag = 0;

			susp_x_clear_flag = rb;
			attrset(A_UNDERLINE);
			mvprintw(D_VEH_ROW,D_CLEAR_SUSP_COL+10,"%c",susp_x_clear_flag);
			attrset(0);
			return(updt_piu(key_val,piu_ptr));
		}
		else {
			prt_error(UI_NOFIELD," ");
			attrset(A_UNDERLINE);
			mvprintw(D_VEH_ROW,D_CLEAR_SUSP_COL+10,"%c",susp_x_clear_flag);
			attrset(0);
			old_piu = *piu_ptr;
			/*ret_code = updt_piu(key_val,piu_ptr);	*/
			ret_code = DONE;
			*piu_ptr = old_piu;
			return(ret_code);
		}
	}
	else {
		return(updt_piu(key_val,piu_ptr));
	}
}

/***********************************************************************************/
int check_susp_from_date(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int  old_piu,ret_code;

	if (*field_entered) {
		if((verify_date(read_buf,p_from_date)) == -1) {
			prt_error(BAD_DATE,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_3, "<format is MM/DD/YY>"));
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		attrset(A_UNDERLINE);
		mvprintw(D_PASS_ROW,D_SUSP_FROM_DATE_COL,"%s",p_from_date);
		attrset(0);
		susp_from_dt_tm = get_internal_time(p_from_date,p_from_time);
	}
	else {
		get_pkup_date(p_from_date);
		attrset(A_UNDERLINE);
		mvprintw(D_PASS_ROW,D_SUSP_FROM_DATE_COL,"%s",p_from_date);
		attrset(0);
		susp_from_dt_tm = get_internal_time(p_from_date, p_from_time);
	}
	return(updt_piu(key_val,piu_ptr));
}

/***********************************************************************************/
int check_susp_d_from_date(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int  old_piu,ret_code;

	if (*field_entered) {
		if((verify_date(read_buf,d_from_date)) == -1) {
			prt_error(BAD_DATE,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_3, "<format is MM/DD/YY>"));
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		attrset(A_UNDERLINE);
		mvprintw(D_DEL_ROW,D_SUSP_FROM_DATE_COL,"%s",d_from_date);
		attrset(0);
		susp_d_from_dt_tm = get_internal_time(d_from_date,d_from_time);
	}
	else {
		get_pkup_date(d_from_date);
		attrset(A_UNDERLINE);
		mvprintw(D_DEL_ROW,D_SUSP_FROM_DATE_COL,"%s",d_from_date);
		attrset(0);
		susp_d_from_dt_tm = get_internal_time(d_from_date, d_from_time);
	}
	return(updt_piu(key_val,piu_ptr));
}

/***********************************************************************************/
int check_susp_x_from_date(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int  old_piu,ret_code;

	if (*field_entered) {
		if((verify_date(read_buf,x_from_date)) == -1) {
			prt_error(BAD_DATE,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_3, "<format is MM/DD/YY>"));
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		attrset(A_UNDERLINE);
		mvprintw(D_TYPEX_ROW,D_SUSP_FROM_DATE_COL,"%s",x_from_date);
		attrset(0);
		susp_x_from_dt_tm = get_internal_time(x_from_date,x_from_time);
	}
	else {
		get_pkup_date(x_from_date);
		attrset(A_UNDERLINE);
		mvprintw(D_TYPEX_ROW,D_SUSP_FROM_DATE_COL,"%s",x_from_date);
		attrset(0);
		susp_x_from_dt_tm = get_internal_time(x_from_date, x_from_time);
	}
	return(updt_piu(key_val,piu_ptr));
}

/***********************************************************************************/
int check_susp_from_time(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int  old_piu,ret_code;

	if (*field_entered) {
		if((verify_time(read_buf,p_from_time)) == -1) {
			prt_error(BAD_TIME,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_4, "<format is HH:MM>"));
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		attrset(A_UNDERLINE);
		mvprintw(D_PASS_ROW,D_SUSP_FROM_TIME_COL,"%s",p_from_time);
		attrset(0);
		susp_from_dt_tm = get_internal_time(p_from_date,p_from_time);
	}
	else {
		get_pkup_time(p_from_time);
		attrset(A_UNDERLINE);
		mvprintw(D_PASS_ROW,D_SUSP_FROM_TIME_COL,"%s",p_from_time);
	attrset(0);
	susp_from_dt_tm = get_internal_time(p_from_date,p_from_time);
	}
	return(updt_piu(key_val,piu_ptr));
}

/***********************************************************************************/
int check_susp_d_from_time(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int  old_piu,ret_code;

	if (*field_entered) {
		if((verify_time(read_buf,d_from_time)) == -1) {
			prt_error(BAD_TIME,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_4, "<format is HH:MM>"));
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		attrset(A_UNDERLINE);
		mvprintw(D_DEL_ROW,D_SUSP_FROM_TIME_COL,"%s",d_from_time);
		attrset(0);
		susp_d_from_dt_tm = get_internal_time(d_from_date,d_from_time);
	}
	else {
		get_pkup_time(d_from_time);
		attrset(A_UNDERLINE);
		mvprintw(D_DEL_ROW,D_SUSP_FROM_TIME_COL,"%s",d_from_time);
		attrset(0);
		susp_d_from_dt_tm = get_internal_time(d_from_date,d_from_time);
	}
	return(updt_piu(key_val,piu_ptr));
}

/***********************************************************************************/
int check_susp_x_from_time(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int  old_piu,ret_code;

	if (*field_entered) {
		if((verify_time(read_buf,x_from_time)) == -1) {
			prt_error(BAD_TIME,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_4, "<format is HH:MM>"));
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		attrset(A_UNDERLINE);
		mvprintw(D_TYPEX_ROW,D_SUSP_FROM_TIME_COL,"%s",x_from_time);
		attrset(0);
		susp_x_from_dt_tm = get_internal_time(x_from_date,x_from_time);
	}
	else {
		get_pkup_time(x_from_time);
		attrset(A_UNDERLINE);
		mvprintw(D_TYPEX_ROW,D_SUSP_FROM_TIME_COL,"%s",x_from_time);
		attrset(0);
		susp_x_from_dt_tm = get_internal_time(x_from_date,x_from_time);
	}
	return(updt_piu(key_val,piu_ptr));
}

/***********************************************************************************/
int check_susp_to_date(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int  old_piu,ret_code;

	if (*field_entered) {
		if((verify_date(read_buf,p_to_date)) == -1) {
			prt_error(BAD_DATE,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_3, "<format is MM/DD/YY>"));
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		if ( get_internal_time(p_to_date, p_to_time) < susp_from_dt_tm )
		  {
		    prt_error(BAD_DATE, " " );
		    old_piu = *piu_ptr;
		    ret_code = updt_piu(key_val, piu_ptr);
		    *piu_ptr = old_piu;
		    return(ret_code);
		  }
		attrset(A_UNDERLINE);
		mvprintw(D_PASS_ROW,D_SUSP_TO_DATE_COL,"%s",p_to_date);
		attrset(0);
		susp_to_dt_tm = get_internal_time(p_to_date,p_to_time);
	}
	else {
	        /* Set high limit */
	        if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		    sprintf(read_buf, "31%c12%c99", DATE_SEPERATOR, DATE_SEPERATOR);
	        else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
		    sprintf(read_buf, "99%c12%c31", DATE_SEPERATOR, DATE_SEPERATOR);
		else
		    sprintf(read_buf, "12%c31%c99", DATE_SEPERATOR, DATE_SEPERATOR);
		verify_date(read_buf,p_to_date);
		attrset(A_UNDERLINE);
		mvprintw(D_PASS_ROW,D_SUSP_TO_DATE_COL,"%s",p_to_date);
		attrset(0);
		susp_to_dt_tm = get_internal_time(p_to_date, p_to_time);
	}
	return(updt_piu(key_val,piu_ptr));
}


/***********************************************************************************/
int check_susp_d_to_date(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int  old_piu,ret_code;

	if (*field_entered) {
		if((verify_date(read_buf,d_to_date)) == -1) {
			prt_error(BAD_DATE,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_3, "<format is MM/DD/YY>"));
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		if ( get_internal_time(d_to_date, d_to_time) < susp_d_from_dt_tm )
		  {
		    prt_error(BAD_DATE, " " );
		    old_piu = *piu_ptr;
		    ret_code = updt_piu(key_val, piu_ptr);
		    *piu_ptr = old_piu;
		    return(ret_code);
		  }
		attrset(A_UNDERLINE);
		mvprintw(D_DEL_ROW,D_SUSP_TO_DATE_COL,"%s",d_to_date);
		attrset(0);
		susp_d_to_dt_tm = get_internal_time(d_to_date,d_to_time);
	}
	else {
	        /* Set high limit */
	        if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		    sprintf(read_buf, "31%c12%c99", DATE_SEPERATOR, DATE_SEPERATOR);
	        else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
		    sprintf(read_buf, "99%c12%c31", DATE_SEPERATOR, DATE_SEPERATOR);
		else
		    sprintf(read_buf, "12%c31%c99", DATE_SEPERATOR, DATE_SEPERATOR);
		verify_date(read_buf,d_to_date);
		attrset(A_UNDERLINE);
		mvprintw(D_DEL_ROW,D_SUSP_TO_DATE_COL,"%s",d_to_date);
		attrset(0);
		susp_to_dt_tm = get_internal_time(d_to_date, d_to_time);
	}
	return(updt_piu(key_val,piu_ptr));
}


/***********************************************************************************/
int check_susp_x_to_date(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int  old_piu,ret_code;

	if (*field_entered) {
		if((verify_date(read_buf,x_to_date)) == -1) {
			prt_error(BAD_DATE,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_3, "<format is MM/DD/YY>"));
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		if ( get_internal_time(x_to_date, x_to_time) < susp_x_from_dt_tm )
		  {
		    prt_error(BAD_DATE, " " );
		    old_piu = *piu_ptr;
		    ret_code = updt_piu(key_val, piu_ptr);
		    *piu_ptr = old_piu;
		    return(ret_code);
		  }
		attrset(A_UNDERLINE);
		mvprintw(D_TYPEX_ROW,D_SUSP_TO_DATE_COL,"%s",x_to_date);
		attrset(0);
		susp_x_to_dt_tm = get_internal_time(x_to_date,x_to_time);
	}
	else {
	        /* Set high limit */
	        if (DATE_FORMAT == DAY_FIRST_IN_DATE)
		    sprintf(read_buf, "31%c12%c99", DATE_SEPERATOR, DATE_SEPERATOR);
	        else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
		    sprintf(read_buf, "99%c12%c31", DATE_SEPERATOR, DATE_SEPERATOR);
		else
		    sprintf(read_buf, "12%c31%c99", DATE_SEPERATOR, DATE_SEPERATOR);
		verify_date(read_buf,x_to_date);
		attrset(A_UNDERLINE);
		mvprintw(D_TYPEX_ROW,D_SUSP_TO_DATE_COL,"%s",x_to_date);
		attrset(0);
		susp_x_to_dt_tm = get_internal_time(x_to_date, x_to_time);
	}
	return(updt_piu(key_val,piu_ptr));
}

/***********************************************************************************/
int check_susp_to_time(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int  old_piu,ret_code;

	if (*field_entered) {
		if((verify_time(read_buf,p_to_time)) == -1) {
			prt_error(BAD_TIME,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_4, "<format is HH:MM>"));
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		if ( get_internal_time(p_to_date, p_to_time) < susp_from_dt_tm )
		  {
		    prt_error(BAD_DATE, " " );
		    old_piu = *piu_ptr;
		    ret_code = updt_piu(key_val, piu_ptr);
		    *piu_ptr = old_piu;
		    return(ret_code);
		  }
		attrset(A_UNDERLINE);
		mvprintw(D_PASS_ROW,D_SUSP_TO_TIME_COL,"%s",p_to_time);
		attrset(0);
		susp_to_dt_tm = get_internal_time(p_to_date,p_to_time);
	}
	else {
#ifdef HR12
		strcpy(read_buf,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_6, "MIDN"));			/* midnight */
#else
		strcpy(read_buf, "23:59");
#endif
		verify_time(read_buf,p_to_time);
		attrset(A_UNDERLINE);
		mvprintw(D_PASS_ROW,D_SUSP_TO_TIME_COL,"%s",p_to_time);
		attrset(0);
		susp_to_dt_tm = get_internal_time(p_to_date,p_to_time);
	}
	return(updt_piu(key_val,piu_ptr));
}


/***********************************************************************************/
int check_susp_d_to_time(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int  old_piu,ret_code;

	if (*field_entered) {
		if((verify_time(read_buf,d_to_time)) == -1) {
			prt_error(BAD_TIME,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_4, "<format is HH:MM>"));
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		if ( get_internal_time(d_to_date, d_to_time) < susp_d_from_dt_tm )
		  {
		    prt_error(BAD_DATE, " " );
		    old_piu = *piu_ptr;
		    ret_code = updt_piu(key_val, piu_ptr);
		    *piu_ptr = old_piu;
		    return(ret_code);
		  }
		attrset(A_UNDERLINE);
		mvprintw(D_DEL_ROW,D_SUSP_TO_TIME_COL,"%s",d_to_time);
		attrset(0);
		susp_d_to_dt_tm = get_internal_time(d_to_date,d_to_time);
	}
	else {
#ifdef HR12
		strcpy(read_buf,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_6, "MIDN"));			/* midnight */
#else
		strcpy(read_buf, "23:59");
#endif
		verify_time(read_buf,d_to_time);
		attrset(A_UNDERLINE);
		mvprintw(D_DEL_ROW,D_SUSP_TO_TIME_COL,"%s",d_to_time);
		attrset(0);
		susp_d_to_dt_tm = get_internal_time(d_to_date,d_to_time);
	}
	return(updt_piu(key_val,piu_ptr));
}


/***********************************************************************************/
int check_susp_x_to_time(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int  old_piu,ret_code;

	if (*field_entered) {
		if((verify_time(read_buf,x_to_time)) == -1) {
			prt_error(BAD_TIME,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_4, "<format is HH:MM>"));
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		if ( get_internal_time(x_to_date, x_to_time) < susp_x_from_dt_tm )
		  {
		    prt_error(BAD_DATE, " " );
		    old_piu = *piu_ptr;
		    ret_code = updt_piu(key_val, piu_ptr);
		    *piu_ptr = old_piu;
		    return(ret_code);
		  }
		attrset(A_UNDERLINE);
		mvprintw(D_TYPEX_ROW,D_SUSP_TO_TIME_COL,"%s",x_to_time);
		attrset(0);
		susp_x_to_dt_tm = get_internal_time(x_to_date,x_to_time);
	}
	else {
#ifdef HR12
		strcpy(read_buf,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_6, "MIDN"));			/* midnight */
#else
		strcpy(read_buf, "23:59");
#endif
		verify_time(read_buf,x_to_time);
		attrset(A_UNDERLINE);
		mvprintw(D_TYPEX_ROW,D_SUSP_TO_TIME_COL,"%s",x_to_time);
		attrset(0);
		susp_x_to_dt_tm = get_internal_time(x_to_date,x_to_time);
	}
	return(updt_piu(key_val,piu_ptr));
}

/********************************************************************************/
int check_pass(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int   old_piu,ret_code;
	char  rb;

	rb = Toupper(read_buf[0]);  /* Make sure it's upper case */

	if (*field_entered) {
		if (rb == YES || rb == NO) {
			susp_pass_call = rb;
			if (rb == NO)
				clear_susp_flag = TRUE;
			attrset(A_UNDERLINE);
			mvprintw(D_PASS_ROW,D_PASS_COL,"%c",susp_pass_call);
			attrset(0);
			if ((rb == NO) && (key_val != KEY_UP)) {
				*piu_ptr += 4;
			}
			return(updt_piu(key_val,piu_ptr));
		}
		else	{
			prt_error(UI_NOFIELD," ");
			attrset(A_UNDERLINE);
			mvprintw(D_PASS_ROW,D_PASS_COL,"%c",susp_pass_call);
			attrset(0);
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
	}
	else  		
		if ((rb == NO) && (key_val != KEY_UP)) {
			*piu_ptr += 4;
		}
		return(updt_piu(key_val,piu_ptr));
}

/*************************************************************************************/
int check_del(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int  old_piu,ret_code;
	char  rb;

	rb = Toupper(read_buf[0]);  /* Make sure it's upper case */

	if (*field_entered) {
		if (rb == YES || rb == NO) {
			susp_del_call = rb;
			if (rb == NO)
				clear_susp_d_flag = TRUE;
			attrset(A_UNDERLINE);
			mvprintw(D_DEL_ROW,D_DEL_COL,"%c",susp_del_call);
			attrset(0);
			if ((rb == NO) && (key_val != KEY_UP)) {
				*piu_ptr += 4;
			}
			if ((susp_pass_call == NO) && (key_val == KEY_UP)) {
				*piu_ptr -= 4;
			}
			return(updt_piu(key_val,piu_ptr));
		}
		else	{
			prt_error(UI_NOFIELD," ");
			attrset(A_UNDERLINE);
			mvprintw(D_DEL_ROW,D_DEL_COL,"%c",susp_del_call);
			attrset(0);
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
	}
	else  	
		if ((rb == NO) && (key_val != KEY_UP)) {
			*piu_ptr += 4;
		}
		if ((susp_pass_call == NO) && (key_val == KEY_UP)) {
			*piu_ptr -= 4;
		}
		return(updt_piu(key_val,piu_ptr));
}

/***************************************************************************/
int check_typex(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int  old_piu,ret_code;
	char  rb;

	rb = Toupper(read_buf[0]);  /* Make sure it's upper case */

	if (*field_entered) {
		if (rb == YES || rb == NO) {
			susp_typex_call = rb;
			if (rb == NO)
				clear_susp_x_flag = TRUE;	
			attrset(A_UNDERLINE);
			mvprintw(D_TYPEX_ROW,D_TYPEX_COL,"%c",susp_typex_call);
			attrset(0);
			if ((rb == NO) && (key_val != KEY_UP)) {
				*piu_ptr += 4;
			}
			if ((susp_del_call == NO) && (key_val == KEY_UP)) {
				*piu_ptr -= 4;
			}
			return(updt_piu(key_val,piu_ptr));
		}
		else	{
			prt_error(UI_NOFIELD," ");
			attrset(A_UNDERLINE);
			mvprintw(D_TYPEX_ROW,D_TYPEX_COL,"%c",susp_typex_call);
			attrset(0);
			old_piu = *piu_ptr;
			ret_code = updt_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
	}
	else  	
		if ((rb == NO) && (key_val != KEY_UP)) {
			*piu_ptr += 4;
		}
		if ((susp_del_call == NO) && (key_val == KEY_UP)) {
			*piu_ptr -= 4;
		}
		return(updt_piu(key_val,piu_ptr));
}

/*********************************************************************************/
int check_reason(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	if (*field_entered) {
		strcpy(susp_reason,read_buf);
		attrset(A_UNDERLINE);
		mvprintw(D_REASON_ROW,D_REASON_COL,"%s",susp_reason);
		attrset(0);
	}
	if ((susp_typex_call == NO) && (key_val == KEY_UP)) {
		*piu_ptr -= 4;
	}
	return(updt_piu(key_val,piu_ptr));
}

/*****************************************************************************/
int check_msg1(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	if (*field_entered) 
		strcpy(message1,read_buf);
	refresh();			/* sine curses bug */
	return(updt_piu(key_val,piu_ptr));
}

/*******************************************************************************/
int check_msg2(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	if (*field_entered) 
		strcpy(message2,read_buf);
	refresh();			/* sine curses bug */
	return(updt_piu(key_val,piu_ptr));
}

/******************************************************************************/
int check_msg3(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;  				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	if (*field_entered) 
		strcpy(message3,read_buf);
	refresh();			/* sine curses bug */
	return(updt_piu(key_val,piu_ptr));
}

/**************************************************************************************/
send_to_disk()
{
	char tmp[4];
	int i;

	if (dr_flag) {					/* driver file */
		if ((db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISEQUAL+ISLOCK,0)) < 0) {
			prt_error(UI_DSKREAD," ");
			return(DONE);
		}
		if (clear_susp_flag) {					/* clear suspension */
			dr_ptr->susp_from_dt_tm = NULL;
			dr_ptr->susp_to_dt_tm = NULL;
			dr_ptr->susp_pass_call = NO;
			strcpy(dr_ptr->susp_from_date,"00/00/00");
			strcpy(dr_ptr->susp_from_time,"00:00");
			strcpy(dr_ptr->susp_to_date,"00/00/00");
			strcpy(dr_ptr->susp_to_time,"00:00");
			strcpy(dr_ptr->susp_reason,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_7, "REINSTATED"));
		}
		else {
			dr_ptr->susp_from_dt_tm = susp_from_dt_tm;
			dr_ptr->susp_to_dt_tm = susp_to_dt_tm;
			strcpy(dr_ptr->susp_from_date,p_from_date);
			strcpy(dr_ptr->susp_from_time,p_from_time);
			strcpy(dr_ptr->susp_to_date,p_to_date);
			strcpy(dr_ptr->susp_to_time,p_to_time);
			dr_ptr->susp_pass_call = susp_pass_call;
			strcpy(dr_ptr->susp_reason,susp_reason);
			sprintf(message,"%-32s%-32s%-32s",message1,message2,message3);
			strcpy(dr_ptr->susp_message,message);
		}
		if (clear_susp_d_flag) {					/* clear suspension */
			dr_ptr->susp_d_from_dt_tm = NULL;
			dr_ptr->susp_d_to_dt_tm = NULL;
			dr_ptr->susp_del_call = NO;
			strcpy(dr_ptr->susp_d_from_date,"00/00/00");
			strcpy(dr_ptr->susp_d_from_time,"00:00");
			strcpy(dr_ptr->susp_d_to_date,"00/00/00");
			strcpy(dr_ptr->susp_d_to_time,"00:00");
			strcpy(dr_ptr->susp_reason,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_7, "REINSTATED"));
		}
		else {
			dr_ptr->susp_d_from_dt_tm = susp_d_from_dt_tm;
			dr_ptr->susp_d_to_dt_tm = susp_d_to_dt_tm;
			strcpy(dr_ptr->susp_d_from_date,d_from_date);
			strcpy(dr_ptr->susp_d_from_time,d_from_time);
			strcpy(dr_ptr->susp_d_to_date,d_to_date);
			strcpy(dr_ptr->susp_d_to_time,d_to_time);
			dr_ptr->susp_del_call = susp_del_call;
			strcpy(dr_ptr->susp_reason,susp_reason);
			sprintf(message,"%-32s%-32s%-32s",message1,message2,message3);
			strcpy(dr_ptr->susp_message,message);
		}
		if (clear_susp_x_flag) {					/* clear suspension */
			dr_ptr->susp_x_from_dt_tm = NULL;
			dr_ptr->susp_x_to_dt_tm = NULL;
			dr_ptr->susp_typex_call = NO;
			strcpy(dr_ptr->susp_x_from_date,"00/00/00");
			strcpy(dr_ptr->susp_x_from_time,"00:00");
			strcpy(dr_ptr->susp_x_to_date,"00/00/00");
			strcpy(dr_ptr->susp_x_to_time,"00:00");
			strcpy(dr_ptr->susp_reason,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_7, "REINSTATED"));
		}
		else {
			dr_ptr->susp_x_from_dt_tm = susp_x_from_dt_tm;
			dr_ptr->susp_x_to_dt_tm = susp_x_to_dt_tm;
			strcpy(dr_ptr->susp_x_from_date,x_from_date);
			strcpy(dr_ptr->susp_x_from_time,x_from_time);
			strcpy(dr_ptr->susp_x_to_date,x_to_date);
			strcpy(dr_ptr->susp_x_to_time,x_to_time);
			dr_ptr->susp_typex_call = susp_typex_call;
			strcpy(dr_ptr->susp_reason,susp_reason);
			sprintf(message,"%-32s%-32s%-32s",message1,message2,message3);
			strcpy(dr_ptr->susp_message,message);
		}
		attrset(0);
		if ((db(DRIVER_FILE_ID,UPDATE_REC,&dr_key1,0,0)) < 0)	{
			prt_error(UI_NOUPDATE," ");
			return(DONE);
		}
		if ((db(DRIVER_FILE_ID,RELEASE,&dr_key1,0,0)) < 0)	{
			prt_error(UI_NOUPDATE," ");
			return(DONE);
		}
		send_dr_ipc();
	}
	else {
		if ((db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISEQUAL+ISLOCK,0)) < 0) {
			prt_error(UI_DSKREAD," ");
			return(DONE);
		}
		if (clear_susp_flag) {					/* clear suspension */
			vh_ptr->susp_from_dt_tm = NULL;
			vh_ptr->susp_to_dt_tm = NULL;
			strcpy(vh_ptr->susp_from_date,"00/00/00");
			strcpy(vh_ptr->susp_from_time,"00:00");
			strcpy(vh_ptr->susp_to_date,"00/00/00");
			strcpy(vh_ptr->susp_to_time,"00:00");
			vh_ptr->susp_pass_call = NO;
			strcpy(vh_ptr->susp_reason,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_7, "REINSTATED"));
		}
		else {
			vh_ptr->susp_from_dt_tm = susp_from_dt_tm;
			vh_ptr->susp_to_dt_tm = susp_to_dt_tm;
			strcpy(vh_ptr->susp_from_date, p_from_date);
			strcpy(vh_ptr->susp_from_time, p_from_time);
			strcpy(vh_ptr->susp_to_date, p_to_date);
			strcpy(vh_ptr->susp_to_time, p_to_time);
			vh_ptr->susp_pass_call = susp_pass_call;
			strcpy(vh_ptr->susp_reason,susp_reason);
			sprintf(message,"%-32s%-32s%-32s",message1,message2,message3);
			strcpy(vh_ptr->susp_message,message);
		}
		if (clear_susp_d_flag) {					/* clear suspension */
			vh_ptr->susp_d_from_dt_tm = NULL;
			vh_ptr->susp_d_to_dt_tm = NULL;
			strcpy(vh_ptr->susp_d_from_date,"00/00/00");
			strcpy(vh_ptr->susp_d_from_time,"00:00");
			strcpy(vh_ptr->susp_d_to_date,"00/00/00");
			strcpy(vh_ptr->susp_d_to_time,"00:00");
			vh_ptr->susp_del_call = NO;
			strcpy(vh_ptr->susp_reason,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_7, "REINSTATED"));
		}
		else {
			vh_ptr->susp_d_from_dt_tm = susp_d_from_dt_tm;
			vh_ptr->susp_d_to_dt_tm = susp_d_to_dt_tm;
			strcpy(vh_ptr->susp_d_from_date, d_from_date);
			strcpy(vh_ptr->susp_d_from_time, d_from_time);
			strcpy(vh_ptr->susp_d_to_date, d_to_date);
			strcpy(vh_ptr->susp_d_to_time, d_to_time);
			vh_ptr->susp_del_call = susp_del_call;
			strcpy(vh_ptr->susp_reason,susp_reason);
			sprintf(message,"%-32s%-32s%-32s",message1,message2,message3);
			strcpy(vh_ptr->susp_message,message);
		}
		if (clear_susp_x_flag) {					/* clear suspension */
			vh_ptr->susp_x_from_dt_tm = NULL;
			vh_ptr->susp_x_to_dt_tm = NULL;
			strcpy(vh_ptr->susp_x_from_date,"00/00/00");
			strcpy(vh_ptr->susp_x_from_time,"00:00");
			strcpy(vh_ptr->susp_x_to_date,"00/00/00");
			strcpy(vh_ptr->susp_x_to_time,"00:00");
			vh_ptr->susp_typex_call = NO;
			strcpy(vh_ptr->susp_reason,catgets(UI_m_catd, UI_1, UI_SP_CK_FLD_7, "REINSTATED"));
		}
		else {
			vh_ptr->susp_x_from_dt_tm = susp_x_from_dt_tm;
			vh_ptr->susp_x_to_dt_tm = susp_x_to_dt_tm;
			strcpy(vh_ptr->susp_x_from_date, x_from_date);
			strcpy(vh_ptr->susp_x_from_time, x_from_time);
			strcpy(vh_ptr->susp_x_to_date, x_to_date);
			strcpy(vh_ptr->susp_x_to_time, x_to_time);
			vh_ptr->susp_typex_call = susp_typex_call;
			strcpy(vh_ptr->susp_reason,susp_reason);
			sprintf(message,"%-32s%-32s%-32s",message1,message2,message3);
			strcpy(vh_ptr->susp_message,message);
		}
		attrset(0);
		if ((db(VEHICLE_FILE_ID,UPDATE_REC,&veh_key2,0,0)) < 0)	{
			prt_error(UI_NOUPDATE," ");
			return(DONE);
		}
		if ((db(VEHICLE_FILE_ID,RELEASE,&veh_key2,0,0)) < 0)	{
			prt_error(UI_NOUPDATE," ");
			return(DONE);
		}
		send_vh_ipc();
	}

	prt_entry_areas(stdscr,sp_scr_flds,MAX_PIU_SUSPEND);
	sp_print_values();				/* print default values on screen and reset values */
}

/***************************************************************************/
clear_fld(row1,col1,nums)
int  row1;
int  col1;
int  nums;
{
	int  i;

	move(row1,col1);
	refresh();
	for (i = 0; i < nums; ++i)
		addch(' ');
}

/*******************************************************************/
numeric(in_str)
char  *in_str;
{
	int i;
	int valid;
	i = 0;
	valid = 1;
	while(*in_str) {
		if (!isdigit(*in_str) )
			valid = 0;
			in_str++;
	}
	return(valid);
}

/**************************************************************************************/
/* send_dr_ipc - send a message to dispatch that a record in the driver file changed. */
/**************************************************************************************/
send_dr_ipc()
{
	struct df_maint df_maint_ipc;			/* structure for passing information to dispatch  */

	df_maint_ipc.d_prcs_id = DISP_DEST;
	df_maint_ipc.u_prcs_id = pid;
	df_maint_ipc.rec_type = DFM_REQ;		
	df_maint_ipc.which_file = DRIVER_FILE_ID;
	df_maint_ipc.fleet = dr_ptr->fleet; 
	df_maint_ipc.key = dr_ptr->id; 
	df_maint_ipc.cisam_num = isrecnum;         
	if((msgsnd(msgkey,&df_maint_ipc,sizeof(df_maint_ipc),0)) == -1)  {
		prt_error(UI_DRIVER_SND_ERR," ");
		return(-1);
	}
	return(0);
}

/********************************************************************************************/
/* send_vh_ipc - send a message to dispatch that a record in the user profile file changed. */
/********************************************************************************************/
send_vh_ipc()
{
	struct df_maint df_maint_ipc;			/* structure for passing information to dispatch  */

	df_maint_ipc.d_prcs_id = DISP_DEST;
	df_maint_ipc.u_prcs_id = pid;
	df_maint_ipc.rec_type = DFM_REQ;		
	df_maint_ipc.which_file = VEHICLE_FILE_ID;
	df_maint_ipc.key = vh_ptr->nbr; 
	df_maint_ipc.fleet = vh_ptr->fleet;
	df_maint_ipc.cisam_num = isrecnum;         
	if((msgsnd(msgkey,&df_maint_ipc,sizeof(df_maint_ipc),0)) == -1)  {
		prt_error(UI_VEHICLE_SND_ERR," ");
		return(-1);
	}
	return(0);
}
