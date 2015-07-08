/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: INDV_maint.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/07/23 16:57:28 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/INDV/INDV_maint.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: INDV_maint.c,v 1.2 1998/07/23 16:57:28 taxiadm Exp $";

#include <stdio.h>
#include <memory.h>

#include "taxipak.h"
#include "taxi_defs.h"
#include "taxi_db.h"
#include "mem_mgmt.h"		/* TMALLOC() TFREE() */

#include "Indv_db.h"

extern char req_buf[];
static int object_active_cnt=0;	/* count of active "Objects" */


RET_STATUS
Individual_alloc(INDV_REC_HNDL *hndl)
{
	static int obj_instance_nbr = 0; /* sequential cnt of objects created */

	char err_msg[STD_ERR_MSG_SIZE];


	if (!hndl)	/* test if a valid addr for hndl */
		return INDV_LOG (INDV_NULL_ARG, 
		 "NULL Individual handle address; Individual_alloc()");

	*hndl = (void *)TMALLOC(INDIVIDUAL, 1);	/* do alloc */
	if (!*hndl)	/* did malloc work? */
	{
		sprintf (err_msg,
		 "malloc(%d); Unable to allocate object (%s)",
		 sizeof(INDIVIDUAL)*1, INDIVIDUAL_TYPE_NAME);
		return INDV_LOG (INDV_NO_MEMORY, err_msg);
	}
	/* zero zero zero */

	memset((char *)*hndl, '\0', sizeof(INDIVIDUAL));

	/* set internal info */

	++object_active_cnt;		/* for internal count */

	return SUCCESS;
}

RET_STATUS
Individual_free(INDV_REC_HNDL *hndl)
{
	char err_msg[STD_ERR_MSG_SIZE];

	/* ~test for valid pointer */

	if (hndl && *hndl && (object_active_cnt > 0))
	{
		/* de-allocate */

		TFREE(*hndl);

		*hndl = NULL;	/* clear this up */
		object_active_cnt--;

		return SUCCESS;
	}
	strcpy(err_msg, "Individual_free(");
	if (!hndl)
		strcat(err_msg, "Individual handle address NULL");
	else if (!*hndl)
		strcat(err_msg, "Individual handle NULL");
	if (object_active_cnt <= 0)
		sprintf(
		 &err_msg[strlen(err_msg)], " ((%d)active objects <= none)",
		 object_active_cnt);
	strcat(err_msg, ")");

	return INDV_LOG (INDV_NULL_ARG, err_msg);
}

PRIVATE_INDV_FUNC static int
_Individual_nbr_objects(void)	/* return nbr of "Objects" active(not freed) */
{
	return object_active_cnt;
}

PRIVATE_INDV_FUNC RET_STATUS
_Individual_db_open(void)
{
	RET_STATUS rc = SUCCESS;

	if ( ! DB_FILE_IS_OPEN(INDV_FILE_ID) )
		rc = db_open(INDV_FILE_ID, DB_READ_WRITE + DB_MANUAL_LOCK);

	return rc;
}


RET_STATUS
Individual_read(		/* read a record from the data base */
 INDV_REC_HNDL hndl,
 int mode,
 int key_type)		/* index into Individual db key array */
{
	RET_STATUS rc;
	char err_msg[STD_ERR_MSG_SIZE];

	if (!hndl)
		return INDV_LOG (INDV_NULL_ARG,"Null object handle; Individual_read()");

	/* read db */
	rc = db_read_key(INDV_FILE_ID, hndl, Individual_keys[key_type], mode);
	if (rc != SUCCESS)	/* test errors */
	{
		if (err_value(rc) == EENDFILE || err_value(rc) == ENOREC)
			return rc;
		else
		{
			sprintf (err_msg,
			"DB read(%s _FILE_ID, , key_type[%d], %d); Individual_read()",
			 INDIVIDUAL_TYPE_NAME, key_type, mode);
			INDV_LOG(rc, err_msg);
		}
		return rc;
	}

	return SUCCESS;
}

RET_STATUS
Individual_release_locks(void)
{
	RET_STATUS rc;
	char err_msg[STD_ERR_MSG_SIZE];

	/* call db to release */

	rc = db_release(INDV_FILE_ID);
	if (rc != SUCCESS)	/* test for success */
		return INDV_LOG(
		 rc, "DB release lock failed; Individual_release_locks()");

	return SUCCESS;
}

RET_STATUS
Individual_add(INDV_REC_HNDL hndl)	/* record is implied in hndl */
{
	RET_STATUS rc;

	if (!hndl)
		return INDV_LOG (INDV_NULL_ARG, "Null object handle; Individual_add()");

	/* write to the data base */
	rc = db_write(INDV_FILE_ID, hndl);
	if (rc != SUCCESS)
		return INDV_LOG(rc, "DB Add Record failed; Individual_add()");

	return SUCCESS;
}

RET_STATUS
Individual_update(INDV_REC_HNDL hndl)  /* update an already existing record */
{
	RET_STATUS rc;
	char err_msg[STD_ERR_MSG_SIZE];

	if (!hndl)
		return INDV_LOG (
		 INDV_NULL_ARG, "Null object handle; Individual_update()");

	/* CALLER HANDLED : client sequence for an update:
		Individual_read - record with lock
		Individual_set_rec_info - set items in record
		Individual_update
		Individual_release_locks
	** CALLER HANDLED */

	/* update record */

	rc = db_update(INDV_FILE_ID, hndl);
	if (rc != SUCCESS)
		return INDV_LOG(rc,"DB Update Record failed; Individual_update()");

	return SUCCESS;
}

RET_STATUS
Individual_delete(INDV_REC_HNDL hndl)
{
	RET_STATUS rc;
	char err_msg[STD_ERR_MSG_SIZE];

	if (hndl)	
	{
		rc = db_read_key(INDV_FILE_ID,
		  hndl, Individual_keys[INDV_PRIMARY_KEY], DB_EQUAL);
		if (rc != SUCCESS)	
		{
			if (err_value(rc) != ENOREC && err_value(rc) != EENDFILE)
				return INDV_LOG(
				 rc, "DB delete Record failed; Individual_delete()");
			return rc;
		}
	}

	/* delete current record */

	rc = db_delete(INDV_FILE_ID, hndl);
	if (rc != SUCCESS)	/* handle errors */
		return INDV_LOG(
		 rc, "DB Delete Record failed; Individual_update()");

	return SUCCESS;
}

PRIVATE_INDV_FUNC static RET_STATUS
Individual_rec_value(
 INDV_REC_HNDL hndl,
 int attr,
 HNDL buf,
 int action)
{
	char err_msg[STD_ERR_MSG_SIZE];
	INDV_REC *op = (INDV_REC *)hndl; /* object pointer */
	char *get_set = (action==GET)?(GET_STR):(PUT_STR);

	if (action != GET && action != PUT)	/* test for valid pointers */
	{
		sprintf (err_msg,
		 "Invalid action(%d) requested of Individual_rec_value()", action);
		return INDV_LOG (INDV_NULL_ARG, err_msg);
	}
	if (!hndl  || !buf)	/* test for valid pointers */
	{
		sprintf (err_msg,
		 "Null object handle or buffer handle; Individual_rec_value(%s)",
		 get_set);
		return INDV_LOG (INDV_NULL_ARG, err_msg);
	}

	switch (attr)
	{
	case INDV_ID:
		if (action == GET)
			*(HNDL *)buf = (HNDL)&op->individual_id;
		else
			op->individual_id = *(INDIVIDUAL_ID *)buf;
		break;
	case INDV_ACCOUNT_ID:
		if (action == GET)
			*(HNDL *)buf = (HNDL)&op->individual_id.acct_id;
		else
			op->individual_id.acct_id = *(ACCOUNT_ID *)buf;
		break;
	case INDV_NAME:
		if (action == GET)
			*(HNDL *)buf = (HNDL)op->individual_id.name;
		else
			memcpy((char *)
			op->individual_id.name,(char *)buf, sizeof(op->individual_id.name));
		break;
	case INDV_SUBACCOUNT:
		if (action == GET)
			*(HNDL *)buf = (HNDL)op->subacct_nbr;
		else
			memcpy(op->subacct_nbr, (char *)buf, sizeof(op->subacct_nbr));
		break;
	case INDV_CAN_CHARGE:
		if (action == GET)
			*(char *)buf = op->can_charge;
		else
			op->can_charge = (char)buf;
		break;
	default:
		sprintf (err_msg,
		 "Invalid Attribute(%d); Individual_rec_value(%s)", attr, get_set);

		return INDV_LOG (INDV_INVALID_ATTR, err_msg);

		break;		/* NOTREACHED */
	}
	return SUCCESS;
}

RET_STATUS
Individual_get_rec_info(
 INDV_REC_HNDL hndl,
 int attribute,
 HNDL buf)
{
	return Individual_rec_value(hndl, attribute, buf, GET);
}

RET_STATUS
Individual_set_rec_info(
 INDV_REC_HNDL hndl,
 int attribute,
 HNDL buf)
{
	return Individual_rec_value(hndl, attribute, buf, PUT);
}

/***TEST_INDIVIDUAL**********************************************************/
/***TEST_INDIVIDUAL**********************************************************/
/***TEST_INDIVIDUAL**********************************************************/

#ifdef TEST_INDIVIDUAL

#include "language.h"

#define _INDV_TEST_HNDLS 200		/* make arrays for testing */

char exe_name[100];	/* name of this executable */
char alpha[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz ";

BOOLEAN
Account_id_compare(ACCOUNT_ID *ac_id1, ACCOUNT_ID *ac_id2)
{
    if (!ac_id1 || !ac_id2)
		return FALSE;

	if ((ac_id1->fleet == ac_id2->fleet) &&
	 (!strcmp(ac_id1->nbr, ac_id2->nbr)))
	  return TRUE;

  return FALSE;
}

void
Scrn_print_error(char *msg)
{
	char err_msg[STD_ERR_MSG_SIZE];

	sprintf (err_msg, "Scrn_print_error(\"%s\")", msg);
	INDV_LOG(0, err_msg);
}

RET_STATUS
Scrn_choice_alloc(int max_string_len, SCRN_CHOICE_LIST **scl)
{
	*scl = TMALLOC(SCRN_CHOICE_LIST, 1);
	if (!*scl)
		INDV_LOG_FLAG(NO_MEMORY,
		"Unable to alloc (SCRN_CHOICE_LIST); Scrn_choice_alloc()",
		FORCE_LOG, FORCE_ABORT);

	(*scl)->max_choices = SCRN_CW_MAX_ROWS-1;
}
RET_STATUS
Scrn_choice_free(SCRN_CHOICE_LIST **scl)
{
	if (*scl)
		TFREE(*scl);
	*scl = NULL;
}
RET_STATUS
Scrn_choice_window(
 SCRN_CHOICE_LIST *scl,
 RET_STATUS ((*next)()), RET_STATUS ((*prev)()), HNDL *hndl)
{
	int i;
	RET_STATUS rc = SUCCESS;
	char c[256];
	INDV_LIST_CONTEXT *choice_data = (INDV_LIST_CONTEXT *)(scl->data);
	INDV_REC *rec;

	printf ("Scrn_choice_window() for account (fleet \'%c\', nbr \"%s\").\n",
	 choice_data->acct_id.fleet, choice_data->acct_id.nbr);

	rc = next(scl);	/* do setup */

	*c = '\0';
	while(*c != 'q')
	{
		printf ("(%d) possible choices (%d)/screen.\n",
		scl->nbr_choices, scl->max_choices);
		for (i = 0;
		(rc ==SUCCESS || rc ==INDV_END_OF_LIST) && i < scl->nbr_choices; i++)
		{
			rec = (INDV_REC *)scl->choices[i].data;
			printf (
			"Scrn_choice_window() : name[%d] = \"%s\" for acct_id(%c,%s).\n",
			i, scl->choices[i].string,
			rec->individual_id.acct_id.fleet, rec->individual_id.acct_id.nbr);
		}

		*c = '\0';
		printf("\nEnter \'n\' next, \'p\' previous, \'q\' quit or \'e\' exit test; npq\n");
		scanf("%s", c);

		printf ("Entered \'%c\'.\n", *c);

		switch (*c)
		{
		case 'n':
			printf ("case \'n\': Entered \'%c\'.\n", *c);
			rc = next(scl);
			break;
		case 'p':
			printf ("case \'p\': Entered \'%c\'.\n", *c);
			rc = prev(scl);
			break;
		case 'e':
			printf ("case \'e\': Entered \'%c\' exit test.\n", *c);
			return FAIL;
			break;
		case 'q':
			printf ("case \'q\': Entered \'%c\' to next Account.\n", *c);
			return rc;
			break;
		default:
			printf ("default: Entered \'%c\'.\n", *c);
			break;
		}
		if (rc != SUCCESS)
			INDV_LOG(rc, "next() || prev() rc");
	}
	return rc;
}


BOOLEAN
 _INDV_add_update(INDV_REC_HNDL individual)
{
    INDV_REC *indv = (INDV_REC *)individual;
    char err_msg[STD_ERR_MSG_SIZE];
    RET_STATUS rc;
    char msg[100];
    INDV_REC indv_tmp;
    int key_type = INDV_PRIMARY_KEY;

    if ((rc = Individual_add((INDV_REC_HNDL)indv)) != SUCCESS)
    {
        strcpy(msg, "add");
        if (EDUPL == err_value(rc)) /* duplicate record */
        {
            strcat(msg,"/update");
            memcpy(&indv_tmp, indv, sizeof(INDV_REC));
            rc = Individual_read(
			 (INDV_REC_HNDL)&indv_tmp, DB_EQUAL, key_type);

            rc = Individual_update((INDV_REC_HNDL)indv);
        }
        if (rc != SUCCESS)
        {
            sprintf (err_msg,
            "Individual_%s() failed; INDV_add_update().", msg);
            INDV_LOG(rc, err_msg);

            return FALSE;
        }
    }
    return TRUE;
}

void
_INDV_fill_rec(INDV_REC *indv)
{
    int i;
    static int entry_cnt = -1;

    entry_cnt++;

    indv->individual_id.acct_id.fleet = alpha[entry_cnt%(sizeof(alpha)-1)];
    sprintf(
	 indv->individual_id.acct_id.nbr, "nbr(%d)", (entry_cnt%(sizeof(alpha)-1)));
    sprintf(indv->individual_id.name,
	 "name(%d,%c)", entry_cnt, indv->individual_id.acct_id.fleet);
    sprintf(indv->subacct_nbr, "s%d", entry_cnt%10);
    indv->can_charge = (entry_cnt%5)?YES:NO;
}

void
_INDV_dump_rec(INDV_REC_HNDL individual)
{
    INDV_REC *indv = (INDV_REC *)individual;

    printf("\n");
    printf(
	"%s : individual_id.acct_id.fleet = \'%c\'.\n",
	exe_name, indv->individual_id.acct_id.fleet);
    printf(
	"%s : individual_id.acct_id.nbr = \"%s\".\n",
	exe_name, indv->individual_id.acct_id.nbr);
    printf(
     "%s : individual_id.name = \"%s\".\n", exe_name, indv->individual_id.name);
	printf("%s : subacct_nbr = \"%s\".\n", exe_name, indv->subacct_nbr);
	printf("%s : can_charge = \'%c\'.\n", exe_name, indv->can_charge);
}


int main (int argc, char **argv)
{
	int i, added=0, deleted=0, found;
	RET_STATUS rc;
	INDV_REC *indv;	/* for debugging */
	INDV_REC_HNDL hndl[_INDV_TEST_HNDLS], tmp_hndl;
	INDV_REC indv_test[_INDV_TEST_HNDLS];	/* test values */
	INDIVIDUAL_ID *id_ret;
	ACCOUNT_ID acct_id, *acct_ret;
	INDV_CUR_HNDL cur;
	char *sub_ret;
	char can_charge_ret;
	BOOLEAN rec_ok[_INDV_TEST_HNDLS];	/* mark good records */
	int mode, key_type = INDV_PRIMARY_KEY;
	char buf[256], name[256];
    char err_msg[STD_ERR_MSG_SIZE];

	strncpy(exe_name, argv[0], sizeof(exe_name)-1);
	exe_name[sizeof(exe_name)-1] = '\0';

	/* open or read array of  objects */
	if ((rc = _Individual_db_open()) != SUCCESS)
    {
		sprintf(err_msg,
         "%s : ERROR EXIT: _Individual_open() err(%d).\n", exe_name,  rc);
        INDV_LOG_FLAG(rc, err_msg, FORCE_LOG, FORCE_ABORT);
    }

	for (i = 0; i < (sizeof(hndl)/sizeof(hndl[0])); i++)	/* fill array */
	{
		/* TEST Individual_alloc() */

		if ((rc = Individual_alloc( &hndl[i] )) != SUCCESS)
		{
			sprintf (err_msg, "%s : Individual_alloc() err(%d).\n", exe_name, rc);
        	INDV_LOG_FLAG(rc, err_msg, FORCE_LOG, FORCE_ABORT);
		}
		indv = (INDV_REC *)hndl[i];

		/* */
		/* TEST db based objects */
		/* */

		/* */
		/* fill in objects */
		/* */

		_INDV_fill_rec((INDV_REC *)hndl[i]);
/*999*
		_INDV_dump_rec(hndl[i]);	/*999*/

		rec_ok[i] = FALSE;
		if (_INDV_add_update(hndl[i]))
		{
			rec_ok[i] = TRUE;
			added++;	/* count added records */

			if (SUCCESS!= (rc = Individual_read(hndl[i], DB_EQUAL, key_type)))
				printf (
				 "%s : Individual_read(DB_EQUAL) err(%d).\n", exe_name, rc);
	
			/* TEST Individual_set/get_rec_info() for db objects */
	
			if (SUCCESS != (rc =
			 Individual_get_rec_info(hndl[i], INDV_ID, (HNDL)&id_ret)))
				printf (
				 "Individual_get_rec_info(INDV_ID) err(%d).\n", exe_name, rc);
	
			indv_test[i].individual_id = *id_ret;
	
			if (SUCCESS != (rc =
			 Individual_set_rec_info(
			 hndl[i], INDV_ID, (HNDL)&indv_test[i].individual_id)))
				printf (
				 "Individual_set_rec_info(INDV_ID) err(%d).\n", exe_name, rc);
	
	
			if (SUCCESS != (rc =
			 Individual_get_rec_info(hndl[i], INDV_ACCOUNT_ID, (HNDL)&acct_ret)))
				printf (
				 "Individual_get_rec_info(INDV_ACCOUNT_ID) err(%d).\n", exe_name, rc);
	
			indv_test[i].individual_id.acct_id = *acct_ret;
	
			if (SUCCESS != (rc =
			 Individual_set_rec_info(
			 hndl[i], INDV_ACCOUNT_ID, (HNDL)&indv_test[i].individual_id.acct_id)))
				printf (
				 "Individual_set_rec_info(INDV_ACCOUNT_ID) err(%d).\n",
				 exe_name, rc);
	
	
			if (SUCCESS != (rc =
			 Individual_get_rec_info(hndl[i], INDV_SUBACCOUNT, (HNDL)&sub_ret)))
				printf (
				 "Individual_get_rec_info(INDV_SUBACCOUNT) err(%d).\n", exe_name, rc);
	
			memcpy(
			 indv_test[i].subacct_nbr, sub_ret, sizeof(indv_test[i].subacct_nbr));
	
			/* set to new value and set */
	
			sprintf(
			 indv_test[i].subacct_nbr,
			 "n%d", ((sizeof(hndl)/sizeof(hndl[0]) - i)%100));
	
			if (SUCCESS != (rc =
			 Individual_set_rec_info(
			 hndl[i], INDV_SUBACCOUNT, (HNDL)indv_test[i].subacct_nbr)))
				printf (
				 "Individual_set_rec_info(INDV_SUBACCOUNT) err(%d).\n",
				 exe_name, rc);
	
	
			if (SUCCESS != (rc =
			 Individual_get_rec_info(hndl[i],
			  INDV_CAN_CHARGE, (HNDL)&can_charge_ret)))
				printf (
				 "Individual_get_rec_info(INDV_CAN_CHARGE) err(%d).\n", exe_name, rc);
	
			indv_test[i].can_charge = can_charge_ret;
	
			if (SUCCESS != (rc =
			 Individual_set_rec_info(
			 hndl[i], INDV_CAN_CHARGE, (HNDL)indv_test[i].can_charge)))
				printf (
				 "Individual_set_rec_info(INDV_CAN_CHARGE err(%d).\n",
				 exe_name, rc);
	
	
			/* TEST Individual_update() */
	
			if (SUCCESS != (rc = Individual_update(hndl[i])))
			{
				sprintf (
				 err_msg, "%s : Individual_update() err(%d).\n", exe_name, rc);
        		INDV_LOG_FLAG(rc, err_msg, FORCE_LOG, FORCE_ABORT);
			}
	
			/* a lark */
	
			if (SUCCESS != (rc = Individual_read(hndl[i], DB_EQUAL, key_type)))
				printf ("%s : Individual_read() err(%d).\n", exe_name, rc);

        }
	}		/* end open all objects */
    printf ("%s : (%d) rows added/updated.\n", exe_name, added);

	if (SUCCESS != (rc = Individual_alloc(&tmp_hndl)))
        INDV_LOG_FLAG(rc, "Unable to alloc tmp_hndl", FORCE_LOG, FORCE_ABORT);

	i = 0;
	if (SUCCESS != (rc = Individual_read(tmp_hndl, DB_FIRST, key_type)))
		printf ("%s : ERROR: Individual_read(DB_FIRST) err(%d).\n",
		 exe_name, rc);

	i++;

    for ( ; i < (sizeof(hndl)/sizeof(hndl[0])); i++)    /* test array */
    {
        /* test new services here */

        if (rec_ok[i])
        {
            if (SUCCESS != (rc = Individual_read(tmp_hndl, DB_NEXT, key_type)))
                printf (
                 "%s : ERROR:[%d] Individual_read(DB_NEXT) err(%d).\n",
                  exe_name, i,rc);
        }
/*999*
		_INDV_dump_rec(tmp_hndl);	/*999*/

    }

	rc = Individual_free(&tmp_hndl);

    printf ("%s : compare set/update/read/get results.\n", exe_name);
	for (i = 0; i < (sizeof(hndl)/sizeof(hndl[0])); i++)	/* test array */
	{
        /* test new services here */

        if (rec_ok[i])
        {
			if (SUCCESS != (rc = Individual_read(hndl[i], DB_EQUAL, key_type)))
			{
				sprintf (err_msg,
				 "%s : ERROR: Individual_read(DB_EQUAL) err(%d).\n",
		 		exe_name, rc);
        		INDV_LOG_FLAG(rc, err_msg, FORCE_LOG, FORCE_ABORT);
			}
			indv = (INDV_REC *)hndl[i];

            /* TEST Individual_get_rec_info() for db objects */

            if (SUCCESS != (rc =
             Individual_get_rec_info(hndl[i], INDV_ID, (HNDL)&id_ret )))
                printf ("%s : Individual_get_rec_info(INDV_ID) err(%d).\n",
                 exe_name, rc);
            if (memcmp(id_ret,
			 &indv_test[i].individual_id, sizeof(indv_test[i].individual_id)))
               printf (
               "%s: UNEXPECTED RESULT: INDV_ID[%d] set/update/read/get FAILED.\n",
                  exe_name, i);
             else
                printf(
				 "%s: INDV_ID[%d] set/update/read/get OK.\n", exe_name,i);

            /* compare other set/update/read/get results here */

            if (SUCCESS != (rc =
             Individual_get_rec_info(hndl[i], INDV_ACCOUNT_ID,(HNDL)&acct_ret)))
                printf (
				 "%s : Individual_get_rec_info(INDV_ACCOUNT_ID) err(%d).\n",
                 exe_name, rc);
            if (memcmp(acct_ret,
			 &indv_test[i].individual_id.acct_id,
			 sizeof(indv_test[i].individual_id.acct_id)))
               printf (
               "%s: UNEXPECTED RESULT: INDV_ACCOUNT_ID[%d] set/update/read/get FAILED.\n",
                  exe_name, i);
             else
                printf(
				 "%s: INDV_ACCOUNT_ID[%d] set/update/read/get OK.\n",
				 exe_name,i);


			if (SUCCESS != (rc =
			 Individual_get_rec_info(hndl[i], INDV_SUBACCOUNT, (HNDL)&sub_ret)))
				printf (
				 "Individual_get_rec_info(INDV_SUBACCOUNT) err(%d).\n",
				  exe_name, rc);
			if (memcmp(sub_ret,
			 indv_test[i].subacct_nbr, sizeof(indv_test[i].subacct_nbr)))
               printf (
               "%s: UNEXPECTED RESULT: INDV_SUBACCOUNT[%d] set/update/read/get FAILED.\n",
                  exe_name, i);
             else
                printf(
				 "%s: INDV_SUBACCOUNT[%d] set/update/read/get OK.\n",
				 exe_name,i);

			if (SUCCESS != (rc =
			 Individual_get_rec_info(hndl[i],
			 INDV_CAN_CHARGE, (HNDL)&can_charge_ret)))
				printf (
				 "Individual_get_rec_info(INDV_CAN_CHARGE) err(%d).\n",
				 exe_name, rc);
			if (can_charge_ret != indv_test[i].can_charge)
               printf (
               "%s: UNEXPECTED RESULT: INDV_CAN_CHARGE[%d] set/update/read/get FAILED.\n",
                  exe_name, i);
             else
                printf(
				 "%s: INDV_CAN_CHARGE[%d] set/update/read/get OK.\n",
				 exe_name,i);


/*999*  * leave record in db for tests below *
            if (SUCCESS == (rc = Individual_delete(hndl[i])))
                deleted++;
            else
                printf ("%s : ERROR: Individual_delete() err(%d).\n",
                 exe_name, rc);
*999*   * leave record in db for tests below */

        }
    }
    printf("%s : (%d) records deleted.\n", exe_name, deleted);

	/* test Individual list processing */

	if (SUCCESS != (rc = Individual_alloc(&tmp_hndl)))
        INDV_LOG_FLAG(rc, "Unable to alloc tmp_hndl", FORCE_LOG, FORCE_ABORT);

	/* find all indv's for an account */
	found = 0;
	for (i = 0; i < strlen(alpha)-1; i++)
	{
    	acct_id.fleet = alpha[i%(sizeof(alpha)-1)];
		sprintf(acct_id.nbr, "nbr(%d)", (i%(sizeof(alpha)-1)));
		if (SUCCESS != (rc = Individual_open_list(&acct_id, &cur)))
			printf ("%s : Individual_open_list(fleet=%c, ) failed.",
			 exe_name, acct_id.fleet);

		while (rc == SUCCESS)
		{
			if (SUCCESS == (rc = Individual_read_list(cur, tmp_hndl)))
				found++;
		}
		rc = Individual_close_list(&cur);

		printf ("%s : (%d) Individual's found for (fleet,nbr) \'%c\',%s.\n",
		 exe_name, found, acct_id.fleet, acct_id.nbr);

		found = 0;
	}

	/* read exact's */
	found = 0;
	for (i = 0; i < strlen(alpha)-1; i++)
	{
    	acct_id.fleet = alpha[i%(sizeof(alpha)-1)];
		sprintf(acct_id.nbr, "nbr(%d)", (i%(sizeof(alpha)-1)));
		if (SUCCESS != (rc = Individual_open_list(&acct_id, &cur)))
			printf ("%s : Individual_open_list(fleet=%c, ) failed.",
			 exe_name, acct_id.fleet);

    	sprintf(name, "name(%d,%c)", i, acct_id.fleet);

		if (SUCCESS == (rc = Individual_read_exact(cur, name, tmp_hndl)))
		{
			printf ("%s : Individual %s exactly found (fleet,nbr,name) \'%c\',%s,%s.\n",
			exe_name, name, acct_id.fleet, acct_id.nbr, name);

			_INDV_dump_rec(tmp_hndl);
		}


    	sprintf(name, "name(%d)", i, acct_id.fleet);	/* non-matching name here */

		if (SUCCESS == (rc = Individual_read_exact(cur, name, tmp_hndl)))
			printf (
			 "%s : UNEXPECTED:: Individual %s exactly found (fleet,nbr,name) \'%c\',%s,%s.\n",
			 exe_name, name, acct_id.fleet, acct_id.nbr, name);


		rc = Individual_close_list(&cur);
	}
	printf ("\n");

	/* test Individual_remove_account() */
    acct_id.fleet = alpha[1%(sizeof(alpha)-1)];
	sprintf(acct_id.nbr, "nbr(%d)", (1%(sizeof(alpha)-1)));
	rc = Individual_remove_account(&acct_id);
	if (rc == SUCCESS)
		printf ("%s : Individuals removed for Account (flt\'%c\", nbr\"%s\").\n",
		 exe_name, acct_id.fleet, acct_id.nbr);
	else
		printf ("%s : Individuals NOT removed for Account (flt\'%c\", nbr\"%s\").\n",
		 exe_name, acct_id.fleet, acct_id.nbr);


	/* test Individual_get_choice() */

	printf ("\nTest Individual_get_choice(): Invalid Accounts.\n");
	for (i = 0; i < 3; i++)
	{
		/* make some invalid accounts */

    	acct_id.fleet = '\0';
		sprintf(acct_id.nbr, "xyz(%d)", i);
		
		if (FAIL == (rc = Individual_get_choice(&acct_id, tmp_hndl)))
			break;	/* quick out */
	}

	if (SUCCESS != (rc = Individual_read(tmp_hndl, DB_FIRST, key_type)))
	{
		sprintf (err_msg,
		 "%s : ERROR: Individual_read(DB_FIRST) err(%d).\n", exe_name, rc);
   		INDV_LOG_FLAG(rc, err_msg, FORCE_LOG, FORCE_ABORT);
	}
	indv = (INDV_REC *)tmp_hndl;

	for (i = 0; i < 3 && rc == SUCCESS; i++)
	{
		acct_id = ((INDV_REC *)tmp_hndl)->individual_id.acct_id;
		
		if (FAIL == (rc = Individual_get_choice(&acct_id, tmp_hndl)))
			break;	/* quick out */

		if (SUCCESS != (rc = Individual_read(tmp_hndl, DB_NEXT, key_type)))
		{
			sprintf (err_msg,
			 "%s : ERROR: Individual_read(DB_NEXT) err(%d).\n", exe_name, rc);
       		INDV_LOG_FLAG(rc, err_msg, FORCE_LOG, FORCE_ABORT);
		}
		indv = (INDV_REC *)tmp_hndl;
	}

	printf ("\nTest Individual_get_choice(): Valid Accounts.\n");

	for (i = 0; i < strlen(alpha)-1; i++)
	{
		/* make some valid accounts */

    	acct_id.fleet = alpha[i%(sizeof(alpha)-1)];
		sprintf(acct_id.nbr, "nbr(%d)", (i%(sizeof(alpha)-1)));
		
		if (FAIL == (rc = Individual_get_choice(&acct_id, tmp_hndl)))
			break;	/* quick out */
	}

	rc = Individual_free(&tmp_hndl);

    /* TEST Individual_release_locks() for db objects */
    if (SUCCESS != (rc = Individual_release_locks()))
        printf ("%s : Individual_release_locks() err(%d).\n", exe_name,  rc);


	/* ERROR cases */ /* ERROR cases */ 

	printf("%s: ERROR cases: set/get\n", exe_name);

	/* TEST Individual_set_rec_info(NULL) for db objects */

	if (SUCCESS != (rc =
	 Individual_set_rec_info(hndl[i], INDV_NAME, NULL )))
		printf ("%s: Individual_set_rec_info(NULL) returned err(%d).\n",
		 exe_name, rc);
	else
		printf (
	 	"%s: UNEXPECTED RESULT: Object_set_rec_info(NULL) should report error.\n",
		  exe_name);

	/* TEST Individual_set_rec_info(-888) for db objects */

	if (SUCCESS != (rc = Individual_set_rec_info(hndl[i], -888, (HNDL)buf )))
		printf ("%s: Individual_set_rec_info(attribute=-888) returned err(%d).\n",
		 exe_name, rc);
	else
		printf (
	 	"%s: UNEXPECTED RESULT: Object_set_rec_info(attribute=-888) should report error.\n",
		  exe_name);

	/* TEST Individual_get_rec_info(NULL, ) for db objects */

	if (SUCCESS != (rc =
	 Individual_get_rec_info(NULL, INDV_NAME, (HNDL)buf)))
		printf ("%s: Individual_get_rec_info(NULL, ) returned err(%d).\n",
		 exe_name, rc);
	else
		printf (
	 	"%s: UNEXPECTED RESULT: Object_get_rec_info(NULL, ) should report error.\n",
		  exe_name);

	/* TEST Individual_get_rec_info(, NULL) for db objects */

	if (SUCCESS != (rc =
	 Individual_get_rec_info(hndl[i], INDV_NAME, NULL )))
		printf ("%s: Individual_get_rec_info(, NULL) returned err(%d).\n",
		 exe_name, rc);
	else
		printf (
	 	"%s: UNEXPECTED RESULT: Object_get_rec_info(, NULL) should report error.\n",
		  exe_name);

	/* TEST Individual_get_rec_info(-888) for db objects */

	if (SUCCESS != (rc =
	 Individual_get_rec_info(hndl[i], -888, (HNDL)buf )))
		printf ("%s: Individual_get_rec_info(attribute=-888) returned err(%d).\n",
		 exe_name, rc);
	else
		printf (
	 	"%s: UNEXPECTED RESULT: Object_get_rec_info(attribute=-888) should report error.\n",
		  exe_name);

	/* ERROR cases */

	if (SUCCESS != (rc = Individual_set_rec_info(NULL, INDV_ID, (HNDL)buf )))
		printf ("%s: Individual_set_rec_info(NULL, ) returned err(%d).\n",
		 exe_name, rc);
	else
		printf (
	 	"%s: UNEXPECTED RESULT: Object_set_rec_info(NULL, ) should report error.\n",
		  exe_name);
	if (SUCCESS != (rc = Individual_set_rec_info(hndl[i], INDV_ID, NULL )))
		printf ("%s: Individual_set_rec_info(, NULL) returned err(%d).\n",
		 exe_name, rc);
	else
		printf (
	 	"%s: UNEXPECTED RESULT: Object_set_rec_info(, NULL) should report error.\n",
		  exe_name);

	/* TEST Individual_set_rec_info(-888) (Invalid_Attr) for db objects */

	if (SUCCESS != (rc = Individual_set_rec_info(hndl[i], -888, (HNDL)buf )))
		printf ("%s: Individual_set_rec_info(attribute=-888) returned err(%d).\n",
		 exe_name, rc);
	else
		printf (
	 	"%s: UNEXPECTED RESULT: Object_set_rec_info(attribute=-888) should report error.\n",
		  exe_name);

	/* ERROR cases */

	if (SUCCESS != (rc =
	 Individual_get_rec_info(NULL, INDV_ID, (HNDL)buf )))
		printf ("%s: Individual_get_rec_info(NULL, ) returned err(%d).\n",
		 exe_name, rc);
	else
		printf (
	 	"%s: UNEXPECTED RESULT: Object_get_rec_info(NULL, ) should report error.\n",
		  exe_name);
	if (SUCCESS != (rc = Individual_get_rec_info(hndl[i], INDV_ID, NULL )))
		printf ("%s: Individual_get_rec_info(, NULL) returned err(%d).\n",
		 exe_name, rc);
	else
		printf (
	 	"%s: UNEXPECTED RESULT: Object_get_rec_info(, NULL) should report error.\n",
		  exe_name);

	/* TEST Individual_get_rec_info(-888) for db objects */

	if (SUCCESS != (rc = Individual_get_rec_info(hndl[i], -888, (HNDL)buf )))
		printf ("%s: Individual_get_rec_info(attribute=-888) returned err(%d).\n",
		 exe_name, rc);
	else
		printf (
	 	"%s: UNEXPECTED RESULT: Object_get_rec_info(attribute=-888) should report error.\n",
		  exe_name);
	printf("%s: END ERROR cases: set/get\n", exe_name);

	/* ERROR cases */ /* ERROR cases */


	if (_Individual_nbr_objects())	/* just a test for fun */
		printf("%s: (%d) \'%s\' Objects active.\n",
		 exe_name, _Individual_nbr_objects(), INDIVIDUAL_TYPE_NAME);

	/* TEST Individual_free() for memory objects */

	printf("%s: ERROR cases: Individual_free:\n", exe_name);
	if ((rc = Individual_free( NULL )) != SUCCESS)
		printf (
		 "%s: Individual_free(NULL) reports error(%d).\n", exe_name, rc);
	else
		printf (
		 "%s: UNEXPECTED RESULT: Individual_free(NULL) should report error.\n",
		 exe_name);
	memset(buf, '\0', sizeof(buf));	/* NULL */
	if ((rc = Individual_free( (INDV_REC_HNDL *)buf )) != SUCCESS)
		printf (
		 "%s: Individual_free(handle NULL) reports error(%d).\n",
		 exe_name, rc);
	else
		printf (
		 "%s: UNEXPECTED RESULT: Individual_free(handle NULL) should report error.\n",
		 exe_name);

	printf("%s: END ERROR cases: Individual_free:\n", exe_name);

	for (i = _Individual_nbr_objects()-1; i >= 0; i--)
	{

		/* TEST Individual_free() for memory objects */

		if ((rc = Individual_free( &hndl[i] )) != SUCCESS)
			printf ("%s: Individual_free() err(%d).\n", exe_name, rc);
		if (hndl[i])	
			printf (
			 "%s: UNEXPECTED RESULT: hndl[%d] should be NULL after Individual_free() call.\n",
			  exe_name);
	}
	if ((rc = Individual_free( NULL )) != SUCCESS)
		printf (
		 "%s: Individual_free(NULL) reports error(%d).\n", exe_name, rc);
	else
		printf (
		 "%s: UNEXPECTED RESULT: Individual_free(NULL) should report error.\n",
		 exe_name);

	if (_Individual_nbr_objects())
		printf("%s: UNEXPECTED RESULT: All Objects(%s) NOT free; (%d)left.\n",
		 exe_name, INDIVIDUAL_TYPE_NAME, _Individual_nbr_objects());
	else
		printf("%s: All Objects free.\n", exe_name);

	INDV_LOG_FLAG(SUCCESS, "Have a really nice day, Michael", FORCE_LOG, FORCE_ABORT);
}

#endif	// TEST_INDIVIDUAL
