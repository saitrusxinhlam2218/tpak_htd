/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ACAD_maint.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2003/01/28 16:58:23 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/ACAD/ACAD_maint.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: ACAD_maint.c,v 1.3 2003/01/28 16:58:23 jwelch Exp $";

#include <curses.h>
#include <memory.h>

#include "taxipak.h"
#include "taxi_defs.h"
#include "taxi_db.h"
#include "mem_mgmt.h"		/* TMALLOC() TFREE() */

#include "Account_Address_db.h"

static int object_active_cnt=0;	/* count of active "Objects" */


RET_STATUS
Account_Address_alloc(ACAD_REC_HNDL *hndl)
{
	static int obj_instance_nbr = 0; /* sequential cnt of objects created */

	char err_msg[STD_ERR_MSG_SIZE];


	if (!hndl)	/* test if a valid addr for hndl */
		return ACAD_LOG (ACAD_NULL_ARG, 
		 "NULL Account_Address handle address; Account_Address_alloc()");

	*hndl = (void *)TMALLOC(ACCOUNT_ADDRESS, 1);	/* do alloc */
	if (!*hndl)	/* did malloc work? */
	{
		sprintf (err_msg,
		 "malloc(%d); Unable to allocate object (%s)",
		 sizeof(ACCOUNT_ADDRESS)*1, ACCOUNT_ADDRESS_TYPE_NAME);
		return ACAD_LOG (ACAD_NO_MEMORY, err_msg);
	}
	/* zero zero zero */

	memset((char *)*hndl, '\0', sizeof(ACCOUNT_ADDRESS));

	/* set internal info */
	++object_active_cnt;		/* for internal count */

	return SUCCESS;
}

RET_STATUS
Account_Address_free(ACAD_REC_HNDL *hndl)
{
	RET_STATUS rc;
	char err_msg[STD_ERR_MSG_SIZE];

	/* test for (~potentially)valid pointer */

	if (hndl && *hndl && (object_active_cnt > 0))
	{
		/* de-allocate */

		TFREE(*hndl);

		*hndl = NULL;	/* clear this up */
		object_active_cnt--;

		return SUCCESS;
	}
	strcpy(err_msg, "Account_Address_free(");
	if (!hndl)
	{
		rc = NULL_ARG;
		strcat(err_msg, "Account_Address handle address NULL");
	}
	else if (!*hndl)
	{
		rc = NULL_ARG;
		strcat(err_msg, "Account_Address handle NULL");
	}
	if (object_active_cnt <= 0)
	{
		rc = INVALID_ARG;
		sprintf(
		 &err_msg[strlen(err_msg)], " ((%d)active objects <= none)",
		 object_active_cnt);
	}
	strcat(err_msg, ")");

	return ACAD_LOG (rc, err_msg);
}

PRIVATE_ACAD_FUNC int
_Account_Address_nbr_objects(void)	/* return nbr of "Objects" active(not freed) */
{
	return object_active_cnt;
}

PRIVATE_ACAD_FUNC static RET_STATUS
Account_Address_rec_value(
 ACAD_REC_HNDL hndl,
 int attr,
 HNDL buf,
 int action)
{
	char err_msg[STD_ERR_MSG_SIZE];
	ACCOUNT_ADDRESS *op = (ACCOUNT_ADDRESS *)hndl; /* object pointer */
	char *get_set = (action==GET)?(GET_STR):(PUT_STR);

	if (action != GET && action != PUT)	/* test for valid pointers */
	{
		sprintf (err_msg,
		 "Invalid action(%d) requested of Account_Address_rec_value()", action);
		return ACAD_LOG (ACAD_NULL_ARG, err_msg);
	}
	if (!hndl  || !buf)	/* test for valid pointers */
	{
		sprintf (err_msg,
		 "Null object handle or buffer handle; Account_Address_rec_value(%s)",
		 get_set);
		return ACAD_LOG (ACAD_NULL_ARG, err_msg);
	}

	switch (attr)
	{
	case ACAD_ID:
		if (action == GET)
			*(HNDL *)buf = (HNDL)&op->id;
		else
			memcpy(
			 &op->id, (ACCOUNT_ADDRESS_ID *)buf, sizeof(ACCOUNT_ADDRESS_ID));
		break;
	case ACAD_ADDRESS:
		if (action == GET)
			*(HNDL *)buf = (HNDL)&op->address;
		else
			memcpy(&op->address, (STREET_ADDRESS *)buf, sizeof(op->address));
		break;
	case ACAD_PHONE:
		if (action == GET)
			*(HNDL *)buf = (HNDL)&op->phone;
		else
			memcpy(&op->phone, (PHONE *)buf, sizeof(PHONE));
		break;
	default:
		sprintf (err_msg,
		 "Invalid Attribute(%d); Account_Address_rec_value(%s)", attr, get_set);

		return ACAD_LOG (ACAD_INVALID_ATTR, err_msg);

		break;		/* NOTREACHED */
	}
	return SUCCESS;
}

RET_STATUS
Account_Address_get_rec_info(
 ACAD_REC_HNDL hndl,
 int attribute,
 HNDL buf)
{
	return Account_Address_rec_value(hndl, attribute, buf, GET);
}

RET_STATUS
Account_Address_set_rec_info(
 ACAD_REC_HNDL hndl,
 int attribute,
 HNDL buf)
{
	return Account_Address_rec_value(hndl, attribute, buf, PUT);
}

PRIVATE_ACAD_FUNC RET_STATUS
_Account_Address_db_open(void)
{
	RET_STATUS rc;

	if ( ! DB_FILE_IS_OPEN(ACAD_FILE_ID) )
	{
		rc = db_open(ACAD_FILE_ID, DB_READ_WRITE + DB_MANUAL_LOCK);
		if (rc != SUCCESS)
			return rc;
	}
	return SUCCESS;
}


RET_STATUS
Account_Address_read(		/* read a record from the data base */
 ACAD_REC_HNDL hndl,
 int mode,
 int key_type)		/* index into Account_Address db key array */
{
	RET_STATUS rc;
	char err_msg[STD_ERR_MSG_SIZE];

	if (!hndl)
		return ACAD_LOG (
		 ACAD_NULL_ARG, "Null object handle; Account_Address_read()");

	/* read db */
	db_open(ACAD_FILE_ID, DB_READ_ONLY);
	rc = db_read_key(ACAD_FILE_ID, hndl, Account_Address_keys[key_type], mode);
	if (rc != SUCCESS)	/* test errors */
	{
		if (err_value(rc) == EENDFILE || err_value(rc) == ENOREC)
			return rc;
		else
		{
			sprintf (err_msg,
			"DB read(%s_FILE_ID, , key_type(%d), %d) error; Account_Address_read()",
			 ACCOUNT_ADDRESS_TYPE_NAME, key_type, mode);
			ACAD_LOG(rc, err_msg);
		}
		return rc;
	}

	return SUCCESS;
}

RET_STATUS
Account_Address_release_locks(void)
{
	RET_STATUS rc;
	char err_msg[STD_ERR_MSG_SIZE];

	/* call db to release */

	rc = db_release(ACAD_FILE_ID);
	if (rc != SUCCESS)	/* test for success */
		return ACAD_LOG(
		 rc, "DB release lock failed; Account_Address_release_locks()");

	return SUCCESS;
}

RET_STATUS
Account_Address_add(ACAD_REC_HNDL hndl)	/* record is implied in hndl */
{
	RET_STATUS rc;

	if (!hndl)
		return ACAD_LOG (
		 ACAD_NULL_ARG, "Null object handle; Account_Address_add()");

	/* write to the data base */
	rc = db_write(ACAD_FILE_ID, hndl);
	if (rc != SUCCESS)
		return ACAD_LOG(rc, "DB Add Record failed; Account_Address_add()");

	return SUCCESS;
}

RET_STATUS
Account_Address_update(ACAD_REC_HNDL hndl)  /* update an already existing record */
{
	RET_STATUS rc;
	char err_msg[STD_ERR_MSG_SIZE];

	if (!hndl)
		return ACAD_LOG (
		 ACAD_NULL_ARG, "Null object handle; Account_Address_update()");

	/* CALLER HANDLED : client sequence for an update:
		Account_Address_read - record with lock
		Account_Address_set_rec_info - set items in record
		Account_Address_update
		Account_Address_release_locks
	** CALLER HANDLED */

	/* update record */

	rc = db_update(ACAD_FILE_ID, hndl);
	if (rc != SUCCESS)
		return ACAD_LOG(rc,"DB Update Record failed; Account_Address_update()");

	return SUCCESS;
}

RET_STATUS
Account_Address_delete(ACAD_REC_HNDL hndl)
{
	RET_STATUS rc;
	char err_msg[STD_ERR_MSG_SIZE];

	if (hndl)	/* read to proper record */
	{
		rc = db_read_key(ACAD_FILE_ID,
		 hndl, Account_Address_keys[ACAD_PRIMARY_KEY], DB_EQUAL);
		if (rc != SUCCESS)	/* handle errors */
		{
			if (err_value(rc) != ENOREC)
				return ACAD_LOG(
				 rc, "DB Read Record failed; Account_Address_delete()");
			return rc;
		}
	}

	/* delete current record */

	rc = db_delete(ACAD_FILE_ID, hndl);
	if (rc != SUCCESS)	/* handle errors */
		return ACAD_LOG(
		 rc, "DB Delete Record failed; Account_Address_update()");

	return SUCCESS;
}

