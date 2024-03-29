/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: exception_action.c,v $
*  @(#)  $Revision: 1.27 $
*  @(#)  $Date: 2004/05/12 12:58:59 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/exception_action.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: exception_action.c,v 1.27 2004/05/12 12:58:59 jwelch Exp $";

#include <stdio.h>
#include <sys/types.h>
#include <memory.h>

#include "taxipak.h"
#include "Object.h"
#include "taxi_db.h"
#include "taxi_error.h"
#include "List_public.h"
#include "Call_public.h"
#include "Call_private.h"
#include "Call_timers_public.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "Vehicle_lists_public.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Fleet_lists_public.h"
#include "Zone_public.h"
#include "Zone_private.h"
#include "Call_msg_public.h"
#include "Call_msg_private.h"

#include "Exception_private.h"

#include "dispatch_strdefs.h"
#include "path.h"
#include "mad_error.h"
#include "sprintf.h"
#include "mad_ipc.h"
#include "kdt.h"
#include "dispatch.h"
#include "language.h"
#include "except.h"
#include "ui_ipc.h"
#include "user.h"
#include "timers.h"
#include "writer.h"
#include "mesg.h"
#include "Message_log_db.h"
#include "enhance.h"
#include "switch_ext.h"


extern char * scratch_ptr;
extern time_t mads_time;    /* system time */
extern int semid;
extern char req_buf[];
extern FLEET *fleet[];  /* shared memory fleet structure */

extern struct offsets *offset;  /* offsets of structures in shared memory */
extern long next_except;
extern struct cisam_ch writer_buf;
extern char cur_dt[];
extern char cur_tm[];
extern struct calls *call_pre_process();
extern CALL_REC *get_call_record();
extern char *scratch_ptr;
extern char *last_six();
extern struct exception_count_struct *exception_count;
extern int call_node_err;
 
struct inc_cnt cnt_buf;     /* buffer for updating count of exceptions */
  
extern short exception_debug;

/*----------------------------------------------------
 *====>	Object Functions  (should be part of objects)
 *----------------------------------------------------
 */

/*
 *   Function should be placed in CALL OBJECT 
 */

/*----------------------------------------------------
 *==>	Vehicle get status  (should be part of objects)
 *----------------------------------------------------
 */
static
BOOLEAN 
Vehicle_get_status(VEH_HNDL veh_hndl, int status)
{

   VEHICLE_T		 *veh_ptr;

   veh_ptr = ( VEHICLE_T * )veh_hndl;
   switch ( status )
   {
        case VEH_ACCEPT_STATE           :  
            return (veh_ptr->t_status.accepted);
			break;

        case VEH_BID_STATE              :  
			break;

        case VEH_CALL_OFFER_STATE       :  
            return (veh_ptr->t_status.offered);
			break;

        case VEH_EMERGENCY_STATE        :  
        case VEH_EPOSTED_STATE          :  
        case VEH_FLAG_STATE             :  
        case VEH_FLAG_REQUEST_STATE     :  
        case VEH_IDLE_STATE             :  
        case VEH_IN_USE_STATE           :  
        case VEH_LATE_METER_STATE       :  
        case VEH_LOCAL_STATE            :  
        case VEH_METER_ON_STATE         :  
        case VEH_MDT_WORKING_STATE      :  
        case VEH_MSG_STATE              :  
        case VEH_NOSHOW_STATE           :  
        case VEH_OK_TO_MATCH_STATE      :  
        case VEH_OK_TO_PICKUP_STATE     :  
        case VEH_ON_BREAK_STATE         :  
        case VEH_ON_CALL_STATE          :  
			break;

        case VEH_PICKUP_STATE           :  
			return (veh_ptr->t_status.pckup);                 
			break;

        case VEH_POSTED_STATE           :  
			return (veh_ptr->t_status.posted);
			break;

        case VEH_SHORT_METER_STATE      :  
        case VEH_SIGN_IN_STATE          :  
			break;

        case VEH_SUSPENDED_STATE        :  
        case VEH_TALK_STATE             :  
			break;

        default:
                return( FALSE );
   }
}

/*----------------------------------------------------
 *==>	Call status  (should be part of objects)
 *----------------------------------------------------
 */
static
BOOLEAN 
Call_status(CALL_HNDL call_hndl, int status)
{
   struct calls *call_ptr;

   call_ptr = ( struct calls *)call_hndl;
   switch ( status )
   {
        
        case  CALL_ACCEPT_STATE            :
                return( call_ptr->status.accepted );
        case  CALL_AVAILABLE_STATE         :
                return( call_ptr->status.available );
        case  CALL_CLOSED_STATE            :
                return( call_ptr->status.pickup );
        default:
                return( FALSE );

   }
}


/*----------------------------------------------------
 *====>	Exception Functions 
 *----------------------------------------------------
 */

/*----------------------------------------------------
 *===>	Call Related Exceptions 
 *----------------------------------------------------
 */

/*----------------------------------------------------
 *==>	except action timeout
 *----------------------------------------------------
 */
int
except_action_timeout(exception_shuttle)
	EXCEPTION_SHUTTLE 	*exception_shuttle;
{
	RET_STATUS			rc;
	CALL_HNDL			call_hndl = NULL;
	EXCEPTION			*exception = NULL;
	struct calls		*exception_call = NULL;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, "Resolve TIMEOUT for call #%d with no call ptr", 
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
		return(FAIL);
	  }

	switch (exception_shuttle->resolve_code)
	{
		case R_APPROVE:      
			rc = e_action_call_timeout_approve(call_hndl);
			if (rc != SUCCESS)
				reset_exception(exception);
			else
				remove_exception(exception);

			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

		case R_DISAPPROVE:     
			rc = e_action_call_timeout_disapprove(call_hndl);
			if (rc != SUCCESS)
				reset_exception(exception);
			else
				remove_exception(exception);

			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

		case R_MAN_VEH: 
		case R_MAN_ZONE:       
        case R_CALLBACK:
        case R_SUP_HOLD:
        case R_RESEND:    
        case R_CANCEL:
        case R_MULT:  
			rc = except_action_call_resolutions(exception_shuttle);
			break;

/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

        default:
			reset_exception( exception );
			return ( FAIL );
			break;
	} 

    return(rc);
}

/*----------------------------------------------------
 *==>	except action personal call
 *----------------------------------------------------
 */
int
except_action_personal_call(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
    RET_STATUS         rc;
	time_t				tmp_time;
	CALL_HNDL			call_hndl = NULL;
	struct calls		*exception_call = NULL;
	EXCEPTION			*exception = NULL;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, "Resolve PERSONAL for call #%d with no call ptr", 
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
		return(FAIL);
	  }

    switch (exception_shuttle->resolve_code)
    {

        case R_APPROVE:
			rc = remove_exception(exception);      
			if ( exception_call != NULL )
			  Call_match( (CALL_HNDL) exception_call );      
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);

			break;

        case R_DISAPPROVE:
			if ( exception_call != NULL )
			  {
			    Call_set_value( (CALL_HNDL)exception_call, CALL_PERSONAL_REQUEST_TYPE, (HNDL)NO );
			    Call_set_value( (CALL_HNDL)exception_call, CALL_PERSONAL_VEH_NBR, (HNDL) 0 );
			    Call_match( (CALL_HNDL) exception_call );			    
			  }			
			rc = remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;
			

		case R_MAN_ZONE:       
        case R_MAN_VEH: 
        case R_CALLBACK:
        case R_SUP_HOLD:
        case R_RESEND:    
        case R_CANCEL:
        case R_MULT:  
			rc = except_action_call_resolutions(exception_shuttle);
			break;

/*
 *			currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *			currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

        default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return(rc);
}

/*----------------------------------------------------
 *==>	except action cancel
 *----------------------------------------------------
 */
int
except_action_cancel(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{

    RET_STATUS 			rc;
	CALL_HNDL			call_hndl;
	EXCEPTION			*exception;
	struct calls		*exception_call = NULL;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, "Resolve CANCEL for call #%d with no call ptr", 
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
		return(FAIL);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
			rc = e_action_call_cancel_approve(call_hndl);
/*
 * 			e_action_call_cancel_approve calls Call_cancel() which
 *			removes the exception.
 */
			if (exception->call_ptr->status.callback)
				fleet[exception_call->fleet]->now_callbacks--;
			except_post_process(exception_call,
					    exception_shuttle->resolve_code);
			break;

        case R_DISAPPROVE:     
			rc = remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_CALLBACK:
        case R_SUP_HOLD:
        case R_RESEND:    
        case R_CANCEL:
        case R_MULT:  
			rc = except_action_call_resolutions(exception_shuttle);
			break;

/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return(rc);
}

/*----------------------------------------------------
 *==>	except action hold
 *----------------------------------------------------
 */
int
except_action_hold(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, "Resolve SUP_HOLD for call #%d with no call ptr", 
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
		return(FAIL);
	  }


    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:
        case R_DISAPPROVE:           
			rc = Call_reset_state(call_hndl, CALL_HOLD_STATE, (HNDL)R_APPROVE);
			if (rc == SUCCESS)
				remove_exception(exception);
			else
				reset_exception(exception);
			break;

        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_CALLBACK:
        case R_SUP_HOLD:
        case R_RESEND:    
        case R_CANCEL:
        case R_MULT:  
			rc = except_action_call_resolutions(exception_shuttle);
			break;

/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return(rc);
}


/*----------------------------------------------------
 *==>	except action unzoned
 *----------------------------------------------------
 */
int
except_action_unzoned(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, "Resolve UNZONED for call #%d with no call ptr", 
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_DISAPPROVE:     
			rc = remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_CALLBACK:
        case R_SUP_HOLD:
        case R_RESEND:    
        case R_CANCEL:
        case R_MULT:  
			rc = except_action_call_resolutions(exception_shuttle);
			break;

/*
 *		approve of unzoned not allowed
 */
        case R_APPROVE       :      
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH      :   
        case R_CAN_MULT      :
        case R_M_MAN_VEH     :

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}


/*----------------------------------------------------
 *==>	except action call back
 *----------------------------------------------------
 */
int
except_action_call_back(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, "Resolve CALL_BACK for call #%d with no call ptr", 
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
		return(FAIL);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
        case R_DISAPPROVE:     
			if (fleet[exception_call->fleet]->now_callbacks > 0)
				fleet[exception_call->fleet]->now_callbacks--;
																	   
			rc = remove_exception(exception);
			except_post_process(exception_call, 
					    exception_shuttle->resolve_code);

			break;

        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_CALLBACK:
        case R_SUP_HOLD:
        case R_RESEND:    
        case R_CANCEL:
			if (fleet[exception_call->fleet]->now_callbacks > 0)
				fleet[exception_call->fleet]->now_callbacks--;
			/* Purposely falling through without 'break'	*/
        case R_MULT:  
			rc = except_action_call_resolutions(exception_shuttle);
			break;

/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return(rc);
}


/*----------------------------------------------------
 *==>	except action call out
 *----------------------------------------------------
 */
int
except_action_call_out(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;

	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, "Resolve CALL_OUT for call #%d with no call ptr", 
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
        case R_DISAPPROVE:     
			rc = remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_CALLBACK:
        case R_SUP_HOLD:
        case R_RESEND:    
        case R_CANCEL:
        case R_MULT:  
			rc = except_action_call_resolutions(exception_shuttle);
			break;

/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return(rc);
}


/*----------------------------------------------------
 *==>	except action eta
 *----------------------------------------------------
 */
int
except_action_eta(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;


    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
        case R_DISAPPROVE:     
			rc = remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_RESEND:    
        case R_MULT:  
        case R_CANCEL:
        case R_SUP_HOLD:
        case R_CALLBACK:
			rc = except_action_call_resolutions(exception_shuttle);
			break;

/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}

/*----------------------------------------------------
 *==>	except action no show
 *----------------------------------------------------
 */
int
except_action_no_show(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	VEHICLE_T		*veh_ptr;
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, "Resolve NO_SHOW for call #%d with no call ptr", 
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
		return(FAIL);
	  }


    switch (exception_shuttle->resolve_code)
    {
/*
 *	fix
 *	it looks like the exception could get left in a stuck position
 */
        case R_APPROVE:      
			if ((veh_ptr = exception->veh_ptr) == NULL) 
			  {
				sprintf(scratch_ptr, "except_action_no_show - no veh_ptr");
				ERROR(' ', "", scratch_ptr);
				reset_exception(exception);
				return(FAIL);
			  } 
			else 
			if (exception_call->veh_ptr != veh_ptr)
			  {
				sprintf(scratch_ptr, 
					"except_action_no_show - veh_ptr %x call_ptr veh_ptr %x",
					veh_ptr, exception_call->veh_ptr);
				ERROR(exception_call->fleet, "", scratch_ptr);
			  }
			rc = Call_reset_state(call_hndl, CALL_NOSHOW_STATE, 
					(HNDL)R_APPROVE);


                        
			if (rc == SUCCESS)
				remove_exception(exception); 
			else
				reset_exception(exception);

                        // generate a VOICE exception for just the vehicle
                        // because the CALL is about to be destroyed
                        if ( exception_call->status.req_talk )
                          mk_except((struct calls *) NULL, veh_ptr, REQ_TO_TLK, 0, -1);                        
			except_post_process(exception_call,
					    exception_shuttle->resolve_code);
			break;

        case R_DISAPPROVE:
			rc = Call_reset_state(call_hndl, CALL_NOSHOW_STATE, 
					(HNDL)R_DISAPPROVE);
			if (rc == SUCCESS)
				remove_exception(exception); 
			else
				reset_exception(exception); 
			except_post_process(exception_call,
                                        exception_shuttle->resolve_code);
			break;

        case R_MAN_VEH:       
        case R_MAN_ZONE:       
        case R_CALLBACK:
        case R_SUP_HOLD:
        case R_RESEND:    
        case R_CANCEL:
        case R_MULT:  
			rc = except_action_call_resolutions(exception_shuttle);
			break;

/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *			currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}

/*----------------------------------------------------
 *==>	except action ns_short -- short no show
 *----------------------------------------------------
 */
int
except_action_ns_short(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	VEHICLE_T		*veh_ptr;
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, "Resolve NS_SHORT for call #%d with no call ptr", 
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
		return(FAIL);
	  }


    switch (exception_shuttle->resolve_code)
    {
/*
 *	fix
 *	it looks like the exception could get left in a stuck position
 */
        case R_APPROVE:      
			if ((veh_ptr = exception->veh_ptr) == NULL) 
			  {
				sprintf(scratch_ptr, "except_action_ns_short - no veh_ptr");
				ERROR(' ', "", scratch_ptr);
				reset_exception(exception);
				return(FAIL);
			  } 
			else 
			if (exception_call->veh_ptr != veh_ptr)
			  {
				sprintf(scratch_ptr, 
					"except_action_ns_short - veh_ptr %x call_ptr veh_ptr %x",
					veh_ptr, exception_call->veh_ptr);
				ERROR(exception_call->fleet, "", scratch_ptr);
			  }
			rc = Call_reset_state(call_hndl, CALL_NOSHOW_STATE, 
					(HNDL)R_APPROVE);
			if (rc == SUCCESS)
				remove_exception(exception); 
			else
				reset_exception(exception);
			except_post_process(exception_call,
					    exception_shuttle->resolve_code);
			break;

        case R_DISAPPROVE:
			rc = Call_reset_state(call_hndl, CALL_NOSHOW_STATE, 
					(HNDL)R_DISAPPROVE);
			if (rc == SUCCESS)
				remove_exception(exception); 
			else
				reset_exception(exception); 
			except_post_process(exception_call,
                                        exception_shuttle->resolve_code);
			break;

        case R_MAN_VEH:       
        case R_MAN_ZONE:       
        case R_CALLBACK:
        case R_SUP_HOLD:
        case R_RESEND:    
        case R_CANCEL:
        case R_MULT:  
			rc = except_action_call_resolutions(exception_shuttle);
			break;

/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *			currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}

/*----------------------------------------------------
 *==>	except action late meter
 *----------------------------------------------------
 */
int
except_action_late_meter(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, "Resolve LATE_METER for call #%d with no call ptr",
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
        case R_DISAPPROVE:     
			rc = remove_exception(exception); 
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_RESEND:    
        case R_MULT:  
        case R_CANCEL:
        case R_SUP_HOLD:
        case R_CALLBACK:
			rc = except_action_call_resolutions(exception_shuttle);
			break;
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}


/*----------------------------------------------------
 *==>	except action flag request
 *----------------------------------------------------
 */
int
except_action_flag_request(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	struct calls		*vehicle_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;

	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, 
				"Resolve FLAG_REQUEST for call #%d with no call ptr",
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
			if (exception->veh_ptr == NULL)
	  		  {
				rc = remove_exception(exception);
				rc = SUCCESS;
	  		  }
			else
			  {
				vehicle_call = exception->veh_ptr->call_ptr;

/*
 *	fix
 *	what about the case when exception_call = vehicle_call = NULL?
 */
				if (exception_call == vehicle_call) 
					Veh_reset_state((VEH_HNDL) exception->veh_ptr, 
						VEH_FLAG_REQUEST_STATE, (HNDL) R_APPROVE, HNDL_NULL);

				rc = remove_exception(exception);
			  }

			break;

        case R_DISAPPROVE:     
			if (exception->veh_ptr != NULL)
			  {
				rc = Veh_reset_state((VEH_HNDL)exception->veh_ptr,
						VEH_FLAG_REQUEST_STATE, (HNDL)R_DISAPPROVE, HNDL_NULL);
			  }

			remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

/*
 *		not allowed
 */
        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_RESEND:    
        case R_MULT:  
        case R_CANCEL:
        case R_SUP_HOLD:
        case R_CALLBACK:
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}



/*----------------------------------------------------
 *==>	except action bid
 *----------------------------------------------------
 */
int
except_action_bid(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
  return( FAIL );
}



/*----------------------------------------------------
 *==>	except action wake up
 *----------------------------------------------------
 */
int
except_action_wake_up(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, "Resolve WAKE_UP for call #%d with no call ptr",
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
		return(FAIL);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
        case R_DISAPPROVE:     
			rc = Call_cancel(call_hndl, HNDL_NULL, TRUE, 0);
			break;

/*
 *		not allowed
 */
        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_RESEND:    
        case R_MULT:  
        case R_CANCEL:
        case R_SUP_HOLD:
        case R_CALLBACK:
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}



/*----------------------------------------------------
 *==>	except action short meter
 *----------------------------------------------------
 */
int
except_action_short_meter(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, 
				"Resolve SHORT_METER for call #%d with no call ptr",
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
        case R_DISAPPROVE:     
			rc = remove_exception(exception); 
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

/*
 *		not allowed
 */
        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_RESEND:    
        case R_MULT:  
        case R_CANCEL:
        case R_SUP_HOLD:
        case R_CALLBACK:
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}

/*----------------------------------------------------
 *==>	except action manquech
 *----------------------------------------------------
 */
int
except_action_manquech(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, 
				"Resolve MANQUE_CH for call #%d with no call ptr",
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
        case R_DISAPPROVE:
                        if (exception_call != NULL)
			  {
			    if (bit_check(39, exception_call))
			      bit_flip(39, exception_call, 0);
			  }
			rc = remove_exception(exception); 
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

/*
 *		not allowed
 */
        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_RESEND:    
        case R_MULT:  
        case R_CANCEL:
        case R_SUP_HOLD:
        case R_CALLBACK:
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}

/*----------------------------------------------------
 *==>	except action extra taxi
 *----------------------------------------------------
 */
int
except_action_extra_taxi(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	CALL *call_ptr;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, "Resolve EXTRA_TAXI for call #%d with no call ptr",
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
		return(FAIL);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:
			rc = e_action_approve_hold(exception);
			if (rc == SUCCESS)
			{
				if ((call_ptr = (struct calls *) Calls_list_find(exception_call->call_number)) 
					!= NULL)
				call_ptr->time_entered = mads_time;
				remove_exception(exception);
			}
				
			else
				reset_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

        case R_DISAPPROVE:     
			rc = Call_cancel(call_hndl, HNDL_NULL, TRUE, 0);
/*
 *			Call cancel removes except
 */
			break;

        case R_MAN_VEH:       
        case R_MAN_ZONE:       
        case R_CALLBACK:
        case R_SUP_HOLD:
        case R_RESEND:    
        case R_CANCEL:
        case R_MULT:  
			rc = except_action_call_resolutions(exception_shuttle);
			break;

/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}

/*----------------------------------------------------
 *==>	except action call with message
 *----------------------------------------------------
 */
int
except_action_call_with_message(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, "Resolve CALL_W_MSG for call #%d with no call ptr",
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
		return(FAIL);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
			rc = e_action_approve_hold(exception);
			if (rc == SUCCESS)
				remove_exception(exception);
			else
				reset_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

        case R_DISAPPROVE:     
			rc = Call_cancel(call_hndl, HNDL_NULL, TRUE, 0);
/*
 *			Call cancel removes except
 */
			break;

        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_CALLBACK:
        case R_SUP_HOLD:
        case R_RESEND:    
        case R_CANCEL:
        case R_MULT:  
			rc = except_action_call_resolutions(exception_shuttle);
			break;

/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}

/*----------------------------------------------------
 *==>	except action call no message
 *----------------------------------------------------
 */
int
except_action_call_no_message(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, 
				"Resolve CALL_NO_MSG for call #%d with no call ptr",
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
		return(FAIL);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
			rc = e_action_approve_hold(exception);
			if (rc == SUCCESS)
				remove_exception(exception);
			else
				reset_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

        case R_DISAPPROVE:     
			rc = Call_cancel(call_hndl, HNDL_NULL, TRUE, 0);
/*
 *			Call cancel removes except
 */
			break;

        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_CALLBACK:
        case R_SUP_HOLD:
        case R_RESEND:    
        case R_CANCEL:
        case R_MULT:  
			rc = except_action_call_resolutions(exception_shuttle);
			break;
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}

/*----------------------------------------------------
 *==>	except action reject
 *----------------------------------------------------
 */
int
except_action_reject(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, "Resolve REJECT for call #%d with no call ptr",
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
		return(FAIL);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
        case R_DISAPPROVE:
        case R_CANCEL:
			rc = remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

/*
 *		not allowed
 */
        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_RESEND:    
        case R_MULT:  
        case R_SUP_HOLD:
        case R_CALLBACK:
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}

/*----------------------------------------------------
 *==>	except action holiday time call
 *----------------------------------------------------
 */
int
except_action_holiday_time_call(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, 
				"Resolve HOLIDAY_TIME_CALL for call #%d with no call ptr",
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
		return(FAIL);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
			rc = remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

        case R_DISAPPROVE:     
			rc = Call_cancel(call_hndl, HNDL_NULL, TRUE, 0);
/*
 *			Call cancel removes exception
 */
			break;

        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_CALLBACK:
        case R_SUP_HOLD:
        case R_RESEND:    
        case R_CANCEL:
        case R_MULT:  
			rc = except_action_call_resolutions(exception_shuttle);
			break;

/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}

/*----------------------------------------------------
 *==>	except action manual assign
 *----------------------------------------------------
 */
int
except_action_manual(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, "Resolve MANUAL for call #%d with no call ptr",
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
		return(FAIL);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
			if (exception->veh_ptr == NULL) 
			  {
				rc = remove_exception(exception);
			  }
			else
			if (exception->veh_ptr->call_ptr == NULL && 
				exception->veh_ptr->t_status.kdt_working) /* seriuos error */ 
			  {
				sprintf(scratch_ptr, 
					"except_action_manual - except null ptr for call #%d",
					exception_call->call_number);
				ERROR(' ', "", scratch_ptr);

				rc = reset_exception(exception);
			  } 
			else 
			  {
				if (!exception->veh_ptr->t_status.accepted) 
				  {
					Veh_set_state((VEH_HNDL)exception->veh_ptr,
							VEH_ACCEPT_STATE, (HNDL)exception_call);
				  }

				rc = Veh_set_state((VEH_HNDL)exception->veh_ptr, 
						VEH_PICKUP_STATE, (HNDL)exception_call);
				rc = Veh_reset_state((VEH_HNDL)exception->veh_ptr,
						VEH_PICKUP_STATE, (HNDL)exception_call, HNDL_NULL);

				remove_exception(exception);
			  }

			break;

        case R_DISAPPROVE:     
			rc = Call_reset_state(call_hndl, 
					CALL_OFFERED_STATE, CALL_NO_ACCEPT_PENALTY);
			remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_RESEND:    
        case R_MULT:  
        case R_CANCEL:
        case R_SUP_HOLD:
        case R_CALLBACK:
			rc = except_action_call_resolutions(exception_shuttle);
			break;
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}

/*----------------------------------------------------
 *==>	except action multiple
 *----------------------------------------------------
 */
except_action_multiple(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;

	if (call_hndl == NULL)
	  {
		sprintf(scratch_ptr, "Resolve MULTIPLE for call #%d with no call ptr",
				exception_shuttle->call_nbr);
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
			rc = e_action_approve_mult(exception);
			if (rc == SUCCESS)
				remove_exception(exception);
			else
				reset_exception(exception);

			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

        case R_DISAPPROVE:     
			rc = remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

        case R_MULT:
			rc = e_action_mult_mult( exception );
			break;

        case R_CANCEL:
			rc = e_action_cancel( exception );
			break;
/*
 *		not allowed
 */
        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_RESEND:    
        case R_SUP_HOLD:
        case R_CALLBACK:
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}


/*----------------------------------------------------
 *==>	except action call confirmation
 *----------------------------------------------------
 */
except_action_call_confirmation(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;

    return(FAIL);
}

/*----------------------------------------------------
 *==>	except action voice dispatch
 *----------------------------------------------------
 */
except_action_voice_dispatch(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


    return( FAIL );
}


/*----------------------------------------------------
 *==>	except action call resolutions
 *----------------------------------------------------
 */
except_action_call_resolutions(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (call_hndl == NULL)
	  {
		reset_exception(exception);
		return(FAIL);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_MAN_ZONE:       
			rc = r_action_manual_zone(exception_shuttle);
			break;

        case R_MAN_VEH:       
			rc = r_action_manual_vehicle(exception_shuttle);
			break;

        case R_CALLBACK      :
			rc = e_action_callback(exception);
			break;

        case R_SUP_HOLD:
			rc = e_action_sup_hold(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

        case R_RESEND        :    
			rc = e_action_resend(exception);
			break;

        case R_CANCEL:
			rc = e_action_cancel(exception);
			break;

        case R_MULT:  
			rc = r_action_multiple(exception);
			break;

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}


/*----------------------------------------------------
 *===>	Vehicle Related Exceptions 
 *----------------------------------------------------
 */

/*----------------------------------------------------
 *==>	except action request to talk
 *----------------------------------------------------
 */
int
except_action_request_to_talk(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (exception->veh_ptr == NULL) 
	  {
		sprintf(scratch_ptr, "Resolve REQUEST_TO_TALK with no veh ptr");
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
		return(FAIL);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
			rc = Veh_reset_state((VEH_HNDL) exception->veh_ptr,
						VEH_TALK_STATE, (HNDL) R_APPROVE, HNDL_NULL);
			remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

        case R_DISAPPROVE:     
			rc = Veh_reset_state((VEH_HNDL) exception->veh_ptr,
						VEH_TALK_STATE, (HNDL)R_DISAPPROVE, HNDL_NULL);
			remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

		      case R_CANCEL:
			rc = except_action_call_resolutions(exception_shuttle);
			break;

/*
 *		not allowed
 */
        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_RESEND:    
        case R_MULT:  
        case R_SUP_HOLD:
        case R_CALLBACK:
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}


/*----------------------------------------------------
 *==>	except action attribute hold
 *----------------------------------------------------
 */
int
except_action_attribute_hold(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	VEH_HNDL                        veh_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	
	exception_call = (struct calls *)exception->call_ptr;
	if ( exception_call == NULL )
	  {
	    remove_exception(exception);
	    except_post_process(exception_call, 
				exception_shuttle->resolve_code);
	    return(FAIL);
	  }
	
	Veh_get_hndl(BY_NBR, exception_call->personal_vehicle,
		     exception_call->fleet, &veh_hndl);

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:
			remove_exception(exception);      
                        if ( ( veh_hndl != HNDL_NULL ) && ( call_hndl != HNDL_NULL ) )
			  {
			    Call_set_state( call_hndl, CALL_PERS_ATTR_OVERRIDE, HNDL_NULL );
			    Call_match( call_hndl );
			  }
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

        case R_DISAPPROVE:
			if ( call_hndl != HNDL_NULL )
			  Call_cancel(call_hndl, HNDL_NULL, TRUE, 0);
			remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

		      case R_CANCEL:
			rc = except_action_call_resolutions(exception_shuttle);
			break;

/*
 *		not allowed
 */
        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_RESEND:    
        case R_MULT:  
        case R_SUP_HOLD:
        case R_CALLBACK:
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}

/*----------------------------------------------------
 *==>	except action message from driver
 *----------------------------------------------------
 */
int
except_action_message_from_driver(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
#define MSG_FROM_DRV_SEND_DETAILS   63     // TSAB wants to trigger trip details whenever this message is approved
    
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (exception->veh_ptr == NULL) 
	  {
		sprintf(scratch_ptr, "Resolve MESSAGE_FROM_DRIVER with no veh ptr");
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
	  }
	
    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:
			// TSAB will automatically resend trip details with specific message type
			if ( (0) &&
			     ( exception->info == MSG_FROM_DRV_SEND_DETAILS ) &&
			     ( exception_call != NULL ) )
			    {
                              if ( exception_call->status.accepted )
				send_assign(exception_call->veh_ptr->mid, 
					    exception_call, exception_call->veh_ptr, FALSE);
			    }
			
			rc = remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);


			break;

        case R_DISAPPROVE:     
			rc = remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

/*
 *		not allowed
 */
        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_RESEND:    
        case R_MULT:  
        case R_CANCEL:
        case R_SUP_HOLD:
        case R_CALLBACK:
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}

/*----------------------------------------------------
 *==>	except action message to driver
 *----------------------------------------------------
 */
int
except_action_message_to_driver(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (exception->veh_ptr == NULL) 
	  {
		sprintf(scratch_ptr, "Resolve MESSAGE_TO_DRIVER with no veh ptr");
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
			if (exception->veh_ptr != NULL) 
			  {
				rc = Veh_reset_state((VEH_HNDL)exception->veh_ptr, 
						VEH_MSG_STATE, (HNDL) R_APPROVE, HNDL_NULL);
			  }
			/* send message to driver with no approval required 12/19/93 */
			rc = disp_drv_mesg( exception->info, 0 );
			remove_exception(exception);
			except_post_process(exception_call,
					exception_shuttle->resolve_code);
			break;

        case R_DISAPPROVE:     
			if (exception->veh_ptr != NULL) 
			  {
				rc = Veh_reset_state((VEH_HNDL)exception->veh_ptr, 
						VEH_MSG_STATE, (HNDL) R_DISAPPROVE, HNDL_NULL);
			  }

			remove_exception(exception);
			except_post_process(exception_call,
					exception_shuttle->resolve_code);
			break;

/*
 *		not allowed
 */
        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_RESEND:    
        case R_MULT:  
        case R_CANCEL:
        case R_SUP_HOLD:
        case R_CALLBACK:
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}

  
/*----------------------------------------------------
 *==>	except action emergency
 *----------------------------------------------------
 */
int
except_action_emergency(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (exception->veh_ptr == NULL) 
	  {
		sprintf(scratch_ptr, "Resolve EMERGENCY with no veh ptr");
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:
			rc = Veh_reset_state((VEH_HNDL)exception->veh_ptr, 
					VEH_EMERGENCY_STATE, HNDL_NULL, HNDL_NULL);
			if (rc == SUCCESS)
				remove_exception(exception); 
			else
				reset_exception(exception); 

			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			/* set the resolved state so that the next GPS message will send an 
			   authorize message to the MDT */
			Veh_set_state((VEH_HNDL)exception->veh_ptr,
				        VEH_EMERGENCY_RESOLVED_STATE, HNDL_NULL );
			break;
      
        case R_DISAPPROVE:     
			rc = Veh_reset_state((VEH_HNDL)exception->veh_ptr, 
					VEH_EMERGENCY_STATE, HNDL_NULL, HNDL_NULL);
			if (rc == SUCCESS)
				remove_exception(exception); 
			else
				reset_exception(exception); 

			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			
			authorize( exception->veh_ptr );
			
			break;

/*
 *		not allowed
 */
        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_RESEND:    
        case R_MULT:  
        case R_CANCEL:
        case R_SUP_HOLD:
        case R_CALLBACK:
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}

/*----------------------------------------------------
 *==>	except action meter fault
 *----------------------------------------------------
 */
except_action_meter_fault(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (exception->veh_ptr == NULL) 
	  {
		sprintf(scratch_ptr, "Resolve METER_FAULT with no veh ptr");
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
	  }

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
        case R_DISAPPROVE:     
			rc = remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_RESEND:    
        case R_MULT:  
        case R_CANCEL:
        case R_SUP_HOLD:
        case R_CALLBACK:
			rc = except_action_call_resolutions(exception_shuttle);
			break;

/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}

/*----------------------------------------------------
 *==>	except action missing receipt
 *----------------------------------------------------
 */
except_action_missing_receipt(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;
	if (exception->veh_ptr == NULL) 
	  {
		sprintf(scratch_ptr, "Resolve MISSING_RECEIPT with no veh ptr");
		ERROR(' ', "", scratch_ptr);
		remove_exception(exception);
		return(FAIL);
	  } 

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:      
			rc = remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

        case R_DISAPPROVE:     
			rc = remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

/*
 *		not allowed
 */
        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_RESEND:    
        case R_MULT:  
        case R_CANCEL:
        case R_SUP_HOLD:
        case R_CALLBACK:
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}


/*----------------------------------------------------
 *===>	System Related Exceptions 
 *----------------------------------------------------
 */

/*----------------------------------------------------
 *==>	except action system error
 *----------------------------------------------------
 */
int
except_action_system_error(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:
                       if (exception->info == CALL_NODE_ERR)
				call_node_err = 0;

		       if ( (0) && ( ( exception->info >= NO_ACK_MDT_1 ) &&
							   ( exception->info <= NO_ACK_ITM_3 )) &&
			    ( (VEH_HNDL)Call_get_value((CALL_HNDL)exception_call, CALL_VEH_HNDL) != HNDL_NULL ))
			 Call_msg_send( (CALL_MSG_LIST_HNDL)Call_get_value( (CALL_HNDL)exception_call, CALL_MSG_LIST ),
				        (VEH_HNDL) Call_get_value( (CALL_HNDL)exception_call, CALL_VEH_HNDL ) );
		       else
			 rc = e_action_resend( exception );

		       rc = remove_exception( exception );
		       except_post_process(exception_call, 
					   exception_shuttle->resolve_code);
		       break;
		       
        case R_DISAPPROVE:     

			if (exception->info == CALL_NODE_ERR)
				call_node_err = 0;

			rc = remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

/*
 *		not allowed
 */
        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_MULT:
        case R_RESEND:
        case R_CANCEL:
        case R_SUP_HOLD:
        case R_CALLBACK:
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}

int
except_action_samplan(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:
		       rc = remove_exception( exception );
		       except_post_process(exception_call, 
					   exception_shuttle->resolve_code);
		       break;
		       
        case R_DISAPPROVE:     

			rc = remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

/*
 *		not allowed
 */
        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_MULT:
        case R_RESEND:
        case R_CANCEL:
        case R_SUP_HOLD:
        case R_CALLBACK:
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}

int
except_action_itm_invoice_nak(exception_shuttle)
   EXCEPTION_SHUTTLE *exception_shuttle;
{
	CALL_HNDL			call_hndl;
	struct calls		*exception_call;
	EXCEPTION			*exception;
	RET_STATUS			rc = SUCCESS;


	exception = exception_shuttle->except_ptr;
	call_hndl = (CALL_HNDL)exception->call_ptr;
	exception_call = (struct calls *)exception->call_ptr;

    switch (exception_shuttle->resolve_code)
    {
        case R_APPROVE:
                       Call_msg_send( (CALL_MSG_LIST_HNDL)Call_get_value( (CALL_HNDL)exception_call, CALL_MSG_LIST ),
				      (VEH_HNDL) Call_get_value( (CALL_HNDL)exception_call, CALL_VEH_HNDL ) );      
		       rc = remove_exception( exception );
		       except_post_process(exception_call, 
					   exception_shuttle->resolve_code);
		       break;
		       
        case R_DISAPPROVE:     

			rc = remove_exception(exception);
			except_post_process(exception_call, 
					exception_shuttle->resolve_code);
			break;

/*
 *		not allowed
 */
        case R_MAN_ZONE:       
        case R_MAN_VEH:       
        case R_MULT:
        case R_RESEND:
        case R_CANCEL:
        case R_SUP_HOLD:
        case R_CALLBACK:
/*
 *		currently handled in resolve_except before this is called
 */
        case R_DISPATCH:   
        case R_CAN_MULT:
        case R_M_MAN_VEH:

/*
 *		currently not used
 */
        case R_NO_ACTION: 
        case R_ENTERED:
        case R_UPDATED:

		default:
			reset_exception( exception );
			return ( FAIL );
			break;
    } 

    return( rc );
}


/*----------------------------------------------------
 *====>	Exception Action Functions 
 *		Called when approving or disapproving existing exception.
 *		These functions should not remove or reset the exception
 *		They should return success if the exception can be 
 *			removed, or !success if the exception should be reset
 *----------------------------------------------------
 */
/*---------------------------------------------------
 *==>	e action cancel 
 *		called when cancelling exception <resolution>
 *---------------------------------------------------
 */
int
e_action_cancel(exception)
	EXCEPTION		*exception;
{
	struct calls	*exception_call = NULL;

	if (exception_debug)
		printf("- e_action_cancel\n");

	if (exception == NULL)
		return(FAIL);

	exception_call = (struct calls *)exception->call_ptr;

/*
 *	remove the exception structure but leave the bit set on the call
 */
	lock_ex(exception->call_ptr, exception, LOCK_REMOVE);

	if (exception_call == NULL)
		return( FAIL );

	return( action_cancel( exception_call ) );
}

/*----------------------------------------------------
 *==>	e action call timeout disapprove
 *----------------------------------------------------
 */
int 
e_action_call_timeout_disapprove(CALL_HNDL call_hndl)
{
 
	CALL 	*call_ptr;

	if (exception_debug)
		printf("- e_action_call_timeout_disapprove\n");

	call_ptr = (CALL *)call_hndl;
 
	if (call_ptr == NULL)  /* mrb 10/4/90 */
	{
		ERROR(' ', "", "Disapproval of timeout with no call");
		return( FAIL );
	}
 
	Call_reset_state((CALL_HNDL) call_ptr, CALL_TIMEOUT_STATE, 
		(HNDL)R_DISAPPROVE);
	bit_flip(PERSONAL, call_ptr, 0);

	return ( SUCCESS );
}

/*----------------------------------------------------
 *==>	e action call timeout approve
 *----------------------------------------------------
 */
int
e_action_call_timeout_approve(CALL_HNDL call_hndl)
{

	CALL *call_ptr;
	FLEET *fl_ptr;
	struct zones *zn_ptr;
	int timeout;
	int flag;

	call_ptr = (CALL *) call_hndl;

	if (exception_debug)
		printf("- e_action_call_timeout_approve\n");

	if (call_ptr == NULL)	/* mrb 10/4/90 */
	{
		sprintf(scratch_ptr, "Approval of timeout with no call ptr");
		ERROR(' ', "", scratch_ptr);
		return ( SUCCESS );
	}

	if (Call_reset_state((CALL_HNDL) call_ptr, CALL_TIMEOUT_STATE, 
		(HNDL)R_APPROVE) == SUCCESS)
		return ( SUCCESS );
	else
		return ( FAIL );
}

/*----------------------------------------------------
 *==>	e action call cancel approve
 *----------------------------------------------------
 */
int
e_action_call_cancel_approve(CALL_HNDL call_hndl)
{

	CALL 		*call_ptr;
	VEHICLE_T 	*veh_ptr;
	
	if (exception_debug)
		printf("- e_action_call_cancel_approve\n");

	call_ptr = (CALL *) call_hndl;

	if (!(Call_is_zoned((CALL_HNDL) call_ptr)) && !call_ptr->status.unzoned)
		Call_update_counts((CALL_HNDL)call_ptr, INCREMENT_NOW);
	else 
	if (call_ptr->veh_ptr != NULL && 
		call_ptr->veh_ptr->call_nbr == call_ptr->call_number)
		Call_update_counts((CALL_HNDL)call_ptr, INCREMENT_NOW);

	veh_ptr = call_ptr->veh_ptr;

	Call_cancel((CALL_HNDL)call_ptr, (VEH_HNDL)call_ptr->veh_ptr, FALSE, 0);

	return ( SUCCESS );
}

/*---------------------------------------------------
 *==>   e action mult    mult
 *---------------------------------------------------
 */
int
e_action_mult_mult( exception )
     EXCEPTION    *exception;
{
  /** R_MULT comes through for non-lead calls of multi-trips **/
  /** Just deactivate call node and remove exception         **/
  if (exception_debug)
    printf("- e_action_approve_mult\n");
  
  if (exception == NULL || exception->call_ptr == NULL)
    return(FAIL);
  
  if (exception->call_ptr->veh_ptr == NULL && 
      exception->call_ptr->call_number != 0)
    {
      remove_except( exception );
      exception->call_ptr->except_ptr = NULL;
      Call_deactivate( (CALL_HNDL)exception->call_ptr );
    }
}

/*---------------------------------------------------
 *==>   e action approve mult
 *---------------------------------------------------
 */
int
e_action_approve_mult( exception )
     EXCEPTION    *exception;
{
  FLEET_HNDL       fleet_hndl;
  EXCEPTION        *pExcpt;
  EXCPT_HNDL       excpt_hndl;
  
	if (exception_debug)
		printf("- e_action_approve_mult\n");

	if (exception == NULL || exception->call_ptr == NULL)
		return(FAIL);

	if (exception->call_ptr->veh_ptr == NULL && 
	    exception->call_ptr->call_number != 0)
	  {
	    if (!Call_is_zoned((CALL_HNDL)exception->call_ptr))
	      {
		if (Zone_add_call(exception->call_ptr->pickup_zone, 
				  (CALL_HNDL)exception->call_ptr) == FAIL)
		  { 
		    bit_flip(UNZONED, exception->call_ptr, 1);
		    exception->call_ptr->pickup_zone = 0;
		    
		    sprintf(scratch_ptr, "e_action_approve_hold - unzoned call %d",
			    exception->call_ptr->call_number);
		    ERROR(fleet[exception->call_ptr->fleet]->fleet_id, "", 
			  scratch_ptr);
		  }			   
	      }
	  }

	return(SUCCESS);
      }

/*---------------------------------------------------
 *==>	e action approve hold  
 *---------------------------------------------------
 */
int
e_action_approve_hold(exception)
	EXCEPTION	*exception;
{

	if (exception_debug)
		printf("- e_action_approve_hold\n");

	if (exception == NULL || exception->call_ptr == NULL)
		return(FAIL);

	if (exception->call_ptr->veh_ptr == NULL && 
		exception->call_ptr->call_number != 0)
	  {
		if (!Call_is_zoned((CALL_HNDL)exception->call_ptr))
		  {
			if (Zone_add_call(exception->call_ptr->pickup_zone, 
				(CALL_HNDL)exception->call_ptr) == FAIL)
			  { 
				bit_flip(UNZONED, exception->call_ptr, 1);
				exception->call_ptr->pickup_zone = 0;

				sprintf(scratch_ptr, "e_action_approve_hold - unzoned call %d",
						exception->call_ptr->call_number);
				ERROR(fleet[exception->call_ptr->fleet]->fleet_id, "", 
						scratch_ptr);
			  }			   
		  }   
	  }

	return(SUCCESS);
}

/*---------------------------------------------------
 *==>	e action callback
 *---------------------------------------------------
 */
int
e_action_callback(exception)
	EXCEPTION		*exception;
{
	struct calls	*exception_call = NULL;

	if (exception_debug)
		printf("- e_action_callback\n");

	if (exception == NULL)
		return(FAIL);

	exception_call = (struct calls *)exception->call_ptr;
	if (exception_call == NULL)
	{   
		sprintf(scratch_ptr, "CALLBACK excpt %d NULL call_ptr", exception->nbr);
		ERROR(' ', "", scratch_ptr);
		return(FAIL);
	}

	reset_exception(exception);

	return( action_callback( exception_call ) );
}

/*---------------------------------------------------
 *==>	e action sup hold 
 *---------------------------------------------------
 */
int
e_action_sup_hold(exception)
	EXCEPTION		*exception;
{
	struct calls	*exception_call = NULL;

	if (exception_debug)
		printf("- e_action_sup_hold\n");

	if (exception == NULL)
		return(FAIL);

	exception_call = (struct calls *)exception->call_ptr;

	reset_exception(exception);

	if (exception_call == NULL)
		return( FAIL );

	if (exception_call->status.sup_hold)
	{             /* call was being held */
		bit_flip(SUP_HOLD, exception_call, 0);
		(void)lock_ex(exception_call, exception, LOCK_REMOVE);
	} 
	else
	{
		mk_except(exception_call, (struct veh_driv *)NULL, SUP_HOLD, 0, -1);
		reset_exception(exception);
	}

	return( SUCCESS );
}

/*---------------------------------------------------
 *==>	e action resend 
 *---------------------------------------------------
 */
int
e_action_resend(exception)
	EXCEPTION		*exception;
{
	struct calls	*exception_call = NULL;

	if (exception_debug)
		printf("- e_action_resend\n");

	if (exception == NULL)
		return(FAIL);

	exception_call = (struct calls *)exception->call_ptr;

	reset_exception(exception);

	if (exception_call == NULL)
		return(FAIL);

	return( action_resend(exception_call) );

}

/*----------------------------------------------------
 *====>	Resolve Action Functions 
 *----------------------------------------------------
 */
/*----------------------------------------------------
 *==>	r action manual vehicle
 *----------------------------------------------------
 */
int
r_action_manual_vehicle(exception_shuttle)
	EXCEPTION_SHUTTLE	*exception_shuttle;
{
	EXCEPTION		*exception;
	VEHICLE_T		*veh_ptr = NULL;
	VEHICLE_T		*tmp_veh_ptr = NULL;
	struct calls	*call_ptr = NULL;
	RET_STATUS		rc;
	CALL_HNDL		call_hndl;
	short			veh_nbr;
	long			tmp_time;


	if (exception_shuttle == NULL)
		return( FAIL );

	exception = exception_shuttle->except_ptr;

	veh_nbr = exception_shuttle->veh_nbr;

	if (exception == NULL)
		call_ptr = (struct calls *)Calls_list_find(exception_shuttle->call_nbr);
	else
		call_ptr = exception->call_ptr;

	if (call_ptr == NULL)
		return( FAIL );

	if (veh_nbr > 0)
	{	
		rc = Veh_get_hndl(BY_NBR, veh_nbr, call_ptr->fleet, 
				(VEH_HNDL *)&veh_ptr);

		if (rc != SUCCESS) 
		{
			sprintf(scratch_ptr, "r_action_man_veh -invalid vehicle %d", 
					veh_nbr);
			ERROR(call_ptr->fleet, "", scratch_ptr);
			reset_exception(exception);
			except_post_process(call_ptr, exception_shuttle->resolve_code);
			return( SUCCESS );
		}
	}


	/* if another vehicle has the call, send redispatched message */
	tmp_veh_ptr = call_ptr->veh_ptr; 
	if (tmp_veh_ptr != NULL && tmp_veh_ptr != veh_ptr) 
	{
	           if (tmp_veh_ptr->t_status.accepted)
		     Call_update_counts((CALL_HNDL)call_ptr, INCREMENT_NOW);
		   
		   sprintf(scratch_ptr, "%s %d %s", CALL_, call_ptr->call_number, 
			   CALL_REASSIGNED);
		   send_text(tmp_veh_ptr->mid, scratch_ptr, tmp_veh_ptr, PRIORITY_3);
		   
		   Veh_reset_state((VEH_HNDL) tmp_veh_ptr, 
				   VEH_ON_CALL_STATE, HNDL_ERR, HNDL_NULL);
		   
		   tmp_veh_ptr->zone_num = 0;
		   tmp_veh_ptr->c_post_time = 0;
	  
	}
	
	tmp_veh_ptr = call_ptr->veh_ptr;
	if (tmp_veh_ptr != NULL && tmp_veh_ptr == veh_ptr) 
	  {
	    reset_exception(exception);
	    except_post_process(call_ptr, exception_shuttle->resolve_code);
	    return( SUCCESS );
	  }
	
	if (veh_nbr == 0) 
	  {
	    if (tmp_veh_ptr != NULL && tmp_veh_ptr->t_status.accepted)
	      Call_update_counts((CALL_HNDL)call_ptr, INCREMENT_NOW);
	    
	    call_ptr->status.available = 1;	/* take veh off call  */
	    call_ptr->status.offered = 0;
	    call_ptr->veh_ptr = NULL;
	    
	    writer_unassgn(call_ptr->call_number);
	    
	    if (!Call_is_zoned((CALL_HNDL) call_ptr)) 
	      {
		if (Zone_add_call(call_ptr->pickup_zone, 
				  (CALL_HNDL) call_ptr) == FAIL) 
		  {
		    bit_flip(UNZONED, call_ptr, 1);
		    call_ptr->pickup_zone = 0;	/* make 'em manually zone it */
		    sprintf(scratch_ptr, "res_except - zoned/unzoned call %d",
			    call_ptr->call_number);
		    ERROR(fleet[call_ptr->fleet]->fleet_id, 
			  "", scratch_ptr);
		  }
	      }
	  } 
	else
	  {
	    ++fleet[call_ptr->fleet]->asigns;
	    veh_ptr->assigns++;
	    
	    if (veh_ptr->t_status.accepted || 
		veh_ptr->t_status.offered ||
		veh_ptr->t_status.pckup) 
	      {
		if (veh_ptr->t_status.posted)
		  Veh_reset_state((VEH_HNDL) veh_ptr, 
				  VEH_POSTED_STATE, HNDL_ERR, HNDL_NULL);
		
		Zone_remove_call(call_ptr->pickup_zone, (CALL_HNDL) call_ptr);
		Call_update_counts((CALL_HNDL)call_ptr, DECREMENT_NOW);
		Call_delete_timer((CALL_HNDL) call_ptr, CALL_ALL_TIMERS);
		
	      } 
	    else
	      {	/* vehicle not busy with a call */
		veh_ptr->call_ptr = call_ptr;	
		veh_ptr->call_nbr = call_ptr->call_number;
		call_ptr->veh_ptr = veh_ptr;
		call_ptr->status.available = 0;	
		call_ptr->status.offered = 1;
		veh_ptr->t_status.offered = 1;
		
		if (exception != NULL && exception->type == PERSONAL) 
		  {
		    /* must fudge the time so except_post_process */
		    /* does not remove it from memory */
		    
		    tmp_time = (long)fleet[call_ptr->fleet]->
		      zone_ptr[call_ptr->pickup_zone]->
			pickup_lead_time + mads_time + 
			  fleet[call_ptr->fleet]->time_call_freq;
		    
		    call_ptr->due_time = get_super_lead(tmp_time, 
							fleet[call_ptr->fleet]->
							zone_ptr[call_ptr->pickup_zone]->super_zone,
							call_ptr->fleet);
		  }
		
		Veh_set_state((VEH_HNDL) veh_ptr, VEH_ACCEPT_STATE,
			      (HNDL) veh_ptr->call_ptr);
		
		if (!offset->status.ncp || 
		    !Veh_get_state((VEH_HNDL) veh_ptr, VEH_MDT_WORKING_STATE)) 
		  {
		    Veh_set_state((VEH_HNDL) veh_ptr, VEH_PICKUP_STATE, 
				  HNDL_NULL);
		    Veh_reset_state((VEH_HNDL) veh_ptr, VEH_PICKUP_STATE, 
				    HNDL_NULL, HNDL_NULL);
		    
					remove_exception(exception);
		    except_post_process(call_ptr, 
					exception_shuttle->resolve_code);
		    
		    return( SUCCESS );
		  }
	      }
	  }
	
#ifdef FOO
	if (call_ptr->except_ptr != NULL) 
	  {
	    if (tmp_veh_ptr != NULL)
	      tmp_veh_ptr->t_status.noshow = 0;
	    
	    remove_exception(exception);
	  }
#endif
	
	
	except_post_process(call_ptr, exception_shuttle->resolve_code);
	return( SUCCESS );
      }

/*----------------------------------------------------
 *==>	r action manual zone
 *----------------------------------------------------
 */
int
r_action_manual_zone(exception_shuttle)
	EXCEPTION_SHUTTLE	*exception_shuttle;
{
	EXCEPTION		*exception;
	struct calls	*call_ptr = NULL;
	RET_STATUS		rc;
	CALL_HNDL		call_hndl;
	short			fleet_nbr;     
	short  			pickup_zone_nbr;
	int  			call_nbr;
	FLEET_HNDL		fleet_hndl;
	FLEET_EXCPT_LIST_HNDL   fl_excpt_list;
	VEH_HNDL		veh_hndl;
	ZONE_HNDL		zone_hndl;
	EXCEPT                  *except_ptr;
        


/*
 *	initialize
 */
	if (exception_shuttle == NULL)
		return( FAIL );

	exception = exception_shuttle->except_ptr;

	if (exception == NULL)
		call_ptr = (struct calls *)Calls_list_find(exception_shuttle->call_nbr);
	else
		call_ptr = exception->call_ptr;

	if (call_ptr == NULL)     
		return( FAIL );

/*
 *	process zone if exists
 */
	if (fleet[call_ptr->fleet]->zone_ptr[exception_shuttle->zone_nbr] != 
			NULL) 
	{
/*
 *		no old zone
 */
		if (call_ptr->pickup_zone == 0) 
		{ 
			call_ptr->pickup_zone = exception_shuttle->zone_nbr;

			if ( ( (short)Call_get_value( (CALL_HNDL) call_ptr, CALL_TYPE_TIME_CALL ) ||
			    ( (short)Call_get_value( (CALL_HNDL) call_ptr, CALL_TYPE_SUBSCRIPTION ) ) )
			    &&
			    (time_t)Call_get_value( (CALL_HNDL) call_ptr, CALL_TC_ACTIVATE_TIME ) == 0 )	     
			  {
			    (void) Zone_get_hndl((short) Call_get_value((CALL_HNDL)call_ptr, CALL_PICKUP_ZONE),
						 (short) Call_get_value((CALL_HNDL)call_ptr, CALL_FLEET_NBR),
						 &zone_hndl );
			    Zone_add_tc( zone_hndl, (CALL_HNDL) call_ptr );
			  }
			else
			  if (call_ptr->veh_ptr == NULL && call_ptr->call_number != 0)
			    (void)Zone_add_call(call_ptr->pickup_zone, (CALL_HNDL)call_ptr);
		} 
		else 
/*
 *		old zone exists
 */
		  {
		        /* this will remove from TC list if call is TC */
		        (void) Zone_get_hndl(exception_shuttle->old_zone_nbr,
					     (short) Call_get_value((CALL_HNDL)call_ptr, CALL_FLEET_NBR),
					     &zone_hndl);
			
			Zone_remove_tc( zone_hndl, (CALL_HNDL) call_ptr);

			if ( ( (short)Call_get_value( (CALL_HNDL) call_ptr, CALL_TYPE_TIME_CALL ) ||
			    ( (short)Call_get_value( (CALL_HNDL) call_ptr, CALL_TYPE_SUBSCRIPTION ) ) )
			    &&
			    (time_t)Call_get_value( (CALL_HNDL) call_ptr, CALL_TC_ACTIVATE_TIME ) == 0 )
			  {
			    (void) Zone_get_hndl( exception_shuttle->zone_nbr,
						 (short) Call_get_value((CALL_HNDL)call_ptr, CALL_FLEET_NBR),
						  &zone_hndl );
			    Zone_add_tc( zone_hndl, (CALL_HNDL) call_ptr );
			  }


			    // Check if trip is being offered to vehicle - rescind offer and rezone trip
			      if ( !Call_get_state((CALL_HNDL)call_ptr, CALL_AVAILABLE_STATE) &&
				  Call_get_state((CALL_HNDL)call_ptr, CALL_OFFERED_STATE) )
				Call_reset_state((CALL_HNDL)call_ptr, CALL_OFFERED_STATE, CALL_NAK_ON_CALL_OFFER);
			
			if (call_ptr->status.available) 
			  {  

			    (void)Zone_remove_call( call_ptr->pickup_zone, (CALL_HNDL) call_ptr );
			    

						
		    call_ptr->pickup_zone = exception_shuttle->zone_nbr;
			    
			    if (call_ptr->veh_ptr == NULL && call_ptr->call_number != 0)
			      (void)Zone_add_call(call_ptr->pickup_zone, (CALL_HNDL)call_ptr);
			    
			    if (exception != NULL)
			      reset_exception(exception);
			    
			    except_post_process(call_ptr, exception_shuttle->resolve_code);
			    return( SUCCESS );
			  }
		  }
		
		bit_flip(UNZONED, call_ptr, 0);
		if (call_ptr->except_ptr != NULL)
		  (void)lock_ex(call_ptr, call_ptr->except_ptr, LOCK_REMOVE);    
	} 
	else
	{
		if (exception != NULL)
			reset_exception(exception);
	}

/*
 *	after zoning the call do other processing
 */
	except_post_process(call_ptr, exception_shuttle->resolve_code);
	return( SUCCESS );
}

/*---------------------------------------------------
 *==>	r action callback
 *---------------------------------------------------
 */
int
r_action_callback(call_number)
	long			call_number;
{
	struct calls	*exception_call = NULL;

	if (exception_debug)
		printf("- r_action_callback\n");

	exception_call = (struct calls *)Calls_list_find(call_number);

	if (exception_call == NULL)
		return( FAIL );

	return( action_callback( exception_call ) );
}

/*---------------------------------------------------
 *==>	r action sup hold 
 *---------------------------------------------------
 */
int
r_action_sup_hold(call_number)
	long			call_number;
{
	struct calls	*exception_call = NULL;
	FLEET_EXCPT_LIST_HNDL fl_excpt_list;
	EXCEPT          *except_ptr;
	
	if (exception_debug)
		printf("- r_action_sup_hold\n");

	exception_call = (struct calls *)Calls_list_find(call_number);

	if (exception_call == NULL)
		return( FAIL );


	if (exception_call->status.sup_hold)
	{             /* call was being held */
		bit_flip(SUP_HOLD, exception_call, 0);
		(void)lock_ex(exception_call, exception_call->except_ptr, LOCK_REMOVE);
	} 
	else
	{
	        bit_flip(SUP_HOLD, exception_call, 1);
		mk_except(exception_call, (struct veh_driv *)NULL, SUP_HOLD, 0, -1);
		reset_exception(exception_call->except_ptr);
	}

	return( SUCCESS );
}

/*---------------------------------------------------
 *==>	r action resend 
 *---------------------------------------------------
 */
int
r_action_resend(exception_shuttle)
	EXCEPTION_SHUTTLE	*exception_shuttle;
{
	struct calls	*exception_call = NULL;

	if (exception_debug)
		printf("- r_action_resend\n");

	if (exception_shuttle == NULL)
		return(FAIL);

	exception_call = 
			(struct calls *)Calls_list_find(exception_shuttle->call_nbr);

	if (exception_call == NULL)
		return( FAIL );

	return( action_resend(exception_call) );
}

/*---------------------------------------------------
 *==>	r action resend 
 *---------------------------------------------------
 */
int
r_action_emerg(exception_shuttle)
	EXCEPTION_SHUTTLE	*exception_shuttle;
{
  FLEET_HNDL fleet_hndl = HNDL_NULL;
  FLEET_VEH_NBR_LIST_HNDL fleet_veh_nbr_list = HNDL_NULL;
	VEH	*exception_veh = NULL;

	if (exception_debug)
		printf("- r_action_emerg\n");

	if (exception_shuttle == NULL)
		return(FAIL);

	Fleet_get_hndl(BY_NBR, 0, &fleet_hndl);
	if ( fleet_hndl == HNDL_NULL )
	  return(FAIL);

	fleet_veh_nbr_list = (FLEET_VEH_NBR_LIST_HNDL) Fleet_get_value(fleet_hndl, FLEET_VEH_NBR_LIST);
	if ( fleet_veh_nbr_list == HNDL_NULL )
	  return(FAIL);
	
	exception_veh = 
	  (VEH *)Fleet_veh_nbr_list_find( fleet_veh_nbr_list, exception_shuttle->veh_nbr );

	if (exception_veh == NULL)
		return( FAIL );

	Veh_set_state((VEH_HNDL)exception_veh,
		      VEH_EMERGENCY_STATE, (HNDL)NULL);
	
	return(FAIL  );/*action_resend(exception_call)*/
}


/*---------------------------------------------------
 *==>	r action cancel
 *---------------------------------------------------
 */
int
r_action_cancel(call_number)
	long			call_number;
{
	struct calls	*exception_call = NULL;

	if (exception_debug)
		printf("- r_action_cancel\n");

	exception_call = (struct calls *)Calls_list_find(call_number);

	if (exception_call == NULL)
		return( FAIL );

	return( action_cancel( exception_call ) );
}

/*---------------------------------------------------
 *==>	r action multiple 
 *---------------------------------------------------
 */
int
r_action_multiple(exception)
	EXCEPTION		*exception;
{
	struct calls	*exception_call = NULL;
	struct veh_driv	*vehicle = NULL;

	if (exception_debug)
		printf("- r_action_multiple\n");

	if (exception == NULL)
		return(FAIL);

	exception_call = (struct calls *)exception->call_ptr;

	if (exception_call == NULL)
		return( FAIL );

	remove_exception(exception);

	Call_deactivate((CALL_HNDL)exception_call);

	return( SUCCESS );
}

/*----------------------------------------------------
 *====>	Generic Action Functions 
 *----------------------------------------------------
 */

/*---------------------------------------------------
 *==>	remove exception
 *---------------------------------------------------
 */
int
remove_exception(exception)
	EXCEPTION	*exception;
{

	if (exception_debug)
		printf("- remove_exception\n");

	if (exception == NULL)
		return( FAIL );

	bit_flip(exception->type, exception->call_ptr, 0);
	lock_ex(exception->call_ptr, exception, LOCK_REMOVE);

	return(SUCCESS);
}

/*---------------------------------------------------
 *==>	reset exception
 *---------------------------------------------------
 */
int
reset_exception(exception)
	EXCEPTION	*exception;
{

	if (exception_debug)
		printf("- reset_exception\n");

	if (exception == NULL)
		return( FAIL );

	lock_ex(exception->call_ptr, exception, LOCK_FREE);

	return(SUCCESS);
}

/*---------------------------------------------------
 *==>	action cancel
 *---------------------------------------------------
 */
int
action_cancel(exception_call)
	struct calls	*exception_call;
{
	struct veh_driv	*vehicle = NULL;


	if (exception_debug)
		printf("- action_cancel\n");

	if (exception_call == NULL)
		return( FAIL );

	if (fleet[exception_call->fleet]->cancel_action == '3')
	{ 
		if (!Call_is_zoned((CALL_HNDL)exception_call) && 
			!exception_call->status.unzoned)
			Call_update_counts((CALL_HNDL)exception_call, INCREMENT_NOW);
		else 
		if (exception_call->veh_ptr != NULL && 
			exception_call->veh_ptr->call_nbr == exception_call->call_number)
			Call_update_counts((CALL_HNDL)exception_call, INCREMENT_NOW);

/*
 *		store off vehicle because Call_cancel will remove it from call
 */
		vehicle = exception_call->veh_ptr;

                if ( exception_call->status.ns_short ||
                     exception_call->status.no_show )
                  Call_cancel((CALL_HNDL)exception_call, 
                              (VEH_HNDL)exception_call->veh_ptr, FALSE, 3000);
                else
                  Call_cancel((CALL_HNDL)exception_call, 
                              (VEH_HNDL)exception_call->veh_ptr, FALSE, 0);

	} 
	else
	{
		mk_except(exception_call, (struct veh_driv *)NULL, CALL_CANCEL, 0, -1);
	}

	return( SUCCESS );
}

/*---------------------------------------------------
 *==>	action callback 
 *---------------------------------------------------
 */
int
action_callback(exception_call)
	struct calls	*exception_call;
{

	if (exception_debug)
		printf("- action_callback\n");

	if (exception_call == NULL)
		return(FAIL);

	if (bit_check(CALLBACK, exception_call) > 0)
	  return(SUCCESS);
	
	mk_except(exception_call, exception_call->veh_ptr, CALLBACK, 0, -1);

	fleet[exception_call->fleet]->now_callbacks++;
	fleet[exception_call->fleet]->callbacks++;

	if (exception_call->veh_ptr != NULL)
		exception_call->veh_ptr->callbacks++;

	return(SUCCESS);
}

/*---------------------------------------------------
 *==>	action resend 
 *---------------------------------------------------
 */
int
action_resend(exception_call)
	struct calls	*exception_call;
{

        CALL_HNDL call_hndl;
	if (exception_debug)
		printf("- action_resend\n");

	if (exception_call == NULL)
		return(FAIL);

	call_hndl = (CALL_HNDL)(exception_call);
	if ((exception_call->veh_ptr != NULL) &&
	    ( Call_get_state( (CALL_HNDL)call_hndl, CALL_ACCEPT_STATE ) ) )
		send_assign(exception_call->veh_ptr->mid, 
			    exception_call, exception_call->veh_ptr, FALSE);

	return( SUCCESS );
}

