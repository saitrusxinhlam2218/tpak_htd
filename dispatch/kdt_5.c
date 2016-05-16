static char sz__FILE__[]="@(#)kdt_5.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  kdt_5.c; Rel: 10.1.2.1; Get date: 5/11/95 at 10:15:19
* @(#)  Last delta: 12/2/94 at 17:57:05
* @(#)  SCCS File: /taxi/sccs/s.kdt_5.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

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

#include "dispatch_strdefs.h"
#include "path.h"
#include "sprintf.h"
#include "mads_types.h"
#include "mad_error.h"
#include "mads_isam.h"
#include "rate_isam.h"
#include "dispatch.h"
#include "language.h"
#include "kdt.h"
#include "veh_driv.h"
#include "df_maint.h"
#include "timers.h"
#include "writer.h"
#include "ex_key.h"
#include "except.h"
#include "calls.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "msg.h"
#include "msg_mmp.h"
#include "tmplen.h"
#include "itm.h"
#include "KelaNode_db.h"
#define LIBRARY 1
#include "enhance.h"
#include "switch_ext.h"
#include "strmap.h"

void  delete_outb_char();
void  mk_outb_header();
void  terminate_outb_text();
void  mk_msg_priority();
void  config_outb_msg();

#define ABRIV TRUE
#define NOABRIV FALSE
#define MAP_START  4		/* map field offset in cal_ptr->pickup_pmap */

static char str_temp[TMP_STR_LEN];
struct cisam_cl cisam_call;

extern char *scratch_ptr;
extern char req_buf[];
extern struct fleets *fleet[FLEET_MAX];	/* shared memory fleet structure */
extern struct offsets *offset;	/* pointer to shared memory offsets */
extern struct cisam_ch writer_buf;


#define KOGE_COL2_START_1      22
#define KOGE_COL2_START_2      21
/*****************************************************************************/
/* */
/* get_call_record (cisam_number, call_number)                              */
/* */
/* This routine accepts the C-ISAM number and the C-ISAM call number, and   */
/* returns a pointer to a call structure.                                   */
/* */
/*****************************************************************************/

struct cisam_cl *
get_call_record(cisam_number, call_number)
   int   cisam_number, call_number;
{
   struct cisam_cl *call_input;
   call_input = (struct cisam_cl *) req_buf;

#ifdef DEBUG
   if (offset->prattle >= LEVEL5)
   {
      sprintf(trace_str,
	      "ENTERING get_call_record (cisam_number=%d, call_number=%d)\n",
	      cisam_number, call_number);
      TRACE(trace_str);
   }
#endif

   if (db(CALL_FILE_ID, READ_NUM, &cl_key1, ISEQUAL, cisam_number) != 0)
   {
      call_input->nbr = call_number;
      if (db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL, 0) != 0)
      {
	 sprintf(scratch_ptr,"db error %d get_call_record1: call %d", iserrno, call_number);
	 ERROR(' ', CALL_FILE, scratch_ptr);
	 return (NULL);
      }
   } else if (call_number != call_input->nbr)
   {				/* isam exists but not right record */
      call_input->nbr = call_number;
      if (db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL, 0) != 0)
      {
	 sprintf(scratch_ptr,"db error %d get_call_record2: call %d", iserrno, call_number);
	 ERROR(' ', CALL_FILE, scratch_ptr);
	 return (NULL);
      }
   }
   /* copy the buffer from disk */
   memcpy(&cisam_call, req_buf, sizeof(struct cisam_cl));

   return (&cisam_call);
}				/* end get_call_record */

#ifdef USED
static int
mk_send_offer_fields(call_ptr, veh_ptr)
   struct calls *call_ptr;
   struct veh_driv *veh_ptr;
/* make up part of the send offer message */
/* NOT PRESENTLY USED */
{
   short len = 0;

   if (call_ptr->grouped_with)
   {
      len += add_outb_space();
      len += add_outb_text(MULTIPLE_CALL);
   }
   if (call_ptr->personal_request == ONLY_PERS_REQ &&
       veh_ptr->veh_nbr == call_ptr->personal_vehicle)
   {
      if (len)
	 len += add_outb_space();
      len += add_outb_text(PERSONAL_ONLY);
   }
   if (call_ptr->personal_request == IF_PERS_REQ &&
       veh_ptr->veh_nbr == call_ptr->personal_vehicle)
   {
      if (len)
	 len += add_outb_space();
      len += add_outb_text(PERSONAL_IF);
   }
   return (len);
}				/* end mk_send_offer_fields */
#endif

/***********************************************************
 * This function was added due to the improper use of is_time_call()
 * TEK 10.02.90
 *************************************************************/
static int
is_time_call(call)
   struct cisam_cl *call;
/* does this trip require a due time for the assignment */
{
   int ii;

   if (!strncmp(call->extended_type, "KE",2))
     return (FALSE);
   
   for(ii=0;ii<sizeof(call->extended_type);ii++)
   {
    if (call->extended_type[ii]==SUBS)
            return (TRUE);
    if (call->extended_type[ii] == '\0' )					/* no more valid type chars */
	    break;
   }
   if (call->extended_type[2]==TIMEC)
     return(TRUE);
   
   return (FALSE);
}

/*
 * Function to test for AUTOBOOK calls
 */
static int is_autobook_call(call)
struct cisam_cl *call;
{
	int ii;
 
	for ( ii = 0; ii < sizeof(call->extended_type); ii++ )
	{

		if ( call->extended_type[ii] == AUTOBOOK_CALL )
			return(TRUE);

		if ( call->extended_type[ii] == '\0' )				/* no more valid type chars */
			break;
	}

	return(FALSE);
}
 

int
mk_pickup_address(assign_opt, cl_ptr, type, blk_len)
   char assign_opt;
   struct cisam_cl *cl_ptr;
   int   type;			/* street only or complete */
   int blk_len;                     
/* make up the pickup address portion of the call */
{
   int   len, ii, adjust;
   static char str_name[MAX_DISPLAY_LENGTH + TMP_STR_LEN];  /* little to big, but... */
   char *pDelimit = NULL;
   char *pTemp = NULL;

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
      if (len = add_outb_text(cl_ptr->pckup_pre_dir) != 0)
	 len += add_outb_space();

      /* PICKUP_STR_NAME */
      strcpy(str_name, cl_ptr->pckup_str_name);
      str_name[strlen(str_name) - adjust] = '\0';
      if (strlen(str_name));
      {
	 len += add_outb_text(str_name);
	 len += add_outb_space();
      }

      /* STREET TYPE - PCKUP */
      if (add_outb_text(cl_ptr->pckup_str_type))
      {
	 len += strlen(cl_ptr->pckup_str_type);
      }
      /* PICKUP POST_DIR */
      if (strlen(cl_ptr->pckup_post_dir))
      {
	 len += add_outb_space();
	 len += add_outb_text(cl_ptr->pckup_post_dir);
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
              delete_outb_char(); /* start_again */
            len = 0;
            goto redo;
         }
         else
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
      if (cl_ptr->pckup_str_nbr)
      {
	 sprintf(str_temp, "%d", cl_ptr->pckup_str_nbr);
	 len = add_outb_text(str_temp);
	 len += add_outb_space();
      } else if (strlen(cl_ptr->pckup_apt_room) || strlen(cl_ptr->pckup_pmap))
	 /* || is_time_call(call_ptr)) */
	 len = add_outb_text("      ");

      /* APARTMENT NUMBER - PICKUP */
      if (add_outb_text(cl_ptr->pckup_apt_room))
	 len += add_outb_space();

      /* CITY - PCKUP */
      len += add_outb_text(cl_ptr->pckup_city);
      len += add_outb_space();

      /* MAP COORDINATES - PICKUP */
      if (strlen(cl_ptr->pckup_pmap))
      {
	 len += add_outb_text(MAP_HDR);
	 len += add_outb_text(cl_ptr->pckup_pmap);
      }

      /* need a CR ???? */
      return (len);

   case TH_FORMAT:
   case H_FORMAT:

      /* LINE 1 ********************************************* LINE */


      /* PICKUP_STR_NAME */
      if (add_outb_text(cl_ptr->pckup_str_name));
      {
	 len += add_outb_space();
	 len += strlen(cl_ptr->pckup_str_name);
      }

      /* STREET TYPE - PCKUP */
      if (add_outb_text(cl_ptr->pckup_str_type))
      {
	 len += strlen(cl_ptr->pckup_str_type);
      }

      if (type == STREET_ONLY)
	 return (len);

      /* CITY - PCKUP */
      len += add_outb_space();
      len += add_outb_text(cl_ptr->pckup_city);

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
      if (cl_ptr->pckup_str_nbr)
      {
	 sprintf(str_temp, "%d", cl_ptr->pckup_str_nbr);
	 len = add_outb_text(str_temp);
	 len += add_outb_space();
      } else if (strlen(cl_ptr->pckup_apt_room) || strlen(cl_ptr->pckup_pmap)
		 || is_time_call(cl_ptr))
	 len = add_outb_text("      ");

      /* APARTMENT NUMBER - PICKUP */
      if (add_outb_text(cl_ptr->pckup_apt_room))
	 len += add_outb_space();
      else if (strlen(cl_ptr->pckup_apt_room) || cl_ptr->pckup_str_nbr ||
	       is_time_call(cl_ptr))
	 for (ii = 0; ii < sizeof(cl_ptr->pckup_apt_room); ii++)
	    len += add_outb_space();
      /*
       * CITY - PCKUP len += add_outb_text(cl_ptr->pckup_city); len +=
       * add_outb_space();
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

   pTemp = strstr(cl_ptr->general_cmnt, "PUH.");
   if ( strlen( cl_ptr->phone_number ) && (pTemp == NULL))
   {
        len += add_outb_text( "PUH: ");
        len += add_outb_text( cl_ptr->phone_number );
        add_outb_text(CR);
   }

      return (len);

    case KOGE_FORMAT:
      len = 0;
      if ( strlen(cl_ptr->pckup_adr_cmnt) )
	len = add_outb_text(cl_ptr->pckup_adr_cmnt);

      if ( is_time_call( cl_ptr ) )
	{
	  while ( len != KOGE_COL2_START_1 )
	    len += add_outb_space();
	  len += add_outb_text(TIME_DUE_HDR);
	  len += add_outb_text(cl_ptr->due_time);

	}
      if ( len != 0 )
	add_outb_text(CR);

      /* Line 2 */
      len = add_outb_text( cl_ptr->pckup_str_name );
      while ( len != KOGE_COL2_START_1 )
	len += add_outb_space();
      if ( cl_ptr->pckup_str_nbr )
	{
	  sprintf( str_temp, "%d", cl_ptr->pckup_str_nbr );
	  len += add_outb_text( str_temp );
	}
      if ( len != 0 )
	add_outb_text(CR);

      /* Line 3 */
      len = add_outb_text( cl_ptr->pckup_city );
      while ( len != KOGE_COL2_START_2 )
	len += add_outb_space();
      if ( strlen( cl_ptr->pckup_pmap ) )
	len += add_outb_text( cl_ptr->pckup_pmap );
      if ( len != 0 )
	add_outb_text(CR);
      

   }				/* end case */
   if ( strlen( cl_ptr->phone_number ) )
   {
	len += add_outb_text( "PUH: ");
	len += add_outb_text( cl_ptr->phone_number );
        add_outb_text(CR);
   }
}				/* end mk_pickup_address */

#ifdef EXTRA_FIELDS
static int
mk_intr_fields(cl_ptr)
   struct cisam_cl *cl_ptr;
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
      len += add_outb_text(DELIEVERY_TYPE);
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
mk_attr_fields(max_lines, abriv, call_ptr, veh_ptr)
   int   max_lines;		/* how may line to allow */
   short abriv;			/* abriviation indicator */
   struct calls *call_ptr;
   struct veh_driv *veh_ptr;
/* make the attribute fields */
/* all on one line if abbreviated - two per line otherwise */
/* return the number of characters added - useful for abriv only */
{
   short jj, i,field_cnt;
   unsigned char *byte_ptr;
   unsigned char mask;
   int   header_adjust;
   int   len = 0;

   header_adjust = sizeof(ATTRIBUTES_HDR) / 2;

   if (Call_get_value((CALL_HNDL) call_ptr, CALL_VEH_DRIV_ATTR ))
   {
#ifdef EXTRA_FIELDS
      char_cnt += add_outb_text(ATTRIBUTES_HDR);
#endif



      field_cnt = 0;
      str_temp[0] = '\0';

      byte_ptr = (unsigned char *) &call_ptr->vehicle_attributes;      

      /* VEHICLE ATTRIBUTES */
      for ( i = 0; i < 4; i ++ )
	{
	  mask = 0x80;
	  for (jj = 0; jj <= 8; jj++)
	    {
	      if (*byte_ptr & mask)	/* this attribute is set */
		{
		  if (abriv)
		    {
		      if (fleet[veh_ptr->fleet_nbr]->call_assignment_info == S_FORMAT)
			sprintf(str_temp, " %s", fleet[veh_ptr->fleet_nbr]->
				vd_attr_info[jj +(i*8)].veh_attr);
		      else
			sprintf(str_temp, "%s ", fleet[veh_ptr->fleet_nbr]->
				vd_attr_info[jj + (i*8)].veh_attr);
		    }
		  else
		    {
		      field_cnt++;
		      if (get_outb_line_size() == 0)	/* not first attr line */
			add_outb_text(" ");
		      strncpy(str_temp,
			      fleet[veh_ptr->fleet_nbr]->vd_attr_info[jj + (i*8)].veh_at_desc,
			      (field_cnt < 3) ? ATTR_FIELD_WIDTH - header_adjust :
			      ATTR_FIELD_WIDTH);
		      if (field_cnt % 2)
			strcat(str_temp, " ");	/* attribute separator */
		    }
		  len += add_outb_text(str_temp);
		  if (field_cnt && !(field_cnt % 2))
		    {
		      add_outb_text(CR);
		      if ((field_cnt % 2) >= max_lines)
			return(SUCCESS);
		    }
		}
	      mask >>= 1;		/* shift right once */
	    }
	  ++byte_ptr;
	}

      byte_ptr = (unsigned char *) &call_ptr->driver_attributes;

      /* DRIVER ATRIBUTES */
      for ( i = 0; i < 4; i ++ )
	{
	  mask = 0x80;      
	  for (jj = 0; jj < ATTR_MAX; jj++)
	    {
	      if (*byte_ptr & mask)	/* this attribute is set */
		{
		  if (abriv)
		    {
		      if (fleet[veh_ptr->fleet_nbr]->call_assignment_info == S_FORMAT)
			sprintf(str_temp, " %s", fleet[veh_ptr->fleet_nbr]->
				vd_attr_info[jj +(i*8)].drv_attr);
		      else
			sprintf(str_temp, "%s ", fleet[veh_ptr->fleet_nbr]->
				vd_attr_info[jj+(i*8)].drv_attr);
		    }
		  else
		    {
		      field_cnt++;
		      if (get_outb_line_size() == 0)	/* not first attr line */
			add_outb_text(" ");
		      strncpy(str_temp,
			      fleet[veh_ptr->fleet_nbr]->vd_attr_info[jj+(i*8)].drv_at_desc,
			      (field_cnt < 3) ? ATTR_FIELD_WIDTH - header_adjust :
			      ATTR_FIELD_WIDTH);
		      if (field_cnt % 2)
			strcat(str_temp, " ");	/* attribute separator */
		    }
		  len += add_outb_text(str_temp);
		  if (field_cnt && !(field_cnt % 2))
		    {
		      add_outb_text(CR);
		      if ((field_cnt % 2) >= max_lines)
			return(SUCCESS);
		    }
		}
	      mask >>= 1;		/* shift right once */
	    }
	  ++byte_ptr;
	}
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
struct calls *call_ptr;		/* call no offered */
struct veh_driv *veh_ptr;
{
   struct cisam_cl *cl_ptr;
   char *pTmp;
   unsigned char *byte_ptr;
   unsigned char mask;
   char  offer_opt;
   int   len = 0;
   int   line_cnt = 1;
   int ii;
   int firstcall, nextcall;
   int   c_isam_num;
   double call_inx = 0.0, call_iny = 0.0, call_outx = 0.0, call_outy = 0.0;
   char   bcast_buf[512];   

   if (!veh_ptr->t_status.kdt_working)
   {				/* in emulation mode */
     Call_delete_timer( (CALL_HNDL)call_ptr, T_ACCPT );
     send_assign(term_id, call_ptr, veh_ptr, FALSE);	/* send call assignment info */
     return (0);
   }
   
   if ((cl_ptr = get_call_record(call_ptr->c_isam_num,
				 call_ptr->call_number)) == NULL)
     {
       if ((call_ptr != NULL))
	 if(bit_check(SYS_ERR,call_ptr) <= 0) /* Check if SYS_ERR exception already occurred */
	   log_assign_nak(SYS_ERR, call_ptr);
       
       mk_except(call_ptr, veh_ptr, SYS_ERR, ASSIGN_ERR, -1);
       return (-1);
     }
   
   /* LINE  1 ************************************************ LINE */

   /* CALL HEADER */
   strcpy(str_temp, CALL_AVAIL_HDR);
   (void) mk_outb_text(str_temp);

   /* ZONE - PICK UP */
#ifdef NAMERICA
   if ( ( call_ptr->dest_zone <= 0 ) ||
        ( fleet[veh_ptr->fleet_nbr]->send_dest_offer == NO ) )
#else
   if ( ( call_ptr->dest_zone <= 0 ) || (!(0) ) )
#endif
     {
       if ( (char)Fleet_get_value( (FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_SEND_CALL_TYPE ) == YES )
	 {
	   if (!strncmp(cl_ptr->extended_type, "KE", 2))
	     sprintf( str_temp, " %03d (%s)",
		      call_ptr->pickup_zone, "KE" );
	   else
	     sprintf( str_temp, " %03d (%s)",
		      call_ptr->pickup_zone, cl_ptr->extended_type );
	   if (!strncmp(cl_ptr->extended_type, "MWAP",4))
	       sprintf(str_temp, "%s%s", str_temp, CR);
	 }
       else
	 sprintf(str_temp, " %03d",
		 call_ptr->pickup_zone);
       add_outb_text( str_temp );       
     }
   else
     {
       sprintf(str_temp, "%s%03d->%03d",
	       CR, call_ptr->pickup_zone, call_ptr->dest_zone);
       (void) add_outb_text(str_temp);

       /** Check for Multi calls **/
       firstcall = cl_ptr->nbr;
       if ( cl_ptr->grouped_with == cl_ptr->nbr )
	 nextcall = 0;
       else
	 nextcall = cl_ptr->grouped_with;
       while ( ( nextcall ) )
	 {
	   c_isam_num = 0;
	   cl_ptr = get_call_record( c_isam_num, nextcall );
	   if ( cl_ptr->dest_zone <= 0 )
	     sprintf(str_temp, " %03d", cl_ptr->pckup_zone);
	   else
	     {
	       sprintf(str_temp, " %03d->%03d",
		       cl_ptr->pckup_zone, cl_ptr->dest_zone);
	     }
	   add_outb_text(str_temp);
	   
	   if ( cl_ptr->grouped_with == firstcall )
	     nextcall = 0;
	   else
	     nextcall = cl_ptr->grouped_with;
	 }
       if (cl_ptr->nbr != firstcall)
	 {
	   c_isam_num = 0;
	   cl_ptr = get_call_record( c_isam_num, firstcall );
	 }

     }
	 


   if ((0) && ((call_ptr->FTJ_nbr) > NOT_EXTERNAL))       /* get offer type from call_ptr if SamPlan and
								     call is an external call */
	   offer_opt = call_ptr->offer_type;
   else
	   offer_opt = fleet[veh_ptr->fleet_nbr]->call_offer_info;


   /** Stockholm wants control over whether to display call type in offer **/
   if (0)
     {
       if ( (char)Fleet_get_value( (FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_SEND_CALL_TYPE ) == YES )
	 {
	   len = add_outb_text( CR );
	   len = add_outb_text(TYPE_HDR);
	   len += add_outb_space();
	   sprintf(str_temp, "%s",
		   cl_ptr->extended_type);
	   add_outb_text(str_temp);
	   add_outb_text(CR);
	 }
     }

#ifdef MPK_DETAILS
      if ( !strncmp(cl_ptr->extended_type, "MPK", 3) )
     {
       len = add_outb_text("%RHAKU:");
       if ((pTmp = strstr(cl_ptr->dest_adr_cmnt, ":")) != NULL )
	 sprintf(str_temp, " %s", pTmp+1);
       add_outb_text(str_temp);
       add_outb_text(CR);
     }
#endif

      if (call_ptr->vehicle_attributes.attr31 ||
	  call_ptr->driver_attributes.attr11)
	{
	  (void)add_outb_text(CR);
	  if (call_ptr->vehicle_attributes.attr31)
	    add_outb_text("IN");
	  if (call_ptr->driver_attributes.attr11 && call_ptr->vehicle_attributes.attr31)
	    add_outb_text(" PP");
	  else if (call_ptr->driver_attributes.attr11)
	    add_outb_text("PP");
	  
	}

   switch (offer_opt)
   {
   case ZONE_ONLY:
      break;                       /* nothing to do */
   case ADDRESS:
      (void)add_outb_text(CR);
      if (strlen(fleet[veh_ptr->fleet_nbr]->zone_ptr[call_ptr->
                                             pickup_zone]->description) > 0)
        {
                sprintf(str_temp, "%s",
                        fleet[veh_ptr->fleet_nbr]->zone_ptr[call_ptr->
                                                 pickup_zone]->description);
                len += add_outb_text(str_temp);
                (void)add_outb_text(CR);
	}
      (void) mk_pickup_address(fleet[veh_ptr->fleet_nbr]->call_assignment_info,
                              cl_ptr,
                              COMPLETE_ADDRESS, 0); 
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
		add_outb_text(CR);
		if ( (0) && ( call_ptr->personal_vehicle > 0 ) )
		  {
		    sprintf( str_temp, "PERSONLIG" );
		    add_outb_text( str_temp );
		  }
#ifdef FOO
		if ( (0) && (fleet[veh_ptr->fleet_nbr]->send_dest_offer == YES ) &&
		     ( call_ptr->dest_zone > 0 ) &&
		     (strlen(fleet[veh_ptr->fleet_nbr]->zone_ptr[call_ptr->dest_zone]->description) > 0 ) )
		  {
		    sprintf( str_temp, "TO %s", fleet[veh_ptr->fleet_nbr]->zone_ptr[call_ptr->dest_zone]->description );
		    len += add_outb_text(str_temp);
		    (void)add_outb_text(CR);
		  }
#endif
      	}


      if (offer_opt == ZONE_DESC)
	 break;

      (void) add_outb_text(CR);

      if (offer_opt == ZONE_BLOCK)
      {

	 /* LINE 3 ********************************************* LINE */

      if (strlen(fleet[veh_ptr->fleet_nbr]->zone_ptr[call_ptr->
                                             pickup_zone]->description) > 0)
        {
                sprintf(str_temp, "%s",
                        fleet[veh_ptr->fleet_nbr]->zone_ptr[call_ptr->
                                                 pickup_zone]->description);
                len += add_outb_text(str_temp);
                (void)add_outb_text(CR);
        }
	 if ((cl_ptr = get_call_record(call_ptr->c_isam_num,
				       call_ptr->call_number)) == NULL)
	 {
	    /* Log NACK on Trip Details to Call History */
	    if ((call_ptr != NULL))
	      if(bit_check(SYS_ERR,call_ptr) <= 0) /* Check if SYS_ERR exception already occurred */
		log_assign_nak(SYS_ERR, call_ptr);

	    mk_except(call_ptr, veh_ptr, SYS_ERR, ASSIGN_ERR, -1);
	    return (-1);
	 }
	 len = 0;

	 /* PICKUP_STR_NAME  etc */
         str_temp[0] = '\0';
	 if (cl_ptr->pckup_str_nbr)
            /* compute now so we can tell how much space needed */
	   
            /** this is the old method which will give the block number for the address
	        This is really only of use in North America
		
	    sprintf(str_temp, "%d", cl_ptr->pckup_str_nbr -
		    cl_ptr->pckup_str_nbr % 100);

	     **/
	    
            sprintf( str_temp, "%d", cl_ptr->pckup_str_nbr );
	 
         len += mk_pickup_address(S_FORMAT, /* ignore the fleet setting */
                                                         cl_ptr,
                                                         STREET_ONLY,
                                                         strlen(str_temp) + 1); 

	 /* STREET NUMBER MOD 100 - PICKUP */
	 if (cl_ptr->pckup_str_nbr)
	 {
            len += add_outb_space();
	    len += add_outb_text(str_temp);
	 }

	 break;
      }
      /* LINE 3 ********************************************* LINE */

      /* CALL TYPE */
      if ( !(0) )
	{
	  len = add_outb_text(TYPE_HDR);
	  len += add_outb_space();
	  sprintf(str_temp, "%s",
		  cl_ptr->extended_type);
	  add_outb_text(str_temp);
	}

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

   case OFFER5 :
   case OFFER6 :
   case OFFER7 :
   case OFFER8 :
   case OFFER9 :
	 for (ii = 0; ii < 10; ii++) /* Blank MDT screen */
		add_outb_text(CR);
	 break;

   }				/* end case */

	// Mark offer with 'FA' if this attribute is present

      if (call_ptr->vehicle_attributes.attr4) // FA attribute set
	{
	  add_outb_text(CR);
	  add_outb_text("FA");
	}   
#ifdef MPK_DETAILS
   if ( !strncmp(cl_ptr->extended_type, "MPK", 3) )
     {
       len = add_outb_text("%RARVIO: ");
       if ((pTmp = strstr(cl_ptr->dest_adr_cmnt, ":")) != NULL )
	 {
	   bzero(str_temp, TMP_STR_LEN);
	   strncpy(str_temp, cl_ptr->dest_adr_cmnt, (pTmp - cl_ptr->dest_adr_cmnt));
	   add_outb_text(str_temp);
	 }
       add_outb_text(CR);
     }
#endif


   /* Set packet id to catch NAK */
   send_pkt_id(OFFER_PKT_TYPE, call_ptr->call_number,
	       (int)Veh_get_value((VEH_HNDL)veh_ptr, VEH_NBR));

   if (cl_ptr->gps_long > 0.0 && cl_ptr->gps_lat > 0.0) // trip has coordinates
     {
       call_inx = cl_ptr->gps_long;
       call_iny = cl_ptr->gps_lat;
       bzero( bcast_buf, 512 );
       LatLong_to_UTM( call_iny, call_inx, &call_outy, &call_outx );
       UTM_to_LatLong( call_iny, call_inx, &call_outy, &call_outx );
       
       sprintf( bcast_buf, "%%.XQ+%07d+%08d.",
		(int)(call_outy*100000), (int)(call_outx*100000) );       

       add_outb_text(bcast_buf);
     }

   if (send_msg_mmp(term_id, OFFER_DISPLAY, veh_ptr))
   {
      sprintf(str_temp, "unable to send offer to %s", veh_ptr->mid);
      ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_temp);
      /* will wait for the no accept timeout */
      /* if we no_accept may recurse if pipe full */
      /* no_accept(call_ptr, 0); */        /* 0 = Don't penalize driver */
   }

   /* Clear packet id for next message */
   clr_pkt_id();

#ifdef DEBUG
   if (offset->prattle >= LEVEL6)
      TRACE("LEAVING send_offer\n");
#endif
   return (0);
}				/* end send_offer */

int
send_cbook_offer(term_id, call_ptr, veh_ptr)	/* send call offer info */
   char  term_id[];		/* rf id */
struct calls *call_ptr;		/* call no offered */
struct veh_driv *veh_ptr;
{
   struct cisam_cl *cl_ptr;
   unsigned char *byte_ptr;
   unsigned char mask;
   unsigned char *addr;
   char  offer_opt;
   int   len = 0;
   int   line_cnt = 1;
   int ii,jj,i;
   int firstcall, nextcall;
   int   c_isam_num;

   if (!veh_ptr->t_status.kdt_working)
   {				/* in emulation mode */
     Call_delete_timer( (CALL_HNDL)call_ptr, T_ACCPT );
     send_assign(term_id, call_ptr, veh_ptr, FALSE);	/* send call assignment info */
     return (0);
   }
   
   if ((cl_ptr = get_call_record(call_ptr->c_isam_num,
				 call_ptr->call_number)) == NULL)
     {
       if ((call_ptr != NULL))
	 if(bit_check(SYS_ERR,call_ptr) <= 0) /* Check if SYS_ERR exception already occurred */
	   log_assign_nak(SYS_ERR, call_ptr);
       
       mk_except(call_ptr, veh_ptr, SYS_ERR, ASSIGN_ERR, -1);
       return (-1);
     }
   
   /* LINE  1 ************************************************ LINE */

   /* CALL HEADER */
   strcpy(str_temp, CALL_AVAIL_HDR);
   (void) mk_outb_text(str_temp);

   /* ZONE - PICK UP */

   if ( ( call_ptr->dest_zone <= 0 ) || (!(0) ) )
     {
       if ( (char)Fleet_get_value( (FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_SEND_CALL_TYPE ) == YES )
	 sprintf( str_temp, " %03d (%s)",
		 call_ptr->pickup_zone, cl_ptr->extended_type );
       else
	 sprintf(str_temp, " %03d",
		 call_ptr->pickup_zone);
       add_outb_text( str_temp );       
     }
   else
     {
       sprintf(str_temp, "%s%03d->%03d",
	       CR, call_ptr->pickup_zone, call_ptr->dest_zone);
       (void) add_outb_text(str_temp);

       /** Check for Multi calls **/
       firstcall = cl_ptr->nbr;
       if ( cl_ptr->grouped_with == cl_ptr->nbr )
	 nextcall = 0;
       else
	 nextcall = cl_ptr->grouped_with;
       while ( ( nextcall ) )
	 {
	   c_isam_num = 0;
	   cl_ptr = get_call_record( c_isam_num, nextcall );
	   if ( cl_ptr->dest_zone <= 0 )
	     sprintf(str_temp, " %03d", cl_ptr->pckup_zone);
	   else
	     {
	       sprintf(str_temp, " %03d->%03d",
		       cl_ptr->pckup_zone, cl_ptr->dest_zone);
	     }
	   add_outb_text(str_temp);
	   
	   if ( cl_ptr->grouped_with == firstcall )
	     nextcall = 0;
	   else
	     nextcall = cl_ptr->grouped_with;
	 }
       if (cl_ptr->nbr != firstcall)
	 {
	   c_isam_num = 0;
	   cl_ptr = get_call_record( c_isam_num, firstcall );
	 }

     }
	 


   if ((0) && ((call_ptr->FTJ_nbr) > NOT_EXTERNAL))       /* get offer type from call_ptr if SamPlan and
								     call is an external call */
	   offer_opt = call_ptr->offer_type;
   else
	   offer_opt = fleet[veh_ptr->fleet_nbr]->call_offer_info;


   /** Stockholm wants control over whether to display call type in offer **/
   if (0)
     {
       if ( (char)Fleet_get_value( (FLEET_HNDL)fleet[veh_ptr->fleet_nbr], FLEET_SEND_CALL_TYPE ) == YES )
	 {
	   len = add_outb_text( CR );
	   len = add_outb_text(TYPE_HDR);
	   len += add_outb_space();
	   sprintf(str_temp, "%s",
		   cl_ptr->extended_type);
	   add_outb_text(str_temp);
	   add_outb_text(CR);
	 }
     }

   switch (offer_opt)
   {
   case ZONE_ONLY:
      break;                       /* nothing to do */
   case ADDRESS:
      (void) add_outb_text(CR);
      (void) mk_pickup_address(fleet[veh_ptr->fleet_nbr]->call_assignment_info,
                              cl_ptr,
                              COMPLETE_ADDRESS, 0); 
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
		add_outb_text(CR);
		if ( (0) && ( call_ptr->personal_vehicle > 0 ) )
		  {
		    sprintf( str_temp, "PERSONLIG" );
		    add_outb_text( str_temp );
		  }
#ifdef FOO
		if ( (0) && (fleet[veh_ptr->fleet_nbr]->send_dest_offer == YES ) &&
		     ( call_ptr->dest_zone > 0 ) &&
		     (strlen(fleet[veh_ptr->fleet_nbr]->zone_ptr[call_ptr->dest_zone]->description) > 0 ) )
		  {
		    sprintf( str_temp, "TO %s", fleet[veh_ptr->fleet_nbr]->zone_ptr[call_ptr->dest_zone]->description );
		    len += add_outb_text(str_temp);
		    (void)add_outb_text(CR);
		  }
#endif
      	}
      if (offer_opt == ZONE_DESC)
	 break;

      (void) add_outb_text(CR);

      if (offer_opt == ZONE_BLOCK)
      {

	 /* LINE 3 ********************************************* LINE */

	 if ((cl_ptr = get_call_record(call_ptr->c_isam_num,
				       call_ptr->call_number)) == NULL)
	 {
	    /* Log NACK on Trip Details to Call History */
	    if ((call_ptr != NULL))
	      if(bit_check(SYS_ERR,call_ptr) <= 0) /* Check if SYS_ERR exception already occurred */
		log_assign_nak(SYS_ERR, call_ptr);

	    mk_except(call_ptr, veh_ptr, SYS_ERR, ASSIGN_ERR, -1);
	    return (-1);
	 }
	 len = 0;

	 /* PICKUP_STR_NAME  etc */
         str_temp[0] = '\0';
	 if (cl_ptr->pckup_str_nbr)
            sprintf( str_temp, "%d", cl_ptr->pckup_str_nbr );
	 
         len += mk_pickup_address(S_FORMAT, /* ignore the fleet setting */
                                                         cl_ptr,
                                                         STREET_ONLY,
                                                         strlen(str_temp) + 1); 

	 /* STREET NUMBER MOD 100 - PICKUP */
	 if (cl_ptr->pckup_str_nbr)
	 {
            len += add_outb_space();
	    len += add_outb_text(str_temp);
	 }

	 break;
      }
      /* LINE 3 ********************************************* LINE */

      /* CALL TYPE */
      if ( !(0) )
	{
	  len = add_outb_text(TYPE_HDR);
	  len += add_outb_space();
	  sprintf(str_temp, "%s",
		  cl_ptr->extended_type);
	  add_outb_text(str_temp);
	}

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

   case OFFER5 :
   case OFFER6 :
   case OFFER7 :
   case OFFER8 :
   case OFFER9 :
	 for (ii = 0; ii < 10; ii++) /* Blank MDT screen */
		add_outb_text(CR);
	 break;

   }				/* end case */

   // Last two lines are for attributes
   addr = (unsigned char *) &call_ptr->vehicle_attributes;
   if ( *addr )   // got some veh attr's
     {
       add_outb_text(CR);
       add_outb_text("BIL ATTR: ");
       for ( i = 0; i < 4; i++ )
         {
           mask = 0x80;
           for ( jj = 0; jj <= 8; jj++ )
             {
               if ( *addr & mask )
                 {
                   add_outb_text( fleet[veh_ptr->fleet_nbr]->vd_attr_info[jj +(i*8)].veh_attr );
                   add_outb_text( " " );
                 }
               mask >>= 1;
             }
           ++addr;
         }
     }
   addr = (unsigned char *) &call_ptr->driver_attributes;
   if ( *addr )  // got some driver attr's
     {
       add_outb_text(CR);
       add_outb_text("F�R ATTR: ");
       for ( i = 0; i < 4; i++ )
         {
           mask = 0x80;
           for ( jj = 0; jj <= 8; jj++ )
             {
               if ( *addr & mask )
                 {
                   add_outb_text( fleet[veh_ptr->fleet_nbr]->vd_attr_info[jj +(i*8)].drv_attr );
                   add_outb_text( " " );
                 }
               mask >>= 1;
             }
           ++addr;
         }
     }       
       
   /* Set packet id to catch NAK */
   send_pkt_id(OFFER_PKT_TYPE, call_ptr->call_number,
	       (int)Veh_get_value((VEH_HNDL)veh_ptr, VEH_NBR));

   if (send_msg_mmp(term_id, OFFER_DISPLAY, veh_ptr))
   {
      sprintf(str_temp, "unable to send offer to %s", veh_ptr->mid);
      ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_temp);
   }

   /* Clear packet id for next message */
   clr_pkt_id();

   return (0);
}				/* end send_offer */

static void
mk_comment(cl_ptr)
   struct cisam_cl *cl_ptr;
/* make the gerneal comment field of the assignment message */
{
   char  comment_buf[MAX_COMMENT_LENGTH + 1];
   char *cptr;
   int len;
   char *pDestComment;

   /* if we have more than 32 characters, less than 64 then and */
   /* the first line does not end with a space attempt to avoid word wrap  */

   bzero( comment_buf, sizeof( comment_buf ) );
   len = strlen(cl_ptr->general_cmnt);
   if (len > MAX_DISPLAY_LENGTH
       && len < MAX_COMMENT_LENGTH
       && !(isspace(*(cl_ptr->general_cmnt + MAX_DISPLAY_LENGTH - 1))))
   {
      /* copy only the amount of text that can be displayed */
      /* */
#ifdef NAMERICA
     if (((pDestComment = strstr(cl_ptr->general_cmnt, "DEST:")) != NULL) &&
	 (cl_ptr->send_destination != YES))
       {
	 if ((pDestComment - cl_ptr->general_cmnt) == 0 )
	   strncpy(comment_buf, cl_ptr->general_cmnt, MAX_COMMENT_LENGTH);
	 else if ((pDestComment - cl_ptr->general_cmnt) < MAX_COMMENT_LENGTH)
	   strncpy(comment_buf, cl_ptr->general_cmnt, (pDestComment - cl_ptr->general_cmnt - 1));
	 else
	   strncpy(comment_buf, cl_ptr->general_cmnt, MAX_COMMENT_LENGTH);
       }
     else
       strncpy(comment_buf, cl_ptr->general_cmnt, MAX_COMMENT_LENGTH);
#else
     strncpy(comment_buf, cl_ptr->general_cmnt, MAX_COMMENT_LENGTH);
#endif
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
#ifdef NAMERICA
     if (((pDestComment = strstr(cl_ptr->general_cmnt, "DEST:")) != NULL) &&
	 (cl_ptr->send_destination != YES))
       {
	 if ((pDestComment - cl_ptr->general_cmnt) == 0)
	   strncpy(comment_buf, cl_ptr->general_cmnt, MAX_COMMENT_LENGTH);	   
	 else if ((pDestComment - cl_ptr->general_cmnt) < MAX_COMMENT_LENGTH)
	   strncpy(comment_buf, cl_ptr->general_cmnt, (pDestComment - cl_ptr->general_cmnt - 1));
	 else
	   strncpy(comment_buf, cl_ptr->general_cmnt, MAX_COMMENT_LENGTH);
       }
#else
     strncpy(comment_buf, cl_ptr->general_cmnt, MAX_COMMENT_LENGTH);
#endif
     comment_buf[MAX_COMMENT_LENGTH] = '\0';
     add_outb_text(cl_ptr->general_cmnt);
      if (len != MAX_DISPLAY_LENGTH && len != (2 * MAX_DISPLAY_LENGTH))
	 add_outb_text(CR);
   }

}				/* end mk_comment */

static int
get_num_mult_calls(nextcall, callnum)
   int   nextcall;
   int   callnum;
/* find out hwo many calls are grouped together */
/* this is enefficient and should be done at entry time */
{
   struct cisam_cl *cl_ptr;
   int   num_calls = 0;		/* 0 ?? */

   while (nextcall)
   {
      num_calls++;
      if ((cl_ptr = get_call_record(0, nextcall)) == NULL)
	 /* will record this error later */
	 return (num_calls);

      if (cl_ptr->grouped_with == callnum)
	 nextcall = 0;
      else
	 nextcall = cl_ptr->grouped_with;
   }
   return (num_calls);
}				/* end get_num_mult_calls */

/*****************************************************************************/
/* */
/* send_assign - Send Call Assignment Information Terminal                 */
/* */
/*****************************************************************************/

int
send_assign(term_id, call_ptr, veh_ptr, update)	/* send call assignment info */
   char  term_id[];		/* rf id */
struct calls *call_ptr;		/* call no offered */
struct veh_driv *veh_ptr;
   BOOLEAN update;
{				/* variable length message */
   struct cisam_cl *cl_ptr;
   short ii;
   int   callnum, nextcall;
   int   c_isam_num;
   int   mult_num;
   int   total_mult_num;
   int   len = 0;		/* current line length */
   int   len_blanks = 0;
   char  priority = PRIORITY_3;
   char *currency_symbol;
   char  comment_buf[MAX_COMMENT_LENGTH + 1];
   int   status;
   char  assign_opt;
   char  tmp_itm_buf[32];
   struct cisam_cmsg    *cl_msg;
   int   do_update = FALSE;
   int   FirstMultSent;
   char *pDelimit = NULL;
   BOOLEAN bAddDestHdr = FALSE;
   double call_inx = 0.0, call_iny = 0.0, call_outx = 0.0, call_outy = 0.0;
   char   bcast_buf[512];
   char *sMPKnode = NULL;
   KELANODE_REC kelanode_rec;
   int   countKelaNodes = 0;
   int   nodePairs = 0;
   BOOLEAN bSendKela = FALSE;
   int dbmode = ISFIRST;
   char kela_details_temp[TMP_STR_LEN];
   char kela_resp_temp[TMP_STR_LEN];
   char kela_rte_id[128];
   char theRouteID[64];
   char tmp_pax[32];
   char buf[255];
   StrMap *sm;

   sm = sm_new(10);
   
   if ((currency_symbol = nl_langinfo(CRNCYSTR)) == NULL)
      currency_symbol = " ";
   else
     /** this is necessary to move past the indicator '+' or '-' which **/
     /** indicates whether the symbol is leading or trailing           **/
     ++currency_symbol;

#ifdef DEBUG
   if (offset->prattle >= LEVEL3)
   {
      sprintf(trace_str,
	      "ENTERING send_assign (term_id=%s, call_ptr=%x, veh_ptr=%x)\n",
	      term_id, call_ptr, veh_ptr);
      TRACE(trace_str);
   }
#endif

   if ( veh_ptr == NULL )
     {
       //
       return(FAIL);
     }
   
   if (!veh_ptr->t_status.kdt_working)
   {				/* in emulation mode */
      if (!veh_ptr->t_status.accepted)	/* don't generate a manual exception
					 * for accepting the call */
      {				/* sms problem 1062 										 */
	 mk_except(call_ptr, veh_ptr, MANUAL, 0, -1);
	 writer_buf.call_nbr = call_ptr->call_number;
	 /* write out a call * history record */
	 writer_buf.exception_nbr = MANUAL;
	 writer_buf.fleet = veh_ptr->fleet;
	 writer_buf.event_type = EVNT_REG_TYPE;
	 strcpy(writer_buf.event_desc, EVNT_MANUAL);
	 writer_buf.veh_nbr = veh_ptr->veh_nbr;
	 writer_buf.drv_id = veh_ptr->driver_id;
	 writer_buf.user_id = 0;
	 strcpy(writer_buf.action_taken, "");
	 writer_add_hist(&writer_buf);
      }				/* if (!veh_ptr->t_status.accepted ) */
      return (0);
   }
   if (call_ptr == NULL)
   {				/* Call rec not in memory get it */
      nextcall = callnum = veh_ptr->call_nbr;
      c_isam_num = 0;
   } else
   {  /* Call rec in memory procede */
      nextcall = callnum = call_ptr->call_number;
      c_isam_num = call_ptr->c_isam_num;
   }

   mult_num = 0;
   FirstMultSent = 0;
   while (nextcall)
   {
      if ((cl_ptr = get_call_record(c_isam_num, nextcall)) == NULL)
      {
	 /* Log NACK on Trip Details to Call History */
	 if ((call_ptr != NULL))
	   if(bit_check(SYS_ERR,call_ptr) <= 0) /* Check if SYS_ERR exception already occurred */
	     log_assign_nak(SYS_ERR, call_ptr);

	 mk_except(call_ptr, veh_ptr, SYS_ERR, ASSIGN_ERR, -1);
	 return (-1);
      }
      
      if ((0) && (cl_ptr->ext_nbr > NOT_EXTERNAL))       /* get detail type from call_ptr if SamPlan 
										  call is an external call */
	assign_opt = cl_ptr->detail_type;
      else
	assign_opt = fleet[veh_ptr->fleet_nbr]->call_assignment_info;

      if (((cl_ptr->extended_type[1] == INVOICE_CALL) || (cl_ptr->extended_type[1] > '3'))  &&
          (cl_ptr->cash_account != YES) &&
	  (cl_ptr->ext_nbr == NOT_EXTERNAL) &&
	  (strlen(cl_ptr->charge_nbr) > 0) && (0))
	assign_opt = DETAIL10;

      if ((cl_ptr->extended_type[0] == 'T') && (cl_ptr->extended_type[1] == 'H'))
	assign_opt = TH_FORMAT;
      
      if (mult_num == 0 && cl_ptr->grouped_with)
      {
	 total_mult_num = get_num_mult_calls(nextcall, callnum);

	 /* do we need to do this again ?? */
	 cl_ptr = get_call_record(c_isam_num, nextcall);
      }
      c_isam_num = 0;

      if (cl_ptr->grouped_with == callnum)
	 nextcall = 0;
      else
	 nextcall = cl_ptr->grouped_with;

      /** Stockholm patch 980920 - all AUTOBOOK calls are coming in
	  with grouped_with field set to 1.  This causes NAK on trip
	  details and other stuck exceptions problem **/
      if ( nextcall == 1 )
	nextcall = 0;



      mk_outb_text("");		/* to initialize it */



      /* Koge has a distinct format for trip details */
      if ( (0) && ( assign_opt == '2' ) )
	assign_opt = KOGE_FORMAT;

      switch (assign_opt)

      {
      default:
      case S_FORMAT:
	 /* LINES 1 & 2 ****************************************** LINE */
	 (void) mk_pickup_address(assign_opt, cl_ptr, COMPLETE_ADDRESS, 0);  /* 2 lines */


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
	 len += mk_attr_fields(1, ABRIV, call_ptr, veh_ptr);

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
	    mult_num++;
	    sprintf(str_temp, "%s%1d %s %1d", MULTIPLE_CALL_S, mult_num, OF, total_mult_num);
	    len += add_outb_text(str_temp);
	    add_outb_text(CR);
	 }
	 len = 0;

	 /* LINE 5 ********************************************* LINE */
#ifdef NAMERICA
	 if ( cl_ptr->send_destination == YES )
	   {
#endif
         len = add_outb_text(DEST_HDR_S);

	 /* DEST PRE_DIR */
	 if (len = add_outb_text(cl_ptr->dest_pre_dir) != 0)
	    len += add_outb_space();

	 /* STREET NAME - DESTINATION */
	 if (strlen(cl_ptr->dest_str_name))
	 {
	    len += add_outb_text(cl_ptr->dest_str_name);
	 }
	 /* STREET TYPE - DESTINATION */
	 if (strlen(cl_ptr->dest_str_type))
	 {
	    len += add_outb_space();
	    len += add_outb_text(cl_ptr->dest_str_type);
	 }
	 /* DEST POST_DIR */
	 if (strlen(cl_ptr->dest_post_dir))
	 {
	    len += add_outb_space();
	    len += add_outb_text(cl_ptr->dest_post_dir);
	    len += strlen(cl_ptr->dest_post_dir);
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
	 if (cl_ptr->dest_str_nbr)
	 {
	    sprintf(str_temp, "%d", cl_ptr->dest_str_nbr);
	    len += add_outb_text(str_temp);
	    len += add_outb_space();
	 } else if (strlen(cl_ptr->dest_apt_room) || strlen(cl_ptr->dest_dmap))
	    for (ii = 0; ii < sizeof(cl_ptr->dest_str_nbr); ii++)
	       len += add_outb_space();


	 /* APARTMENT NUMBER - DESTINATION */
	 if (add_outb_text(cl_ptr->dest_apt_room))
	 {
	    len += strlen(cl_ptr->dest_apt_room);
	 } else if (strlen(cl_ptr->dest_apt_room) || cl_ptr->dest_str_nbr)
	    for (ii = 0; ii < sizeof(cl_ptr->dest_apt_room); ii++)
	       len += add_outb_space();

	 /* CITY - DESTINATION */
	 if (strlen(cl_ptr->dest_city))
	 {
	    len += add_outb_space();
	    len += add_outb_text(cl_ptr->dest_city);
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
#ifdef NAMERICA
       }
#endif

	 /* LINES 7 - 8 **************************************** LINE */

	 /* COMMENT - GENERAL */
	 mk_comment(cl_ptr);

	 /* LINE 9 ********************************************* LINE */

	 /* ACCOUNT NUMBER */
	 if ( is_autobook_call(cl_ptr) != TRUE )
	   {
	     if (strlen(cl_ptr->charge_nbr))
	       len = add_outb_text(cl_ptr->charge_nbr);
	   }
	 
	 if (strlen(cl_ptr->company_name))
	   len += add_outb_space();

	 if (len < MAX_DISPLAY_LENGTH)
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
#ifdef SCANDINAVIA
	 if (cl_ptr->call_rate != 0.0)
#else
	   if ((cl_ptr->call_rate != 0.0) && (cl_ptr->send_rate == YES ) )
#endif
	   {
	     //	     add_outb_text(CALL_RATE_HDR);
#ifdef SCANDINAVIA
	    sprintf(str_temp, "------------------------------%%RKIINTE� HINTA %7.2f%s%%R------------------------------%%R", cl_ptr->call_rate, " EUR");	    	     
#else
	    sprintf(str_temp, "------------------------------%%RKIINTE� HINTA %7.2f%s%%R------------------------------%%R", cl_ptr->call_rate, " EUR");	    	     
#endif
	     len += add_outb_text(str_temp);
	   } 
	 else if (cl_ptr->advise_arrival == YES)
	   {
	     for (ii = 0; ii < strlen(CALL_RATE_HDR) + 8; ii++)
	       len += add_outb_space();
	   }
	 
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

      case TH_FORMAT:
	 /* LINES 1 & 2 ****************************************** LINE */
	 (void) mk_pickup_address(assign_opt, cl_ptr, COMPLETE_ADDRESS, 0);  /* 2 lines */
	 len = 0;
	 add_outb_text(CR);
	 if (cl_ptr->general_cmnt)
	   mk_comment(cl_ptr);
	 len = 0;
	 if (cl_ptr->passenger_name)
	   {
	     sprintf(str_temp, "%s %s", "ASIAKAS:", cl_ptr->passenger_name);
	     add_outb_text(str_temp);
	     add_outb_text(CR);
	   }

	 len = 0;
	 
	 len += mk_attr_fields(1, ABRIV, call_ptr, veh_ptr);
	 if ( len > 0 )
	   add_outb_text(CR);

	 if (cl_ptr->extended_type[3] == 'L')  // customer is paying in the vehicle not with app
	   {
	     add_outb_text("EI MAKSETA AUTOSSA");
	     add_outb_text(CR);
	     add_outb_text("TEE YHTI�LASKUTUS PERILL�,");
	     add_outb_text(CR);
	     add_outb_text("KUITTI TOIMITETAAN S�HK�ISESTI");
	     add_outb_text(CR);
	   }
	 else
	   {
	     add_outb_text("KYYTI MAKSETAAN AUTOSSA");
	     add_outb_text(CR);
	   }

	 /* STREET NAME - DESTINATION */

	 if (strlen(cl_ptr->dest_str_name) || strlen(cl_ptr->dest_city))
	   {
	     len += add_outb_text("KOHDE:");
	     if (strlen(cl_ptr->dest_str_name))
	       {
		 len += add_outb_text(cl_ptr->dest_str_name);
	       }
	     if (strlen(cl_ptr->dest_str_type))
	       {
		 len += add_outb_space();
		 len += add_outb_text(cl_ptr->dest_str_type);
	       }
	     if (cl_ptr->dest_str_nbr)
	       {
		 sprintf(str_temp, "%d", cl_ptr->dest_str_nbr);
		 len += add_outb_space();
		 len += add_outb_text(str_temp);
	       }
	     if (cl_ptr->dest_city)
	       {
		 len += add_outb_space();
		 len += add_outb_text(cl_ptr->dest_city);
	       }
	     add_outb_text(CR);
	   }
	 /* STREET TYPE - DESTINATION */
	 //	 if (strlen(cl_ptr->dest_str_type))
	 //	 {
	 //	    len += add_outb_space();
	 //	    len += add_outb_text(cl_ptr->dest_str_type);
	 //	 }
	 /* CITY - DESTINATION */
	 //	 if (strlen(cl_ptr->dest_city))
	 //	 {
	 //	    len += add_outb_space();
	 //	    len += add_outb_text(cl_ptr->dest_city);
	 //	 }	 
	 //	 if (len > MAX_DISPLAY_LENGTH)
	 //	 {
	 //	    len -= MAX_DISPLAY_LENGTH;
	 //	    while ((len >= 0) && len--)
	 //	       delete_outb_char();
	 //	 }
	 //	 if (cl_ptr->dest_str_nbr)
	 //	 {
	 //	    sprintf(str_temp, "%d", cl_ptr->dest_str_nbr);
	 //	    len += add_outb_text(str_temp);
	 //	    len += add_outb_space();
	 //	 } else if (strlen(cl_ptr->dest_apt_room) || strlen(cl_ptr->dest_dmap))
	 //	   for (ii = 0; ii < sizeof(cl_ptr->dest_str_nbr); ii++)
	 //	     len += add_outb_space();
	 
	 
	 /* APARTMENT NUMBER - DESTINATION */
	 //	 if (add_outb_text(cl_ptr->dest_apt_room))
	 //	 {
	 //	    len += add_outb_space();
	 //	    len += strlen(cl_ptr->dest_apt_room) + 1;
	 //	 } else if (strlen(cl_ptr->dest_apt_room) || cl_ptr->dest_str_nbr)
	 //	    for (ii = 0; ii < sizeof(cl_ptr->dest_apt_room); ii++)
	 //	       len += add_outb_space();


	 len += add_outb_text(CALL_NUMBER_HDR);
	 sprintf(str_temp, "%06d", cl_ptr->nbr);

	 len += add_outb_text(str_temp);	 

	break;
	
      case H_FORMAT:

	 /* LINES 1 & 2 ****************************************** LINE */
	 (void) mk_pickup_address(assign_opt, cl_ptr, COMPLETE_ADDRESS, 0);  /* 2 lines */

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
	    mult_num++;
	    sprintf(str_temp, "%s%1d %s %1d", MULTIPLE_CALL_H,
		    mult_num, OF, total_mult_num);
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

#ifdef NAMERICA
	 if (cl_ptr->send_destination == YES)
	   {
#endif
	 /* STREET NAME - DESTINATION */
	 if (strlen(cl_ptr->dest_str_name))
	 {
	    len += add_outb_text(DEST_HDR_H);
	    len += add_outb_text(cl_ptr->dest_str_name);
	 }
	 /* STREET TYPE - DESTINATION */
	 if (strlen(cl_ptr->dest_str_type))
	 {
	    len += add_outb_space();
	    len += add_outb_text(cl_ptr->dest_str_type);
	 }
	 /* CITY - DESTINATION */
	 if (strlen(cl_ptr->dest_city))
	 {
	    len += add_outb_space();
	    len += add_outb_text(cl_ptr->dest_city);
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
	 if (cl_ptr->dest_str_nbr)
	 {
	    sprintf(str_temp, "%d", cl_ptr->dest_str_nbr);
	    len += add_outb_text(str_temp);
	    len += add_outb_space();
	 } else if (strlen(cl_ptr->dest_apt_room) || strlen(cl_ptr->dest_dmap))
	    for (ii = 0; ii < sizeof(cl_ptr->dest_str_nbr); ii++)
	       len += add_outb_space();


	 /* APARTMENT NUMBER - DESTINATION */
	 if (add_outb_text(cl_ptr->dest_apt_room))
	 {
	    len += add_outb_space();
	    len += strlen(cl_ptr->dest_apt_room) + 1;
	 } else if (strlen(cl_ptr->dest_apt_room) || cl_ptr->dest_str_nbr)
	    for (ii = 0; ii < sizeof(cl_ptr->dest_apt_room); ii++)
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
#ifdef NAMERICA
       }
#endif

	 /* LINES 6 - 7 **************************************** LINE */

	 /* COMMENT - GENERAL */
	 mk_comment(cl_ptr);

	 /* LINE 8 ********************************************* LINE */

	 /* VEHICLE and DRIVER ATTRIBUTES */
	 len += mk_attr_fields(1, ABRIV, call_ptr, veh_ptr);

	 if (len)
	    len += add_outb_space();

	 /* ACCOUNT NUMBER */
	 if ( is_autobook_call(cl_ptr) != TRUE )
	 {
	     if (strlen(cl_ptr->charge_nbr))
	        len += add_outb_text(cl_ptr->charge_nbr);
	 }
	 if ( (0) &&
	     ( strlen( cl_ptr->charge_nbr ) ) &&
	     ( is_autobook_call(cl_ptr) != TRUE ) )
	   len += add_outb_text(" KONTO");

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
#ifdef SCANDINAVIA
	 if (cl_ptr->call_rate != 0.0)
#else
	 if (( cl_ptr->call_rate != 0.0) && ( cl_ptr->send_rate == YES ) )
#endif
	 {
	   //	    add_outb_text(CALL_RATE_HDR);
#ifdef SCANDINAVIA
	    sprintf(str_temp, "------------------------------%%RKIINTE� HINTA %7.2f%s%%R------------------------------%%R", cl_ptr->call_rate, " EUR");	    
#else
	    sprintf(str_temp, "------------------------------%%RKIINTE� HINTA %7.2f%s%%R------------------------------%%R", cl_ptr->call_rate, " EUR");	    	    

#endif
	    len += add_outb_text(str_temp);
	 }
	 else
	   {
	     for (ii = 0; ii < strlen(CALL_RATE_HDR) + 8; ii++)
	       len += add_outb_space();
	   }
	 len += add_outb_space();

        if (cl_ptr->pod_required == YES)
        {
          len +=  add_outb_text(PROOF_OF_DELIEVERY);
          len += add_outb_space();
        }
        else
          for (ii = 0; ii < strlen(PROOF_OF_DELIEVERY); ii++)
	     len += add_outb_space();


	 /* TRIP NUMBER */
	 len += add_outb_text(CALL_NUMBER_HDR);

	 if ((0) && (cl_ptr->ext_nbr > 0)) /* If FTJ-call replace call number with FTJ_nbr */
	   sprintf(str_temp, "%6d", cl_ptr->ext_nbr);
	 else
	   sprintf(str_temp, "%06d", cl_ptr->nbr);

	 len += add_outb_text(str_temp);

	 // add an invisible field on MPK trips for HTD
	 bSendKela = FALSE;
	 veh_ptr->kela_node = 0;
	 
	 if ( !strncmp(cl_ptr->extended_type, "KE", 2) )
	   {
	     // READ KELA NODE TABLE

	     countKelaNodes = 0;
	     nodePairs = 0;
	     db_open(KELANODE_FILE_ID, ISINOUT | ISMANULOCK);
	     bzero(&kelanode_rec, sizeof(KELANODE_REC));
	     kelanode_rec.tpak_id = cl_ptr->nbr;
	     kelanode_rec.nbr = 1;
	     kelanode_rec.version = 0;
	     dbmode = ISEQUAL;
	     bzero(kela_details_temp, TMP_STR_LEN);
	     while (db_read_key(KELANODE_FILE_ID, &kelanode_rec, &kelanode_key3, dbmode) == SUCCESS)
	       {
		 if (kelanode_rec.version != 0)
		   continue;
		 if (countKelaNodes > 6)
		   break;
		 if (kelanode_rec.tpak_id != cl_ptr->nbr)
		   break;
		 if (countKelaNodes==0) {
		   sprintf(kela_details_temp, kelanode_rec.rte_id);
		   strcpy(theRouteID, kelanode_rec.rte_id);
		 }
		 
		 dbmode = ISNEXT;
		 if (!(countKelaNodes % 2))
		   {
		     nodePairs++;
		     sprintf(kela_details_temp, "%s;%02x;%02x;%s;x", kela_details_temp,
			     nodePairs,
			     nodePairs,
			     kelanode_rec.passenger);
		   }
		 countKelaNodes++;
	       }  
	     if (countKelaNodes > 0)
	       {
		 veh_ptr->kela_node = 1;		 
		 sprintf(str_temp, "%s%01X;1;%s;%s.",
			 "%.K",
			 nodePairs,
			 theRouteID,
                         theRouteID
                 );
		 len += add_outb_text(str_temp);
		 bSendKela = TRUE;
	       }
	     db_close(KELANODE_FILE_ID);
	   }
#ifdef WAIT_FOR_DDS
	 if ( !strncmp(cl_ptr->extended_type, "MPK", 3) )	 
	   {
	     sMPKnode = strstr(cl_ptr->dest_str_name, "MIN:1");
	     if ( sMPKnode != NULL )
	       {
		 sprintf(str_temp, "%s%04X1%c.", "%.L", cl_ptr->nbr%65535, *(sMPKnode+6));
		 len += add_outb_text(str_temp);
	       }
	   }
#endif
	 
#ifdef EXTRA_FIELDS
	 /* ADVISE ARRIVAL */
	 if (cl_ptr->advise_arrival == YES)
	    add_outb_text(ADVISE_ARRIVAL_HDR);
#endif

	 break;

      case DETAIL2 :
	 if (0){

	   mk_sp_details2(cl_ptr, ++mult_num, total_mult_num);

	   break;
	 }
      case DETAIL3 :
      case DETAIL4 :
      case DETAIL5 :
      case DETAIL6 :
      case DETAIL7 :
      case DETAIL8 :
      case DETAIL9 :
	 if (0){
	   for (ii = 0; ii < 10; ii++)
	     add_outb_text(CR);
	   break;
	 }
       case DETAIL10 :
	 mk_sp_details10(cl_ptr);
	 break;

       case KOGE_FORMAT:
	 /* LINES 1, 2, & 3 */
	 len = 0;
	 (void) mk_pickup_address( assign_opt, cl_ptr, COMPLETE_ADDRESS, 0 );

	 /* LINE 4 & 5 - Account Info */
	 len = 0;
	 if ( strlen(cl_ptr->charge_nbr) )
	   {
	     if ( strlen( cl_ptr->company_name ) )
	       {
		 add_outb_text(cl_ptr->company_name);
		 add_outb_text(CR);
	       }

	     len = 0;
	     /* LINE 5 - used only on non-autobooking trips or flat rate */
	     if ( is_autobook_call(cl_ptr) != TRUE )
	       {
		 sprintf( str_temp, "%s %s", "KONTO", cl_ptr->charge_nbr );
		 len = add_outb_text( str_temp );
	       }
	     if ( cl_ptr->call_rate != 0.0 )
	       {
		 while ( len != KOGE_COL2_START_2 - 3 )
		   len += add_outb_space();
		 len += add_outb_text(CALL_RATE_HDR);
		 sprintf( str_temp, "%7.2f%s", cl_ptr->call_rate, currency_symbol );
		 len += add_outb_text( str_temp );
	       }
	   }
	 else
	   {
	     if ( strlen(cl_ptr->passenger_name) )
	       {
		 add_outb_text(cl_ptr->passenger_name);
		 add_outb_text(CR);
	       }
	     len = 0;
	     if ( cl_ptr->call_rate != 0.0 )
	       {
		 while ( len != KOGE_COL2_START_2 - 3 )
		   len += add_outb_space();
		 len += add_outb_text(CALL_RATE_HDR);
		 sprintf( str_temp, "%7.2f%s", cl_ptr->call_rate, currency_symbol );
		 len += add_outb_text( str_temp );
	       }
	   }
	     
	 if ( len != 0 )
	   add_outb_text( CR );

	 /* LINE 6 - Attributes and trip number */
	 len = 0;
	 if ( is_autobook_call( cl_ptr ) )
	   len += add_outb_text("AB ");
	 len += mk_attr_fields(1, ABRIV, call_ptr, veh_ptr);
	 while ( len != KOGE_COL2_START_2 )
	   len += add_outb_space();
	 sprintf( str_temp, "%s %06d%s", CALL_NUMBER_HDR, cl_ptr->nbr, CR );
	 add_outb_text( str_temp );

	 /* LINE 7 - General comment */
	 len = 0;
	 mk_comment(cl_ptr);

	 len = 0;
	 /* LINE 8 - Destination placename */
	 bAddDestHdr = FALSE;
	 if ( strlen( cl_ptr->dest_adr_cmnt ) )
	   {
	     len = add_outb_text(DEST_HDR_H);
	     len += add_outb_text( cl_ptr->dest_adr_cmnt );
	     add_outb_text( CR );
	     bAddDestHdr = TRUE;
	   }

	 len = 0;
	 /* LINE 9 - Destination street name and number */
	 if ( strlen(cl_ptr->dest_str_name) )
	   {
	     if ( bAddDestHdr == FALSE )
	       len = add_outb_text(DEST_HDR_H);
	     len += add_outb_text(cl_ptr->dest_str_name);
	   }
	 if ( cl_ptr->dest_str_nbr )
	   {
	     while ( len != KOGE_COL2_START_1 )
	       len += add_outb_space();
	     sprintf(str_temp, "%d", cl_ptr->dest_str_nbr);
	     len += add_outb_text( str_temp );
	   }
	 if ( len != 0 )
	   add_outb_text( CR );

	 len = 0;
	 /* LINE 10 - Destination city and coordinated */
	 if ( strlen( cl_ptr->dest_city ) )
	   len = add_outb_text( cl_ptr->dest_city );
	 if ( strlen( cl_ptr->dest_dmap ) )
	   {
	     while ( len != KOGE_COL2_START_2 )
	       len += add_outb_space();
	     len += add_outb_text( cl_ptr->dest_dmap );
	   }
	 if ( len != 0 )
	   add_outb_text( CR );

	 len = 0;
	 /* MULTIPLE CALL INDICATOR */
	 if (cl_ptr->grouped_with)
	 {
	    mult_num++;
	    sprintf(str_temp, "%s%1d %s %1d", MULTIPLE_CALL_H,
		    mult_num, OF, total_mult_num);
	    len += add_outb_text(str_temp);
	 }
       
      }                         /* end case */

      //      if ( (char)Veh_get_value( (VEH_HNDL) veh_ptr, VEH_NAV_TYPE ) == STAR2000_MDT )
      //        {
          bzero( bcast_buf, 512 );
          call_inx = cl_ptr->gps_long;
          call_iny = cl_ptr->gps_lat;
          if ( call_inx > 0.0 && call_iny > 0.0 )
            {

	      UTM_to_LatLong( call_iny, call_inx, &call_outy, &call_outx );
	      
              sprintf( bcast_buf, "%%.XQ+%07d+%08d.",
                       (int)(call_outy*100000), (int)(call_outx*100000) );
              add_outb_text( bcast_buf );
            }
          bzero( bcast_buf, 512 );
          call_inx = cl_ptr->dest_gpsx;
          call_iny = cl_ptr->dest_gpsy;
          if ( call_inx > 0.0 && call_iny > 0.0 )
            {
	      
	      UTM_to_LatLong( call_iny, call_inx, &call_outy, &call_outx );
	      
              sprintf( bcast_buf, "%%.XR+%07d+%08d.",
                       (int)(call_outy*100000), (int)(call_outx*100000) );
              add_outb_text( bcast_buf );
            }          
	  //        }
      
      send_pkt_id(ASSIGN_PKT_TYPE, 0,
		  (int)Veh_get_value((VEH_HNDL)veh_ptr, VEH_NBR));
      if (mult_num > 1)
	 status = send_msg_mmp(term_id, ASSIGN_DISPLAY_P1, veh_ptr);
      else
	 status = send_msg_mmp(term_id, ASSIGN_DISPLAY_P3, veh_ptr);
      if (status)
      {
	 /* Log NACK on Trip Details to Call History */
	 if ((call_ptr != NULL))
	   if(bit_check(SYS_ERR,call_ptr) <= 0) /* Check if SYS_ERR exception already occurred */
	     log_assign_nak(SYS_ERR, call_ptr);

	 sprintf(str_temp, "unable to send assignment to %s", veh_ptr->mid);
	 ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_temp);
	 mk_except((struct calls *) veh_ptr->call_ptr, veh_ptr, SYS_ERR, ASSIGN_ERR, -1);
       }
      clr_pkt_id();

      // KELA DETAILS MESSAGE
      // send_msg_mmp(term_id, KELA_DETAILS , veh_ptr);
      if (bSendKela)
	{
	      db_open(KELANODE_FILE_ID, ISINOUT | ISMANULOCK);
	      bzero(&kelanode_rec, sizeof(KELANODE_REC));
	      kelanode_rec.tpak_id = cl_ptr->nbr;
	      kelanode_rec.nbr = 1;
	      kelanode_rec.version = 0;

	      bzero(kela_details_temp, TMP_STR_LEN);
	      bzero(kela_resp_temp, TMP_STR_LEN);
	      //PICKUP node
	      if (db_read_key(KELANODE_FILE_ID, &kelanode_rec, &kelanode_key3, ISEQUAL) == SUCCESS)
		{
		  sm_put(sm, kelanode_rec.full_booking_id, kelanode_rec.rte_id);
		  strcpy(tmp_pax, kelanode_rec.passenger);
		  if (strstr(tmp_pax,"/") != NULL)
		    {
		      *(strstr(tmp_pax,"/")) = ' ';
		    }
		  
		  sprintf(kela_rte_id, "%s",kelanode_rec.rte_id);
		  
		  if (update)
		    sprintf(kela_details_temp, "REITTIMUUTOS! (1/%d)%sKELA ID: %s%s%s:%s %d %s %s%sAIKA: %s%sASIAKAS: %s,%s%s%s%s",
			    countKelaNodes, CR,
			    kelanode_rec.booking_id, CR, (kelanode_rec.type=='D'?"KOHDE":"NOUTO"),
			    kelanode_rec.street, kelanode_rec.streetnbr, kelanode_rec.streetletter, kelanode_rec.city, CR,
			    (strlen(kelanode_rec.pickuptime)==19?(kelanode_rec.pickuptime+strlen(kelanode_rec.pickuptime) - 8):"00:00"),CR,
			    kelanode_rec.passenger, kelanode_rec.phone, CR,
			    kelanode_rec.manual_descript, CR);
		  else
		    sprintf(kela_details_temp, "(1/%d)%sKELA ID: %s%s%s:%s %d %s %s%sAIKA: %s%sASIAKAS: %s,%s%s%s%s",
			    countKelaNodes, CR,
			    kelanode_rec.booking_id, CR, (kelanode_rec.type=='D'?"KOHDE":"NOUTO"),
			    kelanode_rec.street, kelanode_rec.streetnbr, kelanode_rec.streetletter, kelanode_rec.city, CR,
			    (strlen(kelanode_rec.pickuptime)==19?(kelanode_rec.pickuptime+strlen(kelanode_rec.pickuptime) - 8):"00:00"),CR,
			    kelanode_rec.passenger, kelanode_rec.phone, CR,
			    kelanode_rec.manual_descript, CR);
		  
		  sprintf(kela_resp_temp, "%s;%02x;%020s;%s;x;", kela_resp_temp,
			  kelanode_rec.nbr,
			  kelanode_rec.full_booking_id,
			  tmp_pax);
		  mk_outb_text("");
		  add_outb_text(kela_details_temp);
		  send_msg_mmp(veh_ptr->mid, TEXT_DISPLAY, veh_ptr);
		  kela_details_temp[0] = '\0';
		}
	      // node 2
	      kelanode_rec.nbr++;
	      if (db_read_key(KELANODE_FILE_ID, &kelanode_rec, &kelanode_key3, ISEQUAL) == SUCCESS)
		{

		  strcpy(tmp_pax, kelanode_rec.passenger);
		  if (strstr(tmp_pax,"/") != NULL)
		    {
		      *(strstr(tmp_pax,"/")) = ' ';
		    }

		  if (update)
		    sprintf(kela_details_temp, "%sREITTIMUUTOS! (%d/%d)%sKELA ID: %s%s%s: %s %d %s %s%sAIKA: %s%sASIAKAS: %s,%s%s%s",
			    kela_details_temp,
			    kelanode_rec.nbr, countKelaNodes, CR,
			    kelanode_rec.booking_id, CR, (kelanode_rec.type=='D'?"KOHDE":"NOUTO"),
			    kelanode_rec.street, kelanode_rec.streetnbr, kelanode_rec.streetletter, kelanode_rec.city, CR,
			    (strlen(kelanode_rec.pickuptime)==19?(kelanode_rec.pickuptime+strlen(kelanode_rec.pickuptime) - 8):"00:00"),CR,	  
			    kelanode_rec.passenger, kelanode_rec.phone, CR,
			    kelanode_rec.manual_descript);
		  else
		    sprintf(kela_details_temp, "%s(%d/%d)%sKELA ID: %s%s%s: %s %d %s %s%sAIKA: %s%sASIAKAS: %s,%s%s%s",
			    kela_details_temp,
			    kelanode_rec.nbr, countKelaNodes, CR,
			    kelanode_rec.booking_id, CR, (kelanode_rec.type=='D'?"KOHDE":"NOUTO"),
			    kelanode_rec.street, kelanode_rec.streetnbr, kelanode_rec.streetletter, kelanode_rec.city, CR,
			    (strlen(kelanode_rec.pickuptime)==19?(kelanode_rec.pickuptime+strlen(kelanode_rec.pickuptime) - 8):"00:00"),CR,	  
			    kelanode_rec.passenger, kelanode_rec.phone, CR,
			    kelanode_rec.manual_descript);
		  
		  if (!sm_get(sm, kelanode_rec.full_booking_id, buf, sizeof(buf)))
		    {
		      sprintf(kela_resp_temp, "%s%02x;%020s;%s;x;", kela_resp_temp,
			      kelanode_rec.nbr,
			      kelanode_rec.full_booking_id,
			      tmp_pax);
		      sm_put(sm, kelanode_rec.full_booking_id, kelanode_rec.rte_id);
		    }

		  mk_outb_text("");
		  add_outb_text(kela_details_temp);
		  send_msg_mmp(veh_ptr->mid, TEXT_DISPLAY, veh_ptr);
		  kela_details_temp[0] = '\0';
		  
		}
	      
	      // node 3
	      kelanode_rec.nbr++;
	      if (db_read_key(KELANODE_FILE_ID, &kelanode_rec, &kelanode_key3, ISEQUAL) == SUCCESS)
		{
		  strcpy(tmp_pax, kelanode_rec.passenger);
		  if (strstr(tmp_pax,"/") != NULL)
		    {
		      *(strstr(tmp_pax,"/")) = ' ';
		    }
		  if (update)
		    sprintf(kela_details_temp, "%sREITTIMUUTOS! (%d/%d)%sKELA ID: %s%s%s: %s %d %s %s%sAIKA: %s%sASIAKAS: %s,%s%s%s",
			    kela_details_temp,
			    kelanode_rec.nbr, countKelaNodes, CR,
			    kelanode_rec.booking_id, CR, (kelanode_rec.type=='D'?"KOHDE":"NOUTO"),		  
			    kelanode_rec.street, kelanode_rec.streetnbr, kelanode_rec.streetletter, kelanode_rec.city, CR,
			    (strlen(kelanode_rec.pickuptime)==19?(kelanode_rec.pickuptime+strlen(kelanode_rec.pickuptime) - 8):"00:00"),CR,	  
			    kelanode_rec.passenger, kelanode_rec.phone, CR,
			    kelanode_rec.manual_descript);
		  else
		    sprintf(kela_details_temp, "%s(%d/%d)%sKELA ID: %s%s%s: %s %d %s %s%sAIKA: %s%sASIAKAS: %s,%s%s%s",
			    kela_details_temp,
			    kelanode_rec.nbr, countKelaNodes, CR,
			    kelanode_rec.booking_id, CR, (kelanode_rec.type=='D'?"KOHDE":"NOUTO"),		  
			    kelanode_rec.street, kelanode_rec.streetnbr, kelanode_rec.streetletter, kelanode_rec.city, CR,
			    (strlen(kelanode_rec.pickuptime)==19?(kelanode_rec.pickuptime+strlen(kelanode_rec.pickuptime) - 8):"00:00"),CR,	  
			    kelanode_rec.passenger, kelanode_rec.phone, CR,
			    kelanode_rec.manual_descript);		  
		  
		  if (!sm_get(sm, kelanode_rec.full_booking_id, buf, sizeof(buf)))
		    {
		      sprintf(kela_resp_temp, "%s%02x;%020s;%s;x;", kela_resp_temp,
			      kelanode_rec.nbr,
			      kelanode_rec.full_booking_id,
			      tmp_pax);
		      sm_put(sm, kelanode_rec.full_booking_id, kelanode_rec.rte_id);
		    }		  

		  mk_outb_text("");
		  add_outb_text(kela_details_temp);
		  send_msg_mmp(veh_ptr->mid, TEXT_DISPLAY, veh_ptr);
		  kela_details_temp[0] = '\0';		  
		}		  
	      // Node 4
	      kelanode_rec.nbr++;
	      if (db_read_key(KELANODE_FILE_ID, &kelanode_rec, &kelanode_key3, ISEQUAL) == SUCCESS)
		{
		  
		  strcpy(tmp_pax, kelanode_rec.passenger);
		  if (strstr(tmp_pax,"/") != NULL)
		    {
		      *(strstr(tmp_pax,"/")) = ' ';
		    }
		  if (update)
		    sprintf(kela_details_temp, "%sREITTIMUUTOS! (%d/%d)%sKELA ID: %s%s%s: %s %d %s %s%sAIKA: %s%sASIAKAS: %s,%s%s%s",
			    kela_details_temp,
			    kelanode_rec.nbr, countKelaNodes, CR,
			    kelanode_rec.booking_id, CR, (kelanode_rec.type=='D'?"KOHDE":"NOUTO"),
			    kelanode_rec.street, kelanode_rec.streetnbr, kelanode_rec.streetletter, kelanode_rec.city, CR,
			    (strlen(kelanode_rec.pickuptime)==19?(kelanode_rec.pickuptime+strlen(kelanode_rec.pickuptime) - 8):"00:00"),CR,      
			    kelanode_rec.passenger, kelanode_rec.phone, CR,
			    kelanode_rec.manual_descript);
		  else
		    sprintf(kela_details_temp, "%s(%d/%d)%sKELA ID: %s%s%s: %s %d %s %s%sAIKA: %s%sASIAKAS: %s,%s%s%s",
			    kela_details_temp,
			    kelanode_rec.nbr, countKelaNodes, CR,
			    kelanode_rec.booking_id, CR, (kelanode_rec.type=='D'?"KOHDE":"NOUTO"),
			    kelanode_rec.street, kelanode_rec.streetnbr, kelanode_rec.streetletter, kelanode_rec.city, CR,
			    (strlen(kelanode_rec.pickuptime)==19?(kelanode_rec.pickuptime+strlen(kelanode_rec.pickuptime) - 8):"00:00"),CR,      
			    kelanode_rec.passenger, kelanode_rec.phone, CR,
			    kelanode_rec.manual_descript);			
		  
		  if (!sm_get(sm, kelanode_rec.full_booking_id, buf, sizeof(buf)))
		    {
		      sprintf(kela_resp_temp, "%s%02x;%020s;%s;x;", kela_resp_temp,
			      kelanode_rec.nbr,
			      kelanode_rec.full_booking_id,
			      tmp_pax);
		      sm_put(sm,kelanode_rec.full_booking_id, kelanode_rec.rte_id);
		    }		  		      
		  
		  mk_outb_text("");
		  add_outb_text(kela_details_temp);
		  send_msg_mmp(veh_ptr->mid, TEXT_DISPLAY, veh_ptr);
		  kela_details_temp[0] = '\0';		      
		}

	      // Node 5
	      kelanode_rec.nbr++;
	      if (db_read_key(KELANODE_FILE_ID, &kelanode_rec, &kelanode_key3, ISEQUAL) == SUCCESS)
		{
		  
		  strcpy(tmp_pax, kelanode_rec.passenger);
		  if (strstr(tmp_pax,"/") != NULL)
		    {
		      *(strstr(tmp_pax,"/")) = ' ';
		    }
		  if (update)
		    sprintf(kela_details_temp, "%sREITTIMUUTOS! (%d/%d)%sKELA ID: %s%s%s: %s %d %s %s%sAIKA: %s%sASIAKAS: %s,%s%s%s",
			    kela_details_temp,
			    kelanode_rec.nbr, countKelaNodes, CR,
			    kelanode_rec.booking_id, CR, (kelanode_rec.type=='D'?"KOHDE":"NOUTO"),
			    kelanode_rec.street, kelanode_rec.streetnbr, kelanode_rec.streetletter, kelanode_rec.city, CR,
			    (strlen(kelanode_rec.pickuptime)==19?(kelanode_rec.pickuptime+strlen(kelanode_rec.pickuptime) - 8):"00:00"),CR,      
			    kelanode_rec.passenger, kelanode_rec.phone, CR,
			    kelanode_rec.manual_descript);
		  else
		    sprintf(kela_details_temp, "%s(%d/%d)%sKELA ID: %s%s%s: %s %d %s %s%sAIKA: %s%sASIAKAS: %s,%s%s%s",
			    kela_details_temp,
			    kelanode_rec.nbr, countKelaNodes, CR,
			    kelanode_rec.booking_id, CR, (kelanode_rec.type=='D'?"KOHDE":"NOUTO"),
			    kelanode_rec.street, kelanode_rec.streetnbr, kelanode_rec.streetletter, kelanode_rec.city, CR,
			    (strlen(kelanode_rec.pickuptime)==19?(kelanode_rec.pickuptime+strlen(kelanode_rec.pickuptime) - 8):"00:00"),CR,      
			    kelanode_rec.passenger, kelanode_rec.phone, CR,
			    kelanode_rec.manual_descript);			
		  
		  if (!sm_get(sm, kelanode_rec.full_booking_id, buf, sizeof(buf)))
		    {
		      sprintf(kela_resp_temp, "%s%02x;%020s;%s;x;", kela_resp_temp,
			      kelanode_rec.nbr,
			      kelanode_rec.full_booking_id,
			      tmp_pax);
		      sm_put(sm,kelanode_rec.full_booking_id, kelanode_rec.rte_id);
		    }		  		      
		  
		  mk_outb_text("");
		  add_outb_text(kela_details_temp);
		  send_msg_mmp(veh_ptr->mid, TEXT_DISPLAY, veh_ptr);
		  kela_details_temp[0] = '\0';		      
		}

	      // Node 6
	      kelanode_rec.nbr++;
	      if (db_read_key(KELANODE_FILE_ID, &kelanode_rec, &kelanode_key3, ISEQUAL) == SUCCESS)
		{
		  
		  strcpy(tmp_pax, kelanode_rec.passenger);
		  if (strstr(tmp_pax,"/") != NULL)
		    {
		      *(strstr(tmp_pax,"/")) = ' ';
		    }
		  if (update)
		    sprintf(kela_details_temp, "%sREITTIMUUTOS! (%d/%d)%sKELA ID: %s%s%s: %s %d %s %s%sAIKA: %s%sASIAKAS: %s,%s%s%s",
			    kela_details_temp,
			    kelanode_rec.nbr, countKelaNodes, CR,
			    kelanode_rec.booking_id, CR, (kelanode_rec.type=='D'?"KOHDE":"NOUTO"),
			    kelanode_rec.street, kelanode_rec.streetnbr, kelanode_rec.streetletter, kelanode_rec.city, CR,
			    (strlen(kelanode_rec.pickuptime)==19?(kelanode_rec.pickuptime+strlen(kelanode_rec.pickuptime) - 8):"00:00"),CR,      
			    kelanode_rec.passenger, kelanode_rec.phone, CR,
			    kelanode_rec.manual_descript);
		  else
		    sprintf(kela_details_temp, "%s(%d/%d)%sKELA ID: %s%s%s: %s %d %s %s%sAIKA: %s%sASIAKAS: %s,%s%s%s",
			    kela_details_temp,
			    kelanode_rec.nbr, countKelaNodes, CR,
			    kelanode_rec.booking_id, CR, (kelanode_rec.type=='D'?"KOHDE":"NOUTO"),
			    kelanode_rec.street, kelanode_rec.streetnbr, kelanode_rec.streetletter, kelanode_rec.city, CR,
			    (strlen(kelanode_rec.pickuptime)==19?(kelanode_rec.pickuptime+strlen(kelanode_rec.pickuptime) - 8):"00:00"),CR,      
			    kelanode_rec.passenger, kelanode_rec.phone, CR,
			    kelanode_rec.manual_descript);			
		  
		  if (!sm_get(sm, kelanode_rec.full_booking_id, buf, sizeof(buf)))
		    {
		      sprintf(kela_resp_temp, "%s%02x;%020s;%s;x;", kela_resp_temp,
			      kelanode_rec.nbr,
			      kelanode_rec.full_booking_id,
			      tmp_pax);
		      sm_put(sm,kelanode_rec.full_booking_id, kelanode_rec.rte_id);
		    }		  		      
		  
		  mk_outb_text("");
		  add_outb_text(kela_details_temp);
		  send_msg_mmp(veh_ptr->mid, TEXT_DISPLAY, veh_ptr);
		  kela_details_temp[0] = '\0';		      
		}	      	      
	      
	      

	      sm_delete(sm);
	      db_close(KELANODE_FILE_ID);
	      //	      mk_outb_text("");
	      //	      add_outb_text(kela_details_temp);
	      //	      send_msg_mmp(veh_ptr->mid, TEXT_DISPLAY, veh_ptr);

	      // KELA_RESP
	      mk_outb_text("");

	      add_outb_text(kela_rte_id); add_outb_text(kela_resp_temp);
	      send_msg_mmp(term_id, KELA_DETAILS, veh_ptr);      
	  
	}
    }


   /** Set the ACK flag on call messages to NO. This does not apply to Taxi Stockholm
       M_ACTION messages which are sent prior to the trip details **/
   Call_msg_reset_ack( (VEH_HNDL)veh_ptr );
   
#ifdef DEBUG
   if (offset->prattle >= LEVEL6)
     TRACE("LEAVING send_assign\n");
#endif
   return (0);
 }				/* end send_assign */


