static char sz__FILE__[]="@(#)m_catgets.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  m_catgets.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:33:04
* @(#)  Last delta: 12/2/94 at 12:01:47
* @(#)  SCCS File: /taxi/sccs/s.m_catgets.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/


/*-----------------------------------------------------------
 *	m catgets
 *
 *	calls catgets and allocates space for the string returned
 *	returns pointer to space containing string
 *
 *	used when existing code sets (char *)x = catgets();
 *		to allocate memory and store the return from catgets()
 *-----------------------------------------------------------
 */
#include <string.h>
#include <nl_types.h>


char *
m_catgets(catd, set_num, msg_num, s)
	nl_catd		 catd;
	int 		 set_num, 
				 msg_num;
	char 		*s;
{
	char		*string,
				*space,
				*catgets();


	string = catgets(catd, set_num, msg_num, s);

	space = (char *)malloc(strlen(string) + 1);
	strcpy(space, string);

	return(space);
}

