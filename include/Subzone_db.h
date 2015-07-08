/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Subzone_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:33 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Subzone_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _SUBZONE_DB_H
#define _SUBZONE_DB_H
#include <isam.h>
typedef struct  cisam_sz {
       char  nbr[6];                     /* A unique subzone number.       */ 
       short zone_fleet1;               /* Corresponding zone number     for fleet 1.                   */
       short zone_fleet2;               /* Corresponding zone number for for fleet 2.                   */
       short zone_fleet3;               /* Corresponding zone number for fleet 3.                   */
       short zone_fleet4;               /* Corresponding zone number for fleet 4.                   */
       short zone_fleet5;               /* Corresponding zone number for fleet 5.                   */
       short zone_fleet6;               /* Corresponding zone number for fleet 6.                   */
       short zone_fleet7;               /* Corresponding zone number for fleet 7.                   */
       short zone_fleet8;               /* Corresponding zone number for fleet 8.                   */
} SUBZONE_REC;
#ifdef KEY_SRC
struct keydesc sz_key1= {
         ISNODUPS,
         1,
         {0,5,CHARTYPE},
};
#else
extern struct keydesc sz_key1;
#endif
#endif
