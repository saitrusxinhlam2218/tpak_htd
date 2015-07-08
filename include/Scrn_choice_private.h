/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Scrn_choice_private.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/05/12 19:20:44 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Scrn_choice_private.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef Scrn_choice_private_H
#define Scrn_choice_private_H
#define	SCRN_OBJECT_ID				500
#define	SCRN_CW_COLUMNS				132
#define	SCRN_CW_MAX_CHOICES	SCRN_CW_MAX_ROWS * SCRN_CW_MAX_COLUMNS
#define	SCRN_TWO_COL_MAX_STRING_LEN (SCRN_CW_MAX_STRING_LEN - 4) / 2 
#define	SCRN_THREE_COL_MAX_STRING_LEN (SCRN_CW_MAX_STRING_LEN - 8) / 3
#define	SCRN_CW_MAX_COLUMNS				3
#define	SCRN_CW_ROWS					5
#define	SCRN_CW_TOPLINE				18
#define	SCRN_CW_LEFTCOL				0
#define SCRN_CW_COL_1                                 1
#define SCRN_CW_COL2_2                                64
#define SCRN_CW_COL3_2                                40
#define SCRN_CW_COL_3                                 80
 
#define SCRN_CW_FIELD_ROW                             0
#define SCRN_CW_FIELD_COL                             15
#define SCRN_CW_FIELD_CHARS                           2
#define SCRN_CW_FIELD_PIU                             1
 
#endif      // #ifndef Scrn_choice_private_H
