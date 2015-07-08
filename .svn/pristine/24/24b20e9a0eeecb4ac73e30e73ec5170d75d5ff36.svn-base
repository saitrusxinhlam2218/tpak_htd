/*
 *                 RCS INFO
 *
 *  @(#)  $RCSfile: Scrn_choice.c,v $
 *  @(#)  $Revision: 1.3 $
 *  @(#)  $Date: 2003/01/28 16:58:23 $
 *  @(#)  $Author: jwelch $
 *  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/SCRN/Scrn_choice.c,v $
 *
 *  Copyright (c) 1991 - Motorola Mobile Data Division, Seattle, WA
 *
 */


/*
 * Scrn_choice
 *
 * This is a wrapper service which envokes the Scrn_choice_alloc(), 
 * Scrn_do_choice(), and Scrn_choice_free() services.
 */

#include <stdio.h>
#include "taxipak.h"
#include "Object.h"
#include "Scrn.h"
#include "Scrn_private.h"

/**************************************************
			Logic

Display the specified window. Store the handle 
corresponding to the user's choice in the choice 
variable. If the user didn't select an option, the 
choice is set to NULL.

Scrn_choice_alloc()


Scrn_choice_window()


Scrn_choice_free()


**************************************************/

RET_STATUS
Scrn_choice
(
	int			max_string_len,
	RET_STATUS		(*next_page)(),
	RET_STATUS		(*prev_page)(),
	RET_STATUS		(*clean_up)(),
	HNDL			user_data_handle,
	HNDL			*choice
)

{

	SCRN_CHOICE_LIST	*choice_list;
	RET_STATUS		rc,
				rc1;

	if (next_page == NULL)
	{
		return(SCRN_BAD_ARG);
	}
	if (prev_page == NULL)
	{
		return(SCRN_BAD_ARG);
	}
	if ( (rc=Scrn_choice_alloc(max_string_len, &choice_list)) != SUCCESS )
		return ( rc );


	choice_list->data = user_data_handle;
	rc = Scrn_choice_window(choice_list, next_page, prev_page, choice, 0);
	
	if (clean_up!=NULL)
		(*clean_up)(choice_list);

	rc1 = Scrn_choice_free(&choice_list);


	if ( rc == SUCCESS )
		rc = rc1;


	return ( rc );


}


