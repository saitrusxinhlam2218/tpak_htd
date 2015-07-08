/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Tracking_Layer_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2000/03/02 19:44:13 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Tracking_Layer_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Tracking_Layer_db.h,v $	$Revision: 1.2 $"

#ifndef _TL_DB_H
#define _TL_DB_H

#include <isam.h>

#define TL_FLEET_OFFSET        0
#define TL_VEH_NBR_OFFSET      (TL_FLEET_OFFSET + 1)
#define TL_LONG_X_OFFSET       (TL_VEH_NBR_OFFSET + INTSIZE)
#define TL_LAT_Y_OFFSET        (TL_LONG_X_OFFSET + DOUBLESIZE)
#define TL_GPS_TIME_OFFSET     (TL_LAT_Y_OFFSET + DOUBLESIZE)
#define TL_DATE_TIME_OFFSET    (TL_GPS_TIME_OFFSET + LONGSIZE)

#ifdef KEY_SRC

struct keydesc tl_key1 = {
  ISDUPS,
  3,
  { { TL_FLEET_OFFSET, CHARSIZE, CHARTYPE },
      { TL_VEH_NBR_OFFSET, INTSIZE, INTTYPE },
      { TL_DATE_TIME_OFFSET, LONGSIZE, LONGTYPE }
  }
};

#else
extern struct keydesc tl_key1;
#endif

#endif
