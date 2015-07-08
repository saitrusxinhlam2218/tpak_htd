/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: exception_distribute.c,v $
*  @(#)  $Revision: 1.6 $
*  @(#)  $Date: 2004/02/02 18:38:39 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/exception_distribute.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: exception_distribute.c,v 1.6 2004/02/02 18:38:39 jwelch Exp $";

#include <stdio.h>
#include <sys/types.h>
#include <memory.h>

#include "taxipak.h"
#include "Object.h"
#include "taxi_db.h"
#include "taxi_error.h"

#include "Exception_private.h"

short	exception_debug = FALSE;

/*------------------------------------------------------
 *==>	exception distribute
 *------------------------------------------------------
 */
int
exception_distribute(exception_shuttle)
	EXCEPTION_SHUTTLE	*exception_shuttle;
{
	int					exception_type;
	int					rc;


	if (exception_shuttle == NULL)
		return( FAIL );

	if (exception_debug)
		print_exception(exception_shuttle);

	if (exception_shuttle->except_ptr != NULL)
	  {
		rc = exception_distribute_resolution(exception_shuttle);
	  }
	else
	  {
		rc = exception_distribute_action(exception_shuttle);
	  }

	return( rc );
}

/*------------------------------------------------------
 *==>	exception distribute resolution
 *------------------------------------------------------
 */
int
exception_distribute_resolution(exception_shuttle)
	EXCEPTION_SHUTTLE	*exception_shuttle;
{
	int					exception_type;
	int					rc;


/*
 *	get exception type 
 */
	rc = exception_type_from_shuttle(exception_shuttle, &exception_type);
	if (rc != SUCCESS)
	  {
		return(FAIL);
	  }

/*
 *	distribute exception shuttle to appropriate function
 *	based on exception type
 */
	switch(exception_type)
	  {
		case EMERGENCY:
			rc = except_action_emergency(exception_shuttle);
			break;

		case CALL_CANCEL:
			rc = except_action_cancel(exception_shuttle);
			break;

		case UNZONED:
			rc = except_action_unzoned(exception_shuttle);
			break;

		case CALL_W_MSG:
			rc = except_action_call_with_message(exception_shuttle);
			break;

		case CALL_NO_MSG:
			rc = except_action_call_no_message(exception_shuttle);
			break;

		case EXTRA_TAXIS:
			rc = except_action_extra_taxi(exception_shuttle);
			break;

		case CALLBACK:
			rc = except_action_call_back(exception_shuttle);
			break;

		case CALL_TIMEOUT:
			rc = except_action_timeout(exception_shuttle);
			break;

		case REJECT:
			rc = except_action_reject(exception_shuttle);
			break;

		case LATE_METER:
			rc = except_action_late_meter(exception_shuttle);
			break;

		case FLAG_REQ:
			rc = except_action_flag_request(exception_shuttle);
			break;

		case NO_SHOW:
			rc = except_action_no_show(exception_shuttle);
			break;

		case CALLOUT:
			rc = except_action_call_out(exception_shuttle);
			break;

		case REQ_TO_TLK:
			rc = except_action_request_to_talk(exception_shuttle);
			break;

		case ETA:
			rc = except_action_eta(exception_shuttle);
			break;

		case MESSAGE:
			rc = except_action_message_from_driver(exception_shuttle);
			break;

		case BID:
			rc = except_action_bid(exception_shuttle);
			break;

		case DRV_MSG:
			rc = except_action_message_to_driver(exception_shuttle);
			break;

		case WAKE_UP:
			rc = except_action_wake_up(exception_shuttle);
			break;

		case SHORT_METER:
			rc = except_action_short_meter(exception_shuttle);
			break;

		case HOL_TM_CALL:
			rc = except_action_holiday_time_call(exception_shuttle);
			break;

		case SYS_ERR:
			rc = except_action_system_error(exception_shuttle);
			break;

		case SUP_HOLD:
			rc = except_action_hold(exception_shuttle);
			break;

		case MANUAL:
			rc = except_action_manual(exception_shuttle);
			break;

		case PERSONAL:
			rc = except_action_personal_call(exception_shuttle);
			break;

		case MFAULT:
			rc = except_action_meter_fault(exception_shuttle);
			break;

		case MULT:
			rc = except_action_multiple(exception_shuttle);
			break;

		case MISSING_RECEIPT_RETRY_LIMIT:
			rc = except_action_missing_receipt(exception_shuttle);
			break;

               case ATTRIBUTE_HOLD:
			rc = except_action_attribute_hold(exception_shuttle);
			break;

	  case NS_SHORT:
	      rc = except_action_ns_short(exception_shuttle);
	      break;

		      case SP_EXCPT_01 :
		      case SP_EXCPT_02 :
		      case SP_EXCPT_03 :
		      case SP_EXCPT_04 :
		      case SP_EXCPT_05 :
		      case SP_EXCPT_06 :
		      case SP_EXCPT_07 :
		      case SP_EXCPT_08 :
		      case SP_EXCPT_09 :
		      case SP_EXCPT_10 :
		      case SP_EXCPT_11 :
		      case SP_EXCPT_12 :
		      case SP_EXCPT_13 :
		      case SP_EXCPT_14 :
		      case SP_EXCPT_15 :
			rc = except_action_samplan(exception_shuttle);
			break;

		      case ITM_INVOICE_NAK:
			rc = except_action_itm_invoice_nak( exception_shuttle );
			break;

		default:
			rc = FAIL;
			break;
	  }

	return(rc);
}

/*------------------------------------------------------
 *==>	exception distribute action
 *------------------------------------------------------
 */
int
exception_distribute_action(exception_shuttle)
	EXCEPTION_SHUTTLE	*exception_shuttle;
{
	int					rc;


/*
 *	distribute action to appropriate action function
 */
	switch( exception_shuttle->resolve_code )
	  {
		case R_CANCEL:
			rc = r_action_cancel(exception_shuttle->call_nbr);
			break;

		case R_SUP_HOLD:
			rc = r_action_sup_hold(exception_shuttle->call_nbr);
			break;

		case R_CALLBACK:
			rc = r_action_callback(exception_shuttle->call_nbr);
			break;

		case R_MAN_VEH:
			rc = r_action_manual_vehicle(exception_shuttle);
			break;

		case R_MAN_ZONE:
			rc = r_action_manual_zone(exception_shuttle);
			break;

		case R_RESEND:
			rc = r_action_resend(exception_shuttle);
			break;

	  case R_EMERG:
	    rc = r_action_emerg(exception_shuttle);
	    break;
	    
		case R_DISPATCH:
		case R_MULT:
		case R_CAN_MULT:
		case R_M_MAN_VEH:
		default:
			rc = FAIL;
			break;
	  }

	return( rc );
}

/*------------------------------------------------------
 *==>	exception type from shuttle
 *------------------------------------------------------
 */
int
exception_type_from_shuttle(exception_shuttle, exception_type)
	EXCEPTION_SHUTTLE	*exception_shuttle;
	int					*exception_type;
{

	if (exception_shuttle == NULL)
	  {
		*exception_type = 0;
		return(FAIL);
	  }

	if (exception_shuttle->except_ptr == NULL)
	  {
		*exception_type = 0;
		return(FAIL);
	  }

	if (exception_shuttle->except_ptr->type <= 0)
	  {
		*exception_type = 0;
		return(FAIL);
	  }

	*exception_type = exception_shuttle->except_ptr->type;

	return(SUCCESS);
}


/*------------------------------------------------------
 *==>	print exception 
 *------------------------------------------------------
 */

print_exception(exception_shuttle)
	EXCEPTION_SHUTTLE   *exception_shuttle;
{


	if (exception_shuttle == NULL)
	  {
		return(FAIL);
	  }

	printf("--Exception-----------\n");
	printf("  exception #     %ld\n", exception_shuttle->except_nbr);
	printf("  resolve code    %d\n",  exception_shuttle->resolve_code);
	printf("  call #          %ld\n", exception_shuttle->call_nbr);
	printf("  vehicle #       %d\n",  exception_shuttle->veh_nbr);
	printf("  zone #          %d\n",  exception_shuttle->zone_nbr);
	printf("  -----------\n");

	if (exception_shuttle->except_ptr)
	{
	printf("  exception #     %ld\n", exception_shuttle->except_ptr->nbr);
	printf("  type            %d\n",  exception_shuttle->except_ptr->type);
	printf("  zone #          %d\n",  exception_shuttle->except_ptr->zone_nbr);
	printf("  veh #           %d\n",  exception_shuttle->except_ptr->veh_nbr);
	printf("  in use          %c\n",  exception_shuttle->except_ptr->inuse);

	if (exception_shuttle->except_ptr->call_ptr)
	{

	}

	printf("  exception #     %d\n",  exception_shuttle->zone_nbr);
	}
	printf("  -----------\n");

	return(SUCCESS);
}

