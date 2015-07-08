static char sz__FILE__[]="@(#)taxi.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  taxi.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:36:34		            *			
* @(#)  Last delta: 12/2/94 at 12:15:57						    *	
* @(#)  SCCS File: /taxi/sccs/s.taxi.c							    *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS					    *
*                                                                           *
*****************************************************************************/

/* -----------------------------------------------------------------
    Copyright 1994 - TaxiCom Copenhagen, Denmark
    

    System   : TaxiPak enhancement library 
    Filename : taxi.c
    Purpose  : Taxi utilities for re-use 

    Modification history

    Author      Date      Revision     Description
    ---------   -------   -----------  -------------------------
    KAG         940521    0.0         	Prototype 

	funcs:

	int	DispatchUp( void )
  	int InitCommToTP()
  	int signal_to_monitor()

   ----------------------------------------------------------------- */ 
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/times.h>
#include <fcntl.h>
#include <errno.h>

#include "../include/path.h"
#include "enhance_com.h"
#include "err_inc.h"
#include "taxi_inc.h"

int	DispatchUp( void )
{
  FILE *fd;
  int  rc;
  pid_t	Dispatch_pid; 

  if ((fd = fopen(DISPATCH_PID_FILE, "r")) == NULL)
    return(-1);

  fscanf(fd, "%d", &Dispatch_pid);
  fclose(fd);

  /* Check if dispatch PID is still up. kill(pid, 0) returns
     a -1 and an errno of ESRCH if the PID does not exist */
  rc = kill(Dispatch_pid, 0);
  if (rc == -1 && errno == ESRCH)
    return(bFALSE);
  return(bTRUE);
} 

 /**************************************************************************
  * Name: int InitCommToTP()                                          *
  * Type: Function                                                         *
  *                                                                        * 
  * Purpose: ( Monitor notification).				   *
  *                                                                        *
  * Returnvalue: 1 -> Done.                                                *
  *              0 -> Not Done.                                            *
  **************************************************************************/

  int InitCommToTP( int bUsingMonitor )
  {
    int openresult;
    int retry = 5,
        succeeded = 0;

    if ( bUsingMonitor )
    {
      openresult = OpenMsgQ();  /* Open messagequeue to monitor */
      if (openresult == 1)
        return bFALSE;
    }

    while (( retry > 0 ) && ( !succeeded ))
    if ( !DispatchUp() )
        {
          ENH_ERROR("Not able to connect to TaxiPak - will try again in 15 secs.",
								NO_ERROR );
          retry--;
          sleep( 15 );
        }
      else 
        succeeded = bTRUE;
        
    if (( retry == 0 ) && ( !succeeded ))
		ENH_ERROR( "Not able to connect to TaxiPak within limit, shutdown", TAXI_ERROR );

    return bTRUE;
  } /* end init_pi */


