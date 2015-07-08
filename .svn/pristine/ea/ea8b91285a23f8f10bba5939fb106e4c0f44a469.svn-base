/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_lm.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2001/08/14 13:41:24 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_lm.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/
#ident "@(#) head:$RCSfile: fill_lm.c,v $	$Revision: 1.2 $"

static char rcsid[] = "$Id: fill_lm.c,v 1.2 2001/08/14 13:41:24 taxiadm Exp $";

#define KEY_SRC
#include "taxipak.h"
#include "taxi_db.h"
#include "Line_Manager_db.h"

void
fill_line_mgr_key( req_buf, buf )
     char     *req_buf;
     char     *buf;
{
  LINE_MGR_DB_REC       *pLineMgrRec;
  register int          offset = 0;
  
  pLineMgrRec = (LINE_MGR_DB_REC *) req_buf;
  stchar( pLineMgrRec->DDS_Server, &buf[offset], sizeof( pLineMgrRec->DDS_Server ) );
  
}

  
void
fill_line_mgr( req_buf, buf, action )
     char   *req_buf;
     char   *buf;
{
  LINE_MGR_DB_REC      *pLineMgrRec;
  register int         offset = 0;

  pLineMgrRec = (LINE_MGR_DB_REC *)req_buf;
  
  stint( pLineMgrRec->DDS_index, &buf[offset] );
  offset += INTSIZE;
  
  stchar( pLineMgrRec->DDS_Server, &buf[offset], sizeof( pLineMgrRec->DDS_Server ) );
  offset += sizeof( pLineMgrRec->DDS_Server );

  stchar( pLineMgrRec->DDS_Name, &buf[offset], sizeof( pLineMgrRec->DDS_Name ) );
  offset += sizeof( pLineMgrRec->DDS_Name );

  stchar( pLineMgrRec->DDS_Descript, &buf[offset], sizeof( pLineMgrRec->DDS_Descript ) );
  offset += sizeof( pLineMgrRec->DDS_Descript );

}

                    
void
unfill_line_mgr( req_buf, buf )
     char       *req_buf;
     char       *buf;

{
  LINE_MGR_DB_REC       *pLineMgrRec;
  register int          offset = 0;

  pLineMgrRec = (LINE_MGR_DB_REC *) req_buf;

  pLineMgrRec->DDS_index = ldint( &buf[offset] );
  offset += INTSIZE;
  
  ldchar( &buf[offset], sizeof( pLineMgrRec->DDS_Server ), pLineMgrRec->DDS_Server );
  offset += sizeof( pLineMgrRec->DDS_Server );

  ldchar( &buf[offset], sizeof( pLineMgrRec->DDS_Name ), pLineMgrRec->DDS_Name );
  offset += sizeof( pLineMgrRec->DDS_Name );

  ldchar( &buf[offset], sizeof( pLineMgrRec->DDS_Descript ), pLineMgrRec->DDS_Descript );
  offset += sizeof( pLineMgrRec->DDS_Descript );

}

  
