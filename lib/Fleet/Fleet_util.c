/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Fleet_util.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2001/03/12 16:40:50 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/Fleet/Fleet_util.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Fleet_util.c,v 1.3 2001/03/12 16:40:50 taxiadm Exp $";


#include <sys/types.h>		/* temp */
#include <sys/times.h>		/* temp */
#include "taxipak.h"
#include "Object.h"
#include "taxi_error.h"
#include "mad_error.h"
#include "mad_ipc.h" 
#include "Fleet_public.h"
#include "fleet.h"

extern struct offsets *offset;		/* shared menory offset for db redundant db */
extern FLEET *fleet[FLEET_MAX];

#ifdef FOO
/*****************************************************/
/* RET_STATUS Fleet_init()                           */
/* OP: initializes the fleet pointers in shmem       */
/* PRE: must call get_offsets() first                */
/* POST:  global pointers initialized                */
/* ERROR:                                            */
/*****************************************************/


RET_STATUS
Fleet_init()
{
    int i;

    /* initialize pointers to fleet structure in shared memory */
    fleet[0] = offset->fleet;
    for (i = 1; i < FLEET_MAX; i++)
	fleet[i] = fleet[0] + i;
}
#endif


/*****************************************************/
/* RET_STATUS Fleet_get_hndl(from, value, hdnl_ptr)  */
/* OP: get a fleet handle from id or fleet_nbr       */
/* PRE: none                                         */
/* POST:  fleet_hndl returned                        */
/* ERROR: no fleet (FAIL)                            */
/*****************************************************/

RET_STATUS
Fleet_get_hndl(from, value, hndl_ptr)
/* fleet hndl is fleet nbr */
   int   from;
   int   value;
   FLEET_HNDL *hndl_ptr;
{
   int   ii;

   *hndl_ptr = (FLEET_HNDL) HNDL_NULL;

   switch (from)
   {
   case BY_ID:			/* WAS find_fl_nbr */
      for (ii = 0; ii < FLEET_MAX; ii++)
      {
         if (fleet[ii] == NULL)
            continue;

	 if (fleet[ii]->fleet_id == value)
         {
	    *hndl_ptr = (FLEET_HNDL) fleet[ii];
            break;
         }
      }
      break;
   case BY_NBR:
      if (value < 0)
	 return (FAIL);
      if (value > FLEET_MAX)
	 return (FAIL);
      *hndl_ptr = (FLEET_HNDL) fleet[value];
   default:
      break;
   }
   if (*hndl_ptr == HNDL_NULL)
      return (FAIL);
   else
      return (SUCCESS);
}				/* end Fleet_get_hndl) */

/********************************************************/
/* short Fleet_get_nbr(fleet_id)                        */
/* OP:  get the fleet nbr from its id                   */
/* PRE: none                                            */
/* POST: fleet nbr returned                             */
/* ERROR: if illegal id retrun -1                       */
/*              need doc                                */
/********************************************************/

short
Fleet_get_nbr(char fleet_id)
{
   short ii;
   for (ii = 0; ii < FLEET_MAX; ii++)
   {
      if (fleet[ii] == NULL)
         continue;

      if (fleet[ii]->fleet_id == fleet_id)
	 return (ii);
   }
   return (-1);
}				/* end Fleet_get_nbr */

/**********************************************/
/* char Fleet_get_id(fleet_nbr)               */
/* OP:  get the fleet id from its nbr         */
/* PRE: none                                  */
/* POST: fleet id returned                    */
/* ERROR: if illegal nbr retrun 0             */
/*              need doc                      */
/**********************************************/

char
Fleet_get_id(short fleet_nbr)
{
   if ((fleet_nbr < 0) || (fleet_nbr >= FLEET_MAX) || (fleet[fleet_nbr] == NULL))
      return (0);

   return (fleet[fleet_nbr]->fleet_id);
}				/* end Fleet_get_nbr */

struct fleets *
fleet_hndl_to_ptr(fleet_hndl)
   FLEET_HNDL fleet_hndl;
 /* MAGIC */
{
   return ((struct fleets *) fleet_hndl);
}				/* end fleet_hndl_to_ptr */
