/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Card_type_db.h,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 1998/11/16 13:28:56 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Card_type_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _CARD_TYPE_DB_H
#define _CARD_TYPE_DB_H
#define CARD_TYPE_TEST_SUCCESS    1
#define CARD_PREFIX_TEST_FAIL    -1
#define CARD_SVC_CODE_TEST_FAIL  -2
#define CARD_TYPE_TEST_FAIL      -3
#define CARD_TYPE_MODULUS_FAIL   -4
#define CARD_TYPE_BAD_CRED_FAIL  -5
#define CARD_TYPE_PREFIX_LEN    19
#define CARD_TYPE_LEN           3
#define CARD_TYPE_COMMENT_LEN   40
#define CARD_TYPE_SERVICE_LEN   8
#define CARD_TYPE_NBR_LEN       8
typedef struct cisam_card_type
{
  int       serial_nbr;
  char      card_prefix[ CARD_TYPE_PREFIX_LEN + 1 ];
  char      accept_reject;
  char      comment[ CARD_TYPE_COMMENT_LEN + 1 ];
  int       entered_dt_tm;
  char      card_type_low[ CARD_TYPE_PREFIX_LEN + 1 ];
  char      card_type_high[ CARD_TYPE_PREFIX_LEN + 1 ];
  short     service_index;
  char      service_nbr[ CARD_TYPE_SERVICE_LEN + 1 ];
  short     type_index;
  char      type_nbr[ CARD_TYPE_NBR_LEN + 1 ];
  char      check_mod;
} CARD_TYPE_REC;
#ifdef KEY_SRC
struct keydesc ct_key1 = {
  ISDUPS,
  1,
  { { 4, CARD_TYPE_PREFIX_LEN, CHARTYPE }
  }
};
#else
extern struct keydesc ct_key1;
#endif
#endif
