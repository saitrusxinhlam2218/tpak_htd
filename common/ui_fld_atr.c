static char sz__FILE__[]="@(#)ui_fld_atr.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_fld_atr.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:33:23
 * @(#)  Last delta: 12/2/94 at 11:48:12
 * @(#)  SCCS File: /taxi/sccs/s.ui_fld_atr.c
 *                                                                           *
 *****************************************************************************/
/* fld_attr: changes the field area attributes */

#include <curses.h>
#include "call_entry.h"

/* norm_to_rev: changes a string from normal mode on the terminal to reverse 
 * mode. (NOTE: if the line is in reverse initizliaaly, it will be reprinted
 *		in reverse again)
 */
norm_to_rev(win_fld, ptr_scr_fld, piu)
	WINDOW *win_fld;	/* window to be used */
	struct scr_flds *ptr_scr_fld;	/* pointer to appropriate piu struct */
	int piu;	/* # of the field */
{
	int i;	/* scratch variable */
	int c;	/* to read char's */
	struct scr_flds *ptr_to_fld;	/* pointer to the appropriate struct */

	/* initialize pointer to the appropriate element of the structure */
	ptr_to_fld = ptr_scr_fld + piu;

	wmove(win_fld, ptr_to_fld->prompt_row, ptr_to_fld->prompt_col);

	for(i=0;i<ptr_to_fld->max_chars;i++) {
		c = winch(win_fld);
		waddch(win_fld, A_REVERSE|c);
	}
	/* do not refresh */
}



/* rev_to_norm: changes a string from reverse mode on the terminal to normal 
 * mode
 */
rev_to_norm(win_fld,ptr_scr_fld,piu)
	WINDOW *win_fld;	/* window to be used */
	struct scr_flds *ptr_scr_fld;	/* pointer to appropriate piu struct */
	int piu;	/* # of the field */
{
	int i;	/* scratch variable */
	int c;	/* to read char's */
	struct scr_flds *ptr_to_fld;	/* pointer to appropriate field */

	/* initialize pointer to the appropriate element of the structure */
	ptr_to_fld = ptr_scr_fld + piu;

	wmove(win_fld, ptr_to_fld->prompt_row, ptr_to_fld->prompt_col);

	for(i=0;i<ptr_to_fld->max_chars;i++) {
		c = winch(win_fld);
		c = c & 0x1ff;	/* string off the reverse attributes bit,
				 * which is read in by inch() */
		waddch(win_fld,c);
	}
	/* do not refresh */
}


/* revund_to_norm: changes a string from reverse/underline mode on the terminal to normal 
 * mode
 */
revund_to_norm(win_fld,ptr_scr_fld,piu)
	WINDOW *win_fld;	/* window to be used */
	struct scr_flds *ptr_scr_fld;	/* pointer to appropriate piu struct */
	int piu;	/* # of the field */
{
	int i;	/* scratch variable */
	int c;	/* to read char's */
	struct scr_flds *ptr_to_fld;	/* pointer to appropriate field */

	/* initialize pointer to the appropriate element of the structure */
	ptr_to_fld = ptr_scr_fld + piu;

	wmove(win_fld, ptr_to_fld->prompt_row, ptr_to_fld->prompt_col);

	for(i=0;i<ptr_to_fld->max_chars;i++) {
		c = winch(win_fld);
		c = c & 0x0ff;	/* string off the reverse attributes bit,
				 * which is read in by inch() */
		waddch(win_fld,c);
	}
	/* do not refresh */
}
