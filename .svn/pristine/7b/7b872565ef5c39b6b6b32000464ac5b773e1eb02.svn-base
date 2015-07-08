 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  veh_driv.h; Rel: 6.4.0.0; Get date: 10/29/92 at 13:59:46
 * @(#)  Last delta: 5/12/92 at 12:18:00
 * @(#)  SCCS File: /taxi/sccs/s.veh_driv.h
 *                                                                           *
 *****************************************************************************/
#include "itm_missing.h"
#define RESET_VEH(veh_ptr)	{veh_ptr->t_status.idle = 1; \
				veh_ptr->t_status.accepted = 0; \
				veh_ptr->t_status.offered = 0; \
				veh_ptr->t_status.posted = 0; \
				veh_ptr->t_status.pckup = 0; \
				veh_ptr->t_status.late_meter = 0; \
				veh_ptr->t_status.flag = 0; \
				veh_ptr->call_nbr = 0;	\
				veh_ptr->call_ptr = NULL; \
				veh_ptr->zone_num = 0; \
				veh_ptr->warm_time = 0; \
				veh_ptr->time_info = 0;	 }
/* translation of rf id's to vehicle record pointers */
struct rf_index {
	unsigned long rf_id;		/* hex value of rf id */
	struct veh_driv *vehptr;	/* pointer to a vehicle record */
}; 
/* translation of vehicle numbers to vehicle record pointers */
typedef struct veh_index {
	short veh_num;			/* vehicle number */
	struct veh_driv *vehptr;	/* pointer to a vehicle record */
}VEH_INDEX;
#include "Vehicle_public.h"
#include "Vehicle_private.h"
