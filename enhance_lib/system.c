static char sz__FILE__[]="@(#)system.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  system.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:36:32		            *			
* @(#)  Last delta: 12/2/94 at 12:16:02						    *	
* @(#)  SCCS File: /taxi/sccs/s.system.c							    *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS					    *
*                                                                           *
*****************************************************************************/

/* -----------------------------------------------------------------
    Copyright 1994 - TaxiCom Copenhagen, Denmark
    

    System   : TaxiPak enhancement library 
    Filename : system.c
    Purpose  : Utilities for error handling and system commands

    Modification history

    Author      Date      Revision     Description
    ---------   -------   -----------  -------------------------
    KAG         940521    0.0         	Prototype 

	funcs:

	void MakeLogEntry( char *szMsg )
	void LogErrorMsg( int nLine, char *szFile, char *szComment, int nErrLevel )
	cBOOL SendPipeMsg( void *sBuf, int nSize, int nTaxiFd)
	int  OpenPipe( char *szPipeName )
	void FlushPipe( int nPipeFd )

   ----------------------------------------------------------------- */ 
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/times.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sgtty.h>

#include    "enhance_com.h"
#include 	"err_inc.h"
#include	"sys_inc.h"

static char new_szLogMsg[5120];

FILE *OpenLog( char *szLogFile )
{
	FILE *log_fd;

	unlink( szLogFile );
	log_fd = fopen( szLogFile, "w");
	if ( log_fd == NULL )
	{
		sprintf( new_szLogMsg, "Can't open %s\n", szLogFile );
		ENH_ERROR( new_szLogMsg, NO_ERROR );
	}
	return log_fd;
}

int CloseFile(FILE *log_handle)
{
	if ( log_handle != NULL )
		fclose( log_handle );

        return(1);
} 

int	OpenPipe( char *szPipeName )
{
	int nPipeFd;

	nPipeFd = open( szPipeName, O_RDWR | O_NDELAY );
	
	if ( nPipeFd < 0 )
	{
		sprintf( new_szLogMsg, "Can't open pipe (%s)", szPipeName );
		ENH_ERROR( new_szLogMsg, FILE_ERROR );
	} 

	return nPipeFd;
}

void	FlushPipe( int nPipeFd )
{
	int 	nBytes;
	char	TmpBuf[100];
	
	nBytes = read(nPipeFd, TmpBuf, 100 );
#ifdef WHY
	if ( nBytes == FAIL )
		ENH_ERROR( "Problem reading pipe", RESOURCE_PROBLEM );
#endif
	if ( nBytes > 0 ) 
	{
		sprintf( new_szLogMsg, "Flushing %d bytes", nBytes );
		MakeLogEntry( new_szLogMsg );
		while ( read(nPipeFd, TmpBuf, 100 ) > 0 );
#ifdef FINDDOC
	/* flush flags --- TCFLSH, TCSAFLUSH, TIOCFLUSH */
	if ( ioctl( nPipeFd, TIOCFLUSH, 0 ) == FAIL )
		ENH_ERROR( "Can't flush TaxiPak pipe" ,NO_ERROR );
#endif
	}
}


cBOOL SendPipeMsg( void *sBuf, int nSize, int nPipeFd)
{
	int nBytesWrote;

	MakeLogEntry( "SendPipeMsg()" );

	if ( nPipeFd <= 0 )
	{
		sprintf( new_szLogMsg, "Invalid pipe file descriptor (%d)", nPipeFd );
		MakeLogEntry( new_szLogMsg );
		return bFALSE; 
	}

#ifdef KEVIN
	sprintf( new_szLogMsg, " Msg to Taxi --> (%*s)", nSize, sBuf );	
	MakeLogEntry( new_szLogMsg );
#endif

	nBytesWrote = write( nPipeFd, sBuf, nSize );
	if ( nBytesWrote == FAIL )
	{
		ENH_ERROR( "Failed writing to client info to taxipak", NO_ERROR );	
		return bFALSE;
	}

	if ( nBytesWrote != nSize )
	{
		sprintf( new_szLogMsg, "Failed writing full msg ( %d of %d bytes )",
								nBytesWrote, nSize );
		ENH_ERROR( new_szLogMsg, NO_ERROR );
		return bFALSE;	
	}
	
	sprintf( new_szLogMsg, "Wrote %d bytes on (%d) ", nBytesWrote, nPipeFd );
	MakeLogEntry( new_szLogMsg );
	return bTRUE;
}

