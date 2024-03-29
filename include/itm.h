/****************************************************************************
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  itm.h; Rel: 6.10.0.0; Get date: 1/13/92 at 10:44:49
* @(#)  Last delta: 4/2/91 at 13:15:05
* @(#)  SCCS File: /taxi/sccs/s.itm.h
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/
/* file itm.h */
/* itm protocal info */
/* started 11 June 90 */
/* last update 19 July 90 */
/* M. R. Bradley */
/* INBOUND MESSAGE TYPES */
#define BOGUS_METER_OFF      0x00
#define TRIP_COMPLETE        0x04
#define SIGN_ON_INFO_VERS1   0x06
#define SIGN_ON_INFO_VERS2   0x08
#define TRIP_DATA            0x0A
#define TRIP_STOP            0x0B
#define NON_SUPPORTED_1      0x1A
#define BREAK_START          0x1B
#define BREAK_STOP           0x1D
#define NOSHOW              0x1E
#define RQ_CARD_VALIDNESS_1  0x40
#define RQ_CARD_VALIDNESS_2  0x41
#define SHIFT_START          0x46
#define SHIFT_STOP           0x47
#define METER_FAULT          0x16F
#define PRINT_TEXT_ACK       0x21
#define METER_ACTION_ACK     0x48
#define TRIPFIFO_DATA   0x60
#define RQ_CARD         0x61
#define NEW_KEYS        0x62
#define RQ_CARDVAL      0x64
#define MULT_TRIPFIFO_DATA 0x65
#define FTJ_VALID       0x66
#define CCP_MSG_FROM_ITM       0x70
#define LICENSE_TEXT          0x2C
#define COMMER_TEXT           0x2D
#define COMMER_TEXT_FTJ       0x2E
#define COMMER_TEXT_PARCEL    0x2F
#define COMMER_TEXT_COMPANY   0x30
#define COMMER_TEXT_ASSEMBLER 0x31
#define CONTROL_REGS         0x05
#define ITM_NO_SHOW     1
#define NON_ITM_NO_SHOW 0
#ifdef DEBUG_R4
#define TX_PARAMETERS        0x03
#define METER_STATUS         0x07
#endif
/* OUTBOUND MESSAGE TYPES */
#define RECEIPT_FEEDBACK     0x00
#define RQ_RECEIPT_DATA      0x08
#define CARD_VALIDNESS_YES   0x40
#define CARD_VALIDNESS_NO    0x41
#define DRIVER_VALIDNESS_YES 0x44
#define DRIVER_VALIDNESS_NO  0x45 
#define RQ_DRIVER_ID	     0x06
#define ACK_FIFO             0x60
#define CARD_VALIDITY        0x61
#define ACK_KEYS             0x62
#define PRINT_TEXT_STOCK     0x21
#define PRINT_TEXT           0x20
#define METER_ACTION         0x48
#define RELAY_OFF            0x1C
#define CCP_MSG_TO_ITM       0x70
#ifdef DEBUG_R4
#define RQ_STATUS            0x07
#define RQ_PARAMETERS        0x04
#define RQ_CONTROL_REGS      0x05
#endif
#define ITM_MSG_START        2
#define ITM_DRV_ID_SIZE      6       /* should be size */
#define ITM_TEXT_START       0        /* start of text field */
#define ITM_TYPE_START       0        /* start of type field within the text field */
#define ITM_SEQ_START        1        /* message sequence number */
#define ITM_AMOUNT_LEN       8        /* the MDT has 10 */
#define ITM_DRIV_START_VERS1       10       
#define ITM_DRV_ID_SIZE_VERS1      8
#define ITM_DRIV_START_VERS2       23
#define ITM_DRV_ID_SIZE_VERS2      8
#define ITM_DRIV_ID_SERIAL_NBR_START 31
#define ITM_DRIV_SHIFT_START       2
#define TMAT_DRV_ID_SIZE   6
#define TMAT_DRV_ID_OFFSET 9
#define NO_DELIMETER '!'
/* trip data - receipt stuff */
#define SHARE_SIZE 2
#define SEQUENCE_SIZE 6              
#define RECEIPT_NBR_SIZE (SHARE_SIZE + SEQUENCE_SIZE)
#define RECEIPT_FDBCK_INTVL 10
#define RECEIPT_FDBCK_DECREMENT  30
#define ITM_ACCOUNT_FIELD_LEN 40 /* length of an account number in a credit validation request */
/* date and time definitions for trip date etc */
#define TRIP_TIME_SIZE 4
#define TRIP_DATE_SIZE 6
#define TRIP_STOP_INDX      4    
#define TRIP_START_INDX     0
#define TRIP_DATE_INDX      8
#define RECEIPT_NBR_START 100      /* one lees than first */
#define RECEIPT_REQUEST_FLAG "FF"  /* in the request receipt message */
#define RECEIPT_REQUEST_FLAG_SIZE 2 /* size of above */
#define DDS_LAT_OFFSET 18
#define DDS_LONG_OFFSET 28
#define DDS_TIME_OFFSET 14
#define IMSI_ITM_SHIFT_START_LEN    67
#define IMSI_ITM_SHIFT_START_BEGIN  48
#define IMSI_ITM_SIGN_ON_INFO_LEN   73
#define IMSI_ITM_SIGN_ON_INFO_BEGIN 54
#define IMSI_LEN 15
