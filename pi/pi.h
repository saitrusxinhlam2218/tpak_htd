 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  pi.h; Rel: 10.1.2.1; Get date: 4/19/95 at 08:45:25
 * @(#)  Last delta: 12/2/94 at 18:14:01
 * @(#)  SCCS File: /taxi/sccs/s.pi.h
 *                                                                           *
 *****************************************************************************/
/* -------------------------------------------------------------------------------
   Copyright 1993 - TaxiCom Copenhagen, Denmark

   System	:	PLANET - LINK
   Filename	:	pi_action.h
   Purpose	:	This module contains the handling of all the PI-functions
			interfacing TaxiPak.

Modification history

Author	Date	Revision	Description
------  ------- --------------- -----------------------------------------------
LG      931125  0.0		Function-paramters/return-values.

------------------------------------------------------------------------------- */

/* Common PI definitions. */
/**************************/

#define ERROR(e)	write_error(__FILE__,__LINE__,e);		/* ERROR-LOG macro. */

#define SWITCH_ON		1	/* Positive value for on_off_switch.					*/
#define SWITCH_OFF		0	/* Negative value for on_off_switch.					*/
#define TRUE			1	
#define FALSE			0
#define PICKUP_ADDR		0	/* Indicates that the pickup-address should be zoned.			*/
#define DEST_ADDR		1	/* Indicates that the destination-address should be zoned.		*/
#define PI_BROADCAST		2	/* Indication of a broadcast (PI_REPORT_EXCEPTIONS).			*/
#define MAX_CONNECTIONS   16 /* Maximum number of connections to PI using the socket     */

/* Exceptions typed are pre-defined for reporting */
/* A selection has been made from all exceptions */
#define NUM_OF_EXCEP_TYPE   32  /* Number of exception-types.           */
#define STATUS_TABLE_MAX	24	/* Maximum number of words in call-status table (all languages). 	*/
#define NUM_OF_EXCEPTIONS	10	/* Number of exceptions (max.) to get.					*/
#define NUM_OF_EXCEP_MESS   99  /* Number of exception-message-types.         */

#define NUM_OF_CALL_STATUS	6	/* Number of call-status for a single language.				*/
#define NUM_OF_CALL_TYPES	5	/* Number of call-types (Passenger, Future etc.)			*/
#define NUM_OF_ATTRIB   8 /* Number of attributes (vehicle/driver).       */

#define API_DISPATCHER_ID 9999

#define FTJ   1
#define EXT_CALL      1
#define NON_EXT_CALL  2


#define MAX_DATA_LEN  65535    /* Maximum general buffer */

/********************************************
 Maximum msg from API is pi_dispatch_ext_msg
 3*call(structs) = 339*3
 9*cisam_cmsg(structs) = 264 *9
 Total = 3393 (bytes)
********************************************/

#define MAX_IN_BUFFER  65535    /* Max message from API */

#define PI_CHAR_TYPE   sizeof(char)      /* Size of CHARacter.             */
#define PI_SHORT_TYPE  sizeof(short)     /* Size of SHORT integer.           */
#define PI_INT_TYPE    sizeof(int)       /* Size of INTeger.             */
#define PI_LONG_TYPE   sizeof(long)      /* Size of LONG integer.            */

/****************************************/
/* ORDER data-block:			*/
/* <LENGTH>,<DATA>			*/
/****************************************/

struct order_struc {
  int data_len;
  char data[ MAX_DATA_LEN ];
  };

typedef struct order_struc order;

/****************************************/
/* RETURN data-block:			*/
/* <LENGTH>,<PI_OK/PI_FAIL,DATA>	*/
/****************************************/

struct return_struc {
  int data_len;
  char data[ MAX_DATA_LEN ];
  };

typedef struct return_struc return_type;

/******************************************************
		 API message structures

		Structures for msgs from client to server

******************************************************/

/***********************
	PI_GENERATE_EXCEPTION
***********************/

typedef struct ges
{
	int nGroup;
	int nNumber;
} GenExcept;


typedef struct gers
{
	short resp;
	int exception;
} GenExceptResp;



/******************************************************

		Structures for processing multi-call,
		call_msg records

******************************************************/

typedef struct cinf
{
	int cisam_call_nbr;
	int nbr_call_msg;
	int call_nbr;
} CallInfo;

#define VALID_CALL    1
#define INVALID_CALL  -1

/************************************************/
/* The 3 basic enumerations:			*/
/*						*/
/* 1) Function-names (types).			*/
/*						*/
/* 2) Call-status.				*/
/*						*/
/* 3) Exception-type.				*/
/************************************************/

/* PI-functions. */
/*****************/

#define PI_UNKNOWN_ORDER    0 /* Order that can't be processed  */
#define PI_ZONE_ADDRESS     1 /* Gets a zone to an address        */
#define PI_DISPATCH_CALL    2 /* Dispatch a zoned call        */
#define PI_CANCEL_CALL      3 /* Cancel a dispatched call         */
#define PI_GET_CALL         4 /* Get current data for a call        */
#define PI_GET_CAR          5 /* Get car connected to a call        */
#define PI_GET_EXCEPTIONS   6 /* Get exception of a certain type right now    */
#define PI_REPORT_EXCEPTIONS 7 /* Get exceptions of a certain type when they occurs  */
#define PI_ACCEPT_EXCEPTION  8 /* Approval or disapproval of an exception    */
#define PI_SEND_MESSAGE     9 /* Send a message to TaxiPak        */
#define PI_GET_MESSAGE      10  /* Get a message from TaxiPak         */
#define PI_ALIVE            11  /* Check if PI is alive         */
#define PI_QUIT             12  /* Disconnect from PI           */
#define PI_DISP_EXT_CALL    13  /* Dispatch zoned calls from client for Samplan */
#define PI_GENERATE_EXCEPTION 14 /* client generates exception */
#define PI_DISPATCH_ACCOUNT_CALL 15
#define PI_MAKE_CALL        16
#define PI_ZONE_INFO        20  /* get zone counters for number of vehicles */
#define PI_DISPATCH_ACCOUNT_CALL2 21
#define PI_PRE_BOOK_INFO    22  /* get info about availability of times for time calls */
#define PI_GET_CALL_MESSAGE 23  /* get the call messages associated with a trip */
#define PI_FLEET_STATUS     24
#define PI_CALL_NBR         25
#define PI_LINE_MGR_ORDER   26
#define PI_CBACK_COUT_ORDER 27
#define PI_EMERG_PICTURE    28
#define PI_RQST_GPS         29
#define PI_ACCOUNT_INFO     30
#define PI_GET_ACCOUNT_CALLS        31
#define PI_GET_ACCOUNT_ADDR 32
#define PI_GET_ACCOUNT_INDV 33
#define PI_GET_CALLH        34
#define PI_UPDATE_CALL      35
#define PI_VEH_STATUS       36
#define PI_CUSTOMER_ZONE    37
#define PI_COORD_TRANSFORM  66

#define PI_LOG_ATTR         99
/* Attribute offsets */
/*********************/

#define   ATTRIB_SLOT_0   0
#define   PASS_TYPE_OFF   0
#define   DELIV_TYPE_OFF  0
#define   TYPEX_TYPE_OFF  0
#define   WAKE_TYPE_OFF   0

#define   ATTRIB_SLOT_1   1
#define   AUTO_TYPE_OFF   1

#define   ATTRIB_SLOT_2   2
#define   TIMEC_TYPE_OFF  2
#define   SUBS_TYPE_OFF   2

#define   ATTRIB_SLOT_3   3
#define   CHRG_TYPE_OFF   3

#define   ATTRIB_SLOT_4   4
#define   MULT_TYPE_OFF   4



/* Block types ( fbyte values ) */
/********************************/

#define   ORDER_BLOCK_TO_API      0
#define   ANSWER_BLOCK_TO_CLIENT  1
#define   ACK_BLOCK_TO_CLIENT     2
#define   NAK_BLOCK_TO_CLIENT     3
#define   ACK_BLOCK_TO_API        4
#define   NAK_BLOCK_TO_API        5
#define   ORDER_BLOCK_TO_CLIENT   6



/* PI-function return error code (PI_OK = 0/PI_FAIL > 0) */
/*********************************************************/

#define PI_OK			0	/* No failure... 					*/
#define PI_INVALID_LEN		1	/* The data-block has an odd length.			*/
#define PI_INVALID_VALUE	2	/* Unable to get values from the data-block.		*/
#define PI_INVALID_FLEET	3	/* The fleet is outside A - H.				*/
#define PI_DB_OP_FAIL     4 /* Database operation fails - API closes down */

/* PI_ZONE_ADDRESS/PI_DISPATCH_CALL: */
/*************************************/

#define PI_INVALID_PRIO		10	/* The trips priority is outside 6 - 63.		*/
#define PI_INVALID_VEH		11	/* # of vehicles are not within 1 - 9.			*/
#define PI_INVALID_CALL_1	12	/* Incorrect call-type: Pass., Deliv., Extra, Wakeup.	*/
#define PI_INVALID_CALL_2	13	/* Incorrect call-type: Autobooker.			*/
#define PI_INVALID_CALL_3	14	/* Incorrect call-type: Timecall, Subscribtion.		*/
#define PI_INVALID_CALL_4	15	/* Incorrect call-type: Charge.				*/
#define PI_INVALID_CALL_5	16	/* Incorrect call-type: Multi-call.			*/
#define PI_INVALID_ADDR_STR	17	/* Invalid streetname (pickup-addr.)			*/
#define PI_INVALID_ADDR_NUM	18	/* Invalid streetnumber (pickup-addr.)			*/
#define PI_INVALID_CITY		19	/* Invalid city-abbreviation (pickup-addr.)		*/
#define PI_INV_VEH_ATTRIB	20	/* One of the vehicle-attributes doesn't match Y/N.	*/
#define PI_INV_DRV_ATTRIB	21	/* One of the driver-attributes doesn't match Y/N.	*/
#define PI_INV_DUE_TIME		22	/* Due-time for future-call is too old.			*/
#define PI_INV_ZONE		23	/* Pickup-zone is not valid.				*/
#define PI_INV_ACCOUNT          24      /* Invalid account number                               */

/* PI_CANCEL_CALL/PI_GET_CALL/PI_GET_CAR: */
/******************************************/

#define PI_NO_CALL		30	/* Call does not exist.					*/

/* PI_GET_EXCEPTIONS: */
/*********************/

#define PI_INV_EXCEP_TYPE	60	/* Exception-type does not exist.			*/
#define PI_INV_EXCEP_MESS	61	/* Exception-message-number is not valid.		*/

/* PI_REPORT_EXCEPTIONS: */
/*************************/

#define PI_INV_SWITCH 		70	/* Exception-switch should either be ON/OFF.		*/

/* PI_ACCEPT_EXCEPTION: */
/************************/

#define PI_INV_EXCEPTION 	80	/* Exception does not exist.				*/
#define PI_INV_APPROVAL		81	/* Incorrect approval-type (Y/N).			*/

/* PI_SEND_MESSAGE: */
/********************/

#define PI_INVALID_GROUP	90	/* Receiver-group didn't exist. 				*/
#define PI_INVALID_RECEIVER	91	/* Receiver didn't exist. 					*/
#define PI_NOT_RECEIVED		92	/* Message was not received.					*/
#define PI_INVALID_MMP_CHAR	93	/* Invalid character for MMP (resets ACC)   */

/* PI_GET_MESSAGE: */
/*******************/

#define PI_NO_MESS		100	/* Message does not exist.					*/

/* PI_ALIVE: */
/*************/


/* PI_QUIT: */
/************/


/* PI_GENERATE_EXCEPTIONS: */
/***************************/
#define PI_INV_EXCP_GROUP   110     /* Group not defined SAMPLAN = 1 */
#define PI_INV_EXCP_NBR     111     /* Exception number range for group (SAMPLAN: 1-15) */


/* PI_DISPATCH_EXT_EXCEPTIONS: */
/*******************************/
#define PI_INV_OFFER_TYPE     120 /* Not in range (1-10)   */
#define PI_INV_DETAIL_TYPE    121 /* Not in range (1-10)   */
#define PI_INV_NBR_CALL_MSGS  122 /* Not in range ( 1-3 )  */
#define PI_INV_FTJ_NBR        123 /* Not in range (0-MaxInt) */

#define PI_INV_CMSG_DEVICE    130 /* Not in range (0-2) */
#define PI_INV_CMSG_MULTI_NUM 131 /* Multi-call # (1-3) */




/* CALL-status. */
/****************/


static char *call_status[] = {
  "PENDING", "UNASSGND", "ASSIGNED", "PICKUP", "COMPLETE", "CANCELD ", 
  "VENTER", "IK-TILD", "TILDELT", "OPSAML", "F[RDIG", "ANNULRT",
  "V[NTAR  ", "EJ TILLD", "TILLDELD", "HTID    ", "KOMPLETT", "AVBEST  ",
  "ODOTTAA", "AVOIN", "VLITETTY", "NOUTO", "VALMIS", "PERUTTUS"
  };

#define PENDING			1	/* The call is a future-call (waiting)		*/
#define UNASSIGNED		2	/* The call is not connected to a taxi yet	*/
#define ASSIGNED		3	/* The taxi is driver to the customer		*/
#define PICKUP			4	/* The taxi-trip is going on			*/
#define COMPLETE		5	/* The taxi-trip has ended			*/
#define CANCELLED		6	/* The call has been cancelled			*/
#define NOEXIST			7	/* The call does not exist			*/


/* MMP message info */
/********************/
#define	FIRST_MMP_CHAR	0x20
#define LAST_MMP_CHAR 0x5f


// Line Manager Pipes
#define CBACK_PIPE  "/usr/taxi/run/line_mgr_cback_pipe"
#define COUT_PIPE "/usr/taxi/run/line_mgr_cout_pipe"

