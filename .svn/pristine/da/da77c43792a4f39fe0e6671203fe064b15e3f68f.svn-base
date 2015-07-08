/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: hol_cl_fld.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1996/09/06 14:08:21 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/maint/hol_cl_fld.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: hol_cl_fld.c,v 1.2 1996/09/06 14:08:21 jwelch Exp $";

#include "holiday_per_m_strdefs.h"
#include <cursesX.h>
#include "mad_error.h"
#include "holiday_per_maint.h"
#include "scr.h"
#include "holiday_per_scr_ary.h"

extern FILE *trace_fp;		/* trace file pointer */
extern char trace_str[];	/* trace file string */

/******************************************************************************
* 
*******************************************************************************/

hol_cl_flds(piu)
	int piu;	/* # of the field to be cleared */
{
  int i;
  
  /* move the cursor to the 1st char of the field to be cleared */
  move(hol_scr_flds[piu].prompt_row, hol_scr_flds[piu].prompt_col);
  
  /* clear the field */
  for (i=0; i < hol_scr_flds[piu].max_chars ; i++ )  
    addch(' ');
}
