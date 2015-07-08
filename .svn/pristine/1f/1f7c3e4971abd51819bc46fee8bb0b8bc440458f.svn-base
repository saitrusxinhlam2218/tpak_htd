/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ac_trace.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/07/23 17:52:39 $
*  @(#)  $Author: root $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/auto_call/ac_trace.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: ac_trace.c,v 1.2 1998/07/23 17:52:39 root Exp $";

#include <stdio.h>
#include <sys/types.h>
#include "AutoCall_private.h"
#include "TES.h"

#define TES_TRACE  "/usr/taxi/run/traces/autocall_trace"
extern char port;

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
  fprintf( TESTrace, "%.8s %c --> AUTOBOOK ", &stime[11], port );

  switch ( TESType )
    {
    case TES_STARTED:
      fprintf( TESTrace, " %-*s", 32, "TES_STARTED" );
      break;

    case TES_TERMINATE:
      fprintf( TESTrace, " %-*s", 32, "TES_TERMINATE" );
      break;

    case TES_CANCEL_CALL:
      fprintf( TESTrace, " %-*s", 32, "CRIMP_CANCEL_CALL" );
      fprintf( TESTrace, " %-*s", 32, pTESMsg->TESdata );
      break;

    case TES_REQ_ACCT_CALL:
      fprintf( TESTrace, " %-*s", 32, "CRIMP_ACCT_CALL" );
      fprintf( TESTrace, " %-*s", 32, pTESMsg->TESdata );
      break;


    case TES_CLIENT_DISCONNECT:
      fprintf( TESTrace, " %-*s", 32, "CRIMP_CLIENT_DISCONNECT" );
      break;

    case TES_VALID_ACCT:
      fprintf( TESTrace, " %-*s", 32, "CRIMP_VALID_ACCT" );
      break;

    default:
      fprintf( TESTrace, " %-*s", 32, "CRIMP_TO_AC_UNKNOWN" );
      fprintf( TESTrace, " %-*s", 32, pTESMsg->TESdata );
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
  
  tmtime = time((time_t *) 0);
  stime = ctime(&tmtime);
  fprintf( TESTrace, "%.8s %c <-- CRIMP    ", &stime[11], port );

  switch ( TESType )  
    {
    case TES_CALL_NBR:
      fprintf( TESTrace, " %-*s", 32, "AB_CALL_NBR" );
      fprintf( TESTrace, " %-*s", 32,  pTESMsg->TESdata );
      break;

    case TES_STATUS:
      fprintf( TESTrace, " %-*s", 32, "AB_STATUS" );
      fprintf( TESTrace, " %-*s", 32, pTESMsg->TESdata );
      break;

    case TES_OK:
      fprintf( TESTrace, " %-*s", 32, "TES_OK" );
      break;

    case TES_KO:
      fprintf( TESTrace, " %-*s", 32, "TES_KO" );
      fprintf( TESTrace, " %-*s", 32, pTESMsg->TESdata );
      break;


    default:
      fprintf( TESTrace, " %-*s", 32, "TES_FROM_CRIMP_UNKNOWN" );
      fprintf( TESTrace, " %-*s", 32, pTESMsg->TESdata );
      break;
    }
  fprintf( TESTrace, "\n" );

  fclose( TESTrace );
}
