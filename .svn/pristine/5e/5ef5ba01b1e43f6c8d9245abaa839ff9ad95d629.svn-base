/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: MDIPC.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1995/08/09 19:30:33 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/MDIPC.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: MDIPC.h,v $	$Revision: 1.2 $"
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  MDIPC.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:43:57
 * @(#)  Last delta: 12/2/94 at 17:58:46
 * @(#)  SCCS File: /taxi/sccs/s.MDIPC.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:MDIPC.h	10.1.2.1"


/* HEADER */
/******************************************************************************
NAME:
		MDIPC.h
		=======

DESCRIPTION:
		This include file contains definitions of MDIPC constants
to be used in application development based on MDIPC interface but only from
direct IPC facility development team to create, test and implement
IPC routines.

INPUT DATA:


OUTPUT DATA:


NOTES:


UPDATE HISTORY:

Darko N. Vranesic	February 10, 1987	Original.


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

#define	MIN_PRIOR	-99999999999999	/* Minimum priority */
#define	IPCS_NAME_MXSZ	20		/* Maximum IPC structure name size */

#define	MDIPC_UNIQUE	IPC_CREAT	/* Represents flag for unique
					IPC structure creation */
#define	MDIPC_KEY	IPC_PRIVATE	/* Private Message Queue key */

/* Basic module code */
#define	CREATE		1
#define	DELETE		2
#define	OPEN		3
#define	CLOSE		4
#define	GETNAME 	5
#define	INFO		6
#define	WRITE		7
#define	READ		8


/* IPC routine flags */
#define	IPCF_DELETE	IPC_RMID	/* Remove MQ id */
#define	IPCF_STAT	IPC_STAT	/* Get statistics */
#define	IPCF_SET	IPC_SET		/* Set values */
#define IPC_SYSF_NOWAIT	IPC_NOWAIT	/* Unblocking operation flag */
#define	IPC_SYSF_WAIT	0		/* Blocking operation flag */

/* Offset definitions */
#define	OFF_EXACT	0		/* Offset from pointer exactelly */
#define	OFF_BEG		0		/* From beginning of the stream */
#define	OFF_CURR	1		/* From current stream pointer offset */
#define	OFF_END		2		/* From stream end offset */


/* End of include file MDIPC.h */
