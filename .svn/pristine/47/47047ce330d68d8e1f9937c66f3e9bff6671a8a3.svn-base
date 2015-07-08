static char sz__FILE__[]="@(#)ui_init_db.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  ui_init_db.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:50:57
* @(#)  Last delta: 12/2/94 at 17:56:33
* @(#)  SCCS File: /taxi/sccs/s.ui_init_db.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

#include <isam.h>
#include <sys/types.h>

#include "taxi_db.h"

ui_init_db()
{
	int			i;


	for (i = 0; i < MAX_FILES; i++)
	  {
		Db_files[i].fd = -1;
	  }

	return;
}


