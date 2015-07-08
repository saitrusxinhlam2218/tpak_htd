static char sz__FILE__[]="@(#)itm_info.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  itm_info.c; Rel: 6.11.0.0; Get date: 1/22/93 at 10:14:55
* @(#)  Last delta: 4/4/91 at 10:15:10
* @(#)  SCCS File: /taxi/sccs/s.itm_info.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

/****************************************************************************/
/* file itm_info.c 			                                    */
/* routines to request & print out various status and info form itm         */
/* mostly for debugging                                                     */
/* TaxiPak R4                                                               */
/* started 2 July 1990                                                      */
/* last update 13 July 1990                                                  */
/* M. R. Bradley 			                                    */
/****************************************************************************/

#include <memory.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
#include "itm.h"
#include "msg_mmp.h"
#include "language.h"
#include "credit.h"
#include "tmplen.h"

char  credit_card_delimeter = ':';
char  trip_data_delimeter_str[] = "!BCDEFGHIJKLMPQRSTUVWZ%";
extern int itm_flag;
extern char  last_receipt_nbr[];
char  basic_tariff_delimeter_str[] = "ABCDEFGHIJKLM%";
char  trip_tariff_delimeter_str[] = "ABCDEFGHIJZ%";
char  time_tariff_delimeter_str[] = "ABCDEFGHIJZ%";
char  group0_delimeter_str[] = "ABCDEFGLPQRSTUZ%";
char  group1_delimeter_str[] = "ABCDEFGHIJKLPQRSTU%";
char  group2_delimeter_str[] = "DEFGLZ%";
char  car_delimeter_str[] = "ABCDE%";
char  limit_delimeter_str[] = "!A%";

void
get_itm_receipt_nbr(src_ptr, dest_ptr)
   char *src_ptr;
   char *dest_ptr;
{
   char delimeter;
   char get_itm_msg_field();

   if (src_ptr == NULL)
      return;

   delimeter = get_itm_msg_field(src_ptr, dest_ptr, trip_data_delimeter_str);
   while ((delimeter != 'B') && (delimeter != '\0'))
   {
      delimeter = get_itm_msg_field(&src_ptr[strlen(dest_ptr)], dest_ptr, strchr(trip_data_delimeter_str, delimeter) + 1);
   }
}                    /* end get_itm_receipt_nbr*/

void
get_trip_start_time(src_ptr, dest_ptr)
   char *src_ptr;
   char *dest_ptr;
/* get the date time string from the source */
{
 if (src_ptr == NULL)
    return;
 strncpy(dest_ptr, &src_ptr[TRIP_START_INDX], TRIP_TIME_SIZE);
 dest_ptr[TRIP_TIME_SIZE] = '\0';
}                    /* end get_trip_start_time */

void
get_trip_stop_time(src_ptr, dest_ptr)
   char *src_ptr;
   char *dest_ptr;
/* get the date time string from the source */
{
 if (src_ptr == NULL)
    return;
 strncpy(dest_ptr, &src_ptr[TRIP_STOP_INDX], TRIP_TIME_SIZE);
 dest_ptr[TRIP_TIME_SIZE] = '\0';
}                    /* end get_trip_stop_time */

void
get_trip_date(src_ptr, dest_ptr)
   char *src_ptr;
   char *dest_ptr;
/* get the date time string from the source */
{
 if (src_ptr == NULL)
    return;
 strncpy(dest_ptr, &src_ptr[TRIP_DATE_INDX], TRIP_DATE_SIZE);
 dest_ptr[TRIP_DATE_SIZE] = '\0';
}                    /* end get_trip_date */

char
get_itm_msg_field(src_ptr, dest_ptr, delimeter_str)
   char *src_ptr;
   char *dest_ptr;
   char *delimeter_str;
/* get the first field matching on of the delimeters */
/* special case for FIRST_FIELD - no delimeter */
{
   char *ptr_field_end = src_ptr;
   char *ptr_field_start;
   char  delimeter;
   int   field_length;

   delimeter = '\0';
   dest_ptr[0] = '\0';

   if (delimeter_str == NULL)
      return (delimeter);

   if (delimeter_str[0] == NO_DELIMETER)
      ptr_field_start = ptr_field_end;
   else
      ptr_field_start = (char *) strpbrk(ptr_field_end, delimeter_str);

   if (ptr_field_start == NULL)
      return (delimeter);

   if (ptr_field_start[0] == CTRL_START)
      return (delimeter);

   if (delimeter_str[0] == NO_DELIMETER)
      delimeter = NO_DELIMETER;
   else
   {
      delimeter = ptr_field_start[0];
      ptr_field_start++;	/* over the field delimeter */
   }

   ptr_field_end = (char *) strpbrk(ptr_field_start, delimeter_str);
   if (ptr_field_end != ptr_field_start)	/* if not null field */
   {
      ptr_field_end--;		/* end of the data field */
      field_length = ptr_field_end - ptr_field_start + 1;
      strncpy(dest_ptr, ptr_field_start, field_length);
      dest_ptr[field_length] = '\0';
   } else			/* null field */
      dest_ptr[0] = '\0';

   return (delimeter);
}				/* end get_itm_msg_field */

int
get_itm_acct_nbr(text, acct_nbr)
   char *text;
   char *acct_nbr;
/* get the itm credit verification account number */
{
   char *str_ptr;
	 static char str_temp[TMP_STR_LEN];

   if (strlen(text) < ITM_ACCOUNT_FIELD_LEN)
        return (-1);

   strncpy(str_temp, text, ITM_ACCOUNT_FIELD_LEN);

   str_ptr = (char *) strchr(str_temp, credit_card_delimeter);
   if (str_ptr != NULL)
      *str_ptr = '\0';
   else
   {
      str_ptr = (char *) strchr(str_temp, '=');
      if (str_ptr != NULL)
         *str_ptr = '\0';
      else
         str_temp[ITM_ACCOUNT_FIELD_LEN] = '\0';
   }
   strcpy(acct_nbr, str_temp);
   return(0);
}                              /* end get_itm_acct_nbr */

void
get_itm_acct_info(text, acct_info_ptr)
    char * text;
    struct verify_acct_info *acct_info_ptr;
/* get the account infor from the text field of the ITM message */
{
   char *str_ptr;

   get_itm_acct_nbr(text, acct_info_ptr->acct_nbr);
   acct_info_ptr->acct_type = UNKNOWN_ACCT_TYPE;   /* FIX THIS WHEN WE KNWO HOW */

   str_ptr = (char *) strchr(text, credit_card_delimeter);
   if (str_ptr != NULL)
   {
      str_ptr++;
      strncpy(acct_info_ptr->expire_date, str_ptr, EXPIRE_LEN);      /* why isnt it 6 ?? */
      acct_info_ptr->expire_date[EXPIRE_LEN] = '\0';
   }
   else
   {
      str_ptr = (char *) strchr(text, '=');
      acct_info_ptr->expire_date[EXPIRE_LEN] = '\0';
   }
   strncpy(acct_info_ptr->amount, &text[ITM_ACCOUNT_FIELD_LEN], ITM_AMOUNT_LEN);
   acct_info_ptr->amount[ITM_AMOUNT_LEN] = '\0';
}                          /* end get_itm_acct_info */
