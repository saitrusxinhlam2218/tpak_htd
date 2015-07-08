/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Rate_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:07 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Rate_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _RATE_DB_H
#define _RATE_DB_H
#include <isam.h>
typedef struct     cisam_rt  {
       char   fleet;                          /* Fleet indicator           */
       char   pad;                               /* Dummy char.               */
       int    zone_zip_nbr1;                  /* A zone or zip code number.  This one will be the lesser of the pair.       */
       int    zone_zip_nbr2;                  /* A zone or zip code number.  This one will be the greater of the pair.      */
       float  flat_rate;                      /* Dollar amount for this service.                  */
    }RATE_REC;
#ifdef KEY_SRC
struct keydesc rt_key1= {
         ISNODUPS,
         3,
				 { {0,CHARSIZE,CHARTYPE},
					 {2,LONGSIZE,LONGTYPE},
					 {6,LONGSIZE,LONGTYPE} }
};
#else
extern struct keydesc rt_key1;
#endif
#endif
