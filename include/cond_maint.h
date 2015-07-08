/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: cond_maint.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1996/09/06 13:46:40 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/cond_maint.h,v $
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
#define         D_TC_COL                9
#define         D_NBR_COL               15
#define         D_CAT_COL               25
#define         D_DESCRIPT_COL          35
#define         D_FV_COL                75
#define         D_EV_COL                85
#define         MC_TC                   2
#define         MC_NBR                  3
#define         MC_CAT                  1
#define         MC_DESCRIPT             32
#define         MC_FV                   3
#define         MC_EV                   3
#define         F_TC                    1
#define         F_NBR                   (F_TC + 1)
#define         F_CAT                   (F_NBR + 1)
#define         F_DESCRIPT              (F_CAT + 1)
#define         F_FV                    (F_DESCRIPT + 1)
#define         F_EV                    (F_FV + 1)
#define         NUM_FLDS                F_EV
#define         BUFFER_SIZE             133
#define         VALID                   1
#define         NOT_VALID               -1
#define         CR                      015
#define         NEW_LINE                012
#define         F_NAME_SIZE             15
