/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Excpt_type_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:43:09 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Excpt_type_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _EXCPT_TYPE_DB_H
#define _EXCPT_TYPE_DB_H
#include <isam.h>
typedef struct     cisam_et    {
  short   nbr;                       /* A permanent # corresponding to the internal 4 byte exception */
  /* structure(32 bits corresponding to a max of 32 excep types). */
  short    group_nbr;                 /* Exception summary group this exception belongs to.  */
  char     desc[31];                  /* Description of this execep  */
  char     short_desc[9];             /* short exception description */
} EXCEPT_TYPE_REC;
#ifdef KEY_SRC
struct keydesc et_key1= {
         ISNODUPS,
         1,
				 { {0,INTSIZE,INTTYPE} }
};
/* short_desc */
struct keydesc et_key2= {
         ISDUPS,
         1,
				 { {34,8,CHARTYPE} }
};
#else
extern struct keydesc et_key1;
extern struct keydesc et_key2;
#endif
#endif
