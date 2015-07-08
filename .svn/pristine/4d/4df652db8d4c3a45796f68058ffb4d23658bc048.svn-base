/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: async_utl.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1999/07/21 17:37:45 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/async_dual/async_utl.c,v $
*
*  Copyright (c) 1992 - Motorola Mobile Data Division, Seattle, WA
*
***********************************************************************/

/* HEADER */
/******************************************************************************
**
** NAME:
**		alc_utl.c
**	
** DESCRIPTION:
**		Set of modules to access "ALC statistics".
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

/* INCLUDE FILES */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "std.h"
#include "acc.h"
#include "async.h"
#include "ASSERT.h"

/* LOCAL DEFINES */
LOCAL CHAR SccsId[] = "@(#)alc_utl.c     1.0     06Nov89 JH      RNG";


/* LOCAL TYPEDEFS */

/* IMPORTED VARIABLES AND FUNCTIONS */

/* CODE */


EXPORT VOIDT    alc_createshm(), alc_attachshm();
EXPORT VOIDT    alc_lockshm(), alc_unlockshm();
EXPORT VOIDT alc_initshm();
EXPORT VOIDT alc_clear_area();

LOCAL	int alc_shmid;			/* Shared memory semaphore id */
EXPORT ALC_STATS *alc_ptr;		/* Shared memory pointer */


/**********************************************************************
	alc_initshm- creates and initializes the shared memory area
***********************************************************************/
EXPORT VOIDT alc_initshm(key)
key_t key;
{
STATUS *status;

	alc_createshm(key, &status);
    alc_attachshm(key, &status);
    alc_unlockshm();

/*
** clear out "alc stats area"
*/	
	alc_clear_area();

}

/**********************************************************************
	alc_clear_area- clear out alc shared memory area
***********************************************************************/
EXPORT VOIDT alc_clear_area()
{
INT i;
INT size;
CHAR *ptr;

	ptr = (CHAR *) alc_ptr;
    size = sizeof(ALC_STATS);
    for(i=0; i<size; ++i)
    {
        *ptr++ = 0;
    }
}


/**********************************************************************
        alc_createshm - Creates the shared memory segment
***********************************************************************/
EXPORT VOIDT alc_createshm( key, status )
     key_t key;
     STATUS *status;
{
  int alc_id;
  UINT segment_size;

  segment_size = sizeof(ALC_STATS);

  /*
  ** Get shared memory
  */
  alc_id = shmget(key, segment_size, 0777 | IPC_CREAT);
  ASSERT_S("alc_create", (alc_id >= 0));
  /*
  ** Get semaphore
  */
  //      alc_shmid = semget(key,1,0666|IPC_CREAT|IPC_EXCL);
  if ((alc_shmid = semget(key, 1, 0666| IPC_EXCL)) <0)
    alc_shmid = semget(key,1,0666|IPC_CREAT|IPC_EXCL);
  ASSERT_S("alc_create", (alc_shmid >= 0));

  *status = SUCCEED;
}


/**********************************************************************
        alc_attachshm - Attaches to the shared memory segment
***********************************************************************/
EXPORT VOIDT alc_attachshm( key, status )
     key_t key;
     STATUS *status;
{
  int alc_id;
  UINT segment_size;


  segment_size = sizeof(ALC_STATS);
  /*
  ** attach to shared memory segment created
  */

  alc_id = shmget(key, segment_size, 0660);
  ASSERT_S("alc_attach", (alc_id >= 0));

  alc_ptr = (ALC_STATS *) shmat(alc_id,0,0);
  ASSERT_S("alc_attach", (alc_ptr != (ALC_STATS *) -1));
  /*
  ** semaphore
  */
  alc_shmid = semget(key,0,0);
  ASSERT_S("alc_attach", (alc_shmid >= 0));

  *status = SUCCEED;
}



/**********************************************************************
	alc_lockshm - Semaphore lock
***********************************************************************/
EXPORT VOIDT  alc_lockshm()
{
	struct sembuf sb;
	int           rc;

	sb.sem_num = 0;
	sb.sem_op  = -1;
	sb.sem_flg = 0;
	rc = semop(alc_shmid,&sb,1);
}
									


/**********************************************************************
	alc_unlockshm - Semaphore lock
***********************************************************************/
EXPORT VOIDT  alc_unlockshm()
{
	struct sembuf sb;
	int           rc;

	sb.sem_num = 0;
	sb.sem_op  = 1;
	sb.sem_flg = 0;
	rc = semop(alc_shmid,&sb,1);
}
