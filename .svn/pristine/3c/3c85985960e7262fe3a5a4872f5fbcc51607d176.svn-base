/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ac_g7_rqst.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/07/23 17:52:33 $
*  @(#)  $Author: root $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/auto_call/ac_g7_rqst.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: ac_g7_rqst.c,v 1.2 1998/07/23 17:52:33 root Exp $";

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>
#include "language.h"
#include "writer.h"
#include "taxipak.h"
#include "taxi_defs.h"
#include "mads_types.h"
#include "Object.h"
#include "Call_db.h"
#include "Call_private.h"
#include "AutoCall_private.h"
#include "Account_Type.h"
#include "Account.h"
#include "Account_db.h"
#include "Account_private.h"
#include "Account_Type_db.h"
#include "TES.h"
#include "TES_account_private.h"

extern char req_buf[];

G7ValidateClientRqst( sClient, pTESMsg, session )
     int        sClient;
     TES_MSG    *pTESMsg;
     int        session;
{
  char          *pClientNbr;
  int           ClientNbr;
  int           rc = 0;
  AUTOCALL_CLIENT_REC   *pClientRec;
  
  pClientNbr = (char *)pTESMsg->TESdata;
  ClientNbr = atoi( pClientNbr );

  TES_initialize();
  
  if ( ( rc = TES_get_autocall_client( ClientNbr ) ) < 0 )
    {
      GenKOReply( sClient, TES_KO_NO_ACCOUNT );
      return( -1 );
    }
  pClientRec = ( AUTOCALL_CLIENT_REC * ) req_buf;
  G7ClientReply( sClient, pClientRec, session );
  
}

G7ConfirmCall( sClient, pTESMsg, session )
     int         sClient;
     TES_MSG     *pTESMsg;
     int         session;
{
  int            CallNbr;
  int            rc = 0;
  CALL_REC       call_rec;
  APPROVE_INFO   *pApproveInfo;

  pApproveInfo = ( APPROVE_INFO * ) pTESMsg->TESdata;
  
  CallNbr = atoi( ( char * )pApproveInfo->call_nbr );

  if ( CallNbr <= 0 )
    {
      GenKOReply( sClient, TES_KO_BAD_CALL_DATA, session );
      return( -1 );
    }

  if ( pApproveInfo->yes_no == NO )
    TES_cancel_call( CallNbr );

    GenOKReply( sClient, session );
  
}

G7Eta( sClient, pTESMsg, session )
     int         sClient;
     TES_MSG     *pTESMsg;
     int         session;
{    
  char     *pAccountID;
  int      rc = 0;
  CALL_INFO TESCall;
  ACCOUNT_ID ac_id;
  ACCOUNT account;
  CALL_INFO  CallInfo;
  POST_ADDRESS  *rs;
  POST_ADDRESS  PostAddress;
  MT_ZONE       zone;
  short         call_class;
  char          client_type[ AT_SUBSC_TYPE_LEN + 1 ];
  short         eta;

  bzero((char *) &CallInfo, sizeof( CALL_INFO ) );
  pAccountID = (char *)pTESMsg->TESdata;

  TES_initialize();
  
  rc = open_table(ACCOUNT_FILE_ID, DB_READ_WRITE | DB_MANUAL_LOCK);

  ac_id.fleet = 'A';
  strcpy( ac_id.nbr, pAccountID );
  account.account_id = ac_id;
  
  rc = db_read_key(ACCOUNT_FILE_ID, &account, Account_keys[AC_PRIMARY_KEY], DB_EQUAL);
  
  if ( rc != SUCCESS )
    {
      GenKOReply( sClient, TES_KO_BAD_ACCOUNT_DATA, session );
      return( -1 );
    }

  rs = TES_get_account_address( &account.account_id, 1, &PostAddress, NULL );

  if ( rs == NULL )
    {
      GenKOReply( sClient, TES_KO_BAD_ACCOUNT_DATA, session );
      return( -1 );
    }
  
  Account_to_CallInfo( &PostAddress, &CallInfo, &account );

  TES_get_call_class_and_client_type( account.account_type.id, &call_class, client_type );
  zone.fleet = 'A';
  zone.nbr = CallInfo.pickup.zone;
  
  eta = TES_get_zone_busy_rating( &zone, call_class );

  if ( eta <= 0 )
    {
      GenKOReply( sClient, TES_KO_NO_ETA, session );
      return( -1 );
    }
  else
    GenG7EtaReply( sClient, eta, session );
}
  

G7MonitorCall( sClient, pTESMsg, session )
     int         sClient;
     TES_MSG     *pTESMsg;
     int         session;
{
  int            CallNbr;
  int            rc = 0;
  CALL_REC       call_rec;
  CALL_INFO      TESCall;
  time_t         start_time;

  start_time = time(0);

  CallNbr = atoi( (char * )pTESMsg->TESdata );

  if ( CallNbr <= 0 )
    {
      GenKOReply( sClient, TES_KO_BAD_CALL_DATA, session );
      return( -1 );
    }

  call_rec.nbr = CallNbr;

  if (!DB_FILE_IS_OPEN(CALL_FILE_ID))
    {
      rc = db_open(CALL_FILE_ID, DB_READ_ONLY | DB_MANUAL_LOCK);
      if (rc != SUCCESS)
	{
	  GenKOReply( sClient, TES_KO_BAD_CALL_DATA, session );
	  return( -1 );
	}
    }
  rc = db_read_key(CALL_FILE_ID, &call_rec, &cl_key1, DB_EQUAL);
  if (rc != SUCCESS)
    {
      GenKOReply( sClient, TES_KO_BAD_CALL_DATA, session );
      return( -1 );
    }

  while ( 1 )
    {
      rc = db_read_key(CALL_FILE_ID, &call_rec, &cl_key1, DB_EQUAL);
      if ( rc == SUCCESS )
	{
	  if ( strncmp( call_rec.status, CANCELD, strlen( call_rec.status ) ) == 0 )
	    {
	      /** Send TES_CANX message to client **/
	    }

	  /** Vehicle has Accepted call **/
	  if ( call_rec.veh_nbr != 0 )
	    {
	      TES_call_rec_to_info( (HNDL) &call_rec, &TESCall );
	      GenG7CallInfoReply( sClient, &TESCall, session );
	      return( SUCCESS );
	    }
	  sleep( 3 );
	  if ( (time(0) - start_time) > 300 )
	    TerminateConnection( sClient );
	}
      else
	return( -1 );
    }
}
      
G7DispatchClientCall( sClient, pTESMsg, session )
     int         sClient;
     TES_MSG     *pTESMsg;
     int         session;
{
  
  char          *pClientNbr;
  int           ClientNbr;
  int           rc = 0;
  AUTOCALL_CLIENT_REC   *pClientRec;
  CALL_INFO     TESCall;
  
  pClientNbr = (char *)pTESMsg->TESdata;
  ClientNbr = atoi( pClientNbr );

  TES_initialize();

  /** req_buf will contain autocall information on success **/
  if ( ( rc = TES_get_autocall_client( ClientNbr ) ) < 0 )
    {
      GenKOReply( sClient, TES_KO_NO_ACCOUNT, session );
      return( -1 );
    }

  AutoCall_to_CallInfo( &TESCall );
}

AutoCall_to_CallInfo( pTESCall )
     CALL_INFO  *pTESCall;
{
  AUTOCALL_CLIENT_REC  *pClientRec;
  POST_ADDRESS  *Pickup;
  
  pClientRec = ( AUTOCALL_CLIENT_REC * ) req_buf;
  Pickup = &( pTESCall->pickup.cust.address );
  /** need to support multi-fleets if using outside of G7 **/
  pTESCall->fleet = 'A';
  strcpy( pTESCall->account_nbr, pClientRec->AccountNbr );
  strcpy( Pickup->street.name, pClientRec->PckupStrName );
  strcpy( Pickup->street.type, pClientRec->PckupStrType );
  Pickup->street_nbr = pClientRec->PckupStrNbr;
  strcpy( Pickup->city_name, pClientRec->PckupLocality );
  strcpy( pTESCall->pickup.cust.name, pClientRec->Passenger );
  pTESCall->pickup.due_date_time = time(0);
  TES_send_call( pTESCall );
}
  
