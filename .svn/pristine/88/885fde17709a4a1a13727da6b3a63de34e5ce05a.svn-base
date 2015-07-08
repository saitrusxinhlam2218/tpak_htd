static char sz__FILE__[]="@(#)squeeze.c        10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  squeeze.c; Rel: 10.1.2.1; Get date: 5/11/95 at 10:16:07                      *
* @(#)  Last delta: 12/2/94 at 18:53:36                                               *
* @(#)  SCCS File: /taxi/sccs/s.squeeze.c                                                       *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS                                          *
*                                                                           *
*****************************************************************************/


/*************************************************************************
       Data compression / expansion 8 <-> 6 bits

       File: SQUEEZE.C

       (c) Elsoft Oy, Helsinki / Krister Sanmark  1993

**************************************************************************/

typedef unsigned char byte;

/*************************************************************************

       Compress 8 bit string to 6 bit string.
       After it map the result to printable area.


	Input: String pointers and length of source.
	Return length of destination.

**************************************************************************/

int compress(byte *dest, byte *source, int len) {

    int i = 0, j;
    byte *pntr, rem;
    pntr = dest;
    j = 4 * (len / 3);
    if(rem = len % 3) j += rem + 1;


    while(j--) {

	switch(i++ & 0x3) {

	    case 0:
		*pntr++ = *source >> 2;
		break;

	    case 1:
		*pntr = (*source++ << 4) & 0x30;
		*pntr++ |= *source >> 4;
		break;

	    case 2:
		*pntr = (*source++ << 2) & 0x3C;
		*pntr++ |= *source >> 6;
		break;

	    case 3:
		*pntr++ = *source++ & 0x3F;
		break;
	}

    }

    i = len = (int)(pntr - dest);

    while(i--) *dest++ += ' ';

    return(len);

}


/********************************************************************
	Expand 6 bit string to 8 bit string.
	(Compress in reverse order.)

	Input: String pointers and length of source.
	Return length of destination.
	Side effect: The source is corrupted.

**********************************************************************/


int expand(byte *dest, byte *source, int len) {

    int i = 0, j;
    byte *pntr, rem;
    j = 3 * (len / 4);
    if(rem = len % 4) j += (rem-1);
    pntr = source;
    while(len--) *pntr++ -= ' ';
    pntr = dest;

    while(j--) {

	switch(i++ % 3) {

	    case 0:
		*pntr = *source++ << 2;
		*pntr++ |= (*source >> 4) & 0x3;
		break;

	    case 1:
		*pntr = *source++ << 4;
		*pntr++ |= (*source >> 2) &  0xF;
		break;

	    case 2:
		*pntr = *source++ << 6;
		*pntr++ |= *source++ & 0x3F;
		break;

	}

    }

    return((int)(pntr - dest));
}


