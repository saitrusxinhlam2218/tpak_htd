static char sz__FILE__[]="@(#)pmfinal.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  pmfinal.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:31:48
 * @(#)  Last delta: 12/2/94 at 11:40:27
 * @(#)  SCCS File: /taxi/sccs/s.pmfinal.c
 *                                                                           *
 *****************************************************************************/


/* HEADER */
/******************************************************************************
NAME:
		pminit.c
		========


DESCRIPTION:
		This module cleans up the shared memory segment for 
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

#define	ERROR	-1

/* IMPORTED VARIABLES AND FUNCTIONS */


/* LOCAL DEFINES */

/* LOCAL TYPEDEFS */

INT	pm_sal_id;		/* Status Area Lock identifier (semid) */
INT	pm_sa_id;		/* Status Area identifier (shmid) */

EXPORT	struct	sembuf	PM_sembuf;
EXPORT	struct	sembuf	*PM_sops;

#define	PM_SA_KEY	50500
#define	PM_SAL_KEY	50500


/* CODE */


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>     main     <<<<<<< */

main ()
{
	CHAR	*pm_sakey_ptr;
	CHAR	*pm_salkey_ptr;
	int	shmkey;
	int	semkey;
	int  	i, j;
	struct	semid_ds semid_ds;
	union	semun
	{
		int	val;
		struct	semid_ds *buf;
		ushort	*array;
	} arg;
		
	shmkey = PM_SA_KEY;
	pm_sa_id = shmget(shmkey, PM_SA_SIZE, IPC_CREAT);
	shmctl(pm_sa_id, IPC_RMID, NULL);

	semkey = PM_SAL_KEY;
	pm_sal_id = semget(semkey, 1, IPC_CREAT);
        semctl(pm_sal_id, 0, IPC_RMID, NULL);
	
	return(SUCCESS);
}
