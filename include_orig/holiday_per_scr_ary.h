/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: holiday_per_scr_ary.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1996/09/06 13:48:18 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/holiday_per_scr_ary.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: holiday_per_scr_ary.h,v $	$Revision: 1.2 $"

struct scr_flds hol_scr_flds[] = {
  { 0, 0, 1, 0 },
  { 5, D_YR_COL, MC_YR, 1 },
  { 5, D_MONTH_COL, MC_MONTH, 1 },
  { 5, D_DAY_COL, MC_DAY, 1 },
  { 5, D_DESCRIPT_COL, MC_DESCRIPT, 1 }
};
