static char sz__FILE__[]="@(#)kdt_3.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  kdt_3.c; Rel: 10.1.2.1; Get date: 5/11/95 at 10:15:10
* @(#)  Last delta: 12/2/94 at 18:27:39
* @(#)  SCCS File: /taxi/sccs/s.kdt_3.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

/* Based on R3 version 5.17 */

/*****************************************************************************
 *
 *		kdt_interface()(void)		- Process messages from ACC.
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
#include <sys/time.h>
#include <langinfo.h>
#include <locale.h>

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
#include "mads_types.h"
#include "mad_error.h"
#include "mads_isam.h"
#include "kdt.h"
#include "veh_driv.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "except.h"
#include "sprintf.h"
#include "credit.h"
#include "language.h"
#include "path.h"
#include "msg_mmp.h"
#include "itm.h"
#include "calls.h"
#include "writer.h"
#include "timers.h"
#include "Call_timers_public.h"
#define LIBRARY
#include "enhance.h"
#include "switch_ext.h"
#include "tmplen.h"
#include "GPS_private.h"
#include "Call_msg_private.h"

	
void 	nak_handler(MSG_QUEUE_HDR *, struct veh_driv *);			/* Processes NAK errors */
void	nak_error(MSG_QUEUE_HDR *, char *);		/* Writes out error messages */
void 	local_form_hndl(MSG_QUEUE_HDR *, struct veh_driv *, char *);		/* Processes Inbound Local Forms */
void 	local_form_new_hndl(MSG_QUEUE_HDR *, struct veh_driv *, char *);		/* Processes Inbound Local Forms */
void    zonecheck_form_hndl(MSG_QUEUE_HDR *, struct veh_driv *, char *);
void 	form_hndl(MSG_QUEUE_HDR	*, struct veh_driv *, char *);	/* Processes Inbound Forms */
void 	func_hndl(MSG_QUEUE_HDR *, struct veh_driv *, char *);	/* Processes Inbound Function Messages */
void 	ext_inp_hndl(MSG_QUEUE_HDR *, struct veh_driv *, char *);	/* Processes External Input Messages */
void    hia_hndl(MSG_QUEUE_HDR *, struct veh_driv *, char *);
void    kela_request_form_hndl(MSG_QUEUE_HDR *, struct veh_driv *, char *);
void 	can_msg_hndl(MSG_QUEUE_HDR *, struct veh_driv *, char *);	/* Processes Inbound Can Messages */
void	nak_msg_err(MSG_QUEUE_HDR *);			/* Processes Improper Messages */
void 	nak_by_mtype(MSG_QUEUE_HDR *, struct veh_driv *);
void	kdt_trans_stats(MSG_QUEUE_HDR *);
extern struct fleets * fleet_hndl_to_ptr();
extern struct cisam_cl *get_call_record();
int TopLight = 0;

extern char req_buf[];                                                          /* request buffer for db */
extern int incount;

extern int inb_fd;
extern int udp_in_fd;
extern int from_aux_device_fd;
extern int to_aux_device_fd;
extern int use_aux_pipes; 

extern FILE *dummy_file;
extern int user_pipe_fd;
extern int outb_fd;

int nak_cnt = 0;
struct cisam_ch  writer_buf;

/* tuneable parameters */
int count_read_param = 50;               /* how often in async read to ui read */
int count_sleep_param = 500000;              /* when to sleep in asyn read count less than */
extern int kdt_msgs_max;
extern	struct	fleets	  *fleet[];					/* Global fleet structure */

MSG_QUEUE_HDR	msg[MAX_MSG_READ];					/* Holds inbound messages */
#define MAX_ERROR_STR_LEN   40           /* approx # of characters available in db error record */

static void
mk_nak_error(msg, reason)
   MSG_QUEUE_HDR	*msg;
   char reason[];
{
				char p_fields[P_FLD_SIZE + 1];
				char mid[MID_SIZE + 1];
				char pkt_id[3];
				static char str_temp[TMP_STR_LEN];

#ifdef FOO
				
        strncpy(p_fields, msg->p_fields, P_FLD_SIZE);
        p_fields[P_FLD_SIZE] = '\0';
        strncpy(pkt_id, msg->pkt_id, 2);
        pkt_id[2] = '\0';
        strncpy(mid, msg->route_id, MID_SIZE);
        mid[MID_SIZE] = '\0';
        sprintf(str_temp,"%s mid %s pkt %s type %c pfld %s",
                reason, mid, pkt_id, msg->type, p_fields);
        ERROR(' ',"", str_temp);
        if (strlen(msg->text) != 0)
        {
           if (strlen(msg->text) > MAX_ERROR_STR_LEN)
               msg->text[MAX_ERROR_STR_LEN] = '\0';    /* get only part of it */
           sprintf(str_temp, "msg text %s",msg->text);
           ERROR(' ',"",str_temp);
        }
#endif
 
        
}        /* end mk_nak_error */



/*****************************************************************************
 *
 *===>	kdt_interface()(void) 		- Process messages from ACC.
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
 *		kdt_msgs_max(int) 		- Max. # of KDT messages read in.
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
kdt_interface()
{
	short	count=0;								/* Buffer counter */
	struct	veh_driv  *veh_ptr;						/* Vehicle Pointer */
	short	mindex;								/* Index of message to be processed in msg array */
	char	term_id[ROUTE_ID_SIZE + 1];					/* RFID in ASCI format */
	int     Fd_read, maxfdpl,nfound;
	fd_set  readmask;
	struct timeval tdelay;
	VEH_REC    veh_rec;
	
	extern	int	recv_msgqid;						/* Message queue to ACC */

	extern	struct	offsets	  *offset;					/* Shared Memory Pointer */
	extern	struct	veh_driv  *get_veh_id();				/* Returns veh_ptr from term_id */

	memset(msg, '\0', sizeof(msg));						/* Initialize buffer */

	/* read the AUX_PIPE if we are attached. We don't
	   expect large traffic so just process one
	   at a time
	*/
	if ( use_aux_pipes == TRUE )
	  {
	    if ( ( msg[0].size = get_inb_msg( to_aux_device_fd, &msg[0] ) ) != -1 )
	      {
		memcpy(msg[0].pkt_id, "FF", sizeof(msg[0].pkt_id));
		write( outb_fd, &msg[0], msg[0].size + sizeof(msg[0].size) );
	      }
	    memset(msg, '\0', sizeof(msg));					      	    
	  }
	
	while(count < kdt_msgs_max)
	  {
#ifdef DELAY_HERE
                if ((count % count_read_param) == 0) /*UI */
                {
                   read_pipe();
		   //			read_ccp_pipe();
                   CheckInWithMonitor();						/* check for monitor token tek*/
                }
#endif

		//		  read_ccp_pipe();

                if ( (msg[count].size = get_inb_msg(inb_fd, &msg[count])) == -1)
			break;							/* No more messages */

		count++;							/* To store next message */
	}

	// Check for UDP messages
	while(count < kdt_msgs_max)
	  {
#ifdef DELAY_HERE
                if ((count % count_read_param) == 0) /*UI */
                {
                   read_pipe();
		   read_ccp_pipe();
                }
#endif

                if ( (msg[count].size = get_inb_msg(udp_in_fd, &msg[count])) == -1)
			break;							/* No more messages */

		count++;							/* To store next message */
	}	

	if(!offset->status.ncp)							/* NCP disabled for all fleets */
		return;								/* Don't process messages */

	mindex = 0;
	incount = 0;
	while(mindex < count) {							/* Loop for # of messages stored */
                incount++;
                if ((incount % count_read_param) == 0)
                {
		  //read_ccp_pipe();
                   read_pipe();
		   CheckInWithMonitor();
                }
		strncpy(term_id, msg[mindex].route_id, MID_SIZE);		/* Get RFID from input */
		term_id[MID_SIZE] = '\0';

		if(veh_ptr = get_veh_id(term_id))
		  {
		    /* check for DUAL ACC enhancement */
		    if ( ( 1 ) && ( msg[mindex].delvry_prio > '0' ) )
		      {
			if ( (char)Veh_get_value( (VEH_HNDL) veh_ptr, VEH_HOST_LINK ) !=
			           msg[mindex].delvry_prio )
			  {
			    /* change host link value in memory and database table */
			    Veh_set_value( (VEH_HNDL) veh_ptr, VEH_HOST_LINK, (HNDL)msg[mindex].delvry_prio );
			    veh_rec.fleet = fleet[veh_ptr->fleet_nbr]->fleet_id;
			    veh_rec.nbr = veh_ptr->veh_nbr;
			    if (db_read_key(VEHICLE_FILE_ID, &veh_rec, &veh_key2, DB_EQUAL + DB_LOCK) == SUCCESS)
			      {
				veh_rec.host_link = msg[mindex].delvry_prio;
				db_update(VEHICLE_FILE_ID, &veh_rec);
				db_release(VEHICLE_FILE_ID);
			      }
			  }
		      }

                    if ( ( msg[mindex].persist == 'S' ) )
                      Veh_set_value( (VEH_HNDL) veh_ptr, VEH_NAV_TYPE, (HNDL)STAR2000_MDT );
                    else
                      Veh_set_value( (VEH_HNDL) veh_ptr, VEH_NAV_TYPE, (HNDL)0 );

		    msg[mindex].delvry_prio = ' ';
		    
			if(fleet[veh_ptr->fleet_nbr]->criteria.ncp_on) {	/* NCP enabled for fleet */

				switch(msg[mindex].type) {			/* Parse message for type */
					case MT_MSG_STATUS:			/* Status Message */
						nak_handler(&msg[mindex], veh_ptr);	/* Process it */
						break;				/* Do next message */

					case MT_FORM_MSG:			/* Form Message */
						form_hndl(&msg[mindex], veh_ptr, term_id);	/* Process it */
						break;				/* Do next message */

					case MT_FUNC_MSG:			/* Function Message */
						func_hndl(&msg[mindex], veh_ptr, term_id);	/* Process it */
						break;				/* Do next message */
		
					default:
						nak_msg_err(&msg[mindex]);	/* Process it */
						break;				/* Do next message */
				}
			}
		}
		else
			nak_handler(&msg[mindex], NULL);	/* Process it */

		mindex++;							/* get next message */
	}
	FD_ZERO( &readmask );
        if (count == 0)
        {
	  //#ifdef __alpha
	   FD_SET( outb_fd, &readmask );
	   FD_SET( inb_fd, &readmask );
	   FD_SET( user_pipe_fd, &readmask );	   
	   maxfdpl =  3;
	   tdelay.tv_sec = 0;
	   tdelay.tv_usec = count_sleep_param;
	   nfound = select( maxfdpl, &readmask, ( fd_set * ) 0, (fd_set * ) 0,
			    &tdelay );
	   //#else
	   //	   sleep(1);
	   //#endif
        }
#ifdef DEBUG
   if (offset->prattle >= LEVEL1)
   {
      sprintf(trace_str, "LEAVING kdt_interface count %d)\n", count);
      TRACE(trace_str);
   }
#endif
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
 *              msg(MSG_QUEUE_HDR *)            - Inbound messages.
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
nak_handler(msg, veh_ptr)
MSG_QUEUE_HDR	*msg;
struct	veh_driv	*veh_ptr;
{
  char  error_str[80];
  char *message;
  int  uID;
  short   iAcceptTime;
  int  gps_distance;
  char  sPktID[3];
  char  get_pkt_type( unsigned int, short);
  FLEET_HNDL   fleet_hndl;
  FLEET        *pFleet;
  int   fl_nbr;
  extern char *scratch_ptr;
  struct calls *call_ptr;
  KELANODE_REC kelanode_rec;
  char kela_details_temp[TMP_STR_LEN];  
  
  if (veh_ptr == NULL)
    {
      nak_cnt++;
      if (msg->p_fields[P1] == MS_MSG_ACK) nak_cnt--;
      nak_error(msg, "No Vehicle Ptr");
      return;
    }

  /* check for ACK/NAK from aux device and forward on aux_pipe */
  if ( ( use_aux_pipes == TRUE ) && ( !(strncmp(msg->pkt_id, "FF", 2)) ) )
    {
      strcat(msg->text, CHECK_CHAR_STR);
      write( from_aux_device_fd, msg, msg->size + sizeof(msg->size) );
      return;
    }
  
  if(strncmp(msg->pkt_id, "00", 2)) {
    switch(msg->p_fields[P1]) {					/* Get End of transmission type */
    case MS_MSG_ACK:					/* Message ACKED */
      strncpy(sPktID, msg->pkt_id, sizeof(msg->pkt_id));
      sPktID[2] ='\0';
      uID = (unsigned int) strtol(sPktID, (char **) 0, 16);
      switch (get_pkt_type(uID, veh_ptr->veh_nbr))
	{
	case OFFER_PKT_TYPE:
	  if ( veh_ptr->call_ptr )
	    {
	      if (TopLight)
		send_RELAYOFF(veh_ptr);
	      
	      fl_nbr = (int) Veh_get_value((VEH_HNDL) veh_ptr, VEH_FLEET_NBR);
	      (void) Fleet_get_hndl(BY_NBR, fl_nbr, &fleet_hndl);
	      pFleet = fleet_hndl_to_ptr( fleet_hndl );
	      if ( pFleet->zone_ptr[veh_ptr->call_ptr->pickup_zone] != NULL )
		{
		  if ( ( iAcceptTime =
			 pFleet->zone_ptr[veh_ptr->call_ptr->pickup_zone]->accept_duration ) <= 0 )
		    {
		      iAcceptTime = (short) Fleet_get_value(fleet_hndl, FLEET_CALL_ACCEPT_TIME);
		      if ( veh_ptr->roti == TRUE )
			iAcceptTime += pFleet->zone_ptr[veh_ptr->call_ptr->pickup_zone]->roti_time;
		    }
		}
	      else
		{
		  sprintf(scratch_ptr, "problem with accept_timer:fleet %d zone %d", fl_nbr, veh_ptr->call_ptr->pickup_zone);
		  ERROR(' ', "", scratch_ptr);
		  iAcceptTime = 60;
		}
	      
	      Call_delete_timer( (CALL_HNDL) veh_ptr->call_ptr, T_ACCPT );
	      if (!Call_get_state((CALL_HNDL)veh_ptr->call_ptr, CALL_ACCEPT_STATE))
		{
		  Call_add_timer((CALL_HNDL) veh_ptr->call_ptr, T_ACCPT,
				 (int)iAcceptTime,
				  (int (*)())call_accept_notify_func);
		}

	      
	      if ( ( veh_ptr->gps_long > 0.0 ) &&
		   ( veh_ptr->gps_lat  > 0.0 ) &&
		   ( veh_ptr->call_ptr->gps_long > 0.0 ) &&
		   ( veh_ptr->call_ptr->gps_lat  > 0.0 ) )
		gps_distance = (int)GPS_veh_call_get_distance((VEH_HNDL)veh_ptr, (CALL_HNDL)veh_ptr->call_ptr);
	      else
		gps_distance = 0;
	      
	      Call_add_history((CALL_HNDL) veh_ptr->call_ptr, (VEH_HNDL) veh_ptr,
			       ACK_CALL, veh_ptr->zone_num, 0L,
			       (int)gps_distance );

	    }
	  reset_pkt_entry((unsigned int) strtol(msg->pkt_id, 
						(char **) 0, 16),veh_ptr->veh_nbr);	  
	  break;
	case ASSIGN_PKT_TYPE:
	  if ((0) && (veh_ptr->call_ptr != NULL))
	    {
	      if ( (CALL_MSG_LIST_HNDL)Call_get_value( (CALL_HNDL)veh_ptr->call_ptr, CALL_MSG_LIST ) != HNDL_NULL )
		  Call_msg_send( (CALL_MSG_LIST_HNDL)Call_get_value( (CALL_HNDL)veh_ptr->call_ptr, CALL_MSG_LIST ),
				 (VEH_HNDL)veh_ptr );
	    }
	  reset_pkt_entry((unsigned int) strtol(msg->pkt_id, 
					    (char **) 0, 16),veh_ptr->veh_nbr);
	  
	  break;
	case ITM_MDT_PKT_TYPE:
	  veh_ptr->invoice_retry_count = 0;
	  reset_pkt_entry((unsigned int) strtol(msg->pkt_id, 
					    (char **) 0, 16),veh_ptr->veh_nbr);	  
	  break;
	  
	case SP_MDT_PKT_TYPE: /* Ack returned from a SamPlan MDT call message */
	  if (0)
	    Call_msg_mdt_ack( (VEH_HNDL)veh_ptr, (char)get_pkt_num(uID, veh_ptr->veh_nbr) );

	  reset_pkt_entry((unsigned int) strtol(msg->pkt_id, 
					    (char **) 0, 16),veh_ptr->veh_nbr);
	  break;
	  
	default:
	  reset_pkt_entry((unsigned int) strtol(msg->pkt_id, 
					    (char **) 0, 16),veh_ptr->veh_nbr);	  
	  break;
	}
    case MS_MSG_SENT:
      break;	
    default:
      nak_cnt++;
      nak_by_mtype(msg, veh_ptr);
    }					/* Get End of transmission type */
  }
  else {
    nak_cnt++;
    switch(msg->p_fields[P1]) {					/* Get End of transmission type */
    case MS_MSG_ACK:					/* Message ACKED */
      nak_cnt--;
      break;						/* Do nothing */
    case MS_SYNTAX:
      nak_error(msg, "Syntax");
      break;	
    case MS_RES_NA:
      /* most probably termina full - ignore it */
      /* nak_error(msg, "RES NA"); */
      break;	
    case MS_NOT_SUP:
      nak_error(msg, "NOT SUP");
      break;	
    case MS_IL_MSG:
      nak_error(msg, "Bad msg");
      break;	
    case MS_DA_SYNTAX:
      nak_error(msg, "DevA Syntx");
      break;	
    case MS_DA_RES_NA:
      nak_error(msg, "DevA RES NA");
      break;	
    case MS_DA_NOT_SUP:
      nak_error(msg, "DevA NOT SUP");
      break;	
    case MS_DB_SYNTAX:
      nak_error(msg, "DevB Syntx");
      break;	
    case MS_DB_RES_NA:
      nak_error(msg, "DevB RES NA");
      break;	
    case MS_DB_NOT_SUP:
      nak_error(msg, "DevB NOT SUP");
      break;	
    case MS_MSG_NAK:
      /* nak_error(msg, "Message NAK"); */
      break;	
    case MS_MSG_SENT:
      break;	
    case MS_MSG_REJECT:
      nak_error(msg, "Msg reject");
      break;	
    case MS_IL_MSG_TYPE:
      nak_error(msg, "Bad msg typ");
      break;	
    case MS_IL_TRANS:
      nak_error(msg, "Bad xmit site");
      break;	
    case MS_SP1:						/* Not currently used */
    case MS_SP2: 						/* Not currently used */
    case MS_SP3: 						/* Not currently used */
    case MS_DA_SP: 						/* Not currently used */
    case MS_DB_SP: 						/* Not currently used */
    default:
      nak_error(msg, "Unknown");
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
 *              msg(MSG_QUEUE_HDR *)            - Inbound messages.
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
nak_error(msg, str)
MSG_QUEUE_HDR	*msg;
char	*str;
{
	char	p_fields[P_FLD_SIZE + 1];

	strncpy(p_fields, msg->p_fields, P_FLD_SIZE);		/* Get Parmeters for output */
	p_fields[P_FLD_SIZE] = '\0';

   	sprintf(error_str, "Nak: %s", str);
        mk_nak_error(msg, error_str);
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
nak_by_mtype(msg, veh_ptr)
   MSG_QUEUE_HDR *msg;
   struct veh_driv *veh_ptr;
{
   int   cl_idx;
   struct calls *call_ptr;
   char  pkt_id[3];
   struct cisam_ml *ml_ptr;	/* pointer to message log buffer */
   char  get_pkt_type( unsigned int, short );
   int id;
   int call_nbr;
   int gps_distance;
   int tmp_vh_nbr;
   int itm_retries = 0;
   char tmp_itm_buf[32];
   struct cisam_cl  *tmp_cl_ptr;
   CALL_HNDL call_hndl;
   short fl_nbr;
   extern char *scratch_ptr;
   FLEET_HNDL  fleet_hndl;
   ml_ptr = (struct cisam_ml *) req_buf;	/* initialize pointer to
						 * request buffer */

   strncpy(pkt_id, msg->pkt_id, sizeof(msg->pkt_id));
   pkt_id[2] = '\0';
   if (strncmp(pkt_id, last_pkt_id, 2) == 0)
   {
      sprintf(scratch_ptr, " duplicate pkt ids %c%c", pkt_id[0], pkt_id[1]);
      ERROR(' ', "", scratch_ptr);
   }
   strncpy(last_pkt_id, pkt_id, 2);

   id = (unsigned int) strtol(pkt_id, (char **) 0, 16);
   switch (get_pkt_type(id, veh_ptr->veh_nbr))
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
	 mk_nak_error(msg, "Nak: Null veh_ptr");
      } else
      {                         /* with vehicle info */

	 /* Log NACK on Trip Details to Call History */
	 if ((veh_ptr->call_ptr != NULL))
	   if(bit_check(SYS_ERR,veh_ptr->call_ptr) <= 0) /* Check if SYS_ERR exception already occurred */
	     log_assign_nak(SYS_ERR, (struct calls *) veh_ptr->call_ptr);

	 mk_except((struct calls *) veh_ptr->call_ptr, veh_ptr, SYS_ERR, ASSIGN_ERR, -1);
	 mk_nak_error(msg, "Nak: trip assign");
      }
      reset_pkt_entry(id, veh_ptr->veh_nbr);
      break;

   case MSG_PKT_TYPE:
      save_undeliv(get_pkt_num(id, veh_ptr->veh_nbr));
      reset_pkt_entry(id, veh_ptr->veh_nbr);
      break;

   case ITM_MDT_PKT_TYPE:
      break;

   case OFFER_PKT_TYPE:			/* NAK on call offer */
      call_nbr = get_pkt_num(id, veh_ptr->veh_nbr);
      if (reset_pkt_entry(id, veh_ptr->veh_nbr) == FAIL)
	{
	  sprintf(trace_str, "OFFER PKT Reset fail - Offer call %d", veh_ptr->call_nbr);
	  ERROR(' ', "", trace_str);
	}
      if ((call_hndl = Calls_list_find(call_nbr)) == HNDL_ERR)
      {				/* Find call in index */
	 sprintf(scratch_ptr, "Cant find call %d from veh %d", call_nbr, veh_ptr->veh_nbr);
	 ERROR(' ', " ", scratch_ptr);
	 break;
      }
#ifdef FOO
      (void) Fleet_get_hndl(BY_NBR,(int) Veh_get_value((VEH_HNDL) veh_ptr, VEH_FLEET_NBR), &fleet_hndl);
      if ( (char)Fleet_get_value( (FLEET_HNDL)fleet_hndl, FLEET_GPS_STATE ) == YES )
	{
	  if ( ( veh_ptr->gps_long > 0.0 ) &&
	      (  veh_ptr->gps_lat  > 0.0 ) &&
	      (  veh_ptr->call_ptr->gps_lat  > 0.0 ) &&
	      (  veh_ptr->call_ptr->gps_long > 0.0 ) )
	    gps_distance = (int)GPS_veh_call_get_distance((VEH_HNDL)veh_ptr, (CALL_HNDL)veh_ptr->call_ptr);
	  else
	    gps_distance = 0;	  
	  Call_add_history((CALL_HNDL) call_hndl, (VEH_HNDL) veh_ptr, NAK_CALL, veh_ptr->zone_num, 0L,
			   (int)gps_distance);
	}
      else
#endif
	Call_add_history((CALL_HNDL) call_hndl, (VEH_HNDL) veh_ptr, NAK_CALL, veh_ptr->zone_num, 0L,
			 (int)0);	

      if (veh_ptr->call_ptr == NULL)
         break;        /* assume no accept timeout */
      if (veh_ptr->call_ptr->call_number != call_nbr)
         break;       /* also assume no accept timeout */

      call_ptr = (struct calls *) call_hndl;
      fl_nbr = (int) Veh_get_value((VEH_HNDL) veh_ptr, VEH_FLEET_NBR);
      (void) Fleet_get_hndl(BY_NBR, fl_nbr, &fleet_hndl);

      /** It is possible for ACCEPT to arrive before NAK so don't reset call state **/
      /** if this has occurred                                                     **/
      if ( !Call_get_state((CALL_HNDL)call_ptr, CALL_ACCEPT_STATE) )
	Call_reset_state((CALL_HNDL) call_ptr, CALL_OFFERED_STATE, CALL_NAK_ON_CALL_OFFER);
      break;

    case SP_MDT_PKT_TYPE:
      if (0)
	log_ch_nack( SYS_ERR, veh_ptr->call_ptr, MDT,
		     (char)get_pkt_num( id, veh_ptr->veh_nbr ) );
      reset_pkt_entry(id, veh_ptr->veh_nbr);
      break;
	

   default:
      if (veh_ptr == NULL)
	 mk_nak_error(msg, "Nak: no veh_ptr");
      else
	 mk_nak_error(msg, "Nak: bad pkt id");
   }
}            /* end nak_by_mtype */

/*****************************************************************************
 *
 *===>  nak_msg_err()(void)             - Processes improper messages.
 *
 * 	DESCRIPTION:
 *		The following function writes out an error for a message that
 *	can't be processed.
 *
 * 	PARMETERS:
 *              msg(MSG_QUEUE_HDR *)            - Inbound messages.
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
nak_msg_err(msg)
MSG_QUEUE_HDR	*msg;
{
#ifdef FOO
        mk_nak_error(msg, "Bad msg hdr");
#endif
}

/*****************************************************************************
 *
 *===>  form_hndl()(void)               - Process Form Messages.
 *
 * 	DESCRIPTION:
 *		The following processes the forms message type.
 *
 * 	PARMETERS:
 *              msg(MSG_QUEUE_HDR *)            - Inbound messages.
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
form_hndl(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct	veh_driv	*veh_ptr;
char	*term_id;
{
  FLEET_HNDL fleet_hndl;
  char  *pGPSdata;
  int   fl_nbr;
  
	if(veh_ptr == NULL) { 						/* Check for Invalid RFID */
		return;
	}

        fl_nbr = (int) Veh_get_value((VEH_HNDL)veh_ptr, VEH_FLEET_NBR);
        (void) Fleet_get_hndl(BY_NBR, fl_nbr, &fleet_hndl);        
        if ( fleet_hndl == HNDL_NULL )
          return;

	if(!veh_ptr->t_status.kdt_working || veh_ptr->t_status.emer)
	  {
	    if ( ( veh_ptr->t_status.emer ) && ( (char)Fleet_get_value( fleet_hndl, FLEET_GPS_STATE ) == YES ) )
	      {
		if ( ( pGPSdata = (char *)GPS_strstr( msg->text ) ) != NULL )
		  {
		    pGPSdata += 2;
		    local_gps_form_hndl( veh_ptr, pGPSdata );
		  }
	      }
	    return;							/* Message not to be processed */
	  }

	switch(msg->p_fields[P1]) {
		case FT_LOCAL_FORM:
			local_form_hndl(msg, veh_ptr, term_id);		/* Process it */
			break;

	case FT_ZONECHECK_FORM:
	  zonecheck_form_hndl(msg, veh_ptr, term_id);
	  break;

	case FT_NEW_LOCAL_FORM:
	  local_form_new_hndl(msg, veh_ptr, term_id);
	  break;
	  
		case FT_AUX_INP:
                        /* ITM input */
			switch(msg->p_fields[P2])
			  {
			  case AUXF_ITM:
			    itm_msg_hndl(msg,veh_ptr, term_id);
			    break;
			  case AUXF_GPS:
			     if ( ( pGPSdata = (char *)GPS_strstr( msg->text ) ) != NULL )
			       {
				 if ( strlen( pGPSdata ) == GPS_MESSAGE_LEN )
				   {
				     /* if GPS_UPDATE message contains any text before the GPS
					position, copy the message to aux pipe
					*/
				     if ( ( use_aux_pipes == TRUE ) && ( msg->text != pGPSdata ) )
				       {
					 strcat( msg->text, CHECK_CHAR_STR );
					 write( from_aux_device_fd, msg, msg->size + sizeof(msg->size) );
					 msg->text[strlen(msg->text)] = '\0';
				       }
				     pGPSdata += 2;
				     local_gps_form_hndl( veh_ptr, pGPSdata );
				     gps_msg_hndl( veh_ptr, MDT_MSG_GPS );
				   }
			      }
			      else if(use_aux_pipes == TRUE)
			      {
			          strcat(msg->text, CHECK_CHAR_STR);
			          write(from_aux_device_fd, msg, msg->size + sizeof(msg->size));
  			      }
			    break;
			  default:
			    break;
			  }
                        break;

		case FT_FILL_FORM:
		  kela_request_form_hndl(msg, veh_ptr, term_id);
		  break;
		default:
			if ( use_aux_pipes == TRUE )
			  {
			    strcat(msg->text, CHECK_CHAR_STR);
			    write( from_aux_device_fd, msg, msg->size + sizeof(msg->size) );
			  }
			nak_msg_err(msg);
			break;
	}
}

void kela_request_form_hndl(msg, veh_ptr, term_id)
      MSG_QUEUE_HDR *msg;
     struct veh_driv *veh_ptr;
     char *term_id;
{
  char kela_details_temp[TMP_STR_LEN];  
  int dbmode = ISFIRST;
  int i,jj;
  KELANODE_REC kelanode_rec;
  char *pField, *pField2;
  int countKelaNodes,len;
  int booking_id;
  unsigned char id_array[32];
  unsigned char *byte_ptr;
  unsigned char mask;
  char tmp_pax[32];  

  bzero(id_array, 32);
  bzero(&kelanode_rec, sizeof(KELANODE_REC));
  bzero(kela_details_temp, TMP_STR_LEN);

  if ( ( pField = strstr(msg->text, FIELD_END ) ) != NULL )
    {
      pField2 = pField + 2; // booking ID list field
      msg->text[strlen(msg->text)-4] = '\0';
      booking_id = (int)strtol(pField2, (char **) 0, 16);
      byte_ptr = (unsigned char *)booking_id;
      mask = 0x80;
      for (jj=0;jj<=8;jj++)
	{
	  if (booking_id & mask)
	    id_array[jj]=1;
	  mask >>=1;
	}
      
      db_open(KELANODE_FILE_ID, ISINOUT | ISMANULOCK);

      countKelaNodes = 0;
      dbmode = ISEQUAL;
      strncpy(kelanode_rec.rte_id, msg->text,(pField-msg->text));
      for (i=0; i< 8; i++)
	{
	  if (id_array[i] == 1) // need this booking ID
	    {
	      kelanode_rec.nbr = i+1;
	      if (db_read_key(KELANODE_FILE_ID, &kelanode_rec, &kelanode_key1, dbmode) == SUCCESS)
		{
		  strcpy(tmp_pax, kelanode_rec.passenger);
		  if (strstr(tmp_pax,"/") != NULL)
		    {
		      *(strstr(tmp_pax,"/")) = ' ';
		    }		  
		  if (countKelaNodes==0)
		    sprintf(kela_details_temp, kelanode_rec.rte_id);
		  
		  countKelaNodes++;
		  sprintf(kela_details_temp, "%s;%02x;%020s;%s;x", kela_details_temp,
			  kelanode_rec.nbr,
			  kelanode_rec.full_booking_id,
			  tmp_pax);
		}
	    }
	}
      
      if (countKelaNodes > 0)
	{
	  mk_outb_text("");
	  strcat(kela_details_temp,";");
	  mk_outb_type('X');
	  mk_outb_text(kela_details_temp);
	  len = strlen(kela_details_temp);
	  send_msg_mmp(term_id, KELA_DETAILS, veh_ptr);
	}

      db_close(KELANODE_FILE_ID);

    }

}

void
zonecheck_form_hndl(msg, veh_ptr, term_id)
     MSG_QUEUE_HDR *msg;
     struct veh_driv *veh_ptr;
     char *term_id;
{
  switch(msg->p_fields[P1]) {
  case '4':
    k_zonecheck(msg, veh_ptr, term_id);
    break;
  default:
    break;
  }
}


void
local_form_new_hndl(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{
	switch(msg->p_fields[P1]) {
	case LF_STAND_NOTIFY:
	  k_stand_notify(msg, veh_ptr, term_id, 0);	  
	  break;
	default:
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
 *              msg(MSG_QUEUE_HDR *)            - Inbound messages.
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
local_form_hndl(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{
	switch(msg->p_fields[P2]) {
		case LF_S_IN:
		    k_sign_in(msg->text, veh_ptr, term_id, 0);
			break;

		case LF_POST:
			k_post(msg, veh_ptr, term_id);
			break;

		case LF_EPOST:
			k_epost(msg, veh_ptr, term_id);
			break;

		case LF_MSG:
			k_msg(msg, veh_ptr, term_id);
			break;

		case LF_BID:
			k_bid(msg, veh_ptr, term_id);
			break;

		case LF_CPOST:
			k_cpost(msg, veh_ptr, term_id);
			break;

		case LF_UNPOST:
			k_unpost(msg, veh_ptr, term_id);
			break;

		case LF_ETA:
			k_eta(msg, veh_ptr, term_id);
			break;

		case LF_INQ:
			k_inquiry(msg, veh_ptr, term_id);
			break;

		case LF_NOSHOW:
			k_noshow(msg, veh_ptr, term_id, NON_ITM_NO_SHOW);
			break;

		      case LF_ACPT:
			k_accept(msg, veh_ptr, term_id);
			break;

		      case LF_MON:
			k_meter_on(veh_ptr, term_id);
			break;

                case LF_TAXI_ACCT:
                case LF_ACCT1:
                case LF_ACCT2:
                       if ((veh_ptr->term_type  == MDT_V02) || (msg->p_fields[P2] == LF_ACCT_V03))
                       {
                        k_account(msg, veh_ptr, term_id);
                        break;
                       }
                       /* fall through if V03 and not correct local form */

		default:
			nak_msg_err(msg);
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
 *              msg(MSG_QUEUE_HDR *)            - Inbound messages.
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
func_hndl(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{
	if(veh_ptr == NULL) { 						/* Check for Invalid RFID */
		return;
	}

	if(!veh_ptr->t_status.kdt_working || veh_ptr->t_status.emer)	/* Check KDT status */
		return;							/* Message not to be processed */

	switch(msg->p_fields[P1]) {
		case FCT_EMERG:
			k_emergency(veh_ptr, term_id);
			break;

		case FCT_CAN_MSG:
			can_msg_hndl(msg, veh_ptr, term_id);			/* Process it */
			break;
	
		case FCT_EXT_INP:
			ext_inp_hndl(msg, veh_ptr, term_id);
			break;

                case FCT_HIA:
			hia_hndl(msg, veh_ptr, term_id);
			break;

		default:
			nak_msg_err(msg);
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
 *              msg(MSG_QUEUE_HDR *)            - Inbound messages.
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
 *	==>	k_pckup()(void)			- Process pickup message.
 *	==>	k_close()(void)			- Process close message.
 *	==>	k_call_out()(void)		- Process call out message.
 *      ==>     nak_msg_err()(void)             - Processes improper messages.
 *
 *	RETURN:
 *		None.
 *
 *****************************************************************************/

void
can_msg_hndl(msg, veh_ptr, term_id)
MSG_QUEUE_HDR   *msg;
struct veh_driv *veh_ptr;
char 	        *term_id;
{
	static char str_temp[TMP_STR_LEN];

	switch(msg->p_fields[P2]) {
                case CM_ON_DATA:
                        /* ignore the on data channel message */
                        break;

		case CM_ACPT:
			k_accept(msg, veh_ptr, term_id);
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
			k_info(msg, veh_ptr, term_id);
			break;

		case CM_BREAK:
			k_break(veh_ptr, term_id);
			break;

		case CM_PCKUP:
			/** Stockholm has these functions removed **/
			if (!(0))
			  k_pckup( veh_ptr, term_id );
			break;

		case CM_CLOSE:
			if (!(0))
			  k_close( veh_ptr, term_id );
			break;

		case CM_COUT_V02:
                        if (veh_ptr->term_type == MDT_V02)
			   k_call_out(veh_ptr, term_id);
                        else
                           nak_msg_err(msg);
			break;

		case CM_COUT_V03:
                        if (veh_ptr->term_type ==  MDT_V02)
                           nak_msg_err(msg);
                        else
			   k_call_out(veh_ptr, term_id);
			break;

                case CM_POWERUP:
                        if (veh_ptr->zone_num != 0) /* If in zone the unbook */
			  k_unpost(msg, veh_ptr, term_id);
                        break;

		case CM_MFAULT:
			k_mfault(veh_ptr, term_id);
			break;

	case CM_ROTI:
	  if ( msg->text[0] == '1' )
	    k_roti(veh_ptr, term_id, 1);
	  else
	    k_roti(veh_ptr, term_id, 0);
	  break;
	  
		      case 'D':
			break;

		default:
			nak_msg_err(msg);
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
 *              msg(MSG_QUEUE_HDR *)            - Inbound messages.
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
ext_inp_hndl(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char 	*term_id;
{
	switch(msg->p_fields[P2]) {
		case EI_M_ON:
			k_meter_on(veh_ptr, term_id);
			break;

		case EI_M_OFF:
			k_meter_off(veh_ptr, term_id);
			break;

		default:
			nak_msg_err(msg);
			break;
	}
}

#ifdef NAMERICA
void
hia_hndl(msg, veh_ptr, term_id)
     MSG_QUEUE_HDR *msg;
     struct veh_driv *veh_ptr;
     char *term_id;
{
  char  radio_channel;
  CHANNEL_ENTRY *channel_ptr;
  int  found, i;
  extern char *scratch_ptr;

  i = 0;
  found = 0;
  radio_channel = msg->p_fields[P2];
  channel_ptr = &(offset->channels[i]);
  
  while ((strlen(channel_ptr->channel_id)!=0) && (!found))
    {
      channel_ptr = &(offset->channels[i]);
      i++;
      if (channel_ptr->channel_id[0] == radio_channel)
	found = TRUE;
    }
  
  if (veh_ptr == NULL)
    {
      sprintf(scratch_ptr, "NULL veh ptr (hia_hndl):mid (%s)", term_id);
      ERROR(' ', "", scratch_ptr);
    }
  
  if (found)
    veh_ptr->radio_channel = channel_ptr->channel_nbr;
  else
    {
      sprintf(scratch_ptr, " Channel ID (%c) not found:mid (%s)", radio_channel, term_id);
      ERROR(' ', "", scratch_ptr);
    } 
}
#else
void
hia_hndl(msg, veh_ptr, term_id)
     MSG_QUEUE_HDR *msg;
     struct veh_driv *veh_ptr;
     char *term_id;
{
}
#endif

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
/*         msg            outbound message                                   */
/*****************************************************************************/

int
k_account(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char *term_id;
{
  int  rc;
  char get_acct_type();

  /* vehicle is not signed on */
  if(!(SIGNED_ON(veh_ptr)))
    {
      /* notify driver that vehicle has not signed in */
      send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);
      return(SUCCESS);
    }

  /** don't process this message for Stockholm **/
  if (0)
    return(SUCCESS);
  
      rc = verify_acct(veh_ptr->term_type, veh_ptr , get_acct_type(veh_ptr->term_type, msg), msg->text);
      if (rc == GOOD_ACCOUNT)
         send_text_predefined (term_id, GOOD_ACT, veh_ptr);              
      else if (rc == BAD_ACCOUNT)
         send_text_predefined (term_id, BAD_ACT, veh_ptr);
      else /* CANT_VERIFY */
         send_text_predefined (term_id, NO_VERIFY, veh_ptr);

}                                    /* end k_account */

void
inb_msg_recover(fd)
/* attempt to get back in sync from the pipe */
{
  char ch;
  int num;

  num = 10;
  ch = ' ';
  while((ch != CHECK_CHAR) && (num > 0))
     num = read(fd, &ch,1);
  read(fd, &ch, 1);
}              /* end inb_msg_recover */

#define ASYNC_ERROR_MSG "Error in reading async pipe - message(s) lost"
int
get_inb_msg(fd, msg)
   int  fd;
   char *msg;
{
   int   num;
   char *ptr;
   char size[sizeof(int) + 1];
   int msg_size;

   /* read in the size n the priority field*/
   num = read(fd, &msg_size, sizeof(int));
   if (num <= 0)
      return(-1);
 
   msg += sizeof(int);   /* because we dont use the size  field */
 
   /* got a message folks */
   /* msg_size = atoi(size); */
   if (msg_size <= 0)
      return(-1);
   if (msg_size > MAX_MSG_SIZE)

   {
     inb_msg_recover(fd);
     ERROR(' ',"",ASYNC_ERROR_MSG);
     return(-1);
   }

   num = read(fd, msg, msg_size);

   if (msg[num - 2] != CHECK_CHAR)
     {
       inb_msg_recover(fd);
       ERROR(' ',"",ASYNC_ERROR_MSG);
       return(-1);
     }
   
   if (num != msg_size)
   {
      inb_msg_recover(fd);
      ERROR(' ',"",ASYNC_ERROR_MSG);
      return(-1);
   }


   
   //   if ( fd == inb_fd )
     msg[num - 2] = '\0'; /* overwrite the check character */
   return(num);
 }				/* end get_inb_msg */

set_TopLightOn()
{
	TopLight = 1;  /* set global for top light feature */

}

TopLightOn()
{
	return(TopLight);
}

send_RELAYOFF(struct veh_driv *veh_ptr)
{

	if (veh_ptr->term_type == MDT_ITM)
	{
		mk_outb_text("");
		itm_send_msg(veh_ptr->mid, veh_ptr, 0x1C, ""); 
	}

} 

/******************************************************************************
* emulate_kdt - perform  a limited set of kdt commands
*
******************************************************************************/
emulate_kdt(kdt_req)
struct kdt_emulate *kdt_req;
{
	MSG_QUEUE_HDR	msg;
	static char term_id[MID_SIZE + TMP_STR_LEN];
	struct veh_driv *veh_ptr;
	short fl_num;
	FLEET_HNDL   hFleet;
	VEH_HNDL     hVehicle;
	FLEET_VEH_NBR_LIST_HNDL  veh_nbr_list_hndl;
	
	Fleet_get_hndl( BY_ID, kdt_req->fleet_id, &hFleet );
	
	if(  hFleet == HNDL_NULL )
	  {
	    ERROR(kdt_req->fleet_id,"","emulate_kdt - Invalid fleet id");
	    return;
	  }

	veh_nbr_list_hndl = ( FLEET_VEH_NBR_LIST_HNDL ) Fleet_get_value( hFleet, FLEET_VEH_NBR_LIST );


	if ( (hVehicle = ( VEH_HNDL )Fleet_veh_nbr_list_find( veh_nbr_list_hndl, kdt_req->veh_nbr)) == NULL)
	  {
	    ERROR(kdt_req->fleet_id,"","emulate_kdt - Invalid veh_ptr");
	    return;
	}

	veh_ptr = ( struct veh_driv * ) hVehicle;
	
	if (veh_ptr->mid != NULL)
	{
		strncpy(term_id, veh_ptr->mid, MID_SIZE);
		term_id[MID_SIZE + 1] = NULL;
	}

	switch(kdt_req->action_code){
		case EMUL_POST: sprintf(msg.text,"%d",kdt_req->zone_nbr); 
				veh_ptr->priority = kdt_req->priority | 0x30;	/* convert short to char */
				strcpy(term_id, veh_ptr->mid);
				k_post(&msg, veh_ptr, term_id);
				break;
		case EMUL_CPOST:sprintf(msg.text,"%d",kdt_req->zone_nbr); 
				k_cpost(&msg, veh_ptr, term_id);
				break;
		case EMUL_EPOST:sprintf(msg.text,"%d",kdt_req->zone_nbr); 
				k_epost(&msg, veh_ptr, term_id);
				break;
		case EMUL_S_IN: sprintf(msg.text,"%d",kdt_req->drv_id); 
				k_sign_in(msg.text, veh_ptr, term_id, 0);   /* R4 change */
				break;
		case EMUL_S_OUT:k_sign_out(veh_ptr, term_id);
				break;

		case EMUL_RESET:sprintf(msg.text,"%d",0); 
				k_unpost(&msg, veh_ptr, term_id);		  
				break;

		case EMUL_DWNLD:
				veh_ptr->t_status.emer =0;
   				authorize(veh_ptr); 			/* authorize terminal for futher use */
				break;

		case EMUL_ACPT: k_accept(NULL, veh_ptr, term_id);
				break;

		case EMUL_REJ:	k_reject(veh_ptr, term_id);
				break;
				
		case EMUL_NOSHOW: k_noshow(&msg, veh_ptr, term_id, NON_ITM_NO_SHOW);
				break;

		case EMUL_FLAG:	k_flag(veh_ptr, term_id);
				break;

		case EMUL_PCKUP:k_meter_on(veh_ptr, term_id);
				break;

		case EMUL_CLOSE:k_meter_off(veh_ptr, term_id);
				break;
	case EMUL_MSG97:
	  sprintf(msg.text,"%s","97%D"); 
	  k_msg(&msg, veh_ptr, term_id);
	  break;
	case EMUL_MSG98:
	  sprintf(msg.text,"%s","98%D");
	  k_msg(&msg, veh_ptr, term_id);
	  break;
	case EMUL_MSG99:
	  sprintf(msg.text,"%s","99%D10");
	  k_msg(&msg, veh_ptr, term_id);
	  break;	  	  
		
		default :
			ERROR(' ',"","emulate_kdt -  invalid action");
	}
}

local_gps_form_hndl( veh_ptr, pGPSdata )
     struct veh_driv *veh_ptr;
     char *pGPSdata;
{
  double TaxiX = 0.0, TaxiY = 0.0;
  int lTaxiX, lTaxiY;
  time_t  nowtime,gps_time;
  char  date_gps[12], time_gps[12];
  char    *pRadix;
  char  Lat[ LAT_LEN ];
  char  Long[ LONG_LEN ];
  double Northing, Easting;
  double GPS_xmax,GPS_xmin,GPS_ymax,GPS_ymin;
  VEH_REC  veh_rec;
  FLEET_HNDL fleet_hndl;
  int    fl_nbr;

  if ( pGPSdata == NULL )
    return;


  fl_nbr = (int) Veh_get_value((VEH_HNDL)veh_ptr, VEH_FLEET_NBR);
  (void) Fleet_get_hndl(BY_NBR, fl_nbr, &fleet_hndl);        
  if ( fleet_hndl == HNDL_NULL )
    return;
  
  //#ifdef __alpha
  
  memcpy( Lat, (pGPSdata + LATITUDE_OFFSET), LAT_LEN );
  memcpy( Long, (pGPSdata + LONGITUDE_OFFSET), LONG_LEN );
  Lat[ LAT_LEN-1 ] = '\0';
  Long[ LONG_LEN-1 ] = '\0';

  /** Assume LAT of format +(-)xx.xxxxxx and
      LONG of format +(-)xxx.xxxxxx
      Need to change RADIX character to match locale in
      order to use 'atof' call **/
    
  Lat[ 3 ] = *nl_langinfo( RADIXCHAR );
  Long[ 4 ] = *nl_langinfo( RADIXCHAR );
  
  TaxiX = atof( Long );
  TaxiY = atof( Lat );

  if ( ( TaxiX <= 0.0 ) || ( TaxiY <= 0.0 ) )
    return;
  
  if ( (short)Fleet_get_value( fleet_hndl, FLEET_GPS_COORD_SYS ) == UTM )
    {
      LatLong_to_UTM(  TaxiY, TaxiX, &Northing, &Easting );
      TaxiX = Easting;
      TaxiY = Northing;
      /** Testing **/
      //      UTM_to_LatLong( Northing, Easting, &TaxiY, &TaxiX );
    }


      GPS_xmax = (double)fleet[veh_ptr->fleet_nbr]->gps_max_x;
      GPS_xmin = (double)fleet[veh_ptr->fleet_nbr]->gps_min_x;
      GPS_ymax = (double)fleet[veh_ptr->fleet_nbr]->gps_max_y;
      GPS_ymin = (double)fleet[veh_ptr->fleet_nbr]->gps_min_y;

  
  nowtime = time(0);
  if ( ( TaxiX > GPS_xmax ) || ( TaxiX < GPS_xmin ) ||
       ( TaxiY > GPS_ymax ) || ( TaxiY < GPS_ymin ) )
    {
      veh_ptr->gps_lat = 0.0;
      veh_ptr->gps_long = 0.0;
      Veh_set_value((VEH_HNDL)veh_ptr, VEH_GPS_STATE, (HNDL)0);
    }
  else
    {
      veh_ptr->gps_long = TaxiX;
      veh_ptr->gps_lat = TaxiY;
      get_pkup_date( date_gps );
      time_gps[0] = *(pGPSdata + TIME_OF_DAY_OFFSET);
      time_gps[1] = *(pGPSdata + TIME_OF_DAY_OFFSET + 1);
      time_gps[2] = ':';      
      time_gps[3] = *(pGPSdata + TIME_OF_DAY_OFFSET + 2);
      time_gps[4] = *(pGPSdata + TIME_OF_DAY_OFFSET + 3);

      // first calculate GPS time based on current date.  We initially make
      // this assumption because the GPS report does not contain the date only time
      gps_time = get_tm_from_date_time( date_gps, time_gps );

      // if the GPS position is 'far' in the future, it is probably from the previous day
      // adjust it accordingly
      if ( (nowtime - gps_time < 0 ) &&
	   (abs(nowtime - gps_time) > 3600 ) )
	gps_time -= 86400;

      Veh_set_value((VEH_HNDL)veh_ptr, VEH_GPS_TIME, (HNDL)gps_time);
      Veh_set_value((VEH_HNDL)veh_ptr, VEH_GPS_STATE, (HNDL)*(pGPSdata + RECV_STATUS_OFFSET));
    }

  /* update database record */
  bzero(&veh_rec, sizeof(VEH_REC));
  veh_rec.fleet = veh_ptr->fleet;
  veh_rec.nbr = veh_ptr->veh_nbr;
  if ( db_read_key(VEHICLE_FILE_ID, &veh_rec, &veh_key2, ISEQUAL) == SUCCESS )
    {
      veh_rec.gps_lat = veh_ptr->gps_lat;
      veh_rec.gps_long = veh_ptr->gps_long;
      db_update( VEHICLE_FILE_ID, &veh_rec );
    }

  /* add a new record to the tracking layer */
  Tracking_Layer_db_add( (VEH_HNDL) veh_ptr );

  // Check for emergency state of taxi and determine if emergency broadcast is required
  if ( Veh_get_state( (VEH_HNDL)veh_ptr, VEH_EMERGENCY_STATE ) )
    {
      // process list of exceptions to see if bcast params require a broadcast
      GPS_broadcast_emergency( (VEH_HNDL)veh_ptr );
    }
  
  //endif
  
  return;
}
