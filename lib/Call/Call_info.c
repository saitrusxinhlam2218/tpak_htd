/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call_info.c,v $
*  @(#)  $Revision: 1.20 $
*  @(#)  $Date: 2004/02/02 19:18:25 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/Call/Call_info.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Call_info.c,v 1.20 2004/02/02 19:18:25 jwelch Exp $";

#include "mad_error.h"
#include "taxipak.h"
#include "Object.h"
#include "Call_public.h"
#include "Call_private.h"
#include "language.h"
#include "Zone_public.h"
#include "Zone_private.h"
#include "fleet.h"
#include "kdt.h"
#include "veh_driv.h"
#include "enhance.h"


extern struct offsets *offset;
extern struct fleets *fleet[FLEET_MAX];	/* shared memory fleet structure */

long glEnhSwitch;

static RET_STATUS
call_value_write_check(attr)
   int   attr;
{
   switch (attr)
   {
      case CALL_GROUPED_WITH:
	  return(SUCCESS);

     default:
	 return(SUCCESS);

   }
}         /* end call_value_write_check */

HNDL
call_value(call_hndl, op, attr, value)
   CALL_HNDL call_hndl;
   int   op;
   int   attr;
   HNDL  value;
{
   struct calls *call_ptr;
   int  tmp_time;
   long lead_time;
   unsigned char *addr;
   short nb_bytes = 0;
   char error_str[80];

   if (op == PUT)
      if (call_value_write_check(attr) == FAIL)
      {
          sprintf(error_str, "Bad call_value process %d %d", op, attr);
          ERROR(' ',"", error_str);
          return((HNDL)FAIL);
      }

   call_ptr = (struct calls *) call_hndl;

   if (call_ptr == NULL)
	return((HNDL)FAIL);

   /* NEED TO PUT IN UI WRITE CHECKS */
   switch (attr)
   {
   case CALL_CURRENT_LEVEL:
      if (op == PUT)
      {
	 call_ptr->curr_level = (short) value;
	 return ((HNDL) SUCCESS);
      } else
	 return ((HNDL) call_ptr->curr_level);
   case CALL_DRIVER_ATTR_HNDL:
      if (op == PUT)
      {
         ERROR(' ',"","Attempt to PUT CALL_DRIVER_ATTR_HNDL");
         return ((HNDL) FAIL);
      } else
         return ((HNDL) &call_ptr->driver_attributes);
   case CALL_GROUPED_WITH:
      if (op == PUT)
      {
         call_ptr->grouped_with = (int) value;
         return ((HNDL) SUCCESS);
      } else
         return ((HNDL) call_ptr->grouped_with);
      break;
   case CALL_FLEET_NBR:
      if (op == PUT)
      {
	 call_ptr->fleet = (short) value;
	 return ((HNDL) SUCCESS);
      } else
	 return ((HNDL) call_ptr->fleet);
      break;
   case CALL_MULT_CALLS_LIST:
      /* if set & argument add call to call_mult_calls_lis & */
      /* put list pre in this call                           */
      /* if set & no argument this is first call of mult call */
      /* create call_mult_calls_list and add first call to it */
      printf("CALL_MULT_CALLS_LIST not implemented\n");
      return (HNDL_ERR);
   case CALL_NBR:
      if (op == PUT)
      {
	 call_ptr->call_number = (int) value;
	 return ((HNDL) SUCCESS);
      } else
	 return ((HNDL) call_ptr->call_number);
   case CALL_NBR_REJECT:
      switch (op)
      {
      case PUT:
	 call_ptr->call_nbr_reject = 0;
         return((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) call_ptr->call_nbr_reject);
      case DECREMENT:
	 call_ptr->call_nbr_reject--;
	 break;
      case INCREMENT:
	 call_ptr->call_nbr_reject++;
	 break;
      }
      break;
      
   case CALL_PERSONAL_REQUEST_TYPE:
      if (op == PUT)
	{
	  call_ptr->personal_request = (char) value;
	  return((HNDL) SUCCESS);
	} else
	  return ((HNDL)call_ptr->personal_request);
      break;
	    
      return ((HNDL) FAIL);
   case CALL_PICKUP_LEAD_TIME:
      /* get GET code from call_func.c - call_check_pckp_lead */
      if (op != GET)
      {
          if (op == PUT)
             return((HNDL) FAIL);
          else
	     return (HNDL_ERR);
      }
      else
      {
	 if (call_ptr->call_type.wakeup)
	    return ((HNDL) (call_ptr->due_time - (long) fleet[call_ptr->fleet]->wakeup_lead_time));
	 else if (call_ptr->call_type.xtype)
	   tmp_time = fleet[call_ptr->fleet]->zone_ptr[call_ptr->pickup_zone]->typex_lead_time; /* get zone lead */
	 else
	   tmp_time =  fleet[call_ptr->fleet]->zone_ptr[call_ptr->pickup_zone]->pickup_lead_time; /* get zone lead */

	 if ( glEnhSwitch & ENH0006 )
	   {
	     lead_time = call_ptr->due_time - (time_t)get_new_lead(tmp_time, call_ptr->due_time, call_ptr->fleet, offset) -
	       ( fleet[call_ptr->fleet]->time_calls_lead_supp * 60 );
	   }
	 else
	   {
#ifdef SCANDINAVIA
	     lead_time = call_ptr->due_time - tmp_time;
#else
	     if ( ( call_ptr->personal_request == C_PERSONAL_ONLY ) || ( call_ptr->personal_request == C_PERSONAL_IF ) )
	       lead_time = call_ptr->due_time - tmp_time - fleet[call_ptr->fleet]->pers_lead_tm;
	     else
	       lead_time = call_ptr->due_time - tmp_time;
#endif
	   }
	 return ((HNDL) lead_time);
      }
   case CALL_PICKUP_ZONE:
      if (op == PUT)
      {
	 if (call_ptr->call_type.wakeup)
	   call_ptr->pickup_zone = WAKEUP_ZONE;
	 else
	   call_ptr->pickup_zone = (short) value;
	 return ((HNDL) SUCCESS);
      } else
	{
	  if (call_ptr->call_type.wakeup)
	    call_ptr->pickup_zone = WAKEUP_ZONE;
	 return ((HNDL) call_ptr->pickup_zone);
	}
   case CALL_PRIORITY:
      if (op == PUT)
      {
         call_ptr->priority = (short) value;
         return ((HNDL) SUCCESS);
      } else
         return ((HNDL) call_ptr->priority);
   case CALL_REJECT_VEH_LIST:
      if (op == PUT)
      {
	 call_ptr->call_reject_veh_list = (CALL_REJECT_VEH_LIST_HNDL) value;
	 return ((HNDL) SUCCESS);
      } else
	 return ((HNDL) call_ptr->call_reject_veh_list);
      break;

    case CALL_TIMER_LIST:
      if (op == PUT)
	{
	  call_ptr->timer_list = (CALL_TIMER_LIST_HNDL) value;
	  return ((HNDL) SUCCESS);
	} else
	  return ((HNDL) call_ptr->timer_list);
      break;
      
   case CALL_TIME_ENTERED:
      if (op == PUT)
      {
	 call_ptr->time_entered = (time_t) value;
	 return ((HNDL) SUCCESS);
      } else
	 return ((HNDL) call_ptr->time_entered);

   case CALL_ACCEPT_TIME:
      if (op == PUT)
      {
	 call_ptr->accept_time = (time_t) value;
	 return ((HNDL) SUCCESS);
      } else
	  return ((HNDL) call_ptr->accept_time);
      

    case CALL_TC_ACTIVATE_TIME:
      if (op == PUT)
      {
	 call_ptr->tc_activate_time = (time_t) value;
	 return ((HNDL) SUCCESS);
      } else
	 return ((HNDL) call_ptr->tc_activate_time);

    case CALL_M_ACTION_RETRY:
      if (op == PUT)
	{
	  call_ptr->m_action_retry = (short) value;
	  return((HNDL) SUCCESS);
	} else
	  return ((HNDL) call_ptr->m_action_retry);
      
    case CALL_TYPE_TIME_CALL:
      if ( op == GET )
	{
	  if ( call_ptr->call_type.time )
	    return((HNDL) TRUE);
	  else
	    return((HNDL) FALSE);
	}
      return(HNDL_ERR);

   case CALL_TYPE_ILINK:
     if ( op == GET )
       {
         if ( call_ptr->call_type.ilink )
           return((HNDL) TRUE);
         else
           return((HNDL) FALSE);
       }
     else if ( op == PUT )
       {
         call_ptr->call_type.ilink = (short) value;
         return ((HNDL)SUCCESS);
       }
     return(HNDL_ERR);

   case CALL_TYPE_ILINK_REJECT:
     if ( op == GET )
       {
         if ( call_ptr->call_type.ilink_reject )
           return((HNDL) TRUE);
         else
           return((HNDL) FALSE);
       }
     else if ( op == PUT )
       {
         call_ptr->call_type.ilink_reject = (short) value;
         return ((HNDL)SUCCESS);
       }
     
     return(HNDL_ERR);     
     

    case CALL_TYPE_SUBSCRIPTION:
      if ( op == GET )
	{
	  if ( call_ptr->call_type.subscription )
	    return((HNDL) TRUE);
	  else
	    return((HNDL) FALSE);
	}
      return(HNDL_ERR);      
      
   case CALL_VEH_DRIV_ATTR:
     if (op != GET)
       {
         ERROR(' ', "", " Illegal operation on CALL_VEH_DRIV_ATTR");
         if (op == PUT)
	   return((HNDL) FAIL);
         else
	   return (HNDL_ERR);
       }
     addr = (unsigned char *) &call_ptr->vehicle_attributes;
     for ( nb_bytes = 0; nb_bytes < 4; nb_bytes++)
       {
	 if (*addr)
	   return ((HNDL) TRUE);
	 addr++;
       }
     addr = (unsigned char *) &call_ptr->driver_attributes;
     for ( nb_bytes = 0; nb_bytes < 4; nb_bytes++)
       {
	 if (*addr)
	   return ((HNDL) TRUE);
	 addr++;
       }
     return ((HNDL) FALSE);

     break;

   case CALL_VEH_ATTR_HNDL:
      if (op == PUT)
      {
         ERROR(' ',"","Attempt to PUT CALL_VEH_ATTR_HNDL");
         return ((HNDL) FAIL);
      } else
         return ((HNDL) &call_ptr->vehicle_attributes);
   case CALL_VEH_HNDL:
      if (op == PUT)
      {
	 call_ptr->veh_ptr = (struct veh_driv *) value;
	 return ((HNDL) SUCCESS);
      } else
	 return ((HNDL) call_ptr->veh_ptr);

    case CALL_MSG_LIST:
      if (op == PUT)
	{
	  call_ptr->call_msg_list_hndl = (CALL_MSG_LIST_HNDL)value;
	  return ((HNDL) SUCCESS);
	}
      else
	return ((HNDL) call_ptr->call_msg_list_hndl);
      
    case CALL_MSG_NBRS:
      if (op == PUT)
	{
	  call_ptr->call_msg_nbr = (char)value;
	  return ((HNDL) SUCCESS);
	}
      else
	return ((HNDL) call_ptr->call_msg_nbr);
		
	    
    case CALL_TIME_DUE:
      if ( op == PUT )
	{
	  call_ptr->due_time = (time_t) value;
	  return ((HNDL) SUCCESS );
	}
      else
	return ((HNDL) call_ptr->due_time);
      
    case CALL_EXCPT_LIST:
      if (op == GET)
	return ((HNDL) call_ptr->call_except_list_hndl);
      else
	return ((HNDL) HNDL_NULL);
      break;


    case CALL_PERSONAL_VEH_NBR:
      if ( op == GET )
	return ((HNDL)call_ptr->personal_vehicle);
      else
	{
	  call_ptr->personal_vehicle = (short) value;
	  return ((HNDL) SUCCESS );
	}
      break;
		
   default:
      printf("ILLEGAL VALUE FOR call_value %d\n", attr);
      if (op == PUT)
	 return ((HNDL) FAIL);
      else
	 return ((HNDL) NULL);
   }				/* end case */
   return ((HNDL) SUCCESS);
}				/* end call_value */

HNDL
Call_get_value(call_hndl, attr)
   CALL_HNDL call_hndl;
   int   attr;
{
  return (call_value(call_hndl, GET, attr, NULL));
}				/* end Call_get_value */

RET_STATUS
Call_get_info(call_hndl, attr, value_ptr)
   CALL_HNDL call_hndl;
   int   attr;
   HNDL * value_ptr;
{
   if (value_ptr == NULL)
      return(FAIL);
   *value_ptr = call_value(call_hndl, GET, attr, NULL);
   return(SUCCESS);
}                              /* end Call_get_info */

RET_STATUS
call_get_state(call_hndl, state)
   CALL_HNDL call_hndl;
   int   state;
{
   struct calls *call_ptr;
   struct veh_driv *veh_ptr;

   if (call_hndl == HNDL_ERR)
      return(FAIL);
   /* NEED TO TURN DIRECT VEH REF TO GET STATE CALLS */

   call_ptr = (struct calls *) call_hndl;
   veh_ptr = call_ptr->veh_ptr;
   switch (state)
   {

   case CALL_ACCEPT_STATE:
      return (call_ptr->status.accepted ? SUCCESS : FAIL);

   case CALL_AVAILABLE_STATE:
      return (call_ptr->status.available ? SUCCESS : FAIL);

   case CALL_BID_STATE:
      return (call_ptr->status.upforbid ? SUCCESS : FAIL);

   case CALL_FLAG_REQUEST_STATE:
       return (call_ptr->veh_ptr->t_status.flag ? SUCCESS : FAIL);

   case CALL_HOLD_STATE:
      return (call_ptr->status.sup_hold ? SUCCESS : FAIL);

   case CALL_LATE_METER_STATE:	/* WAS do_latemeter */
       return ((int) call_ptr->veh_ptr->t_status.late_meter ? SUCCESS : FAIL);

   case CALL_MULT_CALL_STATE:	/* WAS call_ptr->grouped_with field */
      ERROR(' ', "", "CALL_MULT_CALL_STATE not implemented");
      return (FAIL);

   case CALL_NOSHOW_STATE:	/* WAS part of d_no_show  */

      if (call_ptr->veh_ptr)
      {

	  if ( call_ptr->veh_ptr->call_ptr != NULL )
	      return ((int) call_ptr->veh_ptr->t_status.noshow ? SUCCESS : FAIL);
	  else
	      return ((int) call_ptr->status.no_show ? SUCCESS : FAIL );
	  
      } else
	 return (FAIL);

   case CALL_OFFERED_STATE:	/* WAS reset - d_reject  & no_accept */
      return (call_ptr->status.offered ? SUCCESS : FAIL);

   case CALL_PICKUP_STATE:
      return ((int) call_ptr->status.pickup ? SUCCESS : FAIL);

   case CALL_SHORT_METER_STATE:
      /* CANT GET */
      return ((int) FAIL);

   case CALL_TIMEOUT_STATE:	/* WAS do_timeout */
      return ((int) call_ptr->timeout_flag ? SUCCESS : FAIL);

    case CALL_PERS_ATTR_OVERRIDE:
      return ((int) call_ptr->status.pers_attr_override ? SUCCESS : FAIL );
      
   case CALL_WAIT_EPOST_STATE:
      return ((int) call_ptr->status.wait_epost ? SUCCESS : FAIL);
   }				/* end case */
}				/* end call_get_state */

int
Call_get_state(call_hndl, state)
   CALL_HNDL call_hndl;
   int   state;
{
   int   rc;

   rc = call_get_state(call_hndl, state);

   if (rc == SUCCESS)
      return (TRUE);
   else
      return (FALSE);
}				/* end Call_set_state */

CallAttributes( pVehAttr, pDrvAttr, pCall )
     char   *pVehAttr;
     char   *pDrvAttr;
     CALL   *pCall;
{

  if ( pCall == NULL || pVehAttr == NULL || pDrvAttr == NULL )
    return( -1 );

  if ( pCall->vehicle_attributes.attr1 == 1 )
    *pVehAttr = YES;
  if ( pCall->vehicle_attributes.attr2 == 1 )
    *(pVehAttr + 1 ) = YES;
  if ( pCall->vehicle_attributes.attr3 == 1 )
    *(pVehAttr + 2 ) = YES;
  if ( pCall->vehicle_attributes.attr4 == 1 )
    *(pVehAttr + 3 ) = YES;
  if ( pCall->vehicle_attributes.attr5 == 1 )
    *(pVehAttr + 4 ) = YES;
  if ( pCall->vehicle_attributes.attr6 == 1 )
    *(pVehAttr + 5 ) = YES;
  if ( pCall->vehicle_attributes.attr7 == 1 )
    *(pVehAttr + 6 ) = YES;
  if ( pCall->vehicle_attributes.attr8 == 1 )
    *(pVehAttr + 7 ) = YES;

  if ( pCall->driver_attributes.attr1 == 1 )
    *(pDrvAttr) = YES;
  if ( pCall->driver_attributes.attr2 == 1 )
    *(pDrvAttr + 1 ) = YES;
  if ( pCall->driver_attributes.attr3 == 1 )
    *(pDrvAttr + 2 ) = YES;
  if ( pCall->driver_attributes.attr4 == 1 )
    *(pDrvAttr + 3 ) = YES;
  if ( pCall->driver_attributes.attr5 == 1 )
    *(pDrvAttr + 4 ) = YES;
  if ( pCall->driver_attributes.attr6 == 1 )
    *(pDrvAttr + 5 ) = YES;
  if ( pCall->driver_attributes.attr7 == 1 )
    *(pDrvAttr + 6 ) = YES;
  if ( pCall->driver_attributes.attr8 == 1 )
    *(pDrvAttr + 7 ) = YES;

}
