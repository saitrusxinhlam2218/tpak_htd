/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: seasons_maint.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1996/09/20 12:45:49 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/seasons_maint.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#define		HDR_ROW_1	0
#define		HDR_COL_1	0
#define		HDR_ROW_2	1
#define		HDR_COL_2	56	
#define		REQ_ROW		0
#define		REQ_COL		54
#define		DESC_ROW		22
#define		DESC_COL		0
#define		ERROR_ROW	23
#define		ERROR_COL	0
#define		ERROR_COL1	70
#define		DEL_ROW		0
#define		DEL_COL		0
#define         D_START_DT_COL  9
#define         D_END_DT_COL    28
#define         D_DESCRIPT_COL  47
#define         D_FV_COL        68
#define         D_EV_COL        75
#define         MC_START_DT     8
#define         MC_END_DT       8
#define         MC_DESCRIPT     16
#define         MC_FV           4
#define         MC_EV           4
#define         F_START_DT      1
#define         F_END_DT        ( F_START_DT + 1 )
#define         F_DESCRIPT      ( F_END_DT + 1 )
#define         F_FV            ( F_DESCRIPT + 1)
#define         F_EV            ( F_FV + 1 )
#define         NUM_FLDS        (F_EV)
#define         MAX_ON_PG       5
#define         BUFFER_SIZE     133
#define         VALID           1
#define         NOT_VALID       -1
#define         CR              015
#define         NEW_LINE        012
#define         F_NAME_SIZE     15
