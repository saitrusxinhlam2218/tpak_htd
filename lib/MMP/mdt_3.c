/*  @@(#)  $RCSfile: mdt_3.c,v $
*  @@(#)  $Revision: 2.0 $
*  @@(#)  $Date: 2012/08/28 13:29:51 $
*  @@(#)  $Author: taxiprod $
*  @@(#)  $Source: /usr/users/jwelch/cvs_g7/taxi-g7/lib/MMP/mdt_3.c,v $ */
static char rcsid[] = "$Id: mdt_3.c,v 2.0 2012/08/28 13:29:51 taxiprod Exp $";

/* Based on version 6.29 */

/*****************************************************************************
 *
 *		mdt_interface()(void)		- Process messages from ACC.
 *		nak_handler()(void)		- Process NAK messages.
 *              nak_error()(void)               - Prints error messages to error file.
 *              nak_by_mtype()(void)            - Handle NAK for specific messages.
 *              nak_msg_err()(void)             - Processes improper messages.
 *              form_hndl()(void)               - Process Form Messages.
 *              local_form_hndl()(void)         - Processes Inbound Local Forms.
 *              func_hndl()(void)               - Processes Inbound Function Messages.
 *              can_msg_hndl()(void)            - Processes Inbound Can Messages.
 *              ext_inp_hndl()(void)            - Processes External Input Messages.
 *
 *****************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <memory.h>
#include <string.h>
#include <time.h>

#include "taxipak.h"
#include "Object.h"
#include "Fleet_public.h"
#include "Fleet_lists_public.h"
#include "Fleet_private.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "Call_public.h"
#include "Call_private.h"
#include "Call_lists_public.h"
#include "dispatch_strdefs.h"
#include "mad_error.h"
#include "mdt.h"
#include "mad_ipc.h"
#include "except.h"
#include "sprintf.h"
#include "credit.h"
#include "language.h"
#include "path.h"
#include "msg_mmp.h"
#include "itm.h"
#include "ui_ipc.h"

void  nak_handler(MSG_QUEUE_HDR *, struct veh_driv *);	/* Processes NAK errors */
void  nak_error(MSG_QUEUE_HDR *, char *);	/* Writes out error messages */
void  local_form_hndl(MSG_QUEUE_HDR *, struct veh_driv *, char *);	/* Processes Inbound
									 * Local Forms */
void  form_hndl(MSG_QUEUE_HDR *, struct veh_driv *, char *);	/* Processes Inbound
								 * Forms */
void  func_hndl(MSG_QUEUE_HDR *, struct veh_driv *, char *);	/* Processes Inbound
								 * Function Messages */
void  ext_inp_hndl(MSG_QUEUE_HDR *, struct veh_driv *, char *);	/* Processes External
								 * Input Messages */
void  can_msg_hndl(MSG_QUEUE_HDR *, struct veh_driv *, char *);	/* Processes Inbound Can
								 * Messages */
void  nak_msg_err(MSG_QUEUE_HDR *);	/* Processes Improper Messages */
void  nak_by_mtype(MSG_QUEUE_HDR *, struct veh_driv *);
void  mdt_trans_stats(MSG_QUEUE_HDR *);

extern char req_buf[];		/* request buffer for db */
extern int incount;
#ifdef DEBUG
#ifdef DUAL_PORTS
extern int incount_2;
#endif
#endif
#ifndef MSGQ_
#ifdef DUAL_PORTS
extern inb_fd_2;
#endif
extern int inb_fd;
#endif

int   nak_cnt = 0;

/* tuneable parameters */
#define MAX_READ_COUNT  10	/* how often in async read to ui read */
#define MAX_READ_SLEEP_COUNT  10	/* when to sleep in asyn read count less than */

MSG_QUEUE_HDR inb_msg[MAX_MSG_READ];	/* Holds inbound messages */
#define MAX_ERROR_STR_LEN   40	/* approx # of characters available in db
				 * error record */

void
mk_nak_error(inb_msg, reason)
   MSG_QUEUE_HDR *inb_msg;
   char  reason[];
{
   char  p_fields[P_FLD_SIZE + 1];
   char  mid[MID_SIZE + 1];
   char  pkt_id[3];
   char  str_temp[80];

   strncpy(p_fields, inb_msg->p_fields, P_FLD_SIZE);
   p_fields[P_FLD_SIZE] = '\0';
   strncpy(pkt_id, inb_msg->pkt_id, 2);
   pkt_id[2] = '\0';
   strncpy(mid, inb_msg->route_id, MID_SIZE);
   mid[MID_SIZE] = '\0';
   sprintf(str_temp, "%s mid %s pkt %s type %c pfld %s",
	   reason, mid, pkt_id, inb_msg->type, p_fields);
   ERROR(' ', "", str_temp);
   if (strlen(inb_msg->text) != 0)
   {
      if (strlen(inb_msg->text) > MAX_ERROR_STR_LEN)
	 inb_msg->text[MAX_ERROR_STR_LEN] = '\0';	/* get only part of it */
      sprintf(str_temp, "inb_msg text %s", inb_msg->text);
      ERROR(' ', "", str_temp);
   }
}				/* end mk_nak_error */

/*****************************************************************************
 *
 *===>	mdt_interface()(void) 		- Process messages from ACC.
 *
 * 	DESCRIPTION:
 *		The following function is the main process loop for getting
 *	messages off the the IPC message queue and processing them.
 *
 * 	PARMETERS:
 *		None.
 *
 *	GLOBALS:
 *		recv_msgqid(int)		- Message queue to ACC.
 *		mdt_msgs_max(int) 		- Max. # of KDT messages read in.
 *		offset(struct offsets *) 	- Shared Memory Pointer.
 *
 *	CALLS:
 *	==>	get_veh_id()(struct veh_driv *)	- Returns veh_ptr from term_id.
 *	==>	nak_handler()(void)		- Process NAK messages.
 *	==>	form_hndl()(void)		- Process Form Messages.
 *	==>	func_hndl()(void) 		- Processes Inbound Function Messages.
 *	==>	/ak_msg_err()(void)		- Processes improper messages.
 *
 *	RETURN:
 *		None.
 *
 *****************************************************************************/

void
mdt_interface()
{
   short count;			/* Buffer counter */
   struct veh_driv *veh_ptr;	/* Vehicle Pointer */
   short mindex;		/* Index of message to be processed in
				 * inb_msg array */
   char  term_id[ROUTE_ID_SIZE + 1];	/* RFID in ASCI format */

   extern int recv_msgqid;	/* Message queue to ACC */
   extern struct fleets *fleet[];	/* Global fleet structure */
   extern int mdt_msgs_max;	/* Max. # of KDT messages read in */
   int   rf_id;

   memset(inb_msg, '\0', sizeof(inb_msg));	/* Initialize buffer */


   count = 0;			/* Initialize buffer counter */
#ifdef DUAL_PORTS
   while (count < mdt_msgs_max)
   {				/* Loop for max. # of messages to process */
      if ((count % MAX_READ_COUNT) == 0)	/* UI */
      {
	 CheckMsgQ();		/* check for monitor token tek */
      }
      if (get_inb_msg(inb_fd, &inb_msg[count]) == -1)
	 break;			/* No more messages */
      count++;			/* To store next message */
      incount++;
   }
   while (count < mdt_msgs_max)
   {				/* Loop for max. # of messages to process */
      if ((count % MAX_READ_COUNT) == 0)	/* UI */
      {
	 CheckMsgQ();		/* check for monitor token tek */
      }
      if (get_inb_msg(inb_fd_2, &inb_msg[count]) == -1)
	 break;			/* No more messages */
      count++;			/* To store next message */
      incount_2++;
   }
#else
   while (count < mdt_msgs_max)
   {				/* Loop for max. # of messages to process */

      /* Read in message and store */
#ifndef MSGQ_
      if ((count % MAX_READ_COUNT) == 0)	/* UI */
      {
	 CheckMsgQ();		/* check for monitor token tek */
      }
      if (get_inb_msg(&inb_msg[count]) == -1)
#else
      if (inb_msgrcv(recv_msgqid, (struct inb_msgbuf *) & inb_msg[count], sizeof(MSG_QUEUE_HDR) - sizeof(long), -MAXMTYPE, IPC_NOWAIT) == -1)
#endif
	 break;			/* No more messages */

      mdt_trans_stats(&inb_msg[count]);	/* record the input inb_msg statics */
      count++;			/* To store next message */
   }
#endif


   mindex = 0;			/* Initialize index into buffer */
   while (mindex < count)
   {				/* Loop for # of messages stored */
#ifdef DEBUG
#ifndef DUAL_PORTS
      incount++;
#endif
#endif
      if ((incount % MAX_READ_COUNT) == 0)	/* SHOULD BE count?? */
      {
	 CheckMsgQ();		/* check for monitor token tek */
      }
      strncpy(term_id, inb_msg[mindex].route_id, MID_SIZE);	/* Get RFID from input */
      term_id[MID_SIZE] = '\0';

      rf_id = (int) strtol(term_id, (char **) 0, 16);
      if (1)
      {
	 if (fleet[veh_ptr->fleet_nbr]->criteria.acc_on)
	 {			/* NCP enabled for fleet */

	    switch (inb_msg[mindex].type)
	    {			/* Parse message for type */
	    case MT_MSG_STATUS:/* Status Message */
	       nak_handler(&inb_msg[mindex], veh_ptr);	/* Process it */
	       break;		/* Do next message */

	    case MT_FORM_MSG:	/* Form Message */
	       form_hndl(&inb_msg[mindex], veh_ptr, term_id);	/* Process it */
	       break;		/* Do next message */

	    case MT_FUNC_MSG:	/* Function Message */
	       func_hndl(&inb_msg[mindex], veh_ptr, term_id);	/* Process it */
	       break;		/* Do next message */

	    default:
	       nak_msg_err(&inb_msg[mindex]);	/* Process it */
	       break;		/* Do next message */
	    }
	 }
      } else
	 nak_handler(&inb_msg[mindex], NULL);	/* Process it */

      mindex++;			/* get next message */
   }
   if (count < MAX_READ_SLEEP_COUNT)
   {
      sleep(1);			/* so we dont hog */
      /* printf("S"); fflush(stdout); FOR DEBUGGING */
   }
}

/*****************************************************************************
 *
 *===>	nak_handler()(void)		- Process NAK messages.
 *
 * 	DESCRIPTION:
 *		The following function handles End of Transmission
 *	messages.
 *
 * 	PARMETERS:
 *              inb_msg(MSG_QUEUE_HDR *)            - Inbound messages.
 *
 *	GLOBALS:
 *		None.
 *
 *	CALLS:
 *      ==>	nak_error()(void)               - Prints error messages to error file.
 *
 *	RETURN:
 *		None.
 *
 *****************************************************************************/

void
nak_handler(inb_msg, veh_ptr)
   MSG_QUEUE_HDR *inb_msg;
   struct veh_driv *veh_ptr;
{
   char  error_str[80];
   char *message;

   message = inb_msg->route_id;

   if (veh_ptr == NULL)
   {
      nak_cnt++;
      if (inb_msg->p_fields[P1] == MS_MSG_ACK)
	 nak_cnt--;
      else
	 nak_error(inb_msg, "No Vehicle Ptr");
      return;
   }
   if (strncmp(inb_msg->pkt_id, "00", 2))
   {
      switch (inb_msg->p_fields[P1])
      {				/* Get End of transmission type */
      case MS_MSG_ACK:		/* Message ACKED */
	 if (reset_pkt_entry((unsigned) strtol(inb_msg->pkt_id,
				(char **) 0, 16), veh_ptr->veh_nbr) == FAIL)
	 {
	    if (strlen(message) > MAX_ERROR_STR_LEN)	/* safety */
	       message[MAX_ERROR_STR_LEN] = '\0';	/* get only part of it */
/*****	    sprintf(error_str, "Reset fail #%d msg  %s", incount, message);
	    ERROR(' ', "", error_str);  ****/
	 }
	 break;			/* Do nothing */
      case MS_MSG_SENT:
	 break;
      default:
	 nak_cnt++;
	 nak_by_mtype(inb_msg, veh_ptr);
      }				/* Get End of transmission type */
   } else
   {
      nak_cnt++;
      switch (inb_msg->p_fields[P1])
      {				/* Get End of transmission type */
      case MS_MSG_ACK:		/* Message ACKED */
	 nak_cnt--;
	 break;			/* Do nothing */
      case MS_SYNTAX:
	 nak_error(inb_msg, "Syntax");
	 break;
      case MS_RES_NA:
	 /* most probably termina full - ignore it */
	 /* nak_error(inb_msg, "RES NA"); */
	 break;
      case MS_NOT_SUP:
	 nak_error(inb_msg, "NOT SUP");
	 break;
      case MS_IL_MSG:
	 nak_error(inb_msg, "Bad inb_msg");
	 break;
      case MS_DA_SYNTAX:
	 nak_error(inb_msg, "DevA Syntx");
	 break;
      case MS_DA_RES_NA:
	 nak_error(inb_msg, "DevA RES NA");
	 break;
      case MS_DA_NOT_SUP:
	 nak_error(inb_msg, "DevA NOT SUP");
	 break;
      case MS_DB_SYNTAX:
	 nak_error(inb_msg, "DevB Syntx");
	 break;
      case MS_DB_RES_NA:
	 nak_error(inb_msg, "DevB RES NA");
	 break;
      case MS_DB_NOT_SUP:
	 nak_error(inb_msg, "DevB NOT SUP");
	 break;
      case MS_MSG_NAK:
	 /* nak_error(inb_msg, "Message NAK"); */
	 break;
      case MS_MSG_SENT:
	 break;
      case MS_MSG_REJECT:
	 nak_error(inb_msg, "Msg reject");
	 break;
      case MS_IL_MSG_TYPE:
	 nak_error(inb_msg, "Bad inb_msg typ");
	 break;
      case MS_IL_TRANS:
	 nak_error(inb_msg, "Bad xmit site");
	 break;
      case MS_SP1:		/* Not currently used */
      case MS_SP2:		/* Not currently used */
      case MS_SP3:		/* Not currently used */
      case MS_DA_SP:		/* Not currently used */
      case MS_DB_SP:		/* Not currently used */
      default:
	 nak_error(inb_msg, "Unknown");
      }
   }
}

/*****************************************************************************
 *
 *===>  nak_error()(void)               - Prints error messages to error file.
 *
 * 	DESCRIPTION:
 *		This function writes out an error message to the error file
 *	and also prints message types.
 *
 * 	PARMETERS:
 *              inb_msg(MSG_QUEUE_HDR *)            - Inbound messages.
 *		str(char *)			- Text to write out.
 *
 *	GLOBALS:
 *		error_str(char *)		- Temp area for error messages.
 *
 *	CALLS:
 *	===>	ERROR()(void)			- Error writing routine.
 *
 *	RETURN:
 *		None.
 *
 *****************************************************************************/

void
nak_error(inb_msg, str)
   MSG_QUEUE_HDR *inb_msg;
   char *str;
{
   char  p_fields[P_FLD_SIZE + 1];

   strncpy(p_fields, inb_msg->p_fields, P_FLD_SIZE);	/* Get Parmeters for
							 * output */
   p_fields[P_FLD_SIZE] = '\0';

   sprintf(error_str, "Nak: %s", str);
   mk_nak_error(inb_msg, error_str);
}

/*****************************************************************************
 *
 *===>	nak_by_mtype()(void)		- Handle NAK for specific messages.
 *
 * 	DESCRIPTION:
 *		This function handles special case NAKs ( call assign, search report, etc.)
 *	It will generate exceptions associated with the NAK.
 *
 * 	PARMETERS:
 *		None.
 *
 *	GLOBALS:
 *		None.
 *
 *	CALLS:
 *      ==>     mk_except()(void)               - Generate exceptions for processing.
 *      ==>     ERROR()(void)                   - Error writing routine.
 *
 *	RETURN:
 *		None.
 *
 *****************************************************************************/

char  last_pkt_id[3] = "00";

void
nak_by_mtype(inb_msg, veh_ptr)
   MSG_QUEUE_HDR *inb_msg;
   struct veh_driv *veh_ptr;
{
   int   cl_idx;
   struct calls *call_ptr;
   char  pkt_id[3];
   struct cisam_ml *ml_ptr;	/* pointer to message log buffer */
   char  get_pkt_type();
   long  get_pkt_nbr();
   int   id;
   int   msg_nbr;
   int   call_nbr;
   char  pkt_type;

   extern char *scratch_ptr;
   CALL_HNDL call_hndl;


   ml_ptr = (struct cisam_ml *) req_buf;	/* initialize pointer to
						 * request buffer */

   strncpy(pkt_id, inb_msg->pkt_id, PKT_ID_SIZE);
   pkt_id[PKT_ID_SIZE] = '\0';
   if (strncmp(pkt_id, last_pkt_id, PKT_ID_SIZE) == 0)
   {
      sprintf(scratch_ptr, " duplicate pkt ids %c%c", pkt_id[0], pkt_id[1]);
      ERROR(' ', "", scratch_ptr);
   }
   strcpy(last_pkt_id, pkt_id);

   id = (unsigned) strtol(pkt_id, (char **) 0, 16);
   pkt_type = get_pkt_type(id, veh_ptr->veh_nbr);
   switch (pkt_type)
   {
   case NO_PKT_TYPE:
      return;
   case ASSIGN_PKT_TYPE:
      if (veh_ptr == NULL)
      {				/* not a valid terminal id */
	 /*
	  * mk_except((struct calls *)NULL, (struct veh_driv *)NULL, SYS_ERR,
	  * ASSIGN_ERR,-1);
	  */
	 mk_nak_error(inb_msg, "Nak: Null veh_ptr");
      } else
      {				/* with vehicle info */
	 mk_nak_error(inb_msg, "Nak: trip assign");
      }
      if (reset_pkt_entry(id, veh_ptr->veh_nbr) == FAIL)
      {
	 sprintf(trace_str, "Reset fail - Assign call %d", veh_ptr->call_nbr);
	 ERROR(' ', "", trace_str);
      }
      break;

   case MSG_PKT_TYPE:
      msg_nbr = get_pkt_nbr(id, veh_ptr->veh_nbr);
      /* if no msg_nbr then must be a NAK control message */
      if (msg_nbr > 0)
      {
 	msg_nbr = msg_nbr + 1;
	msg_nbr = msg_nbr - 1;
      }	
      else
      {
/*       (void) Veh_reset_state((VEH_HNDL) veh_ptr, VEH_ON_CALL_STATE, HNDL_NULL, HNDL_NULL); */
	veh_ptr->ext.nak_control_msg_state = NAK_CONTROL_MSG_COMPLETE;
      }
      if (reset_pkt_entry(id, veh_ptr->veh_nbr) == FAIL)
      {
	 sprintf(trace_str, "NAK CONTROL MSG Reset fail - msg to  %d", veh_ptr->veh_nbr);
	 ERROR(' ', "", trace_str);
      }
      break;

   case OFFER_PKT_TYPE:	/* NAK on call offer */
      call_nbr = get_pkt_nbr(id, veh_ptr->veh_nbr);
      if (reset_pkt_entry(id, veh_ptr->veh_nbr) == FAIL)
      {
	 sprintf(trace_str, "OFFER PKT Reset fail - Offer call %d", veh_ptr->call_nbr);
	 ERROR(' ', "", trace_str);
      }
      if (1)
      {				/* Find call in index */
	 sprintf(scratch_ptr, "Cant find call %d from veh %d", call_nbr, veh_ptr->veh_nbr);
	 ERROR(' ', " ", scratch_ptr);
	 break;
      }
      if (veh_ptr->call_ptr == NULL)
      {
	 mk_nak_error(inb_msg, "Nak: trip offer");
	 break;			/* assume no accept timeout */
      }
      if (veh_ptr->call_ptr->call_nbr != call_nbr)
      {
	 mk_nak_error(inb_msg, "Nak: trip offerx");
	 break;			/* also assume no accept timeout */
      }
      call_ptr = (struct calls *) call_hndl;
      sprintf(scratch_ptr, "Nak: trip offer # %d", call_ptr->call_nbr);
      mk_nak_error(inb_msg, scratch_ptr);
      /* NOTE: R4/BASELIE PENALIZES */
      /* Call_reset_state((CALL_HNDL) call_ptr, CALL_OFFERED_STATE, CALL_NAK_ON_CALL_OFFER); */
      break;

   default:
      nak_ext_by_mtype(id, inb_msg, veh_ptr, pkt_type);
   }
}				/* end nak_by_mtype */

/*****************************************************************************
 *
 *===>  nak_msg_err()(void)             - Processes improper messages.
 *
 * 	DESCRIPTION:
 *		The following function writes out an error for a message that
 *	can't be processed.
 *
 * 	PARMETERS:
 *              inb_msg(MSG_QUEUE_HDR *)            - Inbound messages.
 *
 *	GLOBALS:
 *		error_str(char *)		- Temp area for error messages.
 *
 *	CALLS:
 *	==>	ERROR()(void)			- Error writing routine.
 *
 *	RETURN:
 *		None.
 *
 *****************************************************************************/

void
nak_msg_err(inb_msg)
   MSG_QUEUE_HDR *inb_msg;
{
   mk_nak_error(inb_msg, "Bad inb_msg hdr");
}

/*****************************************************************************
 *
 *===>  form_hndl()(void)               - Process Form Messages.
 *
 * 	DESCRIPTION:
 *		The following processes the forms message type.
 *
 * 	PARMETERS:
 *              inb_msg(MSG_QUEUE_HDR *)            - Inbound messages.
 *              veh_ptr(struct veh_driv *)      - Pointer to current vehicle struct.
 *		term_id(char *)			- Pointer to current MID.
 *
 *	GLOBALS:
 *		None.
 *
 *	CALLS:
 *      ==>     local_form_hndl()(void)         - Processes Inbound Local Forms.
 *      ==>     nak_msg_err()(void)             - Processes improper messages.
 *
 *	RETURN:
 *		None.
 *
 *****************************************************************************/

void
form_hndl(inb_msg, veh_ptr, term_id)
   MSG_QUEUE_HDR *inb_msg;
   struct veh_driv *veh_ptr;
   char *term_id;
{
   if (veh_ptr == NULL)
   {				/* Check for Invalid RFID */
      return;
   }
   if (!veh_ptr->t_status.mdt_working || veh_ptr->t_status.emer)	/* Check KDT status */
      return;			/* Message not to be processed */

   switch (inb_msg->p_fields[P1])
   {
   case FT_LOCAL_FORM:
      local_form_hndl(inb_msg, veh_ptr, term_id);	/* Process it */
      break;

   case FT_AUX_INP:
      /* ITM input */
/*      itm_msg_hndl(inb_msg, veh_ptr, term_id); */
      break;

   case FT_DRIVER_QUERY:
   default:
      nak_msg_err(inb_msg);
      break;
   }
}

/*****************************************************************************
 *
 *===>  local_form_hndl()(void)         - Processes Inbound Local Forms.
 *
 * 	DESCRIPTION:
 *		The following processes the local form type.
 *
 * 	PARMETERS:
 *              inb_msg(MSG_QUEUE_HDR *)            - Inbound messages.
 *              veh_ptr(struct veh_driv *)      - Pointer to current vehicle struct.
 *		term_id(char *)			- Pointer to current MID.
 *
 *	GLOBALS:
 *		None.
 *
 *	CALLS:
 *	==>	k_sign_in()(void)		- Process sign in message.
 *	==>	k_post()(void)			- Process book message.
 *	==>	k_epost()(void)			- Process soon to clear message.
 *	==>	k_msg()(void)			- Process message message.
 *	==>	k_cpost()(void)			- Process conditional book.
 *	==>	k_eta()(void)			- Process eta message.
 *	==>	k_inquiry()(void)		- Process inquiry message.
 *	==>	k_noshow()(void)		- Process noshow message.
 *      ==>     nak_msg_err()(void)             - Processes improper messages.
 *
 *	RETURN:
 *		None.
 *
 *****************************************************************************/

void
local_form_hndl(inb_msg, veh_ptr, term_id)
   MSG_QUEUE_HDR *inb_msg;
   struct veh_driv *veh_ptr;
   char *term_id;
{
   switch (inb_msg->p_fields[P2])
   {
   case LF_S_IN:
      k_sign_in(inb_msg->text, veh_ptr, term_id);
      break;

   case LF_POST:
      k_post(inb_msg, veh_ptr, term_id);
      break;

   case LF_EPOST:
      k_epost(inb_msg, veh_ptr, term_id);
      break;

   case LF_MSG:
      k_msg(inb_msg, veh_ptr, term_id);
      break;

   case LF_BID:
      k_bid(inb_msg, veh_ptr, term_id);
      break;

   case LF_CPOST:
      k_cpost(inb_msg, veh_ptr, term_id);
      break;

   case LF_UNPOST:
      k_unpost(inb_msg, veh_ptr, term_id);
      break;

   case LF_ETA:
      k_eta(inb_msg, veh_ptr, term_id);
      break;

   case LF_INQ:
      k_inquiry(inb_msg, veh_ptr, term_id);
      break;

   case LF_NOSHOW:
      k_noshow(inb_msg, veh_ptr, term_id);
      break;

   case LF_TAXI_ACCT:
   case LF_ACCT1:
   case LF_ACCT2:
   case LF_ACCT3:
   case LF_ACCT4:
      if ((veh_ptr->term_type == MDT_V02) || (inb_msg->p_fields[P2] == LF_ACCT_V03))
      {
	 k_account(inb_msg, veh_ptr, term_id);
	 break;
      }
      /* fall through if V03 and not correct local form */

   default:
      local_form_ext_hndl(inb_msg, veh_ptr, term_id);
      break;
   }
}

/*****************************************************************************
 *
 *===>  func_hndl()(void)               - Processes Inbound Function Messages.
 *
 * 	DESCRIPTION:
 *		The following processes the function message type.
 *
 * 	PARMETERS:
 *              inb_msg(MSG_QUEUE_HDR *)            - Inbound messages.
 *              veh_ptr(struct veh_driv *)      - Pointer to current vehicle struct.
 *		term_id(char *)			- Pointer to current MID.
 *
 *	GLOBALS:
 *		None.
 *
 *	CALLS:
 *	==>	k_emergency()(void)		- Process emergency message.
 *      ==>     can_msg_hndl()(void)            - Processes Inbound Can Messages.
 *      ==>     ext_inp_hndl()(void)            - Processes External Input Messages.
 *      ==>     nak_msg_err()(void)             - Processes improper messages.
 *
 *	RETURN:
 *		None.
 *
 *****************************************************************************/

void
func_hndl(inb_msg, veh_ptr, term_id)
   MSG_QUEUE_HDR *inb_msg;
   struct veh_driv *veh_ptr;
   char *term_id;
{
   if (veh_ptr == NULL)
   {				/* Check for Invalid RFID */
      return;
   }
   if (!veh_ptr->t_status.mdt_working || veh_ptr->t_status.emer)	/* Check KDT status */
      return;			/* Message not to be processed */

   switch (inb_msg->p_fields[P1])
   {
   case FCT_EMERG:
      k_emergency(veh_ptr, term_id);
      break;

   case FCT_CAN_MSG:
      can_msg_hndl(inb_msg, veh_ptr, term_id);	/* Process it */
      break;

   case FCT_EXT_INP:
      ext_inp_hndl(inb_msg, veh_ptr, term_id);
      break;

   default:
      nak_msg_err(inb_msg);
      break;
   }
}

/*****************************************************************************
 *
 *===>  can_msg_hndl()(void)            - Processes Inbound Can Messages.
 *
 * 	DESCRIPTION:
 *		The following processes the can message type.
 *
 * 	PARMETERS:
 *              inb_msg(MSG_QUEUE_HDR *)            - Inbound messages.
 *              veh_ptr(struct veh_driv *)      - Pointer to current vehicle struct.
 *		term_id(char *)			- Pointer to current MID.
 *
 *	GLOBALS:
 *		None.
 *
 *	CALLS:
 	==>	k_accept()(void)		- Process accept message.
 *	==>	k_reject()(void)		- Process reject message.
 *	==>	k_flag()(void)			- Process flag message.
 *	==>	k_talk()(void)			- Process talk message.
 *	==>	k_sign_out()(void)		- Process sign out.
 *	==>	k_info()(void)			- Process info message.
 *	==>	k_break()(void)			- Process break message.
 *	==>	k_pickup()(void)			- Process pickup message.
 *	==>	k_close()(void)			- Process close message.
 *	==>	k_call_out()(void)		- Process call out message.
 *      ==>     nak_msg_err()(void)             - Processes improper messages.
 *
 *	RETURN:
 *		None.
 *
 *****************************************************************************/
void
can_msg_hndl(inb_msg, veh_ptr, term_id)
   MSG_QUEUE_HDR *inb_msg;
   struct veh_driv *veh_ptr;
   char *term_id;
{
   switch (inb_msg->p_fields[P2])
   {
   case CM_ON_DATA:
      /* ignore the on data channel message */
      break;

   case CM_ACPT:
      k_accept(veh_ptr, term_id);
      break;

   case CM_RJCT:
      k_reject(veh_ptr, term_id);
      break;

   case CM_FLAG:
      k_flag(veh_ptr, term_id);
      break;

   case CM_VOICE:
      k_talk(veh_ptr, term_id);
      break;

   case CM_S_OFF:
      k_sign_out(veh_ptr, term_id);
      break;

   case CM_INFO:
      k_info(inb_msg, veh_ptr, term_id);
      break;

   case CM_BREAK:
      k_break(veh_ptr, term_id);
      break;

   case CM_PCKUP:
      k_pickup(veh_ptr, term_id);
      break;

   case CM_CLOSE:
      k_close(veh_ptr, term_id);
      break;

   case CM_COUT_V02:
      if (veh_ptr->term_type == MDT_V02)
	 k_call_out(veh_ptr, term_id);
      else
	 nak_msg_err(inb_msg);
      break;

   case CM_COUT_V03:
      if (veh_ptr->term_type == MDT_V02)
	 nak_msg_err(inb_msg);
      else
	 k_call_out(veh_ptr, term_id);
      break;


   case CM_MFAULT:
      k_mfault(veh_ptr, term_id);
      break;

   default:
      can_ext_msg_hndl(inb_msg, veh_ptr, term_id);
      break;
   }
}

/*****************************************************************************
 *
 *===>  ext_inp_hndl()(void)            - Processes Inbound Can Messages.
 *
 * 	DESCRIPTION:
 *		The following processes the external input message type.
 *
 * 	PARMETERS:
 *              inb_msg(MSG_QUEUE_HDR *)            - Inbound messages.
 *              veh_ptr(struct veh_driv *)      - Pointer to current vehicle struct.
 *		term_id(char *)			- Pointer to current MID.
 *
 *	GLOBALS:
 *		None.
 *
 *	CALLS:
 *      ==>	k_meter_on()(void)		- Process meter on message.
 *      ==>	k_meter_off()(void)		- Process meter off message.
 *      ==>     nak_msg_err()(void)             - Processes improper messages.
 *
 *	RETURN:
 *		None.
 *
 *****************************************************************************/

void
ext_inp_hndl(inb_msg, veh_ptr, term_id)
   MSG_QUEUE_HDR *inb_msg;
   struct veh_driv *veh_ptr;
   char *term_id;
{
   switch (inb_msg->p_fields[P2])
   {
   case EI_M_ON:
      k_meter_on(veh_ptr, term_id);
      break;

   case EI_M_OFF:
      k_meter_off(veh_ptr, term_id);
      break;

   default:
      nak_msg_err(inb_msg);
      break;
   }
}


/*****************************************************************************/
/*                                                                           */
/*   k_account()      check an account number                                */
/*                                                                           */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)         */
/*                                                                           */
/*      PARAMS:                                                              */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)      */
/*                                                                           */
/*         inb_msg            outbound message                                   */
/*****************************************************************************/

int
k_account(inb_msg, veh_ptr, term_id)
   MSG_QUEUE_HDR *inb_msg;
   struct veh_driv *veh_ptr;
   char *term_id;
{
   int   rc;


   /* vehicle is not signed on */
   if (!(SIGNED_ON(veh_ptr)))
   {
      /* notify driver that vehicle has not signed in */
      send_text_predefined(term_id, NOT_SIGN_IN, veh_ptr);
      return -1;
   }
   if (veh_ptr->term_type == MDT_V02)
      rc = 1;
   else
      rc = 1;
   if (rc == GOOD_ACCOUNT)
      send_text_predefined(term_id, GOOD_ACT, veh_ptr);
   else if (rc == BAD_ACCOUNT)
      send_text_predefined(term_id, BAD_ACT, veh_ptr);
   else				/* CANT_VERIFY */
      send_text_predefined(term_id, NO_VERIFY, veh_ptr);

   return 0;
}				/* end k_account */

#ifndef MSGQ_
#ifdef ON_AT_A_TIME
int
get_inb_msg(inb_msg)
   char *inb_msg;
/* get inbound message from the async pipe */
{
/* need an alarm */
   int   nbr;
   int   len = 1;
   char *msg_start = inb_msg + 4;

   inb_msg += sizeof(int);	/* because we dont load the size field */
   nbr = read(inb_fd, inb_msg++, 1);
   if (nbr <= 0)
      return (-1);
#ifdef DEBUG_PIPES
   printf("inb inb_msg:%c", inb_msg - 1);
#endif
   while ((*(inb_msg - 1) != '\0') || (len <= MSG_HDR_LEN))
   {
#ifdef DEBUG_PIPES
      printf("%c", *(inb_msg - 1));
#endif
      nbr = read(inb_fd, inb_msg, 1);
      if (nbr)
      {
	 len++;
	 inb_msg++;
      }
   }
#ifdef DEBUG_PIPES
   printf("\n");
   /* printf("inb_msg diff %d\n",inb_msg - msg_start); */
   printf("length = %d\n", len);
#endif
   return (len);
}				/* end get_inb_msg */

#else

void
#ifdef DUAL_PORTS
inb_msg_recover(inb_fd)
   FILE *inb_fd;
#else
inb_msg_recover()
#endif
/* attempt to get back in sync from the pipe */
{
   char  ch;
   int   nbr;

   nbr = 10;
   ch = ' ';
   while ((ch != CHECK_CHAR) && (nbr > 0))
      nbr = read(inb_fd, &ch, 1);
   read(inb_fd, &ch, 1);
}				/* end inb_msg_recover */

#define ASYNC_ERROR_MSG "Error in reading async pipe - message(s) lost"
int
#ifdef DUAL_PORTS
get_inb_msg(inb_fd, inb_msg)
   FILE *inb_fd;
#else
get_inb_msg(inb_msg)
#endif
   char *inb_msg;
{
   int   nbr;
   char *ptr;
   char  size[sizeof(int) + 1];
   int   msg_size;

   /* read in the size n the priority field */
   nbr = read(inb_fd, &msg_size, sizeof(int));
   if (nbr <= 0)
      return (-1);

   inb_msg += sizeof(int);	/* because we dont use the size  field */

   /* got a message folks */
   /* msg_size = atoi(size); */
   if (msg_size <= 0)
      return (-1);
   if (msg_size > MAX_MSG_SIZE)

   {
#ifdef DUAL_PORTS
      inb_msg_recover(inb_fd);
#else
      inb_msg_recover();
#endif
      ERROR(' ', "", ASYNC_ERROR_MSG);
      return (-1);
   }
   nbr = read(inb_fd, inb_msg, msg_size);

   if (inb_msg[nbr - 2] != CHECK_CHAR)
   {
#ifdef DUAL_PORTS
      inb_msg_recover(inb_fd);
#else
      inb_msg_recover();
#endif
      ERROR(' ', "", ASYNC_ERROR_MSG);
      return (-1);
   }
   if (nbr != msg_size)
   {
#ifdef DUAL_PORTS
      inb_msg_recover(inb_fd);
#else
      inb_msg_recover();
#endif
      ERROR(' ', "", ASYNC_ERROR_MSG);
      return (-1);
   }
   inb_msg[nbr - 2] = '\0';	/* overwrite the check character */
   return (nbr);
}				/* end get_inb_msg */
#endif
#endif

/******************************************************************************
* emulate_mdt - perform  a limited set of mdt commands
*
******************************************************************************/
emulate_mdt(mdt_req)		/* BECOMES nochange */
   struct mdt_emulate *mdt_req;
{
   MSG_QUEUE_HDR inb_msg;
   char  term_id[MID_SIZE + 1];
   struct veh_driv *veh_ptr;
   short fl_nbr;
   FLEET_HNDL fleet_hndl;


   switch (mdt_req->action_code)
   {
   case EMUL_POST:
      veh_ptr->priority = mdt_req->priority | 0x30;	/* convert short to char */
      sprintf(inb_msg.text, "%d", mdt_req->zone_nbr);
      k_post(&inb_msg, veh_ptr, term_id);
      break;
#define EMUL_CPOST 2	/*MH / unused ??? */
   case EMUL_CPOST:
      sprintf(inb_msg.text, "%d", mdt_req->zone_nbr);
      k_cpost(&inb_msg, veh_ptr, term_id);
#ifdef EMUL_BID
      printf("emulating bid");
      d_bid(veh_ptr, 200);
#endif
      break;
   case EMUL_EPOST:
      veh_ptr->priority = mdt_req->priority | 0x30;	/* convert short to char */
      sprintf(inb_msg.text, "%d", mdt_req->zone_nbr);
      k_epost(&inb_msg, veh_ptr, term_id);
      break;
   case EMUL_S_IN:
/*      Veh_process_key((VEH_HNDL) veh_ptr, VEH_SIGN_IN_KEY, (HNDL) mdt_req->drv_id); */
      break;
   case EMUL_S_OUT:
      k_sign_out(veh_ptr, term_id);
      break;

   case EMUL_RESET:
      k_unpost(&inb_msg, veh_ptr, term_id);
      break;

   case EMUL_DWNLD:
      veh_ptr->t_status.emer = 0;
      authorize(veh_ptr);	/* authorize terminal for futher use */
      break;

   case EMUL_ACPT:
      k_accept(veh_ptr, term_id);
      break;

   case EMUL_REJ:
      k_reject(veh_ptr, term_id);
      break;

   case EMUL_NOSHOW:
      k_noshow(&inb_msg, veh_ptr, term_id);
      break;

   case EMUL_FLAG:
      k_flag(veh_ptr, term_id);
      break;

   case EMUL_PCKUP:
      k_pickup(veh_ptr, term_id);
      break;

   case EMUL_CLOSE:
      k_close(veh_ptr, term_id);
      break;

   default:
      ERROR(' ', "", "emulate_mdt -  invalid action");
   }
}				/* end emulate_mdt */
