/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: std_rel_maint.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1996/09/06 13:51:07 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/std_rel_maint.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#define		HDR_ROW_1		0
#define		HDR_COL_1		0
#define		HDR_ROW_2		1
#define		HDR_COL_2		56
#define		REQ_ROW			0
#define		REQ_COL			54
#define		DESC_ROW		22
#define		DESC_COL		0
#define		ERROR_ROW		23
#define		ERROR_COL		0
#define		DEL_ROW			0
#define		DEL_COL			0
#define         D_TRANS_CAT_COL         9
#define         D_REL_NBR_COL           26
#define         D_PCKUP_ZONE_COL        43
#define         D_PCKUP_STRNAME_COL     55
#define         D_PCKUP_STRNBR_COL      89
#define         D_DEST_ZONE_COL         43
#define         D_DEST_STRNAME_COL      55
#define         D_DEST_STRNBR_COL       89
#define         D_FLAT_RATE_COL         43
#define         MC_TRANS_CAT            2
#define         MC_REL_NBR              3
#define         MC_ZONE                 3
#define         MC_STRNAME              20
#define         MC_STRNBR               3
#define         MC_FLAT_RATE            6
#define         F_TRANS_CAT             1
#define         F_REL_NBR               (F_TRANS_CAT + 1)
#define         F_PCKUP_ZONE            (F_REL_NBR + 1)
#define         F_PCKUP_STRNAME         (F_PCKUP_ZONE + 1)
#define         F_PCKUP_STRNBR          (F_PCKUP_STRNAME + 1)
#define         F_DEST_ZONE             (F_PCKUP_STRNBR + 1)
#define         F_DEST_STRNAME          (F_DEST_ZONE + 1)
#define         F_DEST_STRNBR           (F_DEST_STRNAME + 1)
#define         F_FLAT_RATE             (F_DEST_STRNBR + 1)
#define         NUM_FLDS                F_FLAT_RATE
#define         BUFFER_SIZE             133
#define         VALID                   1
#define         NOT_VALID               -1
#define         CR                      015
#define         NEW_LINE                012
#define         F_NAME_SIZE             15
#define         REL_FILE               STD_REL_FILE
#define         REL_FILE_ID            STD_REL_FILE_ID
