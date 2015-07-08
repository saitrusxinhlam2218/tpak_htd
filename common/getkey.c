static char sz__FILE__[]="@(#)getkey.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  getkey.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:32:53
 * @(#)  Last delta: 12/2/94 at 11:54:19
 * @(#)  SCCS File: /taxi/sccs/s.getkey.c
 *                                                                           *
 *****************************************************************************/
#include <curses.h>
#include "tokenize.h"

wgetkey(win)
WINDOW *win;
{
    int c, i, pos, not_this_key[MAX_FUNC_KEYS], n_not_this_key;
    int n_chars, buf[20];

    c = wgetsh(win);
    if  (c != ESC)
	return(c);

    /* If there are no keys after a delay, assume just an ESC was hit */
    if ((c = wgetsh_or_wait(win, DELAY_FOR_FUNCTION_KEYS + 1000)) == ERR) {
	return(ESC);
    }
    ungetsh(c);

    for (i = 0; i < N_func_keys; i ++)
	not_this_key[i] = 0;
    n_not_this_key = 0;
    n_chars = 0;

    for (pos = 1; pos < Max_func_key_len; pos ++) {
	
	c = wgetsh(win);
	
	for (i = 0; i < N_func_keys; i ++) {
	    
	    if (not_this_key[i])
		continue;

	    /* DEC C compiler had problems, below expressions must
	       be in parens, otherwise the if statement fails */
	    if ((Fk[i].len < pos) || (Fk[i].s[pos] != c)) {
		not_this_key[i] = 1;
		n_not_this_key ++;
	    }

	    /* We found it */
	    else if (Fk[i].len == pos + 1) {
		return(Fk[i].code);
	    }
	}

	buf[n_chars ++] = c;

	/* If we didn't find it, push back the last character input and
	   return the ESC entered at the top.
	 */
	if (n_not_this_key == N_func_keys) {
	    for (i = n_chars - 1; i >= 0; i --)
		ungetsh(buf[i]);
	    return(ESC);
	}
    }
    return(ERR);
}
