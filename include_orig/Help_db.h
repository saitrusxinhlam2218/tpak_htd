/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Help_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:43:27 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Help_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Help_db.h,v $	$Revision: 1.2 $"

#ifndef _HELP_DB_H
#define _HELP_DB_H
#include <isam.h>

typedef struct cisam_hp   {
  short nav_string;           /* Navigation string corresponding to the menu choice that help was requested on.        */
  short field_nbr;            /* Field number of the field if field specific help information was requested.           */
  short line_nbr;             /* Line number assigned to each message line to sequence the display process.             */ 
  
  /*  Note: The following 2 fields will be filled only in the case of multipl fields having the same help message information. */
  
  short like_nav_string;       /* In case of a like field, this filed would contain the navigation string of the primary */
  /* containing the help information.          */
  short like_field_nbr;        /* In case of a like field, this field would contain the like field number.            */
  char  help_line[131];        /* A line of help information.               */
}HELP_REC ;

#ifdef KEY_SRC
struct keydesc hp_key1= {
         ISNODUPS,
         3,
				 { {0,INTSIZE,INTTYPE},
					 {2,INTSIZE,INTTYPE},
					 {4,INTSIZE,INTTYPE} }
};
#else
extern struct keydesc hp_key1;
#endif

#endif
