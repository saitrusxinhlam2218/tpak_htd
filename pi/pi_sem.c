static char sz__FILE__[]="@(#)pi_sem.c        10.1.2.2(ver) Date Release 5/24/95" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  pi_sem.c; Rel: 10.1.2.2; Get date: 5/24/95 at 14:44:29
 * @(#)  Last delta: 5/24/95 at 14:44:15
 * @(#)  SCCS File: /taxi/sccs/s.pi_sem.c
 *                                                                           *
 *****************************************************************************/

#include <sys/types.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "semops.h"


#define WRITE_LOCK_SEM  0       /* semnum of write lock */
#define READ_LOCK_SEM   1       /* semnum of read lock */

/*
 * NUM_ELEMS is a handy macro that computes the number of
 * elements in an array
 */
#define NUM_ELEMS(arr)  (sizeof(arr) / sizeof(arr[0]))


/*
 *
 *      The following contain a simple API for setting read and write
 *      locks using a two-semaphore semaphore set
 *
 *      The semaphore set is created elsewhere.
 *
 *      All funcions return -1 on error, 0 otherwise
 *
 *      Author: Mats Lfstrm, init ab, mla@init.se
 */


/*
 * The sembuf array used when setting the read lock
 */
static struct sembuf readLock[] = {
    { WRITE_LOCK_SEM,   0,      0 },
    { READ_LOCK_SEM,    1,      SEM_UNDO }
};

/*
 * Sembuf used when unlocking. Note that if we try to unlock
 * without having set the lock, semop(2) will return -1, i.e.
 * indicate an error
 */
static struct sembuf readRelease[] = {
    { READ_LOCK_SEM,   -1,      SEM_UNDO | IPC_NOWAIT }
};

/*
 * Sembuf to set the "I want to write"-lock
 */
static struct sembuf writeLockSet[] = {
    { WRITE_LOCK_SEM,   0,      0 },
    { WRITE_LOCK_SEM,   1,      SEM_UNDO }
};

/*
 * Sembuf to wait for the read queue to drain (before actualy writing)
 */
static struct sembuf writeLockWait[] = {
    { READ_LOCK_SEM,    0,      0 }
};

/*
 * To release the write lock. As in the release of the read lock,
 * an error return will occur if called without having the lcok
 */
static struct sembuf writeLockRelease[] = {
    { WRITE_LOCK_SEM,  -1,      SEM_UNDO | IPC_NOWAIT }
};

/*
 *      SetReadLock     - Sets the "I'm reading" lock
 */
#if defined(__STDC__)
int SetReadLock(int semid)
#else
int SetReadLock(semid)
int semid;
#endif
{
    return(semop(semid, readLock, NUM_ELEMS(readLock)));
}

/*
 *      ReleaseReadLock - Releases the "I'm reading" lock
 */
#if defined(__STDC__)
int ReleaseReadLock(int semid)
#else
int ReleaseReadLock(semid)
int semid;
#endif
{
    return(semop(semid, readRelease, NUM_ELEMS(readRelease)));
}

/*
 *      SetWriteLock    - Sets the "I want to write" lock then
 *                        waits for the read queue to drain
 */
#if defined(__STDC__)
int SetWriteLock(int semid)
#else
int SetWriteLock(semid)
int semid;
#endif
{
    if (semop(semid, writeLockSet, NUM_ELEMS(writeLockSet)) == -1)
	return(-1);

    return(semop(semid, writeLockWait,  NUM_ELEMS(writeLockWait)));
}

/*
 *      ReleaseWriteLock- Releases the "I want to write!" lock
 */
#if defined(__STDC__)
int ReleaseWriteLock(int semid)
#else
int ReleaseWriteLock(semid)
int semid;
#endif
{
    return(semop(semid, writeLockRelease, NUM_ELEMS(writeLockRelease)));
}
