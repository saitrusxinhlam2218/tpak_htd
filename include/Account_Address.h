/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Account_Address.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/05/12 19:17:17 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Account_Address.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef Account_Address_H
#define Account_Address_H
#include "taxipak.h"
#include "Object.h"
#include "taxi_error.h"
#include "Account.h"
#include "Address.h"
#define ACCOUNT_ADDRESS_KEY 0					/* index to db key array */
#define ACAD_PRIMARY_KEY ACCOUNT_ADDRESS_KEY	/* index to db key array */
/* error returns */
#define ACAD_NULL_ARG	err_encode(ACAD_OBJ, NULL_ARG)
#define ACAD_INVALID_ARG	err_encode(ACAD_OBJ, INVALID_ARG)
#define ACAD_INVALID_ATTR	err_encode(ACAD_OBJ, INVALID_ATTR)
#define ACAD_NO_MEMORY	err_encode(ACAD_OBJ, NO_MEMORY)
#define ACAD_NO_CHOICE	err_encode(ACAD_OBJ, NO_CHOICE)
#define ACAD_END_OF_LIST	err_encode(ACAD_OBJ, END_OF_LIST)
typedef enum
{
	ACAD_ID,	/* the id structure */
	ACAD_ADDRESS,	/* address */
	ACAD_PHONE	/* phone */
} ACAD_ATTR;			/* set/get */
typedef struct Account_Address_id
{
	ACCOUNT_ID      account_id;
	int             index;
} ACCOUNT_ADDRESS_ID, *ACCOUNT_ADDRESS_ID_P;
/* public function proto-types */
RET_STATUS
 Account_Address_get_address_choice(ACCOUNT_ID *, STREET_ADDRESS *, PHONE *);
RET_STATUS Account_Address_remove_account(ACCOUNT_ID *);
/* public maintenance function proto-types */
RET_STATUS Account_Address_alloc(ACAD_REC_HNDL *);
RET_STATUS Account_Address_free(ACAD_REC_HNDL *);
	/* for db records/objects */
RET_STATUS Account_Address_set_rec_info(ACAD_REC_HNDL, int attr, HNDL buf);
RET_STATUS Account_Address_get_rec_info(ACAD_REC_HNDL, int attr, HNDL buf);
RET_STATUS Account_Address_release_locks(void);
RET_STATUS Account_Address_read(ACAD_REC_HNDL, int mode, int key_type);
RET_STATUS Account_Address_add(ACAD_REC_HNDL);
RET_STATUS Account_Address_update(ACAD_REC_HNDL); /* updt existing record */
RET_STATUS Account_Address_delete(ACAD_REC_HNDL);
#endif	
