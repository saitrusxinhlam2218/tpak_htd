/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: pminit.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1999/07/21 17:37:54 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/async_dual/pminit.c,v $
*
*  Copyright (c) 1992 - Motorola Mobile Data Division, Seattle, WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: pminit.c,v $	$Revision: 1.2 $"

/* HEADER */
/******************************************************************************
NAME:
		pminit.c
		========


DESCRIPTION:
		This module initializes the shared memory segment for 
		the PARMAN routines.

INPUT DATA:

OUTPUT DATA:

NOTES:

UPDATE HISTORY:

Raymond Chau		June 17, 1987		Created.


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

/* INCLUDE FILES */

#include	"std.h"
#include	<errno.h>
#include	<stdio.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<sys/types.h>
#include	<sys/ipc.h>
#include	<sys/shm.h>
#include	<sys/sem.h>
#include	"parman.h"
#include	"_parman.h"
#include        "taxipak.h"

#define	ERROR	-1

/* IMPORTED VARIABLES AND FUNCTIONS */


/* LOCAL DEFINES */

/* LOCAL TYPEDEFS */

CHAR	pm_path[80];		/* PARMAN Sets directory path */
INT	pm_sal_id;		/* Status Area Lock identifier (semid) */
INT	pm_sa_id;		/* Status Area identifier (shmid) */
SHM	*sts_area;		/* Pointer to the beginning of Status Area */
CHAR    *pm_sa_ptr;
INT	process_id;		/* Process identifier (system wide) */

INT	sts_area_index;		/* sts_area table : current row index */
INT	sts_index;		/* sts_area table : current sts column index */

IMPORT  int	shmdt();
IMPORT 	int	shmget();

EXPORT	struct	sembuf	PM_sembuf;
EXPORT	struct	sembuf	*PM_sops;

#define	PM_SA_KEY	50500
#define	PM_SAL_KEY	50500


/* CODE */


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>     main     <<<<<<< */

pminit ()
{
	CHAR	*pm_sakey_ptr;
	CHAR	*pm_salkey_ptr;
	int	shmkey;
	int	semkey;
	int  	i, j;
	int 	sa_size;
	struct	semid_ds semid_ds;
	union	semun
	{
		int	val;
		struct	semid_ds *buf;
		ushort	*array;
	} arg;
		
	shmkey = PM_SA_KEY;

	/* create the shared memory segment */
	sa_size = sizeof(SHM);
	pm_sa_id = shmget(shmkey, sa_size, (IPC_CREAT | PM_SA_PERM));
	if (pm_sa_id == ERROR)
		return(ERROR);
	else
		printf("SHM CREATED ! ID = %d \n\n", pm_sa_id);

	/* attach to the shared memory and initializes the Status Area */
	pm_sa_ptr = shmat(pm_sa_id,(char *) 0, 0);
	if ((int) pm_sa_ptr == ERROR) 
		return(ERROR);
	else
	{
		sts_area = (SHM *) pm_sa_ptr;
		for (i = 0; i < MAXNUM_PARSET; i++)
		{
			sts_area->rec[i].set_name[0] = '\0';
			sts_area->rec[i].proc_count = 0;
			for (j = 0; j < MAXPROC_PERSET; j++)
			{
				sts_area->rec[i].sts[j].process_id = FREE;
				sts_area->rec[i].sts[j].status = FREE;
			}
		}
		printf("SHM INITIALIZED ! \n\n");
	}

	semkey = PM_SAL_KEY;

	/* create the lock and set the value to "unlocked" */
	pm_sal_id = semget(semkey, 1, (IPC_CREAT | PM_SAL_PERM));
	if (pm_sal_id == ERROR)
		return(ERROR);
	else
	{
		arg.buf = &semid_ds;
		arg.val = PM_CMD_SAULOCK;
		printf("SEM CREATED ! ID = %d \n\n", pm_sal_id);
		if (semctl(pm_sal_id, 0, SETVAL, arg.val) == ERROR)
			return(ERROR);
		else
			printf("SEM UNLOCKED ! \n\n");
	}
	
	/* clean up */
	if (((int) shmdt(pm_sa_ptr)) == ERROR)
		return(ERROR);
	else
		printf("SHM DETACHED ! \n\n");

	return(SUCCESS);
}
