/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: open_table.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/07/23 16:58:04 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/TES/open_table.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: open_table.c,v 1.2 1998/07/23 16:58:04 taxiadm Exp $";

#include <stdio.h>
#include "TES.h"
#include "STD.h"
#include "mad_ipc.h"
#include "writer.h"

/*
 * the following two lines are used to fake out taxipak routines which
 * access shared memory.  Not a good thing, but I have no idea how else
 * they can be disabled.
 */

extern struct offsets *offset;

RET_STATUS
open_table(int table_id, int access_mode)
{
   static int initialized = FALSE;
   static int open_tables[DB_NUM_FILES];
   RET_STATUS rs;
   
 
   rs = SUCCESS;

   if ( initialized != TRUE )
   {
      int i;

      for (i = 0; i < DB_NUM_FILES; i++)
      {
         open_tables[i] = -1;
      }

      initialized = TRUE;
   }

   /* Either need to open or re-open the db file. */
   if ( open_tables[table_id] != access_mode )
   {
      db_close(table_id);

      rs = db_open(table_id, access_mode);
      if (rs == SUCCESS)
      {
         open_tables[table_id] = access_mode;
      }
      else
      {
         TAXI_ERR(iserrno, "Couldn't open db file.");

      }
   }
   return ( rs );
}


void TES_terminate_access()
{
   db_done();

   /* Close any catalogs we opened. */
   catclose(WRITER_catd);
}
