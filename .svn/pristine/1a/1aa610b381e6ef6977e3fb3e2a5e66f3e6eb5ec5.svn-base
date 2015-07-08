/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: INDV_get_info.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/07/23 16:58:00 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/TES/INDV_get_info.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: INDV_get_info.c,v 1.2 1998/07/23 16:58:00 taxiadm Exp $";

#include "TES.h"
#include "Individual.h"
#include "Indv_db.h"

/**********************************************************************
		       TES_get_indv_info()
  Given an account name and subacct return the first name of subacct and other info
/**********************************************************************/

RET_STATUS
TES_get_indv_info( ACCOUNT_ID *account, CALL_INFO *call_info )
{
   INDV_REC_HNDL indv_rec;
   INDIVIDUAL_ID *indv_entry;
   ACCOUNT_ID    *account_rec;
   char		*subacct;
   char 	can_charge;
   char		*name_buf;

   if ( open_table(INDV_FILE_ID, DB_READ_ONLY | DB_MANUAL_LOCK) == 0 && 
      Individual_alloc(&indv_rec) == SUCCESS )
   {
      Individual_set_rec_info(indv_rec, INDV_ACCOUNT_ID, (HNDL) account); 
      Individual_set_rec_info(indv_rec, INDV_SUBACCOUNT, (HNDL) call_info->sub_acct);
      if ( Individual_read(indv_rec, DB_EQUAL, INDV_SUBACCT_KEY) == SUCCESS)
      {
				Individual_get_rec_info(indv_rec, INDV_NAME, (HNDL) &name_buf);
				Individual_get_rec_info(indv_rec, INDV_CAN_CHARGE, (HNDL) &can_charge);
				strncpy(call_info->requested_by, name_buf, sizeof(call_info->requested_by));
				call_info->can_charge = can_charge;
      }
      else
	set_error("Unable to find subaccount requested");
   }
   Individual_free(&indv_rec);
   return ( 1 );
}

