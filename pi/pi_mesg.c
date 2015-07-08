static char sz__FILE__[]="@(#)pi_mesg.c	10.1.2.1(ver) Date Release 12/2/94";
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  pi_mesg.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:46:05
 * @(#)  Last delta: 12/2/94 at 18:52:44
 * @(#)  SCCS File: /taxi/sccs/s.pi_mesg.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:mesg_funcs.c	6.13.0.0"

#include <stdio.h>
#include <isam.h>
#include <sys/types.h>
#include <string.h>

#include "mads_types.h"
#include "mads_isam.h"
#include "mad_ipc.h"
#include "df_maint.h"
#include "ex_key.h"
#include "user.h"
#include "mesg.h"
#include "ui_ipc.h"
#include "ui_error.h"
#include "path.h"
#include "language.h"
#include "ui_strdefs.h"
#include "pi.h"
	
extern int fifo_fd;							/* file descriptor for named pipe to dispatch */
extern int pid;								/* process id of the USER INTERFACE process */
extern int uid;								/* user id of operator signed on to this terminal */
extern int user_index;							/* index into user array in shared memory */
extern struct users *uiuser[];						/* pointer to user profile array in shared memory */
extern struct fleets *fleet[];						/* pointer to fleet array */
extern char req_buf[];							/* request buffer for database reads */
int message_sent;							/* flag set if message sent to a destination */
int cur_time;								/* current time in C format */
struct recv_msg r_msg;							/* receive structure for returning to User process */

extern int global_msg_num;	/* for CALL_W_MSG exception */
struct cisam_mb Current_message;
extern struct file_desc isam_file[];

/***********************************************************************************************************************************
message_send -
This routine is called by the PI-interface to create a message and send it to the appropriate destinations.
The message is typed in by the user and passed to this routine by way of a pointer to a Cisam repeat message structure.  This
structure is used to pass the message because it has corresponding fields for all the fields on the message screen.  The message
may not necessarily be an actual repeat message.  The repeat field must be checked to determine this.  If the message is a repeat
message, a record will be added to the repeat message file, otherwise one is added to the message log file.  The type and selection
fields are tested to determine the destination(s) for this message.  If the destination(s) include dispatch center users, a mailbox
record will be added for those users after they have been verified against the user profile file.  If the destination(s) include
any driver or vehicle, then a message is sent down the named pipe to notify DISPATCH to route the message on to the proper place.
***********************************************************************************************************************************/
	
message_send(msg)
struct cisam_rm *msg;							/* pointer to repeat message structure with message */
{
    	struct cisam_ml	*ml_ptr;					/* pointer to message log buffer */
    	struct cisam_rm *rm_ptr;					/* pointer to repeat message pointer */
	int  rc;
	int  msg_num;
	char msg_flag;
        int return_code = 0;

	
    	ml_ptr = (struct cisam_ml *)req_buf;				/* initialize pointer to request buffer */
    	rm_ptr = (struct cisam_rm *)req_buf;				/* initialize pointer to request buffer */
	
    	cur_time = get_time();						/* get current time */
    	message_sent = 0;

    	if(msg->repeat == YES) {
		if(db(RPTMSG_FILE_ID,OPEN,&rm_key1,ISINOUT+ISMANULOCK,0) < 0) {	
	    		ERROR("Error opening file - repeat-messages");	
	    		return(-92);
		}

		/* fill in repeat message info for new record to add */
		rm_ptr->created_by = msg->created_by;		
		rm_ptr->created_dt_tm = cur_time;				
		get_asc_time(cur_time, rm_ptr->cr_date, rm_ptr->cr_time);	
		rm_ptr->cr_date[8] = '\0';					
		rm_ptr->cr_time[6] = '\0';					
		rm_ptr->repeat = msg->repeat;					
		rm_ptr->fleet = msg->fleet;					
		rm_ptr->to_type = msg->to_type;					
		strncpy(rm_ptr->to_selection, msg->to_selection, 6);		
		rm_ptr->to_selection[6] = '\0';
		rm_ptr->how_often = msg->how_often;				
		rm_ptr->until_dt_tm = msg->until_dt_tm;				
		get_asc_time(rm_ptr->until_dt_tm, rm_ptr->until_date, rm_ptr->until_time);	
		rm_ptr->until_date[8] = '\0';					
		rm_ptr->until_time[6] = '\0';					
		strcpy(rm_ptr->line_1, msg->line_1);			
		strcpy(rm_ptr->line_2, msg->line_2);			

		if(db(RPTMSG_FILE_ID,ADD_REC,&rm_key1,0,0) < 0) {		/* add new record */
	    		ERROR("Error adding record to file - repeat-messages");	
	    		db(RPTMSG_FILE_ID,CLOSE,&rm_key1,0,0);
	    		return(-92);
		}
		msg_num = rm_ptr->msg_nbr;					/* save record number assigned by Cisam */
		if(db(RPTMSG_FILE_ID,CLOSE,&rm_key1,0,0) < 0) {		/* close repeat message file */
	    		ERROR("Error closing file - repeat-messages");	
	    		return(-92);
		}

		/* Repeat messages must go in message log too */
		rc = add_to_msg_log(msg, cur_time);
		if (rc == -1)
		  return(-92);
		msg_flag = MSG_RPT;
    	}
    	else if(msg->repeat == NO) {
	  msg_num = add_to_msg_log(msg, cur_time);
	  if (msg_num == -1)
	    return(-92);
	  global_msg_num = msg_num;
	  msg_flag = MSG_LOG;
    	}
    	else {
	/* ERROR: Invalid repeat field */
		return(-92);							/* unknown repeat flag value */
    	}

    	switch(msg->to_type) {						/* determine destination(s) */
		case '*':	
			return_code = to_all_users(msg, msg_num, msg_flag);
			break;

	case MSG_FLEET:
	  send_msg_to_dispatch(msg_num, msg_flag);
	  break;

		case MSG_SUPERVISOR:
		case MSG_CALL_TAKER:
		case MSG_MANAGER:
			return_code = to_users_type(msg);
			break;

		case MSG_USER:
			return_code = to_user_users(msg);
			break;

		case MSG_VEHICLE:
		case MSG_DRIVER:
		case MSG_ZONE:
	case 'Q': // HTD MPK originated

			return_code = to_mdt(msg, msg_num, msg_flag);
			break;

	        default:
			break;
    	}

        if ( return_code < -90 )
    	  return(return_code);

	if (!message_sent)
	  {
	    return(-92);
	  }

        return( msg_num );
}

add_to_msg_log(msg, cur_time)
     struct cisam_rm *msg;
{
  struct cisam_ml  *ml_ptr;

  if (db(MSGLOG_FILE_ID, OPEN, 0, ISINOUT + ISMANULOCK, 0) < 0)
    {
      ERROR("Error opening file - message-log");	
      return(-1);
    }
  
  ml_ptr = (struct cisam_ml *) req_buf;
  bzero(ml_ptr, sizeof(struct cisam_ml));

  /* fill in message log info for new record to add */
  ml_ptr->from = msg->created_by;
  ml_ptr->date_time_sent = cur_time;				
  get_asc_time(cur_time, ml_ptr->date_sent, ml_ptr->time_sent);	
  ml_ptr->date_sent[9] = '\0';					
  ml_ptr->time_sent[6] = '\0';					
  ml_ptr->supervisor_id = 0;					
  ml_ptr->supervisor_saw = '\0';					
  ml_ptr->repeat = msg->repeat;
  ml_ptr->fleet = msg->fleet;					
  ml_ptr->to_type = msg->to_type;					
  strncpy(ml_ptr->to_selection, msg->to_selection,6);		
  ml_ptr->to_selection[6] = '\0';
  strcpy(ml_ptr->msg_line1, msg->line_1);			
  strcpy(ml_ptr->msg_line2, msg->line_2);
  strcpy(ml_ptr->msgtext, msg->msgtext);
  
  if (db(MSGLOG_FILE_ID, ADD_REC, &ml_key1, 0, 0) < 0)
    {
      ERROR("Error adding record file - message-log");	
      db(MSGLOG_FILE_ID, CLOSE, 0, 0, 0);
      return(-1);
    }

  db(MSGLOG_FILE_ID, CLOSE, 0, 0, 0);
  return(ml_ptr->nbr);
}
  
/***********************************************************************************************************************************
to_all_users -
This routine is called when a message is to be sent to all users.  First the in-house users each get a copy of the message, and
then the drivers get sent a message.
***********************************************************************************************************************************/
to_all_users(msg, msg_num, msg_flag)
struct cisam_rm *msg;
int  msg_num;
char msg_flag;
{
    all_in_house(msg);							/* send to all in house */
    send_msg_to_dispatch(msg_num, msg_flag);				/* send to all drivers */
}



/***********************************************************************************************************************************
all_in_house -
This routine reads all records from the user profile file and creates a mailbox message for every user it finds.
***********************************************************************************************************************************/
all_in_house(msg)
struct cisam_rm *msg;							/* message as entered by user */
{
    int user_num;							/* user to send message to */
    int fleet_index;
    struct cisam_up *up_ptr;						/* pointer to user profile buffer */

    up_ptr = (struct cisam_up *)req_buf;				/* initialize pointer to request buffer */

    if (db(USER_FILE_ID,OPEN,&up_key1,ISINOUT+ISMANULOCK,0) < 0) {	/* open user profile file */
        ERROR("Error opening file - USER");	
	return(-1);
    }
    if (db(MAILBOX_FILE_ID,OPEN,&mb_key1,ISINOUT+ISMANULOCK,0) < 0) {	/* open mailbox file */
        ERROR("Error opening file - MAILBOX");	
	db(USER_FILE_ID,CLOSE,&up_key1,0,0);
	return(-1);
    }
    if (db(USER_FILE_ID,READ_KEYS,&up_key1,ISFIRST,0) < 0) {		/* read first user record */
	/* ERROR: USERS not found */
	db(USER_FILE_ID,CLOSE,&up_key1,0,0);
	db(MAILBOX_FILE_ID,CLOSE,&mb_key1,0,0);
	return(-1);
    }

    /* if there is a fleet to choose, it will
       be valid since we already verified it
       in val_fleet() */
    fleet_index = find_findex(msg->fleet);

    do {
      /* If going to all fleets or the user has
	 permission to use the chosed fleet, send
	 message to the user */
      if (msg->fleet == '*' ||
	  (up_ptr->fleet_perms[fleet_index] == YES))
	one_user(msg, up_ptr->user_id);
    } while (db(USER_FILE_ID,READ_KEYS,&up_key1,ISNEXT,0) >= 0);	/* get next user record */

    if (db(USER_FILE_ID,CLOSE,&up_key1,0,0) < 0)			/* close user profile file */
        ERROR("Error opening file - USER");	
    if (db(MAILBOX_FILE_ID,CLOSE,&mb_key1,0,0) < 0)			/* close mailbox file */
        ERROR("Error closing file - MAILBOX");	
}



/*------------------------------------------------------------------------
 *	one user 
 *		This routine sends the message to a single user.  A mailbox record is 
 *		created for that user and it's status is set to New.
 *------------------------------------------------------------------------
 */

one_user(msg,id)
	struct cisam_rm *msg;
	int id;
{
    struct cisam_mb *mb_ptr;	

    if (!msg)
      {
	ERROR("Null message pointer in one_user()");
	return(-1);
      }

    mb_ptr = (struct cisam_mb *)req_buf;

    mb_ptr->address = id;			
    mb_ptr->from = msg->created_by;	

    mb_ptr->date_time_sent = cur_time;
    get_asc_time(cur_time, mb_ptr->date_sent, mb_ptr->time_sent);	
    mb_ptr->date_sent[9] = '\0';					
    mb_ptr->time_sent[6] = '\0';					

    strncpy(mb_ptr->msg_line1,msg->line_1,100);	
    mb_ptr->msg_line1[100] = '\0';			
    strncpy(mb_ptr->msg_line2,msg->line_2,100);
    mb_ptr->msg_line2[100] = '\0';			
    mb_ptr->status = NEW_MSG;			

    mb_ptr->fleet = msg->fleet;
    mb_ptr->to_type = msg->to_type;
    strcpy(mb_ptr->to_selection, msg->to_selection);

    if (db(MAILBOX_FILE_ID,ADD_REC,&mb_key1,0,0) < 0)
      {
        ERROR("Error adding mailbox record");
      }
    else 
      {
	change_message_count(id, 1);			/* increment counter */
	message_sent = 1;
      }

    return(0);
}


to_mdt(msg, msg_num, msg_flag)
struct cisam_rm *msg;
int  msg_num;
char msg_flag;								/* MSG_LOG, MSG_RPT, MSG_UPD, MSG_DEL */
{
  int  i;
  int  zone_num;
  int  found;
  struct cisam_vh *vh_ptr;
  struct cisam_dr *dr_ptr;
  struct cisam_zn *zn_ptr;

  switch (msg->to_type)
    {
    case MSG_VEHICLE:
    case 'Q':
      if (msg->to_selection[0] != '*')
	{
	  vh_ptr = (struct cisam_vh *)req_buf;
	  vh_ptr->fleet = msg->fleet;
	  vh_ptr->nbr = atoi(msg->to_selection);
	  
	  if (db(VEHICLE_FILE_ID, READ_KEYS, &veh_key2, ISEQUAL, 0) < 0)
	    {
	/* ERROR: TAXI not found */
	      return(-91);
	    }
	}
      break;

    case MSG_DRIVER:
      if (msg->to_selection[0] != '*')
	{
	  dr_ptr = (struct cisam_dr *)req_buf;
	  dr_ptr->fleet = msg->fleet;			
	  dr_ptr->id = atoi(msg->to_selection);
		
	  if (db(DRIVER_FILE_ID, READ_KEYS, &dr_key1, ISEQUAL,0) < 0)
	    {
	/* ERROR: DRIVER not found */
	      return(-91);
	    }
	}
      break;

    case MSG_ZONE:
      zone_num = atoi(msg->to_selection);
      zn_ptr = (struct cisam_zn *) req_buf;
      if (msg->fleet != '*')
	{
	  zn_ptr->fleet = msg->fleet;
	  zn_ptr->nbr = zone_num;
	  if (db(ZONE_FILE_ID, READ_KEYS, &zn_key1, ISEQUAL, 0) < 0)
	    {
	/* ERROR: ZONE not found */
	      return(-91);
	    }
	}

      else 
	{
	  found = 0;
	  for (i = 0; i < FLEET_MAX; i ++)
	    {
	      if (fleet[i]->active == YES)
		{
		  zn_ptr->fleet = fleet[i]->fleet_id;;
		  zn_ptr->nbr = zone_num;
		  if (db(ZONE_FILE_ID, READ_KEYS, &zn_key1, ISEQUAL, 0) >= 0)
		    {
		      found = 1;
		      break;
		    }
		}
	    }
	  if (!found)
	    {
	/* ERROR: ZONE not found */
	      return(-91);
	    }
	}

      break;

    default:
      break;
    }

  send_msg_to_dispatch(msg_num, msg_flag);
}

/*****************************************************************
send_msg_to_dispatch()
This routine sends a message to the named pipe for DISPATCH process.
From the content of the message, dispatch determines what to do with it.
******************************************************************/
send_msg_to_dispatch(msg_num, msg_flag)
int  msg_num;
char msg_flag;								/* MSG_LOG, MSG_RPT, MSG_UPD, MSG_DEL */
{
  struct drv_msg dmsg;							/* pointer to structure being sent down pipe */
  int rc;
  int i;

  if (msg_flag != MSG_LOG &&
      msg_flag != MSG_RPT &&
      msg_flag != MSG_UPD &&
      msg_flag != MSG_DEL)
    return;

  /* fill in pipe structure with info from message */
  dmsg.u_prcs_id = pid;
  dmsg.rec_type = DRV_REQ;
  dmsg.msg_nbr = msg_num;
  dmsg.msg_flag = msg_flag;

  /* Only Dispatchers or managers can send messages immediately.
     Call takers will create an exception that a
     dispatcher must approve before the message is sent. Only
     new messages will generate an exception. Since Call takers
     cannot send repeat messages, no need for a dispatcher to
     approve them. */
  if (msg_flag == MSG_LOG &&
      (uiuser[user_index]->menu_perms.exceptions || uiuser[user_index]->menu_perms.system_maint))
    dmsg.dispatch_flag = 0;
  else
    dmsg.dispatch_flag = 1;

  rc = write(fifo_fd, (char *) &dmsg, sizeof(struct drv_msg));
  if (rc < sizeof(struct drv_msg))
    {
      ERROR("Error writing to pipe");
    }
  else
    message_sent = 1;
}

to_users_type(msg)
     struct cisam_rm *msg;  /* message as entered by user */
{
    int  i;
    int  *user_list;
    int *find_users_of_one_type();


    if (db(MAILBOX_FILE_ID, OPEN, 0, ISINOUT + ISMANULOCK, 0) < 0) {
	ERROR("Error opening file - MAILBOX");
	return(-92);
    }

    user_list = find_users_of_one_type(msg->to_type);
    if (user_list == NULL)
      {
	/* ERROR: No USER of given TYPE */
	return(-91);
      }
    
    /* Send a message to each user found of desired type */
    for (i = 0; user_list[i] != -1; i ++)
      one_user(msg, user_list[i]);

    db(MAILBOX_FILE_ID, CLOSE, 0, 0, 0);

    if (!message_sent)
	;
	/* ERROR: Error adding MAILBOX record */
}

/***********************************************************************************************************************************
to_user_users-
This routine sends a message to a specific in-house user.  The user is selected by the user id.  If a '*' 
is used in the selection, the message is sent to all users that match the first part of the selection before the '*'.
***********************************************************************************************************************************/
to_user_users(msg)
struct cisam_rm *msg;							/* message as entered by user */
{
    struct cisam_up *up_ptr;						/* pointer to user profile buffer */


    if (msg->to_selection[0] == '*')					/* does this go to all in-house users? */
	all_in_house(msg);						/* yes, send to all in-house */

    else {
        up_ptr = (struct cisam_up *)req_buf;				/* initialize pointer to request buffer */
	if (db(USER_FILE_ID,OPEN,&up_key1,ISINOUT+ISMANULOCK,0) < 0) {	/* open user profile file */
	    ERROR("Error opening file - USER");		/* if problem, ERROR */
	    return(-92);
	}
	if (db(MAILBOX_FILE_ID,OPEN,&mb_key1,ISINOUT+ISMANULOCK,0) < 0) {	/* open mailbox file */
	    ERROR("Error opening file - MAILBOX");		/* if problem, ERROR */
	    db(USER_FILE_ID,CLOSE,&up_key1,0,0);			/* close user file too */
	    return(-92);
	}
	up_ptr->user_id = atoi(msg->to_selection);
	if (db(USER_FILE_ID,READ_KEYS,&up_key1,ISEQUAL,0) < 0) {	/* read record */
	/* ERROR: USER not found */
	    db(USER_FILE_ID,CLOSE,&up_key1,0,0);			/* close user profile file */
	    db(MAILBOX_FILE_ID,CLOSE,&mb_key1,0,0);			/* close mailbox file */
	    return(-92);
	}

	one_user(msg,up_ptr->user_id);					/* send to this user */

	if (db(USER_FILE_ID,CLOSE,&up_key1,0,0) < 0)			/* close user profile file */
	    ERROR("Error closing file - USER");
	if (db(MAILBOX_FILE_ID,CLOSE,&mb_key1,0,0) < 0)			/* close mailbox file */
	    ERROR("Error closing file - MAILBOX");
    }
    if (!message_sent)
	;
	/* ERROR: USER not found */
}
/***********************************************************************************************************************************
message_count -
This routine is called when a user logs on to initially set up the count of unread messages in his/her user profile in shared
memory.
***********************************************************************************************************************************/
message_count(usid)
int usid;									/* user id */
{
    int count;									/* message count */
    struct cisam_mb *mb_ptr;							/* pointer to mailbox buffer */

    mb_ptr = (struct cisam_mb *)req_buf;					/* initialize pointer to request buffer */
    count = 0;									/* intialize count to zero */

    if (db(MAILBOX_FILE_ID,OPEN,&mb_key2,ISINOUT+ISMANULOCK,0) < 0) {		/* open mailbox file */
	ERROR("Error opening file - MAILBOX");				/* if problem, ERROR */
	return(0);
    }
    mb_ptr->address = usid;							/* fill in key for this user */
    mb_ptr->status = NEW_MSG;
    mb_ptr->date_time_sent = 0;

    if (db(MAILBOX_FILE_ID,READ_KEYS,&mb_key2,ISGTEQ,0) < 0) {			/* read mailbox record */
	db(MAILBOX_FILE_ID,CLOSE,&mb_key2,0,0);
	return(0);
    }
    while ((mb_ptr->address == usid) && (mb_ptr->status == NEW_MSG)) {		/* count all new messages for this user */
	count += 1;								/* add this one */
	if (db(MAILBOX_FILE_ID,READ_KEYS,&mb_key2,ISNEXT,0) < 0)		/* read next record */
	    break;
    }
    if (db(MAILBOX_FILE_ID,CLOSE,&mb_key2,0,0) < 0)				/* close mailbox file */
	ERROR("Error closing file - MAILBOX_FILE");				/* if problem, ERROR */
    return(count);
}
/******************
change_message_count -
Alter the value in shared memory of the number of
message a user has.
*******************/
change_message_count(user_id, amount)
int user_id;
int amount;
{
  int  i;

  /* Go through all the user memory
     case a user is logged in more than once */
  for (i = 0; i < USER_MAX; i ++)
    {
      if (uiuser[i]->user_id == user_id)
	{
	  uiuser[i]->cnt_msg += amount;
	  if (uiuser[i]->cnt_msg < 0)
	    uiuser[i]->cnt_msg = 0;
	}
    }
}

int *
find_users_of_one_type(user_type)
     char user_type;
{
  struct cisam_up *user_ptr;
  int  i;
  static int *user_list = NULL;
  static size_of_user_list = 0;

  if (user_type != MSG_SUPERVISOR &&
      user_type != MSG_CALL_TAKER &&
      user_type != MSG_MANAGER)
    return(NULL);

  user_ptr = (struct cisam_up *) req_buf;

  /* open the user's profile file on disk */
  if (db(USER_FILE_ID, OPEN, 0, ISINOUT + ISMANULOCK, 0) < 0)
      return(NULL);

  /* read this user's record from the user profile file */
  if(db(USER_FILE_ID, READ_KEYS, &up_key1, ISFIRST, 0) < 0)
    {
      /* db failed to read record from disk */
      db(USER_FILE_ID, CLOSE, 0, 0, 0);
      return(NULL);
    }

  if (user_list == NULL)
    {
      size_of_user_list = 10;
      user_list = (int *) malloc(sizeof(int) * size_of_user_list);
    }

  i = 0;
  while (iserrno == 0)
    {
      /* Expand list if necessary. Need room on end for end of list marker (-1)
	 Note that the list will never decrease in size, but it will be small enough
	 to take up not much memory */
      if (i == size_of_user_list - 1)
	{
	  /* Increase size of list by 1/3 */
	  size_of_user_list = (size_of_user_list * 4) / 3 + 1;
	  user_list = (int *) realloc(user_list, sizeof(int) * size_of_user_list);
	}

      /* The type of user is determined by their permissions on the
	 exceptions and and system maint screens */
      switch (user_type)
	{
	case MSG_SUPERVISOR:
	  if (user_ptr->menu_perms[22] == YES	/* Exception  Screen */
	      && user_ptr->menu_perms[5] == NO)	/* Sytem Maint */
	    user_list[i ++] = user_ptr->user_id;
	  break;
	      
	case MSG_CALL_TAKER:
	  if (user_ptr->menu_perms[22] == NO	/* Exception  Screen */
	      && user_ptr->menu_perms[5] == NO)	/* Sytem Maint */
	    user_list[i ++] = user_ptr->user_id;
	  break;
	      
	case MSG_MANAGER:
	  if (user_ptr->menu_perms[5] == YES)	/* Sytem Maint */
	    user_list[i ++] = user_ptr->user_id;
	  break;
	      
	default:
	  break;
	}
      db(USER_FILE_ID, READ_KEYS, &up_key1, ISNEXT, 0);
    }
	    
  db(USER_FILE_ID, CLOSE, 0, 0, 0);

  /* Mark end of user list */
  user_list[i] = -1;

  return(user_list);
}

/***********************************************************************************************************************************
message_apprvd_deny -
This routine is called when a supervisor is to approve/deny an exception or when a call is routed to a supervisor with a message
attached.
***********************************************************************************************************************************/
message_apprvd_deny(ml_nbr,yes_no_saw)
int ml_nbr;									/* message log number */
char yes_no_saw;								/* Yes No Saw flag */
{
    struct cisam_ml *ml_ptr;							/* pointer to message log buffer */

    ml_ptr = (struct cisam_ml *)req_buf;					/* initialize pointer to request buffer */

    if (db(MSGLOG_FILE_ID,OPEN,&ml_key1,ISINOUT+ISMANULOCK,0) < 0) {		/* open message log file */
	ERROR("Error opening file - MSGLOG");				/* if problem, ERROR */
	return(-1);
    }
    ml_ptr->nbr = ml_nbr;							/* fill key with message log number */
    if (db(MSGLOG_FILE_ID,READ_KEYS,&ml_key1,ISEQUAL+ISLOCK,0) < 0)		/* read message log record with lock */
	ERROR("Error reading record from message # passed - MSGLOG")	/* if problem, ERROR */
    else {
	ml_ptr->supervisor_id = uid;						/* update fields of message log record */
	ml_ptr->supervisor_saw = yes_no_saw;
	if (db(MSGLOG_FILE_ID,UPDATE_REC,&ml_key1,0,0) < 0)			/* update record */
	    ERROR("Error updating record - MSGLOG");			/* if problem, ERROR */
	if (db(MSGLOG_FILE_ID,RELEASE,&ml_key1,0,0) < 0)			/* release lock */
	    ERROR("Error releasing record - MSGLOG");			/* if problem, ERROR */
    }
    if (db(MSGLOG_FILE_ID,CLOSE,&ml_key1,0,0) < 0)				/* close message log file */
	ERROR("Error closing file - MSGLOG");				/* if problem, ERROR */
}


