/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: TES_cancel_call.c,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 2004/02/02 19:19:06 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/TES/TES_cancel_call.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: TES_cancel_call.c,v 1.4 2004/02/02 19:19:06 jwelch Exp $";


#include <fcntl.h>

#include "taxipak.h"
#include "taxi_defs.h"
#include "mads_types.h"
#include "Object.h"
#include "Call_public.h"
#include "Call_private.h"
#include "Call_db.h"
#include "language.h"
#include "ui_ipc.h"
#include "writer.h"
#include "taxi_db.h"
#include "except.h"
#include "TES.h"
#include "path.h"
#include "Call_hist_db.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Fleet_db.h"

RET_STATUS
TES_cancel_call(int call_nbr)
{
  static int fifo_fd = -1;
  RET_STATUS rs;
  struct excpt_rec except_fifo;
  CALL_REC call;
  FLEET_REC fl_rec;

  
  /*
   * Check if call has already been cancelled. Note that a cancelled
   * call can be revived, so look at the last entry in call history
   */
  if (get_last_exception_type_from_callh(call_nbr) == CALL_CANCEL)
  {
    return(err_encode_special(TES_OBJ, APP_ERR_GP, TES_ERR_CALL_ALREADY_CANCELLED));
  }

  /* We close and then reopen because for some
     unkown reason it only works that way */
  db_close(CALL_FILE_ID);
  rs = db_open(CALL_FILE_ID, DB_READ_WRITE | DB_MANUAL_LOCK);
  if (rs != SUCCESS)
    return(rs);

  call.nbr = call_nbr;
  
  rs = db_read_key(CALL_FILE_ID, &call, &cl_key1, DB_EQUAL);
  
  if (rs != SUCCESS)
    return(rs);
  
  db_close(FLEET_FILE_ID);
  rs = db_open(FLEET_FILE_ID, DB_READ_ONLY | DB_MANUAL_LOCK);
  if (rs != SUCCESS)
    return(rs);
    
  fl_rec.id = call.fleet;
  rs = db_read_key(FLEET_FILE_ID, &fl_rec, &fl_key1, DB_EQUAL);

  /* Not using writer.h defined CANCELD becasue in client_dispatch for
     an unknown reason it uses the English, not French. */
  add_call_history_record(&call, CANCELD, CALL_CANCEL);
  
  /* '3' means automatic cancel. Only in that case do we have
     to lock the record. */
  if (fl_rec.cancel_action == '3')
  {
    rs = db_read(CALL_FILE_ID, &call, DB_CURRENT | DB_LOCK);
    if (rs != SUCCESS)
      printf("Didn't read cancel call record successfully, iserrno=%d", iserrno);

    /* Not using writer.h defined CANCELD becasue in client_dispatch for
       an unknown reason it uses the English, not French.
       For some reason catalogs may have been closed. */
    strcpy(call.status, CANCELD);
    call.pri_status = 64;
    call.grouped_with = 0;
    
    rs = db_update(CALL_FILE_ID, &call);
    if (rs != SUCCESS)
      printf("Didn't update cancel call record successfully, iserrno=%d", iserrno);
    db_release(CALL_FILE_ID);
  }
  
  /*
   * Build an exception record to send to dispatch, so the call can be
   * cancelled by a dispatcher
   */
  bzero(&except_fifo, sizeof(except_fifo));
  except_fifo.u_prcs_id = getpid();
  except_fifo.rec_type = EXCEPT_REQ;
  except_fifo.except_ptr = NULL;
  except_fifo.except_nbr = 0;
  except_fifo.resolve_code = R_CANCEL;
  except_fifo.call_nbr = call.nbr;
  except_fifo.veh_nbr = 0;
  except_fifo.zone_nbr = 0;
  
  if (fifo_fd == -1)
  {
    /* open FIFO to write requests to dispatch process */
    if((fifo_fd = open(USER_PIPE, O_RDWR | O_NDELAY)) == -1)
    {
      return(err_encode_special(TES_OBJ, APP_ERR_GP, TES_ERR_CANT_TELL_DISPATCH));
    }
  }

  if (write(fifo_fd, &except_fifo, sizeof(struct excpt_rec)) == -1)
  {
    return(err_encode_special(TES_OBJ, APP_ERR_GP, TES_ERR_CANT_TELL_DISPATCH));
  }

  return(SUCCESS);
}


RET_STATUS
TES_redispatch_call( int call_nbr )
{
  static int fifo_fd = -1;
  RET_STATUS rs;
  struct excpt_rec except_fifo;
  CALL_REC call;
  CALL_HIST_REC callh_rec;

  /* We close and then reopen because for some
     unkown reason it only works that way */
  db_close(CALL_FILE_ID);
  rs = db_open(CALL_FILE_ID, DB_READ_WRITE | DB_MANUAL_LOCK);
  if (rs != SUCCESS)
    return(rs);

  call.nbr = call_nbr;
  
  rs = db_read_key(CALL_FILE_ID, &call, &cl_key1, DB_EQUAL);
  
  if (rs != SUCCESS)
    return(rs);

  if ( !strcmp( call.status, CANCELD ) ) /* trip is cancelled so don't redispatch */
    return(rs);
  
  db_close(FLEET_FILE_ID);
  rs = db_open(FLEET_FILE_ID, DB_READ_ONLY | DB_MANUAL_LOCK);
  if (rs != SUCCESS)
    return(rs);

  strcpy( call.status, UNASSGND );
  rs = db_update( CALL_FILE_ID, &call );

  if (rs != SUCCESS)
    printf("Didn't update cancel call record successfully, iserrno=%d", iserrno);
  db_release(CALL_FILE_ID);  

  bzero(&except_fifo, sizeof(except_fifo));
  except_fifo.u_prcs_id = getpid();
  except_fifo.rec_type = EXCEPT_REQ;
  except_fifo.except_ptr = NULL;
  except_fifo.except_nbr = 0;
  except_fifo.resolve_code = R_DISPATCH_ILINK;
  except_fifo.call_nbr = call.nbr;
  except_fifo.veh_nbr = 0;
  except_fifo.zone_nbr = 0;
  
  if (fifo_fd == -1)
  {
    /* open FIFO to write requests to dispatch process */
    if((fifo_fd = open(USER_PIPE, O_RDWR | O_NDELAY)) == -1)
    {
      return(err_encode_special(TES_OBJ, APP_ERR_GP, TES_ERR_CANT_TELL_DISPATCH));
    }
  }

  if (write(fifo_fd, &except_fifo, sizeof(struct excpt_rec)) == -1)
  {
    return(err_encode_special(TES_OBJ, APP_ERR_GP, TES_ERR_CANT_TELL_DISPATCH));
  }

  db_open( CALLH_FILE_ID, DB_READ_WRITE | DB_MANUAL_LOCK );
  
  bzero( &callh_rec, sizeof(CALL_HIST_REC) );

  callh_rec.call_nbr = call.nbr;
  callh_rec.fleet = call.fleet;
  callh_rec.exception_nbr = 0;
  callh_rec.event_type = REGULAR_EVENT;
  strcpy(callh_rec.event_desc, "TIMEOUT");
  callh_rec.veh_nbr = 0;
  callh_rec.drv_id = 0;
  callh_rec.user_id = 0;
  writer_add_hist( &callh_rec );

  db_close( CALLH_FILE_ID );
  
  return(SUCCESS);
}


/*
  Given a call number, return the value of its last
  exception. Return -1 if not found.
*/
get_last_exception_type_from_callh(int call_nbr)
{
  CALL_HIST_REC ch;
  RET_STATUS rs;
  int   exception_type;
  

  ch.call_nbr = call_nbr;
  ch.status_dt_tm = 0;
  
  rs = open_table(CALLH_FILE_ID, DB_READ_ONLY | DB_MANUAL_LOCK);
  if (rs != SUCCESS)
    return(-1);

  rs = db_read_key(CALLH_FILE_ID, &ch, &ch_key3, ISGTEQ);
  
  /*
   * First check if it can find any record with wanted call number
   */
  if (rs != SUCCESS || ch.call_nbr != call_nbr)
    return (-1);
  
  /*
   * Step through all history in time order to find the last one for given
   * call.
   */
  do
  {
    exception_type = ch.exception_nbr;
    rs = db_read(CALLH_FILE_ID, &ch, ISNEXT);
    /*
     * If call number received from last db call is not the given call
     * number, then the previous db call returned the last call history
     * record for given call number.
     */
    if (rs == SUCCESS)
    {
      if (ch.call_nbr != call_nbr)
        return (exception_type);
    }
  }
  while (rs == SUCCESS);
  
  /* Must have hit end of file */
  return (exception_type);
}

#ifdef _TES_CANCEL_TEST_

#define DECLARING
#include "writer_strdefs.h"
#include "ui_strdefs.h"

char *offset = NULL;

main()
{
  RET_STATUS rs;
  char s[10];

  while (1)
  {
    printf("Call #? ");
    gets(s);
    rs = TES_cancel_call(atoi(s));
    printf("%d\n", rs);
  }
}

#endif
