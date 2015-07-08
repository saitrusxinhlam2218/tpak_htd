 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fl_scr.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:45:49
 * @(#)  Last delta: 12/2/94 at 18:13:53
 * @(#)  SCCS File: /taxi/sccs/s.fl_scr.h
 *                                                                           *
 *****************************************************************************/
struct scr_flds {
	int prompt_row;		/* row on which prompt appears */
	int prompt_col;		/* col on which prompt appears */
	int max_chars;		/* maximum chars allowed on a field */
	int tab_field;
	int (*val_rtn)();	/* pointer to a validation routine */
	char *fmt_str;		/* Format string for printf */
	char *help;
};
