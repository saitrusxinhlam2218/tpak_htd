static char sz__FILE__[]="@(#)pi_call_h.c	10.1.2.1(ver) Date Release 12/2/94";
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  pi_call_h.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:45:42
 * @(#)  Last delta: 12/2/94 at 18:36:30
 * @(#)  SCCS File: /taxi/sccs/s.pi_call_h.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:ui_call_h.c	6.1.0.0"

#include <sys/types.h>

#include "mads_types.h"
#include "mad_ipc.h"
#include "mads_isam.h"
#include "except.h"
#include "df_maint.h"
#include "ui_ipc.h"
#include "language.h"
#include "pi.h"
#include "writer_strdefs.h"

#define NULL 0
extern char req_buf[];				/* for disk io */
extern struct cisam_cl call;
extern int uid;					/* users unix id # */
extern struct offsets *offset;			/* pointer to offsets struct in shared mem */
extern struct keydesc ch_key1, ch_key3;
extern struct call_ipc_rec disp_call;	
extern struct cisam_cl srch_calls[];		/* array of calls struct's */
extern int pid;
int own_isrecnum;
extern int global_msg_num;			/* Global msg# for exceptions. */

/*******************************************************************/
/* add_callh_rec: adds a call record to the disk file, for the     */
/*		exception number specified                         */
/*******************************************************************/
add_callh_rec(bit_num)
int bit_num;		/* number of the exception bit set, for which the call history record is to be added; */
			 	/* numbering starts from 0 */
{
	struct cisam_ch *ch_ptr;		/* for call history file */
	char date_str[10];			/* for date in display format */
	char time_str[10];			/* for time in display format */
	int except_desc_index;
	struct excpt_desc *ex_ptr; 		/*for getting except desc from shared mem */

	except_desc_index = bit_num;
	ex_ptr = offset->except_desc + except_desc_index; 	/* get the pointer to the exception desc slot in shared mem */

	ch_ptr = (struct cisam_ch *)req_buf;

	memset((char *)ch_ptr,'\0',sizeof(struct cisam_ch)); 	/* initialize the buffer for reading call history rec */

	ch_ptr->call_nbr = call.nbr; 	 			/* call # */
	ch_ptr->exception_nbr = (short)(except_desc_index + 1); 	/* get # of the exception */
	ch_ptr->fleet = call.fleet;
	ch_ptr->event_type = REGULAR_EVENT;					/* regular exception */
	ch_ptr->status_dt_tm = (int) time(NULL);
	get_pkup_date(date_str); 					/* get the date in display format */
	strcpy(ch_ptr->status_date,date_str);
	get_pkup_time(time_str); 					/* get the time in display format */
	strcpy(ch_ptr->status_time,time_str);
	strcpy(ch_ptr->event_desc,ex_ptr->short_desc); 			/* get the description of the exception from shared mem */
	ch_ptr->veh_nbr = 0;
	ch_ptr->drv_id = 0;
	ch_ptr->user_id = (short) uid;
									/* let all action fields be NULL's or 0's */
	if(db(CALLH_FILE_ID,ADD_REC,&ch_key1,0,0) < 0)  	/* add the record to disk */
	  ERROR("<call history record>");
}

/*******************************************************************/
/* add_callh_spec: adds a call record to the disk file, for the    */
/*		   case of ENTRY=1 or UPDATE=2                     */
/*******************************************************************/
add_callh_spec(rec_type)
short rec_type;
{
	struct cisam_ch *ch_ptr;		/* for call history file */
	char date_str[10];			/* for date in display format */
	char time_str[10];			/* for time in display format */

	ch_ptr = (struct cisam_ch *)req_buf;

	memset((char *)ch_ptr,'\0',sizeof(struct cisam_ch)); 	/* initialize the buffer for reading call history rec */

	ch_ptr->call_nbr = call.nbr; 	 			/* call # */
	ch_ptr->exception_nbr = 0;
	ch_ptr->fleet = call.fleet;
	ch_ptr->event_type = REGULAR_EVENT;					/* regular exception */
	ch_ptr->status_dt_tm = (int) time(NULL);
	get_pkup_date(date_str); 					/* get the date in display format */
	strcpy(ch_ptr->status_date,date_str);
	get_pkup_time(time_str); 					/* get the time in display format */
	strcpy(ch_ptr->status_time,time_str);

	switch (rec_type) {
		case R_ENTERED:							/* ENTERED */
			strcpy(ch_ptr->event_desc,"INSKR");
			break;
		case R_UPDATED:							/* UPDATED */
			strcpy(ch_ptr->event_desc,"UPPDAT");
			break;
		default:
			strcpy(ch_ptr->event_desc, "OK[ND");
	}

	ch_ptr->veh_nbr = 0;
	ch_ptr->drv_id = 0;
	ch_ptr->user_id = (short) uid;
									/* let all action fields be NULL's or 0's */
	if(db(CALLH_FILE_ID,ADD_REC,&ch_key1,0,0) < 0)  	/* add the record to disk */
		ERROR("<call history record>");
}

/******************************************************************************************************/
/* read_callh_rec: reads the call history record, using the arguments passed to build the key         */
/*	Returns - 0  if db() failed to read a rec						      */
/*		- 1  if record read								      */
/******************************************************************************************************/
int 
read_callh_rec(call_num,except_type)
int call_num;					/* call # of this exception */
short except_type;				/* type of exception */
{
	struct cisam_ch *ch_ptr;

	ch_ptr = (struct cisam_ch *)req_buf;
	ch_ptr->call_nbr = call_num;
	ch_ptr->exception_nbr = except_type;
	ch_ptr->status_dt_tm = 0;

	if(db(CALLH_FILE_ID,READ_KEYS,&ch_key1,ISGTEQ+ISLOCK, 0) < 0) 
          {
		ERROR("<call history record>");
          }

	while(ch_ptr->call_nbr == call_num  &&  ch_ptr->action_dt_tm != 0) { 	/* should we update this record */
										/* this record has an already resolved exception */
		if(db(CALLH_FILE_ID,RELEASE,&ch_key1,ISEQUAL,0)<0)  	/* release the write lock */
			ERROR("<cannot release lock on call history rec>");

		if(db(CALLH_FILE_ID,READ_KEYS,&ch_key1,ISNEXT+ISLOCK, 0) < 0) { 	/* try to get the right record */
			ERROR("<call history record>");
			return(0);
		}
	}

	if(ch_ptr->call_nbr != call_num) 		/* got record with wrong call # */
		return(0);
	return(1);					/* desired record is saved in req_buf[]. worked */
}

/********************************************************************************/
/* update_callh_rec: updates the call history record when resolving exceptions  */
/********************************************************************************/
update_callh_rec(action_code,veh_nbr)
char 	*action_code;			/* action code desc for exception resolution code */
int 	veh_nbr;			/* veh # entered .. for manual vehicle assignment */
{
	struct cisam_ch *ch_ptr;
	char date_str[10];
	char time_str[10];

	ch_ptr = (struct cisam_ch *)req_buf;

	strcpy(ch_ptr->action_taken,action_code);
	ch_ptr->action_dt_tm = time(NULL);
	get_pkup_date(date_str);
	strcpy(ch_ptr->action_date,date_str);
	get_pkup_time(time_str);
	strcpy(ch_ptr->action_time,time_str);
	ch_ptr->action_veh_nbr = veh_nbr;
	ch_ptr->action_drv_id = 0;
	ch_ptr->action_user_id = (short)uid;

	if(db(CALLH_FILE_ID,UPDATE_REC,&ch_key1,ISEQUAL,0) < 0)  		/* write this record to disk now */
		ERROR("<call history record>");

	if(db(CALLH_FILE_ID,RELEASE,&ch_key1,ISEQUAL,0) < 0)  		/* release the write lock */
		ERROR("<cannot release lock on call history rec>");

	return(1); 				/* record updated */
}

/*****************************************************************************************************/
/* generate_callh_recs: generates call history records for all the exceptions associated with a call */
/*****************************************************************************************************/
generate_callh_recs()
{
	int i;					/* scratch variable */
	unsigned char *bit_ptr;			/* pointer to the exception bit struct */
	unsigned char *next_ptr;		/* pointer to next bit struct */
	int size;				/* size in bytes of the bit struct */
	int bit_num;	

/*
 *    fix for mult exception type
	size = 3;				/* size of the status bit struct *
 */
	size = 4;				/* size of the status bit struct */
	bit_ptr = (unsigned char *)&disp_call.status;
	for (i = 0; i < size; i++)  {
		next_ptr = bit_ptr + i;
		if ( *next_ptr ) { 				/* some bits are set in this byte; so generate call history rec's */
			for ( bit_num=i*8; bit_num < (i+1)*8; bit_num++ )  { 	/* check if this bit is set */
				if ( test_bit(bit_num,size,bit_ptr) == 1 ) { 	/* bit # 1 is set, so add a call history rec */
					add_callh_rec(bit_num);
				}
			}
		}
	}
}

/***********************************************************************************************/
/* add_noexcpt_callh: adds a call history record for a call without exception # . ie exception */
/*		# is set to 0, eg: while assigning a vehicle on an exceptions screen           */
/***********************************************************************************************/
add_noexcpt_callh(except_index,resolve_code,action_code,veh_num)
int 	except_index;			/* index of the srch_calls[] array of structure to which this is call is */
int 	resolve_code;			/* exception resolutuion code */
char 	*action_code;			/* description of the action taken */
int 	veh_num;			/* vehicle # entered, if its manual vehicle assignment */
{
	struct cisam_ch *ch_ptr;		/* for call history file */
	int ret_code;				/* return value from db() */
	char date_str[10];			/* for date in display format */
	char time_str[10];			/* for time in display format */

	ch_ptr = (struct cisam_ch *)req_buf;

	memset((char *)ch_ptr,'\0',sizeof(struct cisam_ch)); 		/* initialize the buffer for reading call history rec */

	ch_ptr->call_nbr = srch_calls[except_index].nbr;  		/* call # */
	ch_ptr->fleet = srch_calls[except_index].fleet;

	switch(resolve_code)   { 					/* get the type  of the exception */
		case R_MAN_VEH:
			ch_ptr->exception_nbr = (short)0;		/* since not for an exception */
			break;						/* leave switch */
		case R_CALLBACK:
			ch_ptr->exception_nbr = (short)CALLBACK;	/* this is actually the exception type */
			break;
		case R_CANCEL:
			ch_ptr->exception_nbr = (short)CALL_CANCEL;	/* this is actually the exception type */
			break;
		case R_MULT:
			ch_ptr->exception_nbr = (short)MULT;
			break;
		case R_DISPATCH :
			ch_ptr->exception_nbr = (short)0;
			break;
		case R_SUP_HOLD:	
			ch_ptr->exception_nbr = (short)SUP_HOLD;
			break;
		default:
			ch_ptr->exception_nbr = (short)0;		/* since not for an exception */
			break;
	}

	ch_ptr->event_type = 'R';				/* regular function */
	ch_ptr->status_dt_tm = (int) time(NULL);

	get_pkup_date(date_str); 					/* get the date in display format */
	strcpy(ch_ptr->status_date,date_str);
	
	get_pkup_time(time_str); 					/* get the time in display format */
	strcpy(ch_ptr->status_time,time_str);

	if ( resolve_code == R_MAN_VEH ) 
		ch_ptr->veh_nbr = veh_num;
	else 
		ch_ptr->veh_nbr = 0;

	ch_ptr->drv_id = 0;
	ch_ptr->user_id = (short) uid;

	strcpy(ch_ptr->event_desc,action_code);

	if ( (ret_code = db(CALLH_FILE_ID,ADD_REC,&ch_key1,0,0)) < 0) { 		/* add the record to disk */
		ERROR("<call history record>");
	}

}  /* end add_noexcpt_callh() */

/*
  Given a call number, return the value of its last
  exception. Return -1 if not found.
*/
get_last_exception_type_from_callh(call_num)
     int call_num;
{
  struct cisam_ch *ch_ptr;
  int  rc;
  int  exception_type;


  ch_ptr = (struct cisam_ch *)req_buf;
  ch_ptr->call_nbr = call_num;
  ch_ptr->status_dt_tm = 0;
  
  rc = db(CALLH_FILE_ID, READ_KEYS, &ch_key3, ISGTEQ, 0);

  /* First check if it can find any record with wanted
     call number */
  if (rc == -1 || ch_ptr->call_nbr != call_num)
      return(-1);

  /* Step through all history in time order to find
     the last one for given call. */
  do
    {
      exception_type = ch_ptr->exception_nbr;
      rc = db(CALLH_FILE_ID, READ_KEYS, &ch_key3, ISNEXT, 0);
      /* If call number received from last db call is not the
	 given call number, then the previous db call returned
	 the last call history record for given call number. */
      if (rc != -1)
	{
	  if (ch_ptr->call_nbr != call_num)
	    return(exception_type);
	}
    }
  while (rc != -1);

  /* Must have hit end of file */
  return(exception_type);
}

/*************************************************************************************/
/* fill_call_rec: fills the call_rec structure with info from the cisam_cl struct,   */
/*		where it is saved during call_entry 				     */
/*************************************************************************************/
fill_call_rec()
{
	int i;

	disp_call.u_prcs_id = pid; 				/* copy the process id of the user */
	disp_call.rec_type = CALL_REQ; 				/* copy the type of the request to be sent */
								/* copy the message # for the call to supervisor */
								/* copy the status bits ( for exceptions ) */
	disp_call.c_isam_nbr = (int) own_isrecnum; 		/* copy the cisam_rec # */
	disp_call.call_nbr = call.nbr; 			/* copy the call # */
	disp_call.grouped_with = call.grouped_with; 	/* copy the grouped with field */
	disp_call.fleet = call.fleet; 			/* copy the fleet */
	get_call_type(); 					/* copy the call type */
	disp_call.personal_veh = call.personal_veh;	/* copy cab number if personal request */
	disp_call.personal_rqst = call.personal_rqst;	/* copy O/I */
	disp_call.pickup_zone = call.pckup_zone; 	/* copy the pickup zone */
	disp_call.dest_zone = call.dest_zone; 		/* copy the destination zone */
	disp_call.personal_veh = call.personal_veh; 	/* copy the personal veh fields */
	disp_call.msg_nbr = global_msg_num;			/* copy msg # for call with msg exception */
	strcpy(disp_call.pick_pmap,call.pckup_pmap);	
	
	for ( i = 0; i < 8; i++ )  { 			/* copy the vehicle attributes */
		if ( call.veh_attr[i] == YES )  {
			set_bit(i,sizeof(struct veh_attributes),&disp_call.veh_attr); 			/* set bit */
		}
	}

	for ( i = 0; i < 8; i++ )  { 			/* copy the driver attributes */
		if ( call.drv_attr[i] == YES )  {
			set_bit(i,sizeof(struct drv_attributes),&disp_call.drv_attr); 			/* set bit */
		}
	}

	disp_call.priority = call.priority; 		/* copy the call priority */
	disp_call.due_time = call.due_date_time; 	/* copy the due date time */
	disp_call.updated_call = 0; 				/* intialize to a new call */
	disp_call.old_due_time = 0; 	/* and no old due time */

}  								/* end fill_call_rec() */

/* get_call_type: gets the call type, after looking at the call type
 *		in call.extended_type
 *		Seta appropriate bits in disp_call.call_type
 */
get_call_type()
{

	memset((char *)&disp_call.call_type,'\0', sizeof(struct call_types));

	switch (call.extended_type[0] ) {
		case PASS:
			disp_call.call_type.passenger = 1;
			break;
		case DELIV:
			disp_call.call_type.delivery = 1;
			break;
		case TYPEX:
			disp_call.call_type.xtype = 1;
			break;
		case WAKEUP:
			disp_call.call_type.wakeup = 1;
			break;
	}  /* end switch */
			
	if(call.extended_type[4] == MULT_CALL)
		disp_call.call_type.multiple = 1;

	if ( call.extended_type[1] == '\0' )
		return;

	/* check the other char's */
	switch ( call.extended_type[2] ) {
		case '\0':
			return;
		case SUBS:
			disp_call.call_type.subscription = 1;
			break;
		case TIMEC:
			disp_call.call_type.time = 1;
			break;
		case CHARGE:
			disp_call.call_type.charge = 1;
			break;
	}  /* end switch */

	/* check the last char's */
	switch ( call.extended_type[3] ) {
		case '\0':
			return;
		case SUBS:
			disp_call.call_type.subscription = 1;
			break;
		case TIMEC:
			disp_call.call_type.time = 1;
			break;
		case CHARGE:
			disp_call.call_type.charge = 1;
			break;

	}  /* end switch */

}  /* end get_call_type() */

add_mult_h(call_nbr, fleet)
int	call_nbr;
char	fleet;
{
	struct cisam_ch *ch_ptr;					/* for call history file */
	char date_str[10];						/* for date in display format */
	char time_str[10];						/* for time in display format */

	ch_ptr = (struct cisam_ch *)req_buf;

	memset((char *)ch_ptr,'\0',sizeof(struct cisam_ch)); 		/* initialize the buffer for reading call history rec */

	ch_ptr->call_nbr = call_nbr;  					/* call # */
	ch_ptr->fleet = fleet;

	ch_ptr->exception_nbr = (short)CALL_CANCEL;			/* this is actually the exception type */
	ch_ptr->event_type = 'R';				/* regular function */
	ch_ptr->status_dt_tm = (int)time(NULL);
	get_pkup_date(date_str); 					/* get the date in display format */
	strcpy(ch_ptr->status_date, date_str);
	
	get_pkup_time(time_str); 					/* get the time in display format */
	strcpy(ch_ptr->status_time, time_str);

	ch_ptr->veh_nbr = 0;
	ch_ptr->drv_id = 0;
	ch_ptr->user_id = (short)uid;

	strcpy(ch_ptr->event_desc, catgets(WRITER_catd, 1, 40, "AVBEST  "));

	if(db(CALLH_FILE_ID, ADD_REC, &ch_key1, 0, 0) < 0)  		/* add the record to disk */
		ERROR("<call history record>");
}
