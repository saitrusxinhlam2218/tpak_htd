/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  enh_err.h; Rel: 10.1.0.0; Get date: 4/18/95 at 16:36:15		            *			
* @(#)  Last delta: 12/2/94 at 12:16:22						    *	
* @(#)  SCCS File: /taxi/sccs/s.enh_err.h							    *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS					    *
*                                                                           *
*****************************************************************************/
/* -----------------------------------------------------------------

    Copyright 1994 - TaxiCom Copenhagen, Denmark
    

    System   : TaxiPak enhancement library 
    Filename : enh_err.h

    Modification history

    Author      Date      Revision     Description
    ---------   -------   -----------  -------------------------
    KAG         940521    0.0         	Prototype 

 	This is shared code so that libraries can write to log file
	errors can also be recorded the same way

	This has to be included by the library user if they wish 
	to have error logging
   ----------------------------------------------------------------- */ 

/* Error extern declared for mad_error() routine */
#ifdef FOO
#ifndef NDEBUG
#include "mads_types.h"
#include "key.h"
#include "mad_error.h" 
#include "isam.h"
#endif
#endif

#define ERR_INC	1

extern char	szProcName[];
extern char	szLogFile[];
extern char szLogMsg[];
extern cBOOL  bSilent;
extern FILE *log_fd;

  /**********************************************************************************
  * Purpose: Output error descirption to log file( resource error )		   			* 
  *                                                                        			*
  *	void	LogErrorMsg( int nLine, char *szFile, char *szComment, int nErrLevel )	*
  *                                                                        			*
  *		nLine		Line num of file where error occurs 							*
  *		szFile		source file name where error occurs 							*
  *		szComment	Description of errror from programmer							*	
  *		nErrLevel	Exit value ( 0 non-fatal, rest are fatal - see enhance_com.h)	*
  **********************************************************************************/
void	LogErrorMsg( int nLine, char *szFile, char *szComment, int nErrLevel )
{
	fprintf( log_fd, "%s : File %s, Line %d, %s\n Desc %s\n", szProcName, szFile, nLine, szComment, strerror(errno) );
	fprintf( stderr, "%s : File %s, Line %d, %s\n Desc %s\n", szProcName, szFile, nLine, szComment, strerror(errno) );
	if ( nErrLevel > NO_ERROR )
		exit( nErrLevel );
}

/*******************Error loggin macro *******************************/
/* If debug then log to enhance private log file					*/
/* If release log to TaxiPak Error file!!! 							*/

#ifdef NDEBUG
#define	ENH_ERROR( e, ExitValue ) LogErrorMsg(__LINE__, __FILE__, (char*)e, (int)ExitValue) 
#else
#define ENH_ERROR( e, ExitValue ) mad_error( NULL, NULL, e,  __FILE__ , __LINE__ )
#endif

/********** We can compile with/without debug function calls *******************/
  /*************************************************************************
  * Purpose: Output event description to user defined logfile for debug	   * 
  *                                                                        *
  *		void	MakeLogEntry( char *szMsg )								   *
  *                                                                        *
  * szMsg 	Message defined in external string buffer					   * 
  **************************************************************************/
#ifndef NDEBUG
#define MakeLogEntry( string ) NULL 
#else 
void	MakeLogEntry( char *szMsg )
{
	int nSize;

	if ( !bSilent )
	{
		printf(szProcName );
		printf(szMsg);
		printf( "\n" );
	}

	if ( log_fd != NULL )
	{
		if ( !fwrite(szProcName, strlen(szProcName), 1, log_fd ) ) 
			ENH_ERROR( "Writing(ProcName) to log file" , NO_ERROR );

		if ( !fwrite(szMsg, strlen(szMsg), 1, log_fd ) ) 
			ENH_ERROR( "Writing(LogMsg) to log file", NO_ERROR );

		if ( !fwrite("\n", 1, 1, log_fd ) )
			ENH_ERROR( "Writing(CR) to log file", NO_ERROR );

	}
}
#endif
