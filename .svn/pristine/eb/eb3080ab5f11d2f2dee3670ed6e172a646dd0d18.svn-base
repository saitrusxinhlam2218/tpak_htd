/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call_history.c,v $
*  @(#)  $Revision: 1.9 $
*  @(#)  $Date: 2001/03/12 16:40:39 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/Call/Call_history.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Call_history.c,v 1.9 2001/03/12 16:40:39 taxiadm Exp $";

#include <sys/types.h>		/* temp */
#include <sys/times.h>		/* temp */
#include <isam.h>
#include "taxipak.h"
#include "Object.h"
#include "taxi_error.h"
#include "mad_error.h"
#include "Call_public.h"
#include "Call_private.h"
#include "Call_db.h"
#include "path.h"
#include "Fleet_public.h"
#include "Call_hist_db.h"
#include "except.h"
#include "taxi_db.h"
#include "dispatch.h"
#define DECLARING
#include "writer.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "language.h"
#include "itm.h"

CALL_HIST_REC writer_buf;
extern struct fleets *fleet[FLEET_MAX];	/* shared memory fleet structure */

extern writer_add_hist();

/*****************************************************************/
/* RET_STATUS Call_add_history(call_hndl, veh_hndl, type, param) */
/* OP: formats an adds a hsitory record to the call history db   */
/* PRE: call exists                                              */
/* POST: story updated                                           */
/* ERROR: data base error                                        */
/*        illegal type value                                     */
/*****************************************************************/

RET_STATUS
Call_add_history(
   CALL_HNDL call_hndl,
   VEH_HNDL veh_hndl,
   short type,
   short param,
   char  param2,
   int   param3)
{
   struct calls *call_ptr;
   struct veh_driv *veh_ptr;
   char cur_dt[10];
   char cur_tm[7];
   char temp_buf[32];
   time_t  cur_time = 0;
   
   call_ptr = (struct calls *) call_hndl;
   veh_ptr = (struct veh_driv *) veh_hndl;
   cur_time = time((time_t *) 0);
   bzero( &writer_buf, sizeof( CALL_HIST_REC ) );


   switch (type)
   {
   case BID:
      writer_buf.event_type = REGULAR_EVENT;
      strcpy(writer_buf.event_desc, EVNT_BID);
      writer_buf.user_id = SYSTEM_ID;
      writer_buf.action_veh_nbr = 0;
      writer_buf.action_drv_id = 0;
      break;

   case EXTRA_TAXIS:
      writer_buf.event_type = REGULAR_EVENT;
      strcpy(writer_buf.event_desc, EVNT_EXTRACAB);
      writer_buf.user_id = SYSTEM_ID;
      writer_buf.action_veh_nbr = 0;
      writer_buf.action_drv_id = 0;
      break;

   case CALLOUT:
      writer_buf.event_type = REGULAR_EVENT;
      strcpy(writer_buf.event_desc, EVNT_C_OUT);
      writer_buf.user_id = 0;
      strcpy(writer_buf.action_taken, "");
      break;

   case CALL_TIMEOUT:
      writer_buf.event_type = REGULAR_EVENT;
      strcpy(writer_buf.event_desc, EVNT_TIMEOUT);
      writer_buf.user_id = SYSTEM_ID;
      strcpy(writer_buf.action_taken, "");
      break;

   case CALL_CANCEL:
      writer_buf.event_type = REGULAR_EVENT;
      strcpy(writer_buf.event_desc, ACT_CANCEL);
      get_asc_time(cur_time, cur_dt, cur_tm);
      strcpy(writer_buf.action_date, cur_dt);
      strcpy(writer_buf.action_time, cur_tm);
      writer_buf.user_id = param;
      break;

   case COMMENT_EXC:
      writer_buf.event_type = REGULAR_EVENT;
      strcpy(writer_buf.event_desc, EVNT_COMMENT);
      writer_buf.user_id = param;
      writer_buf.action_taken[0] = NULL;
      break;

   case ETA:
      writer_buf.event_type = REGULAR_EVENT;
      strcpy(writer_buf.event_desc, EVNT_ETA);
      writer_buf.user_id = param;	/* eta */
      strcpy(writer_buf.action_taken, "");
      break;

   case FLAG_REQ:
      if (param == TAKEN)
      {
	 strcpy(writer_buf.action_taken, "");
	 get_asc_time(cur_time, cur_dt, cur_tm);
	 strcpy(writer_buf.action_date, cur_dt);
	 strcpy(writer_buf.action_time, cur_tm);
	 strcpy(writer_buf.action_taken, ACT_APPROVE);
	 writer_buf.action_user_id = SYSTEM_ID;
      }
      else if ( param == 33 )
	{
	  strcpy(writer_buf.action_taken, "");
	  get_asc_time(cur_time, cur_dt, cur_tm);
	  strcpy(writer_buf.action_date, cur_dt);
	  strcpy(writer_buf.action_time, cur_tm);
	  strcpy(writer_buf.action_taken, ACT_DISAPPROVE);
	  writer_buf.action_user_id = SYSTEM_ID;
	}
      writer_buf.action_user_id = 0;
      writer_buf.event_type = REGULAR_EVENT;
      strcpy(writer_buf.event_desc, EVNT_FLAG_REQ);
      writer_buf.user_id = 0;
      writer_buf.action_veh_nbr = 0;
      writer_buf.action_drv_id = 0;
      break;

   case LATE_METER:
      writer_buf.event_type = REGULAR_EVENT;
      strcpy(writer_buf.event_desc, EVNT_LATEMET);
      writer_buf.user_id = SYSTEM_ID;
      strcpy(writer_buf.action_taken, "");
      break;

   case MANUAL:
      writer_buf.event_type = REGULAR_EVENT;
      strcpy(writer_buf.event_desc, EVNT_MANUAL);
      writer_buf.user_id = 0;
      strcpy(writer_buf.action_taken, "");
      break;

   case MESSAGE:
      writer_buf.event_type = REGULAR_EVENT;
      strcpy(writer_buf.event_desc, EVNT_MSGFRDRV);
      writer_buf.user_id = param;	/* msg_nbr */
      strcpy(writer_buf.action_taken, "");
      break;

   case MFAULT:
      writer_buf.exception_nbr = MFAULT;
      strcpy(writer_buf.event_desc, EVNT_MFAULT);
      writer_buf.user_id = 0;
      strcpy(writer_buf.action_taken, "");
      break;

   case NONE:
      writer_buf.event_type = REGULAR_EVENT;
      strcpy(writer_buf.action_taken, "");

      switch (param)
      {
      case CALL_ACCEPT_STATE:
	 strcpy(writer_buf.event_desc, EVNT_ACCEPT);
	 writer_buf.user_id = veh_ptr->zone_num;    
	 break;

      case CALL_CLOSED_STATE:
	 strcpy(writer_buf.event_desc, EVNT_CLOSE);
	 writer_buf.user_id = 0;
	 break;

      case CALL_PICKUP_STATE:
	 strcpy(writer_buf.event_desc, EVNT_PCKUP);
	 writer_buf.user_id = 0;
	 break;

      default:
	 ERROR(' ', "", " Illegal call history request ");
      }				/* end case */
      break;

   case HELD_EPOST:
     writer_buf.event_type = REGULAR_EVENT;
     strcpy(writer_buf.event_desc, EVNT_HOLD_STC);
     writer_buf.action_veh_nbr = 0;
     writer_buf.action_drv_id = 0;
     strcpy(writer_buf.action_taken, "");
     strcpy(writer_buf.action_date, "");
     strcpy(writer_buf.action_time, "");
     writer_buf.action_user_id = 0;
     break;

   case RELEASE_EPOST:
     writer_buf.event_type = REGULAR_EVENT;
     strcpy(writer_buf.event_desc, EVNT_RLSE_STC);
     writer_buf.action_veh_nbr = 0;
     writer_buf.action_drv_id = 0;
     strcpy(writer_buf.action_taken, "");
     strcpy(writer_buf.action_date, "");
     strcpy(writer_buf.action_time, "");
     writer_buf.action_user_id = 0;
     break;     
     
   case NO_SHOW:
      writer_buf.event_type = REGULAR_EVENT;
      if ( param3 == 3000 )
	{
	  strcpy( writer_buf.event_desc, "TXMBOM" );
	  writer_buf.user_id = -1;
	}
      else
	{
	  strcpy(writer_buf.event_desc, EVNT_NOSHOW);
	  writer_buf.user_id = param3;
	}
      
      writer_buf.action_veh_nbr = 0;
      writer_buf.action_drv_id = 0;

      if (param == '3')
      {				/* if automatic */
	 get_asc_time(cur_time, cur_dt, cur_tm);
	 strcpy(writer_buf.action_taken, ACT_APPROVE);
	 strcpy(writer_buf.action_date, cur_dt);
	 strcpy(writer_buf.action_time, cur_tm);
	 writer_buf.action_user_id = SYSTEM_ID;
      } else
      {
	 strcpy(writer_buf.action_taken, "");
         strcpy(writer_buf.action_date, "");
         strcpy(writer_buf.action_time, "");
	 writer_buf.action_user_id = 0;
      }
      break;

   case NS_SHORT:
      writer_buf.event_type = REGULAR_EVENT;

      strcpy(writer_buf.event_desc, "KORT BOM");
      writer_buf.user_id = param3;
      
      writer_buf.action_veh_nbr = 0;
      writer_buf.action_drv_id = 0;

      if (param == '3')
      {				/* if automatic */
	 get_asc_time(cur_time, cur_dt, cur_tm);
	 strcpy(writer_buf.action_taken, ACT_APPROVE);
	 strcpy(writer_buf.action_date, cur_dt);
	 strcpy(writer_buf.action_time, cur_tm);
	 writer_buf.action_user_id = SYSTEM_ID;
      } else
      {
	 strcpy(writer_buf.action_taken, "");
         strcpy(writer_buf.action_date, "");
         strcpy(writer_buf.action_time, "");
	 writer_buf.action_user_id = 0;
      }
      break;      

   case PERSONAL:
      writer_buf.event_type = REGULAR_EVENT;
      strcpy(writer_buf.event_desc, EVNT_PERSONAL);
      writer_buf.user_id = SYSTEM_ID;
      strcpy(writer_buf.action_taken, "");
      strcpy(writer_buf.action_date, cur_dt);
      strcpy(writer_buf.action_time, cur_tm);
      writer_buf.action_veh_nbr = 0;
      writer_buf.action_drv_id = 0;
      writer_buf.action_user_id = 0;
      break;

   case REQ_TO_TLK:
	 writer_buf.event_type = REGULAR_EVENT;
	 strcpy(writer_buf.event_desc, EVNT_TALK);
	 writer_buf.user_id = 0;
	 strcpy(writer_buf.action_taken, "");
	 break;

   case REJECT:
	 get_asc_time(cur_time, cur_dt, cur_tm);
	 writer_buf.event_type = REGULAR_EVENT;
	 strcpy(writer_buf.event_desc, EVNT_REJECT);
	 if (param)
	    writer_buf.user_id = SYSTEM_ID;
	 else
	    writer_buf.user_id = 0;
	 strcpy(writer_buf.action_taken, "");
	 strcpy(writer_buf.action_date, cur_dt);
	 strcpy(writer_buf.action_time, cur_tm);
	 writer_buf.action_veh_nbr = 0;
	 writer_buf.action_drv_id = 0;
	 writer_buf.action_user_id = 0;
	 break;

   case SHORT_METER:
	 writer_buf.event_type = REGULAR_EVENT;
	 strcpy(writer_buf.event_desc, EVNT_SHORTMET);
	 writer_buf.user_id = 0;
	 strcpy(writer_buf.action_taken, "");
	 break;

   case SPECIAL:
	 writer_buf.event_type = REGULAR_EVENT;
	 writer_buf.user_id = param;
       
	 strcpy(writer_buf.event_desc, call_ptr->pickup_pmap);
#ifdef FOO
	 if (call_ptr->except_ptr != NULL)
	 	writer_buf.exception_nbr = call_ptr->except_ptr->nbr;
	 else
		writer_buf.exception_nbr = 0;
#endif
	 writer_buf.action_taken[0] = NULL;
	 break;

   case SUP_HOLD:
	 get_asc_time(cur_time, cur_dt, cur_tm);
	 writer_buf.exception_nbr = SUP_HOLD;
	 writer_buf.event_type = REGULAR_EVENT;
         if (param)
	    strcpy(writer_buf.event_desc, ACT_HOLD);
	 else
	    strcpy(writer_buf.event_desc, EVNT_REJECT);
	 writer_buf.user_id = SYSTEM_ID;
	 strcpy(writer_buf.action_taken, "");
	 strcpy(writer_buf.action_date, cur_dt);
	 strcpy(writer_buf.action_time, cur_tm);
	 writer_buf.action_veh_nbr = 0;
	 writer_buf.action_drv_id = 0;
	 writer_buf.action_user_id = 0;
	 break;

   case TIME_CALL:
	 writer_buf.event_type = REGULAR_EVENT;
         strcpy(writer_buf.event_desc, EVNT_TIME_C);
         writer_buf.user_id = SYSTEM_ID;
         strcpy(writer_buf.action_taken, "");
         writer_buf.veh_nbr = param;
         writer_buf.drv_id = 0;
	 break;

    case NAK_CALL:
         writer_buf.event_type = REGULAR_EVENT;
         strcpy(writer_buf.event_desc, EVNT_NAK_CALL);
         writer_buf.user_id = param;
         sprintf(writer_buf.action_taken, "%d", param3);
         writer_buf.veh_nbr = veh_ptr->veh_nbr;
         writer_buf.drv_id = 0;
         break;

    case ACK_CALL:
         writer_buf.event_type = REGULAR_EVENT;
         strcpy(writer_buf.event_desc, EVNT_ACK_CALL);
         writer_buf.user_id = param;
         sprintf(writer_buf.action_taken, "%d", param3);      
         writer_buf.veh_nbr = veh_ptr->veh_nbr;
         writer_buf.drv_id = 0;
         break;

   default:
      return ( FAIL );
   }            /* end case */

   if (type != SPECIAL)
   	writer_buf.exception_nbr = type;

   if (call_ptr != NULL)
   {
      writer_buf.fleet = Fleet_get_id(call_ptr->fleet);
      writer_buf.call_nbr = call_ptr->call_number;
   } 
   else
   {
      writer_buf.fleet = Fleet_get_id(veh_ptr->fleet_nbr);
      writer_buf.call_nbr = 0;
   }

   if (type != TIME_CALL)
   	if (veh_ptr == NULL)
   	{
      		writer_buf.veh_nbr = 0;
      		writer_buf.drv_id = 0;
   	} 
   	else
   	{
      		writer_buf.veh_nbr = veh_ptr->veh_nbr;
      		writer_buf.drv_id = veh_ptr->driver_id;
   	}

#ifdef NAMERICA
   if ( ( type != HELD_EPOST ) && ( type != RELEASE_EPOST ) )
     {
       if (writer_add_hist(&writer_buf) == 1)
	 return (SUCCESS);
       else
	 return(FAIL);
     }
#else
   if (writer_add_hist(&writer_buf) == 1)
     return (SUCCESS);
   else
     return(FAIL);
#endif
}				/* end Call_add_history */


/***********************************************************************************************************************************
writer_add_hist -
Called by DISPATCH to add a new history record for a call.  The CALLH file is opened, a record is created from the information
passed in parameters, the call record is written to the file, and the file is closed.
***********************************************************************************************************************************/
writer_add_hist(chp)
   CALL_HIST_REC *chp;
{
   CALL_HIST_REC ch_ptr;
   CALL_REC cl_ptr;
   int   call_nbr, call_save, call_next;
   int  callhAdded = 0;
   char cur_dt[10];
   char cur_tm[7];
   time_t cur_time = 0;


   cur_time = time(0);
   memset( &cl_ptr, '\0', sizeof( CALL_REC ) );
   get_asc_time(cur_time, cur_dt, cur_tm);	/* get current time */

   call_save = call_nbr = chp->call_nbr;

   while (call_nbr != 0)
   {

      cl_ptr.nbr = call_nbr;
      if (db_read_key(CALL_FILE_ID, &cl_ptr, &cl_key1, ISEQUAL) < 0)
      {				/* read CALL record */
	 sprintf(error_str, "Error reading record: call_nbr %d", chp->call_nbr);
	 ERROR(' ', CALL_FILE, error_str);
	 return (-1);		/* if error, return */
      }
      call_next = cl_ptr.grouped_with;
      ch_ptr.call_nbr = call_nbr;	/* fill in call number */
      ch_ptr.exception_nbr = chp->exception_nbr;	/* no exception numbmer
							 * at first */
      ch_ptr.fleet = chp->fleet;	/* fill in fleet */
      ch_ptr.event_type = chp->event_type;	/* Regular event */
      ch_ptr.status_dt_tm = cur_time;	/* status date/time is current
					 * date/time */
      strncpy(ch_ptr.status_date, cur_dt, DATE_LEN);	/* in printable format */
      strncpy(ch_ptr.status_time, cur_tm, TIME_LEN);	/* in printable format */
      strncpy(ch_ptr.event_desc, chp->event_desc, 8);	/* event description is
							 * status */
      ch_ptr.veh_nbr = chp->veh_nbr;	/* fill in vehicle number */
      ch_ptr.drv_id = chp->drv_id;	/* fill in driver id */
      ch_ptr.user_id = chp->user_id;	/* fill in user id */
      if ((chp->action_taken[0] != '\0') && (strncmp(chp->action_taken, "        ", 8) != 0))
      {
	 strncpy(ch_ptr.action_taken, chp->action_taken, 8);	/* fill in action taken */
	 strncpy(ch_ptr.action_date, chp->action_date, DATE_LEN);	/* action date,
									 * printable format */
	 strncpy(ch_ptr.action_time, chp->action_time, TIME_LEN);	/* action time,
									 * printable format */
	 ch_ptr.action_dt_tm = chp->action_dt_tm;	/* action date/time is
							 * current date/time */
	 ch_ptr.action_veh_nbr = chp->action_veh_nbr;	/* fill in action
							 * vehicle number */
	 ch_ptr.action_drv_id = chp->action_drv_id;	/* fill in action driver
							 * id */
	 ch_ptr.action_user_id = chp->action_user_id;	/* fill in action user
							 * id */
      } else
      {
	 strncpy(ch_ptr.action_taken, "        ", 8);	/* fill in action taken */
	 strncpy(ch_ptr.action_date, "         ", DATE_LEN);	/* action date,
								 * printable format */
	 strncpy(ch_ptr.action_time, "      ", TIME_LEN);	/* action time,
								 * printable format */
	 ch_ptr.action_dt_tm = 0;	/* action date/time is current
					 * date/time */
	 ch_ptr.action_veh_nbr = 0;	/* fill in action vehicle number */
	 ch_ptr.action_drv_id = 0;	/* fill in action driver id */
	 ch_ptr.action_user_id = 0;	/* fill in action user id */
      }

      if (db_write(CALLH_FILE_ID, &ch_ptr) < 0)
      {				/* write CALLH record to file */
	 sprintf(error_str, "Error writing new record: call_nbr %d", chp->call_nbr);
	 ERROR(ch_ptr.fleet, CALLH_FILE, error_str);
	 return (-1);		/* if error, return */
      }
      call_nbr = call_next;
      callhAdded++;
      if ((call_nbr == call_save) || (callhAdded >= 10)) /* Don't do first call again */
	 call_nbr = 0;
   }
   return (1);
}
