#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/errno.h>

extern int errno;

#define MKEY1    2048

#define PERMS    0666

main()
{
  int readid, writeid, rc;
  struct msg        message_on_queue;
  struct msg        *pMessage;

  bzero( &message_on_queue, sizeof( struct msg ) );
  pMessage = &message_on_queue;

  if ( ( readid = msgget(MKEY1, PERMS | IPC_CREAT) ) < 0 )
    perror("server: can't get message queue");

  while ( rc >= 0 )
    {
      rc = msgrcv( readid, (void *)pMessage, 256, -32767L, MSG_NOERROR );
      
      if ( rc < 0 )
	perror("Error receiving on queue");
    }
}
