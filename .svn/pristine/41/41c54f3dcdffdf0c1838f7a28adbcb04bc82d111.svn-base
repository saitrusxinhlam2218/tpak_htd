/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: initialize.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/07/23 16:58:01 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/TES/initialize.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: initialize.c,v 1.2 1998/07/23 16:58:01 taxiadm Exp $";
#define DECLARING

#include "comm_strdefs.h"
#include "ui_strdefs.h"
#include "writer.h"

/* Do whatever needs to be done before TaxiPak routines may be used. */
TES_initialize()
{
   /* Open the required catalogs. */
   open_catalog(&UI_m_catd, "ui", 0, 0);
   open_catalog(&WRITER_catd, "writer", 0, 0);
}
