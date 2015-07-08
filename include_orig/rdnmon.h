 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  rdnmon.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:48:44
 * @(#)  Last delta: 12/2/94 at 18:21:14
 * @(#)  SCCS File: /taxi/sccs/s.rdnmon.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:rdnmon.h	10.1.2.1"

#define PRIM_MACH         0    /* primary machine both mirror and dispatch up*/
#define NOT_PRIM_MACH     1    /* not a primary machine */
#define DISP_MACH         2    /* dispatch up */
#define MIRROR_MACH       3    /* mirror up */
#define MAX_MACH_LABEL    3    /* max mach label type */

typedef short MACH_LABEL;

MACH_LABEL mach_label;   /* machine label, sent or recv */

#define MACH_LAB_SZ  sizeof(mach_label);
