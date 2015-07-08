/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Intersect_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:43:36 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Intersect_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _INTERSECT_DB_H
#define _INTERSECT_DB_H
#include <isam.h>
typedef struct cisam_in  {
      int    index1;                        /* A system assigned for the second street name from the street name file.          */
      int    index2;                        /* A system assigned for the second street name from the street name file.          */
      char   pre_dir1[3];                   /* Intersection pre-direction */
      char   name1[21];                     /* Intersection street name.  */
      char   type1[4];                      /* Intersection street type.  */
      char   post_dir1[3];                  /* Intersection post-direction*/
      char   pre_dir2[3];                   /* Intersection pre-direction */
      char   name2[21];                     /* Intersection street name.  */
      char   type2[4];                      /* Intersection street type.  */
      char   post_dir2[3];                  /* Intersection post-direction*/
      int    zip;                           /* Intersection zip code.     */
      char   subzone[6];                    /* 1-9999.  Intersection subzone.                */
      char   city[4];                       /* Intersection city.         */
      int    str_x_coord;                   /* X coordinate of the street */
      int    str_y_coord;                   /* Y coordinate of the street */
      short  map_ref_page;                  /* Map page reference number  */
      char   comment[31];                   /* Intersection location cocmment */
      char   map_ref_block[5];              /* Map block designation(E.G F3)  */
  }INTERSECT_REC;
#ifdef KEY_SRC
struct keydesc in_key1 = {
         ISNODUPS,
         2,
				 { {0, LONGSIZE, LONGTYPE},
					 {4, LONGSIZE, LONGTYPE} }
};
#else
extern struct keydesc in_key1;
#endif
#endif
