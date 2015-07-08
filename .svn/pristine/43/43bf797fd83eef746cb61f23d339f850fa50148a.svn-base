static char sz__FILE__[]="@(#)toupper.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  toupper.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:33:21
* @(#)  Last delta: 12/2/94 at 11:17:41
* @(#)  SCCS File: /taxi/sccs/s.toupper.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

/* Because there is a bug in DEC's toupper when the internationalization
   library is used, Toupper() has been defined to replace it. The bug is
   that if the argument is non-ascii (more that 127) a segmentation
   violation results. */

#include <stdlib.h>
#include <ctype.h>

Toupper(c)
int c;
{
  
  if (isascii(c))
    {
      if ( c == 0x7c )		/* PIPE character  */
	return( 0x5c );
      if ( c == 0x7d )		/* RIGHT-CURLY brace */
	return( 0x5d );
      if ( c == 0x60 )		/* EGRAVE */
	return( 0x40 );
      if ( c == 0x7e )		/* TILDE */
	return( 0x5e );
      if ( c == 0x7b )		/* LEFT-CURLY brace */
	return( 0x5b );
      else
	return(toupper(c));
    }
  else
    return(toupper(c));
}
