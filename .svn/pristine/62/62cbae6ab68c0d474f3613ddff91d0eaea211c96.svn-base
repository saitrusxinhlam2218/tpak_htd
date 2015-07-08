static char sz__FILE__[]="@(#)zn_ck_fld.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  zn_ck_fld.c; Rel: 6.6.0.0; Get date: 11/12/91 at 13:39:12
* @(#)  Last delta: 5/21/91 at 17:19:07
* @(#)  SCCS File: /taxi/sccs/s.zn_ck_fld.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
** ***************************************************************************/

#include <stdlib.h>
#include <cursesX.h>
#include <ctype.h>
#include <sys/types.h>
#include <string.h>

#include "zn_m_strdefs.h"
#include "mad_error.h"
#include "df_maint.h"
#include "ex_key.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "zn_maint.h"
#include "zn_msg.h"
#include "language.h"

extern short ipc_required;		/* Set if a variable which resides */
					/* shared memory changes */
extern struct cisam_zn *zn_ptr;		/* pointer to zone file structure */
extern struct cisam_fl *fl_ptr;		/* pointer to fleet file structure */
extern struct cisam_zn zn_buf;		/* pointer to zone file structure */
extern struct cisam_zn *temp_zn_ptr;		/* pointer to zone file structure */
extern char read_buf[];			/* buffer for reading fields */
extern char req_buf[];			/* buffer for reading fields */
extern FILE *trace_fp;			/* trace file pointer */
extern char trace_str[];		/* trace string */
extern int  query;
extern int  skip_seals;
extern int  doing_add;
extern int  error_flag;
extern int piu;				/* prompt in use */
int   number;
extern int key_val;			/* value of key entered */
extern prt_error();
extern clear_err_line();
extern int err_n_fld;
extern int num_ber;
int update_piu();
int valid_cmd();
char *str_to_upper();
int  temp_nbr;
extern int nbr_flag;
extern int fl_flag;
extern int temp_flag;
extern int scr_nbr;
int    range_high = 0, range_low = 0;
struct cisam_zn global_zone_rec;

int 	check_dummy(),check_fleet(),check_zone_nbr(),check_desc(),check_type(),check_spz_detl();
int	check_spz_nbr(),check_bid_alw(),check_max_time_calls(),check_mx_dsp_tm(),check_pkup_l_tm(),check_st_tm_frm(), check_cbook_time();
int 	check_st_tm_to(),check_ep_alwd(),check_pkup_seals(),check_alg(),check_etm();
int 	check_zone(),check_alg(), check_mdt_screen();
int	check_enr_post_lead();
int	check_break_duration();
int	check_accept_duration();
int     check_avail_taxis();
int     check_lat_long();
int     check_radius();
int     check_gps_dist_ceiling();
int     check_match_algorithm();
int     check_stand_tele();
int     check_auto_inqry();
/*  Declare an array of pointers to functions which check the field entered
 *  for validity.  
 */




int (*check_field[])() = { check_dummy, check_fleet, check_zone_nbr, 
			   check_desc, check_type, check_spz_detl,
			   check_spz_nbr, check_bid_alw, check_mdt_screen, check_max_time_calls, check_max_time_calls,
			   check_mx_dsp_tm,
			   check_pkup_l_tm, check_pkup_l_tm, check_st_tm_frm, check_st_tm_to,
			   check_ep_alwd, check_enr_post_lead, check_max_time_calls, check_cbook_time, check_pkup_seals,check_pkup_seals,
		           check_pkup_seals,check_pkup_seals,check_pkup_seals,		
 			   check_pkup_seals,check_pkup_seals,check_pkup_seals,
			   check_pkup_seals,check_pkup_seals,check_pkup_seals,
			   check_pkup_seals,check_pkup_seals,check_pkup_seals,
			   check_pkup_seals,check_pkup_seals,check_break_duration,check_accept_duration,check_avail_taxis,
			   check_max_time_calls,
			   check_lat_long,
			   check_lat_long,
			   check_radius,
			   check_gps_dist_ceiling,
                           check_radius,
			   check_alg,
			   check_etm, check_alg, check_zone, check_zone,
			   check_zone, check_zone, check_zone,
			   check_zone, check_zone, check_zone, 
			   check_match_algorithm,check_etm, check_alg, check_alg,check_etm,check_alg,
			   check_zone, check_zone,
			   check_zone, check_zone, check_zone,
			   check_zone, check_zone, check_zone, check_alg,
			   check_etm, check_alg, check_zone, check_zone,
			   check_zone, check_zone, check_zone,
			   check_zone, check_zone, check_zone, check_alg,
			   check_etm, check_alg, check_zone, check_zone,
			   check_zone, check_zone, check_zone,
			   check_zone, check_zone, check_zone, check_alg,
			   check_etm, check_alg, check_zone, check_zone,
			   check_zone, check_zone, check_zone,
			   check_zone, check_zone, check_zone, check_alg,
			   check_etm, check_alg, check_zone, check_zone,
			   check_zone, check_zone, check_zone,
			   check_zone, check_zone, check_zone, check_alg,
			   check_etm, check_alg, check_zone, check_zone,
			   check_zone, check_zone, check_zone,
			   check_zone, check_zone, check_zone, check_alg,
			   check_etm, check_alg, check_zone, check_zone,
			   check_zone, check_zone, check_zone,
			   check_zone, check_zone, check_zone, check_stand_tele, check_max_time_calls,
                           check_max_time_calls, check_zone_nbr,
			   check_zone_nbr,check_zone_nbr,check_zone_nbr,check_zone_nbr,check_zone_nbr,
			   check_zone_nbr,check_zone_nbr,check_zone_nbr,check_zone_nbr,check_zone_nbr };





/*******************************************************************************
 * check_dummy - this routine is simply a place holder so the first field is 1.
 ********************************************************************************/

int check_dummy (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

/*******************************************************************************
 * check_fleet - check fleet field.
 ********************************************************************************/

int check_fleet (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_fleet\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			if (!isalpha(read_buf[0]))	{
				prt_error(ERROR_N_FLD);
				error_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_fleet - 1\n");
	TRACE(trace_str);
#endif
				return(error_flag);
			}
			fl_ptr->id = read_buf[0];
			if ((db(FLEET_FILE_ID,READ_KEYS,&fl_key1,ISEQUAL,0)) < 0)	{
				prt_error(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_1, "Invalid fleet ID"));
				error_flag = 0;
				return(error_flag);
			}
			ipc_required = 1;
			error_flag = 1;
			fl_flag = 1;
			if (fl_ptr->no_seals == YES)
			  skip_seals = 1;
			else
			  skip_seals = 0;
			clear_err_line();
			zn_ptr->fleet = read_buf[0];
			mvprintw(D_FLEET_ROW,D_FLEET_COL,"%c",zn_ptr->fleet);
			refresh();
			update_piu();
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_fleet - 2\n");
	TRACE(trace_str);
#endif
	}
	else	{
		if (error_flag) 	{
			fl_ptr->id = zn_ptr->fleet;
			if ((db(FLEET_FILE_ID,READ_KEYS,&fl_key1,ISEQUAL,0)) < 0)	{
				prt_error(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_2, "You must enter fleet ID"));
				error_flag = 0;
				return(error_flag);
			}
			if (fl_ptr->no_seals == YES)
			  skip_seals = 1;
			else
			  skip_seals = 0;
			update_piu();
		}
		else
			prt_error(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_2, "You must enter fleet ID"));
	}
        return(error_flag);
}


/*******************************************************************************
 * check_zone_nbr - check zone_nbr field (1 - 999 or 0-32000).
 ********************************************************************************/


int check_zone_nbr (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{

  BOOLEAN     range_1_selected = FALSE;
  BOOLEAN     range_2_selected = FALSE;
  char        zn_msg_buf[132];

  range_high = range_low = 0;
  clear_zn_range();
  
	if(*field_entered)
	  {
	    clear_err_line();
	    if ( read_buf[2] == '*' )
	      {
		range_2_selected = TRUE;
		if ( read_buf[1] == '*' )
		  {
		    read_buf[1] = '0';
		    read_buf[2] = '0';
		    range_low = atoi(read_buf);
		    read_buf[1] = '9';
		    read_buf[2] = '9';
		    range_high = atoi(read_buf);
		    read_buf[1] = '0';
		    read_buf[2] = '0';
		  }
		else
		  {
		    read_buf[2] = '0';
		    range_low = atoi(read_buf);
		    read_buf[2] = '9';
		    range_high = atoi(read_buf);
		    read_buf[2] = '0';
		  }
	      }
	    if (!numeric(read_buf))
	      {
		prt_error(ERROR_N_FLD);
		error_flag = 0;
		return;
	      }
	    number = atoi(read_buf);
	    if ((0 <= number) && (number <= 32000))
	      {
		ipc_required = 1;
		error_flag = 1;
		clear_err_line();
		switch (piu) {
		  
		case F_ZONE:
		  zn_ptr->nbr = (short)number;	/* move command number to isam buffer */ 
		  mvprintw(D_FLEET_ROW,D_ZONE_COL,"%5d",zn_ptr->nbr);
		  if ( range_low > 0 )
		    {
		      sprintf( zn_msg_buf, "%s %d --> %d",
			       catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_88, "Zones Selected: "),
			       range_low, range_high );
		      prt_zn_range( zn_msg_buf );
		    }
		  break;
		  
		case F_STAND_NBR:
		  zn_ptr->stand_nbr = (short)number;
		  if ( number >= 0)
		    mvprintw(D_FLEET_ROW+2, D_STAND_NBR_COL, "%3d",
			     zn_ptr->stand_nbr);
		  else
		    mvprintw(D_FLEET_ROW+2, D_STAND_NBR_COL, "   ");
		  break;

		case F_AUTO_INQRY_1:
		  zn_ptr->inquiry_backup[0] = (short)number;
		  if ( number >= 0)
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL, "%3d",
			     zn_ptr->inquiry_backup[0]);
		  else
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL, "   ");
		  break;
			     
		case F_AUTO_INQRY_2:
		  zn_ptr->inquiry_backup[1] = (short)number;
		  if ( number >= 0)
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+6, "%3d",
			     zn_ptr->inquiry_backup[1]);
		  else
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+6, "   ");
		  break;		  
		case F_AUTO_INQRY_3:
		  zn_ptr->inquiry_backup[2] = (short)number;
		  if ( number >= 0)
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+12, "%3d",
			     zn_ptr->inquiry_backup[2]);
		  else
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+12, "   ");
		  break;		  
		case F_AUTO_INQRY_4:
		  zn_ptr->inquiry_backup[3] = (short)number;
		  if ( number >= 0)
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+18, "%3d",
			     zn_ptr->inquiry_backup[3]);
		  else
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+18, "   ");
		  break;		  
		case F_AUTO_INQRY_5:
		  zn_ptr->inquiry_backup[4] = (short)number;
		  if ( number >= 0)
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+24, "%3d",
			     zn_ptr->inquiry_backup[4]);
		  else
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+24, "   ");
		  break;		  
		case F_AUTO_INQRY_6:
		  zn_ptr->inquiry_backup[5] = (short)number;
		  if ( number >= 0)
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+30, "%3d",
			     zn_ptr->inquiry_backup[5]);
		  else
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+30, "   ");
		  break;		  
		case F_AUTO_INQRY_7:
		  zn_ptr->inquiry_backup[6] = (short)number;
		  if ( number >= 0)
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+36, "%3d",
			     zn_ptr->inquiry_backup[6]);
		  else
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+36, "   ");
		  break;		  
		case F_AUTO_INQRY_8:
		  zn_ptr->inquiry_backup[7] = (short)number;
		  if ( number >= 0)
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+42, "%3d",
			     zn_ptr->inquiry_backup[7]);
		  else
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+42, "   ");
		  break;		  
		case F_AUTO_INQRY_9:
		  zn_ptr->inquiry_backup[8] = (short)number;
		  if ( number >= 0)
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+48, "%3d",
			     zn_ptr->inquiry_backup[8]);
		  else
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+48, "   ");
		  break;		  
		case F_AUTO_INQRY_10:
		  zn_ptr->inquiry_backup[9] = (short)number;
		  if ( number >= 0)
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+54, "%3d",
			     zn_ptr->inquiry_backup[9]);
		  else
		    mvprintw(D_FLEET_ROW+4, D_AUTO_INQRY_COL+54, "   ");
		  break;		  

		default:
		  break;
		}
		  
		refresh();
		nbr_flag = 1;
		update_piu();
	      }
	    else
	      {
		nbr_flag = 0;
		error_flag = 0;
		prt_error(ERROR_N_FLD);
                return(0);
	      }
	}
	else	{
			clear_err_line();
		if (error_flag)	
			update_piu();
		else
                  {
                    prt_error(ERROR_N_FLD);
                    return(0);
                  }
	}
        return(1);
}
/*******************************************************************************
 * check zone description field
 ******************************************************************************/

int check_desc (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_desc\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			ipc_required = 1;
			clear_err_line();
			strcpy(zn_ptr->desc, str_to_upper(read_buf));
			mvprintw(D_FLEET_ROW,D_DESC_COL,"%s",zn_ptr->desc);
			refresh();
			update_piu();
			error_flag = 1;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_desc - 2\n");
	TRACE(trace_str);
#endif
	}
	else	{
		if (error_flag)
			update_piu();
	}
}

int check_stand_tele (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{

	if(*field_entered)	{
			clear_err_line();
			ipc_required = 1;
			clear_err_line();
			strcpy(zn_ptr->stand_telephone, str_to_upper(read_buf));
			mvprintw(D_FLEET_ROW+2,D_STAND_TELE_COL,"%s",zn_ptr->stand_telephone);
			refresh();
			update_piu();
			error_flag = 1;
	}
	else	{
		if (error_flag)
			update_piu();
	}
}

/*******************************************************************************
 * check zone type field
 ******************************************************************************/

int check_type (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	if(*field_entered)
	{
		piu = F_TYPE;
		clear_err_line();
		if (read_buf[0] == '1' ||
	            read_buf[0] == '2' ||
		    read_buf[0] == '3' ||
		    read_buf[0] == '4')
	 	{
			ipc_required = 1;
			error_flag = 1;
			zn_ptr->type = read_buf[0];
			mvprintw(D_FLEET_ROW,D_TYPE_COL,"%c",zn_ptr->type);
			refresh();
		}
		else
		{
			clear_err_line();
			prt_error(ERROR_N_FLD);
			error_flag = 0;
		}
		if ((read_buf[0] == '1' || read_buf[0] == '2' || read_buf[0] == '4') && (key_val != KEY_UP))
		{
			temp_flag = 1;
			if ((1 <= zn_ptr->nbr) && (zn_ptr->nbr <= 999))	
				piu = F_SPZ_NBR;
			else
			{
				prt_error(ERROR_N_FLD);
				piu = F_ZONE;
			}
		}
		if (read_buf[0] == '3')
		{
			temp_flag = 0;
			update_piu();
		}
	}
	else
	{
		if (error_flag)
		{
			if ((temp_flag) && ((key_val == KEY_DOWN) || (key_val == '\r') || (key_val == '\n')))
				piu = F_SPZ_NBR;
			else
				update_piu();
		}
	}			
}

/*******************************************************************************
 * check super zone detail field
 ******************************************************************************/
int check_spz_detl (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{

    char  c;

#ifdef	DEBUG
	sprintf(trace_str,"Entering check_spz_detl\n");
	TRACE(trace_str);
#endif

        c = Toupper(read_buf[0]);

	if(*field_entered)	{
		clear_err_line();

		if (c == YES || c == NO) {
			ipc_required = 1;
			error_flag = 1;
			zn_ptr->superzn_detail = c;
			mvprintw(D_FLEET_ROW,D_SPZ_DETL_COL,"%c",zn_ptr->superzn_detail);
			refresh();
			update_piu();
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_spz_detl - 1\n");
	TRACE(trace_str);
#endif
			}
			else	{
				clear_err_line();		
				prt_error(ERROR_N_FLD);
				error_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_spz_detl - 2\n");
	TRACE(trace_str);
#endif
				return;
			}
		}
		else	{
			if (error_flag)
				update_piu();
		}
			
}

/*******************************************************************************
 * check super zone number  field. ( 0 - 32,000)
 ******************************************************************************/
int check_spz_nbr (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
short   superzn_nbr;

#ifdef	DEBUG
	sprintf(trace_str,"Entering check_spz_nbr\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();

			if (!numeric(read_buf))	{
				prt_error(ERROR_N_FLD);
				error_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_spz_nbr - 1\n");
	TRACE(trace_str);
#endif
				return;
			}
		superzn_nbr = atoi(read_buf);	/* move super zone number into local */
		if ((0 <= superzn_nbr) && (superzn_nbr <= 32000))	{
			ipc_required = 1;
			error_flag = 1;
			clear_err_line();
			zn_ptr->superzn_nbr = (short)superzn_nbr;	/* move command number to isam buffer */ 
			mvprintw(D_FLEET_ROW,D_SPZ_NBR_COL,"%5d",zn_ptr->superzn_nbr);
			refresh();
			update_piu();
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_spz_nbr - 2\n");
	TRACE(trace_str);
#endif
			}
			else	{
				prt_error(ERROR_N_FLD);
				error_flag = 0; 
			}
		}
		else	{
			if (error_flag)	{
				if ((temp_flag) && (key_val == KEY_UP))
					piu = F_TYPE;
				else
					update_piu();
			}
		}

}

/*******************************************************************************
 * check bid_allow field. 
 ******************************************************************************/
int check_bid_alw (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
    char c;

#ifdef	DEBUG
	sprintf(trace_str,"Entering check_bid_alw\n");
	TRACE(trace_str);
#endif
       c = Toupper(read_buf[0]);

	if(*field_entered)	{
	        clear_err_line();
		if (c == YES || c == NO) {
			ipc_required = 1;
			error_flag = 1;
			zn_ptr->bid_allowed = c;
			mvprintw(D_FLEET_ROW,D_BID_ALW_COL,"%c",zn_ptr->bid_allowed);
			refresh();
			update_piu();
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_bid_alw - 1\n");
	TRACE(trace_str);
#endif
			}
			else	{
				clear_err_line();
				prt_error(ERROR_N_FLD);
				error_flag = 0;
				return;
			}
		}
		else	{
			if (error_flag)	
				update_piu();
		}
			
}


/*-----------------------------------------------------------------
 * check max time call field. 
 *
 *		fix for restricted time calls
 *-----------------------------------------------------------------
 */

check_max_time_calls(piu_ptr, read_buf, key_val, field_entered)
	int			*piu_ptr;	
	char		*read_buf;
	int 		 key_val;  
	int 		*field_entered;
{
    short		 short_val = 0;


	if(*field_entered)
	  {
		clear_err_line();
		if (!blank(read_buf))
		{
		  short_val = atoi(read_buf);
		} else
		  short_val = -1;

		if ((-1 <= short_val) && (short_val <= 999))
		  {
			ipc_required = 1;
			error_flag = 1;
			clear_err_line();
			switch (piu ) {
			case F_MAX_TIME_CALLS:
			    zn_ptr->max_time_calls = (short)short_val;
			    
			    if ( short_val >= -1 )
				mvprintw(D_FLEET_ROW, D_MAX_TIME_CALLS_COL,"%3d",
					 zn_ptr->max_time_calls);
			    else
				mvprintw(D_FLEET_ROW, D_MAX_TIME_CALLS_COL,"   ");
			    break;

			case F_TC_REST_INT:
			    zn_ptr->tc_restrict_interval = (short)short_val;
			    if ( short_val >= 0 )
				mvprintw(D_FLEET_ROW, D_TC_REST_INT_COL,"%3d",
					 zn_ptr->tc_restrict_interval);
			    else
				mvprintw(D_FLEET_ROW, D_TC_REST_INT_COL,"   ");
			    break;

                        case F_HTTP_TIMER:
                          zn_ptr->stand_hold_timer = (short)short_val;
                          if ( short_val >= 0 )
                            mvprintw(D_FLEET_ROW+2, D_HTTP_TIMER_COL, "%3d",
                                     zn_ptr->stand_hold_timer);
                          else
                            mvprintw(D_FLEET_ROW+2, D_HTTP_TIMER_COL, "   ");
                          break;


			  
			case F_ROTI:
			  zn_ptr->roti_time = (short)short_val;
			  if ( short_val >= 0 )
			    mvprintw(D_FLEET_ROW+2, D_ROTI_COL, "%3d",
				     zn_ptr->roti_time);
			  else
			    mvprintw(D_FLEET_ROW+2, D_ROTI_COL, "   ");
			  break;

			case F_TC_LOOK_AHEAD:
			    zn_ptr->tc_look_ahead = (short)short_val;
			    if ( short_val >= 0 )
				mvprintw(D_EBOOK_LEAD_TM_ROW, D_TC_LOOK_AHEAD_COL,"%3d",
					 zn_ptr->tc_look_ahead);
			    else
				mvprintw(D_EBOOK_LEAD_TM_ROW, D_TC_LOOK_AHEAD_COL,"   ");
			    break;

			case F_NS_SHORT:
			    zn_ptr->ns_short = (short)short_val;
			    if ( short_val >= 0 )
				mvprintw(D_CHECK_TAXIS_ROW, D_NS_SHORT_COL,"%3d",
					 zn_ptr->ns_short);
			    else
				mvprintw(D_CHECK_TAXIS_ROW, D_NS_SHORT_COL,"   ");
			    break;			    
			}
			
			refresh();
			update_piu();

		  }
		else
		  {
			prt_error(ERROR_N_FLD);
			error_flag = 0;
		  }
	  }
	else
	  {
		if (error_flag)
		update_piu();
	  }
}




/*******************************************************************************
 * check avl_call_q_time field. 
 ******************************************************************************/
int check_mx_dsp_tm (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
short  avl_call_q_time;

#ifdef	DEBUG
	sprintf(trace_str,"Entering check_mx_dsp_tm\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			if (!numeric(read_buf))	{
				prt_error(ERROR_N_FLD);
				error_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_mx_dsp_tm- 1\n");
	TRACE(trace_str);
#endif
				return;
			}
			avl_call_q_time = atoi(read_buf);	/* move super zone number into local */
			if ((0 <= avl_call_q_time) && (avl_call_q_time <= 32000))	{
			ipc_required = 1;
			error_flag = 1;
			clear_err_line();
			zn_ptr->avl_call_q_time= (short)avl_call_q_time;	 
			mvprintw(D_MX_DSP_TM_ROW,D_MX_DSP_TM_COL,"%5d",zn_ptr->avl_call_q_time);
			refresh();
			update_piu();
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_mx_dsp_tm- 2\n");
	TRACE(trace_str);
#endif
			}
			else	{
				prt_error(ERROR_N_FLD);
				error_flag = 0;
			}
				
		}
		else	{
			if (error_flag)
				update_piu();
		}

}



/*******************************************************************************
 * check pckup_lead_time field. 
 ******************************************************************************/
int check_pkup_l_tm (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
short     pckup_lead_time;


	if(*field_entered)	{
			clear_err_line();
			if (!numeric(read_buf))	{
				prt_error(ERROR_N_FLD);
				error_flag = 0;
				return;
			}
			pckup_lead_time = atoi(read_buf);	/* move super zone number into local */
			if ((0 <= pckup_lead_time) && (pckup_lead_time <= 32000))	{
			ipc_required = 1;
			error_flag = 1;
			clear_err_line();
			switch(piu) {

			case F_PKUP_L_TM:
			  zn_ptr->pckup_lead_time= (short)pckup_lead_time;	/* move lead_time to isam buffer */ 
			  mvprintw(D_MX_DSP_TM_ROW,D_PKUP_L_TM_COL,"%5d",zn_ptr->pckup_lead_time);
			  break;

			case F_X_PKUP_L_TM:
			  zn_ptr->typex_lead_time = (short)pckup_lead_time;
			  mvprintw(D_MX_DSP_TM_ROW,D_X_PKUP_L_TM_COL,"%5d",zn_ptr->typex_lead_time);
			  break;

			default:
			  break;
			}
			refresh();
			update_piu();

			}
			else	{
				prt_error(ERROR_N_FLD);
				error_flag = 0;
			}

		}
		else	{
			if (error_flag)
				update_piu();
		}

}




/*******************************************************************************
 * check stand_time_from field. 
 ******************************************************************************/
int check_st_tm_frm (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
short   stand_time_from;

#ifdef	DEBUG
	sprintf(trace_str,"Entering check_st_tm_frm\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			if (!numeric(read_buf))	{
				prt_error(ERROR_N_FLD);
				error_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_st_tm_frm - 1\n");
	TRACE(trace_str);
#endif
				return;
			}
		stand_time_from = atoi(read_buf);	/* move super zone number into local */
	if (((0 <= stand_time_from) && (stand_time_from <= 59)) || ((100<=stand_time_from) && (stand_time_from <=159)) ||
	((200 <= stand_time_from) && (stand_time_from <= 259)) || ((300<=stand_time_from) && (stand_time_from <=359)) ||
	((400 <= stand_time_from) && (stand_time_from <= 459)) || ((500<=stand_time_from) && (stand_time_from <=559)) ||
	((600 <= stand_time_from) && (stand_time_from <= 659)) || ((700<=stand_time_from) && (stand_time_from <=759)) ||
	((800 <= stand_time_from) && (stand_time_from <= 859)) || ((900<=stand_time_from) && (stand_time_from <=959)) ||
	((1000 <= stand_time_from) && (stand_time_from <= 1059)) || ((1100<=stand_time_from) && (stand_time_from <=1159)) ||
	((1200 <= stand_time_from) && (stand_time_from <= 1259)) || ((1300<=stand_time_from) && (stand_time_from <=1359)) ||
	((1400 <= stand_time_from) && (stand_time_from <= 1459)) || ((1500<=stand_time_from) && (stand_time_from <=1559)) ||
	((1600 <= stand_time_from) && (stand_time_from <= 1659)) || ((1700<=stand_time_from) && (stand_time_from <=1759)) ||
	((1800 <= stand_time_from) && (stand_time_from <= 1859)) || ((1900<=stand_time_from) && (stand_time_from <=1959)) ||
	((2000 <= stand_time_from) && (stand_time_from <= 2059)) || ((2100<=stand_time_from) && (stand_time_from <=2159)) ||
	((2200 <= stand_time_from) && (stand_time_from <= 2259)) || ((2300<=stand_time_from) && (stand_time_from <=2359)))  {
			ipc_required = 1;
			error_flag = 1;
			clear_err_line();
			zn_ptr->stand_time_from= (short)stand_time_from;
                        
                        if ( range_low > 0 )
                          global_zone_rec.stand_time_from = (short)stand_time_from;
                        
			mvprintw(D_MX_DSP_TM_ROW,D_ST_TM_FRM_COL,"%5d",zn_ptr->stand_time_from);
			refresh();
			update_piu();
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_st_tm_frm - 2\n");
	TRACE(trace_str);
#endif
		}
		else	{
			error_flag = 0;
			prt_error(ERROR_N_FLD);
		}
	}
	else	{
		if (error_flag)
			update_piu();
	}
}




/*******************************************************************************
 * check stand_time_to field. 
 ******************************************************************************/
int check_st_tm_to (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
short  stand_time_to;

#ifdef	DEBUG
	sprintf(trace_str,"Entering check_st_tm_to\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			if (!numeric(read_buf))	{
				prt_error(ERROR_N_FLD);
				error_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_st_tm_to - 1\n");
	TRACE(trace_str);
#endif
				return;
			}
		stand_time_to = atoi(read_buf);	/* move super zone number into local */
	if (((0 <= stand_time_to) && (stand_time_to <= 59)) || ((100<=stand_time_to) && (stand_time_to <=159)) ||
	((200 <= stand_time_to) && (stand_time_to <= 259)) || ((300<=stand_time_to) && (stand_time_to <=359)) ||
	((400 <= stand_time_to) && (stand_time_to <= 459)) || ((500<=stand_time_to) && (stand_time_to <=559)) ||
	((600 <= stand_time_to) && (stand_time_to <= 659)) || ((700<=stand_time_to) && (stand_time_to <=759)) ||
	((800 <= stand_time_to) && (stand_time_to <= 859)) || ((900<=stand_time_to) && (stand_time_to <=959)) ||
	((1000 <= stand_time_to) && (stand_time_to <= 1059)) || ((1100<=stand_time_to) && (stand_time_to <=1159)) ||
	((1200 <= stand_time_to) && (stand_time_to <= 1259)) || ((1300<=stand_time_to) && (stand_time_to <=1359)) ||
	((1400 <= stand_time_to) && (stand_time_to <= 1459)) || ((1500<=stand_time_to) && (stand_time_to <=1559)) ||
	((1600 <= stand_time_to) && (stand_time_to <= 1659)) || ((1700<=stand_time_to) && (stand_time_to <=1759)) ||
	((1800 <= stand_time_to) && (stand_time_to <= 1859)) || ((1900<=stand_time_to) && (stand_time_to <=1959)) ||
	((2000 <= stand_time_to) && (stand_time_to <= 2059)) || ((2100<=stand_time_to) && (stand_time_to <=2159)) ||
	((2200 <= stand_time_to) && (stand_time_to <= 2259)) || ((2300<=stand_time_to) && (stand_time_to <=2359)))  {
			ipc_required = 1;
			error_flag = 1;
			clear_err_line();
                        
                        if ( range_low > 0 )
                          global_zone_rec.stand_time_to = (short)stand_time_to;
                        
			zn_ptr->stand_time_to= (short)stand_time_to;	 
			mvprintw(D_MX_DSP_TM_ROW,D_ST_TM_TO_COL,"%5d",zn_ptr->stand_time_to);
			refresh();
			update_piu();
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_st_tm_to - 2\n");
	TRACE(trace_str);
#endif
		}
		else	{
				prt_error(ERROR_N_FLD);
				error_flag = 0;
		}
	}
	else	{
		if (error_flag)
			update_piu();
	}

}




/*******************************************************************************
 * check enr_post_allow field.  
 ******************************************************************************/
int check_ep_alwd (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
    char  c;

#ifdef	DEBUG
	sprintf(trace_str,"Entering check_ep_alwd\n");
	TRACE(trace_str);
#endif
    c = Toupper(read_buf[0]);
    
    if (*field_entered)		{
	clear_err_line();
	if (c == YES || c == NO) {
	    ipc_required = 1;
	    error_flag = 1;
	    zn_ptr->enr_post_allow= c;
	    mvprintw(D_MX_DSP_TM_ROW,D_EP_ALWD_COL,"%c",zn_ptr->enr_post_allow);
	    refresh();
	    update_piu();
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_ep_alwd - 1\n");
	TRACE(trace_str);
#endif
	}
	else	{
	    clear_err_line();
	    prt_error(ERROR_N_FLD);
	    error_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_ep_alwd - 2\n");
	TRACE(trace_str);
#endif
	    return;
	}
    }
    else	{
	if (error_flag)	{
	  update_piu();
	}
    }
			
}

/*******************************************************************************
 * check enroute book lead time. ( 0 - 32,000)
 ******************************************************************************/
int check_enr_post_lead (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
short   lead_tm;

#ifdef	DEBUG
	sprintf(trace_str,"Entering check_enr_post_lead\n");
	TRACE(trace_str);
#endif
	if (piu != F_EBOOK_LEAD_TM)
	{
		prt_error("WRONG FIELD!!!");
		return;
	}
	if(*field_entered)	{
			clear_err_line();

			if (!numeric(read_buf))	{
				prt_error(ERROR_N_FLD);
				error_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_enr_post_lead - 1\n");
	TRACE(trace_str);
#endif
				return;
			}
		lead_tm = atoi(read_buf);	/* move super zone number into local */
		if ((0 <= lead_tm) && (lead_tm <= 32767))	{
			ipc_required = 1;
			error_flag = 1;
			clear_err_line();
			zn_ptr->enr_post_lead = (short)lead_tm;	/* move command number to isam buffer */ 
			mvprintw(D_EBOOK_LEAD_TM_ROW,D_EBOOK_LEAD_TM_COL,"%5d",zn_ptr->enr_post_lead);
			refresh();
			update_piu();
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_enr_post_lead - 2\n");
	TRACE(trace_str);
#endif
			}
			else	{
				prt_error(ERROR_N_FLD);
				error_flag = 0; 
			}
		}
		else	{
			if (error_flag)	{
				if ((temp_flag) && (key_val == KEY_UP))
					piu = F_TYPE;
				else
					update_piu();
			}
		}

}

/*******************************************************************************
 * check  MDT screen field.  1, 2, 3 allowed
 ******************************************************************************/

int
check_mdt_screen(piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
  if (piu != F_MDT_SCREEN)
    {
      prt_error("WRONG FIELD!!!");
      return;
    }

  if (*field_entered)
    {
      clear_err_line();
      if (read_buf[0] == '0' ||
	  read_buf[0] == '1' ||
	  read_buf[0] == '2' ||
	  read_buf[0] == '3')
	{
	  ipc_required = 1;
	  error_flag = 1;
	  clear_err_line();
	  zn_ptr->mdt_screen = (short) (read_buf[0] - '0');
	  mvaddch(D_MDT_SCREEN_ROW, D_MDT_SCREEN_COL, read_buf[0]);
	  refresh();
	  update_piu();
	}
      else	{
	error_flag = 0;
	prt_error(ERROR_N_FLD);
      }
    }
  else	{
    if (error_flag)
      update_piu();
  }
}

/*******************************************************************************
* check accept_duration field 
*******************************************************************************/
int check_accept_duration (piu_ptr, read_buf, key_val, field_entered)
	int	*piu_ptr;
	char	*read_buf;
	int	key_val;
	int	*field_entered;
{
	short	accept_time;

#ifdef	DEBUG
	sprintf(trace_str,"Entering check_accept_duration\n");
	TRACE(trace_str);
#endif
	if (piu != F_ACPT_DURATION)
	{
		prt_error("WRONG FIELD!");
		return;
	}
	if (*field_entered)
	{
		clear_err_line();
		if(!numeric(read_buf)){
			prt_error(ERROR_N_FLD);
			error_flag=0;
#ifdef 	DEBUG
	sprintf(trace_str,"Leaving check_accept_duration -1 ");
	TRACE(trace_str);
#endif
			return;
		}
		accept_time = atoi(read_buf);
		if ((0 <= accept_time) && (accept_time <= 99999)){
			ipc_required = 1;
			error_flag = 1;
			clear_err_line();
			zn_ptr->accept_duration = (short)accept_time;
			mvprintw(D_ACPT_DURATION_ROW,D_ACPT_DURATION_COL,"%5d",zn_ptr->accept_duration);
			refresh();
			update_piu();
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_accept_duration -2 ");
	TRACE(trace_str);
#endif
		}
		else {
			prt_error(ERROR_N_FLD);
			error_flag = 0;
		}
	}
	else    {
		if (error_flag) {
			update_piu();
		}
	}
}

int check_cbook_time (piu_ptr, read_buf, key_val, field_entered)
	int	*piu_ptr;
	char	*read_buf;
	int	key_val;
	int	*field_entered;
{
	short	cbook_time;


	if (piu != F_CBOOK_TIME)
	{
		prt_error("WRONG FIELD!");
		return;
	}
	if (*field_entered)
	{
		clear_err_line();
		if(!numeric(read_buf)){
			prt_error(ERROR_N_FLD);
			error_flag=0;
			return;
		}
		cbook_time = atoi(read_buf);
		if ( cbook_time > zn_ptr->avl_call_q_time )
		  {
		    prt_error(ERROR_N_FLD);
		    error_flag = 0;
		    return;
		  }
		else if ((0 <= cbook_time) && (cbook_time <= 99999)){
			ipc_required = 1;
			error_flag = 1;
			clear_err_line();
			zn_ptr->cbook_time = (short)cbook_time;
			mvprintw(D_EBOOK_LEAD_TM_ROW,D_CBOOK_TIME_COL,"%5d",zn_ptr->cbook_time);
			refresh();
			update_piu();
		}
		else {
			prt_error(ERROR_N_FLD);
			error_flag = 0;
		}
	}
	else    {
		if (error_flag) {
			update_piu();
		}
	}
}
/*******************************************************************************
* check break_duration field 
*******************************************************************************/
int check_break_duration (piu_ptr, read_buf, key_val, field_entered)
	int	*piu_ptr;
	char	*read_buf;
	int	key_val;
	int	*field_entered;
{
	short	break_time;

#ifdef	DEBUG
	sprintf(trace_str,"Entering check_break_duration\n");
	TRACE(trace_str);
#endif
	if (piu != F_BREAK_DURATION)
	{
		prt_error("WRONG FIELD!");
		return;
	}
	if (*field_entered)
	{
		clear_err_line();
		if(!numeric(read_buf)){
			prt_error(ERROR_N_FLD);
			error_flag=0;
#ifdef 	DEBUG
	sprintf(trace_str,"Leaving check_break_duration -1 ");
	TRACE(trace_str);
#endif
			return;
		}
		break_time = atoi(read_buf);
		if ((0 <= break_time) && (break_time <= 999)){
			ipc_required = 1;
			error_flag = 1;
			clear_err_line();
			zn_ptr->break_duration = (short)break_time;
			mvprintw(D_BREAK_DURATION_ROW,D_BREAK_DURATION_COL,"%3d",zn_ptr->break_duration);
			refresh();
			update_piu();
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_break_duration -2 ");
	TRACE(trace_str);
#endif
		}
		else {
			prt_error(ERROR_N_FLD);
			error_flag = 0;
		}
	}
	else    {
		if (error_flag) {
			if ((temp_flag) && (key_val == KEY_UP))
				piu = F_TYPE;
			else
				update_piu();
		}
	}
}

/*******************************************************************************
 * check pkup_seals field. (1 - 16) 
 ******************************************************************************/
int check_pkup_seals (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
    char c;

#ifdef	DEBUG
	sprintf(trace_str,"Entering check_pkup_seals\n");
	TRACE(trace_str);
#endif
    c = Toupper(read_buf[0]);

		if (*field_entered)	{
			clear_err_line();
		if (c == YES ||c == NO) {
			ipc_required = 1;
			error_flag = 1;
			clear_err_line();
			switch(piu)	{
				case F_P_SL_1:
					zn_ptr->pckup_seals[0] = c;
					mvprintw(D_PKUP_SLS_ROW,D_P_SL_1_COL,"%c",zn_ptr->pckup_seals[0]);
					break;
				case F_P_SL_2:
					zn_ptr->pckup_seals[1] = c;
					mvprintw(D_PKUP_SLS_ROW,D_P_SL_2_COL,"%c",zn_ptr->pckup_seals[1]);
					break;
				case F_P_SL_3:
					zn_ptr->pckup_seals[2] = c;
					mvprintw(D_PKUP_SLS_ROW,D_P_SL_3_COL,"%c",zn_ptr->pckup_seals[2]);
					break;
				case F_P_SL_4:
					zn_ptr->pckup_seals[3] = c;
					mvprintw(D_PKUP_SLS_ROW,D_P_SL_4_COL,"%c",zn_ptr->pckup_seals[3]);
					break;
				case F_P_SL_5:
					zn_ptr->pckup_seals[4] = c;
					mvprintw(D_PKUP_SLS_ROW,D_P_SL_5_COL,"%c",zn_ptr->pckup_seals[4]);
					break;
				case F_P_SL_6:
					zn_ptr->pckup_seals[5] = c;
					mvprintw(D_PKUP_SLS_ROW,D_P_SL_6_COL,"%c",zn_ptr->pckup_seals[5]);
					break;
				case F_P_SL_7:
					zn_ptr->pckup_seals[6] = c;
					mvprintw(D_PKUP_SLS_ROW,D_P_SL_7_COL,"%c",zn_ptr->pckup_seals[6]);
					break;
				case F_P_SL_8:
					zn_ptr->pckup_seals[7] = c;
					mvprintw(D_PKUP_SLS_ROW,D_P_SL_8_COL,"%c",zn_ptr->pckup_seals[7]);
					break;
				case F_P_SL_9:
					zn_ptr->pckup_seals[8] = c;
					mvprintw(D_PKUP_SLS_ROW,D_P_SL_9_COL,"%c",zn_ptr->pckup_seals[8]);
					break;
				case F_P_SL_10:
					zn_ptr->pckup_seals[9] = c;
					mvprintw(D_PKUP_SLS_ROW,D_P_SL_10_COL,"%c",zn_ptr->pckup_seals[9]);
					break;
				case F_P_SL_11:
					zn_ptr->pckup_seals[10] = c;
					mvprintw(D_PKUP_SLS_ROW,D_P_SL_11_COL,"%c",zn_ptr->pckup_seals[10]);
					break;
				case F_P_SL_12:
					zn_ptr->pckup_seals[11] = c;
					mvprintw(D_PKUP_SLS_ROW,D_P_SL_12_COL,"%c",zn_ptr->pckup_seals[11]);
					break;
				case F_P_SL_13:
					zn_ptr->pckup_seals[12] = c;
					mvprintw(D_PKUP_SLS_ROW,D_P_SL_13_COL,"%c",zn_ptr->pckup_seals[12]);
					break;
				case F_P_SL_14:
					zn_ptr->pckup_seals[13] = c;
					mvprintw(D_PKUP_SLS_ROW,D_P_SL_14_COL,"%c",zn_ptr->pckup_seals[13]);
					break;
				case F_P_SL_15:
					zn_ptr->pckup_seals[14] = c;
					mvprintw(D_PKUP_SLS_ROW,D_P_SL_15_COL,"%c",zn_ptr->pckup_seals[14]);
					break;
				case F_P_SL_16:
					zn_ptr->pckup_seals[15] = c;
					mvprintw(D_PKUP_SLS_ROW,D_P_SL_16_COL,"%c",zn_ptr->pckup_seals[15]);
					break;
			}
				refresh();
				update_piu();
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_pkup_seals- 1\n");
	TRACE(trace_str);
#endif
			}
			else	{
				prt_error(ERROR_N_FLD);
				error_flag = 0;
			}
	} /* field entered */
	else	{
		if (error_flag)
			update_piu();
	}
}



/*******************************************************************************
 * check  alg and reject penalty - (0,1,2,3).
 ******************************************************************************/

int check_alg (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_alg\n");
	TRACE(trace_str);
#endif
	if(*field_entered)		{
			clear_err_line();
		if (read_buf[0] == '0' ||
		    read_buf[0] == '1' ||
		    read_buf[0] == '2' ||
		    read_buf[0] == '3') 	{
			ipc_required = 1;
			error_flag = 1;
			clear_err_line();
			switch(piu)	{
				case F_L1_ALG:
					zn_ptr->lev_alg[0] = read_buf[0];
					mvprintw(D_LEVEL_ROW,D_L1_ALG_COL,"%c",zn_ptr->lev_alg[0]);
					break;
				case F_L2_ALG:
					zn_ptr->lev_alg[2] = read_buf[0];
					mvprintw(D_LVL_2_ROW,D_L2_ALG_COL,"%c",zn_ptr->lev_alg[2]);
					break;
				case F_L3_ALG:
					zn_ptr->lev_alg[3] = read_buf[0];
					mvprintw(D_LVL_3_ROW,D_L3_ALG_COL,"%c",zn_ptr->lev_alg[3]);
					break;
				case F_L4_ALG:
					zn_ptr->lev_alg[4] = read_buf[0];
					mvprintw(D_LVL_4_ROW,D_L4_ALG_COL,"%c",zn_ptr->lev_alg[4]);
					break;
				case F_L5_ALG:
					zn_ptr->lev_alg[5] = read_buf[0];
					mvprintw(D_LVL_5_ROW,D_L5_ALG_COL,"%c",zn_ptr->lev_alg[5]);
					break;
				case F_L6_ALG:
					zn_ptr->lev_alg[6] =read_buf[0];
					mvprintw(D_LVL_6_ROW,D_L6_ALG_COL,"%c",zn_ptr->lev_alg[6]);
					break;
				case F_L7_ALG:
					zn_ptr->lev_alg[7] = read_buf[0];
					mvprintw(D_LVL_7_ROW,D_L7_ALG_COL,"%c",zn_ptr->lev_alg[7]);
					break;
				case F_L8_ALG:
					zn_ptr->lev_alg[8] = read_buf[0];
					mvprintw(D_LVL_8_ROW,D_L8_ALG_COL,"%c",zn_ptr->lev_alg[8]);
					break;
				case F_L1_RPN:
					zn_ptr->lev_rej_penlty[0] = read_buf[0];
					mvprintw(D_LEVEL_ROW,D_L1_RPN_COL,"%c",zn_ptr->lev_rej_penlty[0]);
					break;
				case F_PRM_RPN:
					zn_ptr->lev_rej_penlty[1] = read_buf[0];
					mvprintw(D_PRM_ETM_ROW,D_PRM_RPN_COL,"%c",zn_ptr->lev_rej_penlty[1]);
					break;
				case F_L2_RPN:
					zn_ptr->lev_rej_penlty[2] = read_buf[0];
					mvprintw(D_LVL_2_ROW,D_L2_RPN_COL,"%c",zn_ptr->lev_rej_penlty[2]);
					break;
				case F_L3_RPN:
					zn_ptr->lev_rej_penlty[3] = read_buf[0];
					mvprintw(D_LVL_3_ROW,D_L3_RPN_COL,"%c",zn_ptr->lev_rej_penlty[3]);
					break;
				case F_L4_RPN:
					zn_ptr->lev_rej_penlty[4] = read_buf[0];
					mvprintw(D_LVL_4_ROW,D_L4_RPN_COL,"%c",zn_ptr->lev_rej_penlty[4]);
					break;
				case F_L5_RPN:
					zn_ptr->lev_rej_penlty[5] = read_buf[0];
					mvprintw(D_LVL_5_ROW,D_L5_RPN_COL,"%c",zn_ptr->lev_rej_penlty[5]);
					break;
				case F_L6_RPN:
					zn_ptr->lev_rej_penlty [6]= read_buf[0];
					mvprintw(D_LVL_6_ROW,D_L6_RPN_COL,"%c",zn_ptr->lev_rej_penlty[6]);
					break;
				case F_L7_RPN:
					zn_ptr->lev_rej_penlty[7] = read_buf[0];
					mvprintw(D_LVL_7_ROW,D_L7_RPN_COL,"%c",zn_ptr->lev_rej_penlty[7]);
					break;
				case F_L8_RPN:
					zn_ptr->lev_rej_penlty[8] = read_buf[0];
					mvprintw(D_LVL_8_ROW,D_L8_RPN_COL,"%c",zn_ptr->lev_rej_penlty[8]);
					break;
			}
				refresh();
				update_piu();
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_alg- 1\n");
	TRACE(trace_str);
#endif
			}
			else	{
				error_flag = 0;
				prt_error(ERROR_N_FLD);
			}
	}
	else	{
		if (error_flag)
			update_piu();
	}
					
}


/*******************************************************************************
 * check  excl-time. 
 ******************************************************************************/
int check_etm (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
short     excl_time;

#ifdef	DEBUG
	sprintf(trace_str,"Entering check_excl_tm\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			if (!numeric(read_buf))	{
				prt_error(ERROR_N_FLD);
				error_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_excl_tm- 1\n");
	TRACE(trace_str);
#endif
				return;
			}
			excl_time = atoi(read_buf);	
			if ((0 <= excl_time) && (excl_time <= 32000))	{
			ipc_required = 1;
			error_flag = 1;
			clear_err_line();
		switch(piu)	{
			case F_L1_ETM:
				zn_ptr->lev_excl_time[0]= (short)excl_time;	/* move time to isam buffer */ 
				mvprintw(D_LEVEL_ROW,D_L1_ETM_COL,"%5d",zn_ptr->lev_excl_time[0]);
				break;
			case F_PRM_ETM:
				zn_ptr->lev_excl_time[1]= (short)excl_time;	/* move time to isam buffer */ 
				mvprintw(D_PRM_ETM_ROW,D_PRM_ETM_COL,"%5d",zn_ptr->lev_excl_time[1]);
				break;
			case F_L2_ETM:
				zn_ptr->lev_excl_time[2]= (short)excl_time;	/* move time to isam buffer */ 
				mvprintw(D_LVL_2_ROW,D_L2_ETM_COL,"%5d",zn_ptr->lev_excl_time[2]);
				break;
			case F_L3_ETM:
				zn_ptr->lev_excl_time[3]= (short)excl_time;	/* move time to isam buffer */ 
				mvprintw(D_LVL_3_ROW,D_L3_ETM_COL,"%5d",zn_ptr->lev_excl_time[3]);
				break;
			case F_L4_ETM:
				zn_ptr->lev_excl_time[4]= (short)excl_time;	/* move time to isam buffer */ 
				mvprintw(D_LVL_4_ROW,D_L4_ETM_COL,"%5d",zn_ptr->lev_excl_time[4]);
				break;
			case F_L5_ETM:
			zn_ptr->lev_excl_time[5]= (short)excl_time;	/* move time to isam buffer */ 
				mvprintw(D_LVL_5_ROW,D_L5_ETM_COL,"%5d",zn_ptr->lev_excl_time[5]);
				break;
			case F_L6_ETM:
				zn_ptr->lev_excl_time[6]= (short)excl_time;	/* move time to isam buffer */ 
				mvprintw(D_LVL_6_ROW,D_L6_ETM_COL,"%5d",zn_ptr->lev_excl_time[6]);
				break;
			case F_L7_ETM:
				zn_ptr->lev_excl_time[7]= (short)excl_time;	/* move time to isam buffer */ 
				mvprintw(D_LVL_7_ROW,D_L7_ETM_COL,"%5d",zn_ptr->lev_excl_time[7]);
				break;
			case F_L8_ETM:
				zn_ptr->lev_excl_time[8]= (short)excl_time;	/* move time to isam buffer */ 
				mvprintw(D_LVL_8_ROW,D_L8_ETM_COL,"%5d",zn_ptr->lev_excl_time[8]);
				break;
			}
			refresh();
			update_piu();
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_excl_tm- 2\n");
	TRACE(trace_str);
#endif
			}
			else	{
				prt_error(ERROR_N_FLD);
				error_flag = 0;
			}
		}
		else	{
			if (error_flag)
				update_piu();
		}

}

int check_avail_taxis( piu_ptr, read_buf, key_val, field_entered )
     int  *piu_ptr;	/* pointer to the number of the field read in */
     char *read_buf; /* pointer to the buffer in which the field was read in */
     int key_val;    /* value of the key pressed or entered */
     int *field_entered; /* indicates if data was entered or not */
     
{
  int   i,j;
  char  answer;
  
  if(*field_entered)	{
    clear_err_line();
    if (!isalpha(read_buf[0]))	{
      prt_error(ERROR_N_FLD);
      error_flag = 0;
      return(error_flag);
    }
  }

  answer = read_buf[0];
  if ( answer != YES && answer != NO )
    {
      prt_error( "You must answer Y or N " );
      error_flag = 0;
      return( error_flag );
    }

  zn_ptr->check_avail_taxis = read_buf[0];
  ipc_required = 1;
  mvprintw(D_CHECK_TAXIS_ROW, D_CHECK_TAXIS_COL, "%c", zn_ptr->check_avail_taxis );
  
  update_piu();
  
}
      


/*******************************************************************************
 * check  zone level 1 to 8 and zone 1 to 8. 
 ******************************************************************************/
int check_zone (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */

{
int   i,j;
short  nbr,pre_nbr;

#ifdef	DEBUG
	sprintf(trace_str,"Entering check_zone\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			if (!numeric(read_buf))	{
				prt_error(ERROR_N_FLD);
				error_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_zone_nbr - 1\n");
	TRACE(trace_str);
#endif
				return;
			}
			nbr = atoi(read_buf);
			if ((nbr != 0) && (nbr != 888))	{
				temp_zn_ptr->nbr = (short)nbr;
				temp_zn_ptr->fleet = fl_ptr->id;
				if ((db(ZONE_FILE_ID,READ_KEYS,&zn_key1,ISEQUAL,0)) < 0)	{
					prt_error(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_3, "Invalid zone number"));
					error_flag = 0;
					return;
				}
			}
				temp_nbr = (short)nbr;
				error_flag = 1;
				clear_err_line();
                                ipc_required = 1;
			switch(piu)	{
				case F_L1_Z1 :
					zn_ptr->lev_zone_nbr[0][0] = temp_nbr; 
					mvprintw(D_LEVEL_ROW,D_L1_Z1_COL,"%3d",zn_ptr->lev_zone_nbr[0][0]);
					break;
				case F_L1_Z2 :
					zn_ptr->lev_zone_nbr[0][1] = temp_nbr; 
					mvprintw(D_LEVEL_ROW,D_L1_Z2_COL,"%3d",zn_ptr->lev_zone_nbr[0][1]);
					break;
				case F_L1_Z3 :
					zn_ptr->lev_zone_nbr[0][2] = temp_nbr; 
					mvprintw(D_LEVEL_ROW,D_L1_Z3_COL,"%3d",zn_ptr->lev_zone_nbr[0][2]);
					break;
				case F_L1_Z4 :
					zn_ptr->lev_zone_nbr[0][3] = temp_nbr; 
					mvprintw(D_LEVEL_ROW,D_L1_Z4_COL,"%3d",zn_ptr->lev_zone_nbr[0][3]);
					break;
				case F_L1_Z5 :
					zn_ptr->lev_zone_nbr[0][4] = temp_nbr; 
					mvprintw(D_LEVEL_ROW,D_L1_Z5_COL,"%3d",zn_ptr->lev_zone_nbr[0][4]);
					break;
				case F_L1_Z6 :
					zn_ptr->lev_zone_nbr[0][5] = temp_nbr; 
					mvprintw(D_LEVEL_ROW,D_L1_Z6_COL,"%3d",zn_ptr->lev_zone_nbr[0][5]);
					break;
				case F_L1_Z7 :
					zn_ptr->lev_zone_nbr[0][6] = temp_nbr; 
					mvprintw(D_LEVEL_ROW,D_L1_Z7_COL,"%3d",zn_ptr->lev_zone_nbr[0][6]);
					break;
				case F_L1_Z8 :
					zn_ptr->lev_zone_nbr[0][7] = temp_nbr; 
					mvprintw(D_LEVEL_ROW,D_L1_Z8_COL,"%3d",zn_ptr->lev_zone_nbr[0][7]);
					break;

				case F_L2_Z1 :
					zn_ptr->lev_zone_nbr[2][0] = temp_nbr; 
					mvprintw(D_LVL_2_ROW,D_L2_Z1_COL,"%3d",zn_ptr->lev_zone_nbr[2][0]);
					break;
				case F_L2_Z2 :
					zn_ptr->lev_zone_nbr[2][1] = temp_nbr; 
					mvprintw(D_LVL_2_ROW,D_L2_Z2_COL,"%3d",zn_ptr->lev_zone_nbr[2][1]);
					break;
				case F_L2_Z3 :
					zn_ptr->lev_zone_nbr[2][2] = temp_nbr; 
					mvprintw(D_LVL_2_ROW,D_L2_Z3_COL,"%3d",zn_ptr->lev_zone_nbr[2][2]);
					break;
				case F_L2_Z4 :
					zn_ptr->lev_zone_nbr[2][3] = temp_nbr; 
					mvprintw(D_LVL_2_ROW,D_L2_Z4_COL,"%3d",zn_ptr->lev_zone_nbr[2][3]);
					break;
				case F_L2_Z5 :
					zn_ptr->lev_zone_nbr[2][4] = temp_nbr; 
					mvprintw(D_LVL_2_ROW,D_L2_Z5_COL,"%3d",zn_ptr->lev_zone_nbr[2][4]);
					break;
				case F_L2_Z6 :
					zn_ptr->lev_zone_nbr[2][5] = temp_nbr; 
					mvprintw(D_LVL_2_ROW,D_L2_Z6_COL,"%3d",zn_ptr->lev_zone_nbr[2][5]);
					break;
				case F_L2_Z7 :
					zn_ptr->lev_zone_nbr[2][6] = temp_nbr; 
					mvprintw(D_LVL_2_ROW,D_L2_Z7_COL,"%3d",zn_ptr->lev_zone_nbr[2][6]);
					break;
				case F_L2_Z8 :
					zn_ptr->lev_zone_nbr[2][7] = temp_nbr; 
					mvprintw(D_LVL_2_ROW,D_L2_Z8_COL,"%3d",zn_ptr->lev_zone_nbr[2][7]);
					break;

				case F_L3_Z1 :
					zn_ptr->lev_zone_nbr[3][0] = temp_nbr; 
					mvprintw(D_LVL_3_ROW,D_L3_Z1_COL,"%3d",zn_ptr->lev_zone_nbr[3][0]);
					break;
				case F_L3_Z2 :
					zn_ptr->lev_zone_nbr[3][1] = temp_nbr; 
					mvprintw(D_LVL_3_ROW,D_L3_Z2_COL,"%3d",zn_ptr->lev_zone_nbr[3][1]);
					break;
				case F_L3_Z3 :
					zn_ptr->lev_zone_nbr[3][2] = temp_nbr; 
					mvprintw(D_LVL_3_ROW,D_L3_Z3_COL,"%3d",zn_ptr->lev_zone_nbr[3][2]);
					break;
				case F_L3_Z4 :
					zn_ptr->lev_zone_nbr[3][3] = temp_nbr; 
					mvprintw(D_LVL_3_ROW,D_L3_Z4_COL,"%3d",zn_ptr->lev_zone_nbr[3][3]);
					break;
				case F_L3_Z5 :
					zn_ptr->lev_zone_nbr[3][4] = temp_nbr; 
					mvprintw(D_LVL_3_ROW,D_L3_Z5_COL,"%3d",zn_ptr->lev_zone_nbr[3][4]);
					break;
				case F_L3_Z6 :
					zn_ptr->lev_zone_nbr[3][5] = temp_nbr; 
					mvprintw(D_LVL_3_ROW,D_L3_Z6_COL,"%3d",zn_ptr->lev_zone_nbr[3][5]);
					break;
				case F_L3_Z7 :
					zn_ptr->lev_zone_nbr[3][6] = temp_nbr; 
					mvprintw(D_LVL_3_ROW,D_L3_Z7_COL,"%3d",zn_ptr->lev_zone_nbr[3][6]);
					break;
				case F_L3_Z8 :
					zn_ptr->lev_zone_nbr[3][7] = temp_nbr; 
					mvprintw(D_LVL_3_ROW,D_L3_Z8_COL,"%3d",zn_ptr->lev_zone_nbr[3][7]);
					break;

				case F_L4_Z1 :
					zn_ptr->lev_zone_nbr[4][0] = temp_nbr; 
					mvprintw(D_LVL_4_ROW,D_L4_Z1_COL,"%3d",zn_ptr->lev_zone_nbr[4][0]);
					break;
				case F_L4_Z2 :
					zn_ptr->lev_zone_nbr[4][1] = temp_nbr; 
					mvprintw(D_LVL_4_ROW,D_L4_Z2_COL,"%3d",zn_ptr->lev_zone_nbr[4][1]);
					break;
				case F_L4_Z3 :
					zn_ptr->lev_zone_nbr[4][2] = temp_nbr; 
					mvprintw(D_LVL_4_ROW,D_L4_Z3_COL,"%3d",zn_ptr->lev_zone_nbr[4][2]);
					break;
				case F_L4_Z4 :
					zn_ptr->lev_zone_nbr[4][3] = temp_nbr; 
					mvprintw(D_LVL_4_ROW,D_L4_Z4_COL,"%3d",zn_ptr->lev_zone_nbr[4][3]);
					break;
				case F_L4_Z5 :
					zn_ptr->lev_zone_nbr[4][4] = temp_nbr; 
					mvprintw(D_LVL_4_ROW,D_L4_Z5_COL,"%3d",zn_ptr->lev_zone_nbr[4][4]);
					break;
				case F_L4_Z6 :
					zn_ptr->lev_zone_nbr[4][5] = temp_nbr; 
					mvprintw(D_LVL_4_ROW,D_L4_Z6_COL,"%3d",zn_ptr->lev_zone_nbr[4][5]);
					break;
				case F_L4_Z7 :
					zn_ptr->lev_zone_nbr[4][6] = temp_nbr; 
					mvprintw(D_LVL_4_ROW,D_L4_Z7_COL,"%3d",zn_ptr->lev_zone_nbr[4][6]);
					break;
				case F_L4_Z8 :
					zn_ptr->lev_zone_nbr[4][7] = temp_nbr; 
					mvprintw(D_LVL_4_ROW,D_L4_Z8_COL,"%3d",zn_ptr->lev_zone_nbr[4][7]);
					break;

				case F_L5_Z1 :
					zn_ptr->lev_zone_nbr[5][0] = temp_nbr; 
					mvprintw(D_LVL_5_ROW,D_L5_Z1_COL,"%3d",zn_ptr->lev_zone_nbr[5][0]);
					break;
				case F_L5_Z2 :
					zn_ptr->lev_zone_nbr[5][1] = temp_nbr; 
					mvprintw(D_LVL_5_ROW,D_L5_Z2_COL,"%3d",zn_ptr->lev_zone_nbr[5][1]);
					break;
				case F_L5_Z3 :
					zn_ptr->lev_zone_nbr[5][2] = temp_nbr; 
					mvprintw(D_LVL_5_ROW,D_L5_Z3_COL,"%3d",zn_ptr->lev_zone_nbr[5][2]);
					break;
				case F_L5_Z4 :
					zn_ptr->lev_zone_nbr[5][3] = temp_nbr; 
					mvprintw(D_LVL_5_ROW,D_L5_Z4_COL,"%3d",zn_ptr->lev_zone_nbr[5][3]);
					break;
				case F_L5_Z5 :
					zn_ptr->lev_zone_nbr[5][4] = temp_nbr; 
					mvprintw(D_LVL_5_ROW,D_L5_Z5_COL,"%3d",zn_ptr->lev_zone_nbr[5][4]);
					break;
				case F_L5_Z6 :
					zn_ptr->lev_zone_nbr[5][5] = temp_nbr; 
					mvprintw(D_LVL_5_ROW,D_L5_Z6_COL,"%3d",zn_ptr->lev_zone_nbr[5][5]);
					break;
				case F_L5_Z7 :
					zn_ptr->lev_zone_nbr[5][6] = temp_nbr; 
					mvprintw(D_LVL_5_ROW,D_L5_Z7_COL,"%3d",zn_ptr->lev_zone_nbr[5][6]);
					break;
				case F_L5_Z8 :
					zn_ptr->lev_zone_nbr[5][7] = temp_nbr; 
					mvprintw(D_LVL_5_ROW,D_L5_Z8_COL,"%3d",zn_ptr->lev_zone_nbr[5][7]);
					break;


				case F_L6_Z1 :
					zn_ptr->lev_zone_nbr[6][0] = temp_nbr; 
					mvprintw(D_LVL_6_ROW,D_L6_Z1_COL,"%3d",zn_ptr->lev_zone_nbr[6][0]);
					break;
				case F_L6_Z2 :
					zn_ptr->lev_zone_nbr[6][1] = temp_nbr; 
					mvprintw(D_LVL_6_ROW,D_L6_Z2_COL,"%3d",zn_ptr->lev_zone_nbr[6][1]);
					break;
				case F_L6_Z3 :
					zn_ptr->lev_zone_nbr[6][2] = temp_nbr; 
					mvprintw(D_LVL_6_ROW,D_L6_Z3_COL,"%3d",zn_ptr->lev_zone_nbr[6][2]);
					break;
				case F_L6_Z4 :
					zn_ptr->lev_zone_nbr[6][3] = temp_nbr; 
					mvprintw(D_LVL_6_ROW,D_L6_Z4_COL,"%3d",zn_ptr->lev_zone_nbr[6][3]);
					break;
				case F_L6_Z5 :
					zn_ptr->lev_zone_nbr[6][4] = temp_nbr; 
					mvprintw(D_LVL_6_ROW,D_L6_Z5_COL,"%3d",zn_ptr->lev_zone_nbr[6][4]);
					break;
				case F_L6_Z6 :
					zn_ptr->lev_zone_nbr[6][5] = temp_nbr; 
					mvprintw(D_LVL_6_ROW,D_L6_Z6_COL,"%3d",zn_ptr->lev_zone_nbr[6][5]);
					break;
				case F_L6_Z7 :
					zn_ptr->lev_zone_nbr[6][6] = temp_nbr; 
					mvprintw(D_LVL_6_ROW,D_L6_Z7_COL,"%3d",zn_ptr->lev_zone_nbr[6][6]);
					break;
				case F_L6_Z8 :
					zn_ptr->lev_zone_nbr[6][7] = temp_nbr; 
					mvprintw(D_LVL_6_ROW,D_L6_Z8_COL,"%3d",zn_ptr->lev_zone_nbr[6][7]);
					break;

				case F_L7_Z1 :
					zn_ptr->lev_zone_nbr[7][0] = temp_nbr; 
					mvprintw(D_LVL_7_ROW,D_L7_Z1_COL,"%3d",zn_ptr->lev_zone_nbr[7][0]);
					break;
				case F_L7_Z2 :
					zn_ptr->lev_zone_nbr[7][1] = temp_nbr; 
					mvprintw(D_LVL_7_ROW,D_L7_Z2_COL,"%3d",zn_ptr->lev_zone_nbr[7][1]);
					break;
				case F_L7_Z3 :
					zn_ptr->lev_zone_nbr[7][2] = temp_nbr; 
					mvprintw(D_LVL_7_ROW,D_L7_Z3_COL,"%3d",zn_ptr->lev_zone_nbr[7][2]);
					break;
				case F_L7_Z4 :
					zn_ptr->lev_zone_nbr[7][3] = temp_nbr; 
					mvprintw(D_LVL_7_ROW,D_L7_Z4_COL,"%3d",zn_ptr->lev_zone_nbr[7][3]);
					break;
				case F_L7_Z5 :
					zn_ptr->lev_zone_nbr[7][4] = temp_nbr; 
					mvprintw(D_LVL_7_ROW,D_L7_Z5_COL,"%3d",zn_ptr->lev_zone_nbr[7][4]);
					break;
				case F_L7_Z6 :
					zn_ptr->lev_zone_nbr[7][5] = temp_nbr; 
					mvprintw(D_LVL_7_ROW,D_L7_Z6_COL,"%3d",zn_ptr->lev_zone_nbr[7][5]);
					break;
				case F_L7_Z7 :
					zn_ptr->lev_zone_nbr[7][6] = temp_nbr; 
					mvprintw(D_LVL_7_ROW,D_L7_Z7_COL,"%3d",zn_ptr->lev_zone_nbr[7][6]);
					break;
				case F_L7_Z8 :
					zn_ptr->lev_zone_nbr[7][7] = temp_nbr; 
					mvprintw(D_LVL_7_ROW,D_L7_Z8_COL,"%3d",zn_ptr->lev_zone_nbr[7][7]);
					break;

				case F_L8_Z1 :
					zn_ptr->lev_zone_nbr[8][0] = temp_nbr; 
					mvprintw(D_LVL_8_ROW,D_L8_Z1_COL,"%3d",zn_ptr->lev_zone_nbr[8][0]);
					break;
				case F_L8_Z2 :
					zn_ptr->lev_zone_nbr[8][1] = temp_nbr; 
					mvprintw(D_LVL_8_ROW,D_L8_Z2_COL,"%3d",zn_ptr->lev_zone_nbr[8][1]);
					break;
				case F_L8_Z3 :
					zn_ptr->lev_zone_nbr[8][2] = temp_nbr; 
					mvprintw(D_LVL_8_ROW,D_L8_Z3_COL,"%3d",zn_ptr->lev_zone_nbr[8][2]);
					break;
				case F_L8_Z4 :
					zn_ptr->lev_zone_nbr[8][3] = temp_nbr; 
					mvprintw(D_LVL_8_ROW,D_L8_Z4_COL,"%3d",zn_ptr->lev_zone_nbr[8][3]);
					break;
				case F_L8_Z5 :
					zn_ptr->lev_zone_nbr[8][4] = temp_nbr; 
					mvprintw(D_LVL_8_ROW,D_L8_Z5_COL,"%3d",zn_ptr->lev_zone_nbr[8][4]);
					break;
				case F_L8_Z6 :
					zn_ptr->lev_zone_nbr[8][5] = temp_nbr; 
					mvprintw(D_LVL_8_ROW,D_L8_Z6_COL,"%3d",zn_ptr->lev_zone_nbr[8][5]);
					break;
				case F_L8_Z7 :
					zn_ptr->lev_zone_nbr[8][6] = temp_nbr; 
					mvprintw(D_LVL_8_ROW,D_L8_Z7_COL,"%3d",zn_ptr->lev_zone_nbr[8][6]);
					break;
				case F_L8_Z8 :
					zn_ptr->lev_zone_nbr[8][7] = temp_nbr; 
					mvprintw(D_LVL_8_ROW,D_L8_Z8_COL,"%3d",zn_ptr->lev_zone_nbr[8][7]);
					break;
				}
				refresh();
				update_piu();
		}
		else	{
			if (error_flag)	
				update_piu();
		}
}

int check_gps_dist_ceiling(piu_ptr, read_buf, key_val, field_entered)
 	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
  if(*field_entered)
    {
      clear_err_line();
      ipc_required = 1;
      error_flag = 1;
      clear_err_line();
      switch (piu)
	{
	case F_GPS_DIST_CEILING:
	  zn_ptr->gps_dist_ceiling = (int)atoi(read_buf);
	  mvprintw(D_GPS_DIST_CEILING_ROW,D_GPS_DIST_CEILING_COL,"%6d", zn_ptr->gps_dist_ceiling);
	  break;
	default:
	  break;
	}
      update_piu();
    }
  else
    {
      clear_err_line();
      if ( error_flag )
	update_piu();
      else
	prt_error(ERROR_N_FLD);
    }
}

int check_match_algorithm( piu_ptr, read_buf, key_val, field_entered)
 	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
  if(*field_entered)
    {
      clear_err_line();
      if ( !numeric(read_buf))
	{
	  prt_error(ERROR_N_FLD);
	  error_flag = 0;
	  return;
	}
      ipc_required = 1;
      error_flag = 1;
      clear_err_line();
      zn_ptr->match_algorithm = read_buf[0];
      mvprintw(D_MATCH_ALGORITHM_ROW,D_MATCH_ALGORITHM_COL,"%c", zn_ptr->match_algorithm);
      update_piu();
    }
  else
    {
      clear_err_line();
      if (error_flag)	
	update_piu();
      else
	prt_error(ERROR_N_FLD);
    }
}    


int check_lat_long (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{

  double   nbr_value;
  
  if(*field_entered)
    {
      clear_err_line();
      
      nbr_value = atof(read_buf);
      ipc_required = 1;
      error_flag = 1;
      clear_err_line();
      switch (piu)
	{
	case F_GPS_LAT:
	  zn_ptr->gps_lat = nbr_value;
	  mvprintw(D_GPS_LAT_ROW,D_GPS_LAT_COL,"%10.4f", zn_ptr->gps_lat);
	  break;
	case F_GPS_LONG:
	  zn_ptr->gps_long = nbr_value;
	  mvprintw(D_GPS_LONG_ROW,D_GPS_LONG_COL,"%10.4f", zn_ptr->gps_long);
	  break;
	default:
	  break;
	}
      update_piu();
    }
  else
    {
      clear_err_line();
      if (error_flag)	
	update_piu();
      else
	prt_error(ERROR_N_FLD);
    }
}

int check_radius (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{

  int   nbr_value;
  
  if(*field_entered)
    {
      clear_err_line();
      if (!numeric(read_buf))
	{
	  prt_error(ERROR_N_FLD);
	  error_flag = 0;
	  return;
	}
      nbr_value = atoi(read_buf);
      ipc_required = 1;
      error_flag = 1;
      clear_err_line();
      switch ( piu ) {
      case F_GPS_EMERG_RANGE:
        zn_ptr->emerg_range = nbr_value;
        mvprintw(D_GPS_EMERG_RANGE_ROW,D_GPS_EMERG_RANGE_COL,"%6d", zn_ptr->emerg_range);
        break;        
      case F_GPS_RADIUS:
        zn_ptr->gps_radius = nbr_value;
        mvprintw(D_GPS_RADIUS_ROW,D_GPS_RADIUS_COL,"%6d", zn_ptr->gps_radius);
        break;
      default:
        break;
      }
      update_piu();
    }
  else
    {
      clear_err_line();
      if (error_flag)	
	update_piu();
      else
	prt_error(ERROR_N_FLD);
    }
}


/******************************************************************************
* update_piu - update prompt in use variable.
*******************************************************************************/

update_piu()
{
	int  orig_piu;

#ifdef DEBUG
	sprintf(trace_str,"Entering update_piu\n");
	TRACE(trace_str);
#endif
	orig_piu = piu;

	if(key_val == '\t' ||
	   key_val == '\n' ||
	   key_val == '\r' ||
	   key_val == KEY_DOWN ||
	   key_val == VALID)
          if (doing_add)
            {
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
                          ++piu;
            }
        else
          {
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
				piu = F_FLEET;
			}
			else
				piu += 1;
          }            

	if( key_val == KEY_UP)
          {
		if(piu == F_SCR_2_MIN)  {
			scr_nbr = 1;
			clear();
			prt_upd_hdr();
			prt_labels();
			prt_data();
			piu = F_SCR_1_MAX;
		}
		else if(piu == F_SCR_1_MIN)  {
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


	/* If no seals for fleet and we fall onto a seal
	   change field to after or before seals */
	if (skip_seals && piu >= F_P_SL_1 && piu <= F_P_SL_16)
	  {
	    /* Going forwards */
	    if (orig_piu < piu)
	      piu = F_BREAK_DURATION;

	    /* Going backwards */
	    else
	      piu = F_CBOOK_TIME;
	  }
	

	/* Bid allowed field is not used. Easier to just skip it
	   than get rid of it. */
	if (piu == F_BID_ALW)
	{
	  if (orig_piu < piu)
	    piu ++;
	  else
	    piu --;
	}

	if ( range_low > 0 &&
	     ( piu == F_TYPE - 1 ) )
	  piu = F_MATCH_ALGORITHM;

	if ( ( range_low > 0 ) &&
	     ( piu == F_CHECK_TAXIS + 1 ) )
	  piu = F_MATCH_ALGORITHM;

	if ( ( range_low > 0 ) &&
	     ( piu == F_MATCH_ALGORITHM + 1 ) )
	  piu = F_TYPE;

	if ( ( range_low > 0 ) &&
	     ( piu == F_MATCH_ALGORITHM - 1 ) )
	  piu = F_CHECK_TAXIS;

	if (piu == 0)
	  piu = NUM_FLDS;
	else if (piu > NUM_FLDS)
	  piu = 1;

#ifdef DEBUG
	sprintf(trace_str,"Leaving update_piu \n");
	TRACE(trace_str);
#endif
	return;

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
* blank - make sure the field entered is blank.
****************************************************************************/

blank(in_str)
char in_str[132];
{
	int i;		/* loop index */
	int validity;	/* = 0,1 for bad, good */

	i = 0;
	validity = 1;	/* assume the field is valid */

	while(in_str[i])
		if(!isspace(in_str[i++]))
			validity = 0;

	return(validity);

} /* End of blank()  */

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
