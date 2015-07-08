/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Address_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:42:22 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Address_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

#ifndef _ADDRESS_DB_H
#define _ADDRESS_DB_H

typedef struct    cisam_ad 
{
  int    record_nbr;	      /* Unique record-ID filled in by Informix */
  int   str_index;            /* A system assigned number
				 from the street name file.           */
  char  str_pre_dir[3];       /* Street pre-direction.                */
  char  str_name[21];         /* Street name.                         */
  char  str_type[4];          /* Street type.                         */
  char  str_post_dir[3];      /* Street post-direction.               */
  char  str_city[4];          /* City code.                           */
  char  odd_even;             /* Odd/Even block face flag.  E-even, O-odd, BLANK-both. */
  int   high_nbr;             /* High number of the street section.  If a specific address then high_number */
  /* and low_number must be the same. */
  int  low_nbr;               /*  Low number of the street section. If a specific address then high_number */
  /*  and low_number must be the same     */
  int str_zip;                /*  Zip code associated with this address or address stretch */
  char str_subzone[6];        /*  Subzone associated with this address or address stretch */
  int  str_x_coord;           /*  X coordinate of the street          */
  int  str_y_coord;           /*  Y coordinate of the street          */
  short  map_ref_page;        /*  Map page reference number.          */
  char place_names;           /*  "Y/N" whether place names exist for address. */
  char  map_ref_block[5];     /*  Map block designation (E.g. F3).    */
  char  str_comment[31];      /*  Location comment for this street record. */
  char  ct_comment[31];       /** Call taker comment for display only */
  char  district[21];
} ADDRESS_REC;

#ifdef KEY_SRC
struct keydesc ad_key1=  {
	 ISNODUPS,
	 5,
	 {     {4,LONGSIZE,LONGTYPE},
         {8,2,CHARTYPE},
         {33,2,CHARTYPE},
         {35,3,CHARTYPE},
				 {39,LONGSIZE,LONGTYPE} }
};

/* Composite of (str_index,high_nbr) */
struct keydesc ad_key2= {
         ISDUPS,
         2,
				 { {4,LONGSIZE,LONGTYPE},
					 {39,LONGSIZE,LONGTYPE} }
};

/* str_record_nbr */
struct keydesc ad_key3 =  {
	 ISNODUPS,
	 1,
	 { {0, LONGSIZE, LONGTYPE} }
};
#else
extern struct keydesc ad_key1;
extern struct keydesc ad_key2;
extern struct keydesc ad_key3;
#endif

#endif
