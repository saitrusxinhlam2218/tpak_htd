 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  sp_scr_fld.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:49:24
 * @(#)  Last delta: 12/2/94 at 18:23:00
 * @(#)  SCCS File: /taxi/sccs/s.sp_scr_fld.h
 *                                                                           *
 *****************************************************************************/
						/* declare structure for reading fields on a suspend driver/vehicle screen */
struct scr_flds sp_scr_flds[] = {
	{ 0, 0, 1, 1},
	{D_FLEET_ROW, D_FLEET_COL, CHARS_FLEET, 1},
	{D_VEH_ROW, D_VEH_COL, CHARS_VEH, 1},
	{D_VEH_ROW, D_DRV_COL, CHARS_DRV, 1},
	{D_VEH_ROW, D_CLEAR_SUSP_COL, CHARS_Y_N_FLAG, 1},
	{D_VEH_ROW, D_CLEAR_SUSP_COL+5, CHARS_Y_N_FLAG, 1},
	{D_VEH_ROW, D_CLEAR_SUSP_COL+10, CHARS_Y_N_FLAG, 1},
	{D_PASS_ROW, D_PASS_COL, CHARS_Y_N_FLAG, 1},
	{D_PASS_ROW, D_SUSP_FROM_DATE_COL, CHARS_SUSP_DATE, 1},
	{D_PASS_ROW, D_SUSP_FROM_TIME_COL, CHARS_SUSP_TIME, 1},
	{D_PASS_ROW, D_SUSP_TO_DATE_COL, CHARS_SUSP_DATE, 1},
	{D_PASS_ROW, D_SUSP_TO_TIME_COL, CHARS_SUSP_TIME, 1},
	{D_DEL_ROW, D_DEL_COL, CHARS_Y_N_FLAG, 1},
	{D_DEL_ROW, D_SUSP_FROM_DATE_COL, CHARS_SUSP_DATE, 1},
	{D_DEL_ROW, D_SUSP_FROM_TIME_COL, CHARS_SUSP_TIME, 1},
	{D_DEL_ROW, D_SUSP_TO_DATE_COL, CHARS_SUSP_DATE, 1},
	{D_DEL_ROW, D_SUSP_TO_TIME_COL, CHARS_SUSP_TIME, 1},
	{D_TYPEX_ROW, D_TYPEX_COL, CHARS_Y_N_FLAG, 1},
	{D_TYPEX_ROW, D_SUSP_FROM_DATE_COL, CHARS_SUSP_DATE, 1},
	{D_TYPEX_ROW, D_SUSP_FROM_TIME_COL, CHARS_SUSP_TIME, 1},
	{D_TYPEX_ROW, D_SUSP_TO_DATE_COL, CHARS_SUSP_DATE, 1},
	{D_TYPEX_ROW, D_SUSP_TO_TIME_COL, CHARS_SUSP_TIME, 1},
	{D_REASON_ROW, D_REASON_COL, CHARS_REASON, 1},
	{D_MSG1_ROW, D_MSG_COL, CHARS_MSG, 1},
	{D_MSG2_ROW, D_MSG_COL, CHARS_MSG, 1},
	{D_MSG3_ROW, D_MSG_COL, CHARS_MSG, 1},
};
