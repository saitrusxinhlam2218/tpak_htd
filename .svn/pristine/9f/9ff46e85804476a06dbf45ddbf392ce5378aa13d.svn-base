/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Bad_credit_db.h,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 1998/11/06 02:04:49 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Bad_credit_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Bad_credit_db.h,v $	$Revision: 1.4 $"

#ifndef _BAD_CREDIT_DB_H
#define _BAD_CREDIT_DB_H


#include <isam.h>

#define ISO2_LEAD_CHAR        ';'
#define ISO2_SEPARATOR        "="

#define BAD_CREDIT_DATE_LEN   8
#define BAD_CREDIT_CARD_LEN   40
#define BAD_CREDIT_TYPE_LEN   4
typedef struct cisam_bad_credit
{
  int        serial_nbr;
  char       date_range;
  char       date_start[ BAD_CREDIT_DATE_LEN + 1 ];
  char       date_end[ BAD_CREDIT_DATE_LEN + 1 ];
  char       card_nbr[ BAD_CREDIT_CARD_LEN + 1 ];
  char       card_type[ BAD_CREDIT_TYPE_LEN + 1 ];
  int        entered_dt_tm;
} BAD_CREDIT_REC;

#define CARD_NBR_OFFSET       LONGSIZE + 1 + (BAD_CREDIT_DATE_LEN * 2)

#ifdef KEY_SRC
struct keydesc bc_key1 = {
  ISDUPS,
  1,
  { { CARD_NBR_OFFSET, BAD_CREDIT_CARD_LEN, CHARTYPE }
  }
};
#else
extern struct keydesc bc_key1;
#endif

#endif
