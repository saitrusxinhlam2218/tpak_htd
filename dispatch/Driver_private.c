/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Driver_private.c,v $
*  @(#)  $Revision: 1.6 $
*  @(#)  $Date: 2004/05/12 12:56:39 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Driver_private.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Driver_private.c,v 1.6 2004/05/12 12:56:39 jwelch Exp $";

/* Driver_private.c */
#include <sys/types.h>		/* temp */
#include <sys/times.h>		/* temp */
#include "mad_error.h"
#include "taxipak.h"
#include "Object.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Driver_public.h"
#include "Driver_private.h"
#include "Driver_db.h"
#include "Call_public.h"
#include "List_public.h"
#include "LL.h"
#include "Fleet_lists_public.h"
#include "dispatch.h"
#include "mad_ipc.h"
#include "taxi_db.h"
#include "path.h"
#include "acc.h"
#include "msg_mmp.h"
#include "language.h"
#include "dispatch_strdefs.h"
#include "f_msg_defs.h"

extern time_t mads_time;
extern FLEET *fleet[ ];	/* shared memory fleet structure */
extern char req_buf[];
extern struct offsets *offset;	/* pointer to offset structure */
extern short debug_state;
extern LinkedList *fleet_veh_id_list;

struct veh_driv *
driver_hndl_to_ptr(driver_hndl)
   DRIVER_HNDL driver_hndl;
{
   return ((struct veh_driv *) driver_hndl);
}				/* end driver_hndl_to_ptr */

static HNDL
driver_value(driver_hndl, op, attr, value)
   DRIVER_HNDL driver_hndl;
   int   op;
   int   attr;
   HNDL  value;
{
   struct veh_driv *driver_ptr;
   int   cc;
   struct cisam_dr *drv;

   driver_ptr = driver_hndl_to_ptr(driver_hndl);

   switch (attr)
   {
   case DRIVER_ATTR:
      ERROR(' ',"","case DRIVER_ATTR not implemented");
      if (op == PUT)
         return((HNDL) FAIL);
       else
         return(HNDL_ERR);

   case DRIVER_CLASS:
      if (op == PUT) 
         driver_ptr->driver_class = (char) value;
       else 
         return ((HNDL) driver_ptr->driver_class);
      break;

   case DRIVER_FLEET_ID:
      if (op == PUT)
	 driver_ptr->fleet = (char) value;
      else
	 return ((HNDL) driver_ptr->fleet);
      break;

   case DRIVER_FLEET_NBR:
      if (op == PUT)
	 driver_ptr->fleet_nbr = (short) value;
      else
	 return ((HNDL) driver_ptr->fleet_nbr);
      break;

   case DRIVER_ID:
      if (op == PUT)
	 driver_ptr->driver_id = (short) value;
      else
	 return ((HNDL) driver_ptr->driver_id);
      break;

      /* STATISTICS **************** */

   case DRIVER_NBR_ASSIGNS:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->assigns++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->assigns);
      case PUT:
	 driver_ptr->assigns = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->assigns--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_BREAKS:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->breaks++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->breaks);
      case PUT:
	 driver_ptr->breaks = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->breaks--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_CALLBACKS:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->callbacks++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->callbacks);
      case PUT:
	 driver_ptr->callbacks = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->callbacks--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_CALLOUTS:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->callouts++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->callouts);
      case PUT:
	 driver_ptr->callouts = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->callouts--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_CALLS:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->calls++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->calls);
      case PUT:
	 driver_ptr->calls = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->calls--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_COND_POSTS:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->cond_posts++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->cond_posts);
      case PUT:
	 driver_ptr->cond_posts = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->cond_posts--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_EMERGENCIES:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->emergencies++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->emergencies);
      case PUT:
	 driver_ptr->emergencies = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->emergencies--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_ENROUTE_POSTS:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->enroute_posts++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->enroute_posts);
      case PUT:
	 driver_ptr->enroute_posts = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->enroute_posts--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_FLAGS:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->flags++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->flags);
      case PUT:
	 driver_ptr->flags = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->flags--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_GOING_HOMES:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->going_homes++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->going_homes);
      case PUT:
	 driver_ptr->going_homes = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->going_homes--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_LATE_METERS:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->late_meters++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->late_meters);
      case PUT:
	 driver_ptr->late_meters = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->late_meters--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_MESSAGES:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->messages++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->messages);
      case PUT:
	 driver_ptr->messages = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->messages--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_NO_ACCEPTS:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->no_accepts++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->no_accepts);
      case PUT:
	 driver_ptr->no_accepts = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->no_accepts--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_NO_SHOWS:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->no_shows++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->no_shows);
      case PUT:
	 driver_ptr->no_shows = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->no_shows--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_POSTS:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->posts++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->posts);
      case PUT:
	 driver_ptr->posts = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->posts--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_REJECTS:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->rejects++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->rejects);
      case PUT:
	 driver_ptr->rejects = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->rejects--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_REQ_TO_TALK:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->req_to_talk++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->req_to_talk);
      case PUT:
	 driver_ptr->req_to_talk = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->req_to_talk--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_SHORT_METERS:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->short_meters++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->short_meters);
      case PUT:
	 driver_ptr->short_meters = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->short_meters--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_SIGN_INS:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->sign_ins++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->sign_ins);
      case PUT:
	 driver_ptr->sign_ins = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->sign_ins--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }
   case DRIVER_NBR_SIGN_OFFS:
      switch (op)
      {
      case INCREMENT:
	 driver_ptr->sign_offs++;
	 return ((HNDL) SUCCESS);
      case GET:
	 return ((HNDL) driver_ptr->sign_offs);
      case PUT:
	 driver_ptr->sign_offs = 0;
	 return ((HNDL) SUCCESS);
      case DECREMENT:
	 driver_ptr->sign_offs--;
	 return ((HNDL) SUCCESS);
      default:
	 return ((HNDL) FAIL);
      }

      /* END STATISTICS **************** */

   case DRIVER_Q_POSITION:
      if (op != GET)
	 return ((HNDL) FAIL);
      printf("DRIVER_Q_POSITION not impmented\n");
      break;
   case DRIVER_PRIORITY:
      printf("case DRIVER_PRIORITY_DATE not implemented\n");
      break;
   case DRIVER_SIGNED_ON_DATE:
      printf("DRIVER_SIGNED_ON_DATE not implemented\n");
      break;
   case DRIVER_SUSP_DRV_MSG:
      if (op == GET)
      {
	 drv = (struct cisam_dr *) req_buf;

	 drv->fleet = fleet[driver_ptr->fleet_nbr]->fleet_id;
	 drv->id = driver_ptr->driver_id;
	 if ((cc = db(DRIVER_FILE_ID, READ_KEYS, &dr_key1, ISEQUAL, 0)) != 0)
	 {
	    ERROR(fleet[driver_ptr->fleet_nbr]->fleet_id, DRIVER_FILE, "Cannot find driver in C-ISAM data base");
	    return ((HNDL) NULL);
	 } else
	    return ((HNDL) drv->susp_message);
      }
      ERROR(' ', "", "Illegal operation for DRIVER_SUSP_SUP_MSG_HNDL");
      return ((HNDL) FAIL);
   case DRIVER_SUSP_SUP_MSG:
      printf("DRIVER_SUSP_SUP_MSG not implemented\n");
      break;
   case DRIVER_SUSP_FROM_DATE:
      printf("case DRIVER_SUSP_FROM_DATE not implemented\n");
      break;
   case DRIVER_SUSP_TO_DATE:
      printf("case DRIVER_SUSP_TO_DATE not implemented\n");
      break;
   case DRIVER_SEALS:
      printf("case DRIVER_SEALS not implemented\n");
      break;
   default:
      printf("ILLEGAL VALUE FOR driver_access %d\n", attr);
      if (op == PUT)
	 return ((HNDL) FAIL);
      else
	 return ((HNDL) NULL);
      break;
   }				/* end case */
   return ((HNDL) SUCCESS);
}				/* end driver_value */

HNDL
Driver_get_value(driver_hndl, attr)
   DRIVER_HNDL driver_hndl;
   int   attr;
{
   return (driver_value(driver_hndl, GET, attr, HNDL_NULL));
}				/* end Driver_get_value */

RET_STATUS
Driver_set_value(driver_hndl, attr, hndl)
   DRIVER_HNDL driver_hndl;
   int   attr;
   HNDL  hndl;
{
   HNDL  status;

   if ((attr >= STATS_START) && (attr <= STATS_END))
   {
      printf("ERROR - attempt to set  driver value %d that is a stat \n", attr);
      return (FAIL);		/* cant set a stat */
   }
   if (driver_hndl == (DRIVER_HNDL) NULL)
      return (FAIL);
   status = driver_value(driver_hndl, PUT, attr, hndl);
   return ((short) status);
}				/* end Driver_set_value */

#ifdef ASSERT
static int
assert_check_op(op)
   int   op;
{
   /* TODO */
   return (FALSE);
}				/* end assert_check_op */
#endif

static RET_STATUS
driver_check_state(driver_ptr, state)
   struct veh_driv *driver_ptr;
   int   state;
{
/* insert state_debug & ERROR reporting */
   switch (state)
   {
   case DRIVER_SIGN_IN_STATE:
      if (driver_ptr->t_status.emer == 1)
	 return (FAIL);
      break;
   case DRIVER_SUSPENDED_STATE:
      if (driver_ptr->t_status.emer == 1)
	 return (FAIL);
      break;
   default:
      return (FAIL);
   }
   return (SUCCESS);
}				/* end driver_check_state */

static int
driver_state(driver_hndl, op, state, argument)
   DRIVER_HNDL driver_hndl;
   int   op;
   int   state;
   HNDL  argument;
{
   struct veh_driv *driver_ptr;
   unsigned char  tmp;
   FLEET_HNDL fleet_hndl;
   FLEET_DRIVER_VEH_LIST_HNDL fleet_driver_list;
   short ii,i;
   unsigned char *drv_bit_ptr = 0;
   char error_str[80];

#ifdef ASSERT
   if (assert_check_op(op))
      /* TODO ERROR MESSAGE */
      return (-1);
#endif

   driver_ptr = driver_hndl_to_ptr(driver_hndl);
   if (driver_ptr == NULL)
      return(FAIL);

   switch (state)
   {

   case DRIVER_SIGN_IN_STATE:	/* WAS d_signin */
/************************************************************ */
/* DRIVER_SIGN_IN_STATE                                  */
/* STATE_SET                                             */
/*   OP: driver signs in to a vehicle                    */
/*   ARG: veh_hndl                                       */
/*   PRE: vehicle exists                                 */
/*        driver exists                                  */
/*        driver_veh_list exists                         */
/*   POST:  driver vehicle pair added to driver_veh_list */
/*          any of drivers undelievered mail sent        */
/*          fleet driver atr stats updated               */
/*   ERROR: none                                         */
/* STATE_RESET                                           */
/*   OP: driver signs out the vehicle                    */
/*   PRE: veh exists                                     */
/*        driver exists                                  */
/*        driver_veh_list exists                         */
/*        driver can receive messages                    */
/*   POST: state reset                                   */
/*         driver will no longer receive messages        */
/*         driver vehcile pair removed from driver_veh_list */
/*  ERROR: not signed in state                           */
/*        update doc                                     */
/*************************************************************/

      switch (op)
      {
      case STATE_SET:
#ifdef DEBUG_OO
         if (debug_state)
             printf("Dirver sign in class %c\n", driver_ptr->driver_class);
#endif
	 if (driver_check_state(driver_ptr, state) != SUCCESS)
	    return (FAIL);
	 (void) Fleet_get_hndl(BY_NBR, driver_ptr->fleet_nbr, &fleet_hndl);
         if ( fleet_hndl == HNDL_NULL )
           {
             ERROR('X', "", "Problem with driver SIGN_IN");
             return(FAIL);
           }
         
	 fleet_driver_list = (FLEET_DRIVER_VEH_LIST_HNDL) Fleet_get_value(fleet_hndl, FLEET_DRIVER_VEH_LIST);
	 if (Fleet_driver_veh_list_add(fleet_driver_list, (VEH_HNDL) driver_ptr) == FAIL)
             ERROR(driver_ptr->fleet,"","Fleet_driver_veh_list_add");

	 send_undeliv(driver_ptr);
	 return (SUCCESS);

      case STATE_RESET:
	 if (driver_check_state(driver_ptr, state) != SUCCESS)
	    return (FAIL);
	 (void) Fleet_get_hndl(BY_NBR, driver_ptr->fleet_nbr, &fleet_hndl);

	 if (fleet_hndl == NULL)
	    return (FAIL);

	 fleet_driver_list = (FLEET_DRIVER_VEH_LIST_HNDL) Fleet_get_value(fleet_hndl, FLEET_DRIVER_VEH_LIST);
	 if(Fleet_driver_veh_list_remove(fleet_driver_list, (VEH_HNDL) driver_ptr) == FAIL)
         {
#ifdef TOO_MANY_ERRORS
              sprintf(error_str,"remove failure driver %d signout veh %d list %x",
                                   driver_ptr->driver_id,  driver_ptr->veh_nbr, fleet_driver_list);
              ERROR(driver_ptr->fleet,"", error_str);
#endif
         }

	 return (SUCCESS);

      default:
	 return ((int) driver_ptr->t_status.inuse ? SUCCESS : FAIL);
      }				/* end case */
      break;

   case DRIVER_SUSPENDED_STATE:/* WAS is_drv_suspended  & driv_maint */
/***********************************************************/
/* DRIVER_SUSPENDED_DRIVER_STATE                           */
/* GET ONLY: returns status - may call STATE_SET or STATE_RESET  */
/* STATE_SET (not directly callable)                                              */
/*   OP: driver is suspened                                */
/*   ARG: type of suspension (optional)                    */
/*   PRE: driver exists                                    */
/*        Driver_suspend called */
/*        Driver_get_state(SUSPEND) called */
/*   POST: driver susp attributes set                      */
/*         fleet susp stats incremented                    */
/*         driver unabe to take calls as defined by arg    */
/*   ERROR: none                                           */
/* STATE_RESET (Not directly callable)                     */
/*   OP: driver is nolonger suspended                      */
/*   ARG: none                                             */
/*   PRE:  dirver exists                                   */
/*        Driver_suspend called */
/*        Driver_get_state(SUSPEND) called */
/*   POST: driver susp attributes reset                    */
/*         fleet susp stats decremented                    */
/*         driver able to take calls                       */
/*  ERROR: none                                            */
/***********************************************************/

      if (op == GET)
      {
	 if (driver_check_state(driver_ptr, state) != SUCCESS)
	    return (FAIL);
	 if (Fleet_susp_drv_list_find(
	        (FLEET_SUSP_LIST_HNDL)Fleet_get_value((FLEET_HNDL)fleet[driver_ptr->fleet_nbr],FLEET_DRV_SUSP_PASS_LIST),
		driver_ptr->driver_id) != HNDL_NULL )
	   {
	     if (Fleet_susp_drv_list_find(
			 (FLEET_SUSP_LIST_HNDL)Fleet_get_value((FLEET_HNDL)fleet[driver_ptr->fleet_nbr],FLEET_DRV_SUSP_DELIV_LIST),
			 driver_ptr->driver_id) != HNDL_NULL )
	       {
		 if (Fleet_susp_drv_list_find(
    		         (FLEET_SUSP_LIST_HNDL)Fleet_get_value((FLEET_HNDL)fleet[driver_ptr->fleet_nbr],FLEET_DRV_SUSP_TYPEX_LIST),
			  driver_ptr->driver_id) != HNDL_NULL )
		   return(SUCCESS);
	       }
	   }	 
	 return (FAIL);
      }
      printf("ILLEGAL STATE FOR driver_state %d\n", state);
      return (FAIL);

   default:
      printf("ILLEGAL STATE FOR driver_state %d\n", state);
      return (FAIL);
   }				/* end case */
}				/* end driver_state */

RET_STATUS
Driver_set_state(driver_hndl, state, argument)
   DRIVER_HNDL driver_hndl;
   int   state;
   HNDL  argument;
{
  return ((RET_STATUS) driver_state(driver_hndl, STATE_SET, state, argument));
}				/* end Driver_set_state */

RET_STATUS
Driver_reset_state(driver_hndl, state, argument)
   DRIVER_HNDL driver_hndl;
   int   state;
   HNDL  argument;
{
  return ((RET_STATUS) driver_state(driver_hndl, STATE_RESET, state, argument));

}				/* end Driver_set_state */

int
Driver_get_state(driver_hndl, state)
   DRIVER_HNDL driver_hndl;
   int   state;
{
   int   rc;

   rc = driver_state(driver_hndl, GET, state, NULL);

   if (rc == SUCCESS)
      return (TRUE);
   else
      return (FALSE);

}				/* end Driver_set_state */


RET_STATUS
Driver_increment_stats(driver_hndl, attr)
   DRIVER_HNDL driver_hndl;
   int   attr;
{
   HNDL  status;

   if ((attr >= STATS_START) && (attr <= STATS_END))
   {
      status = driver_value(driver_hndl, INCREMENT, attr, NULL);
      if (status != HNDL_ERR)
	 return (SUCCESS);
   }
   return (FAIL);
}				/* end driver_increment_stats */

RET_STATUS
Driver_decrement_stats(driver_hndl, attr)
   DRIVER_HNDL driver_hndl;
   int   attr;
{
   HNDL  status;

   if ((attr >= STATS_START) && (attr <= STATS_END))
   {
      status = driver_value(driver_hndl, DECREMENT, attr, NULL);
      if (status != HNDL_ERR)
	 return (SUCCESS);
   }
   return (FAIL);
}				/* end driver_decrement_stats */

RET_STATUS
Driver_initialize_stats(driver_hndl, attr)
   DRIVER_HNDL driver_hndl;
   int   attr;
{
   HNDL  status;

   if ((attr >= STATS_START) && (attr <= STATS_END))
   {
      status = driver_value(driver_hndl, PUT, attr, 0);
      if (status == HNDL_ERR)
	 return (FAIL);
      return (SUCCESS);
   }
   return (FAIL);
}				/* end driver_decrement_stats */

/*******************************************************************************
* Driv_maint - Update the veh_driv structure in memory.
******************************************************************************/
Driv_maint(rec_num, driver_id, fleet_id)
   int   rec_num;
   int   driver_id;
   char  fleet_id;
{
   struct veh_driv *veh_ptr;	/* pointer to a vehicle record */
   DRIVER_REC *dr_ptr;		/* pointer to a cisam driver recocrd */
   unsigned char *veh_bit_ptr;	/* pointer to bit structure */
   unsigned char *now_bit_ptr;	/* pointer to bit structure */
   unsigned char mask;		/* mask for bits */
   unsigned char *addr;							/* used as a pointer to a bit structure */   
   short found = 0;
   int   fl_nbr;
   register i, j;
   VEH_HNDL veh_hndl;
   FLEET_HNDL fleet_hndl;
   char error_str[80];
   unsigned tmp_susp_pass_call;		/* Temporary suspension flag (passenger) */
   unsigned tmp_susp_del_call;		/* Temporary suspension flag (delivery) */
   unsigned tmp_susp_typex_call;		/* Temporary suspension flag (typex) */
   char         susp_msg[1024];   

#ifdef DEBUG
   if (offset->prattle >= LEVEL3)
   {
      sprintf(trace_str, "driv_maint : rec_num = %d  driver_id = %d fleet = %c \n", rec_num, driver_id, fleet_id);
      TRACE(trace_str);
   }
#endif

   if (Fleet_get_hndl(BY_ID, fleet_id, &fleet_hndl) == FAIL)
   {
      ERROR(fleet_id, DRIVER_FILE, "driv_maint - cannot find fleet id.\n");
      return;
   }
   /* got to be a better way use -  driver list MRB */
   fl_nbr = (short) Fleet_get_value(fleet_hndl, FLEET_NBR);
   Fleet_veh_id_list_each_get(veh_hndl)
   {
      veh_ptr = (struct veh_driv *) veh_hndl;
      if (driver_id == veh_ptr->driver_id)
      {
	 found = 1;
	 break;
      }
   }
   Fleet_veh_id_list_each_end

      if (!found)
      return;
   dr_ptr = (DRIVER_REC *) req_buf;

   dr_ptr->fleet = fleet_id;
   dr_ptr->id = driver_id;
   if (db(DRIVER_FILE_ID, READ_KEYS, &dr_key1, ISEQUAL, 0) < 0)
   {
      if (iserrno == ENOREC)
      {
	 sprintf(error_str, "driv_maint; driver %d removed from db?", driver_id);
	 ERROR(fleet_id, DRIVER_FILE, "driv_maint; driver removed from db?");
      } else
      {
	 sprintf(error_str, "driv_maint(); C-isam error %d", iserrno);
	 ERROR(fleet_id, DRIVER_FILE, error_str);
      }
      return;
   }
   tmp_susp_pass_call = veh_ptr->driver_attr.susp_pass_call;
   tmp_susp_del_call = veh_ptr->driver_attr.susp_del_call;
   tmp_susp_typex_call = veh_ptr->driver_attr.susp_typex_call;
   
   found = 0;
   mask = 0x80;
   addr = (unsigned char*)&veh_ptr->driver_attr;
     
   for(i=0;i<4;i++)
     {
       mask = 0x80;
       for (j=0; j<=7;j++)
	 {
	   if ( j+(i*8) > 31 )
	     break;
	   
	   if(dr_ptr->attributes[j + (i*8)] == YES)
	     {
	       if ( *addr & mask )  /** is attr bit currently set in memory - no action needed **/
		 {
		   mask >>= 1;
		   continue;
		 }
	       else
		 {
		   /** set the bit in the in memory attr structure **/
		   *addr |= mask;					
		   if(veh_ptr->t_status.inuse)
		     ++fleet[fl_nbr]->now_driv_attr[j + (i*8)]; 
		 }
	     }
	   else if (dr_ptr->attributes[j + (i*8)] == NO )
	     {
	       if ( *addr & mask )  /** is attr bit currently set in memory - clear it **/
		 {
		   *addr ^= mask;
		   if(veh_ptr->t_status.inuse)
		     --fleet[fl_nbr]->now_driv_attr[j + (i*8)];
		 }
	     }
	   mask >>= 1;			       
	 }
       ++addr;
     }     

   veh_ptr->driver_attr.susp_pass_call = tmp_susp_pass_call;
   veh_ptr->driver_attr.susp_del_call = tmp_susp_del_call;
   veh_ptr->driver_attr.susp_typex_call = tmp_susp_typex_call;
   
   veh_bit_ptr = (unsigned char *) &veh_ptr->vehicle_seals;	/* get pointer to start
								 * of seals struct */
   now_bit_ptr = (unsigned char *) &veh_ptr->now_seals;	/* get pointer to start
							 * of seals struct */
   *now_bit_ptr = 0;
   for (i = 0; i < 2; i++)
   {				/* do it for two loops */
      mask = 0x80;		/* set the bit mask */
      for (j = 0; j <= 7; j++)
      {				/* read high bit to low */
	 if ((dr_ptr->seals[j + (i * 8)] == YES) && (*veh_bit_ptr & mask))
	 {			/* check if bit should be set */
	    *now_bit_ptr |= mask;
	 }
	 mask >>= 1;		/* shift the mask for next attribute */
      }
      ++veh_bit_ptr;		/* point to the next 8 bits */
      *now_bit_ptr = 0;
      ++now_bit_ptr;		/* point to the next 8 bits */
   }

   veh_ptr->drv_susp_from_date = dr_ptr->susp_from_dt_tm;
   veh_ptr->drv_susp_to_date = dr_ptr->susp_to_dt_tm;
   veh_ptr->drv_susp_d_from_date = dr_ptr->susp_d_from_dt_tm;
   veh_ptr->drv_susp_d_to_date = dr_ptr->susp_d_to_dt_tm;
   veh_ptr->drv_susp_x_from_date = dr_ptr->susp_x_from_dt_tm;
   veh_ptr->drv_susp_x_to_date = dr_ptr->susp_x_to_dt_tm;

   if ( ( veh_ptr->drv_susp_from_date < mads_time ) &&
        ( mads_time < veh_ptr->drv_susp_to_date )   &&
        ( dr_ptr->susp_pass_call == YES ) )
     {
       veh_ptr->driver_attr.susp_pass_call = 1;
       if (Fleet_susp_drv_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_PASS_LIST),
					   veh_ptr->driver_id) == HNDL_NULL)
	 {
	   Fleet_susp_drv_list_add((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_PASS_LIST),
					  (VEH_HNDL)veh_ptr);
	   if ( veh_ptr->t_status.inuse )
	     {
	       strcpy( susp_msg, MDT_MSG_DRIV_SUSP);
	       mk_outb_text( susp_msg );
	       strcpy(susp_msg, MDT_MSG_PASS_TRIP);
	       strcat(susp_msg, dr_ptr->susp_to_date );
	       strcat(susp_msg, " " );
	       strcat(susp_msg, dr_ptr->susp_to_time );
	       add_outb_text( susp_msg );
	       add_outb_text(CR);
	       send_msg_mmp( veh_ptr->mid, TEXT_DISPLAY, veh_ptr ); 
	     }
	 }
     }
   else
     {
       if ( Fleet_susp_drv_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_PASS_LIST),
					    veh_ptr->driver_id) != HNDL_NULL )
	 {
	   Fleet_susp_drv_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_PASS_LIST),
					     (VEH_HNDL)veh_ptr);
	   if ( veh_ptr->t_status.inuse )
	     {
	       strcpy( susp_msg, MDT_MSG_DRIV_SUSP_END);
	       mk_outb_text( susp_msg );
	       add_outb_text( CR );
	       strcpy(susp_msg, MDT_MSG_PASS_TRIP_END);
	       add_outb_text( susp_msg );
	       add_outb_text( CR );
	       send_msg_mmp( veh_ptr->mid, TEXT_DISPLAY, veh_ptr );
	     }
	 }
	       
     }       

   if ( ( veh_ptr->drv_susp_d_from_date < mads_time ) &&
        ( mads_time < veh_ptr->drv_susp_d_to_date )   &&
        ( dr_ptr->susp_del_call == YES ) )       
     {
       veh_ptr->driver_attr.susp_del_call = 1;
       if (Fleet_susp_drv_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_DELIV_LIST),
					   veh_ptr->driver_id) == HNDL_NULL)
	 {
	   Fleet_susp_drv_list_add((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_DELIV_LIST),
				   (VEH_HNDL)veh_ptr);
	   if ( veh_ptr->t_status.inuse )
	     {
	       strcpy( susp_msg, MDT_MSG_DRIV_SUSP);
	       mk_outb_text( susp_msg );
	       strcpy(susp_msg, MDT_MSG_DELIV_TRIP);
	       strcat(susp_msg, dr_ptr->susp_d_to_date );
	       strcat(susp_msg, " " );
	       strcat(susp_msg, dr_ptr->susp_d_to_time );
	       add_outb_text( susp_msg );
	       add_outb_text(CR);
	       send_msg_mmp( veh_ptr->mid, TEXT_DISPLAY, veh_ptr ); 
	     }
	 }
     }
   else
     {
       if ( Fleet_susp_drv_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_DELIV_LIST),
					    veh_ptr->driver_id) != HNDL_NULL )
	 {
	   Fleet_susp_drv_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_DELIV_LIST),
					     (VEH_HNDL)veh_ptr);
	   if ( veh_ptr->t_status.inuse )
	     {
	       strcpy( susp_msg, MDT_MSG_DRIV_SUSP_END);
	       mk_outb_text( susp_msg );
	       add_outb_text( CR );
	       strcpy(susp_msg, MDT_MSG_DELIV_TRIP_END);
	       add_outb_text( susp_msg );
	       add_outb_text( CR );
	       send_msg_mmp( veh_ptr->mid, TEXT_DISPLAY, veh_ptr );
	     }
	 }
	       
     }       

   if ( ( veh_ptr->drv_susp_x_from_date < mads_time ) &&
        ( mads_time < veh_ptr->drv_susp_x_to_date )   &&
        ( dr_ptr->susp_typex_call == YES ) )       
     {
       veh_ptr->driver_attr.susp_typex_call = 1;
       if (Fleet_susp_drv_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_TYPEX_LIST),
					   veh_ptr->driver_id) == HNDL_NULL)
	 {
	   Fleet_susp_drv_list_add((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_TYPEX_LIST),
				   (VEH_HNDL)veh_ptr);
	   if ( veh_ptr->t_status.inuse )
	     {
	       strcpy( susp_msg, MDT_MSG_DRIV_SUSP);
	       mk_outb_text( susp_msg );
	       strcpy(susp_msg, MDT_MSG_TYPEX_TRIP);
	       strcat(susp_msg, dr_ptr->susp_d_to_date );
	       strcat(susp_msg, " " );
	       strcat(susp_msg, dr_ptr->susp_d_to_time );
	       add_outb_text( susp_msg );
	       add_outb_text(CR);
	       send_msg_mmp( veh_ptr->mid, TEXT_DISPLAY, veh_ptr ); 
	     }
	 }	   
     }
   else
     {
       if ( Fleet_susp_drv_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_TYPEX_LIST),
					    veh_ptr->driver_id) != HNDL_NULL )
	 {
	   Fleet_susp_drv_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_TYPEX_LIST),
					     (VEH_HNDL)veh_ptr);
	   if ( veh_ptr->t_status.inuse )
	     {
	       strcpy( susp_msg, MDT_MSG_DRIV_SUSP_END);
	       mk_outb_text( susp_msg );
	       add_outb_text( CR );
	       strcpy(susp_msg, MDT_MSG_TYPEX_TRIP_END);
	       add_outb_text( susp_msg );
	       add_outb_text( CR );
	       send_msg_mmp( veh_ptr->mid, TEXT_DISPLAY, veh_ptr );
	     }
	 }
	       
     }          

}
