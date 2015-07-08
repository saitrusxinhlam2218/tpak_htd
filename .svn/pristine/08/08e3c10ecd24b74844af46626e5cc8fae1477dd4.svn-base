 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  timer.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:49:54
 * @(#)  Last delta: 12/2/94 at 18:26:56
 * @(#)  SCCS File: /taxi/sccs/s.timer.h
 *                                                                           *
 *****************************************************************************/
	ULONG	tid;	/* timer id returned by start_timer */
	short	time;	/* timeout value in clock ticks */
	BOOLEAN	valid;	/* timeout valid flag */
	UBYTE   fill; 
	USHORT  fill2;	/* pad to 16 byte boundry , dumps nice */
	} TO_cb;	/* timeout control block */
#define	TOCTS	0
#define	TOBID	1
#define	TOWACK	2
#define	TOTEXT	3
#define	TORESP	4
#define	TOIDLE	5
#define	TOSLEEPY	6
#define	TODCD	7
#define	TODSR	8
#define MAXTIMER 9
