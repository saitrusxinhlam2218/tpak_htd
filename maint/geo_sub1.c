static char sz__FILE__[]="@(#)geo_sub1.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  geo_sub1.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:47:50
* @(#)  Last delta: 12/2/94 at 11:35:10
* @(#)  SCCS File: /taxi/sccs/s.geo_sub1.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
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

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <isam.h>
#include <cursesX.h>
#include <signal.h>
#include <memory.h>

#include "geo_m_strdefs.h"
#include "mad_error.h"
#include "mads_types.h"
#include "df_maint.h"
#include "mads_isam.h"
#include "ex_key.h"
#include "geo_scr.h"
#include "geo_msg.h"
#include "tokenize.h"
#include "language.h"


extern struct scr_flds geo_fld[];
FILE *fp,*fopen();
char buffer[10];
extern int upper_case;
int   c,op;
int  temp_int;
int  pl_counter = 0,pl_counter1 = 0;
int  ad_counter = 0,ad_counter1 = 0;
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
extern struct file_desc isam_file[];
extern char  current_street_type[];
extern char  current_street_name[];
extern int search_qur();
extern int temp_index;
extern int key_desc();
extern int prt_data();
extern char mode[];		/* user specified Append or Create */	
extern char buf[];			
extern char req_buf[];			
extern int pre_flag,post_flag,city_flag,oe_flag,nbr_flag,suf_flag,zip_flag;
extern int subzone_flag,x_flag,y_flag,map_flag,loc_flag,high_flag,low_flag;
extern  loop1;
extern  loop2;
extern  loop3;
extern  loop4;
extern struct cisam_sn *sn_ptr; /* pointer to street name file structure */
extern struct cisam_sn *temp_sn_ptr; 
extern struct cisam_sn sn_buf; 
extern struct cisam_lo *lo_ptr; /* pointer to locality file structure */
extern struct cisam_lo *temp_lo_ptr; 
extern struct cisam_lo lo_buf; 
extern struct cisam_sz *sz_ptr; /* pointer to subzone file structure */
extern struct cisam_sz *temp_sz_ptr; 
extern struct cisam_sz sz_buf; 
extern struct cisam_pl *pl_ptr; /* pointer to place name file structure */
extern struct cisam_pl *temp_pl_ptr; 
extern struct cisam_pl pl_buf; 
extern struct cisam_ad *ad_ptr; /* pointer to address name file structure */
extern struct cisam_ad *temp_ad_ptr; 
extern struct cisam_ad ad_buf; 
extern int clear_flds();	/* clear data fields on the screen */
int doing_add = 0; 		/* performing add operation */
int address_record_nbr;
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
short ret_begin;
int legal_city,legal_zone;	/*force user to enter city and street correctly to enter place name */
/******************************************************************************
* process_query - search for specific record in the zone file
*******************************************************************************/

process_query()
{
	int i,a,t = 0;				/* loop index */
	int should_clear = 0;
#ifdef DEBUG
	sprintf(trace_str,"Entering process_query\n");
	TRACE(trace_str);
#endif
	for (i = 0; i< 1024; ++i)	{
		abb[i] = 0;
		als[i] = 0;
		plc[i] = 0;
		adr[i] = 0;
		high[i] = 0;
		low[i] = 0;
	}
	piu = 1;
	clear_err_line();
	clear_flds(1,NUM_FLDS);
	prt_query_hdr();		/* Print the header for the query 
					   screen */

	/*  Process users input of key field(index).  */

	while(1)  {
		key_desc(piu);
		intr_allowed = 1;		/* interrupts allowed */
		key_val = read_field(stdscr,geo_fld,piu,read_buf,&field_entered,&should_clear);
		attrset(0);
		if(saw_intr)  {			/* key_value = interrupt char */
			saw_intr = 0;		/* interrup has been processed */
			index_flag = 0;
			name_flag = 0;
			type_flag = 0;
			clear_err_line();
			clear_flds(1,NUM_FLDS);
			cur_list = 0;
			prt_hdr();
			return;
		}

		/* validate input data, store it in c-isam structure, and
		   adjust the field pointer */

		ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);

			/* if used sn_key1,then press ESC to execute */
			/* sn_key2 must enter value,if sn_key1 skip */
		if ((piu == 2) &&(index_flag) &&(key_val != ESC))	{
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_45, "Enter ESC to execute"));
			piu = 1;
			continue;
		}
		if ((piu == 3) && (!index_flag) && (!name_flag)) 	{
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_46, "You must enter value"));
			piu = 2;
			continue;
		}
#ifdef NAMERICA
		if ((piu == 4) && (!type_flag) && (key_val != KEY_UP)) 	{
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_46, "You must enter value"));
			piu = 3;
			continue;
		}
#endif
		if ((piu == 4) && (key_val != KEY_UP) &&(key_val != ESC)) 	{
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_45, "Enter ESC to execute"));
			piu = 3;
			continue;
		}



	if(key_val == ESC)   {
				ab_ctr = 0;
				al_ctr = 0;
				pl_ctr = 0;
				ad_ctr = 0;
			if (piu == 1)	
			{
				temp_sn_ptr->index = sn_ptr->index;
				temp_sn_ptr->rec_type = '1';
			}
			if (!index_flag)	
				temp_sn_ptr->index = 0;

			if ((piu == 2) ||(piu == 3))	{
				strcpy(temp_sn_ptr->name,sn_ptr->name);
				strcpy(temp_sn_ptr->str_type,sn_ptr->str_type);
				if(db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISGTEQ,0) < 0)	{
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_47, "No record found"));
					piu = 2;
					continue;
				}
			}
				if(db(STRNAME_FILE_ID,READ_KEYS,&sn_key1,ISGTEQ,0) < 0)	{
					prt_error(NO_CUR_LIST);
					piu = 1;
					continue;
				}
				cur_list = 1;
/*
 *				fix
 *				clear fields before showing db record
 */
				clear_flds(1,3);

				mvprintw(1,1,"%18d",temp_sn_ptr->index);
				refresh();
				temp_index = temp_sn_ptr->index;
				search_qur();
				index_flag = 0;
				name_flag = 0;
				type_flag = 0;
				

		/*  Using the key entered in previous step, call C-Isam routine
	    	    to retrieve specific record.   */
			clear_err_line();

			cur_list = 1;
			prt_hdr();
			break;
		}
}

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_query - 4\n");
	TRACE(trace_str);
#endif


}  /* End of process_query()  */






/******************************************************************************
* process_next - read the next record in the user zone file and display it.
*******************************************************************************/

process_next()
{

	if (!cur_list)	{
		prt_error(NO_CUR_LIST);
#ifdef DEBUG
	sprintf(trace_str,"Entering process_next\n");
	TRACE(trace_str);
#endif
		return;
	}
while(1)	{
	mvaddstr(ERROR_ROW-1,1,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_48, " [NEXT MODE]   (0) Main menu (1) Street name & type (2) Abbreviation (3) Aliases (4) Place name (5) Address : "));
	refresh();
	op = ch_option1();
	switch(op)	{
		case '0':
			clear_err_line();
			move(ERROR_ROW-1,1);
			clrtoeol();
			refresh();
			return;
		case '1':
			stt_next();
			break;
		case '2':
			abbrev_next();
			break;
		case '3':
			alias_next();
			break;
		case '4':
			place_next();
			break;
		case '5':
			address_next();
			break;
	}

}/* while */

}  /* End of process_next()  */

stt_next()
{
        int  i;

	temp_sn_ptr->index = temp_index;
	temp_sn_ptr->rec_type = '1';
	clear_err_line();
	if((db(STRNAME_FILE_ID,READ_KEYS,&sn_key1,ISGTEQ,0)) < 0)  
	{
		prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_49, "No record available"));
	}
	else	
	{
		do {
		if(db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISGREAT,0) < 0)
		{
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_49, "No record available"));
			return;
		}
		}
		while(temp_sn_ptr->rec_type != '1');
		for (i= 0; i< 1024; ++i)	{
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
		clear_flds(1,NUM_FLDS);
		temp_index = temp_sn_ptr->index;
		cur_list = 1;
		db(STRNAME_FILE_ID,READ_KEYS,&sn_key1,ISGTEQ,0);
		mvprintw(1,1,"%18d",temp_sn_ptr->index); /* Display index in first field */
		refresh();
		search_qur();
	}
}

abbrev_next()
{
	int  i;

	clear_err_line();
	if(ab_ctr <= 0)
		ab_ctr = 0;
	if(loop1 <= 0)
		loop1 = 0;
	for (i = 0; i <= 1; ++i)	{
	        temp_sn_ptr->record_nbr = abb[ab_ctr+2];
		if((db(STRNAME_FILE_ID, READ_KEYS, &sn_key3, ISEQUAL, 0)) < 0) 	{
			if ( i == 0)	{
				prt_error(NO_CUR_LIST); 
			}
			return(-1);
		}
		if (i == 0)
			clear_flds(4,5);
		mvprintw(D_ST_NAME_ROW+i,D_ABBRV_1_COL,"%s",temp_sn_ptr->name);
		refresh();
		++ab_ctr;
		++loop1;
	}
}

alias_next()
{
        int  i;

	clear_err_line();
	if(al_ctr <= 0)
		al_ctr = 0;
	if(loop2 <= 0)
		loop2 = 0;
	for (i = 0; i <= 1; ++i)	{
	        temp_sn_ptr->record_nbr = als[al_ctr+2];
		if((db(STRNAME_FILE_ID, READ_KEYS, &sn_key3, ISEQUAL, 0)) < 0) 	{
			if ( i == 0)	{
				prt_error(NO_CUR_LIST); 
			}
			return(-1);
		}
		if ( i == 0)
			clear_flds(F_ALIAS_1,F_ALIAS_2);
		mvprintw(D_ST_NAME_ROW+i,D_ALIAS_1_COL,"%s",temp_sn_ptr->name);
		refresh();
		++al_ctr;
		++loop2;
	}
}


place_next()
{
        int  i;

	clear_err_line();
	if(pl_ctr <= 0)
		pl_ctr = 0;
	if(loop3 <= 0)
		loop3 = 0;
	for (i = 0; i <= 2; ++i)	{
	        temp_pl_ptr->record_nbr = plc[pl_ctr+3];
		if((db(PLACENM_FILE_ID, READ_KEYS, &pl_key3, ISEQUAL, 0)) < 0) 	{
			if (i == 0)	{
				prt_error(NO_CUR_LIST); 
			}
			return(-1);
		}
		if ( i == 0)	
			clear_flds(F_PL_NAME_1,F_LOC_3);
		prt_dt1(i);
		++pl_ctr;
		++loop3;
	}
}

address_next()
{
       int  i;

	clear_err_line();
	if (ad_ctr <= 0)
		ad_ctr = 0;
	if(loop4 <= 0)
		loop4 = 0;
	for (i = 0; i <= 8; ++i)	{
	        temp_ad_ptr->record_nbr = adr[ad_ctr+9];
		if((db(ADDRESS_FILE_ID, READ_KEYS, &ad_key3, ISEQUAL, 0)) < 0) 	{
			if ( i == 0)	{
				prt_error(NO_CUR_LIST); 
			}
			return(-1);
		}
		if ( i == 0)
			clear_flds(F_AD_PRE_1,NUM_FLDS);
		prt_dt2(i);
		++ad_ctr;
		++loop4;
	}
}

/******************************************************************************
* process_previous - read the previous record in the zone file and display it.
*******************************************************************************/

process_previous()
{
	if (!cur_list)	{
		prt_error(NO_CUR_LIST);
#ifdef DEBUG
	sprintf(trace_str,"Entering process_previous\n");
	TRACE(trace_str);
#endif
		return;
	}
while(1)	{
	mvaddstr(ERROR_ROW-1,1,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_50, " [PREV MODE]   (0) Main menu (1) Street name & type (2) Abbreviation (3) Aliases (4) Place name (5) Address : "));
	refresh();
	op = ch_option1();
	switch(op)	{
		case '0':
			clear_err_line();
			move(ERROR_ROW-1,1);
			clrtoeol();
			refresh();
			return;
		case '1':
			stt_prev();
			break;
		case '2':
			abbrev_prev();
			break;
		case '3':
			alias_prev();
			break;
		case '4':
			place_prev();
			break;
		case '5':
			address_prev();
			break;
	}

} /* while */
}

stt_prev()
{
        int  i;

	temp_sn_ptr->index = temp_index;
	temp_sn_ptr->rec_type = '1';
	clear_err_line();
	if((db(STRNAME_FILE_ID,READ_KEYS,&sn_key1,ISGTEQ,0)) < 0)  
	{
		prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_49, "No record available"));
	}
	else	
	{
		if(db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISGTEQ,0) < 0)
		{
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_49, "No record available"));
			return;
		}
		do {
		db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISPREV,0);
		}
		while (temp_sn_ptr->rec_type != '1');
		db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISEQUAL,0);

		for (i= 0; i< 1024; ++i)	{
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
		clear_flds(1,NUM_FLDS);
		temp_index = temp_sn_ptr->index;
		cur_list = 1;
		db(STRNAME_FILE_ID,READ_KEYS,&sn_key1,ISGTEQ,0);
		mvprintw(1,1,"%18d",temp_sn_ptr->index); /* Display index in first field */
		refresh();
		search_qur();
	}
}

abbrev_prev()
{
        int  i;

	clear_err_line();
	if (ab_ctr <= 0)	{
		prt_error(NO_CUR_LIST);
		return;
	}
	if ((ab_ctr%2) == 0)
		--ab_ctr;
	for (i = 1; i >= 0; --i)	{
	        temp_sn_ptr->record_nbr = abb[ab_ctr];
		if (db(STRNAME_FILE_ID, READ_KEYS, &sn_key3, ISEQUAL, 0) < 0)  	{
				prt_error(NO_CUR_LIST);
				return;
		}
		if (i == 1)
			clear_flds(F_ABBRV_1,F_ABBRV_2);
		mvprintw(D_ST_NAME_ROW+i,D_ABBRV_1_COL,"%s",temp_sn_ptr->name);
		refresh();
		--ab_ctr;
		--loop1;
	}


}

alias_prev()
{
        int  i;

	clear_err_line();
	if (al_ctr <= 0)	{
		prt_error(NO_CUR_LIST);
		return;
	}
	if ((al_ctr%2) == 0)
		--al_ctr;
	for (i = 1; i >= 0; --i)	{
	        temp_sn_ptr->record_nbr = als[al_ctr];
		if (db(STRNAME_FILE_ID, READ_KEYS, &sn_key3, ISEQUAL, 0) < 0)  	{
				prt_error(NO_CUR_LIST);
				return;
		}
		if (i == 1)
			clear_flds(F_ALIAS_1,F_ALIAS_2);
		mvprintw(D_ST_NAME_ROW+i,D_ALIAS_1_COL,"%s",temp_sn_ptr->name);
		refresh();
		--al_ctr;
		--loop2;
	}

}

place_prev()
{
        int  i;

	clear_err_line();
	if (pl_ctr <= 0)	{
		prt_error(NO_CUR_LIST);
		return;
	}
	if ((pl_ctr%3) == 0)
		--pl_ctr;
	if ((pl_ctr%3) == 1)
		++pl_ctr;
	for (i = 2; i >= 0; --i)	{
	        temp_pl_ptr->record_nbr = plc[pl_ctr];
		if (db(PLACENM_FILE_ID, READ_KEYS, &pl_key3, ISEQUAL, 0) < 0) 	{
				prt_error(NO_CUR_LIST); 
				return;
		}
		if (i == 2)
			clear_flds(F_PL_NAME_1,F_LOC_3);
		prt_dt1(i);
		if (i != 0)	{
			--pl_ctr;
			--loop3;
		}
	}

}

address_prev()
{
        int     temp_ctr;
	int     i;

	clear_err_line();
	if (ad_ctr <= 0)	{
		prt_error(NO_CUR_LIST);	
		return;
	}
	temp_ctr = (ad_ctr % 9);
	switch(temp_ctr)	{
		case 0:
			ad_ctr -= 1;
			break;
		case 1:
			ad_ctr += 7;
			break;
		case 2:
			ad_ctr += 6;
			break;
		case 3:
			ad_ctr += 5;
			break;
		case 4:
			ad_ctr += 4;
			break;
		case 5:
			ad_ctr += 3;
			break;
		case 6:
			ad_ctr += 2;
			break;
		case 7:
			ad_ctr += 1;
			break;
	}
		
		
	for (i = 8; i >= 0; --i)	{
	        temp_ad_ptr->record_nbr = adr[ad_ctr];
		if (db(ADDRESS_FILE_ID, READ_KEYS, &ad_key3, ISEQUAL, 0) < 0) 	{
			prt_error(NO_CUR_LIST);
			return;
		}
		if (i == 8)
			clear_flds(F_AD_PRE_1,NUM_FLDS);
		prt_dt2(i);
		if (i != 0)	{
			--ad_ctr;
			--loop4;
		}
	}

}

/******************************************************************************
* process_add - input data fields and add the new record to the zone file 
*******************************************************************************/

process_add()
{
int    i;
#ifdef DEBUG
	sprintf(trace_str,"Entering process_add\n");
	TRACE(trace_str);
#endif
	clear_err_line();
	doing_add = 1;
	ret_begin = 0;
while(1)	{
	if(ret_begin)
	{
		op = '0';
	}
	else
	{
		mvaddstr(ERROR_ROW-1,1,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_51, " [ADD MODE]   (0) Main menu (1) New street name & type (2) Abbreviation (3) Aliases (4) Place name (5) Address : "));
		refresh();
		op = ch_option1();
	}
	switch(op)	{
		case '0':
			clear_err_line();
			prt_hdr();
		 	move(ERROR_ROW-1,1);
			clrtoeol();
			refresh();
			clear_flds(1,NUM_FLDS);
			doing_add = 0;
			init_flag(); 
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
			for (i= 0; i< 1024; ++i)	{
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
			clear_err_line();
			name_add();
			break;
		case '2':
			clear_err_line();
			abbrev_add();
			break;
		case '3':
			clear_err_line();
			alias_add();
			break;
		case '4':
			clear_err_line();
			memset((char *)pl_ptr,'\0',sizeof(struct cisam_pl));
			place_add();
			break;
		case '5':
			clear_err_line();
			memset((char *)ad_ptr,'\0',sizeof(struct cisam_ad));
			address_add();
			break;
	}

} /* while */

}
	/* reset all the members of the zone file structure */

name_add()
{
int should_clear = 0;
long val;

piu = 2;
clear_flds(1,NUM_FLDS);
while(1)	{
	key_desc(piu);
	prt_add_hdr();		
	intr_allowed = 1;
	key_val = read_field(stdscr,geo_fld,piu,read_buf,&field_entered,&should_clear);
	attrset(0);
	intr_allowed = 0;
	if(saw_intr)  {
		saw_intr = 0;	
		clear_err_line();
		clear_flds(F_ST_NAME,F_TYPE);
		init_flag(); 
		name_flag = 0;
		type_flag = 0;
		ret_begin = 1;
		prt_hdr();
		return;
	}
	ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);
	if ((piu == 3) &&(!name_flag))	{
		prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_52, "You must enter street name"));
		piu = 2;
		continue;
	}
	if ((piu == 3) &&(type_flag) && (key_val != ESC))	{
		prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_45, "Enter ESC to execute"));
		piu = 3;
		continue;
	}
		if(key_val == ESC)	{
			clear_err_line();
			sn_ptr->record_nbr = temp_sn_ptr->record_nbr;
			memcpy(req_buf,(char *)sn_ptr,sizeof(struct cisam_sn));
			if(db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISEQUAL,0) >= 0)	{
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_53, "Duplicated street name & type found"));
				piu = 2;
				continue;
			}

			temp_sn_ptr->index = 1;
			db(STRNAME_FILE_ID,READ_KEYS,&sn_key1,ISGTEQ,0);
			db(STRNAME_FILE_ID,READ_KEYS,&sn_key1,ISLAST,0);
			sn_ptr->rec_type = '1';
			sn_ptr->index = temp_sn_ptr->index + 1;
			mvprintw(1,1,"%18d",sn_ptr->index);
			refresh();
			memcpy(req_buf,(char *)sn_ptr,sizeof(struct cisam_sn)); 
			if((db(STRNAME_FILE_ID,ADD_REC,&sn_key1,0,0)) < 0) {
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_54, "Add record  error"));
				continue;
			}

			strcpy(current_street_name, sn_ptr->name);
			strcpy(current_street_type, sn_ptr->str_type);

			strcpy(sn_ptr->name,"    ");
			strcpy(sn_ptr->str_type,"  ");
			name_flag = 0;
			type_flag = 0;
			temp_index = sn_ptr->index;
			break;
		}	
	}
}
abbrev_add()
{
	int should_clear = 0;

	piu = 4;
	clear_flds(F_ABBRV_1,F_ABBRV_2);
while(1)	{
	key_desc(piu);
	prt_add_hdr();		
	intr_allowed = 1;
	key_val = read_field(stdscr,geo_fld,piu,read_buf,&field_entered,&should_clear);
	attrset(0);
	intr_allowed = 0;
	if(saw_intr)  {
		saw_intr = 0;	
		clear_err_line();
		clear_flds(F_ABBRV_1,F_ABBRV_2);
		init_flag();
		ab_flags_1 = 0;
		ab_flags_2 = 0;
		prt_hdr();
		return;
	}
	ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);
	if ((piu == 4) &&(ab_flags_1) && (key_val != ESC))	{
		prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_45, "Enter ESC to execute"));
		piu = 4;
		continue;
	}
		piu = 4;
	if(key_val == ESC)	{
		clear_err_line();
			sn_ptr->index = temp_index;
			strcpy(sn_ptr->str_type,"  "); 
			sn_ptr->rec_type = '2';
			memcpy(req_buf,(char *)sn_ptr,sizeof(struct cisam_sn));
			/*  Call the C-Isam add record routine.  */
			if((db(STRNAME_FILE_ID,ADD_REC,&sn_key1,0,0)) < 0) {
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_54, "Add record  error"));
				continue;
			}
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_55, "Record added"));
			ab_flags_1 = 0;
			ab_flags_2 = 0;
			break;
		} /* ESC */
	} /* while */
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_add - 2\n");
	TRACE(trace_str);
#endif
}  

alias_add()
{
	int should_clear = 0;

	piu = 6;
	clear_flds(F_ALIAS_1,F_ALIAS_2);
while(1)	{
	key_desc(piu);
	prt_add_hdr();		
	intr_allowed = 1;
	key_val = read_field(stdscr,geo_fld,piu,read_buf,&field_entered,&should_clear);
	attrset(0);
	intr_allowed = 0;
	if(saw_intr)  {
		saw_intr = 0;	
		clear_err_line();
		clear_flds(F_ALIAS_1,F_ALIAS_2);
		init_flag();
		al_flags_1 = 0;
		al_flags_2 = 0;
		prt_hdr();
		return;
	}
	ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);
	if ((piu == 6) &&(al_flags_1) && (key_val != ESC))	{
		prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_45, "Enter ESC to execute"));
		piu = 6;
		continue;
	}
		piu = 6;
	if(key_val == ESC)	{
		clear_err_line();
			sn_ptr->index = temp_index;
			strcpy(sn_ptr->str_type, current_street_type); 
			sn_ptr->rec_type = '3';
			memcpy(req_buf,(char *)sn_ptr,sizeof(struct cisam_sn));
			/*  Call the C-Isam add record routine.  */
			if((db(STRNAME_FILE_ID,ADD_REC,&sn_key1,0,0)) < 0) {
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_54, "Add record  error"));
				continue;
			}
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_55, "Record added"));
			al_flags_1 = 0;
			al_flags_2 = 0; 
				break; 
	} /* ESC */ 
} /* while */ 
#ifdef DEBUG
	 sprintf(trace_str,"Leaving process_add - 2\n"); 
		TRACE(trace_str); 
#endif 
} 
 place_add() 
{ 
	int should_clear = 0;
legal_city = legal_zone = 0;
piu = F_PL_NAME_1; 
clear_flds(F_PL_NAME_1,F_LOC_3); 
while(1)	{ 
	key_desc(piu);
	prt_add_hdr();		
	intr_allowed = 1;
	key_val = read_field(stdscr,geo_fld,piu,read_buf,&field_entered,&should_clear);
	attrset(0);
	intr_allowed = 0;
	if(saw_intr)  {
		saw_intr = 0;	
		clear_err_line();
		clear_flds(F_PL_NAME_1,F_LOC_3);
		init_flag();
		init_pl_flag();
		prt_hdr();
		return;
	}
	ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);
	if (ret_val == -1)
		continue;
	if(legal_city != 1 && key_val == ESC)	/* Must have a valid City and street to enter place name*/
	{ 
		prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_56, "Invalid city"));
		continue;
	}
	if(legal_zone != 1 && key_val == ESC)
	{ 
		prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_57, "Invalid subzone"));
		continue;
	}
	if(key_val == ESC)	{
		clear_err_line();
			pl_ptr->index = temp_index;
			if(pl_ptr->local != YES)
				pl_ptr->local = NO;
			strcpy(pl_ptr->str_name, current_street_name);
			strcpy(pl_ptr->str_type, current_street_type);
			memcpy(req_buf,(char *)pl_ptr,sizeof(struct cisam_pl));
			/*  Call the C-Isam add record routine.  */
			if((db(PLACENM_FILE_ID,ADD_REC,&pl_key1,0,0)) < 0) {
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_54, "Add record  error"));
				continue;
			}
			/*prt_error("Record added");*/
		        temp_ad_ptr->record_nbr = address_record_nbr;
			if (db(ADDRESS_FILE_ID, READ_KEYS, &ad_key3, ISEQUAL, 0) < 0)	
			/*	prt_error("Record read + locked error")*/;
			else	{
					temp_ad_ptr->place_names = YES;
				if((db(ADDRESS_FILE_ID,UPDATE_REC,&pl_key1,0,0)) < 0) 
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_58, "Update address record  error"));
			}
			db(ADDRESS_FILE_ID,RELEASE,&ad_key1,0,0);	
				
			init_pl_flag();
			init_flag();
			break;
		} /* ESC */
	} /* while */
}
address_add()
{
int should_clear = 0;

	piu = F_AD_PRE_1;
	clear_flds(F_AD_PRE_1,NUM_FLDS);

	subzone_flag = 0;
	city_flag = 0;
	oe_flag = 0;
	
while(1)	{
	key_desc(piu);
	prt_add_hdr();		
	intr_allowed = 1;
	key_val = read_field(stdscr,geo_fld,piu,read_buf,&field_entered,&should_clear);
	attrset(0);
	intr_allowed = 0;
	if(saw_intr)  {
		saw_intr = 0;	
		clear_err_line();
		clear_flds(F_AD_PRE_1,NUM_FLDS);
		init_flag();
		init_ad_flag();
		prt_hdr();
		return;
	}
	ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);
	if (ret_val == -1)
		continue;
	if(key_val == ESC)	{
		clear_err_line();
			ad_ptr->str_index = temp_index;
			strcpy(ad_ptr->str_name, current_street_name);
			strcpy(ad_ptr->str_type, current_street_type);

			if(!city_flag) {
			  piu = F_AD_CITY_1;
                          prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_14, "Invalid city code"));
			  continue;
			}

			if(!oe_flag) {
			  piu = F_AD_OE_1;
                          prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_15, "Error in field"));
			  continue;
			}

			if(ad_ptr->low_nbr < 0) {
			  piu = F_LO_NBR_1;
			  prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_23, "Invalid low number"));
			  continue;
			}

			if(ad_ptr->low_nbr >= ad_ptr->high_nbr) {
			  piu = F_HI_NBR_1;
			  prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_26, "Invalid high number"));
			  continue;
			}

			if(!subzone_flag) {
			  piu = F_AD_SBZ_1;
                          prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_19, "Invalid subzone number"));
			  continue;
			}

			memcpy(req_buf,(char *)ad_ptr,sizeof(struct cisam_ad));
			if((db(ADDRESS_FILE_ID,ADD_REC,&ad_key1,0,0)) < 0) {
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_59, "Added  error"));
				continue;
			}
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_55, "Record added"));
			init_ad_flag();
			init_flag();
			break;
		} /* ESC */
	} /* while */
}

/******************************************************************************
* process_update - input data fields and update the zone file record
*******************************************************************************/

process_update()
{
	int  i;

	if (!cur_list)	{
		prt_error(NO_CUR_LIST);
		return;
	}
#ifdef DEBUG
	sprintf(trace_str,"Entering process_update\n");
	TRACE(trace_str);
#endif
	prt_upd_hdr();
	ret_begin = 0;
while(1)	{
	if(ret_begin)
	{
			move(ERROR_ROW-1,1);
			clrtoeol();
			refresh();
			init_flag();
			temp_sn_ptr->index = temp_index;
			temp_sn_ptr->rec_type = '1';
			clear_flds(1,NUM_FLDS);
			if(db(STRNAME_FILE_ID,READ_KEYS,&sn_key1,ISGTEQ,0) < 0)	{
					prt_error(NO_CUR_LIST);
					return;
			}
			cur_list = 1;
			mvprintw(1,1,"%18d",temp_sn_ptr->index);
			refresh();
			temp_index = temp_sn_ptr->index;
			for (i= 0; i< 1024; ++i)	{
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
	/* 		op = '0';*/
	}
/*	else
	{*/
		mvaddstr(ERROR_ROW-1,1,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_60, " [UPDATE MODE]   (0) Main menu (1) Abbreviation (2) Aliases (3) Place name (4) Address : "));
		refresh();
		op = ch_option();

	switch(op)	{
		case '0':
			clear_err_line();
			prt_hdr();
			move(ERROR_ROW-1,1);
			clrtoeol();
			refresh();
			init_flag();
			return;
		case '1':
			abbrev_updt();
			break;
		case '2':
			alias_updt();
			break;
		case '3':
			place_updt();
			break;
		case '4':
			address_updt();
			break;
	}

} /* while */
}

abbrev_updt()
{
	int should_clear = 0;
	int	rec_nbr;

	piu = 4;

	while(1) {
		key_desc(piu);
		intr_allowed = 1;
		key_val = read_field(stdscr,geo_fld,piu,read_buf,&field_entered,&should_clear);
		attrset(0);
		intr_allowed = 0;
		if(saw_intr)  {
			saw_intr = 0;	
			clear_err_line();
			init_flag();
			ab_flags_1 = 0;
			ret_begin = 1; 
			ab_flags_2 = 0;
			prt_hdr();
			return;
		}
	
		ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);

		if(key_val != ESC && (ab_flags_1 || ab_flags_2)) {
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_45, "Enter ESC to execute"));
			continue;
		}
	
		if(key_val == ESC)	{
			clear_err_line();
			if (loop1 == 0)	{
				if (piu == 4)
					rec_nbr = abb[0];
				else if (piu == 5)
					rec_nbr = abb[1];
				else {
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_69, 
						  "No record available for update"));
					continue;
				}
			}
			else	{	/* not first screen */
				if((ab_ctr%2) == 1)	{
					if (piu == 4)
					 	rec_nbr = abb[ab_ctr+1];
					else if (piu == 5)
						rec_nbr = abb[ab_ctr+2];
					else {
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_69, 
						          "No record available for update"));
						continue;
					}
				}
				else	{
					if (piu == 4)
						rec_nbr = abb[ab_ctr];
					else if (piu == 5)
						rec_nbr = abb[ab_ctr+1];
					else {
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_69, 
							  "No record available for update"));
						continue;
					}
				}
			} /* loop1 > 0 */

			temp_sn_ptr->record_nbr = rec_nbr;
			if (db(STRNAME_FILE_ID, READ_KEYS, &sn_key3, ISEQUAL, 0) < 0) {
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_69, "No record available for update"));
				continue;
			}

			sn_ptr->index = temp_sn_ptr->index;
			strcpy(sn_ptr->str_type,temp_sn_ptr->str_type);
			sn_ptr->rec_type = temp_sn_ptr->rec_type;
			sn_ptr->record_nbr = temp_sn_ptr->record_nbr;
			memcpy(req_buf,(char *)sn_ptr,sizeof(struct cisam_sn));

			if((db(STRNAME_FILE_ID,UPDATE_REC,&sn_key1,0,0)) < 0) {
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_62, "Update  error"));
				continue;
			}

			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_63, "Record updated"));
			field_entered = 0;
			ab_flags_1 = 0;
			ab_flags_2 = 0;
			db(STRNAME_FILE_ID,RELEASE,&sn_key1,0,0);
			break;
		} /* ESC */
	} /* while */
}

alias_updt()
{
	int should_clear = 0;
	int	rec_nbr;

	piu = 6;

	while(1) {
		key_desc(piu);
		intr_allowed = 1;
		key_val = read_field(stdscr,geo_fld,piu,read_buf,&field_entered,&should_clear);
		attrset(0);
		intr_allowed = 0;

		if(saw_intr)  {
			saw_intr = 0;	
			clear_err_line();
			init_flag(); 
			al_flags_1 = 0;
			al_flags_2 = 0;
			ret_begin = 1; 
			prt_hdr();
			return;
		}

		ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);

		if(key_val != ESC && (al_flags_1 || al_flags_2)) {
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_45, "Enter ESC to execute"));
			continue;
		}

		if(key_val == ESC) {
			clear_err_line();
			if(loop2 == 0) {
				if (piu == 6) 
					rec_nbr = als[0];
				else if (piu == 7)	
					rec_nbr = als[1];
				else {
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_69, 
						  "No record available for update"));
					continue;
				}
			}
			else {	/* not first screen */
				if((al_ctr%2) == 1)	{
					if(piu == 6) 
						rec_nbr = als[al_ctr + 1];
					else if(piu == 7) 
						rec_nbr = als[al_ctr + 2];
					else {
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_69, 
						  	"No record available for update"));
						continue;
					}
				}
				else	{
					if (piu == 6)
						rec_nbr = als[al_ctr];
					else if (piu == 7)
						rec_nbr = als[al_ctr+1];
					else {
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_69, 
					          	"No record available for update"));
						continue;
					}
				}
			}
		
			temp_sn_ptr->record_nbr = rec_nbr;
			if (db(STRNAME_FILE_ID, READ_KEYS, &sn_key3, ISEQUAL, 0) < 0) {
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_69, "No record available for update"));
				continue;
			}

			sn_ptr->index = temp_sn_ptr->index;
			strcpy(sn_ptr->str_type,temp_sn_ptr->str_type);
			sn_ptr->rec_type = temp_sn_ptr->rec_type;
			sn_ptr->record_nbr = temp_sn_ptr->record_nbr;
			memcpy(req_buf,(char *)sn_ptr,sizeof(struct cisam_sn));

			if((db(STRNAME_FILE_ID,UPDATE_REC,&sn_key1,0,0)) < 0) {
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_62, "Update  error"));
				continue;
			}

			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_63, "Record updated"));
			al_flags_1 = 0;
			al_flags_2 = 0;
			field_entered = 0;
			db(STRNAME_FILE_ID,RELEASE,&sn_key1,0,0);
			break;
		} /* ESC */
	} /* while */
}

place_updt()
{
	int should_clear = 0;

	piu = F_PL_NAME_1;
while(1)	{
	key_desc(piu);
	intr_allowed = 1;
	key_val = read_field(stdscr,geo_fld,piu,read_buf,&field_entered,&should_clear);
	attrset(0);
	intr_allowed = 0;
	if(saw_intr)  {
		saw_intr = 0;	
		clear_err_line();
		init_flag();
		init_pl_flag();
		ret_begin = 1; 
		prt_hdr();
		return;
	}
		if (loop3 == 0)	{
			if ((F_PL_NAME_1 <= piu) && (piu <= F_LOC_1))	
				pl_counter = 0;
			else if ((F_PL_NAME_2 <= piu) && (piu <= F_LOC_2))	
				pl_counter = 1; 
			else if ((F_PL_NAME_3 <= piu) && (piu <= F_LOC_3))	
				pl_counter = 2;
			temp_pl_ptr->record_nbr = plc[pl_counter];
			if (db(PLACENM_FILE_ID, READ_KEYS, &pl_key3, ISEQUAL, 0) < 0) {
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_69,
							  "No record available for update"));
					if (piu <= F_LOC_1)
						prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_69,
								  "No record available for update"));
					else
						--piu;
					continue;
			}

		}
		else	{	/* not first screen */
			if((pl_ctr%3) == 1)	
				pl_counter1 = pl_ctr+2;
			else if((pl_ctr%3) == 2)	{	
				if ((F_PL_NAME_1 <= piu) && (piu <= F_LOC_1))	
					pl_counter1 = pl_ctr+1;
				else if ((F_PL_NAME_2 <= piu) && (piu <= F_LOC_2))	
					pl_counter1 = pl_ctr+2;
			}
			else {
				if ((F_PL_NAME_1 <= piu) && (piu <= F_LOC_1))	
					pl_counter1 = pl_ctr;
				else if ((F_PL_NAME_2 <= piu) && (piu <= F_LOC_2))	
					pl_counter1 = pl_ctr+1;
				else if ((F_PL_NAME_3 <= piu) && (piu <= F_LOC_3))	
					pl_counter1 = pl_ctr+2;
			}

			temp_pl_ptr->record_nbr = plc[pl_counter1];
			if (db(PLACENM_FILE_ID, READ_KEYS, &pl_key3, ISEQUAL, 0) < 0) {
			    prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_61, "Read error"));
			    
			    continue;
			}
		} /* loop3 > 0 */
	ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);
	if ((pl_ptr->str_city[0] == ' ') || (temp_pl_ptr->str_city[0] == ' '))	{
		prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_69, "No record avaliable for update"));
		piu -= 4;
		continue;
	}
	if (ret_val == -1)
		continue;
	if(key_val == ESC)	{
		clear_err_line();
			pl_ptr->record_nbr = temp_pl_ptr->record_nbr;
			pl_ptr->index = temp_pl_ptr->index;
			strcpy(pl_ptr->str_name,temp_pl_ptr->str_name);
			strcpy(pl_ptr->str_type,temp_pl_ptr->str_type);
			if (!pl_name)
				strcpy(pl_ptr->name,temp_pl_ptr->name);
			if (!pl_pre)
				strcpy(pl_ptr->str_pre_dir,temp_pl_ptr->str_pre_dir);
			if (!pl_post)
				strcpy(pl_ptr->str_post_dir,temp_pl_ptr->str_post_dir);
			if (!pl_city)
				strcpy(pl_ptr->str_city,temp_pl_ptr->str_city);
			if (!pl_oe)
				pl_ptr->odd_even = temp_pl_ptr->odd_even;
			if (!pl_street)
				pl_ptr->street_nbr = temp_pl_ptr->street_nbr;
			if (!pl_suf)
				pl_ptr->str_nbr_suf = temp_pl_ptr->str_nbr_suf;
			if (!pl_zip)
				pl_ptr->str_zip = temp_pl_ptr->str_zip;
			if (!pl_comment)
				strcpy(pl_ptr->str_comment,temp_pl_ptr->str_comment);
			if (!pl_loc)
				pl_ptr->local = temp_pl_ptr->local;
			memcpy(req_buf,(char *)pl_ptr,sizeof(struct cisam_pl));
			if((db(PLACENM_FILE_ID,UPDATE_REC,&pl_key1,0,0)) < 0) {
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_62, "Update  error"));
				continue;
			}
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_63, "Record updated"));
			init_pl_flag();
			init_flag();
			init1_flag();
			db(PLACENM_FILE_ID,RELEASE,&pl_key1,0,0);
			break;
		} /* ESC */
	} /* while */

}


address_updt()
{
int   temp_ctr;
int  should_clear = 0;

 ad_pre = ad_post = ad_city = ad_oe = ad_high = ad_low = ad_zip = ad_subzone = ad_comment = ad_x = ad_y = ad_map = ad_pb = 0;
	piu = F_AD_PRE_1;
while(1)	{
	key_desc(piu);
	intr_allowed = 1;
	key_val = read_field(stdscr,geo_fld,piu,read_buf,&field_entered,&should_clear);
	attrset(0);
	intr_allowed = 0;
	if(saw_intr)  {
		saw_intr = 0;	
		clear_err_line();
		init_flag();
		init_ad_flag();
		ret_begin = 1; 
		prt_hdr();
		return;
	}
ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);
	if (ret_val == -1)
		continue;
		if (loop4 == 0)	{
			if ((F_AD_PRE_1 <= piu) && (piu <= F_AD_PB_1))
				ad_counter = 0;
			else if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
				ad_counter = 1;
			else if ((F_AD_PRE_3 <= piu) && (piu <= F_AD_PB_3))
				ad_counter = 2;
			else if ((F_AD_PRE_4 <= piu) && (piu <= F_AD_PB_4))
				ad_counter = 3;
			else if ((F_AD_PRE_5 <= piu) && (piu <= F_AD_PB_5))
				ad_counter = 4;
			else if ((F_AD_PRE_6 <= piu) && (piu <= F_AD_PB_6))	
				ad_counter = 5;
			else if ((F_AD_PRE_7 <= piu) && (piu <= F_AD_PB_7))	
				ad_counter = 6;
			else if ((F_AD_PRE_8 <= piu) && (piu <= F_AD_PB_8))	
				ad_counter = 7;
			else if ((F_AD_PRE_9 <= piu) && (piu <= F_AD_PB_9))	
				ad_counter = 8;

			temp_ad_ptr->record_nbr = adr[ad_counter];
			if (db(ADDRESS_FILE_ID, READ_KEYS, &ad_key3, ISEQUAL, 0) < 0) {
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_69,
						  "No record available for update"));
				if (piu <= F_AD_PB_1)
					prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_70,
							  "No record available for update"));
				else
					--piu;
				continue;
			}
		}
		else	{	/* not first screen */
			temp_ctr = ad_ctr % 9;
			switch(temp_ctr)	{
				case 0:
				  if ((F_AD_PRE_1 <= piu) && (piu <= F_AD_PB_1))
						ad_counter1 = ad_ctr;
				  else if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
						ad_counter1 = ad_ctr+1;
				  else if ((F_AD_PRE_3 <= piu) && (piu <= F_AD_PB_3))
						ad_counter1 = ad_ctr+2;
				  else if ((F_AD_PRE_4 <= piu) && (piu <= F_AD_PB_4))
						ad_counter1 = ad_ctr+3;
				  else if ((F_AD_PRE_5 <= piu) && (piu <= F_AD_PB_5))
						ad_counter1 = ad_ctr+4;
				  else if ((F_AD_PRE_6 <= piu) && (piu <= F_AD_PB_6))	
						ad_counter1 = ad_ctr+5;
				  else if ((F_AD_PRE_7 <= piu) && (piu <= F_AD_PB_7))	
						ad_counter1 = ad_ctr+6;
				  else if ((F_AD_PRE_8 <= piu) && (piu <= F_AD_PB_8))	
						ad_counter1 = ad_ctr+7;
				  else if ((F_AD_PRE_9 <= piu) && (piu <= F_AD_PB_9))	
						ad_counter1 = ad_ctr+8;
					break;
				case 1:
						ad_counter1 = ad_ctr+8;
						break;
				case 2:
				  if ((F_AD_PRE_1 <= piu) && (piu <= F_AD_PB_1))
						ad_counter1 = ad_ctr+7;
				  if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
						ad_counter1 = ad_ctr+8;
					break;
				case 3:
					if ((F_AD_PRE_1 <= piu) && (piu <= F_AD_PB_1))
						ad_counter1 = ad_ctr+6;
					if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
						ad_counter1 = ad_ctr+7;
					if ((F_AD_PRE_3 <= piu) && (piu <= F_AD_PB_3))
						ad_counter1 = ad_ctr+8;
					break;
				case 4:
					if ((F_AD_PRE_1 <= piu) && (piu <= F_AD_PB_1))
						ad_counter1 = ad_ctr+5;
					if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
						ad_counter1 = ad_ctr+6;
					if ((F_AD_PRE_3 <= piu) && (piu <= F_AD_PB_3))
						ad_counter1 = ad_ctr+7;
					if ((F_AD_PRE_4 <= piu) && (piu <= F_AD_PB_4))
						ad_counter1 = ad_ctr+8;
					break;
				case 5:
					if ((F_AD_PRE_1 <= piu) && (piu <= F_AD_PB_1))
						ad_counter1 = ad_ctr+4;
					if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
						ad_counter1 = ad_ctr+5;
					if ((F_AD_PRE_3 <= piu) && (piu <= F_AD_PB_3))
						ad_counter1 = ad_ctr+6;
					if ((F_AD_PRE_4 <= piu) && (piu <= F_AD_PB_4))
						ad_counter1 = ad_ctr+7;
					if ((F_AD_PRE_5 <= piu) && (piu <= F_AD_PB_5))
						ad_counter1 = ad_ctr+8;
					break;
				case 6:
					if ((F_AD_PRE_1 <= piu) && (piu <= F_AD_PB_1))
						ad_counter1 = ad_ctr+3;
					if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
						ad_counter1 = ad_ctr+4;
					if ((F_AD_PRE_3 <= piu) && (piu <= F_AD_PB_3))
						ad_counter1 = ad_ctr+5;
					if ((F_AD_PRE_4 <= piu) && (piu <= F_AD_PB_4))
						ad_counter1 = ad_ctr+6;
					if ((F_AD_PRE_5 <= piu) && (piu <= F_AD_PB_5))
						ad_counter1 = ad_ctr+7;
					if ((F_AD_PRE_6 <= piu) && (piu <= F_AD_PB_6))	
						ad_counter1 = ad_ctr+8;
					break;
				case 7:
					if ((F_AD_PRE_1 <= piu) && (piu <= F_AD_PB_1))
						ad_counter1 = ad_ctr+2;
					if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
						ad_counter1 = ad_ctr+3;
					if ((F_AD_PRE_3 <= piu) && (piu <= F_AD_PB_3))
						ad_counter1 = ad_ctr+4;
					if ((F_AD_PRE_4 <= piu) && (piu <= F_AD_PB_4))
						ad_counter1 = ad_ctr+5;
					if ((F_AD_PRE_5 <= piu) && (piu <= F_AD_PB_5))
						ad_counter1 = ad_ctr+6;
					if ((F_AD_PRE_6 <= piu) && (piu <= F_AD_PB_6))	
						ad_counter1 = ad_ctr+7;
					if ((F_AD_PRE_7 <= piu) && (piu <= F_AD_PB_7))	
						ad_counter1 = ad_ctr+8;
					break;
				case 8:
					if ((F_AD_PRE_1 <= piu) && (piu <= F_AD_PB_1))
						ad_counter1 = ad_ctr+1;
					if ((F_AD_PRE_2 <= piu) && (piu <= F_AD_PB_2))	
						ad_counter1 = ad_ctr+2;
					if ((F_AD_PRE_3 <= piu) && (piu <= F_AD_PB_3))
						ad_counter1 = ad_ctr+3;
					if ((F_AD_PRE_4 <= piu) && (piu <= F_AD_PB_4))
						ad_counter1 = ad_ctr+4;
					if ((F_AD_PRE_5 <= piu) && (piu <= F_AD_PB_5))
						ad_counter1 = ad_ctr+5;
					if ((F_AD_PRE_6 <= piu) && (piu <= F_AD_PB_6))	
						ad_counter1 = ad_ctr+6;
					if ((F_AD_PRE_7 <= piu) && (piu <= F_AD_PB_7))	
						ad_counter1 = ad_ctr+7;
					if ((F_AD_PRE_8 <= piu) && (piu <= F_AD_PB_8))	
						ad_counter1 = ad_ctr+8;
					break;
			}

			temp_ad_ptr->record_nbr = adr[ad_counter1];
			if (db(ADDRESS_FILE_ID, READ_KEYS, &ad_key3, ISEQUAL, 0) < 0) {
			    prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_61, "Read error"));
			    --piu;
			    continue;
			}

		} /* loop4 > 0 */
	if(key_val == ESC)	{
		clear_err_line();
			ad_ptr->record_nbr = temp_ad_ptr->record_nbr;
			ad_ptr->str_index = temp_ad_ptr->str_index;
			strcpy(ad_ptr->str_name,temp_ad_ptr->str_name);
			strcpy(ad_ptr->str_type,temp_ad_ptr->str_type);
			if (!ad_pre)
				strcpy(ad_ptr->str_pre_dir,temp_ad_ptr->str_pre_dir);
			if (!ad_post)
				strcpy(ad_ptr->str_post_dir,temp_ad_ptr->str_post_dir);
			if (!ad_city)
				strcpy(ad_ptr->str_city,temp_ad_ptr->str_city);
			if (!ad_oe)
				ad_ptr->odd_even = temp_ad_ptr->odd_even;
			if (!ad_high)
				ad_ptr->high_nbr = temp_ad_ptr->high_nbr;
			if (!ad_low)
				ad_ptr->low_nbr = temp_ad_ptr->low_nbr;
			if (!ad_zip)
				ad_ptr->str_zip = temp_ad_ptr->str_zip;
			if (!ad_subzone)
				strcpy(ad_ptr->str_subzone,temp_ad_ptr->str_subzone);
			if (!ad_comment)
				strcpy(ad_ptr->str_comment,temp_ad_ptr->str_comment);
			if (!ad_x)
				ad_ptr->str_x_coord = temp_ad_ptr->str_x_coord;
			if (!ad_y)
				ad_ptr->str_y_coord = temp_ad_ptr->str_y_coord;
			if (!ad_map)
				ad_ptr->map_ref_page = temp_ad_ptr->map_ref_page;
			if (!ad_pb)
				strcpy(ad_ptr->map_ref_block,temp_ad_ptr->map_ref_block);
                        if ( temp_ad_ptr->ct_comment != NULL )
                                strcpy(ad_ptr->ct_comment, temp_ad_ptr->ct_comment);
			memcpy(req_buf,(char *)ad_ptr,sizeof(struct cisam_ad));
			if((db(ADDRESS_FILE_ID,UPDATE_REC,&ad_key1,0,0)) < 0) {
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_62, "Update  error"));
				continue;
			}
			prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_63, "Record updated"));
			init_ad_flag();
			init_flag();
			ad_pre = 0;
			ad_post = 0;
			ad_city = 0;
			ad_oe = 0;
			ad_high = 0;
			ad_low = 0;
			ad_zip = 0;
			ad_subzone = 0;
			ad_comment = 0;
			ad_x = 0;
			ad_y = 0;
			ad_map = 0;
			ad_pb = 0;
			db(ADDRESS_FILE_ID,RELEASE,&ad_key1,0,0);
			break;
		} /* ESC */
	} /* while */


}
init_flag()
{
	pre_flag = 1;
	post_flag = 1;
	city_flag = 1;
	oe_flag = 1;
	nbr_flag = 1;
	suf_flag = 1;
	zip_flag = 1;
	subzone_flag =  1;
	x_flag = 1;
	y_flag = 1;
	map_flag = 1;
	loc_flag = 1;
	high_flag = 1;
	low_flag = 1;
}

init_ad_flag()
{
	ad_flags_1 = 0;
	ad_flags_2 = 0;
	ad_flags_3 = 0;
	ad_flags_4 = 0;
	ad_flags_5 = 0;
	ad_flags_6 = 0;
	ad_flags_7 = 0;
	ad_flags_8 = 0;
	ad_flags_9 = 0;

}

init_pl_flag()
{
	pl_flags_1 = 0;
	pl_flags_2 = 0;
	pl_flags_3 = 0;
}

init1_flag()
{
	pl_name = 0;
	pl_pre = 0;
	pl_post = 0;
	pl_city = 0;
	pl_oe = 0;
	pl_street = 0;
	pl_suf = 0;
	pl_zip = 0;
	pl_subzone = 0;
	pl_comment = 0;
	pl_x = 0;
	pl_y = 0;
	pl_map = 0;
	pl_pb = 0;
	pl_loc = 0;
}

ch_option1()
{
    while(1) {
	c = getkey();
	if (c == '0' || c == '1' || c == '2' ||
	    c == '3' || c == '4' || c == '5')
	    return(c);
    }
}
		

