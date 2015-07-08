static char sz__FILE__[]="@(#)get.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  get.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:32:49
 * @(#)  Last delta: 12/2/94 at 11:26:46
 * @(#)  SCCS File: /taxi/sccs/s.get.c
 *                                                                           *
 *****************************************************************************/
#include <curses.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <errno.h>

#define MAX_IN_BUF_SIZE 20

extern int errno;

int n_inbuf = 0;
int inbuf[MAX_IN_BUF_SIZE];

extract_from_buf()
{
    int c;

    if (n_inbuf > 0) {
	c = inbuf[0];
	strcpy((char *)inbuf, (const char *)(inbuf + 1));
	n_inbuf --;
	return(c);
    }
    return(ERR);
}

int wgetsh(win)
WINDOW *win;
{
    int c;

    if ((c = extract_from_buf()) != ERR)
	return(c);

    c = wgetch(win);
    return(c);
}
    
void ungetsh(c)
int c;
{
    int i;

    /*
     * Shift buffer one to right to make room for new character.
     * If buffer is full, lose last char in buffer.
     */
    for (i = ((n_inbuf < MAX_IN_BUF_SIZE) ? n_inbuf: MAX_IN_BUF_SIZE - 1);
	      i > 0; i --)
	inbuf[i] = inbuf[i - 1];
    inbuf[0] = c;

    if (n_inbuf != MAX_IN_BUF_SIZE)
	 n_inbuf ++;
}

/*
 * Wait for either a character to be entered, or a certain time in
 * milliseconds to pass. Return the character if one is entered during
 * the wait, or return ERR if no character is entered. nodelay() must
 * be called at some point before this so that input is not blocked.
 */
int
wgetsh_or_wait(win, ms)
WINDOW *win;
int ms;
{
    int ms_past;
    struct timeb t1, t2;
    int c, changed_delay;
    int n, readfds;
    struct timeval timeout;
    fd_set  readmask;
    int     maxfdpl;

    if ((c = extract_from_buf()) != ERR)
	return(c);


wait_for_char:
    readfds = 1; /* for stdin */
    FD_ZERO(&readmask);      
    FD_SET( 1, &readmask );
    maxfdpl = 2;    
    timeout.tv_sec = ms / 1000;
    timeout.tv_usec = (ms * 1000) - (timeout.tv_sec * 1000000);
    ftime(&t1);
    n = select(maxfdpl, &readmask, (fd_set *) 0, (fd_set *)0, &timeout);    

    /* Found char, read it in */
    if (n > 0) {
	c = wgetch(win);
	return(c);
    }

    /* Timed out */
    if (n == 0)
	return(ERR);

    /* Signal caught while waiting */
    if (n == -1 && errno == EINTR) {

	/* Poll stdin to see if anything entered during
	   signal processing */
        FD_ZERO(&readmask);      
        FD_SET( 1, &readmask );
	maxfdpl = 2;
	readfds = 1; /* for stdin */
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	n = select(1, &readmask, (fd_set *) 0, (fd_set *)0, &timeout);

	/* Found char, read it in */
	if (n > 0) {
	    c = wgetch(win);
	    return(c);
	}

	/* Nothing entered during signal processing, let's see if time
	   alloted has past */
	if (n == 0) {
	    ftime(&t2);

	    /* Timed out */
	    if ((ms_past = (t2.time - t1.time) * 1000 + t2.millitm - t2.millitm) >= ms)
		return(ERR);

	    /* Wait again (minus the spent already) */
	    ms -= ms_past;
	    goto wait_for_char;
	}

	/* Second select failed */
	return(ERR);
    }
    return(ERR);

}

void
flush_input(win)
WINDOW *win;
{
  /* Read input from stdin until empty */
  while (wgetsh_or_wait(win, 0) != ERR);

  /* Empty local input buffer */
  n_inbuf = 0;
  inbuf[0] = '\0';
}
