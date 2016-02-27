/*  @@(#)  $RCSfile: mdt_2.c,v $
*  @@(#)  $Revision: 2.0 $
*  @@(#)  $Date: 2012/08/28 13:29:51 $
*  @@(#)  $Author: taxiprod $
*  @@(#)  $Source: /usr/users/jwelch/cvs_g7/taxi-g7/lib/MMP/mdt_2.c,v $  */
static char rcsid[] = "$Id: mdt_2.c,v 2.0 2012/08/28 13:29:51 taxiprod Exp $";
/* Based on version 6.10 */

#include <stdio.h>
#include <sys/types.h>
#include <memory.h>
#include <string.h>
#include "taxipak.h"
#include "Object.h"
#include "Call_public.h"
#include "Zone_public.h"
#include "Zone_private.h"
#include "Vehicle_public.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Fleet_lists_public.h"
#include "dispatch_strdefs.h"
#include "dispatch.h"
#include "language.h"
#include "sprintf.h"
#include "mad_error.h"
#include "mdt.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "except.h"
#include "mad_ipc.h"
#include "msg_mmp.h"


#define PENALTY_YES	0
extern 	time_t 	mads_time;				/* system time */
extern 	struct drvr_info driver_info;
extern 	struct fleets *fleet[];		/* shared memory fleet structure */
extern 	short get_short_time();
extern 	char *scratch_ptr;
extern 	int fare_option;

/*****************************************************************************/
/*                                                                           */
/*   <PCKUP> - KDT Meter On Command                                          */
/*                                                                           */
/*   k_pickup () : process KDT PCKUP (pickup) command.                        */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         is_pckup_allowed ()  is pckup usage allowed                       */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_pickup ()          pickup process (dispatch)                    */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

#ifndef EXT_K_PICKUP
k_pickup(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{

#ifdef FOO
	if(!(SIGNED_ON(veh_ptr))) {					/* vehicle is not signed on */
		send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);              /* notify driver that vehicle has not signed in */
      		return;
    	}

   if (is_pckup_allowed (veh_ptr) == FALSE){                          /* pckup usage not allowed */
      send_text_predefined (term_id, NO_PCKUP_ALW, veh_ptr);                /* notify driver that pckup key usage is not allowed */
      return;
    }

   if (veh_ptr->t_status.pckup)
     {
       /* Send "ALREADY ON TRIP" */
       send_text_predefined(term_id, NO_ACPT_ACPT, veh_ptr);
       return;
     }

   if (veh_ptr->t_status.accepted && !veh_ptr->t_status.meter_on)
   {
       send_text_predefined(veh_ptr->mid,NO_CLOSE_ALW,veh_ptr);
       return;
   }

   if (validate_pickup(veh_ptr))					  /* validate for approach tariff */	
   	d_pickup (veh_ptr);                                               /* call pickup process (dispatch) */
#else
/*    Veh_process_key((VEH_HNDL) veh_ptr, VEH_PICKUP_KEY, HNDL_NULL); */ 
#endif

   return;
}
#endif

/*****************************************************************************/
/*                                                                           */
/*   <CLOSE> - KDT Meter Off Command                                         */
/*                                                                           */
/*   k_close () : process KDT CLOSE (call close) command.               */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_call_close ()      close process (dispatch)                */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

#ifndef EXT_K_CLOSE
k_close(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{

#ifdef FOO
   if (!(SIGNED_ON(veh_ptr))){						/* vehicle is not signed on */
      send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);                 /* notify driver that vehicle has not signed in */
      return;
    }

   if (!(BUSY(veh_ptr))){
	if(OFFERED(veh_ptr))
      		send_text_predefined (term_id, MUST_PCKP_FIRST, veh_ptr);               /* notify driver that no call close or */
	else
      		send_text_predefined (term_id, NO_CLS_NO_PKP, veh_ptr);           /*   notify driver that no call close or
                                                                        meter off because no pickup or meter is not on */
      return;
    }

   if ( veh_ptr->t_status.meter_on ){                         		/* close usage not allowed */
      send_text_predefined (term_id, NO_CLOSE_ALW, veh_ptr);                /* notify driver that close key usage is not allowed */
      return;
    }

   d_call_close (veh_ptr);                                           /* call close process (dispatch) */
#else
  /* Veh_process_key((VEH_HNDL) veh_ptr, VEH_CLOSE_KEY, HNDL_NULL); */
#endif

   return;
   }
#endif

/*****************************************************************************/
/*                                                                           */
/*   <NSHOW> - Customer No Show Command                                      */
/*                                                                           */
/*   k_noshow () : process KDT NSHOW (no show) command.                      */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_no_show ()         dispatch nshow process                       */
/*                                                                           */
/*      Data Structure Accessed:                                             */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_noshow(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{

#ifdef FOO
   if (!(SIGNED_ON(veh_ptr))){						/* vehicle is not signed on */
      send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);               /* notify driver that vehicle has not signed in */
      return;
    }

   if (!(ACCEPTED(veh_ptr)))
      {
      send_text_predefined (term_id, NO_NSHW_NO_ACPT, veh_ptr);           /* notify driver that can not nshow */
      return;
      }

   if (BUSY(veh_ptr))
      {
      send_text_predefined (term_id, NO_NSHW_MTR_ON, veh_ptr);            /* notify driver that he can't nshow */
      return;
      }

	if (validate_no_show(veh_ptr))				  /* validate that vehicle is allowed to do no show */
   		d_no_show (veh_ptr);                                              /* hand it to dispatch */
	else
	{
		/* This text should say "DENIED" */
      		send_text_predefined (term_id, NO_EPOST_SYS_ERR, veh_ptr);            /* notify driver that nshow denied */
      		return;
	}
#else
   /*  Veh_process_key((VEH_HNDL) veh_ptr, VEH_NOSHOW_KEY, HNDL_NULL); */
#endif
   }

/*****************************************************************************/
/*                                                                           */
/*   <ACPT> - Accept a Call Command                                          */
/*                                                                           */
/*   k_accept () : process KDT ACPT (accept) command.                        */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_accept ()          accept process (dispatch)                    */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_accept(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{

#ifdef FOO
	if (!(SIGNED_ON(veh_ptr))){						/* vehicle is not signed on */
		send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);               	/* notify driver that vehicle has not signed in */
		return;
	}

	if (veh_ptr->t_status.meter_on)
	{
		
           if (veh_ptr->t_status.accepted && !veh_ptr->t_status.stacked_calls)    /* SMS P.1859 */
           {
		send_text_predefined (term_id, NO_POST_MTR_ON, veh_ptr);  
		return;
           }
		
	}

	if (!(OFFERED(veh_ptr))){
		if (!veh_ptr->t_status.stacked_calls ){                   		/* if he does not have multiple calls */
			send_text_predefined (term_id, NO_ACPT_NO_OFFR, veh_ptr);     	/* driver cannot accept a call, none offered */
			return;
		}
		
	}

	if (veh_ptr->t_status.accepted ){  	                      		/* driver has already accepted a call */
		if (!veh_ptr->t_status.stacked_calls ){                   		/* if he does not have multiple calls */
			send_text_predefined (term_id, NO_ACPT_ACPT, veh_ptr);         /* notify driver he cannot accept another call */
			return;
		}
	}

   	d_accept (veh_ptr);   							/* call accept process (dispatch) */
#else
        /* Veh_process_key((VEH_HNDL) veh_ptr, VEH_ACCEPT_KEY , NULL); */
#endif

}

/*****************************************************************************/
/*                                                                           */
/*   <S-OUT> - Sign Off Command                                              */
/*                                                                           */
/*   k_sign_out () : process KDT S-OUT command.                              */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_sign_out ()        sign out process (dispatch)                  */
/*                                                                           */
/*      Data Structure Accessed:                                             */
/*         vehicle             veh_driv struct (creator: dispatch)           */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_sign_out(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{

#ifdef FOO1
   if (!(SIGNED_ON(veh_ptr))){	/* vehicle is not signed on */
      /* notify driver that vehicle has not signed in */
      send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);
      return;
    }
   if (ACCEPTED(veh_ptr)) {
      /* reuse cpost msg - cant sign out on trip */
      send_text_predefined (term_id, NO_CPOST_ACPT, veh_ptr); 
      return;
      }
   if (BUSY(veh_ptr)) {
      /* reuse cpost msg - cant sign out meter on */
      send_text_predefined (term_id, NO_CPOST_MTR_ON, veh_ptr);
      return;
      }
   send_text_predefined (term_id, SIGN_OUT, veh_ptr); /* BECOMES part of Veh_reset_state(SIGN_IN) */
   deauthorize(veh_ptr);  /*  BECOMES part of Veh_reset_state(SIGN_IN) */

#ifdef FOO
   d_sign_out (veh_ptr);   /* call sign out process (dispatch)*/
#else
   /* Veh_reset_state((VEH_HNDL) veh_ptr, VEH_SIGN_IN_STATE, HNDL_NULL, HNDL_NULL); */
#endif
#else
   /* Veh_process_key((VEH_HNDL) veh_ptr, VEH_SIGN_OFF_KEY , NULL); */
#endif
}    /* k_sign_out */

/*****************************************************************************/
/*                                                                           */
/*   <TALK> - Request to Talk Command                                        */
/*                                                                           */
/*   k_talk () : process KDT TALK (talk) command.                            */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_talk ()            talk process (dispatch)                      */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_talk(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{

#ifdef FOO2
   if (!(SIGNED_ON(veh_ptr))){						/* vehicle is not signed on */
      send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);               /* notify driver that vehicle has not signed in */
      return;
    }

#ifdef FOO
   d_talk (veh_ptr);                                                 /* call talk process (dispatch) */
#else
   /* Veh_set_state((VEH_HNDL) veh_ptr, VEH_TALK_STATE, HNDL_NULL); */
#endif
#else
   /* Veh_process_key((VEH_HNDL) veh_ptr, VEH_TALK_KEY , NULL); */
#endif
   
}

/*****************************************************************************/
/*                                                                           */
/*   <REJCT> - Reject a Call Command                                         */
/*                                                                           */
/*   k_reject () : process KDT REJCT (reject) command.                       */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_reject ()          reject process (dispatch)                    */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_reject(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{

#ifdef FOO
	if (!(SIGNED_ON(veh_ptr))){						/* vehicle is not signed on */
		send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);               /* notify driver that vehicle has not signed in */
		return;
	}

	if (!(OFFERED(veh_ptr))){
		if (!veh_ptr->t_status.stacked_calls ){                   		/* if he does not have multiple calls */
			send_text_predefined (term_id, NO_RJCT_NOT_OFF, veh_ptr);     /* driver he can't reject a call, no calls offered */
			return;
		}
	}

	if (ACCEPTED(veh_ptr)){
		if (!veh_ptr->t_status.stacked_calls ){                   		/* if he does not have multiple calls */
			send_text_predefined (term_id, NO_RJCT_NOT_OFF, veh_ptr);     /* driver he can't reject a call, no calls offered */
			return;
		}
	}

	d_reject (veh_ptr,PENALTY_YES);
#else
        /* Veh_process_key((VEH_HNDL) veh_ptr, VEH_REJECT_KEY, (HNDL) VEH_REJECT_PENALTY); */
#endif
}

/*****************************************************************************/
/*                                                                           */
/*   <FLAG> - Flag Request Command                                           */
/*                                                                           */
/*   k_flag() : process KDT's FLAG (flag request) command.          */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_flag ()            flag process (dispatch)                      */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_flag(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{

#ifdef FOO
   if (!(SIGNED_ON(veh_ptr))){						/* vehicle is not signed on */
      send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);               /* notify driver that vehicle has not signed in */
      return;
    }

   d_flag (veh_ptr);                                                 /* call flag process (dispatch) */
   return;
#else
   /* Veh_process_key((VEH_HNDL) veh_ptr, VEH_FLAG_REQUEST_KEY, NULL); */
#endif
}

/*****************************************************************************/
/*                                                                           */
/*   <C-OUT> - Call Out Customer Command                                     */
/*                                                                           */
/*   k_call_out () : process KDT's C-OUT (call out) command.                 */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_c_out ()           c_out process (dispatch)                     */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_call_out(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{

#ifdef FOO
   if (!(SIGNED_ON(veh_ptr))){						/* vehicle is not signed on */
      send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);               /* notify driver that vehicle has not signed in */
      return;
    }

   if (!(ACCEPTED(veh_ptr)))
      {
      send_text_predefined (term_id, NO_C_OUT, veh_ptr);                  /* notify driver that no call out is available */
      return;
      }

   d_c_out (veh_ptr);                                                /* call c_out process (dispatch) */
#else
   /* Veh_process_key((VEH_HNDL) veh_ptr, VEH_CALL_OUT_KEY, HNDL_NULL); */
#endif
}

/*****************************************************************************/
/*                                                                           */
/*   <BREAK> - Going on Break Command                                        */
/*                                                                           */
/*   k_break () : process KDT's BREAK command.                               */
/*               (BREAK is allowed any time.   It only matters to us         */
/*                if driver is posted.  Driver can not take break when       */
/*                he is offered or accepting or serving a call.)             */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_break ()           break process (dispatch)                     */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_break(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{

   /* Veh_process_key((VEH_HNDL) veh_ptr, VEH_BREAK_KEY, HNDL_NULL); */
}

/*****************************************************************************/
/*                                                                           */
/*   <POST> - Post into a Zone Command                                       */
/*                                                                           */
/*   k_post () : process KDT POST command.                                   */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         is_veh_suspend ()    is vehicle suspended                         */
/*         get_veh_susp_msg ()  get vehicle suspension message from          */
/*                             vehicle record                                */
/*         is_zone_valid ()     is zone number valid                         */
/*         check_seals          is a veh allowed to pickup in a zone         */
/*         d_post ()            post process (dispatch)                      */
/*                                                                           */
/*      Data Structure Accessed:                                             */
/*         vehicle             veh_driv struct (creator: dispatch)           */
/*         fleet               fleet struct (creator: dispatch)              */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         msg[index]               inbound struct (inbound msg from NCP)         */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_post(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{
   short zone_nbr;


#ifndef FOO
   /* Veh_process_key((VEH_HNDL) veh_ptr , VEH_POST_KEY, (HNDL) msg->text); */
#else
   if (veh_ptr->t_status.noshow) {
      send_text_predefined (term_id, NO_SHOW_WAIT, veh_ptr);               /* notify driver that noshow was not approved yet */
      return;
      }

   if (!(SIGNED_ON(veh_ptr))){						/* vehicle is not signed on */
      send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);               /* notify driver that vehicle has not signed in */
      return;
    }

   if (is_suspended(term_id, veh_ptr, EITHER, MSG))
      return;

   if (veh_ptr->t_status.meter_on)
   {
		
	send_text_predefined (term_id, NO_POST_MTR_ON, veh_ptr);     	/* driver cannot accept a call with meter on */
	return;
		
   }

   if (BUSY(veh_ptr)) {
      send_text_predefined (term_id, NO_POST_MTR_ON, veh_ptr);            /* notify driver that he can't post */
      return;
      }

   if (ACCEPTED(veh_ptr)) {
      send_text_predefined (term_id, NO_POST_ACPT, veh_ptr);              /* notify driver that can not post */
      return;
      }

   if (OFFERED(veh_ptr)) {
      send_text_predefined (term_id, NO_POST_OFF, veh_ptr);              /* notify driver that can not post */
      return;
      }

   strcat(msg->text, "\0");
   zone_nbr = atoi(msg->text);                                  /* get zone number from msg */

   if (POSTED(veh_ptr)) 
      {
      if ((mads_time - veh_ptr->time_info) < fleet [veh_ptr->fleet_nbr]->posting_freq) 
        {
           send_text_predefined (term_id, NO_POST_TOO_FRE, veh_ptr);          /* too frequent posting, can't post. */
           return;
        }
       else 
        { 
          if (zone_nbr == veh_ptr->zone_nbr) 
             {
                 if( veh_ptr->t_status.on_break)
                 {
#ifdef FOO
                    d_post (veh_ptr, zone_nbr);
#else
                    /* Veh_set_state((VEH_HNDL) veh_ptr, VEH_POSTED_STATE, (HNDL) zone_nbr; */
#endif
                    return;
                 }
                 else
                 {
                    send_text_predefined(term_id, ALREADY_POST, veh_ptr);
                    return;
                 }
             }
         }
      }

#ifdef FOO
   if (is_zone_valid (veh_ptr, zone_nbr) == FALSE) {
#else
   if (1)
   {
#endif
      send_text_predefined (term_id, INV_ZONE_NO, veh_ptr);     /* notify driver he is not allowed to pickup in the zone */
      return;
   }

#ifdef FOO
   if (check_seals(veh_ptr, zone_nbr) < 0){
#else
   if (!Veh_check_seals((VEH_HNDL) veh_ptr, zone_nbr))
   {
#endif
      send_text_predefined (term_id, NO_PCKUP_NOT_ALW, veh_ptr);     /* notify driver he is not allowed to pickup in the zone */
      return;
    }

   d_post (veh_ptr, zone_nbr);                                       /* call post process (dispatch) */
#endif
}

k_unpost(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{

    /* Veh_process_key((VEH_HNDL) veh_ptr, VEH_UNPOST_KEY , NULL); */
}

/*****************************************************************************/
/*                                                                           */
/*   <CPOST> - Conditional Post into a Zone                                  */
/*                                                                           */
/*   k_cpost () : process KDT CPOST (conditional post) command.              */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         is_veh_suspend ()    is vehicle suspended                         */
/*         get_veh_susp_msg ()  get vehicle suspension message from          */
/*                             vehicle record                                */
/*         is_zone_valid ()     is zone number valid                         */
/*         check_seals          is a veh allowed to pickup in a zone         */
/*         is_fleet_valid ()    is a fleet number valid                      */
/*         d_cpost ()           cpost process (dispatch)                     */
/*                                                                           */
/*      Data Structure Accessed:                                             */
/*         vehicle             veh_driv struct (creator: dispatch)           */
/*         fleet               fleet struct (creator: dispatch)              */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         msg[index]               inbound struct (inbound msg from NCP)         */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_cpost(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{
   short zone_nbr;
   short fleet_nbr;
   struct zones *tmp_zone_ptr;


#ifdef FOO
   if (veh_ptr->t_status.noshow) {
      send_text_predefined (term_id, NO_SHOW_WAIT, veh_ptr);               /* notify driver that noshow was not approved yet */
      return;
      }

   if (!(SIGNED_ON(veh_ptr))){						/* vehicle is not signed on */
      send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);               /* notify driver that vehicle has not signed in */
      return;
    }

   if (is_suspended(term_id, veh_ptr, EITHER, MSG))
      return;

   if (ACCEPTED(veh_ptr)) {
      send_text_predefined (term_id, NO_CPOST_ACPT, veh_ptr);             /* notify driver that can not cpost */
      return;
      }

   if (OFFERED(veh_ptr)) {
      send_text_predefined (term_id, NO_CPOST_OFF, veh_ptr);              /* notify driver that can not post */
      return;
      }

   if (BUSY(veh_ptr)) {
      send_text_predefined (term_id, NO_CPOST_MTR_ON, veh_ptr);           /* notify driver that he can't cpost */
      return;
      }

   						/*  check if cond. posting frequency is within fleetwide-defined interval */
   fleet_nbr = veh_ptr->fleet_nbr;            					                       /* get fleet no */
#ifdef FOO
   if (is_fleet_valid (fleet_nbr) == FALSE)   					                       /* invalid fleet number */
#endif
   							/* too frequent cond. posting, can't cpost */
   							/* cond. posting frequency is not within fleetwide-defined interval */
   if ((mads_time - veh_ptr->c_post_time) < (long) fleet [fleet_nbr]->cond_posting_freq) {
      send_text_predefined (term_id, NO_CPOST_TOO_FRE, veh_ptr);
      return;
      }

   if (!(POSTED(veh_ptr))) {
      send_text_predefined (term_id, NO_CPOST_NO_POST, veh_ptr);             /* notify driver that can not cpost */
      return;
      }

   strcat(msg->text, "\0");
   zone_nbr = atoi (msg->text);                       /* get bid zone number from msg */
#ifdef FOO
   if (is_zone_valid (veh_ptr, zone_nbr) == FALSE){                   /* zone is invalid */
#else
   if (1)
   {
#endif
      send_text_predefined (term_id, INV_ZONE_NO, veh_ptr);                      /* notify driver that zone number is invalid */
      return;
      }

#ifdef FOO
   if (check_seals(veh_ptr, zone_nbr) < 0){		         /* check if allowed to post in the zone */
#else
   if (!Veh_check_seals((VEH_HNDL) veh_ptr, zone_nbr))
   {
#endif
      send_text_predefined (term_id, NO_CPOST_NOT_ALW, veh_ptr);                 /* notify driver he is not allowed to cpost into the zone */
      return;
      }

						/* See if driver's posting time is between stand_time_from and stand_time_to */
   tmp_zone_ptr = fleet[veh_ptr->fleet_nbr]->zone_ptr[zone_nbr];
   if(tmp_zone_ptr->type == ZN_STAND){
	if(tmp_zone_ptr->stand_time_from > 0 || tmp_zone_ptr->stand_time_to >0){
		if(get_short_time(mads_time) < tmp_zone_ptr->stand_time_from ||
		   get_short_time(mads_time) > tmp_zone_ptr->stand_time_to) {
      			send_text_predefined (term_id, STAND_CLOSED, veh_ptr);		          /* cannot post at this stand */
      			return;
		}
	}
   }

   d_cpost (veh_ptr, zone_nbr);                                      /* call cpost process (dispatch) */
#else
   strcat(msg->text, "\0");
   zone_nbr = atoi (msg->text);                       /* get bid zone number from msg */
   /* Veh_process_key((VEH_HNDL) veh_ptr, VEH_CPOST_KEY, (HNDL) zone_nbr); */
#endif
}

/*****************************************************************************/
/*                                                                           */
/*   <INQRY> - Zone Inquiry Command                                          */
/*                                                                           */
/*   k_inquiry () : process KDT's INQRY (inquiry) command.                   */
/*                                                                           */
/*      NOTE:                                                                */
/*            INQRY command is handled Entirely by this routine.  That       */
/*            is, it updates the inquiry time in the vehicle data            */
/*            structure, if applicable, dispatch process will never          */
/*            see it.                                                        */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         is_zone_valid ()     is zone number valid                         */
/*         send_zone_info ()    send zone info to KDT (driver)               */
/*                                                                           */
/*      Data Structure Accessed:                                             */
/*         vehicle             veh_driv struct (creator: dispatch)           */
/*                             (partially updated here)                      */
/*         fleet               fleet struct (creator: dispatch)              */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         msg[index]               inbound msg (inbound struct)                  */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_inquiry(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{
   short zone_nbr, nbr_z_fields, i, j;
   long inq_time;
   short inv_zones=0;
   struct zones *zone_pointer;
   char temp [120];
   char temp1 [120];
   char *rec_sep = {"\\"};                                   /* The KDT's record-separators */
   char *digits = {"0123456789"};                            /* The list of possible digits. */
   extern  char *strpbrk (char*, char*)/* , *strncpy (char*, char*, int) */ ;
   char *ptr, number [10];
   long str_len;
   ZONE_HNDL zone_hndl;


   if (!(SIGNED_ON(veh_ptr))){						/* vehicle is not signed on */
      send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);               /* notify driver that vehicle has not signed in */
      return 0;
    }

   /* check if request is too frequent (against fleetwide-defined interval) */
   if ((inq_time = veh_ptr->inqry_time) != FALSE)                    /* get last inqry time; not a first time request */
      {
      if ((mads_time - inq_time) < (long) fleet [veh_ptr->fleet_nbr]->zone_inqry_freq)
         /* the interval between requests is smaller than fleetwide-defined interval, i.e., too frequent */
         {
         send_text_predefined (term_id, NO_INQ_TOO_FRE, veh_ptr);         /* notify driver: no inq, too frequent */

         return 0;
         }
      }

   /* Eliminate the weird record-separators from the KDT (viz., a /036) */
   ptr = msg->text;
   do
      if ((ptr = strpbrk (ptr, rec_sep)) == NULL)
         break;
      else
         for (j=0; j<=3; j++)
            *(ptr + j) = '_';
   while (ptr < (msg->text + strlen (msg->text)));

   /* get each zone number (up to 5) from inbound message data and put all zone info in a string */
   ptr = msg->text;                                                 /* Point to the start of the input string. */
   inv_zones = 0;
   nbr_z_fields = 0;
   temp [0] = '\0';
   for (j=1; j<=5; j++)
      {
      if ((ptr = strpbrk (ptr, digits)) == NULL)                     /* Point the the start of a number. */
         break;                                                      /* There is none. */
      else
         {
         str_len = strspn (ptr, digits);                             /* How many digits are in the number? */
	 if( str_len > 9 )					    /* don't over run the number buffer */
		str_len = 5;
         strncpy (number, ptr, str_len);                             /* Copy the number to the output string. */
         number [str_len] = '\0';                                    /* A null-terminator is needed for the atoi function. */
         zone_nbr = atoi (number);
         if(zone_nbr <= 0)
            {
            ptr = ptr + str_len;                                 /* Point to the rest of the input string. */
            continue;
            }
         else
            {
#ifdef FOO
            if (is_zone_valid (veh_ptr, zone_nbr) == TRUE)           /* valid zone */
#else
            if (1)
#endif
               {
               nbr_z_fields++;                                             /* There is one (more) field to display */
               zone_pointer = fleet [veh_ptr->fleet_nbr]->zone_ptr [zone_nbr];        /* get zone struct pointer */
               i = sprintf (&temp [strlen (temp)], "%03d %02d/%02d/%02d ", zone_nbr,  /* concatenate zone info into a string */
                                                                           zone_pointer->nbr_calls,
                                                                           zone_pointer->nbr_calls,
                                                                           zone_pointer->nbr_veh);
               if (nbr_z_fields == 2 || nbr_z_fields == 4)                 /* That's all for this line of the mdt */
                  sprintf ((&temp [strlen (temp)]), "%s", CR);
               }
            else
               {
               if (inv_zones == 0)                               /* first invalid zone put header string first */
                  sprintf (&temp1 [0], "%s", INV_ZONE_HDR);
               sprintf (&temp1 [strlen (temp1)], " %03d", zone_nbr);
         
               /* put all invalid zones in a string */
               inv_zones++;                                          /* indicate there is a invalid zone*/
               if (inv_zones == 4 && (strlen(temp1) < MAX_DISPLAY_LENGTH))                                   /* should not be hard coded !! */
                  strcat(temp1,CR);
               }
            }
         }
      ptr = ptr + str_len;                                           /* Point to the rest of the input string. */
      }

   if (inv_zones > 0)                                            /* if there are invalid zones */
      {
      if (nbr_z_fields == 1 || nbr_z_fields == 3)
         strcat(temp,CR);
      strcat (temp, temp1);                                          /* add invalid msg to the end of info display */
      }

   /* send zone info (and invalid zone, if any) to driver */
   veh_ptr->inqry_time = mads_time;
   send_zone_info (term_id, temp, veh_ptr);

   return 0;
}

/*****************************************************************************/
/*                                                                           */
/*   <BID> - Fleetwide Call Offer Response Command                           */
/*                                                                           */
/*   k_bid () : process KDT BID (bid) command.                               */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         Bid_is_bid_offered ()    is a bid call offered                    */
/*         send_text_predefined ()         send text to KDT (driver)         */
/*         d_bid ()             bid process (dispatch)                       */
/*                                                                           */
/*      Data Structure Accessed:                                             */
/*         vehicle             veh_driv struct (creator: dispatch)           */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         msg[mindex]               message					*/
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_bid(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{
   short zone_nbr;
   short fleet_nbr;
   ZONE_HNDL zone_hndl;
   int state;


#ifdef OBSOLETE
   if (is_suspended(term_id, veh_ptr, EITHER, MSG))
      return;

   if (veh_ptr->t_status.noshow) {
      send_text_predefined (term_id, NO_SHOW_WAIT, veh_ptr);               /* notify driver that no show not approved */
      return;
      }

   if (!(SIGNED_ON(veh_ptr))){						/* vehicle is not signed on */
      send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);               /* notify driver that vehicle has not signed in */
      return;
    }

   if (ACCEPTED(veh_ptr)) {
      send_text_predefined (term_id, NO_BID_SRV_CALL, veh_ptr);           /* notify driver that can not bid he is serving a call */
      return;
      }

   if (BUSY(veh_ptr))
      {
      send_text_predefined (term_id, NO_BID_SRV_CALL, veh_ptr);           /* notify driver that can not bid he is serving a call */
      return;
      }

   strcat (msg->text, "\0");
   zone_nbr = atoi (msg->text);                           /* get bid zone number from msg */
   fleet_nbr = veh_ptr->fleet_nbr;
/*   zone_hndl = Zone_get_hndl(zone_nbr, fleet_nbr); */
   if (zone_hndl == HNDL_ERR)
   {
      send_text_predefined (term_id, INV_ZONE_NO, veh_ptr);               /* notify driver that no bids offered */
      return;
   }
   state = Zone_get_state(zone_hndl, ZONE_BID_STATE);
   if(state == FALSE)
      {
      send_text_predefined (term_id, NO_BID_OFFR, veh_ptr);               /* notify driver that no bids offered */
      return;
      }

   if (veh_ptr->t_status.bid == 0)                                      /* only one per thank you */
#ifdef FOO
      d_bid (veh_ptr, zone_nbr);                                        /* call bid process (dispatch) */
#else
      /* Veh_set_state((VEH_HNDL) veh_ptr, VEH_BID_STATE, (HNDL) zone_nbr); */
#endif
#else
      strcat (msg->text, "\0");
      zone_nbr = atoi (msg->text);                           /* get bid zone number from msg */
      /* Veh_process_key((VEH_HNDL) veh_ptr, VEH_BID_KEY, (HNDL) zone_nbr); */
#endif
}

/*****************************************************************************/
/*                                                                           */
/*   <S-IN> - Sign In Command                                                */
/*                                                                           */
/*   k_sign_in () : process KDT S-IN (sign in) command.                      */
/*                 (sign-in is allowed for suspended driver/vehicle)         */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         is_veh_suspend ()    is vehicle suspended                         */
/*         chk_driver ()        check status of a driver                     */
/*         get_veh_susp_msg ()  get vehicle suspension message from          */
/*                             vehicle record                                */
/*         d_sign_in ()         sign in process (dispatch)                   */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         msg               inbound text field from the message             */
/*        veh_ptr             pminter to a vehicle data structure            */
/*                                (veh_driv struct)                          */
/*         driver_info         drvr_info struct (retrived driver info        */
/*                                from record by chk_driver ())              */
/*         term_id         terminal id (with \0 at the                       */
/*                                                                           */
/*****************************************************************************/

k_sign_in(msg, veh_ptr, term_id)
char msg[];
struct veh_driv *veh_ptr;
char	*term_id;
{
   int drv_id;
#ifdef FOO
   long ret_val;
#else
   FLEET_HNDL fleet_hndl;
   FLEET_DRIVER_VEH_LIST_HNDL fleet_driver_veh_list;
   VEH_HNDL tmp_veh_hndl;
#endif
   char * get_outb_txt();


   strcat (msg, "\0");
   drv_id = atoi (msg);     /* get driver's ID (drv_id) from inbound msg */


   veh_ptr->t_status.mdt_working = 1;
   /* Veh_process_key((VEH_HNDL) veh_ptr, VEH_SIGN_IN_KEY, (HNDL) drv_id); */
}

/*****************************************************************************/
/*                                                                           */
/*   <MSG> - Message to Dispatcher Command                                   */
/*                                                                           */
/*   k_msg () : process KDT MSG (message) command.                           */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         k_talk()             handle request-to-talk                       */
/*         k_break()            handle go-on-break                           */
/*         k_call_out()         handle call-out                              */
/*         k_flag()             handle flag-request                          */
/*         send_text_predefined ()         send text to KDT (driver)         */
/*         d_msg ()             message process (dispatch)                   */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         msg[index]               inbound struct (inbound msg from NCP)    */
/*         term_id         terminal id (with \0 at the end of 4th char)      */
/*                                                                           */
/*****************************************************************************/

k_msg(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{

/* Real MSG handling */

   if (!(SIGNED_ON(veh_ptr))){						/* vehicle is not signed on */
      send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);               /* notify driver that vehicle has not signed in */
      return 0;
    }

/* Real MSG key */
                 
#ifdef FOO
   d_msg(veh_ptr, atoi (msg->text));                              /* call message process (dispatch) */
#else
    /* Veh_set_state((VEH_HNDL) veh_ptr, VEH_MSG_STATE, (HNDL)(atoi (msg->text))); */
#endif
	return 0;
}

/*****************************************************************************/
/*                                                                           */
/*   <INFO> - Vehicle Inquiry Command                                        */
/*                                                                           */
/*   k_info (zone_nbr) : process KDT's INFO (information) command.           */
/*                                                                           */
/*      NOTE:                                                                */
/*            INFO command is handled Entirely by this routine.  That        */
/*            is, it updates the vehicle info request time in the            */
/*            vehicle data structure, if applicable, and dispatch            */
/*            process will never be invoked.                                 */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         is_veh_valid ()      is vehicle ID valid                          */
/*                                                                           */
/*      Data Structure Accessed:                                             */
/*         vehicle             veh_driv struct (creator: dispatch)           */
/*                             (partially updated here)                      */
/*         fleet               fleet struct (creator: dispatch)              */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         msg[index]               inbound struct (inbound msg from NCP)         */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_info(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{
   short zone_nbr;
   char disp [DISP_SIZE];
   short veh_nbr;
   long inf_time;


#ifdef FOO
   if (!(SIGNED_ON(veh_ptr))){						/* vehicle is not signed on */
      send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);               /* notify driver that vehicle has not signed in */
      return;
    }

   if (!(POSTED(veh_ptr))){						/* vehicle is not signed on */
      send_text_predefined (term_id, NOT_POST, veh_ptr);               /* notify driver that vehicle has not signed in */
      return;
    }

   /* check if request is too frequent (against fleetwide-defined interval) */
   if ((inf_time = veh_ptr->veh_info_time) != FALSE)                 /* get last inqry time (not a first time request) */
      {
      /* the interval between requests is smaller than fleetwide-defined interval, i.e., too frequent */
      if ((mads_time - inf_time) < (long) fleet [veh_ptr->fleet_nbr]->veh_inquiry_freq)
         {
         send_text_predefined (term_id, NO_INFO_TOO_FRE, veh_ptr);        /* notify driver:no info, too frequent */
         return;
         }
      }

	send_qp(veh_ptr->zone_nbr, veh_ptr);

   veh_ptr->veh_info_time = mads_time;					 /* update the veh info request time */
#else
    /* Veh_process_key((VEH_HNDL) veh_ptr, VEH_Q_POSITION_KEY, HNDL_NULL); */
#endif
}

/*****************************************************************************/
/*                                                                           */
/*   <EPOST> - Enroute Post into a Zone Command                              */
/*                                                                           */
/*                                                                           */
/*   k_epost () : process KDT's EPOST (enroute post) command.                */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         is_veh_suspend ()    is vehicle suspended                         */
/*         get_veh_susp_msg ()  get vehicle suspension message from          */
/*                             vehicle record                                */
/*         is_zone_valid ()     is zone number valid                         */
/*         check_seals          is a veh allowed to pickup in a zone         */
/*         d_epost ()           epost process (dispatch)                     */
/*                                                                           */
/*      Data Structure Accessed:                                             */
/*         vehicle             veh_driv struct (creator: dispatch)           */
/*         fleet               fleet struct (creator: dispatch)              */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         msg[index]               inbound msg (inbound struct)                  */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_epost(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{
   short zone_nbr;
   long fleet_nbr;
   struct zones *tmp_zone_ptr;


#ifdef FOO
   if (veh_ptr->t_status.noshow)
      {
      send_text_predefined (term_id, NO_SHOW_WAIT, veh_ptr);               /* notify driver that dispatcher did not approve nshow*/
      return;
      }

   if (!(SIGNED_ON(veh_ptr))){						/* vehicle is not signed on */
      send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);               /* notify driver that vehicle has not signed in */
      return;
    }

   if (is_suspended(term_id, veh_ptr, EITHER, MSG))
      return;

   strcat (msg->text, "\0");
   zone_nbr = atoi(msg->text);                                  /* get bid zone number from msg */

   /*********************************************************/
   /* verify:                                               */
   /*      the zone is valid                                */
   /*      driver is allowed to enroute post in the zone    */
   /*      driver is allowed to pickup/post in the zone     */
   /*********************************************************/

   if(zone_nbr == 0){							/* trying to terminate epost */
   	d_epost (veh_ptr, zone_nbr);                                      /* call epost process (dispatch) */
	return;
   }

#ifdef FOO
   if (is_zone_valid (veh_ptr, zone_nbr) == FALSE){                   /* zone is invalid */
#else
   if (1)
   {
#endif
      send_text_predefined (term_id, INV_ZONE_NO, veh_ptr);                      /* notify driver that zone number is invalid */
      return;
      }

   fleet_nbr = veh_ptr->fleet_nbr;
   									/* not allowed to enroute post in the zone */
   if (fleet [fleet_nbr]->zone_ptr [zone_nbr]->enr_post_allow == NO) 
   {
      send_text_predefined (term_id, NO_EPOST_NOT_ALW, veh_ptr);       /* notify driver he's not allowed to enroute post in zone */
      return;
   }

#ifdef FOO
   if (check_seals(veh_ptr, zone_nbr) < 0){		          /* not allowed to pick up in the zone */
#else
   if (!Veh_check_seals((VEH_HNDL) veh_ptr, zone_nbr))
   {
#endif
      send_text_predefined (term_id, NO_PCKUP_NOT_ALW, veh_ptr);      /* notify driver he is not allowed to post into the zone */
      return;
      }

						/* See if driver's posting time is between stand_time_from and stand_time_to */
   tmp_zone_ptr = fleet[veh_ptr->fleet_nbr]->zone_ptr[zone_nbr];
   if(tmp_zone_ptr->type == ZN_STAND){
	if(tmp_zone_ptr->stand_time_from > 0 || tmp_zone_ptr->stand_time_to >0){
		if(get_short_time(mads_time)< tmp_zone_ptr->stand_time_from || get_short_time(mads_time) > tmp_zone_ptr->stand_time_to){
      			send_text_predefined (term_id, STAND_CLOSED, veh_ptr);		          /* cannot post at this stand */


      			return;
		}
	}
   }

   d_epost (veh_ptr, zone_nbr);                                      /* call epost process (dispatch) */
#else
    /* Veh_process_key((VEH_HNDL) veh_ptr, VEH_EPOST_KEY, (HNDL) msg->text); */
#endif
}

/*****************************************************************************/
/*                                                                           */
/*   <ETA> - Estimated Time of Arrival Command                               */
/*                                                                           */
/*   k_eta () : process KDT's ETA (Estimated Time of Arrival) command.       */
/*             (Driver can ETA at any time.   If ETA follows a call          */
/*              offer, it implies ACPT, otherwise informational              */
/*              exception message is generated for a supervisor.             */
/*              This routine does not generate the exception.)               */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_eta ()             eta process (dispatch)                       */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         msg[index]               inbound msg (inbound struct)                  */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_eta(msg, veh_ptr, term_id)
MSG_QUEUE_HDR	*msg;
struct veh_driv *veh_ptr;
char	*term_id;
{
   long est_time;


#ifdef FOO
   if (!(SIGNED_ON(veh_ptr))){						/* vehicle is not signed on */
      send_text_predefined (term_id, NOT_SIGN_IN, veh_ptr);               /* notify driver that vehicle has not signed in */
      return;
    }

   strcat (msg->text, "\0");
   est_time = atoi (msg->text);                                  /* get estimated time from msg */
   d_eta (veh_ptr, est_time);                                        /* call eta process (dispatch) */
#else
   strcat (msg->text, "\0");
   est_time = atoi (msg->text);                                  /* get estimated time from msg */
   /* Veh_process_key((VEH_HNDL) veh_ptr, VEH_ETA_KEY, (HNDL) est_time); */
#endif
}

/*****************************************************************************/
/*                                                                           */
/*   <M> - Emergency Command                                                 */
/*                                                                           */
/*   k_emergency () : process KDT's M (emergency) command.                   */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         emerg_ack ()         send emergency ack to KDT (driver)           */
/*         d_emergency ()       emergency process (dispatch)                 */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

k_emergency(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{

	if (veh_ptr->t_status.emer)
		return 0;
	else{

#ifdef FOO
   		emerg_ack (veh_ptr);	                /* emergency ACK */
   		d_emergency (veh_ptr);                  /* call emergency process (dispatch); generate exception */
#else
                /* Veh_set_state((VEH_HNDL) veh_ptr, VEH_EMERGENCY_STATE, HNDL_NULL); */
#endif
	}
	return 0;
}

/*****************************************************************************/
/*                                                                           */
/*     - Actual Meter On Command                                             */
/*                                                                           */
/*   k_meter_on () : process KDT meter-on (flag down) command.               */
/*                  (always allowed as long as signed on)                    */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_pickup ()         meter-on process (dispatch)                 */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

#ifndef EXT_K_METER_ON
k_meter_on(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{

#ifdef FOO
   if(!(SIGNED_ON(veh_ptr)))						/* vehicle is not signed on */
   {
      veh_ptr->t_status.meter_on = 1;
      return;
   }

   if (validate_pickup(veh_ptr))					  /* validate for approach tariff */	
   	d_pickup (veh_ptr);                                          	/* call meter_on process (dispatch) */
   veh_ptr->t_status.meter_on = 1;					/* distinguish from the pickup key */
#else
   /* Veh_process_key((VEH_HNDL) veh_ptr, VEH_METER_ON_KEY, NULL);  */
#endif
}
#endif

/*****************************************************************************/
/*                                                                           */
/*   k_meter_off () : process KDT meter-off (flag up) command.               */
/*                   (always allowed as long as signed on)                   */
/*                                                                           */
/*      ROUTINES Called:                                                     */
/*         send_text_predefined ()         send text to KDT (driver)                    */
/*         d_call_close ()      close process (dispatch)                */
/*                                                                           */
/*      GLOBALS:                                                             */
/*         veh_ptr             pointer to a vehicle data structure           */
/*                                (veh_driv struct)                          */
/*         term_id         terminal id (with \0 at the end of 4th char)  */
/*                                                                           */
/*****************************************************************************/

#ifndef EXT_K_METER_OFF
k_meter_off(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{

#ifdef FOO
   if(!(SIGNED_ON(veh_ptr)))						/* vehicle is not signed on */
   {
      veh_ptr->t_status.meter_on = 0;
      return;
   }

   /* Ignore the return value from                */
   /* validate close. We are only interested      */
   /* in catching calls that do not have a pickup */
   /* status when Approach Tariff is "on".        */

   /* MRB check doc */
   validate_close(veh_ptr);

   d_call_close (veh_ptr);                                           /* call close process (dispatch) */
   veh_ptr->t_status.meter_on = 0;					/* turn off meter */
#else
   /* Veh_process_key((VEH_HNDL) veh_ptr, VEH_METER_OFF_KEY, NULL);  */
#endif
}
#endif

k_mfault(veh_ptr, term_id)
struct veh_driv *veh_ptr;
char	*term_id;
{

#ifdef FOO
	d_mfault(veh_ptr, term_id);
#else
        /* Veh_process_key((VEH_HNDL) veh_ptr, VEH_METER_FAULT_KEY, HNDL_NULL); */
#endif

}
