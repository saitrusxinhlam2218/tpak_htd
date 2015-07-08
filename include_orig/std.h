 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  std.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:49:39
 * @(#)  Last delta: 12/2/94 at 18:23:19
 * @(#)  SCCS File: /taxi/sccs/s.std.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:std.h	10.1.2.1"

/* HEADER */
/**************************************************************************

NAME:
    std.h

DESCRIPTION:
    This header file contains standard defines and typedefs used whenever
    possible on all MDI projects.

NOTES:
    Each project must use this file but it will be modified on a compiler/
    target machine basis as required.


UPDATE HISTORY:

    10 Jun 87   D N Vranesic		Added ENUM, STATUS, Sys def.
    24 Apr 87	J Cranna		Added UCHAR definition
    1  Apr 87	J Cranna		Added defines for nested includes
    23 Mar 87	J Cranna		Changed VOID to VOIDT
    12 Apr 86	K Kulak			Created


**************************************************************************
*			 Copyright (c),  1986				 *
*			All rights reserved				 *
*									 *
*			MOBILE DATA INTERNATIONAL, INC		      	 *
*			RICHMOND, BRITISH COLUMBIA			 *
*			CANADA					 	 *
*									 *
**************************************************************************/
/* END HEADER */

#ifndef	std_h
#define	std_h

/* Standard defined storage scopes. */
#define EXPORT
#define IMPORT extern
#define LOCAL static

/* Standard defined data types. */
typedef float FLOAT;		        /* 32 bit floating point */
typedef double DFLOAT;		        /* 64 bit floating point */

typedef long LONGINT;		        /* 32 bit signed integer */
typedef unsigned long ULONGINT;	        /* 32 bit unsigned integer */
typedef unsigned long LBITS;	        /* 32 bits for bit manipulation */

typedef short int INT;		        /* 16 bit signed integer */
typedef unsigned short int UINT;	/* 16 bit unsigned integer */
typedef unsigned short int WORD;	/* 16 bits for bit manipulation */

typedef char SHORTINT;		        /* 8 bit signed integer */
typedef unsigned char USHORTINT;	/* 8 bit unsigned integer */
typedef unsigned char BYTE;		/* 8 bits for bit manipulation */
typedef char CHAR;			/* ascii character */
typedef unsigned char UCHAR;		/* unsigned 8 bit ascii character */

typedef char BOOL;			/* used to hold logical value */

#define ENUM enum 			/* used for enumerating variables */
#define VOIDT void			/* function returning no value */

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif


typedef	ENUM	{SUCCEED, FAIL}	STATUS;		/* For error handling */

#endif	/*	std_h */

