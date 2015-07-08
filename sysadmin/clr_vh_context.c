/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: clr_vh_context.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2005/04/21 07:00:07 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/sysadmin/clr_vh_context.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: clr_vh_context.c,v 1.3 2005/04/21 07:00:07 jwelch Exp $";

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <isam.h>
#include <cursesX.h>
#include <signal.h>

#define	DECLARING
#include "taxipak.h"
#include "Object.h"
#include "path.h"
#include "comm_strdefs.h"
#include "vh_m_strdefs.h"
#include "mad_error.h"
#include "mads_types.h" 
#include "taxi_db.h"
#include "Fleet_db.h"
#include "Vehicle_db.h"
#include "df_maint.h"
#include "vh_maint.h"
#include "literals.h"
#include "language.h"

extern  char req_buf[];

clr_vh_context()
{

  struct cisam_vh *vh_ptr;
  int         done = 0;
  int         mode;
  int         count = 0;
  
  vh_ptr = (struct cisam_vh *) req_buf;
  
  if((db(VEHICLE_FILE_ID,OPEN,&veh_key2,ISINOUT+ISMANULOCK,0)) < 0)
    {   
      perror("Unable to open vehicle file");
      exit (-1);
    }

  mode = ISFIRST;
  bzero(vh_ptr, sizeof(struct cisam_vh));

  while ( !done )
    {
      if ((db(VEHICLE_FILE_ID, READ_KEYS, &veh_key1, mode, 0)) < 0)
	{
	  done = 1;
	}
      else
	{
	  vh_ptr->last_zone_posted = 0;
	  vh_ptr->last_time_posted = 0;
	  vh_ptr->status = 0;
	  db_update(VEHICLE_FILE_ID, vh_ptr);
	  ++count;
	  mode = ISNEXT;
	}
    }
  db_close( VEHICLE_FILE_ID );
}
