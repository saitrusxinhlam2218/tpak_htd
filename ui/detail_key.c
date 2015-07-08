static char sz__FILE__[]="@(#)detail_key.c	10.1.2.2(ver) Date Release 1/30/95" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  detail_key.c; Rel: 6.11.0.0; Get date: 3/24/92 at 12:27:58
* @(#)  Last delta: 1/15/92 at 10:00:58
* @(#)  SCCS File: /taxi/sccs/s.detail_key.c
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
ERROR: "Compile time constant SCANDINAVIA or NAMERICA undefined."
#endif
#endif

#include <sys/types.h>
#include <errno.h>
#include <cursesX.h>
#include <string.h>

#include "taxipak.h"
#include "Object.h"
#include "List_public.h"
#include "ui_strdefs.h"
#include "user.h"
#include "srch_const.h"
#include "func_keys.h"
#include "call_entry.h"
#include "ui_error.h"
#include "ui_def.h"
#include "screens.h"
#include "df_maint.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "menu.h"
#include "zgeo.h"
#include "ex_key.h"
#include "ui_ipc.h"
#include "except.h"					/* included to support exception message */
#include "writer.h"
#include "language.h"
#include "enhance.h"
#include "switch_ext.h"
#include "ui_call_msg.h"
#include "Call_lists_public.h"

#define B_FLD2_COL1	22				/* for printing field 2 in column 1 */
#define	B_DISP_COL2	78				/* for printing labels in column 2 */

extern int map_array[];					/* to map screen name into menus structures */
extern int print_scr();
extern struct menus *cmenu[];				/* array of pointers to menu structures in shared memory */
extern int def_fl_index;				/* index into fleets structure in shared memory */
extern int uid;						/* user's id */
extern int user_index;					/* index into user's structure in shared memory */
extern struct users *uiuser[];				/* array of pointers to users structure in sharec memory */
extern struct fleets *fleet[];				/* array of pointers to fleets structure in shared memory */
extern int disp_extended_scr(), prt_fields(), read_fields();
extern int prt_errror(), call_history();
extern int fill_call_rec(), get_call_type();
extern int scr_name, pid, fifo_fd;
extern int piu, clear_err_flag;
extern int calls_on_page;
extern struct cisam_cl srch_calls[];
extern struct cisam_sc subs_calls[];
extern struct scr_flds extended_scr_fld[];
extern struct scr_flds wakeup_scr_fld[];
extern int update_call();
extern struct cisam_cl call_struct;
extern end_extended_call;
extern short write_dispatch;					/* if should write to FIFO */
extern short write_disk;					/* if should write to calls file on disk */
extern short write_subs;					/* if should write to subs file on disk */
extern short user_changed_dt_tm;				/* did user change date-time field */
extern char req_buf[];
extern int (*e_check_field[])();
extern int (*w_check_field[])();
extern struct keydesc cl_key1;
extern int cur_fl_index;
struct scr_flds *scr_ptr;					/* pointer to 1st element of array of piu structures */
extern short max_piu;						/* max value of piu */
extern int valid_charge_nbr(), charge_related_fields();
extern short silent;
extern struct call_ipc_rec call_fifo;
extern char *sys_errlist[];
extern int num_vh_attr_s , num_dr_attr_s;
extern short send_to_dis_flag;			/* set if mesgs() function is called from send_to_dispatcher() */
extern int global_msg_num;				/* for CALL_W_MSG function key processing */
extern short been_touched;
extern char *Spaces;
extern int New_screen_from_command;
extern struct cisam_sc 	subs_struct;
extern short inters_flag;				/* flag set if user wants to enter inters address */
extern MANUAL_FLAT_HIST   ManualFlatInfo;
extern CALL_MSG_LIST_HNDL cmsg_list_hndl;

int ck_det_entry();		/* detail field validation routine */
int doing_update;		/* Indicates the update function key is being processed. */
int doing_detail;
int key_val;			/* key value returned from read_field routine */
int own_isrecnum;

void
print_detail_screen(cp)
struct cisam_cl *cp;					/* pointer to call_struct */
{
  if (cp->extended_type[0] == WAKEUP)
    {
      scr_ptr = wakeup_scr_fld;
      /* Real calls (including subscription calls that have gone out for bid)
	 have some value for status. Subscription call records do not */
      if (cp->status[0] == '\0' && strchr(cp->extended_type + 1, SUBS) != NULL)
	{
	  max_piu = MAX_PIU_WAK_SUBS;
	  scr_name = SUBS_WAKEUP_SCR;
	}
      else
	{
	  max_piu = MAX_PIU_WAKEUP;
	  scr_name = WAKEUP_CALL_SCR;
	}

      disp_wakeup_scr();
      prt_entry_areas(stdscr, scr_ptr, max_piu); 	/* print entry ares underlinded */
      prt_wold_fields(cp);						/* prints values as in call_struct */
    }
  else
    {
      /* Real calls (including subscription calls that have gone out for bid)
	 have some value for status. Subscription call records do not */
      if (cp->status[0] == '\0' && strchr(cp->extended_type + 1, SUBS) != NULL)
	{
	  max_piu = MAX_PIU_EXT_SUBS;
	  scr_name = SUBS_EXTENDED_SCR;
	}
      else
	{
	  max_piu = MAX_PIU_EXTENDED;
	  scr_name = EXTENDED_CALL_SCR;
	}
      /* initialize scr_ptr to the address of the extended_scr_fld[] array of structures. */
      scr_ptr = extended_scr_fld;  /* points to the 1st element of structure */
      
      /*  print the data for the call which was highlighted on the search screen.  */


      if ((cp->call_msg_nbr > '0') && (0))
	disp_extended_scr(1);
      else
	disp_extended_scr(0);

      prt_entry_areas(stdscr,scr_ptr,max_piu);
      prt_eold_fields(cp);						/* prints values as in call_struct */
    }

  attrset(A_REVERSE | A_UNDERLINE);
  if (doing_update)
    mvaddstr(0, 0, catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_5, "                                                          UPDATE                                                                   "));
  else
    mvaddstr(0, 0, catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_4, "                                                          DETAIL                                                                   "));
  prt_date();
  attrset(0);

}

/*  This function displays a call on an extended call entry screen.  
 *  The detail key can be entered by any of the multi-line search
 *  screens.  To return to the search screen, the user hits the detail key again.
 */
detail_key(srch_index)
int *srch_index;
{
	int end_detail_key;		/* loop control variable */
	char read_buf[READ_BUF_SIZE]; 	/* used to store users input */
	int field_entered;		/* indicates whether or not a date was entered */
	int ret_ck_fld;			/* value returned from the field entered check routine */
	int piu;			/* prompt in use */
	int should_clear = 0;
	int saved_scr_name;
	CALL_MSG_HNDL  cmsg_hndl;	

	saved_scr_name = scr_name;

	clear();

	doing_detail = 1;
	print_detail_screen(&srch_calls[*srch_index]);

	end_detail_key = NOT_DONE;
	while (end_detail_key == NOT_DONE) {

		piu = 1;
		silent = 1;  		/* disable printing of characters */
		key_val = read_field(stdscr,scr_ptr,piu,read_buf,&field_entered,&should_clear);
		if ((key_val == -1) || (key_val == 1))
		    continue;  /* user entered nothing or invalid characters */
		
		if (clear_err_flag)
		    clear_error();
		end_detail_key = ck_det_entry(key_val, srch_index);

	} /* end of while  */

	/* Restore old screen name if it didn't change via command key */
	if (!New_screen_from_command)
	    scr_name = saved_scr_name;

	doing_detail = 0;

}  /* End of detail_key() */

int ck_det_entry(key_val, srch_index)
int key_val;
int *srch_index;
{
    int old_scr_name;
    int index;
    int found;

	switch(key_val)  {

		case NEXT_PG_KEY :
			if (*srch_index < (calls_on_page - 1))
			  {

			    /* If next record is not a call, go forward until one is found */
			    if ((scr_name != SUBS_EXTENDED_SCR && scr_name != SUBS_WAKEUP_SCR) &&
				srch_calls[*srch_index + 1].nbr == 0)
			      {
				index = *srch_index + 1;
				found = 0;
				while (index < calls_on_page - 1)
				  {
				    index++;		/* next call */
				    if (srch_calls[index].nbr != 0)
				      {
					found = 1;
					break;
				      }
				  }
				
				if (!found)
				  {
				    prt_error(UI_NO_RECORD,"");
				    return(NOT_DONE);
				  }
				*srch_index = index;
			      }
			    else
			      (*srch_index)++;
			    
			    clear_s();
			    /* Copy subs calls to search calls */
			    if (scr_name == SUBS_EXTENDED_SCR || scr_name == SUBS_WAKEUP_SCR)
			      {
				subs_to_call(&subs_calls[*srch_index], &srch_calls[*srch_index]);
				subs_struct = subs_calls[*srch_index];
			      }
			    print_detail_screen(&srch_calls[*srch_index]);
			    return(NOT_DONE);
			  }
			else
			  {
			    prt_error(UI_NO_RECORD,"");
			    return(NOT_DONE);
			  }
			break;

		case PREV_PG_KEY:
			if (*srch_index > 0)
			  {

			    /* If previous one is not a call, go back until you find one */
			    if ((scr_name != SUBS_EXTENDED_SCR && scr_name != SUBS_WAKEUP_SCR) &&
				srch_calls[*srch_index - 1].nbr == 0)
			      {
				/* If we fail to find a call, we want to leave *srch_index the same */
				index = *srch_index - 1;
				found = 0;
				while (index > 0)
				  {
				    index--;		/* previous call */
				    if (srch_calls[index].nbr != 0)
				      {
					found = 1;
					break;
				      }
				  }

				if (!found)
				  {
				    prt_error(UI_NO_RECORD,"");
				    return(NOT_DONE);
				  }
				*srch_index = index;
			      }
			    else
			      (*srch_index)--;

			    clear_s();
			    
			    /* Copy subs calls to search calls */
			    if (scr_name == SUBS_EXTENDED_SCR || scr_name == SUBS_WAKEUP_SCR)
			      {
				subs_to_call(&subs_calls[*srch_index], &srch_calls[*srch_index]);
				subs_struct = subs_calls[*srch_index];
			      }
			    print_detail_screen(&srch_calls[*srch_index]);
			    return(NOT_DONE);
			    
			  }
			else
			  {
			    prt_error(UI_NO_RECORD,"");
			    return(NOT_DONE);
			  }
			break;

		case PRINT_SCR_KEY :
			print_scr(NULL);
			return(NOT_DONE);

		case UPDATE_KEY :
			been_touched = TRUE;
			/* lock and read the call record. Read new fields from the terminal. */
			num_vh_attr_s = num_dr_attr_s = 0; /* clear out count */
			key_val = update_key(srch_index);
			attrset(A_REVERSE | A_UNDERLINE);
			mvaddstr(0, 0, catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_4, "                                                          DETAIL                                                                   "));
			attrset( 0 );
			
			if (key_val != COMMAND_KEY)
			  return(NOT_DONE);
			/* Fall through for command key */

		case COMMAND_KEY:
			if (New_screen_from_command) return(DONE);
			old_scr_name = scr_name;
			command_line();
			if (scr_name != old_scr_name || New_screen_from_command)
			    return(DONE);
			return(NOT_DONE);
			break;

		case DETAIL_KEY :
			return(DONE);
			break;

			
		case HISTORY_KEY :
		        old_scr_name = scr_name;
			if ( scr_name == SUBS_EXTENDED_SCR )
			  {
			    if (subcall_history(subs_calls[*srch_index].nbr) == 0)
			      return(NOT_DONE);
			  }
		        else if (call_history(srch_calls[*srch_index].nbr) == 0)
			  return(NOT_DONE);
			if (scr_name != old_scr_name)
			    return(DONE);
			return(NOT_DONE);
			break;
		case APPROVE_KEY :
			if ((0) && (srch_calls[*srch_index].call_msg_nbr > '0')){
				disp_call_msg(srch_calls[*srch_index].nbr,1); /*  display call messages of SamPlan call
										with call number nbr */
				break;
			}

		default :
			prt_error(UI_INV_KEY,"");
			return(NOT_DONE);
			break;

	}
}

/********************************************************************/
/*  The update key can be used while on a detail screen, or an      */
/*  individual call search screen.  Its purpose is to allow the     */
/*  user to update the current call.                                */
/********************************************************************/

update_key(srch_index)
int *srch_index;
{
	int ret_upd_call;				/* return code from update_call() */
	short old_call_zone;				/* watch for zone change from 0 to a valid zone */
	int ret_val;					/* return from call_mesg() */
	int old_pri_status;

	struct excpt_rec except_fifo;			/* to build and write an exception to dispatch */
	struct cisam_cl *cl_ptr;
	struct cisam_cl tmp_call_struct;
	time_t	update_date_time;			/* if due time changes the old due date time is saved here for sending to dispatch*/
	BOOLEAN bCallCancelled = FALSE;
	BOOLEAN bCallClosed = FALSE;

	doing_update = 1;

	if (scr_name == SUBS_WAKEUP_SCR || scr_name == SUBS_EXTENDED_SCR)
	  {
	    ret_val = subs_update_key(srch_index);
	    doing_update = 0;
	    return(ret_val);
	  }

	cl_ptr = (struct cisam_cl *)req_buf;
	memset((char *)cl_ptr,'\0',sizeof(struct cisam_cl));			/* MAKE SURE YOUR WORK AREA IS CLEAN */
	cl_ptr->nbr = srch_calls[*srch_index].nbr;

	/*  In case another user has updated this call record, get a new copy from the disk file. */
	if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL, 0) < 0) {
		prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_1, "<call file>"));
		doing_update = 0;
		return(DONE);
	}
	 
	/*  move the current call record to the structure used by the update_call routine */
	memcpy(&call_struct,req_buf,sizeof(struct cisam_cl));

	/*  save the call information in case the user hits the refresh */
	/*  or update keys while on the update screen */
	memcpy(&tmp_call_struct,req_buf,sizeof(struct cisam_cl));

	while((ret_upd_call = update_call()) == REFRESH_KEY);


	if(ret_upd_call != SEND_KEY && ret_upd_call != SEND_SUPERV_KEY) {
		if(ret_upd_call != UPDATE_KEY && ret_upd_call != COMMAND_KEY) 
			prt_error(UI_INV_KEY,"");
		memcpy(&call_struct,&tmp_call_struct,sizeof(struct cisam_cl));
		doing_update = 0;
		if (ret_upd_call == COMMAND_KEY)
		  return(COMMAND_KEY);
		return(DONE);
	}


	if(strcmp(call_struct.due_date, tmp_call_struct.due_date) ||
	   strcmp(call_struct.due_time, tmp_call_struct.due_time))
		update_date_time = tmp_call_struct.due_date_time;
	else
		update_date_time = 0;

	memcpy(req_buf,&call_struct,sizeof(struct cisam_cl)); 			/* Restore the call structure */

	if(db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0) < 0) {	 	/* get the call record and lock it.  */
		prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_1, "<call file>"));
		doing_update = 0;
		return(DONE);
	}
	memcpy(&tmp_call_struct,req_buf,sizeof(struct cisam_cl));
	old_call_zone = tmp_call_struct.pckup_zone;
	old_pri_status = tmp_call_struct.pri_status;
	
	strncpy(tmp_call_struct.extended_type,call_struct.extended_type,sizeof(call_struct.extended_type) - 1);
	strncpy(tmp_call_struct.pckup_adr_cmnt,call_struct.pckup_adr_cmnt,30);	/* have been updated */
	strncpy(tmp_call_struct.general_cmnt,call_struct.general_cmnt,64);
	tmp_call_struct.pckup_str_nbr = call_struct.pckup_str_nbr;
	tmp_call_struct.pckup_str_suf = call_struct.pckup_str_suf;
	strncpy(tmp_call_struct.pckup_pre_dir,call_struct.pckup_pre_dir,2);
	strncpy(tmp_call_struct.pckup_str_name,call_struct.pckup_str_name,20);
	strncpy(tmp_call_struct.pckup_str_key,call_struct.pckup_str_key,4);
	strncpy(tmp_call_struct.pckup_str_type,call_struct.pckup_str_type,3);
	strncpy(tmp_call_struct.pckup_post_dir,call_struct.pckup_post_dir,2);
	strncpy(tmp_call_struct.intr_pre_dir,call_struct.intr_pre_dir,2);
	strncpy(tmp_call_struct.intr_str_name,call_struct.intr_str_name,20);
	strncpy(tmp_call_struct.intr_str_type,call_struct.intr_str_type,3);
	strncpy(tmp_call_struct.intr_post_dir,call_struct.intr_post_dir,2);
	strncpy(tmp_call_struct.pckup_apt_room,call_struct.pckup_apt_room,5);
	strncpy(tmp_call_struct.pckup_city,call_struct.pckup_city,3);
	tmp_call_struct.pckup_zone = call_struct.pckup_zone;
	strncpy(tmp_call_struct.passenger_name,call_struct.passenger_name,20);
	strncpy(tmp_call_struct.phone_area,call_struct.phone_area,3);
	strncpy(tmp_call_struct.phone_nbr,call_struct.phone_nbr,7);
	strncpy(tmp_call_struct.phone_ext,call_struct.phone_ext,5);
	strncpy(tmp_call_struct.phone_number, call_struct.phone_number, CHARS_PHONE_NUMBER);
	strncpy(tmp_call_struct.dest_adr_cmnt,call_struct.dest_adr_cmnt,30);
	tmp_call_struct.dest_str_nbr = call_struct.dest_str_nbr;
	tmp_call_struct.dest_str_suf = call_struct.dest_str_suf;
	strncpy(tmp_call_struct.dest_pre_dir,call_struct.dest_pre_dir,2);
	strncpy(tmp_call_struct.dest_str_name,call_struct.dest_str_name,20);
	strncpy(tmp_call_struct.dest_str_type,call_struct.dest_str_type,3);
	strncpy(tmp_call_struct.dest_post_dir,call_struct.dest_post_dir,2);
	strncpy(tmp_call_struct.dest_apt_room,call_struct.dest_apt_room,5);
	strncpy(tmp_call_struct.dest_city,call_struct.dest_city,3);
	tmp_call_struct.dest_zone = call_struct.dest_zone;
	tmp_call_struct.personal_veh = call_struct.personal_veh;
	tmp_call_struct.personal_rqst = call_struct.personal_rqst;
	tmp_call_struct.veh_attr_flag = call_struct.veh_attr_flag;
	memcpy(tmp_call_struct.veh_attr,call_struct.veh_attr,33);	/* memcpy used in case nulls are in string */
	tmp_call_struct.drv_attr_flag = call_struct.drv_attr_flag;
	memcpy(tmp_call_struct.drv_attr,call_struct.drv_attr,33);	/* memcpy used in case nulls are in string */
	tmp_call_struct.priority = call_struct.priority;
	tmp_call_struct.pri_status = call_struct.pri_status;
	strncpy(tmp_call_struct.hold_line,call_struct.hold_line,4);
	tmp_call_struct.local = call_struct.local;
	tmp_call_struct.advise_arrival = call_struct.advise_arrival;
#ifdef NAMERICA
	tmp_call_struct.send_destination = call_struct.send_destination;
	tmp_call_struct.send_rate = call_struct.send_rate;
#endif
	strncpy(tmp_call_struct.charge_nbr,call_struct.charge_nbr,18);
	strncpy(tmp_call_struct.requested_by,call_struct.requested_by,20);
	strncpy(tmp_call_struct.person_to_see,call_struct.person_to_see,20);
	tmp_call_struct.pod_required = call_struct.pod_required;
	strncpy(tmp_call_struct.pod_name,call_struct.pod_name,20);
	tmp_call_struct.call_rate = call_struct.call_rate;
	tmp_call_struct.vat = call_struct.vat;
	tmp_call_struct.last_changed_by = (short)uid;
	strncpy(tmp_call_struct.company_name,call_struct.company_name,25);
	strncpy(tmp_call_struct.ac_addt1_info1,call_struct.ac_addt1_info1,20);
	strncpy(tmp_call_struct.ac_addt1_info2,call_struct.ac_addt1_info2,20);
	strncpy(tmp_call_struct.driver_msg,call_struct.driver_msg,36);
	strncpy(tmp_call_struct.pckup_pmap,call_struct.pckup_pmap,sizeof(call_struct.pckup_pmap) - 1);
	strncpy(tmp_call_struct.dest_dmap,call_struct.dest_dmap,sizeof(call_struct.dest_dmap) - 1);
	strncpy(tmp_call_struct.zone_desc,call_struct.zone_desc,20);
	strncpy(tmp_call_struct.pass_name_key,call_struct.pass_name_key,4);
	strncpy(tmp_call_struct.due_date,call_struct.due_date,9);
	strncpy(tmp_call_struct.due_time,call_struct.due_time,6);
	strncpy(tmp_call_struct.status,call_struct.status,8);
	tmp_call_struct.due_date_time = call_struct.due_date_time;
	tmp_call_struct.via_addresses = call_struct.via_addresses;
	strncpy(tmp_call_struct.invoice_nbr1, call_struct.invoice_nbr1, 8);
	strncpy(tmp_call_struct.invoice_nbr2, call_struct.invoice_nbr2, 8);
	strncpy(tmp_call_struct.invoice_nbr3, call_struct.invoice_nbr3, 8);
	strncpy(tmp_call_struct.invoice_nbr4, call_struct.invoice_nbr4, 8);
	strncpy(tmp_call_struct.ct_adr_cmnt, call_struct.ct_adr_cmnt, 30);
	tmp_call_struct.gps_lat = call_struct.gps_lat;
	tmp_call_struct.gps_long = call_struct.gps_long;

	if ( 1 )
	  {
	    if ( call_struct.extended_type[2] == TIMEC )
	      tmp_call_struct.drv_attr[22] = YES;
	    else
	      tmp_call_struct.drv_attr[22] = NO;
	  }
	
        time_subs_call(); /* MRB new */

	if ( !strcmp( tmp_call_struct.status, COMPLETE ) )
	  bCallClosed = TRUE;

        if ( !strcmp( tmp_call_struct.status, NO_SHOW_ITM ) )
          bCallCancelled = TRUE;
	
	if ( ( !strncmp( tmp_call_struct.status, CANCELD, 6 ) ) ||
	     ( !strncmp( tmp_call_struct.status, "XFER", 4 ) ) )
	  bCallCancelled = TRUE;
	else
	  {
	    if (tmp_call_struct.dispd_date_time == 0) {				/* not dispatched yet */
	      if(/*tmp_*/call_struct.extended_type[2] == TIMEC || /*tmp_*/call_struct.extended_type[2] == SUBS) {
		tmp_call_struct.pri_status = 63;			/* PENDING time call */
		strcpy(tmp_call_struct.status,PENDING);
	      }
	      else {
		tmp_call_struct.pri_status = tmp_call_struct.priority;	/* not time call,reg pri */
		strcpy(tmp_call_struct.status,UNASSGND);
	      }
	    }
	  }
	memcpy(req_buf,&tmp_call_struct,sizeof(struct cisam_cl));		/* copy the updated structure into req_buf.  */

	if(db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {
		prt_error(UI_NOUPDATE,catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_1, "<call file>"));
		doing_update = 0;
		return(DONE);
	}

	own_isrecnum = isrecnum;

	memcpy(&srch_calls[*srch_index],&tmp_call_struct,sizeof(struct cisam_cl));	/* update the call searches array  */
	if(db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0) {
		prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_2, "<error releasing call file>"));
		doing_update = 0;
		return(DONE);
	}
	add_callh_spec(R_UPDATED);							/* write "UPDATED" rec to call hist */
        if ( bCallCancelled == TRUE )
	  return(DONE);

	if ( bCallClosed == TRUE )
	  return(DONE);
	    
	if ( ManualFlatInfo.ManualFlatPerformed )
	  add_callh_spec(R_MANUAL_RATE);
	
#ifdef ALLOW_UPDATES_WHILE_DISP
	if(tmp_call_struct.dispd_date_time == 0) {
#endif
		if(old_call_zone != call_struct.pckup_zone) {
			except_fifo.u_prcs_id = pid;				/* zone newly defined and not dispatched yet */
			except_fifo.rec_type = EXCEPT_REQ;
			except_fifo.except_ptr = NULL;
			except_fifo.resolve_code = (short) R_MAN_ZONE;		/* this call was manually zoned */
			except_fifo.except_nbr = 0;
			except_fifo.call_nbr = call_struct.nbr;
			except_fifo.grouped_with = call_struct.grouped_with;
			except_fifo.veh_nbr = call_struct.veh_nbr;
			except_fifo.zone_nbr = call_struct.pckup_zone;
			except_fifo.old_zone_nbr = old_call_zone; /*send old call zone thru so that the F35 intervals for the 2 zones*/
							      /* so the old and new can be updated appropriately.  old_call_zone*/
							      /* to decrement the inverval and the call record pickup zone for*/
							      /* increcmenting the new zone inverval*/
			except_fifo.old_due_time = update_date_time;		/*send old time for removing call from old F35 interval*/
										/*if the date is zero then date or time was not changed*/
	
			if(old_call_zone == 0 && read_callh_rec(call_struct.nbr,UNZONED)) { 	/* returns non-0 if found rec */
				update_callh_rec(ACT_MANZONE,call_struct.pckup_zone); 	   	/* update the UNZONED hist record */
				update_unzoned(call_struct.fleet, call_struct.nbr, call_struct.pckup_zone, uid);
			}

			if(key_val == SEND_SUPERV_KEY) {
				send_to_dis_flag = 1;
				ret_val = call_mesgs();				/* processing inside the msg window */
				send_to_dis_flag = 0;

				if(global_msg_num != -1) {
					if(global_msg_num > 0) {		/* call with msg bit for generating exception */
						set_bit(CALL_W_MSG-1,sizeof(struct except_bits),(unsigned char *)&call_fifo.status);
						set_call_bit(CALL_W_MSG - 1);
					}
					else {
						set_bit(CALL_NO_MSG-1,sizeof(struct except_bits),(unsigned char*)&call_fifo.status);
						set_call_bit(CALL_NO_MSG - 1);
					}
				}
				call_fifo.msg_nbr = global_msg_num;		/* copy msg # for call with msg exception */
				global_msg_num = 0;				/* set the msg # to 0, for next processing */
			}


                        // MULTI_TRIP Patch - don't sent dispatch IPC message
                        // if multi is updated and it is not in a UNASSIGNED/PENDING state
                        //                        if ( call_fifo.call_type.multiple == 1 )
                        if ( call_struct.extended_type[4] == MULT_CALL )
                          {
                            if ( !strcmp( call_struct.status, PENDING ) ||
                                 !strcmp( call_struct.status, UNASSGND ) )
                              if(write(fifo_fd,&except_fifo,sizeof(struct excpt_rec)) == -1)    /* tell dispatch about update */
				prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_3, "<except rec to FIFO>"));
                          }
			else if(write(fifo_fd,&except_fifo,sizeof(struct excpt_rec)) == -1)    /* tell dispatch about update */
                          prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_3, "<except rec to FIFO>"));			
		}
		else {
			fill_call_rec();
#ifdef FOO
			call_fifo.old_due_time = update_date_time;
#endif
										/*if the date is zero then date or time was not changed*/
			call_fifo.updated_call = 1;		/*send that we are updating a call already in an F35 interval*/
			if(key_val == SEND_SUPERV_KEY) {
				send_to_dis_flag = 1;
				ret_val = call_mesgs();				/* processing inside the msg window */
				send_to_dis_flag = 0;

				if(global_msg_num != -1) {
					if(global_msg_num > 0) {		/* call with msg bit for generating exception */
						set_bit(CALL_W_MSG-1,sizeof(struct except_bits),(unsigned char *)&call_fifo.status);
						set_call_bit(CALL_W_MSG - 1);
					}
					else {
						set_bit(CALL_NO_MSG-1,sizeof(struct except_bits),(unsigned char*)&call_fifo.status);
						set_call_bit(CALL_NO_MSG - 1);
					}
				}
				call_fifo.msg_nbr = global_msg_num;		/* copy msg # for call with msg exception */
				global_msg_num = 0;				/* set the msg # to 0, for next processing */
			}

                        // MULTI_TRIP Patch - don't sent dispatch IPC message
                        // if multi is updated and it is not in a UNASSIGNED/PENDING state
                        if ( call_fifo.call_type.multiple == 1 )
                          {
                            if ( !strcmp( call_struct.status, PENDING ) ||
                                 !strcmp( call_struct.status, UNASSGND ) )
                              if(write(fifo_fd,&call_fifo,sizeof(struct call_ipc_rec)) == -1)    /* tell dispatch about update */
				prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_3, "<call rec to FIFO>"));
                          }                        
			else if (write(fifo_fd,&call_fifo,sizeof(struct call_ipc_rec)) == -1 )  { 		/* write on fifo failed */
				prt_error(UI_WRITE,"<%s>",sys_errlist[errno]);
				doing_update = 0;
				return(DONE);
			}
		}
#ifdef ALLOW_UPDATES_WHILE_DISP
	      }
#endif
	

	write_dispatch = 1;

	
	/* set all flags to default values */
	write_disk = 1;						/* regardless of whether the call to dispatch */
	write_subs = 0;						/* succeded or not */
	prt_error(UI_REC_UPDATED," ");
	doing_update = 0;
	user_changed_dt_tm = 0;

	return(NOT_DONE);
}


subs_update_key(srch_index)
int *srch_index;
{
  int ret_upd_call;				/* return code from update_call() */
  
  struct cisam_sc *sc_ptr;
  struct cisam_sc tmp_subs_struct;
  struct cisam_cl tmp_call_struct;
  int piu;
  int rc;
  
  sc_ptr = (struct cisam_sc *) req_buf;
  memset((char *)sc_ptr, '\0', sizeof(struct cisam_sc));
  sc_ptr->nbr = subs_calls[*srch_index].nbr;
  if (db(SUBCALL_FILE_ID, READ_KEYS, &sc_key1, ISEQUAL, 0) < 0)
    {
      prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_UI_SUBS_2, "<subscription file>"));
      return(DONE);
    }
	    
  subs_to_call(sc_ptr, &call_struct);
  subs_struct = *sc_ptr;
  tmp_subs_struct = subs_struct;
  tmp_call_struct = call_struct;

update_subs:
  while((ret_upd_call = update_call()) == REFRESH_KEY);

  if (ret_upd_call != SEND_KEY && ret_upd_call != SEND_SUPERV_KEY)
    {
      if(ret_upd_call != UPDATE_KEY && ret_upd_call != COMMAND_KEY) 
	prt_error(UI_INV_KEY,"");
      call_struct = tmp_call_struct;
      subs_struct = tmp_subs_struct;
      if (ret_upd_call == COMMAND_KEY)
	return(COMMAND_KEY);
      return(DONE);
    }

  fill_subs_rec();

  /* piu is to keep subs_send_happy */
  piu = 1;
  rc = subs_send(&piu, NO);

  if (rc == DONE)
  {
      srch_calls[*srch_index] = call_struct;	/* update the call searches array  */
      subs_calls[*srch_index] = subs_struct;	/* update the subcall searches array  */
      return(DONE);
  }

  /* subs_send() failed, go to update again */
  goto update_subs;
}


update_wakeup()
{
  int piu;					/* prompt in use */
  int field_entered = 0;				/* whether field entered or not */
  int save_scr_name;
  char read_buf[READ_BUF_SIZE];			/* buffer for reading fields on screen */
  int ret_ck_fld;					/* return value of check field routine */
  int should_clear = 0;
  
  save_scr_name = scr_name;

  disp_wakeup_scr();

  prt_entry_areas(stdscr,scr_ptr,max_piu); 	/* print entry ares underlinded */
  prt_wold_fields(&call_struct);		/* prints values as in call_struct */

  attrset(A_REVERSE);
  mvaddstr(0, 0, catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_5, "                                                          UPDATE                                                                   "));
  prt_date();
  attrset(0);

  piu = W_NAME;
  
  /* at this point, the screen with default old values */
  /* is on display and prompt is in the right field */
  /*  Enter a while loop which executes */
      
  /*  by "piu" is being entered; and the user is still on the same screen.  */
  end_extended_call = 0;				/* unset flag, so that we go in loop */
  
  while (!end_extended_call){ 			/* Initialize local integer field_entered = 0. */
    /* This flag set to 0 if field defined by piu is not entered by user, */
    /* if field is entered then it gets set to 1 (by read_field() function). */
    
    key_val = read_field(stdscr,scr_ptr,piu,read_buf,&field_entered,&should_clear);
    
    if (clear_err_flag ) 						/* if last line has an error msg printed on it */
      clear_error();						/* then clear it */
    
    if ( (ret_ck_fld = (*w_check_field[piu])(scr_ptr,&piu,read_buf,key_val,&field_entered)) == DONE )  {
      scr_name = save_scr_name; 				/* restore the scr_name */
      
      /* field has been entered, and piu has been */
      /* changed to next field which is to be entered */
      if (piu > max_piu )
	piu = 1;	 				/* wrap around */
      else if (piu == 0)
	piu = max_piu;					/* wrap around to last field */
    }
    else if (key_val == PRINT_SCR_KEY)
      print_scr(NULL);
    else if (key_val == SEND_SUPERV_KEY || key_val == REFRESH_KEY || key_val == UPDATE_KEY || 
	     key_val == SEND_KEY || key_val == COMMAND_KEY) {
      scr_name = save_scr_name;				/* restore the screen name */
      end_extended_call = 1;
      return(key_val);
    }
    else
      prt_error(UI_INV_KEY,"");
    
  }
}

/********************************************************************************/
/*  There are two ways to get to this routine : 			        */
/*  1)  From the detail screen which is the result of hitting the detail        */
/*	key while on a multi_line search, then hitting the update key.          */
/*  2)  From the individual call search screen, then hitting the update key.    */
/********************************************************************************/
update_call()
{
	int piu;					/* prompt in use */
	int field_entered = 0;				/* whether field entered or not */
	int save_scr_name;
	char read_buf[READ_BUF_SIZE];			/* buffer for reading fields on screen */
	int ret_ck_fld;					/* return value of check field routine */
	int should_clear = 0;
	int i;
	int last_piu;
	int rc;
	char AttributeSet;
	int AttrCol = 0, AttrRow = 0;
	short MaxAttrs;

	save_scr_name = scr_name; 				/* restore the scr_name */


	/* Display the EXTENDED_CALL_SCR by calling the disp_extended_scr(). */
	/* This function looks at the global variable scr_name and displays  */
	/* the appropriate screen. Routine displays the screen with labels only */ 
	if (call_struct.extended_type[0] == WAKEUP)
	  {
	    rc = update_wakeup();
	    return(rc);
	  }

	print_detail_screen(&call_struct);
	refresh();
	attrset(A_REVERSE | A_UNDERLINE);
	mvaddstr(0, 0, catgets(UI_m_catd, UI_1, UI_DETAIL_KEY_5, "                                                          UPDATE                                                                   "));
	prt_date();
	attrset(0);

#ifdef NAMERICA
        piu = E_CHARGE;
#else
	piu = E_PKUP_ST_NAME;
#endif

							/* at this point, the screen with default old values */
							/* is on display and prompt is in the right field */
							/*  Enter a while loop which executes */



	if (call_struct.intr_str_name[0] != '\0')
	  inters_flag = 1;
	else
	  inters_flag = 0;

	last_piu = 0;

	end_extended_call = 0;
	while (!end_extended_call){ 			/* Initialize local integer field_entered = 0. */
		 					/* This flag set to 0 if field defined by piu is not entered by user, */
		 					/* if field is entered then it gets set to 1 (by read_field() function). */

		if (piu == 1)
		  silent = 1;		/* disable printing of chars */

		should_clear = (piu==E_TIME || piu==E_POD_TIME); 		/* Read the field defined by "piu" */

		/* Can't update "# of cars" field on update screen */
#ifdef SCANDINAVIA
		if ((piu == E_CAR_NO) || (piu == E_CASH_ACCT))
#else
                  if ((piu == E_CAR_NO))
#endif
		  {
		    if (last_piu < piu)
		      {
			/* Go to field after */
			piu++;
		      }
		    else
		      piu--;
		  }


		if ( ( ( piu == E_RATE ) && ( 0 ) ) || ( piu == E_VAT ) )
		  {
		    if ( last_piu < piu )
		      {
			piu += 2; /** this moves us past the E_VAT field as well **/
		      }
		    else
		      {
			piu--;
		      }
		  }

	      	key_val = read_field(stdscr,scr_ptr,piu,read_buf,&field_entered,&should_clear);
				
		if (key_val == -1) 						/* read_field() timed out before field was entered*/
			continue; 						/* so go back to the same field */


		if (clear_err_flag ) 						/* if last line has an error msg printed on it */
			clear_error();						/* then clear it */
			
										/* check routines will validate orrect fields. */


		
		last_piu = piu;
		if ( key_val == ALTERNATE_KEY && piu == E_CAR_NEEDS )
		  {
                    alt_attr();

		  }

	     	else if ( (ret_ck_fld = (*e_check_field[piu])(scr_ptr,&piu,read_buf,key_val,&field_entered)) == DONE )  {

										/* field has been entered, and piu has been */
										/* changed to next field which is to be entered */
			if (piu > max_piu )
				piu = 1;	 				/* wrap around */
			else if (piu == 0)
				piu = max_piu;					/* wrap around to last field */
		} else if (key_val == ALTERNATE_KEY) {
			altrn_adr(&piu);          /* look for other address names */
			if (New_screen_from_command) {   /* user has used command key in altrn_ard */
				inters_flag = 0;
				return(COMMAND_KEY);
			}
		} else if (key_val == PRINT_SCR_KEY) {
			print_scr(NULL);
		} else if (key_val == SEND_SUPERV_KEY || key_val == REFRESH_KEY || key_val == UPDATE_KEY ||
			   key_val == SEND_KEY || key_val == COMMAND_KEY)
		  {
		    if ( ( key_val == SEND_KEY ) && ( scr_name != SUBS_EXTENDED_SCR ) )
		      {
			if ( !tcr_check_time_call_restriction(&call_struct, TRUE, key_val) )
			  continue;
		      }
			end_extended_call = 1;
			inters_flag = 0;
			scr_name = save_scr_name; 				/* restore the scr_name */
			return(key_val);
		} else {
			prt_error(UI_INV_KEY,"");
		}

	}
	inters_flag = 0;
	scr_name = save_scr_name; 				/* restore the scr_name */
	return(0);
}

