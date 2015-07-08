static char sz__FILE__[]="@(#)semautils.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  semautils.c; Rel: 10.1.2.1; Get date: 4/20/95 at 09:47:58
 * @(#)  Last delta: 12/2/94 at 18:22:14
 * @(#)  SCCS File: /taxi/sccs/s.semautils.c
 *                                                                           *
 *****************************************************************************/

#include <sys/types.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "mad_error.h"

#define	SEMA_KEY	5
#define	SEMNUM		0   /* there is only one semaphore starting
			       with number 0 */
#define CREATE_SEMA     0   /* create semaphore */
#define GET_SEMA_ID     1   /* get semaphore id */

extern int errno;
extern char *sys_errlist[];
int semid_sy = -1;

/* init semaphore utility for sync shared memory */
init_sema_sync()
{
if ((semid_sy = get_semid(CREATE_SEMA)) == -1)
   return(-1);

if (set_sem(semid_sy, 1) == -1)
   return(-1);
return(0);
}

/* gets semaphore id */
get_semid(mode)
int mode;
{
	if (mode == CREATE_SEMA)
		semid_sy = semget(SEMA_KEY, 1, 0666 | IPC_CREAT);
	else
		semid_sy = semget(SEMA_KEY, 1, 0);

	if (semid_sy == -1 ) {
		return(-1);
	}
return(semid_sy);
}

/* set_sem: sets the semaphore value to the argumant specified */
set_sem(sema_id, set_to)
	int sema_id;	/* semaphore id to be operated on */
	int set_to;	/* value to set the semaphore to */
{
	register int ret_val;

	/* set the semaphore value */
	if ( ( ret_val = semctl(sema_id, SEMNUM, SETVAL, set_to)) == -1 )  {
		/* semctl failed */
		sprintf(trace_str, "%s <%s>", "semctl failed",  sys_errlist[errno]);
		ERROR(' ', "", trace_str);
		return(-1);
	}  /* end semctl */

#ifdef DEBUG
	/* get the semaphore value */
	if ( ( ret_val = semctl(sema_id, SEMNUM, GETVAL, set_to)) == -1 )  {
		/* semctl failed */
		sprintf(trace_str, "semctl failed <%s>", sys_errlist[errno]);
		ERROR(' ', "", trace_str);
		return(-1);
	}  /* end semctl */
	sprintf(trace_str, "%s %d\n", "after setting; semaphore value =", ret_val);
	TRACE(trace_str);
#endif

return(0);
}  /* set_sem */

/* operate: does a semop call on a semaphore */
sema_oper(oper)
	short oper;	/* operation */
{

	struct sembuf sb ;

	sb.sem_num = SEMNUM;
	sb.sem_op = oper;
	sb.sem_flg = SEM_UNDO;

	if (semid_sy == -1)
		if (get_semid(GET_SEMA_ID) == -1)
			return (-1);

	if ( semop(semid_sy, &sb, 1) == -1 ) {	
		sprintf(trace_str, "%s (%d) %s <%s>", "semop", oper,
		   "failed", sys_errlist[errno]);
		ERROR(' ', "", trace_str);
		return(-1);
	}
return(0);
}

rm_sema()
{
struct semid_ds arg;

	if (semctl(semid_sy, SEMNUM, IPC_STAT, &arg) < 0)
		{
		sprintf(trace_str, "semctl failed <%s>", sys_errlist[errno]);
		ERROR(' ', "", trace_str);
		return(-1);
		}

	if (semctl(semid_sy, SEMNUM, IPC_RMID, &arg) < 0)
		{
		sprintf(trace_str, "semctl failed <%s>", sys_errlist[errno]);
		ERROR(' ', "", trace_str);
		return(-1);
		}
return(0);
}
