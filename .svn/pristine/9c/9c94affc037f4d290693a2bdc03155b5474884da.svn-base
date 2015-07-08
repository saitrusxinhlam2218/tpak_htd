/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: INDV_choice.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2003/01/28 16:58:23 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/INDV/INDV_choice.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: INDV_choice.c,v 1.3 2003/01/28 16:58:23 jwelch Exp $";


#include <stdio.h>
#include "taxipak.h"
#include "taxi_defs.h"
#include "taxi_db.h"
#include "mem_mgmt.h"	/* TMALLOC() TFREE() */

#include "Indv_db.h"


RET_STATUS
Individual_choice_format(INDV_REC * rec, char * string)
{
	sprintf (string, "%*.*s %*.*s",
         INDV_NAME_LEN, INDV_NAME_LEN, rec->individual_id.name,
         INDV_SUBACCOUNT_LEN, INDV_SUBACCOUNT_LEN, rec->subacct_nbr);

        return(SUCCESS);

}

RET_STATUS
Individual_get_choice(ACCOUNT_ID *acct_id, INDV_REC_HNDL rec)
{
	int i;
	SCRN_CHOICE_LIST *list;
	HNDL window_hndl;
	INDIVIDUAL individuals[INDV_MAX_INDIVIDUAL];
	INDIVIDUAL *choice_rec = NULL;
	INDV_LIST_CONTEXT list_info;
	RET_STATUS rc;

	if (!acct_id || !rec)
		return INDV_LOG(INDV_NULL_ARG,
		 "NULL Account_id or Individual handle; Individual_get_choice()");

	/* alloc space for N rows of data */
	rc = Scrn_choice_alloc( INDV_NAME_LEN+INDV_SUBACCOUNT_LEN+3, &list );

	for (i=0; i < list->max_choices; i++)
		Individual_alloc( &list->choices[i].data );

	/* fill 1st page of individuals */
	list_info.acct_id = *acct_id;	/* copy acct_id to acct_id */
	list_info.prev_page_rc = SUCCESS;
	list_info.not_read = TRUE;

	list->data = (HNDL)&list_info;

	/* display window get choice */
	rc = Scrn_choice_window(list,
	 Individual_get_next_page,
	 Individual_get_prev_page, (HNDL *)&choice_rec);

	/* make copy of record for return to client */

	if ((rc == SUCCESS) && (choice_rec))
		*(INDIVIDUAL *)rec = *choice_rec;	/* copy choice into rec */

	/* clean-up */
	for (i = 0; i < list->max_choices; i++)
		Individual_free(&list->choices[i].data);
	Scrn_choice_free( &list );

	return rc;
}

BOOLEAN
_indv_acct_id_cmp(ACCOUNT_ID *ac_id1, ACCOUNT_ID *ac_id2)
{
	if (ac_id1->fleet != ac_id2->fleet || strcmp(ac_id1->nbr, ac_id2->nbr))
		return FALSE;
	return TRUE;
}

RET_STATUS
Individual_get_next_page(SCRN_CHOICE_LIST *list)	/* call-back */
{
	int i;
	int nbr_choices;
	INDV_LIST_CONTEXT *list_info;
	INDV_REC *rec, tmp_rec;
	RET_STATUS rc;

	if (!list)
		return INDV_LOG(
		 INDV_NULL_ARG, "NULL list pointer; (Individual_get_next_page())");

	list_info = (INDV_LIST_CONTEXT *)list->data;

	nbr_choices = 0;
	for (i=0; i < list->max_choices; i++)
	{
		tmp_rec = *(INDV_REC *)list->choices[i].data;

		if (list_info->not_read)	/* position record pointer */
		{
			/* set-up key */

			/* copy acct_id */
			tmp_rec.individual_id.acct_id = list_info->acct_id;
			memset(
			 tmp_rec.individual_id.name,
			 '\0', sizeof(tmp_rec.individual_id.name));

			/* position record pointer */

			rc =Individual_read(
			 (INDV_REC_HNDL)&tmp_rec, DB_GTEQ, INDV_PRIMARY_KEY);
		}
		else if (i == 0)	/* position record pointer */
		{
			/* copy data in last record to 1st record for key info */

			tmp_rec = *(INDV_REC *)list->choices[list->nbr_choices-1].data;
			rc = Individual_read(
			 (INDV_REC_HNDL)&tmp_rec, DB_GREAT,INDV_PRIMARY_KEY);
			 
		}
		else
			rc = Individual_read(
			 (INDV_REC_HNDL)&tmp_rec, DB_NEXT, INDV_PRIMARY_KEY);

		if (rc != SUCCESS ||
		 !_indv_acct_id_cmp(
		  &tmp_rec.individual_id.acct_id, &list_info->acct_id))
			break;

		list_info->not_read = FALSE;

		rec = (INDV_REC *)list->choices[i].data;	/* rec pointer */
		*rec = tmp_rec;		/* copy just read data into list buffer */
		nbr_choices++;

		/* copy name into screen data buffer */
	 	Individual_choice_format(rec, list->choices[i].string);	
	}
	if (nbr_choices == 0)
	{
		list_info->prev_page_rc = SUCCESS;
		return INDV_END_OF_LIST;
	}

	list->nbr_choices = nbr_choices;

	if (rc == SUCCESS || err_value(rc) == EENDFILE)
	{
		if (list_info->prev_page_rc != SUCCESS)
		{
			rc = list_info->prev_page_rc;
			list_info->prev_page_rc = SUCCESS;
		}
		return rc;
	}

	list_info->prev_page_rc = SUCCESS;
	return rc;
}


RET_STATUS
Individual_get_prev_page(SCRN_CHOICE_LIST *list)	/* call-back */
{
	int nbr_choices;
	int i, offset;
	INDV_LIST_CONTEXT *list_info;
	INDV_REC *rec, tmp_rec;
	RET_STATUS rc;

	if (!list)
		return INDV_LOG(
		 INDV_NULL_ARG, "NULL list pointer; (Individual_get_prev_page())");

	list_info = (INDV_LIST_CONTEXT *)list->data;
	list_info->prev_page_rc = SUCCESS;

	if (list_info->not_read)	/* this call must be call for first page */
	{
		list_info->prev_page_rc = INDV_END_OF_LIST;	/* report this later  */
		return Individual_get_next_page(list);
	}

	nbr_choices = 0;

	/* position record pointer */

	tmp_rec = *(INDV_REC *)(list->choices[0].data);	/* set key field(s) */
	rc = Individual_read((INDV_REC_HNDL)&tmp_rec, DB_GTEQ, INDV_PRIMARY_KEY);

	/* fill record array from max down */
	/* clean-up later if not enough records to fill array completely */

	for (i = list->max_choices-1; i >= 0 && rc == SUCCESS; i--)
	{
		rc = Individual_read((INDV_REC_HNDL)&tmp_rec,DB_PREV, INDV_PRIMARY_KEY);
		if (rc != SUCCESS ||
		 !_indv_acct_id_cmp(&tmp_rec.individual_id.acct_id, &list_info->acct_id))
			break;

		rec = (INDV_REC *)list->choices[i].data;	/* rec pointer */
		*rec = tmp_rec;		/* copy just read data into list buffer */
		nbr_choices++;

		/* copy name found to screen data buffer */

	 	Individual_choice_format(rec, list->choices[i].string);	
	}

	if (nbr_choices != list->max_choices)
	{
		if (nbr_choices == 0)	/* at start of data or there is no data */
		{
			list_info->prev_page_rc = INDV_END_OF_LIST;	/* report this later */
			list_info->not_read = TRUE;	/* point to 1st record */
			return Individual_get_next_page(list);
		}

		/* shift records to lower range of array */
		/* where rec at list->max_choices is moved to rec at nbr_choices, etc */
		for (i = 0; i < nbr_choices; i++)
		{
			offset = i + (list->max_choices - nbr_choices);
			rec = (INDV_REC *)list->choices[i].data;

			*rec = *(INDV_REC *)list->choices[offset].data;

			/* copy name found to screen data buffer */

	 		Individual_choice_format(rec, list->choices[offset].string);	
		}
		
		/* position rec pointer */
		tmp_rec = *(INDV_REC *)list->choices[nbr_choices-1].data;
		rc = Individual_read(
		 (INDV_REC_HNDL)&tmp_rec, DB_EQUAL, INDV_PRIMARY_KEY);

		/* top off array with records after original "1st" record */
		for (i = nbr_choices; i < list->max_choices && rc == SUCCESS; i++)
		{
			rc = Individual_read(
			 (INDV_REC_HNDL)&tmp_rec, DB_NEXT, INDV_PRIMARY_KEY);
			if (rc != SUCCESS ||
			 !_indv_acct_id_cmp(
			  &tmp_rec.individual_id.acct_id, &list_info->acct_id))
				break;

			nbr_choices++;
			rec = (INDV_REC *)list->choices[i].data;	/* rec pointer */
			*rec = tmp_rec;		/* copy just read data into list buffer */

			/* copy name found to screen data buffer */

	 		Individual_choice_format(rec, list->choices[i].string);	
		}
	}

	list->nbr_choices = nbr_choices;
	list_info->prev_page_rc = SUCCESS;	/* don't report this later */

	return SUCCESS;
}
