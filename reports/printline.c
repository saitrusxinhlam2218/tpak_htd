static char sz__FILE__[]="@(#)printline.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  printline.c; Rel: 5.1.0.0; Get date: 2/27/90 at 12:22:08
 * @(#)  Last delta: 12/14/89 at 17:11:35
 * @(#)  SCCS File: /taxi/sccs/s.printline.c
 *                                                                           *
 *****************************************************************************/

#include <string.h>
#include <ctype.h>

#include "rep_strdefs.h"

/*====================================================================
= printable_line() -
=	Scans a buffer right to left looking for an alphanumeric character.
=	Returns 1 if one is found, else returns 0.
=	printable_line is modified to skip several default values.
=	Who:	Bruce Young
=	What:	Written		When: 20 February 1988
=====================================================================*/
/*
What the FUCK is this function supposed to do!!!
What it does is return 1 if the line has any alphBETIC character that is
not an 'N'. That's the only case it returns 1, a zero is returned if the
line is empty, or is all digits. Well, I've changed it to return 0 only if
the line is empty, and 1 otherwise. D. Rudoff 6-7-90
*/

printable_line(iobuff)
char *iobuff;
{
  while (*iobuff)
    {
      if (!isspace(*iobuff))
	return(1);
      iobuff ++;
    }

  return(0);

#ifdef FUCKUP
	int bufflength;
	bufflength=strlen(iobuff);
	while (bufflength--) {
		 /* skip date fields */
		if (isdigit(iobuff[bufflength]) &&
		    iobuff[bufflength-2]== '/')
			bufflength -= 8;

		 /* print only lines containing [a-z] | [A-M] | [O-Z] */
		if (isalpha(iobuff[bufflength]) &&
		(iobuff[bufflength] !='0') &&
		(iobuff[bufflength] !='N' )) return(1);
	}	/* end while */
	return(0);
#endif
}	/* printable_line */
