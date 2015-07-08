/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: do_credit.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1997/08/09 18:08:10 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/sysadmin/do_credit.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: do_credit.c,v 1.2 1997/08/09 18:08:10 taxiadm Exp $";

/*
 * do_credit.c Created 6-6-90 Doug Rudoff
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "path.h"
#include "credit.h"
#include "language.h"
#include "credit_strdefs.h"

char *tarfile = NULL;

int
get_credit_file_type(fp)
   FILE *fp;
{
   char  buf[MAX_LEN];
   int   start;
   int    rec_len;


   rewind(fp);

   if (fgets(buf, MAX_LEN, fp) == NULL)
      return (-1);

   /*
    * A backed up file has been already converted into internal format. The
    * first char of the first line will start with a '!' or '#'.
    */
   if ((buf[0] == '!') || (buf[0] == '#'))
      return (BACKED_UP_FILE_TYPE);

   /* how big is a record */
   rec_len = strlen(buf);

   if (rec_len == 0)
      return (-1);

   if (rec_len  >  S_CONVERTED_RECORD_LEN)
   {
      /* Find the 3 character file type code */
      if (strncmp(buf, UPDATE_FILE_CODE, 3) == 0)
	 return (UPDATE_FILE_TYPE);
      else if (strncmp(buf, NEW_FILE_CODE, 3) == 0)
	 return (NEW_FILE_TYPE);
      return (-1);
   }
   if (rec_len == S_CONVERTED_RECORD_LEN)
      return (STOCKHOLM_FILE_TYPE);
   return (-1);
}				/* end get_credit_file_type */

void
back_up_credit()
{
   char  s[10], system_call[132], str[MAX_LEN];


   if (chdir(CREDIT_BASE_DIR) == -1)
   {
      printf("%s (%s)\n", catgets(CREDIT_catd, 1, 4, "Could not change to proper directory."),
	     CREDIT_BASE_DIR);
      return;
   }
   printf("\n\n%s\n%s : ",
	  catgets(CREDIT_catd, 1, 5, "If you want to back up the bad credit card account file,"),
	  catgets(CREDIT_catd, 1, 6, "insert the tape and type 'Y' followed by a return when ready"));

   gets(s);

   printf("\n");

   if (toupper(s[0]) != YES)
   {
      printf("%s\n", catgets(CREDIT_catd, 1, 7, "Bad credit card file not backed up."));
      return;
   }
   /*
    * Make a soft link to the credit file (removing any other file with the
    * same name first), tar it, and remove the link
    */

   /* Remove old loaded file, and create a soft link to current credit file */

   sprintf(system_call, "rm -f %s; ln -s %s %s;",
	   LOADED_CREDIT_FILE, BAD_CREDIT_FILE, LOADED_CREDIT_FILE);

   /*
    * If tarfile is not NULL then use other than default tarfile. The 'h'
    * flag with tar will cause the link to act like a real file.
    */
   /*
    * fix
    * 
    * use shell script for tar command so customer can modify copy method
    */
   /*
    * if (tarfile) sprintf(str, "tar -chf %s %s;", tarfile,
    * LOADED_CREDIT_FILE); else sprintf(str, "tar -ch %s;",
    * LOADED_CREDIT_FILE);
    */

   system(system_call);

   sprintf(system_call, "%s/%s %s; ",
	   CREDIT_BASE_DIR, BACKUP_CREDIT_FILE, LOADED_CREDIT_FILE);

   sprintf(str, "rm %s", LOADED_CREDIT_FILE);

   strcat(system_call, str);

   system(system_call);

   printf("%s\n",
	  catgets(CREDIT_catd, 1, 8, "Done backing up the bad credit card file. Please remove the tape."));
}

void
new_or_update_credit()
{
   FILE *fp;
   char  s[10], system_call[MAX_LEN];
   char  new_converted_file[MAX_LEN];
   char  merge_file[MAX_LEN];
   char  update_file[MAX_LEN];
   char *cptr;
   int   file_type;


   if (chdir(CREDIT_BASE_DIR) == -1)
   {
      printf("%s (%s)\n", catgets(CREDIT_catd, 1, 4, "Could not change to proper directory."),
	     CREDIT_BASE_DIR);
      return;
   }
   printf("\n\n%s\n%s\n%s : ",
	  catgets(CREDIT_catd, 1, 9, "If you want to load a new bad credit card file"),
	  catgets(CREDIT_catd, 1, 10, "or update the current bad credit card account file,"),
	  catgets(CREDIT_catd, 1, 6, "insert the tape and type 'Y' followed by a return when ready"));

   gets(s);

   printf("\n");

   if (toupper(s[0]) != YES)
   {
      printf("%s\n", catgets(CREDIT_catd, 1, 11, "Bad credit card file not loaded."));
      return;
   }
   printf("%s\n", catgets(CREDIT_catd, 1, 12, "Reading in bad credit card file ..."));

   /* Remove any previously loaded credit file laying around */
   sprintf(system_call, "rm -f %s", LOADED_CREDIT_FILE);
   system(system_call);

   /*
    * Read the file from the tape. The file that has been tarred onto tape
    * must have the name define by LOADED_CREDIT_FILE
    */
   /*
    * if (tarfile) sprintf(system_call, "tar -xvf %s", tarfile); else
    * sprintf(system_call, "tar -xv");
    */
   sprintf(system_call, "%s/%s %s",
	   CREDIT_BASE_DIR, UPDATE_CREDIT_FILE, LOADED_CREDIT_FILE);

   system(system_call);

   if ((fp = fopen(LOADED_CREDIT_FILE, "r")) == NULL)
   {
      printf("%s\n", catgets(CREDIT_catd, 1, 13, "Bad credit file not loaded properly."));
      return;
   }
   printf("%s\n", catgets(CREDIT_catd, 1, 14, "Bad credit file loaded."));

   file_type = get_credit_file_type(fp);

   fclose(fp);

   switch (file_type)
   {
   case BACKED_UP_FILE_TYPE:
      /* A backed up credit file is already in the proper format */
      printf("%s\n", catgets(CREDIT_catd, 1, 15, "Restoring bad credit file ..."));

      if (try_to_lock_bad_credit_file() == -1)
      {
	 printf("%s\n", catgets(CREDIT_catd, 1, 16, "Cannot lock the bad credit file."));
	 printf("%s\n", catgets(CREDIT_catd, 1, 17, "Bad credit file NOT restored."));
	 unlink(LOADED_CREDIT_FILE);
	 return;
      }
      rename(LOADED_CREDIT_FILE, BAD_CREDIT_FILE);
      unlock_credit_file();
      break;

   case STOCKHOLM_FILE_TYPE:
   case NEW_FILE_TYPE:
   case UPDATE_FILE_TYPE:
      sprintf(new_converted_file, "%s.convert", LOADED_CREDIT_FILE);

      /* A new or update credit file needs to be converted to new format */
      if (format_credit_file(file_type, LOADED_CREDIT_FILE,
					    new_converted_file) == -1)
      {
	 /* Get rid of unneeded files */
	 unlink(LOADED_CREDIT_FILE);
	 unlink(new_converted_file);
	 printf("%s\n",
		catgets(CREDIT_catd, 1, 18, "Unable to convert new bad credit card data file to new format."));
	 printf("%s\n", catgets(CREDIT_catd, 1, 19, "No change to bad credit card file."));
	 return;
      }
      /* Get rid of unneeded original file */
      unlink(LOADED_CREDIT_FILE);

      if ((file_type == NEW_FILE_TYPE) || file_type == (STOCKHOLM_FILE_TYPE))
      {
	 printf("%s\n", catgets(CREDIT_catd, 1, 20, "Creating or replacing current bad credit file ..."));
	 if (try_to_lock_bad_credit_file() == -1)
	 {
	    printf("%s\n", catgets(CREDIT_catd, 1, 16, "Cannot lock the bad credit file."));
	    printf("%s\n", catgets(CREDIT_catd, 1, 21, "Bad credit file NOT replaced or created."));
	    unlink(new_converted_file);
	    return;
	 }
	 rename(new_converted_file, BAD_CREDIT_FILE);
	 unlock_credit_file();
      } else if (file_type == UPDATE_FILE_TYPE)
      {
	 printf("%s\n", catgets(CREDIT_catd, 1, 22, "Updating current bad credit file ..."));
	 sprintf(merge_file, "%s.merge", LOADED_CREDIT_FILE);
	 sprintf(update_file, "%s.update", LOADED_CREDIT_FILE);


	 /*
	  * Merge and sort the current credit file with the new update file
	  * and put in a new file. (sort -um merges and removes duplicate
	  * entries from the two files.)
	  */
	 sprintf(system_call, "sort -um %s %s > %s",
		 BAD_CREDIT_FILE, new_converted_file, merge_file);
	 system(system_call);

	 if (update_credit_file(merge_file, update_file) == -1)
	 {
	    /* Get rid of unneeded files */
	    unlink(merge_file);
	    unlink(new_converted_file);
	    unlink(update_file);
	    printf("%s\n",
		   catgets(CREDIT_catd, 1, 23, "Unable to update bad credit card file with new data."));
	    printf("%s\n", catgets(CREDIT_catd, 1, 19, "No change to bad credit card file."));
	    return;
	 }
	 /* Get rid of unneeded files */
	 unlink(merge_file);
	 unlink(new_converted_file);

	 if (try_to_lock_bad_credit_file() == -1)
	 {
	    printf("%s\n", catgets(CREDIT_catd, 1, 16, "Cannot lock the bad credit file."));
	    printf("%s\n", catgets(CREDIT_catd, 1, 24, "Bad credit file NOT updated."));
	    unlink(update_file);
	    return;
	 }
	 rename(update_file, BAD_CREDIT_FILE);
	 unlock_credit_file();
      }
      break;

   default:
      printf("%s\n", catgets(CREDIT_catd, 1, 25, "ERROR: Unknown file type. No change to bad credit file."));
      return;
   }				/* end case */

   printf("%s\n",
	  catgets(CREDIT_catd, 1, 26, "Done loading bad credit card file. Please remove the tape."));

   /* sync the bad credit file	 */
   cptr = strrchr(BAD_CREDIT_FILE, '/');

   if (cptr != NULL)
      sprintf(system_call, "syncdb -f %s", ++cptr);
   else
      sprintf(system_call, "syncdb -f %s", BAD_CREDIT_FILE);

   system(system_call);
}
