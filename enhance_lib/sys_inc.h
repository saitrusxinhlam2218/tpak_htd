/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  sys_inc.h; Rel: 10.1.0.0; Get date: 4/18/95 at 16:36:30		            *			
* @(#)  Last delta: 12/2/94 at 12:15:39						    *	
* @(#)  SCCS File: /taxi/sccs/s.sys_inc.h							    *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS					    *
*                                                                           *
*****************************************************************************/
/* -----------------------------------------------------------------

    Copyright 1994 - TaxiCom Copenhagen, Denmark
    

    System   : TaxiPak enhancement library 
    Filename : sys_inc.h
    Purpose  : Utilities for error handling and system commands

    Modification history

    Author      Date      Revision     Description
    ---------   -------   -----------  -------------------------
    KAG         940521    0.0         	Prototype 

	funcs:

	prototypes for external linking
	private declarations...

   ----------------------------------------------------------------- */ 


int	 OpenPipe( char *szPipeName );
void FlushPipe( int nPipeFd );
cBOOL SendPipeMsg( void *sbuf, int nSize, int nTaxiFd);
FILE *OpenLog( char *szLogFile );

extern char szLogMsg[];

