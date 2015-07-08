/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ac_generic_replies.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2002/03/25 16:13:50 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/auto_call/ac_generic_replies.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: ac_generic_replies.c,v 1.3 2002/03/25 16:13:50 jwelch Exp $";

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>
#include "AutoCall_private.h"
#include "TPakWeb_User.h"
#include "TPakWeb_UserAddress.h"
#include "TES.h"
#include "Minitel.h"

extern char port;

GenOKReply( sClient, session )
     int   sClient;
     int   session;
{
  TES_MSG      TESMsg;
    
  sprintf( TESMsg.TEStype, "%3d", TES_OK );
  TESMsg.TESdata[0] = '\0';

  SendAutoCall( sClient, &TESMsg );
  AutoCall_to_Client( sClient, &TESMsg, TES_OK, session );
}

GenKOReply( sClient, ErrorCode, session )
     int   sClient;
     int   ErrorCode;
     int   session;
{
  TES_MSG     TESMsg;

  sprintf( TESMsg.TEStype, "%3d", TES_KO );
  sprintf( TESMsg.TESdata, "%03d", ErrorCode );

  SendAutoCall( sClient, &TESMsg );
  AutoCall_to_Client( sClient, &TESMsg, TES_KO, session );

}

GenOKAcctReply( sClient, session )
     int      sClient;
     int      session;
{
  TES_MSG     TESMsg;

  sprintf( TESMsg.TEStype, "%3d", TES_STATUS );
  sprintf( TESMsg.TESdata, "%3d", TES_ACCT_OK );
  SendAutoCall( sClient, &TESMsg );
  AutoCall_to_Client( sClient, &TESMsg, TES_STATUS, session );
}

GenKOAcctReply( sClient, session )
     int      sClient;
     int      session;
{
  TES_MSG     TESMsg;

  sprintf( TESMsg.TEStype, "%3d", TES_STATUS );
  sprintf( TESMsg.TESdata, "%3d", TES_ACCT_KO );
  SendAutoCall( sClient, &TESMsg );
  AutoCall_to_Client( sClient, &TESMsg, TES_STATUS, session );
}

GenCallCancelReply( sClient, session )
     int      sClient;
     int      session;
{
  TES_MSG     TESMsg;

  sprintf( TESMsg.TEStype, "%3d", TES_STATUS );
  sprintf( TESMsg.TESdata, "%3d", TES_CALL_CANX );
  SendAutoCall( sClient, &TESMsg );
  AutoCall_to_Client( sClient, &TESMsg, TES_STATUS, session );
}

GenNoCallCancelReply( sClient, session )
     int      sClient;
     int      session;
{
  TES_MSG     TESMsg;

  sprintf( TESMsg.TEStype, "%3d", TES_STATUS );
  sprintf( TESMsg.TESdata, "%3d", TES_CALL_NOCANX );
  SendAutoCall( sClient, &TESMsg );
  AutoCall_to_Client( sClient, &TESMsg, TES_STATUS, session );
}

GenKOAddrReply( sClient, session, reason )
     int     sClient;
     int     session;
     int     reason;
{
  TES_MSG    TESMsg;

  sprintf( TESMsg.TEStype, "%3d", reason );
  SendAutoCall( sClient, &TESMsg );  
  AutoCall_to_Client( sClient, &TESMsg, reason, session );
}

GenOKAccountInfoReply( sClient, session, pPostAddr )
  int sClient;
  int session;
  POST_ADDRESS *pPostAddr;
{
  
  TES_MSG TESMsg;

  sprintf( TESMsg.TEStype, "%3d", TES_GET_ACCT_INFO );
  sprintf( TESMsg.TESdata, "%-20.20s%6d%-20.20s%-5.5s",
	   pPostAddr->street.name,
	   pPostAddr->street_nbr,
	   pPostAddr->city_name,
	   pPostAddr->apt );
  SendAutoCall( sClient, &TESMsg );
  AutoCall_to_Client( sClient, &TESMsg, TES_GET_ACCT_INFO, session );
}
  
GenOKAddrReply( sClient, session, pWebAddr )
     int     sClient;
     int     session;
     WEBADDR *pWebAddr;
{
  TES_MSG    TESMsg;

  sprintf( TESMsg.TEStype, "%3d", TES_GET_ADDR );
  sprintf( TESMsg.TESdata, "%08d0101%-2.2s%-20.20s%-3.3s%-2.2s%-3.3s%06d%-5.5s%-30.30s%-30.30s%-2.2s%-2.2s",
	  pWebAddr->webuser_nbr,
	  pWebAddr->str_pre_dir,
	  pWebAddr->str_name,
	  pWebAddr->str_type,
	  pWebAddr->str_post_dir,
	  pWebAddr->str_city,
	  pWebAddr->str_nbr,
	  pWebAddr->str_apartment,
	  pWebAddr->str_comment,
	  pWebAddr->str_ct_comment,
	  pWebAddr->str_pre_dir,
	  pWebAddr->str_post_dir);

  SendAutoCall( sClient, &TESMsg );
  AutoCall_to_Client( sClient, &TESMsg, TES_GET_ADDR, session );
}

GenProfileReply( sClient, pWebUser, session )
     int     sClient;
     WEBUSER *pWebUser;
     int     session;
{
  TES_MSG    TESMsg;

  sprintf( TESMsg.TEStype, "%3d", TES_GET_PROFILE );  
  sprintf( TESMsg.TESdata, "%07d%-17.17s%-13.13s%-21.21s%-17.17s%-33.33s%-33.33s",
	  pWebUser->nbr,
	  pWebUser->user_name,
	  pWebUser->passwd,
	  pWebUser->fullname,
	  pWebUser->tele,
	  pWebUser->email,
	  pWebUser->service );


  SendAutoCall( sClient, &TESMsg );  
  AutoCall_to_Client( sClient, &TESMsg, TES_GET_PROFILE, session );
}

GenCallNbrReply( sClient, CallNbr, session )
     int      sClient;
     int      CallNbr;
     int      session;
{
  TES_MSG     TESMsg;

  sprintf( TESMsg.TEStype, "%3d", TES_CALL_NBR );
  sprintf( TESMsg.TESdata, "%08d", CallNbr );

  SendAutoCall( sClient, &TESMsg );
  AutoCall_to_Client( sClient, &TESMsg, TES_CALL_NBR, session );
  
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

  /** increment length to account for port number field **/
  ++msg_length;
  sprintf( MsgAutoCall.MsgSize, "%02x", msg_length );
  MsgAutoCall.MsgSize[ AC_MSG_SIZE_LEN ] = '\0';
  MsgAutoCall.MsgPort = port;
  memcpy( MsgAutoCall.MsgTES, pTESMsg->TEStype, TES_TYPE_LEN );
  memcpy( &MsgAutoCall.MsgTES[ TES_TYPE_LEN ], pTESMsg->TESdata, msg_length - 1 );

  rc = send( sClient, (char * ) &MsgAutoCall, msg_length + 2, 0 );
}
