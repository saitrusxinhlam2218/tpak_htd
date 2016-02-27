/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: mdt_ext.c,v $
*  @(#)  $Revision: 2.0 $
*  @(#)  $Date: 2012/08/28 13:29:51 $
*  @(#)  $Author: taxiprod $
*  @(#)  $Source: /usr/users/jwelch/cvs_g7/taxi-g7/lib/MMP/mdt_ext.c,v $
*
*  Copyright (c) 1992 - Motorola Mobile Data Division, Seattle, WA
*
***********************************************************************/

static char rcsid[] = "$Id: mdt_ext.c,v 2.0 2012/08/28 13:29:51 taxiprod Exp $";

/* G7 UNIQUE */

#include <stdio.h>
#include <sys/types.h>
#include <memory.h>
#include <string.h>
#include "taxipak.h"
#include "Object.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "mad_error.h"
#include "mad_ipc.h"
#include "msg_mmp.h"
#include <time.h>
#include "dispatch_strdefs.h"
#include "mdt.h"
#include "msg_mmp.h"
#include "Driver_public.h"
#include "Fleet_public.h"
#include "Fleet_private.h"

static char str_temp[81];

extern char *scratch_ptr;
extern struct fleets *fleet[FLEET_MAX];
 /* shared memory fleet structure */

#ifdef EXT_AUTHORIZE
authorize(veh_ptr)
   struct veh_driv *veh_ptr;
{
   char  group_ids[10];


   mk_outb_text("");

   if (send_msg_mmp(veh_ptr->mid, VEH_AUTHORIZE_AND_OPEN_PORT, veh_ptr) != 0)
   {
      sprintf(str_temp, "unable to send authorize/open serial port to %s", veh_ptr->mid);
      ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_temp);
   }

   /* Send down fleet mid group (authorize multicast messages) */
   if (veh_ptr->driver_class != DRIVER_CLASS_CLUB_AFFAIRE)
   {
      sprintf(group_ids, "%s%d", FLEET_MID, veh_ptr->fleet_nbr);

      mk_outb_text(&group_ids[2]);	/* Skip first two zeros. */

      if (send_msg_mmp(veh_ptr->mid, ASGN_GROUP1, veh_ptr) != 0)
      {
         sprintf(str_temp, "unable to send group id to %s", veh_ptr->mid);
         ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_temp);
      }
   }
   else
   {
      /* Multiple group MID's *must* be sent in ascending numerical order! */

      /* Since we don't use the leading two zeros here, we must do some fancy overlay work. */
      sprintf(str_temp, "%s%d", FLEET_MID, veh_ptr->fleet_nbr);
      sprintf(group_ids, "%s%d%s", CLUB_MID, veh_ptr->fleet_nbr, &str_temp[2]);

      mk_outb_text(&group_ids[2]);	/* Skip first two zeros. */

      if (send_msg_mmp(veh_ptr->mid, ASGN_GROUP2, veh_ptr) != 0)
      {
         sprintf(str_temp, "unable to send group id's to %s", veh_ptr->mid);
         ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_temp);
      }
   }

}				/* end authorize */
#endif  //  EXT_AUTHORIZE

void
k_close(veh_ptr, term_id)
   struct veh_driv *veh_ptr;
   char *term_id;
{
   char  trace_str[80];

   /* ASSIGNED CALL */
      if (veh_ptr->t_status.meter_on)
      {
          veh_ptr->t_status.meter_on = 0;
          sprintf(trace_str, "meter on problem veh %d", veh_ptr->veh_nbr);
          ERROR(' ',"",trace_str);
      }
   /* FLAG */
   return;
}				/* end k_close */


void
k_pickup(veh_ptr, term_id)
   struct veh_driv *veh_ptr;
   char *term_id;
{
   char  trace_str[80];

   veh_ptr->t_status.meter_working = TRUE;	/* show be db default */
   return;
}				/* end k_pickup */

/*****************************************************************************/
/*                                                                           */
/* send_qp - Send Queue Position Message to MDT                              */
/*                                                                           */
/*****************************************************************************/

RET_STATUS
send_qp(zone_nbr, veh_ptr)
   int   zone_nbr;		/* zone number posted into */
   struct veh_driv *veh_ptr;
{
   short q_position;
   char *dr_desc;		/* Driver class description. */
   char  str_temp[81];


   if (zone_nbr != veh_ptr->zone_nbr)
   {
      sprintf(scratch_ptr, "zn mismatch %d %d", zone_nbr, veh_ptr->zone_nbr);
      ERROR(' ', "", scratch_ptr);
      printf("!!!!!!!!!!!!!!!!!!%s\n", scratch_ptr);
   }
   /* Translate class id char to a string description. */
   switch ('A')
   {

   case DRIVER_CLASS_CLUB_AFFAIRE:
      dr_desc = DR_CLASS_CLUB_AFFAIRE_STR;
      break;

   case DRIVER_CLASS_CLUB_ALL:
      dr_desc = DR_CLASS_CLUB_ALL_STR;
      break;

   case DRIVER_CLASS_INSPECTOR:
      dr_desc = DR_CLASS_INSPECTOR_STR;
      break;

   case DRIVER_CLASS_AEROTAXI:
      dr_desc = DR_CLASS_AEROTAXI_STR;
      break;

   case DRIVER_CLASS_ETOILE:
      dr_desc = DR_CLASS_ETOILE_STR;
      break;

   case DRIVER_CLASS_ROOKIE:
      dr_desc = DR_CLASS_ROOKIE_STR;
      break;

   case DRIVER_CLASS_ORDINAIRE:
      dr_desc = DR_CLASS_ORDINAIRE_STR;
      break;

   default:
      dr_desc = "?";		/* Driver class unknown! */

   }


   /* NOTE:  Data items on qp_string have been reordered for G7 */

   strcpy(str_temp, QUEUE_POSITION_HDR);
   mk_outb_text(str_temp);
   sprintf(str_temp, " %2d %3d %2s", q_position, zone_nbr, dr_desc);
   add_outb_text(str_temp);

   if (send_msg_mmp(veh_ptr->mid, STATUS_DISPLAY, veh_ptr) != 0)
   {
      ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", "unable to send_msg_mmp");
      return (VEH_MESSAGE_NOT_SENT);
   }

   return (SUCCESS);
}				/* end send_qp */

RET_STATUS
can_ext_msg_hndl(msg, veh_ptr, term_id)
   MSG_QUEUE_HDR *msg;
   struct veh_driv *veh_ptr;
   char *term_id;
{
   switch (msg->p_fields[P2])
   {
   default:
      nak_msg_err(msg);
      return (FAIL);		/* unknown MDT key sequence */
   }
}				/* end can_ext_msg_hndl */

void
local_form_ext_hndl(msg, veh_ptr, term_id)
   MSG_QUEUE_HDR *msg;
   struct veh_driv *veh_ptr;
   char *term_id;
{
   switch (msg->p_fields[P2])
   {
   case LF_INVOICE:
      break;
   default:
      nak_msg_err(msg);
      break;
   }				/* end case */
}				/* end local_form_ext_hndl */

nak_ext_by_mtype(id, inb_msg, veh_ptr, pkt_type)
   int id;
   MSG_QUEUE_HDR *inb_msg;
   struct veh_driv *veh_ptr;
   char  pkt_type;
{
   switch (pkt_type)
   {
   case INVOICE_PKT_TYPE:
      reset_pkt_entry(id, veh_ptr->veh_nbr);
      break;
   default:
      if (veh_ptr == NULL)
	 mk_nak_error(inb_msg, "Nak: no veh_ptr");
      else
	 mk_nak_error(inb_msg, "Nak: bad pkt id");
   }
}				/* end nak_ext_by_mtype */

void
k_meter_on(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{
   /* should not happen in G7 but ... */
   k_pickup(veh_ptr, term_id);
   return;
}                              /* end k_meter_on */

void
k_meter_off(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{
   /* should not happen in G7 but ... */
   k_close(veh_ptr, term_id);
   return;
}                             /* end k_meter_off */
