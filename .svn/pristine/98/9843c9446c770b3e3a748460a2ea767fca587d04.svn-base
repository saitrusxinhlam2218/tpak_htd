/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Unzoned_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:57 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Unzoned_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Unzoned_db.h,v $	$Revision: 1.2 $"

#ifndef _UNZONED_DB_H
#define _UNZONED_DB_H

#include <isam.h>

typedef struct   cisam_uz  {
       int    call_nbr;                      /* Call number this address is associated with.           */
	char  fleet;
       char   call_date[10];                     /* Date this call was placed  */
       char  call_time[7];                     /* Time this call was place.  */
       short  entered_by;                    /* User ID of the call taker who entered it.            */
       short  zoned_as;                       /* Zone number assigned to his address by the call taker or supervisor.      */
       short  zoned_by;                       /* User ID of the person who zoned this call.          */
       int    street_nbr;                     /* See street file.          */
       char   type_of_service;               /* Call type-passenger, delivery, type 'X' etc.  (see call record for complete list).*/
       char   charge_nbr[19];                 /* Account number, if a charge call.              */
       char   str_place_name1[21];            /* See street file.         */
       char   str_type1[4];                   /* See street file.          */
       char   pre_dir1[3];                   /* See street file.          */
       char   post_dir1[3];                   /* See street file.          */
       char   pre_dir2[3];                    /* Intersecting street.  See street file.              */
       char   str_place_name2[21];            /* Intersecting street file  */
       char   str_type2[4];                /* Intersecting street.  See street file.              */
       char   post_dir2[3];                   /* Intersecting street.  See street file.              */	
       char   city[4];                        /* City.                     */

 }UNZONE_REC;

#ifdef KEY_SRC

struct keydesc uz_key1= {
         ISNODUPS,
         1,
         {0,LONGSIZE,LONGTYPE},
};

/* Composite of (str_place_name1,str_type1,pre_dir1,post_dir1) */
struct keydesc uz_key2= {
         ISDUPS,
         4,
				 { {49,20,CHARTYPE},
					 {69,3,CHARTYPE},
					 {72,2,CHARTYPE},
					 {74,2,CHARTYPE} }
};

#else
extern struct keydesc uz_key1;
extern struct keydesc uz_key2;
#endif

#endif
