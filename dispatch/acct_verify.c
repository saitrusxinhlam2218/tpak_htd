static char sz__FILE__[]="@(#)acct_verify.c	10.1.2.4(ver) Date Release 1/27/95" ;

/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  acct_verify.c; Rel: 6.24.0.0; Get date: 2/26/92 at 09:32:45
* @(#)  Last delta: 1/2/92 at 14:11:21
* @(#)  SCCS File: /taxi/sccs/s.acct_verify.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

/* Based on R3 version 5.02 */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <strings.h>
#include <ctype.h>
#include <langinfo.h>

#include "path.h"
#include "credit.h"
#include "mads_types.h"
#include "kdt.h"
#include "veh_driv.h"
#include "mad_error.h"

#include "msg_mmp.h"
#include "language.h"
#include "dispatch_strdefs.h"
#define  LIBRARY 1
#include "enhance.h"
#include "switch_ext.h"
#include "tmplen.h"

#include "Account_db.h"
#include "Card_type_db.h"
#include "Bad_credit_db.h"
#include "taxi_db.h"

extern char req_buf[];
extern authorization_nbr;       /* Authorization number used by ENH0002 (sp_misc.c) */

void  write_credit_transaction();
void  remove_leading_zeros();
void get_card_name(char *card_name, char *record);
static void normalize_acct_nbr();

FILE *acct_fp = NULL;
FILE *acct_type_fp = NULL;
extern FILE *credit_fp;
extern char credit_file_name[64];

int   num_accts;
int   start_of_records;
time_t last_modification_time = 0;
static char str_temp[TMP_STR_LEN];

int   bad_acct_file_type;	/* HELSINKI or STOCKHOLM */
int   credit_record_length = 0;
int   account_len = 0;

int
open_bad_acct_file()
{
   struct stat stat_buf;
   char  record[MAX_CREDIT_RECORD_LEN];

   if (acct_fp == NULL)
   {
      acct_fp = fopen(BAD_CREDIT_FILE, "r");
      if (acct_fp == NULL)
      {
         account_len = (H_ACCOUNT_LEN > S_ACCOUNT_LEN) ? H_ACCOUNT_LEN : S_ACCOUNT_LEN;
	 ERROR(' ', "", "Can't fopen bad credit file");
	 return (-1);
      }
			while (fgets(record, MAX_CREDIT_RECORD_LEN, acct_fp) != NULL)
      {
	 if (record[0] == '#')
         {
            bad_acct_file_type = STOCKHOLM_FILE_TYPE;
	    credit_record_length = S_CONVERTED_RECORD_LEN;
	    account_len = S_ACCOUNT_LEN;
	    break;
         }
         if (record[0] == '!')
         {
            bad_acct_file_type = HELSINKI_FILE_TYPE;
	    credit_record_length = H_CONVERTED_RECORD_LEN;
	    account_len = H_ACCOUNT_LEN;
						fgets(record, MAX_CREDIT_RECORD_LEN, acct_fp);
            break;
         }
      }

      /* The number of accounts in the file follows the '#' */
      if (record[0] == '#')
         num_accts = atoi(record + 1);
      else
         num_accts = 0;
      if (num_accts == 0)
      {
         account_len = (H_ACCOUNT_LEN > S_ACCOUNT_LEN) ? H_ACCOUNT_LEN : S_ACCOUNT_LEN;
	 ERROR(' ', "", "errro reading num accts field");
	 return (-1);
      }
      start_of_records = ftell(acct_fp);
      return (0);
   } else
   {
      /* Check to see if account file has been modified */
      /* since last time it was */

      if (fstat(fileno(acct_fp), &stat_buf) == -1)
      {
	 ERROR(' ', "", "Can't fstat bad credit file");
	 fclose(acct_fp);
         acct_fp = NULL;
	 return (-1);
      }
      if (stat_buf.st_ctime != last_modification_time)
      {
         if (last_modification_time == 0)
         {
	     last_modification_time = stat_buf.st_ctime;
             return(0);
         }
         last_modification_time = stat_buf.st_ctime;
	 fclose(acct_fp);
         acct_fp = NULL;
	 return (open_bad_acct_file());
      }
      return (0);               /* most of the time */
   }
}				/* end open_bad_acct_file */

/*********************************************************************
 * Name:    check_acct_type_file(struct verify_acct_info *acct_info) *
 *                                                                   *
 * Purpose: Function checks for presence of card                     *
 *          number prefix in account type file.                      *
 *          If number is found and has a '+' in front                *
 *          the card name is retrieved if ENH0002 is                 *
 *          set.                                                     *
 *                                                                   *
 * acct_info   pointer to information about account                  *
/*********************************************************************/
 int
 check_acct_type_file(struct verify_acct_info *acct_info)
{
	struct stat stat_buf;
	int i,j;
	char  record[MAX_CREDIT_RECORD_LEN];

	acct_type_fp = fopen("/usr/taxi/etc/acct_type_file", "r");
	if (acct_type_fp == NULL)
	{
		ERROR(' ', "", "Can't fopen acct type file");
		return (0);
	}
	while (fgets(record, MAX_CREDIT_RECORD_LEN, acct_type_fp) != NULL)
	{
		j=0;
		while ((record[j]!=' ')&&(record[j]!=NULL)&&(record[j]!='*'))
		{
			j++;
		}
		record[j] = NULL;
		if(strncmp(record+1, acct_info->acct_nbr, strlen(record+1))==0)
		{
			if(record[0]=='-')
			{
				fclose(acct_type_fp);
				if (0)
				  get_card_name(acct_info->card_name, &record[j+1]);
				return(-1);    /* no do not go check the bad account file*/
			}
			else
			{
				fclose(acct_type_fp);
				if (0)
				  get_card_name(acct_info->card_name, &record[j+1]);
				return(0);		/* yes go check bad account file*/
			}
		}
	}
	fclose(acct_type_fp);
	return(-1);		/*assume yes go check bad account file*/
}				/* end check_acct_type_file */

static int
get_acct_rec(val, ptr)
   int   val;
   char *ptr;			/* location for bad account record */
{
   int   offset;
	 static char  err_buf[TMP_STR_LEN];

   offset = val * credit_record_length + start_of_records;

   /* printf("val = %d ofset = %d\n",val, offset); */

   if (fseek(acct_fp, offset, 0L) < 0)
   {
      sprintf(err_buf, "fseek error: errno= %d, offset=%d\n", errno, offset);
      ERROR(' ', "", err_buf);
      ptr[0] = '\0';
      return (-1);
   }
	 if (fgets(ptr, MAX_CREDIT_RECORD_LEN, acct_fp) == NULL)
   {
      sprintf(err_buf, "fgets error: errno=%d, offset=%d\n", errno, offset);
      ERROR(' ', "", err_buf);
      ptr[0] = '\0';
      return (-1);
   }
   return (0);

}				/* end get_acct_rec */

/********************************************/
/* */
/* verify_taxi_acct()                       */
/* is the account number valid - ie good    */
/* taxi accounts only                       */
/* */
/********************************************/

/* NOTE TODO - need to accomodate 19 characters of V03 & 5 characters of V02 */
/* plus what ever the data base does */

static int
verify_taxi_acct(fleet, acct_nbr)
   char  fleet;
   char *acct_nbr;		/* poitnter to inbound messge text */
{
   struct cisam_ac *ac_ptr;
   int   rc;

   /* Open account file */
   if (db(ACCOUNT_FILE_ID, OPEN, &ac_key1, ISINOUT + ISMANULOCK, 0) < 0)
      return (CANT_VERIFY_ACCOUNT);

   /* Wipe out for db call */
   memset(req_buf, '\0', sizeof(struct cisam_ac));

   /* build key for reading account file */
   ac_ptr = (struct cisam_ac *) req_buf;
   ac_ptr->fleet = fleet;
   strcpy(ac_ptr->nbr, acct_nbr);

   /* Try to read the record from the account file */
   if (db(ACCOUNT_FILE_ID, READ_KEYS, &ac_key1, ISEQUAL, 0) < 0)
      rc = BAD_ACCOUNT;
   else
   {
      if (ac_ptr->account_charge_enabled == YES)
	 rc = GOOD_ACCOUNT;
      else
	 rc = BAD_ACCOUNT;
   }

   /* Close account file */
   if (db(ACCOUNT_FILE_ID, CLOSE, &ac_key1, ISEQUAL, 0) < 0)
   {
      ERROR(' ', "", "Error closing account file");
   }
   return (rc);
}				/* end verify_taxi_acct */

verify_taximat_card_nbr( char *card_nbr )
{
  char            *pCardEnd;
  BAD_CREDIT_REC  bad_card;

  if ( *card_nbr != ISO2_LEAD_CHAR )
    return( -1 );
  else
    ++card_nbr;

  if ( ( pCardEnd = strstr( card_nbr, ISO2_SEPARATOR ) ) != NULL )
    *pCardEnd = '\0';

  strcpy( bad_card.card_nbr, card_nbr );

  if ( db_read_key( BAD_CREDIT_FILE_ID, &bad_card, &bc_key1, ISEQUAL ) == SUCCESS )
    return( -1 );
  else
    return( 1 );
}

verify_taximat_card_type( char *card_nbr, char *pCardDesc )
{
  char            card_type_buf[16];
  char            num_compare[ CARD_TYPE_PREFIX_LEN + 1 ];
#ifndef __alpha
  char            tmp_card_nbr[ 64 ];
#endif
  char            *pSeparator = NULL, *pTmpCard;
  CARD_TYPE_REC   card_type;
  int             rc = 0, i;
  int             reject_code = CARD_PREFIX_TEST_FAIL;

  bzero( &card_type, sizeof(CARD_TYPE_REC) );
  bzero( num_compare, sizeof(num_compare) );

  if ( card_nbr == NULL )
    return( -1 );
  
  if ( *card_nbr != ISO2_LEAD_CHAR )
    return( -1 );
  else
    ++card_nbr;


#ifndef __alpha
  bzero( tmp_card_nbr, sizeof(tmp_card_nbr) );
  strcpy( tmp_card_nbr, card_nbr );
  pTmpCard = &tmp_card_nbr[0];
#else
  pTmpCard = strdup( card_nbr );
#endif
  pSeparator = (char *)strstr( pTmpCard, ISO2_SEPARATOR );

  if ( pSeparator != NULL )
    *pSeparator = '\0';    
  
  rc = db_read_key( CARD_TYPE_FILE_ID, &card_type, &ct_key1, ISFIRST );
  while ( rc == 0 )
    {
      bzero( num_compare, sizeof(num_compare) );
      /** check low end of range **/
      strncpy( num_compare, pTmpCard, strlen(card_type.card_type_high) );

      /* pad low prefix at the end so low and high are same length */
      for ( i = strlen(card_type.card_type_low); i <= strlen(card_type.card_type_high); i++ )
	  card_type.card_type_low[i] = '0';

      card_type.card_type_low[strlen(card_type.card_type_high)] = '\0';
      num_compare[strlen(card_type.card_type_high)] = '\0';
      
      if ( strncmp( num_compare, card_type.card_type_low, strlen( card_type.card_type_low ) ) >= 0 )
	{
	  /** check high end of range **/
	  strncpy( num_compare, pTmpCard, strlen(card_type.card_type_high) );
	  num_compare[strlen(card_type.card_type_high)] = '\0';
	  if ( strncmp( num_compare, card_type.card_type_high, strlen( card_type.card_type_high ) ) <= 0 )
	    {
	      strcpy( pCardDesc, card_type.comment );
	      if ( card_type.accept_reject != YES )
		{
#ifdef __alpha
		  free( pTmpCard );
#endif
		  return( CARD_PREFIX_TEST_FAIL );
		}
	      else
		{
		  if ( card_type.service_index > 0 )
		    {
		      pSeparator = (char *)strstr( card_nbr, ISO2_SEPARATOR );
		      if ( pSeparator == NULL )
			{
#ifdef __alpha
			  free( pTmpCard );
#endif
			  return( CARD_SVC_CODE_TEST_FAIL );
			}
		      if ( strncmp( card_type.service_nbr, ( pSeparator + card_type.service_index ),
				   strlen( card_type.service_nbr ) ) )
			{
			  if ( abs(CARD_SVC_CODE_TEST_FAIL) > abs(reject_code) )
			    reject_code = CARD_SVC_CODE_TEST_FAIL;
			}
		      else
			{
			  if ( card_type.type_index > 0 )
			    {
			      if ( strncmp( card_type.type_nbr, ( pSeparator + card_type.type_index ),
					   strlen( card_type.type_nbr ) ) )
				{
				  if ( abs(CARD_TYPE_TEST_FAIL) > abs(reject_code) )
				    reject_code = CARD_TYPE_TEST_FAIL;
				}
			      else
				{
				  if ( card_type.check_mod == YES )
				    {
#ifdef __alpha
				      free( pTmpCard );
#endif
				      return( verify_card_type_mod10( card_nbr ) );
				    }
				  else
				    {
#ifdef __alpha
				      free( pTmpCard );
#endif
				      return( CARD_TYPE_TEST_SUCCESS );
				    }
				}
			    }
			  else /* good range and service code and no card type defined */
			    {
			      if ( card_type.check_mod == YES )
				{
#ifdef __alpha
				  free( pTmpCard );
#endif
				  return( verify_card_type_mod10( card_nbr ) );
				}
			      else
				{
#ifdef __alpha
				  free( pTmpCard );
#endif
				  return( CARD_TYPE_TEST_SUCCESS );
				}
			    }
			}
		    }
		  else /* good range and no service index defined */
		    {
		      if ( card_type.check_mod == YES )
			{
#ifdef __alpha
			  free( pTmpCard );
#endif
			  return( verify_card_type_mod10( card_nbr ) );
			}
		      else
			{
#ifdef __alpha
			  free( pTmpCard );
#endif
			  return( CARD_TYPE_TEST_SUCCESS );
			}
		    }
		}
	    }
	}
      rc = db_read_key( CARD_TYPE_FILE_ID, &card_type, &ct_key1, ISNEXT );
    }
#ifdef __alpha
      free( pTmpCard );
#endif
  return( reject_code );
  
}
  
/********************************************/
/* */
/* verify_card_acct()                       */
/* is the account number valid - ie NOT bad */
/* non taxi accounts only                   */
/* */
/********************************************/

static int
verify_card_acct(card_type, acct_nbr)
   char  card_type;		/* not presently used */
   char *acct_nbr;		/* pointer to inbound messge text */
{
	 char  acct_buf[MAX_CREDIT_RECORD_LEN];
   int   lo = 0;
   int   hi;
   int   mid;
   int   str_val;
   char  given_account[MAX_CREDIT_RECORD_LEN + 1];
   int   ii;
   int   acct_len;

   if (open_bad_acct_file() == -1)
      return (CANT_VERIFY_ACCOUNT);

   if (num_accts == 0)
      return (CANT_VERIFY_ACCOUNT);

   if (try_to_lock_bad_credit_file() == -1)
   {
    /*fclose(acct_fp); */
      return (CANT_VERIFY_ACCOUNT);
   }
   hi = num_accts - 1;

   /* pad the account length with leading zeroes if necessary */
   given_account[0] = '\0';
   acct_len = strlen(acct_nbr);
   if (acct_len < account_len)
      for (ii = 0; ii < (account_len - acct_len); ii++)
	 strcat(given_account, "0");
   strcat(given_account, acct_nbr);

   /* Do a binary search of account file */
   while (mid = (lo + hi) / 2, lo <= hi)
   {
      if (get_acct_rec(mid, acct_buf) == -1)
      {
         /*fclose(acct_fp); */
	 unlock_credit_file();
	 return (CANT_VERIFY_ACCOUNT);	/* probable file problem */
      }
      str_val = strncmp(acct_buf, given_account, account_len);

      if (str_val > 0)
	 hi = mid - 1;
      else if (str_val < 0)
	 lo = mid + 1;
      else
      {
	 if (date_has_passed(acct_buf))
	 {
	    /* Account is no good */
	    /* fclose(acct_fp); */
	    unlock_credit_file();
	    return (BAD_ACCOUNT);
	 } else
	 {
	    /* Account is still good */
	    /* fclose(acct_fp); */
	    unlock_credit_file();
	    return (GOOD_ACCOUNT);
	 }
      }
   }

   /* fclose(acct_fp); */
   unlock_credit_file();

   /* Couldn't find the account #, assume it is valid */
   return (GOOD_ACCOUNT);

}				/* end verify_card_acct */

int
verify_acct(mdt_type, veh_ptr, acct_type, msg)
   int   mdt_type;
   struct veh_driv *veh_ptr;
   char  acct_type;
   char *msg;
{
   int   ii, diff, size;
   char  account_number[MAX_CREDIT_RECORD_LEN];
   struct verify_acct_info acct_info;
   int   rc;


   switch (mdt_type)
   {
   case MDT_ITM:

      if (get_itm_acct_info(msg, &acct_info) == -1)
      {
           /* probably wrong term type */
           sprintf(str_temp,"possible term type error, veh # %d", veh_ptr->veh_nbr);
           ERROR(' ',"", str_temp);
           rc = CANT_VERIFY_ACCOUNT;
           break;
      }

      if (bad_acct_file_type == STOCKHOLM_FILE_TYPE)
      {
	 if (check_acct_type_file(&acct_info) == -1)
	 {
	    rc = BAD_ACCOUNT; 
	    break;
	 }
      }

      normalize_acct_nbr(acct_info.acct_nbr);
      rc = verify_card_acct(acct_info.acct_type, acct_info.acct_nbr);
      /* do something about the receipt */
      break;

   case MDT_V02:
      acct_info.acct_type = '1' + (acct_type - 'A');	/* normalize to integers */
      acct_info.amount[0] = '\0';
      acct_info.expire_date[0] = '\0';
      size = get_mmp_msg_field(msg, acct_info.acct_nbr, ACCOUNT_V02_FIELD);
      
      /* quickey test to see if termianl type wrong - really V03 */
      /* kinda crude - make better later */
      /* too much hard coding */
      if (size <= 1)
      {
           sprintf(str_temp,"possible term type error, veh # %d", veh_ptr->veh_nbr);
           ERROR(' ',"", str_temp);
           rc = CANT_VERIFY_ACCOUNT;
           break;
      }

#ifdef R4
      if (size == 0)
	 fprintf(stderr, "BAD V02 ACCT FIELD ");
#endif
      if (acct_type == LF_TAXI_ACCT)
	 rc = verify_taxi_acct(veh_ptr->fleet, acct_info.acct_nbr);
      else
      {
	 normalize_acct_nbr(acct_info.acct_nbr);
	 rc = verify_card_acct(acct_info.acct_type, acct_info.acct_nbr);
      }
      break;

   case MDT_V03:
      acct_info.acct_type = acct_type;
      size = get_mmp_msg_field(msg, acct_info.acct_nbr, ACCOUNT_V03_FIELD);
      if (size == 0)
      {
         if (strlen(msg) > 40)
            msg[40] = '\0';
	 sprintf(str_temp, "No field %d in message %s\n", 4, msg); 
         ERROR(' ',"",str_temp);
	 return (FALSE);
      }
      if (size > account_len)
      {
         sprintf(str_temp,"Illegal account number %s", acct_info.acct_nbr);
         ERROR(' ',"",str_temp);
	 return (FALSE);
      }
      size = get_mmp_msg_field(msg, str_temp, EXPIRE_FIELD_V03);
      str_temp[EXPIRE_LEN] = '\0';	/* its too long coming in  at 6
					 * instead of 4 */
      strcpy(acct_info.expire_date, str_temp);
      size = get_mmp_msg_field(msg, acct_info.amount, AMOUNT_1_FIELD_V03);
      size = get_mmp_msg_field(msg, str_temp, AMOUNT_2_FIELD_V03);

      /* while Vo3 has tw fields - ITM has only one */
      strcat(acct_info.amount, str_temp);

      if (acct_type == CARD_TYPE_ACCT_V03)
	 rc = verify_taxi_acct(veh_ptr->fleet, acct_info.acct_nbr);
      else
      {
	 if (bad_acct_file_type == STOCKHOLM_FILE_TYPE)
         {
	    if (check_acct_type_file(&acct_info) == -1)
	    {
  	        rc = BAD_ACCOUNT;
		break;
	    }
	 }
	 normalize_acct_nbr(acct_info.acct_nbr);
	 rc = verify_card_acct(acct_info.acct_type, acct_info.acct_nbr);
      }
      break;
   default:
       sprintf(str_temp,"Illegal mdt_type %d", mdt_type);
       ERROR(' ',"", str_temp);
      return (FALSE);
   }				/* end case */
   write_credit_transaction(veh_ptr, &acct_info, rc);
   return (rc);
}				/* end verify_acct */

int
date_has_passed(acct_buf)
   char *acct_buf;
{
   time_t  nowtime;
   struct tm *tm_ptr;
   char  today_date[10];
   char  card_date[10];


   if (bad_acct_file_type != HELSINKI_FILE_TYPE)
      return (TRUE);

   nowtime = time(NULL);
   tm_ptr = localtime(&nowtime);
   sprintf(today_date,
	   "%02d%02d%02d",
	   tm_ptr->tm_year % 100, tm_ptr->tm_mon + 1, tm_ptr->tm_mday);
   /* printf("this date - %s\n", today_date); */

   strncpy(card_date, acct_buf + DATE_START, DATE_LEN);
   card_date[DATE_LEN] = '\0';

   /* printf("card_date %s\n", card_date); */

   if (strcmp(today_date, card_date) < 0)
      return (FALSE);
   else
      return (TRUE);

}				/* end date_has_passed */

void
write_credit_transaction(veh_ptr, acct_info, response)
   struct veh_driv *veh_ptr;
   struct verify_acct_info *acct_info;
   int   response;
{
   time_t  nowtime;
   struct tm *tm_ptr;
   char  present_date[10];
   char  present_time[10];
   char  response_str[RESPONSE_LEN + 1];
   char  record[133];
   char *format_amount_str();

   if (response == GOOD_ACCOUNT)
   {
      strncpy(response_str,
	      catgets(DISPATCH_catd, DISPATCH_SET, DISPATCH_1, "GOOD"),
	      RESPONSE_LEN);
      response_str[RESPONSE_LEN] = '\0';
   } else if (response == BAD_ACCOUNT)
   {
      strncpy(response_str,
	      catgets(DISPATCH_catd, DISPATCH_SET, DISPATCH_2, "BAD"),
	      RESPONSE_LEN);
      response_str[RESPONSE_LEN] = '\0';
   } else
   {
      strncpy(response_str,
	      catgets(DISPATCH_catd, DISPATCH_SET, DISPATCH_3, "NOVERIFY"),
	      RESPONSE_LEN);
      response_str[RESPONSE_LEN] = '\0';
   }

   nowtime = time(NULL);
   if (0)
     authorization_nbr = nowtime;
   tm_ptr = localtime(&nowtime);

   if (DATE_FORMAT == DAY_FIRST_IN_DATE)
      sprintf(present_date,
	      "%02d%c%02d%c%02d",
	      tm_ptr->tm_mday, DATE_SEPERATOR,
	      tm_ptr->tm_mon + 1, DATE_SEPERATOR, tm_ptr->tm_year % 100);
   else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
      sprintf(present_date,
	      "%02d%c%02d%c%02d",
	      tm_ptr->tm_year % 100, DATE_SEPERATOR,
	      tm_ptr->tm_mon + 1, DATE_SEPERATOR, tm_ptr->tm_mday);
   else
      sprintf(present_date,
	      "%02d%c%02d%c%02d",
	      tm_ptr->tm_mon + 1, DATE_SEPERATOR,
	      tm_ptr->tm_mday, DATE_SEPERATOR, tm_ptr->tm_year % 100);

   sprintf(present_time, "%02d:%02d", tm_ptr->tm_hour, tm_ptr->tm_min);

   if (account_len > ACCOUNT_LEN)
      /* hope we only cut off zeros */
      strcpy(str_temp, &acct_info->acct_nbr[account_len - ACCOUNT_LEN]);
   else
      strcpy(str_temp, acct_info->acct_nbr);

   sprintf(record, "%ld %s %s   %c   %*s %*s %s %-*s %*d %*d %10d\n",
	   nowtime, present_date, present_time, acct_info->acct_type,
           ACCOUNT_LEN, str_temp,
	   EXPIRE_LEN, acct_info->expire_date,
	   format_amount_str(acct_info->amount),
	   RESPONSE_LEN, response_str,
	   DRV_ID_SIZE, veh_ptr->driver_id,
	   MID_SIZE, veh_ptr->veh_nbr,
	   veh_ptr->call_nbr);

   /* check to see if it is time to close the current archive   */
   chk_midnite();

   if (credit_fp == NULL)
   {
      if ((credit_fp = fopen(CREDIT_TRANSACTION_FILE, "a")) == NULL)
      {
	 ERROR(' ', "", "Can't open CREDIT_TRANSACTION_FILE");
	 return;
      }
   }
   /* seek to the end of the file before doing the write  */
   /* This may seem like a stupid statement but it allows */
   /* the file to be zero by calling truncate on the file */
   /* Otherwise when the purge routines are run the file  */
   /* would have to be closed.                            */
   fseek(credit_fp, 0L, 2);

   fprintf(credit_fp, "%s", record); /* Added format string to prevent %-characters from being interpreted. TC */
   strcpy(req_buf, record);

#ifdef DEBUG_R4_LOTS
   fprintf(stderr, "TRANSACTION: %s\n", record);
#endif

   fflush(credit_fp);
}				/* end write_credit_transaction */

char
get_acct_type(mdt_type, msg)
   int   mdt_type;
   MSG_QUEUE_HDR *msg;
/* determine what kind of account we have */
{
   int   size;

   switch (mdt_type)
   {
   case MDT_ITM:
      return (UNKNOWN_ACCT_TYPE);
   case MDT_V02:
      return (msg->p_fields[P2]);
   case MDT_V03:
      return (UNKNOWN_ACCT_TYPE);
   }
}				/* end get_acct_type */

static void
normalize_acct_nbr(acct_nbr)
   char *acct_nbr;
{
   int   diff;
   int   size;
   int   ii;
   char *ptr;

   size = strlen(acct_nbr);

   /* trailing blaks are discarded */
   ptr = &acct_nbr[size - 1];
   while (*ptr == ' ')
   {
      *ptr = '\0';
      ptr--;
   }

   /* leading blanks are 0 */
   ptr = acct_nbr;
   while (*ptr == ' ')
   {
      *ptr = '0';
      ptr++;
   }

   size = strlen(acct_nbr);
   diff = account_len - size;
   strcpy(str_temp, acct_nbr);
   if (diff == 0)
      return;

   /* not real sure we need all this - MRB */
   for (ii = 0; ii < size; ii++)
   {
      /* imbedded blanks */
      if (str_temp[ii] == ' ')
	 acct_nbr[ii + diff] = '0';
      else if (!isdigit(str_temp[ii]))
	 acct_nbr[ii + diff] = 'X';	/* for debug */
      else
	 acct_nbr[ii + diff] = str_temp[ii];
   }				/* end for */
   if (diff)
      for (ii = 0; ii < diff; ii++)
	 acct_nbr[ii] = '0';	/* leading zeros */
   acct_nbr[account_len] = '\0';
}				/* end normalize_acct_nbr */

#define AMOUNT_WIDTH (AMOUNT_LEN + 10)
char  formatted_amount[AMOUNT_WIDTH + 1];

char *
format_amount_str(amount_str)
   char *amount_str;
/* format the charge amount string */
{
   char *currency_symbol;
   char *radixchar;
   int   size;
   int   width;
   int   ii;
   char  fraction_str[3];
   char *temp_ptr;


   if ((currency_symbol = nl_langinfo(CRNCYSTR)) == NULL)
      currency_symbol = " ";

   if ((radixchar = nl_langinfo(RADIXCHAR)) == NULL)
      radixchar = ".";

   size = strlen(amount_str);

   width = strlen(currency_symbol) + strlen(radixchar) + AMOUNT_LEN;

   if ((size == 0) || (size > AMOUNT_LEN))	/* probably a V02 or error */
   {
      for (ii = 0; ii < width; ii++)
	 formatted_amount[ii] = ' ';
      formatted_amount[width] = '\0';
      return (formatted_amount);
   }
   if (size == 1)		/* could happen ? */
   {
      fraction_str[0] = '0';
      fraction_str[1] = amount_str[0];
      amount_str[0] = '\0';
   } else
   {
      fraction_str[0] = amount_str[size - 2];
      fraction_str[1] = amount_str[size - 1];
      amount_str[size - 2] = '\0';
   }
   fraction_str[2] = '\0';

   /* leading blanks and zeros */
   while (*amount_str == ' ' || *amount_str == '0')
      amount_str++;

   /* trailing blanks */
   temp_ptr = amount_str + strlen(amount_str) - 1;
   while (*temp_ptr == ' ')
      temp_ptr--;
   *(temp_ptr + 1) = '\0';

   /* imbedded blanks */
   temp_ptr = amount_str;
   while (*temp_ptr != '\0')
   {
      if (*temp_ptr == ' ')
	 *temp_ptr = '0';
      temp_ptr++;
   }

   /* blanks in the fractional part */
   for (ii = 0; ii < 2; ii++)
      if (fraction_str[ii] == ' ')
	 fraction_str[ii] = '0';

#ifdef SCANDINAVIA
   sprintf(formatted_amount, "%*s%s%s%s",
	   width - strlen(radixchar) - strlen(currency_symbol) - 2,
	   amount_str,
	   radixchar,
	   fraction_str,
	   currency_symbol);
#else
   size = strlen(amount_str);	/* need to recalculate */
   sprintf(formatted_amount, "%*s%s%s%s",
	   width - size - strlen(radixchar) - 2,
	   currency_symbol,
	   amount_str,
	   radixchar,
	   fraction_str);
#endif

   return (formatted_amount);
}				/* end format_amount_str */


/*******************************************************************************************
 * Name: int check_card_validity(veh_driv veh_ptr, char *szCardMessage, char *szCardName)  *
 * Type:                                                                                   *
 *                                                                                         *
 * Purpose: Verify credit card using SamPlan ENH0002                                       *
 *                                                                                         *
 *                                                                                         *
 * Returnvalue: Account nbr OK:       0                                                    *
 *              Account nbr NOT OK : -1                                                    *
 *******************************************************************************************/
int check_card_validity(struct veh_driv *veh_ptr, char *szCardMessage, char *szCardName)
{
  char str_tmp[512];
  char *str_ptr, *tmp_ptr;
  int ret,rc;
  struct verify_acct_info acct_info;

  /***************************
   * Retrieve account number *
   ***************************/
  strcpy(str_tmp,szCardMessage);
  
  if ( veh_ptr->term_type == MDT_V03_ITM_VERS2 )
    str_ptr = strchr(&str_tmp[2],':'); /* find first occurrence of ':' after key and index fields */
  else
    str_ptr = strchr(&str_tmp[4],':');
  
  if ( str_ptr == NULL )
    return( -1 );
  str_ptr++;
  tmp_ptr = strchr( str_ptr, '=' );
  if ( tmp_ptr == NULL )
    return( -1 );
  strcpy(tmp_ptr,"");    /* find end of credit acct as an '='-character */

  /***************************
   * Initialize account info *
   ***************************/
  acct_info.acct_type = UNKNOWN_ACCT_TYPE;
  acct_info.amount[0] = '\0';
  acct_info.expire_date[0] = '\0';
  acct_info.card_name[0] = '\0';
  strcpy(acct_info.acct_nbr,str_ptr);

  /**********************
   * Check account type *
   **********************/
  ret = check_acct_type_file(&acct_info);
  if (ret )
  {
    write_credit_transaction(veh_ptr, &acct_info, ret);
    strcpy(szCardName,acct_info.card_name);
/*    printf("Bad credit type\n"); */
    return(-1);
  }

  /************************************************
   * Check account number against bad_credit_file *
   ************************************************/
  normalize_acct_nbr(str_ptr);
  rc = verify_card_acct(UNKNOWN_ACCT_TYPE, str_ptr);
  if (rc == BAD_ACCOUNT)
  {
    write_credit_transaction(veh_ptr, &acct_info, rc);
    strcpy(szCardName,acct_info.card_name);
/*    printf("Bad credit account\n"); */
    return(-1);
  }

  strcpy(szCardName,acct_info.card_name);
  write_credit_transaction(veh_ptr, &acct_info, rc);
  return (0);

} /* check_card_validity */


/**************************************************************************
 * Name: void get_card_name(char *card_name, char *record)                *
 * Type:                                                                  *
 *                                                                        *
 * Purpose: Get the name of the credit card from the information          *
 *          retrieved from the account_type file                          *
 *                                                                        *
 *                                                                        *
 * Returnvalue: NONE                                                      *
 **************************************************************************/
void get_card_name(char *card_name, char *record)
{

  char *str_ptr,*dum_ptr;
  int j;

  /***********************************
   * Check if card name present      *
   ***********************************/
  if ((str_ptr = (char *)strchr(record,'#')) == NULL)
  {
    /* set card_name to Unknown */
    strcpy(card_name,catgets(DISPATCH_catd, DISPATCH_SET, DISPATCH_4, "UNKNOWN"));

    return;
  }
  str_ptr += 2;

  if ((dum_ptr = (char *)strchr(str_ptr,'#')) == NULL)
  {
    /* No comment found */
    j = strlen(str_ptr);
    while(((*(str_ptr+j) == ' ') || (*(str_ptr+j) == '\t')) && (j != 0))
      j--;
    if (j != 0)
      *(str_ptr + j + 1) = NULL;
  }
  else
  {
    /* Comment found */
    j = 1;
    while (((*(dum_ptr-j) == ' ') || (*(dum_ptr-j) == '\t')) && (str_ptr <= dum_ptr-j))
      j++;
    *(dum_ptr - j + 1) = NULL;

  }
  strcpy(card_name,str_ptr);
/*  j = 0;
  while (card_name[j] != NULL)
  {
    card_name[j] = Toupper(card_name[j]);
    j++;
  } */
} /* end get_card_name */


static int mod10_wts[] = {2, 1, 0};

verify_card_type_mod10( char *card_nbr )
{
  int  weight, sum = 0,number_length,i, term;
  char number_string[64];
  char *pSeparator = NULL;

  weight = 2;
  strcpy( number_string, card_nbr );
  pSeparator = (char *)strstr( number_string, ISO2_SEPARATOR );
  if ( pSeparator != NULL )
    *pSeparator = '\0';
  number_length = strlen(number_string);
    
  for ( i = 0; i <= number_length - 1; i++ )
    {
      term = number_string[i] - '0';

      if ( term < 0 || term > 9 )
	continue;

      if ( term != 0 )
	{
	  term = (term * (weight));
	  if ( term < 10 )
	    sum += term;
	  else
	    sum = sum + 1 + (term - 10);
	}
      
      if ( weight == 2 )
	weight = 1;
      else
	weight = 2;

    }

  if ( sum % 10 == 0 )
    return( CARD_TYPE_TEST_SUCCESS );
  else
    return( CARD_TYPE_MODULUS_FAIL );
}
