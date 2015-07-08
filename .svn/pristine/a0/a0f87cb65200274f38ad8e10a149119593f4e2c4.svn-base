/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Account_Address_private.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/05/12 19:17:20 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Account_Address_private.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef Account_Address_private_H
#define Account_Address_private_H
#include "Account_Address.h"
#include "Scrn.h"
#define PRIVATE_ACAD_FUNC	/* document "private" functions */
#define PRIVATE_ACAD_DATA	/* document "private" data */
#define ACCOUNT_ADDRESS_TYPE_NAME "Account_Address"   /* name type of object */
#define ACCOUNT_ADDRESS_ID_NBR	ACAD_OBJ	/* unique across all objects */
/* create RET_STATUS'es using ACAD_rcode=ACAD_LOG(code, msg) */
#define ACAD_SPECIAL_LOG(code,group,msg) \
    (((code != SUCCESS) && (err_object(code)==0))? /* if acad type error */ \
	(TAXI_ERR( \
	err_encode_special(ACCOUNT_ADDRESS_ID_NBR,(group),(code)), (msg))): \
	 (TAXI_ERR((code), (msg))))
#define ACAD_LOG(code,msg)	(ACAD_SPECIAL_LOG((code), APP_ERR_GP, (msg)))
/* RET_STATUS'es using ACAD_rcode=ACAD_LOG_FLAG(code,msg,log?,flag) */
#define ACAD_SPECIAL_LOG_FLAG(code,group,msg,log,abort) \
    (((code != SUCCESS) && (err_object(code)==0))? /* if acad type error */ \
    (TAXI_ERR_FLAG( \
    err_encode_special( \
	ACCOUNT_ADDRESS_ID_NBR,(group),(code)),(msg),(log),(abort))): \
    (TAXI_ERR_FLAG((code),(msg),(log),(abort))))
#define ACAD_LOG_FLAG(code,msg,log,abort) \
    (ACAD_SPECIAL_LOG_FLAG((code), APP_ERR_GP, (msg), (log), (abort)))
#define ACCOUNT_ADDRESS_MAX_NAME_LEN 100    /* specific name of this object */
#define MAX_ADDRESSES_PER_ACCOUNT 10	/* account addresses */
#define FORMATTED_ADDRESS_SIZE 60
typedef struct Account_Address_rec
{
	ACCOUNT_ADDRESS_ID id;
	STREET_ADDRESS address;
	PHONE phone;
} ACCOUNT_ADDRESS, *ACCOUNT_ADDRESS_P;
typedef ACCOUNT_ADDRESS ACAD_REC;	/* for taxi_db.h */
typedef struct _account_addresses
{
	int max_index;
	int next_index;
	ACCOUNT_ADDRESS info[MAX_ADDRESSES_PER_ACCOUNT];
} ACCOUNT_ADDRESSES, *ACCOUNT_ADDRESSES_P;
/* private function prototypes */
PRIVATE_ACAD_FUNC int _Account_Address_nbr_objects(void);/* nbr objects active */
PRIVATE_ACAD_FUNC static RET_STATUS Account_Address_rec_value(/* get || set */
	ACAD_REC_HNDL hndl, int attr, HNDL buf, int get_put);
PRIVATE_ACAD_FUNC RET_STATUS _Account_Address_db_open(void); /* open table */
PRIVATE_ACAD_FUNC RET_STATUS
 Account_Address_format_address(ACCOUNT_ADDRESS *, char *);
	/* call-backs */
PRIVATE_ACAD_FUNC RET_STATUS Account_Address_next_page( SCRN_CHOICE_LIST * );
PRIVATE_ACAD_FUNC RET_STATUS Account_Address_prev_page( SCRN_CHOICE_LIST * ); 
/*
 *
 * test section : define -DTEST_ACCOUNT_ADDRESS for testing
 *
 */
#ifdef TEST_ACCOUNT_ADDRESS
#define TAXI_ERR(code, msg)      /* rc=TAXI_ERR(rc, "message") */ \
    (printf("TAXI_ERR @[ %s(%d) ]:: rc=%d(obj=%d, grp=%d, ret=%d) ms=\n\"%s.\"\n",\
    __FILE__, __LINE__, (code), \
    err_object((code)), err_group((code)), err_value((code)), (msg)))?  \
    (code):(code)       /* hack for code returning and error printing */
#define TAXI_ERR_FLAG(code,msg,log_flag,abort_flag) \
    ((printf("TAXI_ERR_FLAG @[ %s(%d) ]:: (obj=%d, grp=%d, ret=%d) ms=\n\"%s.\"\n",\
    __FILE__, __LINE__, \
    err_object((code)), err_group((code)), err_value((code)), (msg)))?  \
    ((abort_flag==FORCE_ABORT)?(exit(1)):(code)): \
    ((abort_flag==FORCE_ABORT)?(exit(1)):(code))) \
    /* hack for printing and returning code or printing code and exiting */
    /* memory management when testing */
void *malloc(int);          /* do this or die */
#define TMALLOC(type, cnt)  (type *)malloc(sizeof(type)*(cnt))
#define TFREE(ptr)          free((ptr))
#endif      
#endif	
