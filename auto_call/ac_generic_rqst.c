/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ac_generic_rqst.c,v $
*  @(#)  $Revision: 1.7 $
*  @(#)  $Date: 2004/02/02 18:55:32 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/auto_call/ac_generic_rqst.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: ac_generic_rqst.c,v 1.7 2004/02/02 18:55:32 jwelch Exp $";

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>
#include "taxipak.h"
#include "language.h"
#include "taxi_defs.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "Object.h"
#include "Account_db.h"
#include "AutoCall_private.h"
#include "TPakWeb_User.h"
#include "TPakWeb_UserAddress.h"
#include "TES.h"

StartedRqst( sClient, session )
     int  sClient;
     int  session;
{
  GenOKReply( sClient, session );
}

TerminateRqst( sClient, session )
     int  sClient;
     int  session;
{
  GenOKReply( sClient, session );
  
  TerminateConnection( sClient, session );
  exit( -1 );
}

CancelCall( sClient, pTESMsg, session )
     int      sClient;
     TES_MSG  *pTESMsg;
     int      session;
{
  int       CallNbr;

  CallNbr =  atoi((char *)pTESMsg->TESdata );

  if ( CallNbr <= 0 )
    {
      GenKOReply( sClient, TES_CALL_NOCANX, session );
      return( -1 );
    }

  if ( TES_cancel_call( CallNbr ) == SUCCESS )
    GenCallCancelReply( sClient, session );
  else
    GenNoCallCancelReply( sClient, TES_CALL_NOCANX, session );
}

AccountInfo( sClient, pTESMsg, session )
          int      sClient;
     TES_MSG  *pTESMsg;
     int      session;
{
  char     AccountID[TES_AC_NBR_LEN + 1];
  char     Acad[TES_ACAD_LEN + 1];
  char     Subacct[TES_SUBACCT_LEN + 1];
  char     NbrTaxis[TES_NBR_TAXIS_LEN + 1];
  int      rc = 0, i, iNbrTaxis = 0;
  LOC_REC loc_rec;  
  ACCOUNT_REC account;
  ACCOUNT_ID ac_id;
  POST_ADDRESS  *rs;
  POST_ADDRESS  PostAddress;
  CALL_INFO     CallInfo;

    if ( strlen( pTESMsg->TESdata ) != LEN_TES_REQ_ACCT_CALL )
    return( -1 );

  memcpy( AccountID, &pTESMsg->TESdata[TES_AC_NBR_OFFSET], TES_AC_NBR_LEN );
  AccountID[TES_AC_NBR_LEN] = EOS;
  memcpy( Subacct, &pTESMsg->TESdata[TES_SUBACCT_OFFSET], TES_SUBACCT_LEN );
  Subacct[TES_SUBACCT_LEN] = EOS;
  memcpy( Acad, &pTESMsg->TESdata[TES_ACAD_OFFSET], TES_ACAD_LEN );
  Acad[TES_ACAD_LEN] = EOS;
  memcpy( NbrTaxis, &pTESMsg->TESdata[TES_NBR_TAXIS_OFFSET], TES_NBR_TAXIS_LEN );
  NbrTaxis[TES_NBR_TAXIS_LEN] = EOS;

  TES_initialize();

  rc = open_table(ACCOUNT_FILE_ID, DB_READ_WRITE | DB_MANUAL_LOCK);

  if ( rc != SUCCESS )
    {
      GenKOAcctReply( sClient, session );
      return( -1 );
    }

  /** add multi-fleet support later **/
  if ( YES == 'K' )
    ac_id.fleet = 'H';
  else
    ac_id.fleet = 'A';
  
  strcpy( ac_id.nbr, AccountID );
  account.fleet = ac_id.fleet;
  strcpy( account.nbr, ac_id.nbr );

  rc = db_read_key(ACCOUNT_FILE_ID, &account, &ac_key1, DB_EQUAL);

  if ( rc != SUCCESS )
    {
      GenKOAcctReply( sClient, session );
      return( -1 );
    }

  rs =  TES_get_account_address( &ac_id, atoi( Acad ), &PostAddress, NULL );

  if ( rs == NULL )
    {
      GenKOAcctReply( sClient, session );
      return( -1 );
    }

  // Translate CITY code to CITY name
  rc = db_open(LOCALITY_FILE_ID, DB_READ_WRITE | DB_MANUAL_LOCK);
  if ( rc == SUCCESS )
    {
      bzero(&loc_rec, sizeof(LOC_REC) );
      strcpy(loc_rec.code, PostAddress.city_name);
      if ( ( rc = db_read_key( LOCALITY_FILE_ID, &loc_rec, &lo_key1, DB_EQUAL ) ) == SUCCESS )
	strncpy(PostAddress.city_name, loc_rec.full_name, 20);
      db_close(LOCALITY_FILE_ID);
    }

  if ( atoi( Subacct ) != 0 )
    {
      strcpy( CallInfo.sub_acct, Subacct );      
      TES_get_indv_info( &ac_id, &CallInfo );
      if ( strlen( CallInfo.requested_by ) <= 0 )
	{
	  GenKOAcctReply( sClient, session );
	  return( -1 );
	}
    }

  GenOKAccountInfoReply( sClient, session, &PostAddress );
}

AccountValidate( sClient, pTESMsg, session )
     int      sClient;
     TES_MSG  *pTESMsg;
     int      session;
{
  char     AccountID[TES_AC_NBR_LEN + 1];
  char     Acad[TES_ACAD_LEN + 1];
  char     Subacct[TES_SUBACCT_LEN + 1];
  char     NbrTaxis[TES_NBR_TAXIS_LEN + 1];
  int      rc = 0, i, iNbrTaxis = 0;
  ACCOUNT_REC account;
  ACCOUNT_ID ac_id;
  POST_ADDRESS  *rs;
  POST_ADDRESS  PostAddress;
  CALL_INFO     CallInfo;

    if ( strlen( pTESMsg->TESdata ) != LEN_TES_REQ_ACCT_CALL )
    return( -1 );
  
  memcpy( AccountID, &pTESMsg->TESdata[TES_AC_NBR_OFFSET], TES_AC_NBR_LEN );
  AccountID[TES_AC_NBR_LEN] = EOS;
  memcpy( Subacct, &pTESMsg->TESdata[TES_SUBACCT_OFFSET], TES_SUBACCT_LEN );
  Subacct[TES_SUBACCT_LEN] = EOS;
  memcpy( Acad, &pTESMsg->TESdata[TES_ACAD_OFFSET], TES_ACAD_LEN );
  Acad[TES_ACAD_LEN] = EOS;
  memcpy( NbrTaxis, &pTESMsg->TESdata[TES_NBR_TAXIS_OFFSET], TES_NBR_TAXIS_LEN );
  NbrTaxis[TES_NBR_TAXIS_LEN] = EOS;

  TES_initialize();

  rc = open_table(ACCOUNT_FILE_ID, DB_READ_WRITE | DB_MANUAL_LOCK);

  if ( rc != SUCCESS )
    {
      GenKOAcctReply( sClient, session );
      return( -1 );
    }

  /** add multi-fleet support later **/
  if ( YES == 'K' )
    ac_id.fleet = 'H';
  else
    ac_id.fleet = 'A';
  
  strcpy( ac_id.nbr, AccountID );
  account.fleet = ac_id.fleet;
  strcpy( account.nbr, ac_id.nbr );

  rc = db_read_key(ACCOUNT_FILE_ID, &account, &ac_key1, DB_EQUAL);

  if ( rc != SUCCESS )
    {
      GenKOAcctReply( sClient, session );
      return( -1 );
    }

  rs =  TES_get_account_address( &ac_id, atoi( Acad ), &PostAddress, NULL );

  if ( rs == NULL )
    {
      GenKOAcctReply( sClient, session );
      return( -1 );
    }

  if ( atoi( Subacct ) != 0 )
    {
      strcpy( CallInfo.sub_acct, Subacct );      
      TES_get_indv_info( &ac_id, &CallInfo );
      if ( strlen( CallInfo.requested_by ) <= 0 )
	{
	  GenKOAcctReply( sClient, session );
	  return( -1 );
	}
    }

  GenOKAcctReply( sClient, session );

  return ( 1 );
}

ProfileGet( sClient, pTESMsg, session )
     int      sClient;
     TES_MSG  *pTESMsg;
     int      session;
{
  WEBUSER  webuser;
  WEB_USER_MSG *pWebUser;
  
  bzero( &webuser, sizeof( webuser ) );

  pWebUser = ( WEB_USER_MSG * )pTESMsg->TESdata;
  strcpy( webuser.user_name, pWebUser->user_name );
  
  if ( db_read_key( WEBUSER_FILE_ID, &webuser, &webuser_key2, ISEQUAL ) != SUCCESS )
      {
	  GenKOAcctReply( sClient, session );
	  return(-1);
      }
  GenProfileReply( sClient, &webuser, session );
  
}

ProfileAdd( sClient, pTESMsg, session )
     int       sClient;
     TES_MSG   *pTESMsg;
     int       session;
{
  WEBUSER      webuser;
  WEB_USER_MSG  *pWebUser;

  bzero( &webuser, sizeof( webuser ) );

  pWebUser = ( WEB_USER_MSG * )pTESMsg->TESdata;
  strcpy( webuser.user_name, pWebUser->user_name );
  sprintf( webuser.passwd, "%-13.13s", pWebUser->passwd );
  sprintf( webuser.fullname, "%-21.21s", pWebUser->fullname );
  sprintf( webuser.tele, "%-17.17s", pWebUser->tele );
  sprintf( webuser.email, "%-33.33s", pWebUser->email );
  sprintf( webuser.service, "%-33.33s", pWebUser->service );
  db_write( WEBUSER_FILE_ID, &webuser );
  
}


ProfileUpdate( sClient, pTESMsg, session )
     int       sClient;
     TES_MSG   *pTESMsg;
     int       session;
{
  WEBUSER      webuser;
  WEB_USER_MSG  *pWebUser;

  bzero( &webuser, sizeof( webuser ) );


  pWebUser = ( WEB_USER_MSG * )pTESMsg->TESdata;
  
  webuser.nbr = atoi( pWebUser->user_id );
  if ( db_read_key( WEBUSER_FILE_ID, &webuser, &webuser_key1, ISEQUAL ) == SUCCESS )
    {
      strcpy( webuser.user_name, pWebUser->user_name );
      sprintf( webuser.passwd, "%-13.13s", pWebUser->passwd );
      sprintf( webuser.fullname, "%-21.21s", pWebUser->fullname );
      sprintf( webuser.tele, "%-17.17s", pWebUser->tele );
      sprintf( webuser.email, "%-33.33s", pWebUser->email );
      db_update( WEBUSER_FILE_ID, &webuser );
    }
  
}

ProfileRemove( sClient, pTESMsg, session )
     int       sClient;
     TES_MSG   *pTESMsg;
     int       session;
{
  WEBUSER      webuser;
  WEB_USER_MSG  *pWebUser;

  bzero( &webuser, sizeof( webuser ) );


  pWebUser = ( WEB_USER_MSG * )pTESMsg->TESdata;
  
  webuser.nbr = atoi( pWebUser->user_id );
  if ( db_read_key( WEBUSER_FILE_ID, &webuser, &webuser_key1, ISEQUAL ) == SUCCESS )
    {
      db_delete( WEBUSER_FILE_ID, &webuser );
    }
}

ProfileAddressGet( sClient, pTESMsg, session )
     int       sClient;
     TES_MSG   *pTESMsg;
     int       session;
{

  WEB_ADDR_MSG   *web_addr_msg;
  WEBADDR       web_addr;
  int            web_usr_nbr;

  bzero( &web_addr, sizeof( WEBADDR ) );
  web_addr_msg = ( WEB_ADDR_MSG * )pTESMsg->TESdata;
  web_addr.webuser_nbr = atoi( web_addr_msg->webuser_nbr );

  if ( db_read_key( WEBADDR_FILE_ID, &web_addr, &webaddr_key1, ISEQUAL ) != SUCCESS )
    {
      GenKOAddrReply( sClient, session, TES_KO_NO_ADDR );
    }
  else
    GenOKAddrReply( sClient, session, &web_addr );

}

ProfileAddressUpdate( sClient, pTESMsg, session )
     int       sClient;
     TES_MSG   *pTESMsg;
     int       session;
{
}

ProfileAddressAdd( sClient, pTESMsg, session )
     int       sClient;
     TES_MSG   *pTESMsg;
     int       session;
{
}

ProfileAddressRemove( sClient, pTESMsg, session )
     int       sClient;
     TES_MSG   *pTESMsg;
     int       session;
{
}

AccountCallRqst( sClient, pTESMsg, session )
     int      sClient;
     TES_MSG  *pTESMsg;
     int      session;
{
  char     AccountID[TES_AC_NBR_LEN + 1];
  char     Acad[TES_ACAD_LEN + 1];
  char     Subacct[TES_SUBACCT_LEN + 1];
  char     NbrTaxis[TES_NBR_TAXIS_LEN + 1];
  int      rc = 0, i, iNbrTaxis = 0;
  CALL_INFO TESCall;
  ACCOUNT_REC account;
  ACCOUNT_ID ac_id;  
  CALL_INFO  CallInfo;
  POST_ADDRESS  *rs;
  POST_ADDRESS  PostAddress;

  bzero((char *) &CallInfo, sizeof( CALL_INFO ) );
  if ( strlen( pTESMsg->TESdata ) != LEN_TES_REQ_ACCT_CALL )
    {
      GenKOAcctReply( sClient, session );
      return( -1 );
    }
  
  memcpy( AccountID, &pTESMsg->TESdata[TES_AC_NBR_OFFSET], TES_AC_NBR_LEN );
  AccountID[TES_AC_NBR_LEN] = EOS;
  memcpy( Subacct, &pTESMsg->TESdata[TES_SUBACCT_OFFSET], TES_SUBACCT_LEN );
  Subacct[TES_SUBACCT_LEN] = EOS;
  memcpy( Acad, &pTESMsg->TESdata[TES_ACAD_OFFSET], TES_ACAD_LEN );
  Acad[TES_ACAD_LEN] = EOS;
  memcpy( NbrTaxis, &pTESMsg->TESdata[TES_NBR_TAXIS_OFFSET], TES_NBR_TAXIS_LEN );
  NbrTaxis[TES_NBR_TAXIS_LEN] = EOS;

  TES_initialize();

  rc = open_table(ACCOUNT_FILE_ID, DB_READ_WRITE | DB_MANUAL_LOCK);

  if ( rc != SUCCESS )
    {
      GenKOAcctReply( sClient, session );
      return( -1 );
    }

  /** add multi-fleet support later **/
  if ( YES == 'K' )
    ac_id.fleet = 'H';
  else
    ac_id.fleet = 'A';
  
  strcpy( ac_id.nbr, AccountID );
  account.fleet = ac_id.fleet;
  strcpy( account.nbr, ac_id.nbr );

  rc = db_read_key(ACCOUNT_FILE_ID, &account, &ac_key1, DB_EQUAL);

  if ( rc != SUCCESS )
    {
      GenKOAcctReply( sClient, session );
      return( -1 );
    }

  rs =  TES_get_account_address( &ac_id, atoi( Acad ), &PostAddress, NULL );
  
  if ( rs == NULL )
    {
      GenKOAcctReply( sClient, session );
      return( -1 );
    }
  
  Account_to_CallInfo( &PostAddress, &CallInfo, &account );

  strcpy( CallInfo.sub_acct, Subacct );
  TES_get_indv_info( &ac_id, &CallInfo );

  iNbrTaxis = atoi( NbrTaxis );
  if ( iNbrTaxis > 10 )
    iNbrTaxis = 10;

  i = 0;
  while ( i < iNbrTaxis )
    {
      CallInfo.nbr_in_group = iNbrTaxis;
      CallInfo.pos_in_group = i + 1;
      TES_send_call( &CallInfo );
      
      if ( CallInfo.call_nbr > 0 )
	{
	  GenCallNbrReply( sClient, CallInfo.call_nbr, session );
	  rc = 0;
	}
      else
	{
	  GenKOAcctReply( sClient, session );
	  rc = 1;
	}
      CallInfo.call_nbr = 0;
      i++;
    }
    return ( rc );
}


Account_to_CallInfo( pPostAddress, pTESCall, pAccount )
     POST_ADDRESS       *pPostAddress;
     CALL_INFO     *pTESCall;
     ACCOUNT_REC       *pAccount;
{
  POST_ADDRESS     *Pickup;
  int zone;
  int zip;
  int i;

  Pickup = &( pTESCall->pickup.cust.address );

  pTESCall->fleet = pAccount->fleet;
  strcpy( pTESCall->account_nbr, pAccount->nbr );
  pTESCall->pickup.due_date_time = time( 0 );

  /* set some attributes */
  for ( i = 0; i < ATTR_MAX; i++ )
    {
      if ( pAccount->drv_attr[i] == YES )
	pTESCall->pickup.requested_attributes.driver[i] = YES;
      else
	pTESCall->pickup.requested_attributes.driver[i] = NO;

      if ( pAccount->veh_attr[i] == YES )
	pTESCall->pickup.requested_attributes.vehicle[i] = YES;
      else
	pTESCall->pickup.requested_attributes.vehicle[i] = NO;
    }

  //Hard coding some attributes for Helsinki Taxi
  pTESCall->pickup.requested_attributes.driver[19] = YES;
  pTESCall->pickup.requested_attributes.driver[25] = YES;
  pTESCall->pickup.requested_attributes.vehicle[25] = YES;

  if ( strlen( pAccount->driver_msg ) )
    strcpy( pTESCall->pickup.general_comments, pAccount->driver_msg );

  strcpy( Pickup->street.name, pPostAddress->street.name );
  Pickup->street_nbr = pPostAddress->street_nbr;
  strcpy( Pickup->street.type, pPostAddress->street.type );
  strcpy( Pickup->city_name, pPostAddress->city_name );
  strcpy( Pickup->apt, pPostAddress->apt );

  pTESCall->priority = pAccount->priority;
  
  /** Now we can attempt to zone the address **/
  zip = 0;
  zone = ac_zone_addr( pTESCall->fleet, &pTESCall->pickup.cust.address, &zip );
  if ( zone > 0 )
    pTESCall->pickup.zone = zone;
  
  
}
  
writer_add_hist()
{
}
