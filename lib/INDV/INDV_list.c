/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: INDV_list.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/07/23 16:57:26 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/INDV/INDV_list.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: INDV_list.c,v 1.2 1998/07/23 16:57:26 taxiadm Exp $";

#include <stdio.h>

#include "taxipak.h"
#include "taxi_defs.h"
#include "taxi_db.h"
#include "mem_mgmt.h"	/* TALLOC() TFREE() */

#include "Indv_db.h"

RET_STATUS
Individual_open_list(ACCOUNT_ID *acct_id, INDV_CUR_HNDL *control)
{
	RET_STATUS rc;
	INDV_LIST_CONTEXT *cur;
	char err_msg[STD_ERR_MSG_SIZE];

	if (SUCCESS != (rc = _Individual_db_open()))
		INDV_LOG(INDV_FILE_NOT_FOUND,
		 "Unable to open INDIVIDUAL Table; (Individual_open_list())");

	/* get memory for list */
	cur = (void *)TMALLOC(INDV_LIST_CONTEXT, 1);
	if (!cur)
	{
        sprintf (err_msg,
         "malloc(%d); Unable to allocate (INDV_LIST_CONTEXT)",
		  sizeof(INDV_LIST_CONTEXT)*1);
        return INDV_LOG (INDV_NO_MEMORY, err_msg);
	}

	/* set acct_id in list */
	cur->acct_id = *acct_id;

	/* set not_read */
	cur->not_read = TRUE;

	*control = (INDV_CUR_HNDL)cur;

	return SUCCESS;
}

RET_STATUS
Individual_read_list(INDV_CUR_HNDL control, INDV_REC_HNDL buffer)
{
	RET_STATUS rc;
	INDV_LIST_CONTEXT *cur = (INDV_LIST_CONTEXT *)control;
	INDIVIDUAL *rec = (INDIVIDUAL *)buffer;

	if (!cur)
		return INDV_LOG(
		 INDV_NULL_ARG, "control handle invalid; (Individual_read_list())");

	if (!rec)
		return INDV_LOG(
		 INDV_NULL_ARG, "buffer handle invalid; (Individual_read_list())");

	if (cur->not_read)
	{
		/* build a key from read_hand->acct_id */
		rec->individual_id.acct_id = cur->acct_id;
		memset(rec->individual_id.name, '\0',sizeof(rec->individual_id.name));

		/* read db first record */
		rc = Individual_read((INDV_REC_HNDL)rec, DB_GREAT, INDV_PRIMARY_KEY);

		cur->not_read = FALSE;
	}
	else	/* just read next record */
		rc = Individual_read((INDV_REC_HNDL)rec, DB_NEXT, INDV_PRIMARY_KEY);

	if (rc == SUCCESS)
	{
		if (!Account_id_compare(&rec->individual_id.acct_id, &cur->acct_id))
			rc = INDV_END_OF_LIST;
	}
	else
	{
		if ((err_value(rc) == EENDFILE) || (err_value(rc) == ENOREC))
			rc = INDV_END_OF_LIST;
		else
			/* log Informix error */
			return INDV_LOG(rc, "DB Read failed; Individual_read_list()");
	}
	return rc;
}

RET_STATUS
Individual_read_exact(INDV_CUR_HNDL control, char *name, INDV_REC_HNDL buffer)
{
	RET_STATUS rc;
	INDV_LIST_CONTEXT *cur = (INDV_LIST_CONTEXT *)control;
	INDIVIDUAL *rec = (INDIVIDUAL *)buffer;

	if (!cur)
		return INDV_LOG(INDV_INVALID_READ_HNDL,
		"control handle invalid; (Individual_read_exact())");

	if (!buffer)
		return INDV_LOG(INDV_INVALID_READ_HNDL,
		"buffer handle invalid; (Individual_read_exact())");

	/* build a key with new name */
	rec->individual_id.acct_id = cur->acct_id;
	strncpy(rec->individual_id.name, name, sizeof(rec->individual_id.name));

	/* read for exact match */
	rc = Individual_read((INDV_REC_HNDL)rec, DB_EQUAL, INDV_PRIMARY_KEY);

	if (rc != SUCCESS)
	{
		if (err_value(rc) == ENOREC)
			return INDV_NOT_FOUND;
		else
			return INDV_LOG(rc,"DB Read EXACT failed; Individual_read_exact()");
	}
	return SUCCESS;
}

RET_STATUS
Individual_close_list(INDV_CUR_HNDL *control)
{
	RET_STATUS rc = FAIL;

	if (control)
	{
		if (*control)
			TFREE(*control);
		*control = NULL;
		rc = SUCCESS;
	}

	return rc;
}

RET_STATUS		/* remove all individuals for an account */
Individual_remove_account(ACCOUNT_ID *id)
{
	RET_STATUS rc = SUCCESS;
	INDV_REC rec;
	
	while (rc == SUCCESS)
	{
		/* build a key for this account_id */
		rec.individual_id.acct_id = *id;
		memset(rec.individual_id.name, '\0',sizeof(rec.individual_id.name));

		/* read db next record */
		rc = Individual_read((INDV_REC_HNDL)&rec, DB_GREAT, INDV_PRIMARY_KEY);
		if (rc == SUCCESS)
		{
			/* are we still on the same account? */

			if (!Account_id_compare(id, &rec.individual_id.acct_id))
				break;

			if (SUCCESS != (rc = Individual_delete((INDV_REC_HNDL)&rec)))	/* delete current record */
				return INDV_LOG(
				 rc, "unable to delete; Individual_remove_account()");
		}
	}

	return SUCCESS;
}
