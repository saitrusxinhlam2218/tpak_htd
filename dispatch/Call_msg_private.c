/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call_msg_private.c,v $
*  @(#)  $Revision: 1.15 $
*  @(#)  $Date: 2004/06/22 17:08:16 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/dispatch/Call_msg_private.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Call_msg_private.c,v 1.15 2004/06/22 17:08:16 jwelch Exp $";
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include "taxipak.h"
#include "taxi_db.h"
#include "language.h"
#include "mad_error.h"
#include "timers.h"
#include "Object.h"
#include "List_public.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "Call_public.h"
#include "Call_lists_public.h"
#include "Call_private.h"
#include "Call_msg_public.h"
#include "Call_msg_private.h"
#include "Call_msg_db.h"
#include "Exception_private.h"
#include "Fleet_public.h"
#include "itm.h"
#include "kdt.h"
#include "msg_mmp.h"
#include "enhance.h"
#include "switch_ext.h"
#include "writer.h"
#include "Call_hist_db.h"

CALL_MSG_STRUCT     *free_call_msg;
int                 free_call_msg_nbr = 0;
static CALL_MSG_HNDL get_free_call_msg();
RET_STATUS Call_msg_reset_ack( VEH_HNDL );
static void free_call_msg_memory( CALL_MSG_HNDL );

Call_msg_services_start()
{
  CALL_MSG_STRUCT  *call_msg, *tmp_call_msg;
  int i;

  call_msg = (CALL_MSG_STRUCT *)malloc(sizeof(CALL_MSG_STRUCT) * MAX_CALL_MSG);

  tmp_call_msg = call_msg;
  free_call_msg = call_msg;
  for ( i = 0; i < MAX_CALL_MSG; i++ )
    {
      call_msg->next = ++tmp_call_msg;
      ++call_msg;
      free_call_msg_nbr++;
    }
  call_msg->next = NULL;
}
  
RET_STATUS
Call_msg_list_load( CALL_MSG_LIST_HNDL cmsg_list_hndl, int cl_nbr )
{
  CMSG_REC  cmsg_rec;
  int       done = 0;
  CALL_MSG_HNDL  cmsg_hndl;

  bzero( &cmsg_rec, sizeof( CMSG_REC ) );
  cmsg_rec.call_nbr = cl_nbr;
  cmsg_rec.msg_nbr = '1';
  
  while ( !done )
    {
      if ( db_read_key( CALLMSG_FILE_ID, &cmsg_rec, &cmsg_key1, ISEQUAL ) == SUCCESS )
	{
	  cmsg_hndl = (CALL_MSG_HNDL)get_free_call_msg();
	  Call_msg_rec_2_obj( &cmsg_rec, cmsg_hndl );
	  Call_msg_list_add( cmsg_list_hndl, cmsg_hndl );
	  cmsg_rec.msg_nbr++;
	}
      else
	{
	  cmsg_rec.msg_nbr++;
	  if ( db_read_key( CALLMSG_FILE_ID, &cmsg_rec, &cmsg_key1, ISEQUAL ) == SUCCESS )
	    {
	      cmsg_hndl = (CALL_MSG_HNDL)get_free_call_msg();
	      Call_msg_rec_2_obj( &cmsg_rec, cmsg_hndl );
	      Call_msg_list_add( cmsg_list_hndl, cmsg_hndl );
	      cmsg_rec.msg_nbr++;
	    }
	  else
	    done = 1;
	}
    }
  return(SUCCESS);
}

Call_msg_list_clear( cmsg_list_hndl )
     CALL_MSG_LIST_HNDL    cmsg_list_hndl;
{
   CALL_MSG_HNDL       cmsg_hndl;
   CALL_MSG_HNDL       cmsg_array[ 32 ];
   int                 i = 0;

   for ( i = 0; i < 32; i++ )
     cmsg_array[i] = HNDL_NULL;

   i = 0;
   for ( cmsg_hndl = (CALL_MSG_HNDL)LL_GetFirst( cmsg_list_hndl);
	 cmsg_hndl != HNDL_NULL;
	 cmsg_hndl = (CALL_MSG_HNDL)LL_GetNext( cmsg_list_hndl))
     {
       cmsg_array[i] = cmsg_hndl;
       i++;
     } 

   for ( i = 0; i < 32; i++ )
     {
       if ( cmsg_array[i] != HNDL_NULL )
	 {
	   Call_msg_list_remove( cmsg_list_hndl, cmsg_array[i] );
	   free_call_msg_memory( cmsg_array[i] );
	 }
       else
	 break;
     }
   
   Call_msg_list_destroy( cmsg_list_hndl );
       
 }

Call_msg_rec_2_obj( CMSG_REC *src, CALL_MSG_HNDL dest )
{
  int msg_nbr;
  
  Call_msg_set_value( dest, CALL_MSG_TYPE, (HNDL)src->device );
  Call_msg_set_value( dest, CALL_MSG_CALL_NBR, (HNDL)src->call_nbr );
  Call_msg_set_value( dest, CALL_MSG_MSG_NBR, (HNDL)src->msg_nbr );
  Call_msg_set_value( dest, CALL_MSG_ACK, (HNDL)src->ack );
}

RET_STATUS
Call_msg_send( cmsg_list_hndl, veh_hndl )
     CALL_MSG_LIST_HNDL cmsg_list_hndl;
     VEH_HNDL           veh_hndl;
{
  CALL_MSG_HNDL   cmsg_hndl;
  CMSG_REC        cmsg_rec;
  char            dummy[360];
  char            *pStart = NULL, *pTariff = NULL, *pTariffEnd = NULL, *pNumber = NULL;
  char            sStart[64], sEnd[64], sMsgtoSend[256];
  
  bzero( &cmsg_rec, sizeof( CMSG_REC ) );
  Call_msg_list_each_get( cmsg_list_hndl, cmsg_hndl )
    {
      if ( (char)Call_msg_get_value( cmsg_hndl, CALL_MSG_ACK ) == NO )
	{
	  /** Flag the message as 'send in progress' **/
	  Call_msg_set_value( cmsg_hndl, CALL_MSG_ACK, (HNDL) SUSP_ON );
	  
	  /** send the message **/
	  cmsg_rec.call_nbr = (int)Call_msg_get_value( cmsg_hndl, CALL_MSG_CALL_NBR );
	  cmsg_rec.msg_nbr = (char)Call_msg_get_value( cmsg_hndl, CALL_MSG_MSG_NBR );
	  if ( db_read_key( CALLMSG_FILE_ID, &cmsg_rec, &cmsg_key1, ISEQUAL ) != SUCCESS )
	    return( FAIL );
	  switch( (char)Call_msg_get_value( cmsg_hndl, CALL_MSG_TYPE ) )
	    {
	    case MDT:
	      send_pkt_id( SP_MDT_PKT_TYPE,
			   (int)Call_msg_get_value( cmsg_hndl, CALL_MSG_MSG_NBR ),
			   (int)Veh_get_value(veh_hndl, VEH_NBR ) );
	      format_mdt_msg( dummy, cmsg_rec.msg );
	      send_text( (char *)Veh_get_value(veh_hndl, VEH_MID), dummy, veh_hndl, PRIORITY_3 );
	      clr_pkt_id();
	      return(SUCCESS);

	    case METER:
	      if ( (short)Veh_get_value(veh_hndl, VEH_TERM_TYPE) == MDT_V03_ITM_VERS2 )
		{
		  bzero( sEnd, sizeof( sEnd ) );
		  bzero( sStart, sizeof( sStart ) );
		  bzero( sMsgtoSend, sizeof( sMsgtoSend ) );
		  pStart = cmsg_rec.msg;
		  pTariff = (char *)strstr( cmsg_rec.msg, "A11" );
		  if ( pTariff != NULL )
		    {
		      strncpy( sStart, pStart, (pTariff - pStart) );		      
		      pNumber = ++pTariff;
		      pTariffEnd = NULL;
		      while ( pTariffEnd == NULL )
			{
			  if ( isdigit( *pNumber ) )
			    ++pNumber;
			  else
			    pTariffEnd = pNumber;
			}
		      strcpy( sEnd, pTariffEnd );
		      sprintf( sMsgtoSend, "%sJ%d%s", sStart, (int)Veh_get_value(veh_hndl, VEH_CALL_NBR), sEnd );
		      itm_send_msg( (char *)Veh_get_value(veh_hndl, VEH_MID),veh_hndl, METER_ACTION, sMsgtoSend );
		    }
		  else
		    {
		      sprintf( sMsgtoSend, "%sJ%d", pStart, (int)Veh_get_value(veh_hndl, VEH_CALL_NBR) );
		      itm_send_msg( (char *)Veh_get_value(veh_hndl, VEH_MID), veh_hndl, METER_ACTION, sMsgtoSend );
		    }
		}
	      else
		itm_send_msg( (char *)Veh_get_value(veh_hndl, VEH_MID), veh_hndl, METER_ACTION, cmsg_rec.msg );
	      return(SUCCESS);

	    case PRINTER:
	      if (0)
		itm_send_msg( (char *)Veh_get_value(veh_hndl, VEH_MID), veh_hndl, PRINT_TEXT_STOCK, cmsg_rec.msg );
	      else
		{
		  /** For sites other than Stockholm, we won't get an ACK back on the PRINT_TEXT message **/
		  /** so we need to set call message ACK value to YES after sending                      **/
		  itm_send_msg( (char *)Veh_get_value(veh_hndl, VEH_MID), veh_hndl, PRINT_TEXT, cmsg_rec.msg );
		  Call_msg_set_value( cmsg_hndl, CALL_MSG_ACK, (HNDL) YES );
		  Call_msg_update_ack( cmsg_hndl );
		  Call_msg_send( cmsg_list_hndl, veh_hndl );
		}
	      return(SUCCESS);

	    default:
	      break;
	    }
	}
    } Call_msg_list_each_end(cmsg_list_hndl)
}

RET_STATUS
Call_msg_send_m_action( cmsg_list_hndl, veh_hndl )
     CALL_MSG_LIST_HNDL cmsg_list_hndl;
     VEH_HNDL           veh_hndl;
{
  CALL_MSG_HNDL   cmsg_hndl;
  CMSG_REC        cmsg_rec;
  char            *pStart = NULL, *pTariff = NULL, *pTariffEnd = NULL, *pNumber = NULL;
  char            sStart[64], sEnd[64], sMsgtoSend[256];  

  bzero( &cmsg_rec, sizeof( CMSG_REC ) );
  Call_msg_list_each_get( cmsg_list_hndl, cmsg_hndl )
    {
      if ( (char)Call_msg_get_value( cmsg_hndl, CALL_MSG_ACK ) == NO )
	{
	  switch( (char)Call_msg_get_value( cmsg_hndl, CALL_MSG_TYPE ) )
	    {
	    case METER:
	      /** Flag the message as 'send in progress' **/
	      Call_msg_set_value( cmsg_hndl, CALL_MSG_ACK, (HNDL) SUSP_ON );
	      cmsg_rec.call_nbr = (int)Call_msg_get_value( cmsg_hndl, CALL_MSG_CALL_NBR );
	      cmsg_rec.msg_nbr = (char)Call_msg_get_value( cmsg_hndl, CALL_MSG_MSG_NBR );
	      if ( db_read_key( CALLMSG_FILE_ID, &cmsg_rec, &cmsg_key1, ISEQUAL ) != SUCCESS )
		return( FAIL );
	      if ( (short)Veh_get_value(veh_hndl, VEH_TERM_TYPE) == MDT_V03_ITM_VERS2 )
		{
		  bzero( sEnd, sizeof( sEnd ) );
		  bzero( sStart, sizeof( sStart ) );
		  bzero( sMsgtoSend, sizeof( sMsgtoSend ) );
		  pStart = cmsg_rec.msg;
		  pTariff = (char *)strstr( cmsg_rec.msg, "A11" );
		  if ( pTariff != NULL )
		    {
		      strncpy( sStart, pStart, (pTariff - pStart ) );		      
		      pNumber = ++pTariff;
		      pTariffEnd = NULL;
		      while ( pTariffEnd == NULL )
			{
			  if ( isdigit( *pNumber ) )
			    ++pNumber;
			  else
			    pTariffEnd = pNumber;
			}
		      strcpy( sEnd, pTariffEnd );
		      sprintf( sMsgtoSend, "%sJ%d%s", sStart, (int)Veh_get_value(veh_hndl, VEH_CALL_NBR), sEnd );
		      itm_send_msg( (char *)Veh_get_value(veh_hndl, VEH_MID),veh_hndl, METER_ACTION, sMsgtoSend );
		    }
		  else
		    {
		      sprintf( sMsgtoSend, "%sJ%d", pStart, (int)Veh_get_value(veh_hndl, VEH_CALL_NBR) );
		      itm_send_msg( (char *)Veh_get_value(veh_hndl, VEH_MID), veh_hndl, METER_ACTION, sMsgtoSend );
		    }
		}
	      else	      
		itm_send_msg( (char *)Veh_get_value(veh_hndl, VEH_MID), veh_hndl, METER_ACTION, cmsg_rec.msg );
	      return(SUCCESS);

	    default:
	      break;

	    }
	}
    } Call_msg_list_each_end(cmsg_list_hndl)

        return(SUCCESS);
}


RET_STATUS
Call_msg_itm_ack( veh_hndl )
     VEH_HNDL     veh_hndl;
{
  CALL_MSG_HNDL   cmsg_hndl;
  CALL_HNDL       call_hndl;
  CMSG_REC        cmsg_rec;
  FLEET_HNDL      fleet_hndl;

  if ( ( call_hndl = (CALL_HNDL)Veh_get_value( veh_hndl, VEH_CALL_HNDL ) ) == HNDL_NULL )
    return ( FAIL );

  Fleet_get_hndl( BY_NBR, (short)Veh_get_value( veh_hndl, VEH_FLEET_NBR ), &fleet_hndl );  

  Call_msg_list_each_get( (CALL_MSG_LIST_HNDL)Call_get_value( call_hndl, CALL_MSG_LIST ), cmsg_hndl )
    {
      if ( ( (char)Call_msg_get_value( cmsg_hndl, CALL_MSG_TYPE ) == METER ) &&
	   ( ( (char)Call_msg_get_value( cmsg_hndl, CALL_MSG_ACK ) == NO ) ||
	     ( (char)Call_msg_get_value( cmsg_hndl, CALL_MSG_ACK ) == SUSP_ON ) ) )
	{
	  Call_msg_update_ack( cmsg_hndl );
	  if ( (0) && ( (short)Fleet_get_value(fleet_hndl, FLEET_M_ACTION_TIMER) > 0 ) )
	    {
	      if ( Call_delete_timer(call_hndl, T_M_ACTION) != SUCCESS )
		{
		  sprintf(error_str,"Error call_m_action_notify_func - call %d timer not in list",(int)Call_get_value(call_hndl, CALL_NBR));
		  ERROR(' ',"", error_str);
		}	      
	      Call_send_assign(call_hndl, veh_hndl );
	    }
	  break;
	}
    } Call_msg_list_each_end((CALL_MSG_LIST_HNDL)Call_get_value( call_hndl, CALL_MSG_LIST ))

   Call_msg_send( (CALL_MSG_LIST_HNDL)Call_get_value( call_hndl, CALL_MSG_LIST ), veh_hndl );      
}



RET_STATUS
Call_msg_printer_ack( veh_hndl )
     VEH_HNDL     veh_hndl;
{
  CALL_MSG_HNDL   cmsg_hndl;
  CALL_HNDL       call_hndl;
  CMSG_REC        cmsg_rec;

  if ( ( call_hndl = (CALL_HNDL)Veh_get_value( veh_hndl, VEH_CALL_HNDL ) ) == HNDL_NULL )
    return ( FAIL );

  Call_msg_list_each_get( (CALL_MSG_LIST_HNDL)Call_get_value( call_hndl, CALL_MSG_LIST ), cmsg_hndl )
    {
      if ( ( (char)Call_msg_get_value( cmsg_hndl, CALL_MSG_TYPE ) == PRINTER ) &&
	   ( ( (char)Call_msg_get_value( cmsg_hndl, CALL_MSG_ACK ) == NO ) ||
	     ( (char)Call_msg_get_value( cmsg_hndl, CALL_MSG_ACK ) == SUSP_ON ) ) )
	{
	  /** mark ACK in cmsg record and in list record **/
	  Call_msg_update_ack( cmsg_hndl );
	  break;
	}
    } Call_msg_list_each_end((CALL_MSG_LIST_HNDL)Call_get_value( call_hndl, CALL_MSG_LIST ))

   Call_msg_send( (CALL_MSG_LIST_HNDL)Call_get_value( call_hndl, CALL_MSG_LIST ), veh_hndl );
}

RET_STATUS
Call_msg_mdt_ack( veh_hndl, msg_nbr )
     VEH_HNDL     veh_hndl;
     char         msg_nbr;
{
  CALL_MSG_HNDL   cmsg_hndl;
  CALL_HNDL       call_hndl;
  CMSG_REC        cmsg_rec;

  if ( ( call_hndl = (CALL_HNDL)Veh_get_value( veh_hndl, VEH_CALL_HNDL ) ) == HNDL_NULL )
    return ( FAIL );

  Call_msg_list_each_get( (CALL_MSG_LIST_HNDL)Call_get_value( call_hndl, CALL_MSG_LIST ), cmsg_hndl )
    {
      if ( (char)Call_msg_get_value( cmsg_hndl, CALL_MSG_MSG_NBR ) == msg_nbr )
	{
	  /** mark ACK in cmsg record and in list record **/
	  Call_msg_update_ack( cmsg_hndl );
	  break;
	}
    } Call_msg_list_each_end((CALL_MSG_LIST_HNDL)Call_get_value( call_hndl, CALL_MSG_LIST ))

   Call_msg_send( (CALL_MSG_LIST_HNDL)Call_get_value( call_hndl, CALL_MSG_LIST ), veh_hndl );
}

RET_STATUS
Call_msg_itm_nak( veh_hndl )
     VEH_HNDL     veh_hndl;
{

  CALL_MSG_HNDL   cmsg_hndl;
  CALL_HNDL       call_hndl;
  FLEET_HNDL      fleet_hndl;
  CALL_HIST_REC   writer_buf;
  struct veh_driv *veh_ptr;
  
  if ( veh_hndl == HNDL_NULL )
    return ( FAIL );

  if ( ( call_hndl = (CALL_HNDL)Veh_get_value( veh_hndl, VEH_CALL_HNDL ) ) == HNDL_NULL )
    return ( FAIL );

  Fleet_get_hndl( BY_NBR, (short)Call_get_value( call_hndl, CALL_FLEET_NBR ), &fleet_hndl );
  
  Call_msg_list_each_get( (CALL_MSG_LIST_HNDL)Call_get_value( call_hndl, CALL_MSG_LIST ), cmsg_hndl )
  {
    if ( ( (char) Call_msg_get_value( cmsg_hndl, CALL_MSG_TYPE ) == METER ) &&
	 ( ( (char) Call_msg_get_value( cmsg_hndl, CALL_MSG_ACK ) == NO ) ||
	   ( (char) Call_msg_get_value( cmsg_hndl, CALL_MSG_ACK ) == SUSP_ON ) ) )
      {
	/* Stockholm rule:
	     if NAK is received and it is after the first transmission, redispatch the trip
	     if NAK is received and it is after one or more retries, assume that it was received and send details
	*/
	if (0)
	  {
	     if ( (short)Call_get_value(call_hndl, CALL_M_ACTION_RETRY) ==
		  (short)Fleet_get_value(fleet_hndl, FLEET_M_ACTION_RETRY ) )
	       {
		 /* NAK received on first transmission */
		 Call_msg_set_value( cmsg_hndl, CALL_MSG_ACK, (HNDL) NO );
		 Call_delete_timer( call_hndl, T_M_ACTION );
		 veh_ptr = (struct veh_driv *)veh_hndl;
		 writer_buf.call_nbr = (int)Call_get_value( call_hndl, CALL_NBR );
		 writer_buf.fleet = veh_ptr->fleet;
		 writer_buf.exception_nbr = ITM_INVOICE_NAK;
		 writer_buf.event_type = EVNT_REG_TYPE;
		 strcpy(writer_buf.event_desc,"TXM BUSY"); 
		 writer_buf.veh_nbr = veh_ptr->veh_nbr;
		 writer_buf.drv_id = veh_ptr->driver_id;
		 writer_buf.user_id = -1;
		 writer_add_hist(&writer_buf);
		 Call_msg_reset_ack( veh_hndl );
		 Call_redispatch( call_hndl, M_ACTION_REDISPATCH );
		 return( SUCCESS );
	       }
	     else
	       {
		 /* NAK received after one of the retries */
		 Call_msg_update_ack( cmsg_hndl );
		 Call_delete_timer( call_hndl, T_M_ACTION );		 
		 Call_send_assign( call_hndl, veh_hndl );
		 return( SUCCESS );
	       }
	   }
	else
	  {
	    Call_msg_set_value( cmsg_hndl, CALL_MSG_ACK, (HNDL) NO );
	    mk_except( call_hndl, veh_hndl, SYS_ERR, NO_ACK_ITM_1,
		      (short)Call_get_value( call_hndl, CALL_FLEET_NBR ) );
	    log_ch_nack( SYS_ERR, call_hndl, METER, (char)Call_msg_get_value( cmsg_hndl, CALL_MSG_MSG_NBR ) );
	    return( SUCCESS );
	  }
      }
  } Call_msg_list_each_end((CALL_MSG_LIST_HNDL)Call_get_value( call_hndl, CALL_MSG_LIST ))

  return( FAIL );
}


RET_STATUS
Call_msg_reset_ack( veh_hndl )
     VEH_HNDL     veh_hndl;
{
  CALL_MSG_HNDL   cmsg_hndl;
  CALL_HNDL       call_hndl;
  CMSG_REC        cmsg_rec;
  FLEET_HNDL      fleet_hndl;

  if ( ( call_hndl = (CALL_HNDL)Veh_get_value( veh_hndl, VEH_CALL_HNDL ) ) == HNDL_NULL )
    return ( FAIL );

  Fleet_get_hndl( BY_NBR, (short)Veh_get_value( veh_hndl, VEH_FLEET_NBR ), &fleet_hndl );

  Call_msg_list_each_get( (CALL_MSG_LIST_HNDL)Call_get_value( call_hndl, CALL_MSG_LIST ), cmsg_hndl )
    {
      /* don't reset ACK's on Stockholm's M_ACTION messages */
      if ( ( (char)Call_msg_get_value( cmsg_hndl, CALL_MSG_TYPE ) == METER ) &&
	   (0) )
	continue;
      
      Call_msg_set_value( cmsg_hndl, CALL_MSG_ACK, (HNDL)NO );
      Call_msg_update_nak( cmsg_hndl );
    } Call_msg_list_each_end((CALL_MSG_LIST_HNDL)Call_get_value( call_hndl, CALL_MSG_LIST ))
}

RET_STATUS
Call_msg_reset_m_action_ack( veh_hndl )
     VEH_HNDL     veh_hndl;
{
  CALL_MSG_HNDL   cmsg_hndl;
  CALL_HNDL       call_hndl;
  CMSG_REC        cmsg_rec;
  FLEET_HNDL      fleet_hndl;

  if ( ( call_hndl = (CALL_HNDL)Veh_get_value( veh_hndl, VEH_CALL_HNDL ) ) == HNDL_NULL )
    return ( FAIL );

  Fleet_get_hndl( BY_NBR, (short)Veh_get_value( veh_hndl, VEH_FLEET_NBR ), &fleet_hndl );

  Call_msg_list_each_get( (CALL_MSG_LIST_HNDL)Call_get_value( call_hndl, CALL_MSG_LIST ), cmsg_hndl )
    {
      if ( ( (char)Call_msg_get_value( cmsg_hndl, CALL_MSG_TYPE ) == METER ) &&
	   (0) && ( (short)Fleet_get_value( fleet_hndl, FLEET_M_ACTION_TIMER ) > 0 ) )
	{      
	  Call_msg_set_value( cmsg_hndl, CALL_MSG_ACK, (HNDL)NO );
	  Call_msg_update_nak( cmsg_hndl );
	}
    } Call_msg_list_each_end((CALL_MSG_LIST_HNDL)Call_get_value( call_hndl, CALL_MSG_LIST ))
}

RET_STATUS
Call_msg_update_nak( cmsg_hndl )
     CALL_MSG_HNDL   cmsg_hndl;
{
  CMSG_REC           cmsg_rec;

  if ( cmsg_hndl == HNDL_NULL )
    return (FAIL);

  cmsg_rec.call_nbr = (int)Call_msg_get_value( cmsg_hndl, CALL_MSG_CALL_NBR );
  cmsg_rec.msg_nbr = (char)Call_msg_get_value( cmsg_hndl, CALL_MSG_MSG_NBR );
  if ( db_read_key( CALLMSG_FILE_ID, &cmsg_rec, &cmsg_key1, ISEQUAL ) != SUCCESS )
    return( FAIL );
  cmsg_rec.ack = NO;
  db_update( CALLMSG_FILE_ID, &cmsg_rec );

  return(SUCCESS);
}

RET_STATUS
Call_msg_update_ack( cmsg_hndl )
     CALL_MSG_HNDL   cmsg_hndl;
{
  CMSG_REC           cmsg_rec;

  if ( cmsg_hndl == HNDL_NULL )
    return (FAIL);

  cmsg_rec.call_nbr = (int)Call_msg_get_value( cmsg_hndl, CALL_MSG_CALL_NBR );
  cmsg_rec.msg_nbr = (char)Call_msg_get_value( cmsg_hndl, CALL_MSG_MSG_NBR );
  if ( db_read_key( CALLMSG_FILE_ID, &cmsg_rec, &cmsg_key1, ISEQUAL ) != SUCCESS )
    return( FAIL );
  cmsg_rec.ack = YES;
  db_update( CALLMSG_FILE_ID, &cmsg_rec );

  Call_msg_set_value( cmsg_hndl, CALL_MSG_ACK, (HNDL)cmsg_rec.ack );

  return(SUCCESS);
}

static HNDL
call_msg_value( cmsg_hndl, op, attr, value )
     CALL_MSG_HNDL  cmsg_hndl;
     int            op;
     int            attr;
     HNDL           value;
{
  CALL_MSG_STRUCT   *pCmsg;

  pCmsg = ( CALL_MSG_STRUCT * )cmsg_hndl;

  switch (attr)
    {
    case CALL_MSG_TYPE:
      if ( op == PUT )
	pCmsg->type = ( char ) value;
      else
	return ((HNDL) pCmsg->type );
      break;
      
    case CALL_MSG_CALL_NBR:
      if ( op == PUT )
	pCmsg->call_nbr = ( int ) value;
      else
	return ((HNDL) pCmsg->call_nbr);
      break;
		
    case CALL_MSG_MSG_NBR:
      if ( op == PUT )
	pCmsg->msg_nbr = ( char ) value;
      else
	return ((HNDL) pCmsg->msg_nbr);
      break;
    case CALL_MSG_ACK:
      if ( op == PUT )
	pCmsg->ack_nak = ( char ) value;
      else
	return ((HNDL) pCmsg->ack_nak );
    default:
      if ( op == PUT )
	return ((HNDL)FAIL);
      else
	return ((HNDL)NULL);
      break;
    }
  return ((HNDL) SUCCESS);
}

RET_STATUS
Call_msg_set_value( cmsg_hndl, attr, hndl )
     CALL_MSG_HNDL  cmsg_hndl;
     int            attr;
     HNDL           hndl;
{
  HNDL              status;

  status = (HNDL) call_msg_value( cmsg_hndl, PUT, attr, hndl );

  return ((int) status);
}

HNDL
Call_msg_get_value( cmsg_hndl, attr )
     CALL_MSG_HNDL  cmsg_hndl;
     int            attr;
{
  return ((HNDL) call_msg_value( cmsg_hndl, GET, attr, HNDL_NULL ) );
}


  
  
static CALL_MSG_HNDL
get_free_call_msg()
{
  CALL_MSG_STRUCT *call_msg;
  char error_str[80];
  
  if ( free_call_msg->next == NULL )   /* no free nodes */
    {
      sprintf( error_str, "No free nodes to add Call Msg" );
      ERROR(' ', "", error_str);
      return (HNDL_NULL);
    }

  call_msg = free_call_msg;
  free_call_msg = free_call_msg->next;
  free_call_msg_nbr--;
  
  if ( free_call_msg->next == NULL )
    {
      sprintf(error_str, "Allocating last Call Msg. %d left", free_call_msg_nbr);
      ERROR(' ', "", error_str);
    }
  memset((char *)call_msg, '\0', sizeof(CALL_MSG_STRUCT));

  return((CALL_MSG_HNDL)call_msg);
}

static void
free_call_msg_memory( CALL_MSG_HNDL cmsg_hndl )
{
  CALL_MSG_STRUCT   *call_msg;

  call_msg = ( CALL_MSG_STRUCT * )cmsg_hndl;
  memset((char *)call_msg, '\0', sizeof(CALL_MSG_STRUCT));

  call_msg->next = free_call_msg;
  free_call_msg_nbr++;
  free_call_msg = call_msg;
}

  
