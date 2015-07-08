/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: GPS_params_db.h,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 2003/01/28 16:38:43 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/GPS_params_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: GPS_params_db.h,v $	$Revision: 1.4 $"

#ifndef _GPS_PARAMS_DB_H
#define _GPS_PARAMS_DB_H

#include <isam.h>

/** Differential Correction Messages **/
#define ONCORE_MSG_TYPE_LEN         4
#define ONCORE_CORRECTION_MSG_LEN   52
#define ONCORE_FIX_POSITION         "@@As"
#define ONCORE_HOLD_POSITION        "@@At"
#define ONCORE_CORRECTION           "@@Bh"
#define ONCORE_DATA_OUTPUT_MSG      "@@Ea"
#define ONCORE_DATA_12CHLOUTPUT_MSG "@@Ha"
#define ONCORE_DATA_12CHSOUTPUT_MSG "@@Hb"
#define ONCORE_POSITION_OUTPUT      "@@Ba"
#define ONCORE_GET_TIME             "@@Aa"
#define ONCORE_GET_DATE             "@@Ac"
#define ONCORE_GET_SATELLITESELECT  "@@Ah"
#define ONCORE_GMT_OFFSET		    "@@Ab"
#define ONCORE_GMT_OFFSET_12CH	    "@@Gb"
#define ONCORE_END_OF_MSG           "\n"
#define MILLIARCSECS_PER_DEGREE     3600000
#define GPS8CH                      1
#define GPS12CH                     2
#define DIFF_SERVER_LEN      32

typedef struct cisam_gps_params
{
  float         diff_latitude;
  float         diff_longitude;
  int           diff_height;
  int           diff_frequency;
  char          diff_server[ DIFF_SERVER_LEN + 1 ];
  int           diff_port;
} GPS_PARAMS;

#endif
