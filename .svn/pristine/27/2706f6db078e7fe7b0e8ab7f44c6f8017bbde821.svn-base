/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: IPCS.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1995/08/09 19:30:31 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/IPCS.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: IPCS.h,v $	$Revision: 1.2 $"
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  IPCS.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:43:53
 * @(#)  Last delta: 12/2/94 at 17:58:38
 * @(#)  SCCS File: /taxi/sccs/s.IPCS.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:IPCS.h	10.1.2.1"


/* HEADER */
/******************************************************************************
NAME:
		IPCS.h
		======

DESCRIPTION:
		This include file contains definitions of MDIPC constants
to be used in application development based on MDIPC interface.

INPUT DATA:


OUTPUT DATA:


NOTES:


UPDATE HISTORY:

Darko N. Vranesic	Jan 14, 1988	Added ERR_INTRPT. Specified IPCS_h.
Darko N. Vranesic	February 24, 1987	IPCX_ removed from structures.
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

#ifndef	IPCS_h
#define	IPCS_h

/* Error Codes */
#define	ERR_INVPAR	-1	/* Invalid parameter passed */
#define	ERR_UNACRE	-2	/* Unable to create IPC structure */
#define	ERR_IDPRES	-3	/* IPC structure ID already present */
#define	ERR_NMPRES	-4	/* IPC structure name already present */
#define	ERR_NOSPAC	-5	/* Imposed limit would be exceeded */
#define	ERR_ACCESS	-6	/* Permission denied */
#define	ERR_IDINVA	-7	/* IPC structure ID invalid (unknown) */
#define	ERR_NMINVA	-8	/* IPC structure name invalid (unknown) */
#define	ERR_PTRINV	-9	/* Invalid pointer - illegal address */
#define	ERR_TOOBIG	-10	/* Too big IPC element or IPC structure size */
#define	ERR_SEMPTY	-11	/* IPC structure empty */
#define	ERR_SFULL 	-12	/* IPC structure full */
#define	ERR_CONFIG	-13	/* IPC configuration database access error */
#define	ERR_INTRPT	-14	/* System interrupt */
#define	ERR_DEFAULT	-99	/* Default error - none of ones defined above */

/* Constants  -----  Flags */

#define	IPCF_WAIT	-1	/* Wait indefinitely for read or write */
#define	IPCF_NOWAIT	0	/* Do not wait for read or write */
#define	IPCF_READ	0222	/* Read access only */
#define	IPCF_UPDATE	0666	/* Read/Write access */

	  /*  -----  Permissions */

#define	IPCP_UR		00400	/* User read permission */
#define	IPCP_UW		00200	/* User write permission */
#define	IPCP_UALL	00600	/* User read/write permission */
#define	IPCP_GR		00040	/* Group read permission */
#define	IPCP_GW		00020	/* Group write permission */
#define	IPCP_GALL	00060	/* Group read/write permission */
#define	IPCP_OR		00004	/* Others read permission */
#define	IPCP_OW		00002	/* Others write permission */
#define	IPCP_OALL	00006	/* Others read/write permission */
#define	IPCP_ALL	00666	/* Read/write permission to all */

/* Struct definitions */

typedef	struct
	{
		LONGINT  prio;     /* element priority */
		CHAR     data[1];  /* element */
	} IPCS;

#endif	/*	IPCS_h */
/* End of include file IPCS.h */
