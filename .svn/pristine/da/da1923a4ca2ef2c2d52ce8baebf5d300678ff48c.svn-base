/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Attribute_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:42:23 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Attribute_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

#ifndef _ATTRIBUTE_DB_H
#define _ATTRIBUTE_DB_H

typedef struct    cisam_at  
 {
        char   fleet;                     /* Fleet indicator.              */
        char   type;                      /* Either Vehicle or Driver attribute.                    */
				short  nbr;                       /* Attribute number.             */
        char   desc[21];                  /* Attribute description.        */
        char   abbrev[3];                 /* Attribute abbreviation as to be seen on the screen.     */
	float  factor_value;
	short  extra_value;
 } ATTRIB_REC;

#ifdef KEY_SRC

struct keydesc at_key1= {
  ISNODUPS,
  3,
  { {0,CHARSIZE,CHARTYPE},
      {1,CHARSIZE,CHARTYPE},
      {2,INTSIZE,INTTYPE} }
};

#else
extern struct keydesc at_key1;
#endif

#endif
