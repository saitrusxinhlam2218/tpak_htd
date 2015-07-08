/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Street_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:28 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Street_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _STREET_DB_H
#define _STREET_DB_H
#include <isam.h>
typedef struct cisam_sn  {
       int    record_nbr;		  /* System assigned for redundancy only */
       int    index;                      /* Index # identifier to correspond to the name.  Used to correlate to the address file */
       char   rec_type;                   /* Type of NAME record.  Choices: */
					  /*    1) Main record. */
                                          /*    2) Abbreviation. */
                                          /*    3) Alias name.  */
       char   name[21];                  /* Street name, street abbreviation, street alias name or place name.  */
       char   str_type[4];               /* Street type (AVE. ST. ) etc.      */
} STREET_NAME_REC;
#ifdef KEY_SRC
struct keydesc sn_key1= {
         ISDUPS,
         2,
				 { {4,LONGSIZE,LONGTYPE},
					 {8,CHARSIZE,CHARTYPE} }
};
/* Composite of (name,str_type) */
struct keydesc sn_key2= {
         ISDUPS,
         2,
				 { {9,20,CHARTYPE},
					 {29,3,CHARTYPE} }
};
/* record_nbr system assigned for redundancy only */
struct keydesc sn_key3= {
         ISNODUPS,
         1,
         {0,LONGSIZE,LONGTYPE},
};
#else
extern struct keydesc sn_key1;
extern struct keydesc sn_key2;
extern struct keydesc sn_key3;
#endif
#endif
