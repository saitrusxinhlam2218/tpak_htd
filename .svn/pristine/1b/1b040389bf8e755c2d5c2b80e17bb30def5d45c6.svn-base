/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: db_new.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/24 01:14:35 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/db/db_new.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: db_new.c,v 1.2 1998/02/24 01:14:35 taxiadm Exp $";


#include <stdio.h>
#include <isam.h>

#include "taxipak.h"
#include "taxi_error.h"
#include "Object.h"
#include "path.h"
#define DB_SRC
#include "mads_isam.h"
#include "rate_isam.h"


/***************************************************************************
  db_error():	There are three types of errors possible:
  		1) UNIX errors
		2) Informix errors
		3) db library errors

	        If the passed argument is non-zero, then it is assumed
		to be a db library error. Otherwise it is assumed to be
		a UNIX error on an Informix error.

  Returns:	The proper error object for the error.
*****************************************************************************/

RET_STATUS
db_error(err)
   RET_STATUS err;
{
   /* UNIX or Informix errors */
   if (err == 0)
   {
      /*
       * iserrno will hold the UNIX error. UNIX errors are below 100,
       * Informix errors are 100 and above.
       */

      /* UNIX error */
      if (iserrno < 100)
	 return (err_encode_special(DB_OBJ, UNIX_ERR_GP, iserrno));

      /* Informix error */
      return (err_encode_special(DB_OBJ, DB_ERR_GP, iserrno));

   }
   return (err_encode(DB_OBJ, err));

}				/* end db_error */

/**************************************************************
  db_open():  Open a db file.

  Returns: SUCCESS or an error code.

  Side Effects:
  Stores the file's file descriptor in the global Db_files.

**************************************************************/

RET_STATUS
db_open(file_index, mode)
   int   file_index;		/* Reference to file in database file array */
   int   mode;			/* C-ISAM value for file access mode */
{
   int   rc;

   if (file_index < 0 || file_index >= DB_NUM_FILES)
      return (db_error(DB_ERR_FILE_ID_INVALID));

   if (DB_FILE_IS_OPEN(file_index))
      return (db_error(DB_ERR_FILE_OPEN));

   rc = isopen(Db_files[file_index].name, mode);

   if (rc == -1)
      return (db_error(0));

   Db_files[file_index].fd = rc;
   return (SUCCESS);
}				/* end db_open */

/******************************************************************************
  db_close(): Close a db file.

  Returns:
  -1    Error. C-ISAM error in iserrno.
  0	File closed.

  Side Effects:
  The global array storing data about the database file state sets the file
  descriptor to -1 to note that the file is closed. The file's current read
  key is set to NULL.
*******************************************************************************/

RET_STATUS
db_close(file_index)
   int   file_index;		/* Reference to file in database file array */
{
   int   rc;

   if (file_index < 0 || file_index >= DB_NUM_FILES)
   {
      return (db_error(DB_ERR_FILE_ID_INVALID));
   }
   rc = isclose(Db_files[file_index].fd);
   if (rc == -1)
   {
      return (db_error(0));
   }
   Db_files[file_index].fd = -1;
   Db_files[file_index].current_key = NULL;

   return (SUCCESS);
}				/* end db_close */

/*****************************************************************************
  db_done(): Close all open db files.

  Returns: None. Since this would be called as a program is exiting, you can
           get away with blindly closing files. What are you going to do
	   if a close fails anyway?

  Side Effects:
  Same as closing all open database files.
******************************************************************************/

void
db_done()
{
   int   i;

   for (i = 0; i < DB_NUM_FILES; i++)
      if (DB_FILE_IS_OPEN(i))
	 db_close(i);
}				/* end db_done */


/**************************************************************************
  db_read(): Read a record from a db file using the file's current key
	     (which will be the default key if the current key has not
	     been set).

  Returns:
  -1    Error. C-ISAM error in iserrno.
  0	Succesful read.
***************************************************************************/

RET_STATUS
db_read(file_index, c_record, mode)
   int   file_index;		/* Reference to file in database file array */
   void *c_record;		/* Buffer to store the read result in */
   int   mode;			/* C-ISAM value for file read mode */
{
  int   rc;
   char isam_record[DB_MAX_ISAM_RECORD_LEN];
  
  if (file_index < 0 || file_index >= DB_NUM_FILES)
    return (db_error(DB_ERR_FILE_ID_INVALID));
  
  (*Db_files[file_index].c_to_key_func) (c_record, isam_record);
  
  rc = isread(Db_files[file_index].fd, isam_record, mode);
  
  /* Should we zero out failed reads? */
  
  /*
   * If we reach beginning or end of file (iserrno == EENDFILE) then either
   * the first or last record in the file. This is done because the pointer
   * counts past the EOF
   */
  if (rc == -1)
  {
    if (iserrno == EENDFILE)
    {
      if (mode == ISNEXT)
      {
	rc = isread(Db_files[file_index].fd, isam_record, ISLAST);
	if (rc == -1)
	  return(db_error(0));
      }
      
      else if (mode == ISPREV)
      {
	rc = isread(Db_files[file_index].fd, isam_record, ISFIRST);
	if (rc == -1)
	  return(db_error(0));
      }
      
      /* Some other read mode */
      else
	return (db_error(0));

      /* If read of last/first was successful, we still need
	 to return an error (but fill the c_record with the read
	 data) */
      iserrno = EENDFILE;
    }

    /* Some other error than EENDFILE */
    else
      return (db_error(0));
  }

  (*Db_files[file_index].isam_to_c_func) (c_record, isam_record);
  
  if (iserrno == EENDFILE)
    return (db_error(0));
  
  return (SUCCESS);
}				/* end db_read */

/****************************************************************************
  db_set_key(): Choose a new key to read from a db file.

  Returns:
    -1    Error. C-ISAM error in iserrno.
    0     Succesful setting of key.

  Side Effects:
  If the key passed in is the same as the current key, nothing happens and
  the current record remains the same (and the current key stays the same).
  If the new key is different than the current key, the current record is
  set to first record of new key.
***************************************************************************/

RET_STATUS
db_set_key(file_index, key)
   int   file_index;		/* Reference to file in database file array */
   struct keydesc *key;		/* C-ISAM key structure for desired database
				 * file */
{
   int   rc;

   if (file_index < 0 || file_index >= DB_NUM_FILES)
      return (db_error(DB_ERR_FILE_ID_INVALID));

   if (key == NULL)
      return (db_error(DB_ERR_KEY_NULL));

   if (Db_files[file_index].current_key == key)
      return (SUCCESS);

   /*
    * Although we could go anywhere in the file, use ISFIRST to keep things
    * easy and efficient (finding the first record for a key is pretty quick)
    */
   rc = isstart(Db_files[file_index].fd, key, 0, NULL, ISFIRST);

   if (rc == -1)
   {
      Db_files[file_index].current_key = NULL;
      return (db_error(0));
   }
   Db_files[file_index].current_key = key;

   return (SUCCESS);
}				/* end db-set_key */

/****************************************************************************
  db_read_key(): Set a key AND read a record. This is the same as calling
                 db_set_key() followed by db_read().

  Returns:
    -1    Error. C-ISAM error in iserrno.
    0     Succesful setting of key.

  Side Effects:
  The current key is set to the passed in key.

*****************************************************************************/

RET_STATUS
db_read_key(file_index, c_record, key, mode)
   int   file_index;		/* Reference to file in database file array */
   void *c_record;		/* Buffer to store the read result in */
   struct keydesc *key;		/* C-ISAM key structure for desired database
				 * file */
   int   mode;			/* C-ISAM value for file read mode */
{
   int   rc;

   rc = db_set_key(file_index, key);
   if (rc != SUCCESS)
      return (rc);

   rc = db_read(file_index, c_record, mode);
   return (rc);
}				/* end db_read_key */

/****************************************************************************
  db_read_by_record_number(): Read a record from a db file by its record
  number. Record number refers to the physical order of the records in
  the file.


  Returns:
    -1    Error. C-ISAM error in iserrno.
    0     Succesful

  Side Effects:

*****************************************************************************/

RET_STATUS
db_read_by_record_number(file_index, record_num, c_record)
   int   file_index;		/* Reference to file in database file array */
   int   record_num;		/* Record number */
   void *c_record;		/* Buffer to store the read result in */
{
   int   rc;
   struct keydesc key;
    char isam_record[DB_MAX_ISAM_RECORD_LEN];

   if (file_index < 0 || file_index >= DB_NUM_FILES)
      return (db_error(DB_ERR_FILE_ID_INVALID));

   /* Set key for reading by record number */
   key.k_nparts = 0;
   isrecnum = record_num;

   rc = isstart(Db_files[file_index].fd, &key, 0, isam_record, ISEQUAL);

   /*
    * Since I don't know whether the the above isstart will change the
    * current key, set it to NULL to play safe
    */
   Db_files[file_index].current_key = NULL;

   if (rc == -1)
      return (db_error(0));

   rc = isread(Db_files[file_index].fd, isam_record, ISCURR);

   if (rc == -1)
      return (db_error(0));


   (*Db_files[file_index].isam_to_c_func) (c_record, isam_record);

   return (SUCCESS);
}				/* end db_read_by_record_number */

/***************************************************************
  db_write(): Write a new record to a db file.

  Returns:
      -1    Error. C-ISAM error in iserrno.
      0     Succesful.

  Side Effects:
  The current record will be set to the newly written record.
***************************************************************/

RET_STATUS
db_write(file_index, c_record)
   int   file_index;		/* Reference to file in database file array */
   void *c_record;		/* Buffer holding data to write to the db
				 * file */
{
   int   rc;
    char isam_record[DB_MAX_ISAM_RECORD_LEN];

   if (file_index < 0 || file_index >= DB_NUM_FILES)
      return (db_error(DB_ERR_FILE_ID_INVALID));

   (*Db_files[file_index].c_to_isam_func) (c_record, isam_record, ADD_REC);

   rc = iswrite(Db_files[file_index].fd, isam_record);

   if (rc == -1)
      return (db_error(-1));

   return (SUCCESS);
}				/* end db_write */

/********************************************************************
  db_delete() Delete the current record from a db file.
  Returns:
      -1    Error. C-ISAM error in iserrno.
      0     Succesful.

  Side Effects:
  The current record is UNDEFINED after this operation.
********************************************************************/

RET_STATUS
db_delete(file_index, c_record)
   int   file_index;		/* Reference to file in database file array */
   void	*c_record;
{
   int   rc;

   if (file_index < 0 || file_index >= DB_NUM_FILES)
      return (db_error(DB_ERR_FILE_ID_INVALID));

   rc = isdelcurr(Db_files[file_index].fd);

   if (rc == -1)
      return (db_error(0));


   return (SUCCESS);
}				/* end db_delete */

/********************************************************************************
  db_release(): Release record locks from a db file.
                Records may be locked by doing a db_read() with ISLOCK as
		part of the mode.

  Returns:
    -1    Error. C-ISAM error in iserrno.
    0     Succesful

*********************************************************************************/

RET_STATUS
db_release(file_index)
   int   file_index;		/* Reference to file in database file array */
{
   int   rc;

   if (file_index < 0 || file_index >= DB_NUM_FILES)
      return (db_error(DB_ERR_FILE_ID_INVALID));

   rc = isrelease(Db_files[file_index].fd);

   if (rc == -1)
      return (db_error(0));

   return (SUCCESS);
}				/* end db_release */

/*****************************************************
  db_update(): Update current record in a db file.

  Returns:
    -1    Error. C-ISAM error in iserrno.
    0     Succesful setting of key.
******************************************************/

RET_STATUS
db_update(file_index, c_record)
   int   file_index;		/* Reference to file in database file array */
   void *c_record;		/* Buffer holding data to update in the db
				 * file */
{
   int   rc;
    char isam_record[DB_MAX_ISAM_RECORD_LEN];

   if (file_index < 0 || file_index >= DB_NUM_FILES)
      return (db_error(DB_ERR_FILE_ID_INVALID));

   (*Db_files[file_index].c_to_isam_func) (c_record, isam_record, UPDATE_REC);
   rc = isrewcurr(Db_files[file_index].fd, isam_record);

   if (rc == -1)
      return (db_error(0));


   return (SUCCESS);
}				/* end db_update */

/********************************************************************************
  db_lock(): Lock an entire DB file.

  Returns:
    -1    Error. C-ISAM error in iserrno.
    0     Succesful

*********************************************************************************/

RET_STATUS
db_lock(file_index)
   int   file_index;		/* Reference to file in database file array */
{
   int   rc;

   if (file_index < 0 || file_index >= DB_NUM_FILES)
      return (db_error(DB_ERR_FILE_ID_INVALID));

   rc = islock(Db_files[file_index].fd);

   if (rc == -1)
      return (db_error(0));

   return (SUCCESS);
}				/* end db_release */

/********************************************************************************
  db_unlock(): Unlock an entire DB file.

  Returns:
    -1    Error. C-ISAM error in iserrno.
    0     Succesful

*********************************************************************************/

RET_STATUS
db_unlock(file_index)
   int   file_index;		/* Reference to file in database file array */
{
   int   rc;

   if (file_index < 0 || file_index >= DB_NUM_FILES)
      return (db_error(DB_ERR_FILE_ID_INVALID));

   rc = isunlock(Db_files[file_index].fd);

   if (rc == -1)
      return (db_error(0));

   return (SUCCESS);
}				/* end db_release */
