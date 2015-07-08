/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call_rec_info.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2001/03/12 16:40:42 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/Call/Call_rec_info.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Call_rec_info.c,v 1.3 2001/03/12 16:40:42 taxiadm Exp $";

#include <sys/types.h>		/* temp */
#include <sys/times.h>		/* temp */
#include "mad_error.h"
#include "mads_types.h"
#include "taxipak.h"
#include "Object.h"
#include "Call_public.h"
#include "Call_private.h"

extern short debug_state;

static HNDL
call_rec_value(call_rec_hndl, op, attr, value)
   CALL_REC_HNDL call_rec_hndl;
   int   op;
   int   attr;
   HNDL  value;
{
   CALL_REC * call_rec_ptr;
   long  tmp_time;
   unsigned char *addr;

   call_rec_ptr =  (CALL_REC *) call_rec_hndl;
   switch (attr)
   {
   case CALL_FLEET:
      if (op == PUT)
      {
	 call_rec_ptr->fleet = (char) value;
	 return ((HNDL) SUCCESS);
      } else
	 return ((HNDL) call_rec_ptr->fleet);
      break;
   case CALL_NBR:
      if (op == PUT)
      {
	 call_rec_ptr->nbr = (int) value;
	 return ((HNDL) SUCCESS);
      } else
	 return ((HNDL) call_rec_ptr->nbr);
   case CALL_VEH_DRIV_ATTR:
      /* see taxi_sup - call_rec_check_attr_on */
      /* TRUE or FALSE */
      /* MAY WANT FORMAT CONVERSION */
      if (op != GET)
      {
	 ERROR(' ', "", " Illegal operation on CALL_VEH_DRIV_ATTR");
         if (op == PUT)
             return((HNDL) FAIL);
         else
	    return (HNDL_ERR);
      }
      addr = (unsigned char *) call_rec_ptr->veh_attr;
      if (*addr)
	 return ((HNDL) TRUE);
      else
      {
	 addr = (unsigned char *) call_rec_ptr->drv_attr;
	 if (*addr)
	    return ((HNDL) TRUE);
	 else
	    return ((HNDL) FALSE);
      }

      break;
   default:
      printf("ILLEGAL VALUE FOR call_rec_value %d\n", attr);
      if (op == PUT)
	 return ((HNDL) FAIL);
      else
	 return ((HNDL) NULL);
   }				/* end case */
   return ((HNDL) SUCCESS);
}				/* end call_rec_value */


RET_STATUS
Call_get_rec_info(call_rec_hndl, attr, value_ptr)
   CALL_REC_HNDL call_rec_hndl;
   int   attr;
   HNDL * value_ptr;
{
   if (value_ptr == NULL)
      return(FAIL);
   *value_ptr = call_rec_value(call_rec_hndl, GET, attr, NULL);
   return(SUCCESS);
}                              /* end Call_get_rec_info */

RET_STATUS
Call_set_rec_info(call_rec_hndl, attr, hndl)
   CALL_REC_HNDL call_rec_hndl;
   int   attr;
   HNDL  hndl;
{
   HNDL  status;

   if ((attr >= STATS_START) && (attr <= STATS_END))
   {
      printf("ERROR - attempt to set  call_rec value %d that is a stat \n", attr);
      return (FAIL);		/* cant set a stat */
   }
   if (call_rec_hndl == (CALL_REC_HNDL) NULL)
      return (FAIL);
   status = call_rec_value(call_rec_hndl, PUT, attr, hndl);
   return ((short) status);
}				/* end Call_set_rec_value */

