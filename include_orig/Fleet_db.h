/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Fleet_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:43:19 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Fleet_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Fleet_db.h,v $	$Revision: 1.2 $"

#ifndef _FLEET_DB_H
#define _FLEET_DB_H

#include <isam.h>

/* fleet */
#define FL_ID			0
#define FL_NBR			FL_ID + 2

#define FL_KEY1 1
#define FL_KEY2 2

#ifdef KEY_SRC
struct keydesc fl_key1=  {
	 ISNODUPS,
	 1,
         {FL_ID,CHARSIZE,CHARTYPE},
};

/* nbr */
struct keydesc fl_key2=  {
	 ISNODUPS,
	 1,
         {FL_NBR,INTSIZE,INTTYPE},
};

#else
extern struct keydesc fl_key1;
extern struct keydesc fl_key2;
#endif

#endif
