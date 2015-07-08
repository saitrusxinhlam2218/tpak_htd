/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: update_credit.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1997/08/09 18:08:25 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/sysadmin/update_credit.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: update_credit.c,v 1.2 1997/08/09 18:08:25 taxiadm Exp $";

/* update_credit.c
   Created 6-6-90 Doug Rudoff */

#include <stdio.h>
#include <strings.h>

#include "path.h"
#include "credit.h"
#include "credit_strdefs.h"


update_credit_file(credit_file, update_file)
     char *credit_file;
     char *update_file;
{
  FILE *fp_in, *fp_out;
  char system_call[MAX_LEN];
  char record[MAX_LEN];
  char account[H_ACCOUNT_LEN + 1];
  char date[DATE_LEN + 1];
  char warning[WARNING_LEN + 1];
  char update_code;
  struct record_struct
    {
      char record[H_CONVERTED_RECORD_LEN + 1];
      char account[H_ACCOUNT_LEN + 1];
      char date[DATE_LEN + 1];
      char warning[WARNING_LEN + 1];
      char update_code;
    } records[MAX_DUPLICATE_ACCOUNTS];
  char *oldest_add_date, *newest_add_date, *newest_remove_date;
  int  n_new_records;
  int  add_records[MAX_DUPLICATE_ACCOUNTS];
  int  remove_records[MAX_DUPLICATE_ACCOUNTS];
  int  n_records, no_more_records, records_read, i, n_adds, n_removes;
  int  count, oldest_add_index, newest_add_index, newest_remove_index;


  if ((fp_in = fopen(credit_file, "r")) == NULL)
    {
      fprintf(stderr, "%s %s\n", catgets(CREDIT_catd, 1, 34, "Can't open credit file:"), credit_file);
      return(-1);
    }

  if ((fp_out = fopen(update_file, "w")) == NULL)
    {
      fprintf(stderr, "%s %s\n", catgets(CREDIT_catd, 1, 35, "Can't open update file:"), update_file);
      fclose(fp_in);
      return(-1);
    }

  while (1)
    {
      if (fgets(record, MAX_LEN, fp_in) == NULL)
	{
	  fprintf(stderr, "%s %s\n", catgets(CREDIT_catd, 1, 36, "Error reading file:"), credit_file);
	  return(-1);
	}
      if (record[0] != '#')
	fprintf(fp_out, record);
      else
	break;
    }

  n_records = atoi(record + 1);

  fgets(record, MAX_LEN, fp_in);
  sscanf(record, "%s %s %s %c", account, date, warning, &update_code);
  no_more_records = 0;
  records_read = 1;
  n_new_records = 0;

  do
    {
      strncpy(records[0].record, record, H_CONVERTED_RECORD_LEN);
      records[0].record[H_CONVERTED_RECORD_LEN] = '\0';
      strcpy(records[0].account, account);
      strcpy(records[0].date, date);
      records[0].update_code = update_code;

      count = 0;
      while (1)
	{
	  if (fgets(record, MAX_LEN, fp_in) == NULL)
	    {
	      no_more_records = 1;
	      break;
	    }
	  records_read ++;
	  sscanf(record, "%s %s %s %c", account, date, warning, &update_code);

	  /* Exit loop if new account number found */
	  if (strcmp(account, records[count].account) != 0)
	    break;

	  /* If new account is the same as the last account then add it
	     to the record array and then continue to read in the next
	     account record. */
	  count ++;

	  /* Don't go out of the array bounds */
	  if (count == MAX_DUPLICATE_ACCOUNTS)
	    {
	      fprintf(stderr, "%s\n",
		      catgets(CREDIT_catd, 1, 37, "Too many duplicate records. Ignoring the last ones."));
	      break;
	    }

	  strncpy(records[count].record, record, H_CONVERTED_RECORD_LEN);
	  records[count].record[H_CONVERTED_RECORD_LEN] = '\0';
	  strcpy(records[count].account, account);
	  strcpy(records[count].date, date);
	  records[count].update_code = update_code;
	}

      /* A new account number was found. The records array contains the
	 information for all the records with the same account. */

      /* Seperate the records into add records and remove records */
      n_adds = 0;
      n_removes = 0;
      for (i = 0; i <= count; i ++)
	{
	  /* Consider new and add records to be the same */

	  if (records[i].update_code == NEW_RECORD)
	    add_records[n_adds ++] = i;
	  else if (records[i].update_code == ADD_RECORD)
	    add_records[n_adds ++] = i;
	  else if (records[i].update_code == REMOVE_RECORD)
	    remove_records[n_removes ++] = i;

	  /* Otherwise ignore the record */
	}

      /* Note: this ignores the warning level, maybe we want to be
	 aware of the warning level when decided which of duplicate
	 adds we should place in the file. */

      /* No removes, so just add the record */
      if (n_adds > 0 && n_removes == 0)
	{

	  if (n_adds == 1)
	    oldest_add_index = add_records[0];

	  /* If there are multiple adds, find the earliest one. */
	  else if (n_adds > 1)
	    {
	      oldest_add_date = MAX_DATE;
	      for (i = 0; i < n_adds; i ++)
		if (strcmp(records[add_records[i]].date, oldest_add_date) < 0)
		  {
		    oldest_add_index = add_records[i];
		    oldest_add_date = records[oldest_add_index].date;
		  }
	    }
	  
	  fprintf(fp_out, records[oldest_add_index].record);
	  n_new_records ++;
	}

      else if (n_adds > 0 && n_removes > 0)
	{
	  /* Here we have a mixture of adds and removes. I am making the
	     assumption that the date on record refers to whether the
	     record should be added or removed. That means this: first find
	     the add record with latest date and the remove record with the
	     newest date. If the remove record is newer than the add record
	     than, don't use this account. If the add record is newer than
	     than the remove record, the account has been removed, but added
	     again, so the account number is added to the list. */

	  if (n_adds == 1)
	    newest_add_index = add_records[0];

	  /* If there are multiple adds find the newest one */
	  else if (n_adds > 1)
	    {
	      newest_add_date = MIN_DATE;
	      for (i = 0; i < n_adds; i ++)
		if (strcmp(records[add_records[i]].date, newest_add_date) > 0)
		  {
		    newest_add_index = add_records[i];
		    newest_add_date = records[newest_add_index].date;
		  }
	    }
	     
	  if (n_removes == 1)
	    newest_remove_index = remove_records[0];

	  /* If there are multiple removes find the newest one */
	  else if (n_removes > 1)
	    {
	      newest_remove_date = MIN_DATE;
	      for (i = 0; i < n_removes; i ++)
		if (strcmp(records[remove_records[i]].date,
			   newest_remove_date) > 0)
		  {
		    newest_remove_index = remove_records[i];
		    newest_remove_date = records[newest_remove_index].date;
		  }
	    }

	  /* If newest add is newer than newest remove, add the account record
	     to the output file. Or if the newest add and the newest remove
	     fall on the same date, assume that the record is being updated. */
	  if (strcmp(records[newest_add_index].date,
		     records[newest_remove_index].date) >= 0)
	    {
	      fprintf(fp_out, records[newest_add_index].record);
	      n_new_records ++;
	    }
	}
    }
  while (!no_more_records);
  
  /* Output the number of records at end of file (it's moved to the top) */
  fprintf(fp_out, "#%d\n", n_new_records);

  fclose(fp_in);
  fclose(fp_out);

  /* Use the unix utility 'sort' to move the above outputted number of
     records to its proper place at top of file (yes this is a major
     kludge). */
  sprintf(system_call, "sort %s > %s", update_file, TEMP_CREDIT_SORT_FILE);
  system(system_call);
  rename(TEMP_CREDIT_SORT_FILE, update_file);

}
