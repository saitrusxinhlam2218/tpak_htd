/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Driver.c,v $
*  @(#)  $Revision: 1.8 $
*  @(#)  $Date: 2003/01/28 16:01:24 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Driver.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Driver.c,v 1.8 2003/01/28 16:01:24 jwelch Exp $";

/* Driver.c                                                                */
#include <sys/types.h>		/* temp */
#include <sys/times.h>		/* temp */
#include "mad_error.h"
#include "taxipak.h"
#include "Object.h"
#include "Call_public.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "Driver_public.h"
#include "Driver_private.h"
#include "Driver_db.h"
#include "List_public.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Fleet_lists_public.h"
#include "path.h"
#include "language.h"
#include "taxi_db.h"
#include "Stats_drv_db.h"
#include "mad_ipc.h"
#include "enhance.h"
#include "switch_ext.h"
#include "ex_key.h"

extern time_t mads_time;
extern FLEET *fleet [ ];		/* shared memory fleet structure */
extern struct offsets	*offset;
extern char req_buf [];
extern int veh_used;			/* number of vehicles loaded into memory */
extern int veh_stats_inc;
struct cisam_dr *db_driv_info;

static void driver_write_stats_db();
static void driver_update_counts();



/*******************************************************/
/* RET_STATUS Driver_send_msg(veh_hndl, msg_type)      */
/* OP: send a message to a driver                      */
/* PRE: any msg text on outb.text                      */
/*      dirver exists                                  */
/* POST: formated message sent to driver if signed on  */
/*       msg sent to mail box if not signed in         */
/* ERROR: message transmission                         */
/*******************************************************/

RET_STATUS
Driver_send_msg(driver_hndl, msg_type)
   DRIVER_HNDL driver_hndl;
   int   msg_type;
{
  return(SUCCESS);
     /* nothing yet */
}				/* end Driver_send_msg */

/**************************************************************/
/* RET_STATUS Driver_get_hndl(driver_id, fleet_nbr, hndl_ptr) */
/* OP: get a driver handle from driver_id                     */
/* PRE: none                                                  */
/* POST:  driver_hndl returned if signed on                   */
/* ERROR: FAIL     if no signed on                            */
/**************************************************************/

RET_STATUS
Driver_get_hndl(
   int   driver_id,
   short fleet_nbr,
   DRIVER_HNDL * hndl_ptr)
{
   FLEET_HNDL fleet_hndl;
   FLEET_DRIVER_VEH_LIST_HNDL fleet_driver_veh_list;

   (void) Fleet_get_hndl(BY_NBR, fleet_nbr, &fleet_hndl);
   fleet_driver_veh_list = (FLEET_DRIVER_VEH_LIST_HNDL) Fleet_get_value(fleet_hndl, FLEET_DRIVER_VEH_LIST);
   *hndl_ptr = (DRIVER_HNDL) Fleet_driver_veh_list_find(fleet_driver_veh_list, driver_id);
   if (*hndl_ptr == HNDL_NULL)
      return(FAIL);
   else
      return (SUCCESS);
}				/* end Driver_get_hndl) */


/***********************************************/
/* RET_STATUS Driver_send_status(driver_hndl)  */
/* OP: send driver part of veh/driv status     */
/* PRE: driver exists                          */
/* POST:  driver status send to UI             */
/* ERROR: none                                 */
/***********************************************/

RET_STATUS
Driver_send_status(driver_hndl)		/* WAS veh_driv_status */
   DRIVER_HNDL driver_hndl;
{
  return(SUCCESS);
}				/* end Driver_send_status */

/*********************************************************************************/
/* int Driver_check_attr(driver_hndl, call_hndl)                                 */
/* OP: checks driver attributes to see if ok to distapch                         */
/* PRE: call & driver exist - driver signed on                                   */
/* POST: FALSE returned if driver had rejected call                              */
/*       FALSE returned if driver suspended                                      */
/*       FALSE returned if driver a rookie with priority                         */
/*       FALSE returned if driver_attributes dont match call_driver_attributes   */
/*       otherwise TRUE returned                                                 */
/* ERROR: none                                                                   */
/*********************************************************************************/

int
Driver_check_attr(driver_hndl, call_hndl)	/* WAS * check_att */
   DRIVER_HNDL driver_hndl;
   CALL_HNDL call_hndl;
{
  return(SUCCESS);
   /* presently part of Veh_check_attr */
}				/* end Driver_check_attr */

/*************************************************/
/* int Driver_check_seals(driver_hndl, zone_nbr) */
/* OP: compares zone and drivers for identity    */
/* PRE: driver exists                            */
/* POST:  TRUE returned  if FLEET_SUPPRESS_SEALS */
/*        TRUE retruned  if seals match          */
/*        otherwise FALSE returned               */
/* ERROR: none                                   */
/*************************************************/

int
Driver_check_seals(driver_hndl, zone_nbr)	/* WAS check_seals */
   DRIVER_HNDL driver_hndl;
   int   zone_nbr;
{
  return(SUCCESS);
     /* presently part of Veh_check_seals */
}				/* end Driver_check_seals */


/*****************************************************/
/* RET_STATUS Driver_suspend(driver_hndl, attr_list) */
/* OP: modify  the driver suspend data               */
/* PRE: driver exists                                */
/*      db updated                                   */
/* POST:  driver suspend values set                  */
/* ERROR: db access error                            */
/*****************************************************/

RET_STATUS
Driver_suspend(driver_hndl, attr_list)
   DRIVER_HNDL driver_hndl;
    /* ATTR_LIST */ int attr_list;
{
  return(SUCCESS);
}				/* end Driver_suspend */

/****************************************************************/
/* RET_STATUS Driver_initialize(driver_id, veh_hndl, fleet_hndl)*/
/* OP:  initialize driver from the db                           */
/* PRE: veh exists                                      */
/* POST: veh driver stuff initialized                   */
/* ERROR: invalid id                                    */
/*         update doc                                   */
/********************************************************/

RET_STATUS
Driver_initialize(driver_id, veh_hndl, drv_validate)   /* Was part of chk_driver */
   int   driver_id;
   VEH_HNDL veh_hndl;
   char drv_validate;
{
   int   cc;
   long  cur_time;
   struct cisam_dr *drv;
   struct veh_driv *tmp_veh_ptr;
   FLEET_DRIVER_VEH_LIST_HNDL fleet_driver_veh_list;
   FLEET_HNDL   fleet_hndl;
   struct veh_driv *veh_ptr;

   veh_ptr = (struct veh_driv *) veh_hndl;
   drv = (struct cisam_dr *) req_buf;

   drv->fleet = fleet[veh_ptr->fleet_nbr]->fleet_id;
   drv->id = driver_id;
   if ((cc = db(DRIVER_FILE_ID, READ_KEYS, &dr_key1, ISEQUAL + ISLOCK, 0)) == 0)
   {
      if ((drv->signed_on_dt_tm > 0) && (drv->signed_of_dt_tm == 0))
      {			
	  // Check if already signed on
	 if (Veh_get_hndl(BY_NBR, drv->last_veh_nbr, veh_ptr->fleet_nbr, (VEH_HNDL *) &tmp_veh_ptr) == SUCCESS)
	 {
	    if (drv->id == tmp_veh_ptr->driver_id)
	    {			/* is he signed on in another cab */
	       db(DRIVER_FILE_ID, RELEASE, 0, 0, 0);
	       ERROR(' ', "", "Driver sign on error ");
	       return (FAIL);
	    }
	 }


      }
      // Check if serial nbr on driver card matches value in data record - TSAB only
      if ( (0) && ( drv_validate != '\0' ) )
	  {
	      if ( drv->serial_nbr != drv_validate )
		  {
		      sprintf(error_str, "Driver ID problem: drv %d S1 %c S2 %c",
			      driver_id, drv->serial_nbr, drv_validate);
		      ERROR(' ', "", error_str);
                      db(DRIVER_FILE_ID, RELEASE, 0, 0, 0);                      
                      return (FAIL);
		  }
	  }
   } else
      return (FAIL);

   drv->signed_on_dt_tm = mads_time;
   drv->signed_of_dt_tm = 0;
   drv->last_veh_nbr = veh_ptr->veh_nbr;

   /* Zero-out all the statistics */
   drv->assigns = drv->calls = drv->flags = drv->bids = drv->posts = drv->cond_posts =
      drv->enroute_posts = drv->short_meters = drv->late_meters = drv->no_accepts = drv->rejects = drv->breaks =
      drv->req_to_talk = drv->messages = drv->callbacks = drv->callouts = drv->no_shows = 0;

   db(DRIVER_FILE_ID, UPDATE_REC, &dr_key1, 0, 0);
   db(DRIVER_FILE_ID, RELEASE, &dr_key1, 0, 0);

   memset(&veh_ptr->driver_attr, '\0', sizeof(struct drv_attributes));

   veh_ptr->drv_susp_from_date = drv->susp_from_dt_tm;
   veh_ptr->drv_susp_to_date = drv->susp_to_dt_tm;
   veh_ptr->drv_susp_d_from_date = drv->susp_d_from_dt_tm;
   veh_ptr->drv_susp_d_to_date = drv->susp_d_to_dt_tm;
   veh_ptr->drv_susp_x_from_date = drv->susp_x_from_dt_tm;
   veh_ptr->drv_susp_x_to_date = drv->susp_x_to_dt_tm;

   if (Fleet_get_hndl(BY_ID, veh_ptr->fleet, &fleet_hndl) == FAIL)
     {
       sprintf(error_str, "Bad fleet id='%c'. Vehicle=%d", veh_ptr->fleet, veh_ptr->veh_nbr);
       ERROR(veh_ptr->fleet, "",error_str);
     }
   if ( ( veh_ptr->drv_susp_from_date < mads_time ) &&
        ( mads_time < veh_ptr->drv_susp_to_date ) &&
        ( drv->susp_pass_call == YES ) )
     {
       veh_ptr->driver_attr.susp_pass_call = 1;
       if (Fleet_susp_drv_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_PASS_LIST),
				    driver_id) == HNDL_NULL)
	 {
	   Fleet_susp_drv_list_add((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_PASS_LIST),
				   (VEH_HNDL)veh_ptr);

	 }
       
     }
   else
     {
       if ( Fleet_susp_drv_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_PASS_LIST),
					    driver_id) != HNDL_NULL )
	 {
	   Fleet_susp_drv_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_PASS_LIST),
					     (VEH_HNDL)veh_ptr);

	 }
	       
     }       

   if ( ( veh_ptr->drv_susp_d_from_date < mads_time ) &&
        ( mads_time < veh_ptr->drv_susp_d_to_date )   &&
        ( drv->susp_del_call == YES ) )
     {
       veh_ptr->driver_attr.susp_del_call = 1;
       if (Fleet_susp_drv_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_DELIV_LIST),
				    driver_id) == HNDL_NULL)
	 {
	   Fleet_susp_drv_list_add((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_DELIV_LIST),
				   (VEH_HNDL)veh_ptr);

	 }
     }
   else
     {
       if ( Fleet_susp_drv_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_DELIV_LIST),
					    driver_id) != HNDL_NULL )
	 {
	   Fleet_susp_drv_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_DELIV_LIST),
					     (VEH_HNDL)veh_ptr);

	 }
	       
     }          

   if ( ( veh_ptr->drv_susp_x_from_date < mads_time ) &&
        ( mads_time < veh_ptr->drv_susp_x_to_date )   &&
        ( drv->susp_typex_call == YES ) )
     {
       veh_ptr->driver_attr.susp_typex_call = 1;
       if (Fleet_susp_drv_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_TYPEX_LIST),
				    driver_id) == HNDL_NULL)
	 {
	   Fleet_susp_drv_list_add((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_TYPEX_LIST),
				   (VEH_HNDL)veh_ptr);

	 }
     }      
   else
     {
       if ( Fleet_susp_drv_list_find((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_TYPEX_LIST),
					    driver_id) != HNDL_NULL )
	 {
	   Fleet_susp_drv_list_remove((FLEET_SUSP_LIST_HNDL)Fleet_get_value(fleet_hndl, FLEET_DRV_SUSP_TYPEX_LIST),
					     (VEH_HNDL)veh_ptr);

	 }
	       
     }       

   veh_ptr->driver_attr.attr1 = (drv->attributes[0] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr2 = (drv->attributes[1] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr3 = (drv->attributes[2] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr4 = (drv->attributes[3] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr5 = (drv->attributes[4] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr6 = (drv->attributes[5] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr7 = (drv->attributes[6] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr8 = (drv->attributes[7] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr9 = (drv->attributes[8] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr10 = (drv->attributes[9] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr11 = (drv->attributes[10] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr12 = (drv->attributes[11] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr13 = (drv->attributes[12] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr14 = (drv->attributes[13] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr15 = (drv->attributes[14] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr16 = (drv->attributes[15] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr17 = (drv->attributes[16] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr18 = (drv->attributes[17] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr19 = (drv->attributes[18] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr20 = (drv->attributes[19] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr21 = (drv->attributes[20] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr22 = (drv->attributes[21] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr23 = (drv->attributes[22] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr24 = (drv->attributes[23] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr25 = (drv->attributes[24] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr26 = (drv->attributes[25] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr27 = (drv->attributes[26] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr28 = (drv->attributes[27] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr29 = (drv->attributes[28] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr30 = (drv->attributes[29] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr31 = (drv->attributes[30] == YES) ? 1 : 0;
   veh_ptr->driver_attr.attr32 = (drv->attributes[31] == YES) ? 1 : 0;   
   veh_ptr->now_seals.s1 = (drv->seals[0] == YES) ? 1 : 0;
   veh_ptr->now_seals.s2 = (drv->seals[1] == YES) ? 1 : 0;
   veh_ptr->now_seals.s3 = (drv->seals[2] == YES) ? 1 : 0;
   veh_ptr->now_seals.s4 = (drv->seals[3] == YES) ? 1 : 0;
   veh_ptr->now_seals.s5 = (drv->seals[4] == YES) ? 1 : 0;
   veh_ptr->now_seals.s6 = (drv->seals[5] == YES) ? 1 : 0;
   veh_ptr->now_seals.s7 = (drv->seals[6] == YES) ? 1 : 0;
   veh_ptr->now_seals.s8 = (drv->seals[7] == YES) ? 1 : 0;
   veh_ptr->now_seals.s9 = (drv->seals[8] == YES) ? 1 : 0;
   veh_ptr->now_seals.s10 = (drv->seals[9] == YES) ? 1 : 0;
   veh_ptr->now_seals.s11 = (drv->seals[10] == YES) ? 1 : 0;
   veh_ptr->now_seals.s12 = (drv->seals[11] == YES) ? 1 : 0;
   veh_ptr->now_seals.s13 = (drv->seals[12] == YES) ? 1 : 0;
   veh_ptr->now_seals.s14 = (drv->seals[13] == YES) ? 1 : 0;
   veh_ptr->now_seals.s15 = (drv->seals[14] == YES) ? 1 : 0;
   veh_ptr->now_seals.s16 = (drv->seals[15] == YES) ? 1 : 0;
   veh_ptr->driver_class = drv->class;
/*    strcpy(veh_ptr->susp_msg, drv->susp_message); */
   return(SUCCESS);
}                             /* end Driver_initialize */

/********************************************************/
/* RET_STATUS Driver_get_info(dr_hndl, attr, value_ptr) */
/* OP:  get an attribute value from the driver object   */
/* PRE: object exists and value_ptr is valid            */
/* POST: value_ptr has value and RET_STATUS returned    */
/* ERROR: invalid value_ptr (FAIL)                      */
/********************************************************/

RET_STATUS
Driver_get_info(
	DRIVER_HNDL		driver_hndl,
	int   			attr,
	HNDL 			*value_ptr )
{
	if (value_ptr == NULL)
		return(FAIL);

	*value_ptr = (HNDL) Driver_get_value(driver_hndl, attr);

	return(SUCCESS);
}

/*****************************************************************************
* driver_write_stats_db - put the number of commands since last check into a cisam file
*****************************************************************************/
static void
driver_write_stats_db(dest,src)
   struct cisam_sd *dest;
   struct veh_driv *src;
{

	dest->fleet = src->fleet;
	dest->drv_id  = src->driver_id;
	dest->veh_nbr = src->veh_nbr;

	get_asc_time(src->stat_start_tm,dest->from_date,dest->from_time);		/* get from time */
	dest->from_dt_tm = src->stat_start_tm;	 	

	get_asc_time(mads_time,dest->to_date,dest->to_time);			/* get current time */
	dest->to_dt_tm = mads_time;	

   dest->calls = src->calls - src->tmp_calls;
   dest->flags = src->flags - src->tmp_flags;
   dest->bids = src->bids;
   dest->posts = src->posts - src->tmp_posts;
   dest->cond_posts = src->cond_posts - src->tmp_cond_posts;
   dest->enroute_posts = src->enroute_posts - src->tmp_enroute_posts;	
   dest->short_meters = src->short_meters - src->tmp_short_meters;	
   dest->late_meters = src->late_meters - src->tmp_late_meters;
   dest->no_accepts = src->no_accepts - src->tmp_no_accepts;
   dest->rejects =  src->rejects - src->tmp_rejects;
   dest->breaks = src->breaks - src->tmp_breaks;
   dest->req_to_talk = src->req_to_talk - src->tmp_req_to_talk;	
   dest->messages = src->messages - src->tmp_messages;	
   dest->callbacks = src->callbacks - src->tmp_callbacks;	
   dest->callouts = src->callouts - src->tmp_callouts;	
   dest->no_shows = src->no_shows - src->tmp_no_shows;	
   dest->emergencies = src->emergencies - src->tmp_emergencies;	
   dest->sign_ins = src->sign_ins - src->tmp_sign_ins;	
   dest->sign_offs = src->sign_offs - src->sign_offs;	
}              /* end driver_write_stats_db */

static void
driver_update_counts(driver_ptr)
   struct veh_driv *driver_ptr;
{

   driver_ptr->stat_start_tm = mads_time;
   driver_ptr->tmp_calls = driver_ptr->calls;
   driver_ptr->tmp_flags = driver_ptr->flags;
   driver_ptr->tmp_bids = driver_ptr->bids;		
   driver_ptr->tmp_posts = driver_ptr->posts;
   driver_ptr->tmp_cond_posts = driver_ptr->cond_posts;	
   driver_ptr->tmp_enroute_posts = driver_ptr->enroute_posts;	
   driver_ptr->tmp_short_meters = driver_ptr->short_meters;	
   driver_ptr->tmp_late_meters = driver_ptr->late_meters;
   driver_ptr->tmp_no_accepts = driver_ptr->no_accepts;
   driver_ptr->tmp_rejects =  driver_ptr->rejects;
   driver_ptr->tmp_breaks = driver_ptr->breaks;
   driver_ptr->tmp_req_to_talk = driver_ptr->req_to_talk;	
   driver_ptr->tmp_messages = driver_ptr->messages;	
   driver_ptr->tmp_callbacks = driver_ptr->callbacks;	
   driver_ptr->tmp_callouts = driver_ptr->callouts;	
   driver_ptr->tmp_no_shows = driver_ptr->no_shows;	
   driver_ptr->tmp_emergencies = driver_ptr->emergencies;	
   driver_ptr->tmp_sign_ins = driver_ptr->sign_ins;	
   driver_ptr->tmp_sign_offs = driver_ptr->sign_offs;	
}                      /* end driver_update_counts */
