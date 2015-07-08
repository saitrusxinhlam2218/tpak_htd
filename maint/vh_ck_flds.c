static char sz__FILE__[]="@(#)vh_ck_flds.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  vh_ck_flds.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:48:34
 * @(#)  Last delta: 12/2/94 at 11:52:19
 * @(#)  SCCS File: /taxi/sccs/s.vh_ck_flds.c
 *                                                                           *
 *****************************************************************************/

#include <pwd.h>
#include <cursesX.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>

#include "comm_strdefs.h"
#include "vh_m_strdefs.h"
#include "mad_error.h"
#include "mads_types.h"
#include "call_entry.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "ex_key.h"
#include "vh_maint.h"
#include "literals.h"
#include "path.h"
#include "tokenize.h"
#include "language.h"

#define	FST_TM_BYTE	6
#define	LST_TM_BYTE 	12
#define	MIN(x,y)	(x < y ? x: y)
#define	MAX(x,y)	(x > y ? x: y)
#define	MAIL_COL	105			/* column # to print mail msg on */
#define	ZONE_DESC_CHARS	20			/* # of char's in zone_desc */
#define	MIN_DATE_CHARS	6  			/* min # of chars allowed for date entry */
#define	MIN_TIME_CHARS	4
#define MID_MAX		3839

extern short ipc_required;		/* Set if a variable which resides */
extern struct cisam_vh *vh_ptr;		/* pointer to user profile file structure */
extern struct cisam_fl *fl_ptr;		/* pointer to user profile file structure */
extern char read_buf[];			/* buffer for reading fields */
extern FILE *trace_fp;			/* trace file pointer */
extern char trace_str[];		/* trace string */
extern int piu;				/* prompt in use */
extern int key_val;			/* value of key entered */
extern int err_n_fld;			/* set if an error in field condition exists */
extern int doing_add;			/* performing add operation */
extern char *req_buf[];
extern fleet_flag;
extern vh_flag;
extern rf_flag;
extern prt_error();
extern clear_err_line();
extern dt_tm();
extern struct scr_flds vh_scr_flds[];
extern char zero_date[];
extern struct fleets *fleet[];
char *to_upper();
char a_or_p;				/* Last character in the time field indicating am or pm  */
char tmp_date[10], tmp_time[7], g_date_time[13];
long g_tmp_dt_tm;
struct tm *tm_ptr;

int update_piu();
int numeric();
long conversion();

int check_dummy(), check_fleet_1(), check_fleet_2(), check_veh_nbr();
int check_rfid(), check_lic_nbr(), check_make(), check_model();
int check_susp_rsn(), check_date(), check_time();
int check_sp_msg_1();
int check_drvr_cmnt(), check_l_emerg(), check_e_res();
int check_sgn_on(), check_sgn_off(), check_driver();
int check_last_call(), check_short(), check_y_n();
int	check_schedule(), check1to3(), check_miss_rec();
int check_color(), check_tele(), check_attr_select();

/* Declare an array of pointers to functions which check the field entered for validity. */

int (*check_field[])() = { check_dummy, check_fleet_1, check_fleet_2, 
			   check_veh_nbr, check_rfid, check_lic_nbr,
			   check_make, check_model, check_color, check_tele, check_y_n,
			   check_y_n, check_schedule, check_y_n, check_y_n,
			   check_y_n, check_y_n, check_y_n,
			   check_y_n, check_y_n, check_y_n,
			   check_y_n, check_y_n, check_y_n,
			   check_y_n, check_y_n, check_y_n,
			   check_y_n, check_y_n, check1to3,
			   check_miss_rec, check_tele, check_attr_select,
			   check_y_n, check_date, check_time, 
			   check_date, check_time,
			   check_susp_rsn, 
			   check_y_n, check_date, check_time, 
			   check_date, check_time,
			   check_y_n, check_date, check_time, 
			   check_date, check_time,
			   check_sp_msg_1, 
			   check_drvr_cmnt, check_date, check_time,
			   check_date, check_time, check_date, check_time, 
			   check_date, check_time, 
			   check_short, check_last_call, check_short,
			   check_short, check_short, check_short,
			   check_short, check_short, check_short,
			   check_short, check_short, check_short, check_short,
			   check_short, check_short, check_short,
			   check_short, check_short, check_short };

check_dummy(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
{
}

/*-----------------------------------------------------------------------------
= check_fleet_1 - check the fleet indicator.
-----------------------------------------------------------------------------*/

check_fleet_1(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
{
	static struct cisam_vh *v_ptr;
	static struct cisam_vh v_buf;
		v_ptr = &v_buf;
		clear_err_line();
	if(*field_entered)  {
		if(!isalpha(read_buf[0]))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
			return(NOT_VALID);
		}
		memcpy((char *)v_ptr,req_buf,sizeof(struct cisam_vh));
		fl_ptr->id = Toupper(read_buf[0]);
		if(db(FLEET_FILE_ID,READ_KEYS,&fl_key1,ISEQUAL,0) < 0)	{
			prt_error(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_3, "Invalid fleet ID"));
			return(NOT_VALID);
		}
		mvprintw(D_FLEET_ROW,D_FLEET_2_COL,"%d",fl_ptr->nbr);
		refresh();
		v_ptr->fleet_nbr = fl_ptr->nbr;
		fleet_flag = 1;
		err_n_fld = 0;
		memcpy(req_buf,(char *)v_ptr,sizeof(struct cisam_vh));
		vh_ptr->fleet = Toupper(read_buf[0]);
		mvprintw(D_FLEET_ROW,D_FLEET_1_COL,"%c",vh_ptr->fleet);
		refresh();
		piu = 3;
		return(NOT_VALID);
	}
	else	{
		if(!fleet_flag && key_val != ESC) {
			prt_error(catgets(VH_MAINT_m_catd, VH_MAINT_SET, VH_MAINT_4, "You must enter value"));
			return(NOT_VALID);
		}
		if(fleet_flag)
			piu = 3;
	}
}

/*----------------------------------------------------------------------------
* check_fleet_2 - check the fleet number corresponding to this vehicles fleet.
----------------------------------------------------------------------------*/

check_fleet_2(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
{
	int flt_nbr;		/* temporary fleet number */
	if(*field_entered)  {
		clear_err_line();
		flt_nbr = atoi(read_buf);
		if(0 <= flt_nbr && flt_nbr <= 7) {
			err_n_fld = 0;
			vh_ptr->fleet_nbr = (short) flt_nbr;
			update_piu();
			return(VALID);
		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
			return(NOT_VALID);
		}
	}
	else  {
		if(!err_n_fld)
			update_piu();
		return(VALID);
	}
}

/*----------------------------------------------------------------------------
* check_veh_nbr - check the vehicle number.
----------------------------------------------------------------------------*/

check_veh_nbr(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
{
	int v_nbr;		/* temporary vehicle number */

	clear_err_line();
	if(*field_entered) {
		if(!numeric(read_buf)) {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
			return(NOT_VALID);
		}
		v_nbr = atoi(read_buf);
		if(1 <= v_nbr && v_nbr <= 9999) {
			err_n_fld = 0;
			vh_ptr->nbr = (short)v_nbr;
			vh_flag = 1;
			mvprintw(D_FLEET_ROW,D_VEH_NBR_COL,"%4d",vh_ptr->nbr);
			refresh();
			update_piu();
			return(VALID);
		}
	}
	else  {
		update_piu();
		return(VALID);
	}
}

/*---------------------------------------------------------------------------
* check_last_call - check the last call taken by this vehicle.
---------------------------------------------------------------------------*/

check_last_call(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
{
	struct cisam_cl *cl_ptr;
	struct cisam_vh vh_buf;
	int tmp_call_nbr;

	if(*field_entered)  {
		clear_err_line();
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
			return(NOT_VALID);
		}
		/* Save the vehicle file information */

		memcpy((char *)&vh_buf,req_buf,sizeof(struct cisam_vh));

		cl_ptr = (struct cisam_cl *)req_buf;

		if(db(CALL_FILE_ID,OPEN,&cl_key1,ISINOUT+ISMANULOCK) < 0) {
			prt_error(CALL_OPEN_ERR);
			sleep(3);
			ERROR(' ',CALL_FILE,CALL_OPEN_ERR);
			cleanup();
		}
		tmp_call_nbr = atoi(read_buf);
		cl_ptr->nbr = tmp_call_nbr;
		if(db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL,0) < 0) {
			if(iserrno == ENOREC) {
				prt_error(ERROR_N_FLD);
				err_n_fld = 1;
				memcpy(req_buf,(char *)&vh_buf,sizeof(struct cisam_vh));
				return(NOT_VALID);
			}
			else  {
				prt_error(READ_CALL_ERR);
				sleep(3);
				ERROR(' ',CALL_FILE,READ_CALL_ERR);
				cleanup();
			}
		}
		if(db(CALL_FILE_ID,CLOSE,&cl_key1,0,0) < 0)  {
			prt_error(CALL_CLOSE_ERR);
			sleep(3);
			ERROR(' ',CALL_FILE,CALL_CLOSE_ERR);
			cleanup();
		}
		memcpy(req_buf,(char *)&vh_buf,sizeof(struct cisam_vh));
		vh_ptr->last_call = tmp_call_nbr;
		err_n_fld = 0;
		mvprintw(D_LST_EMRG_ROW,D_CL_SRV_COL,"%18d",vh_ptr->last_call);
		refresh();
		update_piu();
		return(VALID);
	}
	else  {
		if(!err_n_fld)
			update_piu();
		return(VALID);

	}
}

/*-----------------------------------------------------------------------------
* check_rfid - check the RFID.
-----------------------------------------------------------------------------*/

check_rfid (piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
{
	unsigned short rf_id;		/* temporary RFID */

	if(*field_entered)  {
		clear_err_line();
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
			return(NOT_VALID);
		}
   		rf_id = atoi(read_buf);

		if((1 <= rf_id) && (rf_id <= MID_MAX))  {
			ipc_required = 1;
			err_n_fld = 0;
			rf_flag = 1;
			sprintf(vh_ptr->mobile_rf_id, "%05x", rf_id);
			mvprintw(D_FLEET_ROW,D_RFID_COL,"%05d",strtol(vh_ptr->mobile_rf_id, (char **)NULL, 16));
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
	else  {
		if(!err_n_fld)
			update_piu();
		return(VALID);
	}
}

/*----------------------------------------------------------------------------
* check_lic_nbr - check the license number.
-----------------------------------------------------------------------------*/

check_lic_nbr (piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
{
	if(*field_entered)  {
		clear_err_line();
		err_n_fld = 0;
		strcpy(vh_ptr->license_nbr,read_buf);
		mvprintw(D_FLEET_ROW,D_LIC_NBR_COL,"%s",vh_ptr->license_nbr);
		refresh();
		update_piu();
		return(VALID);
	}
	else  {
		if(!err_n_fld)
			update_piu();
		return(VALID);
	}
}

/*------------------------------------------------------------------------
= check_make - check the make of taxii.
-------------------------------------------------------------------------*/

check_make(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
{
	if(*field_entered)  {
		clear_err_line();
		err_n_fld = 0;
		strcpy(vh_ptr->make,to_upper(read_buf));
		mvprintw(D_FLEET_ROW,D_MAKE_COL,"%s",vh_ptr->make);
		refresh();
		update_piu();
		return(VALID);
	}
	else  {
 		if(!err_n_fld)
			update_piu();
		return(VALID);
	}
}

/*--------------------------------------------------------------------------
= check_model - check the taxi model.
----------------------------------------------------------------------------*/

check_model(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf;  	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
{
	if(*field_entered)  {
		clear_err_line();
		err_n_fld = 0;
		strcpy(vh_ptr->model,to_upper(read_buf));
		mvprintw(D_FLEET_ROW,D_MODEL_COL,"%s",vh_ptr->model);
		refresh();
		update_piu();
		return(VALID);
	}
	else  {
		if(!err_n_fld)
			update_piu();
		return(VALID);
	}
}

check_color(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf;  	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
{
	if(*field_entered)  {
		clear_err_line();
		err_n_fld = 0;
		strcpy(vh_ptr->color,to_upper(read_buf));
		mvprintw(D_FLEET_ROW,D_COLOR_COL,"%s",vh_ptr->color);
		refresh();
		update_piu();
		return(VALID);
	}
	else  {
		if(!err_n_fld)
			update_piu();
		return(VALID);
	}
}

check_tele(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf;  	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
{
	if(*field_entered)  {
		clear_err_line();
		err_n_fld = 0;
		switch (piu) {
		case F_MDT_TELE:
		  ipc_required = 0;
		  strcpy(vh_ptr->mdt_tele,to_upper(read_buf));
		  mvprintw(D_SEALS_ROW,D_MDT_TELE_COL,"%s",vh_ptr->mdt_tele);
		  break;
		case F_TELEPHONE:
		  ipc_required = 0;		  
		  strcpy(vh_ptr->telephone,to_upper(read_buf));
		  mvprintw(D_FLEET_ROW,D_TELEPHONE_COL,"%s",vh_ptr->telephone);
		  break;
		}
		
		refresh();
		update_piu();
		return(VALID);
	}
	else  {
		if(!err_n_fld)
			update_piu();
		return(VALID);
	}
}

/*--------------------------------------------------------------------------
= check_y_n - check kdt_ok, met_ok, seals, and attributes.
--------------------------------------------------------------------------*/

check_y_n(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
{
    char c;
    int i;

	if(*field_entered)  {
	        c = Toupper(read_buf[0]);
		clear_err_line();
		if(c == YES || c == NO) {
			err_n_fld = 0;
			switch(piu)  {
				case F_KDT_OK :
					ipc_required = 1;
					vh_ptr->kdt_working = c;
					mvprintw(D_FLEET_ROW,D_KDT_OK_COL,"%c",vh_ptr->kdt_working);
					break;
				case F_MET_OK :
					ipc_required = 1;
					vh_ptr->meter_working = c;
					mvprintw(D_FLEET_ROW,D_MET_OK_COL,"%c",vh_ptr->meter_working);
					break;
				case F_SEAL_1 :
					ipc_required = 1;
					vh_ptr->seals[0] = c;
					mvprintw(D_SEALS_ROW,D_SEAL_1_COL,"%c",vh_ptr->seals[0]);
					break;
				case F_SEAL_2 :
					ipc_required = 1;
					vh_ptr->seals[1] = c;
					mvprintw(D_SEALS_ROW,D_SEAL_2_COL,"%c",vh_ptr->seals[1]);
					break;
				case F_SEAL_3 :
					ipc_required = 1;
					vh_ptr->seals[2] = c;
					mvprintw(D_SEALS_ROW,D_SEAL_3_COL,"%c",vh_ptr->seals[2]);
					break;
				case F_SEAL_4 :
					ipc_required = 1;
					vh_ptr->seals[3] = c;
					mvprintw(D_SEALS_ROW,D_SEAL_4_COL,"%c",vh_ptr->seals[3]);
					break;
				case F_SEAL_5 :
					ipc_required = 1;
					vh_ptr->seals[4] = c;
					mvprintw(D_SEALS_ROW,D_SEAL_5_COL,"%c",vh_ptr->seals[4]);
					break;
				case F_SEAL_6 :
					ipc_required = 1;
					vh_ptr->seals[5] = c;
					mvprintw(D_SEALS_ROW,D_SEAL_6_COL,"%c",vh_ptr->seals[5]);
					break;
				case F_SEAL_7 :
					ipc_required = 1;
					vh_ptr->seals[6] = c;
					mvprintw(D_SEALS_ROW,D_SEAL_7_COL,"%c",vh_ptr->seals[6]);
					break;
				case F_SEAL_8 :
					ipc_required = 1;
					vh_ptr->seals[7] = c;
					mvprintw(D_SEALS_ROW,D_SEAL_8_COL,"%c",vh_ptr->seals[7]);
					break;
				case F_SEAL_9 :
					ipc_required = 1;
					vh_ptr->seals[8] = c;
					mvprintw(D_SEALS_ROW,D_SEAL_9_COL,"%c",vh_ptr->seals[8]);
					break;
				case F_SEAL_10 :
					ipc_required = 1;
					vh_ptr->seals[9] = c;
					mvprintw(D_SEALS_ROW,D_SEAL_10_COL,"%c",vh_ptr->seals[9]);
					break;
				case F_SEAL_11 :
					ipc_required = 1;
					vh_ptr->seals[10] = c;
					mvprintw(D_SEALS_ROW,D_SEAL_11_COL,"%c",vh_ptr->seals[10]);
					break;
				case F_SEAL_12 :
					ipc_required = 1;
					vh_ptr->seals[11] = c;
					mvprintw(D_SEALS_ROW,D_SEAL_12_COL,"%c",vh_ptr->seals[11]);
					break;
				case F_SEAL_13 :
					ipc_required = 1;
					vh_ptr->seals[12] = c;
					mvprintw(D_SEALS_ROW,D_SEAL_13_COL,"%c",vh_ptr->seals[12]);
					break;
				case F_SEAL_14 :
					ipc_required = 1;
					vh_ptr->seals[13] = c;
					mvprintw(D_SEALS_ROW,D_SEAL_14_COL,"%c",vh_ptr->seals[13]);
					break;
				case F_SEAL_15 :
					ipc_required = 1;
					vh_ptr->seals[14] = c;
					mvprintw(D_SEALS_ROW,D_SEAL_15_COL,"%c",vh_ptr->seals[14]);
					break;
				case F_SEAL_16 :
					ipc_required = 1;
					vh_ptr->seals[15] = c;
					mvprintw(D_SEALS_ROW,D_SEAL_16_COL,"%c",vh_ptr->seals[15]);
					break;
				case F_ATTR_1 :
					if ( c == NO )
					  {
					    for ( i = 0; i < ATTR_MAX; i++ )
					      vh_ptr->attr[i] = NO;
					    mvaddstr( D_ATTR_ROW + 1, 0, "                                               " );
					    mvaddstr( D_ATTR_ROW + 2, 0, "                                               " );
					  }
					mvprintw(D_ATTR_ROW,D_ATTR_1_COL,"%c", c );
					ipc_required = 1;
					break;
#ifdef FOO
				case F_ATTR_2 :
					vh_ptr->attr[1] = c;
					mvprintw(D_ATTR_ROW,D_ATTR_2_COL,"%c",vh_ptr->attr[1]);
					ipc_required = 1;
					break;
				case F_ATTR_3 :
					vh_ptr->attr[2] = c;
					mvprintw(D_ATTR_ROW,D_ATTR_3_COL,"%c",vh_ptr->attr[2]);
					ipc_required = 1;
					break;
				case F_ATTR_4 :
					vh_ptr->attr[3] = c;
					mvprintw(D_ATTR_ROW,D_ATTR_4_COL,"%c",vh_ptr->attr[3]);
					ipc_required = 1;
					break;
				case F_ATTR_5 :
					vh_ptr->attr[4] = c;
					mvprintw(D_ATTR_ROW,D_ATTR_5_COL,"%c",vh_ptr->attr[4]);
					ipc_required = 1;
					break;
				case F_ATTR_6 :
					vh_ptr->attr[5] = c;
					mvprintw(D_ATTR_ROW,D_ATTR_6_COL,"%c",vh_ptr->attr[5]);
					ipc_required = 1;
					break;
				case F_ATTR_7 :
					vh_ptr->attr[6] = c;
					mvprintw(D_ATTR_ROW,D_ATTR_7_COL,"%c",vh_ptr->attr[6]);
					ipc_required = 1;
					break;
				case F_ATTR_8 :
					vh_ptr->attr[7] = c;
					mvprintw(D_ATTR_ROW,D_ATTR_8_COL,"%c",vh_ptr->attr[7]);
					ipc_required = 1;
					break;
#endif
				case F_PASS :
					vh_ptr->susp_pass_call = c;
					mvprintw(D_ATTR_ROW,D_PASS_COL,"%c",vh_ptr->susp_pass_call);
					break;
				case F_DEL :
					vh_ptr->susp_del_call = c;
					mvprintw(D_ATTR_ROW+1,D_DEL_COL,"%c",vh_ptr->susp_del_call);
					break;
				case F_XTYP :
					vh_ptr->susp_typex_call = c;
					mvprintw(D_ATTR_ROW+2,D_XTYP_COL,"%c",vh_ptr->susp_typex_call);
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
}

/*--------------------------------------------------------------------------
= check_susp_rsn - check the suspended reason.
--------------------------------------------------------------------------*/

check_susp_rsn(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
{
	if(*field_entered)  {
		clear_err_line();
		err_n_fld = 0;
		ipc_required = 1;
		strcpy(vh_ptr->susp_reason,to_upper(read_buf));
		mvprintw(D_ATTR_ROW,D_SUSP_RSN_COL,"%s",vh_ptr->susp_reason);
		refresh();
		update_piu();
		return(VALID);
	}
	if(!err_n_fld)
		update_piu();
	return(VALID);
}

/*----------------------------------------------------------------------------
= check_sp_msg_1 - check the first half of the suspension message.
----------------------------------------------------------------------------*/

check_sp_msg_1(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
{
	if(*field_entered)  {
		clear_err_line();
		err_n_fld = 0;
		strcpy(vh_ptr->susp_message,to_upper(read_buf));
		mvprintw(D_SP_MSG_1_ROW,D_SP_MSG_1_COL,"%s",vh_ptr->susp_message);
		refresh();
		update_piu();
		return(VALID);
	}
	else  {
		if(!err_n_fld)
			update_piu();
		return(VALID);
	}
}

/*-----------------------------------------------------------------------------
= check_drvr_cmnt - check the driver comment.
-----------------------------------------------------------------------------*/

check_drvr_cmnt(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
{
	if(*field_entered)  {
		clear_err_line();
		err_n_fld = 0;
		strcpy(vh_ptr->comments,to_upper(read_buf));
		mvprintw(D_DRVR_CMNT_ROW,D_DRVR_CMNT_COL,"%s",vh_ptr->comments);
		refresh();
		update_piu();
		return(VALID);
	}
	else  {
		if(!err_n_fld)
			update_piu();
		return(VALID);
	}
}

/*---------------------------------------------------------------------------
= check_short - check fields that are defined as shorts.
---------------------------------------------------------------------------*/

check_short(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
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
		if(0 <= tmp_num && tmp_num <= 32767) {
			err_n_fld = 0;
			switch(piu)  {
				case F_DRIVER :
					vh_ptr->last_driver = (short)tmp_num;
					mvprintw(D_LST_EMRG_ROW,D_DRIVER_COL,"%4d",vh_ptr->last_driver);
					break;
				case F_CL_SRV :
					vh_ptr->last_call = (short)tmp_num;
					mvprintw(D_LST_EMRG_ROW,D_CL_SRV_COL,"%18d",vh_ptr->last_call);
					break;
				case F_POST :
					vh_ptr->posts = (short)tmp_num;
					mvprintw(D_POST_ROW,D_POST_COL,"%4d",vh_ptr->posts);
					ipc_required = 1;
					break;
				case F_CPOST :
					vh_ptr->cond_posts = (short)tmp_num;
					mvprintw(D_POST_ROW,D_CPOST_COL,"%4d",vh_ptr->cond_posts);
					ipc_required = 1;
					break;
				case F_EPOST :
					vh_ptr->enroute_posts = (short)tmp_num;
					mvprintw(D_POST_ROW,D_EPOST_COL,"%4d",vh_ptr->enroute_posts);
					ipc_required = 1;
					break;
				case F_N_ACPT :
					vh_ptr->no_accepts = (short)tmp_num;
					mvprintw(D_POST_ROW,D_N_ACPT_COL,"%4d",vh_ptr->no_accepts);
					ipc_required = 1;
					break;
				case F_REJCT :
					vh_ptr->reject = (short)tmp_num;
					mvprintw(D_POST_ROW,D_REJCT_COL,"%4d",vh_ptr->reject);
					ipc_required = 1;
					break;
				case F_C_OUT :
					vh_ptr->callouts = (short)tmp_num;
					mvprintw(D_POST_ROW,D_C_OUT_COL,"%4d",vh_ptr->callouts);
					ipc_required = 1;
					break;
				case F_NSHOW :
					vh_ptr->no_shows = (short)tmp_num;
					mvprintw(D_POST_ROW,D_NSHOW_COL,"%4d",vh_ptr->no_shows);
					ipc_required = 1;
					break;
				case F_FLAG :
					vh_ptr->flags = (short)tmp_num;
					mvprintw(D_POST_ROW,D_FLAG_COL,"%4d",vh_ptr->flags);
					ipc_required = 1;
					break;
				case F_BID :
					vh_ptr->bids = (short)tmp_num;
					mvprintw(D_POST_ROW,D_BID_COL,"%4d",vh_ptr->bids);
					ipc_required = 1;
					break;
				case F_TALK :
					vh_ptr->req_to_talk = (short)tmp_num;
					mvprintw(D_POST_ROW,D_TALK_COL,"%4d",vh_ptr->req_to_talk);
					ipc_required = 1;
					break;
				case F_MSG :
					vh_ptr->messages = (short)tmp_num;
					mvprintw(D_POST_ROW,D_MSG_COL,"%4d",vh_ptr->messages);
					ipc_required = 1;
					break;
				case F_BREAK :
					vh_ptr->breaks = (short)tmp_num;
					mvprintw(D_POST_ROW,D_BREAK_COL,"%4d",vh_ptr->breaks);
					ipc_required = 1;
					break;
				case F_ASSIGNS :
					vh_ptr->assigns = (short)tmp_num;
					mvprintw(D_POST_ROW,D_ASSIGNS_COL,"%4d",vh_ptr->assigns);
					ipc_required = 1;
					break;
				case F_CALLS :
					vh_ptr->calls = (short)tmp_num;
					mvprintw(D_POST_ROW,D_CALLS_COL,"%4d",vh_ptr->calls);
					ipc_required = 1;
					break;
				case F_SH_MET :
					vh_ptr->short_meters = (short)tmp_num;
					mvprintw(D_POST_ROW,D_SH_MET_COL,"%4d",vh_ptr->short_meters);
					ipc_required = 1;
					break;
				case F_LT_MET :
					vh_ptr->late_meters = (short)tmp_num;
					mvprintw(D_POST_ROW,D_LT_MET_COL,"%4d",vh_ptr->late_meters);
					ipc_required = 1;
					break;
				case F_CL_BACK :
					vh_ptr->callbacks = (short)tmp_num;
					mvprintw(D_POST_ROW,D_CL_BACK_COL,"%4d",vh_ptr->callbacks);
					ipc_required = 1;
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
}

/*---------------------------------------------------------------------------
 * check schedule 
 *---------------------------------------------------------------------------
 */

check_schedule(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
{
	int tmp_num;
	if(*field_entered)  {
		clear_err_line();
		if (!strlen(read_buf) || (read_buf[0] == ' '))
		  {
			err_n_fld = 0;
			vh_ptr->schedule_num = (short)0;
			mvprintw(D_FLEET_ROW,D_SCHEDULE_COL,"%2d",vh_ptr->schedule_num);
			refresh();
			update_piu();
			return(VALID);
		  }
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
			return(NOT_VALID);
		}
		tmp_num = atoi(read_buf);
		if(0 <= tmp_num && tmp_num <= 25) {
			err_n_fld = 0;
			vh_ptr->schedule_num = (short)tmp_num;
			mvprintw(D_FLEET_ROW,D_SCHEDULE_COL,"%2d",vh_ptr->schedule_num);
			refresh();
			update_piu();
			return(VALID);
		}
		else {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
			return(NOT_VALID);
		}

	}
	if(!err_n_fld)
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
    char *date_ptr, *time_ptr, new_date[9], new_time[7], yymmddhhmmss[13], zero_date[9];
    time_t *date_time_ptr, date_time;
    int *pDTfield = NULL;
    time_t tmpTime = 0;
    
    struct tm *date_time_struct_ptr;


    if (!*field_entered) {
	update_piu();
	return(VALID);
    }

    date_ptr = NULL;
    time_ptr = NULL;

    row = vh_scr_flds[*piu_ptr].prompt_row;
    date_col = vh_scr_flds[*piu_ptr].prompt_col;

    switch (piu) {
    case F_SUSP_FM_1 :
	date_time_ptr = &vh_ptr->susp_from_dt_tm;
	pDTfield = &vh_ptr->susp_from_dt_tm;
	date_ptr = vh_ptr->susp_from_date;
	time_ptr = vh_ptr->susp_from_time;
	time_col = D_SUSP_FM_2_COL;
	break;

    case F_SUSP_TO_1 :
	date_time_ptr = &vh_ptr->susp_to_dt_tm;
	pDTfield = &vh_ptr->susp_to_dt_tm;
	date_ptr = vh_ptr->susp_to_date;
	time_ptr = vh_ptr->susp_to_time;
	time_col = D_SUSP_TO_2_COL;
	break;

    case F_SUSP_D_FM_1 :
	date_time_ptr = &vh_ptr->susp_d_from_dt_tm;
	pDTfield = &vh_ptr->susp_d_from_dt_tm;
	date_ptr = vh_ptr->susp_d_from_date;
	time_ptr = vh_ptr->susp_d_from_time;
	time_col = D_SUSP_FM_2_COL;
	break;

    case F_SUSP_D_TO_1 :
	date_time_ptr = &vh_ptr->susp_d_to_dt_tm;
	pDTfield = &vh_ptr->susp_d_to_dt_tm;
	date_ptr = vh_ptr->susp_d_to_date;
	time_ptr = vh_ptr->susp_d_to_time;
	time_col = D_SUSP_TO_2_COL;
	break;

    case F_SUSP_X_FM_1 :
	date_time_ptr = &vh_ptr->susp_x_from_dt_tm;
	pDTfield = &vh_ptr->susp_x_from_dt_tm;
	date_ptr = vh_ptr->susp_x_from_date;
	time_ptr = vh_ptr->susp_x_from_time;
	time_col = D_SUSP_FM_2_COL;
	break;

    case F_SUSP_X_TO_1 :
	date_time_ptr = &vh_ptr->susp_x_to_dt_tm;
	pDTfield = &vh_ptr->susp_x_to_dt_tm;
	date_ptr = vh_ptr->susp_x_to_date;
	time_ptr = vh_ptr->susp_x_to_time;
	time_col = D_SUSP_TO_2_COL;
	break;

    case F_L_EMRG_1 :
	date_time_ptr = &vh_ptr->last_emerg_on;
	pDTfield = &vh_ptr->last_emerg_on;
	time_col = D_L_EMRG_2_COL;
	break;

    case F_E_RES_1 :
	date_time_ptr = &vh_ptr->last_emerg_off;
	pDTfield = &vh_ptr->last_emerg_off;
	time_col = D_E_RES_2_COL;
	break;

    case F_SGN_ON_1 :
	date_time_ptr = &vh_ptr->last_signed_on;
	pDTfield = &vh_ptr->last_signed_on;
	time_col = D_SGN_ON_2_COL;
	break;

    case F_SGN_OFF_1 :
	date_time_ptr = &vh_ptr->last_signed_off;
	pDTfield = &vh_ptr->last_signed_off;
	time_col = D_SGN_OFF_2_COL;
	break;

    default:
	abort();
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
	mvaddstr(row, date_col, zero_date);
	mvaddstr(row, time_col, "00:00");
	if (time_ptr)
		strcpy(time_ptr, "00:00");
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
	tmpTime = (time_t)(*pDTfield);
	date_time_struct_ptr = localtime(&tmpTime);
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


    if (!*field_entered) {
	update_piu();
	return(VALID);
    }

    time_ptr = NULL;
    date_ptr = NULL;

    row = vh_scr_flds[*piu_ptr].prompt_row;
    time_col = vh_scr_flds[*piu_ptr].prompt_col;

    switch (piu) {
    case F_SUSP_FM_2 :
	date_time_ptr = &vh_ptr->susp_from_dt_tm;
	pDTfield = &vh_ptr->susp_from_dt_tm;
	time_ptr = vh_ptr->susp_from_time;
	date_ptr = vh_ptr->susp_from_date;
	date_col = D_SUSP_FM_1_COL;
	break;

    case F_SUSP_TO_2 :
	date_time_ptr = &vh_ptr->susp_to_dt_tm;
	pDTfield = &vh_ptr->susp_to_dt_tm;	
	time_ptr = vh_ptr->susp_to_time;
	date_ptr = vh_ptr->susp_to_date;
	date_col = D_SUSP_TO_1_COL;
	break;

    case F_SUSP_D_FM_2 :
	date_time_ptr = &vh_ptr->susp_d_from_dt_tm;
	pDTfield = &vh_ptr->susp_d_from_dt_tm;	
	time_ptr = vh_ptr->susp_d_from_time;
	date_ptr = vh_ptr->susp_d_from_date;
	date_col = D_SUSP_FM_1_COL;
	break;

    case F_SUSP_D_TO_2 :
	date_time_ptr = &vh_ptr->susp_d_to_dt_tm;
	pDTfield = &vh_ptr->susp_d_to_dt_tm;	
	time_ptr = vh_ptr->susp_d_to_time;
	date_ptr = vh_ptr->susp_d_to_date;
	date_col = D_SUSP_TO_1_COL;
	break;

    case F_SUSP_X_FM_2 :
	date_time_ptr = &vh_ptr->susp_x_from_dt_tm;
	pDTfield = &vh_ptr->susp_x_from_dt_tm;	
	time_ptr = vh_ptr->susp_x_from_time;
	date_ptr = vh_ptr->susp_x_from_date;
	date_col = D_SUSP_FM_1_COL;
	break;

    case F_SUSP_X_TO_2 :
	date_time_ptr = &vh_ptr->susp_x_to_dt_tm;
	pDTfield = &vh_ptr->susp_x_to_dt_tm;	
	time_ptr = vh_ptr->susp_x_to_time;
	date_ptr = vh_ptr->susp_x_to_date;
	date_col = D_SUSP_TO_1_COL;
	break;

    case F_L_EMRG_2 :
	date_time_ptr = &vh_ptr->last_emerg_on;
	pDTfield = &vh_ptr->last_emerg_on;	
	date_col = D_L_EMRG_1_COL;
	break;

    case F_E_RES_2 :
	date_time_ptr = &vh_ptr->last_emerg_off;
	pDTfield = &vh_ptr->last_emerg_off;	
	date_col = D_E_RES_1_COL;
	break;

    case F_SGN_ON_2 :
	date_time_ptr = &vh_ptr->last_signed_on;
	pDTfield = &vh_ptr->last_signed_on;	
	date_col = D_SGN_ON_1_COL;
	break;

    case F_SGN_OFF_2 :
	date_time_ptr = &vh_ptr->last_signed_off;
	pDTfield = &vh_ptr->last_signed_off;	
	date_col = D_SGN_OFF_1_COL;
	break;

    default:
	abort();
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
		sprintf(new_time, "%12:%02da", date_time_struct_ptr->tm_min);
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

	update_piu();
	return(VALID);
    }
	
    /* Can't enter time if date is 0 */
    else if (*date_time_ptr == 0) {
	prt_error(DATE_ZERO);
	err_n_fld = 1;
	return(NOT_VALID);
    }

    /* 00:00 is valid for both HR12 and HR24 */
    if (strcmp(read_buf, "00:00") == 0) {
	if (time_ptr)
	    strcpy(time_ptr, read_buf);
	
	mvaddstr(row, time_col, read_buf);
	
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
    date_time_struct_ptr = localtime(&tmpTime);

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
    
    *pDTfield = (int)date_time;
    if (time_ptr)
	strcpy(time_ptr, new_time);

    mvaddstr(row, time_col, new_time);

    update_piu();

    return(VALID);
}

/*-----------------------------------------------------------------------------
= update_piu - update prompt in use variable.
-----------------------------------------------------------------------------*/

update_piu()
{
	if(key_val == '\t' || key_val == '\n' || key_val == '\r' || key_val == KEY_DOWN || key_val == VALID) {
		if ( piu == F_TERM_TYPE )
			piu = piu + 2;		/* skip the missing receipts field */
		else if((piu == F_PASS) && (vh_ptr->susp_pass_call == NO))
			piu = F_DEL;
		else if((piu == F_DEL) && (vh_ptr->susp_del_call == NO))
			piu = F_XTYP;
		else if((piu == F_XTYP) && (vh_ptr->susp_typex_call == NO))
			piu = F_SP_MSG_1;
		else
			piu++;
		if(doing_add && piu > NUM_FLDS) 
			piu=1;
		else if(piu > NUM_FLDS) 
			piu=F_RFID;
	}

	if(key_val == KEY_UP) {
		if ( piu == F_ATTR_1 ) 
			piu = piu - 2;      /* skip the missing receipts field */ 
		else if((piu == F_DEL) && (vh_ptr->susp_pass_call == NO))
			piu = F_PASS;
		else if((piu == F_XTYP) && (vh_ptr->susp_del_call == NO))
			piu = F_DEL;
		else if((piu == F_SP_MSG_1) && (vh_ptr->susp_typex_call == NO))
			piu = F_XTYP;
		else 
			piu--;
		if(doing_add && piu < 1) 
			piu=NUM_FLDS;
		else if(piu < F_RFID) 
			piu=NUM_FLDS;
	}
	return;
}

/*---------------------------------------------------------------------------
= numeric - make sure the field entered is a decimal number.
---------------------------------------------------------------------------*/

numeric(in_str)
char in_str[132];
{
	int i = 0;		/* loop index */
	int validity = 1;	/* = 0,1 for bad, good, start with a valid string */

	while(in_str[i]) 
		if(!isdigit(in_str[i++])) 
			validity = 0;

	return(validity);
}

/*---------------------------------------------------------------------------
= alpha - make sure the field entered is alphabetic.
---------------------------------------------------------------------------*/

alpha(in_str)
char in_str[132];
{
	int i = 0;		/* loop index */
	int validity = 1;	/* = 0,1 for bad, good, start with a valid string */

	while(in_str[i]) 
		if(!isalpha(in_str[i++])) 
			validity = 0;

	return(validity);
}

/*----------------------------------------------------------------------------
= itoa - convert integer to a string.
----------------------------------------------------------------------------*/

itoa(inint, buffer)
char buffer[];
int inint;
{ 
	sprintf(buffer,"%d",inint); 
	return;
}

/*----------------------------------------------------------------------------
= reverse - reverse string s in place.
----------------------------------------------------------------------------

reverse(s)
char s[];
{
	int c,i,j;
	for (i=0, j=strlen(s)-1; i<j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
	return;
}

/*---------------------------------------------------------------------------
= to_upper - convert a character string to upper case.
---------------------------------------------------------------------------*/

char *
to_upper(in_buf)
char in_buf[BUFFER_SIZE];
{
	int i;
	char local_buf[BUFFER_SIZE];

	i = 0;
	while(in_buf[i])  {
		local_buf[i] = Toupper(in_buf[i]);
		i++;
	}
	local_buf[i] = '\0';
	return(local_buf);
}

/*---------------------------------------------------------------------------
= get_maint_time - put the original time into the g_date_time field
---------------------------------------------------------------------------*/

get_maint_time(piu)
int piu;
{
	char timebuff[4];

	switch(piu)  {
		case F_SUSP_FM_1: 
			tm_ptr = localtime(&vh_ptr->susp_from_dt_tm); break;
		case F_SUSP_TO_1: 
			tm_ptr = localtime(&vh_ptr->susp_to_dt_tm); break;
		case F_SUSP_D_FM_1: 
			tm_ptr = localtime(&vh_ptr->susp_d_from_dt_tm); break;
		case F_SUSP_D_TO_1: 
			tm_ptr = localtime(&vh_ptr->susp_d_to_dt_tm); break;
		case F_SUSP_X_FM_1: 
			tm_ptr = localtime(&vh_ptr->susp_x_from_dt_tm); break;
		case F_SUSP_X_TO_1: 
			tm_ptr = localtime(&vh_ptr->susp_x_to_dt_tm); break;
		case F_L_EMRG_1: 
			tm_ptr = localtime(&vh_ptr->last_emerg_on); break;
		case F_E_RES_1: 
			tm_ptr = localtime(&vh_ptr->last_emerg_off); break;
		case F_SGN_ON_1: 
			tm_ptr = localtime(&vh_ptr->last_signed_on); break;
		case F_SGN_OFF_1: 
			tm_ptr = localtime(&vh_ptr->last_signed_off); break;
	}

	/* store time values to the global date and time buffer */
	sprintf(timebuff,"%02d",tm_ptr->tm_hour);
	g_date_time[6]=timebuff[0];
	g_date_time[7]=timebuff[1];
	sprintf(timebuff,"%02d",tm_ptr->tm_min);
	g_date_time[8]=timebuff[0];
	g_date_time[9]=timebuff[1];
}

/*---------------------------------------------------------------------------
= get_maint_date - put the original date into the g_date_time field
---------------------------------------------------------------------------*/

get_maint_date(pasd_piu)
int pasd_piu;
{
	char datebuff[4];
	
	switch(pasd_piu)  {
		case F_SUSP_FM_2: 
			tm_ptr = localtime(&vh_ptr->susp_from_dt_tm); break;
		case F_SUSP_TO_2: 	
			tm_ptr = localtime(&vh_ptr->susp_to_dt_tm); break;
		case F_SUSP_D_FM_2: 
			tm_ptr = localtime(&vh_ptr->susp_d_from_dt_tm); break;
		case F_SUSP_D_TO_2: 	
			tm_ptr = localtime(&vh_ptr->susp_d_to_dt_tm); break;
		case F_SUSP_X_FM_2: 
			tm_ptr = localtime(&vh_ptr->susp_x_from_dt_tm); break;
		case F_SUSP_X_TO_2: 	
			tm_ptr = localtime(&vh_ptr->susp_x_to_dt_tm); break;
		case F_L_EMRG_2: 
			tm_ptr = localtime(&vh_ptr->last_emerg_on); break;
		case F_E_RES_2: 	
			tm_ptr = localtime(&vh_ptr->last_emerg_off); break;
		case F_SGN_ON_2: 
			tm_ptr = localtime(&vh_ptr->last_signed_on); break;
		case F_SGN_OFF_2: 
			tm_ptr = localtime(&vh_ptr->last_signed_off); break;
	} 

	/* store date values to the global date and time buffer */
	if ( tm_ptr->tm_year >= 100 )
	  tm_ptr->tm_year -= 100;	
	sprintf(datebuff,"%02d",tm_ptr->tm_year);
	g_date_time[0]=datebuff[0];
	g_date_time[1]=datebuff[1];
	sprintf(datebuff,"%02d",tm_ptr->tm_mon+1);
	g_date_time[2]=datebuff[0];
	g_date_time[3]=datebuff[1];
	sprintf(datebuff,"%02d",tm_ptr->tm_mday);
	g_date_time[4]=datebuff[0];
	g_date_time[5]=datebuff[1];
}

/*---------------------------------------------------------------------------
= set_maint_date 
=	good_entry=1, save the long integer date_time and display the date
=	good_entry=0, display the original date
---------------------------------------------------------------------------*/

set_maint_date(pasd_piu,good_entry,local_date)
int pasd_piu,good_entry;
char *local_date;
{
	long tmp_dt_tm;

	if (good_entry) tmp_dt_tm = conversion(g_date_time);

	switch(pasd_piu)  {
	case F_SUSP_FM_1:
		if(good_entry) {
			vh_ptr->susp_from_dt_tm = tmp_dt_tm;
			strcpy(vh_ptr->susp_from_date,local_date);
			mvprintw(D_ATTR_ROW,D_SUSP_FM_1_COL,"%s",local_date);
		}
		else  mvprintw(D_ATTR_ROW,D_SUSP_FM_1_COL,"%s",vh_ptr->susp_from_date);
		break;
	case F_SUSP_TO_1 :
		if(good_entry) {
			vh_ptr->susp_to_dt_tm = tmp_dt_tm;
			strcpy(vh_ptr->susp_to_date,local_date);
			mvprintw(D_ATTR_ROW,D_SUSP_TO_1_COL,"%s",local_date);
		}
		else  mvprintw(D_ATTR_ROW,D_SUSP_TO_1_COL,"%s",vh_ptr->susp_to_date);
		break;
	case F_SUSP_D_FM_1:
		if(good_entry) {
			vh_ptr->susp_d_from_dt_tm = tmp_dt_tm;
			strcpy(vh_ptr->susp_d_from_date,local_date);
			mvprintw(D_ATTR_ROW+1,D_SUSP_FM_1_COL,"%s",local_date);
		}
		else  mvprintw(D_ATTR_ROW+1,D_SUSP_FM_1_COL,"%s",vh_ptr->susp_d_from_date);
		break;
	case F_SUSP_D_TO_1 :
		if(good_entry) {
			vh_ptr->susp_d_to_dt_tm = tmp_dt_tm;
			strcpy(vh_ptr->susp_d_to_date,local_date);
			mvprintw(D_ATTR_ROW+1,D_SUSP_TO_1_COL,"%s",local_date);
		}
		else  mvprintw(D_ATTR_ROW+1,D_SUSP_TO_1_COL,"%s",vh_ptr->susp_d_to_date);
		break;
	case F_SUSP_X_FM_1:
		if(good_entry) {
			vh_ptr->susp_x_from_dt_tm = tmp_dt_tm;
			strcpy(vh_ptr->susp_x_from_date,local_date);
			mvprintw(D_ATTR_ROW+2,D_SUSP_FM_1_COL,"%s",local_date);
		}
		else  mvprintw(D_ATTR_ROW+2,D_SUSP_FM_1_COL,"%s",vh_ptr->susp_x_from_date);
		break;
	case F_SUSP_X_TO_1 :
		if(good_entry) {
			vh_ptr->susp_x_to_dt_tm = tmp_dt_tm;
			strcpy(vh_ptr->susp_x_to_date,local_date);
			mvprintw(D_ATTR_ROW+2,D_SUSP_TO_1_COL,"%s",local_date);
		}
		else  mvprintw(D_ATTR_ROW+2,D_SUSP_TO_1_COL,"%s",vh_ptr->susp_x_to_date);
		break;
	case F_L_EMRG_1 : 
		if ( good_entry ) {
			vh_ptr->last_emerg_on = tmp_dt_tm;
			mvprintw(D_LST_EMRG_ROW,D_L_EMRG_1_COL,"%s",local_date);
		}
		else  {
			if(vh_ptr->last_emerg_on == 0)
			  mvprintw(D_LST_EMRG_ROW,D_L_EMRG_1_COL, zero_date);
			else  {
				tm_ptr = localtime(&vh_ptr->last_emerg_on);
				if ( tm_ptr->tm_year >= 100 )
				  tm_ptr->tm_year -= 100;				
				if (DATE_FORMAT == DAY_FIRST_IN_DATE)
				    mvprintw(D_LST_EMRG_ROW,D_L_EMRG_1_COL,"%02d%c%02d%c%02d",
					     tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_year);
				else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
				    mvprintw(D_LST_EMRG_ROW,D_L_EMRG_1_COL,"%02d%c%02d%c%02d",
					     tm_ptr->tm_year, DATE_SEPERATOR, tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_mday);
				else
				    mvprintw(D_LST_EMRG_ROW,D_L_EMRG_1_COL,"%02d%c%02d%c%02d",
					     tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_year);
			}
		}
		break;
	case F_E_RES_1 :
		if ( good_entry ) {
			vh_ptr->last_emerg_off = tmp_dt_tm;
			mvprintw(D_LST_EMRG_ROW,D_E_RES_1_COL,"%s",local_date);
		}
		else  {
			if(vh_ptr->last_emerg_off == 0)
			  mvprintw(D_LST_EMRG_ROW,D_E_RES_1_COL, zero_date);
			else  {
				tm_ptr = localtime(&vh_ptr->last_emerg_off);
				if ( tm_ptr->tm_year >= 100 )
				  tm_ptr->tm_year -= 100;								
				if (DATE_FORMAT == DAY_FIRST_IN_DATE)
				    mvprintw(D_LST_EMRG_ROW,D_E_RES_1_COL,"%02d%c%02d%c%02d",
					     tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_year);
				else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
				    mvprintw(D_LST_EMRG_ROW,D_E_RES_1_COL,"%02d%c%02d%c%02d",
					     tm_ptr->tm_year, DATE_SEPERATOR, tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_mday);
				else
				    mvprintw(D_LST_EMRG_ROW,D_E_RES_1_COL,"%02d%c%02d%c%02d",
					     tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_year);
			}
		}
		break;
	case F_SGN_ON_1 :
		if ( good_entry ) {
			vh_ptr->last_signed_on = tmp_dt_tm;
			mvprintw(D_LST_EMRG_ROW,D_SGN_ON_1_COL,"%s",local_date);
		}
		else  {
			if(vh_ptr->last_signed_on == 0)
			  mvprintw(D_LST_EMRG_ROW,D_SGN_ON_1_COL, zero_date);
			else  {
				tm_ptr = localtime(&vh_ptr->last_signed_on);
				if ( tm_ptr->tm_year >= 100 )
				  tm_ptr->tm_year -= 100;								
				if (DATE_FORMAT == DAY_FIRST_IN_DATE)
				    mvprintw(D_LST_EMRG_ROW,D_SGN_ON_1_COL,"%02d%c%02d%c%02d",
					     tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_year);
				else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
				    mvprintw(D_LST_EMRG_ROW,D_SGN_ON_1_COL,"%02d%c%02d%c%02d",
					     tm_ptr->tm_year, DATE_SEPERATOR, tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_mday);
				else
				    mvprintw(D_LST_EMRG_ROW,D_SGN_ON_1_COL,"%02d%c%02d%c%02d",
					     tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_year);
			}
		}
		break;
	case F_SGN_OFF_1 :
		if ( good_entry ) {
			vh_ptr->last_signed_off = tmp_dt_tm;
			mvprintw(D_LST_EMRG_ROW,D_SGN_OFF_1_COL,"%s",local_date);
		}
		else  {
			if(vh_ptr->last_signed_off)
			  mvprintw(D_LST_EMRG_ROW,D_SGN_OFF_1_COL, zero_date);
			else  {
				tm_ptr = localtime(&vh_ptr->last_signed_off);
				if ( tm_ptr->tm_year >= 100 )
				  tm_ptr->tm_year -= 100;								
				if (DATE_FORMAT == DAY_FIRST_IN_DATE)
				    mvprintw(D_LST_EMRG_ROW,D_SGN_OFF_1_COL,"%02d%c%02d%c%02d",
					     tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_year);
				else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
				    mvprintw(D_LST_EMRG_ROW,D_SGN_OFF_1_COL,"%02d%c%02d%c%02d",
					     tm_ptr->tm_year, DATE_SEPERATOR, tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_mday);
				else
				    mvprintw(D_LST_EMRG_ROW,D_SGN_OFF_1_COL,"%02d%c%02d%c%02d",
					     tm_ptr->tm_mon+1, DATE_SEPERATOR, tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_year);
			}
		}
		break;
	}

	if(good_entry) { 	/* needs inter_process communication, was no error */
		ipc_required = 1;
		err_n_fld = 0;
		return(VALID);
	}
	else  { 		/* print the error condition */
		prt_error(ERROR_N_FLD);
		err_n_fld = 1;
		return(NOT_VALID);
	}
}

/*---------------------------------------------------------------------------
= set_maint_time 
=	good_entry=1, save the long integer date_time and display the time
=	good_entry=0, display the original time
---------------------------------------------------------------------------*/

set_maint_time(pasd_piu,good_entry,local_time)
int pasd_piu,good_entry;
char *local_time;
{
	long tmp_dt_tm;
	char am_or_pm;

	if(good_entry) 
		tmp_dt_tm = conversion(g_date_time);

	switch(pasd_piu)  {
		case F_SUSP_FM_2 :
			if(good_entry) {
				vh_ptr->susp_from_dt_tm = tmp_dt_tm;
				strcpy(vh_ptr->susp_from_time,local_time);
				mvprintw(D_ATTR_ROW,D_SUSP_FM_2_COL,"%s",local_time);
			}
			else  mvprintw(D_ATTR_ROW,D_SUSP_FM_2_COL,"%s",vh_ptr->susp_from_time);
			break;
		case F_SUSP_TO_2 :
			if(good_entry) {
				vh_ptr->susp_to_dt_tm = tmp_dt_tm;
				strcpy(vh_ptr->susp_to_time,local_time);
				mvprintw(D_ATTR_ROW,D_SUSP_TO_2_COL,"%s",local_time);
			}
			else  mvprintw(D_ATTR_ROW,D_SUSP_TO_2_COL,"%s",vh_ptr->susp_to_time);
			break;
		case F_SUSP_D_FM_2 :
			if(good_entry) {
				vh_ptr->susp_d_from_dt_tm = tmp_dt_tm;
				strcpy(vh_ptr->susp_d_from_time,local_time);
				mvprintw(D_ATTR_ROW+1,D_SUSP_FM_2_COL,"%s",local_time);
			}
			else  mvprintw(D_ATTR_ROW+1,D_SUSP_FM_2_COL,"%s",vh_ptr->susp_from_time);
			break;
		case F_SUSP_D_TO_2 :
			if(good_entry) {
				vh_ptr->susp_d_to_dt_tm = tmp_dt_tm;
				strcpy(vh_ptr->susp_d_to_time,local_time);
				mvprintw(D_ATTR_ROW+1,D_SUSP_TO_2_COL,"%s",local_time);
			}
			else  mvprintw(D_ATTR_ROW+1,D_SUSP_TO_2_COL,"%s",vh_ptr->susp_to_time);
			break;
		case F_SUSP_X_FM_2 :
			if(good_entry) {
				vh_ptr->susp_x_from_dt_tm = tmp_dt_tm;
				strcpy(vh_ptr->susp_x_from_time,local_time);
				mvprintw(D_ATTR_ROW+2,D_SUSP_FM_2_COL,"%s",local_time);
			}
			else  mvprintw(D_ATTR_ROW+2,D_SUSP_FM_2_COL,"%s",vh_ptr->susp_x_from_time);
			break;
		case F_SUSP_X_TO_2 :
			if(good_entry) {
				vh_ptr->susp_x_to_dt_tm = tmp_dt_tm;
				strcpy(vh_ptr->susp_x_to_time,local_time);
				mvprintw(D_ATTR_ROW+2,D_SUSP_TO_2_COL,"%s",local_time);
			}
			else  mvprintw(D_ATTR_ROW+2,D_SUSP_TO_2_COL,"%s",vh_ptr->susp_x_to_time);
			break;
		case F_L_EMRG_2 : 
			if(good_entry) {
				vh_ptr->last_emerg_on = tmp_dt_tm;
				mvprintw(D_LST_EMRG_ROW,D_L_EMRG_2_COL,"%s",local_time);
			}
			else {
				tm_ptr = localtime(&vh_ptr->last_emerg_on);
#ifdef HR24
				mvprintw(D_LST_EMRG_ROW,D_L_EMRG_2_COL,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#else
				if (tm_ptr->tm_hour >= 12) {
				    am_or_pm = 'p';
				    tm_ptr->tm_hour -= 12;
				}
				else
				    am_or_pm = 'a';
				mvprintw(D_LST_EMRG_ROW,D_L_EMRG_2_COL,"%02d:%02d%c",tm_ptr->tm_hour,tm_ptr->tm_min, am_or_pm);
#endif
			}
			break;
		case F_E_RES_2 :
			if(good_entry) {
				vh_ptr->last_emerg_off = tmp_dt_tm;
				mvprintw(D_LST_EMRG_ROW,D_E_RES_2_COL,"%s",local_time);
			}
			else  {
				tm_ptr = localtime(&vh_ptr->last_emerg_off);
#ifdef HR24
				mvprintw(D_LST_EMRG_ROW,D_E_RES_2_COL,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#else
				if (tm_ptr->tm_hour >= 12) {
				    am_or_pm = 'p';
				    tm_ptr->tm_hour -= 12;
				}
				else
				    am_or_pm = 'a';
				mvprintw(D_LST_EMRG_ROW,D_E_RES_2_COL,"%02d:%02d%c",tm_ptr->tm_hour,tm_ptr->tm_min, am_or_pm);
#endif
			}
			break;
		case F_SGN_ON_2 :
			if(good_entry) {
				vh_ptr->last_signed_on = tmp_dt_tm;
				mvprintw(D_LST_EMRG_ROW,D_SGN_ON_2_COL,"%s",local_time);
			}
			else  {
				tm_ptr = localtime(&vh_ptr->last_signed_on);
#ifdef HR24
				mvprintw(D_LST_EMRG_ROW,D_SGN_ON_2_COL,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#else
				if (tm_ptr->tm_hour >= 12) {
				    am_or_pm = 'p';
				    tm_ptr->tm_hour -= 12;
				}
				else
				    am_or_pm = 'a';
				mvprintw(D_LST_EMRG_ROW,D_SGN_ON_2_COL,"%02d:%02d%c",tm_ptr->tm_hour,tm_ptr->tm_min, am_or_pm);
#endif
			}
			break;
		case F_SGN_OFF_2 :
			if(good_entry) {
				vh_ptr->last_signed_off = tmp_dt_tm;
				mvprintw(D_LST_EMRG_ROW,D_SGN_OFF_2_COL,"%s",local_time);
			}
			else  {
				tm_ptr = localtime(&vh_ptr->last_signed_off);
#ifdef HR24
				mvprintw(D_LST_EMRG_ROW,D_SGN_OFF_2_COL,"%02d:%02d",tm_ptr->tm_hour,tm_ptr->tm_min);
#else
				if (tm_ptr->tm_hour >= 12) {
				    am_or_pm = 'p';
				    tm_ptr->tm_hour -= 12;
				}
				else
				    am_or_pm = 'a';
				mvprintw(D_LST_EMRG_ROW,D_SGN_OFF_2_COL,"%02d:%02d%c",tm_ptr->tm_hour,tm_ptr->tm_min, am_or_pm);
#endif
			}
			break;
		}

		if(good_entry) {	/* a valid field, notify an ipc is needed */
			ipc_required = 1;
			err_n_fld = 0;
			return(VALID);
		}
		else {			/* not a valid field, print the error message and set the flags */
			prt_error(ERROR_N_FLD);
			ipc_required = 0;
			err_n_fld = 1;
			return(NOT_VALID);
		}
}


/****************************************************************************
* check1to3 - check the fields with values of 1 to 3.
****************************************************************************/

int check1to3 (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int value;		/* temporary value */
#ifdef DEBUG
	sprintf(trace_str,"Entering check_1to3 \n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		read_buf[1] = '\0';
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to3 - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		value = atoi(read_buf);
		if(( value >= 1 ) && (value <= 9))  {
			err_n_fld = 0;
			switch(piu)  {
			case F_TERM_TYPE :
				vh_ptr->term_type = value;
				mvprintw(D_SEALS_ROW,D_TERM_TYPE_COL,"%5d",vh_ptr->term_type);
				break;
			}
			refresh();
			piu = F_ATTR_1;	/* missing receipts is display only so skip to attr_1 */
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to3 - 2\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to3 - 3\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
	}
	else  {
		if(!err_n_fld)
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to3 - 4\n");
	TRACE(trace_str);
#endif
		return(VALID);

	}
} /* End of check_1to3()  */


check_miss_rec(piu_ptr, read_buf, key_val, field_entered)
int *piu_ptr;		/* pointer to the number of the field read in */
char *read_buf; 	/* pointer to the buffer in which the field was read in */
int key_val;    	/* value of the key pressed or entered */
int *field_entered; 	/* indicates if data was entered or not */
{

/* This field is display only so we should never get here */
		clear_err_line();
		mvprintw(D_SEALS_ROW,D_NUM_MISS_RECEIPTS_COL,"%14d",vh_ptr->itm_nbr_missing_receipts);
		refresh();
		update_piu();
		return(VALID);
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
      for ( i = 0; i < 31; i++ )
        vh_ptr->attr[i] = NO;
      stay_put = 1;
      mvwprintw(stdscr, D_ATTR_ROW, D_ATTR_1_COL, "  ");
    }

  else if ( ( strlen(read_buf) ) &&
       ( strncmp(read_buf, "  ", 2) ) )
    {
      mvwprintw(stdscr, D_ATTR_ROW, D_ATTR_1_COL, "  ");      
      found_it = 0;
      stay_put = 1;
      for ( i = 0; i < 31; i++ )
        {
          if ((!strncmp(fleet[0]->vd_attr_info[i].veh_attr, read_buf, 2)) &&
              (fleet[0]->vd_attr_info[i].veh_attr[0] != EOS))
            {
              found_it = TRUE;

              if ( vh_ptr->attr[i] == YES )
                vh_ptr->attr[i] = NO;
              else
                vh_ptr->attr[i] = YES;
              break;
            }
        }
      if ( found_it == 0 )
        prt_error("ATTRIBUTE NOT AVAILABLE");
    }
                  
  mvwprintw(stdscr, D_ATTR_ROW+1, 1, "                                            ");
  mvwprintw(stdscr, D_ATTR_ROW+2, 1, "                                            ");  

  col = 1;
  row = D_ATTR_ROW+1;
  for ( i = 0; i < 31; i++ )
    {
      if ( vh_ptr->attr[i] == YES )
        {
          mvprintw(row, col, "%2s ", fleet[0]->vd_attr_info[i].veh_attr);
          col += 3;
        }
      if ( ( col >= L_PASS_COL -2 ) && ( row == D_ATTR_ROW+1 ) )
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
