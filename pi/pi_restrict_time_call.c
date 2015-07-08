/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: pi_restrict_time_call.c,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 2001/10/03 17:52:37 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/pi/pi_restrict_time_call.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: pi_restrict_time_call.c,v $	$Revision: 1.4 $"

static char rcsid[] = "$Id: pi_restrict_time_call.c,v 1.4 2001/10/03 17:52:37 taxiadm Exp $";

#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include <time.h>

#include "taxipak.h"
#include "mem_mgmt.h"
#include "Object.h"
#include "mads_types.h"
#include "ui_restrict_time_calls.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "ui_ipc.h"
#include "pi.h"
#include "pi_struc.h"

extern struct fleets *fleet[];						/* pointer to fleet array 		*/
extern int pid;								/* process id of this interface process	*/
extern int msgkey;

pi_tcr_check_time_call_restriction( pPreBookInfo, fl_nbr )
     PRE_BOOK_INFO *pPreBookInfo;
     int           fl_nbr;
{
  int i = 0;
  time_t  book_dt_tm;
  int          num_time_calls = 0;
  HNDL    hTCResp = HNDL_NULL;
  TC_RESP_REC  *tc_rp;
  TIMES        *time_slots;
  struct tc_rqst tc_rq;
  int  rc, times_thru;
  ZONE_REC     zone;
  short        max_time_calls;
  
/*
 *      check how many time calls currently set for this time
 */
  
  book_dt_tm = get_tm_from_date_time( pPreBookInfo->date, pPreBookInfo->book_list[0].time );
  
  tc_rq.d_prcs_id = DISP_DEST;
  tc_rq.u_prcs_id = pid;
  tc_rq.rec_type = TC_REQ;
  tc_rq.fleet = pPreBookInfo->fleet;
  tc_rq.zone_nbr = pPreBookInfo->zone_nbr;
  tc_rq.due_date_time = book_dt_tm;
  
  msgsnd( msgkey, (struct msgbuf *)&tc_rq, sizeof( struct tc_rqst ), IPC_NOWAIT );
  hTCResp = TMALLOC( TC_RESP_REC, 1 );
  tc_rp = ( TC_RESP_REC * )hTCResp;
  
  times_thru = 0;
  while ( times_thru <= 2 )
    {
      rc = msgrcv( msgkey, (void *)tc_rp, sizeof(TC_RESP_REC), (long)pid, 0 );
      if ( rc == -1 )
	{
	  if ( errno != EINTR )
	    {
	      TFREE( hTCResp );
	      return(0);
	    }
	  if (++times_thru == 2) 
	    {
	      TFREE( hTCResp );
	      return(0);
	    }
	}
      else 
	if (tc_rp->rec_type == TC_REQ)
	  break;
	else
	  {
	    TFREE( hTCResp );
	    return(0);
	  }
    }  

  bzero( &zone, sizeof(ZONE_REC) );
  zone.fleet = pPreBookInfo->fleet;
  zone.nbr = pPreBookInfo->zone_nbr;
  if ( db_read_key(ZONE_FILE_ID, &zone, &zn_key1, ISEQUAL) != SUCCESS )
    max_time_calls = 0;
  else
    max_time_calls = zone.max_time_calls;

  time_slots = (TIMES *)tc_rp->time_slots;
  for ( i = 0; i <= 6; i++ )
    {
      pPreBookInfo->book_list[i].time[0] = (time_slots+i)->time_string[0];
      pPreBookInfo->book_list[i].time[1] = (time_slots+i)->time_string[1];
      pPreBookInfo->book_list[i].time[2] = (time_slots+i)->time_string[3];
      pPreBookInfo->book_list[i].time[3] = (time_slots+i)->time_string[4];
      if ( max_time_calls == 0 )
	pPreBookInfo->book_list[i].n_free = 99;
      else
	pPreBookInfo->book_list[i].n_free = max_time_calls - (time_slots+i)->count;
    }

  TFREE( hTCResp );

}

