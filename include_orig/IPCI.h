/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: IPCI.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1995/08/09 19:30:28 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/IPCI.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: IPCI.h,v $	$Revision: 1.2 $"
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  IPCI.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:43:50
 * @(#)  Last delta: 12/2/94 at 17:58:36
 * @(#)  SCCS File: /taxi/sccs/s.IPCI.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:IPCI.h	10.1.2.1"


/* HEADER */
/******************************************************************************
NAME:
		IPCI.h
		======

DESCRIPTION:
		This include file contains definitions of MDIPC structures
to be used in application development based on MDIPC interface.
		Access to this file is needed when program issues call to
IPC routines to obtain IPC structure information.

INPUT DATA:


OUTPUT DATA:


NOTES:


UPDATE HISTORY:

Darko N. Vranesic	February 10, 1987	Original.
Darko N. Vranesic	February 24, 1987	IPCX_ removed from structures.


*******************************************************************************
*
*		Copyright (c), 1987
*		All rights reserved
*
*	MOBILE DATA INTERNATIONAL, INC.
*		RICHMOND, BRITISH COLUMBIA
*		CANADA
*
******************************************************************************/
/* END HEADER */

typedef struct	/* IPC Permission structure */
	{
	UINT	uid;	/* owner's user id */
	UINT	gid;	/* owner's group id */
	UINT	cuid;	/* creator's user id */
	UINT	cgid;	/* creator's group id */
	UINT	mode;	/* access mode */
	UINT	seq;	/* slot usage sequence number */
	INT		key;	/* key */
	} IPCP;

typedef struct	ICPD0	/* IPC Data structure */
	{
	struct IPCD0    *next;     /* ptr to next data */
	LONGINT         prior;     /* data priority */
	INT        length;    /* data length in bytes */
	INT        mapadd;    /* data map address */
	} IPCD;

typedef struct	/* IPC Information structure */
	{
	IPCP	perm;	/* operation permission struct */
	IPCD	*first; /* ptr to first element in IPC structure */
	IPCD	*last;  /* ptr to last element in IPC structure */
	UINT	cbytes;	/* current # bytes in IPC structure */
	UINT	qnum;	/* # of elements in IPC structure */
	UINT	qbytes;	/* max # of bytes in IPC structure */
	UINT	lspid;	/* pid of last element write */
	UINT	lrpid;	/* pid of last element read */
	LONGINT		stime;	/* last element write time */
	LONGINT		rtime;	/* last element read time */
	LONGINT		ctime;	/* last change time */
	} IPCI;

/* End of include file IPCI.h */
