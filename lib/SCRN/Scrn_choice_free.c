/*
 *                 RCS INFO
 *
 *  @(#)  $RCSfile: Scrn_choice_free.c,v $
 *  @(#)  $Revision: 1.2 $
 *  @(#)  $Date: 2002/03/29 14:53:44 $
 *  @(#)  $Author: jwelch $
 *  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/SCRN/Scrn_choice_free.c,v $
 *
 *  Copyright (c) 1991 - Motorola Mobile Data Division, Seattle, WA
 *
 */


/*
 * Screen_choice_free
 *
 * Given an open choice_list, free the resouces.
 */

#include <stdio.h>
#include "taxipak.h"
#include "Object.h"
#include "Scrn.h"
#include "Scrn_private.h"


RET_STATUS
Scrn_choice_free 
(
	SCRN_CHOICE_LIST **choice_list
)
{

	if ( (choice_list != NULL) && (*choice_list != NULL) )
	{
		free(*choice_list);
		*choice_list = NULL;
	}
	return ( SUCCESS );
}

