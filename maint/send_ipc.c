/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: send_ipc.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1997/02/10 21:16:38 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/maint/send_ipc.c,v $
*
*  Copyright (c) 1996 - MobileSoft Consulting, Inc. Dublin, OH
*
***********************************************************************/

#include <stdio.h>
#include <decimal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include "mads_types.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "df_maint.h"

struct df_maint df_maint_ipc;

send_ipc( nargs )
     int nargs;
{
  int     db_nbr;
  int     qid, key = MSGKEY;

  if ( nargs != 1 )
    return( -1 );

  popint( &db_nbr );

  df_maint_ipc.d_prcs_id = DISP_DEST;
  df_maint_ipc.u_prcs_id = getpid();
  df_maint_ipc.rec_type = DFM_REQ;
  df_maint_ipc.which_file = db_nbr;
  if((qid = msgget(key,0666 | IPC_CREAT)) == -1)
    {
      return( -1 );
    }
  if((msgsnd(qid,&df_maint_ipc,sizeof(df_maint_ipc), IPC_NOWAIT)) == -1)
    {
      return( -1 );
    }
  return ( 1 );
}
