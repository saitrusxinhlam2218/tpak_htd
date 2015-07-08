/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: taxi_defs.h,v $
*  @(#)  $Revision: 1.6 $
*  @(#)  $Date: 1998/11/09 09:35:03 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/taxi_defs.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
/*
*
*  taxi_defs.h
*
*  Basic TaxiPak appliction definitions
*/
#ifndef _TAXI_DEFS_H_
#define _TAXI_DEFS_H_
#define TIME_LEN	6
#ifndef DATE_LEN
#define DATE_LEN        9
#endif
#define NAME_LEN	20	/* For names of people */
#define ATTR_MAX 	32	/* maximum number of attributes */
#define NUM_ATTR        ATTR_MAX /* for compatability */
#define LEVEL_MAX       9
#define NUM_SEALS	16
#define MAX_CANM	999	/* Maximum number of canned messages */
#define EXC_GROUPS	12	/* Number of exception groups defined */
#endif		/* end #ifndef _TAXI_DEFS_H_ */
