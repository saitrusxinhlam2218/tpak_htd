static char sz__FILE__[]="@(#)kdt_init.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  kdt_init.c; Rel: 10.1.2.1; Get date: 5/11/95 at 10:15:24
 * @(#)  Last delta: 12/2/94 at 18:02:38
 * @(#)  SCCS File: /taxi/sccs/s.kdt_init.c
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/

/* based on R3 V 5.10 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include "dispatch_strdefs.h"
#include "mads_types.h"
#include "kdt.h"
#include "mad_error.h"

void t_out_hdlr();
#include <signal.h>
#include "path.h"
#include <errno.h>
int     outb_fd;   /* named pipe to async */
int     inb_fd;    /* named pipe from async */
int     udp_out_fd;
int     udp_in_fd;
int     from_aux_device_fd;
int     to_aux_device_fd;
int     use_aux_pipes = TRUE;

void 
kdt_init(warm)
int warm;
{
	MSG_QUEUE_HDR msg;
	void init_outb_msg();

        char ch;

        signal(SIGALRM, t_out_hdlr);
        if ((outb_fd = open(TO_ASYNC_PIPE, O_RDWR)) < 0)
        {
          ERROR(' ',"","cant open to async pipe");
          cleanup();
        }
        if ((inb_fd = open(FROM_ASYNC_PIPE, O_RDWR | O_NDELAY)) < 0)
        {
          ERROR(' ',""," can't open from async pipe");
          cleanup();
        }

        if ((udp_out_fd = open(TO_UDP_PIPE, O_RDWR | O_NDELAY)) < 0)
          {
            ERROR(' ', "","can't open UDP pipe");
          }
        if ((udp_in_fd = open(FROM_UDP_PIPE, O_RDWR | O_NDELAY)) < 0)
          {
            ERROR(' ', "","can't open UDP pipe");
          }            
	
	if ((from_aux_device_fd = open(FROM_AUX_DEVICE_PIPE, O_RDWR | O_NDELAY)) < 0)
	  use_aux_pipes = FALSE;
	if ((to_aux_device_fd = open(TO_AUX_DEVICE_PIPE, O_RDWR | O_NDELAY)) < 0)
	  use_aux_pipes = FALSE;

	if ( use_aux_pipes == TRUE )
	  while(read(to_aux_device_fd, &ch, 1) > 0);
	
        /* flush the pipe */
        if (!warm)
           while(read(inb_fd, &ch,1) > 0);

	init_outb_msg();
}

