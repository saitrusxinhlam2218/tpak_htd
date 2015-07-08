/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: _parman.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1995/08/09 19:30:41 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/_parman.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: _parman.h,v $	$Revision: 1.2 $"
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  _parman.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:44:08
 * @(#)  Last delta: 12/2/94 at 17:58:52
 * @(#)  SCCS File: /taxi/sccs/s._parman.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:_parman.h	10.1.2.1"

/* HEADER */
/******************************************************************************
NAME:
		_parman_h
		=======

DESCRIPTION:

		Include file for GETPAR and PARMAN modules.


INPUT DATA:


OUTPUT DATA:


NOTES:


UPDATE HISTORY:

Darko N. Vranesic	March 11, 1987		Original.
Raymond Chau		June 16, 1987		Removed def of FREE.
						Added def of PM_SA_ADDR.
						Re-define structure SHM.


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

#ifndef	_parman_h
#define	_parman_h

/* Subroutine numbers */

#define	SR_SET_CREATE	1
#define	SR_SET_DELETE	2
#define	SR_SET_OPEN	3
#define	SR_SET_CLOSE	4
#define	SR_SET_LOCK	5
#define	SR_SET_UNLOCK	6
#define	SR_STS_CHECK	7
#define	SR_STS_SET	8
#define	SR_STS_RESET	9
#define	SR_VAL_CREATE	10
#define	SR_VAL_DELETE	11
#define	SR_VAL_READ	12
#define	SR_VAL_WRITE	13
#define	SR_semop	14	/* semaphore operation */

/* Constants and Flags */

#define	PM_CMD_SALOCK	-1	/* Status area lock function code */
#define	PM_CMD_SAULOCK	1	/* Status area unlock function code */
#define PM_SA_ADDR	0x1000000   /* SHM virtual address */
#define	PM_SA_PERM	PM_PERM_ALL /* SHM permissions at creat-time */
#define	PM_SAL_PERM	PM_PERM_ALL /* SEM permissions at creat-time */

#define	NOC_PATH	(79 - MAXLEN_PS_NAME)
					/* # of chars in PARMAN path */
#define	NOC_PATHNAME	(NOC_PATH + MAXLEN_PS_NAME)
				/* # of chars for file full path name */
#define	RECORD_LENGTH	(MAXLEN_P_NAME + MAXLEN_P_VALUE + 2)
				/* Parameter Set record length in chars */

#define	OFF_BEG		0	/* From the file beginning (offset) */
#define	OFF_CURR	1	/* From the current offset */
#define	OFF_END		2	/* From the end of file offset */

/* Typedefs */

typedef	struct
{
	INT	process_id;
	BYTE	status;
} STS;

typedef	struct
{
	CHAR	set_name[MAXLEN_PS_NAME];
	INT	proc_count;
	STS	sts[MAXPROC_PERSET];
} SHMREC;

typedef struct
{
	SHMREC	rec[MAXNUM_PARSET];
} SHM;

#define	PM_SA_SIZE	sizeof(SHM)

/* End of the include file _parman_h */
#endif
