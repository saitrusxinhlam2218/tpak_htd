/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: parman_utl.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1999/07/21 17:37:51 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/async_dual/parman_utl.c,v $
*
*  Copyright (c) 1992 - Motorola Mobile Data Division, Seattle, WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: parman_utl.c,v $	$Revision: 1.2 $"

/* HEADER */
/******************************************************************************
**
** NAME:
**		parman_utl.c
**	
** DESCRIPTION:
**		Set of parman "conversion" utils.
**	
** INPUT DATA:
**	
**	
** OUTPUT DATA:
**	
**	
** NOTES:
**	
**	
** UPDATE HISTORY:
**	
**	J.Hoy Nov 6/89 Original
**	
**	
**	
**		Copyright (c), 1989
**		All rights reserved
**
**	MOBILE DATA INTERNATIONAL, INC.
**		RICHMOND, BRITISH COLUMBIA
**		CANADA
**
*******************************************************************************/
/* END HEADER */
#include "STDIO.h"
#include "std.h"
#include "parman.h"


/* LOCAL DEFINES */
LOCAL CHAR SccsId[] = "@(#)parman_utl.c     1.0     06Nov89 JH      RNG";




EXPORT LONGINT read_tolong();
EXPORT INT read_toint();

/* CODE */


/**************************************************************************
	read_tolong - read in data from parset and convert to long int
***************************************************************************/

EXPORT LONGINT read_tolong(ps, string)
INT ps;
CHAR *string;
{
CHAR data[MAXLEN_P_VALUE+1];
LONGINT lvalue;
INT i;

	parval_read(ps, string, data, sizeof(data) );

/*
** strip trailing blanks
*/
	for(i=MAXLEN_P_VALUE-1; data[i] == ' '; i--)
		data[i] = NULL;
	lvalue = atol(data);
	return(lvalue);
}


/**************************************************************************
	read_toint - read in data from parset and convert to int
***************************************************************************/

EXPORT INT read_toint(ps, string)
INT ps;
CHAR *string;
{
CHAR data[MAXLEN_P_VALUE+1];
INT ivalue;
INT i;

	parval_read(ps, string, data, sizeof(data) );

/*
** strip trailing blanks
*/
	for(i=MAXLEN_P_VALUE-1; data[i] == ' '; i--)
		data[i] = NULL;
	ivalue = atoi(data);
	return(ivalue);
}

