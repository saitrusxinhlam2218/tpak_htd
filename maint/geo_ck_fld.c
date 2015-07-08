static char sz__FILE__[]="@(#)geo_ck_fld.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  geo_ck_fld.c; Rel: 5.3.0.0; Get date: 3/26/90 at 14:02:19
 * @(#)  Last delta: 3/23/90 at 16:02:23
 * @(#)  SCCS File: /taxi/sccs/s.geo_ck_fld.c
 *                                                                           *
 *****************************************************************************/

/* Make sure location constants are defined. Create a compile error if they
   are not defined or both are defined. */
#ifdef SCANDINAVIA
#ifdef NAMERICA
ERROR: "Both compile time constants SCANDINAVIA and NAMERICA are defined."
#endif
#else
#ifdef NAMERICA
#else
ERROR: Compile time constant SCANDINAVIA or NAMERICA undefined.
#endif
#endif

#include <cursesX.h>
#include <ctype.h>
#include <sys/types.h>

#include "mad_error.h"
#include "df_maint.h"
#include "ex_key.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "geo_scr.h"
#include "geo_msg.h"
#include "tokenize.h"
#include "language.h"
#include "geo_m_strdefs.h"
#include "func_keys.h"

extern struct scr_flds geo_fld[];
extern short ipc_required;		/* Set if a variable which resides */
					/* shared memory changes */
extern struct cisam_sn *sn_ptr;		/* pointer to street name file structure */
extern struct cisam_ad *ad_ptr;		/* pointer to address file structure */
extern struct cisam_pl *pl_ptr;		/* pointer to place name file structure */
extern struct cisam_lo *lo_ptr;		/* pointer to place name file structure */
extern struct cisam_sz *sz_ptr;		/* pointer to place name file structure */
extern struct cisam_sn sn_buf;		
extern struct cisam_ad ad_buf;		
extern struct cisam_pl pl_buf;		
extern struct cisam_lo lo_buf;		
extern struct cisam_sl sz_buf;		
extern struct cisam_sn *temp_sn_ptr;		/* pointer to street name file structure */
extern struct cisam_ad *temp_ad_ptr;		/* pointer to address file structure */
extern struct cisam_pl *temp_pl_ptr;		/* pointer to place name file structure */
extern struct cisam_lo *temp_lo_ptr;		/* pointer to place name file structure */
extern struct cisam_sz *temp_sz_ptr;		/* pointer to place name file structure */
extern char read_buf[];			/* buffer for reading fields */
extern char req_buf[];			/* buffer for reading fields */
extern FILE *trace_fp;			/* trace file pointer */
extern char trace_str[];		/* trace string */
extern int  adr[],high[],low[];
extern int  ab_flags_1,ab_flags_2,al_flags_1,al_flags_2;
extern int  pl_flags_1,pl_flags_2,pl_flags_3;
extern int  ad_flags_1,ad_flags_2,ad_flags_3;
extern int  ad_flags_4,ad_flags_5,ad_flags_6;
extern int  ad_flags_7,ad_flags_8,ad_flags_9;
extern int  ad_pre,ad_post,ad_city,ad_oe,ad_high,ad_low;
extern int  ad_zip,ad_subzone,ad_comment,ad_x,ad_y,ad_map,ad_pb;
extern int  pl_name,pl_pre,pl_post,pl_city,pl_oe,pl_street;
extern int  pl_suf,pl_zip,pl_subzone,pl_comment,pl_x;
extern int  pl_y,pl_map,pl_pb,pl_loc;
extern int  query;
extern int  doing_add;
extern int  pre_flag,post_flag,city_flag,oe_flag,nbr_flag,suf_flag,high_flag;
extern int  zip_flag,subzone_flag,x_flag,y_flag,map_flag,loc_flag,low_flag;
extern int name_flag;
extern int address_record_nbr;
extern int index_flag;
extern int temp_index;
extern int type_flag;
extern int doing_add;
extern int piu;			/* prompt in use */
extern int key_val;			/* value of key entered */
extern prt_error();
extern clear_err_line();
extern int err_n_fld;
extern int legal_city,legal_zone;	/* must be true to enter a place name */
int update_piu();
int valid_cmd();
char *str_to_upper();
int high_number,low_number;
extern int  temp_flag1;
extern int temp_flag;
int 	check_dummy(),check_index(),check_name(),check_type(),check_abbrv(),check_alias();
int	check_pl_name(),check_pl_city(),check_pl_oe();
int 	check_st_nbr(),check_pl_sbz(),check_pl_cmnt();
int 	check_pl_map(),check_pl_pb(),check_loc();
int 	check_ad_pre(),check_ad_post(),check_ad_city(),check_ad_oe(),check_hi_nbr();
int 	check_lo_nbr(),check_ad_zip(),check_ad_sbz(),check_ad_cmnt(),check_ad_x_cd();
int 	check_ad_y_cd(),check_ad_map(),check_ad_pb();

/*  Declare an array of pointers to functions which check the field
 *  entered for validity.
 */

int (*check_field[])() = { check_dummy, check_index,check_name, check_type, 
		   check_abbrv, check_abbrv,check_alias,check_alias,
		   check_pl_name, check_pl_city, 
		   check_pl_oe,check_st_nbr, 
		   check_pl_sbz, check_pl_cmnt, 
		   check_pl_map, check_pl_pb, check_loc, check_pl_name,
	 	   check_pl_city,check_pl_oe, 
		   check_st_nbr, check_pl_sbz, 
	       	   check_pl_cmnt, check_pl_map,
		   check_pl_pb, check_loc, check_pl_name,
	       	   check_pl_city, check_pl_oe, check_st_nbr,
		   check_pl_sbz,check_pl_cmnt,
		   check_pl_map,check_pl_pb,
		   check_loc, check_ad_pre, check_ad_post, check_ad_city,
	   	   check_ad_oe, check_lo_nbr, check_hi_nbr, check_ad_zip, 
		   check_ad_sbz, check_ad_cmnt,check_ad_x_cd, check_ad_y_cd,
		   check_ad_map, check_ad_pb, check_ad_pre, check_ad_post, 
		   check_ad_city, check_ad_oe, check_lo_nbr, check_hi_nbr,
		   check_ad_zip, check_ad_sbz, check_ad_cmnt,check_ad_x_cd,
	   	   check_ad_y_cd, check_ad_map, check_ad_pb, check_ad_pre, 
		   check_ad_post, check_ad_city, check_ad_oe, check_lo_nbr, 
		   check_hi_nbr, check_ad_zip, check_ad_sbz, check_ad_cmnt,
		   check_ad_x_cd, check_ad_y_cd, check_ad_map, check_ad_pb,
		   check_ad_pre, check_ad_post, check_ad_city, check_ad_oe, 
		   check_lo_nbr, check_hi_nbr, check_ad_zip, check_ad_sbz, 
		   check_ad_cmnt,check_ad_x_cd, check_ad_y_cd, check_ad_map, 
		   check_ad_pb, check_ad_pre, check_ad_post, check_ad_city,
		   check_ad_oe, check_lo_nbr, check_hi_nbr, check_ad_zip, 
		   check_ad_sbz, check_ad_cmnt,check_ad_x_cd, check_ad_y_cd, 
		   check_ad_map, check_ad_pb, check_ad_pre, check_ad_post, 
		   check_ad_city, check_ad_oe, check_lo_nbr, check_hi_nbr,
		   check_ad_zip, check_ad_sbz, check_ad_cmnt,check_ad_x_cd,
		   check_ad_y_cd, check_ad_map, check_ad_pb, check_ad_pre, 
		   check_ad_post, check_ad_city, check_ad_oe, check_lo_nbr, 
		   check_hi_nbr, check_ad_zip, check_ad_sbz, check_ad_cmnt,
		   check_ad_x_cd, check_ad_y_cd, check_ad_map, check_ad_pb, 
		   check_ad_pre, check_ad_post, check_ad_city, check_ad_oe, 
		   check_lo_nbr, check_hi_nbr, check_ad_zip, check_ad_sbz, 
		   check_ad_cmnt,check_ad_x_cd, check_ad_y_cd, check_ad_map, 
		   check_ad_pb, check_ad_pre, check_ad_post, check_ad_city, 
	   	   check_ad_oe, check_lo_nbr, check_hi_nbr, check_ad_zip, 
		   check_ad_sbz, check_ad_cmnt,check_ad_x_cd, check_ad_y_cd, 
		   check_ad_map, check_ad_pb}; 
			 	 


int check_dummy (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_index (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{

#ifdef	DEBUG
	sprintf(trace_str,"Entering check_index\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			if (!numeric(read_buf))	{
				prt_error(ERROR_N_FLD);
				pre_flag = 0;
				index_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_index- 1\n");
	TRACE(trace_str);
#endif
				return;
			}
			ipc_required = 1;
			pre_flag = 1;
			clear_err_line();
			sn_ptr->index = atoi(read_buf);	
			mvprintw(1,1,"%18d",sn_ptr->index);
			refresh();
			index_flag = 1;
			update_piu();
	}
	else		{
		update_piu();
	}
		
}

int check_name (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_name\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			ipc_required = 1;
			strcpy(sn_ptr->name,str_to_upper(read_buf));
			mvprintw(D_ST_NAME_ROW,D_ST_NAME_COL,"%s",sn_ptr->name);
			refresh();
			name_flag = 1;
			++piu;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_name - 2\n");
	TRACE(trace_str);
#endif
	}

	else	{
		if (!name_flag)	{
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_1, "You must enter street name"));
			piu = 2;
		}
		else
			++piu;
	}
}

int check_type (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_type\n");
	TRACE(trace_str);
#endif

#ifdef SCANDINAVIA
	update_piu();
	return;
#else
	if(*field_entered)	{
			clear_err_line();
			ipc_required = 1;
			strcpy(sn_ptr->str_type,str_to_upper(read_buf));
			mvprintw(D_ST_NAME_ROW,D_TYPE_COL,"%s",sn_ptr->str_type);
			type_flag = 1;
			refresh();
			if ((key_val != KEY_UP) && (key_val != ESC))
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
			if (key_val == KEY_UP)
				--piu;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_type - 2\n");
	TRACE(trace_str);
#endif
	}

	else	{
		if (!type_flag)
			strcpy(sn_ptr->str_type,"  ");
		if ((!type_flag) && (key_val != KEY_UP)) 	{
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_3, "You must enter street type"));
			piu = 3;
		}
		if (key_val == KEY_UP)
			--piu;
	}
#endif /* #else */
}

int check_abbrv (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_abbrev\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			ipc_required = 1;
			strcpy(sn_ptr->name,str_to_upper(read_buf));
			if (piu == 4)	{
				mvprintw(D_ST_NAME_ROW,D_ABBRV_1_COL,"%s",sn_ptr->name);
				ab_flags_1 = 1;
			}
			else	{
				mvprintw(D_ST_NAME_ROW+1,D_ABBRV_2_COL,"%s",sn_ptr->name);
				ab_flags_2 = 1;
			}
			refresh();

#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_abbrev - 2\n");
	TRACE(trace_str);
#endif
	}

	else	{
		update_piu();
		if ((piu == 5) && (ab_flags_1))
			piu = 4;
		if ((piu == 6) && (ab_flags_2))
			piu = 5;
		if ((piu == 6) || (piu == 3))
			piu = 4;
	}


}

int check_alias (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_alias\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			ipc_required = 1;
			strcpy(sn_ptr->name,str_to_upper(read_buf));
			if (piu == 6)	{
				mvprintw(D_ST_NAME_ROW,D_ALIAS_1_COL,"%s",sn_ptr->name);
				al_flags_1 = 1;
			}
			else	{
				mvprintw(D_ST_NAME_ROW+1,D_ALIAS_2_COL,"%s",sn_ptr->name);
				al_flags_2 = 1;
			}
			refresh();
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_alias - 2\n");
	TRACE(trace_str);
#endif
	}
	else	{
		update_piu();
		if ((piu == F_ALIAS_2) && (al_flags_1))
			piu = F_ALIAS_1;
		if ((piu == F_PL_NAME_1) && (al_flags_2))
			piu = F_ALIAS_2;
		if ((piu == F_PL_NAME_1) || (piu == F_ABBRV_2))
			piu = F_ALIAS_1;
	}


}


/*------------------------------------------------------------------
 *	check pl name
 *------------------------------------------------------------------
 */

check_pl_name (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{

	if (*field_entered)	
	  {
		clear_err_line();
		ipc_required = 1;
		strcpy(pl_ptr->name,str_to_upper(read_buf));

/*
 *		fix
 *		must enter a placename
 */
		if (string_null(read_buf, sizeof(pl_ptr->name)))
		  {
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_46, 
					"You must enter value"));
			pl_name = 0;
			return(-1);
		  }

		switch(piu)	
		  {
			case F_PL_NAME_1:
				mvprintw(D_PL_NAME_1_ROW,D_PL_NAME_COL,"%s",pl_ptr->name);
				refresh();
				pl_flags_1 = 1;
				update_piu();
				if (key_val == KEY_UP)
					piu = F_PL_NAME_1;
				break;
			case F_PL_NAME_2:
				mvprintw(D_PL_NAME_2_ROW,D_PL_NAME_COL,"%s",pl_ptr->name);
				refresh();
				pl_flags_2 = 1;
				update_piu();
				if (key_val == KEY_UP)
					piu = F_PL_NAME_2;
				break;
			case F_PL_NAME_3:
				mvprintw(D_PL_NAME_3_ROW,D_PL_NAME_COL,"%s",pl_ptr->name);
				refresh();
				pl_flags_3 = 1;
				update_piu();
				if (key_val == KEY_UP)
					piu = F_PL_NAME_3;
				break;
		  }
		pl_name = 1;
	  }
	else	
	  {	
/*
 *		fix
 *		must enter a placename
 */
		if (string_null(read_buf, sizeof(pl_ptr->name))) 
		  {
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_46, 
					"You must enter value"));
			pl_name = 0;
			return(-1);
		  }

		switch(piu)	
		  {
			case F_PL_NAME_1:
				update_piu();
				if (key_val == KEY_UP)
					piu = F_PL_NAME_1;
				break;
			case F_PL_NAME_2:
				update_piu();
				if ((key_val == KEY_UP) && (pl_flags_2))	
				  {
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET,
							GEO_MAINT_2, "Enter ESC to execute"));
					piu = F_PL_NAME_2;
				  }
				break;
			case F_PL_NAME_3:
				update_piu();
				if ((key_val == KEY_UP) && (pl_flags_3))	
				  {
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, 
							GEO_MAINT_2, "Enter ESC to execute"));
					piu = F_PL_NAME_3;
				  }
				break;
		  }
	  }
}


/*------------------------------------------------------------------
 *	check pl pre
 *------------------------------------------------------------------
 */

/* 
check_pl_pre (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	
	char *read_buf; 
	int key_val;    
	int *field_entered; 
{

	clear_err_line();

	if(*field_entered)	
	  {
		str_to_upper(read_buf);

		if ((strcmp(read_buf,
			catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_4, "W"))!= 0) &&
			(strcmp(read_buf,"  ")!= 0)  && 
			(strcmp(read_buf," ") != 0) &&
			(strcmp(read_buf,
			catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_5, "E"))!= 0) &&
			(strcmp(read_buf,
			catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_6, "N"))!= 0) &&
			(strcmp(read_buf,
			catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_7, "S"))!= 0) &&
			(strcmp(read_buf,
			catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_8, "NW"))!= 0) &&
			(strcmp(read_buf,
			catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_9, "NE"))!= 0) &&
			(strcmp(read_buf,
			catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_10,"SE"))!= 0) &&
			(strcmp(read_buf,
			catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_11,"SW"))!= 0))
		  {
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_12, 
					"Invalid pre direction"));
			pre_flag = 0;
			return(-1);
		  }

		strcpy(pl_ptr->str_pre_dir,read_buf);
		pre_flag = 1;

		switch(piu)	
		  {
			case F_PL_PRE_1:
				mvprintw(D_PL_NAME_1_ROW,D_PL_PRE_COL,"%s",pl_ptr->str_pre_dir);
				pl_flags_1 = 1;
				break;
			case F_PL_PRE_2:
				mvprintw(D_PL_NAME_2_ROW,D_PL_PRE_COL,"%s",pl_ptr->str_pre_dir);
				pl_flags_2 = 1;
				break;
			case F_PL_PRE_3:
				mvprintw(D_PL_NAME_3_ROW,D_PL_PRE_COL,"%s",pl_ptr->str_pre_dir);
				pl_flags_3 = 1;
				break;
		  }

		refresh();
		update_piu();
		pl_pre = 1;
	  }
	else	
	  {
		update_piu();

		if ((!pre_flag) && (key_val != KEY_UP))	
		  {
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, 
					GEO_MAINT_12, "Invalid pre direction"));
			--piu;
			return(-1);
		  }
	  }
}

**/

/*------------------------------------------------------------------
 *	check pl post
 *------------------------------------------------------------------
 */

/***
check_pl_post (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	
	char *read_buf; 
	int key_val;    
	int *field_entered; 
{

	if(*field_entered)	
	  {
		clear_err_line();
		ipc_required = 1;
		str_to_upper(read_buf);

		if ((strcmp(read_buf,
			catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_4, "W"))!= 0) &&
			(strcmp(read_buf,"  ")!= 0) && 
			(strcmp(read_buf," ") != 0) &&
			(strcmp(read_buf,
			catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_5, "E"))!= 0) &&
			(strcmp(read_buf,
			catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_6, "N"))!= 0) &&
			(strcmp(read_buf,
			catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_7, "S"))!= 0) &&
			(strcmp(read_buf,
			catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_8, "NW"))!=0) &&
			(strcmp(read_buf,
			catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_9, "NE"))!=0) &&
			(strcmp(read_buf,
			catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_10,"SE"))!=0) &&
			(strcmp(read_buf,
			catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_11,"SW"))!=0))
		  {
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, 
					GEO_MAINT_13, "Invalid post direction"));
			post_flag = 0;
			return(-1);
		  }

		strcpy(pl_ptr->str_post_dir,read_buf);
		post_flag = 1;

		switch(piu)	
		  {
			case F_PL_POST_1:
				mvprintw(D_PL_NAME_1_ROW,D_PL_POST_COL,"%s",
						pl_ptr->str_post_dir);
				pl_flags_1 = 1;
				break;
			case F_PL_POST_2:
				mvprintw(D_PL_NAME_2_ROW,D_PL_POST_COL,"%s",
						pl_ptr->str_post_dir);
				pl_flags_2 = 1;
				break;
			case F_PL_POST_3:
				mvprintw(D_PL_NAME_3_ROW,D_PL_POST_COL,"%s",
						pl_ptr->str_post_dir);
				pl_flags_3 = 1;
				break;
		  }

		refresh();
		update_piu();
		pl_post = 1;
	  }
	else
	  {
		update_piu();

		if ((!post_flag) && (key_val != KEY_UP))	
		  {
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, 
					GEO_MAINT_13, "Invalid post direction"));
			--piu;
			return(-1);
		  }
	  }
}
**/


/*------------------------------------------------------------------
 *	check pl city
 *------------------------------------------------------------------
 */

check_pl_city (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{

	legal_city = 0;	

	if (*field_entered)	
	  {
		clear_err_line();
		ipc_required = 1;
		str_to_upper(read_buf);

		strcpy(temp_lo_ptr->code,read_buf);
		if ((db(LOCALTY_FILE_ID,READ_KEYS,&lo_key1,ISEQUAL,0)) < 0)	
		  {
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, 
					GEO_MAINT_14, "Invalid city code"));
			city_flag = 0;
			return(-1);
		  }

		strcpy(pl_ptr->str_city,temp_lo_ptr->act_city_code);
		legal_city = city_flag = 1;

		switch(piu)	
		  {
			case F_PL_CITY_1:
				mvprintw(D_PL_NAME_1_ROW,D_PL_CITY_COL,"%s",pl_ptr->str_city);
				pl_flags_1 = 1;
				break;
			case F_PL_CITY_2:
				mvprintw(D_PL_NAME_2_ROW,D_PL_CITY_COL,"%s",pl_ptr->str_city);
				pl_flags_2 = 1;
				break;
			case F_PL_CITY_3:
				mvprintw(D_PL_NAME_3_ROW,D_PL_CITY_COL,"%s",pl_ptr->str_city);
				pl_flags_3 = 1;
				break;
		  }

		refresh();
		update_piu();
		pl_city = 1;
	  }
	else	
	  {
		legal_city = city_flag;

/*
 *		fix
 *		city is required
 */
		if (string_null(read_buf, sizeof(pl_ptr->str_city)))
		  {
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, 
					GEO_MAINT_14, "Invalid city code"));
			city_flag = 0;
			return(-1);
		  }

		update_piu();

		if ((!city_flag) && (key_val != KEY_UP))	
		  {
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, 
					GEO_MAINT_14, "Invalid city code"));
			--piu;
			return(-1);
		  }
	  }
}


/*--------------------------------------------------------------------
 *	check pl oe
 *--------------------------------------------------------------------
 */

check_pl_oe (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
    char c;


	c = Toupper(read_buf[0]);

	clear_err_line();

	if(*field_entered)	
	  {
		if (c == ADDRESS_ODD || c == ADDRESS_EVEN || c == ADDRESS_BOTH) 
		  {
			ipc_required = 1;
			oe_flag = 1;
			pl_ptr->odd_even = c;

			switch(piu)	
			  {
				case F_PL_OE_1:
					mvprintw(D_PL_NAME_1_ROW,D_PL_OE_COL,"%c",pl_ptr->odd_even);
					pl_flags_1 = 1;
					break;
				case F_PL_OE_2:
					mvprintw(D_PL_NAME_2_ROW,D_PL_OE_COL,"%c",pl_ptr->odd_even);
					pl_flags_2 = 1;
					break;
				case F_PL_OE_3:
					mvprintw(D_PL_NAME_3_ROW,D_PL_OE_COL,"%c",pl_ptr->odd_even);
					pl_flags_3 = 1;
					break;
			  }

			refresh();
			update_piu();
			pl_oe = 1;
		  }
		else	
		  {
			clear_err_line();		
			prt_error(ERROR_N_FLD);
			oe_flag = 0;
			return(-1);
		  }
	  }
	else	
	  {	
/*
 *		fix
 *		odd/even not required - why does it even exist for placenames?
 */
		update_piu();

		if ((!oe_flag) && (key_val != KEY_UP))	
		  {
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, 
					GEO_MAINT_15, "Error in field"));
			--piu;
			return(-1);
		  }
	  }
}



/*--------------------------------------------------------------------
 *	check st nbr
 *--------------------------------------------------------------------
 */

check_st_nbr (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int aa;


	legal_zone = 0; 

	if(*field_entered)	
	  {
		clear_err_line();
/*
 *		fix
 *		street name is required
 */
		if (string_null(read_buf, sizeof(read_buf)) || !numeric(read_buf))	
		  {
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, 
					GEO_MAINT_16, "Invalid street number"));
			nbr_flag = 0;
			return(-1);
		  }

		nbr_flag = 1;
		clear_err_line();
		pl_ptr->street_nbr = atoi(read_buf);	

		switch(piu)	
		  {
			case F_ST_NBR_1:
				mvprintw(D_PL_NAME_1_ROW,D_ST_NBR_COL,"%6d",pl_ptr->street_nbr);
				pl_flags_1 = 1;
				if (pl_flags_1)	
				  {
					aa = get_subzone();
					if (aa == 0)	
					  {
						legal_zone = subzone_flag = 1;
						mvprintw(D_PL_NAME_1_ROW,D_PL_SBZ_COL,"%s",
								pl_ptr->str_subzone);
						mvprintw(D_PL_NAME_1_ROW,D_PL_MAP_COL,"%3d",
								pl_ptr->map_ref_page);
						mvprintw(D_PL_NAME_1_ROW,D_LOC_COL,"%c",
								pl_ptr->local);
						clear_flds(F_PL_PB_1,F_PL_PB_1);
						mvprintw(D_PL_NAME_1_ROW,D_PL_PB_COL,"%s",
								pl_ptr->map_ref_block);
						pl_flags_1 = 1;
					  }
				  }
				break;
			case F_ST_NBR_2:
				mvprintw(D_PL_NAME_2_ROW,D_ST_NBR_COL,"%6d",
						pl_ptr->street_nbr);
				pl_flags_2 = 1;

				if (pl_flags_2)	
				  {
					aa = get_subzone();
					if (aa == 0)	
					  {
						legal_zone = subzone_flag = 1;
						mvprintw(D_PL_NAME_2_ROW,D_PL_SBZ_COL,"%s",
								pl_ptr->str_subzone);
						mvprintw(D_PL_NAME_2_ROW,D_PL_MAP_COL,"%3d",
								pl_ptr->map_ref_page);
						mvprintw(D_PL_NAME_2_ROW,D_LOC_COL,"%c",
								pl_ptr->local);
						clear_flds(F_PL_PB_2,F_PL_PB_2);
						mvprintw(D_PL_NAME_2_ROW,D_PL_PB_COL,"%s",
								pl_ptr->map_ref_block);
						pl_flags_2 = 1;
					  }
				  }
				break;
			case F_ST_NBR_3:
				mvprintw(D_PL_NAME_3_ROW,D_ST_NBR_COL,"%6d",pl_ptr->street_nbr);
				pl_flags_3 = 1;
				if (pl_flags_3)	
				  {
					aa = get_subzone();
					if (aa == 0)	
					  {
						legal_zone = subzone_flag = 1;
						mvprintw(D_PL_NAME_3_ROW,D_PL_SBZ_COL,"%s",
								pl_ptr->str_subzone);
						mvprintw(D_PL_NAME_3_ROW,D_PL_MAP_COL,"%3d",
								pl_ptr->map_ref_page);
						mvprintw(D_PL_NAME_3_ROW,D_LOC_COL,"%c",
								pl_ptr->local);
						clear_flds(F_PL_PB_3,F_PL_PB_3);
						mvprintw(D_PL_NAME_3_ROW,D_PL_PB_COL,"%s",
								pl_ptr->map_ref_block);
						pl_flags_3 = 1;
					  }
				  }
				break;
		  }

		refresh();
		update_piu();
		pl_street = 1;
	  }
	else	
	  {
/* 
 *		fix 
 *		street number is required
 */
		if (string_null(read_buf))	
		  {
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, 
					GEO_MAINT_16, "Invalid street number"));
			nbr_flag = 0;
			return(-1);
		  }

		switch(piu)	
		  {
			case F_ST_NBR_1:
				if (pl_flags_1)	
				  {
					aa = get_subzone();
					if (aa == 0)	
					  {
						legal_zone = subzone_flag = 1;
						mvprintw(D_PL_NAME_1_ROW,D_PL_SBZ_COL,"%s",
								pl_ptr->str_subzone);
						mvprintw(D_PL_NAME_1_ROW,D_PL_MAP_COL,"%3d",
								pl_ptr->map_ref_page);
						mvprintw(D_PL_NAME_1_ROW,D_LOC_COL,"%c",
								pl_ptr->local);
						clear_flds(F_PL_PB_1,F_PL_PB_1);
						mvprintw(D_PL_NAME_1_ROW,D_PL_PB_COL,"%s",
								pl_ptr->map_ref_block);
					  }
				  }
				break;
			case F_ST_NBR_2:
				if (pl_flags_2)	
				  {
					aa = get_subzone();
					if (aa == 0)	
					  {
						legal_zone = subzone_flag = 1;
						mvprintw(D_PL_NAME_2_ROW,D_PL_SBZ_COL,"%s",
								pl_ptr->str_subzone);
						mvprintw(D_PL_NAME_2_ROW,D_PL_MAP_COL,"%3d",
								pl_ptr->map_ref_page);
						mvprintw(D_PL_NAME_2_ROW,D_LOC_COL,"%c",
								pl_ptr->local);
						clear_flds(F_PL_PB_2,F_PL_PB_2);
						mvprintw(D_PL_NAME_2_ROW,D_PL_PB_COL,"%s",
								pl_ptr->map_ref_block);
					  }
				  }
				break;
			case F_ST_NBR_3:
				if (pl_flags_3)	
				  {
					aa = get_subzone();
					if (aa == 0)	
					  {
						legal_zone = subzone_flag = 1;
						mvprintw(D_PL_NAME_3_ROW,D_PL_SBZ_COL,"%s",
								pl_ptr->str_subzone);
						mvprintw(D_PL_NAME_3_ROW,D_PL_MAP_COL,"%3d",
								pl_ptr->map_ref_page);
						mvprintw(D_PL_NAME_3_ROW,D_LOC_COL,"%c",
								pl_ptr->local);
						clear_flds(F_PL_PB_3,F_PL_PB_3);
						mvprintw(D_PL_NAME_3_ROW,D_PL_PB_COL,"%s",
								pl_ptr->map_ref_block);
					  }
				  }
				break;
		  }

		refresh();
		update_piu();

		if ((!nbr_flag) && (key_val != KEY_UP))	
		  {
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, 
					GEO_MAINT_16, "Invalid street number"));
			--piu;
			return(-1);
		  }
	  }
}


/*--------------------------------------------------------------------
 *	check suf
 *--------------------------------------------------------------------
 */

/** 
check_suf (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	
	char *read_buf; 
	int key_val;    
	int *field_entered; 
{

	clear_err_line();
	if(*field_entered)	
	  {
		if (read_buf[0] != 'x' &&
		    read_buf[0] != 'X' &&
		    read_buf[0] != 'y' && 
		    read_buf[0] != 'Y' &&
		    read_buf[0] != 'z' &&
		    read_buf[0] != 'Z') 	
		  {
			ipc_required = 1;
			suf_flag = 1;
			pl_ptr->str_nbr_suf = read_buf[0];

			switch(piu)	
			  {
				case F_SUF_1:
					mvprintw(D_PL_NAME_1_ROW,D_SUF_COL,"%c",
							pl_ptr->str_nbr_suf);
					pl_flags_1 = 1;
					break;
				case F_SUF_2:
					mvprintw(D_PL_NAME_2_ROW,D_SUF_COL,"%c",
							pl_ptr->str_nbr_suf);
					pl_flags_2 = 1;
					break;
				case F_SUF_3:
					mvprintw(D_PL_NAME_3_ROW,D_SUF_COL,"%c",
							pl_ptr->str_nbr_suf);
					pl_flags_3 = 1;
					break;
			  }
			refresh();
			update_piu();
			pl_suf = 1;
		  }
		else	
		  {
			clear_err_line();		
			prt_error(ERROR_N_FLD);
			suf_flag = 0;
			return(-1);
		  }
	  }
	else	
	  {

		update_piu();
		if ((!suf_flag) && (key_val != KEY_UP))	
		  {
			--piu;
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, 
					GEO_MAINT_17, "Invalid suffix"));
			return(-1);
		  }
	  }
}

**/

/*--------------------------------------------------------------------
 *	check pl zip
 *--------------------------------------------------------------------
 */

/**
   
check_pl_zip (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	
	char *read_buf; 
	int key_val;    
	int *field_entered; 
{


	if(*field_entered)	
	  {
		clear_err_line();

		if (!numeric(read_buf))	
		  {
			prt_error(ERROR_N_FLD);
			zip_flag = 0;
			return(-1);
		  }

		zip_flag = 1;
		clear_err_line();
		pl_ptr->str_zip = atoi(read_buf);	

		switch(piu)	
		  {
			case F_PL_ZIP_1:
				mvprintw(D_PL_NAME_1_ROW,D_PL_ZIP_COL,"%5d",pl_ptr->str_zip);
				pl_flags_1 = 1;
				break;
			case F_PL_ZIP_2:
				mvprintw(D_PL_NAME_2_ROW,D_PL_ZIP_COL,"%5d",pl_ptr->str_zip);
				pl_flags_2 = 1;
				break;
			case F_PL_ZIP_3:
				mvprintw(D_PL_NAME_3_ROW,D_PL_ZIP_COL,"%5d",pl_ptr->str_zip);
				pl_flags_3 = 1;
				break;
		  }
		refresh();
		update_piu();
		pl_zip = 1;
	  }
	else	
	  {
		update_piu();
		if ((!zip_flag) && (key_val != KEY_UP))	
		  {
			--piu;
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, 
					GEO_MAINT_18, "Invalid zip code"));
			return(-1);
		  }
	  }
}
**/

/*--------------------------------------------------------------------
 *	check pl sbz
 *--------------------------------------------------------------------
 */

check_pl_sbz (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int  aa;


	if(*field_entered)	
	  {
		clear_err_line();
		ipc_required = 1;
		str_to_upper(read_buf);
		strcpy(temp_sz_ptr->nbr,read_buf);

		if ((db(SUBZONE_FILE_ID,READ_KEYS,&sz_key1,ISEQUAL,0)) < 0)	
		  {
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, 
					GEO_MAINT_19, "Invalid subzone number"));
			subzone_flag = 0;
			return(-1);
		  }

		strcpy(pl_ptr->str_subzone,read_buf);
		legal_zone = subzone_flag = 1;
		update_piu();
		pl_subzone = 1;

		switch(piu)	
		  {
			case F_PL_SBZ_1:
				mvprintw(D_PL_NAME_1_ROW,D_PL_SBZ_COL,"%s",pl_ptr->str_subzone);
				pl_flags_1 = 1;
				break;
			case F_PL_SBZ_2:
				mvprintw(D_PL_NAME_2_ROW,D_PL_SBZ_COL,"%s",pl_ptr->str_subzone);
				pl_flags_2 = 1;
				break;
			case F_PL_SBZ_3:
				mvprintw(D_PL_NAME_3_ROW,D_PL_SBZ_COL,"%s",pl_ptr->str_subzone);
				pl_flags_3 = 1;
				break;
		  }
		refresh();
	  }
	else	
	  {
		switch(piu)	
		  {
			case F_PL_SBZ_1:
				if (pl_flags_1)	
				  {
					aa = get_subzone();
					if (aa == 0)	
					  {
						subzone_flag = 1;
						mvprintw(D_PL_NAME_1_ROW,D_PL_SBZ_COL,"%s",
								pl_ptr->str_subzone);
						pl_flags_1 = 1;
					  }
				  }
				break;
			case F_PL_SBZ_2:
				if (pl_flags_2)	
				  {
					aa = get_subzone();
					if (aa == 0)	
					  {
						subzone_flag = 1;
						mvprintw(D_PL_NAME_2_ROW,D_PL_SBZ_COL,"%s",
								pl_ptr->str_subzone);
						pl_flags_2 = 1;
					  }
				  }
				break;
			case F_PL_SBZ_3:
				if (pl_flags_3)	
				  {
					aa = get_subzone();
					if (aa == 0)	
					  {
						subzone_flag = 1;
						mvprintw(D_PL_NAME_3_ROW,D_PL_SBZ_COL,"%s",
								pl_ptr->str_subzone);
						pl_flags_3 = 1;
					  }
				  }
				break;
		  }

		refresh();
		update_piu();

		if ((!subzone_flag) && (key_val != KEY_UP))	
		  {
			--piu;
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, 
					GEO_MAINT_19, "Invalid subzone number"));
			return(-1);
		  }
	  }
}

int check_pl_cmnt (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_pl_cmnt\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			ipc_required = 1;
			strcpy(pl_ptr->str_comment,str_to_upper(read_buf));
		switch(piu)	{
			case F_PL_CMNT_1:
					mvprintw(D_PL_NAME_1_ROW,D_PL_CMNT_COL,"%s",pl_ptr->str_comment);
					pl_flags_1 = 1;
					break;
			case F_PL_CMNT_2:
					mvprintw(D_PL_NAME_2_ROW,D_PL_CMNT_COL,"%s",pl_ptr->str_comment);
					pl_flags_2 = 1;
					break;
			case F_PL_CMNT_3:
					mvprintw(D_PL_NAME_3_ROW,D_PL_CMNT_COL,"%s",pl_ptr->str_comment);
					pl_flags_3 = 1;
					break;
		}
			refresh();
			update_piu();
			pl_comment = 1;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_pl_cmnt - 2\n");
	TRACE(trace_str);
#endif
	}

	else	
		update_piu();
}

/**
int check_pl_x_cd (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	
	char *read_buf; 
	int key_val;    
	int *field_entered; 
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_pl_x_cd\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			if (!numeric(read_buf))	{
				prt_error(ERROR_N_FLD);
				x_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_pl_x_cd- 1\n");
	TRACE(trace_str);
#endif
				return(-1);
			}
			x_flag = 1;
			clear_err_line();
			pl_ptr->str_x_coord = atoi(read_buf);	
		switch(piu)	{
			case F_PL_X_CD_1:
					mvprintw(D_PL_NAME_1_ROW,D_PL_X_CD_COL,"%8d",pl_ptr->str_x_coord);
					pl_flags_1 = 1;
					break;
			case F_PL_X_CD_2:
					mvprintw(D_PL_NAME_2_ROW,D_PL_X_CD_COL,"%8d",pl_ptr->str_x_coord);
					pl_flags_2 = 1;
					break;
			case F_PL_X_CD_3:
					mvprintw(D_PL_NAME_3_ROW,D_PL_X_CD_COL,"%8d",pl_ptr->str_x_coord);
					pl_flags_3 = 1;
					break;
		}
			refresh();
			update_piu();
			pl_x = 1;
	}
	else	{
		switch(piu)	{
			case F_PL_X_CD_1:
					if(pl_flags_1)	{
						mvprintw(D_PL_NAME_1_ROW,D_PL_X_CD_COL,"%8d",pl_ptr->str_x_coord);
						pl_flags_1 = 1;
					}
						break;
			case F_PL_X_CD_2:
					if(pl_flags_2)	{
						mvprintw(D_PL_NAME_2_ROW,D_PL_X_CD_COL,"%8d",pl_ptr->str_x_coord);
						pl_flags_2 = 1;
					}
					break;
			case F_PL_X_CD_3:
					if(pl_flags_3)	{
						mvprintw(D_PL_NAME_3_ROW,D_PL_X_CD_COL,"%8d",pl_ptr->str_x_coord);
						pl_flags_3 = 1;
					}
					break;
		}
			refresh();
		update_piu();
		if ((!x_flag) && (key_val != KEY_UP))	{
			--piu;
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_20, "Invalid x coordinate"));
			return(-1);
		}
	}
}

**/
/**
int check_pl_y_cd (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	
	char *read_buf; 
	int key_val;    
	int *field_entered; 
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_pl_y_cd\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			if (!numeric(read_buf))	{
				prt_error(ERROR_N_FLD);
				y_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_pl_y_cd- 1\n");
	TRACE(trace_str);
#endif
				return(-1);
			}
			y_flag = 1;
			clear_err_line();
			pl_ptr->str_y_coord = atoi(read_buf);	
		switch(piu)	{
			case F_PL_Y_CD_1:
					mvprintw(D_PL_NAME_1_ROW,D_PL_Y_CD_COL,"%8d",pl_ptr->str_y_coord);
					pl_flags_1 = 1;
					break;
			case F_PL_Y_CD_2:
					mvprintw(D_PL_NAME_2_ROW,D_PL_Y_CD_COL,"%8d",pl_ptr->str_y_coord);
					pl_flags_2 = 1;
					break;
			case F_PL_Y_CD_3:
					mvprintw(D_PL_NAME_3_ROW,D_PL_Y_CD_COL,"%8d",pl_ptr->str_y_coord);
					pl_flags_3 = 1;
					break;
		}
			refresh();
			update_piu();
			pl_y = 1;
	}
	else	{
		switch(piu)	{
			case F_PL_Y_CD_1:
					if (pl_flags_1)	{
						mvprintw(D_PL_NAME_1_ROW,D_PL_Y_CD_COL,"%8d",pl_ptr->str_y_coord);
						pl_flags_1 = 1;
					}
					break;
			case F_PL_Y_CD_2:
					if (pl_flags_2)	{
						mvprintw(D_PL_NAME_2_ROW,D_PL_Y_CD_COL,"%8d",pl_ptr->str_y_coord);
						pl_flags_2 = 1;
					}
					break;
			case F_PL_Y_CD_3:
					if (pl_flags_3)	{
						mvprintw(D_PL_NAME_3_ROW,D_PL_Y_CD_COL,"%8d",pl_ptr->str_y_coord);
						pl_flags_3 = 1;
					}
					break;
		}
			refresh();
		update_piu();
		if ((!y_flag) && (key_val != KEY_UP))	{
			--piu;
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_21, "Invalid y coordinate"));
			return(-1);
		}
	}
		
}
**/
int check_pl_map (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
int    num;
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_pl_map\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			if (!numeric(read_buf))	{
				prt_error(ERROR_N_FLD);
				map_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_pl_map- 1\n");
	TRACE(trace_str);
#endif
				return(-1);
			}
			ipc_required = 1;
			map_flag = 1;
			clear_err_line();
			num = atoi(read_buf);	
			pl_ptr->map_ref_page = (short)num;	
		switch(piu)	{
			case F_PL_MAP_1:
					mvprintw(D_PL_NAME_1_ROW,D_PL_MAP_COL,"%3d",pl_ptr->map_ref_page);
					pl_flags_1 = 1;
					break;
			case F_PL_MAP_2:
					mvprintw(D_PL_NAME_2_ROW,D_PL_MAP_COL,"%3d",pl_ptr->map_ref_page);
					pl_flags_2 = 1;
					break;
			case F_PL_MAP_3:
					mvprintw(D_PL_NAME_3_ROW,D_PL_MAP_COL,"%3d",pl_ptr->map_ref_page);
					pl_flags_3 = 1;
					break;
		}
			refresh();
			update_piu();
			pl_map = 1;
	}
	else	{
		switch(piu)	{
			case F_PL_MAP_1:
					if (pl_flags_1)	{
						mvprintw(D_PL_NAME_1_ROW,D_PL_MAP_COL,"%3d",pl_ptr->map_ref_page);
						pl_flags_1 = 1;
					}
					break;
			case F_PL_MAP_2:
					if (pl_flags_2)	{
						mvprintw(D_PL_NAME_2_ROW,D_PL_MAP_COL,"%3d",pl_ptr->map_ref_page);
						pl_flags_2 = 1;
					}
					break;
			case F_PL_MAP_3:
					if (pl_flags_3)	{
						mvprintw(D_PL_NAME_3_ROW,D_PL_MAP_COL,"%3d",pl_ptr->map_ref_page);
						pl_flags_3 = 1;
					}
					break;
		}
			refresh();
		update_piu();
		if ((!map_flag) && (key_val != KEY_UP))	{
			--piu;
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_22, "Invalid map reference page number"));
			return(-1);
		}
	}
}

int check_pl_pb (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_pl_pb\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			ipc_required = 1;
			strcpy(pl_ptr->map_ref_block,str_to_upper(read_buf));
		switch(piu)	{
			case F_PL_PB_1:
					mvprintw(D_PL_NAME_1_ROW,D_PL_PB_COL,"%s",pl_ptr->map_ref_block);
					pl_flags_1 = 1;
					break;
			case F_PL_PB_2:
					mvprintw(D_PL_NAME_2_ROW,D_PL_PB_COL,"%s",pl_ptr->map_ref_block);
					pl_flags_2 = 1;
					break;
			case F_PL_PB_3:
					mvprintw(D_PL_NAME_3_ROW,D_PL_PB_COL,"%s",pl_ptr->map_ref_block);
					pl_flags_3 = 1;
					break;
		}
			refresh();
			update_piu();
			pl_pb = 1;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_pl_pb - 2\n");
	TRACE(trace_str);
#endif
	}

	else	{
		switch(piu)	{
			case F_PL_PB_1:
					if (pl_flags_1)	{
						clear_flds(21,21);
						mvprintw(D_PL_NAME_1_ROW,D_PL_PB_COL,"%s",pl_ptr->map_ref_block);
						pl_flags_1 = 1;
					}
					break;
			case F_PL_PB_2:
					if (pl_flags_2)	{
						clear_flds(36,36);
						mvprintw(D_PL_NAME_2_ROW,D_PL_PB_COL,"%s",pl_ptr->map_ref_block);
						pl_flags_2 = 1;
					}
					break;
			case F_PL_PB_3:
					if (pl_flags_3)	{
						clear_flds(51,51);
						mvprintw(D_PL_NAME_3_ROW,D_PL_PB_COL,"%s",pl_ptr->map_ref_block);
						pl_flags_3 = 1;
					}
					break;
		}
			refresh();
		update_piu();
	}
}

int check_loc (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
    char c;

#ifdef	DEBUG
	sprintf(trace_str,"Entering check_loc\n");
	TRACE(trace_str);
#endif
        c = Toupper(read_buf[0]);
        clear_err_line();
	if(*field_entered)	{

		if (c == YES || c == NO) {
			ipc_required = 1;
			loc_flag = 1;
			pl_ptr->local = c;
		switch(piu)	{
			case F_LOC_1:
					mvprintw(D_PL_NAME_1_ROW,D_LOC_COL,"%c",pl_ptr->local);
					refresh();
					pl_flags_1 = 1;
					update_piu();
					if ((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r'))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_LOC_1;
					}
					break;
			case F_LOC_2:
					mvprintw(D_PL_NAME_2_ROW,D_LOC_COL,"%c",pl_ptr->local);
					refresh();
					pl_flags_2 = 1;
					update_piu();
					if ((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r'))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_LOC_2;
					}
					break;
			case F_LOC_3:
					mvprintw(D_PL_NAME_3_ROW,D_LOC_COL,"%c",pl_ptr->local);
					refresh();
					pl_flags_3 = 1;
					update_piu();
					if ((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r'))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_LOC_3;
					}
					break;
		}
			pl_loc = 1;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_pl_loc - 1\n");
	TRACE(trace_str);
#endif
	}
	else	{
			clear_err_line();		
			prt_error(ERROR_N_FLD);
			loc_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_pl_loc - 2\n");
	TRACE(trace_str);
#endif
			return(-1);
		}
	}



	else	{
		if (!loc_flag)	{
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_15, "Error in field"));
			return(-1);
		}
		switch(piu)	{
			case F_LOC_1:
				update_piu();
				if (((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r')) && (pl_flags_1))	{
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
					piu = F_LOC_1;
				}
				break;
			case F_LOC_2:
				update_piu();
				if (((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r')) && (pl_flags_2))	{
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
					piu = F_LOC_2;
				}
				break;
			case F_LOC_3:
				update_piu();
				if ((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r')) 	{
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
					piu = F_LOC_3;
				}
				break;
		}
	}
}

int check_ad_pre (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_ad_pre\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			ipc_required = 1;
			str_to_upper(read_buf);
if ((strcmp(read_buf,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_4, "W"))!= 0) &&(strcmp(read_buf,"  ")!= 0)  && (strcmp(read_buf," ") != 0) &&(strcmp(read_buf,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_5, "E")) != 0) && (strcmp(read_buf,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_6, "N")) != 0) && (strcmp(read_buf,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_7, "S"))!= 0) && (strcmp(read_buf,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_8, "NW"))!= 0) && (strcmp(read_buf,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_9, "NE"))!= 0) && (strcmp(read_buf,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_10, "SE"))!= 0) && (strcmp(read_buf,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_11, "SW"))!= 0)){
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_12, "Invalid pre direction"));
			pre_flag = 0;
			return(-1);
		}
			strcpy(ad_ptr->str_pre_dir,read_buf);
			pre_flag = 1;
		switch(piu)	{
			case F_AD_PRE_1:
					mvprintw(D_ADD_ROW_1,D_AD_PRE_COL,"%s",ad_ptr->str_pre_dir);
					refresh();
					ad_flags_1 = 1;
					update_piu();
					if (key_val == KEY_UP)	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PRE_1;
					}
					break;
			case F_AD_PRE_2:
					mvprintw(D_ADD_ROW_2,D_AD_PRE_COL,"%s",ad_ptr->str_pre_dir);
					refresh();
					ad_flags_2 = 1;
					update_piu();
					if (key_val == KEY_UP)	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PRE_2;
					}
					break;
			case F_AD_PRE_3:
					mvprintw(D_ADD_ROW_3,D_AD_PRE_COL,"%s",ad_ptr->str_pre_dir);
					refresh();
					ad_flags_3 = 1;
					update_piu();
					if (key_val == KEY_UP)	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PRE_3;
					}
					break;
			case F_AD_PRE_4:
					mvprintw(D_ADD_ROW_4,D_AD_PRE_COL,"%s",ad_ptr->str_pre_dir);
					refresh();
					ad_flags_4 = 1;
					update_piu();
					if (key_val == KEY_UP)	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PRE_4;
					}
					break;
			case F_AD_PRE_5:
					mvprintw(D_ADD_ROW_5,D_AD_PRE_COL,"%s",ad_ptr->str_pre_dir);
					refresh();
					ad_flags_5 = 1;
					update_piu();
					if (key_val == KEY_UP)	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PRE_5;
					}
					break;
			case F_AD_PRE_6:
					mvprintw(D_ADD_ROW_6,D_AD_PRE_COL,"%s",ad_ptr->str_pre_dir);
					refresh();
					ad_flags_6 = 1;
					update_piu();
					if (key_val == KEY_UP)	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PRE_6;
					}
					break;
			case F_AD_PRE_7:
					mvprintw(D_ADD_ROW_7,D_AD_PRE_COL,"%s",ad_ptr->str_pre_dir);
					refresh();
					ad_flags_7 = 1;
					update_piu();
					if (key_val == KEY_UP)	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PRE_7;
					}
					break;
			case F_AD_PRE_8:
					mvprintw(D_ADD_ROW_8,D_AD_PRE_COL,"%s",ad_ptr->str_pre_dir);
					refresh();
					ad_flags_8 = 1;
					update_piu();
					if (key_val == KEY_UP)	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PRE_8;
					}
					break;
			case F_AD_PRE_9:
					mvprintw(D_ADD_ROW_9,D_AD_PRE_COL,"%s",ad_ptr->str_pre_dir);
					refresh();
					ad_flags_9 = 1;
					update_piu();
					if (key_val == KEY_UP)	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PRE_9;
					}
					break;
		}
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_ad_pre - 2\n");
	TRACE(trace_str);
#endif
				ad_pre = 1;
	}

	else	{
		if (!pre_flag)	{
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_12, "Invalid pre direction"));
			return(-1);
		}
		switch(piu)	{
			case F_AD_PRE_1:
				update_piu();
				if (key_val == KEY_UP)
					piu = F_AD_PRE_1;
				break;
			case F_AD_PRE_2:
				update_piu();
				if ((key_val == KEY_UP) && (ad_flags_2))	{
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
					piu = F_AD_PRE_2;
				}
				break;
			case F_AD_PRE_3:
				update_piu();
				if ((key_val == KEY_UP) && (ad_flags_3))	{
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
					piu = F_AD_PRE_3;
				}
				break;
			case F_AD_PRE_4:
				update_piu();
				if ((key_val == KEY_UP) && (ad_flags_4))	{
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
					piu = F_AD_PRE_4;
				}
				break;
			case F_AD_PRE_5:
				update_piu();
				if ((key_val == KEY_UP) && (ad_flags_5))	{
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
					piu = F_AD_PRE_5;
				}
				break;
			case F_AD_PRE_6:
				update_piu();
				if ((key_val == KEY_UP) && (ad_flags_6))	{
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
					piu = F_AD_PRE_6;
				}
				break;
			case F_AD_PRE_7:
				update_piu();
				if ((key_val == KEY_UP) && (ad_flags_7))	{
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
					piu = F_AD_PRE_7;
				}
				break;
			case F_AD_PRE_8:
				update_piu();
				if ((key_val == KEY_UP) && (ad_flags_8))	{
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
					piu = F_AD_PRE_8;
				}
				break;
			case F_AD_PRE_9:
				update_piu();
				if ((key_val == KEY_UP) && (ad_flags_9))	{
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
					piu = F_AD_PRE_9;
				}
				break;
		}
	}
}

int check_ad_post (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_ad_post\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			ipc_required = 1;
			str_to_upper(read_buf);
if ((strcmp(read_buf,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_4, "W"))!= 0) &&(strcmp(read_buf,"  ")!= 0) &&(strcmp(read_buf," ")!=0) &&(strcmp(read_buf,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_5, "E")) != 0) && (strcmp(read_buf,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_6, "N")) != 0) && (strcmp(read_buf,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_7, "S"))!= 0) && (strcmp(read_buf,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_8, "NW"))!= 0) && (strcmp(read_buf,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_9, "NE"))!= 0) && (strcmp(read_buf,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_10, "SE"))!= 0) && (strcmp(read_buf,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_11, "SW"))!= 0)){
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_13, "Invalid post direction"));
			post_flag = 0;
			return(-1);
		}
			strcpy(ad_ptr->str_post_dir,read_buf);
			post_flag = 1;
		switch(piu)	{
			case F_AD_POST_1:
					mvprintw(D_ADD_ROW_1,D_AD_POST_COL,"%s",ad_ptr->str_post_dir);
					ad_flags_1 = 1;
					break;
			case F_AD_POST_2:
					mvprintw(D_ADD_ROW_2,D_AD_POST_COL,"%s",ad_ptr->str_post_dir);
					ad_flags_2 = 1;
					break;
			case F_AD_POST_3:
					mvprintw(D_ADD_ROW_3,D_AD_POST_COL,"%s",ad_ptr->str_post_dir);
					ad_flags_3 = 1;
					break;
			case F_AD_POST_4:
					mvprintw(D_ADD_ROW_4,D_AD_POST_COL,"%s",ad_ptr->str_post_dir);
					ad_flags_4 = 1;
					break;
			case F_AD_POST_5:
					mvprintw(D_ADD_ROW_5,D_AD_POST_COL,"%s",ad_ptr->str_post_dir);
					ad_flags_5 = 1;
					break;
			case F_AD_POST_6:
					mvprintw(D_ADD_ROW_6,D_AD_POST_COL,"%s",ad_ptr->str_post_dir);
					ad_flags_6 = 1;
					break;
			case F_AD_POST_7:
					mvprintw(D_ADD_ROW_7,D_AD_POST_COL,"%s",ad_ptr->str_post_dir);
					ad_flags_7 = 1;
					break;
			case F_AD_POST_8:
					mvprintw(D_ADD_ROW_8,D_AD_POST_COL,"%s",ad_ptr->str_post_dir);
					ad_flags_8 = 1;
					break;
			case F_AD_POST_9:
					mvprintw(D_ADD_ROW_9,D_AD_POST_COL,"%s",ad_ptr->str_post_dir);
					ad_flags_9 = 1;
					break;
		}
			refresh();
			update_piu(); 
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_ad_post - 2\n");
	TRACE(trace_str);
#endif
				ad_post = 1;
	}

	else	{
		update_piu();
		if ((!post_flag) && (key_val != KEY_UP))	{
			--piu;
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_13, "Invalid post direction"));
			return(-1);
		}	
	}
}

int check_ad_city (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_ad_city\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			ipc_required = 1;
			str_to_upper(read_buf);
			strcpy(temp_lo_ptr->code,read_buf);
			if ((db(LOCALTY_FILE_ID,READ_KEYS,&lo_key1,ISEQUAL,0)) < 0)	{
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_15, "Error in field"));
				city_flag = 0;
				return(-1);
			}
			city_flag = 1;
			strcpy(ad_ptr->str_city,temp_lo_ptr->act_city_code);
		switch(piu)	{
			case F_AD_CITY_1:
					mvprintw(D_ADD_ROW_1,D_AD_CITY_COL,"%s",ad_ptr->str_city);
					ad_flags_1 = 1;
					break;
			case F_AD_CITY_2:
					mvprintw(D_ADD_ROW_2,D_AD_CITY_COL,"%s",ad_ptr->str_city);
					ad_flags_2 = 1;
					break;
			case F_AD_CITY_3:
					mvprintw(D_ADD_ROW_3,D_AD_CITY_COL,"%s",ad_ptr->str_city);
					ad_flags_3 = 1;
					break;
			case F_AD_CITY_4:
					mvprintw(D_ADD_ROW_4,D_AD_CITY_COL,"%s",ad_ptr->str_city);
					ad_flags_4 = 1;
					break;
			case F_AD_CITY_5:
					mvprintw(D_ADD_ROW_5,D_AD_CITY_COL,"%s",ad_ptr->str_city);
					ad_flags_5 = 1;
					break;
			case F_AD_CITY_6:
					mvprintw(D_ADD_ROW_6,D_AD_CITY_COL,"%s",ad_ptr->str_city);
					ad_flags_6 = 1;
					break;
			case F_AD_CITY_7:
					mvprintw(D_ADD_ROW_7,D_AD_CITY_COL,"%s",ad_ptr->str_city);
					ad_flags_7 = 1;
					break;
			case F_AD_CITY_8:
					mvprintw(D_ADD_ROW_8,D_AD_CITY_COL,"%s",ad_ptr->str_city);
					ad_flags_8 = 1;
					break;
			case F_AD_CITY_9:
					mvprintw(D_ADD_ROW_9,D_AD_CITY_COL,"%s",ad_ptr->str_city);
					ad_flags_9 = 1;
					break;
		}
			refresh();
			update_piu();
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_ad_city - 2\n");
	TRACE(trace_str);
#endif
			ad_city = 1;
	}

	else	{
		update_piu();
		if ((!city_flag) && (key_val != KEY_UP))	{
			piu = F_AD_CITY_1;
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_14, "Invalid city code"));
			return(-1);
		}
	}
}

int check_ad_oe (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
    char c;

#ifdef	DEBUG
	sprintf(trace_str,"Entering check_ad_oe\n");
	TRACE(trace_str);
#endif
        c = Toupper(read_buf[0]);
        clear_err_line();
	if(*field_entered)	{

		if (c == ADDRESS_ODD || c == ADDRESS_EVEN || c == ADDRESS_BOTH) {
			oe_flag = 1;
			ad_ptr->odd_even = c;
		switch(piu)	{
			case F_AD_OE_1:
					mvprintw(D_ADD_ROW_1,D_AD_OE_COL,"%c",ad_ptr->odd_even);
					ad_flags_1 = 1;
					break;
			case F_AD_OE_2:
					mvprintw(D_ADD_ROW_2,D_AD_OE_COL,"%c",ad_ptr->odd_even);
					ad_flags_2 = 1;
					break;
			case F_AD_OE_3:
					mvprintw(D_ADD_ROW_3,D_AD_OE_COL,"%c",ad_ptr->odd_even);
					ad_flags_3 = 1;
					break;
			case F_AD_OE_4:
					mvprintw(D_ADD_ROW_4,D_AD_OE_COL,"%c",ad_ptr->odd_even);
					ad_flags_4 = 1;
					break;
			case F_AD_OE_5:
					mvprintw(D_ADD_ROW_5,D_AD_OE_COL,"%c",ad_ptr->odd_even);
					ad_flags_5 = 1;
					break;
			case F_AD_OE_6:
					mvprintw(D_ADD_ROW_6,D_AD_OE_COL,"%c",ad_ptr->odd_even);
					ad_flags_6 = 1;
					break;
			case F_AD_OE_7:
					mvprintw(D_ADD_ROW_7,D_AD_OE_COL,"%c",ad_ptr->odd_even);
					ad_flags_7 = 1;
					break;
			case F_AD_OE_8:
					mvprintw(D_ADD_ROW_8,D_AD_OE_COL,"%c",ad_ptr->odd_even);
					ad_flags_8 = 1;
					break;
			case F_AD_OE_9:
					mvprintw(D_ADD_ROW_9,D_AD_OE_COL,"%c",ad_ptr->odd_even);
					ad_flags_9 = 1;
					break;
		}
			refresh();
			update_piu();
			ad_oe = 1;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_ad_oel - 1\n");
	TRACE(trace_str);
#endif
		}
		else	{
			clear_err_line();		
			prt_error(ERROR_N_FLD);
			oe_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_ad_oe - 2\n");
	TRACE(trace_str);
#endif
			return(-1);
		}
	}
	else	{	
		update_piu();
		if ((!oe_flag) && (key_val != KEY_UP))	{
			piu = F_AD_OE_1;
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_15, "Error in field"));
			return(-1);
		}
	}
}



int check_lo_nbr (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_lo_nbr\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			if (!numeric(read_buf))	{
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_23, "Invalid low number"));
				low_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_lo_nbr- 1\n");
	TRACE(trace_str);
#endif
				return(-1);
			}
			low_number = atoi(read_buf);
			if (!doing_add)	{
				if (ad_high)	{
					if (low_number >  high_number)	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_23, "Invalid low number"));
						low_flag = 0;
						return(-1);
					}
				}
				else	{
					if (low_number > temp_ad_ptr-> high_nbr)	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_23, "Invalid low number"));
						low_flag = 0;
						return(-1);
					}
				}
			}
/*
for (i = 0; i < 1024; ++i)	{
	if ((high[i] == temp_ad_ptr->high_nbr) &&(low[i] == temp_ad_ptr->low_nbr))	
		continue;
		if ((low[i] <= low_number) && ( low_number <= high[i]))	{
			db(ADDRESS_FILE_ID,READ_NUM,&ad_key1,ISEQUAL,adr[i]);
	if (doing_add)	{
	
if ((strcmp(temp_ad_ptr->str_pre_dir,ad_ptr->str_pre_dir) == 0) && (strcmp(temp_ad_ptr->str_post_dir,ad_ptr->str_post_dir) == 0) && (strcmp(temp_ad_ptr->str_city,ad_ptr->str_city) == 0) && (temp_ad_ptr->odd_even == ad_ptr->odd_even))	{
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_24, "Range overlapping"));
					low_flag = 0;
					return(-1);
}	
	} 
	else	{
			
		
	}
		} 	
}
*/

			low_flag = 1;
			clear_err_line();
			ad_ptr->low_nbr = low_number;	
		switch(piu)	{
			case F_LO_NBR_1:
					mvprintw(D_ADD_ROW_1,D_LO_NBR_COL,"%6d",ad_ptr->low_nbr);
					ad_flags_1 = 1;
					break;
			case F_LO_NBR_2:
					mvprintw(D_ADD_ROW_2,D_LO_NBR_COL,"%6d",ad_ptr->low_nbr);
					ad_flags_2 = 1;
					break;
			case F_LO_NBR_3:
					mvprintw(D_ADD_ROW_3,D_LO_NBR_COL,"%6d",ad_ptr->low_nbr);
					ad_flags_3 = 1;
					break;
			case F_LO_NBR_4:
					mvprintw(D_ADD_ROW_4,D_LO_NBR_COL,"%6d",ad_ptr->low_nbr);
					ad_flags_4 = 1;
					break;
			case F_LO_NBR_5:
					mvprintw(D_ADD_ROW_5,D_LO_NBR_COL,"%6d",ad_ptr->low_nbr);
					ad_flags_5 = 1;
					break;
			case F_LO_NBR_6:
					mvprintw(D_ADD_ROW_6,D_LO_NBR_COL,"%6d",ad_ptr->low_nbr);
					ad_flags_6 = 1;
					break;
			case F_LO_NBR_7:
					mvprintw(D_ADD_ROW_7,D_LO_NBR_COL,"%6d",ad_ptr->low_nbr);
					ad_flags_7 = 1;
					break;
			case F_LO_NBR_8:
					mvprintw(D_ADD_ROW_8,D_LO_NBR_COL,"%6d",ad_ptr->low_nbr);
					ad_flags_8 = 1;
					break;
			case F_LO_NBR_9:
					mvprintw(D_ADD_ROW_9,D_LO_NBR_COL,"%6d",ad_ptr->low_nbr);
					ad_flags_9 = 1;
					break;
		}
			refresh();
			update_piu();
			ad_low = 1;
	}
	else	{
		update_piu();
		if ((!low_flag) && (key_val != KEY_UP))	{
			piu = F_LO_NBR_1;
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_23, "Invalid low number"));
			return(-1);
		}
	}
		
}


int check_hi_nbr (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_hi_nbr\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			if (!numeric(read_buf))	{
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_25, "Invalid number"));
				high_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_hi_nbr- 1\n");
	TRACE(trace_str);
#endif
				return(-1);
			}
			high_number = atoi(read_buf);
		if (doing_add)	{
			if (high_number < low_number)	{
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_26, "Invalid high number"));
				high_flag = 0;
				return(-1);
			}
		}
		else	{
			if (ad_low)	{
				if (high_number < low_number)	{
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_26, "Invalid high number"));
					high_flag = 0;
					return(-1);
				}
			}
			else	{
				if (high_number < temp_ad_ptr->low_nbr)	{
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_26, "Invalid high number"));
					high_flag = 0;
					return(-1);
				}
				
			}
		}
		/*


	
			for (i = 0; i< 1024; ++i)	{
				if ((high[i] == temp_ad_ptr->high_nbr) &&(low[i] == temp_ad_ptr->low_nbr))	{
					++i;
				}
if ((low[i] <= high_number) && ( high_number <= high[i]))	{
if ((strcmp(temp_ad_ptr->str_pre_dir,ad_ptr->str_pre_dir) == 0) && (strcmp(temp_ad_ptr->str_post_dir,ad_ptr->str_post_dir) == 0) && (strcmp(temp_ad_ptr->str_city,ad_ptr->str_city) == 0) && (temp_ad_ptr->odd_even == ad_ptr->odd_even))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_24, "Range overlapping"));
						high_flag = 0;
						return(-1);
				}
			}
		}	


*/


			high_flag = 1;
			clear_err_line();
			ad_ptr->high_nbr = high_number;
			temp_flag1 = 1;
		switch(piu)	{
			case F_HI_NBR_1:
					mvprintw(D_ADD_ROW_1,D_HI_NBR_COL,"%6d",ad_ptr->high_nbr);
					ad_flags_1 = 1;
					break;
			case F_HI_NBR_2:
					mvprintw(D_ADD_ROW_2,D_HI_NBR_COL,"%6d",ad_ptr->high_nbr);
					ad_flags_2 = 1;
					break;
			case F_HI_NBR_3:
					mvprintw(D_ADD_ROW_3,D_HI_NBR_COL,"%6d",ad_ptr->high_nbr);
					ad_flags_3 = 1;
					break;
			case F_HI_NBR_4:
					mvprintw(D_ADD_ROW_4,D_HI_NBR_COL,"%6d",ad_ptr->high_nbr);
					ad_flags_4 = 1;
					break;
			case F_HI_NBR_5:
					mvprintw(D_ADD_ROW_5,D_HI_NBR_COL,"%6d",ad_ptr->high_nbr);
					ad_flags_5 = 1;
					break;
			case F_HI_NBR_6:
					mvprintw(D_ADD_ROW_6,D_HI_NBR_COL,"%6d",ad_ptr->high_nbr);
					ad_flags_6 = 1;
					break;
			case F_HI_NBR_7:
					mvprintw(D_ADD_ROW_7,D_HI_NBR_COL,"%6d",ad_ptr->high_nbr);
					ad_flags_7 = 1;
					break;
			case F_HI_NBR_8:
					mvprintw(D_ADD_ROW_8,D_HI_NBR_COL,"%6d",ad_ptr->high_nbr);
					ad_flags_8 = 1;
					break;
			case F_HI_NBR_9:
					mvprintw(D_ADD_ROW_9,D_HI_NBR_COL,"%6d",ad_ptr->high_nbr);
					ad_flags_9 = 1;
					break;
		}
			refresh();
			update_piu();
			ad_high = 1;
	}
	else	{
		update_piu();
		if (low_number < high_number)
			high_flag = 1;
		if ((!high_flag) && (key_val != KEY_UP))	{
			piu = F_HI_NBR_1;
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_26, "Invalid high number"));
			return(-1);
		}
	}
		
}

int check_ad_zip (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_ad_zip\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			if (!numeric(read_buf))	{
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_18, "Invalid zip code"));
				zip_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_ad_zip- 1\n");
	TRACE(trace_str);
#endif
				return(-1);
			}
			zip_flag = 1;
			clear_err_line();
			ad_ptr->str_zip = atoi(read_buf);	
		switch(piu)	{
			case F_AD_ZIP_1:
					mvprintw(D_ADD_ROW_1,D_AD_ZIP_COL,"%5d",ad_ptr->str_zip);
					ad_flags_1 = 1;
					break;
			case F_AD_ZIP_2:
					mvprintw(D_ADD_ROW_2,D_AD_ZIP_COL,"%5d",ad_ptr->str_zip);
					ad_flags_2 = 1;
					break;
			case F_AD_ZIP_3:
					mvprintw(D_ADD_ROW_3,D_AD_ZIP_COL,"%5d",ad_ptr->str_zip);
					ad_flags_3 = 1;
					break;
			case F_AD_ZIP_4:
					mvprintw(D_ADD_ROW_4,D_AD_ZIP_COL,"%5d",ad_ptr->str_zip);
					ad_flags_4 = 1;
					break;
			case F_AD_ZIP_5:
					mvprintw(D_ADD_ROW_5,D_AD_ZIP_COL,"%5d",ad_ptr->str_zip);
					ad_flags_5 = 1;
					break;
			case F_AD_ZIP_6:
					mvprintw(D_ADD_ROW_6,D_AD_ZIP_COL,"%5d",ad_ptr->str_zip);
					ad_flags_6 = 1;
					break;
			case F_AD_ZIP_7:
					mvprintw(D_ADD_ROW_7,D_AD_ZIP_COL,"%5d",ad_ptr->str_zip);
					ad_flags_7 = 1;
					break;
			case F_AD_ZIP_8:
					mvprintw(D_ADD_ROW_8,D_AD_ZIP_COL,"%5d",ad_ptr->str_zip);
					ad_flags_8 = 1;
					break;
			case F_AD_ZIP_9:
					mvprintw(D_ADD_ROW_9,D_AD_ZIP_COL,"%5d",ad_ptr->str_zip);
					ad_flags_9 = 1;
					break;
		}
			refresh();
			update_piu();
			ad_zip = 1;
	}
	else	{
		update_piu();
		if ((!zip_flag) && (key_val != KEY_UP))	{
			--piu;
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_18, "Invalid zip code"));
			return(-1);
		}
	}
		
}

int check_ad_sbz (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_ad_sbz\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			ipc_required = 1;
			str_to_upper(read_buf);
			strcpy(temp_sz_ptr->nbr,read_buf);
			if ((db(SUBZONE_FILE_ID,READ_KEYS,&sz_key1,ISEQUAL,0)) < 0)	{
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_19, "Invalid subzone number"));
				subzone_flag = 0;
				return(-1);
			}
			strcpy(ad_ptr->str_subzone,read_buf);
			subzone_flag = 1;
		switch(piu)	{
			case F_AD_SBZ_1:
					mvprintw(D_ADD_ROW_1,D_AD_SBZ_COL,"%s",ad_ptr->str_subzone);
					ad_flags_1 = 1;
					break;
			case F_AD_SBZ_2:
					mvprintw(D_ADD_ROW_2,D_AD_SBZ_COL,"%s",ad_ptr->str_subzone);
					ad_flags_2 = 1;
					break;
			case F_AD_SBZ_3:
					mvprintw(D_ADD_ROW_3,D_AD_SBZ_COL,"%s",ad_ptr->str_subzone);
					ad_flags_3 = 1;
					break;
			case F_AD_SBZ_4:
					mvprintw(D_ADD_ROW_4,D_AD_SBZ_COL,"%s",ad_ptr->str_subzone);
					ad_flags_4 = 1;
					break;
			case F_AD_SBZ_5:
					mvprintw(D_ADD_ROW_5,D_AD_SBZ_COL,"%s",ad_ptr->str_subzone);
					ad_flags_5 = 1;
					break;
			case F_AD_SBZ_6:
					mvprintw(D_ADD_ROW_6,D_AD_SBZ_COL,"%s",ad_ptr->str_subzone);
					ad_flags_6 = 1;
					break;
			case F_AD_SBZ_7:
					mvprintw(D_ADD_ROW_7,D_AD_SBZ_COL,"%s",ad_ptr->str_subzone);
					ad_flags_7 = 1;
					break;
			case F_AD_SBZ_8:
					mvprintw(D_ADD_ROW_8,D_AD_SBZ_COL,"%s",ad_ptr->str_subzone);
					ad_flags_8 = 1;
					break;
			case F_AD_SBZ_9:
					mvprintw(D_ADD_ROW_9,D_AD_SBZ_COL,"%s",ad_ptr->str_subzone);
					ad_flags_9 = 1;
					break;
		}
			refresh();
			update_piu();
			ad_subzone = 1;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_ad_sbz - 2\n");
	TRACE(trace_str);
#endif
	}

	else	{
		update_piu();
		if ((!subzone_flag) && (key_val != KEY_UP))	{
			piu = F_AD_SBZ_1;
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_19, "Invalid subzone number"));
			return(-1);
		}
	}
}

int check_ad_cmnt (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_ad_cmnt\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			ipc_required = 1;
			strcpy(ad_ptr->str_comment,str_to_upper(read_buf));
		switch(piu)	{
			case F_AD_CMNT_1:
					mvprintw(D_ADD_ROW_1,D_AD_CMNT_COL,"%s",ad_ptr->str_comment);
					ad_flags_1 = 1;
					break;
			case F_AD_CMNT_2:
					mvprintw(D_ADD_ROW_2,D_AD_CMNT_COL,"%s",ad_ptr->str_comment);
					ad_flags_2 = 1;
					break;
			case F_AD_CMNT_3:
					mvprintw(D_ADD_ROW_3,D_AD_CMNT_COL,"%s",ad_ptr->str_comment);
					ad_flags_3 = 1;
					break;
			case F_AD_CMNT_4:
					mvprintw(D_ADD_ROW_4,D_AD_CMNT_COL,"%s",ad_ptr->str_comment);
					ad_flags_4 = 1;
					break;
			case F_AD_CMNT_5:
					mvprintw(D_ADD_ROW_5,D_AD_CMNT_COL,"%s",ad_ptr->str_comment);
					ad_flags_5 = 1;
					break;
			case F_AD_CMNT_6:
					mvprintw(D_ADD_ROW_6,D_AD_CMNT_COL,"%s",ad_ptr->str_comment);
					ad_flags_6 = 1;
					break;
			case F_AD_CMNT_7:
					mvprintw(D_ADD_ROW_7,D_AD_CMNT_COL,"%s",ad_ptr->str_comment);
					ad_flags_7 = 1;
					break;
			case F_AD_CMNT_8:
					mvprintw(D_ADD_ROW_8,D_AD_CMNT_COL,"%s",ad_ptr->str_comment);
					ad_flags_8 = 1;
					break;
			case F_AD_CMNT_9:
					mvprintw(D_ADD_ROW_9,D_AD_CMNT_COL,"%s",ad_ptr->str_comment);
					ad_flags_9 = 1;
					break;
		}
			refresh();
			update_piu();
			ad_comment = 1;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_ad_cmnt - 2\n");
	TRACE(trace_str);
#endif
	}

	else	
		update_piu();
}

int check_ad_x_cd (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_ad_x_cd\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			if (!numeric(read_buf))	{
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_20, "Invalid x coordinate"));
				x_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_ad_x_cd- 1\n");
	TRACE(trace_str);
#endif
				return(-1);
			}
			x_flag = 1;
			clear_err_line();
			ad_ptr->str_x_coord = atoi(read_buf);	
		switch(piu)	{
			case F_AD_X_CD_1:
					mvprintw(D_ADD_ROW_1,D_AD_X_CD_COL,"%8d",ad_ptr->str_x_coord);
					ad_flags_1 = 1;
					break;
			case F_AD_X_CD_2:
					mvprintw(D_ADD_ROW_2,D_AD_X_CD_COL,"%8d",ad_ptr->str_x_coord);
					ad_flags_2 = 1;
					break;
			case F_AD_X_CD_3:
					mvprintw(D_ADD_ROW_3,D_AD_X_CD_COL,"%8d",ad_ptr->str_x_coord);
					ad_flags_3 = 1;
					break;
			case F_AD_X_CD_4:
					mvprintw(D_ADD_ROW_4,D_AD_X_CD_COL,"%8d",ad_ptr->str_x_coord);
					ad_flags_4 = 1;
					break;
			case F_AD_X_CD_5:
					mvprintw(D_ADD_ROW_5,D_AD_X_CD_COL,"%8d",ad_ptr->str_x_coord);
					ad_flags_5 = 1;
					break;
			case F_AD_X_CD_6:
					mvprintw(D_ADD_ROW_6,D_AD_X_CD_COL,"%8d",ad_ptr->str_x_coord);
					ad_flags_6 = 1;
					break;
			case F_AD_X_CD_7:
					mvprintw(D_ADD_ROW_7,D_AD_X_CD_COL,"%8d",ad_ptr->str_x_coord);
					ad_flags_7 = 1;
					break;
			case F_AD_X_CD_8:
					mvprintw(D_ADD_ROW_8,D_AD_X_CD_COL,"%8d",ad_ptr->str_x_coord);
					ad_flags_8 = 1;
					break;
			case F_AD_X_CD_9:
					mvprintw(D_ADD_ROW_9,D_AD_X_CD_COL,"%8d",ad_ptr->str_x_coord);
					ad_flags_9 = 1;
					break;
		}
			refresh();
			update_piu();
			ad_x = 1;
	}
	else	{
		update_piu();
		if ((!x_flag) && (key_val != KEY_UP))	{
			--piu;
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_20, "Invalid x coordinate"));
			return(-1);
		}
	}
		

}

int check_ad_y_cd (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_ad_y_cd\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			if (!numeric(read_buf))	{
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_21, "Invalid y coordinate"));
				y_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_ad_y_cd- 1\n");
	TRACE(trace_str);
#endif
				return(-1);
			}
			y_flag = 1;
			clear_err_line();
			ad_ptr->str_y_coord = atoi(read_buf);	
		switch(piu)	{
			case F_AD_Y_CD_1:
					mvprintw(D_ADD_ROW_1,D_AD_Y_CD_COL,"%8d",ad_ptr->str_y_coord);
					ad_flags_1 = 1;
					break;
			case F_AD_Y_CD_2:
					mvprintw(D_ADD_ROW_2,D_AD_Y_CD_COL,"%8d",ad_ptr->str_y_coord);
					ad_flags_2 = 1;
					break;
			case F_AD_Y_CD_3:
					mvprintw(D_ADD_ROW_3,D_AD_Y_CD_COL,"%8d",ad_ptr->str_y_coord);
					ad_flags_3 = 1;
					break;
			case F_AD_Y_CD_4:
					mvprintw(D_ADD_ROW_4,D_AD_Y_CD_COL,"%8d",ad_ptr->str_y_coord);
					ad_flags_4 = 1;
					break;
			case F_AD_Y_CD_5:
					mvprintw(D_ADD_ROW_5,D_AD_Y_CD_COL,"%8d",ad_ptr->str_y_coord);
					ad_flags_5 = 1;
					break;
			case F_AD_Y_CD_6:
					mvprintw(D_ADD_ROW_6,D_AD_Y_CD_COL,"%8d",ad_ptr->str_y_coord);
					ad_flags_6 = 1;
					break;
			case F_AD_Y_CD_7:
					mvprintw(D_ADD_ROW_7,D_AD_Y_CD_COL,"%8d",ad_ptr->str_y_coord);
					ad_flags_7 = 1;
					break;
			case F_AD_Y_CD_8:
					mvprintw(D_ADD_ROW_8,D_AD_Y_CD_COL,"%8d",ad_ptr->str_y_coord);
					ad_flags_8 = 1;
					break;
			case F_AD_Y_CD_9:
					mvprintw(D_ADD_ROW_9,D_AD_Y_CD_COL,"%8d",ad_ptr->str_y_coord);
					ad_flags_9 = 1;
					break;
		}
			refresh();
			update_piu();
			ad_y = 1;
	}
	else	{
		update_piu();
		if ((!y_flag) && (key_val != KEY_UP))	{
			--piu;
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_21, "Invalid y coordinate"));
			return(-1);
		}
	}
}


int check_ad_map (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
int num;
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_ad_map\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			if (!numeric(read_buf))	{
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_27, "Invalid page reference number"));
				map_flag = 0;
#ifdef	DEBUG
	sprintf(trace_str,"Leaving check_ad_map- 1\n");
	TRACE(trace_str);
#endif
				return(-1);
			}
			map_flag = 1;
			clear_err_line();
			num = atoi(read_buf);
			ad_ptr->map_ref_page = (short)num;	
		switch(piu)	{
			case F_AD_MAP_1:
					mvprintw(D_ADD_ROW_1,D_AD_MAP_COL,"%3d",ad_ptr->map_ref_page);
					ad_flags_1 = 1;
					break;
			case F_AD_MAP_2:
					mvprintw(D_ADD_ROW_2,D_AD_MAP_COL,"%3d",ad_ptr->map_ref_page);
					ad_flags_2 = 1;
					break;
			case F_AD_MAP_3:
					mvprintw(D_ADD_ROW_3,D_AD_MAP_COL,"%3d",ad_ptr->map_ref_page);
					ad_flags_3 = 1;
					break;
			case F_AD_MAP_4:
					mvprintw(D_ADD_ROW_4,D_AD_MAP_COL,"%3d",ad_ptr->map_ref_page);
					ad_flags_4 = 1;
					break;
			case F_AD_MAP_5:
					mvprintw(D_ADD_ROW_5,D_AD_MAP_COL,"%3d",ad_ptr->map_ref_page);
					ad_flags_5 = 1;
					break;
			case F_AD_MAP_6:
					mvprintw(D_ADD_ROW_6,D_AD_MAP_COL,"%3d",ad_ptr->map_ref_page);
					ad_flags_6 = 1;
					break;
			case F_AD_MAP_7:
					mvprintw(D_ADD_ROW_7,D_AD_MAP_COL,"%3d",ad_ptr->map_ref_page);
					ad_flags_7 = 1;
					break;
			case F_AD_MAP_8:
					mvprintw(D_ADD_ROW_8,D_AD_MAP_COL,"%3d",ad_ptr->map_ref_page);
					ad_flags_8 = 1;
					break;
			case F_AD_MAP_9:
					mvprintw(D_ADD_ROW_9,D_AD_MAP_COL,"%3d",ad_ptr->map_ref_page);
					ad_flags_9 = 1;
					break;
		}
			refresh();
			update_piu();
			ad_map = 1;
	}
	else	{
		update_piu();
		if ((!map_flag) && (key_val != KEY_UP))	{
			--piu;
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_22, "Invalid map reference page number"));
			return(-1);
		}
	}
}




int check_ad_pb (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
int num;
#ifdef	DEBUG
	sprintf(trace_str,"Entering check_ad_pb\n");
	TRACE(trace_str);
#endif
	if(*field_entered)	{
			clear_err_line();
			ipc_required = 1;
			strcpy(ad_ptr->map_ref_block,str_to_upper(read_buf));
		switch(piu)	{
			case F_AD_PB_1:
					mvprintw(D_ADD_ROW_1,D_AD_PB_COL,"%s",ad_ptr->map_ref_block);
					refresh();
					ad_flags_1 = 1;
					update_piu();
					if ((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r'))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PB_1;
					}
					break;
			case F_AD_PB_2:
					mvprintw(D_ADD_ROW_2,D_AD_PB_COL,"%s",ad_ptr->map_ref_block);
					refresh();
					ad_flags_2 = 1;
					update_piu();
					if ((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r'))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PB_2;
					}
					break;
			case F_AD_PB_3:
					mvprintw(D_ADD_ROW_3,D_AD_PB_COL,"%s",ad_ptr->map_ref_block);
					refresh();
					ad_flags_3 = 1;
					update_piu();
					if ((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r'))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PB_3;
					}
					break;
			case F_AD_PB_4:
					mvprintw(D_ADD_ROW_4,D_AD_PB_COL,"%s",ad_ptr->map_ref_block);
					refresh();
					ad_flags_4 = 1;
					update_piu();
					if ((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r'))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PB_4;
					}
					break;
			case F_AD_PB_5:
					mvprintw(D_ADD_ROW_5,D_AD_PB_COL,"%s",ad_ptr->map_ref_block);
					refresh();
					ad_flags_5 = 1;
					update_piu();
					if ((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r'))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PB_5;
					}
					break;
			case F_AD_PB_6:
					mvprintw(D_ADD_ROW_6,D_AD_PB_COL,"%s",ad_ptr->map_ref_block);
					refresh();
					ad_flags_6 = 1;
					update_piu();
					if ((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r'))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PB_6;
					}
					break;
			case F_AD_PB_7:
					mvprintw(D_ADD_ROW_7,D_AD_PB_COL,"%s",ad_ptr->map_ref_block);
					refresh();
					ad_flags_7 = 1;
					update_piu();
					if ((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r'))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PB_7;
					}
					break;
			case F_AD_PB_8:
					mvprintw(D_ADD_ROW_8,D_AD_PB_COL,"%s",ad_ptr->map_ref_block);
					refresh();
					ad_flags_8 = 1;
					update_piu();
					if ((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r'))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PB_8;
					}
					break;
			case F_AD_PB_9:
					mvprintw(D_ADD_ROW_9,D_AD_PB_COL,"%s",ad_ptr->map_ref_block);
					refresh();
					ad_flags_9 = 1;
					update_piu();
					if ((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r'))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = NUM_FLDS;
					}
					break;
		}
				ad_pb = 1;
	}
	else	{
		switch(piu)	{
			case F_AD_PB_1:
				update_piu();
				if (((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r') || (key_val == TAB_KEY)) && (ad_flags_1))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PB_1;
				}
					break;
			case F_AD_PB_2:
				update_piu();
				if (((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r') || (key_val == TAB_KEY)) && (ad_flags_2))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PB_2;
				}
					break;
			case F_AD_PB_3:
				update_piu();
				if (((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r') || (key_val == TAB_KEY)) && (ad_flags_3))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PB_3;
				}
					break;
			case F_AD_PB_4:
				update_piu();
				if (((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r') || (key_val == TAB_KEY)) && (ad_flags_4))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PB_4;
				}
					break;
			case F_AD_PB_5:
				update_piu();
				if (((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r') || (key_val == TAB_KEY)) && (ad_flags_5))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PB_5;
				}
					break;
			case F_AD_PB_6:
				update_piu();
				if (((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r') || (key_val == TAB_KEY)) && (ad_flags_6))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PB_6;
				}
					break;
			case F_AD_PB_7:
				update_piu();
				if (((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r') || (key_val == TAB_KEY)) && (ad_flags_7))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PB_7;
				}
					break;
			case F_AD_PB_8:
				update_piu();
				if (((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r') || (key_val == TAB_KEY)) && (ad_flags_8))	{
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_2, "Enter ESC to execute"));
						piu = F_AD_PB_8;
				}
					break;
			case F_AD_PB_9:
				update_piu();
				if ((key_val == KEY_DOWN) || (key_val == '\n') || (key_val == '\r') || (key_val == TAB_KEY)) 	{
						piu = NUM_FLDS;
				}
					break;
		}
	}
}


/**************************************************************
* update_piu
**************************************************************/
update_piu()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering update_piu\n");
	TRACE(trace_str);
#endif
	if(key_val == '\t' ||
	key_val == '\n' ||
	key_val == '\r' ||
	key_val == KEY_DOWN ||
	key_val == OK )
		++piu;
	if(key_val == KEY_UP)
		--piu;
	return;
}


/**************************************************************
*numeric 
**************************************************************/
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


/*--------------------------------------------------------
 *==>   get subzone
 *--------------------------------------------------------
 */

get_subzone()
{
	struct cisam_ad	*address;
	short			 number_odd,
					 in_address_range = FALSE,
					 odd_even_consistent = FALSE;


/*
 *	initialize placename 
 */
	pl_ptr->index = temp_index;

	if ((pl_ptr->str_pre_dir[0] != 'W') && 
		(pl_ptr->str_pre_dir[0] != 'N') && 
		(pl_ptr->str_pre_dir[0] != 'E') && 
		(pl_ptr->str_pre_dir[0] != 'S'))
	  {
		pl_ptr->str_pre_dir[0] = '\0';
		pl_ptr->str_pre_dir[1] = '\0';
	  }


	if ((pl_ptr->str_post_dir[0] != 'W') && 
		(pl_ptr->str_post_dir[0] != 'N') && 
		(pl_ptr->str_post_dir[0] != 'E') && 
		(pl_ptr->str_post_dir[0] != 'S'))
	  {
		pl_ptr->str_post_dir[0] = '\0';
		pl_ptr->str_post_dir[1] = '\0';
	  }

	number_odd = (pl_ptr->street_nbr % 2);

/*
 *	initialize address 
 */
	address = (struct cisam_ad *)req_buf;
	memset(address, '\0', sizeof(struct cisam_ad));
	address->str_index = temp_index; 
	strcpy(address->str_pre_dir, pl_ptr->str_pre_dir); 
	strcpy(address->str_post_dir, pl_ptr->str_post_dir); 
	strcpy(address->str_city, pl_ptr->str_city); 
	address->high_nbr = 0;

/*
 *	get first address 
 */
	if (db(ADDRESS_FILE_ID,READ_KEYS,&ad_key1,ISGTEQ,0) < 0)
	  {
		prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, 
				GEO_MAINT_28, "No match subzone"));
		subzone_flag = 0;
		return(-1);
	  }

/*
 *	find valid address range for placename
 */
	while(1)
	  {
		if (address->str_index != temp_index)	
			break;

		if (!strcmp(address->str_city, pl_ptr->str_city) &&
			!strcmp(address->str_pre_dir, pl_ptr->str_pre_dir) &&
			!strcmp(address->str_post_dir, pl_ptr->str_post_dir))
		  {
			if (pl_ptr->street_nbr <= address->high_nbr && 
				pl_ptr->street_nbr >= address->low_nbr)
				in_address_range = TRUE;

			if ((number_odd && address->odd_even != ADDRESS_EVEN) ||
				(!number_odd && address->odd_even != ADDRESS_ODD))
				odd_even_consistent = TRUE;

			if (in_address_range && odd_even_consistent)
			  {
				if (pl_ptr->str_zip == 0)
					pl_ptr->str_zip = temp_ad_ptr->str_zip;
				strcpy(pl_ptr->str_subzone,temp_ad_ptr->str_subzone);
				if (pl_ptr->str_x_coord == 0)
					pl_ptr->str_x_coord = temp_ad_ptr->str_x_coord;
				if (pl_ptr->str_y_coord == 0)
					pl_ptr->str_y_coord = temp_ad_ptr->str_y_coord;
				if (pl_ptr->map_ref_page == 0)
					pl_ptr->map_ref_page = temp_ad_ptr->map_ref_page;
				strcpy(pl_ptr->map_ref_block, temp_ad_ptr->map_ref_block);
				address_record_nbr = temp_ad_ptr->record_nbr;
				if ((pl_ptr->local != 'N')&&(pl_ptr->local != 'Y'))
					pl_ptr->local = 'N';
				subzone_flag = 1;
				return(0);
			  }
		  }
		if (db(ADDRESS_FILE_ID,READ_KEYS,&ad_key1,ISNEXT, 0) < 0)
			break;

		in_address_range = odd_even_consistent = FALSE;
	  }

	prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, 
				GEO_MAINT_28, "No match subzone"));
	subzone_flag = 0;

	return(-1);
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



/*-----------------------------------------------------------------
 *	string null
 *-----------------------------------------------------------------
 */

string_null(string, size)
	char		 string[];
	int			 size;
{
	int			 i;	
	short		 string_is_null = TRUE;


	for (i = 0; i < size; i++)
	  {
		if (string[i] == '\0')
			break;

		if (isprint(string[i]))
			string_is_null = FALSE;
	  }

	return(string_is_null);
}


