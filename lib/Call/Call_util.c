/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call_util.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2000/10/23 19:06:18 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/Call/Call_util.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

static char rcsid[] = "$Id: Call_util.c,v 1.2 2000/10/23 19:06:18 taxiadm Exp $";

#include <sys/time.h>
#include "taxipak.h"
#include "Object.h"
#include "taxi_error.h"
#include "Call_public.h"
#include "Call_private.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "mem_mgmt.h"
#include "taxi_db.h"
#include "Call_db.h"

static struct keydesc *Call_key_desc[] =
{
   &cl_key1,  /* Note cl_key1 in zeroth and first position */
   &cl_key1,
   &cl_key2,
   &cl_key3,
   &cl_key4,
   &cl_key5,
   &cl_key6,
   &cl_key7,
   &cl_key8,
   &cl_key9
};


#define CALL_MAX_KEY 10

RET_STATUS
Call_rec_alloc(CALL_REC_HNDL * hndl)
{
   char  err_msg[STD_ERR_MSG_SIZE];

   if (hndl == NULL)		/* test if a valid addr for hndl */
      return TAXI_ERR(NULL_ARG, "Null object handle address; Call_alloc()");

   *hndl = (CALL_REC_HNDL) TMALLOC(CALL_REC, 1);	/* do alloc */
   if (*hndl == HNDL_NULL)	/* did malloc work? */
   {
      sprintf(err_msg,
	      "Unable to allocate Call record object");
      return TAXI_ERR(NO_MEMORY, err_msg);
   }
   /* zero zero zero */
   memset((char *) *hndl, '\0', sizeof(CALL_REC));

   return SUCCESS;
}				/* end Call_rec_alloc */

RET_STATUS
Call_rec_free(CALL_REC_HNDL * hndl)
{
   char  err_msg[STD_ERR_MSG_SIZE];

   /* test for (~potentially)valid pointer */
   if ((hndl != NULL) && *hndl)
   {
      /* de-allocate */
      TFREE(*hndl);

      *hndl = NULL;		/* clear this up */

      return SUCCESS;
   }
   strcpy(err_msg, "Call_free(");
   if (hndl == NULL)
      strcat(err_msg, "object handle address NULL");
   else if (!*hndl)
      strcat(err_msg, "object handle NULL");
   return TAXI_ERR(NULL_ARG, err_msg);
}				/* end Call_rec_free */


/* read an object from the data base */
RET_STATUS
Call_rec_read(
	  CALL_REC_HNDL hndl,
	  int mode,
	  int key_type)
{
   RET_STATUS rc;
   char  err_msg[STD_ERR_MSG_SIZE];

   if (!hndl)
      return TAXI_ERR(NULL_ARG, "Null object handle; Call_read()");

    rc = db_read_key(CALL_FILE_ID, hndl,
    Call_key_desc[key_type], mode); 

   if (rc != SUCCESS)		/* test errors */
   {
       switch( err_value(rc) )
       {
       case EENDFILE:
       case ENOREC:
	   break;      /* no logging for these errors... */

       default:
	   sprintf(err_msg,
		   "DB read(%s _FILE_ID, , key_type(%d), %d); Call_read()",
		   "Call", key_type, mode);
	   TAXI_ERR(rc, err_msg);
       }
       return (rc);
   }
   return (SUCCESS);
}				/* end Call_read */

RET_STATUS
Call_release_locks(void)
{
   RET_STATUS rc;
   char  err_msg[STD_ERR_MSG_SIZE];

   /* call db to release */
   rc = db_release(CALL_FILE_ID);
   if (rc != SUCCESS)		/* test for success */
      return TAXI_ERR(rc,
		 "DB release lock failed; Call_release_locks()");

   return SUCCESS;
}				/* end Call_release_locks */

RET_STATUS
Call_db_add(CALL_REC_HNDL hndl)	/* record is implied in hndl */
{
   RET_STATUS rc;

   if (!hndl)
      return TAXI_ERR(NULL_ARG,
		 "Null object handle; Call_add()");

   /* write to the data base */
   rc = db_write(CALL_FILE_ID, hndl);
   if (rc != SUCCESS)
      return TAXI_ERR(rc,
		 "DB Add Record failed; Call_add()");

   return SUCCESS;
}				/* end Call_add */

RET_STATUS
Call_db_update(CALL_REC_HNDL hndl)		/* update an already existing record */
{
   RET_STATUS rc;
   char  err_msg[STD_ERR_MSG_SIZE];

   if (!hndl)
      return TAXI_ERR(NULL_ARG, "Null object handle; Call_update()");

   /*
    * CALLER HANDLED : client sequence for an update: Call_read - record with
    * lock Call_set_rec_info - set items in record Call_update
    * Call_release_locks * CALLER HANDLED
    */

   /* update record */
   rc = db_update(CALL_FILE_ID, hndl);
   if (rc != SUCCESS)
      return TAXI_ERR(rc, "DB Update Record failed; Call_update()");

   return SUCCESS;
}				/* end Call_update */

RET_STATUS
Call_db_delete(CALL_REC_HNDL hndl)
{
   RET_STATUS rc;
   char  err_msg[STD_ERR_MSG_SIZE];

   if (hndl)			/* read to proper record */
   {
      rc = db_read(CALL_FILE_ID, hndl, DB_EQUAL);
      if (rc != SUCCESS)	/* handle errors */
      {
	 if (err_value(rc) != RECORD_NOT_FOUND)
	    return TAXI_ERR(rc,
		       "DB Read Record failed; Call_delete()");
	 return rc;
      }
   }
   /* delete current record */
   rc = db_delete(CALL_FILE_ID, hndl);
   if (rc != SUCCESS)		/* handle errors */
      return TAXI_ERR(rc, "DB Delete Record failed; Call_update()");

   return SUCCESS;
}				/* end Call_delete */
