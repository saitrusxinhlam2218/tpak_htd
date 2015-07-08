/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ac_g7_replies.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/07/23 17:52:32 $
*  @(#)  $Author: root $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/auto_call/ac_g7_replies.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: ac_g7_replies.c,v 1.2 1998/07/23 17:52:32 root Exp $";

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>
#include "AutoCall_private.h"
#include "AutoCall_db.h"
#include "Account.h"
#include "TES.h"


G7ClientReply( sClient, pClientRec, session )
     int    sClient;
     AUTOCALL_CLIENT_REC   *pClientRec;
     int    session;
{
  CLIENT_DATA   ClientReturnData;
  TES_MSG       TESMsg;
  char          frmt_buf[ 132 ];

  sprintf( frmt_buf, "%-*s", CD_NAME_LEN, pClientRec->Passenger );
  memcpy( ClientReturnData.Name, frmt_buf, CD_NAME_LEN );

  sprintf( frmt_buf, "%-*s", CD_USER_NAME_LEN, pClientRec->UserName );
  memcpy( ClientReturnData.UserName, frmt_buf, CD_USER_NAME_LEN );

  sprintf( frmt_buf, "%-*d", CD_STR_NBR_LEN, pClientRec->PckupStrNbr );
  memcpy( ClientReturnData.StreetNbr, frmt_buf, CD_STR_NBR_LEN );

  if ( pClientRec->PckupStrSuffix != '\0' )
    ClientReturnData.StreetNbrSuff = pClientRec->PckupStrSuffix;
  else
    ClientReturnData.StreetNbrSuff = ' ';

  sprintf( frmt_buf, "%-*s", CD_STR_NAME_LEN, pClientRec->PckupStrName );
  memcpy( ClientReturnData.StreetName, frmt_buf, CD_STR_NAME_LEN );

  sprintf( frmt_buf, "%-*s", CD_STR_TYPE_LEN, pClientRec->PckupStrType );
  memcpy( ClientReturnData.StreetType, frmt_buf, CD_STR_TYPE_LEN );

  sprintf( frmt_buf, "%-*s", CD_CITY_LEN, pClientRec->PckupLocality );
  memcpy( ClientReturnData.City, frmt_buf, CD_CITY_LEN );

  sprintf( TESMsg.TEStype, "%3d", TES_G7_CLIENT );
  memcpy( TESMsg.TESdata, &ClientReturnData, sizeof( ClientReturnData ) );

  SendAutoCall( sClient, &TESMsg );
  AutoCall_to_Client( sClient, &TESMsg, TES_G7_CLIENT, session );

}

GenG7EtaReply( sClient, eta, session )
     int          sClient;
     short        eta;
     int          session;
{
  TES_MSG       TESMsg;
  char          frmt_buf[ 132 ];

  sprintf( TESMsg.TEStype, "%3d", TES_G7_ETA );
  sprintf( TESMsg.TESdata, "%02d", eta );

  SendAutoCall( sClient, &TESMsg );
  AutoCall_to_Client( sClient, &TESMsg, TES_G7_ETA, session );
}
  
GenG7CallInfoReply( sClient, pTESCall, session )
     int          sClient;
     CALL_INFO    *pTESCall;
     int          session;
{
  VEHICLE_ID         vehicle_id;
  VEHICLE_INFO       vehicle_info;
  TES_MSG            TESMsg;
  CALL_ACCEPT_INFO   AcceptInfo;
  char               frmt_buf[ 132 ];
  
  if ( pTESCall == NULL )
    return( -1 );

  vehicle_id.fleet = pTESCall->fleet;
  vehicle_id.nbr = pTESCall->vehicle_nbr;

  if ( TES_vehicle_get_info( &vehicle_id, &vehicle_info ) == NULL )
    return( -1 );

  sprintf( frmt_buf, "%-*s", CI_MAKE_MODEL_LEN, vehicle_info.model );
  memcpy( AcceptInfo.MakeModel, frmt_buf, CI_MAKE_MODEL_LEN );

  sprintf( frmt_buf, "%-*s", CI_VEH_COLOR_LEN, vehicle_info.color );
  memcpy( AcceptInfo.Color, frmt_buf, CI_VEH_COLOR_LEN );

  sprintf( frmt_buf, "%-*d", CI_VEH_NBR_LEN, vehicle_id.nbr );
  memcpy( AcceptInfo.VehNbr, frmt_buf, CI_VEH_NBR_LEN );

  sprintf( frmt_buf, "%-*d", CI_DELAY_LEN, pTESCall->pickup.wait_time );
  memcpy( AcceptInfo.Delay, frmt_buf, CI_DELAY_LEN );

  sprintf( TESMsg.TEStype, "%3d", TES_G7_CALL_INFO );
  memcpy( TESMsg.TESdata, &AcceptInfo, sizeof( AcceptInfo ) );

  SendAutoCall( sClient, &TESMsg );
  AutoCall_to_Client( sClient, &TESMsg, TES_G7_CALL_INFO, session );
}
  

  
  
