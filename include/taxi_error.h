/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: taxi_error.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/02/23 21:44:38 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/taxi_error.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
/*
 *  taxi_error.h
 *
 *   Error definitions and functions for TaxiPak.
 *
 *   These definitions, macros and functions are designed to support
 *   The concept of error tuples.  A tuple is a three-part value
 *   containing:
 *
 *     - the object where the error was "created"
 *     - the group the error belongs to (application, UNIX, DataBase)
 *     - an integral value for the error
 */
#ifndef _TAXI_ERR_H_
#define _TAXI_ERR_H_
#include "Object.h"
#define DB_ERR_MSG_SIZE 52          /* db error */
#define STD_ERR_MSG_SIZE 256        /* to stderr */
#define UI_ERR_MSG_SIZE 132         /* for ui */
#define DEFAULT_LOG  0
#define NO_LOG       1
#define FORCE_LOG    2
#define DEFAULT_ABORT  0
#define NO_ABORT       1
#define FORCE_ABORT    2
/*
 *  Error group IDs  (1 -255)
 */
#define  APP_ERR_GP   0
#define  UNIX_ERR_GP   1
#define  DB_ERR_GP     2
/*
 *  useful macros 
 */
#define err_encode_special(id,gp,val)\
          ((RET_STATUS)(((id)<<24)|(((gp)&0xFF)<<16)|((unsigned short)val)))
#define err_encode(id,val) (err_encode_special((id),APP_ERR_GP,(val)))
#define err_object(ret_val)  ((int)((ret_val)>>24))
#define err_group(ret_val)   ((int)(((ret_val)>>16) & 0x000000FF))
#define err_value(ret_val)   ((int)((ret_val) & 0x0000FFFF))
#define TAXI_ERR_FLAG(ret_val,msg,log_flag,abort_flag)\
    (tp_error((ret_val), (msg), (log_flag), (abort_flag),__FILE__,__LINE__ ))
#define TAXI_ERR(ret_val,msg)\
    (tp_error((ret_val), (msg), DEFAULT_LOG,DEFAULT_ABORT,__FILE__,__LINE__ ))
/*
 *  Function prototypes...
 */
#ifdef PROTOTYPE
RET_STATUS tp_error( RET_STATUS, char *, BOOLEAN, BOOLEAN, char *, int );
void ERR_get_text( RET_STATUS, char * );
void ERR_format_tuple( RET_STATUS, char * );
#else
RET_STATUS tp_error();
void ERR_format_tuple();
#endif
/*
 *  Common error values  (range 1 - 100)
 */
#define NO_MEMORY		1       
#define FILE_NOT_FOUND		2
#define NULL_ARG		3		/* WAS NO_DATA */
#define INVALID_ATTR		4
#define RECORD_NOT_FOUND	5
#define READ_FAILED		6
#define ADD_FAILED		7
#define UPDATE_FAILED		8
#define DELETE_FAILED		9
#define FILE_OPEN_FAILED	10
#define FILE_CLOSE_FAILED	11
#define END_OF_FILE		12
#define FILE_NOT_OPEN		13	/* file should be open but is not */
#define OBJECT_NOT_FOUND	14
#define INVALID_ARG		15  /* invalid function parameter value detected */
#define END_OF_LIST		16
#define SMALL_SHARED_MEM	17	/* not enough shared memory for records available */
#define	IPC_FAILED		19
#define	UNSUPPORTED_ARG		20
#define	NO_CHOICE	21		/* user selected none */
#define ERR_OBJ_BEGIN   100  /* beginning number for object-specific values */
#endif   /* must be last line of file */
