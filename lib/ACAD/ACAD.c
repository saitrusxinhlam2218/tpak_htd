/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ACAD.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/07/23 16:56:42 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/ACAD/ACAD.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: ACAD.c,v 1.2 1998/07/23 16:56:42 taxiadm Exp $";

#include <stdio.h>

#include "taxipak.h"
#include "mem_mgmt.h"		/* TMALLOC() TFREE() */
#include "taxi_db.h"
#include "Account_Address_db.h"


PRIVATE_ACAD_FUNC RET_STATUS
Account_Address_format_address(ACCOUNT_ADDRESS *acad, char *str)
{
	/* take an Account_Address and format it for display */
	/* see docs/LAYOUT/G7_misc 7.4 ALT Address Screen pg. 133 */

  	/* Prevent problem of EOS for suffix prematurely ending address
	   string */
	if (acad->address.street_nbr_suffix == EOS)
	  acad->address.street_nbr_suffix = ' ';

	sprintf(str, "%4d%c  %-*.*s %-*.*s %-*.*s ",
	 acad->address.street_nbr, acad->address.street_nbr_suffix,
	 sizeof(acad->address.street_type), sizeof(acad->address.street_type),
	 acad->address.street_type,
	 sizeof(acad->address.street_name), sizeof(acad->address.street_name),
	 acad->address.street_name,
	 sizeof(acad->address.city_name), sizeof(acad->address.city_name),
	 acad->address.city_name);

	return SUCCESS;	/* follow the convention */
}

RET_STATUS
Account_Address_get_address_choice(
 ACCOUNT_ID	*clients_account_id,
 STREET_ADDRESS	*clients_address,
 PHONE		*clients_phone)
{
	int mode;
	int count;
	RET_STATUS rc;
	SCRN_CHOICE_LIST *list;
	ACCOUNT_ADDRESSES account_addresses;
	ACAD_REC *choice_rec = NULL;

	if (!clients_account_id || !clients_address || !clients_phone)
		return ACAD_LOG(ACAD_INVALID_ARG, "Null client information");

	mode = DB_GTEQ;	/* read equal */
	account_addresses.info[0].id.account_id = *clients_account_id;
	account_addresses.info[0].id.index = 0;

	for (count=0; count < MAX_ADDRESSES_PER_ACCOUNT; count++)
	{
		rc = Account_Address_read(
	 	(ACAD_REC_HNDL)&account_addresses.info[count], mode, ACCOUNT_ADDRESS_KEY);
		if (rc == SUCCESS)
		{
			/* if account_id's don't match */
			if (!Account_id_compare(
			 &account_addresses.info[count].id.account_id, clients_account_id))
				break;
		}
		else
			break;

		mode = DB_NEXT;
	} 

	account_addresses.max_index = count;

	if (rc != SUCCESS && err_value(rc) != EENDFILE)
		return rc;

	if (SUCCESS != (rc =
	 Scrn_choice_alloc(FORMATTED_ADDRESS_SIZE, &list)))
		return rc;

	account_addresses.next_index = 0;
	list->data = (HNDL)&account_addresses;

	rc = Scrn_choice_window(list, Account_Address_next_page, Account_Address_prev_page, (HNDL *)&choice_rec);
	if (rc == SUCCESS)
	{
		if (!choice_rec)
			rc = ACAD_NO_CHOICE;
		else
		{
			if (clients_address)
				*clients_address = choice_rec->address;
			if (clients_phone)
				*clients_phone = choice_rec->phone;

		}
	}
	Scrn_choice_free(&list);

	return rc;
}

PRIVATE_ACAD_FUNC RET_STATUS
Account_Address_next_page( SCRN_CHOICE_LIST *list)	/* call-back */
{
	int i, starting_next;
	ACCOUNT_ADDRESSES *acad = (ACCOUNT_ADDRESSES *)list->data;

	if (!list)
		return
		 ACAD_LOG(
		  ACAD_NULL_ARG, "Choice_list null; Account_Address_next_page()");
	if (!list->data)
		return ACAD_LOG(
		 ACAD_INVALID_ARG,
		  "Choice_list->data null; Account_Address_next_page()");

	starting_next = acad->next_index;	/* save for rc adjustment */

	/* adjust next_index to point to next data even if it is same as last */

	if (acad->next_index >= acad->max_index)
		acad->next_index =
		 acad->max_index -
		  ((acad->next_index%list->max_choices)?
		  (acad->next_index%list->max_choices):list->max_choices);

	for (i=0;
	 i < list->max_choices && acad->next_index < acad->max_index;
	  i++, acad->next_index++)
	{
		list->choices[i].data = (HNDL)&acad->info[acad->next_index];

		Account_Address_format_address((ACCOUNT_ADDRESS *)
		 &acad->info[acad->next_index], list->choices[i].string);
	}

	list->nbr_choices =i;

	if (acad->next_index == starting_next)
		return ACAD_END_OF_LIST;

	return SUCCESS;
}

PRIVATE_ACAD_FUNC RET_STATUS
Account_Address_prev_page( SCRN_CHOICE_LIST *list)	/* call back */
{
	ACCOUNT_ADDRESSES *acad = (ACCOUNT_ADDRESSES *)list->data;

	if (!list)
		return
		 ACAD_LOG(
		  ACAD_NULL_ARG, "Choice_list null; Account_Address_next_page()");
	if (!list->data)
		return ACAD_LOG(
		 ACAD_INVALID_ARG,
		 "Choice_list->data null; Account_Address_next_page()");

	/* at top and bottom of data */
	if ((acad->next_index - list->max_choices) <= 0)
		return ACAD_END_OF_LIST;

	/* if next_index is pointing to the end of the list */
	if (acad->next_index >= acad->max_index)
		acad->next_index =
		 (acad->max_index - list->max_choices) -
		  ((acad->next_index%list->max_choices)?
		  (acad->next_index%list->max_choices):list->max_choices);
	else
		acad->next_index -= list->max_choices*2;

	if (acad->next_index < 0)
		acad->next_index = 0;

	return Account_Address_next_page(list);
}
