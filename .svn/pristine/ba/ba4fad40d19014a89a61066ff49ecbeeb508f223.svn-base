/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Street_type_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:29 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Street_type_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _STREET_TYPE_DB_H
#define _STREET_TYPE_DB_H
#include <isam.h>
typedef struct   cisam_st  {
       char    abbrev[4];          /*  Street type or an abbreviation.        */
       char    actual[4];          /* Actual street type, if an abbreviation  */
       char    desc[13];           /* Street type description.                */
}STREET_TYPE_REC;
#ifdef KEY_SRC
struct keydesc st_key1= {
         ISNODUPS,
         1,
         {0,3,CHARTYPE},
};
#else
extern struct keydesc st_key1;
#endif
#endif
