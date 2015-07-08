static char sz__FILE__[]="@(#)kdt_1.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  kdt_1.c; Rel: 6.7.0.0; Get date: 1/22/93 at 10:15:02
 * @(#)  Last delta: 11/30/92 at 14:15:38
 * @(#)  SCCS File: /taxi/sccs/s.kdt_1.c
 *                                                                           *
 *****************************************************************************/

/* based on R3 version 5.14 */

#include <stdio.h>
#include <sys/types.h>
#include <memory.h>
#include <string.h>
#include "dispatch_strdefs.h"
#include "dispatch.h"
#include "mads_types.h"
#include "sprintf.h"
#include "taxi_db.h"
#include "mad_error.h"
#include "kdt.h"
#include "df_maint.h"
#include "ex_key.h"
#include "except.h"
#include "calls.h"
#include "mad_ipc.h"
#include "language.h"
#include "path.h"
#include "Vehicle_private.h"
#include "List_public.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Fleet_lists_public.h"
#include "Driver_db.h"

struct cisam_dr *db_driv_info;
struct cisam_vh *db_veh_info;
extern struct offsets *offset;				/* offsets of structures in shared memory */
extern time_t mads_time;
extern char buf [];
extern struct veh_driv *get_veh_ptr();
extern char req_buf [];
extern int veh_used;					/* number of vehicle used in system at current time */
extern struct fleets *fleet [FLEET_MAX];		/* shared memory fleet structure */
extern char *scratch_ptr;
extern char *last_six();

/*****************************************************************************/
/*                                                                           */
/* Support Routines                                                          */
/*                                                                           */
/* In order to facilitate the likely enhancement of the MADS system, most    */
/* data or file structure specific activities have been segregated into      */
/* support routines.  This kind of modularity ensures ease of maintenance    */
/* and enhancement in the future.                                            */
/*                                                                           */
/* The support routines are:                                                 */
/*                                                                           */
/*      get_veh_id (term_id) -           translate RF ID to vehicle struct   */
/*                                      pointer.                             */
/*      get_veh_ptr (veh_id,fleet_no) -  translate a vehicle number to a     */
/*                                      vehicle structure pointer.           */
/*      chk_driver (drv_id, veh_ptr)  -  verify validity of the driver id.   */
/*      is_veh_suspend (veh_ptr)      -  verify that the vehicle is not      */
/*                                      suspended.                           */
/*      is_zone_valid (veh_ptr,zone_nbr) -  verify zone number validity for  */
/*                                         the specific vehicles fleet.      */
/*      is_pckup_allowed (veh_ptr)    -  can the driver use KDTs <PCKUP>     */
/*                                      button on the call he accepted.      */
/*      is_home_allowed (veh_ptr)     -  is the <HOME> function allowed for  */
/*                                      this vehicle/driver.                 */
/*      is_fleet_valid (fleet_id)     -  verify validity of fleet ID.        */
/*      get_veh_info_disp (veh_ptr, zone_nbr)   -  get, format and return    */
/*                                      vehicle information requested by     */
/*                                      driver.                              */
/*      get_queue_position (veh_ptr)  -  return vehicles post queue          */
/*                                      position.                            */
/*      get_veh_susp_msg (veh_ptr)-  retrieve suspension message to          */
/*                                      be sent to driver.                   */
/*                                                                           */
/*****************************************************************************/


/*****************************************************************************/
/*                                                                           */
/*  get_veh_id - RF ID to Vehicle Pointer Conversion Routine                 */
/*                                                                           */
/*  get_veh_id (term_id): get vehicle ID from terminal ID (term_id) off      */
/*                      a rf_id to veh_id table.                             */
/*                                                                           */
/*     Data Structure Accessed:                                              */
/*        rf_idx             rf_index struct (creator: dispatch)             */
/*                                                                           */
/*      GLOBALS:                                                             */
/*        veh_used           number of vehicles in the system                */
/*                                                                           */
/*     Return:                                                               */
/*        NULL   - Not valid terminal or KDT                                 */
/*        vehptr - pointer to the vehicle data structure                     */
/*                                                                           */
/*****************************************************************************/

struct veh_driv *
get_veh_id(term_id)
char *term_id;
{
   register high, low=0, indx;
   unsigned long rf_id;
   struct veh_driv *veh_ptr;

#ifdef DEBUG
	if(offset->prattle >= LEVEL5){
   		sprintf (trace_str, "ENTERING get_veh_id (term_id=%s)\n", term_id);
   		TRACE (trace_str);
	}
#endif

   rf_id = (unsigned long) strtol(term_id, (char **) 0, 16);
   if ((veh_ptr = (struct veh_driv *) Fleet_veh_id_list_find(rf_id)) == NULL)
     return (NULL);
   else
     return( veh_ptr );
}

/*****************************************************************************/
/*                                                                           */
/*   get_veh_ptr - Vehicle Number to Vehicle Pointer Conversion Routine      */
/*                                                                           */
/*  get_veh_ptr (veh_id, fleet_no): get vehicle pointer for a vehicle ID     */
/*               in a fleet from internal vehicle_ID - vehicle_pointer       */
/*               table.                                                      */
/*               (Potentially, vehicle ID's may be the same for              */
/*                different fleet.  If they are the same then it is in       */
/*                sequential order of the fleet number in this table)        */
/*                                                                           */
/*     Data Structure Accessed:                                              */
/*        veh_idx            veh_index struct (creator: dispatch)            */
/*                                                                           */
/*      GLOBALS:                                                             */
/*        veh_used           number of vehicles in the system                */
/*                                                                           */
/*     Return:                                                               */
/*        NULL   - Not valid vehicle ID                                      */
/*        vehptr - pointer to the vehicle data structure                     */
/*                                                                           */
/*****************************************************************************/

struct veh_driv *get_veh_ptr (veh_id, fleet_nbr)
short veh_id, fleet_nbr;
   {
   int high, low, diff, indx;
   short found = 0;
   struct veh_driv *veh_ptr;
   VEH_HNDL veh_hndl;
   FLEET_HNDL fleet_hndl;
   FLEET_VEH_NBR_LIST_HNDL fleet_veh_nbr_list;

   if (Fleet_get_hndl(BY_NBR, fleet_nbr, &fleet_hndl) == FAIL)
     {
       sprintf(scratch_ptr, "Bad fleet id='%d'. Vehicle=%d", fleet_nbr, veh_id);
       ERROR(' ', VEHICLE_FILE, scratch_ptr);
       return(NULL);
     }
   
   fleet_veh_nbr_list = (FLEET_VEH_NBR_LIST_HNDL) Fleet_get_value(fleet_hndl, FLEET_VEH_NBR_LIST);
   
   if ((veh_ptr = (struct veh_driv *) Fleet_veh_nbr_list_find(fleet_veh_nbr_list, veh_id)) == NULL)
     return ( NULL );
   else
     return (veh_ptr);

   }

/*****************************************************************************/
/*                                                                           */
/*   is_veh_suspend - Vehicle Suspension Verification Routine                */
/*                                                                           */
/*  is_veh_suspend (veh_ptr) : check if a vehicle (represented by  */
/*                            veh_ptr) is suspended for a specific suspension type. */
/*                                                                           */
/*      Data Structure Accessed:                                             */
/*         vehicle             veh_driv struct (creator: dispatch)           */
/*                                                                           */
/*     Return:                                                               */
/*        TRUE  (1)  - suspended                                             */
/*        FALSE (0)  - NOT suspended                                         */
/*                                                                           */
/*****************************************************************************/

static int 
is_veh_suspend (veh_ptr)
struct veh_driv *veh_ptr;
{
	int ret_val = 1;	/* default the value to be returned to TRUE (suspended)*/
	VEH_HNDL  veh_hndl, check_veh_hndl;
	FLEET_HNDL fleet_hndl;
	FLEET_SUSP_LIST_HNDL fleet_susp_list_hndl;
	BOOL susp_pass_flag = FALSE,susp_deliv_flag = FALSE, susp_typex_flag = FALSE;

	veh_hndl = (VEH_HNDL)veh_ptr;
	Fleet_get_hndl( BY_ID, veh_ptr->fleet, &fleet_hndl );

	/* Check PASS suspension list */
	fleet_susp_list_hndl = (FLEET_SUSP_LIST_HNDL)Fleet_get_value( fleet_hndl, FLEET_VEH_SUSP_PASS_LIST );
	Fleet_susp_veh_list_each_get( fleet_susp_list_hndl, check_veh_hndl )
	  {
	    if ( check_veh_hndl == veh_hndl )
	      {
		susp_pass_flag = TRUE;
		break;
	      }
	  } Fleet_susp_veh_list_each_end(fleet_susp_list_hndl)

	/* Check DELIV suspension list */
	fleet_susp_list_hndl = (FLEET_SUSP_LIST_HNDL)Fleet_get_value( fleet_hndl, FLEET_VEH_SUSP_DELIV_LIST );
	Fleet_susp_veh_list_each_get( fleet_susp_list_hndl, check_veh_hndl )
	  {
	    if ( check_veh_hndl == veh_hndl )
	      {
		susp_deliv_flag = TRUE;
		break;
	      }
	  } Fleet_susp_veh_list_each_end(fleet_susp_list_hndl)

	fleet_susp_list_hndl = (FLEET_SUSP_LIST_HNDL)Fleet_get_value( fleet_hndl, FLEET_VEH_SUSP_TYPEX_LIST );
	Fleet_susp_veh_list_each_get( fleet_susp_list_hndl, check_veh_hndl )
	  {
	    if ( check_veh_hndl == veh_hndl )
	      {
		susp_typex_flag = TRUE;
		break;
	      }
	  } Fleet_susp_veh_list_each_end(fleet_susp_list_hndl)

	/** if suspended from all vehicle can't work at all **/
	if ( susp_pass_flag && susp_deliv_flag && susp_typex_flag )
	  return( TRUE );

	return( FALSE );
}


/*****************************************************************************/
/*                                                                           */
/*   is_driv_suspend - Driver Suspension Verification Routine                */
/*                                                                           */
/*  is_driv_suspend (veh_ptr) : check if a driver (represented by            */
/*                            drv_id) is suspended.                          */
/*                                                                           */
/*      Data Structure Accessed:                                             */
/*         vehicle             veh_driv struct (creator: dispatch)           */
/*                                                                           */
/*     Return:                                                               */
/*        TRUE  (1)  - suspended                                             */
/*        FALSE (0)  - NOT suspended                                         */
/*                                                                           */
/*****************************************************************************/

static int 
is_driv_suspend (veh_ptr)
struct veh_driv *veh_ptr;
{
	int ret_val = 1;		/* default the value to be returned to TRUE (suspended)*/

#ifdef DEBUG
	if(offset->prattle >= LEVEL5){
   		sprintf (trace_str, "ENTERING is_driv_suspend (veh_ptr=%x)\n", veh_ptr);
   		TRACE (trace_str);
	}
#endif

	if (((veh_ptr->drv_susp_from_date == 0) || (veh_ptr->drv_susp_to_date == 0)) &&
	   ((veh_ptr->drv_susp_d_from_date == 0) || (veh_ptr->drv_susp_d_to_date == 0)) &&
	   ((veh_ptr->drv_susp_x_from_date == 0) || (veh_ptr->drv_susp_x_to_date == 0))) {
		return (FALSE); 							/* time 0 indicates no suspension */
	}

	if ((mads_time < veh_ptr->drv_susp_from_date) || (mads_time > veh_ptr->drv_susp_to_date)) {
		if ((!veh_ptr->vehicle_attr.susp_pass_call) && (veh_ptr->driver_attr.susp_pass_call)){
			veh_ptr->driver_attr.susp_pass_call = 0;
			fleet [veh_ptr->fleet_nbr]->suspended--;
		}
      		ret_val = FALSE;   							/* future time suspension */
	}
	else if ((!veh_ptr->t_status.veh_susp) && (!veh_ptr->t_status.drv_susp)){
		veh_ptr->t_status.drv_susp = 1;						/* suspend this vehicle */
		fleet [veh_ptr->fleet_nbr]->suspended++;
	}

	if ((mads_time < veh_ptr->drv_susp_d_from_date) || (mads_time > veh_ptr->drv_susp_d_to_date)) {
		if ((!veh_ptr->vehicle_attr.susp_del_call) && (veh_ptr->driver_attr.susp_del_call)){
			veh_ptr->driver_attr.susp_del_call = 0;
			fleet [veh_ptr->fleet_nbr]->suspended--;
		}
      		ret_val = FALSE;   							/* future time suspension */
	}
	else if ((!veh_ptr->t_status.veh_susp_d) && (!veh_ptr->t_status.drv_susp_d)){
		veh_ptr->t_status.drv_susp_d = 1;					/* suspend this vehicle */
		fleet [veh_ptr->fleet_nbr]->d_suspended++;
	}

	if ((mads_time < veh_ptr->drv_susp_x_from_date) || (mads_time > veh_ptr->drv_susp_x_to_date)) {
		if ((!veh_ptr->vehicle_attr.susp_typex_call) && (veh_ptr->driver_attr.susp_typex_call)){
			veh_ptr->driver_attr.susp_typex_call = 0;
			fleet [veh_ptr->fleet_nbr]->suspended--;
		}
      		ret_val = FALSE;   							/* future time suspension */
	}
	else if ((!veh_ptr->t_status.veh_susp_x) && (!veh_ptr->t_status.drv_susp_x)){
		veh_ptr->t_status.drv_susp_x = 1;					/* suspend this vehicle */
		fleet [veh_ptr->fleet_nbr]->x_suspended++;
	}

	if (!veh_ptr->driver_attr.susp_pass_call || !veh_ptr->driver_attr.susp_del_call || !veh_ptr->driver_attr.susp_typex_call)
		ret_val = FALSE;					/* checking for suspension from all three types of calls */

	return(ret_val);
}

/*****************************************************************************/
/*                                                                           */
/*  is_zone_valid - Zone Validity Verification Routine                       */
/*                                                                           */
/*  is_zone_valid (veh_ptr, zone_nbr): check if zone number entered by       */
/*                            by a vehicle (represented by vehicle           */
/*                            pointer) is valid.                             */
/*                                                                           */
/*     Data Structure Accessed:                                              */
/*        vehicle             veh_driv struct (creator: dispatch)            */
/*        fleet               fleet struct (creator: dispatch)               */
/*                                                                           */
/*     Return:                                                               */
/*        TRUE  (1)  - valid                                                 */
/*        FALSE (0)  - NOT valid                                             */
/*                                                                           */
/*****************************************************************************/

int is_zone_valid (veh_ptr, zone_nbr)
struct veh_driv *veh_ptr;
short zone_nbr;
   {
   int result;

#ifdef DEBUG
	if(offset->prattle >= LEVEL5){
   		sprintf (trace_str, "ENTERING is_zone_valid (veh_ptr=%x, zone_nbr=%d)\n", veh_ptr, zone_nbr);
   		TRACE (trace_str);
	}
#endif

   result = (zone_nbr < ZONE_MAX) && (zone_nbr > 0) && (fleet [veh_ptr->fleet_nbr]->zone_ptr [zone_nbr] != NULL);

#ifdef DEBUG
	if(offset->prattle >= LEVEL5){
   		sprintf (trace_str, "LEAVING is_zone_valid (%d)\n", result);
   		TRACE (trace_str);
	}
#endif

   return (result);
   }


/*****************************************************************************/
/*                                                                           */
/*  is_pckup_allowed - Pickup Verification Routine                           */
/*                                                                           */
/*  is_pckup_allowed (veh_ptr): check if using PCKUP usage is allowed        */
/*                     by a vehicle (represented by vehicle pointer).        */
/*     Data Structure Accessed:                                              */
/*        vehicle             veh_driv struct (creator: dispatch)            */
/*                                                                           */
/*     Return:                                                               */
/*        TRUE  (1)  - PCKUP key usage is allowed                            */
/*        FALSE (0)  - NOT allowed                                           */
/*                                                                           */
/*****************************************************************************/

int is_pckup_allowed (veh_ptr)
struct veh_driv *veh_ptr;
{
#ifdef DEBUG
	if(offset->prattle >= LEVEL5){
   		sprintf (trace_str, "ENTERING is_pckup_allowed (veh_ptr=%x)\n", veh_ptr);
   		TRACE (trace_str);
	}
#endif

	if (veh_ptr->t_status.meter_working == FALSE)                 /* meter NOT WORKING */
		return (TRUE);

        if (veh_ptr->call_ptr == NULL)
             return(TRUE);

	if (veh_ptr->call_ptr->call_type.passenger && fleet [veh_ptr->fleet_nbr]->criteria.met_pass_call)
		return (TRUE);

	if (veh_ptr->call_ptr->call_type.delivery && fleet [veh_ptr->fleet_nbr]->criteria.met_del_call)
		return (TRUE);

	if (veh_ptr->call_ptr->call_type.xtype && fleet [veh_ptr->fleet_nbr]->criteria.met_typex_call)
		return (TRUE);

	return(FALSE);

}

/*****************************************************************************/
/*                                                                           */
/*  is_fleet_valid - Fleet ID Verification Routine                           */
/*                                                                           */
/*  is_fleet_valid (fleet_id) : check if fleet ID is valid.                  */
/*                                                                           */
/*                  valid fleet ID:                                          */
/*                     1. fleet_id must be smaller than FLEET_MAX            */
/*                                  and                                      */
/*                     2. fleet pointer is NOT NULL                          */
/*                                                                           */
/*     Data Structure Accessed:                                              */
/*        fleet               fleet struct (creator: dispatch)               */
/*                                                                           */
/*     Return:                                                               */
/*        TRUE  (1)  - vaild fleet ID                                        */
/*        FALSE (0)  - invalid fleet ID                                      */
/*                                                                           */
/*****************************************************************************/

int is_fleet_valid (fleet_num)
short fleet_num;
   {
   int result;

#ifdef DEBUG
	if(offset->prattle >= LEVEL5){
   		sprintf (trace_str, "ENTERING is_fleet_valid (fleet_num=%d)\n", fleet_num);
   		TRACE (trace_str);
	}
#endif

	if((fleet_num >= 0) && (fleet_num < FLEET_MAX) && (fleet[fleet_num] != NULL))
  		return(TRUE); 
	else 
		return(FALSE);

   }


/*****************************************************************************/
/*                                                                           */
/*  get_veh_susp_msg - Suspension Message Retrieval Routine                  */
/*                                                                           */
/*  get_veh_susp_msg (veh_ptr): get vehicle suspension message reason        */
/*                                                                           */
/*     DataBase Accessed:                                                    */
/*        vehicle                                                            */
/*                                                                           */
/*     GLOBALS:                                                              */
/*                                                                           */
/*     Return:                                                               */
/*        NULL  - could not find vehicle record                              */
/*        vehicle suspension msg                                             */
/*                                                                           */
/*****************************************************************************/

char *get_veh_susp_msg (veh_ptr)
struct veh_driv *veh_ptr;
{
   	int cc;
   	char *veh_susp_message;
   	struct cisam_vh *veh;
   	veh = (struct cisam_vh *) req_buf;
   	db_veh_info = (struct cisam_vh *) req_buf;

#ifdef DEBUG
	if(offset->prattle >= LEVEL5){
   		sprintf (trace_str, "ENTERING get_veh_susp_msg (veh_ptr=%x)\n", veh_ptr);
   		TRACE (trace_str);
	}
#endif

   	veh->fleet = fleet [veh_ptr->fleet_nbr]->fleet_id;                         /* get vehicle record (veh_susp_message) */
   	veh->nbr = veh_ptr->veh_nbr;
   	if ((cc = db (VEHICLE_FILE_ID, READ_KEYS, &veh_key2, ISEQUAL, 0)) < 0) {
      		ERROR (fleet [veh_ptr->fleet_nbr]->fleet_id, VEHICLE_FILE, "Cannot find vehicle in C-ISAM data base")
      		return(NULL);
      	}
   	else {
      		veh_susp_message = db_veh_info->susp_message;
      		return (veh_susp_message);
	}
}

/*****************************************************************************/
/*                                                                           */
/*  get_drv_susp_msg - Suspension Message Retrieval Routine                  */
/*                                                                           */
/*  get_drv_susp_msg (veh_ptr, drv_id): get driver suspension message        */
/*                                     (reason).                             */
/*                                                                           */
/*     DataBase Accessed:                                                    */
/*        driver                                                             */
/*                                                                           */
/*     GLOBALS:                                                              */
/*                                                                           */
/*     Return:                                                               */
/*        NULL  - could not find driver record                               */
/*        driver suspension msg                                              */
/*                                                                           */
/*****************************************************************************/
char *get_drv_susp_msg (veh_ptr, drv_id)
struct veh_driv *veh_ptr;
int drv_id;
{
   	int cc;
   	char *drv_susp_message;
   	struct cisam_dr *drv;
   	drv = (struct cisam_dr *) req_buf;
   	db_driv_info = (struct cisam_dr *) req_buf;

#ifdef DEBUG
	if(offset->prattle >= LEVEL5){
   		sprintf (trace_str, "ENTERING get_drv_susp_msg (veh_ptr=%x, drv_id=%d)\n", veh_ptr, drv_id);
   		TRACE (trace_str);
	}
#endif

   	drv->fleet = fleet [veh_ptr->fleet_nbr]->fleet_id;                                /* get driver record (drv_susp_message) */
   	drv->id = drv_id;
   	if ((cc = db (DRIVER_FILE_ID, READ_KEYS, &dr_key1, ISEQUAL, 0)) != 0) {
      		ERROR (fleet [veh_ptr->fleet_nbr]->fleet_id, DRIVER_FILE, "Cannot find driver in C-ISAM data base")
      		return(NULL);
      	}
   	else {
      		drv_susp_message = db_driv_info->susp_message;
         	return (drv_susp_message);
	}
}


