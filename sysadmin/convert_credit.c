/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: convert_credit.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1997/08/09 18:08:05 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/sysadmin/convert_credit.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: convert_credit.c,v 1.2 1997/08/09 18:08:05 taxiadm Exp $";

#include <stdio.h>
#include <strings.h>

#include "path.h"
#include "credit.h"
#include "credit_strdefs.h"

static int
s_format_credit_file(input_file, output_file)
   char *input_file;
   char *output_file;
/* stockholm bad credit file conversion */
{
   FILE *fp_in, *fp_out;
   char  system_call[MAX_LEN];
   char  buf[MAX_LEN];
   int   i, n_records, len;
   int   offset_card_num;
   /* char card_num[ACCOUNT_LEN + 1]; */
   char  card_num[S_CONVERTED_RECORD_LEN + 1];


   if ((fp_in = fopen(input_file, "r")) == NULL)
   {
      fprintf(stderr, "%s %s\n", catgets(CREDIT_catd, 1, 27, "Unable to open input file:"), input_file);
      return (-1);
   }
   if ((fp_out = fopen(output_file, "w")) == NULL)
   {
      fprintf(stderr, "%s %s\n", catgets(CREDIT_catd, 1, 28, "Unable to open output file:"), output_file);
      fclose(fp_in);
      return (-1);
   }
   /* Count the number of records */
   n_records = 0;
   while (fgets(buf, MAX_LEN, fp_in) != NULL)
   {
      n_records++;
   }

   fprintf(fp_out, "#%d\n", n_records);

   /* Go back to start of input file and read past header */
   rewind(fp_in);

   /* Null terminate all the strings in the record */
   card_num[S_CONVERTED_RECORD_LEN] = '\0';

   for (i = 0; i < n_records; i++)
   {
      fgets(buf, MAX_LEN, fp_in);

      len = strlen(buf);
      if (len != S_RAW_RECORD_LEN)
      {
	 fprintf(stderr, "%d: %s\n",
		 i, catgets(CREDIT_catd, 1, 33, "Record # is short, skipping it..."));
	 continue;
      }
      strncpy(card_num, buf, S_CONVERTED_RECORD_LEN - 1);

      fprintf(fp_out, "%s\n", card_num);
   }

   fclose(fp_in);
   fclose(fp_out);

   /*
    * Use the unix utility 'sort' to guarantee order in the output file.
    */
   sprintf(system_call, "sort %s > %s", output_file, TEMP_CREDIT_SORT_FILE);
   system(system_call);
   rename(TEMP_CREDIT_SORT_FILE, output_file);

   return (0);
}				/* end s_format_credit_file */

static int
h_format_credit_file(input_file, output_file)
   char *input_file;
   char *output_file;
/*
 * This routines takes the bad credit card number list provided by
 * LUOTTOKUNTA of Helsinki, and converts into a new format that is easier to
 * use for reading, updating and merging other lists.
 */
{
   FILE *fp_in, *fp_out;
   char  system_call[MAX_LEN];
   char  buf[MAX_LEN];
   int   i, n_records, n_records_file_says_it_has, len;
   int   offset_warning, offset_update_code, offset_card_num;
   int   offset_date, offset_block_code;
   char  record_code[RECORD_CODE_LEN + 1];
   char  warning[WARNING_LEN + 1];
   char  update_code, block_code;
   char  card_num[H_ACCOUNT_LEN + 1];
   char  date[DATE_LEN + 1];


   if ((fp_in = fopen(input_file, "r")) == NULL)
   {
      fprintf(stderr, "%s %s\n", catgets(CREDIT_catd, 1, 27, "Unable to open input file:"), input_file);
      return (-1);
   }
   if ((fp_out = fopen(output_file, "w")) == NULL)
   {
      fprintf(stderr, "%s %s\n", catgets(CREDIT_catd, 1, 28, "Unable to open output file:"), output_file);
      fclose(fp_in);
      return (-1);
   }
   /* Get the file header */
   fgets(buf, MAX_LEN, fp_in);

   /*
    * Start the header of the output file with a '!' so it will stay at the
    * front during a sort.
    */
   fprintf(fp_out, "!%s", buf);

   /* Count the number of records */
   n_records = 0;
   while (fgets(buf, MAX_LEN, fp_in) != NULL)
   {
      /*
       * The record with the number of records will start with a "702" or
       * "712".
       */
      if (buf[2] == '2')
	 break;
      n_records++;
   }

   n_records_file_says_it_has = atoi(buf + 3);

   if (n_records != n_records_file_says_it_has)
   {
      fprintf(stderr, "%s\n",
	      catgets(CREDIT_catd, 1, 29, "Hmm... We have a conflict with the actual number of records"));
      fprintf(stderr, "%s\n",
	      catgets(CREDIT_catd, 1, 30, "in the input file and the number that are actually there."));
      fprintf(stderr, "%s\n",
	      catgets(CREDIT_catd, 1, 31, "I will use the number that are actually there, but you might"));
      fprintf(stderr, "%s\n",
	      catgets(CREDIT_catd, 1, 32, "want to check out the input file to see if it has some errors."));
   }
   /*
    * Print number of records in the file starting with a '#'. When file is
    * sorted '#' will remain after header (starting with '!') and the bad
    * account numbers (starting with a digit).
    */
   fprintf(fp_out, "#%d\n", n_records);

   /* Go back to start of input file and read past header */
   rewind(fp_in);
   fgets(buf, MAX_LEN, fp_in);

   /* Null terminate all the strings in the record */
   record_code[RECORD_CODE_LEN] = '\0';
   warning[WARNING_LEN] = '\0';
   card_num[H_ACCOUNT_LEN] = '\0';
   date[DATE_LEN] = '\0';

   /* Calculate offsets into structure */
   offset_warning = RECORD_CODE_LEN;
   offset_update_code = offset_warning + WARNING_LEN;
   offset_card_num = offset_update_code + 1;
   offset_date = offset_card_num + H_ACCOUNT_LEN;
   offset_block_code = offset_date + DATE_LEN;


   for (i = 0; i < n_records; i++)
   {
      fgets(buf, MAX_LEN, fp_in);

      len = strlen(buf);
      if (len != H_RAW_RECORD_LEN)
      {
	 fprintf(stderr, "%d: %s\n",
		 i, catgets(CREDIT_catd, 1, 33, "Record # is short, skipping it..."));
	 continue;
      }
      /* Now let's split the record up into its component parts */
      strncpy(record_code, buf, RECORD_CODE_LEN);

      strncpy(warning, buf + offset_warning, WARNING_LEN);

      update_code = buf[offset_update_code];

      strncpy(card_num, buf + offset_card_num, H_ACCOUNT_LEN);

      strncpy(date, buf + offset_date, DATE_LEN);

      block_code = buf[offset_block_code];

      fprintf(fp_out, "%s %s %s %c\n", card_num, date, warning, update_code);
   }

   fclose(fp_in);
   fclose(fp_out);

   /*
    * Use the unix utility 'sort' to guarantee order in the output file.
    */
   sprintf(system_call, "sort %s > %s", output_file, TEMP_CREDIT_SORT_FILE);
   system(system_call);
   rename(TEMP_CREDIT_SORT_FILE, output_file);

   return (0);
}				/* end  h_format_credit_file */

int
format_credit_file(file_type, input_file, output_file)
   int   file_type;
   char *input_file;
   char *output_file;
{
   switch (file_type)
   {
   case STOCKHOLM_FILE_TYPE:
      return (s_format_credit_file(input_file, output_file));

   default:
      return (h_format_credit_file(input_file, output_file));
   }				/* end case */
}				/* end format_credit_file */
