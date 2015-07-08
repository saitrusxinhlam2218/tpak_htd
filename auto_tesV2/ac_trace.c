/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ac_trace.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2002/03/25 17:48:26 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/auto_tes/ac_trace.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: ac_trace.c,v $	$Revision: 1.3 $"

static char rcsid[] = "$Id: ac_trace.c,v 1.3 2002/03/25 17:48:26 jwelch Exp $";

#include <stdio.h>
#include <sys/types.h>
#include "Auto_TES.h"
#include "TES.h"

#define TES_TRACE  "/usr/taxi/run/traces/autocall_trace"

void
Generic_logMsg( int sClient, char *logMsg, int session )
{
  FILE   *TESTrace;
  char     *ctime(), *stime;
  time_t     tmtime;
  char     Tbuf[BUFSIZ];

  if ((TESTrace = fopen(TES_TRACE, "a")) == NULL)
    {
      perror("Unable to open AutoCall trace file");
      return;
    }
  Tbuf[0] = '\0';
  
  tmtime = time((long *) 0);
  stime = ctime(&tmtime);
  fprintf( TESTrace, "%.8s %05d --> INFOMSG  ", &stime[11], session );

  fprintf( TESTrace, " %s", logMsg );

  fprintf( TESTrace, "\n" );
  
  fclose( TESTrace );
}

void
Client_to_AutoCall( sClient,  pTESMsg, TESType, session )
     int      sClient;
     TES_MSG  *pTESMsg;
     int      TESType;
     int      session;
{
  FILE   *TESTrace;
  char     *ctime(), *stime;
  time_t     tmtime;
  char     Tbuf[BUFSIZ];

  if ((TESTrace = fopen(TES_TRACE, "a")) == NULL)
    {
      perror("Unable to open AutoCall trace file");
      return;
    }
  Tbuf[0] = '\0';
  
  tmtime = time((long *) 0);
  stime = ctime(&tmtime);
  fprintf( TESTrace, "%.8s %05d --> AUTOCALL ", &stime[11], session );

  switch ( TESType )
    {
    case TES_STARTED:
      fprintf( TESTrace, " %-*s", 32, "TES_STARTED" );
      fprintf( TESTrace, " %03d", TESType );      
      break;

    case TES_TERMINATE:
      fprintf( TESTrace, " %-*s", 32, "TES_TERMINATE" );
      fprintf( TESTrace, " %03d", TESType );
      break;

    case TES_CANCEL_CALL:
      fprintf( TESTrace, " %-*s", 32, "TES_CANCEL_CALL" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;

    case TES_REQ_ACCT_CALL:
      fprintf( TESTrace, " %-*s", 32, "TES_REQ_ACCT_CALL" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;

    case TES_G7_MONITOR_CALL:
      fprintf( TESTrace, " %-*s", 32, "TES_G7_MONITOR_CALL" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;

    case TES_G7_CONFIRM:
      fprintf( TESTrace, " %-*s", 32, "TES_G7_CONFIRM" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;

    case TES_G7_DISPATCH_DETAILED_CALL:
      fprintf( TESTrace, " %-*s", 32, "TES_G7_DISPATCH_DETAILED_CALL" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;

    case TES_G7_RQST_ETA:
      fprintf( TESTrace, " %-*s", 32, "TES_G7_RQST_ETA" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;

    case TES_G7_RQST_ACCT_ETA:
      fprintf( TESTrace, " %-*s", 32, "TES_G7_RQST_ACCT_ETA" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;

    case TES_G7_RQST_GPUB_ETA:
      fprintf( TESTrace, " %-*s", 32, "TES_G7_RQST_GPUB_ETA" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;
      
    case TES_CLIENT_DISCONNECT:
      fprintf( TESTrace, " %-*s", 32, "TES_CLIENT_DISCONNECT" );
      fprintf( TESTrace, " %03d", TESType );
      break;

    case TES_RQST_ADDR:
      fprintf( TESTrace, " %-*s", 32, "TES_RQST_ADDR" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;

    case TES_REQ_DETAILED_CALL:
	fprintf( TESTrace, " %-*s", 32, "TES_REQ_DETAILED_CALL" );
	fprintf( TESTrace, " %03d%-s", TESType, pTESMsg->TESdata );
	break;	

    case TES_REQ_CALL_DETAILS:
	fprintf( TESTrace, " %-*s", 32, "TES_REQ_CALL_DETAILS" );
	fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;

    case TES_MONITOR_CALL:
	fprintf( TESTrace, " %-*s", 32, "TES_MONITOR_CALL" );
	fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;

    case TES_REQ_STAND_TAXI:
	fprintf( TESTrace, " %-*s", 32, "TES_STAND_TAXI" );
	fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;      

    case TES_REQ_STAND_TAXIv2:
      fprintf( TESTrace, " %-*s", 32, "TES_STAND_TAXIv2" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;
      
    default:
      fprintf( TESTrace, " %-*s", 32, "TES_TO_AC_UNKNOWN" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;
    }

  fprintf( TESTrace, "\n" );

  fclose( TESTrace );
}

void
AutoCall_to_Client( sClient,  pTESMsg, TESType, session )
     int      sClient;
     TES_MSG  *pTESMsg;
     int      TESType;
     int      session;
{
  FILE   *TESTrace;
  char     *ctime(), *stime;
  time_t     tmtime;
  char     Tbuf[BUFSIZ];

  if ((TESTrace = fopen(TES_TRACE, "a")) == NULL)
    {
      perror("Unable to open AutoCall trace file");
      return;
    }
  Tbuf[0] = '\0';
  
  tmtime = time((long *) 0);
  stime = ctime(&tmtime);
  fprintf( TESTrace, "%.8s %05d <-- AUTOCALL ", &stime[11], session );

  switch ( TESType )  
    {
    case TES_CALL_NBR:
      fprintf( TESTrace, " %-*s", 32, "TES_CALL_NBR" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32,  pTESMsg->TESdata );
      break;

    case TES_DRV_INFO:
      fprintf( TESTrace, " %-*s", 32, "TES_DRV_INFO" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32,  pTESMsg->TESdata );
      break;      

    case TES_OK:
      fprintf( TESTrace, " %-*s", 32, "TES_OK" );
      fprintf( TESTrace, " %03d", TESType );
      break;

    case TES_KO:
      fprintf( TESTrace, " %-*s", 32, "TES_KO" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;

    case TES_G7_CALL_INFO:
      fprintf( TESTrace, " %-*s", 32, "TES_G7_CALL_INFO" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;

    case TES_G7_ETA:
      fprintf( TESTrace, " %-*s", 32, "TES_G7_ETA" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;

    case TES_G7_NTAXI:
      fprintf( TESTrace, " %-*s", 32, "TES_G7_NTAXI" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;

    case TES_G7_CUST_USE:
      fprintf( TESTrace, " %-*s", 32, "TES_G7_CUST_USE" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;

    case TES_ADDR_REPLY:
      fprintf( TESTrace, " %-*s", 32, "TES_ADDR_REPLY" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;

    case TES_CALL_DETAILS:
	fprintf( TESTrace, " %-*s", 32, "TES_CALL_DETAILS" );
	fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
	break;	
      
    default:
      fprintf( TESTrace, " %-*s", 32, "TES_FROM_AC_UNKNOWN" );
      fprintf( TESTrace, " %03d%-*s", TESType, 32, pTESMsg->TESdata );
      break;
    }
  fprintf( TESTrace, "\n" );

  fclose( TESTrace );
}

