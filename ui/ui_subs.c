static char sz__FILE__[]="@(#)ui_subs.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  ui_subs.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:51:50
* @(#)  Last delta: 12/2/94 at 18:05:39
* @(#)  SCCS File: /taxi/sccs/s.ui_subs.c
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

#include <cursesX.h>
#include <sys/types.h>
#include <isam.h>
#include <time.h>
#include <string.h>

#include "mads_types.h"
#include "mads_isam.h"
#include "ui_error.h"
#include "df_maint.h"
#include "call_entry.h"
#include "ui_def.h"
#include "screens.h"
#include "zgeo.h"
#include "user.h"
#include "ui_ipc.h"
#include "writer.h"
#include "language.h"
#include "ui_strdefs.h"
#include "srch.h"
#include "Exception_private.h"
#include "switch_enh.h"

extern struct cisam_sc	subs_struct;	/* struct for subs calls */
extern struct cisam_cl call_struct;
extern int cur_fl_index;	/* index of the fleet the user is currently * working with */
extern struct fleets *fleet[];	/* array of pointers to fleets struct */
extern char req_buf[];
extern struct keydesc sc_key1, cl_key1;
extern int scr_name;
extern short user_knows;	/* if user knows that call is unzoned */
extern short call_zoned;	/* if call is not or not */
extern short write_dispatch;	/* if should write to FIFO */
extern short write_disk;	/* if should write to calls file on disk */
extern short write_subs;	/* if should write to subs file on disk */
extern short user_changed_dt_tm;	/* if should write to subs file on disk */
extern int b_prt_default(), e_prt_default(), w_prt_default();
extern struct cisam_cl call_struct;	/* for writing call file */
extern char req_buf[];
extern struct scr_flds basic_scr_flds[], extended_scr_fld[], wakeup_scr_fld[];
extern int pid;	/* process id of this process */
extern short inters_flag;	/* flag fet if intersection address should be read */
extern short clear_err_flag;	/* set if an error msg on the last line is to be cleared */
extern short write_unzoned_flag;	/* set if a record shoule be added to the unzoned address file */
extern int uid;	/* users unix id */
extern int toggle_piu;	/* piu value when toggle key is pressed */
extern int toggle_flag;	/* 1=toggle key pressed; 0=toggle key not pressed */
extern struct users *uiuser[];
extern int user_index;
extern struct call_ipc_rec call_fifo;
extern short zone_pkup_lead_time;		/* pickup lead time for pickup zone */
extern short get_short_time();
extern int fifo_fd;				/* fiel descriptor for FIFO */
extern int errno;
extern char *sys_errlist[];
extern struct call_ipc_rec call_fifo;		/* for call rec to FIFO */
extern int doing_update;
extern int calls_on_page;
extern long glEnhSwitch; 

/* fill_subs_rec: fills the cisam_sc (structure for subscription 
 *	record = subs_struct) from the cisam_cl (structure for 
 *	the calls file = call_struct).  
 *
 * NOTE:from_date & to_date shold be in yymmdd format as int's
 *	the time fields of short = hhmm format as shorts
 */
fill_subs_rec()
{

	  /* nbr is the system assigned subscription # */
          /*   subs_struct.nbr  ... assigned by system */
                                           
          subs_struct.fleet =  call_struct.fleet;
          strcpy(subs_struct.extended_type, call_struct.extended_type);
          strcpy(subs_struct.pckup_adr_cmnt,call_struct.pckup_adr_cmnt);
          subs_struct.local = call_struct.local;
          subs_struct.pckup_str_nbr = call_struct.pckup_str_nbr;
          subs_struct.pckup_str_suf =  call_struct.pckup_str_suf;
          strcpy(subs_struct.pckup_pre_dir, call_struct.pckup_pre_dir);
          strcpy(subs_struct.pckup_str_name,call_struct.pckup_str_name);
          strcpy(subs_struct.pckup_str_type,call_struct.pckup_str_type);
          strcpy(subs_struct.pckup_post_dir,call_struct.pckup_post_dir);
          strcpy(subs_struct.intr_pre_dir,call_struct.intr_pre_dir);
          strcpy(subs_struct.intr_str_name,call_struct.intr_str_name);
          strcpy(subs_struct.intr_str_type,call_struct.intr_str_type);
          strcpy(subs_struct.intr_post_dir,call_struct.intr_post_dir);
          strcpy(subs_struct.pckup_apt_room,call_struct.pckup_apt_room);
          strcpy(subs_struct.pckup_city,call_struct.pckup_city);
          strcpy(subs_struct.passenger_name, call_struct.passenger_name);
          strcpy(subs_struct.pass_name, call_struct.pass_name_key);
          strcpy(subs_struct.pckup_str_key, call_struct.pckup_str_key);
          subs_struct.pckup_zone  = call_struct.pckup_zone;
#ifdef NAMERICA
          strcpy(subs_struct.phone_area,call_struct.phone_area);
          strcpy(subs_struct.phone_nbr,call_struct.phone_nbr);
          strcpy(subs_struct.phone_ext,call_struct.phone_ext);
#else
          strcpy(subs_struct.phone_number, call_struct.phone_number);

	  /* Search is keyed off of phone_nbr, so copy it */
          strncpy(subs_struct.phone_nbr, call_struct.phone_number, 7);
	  subs_struct.phone_nbr[7] = '\0';
#endif
          strcpy(subs_struct.general_cmnt, call_struct.general_cmnt);
          strcpy(subs_struct.dest_adr_cmnt, call_struct.dest_adr_cmnt);
          subs_struct.dest_str_nbr  = call_struct.dest_str_nbr;
          subs_struct.dest_str_suf  = call_struct.dest_str_suf;
          strcpy(subs_struct.dest_pre_dir,call_struct.dest_pre_dir);
	  strcpy(subs_struct.dest_str_name, call_struct.dest_str_name);
          strcpy(subs_struct.dest_str_type,call_struct.dest_str_type);
          strcpy(subs_struct.dest_post_dir,call_struct.dest_post_dir);
          strcpy(subs_struct.dest_apt_room,call_struct.dest_apt_room);
          strcpy(subs_struct.dest_city,call_struct.dest_city);
          subs_struct.dest_zone = call_struct.dest_zone;
          subs_struct.personal_veh = call_struct.personal_veh;
          subs_struct.personal_rqst = call_struct.personal_rqst;
          subs_struct.veh_attr_flag = call_struct.veh_attr_flag;
          memcpy(subs_struct.veh_attr,call_struct.veh_attr, 33);
          subs_struct.drv_attr_flag = call_struct.drv_attr_flag;
          memcpy(subs_struct.drv_attr,call_struct.drv_attr, 33);
          subs_struct.nbr_of_vehs = call_struct.nbr_of_vehs;
          subs_struct.priority = call_struct.priority;
          strcpy(subs_struct.charge_nbr,call_struct.charge_nbr);
          strcpy(subs_struct.requested_by,call_struct.requested_by);
          strcpy(subs_struct.person_to_see,call_struct.person_to_see);
          subs_struct.pod_required = call_struct.pod_required;
          subs_struct.call_rate = call_struct.call_rate;

          subs_struct.date_time_in = call_struct.date_time_in;
          subs_struct.entered_by = call_struct.entered_by;
          subs_struct.last_changed_by =  call_struct.last_changed_by;
                                          
          strcpy(subs_struct.date_in,call_struct.date_in);
          strcpy(subs_struct.time_in,call_struct.time_in);
          strcpy(subs_struct.ac_addt1_info1,call_struct.ac_addt1_info1);
          strcpy(subs_struct.ac_addt1_info2,call_struct.ac_addt1_info2);
          strcpy(subs_struct.status_bits,call_struct.status_bits);
          strcpy(subs_struct.company_name,call_struct.company_name);
          strcpy(subs_struct.driver_msg,call_struct.driver_msg);
          strcpy(subs_struct.pckup_pmap,call_struct.pckup_pmap);
          strcpy(subs_struct.dest_dmap,call_struct.dest_dmap);
                                  
	  subs_struct.advise_arrival = call_struct.advise_arrival;
#ifdef NAMERICA
	  subs_struct.send_dest = call_struct.send_destination;
	  subs_struct.send_rate = call_struct.send_rate;
#endif

}	/* end fill_subs_rec() */


void
subs_to_call(subs, call)
     struct cisam_sc *subs;
     struct cisam_cl *call;
{

  bzero(call, sizeof(struct cisam_cl));

  call->fleet = subs->fleet;
  strcpy(call->extended_type, subs->extended_type);
  strcpy(call->pckup_adr_cmnt, subs->pckup_adr_cmnt);
  call->local = subs->local;
  call->pckup_str_nbr = subs->pckup_str_nbr;
  call->pckup_str_suf = subs->pckup_str_suf;
  strcpy(call->pckup_pre_dir, subs->pckup_pre_dir);
  strcpy(call->pckup_str_name, subs->pckup_str_name);
  strcpy(call->pckup_str_type, subs->pckup_str_type);
  strcpy(call->pckup_post_dir, subs->pckup_post_dir);
  strcpy(call->intr_pre_dir, subs->intr_pre_dir);
  strcpy(call->intr_str_name, subs->intr_str_name);
  strcpy(call->intr_str_type, subs->intr_str_type);
  strcpy(call->intr_post_dir, subs->intr_post_dir);
  strcpy(call->pckup_apt_room, subs->pckup_apt_room);
  strcpy(call->pckup_city, subs->pckup_city);
  strcpy(call->passenger_name, subs->passenger_name);
  strcpy(call->pass_name_key, subs->pass_name);
  strcpy(call->pckup_str_key, subs->pckup_str_key);
  call->pckup_zone = subs->pckup_zone ;
#ifdef NAMERICA
  strcpy(call->phone_area, subs->phone_area);
  strcpy(call->phone_nbr, subs->phone_nbr);
  strcpy(call->phone_ext, subs->phone_ext);
#else
  strcpy(call->phone_number, subs->phone_number);

  /* Search is keyed off of phone_nbr, so copy it */
  strncpy(call->phone_nbr, subs->phone_number, 7);
  call->phone_nbr[7] = '\0';
#endif
  strcpy(call->general_cmnt, subs->general_cmnt);
  strcpy(call->dest_adr_cmnt, subs->dest_adr_cmnt);
  call->dest_str_nbr = subs->dest_str_nbr;
  call->dest_str_suf = subs->dest_str_suf;
  strcpy(call->dest_pre_dir, subs->dest_pre_dir);
  strcpy(call->dest_str_name, subs->dest_str_name);
  strcpy(call->dest_str_type, subs->dest_str_type);
  strcpy(call->dest_post_dir, subs->dest_post_dir);
  strcpy(call->dest_apt_room, subs->dest_apt_room);
  strcpy(call->dest_city, subs->dest_city);
  call->dest_zone = subs->dest_zone;
  call->personal_veh = subs->personal_veh;
  call->personal_rqst = subs->personal_rqst;
  call->veh_attr_flag = subs->veh_attr_flag;
  memcpy(call->veh_attr, subs->veh_attr, 33);	/* memcpy used in case nulls are in string */
  call->drv_attr_flag = subs->drv_attr_flag;
  memcpy(call->drv_attr, subs->drv_attr, 33);	/* memcpy used in case nulls are in string */
  call->nbr_of_vehs = subs->nbr_of_vehs;
  call->priority = subs->priority;
  strcpy(call->charge_nbr, subs->charge_nbr);
  strcpy(call->requested_by, subs->requested_by);
  strcpy(call->person_to_see, subs->person_to_see);
  call->pod_required = subs->pod_required;
  call->call_rate = subs->call_rate;

  call->date_time_in = subs->date_time_in;
  call->entered_by = subs->entered_by;
  call->last_changed_by = subs->last_changed_by;
                                          
  strcpy(call->date_in, subs->date_in);
  strcpy(call->time_in, subs->time_in);
  strcpy(call->ac_addt1_info1, subs->ac_addt1_info1);
  strcpy(call->ac_addt1_info2, subs->ac_addt1_info2);
  strcpy(call->status_bits, subs->status_bits);
  strcpy(call->company_name, subs->company_name);
  strcpy(call->driver_msg, subs->driver_msg);
  strcpy(call->pckup_pmap, subs->pckup_pmap);
  strcpy(call->dest_dmap, subs->dest_dmap);
                                  
  call->advise_arrival = subs->advise_arrival;
#ifdef NAMERICA
  call->send_destination = subs->send_dest;
  call->send_rate = subs->send_rate;
#endif

}	/* end subs_to_call */

/* write the subscription record to disk */
write_subs_rec()
{
	struct cisam_sc *sptr;
	
	sptr = (struct cisam_sc *)req_buf;

	memcpy((char*)sptr,&subs_struct,sizeof(struct cisam_sc));
	if ( db(SUBCALL_FILE_ID,ADD_REC,&sc_key1,ISEQUAL,0) < 0 ) {
		prt_error(UI_WRITE,"<%s %d>", catgets(UI_m_catd, UI_1, UI_UI_SUBS_1, "subscription file"), iserrno);
	}  /* end if */
	/* save subscription call # in subs_struct */
	subs_struct.nbr = sptr->nbr;

}  /* end write_subs_rec()  */


/* open_subs_file: opens the subscription file */
open_subs_file()
{
	if ( db(SUBCALL_FILE_ID, OPEN, &sc_key1, ISINOUT | ISMANULOCK, 0 ) != SUCCESS ) {
		prt_error(UI_OPEN,catgets(UI_m_catd, UI_1, UI_UI_SUBS_2, "<subscription file>"));
		return(-1);
	}
	return(0);
}  /* end open_subs_file()  */


/* close_subs_file: closes the subscription file */
close_subs_file()
{
        return( 0 );
	if ( db(SUBCALL_FILE_ID,CLOSE,&sc_key1,ISEQUAL,0) < 0 ) {
		prt_error(UI_CLOSE,catgets(UI_m_catd, UI_1, UI_UI_SUBS_2, "<subscription file>"));
		return(-1);
	}
	return(0);
}  /* end close_subs_file()  */


/* ----------------------------------------------------------------------------------------------------------------------------	*
* subs_send: adds a record to the subscription call file; after checking exceptions. Should only be called from subscription
* screens
* ----------------------------------------------------------------------------------------------------------------------------- */
subs_send(piu_ptr,send_dup_flag)
int *piu_ptr;						/* pointer to piu */
char send_dup_flag;					/* YES send_duplicate key entered; NO send key entered */
{
	char loop_count;				/* # of requetss to be written to FIFO */
	struct scr_flds *ptr_scr_fld;			/* pointer to the array of piu structs*/
	int (*func_ptr_defs)();				/* pointer to either b_prt_defaults or e_prt_defaults or w_prt_defaults */

	int cisam_ret_num;				/* cisam generated # foirom calls file */
	struct scr_flds *ptr_to_fld;			/* points to approp. piu struct */
	int i;
	int max_piu;					/* max piu value */
	struct cisam_cl *temp_cisam_ptr;		/* for assigning to req_buf */
	char no_of_vehs;				/* # of calls to service this call */
	short due_time;
	int call_nbr;
	int tmp;

	if ( scr_name !=  SUBS_BASIC_SCR && scr_name != SUBS_EXTENDED_SCR && scr_name != SUBS_WAKEUP_SCR ) {
 		/* not called from subscription call entry screen */
		prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_UI_SUBS_3, "<send/send_dup key from this screen>"));
		return(NOT_DONE);
	}

	/* subscription calls with exceptions are not allowed to be entered */
	if ( check_subs_errors(piu_ptr) == NOT_DONE) 			
		return(NOT_DONE);

	if (doing_update)
	  call_struct.last_changed_by = (short) uid;
	else
	  {
	    call_struct.date_time_in = time(NULL);
	    call_struct.entered_by = (short)uid;
	    get_pkup_time(call_struct.time_in); 			/* save the date & the time this call */
	    get_pkup_date(call_struct.date_in); 			/* is entered, in display format */
	  }
	time_subs_call();					/* checks whether its a time or subscription call */
								/* (also sets call_struct.due_date_time field) */
	if((call_struct.priority = get_call_pri()) == -1){ 			/* get the call priority and save it */
		call_struct.priority = fleet[cur_fl_index]->passenger_priority_def;	/* default to passenger */
	}
	call_struct.pri_status = call_struct.priority;

#ifdef NAMERICA
	if (call_struct.phone_nbr[0] == '\0')
		call_struct.phone_area[0] = '\0';		/* no phone number -> no area code */
#endif
	
	/* write record to subscription file */
	if (doing_update)
	  {
	    update_subs_rec(&call_struct, &subs_struct);
	    if (0)
	      add_subcallh_spec( R_UPDATED, &subs_struct );
	  }
	else
	  {
	    fill_subs_rec();
	    write_subs_rec();
	    if (0)	    
	      add_subcallh_spec( R_ENTERED, &subs_struct );
	  }
	
	/* check if a record should be added to the call file */
	if (( due_time = (short)should_add_callrec(&subs_struct) ) && (!doing_update))  {
		/* should add a call rec */
		if(due_time > get_short_time((long)time(NULL) + (long)zone_pkup_lead_time)) {
			call_struct.pri_status = 63;			/* PENDING time call */
			strcpy(call_struct.status,PENDING);
		}
		else
			strcpy(call_struct.status,UNASSGND);

		call_nbr = add_call_rec(&call_struct,&subs_struct,due_time);
		fill_call_rec(); 		/* FILL IN CALL_REC struct fields here */
		
		if(call_struct.extended_type[4] == MULT_CALL)
			generate_callh_recs();
		else
			if (write(fifo_fd,&call_fifo,sizeof(struct call_ipc_rec)) == -1) {
				prt_error(UI_WRITE,"<%s>", sys_errlist[errno]); 		/* write on fifo failed */
				return(NOT_DONE);
			}

	/* set call_nbr1 to the call_nbr just created so it gets noted as created */
		subs_struct.call_nbr1 = call_nbr;
		update_subs_rec(&call_struct,&subs_struct);
	}

	/* increment the count for statusr file */
	bump_stat(call_struct.fleet,ENTERING_CALL);

	/* No more work to be done for update */
	if (doing_update)
	{
		tmp = 0;
		if (subs_struct.call_nbr2 != 0)
		{
			temp_cisam_ptr = (struct cisam_cl *)req_buf;
			temp_cisam_ptr->nbr = subs_struct.call_nbr2;
			if ( db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL,0) == 0 )
				if (temp_cisam_ptr->due_date_time > time(NULL))
					tmp += 2;
		}
		if (subs_struct.call_nbr1 != 0)
		{
			temp_cisam_ptr = (struct cisam_cl *)req_buf;
			temp_cisam_ptr->nbr = subs_struct.call_nbr1;
			if ( db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL,0) == 0 )
				if (temp_cisam_ptr->due_date_time > time(NULL))
					tmp += 1;
		}
		if (tmp == 0)
		{
			prt_error(UI_REC_UPDATED,"");
		}
		else
		if (tmp == 1)
		{
			prt_error(UI_NO_MSG,catgets(UI_m_catd, UI_1, UI_UI_SUBS_11,
				"Sub Call has been updated.  Pending call #%d may need to be updated."),
				subs_struct.call_nbr1);
		}
		else
		if (tmp == 2)
		{
			prt_error(UI_NO_MSG,catgets(UI_m_catd, UI_1, UI_UI_SUBS_11,
				"Sub Call has been updated.  Pending call #%d may need to be updated."),
				subs_struct.call_nbr2);
		}
		else
			prt_error(UI_NO_MSG,catgets(UI_m_catd, UI_1, UI_UI_SUBS_12,
				"Sub Call has been updated.  Pending calls #%d and #%d may need to be updated."),
				subs_struct.call_nbr1, subs_struct.call_nbr2);
		return(DONE);
	}

	switch ( scr_name) {					/* clear the fields on the screen; get the max piu value */
		case SUBS_BASIC_SCR:
			max_piu = MAX_PIU_BAS_SUBS;
			ptr_scr_fld = basic_scr_flds;
			func_ptr_defs = b_prt_default;
			break;
		case SUBS_EXTENDED_SCR:
			max_piu = MAX_PIU_EXT_SUBS;
			ptr_scr_fld = extended_scr_fld;
			func_ptr_defs = e_prt_default;
			break;
		case SUBS_WAKEUP_SCR:
			max_piu = MAX_PIU_WAK_SUBS;
			ptr_scr_fld = wakeup_scr_fld;
			func_ptr_defs = w_prt_default;
			break;
		default:
			prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_UI_SUBS_4, "<called send()>"));
	}  /* end switch */

	write_dispatch = 1; 				/* write to fifio */
	write_disk = 1;					/* write to call file */
	write_subs = 0;  				/* do not write to subs file */
	

	if (send_dup_flag == NO) { 					/* if send key hit; clear all structures */
		if ( scr_name != WAKEUP_CALL_SCR  &&  scr_name != SUBS_WAKEUP_SCR ) {
			get_zone_desc(C_CLEAR, PICKUP_ADDR, &call_struct, NULL);			/* clear the zone_desc */
			get_zone_desc(C_CLEAR, DEST_ADDR, &call_struct, NULL);	

			get_est_wait_time(0); 					/* clear the estimated time field area */

			print_addr_comment(C_CLEAR,PICKUP_ADDR); 	/* clear the pkup and dest comments */
			print_addr_comment(C_CLEAR,DEST_ADDR);
		}  /* end if .. not a wakeup screen */

		attrset(A_UNDERLINE); 					/* clear all the fields */
		prt_entry_areas(stdscr,ptr_scr_fld,max_piu);

		if ( scr_name != WAKEUP_CALL_SCR  &&  scr_name != SUBS_WAKEUP_SCR ) {
			attrset(0);
			inters_flds(C_CLEAR,ptr_scr_fld);				/* clear the intersection field underlining */
			attrset(A_UNDERLINE); 					/* clear all the fields */
		}  /* end if */

		call_zoned = 0;						/* call not zoned */
		user_knows = 0;						/* user does not know */

		if ( scr_name == WAKEUP_CALL_SCR || scr_name == SUBS_WAKEUP_SCR )
			*piu_ptr = W_NAME;				/* go to the name field */
		else
			*piu_ptr = 3;					/* go to pkup address field */

		inters_flag = 0;					/* so as not to read the intersection address */
		memset((char *)&call_struct,'\0',sizeof(struct cisam_cl));
		memset((char *)&subs_struct,'\0',sizeof(struct cisam_sc));
		

		(*func_ptr_defs)(); 					/* print the default again */
		if ( scr_name == EXTENDED_CALL_SCR  ||  scr_name == SUBS_EXTENDED_SCR )
			charge_related_fields(C_CLEAR,NULL,&call_struct); 		/* clear the charge # fields, etc */

	}  								/* end if ...!send_dup_flag */
	else 								/* send & duplicate key entered */ 
		prt_error(UI_SENT," ");		 			/* inform user that call is being processed */

	return(DONE);
}  									/* end send() */

/* ============================================================================================================================	*
 * check_subs_errors: for subscription calls, no exceptions are allowed. So if a call has exceptions, then let 		*
 * the user know, never enter a call with exception. This means that subscription wakeup calls can never be entered		*
 *		Returns:													*
 *			DONE - if call has no exceptions									*
 *			NOT_DONE - if  call has exceptions									*
 * ============================================================================================================================ */
int check_subs_errors(piu_ptr)
int *piu_ptr;
{
    int zone_num;
    int zip = 0;

    /* check that at least one of the time is eneterd for at least one of the weekdays */
    if (subs_struct.mon_time <= 0 &&
	subs_struct.tue_time <= 0 &&
	subs_struct.wed_time <= 0 &&
	subs_struct.thu_time <= 0 &&
	subs_struct.fri_time <= 0 &&
	subs_struct.sat_time <= 0 &&
	subs_struct.sun_time <= 0)
    {
	/* no time entry, do nothing */
	prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_SUBS_7, "<no pickup time entry>"));
	if ( scr_name == SUBS_BASIC_SCR )
	    *piu_ptr = B_MON_TIME;
	else if ( scr_name == SUBS_EXTENDED_SCR )
	    *piu_ptr = E_MON_TIME;
	else if ( scr_name == SUBS_WAKEUP_SCR )
	    *piu_ptr = W_MON_TIME;
	
	return(NOT_DONE);	
    }
	
    /* if we are on a wakeup call entry screen; then done */
    if ( scr_name == SUBS_WAKEUP_SCR )
	return(DONE);

    /* hold line bit is to be ignored for subscription calls */
    
    /* If the user has not entered a pickup street name, put them back in the pickup street number field */
    if ( scr_name == SUBS_BASIC_SCR || scr_name == SUBS_EXTENDED_SCR )
    {
	if( call_struct.pckup_str_name[0] == '\0' )  {
	    *piu_ptr = B_PKUP_ST_NO;
	    prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_SUBS_5, "<pickup street name>"));
	    return(NOT_DONE);
	}
    }
    
    if ( call_struct.pckup_zone == 0)
    {
	zone_num = zone_addr(&call_struct, PICKUP_ADDR, &zip);
	if (zone_num <= 0)
	{
	    /* call not zoned yet; if user does not know about it, then inform him, and give him a chance to zone it */
	    prt_error(UI_NOZONE,catgets(UI_m_catd, UI_1, UI_UI_SUBS_6, "<call>")); 
	    
	    /* if user has permission to zone, then place him in the zone field */
	    if ( uiuser[user_index]->can_zone_calls == YES)
	    {
		switch ( scr_name )
		{
		case SUBS_BASIC_SCR:
		    *piu_ptr = B_PKUP_ZONE;
		    break;
		    
		case SUBS_EXTENDED_SCR:
		    *piu_ptr = E_PKUP_ZONE;
		    break;
		}
	    }

	    else
	    {
		/* user is not allowed to zone calls, place user in street name field */
		if ( scr_name == SUBS_BASIC_SCR )
		    *piu_ptr = B_PKUP_ST_NO;
		else if ( scr_name == SUBS_EXTENDED_SCR )
		    *piu_ptr = E_PKUP_ST_NO;
	    }
	    return(NOT_DONE);
	}
	
	else /* zone > 0 */
	{
	    call_struct.pckup_zone = zone_num;
	    call_zoned = 1;
	    call_fifo.status.unzoned = 0;
	}
	
    }  /* end if zone <= 0 */
    
    else  {
	/* call is already zoned; so set the bit to 0 */
	call_zoned = 1;
	call_fifo.status.unzoned = 0;
    }  /* end else */
    
    return(DONE);
    
}  /* end check_subs_errors() */

/* ============================================================================================================================	*
 * should_add_callrec: returns time call is due, if a record should be added to the call file for this subscription call, 
 * else returns 0
 * ============================================================================================================================ */
int should_add_callrec(sptr)
	struct cisam_sc *sptr;	/* pointer to cisam_sc struct */
{
	time_t nowtime;
	struct tm *tm_ptr;
	char time_buf[7];
	short formatted_time;
	short *time_ptr;
	short *time_chg_ptr;
	short time_change;
	int now_date;
	char temp_date[10];


	nowtime = time(NULL);
	tm_ptr = localtime(&nowtime);

	if (DATE_FORMAT == MONTH_FIRST_IN_DATE)
		sprintf(temp_date,"%02d%02d%02d", 
			tm_ptr->tm_mon+1, tm_ptr->tm_mday, tm_ptr->tm_year % 100 );
	else
	if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
		sprintf(temp_date,"%02d%02d%02d", 
				tm_ptr->tm_year % 100, tm_ptr->tm_mon+1, tm_ptr->tm_mday);
	else
		sprintf(temp_date,"%02d%02d%02d", 
				tm_ptr->tm_mday, tm_ptr->tm_mon+1, tm_ptr->tm_year % 100 );

	now_date = atoi(temp_date);

	if ( (sptr->from_date <= now_date) && ((sptr->to_date >= now_date) || (sptr->to_date == 0)) )  {
		/* dates entered are valid */
		sprintf(time_buf,"%02d%02d", tm_ptr->tm_hour, tm_ptr->tm_min);
		formatted_time = atoi(time_buf);
		switch ( tm_ptr->tm_wday )  {
			case 0:	/* sunday */
				time_ptr = &sptr->sun_time;
				time_chg_ptr = &sptr->sun_time_change;
				break;
			case 1:	/* monday */
				time_ptr = &sptr->mon_time;
				time_chg_ptr = &sptr->mon_time_change;
				break;
			case 2:	/* tuesday */
				time_ptr = &sptr->tue_time;
				time_chg_ptr = &sptr->tue_time_change;
				break;
			case 3:	/* wednesday */
				time_ptr = &sptr->wed_time;
				time_chg_ptr = &sptr->wed_time_change;
				break;
			case 4:	/* thursday */
				time_ptr = &sptr->thu_time;
				time_chg_ptr = &sptr->thu_time_change;
				break;
			case 5:	/* friday */
				time_ptr = &sptr->fri_time;
				time_chg_ptr = &sptr->fri_time_change;
				break;
			case 6:	/* saturday */
				time_ptr = &sptr->sat_time;
				time_chg_ptr = &sptr->sat_time_change;
				break;
		}  /* end switch */

		if ( *time_chg_ptr > 0  &&  *time_chg_ptr > formatted_time ) {
			/* this call is due for today, as specified in the time change field */
			time_change = *time_chg_ptr;
			*time_chg_ptr = 0;
			return((int)time_change);
		}  /* end if */

		if ( *time_ptr > formatted_time )  
			/* there is a call due today */
			return((int)*time_ptr);
		else
			return(0);
		
	}  /* end if ...  dates entered are valid */

	return(0);	/* dates entered are not valid */

}  /* end should_add_callrec()  */

/* add_call_rec: adds a call record for the subscription call entered ; returns call # */
add_call_rec(cptr,sptr,due_time)
	struct cisam_cl *cptr;
	struct cisam_sc *sptr;
	short due_time;	/* in hhmm format */
{

	struct cisam_cl *temp_ptr;
	int len;

	/* save info in call_struct; most of the inffo is already there */
	cptr->subsc_call_nbr = sptr->nbr;
	sprintf(cptr->due_date,"%d", sptr->from_date);
	len = strlen(cptr->due_date);
	cptr->due_date[8] = '\0';
	cptr->due_date[7] = cptr->due_date[len-1];
	cptr->due_date[6] = cptr->due_date[len-2];
	cptr->due_date[5] = DATE_SEPERATOR;
	cptr->due_date[4] = cptr->due_date[len-3];
	cptr->due_date[3] = cptr->due_date[len-4];
	cptr->due_date[2] = DATE_SEPERATOR;
	cptr->due_date[1] = cptr->due_date[len-5];
	cptr->due_date[0] = (len == 5) ? '0': cptr->due_date[0];

	get_time_from_short(due_time,cptr->due_time);

	cptr->due_date_time = get_internal_time(cptr->due_date,cptr->due_time);

	temp_ptr = (struct cisam_cl *)req_buf;
	memcpy(req_buf,(char *)cptr,sizeof(struct cisam_cl));
	if ( db(CALL_FILE_ID,ADD_REC,&cl_key1,ISEQUAL,0) < 0 ) { 	/* should write record to call file */
		prt_error(UI_WRITE,"%s <%d>", catgets(UI_m_catd, UI_1, UI_UI_SUBS_8, "call file"), iserrno); 				/* db() failed to write rec. */
		return(0);
	}

	/* record written to disk */
	cptr->nbr =  temp_ptr->nbr;
	return(cptr->nbr);	/* return call # */

}  /* end add_call_rec()  */

/* update_subs_rec: updates the subscription call record with the new call # on the stack from the call file */
update_subs_rec(cptr,sptr)
	struct cisam_cl *cptr;
	struct cisam_sc *sptr;
{
	
	struct cisam_sc *temp_ptr;

	temp_ptr = (struct cisam_sc *)req_buf;
	temp_ptr->nbr = sptr->nbr;

	if((db(SUBCALL_FILE_ID,READ_KEYS,&sc_key1,ISEQUAL+ISLOCK,0)) < 0)  {
		prt_error(UI_DSKREAD,"<%s %d>", catgets(UI_m_catd, UI_1, UI_UI_SUBS_9, "subcall record"), iserrno);
		return;
	}

	memcpy(req_buf,(char *)sptr,sizeof(struct cisam_sc));	/* req'd for possible change in 1 time change field */

	if((db(SUBCALL_FILE_ID,UPDATE_REC,&sc_key1,0,0)) < 0)  { 	/* Update the call recs on disk/memory */
		prt_error(UI_NOUPDATE,"<%s %d>", catgets(UI_m_catd, UI_1, UI_UI_SUBS_9, "subcall record"), iserrno);
		return;
	}

	if((db(SUBCALL_FILE_ID,RELEASE,&sc_key1,0,0)) < 0)  {
		prt_error(UI_FATAL,"<%s %d>", catgets(UI_m_catd, UI_1, UI_UI_SUBS_10, "cannot release lock on subcall record"), iserrno);
		return;
	}

}  /* end update_subs_rec() */

void
cancel_subs(srch_index)
     int srch_index;
{
  WINDOW  *wind;
  int     rc;
  int     start_row;
  int     c;
  struct cisam_sc *sc_ptr;
  extern struct cisam_sc subs_calls[];

  
  if (srch_index == NOT_AVAILABLE || srch_index < 0 || srch_index > calls_on_page)
    {
      prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_25, "<no call highlighted>"));
      return;
    }

  if (srch_index > 8)
    start_row = 7;
  else
    start_row = 16;

  wind = newwin(3, 82, start_row, 22);
  if (wind == NULL)
    return;

  tpak_box(wind, 0, 0);

  mvwaddstr(wind, 1, 1, catgets(UI_m_catd, UI_1, UI_TEXT_9, "Are you sure you want to remove this subscription (Y/N)? "));
  wrefresh(wind);

  while (1)
    {
      c = wgetkey(wind);
      c = Toupper(c);
      
      if (c == NO)
	{
	  delwin(wind);
	  touchwin(stdscr);
	  refresh();
	  return;
	}

      if (c == YES)
	break;

      beep();
    }

  sc_ptr = (struct cisam_sc *) req_buf;
  bzero(sc_ptr, sizeof(struct cisam_sc));
  sc_ptr->nbr = subs_calls[srch_index].nbr;

  rc = db(SUBCALL_FILE_ID, READ_KEYS, &sc_key1, ISEQUAL, 0);
  if (rc < 0)
      prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_10, "Can't delete subscription record"));
    
  else
    {
      rc = db(SUBCALL_FILE_ID, DELETE, &sc_key1, 0, 0);
      if (rc < 0)
	prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_10, "Can't delete subscription record"));
      else
	{
	  dim_line(srch_index);
	  prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_11, "Subscription record deleted"));
	}
    }

  delwin(wind);
  touchwin(stdscr);
  refresh();
}
