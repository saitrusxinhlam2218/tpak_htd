/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Seal_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:17 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Seal_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _SEAL_DB_H
#define _SEAL_DB_H
#include <isam.h>
typedef struct      cisam_sl   {
        short   seal_nbr;                     /* Seal number.              */
        char    desc[21];                     /* Seal description.         */
        char    abbrev[4];                    /* Abbreviation to be used on screens as field labels.  */
}SEAL_REC;
#ifdef KEY_SRC
struct keydesc sl_key1= {
         ISNODUPS,
         1,
         {0,INTSIZE,INTTYPE},
};
#else
extern struct keydesc sl_key1;
#endif
#endif
