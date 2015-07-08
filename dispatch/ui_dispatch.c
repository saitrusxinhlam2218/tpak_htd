/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ui_dispatch.c,v $
*  @(#)  $Revision: 1.16 $
*  @(#)  $Date: 2004/06/21 18:23:01 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/ui_dispatch.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: ui_dispatch.c,v 1.16 2004/06/21 18:23:01 jwelch Exp $";

#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/times.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <memory.h>
#include "taxipak.h"
#include "Object.h"
#include "Call_public.h"
#include "Call_private.h"
#include "mad_ipc.h"
#include "except.h"
#include "ui_ipc.h"
#include "df_maint.h"
#include "mad_error.h"
#include "taxi_db.h"
#include "path.h"
#include "menu.h"
#include "Menu_db.h"
#define  LIBRARY 1
#include "enhance.h"
#include "switch_ext.h"
#include "Ilink_private.h"

extern int call_buf_size;
extern time_t mads_time;	/* system time */
extern struct offsets *offset;	/* offsets of structures in shared memory */
extern int user_pipe_fd;	/* file descriptor of user interface pipe */
extern int dis_msgqid;
extern int pi_msgqid;
extern struct tms cputim;	/* structure to keep track of cpu ticks */
extern long tticks0, tticks1;
extern char req_buf[];
extern int menus_used;
extern void build_tcb_params();

time_t      last_msgq_check = 0;

#define PIPE_LEN BUFSIZ

/*******************************************************************************
* read_pipe - read the user interface pipe
*
* The pipe contains messages pertaining to calls entering the system, exceptions
* to be resolved, and user messages to be sent.
*******************************************************************************/
void
read_pipe()
{
   static char buf[PIPE_LEN];
   static char str[50];
   struct call_ipc_rec *call_ipc_rec_ptr;
   ILINK_RQST *ilink_rqst;
   static int poffset;
   int   len = 0;
   int   bytes_read;
   struct zones *zn_ptr;

   if (poffset != 0)
   {
      len = call_buf_size - poffset;	/* how much was left */
      memcpy(buf, &buf[poffset], len);
      poffset = 0;
   }
   if ((bytes_read = read(user_pipe_fd, &buf[len], call_buf_size - len)) <= 0)
   {
      return;
   }

   bytes_read += len;		/* take into account stuff from previous read */

   while (poffset < bytes_read)
   {
      call_ipc_rec_ptr = (struct call_ipc_rec *) & buf[poffset];

      switch (call_ipc_rec_ptr->rec_type)
      {
        
      case CALL_REQ:
	if ((poffset + sizeof(struct call_ipc_rec)) > bytes_read)
	  /* incomplete record */
	  {
	    readPipeError(call_ipc_rec_ptr, 
			  (poffset + sizeof(struct call_ipc_rec)), bytes_read);
	    return;
	  }
	else
	  {
	    call_ipc_rec_ptr->FTJ_nbr = -1; // All order should obey shift priority rules
	    poffset += sizeof(struct call_ipc_rec);
	    (void) Call_initialize(call_ipc_rec_ptr, FALSE);
	  }
	break;

      case EXCEPT_REQ:
	if ((poffset + sizeof(struct excpt_rec)) > bytes_read)
	  {
	    readPipeError(call_ipc_rec_ptr, 
			  (poffset + sizeof(struct excpt_rec)), bytes_read);
	    return;
	  }
	else
	  {
	    res_except((struct excpt_rec *) & buf[poffset]);
	    poffset += sizeof(struct excpt_rec);
	  }
	break;

      case GPS_POS_REQ:
	if ((poffset + sizeof(struct excpt_rec)) > bytes_read)
	  {
	    readPipeError(call_ipc_rec_ptr, 
			  (poffset + sizeof(struct excpt_rec)), bytes_read);
	    return;
	  }
	else
	  {
	    GPS_request_position((struct excpt_rec *) & buf[poffset]);
	    poffset += sizeof(struct excpt_rec);
	  }
	break;	
	
      case DRV_REQ:
	if ((poffset + sizeof(struct drv_msg)) > bytes_read)
	  {
	    readPipeError(call_ipc_rec_ptr, 
			  (poffset + sizeof(struct drv_msg)), bytes_read);
	    return;
	  }
	else
	  {
	    disp_send_mesg((struct drv_msg *) & buf[poffset]);
	    poffset += sizeof(struct drv_msg);
	  }
	break;
	
      case KDT_REQ:
	if ((poffset + sizeof(struct kdt_emulate)) > bytes_read)
	  {
	    readPipeError(call_ipc_rec_ptr, 
			  (poffset + sizeof(struct kdt_emulate)), bytes_read);
	    return;
	  }
	else
	  {
	    emulate_kdt((struct kdt_emulate *) & buf[poffset]);
	    poffset += sizeof(struct kdt_emulate);
	  }
	break;
	
      case SP_EXCEPT_REQ :  /* Request for SamPlan exception generation */
	if (0)
	  {
	    if( (poffset + sizeof(struct sp_excpt_rec)) > bytes_read) /* incomplete record */
	      return;
	    else
	      {
		generate_exception((struct sp_excpt_rec*)&buf[poffset]);
		poffset += sizeof(struct sp_excpt_rec);
	      }
	    break;
	  }
	
      case SP_CALL_REQ :   /* SamPlan FTJ-call */
        //
	  {
	    if( (poffset + sizeof(struct call_ipc_rec)) > bytes_read) /* incomplete record */
	      return;
	    else
	      {
		call_ipc_rec_ptr->FTJ_nbr = -1;
		(void)Call_initialize(call_ipc_rec_ptr, FALSE);
		poffset += sizeof(struct call_ipc_rec);
	      }
	    break;
	  }

      case ILINK_REQ:  // reply from Ilink_client that the transfer was rejected
          if ( (poffset + sizeof(ILINK_RQST)) > bytes_read )
            return;
          else
            {
              InterLink_transfer_reject( (ILINK_RQST *)&buf[poffset]);
              poffset += sizeof(ILINK_RQST);
            }
          break;
          
      default:
	poffset = 0;
	return;
      }
      
    }
   poffset = 0;
 }				/* end read_pipe */

/*******************************************************************************
* get_msg - Read the user interface message queue and the maintenance queue.
*
* After determining the type of the message, invoke the proper function to
* complete the request.
*******************************************************************************/
void
get_msg()
{
   struct veh_driv *veh_ptr;
   static char msg_buf[1024];
   struct df_maint *buf_ptr;	/* just used to get the type code */
   struct msqid_ds msgq_stat;
   int rc = 0;
   
   if ( ( mads_time - last_msgq_check ) > 60 )
     {
       bzero( &msgq_stat, sizeof( struct msqid_ds ) );
       last_msgq_check = mads_time;
       if ( msgctl( dis_msgqid, IPC_STAT, &msgq_stat ) == 0 )
	 {
	   if ( msgq_stat.msg_cbytes > 500 )
	     while (msgrcv( dis_msgqid, (struct msgbuf *) msg_buf, sizeof msg_buf, -32767L, IPC_NOWAIT) > 0);
	 }

	 {
	   if ( msgctl( pi_msgqid, IPC_STAT, &msgq_stat ) == 0 )
	     {
	       if ( msgq_stat.msg_cbytes > 500 )
		 while (msgrcv( pi_msgqid, (struct msgbuf *) msg_buf, sizeof msg_buf, -32767L, IPC_NOWAIT) > 0);
	     }
	 }
     }

   if (msgrcv(pi_msgqid, (struct msgbuf *) msg_buf, sizeof msg_buf, DISP_DEST, IPC_NOWAIT) > 0)
   {
      buf_ptr = (struct df_maint *) msg_buf;	/* just get the type code */
      switch (buf_ptr->rec_type)
      {
	 
      case ZONE_REQ:		/* zone request */
	 Zone_send_status(pi_msgqid, (struct zone_rqst *) msg_buf);
	 break;

      case VEH_REQ:		/* give user interface vehicle information */
	 Veh_send_status((struct veh_rqst *) msg_buf);
	 break;

       case TC_REQ:
	 TC_send_status(pi_msgqid, (struct tc_rqst *) msg_buf);
	 break;
	 
	case PI_ZONE_INFO_REQ:
	 Zone_send_pi_status(pi_msgqid, (struct zone_rqst *) msg_buf);
	 break;

      default:
	break;
      }
   }
   
   if (msgrcv(dis_msgqid, (struct msgbuf *) msg_buf, sizeof msg_buf, DISP_DEST, IPC_NOWAIT) > 0)
   {
      buf_ptr = (struct df_maint *) msg_buf;	/* just get the type code */
      switch (buf_ptr->rec_type)
      {
      case DFM_REQ:		/* disk file maintenace request */
	 switch (buf_ptr->which_file)
	 {
	 case DRIVER_FILE_ID:
	    Driv_maint(buf_ptr->cisam_num, buf_ptr->key, buf_ptr->fleet);
	    break;
	 case FLEET_FILE_ID:
	    Fleet_maint(buf_ptr->cisam_num, buf_ptr->key, buf_ptr->fleet);
	    break;
	 case MENU_FILE_ID:
	    menu_maint(buf_ptr->cisam_num, buf_ptr->key);
	    break;
	 case VEHICLE_FILE_ID:
	    Veh_maint(buf_ptr->cisam_num, buf_ptr->key, buf_ptr->fleet);
	    break;
	 case ZONE_FILE_ID:
	    Zone_maint(buf_ptr->cisam_num, buf_ptr->key, buf_ptr->fleet);
	    break;
	  case RP_FILE_ID:
	    build_res_periods( offset->res_period );
	    break;
	  case HOLIDAY_FILE_ID:
	    build_holidays( offset->Holidays );
	    break;
	  case ZONEPOLY_FILE_ID:
	    ZonePoly_maint( buf_ptr->key, buf_ptr->fleet );
	    printf( "%d %c\n", buf_ptr->key, buf_ptr->fleet );
	    break;
	 default:
            break;
	 }
	 break;
	 
      case ZONE_REQ:		/* zone request */
	 Zone_send_status(dis_msgqid, (struct zone_rqst *) msg_buf);
	 break;

      case VEH_REQ:		/* give user interface vehicle information */
	 Veh_send_status((struct veh_rqst *) msg_buf);
	 break;

       case TC_REQ:
	 TC_send_status(dis_msgqid, (struct tc_rqst *) msg_buf);
	 break;

       case PI_ZONE_INFO_REQ:
	 Zone_send_pi_status(dis_msgqid, (struct zone_rqst *) msg_buf);
	 break;

      case EMERG_BCAST_REQ:
        Veh_send_emerg_bcast_status( dis_msgqid, (EMERG_BCAST_RQST *) msg_buf );
        break;

      case ZONE_STAND_HOLD:
        Zone_stand_hold( dis_msgqid, (struct zone_stand_hold *) msg_buf );
        break;

      case ZONE_STAND_HOLDv2:
	Zone_stand_holdV2( dis_msgqid, (struct zone_stand_hold *) msg_buf );
	break;

      default:
	 break;
      }
      return;
   } else
      return;
}				/* end get_msg */

/*******************************************************************************
* menu_maint - Update the menu structure in memory.
******************************************************************************/
menu_maint(rec_num, key_fld)
   int   rec_num;
   int   key_fld;
{
   struct menus *menu_ptr;	/* pointer to a menu record */
   struct cisam_mf *mf_ptr;	/* pointer to a cisam menu record */
   int   idx;
   int    i;

#ifdef DEBUG
   if (offset->prattle >= LEVEL3)
   {
      sprintf(trace_str, "menu_maint : rec_num = %d  key_fld = %d \n", rec_num, key_fld);
      TRACE(trace_str);
   }
#endif

   if (db(MENU_FILE_ID, OPEN, &mf_key1, ISINOUT | ISMANULOCK, 0) < 0)
   {				/* open the file */
      ERROR(' ', MENU_FILE, "menu_maint - cannot open file.\n");
      return;
   }
   mf_ptr = (struct cisam_mf *) req_buf;

   if (db(MENU_FILE_ID, READ_NUM, &mf_key1, ISEQUAL, rec_num) < 0)
   {				/* read record */
      mf_ptr->nbr = key_fld;
      if (db(MENU_FILE_ID, READ_KEYS, &mf_key1, ISEQUAL, 0) < 0)
      {				/* read record */
	 ERROR(' ', MENU_FILE, "menu_maint - cannot find record.\n");
	 db(MENU_FILE_ID, CLOSE, &mf_key1, 0, 0);	/* close the file */
	 return;
      }
   }
   menu_ptr = offset->menu;
   for ( i=0;i<MENU_MAX;i++ )
     {
       if ( menu_ptr->mf_command == mf_ptr->command )
	 break;
       else
	 ++menu_ptr;
     }

   fill_menu(menu_ptr, mf_ptr);

   db(MENU_FILE_ID, CLOSE, &mf_key1, 0, 0);	/* close the file */
   return;
}


/*----------------------------------------------------------
 *	read Pipe Error
 *----------------------------------------------------------
 */

readPipeError(
	CALL_IPC_REC	*call_ipc,
	int				expected_to_read,
	int				actually_read
	)
{
   char error_string[80];
   struct excpt_rec	*exception;

	switch(call_ipc->rec_type)
	{
		case CALL_REQ:
			sprintf(error_string, "%s expect %d bytes - read %d",
				"CALL_REQ size mismatch", expected_to_read, actually_read);
			ERROR(' ',"",error_string);

			sprintf(error_string, 
					"----process id %ld", call_ipc->u_prcs_id);
			ERROR(' ',"",error_string);

			if (call_ipc->c_isam_nbr)
			{
				sprintf(error_string, 
					"----call c_isam_nbr %d", call_ipc->c_isam_nbr);
				ERROR(' ',"",error_string);
			}
			if (call_ipc->call_nbr)
			{
				sprintf(error_string, 
					"----call call_nbr %d", call_ipc->call_nbr);
				ERROR(' ',"",error_string);
			}
			break;

		case EXCEPT_REQ:
			sprintf(error_string, "%s expect %d bytes - read %d",
				"EXCEPT_REQ size mismatch", expected_to_read, actually_read);
			ERROR(' ',"",error_string);

			exception = (struct excpt_rec *)call_ipc;
			sprintf(error_string, 
					"----process id %ld", exception->u_prcs_id);
			ERROR(' ',"",error_string);

			if (exception->except_nbr)
			{
				sprintf(error_string, 
					"----exception number %ld", exception->except_nbr);
				ERROR(' ',"",error_string);
			}
			if (exception->resolve_code)
			{
				sprintf(error_string, 
					"----exception resolve_code %d", exception->resolve_code);
				ERROR(' ',"",error_string);
			}
			if (exception->call_nbr)
			{
				sprintf(error_string, 
					"----exception call_nbr %ld", exception->call_nbr);
				ERROR(' ',"",error_string);
			}
			if (exception->veh_nbr)
			{
				sprintf(error_string, 
					"----exception veh_nbr %d", exception->veh_nbr);
				ERROR(' ',"",error_string);
			}
			if (exception->zone_nbr)
			{
				sprintf(error_string, 
					"----exception zone_nbr %d", exception->zone_nbr);
				ERROR(' ',"",error_string);
			}
			break;

		case DRV_REQ:
			sprintf(error_string, "%s expect %d bytes - read %d",
					"DRV_REQ size mismatch", expected_to_read, actually_read);
			ERROR(' ',"",error_string);
			break;

		case KDT_REQ:
			sprintf(error_string, "%s expect %d bytes - read %d",
					"MDT_REQ size mismatch", expected_to_read, actually_read);
			ERROR(' ',"",error_string);
			break;

		default:
			break;
	}

	return( TRUE );
}

