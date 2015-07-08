/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ac_generic_rqst.c,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2004/02/02 18:55:53 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/auto_tes/ac_generic_rqst.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: ac_generic_rqst.c,v $	$Revision: 1.5 $"

static char rcsid[] = "$Id: ac_generic_rqst.c,v 1.5 2004/02/02 18:55:53 jwelch Exp $";

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <errno.h>
#include <math.h>
#include "taxipak.h"
#include "language.h"
#include "writer.h"
#include "taxi_defs.h"
#include "mem_mgmt.h"
#include "mads_types.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "Object.h"
#include "Account.h"
#include "Account_db.h"
#include "Call_public.h"
#include "Call_private.h"
#include "Call_db.h"
#include "Auto_TES.h"
#include "TES.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Zone_public.h"
#include "Zone_private.h"

#include "Zone_private.h"
#include "Zone_db.h"



extern struct fleets *fleet[];
void  t_out_hdlr();

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
      GenKOReply( sClient, TES_KO_BAD_CALL_DATA, session );
      return( -1 );
    }

  if ( TES_cancel_call( CallNbr ) == SUCCESS )
    GenOKReply( sClient, session );
  else
    GenKOReply( sClient, TES_KO_BAD_CALL_DATA, session );
}

AddressValidRqst( sClient, pTESMsg, session )
     int      sClient;
     TES_MSG  *pTESMsg;
     int      session;
{
  TES_ADDR     *pAddress;
  POST_ADDRESS Pickup;  
  char         tmp_buf[ 32 ];
  int          str_nbr;
  int zone = 0;
  int zip;    

  bzero( &Pickup, sizeof( POST_ADDRESS ) );
  pAddress = (TES_ADDR *)pTESMsg->TESdata;
  TES_initialize();

  memcpy( tmp_buf, pAddress->str_nbr, ADDR_STR_NBR_LEN );
  tmp_buf[ADDR_STR_NBR_LEN] = '\0';
  str_nbr = atoi( tmp_buf );
  Pickup.street_nbr = str_nbr;

  memcpy( tmp_buf, pAddress->str_name, ADDR_STR_NAME_LEN );
  tmp_buf[ADDR_STR_NAME_LEN] = '\0';
  strcpy( Pickup.street.name, tmp_buf );
  strtoupper( Pickup.street.name );

  memcpy( tmp_buf, pAddress->str_city, ADDR_STR_CITY_LEN );  
  tmp_buf[ADDR_STR_CITY_LEN] = '\0';
  strcpy( Pickup.city_name, tmp_buf );
  strtoupper( Pickup.city_name );  

  zip = 0;
  zone = ac_zone_addr( pAddress->fleet, &Pickup, &zip );

  if ( zone <= 0 )
    {
      GenKOReply( sClient, TES_KO_BAD_ADDR, session );
      return(0);
    }
  
  sprintf( pAddress->zone, "%3d", zone );
  sprintf( pAddress->gps_x, "%08d", Pickup.gps_x );
  sprintf( pAddress->gps_y, "%08d", Pickup.gps_y );  
  
  GenAddrReply( sClient, pAddress, session );
  return(0);
}

  
AccountCallRqst( sClient, pTESMsg, session )
     int      sClient;
     TES_MSG  *pTESMsg;
     int      session;
{
  TES_ACCOUNT *pAccountID;
  int      rc = 0;
  CALL_INFO TESCall;
  ACCOUNT_ID ac_id;
  ACCOUNT account;
  CALL_INFO  CallInfo;
  POST_ADDRESS  *rs;
  POST_ADDRESS  PostAddress;

  bzero((char *) &CallInfo, sizeof( CALL_INFO ) );
  pAccountID = (TES_ACCOUNT *)pTESMsg->TESdata;

  TES_initialize();

  rc = open_table(ACCOUNT_FILE_ID, DB_READ_WRITE | DB_MANUAL_LOCK);

  if ( rc != SUCCESS )
    {
      GenKOReply( sClient, TES_KO_BAD_ACCOUNT_DATA, session );
      return( -1 );
    }

  ac_id.fleet = pAccountID->fleet;
  strcpy( ac_id.nbr, pAccountID->account_nbr );
  account.fleet = ac_id.fleet;
  strcpy(account.nbr, ac_id.nbr);

  rc = db_read_key(ACCOUNT_FILE_ID, &account, Account_keys[AC_PRIMARY_KEY], DB_EQUAL);

  if ( rc != SUCCESS )
    {
      GenKOReply( sClient, TES_KO_BAD_ACCOUNT_DATA, session );
      return( -1 );
    }

  rs =  TES_get_account_address( &ac_id, 1, &PostAddress, NULL );
  
  if ( rs == NULL )
    {
      GenKOReply( sClient, TES_KO_BAD_ACCOUNT_DATA, session );
      return( -1 );
    }
  
  Account_to_CallInfo( &PostAddress, &CallInfo, &account );

  TES_send_call( &CallInfo );

  if ( CallInfo.call_nbr > 0 )
    {
      GenCallNbrReply( sClient, CallInfo.call_nbr, session );
      return( 0 );
    }
  else
    {
      GenKOReply( sClient, TES_KO_CALL_STATE, session );
      return( -1 );
    }
}

Account_to_CallInfo( pPostAddress, pTESCall, pAccount )
     POST_ADDRESS       *pPostAddress;
     CALL_INFO     *pTESCall;
     ACCOUNT       *pAccount;
{
  POST_ADDRESS     *Pickup;
  int zone;
  int zip;  

  Pickup = &( pTESCall->pickup.cust.address );

  pTESCall->fleet = pAccount->fleet;
  strcpy( pTESCall->account_nbr, pAccount->nbr );
  pTESCall->pickup.due_date_time = time( 0 );

  strcpy( Pickup->street.name, pPostAddress->street.name );
  Pickup->street_nbr = pPostAddress->street_nbr;
  strcpy( Pickup->street.type, pPostAddress->street.type );
  strcpy( Pickup->city_name, pPostAddress->city_name );

  strncpy(pTESCall->pickup.requested_attributes.vehicle, pAccount->veh_attr, ATTR_MAX);
  pTESCall->pickup.requested_attributes.vehicle[ATTR_MAX] = EOS;

  strncpy(pTESCall->pickup.requested_attributes.driver, pAccount->drv_attr, ATTR_MAX);
  pTESCall->pickup.requested_attributes.driver[ATTR_MAX] = EOS;
  
  /** Now we can attempt to zone the address **/
  zip = 0;  
  zone = ac_zone_addr( pTESCall->fleet, &pTESCall->pickup.cust.address, &zip );
  
  // zone = TES_zone_address( pTESCall->fleet, &pTESCall->pickup.cust.address );
  if ( zone > 0 )
    pTESCall->pickup.zone = zone;
  
  
}
  
DispatchDetailedCall( sClient, pTESMsg, session )
     int                       sClient;
     TES_MSG            *pTESMsg;
     int                       session;
{
    char                      *pTESData;
    int                         rc = 0, i;
    CALL_PARAM       CallParam;
    CALL_INFO           CallInfo;

    bzero( &CallInfo, sizeof( CALL_INFO ) );
    pTESData = pTESMsg->TESdata;

    if ( strlen( pTESData) != CP_LENGTH ) 
	{
	    GenKOReply( sClient, TES_KO_BAD_CALL_DATA, session );
	    return( -1 );
	}
    
    // Load it up!
    DetailedCall_to_CallParam( pTESData, &CallParam );
    
    TES_initialize();
    
    CallParam_to_CallInfo( &CallParam, &CallInfo );

    if ( CallInfo.pickup.zone <= 0 )
	{
	    GenKOReply( sClient, TES_KO_UNZONED, session );
	    return(-1);
	}

    strcpy( CallInfo.pickup.general_comments, CallParam.Comments );
    strcpy( CallInfo.pickup.cust.address.apt, CallParam.Apartment );

    for ( i = 0; i < 32; i++ )
      {
        if ( CallParam.VehAttr[i] == YES )
          CallInfo.pickup.requested_attributes.vehicle[i] = YES;
        else
          CallInfo.pickup.requested_attributes.vehicle[i] = NO;

        if ( CallParam.DrvAttr[i] == YES )
          CallInfo.pickup.requested_attributes.driver[i] = YES;
        else
          CallInfo.pickup.requested_attributes.driver[i] = NO;
      }

    // Hard coding some vehicle attributes for Helsinki Taxi    
    CallInfo.pickup.requested_attributes.vehicle[4] = YES;
    CallInfo.pickup.requested_attributes.vehicle[5] = YES;
    CallInfo.pickup.requested_attributes.vehicle[6] = YES;
    CallInfo.pickup.requested_attributes.vehicle[8] = YES;
    CallInfo.pickup.requested_attributes.vehicle[9] = YES;
    CallInfo.pickup.requested_attributes.driver[23] = YES;

    // Premium order
    CallInfo.pickup.requested_attributes.vehicle[25] = YES;
    CallInfo.pickup.requested_attributes.driver[25] = YES;      
    
    TES_send_call( &CallInfo );
    if ( CallInfo.call_nbr > 0 )
	{
	    GenCallNbrReply( sClient, CallInfo.call_nbr, session );
	    return(0);
	}
    else
	{
	    GenKOReply( sClient, TES_KO_CALL_STATE, session );
	    return(-1);
	}
    
}

MonitorCall( int sClient, TES_MSG *pTESMsg, int session )
{
  int   CallNbr, iDuration;
  int   rc = 0;
  char  *pTESdata;
  CALL_REC   call_rec;
  CALL_INFO  TESCall;
  char       sCallNbr[16];
  char       sDuration[8];
  time_t     start_time;

  start_time = time(0);

  pTESdata = (char *)pTESMsg->TESdata;
  
  bzero( sCallNbr, sizeof(sCallNbr) );
  bzero( sDuration, sizeof(sDuration) );
  strncpy( sCallNbr, pTESdata, 8 );
  
  CallNbr = atoi( sCallNbr );

  if ( strlen( pTESdata ) > 8 )
    {
      strncpy( sDuration, (pTESdata + 8), 3 );
      iDuration = atoi( sDuration );
    }
  else
    iDuration = 300;  // monitor for 5 minutes by default

  if ( iDuration > 3000 )
    iDuration = 3000;
  
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
	      GenKOReply( sClient, TES_KO_CALL_STATE, session );
	      return( -1 );
	    }

	  /** Vehicle has Accepted call **/
	  if ( call_rec.veh_nbr != 0 )
	    {
	      GenCallInfoReply( sClient, &call_rec, session );
	      return( SUCCESS );
	    }
	  sleep( 3 );
	  if ( (time(0) - start_time) > iDuration )
	    {
	      GenKOReply( sClient, TES_KO_NO_TAXIS, session );	      
	      TES_cancel_call( call_rec.nbr );	      
	      return( 99 );
	    }
	}
      else
	return( -1 );
    }
}

ReqCallDetails( sClient, pTESMsg, session )
     int                       sClient;
     TES_MSG            *pTESMsg;
     int                       session;
{
    char                      *pTESData;
    int                         rc = 0;
    CALL_REC            call_rec;

    pTESData = pTESMsg->TESdata;

    if ( strlen( pTESData ) != CP_NBR_LEN )
	{
	    GenKOReply( sClient, TES_KO_BAD_CALL_DATA, session );
	    return( -1 );
	}

    bzero( &call_rec, sizeof( CALL_REC ) );

    call_rec.nbr = atoi( (char *)pTESData );

    if ( call_rec.nbr <= 0 )
	{
	    GenKOReply( sClient, TES_KO_BAD_CALL_DATA, session );
	    return( -1 );
	}

    if (!DB_FILE_IS_OPEN(CALL_FILE_ID))
      {
        if ( ( rc = db_open( CALL_FILE_ID, DB_READ_WRITE | DB_MANUAL_LOCK ) ) != SUCCESS )
          {
	    GenKOReply( sClient, TES_KO_BAD_CALL_DATA, session );
	    return( -1 );
          }
      }

    if ( ( rc = db_read_key( CALL_FILE_ID, &call_rec, &cl_key1, DB_EQUAL ) ) != SUCCESS )
	{
	    GenKOReply( sClient, TES_KO_NO_CALL, session );
	    return( -1 );
	}	    

    GenCallDetailsReply( sClient, &call_rec, session );
    return(0);
}

ReqStandTaxi( sClient, pTESMsg, session )
     int                       sClient;
     TES_MSG            *pTESMsg;
     int                       session;
{
    char                      *pTESData;
    int                         rc = 0, times_thru;
    char                 b_number[32], query_id[64], latClient[16], lonClient[16];
    struct zone_stand_hold zrq;
    DRV_INFO             drv;
    
    struct zone_stand_hold  *zrp;
    ZONE_RESP_HNDL hZoneResp;
    ZONE_REC  zone_rec;
    int pid = getpid();
    int msgkey;
    char  *p = NULL;

    pTESData = pTESMsg->TESdata;

    if ( strlen( pTESData ) < 4 )
	{
	    GenKOReply( sClient, TES_KO_BAD_CALL_DATA, session );
	    return( -1 );
	}

    // Received data is of the form <phone_number>!<query_id>
    p = (char *)strtok( pTESData, "!" );
    if ( p != NULL )
      strcpy( b_number, p );
    else
      {
        bzero( &drv, sizeof(DRV_INFO) );
        sprintf( drv.drv_info_id, "%08d", 0 );        
        sprintf( drv.drv_info_tele, "%-18s", "0" );        
        GenStandTaxiReply( sClient, &drv, session );                
        return( -1 );
      }

    p = (char *)strtok((char *)NULL, "!");
    if ( p != NULL )
      strcpy( query_id, p );
    else
      {
        bzero( &drv, sizeof(DRV_INFO) );
        sprintf( drv.drv_info_id, "%08d", 0 );        
        sprintf( drv.drv_info_tele, "%-18s", "0" );        
        GenStandTaxiReply( sClient, &drv, session );                        
        return( -1 );
      }        

    if ( (msgkey = msgget(MSGKEY,IPC_CREAT|0666)) == -1 ) 
      return(-1);


    // Lookup the stand telephone
    strcpy( zone_rec.stand_telephone, b_number );
    
    db_open( ZONE_FILE_ID, DB_READ_ONLY|DB_MANUAL_LOCK );
    if ( db_read_key( ZONE_FILE_ID, &zone_rec, &zn_key2, ISEQUAL ) != SUCCESS )  // invalid tele number
      {
        bzero( &drv, sizeof(DRV_INFO) );
        sprintf( drv.drv_info_id, "%08d", 0 );        
        sprintf( drv.drv_info_tele, "%-18s", "0" );        
        GenStandTaxiReply( sClient, &drv, session );        
        return(0);
      }
    if ( zone_rec.type != '1' )  // not a taxi stand
      {
        bzero( &drv, sizeof(DRV_INFO) );
        sprintf( drv.drv_info_id, "%08d", 0 );        
        sprintf( drv.drv_info_tele, "%-18s", "0" );        
        GenStandTaxiReply( sClient, &drv, session );        
        return(0);
      }        

    
    zrq.d_prcs_id = DISP_DEST;						/* send ipc to Dispatch */
    zrq.u_prcs_id = pid;						/* user's process id */
    zrq.rec_type = ZONE_STAND_HOLD;						/* zone request type */
    zrq.fleet = zone_rec.fleet;
    zrq.zone_nbr = zone_rec.nbr;
    strcpy( zrq.query_id, query_id );
    
    msgsnd(msgkey,(struct msgbuf *)&zrq,sizeof(struct zone_stand_hold), IPC_NOWAIT);	/* send ipc message */

    times_thru = 0;    
    hZoneResp = (HNDL)malloc( sizeof(struct zone_stand_hold) );
    zrp = ( struct zone_stand_hold * )hZoneResp;

    signal(SIGALRM, t_out_hdlr);
    while (times_thru <=2 )
      {
	alarm(30);
	rc = msgrcv(msgkey,(void *)zrp,sizeof(struct zone_stand_hold),(long) pid,0);
        ++times_thru;
        if ( zrp->rec_type == ZONE_STAND_HOLD )
          break;
      }
    alarm(0);
    if ( zrp->veh_nbr > 0 )
      {
        bzero( &drv, sizeof(DRV_INFO) );
        sprintf( drv.drv_info_id, "%08d", zrp->veh_nbr );        
        sprintf( drv.drv_info_tele, "%-18s", zrp->veh_tele );
      }
    else
      {
        bzero( &drv, sizeof(DRV_INFO) );
        sprintf( drv.drv_info_id, "%08d", 0 );        
        sprintf( drv.drv_info_tele, "%-18s", "0" );
      }        
    
    GenStandTaxiReply( sClient, &drv, session );
    free( zrp );
    return(0);
}

ReqStandTaxiV2( sClient, pTESMsg, session )
     int                       sClient;
     TES_MSG            *pTESMsg;
     int                       session;
{
    char                      *pTESData;
    int                         rc = 0, times_thru;
    char                 b_number[32], query_id[64], latClient[16], lonClient[16];
    struct zone_stand_hold zrq;
    DRV_INFO             drv;
    char logMsg[128];
    struct zone_stand_hold  *zrp;
    ZONE_RESP_HNDL hZoneResp;
    ZONE_REC  zone_rec;
    int pid = getpid();
    int msgkey;
    char  *p = NULL;
    double custX = 0.0, custY = 0.0;
    double Northing, Easting;
    short queryZone = 0;
    double degreeVal = 0.0;
    double minuteVal = 0.0;
    double secondsVal = 0.0;
    
    char             deg_buf[8];
    char             min_buf[8];
    char             sec_buf[8];
    
    pTESData = pTESMsg->TESdata;

    if ( strlen( pTESData ) < 4 )
      {
	GenKOReply( sClient, TES_KO_BAD_CALL_DATA, session );
	return( -1 );
      }
    
    // Received data is of the form <lat>!<lon>!<phone_number>!<query_id>
    // LAT = dd.mmss
    // LON = dd.mmss
    p = (char *)strtok( pTESData, "!" );
    if ( p != NULL )
      strcpy( latClient, p );
    else
      {
        bzero( &drv, sizeof(DRV_INFO) );
        sprintf( drv.drv_info_id, "%08d", 0 );        
        sprintf( drv.drv_info_tele, "%-18s", "0" );        
        GenStandTaxiReply( sClient, &drv, session );                
        return( -1 );
      }

    p = (char *)strtok((char *)NULL, "!");
    if ( p != NULL )
      strcpy( lonClient, p );
    else
      {
        bzero( &drv, sizeof(DRV_INFO) );
        sprintf( drv.drv_info_id, "%08d", 0 );        
        sprintf( drv.drv_info_tele, "%-18s", "0" );        
        GenStandTaxiReply( sClient, &drv, session );                        
        return( -1 );
      }        

    p = (char *)strtok((char *)NULL, "!");
    if ( p != NULL )
      strcpy( b_number, p );
    else
      {
	bzero( &drv, sizeof(DRV_INFO) );
        sprintf( drv.drv_info_id, "%08d", 0 );        
        sprintf( drv.drv_info_tele, "%-18s", "0" );        
        GenStandTaxiReply( sClient, &drv, session );                        
        return( -1 );
      }

    p = (char *)strtok((char *)NULL, "!");
    if ( p!= NULL )
      strcpy( query_id, p );
    else
      {
	bzero( &drv, sizeof(DRV_INFO) );
        sprintf( drv.drv_info_id, "%08d", 0 );        
        sprintf( drv.drv_info_tele, "%-18s", "0" );        
        GenStandTaxiReply( sClient, &drv, session );                        
        return( -1 );
      }      
	
    if ( (msgkey = msgget(MSGKEY,IPC_CREAT|0666)) == -1 ) 
      return(-1);


    // Clear out requests with EMPTY positions
    if (!strncmp(lonClient, "MPTY", 4) )
      {
	sprintf(logMsg, "EMPTY COORDINATES RECEIVED...");
	Generic_logMsg( sClient, logMsg, session );
	bzero( &drv, sizeof(DRV_INFO) );
	sprintf( drv.drv_info_id, "%08d", 0 );
	sprintf( drv.drv_info_tele, "%-18s", "0");
	GenStandTaxiReply( sClient, &drv, session );
	return(0);
      }
    
    // Use the LAT-LON to determine the zone that geofences this position
    
    // Lookup the stand telephone
    strcpy( zone_rec.stand_telephone, b_number );

    custX = (double)atof(lonClient);
    custX /= 10000.0;
    // 60.13241
    minuteVal = modf(custX, &degreeVal);
    // deg = 60.0 minute = .13241
    minuteVal *= 100.0;
    // minute = 13.241
    secondsVal = modf(minuteVal, &minuteVal);
    // degreeVal = 60.0 minuteVal = 13.0 secondsVal=.241
    secondsVal *= 100.0;
    minuteVal = (minuteVal + secondsVal/60.0)/60.0;
    custX = degreeVal + minuteVal;
    
    custY = (double)atof(latClient);
    custY /= 10000.0;
    minuteVal = modf(custY, &degreeVal);
    minuteVal *= 100.0;
    secondsVal = modf(minuteVal, &minuteVal);
    secondsVal *= 100.0;
    minuteVal = (minuteVal + secondsVal/60.0)/60.0;
    custY = degreeVal + minuteVal;

    LatLong_to_UTM( custY, custX, &Northing, &Easting );
    printf("CustX = %f CustY = %f Northing = %f Easting = %f\n", custX, custY, Northing, Easting);
    queryZone = GetCustomerZone( 7, Easting, Northing);
    printf("queryZone = %d\n", queryZone);
    
    if ( queryZone == 0 ) // try to find closest centroid
      queryZone = GetCustomerZoneCentroid( 7, Easting, Northing );
    printf("queryZone = %d\n", queryZone);
    
    sprintf(logMsg, "CUSTOMER ZONE = %d ", queryZone);
    
    // Now lookup the taxi stand for this customer
    db_open( ZONE_FILE_ID, DB_READ_ONLY|DB_MANUAL_LOCK );
    zone_rec.fleet = 'H';
    zone_rec.nbr = queryZone;
    if ( db_read_key( ZONE_FILE_ID, &zone_rec, &zn_key1, ISEQUAL ) != SUCCESS ) // zone number query
      {
	printf("zone table query failed H - %d\n", queryZone);
        bzero( &drv, sizeof(DRV_INFO) );
        sprintf( drv.drv_info_id, "%08d", 0 );        
        sprintf( drv.drv_info_tele, "%-18s", "0" );        
        GenStandTaxiReply( sClient, &drv, session );
	db_close( ZONE_FILE_ID );
	sprintf(logMsg, "%s MAIN TAXI STAND: %d", logMsg, 0);
	Generic_logMsg( sClient, logMsg, session );
        return(0);
      }
    if ( ( zone_rec.type == '1' ) || ( zone_rec.stand_nbr == 0 ) ) // already a taxi stand
      queryZone = zone_rec.nbr;
    else
      queryZone = zone_rec.stand_nbr;
    sprintf(logMsg, "%s *MAIN TAXI STAND*: %d", logMsg, queryZone);
    Generic_logMsg( sClient, logMsg, session );    
    db_close( ZONE_FILE_ID );
    
    zrq.d_prcs_id = DISP_DEST;						/* send ipc to Dispatch */
    zrq.u_prcs_id = pid;						/* user's process id */
    zrq.rec_type = ZONE_STAND_HOLDv2;						/* zone request type */
    zrq.fleet = 'H';
    zrq.zone_nbr = queryZone;
    strcpy( zrq.query_id, query_id );

    printf("Initiating msgq query %d \n", zrq.zone_nbr);
    msgsnd(msgkey,(struct msgbuf *)&zrq,sizeof(struct zone_stand_hold), IPC_NOWAIT);	/* send ipc message */

    times_thru = 0;    
    hZoneResp = (HNDL)malloc( sizeof(struct zone_stand_hold) );
    zrp = ( struct zone_stand_hold * )hZoneResp;

    signal(SIGALRM, t_out_hdlr);
    while (times_thru <=2 )
      {
	alarm(30);
	rc = msgrcv(msgkey,(void *)zrp,sizeof(struct zone_stand_hold),(long) pid,0);
        ++times_thru;
	printf("Received msgq reply %c\n", zrp->rec_type);
        if ( zrp->rec_type == ZONE_STAND_HOLDv2 )
          break;
      }
    alarm(0);
    if ( zrp->veh_nbr > 0 )
      {
        bzero( &drv, sizeof(DRV_INFO) );
        sprintf( drv.drv_info_id, "%08d", zrp->veh_nbr );        
        sprintf( drv.drv_info_tele, "%-18s", zrp->veh_tele );
      }
    else
      {
        bzero( &drv, sizeof(DRV_INFO) );
        sprintf( drv.drv_info_id, "%08d", 0 );        
        sprintf( drv.drv_info_tele, "%-18s", "0" );
      }        
    
    GenStandTaxiReply( sClient, &drv, session );
    free( zrp );
    return(0);
}


    

DetailedCall_to_CallParam( char *pTESData, CALL_PARAM *pCall )
{
    bzero( pCall, sizeof( CALL_PARAM ) );

    pCall->Fleet = *(pTESData);
    strncpy( pCall->DueDate, (pTESData + CP_DUE_DATE_OFFSET), CP_DUE_DATE_LEN );
    strncpy( pCall->DueTime, (pTESData + CP_DUE_TIME_OFFSET), CP_DUE_TIME_LEN );
    strncpy( pCall->StreetNbr, (pTESData + CP_STR_NBR_OFFSET), CP_STR_NBR_LEN );
    strncpy( pCall->StreetType, (pTESData + CP_STR_TYPE_OFFSET), CP_STR_TYPE_LEN );
    strncpy( pCall->StreetName, (pTESData + CP_STR_NAME_OFFSET), CP_STR_NAME_LEN );
    strncpy( pCall->PhoneNbr, (pTESData + CP_PHONE_NBR_OFFSET), CP_PHONE_NBR_LEN );
    strtoupper( pCall->StreetName );
    strncpy( pCall->CityName, (pTESData + CP_CITY_NAME_OFFSET), CP_CITY_NAME_LEN );
    strtoupper( pCall->CityName );
    strncpy( pCall->PassengerName, (pTESData + CP_PASS_NAME_OFFSET), CP_PASS_NAME_LEN );
    strtoupper( pCall->PassengerName );
    strncpy( pCall->Apartment, (pTESData + CP_APARTMENT_OFFSET), CP_APARTMENT_LEN );
    strtoupper( pCall->Apartment );
    strncpy( pCall->Comments, (pTESData + CP_COMMENTS_OFFSET), CP_COMMENTS_LEN );
    strtoupper( pCall->Comments );
    strncpy( pCall->VehAttr, (pTESData + CP_VEHATTR_OFFSET), CP_VEHATTR_LEN );
    strtoupper( pCall->VehAttr );
    strncpy( pCall->DrvAttr, (pTESData + CP_DRVATTR_OFFSET), CP_DRVATTR_LEN );
    strtoupper( pCall->DrvAttr );    
}


CallParam_to_CallInfo( CALL_PARAM *src, CALL_INFO *dest )
{
  POST_ADDRESS     *Pickup;
  int              zone, i;
  char             date_buf[ 8 ];
  char             time_buf[ 8 ];
  char             yr_tmp[ 8 ];
  struct tm        *tm_ptr;
  time_t           nowtime;
  int zip;    


  bzero( date_buf, sizeof( date_buf ) );
  bzero( time_buf, sizeof( time_buf ) );

  Pickup = &( dest->pickup.cust.address );
  dest->fleet = src->Fleet;

  if ( !strcmp( src->DueDate, "      " ) )   // Immediate Call
      {
	  get_pkup_date( date_buf );
	  get_pkup_time( time_buf );
      }

  get_date_time_from_text( &dest->pickup.due_date_time, date_buf, time_buf );

  Pickup->street_nbr = atoi( src->StreetNbr );
  strcpy( Pickup->street.name, src->StreetName );
  strcpy( Pickup->street.type, src->StreetType );
  strcpy( Pickup->city_name, src->CityName );
  strcpy( dest->pickup.cust.phone_nbr, src->PhoneNbr );
  strcpy( dest->requested_by, src->PassengerName );

  /** Now we can attempt to zone the address **/
  zip = 0;  
  zone = ac_zone_addr( dest->fleet, &dest->pickup.cust.address, &zip );
  
  // zone = TES_zone_address( pTESCall->fleet, &pTESCall->pickup.cust.address );
  if ( zone > 0 )
    dest->pickup.zone = zone;  
}
	  
int strtoupper( unsigned char *pBuf )
{
    int c;
    
    if ( pBuf == NULL )
	return( -1 );

    while ( *pBuf != '\0' ) {
	c = *pBuf;
	*pBuf = toupper( c );
	++pBuf;
    }
}

	    
writer_add_hist()
{
}

void
t_out_hdlr()
{
  printf("Request timed out...\n");
  exit(0);
}
