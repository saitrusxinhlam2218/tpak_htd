/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Shift_hist_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/03/13 18:14:16 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Shift_hist_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

#ifndef _SHIFT_HIST_DB_H
#define _SHIFT_HIST_DB_H

#include <isam.h>

#ifdef KEY_SRC
/* call_nbr */
struct keydesc hh_key1=  {
	 ISDUPS,
	 1,
				 {4,LONGSIZE,LONGTYPE},
};
/* record_nbr system assigned for redundancy only */
struct keydesc hh_key2=  {
	 ISNODUPS,
	 1,
         {0,LONGSIZE,LONGTYPE},
};

#endif

#endif
