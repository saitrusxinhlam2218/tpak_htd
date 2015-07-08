/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: db.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2003/01/28 16:38:44 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#include <isam.h>
#ifdef _MYSQL_
#include <mysql.h>
#endif
#ifndef _DB_H_
#define _DB_H_
#define DB_MAX_ISAM_RECORD_LEN	2048
struct Db_file_list {
  char *name;
  void  (*c_to_isam_func)();
  void  (*isam_to_c_func)();
  void  (*c_to_key_func)();
  int  fd;
  struct keydesc *current_key;
  int  size;
};
typedef struct Db_file_list DB_FILES;
#ifdef _MYSQL_
struct Db_mysql_file_list {
  char *name;
  void  (*c_to_isam_func)();
  void  (*isam_to_c_func)();
  void  (*c_to_key_func)();
  int  fd;
  struct keydesc *current_key;
  int  size;
  MYSQL_RES *result;
  int  position;
};
typedef struct Db_mysql_file_list DB_MYSQL_FILES;
#endif
/*
 Db_files must be initialized elsewhere in the program that uses Db.
 It must be initialized in the following fashion.
DB_FILES Db_files[] =
{
  {FILE_1, isam_to_c_func1, c_to_cisam_func1, c_to_key_func1,
     -1, NULL, sizeof(struct_1)},
  {FILE_2, isam_to_c_func2, c_to_cisam_func2, c_to_key_func2,
     -1, NULL, sizeof(struct_2)},
  {FILE_3, isam_to_c_func3, c_to_cisam_func3, c_to_key_func3,
     -1, NULL, sizeof(struct_3)},
  ... (As many files as necessary)
  {FILE_N, isam_to_c_funcN, c_to_cisam_funcN, c_to_key_funcN,
     -1, NULL, sizeof(struct_N)},
}
*/
#define DB_FILE_IS_OPEN(i) (Db_files[i].fd != -1)
/* Open read/write modes */
#define DB_READ_ONLY	ISINPUT
#define DB_WRITE_ONLY	ISOUTPUT
#define DB_READ_WRITE	ISINOUT
/* Open lock modes */
#define DB_EXCL_LOCK	ISEXCLLOCK
#define DB_AUTO_LOCK	ISAUTOLOCK
#define DB_MANUAL_LOCK	ISMANULOCK
/* Read modes */
#define DB_CURRENT	ISCURR
#define DB_FIRST	ISFIRST
#define DB_LAST		ISLAST
#define DB_PREV		ISPREV
#define DB_NEXT		ISNEXT
#define DB_GTEQ		ISGTEQ
#define DB_EQUAL	ISEQUAL
#define DB_GREAT	ISGREAT
#define DB_LOCK		ISLOCK
/* Errors */
#define DB_ERR_FILE_ID_INVALID	200
#define DB_ERR_FILE_OPEN	201
#define DB_ERR_KEY_NULL		202
#endif 
