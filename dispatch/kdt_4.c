static char sz__FILE__[]="@(#)kdt_4.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  kdt_4.c; Rel: 6.41.0.0; Get date: 1/28/92 at 14:03:29
* @(#)  Last delta: 2/27/91 at 15:25:54
* @(#)  SCCS File: /taxi/sccs/s.kdt_4.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

/* Based on R3 version 5.42 */

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
#include "Timer_public.h"
#include "dispatch_strdefs.h"
#include "path.h"
#include "sprintf.h"
#include "mads_types.h"
#include "mad_error.h"
#include "mads_isam.h"
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
#include "enhance.h"
#include "switch_ext.h"
#include "List_public.h"
#include "Vehicle_private.h"
#include "Zone_public.h"
#include "Zone_lists_public.h"
#include "TimeCalls_public.h"
#include "TimeCalls_private.h"

#define MSGSIZE		MMP_TEXT_SIZE

extern int udp_out_fd;
#ifdef PIPES
#define WRITE_TIME_OUT 2
extern outb_fd;
#ifdef DUAL_PORTS
extern outb_fd_2;
#endif

#else
extern int send_msgqid;
#endif

extern struct cisam_ch writer_buf;
struct cisam_cl cisam_call;
extern time_t mads_time;	/* system time */
extern char buf[];
extern char req_buf[];
extern struct veh_driv *get_veh_id();
extern struct fleets *fleet[FLEET_MAX];	/* shared memory fleet structure */
extern struct offsets *offset;	/* pointer to shared memory offsets */
extern char *scratch_ptr;
char *last_six();
void  delete_outb_char();
void  mk_outb_header();
void  terminate_outb_text();
void  mk_msg_priority();
void  config_outb_msg();
void  dump_outb_msg();
void  t_out_hdlr();

static char str_temp[TMP_STR_LEN];
jmp_buf env;

/* how many time we timeout before the msg is lost */
#define OUTB_RETRY_MAX 2
int retry_cnt;
/* how many message in a row are lost befor we mk an exception */
#define MAX_OUTB_LOST 20
int lost_outb;
int   incount = 0;
int   outcount = 0;
int   out_errors = 0;
int   max_rate = 0;
MSG_QUEUE_HDR outb;
#ifdef DEBUG
extern int ok_stats_flg;
int   timecount = 0;

int   msg_count1 = 0;		/* current  minute */
int limit = 1;

extern int response_cnts;
extern int max_response_time;
extern int total_time; 

void  dump_outb(void);
#endif

/*****************************************************************************/
/* */
/* Message send routine                                                     */
/* */
/* This routine performs all of the msgsnd's in kdt_4.c                     */
/* */
/*****************************************************************************/

#ifndef PIPES
static int
msgsend(msg_qid, send_struct, num_bytes, action, veh_ptr)
   int   msg_qid;
   MSG_QUEUE_HDR *send_struct;
   int   num_bytes;
   int   action;
   struct veh_driv *veh_ptr;
{
   int   result = 0;
   struct msqid_ds infoque;

#ifdef DEBUG
   if (offset->prattle >= LEVEL6)
   {
      sprintf(trace_str,
	      "ENTERING msgsend (msg_qid=%d, send_struct=%x, num_bytes %= d, action %= d, veh_ptr %= x) \ n ",
	      msg_qid, send_struct, num_bytes, action, veh_ptr);
      TRACE(trace_str);
   }
   if (offset->prattle == -10)
   {
      record('o', send_struct, num_bytes + sizeof(long));
   }
#endif

   if (offset->status.ncp)
   {				/* send only if acc is up */
      if (veh_ptr != NULL)
      {				/* allow fleetwides without a veh_ptr */
	 /* only do send if acc is up, and no emergency */
	 if (!veh_ptr->t_status.kdt_working || (veh_ptr->t_status.emer))
	    return (0);
	 if (!fleet[veh_ptr->fleet_nbr]->criteria.ncp_on)
	    return (0);
      }
      if (msgctl(msg_qid, IPC_STAT, &infoque) < 0)	/* probably BOGUS MRB */
      {
	 if (veh_ptr != NULL)
	 {			/* allow fleetwides without a veh_ptr */
	    sprintf(scratch_ptr,
		    "can't write to output queue, full, msg to vehicle %d",
		    veh_ptr->veh_nbr);
	    ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, " ", scratch_ptr);
	 } else
	    ERROR(' ', " ", "can't write to output queue, full");

	 return (-1);
      }
      if (infoque.msg_qnum > 60)/* 60 is BOGUS MRB */
      {
	 offset->status.ncp = 0;
	 return (-1);
      }
      result = msgsnd(msg_qid, (struct msgbuf *) send_struct, num_bytes, action);
   }
#ifdef DEBUG
   if (result < 0)
   {
      out_errors++;
      if (ok_stats_flg)
	 printf("async send error %d\n", errno);
#ifdef MORE_DEBUG
      dump_msg_q(msg_qid);
#endif
   }
#endif
   return (result);
}				/* end msgsend */
#endif

/*****************************************************************************/
/* strcopy (string_pointer)                                                 */
/* */
/* This routine will copy a string (like strcpy), but it will return the    */
/* number of characters copied (like strlen).                               */
/* */
/*****************************************************************************/

strcopy(string_ptr_1, string_ptr_2)
   char *string_ptr_1, *string_ptr_2;
{
   int   result;

#ifdef DEBUG
   if (offset->prattle >= LEVEL8)
   {
      sprintf(trace_str, "ENTERING strcopy (string_ptr_2='%s')\n",
	      string_ptr_2);
      TRACE(trace_str);
   }
#endif

   strcpy(string_ptr_1, string_ptr_2);
   result = strlen(string_ptr_1);

#ifdef DEBUG
   if (offset->prattle >= LEVEL8)
   {
      sprintf(trace_str, "LEAVING strcopy (%d)\n", result);
      TRACE(trace_str);
   }
#endif

   return (result);
}				/* end strcopy */

/****************************************************************************/
/* */
/* Outbound Command Processing Flows                                        */
/* */
/* The outbound KDT routines are called either by the KDT inbound/support   */
/* routines (in those instances where an inbound command is fully processed */
/* by the KDT routines or an error is encountered) and by the DISPATCH      */
/* routines.                                                                */
/* */
/* Functionally, there are several types of outbound messages:              */
/* */
/* emergency                          emerg_ack (term_id)                   */
/* registration                       term_reg (term_id)                    */
/* deregistration                     term_dereg (term_id)                  */
/* Authorize Terminal                 authorize(veh_ptr)    	            */
/* Deauthorize Terminal               deauthorize(veh_ptr)    	            */
/* vehicle info                       send_veh_info(term_id, disp_info)     */
/* zone info                          send_zone_info(term_id,zone_no)       */
/* message                            send_msg_mmp (term_id, type  &        */
/* p_field, veh_ptr,      */
/* priority)              */
/* predefined text                    send_text_predefined (term_id,        */
/* text_id, veh_ptr)     */
/* not predefined text                send_text (term_id, text_str,         */
/* veh_ptr)                */
/* clear terminal screen              clear_terminal_screen (term_id,       */
/* device, veh_ptr        */
/* search display                     send_srch_display (fleet_no)          */
/* bid display                        send_bid_display (fleet_no)           */
/* sign on info                       send_signon_info (term_id)            */
/* send queue position		      send_qp(zone_nbr, veh_ptr)    	    */
/* break time limit                   send_brk_time (term_id)               */
/* fleetwide text broadcast           send_fleetwide (fleet_no, text)       */
/****************************************************************************/


authorize(veh_ptr)
   struct veh_driv *veh_ptr;
{
   char  fleet_id[6];
   char  auth_text[256];
   char  attribute_buf[256];
   int   i = 0;
   VEH_REC    veh_rec;
   mk_outb_text("");

#ifndef NAMERICA
   /* new method of terminal authorization combines auth,group_id, and sign_on into
      a single message */
   memcpy(fleet_id, GROUP_MID, MID_SIZE - 1);
   sprintf(fleet_id + MID_SIZE - 1, "%d", veh_ptr->fleet_nbr);
   if ( !(1) )
     sprintf( auth_text, "SIGN ON%%P%%P%%P%%F %s", &fleet_id[2] );
   else
     {
       /* build an extension to the sign on message that includes attribute descriptors */
       bzero( attribute_buf, sizeof( attribute_buf ) );
       /* vehicle attributes comes first */
       for ( i = 0; i < 32; i++ )
	 {
	   if ( test_bit( i, sizeof( veh_ptr->vehicle_attr ), &veh_ptr->vehicle_attr ) )
	     {
	       if ( strlen(fleet[veh_ptr->fleet_nbr]->vd_attr_info[i].veh_attr) < 2 )
		 {
		   strcat( attribute_buf, " " );
		   strcat( attribute_buf, fleet[veh_ptr->fleet_nbr]->vd_attr_info[i].veh_attr );
		 }
	       else
		 strcat( attribute_buf, fleet[veh_ptr->fleet_nbr]->vd_attr_info[i].veh_attr );
	     }
	   else
	     strcat( attribute_buf, "  " );
	 }
       for ( i = 0; i < 32; i++ )
	 {
	   if ( test_bit( i, sizeof( veh_ptr->driver_attr ), &veh_ptr->driver_attr ) )
	     {
	       if ( strlen(fleet[veh_ptr->fleet_nbr]->vd_attr_info[i].drv_attr) < 2 )
		 {
		   strcat( attribute_buf, " " );
		   strcat( attribute_buf, fleet[veh_ptr->fleet_nbr]->vd_attr_info[i].drv_attr );
		 }
	       else
		 strcat( attribute_buf, fleet[veh_ptr->fleet_nbr]->vd_attr_info[i].drv_attr );
	     }
	   else
	     strcat( attribute_buf, "  " );
	 }
       sprintf( auth_text, "SIGN ON%%P%%P%%P%%F %s%%P%s", &fleet_id[2], attribute_buf );
     }

   // Porvoo taksi requires driver mobile telephone sent in authorization message
   if ( fleet[veh_ptr->fleet_nbr]->fleet_id == 'B' )
     {
       bzero(&veh_rec, sizeof(VEH_REC));   
       veh_rec.fleet = fleet[veh_ptr->fleet_nbr]->fleet_id;
       veh_rec.nbr = veh_ptr->veh_nbr;
       if (db_read_key(VEHICLE_FILE_ID, &veh_rec, &veh_key2, DB_EQUAL) == SUCCESS)
	 sprintf( auth_text, "%s%%X%s", auth_text, veh_rec.mdt_tele );
     }
   
   
   mk_outb_text(auth_text);
   send_msg_mmp(veh_ptr->mid, AUTHORIZE, veh_ptr);

   /* send timer configuration message for new MDT S/W */
   if ( (short)Veh_get_value((VEH_HNDL)veh_ptr, VEH_TERM_TYPE) == MDT_V03_ITM_VERS2 )
     {
       sprintf( auth_text, "%%F&%d%%P%%F!%d%%F%%%d%%F\\(%d",
	       fleet[veh_ptr->fleet_nbr]->veh_inquiry_freq, fleet[veh_ptr->fleet_nbr]->posting_freq,
	       fleet[veh_ptr->fleet_nbr]->cond_posting_freq, fleet[veh_ptr->fleet_nbr]->zone_inqry_freq );
       mk_outb_text(auth_text);
       send_msg_mmp(veh_ptr->mid, TIMER_DWNLD, veh_ptr);
     }
#else
   if (send_msg_mmp(veh_ptr->mid, AUTHORIZE, veh_ptr) != 0)
     {
       sprintf(str_temp, "unable to send authorize to %s", veh_ptr->mid);
       ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_temp);
     }

   memcpy(fleet_id, GROUP_MID, MID_SIZE - 1);
   sprintf(fleet_id + MID_SIZE - 1, "%d", veh_ptr->fleet_nbr);
   mk_outb_text(&fleet_id[2]);

   if (send_msg_mmp(veh_ptr->mid, ASGN_GROUP1, veh_ptr) != 0)
     {
       sprintf(str_temp,"unable to send group id to %s",veh_ptr->mid);
       ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_temp);
     }
#endif

 }				/* end authorize */

deauthorize(veh_ptr)
   struct veh_driv *veh_ptr;
{
   char  fleet_id[6];

#ifdef DEBUG
   if (offset->prattle >= LEVEL3)
   {
      sprintf(trace_str, "ENTERING deauthorize (veh_ptr=%x)\n", veh_ptr);
      TRACE(trace_str);
   }
#endif

   /* if (veh_ptr->term_type != MDT_ITM) MRB */
   {
      mk_outb_text("");
      if (send_msg_mmp(veh_ptr->mid, DEAUTHORIZE, veh_ptr) != 0)
      {
         sprintf(str_temp,"unable to send deauthorize to %s",veh_ptr->mid);
         ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_temp);
      }

   }
#ifdef DEBUG
   if (offset->prattle >= LEVEL6)
      TRACE("LEAVING deauthorize\n");
#endif
}				/* end deauthorize */

/*****************************************************************************/
/* */
/* send_zone_info - <INQRY> Terminal Command Response Routine              */
/* */
/*****************************************************************************/

send_zone_info(term_id, zone_info, veh_ptr)	/* send zone info */
   char  term_id[];		/* rf id */
char  zone_info[];		/* zone info string */
struct veh_driv *veh_ptr;
{
#ifdef DEBUG
   if (offset->prattle >= LEVEL4)
   {
      sprintf(trace_str,
	 "ENTERING send_zone_info (term_id=%s, zone_info=%s, veh_ptr=%x)\n",
	      term_id, zone_info, veh_ptr);
      TRACE(trace_str);
   }
#endif

   sprintf(str_temp, "%s", zone_info);

   if (send_text(term_id, str_temp, veh_ptr, PRIORITY_2) != 0)
      ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", "unable to send zone info");

#ifdef DEBUG
   if (offset->prattle >= LEVEL6)
      TRACE("LEAVING send_zone_info\n");
#endif
}				/* end send_zone_info */

#ifdef DUAL_PORTS
int
send_port_1()
/* choose a port based on mid & multicast */
{
     if (outb.multicast == 'M')
        return (TRUE);
     if (outb.route_id[MID_SIZE -1] > '8')
       return (TRUE);
     else
       return (FALSE);
}                              /* end send_port_1 */
#endif

int
transmit_message_udp( veh_ptr )
  struct veh_driv *veh_ptr;
{
  UDP_OUT  udp_out;
  int      result;
  
  bzero( &udp_out, sizeof(UDP_OUT) );

  strcpy( udp_out.dest_ip_addr, veh_ptr->ip_address );  
  outb.size = MSG_HDR_LEN + get_outb_text_size();
  //  terminate_outb_msg();  
  memcpy( &udp_out.tpak_msg, &outb, sizeof(MSG_QUEUE_HDR) );


  result = write( udp_out_fd, &udp_out, outb.size + sizeof(outb.size) + 24 );

  return(0);
}
    
/********************************************************************/
/* */
/* transmit_message - preform the message send                      */
/* */
/*******************************************************************/

static int
transmit_message(veh_ptr)
   struct veh_driv *veh_ptr;
{
   int   result;

   

   /* should move to msgsend ? */
   /* dont send outb.prio */
   retry_cnt = 0;
   if (offset->status.ncp)
   {				/* send only if ncp is up */
      if (veh_ptr != NULL)
      {				/* allow fleetwides without a veh_ptr */
	 /* only do send if ncp is up, and no emergency */
	if (!veh_ptr->t_status.kdt_working) // || (veh_ptr->t_status.emer))
	    return (0);
	 if (!fleet[veh_ptr->fleet_nbr]->criteria.ncp_on)
	    return (0);

	 /* fill appropriate field with host link if dual-acc supported */
	 if ( 1 )
	   {
	     outb.delvry_prio = (char)Veh_get_value( (VEH_HNDL)veh_ptr, VEH_HOST_LINK );
	     if ( outb.delvry_prio != '1' &&
		  outb.delvry_prio != '2' &&
		  outb.delvry_prio != '3' &&
		  outb.delvry_prio != '4' &&
		  outb.delvry_prio != '5' &&
		  outb.delvry_prio != '6' &&
		  outb.delvry_prio != '7' &&
		  outb.delvry_prio != '8')
	       outb.delvry_prio = '1';
	   }
      }
      signal(SIGALRM, t_out_hdlr);
      errno = 0;
      if (setjmp(env) != 0)
      {
         lost_outb++;
         /* attend to the user interface */
         if (lost_outb == MAX_OUTB_LOST)
         {
            /* onle do once */
            mk_except((struct calls *)NULL, (struct veh_driv *)NULL, SYS_ERR, NCP_ALARM,-1); 
         }

         return(-1);
      }
      alarm(WRITE_TIME_OUT);

      terminate_outb_msg();
      outb.size = MSG_HDR_LEN + get_outb_text_size() + 1;

      if ( outb.delvry_prio == ' ' ) // UDP-GPRS message
	{
	  result = write(udp_out_fd, &outb, outb.size + sizeof(outb.size));
	  result = write(outb_fd, &outb, outb.size + sizeof(outb.size));
	}
      else if ( outb.delvry_prio == '8' )
	result = write(udp_out_fd, &outb, outb.size + sizeof(outb.size));
      else
	result = write(outb_fd, &outb, outb.size + sizeof(outb.size));

      alarm(0);
      if (result < 0)
      {
      /* if the timeout breaks us out the retrys wont work */

         lost_outb++;  /* in case time_out_hdlr really works */
         if (lost_outb == MAX_OUTB_LOST)
         {
            /* onle do once */
            mk_except((struct calls *)NULL, (struct veh_driv *)NULL, SYS_ERR, NCP_ALARM,-1); 
         }
	 return (result);
      }
   }
   lost_outb = 0;
   return (0);

}				/* end transmit_message */

/******************************************************************/
/* */
/*
 * send_msg_mmp - User defined mmp specific message Transmission to Terminal
 */
/* */
/******************************************************************/

int
send_msg_mmp(term_id, mmp_hdr, veh_ptr)	/* send message */
   char  term_id[];		/* rf id */
char  mmp_hdr[];		/* text */
struct veh_driv *veh_ptr;
{
   int   status;

#ifdef DEBUG
   if (offset->prattle >= LEVEL4)
   {
      sprintf(trace_str,
	      "ENTERING send_msg_mmp (term_id=%s, veh_ptr=%x)\n",
	      term_id, veh_ptr);
      TRACE(trace_str);
   }
#endif

   mk_outb_header(term_id, mmp_hdr);
   if (is_text_outb())
      terminate_outb_text();	/* if there is any text terminate it */

   status = transmit_message(veh_ptr);

#ifdef DEBUG

   if (offset->prattle >= LEVEL6)
      TRACE("LEAVING send_msg_mmp\n");
#endif
   return (status);
}				/* end send_msg_mmp */

/*****************************************************************************/
/* */
/* send_text_predefined - Canned Text Transmission to Terminal                           */
/* */
/*****************************************************************************/

int
send_text_predefined(term_id, text_id, veh_ptr)	/* send predefined text */
   char  term_id[];		/* rf id */
char  text_id;			/* handle to text string to be displayed */
struct veh_driv *veh_ptr;
{
   int   status;

   char *get_outb_header();
   char *get_outb_txt();

#ifdef DEBUG
   int   limit = 1;
   int   ii;
   if (offset->prattle >= LEVEL4)
   {
      sprintf(trace_str,
	      "ENTERING send_text_predefined (term_id=%s, veh_ptr=%x)\n",
	      term_id, veh_ptr);
      TRACE(trace_str);
   }
#endif

   if (is_valid_text_id(text_id))
   {
      mk_outb_header(term_id, get_outb_header(text_id));
      mk_outb_text(get_outb_txt(text_id));
      terminate_outb_text();

#ifdef DEBUG
      for (ii = 0; ii < limit; ii++)
#endif
	 status = transmit_message(veh_ptr);
      if (status)
      {
         sprintf(str_temp,"unable to send msg to %s",veh_ptr->mid);
         ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_temp);
      }


   } else
      ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "",
	    "send_text_predefined: bad text_id ");

#ifdef DEBUG
   if (offset->prattle >= LEVEL6)
      TRACE("LEAVING send_text_predefined\n");
#endif
   return (status);
}				/* end send_text_predefined */

/*****************************************************************************/
/* */
/* send_text - Normal Text Transmission to Terminal                           */
/* */
/*****************************************************************************/

int
send_text(term_id, text_str, veh_ptr, priority)	/* send text */
   char  term_id[];		/* rf id */
char  text_str[];		/* text */
struct veh_driv *veh_ptr;
char  priority;
/* null string will terminate text already there */
{
   int   status;
#ifdef DEBUG
   int   ii;
#endif

#ifdef DEBUG
   if (offset->prattle >= LEVEL4)
   {
      sprintf(trace_str,
	      "ENTERING send_text (term_id=%s, veh_ptr=%x)\n",
	      term_id, veh_ptr);
      TRACE(trace_str);
   }
#endif
   
   mk_outb_header(term_id, TEXT_DISPLAY);
   if (priority >= PRIORITY_0 && priority <= PRIORITY_3)
      mk_msg_priority(priority);
   if (text_str != NULL)
      mk_outb_text(text_str);

   terminate_outb_text();

#ifdef DEBUG
   for (ii = 0; ii < limit; ii++)
#endif
      status = transmit_message(veh_ptr);

#ifdef DEBUG
   if (offset->prattle >= LEVEL6)
      TRACE("LEAVING send_text\n");
#endif
   return (status);
}				/* end send_text */

/*****************************************************************************/
/* */
/*
 * clear_terminal_screen   clear the terminal device screen /*
 */
/*****************************************************************************/

clear_terminal_screen(term_id, device, veh_ptr)	/* device clear */
   char  term_id[];		/* rf id */
char  device;			/* which part of screen */
struct veh_driv *veh_ptr;
{

#ifndef CLEAR_NEEDED
   /* obsolete - not supported */
   return;
#else
#ifdef DEBUG
   if (offset->prattle >= LEVEL4)
   {
      sprintf(trace_str,
	      "ENTERING clear_terminal_screen (term_id=%s, veh_ptr=%x)\n",
	      term_id, veh_ptr);
      TRACE(trace_str);
   }
#endif
   mk_clear_mmp(term_id, device);	/* set the bits */

   if (transmit_message(veh_ptr) != 0)
      ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "",
	    "clear_terminal_screen: msgsend");
#ifdef DEBUG
   if (offset->prattle >= LEVEL6)
      TRACE("LEAVING clear_terminal_screen\n");
#endif
#endif
}				/* end clear_terminal_screen */

static void
make_search_report_text(mdt_screen, fl_ptr)
int mdt_screen;
struct fleets *fl_ptr;
{
    int   i;
    char  num_cabs[3], num_calls[3];
    char  str[MAX_DISPLAY_LENGTH + 1];
    int   num_lines = 1;
    int   column = 0;
    
    /* Init outb text */
    mk_outb_text("");
    
    for (i = 0; i < fl_ptr->search_report.screen[mdt_screen].num_in_search; i ++)
    {
	/* Only 3 zone/cabs/calls info per line of MDT.
	   Put a CR after each 3 */
	if (i && (i % 3 == 0))
	{
	    num_lines++;

	    /* Exit if no more room in MDT memory */
	    if (num_lines > MAX_MSG_LINES)
		break;
	    add_outb_text(CR);
	    column = 0;
	}
	
	if (fl_ptr->search_report.screen[mdt_screen].count[i].num_cabs > 99)
	    strcpy(num_cabs, "**");
	else
	    sprintf(num_cabs, "%02d", fl_ptr->search_report.screen[mdt_screen].count[i].num_cabs);
	
	if (fl_ptr->search_report.screen[mdt_screen].count[i].num_calls > 99)
	    strcpy(num_calls, "**");
	else
	    sprintf(num_calls, "%02d", fl_ptr->search_report.screen[mdt_screen].count[i].num_calls);
	
	sprintf(str, "%03d:%2s/%2s", fl_ptr->search_report.screen[mdt_screen].count[i].zone_num,
		num_calls, num_cabs);
	add_outb_text(str);
	
	/* Only the first two columns of zones need spacing,
	   the last one doesn't */
	if (column != 2)
	    add_outb_text("  "); /* 2 char spacing */
	column ++;
    }
}				/* end make_search_report_text */

/*****************************************************************************/
/* */
/* send_srch_display - Search Report Transmission Routine                  */
/* */
/*****************************************************************************/

send_srch_display(fleet_nbr)	/* fleetwide broadcast */
   int   fleet_nbr;		/* fleet no */
{
   char  term_id[MID_SIZE + 1];
   struct fleets *fl_ptr;

   fl_ptr = fleet[fleet_nbr];
   memcpy(term_id, GROUP_MID, MID_SIZE - 1);
   sprintf(term_id + MID_SIZE - 1, "%d", fleet_nbr);

   config_outb_msg(10, "000", ' ', ' ', 'M', ' ');

   if ( fleet_nbr == 1 )
     outb.delvry_prio = '8';
   
   /******************************************************/
   /* search report includes:                            */
   /* list of zone number - available calls pair    */
   /* for all reported zone   */
   /******************************************************/

   /* Zones for search reports are seperated into 3 seperate screens */

   make_search_report_text(0, fl_ptr);
   if (send_msg_mmp(term_id, SEND_SRCH1, (struct veh_driv *) NULL) != 0)
      ERROR(fl_ptr->fleet_id, "", "unable to send search report 1");

   make_search_report_text(1, fl_ptr);
   if (send_msg_mmp(term_id, SEND_SRCH2, (struct veh_driv *) NULL) != 0)
       ERROR(fl_ptr->fleet_id, "", "unable to send search report 2");

   make_search_report_text(2, fl_ptr);
   if (send_msg_mmp(term_id, SEND_SRCH3, (struct veh_driv *) NULL) != 0)
       ERROR(fl_ptr->fleet_id, "", "unable to send search report 3");

   config_outb_msg(10, "000", ' ', ' ', ' ', ' ');

#ifdef DEBUG
   if (offset->prattle >= LEVEL6)
      TRACE("LEAVING send_srch_display\n");
#endif
}

send_stand_bcast(fleet_nbr)
     int fleet_nbr;
{
  int num_lines = 1;
  char term_id[MID_SIZE + 1];
  struct fleets *fl_ptr;
  int i = 0;
  FLEET_HNDL fleet_hndl;
  short bcast_interval = 0;
  short bcast_life = 0;
  bool found_one = FALSE;
  bool found_couple = FALSE, found_many = FALSE;
  char str_one[512];
  char str_couple[512];
  char str_many[512];

  bzero(str_one, 512);
  bzero(str_couple, 512);
  bzero(str_many, 512);
  
  fleet_hndl = (FLEET_HNDL)fleet[fleet_nbr];

  if ( fleet_hndl != HNDL_NULL )
    {
      bcast_interval = (short) Fleet_get_value(fleet_hndl, FLEET_STAND_NTFY_INTERVAL);
      bcast_life = (short) Fleet_get_value(fleet_hndl, FLEET_STAND_NTFY_LIFE);
    }
  
  fl_ptr = fleet[fleet_nbr];
  
   memcpy(term_id, GROUP_MID, MID_SIZE - 1);
   sprintf(term_id + MID_SIZE - 1, "%d", fleet_nbr);

   config_outb_msg(10, "000", ' ', ' ', 'M', ' ');

   bzero(str_temp, TMP_STR_LEN);
   for (i=0;i<MAX_STAND_BCAST_REPORT;i++)
     {
       if ((mads_time - fl_ptr->bcast_report[i].last_report_time > bcast_life))
	 {
	   fl_ptr->bcast_report[i].zone_num = 0;
	   fl_ptr->bcast_report[i].bcast_time = 0;
	   fl_ptr->bcast_report[i].last_report_time = 0;
	   fl_ptr->bcast_report[i].stand_bcast_type = 0;
	   continue;
	 }
       
       if (( fl_ptr->bcast_report[i].zone_num > 0) && ( fl_ptr->bcast_report[i].bcast_time == 0 ) )
	 {
	   fl_ptr->bcast_report[i].bcast_time = mads_time;
	   switch (fl_ptr->bcast_report[i].stand_bcast_type)
	     {
	     case 1:
	       found_one = TRUE;
	       sprintf(str_one, "%s %d", str_one, fl_ptr->bcast_report[i].zone_num);
	       break;
	     case 2:
	       found_couple = TRUE;
	       sprintf(str_couple, "%s %d", str_couple, fl_ptr->bcast_report[i].zone_num);
	       break;
	     case 3:
	       found_many = TRUE;
	       sprintf(str_many, "%s %d", str_many, fl_ptr->bcast_report[i].zone_num);
	       break;
	     default:
	       break;
	     }
	 }
     }

   if (found_one)
     sprintf(str_temp,"%s%s%s%s", "ASIAKAS TOLPALLA", CR, str_one, CR);

   if (found_couple)
     sprintf(str_temp,"%s%s%s%s%s", str_temp, "ASIAKKAITA TOLPALLA", CR, str_couple, CR);

   if (found_many)
     sprintf(str_temp,"%s%s%s%s%s", str_temp, "PALJON ASIAKKAITA TOLPALLA", CR, str_many, CR);     
       
   if (strlen(str_temp))
     {
       if (send_text(term_id, str_temp, (struct veh_driv *) NULL, PRIORITY_1) != 0)
	 ERROR(fleet[fleet_nbr]->fleet_id, "", "unable to send stand_bcast msg");
       config_outb_msg(10, "000", ' ', ' ', ' ', ' ');       
     }
}

/*****************************************************************************
/*
/* send_bid_display - Bid Report Transmission Routine
/*
/*****************************************************************************/

send_bid_display(fleet_nbr)	/* fleetwide broadcast */
   int   fleet_nbr;		/* fleet no */
{
   register jj, kk, bid_idx;
   int   num_lines = 1;
   int i,j;

   char  ascii_time[30];
   char  term_id[MID_SIZE + 1];
   struct fleets *fl_ptr;
   extern short current[];

   ZONE_CALLS_LIST_HNDL calls_list_hndl;
   ZONE_HNDL hZone;
   CALL_HNDL call_hndl;
   struct calls *call_ptr;
   unsigned char *attr_val;
   unsigned char mask;
   int attr_offset = 0;
   int count = 0;   
   int fleet_cbook_time = 0;
   int cbook_time = 0;
   struct zones *zone_ptr = HNDL_NULL;
   char str_temp2[TMP_STR_LEN];   
   
#ifdef DEBUG
   if (offset->prattle >= LEVEL5)
   {
      sprintf(trace_str, "ENTERING send_bid_display (fleet_nbr=%d)\n", fleet_nbr);
      TRACE(trace_str);
   }
#endif

   fl_ptr = fleet[fleet_nbr];
   memcpy(term_id, GROUP_MID, MID_SIZE - 1);
   sprintf(term_id + MID_SIZE - 1, "%d", fleet_nbr);

   config_outb_msg(10, "000", ' ', ' ', 'M', ' ');

   if ( fleet_nbr == 1 )
     outb.delvry_prio = '8';
   
   /******************************************************/
   /*
    * bid report includes: /* list of zone number /*****************************************************
    */

   /* convert search_report.srch_time to hh:mm */
   /** TR
      memcpy(ascii_time, ctime(&fl_ptr->srch_rept.srch_time),
             strlen(ctime(&fl_ptr->srch_rept.srch_time)));
      sprintf(str_temp, "%5.5s (%3d)", ascii_time + 11,
              fl_ptr->srch_rept.num_in_srch);
    **/

   strcpy(str_temp, "\0");
   mk_outb_text(str_temp);
   
   fleet_cbook_time = fl_ptr->cbook_time;
   
   kk = 0;
   for (jj = 0; jj < ZONE_MAX; jj++)
   {				/* forming zones */
      if (kk && (kk % (MAX_DISPLAY_LENGTH / 4)) == 0)
      {
	 num_lines++;
	 if (num_lines > MAX_MSG_LINES)
	    break;
	 add_outb_text(CR);
      }
      if (fl_ptr->zone_sumry[jj].time_outs == YES)
      {
	
	if (Zone_get_hndl( fl_ptr->zone_sumry[jj].zone_num, fleet_nbr, &hZone ) != FAIL)
	  {
	    calls_list_hndl = (ZONE_CALLS_LIST_HNDL) Zone_get_value( hZone, ZONE_AVAIL_CALLS_LIST );

	    zone_ptr = (struct zones *)hZone;
	    if ( zone_ptr->cbook_time > 0 )
	      cbook_time = zone_ptr->cbook_time;
	    else
	      cbook_time = fleet_cbook_time;
	    
	    Zone_calls_list_each_get( calls_list_hndl, call_hndl )
	      {
		bzero(str_temp, TMP_STR_LEN);		

		call_ptr = (struct calls *)call_hndl;		
		if ( ( Call_get_state( (CALL_HNDL)call_hndl, CALL_AVAILABLE_STATE ) ) &&
		     ( (mads_time - (time_t)Call_get_value( call_hndl, CALL_TIME_ENTERED ) ) >= cbook_time ) )
		  //		     ( call_ptr->status.call_tmout == 1 ) )
		  {
		    kk++;
		    sprintf(str_temp, "%3d", fl_ptr->zone_sumry[jj].zone_num);

		    // Driver attributes
		    attr_val = (unsigned char *) &call_ptr->vehicle_attributes;

		    attr_offset = 0x20;
		    count = 0;

		    for (i = 0; i < 4; i++) {
		      mask = 0x80;		      
		      for (j=0; j < 8; j++) {
			if (*attr_val & mask) {
			  if (count == 0)
			    sprintf(str_temp, "%s%%.A%c", str_temp, (attr_offset));
			  else
			    sprintf(str_temp, "%s%c", str_temp, (attr_offset));
			  ++count;
			}
			++attr_offset;
			if (attr_offset==0x2E) attr_offset=0x2F;  // can't use 0x3E
			if (attr_offset==0x3E) attr_offset=0x3F; // can't use 0x2E
			mask >>= 1;
		      }
		      attr_val++;
		    }
		    // Vehicle attributes
		    attr_val = (unsigned char *) &call_ptr->driver_attributes;
		    attr_offset = 0x41;

		    for (i = 0; i < 4; i++) {
		      mask = 0x80;		      
		      for (j = 0; j < 8; j++) {
			if (*attr_val & mask) {
			  if (count == 0)
			    sprintf(str_temp, "%s%%.A%c", str_temp, (attr_offset));
			  else
			    sprintf(str_temp, "%s%c", str_temp, (attr_offset));
			  ++count;
			}
			++attr_offset;
			mask >>= 1;
		      }
		      attr_val++;
		    }

		    bzero(str_temp2, sizeof(str_temp2));
		    if (count > 0)
		      sprintf(str_temp2, "%s.", str_temp); // terminate attribute clause
		    else
		      sprintf(str_temp2, "%s ", str_temp);

		    if ( strlen(str_temp2) + strlen(outb.text) <= 384 )
		      add_outb_text(str_temp2);
		  }
	      } Zone_calls_list_each_end(calls_list_hndl)
          }

      }

   }  // for every zone in fleet

   if (kk == 0)   /* if there a no timed out trips then we need to send an "*" */
      add_outb_text("*");

   if (send_msg_mmp(term_id, SEND_BID, (struct veh_driv *) NULL) != 0)
      ERROR(fl_ptr->fleet_id, "", "unable to send bid list");

   config_outb_msg(10, "000", ' ', ' ', ' ', ' ');

#ifdef DEBUG
   if (offset->prattle >= LEVEL6)
      TRACE("LEAVING send_srch_display\n");
#endif
}

/*****************************************************************************
/*
/* send_qp - Send Queue Position Message to MDT
/*
/*****************************************************************************/

send_qp(zone_nbr, veh_ptr)
   int   zone_nbr;		/* zone number posted into */
   struct veh_driv *veh_ptr;
{
   short q_position;
   TIMER_HANDLE *ebook_timer = NULL;
   float   time_left = 0.0;
   struct drv_attributes tc_drv_attr;
   struct veh_attributes tc_veh_attr;
   int   i;
   TC_STRUCT   *tc_struct;
   ZONE_HNDL   zone_hndl;
   FLEET_HNDL  fleet_hndl;
   TC_HNDL     tc_hndl;
   int         num_tc_zone;
   int         look_ahead_value = 0;
   struct cisam_cl   db_call;

   short get_queue_position();

   if (zone_nbr != veh_ptr->zone_num)
   {
      sprintf(scratch_ptr,"zn mismatch %d %d", zone_nbr, veh_ptr->zone_num);
      ERROR(' ',"",scratch_ptr);
      printf("!!!!!!!!!!!!!!!!!!%s\n",scratch_ptr);
   }
   q_position = (short) Veh_get_value((VEH_HNDL) veh_ptr, VEH_Q_POSITION);

   strcpy(str_temp, QUEUE_POSITION_HDR);
   mk_outb_text(str_temp);
   
   if ( ( Veh_get_state((VEH_HNDL)veh_ptr, VEH_EPOSTED_STATE) ) &&
	( veh_ptr->veh_epost_timer != NULL ) &&
	(0) )
     {
       ebook_timer = (TIMER_HANDLE)veh_ptr->veh_epost_timer;
       time_left = (float)((int)Timer_get_value(ebook_timer, TIMER_EXPIRE_TIME) - mads_time)/60;
       if (time_left <= 0 )
	 time_left = 1;
       if ( ( (CALL_HNDL)Veh_get_value((VEH_HNDL)veh_ptr, VEH_HELD_EPOST_CALL) != HNDL_NULL ) &&
            ( strcmp("Danish", LANGUAGE) ) )
	 sprintf(str_temp, " %3d %2d %02.0fMN*", zone_nbr, q_position, time_left);
       else
	 sprintf(str_temp, " %3d %2d %02.0fMN", zone_nbr, q_position, time_left);
     }
   else
     {
       num_tc_zone = 0;
       if ( Zone_get_hndl( zone_nbr, veh_ptr->fleet_nbr, &zone_hndl ) == SUCCESS )
         {
           Fleet_get_hndl( BY_NBR, veh_ptr->fleet_nbr, &fleet_hndl );
           Zone_tc_list_each_get( Zone_get_value( zone_hndl, ZONE_TC_LIST ), tc_hndl )
	 {
           if ((short)Zone_get_value( zone_hndl, ZONE_TC_LOOK_AHEAD ) > 0 )
             look_ahead_value = (int)((short)Zone_get_value( zone_hndl, ZONE_TC_LOOK_AHEAD ) * 5 * 60);
           else
             look_ahead_value = (int)((short)Fleet_get_value( fleet_hndl, FLEET_TC_LOOK_AHEAD ) * 5 * 60);

	   if ( ( mads_time + look_ahead_value ) > (time_t)TC_get_value( tc_hndl, TC_LEAD_TIME ) )
	     {
               // check the vehicle and driver attributes for a match
               tc_struct = (TC_STRUCT *)tc_hndl;
               bzero( &tc_drv_attr, sizeof( struct drv_attributes ) );
               bzero( &tc_veh_attr, sizeof( struct veh_attributes ) );
               for ( i = 0; i < (ATTR_MAX - 1); i++ )
                 {
                   if ( ( tc_struct->veh_attr[i] == YES ) )
                     set_bit( i, sizeof( struct veh_attributes ), &tc_veh_attr );
                   if ( ( tc_struct->drv_attr[i] == YES ) )
                     set_bit( i, sizeof( struct drv_attributes ), &tc_drv_attr );
                 }
               if ( !Vehicle_match_attr( (unsigned char *)&veh_ptr->vehicle_attr, (unsigned char *)&tc_veh_attr ) )
                 continue;
               if ( !Vehicle_match_attr( (unsigned char *)&veh_ptr->driver_attr, (unsigned char *)&tc_drv_attr ) )
                 continue;

               bzero( &db_call, sizeof( struct cisam_cl ) );
               db_call.nbr = tc_struct->nbr;
               if ( db_read_key( CALL_FILE_ID, &db_call, &cl_key1, DB_EQUAL ) != SUCCESS )
                 continue;
               
               if ( db_call.pckup_zone != tc_struct->zone )
                 continue;
               
               if ( strncmp( db_call.status, PENDING, 5 ) )
                 continue;

               if ( veh_ptr->zone_num != db_call.pckup_zone )
                 continue;
               
               ++num_tc_zone;
	     }
	 } Zone_tc_list_each_end(Zone_get_value( zone_hndl, ZONE_TC_LIST ))

             if ( num_tc_zone > 99 )
              num_tc_zone = 99;
           
           sprintf(str_temp, " %3d %2d %c%d", zone_nbr, q_position, TIMEC,num_tc_zone);

         }
     }

   //     sprintf(str_temp, " %3d %2d", zone_nbr, q_position);
   add_outb_text(str_temp);

   if (send_msg_mmp(veh_ptr->mid, STATUS_DISPLAY, veh_ptr) != 0)
      ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", "unable to send_msg_mmp");

}				/* end send_qp */

/*****************************************************************************/
/* */
/* send_brk_time - <BREAK> Command Response Routine                         */
/* */
/*****************************************************************************/

send_brk_time(veh_ptr)		/* send break time limit */
   struct veh_driv *veh_ptr;
{
   char  temp[80];

#ifdef DEBUG
   if (offset->prattle >= LEVEL4)
   {
      sprintf(trace_str, "ENTERING send_brk_time (veh_ptr=%x)\n", veh_ptr);
      TRACE(trace_str);
   }
#endif

   if ((fleet[veh_ptr->fleet_nbr]->zone_ptr[veh_ptr->zone_num]->break_duration) == 0)
   	   sprintf(temp, "%s %d %s", BREAK_TIME,
	   fleet[veh_ptr->fleet_nbr]->break_duration / 60,
	   MINUTES);
   else
	   sprintf(temp, "%s %d %s", BREAK_TIME,
	   fleet[veh_ptr->fleet_nbr]->zone_ptr[veh_ptr->zone_num]->break_duration / 60,
	   MINUTES);

   (void) send_text(veh_ptr->mid, temp, veh_ptr, PRIORITY_2);	/* send it out */

#ifdef DEBUG
   if (offset->prattle >= LEVEL6)
      TRACE("LEAVING send_brk_time\n");
#endif
}				/* end send_brk_time */

/*****************************************************************************/
/* */
/* send_fleetwide - Fleetwide Text Broadcast Routine                        */
/* */
/*****************************************************************************/

send_fleetwide(fleet_nbr, text)	/* fleetwide broadcast text */
   int   fleet_nbr;
   char  text[];		/* text string to be sent */
{				/* variable length message */
   long  i;
   char  term_id[MID_SIZE + 1];

#ifdef DEBUG
   if (offset->prattle >= LEVEL5)
   {
      sprintf(trace_str,
	      "ENTERING send_fleetwide (text=%s, fleet=%d)\n",
	      text, fleet_nbr);
      TRACE(trace_str);
   }
#endif


   config_outb_msg(10, "000", ' ', ' ', 'M', ' ');

   memcpy(term_id, GROUP_MID, MID_SIZE - 1);
   sprintf(term_id + MID_SIZE - 1, "%d", fleet_nbr);

   sprintf(str_temp, "%s", text);

   if (send_text(term_id, str_temp, (struct veh_driv *) NULL, PRIORITY_1) != 0)
      ERROR(fleet[fleet_nbr]->fleet_id, "", "unable to send fleetwide msg");

   config_outb_msg(10, "000", ' ', ' ', ' ', ' ');

#ifdef DEBUG
   if (offset->prattle >= LEVEL6)
      TRACE("LEAVING send_fleetwide\n");
#endif
}

void
dump_outb()
/* print the current outbound message message */
{
   char  temp_str[80];
   int   ii;

#ifdef PIPES
   sprintf(trace_str, "OUTBOUND MSG fid =  %d\n", outb_fd);
#else
   sprintf(trace_str, "OUTBOUND MSG qid =  %d\n", send_msgqid);
#endif
   TRACE(trace_str);

   sprintf(trace_str, " time send = %8.8d\n", ctime(&mads_time) + 11);
   TRACE(trace_str);

   dump_outb_msg();		/* the mmp stuff */
}				/* end dump_outb */

void
t_out_hdlr()
{
  extern void  t_out_hdlr();  
   retry_cnt++;
   alarm(0);
#ifdef DEBUG_LOTS
   if (ok_stats_flg)
      printf("write timeout error\n");
#endif
/*    ERROR(' ',"", "write to async timeout - msg lost"); */
   signal(SIGALRM, t_out_hdlr);
   if (retry_cnt > OUTB_RETRY_MAX)
   {
#ifdef DEBUG
      out_errors++;
#endif
      longjmp(env,1); /* sorry to use this but time out doesnt abort blocked write */
   }
   else
{
      alarm(WRITE_TIME_OUT); /* retry */
}
}				/* end time_out_hdlr */
