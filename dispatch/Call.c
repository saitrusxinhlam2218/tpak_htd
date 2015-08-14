/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call.c,v $
*  @(#)  $Revision: 1.53 $
*  @(#)  $Date: 2005/04/21 06:43:47 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Call.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Call.c,v 1.53 2005/04/21 06:43:47 jwelch Exp $";


#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include "taxipak.h"
#include "Object.h"
#include "List_public.h"
#include "Call_public.h"
#include "Call_private.h"
#include "Call_db.h"
#include "Vehicle_public.h"
#include "TimeCalls_public.h"
#include "TimeCalls_private.h"
#include "Zone_public.h"
#include "Zone_lists_public.h"
#include "Call_lists_public.h"
#include "Call_timers_public.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Fleet_lists_public.h"
#include "mad_ipc.h"
#include "taxi_db.h"
#include "language.h"
#include "path.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "Call_msg_public.h"
#include "Call_msg_private.h"
#include "GPS_private.h"
#include "dispatch.h"
#include "dispatch_strdefs.h"
#include "kdt.h"
#include "msg_mmp.h"
#include "f_msg_defs.h"
#include "taxi_db.h"
#include "writer.h"

#include "ui_ipc.h"
#include "timers.h"
#include "mad_error.h"
#include "except.h"
#include "mesg.h"
#include "Message_log_db.h"
#include "enhance.h"
#include "switch_ext.h"
#include "Interlink_db.h"

extern struct cisam_ch writer_buf;
extern struct offsets *offset;
extern time_t mads_time;
extern int warm_start;		/* BOO */
extern FLEET *fleet[ ];
extern CALLS_LIST_HNDL calls_list;
extern char req_buf[];
extern int free_calls_nbr;
extern MSG_QUEUE_HDR outb;
int call_node_err = 0;
RET_STATUS re_build_calls();
BOOLEAN Call_is_time_call();   
RET_STATUS Call_change_zone(CALL_HNDL, short);
extern int warm_start;
extern int num_tc;

#define RANDOM(min,max) (rand()%(int)((max)-(min))+(min)+1)


set_seed()			/* BECOMES no change */
{
   struct tm *tt = NULL;
   time_t  t;

   t = time(0);
   tt = localtime(&t);
   srand(tt->tm_sec);
}

/*************************************************************************/
/* RET_STATUS Call_cancel(call_hndl, veh_hndl, db_update)                */
/*   OP: call has been canceled                                          */
/*   PRE: call exists                                                    */
/*        vehicle exists if call in offered or assigned state            */
/*   POST:                                                               */
/*       if CALL_VEH_HNDL                                                */
/*            Veh_call_cancel called                                     */
/*       any outstanding requests cancelled                              */
/*       Call_update_counts (DECREMENT_NOW) called                       */
/*       Call_deactivate called                                          */
/*   ERROR: none                                                         */
/*************************************************************************/

RET_STATUS
Call_cancel(CALL_HNDL call_hndl, VEH_HNDL veh_hndl, short update, int no_show)	/* Was call_cancel  call to
					 * drv_cancel */
{
   struct calls *call_ptr;
   char error_str[80];
   ZONE_HNDL   zone_hndl = HNDL_NULL;
   FLEET_HNDL  fleet_hndl = HNDL_NULL;
   VEH_HNDL    epost_veh_hndl = HNDL_NULL;
   ZONE_VEH_LIST_HNDL veh_list_hndl = HNDL_NULL;
   ILINK_DB_REC  ilink_rec;
   RET_STATUS rc;
   
   if (veh_hndl != NULL)
      Veh_call_cancel(veh_hndl, call_hndl);

   /* remove the reference to any vehicle */
  
   call_ptr = (struct calls *) call_hndl;

   (void) Fleet_get_hndl(BY_NBR, (short)Call_get_value( call_hndl, CALL_FLEET_NBR ), &fleet_hndl);
   if ( fleet_hndl == HNDL_NULL )
     {
       sprintf(error_str, "Call cancel with NULL fleet");
       ERROR(' ',"", error_str);
       return(FAIL);
     }       
   call_ptr->veh_ptr = NULL;
   
   if (call_ptr->call_number == 0)
   {
        sprintf(error_str, "Call cancel with 0 call nbr due time %d", call_ptr->due_time);
        ERROR(' ',"", error_str);
        return(FAIL);
   }

   (void) Zone_get_hndl( call_ptr->pickup_zone,
			(short) Call_get_value((CALL_HNDL)call_ptr, CALL_FLEET_NBR),
			&zone_hndl );
   Zone_remove_tc( zone_hndl, (CALL_HNDL) call_ptr );
   
   if (update)
   {
         Call_add_history(call_hndl, (VEH_HNDL) veh_hndl, CALL_CANCEL, 0, 0L, 0);
         writer_cancel(call_ptr->call_number);
   }

   if ( (call_ptr->except_ptr != NULL) )
     {
       if ( call_ptr->status.callback == 1 &&
            no_show == 3000 )
         {
           lock_ex(call_ptr, call_ptr->except_ptr, LOCK_REMOVE);
           call_ptr->status.callback = 0;
           mk_except(call_ptr, NULL, CALLBACK, 0, -1);
         }
       else
         lock_ex(call_ptr, call_ptr->except_ptr, LOCK_REMOVE);
     }

   if ( Call_get_state( call_hndl, CALL_WAIT_EPOST_STATE ) )
     {
       veh_list_hndl = (ZONE_VEH_LIST_HNDL) Zone_get_value( zone_hndl, ZONE_AVAIL_VEH_LIST );
       Zone_veh_list_each_get( veh_list_hndl, epost_veh_hndl )
	 {
	   if ( ( Veh_get_state( epost_veh_hndl, VEH_EPOSTED_STATE ) ) &&
		( Veh_get_value( epost_veh_hndl, VEH_HELD_EPOST_CALL ) == call_hndl ) )
	     Zone_release_epost( zone_hndl, (VEH_HNDL) epost_veh_hndl );
	 } Zone_veh_list_each_end(veh_list_hndl)
     }

   Call_update_counts((CALL_HNDL) call_ptr, DECREMENT_NOW);

   // Notify Interlink if this was an order transferred from the TaxiPak system
   if ( ( (char)Fleet_get_value( fleet_hndl, FLEET_ILINK_ACTIVE ) == YES ) &&
        ( (short)Call_get_value( call_hndl, CALL_TYPE_ILINK ) == TRUE ) )
     {
       Call_deactivate((CALL_HNDL) call_ptr);
       return(SUCCESS);
     }
   
   if ( call_ptr->except_ptr == NULL )
     Call_deactivate((CALL_HNDL) call_ptr);

   if ( no_show != 3000 )
     Fleet_increment_stats( fleet_hndl, FLEET_NBR_CANCELS, 0 );

   return (SUCCESS);
}				/* end Call_cancel */


/********************************************************/
/* RET_STATUS Call_match(call_hndl)                     */
/* OP: preprocess a call prior to offering or matching  */
/* PRE: call exists                                     */
/* POST:                                                */
/*       if warmstart                                   */
/*           T_ZONE_TIMEOUT scheduled for call          */
/*       else                                           */
/*          if !ONLY_PERS_REQ || !IF_PERS_REQ           */
/*            Zone_add-call called                      */
/*            Dispatcher_call_match_called              */
/*          if ONLY_PERS_REQ && veh not signed on       */
/*             CALL_HOLD_STATE seT                      */
/*          if ONLY_PERS_REQ && VEH_IDLE_STATE          */
/*             Dispatcher_call_offer called             */
/*          if ONLY_PERS_REQ && !VEH_IDLE_STATE         */
/*             if VEH_TERM_WORKING_STATE                */
/*                T_ACCEPT time started                 */
/*                Call_send_offer called                */
/*                Call added to VEH_STACKED_CALLS_LIST  */
/*             else                                     */
/*                 MANUAL request generated             */
/*                 Call_add_history (MANUAL) called     */
/*          if IF_PERS_REQ && !VEH_SIGN_ON_STATE        */
/*             Zone_add_call called                     */
/*             Dispatcher_call_match called             */
/*          if IF_PERS_REQ && !VEH_IDLE_STATE           */
/*             if VEH_TERM_WORKING_STATE                */
/*                T_ACCEPT time started                 */
/*                Call_send_offer called                */
/*                Call added to VEH_STACKED_CALLS_LIST  */
/*             else                                     */
/*                 MANUAL request generated             */
/*                 Call_add_history (MANUAL) called     */
/*          if IF_PERS_REQ && VEH_IDLE_STATE            */
/*             if veh has rejected call                 */
/*                Dispatch_call_match called            */
/*             else                                     */
/*                Dispatcher_call_offer called          */
/* ERROR: if call due_time > CALL_PICKUP_LEAD_TIME      */
/*           if veh_hndl Veh_call_cancel called         */
/*           Call_deactivate called                     */
/*        Unable to add call to zone                    */
/*        Personal call and no veh_hndl                 */
/********************************************************/

RET_STATUS
Call_match(call_hndl)		/* WAS do_pers_n_match */
   CALL_HNDL call_hndl;
{
   struct calls *call_ptr;
   struct veh_driv *veh_ptr;
   CALL_REJECT_VEH_LIST_HNDL call_reject_veh_list;
   char  error_str[80];


   call_ptr = (struct calls *) call_hndl;

   if (call_ptr == NULL)
      return (FAIL);


   if (warm_start)
   {
      call_ptr->status.available = 1;
      if (Call_add_timer((CALL_HNDL) call_ptr, T_ZONE_TIMEOUT,
		 RANDOM(30, 240), (int (*)())call_zone_timeout_notify_func) != SUCCESS)
      {
	 ERROR(' ', "", "add call timer T_ZONE_TIMEOUT");
      }
      return (SUCCESS);
   }

   if (!((call_ptr->personal_request == ONLY_PERS_REQ) || (call_ptr->personal_request == IF_PERS_REQ)))
   {
      if (Zone_add_call(call_ptr->pickup_zone, (CALL_HNDL) call_ptr) == FAIL)
      {
	 call_ptr->pickup_zone = 0;
	 sprintf(error_str, "Call_match - zoned/unzoned call %d",
		 call_ptr->call_number);
	 ERROR(fleet[call_ptr->fleet]->fleet_id, "", error_str);
	 mk_except(call_ptr, (struct veh_driv *) NULL, UNZONED, call_ptr->msg_nbr, -1);
      } else
      {
	 call_ptr->status.available = 1;
	 Dispatcher_call_match((CALL_HNDL) call_ptr);
      }
      return (SUCCESS);
   }

   /* Personals handled here */

   /* personal call with an invalid vehicle number assigned */
   if (Veh_get_hndl(BY_NBR, call_ptr->personal_vehicle,
		    call_ptr->fleet, (VEH_HNDL *) & veh_ptr) == FAIL)
   {
      sprintf(error_str, "Invalid vehicle for personal call_ptr %x  # %d - ",
	      call_ptr, call_ptr->call_number);
      ERROR(fleet[call_ptr->fleet]->fleet_id, "", error_str);
      call_ptr->personal_vehicle = 0;
      call_ptr->personal_request = ' ';
      if (Zone_add_call(call_ptr->pickup_zone, (CALL_HNDL) call_ptr) == FAIL)
      {				/* bad zone */
	 call_ptr->pickup_zone = 0;	/* make 'em manually zone it */
	 sprintf(error_str, "Call_match - zoned/unzoned call %d",
		 call_ptr->call_number);
	 ERROR(fleet[call_ptr->fleet]->fleet_id, "", error_str);
	 mk_except(call_ptr, (struct veh_driv *) NULL, UNZONED, call_ptr->msg_nbr, -1);
      } else
      {
	 call_ptr->status.available = 1;
	 Dispatcher_call_match((CALL_HNDL) call_ptr);
      }
      return (SUCCESS);
   }

   /* personal flag ONLY_PERS_REQ is the only option allowed */
   if (call_ptr->personal_request == ONLY_PERS_REQ)
   {				/* if only, let them know */
     Call_add_history((CALL_HNDL) call_ptr, (VEH_HNDL) veh_ptr, PERSONAL, 0, 0L, 0);

     if (0)
       {
	 if ( ( Call_get_state( (CALL_HNDL) call_ptr, CALL_PERS_ATTR_OVERRIDE ) ) == FALSE )
	   {
	     /* Veh must be signed on to call Veh_check_attr */
	     if ( ( veh_ptr->signed_on_date > 0 ) && ( veh_ptr->signed_off_date == 0 ) )
	       {
		 if (Veh_check_attr( (VEH_HNDL) veh_ptr, (CALL_HNDL) call_ptr, FALSE ) == FALSE )
		   {
		     mk_except(call_ptr, NULL, ATTRIBUTE_HOLD, 0, -1);
		     return (SUCCESS);
		   }
	       }
	   }
       }
     
      if (( !veh_ptr->t_status.inuse) ||
	  ( veh_ptr->t_status.meter_on) ||
	  ( !veh_ptr->t_status.idle) ||
	  ( veh_ptr->t_status.on_break ) ||
	  ( veh_ptr->call_ptr != NULL ) )
	mk_except(call_ptr, veh_ptr, PERSONAL, 0, -1);
      else
	{
	  call_ptr->status.available = 1;
	  Dispatcher_call_offer((VEH_HNDL) veh_ptr, (CALL_HNDL) call_ptr, EXPLICIT_ACCEPT, 0);
	}
      return (SUCCESS);
   }

}				/* end Call_match */

/*******************************************/
/* RET_STATUS Call_services_start()        */
/* OP: call setup                          */
/* PRE: none                               */
/* POST: Calls_list created                */
/*       Calls_free_list created           */
/* ERROR: cannot creat lists               */
/*        lists exist                      */
/*******************************************/

RET_STATUS
Call_services_start()
{
   calls_list = Calls_list_create();
   set_seed();
   if (calls_list == HNDL_ERR)
   {
      ERROR(' ', "", " Could not create calls_list");
      cleanup();
      return (FAIL);
   }
   return (SUCCESS);
}				/* end Call_services_start  */

/************************************/
/* RET_STATUS Call_services_stop()  */
/* OP: terminate call services      */
/* PRE: none                        */
/* POST: Calls_list freed           */
/*       Calls_free_list freed      */
/* ERROR: creat lists               */
/*        lists dont exist          */
/************************************/

RET_STATUS
Call_services_stop()
{
   printf("Call_services_stop not implemented\n");
   ERROR(' ', "", "Call_services_stop not implemented");
   return (FAIL);
}				/* end Call_services_stop  */

/***********************************************/
/* RET_STATUS Call_deactivate(call_hndl)       */
/* OP: deletes a call from  memory             */
/* PRE:  call exists                           */
/* POST:                                       */
/*       if CALL_BID_STATE                     */
/*          CALL_BID_STATE reset               */
/*       else                                  */
/*          any timers deleted                 */
/*       Zone_remove_call called               */
/*       if no exceptions                      */
/*          all list destroyed                 */
/*          call removed from calls_list       */
/* ERROR: unable to remove from lists          */
/***********************************************/

RET_STATUS
Call_deactivate(call_hndl)	/* WAS call_cancel *& remove_call */
   CALL_HNDL call_hndl;
{
   struct calls *call_ptr;
   FLEET_HNDL   fleet_hndl;
   char  error_str[80];

   call_ptr = (struct calls *) call_hndl;
   if (call_ptr == NULL)
   {
       ERROR(' ',"","attempt to deactivate a null call");
       return(FAIL);
   }
   if (call_ptr->call_number == 0)
   {
       ERROR(' ',"","attempt to deactivate a call with no nbr"); 
       return(FAIL);
   }
   if ((call_ptr < offset->call))
   {
        sprintf(error_str,"attempt to deactivate something that is not a call at %x", call_ptr);
        ERROR(' ',"",error_str);
        return(FAIL);
   }

   if (call_ptr->pickup_zone)
      Zone_remove_call(call_ptr->pickup_zone, (CALL_HNDL) call_ptr);

   Call_delete_timer((CALL_HNDL) call_ptr, CALL_ALL_TIMERS);	/* May not be any */

   if ( (short)Call_get_value( call_hndl, CALL_TYPE_ILINK ) == FALSE )
     {
       if (call_ptr->except_ptr != NULL)	/* if exceptions exist leave alone */
	 return (SUCCESS);
     }
   else
     {
       // Always remove all exceptions in TaxiPak for a trip that has been
       // successfully transferred to Interlink
       if ( call_ptr->except_ptr != NULL )
	 remove_except(call_ptr->except_ptr);
     }
   
   if ( (CALL_MSG_LIST_HNDL)Call_get_value( call_hndl, CALL_MSG_LIST ) != HNDL_NULL )
     Call_msg_list_clear( (CALL_MSG_LIST_HNDL)Call_get_value( call_hndl, CALL_MSG_LIST ) );

   // Cleanup all linked lists for the call object
   if (call_ptr->call_reject_veh_list != HNDL_ERR)
     Call_reject_veh_list_destroy(call_ptr->call_reject_veh_list);
   if (call_ptr->timer_list != HNDL_ERR)
     LL_Destroy((LinkedList *)call_ptr->timer_list);
   if (call_ptr->call_except_list_hndl != HNDL_ERR)
     LL_Destroy(call_ptr->call_except_list_hndl);
   if (call_ptr->call_mult_list_hndl != HNDL_ERR)
     LL_Destroy(call_ptr->call_mult_list_hndl);
   

   if (Fleet_get_hndl(BY_NBR, (short)Call_get_value(call_hndl, CALL_FLEET_NBR), &fleet_hndl) != FAIL)
     {
       Fleet_big_car_trip_list_remove(
                                      (FLEET_BIG_CAR_TRIP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_BIG_CAR_TRIP_LIST),
                                      call_hndl
                                      );
     }
         
   if ((Calls_list_remove((CALL_HNDL) call_ptr)) == FAIL)
   {
      sprintf(error_str, "can't remove call CALL %d from calls_list",
	      call_ptr->call_number);
      ERROR(' ', " ", error_str);
   }
   bzero( call_ptr, sizeof( struct calls ) );
   call_ptr->call_number = 0;                                              /* clear out call number */
   call_ptr->status.available = 0;
   call_ptr->timer_list = NULL;
   call_ptr->next = offset->free_call;                                     /* add the call node to the free list */
   free_calls_nbr++;
   offset->free_call  = call_ptr;

   return (SUCCESS);
}				/* end Call_deactivate */

/**********************************************************/
/* RET_STATUS Call_create()                               */
/* OP: creates a call from UI                             */
/* PRE: Calls_free_list exiists                           */
/*      Calls_list exists                                 */
/* POST:  call send to db                                 */
/*        CALL_NBR assigned                               */
/*       if call active Call_activate called              */
/* ERROR:  Call_activate fails                            */
/*         db access fails                                */
/**********************************************************/

RET_STATUS
Call_create(call_rec_hndl)	/* covers UI side  */
   CALL_REC_HNDL call_rec_hndl;
{
/* part executed on UI side directly                      */
   printf("Call_create not implemented\n");
   ERROR(' ', "", "Call_create not implemented");
   return (FAIL);
}				/* end Call_create */


RET_STATUS
Call_load(call_buf, cl_rec_hndl, call_nbr)
   CALL_IPC_REC *call_buf;
   CALL_REC_HNDL cl_rec_hndl;
   int   call_nbr;
{
   CALL_REC *cl_ptr;		/* pointer to a cisam call record */
   char  error_str[80];

   cl_ptr = (CALL_REC *) cl_rec_hndl;
   if (call_nbr != 0)		/* already there */
   {
      memset(cl_ptr, '\0', sizeof(CALL_REC));
      cl_ptr->nbr = call_nbr;
      if (db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL, 0) < 0)	/* read CALL record */
      {
	 sprintf(error_str, "Call_load - error reading call_nbr %d\n", cl_ptr->nbr);
	 ERROR(' ', CALL_FILE, error_str);
	 return (FAIL);
      }
   }
   else
   {
      ERROR(' ',"","No call nbr");
      return(FAIL);
   }
   memset(call_buf, '\0', sizeof(CALL_IPC_REC));
   return (Call_ipc_rec_load(call_buf, cl_rec_hndl));
}				/* end Call_load */


/*************************************************************/
/* RET_STATUS Call_ipc_rec_load(call_buf, cl_ptr)            */
/* OP: load a call from the data base and update             */
/* PRE: call_buf exists                                      */
/*      call db record inmemory and addresed by cl_ptr       */
/* POST: call_buf contains call db record                    */
/*       call_buf updated by contents of cl_ptr              */
/* ERROR: db access errro                                    */
/*************************************************************/

RET_STATUS
Call_ipc_rec_load(call_buf, cl_rec_hndl)	/* WAS fill_call_ipc_rec */
   CALL_IPC_REC *call_buf;
   CALL_REC_HNDL cl_rec_hndl;
{
   CALL_REC *cl_ptr;		/* pointer to a cisam call record */
   unsigned char mask;		/* holds the bit mask value */
   unsigned char *addr;		/* used as a pointer to a bit structure */
   register i,j;

   cl_ptr = (CALL_REC *) cl_rec_hndl;
#ifdef DEBUG
   if (offset->prattle >= LEVEL7)
   {
      sprintf(trace_str, "Call_ipc_rec_load  : call_buf = %x cl_ptr = %x \n", call_buf, cl_ptr);
      TRACE(trace_str);
   }
#endif
   call_buf->u_prcs_id = DISP_DEST;	/* sent by dispatch */
   call_buf->rec_type = CALL_REQ;

   memset(&call_buf->status, '\0', sizeof(struct except_bits));
   call_buf->c_isam_nbr = isrecnum;	/* populate with record number */
   call_buf->call_nbr = cl_ptr->nbr;
   call_buf->grouped_with = cl_ptr->grouped_with;
   call_buf->fleet = cl_ptr->fleet;
   memset(&call_buf->call_type, '\0', sizeof(call_buf->call_type));
   call_fill_bits(&call_buf->call_type, cl_ptr->extended_type, sizeof cl_ptr->extended_type);
   call_buf->pickup_zone = cl_ptr->pckup_zone;
   call_buf->dest_zone = cl_ptr->dest_zone;
   call_buf->personal_veh = cl_ptr->personal_veh;
   call_buf->personal_rqst = cl_ptr->personal_rqst;
   call_buf->priority = cl_ptr->priority;
   call_buf->due_time = cl_ptr->due_date_time;


   strcpy(call_buf->pick_pmap, cl_ptr->pckup_pmap);

   memset(&call_buf->drv_attr, '\0', sizeof(struct drv_attributes));
   memset(&call_buf->veh_attr, '\0', sizeof(struct veh_attributes));

   addr = (unsigned char *) &call_buf->veh_attr;        /* must use unsigned
                                                         * variables */
   for ( i = 0; i < 4; i++ )
     {
       // IBM
       //        mask = 0x01;
       mask = 0x80;
       for ( j = 0; j < 8; j++ )
	 {
	   if (cl_ptr->veh_attr[i*8 + j] == '\0')
	     break;
	   if (cl_ptr->veh_attr[i*8 + j] == YES)
	     *addr |= mask;
	   //            mask<<=1;
	   mask>>=1;
	 }
       ++addr;
     }
   addr = (unsigned char *) &call_buf->drv_attr;

   for ( i = 0; i < 4; i++ )
     {
       // IBM
       //        mask = 0x01;
       mask = 0x80;
       for ( j = 0; j < 8; j++ )
	 {
	   if (cl_ptr->drv_attr[i*8 + j] == '\0')
	     break;
	   if (cl_ptr->drv_attr[i*8 + j] == YES)
	     *addr |= mask;
	   //            mask<<=1;
	   mask>>=1;
	 }
       ++addr;
     }


   return (SUCCESS);
}				/* end Call_ipc_rec_load */

/*********************************************************/
/* CALL_HNDL Call_activate(call_buf, call_exists)        */
/* OP: make call active                                  */
/* PRE: Call_list exists                                 */
/*      call data exists n call_buf                      */
/* POST:                                                 */
/*       if call alrady in Calls_list                    */
/*           call_exists set to TRUE                     */
/*       else                                            */
/*           call storage allocated form free_calls_list */
/*           call added to Calls_list                    */
/*       call initialized from call_buf                  */
/*       Call_update_counts (INCREMENT_BOTH) called      */
/* ERROR: zone does not exist                            */
/*        Fleet does not exist                           */
/*********************************************************/

CALL_HNDL
Call_activate(call_buf, call_exists)	/* WAS add_call */
   CALL_IPC_REC *call_buf;
   short *call_exists;
{
   struct calls *new_call;	/* pointer to a call record */
   int   fl_nbr, i;
   ZONE_HNDL zone_hndl = HNDL_NULL;
   FLEET_HNDL fleet_hndl;
   char  error_str[80];
   RET_STATUS rc;
   struct cisam_cl  db_call;
   TC_HNDL          tc_hndl, found_tc_hndl = HNDL_NULL;
   TC_STRUCT        *pTimeCall;
   ZONE_TC_LIST_HNDL    tc_list_hndl;   

   *call_exists = 0;		/* don't know if call exists yet */

   if (Fleet_get_hndl(BY_ID, call_buf->fleet, &fleet_hndl) == FAIL)
   {				/* validate the fleet */
      sprintf(error_str, "add_call - fleet %c does not exist", call_buf->fleet);
      ERROR(call_buf->fleet, " ", error_str);
      return (HNDL_NULL);
   }
   fl_nbr = (short) Fleet_get_value(fleet_hndl, FLEET_NBR);

   if (call_buf->pickup_zone < 0 || call_buf->pickup_zone >= ZONE_MAX)	/* validate the pickup
									 * zone */
      call_buf->pickup_zone = 0;

   if (call_buf->pickup_zone != 0)
   {
      Zone_get_hndl(call_buf->pickup_zone, fl_nbr, &zone_hndl);
      if ((zone_hndl == HNDL_ERR))
      {
	 sprintf(error_str, "Call_activate - zone %d does not exist", call_buf->pickup_zone);
	 ERROR(call_buf->fleet, " ", error_str);
	 return (HNDL_NULL);
      }
   }

   
   if ((new_call = (struct calls *) Calls_list_find(call_buf->call_nbr)) != NULL)
     ++(*call_exists);
   else
   {
      if (offset->free_call->next == NULL)	/* BIG TROUBLE - NO NODES */
      {
	 sprintf(error_str, "no free node to add call %d %d left", call_buf->call_nbr, free_calls_nbr);
	 ERROR(call_buf->fleet, " ", error_str);
         if ((rc = re_build_calls()) == FAIL)
         {
            if (!call_node_err)
            {
	       mk_except((struct calls *) NULL, (struct veh_driv *) NULL,
	         	   SYS_ERR, CALL_NODE_ERR, -1);
               call_node_err = 1;
            }
	    return (HNDL_NULL);
         }

      }
      new_call = offset->free_call;
      offset->free_call = offset->free_call->next;
      free_calls_nbr--;
      if (offset->free_call->next == NULL)
      {
         sprintf(error_str, "allocating last call at %x %d left ", new_call, free_calls_nbr);
         ERROR(call_buf->fleet, " ", error_str);
      }
      memset((char *) new_call, '\0', sizeof(struct calls));
      new_call->call_number = call_buf->call_nbr;
      new_call->time_entered = mads_time;
      new_call->accept_time = 0;
      Calls_list_add((CALL_HNDL) new_call);
      
      /* remove from TC list if present */
      tc_list_hndl = (ZONE_TC_LIST_HNDL) Zone_get_value( zone_hndl, ZONE_TC_LIST );
      if ( tc_list_hndl != NULL )
	{
	  Zone_tc_list_each_get( tc_list_hndl, tc_hndl )
	    {
	      pTimeCall = ( TC_STRUCT * )tc_hndl;
	      if ( pTimeCall->nbr == (int) call_buf->call_nbr )
		{
		  found_tc_hndl = tc_hndl;
		  break;
		}
	    } Zone_tc_list_each_end(tc_list_hndl)
	      
	      if ( found_tc_hndl != HNDL_NULL )
		{
		  Zone_tc_list_remove( tc_list_hndl, found_tc_hndl );
		  --num_tc;
		}
	}      
    }

   /* populate the new call with call information */
   if (call_buf->priority < 0 || call_buf->priority > EX_PRI_MAX)
      new_call->priority = EX_PRI_MAX;
   else
      new_call->priority = call_buf->priority;

   if (!*call_exists)
   {				/* certain fields cannot be updated by users */
      new_call->curr_level = 0;
      new_call->fleet = fl_nbr;
      new_call->msg_nbr = call_buf->msg_nbr;
      new_call->pickup_zone = call_buf->pickup_zone;
      memcpy(&new_call->status, &call_buf->status, sizeof(struct except_bits));	/* moved from below ess
										 * 9/14 */
      /* WHAT ABOUT STATUS BITS ?? */
   }
   else
     {
       if ( (new_call->pickup_zone != call_buf->pickup_zone) &&
	   ( Call_get_state( (CALL_HNDL)new_call, CALL_AVAILABLE_STATE ) ) )
	 Call_change_zone((CALL_HNDL) new_call, call_buf->pickup_zone);
     }

   strcpy(new_call->pickup_pmap, call_buf->pick_pmap);
   new_call->grouped_with = call_buf->grouped_with;
   if ( new_call->grouped_with == 1 )
     new_call->grouped_with = 0;
   new_call->dest_zone = call_buf->dest_zone;
   new_call->personal_vehicle = call_buf->personal_veh;
   new_call->personal_request = call_buf->personal_rqst;
   memcpy(&new_call->call_type, &call_buf->call_type, sizeof(struct call_types));
   new_call->due_time = call_buf->due_time;	/* need to add code to update
						 * call queue or */
   /* take call out of memory all together !!!!! */
   new_call->c_isam_num = call_buf->c_isam_nbr;

   /* reread call record to get attribute settings - this is temporary to fix
      problem with multi-trips being dispatched with improper attributes
   */
   bzero( &db_call, sizeof( struct cisam_cl ) );
   db_call.nbr = call_buf->call_nbr;
   if ( db_read_key( CALL_FILE_ID, &db_call, &cl_key1, DB_EQUAL ) != SUCCESS )
     {
       sprintf( error_str, "Call_activate - error reading DB (%d)", db_call.nbr );
     }
   bzero( &new_call->vehicle_attributes, sizeof(new_call->vehicle_attributes) );
   bzero( &new_call->driver_attributes, sizeof(new_call->driver_attributes) );
   if ( db_call.veh_attr_flag == YES )
     {
       for ( i = 0; i < ATTR_MAX; i++ )
	 {
	   if ( db_call.veh_attr[i] == YES )
	     TC_set_bit( i, 1, &new_call->vehicle_attributes );
	 }
     }
   if ( db_call.drv_attr_flag == YES )
     {
       for ( i = 0; i < ATTR_MAX; i++ )
	 {
	   if ( db_call.drv_attr[i] == YES )
	     TC_set_bit( i, 1, &new_call->driver_attributes );
	 }
     }

#ifdef FOO
   else
     {
       memcpy(&new_call->vehicle_attributes, &call_buf->veh_attr, sizeof(struct veh_attributes));
       memcpy(&new_call->driver_attributes, &call_buf->drv_attr, sizeof(struct drv_attributes));
     }
#endif

   /* Make sure we're on the map */
   new_call->call_msg_nbr = call_buf->call_msg_nbr;
   if (0)
     {
       if ( call_buf->gps_lat > GPS_YMAX || call_buf->gps_lat < GPS_YMIN ||
	   call_buf->gps_long > GPS_XMAX || call_buf->gps_long < GPS_XMIN )
	 {
	   call_buf->gps_lat = 0.0;
	   call_buf->gps_long = 0.0;
	 }
     }
   else
     {
       if (isnan(call_buf->gps_lat)||isnan(call_buf->gps_long))
	 {
	   call_buf->gps_lat = 0.0;
	   call_buf->gps_long = 0.0;
	 }
	 
       if ( call_buf->gps_long > fleet[fl_nbr]->gps_max_x || call_buf->gps_long < fleet[fl_nbr]->gps_min_x ||
	    call_buf->gps_lat > fleet[fl_nbr]->gps_max_y || call_buf->gps_lat < fleet[fl_nbr]->gps_min_y )
	 {
	   call_buf->gps_lat = 0.0;
	   call_buf->gps_long = 0.0;
	 }
     }
   
   new_call->gps_lat = call_buf->gps_lat;
   new_call->gps_long = call_buf->gps_long;
   new_call->FTJ_nbr = call_buf->FTJ_nbr;

   if (!*call_exists)
     {
       /* temporarily use old_due_time field to hold tc_activate_time */
       new_call->tc_activate_time = call_buf->old_due_time;
       Call_update_counts((CALL_HNDL) new_call, INCREMENT_BOTH);
     }

#ifdef DEBUG
   if (offset->prattle >= LEVEL2)
   {
      sprintf(error_str, "Call_activate: call number = %i call_ptr = %x \n",
	      new_call->call_number, new_call);
      TRACE(trace_str);
   }
#endif

   return ((CALL_HNDL) new_call);
}				/* end Call_activate */

/***********************************************/
/* RET_STATUS Call_call_out(call_hndl)         */
/* OP: process a call out request              */
/* PRE: call exists                            */
/* POST: call history updated                  */
/*       CALL_CALL_OUT_REQUEST made            */
/* ERROR: tbd                                  */
/***********************************************/

RET_STATUS
Call_call_out(call_hndl)
   CALL_HNDL call_hndl;
{
   struct calls *call_ptr;

   call_ptr = (struct calls *) call_hndl;

   if (call_hndl == HNDL_NULL)
   {
      ERROR(' ', "", "Null call hndl - CALLOUT");
      return (FAIL);
   }


   mk_except(call_ptr, call_ptr->veh_ptr, CALLOUT, 0, -1);
   Call_add_history((CALL_HNDL) call_ptr, (VEH_HNDL) call_ptr->veh_ptr, CALLOUT, 0, 0L, 0);
   return (SUCCESS);
}				/* end Call_call_out */

#ifndef EXT_CALL_ETA
/*******************************************/
/* RET_STATUS Call_eta(call_hndl, eta)     */
/* OP: process an eta from the vehicle     */
/* PRE: call exists                        */
/*      vehcile exists                     */
/* POST: cal history updated               */
/* ERROR: veh_hndl != CALL_VEH_HNDL        */
/*******************************************/

RET_STATUS
Call_eta(call_hndl, veh_hndl, eta)
   CALL_HNDL call_hndl;
   VEH_HNDL veh_hndl;
   int   eta;
{
   CALL *call_ptr;

   call_ptr = (CALL *) call_hndl;

   Call_add_history(call_hndl, veh_hndl, ETA, (short) eta, 0L, 0);
   return (SUCCESS);
}				/* end Call_eta */
#endif

/**********************************************************/
/* RET_STATUS Call_get_status(call_hndl, call_struct_ptr) */
/* OP: fill in the call structure with call status        */
/* PRE: call exists                                       */
/*      call_struct_ptr != NULL                           */
/* POST: call_struct filled                               */
/* ERROR: none                                            */
/**********************************************************/

RET_STATUS
Call_get_status(call_hndl, call_struct_ptr)
   CALL_HNDL call_hndl;
   struct calls *call_struct_ptr;
{
   printf("Call_get_status not implemented\n");
   ERROR(' ', "", "Call_get_status not implemented");
   return (FAIL);
}				/* end Call_get_status */


/**************************************************************/
/* RET_STATUS Call_redispatch(call_hndl)                      */
/*   OP: call needs to be redispatched                        */
/*   PRE: call assigned to a vehicle                          */
/*   POST: Call timers deleted                                */
/*         if CALL_ACCEPTED_STATE && !CALL_PICKUP_STATE       */
/*            Veh added to Call_reject_veh_list               */
/*            CALL_NBR_REJECTS incremented                    */
/*            CALL_REDISPATCH msg sent to vehicle             */
/*            VEH_ON_CALL_STATE (VEH_STATCKE_CALLS) reset     */
/*          Call_load called                                  */
/*          CALL_VEH_HNDL = HNDL_NULL                         */
/*          CALL_CURRENT_LEVEL = 0                            */
/*          Call_status initialized off                       */
/*          if ONLY_PER_REQUEST && no SUP_HOLD request answer */
/*              CALL_HOLD_STATE set                           */
/*          if IF_PERS_REQUEST                                */
/*             Zone_add_call called                           */
/*          Call_initialize called                            */
/*   ERROR: cannot add call to zone                           */
/*          Call_initialize fails                             */
/**************************************************************/

RET_STATUS
Call_redispatch(call_hndl, redispatch_type)
   CALL_HNDL call_hndl;
     int     redispatch_type;
{
   struct calls *call_ptr;
   struct veh_driv *veh_ptr;
   FLEET_HNDL   fleet_hndl;
   CALL_REJECT_VEH_LIST_HNDL tmp_list = HNDL_NULL;
   CALL_REC *cl_ptr;
   CALL_IPC_REC call_buf;
   short no_except = FALSE;
   int      FTJ_nbr;
   CALL_MSG_HNDL cmsg_hndl;

   call_ptr = (struct calls *) call_hndl;
   veh_ptr = call_ptr->veh_ptr;

   FTJ_nbr = call_ptr->FTJ_nbr;


   if (veh_ptr != NULL)
     {
       if ((tmp_list = Call_get_value(call_hndl, CALL_REJECT_VEH_LIST)) == HNDL_ERR)
	 {
	   tmp_list = Call_reject_veh_list_create();
	   Call_set_value(call_hndl, CALL_REJECT_VEH_LIST, (HNDL) tmp_list);
	 }
       Call_reject_veh_list_add(tmp_list, (VEH_HNDL) veh_ptr);
       call_ptr->call_nbr_reject++;
     }
   Call_delete_timer((CALL_HNDL) call_ptr, CALL_ALL_TIMERS);	/* May not be any */
   if (veh_ptr != NULL)
     Veh_redispatch_call((VEH_HNDL) veh_ptr, (CALL_HNDL) call_ptr, redispatch_type);
   
       
   cl_ptr = (CALL_REC *) req_buf;

   writer_unassgn( call_ptr->call_number );
   if (Call_load(&call_buf, (CALL_REC_HNDL) cl_ptr, call_ptr->call_number) == FAIL)
     return (FAIL);



   /* following skipped by Call_initialize */
   call_ptr->veh_ptr = NULL;
   call_ptr->curr_level = 0;
   call_ptr->msg_nbr = call_buf.msg_nbr;
   call_ptr->pickup_zone = call_buf.pickup_zone;
   memcpy(&call_ptr->status, &call_buf.status, sizeof(struct except_bits));

   /* Redispatching a Subscription Call that should be designated as activated */
   if ( call_ptr->call_type.time && call_ptr->call_type.subscription )
     call_buf.old_due_time = mads_time;

   call_buf.FTJ_nbr = FTJ_nbr;

   if (0)
     {
       if ( (CALL_MSG_LIST_HNDL)Call_get_value( (CALL_HNDL)call_ptr, CALL_MSG_LIST ) == HNDL_NULL )
	 {
	   Call_set_value( (CALL_HNDL)call_ptr, CALL_MSG_LIST, Call_msg_list_create() );
	   Call_msg_list_load( (CALL_MSG_LIST_HNDL) Call_get_value( (CALL_HNDL)call_ptr, CALL_MSG_LIST ),
			       (int) Call_get_value( (CALL_HNDL)call_ptr, CALL_NBR ) );
	 }
	 
       Call_msg_list_each_get( (CALL_MSG_LIST_HNDL)Call_get_value( (CALL_HNDL)call_ptr, CALL_MSG_LIST ), cmsg_hndl )
	 {
	   Call_msg_set_value( cmsg_hndl, CALL_MSG_ACK, (HNDL)NO );
	   Call_msg_update_nak( cmsg_hndl );
	 } Call_msg_list_each_end((CALL_MSG_LIST_HNDL)Call_get_value( (CALL_HNDL)call_ptr, CALL_MSG_LIST ))
     }
   
   /* following does Call_match */
   if ( redispatch_type == NORMAL_REDISPATCH )
     Call_initialize(&call_buf, FALSE);
   else if ( redispatch_type == ILINK_REDISPATCH )
     Call_initialize(&call_buf, TRUE);

   return (SUCCESS);
}				/* end Call_redispatch */

/***********************************************************/
/* int Call_is_zoned(call_hndl)                            */
/* OP: return TRUE if call in its pickup zone  else FALSE  */
/* PRE: call_hndl != NULL                                  */
/* POST: none                                              */
/* ERROR: none                                             */
/***********************************************************/

int
Call_is_zoned(call_hndl)	/* WAS in in_zone */
   CALL_HNDL call_hndl;
{
   int   zone_nbr;
   ZONE_HNDL zone_hndl;
   ZONE_CALLS_LIST_HNDL zone_calls_list;

   zone_nbr = (short) Call_get_value(call_hndl, CALL_PICKUP_ZONE);
   if (zone_nbr == 0)
      return (FALSE);
   if (Zone_get_hndl(zone_nbr, (short) Call_get_value(call_hndl, CALL_FLEET_NBR), &zone_hndl) == FAIL)
      return (FALSE);
   zone_calls_list = (ZONE_CALLS_LIST_HNDL) Zone_get_value(zone_hndl, ZONE_AVAIL_CALLS_LIST);
   if (Zone_calls_list_find(zone_calls_list, call_hndl) == HNDL_ERR)
      return (FALSE);
   return (TRUE);

}				/* end Call_is_zoned */

int
check_except(call_ptr)
   struct calls *call_ptr;
{
   unsigned char *excpt_bit_ptr;
   unsigned char mask;
   register i, j;

#ifdef DEBUG
   if (offset->prattle >= LEVEL6)
   {
      sprintf(trace_str, "check_except : call_ptr %x \n", call_ptr);
      TRACE(trace_str);
   }
#endif
   
   excpt_bit_ptr = (unsigned char *) &call_ptr->status;
   for (i = 0; i < EXCEPT_BYTES_MAX; i++)
   {
      mask = 0x80;
      for (j = 1; j <= 8; j++)
      {
	 if (*excpt_bit_ptr & mask)
	    return (j + (i * 8));
	 mask >>= 1;
      }
      ++excpt_bit_ptr;
   }

   return (0);
}				/* end check_except */

static RET_STATUS
get_call_count_indicies(call_ptr, row, column)
   struct calls *call_ptr;
   short *row;
   short *column;
{
   if (call_ptr->call_type.charge)
      *column = 0;
   else if (call_ptr->call_type.time)
      *column = 2;
   else if (call_ptr->call_type.subscription)
      *column = 4;
   else
      return (FAIL);

   if (call_ptr->call_type.passenger)
   {
      *row = 0;
      return (SUCCESS);
   }
   if (call_ptr->call_type.delivery)
   {
      *row = 1;
      return (SUCCESS);
   }
   if (call_ptr->call_type.xtype)
   {
      *row = 2;
      return (SUCCESS);
   }
   return (FAIL);
}				/* end get_call_count_indicies */

/********************************************************/
/* RET_STATUS Call_update_counts(call_hndl, op)         */
/* OP: update the system fleet call count statistics    */
/* PRE: call exists                                     */
/*      fleet_exists                                    */
/* POST: fleet today and/or now call counts altered     */
/*          depending on op as follows                  */
/*       INCREMENT_BOTH                                 */
/*       INCREMENT_NOW                                  */
/*       DECREMENT_BOTH                                 */
/*       DECREMENT_TODAY                                */
/*       DECREMENT_NOW                                  */
/* ERROR: none                                          */
/********************************************************/

RET_STATUS
Call_update_counts(		/* WASS up_call_counts */
		   CALL_HNDL call_hndl,
		   short op)
{
   struct calls *call_ptr;
   short column;
   short row;
   short type_count = TRUE;
   FLEET *fleet_ptr;

   call_ptr = (struct calls *) call_hndl;
   fleet_ptr = fleet[call_ptr->fleet];

   if (get_call_count_indicies(call_ptr, &row, &column) == FAIL)
      type_count = FALSE;

   switch (op)
   {
   case INCREMENT_BOTH:
      if (!call_ptr->call_type.wakeup)
      {
	 ++fleet_ptr->calls;
	 if (type_count)
	    ++fleet_ptr->call_stats[row][column + 1];
      } else
	 ++fleet_ptr->wakeups;
   case INCREMENT_NOW:
      if (!call_ptr->call_type.wakeup)
      {
	 ++fleet_ptr->now_calls;
	 if (type_count)
	    ++fleet_ptr->call_stats[row][column];
      } else
	 ++fleet_ptr->now_wakeups;
      break;
   case DECREMENT_BOTH:
   case DECREMENT_TODAY:
      if (!call_ptr->call_type.wakeup)
      {
	 --fleet_ptr->calls;
	 if (type_count)
	    --fleet_ptr->call_stats[row][column + 1];
      } else
	 --fleet_ptr->wakeups;
      if (op != DECREMENT_BOTH)
	 break;
   case DECREMENT_NOW:
      if (!call_ptr->call_type.wakeup)
      {
	 --fleet_ptr->now_calls;
	 if (type_count)
	    --fleet_ptr->call_stats[row][column];
      } else
	 --fleet_ptr->now_wakeups;
      break;
   default:
      ERROR(' ', "", "Illegal Call_count_operation");
      return (FAIL);
   }				/* end case */
   return (SUCCESS);
}				/* end Call_update_counts */

/*****************************************************************/
/* RET_STATUS Call_send_offer(call_hndl, veh_hndl)               */
/* OP: send a call offer to vehicle                              */
/* PRE: vehicle exists                                           */
/*      call exists                                              */
/* POST:  call offer sent to vehicle                             */
/*        offer format depends on value of FLEET_CALL_OFFER_INFO */
/* ERROR: none                                                   */
/*****************************************************************/

RET_STATUS
Call_send_offer(call_hndl, veh_hndl)
   CALL_HNDL call_hndl;
   VEH_HNDL veh_hndl;
{
   struct calls *call_ptr;
   struct veh_driv *veh_ptr;
   char error_str[80];

   veh_ptr = (struct veh_driv *) veh_hndl;
   call_ptr = (struct calls *) call_hndl;

   return (send_offer(veh_ptr->mid, call_ptr, veh_ptr));
}				/* end Call_send_offer */

RET_STATUS
Call_cbook_send_offer(call_hndl, veh_hndl)
   CALL_HNDL call_hndl;
   VEH_HNDL veh_hndl;
{
   struct calls *call_ptr;
   struct veh_driv *veh_ptr;
   char error_str[80];

   veh_ptr = (struct veh_driv *) veh_hndl;
   call_ptr = (struct calls *) call_hndl;

   return (send_cbook_offer(veh_ptr->mid, call_ptr, veh_ptr));
}				/* end Call_send_offer */

/*************************************************************/
/* CALL_REC * Call_get_record(cisam_number, call_nbr) */
/* OP: return the db call record for indicated call          */
/* PRE: none                                                 */
/* POST: call db in memory and address returned              */
/* ERROR: db access                                          */
/*************************************************************/

CALL_REC_HNDL
Call_get_record(cisam_number, call_nbr)
   int   cisam_number, call_nbr;
{
   CALL_REC *call_input;
   char  error_str[80];
   static CALL_REC cisam_call;

   call_input = (CALL_REC *) req_buf;

#ifdef DEBUG
   if (offset->prattle >= LEVEL5)
   {
      sprintf(trace_str,
	      "ENTERING get_call_record (cisam_number=%d, call_nbr=%d)\n",
	      cisam_number, call_nbr);
      TRACE(trace_str);
   }
#endif

   call_input->nbr = call_nbr;
   if (db_read_key(CALL_FILE_ID, call_input, &cl_key1, DB_EQUAL) != SUCCESS)
/* if (db(CALL_FILE_ID, READ_NUM, &cl_key1, ISEQUAL, cisam_number) != 0) */
   {
      call_input->nbr = call_nbr;
      if (db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL, 0) != 0)
      {
	 sprintf(error_str, "db error %d get_call_record1: call %d", iserrno, call_nbr);
	 ERROR(' ', CALL_FILE, error_str);
	 return (NULL);
      }
   } else if (call_nbr != call_input->nbr)
   {				/* isam exists but not right record */
      call_input->nbr = call_nbr;
      if (db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL, 0) != 0)
      {
	 sprintf(error_str, "db error %d get_call_record2: call %d", iserrno, call_nbr);
	 ERROR(' ', CALL_FILE, error_str);
	 return (NULL);
      }
   }
   /* copy the buffer from disk */
   memcpy(&cisam_call, req_buf, sizeof(CALL_REC));

   return ((CALL_REC_HNDL) & cisam_call);
}				/* end Call_get_record */

/********************************************************/
/* RET_STATUS Call_send_cancel(call_hndl, veh_hndl)     */
/* OP: send a cancel message to the vehicle             */
/* PRE: call exists                                     */
/*      vehicel exists                                  */
/* POST: message sent to vehicle                        */
/* ERROR: unable to get call database record            */
/********************************************************/

RET_STATUS
Call_send_cancel(call_hndl, veh_hndl)
   CALL_HNDL call_hndl;
   VEH_HNDL veh_hndl;
{
   CALL_REC *cl_ptr;
   char  str_temp[80];
   struct calls *call_ptr;
   struct cisam_ml *ml_ptr;

   if (veh_hndl == HNDL_NULL)
      return (FAIL);
   if (call_hndl == HNDL_NULL)
     return (FAIL);

   call_ptr = (struct calls *) call_hndl;
   ml_ptr = (struct cisam_ml *)req_buf;
   if (call_ptr == NULL)
     return (FAIL);
   
   if ((cl_ptr = (CALL_REC *) Call_get_record(call_ptr->c_isam_num, call_ptr->call_number)) != NULL)
   {
     if ((0) && (cl_ptr->ext_nbr > 0))
       /* If FTJ-call replace call number with FTJ_nbr */
       sprintf(str_temp,"%s %6d %s ",CALL_,cl_ptr->ext_nbr, CANCELED_);
     else
       sprintf(str_temp,"%s %6d %s ",CALL_,cl_ptr->nbr, CANCELED_);
			         
      mk_outb_text(str_temp);
      add_outb_text(CR);

      (void) mk_pickup_address(fleet[call_ptr->fleet]->call_assignment_info,
			       cl_ptr,
			       COMPLETE_ADDRESS, 0);	/* 2 lines */
      
      ml_ptr->from = -1;   /** system generated message **/
      ml_ptr->fleet = 'A';
      sprintf(ml_ptr->to_selection, "%5d",call_ptr->veh_ptr->veh_nbr);
      ml_ptr->to_selection[5] = '\0';
      ml_ptr->to_type = MSG_VEHICLE;
      ml_ptr->date_time_sent = time(0);
      ml_ptr->supervisor_id = 0;
      ml_ptr->repeat = NO;
      get_asc_time(ml_ptr->date_time_sent, ml_ptr->date_sent, ml_ptr->time_sent);
      strcpy(ml_ptr->msg_line1, outb.text);
      ml_ptr->msg_line1[100] = '\0';
      ml_ptr->msg_line2[100] = '\0';
      if (db(MSGLOG_FILE_ID, ADD_REC, &ml_key1, 0, 0) < 0)
	{
	  sprintf(error_str, "Add Error: iserrno = %d nbr = %d from = %d",
		  iserrno, ml_ptr->nbr, ml_ptr->from);
	  ERROR(' ', MSGLOG_FILE, error_str);
	}
      
      send_msg_mmp(call_ptr->veh_ptr->mid, CANCEL_DISPLAY, call_ptr->veh_ptr);
      clr_pkt_id();

      if ( (0)  ||
           ( (short)Veh_get_value((VEH_HNDL)call_ptr->veh_ptr, VEH_TERM_TYPE) == MDT_OPUS_2K ) )
	send_text_predefined(call_ptr->veh_ptr->mid, TRIP_CANX, call_ptr->veh_ptr);
     
      return (SUCCESS);
   }
   return (FAIL);
}				/* end Call_send_cancel_msg */

/*********************************************************/
/* RET_STATUS Call_send_assign(call_hndl, veh_hndl)      */
/* OP: send the call assignment to the vehicle           */
/* PRE: vehicle exists                                   */
/*      call_exists                                      */
/* POST: call assignment sent to vehicle                 */
/*       format determined by FLEET_CALL_ASSIGNMENT_INFO */
/*       if multiple calls - all call assignments sent   */
/* ERROR: cannot transmit message                        */
/*********************************************************/

RET_STATUS
Call_send_assign(call_hndl, veh_hndl, update)
   CALL_HNDL call_hndl;
   VEH_HNDL veh_hndl;
   BOOLEAN update;
{
   /* GENERALLY TEMPORARY */
   struct calls *call_ptr;
   struct veh_driv *veh_ptr;

   call_ptr = (struct calls *) call_hndl_to_ptr(call_hndl);
   veh_ptr = (struct veh_driv *) veh_hndl_to_ptr(veh_hndl);
   return (send_assign((char *) Veh_get_value(veh_hndl, VEH_MID), call_ptr, veh_ptr, update));
}				/* end Call_send_assign */

/****************************************************/
/* RET_STATUS Call_get_hndl(call_nbr, hndl_ptr)     */
/* OP: get the call hndl                            */
/* PRE: Call_list exists                            */
/* POST: call_hndl returned in second argument      */
/* ERROR: no call_hndl                              */
/****************************************************/


RET_STATUS
Call_get_hndl(call_nbr, hndl_ptr)
   int   call_nbr;
   CALL_HNDL *hndl_ptr;
{
   CALL_HNDL rc;
   *hndl_ptr = Calls_list_find(call_nbr);
   if (*hndl_ptr == HNDL_NULL)
      return (FAIL);
   else
      return (SUCCESS);
}				/* end Call_get_hndl */

/* could also serve R_MAN_ZONE with a little work */
/* as is some of functionality no used */
RET_STATUS
Call_change_zone(
   CALL_HNDL call_hndl,
   short zone_nbr)
{
   CALL *call_ptr;
   call_ptr = (CALL *) call_hndl;

   if (call_ptr->pickup_zone == 0)
   {				/* unzoned call ? */
      call_ptr->pickup_zone = zone_nbr;
      if (call_ptr->veh_ptr == NULL && call_ptr->call_number != 0)
	 (void) Zone_add_call(call_ptr->pickup_zone, (CALL_HNDL) call_ptr);
   } else
   {
      if (Call_is_zoned(call_hndl))
      {				/* avail call with existing zone */
	 (void) Zone_remove_call(call_ptr->pickup_zone, (CALL_HNDL) call_ptr);

         call_ptr->pickup_zone = zone_nbr;
	 if (zone_nbr == 0)
	    return (SUCCESS);
	 if (call_ptr->veh_ptr == NULL && call_ptr->call_number != 0)
	    (void) Zone_add_call(call_ptr->pickup_zone, (CALL_HNDL) call_ptr);
      }
      else
         call_ptr->pickup_zone = zone_nbr;
   }
   return (SUCCESS);
}				/* end Call_change_zone */

RET_STATUS
re_build_calls()
{
   register int i;
   struct calls *tmp_call_ptr;
   struct calls *last_call_ptr;
   char error_str[80];

   if (free_calls_nbr <= 1)
       return(FAIL);

   free_calls_nbr = 0;
   tmp_call_ptr = offset->call;
   last_call_ptr = offset->free_call;

   for (i = 0; i < CALLS_MAX - 1; i++)
   {				/* loop for maximum number of calls */
      if (tmp_call_ptr->call_number ==0)
      {
          last_call_ptr->next = tmp_call_ptr;
          last_call_ptr = tmp_call_ptr;
          free_calls_nbr++;
      }
      ++tmp_call_ptr;
   }

   if (free_calls_nbr == 0)
       return(FAIL);
   last_call_ptr->next = NULL;

   sprintf(error_str,"reclaimed %d calls", free_calls_nbr);
   ERROR(' ',"",error_str);
   return (SUCCESS);
}				/* end re_build_calls */

/*******************************************************/
/* CALL_HNDL Call_initialize(call_ipc_rec_ptr)	       */
/* OP: set initial call disposition  depending on type */
/* PRE: call exists                                    */
/* POST: SEE CODE FOR ACTUAL POST CONDITION            */
/*       Call_activate called                          */
/*       sets CALL_TYPE                                */
/*       depending on type may call                    */
/*           Zone_add_call                             */
/*           mk_except                                 */
/*           Call_match                                */
/*           Call_add_history                          */
/*           Call_cancel                               */
/* ERROR: Cannot add call to Zone                      */
/*******************************************************/

CALL_HNDL
Call_initialize(call_ipc_rec_ptr, ilink_redispatch)	/* WAS call_pre_process */
   CALL_IPC_REC *call_ipc_rec_ptr;
int ilink_redispatch;
{
   short exists;
   struct calls *call_ptr;
   short type;
   char  error_str[80];
   int   fleet_nbr;
   BOOLEAN Call_is_time_call();
   ZONE_HNDL hZone = HNDL_NULL;
   FLEET_HNDL fleet_hndl = HNDL_NULL;
   VEH_HNDL   veh_hndl;
   struct cisam_cl *cl_ptr;   
   

   if ((call_ptr = (struct calls *) Call_activate(call_ipc_rec_ptr, &exists)) == NULL)
      return (HNDL_NULL);

   // Make sure that unlinked MULTI trips are not present in a zone
   if ( ( call_ptr->call_type.multiple == 1 ) && ( exists == 1 ) && ( call_ptr->grouped_with <= 0 ) )
     Zone_remove_call( call_ptr->pickup_zone, (CALL_HNDL) call_ptr );
       
   if ( ilink_redispatch == TRUE )
     {
       Call_set_value( (CALL_HNDL)call_ptr, CALL_TYPE_ILINK_REJECT, (HNDL)TRUE );
       /* also need to reset the time_call flag for the trip */
       call_ptr->call_type.time = 0;
     }
   
   (void) Fleet_get_hndl( BY_ID, call_ipc_rec_ptr->fleet, &fleet_hndl );
   (void) Zone_get_hndl( call_ptr->pickup_zone,
			call_ptr->fleet,
			&hZone );

   if ( ( exists ) && (0) &&
        ( ( call_ptr->vehicle_attributes.attr3 ) ||
          ( call_ptr->vehicle_attributes.attr4 ) ||
          ( call_ptr->vehicle_attributes.attr5 ) ) )
     {
       Fleet_big_car_trip_list_remove( (FLEET_BIG_CAR_TRIP_LIST_HNDL)Fleet_get_value( fleet_hndl, FLEET_BIG_CAR_TRIP_LIST ),
                                       (CALL_HNDL) call_ptr );
     }


   if ( ( call_ptr->call_type.multiple == 1 ) && ( call_ptr->grouped_with <= 0 ) )
     call_ptr->status.multiple = 1;

   if ( ( call_ptr->call_type.multiple == 1 ) && ( call_ptr->grouped_with > 0 ) )
       type = 0;
     else
       type = check_except(call_ptr);


   /** how to determine if call has been updated with new zone?? **/
   /** in order to do TC list processing???                      **/

   if ( !warm_start )
     {
       if ( ( (short)Call_get_value( (CALL_HNDL) call_ptr, CALL_TYPE_TIME_CALL ) ||
	      ( (short)Call_get_value( (CALL_HNDL) call_ptr, CALL_TYPE_SUBSCRIPTION ) ) ) &&
            (type != UNZONED) &&
            (time_t)Call_get_value( (CALL_HNDL) call_ptr, CALL_TC_ACTIVATE_TIME ) == 0 )
	 {
	   /* check if the call was previously assigned */
	   if ( ( veh_hndl = (VEH_HNDL)Call_get_value( (CALL_HNDL) call_ptr, CALL_VEH_HNDL ) ) != HNDL_NULL )
	     {
	       if ( ( (CALL_HNDL)Veh_get_value((VEH_HNDL)veh_hndl, VEH_CALL_HNDL) ) == (CALL_HNDL)call_ptr )
		 {
		   Veh_call_cancel( veh_hndl, (CALL_HNDL)call_ptr );
		   writer_time_call(call_ptr->call_number);
		 }
	     }
	   if ( type == HOL_TM_CALL )
	     mk_except( call_ptr, (struct veh_driv *)NULL, HOL_TM_CALL, 0, -1 );

           // add handling for MULTI_TRIPS that are linked as time calls
           if ( ( call_ptr->call_type.multiple == 1 ) &&
                ( call_ipc_rec_ptr->c_isam_nbr == 0 ) )
             {

             }
           else
             {
               Zone_add_tc( hZone, (CALL_HNDL) call_ptr );
               // fleet call counter was incremeneted in Call_activate()
               // we will decrement here so the time call is not counted twice at activation
               Call_update_counts( (CALL_HNDL) call_ptr, DECREMENT_BOTH );
               Call_deactivate( (CALL_HNDL) call_ptr );
               return ((CALL_HNDL) call_ptr);
             }
	 }
     }

   /** Stockholm patch - prevent the generation of MULT exceptions
       on Samplan multi-trips
   **/
   if ( ( type == MULT ) && ( call_ptr != NULL ) )
     {
       if ( (0) && ( call_ptr->FTJ_nbr > 0 ) )
	 type = 0;
     }
   
   if ( ((short)Call_get_value( (CALL_HNDL) call_ptr, CALL_TYPE_TIME_CALL ) ) == TRUE &&
        type == 0 )
     {
       /* call was previously in TC list. Add a LAUNCH history event */
       /* to indicate the transition from TC to UNASSIGNED           */
       Call_add_history((CALL_HNDL) call_ptr, HNDL_NULL, TIME_CALL, 0, 0L, 0 );
     }

   if (call_ptr->call_type.wakeup)
   {
      type = WAKE_UP;
   } else
   {
     if ( ( call_ptr->call_type.multiple == 1 ) && ( call_ptr->grouped_with > 0 ) )
       type = 0;
     else
       type = check_except(call_ptr);     
   }
   bit_flip(type, call_ptr, 0);

   if (!call_ptr->status.accepted)
      call_ptr->accept_time = 0;

   /** Stockholm patch - prevent the generation of MULT exceptions
     on Samplan multi-trips
     **/
   if ( ( type == MULT ) && ( call_ptr != NULL ) )
     {
       if ( (0) && ( call_ptr->FTJ_nbr > 0 ) )
	 type = 0;
     }

   /** Stockholm patch - Need to ensure that all Samplan calls have call messages
       loaded.  Occasionally, the call will enter dispatch with CALL_MSG_NBR set to
       '0' when records exist in the call message table. **/
   if ( ( (char)Call_get_value( (CALL_HNDL)call_ptr, CALL_MSG_NBRS ) > '0' ) ||
          ( call_ptr->FTJ_nbr > 0 ) )
     {
       if ( (CALL_MSG_LIST_HNDL)Call_get_value( (CALL_HNDL)call_ptr, CALL_MSG_LIST ) == HNDL_NULL )       
	 Call_set_value( (CALL_HNDL)call_ptr, CALL_MSG_LIST, Call_msg_list_create() );
       else
	 {
	   Call_msg_list_clear( (CALL_MSG_LIST_HNDL)Call_get_value( (CALL_HNDL)call_ptr, CALL_MSG_LIST ) );
	   Call_set_value( (CALL_HNDL)call_ptr, CALL_MSG_LIST, Call_msg_list_create() );
	 }
       Call_msg_list_load( (CALL_MSG_LIST_HNDL) Call_get_value( (CALL_HNDL)call_ptr, CALL_MSG_LIST ),
			   (int) Call_get_value( (CALL_HNDL)call_ptr, CALL_NBR ) );

       if ( Call_get_state( (CALL_HNDL)call_ptr, CALL_ACCEPT_STATE ) )
	 {
	   if ( (VEH_HNDL) Call_get_value( (CALL_HNDL)call_ptr, CALL_VEH_HNDL ) == HNDL_NULL )
	     {
	       sprintf( error_str, "Call (%d) accepted but no VEH_HNDL", call_ptr->call_number );
	       ERROR(' ', "", error_str );
	     }
	   else
	     Call_msg_send( (CALL_MSG_LIST_HNDL) Call_get_value( (CALL_HNDL)call_ptr, CALL_MSG_LIST ),
			   (VEH_HNDL) Call_get_value( (CALL_HNDL)call_ptr, CALL_VEH_HNDL ) );
	 }
     }

   if (call_ptr->pickup_zone == 0 &&
       call_ipc_rec_ptr->personal_rqst == ONLY_PERS_REQ &&
       call_ipc_rec_ptr->personal_veh > 0)
     {
       type = PERSONAL;
       call_ptr->pickup_zone = 999;
       Zone_add_call(call_ptr->pickup_zone, (CALL_HNDL)call_ptr);
       Call_add_history((CALL_HNDL) call_ptr, HNDL_NULL, PERSONAL, 0, 0L, 0);
       mk_except(call_ptr, (struct veh_driv *) NULL, type, 0, -1);

     }
   if (call_ptr->pickup_zone != 0)
   {
      if (type == UNZONED)
      {
	 call_ptr->pickup_zone = 0;
	 sprintf(error_str, "Call_initialize - zoned/unzoned call %d", call_ptr->call_number);
	 ERROR(fleet[call_ptr->fleet]->fleet_id, "", error_str);
      } else if (!exists)
      {
	 if ((type != MULT)&& (type != EXTRA_TAXIS) && (type != CALL_NO_MSG) && (type != CALL_W_MSG) &&
	     (type != HOL_TM_CALL) && (call_ipc_rec_ptr->personal_rqst != ONLY_PERS_REQ) &&
	     (call_ipc_rec_ptr->personal_rqst != IF_PERS_REQ) && (!call_ipc_rec_ptr->status.personal))
	 {
	    // Verify that call GPS coordinates are within the pickup zone
	   if (0)
	     GPS_verify_call_coords( (CALL_HNDL) call_ptr );
	   
	    if (Zone_add_call(call_ptr->pickup_zone, (CALL_HNDL) call_ptr) == FAIL)
	    {
	       type = UNZONED;
	       sprintf(error_str, "Cant add call %d to zone %d",
		       call_ptr->call_number, call_ptr->pickup_zone);
	       call_ptr->pickup_zone = 0;
	       ERROR(fleet[call_ptr->fleet]->fleet_id, "", error_str);
	    }
	 }
      } else if ((type == PERSONAL) && (call_ptr->personal_request != ONLY_PERS_REQ) &&
		 (call_ptr->personal_request != IF_PERS_REQ) &&
		 !(Call_is_zoned((CALL_HNDL) call_ptr)))
      {
	 if (Zone_add_call(call_ptr->pickup_zone, (CALL_HNDL) call_ptr) == FAIL)
	 {
	    type = UNZONED;
	    call_ptr->pickup_zone = 0;
	    sprintf(error_str, "Call_initialize - zoned/unzoned call %d",
		    call_ptr->call_number);
	    ERROR(fleet[call_ptr->fleet]->fleet_id, "", error_str);
	 }
      }
   } else if (!call_ptr->call_type.wakeup)
   {
      type = UNZONED;
      call_ptr->pickup_zone = 0;
   }
   fleet_nbr = call_ptr->fleet;	/* for core dumps */


   // set premium-shift initial state
     if (call_ptr->vehicle_attributes.attr26 && call_ptr->vehicle_attributes.attr26)
       call_ptr->premium_shift_match_state = PREMIUM_INSHIFT;
     else
       call_ptr->premium_shift_match_state = NON_PREMIUM;   
   
   if (type > 0)
   {
     if (call_ptr->status.accepted)
       {
	 cl_ptr = Call_get_record(call_ptr->c_isam_num, call_ptr->call_number);
	 if ((cl_ptr != NULL) && (!strncmp(cl_ptr->extended_type, "KE", 2)))
	   Call_send_assign((CALL_HNDL)call_ptr, (VEH_HNDL)(call_ptr->veh_ptr), TRUE);
	 return ((CALL_HNDL)call_ptr);
       }
     
      if (type == WAKE_UP && ((mads_time + fleet[call_ptr->fleet]->time_call_freq +
			       fleet[call_ptr->fleet]->wakeup_lead_time + 60) < call_ptr->due_time))
      {
	 Call_cancel((CALL_HNDL) call_ptr, HNDL_NULL, TRUE, 0);
	 return (NULL);
      } else
      {
	 mk_except(call_ptr, (struct veh_driv *) NULL, type, call_ptr->msg_nbr, -1);
	 if (call_ipc_rec_ptr->status.extra_taxis)
	 {
	    Call_add_history((CALL_HNDL) call_ptr, HNDL_NULL, EXTRA_TAXIS, 0, 0L, 0);
	 }
	 if ( (call_ptr->personal_request == ONLY_PERS_REQ) && (exists) )
	 {
	    Call_add_history((CALL_HNDL) call_ptr, HNDL_NULL, PERSONAL, 0, 0L, 0);
	    if (!call_ptr->status.offered)
	      Call_match((CALL_HNDL) call_ptr);
	 } 
      }
   } else
   {

     if (call_ptr->status.accepted)
       {
	 cl_ptr = Call_get_record(call_ptr->c_isam_num, call_ptr->call_number);
	 if ((cl_ptr != NULL) && (!strncmp(cl_ptr->extended_type, "KE", 2)))
	   Call_send_assign((CALL_HNDL)call_ptr, (VEH_HNDL)(call_ptr->veh_ptr), TRUE);
       }
     else if (!call_ptr->status.offered)
       Call_match((CALL_HNDL) call_ptr);


     //     if ( ( (short)Call_get_value( (CALL_HNDL) call_ptr, CALL_TYPE_ILINK ) == TRUE ) &&
     //          ( call_ipc_rec_ptr->ilink_timeout > 0 ) )
     //       Call_add_timer((CALL_HNDL) call_ptr, T_ILINK_TIMEOUT, call_ipc_rec_ptr->ilink_timeout,
     //                      call_ilink_timeout_notify_func);
       
   }


   
   return ((CALL_HNDL) call_ptr);
}				/* end Call_initialize */

BOOLEAN
Call_is_time_call(int call_nbr)
{
  CALL_REC call_rec;

    /* Have to read call record to get PENDING status */
    call_rec.nbr = call_nbr;
    if (db_read_key(CALL_FILE_ID, &call_rec, &cl_key1, DB_EQUAL) != SUCCESS)
    {
      sprintf(error_str, "Can't read call %d from call file", call_nbr);
      ERROR(' ', "", error_str);
      return FALSE;
    }

    if (strcmp(call_rec.status, PENDING) == 0)
      return TRUE;

  return FALSE;
}
