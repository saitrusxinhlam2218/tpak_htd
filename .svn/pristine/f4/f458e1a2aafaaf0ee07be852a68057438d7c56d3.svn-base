/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Holiday_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:43:31 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Holiday_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Holiday_db.h,v $	$Revision: 1.2 $"

#ifndef _HOLIDAY_DB_H
#define _HOLIDAY_DB_H
#include <isam.h>

typedef struct   cisam_hl {
       int	mon;			/* Holiday month.	*/
       int	day;			/* Holiday day.	*/
       int	year;			/* Holiday year.	*/ 
       char	desc[21];		/* Holiday description.	*/
 }HOLIDAY_REC;

#ifdef KEY_SRC
struct keydesc hl_key1= {
         ISNODUPS,
         3,
				 { {0,LONGSIZE,LONGTYPE},
					 {4,LONGSIZE,LONGTYPE},
					 {8,LONGSIZE,LONGTYPE} }
};
#else
extern struct keydesc hl_key1;
#endif

#endif
