/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: GPS_private.h,v $
*  @(#)  $Revision: 1.10 $
*  @(#)  $Date: 2000/05/24 14:02:55 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/GPS_private.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _GPS_PRIVATE_H
#define _GPS_PRIVATE_H
#define GPS_DELIMITER  "%P"
#define GPS_MESSAGE_LEN 32
#define MDT_MSG_POST       1001
#define MDT_MSG_UNPOST     1002
#define MDT_MSG_CPOST      1003
#define MDT_MSG_INQUIRY    1004
#define MDT_MSG_SIGNIN     1005
#define MDT_MSG_MSG        1006
#define MDT_MSG_QPOS       1007
#define MDT_MSG_EPOST      1008
#define MDT_MSG_ETA        1009
#define MDT_MSG_NOTRIP     1010
#define MDT_MSG_ACCEPT     1011
#define MDT_MSG_GPS        1012
#define RECV_STATUS_LEN  1
#define TIME_OF_DAY_LEN  6
#define LAT_LEN          10
#define LONG_LEN         11
#define RECV_STATUS_OFFSET  0
#define TIME_OF_DAY_OFFSET  (RECV_STATUS_OFFSET + RECV_STATUS_LEN)
#define LATITUDE_OFFSET     (TIME_OF_DAY_OFFSET + TIME_OF_DAY_LEN)
#define LONGITUDE_OFFSET    (LATITUDE_OFFSET    + LAT_LEN   )
#define  MILLIARCS_PER_DEGREE   3600000.0
/** Specific to KOGE temporarily **/
#define GPS_XMAX     730000.0
#define GPS_XMIN     617000.0
#define GPS_YMAX     6230000.0
#define GPS_YMIN     6040000.0
/* GPS Receiver Status Indicators */
#define GPS_STATE_BAD_ALMANAC      '!'
#define GPS_STATE_INSUFF_SATS      '"'
#define GPS_STATE_ACQUIRING_SATS   '%'
#define GPS_STATE_DIFFERENTIAL     '('
#define GPS_STATE_2D_FIX           '0'
#define GPS_STATE_3D_FIX           '@'
/* Coordinate Systems in Use */
#define LAT_LONG     1
#define UTM          2
#define GPS_TRACE_FILE   "/usr/taxi/run/traces/gps_disp_trace"
/** Define data structure that is used to pass GPS position info
    to GPS_proc and on to PC display system **/
#define MAP_APP_SVC  "finger"
#define MAP_APP_HOST "tpak_mapper"
#define MAP_APP_RQST_TYPE_UPDATE   '0'
#define MAP_APP_RQST_TYPE_SHOW     '1'
#define MAP_APP_RQST_TYPE_EMERG    '2'
#define MAP_APP_RQST_TYPE_CLIENT   '3'
#define MAP_APP_RQST_TYPE_SIGNOFF  '4'
#define MAP_APP_RQST_TYPE_ZONE     '5'
#define MAP_APP_RQST_TYPE_ZOOM_IN  '6'
#define MAP_APP_RQST_TYPE_ZOOM_OUT  '7'
typedef struct _MAP_APP_STRUCT
{
  char  requestType;
  char  vehicleID[10];
  char  Ycoord[10];
  char  Xcoord[10];
  char  driverID[10];
  char  status;
  char  zone[4];
} MAP_APP_STRUCT;
typedef struct _MAP_APP_ZONE_STRUCT
{
  char  requestType;
  char  zone[4];
  char  Ycoord[10];
  char  Xcoord[10];
  char  radius[5];
} MAP_APP_ZONE_STRUCT;
#endif
