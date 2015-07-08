static char sz__FILE__[]="@(#)getoffsets.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  getoffsets.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:32:55
 * @(#)  Last delta: 12/2/94 at 11:34:34
 * @(#)  SCCS File: /taxi/sccs/s.getoffsets.c
 *                                                                           *
 *****************************************************************************/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#include "taxipak.h"
#include "Object.h"
#include "mad_error.h"
#include "mad_ipc.h"

/**************************************************************
*  get_offsets()
*
*     Get the address of beginning of the offsets struct 
*     in the system shared memory. This structure has the
*     status bits for the system.
*
*     It attaches to the shared memory and returns the address.
*
*         3/18/88          Jack Chou
*
*     ROUTINES used:
*         shmget       get shared memory id
*         shmat        attach to a shared memory
*
*     RETURN:
*         0     failed
*         >0    offsets starting addr
**************************************************************/

struct offsets *get_offsets()
{
char *shmaddr;
int shmid;

#ifdef DEBUG
sprintf(trace_str, "ENTERING get_offsets\n");
TRACE(trace_str);
#endif

/* get shared memory id */

if ((shmid = shmget(SHMKEY, 0, 0)) == -1)
   {
   sprintf(trace_str, "shmget error (%d)", errno);
   ERROR(' ', "", trace_str);
   return(0);
   }

/* get the address */

//  0x1000000
 if ( (shmaddr = shmat(shmid, (const void *)0x0a00000010000000, 0)) == (char *)-1 )  
   //if ((shmaddr = (char *)shmat(shmid, (void *)SHMKEY_MAP_ADDR, 0)) == -1)
   {
   sprintf(trace_str, "shmat error (%d)", errno);
   ERROR(' ', "", trace_str);
   return(0);
   }

#ifdef DEBUG
sprintf(trace_str, "shmid= %X   offsets addr= %X\n", shmid, shmaddr,
   *(unsigned int *)shmaddr);
TRACE(trace_str);
sprintf(trace_str, "LEAVING get_offsets\n");
TRACE(trace_str);
#endif

return((struct offsets *)shmaddr);
}
