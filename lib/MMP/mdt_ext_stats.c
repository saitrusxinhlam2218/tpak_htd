/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: mdt_ext_stats.c,v $
*  @(#)  $Revision: 2.0 $
*  @(#)  $Date: 2012/08/28 13:29:51 $
*  @(#)  $Author: taxiprod $
*  @(#)  $Source: /usr/users/jwelch/cvs_g7/taxi-g7/lib/MMP/mdt_ext_stats.c,v $
*
*  Copyright (c) 1992 - Motorola Mobile Data Division, Seattle, WA
*
***********************************************************************/

static char rcsid[] = "$Id: mdt_ext_stats.c,v 2.0 2012/08/28 13:29:51 taxiprod Exp $";

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include "mad_error.h"
#include "taxipak.h"
#include "Object.h"
#include "mdt.h"
#include "msg.h"
#include "itm.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "mdt_stats.h"
#include "mdt_ext_stats.h"

extern long  mdt_use_cnt[LAST_STAT + 1];	/* mdt keys (transactions) usage count */

RET_STATUS
mdt_ext_trans_stats(msg)
   MSG_QUEUE_HDR *msg;
{
   /* FORM message */
   switch (msg->type)
   {				/* Parse message for type */
   case MT_MSG_STATUS:		/* Status Message */
      break;			/* Do next message */

   case MT_FORM_MSG:		/* Form Message */
      switch (msg->p_fields[P1])
      {
      case FT_LOCAL_FORM:
	 switch (msg->p_fields[P2])
	 {
         case LF_INVOICE:
            mdt_use_cnt[INVOICE_REQ_IDX]++;
            break;
             
	 default:
           return(FAIL);
	 }
	 break;

      case FT_AUX_INP:
	 break;

      default:
         return(FAIL);
      }
      break;			/* Do next message */

   case MT_FUNC_MSG:		/* Function Message */
      switch (msg->p_fields[P1])
      {
      case FCT_EMERG:
         return(FAIL);

      case FCT_CAN_MSG:
	 switch (msg->p_fields[P2])
	 {

         case CM_CLUB:
             mdt_use_cnt[TOGGLE_IDX]++;
             break;
	 default:
             return(FAIL);
	 }
	 break;

      case FCT_EXT_INP:
	 switch (msg->p_fields[P2])
	 {

	 default:
            return(FAIL);
	 }
	 break;

      default:
           return(FAIL);
	 break;
      }
      break;			/* Do next message */

   default:
      return(FAIL);
      break;			/* Do next message */
   }
   return(SUCCESS);
}				/* end mdt_ext_trans_stats */


void
mdt_ext_stats_disp(index, key_str)
    short index;
    char * key_str;
{
      switch (index)
      {
      case TOGGLE_IDX:
	 strcpy(key_str, "CLUB TOGGLE       ");
	 break;
      case INVOICE_REQ_IDX:
         strcpy(key_str, "INVOICE REQUEST   "); 
         break;
      }
}           /* end mdt_ext_stats_disp */
