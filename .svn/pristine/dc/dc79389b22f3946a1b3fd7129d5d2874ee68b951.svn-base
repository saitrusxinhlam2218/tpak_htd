/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: attr_select.c,v $
*  @(#)  $Revision: 1.9 $
*  @(#)  $Date: 2001/12/07 17:08:42 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/ui/attr_select.c,v $
*
*  Copyright (c) 2001 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

#include <cursesX.h>
#include <sys/types.h>

#include "taxipak.h"
#include "taxi_defs.h"
#include "mads_types.h"
#include "Object.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Call_public.h"
#include "Call_private.h"
#include "Scrn.h"
#include "screens.h"
#include "call_entry.h"
#include "language.h"
#include "func_keys.h"
#include "mads_types.h"
#include "ui_ipc.h"
#include "ui_strdefs.h"
#include "writer.h"
#include "taxi_db.h"
#include "except.h"
#include "tokenize.h"
#include "ui_def.h"
#include "path.h"
#include "enhance.h"

#define ATTR_LINE_ROW   1
#define ATTR_SHORT_COL  1
#define ATTR_LONG_COL   6

extern struct cisam_cl call_struct;			/* for disk i/o */
extern struct fleets *fleet[];
extern int      cur_fl_index;
extern int    scr_name;

typedef struct _ATTR_SELECT_ARRAY
{
  char      attr_short[3];
  char      attr_long[21];
  char      vh_drv;
  int       attr_nbr;
} ATTR_SELECT_ARRAY;

RET_STATUS attr_scr_background();
RET_STATUS attr_scr_init();
RET_STATUS attr_scr_pre_key();
RET_STATUS attr_scr_post_key();
RET_STATUS attr_scr_exit();
RET_STATUS highlight_chosen_attributes();
RET_STATUS function_key_basics();
//RET_STATUS attr_scr_verify();
RET_STATUS attr_scr_verify( SCRN *, char *, int, BOOLEAN );
RET_STATUS CheckAttrClass(char, int, char);

#define F_ATTR1_SHORT   1
#define F_ATTR1_LONG    2
#define F_ATTR2_SHORT   3
#define F_ATTR2_LONG    4
#define F_ATTR3_SHORT   5
#define F_ATTR3_LONG    6
#define F_ATTR4_SHORT   7
#define F_ATTR4_LONG    8
#define F_ATTR5_SHORT   9
#define F_ATTR5_LONG    10
#define F_ATTR6_SHORT   11
#define F_ATTR6_LONG    12
#define F_ATTR7_SHORT   13
#define F_ATTR7_LONG    14
#define F_ATTR8_SHORT   15
#define F_ATTR8_LONG    16
#define F_ATTR9_SHORT   17
#define F_ATTR9_LONG    18
#define F_ATTR10_SHORT   19
#define F_ATTR10_LONG    20
#define F_ATTR11_SHORT   21
#define F_ATTR11_LONG    22
#define F_ATTR12_SHORT   23
#define F_ATTR12_LONG    24
#define F_ATTR13_SHORT   25
#define F_ATTR13_LONG    26
#define F_ATTR14_SHORT   27
#define F_ATTR14_LONG    28
#define F_ATTR15_SHORT   29
#define F_ATTR15_LONG    30
#define F_ATTR16_SHORT   31
#define F_ATTR16_LONG    32
#define F_ATTR17_SHORT   33
#define F_ATTR17_LONG    34
#define F_ATTR18_SHORT   35
#define F_ATTR18_LONG    36
#define F_ATTR19_SHORT   37
#define F_ATTR19_LONG    38
#define F_ATTR20_SHORT   39
#define F_ATTR20_LONG    40
#define F_ATTR21_SHORT   41
#define F_ATTR21_LONG    42
#define F_ATTR22_SHORT   43
#define F_ATTR22_LONG    44
#define F_ATTR23_SHORT   45
#define F_ATTR23_LONG    46
#define F_ATTR24_SHORT   47
#define F_ATTR24_LONG    48
#define F_ATTR25_SHORT   49
#define F_ATTR25_LONG    50
#define F_ATTR26_SHORT   51
#define F_ATTR26_LONG    52
#define F_ATTR27_SHORT   53
#define F_ATTR27_LONG    54
#define F_ATTR28_SHORT   55
#define F_ATTR28_LONG    56
#define F_ATTR29_SHORT   57
#define F_ATTR29_LONG    58
#define F_ATTR30_SHORT   59
#define F_ATTR30_LONG    60
#define F_ATTR31_SHORT   61
#define F_ATTR31_LONG    62
#define F_ATTR32_SHORT   63
#define F_ATTR32_LONG    64
#define F_ATTR33_SHORT   65
#define F_ATTR33_LONG    66
#define F_ATTR34_SHORT   67
#define F_ATTR34_LONG    68
#define F_ATTR35_SHORT   69
#define F_ATTR35_LONG    70
#define F_ATTR36_SHORT   71
#define F_ATTR36_LONG    72
#define F_ATTR37_SHORT   73
#define F_ATTR37_LONG    74
#define F_ATTR38_SHORT   75
#define F_ATTR38_LONG    76
#define F_ATTR39_SHORT   77
#define F_ATTR39_LONG    78
#define F_ATTR40_SHORT   79
#define F_ATTR40_LONG    80
#define F_ATTR41_SHORT   81
#define F_ATTR41_LONG    82
#define F_ATTR42_SHORT   83
#define F_ATTR42_LONG    84
#define F_ATTR43_SHORT   85
#define F_ATTR43_LONG    86
#define F_ATTR44_SHORT   87
#define F_ATTR44_LONG    88
#define F_ATTR45_SHORT   89
#define F_ATTR45_LONG    90
#define F_ATTR46_SHORT   91
#define F_ATTR46_LONG    92
#define F_ATTR47_SHORT   93
#define F_ATTR47_LONG    94
#define F_ATTR48_SHORT   95
#define F_ATTR48_LONG    96
#define F_ATTR49_SHORT   97
#define F_ATTR49_LONG    98
#define F_ATTR50_SHORT   99
#define F_ATTR50_LONG    100
#define F_ATTR51_SHORT   101
#define F_ATTR51_LONG    102
#define F_ATTR52_SHORT   103
#define F_ATTR52_LONG    104
#define F_ATTR53_SHORT   105
#define F_ATTR53_LONG    106
#define F_ATTR54_SHORT   107
#define F_ATTR54_LONG    108
#define F_ATTR55_SHORT   109
#define F_ATTR55_LONG    110
#define F_ATTR56_SHORT   111
#define F_ATTR56_LONG    112
#define F_ATTR57_SHORT   113
#define F_ATTR57_LONG    114
#define F_ATTR58_SHORT   115
#define F_ATTR58_LONG    116
#define F_ATTR59_SHORT   117
#define F_ATTR59_LONG    118
#define F_ATTR60_SHORT   119
#define F_ATTR60_LONG    120
#define F_ATTR61_SHORT   121
#define F_ATTR61_LONG    122
#define F_ATTR62_SHORT   123
#define F_ATTR62_LONG    124
#define F_ATTR63_SHORT   125
#define F_ATTR63_LONG    126
#define F_ATTR64_SHORT   127
#define F_ATTR64_LONG    128
#define F_ATTR65_SHORT   129
#define F_ATTR65_LONG    130
#define F_ATTR66_SHORT   131
#define F_ATTR66_LONG    132

#define F_FIELDS_MAX     F_ATTR66_LONG

#define ATTR_SCR_ENTRIES_MAX 64

#define ATTR_SHORT_LEN  2
//#define ATTR_LONG_LEN   20
#define ATTR_LONG_LEN   1

#define ATTR_ENTRY_SCR  99
ATTR_SELECT_ARRAY   AttrSel[ATTR_MAX * 3];

FIELD Attr_scr1_fields[] =
{
  {F_ATTR1_SHORT, ATTR_LINE_ROW, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[0].attr_short, NULL},
  {F_ATTR1_LONG, ATTR_LINE_ROW, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR12_LONG, "F_ATTR1_LONG", AttrSel[0].attr_long, attr_scr_verify},
  
  {F_ATTR2_SHORT, ATTR_LINE_ROW + 1, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[1].attr_short, NULL},
  {F_ATTR2_LONG, ATTR_LINE_ROW + 1, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR13_LONG, "F_ATTR1_LONG", AttrSel[1].attr_long, attr_scr_verify},
  
  {F_ATTR3_SHORT, ATTR_LINE_ROW + 2, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[2].attr_short, NULL},
  {F_ATTR3_LONG, ATTR_LINE_ROW + 2, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR14_LONG, "F_ATTR1_LONG", AttrSel[2].attr_long, attr_scr_verify},
  
  {F_ATTR4_SHORT, ATTR_LINE_ROW + 3, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[3].attr_short, NULL},
  {F_ATTR4_LONG, ATTR_LINE_ROW + 3, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR15_LONG, "F_ATTR1_LONG", AttrSel[3].attr_long, attr_scr_verify},
  
  {F_ATTR5_SHORT, ATTR_LINE_ROW + 4, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[4].attr_short, NULL},
  {F_ATTR5_LONG, ATTR_LINE_ROW + 4, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR16_LONG, "F_ATTR1_LONG", AttrSel[4].attr_long, attr_scr_verify},
  
  {F_ATTR6_SHORT, ATTR_LINE_ROW + 5, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[5].attr_short, NULL},
  {F_ATTR6_LONG, ATTR_LINE_ROW + 5, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR17_LONG, "F_ATTR1_LONG", AttrSel[5].attr_long, attr_scr_verify},
  
  {F_ATTR7_SHORT, ATTR_LINE_ROW + 6, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[6].attr_short, NULL},
  {F_ATTR7_LONG, ATTR_LINE_ROW + 6, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR18_LONG, "F_ATTR1_LONG", AttrSel[6].attr_long, attr_scr_verify},
  
  {F_ATTR8_SHORT, ATTR_LINE_ROW + 7, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[7].attr_short, NULL},
  {F_ATTR8_LONG, ATTR_LINE_ROW + 7, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR19_LONG, "F_ATTR1_LONG", AttrSel[7].attr_long, attr_scr_verify},
  
  {F_ATTR9_SHORT, ATTR_LINE_ROW + 8, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[8].attr_short, NULL},
  {F_ATTR9_LONG, ATTR_LINE_ROW + 8, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR20_LONG, "F_ATTR1_LONG", AttrSel[8].attr_long, attr_scr_verify},
  
  {F_ATTR10_SHORT, ATTR_LINE_ROW + 9, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[9].attr_short, NULL},
  {F_ATTR10_LONG, ATTR_LINE_ROW + 9, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR21_LONG, "F_ATTR1_LONG", AttrSel[9].attr_long, attr_scr_verify},
  
  {F_ATTR11_SHORT, ATTR_LINE_ROW + 10, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[10].attr_short, NULL},
  {F_ATTR11_LONG, ATTR_LINE_ROW + 10, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR22_LONG, "F_ATTR1_LONG", AttrSel[10].attr_long, attr_scr_verify},
  
  {F_ATTR12_SHORT, ATTR_LINE_ROW, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[11].attr_short, NULL},
  {F_ATTR12_LONG, ATTR_LINE_ROW, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR23_LONG, "F_ATTR1_LONG", AttrSel[11].attr_long, attr_scr_verify},
  
  {F_ATTR13_SHORT, ATTR_LINE_ROW + 1, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[12].attr_short, NULL},
  {F_ATTR13_LONG, ATTR_LINE_ROW + 1, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR24_LONG, "F_ATTR1_LONG", AttrSel[12].attr_long, attr_scr_verify},
  
  {F_ATTR14_SHORT, ATTR_LINE_ROW + 2, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[13].attr_short, NULL},
  {F_ATTR14_LONG, ATTR_LINE_ROW + 2, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR25_LONG, "F_ATTR1_LONG", AttrSel[13].attr_long, attr_scr_verify},
  
  {F_ATTR15_SHORT, ATTR_LINE_ROW + 3, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[14].attr_short, NULL},
  {F_ATTR15_LONG, ATTR_LINE_ROW + 3, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR26_LONG, "F_ATTR1_LONG", AttrSel[14].attr_long, attr_scr_verify},
  
  {F_ATTR16_SHORT, ATTR_LINE_ROW + 4, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[15].attr_short, NULL},
  {F_ATTR16_LONG, ATTR_LINE_ROW + 4, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR27_LONG, "F_ATTR1_LONG", AttrSel[15].attr_long, attr_scr_verify},
  
  {F_ATTR17_SHORT, ATTR_LINE_ROW + 5, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[16].attr_short, NULL},
  {F_ATTR17_LONG, ATTR_LINE_ROW + 5, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR28_LONG, "F_ATTR1_LONG", AttrSel[16].attr_long, attr_scr_verify},
  
  {F_ATTR18_SHORT, ATTR_LINE_ROW + 6, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[17].attr_short, NULL},
  {F_ATTR18_LONG, ATTR_LINE_ROW + 6, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR29_LONG, "F_ATTR1_LONG", AttrSel[17].attr_long, attr_scr_verify},
  
  {F_ATTR19_SHORT, ATTR_LINE_ROW + 7, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[18].attr_short, NULL},
  {F_ATTR19_LONG, ATTR_LINE_ROW + 7, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR30_LONG, "F_ATTR1_LONG", AttrSel[18].attr_long, attr_scr_verify},
  
  {F_ATTR20_SHORT, ATTR_LINE_ROW + 8, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[19].attr_short, NULL},
  {F_ATTR20_LONG, ATTR_LINE_ROW + 8, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR31_LONG, "F_ATTR1_LONG", AttrSel[19].attr_long, attr_scr_verify},
  
  {F_ATTR21_SHORT, ATTR_LINE_ROW + 9, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[20].attr_short, NULL},
  {F_ATTR21_LONG, ATTR_LINE_ROW + 9, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR32_LONG, "F_ATTR1_LONG", AttrSel[20].attr_long, attr_scr_verify},
  
  {F_ATTR22_SHORT, ATTR_LINE_ROW + 10, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[21].attr_short, NULL},
  {F_ATTR22_LONG, ATTR_LINE_ROW + 10, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR33_LONG, "F_ATTR1_LONG", AttrSel[21].attr_long, attr_scr_verify},
  
  {F_ATTR23_SHORT, ATTR_LINE_ROW, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[22].attr_short, NULL},
  {F_ATTR23_LONG, ATTR_LINE_ROW, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR2_LONG, "F_ATTR1_LONG", AttrSel[22].attr_long, attr_scr_verify},
  
  {F_ATTR24_SHORT, ATTR_LINE_ROW + 1, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[23].attr_short, NULL},
  {F_ATTR24_LONG, ATTR_LINE_ROW + 1, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR3_LONG, "F_ATTR1_LONG", AttrSel[23].attr_long, attr_scr_verify},
  
  {F_ATTR25_SHORT, ATTR_LINE_ROW + 2, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[24].attr_short, NULL},
  {F_ATTR25_LONG, ATTR_LINE_ROW + 2, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR4_LONG, "F_ATTR1_LONG", AttrSel[24].attr_long, attr_scr_verify},
  
  {F_ATTR26_SHORT, ATTR_LINE_ROW + 3, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[25].attr_short, NULL},
  {F_ATTR26_LONG, ATTR_LINE_ROW + 3, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR5_LONG, "F_ATTR1_LONG", AttrSel[25].attr_long, attr_scr_verify},
  
  {F_ATTR27_SHORT, ATTR_LINE_ROW + 4, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[26].attr_short, NULL},
  {F_ATTR27_LONG, ATTR_LINE_ROW + 4, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR6_LONG, "F_ATTR1_LONG", AttrSel[26].attr_long, attr_scr_verify},
  
  {F_ATTR28_SHORT, ATTR_LINE_ROW + 5, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[27].attr_short, NULL},
  {F_ATTR28_LONG, ATTR_LINE_ROW + 5, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR7_LONG, "F_ATTR1_LONG", AttrSel[27].attr_long, attr_scr_verify},
  
  {F_ATTR29_SHORT, ATTR_LINE_ROW + 6, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[28].attr_short, NULL},
  {F_ATTR29_LONG, ATTR_LINE_ROW + 6, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR8_LONG, "F_ATTR1_LONG", AttrSel[28].attr_long, attr_scr_verify},
  
  {F_ATTR30_SHORT, ATTR_LINE_ROW + 7, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[29].attr_short, NULL},
  {F_ATTR30_LONG, ATTR_LINE_ROW + 7, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR9_LONG, "F_ATTR1_LONG", AttrSel[29].attr_long, attr_scr_verify},

  {F_ATTR31_SHORT, ATTR_LINE_ROW + 8, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[30].attr_short, NULL},
  {F_ATTR31_LONG, ATTR_LINE_ROW + 8, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR10_LONG, "F_ATTR1_LONG", AttrSel[30].attr_long, attr_scr_verify},

  {F_ATTR32_SHORT, ATTR_LINE_ROW + 9, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[31].attr_short, NULL},
  {F_ATTR32_LONG, ATTR_LINE_ROW + 9, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR11_LONG, "F_ATTR1_LONG", AttrSel[31].attr_long, attr_scr_verify},

  {F_ATTR33_SHORT, ATTR_LINE_ROW + 10, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_ATTR1_SHORT", AttrSel[32].attr_short, NULL},
  {F_ATTR33_LONG, ATTR_LINE_ROW + 10, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, SILENT, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[32].attr_long, attr_scr_verify},

  {F_ATTR34_SHORT, ATTR_LINE_ROW, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[33].attr_short, NULL},
  {F_ATTR34_LONG, ATTR_LINE_ROW, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[33].attr_long, attr_scr_verify},

  {F_ATTR35_SHORT, ATTR_LINE_ROW + 1, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[34].attr_short, NULL},
  {F_ATTR35_LONG, ATTR_LINE_ROW + 1, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[34].attr_long, attr_scr_verify},

  {F_ATTR36_SHORT, ATTR_LINE_ROW + 2, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[35].attr_short, NULL},
  {F_ATTR36_LONG, ATTR_LINE_ROW + 2, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[35].attr_long, attr_scr_verify},

  {F_ATTR37_SHORT, ATTR_LINE_ROW + 3, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[36].attr_short, NULL},
  {F_ATTR37_LONG, ATTR_LINE_ROW + 3, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[36].attr_long, attr_scr_verify},

  {F_ATTR38_SHORT, ATTR_LINE_ROW + 4, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[37].attr_short, NULL},
  {F_ATTR38_LONG, ATTR_LINE_ROW + 4, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[37].attr_long, attr_scr_verify},

  {F_ATTR39_SHORT, ATTR_LINE_ROW + 5, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[38].attr_short, NULL},
  {F_ATTR39_LONG, ATTR_LINE_ROW + 5, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[38].attr_long, attr_scr_verify},

  {F_ATTR40_SHORT, ATTR_LINE_ROW + 6, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[39].attr_short, NULL},
  {F_ATTR40_LONG, ATTR_LINE_ROW + 6, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[39].attr_long, attr_scr_verify},

  {F_ATTR41_SHORT, ATTR_LINE_ROW + 7, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[40].attr_short, NULL},
  {F_ATTR41_LONG, ATTR_LINE_ROW + 7, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[40].attr_long, attr_scr_verify},

  {F_ATTR42_SHORT, ATTR_LINE_ROW + 8, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[41].attr_short, NULL},
  {F_ATTR42_LONG, ATTR_LINE_ROW + 8, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[41].attr_long, attr_scr_verify},

  {F_ATTR43_SHORT, ATTR_LINE_ROW + 9, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[42].attr_short, NULL},
  {F_ATTR43_LONG, ATTR_LINE_ROW + 9, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[42].attr_long, attr_scr_verify},

  {F_ATTR44_SHORT, ATTR_LINE_ROW + 10, ATTR_SHORT_COL, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[43].attr_short, NULL},
  {F_ATTR44_LONG, ATTR_LINE_ROW + 10, ATTR_LONG_COL, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[43].attr_long, attr_scr_verify},

  {F_ATTR45_SHORT, ATTR_LINE_ROW, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[44].attr_short, NULL},
  {F_ATTR45_LONG, ATTR_LINE_ROW, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[44].attr_long, attr_scr_verify},

  {F_ATTR46_SHORT, ATTR_LINE_ROW + 1, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[45].attr_short, NULL},
  {F_ATTR46_LONG, ATTR_LINE_ROW + 1, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[45].attr_long, attr_scr_verify},

  {F_ATTR47_SHORT, ATTR_LINE_ROW + 2, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[46].attr_short, NULL},
  {F_ATTR47_LONG, ATTR_LINE_ROW + 2, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[46].attr_long, attr_scr_verify},

  {F_ATTR48_SHORT, ATTR_LINE_ROW + 3, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[47].attr_short, NULL},
  {F_ATTR48_LONG, ATTR_LINE_ROW + 3, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[47].attr_long, attr_scr_verify},

  {F_ATTR49_SHORT, ATTR_LINE_ROW + 4, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[48].attr_short, NULL},
  {F_ATTR49_LONG, ATTR_LINE_ROW + 4, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[48].attr_long, attr_scr_verify},

  {F_ATTR50_SHORT, ATTR_LINE_ROW + 5, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[49].attr_short, NULL},
  {F_ATTR50_LONG, ATTR_LINE_ROW + 5, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[49].attr_long, attr_scr_verify},

  {F_ATTR51_SHORT, ATTR_LINE_ROW + 6, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[50].attr_short, NULL},
  {F_ATTR51_LONG, ATTR_LINE_ROW + 6, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[50].attr_long, attr_scr_verify},

  {F_ATTR52_SHORT, ATTR_LINE_ROW + 7, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[51].attr_short, NULL},
  {F_ATTR52_LONG, ATTR_LINE_ROW + 7, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[51].attr_long, attr_scr_verify},

  {F_ATTR53_SHORT, ATTR_LINE_ROW + 8, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[52].attr_short, NULL},
  {F_ATTR53_LONG, ATTR_LINE_ROW + 8, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[52].attr_long, attr_scr_verify},

  {F_ATTR54_SHORT, ATTR_LINE_ROW + 9, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[53].attr_short, NULL},
  {F_ATTR54_LONG, ATTR_LINE_ROW + 9, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[53].attr_long, attr_scr_verify},

  {F_ATTR55_SHORT, ATTR_LINE_ROW + 10, ATTR_SHORT_COL + 30, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[54].attr_short, NULL},
  {F_ATTR55_LONG, ATTR_LINE_ROW + 10, ATTR_LONG_COL + 30, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[54].attr_long, attr_scr_verify},

  {F_ATTR56_SHORT, ATTR_LINE_ROW, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[55].attr_short, NULL},
  {F_ATTR56_LONG, ATTR_LINE_ROW, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[55].attr_long, attr_scr_verify},

  {F_ATTR57_SHORT, ATTR_LINE_ROW + 1, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[56].attr_short, NULL},
  {F_ATTR57_LONG, ATTR_LINE_ROW + 1, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[56].attr_long, attr_scr_verify},

  {F_ATTR58_SHORT, ATTR_LINE_ROW + 2, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[57].attr_short, NULL},
  {F_ATTR58_LONG, ATTR_LINE_ROW + 2, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[57].attr_long, attr_scr_verify},

  {F_ATTR59_SHORT, ATTR_LINE_ROW + 3, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[58].attr_short, NULL},
  {F_ATTR59_LONG, ATTR_LINE_ROW + 3, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[58].attr_long, attr_scr_verify},

  {F_ATTR60_SHORT, ATTR_LINE_ROW + 4, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[59].attr_short, NULL},
  {F_ATTR60_LONG, ATTR_LINE_ROW + 4, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[59].attr_long, attr_scr_verify},

  {F_ATTR61_SHORT, ATTR_LINE_ROW + 5, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[60].attr_short, NULL},
  {F_ATTR61_LONG, ATTR_LINE_ROW + 5, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[60].attr_long, attr_scr_verify},

  {F_ATTR62_SHORT, ATTR_LINE_ROW + 6, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[61].attr_short, NULL},
  {F_ATTR62_LONG, ATTR_LINE_ROW + 6, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[61].attr_long, attr_scr_verify},

  {F_ATTR63_SHORT, ATTR_LINE_ROW + 7, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[62].attr_short, NULL},
  {F_ATTR63_LONG, ATTR_LINE_ROW + 7, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[62].attr_long, attr_scr_verify},

  {F_ATTR64_SHORT, ATTR_LINE_ROW + 8, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[63].attr_short, NULL},
  {F_ATTR64_LONG, ATTR_LINE_ROW + 8, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[63].attr_long, attr_scr_verify},

  {F_ATTR65_SHORT, ATTR_LINE_ROW + 9, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[64].attr_short, NULL},
  {F_ATTR65_LONG, ATTR_LINE_ROW + 9, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[64].attr_long, attr_scr_verify},

  {F_ATTR66_SHORT, ATTR_LINE_ROW + 10, ATTR_SHORT_COL + 60, ATTR_SHORT_LEN, UPPER, "%2s - ", STRING_TYPE, LEFT, TURNED_OFF, 0, 0, "F_ATTR1_SHORT", AttrSel[65].attr_short, NULL},
  {F_ATTR66_LONG, ATTR_LINE_ROW + 10, ATTR_LONG_COL + 60, ATTR_LONG_LEN, UPPER | NO_UNDERLINE |  EXIT_AT_END, "%-20s", STRING_TYPE, LEFT, TURNED_OFF, 0, F_ATTR1_LONG, "F_ATTR1_LONG", AttrSel[65].attr_long, attr_scr_verify},
  
  
#include "nu_end_field.h"
};


SCRN Attr_entry =
{
  //F_FIELDS_MAX
  F_FIELDS_MAX, 1, 0, ATTR_ENTRY_SCR, NULL,
  attr_scr_background, attr_scr_init, attr_scr_pre_key,
  attr_scr_post_key, attr_scr_exit,
  Attr_scr1_fields, AttrSel, highlight_chosen_attributes
};

#define ATTR_SET_ROW 1
#define ATTR_SET_COL 2
#define ATTR_SET_CHARS 1

#define FIRST_PAGE 0
#define FORWARD_PAGE 1
#define BACK_PAGE  2

short LastAttrNbr;



#define ATTR_WIN_ROWS  14
#define ATTR_WIN_COLS  100
#define ATTR_SCR_ROW   4
#define ATTR_SCR_COL   5


alt_attr()
{
  Attr_entry.window = newwin( ATTR_WIN_ROWS, ATTR_WIN_COLS, ATTR_SCR_ROW, ATTR_SCR_COL );  
  Scrn_process( &Attr_entry );
  return(SUCCESS);
}

RET_STATUS
attr_scr_init(SCRN *screen)
{
  ATTR_SELECT_ARRAY *attrsel;
  int i, field_count;
  int rs;


  for ( i = 0; i < 64; i++ )
    bzero( &AttrSel[i], sizeof(ATTR_SELECT_ARRAY) );
  
  box(screen->window, 0, 0);
  
  Scrn_default_field_use_all(screen);
  wattrset( screen->window, 0 );
  field_count = 0;
  for ( i = 0; i < 32; i++ )
    {
      if ( ( fleet[cur_fl_index]->vd_attr_info[i].veh_attr[0] != EOS ) )
        {
          ++field_count;
          if ( field_count > ATTR_SCR_ENTRIES_MAX )
            break;
          attrsel = (ATTR_SELECT_ARRAY *)screen->field[(F_ATTR1_SHORT + (field_count - 1)*2) - 1].data;
          attrsel->vh_drv = C_VEHICLE;
          attrsel->attr_nbr = i;          
          if ( call_struct.veh_attr[i] == YES )
            {
              screen->field[(F_ATTR1_SHORT + (field_count - 1)*2) - 1].flags = DISPLAY_ONLY | REVERSE_TEXT;
              //              screen->field[(F_ATTR1_LONG + (field_count - 1)*2) - 1].flags |= REVERSE_TEXT;
            }
          else
            {
              screen->field[(F_ATTR1_SHORT + (field_count - 1)*2) - 1].flags = DISPLAY_ONLY;
            }
          Scrn_init_string_field( screen, F_ATTR1_SHORT + (field_count - 1)*2, fleet[cur_fl_index]->vd_attr_info[i].veh_attr );
          Scrn_print_field_by_num( screen, F_ATTR1_SHORT + (field_count - 1)*2 );
          Scrn_init_string_field( screen, F_ATTR1_LONG + (field_count - 1)*2, fleet[cur_fl_index]->vd_attr_info[i].veh_at_desc );
          Scrn_print_field_by_num( screen, F_ATTR1_LONG + (field_count -1)*2 );

          

        }
      if ( ( fleet[cur_fl_index]->vd_attr_info[i].drv_attr[0] != EOS ) )
        {
          ++field_count;
          if ( field_count > ATTR_SCR_ENTRIES_MAX )
            break;
          attrsel = (ATTR_SELECT_ARRAY *)screen->field[(F_ATTR1_SHORT + (field_count - 1)*2) - 1].data;          
          attrsel->vh_drv = C_DRIVER;
          attrsel->attr_nbr = i;                    
          if ( call_struct.drv_attr[i] == YES )
            {
              screen->field[(F_ATTR1_SHORT + (field_count - 1)*2)-1].flags = DISPLAY_ONLY | REVERSE_TEXT;
              //              screen->field[(F_ATTR1_LONG + (field_count - 1)*2) - 1].flags |= REVERSE_TEXT;              
            }
          else
            {
              screen->field[(F_ATTR1_SHORT + (field_count - 1)*2)-1].flags = DISPLAY_ONLY;
            }          
          Scrn_init_string_field( screen, F_ATTR1_SHORT + (field_count - 1)*2, fleet[cur_fl_index]->vd_attr_info[i].drv_attr );
          Scrn_print_field_by_num( screen, F_ATTR1_SHORT + (field_count - 1)*2 );          
          Scrn_init_string_field( screen, F_ATTR1_LONG + (field_count - 1)*2, fleet[cur_fl_index]->vd_attr_info[i].drv_at_desc );
          Scrn_print_field_by_num( screen, F_ATTR1_LONG + (field_count -1)*2 );

        }
    }

  for ( i = field_count; i < ATTR_SCR_ENTRIES_MAX; i++ )
    {
      screen->field[F_ATTR1_SHORT + (i)*2].field_use = TURNED_OFF;
      screen->field[F_ATTR1_LONG + (i)*2].field_use = TURNED_OFF;
      Scrn_clear_field_by_num(screen, i);      
    }

  Scrn_set_current_field_id( screen, F_ATTR1_LONG );
  wrefresh( screen->window );
}

RET_STATUS
attr_scr_background(SCRN *screen)
{
  ATTR_SELECT_ARRAY  *attrsel;

  attrsel = (ATTR_SELECT_ARRAY *)screen->record;


}

RET_STATUS
attr_scr_pre_key(SCRN *screen, int key_entered, BOOLEAN *done_with_screen)
{
  RET_STATUS rs;
  ATTR_SELECT_ARRAY *attrsel;
  int i = 0;
  
  attrsel = (ATTR_SELECT_ARRAY *)screen->record;

  switch (key_entered)
    {
    case ALTERNATE_KEY:
      *done_with_screen = TRUE;
      return(SUCCESS);

    case YES:
      attrsel = (ATTR_SELECT_ARRAY *)screen->field[screen->curr_field_num - 1].data;
        {
          // set the attribute
          if ( attrsel->vh_drv == C_VEHICLE )
            call_struct.veh_attr[attrsel->attr_nbr] = YES;
          else if ( attrsel->vh_drv == C_DRIVER )
            call_struct.drv_attr[attrsel->attr_nbr] = YES;          
          screen->field[screen->curr_field_num - 1].flags = DISPLAY_ONLY | REVERSE_TEXT;
          //          screen->field[screen->curr_field_num].flags |= REVERSE_TEXT;                        
        }

      Scrn_print_field_by_num( screen, screen->curr_field_num - 1 );
      Scrn_print_field_by_num( screen, screen->curr_field_num );
      return(SUCCESS);


    case NO:
      attrsel = (ATTR_SELECT_ARRAY *)screen->field[screen->curr_field_num - 1].data;
       if ( screen->field[screen->curr_field_num - 1].flags & REVERSE_TEXT )
         {
           // clear the attribute
           if ( attrsel->vh_drv == C_VEHICLE )
             call_struct.veh_attr[attrsel->attr_nbr] = NO;
           else if ( attrsel->vh_drv == C_DRIVER )
             call_struct.drv_attr[attrsel->attr_nbr] = NO;
           screen->field[screen->curr_field_num - 1].flags = DISPLAY_ONLY;
           screen->field[screen->curr_field_num].flags = SILENT;
         }
      Scrn_print_field_by_num( screen, screen->curr_field_num - 1 );
      return(SUCCESS);

    case NEXT_PG_KEY:
      if ( strlen((char *)screen->field[F_ATTR34_LONG].data) == 0 )  // nothing on second page
        return(SUCCESS);
      
        for ( i = 0; i < 66; i++ )
          {
            screen->field[i].field_use = TURNED_OFF;
            Scrn_clear_field_by_num( screen, i );
          }
        for ( i = 66; i < 132; i++ )
          {
            if ( strlen((char *)screen->field[i].data) == 0 )
              {
                screen->field[i].field_use = TURNED_OFF;
                Scrn_clear_field_by_num(screen, i);
                if ( i % 2 )
                  mvwprintw(screen->window, screen->field[i].row,
                            screen->field[i].col, screen->field[i].format,
                            "                    ");
                
                Scrn_print_field_by_num( screen, i );
              }
            else
              {
                if ( i % 2 )
                  screen->field[i].field_use = SILENT;
                else
                  screen->field[i].field_use = DISPLAY_ONLY;
                
                Scrn_print_field_by_num( screen, i );
              }
          }
        Scrn_set_current_field_id( screen, F_ATTR34_LONG );
        wrefresh( screen->window );
      return(SUCCESS);

    case PREV_PG_KEY:
      {
        for ( i = 66; i < 132; i++ )
          {
            screen->field[i].field_use = TURNED_OFF;
            Scrn_clear_field_by_num( screen, i );
          }
        
        for ( i = 0; i < 66; i++ )
          {
            if ( i % 2 )
              screen->field[i].field_use = SILENT;
            else
              screen->field[i].field_use = DISPLAY_ONLY;
            
            Scrn_print_field_by_num( screen, i );
          }
        Scrn_set_current_field_id( screen, F_ATTR1_LONG );        
      }
      return(SUCCESS);
      
    default:
      rs = function_key_basics( key_entered, screen );
      if ( rs != SUCCESS )
        return ( FAIL );
      break;
    }
  
  return(SUCCESS);  
}

RET_STATUS
attr_scr_post_key( SCRN *screen, int key_entered, BOOLEAN *done_with_screen)
{
  RET_STATUS rs;
  ATTR_SELECT_ARRAY *attrsel;

  attrsel = (ATTR_SELECT_ARRAY *)screen->record;

}

RET_STATUS
attr_scr_exit( SCRN *screen, int key_entered, BOOLEAN *exit_screen)
{
  ATTR_SELECT_ARRAY *attrsel;
  int pos = 0, attr_row, attr_col, i;
  
  attrsel = (ATTR_SELECT_ARRAY *)screen->record;

  werase( screen->window );

  if ( ( scr_name == BASIC_CALL_SCR ) || ( scr_name == SUBS_BASIC_SCR ) )
    {
      attr_row = B_CAR_ROW;
      attr_col = B_CAR_ATTR1_COL;
    }
  else
    {
      attr_row = E_CAR_ROW;
      attr_col = E_CAR_ATTR1_COL;
    }

  if ( scr_name != ZONE_STATUS_SCR )
    {
      mvwprintw(stdscr, attr_row + 1, attr_col, "                                  ");
      
      pos = 0;
      for ( i = 0; i < ATTR_MAX; i++ )
        {
          if ( call_struct.veh_attr[i] == YES )
            {
              mvwprintw(stdscr, attr_row + 1, attr_col + pos, "%s ", fleet[cur_fl_index]->vd_attr_info[i].veh_attr);
              pos += 3;
            }
          if ( call_struct.drv_attr[i] == YES )
            {
              mvwprintw(stdscr, attr_row + 1, attr_col + pos, "%s ", fleet[cur_fl_index]->vd_attr_info[i].drv_attr);
              pos += 3;
            }          
        }
    }
  
  touchwin( stdscr );
  *exit_screen = TRUE;
}


RET_STATUS
attr_scr_verify( SCRN *screen,
                 char *read_buf,
                 int key_entered,
                 BOOLEAN field_changed )
{
  ATTR_SELECT_ARRAY *attrsel;

  attrsel = (ATTR_SELECT_ARRAY *)screen->record;
  
  switch ( read_buf[0] )
    {
    case 'Y':
    case 'y':
    case 'K':
    case 'k':
    case 'J':
    case 'j':

      attrsel = (ATTR_SELECT_ARRAY *)screen->field[screen->curr_field_num - 1].data;
      
      if ( ( call_struct.veh_attr[attrsel->attr_nbr] == NO ) &&
           ( attrsel->vh_drv == C_VEHICLE ) )
        {
          if ( count_yeses(call_struct.veh_attr) >= (fleet[cur_fl_index]->max_nbr_veh_atr) )
            {
              Scrn_print_error("Too many vehicle attributes");
              break;
            }
        }

      if ( ( call_struct.drv_attr[attrsel->attr_nbr] == NO ) &&
           ( attrsel->vh_drv == C_DRIVER ) )
        {
          if ( count_yeses(call_struct.drv_attr) >= (fleet[cur_fl_index]->max_nbr_drv_atr) )
            {
              Scrn_print_error( "Too many driver attributes");
              break;
            }
        }
      
        {
          // set the attribute
          if ( attrsel->vh_drv == C_VEHICLE )
            {
              call_struct.veh_attr[attrsel->attr_nbr] = YES;
              call_struct.veh_attr_flag = YES;
            }
          else if ( attrsel->vh_drv == C_DRIVER )
            {
              call_struct.drv_attr[attrsel->attr_nbr] = YES;
              call_struct.drv_attr_flag = YES;
            }
          screen->field[screen->curr_field_num - 1].flags = DISPLAY_ONLY | REVERSE_TEXT;
          //          screen->field[screen->curr_field_num].flags |= REVERSE_TEXT;
          Scrn_print_field_by_num( screen, screen->curr_field_num - 1 );
          //          Scrn_print_field_by_num( screen, screen->curr_field_num );                    
        }
        break;

    case 'E':
    case 'e':
    case 'N':
    case 'n':
      attrsel = (ATTR_SELECT_ARRAY *)screen->field[screen->curr_field_num - 1].data;
       if ( screen->field[screen->curr_field_num - 1].flags & REVERSE_TEXT )
         {
           // clear the attribute
           if ( attrsel->vh_drv == C_VEHICLE )
             call_struct.veh_attr[attrsel->attr_nbr] = NO;
           else if ( attrsel->vh_drv == C_DRIVER )
             call_struct.drv_attr[attrsel->attr_nbr] = NO;
           screen->field[screen->curr_field_num - 1].flags = DISPLAY_ONLY;
           //           screen->field[screen->curr_field_num].flags = SILENT;
           Scrn_print_field_by_num( screen, screen->curr_field_num - 1 );
           //           Scrn_print_field_by_num( screen, screen->curr_field_num );                      
         }
       break;

    default:
      break;
    }
  
  Scrn_update_field_num( screen, key_entered );
}

RET_STATUS
CheckAttrClass( char veh_or_driv, int attr_nbr, char call_class )
{
  return(SUCCESS);
}
        
RET_STATUS
highlight_chosen_attributes(SCRN *screen)
{
  int i;
  int pos = 0, attr_col, attr_row;

  if ( scr_name == ZONE_STATUS_SCR )
    return;
  
  attrset(0);

  if ( ( scr_name == BASIC_CALL_SCR ) || ( scr_name == SUBS_BASIC_SCR ) )
    {
      attr_row = B_CAR_ROW;
      attr_col = B_CAR_ATTR1_COL;
    }
  else
    {
      attr_row = E_CAR_ROW;
      attr_col = E_CAR_ATTR1_COL;
    }  
  mvwprintw(stdscr, attr_row + 1, attr_col, "                                               ");  
  if ( (call_struct.veh_attr_flag == YES) || (call_struct.drv_attr_flag == YES) )
  {
    for (i = 0; i < ATTR_MAX; i ++)
    {
      if (call_struct.veh_attr[i] == YES)
        {
          mvwprintw(stdscr, attr_row + 1, attr_col + pos, "%s ", fleet[cur_fl_index]->vd_attr_info[i].veh_attr);
          pos += 3;
          call_struct.veh_attr_flag = YES;
        }
      if ( call_struct.drv_attr[i] == YES )
        {
          mvwprintw(stdscr, attr_row + 1, attr_col + pos, "%s ", fleet[cur_fl_index]->vd_attr_info[i].drv_attr);
          pos += 3;
          call_struct.drv_attr_flag = YES;
        }
      
    }
  }


  
  return(SUCCESS);
}

int
count_yeses(char *str)
{
  int yeses;

  yeses = 0;
  while (*str)
  {
    if (*str == YES)
      yeses++;
    str++;
  }

  return(yeses);
}

RET_STATUS
function_key_basics(int key_entered, SCRN *screen)
{
  int tmp_scr_name;
  WINDOW *ch_svc_win;   
  
  switch (key_entered)
  {
    case SEARCH_KEY:
      searches();
      return(SUCCESS);
      

    case PRINT_SCR_KEY:
      print_scr(NULL);
      return(SUCCESS);

    case MESSAGE_KEY:
      mesgs(NULL);
      return(SUCCESS);
    
    case RCV_MSG_KEY:
      return(SUCCESS);

    /* ^L: Refresh screen */
    case '\014':
    /* ^R: Refresh screen */
      case '\022':
      wrefresh(curscr);
      return(SUCCESS);

    /* Key not used, return FAIL */
    default:
      return(FAIL);

  }
}
