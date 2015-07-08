/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Ilink_private.h,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2005/04/21 06:54:20 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Ilink_private.h,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ifndef _ILINK_PRIVATE_H
#define _ILINK_PRIVATE_H
// Define PIPE from dispatch that will be read by TaxiPak's interlink client
#define INTERLINK_PIPE                   "/usr/taxi/run/ilink_pipe"
// XML Tags Defined here
#define TAG_ILINK_OPEN                   "<INTERLINK>"
#define TAG_ILINK_CLOSE                  "</INTERLINK>"
#define TAG_ORDER_OPEN                   "<ORDER>"
#define TAG_ORDER_CLOSE                  "</ORDER>"
#define TAG_DIRECTIVE_OPEN               "<DIRECTIVE>"
#define TAG_DIRECTIVE_CLOSE              "</DIRECTIVE>"
#define TAG_STATUS_OPEN                  "<STATUS>"
#define TAG_STATUS_CLOSE                 "</STATUS>"
#define TAG_ERROR_OPEN                   "<ERROR>"
#define TAG_ERROR_CLOSE                  "</ERROR>"
#define TAG_ORDERID_OPEN                 "<ORDERID>"
#define TAG_ORDERID_CLOSE                "</ORDERID>"
#define TAG_DUEDATETIME_OPEN             "<DUEDATETIME>"
#define TAG_DUEDATETIME_CLOSE            "</DUEDATETIME>"
#define TAG_VEHICLEATTR_OPEN             "<VEHICLEATTR>"
#define TAG_VEHICLEATTR_CLOSE            "</VEHICLEATTR>"
#define TAG_DRIVERATTR_OPEN              "<DRIVERATTR>"
#define TAG_DRIVERATTR_CLOSE             "</DRIVERATTR>"
#define TAG_PICKUPADDRESS_OPEN           "<PICKUPADDRESS>"
#define TAG_PICKUPADDRESS_CLOSE          "</PICKUPADDRESS>"
#define TAG_DESTADDRESS_OPEN             "<DESTADDRESS>"
#define TAG_DESTADDRESS_CLOSE            "</DESTADDRESS>"
#define TAG_CUSTOMER_OPEN                "<CUSTOMER>"
#define TAG_CUSTOMER_CLOSE               "</CUSTOMER>"
#define TAG_STREETNBR_OPEN               "<STREETNBR>"
#define TAG_STREETNBR_CLOSE              "</STREETNBR>"
#define TAG_STRNAME_OPEN                 "<STRNAME>"
#define TAG_STRNAME_CLOSE                "</STRNAME>"
#define TAG_APARTMENT_OPEN               "<APARTMENT>"
#define TAG_APARTMENT_CLOSE              "</APARTMENT>"
#define TAG_CITY_OPEN                    "<CITY>"
#define TAG_CITY_CLOSE                   "</CITY>"
#define TAG_ZONE_OPEN                    "<ZONE>"
#define TAG_ZONE_CLOSE                   "</ZONE>"
#define TAG_PHONE_OPEN                   "<PHONE>"
#define TAG_PHONE_CLOSE                  "</PHONE>"
#define TAG_COMMENT_OPEN                 "<COMMENT>"
#define TAG_COMMENT_CLOSE                "</COMMENT>"
#define TAG_ENTERED_OPEN                 "<ENTERED>"
#define TAG_ENTERED_CLOSE                "</ENTERED>"
#define TAG_TAXI800_ID_OPEN              "<TAXI800_ID>"
#define TAG_TAXI800_ID_CLOSE             "</TAXI800_ID>"
#define TAG_TPAK_ID_OPEN                 "<TPAK_ID>"
#define TAG_TPAK_ID_CLOSE                "</TPAK_ID>"
#define TAG_CANCEL_OPEN                  "<CANCEL>"
#define TAG_CANCEL_CLOSE                 "</CANCEL>"
#define TAG_ERRORMESS_OPEN               "<ERRORMESS>"
#define TAG_ERRORMESS_CLOSE              "</ERRORMESS>"
#define TAG_ACCEPTED_OPEN                "<ACCEPTED>"
#define TAG_ACCEPTED_CLOSE               "</ACCEPTED>"
#define TAG_COMPLETED_OPEN               "<COMPLETED>"
#define TAG_COMPLETED_CLOSE              "</COMPLETED>"
#define TAG_TIMEOUT_OPEN                 "<TIMEOUT>"
#define TAG_TIMEOUT_CLOSE                "</TIMEOUT>"
#define TAG_ORDER_OPEN                   "<ORDER>"
#define TAG_ORDER_CLOSE                  "</ORDER>"
#define TAG_TAXIID_OPEN                  "<TAXIID>"
#define TAG_TAXIID_CLOSE                 "</TAXIID>"
#define TAG_FLEET_OPEN                   "<FLEET>"
#define TAG_FLEET_CLOSE                  "</FLEET>"
#define TAG_VEHNBR_OPEN                  "<VEHNBR>"
#define TAG_VEHNBR_CLOSE                 "</VEHNBR>"
#define TAG_TAXIMOBILEPHONE_OPEN        "<TAXIMOBILEPHONE>"
#define TAG_TAXIMOBILEPHONE_CLOSE       "</TAXIMOBILEPHONE>"
#define TAG_REQUESTORDER_OPEN           "<REQUESTORDER>"
#define TAG_REQUESTORDER_CLOSE          "</REQUESTORDER>"
// Interlink Definitions
#define            ILINK_TRANSFER_ORDER       001
#define            ILINK_CANCEL_ORDER         002
#define            ILINK_CHANGE_STATUS        003
#define            ILINK_ECHO                 004
#define            ILINK_REQUEST_ORDER        005
#define            ILINK_DISP_ACCEPT          100
#define            ILINK_DISP_COMPLETE        101
#define            ILINK_DISP_TRANSFER        102
#define            ILINK_DISP_TIMEOUT         103
#define            ILINK_DISP_CANCEL          104
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
#define            TES_CLIENT_CONNECT         198
#define            TES_CLIENT_DISCONNECT      199
/** TES Replies to Clients - generic          **/
#define            TES_OK                     101
#define            TES_KO                     102
/** AutoCall Message Defines **/
#define            AC_MSG_SIZE_LEN            3
#define            AC_MSG_TES_LEN             1024
typedef struct _AC_MSG
{
  char         MsgSize[ AC_MSG_SIZE_LEN ];
  char         MsgTES[ AC_MSG_TES_LEN ];
} AC_MSG;
/** TES Message Defines **/
#define            TES_TYPE_LEN               3
#define            TES_DATA_LEN               1024
typedef struct _TES_MSG
{
  char         TEStype[ TES_TYPE_LEN ];
  char         TESdata[ TES_DATA_LEN ];
} TES_MSG;
#define             CP_FLEET_LEN                   1
#define             CP_DUE_DATE_LEN            8
#define             CP_DUE_TIME_LEN            6
#define             CP_PHONE_NBR_LEN           18
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
#define             CP_CITY_NAME_LEN           8
#define             CP_POSTAL_CODE_LEN         5
#define             CP_NBR_LEN                             8
#define             CP_APARTMENT_LEN           5
#define             CP_COMMENTS_LEN            64
#define             CP_VEHATTR_LEN             32
#define             CP_DRVATTR_LEN             32
#define             CP_ZONE_LEN                8
#define             ILINK_FLEET_ID             'H'
typedef struct _CALL_PARAM
{
    char              Fleet;
    char              DueDate[ CP_DUE_DATE_LEN + 1 ];
    char              DueTime[ CP_DUE_TIME_LEN + 1 ];
    char              StreetNbr[ CP_STR_NBR_LEN + 1 ];
    char              StreetType[ CP_STR_TYPE_LEN + 1 ];
    char              StreetName[ CP_STR_NAME_LEN + 1 ];
    char              CityName[ CP_CITY_NAME_LEN + 1 ];
  char                Zone[ CP_ZONE_LEN + 1 ];
    char              PhoneNbr[ CP_PHONE_NBR_LEN + 1];    
    char              PassengerName[ CP_PASS_NAME_LEN + 1 ];
  char              Apartment[ CP_APARTMENT_LEN + 1 ];
  char              Comments[ CP_COMMENTS_LEN + 1 ];
  char              VehAttr[ CP_VEHATTR_LEN + 1 ];
  char              DrvAttr[ CP_DRVATTR_LEN + 1 ];
    char              DestStreetNbr[ CP_STR_NBR_LEN + 1 ];
    char              DestStreetType[ CP_STR_TYPE_LEN + 1 ];
    char              DestStreetName[ CP_STR_NAME_LEN + 1 ];
  char              DestApartment[ CP_APARTMENT_LEN + 1 ];
    char              DestCityName[ CP_CITY_NAME_LEN + 1 ];  
} CALL_PARAM;
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
