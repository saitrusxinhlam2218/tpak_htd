static char sz__FILE__[]="@(#)up_ck_flds.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  up_ck_flds.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:48:21
 * @(#)  Last delta: 12/2/94 at 11:47:54
 * @(#)  SCCS File: /taxi/sccs/s.up_ck_flds.c
 *                                                                           *
 *****************************************************************************/

#include "up_m_strdefs.h"
#include <pwd.h>
#include <cursesX.h>
#include <ctype.h>
#include <sys/types.h>
#include "mad_error.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "up_maint.h"
#include "ex_key.h"
#include "df_maint.h"
#include "literals.h"
#include "path.h"
#include "tokenize.h"
#include "language.h"

extern short ipc_required;		/* Set if a variable which resides */
					/* shared memory changes */
extern struct cisam_up *up_ptr;		/* pointer to user profile file structure */
extern struct cisam_up *temp_up_ptr;		/* pointer to user profile file structure */
extern char read_buf[];			/* buffer for reading fields */
extern FILE *trace_fp;			/* trace file pointer */
extern char trace_str[];		/* trace string */
extern int piu;				/* prompt in use */
extern int key_val;			/* value of key entered */
extern int err_n_fld;			/* set if an error in field condition exists */
extern int fl_flags;
extern int city_flags;
extern int doing_add;			/* performing add operation */
extern char req_buf[];
extern int usr_id_flag;
extern int name_flag;
extern prt_error();
extern clear_err_line();

int update_piu();
int numeric();
char *str_to_upper();
char *str_to_lower();


int check_dummy(), check_id(), check_name(), check_full(),check_grt_name(), check_y_n();
int check_fleet(), check_screen(), check_search(), check_position();
int check_cl_type(), check_city(), check_hi_pri();
int check_lg_file(), check_spr_zone();

/*  Declare an array of pointers to functions which check the field entered
 *  for validity.  
 */

int (*check_field[])() = {check_dummy, check_id, check_name, check_full,check_grt_name,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_fleet,
			  check_screen, check_search, check_position,
			  check_cl_type, check_city, check_hi_pri, 
			  check_y_n, check_lg_file,check_y_n,check_spr_zone,
			  check_spr_zone, check_spr_zone, check_spr_zone,
			  check_spr_zone, check_spr_zone, check_spr_zone,
			  check_spr_zone, check_spr_zone, check_spr_zone,
			  check_spr_zone, check_spr_zone, check_spr_zone,
			  check_spr_zone, check_spr_zone, check_spr_zone,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n, check_y_n, check_y_n };

/******************************************************************************
* check_dummy - This routine is simply a place holder so the first field is 1.
*****************************************************************************/

int check_dummy (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}


/*****************************************************************************
* check_id - check the user ID.
*****************************************************************************/

int check_id (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int uid;		/* user id */

#ifdef DEBUG
	sprintf(trace_str,"Entering check_id\n");
	TRACE(trace_str);
#endif

		clear_err_line();
	if(*field_entered)  {
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_id - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		err_n_fld = 0;
		uid = atoi(read_buf);
		up_ptr->user_id = (short) uid;	/* move user id into isam buffer */
		usr_id_flag = 1;
		mvprintw(D_USER_ID_ROW,D_USER_ID_COL,"%4d",up_ptr->user_id);
		refresh();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_id - 2\n");
	TRACE(trace_str);
#endif
		return(VALID);
	}
	else	{
		if((!usr_id_flag) && (key_val != ESC))	{
			prt_error(catgets(UP_MAINT_m_catd, UP_MAINT_SET, UP_MAINT_1, "You must enter value"));
			err_n_fld = 1;
			return(NOT_VALID);
		}
		else	{
			err_n_fld = 0;
			return(VALID);
		}
	}
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_id - 3\n");
	TRACE(trace_str);
#endif

}  /* End of check_id()  */

/*****************************************************************************
* check_name - check the user name.
*****************************************************************************/

int check_name (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	struct passwd *getpwent(), *pw_ptr;

#ifdef DEBUG
	sprintf(trace_str,"Entering check_name\n");
	TRACE(trace_str);
#endif
		clear_err_line();
	if(*field_entered)  {
		err_n_fld = 0;
			while(1)  {
				if((pw_ptr = getpwent()) == NULL)  {
					prt_error(NO_U_NAME);
					setpwent();
					err_n_fld = 1;
					return(NOT_VALID);
				}
				
				if(strcmp(pw_ptr->pw_name,str_to_lower(read_buf)) == 0)  {
					up_ptr->user_id = (short)pw_ptr->pw_uid;
					setpwent();
					mvprintw(D_USER_ID_ROW,D_USER_ID_COL,"%4d",up_ptr->user_id);
					refresh();
					break;
				}
			}
		strcpy(up_ptr->user_name,str_to_lower(read_buf));
		name_flag = 1;
		mvprintw(D_USER_ID_ROW,D_USER_NAME_COL,"%s",up_ptr->user_name);
		refresh();
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_name - 1\n");
	TRACE(trace_str);
#endif
		return(VALID);
	}
	else	{
		if (!err_n_fld)
			update_piu();
		return(VALID);
	}

}  /* End of check_name()  */

/*****************************************************************************
* check_grt_name - check the greeting name.
****************************************************************************/

int check_grt_name (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering check_grt_name\n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		ipc_required = 1;
		clear_err_line();
		err_n_fld = 0;
		strcpy(up_ptr->greeting_name,str_to_upper(read_buf));
		mvprintw(D_USER_ID_ROW,D_GRT_NAME_COL,"%s",up_ptr->greeting_name);
		refresh();
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_grt_name - 1\n");
	TRACE(trace_str);
#endif
		return(VALID);
	}
	if(!err_n_fld)
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_grt_name - 2\n");
	TRACE(trace_str);
#endif
	return(VALID);

}  /* End of check_grt_name()  */
/*****************************************************************************
* check_full - check the greeting name.
****************************************************************************/

int check_full (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering check_full\n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		err_n_fld = 0;
/*
		removed uppercase 910227 to be consistent with /etc/passwd version -- tek
		strcpy(up_ptr->fullname,str_to_upper(read_buf));
*/
		strcpy(up_ptr->fullname,read_buf);
		mvprintw(D_USER_ID_ROW,D_FULLNAME,"%s",up_ptr->fullname);
		refresh();
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_full- 1\n");
	TRACE(trace_str);
#endif
		return(VALID);
	}
	if(!err_n_fld)
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_full- 2\n");
	TRACE(trace_str);
#endif
	return(VALID);

}  /* End of check_full*/

/*****************************************************************************
* check_y_n - check the field for a 'y' or 'n'.
****************************************************************************/

int check_y_n (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
    char c;

#ifdef DEBUG
	sprintf(trace_str,"Entering check_y_n\n");
	TRACE(trace_str);
#endif
    c = Toupper(read_buf[0]);

	if(*field_entered)  {
		clear_err_line();
		if(c == YES || c == NO) {
			clear_err_line();
			err_n_fld = 0;
			ipc_required = 1;
			switch(piu)  {
				case F_FL_PERM_1 :
					up_ptr->fleet_perms[0] = c;
					mvprintw(D_USER_ID_ROW,D_FLT_PERM_1_COL,"%c",up_ptr->fleet_perms[0]);
					break;
				case F_FL_PERM_2 :
					up_ptr->fleet_perms[1] = c;
					mvprintw(D_USER_ID_ROW,D_FLT_PERM_2_COL,"%c",up_ptr->fleet_perms[1]);
					break;
				case F_FL_PERM_3 :
					up_ptr->fleet_perms[2] = c;
					mvprintw(D_USER_ID_ROW,D_FLT_PERM_3_COL,"%c",up_ptr->fleet_perms[2]);
					break;
				case F_FL_PERM_4 : 
					up_ptr->fleet_perms[3] = c;
					mvprintw(D_USER_ID_ROW,D_FLT_PERM_4_COL,"%c",up_ptr->fleet_perms[3]);
					break;
				case F_FL_PERM_5 :
					up_ptr->fleet_perms[4] = c;
					mvprintw(D_USER_ID_ROW,D_FLT_PERM_5_COL,"%c",up_ptr->fleet_perms[4]);
					break;
				case F_FL_PERM_6 :
					up_ptr->fleet_perms[5] = c;
					mvprintw(D_USER_ID_ROW,D_FLT_PERM_6_COL,"%c",up_ptr->fleet_perms[5]);
					break;
				case F_FL_PERM_7 :
					up_ptr->fleet_perms[6] = c;
					mvprintw(D_USER_ID_ROW,D_FLT_PERM_7_COL,"%c",up_ptr->fleet_perms[6]);
					break;
				case F_FL_PERM_8 :
					up_ptr->fleet_perms[7] = c;
					mvprintw(D_USER_ID_ROW,D_FLT_PERM_8_COL,"%c",up_ptr->fleet_perms[7]);
					break;
				case F_M_PERM_1 :
					up_ptr->menu_perms[1] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_1_COL,"%c",up_ptr->menu_perms[1]);
					break;
				case F_M_PERM_2 :
					up_ptr->menu_perms[2] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_2_COL,"%c",up_ptr->menu_perms[2]);
					break;
				case F_M_PERM_3 :
					up_ptr->menu_perms[3] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_3_COL,"%c",up_ptr->menu_perms[3]);
					break;
				case F_M_PERM_4 :
					up_ptr->menu_perms[4] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_4_COL,"%c",up_ptr->menu_perms[4]);
					break;
				case F_M_PERM_5 :
					up_ptr->menu_perms[5] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_5_COL,"%c",up_ptr->menu_perms[5]);
					break;
				case F_M_PERM_6 :
					up_ptr->menu_perms[6] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_6_COL,"%c",up_ptr->menu_perms[6]);
					break;
				case F_M_PERM_7 :
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_7_COL,"%c", NO);
					break;
				case F_M_PERM_8 :
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_8_COL,"%c", NO);
					break;
				case F_M_PERM_9 :
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_9_COL,"%c", NO);
					break;
				case F_M_PERM_11 :
					up_ptr->menu_perms[7] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_11_COL,"%c",up_ptr->menu_perms[7]);
					break;
				case F_M_PERM_12 :	
					up_ptr->menu_perms[8] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_12_COL,"%c",up_ptr->menu_perms[8]);
					break;
				case F_M_PERM_13 :
					up_ptr->menu_perms[9] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_13_COL,"%c",up_ptr->menu_perms[9]);
					break;
				case F_M_PERM_14 :
					up_ptr->menu_perms[10] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_14_COL,"%c",up_ptr->menu_perms[10]);
					break;
				case F_M_PERM_15 :
					up_ptr->menu_perms[11] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_15_COL,"%c",up_ptr->menu_perms[11]);
					break;
				case F_M_PERM_16 :
					up_ptr->menu_perms[12] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_16_COL,"%c",up_ptr->menu_perms[12]);
					break;
				case F_M_PERM_17 :
					up_ptr->menu_perms[13] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_17_COL,"%c",up_ptr->menu_perms[13]);
					break;
				case F_M_PERM_18 :
					up_ptr->menu_perms[14] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_18_COL,"%c",up_ptr->menu_perms[14]);
					break;
				case F_M_PERM_19 :
					up_ptr->menu_perms[15] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_19_COL,"%c",up_ptr->menu_perms[15]);
					break;
				case F_M_PERM_21 :
					up_ptr->menu_perms[16] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_21_COL,"%c",up_ptr->menu_perms[16]);
					break;
				case F_M_PERM_22 :
					up_ptr->menu_perms[17] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_22_COL,"%c",up_ptr->menu_perms[17]);
					break;
				case F_M_PERM_23 :
					up_ptr->menu_perms[18] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_23_COL,"%c",up_ptr->menu_perms[18]);
					break;
				case F_M_PERM_24 :
					up_ptr->menu_perms[19] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_24_COL,"%c",up_ptr->menu_perms[19]);
					break;
				case F_M_PERM_25 :
					up_ptr->menu_perms[20] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_25_COL,"%c",up_ptr->menu_perms[20]);
					break;
				case F_M_PERM_26 :
					up_ptr->menu_perms[21] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_26_COL,"%c",up_ptr->menu_perms[21]);
					break;
				case F_M_PERM_27 :
					up_ptr->menu_perms[22] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_27_COL,"%c",up_ptr->menu_perms[22]);
					break;
				case F_M_PERM_28 :
					up_ptr->menu_perms[23] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_28_COL,"%c",up_ptr->menu_perms[23]);
					break;
				case F_M_PERM_29 :
					up_ptr->menu_perms[24] = c;
					mvprintw(D_M_PERM_1_ROW,D_M_PERM_29_COL,"%c",up_ptr->menu_perms[24]);
					break;
				case F_M_PERM_31 :
					up_ptr->menu_perms[25] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_31_COL,"%c",up_ptr->menu_perms[25]);
					break;
				case F_M_PERM_32 :
					up_ptr->menu_perms[26] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_32_COL,"%c",up_ptr->menu_perms[26]);
					break;
				case F_M_PERM_33 :
					up_ptr->menu_perms[27] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_33_COL,"%c",up_ptr->menu_perms[27]);
					break;
				case F_M_PERM_34 :
					up_ptr->menu_perms[28] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_34_COL,"%c",up_ptr->menu_perms[28]);
					break;
				case F_M_PERM_35 :
					up_ptr->menu_perms[29] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_35_COL,"%c",up_ptr->menu_perms[29]);
					break;
				case F_M_PERM_36 :
					up_ptr->menu_perms[30] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_36_COL,"%c",up_ptr->menu_perms[30]);
					break;
				case F_M_PERM_37 :
					up_ptr->menu_perms[31] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_37_COL,"%c",up_ptr->menu_perms[31]);
					break;
				case F_M_PERM_38 :
					up_ptr->menu_perms[32] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_38_COL,"%c",up_ptr->menu_perms[32]);
					break;
				case F_M_PERM_39 :
					up_ptr->menu_perms[33] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_39_COL,"%c",up_ptr->menu_perms[33]);
					break;
				case F_M_PERM_61 :
					up_ptr->menu_perms[34] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_61_COL,"%c",up_ptr->menu_perms[34]);
					break;
				case F_M_PERM_62 :
					up_ptr->menu_perms[35] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_62_COL,"%c",up_ptr->menu_perms[35]);
					break;
				case F_M_PERM_63 :
					up_ptr->menu_perms[36] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_63_COL,"%c",up_ptr->menu_perms[36]);
					break;
				case F_M_PERM_64 :
					up_ptr->menu_perms[37] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_64_COL,"%c",up_ptr->menu_perms[37]);
					break;
				case F_M_PERM_65 :
					up_ptr->menu_perms[38] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_65_COL,"%c",up_ptr->menu_perms[38]);
					break;
				case F_M_PERM_66 :
					up_ptr->menu_perms[39] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_66_COL,"%c",up_ptr->menu_perms[39]);
					break;
				case F_M_PERM_67 :
					up_ptr->menu_perms[40] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_67_COL,"%c",up_ptr->menu_perms[40]);
					break;
				case F_M_PERM_68 :
					up_ptr->menu_perms[41] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_68_COL,"%c",up_ptr->menu_perms[41]);
					break;
				case F_M_PERM_69 :
					up_ptr->menu_perms[42] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_69_COL,"%c",up_ptr->menu_perms[42]);
					break;
				case F_M_PERM_281 :
					up_ptr->menu_perms[43] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_281_COL,"%c",up_ptr->menu_perms[43]);
					break;
				case F_M_PERM_282 :
					up_ptr->menu_perms[44] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_282_COL,"%c",up_ptr->menu_perms[44]);
					break;
				case F_M_PERM_283 :
					up_ptr->menu_perms[45] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_283_COL,"%c",up_ptr->menu_perms[45]);
					break;
				case F_M_PERM_284 :
					up_ptr->menu_perms[46] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_284_COL,"%c",up_ptr->menu_perms[46]);
					break;
				case F_M_PERM_285 :
					up_ptr->menu_perms[47] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_285_COL,"%c",up_ptr->menu_perms[47]);
					break;
				case F_M_PERM_286 :
					up_ptr->menu_perms[48] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_286_COL,"%c",up_ptr->menu_perms[48]);
					break;
				case F_M_PERM_287 :
					up_ptr->menu_perms[49] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_287_COL,"%c",up_ptr->menu_perms[49]);
					break;
				case F_M_PERM_288 :
					up_ptr->menu_perms[50] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_288_COL,"%c",up_ptr->menu_perms[50]);
					break;
				case F_M_PERM_289 :
					up_ptr->menu_perms[51] = c;
					mvprintw(D_M_PERM_2_ROW,D_M_PERM_289_COL,"%c",up_ptr->menu_perms[51]);
					break;
				case F_CN_ZONE_CLS :
					up_ptr->can_zone_calls = c;
					mvprintw(D_DF_FLEET_ROW,D_CN_ZN_CLS_COL,"%c",up_ptr->can_zone_calls);
					break;
				case F_EX_GP_1 :
					up_ptr->group_1 = c;
					mvprintw(D_GROUPS_ROW,D_GRP_1_COL,"%c",up_ptr->group_1);
					break;
				case F_EX_GP_2 :
					up_ptr->group_2 = c;
					mvprintw(D_GROUPS_ROW,D_GRP_2_COL,"%c",up_ptr->group_2);
					break;
				case F_EX_GP_3 :
					up_ptr->group_3 = c;
					mvprintw(D_GROUPS_ROW,D_GRP_3_COL,"%c",up_ptr->group_3);
					break;
				case F_EX_GP_4 :
					up_ptr->group_4 = c;
					mvprintw(D_GROUPS_ROW,D_GRP_4_COL,"%c",up_ptr->group_4);
					break;
				case F_EX_GP_5 :
					up_ptr->group_5 = c;
					mvprintw(D_GROUPS_ROW,D_GRP_5_COL,"%c",up_ptr->group_5);
					break;
				case F_EX_GP_6 :
					up_ptr->group_6 = c;
					mvprintw(D_GROUPS_ROW,D_GRP_6_COL,"%c",up_ptr->group_6);
					break;
				case F_EX_GP_7 :
					up_ptr->group_7 = c;
					mvprintw(D_GROUPS_ROW,D_GRP_7_COL,"%c",up_ptr->group_7);
					break;
				case F_EX_GP_8 :
					up_ptr->group_8 = c;
					mvprintw(D_GROUPS_ROW,D_GRP_8_COL,"%c",up_ptr->group_8);
					break;
				case F_EX_GP_9 :
					up_ptr->group_9 = c;
					mvprintw(D_GROUPS_ROW,D_GRP_9_COL,"%c",up_ptr->group_9);
					break;
				case F_EX_GP_10 :
					up_ptr->group_10 = c;
					mvprintw(D_GROUPS_ROW,D_GRP_10_COL,"%c",up_ptr->group_10);
					break;
				case F_EX_GP_11 :
					up_ptr->group_11 = c;
					mvprintw(D_GROUPS_ROW,D_GRP_11_COL,"%c",up_ptr->group_11);
					break;
				case F_EX_GP_12 :
					up_ptr->group_12 = c;
					mvprintw(D_GROUPS_ROW,D_GRP_12_COL,"%c",up_ptr->group_12);
					break;
				case F_EX_TYP_1 :
					up_ptr->type_1 = c;
					mvprintw(D_TYPE_ROW,D_TYP_1_COL,"%c",up_ptr->type_1);
					break;
				case F_EX_TYP_2 :
					up_ptr->type_2 = c;
					mvprintw(D_TYPE_ROW,D_TYP_2_COL,"%c",up_ptr->type_2);
					break;
				case F_EX_TYP_3 :
					up_ptr->type_3 = c;
					mvprintw(D_TYPE_ROW,D_TYP_3_COL,"%c",up_ptr->type_3);
					break;
				case F_EX_TYP_4 :
					up_ptr->type_4 = c;
					mvprintw(D_TYPE_ROW,D_TYP_4_COL,"%c",up_ptr->type_4);
					break;
				case F_EX_TYP_5 :
					up_ptr->type_5 = c;
					mvprintw(D_TYPE_ROW,D_TYP_5_COL,"%c",up_ptr->type_5);
					break;
				case F_EX_TYP_6 :
					up_ptr->type_6 = c;
					mvprintw(D_TYPE_ROW,D_TYP_6_COL,"%c",up_ptr->type_6);
					break;
				case F_EX_TYP_7 :
					up_ptr->type_7 = c;
					mvprintw(D_TYPE_ROW,D_TYP_7_COL,"%c",up_ptr->type_7);
					break;
				case F_EX_TYP_8 :
					up_ptr->type_8 = c;
					mvprintw(D_TYPE_ROW,D_TYP_8_COL,"%c",up_ptr->type_8);
					break;
				case F_EX_TYP_9 :
					up_ptr->type_9 = c;
					mvprintw(D_TYPE_ROW,D_TYP_9_COL,"%c",up_ptr->type_9);
					break;
				case F_EX_TYP_10 :
					up_ptr->type_10 = c;
					mvprintw(D_TYPE_ROW,D_TYP_10_COL,"%c",up_ptr->type_10);
					break;
				case F_EX_TYP_11 :
					up_ptr->type_11 = c;
					mvprintw(D_TYPE_ROW,D_TYP_11_COL,"%c",up_ptr->type_11);
					break;
				case F_EX_TYP_12 :
					up_ptr->type_12 = c;
					mvprintw(D_TYPE_ROW,D_TYP_12_COL,"%c",up_ptr->type_12);
					break;
				case F_EX_TYP_13 :
					up_ptr->type_13 = c;
					mvprintw(D_TYPE_ROW,D_TYP_13_COL,"%c",up_ptr->type_13);
					break;
				case F_EX_TYP_14 :
					up_ptr->type_14 = c;
					mvprintw(D_TYPE_ROW,D_TYP_14_COL,"%c",up_ptr->type_14);
					break;
				case F_EX_TYP_15 :
					up_ptr->type_15 = c;
					mvprintw(D_TYPE_ROW,D_TYP_15_COL,"%c",up_ptr->type_15);
					break;
				case F_EX_TYP_16 :
					up_ptr->type_16 = c;
					mvprintw(D_TYPE_ROW,D_TYP_16_COL,"%c",up_ptr->type_16);
					break;
				case F_EX_TYP_17 :
					up_ptr->type_17 = c;
					mvprintw(D_TYPE_ROW,D_TYP_17_COL,"%c",up_ptr->type_17);
					break;
				case F_EX_TYP_18 :
					up_ptr->type_18 = c;
					mvprintw(D_TYPE_ROW,D_TYP_18_COL,"%c",up_ptr->type_18);
					break;
				case F_EX_TYP_19 :
					up_ptr->type_19 = c;
					mvprintw(D_TYPE_ROW,D_TYP_19_COL,"%c",up_ptr->type_19);
					break;
				case F_EX_TYP_20 :
					up_ptr->type_20 = c;
					mvprintw(D_TYPE_ROW,D_TYP_20_COL,"%c",up_ptr->type_20);
					break;
				case F_EX_TYP_21 :
					up_ptr->type_21 = c;
					mvprintw(D_TYPE_ROW,D_TYP_21_COL,"%c",up_ptr->type_21);
					break;
				case F_EX_TYP_22 :
					up_ptr->type_22 = c;
					mvprintw(D_TYPE_ROW,D_TYP_22_COL,"%c",up_ptr->type_22);
					break;
				case F_EX_TYP_23 :
					up_ptr->type_23 = c;
					mvprintw(D_TYPE_ROW,D_TYP_23_COL,"%c",up_ptr->type_23);
					break;
				case F_EX_TYP_24 :
					up_ptr->type_24 = c;
					mvprintw(D_TYPE_ROW,D_TYP_24_COL,"%c",up_ptr->type_24);
					break;
				case F_EX_TYP_25 :
					up_ptr->type_25 = c;
					mvprintw(D_TYPE_ROW,D_TYP_25_COL,"%c",up_ptr->type_25);
					break;
				case F_EX_TYP_26 :
					up_ptr->type_26 = c;
					mvprintw(D_TYPE_ROW,D_TYP_26_COL,"%c",up_ptr->type_26);
					break;
				case F_EX_TYP_27 :
					up_ptr->type_27 = c;
					mvprintw(D_TYPE_ROW,D_TYP_27_COL,"%c",up_ptr->type_27);
					break;
				case F_EX_TYP_28 :
					up_ptr->type_28 = c;
					mvprintw(D_TYPE_ROW,D_TYP_28_COL,"%c",up_ptr->type_28);
					break;
				case F_EX_TYP_29 :
					up_ptr->type_29 = c;
					mvprintw(D_TYPE_ROW,D_TYP_29_COL,"%c",up_ptr->type_29);
					break;
				case F_EX_TYP_30 :
					up_ptr->type_30 = c;
					mvprintw(D_TYPE_ROW,D_TYP_30_COL,"%c",up_ptr->type_30);
					break;
				case F_EX_TYP_31 :
					up_ptr->type_31 = c;
					mvprintw(D_TYPE_ROW,D_TYP_31_COL,"%c",up_ptr->type_31);
					break;
				case F_EX_TYP_32 :
					up_ptr->type_32 = c;
					mvprintw(D_TYPE_ROW,D_TYP_32_COL,"%c",up_ptr->type_32);
					break;
				      case F_AUTO_CALL_MSG:
					up_ptr->auto_free_text = c;
					mvprintw( D_AUTO_CALL_MSG_ROW, D_AUTO_CALL_MSG_COL, "%c", up_ptr->auto_free_text );
					break;
#ifdef NAMERICA
				      case F_HIA_1:
					up_ptr->hia_1 = c;
					mvprintw(D_HIA_ROW,D_HIA_1_COL, "%c", up_ptr->hia_1);
					break;

					
				      case F_HIA_2:
					up_ptr->hia_2 = c;
					mvprintw(D_HIA_ROW,D_HIA_2_COL, "%c", up_ptr->hia_2);
					break;
				      case F_HIA_3:
					up_ptr->hia_3 = c;
					  mvprintw(D_HIA_ROW,D_HIA_3_COL, "%c", up_ptr->hia_3);
					break;
				      case F_HIA_4:
					up_ptr->hia_4 = c;
					  mvprintw(D_HIA_ROW,D_HIA_4_COL, "%c", up_ptr->hia_4);
					break;
				      case F_HIA_5:
					up_ptr->hia_5 = c;
					  mvprintw(D_HIA_ROW,D_HIA_5_COL, "%c", up_ptr->hia_5);
					break;
				      case F_HIA_6:
					up_ptr->hia_6 = c;
					  mvprintw(D_HIA_ROW,D_HIA_6_COL, "%c", up_ptr->hia_6);
					break;
				      case F_HIA_7:
					up_ptr->hia_7 = c;
					  mvprintw(D_HIA_ROW,D_HIA_7_COL, "%c", up_ptr->hia_7);
					break;
				      case F_HIA_8:
					up_ptr->hia_8 = c;
					  mvprintw(D_HIA_ROW,D_HIA_8_COL, "%c", up_ptr->hia_8);
					break;
				      case F_HIA_9:
					up_ptr->hia_9 = c;
					  mvprintw(D_HIA_ROW,D_HIA_9_COL, "%c", up_ptr->hia_9);
					break;
				      case F_HIA_10:
					up_ptr->hia_10 = c;
					  mvprintw(D_HIA_ROW,D_HIA_10_COL, "%c", up_ptr->hia_10);
					break;
				      case F_HIA_11:
					up_ptr->hia_11 = c;
					  mvprintw(D_HIA_ROW,D_HIA_11_COL, "%c", up_ptr->hia_11);
					break;
				      case F_HIA_12:
					up_ptr->hia_12 = c;
					  mvprintw(D_HIA_ROW,D_HIA_12_COL, "%c", up_ptr->hia_12);
					break;
				      case F_HIA_13:
					up_ptr->hia_13 = c;
					  mvprintw(D_HIA_ROW,D_HIA_13_COL, "%c", up_ptr->hia_13);
					break;
				      case F_HIA_14:
					up_ptr->hia_14 = c;
					  mvprintw(D_HIA_ROW,D_HIA_14_COL, "%c", up_ptr->hia_14);
					break;
				      case F_HIA_15:
					up_ptr->hia_15 = c;
					  mvprintw(D_HIA_ROW,D_HIA_15_COL, "%c", up_ptr->hia_15);
					break;
				      case F_HIA_16:
					up_ptr->hia_16 = c;
					  mvprintw(D_HIA_ROW,D_HIA_16_COL, "%c", up_ptr->hia_16);
					break;
				      case F_HIA_17:
					up_ptr->hia_17 = c;
					  mvprintw(D_HIA_ROW,D_HIA_17_COL, "%c", up_ptr->hia_17);
					break;
				      case F_HIA_18:
					up_ptr->hia_18 = c;
					  mvprintw(D_HIA_ROW,D_HIA_18_COL, "%c", up_ptr->hia_18);
					break;
#endif
				       
				      }
			refresh();
			update_piu();
#ifdef DEBUG
		sprintf(trace_str,"Leaving check_y_n - 1\n");
		TRACE(trace_str);
#endif
			return(VALID);
		}
		prt_error(ERROR_N_FLD);
		err_n_fld = 1;
#ifdef DEBUG
		sprintf(trace_str,"Leaving check_y_n - 2\n");
		TRACE(trace_str);
#endif
		return(NOT_VALID);
	}
	if(!err_n_fld)
		update_piu();
#ifdef DEBUG
		sprintf(trace_str,"Leaving check_y_n - 3\n");
		TRACE(trace_str);
#endif
	return(VALID);

}  /* End of check_y_n()  */


/*****************************************************************************
* check_fleet - check fleet.
******************************************************************************/

int check_fleet (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	struct cisam_fl *fl_ptr;
	struct cisam_up up_buf;
	char tmp_fleet;

#ifdef DEBUG
	sprintf(trace_str,"Entering check_fleet\n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		if(!isalpha(read_buf[0])) {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
			fl_flags = 0;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_fleet - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		/* Save the user profile information */

		memcpy((char *)&up_buf,req_buf,sizeof(struct cisam_up));

		fl_ptr = (struct cisam_fl *) req_buf;

		if((db(FLEET_FILE_ID,OPEN,&fl_key1,ISINOUT+ISMANULOCK,0)) < 0)  { 
			prt_error(FLEET_OPEN_ERR);
			sleep(3);
			prt_cisam_err(iserrno,FLEET_OPEN_ERR);
			ERROR(' ',FLEET_FILE,FLEET_OPEN_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_fleet - 2\n");
	TRACE(trace_str);
#endif
			cleanup();
		}
		fl_ptr->id = Toupper(read_buf[0]);
		if((db(FLEET_FILE_ID,READ_KEYS,&fl_key1,ISEQUAL,0)) < 0)  {
			if(iserrno == ENOREC)  {
				prt_error(ERROR_N_FLD);
				err_n_fld = 1;
				fl_flags = 0;
				memcpy(req_buf,(char *)&up_buf,sizeof(struct cisam_up));
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_fleet - 2\n");
	TRACE(trace_str);
#endif
				return(NOT_VALID);
			}
			else  {
				prt_error(READ_FLEET_ERR);
				sleep(3);
				prt_cisam_err(iserrno,READ_FLEET_ERR);
				ERROR(' ',FLEET_FILE,READ_FLEET_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_fleet - 3\n");
	TRACE(trace_str);
#endif
				cleanup();
			}
		}
		if((db(FLEET_FILE_ID,CLOSE,&fl_key1,0,0)) < 0)  {
			prt_error(FLEET_CLOSE_ERR);
			prt_cisam_err(iserrno,FLEET_CLOSE_ERR);
			ERROR(' ',FLEET_FILE,FLEET_CLOSE_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_fleet - 4\n");
	TRACE(trace_str);
#endif
			cleanup();
		}
		tmp_fleet = fl_ptr->id;

		/* Restore the user profile information */

		memcpy(req_buf,(char *)&up_buf,sizeof(struct cisam_up));
		ipc_required = 1;
		clear_err_line();
		err_n_fld = 0;
		fl_flags = 1;
		up_ptr->default_fleet = Toupper(tmp_fleet);
		mvprintw(D_DF_FLEET_ROW,D_DF_FLEET_COL,"%c",up_ptr->default_fleet);
		refresh();
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_fleet - 5\n");
	TRACE(trace_str);
#endif
		return(VALID);
	}
	else	{
		if (!fl_flags)	{
			prt_error(ERROR_N_FLD);
			return(NOT_VALID);
		}
	}
	if(!err_n_fld)
		update_piu();

#ifdef DEBUG
	sprintf(trace_str,"Leaving check_fleet - 6\n");
	TRACE(trace_str);
#endif
	return(VALID);

}  /* End of check_fleet()  */

/******************************************************************************
* check_screen - check the default screen.
******************************************************************************/

int check_screen (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int screen;			/* screen number input by the user */

#ifdef DEBUG
	sprintf(trace_str,"Entering check_screen\n");
	TRACE(trace_str);
#endif

	if(*field_entered)  {
		clear_err_line();

		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_screen - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}

		screen = atoi(read_buf);

		if(((0 <= screen) && (screen <= 9)) ||
	   	   ((11 <= screen) && (screen <= 19)) ||
	   	   ((21 <= screen) && (screen <= 29)) ||
	   	   ((31 <= screen) && (screen <= 39)) ||
	   	   ((61 <= screen) && (screen <= 69)) ||
	   	   ((281 <= screen) && (screen <= 289)))  {
			ipc_required = 1;
			clear_err_line();
			err_n_fld = 0;
			up_ptr->default_screen = (short)screen;  /* move command number to
						   	      isam buffer  */
			mvprintw(D_DF_FLEET_ROW,D_DF_SCR_COL,"%3d",up_ptr->default_screen);
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_screen - 2\n");
	TRACE(trace_str);
#endif

			return(VALID);
			}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_screen - 3\n");
	TRACE(trace_str);
#endif

			return(NOT_VALID);
		}
	}
	else  {
		if(!err_n_fld)
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_screen - 4\n");
	TRACE(trace_str);
#endif
		return(VALID);
	}

}  /* End of check_screen() */

/*****************************************************************************
* check_search - check the default search.
*****************************************************************************/

int check_search (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{

	int search;			/* search number input by the user */

#ifdef DEBUG
	sprintf(trace_str,"Entering check_search\n");
	TRACE(trace_str);
#endif

	if(*field_entered)  {
		clear_err_line();

		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_search - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}

		search = atoi(read_buf);

		if(((21 <= search) && (search <= 27)) ||
	   	   ((281 <= search) && (search <= 283)))  {
			ipc_required = 1;
			clear_err_line();
			err_n_fld = 0;
			up_ptr->default_search = (short)search;  /* move command number to
						   	      isam buffer  */
			mvprintw(D_DF_FLEET_ROW,D_DF_SRCH_COL,"%3d",up_ptr->default_search);
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_search - 2\n");
	TRACE(trace_str);
#endif

			return(VALID);
			}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_search - 3\n");
	TRACE(trace_str);
#endif

			return(NOT_VALID);
		}
	}
	else  {
		if(!err_n_fld)
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_search - 4\n");
	TRACE(trace_str);
#endif
		return(VALID);
	}

}  /* End of check_search()  */

/****************************************************************************
* check_position - check default user position.
*****************************************************************************/

int check_position (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int pos;		/* user position entered by the user */

#ifdef DEBUG
	sprintf(trace_str,"Entering check_position\n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();

		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_position - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		ipc_required = 1;
		err_n_fld = 0;
		pos = atoi(read_buf);	
		up_ptr->default_pos = (short) pos;	/* move position to isam buffer */
		mvprintw(D_DF_FLEET_ROW,D_DF_POS_COL,"%3d",up_ptr->default_pos);
		refresh();
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_position - 2\n");
	TRACE(trace_str);
#endif
		return(VALID);
	}
	if(!err_n_fld)
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_postion - 3\n");
	TRACE(trace_str);
#endif
	return(VALID);

}  /* End of check_position()  */

/****************************************************************************
* check_cl_type - check default call type.
****************************************************************************/

int check_cl_type (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
    char c;

#ifdef DEBUG
	sprintf(trace_str,"Entering check_cl_type\n");
	TRACE(trace_str);
#endif
    c = Toupper(read_buf[0]);

	if(*field_entered)  {
		clear_err_line();

		if (c == PASS || c == DELIV || c ==TYPEX) {
			ipc_required = 1;
			err_n_fld = 0;
			up_ptr->default_call_tp = c;
			mvprintw(D_DF_FLEET_ROW,D_DF_CL_TYP_COL,"%c",up_ptr->default_call_tp);
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_cl_type - 1\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
		else  {
			clear_err_line();
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_cl_type - 2\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
	}
	if(!err_n_fld)
		update_piu();

#ifdef DEBUG
	sprintf(trace_str,"Leaving check_cl_type - 3\n");
	TRACE(trace_str);
#endif
	return(VALID);
}  /* End of check_cl_type()  */

/*****************************************************************************
* check_city - check city.
******************************************************************************/

int check_city (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	struct cisam_lo *lo_ptr;
	struct cisam_up up_buf;
	char tmp_city[4];

#ifdef DEBUG
	sprintf(trace_str,"Entering check_city\n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();

		/* save the user profile information */

		memcpy((char *) &up_buf,req_buf,sizeof(struct cisam_up));

		lo_ptr = (struct cisam_lo *) req_buf;
		if((db(LOCALTY_FILE_ID,OPEN,&lo_key1,ISINOUT+ISMANULOCK,0)) < 0)  {
			prt_error(LOCALITY_OPEN_ERR);
			sleep(3);
			prt_cisam_err(iserrno,LOCALITY_OPEN_ERR);
			ERROR(' ',LOCALTY_FILE,LOCALITY_OPEN_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_city - 2\n");
	TRACE(trace_str);
#endif
			cleanup();
		}
		strcpy(lo_ptr->code,str_to_upper(read_buf));
		strcpy(tmp_city,"");
		if (strcmp(lo_ptr->code," "))
		{
		if((db(LOCALTY_FILE_ID,READ_KEYS,&lo_key1,ISEQUAL,0)) < 0)  {
			if(iserrno == ENOREC)  {
				prt_error(ERROR_N_FLD);
				err_n_fld = 1;
				memcpy(req_buf,(char *)&up_buf,sizeof(struct cisam_up));
				prt_error(ERROR_N_FLD);
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_city - 3\n");
	TRACE(trace_str);
#endif
				return(NOT_VALID);
			}
			else  {
				prt_error(READ_LOCALITY_ERR);
				sleep(3);
				prt_cisam_err(iserrno,READ_LOCALITY_ERR);
				ERROR(' ',LOCALTY_FILE,READ_LOCALITY_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_city - 4\n");
	TRACE(trace_str);
#endif
				cleanup();
			}
		}
		if((db(LOCALTY_FILE_ID,CLOSE,&lo_key1,0,0)) <0)  {
			prt_error(LOCALITY_CLOSE_ERR);
			prt_cisam_err(iserrno,LOCALITY_CLOSE_ERR);
			ERROR(' ',LOCALTY_FILE,LOCALITY_CLOSE_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_city - 5\n");
	TRACE(trace_str);
#endif
			cleanup();
		}
		strcpy(tmp_city,lo_ptr->act_city_code);
		}
		memcpy(req_buf,(char *) &up_buf,sizeof(struct cisam_up));
		strcpy(up_ptr->default_city,tmp_city);
		ipc_required = 1;
		clear_err_line();
		city_flags = 1;
		err_n_fld = 0;
		mvprintw(D_DF_FLEET_ROW,D_DF_CITY_COL,"%s",up_ptr->default_city);
		refresh();
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_city - 6\n");
	TRACE(trace_str);
#endif
		return(VALID);
	}
	else	{
		if (doing_add)	{
			if (!city_flags)	{
				prt_error(ERROR_N_FLD);
				return(NOT_VALID);
			}
		}
		if(!err_n_fld)
			update_piu();
	}

#ifdef DEBUG
	sprintf(trace_str,"Leaving check_city - 7\n");
	TRACE(trace_str);
#endif
	return(VALID);

}  /* End of check_city()  */

/*****************************************************************************
* check_hi_pri - check the highest priority.
******************************************************************************/

int check_hi_pri (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int hi_pri;		/* highest priority entered by the user */

#ifdef DEBUG
	sprintf(trace_str,"Entering check_hi_pri\n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		 
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_hi_pri - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		hi_pri = atoi(read_buf);
		if((1 <= hi_pri) && (hi_pri <= 63)) {
			ipc_required = 1;
			clear_err_line();
			err_n_fld = 0;
			up_ptr->high_call_pri = (short) hi_pri;
			mvprintw(D_DF_FLEET_ROW,D_DF_HI_CL_PRI_COL,"%2d",up_ptr->high_call_pri);
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_hi_pri - 2\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_hi_pri - 3\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
	}
	if(!err_n_fld)
		update_piu();

#ifdef DEBUG
	sprintf(trace_str,"Leaving check_hi_pri - 4\n");
	TRACE(trace_str);
#endif
	return(VALID);

}  /* End of check_hi_pri()  */

/****************************************************************************
* check_lg_file - check logo file.
****************************************************************************/

int check_lg_file (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering check_lg_file\n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		 
		if(valid_path(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_lg_file - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		clear_err_line();
		err_n_fld = 0;
		ipc_required = 1;
		strcpy(up_ptr->logo_file,read_buf);
		mvprintw(D_DF_FLEET_ROW,D_LG_FILE_COL,"%s",up_ptr->logo_file);
		refresh();
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_lg_file - 2\n");
	TRACE(trace_str);
#endif
		return(VALID);
	}
	if(!err_n_fld)
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_lg_file - 3\n");
	TRACE(trace_str);
#endif
	return(VALID);

} /* End of check_lg_file()  */

/****************************************************************************
* check_spr_zone - check super zone.
***************************************************************************/

int check_spr_zone (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	struct cisam_zn *zn_ptr;
	struct cisam_up up_buf;
	int spz;		/* super zone entered by the user */
	char tmp_fleet;

#ifdef DEBUG
	sprintf(trace_str,"Entering check_spr_zone\n");
	TRACE(trace_str);
#endif

	if(*field_entered)  {
		clear_err_line();
		 
		spz = atoi(read_buf);
		if((0 <= spz) && (spz <= 32767))  {
			if(spz)  {
				tmp_fleet = up_ptr->default_fleet;

				/* Save the user profile information */
	
				memcpy((char *)&up_buf,req_buf,sizeof(struct cisam_up));

				zn_ptr = (struct cisam_zn *) req_buf;

				zn_ptr->fleet = tmp_fleet;
				zn_ptr->nbr = spz;
				if((db(ZONE_FILE_ID,READ_KEYS,&zn_key1,ISEQUAL,0)) < 0)  {
					if(iserrno == ENOREC)  {
						prt_error(ERROR_N_FLD);
						err_n_fld = 1;
						memcpy(req_buf,(char *)&up_buf,sizeof(struct cisam_up));
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_spr_zone - 2\n");
	TRACE(trace_str);
#endif
						return(NOT_VALID);
					}
					else  {
						prt_error(READ_ZONE_ERR);
						sleep(3);
						prt_cisam_err(iserrno,READ_ZONE_ERR);
						ERROR(' ',ZONE_FILE,READ_ZONE_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_spr_zone - 3\n");
	TRACE(trace_str);
#endif
						cleanup();
					}
				}
				if(zn_ptr->type != '3')  {
					prt_error(ERROR_N_FLD);
					err_n_fld = 1;
					memcpy(req_buf,(char *)&up_buf,sizeof(struct cisam_up));
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_spr_zone - 4\n");
	TRACE(trace_str);
#endif
					return(NOT_VALID);
				}
				memcpy(req_buf,(char *)&up_buf,sizeof(struct cisam_up));
			}  /* End of if(spz)  */

			ipc_required = 1;
			err_n_fld = 0;
			switch(piu)  {
				case F_SPZ_1 :
					up_ptr->superzone_1 = (short) spz;	/* move user id into isam buffer */
					mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_1_COL,"%5d",up_ptr->superzone_1);
					break;
				case F_SPZ_2 :
					up_ptr->superzone_2 = (short) spz;	/* move user id into isam buffer */
					mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_2_COL,"%5d",up_ptr->superzone_2);
					break;
				case F_SPZ_3 :
					up_ptr->superzone_3 = (short) spz;	/* move user id into isam buffer */
					mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_3_COL,"%5d",up_ptr->superzone_3);
					break;
				case F_SPZ_4 :
					up_ptr->superzone_4 = (short) spz;	/* move user id into isam buffer */
					mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_4_COL,"%5d",up_ptr->superzone_4);
					break;
				case F_SPZ_5 :
					up_ptr->superzone_5 = (short) spz;	/* move user id into isam buffer */
					mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_5_COL,"%5d",up_ptr->superzone_5);
					break;
				case F_SPZ_6 :
					up_ptr->superzone_6 = (short) spz;	/* move user id into isam buffer */
					mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_6_COL,"%5d",up_ptr->superzone_6);
					break;
				case F_SPZ_7 :
					up_ptr->superzone_7 = (short) spz;	/* move user id into isam buffer */
					mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_7_COL,"%5d",up_ptr->superzone_7);
					break;
				case F_SPZ_8 :
					up_ptr->superzone_8 = (short) spz;	/* move user id into isam buffer */
					mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_8_COL,"%5d",up_ptr->superzone_8);
					break;
				case F_SPZ_9 :
					up_ptr->superzone_9 = (short) spz;	/* move user id into isam buffer */
					mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_9_COL,"%5d",up_ptr->superzone_9);
					break;
				case F_SPZ_10 :
					up_ptr->superzone_10 = (short) spz;	/* move user id into isam buffer */
					mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_10_COL,"%5d",up_ptr->superzone_10);
					break;
				case F_SPZ_11 :
					up_ptr->superzone_11 = (short) spz;	/* move user id into isam buffer */
					mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_11_COL,"%5d",up_ptr->superzone_11);
					break;
				case F_SPZ_12 :
					up_ptr->superzone_12 = (short) spz;	/* move user id into isam buffer */
					mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_12_COL,"%5d",up_ptr->superzone_12);
					break;
				case F_SPZ_13 :
					up_ptr->superzone_13 = (short) spz;	/* move user id into isam buffer */
					mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_13_COL,"%5d",up_ptr->superzone_13);
					break;
				case F_SPZ_14 :
					up_ptr->superzone_14 = (short) spz;	/* move user id into isam buffer */
					mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_14_COL,"%5d",up_ptr->superzone_14);
					break;
				case F_SPZ_15 :
					up_ptr->superzone_15 = (short) spz;	/* move user id into isam buffer */
					mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_15_COL,"%5d",up_ptr->superzone_15);
					break;
				case F_SPZ_16 :
					up_ptr->superzone_16 = (short) spz;	/* move user id into isam buffer */
					mvprintw(D_SPR_ZONES_ROW,D_S_ZONE_16_COL,"%5d",up_ptr->superzone_16);
					break;
			}
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_spr_zone - 5\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_spr_zone - 6\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
	}
	if(!err_n_fld)
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_spr_zone - 7\n");
	TRACE(trace_str);
#endif
	return(VALID);

}  /* End of check_spr_zone() */

/******************************************************************************
* update_piu - update prompt in use variable.
*******************************************************************************/

update_piu()
{
	if(key_val == '\t' ||
	   key_val == '\n' ||
	   key_val == '\r' ||
	   key_val == KEY_DOWN ||
	   key_val == VALID)	{
		if (piu == 18)
			piu = 21;
		if (!doing_add)	{
			 if(piu == NUM_FLDS)
				piu = 3;
			else
				piu += 1;
		}
		else	{
			 if(piu == NUM_FLDS)
				piu = 2;
			else
				piu += 1;
		}
		
		/* Skip default position field, no longer used */
		if (piu == F_DF_POS)
		  piu ++;
	}

	if(key_val == KEY_UP)	{
		if(piu == 22)
			piu = 19;
		if (!doing_add)	{
			if (piu == 3)
				piu = NUM_FLDS;
			else
				piu -= 1;
		}
		else	{
			 if(piu == 2)
				piu = NUM_FLDS;
			else
				piu -= 1;
		}

		/* Skip default position field, no longer used */
		if (piu == F_DF_POS)
		  piu --;
	}

} /* End of update_piu()  */

/****************************************************************************
* numeric - make sure the field entered is numeric.
****************************************************************************/

numeric(in_str)
char in_str[132];
{
	int i;		/* loop index */
	int validity;	/* = 0,1 for bad, good */

	i = 0;
	validity = 1;	/* assume the field is valid */

	while(in_str[i])
		if(!isdigit(in_str[i++]))
			validity = 0;

	return(validity);

} /* End of numeric()  */


/****************************************************************************
* alpha or space - make sure the field entered is alphabetic.
* add 910227 to allow city not to be required in profile for multi-fleet 
* installs
****************************************************************************/

alpha_or_space(in_str)
char *in_str;
{
	int i;		/* loop index */
	int validity;	/* = 0,1 for bad, good */

	i = 0;
	validity = 1;	/* assume the field is valid */

	while(*in_str)
	{
		if(!isprint(*in_str))
			validity = 0;
		in_str += 1;
	}
	return(validity);
}  /* End of alpha()  */

/****************************************************************************
* alpha - make sure the field entered is alphabetic.
****************************************************************************/

alpha(in_str)
char in_str[132];
{
	int i;		/* loop index */
	int validity;	/* = 0,1 for bad, good */

	i = 0;
	validity = 1;	/* assume the field is valid */

	while(in_str[i])
		if(!isalpha(in_str[i++]))
			validity = 0;
	return(validity);
}  /* End of alpha()  */

/****************************************************************************
* valid_path - make sure the field entered is alphabetic or a /.
****************************************************************************/

valid_path(in_str)
char in_str[132];
{
	int i;		/* loop index */
	int validity;	/* = 0,1 for bad, good */

	i = 0;
	validity = 1;	/* assume the field is valid */

	while(in_str[i])  {
		if((!isalpha(in_str[i])) || (in_str[i] == '/'))  {
			validity = 0;
			break;
		}
		i++;
	}

	return(validity);

}  /* End of valid_path()  */

/****************************************************************************
* str_to_upper - convert a character string to upper case.
****************************************************************************/

char *str_to_upper(in_buf)
char *in_buf;
{
    char *original_in_buf;

    original_in_buf = in_buf;

    while (*in_buf)	{
	*in_buf = Toupper(*in_buf);
	in_buf ++;
    }
    return(original_in_buf);
}

/****************************************************************************
* str_to_lower - convert a character string to lower case.
****************************************************************************/

char *str_to_lower(in_buf)
char *in_buf;
{
    char *original_in_buf;

    original_in_buf = in_buf;

    while (*in_buf)	{
	*in_buf = tolower(*in_buf);
	in_buf ++;
    }
    return(original_in_buf);
}
