/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Individual_private.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/05/12 19:17:34 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Individual_private.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Individual_private.h,v $	$Revision: 1.2 $"

#ifndef Individual_private_H
#define Individual_private_H

#include "Individual.h"
#include "Scrn.h"

#define PRIVATE_INDV_FUNC	/* document "private" functions */

#define INDIVIDUAL_TYPE_NAME	"Individual"	/* name type of object */
#define INDIVIDUAL_ID_NBR	INDV_OBJ	/* unique across all objects */

#define INDIVIDUAL_MAX_NAME_LEN 100	/* specific name of this object */

/* create RET_STATUS'es using INDV_rcode=INDV_LOG(code, msg) */

#define INDV_SPECIAL_LOG(code,group,msg) \
	(((code != SUCCESS) && (err_object(code)==0))?	/* test if indv error */ \
	(TAXI_ERR(err_encode_special(INDIVIDUAL_ID_NBR,(group),(code)), (msg))): \
	(TAXI_ERR((code), (msg))))
#define INDV_LOG(code,msg) (INDV_SPECIAL_LOG((code),APP_ERR_GP,(msg)))
#define INDV_SPECIAL_LOG_FLAG(code,group,msg,log,abort) \
	(((code != SUCCESS) && (err_object(code)==0))?	/* test if indv error */ \
    (TAXI_ERR_FLAG( \
    err_encode_special( \
	INDIVIDUAL_ID_NBR,(group),(code)),(msg),(log),(abort))): \
    (TAXI_ERR_FLAG((code),(msg),(log),(abort))))
#define INDV_LOG_FLAG(code,msg,log,abort) \
	(INDV_SPECIAL_LOG_FLAG((code), APP_ERR_GP, (msg), (log), (abort)))


#define INDV_MAX_INDIVIDUAL_LIST    8

typedef struct cisam_indv
{
    INDIVIDUAL_ID individual_id;

    char    subacct_nbr[INDV_SUBACCOUNT_LEN + 1];
    char    can_charge;
} INDIVIDUAL;

typedef INDIVIDUAL INDV_REC;    /* for taxi_db.h */

typedef struct indv_rd_ctxt
{
    ACCOUNT_ID  acct_id;
    BOOLEAN     not_read;
	RET_STATUS	prev_page_rc;
} INDV_LIST_CONTEXT, *INDV_LIST_HANDLE;



/* private function prototypes */

/* nbr of objects active */
PRIVATE_INDV_FUNC static int _Individual_nbr_objects(void);
PRIVATE_INDV_FUNC RET_STATUS _Individual_db_open(void);
PRIVATE_INDV_FUNC static RET_STATUS Individual_rec_value(/* get || set */
	INDV_REC_HNDL hndl, int attr, HNDL buf, int get_put);
RET_STATUS Individual_get_next_page(SCRN_CHOICE_LIST *list);
RET_STATUS Individual_get_prev_page(SCRN_CHOICE_LIST *list); 



/****TEST_INDIVIDUAL***********************************************************/
/****TEST_INDIVIDUAL***********************************************************/
/****TEST_INDIVIDUAL***********************************************************/
/*
 *
 * test section : define -DTEST_INDIVIDUAL for testing
 * 
 */

#ifdef TEST_INDIVIDUAL

#define TAXI_ERR(code, msg)		/* rc=TAXI_ERR(rc, "message") */ \
	(printf("TAXI_ERR @[ %s(%d) ]:: rc=%d(obj=%d, grp=%d, ret=%d) ms=\n\"%s.\"\n",\
	__FILE__, __LINE__, (code), \
	err_object((code)), err_group((code)), err_value((code)), (msg)))?  \
	(code):(code)		/* hack for code returning and error printing */

#define TAXI_ERR_FLAG(code,msg,log_flag,abort_flag) \
    ((printf("TAXI_ERR_FLAG @[ %s(%d) ]:: (obj=%d, grp=%d, ret=%d) ms=\n\"%s.\"\n",\
	__FILE__, __LINE__, \
    err_object((code)), err_group((code)), err_value((code)), (msg)))?  \
    ((abort_flag==FORCE_ABORT)?(exit(1)):(code)): \
    ((abort_flag==FORCE_ABORT)?(exit(1)):(code))) \
    /* hack for printing and returning code or printing code and exiting */

#define catgets(a,b,c,dmsg) (dmsg)

	/* memory management when testing */

void *malloc(int);			/* do this or die */
#define TMALLOC(type, cnt)	(type *)malloc(sizeof(type)*(cnt))
#define TFREE(ptr)			free((ptr))

#endif      // #ifdef TEST_INDIVIDUAL


#endif	// Individual_private_H
