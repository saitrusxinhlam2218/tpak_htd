 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  systat.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:49:50
 * @(#)  Last delta: 12/2/94 at 18:25:38
 * @(#)  SCCS File: /taxi/sccs/s.systat.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:systat.h	10.1.2.1"

/* Nmonics used to set status bits in system shared memory */

#define DISP_STAT   1                 /* dispatch status */
#define NCP_STAT    2                 /* NCP status */
#define RDCY_STAT   3                 /* redundancy status */
#define PRIM_STAT   4                 /* primary machine status */
#define NDISP_STAT  5                 /* Non-dispatch status */
#define NNCP_STAT   6                 /* Non-NCP status */
#define NRDCY_STAT  7                 /* Non-redundancy status */
#define SECD_STAT   8                 /* secondary (Non-primary) machine */
