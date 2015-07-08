/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Canmsg_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:42:37 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Canmsg_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _CANMSG_DB_H
#define _CANMSG_DB_H
#include <isam.h>
typedef struct 	cisam_cm	{
		int	nbr;		/* Canned message number (1 through 24)	*/
		char	text[41];	/* Canned message text			*/
} CANMSG_REC;
#ifdef KEY_SRC
struct keydesc cm_key1= {
         ISNODUPS,
         1,
				 { {0,LONGSIZE,LONGTYPE} }
};
#else
extern struct keydesc cm_key1;
#endif
#endif
