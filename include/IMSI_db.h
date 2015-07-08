/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: IMSI_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2005/01/11 14:06:45 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/IMSI_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _IMSI_DB_H
#define _IMSI_DB_H
#include <isam.h>
#define VH_IMSI_FLEET                0
#define VH_IMSI_NBR                  1  
#define VH_IMSI_IMSI                 3
#define IMSI_SIZE 32
#define IMSI_TELE_SIZE 32
typedef struct cisam_imsi {
  char fleet;
  short nbr;
  char imsi[IMSI_SIZE + 1];
  char telephone[IMSI_TELE_SIZE + 1];
} IMSI_REC;
#ifdef KEY_SRC
/* Composite of (fleet,nbr) */
struct keydesc imsi_key1=  {
	 ISNODUPS,
	 2,
	 {     {VH_IMSI_FLEET,CHARSIZE,CHARTYPE},
	       {VH_IMSI_NBR,INTSIZE,INTTYPE} }
};
/* Composite of (fleet,imsi_nbr) */
struct keydesc imsi_key2 = {
  ISNODUPS,
  2,
  { {VH_IMSI_FLEET,CHARSIZE,CHARTYPE},
    {VH_IMSI_IMSI, IMSI_SIZE, CHARTYPE} }
};
#else
extern struct keydesc imsi_key1;
extern struct keydesc imsi_key2;
#endif
#endif
