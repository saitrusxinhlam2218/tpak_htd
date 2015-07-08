/****************************************************************************
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  msg_mmp.h; Rel: 6.6.0.0; Get date: 2/26/92 at 08:42:31
* @(#)  Last delta: 12/13/90 at 08:49:37
* @(#)  SCCS File: /taxi/sccs/s.msg_mmp.h
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/
#ident "@(#) head:msg_mmp.h	6.6.0.0"

/* Based on R3 version 5.11 */

/* file msg_mmp.h */
/* mmp protocal info */
/* started 26 Jan 90 */
/* last update 13 Dec  1990 */
/* M. R. Bradley */

#define MAX_PROMPT_LENGTH 15

#ifdef MDT420_TEST
#define MAX_DISPLAY_LENGTH 28
#else
#define MAX_DISPLAY_LENGTH 32
#endif

#define MAX_STATUS_LENGTH 16
#define MAX_COMMENT_LINES 2
#define MAX_COMMENT_LENGTH (MAX_COMMENT_LINES * MAX_DISPLAY_LENGTH)
#define MAX_MSG_LINES 10
#define ATTR_FIELD_WIDTH ((MAX_DISPLAY_LENGTH/2) -1)

#define FIELD_END "%D"     
#define EOM "%E"
#define CTRL_START '%'
#define CR_END 'R'
#define EOM_END 'E'
#define CANNED '3'

#define PROMPT      '3'
#define DISPLAY_BUF '2'
#define STATUS      '1'
#define DISPLAY     '0'

#define CR  "%R"

#define SEND_SRCH1	"133000"
#define SEND_SRCH2	"133100"
#define SEND_SRCH3	"133200"
#define SEND_BID 	"133300"
#define SEND_EMERG_BCAST "133F00"
#define TIMER_DWNLD     "134000"
#define DT_DWNLD 	"080000"
#define AUTHORIZE 	"021000"
#define DEAUTHORIZE     "011000"
#define EMERG_ACK 	"000100"
#define ASGN_GROUP1	"0A0100"
#define ITM_MSG         "200000"
#define GPS_MSG         "200100"

#define OFFER_DISPLAY      "221300"
#define ASSIGN_DISPLAY_P1  "222100"
#define ASSIGN_DISPLAY_P3  "222300"
#define TEXT_DISPLAY 	   "220300"
#define STATUS_DISPLAY     "213300"
#define PROMPT_DISPLAY     "230300"
#define CANCEL_DISPLAY     "225300"
#define GPS_RQST           "300100"
#define POLY_DOWNLOAD      "240000"
#define KELA_DETAILS       "229000"

#define FROM_ITM        "231"

/* V03 credit types */
#define CARD_TYPE1_V03     '1'
#define CARD_TYPE_ACCT_V03 '2'
#define CARD_TYPE3_V03     '3'
#define CARD_TYPE4_V03     '4'
#define CARD_TYPE5_V03     '5'
#define CARD_TYPE6_V03     '6'

/* message display priorities */

/* 0 - solid msg light on, 1 beep, buffer message */
#define PRIORITY_0 '0'
/* 1 - blinking msg light, 2 beeps, buffer message */
#define PRIORITY_1 '1'
/* 2 - no message light, 2 beeps, displays message */
#define PRIORITY_2 '2'
/* 3 - same as 2 but will force buffer if it is full */
#define PRIORITY_3 '3'

/* field definitions for forms */
#define EXPIRE_FIELD       5
#define EXPIRE_FIELD_V03   5
#define EXPIRE_FIELD_ITM   4
#define ACCOUNT_TYPE_FIELD 1  /* VO3 */
#define ACCOUNT_V02_FIELD  1
#define ACCOUNT_V03_FIELD 4 
#define ACCOUNT_ITM_FIELD 3 
#define RECEIPT_NUM_FIELD  1
#define METER_STATUS_FIELD 1
#define AMOUNT_1_FIELD_V03 2
#define AMOUNT_2_FIELD_V03 3
#define AMOUNT_1_FIELD_ITM 1
#define AMOUNT_2_FIELD_ITM 2
#define AMOUNT_1_FIELD    2
#define AMOUNT_2_FIELD    3

#define DRV_ID_SIZE       5   /* from the VO2 */

/* pkt id types */
#define NO_PKT_TYPE     'X'
#define OFFER_PKT_TYPE  'O'
#define ASSIGN_PKT_TYPE 'C'
#define MSG_PKT_TYPE    'M'
#define SP_MDT_PKT_TYPE 'S'
#define ITM_MDT_PKT_TYPE 'I'   /* Used for ITM Meter messages that require ACK */
