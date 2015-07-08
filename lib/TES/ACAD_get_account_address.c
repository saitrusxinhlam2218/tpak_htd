/*
 *                 RCS INFO
 *
 *  @(#)  $RCSfile: ACAD_get_account_address.c,v $
 *  @(#)  $Revision: 1.3 $
 *  @(#)  $Date: 1998/12/21 15:36:51 $
 *  @(#)  $Author: taxiadm $
 *  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/TES/ACAD_get_account_address.c,v $
 *
 *  Copyright (c) 1991 - Motorola Mobile Data Division, Seattle, WA
 *
 */
#include "Account_Address.h"
#include "TES.h"



/**********************************************************************
		      TES_get_account_address()
  Return the address associated with an account id and index from the
  Account Address Object.
/**********************************************************************/

POST_ADDRESS *
TES_get_account_address(ACCOUNT_ID * account_id, int address_index, POST_ADDRESS *address, MINITEL_PHONE *phone)
{
   ACAD_REC_HNDL hndl;
   RET_STATUS rc;
   ACCOUNT_ADDRESS_ID account_address_id;
   STREET_ADDRESS *street_address = NULL;
   PHONE *rec_phone = NULL;

   account_address_id.account_id = *account_id;
   account_address_id.index = address_index;
   db_open(ACAD_FILE_ID, DB_READ_ONLY|DB_MANUAL_LOCK);
   if ( open_table(ACAD_FILE_ID, DB_READ_ONLY | DB_MANUAL_LOCK) == 0 && 
      (rc = Account_Address_alloc(&hndl)) == SUCCESS )
   {
      if ( (rc = Account_Address_set_rec_info(hndl, ACAD_ID, (HNDL) &account_address_id)) == SUCCESS &&
         (rc = Account_Address_read(hndl, DB_EQUAL, ACCOUNT_ADDRESS_KEY)) == SUCCESS &&
         (rc = Account_Address_get_rec_info(hndl, ACAD_ADDRESS, (HNDL) &street_address)) == SUCCESS &&
         (rc = Account_Address_get_rec_info(hndl, ACAD_PHONE, (HNDL) &rec_phone)) == SUCCESS)
      {
         if ( street_address != NULL   &&   address != NULL )
         {
            address->street_nbr = street_address->street_nbr;
            if (street_address->street_nbr_suffix != NULL)
               address->street_nbr_suffix = street_address->street_nbr_suffix;
            else
               address->street_nbr_suffix = ' ';
            strncpy(address->street.name, street_address->street_name, sizeof(address->street.name) - 1);
            strncpy(address->street.type, street_address->street_type, sizeof(address->street.type) - 1);
            strncpy(address->city_name, street_address->city_name, sizeof(address->city_name) - 1);
	    strncpy(address->apt, street_address->apt, sizeof(address->apt) - 1);
         }
         if ( rec_phone != NULL   &&   phone != NULL )
         {
            strcpy(phone->nbr, rec_phone->phone_nbr);
            strcpy(phone->ext, rec_phone->phone_ext);
         }
      }
      Account_Address_free( &hndl );
   }
   return ( (rc == SUCCESS) ? address :   (POST_ADDRESS *) NULL );
}

