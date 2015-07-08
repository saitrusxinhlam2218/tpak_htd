/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ac_generic_replies.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2002/03/25 17:48:26 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/auto_tes/ac_generic_replies.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

static char rcsid[] = "$Id: ac_generic_replies.c,v 1.3 2002/03/25 17:48:26 jwelch Exp $";


#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>
#include "taxipak.h"
#include "taxi_db.h"
#include "taxi_defs.h"
#include "mads_types.h"
#include "Object.h"
#include "Call_public.h"
#include "Call_private.h"
#include "Call_db.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "Vehicle_db.h"
#include "Auto_TES.h"

GenOKReply( sClient, session )
     int   sClient;
     int   session;
{
  TES_MSG      TESMsg;
    
  sprintf( TESMsg.TEStype, "%3d", TES_OK );
  TESMsg.TESdata[0] = '\0';

  bzero(TESMsg.TESdata, sizeof(TESMsg.TESdata));  
  SendAutoCall( sClient, &TESMsg );
  AutoCall_to_Client( sClient, &TESMsg, TES_OK, session );
}

GenKOReply( sClient, ErrorCode, session )
     int   sClient;
     int   ErrorCode;
     int   session;
{
  TES_MSG     TESMsg;

  bzero(TESMsg.TESdata, sizeof(TESMsg.TESdata));  
  sprintf( TESMsg.TEStype, "%3d", TES_KO );
  sprintf( TESMsg.TESdata, "%04d", ErrorCode );

  SendAutoCall( sClient, &TESMsg );
  AutoCall_to_Client( sClient, &TESMsg, TES_KO, session );

}

GenCallInfoReply( int sclient, CALL_REC *pcall, int session )
{
  TES_MSG   tesmsg;
  VEH_REC   veh_rec;
  int       rc = 0;
  
  bzero(tesmsg.TESdata, sizeof(tesmsg.TESdata));  
  bzero( &veh_rec, sizeof( VEH_REC ) );
  veh_rec.fleet = pcall->fleet;
  veh_rec.nbr = pcall->veh_nbr;
  db_open( VEHICLE_FILE_ID, DB_READ_WRITE | DB_MANUAL_LOCK );
  if ( ( rc = db_read_key( VEHICLE_FILE_ID, &veh_rec, &veh_key2, DB_EQUAL ) ) != SUCCESS )
    {
      GenKOReply( sclient, TES_KO_BAD_CALL_DATA, session );
      db_close( VEHICLE_FILE_ID );
      return(-1);
    }
  
  sprintf( tesmsg.TEStype, "%3d", TES_CALL_INFO );
  sprintf( tesmsg.TESdata, "%-2s%-10s%-10s%-6d",
           "99",
           veh_rec.make,
           veh_rec.color,
           veh_rec.nbr );
  
  SendAutoCall( sclient, &tesmsg );
  AutoCall_to_Client( sclient, &tesmsg, TES_CALL_DETAILS, session );
  db_close( VEHICLE_FILE_ID );  
}  
           
  
GenCallDetailsReply( sClient, pCall, session )
     int       sClient;
     CALL_REC   *pCall;
     int       session;
{
    TES_MSG     TESMsg;

    bzero(TESMsg.TESdata, sizeof(TESMsg.TESdata));    
    sprintf( TESMsg.TEStype, "%3d", TES_CALL_DETAILS );
    sprintf( TESMsg.TESdata, "%c%08d%-8s%-5s%-8s%04d   %-20s%-3s%03d%-10s%-14s%06d%08d",
	     pCall->fleet,
	     pCall->nbr,
	     pCall->due_date,
	     pCall->due_time,
	     pCall->status,
	     pCall->pckup_str_nbr,
	     pCall->pckup_str_name,
	     pCall->pckup_city,
	     pCall->pckup_zone,
	     pCall->phone_number,
	     pCall->passenger_name,
	     pCall->veh_nbr,
	     pCall->drv_id );
    
    SendAutoCall( sClient, &TESMsg );
    AutoCall_to_Client( sClient, &TESMsg, TES_CALL_DETAILS, session );
}

GenAddrReply( sClient, pAddress, session )
     int      sClient;
     TES_ADDR *pAddress;
     int      session;
{
  TES_MSG     TESMsg;

  bzero(TESMsg.TESdata, sizeof(TESMsg.TESdata));  
  sprintf( TESMsg.TEStype, "%3d", TES_ADDR_REPLY );
  memcpy( TESMsg.TESdata, pAddress, sizeof( TES_ADDR ) );
  SendAutoCall( sClient, &TESMsg );
  AutoCall_to_Client( sClient, &TESMsg, TES_ADDR_REPLY, session );
}


GenCallNbrReply( sClient, CallNbr, session )
     int      sClient;
     int      CallNbr;
     int      session;
{
  TES_MSG     TESMsg;

  bzero(TESMsg.TESdata, sizeof(TESMsg.TESdata));  
  sprintf( TESMsg.TEStype, "%3d", TES_CALL_NBR );
  sprintf( TESMsg.TESdata, "%08d", CallNbr );

  SendAutoCall( sClient, &TESMsg );
  AutoCall_to_Client( sClient, &TESMsg, TES_CALL_NBR, session );
  
}

GenStandTaxiReply( sClient, pDrv, session )
  int sClient;
DRV_INFO *pDrv;
int session;
{
  TES_MSG  TESMsg;

  bzero(TESMsg.TESdata, sizeof(TESMsg.TESdata));
  sprintf( TESMsg.TEStype, "%3d", TES_DRV_INFO );
  memcpy( TESMsg.TESdata, pDrv, sizeof( DRV_INFO ) );

  SendAutoCall( sClient, &TESMsg );
  AutoCall_to_Client( sClient, &TESMsg, TES_DRV_INFO, session );
  
}  

SendAutoCall( sClient, pTESMsg )
     int       sClient;
     TES_MSG   *pTESMsg;
{
  int         msg_length,rc, i=0;
  AC_MSG      MsgAutoCall;

  msg_length = TES_TYPE_LEN;
  while ( pTESMsg->TESdata[i] != NULL )
    {
      ++i;
      ++msg_length;
    }
      
  sprintf( MsgAutoCall.MsgSize, "%03x", msg_length );
  MsgAutoCall.MsgSize[ AC_MSG_SIZE_LEN ] = '\0';
  memcpy( MsgAutoCall.MsgTES, pTESMsg->TEStype, TES_TYPE_LEN );
  memcpy( &MsgAutoCall.MsgTES[ TES_TYPE_LEN ], pTESMsg->TESdata, msg_length );

  rc = send( sClient, (char * ) &MsgAutoCall, msg_length + 3, 0 );
}
