static char sz__FILE__[]="@(#)geo_sub3.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  geo_sub3.c; Rel: 5.13.0.0; Get date: 8/10/90 at 08:44:02
 * @(#)  Last delta: 8/9/90 at 13:08:31
 * @(#)  SCCS File: /taxi/sccs/s.geo_sub3.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <isam.h>
#include <cursesX.h>
#include <memory.h>
#include <signal.h>

#include "geo_m_strdefs.h"
#include "mad_error.h"
#include "mads_types.h"
#include "df_maint.h"
#include "mads_isam.h"
#include "ex_key.h"
#include "geo_scr.h"
#include "path.h"
#include "geo_msg.h"
#include "tokenize.h"
#include "language.h"

extern struct scr_flds geo_fld[];

extern int upper_case;
int  c,op;
int  pl_ter = 0,pl_ter1 = 0;
int  ad_ter = 0,ad_ter1 = 0;
extern int  ab_ctr,al_ctr,pl_ctr,ad_ctr;
extern int ret_val;		/* return value */
extern int piu;			/* prompt in use */
extern int intr_allowed;	/* interrupts allowed */
extern int key_val;		/* value returned from the check routines */
extern char read_buf[];		/* buffer for reading fields */
extern int field_entered;	/* field entered or not */
extern int saw_intr;		/* indicates an interrupt signal was entered */
extern int cur_list;		/* indicates a row is in the current list */
extern int (*check_field[])();	/* Array of pointers to routines to validate
				   the data input by the user */
extern int record_nbr;		/* C_ISAM record number */
extern char o_file[];		/* output file as specified by the user */
extern int prt_desc();
extern int search_qur();
extern int temp_index;
extern int key_desc();
extern int prt_data();
extern char mode[];		/* user specified Append or Create */	
extern char buf[];			
extern char req_buf[];			
extern  loop1;
extern  loop2;
extern  loop3;
extern  loop4;
extern struct cisam_sn *sn_ptr; /* pointer to street name file structure */
extern struct cisam_sn *temp_sn_ptr; 
extern struct cisam_sn sn_buf; 
extern struct cisam_pl *pl_ptr; /* pointer to place name file structure */
extern struct cisam_pl *temp_pl_ptr; 
extern struct cisam_pl pl_buf; 
extern struct cisam_ad *ad_ptr; /* pointer to address name file structure */
extern struct cisam_ad *temp_ad_ptr; 
extern struct cisam_ad ad_buf; 
extern struct cisam_lo *lo_ptr; /* pointer to locality file structure */
extern struct cisam_lo *temp_lo_ptr; 
extern struct cisam_lo lo_buf; 
extern struct cisam_sz *sz_ptr; /* pointer to subzone file structure */
extern struct cisam_sz *temp_sz_ptr; 
extern struct cisam_sz sz_buf; 
extern int clear_flds();	/* clear data fields on the screen */
extern int doing_add;		/* performing add operation */
extern int index_flag;	
extern int abb[],als[],plc[],adr[],high[],low[];
extern int ab_flags_1,ab_flags_2,al_flags_1,al_flags_2;
extern int pl_flags_1,pl_flags_2,pl_flags_3;
extern int pl_name,pl_pre,pl_post,pl_city;
extern int pl_oe,pl_street,pl_suf,pl_zip;
extern int pl_subzone,pl_comment,pl_x,pl_y;
extern int pl_map,pl_pb,pl_loc;
extern int ad_flags_1,ad_flags_2,ad_flags_3;
extern int ad_flags_4,ad_flags_5,ad_flags_6;
extern int ad_flags_7,ad_flags_8,ad_flags_9;
extern int ad_pre,ad_post,ad_city;
extern int ad_oe,ad_high,ad_low;
extern int ad_zip,ad_subzone,ad_comment;
extern int ad_x,ad_y,ad_map,ad_pb;
extern int name_flag;
extern int type_flag;
extern int  abb_count, als_count, plc_count, adr_count;
/******************************************************************************
* process_remove - remove the current zone file record.
*******************************************************************************/

process_remove()
{
int   i;
	if (!cur_list)	{
		prt_error(NO_CUR_LIST);
		return;
	}
#ifdef DEBUG
	sprintf(trace_str,"Entering process_remove\n");
	TRACE(trace_str);
#endif
while(1)	{
	mvaddstr(ERROR_ROW-1,1,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_87, " [REMOVE MODE]   (0) Main menu (1) Street name & type (2) Abbreviation (3) Aliases (4) Place name (5) Address : "));
	refresh();
	op = ch_option1();
	switch(op)	{
		case '0':
			clear_err_line();
			prt_hdr();
			move(ERROR_ROW-1,1);
			clrtoeol();
			refresh();
			clear_flds(1,NUM_FLDS);
			temp_sn_ptr->index = temp_index;
			temp_sn_ptr->rec_type = '1';
			if(db(STRNAME_FILE_ID,READ_KEYS,&sn_key1,ISGTEQ,0) < 0)	{
				prt_error(NO_CUR_LIST);
				return;
			}
			cur_list = 1;
			mvprintw(1,1,"%18d",temp_sn_ptr->index);
			refresh();
			temp_index = temp_sn_ptr->index;
			for ( i = 0; i< 1024; ++i)	{
				abb[i] = 0;
				als[i] = 0;
				plc[i] = 0;
				adr[i] = 0;
				high[i] = 0;
				low[i] = 0;
			}
			ab_ctr = 0;
			al_ctr = 0;
			pl_ctr = 0;
			ad_ctr = 0;
			loop1 = 0;
			loop2 = 0;
			loop3 = 0;
			loop4 = 0;
			search_qur();
			return;
		case '1':
			mvaddstr(0,70,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_88, " Enter ESC to remove the record in cursor position "));
			name_rm();
			break;
		case '2':
			mvaddstr(0,70,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_88, " Enter ESC to remove the record in cursor position "));
			abbrev_rm();
			break;
		case '3':
			mvaddstr(0,70,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_88, " Enter ESC to remove the record in cursor position "));
			alias_rm();
			break;
		case '4':
			mvaddstr(0,70,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_88, " Enter ESC to remove the record in cursor position "));
			place_rm();
			break;
		case '5':
			mvaddstr(0,70,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_88, " Enter ESC to remove the record in cursor position "));
			address_rm();
			break;
	}

} /* while */


}  
name_rm()
{
	int should_clear = 0;
	int i;

	piu = 2;
while(1)	{
	intr_allowed = 1;
	key_val = read_field(stdscr,geo_fld,piu,read_buf,&field_entered,&should_clear);
	attrset(0);
	if(saw_intr)  {
		saw_intr = 0;	
		clear_err_line();
		prt_hdr();
		return;
	}
	ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);
	if (key_val != ESC)	{
		prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_89, "Enter ESC to remove all records"));
		piu = 2;
		continue;
	}
	if (key_val == ESC)	{
		clear_err_line();
		clear_flds(1,NUM_FLDS);
		temp_sn_ptr->index = temp_index;
		if((db(STRNAME_FILE_ID,READ_KEYS,&sn_key1,ISGTEQ,0)) < 0) 	{
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_90, "Read error"));
			continue;
		}
		db(STRNAME_FILE_ID,DELETE,&sn_key1,0,0); 
		for (i = 0; i < abb_count; ++i)	{
		        temp_sn_ptr->record_nbr = abb[i];
			if (db(STRNAME_FILE_ID, READ_KEYS, &sn_key3, ISEQUAL, 0) < 0) 
				break;
			if (db(STRNAME_FILE_ID, DELETE, 0, 0, 0) < 0) 
				break;
		}
		for (i = 0; i < als_count; ++i)	{
		        temp_sn_ptr->record_nbr = als[i];
			if (db(STRNAME_FILE_ID, READ_KEYS, &sn_key3, ISEQUAL, 0) < 0) 
				break;
			if (db(STRNAME_FILE_ID, DELETE, 0, 0, 0) < 0) 
				break;
		}

		for (i = 0; i< plc_count; ++i)	{
		        temp_pl_ptr->record_nbr = plc[i];
			if (db(PLACENM_FILE_ID, READ_KEYS, &pl_key3, ISEQUAL, 0) < 0) 
				break;
			if (db(PLACENM_FILE_ID, DELETE, 0, 0, 0) < 0) 
				break;
		}
		for (i = 0; i< adr_count; ++i)	{
		        temp_ad_ptr->record_nbr = adr[i];
			if (db(ADDRESS_FILE_ID, READ_KEYS, &ad_key3, ISEQUAL, 0) < 0) 
				break;
			if (db(ADDRESS_FILE_ID, DELETE, 0, 0, 0) < 0) 
				break;
		}
			ab_ctr = 0;
			al_ctr = 0;
			pl_ctr = 0;
			ad_ctr = 0;
			loop1 = 0;
			loop2 = 0;
			loop3 = 0;
			loop4 = 0;
	pl_flags_1 = 0;
	pl_flags_2 = 0;
	pl_flags_3 = 0;
	ad_flags_1 = 0;
	ad_flags_2 = 0;
	ad_flags_3 = 0;
	ad_flags_4 = 0;
	ad_flags_5 = 0;
	ad_flags_6 = 0;
	ad_flags_7 = 0;
	ad_flags_8 = 0;
	ad_flags_9 = 0;
			ab_flags_1 = 0;
			ab_flags_2 = 0;
			al_flags_1 = 0;
			al_flags_2 = 0;
		break;
		}
	}
}
abbrev_rm()
{
	int should_clear = 0;

	piu = 4;
while(1)	{
	intr_allowed = 1;
	key_val = read_field(stdscr,geo_fld,piu,read_buf,&field_entered,&should_clear);
	attrset(0);
	if(saw_intr)  {
		saw_intr = 0;	
		clear_err_line();
		prt_hdr();
		return;
	}
	ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);
	if (key_val == ESC)	{
		clear_err_line();
		if (loop1 == 0)	{
			if (piu == 4)	{
			        temp_sn_ptr->record_nbr = abb[0];
				if (db(STRNAME_FILE_ID, READ_KEYS, &sn_key3, ISEQUAL, 0) < 0) {
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_90, "Read error"));
					piu = 4;
					continue;
				}
			}

			if (piu == 5)	{	
			        temp_sn_ptr->record_nbr = abb[1];
				if (db(STRNAME_FILE_ID, READ_KEYS, &sn_key3, ISEQUAL, 0) < 0) {
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_90, "Read error"));
					piu = 5;
					continue;
				}
			}
		}
		else	{	/* not first screen */
			if((ab_ctr%2) == 1)	{
			        temp_sn_ptr->record_nbr = abb[ab_ctr + 1];
				if (db(STRNAME_FILE_ID, READ_KEYS, &sn_key3, ISEQUAL, 0) < 0) {
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_90, "Read error"));
					continue;
				}
			}
			else	{
				if (piu == 4)	{
				        temp_sn_ptr->record_nbr = abb[ab_ctr];
					if (db(STRNAME_FILE_ID, READ_KEYS, &sn_key3, ISEQUAL, 0) < 0) {
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_90, "Read error"));
						continue;
					}
				}
				if (piu == 5)	{
				        temp_sn_ptr->record_nbr = abb[ab_ctr + 1];
					if (db(STRNAME_FILE_ID, READ_KEYS, &sn_key3, ISEQUAL, 0) < 0) {
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_90, "Read error"));
						continue;
					}
				}
			}
		} /* loop1 > 0 */
			if (piu == 4)
				clear_flds(4,4);
			else
				clear_flds(5,5);
			if((db(STRNAME_FILE_ID,DELETE,&sn_key1,0,0)) < 0) {
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_91, "Remove  error"));
				continue;
			}
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_92, "Record removed"));
			ab_flags_1 = 0;
			ab_flags_2 = 0;
			--loop1;
			break;
		} /* ESC */
	} /* while */
}
alias_rm()
{
	int should_clear = 0;

	piu = 6;
while(1)	{
	intr_allowed = 1;
	key_val = read_field(stdscr,geo_fld,piu,read_buf,&field_entered,&should_clear);
	attrset(0);
	if(saw_intr)  {
		saw_intr = 0;	
		clear_err_line();
		prt_hdr();
		return;
	}
	ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);
	if (key_val == ESC)	{
		clear_err_line();
		if (loop2 == 0)	{
			if (piu == 6)	{
			        temp_sn_ptr->record_nbr = als[0];
				if (db(STRNAME_FILE_ID, READ_KEYS, &sn_key3, ISEQUAL, 0) < 0) {
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_90, "Read error"));
					piu = 6;
					continue;
				}
			}

			if (piu == 7)	{	
			        temp_sn_ptr->record_nbr = als[1];
				if (db(STRNAME_FILE_ID, READ_KEYS, &sn_key3, ISEQUAL, 0) < 0) {
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_90, "Read error"));
					piu = 7;
					continue;
				}
			}
		}
		else	{	/* not first screen */
			if((al_ctr%2) == 1)	{
			        temp_sn_ptr->record_nbr = als[al_ctr + 1];
				if (db(STRNAME_FILE_ID, READ_KEYS, &sn_key3, ISEQUAL, 0) < 0) {
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_90, "Read error"));
					continue;
				}
			}
			else	{
				if (piu == 6)	{
				        temp_sn_ptr->record_nbr = als[al_ctr];
					if (db(STRNAME_FILE_ID, READ_KEYS, &sn_key3, ISEQUAL, 0) < 0) {
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_90, "Read error"));
						continue;
					}
				}
				if (piu == 7)	{
				        temp_sn_ptr->record_nbr = als[al_ctr + 1];
					if (db(STRNAME_FILE_ID, READ_KEYS, &sn_key3, ISEQUAL, 0) < 0) {
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_90, "Read error"));
						continue;
					}
				}
			}
		} /* loop2 > 0 */
			if (piu == 6)
				clear_flds(6,6);
			else
				clear_flds(7,7);
			if((db(STRNAME_FILE_ID,DELETE,&sn_key1,0,0)) < 0) {
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_91, "Remove  error"));
				continue;
			}
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_92, "Record removed"));
			al_flags_1 = 0;
			al_flags_2 = 0;
			--loop2;
			break;
		} /* ESC */
	} /* while */
}

place_rm()
{
	int should_clear = 0;

	piu = F_PL_NAME_1;
	pl_flags_1 = 0;
	pl_flags_2 = 0;
	pl_flags_3 = 0;
while(1)	{
	intr_allowed = 1;
	key_val = read_field(stdscr,geo_fld,piu,read_buf,&field_entered,&should_clear);
	attrset(0);
	if(saw_intr)  {
		saw_intr = 0;	
		clear_err_line();
		prt_hdr();
		return;
	}
	ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);
	if ((key_val == KEY_DOWN) ||(key_val == '\n') ||(key_val == '\t') ||(key_val == '\r'))	{
		if (piu == F_PL_CITY_1)		
			piu = F_PL_NAME_2;
		else if (piu == F_PL_CITY_2)
			piu = F_PL_NAME_3;
		else if (piu == F_PL_CITY_3)
			piu = F_PL_NAME_1;
	}
	if(key_val == ESC)	{
		clear_err_line();
		if (loop3 == 0)	{
			if ((F_PL_NAME_1 <= piu) && (piu <= F_LOC_1))	
				pl_ter = 0;
			else if ((F_PL_NAME_2 <= piu) && (piu <= F_LOC_2))	
				pl_ter = 1; 
			else if ((F_PL_NAME_3 <= piu) && (piu <= F_LOC_3))	
				pl_ter = 2;

			temp_pl_ptr->record_nbr = plc[pl_ter];
			if (db(PLACENM_FILE_ID, READ_KEYS, &pl_key3, ISEQUAL, 0) < 0) {
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_90, "Read error"));
					continue;
			}

		}
		else	{	/* not first screen */
			if((pl_ctr%3) == 1)	
				pl_ter1 = pl_ctr+2;
			if((pl_ctr%3) == 2)	{	
				if ((F_PL_NAME_1 <= piu) && (piu <= F_LOC_1))	
					pl_ter1 = pl_ctr+1;
				else if ((F_PL_NAME_2 <= piu) && (piu <= F_LOC_2))	
					pl_ter1 = pl_ctr+2;
			}
			if((pl_ctr%3) == 0)	{
				if ((F_PL_NAME_1 <= piu) && (piu <= F_LOC_1))	
					pl_ter1 = pl_ctr;
				else if ((F_PL_NAME_2 <= piu) && (piu <= F_LOC_2))	
					pl_ter1 = pl_ctr+1;
				else if ((F_PL_NAME_3 <= piu) && (piu <= F_LOC_3))	
					pl_ter1 = pl_ctr+2;
			}

			temp_pl_ptr->record_nbr = plc[pl_ter1];
			if (db(PLACENM_FILE_ID, READ_KEYS, &pl_key3, ISEQUAL, 0) < 0) {
			    prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_90, "Read error"));
			    continue;
			}
		} /* loop3 > 0 */
			if((db(PLACENM_FILE_ID,DELETE,&pl_key1,0,0)) < 0) {
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_93, "Removed  error"));
				continue;
			}
			if ((F_PL_NAME_1 <= piu) && (piu <= F_LOC_1))	
				clear_flds(F_PL_NAME_1,F_LOC_1);
	                else if ((F_PL_NAME_2 <= piu) && (piu <= F_LOC_2))	
				clear_flds(F_PL_NAME_2,F_LOC_2);
			else if ((F_PL_NAME_3 <= piu) && (piu <= F_LOC_3))	
				clear_flds(F_PL_NAME_3,F_LOC_3);
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_92, "Record removed"));
			--loop3;
			temp_sn_ptr->index = temp_pl_ptr->index;
			break;
		} /* ESC */
	} /* while */


}

address_rm()
{
int   temp_ctr;
int   should_clear = 0;

	piu = F_AD_PRE_1;
	ad_flags_1 = 0;
	ad_flags_2 = 0;
	ad_flags_3 = 0;
	ad_flags_4 = 0;
	ad_flags_5 = 0;
	ad_flags_6 = 0;
	ad_flags_7 = 0;
	ad_flags_8 = 0;
	ad_flags_9 = 0;
while(1)	{
	intr_allowed = 1;
	key_val = read_field(stdscr,geo_fld,piu,read_buf,&field_entered,&should_clear);
	attrset(0);
	if(saw_intr)  {
		saw_intr = 0;	
		clear_err_line();
		prt_hdr();
		return;
	}
	ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);
	if ((key_val == KEY_DOWN) ||(key_val == '\n') ||(key_val == '\t') ||(key_val == '\r'))	{
		if (piu == F_AD_POST_1)
			piu = F_AD_PRE_2;
		if (piu == F_AD_POST_2)
			piu = F_AD_PRE_3;
		if (piu == F_AD_POST_3)
			piu = F_AD_PRE_4;
		if (piu == F_AD_POST_4)
			piu = F_AD_PRE_5;
		if (piu == F_AD_POST_5)
			piu = F_AD_PRE_6;
		if (piu == F_AD_POST_6)
			piu = F_AD_PRE_7;
		if (piu == F_AD_POST_7)
			piu = F_AD_PRE_8;
		if (piu == F_AD_POST_8)
			piu = F_AD_PRE_9;
		if (piu == F_AD_POST_9)
			piu = F_AD_PRE_1;
	}
	if(key_val == ESC)	{
		clear_err_line();
		if (loop4 == 0)	{
			if ((F_AD_PRE_1 <= piu)  && (piu <= F_AD_PB_1))
				ad_ter = 0;
			else if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
				ad_ter = 1;
			else if ((F_AD_PRE_3 <= piu) && (piu <= F_AD_PB_3))
				ad_ter = 2;
			else if ((F_AD_PRE_4 <= piu) && (piu <= F_AD_PB_4))
				ad_ter = 3;
			else if ((F_AD_PRE_5 <= piu) && (piu <= F_AD_PB_5))
				ad_ter = 4;
			else if ((F_AD_PRE_6 <= piu) && (piu <= F_AD_PB_6))	
				ad_ter = 5;
			else if ((F_AD_PRE_7 <= piu) && (piu <= F_AD_PB_7))	
				ad_ter = 6;
			else if ((F_AD_PRE_8 <= piu) && (piu <= F_AD_PB_8))	
				ad_ter = 7;
			else if ((F_AD_PRE_9 <= piu) && (piu <= NUM_FLDS))	
				ad_ter = 8;

			temp_ad_ptr->record_nbr = adr[ad_ter];
			if (db(ADDRESS_FILE_ID, READ_KEYS, &ad_key3, ISEQUAL, 0) < 0) {
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_90, "Read error"));
				continue;
			}
		}
		else	{	/* not first screen */
			temp_ctr = ad_ctr % 9;
			switch(temp_ctr)	{
				case 0:
					if ((F_AD_PRE_1 <= piu) && (piu <= F_AD_PB_1))
						ad_ter1 = ad_ctr;
					if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
						ad_ter1 = ad_ctr+1;
					if ((F_AD_PRE_3 <= piu) && (piu <= F_AD_PB_3))
						ad_ter1 = ad_ctr+2;
					if ((F_AD_PRE_4 <= piu) && (piu <= F_AD_PB_4))
						ad_ter1 = ad_ctr+3;
					if ((F_AD_PRE_5 <= piu) && (piu <= F_AD_PB_5))
						ad_ter1 = ad_ctr+4;
					if ((F_AD_PRE_6 <= piu) && (piu <= F_AD_PB_6))	
						ad_ter1 = ad_ctr+5;
					if ((F_AD_PRE_7 <= piu) && (piu <= F_AD_PB_7))	
						ad_ter1 = ad_ctr+6;
					if ((F_AD_PRE_8 <= piu) && (piu <= F_AD_PB_8))	
						ad_ter1 = ad_ctr+7;
					if ((F_AD_PRE_9 <= piu) && (piu <= F_AD_PB_9))	
						ad_ter1 = ad_ctr+8;
					break;
				case 1:
						ad_ter1 = ad_ctr+8;
						break;
				case 2:
					if ((F_AD_PRE_1 <= piu) && (piu <= F_AD_PB_1))
						ad_ter1 = ad_ctr+7;
					if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
						ad_ter1 = ad_ctr+8;
					break;
				case 3:
					if ((F_AD_PRE_1 <= piu) && (piu <= F_AD_PB_1))
						ad_ter1 = ad_ctr+6;
					if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
						ad_ter1 = ad_ctr+7;
					if ((F_AD_PRE_3 <= piu) && (piu <= F_AD_PB_3))
						ad_ter1 = ad_ctr+8;
					break;
				case 4:
					if ((F_AD_PRE_1 <= piu) && (piu <= F_AD_PB_1))
						ad_ter1 = ad_ctr+5;
					if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
						ad_ter1 = ad_ctr+6;
					if ((F_AD_PRE_3 <= piu) && (piu <= F_AD_PB_3))
						ad_ter1 = ad_ctr+7;
					if ((F_AD_PRE_4 <= piu) && (piu <= F_AD_PB_4))
						ad_ter1 = ad_ctr+8;
					break;
				case 5:
					if ((F_AD_PRE_1 <= piu) && (piu <= F_AD_PB_1))
						ad_ter1 = ad_ctr+4;
					if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
						ad_ter1 = ad_ctr+5;
					if ((F_AD_PRE_3 <= piu) && (piu <= F_AD_PB_3))
						ad_ter1 = ad_ctr+6;
					if ((F_AD_PRE_4 <= piu) && (piu <= F_AD_PB_4))
						ad_ter1 = ad_ctr+7;
					if ((F_AD_PRE_5 <= piu) && (piu <= F_AD_PB_5))
						ad_ter1 = ad_ctr+8;
					break;
				case 6:
					if ((F_AD_PRE_1 <= piu) && (piu <= F_AD_PB_1))
						ad_ter1 = ad_ctr+3;
					if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
						ad_ter1 = ad_ctr+4;
					if ((F_AD_PRE_3 <= piu) && (piu <= F_AD_PB_3))
						ad_ter1 = ad_ctr+5;
					if ((F_AD_PRE_4 <= piu) && (piu <= F_AD_PB_4))
						ad_ter1 = ad_ctr+6;
					if ((F_AD_PRE_5 <= piu) && (piu <= F_AD_PB_5))
						ad_ter1 = ad_ctr+7;
					if ((F_AD_PRE_6 <= piu) && (piu <= F_AD_PB_6))	
						ad_ter1 = ad_ctr+8;
					break;
				case 7:
					if ((F_AD_PRE_1 <= piu) && (piu <= F_AD_PB_1))
						ad_ter1 = ad_ctr+2;
					if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
						ad_ter1 = ad_ctr+3;
					if ((F_AD_PRE_3 <= piu) && (piu <= F_AD_PB_3))
						ad_ter1 = ad_ctr+4;
					if ((F_AD_PRE_4 <= piu) && (piu <= F_AD_PB_4))
						ad_ter1 = ad_ctr+5;
					if ((F_AD_PRE_5 <= piu) && (piu <= F_AD_PB_5))
						ad_ter1 = ad_ctr+6;
					if ((F_AD_PRE_6 <= piu) && (piu <= F_AD_PB_6))	
						ad_ter1 = ad_ctr+7;
					if ((F_AD_PRE_7 <= piu) && (piu <= F_AD_PB_7))	
						ad_ter1 = ad_ctr+8;
					break;
				case 8:
					if ((F_AD_PRE_1 <= piu) && (piu <= F_AD_PB_1))
						ad_ter1 = ad_ctr+1;
					if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
						ad_ter1 = ad_ctr+2;
					if ((F_AD_PRE_3 <= piu) && (piu <= F_AD_PB_3))
						ad_ter1 = ad_ctr+3;
					if ((F_AD_PRE_4 <= piu) && (piu <= F_AD_PB_4))
						ad_ter1 = ad_ctr+4;
					if ((F_AD_PRE_5 <= piu) && (piu <= F_AD_PB_5))
						ad_ter1 = ad_ctr+5;
					if ((F_AD_PRE_6 <= piu) && (piu <= F_AD_PB_6))	
						ad_ter1 = ad_ctr+6;
					if ((F_AD_PRE_7 <= piu) && (piu <= F_AD_PB_7))	
						ad_ter1 = ad_ctr+7;
					if ((F_AD_PRE_8 <= piu) && (piu <= F_AD_PB_8))	
						ad_ter1 = ad_ctr+8;
					break;
			}

			temp_ad_ptr->record_nbr = adr[ad_ter1];
			if (db(ADDRESS_FILE_ID, READ_KEYS, &ad_key3, ISEQUAL, 0) < 0) {
			    prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_90, "Read error"));
			    continue;
			}
		} /* loop4 > 0 */
			temp_sn_ptr->index = temp_ad_ptr->str_index;
			if((db(ADDRESS_FILE_ID,DELETE,&ad_key1,0,0)) < 0) {
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_93, "Removed  error"));
				continue;
			}
			if ((F_AD_PRE_1 <= piu) && (piu <= F_AD_PB_1))
				clear_flds(F_AD_PRE_1,F_AD_PB_1);
			if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
				clear_flds(F_AD_PRE_2,F_AD_PB_2);
			if ((F_AD_PRE_3 <= piu) && (piu <= F_AD_PB_3))
				clear_flds(F_AD_PRE_3,F_AD_PB_3);
			if ((F_AD_PRE_4 <= piu) && (piu <= F_AD_PB_4))
				clear_flds(F_AD_PRE_4,F_AD_PB_4);
			if ((F_AD_PRE_5 <= piu) && (piu <= F_AD_PB_5))
				clear_flds(F_AD_PRE_5,F_AD_PB_5);
			if ((F_AD_PRE_6 <= piu) && (piu <= F_AD_PB_6))	
				clear_flds(F_AD_PRE_6,F_AD_PB_6);
			if ((F_AD_PRE_7 <= piu) && (piu <= F_AD_PB_7))	
				clear_flds(F_AD_PRE_7,F_AD_PB_7);
			if ((F_AD_PRE_8 <= piu) && (piu <= F_AD_PB_8))	
				clear_flds(F_AD_PRE_8,F_AD_PB_8);
			if ((F_AD_PRE_9 <= piu) && (piu <= F_AD_PB_9))	
				clear_flds(F_AD_PRE_9,NUM_FLDS);
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_92, "Record removed"));
			--loop4;
			break;
		} /* ESC */
	} /* while */



}

/******************************************************************************
* process_output - open an output file and call prt_scr.
*******************************************************************************/

process_output()
{
	FILE *o_file_fp;
	FILE *fopen();

#ifdef DEBUG
	sprintf(trace_str,"Entering process_output\n");
	TRACE(trace_str);
#endif

	intr_allowed = 1;		
	clear_err_line();
	echo();
	prompt_fname();	
	if(saw_intr)  {
		noecho();
		saw_intr = 0;		
		intr_allowed = 0; 	
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_output - 1\n");
	TRACE(trace_str);
#endif
		return;
	}

	prompt_mode();		

	noecho();
	if(saw_intr)  {
		saw_intr = 0;	
		intr_allowed = 0; 
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_output - 2\n");
	TRACE(trace_str);
#endif
		return;
	}

	if((o_file_fp = fopen(o_file,mode)) == NULL)  {
		prt_error(OPEN_OUT_ERR);
		sleep(3);
		ERROR(' ',ZONE_FILE,OPEN_OUT_ERR);
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_output - 3\n");
	TRACE(trace_str);
#endif
		return;
	}

        prt_scr(o_file_fp);
	fclose(o_file_fp);
	prt_hdr();

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_output - 4\n");
	TRACE(trace_str);
#endif
	return;
}   

/*****************************************************************************
* store_def - store default values for the fleet/menu permission arrays.
****************************************************************************/

store_def()
{
/*
	int i,j;	

#ifdef DEBUG
	sprintf(trace_str,"Entering store_def\n");
	TRACE(trace_str);
#endif
	zn_ptr->type = '2';
	zn_ptr->superzn_detail = NO;
	zn_ptr->bid_allowed = NO;
	zn_ptr->enr_post_allow = NO;
	for(i = 0; i < 16 ; i++)	{
		zn_ptr->pckup_seals[i] = NO;
	}
#ifdef DEBUG
	sprintf(trace_str,"Leaving store_def\n");
	TRACE(trace_str);
#endif
	return;
*/
}   


/*********************************************************************
 * Key description.
 ********************************************************************/

key_desc(piu)
int  piu;
{
	int     k;
	move(ERROR_ROW-1,ERROR_COL);
	refresh();
	for (k = 0; k < 131; ++k)
		addch(' ');
	switch(piu)	{
		case 1:
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_94, " Index number "));
			break;
		case 2:
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_95, " Street full name "));
			break;
		case 3:
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_96, " Street type "));
			break;
		case 4:
		case 5:
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_97, " Street name abbreviation "));
			break;
		case 6:
		case 7:
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_98, " Street alias name  "));
			break;
		case F_PL_NAME_1:
		case F_PL_NAME_2:
		case F_PL_NAME_3:
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_99, " Place name "));
			break;

		case F_AD_PRE_1:
		case F_AD_PRE_2:
		case F_AD_PRE_3:
		case F_AD_PRE_4:
		case F_AD_PRE_5:
		case F_AD_PRE_6:
		case F_AD_PRE_7:
		case F_AD_PRE_8:
		case F_AD_PRE_9:		  
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_100, " Street pre direction "));
			break;

	case F_AD_POST_1:
	case F_AD_POST_2:
	case F_AD_POST_3:
	case F_AD_POST_4:
	case F_AD_POST_5:
	case F_AD_POST_6:
	case F_AD_POST_7:
	case F_AD_POST_8:
	case F_AD_POST_9:	  
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_101, " Street post direction "));
			break;
			
		case F_PL_CITY_1:
		case F_PL_CITY_2:
		case F_PL_CITY_3:
	case F_AD_CITY_1:
	case F_AD_CITY_2:
	case F_AD_CITY_3:
	case F_AD_CITY_4:
	case F_AD_CITY_5:
	case F_AD_CITY_6:
	case F_AD_CITY_7:
	case F_AD_CITY_8:
	case F_AD_CITY_9:	  
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_102, " City code "));
			break;
			
		case F_PL_OE_1:
		case F_PL_OE_2:
		case F_PL_OE_3:
	case F_AD_OE_1:
	case F_AD_OE_2:
	case F_AD_OE_3:
	case F_AD_OE_4:
	case F_AD_OE_5:
	case F_AD_OE_6:
	case F_AD_OE_7:
	case F_AD_OE_8:
	case F_AD_OE_9:	  
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_103, "  E-even, O-odd, B-both "));
			break;
			
		case F_ST_NBR_1:
		case F_ST_NBR_2:
		case F_ST_NBR_3:
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_104, " Street address number "));
			break;


	case F_AD_ZIP_1:
	case F_AD_ZIP_2:
	case F_AD_ZIP_3:
	case F_AD_ZIP_4:
	case F_AD_ZIP_5:
	case F_AD_ZIP_6:
	case F_AD_ZIP_7:
	case F_AD_ZIP_8:
	case F_AD_ZIP_9:	  
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_106, " Zip code associated with this address or address range "));
			break;
			
		case F_PL_SBZ_1:
		case F_PL_SBZ_2:
		case F_PL_SBZ_3:
	case F_AD_SBZ_1:
	case F_AD_SBZ_2:
	case F_AD_SBZ_3:
	case F_AD_SBZ_4:
	case F_AD_SBZ_5:
	case F_AD_SBZ_6:
	case F_AD_SBZ_7:
	case F_AD_SBZ_8:
	case F_AD_SBZ_9:	  
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_107, " Subzone associated with this address or address range "));
			break;
			
		case F_PL_CMNT_1:
		case F_PL_CMNT_2:
		case F_PL_CMNT_3:
	case F_AD_CMNT_1:
	case F_AD_CMNT_2:
	case F_AD_CMNT_3:
	case F_AD_CMNT_4:
	case F_AD_CMNT_5:
	case F_AD_CMNT_6:
	case F_AD_CMNT_7:
	case F_AD_CMNT_8:
	case F_AD_CMNT_9:	  
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_108, " Location comment for this street record "));
			break;
			
	case F_AD_X_CD_1:
	case F_AD_X_CD_2:
	case F_AD_X_CD_3:
	case F_AD_X_CD_4:
	case F_AD_X_CD_5:
	case F_AD_X_CD_6:
	case F_AD_X_CD_7:
	case F_AD_X_CD_8:
	case F_AD_X_CD_9:	  
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_109, " X coordinate of the street "));
			break;
			
	case F_AD_Y_CD_1:
	case F_AD_Y_CD_2:
	case F_AD_Y_CD_3:
	case F_AD_Y_CD_4:
	case F_AD_Y_CD_5:
	case F_AD_Y_CD_6:
	case F_AD_Y_CD_7:
	case F_AD_Y_CD_8:
	case F_AD_Y_CD_9:	  
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_110, " Y coordinate of the street "));
			break;
			
		case F_PL_MAP_1:
		case F_PL_MAP_2:
		case F_PL_MAP_3:
	case F_AD_MAP_1:
	case F_AD_MAP_2:
	case F_AD_MAP_3:
	case F_AD_MAP_4:
	case F_AD_MAP_5:
	case F_AD_MAP_6:
	case F_AD_MAP_7:
	case F_AD_MAP_8:
	case F_AD_MAP_9:	  
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_111, " Map page reference number "));
			break;
			
		case F_PL_PB_1:
		case F_PL_PB_2:
		case F_PL_PB_3:
	case F_AD_PB_1:
	case F_AD_PB_2:
	case F_AD_PB_3:
	case F_AD_PB_4:
	case F_AD_PB_5:
	case F_AD_PB_6:
	case F_AD_PB_7:
	case F_AD_PB_8:
	case F_AD_PB_9:	  
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_112, " Map block designation "));
			break;
			
		case F_LOC_1:
		case F_LOC_2:
		case F_LOC_3:
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_113, " Y/N, whether this place name implies a local pickup "));
			break;
			
	case F_LO_NBR_1:
	case F_LO_NBR_2:
	case F_LO_NBR_3:
	case F_LO_NBR_4:
	case F_LO_NBR_5:
	case F_LO_NBR_6:
	case F_LO_NBR_7:
	case F_LO_NBR_8:
	case F_LO_NBR_9:	  
			prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_114, " Low number of the street section "));
			break;
	case F_HI_NBR_1:
	case F_HI_NBR_2:
	case F_HI_NBR_3:
	case F_HI_NBR_4:
	case F_HI_NBR_5:
	case F_HI_NBR_6:
	case F_HI_NBR_7:
	case F_HI_NBR_8:
	case F_HI_NBR_9:
	  prt_desc(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_115, " High number of the street section "));
	  break;
	default:
	  break;
	}
}

ch_option()
{
    while (1) {
	c = getkey();
	if (c == '0' || c == '1' || c == '2' ||
	    c == '3' || c == '4')
	    return(c);
    }

}
		
