/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Auto_TES.h,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 2002/03/21 14:52:15 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Auto_TES.h,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Auto_TES.h,v $	$Revision: 1.4 $"

#ifndef _AUTOCALL_TES_H
#define _AUTOCALL_TES_H

/** TES Client Requests - Generic **/
#define            TES_STARTED                101
#define            TES_TERMINATE              102
#define            TES_CANCEL_CALL            103
#define            TES_GEN_EXCEPTION          104
#define            TES_UPDATE_CALL                 105
#define            TES_REQ_ACCT_CALL            106
#define            TES_RQST_ADDR                    107
#define            TES_REQ_DETAILED_CALL      108
#define            TES_MONITOR_CALL               109
#define            TES_REQ_CALL_DETAILS        110
#define            TES_REQ_STAND_TAXI         111
#define            TES_REQ_STAND_TAXIv2       112
#define            TES_CLIENT_DISCONNECT      199

/** TES Client Requests - G7      **/
#define            TES_G7_VALIDATE_CLIENT     201
#define            TES_G7_DISPATCH_CALL       202
#define            TES_G7_MONITOR_CALL        203
#define            TES_G7_CONFIRM             204
#define            TES_G7_GET_CALL            205
#define            TES_G7_CALLBACK            206
#define            TES_G7_ZONE_CALL           207
#define            TES_G7_RQST_ETA            208
#define            TES_G7_DISPATCH_DETAILED_CALL 209
#define            TES_G7_RQST_GPUB_ETA       210
#define            TES_G7_RQST_ACCT_ETA       211

/** TES Client Requests - Stockholm **/

/** TES Replies to Clients - generic          **/
#define            TES_OK                     101
#define            TES_KO                     102
#define            TES_CALL_PARAM             103
#define            TES_CALL_NBR               104
#define            TES_ADDR_REPLY             105
#define            TES_CALL_INFO              106
#define            TES_CALL_DETAILS           107
#define            TES_DRV_INFO               108

/** TES Replies to Clients - G7    **/
#define            TES_G7_CLIENT              201
#define            TES_G7_ETA                 202
#define            TES_G7_CALL_INFO           203
#define            TES_G7_NTAXI               204
#define            TES_G7_CUST_USE            205

/** AutoCall Message Defines **/
#define            AC_MSG_SIZE_LEN            3
#define            AC_MSG_TES_LEN             256
typedef struct _AC_MSG
{
  char         MsgSize[ AC_MSG_SIZE_LEN ];
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

#define TES_ACCT_NBR_LEN 18
typedef struct _TES_ACCOUNT_
{
  char         fleet;
  char         account_nbr[ TES_ACCT_NBR_LEN ];
} TES_ACCOUNT;

#define ADDR_STR_NBR_LEN           4
#define ADDR_STR_NAME_LEN          20
#define ADDR_STR_TYPE_LEN          3
#define ADDR_STR_CITY_LEN          3
#define ADDR_ZONE_LEN              3
#define ADDR_GPS_X_LEN             8
#define ADDR_GPS_Y_LEN             8
#define ADDR_APART_LEN             5

typedef struct _TES_ADDR_
{
  char              fleet;
  char              str_nbr[ ADDR_STR_NBR_LEN ];
  char              str_name[ ADDR_STR_NAME_LEN ];
  char              str_type[ ADDR_STR_TYPE_LEN ];
  char              str_city[ ADDR_STR_CITY_LEN ];
  char              zone[ ADDR_ZONE_LEN ];
  char              gps_x[ ADDR_GPS_X_LEN ];
  char              gps_y[ ADDR_GPS_Y_LEN ];
} TES_ADDR;

// DRV info defines
#define             DRV_INFO_ID_LEN           8
#define             DRV_INFO_TELE_LEN        18

typedef struct _DRV_INFO
{
  char              drv_info_id[ DRV_INFO_ID_LEN ];
  char              drv_info_tele[ DRV_INFO_TELE_LEN ];
} DRV_INFO;

/** TES Call Info Defines **/
#define             CI_DELAY_LEN              2
#define             CI_MAKE_MODEL_LEN         10
#define             CI_VEH_COLOR_LEN          10
#define             CI_VEH_NBR_LEN            6

typedef struct _CALL_INFO
{
  char              Delay[ CI_DELAY_LEN ];
  char              MakeModel[ CI_MAKE_MODEL_LEN ];
  char              Color[ CI_VEH_COLOR_LEN ];
  char              VehNbr[ CI_VEH_NBR_LEN ];
} CALL_ACCEPT_INFO;


// Defines for TES_CALL_DETAILS
#define           CD_CALL_NBR                    8
#define           CD_DUE_DATE_LEN            8
#define           CD_DUE_TIME_LEN             5
#define           CD_STATUS                         8
#define           CD_STR_NBR                      4
#define           CD_STR_TYPE                   3
#define           CD_STR_NAME                   20
#define           CD_CITY                             3
#define           CD_ZONE                           3
#define           CD_PHONE                         10
#define           CD_PASSENGER                 14
#define           CD_VEH_NBR                       6
#define           CD_DRV_NBR                       8

#define             CP_FLEET_LEN                   1
#define             CP_DUE_DATE_LEN            6
#define             CP_DUE_TIME_LEN            4
#define             CP_PHONE_NBR_LEN           10
#define             CP_PHONE_EXT_LEN           4
#define             CP_DRV_ATTR_LEN            8
#define             CP_VEH_ATTR_LEN            8
#define             CP_PASS_NAME_LEN           14
#define             CP_NBR_TAXIS_LEN           1
#define             CP_TRIP_COMMENT_LEN        32
#define             CP_CAN_CHARGE_LEN          1
#define             CP_CLIENT_REF_LEN          11
#define             CP_STR_NBR_LEN             4
#define             CP_STR_TYPE_LEN            3
#define             CP_STR_NAME_LEN            20
#define             CP_CITY_NAME_LEN           3
#define             CP_POSTAL_CODE_LEN         5
#define             CP_NBR_LEN                             8
#define             CP_APARTMENT_LEN           5
#define             CP_COMMENTS_LEN            64
#define             CP_VEHATTR_LEN             32
#define             CP_DRVATTR_LEN             32

#define             CP_FLEET_OFFSET                  (0)
#define             CP_DUE_DATE_OFFSET           (CP_FLEET_OFFSET + CP_FLEET_LEN)
#define             CP_DUE_TIME_OFFSET            (CP_DUE_DATE_OFFSET + CP_DUE_DATE_LEN)
#define             CP_STR_NBR_OFFSET              (CP_DUE_TIME_OFFSET + CP_DUE_TIME_LEN)
#define             CP_STR_TYPE_OFFSET            (CP_STR_NBR_OFFSET + CP_STR_NBR_LEN)
#define             CP_STR_NAME_OFFSET           (CP_STR_TYPE_OFFSET + CP_STR_TYPE_LEN)
#define             CP_CITY_NAME_OFFSET          (CP_STR_NAME_OFFSET + CP_STR_NAME_LEN)
#define             CP_PHONE_NBR_OFFSET         (CP_CITY_NAME_OFFSET + CP_CITY_NAME_LEN)
#define             CP_PASS_NAME_OFFSET         (CP_PHONE_NBR_OFFSET + CP_PHONE_NBR_LEN)
#define             CP_APARTMENT_OFFSET         (CP_PASS_NAME_OFFSET + CP_PASS_NAME_LEN)
#define             CP_COMMENTS_OFFSET          (CP_APARTMENT_OFFSET + CP_APARTMENT_LEN)
#define             CP_VEHATTR_OFFSET           (CP_COMMENTS_OFFSET + CP_COMMENTS_LEN)
#define             CP_DRVATTR_OFFSET           (CP_VEHATTR_OFFSET + CP_VEHATTR_LEN)
#define             CP_LENGTH                   (CP_DRVATTR_OFFSET + CP_DRVATTR_LEN)

typedef struct _CALL_PARAM
{
    char              Fleet;
    char              DueDate[ CP_DUE_DATE_LEN + 1 ];
    char              DueTime[ CP_DUE_TIME_LEN + 1 ];
    char              StreetNbr[ CP_STR_NBR_LEN + 1 ];
    char              StreetType[ CP_STR_TYPE_LEN + 1 ];
    char              StreetName[ CP_STR_NAME_LEN + 1 ];
    char              CityName[ CP_CITY_NAME_LEN + 1 ];
    char              PhoneNbr[ CP_PHONE_NBR_LEN + 1];    
    char              PassengerName[ CP_PASS_NAME_LEN + 1 ];
  char              Apartment[ CP_APARTMENT_LEN + 1 ];
  char              Comments[ CP_COMMENTS_LEN + 1 ];
  char              VehAttr[ CP_VEHATTR_LEN + 1 ];
  char              DrvAttr[ CP_DRVATTR_LEN + 1 ];
} CALL_PARAM;

typedef struct _APPROVE_INFO
{
  char              call_nbr[ CP_NBR_LEN ];
  char              yes_no;
} APPROVE_INFO;


/** Error return codes   **/
#define             TES_KO_SYS_ERR                1000
#define             TES_KO_BAD_FORMAT             1001
#define             TES_KO_BAD_CALL_DATA          2000
#define             TES_KO_NO_CALL                2001
#define             TES_KO_UNZONED                2002
#define             TES_KO_CALL_STATE             2003
#define             TES_KO_NO_ETA                 2004
#define             TES_KO_NO_TAXIS               2005
#define             TES_KO_BAD_ACCOUNT_DATA       3000
#define             TES_KO_NO_ACCOUNT             3001
#define             TES_KO_BAD_ADDR               3002
#define             TES_KO_BAD_EXCEPTION_DATA     4000

#endif
