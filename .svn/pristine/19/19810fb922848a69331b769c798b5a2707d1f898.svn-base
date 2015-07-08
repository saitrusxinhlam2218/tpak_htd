/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: taxipak.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/02/23 21:44:39 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/taxipak.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

/*
 * This file contains definitions common throughout ALL of the
 * TaxiPak application.  These definitions should never be defined
 * elsewhere without sufficient justification.  Likewise, this file
 * should be kept to essentials, and not used as a kitchen sink for
 * convenience mechanisms.
 */

#ifndef _TAXIPAK_H_
#define _TAXIPAK_H_

#ifndef _SCROLLWIN
//#include <cursesX.h>    /* to keep cpp quiet about redef of TRUE/FALSE */
#endif

#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif

#ifndef NULL
#define NULL    0
#endif

#ifndef ESC
#define ESC  '\033'
#endif

#define EOS  '\0'

typedef unsigned int  RET_STATUS;
typedef RET_STATUS (*RC_FUNC_PTR)();

typedef enum {DISPATCH, UI} TAXIPAK_PROCESS;

#define SUCCESS  ((RET_STATUS) 0)
#define FAIL     ((RET_STATUS) -1)


#define SECONDS_PER_MINUTE      60
#define MINUTES_PER_HOUR        60
#define HOURS_PER_DAY           24
#define DAYS_PER_WEEK           7
#define SECONDS_PER_HOUR        (SECONDS_PER_MINUTE * MINUTES_PER_HOUR)
#define SECONDS_PER_DAY         (SECONDS_PER_HOUR * HOURS_PER_DAY)
#define MAX_TIME_OF_DAY         2359        /* one minute before midnight */

#include "STD.h"
#include "taxi_defs.h"
#include "mads_types.h"

#endif
