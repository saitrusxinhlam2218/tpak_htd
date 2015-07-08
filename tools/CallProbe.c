/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: CallProbe.c,v $
*  @(#)  $Revision: 1.1 $
*  @(#)  $Date: 2002/03/25 17:17:29 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/tools/CallProbe.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: CallProbe.c,v $	$Revision: 1.1 $"

static char rcsid[] = "$Id: CallProbe.c,v 1.1 2002/03/25 17:17:29 jwelch Exp $";

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "taxipak.h"
#include "Object.h"
#include "List_private.h"
#include "List_public.h"
#include "Zone_public.h"
#include "Zone_private.h"
#include "Fleet_public.h"
#include "Fleet_private.h"

#include "mads_types.h"
#include "fleet.h"
#include "mad_ipc.h"
#include "menu.h"
#include "user.h"
#include "calls.h"
#include "except.h"

char *shm_ptr;					/* pointer to allocated shared memory */
struct offsets *offset;

main( int argc, char **argv[] )
{
 int  shmid;	/* identifier of shared memory */
 int size;
 int seg_size;
 int i;
 CALL *pCall;
 char  *ctime(), *stime; 

  seg_size = sizeof(struct offsets) +	FLEET_MAX * sizeof(struct fleets) 
    + CALLS_MAX * sizeof(struct calls) + USER_MAX * sizeof(struct users) 
      + EXCEPT_MAX * sizeof(struct excepts) 
	+ MENU_MAX * sizeof(struct menus)
	  + EXCEPT_DESC_MAX * sizeof(struct excpt_desc)
	    + sizeof(struct exception_count_struct);

  if ( (shmid = shmget(SHMKEY,seg_size,0666)) == -1 )  {
    /* shmget failed */
    perror("Shared memory get");
    exit(-1);
  }  /* end shmget */

  if ( (shm_ptr = shmat(shmid, 0x1000000, SHM_RND)) == (char *)-1 )  {
    /* shmat failed */
    perror("Shared memory attach");
    exit(-1);
  }  /* end if shmat */
 
 offset  = (struct offsets *)shm_ptr;
 pCall = (CALL *)offset->call;
 i = 0;
 for (i = 0; i < CALLS_MAX; i++ )
   {
     pCall = (CALL *)(offset->call + i);
     if ( pCall->call_number > 0 )
       {
	 stime = ctime( &pCall->due_time );
	 printf( "call nbr: %d zone: %d due_time: %.19s Time call: %d Level: %d accept_time: %d\n",
		pCall->call_number,
		pCall->pickup_zone,
		stime, pCall->call_type.time, pCall->curr_level,
		pCall->accept_time);
       }
   }
}
