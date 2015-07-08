static char sz__FILE__[]="@(#)fl_ck_flds.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fl_ck_flds.c; Rel: 6.7.0.0; Get date: 4/14/92 at 16:11:38
 * @(#)  Last delta: 1/24/92 at 17:06:03
 * @(#)  SCCS File: /taxi/sccs/s.fl_ck_flds.c
 *                                                                           *
 *****************************************************************************/

#include <pwd.h>
#include <curses.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <limits.h>
#include <float.h>

#include "mad_error.h"
#include "df_maint.h"
#include "ex_key.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "fl_maint.h"
#include "literals.h"
#include "fl_scr.h"
#include "tokenize.h"
#include "language.h"
#include "fl_m_strdefs.h"

extern struct cisam_fl *fl_ptr;		/* pointer to fleet file structure */
extern char read_buf[];			/* buffer for reading fields */
extern FILE *trace_fp;			/* trace file pointer */
extern char trace_str[];		/* trace string */
extern char req_buf[];			/* buffer for the db routine */
extern int piu;				/* prompt in use */
extern int key_val;			/* value of key entered */
extern int err_n_fld;			/* set if an error in field condition exists */
extern int doing_add;			/* performing add operation */
extern int scr_nbr;			/* screen number */
extern int fl_flag;
extern void prt_error(), prt_cisam_err();
extern void clear_err_line();
extern dt_tm();
extern struct scr_flds fl_scr_flds[];

void update_piu();
int numeric();
long convert_dt_tm();
char *str_to_upper();
double atof();
double strtod();

/*  declare an array of pointers to functions which check the 
 *  field entered for validity.
 */

int check_dummy (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}



/******************************************************************************
* check_fleet - check the fleet indicator.
******************************************************************************/

int check_fleet (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
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
		err_n_fld = 0;
		fl_flag = 1;
		fl_ptr->id = Toupper(read_buf[0]);
		mvprintw(ROW_3,D_ID_COL,"%c",fl_ptr->id);
		refresh();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_fleet - 2\n");
	TRACE(trace_str);
#endif 
		return(VALID);
	}
	else	{

		if((!fl_flag) && (key_val != ESC))	{
			prt_error(catgets(FL_MAINT_m_catd, FL_MAINT_SET, FL_MAINT_1, "You must enter value"));
			return(NOT_VALID);
		}
	}

} /* End of check_fleet()  */

/****************************************************************************
* check_1or2 - check fields with 1 or 2 as valid input.
****************************************************************************/

int check_1or2 (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{

#ifdef DEBUG
	sprintf(trace_str,"Entering check_1or2 \n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		if((read_buf[0] == '1')  ||
		   (read_buf[0] == '2'))  {
			err_n_fld = 0;
			switch(piu)  {
				case F_D_MSG_AP :
					fl_ptr->drv_msg_approve = read_buf[0];
					mvprintw(ROW_19,D_D_MSG_AP_COL+1,"%c",fl_ptr->drv_msg_approve);
					break;
				case F_S_CL_RVW :
					fl_ptr->subsc_review = read_buf[0];
					mvprintw(ROW_20,D_S_CL_RVW_COL+1,"%c",fl_ptr->subsc_review);
					break;

				case F_APPROACH_TARIFF:
					fl_ptr->approach_tariff = read_buf[0];
					mvprintw(fl_scr_flds[piu].prompt_row, fl_scr_flds[piu].prompt_col,
						 fl_scr_flds[piu].fmt_str, fl_ptr->approach_tariff);
					break;

				case F_HANDLE_SYSTEM_ASSIGNED_CALLS:
					fl_ptr->handle_system_assigned_calls = (short) (read_buf[0] - '0');
					mvprintw(fl_scr_flds[piu].prompt_row, fl_scr_flds[piu].prompt_col,
						 fl_scr_flds[piu].fmt_str, fl_ptr->handle_system_assigned_calls);
					break;

				      case F_GPS_COORD_SYS:
					fl_ptr->gps_coord_sys = (short)(read_buf[0] - '0');
					mvprintw(fl_scr_flds[piu].prompt_row, fl_scr_flds[piu].prompt_col,
						 fl_scr_flds[piu].fmt_str, fl_ptr->gps_coord_sys);
					break;
						 

				default:
					break;
			}
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1or2 - 1\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1or2 - 2\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
	}
	else  {
		if(!err_n_fld)
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1or2 - 3\n");
	TRACE(trace_str);
#endif
		return(VALID);

	}
} /* End of check_1or2()  */

/****************************************************************************
* check_1to3 - check the fields with values of 1 to 3.
****************************************************************************/

int check_1to3 (piu_ptr, read_buf, key_val, field_entered)
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
		if((1 <= value) && (value <= 3))  {
			err_n_fld = 0;
			switch(piu)  {

			case F_CNCL_2 :
				fl_ptr->cancel_action = read_buf[0];
				mvprintw(ROW_8,D_CNCL_2_COL+1,"%c",fl_ptr->cancel_action);
				break;
			case F_C_OFFER :
				fl_ptr->call_offer_proc = read_buf[0];
				mvprintw(ROW_13,D_C_OFFER_COL+1,"%c",fl_ptr->call_offer_proc);
				break;
			case F_N_ACPT_TP :
				fl_ptr->no_acpt_penalt = read_buf[0];
				mvprintw(ROW_7,D_N_ACPT_TP_COL+1,"%c",fl_ptr->no_acpt_penalt);
				break;
			case F_NAK_OFFER :
				fl_ptr->nak_offer_penal = read_buf[0];
				mvprintw(ROW_9,D_NAK_OFFER_COL+1,"%c",fl_ptr->nak_offer_penal);
				break;
			case F_EPOST_AL : 
				fl_ptr->when_enr_post = read_buf[0];
				mvprintw(ROW_11,D_EPOST_AL_COL+4,"%c",fl_ptr->when_enr_post);
				break;
			case F_ST_FLG_P :
				fl_ptr->auto_stand_flag = read_buf[0];
				mvprintw(ROW_17, D_ST_FLG_P_COL+1,"%c",fl_ptr->auto_stand_flag);
				break;

			}
			refresh();
			update_piu();
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

/****************************************************************************
* check_1to4 - check the fields with values of 1 to 4.
****************************************************************************/

int check_1to4 (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int value;		/* temporary vehicle number */
#ifdef DEBUG
	sprintf(trace_str,"Entering check_1to4 \n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		read_buf[1] = '\0';
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to4 - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		value = atoi(read_buf);
		if((1 <= value) && (value <= 4))  {
			err_n_fld = 0;
			switch(piu)  {
			  case F_REJ_LVL_1 :
			  fl_ptr->lev1_rej_penalt = read_buf[0];
			  mvprintw(ROW_3,D_REJ_LVL_1_COL,"%c",fl_ptr->lev1_rej_penalt);
			  break;
			case F_REJ_PRM :
			  fl_ptr->main_rej_penalt = read_buf[0];
			  mvprintw(ROW_3,D_REJ_PRM_COL,"%c",fl_ptr->main_rej_penalt);
			  break;
			case F_REJ_LVL_2 :
				fl_ptr->lev2_rej_penalt = read_buf[0];
				mvprintw(ROW_3,D_REJ_LVL_2_COL,"%c",fl_ptr->lev2_rej_penalt);
				break;
			case F_REJ_LVL_3 :
				fl_ptr->lev3_rej_penalt = read_buf[0];
				mvprintw(ROW_3,D_REJ_LVL_3_COL,"%c",fl_ptr->lev3_rej_penalt);
				break;
			case F_REJ_LVL_4 :
				fl_ptr->lev4_rej_penalt = read_buf[0];
				mvprintw(ROW_3,D_REJ_LVL_4_COL,"%c",fl_ptr->lev4_rej_penalt);
				break;
			case F_REJ_LVL_5 :
				fl_ptr->lev5_rej_penalt = read_buf[0];
				mvprintw(ROW_3,D_REJ_LVL_5_COL,"%c",fl_ptr->lev5_rej_penalt);
				break;
			case F_REJ_LVL_6 :
				fl_ptr->lev6_rej_penalt = read_buf[0];
				mvprintw(ROW_3,D_REJ_LVL_6_COL,"%c",fl_ptr->lev6_rej_penalt);
				break;
			case F_REJ_LVL_7 :
				fl_ptr->lev7_rej_penalt = read_buf[0];
				mvprintw(ROW_3,D_REJ_LVL_7_COL,"%c",fl_ptr->lev7_rej_penalt);
				break;
			        case F_REJ_LVL_8 :
				  fl_ptr->lev8_rej_penalt = read_buf[0];
			          mvprintw(ROW_3,D_REJ_LVL_8_COL,"%c",fl_ptr->lev8_rej_penalt);
				  break;			  
				case F_N_SH_2 :
					fl_ptr->no_show_action = read_buf[0];
					mvprintw(ROW_7,D_N_SHOW_2_COL+1,"%c",fl_ptr->no_show_action);
					break;
				case F_R_CALC_1 :
					fl_ptr->pass_rate_calc = read_buf[0];
					mvprintw(ROW_17,D_R_CALC_1_COL+1,"%c",fl_ptr->pass_rate_calc);
					break;

				case F_R_CALC_2 :
					fl_ptr->dlv_rate_calc = read_buf[0];
					mvprintw(ROW_17,D_R_CALC_2_COL+1,"%c",fl_ptr->dlv_rate_calc);
					break;
					

			}
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to4 - 2\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to4 - 3\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
	}
	else  {
		if(!err_n_fld)
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to4 - 4\n");
	TRACE(trace_str);
#endif
		return(VALID);

	}
} /* End of check_1to4()  */

/****************************************************************************
* check_1to5 - check the fields with values of 1 to 5.
****************************************************************************/

int check_1to5 (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int value;		/* temporary value */
#ifdef DEBUG
	sprintf(trace_str,"Entering check_1to5 \n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		read_buf[1] = '\0';
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to5 - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		value = atoi(read_buf);
		if((1 <= value) && (value <= 5))  {
			err_n_fld = 0;
			switch(piu)  {
				case F_CNCL_1 :
					fl_ptr->drv_cancel_pri = read_buf[0];
					mvprintw(ROW_8,D_CNCL_1_COL+1,"%c",fl_ptr->drv_cancel_pri);
					break;
				case F_REG_PRI :
					fl_ptr->drv_regular_pri = read_buf[0];
					mvprintw(ROW_10,D_REG_PRI_COL+1,"%c",fl_ptr->drv_regular_pri);
					break;
				case F_LOCAL_PRI :
					fl_ptr->drv_local_pri = read_buf[0];
					mvprintw(ROW_11,D_LOCAL_PRI_COL+1,"%c",fl_ptr->drv_local_pri);
					break;
				case F_N_SH_1 :
					fl_ptr->drv_no_show_pri = read_buf[0];
					mvprintw(ROW_7,D_N_SHOW_1_COL+1,"%c",fl_ptr->drv_no_show_pri);
					break;
				      case F_MTCH_ALG :
					fl_ptr->match_algorithm = read_buf[0];
					mvprintw(ROW_14,D_MATCH_ALG_COL+1,"%c",fl_ptr->match_algorithm);
					break;										
			}
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to5 - 2\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to5 - 3\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
	}
	else  {
		if(!err_n_fld)
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to5 - 4\n");
	TRACE(trace_str);
#endif
		return(VALID);

	}
} /* End of check_1to5()  */

/****************************************************************************
* check_0to8 - check the fields with values of 1 to 8.
****************************************************************************/

int check_0to8 (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int value;		/* temporary vehicle number */
#ifdef DEBUG
	sprintf(trace_str,"Entering check_0to8 \n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		read_buf[1] = '\0';
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_0to8 - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		value = atoi(read_buf);
		if((0 <= value) && (value <= 8))  {
			err_n_fld = 0;
			switch(piu)  {
				case F_D_ATTR :
					fl_ptr->max_nbr_drv_atr = value;
					mvprintw(ROW_8,D_D_ATTR_COL+1,"%d",fl_ptr->max_nbr_drv_atr);
					break;
				case F_V_ATTR :
					fl_ptr->max_nbr_veh_atr = value;
					mvprintw(ROW_9,D_VEH_ATTR_COL+1,"%d",fl_ptr->max_nbr_veh_atr);
					break;
				case F_M_ACTION_RETRY:
					fl_ptr->m_action_retry = value;
					mvprintw(ROW_10,D_M_ACTION_RETRY_COL+1,"%d",fl_ptr->m_action_retry);
					break;
			}
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_0to8 - 2\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_0to8 - 3\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
	}
	else  {
		if(!err_n_fld)
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_0to8 - 4\n");
	TRACE(trace_str);
#endif
		return(VALID);

	}
} /* End of check_0to8()  */

/****************************************************************************
* check_1to8 - check the fields with values of 1 to 8.
****************************************************************************/

int check_1to8 (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int value;		/* temporary vehicle number */
#ifdef DEBUG
	sprintf(trace_str,"Entering check_1to8 \n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		read_buf[1] = '\0';
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to8 - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		value = atoi(read_buf);
		if((1 <= value) && (value <= 8))  {
			err_n_fld = 0;
			switch(piu)  {
				case F_D_ATTR :
					fl_ptr->max_nbr_drv_atr = read_buf[0];
					mvprintw(ROW_8,D_D_ATTR_COL+1,"%c",fl_ptr->max_nbr_drv_atr);
					break;
				case F_V_ATTR :
					fl_ptr->max_nbr_veh_atr = read_buf[0];
					mvprintw(ROW_9,D_VEH_ATTR_COL+1,"%c",fl_ptr->max_nbr_veh_atr);
					break;
				case F_Z_BK_LVL :
					fl_ptr->zone_backup_lev = read_buf[0];
					mvprintw(ROW_18,D_Z_BK_LVL_COL+1,"%c",fl_ptr->zone_backup_lev);
					break;

				case F_NUM_LEVELS_TO_COMPUTE_STATS:
					fl_ptr->num_levels_to_compute_stats = (short)(read_buf[0] - '0');
					mvprintw(fl_scr_flds[piu].prompt_row, fl_scr_flds[piu].prompt_col,
						 fl_scr_flds[piu].fmt_str, fl_ptr->num_levels_to_compute_stats);
					break;


			}
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to8 - 2\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to8 - 3\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
	}
	else  {
		if(!err_n_fld)
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to8 - 4\n");
	TRACE(trace_str);
#endif
		return(VALID);

	}
} /* End of check_1to8()  */

/****************************************************************************
* check_1to9 - check the fields with values of 1 to 9.
****************************************************************************/

int check_1to9 (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int value;		/* temporary vehicle number */
#ifdef DEBUG
	sprintf(trace_str,"Entering check_1to9 \n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		read_buf[1] = '\0';
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to9 - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		value = atoi(read_buf);
		if((1 <= value) && (value <= 9))  {
			err_n_fld = 0;
			fl_ptr->max_vehs_call = value;
			mvprintw(ROW_10,D_VEH_CALL_COL+1,"%3d",fl_ptr->max_vehs_call);
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to9 - 2\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to9 - 3\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
	}
	else  {
		if(!err_n_fld)
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to9 - 4\n");
	TRACE(trace_str);
#endif
		return(VALID);

	}
} /* End of check_1to9()  */

/****************************************************************************
* check_1to63 - check the fields with values of 1 to 63.
****************************************************************************/

int check_1to63 (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int value;		/* temporary value */
#ifdef DEBUG
	sprintf(trace_str,"Entering check_1to63 \n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to63 - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		value = atoi(read_buf);
		if((1 <= value) && (value <= 63))  {
			err_n_fld = 0;
			fl_ptr->high_rookie_pri = value;
			mvprintw(ROW_11,D_ROOKIE_COL+1,"%d",fl_ptr->high_rookie_pri);
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to63 - 2\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to63 - 3\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
	}
	else  {
		if(!err_n_fld)
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to63 - 4\n");
	TRACE(trace_str);
#endif
		return(VALID);

	}
} /* End of check_1to63()  */

/****************************************************************************
* check_6to63 - check the fields with values of 6 to 63.
****************************************************************************/

int check_6to63 (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int value;		/* temporary vehicle number */
#ifdef DEBUG
	sprintf(trace_str,"Entering check_6to63 \n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_6to63 - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		value = atoi(read_buf);
		if((5 <= value) && (value <= 63))  {
			err_n_fld = 0;
			switch(piu)  {
				case F_CHARGE :
					fl_ptr->charge_pri_def = value;
					mvprintw(ROW_17,D_CHARGE_COL+1,"%2d",fl_ptr->charge_pri_def);
					break;
				case F_S_CALL :
					fl_ptr->subsc_pri_def = value;
					mvprintw(ROW_18,D_S_CALLS_COL+1,"%2d",fl_ptr->subsc_pri_def);
					break;
				case F_TM_CLS :
					fl_ptr->calls_pri_def = value;
					mvprintw(ROW_19,D_T_CALLS_COL+1,"%2d",fl_ptr->calls_pri_def);
					break;
				case F_PASS :
					fl_ptr->pass_pri_def = value;
					mvprintw(ROW_17,D_PASS_COL+1,"%2d",fl_ptr->pass_pri_def);
					break;
				case F_DEL :
					fl_ptr->del_pri_def = value;
					mvprintw(ROW_18,D_DEL_COL+1,"%2d",fl_ptr->del_pri_def);
					break;
				case F_X_TYPE :
					fl_ptr->typex_pri_def = value;
					mvprintw(ROW_19,D_X_TYPE_COL+1,"%2d",fl_ptr->typex_pri_def);
					break;
				case F_W_KUP :
					fl_ptr->wake_up_pri_def = value;
					mvprintw(ROW_20,D_W_KUP_COL+1,"%2d",fl_ptr->wake_up_pri_def);
					break;
			}
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_6to63 - 2\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_6to63 - 3\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
	}
	else  {
		if(!err_n_fld)
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_6to63 - 4\n");
	TRACE(trace_str);
#endif
		return(VALID);

	}
} /* End of check_6to63()  */
/****************************************************************************
* check_call_typ - check call type.
****************************************************************************/

int check_call_typ (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{

        char c;

#ifdef DEBUG
	sprintf(trace_str,"Entering check_call_typ \n");
	TRACE(trace_str);
#endif
        c = Toupper(read_buf[0]);

	if(*field_entered)  {
		clear_err_line();
		if(c == PASS || c == DELIV || c == TYPEX || c == WAKEUP) {
			err_n_fld = 0;
			fl_ptr->call_type_def = c;
			mvprintw(ROW_12,D_C_TYPE_COL+1,"%c",fl_ptr->call_type_def);
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_call_typ - 1\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_call_typ - 2\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
	}
	else  {
		if(!err_n_fld)
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_call_typ - 3\n");
	TRACE(trace_str);
#endif
		return(VALID);

	}
} /* End of check_call_typ()  */


/****************************************************************************
* check_1to999 - check the fields with values of 1 to 999.
****************************************************************************/

int check_1to999 (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int value;		/* temporary vehicle number */
#ifdef DEBUG
	sprintf(trace_str,"Entering check_1to999 \n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_1to999 - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		value = atoi(read_buf);
		switch (piu) {
		case F_REJ_CNT:
		    if((1 <= value) && (value <= 999))  {
			err_n_fld = 0;
			fl_ptr->max_rej_count = value;
			mvprintw(ROW_7,D_REJ_CNT_COL+1,"%3d",fl_ptr->max_rej_count);
		    }
		    else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
			return(NOT_VALID);
		    }				    
		    break;

		case F_NS_SHORT:
		    if((0 <= value) && (value <= 999))  {
			err_n_fld = 0;
			fl_ptr->ns_short = value;
			mvprintw(ROW_14,D_NS_SHORT_COL+1,"%3d",fl_ptr->ns_short);
		    }
		    else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
			return(NOT_VALID);
		    }		    
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
} /* End of check_1to999()  */

/****************************************************************************
* check_nbr - check the fleet number.
****************************************************************************/

int check_nbr (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int nbr;		/* temporary driver number */
#ifdef DEBUG
	sprintf(trace_str,"Entering check_nbr \n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		clear_err_line();
		read_buf[1] = '\0';
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_nbr - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
		nbr = atoi(read_buf);
		if((0 <= nbr) && (nbr <= 7))  {
			err_n_fld = 0;
			fl_ptr->nbr = (short) nbr;
			mvprintw(ROW_3,D_NBR_COL,"%d",fl_ptr->nbr);
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_nbr - 2\n");
	TRACE(trace_str);
#endif
			return(VALID);
		}
		else  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_nbr - 3\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}
	}
	else  {
		if(!err_n_fld)
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_nbr - 4\n");
	TRACE(trace_str);
#endif
		return(VALID);

	}
} /* End of check_nbr()  */

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
				strcpy(fl_ptr->name,str_to_upper(read_buf));
				mvprintw(ROW_3,D_NAME_COL,"%s",fl_ptr->name);
				break;
			case F_COMMENT :
				strcpy(fl_ptr->comments,str_to_upper(read_buf));
				mvprintw(ROW_4,D_COMMENT_COL,"%s",fl_ptr->comments);
				break;
			case F_AU_TL_PT :
				strcpy(fl_ptr->audt_trail_port,read_buf);
				mvprintw(ROW_3,D_AU_TR_PT_COL+1,"%s",fl_ptr->audt_trail_port);
				break;
			case F_CITY :
				strcpy(fl_ptr->city_default,read_buf);
				mvprintw(ROW_10,D_CITY_COL+1,"%s",fl_ptr->city_default);
				break;
			case F_1ST_TXT :
				strcpy(fl_ptr->estimate_text1,str_to_upper(read_buf));
				mvprintw(ROW_10,D_1ST_TXT_COL+1,"%s",fl_ptr->estimate_text1);
				break;
			case F_2ND_TXT :
				strcpy(fl_ptr->estimate_text2,str_to_upper(read_buf));
				mvprintw(ROW_11,D_2ND_TXT_COL+1,"%s",fl_ptr->estimate_text2);
				break;
			case F_3RD_TXT :
				strcpy(fl_ptr->estimate_text3,str_to_upper(read_buf));
				mvprintw(ROW_12,D_3RD_TXT_COL+1,"%s",fl_ptr->estimate_text3);
				break;
			case F_4TH_TXT :
				strcpy(fl_ptr->estimate_text4,str_to_upper(read_buf));
				mvprintw(ROW_13,D_4TH_TXT_COL+1,"%s",fl_ptr->estimate_text4);
				break;
			case F_5TH_TXT :
				strcpy(fl_ptr->estimate_text5,str_to_upper(read_buf));
				mvprintw(ROW_14,D_5TH_TXT_COL+1,"%s",fl_ptr->estimate_text5);
				break;
			case F_6TH_TXT :
				strcpy(fl_ptr->estimate_text6,str_to_upper(read_buf));
				mvprintw(ROW_15,D_6TH_TXT_COL+1,"%s",fl_ptr->estimate_text6);
				break;
			case F_7TH_TXT :
				strcpy(fl_ptr->estimate_text7,str_to_upper(read_buf));
				mvprintw(ROW_16,D_7TH_TXT_COL+1,"%s",fl_ptr->estimate_text7);
				break;
			case F_8TH_TXT :
				strcpy(fl_ptr->estimate_text8,str_to_upper(read_buf));
				mvprintw(ROW_17,D_8TH_TXT_COL+1,"%s",fl_ptr->estimate_text8);
				break;
			case F_DEFAULT :
				strcpy(fl_ptr->def_est_text,str_to_upper(read_buf));
				mvprintw(ROW_18,D_DEFAULT_COL+1,"%s",fl_ptr->def_est_text);
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

check_area_code(piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
    if (*field_entered) {
	clear_err_line();
	if(!numeric(read_buf))  {
	    prt_error(ERROR_N_FLD);
	    err_n_fld = 1;
	    return(NOT_VALID);
	}

	strcpy(fl_ptr->phone_area_def,read_buf);
	mvprintw(ROW_11,D_A_CODE_COL+1,"%s",fl_ptr->phone_area_def);
	refresh();
    }
    update_piu();
    return(VALID);
}

check_missing_receipts(piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
    short  tmp;

    if (*field_entered) {
	clear_err_line();
	if(!numeric(read_buf))  {
	    prt_error(ERROR_N_FLD);
	    err_n_fld = 1;
	    return(NOT_VALID);
	}

	tmp = (short) atoi(read_buf);
	if (tmp < 0 || tmp > 99)
	{
	    prt_error(ERROR_N_FLD);
	    err_n_fld = 1;
	    return(NOT_VALID);
	}

	fl_ptr->itm_missing_receipts_max = tmp;
	mvprintw(fl_scr_flds[piu].prompt_row, fl_scr_flds[piu].prompt_col,
		 fl_scr_flds[piu].fmt_str, fl_ptr->itm_missing_receipts_max);

	refresh();
    }
    update_piu();

    return(VALID);
}

check_time_call(piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
    short  tmp;

    if (*field_entered) {
	clear_err_line();
	if(!numeric(read_buf))  {
	    prt_error(ERROR_N_FLD);
	    err_n_fld = 1;
	    return(NOT_VALID);
	}

	tmp = (short) atoi(read_buf);
	if (tmp < 0 || tmp > 99)
	{
	    prt_error(ERROR_N_FLD);
	    err_n_fld = 1;
	    return(NOT_VALID);
	}

	/* For some reason, this is stored as a float in the DB.
	   It is always displayed as an integer */
	fl_ptr->future_call_max = (float) tmp;
	mvprintw(fl_scr_flds[piu].prompt_row, fl_scr_flds[piu].prompt_col,
		 fl_scr_flds[piu].fmt_str, tmp);

	refresh();
    }
    update_piu();

    return(VALID);
}


/***************************************************************************
* check_y_n - check fields with values of y or n.
***************************************************************************/

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
			err_n_fld = 0;
			switch(piu)  {
				case F_ACT :
					fl_ptr->active = c;
					mvprintw(ROW_3,D_ACT_COL,"%c",fl_ptr->active);
					break;
				case F_EPOST_ACT :
					fl_ptr->enr_post_action = c;
					mvprintw(ROW_9,D_EPOST_ACT_COL+1,"%c",fl_ptr->enr_post_action);
					break;
				case F_PRSNLS :
					fl_ptr->no_personal = c;
					mvprintw(ROW_12,D_PRSNLS_COL+4,"%c",fl_ptr->no_personal);
					break;
				case F_CK_SEALS :
					fl_ptr->no_seals = c;
					mvprintw(ROW_17,D_CK_SEALS_COL+1, "%c",fl_ptr->no_seals);
					break;
				case F_H_LINE :
					fl_ptr->hold_line_flag = c;
					mvprintw(ROW_18,D_H_LINE_COL+1,"%c",fl_ptr->hold_line_flag);
					break;
				case F_GRT_LINE :
					fl_ptr->greeting_line = c;
					mvprintw(ROW_19,D_GRT_LINE_COL+1,"%c",fl_ptr->greeting_line);
					break;
				case F_MAN_MET_1 :
					fl_ptr->met_pass_call = c;
					mvprintw(ROW_17,D_MAN_MET_1_COL+1,"%c",fl_ptr->met_pass_call);
					break;
				case F_MAN_MET_2 :
					fl_ptr->met_del_call = c;
					mvprintw(ROW_18,D_MAN_MET_2_COL+1,"%c",fl_ptr->met_del_call);
					break;
				case F_MAN_MET_3 :
					fl_ptr->met_typex_call = c;
					mvprintw(ROW_19,D_MAN_MET_3_COL+1,"%c",fl_ptr->met_typex_call);
					break;
#ifdef SCANDINAVIA
				      case F_FLAT_RATE_FILTER:
					fl_ptr->FR_distance_filter = c;
					mvprintw(D_FLAT_RATE_FILTER_ROW, D_FLAT_RATE_FILTER_COL, "%c", fl_ptr->FR_distance_filter);
					break;
#else
				      case F_SEND_DEST_ADDR :
					fl_ptr->send_dest_addr = c;
					mvprintw(ROW_9, D_SEND_DEST_ADDR_COL+4, "%c", fl_ptr->send_dest_addr);
					break;


				      case F_SEND_RATE :
					fl_ptr->send_rate = c;
					mvprintw(ROW_22, D_SEND_RATE_COL+1, "%c", fl_ptr->send_rate);
					break;

				      case F_SEND_DEST_OFFER:
					fl_ptr->send_dest_offer = c;
					mvprintw(ROW_21,D_SEND_DEST_OFFER_COL+1,"%c", fl_ptr->send_dest_offer);
					break;

				      case F_SEND_PCKUP_CMT:
					fl_ptr->send_pckup_cmt = c;
					mvprintw(ROW_22,D_SEND_PCKUP_CMT_COL+1,"%c",fl_ptr->send_pckup_cmt);
					break;
#endif
				      case F_SEND_CALL_TYPE:
					fl_ptr->send_call_type = c;
					mvprintw(ROW_17,D_SEND_CALL_TYPE_COL,"%c",fl_ptr->send_call_type);
					break;
					
				      case F_GPS:
					fl_ptr->gps_state = c;
#ifndef NAMERICA
					mvprintw(ROW_20,D_GPS_COL+1,"%c",fl_ptr->gps_state);
#else
					mvprintw(ROW_23,D_GPS_COL+1,"%c",fl_ptr->gps_state);
#endif
					break;

				      case F_GPS_BOOK:
					fl_ptr->gps_book = c;
					mvprintw(ROW_21,D_C_TYPE_COL+1,"%c",fl_ptr->gps_book);
					break;

                        case F_ENROUTE_BREAK:
                          fl_ptr->enroute_break = c;
                          mvprintw(D_AUTO_REJECT_TIME_ROW+2, D_ENROUTE_BREAK_COL+1,"%c",fl_ptr->enroute_break);
                          break;

                        case F_LOAD_UNLOAD:
                          fl_ptr->load_unload = c;
                          mvprintw(D_AUTO_REJECT_TIME_ROW+3, D_LOAD_UNLOAD_COL+1,"%c",fl_ptr->load_unload);
                          break;			  

                        case F_ILINK_ACTIVE:
                          fl_ptr->ilink_active = c;
                          mvprintw(ROW_3, D_ILINK_ACTIVE_COL,"%c", fl_ptr->ilink_active);
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
		if((0 <= tmp_num) && (tmp_num <= INT_MAX))  {
			err_n_fld = 0;
			switch(piu)  {
				case F_EXC_LVL_1 :
					fl_ptr->lev1_excl_time = (short)tmp_num;
					mvprintw(ROW_4,D_EXC_LVL_1_COL,"%2d",fl_ptr->lev1_excl_time);
					break;
				case  F_N_ACPT_BT :	
					fl_ptr->no_acpt_penalty = (short)tmp_num;
					mvprintw(ROW_8,D_N_ACPT_BT_COL+1,"%5d",fl_ptr->no_acpt_penalty);
					break;
				case F_EXC_PRM :
					fl_ptr->main_excl_time = (short)tmp_num;
					mvprintw(ROW_4,D_EXC_PRM_COL,"%2d",fl_ptr->main_excl_time);
					break;
				case F_EXC_LVL_2 :
					fl_ptr->lev2_excl_time = (short)tmp_num;
					mvprintw(ROW_4,D_EXC_LVL_2_COL,"%2d",fl_ptr->lev2_excl_time);
					break;
				case F_EXC_LVL_3 :
					fl_ptr->lev3_excl_time = (short)tmp_num;
					mvprintw(ROW_4,D_EXC_LVL_3_COL,"%2d",fl_ptr->lev3_excl_time);
					break;
				case F_EXC_LVL_4 :
					fl_ptr->lev4_excl_time = (short)tmp_num;
					mvprintw(ROW_4,D_EXC_LVL_4_COL,"%2d",fl_ptr->lev4_excl_time);
					break;
				case F_EXC_LVL_5 :
					fl_ptr->lev5_excl_time = (short)tmp_num;
					mvprintw(ROW_4,D_EXC_LVL_5_COL,"%2d",fl_ptr->lev5_excl_time);
					break;
				case F_EXC_LVL_6 :
					fl_ptr->lev6_excl_time = (short)tmp_num;
					mvprintw(ROW_4,D_EXC_LVL_6_COL,"%2d",fl_ptr->lev6_excl_time);
					break;
				case F_EXC_LVL_7 :
					fl_ptr->lev7_excl_time = (short)tmp_num;
					mvprintw(ROW_4,D_EXC_LVL_7_COL,"%2d",fl_ptr->lev7_excl_time);
					break;
				case F_EXC_LVL_8 :
					fl_ptr->lev8_excl_time = (short)tmp_num;
					mvprintw(ROW_4,D_EXC_LVL_8_COL,"%2d",fl_ptr->lev8_excl_time);
					break;
				case F_POST_1 :
					fl_ptr->post_freq = (short)tmp_num;
					mvprintw(ROW_7,D_POST_1_COL+1,"%5d",fl_ptr->post_freq);
					break;
				case F_CPOST :
					fl_ptr->cond_post_freq = (short)tmp_num;
					mvprintw(ROW_8,D_CPOST_COL+1,"%5d",fl_ptr->cond_post_freq);
					break;
				case F_INQRY_2 :
					fl_ptr->zone_inqry_freq = (short)tmp_num;
					mvprintw(ROW_9,D_INQRY_2_COL+1,"%5d",fl_ptr->zone_inqry_freq);
					break;
				case F_SRCH :
					fl_ptr->zone_info_freq = (short)tmp_num;
					mvprintw(ROW_10,D_SRCH_COL+1,"%5d",fl_ptr->zone_info_freq);
					break;
				case F_INFO :
					fl_ptr->veh_inqry_freq = (short)tmp_num;
					mvprintw(ROW_11,D_INFO_COL+1,"%5d",fl_ptr->veh_inqry_freq);
					break;
				case F_ZN_STAT :
					fl_ptr->zone_stat_freq = (short)tmp_num;
					mvprintw(ROW_12,D_ZN_STAT_COL+1,"%5d",fl_ptr->zone_stat_freq);
					break;
				case F_TIME_CALL :
					fl_ptr->time_call_freq = (short)tmp_num;
					mvprintw(ROW_13,D_TM_CALL_COL+1,"%5d",fl_ptr->time_call_freq);
					break;
				case F_WAKEUP_LTM :
					fl_ptr->wakeup_lead_time = (short)tmp_num;
					mvprintw(fl_scr_flds[piu].prompt_row,fl_scr_flds[piu].prompt_col,"%5d",
						fl_ptr->wakeup_lead_time);
					break;
				case F_ACPT :
					fl_ptr->call_accept_per = (short)tmp_num;
					mvprintw(ROW_7,D_ACPT_COL+1,"%5d",fl_ptr->call_accept_per);
					break;
				case F_BREAK :
					fl_ptr->break_duration = (short)tmp_num;
					mvprintw(ROW_8,D_BREAK_COL+1,"%5d",fl_ptr->break_duration);
					break;
				case F_SH_METER :
					fl_ptr->short_meter_dur = (short)tmp_num;
					mvprintw(ROW_9,D_SH_METER_COL+1,"%5d",fl_ptr->short_meter_dur);
					break;
				case F_M_ACTION_TIMER :
					fl_ptr->m_action_timer = (short)tmp_num;
					mvprintw(ROW_10,D_M_ACTION_TIMER_COL+1,"%5d",fl_ptr->m_action_timer);
					break;
				case F_LOCAL_DUR :
					fl_ptr->local_calls_dur = (short)tmp_num;
					mvprintw(ROW_11,D_LOCAL_DUR_COL+1,"%5d",fl_ptr->local_calls_dur);
					break;
				case F_C_MATCH :
					fl_ptr->avl_call_q_time = (short)tmp_num;
					mvprintw(ROW_12,D_C_MATCH_COL+1,"%5d",fl_ptr->avl_call_q_time);
					break;
			case F_TC_INT:
			  fl_ptr->cbook_time = (short)tmp_num;
			  mvprintw(ROW_15,D_TC_INT_COL+1,"%5d",fl_ptr->cbook_time);
			  break;

				      case F_CANX_TO_MOFF:
					fl_ptr->canx_to_moff_time = (short)tmp_num;
					mvprintw(ROW_13,D_CANX_TO_MOFF_COL+1,"%5d",fl_ptr->canx_to_moff_time);
					break;
				case F_EPOST_LT :
					fl_ptr->enr_post_lead = (short)tmp_num;
					mvprintw(ROW_11,D_EPOST_LT_COL+4,"%5d",fl_ptr->enr_post_lead);
					break;
				case F_TIME_C :
					fl_ptr->future_call_min = (short)tmp_num;
					mvprintw(ROW_12,D_TIME_C_COL+4,"%5d",fl_ptr->future_call_min);
					break;
				case F_LATE_M :
					fl_ptr->late_meter = (short)tmp_num;
					mvprintw(ROW_13,D_LATE_M_COL+4,"%5d",fl_ptr->late_meter);
					break;
				case F_N_KDT :
					fl_ptr->pers_lead_tm = (short)tmp_num;
					mvprintw(ROW_12,D_N_KDT_COL+1,"%5d",fl_ptr->pers_lead_tm);
					break;
				case F_TM_C_PRC :
					fl_ptr->time_call_max = (short)tmp_num;
					mvprintw(ROW_13,D_TM_C_PRC_COL+1,"%5d",fl_ptr->time_call_max);
					break;
				case F_T_CL_RPT :
					fl_ptr->time_calls_freq = (short)tmp_num;
					mvprintw(ROW_3,D_T_CL_RPT_COL+1,"%5d",fl_ptr->time_calls_freq);
					break;
				case F_MSG_RPT :
					fl_ptr->msg_rpt_freq = (short)tmp_num;
					mvprintw(ROW_4,D_MSG_RPT_COL+1,"%5d",fl_ptr->msg_rpt_freq);
					break;
				case F_H_SRCH :
					fl_ptr->hist_search_def = (short)tmp_num;
					mvprintw(ROW_13,D_H_SRCH_COL+1,"%5d",fl_ptr->hist_search_def);
					break;
				case F_1ST_EST :
					fl_ptr->range_limit1 = (short)tmp_num;
					mvprintw(ROW_10,D_1ST_EST_COL+1,"%5d",fl_ptr->range_limit1);
					break;
				case F_2ND_EST :
					fl_ptr->range_limit2 = (short)tmp_num;
					mvprintw(ROW_11,D_2ND_EST_COL+1,"%5d",fl_ptr->range_limit2);
					break;
				case F_3RD_EST :
					fl_ptr->range_limit3 = (short)tmp_num;
					mvprintw(ROW_12,D_3RD_EST_COL+1,"%5d",fl_ptr->range_limit3);
					break;
				case F_4TH_EST :
					fl_ptr->range_limit4 = (short)tmp_num;
					mvprintw(ROW_13,D_4TH_EST_COL+1,"%5d",fl_ptr->range_limit4);
					break;
				case F_5TH_EST :
					fl_ptr->range_limit5 = (short)tmp_num;
					mvprintw(ROW_14,D_5TH_EST_COL+1,"%5d",fl_ptr->range_limit5);
					break;
				case F_6TH_EST :
					fl_ptr->range_limit6 = (short)tmp_num;
					mvprintw(ROW_15,D_6TH_EST_COL+1,"%5d",fl_ptr->range_limit6);
					break;
				case F_7TH_EST :
					fl_ptr->range_limit7 = (short)tmp_num;
					mvprintw(ROW_16,D_7TH_EST_COL+1,"%5d",fl_ptr->range_limit7);
					break;
				case F_8TH_EST :
					fl_ptr->range_limit8 = (short)tmp_num;
					mvprintw(ROW_17,D_8TH_EST_COL+1,"%5d",fl_ptr->range_limit8);
					break;

				case F_INACTIVE_RETRY_INTERVAL:
					fl_ptr->inactive_retry_interval = (short)tmp_num;
					mvprintw(fl_scr_flds[piu].prompt_row, fl_scr_flds[piu].prompt_col,
						 fl_scr_flds[piu].fmt_str, fl_ptr->inactive_retry_interval);
					break;
					
				case F_INACTIVE_TIME_OUT_PERIOD:
					fl_ptr->inactive_time_out_period = (short)tmp_num;
					mvprintw(fl_scr_flds[piu].prompt_row, fl_scr_flds[piu].prompt_col,
						 fl_scr_flds[piu].fmt_str, fl_ptr->inactive_time_out_period);
					break;

				case F_STATS_COMPUTE_FREQ:
					fl_ptr->stats_compute_freq = (short)tmp_num;
					mvprintw(fl_scr_flds[piu].prompt_row, fl_scr_flds[piu].prompt_col,
						 fl_scr_flds[piu].fmt_str, fl_ptr->stats_compute_freq);
					break;

				case F_FLG_TIME :
					fl_ptr->auto_flag_time = (short)tmp_num;
					mvprintw(ROW_21,D_FLG_TIME_COL+1,"%3d",fl_ptr->auto_flag_time);
					break;

                                case F_TIME_CLS_INT_REQ:
                                        fl_ptr->time_int_req = (short)tmp_num;
                                        mvprintw(ROW_14,D_TIME_CLS_INT_REQ_COL+1,"%2d",fl_ptr->time_int_req);
                                        break;

#ifdef NAMERICA
				      case F_UNASSGND_LK_AHD:
					fl_ptr->unassgnd_look_ahead = (short)tmp_num;
					mvprintw(ROW_20,D_UNASSGND_LK_AHD+1,"%3d",fl_ptr->unassgnd_look_ahead);
					break;
#endif

				      case F_TIME_CLS_LEAD_SUPP:
					fl_ptr->time_calls_lead_supp = (short)tmp_num;
					mvprintw(ROW_15,D_TIME_CLS_LEAD_SUPP_COL+1, "%2d", fl_ptr->time_calls_lead_supp);
					break;

				      case F_GPS_TIME:
					fl_ptr->gps_time = (short)tmp_num;
					mvprintw(ROW_21, D_GPS_COL-1, "%3d", fl_ptr->gps_time);
					break;

                        case F_EMERG_RANGE:
                          fl_ptr->emerg_range = (short)tmp_num;
                          mvprintw(ROW_22, D_EMERG_RANGE_COL+1, "%5d", fl_ptr->emerg_range);
                          break;

				      case F_GPS_DISTANCE_CEILING:
					fl_ptr->gps_distance_ceiling = (int)tmp_num;
					mvprintw(ROW_19, D_CITY_COL+1, "%5d", fl_ptr->gps_distance_ceiling);
					break;

				      case F_AUTO_REJECT_TIME:
					fl_ptr->auto_reject_time = (int)tmp_num;
					mvprintw(ROW_18, D_AUTO_REJECT_TIME_COL, "%3d", fl_ptr->auto_reject_time);
					break;

                        case F_ILINK_TIMEOUT:
                          fl_ptr->ilink_timeout = (int)tmp_num;
                          mvprintw(ROW_4, D_ILINK_TIMEOUT_COL, "%3d", fl_ptr->ilink_timeout);
                          break;
					
			        default:
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

int
check_0or1 (piu_ptr, read_buf, key_val, field_entered)
     int *piu_ptr;	/* pointer to the number of the field read in */
     char *read_buf; /* pointer to the buffer in which the field was read in */
     int key_val;    /* value of the key pressed or entered */
     int *field_entered; /* indicates if data was entered or not */
{
  
  if (*field_entered)
    {
      clear_err_line();
      if (read_buf[0] == '0'  ||
	  read_buf[0] == '1')
	{
	  err_n_fld = 0;
	  switch(piu)
	    {
	      
	    default:
	      break;
	    }
	  refresh();
	  update_piu();
	  return(VALID);
	}
      else  
	{
	  prt_error(ERROR_N_FLD);
	  err_n_fld = 1;
	  return(NOT_VALID);
	}
    }
  else 
    {
      if(!err_n_fld)
	update_piu();
      return(VALID);
      
    }
} /* End of check_0or1()  */

int
check_0to4(piu_ptr, read_buf, key_val, field_entered)
     int *piu_ptr;	/* pointer to the number of the field read in */
     char *read_buf; /* pointer to the buffer in which the field was read in */
     int key_val;    /* value of the key pressed or entered */
     int *field_entered; /* indicates if data was entered or not */
{
  
  if (*field_entered)
    {
      clear_err_line();
      if (read_buf[0] >= '0' &&
	  read_buf[0] <= '4')
	{
	  err_n_fld = 0;
	  switch(piu)
	    {
	    case F_CALL_OFFER:
	      fl_ptr->cal_offer_info = read_buf[0];
	      mvprintw(fl_scr_flds[piu].prompt_row, fl_scr_flds[piu].prompt_col,
		       fl_scr_flds[piu].fmt_str, fl_ptr->cal_offer_info);
	      break;

	    case F_CALL_ASSIGN:
	      fl_ptr->cal_assign_info = read_buf[0];
	      mvprintw(fl_scr_flds[piu].prompt_row, fl_scr_flds[piu].prompt_col,
		       fl_scr_flds[piu].fmt_str, fl_ptr->cal_assign_info);
	      break;	      
	      
	    default:
	      break;
	    }
	  refresh();
	  update_piu();
	  return(VALID);
	}
      else  
	{
	  prt_error(ERROR_N_FLD);
	  err_n_fld = 1;
	  return(NOT_VALID);
	}
    }
  else 
    {
      if(!err_n_fld)
	update_piu();
      return(VALID);
      
    }
} /* End of check_0to4()  */

/****************************************************************************
* check_1to12 - check the fields with values of 1 to 12
****************************************************************************/

int check_1to12 (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int value;
	
	if(*field_entered)  {
		clear_err_line();
		read_buf[2] = '\0';
		if(!numeric(read_buf))  {
			prt_error(ERROR_N_FLD);
			err_n_fld = 1;
			return(NOT_VALID);
		}
		value = atoi(read_buf);
		if((1 <= value) && (value <= 12))  {
		  fl_ptr->time_int_req = (short)value;
		  mvprintw(ROW_14,D_TIME_CLS_INT_REQ_COL+1,"%2d",fl_ptr->time_int_req);
		  err_n_fld = 0;
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
} /* End of check_1to12()  */

/******************************************************************************
* update_piu - update prompt in use variable.
*******************************************************************************/

void update_piu()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering update_piu\n");
	TRACE(trace_str);
#endif
	if(key_val == '\t' ||
	   key_val == '\n' ||
	   key_val == KEY_DOWN ||
	   key_val == VALID)
		if(doing_add)  {
			if(piu == F_SCR_1_MAX)  {
				scr_nbr = 2;
				clear();
				prt_labels();
				prt_data();
				piu = F_SCR_2_MIN;
			}
			else if(piu == F_SCR_2_MAX)  {
				scr_nbr = 1;
				clear();
				prt_labels();
				prt_data();
				piu = F_SCR_1_MIN;
			}
			else 
				piu += 1;

		}
		else   {
			if(piu == F_SCR_1_MAX)  {
				scr_nbr = 2;
				clear();
				prt_upd_hdr();
				prt_labels();
				prt_data();
				piu = F_SCR_2_MIN;
			}
			else if(piu == F_SCR_2_MAX)  {
				scr_nbr = 1;
				clear();
				prt_upd_hdr();
				prt_labels();
				prt_data();
				piu = F_ACT;
			}
			else
				piu += 1;
		}
	if( key_val == KEY_UP)  {
		if(piu == F_SCR_2_MIN)  {
			scr_nbr = 1;
			clear();
			prt_upd_hdr();
			prt_labels();
			prt_data();
			piu = F_R_CALC_2;
		}
		else if(piu == F_ACT)  {
			scr_nbr = 2;
			clear();
			prt_upd_hdr();
			prt_labels();
			prt_data();
			piu = F_SCR_2_MAX;
		}
		else
			piu -= 1;
	}
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

int check_float( piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	double tmp_num;

	if(*field_entered)  {
		clear_err_line();

		tmp_num = atof(read_buf);
		if(( tmp_num >= -999999999.9) && (tmp_num <= 999999999.9))  {
			err_n_fld = 0;
			switch(piu)  {
			case F_GPS_MAX_X:
			  fl_ptr->gps_max_x = tmp_num;
			  mvprintw( ROW_15, D_GPS_MAXMIN_COL, "%10.2f", fl_ptr->gps_max_x);
			  break;
			case F_GPS_MIN_X:
			  fl_ptr->gps_min_x = tmp_num;
			  mvprintw( ROW_16, D_GPS_MAXMIN_COL, "%10.2f", fl_ptr->gps_min_x);
			  break;
			case F_GPS_MAX_Y:
			  fl_ptr->gps_max_y = tmp_num;
			  mvprintw( ROW_17, D_GPS_MAXMIN_COL, "%10.2f", fl_ptr->gps_max_y);
			  break;
			case F_GPS_MIN_Y:
			  fl_ptr->gps_min_y = tmp_num;
			  mvprintw( ROW_18, D_GPS_MAXMIN_COL, "%10.2f", fl_ptr->gps_min_y);
			  break;
			default:
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

}  /* End of check_float */	
			  
