/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: mdt_5.c,v $
*  @(#)  $Revision: 2.0 $
*  @(#)  $Date: 2012/08/28 13:29:51 $
*  @(#)  $Author: taxiprod $
*  @(#)  $Source: /usr/users/jwelch/cvs_g7/taxi-g7/lib/MMP/mdt_5.c,v $
*
*  Copyright (c) 1992 - Motorola Mobile Data Division, Seattle, WA
*
***********************************************************************/

static char rcsid[] = "$Id: mdt_5.c,v 2.0 2012/08/28 13:29:51 taxiprod Exp $";

/* Based on version 6.4 */
/* G7 UNIQUE */

/**************************************************************************/
/* trip offer - send_offer()                                              */
/* trip details - send_assign()                                           */
/* associated support routines                                            */
/**************************************************************************/

/* Make sure location constants are defined.*/
/* Create a compile error if they           */
/*   are not defined or both are defined.   */

#ifdef SCANDINAVIA
#ifdef NAMERICA
ERROR:"Both compile time constants SCANDINAVIA and NAMERICA are defined."
#endif
#else
#ifdef NAMERICA
#else
ERROR:"Compile time constant SCANDINAVIA or NAMERICA undefined."
#endif
#endif

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <langinfo.h>
#include <setjmp.h>

#include "taxipak.h"
#include "Object.h"
#include "Call_lists_public.h"
#include "Call_public.h"
#include "Call_private.h"
#include "Call_db.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Zone_public.h"
#include "Zone_private.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"

#include "dispatch_strdefs.h"
#include "path.h"
#include "sprintf.h"
#include "mad_error.h"
 /* #include "taxi_db.h" *//* for acct support */
#include "dispatch.h"
#include "language.h"
#include "mdt.h"
#include "timers.h"
#include "writer.h"
#include "except.h"
#include "mad_ipc.h"
#include "msg.h"
#define IS_FRENCH 1
#include "msg_mmp.h"

void  delete_outb_char();
void  send_pkt_id();
void  clr_pkt_id();
void  mk_outb_header();
void  terminate_outb_text();
void  mk_msg_priority();
void  config_outb_msg();

#define ABRIV TRUE
#define NOABRIV FALSE
#define MAP_START  4		/* map field offset in cal_ptr->pickup_pmap */

static char str_temp[81];

extern char *scratch_ptr;
extern char req_buf[];
extern struct fleets *fleet[FLEET_MAX];	/* shared memory fleet structure */
extern struct cisam_ch writer_buf;

static int
add_expected_due_time(due_time, algorithm_delay)
   char *due_time;
   short algorithm_delay;
{
   char  hrs[4];
   char  min[80]; /* was 3 */
   short ii, jj;
   short i_hrs, i_min;
   int   len;

#ifdef FOO
   for (ii = 0; due_time[ii] != TIME_SEPERATOR && ii < 3; ii++)
      hrs[ii] = due_time[ii];
   hrs[ii] = '\0';
   ii++;
   for (jj = 0; jj < 3; ii++, jj++)
      min[jj] = due_time[ii];
   min[jj] = '\0';
   i_hrs = atoi(hrs);
   i_min = atoi(min);

   i_min += algorithm_delay;
#endif
   i_min = algorithm_delay;
#ifdef FOO
   i_hrs = 0;
   if (i_min >= 60)
   {
      i_min -= 60;
      i_hrs++;
   }
   if (i_hrs >= 24)
      i_hrs -= 24;
   sprintf(hrs, "%d%c", i_hrs, TIME_SEPERATOR);
   len = add_outb_text(hrs);
   sprintf(min, "%2.2d", i_min);
#endif
   sprintf(min, "%d %s", i_min, MINUTES);
   len += add_outb_text(min);
}				/* end add_expected_due_time */

/***********************************************************
 * This function was added due to the improper use of is_time_call()
 * TEK 10.02.90
 *************************************************************/
static int
is_time_call(call)
   CALL_REC *call;
/* does this trip require a due time for the assignment */
{
   int   ii;

   for (ii = 0; ii < sizeof(call->extended_type); ii++)
   {

      /* TBDONE USE FIXED OFFSETS */
      if (call->extended_type[ii] == TIMEC)
	 return (TRUE);
      if (call->extended_type[ii] == SUBS)
	 return (TRUE);
#ifdef FOO
      if (call->extended_type[ii] == '\0')	/* no more valid type chars */
	 break;
#endif
   }
   return (FALSE);
}

int
is_account_call(call)
   CALL_REC *call;
/* does this trip have a charge number for the assignment */
{
   int   ii;

   for (ii = 0; ii < sizeof(call->extended_type); ii++)
   {
      if (call->extended_type[ii] == CHARGE)
	 return (TRUE);
#ifdef FOO
      if (call->extended_type[ii] == '\0')	/* no more valid type chars */
	 break;
#endif
   }
   return (FALSE);
}

int
mk_pickup_address(assign_opt, cl_ptr, type, blk_len)
   char  assign_opt;
   CALL_REC *cl_ptr;
   int   type;			/* street only or complete */
   int   blk_len;
/* make up the pickup address portion of the call */
{
   int   len, ii, adjust;
   char  str_name[MAX_DISPLAY_LENGTH + 1];	/* little to big, but... */

   len = 0;
   adjust = 0;

   /* assign_opt essentially ignore for type = STREET_ONLY */
   switch (assign_opt)
   {
   default:
   case S_FORMAT:
      /* LINE 1 ********************************************* LINE */

redo:
      /* PICKUP_PRE_DIR */
      if (len = add_outb_text(cl_ptr->pckup_address.street_pre_dir) != 0)
	 len += add_outb_space();

      /* PICKUP_STR_NAME */
      strcpy(str_name, cl_ptr->pckup_address.street_name);
      str_name[strlen(str_name) - adjust] = '\0';
      if (strlen(str_name));
      {
	 len += add_outb_text(str_name);
	 len += add_outb_space();
      }

      /* STREET TYPE - PCKUP */
      if (add_outb_text(cl_ptr->pckup_address.street_type))
      {
	 len += strlen(cl_ptr->pckup_address.street_type);
      }
      /* PICKUP POST_DIR */
      if (strlen(cl_ptr->pckup_address.street_post_dir))
      {
	 len += add_outb_space();
	 len += add_outb_text(cl_ptr->pckup_address.street_post_dir);
      }
#ifdef EXTRA_FIELDS
      /* INTERSECTION - PCKUP */
      /* may want a blank */
      mk_intr_fields(call_ptr);

#endif

      if (type == STREET_ONLY)
      {
	 if ((len + blk_len) > MAX_DISPLAY_LENGTH)
	 {
	    adjust = len + blk_len - MAX_DISPLAY_LENGTH;
	    while ((len >= 0) && len--)
	       delete_outb_char();	/* start_again */
	    len = 0;
	    goto redo;
	 } else
	    return (len);
      }
      if (len > MAX_DISPLAY_LENGTH)
      {
	 len -= MAX_DISPLAY_LENGTH;
	 while ((len >= 0) && len--)
	    delete_outb_char();
      }
      if (len > 0)
	 add_outb_text(CR);

      len = 0;

      /* LINE 2 ********************************************* LINE */

      /* STREET NUMBER - PICKUP */
      if (cl_ptr->pckup_address.street_nbr)
      {
	 sprintf(str_temp, "%d", cl_ptr->pckup_address.street_nbr);
	 len = add_outb_text(str_temp);
	 len += add_outb_space();
      } else if (strlen(cl_ptr->pckup_address.apt) || strlen(cl_ptr->pckup_pmap))
	 /* || is_time_call(call_ptr)) */
	 len = add_outb_text("      ");

      /* APARTMENT NUMBER - PICKUP */
      if (add_outb_text(cl_ptr->pckup_address.apt))
	 len += add_outb_space();

      /* CITY - PCKUP */
      len += add_outb_text(cl_ptr->pckup_address.city_name);
      len += add_outb_space();

      /* MAP COORDINATES - PICKUP */
      if (strlen(cl_ptr->pckup_pmap))
      {
	 len += add_outb_text(MAP_HDR);
	 len += add_outb_text(cl_ptr->pckup_pmap);
      }
      /* need a CR ???? */
      return (len);

   case H_FORMAT:

      /* LINE 1 ********************************************* LINE */


      /* PICKUP_STR_NAME */
      if (add_outb_text(cl_ptr->pckup_address.street_name));
      {
	 len += add_outb_space();
	 len += strlen(cl_ptr->pckup_address.street_name);
      }

      /* STREET TYPE - PCKUP */
      if (add_outb_text(cl_ptr->pckup_address.street_type))
      {
	 len += strlen(cl_ptr->pckup_address.street_type);
      }
      if (type == STREET_ONLY)
	 return (len);

      /* CITY - PCKUP */
      len += add_outb_space();
      len += add_outb_text(cl_ptr->pckup_address.city_name);

      if (len > MAX_DISPLAY_LENGTH)
      {
	 len -= MAX_DISPLAY_LENGTH;
	 while ((len >= 0) && len--)
	    delete_outb_char();
      }
      if (len > 0)
	 add_outb_text(CR);

      len = 0;

      /* LINE 2 ********************************************* LINE */

      /* STREET NUMBER - PICKUP */
      if (cl_ptr->pckup_address.street_nbr)
      {
	 sprintf(str_temp, "%d", cl_ptr->pckup_address.street_nbr);
	 len = add_outb_text(str_temp);
	 len += add_outb_space();
      } else if (strlen(cl_ptr->pckup_address.apt) || strlen(cl_ptr->pckup_pmap)
		 || is_time_call(cl_ptr))
	 len = add_outb_text("      ");

      /* APARTMENT NUMBER - PICKUP */
      if (add_outb_text(cl_ptr->pckup_address.apt))
	 len += add_outb_space();
      else if (strlen(cl_ptr->pckup_address.apt) || cl_ptr->pckup_address.street_nbr ||
	       is_time_call(cl_ptr))
	 for (ii = 0; ii < sizeof(cl_ptr->pckup_address.apt); ii++)
	    len += add_outb_space();
      /*
       * CITY - PCKUP len += add_outb_text(cl_ptr->pckup_address.city_name);
       * len += add_outb_space();
       */

      /* MAP COORDINATES - PICKUP */
      if (strlen(cl_ptr->pckup_pmap))
      {
	 /* len += add_outb_text(MAP_HDR); */
	 len += add_outb_text(cl_ptr->pckup_pmap);
      } else if (get_outb_line_size() || is_time_call(cl_ptr))
	 for (ii = 0; ii < sizeof(cl_ptr->pckup_pmap); ii++)
	    len += add_outb_space();

      /* TIME DUE */
      if (is_time_call(cl_ptr))
      {
	 len += add_outb_space();
	 len += add_outb_text(TIME_DUE_HDR);
	 len += add_outb_text(cl_ptr->due_time);
      }
      if (len > MAX_DISPLAY_LENGTH)
      {
	 len -= MAX_DISPLAY_LENGTH;
	 while ((len >= 0) && len--)
	    delete_outb_char();
      }
      if (len > 0)
	 add_outb_text(CR);

      return (len);

   case F_FORMAT:

      /* LINE 1 ********************************************* LINE */

      /* STREET NUMBER - PICKUP */
      if (cl_ptr->pckup_address.street_nbr)
      {
	 sprintf(str_temp, "%d", cl_ptr->pckup_address.street_nbr);
	 len = add_outb_text(str_temp);
	 sprintf(str_temp, " %c", cl_ptr->pckup_address.street_nbr_suffix);
	 len += add_outb_text(str_temp);
	 len += add_outb_space();
      } else if (strlen(cl_ptr->pckup_address.apt) || strlen(cl_ptr->pckup_pmap)
		 || is_time_call(cl_ptr))
	 len = add_outb_text("      ");

      /* STREET TYPE - PCKUP */
      if (add_outb_text(cl_ptr->pckup_address.street_type))
      {
	 len += strlen(cl_ptr->pckup_address.street_type);
	 len += add_outb_space();
      }
      /* PICKUP_STR_NAME */
      if (add_outb_text(cl_ptr->pckup_address.street_name));
      {
	 len += add_outb_space();
	 len += strlen(cl_ptr->pckup_address.street_name);
      }

      if (len > MAX_DISPLAY_LENGTH)
      {
	 len -= MAX_DISPLAY_LENGTH;
	 while ((len >= 0) && len--)
	    delete_outb_char();
      }
      if (type == STREET_ONLY)
	 return (len);

      if (len > 0)
	 add_outb_text(CR);

      /* LINE 2 ********************************************* LINE */

      len = 0;
      /* CITY - PCKUP */
      len += add_outb_text(cl_ptr->pckup_address.city_name);

      /* CLIENT TYPE */
      len += add_outb_space();
      if (strlen(cl_ptr->ext.client_type))
	 add_outb_text(cl_ptr->ext.client_type);
      else
	 add_outb_text("NONE");
   }				/* end case */
}				/* end mk_pickup_address */

#ifdef EXTRA_FIELDS
static int
mk_intr_fields(cl_ptr)
   CALL_REC *cl_ptr;
/* make up the street interection */
{
   int   len = 0;

   if (strlen(cl_ptr->intr_str_name))
   {
      len += add_outb_text(X_HDR);
      len += add_outb_text(cl_ptr->intr_str_name);
      len += add_outb_space();
      len += add_outb_text(cl_ptr->intr_str_name);
      len += add_outb_space();
   }
   return (len);
}				/* end mk_intr_fields */
#endif

#ifdef ALTERNATE
int
is_time_call(call_ptr)
   struct calls *call_ptr;
/* does this trip require a due time for the assignment */
{
   if (call_ptr->call_type.time)
      return (TRUE);
   if (call_ptr->call_type.subscription)
      return (TRUE);
   return (FALSE);
}				/* end is_time_call */
#endif

static int
mk_call_type(call_ptr)
   struct calls *call_ptr;
{
   int   len = 0;		/* number of characters added */

   len = add_outb_text(TYPE_HDR);
   len += add_outb_space();
   if (call_ptr->call_type.passenger)
      len += add_outb_text(PASSENGER_TYPE);
   if (call_ptr->call_type.delivery)
      len += add_outb_text(DELIVERY_TYPE);
   if (call_ptr->call_type.xtype)	/* auto book */
      len += add_outb_text(X_TYPE);
   if (call_ptr->call_type.subscription)
      len += add_outb_text(SUBSCRIPTION_TYPE);
   if (is_time_call(call_ptr))
      len += add_outb_text(TIME_CALL_TYPE);
   if (call_ptr->call_type.charge)
      len += add_outb_text(CHARGE_TYPE);
   if (call_ptr->call_type.multiple)
      len += add_outb_text(MULTIPLE_CALL_TYPE);
   return (len);
}				/* end mk_call_type */

static int
mk_attr_fields(max_lines, abbriv, call_ptr, veh_ptr, attr_limit)
   int   max_lines;		/* how may line to allow */
   short abbriv;		/* abbriviation indicator */
   struct calls *call_ptr;
   struct veh_driv *veh_ptr;
   short attr_limit;
/* make the attribute fields */
/* all on one line if abbreviated - two per line otherwise */
/* return the number of characters added - useful for abbriv only */
{
   short jj, field_cnt;
   unsigned char *byte_ptr;
   unsigned char mask;
   int   header_adjust;
   int   len = 0;

   header_adjust = sizeof(ATTRIBUTES_HDR) / 2;

   if (Call_get_value((CALL_HNDL) call_ptr, CALL_VEH_DRIV_ATTR))
   {
#ifdef EXTRA_FIELDS
      char_cnt += add_outb_text(ATTRIBUTES_HDR);
#endif

      field_cnt = 0;
      str_temp[0] = '\0';


      byte_ptr = (unsigned char *) &call_ptr->driver_attributes;
// IBM
//      mask = 0x01;		/* start with high bit */
      mask = 0x80;		/* start with high bit */

      /* DRIVER ATRIBUTES */
      for (jj = 0; jj < ATTR_MAX; jj++)
      {
	 if (*byte_ptr & mask)	/* this attribute is set */
	 {
	    if (abbriv)
	    {
	       if (attr_limit)
		  if (field_cnt >= attr_limit)
		     break;
	       if (fleet[veh_ptr->fleet_nbr]->call_assignment_info == S_FORMAT)
		  sprintf(str_temp, " %s", fleet[veh_ptr->fleet_nbr]->
			  vd_attr_info[jj].drv_attr);
	       else
		  sprintf(str_temp, "%s ", fleet[veh_ptr->fleet_nbr]->
			  vd_attr_info[jj].drv_attr);
	       field_cnt++;
	    } else
	    {
	       if (attr_limit)
		  if (field_cnt >= attr_limit)
		     break;
	       field_cnt++;
	       if (get_outb_line_size() == 0)	/* not first attr line */
		  add_outb_text(" ");
	       strncpy(str_temp,
		    fleet[veh_ptr->fleet_nbr]->vd_attr_info[jj].drv_at_desc,
		       (field_cnt < 3) ? ATTR_FIELD_WIDTH - header_adjust :
		       ATTR_FIELD_WIDTH);
	       if (field_cnt % 2)
		  strcat(str_temp, " ");	/* attribute separator */
	    }
	    len += add_outb_text(str_temp);
	    if (!abbriv)
	       if (field_cnt && !(field_cnt % 2))
	       {
		  add_outb_text(CR);
		  if ((field_cnt % 2) >= max_lines)
		     return len;
	       }
	 }
//	 mask <<= 1;		/* shift right once */
	 mask >>= 1;		/* shift right once */
      }

      byte_ptr = (unsigned char *) &call_ptr->vehicle_attributes;
//      mask = 0x01;		/* start with high bit */
      mask = 0x80;		/* start with high bit */
      if (abbriv)
	 field_cnt = 0;

      /* VEHICLE ATTRIBUTES */
      for (jj = 0; jj < ATTR_MAX; jj++)
      {
	 if (*byte_ptr & mask)	/* this attribute is set */
	 {
	    if (abbriv)
	    {
	       if (attr_limit)
		  if (field_cnt >= attr_limit)
		     break;
	       if (fleet[veh_ptr->fleet_nbr]->call_assignment_info == S_FORMAT)
		  sprintf(str_temp, " %s", fleet[veh_ptr->fleet_nbr]->
			  vd_attr_info[jj].veh_attr);
	       else
		  sprintf(str_temp, "%s ", fleet[veh_ptr->fleet_nbr]->
			  vd_attr_info[jj].veh_attr);
	       field_cnt++;
	    } else
	    {
	       field_cnt++;
	       if (get_outb_line_size() == 0)	/* not first attr line */
		  add_outb_text(" ");
	       strncpy(str_temp,
		    fleet[veh_ptr->fleet_nbr]->vd_attr_info[jj].veh_at_desc,
		       (field_cnt < 3) ? ATTR_FIELD_WIDTH - header_adjust :
		       ATTR_FIELD_WIDTH);
	       if (field_cnt % 2)
		  strcat(str_temp, " ");	/* attribute separator */
	    }
	    len += add_outb_text(str_temp);
	    if (!abbriv)
	       if (field_cnt && !(field_cnt % 2))
	       {
		  add_outb_text(CR);
		  if ((field_cnt % 2) >= max_lines)
		     return len;
	       }
	 }
//	 mask <<= 1;		/* shift right once */
	 mask >>= 1;		/* shift right once */
      }

      if (!abbriv)
	 if (field_cnt && (field_cnt % 2))
	    add_outb_text(CR);
   }
   return (len);
}				/* end mk_attr_fields */

/*****************************************************************************/
/* */
/* send_offer - Call Offer to Terminal Routine                             */
/* */
/*****************************************************************************/

int
send_offer(term_id, call_ptr, veh_ptr)	/* send call offer info */
   char  term_id[];		/* rf id */
   struct calls *call_ptr;	/* call no offered */
   struct veh_driv *veh_ptr;
{
   CALL_REC *cl_ptr;
   unsigned char *byte_ptr;
   char  offer_opt;
   int   len = 0;
   int   line_cnt = 1;
   int   temp_nbr;

#define FIELD_ALG_START2  26

   offer_opt = fleet[veh_ptr->fleet_nbr]->call_offer_info;

   /* LINE  1 ************************************************ LINE */

   /* CALL HEADER */

    if (offer_opt == ZONE_CITY_STREET)
      strcpy(str_temp, "ZONE");
    else
      strcpy(str_temp, CALL_AVAIL_HDR);
    mk_outb_text(str_temp);

   /* ZONE - PICK UP */
   sprintf(str_temp, " %03d", call_ptr->pickup_zone);
   (void) add_outb_text(str_temp);

   switch (offer_opt)
   {
   case ZONE_ONLY:
      break;			/* nothing to do */
   case CALL_ADDRESS:
      if (1)
      {
	 return (-1);
      }
      (void) add_outb_text(CR);
      (void) mk_pickup_address(fleet[veh_ptr->fleet_nbr]->call_assignment_info,
			       cl_ptr,
			       COMPLETE_ADDRESS, 0);
      break;

   case ZONE_CITY_STREET:
      if (1)
         {
            return (-1); 
         }
         (void) add_outb_space();
         len = 9; /* Length of 'ZONE xxx ' */

         /* CITY - PCKUP */
         len += add_outb_text(cl_ptr->pckup_address.city_name);
         len += add_outb_space();

	 if (len >= FIELD_ALG_START2 )
	 {
	    len -= FIELD_ALG_START2;
	    while ((len >= 0) && len--)
	       delete_outb_char();
	    add_outb_space();
	 }
         else
	 if (len < FIELD_ALG_START2)
	 {
	    len = FIELD_ALG_START2 - len;
	    while ((len >= 0) && len--)
		add_outb_space();
	    add_outb_space();
	 }   
         
	 if (is_time_call(cl_ptr))
	 {
	    sprintf(str_temp, "%05s", cl_ptr->due_time);
	 }
	 else
	 {
	    add_outb_space();
            sprintf(str_temp, "%02d", cl_ptr->ext.algorithm_delay);
	    strcat(str_temp, "MN");
	 }
	    len += add_outb_text(str_temp);
         
	    add_outb_text(CR);

	 len = 0;

         /* LINE 2: ******************************************* LINE */

         /* STREET TYPE - PCKUP */
         if (add_outb_text(cl_ptr->pckup_address.street_type))
         {
	    len += strlen(cl_ptr->pckup_address.street_type);
	    len += add_outb_space();
	    if (len != 4)
	      len += add_outb_space();
         }
         /* PICKUP_STR_NAME */
         if (add_outb_text(cl_ptr->pckup_address.street_name));
         {
	    len += add_outb_space();
	    len += strlen(cl_ptr->pckup_address.street_name);
         }

         if (len >= FIELD_CLIENT_START)
	 {
	    len -= FIELD_CLIENT_START;
	    while ((len >0) && (len--))
	      delete_outb_char();
	 }else
	 if (len < FIELD_CLIENT_START)
	 {
	    len = FIELD_CLIENT_START - len;
	    while ((len > 0) && (len--))
	      add_outb_space();
	  }
         add_outb_text("   ");
         sprintf(str_temp, "%5s", cl_ptr->ext.client_type);
         if (add_outb_text(str_temp))
	    len += strlen(cl_ptr->ext.client_type);

         break;

   case ZONE_DESC:
   case ZONE_PMAP:
   default:

      /* LINE 2 ************************************** LINE */

      len = 0;

      /* ZONE DESCRIPTION */
      if (offer_opt != ZONE_BLOCK)
	 if (strlen(fleet[veh_ptr->fleet_nbr]->zone_ptr[call_ptr->
					     pickup_zone]->description) > 0)
	 {
	    sprintf(str_temp, "%s",
		    fleet[veh_ptr->fleet_nbr]->zone_ptr[call_ptr->
						 pickup_zone]->description);
	    (void) add_outb_text(CR);
	    len += add_outb_text(str_temp);
	 }
      if (offer_opt == ZONE_DESC)
	 break;

      (void) add_outb_text(CR);

      if (offer_opt == ZONE_BLOCK)
      {

	 /* LINE 3 ********************************************* LINE */

	 if (1)
	 {
	    return (-1);
	 }
	 len = 0;

	 /* PICKUP_STR_NAME  etc */
	 str_temp[0] = '\0';

	 if (fleet[veh_ptr->fleet_nbr]->call_assignment_info != F_FORMAT)
	 {
	    if (cl_ptr->pckup_address.street_nbr)
	       /* compute now so we can tell how much space needed */
	       sprintf(str_temp, "%d", cl_ptr->pckup_address.street_nbr -
		       cl_ptr->pckup_address.street_nbr % 100);
	    len += mk_pickup_address(S_FORMAT,	/* ignore the fleet setting */
				     cl_ptr,
				     STREET_ONLY,
				     strlen(str_temp) + 1);

	    /* STREET NUMBER MOD 100 - PICKUP */
	    if (cl_ptr->pckup_address.street_nbr)
	    {
	       len += add_outb_space();
	       len += add_outb_text(str_temp);
	    }
	 } else
	 {
	    temp_nbr = cl_ptr->pckup_address.street_nbr;
	    cl_ptr->pckup_address.street_nbr -= (cl_ptr->pckup_address.street_nbr % 100);
	    len += mk_pickup_address(F_FORMAT,	/* ignore the fleet setting */
				     cl_ptr,
				     STREET_ONLY,
				     0);
	    cl_ptr->pckup_address.street_nbr = temp_nbr;
	 }

	 break;
      }
      /* LINE 3 ********************************************* LINE */

      /* CALL TYPE */
      (void) mk_call_type(call_ptr);

      /* LINE 4 ********************************************* LINE */

      /* MAP REFERENCE - PICKUP */
      if (call_ptr->pickup_pmap[0])
      {
	 (void) add_outb_text(CR);
	 (void) add_outb_text(PAGE_HDR);
	 strcpy(str_temp, call_ptr->pickup_pmap);
	 str_temp[MAP_START] = '\0';
	 (void) add_outb_text(str_temp);
	 (void) add_outb_text(",");
	 (void) add_outb_text(BLOCK_HDR);
	 (void) add_outb_text(&call_ptr->pickup_pmap[MAP_START]);
      }
      break;

   }				/* end case */

   /* Set packet id to catch NAK */
   send_pkt_id(OFFER_PKT_TYPE, call_ptr->call_nbr, veh_ptr->veh_nbr);

   if (send_msg_mmp(term_id, OFFER_DISPLAY, veh_ptr))
   {
      sprintf(str_temp, "unable to send offer to %s", veh_ptr->mid);
      ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_temp);
      /* will wait for the no accept timeout */
      /* if we no_accept may recurse if pipe full */
      /* no_accept(call_ptr, 0); *//* 0 = Don't penalize driver */
   }
   /* Clear packet id for next message */
   clr_pkt_id();

   return (0);
}				/* end send_offer */

static void
mk_comment(cl_ptr)
   CALL_REC *cl_ptr;
/* make the gerneal comment field of the assignment message */
{
   char  comment_buf[MAX_COMMENT_LENGTH + 1];
   char *cptr;
   int   len;

   /* if we have more than 32 characters, less than 64 then and */
   /* the first line does not end with a space attempt to avoid word wrap  */

   len = strlen(cl_ptr->general_cmnt);
   if (len > MAX_DISPLAY_LENGTH
       && len < MAX_COMMENT_LENGTH
       && !(isspace(*(cl_ptr->general_cmnt + MAX_DISPLAY_LENGTH - 1))))
   {
      /* copy only the amount of text that can be displayed */
      /* */
      strncpy(comment_buf, cl_ptr->general_cmnt, MAX_COMMENT_LENGTH);
      comment_buf[MAX_COMMENT_LENGTH] = '\0';

      /* search backwards for a space to see if we can split the line	 */
      /* */
      cptr = &comment_buf[MAX_DISPLAY_LENGTH];

      while (!isspace(*cptr) && cptr > comment_buf)
	 --cptr;

      /* check to see if we can move a word from the first line to the second */
      /* if not then just output the comment and don't worry about the split  */
      /* */
      if ((len - (int) (cptr - comment_buf) + 1) <= MAX_DISPLAY_LENGTH)
      {
	 *cptr = '\0';
	 add_outb_text(comment_buf);

	 /* Don't write out the CR if the line is equal to the	 */
	 /* MAX_DISPLAY_LENGTH	it will be done by add_outb_text */
	 /* */
	 if ((int) (cptr - comment_buf) < MAX_DISPLAY_LENGTH)
	    add_outb_text(CR);

	 add_outb_text(++cptr);
	 if (strlen(cptr) < MAX_DISPLAY_LENGTH)
	    add_outb_text(CR);

      } else
      {
	 add_outb_text(comment_buf);
	 add_outb_text(CR);
      }
   } else
   {
      if (len)
      {
	 add_outb_text(cl_ptr->general_cmnt);
	 if (len != MAX_DISPLAY_LENGTH && len != (2 * MAX_DISPLAY_LENGTH))
	    add_outb_text(CR);
      }
   }

}				/* end mk_comment */


static int
get_nbr_mult_calls(nextcall, call_nbr)
   int   nextcall;
   int   call_nbr;
/* find out hwo many calls are grouped together */
/* this is enefficient and should be done at entry time */
{
   CALL_REC *cl_ptr;
   int   nbr_calls = 0;		/* 0 ?? */

   while (nextcall)
   {
      nbr_calls++;
      if (1)
	 /* will record this error later */
	 return (nbr_calls);

      if (cl_ptr->grouped_with == call_nbr)
	 nextcall = 0;
      else
	 nextcall = cl_ptr->grouped_with;
   }
   return (nbr_calls);
}				/* end get_nbr_mult_calls */

/*****************************************************************************/
/* */
/* send_assign - Send Call Assignment Information Terminal                 */
/* */
/*****************************************************************************/

int
send_assign(term_id, call_ptr, veh_ptr)	/* send call assignment info */
   char  term_id[];		/* rf id */
   struct calls *call_ptr;	/* call no offered */
   struct veh_driv *veh_ptr;
{				/* variable length message */

   CALL_REC *cl_ptr;
   struct cisam_ac *ac_ptr;
   struct cisam_ad *ad_ptr;
   short ii;
   int   call_nbr, nextcall, i = 0, comment_length = 0;
   int   c_isam_num;
   int   mult_nbr;
   int   total_mult_nbr;
   int   len = 0;		/* current line length */
   int   len_blanks = 0;
   char  priority = PRIORITY_3;
   char *currency_symbol;
   char  comment_buf[MAX_COMMENT_LENGTH + 1];
   int   status, j = 0;
   char  assign_opt;
   char  buffer[30], temp_buf[BUFSIZ];

   if ((currency_symbol = nl_langinfo(CRNCYSTR)) == NULL)
      currency_symbol = " ";


   if (call_ptr == NULL)
   {				/* Call rec not in memory get it */
      nextcall = call_nbr = veh_ptr->call_nbr;
      c_isam_num = 0;
   } else
   {				/* Call rec in memory procede */
      nextcall = call_nbr = call_ptr->call_nbr;
      c_isam_num = call_ptr->c_isam_num;
   }

   assign_opt = fleet[veh_ptr->fleet_nbr]->call_assignment_info;
   mult_nbr = 0;

   while (nextcall)
   {
      if (1)
      {
	 return (-1);
      }
      if (mult_nbr == 0 && cl_ptr->grouped_with)
      {
	 total_mult_nbr = get_nbr_mult_calls(nextcall, call_nbr);
	 /* do we need to do this again ?? */
      }
      c_isam_num = 0;

      if (cl_ptr->grouped_with == call_nbr)
	 nextcall = 0;
      else
	 nextcall = cl_ptr->grouped_with;



      mk_outb_text("");		/* to initialize it */

      /* LINES 1 & 2 ****************************************** LINE */
      (void) mk_pickup_address(assign_opt, cl_ptr, COMPLETE_ADDRESS, 0);	/* 2 lines */

      switch (assign_opt)

      {
      default:
      case S_FORMAT:
	 /* LINE 3 *********************************************** LINE */

	 /* PASSENGER NAME */
	 add_outb_text(CR);

	 len = 0;
	 if (strlen(cl_ptr->passenger_name))
	    len += add_outb_text(cl_ptr->passenger_name);
	 else
	    for (ii = 0; ii <= sizeof(cl_ptr->passenger_name); ii++)
	    {
	       len += add_outb_space();
	       len_blanks = len;
	    }

	 /* VEHICLE and DRIVER ATTRIBUTES */
	 len += mk_attr_fields(1, ABRIV, call_ptr, veh_ptr, 0);

	 if (len == len_blanks)	/* no attributes or passenger name */
	    while ((len >= 0) && len--)
	       delete_outb_char();	/* back out the blanks */

	 if (len > MAX_DISPLAY_LENGTH)
	 {
	    len -= MAX_DISPLAY_LENGTH;
	    while ((len >= 0) && len--)
	       delete_outb_char();
	 }
	 if (len > 0)
	    add_outb_text(CR);

	 len = 0;

	 /* LINE 4 ********************************************* LINE */

	 /* MULTIPLE CALL INDICATOR */
	 if (cl_ptr->grouped_with)
	 {
	    /* len += add_outb_space(); */
	    mult_nbr++;
	    sprintf(str_temp, "%s%1d %s %1d", MULTIPLE_CALL_S, mult_nbr, OF, total_mult_nbr);
	    len += add_outb_text(str_temp);
	    add_outb_text(CR);
	 }
	 len = 0;

	 /* LINE 5 ********************************************* LINE */

	 len = add_outb_text(DEST_HDR_S);

	 /* DEST PRE_DIR */
	 if (len = add_outb_text(cl_ptr->dest_address.street_pre_dir) != 0)
	    len += add_outb_space();

	 /* STREET NAME - DESTINATION */
	 if (strlen(cl_ptr->dest_address.street_name))
	 {
	    len += add_outb_text(cl_ptr->dest_address.street_name);
	 }
	 /* STREET TYPE - DESTINATION */
	 if (strlen(cl_ptr->dest_address.street_type))
	 {
	    len += add_outb_space();
	    len += add_outb_text(cl_ptr->dest_address.street_type);
	 }
	 /* DEST POST_DIR */
	 if (strlen(cl_ptr->dest_address.street_post_dir))
	 {
	    len += add_outb_space();
	    len += add_outb_text(cl_ptr->dest_address.street_post_dir);
	    len += strlen(cl_ptr->dest_address.street_post_dir);
	 }
	 if (len > MAX_DISPLAY_LENGTH)
	 {
	    len -= MAX_DISPLAY_LENGTH;
	    while ((len >= 0) && len--)
	       delete_outb_char();
	 }
	 if (len > 0)
	    add_outb_text(CR);

	 len = 0;

	 /* LINE 6 ********************************************* LINE */

	 /* STREET NUMBER - DESTINATION */
	 if (cl_ptr->dest_address.street_nbr)
	 {
	    sprintf(str_temp, "%d", cl_ptr->dest_address.street_nbr);
	    len += add_outb_text(str_temp);
	    len += add_outb_space();
	 } else if (strlen(cl_ptr->dest_address.apt) || strlen(cl_ptr->dest_dmap))
	    for (ii = 0; ii < sizeof(cl_ptr->dest_address.street_nbr); ii++)
	       len += add_outb_space();


	 /* APARTMENT NUMBER - DESTINATION */
	 if (add_outb_text(cl_ptr->dest_address.apt))
	 {
	    len += strlen(cl_ptr->dest_address.apt);
	 } else if (strlen(cl_ptr->dest_address.apt) || cl_ptr->dest_address.street_nbr)
	    for (ii = 0; ii < sizeof(cl_ptr->dest_address.apt); ii++)
	       len += add_outb_space();

	 /* CITY - DESTINATION */
	 if (strlen(cl_ptr->dest_address.city_name))
	 {
	    len += add_outb_space();
	    len += add_outb_text(cl_ptr->dest_address.city_name);
	 }
	 /* MAP - DESTINATION */
	 if (strlen(cl_ptr->dest_dmap))
	 {
	    len += add_outb_space();
	    add_outb_text(DEST_DMAP_HDR);
	    len += add_outb_text(cl_ptr->dest_dmap);
	 }
	 if (len > MAX_DISPLAY_LENGTH)
	 {
	    len -= MAX_DISPLAY_LENGTH;
	    while ((len >= 0) && len--)
	       delete_outb_char();
	 }
	 if (len > 0)
	    add_outb_text(CR);

	 len = 0;

	 /* LINES 7 - 8 **************************************** LINE */

	 /* COMMENT - GENERAL */
	 mk_comment(cl_ptr);

	 /* LINE 9 ********************************************* LINE */

	 /* ACCOUNT NUMBER */
	 if (strlen(cl_ptr->charge_nbr))
	    len = add_outb_text(cl_ptr->charge_nbr);

	 if (strlen(cl_ptr->company_name))
	    len += add_outb_space();

	 /* COMPANY NAME */
	 len += add_outb_text(cl_ptr->company_name);

	 if (len > MAX_DISPLAY_LENGTH)
	 {
	    len -= MAX_DISPLAY_LENGTH;
	    while ((len >= 0) && len--)
	       delete_outb_char();
	 }
	 if (len > 0)
	    add_outb_text(CR);

	 len = 0;

	 /* LINE 10 ******************************************** LINE */

	 /* CALL RATE */
	 if (cl_ptr->call_rate != 0.0)
	 {
	    add_outb_text(CALL_RATE_HDR);
#ifdef SCANDINAVIA
	    sprintf(str_temp, "%7.2f%s", cl_ptr->call_rate, currency_symbol);
#else
	    sprintf(str_temp, "%s%7.2f", currency_symbol, cl_ptr->call_rate);
#endif
	    len += add_outb_text(str_temp);
	 } else if (cl_ptr->advise_arrival == YES)
	    for (ii = 0; ii < sizeof(CALL_RATE_HDR) + 8; ii++)
	       len += add_outb_space();
	 if (len)
	    len += add_outb_space();

	 /* ADVISE ARRIVAL */
	 if (cl_ptr->advise_arrival == YES)
	    add_outb_text(ADVISE_ARRIVAL_HDR);

	 if (len > MAX_DISPLAY_LENGTH)
	 {
	    len -= MAX_DISPLAY_LENGTH;
	    while ((len >= 0) && len--)
	       delete_outb_char();
	 }
	 break;

      case H_FORMAT:
	 /* LINE 3 *********************************************** LINE */

	 len = 0;

	 /* PASSENGER NAME */
	 if (strlen(cl_ptr->passenger_name))
	    len += add_outb_text(cl_ptr->passenger_name);
	 else if (cl_ptr->grouped_with)	/* ONLY IF MORE FIELDS */
	    for (ii = 0; ii <= sizeof(cl_ptr->passenger_name); ii++)
	       len += add_outb_space();

	 /* MULTIPLE CALL INDICATOR */
	 if (cl_ptr->grouped_with)
	 {
	    len += add_outb_space();
	    mult_nbr++;
	    sprintf(str_temp, "%s%1d %s %1d", MULTIPLE_CALL_H,
		    mult_nbr, OF, total_mult_nbr);
	    len += add_outb_text(str_temp);
	 }
	 if (len > MAX_DISPLAY_LENGTH)
	 {
	    len -= MAX_DISPLAY_LENGTH;
	    while ((len >= 0) && len--)
	       delete_outb_char();
	 }
	 if (len > 0)
	    add_outb_text(CR);

	 len = 0;

	 /* LINE 4 ********************************************* LINE */

	 /* STREET NAME - DESTINATION */
	 if (strlen(cl_ptr->dest_address.street_name))
	 {
	    len += add_outb_text(DEST_HDR_H);
	    len += add_outb_text(cl_ptr->dest_address.street_name);
	 }
	 /* STREET TYPE - DESTINATION */
	 if (strlen(cl_ptr->dest_address.street_type))
	 {
	    len += add_outb_space();
	    len += add_outb_text(cl_ptr->dest_address.street_type);
	 }
	 /* CITY - DESTINATION */
	 if (strlen(cl_ptr->dest_address.city_name))
	 {
	    len += add_outb_space();
	    len += add_outb_text(cl_ptr->dest_address.city_name);
	 }
	 if (len > MAX_DISPLAY_LENGTH)
	 {
	    len -= MAX_DISPLAY_LENGTH;
	    while ((len >= 0) && len--)
	       delete_outb_char();
	 }
	 if (len > 0)
	    add_outb_text(CR);

	 len = 0;

	 /* LINE 5 ********************************************* LINE */

	 /* STREET NUMBER - DESTINATION */
	 if (cl_ptr->dest_address.street_nbr)
	 {
	    sprintf(str_temp, "%d", cl_ptr->dest_address.street_nbr);
	    len += add_outb_text(str_temp);
	    len += add_outb_space();
	 } else if (strlen(cl_ptr->dest_address.apt) || strlen(cl_ptr->dest_dmap))
	    for (ii = 0; ii < sizeof(cl_ptr->dest_address.street_nbr); ii++)
	       len += add_outb_space();


	 /* APARTMENT NUMBER - DESTINATION */
	 if (add_outb_text(cl_ptr->dest_address.apt))
	 {
	    len += add_outb_space();
	    len += strlen(cl_ptr->dest_address.apt) + 1;
	 } else if (strlen(cl_ptr->dest_address.apt) || cl_ptr->dest_address.street_nbr)
	    for (ii = 0; ii < sizeof(cl_ptr->dest_address.apt); ii++)
	       len += add_outb_space();

	 /* MAP - DESTINATION */
	 if (strlen(cl_ptr->dest_dmap))
	 {
	    len += add_outb_space();
	    len += add_outb_text(cl_ptr->dest_dmap);
	 }
	 if (len > MAX_DISPLAY_LENGTH)
	 {
	    len -= MAX_DISPLAY_LENGTH;
	    while ((len >= 0) && len--)
	       delete_outb_char();
	 }
	 if (len > 0)
	    add_outb_text(CR);

	 len = 0;

	 /* LINES 6 - 7 **************************************** LINE */

	 /* COMMENT - GENERAL */
	 mk_comment(cl_ptr);

	 /* LINE 8 ********************************************* LINE */

	 /* VEHICLE and DRIVER ATTRIBUTES */
	 len += mk_attr_fields(1, ABRIV, call_ptr, veh_ptr, 0);

	 if (len)
	    len += add_outb_space();

	 /* ACCOUNT NUMBER */
	 if (strlen(cl_ptr->charge_nbr))
	    len += add_outb_text(cl_ptr->charge_nbr);

	 if (len > MAX_DISPLAY_LENGTH)
	 {
	    len -= MAX_DISPLAY_LENGTH;
	    while ((len >= 0) && len--)
	       delete_outb_char();
	 }
	 if (len > 0)
	    add_outb_text(CR);

	 len = 0;

	 /* LINE 9 ******************************************** LINE */

	 /* COMPANY NAME */
	 len += add_outb_text(cl_ptr->company_name);

	 if (len > MAX_DISPLAY_LENGTH)
	 {
	    len -= MAX_DISPLAY_LENGTH;
	    while ((len >= 0) && len--)
	       delete_outb_char();
	 }
	 if (len > 0)
	    add_outb_text(CR);

	 len = 0;

	 /* LINE 10 ******************************************** LINE */

	 /* CALL RATE */
	 if (cl_ptr->call_rate != 0.0)
	 {
	    add_outb_text(CALL_RATE_HDR);
#ifdef SCANDINAVIA
	    sprintf(str_temp, "%7.2f%s", cl_ptr->call_rate, currency_symbol);
#else
	    sprintf(str_temp, "%s%7.2f", currency_symbol, cl_ptr->call_rate);
#endif
	    len += add_outb_text(str_temp);
	 } else
	    for (ii = 0; ii < sizeof(CALL_RATE_HDR) + 8; ii++)
	       len += add_outb_space();

	 len += add_outb_space();

	 if (cl_ptr->pod_required == YES)
	 {
	    len += add_outb_text(PROOF_OF_DELIVERY);
	    len += add_outb_space();
	 } else
	    for (ii = 0; ii < sizeof(PROOF_OF_DELIVERY); ii++)
	       len += add_outb_space();


	 /* TRIP NUMBER */
	 len += add_outb_text(CALL_NUMBER_HDR);
	 sprintf(str_temp, "%6d", cl_ptr->nbr);
	 len += add_outb_text(str_temp);

#ifdef EXTRA_FIELDS
	 /* ADVISE ARRIVAL */
	 if (cl_ptr->advise_arrival == YES)
	    add_outb_text(ADVISE_ARRIVAL_HDR);
#endif

	 if (len > MAX_DISPLAY_LENGTH)
	 {
	    len -= MAX_DISPLAY_LENGTH;
	    while ((len >= 0) && len--)
	       delete_outb_char();
	 }
	 break;

      case F_FORMAT:
	 /* LINE 3 *********************************************** LINE */

	 add_outb_text(CR);

	 len = 0;
	 /* COMPANY NAME */
	 if (is_account_call(cl_ptr) == TRUE)
	 {
	    if (strlen(cl_ptr->company_name) > MDT_AC_NAME_LEN)
	       cl_ptr->company_name[MDT_AC_NAME_LEN] = '\0';
	    len += add_outb_text(cl_ptr->company_name);
	    if (len == 0)
	       for (i = 0; i < MDT_AC_NAME_LEN; i++)
		  len += add_outb_space();

	    if (cl_ptr->can_charge == YES)
	    {
	       len += add_outb_space();
	       len += add_outb_text(CHARGE_FLAG);	/* OK TO CHARGE */
	    }
	    if (len > 0)
	       add_outb_text(CR);

	    len = 0;
	 }
	 /* LINE 4 *********************************************** LINE */

	 /* PASSENGER NAME */
	 if (strlen(cl_ptr->passenger_name) > MDT_PASSENGER_NAME_LEN)
	    cl_ptr->passenger_name[MDT_PASSENGER_NAME_LEN] = '\0';
	 if (strlen(cl_ptr->passenger_name))
	 {
	    len += add_outb_text(cl_ptr->passenger_name);
	    len += add_outb_space();
	 }
	 /*
	  * else for (ii = 0; ii <= sizeof(cl_ptr->passenger_name); ii++) {
	  * len += add_outb_space(); len_blanks = len; }
	  */

	 len = 0;

	 /* PHONE EXTENSION */
	 if (strlen(cl_ptr->pckup_phone.phone_ext))
	 {
	    len += add_outb_text(PHONE_HDR);
	    len += add_outb_text(cl_ptr->pckup_phone.phone_ext);
	 }
	 if (strlen(cl_ptr->passenger_name) || strlen(cl_ptr->pckup_phone.phone_ext))
	    add_outb_text(CR);

	 /* LINE 5 *********************************************** LINE */

	 /* RESERVATION */

#ifdef FOO
	 for (j = 0; j < strlen(cl_ptr->due_time); j++)
	 {
	    if (cl_ptr->due_time[j] == ':')
	       cl_ptr->due_time[j] = TIME_SEPERATOR;
	 }
#endif
	 if (is_time_call(cl_ptr))
	 {
	    len += add_outb_text(RESV_TIME_HDR);
	    len += add_outb_space();
	    len += add_outb_text(cl_ptr->due_time);
	 } 
	 else
	 {
	    len += add_outb_text(T_DELAY);
	    len += add_outb_space();
	    sprintf(temp_buf, "%d", cl_ptr->ext.algorithm_delay);
	    len += add_outb_text(temp_buf);
	    len += add_outb_text(MINUTES_SYMBOL);
/*	    len += add_expected_due_time(cl_ptr->due_time, call_ptr->ext.algorithm_delay);*/
	 }
	 /* MAP COORDINATES - PICKUP */
	 if (strlen(cl_ptr->pckup_pmap))
	 {
	    len += add_outb_space();
	    len += add_outb_space();
	    len += add_outb_text(PAGE_HDR);
	    len += add_outb_text(cl_ptr->pckup_pmap);
	 }
	 add_outb_text(CR);

	 len = 0;


	 /* LINE 6 *********************************************** LINE */

	 if (cl_ptr->nbr_in_group > '1')
	 {
	    len += add_outb_text(TAXIS_HDR);
	    len += add_outb_space();
	    sprintf(buffer, "%c", cl_ptr->pos_in_group);
	    if (strlen(buffer))
	       len += add_outb_text(buffer);
	    len += add_outb_space();
	    len += add_outb_text(OF2);
	    sprintf(buffer, "%c", cl_ptr->nbr_in_group);
	    if (!strlen(buffer))
	       len += add_outb_space();
	    else
	       len += add_outb_text(buffer);
	    len += add_outb_space();
	 }
	 /* VEHICLE and DRIVER ATTRIBUTES */
	 len += mk_attr_fields(1, ABRIV, call_ptr, veh_ptr, MDT_ATTR_MAX);

	 if (len == len_blanks)	/* no attributes or passenger name */
	    while ((len >= 0) && len--)
	       delete_outb_char();	/* back out the blanks */

	 if (len > MAX_DISPLAY_LENGTH)
	 {
	    len -= MAX_DISPLAY_LENGTH;
	    while ((len >= 0) && len--)
	       delete_outb_char();
	 }
	 if (len > 0)
	    add_outb_text(CR);

	 len = 0;

	 /* LINE 7 ********** ACCOUNT COMMENT ******************** LINE */

	 if (strlen(cl_ptr->driver_msg))
	 {
	    if (strlen(cl_ptr->driver_msg) > AC_DRIVER_MSG_LEN)
	    {
	       cl_ptr->driver_msg[AC_DRIVER_MSG_LEN] = '\0';
	       ERROR(' ', "", "AC_DRIVER_MSG too long");
	    }
	    len += add_outb_text(cl_ptr->driver_msg);
	    add_outb_text(CR);
	    len = 0;
	 }
	 /* LINE 8 - 10********************************************* LINE */

	 mk_comment(cl_ptr);
      }				/* end case */


      send_pkt_id(ASSIGN_PKT_TYPE, 0, veh_ptr->veh_nbr);
      if (mult_nbr > 1)
	 status = send_msg_mmp(term_id, ASSIGN_DISPLAY_P1, veh_ptr);
      else
	 status = send_msg_mmp(term_id, ASSIGN_DISPLAY_P3, veh_ptr);
      if (status)
      {
	 sprintf(str_temp, "unable to send assignment to %s", veh_ptr->mid);
	 ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_temp);
      }
      clr_pkt_id();
   }
   return (0);
}				/* end send_assign */
