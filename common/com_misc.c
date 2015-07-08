static char sz__FILE__[]="@(#)com_misc.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  com_misc.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:32:38
 * @(#)  Last delta: 12/2/94 at 11:20:59
 * @(#)  SCCS File: /taxi/sccs/s.com_misc.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>

/****************************************************************************
last_six - converts an integer to a string with  the least most six digits
*****************************************************************************/
char *
last_six(num)
int num;
{
	int j;
	char temp_str[21];
	static char temp_call_nbr[10];

		sprintf(temp_str, "%d", num);			/* get last six digits of the call number */
		for(j=0; temp_str[j] != '\0'; j++);
		if(j > 5)
			strcpy(temp_call_nbr, &temp_str[j-6]);			/* more then 6 digits in the call number */
		else
			strcpy(temp_call_nbr, temp_str);

		return(temp_call_nbr);
}
