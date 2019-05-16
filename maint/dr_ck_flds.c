static char sz__FILE__[]="@(#)dr_ck_flds.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  dr_ck_flds.c; Rel: 6.3.0.0; Get date: 11/4/92 at 09:34:54
* @(#)  Last delta: 10/28/90 at 10:15:56
* @(#)  SCCS File: /taxi/sccs/s.dr_ck_flds.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  dr_ck_flds.c; Rel: 5.4.0.0; Get date: 3/26/90 at 14:02:02
 * @(#)  Last delta: 3/23/90 at 15:56:29
 * @(#)  SCCS File: /taxi/sccs/s.dr_ck_flds.c
 *                                                                           *
 *****************************************************************************/

#include "comm_strdefs.h"
#include "dr_m_strdefs.h"
#include <pwd.h>
#include <curses.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include "mad_error.h"
#include "df_maint.h"
#include "ex_key.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "dr_maint.h"
#include "literals.h"
#include "path.h"
#include "tokenize.h"
#include "language.h"
#include "func_keys.h"
#include "scr.h"

extern short ipc_required;		/* Set if a variable which resides */
					/* shared memory changes */
extern struct cisam_dr *dr_ptr;		/* pointer to driver file structure */
extern struct cisam_cl *cl_ptr;		/* pointer to call file structure */
extern struct cisam_fl *fl_ptr;		
extern char read_buf[];			/* buffer for reading fields */
extern FILE *trace_fp;			/* trace file pointer */
char *str_to_upper();
extern char trace_str[];		/* trace string */
extern char req_buf[];			/* buffer for the db routine */
extern int piu;				/* prompt in use */
extern int key_val;			/* value of key entered */
extern int err_n_fld;			/* set if an error in field condition exists */
extern int doing_add;			/* performing add operation */

extern prt_error(), prt_cisam_err();
extern clear_err_line();
extern dt_tm();
extern fleet_flag,id_flag;
char a_or_p;			/* indicates whether the last character of the time field is an 'a' or 'p'  */
extern struct scr_flds dr_scr_flds[];
extern struct fleets *fleet[];

int update_piu();
int numeric();
long conversion();

int check_dummy(), check_fleet(), check_dr_id(), check_class();
int check_y_n(), check_date(), check_time(), check_susp_rsn();
int check_sp_msg_1(), check_anything(), check_date_str();
int check_cmnt(), check_lst_veh(), check_last_call(), check_short(), check_0to9();
int check_attr_select();
/*  declare an array of pointers to functions which check the 
 *  field entered for validity.
 */

int (*check_field[])() = {check_dummy, check_fleet, check_dr_id, check_class,
			  check_anything, check_anything, check_anything, 
			  check_anything, check_anything, check_anything, 
			  check_anything, check_anything,
			  check_y_n, check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n, check_y_n,
			  check_y_n, check_y_n, check_y_n, check_y_n,
			  check_attr_select,
			  check_y_n, check_date, check_time, check_date,
			  check_time, check_susp_rsn, 
			  check_y_n, check_date, check_time, check_date,
			  check_time,
			  check_y_n, check_date, check_time, check_date,
			  check_time, 
			  check_sp_msg_1,
			  check_cmnt, check_date_str,
			  check_date_str, check_0to9, check_date, 
			  check_time, check_date,
			  check_time, check_date,
			  check_time, check_date,
			  check_time, check_lst_veh, check_last_call,
			  check_short, check_short, check_short,
			  check_short, check_short, check_short,
			  check_short, check_short, check_short, 
			  check_short, check_short, check_short,
			  check_short, check_short, check_short,
			  check_short, check_short};

int check_dummy(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
{
}

/******************************************************************************
* check_fleet - check the fleet indicator.
******************************************************************************/

int check_fleet(piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	static struct cisam_dr *d_ptr;
	static struct cisam_dr d_buf;

	d_ptr = &d_buf;
#ifdef DEBUG
	sprintf(trace_str,"Entering check_fleet\n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		if(!isalpha(read_buf[0]))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_fleet - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		memcpy((char *)d_ptr,req_buf,sizeof(struct cisam_dr));
		fl_ptr->id = Toupper(read_buf[0]);
		if ((db(FLEET_FILE_ID,READ_KEYS,&fl_key1,ISEQUAL,0)) < 0)	{
			prt_error(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_1, "Invalid fleet ID"));
			return(NOT_VALID);
		}
		err_n_fld = 0;
		fleet_flag = 1;
		memcpy(req_buf,(char *)d_ptr,sizeof(struct cisam_dr));
		dr_ptr->fleet = Toupper(read_buf[0]);
		mvprintw(D_FLEET_ROW,D_FLEET_COL,"%c",dr_ptr->fleet);
		refresh();
		piu = 2;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_fleet - 2\n");
	TRACE(trace_str);
#endif 
		return(NOT_VALID);
	}
	else	{	/* not entered */
		clear_err_line();
		if ((!fleet_flag) && (key_val != ESC))	{
			prt_error(catgets(DR_MAINT_m_catd, DR_MAINT_SET, DR_MAINT_2, "You must enter value"));
			return(NOT_VALID);
		}
		if (fleet_flag)
			update_piu();
	}

} /* End of check_fleet()  */

/****************************************************************************
* check_class - check the driver classification.
****************************************************************************/

int check_class (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{

#ifdef DEBUG
	sprintf(trace_str,"Entering check_class \n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		if((read_buf[0] == '1')  ||
		   (read_buf[0] == '2'))  {
			err_n_fld = 0;
			dr_ptr->class = read_buf[0];
			mvprintw(D_FLEET_ROW,D_CLASS_COL,"%c",dr_ptr->class);
			ipc_required = 1;
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_class - 1\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_class - 2\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
	}
	else  {
		if(!err_n_fld)
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_class - 3\n");
	TRACE(trace_str);
#endif
		return(VALID);

	}
} /* End of check_class()  */

/****************************************************************************
* check_lst_veh - check the last vehicle number for this driver.
****************************************************************************/

int check_lst_veh (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int v_id;		/* temporary vehicle number */
#ifdef DEBUG
	sprintf(trace_str,"Entering check_lst_veh \n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_lst_veh - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		v_id = atoi(read_buf);
		if((1 <= v_id) && (v_id <= 999))  {
			err_n_fld = 0;
			dr_ptr->last_veh_nbr = (short) v_id;
			mvprintw(D_EMRG_ROW,D_LST_VEH_COL,"%4d",dr_ptr->last_veh_nbr);
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_lst_veh - 2\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_lst_veh - 3\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
	}
	else  {
		if(!err_n_fld)
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_lst_veh - 4\n");
	TRACE(trace_str);
#endif
		return(VALID);

	}
} /* End of check_lst_veh()  */

/****************************************************************************
* check_dr_id - check the driver number.
****************************************************************************/

int check_dr_id (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int dr_id;		/* temporary driver number */
#ifdef DEBUG
	sprintf(trace_str,"Entering check_dr_id \n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_dr_id - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		dr_id = atoi(read_buf);
		if((1 <= dr_id) && (dr_id <= 999999))  {
			err_n_fld = 0;
			dr_ptr->id = dr_id;
			id_flag = 1;
			mvprintw(D_FLEET_ROW,D_DR_ID_COL,"%6d",dr_ptr->id);
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_dr_id - 2\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
	}
	else  {
		clear_err_line();
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_dr_id - 4\n");
	TRACE(trace_str);
#endif
		return(VALID);

	}
} /* End of check_dr_id()  */

/****************************************************************************
* check_last_call - check the last call taken by this driver.
****************************************************************************/

int check_last_call (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	struct cisam_cl *cl_ptr;		/* pointer to call file structure */
	struct cisam_dr dr_buf;		/* temporary pointer to driver file structure  */
	int tmp_call_nbr;			/* temporary call number */

#ifdef DEBUG
	sprintf(trace_str,"Entering check_last_call \n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		tmp_call_nbr = atoi(read_buf);

		/* Save the driver file information  */

		memcpy((char *)&dr_buf,req_buf,sizeof(struct cisam_dr));

		cl_ptr = (struct cisam_cl *) req_buf;
		if((db(CALL_FILE_ID,OPEN,&cl_key1,ISINOUT+ISMANULOCK,0)) < 0)  {
			prt_error(CALL_OPEN_ERR);
			sleep(3);
			prt_cisam_err(iserrno,CALL_OPEN_ERR);
			ERROR(' ',CALL_FILE,CALL_OPEN_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_last_call - 1\n");
	TRACE(trace_str);
#endif
			cleanup();
		}
		cl_ptr->nbr = tmp_call_nbr;
		if((db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL,0)) < 0)  {
			if(iserrno == ENOREC)  {
				prt_error(ERROR_N_FLD);
				err_n_fld = 1;
				memcpy(req_buf,(char *)&dr_buf,sizeof(struct cisam_dr));
				prt_error(ERROR_N_FLD);
				err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_last_call - 2\n");
	TRACE(trace_str);
#endif
				return(NOT_VALID);
			}
			else  {
				prt_error(READ_CALL_ERR);
				sleep(3);
				prt_cisam_err(iserrno,READ_CALL_ERR);
				ERROR(' ',CALL_FILE,READ_CALL_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_last_call - 3\n");
	TRACE(trace_str);
#endif
				cleanup();
			}
		}
		if((db(CALL_FILE_ID,CLOSE,&cl_key1,0,0)) < 0)  {
			prt_error(CALL_CLOSE_ERR);
			prt_cisam_err(iserrno,CALL_CLOSE_ERR);
			ERROR(' ',CALL_FILE,CALL_CLOSE_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_last_call - 4\n");
	TRACE(trace_str);
#endif
			cleanup();
		}
		memcpy(req_buf,(char *)&dr_buf,sizeof(struct cisam_dr));
		ipc_required = 1;
		dr_ptr->last_call = tmp_call_nbr;
		err_n_fld = 0;
		mvprintw(D_EMRG_ROW,D_LST_CALL_COL,"%18d",dr_ptr->last_call);
		refresh();
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_last_call - 5\n");
	TRACE(trace_str);
#endif
		return(VALID);
	}
	else  {
		if(!err_n_fld)
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_last_call - 6\n");
	TRACE(trace_str);
#endif
		return(VALID);

	}
} /* End of check_last_call()  */

/*****************************************************************************
* check_anything - check fields where anything is valid.
*****************************************************************************/

int check_anything (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering check_anything\n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		err_n_fld = 0;
		switch(piu)  {
			case F_NAME :
				strcpy(dr_ptr->name,str_to_upper(read_buf));
				mvprintw(D_FLEET_ROW,D_NAME_COL,"%s",dr_ptr->name);
				break;
			case F_ADDRESS :
				strcpy(dr_ptr->address,str_to_upper(read_buf));
				mvprintw(D_FLEET_ROW,D_ADD_COL,"%s",dr_ptr->address);
				break;
			case F_CITY :
				strcpy(dr_ptr->city,str_to_upper(read_buf));
				mvprintw(D_FLEET_ROW,D_CITY_COL,"%s",dr_ptr->city);
				break;
			case F_ZIP :
				strcpy(dr_ptr->zip,str_to_upper(read_buf));
				mvprintw(D_FLEET_ROW,D_ZIP_COL,"%s",dr_ptr->zip);
				break;
			case F_Z_SFX :
				strcpy(dr_ptr->zip_suf,str_to_upper(read_buf));
				mvprintw(D_FLEET_ROW,D_ZSFX_COL,"%s",dr_ptr->zip_suf);
				break;
			case F_PHONE :
				strcpy(dr_ptr->phone,str_to_upper(read_buf));
				mvprintw(D_PHONE_ROW,D_PHONE_COL,"%s",dr_ptr->phone);
				break;
			case F_LIC :
				strcpy(dr_ptr->license_nbr,str_to_upper(read_buf));
				mvprintw(D_PHONE_ROW,D_LIC_COL,"%s",dr_ptr->license_nbr);
				break;
			case F_TXI_LIC :
				strcpy(dr_ptr->taxi_license,str_to_upper(read_buf));
				mvprintw(D_PHONE_ROW,D_TXI_LIC_COL,"%s",dr_ptr->taxi_license);
				break;
		}
		refresh();
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_anything - 1\n");
	TRACE(trace_str);
#endif
		return(VALID);
	}
	else  {
		if(!err_n_fld)
			update_piu();
		return(VALID);
	}
}  /* End of check_anything()  */

/***************************************************************************
* check_y_n - "P", "D", "X", met_ok, seals, and attributes.
***************************************************************************/

int check_y_n (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	char c;
	int i;

#ifdef DEBUG
	sprintf(trace_str,"Entering check_y_n\n");
	TRACE(trace_str);
#endif
	c = Toupper(read_buf[0]);

	if(*field_entered)  {
		clear_err_line();
		if (c == YES || c == NO) {
			err_n_fld = 0;
			switch(piu)  {
				case F_SEAL_1 :
					dr_ptr->seals[0] = c;
					mvprintw(D_PHONE_ROW,D_SEAL_1_COL,"%c",dr_ptr->seals[0]);
					ipc_required = 1;
					break;
				case F_SEAL_2 :
					dr_ptr->seals[1] = c;
					mvprintw(D_PHONE_ROW,D_SEAL_2_COL,"%c",dr_ptr->seals[1]);
					ipc_required = 1;
					break;
				case F_SEAL_3 :
					dr_ptr->seals[2] = c;
					mvprintw(D_PHONE_ROW,D_SEAL_3_COL,"%c",dr_ptr->seals[2]);
					ipc_required = 1;
					break;
				case F_SEAL_4 :
					dr_ptr->seals[3] = c;
					mvprintw(D_PHONE_ROW,D_SEAL_4_COL,"%c",dr_ptr->seals[3]);
					ipc_required = 1;
					break;
				case F_SEAL_5 :
					dr_ptr->seals[4] = c;
					mvprintw(D_PHONE_ROW,D_SEAL_5_COL,"%c",dr_ptr->seals[4]);
					ipc_required = 1;
					break;
				case F_SEAL_6 :
					dr_ptr->seals[5] = c;
					mvprintw(D_PHONE_ROW,D_SEAL_6_COL,"%c",dr_ptr->seals[5]);
					ipc_required = 1;
					break;
				case F_SEAL_7 :
					dr_ptr->seals[6] = c;
					mvprintw(D_PHONE_ROW,D_SEAL_7_COL,"%c",dr_ptr->seals[6]);
					ipc_required = 1;
					break;
				case F_SEAL_8 :
					dr_ptr->seals[7] = c;
					mvprintw(D_PHONE_ROW,D_SEAL_8_COL,"%c",dr_ptr->seals[7]);
					ipc_required = 1;
					break;
				case F_SEAL_9 :
					dr_ptr->seals[8] = c;
					mvprintw(D_PHONE_ROW,D_SEAL_9_COL,"%c",dr_ptr->seals[8]);
					ipc_required = 1;
					break;
				case F_SEAL_10 :
					dr_ptr->seals[9] = c;
					mvprintw(D_PHONE_ROW,D_SEAL_10_COL,"%c",dr_ptr->seals[9]);
					ipc_required = 1;
					break;
				case F_SEAL_11 :
					dr_ptr->seals[10] = c;
					mvprintw(D_PHONE_ROW,D_SEAL_11_COL,"%c",dr_ptr->seals[10]);
					ipc_required = 1;
					break;
				case F_SEAL_12 :
					dr_ptr->seals[11] = c;
					mvprintw(D_PHONE_ROW,D_SEAL_12_COL,"%c",dr_ptr->seals[11]);
					ipc_required = 1;
					break;
				case F_SEAL_13 :
					dr_ptr->seals[12] = c;
					mvprintw(D_PHONE_ROW,D_SEAL_13_COL,"%c",dr_ptr->seals[12]);
					ipc_required = 1;
					break;
				case F_SEAL_14 :
					dr_ptr->seals[13] = c;
					mvprintw(D_PHONE_ROW,D_SEAL_14_COL,"%c",dr_ptr->seals[13]);
					ipc_required = 1;
					break;
				case F_SEAL_15 :
					dr_ptr->seals[14] = c;
					mvprintw(D_PHONE_ROW,D_SEAL_15_COL,"%c",dr_ptr->seals[14]);
					ipc_required = 1;
					break;
				case F_SEAL_16 :
					dr_ptr->seals[15] = c;
					mvprintw(D_PHONE_ROW,D_SEAL_16_COL,"%c",dr_ptr->seals[15]);
					ipc_required = 1;
					break;
				      case F_ATTR_1 :
					if ( c == NO )
					  {
					    for ( i = 0; i < ATTR_MAX; i++ )
					      dr_ptr->attributes[i] = NO;
					    mvaddstr( D_ATTR_ROW + 1, 0, "                                               " );
					    mvaddstr( D_ATTR_ROW + 2, 0, "                                               " );
					  }
					mvprintw(D_ATTR_ROW,D_ATTR_1_COL,"%c", c );
					ipc_required = 1;
					break;
#ifdef FOO
				case F_ATTR_2 :
					dr_ptr->attributes[1] = c;
					mvprintw(D_ATTR_ROW,D_ATTR_2_COL,"%c",dr_ptr->attributes[1]);
					ipc_required = 1;
					break;
				case F_ATTR_3 :
					dr_ptr->attributes[2] = c;
					mvprintw(D_ATTR_ROW,D_ATTR_3_COL,"%c",dr_ptr->attributes[2]);
					ipc_required = 1;
					break;
				case F_ATTR_4 :
					dr_ptr->attributes[3] = c;
					mvprintw(D_ATTR_ROW,D_ATTR_4_COL,"%c",dr_ptr->attributes[3]);
					ipc_required = 1;
					break;
				case F_ATTR_5 :
					dr_ptr->attributes[4] = c;
					mvprintw(D_ATTR_ROW,D_ATTR_5_COL,"%c",dr_ptr->attributes[4]);
					ipc_required = 1;
					break;
				case F_ATTR_6 :
					dr_ptr->attributes[5] = c;
					mvprintw(D_ATTR_ROW,D_ATTR_6_COL,"%c",dr_ptr->attributes[5]);
					ipc_required = 1;
					break;
				case F_ATTR_7 :
					dr_ptr->attributes[6] = c;
					mvprintw(D_ATTR_ROW,D_ATTR_7_COL,"%c",dr_ptr->attributes[6]);
					ipc_required = 1;
					break;
				case F_ATTR_8 :
					dr_ptr->attributes[7] = c;
					mvprintw(D_ATTR_ROW,D_ATTR_8_COL,"%c",dr_ptr->attributes[7]);
					ipc_required = 1;
					break;
#endif
				case F_SP_PASS :
					dr_ptr->susp_pass_call = c;
					mvprintw(D_ATTR_ROW,D_SUSP_PASS_COL,"%c",dr_ptr->susp_pass_call);
					break;
				case F_SP_DEL :
					dr_ptr->susp_del_call = c;
					mvprintw(D_ATTR_ROW+1,D_SUSP_DEL_COL,"%c",dr_ptr->susp_del_call);
					break;
				case F_SP_TPX :
					dr_ptr->susp_typex_call = c;
					mvprintw(D_ATTR_ROW+2,D_SUSP_TPX_COL,"%c",dr_ptr->susp_typex_call);
					break;
			}
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_y_n - 1\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_y_n - 2\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
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
* check_susp_rsn - check the suspended reason.
*****************************************************************************/

int check_susp_rsn (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering check_susp_rsn\n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		err_n_fld = 0;
		ipc_required = 1;
		strcpy(dr_ptr->susp_reason,str_to_upper(read_buf));
		mvprintw(D_ATTR_ROW,D_SUSP_RSN_COL,"%s",dr_ptr->susp_reason);
		refresh();
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_susp_rsn\n");
	TRACE(trace_str);
#endif
		return(VALID);
	}
	if(!err_n_fld)
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_susp_rsn\n");
	TRACE(trace_str);
#endif
	return(VALID);

}  /* End of check_susp_rsn()  */

/****************************************************************************
* check_sp_msg_1 - check the first half of the suspension message.
*****************************************************************************/

int check_sp_msg_1 (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering check_susp\n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		err_n_fld = 0;
		strcpy(dr_ptr->susp_message,str_to_upper(read_buf));
		mvprintw(D_SUSP_MSG_1_ROW,D_SUSP_MSG_1_COL,"%s",dr_ptr->susp_message);
		refresh();
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_susp - 1\n");
	TRACE(trace_str);
#endif
		return(VALID);
	}
	else  {
		if(!err_n_fld)
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_susp - 2\n");
	TRACE(trace_str);
#endif
		return(VALID);
	}
}

/******************************************************************************
* check_cmnt - check the driver comment.
******************************************************************************/

int check_cmnt (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering check_cmnt\n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		err_n_fld = 0;
		strcpy(dr_ptr->comments,str_to_upper(read_buf));
		mvprintw(D_CMNT_ROW,D_CMNT_COL,"%s",dr_ptr->comments);
		refresh();
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_cmnt - 1\n");
	TRACE(trace_str);
#endif
		return(VALID);
	}
	else  {
		if(!err_n_fld)
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_cmnt - 2\n");
	TRACE(trace_str);
#endif
		return(VALID);
	}
}  /* End of check_cmnt() */

/****************************************************************************
* check_short - check fields that are defined as shorts.
****************************************************************************/

int check_short (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int tmp_num;
#ifdef DEBUG
	sprintf(trace_str,"Entering check_short\n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_short - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		tmp_num = atoi(read_buf);
		if((0 <= tmp_num) && (tmp_num <= 32767))  {
			err_n_fld = 0;
			switch(piu)  {
				case F_POSTS :
					dr_ptr->posts = (short) tmp_num;
					mvprintw(D_POSTS_ROW,D_POSTS_COL,"%4d",dr_ptr->posts);
					ipc_required = 1;
					break;
				case F_COND_POSTS :
					dr_ptr->cond_posts = (short) tmp_num;
					mvprintw(D_POSTS_ROW,D_CND_POSTS_COL,"%4d",dr_ptr->cond_posts);
					ipc_required = 1;
					break;
				case F_ENR_POSTS :
					dr_ptr->enroute_posts = (short) tmp_num;
					mvprintw(D_POSTS_ROW,D_ENR_POSTS_COL,"%4d",dr_ptr->enroute_posts);
					ipc_required = 1;
					break;
				case F_NO_ACCEPTS :
					dr_ptr->no_accepts = (short) tmp_num;
					mvprintw(D_POSTS_ROW,D_N_ACPTS_COL,"%4d",dr_ptr->no_accepts);
					ipc_required = 1;
					break;
				case F_REJECTS :
					dr_ptr->rejects = (short) tmp_num;
					mvprintw(D_POSTS_ROW,D_REJECTS_COL,"%4d",dr_ptr->rejects);
					ipc_required = 1;
					break;
				case F_CL_OUTS :
					dr_ptr->callouts = (short) tmp_num;
					mvprintw(D_POSTS_ROW,D_CL_OUTS_COL,"%4d",dr_ptr->callouts);
					ipc_required = 1;
					break;
				case F_NO_SHOWS :
					dr_ptr->no_shows = (short) tmp_num;
					mvprintw(D_POSTS_ROW,D_NO_SHWS_COL,"%4d",dr_ptr->no_shows);
					ipc_required = 1;
					break;
				case F_FLAGS :
					dr_ptr->flags = (short) tmp_num;
					mvprintw(D_POSTS_ROW,D_FLAGS_COL,"%4d",dr_ptr->flags);
					ipc_required = 1;
					break;
				case F_BIDS :
					dr_ptr->bids = (short) tmp_num;
					mvprintw(D_POSTS_ROW,D_BIDS_COL,"%4d",dr_ptr->bids);
					ipc_required = 1;
					break;
				case F_RQ_TO_TALK :
					dr_ptr->req_to_talk = (short) tmp_num;
					mvprintw(D_POSTS_ROW,D_RQ_TLK_COL,"%4d",dr_ptr->req_to_talk);
					ipc_required = 1;
					break;
				case F_MSGS :
					dr_ptr->messages = (short) tmp_num;
					mvprintw(D_POSTS_ROW,D_MSGS_COL,"%4d",dr_ptr->messages);
					ipc_required = 1;
					break;
				case F_BREAKS :
					dr_ptr->breaks = (short) tmp_num;
					mvprintw(D_POSTS_ROW,D_BREAKS_COL,"%4d",dr_ptr->breaks);
					ipc_required = 1;
					break;
				case F_ASSIGNS :
					dr_ptr->assigns = (short) tmp_num;
					mvprintw(D_POSTS_ROW,D_ASSGN_COL,"%4d",dr_ptr->assigns);
					ipc_required = 1;
					break;
				case F_CALLS :
					dr_ptr->calls = (short) tmp_num;
					mvprintw(D_POSTS_ROW,D_CALLS_COL,"%4d",dr_ptr->calls);
					ipc_required = 1;
					break;
				case F_SH_METERS :
					dr_ptr->short_meters = (short) tmp_num;
					mvprintw(D_POSTS_ROW,D_SH_MTRS_COL,"%4d",dr_ptr->short_meters);
					ipc_required = 1;
					break;
				case F_LT_METERS :
					dr_ptr->late_meters = (short) tmp_num;
					mvprintw(D_POSTS_ROW,D_LT_MTRS_COL,"%4d",dr_ptr->late_meters);
					ipc_required = 1;
					break;
				case F_CL_BACKS :
					dr_ptr->callbacks = (short) tmp_num;
					mvprintw(D_POSTS_ROW,D_CL_BCKS_COL,"%4d",dr_ptr->callbacks);
					ipc_required = 1;
					break;
			}
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_short - 2\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_short - 3\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
	}
	if(!err_n_fld)
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_short - 4\n");
	TRACE(trace_str);
#endif
	return(VALID);

}  /* End of check_short()  */

int check_date_str(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered;	/* indicates if data was entered or not */
{
    int  i, row, date_col, len, all_zero_or_space_or_date_seperator;
    char new_date[9];

	/* if nothing has been entered then were out of here	*/
	/*														*/
    if (!*field_entered && !err_n_fld)
	{
		update_piu();
		return(VALID);
    } 

	/* Remember to clear the error line from what would have	*/
	/* been previous errors or failure of other routies to 		*/
	/* tidy up.													*/
	clear_err_line();

    row      = dr_scr_flds[*piu_ptr].prompt_row;
    date_col = dr_scr_flds[*piu_ptr].prompt_col;


	/* check to see if a zero date has been entered	*/
	/*												*/
    len = strlen(read_buf);
    all_zero_or_space_or_date_seperator = 1;
    for (i = 0; i < len; i ++)
	{
    	if ( read_buf[i] != '0'
          && read_buf[i] != ' '
          && read_buf[i] != DATE_SEPERATOR)
		{
	        all_zero_or_space_or_date_seperator = 0;
        	break;
   		}
    }

	/* if zero date entered store the zero date string, otherwise	*/
	/* check the date with the common verify date routine			*/
	/*																*/	
    if (all_zero_or_space_or_date_seperator) 
		sprintf(new_date, "00%c00%c00", DATE_SEPERATOR, DATE_SEPERATOR);
	else
    	if (verify_date(read_buf, new_date) == -1) 
		{
			/* bad date, flag it and return	*/
       		prt_error(ERROR_N_FLD);
       		err_n_fld = 1;
       		return(NOT_VALID);
    	}

    switch (piu)
	{
    case F_LIC_RNW_DATE :
        strcpy(dr_ptr->lic_renew, new_date);
    	break;

    case F_TXI_LIC_RNW_DATE :
        strcpy(dr_ptr->taxi_lic_renew, new_date);
    	break;

	default:
        	return(NOT_VALID);
	}

	/* good date, draw it and leave	*/
	err_n_fld = 0;
    mvaddstr(row, date_col, new_date);
    update_piu();
    return(VALID);

}

int check_date (piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered;	/* indicates if data was entered or not */
{
    int  i, row, date_col, time_col, len, all_zero_or_space_or_date_seperator;
    char *date_ptr, *time_ptr, new_date[9], new_time[6], yymmddhhmmss[13], zero_date[9];
    time_t *date_time_ptr, date_time;
    struct tm *date_time_struct_ptr;
    int *pDTfield = NULL;
    time_t tmpTime = 0;

    if (!*field_entered && !err_n_fld) {
	update_piu();
	return(VALID);
    }

	/* Remember to clear the error line from what would have	*/
	/* been previous errors or failure of other routies to 		*/
	/* tidy up.													*/
	clear_err_line();

    date_ptr = NULL;
    time_ptr = NULL;

    row = dr_scr_flds[*piu_ptr].prompt_row;
    date_col = dr_scr_flds[*piu_ptr].prompt_col;

    switch (piu) {
    case F_SP_FM_DATE :
      date_time_ptr = &dr_ptr->susp_from_dt_tm;
      pDTfield = &dr_ptr->susp_from_dt_tm;
	date_ptr = dr_ptr->susp_from_date;
	time_ptr = dr_ptr->susp_from_time;
	time_col = D_SP_FM_TIME_COL;
	break;

    case F_SP_TO_DATE :
	date_time_ptr = &dr_ptr->susp_to_dt_tm;
	pDTfield = &dr_ptr->susp_to_dt_tm;	
	date_ptr = dr_ptr->susp_to_date;
	time_ptr = dr_ptr->susp_to_time;
	time_col = D_SP_TO_TIME_COL;
	break;

    case F_SP_D_FM_DATE :
	date_time_ptr = &dr_ptr->susp_d_from_dt_tm;
	pDTfield = &dr_ptr->susp_d_from_dt_tm;	
	date_ptr = dr_ptr->susp_d_from_date;
	time_ptr = dr_ptr->susp_d_from_time;
	time_col = D_SP_FM_TIME_COL;
	break;

    case F_SP_D_TO_DATE :
	date_time_ptr = &dr_ptr->susp_d_to_dt_tm;
	pDTfield = &dr_ptr->susp_d_to_dt_tm;	
	date_ptr = dr_ptr->susp_d_to_date;
	time_ptr = dr_ptr->susp_d_to_time;
	time_col = D_SP_TO_TIME_COL;
	break;

    case F_SP_X_FM_DATE :
	date_time_ptr = &dr_ptr->susp_x_from_dt_tm;
	pDTfield = &dr_ptr->susp_x_from_dt_tm;	
	date_ptr = dr_ptr->susp_x_from_date;
	time_ptr = dr_ptr->susp_x_from_time;
	time_col = D_SP_FM_TIME_COL;
	break;

    case F_SP_X_TO_DATE :
	date_time_ptr = &dr_ptr->susp_x_to_dt_tm;
	pDTfield = &dr_ptr->susp_x_to_dt_tm;	
	date_ptr = dr_ptr->susp_x_to_date;
	time_ptr = dr_ptr->susp_x_to_time;
	time_col = D_SP_TO_TIME_COL;
	break;

    case F_EMRG_ON_DATE :
	date_time_ptr = &dr_ptr->emerg_on_dt_tm;
	pDTfield = &dr_ptr->emerg_on_dt_tm;	
	time_col = D_EMRG_ON_TIME_COL;
	break;

    case F_EMRG_OFF_DATE :
	date_time_ptr = &dr_ptr->emerg_of_dt_tm;
	pDTfield = &dr_ptr->emerg_of_dt_tm;	
	time_col = D_EMRG_OFF_TIME_COL;
	break;

    case F_SGN_ON_DATE :
	date_time_ptr = &dr_ptr->signed_on_dt_tm;
	pDTfield = &dr_ptr->signed_on_dt_tm;	
	time_col = D_SGN_ON_TIME_COL;
	break;

    case F_SGN_OFF_DATE :
	date_time_ptr = &dr_ptr->signed_of_dt_tm;
	pDTfield = &dr_ptr->signed_of_dt_tm;	
	time_col = D_SGN_OFF_TIME_COL;
	break;
    }
    
    len = strlen(read_buf);
    all_zero_or_space_or_date_seperator = 1;
    for (i = 0; i < len; i ++) {
	if (read_buf[i] != '0' &&
	    read_buf[i] != ' ' &&
	    read_buf[i] != DATE_SEPERATOR) {
	    all_zero_or_space_or_date_seperator = 0;
	    break;
	}
    }

    if (all_zero_or_space_or_date_seperator) {
	*date_time_ptr = 0;
	*pDTfield = 0;
	sprintf(zero_date, "00%c00%c00", DATE_SEPERATOR, DATE_SEPERATOR);
	if (date_ptr)
	    strcpy(date_ptr, zero_date);
	if (time_ptr)
	    strcpy(time_ptr, "00:00");
	mvaddstr(row, date_col, zero_date);
	mvaddstr(row, time_col, "00:00");

	err_n_fld = 0;
	update_piu();
	return(VALID);
    }

    if (verify_date(read_buf, new_date) == -1) {
	prt_error(ERROR_N_FLD);
	err_n_fld = 1;
	return(NOT_VALID);
    }

    if (strcmp(read_buf, catgets(COMMON_m_catd, COMMON_SET, COMMON_NOW, "NOW")) == 0) {
	*date_time_ptr = time(0);
	*pDTfield = (int)time(0);
	date_time_struct_ptr = localtime(date_time_ptr);
	mvaddstr(row, date_col, new_date);
    	if (date_ptr)
		strcpy(date_ptr, new_date);
#ifdef HR24
	sprintf(new_time, "%02d:%02d", date_time_struct_ptr->tm_hour,
		date_time_struct_ptr->tm_min);
#else
	if (date_time_struct_ptr->tm_hour < 12) {

	    /* 00:XX is 12:XX am */
	    if (date_time_struct_ptr->tm_hour == 0)
		sprintf(new_time, "12:%02da", date_time_struct_ptr->tm_min);
	    else
		sprintf(new_time, "%02d:%02da", date_time_struct_ptr->tm_hour,
			 date_time_struct_ptr->tm_min);
	}
	else {
	    if (date_time_struct_ptr->tm_hour == 12)
		sprintf(new_time, "12:%02dp", date_time_struct_ptr->tm_min);
	    else
		sprintf(new_time, "%02d:%02dp", date_time_struct_ptr->tm_hour - 12,
			 date_time_struct_ptr->tm_min);
	}

#endif
	if (time_ptr)
		strcpy(time_ptr, new_time);

	mvprintw(row, time_col, new_time);

	err_n_fld = 0;
	update_piu();
	return(VALID);
    }


    /* Now get the unix time for the new date entered and the
       time that is currently entered in the time field */

    tmpTime = (time_t)(*pDTfield);
    date_time_struct_ptr = localtime(&tmpTime);

	if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
	{
		yymmddhhmmss[0] = new_date[0];
		yymmddhhmmss[1] = new_date[1];
		yymmddhhmmss[2] = new_date[3];
		yymmddhhmmss[3] = new_date[4];
		yymmddhhmmss[4] = new_date[6];
		yymmddhhmmss[5] = new_date[7];
	} else
	{
	    /* Get year */
    	yymmddhhmmss[0] = new_date[6];
    	yymmddhhmmss[1] = new_date[7];

    	if (DATE_FORMAT == DAY_FIRST_IN_DATE) {

		/* Get month */
		yymmddhhmmss[2] = new_date[3];
		yymmddhhmmss[3] = new_date[4];

		/* Get day */
		yymmddhhmmss[4] = new_date[0];
		yymmddhhmmss[5] = new_date[1];
    	}
    	else {
		/* Get month */
		yymmddhhmmss[2] = new_date[0];
		yymmddhhmmss[3] = new_date[1];

		/* Get day */
		yymmddhhmmss[4] = new_date[3];
		yymmddhhmmss[5] = new_date[4];
    	}
	}

    /* Fill in the hour and minute */
    sprintf(&yymmddhhmmss[6], "%02d", date_time_struct_ptr->tm_hour);
    sprintf(&yymmddhhmmss[8], "%02d", date_time_struct_ptr->tm_min);

    /* The seconds are 00 */
    yymmddhhmmss[10] = '0';
    yymmddhhmmss[11] = '0';

    yymmddhhmmss[12] = '\0';

    if ((date_time = conversion(yymmddhhmmss)) == 0) {
	prt_error(ERROR_N_FLD);
	err_n_fld = 1;
	return(NOT_VALID);
    }
    
    //    *date_time_ptr = date_time;
    *pDTfield = (int)date_time;
    if (date_ptr)
	strcpy(date_ptr, new_date);

    mvaddstr(row, date_col, new_date);

    update_piu();

    return(VALID);
}

int check_time(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered;	/* indicates if data was entered or not */
{
    int  i, row, date_col, time_col, len, all_zero_or_space_or_colon;
    char *time_ptr, *date_ptr, new_time[7], yymmddhhmmss[13], date_string[9];
    time_t *date_time_ptr, date_time;
    struct tm *date_time_struct_ptr;
    int *pDTfield = NULL;
    time_t tmpTime = 0;

    if (!*field_entered && !err_n_fld) {
	update_piu();
	return(VALID);
    }

	/* Remember to clear the error line from what would have	*/
	/* been previous errors or failure of other routies to 		*/
	/* tidy up.													*/
	clear_err_line();


    time_ptr = NULL;
    date_ptr = NULL;

    row = dr_scr_flds[*piu_ptr].prompt_row;
    time_col = dr_scr_flds[*piu_ptr].prompt_col;

    switch (piu) {
    case F_SP_FM_TIME :
	date_time_ptr = &dr_ptr->susp_from_dt_tm;
	pDTfield = &dr_ptr->susp_from_dt_tm;
	time_ptr = dr_ptr->susp_from_time;
	date_ptr = dr_ptr->susp_from_date;
	date_col = D_SP_FM_DATE_COL;
	break;

    case F_SP_TO_TIME :
	date_time_ptr = &dr_ptr->susp_to_dt_tm;
	pDTfield = &dr_ptr->susp_to_dt_tm;	
	time_ptr = dr_ptr->susp_to_time;
	date_ptr = dr_ptr->susp_to_date;
	date_col = D_SP_TO_DATE_COL;
	break;

    case F_SP_D_FM_TIME :
	date_time_ptr = &dr_ptr->susp_d_from_dt_tm;
	pDTfield = &dr_ptr->susp_d_from_dt_tm;	
	time_ptr = dr_ptr->susp_d_from_time;
	date_ptr = dr_ptr->susp_d_from_date;
	date_col = D_SP_FM_DATE_COL;
	break;

    case F_SP_D_TO_TIME :
	date_time_ptr = &dr_ptr->susp_d_to_dt_tm;
	pDTfield = &dr_ptr->susp_d_to_dt_tm;	
	time_ptr = dr_ptr->susp_d_to_time;
	date_ptr = dr_ptr->susp_d_to_date;
	date_col = D_SP_TO_DATE_COL;
	break;

    case F_SP_X_FM_TIME :
	date_time_ptr = &dr_ptr->susp_x_from_dt_tm;
	pDTfield = &dr_ptr->susp_x_from_dt_tm;	
	time_ptr = dr_ptr->susp_x_from_time;
	date_ptr = dr_ptr->susp_x_from_date;
	date_col = D_SP_FM_DATE_COL;
	break;

    case F_SP_X_TO_TIME :
	date_time_ptr = &dr_ptr->susp_x_to_dt_tm;
	pDTfield = &dr_ptr->susp_x_to_dt_tm;	
	time_ptr = dr_ptr->susp_x_to_time;
	date_ptr = dr_ptr->susp_x_to_date;
	date_col = D_SP_TO_DATE_COL;
	break;

    case F_EMRG_ON_TIME :
	date_time_ptr = &dr_ptr->emerg_on_dt_tm;
	pDTfield = &dr_ptr->emerg_on_dt_tm;	
	date_col = D_EMRG_ON_DATE_COL;
	break;

    case F_EMRG_OFF_TIME :
	date_time_ptr = &dr_ptr->emerg_of_dt_tm;
	pDTfield = &dr_ptr->emerg_of_dt_tm;	
	date_col = D_EMRG_OFF_DATE_COL;
	break;

    case F_SGN_ON_TIME :
	date_time_ptr = &dr_ptr->signed_on_dt_tm;
	pDTfield = &dr_ptr->signed_on_dt_tm;	
	date_col = D_SGN_ON_DATE_COL;
	break;

    case F_SGN_OFF_TIME :
	date_time_ptr = &dr_ptr->signed_of_dt_tm;
	pDTfield = &dr_ptr->signed_of_dt_tm;	
	date_col = D_SGN_OFF_DATE_COL;
	break;
    }
    
    len = strlen(read_buf);
    all_zero_or_space_or_colon = 1;
    for (i = 0; i < len; i ++) {
	if (read_buf[i] != '0' &&
	    read_buf[i] != ' ' &&
	    read_buf[i] != ':') {
	    all_zero_or_space_or_colon = 0;
	    break;
	}
    }

    /* Make anything with only zeros, spaces, or colons into 00:00 */
    if (all_zero_or_space_or_colon) {
	strcpy(read_buf, "00:00");
    }

    else if (strcmp(read_buf, catgets(COMMON_m_catd, COMMON_SET, COMMON_NOW, "NOW")) == 0) {
	*date_time_ptr = time(0);
	*pDTfield = (int)time(0);
	tmpTime = (time_t)(*pDTfield);
	date_time_struct_ptr = localtime(&tmpTime);
	if ( date_time_struct_ptr->tm_year >= 100 )
	  date_time_struct_ptr->tm_year -= 100;					
	if (DATE_FORMAT == DAY_FIRST_IN_DATE)
	    sprintf(date_string, "%02d%c%02d%c%02d",
		    date_time_struct_ptr->tm_mday,
		    DATE_SEPERATOR,
		    date_time_struct_ptr->tm_mon + 1,
		    DATE_SEPERATOR,
		    date_time_struct_ptr->tm_year);
	else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
	    sprintf(date_string, "%02d%c%02d%c%02d",
		    date_time_struct_ptr->tm_year,
		    DATE_SEPERATOR,
		    date_time_struct_ptr->tm_mon + 1,
		    DATE_SEPERATOR,
		    date_time_struct_ptr->tm_mday);
	else
	    sprintf(date_string, "%02d%c%02d%c%02d",
		    date_time_struct_ptr->tm_mon + 1,
		    DATE_SEPERATOR,
		    date_time_struct_ptr->tm_mday,
		    DATE_SEPERATOR,
		    date_time_struct_ptr->tm_year);
	
	mvprintw(row, date_col, date_string);

	if (date_ptr)
		strcpy(date_ptr, date_string);

#ifdef HR24
	sprintf(new_time, "%02d:%02d", date_time_struct_ptr->tm_hour,
		 date_time_struct_ptr->tm_min);
#else
	if (date_time_struct_ptr->tm_hour < 12) {

	    /* 00:XX is 12:XX am */
	    if (date_time_struct_ptr->tm_hour == 0)
		sprintf(new_time, "12:%02da", date_time_struct_ptr->tm_min);
	    else
		sprintf(new_time, "%02d:%02da", date_time_struct_ptr->tm_hour,
			 date_time_struct_ptr->tm_min);
	}
	else {
	    if (date_time_struct_ptr->tm_hour == 12)
		sprintf(new_time, "12:%02dp", date_time_struct_ptr->tm_min);
	    else
		sprintf(new_time, "%02d:%02dp", date_time_struct_ptr->tm_hour - 12,
			 date_time_struct_ptr->tm_min);
	}
#endif

	mvprintw(row, time_col, new_time);

	if (time_ptr)
		strcpy(time_ptr, new_time);

	err_n_fld = 0;
	update_piu();
	return(VALID);
    }
	
    /* Can't enter time if date is 0 */
    else if (*date_time_ptr == 0) {
	if (time_ptr)
	    strcpy(time_ptr, "00:00");
	mvaddstr(row, time_col, "00:00 ");

	prt_error(DATE_ZERO);
	err_n_fld = 1;
	return(NOT_VALID);
    }

    /* 00:00 is valid for both HR12 and HR24 */
    if (strcmp(read_buf, "00:00") == 0) {
	if (time_ptr)
	    strcpy(time_ptr, read_buf);
	
	mvaddstr(row, time_col, read_buf);
	
	err_n_fld = 0;
	update_piu();
	return(VALID);
    }

    if (verify_time(read_buf, new_time) == -1) {
	prt_error(ERROR_N_FLD);
	err_n_fld = 1;
	return(NOT_VALID);
    }


    /* Now get the unix time for the new date entered and the
       time that is currently entered in the time field */

    tmpTime = (time_t)(*pDTfield);
    date_time_struct_ptr = localtime(&tmpTime); //date_time_ptr);
    if ( date_time_struct_ptr->tm_year >= 100 )
      date_time_struct_ptr->tm_year -= 100;				    
    
    /* Get year */
    sprintf(&yymmddhhmmss[0], "%02d", date_time_struct_ptr->tm_year);

    /* Get month */
    sprintf(&yymmddhhmmss[2], "%02d", date_time_struct_ptr->tm_mon + 1);

    /* Get day */
    sprintf(&yymmddhhmmss[4], "%02d", date_time_struct_ptr->tm_mday);

    /* Get hour */
#ifdef HR24

    yymmddhhmmss[6] = new_time[0];
    yymmddhhmmss[7] = new_time[1];

#else HR12
    hour = TO_DIGIT(new_time[0]) * 10 + TO_DIGIT(new_time[1])
    if (new_time[5] == 'a') {
	/* 12:XXam is really 00:XX */
	if (hour == 12)
	    hour = 0;
    }
    else { /* 'p' */
	if (hour > 12)
	    hour +=12;
    }
    sprintf(&yymmddhhmmss[6], "%02d", hour);

#endif

    /* Get minute */
    yymmddhhmmss[8] = new_time[3];
    yymmddhhmmss[9] = new_time[4];

    /* The seconds are 00 */
    yymmddhhmmss[10] = '0';
    yymmddhhmmss[11] = '0';

    yymmddhhmmss[12] = '\0';

    if ((date_time = conversion(yymmddhhmmss)) == 0) {
	prt_error(ERROR_N_FLD);
	err_n_fld = 1;
	return(NOT_VALID);
    }
    
    *pDTfield = date_time;
    if (time_ptr)
      {
	new_time[5] = '\0';
	strcpy(time_ptr, new_time);
      }

    mvaddstr(row, time_col, new_time);

	err_n_fld = 0;
    update_piu();

    return(VALID);
}

int check_0to9 (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int tmp_num;

	if(*field_entered)  {
		clear_err_line();
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
			return(NOT_VALID);
		}
		tmp_num = atoi(read_buf);
		if((0 <= tmp_num) && (tmp_num <= 9))  {
			err_n_fld = 0;
			switch(piu)  {
			case F_ID_SERIAL:
			    dr_ptr->serial_nbr = (char) read_buf[0];
			    mvprintw(D_CMNT_ROW,D_ID_SERIAL_COL,"%c",dr_ptr->serial_nbr);
			    break;
			}
			refresh();
			update_piu();
			return(VALID);

		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
			return(NOT_VALID);
		}
	}
	if(!err_n_fld)
	    update_piu();

	return(VALID);
	
}  /* End of check_0to9()  */			


/******************************************************************************
* update_piu - update prompt in use variable.
*******************************************************************************/

update_piu()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering update_piu\n");
	TRACE(trace_str);
#endif
	if(key_val == '\t' ||
	   key_val == '\n' ||
	   key_val == KEY_DOWN ||
	   key_val == VALID)
		if(doing_add)
			if(piu == NUM_FLDS)
				piu = 1;
			else if (( piu == F_SP_PASS) && (dr_ptr->susp_pass_call == NO))
				piu = F_SP_DEL;
			else if (( piu == F_SP_DEL) && (dr_ptr->susp_del_call == NO))
				piu = F_SP_TPX;
			else if (( piu == F_SP_TPX) && (dr_ptr->susp_typex_call == NO))
				piu = F_MSG_1;
			else 
				piu += 1;
		else if(piu == NUM_FLDS)
			piu = F_DRVR_ID;
/*	Why skip name ?	else if(piu == F_CLASS)
			piu = F_ADDRESS;
*/
		else if(piu == F_SGN_OFF_TIME)
			piu = F_LST_CALL;
		else if (( piu == F_SP_PASS) && (dr_ptr->susp_pass_call == NO))
			piu = F_SP_DEL;
		else if (( piu == F_SP_DEL) && (dr_ptr->susp_del_call == NO))
			piu = F_SP_TPX;
		else if (( piu == F_SP_TPX) && (dr_ptr->susp_typex_call == NO))
			piu = F_MSG_1;
		else
			piu += 1;

	if( key_val == KEY_UP)
		if(doing_add)
			if(piu == 1)
				piu = NUM_FLDS;
			else if (( piu == F_SP_DEL) && (dr_ptr->susp_pass_call == NO))
				piu = F_SP_PASS;
			else if (( piu == F_SP_TPX) && (dr_ptr->susp_del_call == NO))
				piu = F_SP_DEL;
			else if (( piu == F_MSG_1) && (dr_ptr->susp_typex_call == NO))
				piu = F_SP_TPX;
			else
				piu -= 1;
		else if(piu == F_LST_CALL)
			piu = F_SGN_OFF_TIME;
/* Why skip name		else if(piu == F_ADDRESS)
			piu = F_CLASS;
*/
		else if(piu == F_CLASS)
			piu = F_DRVR_ID;
		else if(piu == F_DRVR_ID)
			piu = NUM_FLDS;
		else if (( piu == F_SP_DEL) && (dr_ptr->susp_pass_call == NO))
			piu = F_SP_PASS;
		else if (( piu == F_SP_TPX) && (dr_ptr->susp_del_call == NO))
			piu = F_SP_DEL;
		else if (( piu == F_MSG_1) && (dr_ptr->susp_typex_call == NO))
			piu = F_SP_TPX;
		else
			piu -= 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving update_piu \n");
	TRACE(trace_str);
#endif
	return;

} /* End of update_piu()  */

/****************************************************************************
* numeric - make sure the field entered is a decimal number.
****************************************************************************/

numeric(in_str)
char *in_str;
{
    while(*in_str) {
	if(!isdigit(*in_str))
	    return(0);
	in_str ++;
    }
    return(1);

} /* End of numeric()  */

/****************************************************************************
* x_numeric - make sure the field entered is a hexidecimal number.
****************************************************************************/

x_numeric(in_str)
char *in_str;
{
    while(*in_str) {
	if(!isxdigit(*in_str))
	    return(0);
	in_str ++;
    }
    
    return(1);

} /* End of x_numeric()  */

/****************************************************************************
* alpha - make sure the field entered is alphabetic.
****************************************************************************/

alpha(in_str)
char *in_str;
{
    while(*in_str) {
	if(!isalpha(*in_str))
	    return(0);
	in_str ++;
    }
    return(1);
}

/* Upcase a string, returning the string */
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

int
check_attr_select(piu_ptr, read_buf, key_val, field_entered)
  int *piu_ptr;
char *read_buf;		/* pointer to the buffer in which the field
                         * was read in */
int   key_val;		/* value of the key pressed or entered */
int  *field_entered;		/* indicates if data was entered or not */
{
  char  c;
  int col,row,i;
  int stay_put = 0;
  int found_it = 0;
  
  mvwprintw(stdscr, D_ATTR_ROW, D_ATTR_1_COL, "  ");

  if ( !strncmp(read_buf, "XX", 2) )
    {
      for ( i = 0; i < 63; i++ )
        dr_ptr->attributes[i] = NO;
      stay_put = 1;
      mvwprintw(stdscr, D_ATTR_ROW, D_ATTR_1_COL, "  ");
    }

  else if ( ( strlen(read_buf) ) &&
       ( strncmp(read_buf, "  ", 2) ) )
    {
      mvwprintw(stdscr, D_ATTR_ROW, D_ATTR_1_COL, "  ");      
      found_it = 0;
      stay_put = 1;
      for ( i = 0; i < 63; i++ )
        {
          if ((!strncmp(fleet[0]->vd_attr_info[i].drv_attr, read_buf, 2)) &&
              (fleet[0]->vd_attr_info[i].drv_attr[0] != EOS))
            {
              found_it = TRUE;

              if ( dr_ptr->attributes[i] == YES )
                dr_ptr->attributes[i] = NO;
              else
                dr_ptr->attributes[i] = YES;
              break;
            }
        }
      if ( found_it == 0 )
        prt_error("Virhe kentässä");
    }
                  
  mvwprintw(stdscr, D_ATTR_ROW+1, 1, "                                            ");
  mvwprintw(stdscr, D_ATTR_ROW+2, 1, "                                            ");  

  col = 1;
  row = D_ATTR_ROW+1;
  for ( i = 0; i < 63; i++ )
    {
      if ( dr_ptr->attributes[i] == YES )
        {
          mvprintw(row, col, "%2s ", fleet[0]->vd_attr_info[i].drv_attr);
          col += 3;
        }
      if ( ( col >= L_SUSP_PASS_COL -2 ) && ( row == D_ATTR_ROW+1 ) )
        {
          col = 1;
          row = D_ATTR_ROW + 2;
        }
    }
  
  refresh();
  if ( !stay_put )
    update_piu();
  
  return (VALID);


}
