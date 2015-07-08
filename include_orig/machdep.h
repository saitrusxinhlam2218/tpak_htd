 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  machdep.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:47:24
 * @(#)  Last delta: 12/2/94 at 18:17:42
 * @(#)  SCCS File: /taxi/sccs/s.machdep.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:machdep.h	10.1.2.1"

/********************************************************************************
*   Notes 
*   
*   Here are the names of the reserved symbols predefined by the
*   preprocessor for a varity of c compilers. Use these before adding you
*   own
*   
*   Motorola 
*   ========
*   Operating System : unix, sysV68, dmert, gcos, ibm, os, tss
*   Hardware	     : interdata, pdp11, u370, u3b, u3b5, u3b2, u3b20d,
*			vax, m68k
*   
*
*   DEC
*   ===
*   Operating System : bsd4_2, ultrix, unix, vax, VAX, vaxc, VAXC, vax11c,
*			VAX11C
*
*   SUN
*   ===
*   Operating System : unix, gcos, ibm, os, tss
*   Hardware	     : interdata, pdp11, u370, u3b, u3b2, u3b5,
*                         u3b15, u3b20d, vax, mc68000, mc68010,
*                         mc68020, ns32000, iAPX286, i386, sparc,
*                         sun
*  
********************************************************************************/ 
#if  sysV68
#define SYSV 1
#endif


#if  (bsd4_2|ultrix)
#define CLEAR	system("clear")
#else
#define CLEAR	system("tput clear")
#endif








