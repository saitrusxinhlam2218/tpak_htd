/*
 *                 RCS INFO
 *
 *  @(#)  $RCSfile: Scrn_choice_alloc.c,v $
 *  @(#)  $Revision: 1.2 $
 *  @(#)  $Date: 2002/03/29 14:53:44 $
 *  @(#)  $Author: jwelch $
 *  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/SCRN/Scrn_choice_alloc.c,v $
 *
 *  Copyright (c) 1991 - Motorola Mobile Data Division, Seattle, WA
 *
 */


/*
 * Scrn_choice_alloc
 *
 * This service will open an access path to allow choices to be made. The 
 * number passed in is the maximum expected length of an entry. This service 
 * will allocate a SCRN_CHOICE_LIST, and return a pointer to the newly 
 * created object. The maximum string len available for display on the screen
 * will be saved in the structure along with the maximum choices available for 
 * display on the screen. If the requested maximum string length is greater than
 * available on a screen, it will be truncated to the screen maximum. Otherwise
 * is will be rounded up to the maximum available for the number of columns that
 * can fit given the requested maximum. There is a current limit of up to 3 
 * colums currently, but this methodology may be extended to handle a greater 
 * number of columns.
 */

#include <stdio.h>
#include "taxipak.h"
#include "Object.h"
#include "taxi_error.h"
#include "mem_mgmt.h"
#include "Scrn.h"
#include "Scrn_private.h"


/**************************************************
		Logic


Check validity of parameter.


Determine the maximum string len and number of entries.


Allocate SCRN_CHOICE_LIST structure, fill it in and return 
success.


**************************************************/

RET_STATUS
Scrn_choice_alloc
(
	int			max_string_len,
	SCRN_CHOICE_LIST	**choice_list 
)

{

	int entries;
	int string_len;
	SCRN_CHOICE_LIST	*list ;

/*
	if ( choice_list == NULL )
		return ( ERR(SCRN_BAD_ARG, "Invalid Argument") );
*/


	if ( max_string_len > SCRN_TWO_COL_MAX_STRING_LEN )
	{
		string_len = SCRN_CW_MAX_STRING_LEN;
		entries = SCRN_CW_MAX_ROWS;
	}
	else
	{
		if ( max_string_len > SCRN_THREE_COL_MAX_STRING_LEN )
		{
			string_len = SCRN_TWO_COL_MAX_STRING_LEN;
			entries = SCRN_CW_MAX_ROWS * 2;
		}
		else
		{
			string_len = SCRN_THREE_COL_MAX_STRING_LEN;
			entries = SCRN_CW_MAX_ROWS * 3;
		}
	}

	list = (SCRN_CHOICE_LIST *)
	   mm_malloc(sizeof (SCRN_CHOICE_LIST)+(entries - SCRN_CW_MAX_ROWS) *
				 sizeof(SCRN_CHOICE), __FILE__, __LINE__);
 	if ( list == NULL )
		return ( SCRN_MALLOC );


	list->display_len = string_len;
	list->max_choices = entries;
	*choice_list = list;
	
	return ( SUCCESS );


}

