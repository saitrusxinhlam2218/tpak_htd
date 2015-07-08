/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Error_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:43:01 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Error_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _ERROR_DB_H
#define _ERROR_DB_H
#include <isam.h>
typedef struct  cisam_el  {
  int      nbr;            
  char     fleet;          
  char     file[8];
  char     date[9];
  char     time[9];
  char     source_file[15];
  short    line_nbr;       
  char     error_msg[51];  
} ERROR_REC;
#ifdef KEY_SRC
struct keydesc el_key1= {
         ISNODUPS,
         1,
				 { {0,LONGSIZE,LONGTYPE} }
};
#else
extern struct keydesc el_key1;
#endif
#endif
