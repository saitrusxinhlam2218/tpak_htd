 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  menscr_ary.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:47:45
 * @(#)  Last delta: 12/2/94 at 18:18:21
 * @(#)  SCCS File: /taxi/sccs/s.menscr_ary.h
 *                                                                           *
 *****************************************************************************/
struct scr_flds menu_scr_flds[] = {
	{ 0, 0, 1,1},
	{ D_M_NBR_ROW, D_M_NBR_COL, MC_NBR,0},
	{ D_M_NBR_ROW, D_CMD_COL, MC_CMD,1},
	{ D_M_NBR_ROW, D_LST_MENU_COL, MC_LST_MENU,1},
	{ D_M_NBR_ROW, D_TYPE_COL, MC_TYPE,1},
	{ D_M_NBR_ROW, D_DESC_COL, MC_DESC,1},
	{ D_UNIX_CMD_ROW, D_UNIX_CMD_COL, MC_UNIX_CMD,1},
	{ D_UNIX_CMD_ROW, D_PRE_SCR_COL, MC_PRE_SCR,1},
};
