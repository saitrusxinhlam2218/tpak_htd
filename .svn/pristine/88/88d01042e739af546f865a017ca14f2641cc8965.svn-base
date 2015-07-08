/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  err_inc.h; Rel: 10.1.0.0; Get date: 4/18/95 at 16:36:23		            *			
* @(#)  Last delta: 12/2/94 at 12:15:47						    *	
* @(#)  SCCS File: /taxi/sccs/s.err_inc.h							    *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS					    *
*                                                                           *
*****************************************************************************/
/* -----------------------------------------------------------------

    Copyright 1994 - TaxiCom Copenhagen, Denmark
    

    System   : TaxiPak enhancement library 
    Filename : err_inc.h

    Modification history

    Author      Date      Revision     Description
    ---------   -------   -----------  -------------------------
    KAG         940521    0.0         	Prototype 

	This is shared defines for all objects using debugging

	NOTE: 	if external program ( ie. enhancement )
			uses debug flag then compile library must use as well
 
   ----------------------------------------------------------------- */ 


#ifdef NDEBUG
void	MakeLogEntry( char *szMsg );
#else 
#define MakeLogEntry( string ) NULL 
#endif

void	LogErrorMsg( int nLine, char *szFile, char *szComment, int nErrLevel ); 
#ifdef NDEBUG
#define	ENH_ERROR( e, ExitValue ) LogErrorMsg(__LINE__, __FILE__, (char*)e, (int)ExitValue) 
#else
#define ENH_ERROR( e, ExitValue ) NULL 
#endif
