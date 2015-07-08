static char sz__FILE__[]="@(#)zn_sub1.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  zn_sub1.c; Rel: 6.8.0.0; Get date: 4/9/92 at 09:25:04
* @(#)  Last delta: 3/20/92 at 11:03:33
* @(#)  SCCS File: /taxi/sccs/s.zn_sub1.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <isam.h>
#include <cursesX.h>
#include <signal.h>
#include <memory.h>

#include "zn_m_strdefs.h"
#include "mad_error.h"
#include "mads_types.h"
#include "df_maint.h"
#include "mads_isam.h"
#include "ex_key.h"
#include "zn_maint.h"
#include "zn_msg.h"
#include "tokenize.h"
#include "language.h"

int    num_ber;
extern int upper_case;
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
extern int key_desc();
extern nbr_flag;
extern fl_flag;
extern char mode[];		/* user specified Append or Create */	
extern char buf[];			
extern char req_buf[];			
extern struct scr_flds zone_scr_fld[];
extern struct cisam_zn *zn_ptr; /* pointer to zone file structure */
extern struct cisam_zn *temp_zn_ptr; /* pointer to seal file structure */
extern struct cisam_zn zn_buf; /* pointer to seal file structure */
extern int clear_flds();	/* clear data fields on the screen */
extern int doing_add;		/* performing add operation */
extern int    range_high, range_low;
extern struct cisam_zn global_zone_rec;
int updated_zones[100];
extern int scr_nbr;		/* screen number */

/******************************************************************************
* process_query - search for specific record in the zone file
*******************************************************************************/

process_query()
{
	int i = 0;				/* loop index */
	int should_clear = 0;
#ifdef DEBUG
	sprintf(trace_str,"Entering process_query\n");
	TRACE(trace_str);
#endif

	clear_err_line();

        if(scr_nbr == 2)  {
          clear();
          scr_nbr = 1;
          prt_labels();
	}
	else
          clear_flds();

	prt_query_hdr();		/* Print the header for the query 
					   screen */
	/* Initial search values */
	zn_ptr->fleet = ' ';
	zn_ptr->nbr = 0;
	piu = F_FLEET;

	range_low = range_high = 0;
	clear_zn_range();	
	/*  Process users input of key field(fleet,nbr).  */

	while(1)  {
		key_desc(piu);
		intr_allowed = 1;		/* interrupts allowed */
		key_val = read_field(stdscr,zone_scr_fld,piu,read_buf,&field_entered,&should_clear);
		attrset(0);
		if(saw_intr)  {			/* key_value = interrupt char */
			saw_intr = 0;		/* interrup has been processed */
			cur_list = 0;		/* no row in current list */
			clear_flds();
			clear_err_line();
			prt_hdr();
			return;
		}
		intr_allowed = 0;		/* interrupts not allowed */

		if(saw_intr)  {			/* key_value = interrupt char */
			saw_intr = 0;		/* interrup has been processed */
			cur_list = 0;		/* no row in current list */
			clear_flds();
			clear_err_line();
			prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_query - 1\n");
	TRACE(trace_str);
#endif
			return;
		}

		/* validate input data, store it in c-isam structure, and
		   adjust the field pointer */

		ret_val = check_field[piu](&piu,read_buf,key_val,&field_entered);
                if (!ret_val) /* error returned...user must correct before proceding */
                   continue;

		/* Must enter a fleet value */
		if (key_val != ESC && zn_ptr->fleet == ' ')	{	
			prt_error(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_5, "You must enter value"));
			piu = F_FLEET;
			continue;
		}

		/* Only piu's of FLEET and ZONE allowed */
		if ((piu == F_DESC) && (key_val != ESC)) 	{
			piu = F_FLEET;
			prt_error(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_6, "Enter ESC to execute"));
		}
		else if (piu == 0)
		  piu = F_ZONE;
	
		

		if(key_val == ESC)   {
			if ((!fl_flag) && (!nbr_flag))	{
				zn_ptr->fleet = 'A';
				zn_ptr->nbr = 0;
			}
				

		/*  Using the key entered in previous step, call C-Isam routine
	    	    to retrieve specific record.  */
			clear_err_line();
			memcpy(req_buf,(char *)zn_ptr,sizeof(struct cisam_zn)); 
			if((db(ZONE_FILE_ID,READ_KEYS,&zn_key1,ISGTEQ,0)) < 0)  {
				if(iserrno == ENOREC)  {
					prt_error(NO_ROWS);
					clear_flds();
					cur_list = 0;	/* row in current list is invalid */
					prt_hdr();

            
                                        
					return;
				}
				else  {
					prt_error(READ_ZONE_ERR);
					sleep(3);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_query - 3\n");
	TRACE(trace_str);
#endif
					cleanup();
				}
			}
                        
                        if ( range_low > 0 ) // load up the global zone rec
                          memcpy( &global_zone_rec, (char *)zn_ptr, sizeof( struct cisam_zn ) );
                        
			record_nbr = isrecnum;	/* save record number */
			cur_list = 1;		/* valid row in current list */
			memcpy((char *)zn_ptr,req_buf,sizeof(struct cisam_zn)); 
        		prt_data();  		
			prt_hdr();
			break;
		}
	}

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_query - 4\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of process_query()  */

/******************************************************************************
* process_next - read the next record in the user zone file and display it.
*******************************************************************************/

process_next()
{

#ifdef DEBUG
	sprintf(trace_str,"Entering process_next\n");
	TRACE(trace_str);
#endif

	clear_err_line();
	if(!cur_list)  {
		prt_error(NO_CUR_LIST);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_next - 1\n");
	TRACE(trace_str);
#endif
		return;
	}

	/*  Call the C-Isam routine to get the next entry.   */
	if ( range_low > 0 )
	  {
	    temp_zn_ptr = ( struct cisam_zn * ) req_buf;
	    temp_zn_ptr->nbr = zn_ptr->nbr + 1;
	    if ( temp_zn_ptr->nbr > (short)range_high )
	      {
		prt_error(NO_ROWS_DIR);
		return;
	      }
	    if ((db(ZONE_FILE_ID, READ_KEYS, &zn_key1, ISGTEQ, 0) ) < 0 )
	      {
		prt_error(NO_ROWS_DIR);
		return;
	      }
	    if ( ( temp_zn_ptr->nbr > (short)range_high ) ||
		 ( temp_zn_ptr->nbr == zn_ptr->nbr ) )
	      {
		prt_error(NO_ROWS_DIR);
		return;
	      }
	  }
	else
	  {
	    if((db(ZONE_FILE_ID,READ_KEYS,&zn_key1,ISNEXT,0)) < 0)  {
	      if(iserrno == EENDFILE)  {
		prt_error(NO_ROWS_DIR);
		return;
	      }
	      else  {
		prt_error(RD_NXT_ZONE_ERR);
		sleep(3);
		cleanup();
	      }
	    }
	  }
	
	memcpy((char *)zn_ptr,req_buf,sizeof(struct cisam_zn)); 
	record_nbr = isrecnum;	/* save the record number for future read */
	clear_flds();
	cur_list = 1;
        prt_data();  		/* Print retrieved data on the screen. */

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_next - 4\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of process_next()  */

/******************************************************************************
* process_previous - read the previous record in the zone file and display it.
*******************************************************************************/

process_previous()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering process_previous\n");
	TRACE(trace_str);
#endif

	clear_err_line();
	if(!cur_list)  {
		prt_error(NO_CUR_LIST);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_previous - 1\n");
	TRACE(trace_str);
#endif
		return;
	}

	/*  Call the C-Isam routine to get the previous entry.  */
	if ( range_low > 0 )
	  {
	    temp_zn_ptr = ( struct cisam_zn * ) req_buf;
	    temp_zn_ptr->nbr = zn_ptr->nbr - 1;
	    if ( temp_zn_ptr->nbr < (short)range_low )
	      {
		prt_error(NO_ROWS_DIR);
		return;
	      }
	    if ((db(ZONE_FILE_ID, READ_KEYS, &zn_key1, ISPREV, 0) ) < 0 )
	      {
		prt_error(NO_ROWS_DIR);
		return;
	      }
	    if ( ( temp_zn_ptr->nbr < (short)range_low ) ||
		 ( temp_zn_ptr->nbr == zn_ptr->nbr ) )
	      {
		prt_error(NO_ROWS_DIR);
		return;
	      }
	  }
	else
	  {
	    if((db(ZONE_FILE_ID,READ_KEYS,&zn_key1,ISPREV,0)) < 0)  {
	      if(iserrno == EENDFILE)  {
		prt_error(NO_ROWS_DIR);
		
		return;
	      }
	      else  {
		prt_error(RD_PREV_ZONE_ERR);
		sleep(3);
		
		cleanup();
	      }
	    }
	  }
	memcpy((char *)zn_ptr,req_buf,sizeof(struct cisam_zn));
	record_nbr = isrecnum;	/* save the record number for future read */
	clear_flds();
	cur_list = 1;
        prt_data();  		/* Print retrieved data on the screen. */

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_previous - 4\n");
	TRACE(trace_str);
#endif
 	return;

}  /* End of process_previous()  */

/******************************************************************************
* process_add - input data fields and add the new record to the zone file 
*******************************************************************************/

process_add()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering process_add\n");
	TRACE(trace_str);
#endif
	clear_err_line();

	if(scr_nbr == 2)  {
		clear();
		scr_nbr = 1;
		prt_labels();
	}
	else
          clear_flds();        


	/* reset all the members of the zone file structure */


	memset((char *)zn_ptr,'\0',sizeof(struct cisam_zn));
	store_def();		/* default  'N' */

	piu = F_FLEET;		/* start with the fleet id */
	scr_nbr = 1;		/* on the first screen */        
	prt_add_hdr();		/* Print the header line for the the add screen.  */
	prt_labels();		/* print the screen labels  */
	prt_data();
	doing_add = 1;
	process_input();	/* Process each field as input by the user */
	doing_add = 0;
	if(saw_intr)  {
		saw_intr = 0; 	/* interrupt has been processed */
		cur_list = 0;	/* no valid row in current list */
		clear_flds();
		clear_err_line();
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_add - 1\n");
	TRACE(trace_str);
#endif
		return;
	}
	
	memcpy(req_buf,(char *)zn_ptr,sizeof(struct cisam_zn));

	if ( zn_ptr->nbr == 0 )
	  return;
	
	/*  Call the C-Isam add record routine.  */
	if((db(ZONE_FILE_ID,ADD_REC,&zn_key1,0,0)) < 0)  {
		prt_error(ADD_ZONE_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_add - 2\n");
	TRACE(trace_str);
#endif
		prt_hdr();		/* Display the user profile file maintenance screen header line  */
		return;
	}
	prt_error(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_7, "Record added"));
	record_nbr = isrecnum;
	cur_list = 1;		/* valid row in current list */
	prt_hdr();		/* Display the user profile file maintenance screen header line  */

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_add - 3\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of process_add()  */

/******************************************************************************
* process_update - input data fields and update the zone file record
*******************************************************************************/

process_update()
{
  int i,k;
  struct cisam_zn  new_zone_rec;
  
  clear_err_line();
  if ( range_low > 0 )
    piu = F_TYPE;
  else
    piu = F_FLEET;	

  while(1)
    {
      if(!cur_list)
	{
	  prt_error(NO_CUR_LIST);
	  
	  return;
	}

      if ( range_low > 0 )
        memcpy( &global_zone_rec, (char *)zn_ptr, sizeof( struct cisam_zn ) );

      if(scr_nbr == 2)  {
        clear();
        scr_nbr = 1;
        prt_labels();
        prt_data();
      }
      
      prt_upd_hdr();		/* Print the header line for the update screen.  */
      
      process_input();	/* Process each field as input by the user */
      if(saw_intr)
	{
	  saw_intr = 0;	/* interrupt has been processed */
	  cur_list = 0;	/* no valid row in current list */
	  clear_flds();
	  clear_err_line();
	  prt_hdr();
	  
	  return;
	}
      
      if ( range_low > 0 )
	{
          memcpy( &new_zone_rec, (char *)zn_ptr, sizeof( struct cisam_zn ) );
          if ( new_zone_rec.break_duration == global_zone_rec.break_duration )
            new_zone_rec.break_duration = -1;
          if (new_zone_rec.type == global_zone_rec.type )
            new_zone_rec.type = -1;
          if ( new_zone_rec.superzn_detail == global_zone_rec.superzn_detail )
            new_zone_rec.superzn_detail = -1;
          if ( new_zone_rec.superzn_nbr == global_zone_rec.superzn_nbr )
            new_zone_rec.superzn_nbr = -1;
          if ( new_zone_rec.mdt_screen == global_zone_rec.mdt_screen )
            new_zone_rec.mdt_screen = -1;
          if ( new_zone_rec.max_time_calls == global_zone_rec.max_time_calls )
            new_zone_rec.max_time_calls = -1;
          if ( new_zone_rec.avl_call_q_time == global_zone_rec.avl_call_q_time )
            new_zone_rec.avl_call_q_time = -1;
          if ( new_zone_rec.pckup_lead_time == global_zone_rec.pckup_lead_time )
            new_zone_rec.pckup_lead_time = -1;
          if ( new_zone_rec.stand_time_from == global_zone_rec.stand_time_from )
            new_zone_rec.stand_time_from = -1;
          if ( new_zone_rec.stand_time_to == global_zone_rec.stand_time_to )
            new_zone_rec.stand_time_to = -1;
          if ( new_zone_rec.enr_post_allow == global_zone_rec.enr_post_allow )
            new_zone_rec.enr_post_allow = -1;
          if ( new_zone_rec.enr_post_lead == global_zone_rec.enr_post_lead )
            new_zone_rec.enr_post_lead = -1;
          if ( new_zone_rec.accept_duration == global_zone_rec.accept_duration )
            new_zone_rec.accept_duration = -1;
          if ( new_zone_rec.check_avail_taxis == global_zone_rec.check_avail_taxis )
            new_zone_rec.check_avail_taxis = -1;
          if ( new_zone_rec.match_algorithm == global_zone_rec.match_algorithm )
            new_zone_rec.match_algorithm = -1;
          if ( new_zone_rec.tc_look_ahead == global_zone_rec.tc_look_ahead )
            new_zone_rec.tc_look_ahead = -1;
          if ( new_zone_rec.tc_restrict_interval == global_zone_rec.tc_restrict_interval )
            new_zone_rec.tc_restrict_interval = -1;
          if ( new_zone_rec.ns_short == global_zone_rec.ns_short )
            new_zone_rec.ns_short = -1;

	  bzero( updated_zones, sizeof(updated_zones) );
	  k = 0;
	  for ( i = range_low; i <= range_high; i++ )
	    {
	      temp_zn_ptr = ( struct cisam_zn * )req_buf;
	      temp_zn_ptr->fleet = zn_ptr->fleet;
	      temp_zn_ptr->nbr = i;

	      // Get record for the range
	      if((db(ZONE_FILE_ID,READ_KEYS,&zn_key1,ISEQUAL+ISLOCK,0)) < 0)
		continue;
	      
	      // Update values from the master record
	      updated_zones[k++] = temp_zn_ptr->nbr;

              if ( new_zone_rec.break_duration != -1 )
                temp_zn_ptr->break_duration = new_zone_rec.break_duration;
              if (new_zone_rec.type != -1 )
                temp_zn_ptr->type = new_zone_rec.type;
              if ( new_zone_rec.superzn_detail != -1 )
                temp_zn_ptr->superzn_detail = new_zone_rec.superzn_detail;
              if ( new_zone_rec.superzn_nbr != -1 )
                temp_zn_ptr->superzn_nbr = new_zone_rec.superzn_nbr;
              if ( new_zone_rec.mdt_screen != -1 )
                temp_zn_ptr->mdt_screen = new_zone_rec.mdt_screen;
              if ( new_zone_rec.max_time_calls != -1 )
                temp_zn_ptr->max_time_calls = new_zone_rec.max_time_calls;
              if ( new_zone_rec.avl_call_q_time != -1 )
                temp_zn_ptr->avl_call_q_time = new_zone_rec.avl_call_q_time;
              if ( new_zone_rec.pckup_lead_time != -1 )
                temp_zn_ptr->pckup_lead_time = new_zone_rec.pckup_lead_time;
              if ( new_zone_rec.stand_time_from != -1 )
                temp_zn_ptr->stand_time_from = new_zone_rec.stand_time_from;
              if ( new_zone_rec.stand_time_to != -1 )
                temp_zn_ptr->stand_time_to = new_zone_rec.stand_time_to;
              if ( new_zone_rec.enr_post_allow != -1 )
                temp_zn_ptr->enr_post_allow = new_zone_rec.enr_post_allow;
              if ( new_zone_rec.enr_post_lead != -1 )
                temp_zn_ptr->enr_post_lead = new_zone_rec.enr_post_lead;
              if ( new_zone_rec.accept_duration != -1 )
                temp_zn_ptr->accept_duration = new_zone_rec.accept_duration;
              if ( new_zone_rec.check_avail_taxis != -1 )
                temp_zn_ptr->check_avail_taxis = new_zone_rec.check_avail_taxis;
              if ( new_zone_rec.match_algorithm != -1 )
                temp_zn_ptr->match_algorithm = new_zone_rec.match_algorithm;
              if ( new_zone_rec.tc_look_ahead != -1 )
                temp_zn_ptr->tc_look_ahead = new_zone_rec.tc_look_ahead;
              if ( new_zone_rec.tc_restrict_interval != -1 )
                temp_zn_ptr->tc_restrict_interval = new_zone_rec.tc_restrict_interval;
              if ( new_zone_rec.ns_short != -1 )
                temp_zn_ptr->ns_short = new_zone_rec.ns_short;

	      if ( (db(ZONE_FILE_ID,UPDATE_REC,&zn_key1,0,0)) < 0 )
		continue;
              
              db(ZONE_FILE_ID, RELEASE, &zn_key1, 0, 0);
	    }
	  prt_error(REC_UPDATED);
	  record_nbr = isrecnum;
	  cur_list = 1;		
	  prt_hdr();	
  	  return;
	}
      else
	{
	  
	  if((db(ZONE_FILE_ID,READ_NUM,&zn_key1,ISEQUAL,record_nbr)) < 0)
	    {
	      prt_error(LOCK_ZONE_ERR);
	      prt_hdr();
	      return;
	    }
	  if((db(ZONE_FILE_ID,READ_KEYS,&zn_key1,ISEQUAL+ISLOCK,0)) < 0)
	    {
	      prt_error(LOCK_ZONE_ERR);
	      prt_hdr();
	      return;
	    }
	  /*  Call the C-Isam update record routine.  */
	  memcpy(req_buf,(char *)zn_ptr,sizeof(struct cisam_zn));   
	  if((db(ZONE_FILE_ID,UPDATE_REC,&zn_key1,0,0)) < 0)
	    {
	      prt_error(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_8, "Duplicated index found. please enter again! "));
	      continue;
	    }
	  prt_error(REC_UPDATED);	      
	  if((db(ZONE_FILE_ID,RELEASE,&zn_key1,0,0)) < 0)
	    {
	      prt_error(RLS_ZONE_ERR);
	      cur_list = 0;
	      prt_hdr();
	      return;		  
	    }
	  if((db(ZONE_FILE_ID,UNLOCK,&zn_key1,0,0)) < 0)
	    {
	      prt_error(RLS_ZONE_ERR);
	      cur_list = 0;
	      prt_hdr();
	      return;
	    }	  
	}

      record_nbr = isrecnum;
      cur_list = 1;		/* valid row in current list */
      prt_hdr();		/* Display the user profile file maintenance screen header line */
  
      return;
    }
}


/******************************************************************************
* process_remove - remove the current zone file record.
*******************************************************************************/

process_remove()
{

	int i;				/* loop index */
	char answer;			/* response from user */

#ifdef DEBUG
	sprintf(trace_str,"Entering process_remove\n");
	TRACE(trace_str);
#endif

	clear_err_line();
	if(!cur_list)  {
		prt_error(NO_CUR_LIST);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_remove - 1\n");
	TRACE(trace_str);
#endif
		return;
	}

	answer = prompt_confirm();	/* make sure the user wants to remove this record */

	if(answer == NO) {
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_remove - 2\n");
	TRACE(trace_str);
#endif
		return;
	}

	/*  If the user responded affirmatively, call C-Isam routine 
	    to delete the record.  */

	if((db(ZONE_FILE_ID,DELETE,&zn_key1,0,0)) < 0)  {
		prt_error(DEL_ZONE_ERR);
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_remove - 3\n");
	TRACE(trace_str);
#endif
		return;
	}
	cur_list = 0;		/* no ro in current list */
	clear_flds();
	prt_hdr();

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_remove - 4\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of process_remove()  */

/******************************************************************************
* process_output - open an output file and call prt_scr.
*******************************************************************************/

process_output()
{

	FILE *o_file_fp;		/* output file pointer */
	FILE *fopen();

#ifdef DEBUG
	sprintf(trace_str,"Entering process_output\n");
	TRACE(trace_str);
#endif

	intr_allowed = 1;		/* interrupts are allowed */
	clear_err_line();
	echo();
	prompt_fname();			/* prompt the user for the output file name */
	if(saw_intr)  {
		noecho();
		saw_intr = 0;		/* interrupt was processed */
		intr_allowed = 0; 	/* interrupts not allowed */
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_output - 1\n");
	TRACE(trace_str);
#endif
		return;
	}

	prompt_mode();		/* Ask the user: Append to existing file or Create a
					   new one?  */

	noecho();
	if(saw_intr)  {
		saw_intr = 0;		/* interrupt was processed */
		intr_allowed = 0; 	/* interrupts not allowed */
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_output - 2\n");
	TRACE(trace_str);
#endif
		return;
	}

	if((o_file_fp = fopen(o_file,mode)) == NULL)  {
		prt_error(OPEN_OUT_ERR);
		prt_hdr();
#ifdef DEBUG
	sprintf(trace_str,"Leaving process_output - 3\n");
	TRACE(trace_str);
#endif
		return;
	}

        prt_scr(o_file_fp);		/*  Write the screen information to the file.  */
	fclose(o_file_fp);
	prt_hdr();

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_output - 4\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of process_output() */

/*****************************************************************************
* store_def - store default values for the fleet/menu permission arrays.
****************************************************************************/

store_def()
{
	int i,j;	

#ifdef DEBUG
	sprintf(trace_str,"Entering store_def\n");
	TRACE(trace_str);
#endif
	zn_ptr->type = '2';
	zn_ptr->superzn_detail = NO;
	zn_ptr->bid_allowed = NO;
	zn_ptr->max_time_calls = 0;
	zn_ptr->enr_post_allow = NO;
	for(i = 0; i < 16 ; i++)	{
		zn_ptr->pckup_seals[i] = NO;
	}
#ifdef DEBUG
	sprintf(trace_str,"Leaving store_def\n");
	TRACE(trace_str);
#endif
	return;
}  /* End of store_def()  */


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
		case F_FLEET:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_9, " Fleet Indicator "));
			break;

		case F_ZONE:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_10,
					 " Zone number specific to this fleet [1 - 999, super zone 0 - 32,000] "));
			break;

		case F_DESC:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_11,
					 " English description for this zone "));
			break;

		case F_TYPE:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_12,
					 " Type of zone.[1 = stand, 2 = zone, 3 = superzone, 4 = backup zone] "));
			break;

		case F_SPZ_DETL:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_13,
					 " Y- Create description. N- Print the english description "));
			break;

		case F_SPZ_NBR:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_14, " Superzone number. [0 - 32,000] "));
			break;

		case F_BID_ALW:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_15, " Y/N "));
			break;

		case F_MAX_TIME_CALLS:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_65,
					 " Number of time calls allowed for a single time slot (0 for unrestricted)"));
			break;

	case F_TC_LOOK_AHEAD:
	    prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_85," How far into the future should we go to select time calls.(number of 5 minute slots)"));
	    break;

	case F_NS_SHORT:
	    prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_87," Minimum number of minutes required for no show request"));
	    break;

	case F_CBOOK_TIME:
	  prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_89, " Age of trip before it is available for conditional book-in"));
	  break;

	    case F_TC_REST_INT:
		prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_86," Time interval required between time calls (when restricted)"));
		break;

		case F_MX_DSP_TM:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_16,
					 " Number of minutes a call remains in dispatch "));
			break;

		case F_PKUP_L_TM:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_17,
					 " Number of minutes before due time of time calls "));
			break;

		      case F_X_PKUP_L_TM:
			prt_desc(" Number of minutes before due time of time calls (type X)");
			break;			

		case F_ST_TM_FRM:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_18,
					 " Beginning time. [24 hours format : 0 to 2359 ] "));
			break;

		case F_ST_TM_TO:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_19,
					 " Ending time. [24 hours format : 0 to 2359 ] "));
			break;

		case F_EP_ALWD:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_20,
					 " Whether enroute posting is allowed into this zone.[Y/N] "));
			break;
		
		case F_EBOOK_LEAD_TM:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_90,
					 " Maximum allowable time between EBOOK and actual BOOK. (In Minutes) "));
			break;

		case F_MDT_SCREEN:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_68,
					 " The MDT query screen the zone appears on. [1, 2, or 3]"));
			break;

		case F_BREAK_DURATION:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_73,
					 " Maximum allowable break time. (In Minutes) "));
			break;

		case F_ACPT_DURATION:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_74,
					 " Number of seconds given a driver to press the ACPT key. (In Seconds) "));
			break;

		      case F_CHECK_TAXIS:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_76,
					 " Check for available taxis in zone prior to call entry (Y/N) " ) );
			break;

		case F_P_SL_1:
		case F_P_SL_2:
		case F_P_SL_3:
		case F_P_SL_4:
		case F_P_SL_5:
		case F_P_SL_6:
		case F_P_SL_7:
		case F_P_SL_8:
		case F_P_SL_9:
		case F_P_SL_10:
		case F_P_SL_11:
		case F_P_SL_12:
		case F_P_SL_13:
		case F_P_SL_14:
		case F_P_SL_15:
		case F_P_SL_16:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_21,
					 " Indicated seals required in this zone. [Y/N] "));
			break;
		case F_L1_ALG:
		case F_L2_ALG:
		case F_L3_ALG:
		case F_L4_ALG:
		case F_L5_ALG:
		case F_L6_ALG:
		case F_L7_ALG:
		case F_L8_ALG:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_22,
					 " 0 - Default to fleet. 1 - Temporal by zone type. 2 - Seq by zone,temporal by cab. 3 - Seq by zone, cab queue order "));
			break;

		case F_L1_ETM:
		case F_PRM_ETM:
		case F_L2_ETM:
		case F_L3_ETM:
		case F_L4_ETM:
		case F_L5_ETM:
		case F_L6_ETM:
		case F_L7_ETM:
		case F_L8_ETM:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_23, " Number of minutes defines the interval "));
			break;

		case F_L1_RPN:
		case F_PRM_RPN:
		case F_L2_RPN:
		case F_L3_RPN:
		case F_L4_RPN:
		case F_L5_RPN:
		case F_L6_RPN:
		case F_L7_RPN:
		case F_L8_RPN:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_24,
					 " Reject penalty: 0 - Default by fleet. 1 - Not allowed. 2 - No penalty. 3 - bottom of queue "));
			break;

		      case F_GPS_LAT:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_78,
					 "Y coordinate for zone center on map" ) );
			break;

		      case F_GPS_LONG:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_79,
					 "X coordinate for zone center on map" ) );
			break;

		      case F_GPS_RADIUS:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_80,
					 "Radius of circle representing zone on map" ) );
			break;

		      case F_GPS_DIST_CEILING:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_81,
					 "Customer radius for algorithm calculations (meters)"));
			break;

#ifdef NEW_4_5
        case F_GPS_EMERG_RANGE:
          prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_89,
                           "Radius around an emergency taxi to broadcast emergency taxi's location(meters)"));
			break;
#endif

		      case F_MATCH_ALGORITHM:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_22,
					 "3 - Zone Queueing Method 4 - GPS Closest Taxi in Primary and Backup 5 - Zone Queueing method in Primary, Closest Taxi in Backup"));
			break;

        case F_STAND_TELE:
          prt_desc("Taxi stand telephone number");
          break;

	case F_ROTI:
	  prt_desc("Additional trip offer timer for taxis in ROTI state");
	  break;

	case F_STAND_NBR:
	  prt_desc("Taxi stand number for this zone");
	  break;
	  
        case F_HTTP_TIMER:
          prt_desc("Expiration timer for HTTP request");
          break;

	case F_AUTO_INQRY_1:
	case F_AUTO_INQRY_2:
	case F_AUTO_INQRY_3:
	case F_AUTO_INQRY_4:
	case F_AUTO_INQRY_5:
	case F_AUTO_INQRY_6:
	case F_AUTO_INQRY_7:
	case F_AUTO_INQRY_8:
	case F_AUTO_INQRY_9:
	case F_AUTO_INQRY_10:
	  prt_desc("Automatic zone inquiry backup zone");
	  break;
	  

		default:
			prt_desc(catgets(ZN_MAINT_m_catd, ZN_MAINT_SET, ZN_MAINT_25,
					 " Zone backup from fleet ID and zone number "));
			break;
	}
}
