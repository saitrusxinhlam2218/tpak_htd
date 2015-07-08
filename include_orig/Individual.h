/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Individual.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/05/12 19:17:33 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Individual.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Individual.h,v $	$Revision: 1.2 $"

#ifndef Individual_H
#define Individual_H

#include "Object.h"
#include "taxi_error.h"

#include "Account.h"

/* index to db keys array */

#define INDV_PRIMARY_KEY 0
#define INDV_SUBACCT_KEY 1

/* length's */

#define INDV_NAME_LEN		20
#define INDV_SUBACCOUNT_LEN	4
#define INDV_CAN_CHARGE_LEN	1
#define INDV_MAX_INDIVIDUAL 10	/* max for choice */

/* error's */

#define NO_INDIVIDUALS_EXIST	ERR_OBJ_BEGIN+1
#define INDV_NONE_CHOSEN		ERR_OBJ_BEGIN+3

#define INDV_NOT_FOUND	err_encode(INDV_OBJ, NO_INDIVIDUALS_EXIST)

#define INDV_NO_MEMORY	err_encode(INDV_OBJ, NO_MEMORY)
#define INDV_INVALID_READ_HNDL	err_encode(INDV_OBJ, NULL_ARG)
#define INDV_NULL_ARG	err_encode(INDV_OBJ, NULL_ARG)
#define INDV_INVALID_ATTR	err_encode(INDV_OBJ, INVALID_ATTR)
#define INDV_INVALID_ARG	err_encode(INDV_OBJ, INVALID_ARG)
#define INDV_END_OF_LIST	err_encode(INDV_OBJ, END_OF_LIST)
#define INDV_FILE_NOT_FOUND	err_encode(INDV_OBJ, FILE_NOT_FOUND)


typedef enum
{
	INDV_ID,		/* the INDIVIDUAL_ID struct */

	INDV_ACCOUNT_ID,
	INDV_NAME,
	INDV_SUBACCOUNT,
	INDV_CAN_CHARGE
} INDV_ATTR;			/* set/get _info() */


typedef struct _indv_pkey
{
	ACCOUNT_ID	acct_id;
	char	name[ INDV_NAME_LEN + 1];
} INDIVIDUAL_ID;

typedef HNDL INDV_CUR_HNDL;


/* public function prototypes */

	/* in Individual_list.c */

RET_STATUS Individual_open_list(ACCOUNT_ID *acct_id, INDV_CUR_HNDL *indv_list);
RET_STATUS Individual_read_list(INDV_CUR_HNDL indv_list, INDV_REC_HNDL buffer);
RET_STATUS
 Individual_read_exact(INDV_CUR_HNDL i_list, char *name, INDV_REC_HNDL buffer);
RET_STATUS Individual_close_list(INDV_CUR_HNDL *indv_list);

RET_STATUS Individual_remove_account(ACCOUNT_ID *);	/* delete indv's w/acct */

	/* in Individual_choice.c */

RET_STATUS Individual_get_choice(ACCOUNT_ID *acct_id, INDV_REC_HNDL individual);


/* public maintenance function prototypes */

RET_STATUS Individual_alloc(INDV_REC_HNDL *);
RET_STATUS Individual_free(INDV_REC_HNDL *);

	/* for db records/objects */
RET_STATUS Individual_set_rec_info(INDV_REC_HNDL, int attr, HNDL buf);
RET_STATUS Individual_get_rec_info(INDV_REC_HNDL, int attr, HNDL buf);
RET_STATUS Individual_read(INDV_REC_HNDL, int mode, int key_type);
RET_STATUS Individual_add(INDV_REC_HNDL);
RET_STATUS Individual_update(INDV_REC_HNDL); /* updt existing record */
RET_STATUS Individual_delete(INDV_REC_HNDL);
RET_STATUS Individual_release_locks(void);


#endif	// Individual_H
