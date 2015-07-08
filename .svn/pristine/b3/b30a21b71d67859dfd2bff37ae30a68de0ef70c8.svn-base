/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Place_db.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2004/08/09 13:56:46 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Place_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _PLACE_DB_H
#define _PLACE_DB_H
#include <isam.h>
typedef struct  cisam_pl  {
  int	  record_nbr;	     /* Unique record-ID filled in by Informix        */
  char    name[21];          /* Place name                                    */
  char    str_name[21];      /* Street name                                   */
  int     index;             /* A system assigned number from the street name file. */
  char    str_pre_dir[3];    /* Street pre_direction                          */
  char    str_type[4];       /* Street Type                                   */
  char    str_post_dir[3];   /* Street post_dirction.                         */
  char    str_city[4];       /* City code.                                    */
  char    odd_even;          /* Odd/Even block face flag. E-even, O-odd, BLANK-both.  */
  char    str_nbr_suf;       /* An alpha suffix that follows street number to accomdate addresses such as 10226A, In addition */
                             /*  suffixes X,Y and Z are reserved for represent of fractional address. X-1/4,Y-1/2,Z-3/4.  */
  int     street_nbr;        /* Street address number.                        */
  int     str_zip;           /* Zip code associated with this address or address stretch. */
  char    str_subzone[6];    /* Subzone associated with this address or address stretch. */
  int     str_x_coord;       /* X coordinate of the street.                   */
  int     str_y_coord;       /* Y coordinate of the street.                   */
  short   map_ref_page;      /* Map page reference number.                    */
  char    map_ref_block[5];  /* Map block designation (E.G. F3).              */
  char    str_comment[61];   /* Location comment for this street record.      */
  char    local;             /* "Y"/"N", whether this place name implies a local pickup. */
 } PLACENAME_REC;
#ifdef KEY_SRC
struct keydesc pl_key1= {
         ISNODUPS,
         2,
				 { {4,20,CHARTYPE},
					 {55,3,CHARTYPE} }
};
/* Composite of (index,str_pre_dir,str_post_dir,str_city,street_nbr) */
struct keydesc pl_key2= {
         ISDUPS,
         5,
				 { {44,LONGSIZE,LONGTYPE},
					 {48,2,CHARTYPE},
					 {53,2,CHARTYPE},
					 {55,3,CHARTYPE},
					 {60,LONGSIZE,LONGTYPE} }
};
/* str_record_nbr */
struct keydesc pl_key3 =  {
	 ISNODUPS,
	 1,
         {0, LONGSIZE, LONGTYPE}
};
#else
extern struct keydesc pl_key1;
extern struct keydesc pl_key2;
extern struct keydesc pl_key3;
#endif
#endif
