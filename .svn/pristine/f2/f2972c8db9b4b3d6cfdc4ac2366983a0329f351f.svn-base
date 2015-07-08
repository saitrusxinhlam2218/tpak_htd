/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Locality_db.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2002/09/11 13:39:44 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Locality_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _LOCALITY_DB_H
#define _LOCALITY_DB_H
#include <isam.h>
typedef struct   cisam_lo   {
       char   code[4];                 /* City name, alias or abbrevivation*/
       char   act_city_code[4];        /* City code, if an alias or abbreviation record.             */
       char   subzone[6];              /* 1-9999.  A subzone number that would translate to the correct zone */
                                       /* number for the entire city.  Should be filled in only for cities */
                                       /* with no address information in the address file.                    */
       int    zip;                     /* 1-99999.  A zip code number for the entire city. Should be filled in */
                                       /* only for cities with no address information in the address file. */
       char   full_name[26];           /* City full name for report prirnting etc.  to the address filel.      */
   } LOC_REC;
#ifdef KEY_SRC
struct keydesc lo_key1= {
         ISNODUPS,
         1,
         {0,3,CHARTYPE},
};
struct keydesc lo_key2 = {
    ISNODUPS,
    1,
    { 6, 5, CHARTYPE},
  };
#else
extern struct keydesc lo_key1;
extern struct keydesc lo_key2;
#endif
#endif
