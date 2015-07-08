/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Line_Manager_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2001/08/14 13:42:48 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Line_Manager_db.h,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Line_Manager_db.h,v $	$Revision: 1.2 $"

#include "Line_Manager_private.h"

#ifndef _LINE_MGR_DB_H
#define _LINE_MGR_DB_H

typedef struct _LINE_MGR_DB_REC
{
  short              DDS_index;
  char               DDS_Server[SERVER_NAME_SZ + 1];
  char               DDS_Name[CSTA_OBJ_SZ + 1];
  char               DDS_Descript[32];
} LINE_MGR_DB_REC;

#define TINFO_ACCT_LEN  18
#define TINFO_DATE_LEN  9
#define TINFO_TIME_LEN  6
#define TINFO_PHONE_LEN 18
#define TINFO_G7_CARD_LEN 19

typedef struct _TAOINFO_REC
{
  int                nbr;
  char               acct[ TINFO_ACCT_LEN + 1 ];
  char               tc_flag;
  char               date[ TINFO_DATE_LEN + 1 ];
  char               time[ TINFO_TIME_LEN + 1 ];
  int                due_dttm;
  char               phone[ TINFO_PHONE_LEN + 1 ];
  char               g7_card[ TINFO_G7_CARD_LEN + 1 ];
  int                enter_dttm;
} TAOINFO_REC;
  
#include <isam.h>
#ifdef KEY_SRC

#define LINE_MGR_KEY_MAX 1
struct keydesc   line_mgr_key1 =
{
  ISNODUPS, 1,
  { 2, 32, CHARTYPE }
};

struct keydesc *Line_mgr_keys[ ] =
{
   &line_mgr_key1
 };

struct keydesc taoinfo_key1 =
{
  ISNODUPS, 1,
  { 0, 4, LONGTYPE }
};

#else
extern struct keydesc *Line_mgr_keys[];
extern struct keydesc line_mgr_key1;
extern struct keydesc taoinfo_key1;
#endif
#endif


