/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: AutoCall_private.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2002/03/25 16:10:43 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/AutoCall_private.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: AutoCall_private.h,v $	$Revision: 1.3 $"

#ifndef _AUTOCALL_PRIVATE_H
#define _AUTOCALL_PRIVATE_H

#include "TPakWeb_User.h"

/** TES Client Requests - Generic **/
#define            TES_REQ_ACCT_CALL          101
#define            TES_CANCEL_CALL            102
#define            TES_VALID_ACCT             103
#define            TES_STARTED                104
#define            TES_TERMINATE              105
#define            TES_GEN_EXCEPTION          106
#define            TES_UPDATE_CALL            107
#define            TES_GET_ACCT_INFO          108
#define            TES_CLIENT_DISCONNECT      199

#define            LEN_TES_REQ_ACCT_CALL      12   /* account_nbr, subaccount, acad, nbr_taxis */

/** TES Client Requests - Stockholm **/

/** TES Replies to Clients - generic          **/
#define            TES_CALL_NBR               201
#define            TES_STATUS                 202
#define            TES_OK                     203
#define            TES_KO                     204

/** TES Status Codes                **/
#define            TES_ACCT_OK                201
#define            TES_ACCT_KO                202
#define            TES_CALL_CANX              203
#define            TES_CALL_NOCANX            204

/** TES Web Requests - Generic **/
#define            TES_GET_PROFILE            600
#define            TES_UPDATE_PROFILE         601
#define            TES_ADD_PROFILE            602
#define            TES_REMOVE_PROFILE         603
#define            TES_GET_ADDR               604
#define            TES_UPDATE_ADDR            605
#define            TES_ADD_ADDR               606
#define            TES_REMOVE_ADDR            607

/** TES Web Replies - Generic **/
#define            TES_KO_NOPROFILE           202
#define            TES_KO_ADDR_NO_ZONE        203
#define            TES_KO_NO_ADDR             204

/** AutoCall Message Defines **/
#define            AC_MSG_SIZE_LEN            2
#define            AC_MSG_TES_LEN             256

typedef struct _AC_MSG
{
  char         MsgSize[ AC_MSG_SIZE_LEN ];
  char         MsgPort;
  char         MsgTES[ AC_MSG_TES_LEN ];
} AC_MSG;

/** TES Message Defines **/
#define            TES_TYPE_LEN               3
#define            TES_DATA_LEN               256

typedef struct _TES_MSG
{
  char         TEStype[ TES_TYPE_LEN ];
  char         TESdata[ TES_DATA_LEN ];
} TES_MSG;

#define USER_ID_LEN 6
typedef struct _WEB_USER_MSG
{
  char          user_id[ USER_ID_LEN + 1];
  char          user_name[USER_NAME_LEN + 1];
  char          passwd[USER_PASSWD_LEN + 1];
  char          fullname[USER_FULLNAME_LEN + 1];
  char          tele[USER_TELE_LEN + 1];
  char          email[USER_EMAIL_LEN + 1];
  char          service[USER_SERVICE_LEN + 1];
  char          cookie[USER_COOKIE_LEN + 1];
} WEB_USER_MSG;

typedef struct _WEB_ADDR_MSG
{
  char         webuser_nbr[8];
  char         seq_no[2];
  char         tot_no[2];
  char         str_pre_dir[2];
  char         str_name[20];
  char         str_type[3];
  char         str_post_dir[2];
  char         str_city[3];
  char         str_nbr[6];
  char         str_apartment[5];
  char         str_comment[30];
  char         str_ct_comment[30];
} WEB_ADDR_MSG;

/** Error return codes   **/
#define             TES_KO_SYS_ERR                1000
#define             TES_KO_BAD_FORMAT             1001
#define             TES_KO_BAD_CALL_DATA          2000
#define             TES_KO_NO_CALL                2001
#define             TES_KO_UNZONED                2002
#define             TES_KO_CALL_STATE             2003
#define             TES_KO_NO_ETA                 2004
#define             TES_KO_BAD_ACCOUNT_DATA       3000
#define             TES_KO_NO_ACCOUNT             3001
#define             TES_KO_BAD_EXCEPTION_DATA     4000


/** Defines for TES_REQ_ACCT_CALL **/
#define             TES_AC_NBR_LEN                6
#define             TES_SUBACCT_LEN               2
#define             TES_ACAD_LEN                  2
#define             TES_NBR_TAXIS_LEN             2
#define             TES_AC_NBR_OFFSET             0
#define             TES_SUBACCT_OFFSET            (TES_AC_NBR_OFFSET + TES_AC_NBR_LEN)
#define             TES_ACAD_OFFSET               (TES_SUBACCT_OFFSET + TES_SUBACCT_LEN)
#define             TES_NBR_TAXIS_OFFSET          (TES_ACAD_OFFSET + TES_ACAD_LEN)

#endif
