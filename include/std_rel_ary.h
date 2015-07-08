/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: std_rel_ary.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1996/09/06 13:50:57 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/std_rel_ary.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
struct scr_flds rel_scr_flds[] = {
  { 0, 0, 1, 0 },
  { 5, D_TRANS_CAT_COL, MC_TRANS_CAT, 1 },
  { 5, D_REL_NBR_COL, MC_REL_NBR, 1 },
  { 5, D_PCKUP_ZONE_COL, MC_ZONE, 1 },
  { 5, D_PCKUP_STRNAME_COL, MC_STRNAME, 1 },
  { 5, D_PCKUP_STRNBR_COL, MC_STRNBR, 1 },
  { 8, D_DEST_ZONE_COL, MC_ZONE, 1 },
  { 8, D_DEST_STRNAME_COL, MC_STRNAME, 1 },
  { 8, D_DEST_STRNBR_COL, MC_STRNBR, 1 },
  { 10, D_FLAT_RATE_COL, MC_FLAT_RATE, 1 }
};
