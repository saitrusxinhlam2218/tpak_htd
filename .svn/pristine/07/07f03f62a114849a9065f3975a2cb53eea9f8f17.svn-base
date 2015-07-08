/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: res_per_cl_fld.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1996/09/06 14:08:51 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/maint/res_per_cl_fld.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: res_per_cl_fld.c,v 1.2 1996/09/06 14:08:51 jwelch Exp $";

#include "res_per_m_strdefs.h"
#include <cursesX.h>
#include "mad_error.h"
#include "res_per_maint.h"
#include "scr.h"
#include "res_per_scr_ary.h"

extern FILE *trace_fp;		/* trace file pointer */
extern char trace_str[];	/* trace file string */

/******************************************************************************
* 
*******************************************************************************/

res_per_cl_flds(piu)
	int piu;	/* # of the field to be cleared */
{
  int i;
  
  /* move the cursor to the 1st char of the field to be cleared */
  move(res_per_scr_flds[piu].prompt_row, res_per_scr_flds[piu].prompt_col);
  
  /* clear the field */
  for (i=0; i < res_per_scr_flds[piu].max_chars ; i++ )  
    addch(' ');

}
