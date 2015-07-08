/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: getreturn.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1997/08/09 18:08:13 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/sysadmin/getreturn.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: getreturn.c,v 1.2 1997/08/09 18:08:13 taxiadm Exp $";

#include <stdio.h>

char getret()
{
	char inbuf[10];

        inbuf[0]=0;	
	fflush(stdin);
	fflush(stdout);
	gets(inbuf);
	fflush(stdin);
	return(inbuf[0]);
}	
