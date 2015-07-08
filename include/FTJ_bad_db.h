/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: FTJ_bad_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2001/03/12 16:42:05 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/FTJ_bad_db.h,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ifndef _FTJ_BAD_DB_H
#define _FTJ_BAD_DB_H
#include <isam.h>
#define FTJ_NBR_LEN 10
#define FTJ_DATE_LEN 9
typedef struct _FTJ_BAD_STRUCT {
    char       ftj_number[ FTJ_NBR_LEN + 1 ];
    char       ftj_start_date[ FTJ_DATE_LEN + 1 ];
    char       ftj_added_date[ FTJ_DATE_LEN + 1 ];
} FTJ_BAD_STRUCT;
#ifdef KEY_SRC
struct keydesc ftj_bad_key1 = {
    ISNODUPS,
    1,
    { { 0, FTJ_NBR_LEN, CHARTYPE}
    }
};
#else
extern struct keydesc ftj_bad_key1;
#endif
#endif
