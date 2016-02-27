/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: mdt_init.c,v $
*  @(#)  $Revision: 2.1 $
*  @(#)  $Date: 2015/07/21 09:20:58 $
*  @(#)  $Author: jazam $
*  @(#)  $Source: /usr/users/jwelch/cvs_g7/taxi-g7/lib/MMP/mdt_init.c,v $
*
*  Copyright (c) 1992 - Motorola Mobile Data Division, Bothell, WA
*
***********************************************************************/
static char rcsid[] = "$Id: mdt_init.c,v 2.1 2015/07/21 09:20:58 jazam Exp $";

/* based on R3 V 5.10 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include "dispatch_strdefs.h"
#include "mads_types.h"
#include "mdt.h"
#include "mad_error.h"
#ifndef MSGQ_
void  time_out_hdlr();
#include <signal.h>
#include "path.h"
#include <errno.h>
int   outb_fd;			/* named pipe to async */
int   inb_fd;			/* named pipe from async */
#ifdef DUAL_PORTS
int   outb_fd_2;
int   inb_fd_2;
#endif

#else
int   send_msgqid;
int   recv_msgqid;
#endif

void
mdt_init(warm)
   int   warm;
{
   MSG_QUEUE_HDR msg;
   void  init_outb_msg();

#ifndef MSGQ_
   char  ch;

   signal(SIGALRM, time_out_hdlr);
   if ((outb_fd = open(TO_ASYNC_PIPE, O_RDWR)) < 0)
   {
#ifdef DEBUG
      fprintf(stderr, "Pipe open failure - to async errno=%d\n", errno);
      perror("");
#endif
      ERROR(' ', "", "cant open to async pipe");
      cleanup();
   }
   if ((inb_fd = open(FROM_ASYNC_PIPE, O_RDWR | O_NDELAY)) < 0)
   {
#ifdef DEBUG
      fprintf(stderr, "Pipe open failure - from async errno=%d\n", errno);
      perror("");
#endif
      ERROR(' ', "", " can't open from async pipe");
      cleanup();
   }
   /* flush the pipe */
   if (!warm)
      while (read(inb_fd, &ch, 1) > 0);
#ifdef DUAL_PORTS
   if ((outb_fd_2 = open(TO_ASYNC_PIPE_2, O_RDWR)) < 0)
   {
#ifdef DEBUG
      fprintf(stderr, "Pipe_2 open failure - to async errno=%d\n", errno);
      perror("");
#endif
      ERROR(' ', "", "cant open to async pipe_2");
      cleanup();
   }
   if ((inb_fd_2 = open(FROM_ASYNC_PIPE_2, O_RDWR | O_NDELAY)) < 0)
   {
#ifdef DEBUG
      fprintf(stderr, "Pipe_2 open failure - from async errno=%d\n", errno);
      perror("");
#endif
      ERROR(' ', "", " can't open from async pipe_2");
      cleanup();
   }
   /* flush the pipe */
   if (!warm)
      while (read(inb_fd_2, &ch, 1) > 0);
#endif
#else
   if ((send_msgqid = msgget(TO_ACC_KEY, IPC_CREAT | 0666)) == -1)
   {
      ERROR(' ', "", " can't get ACC Send msg queue id");
      cleanup();
   }
   if ((recv_msgqid = msgget(FROM_ACC_KEY, IPC_CREAT | 0666)) == -1)
   {
      ERROR(' ', "", " can't get ACC Recv msg queue id");
      cleanup();
   }
   while (msgrcv(recv_msgqid, (struct msgbuf *) & msg, sizeof(MSG_QUEUE_HDR) - sizeof(long), -MAXMTYPE, IPC_NOWAIT) != -1)
      ;
#endif

   init_outb_msg();
}
