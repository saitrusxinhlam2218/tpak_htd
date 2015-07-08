/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Account.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/05/12 19:17:16 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Account.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef Account_H
#define Account_H
#include <sys/types.h>	/* time_t */
#include <sys/time.h>
#include "taxipak.h"
#include "Object.h"
#include "Address.h"
/* key indices */
#define AC_KEY_PRIMARY      0
#define AC_PRIMARY_KEY AC_KEY_PRIMARY	/* spelling is another way */
/* error codes */
#define AC_NEGATIVE_POINTS	err_encode(AC_OBJ, ERR_OBJ_BEGIN+1)
#define AC_RESERVE_UPDATE	err_encode(AC_OBJ, ERR_OBJ_BEGIN+2)
#define AC_NO_ACCESS		err_encode(AC_OBJ, ERR_OBJ_BEGIN+3)
#define AC_INV_ACCOUNT_TYPE	err_encode(AC_OBJ, ERR_OBJ_BEGIN+4)
#define AC_INSUFFICIENT_POINTS	err_encode(AC_OBJ, ERR_OBJ_BEGIN+5)
#define AC_SUSPEND_SUB		err_encode(AC_OBJ, ERR_OBJ_BEGIN+6)
#define AC_NO_MEMORY		err_encode(AC_OBJ, NO_MEMORY)
#define AC_NULL_ARG			err_encode(AC_OBJ, NULL_ARG)
#define AC_INVALID_ATTR		err_encode(AC_OBJ, INVALID_ATTR)
#define AC_INVALID_ARG		err_encode(AC_OBJ, INVALID_ARG)
#define AC_RECORD_NOT_FOUND	err_encode(AC_OBJ, RECORD_NOT_FOUND)
#define AC_NO_SCREEN_CHOICE	err_encode(AC_OBJ, NO_CHOICE)
/* lengths */
#define AC_NUMBER_LEN           18
#define AC_NAME_LEN             25
#define AC_PHONE_CONTACT_LEN    20
#define AC_ATTENTION_LEN        20
#define AC_COMMENT_LEN          40
#define AC_HEADER_1_LEN         30
#define AC_HEADER_2_LEN         15
#define AC_DRIVER_MSG_LEN       32
#define AC_RECEIPT_NUMBER_LEN    4
#define AC_PASSWORD_LEN          4
typedef enum
{
	/* Account Object individual attributes */
	AC_NAME,
	AC_PHONE_CONTACT,
	AC_ATTENTION,
	AC_COMMENT,
	AC_HEADER_1,
	AC_HEADER_2,
	AC_HEADER_3,
	AC_HEADER_4,
	AC_DRIVER_MSG,
	AC_PRIORITY,
	AC_SHOW_PICKUP_ADDRESS,
	AC_RECEIPT_NUM,
	AC_AUTOBOOK_ENABLE,
	AC_CHARGE_ENABLE,
	AC_RESERVATION_POINTS,
	AC_SUSPEND_FLAG,
	AC_ACCOUNT_TYPE_ID,
	/* Account Object group attributes */
	AC_ID,
	AC_ADDRESS,
	AC_PHONE_NUMBER,
	AC_PASSWORD,
	AC_DRV_ATTR,
	AC_VEH_ATTR
} AC_ATTR;			/* set/get _info() */
typedef struct Account_id
{
	char fleet;
	char nbr[AC_NUMBER_LEN + 1];
} ACCOUNT_ID;
/* public function proto-types */
BOOLEAN Account_id_compare(ACCOUNT_ID *ac1, ACCOUNT_ID *ac2);	/* test == */
RET_STATUS Account_deduct_reservation_points(ACCOUNT_ID *, time_t, short *);
RET_STATUS Account_check_reservation_points( AC_REC_HNDL, time_t, char flt_id );
RET_STATUS Account_get_required_points(AC_REC_HNDL, time_t, char flt_id, short *);
BOOLEAN Account_is_suspended(AC_REC_HNDL account_handle);
RET_STATUS Account_get_address_choice( ACCOUNT_ID *, STREET_ADDRESS *, PHONE *);
RET_STATUS
 Account_get_individual_choice( ACCOUNT_ID *account_id, INDV_REC_HNDL i_hndl);
/* public maintenance function proto-types */
RET_STATUS Account_alloc(AC_REC_HNDL *);
RET_STATUS Account_free(AC_REC_HNDL *);
	/* for db records/objects */
RET_STATUS Account_read(AC_REC_HNDL, int mode, int key_type);
RET_STATUS Account_release_locks(void);
RET_STATUS Account_add(AC_REC_HNDL);
RET_STATUS Account_update(AC_REC_HNDL);	/* updt already existing record */
RET_STATUS Account_delete(AC_REC_HNDL);
RET_STATUS Account_set_rec_info(AC_REC_HNDL, int attr, HNDL buf);
RET_STATUS Account_get_rec_info(AC_REC_HNDL, int attr, HNDL buf);
#endif	
