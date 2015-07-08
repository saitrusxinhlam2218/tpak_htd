 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  credit.h; Rel: 6.10.0.0; Get date: 2/26/92 at 13:37:55
 * @(#)  Last delta: 1/15/92 at 09:41:01
 * @(#)  SCCS File: /taxi/sccs/s.credit.h
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:credit.h	6.10.0.0"

/* Include file for credit verification.
   Doug Rudoff 6-6-90 */


/* There is the record length in the raw bad account file and the
   record length in the converted files (that are used by the routines
   for credit verification. */

/* Records include 1 char for newline char */
#define H_RAW_RECORD_LEN	33
#define S_RAW_RECORD_LEN	21
#define H_CONVERTED_RECORD_LEN	32   /* HELSINKI */
#define S_CONVERTED_RECORD_LEN	20   /* STOCKHOLM */
#define MAX_CREDIT_RECORD_LEN   99
#define MAX_LEN			100

#define RECORD_CODE_LEN		3
#define WARNING_LEN		2
#define H_ACCOUNT_LEN		19   
#define S_ACCOUNT_LEN		19 
#define ACCOUNT_LEN             19          /* for reporting */
#define MAX_ACCOUNT_LEN         H_ACCOUNT_LEN
#define CR_DATE_LEN		6
#define EXPIRE_LEN              4 
#define AMOUNT_LEN             10
#define CARD_NAME_LEN         256

/* Max string len for word "GOOD" "BAD" or "NOVERIFY" */
#define RESPONSE_LEN		8

/* Helsinki date starts one character after account # in internal format */
#define DATE_START		H_ACCOUNT_LEN + 1

#define MAX_DUPLICATE_ACCOUNTS 20

#define NEW_RECORD		'0'
#define ADD_RECORD		'1'
#define REMOVE_RECORD		'9'

/* Used to find the minimum and maximum dates using a strcmp() */
#define MIN_DATE "000000"
#define MAX_DATE "999999"

#define BACKED_UP_FILE_TYPE	1
#define UPDATE_FILE_TYPE	2
#define NEW_FILE_TYPE		3
#define STOCKHOLM_FILE_TYPE     4
#define HELSINKI_FILE_TYPE      5

#define UPDATE_FILE_CODE	"700"
#define NEW_FILE_CODE		"710"

#define GOOD_ACCOUNT		1
#define BAD_ACCOUNT		2
#define CANT_VERIFY_ACCOUNT	3

#define BACKUP_CREDIT_FILE		"credit_backup.sh"
#define UPDATE_CREDIT_FILE		"credit_update.sh"


struct verify_acct_info{
   char acct_type;
   char acct_nbr[MAX_ACCOUNT_LEN + 1];
   char expire_date[EXPIRE_LEN + 1];
   char amount[AMOUNT_LEN + 1];
   char card_name[CARD_NAME_LEN];
};


