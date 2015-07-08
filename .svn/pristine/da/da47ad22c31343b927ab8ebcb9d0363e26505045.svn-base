/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: autobook_nbrs.c,v $
*  @(#)  $Revision: 1.1 $
*  @(#)  $Date: 2002/03/25 17:17:28 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/tools/autobook_nbrs.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: autobook_nbrs.c,v $	$Revision: 1.1 $"

static char rcsid[] = "$Id: autobook_nbrs.c,v 1.1 2002/03/25 17:17:28 jwelch Exp $";

#include <stdio.h>

#define	TERM_MOD	9
#define	SUM_MOD		10

#define	TRUE		1
#define	FALSE		0


static int mod10_wts[] = {1, 2, 0};



/*------------------------------------------------------------------
 *	check digit
 *
 *		going from right to left
 *			term = (weight * term) % 9
 *			-->note: where weight alternates between 1 and 2
 *			sum = (sum + term) % 10
 *		if sum == 0 then return(true)
 *------------------------------------------------------------------
 */

main(int argc, char **argv)
{
	char	start_number_string[10];
	char	  end_number_string[10];
	char    current_number_string[10];
	int     start, end, current;
	int		number_length,
			i,
			term,
			*weight,
			sum;
	


	printf("Enter start number: " );
	gets( start_number_string );
	printf("Enter end number:   " );
	gets( end_number_string );
        start = atoi( start_number_string );
	end = atoi( end_number_string );
	strcpy( current_number_string, start_number_string );


	while (atoi(current_number_string) < end )
	  {
	    number_length = strlen(current_number_string);
	    sum = 0;
	    weight = mod10_wts;	    
	    for (i = number_length - 1; i >= 0; i--)
	      {
		term = current_number_string[i] - '0';
		
		if (term < 0 || term > 9)
		  continue;
		
		if (term != 0)
		  {
		    term = (term * (*weight)) % TERM_MOD;
		    
		    if (term == 0)
		      term = TERM_MOD;
		    
		    sum = (sum + term) % SUM_MOD;
		  }
		
		weight++;
		
		if (!(*weight))
		  weight = mod10_wts;
	      }
	    if ( sum == 0 )
	      printf(  "%s\n", current_number_string );

	    current = atoi(current_number_string);
	    ++current;
	    sprintf(current_number_string, "%d", current);
	  }

}
