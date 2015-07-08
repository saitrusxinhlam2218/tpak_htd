static char sz__FILE__[]="@(#)ab_io.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ab_io.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:32:06
 * @(#)  Last delta: 12/2/94 at 11:17:21
 * @(#)  SCCS File: /taxi/sccs/s.ab_io.c
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/

/*---------------------------------------------------------
 *	ab io.c
 *---------------------------------------------------------
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include <sgtty.h>
#include <fcntl.h>

/*
 *	taxi includes
 */
#include "mads_types.h"
#include "path.h"
#include "mad_error.h"
#include "ui_ipc.h"

/*
 *	ab includes
 */
#include "ab_defines.h"

/*
 *	globals
 */

/*
 *	externs
 */
extern short				debug_level;



/*
 *========> pipe to dispatch
 */

/*--------------------------------------------------------
 *==>	i0000 open pipe to dispatch
 *--------------------------------------------------------
 */

i0000_open_pipe_to_dispatch()
{
	int			 fd;


	if (DEBUG(AB_DBX_NO_DISPATCH))
		return(TRUE);

	if ((fd = open(USER_PIPE, O_RDWR, 0)) < 0)
	  {
		ERROR(' ', "", "Error opening user pipe");
		return(AB_NULL_FD);
	  }

	return(fd);
}


/*--------------------------------------------------------
 *==>	i0100 write call to dispatch
 *--------------------------------------------------------
 */

i0100_write_call_to_dispatch(fd, call)
	int				 fd;
	struct call_rec	*call;
{
	int				 rc;

	if (DEBUG(AB_DBX_NO_DISPATCH))
		return(TRUE);

	if ((rc = write(fd, call, sizeof(struct call_rec))) == -1)
	  {
		ERROR(' ', "", "Error writing to user pipe");
		return(FALSE);
	  }

	return(TRUE);
}


/*
 *========> port to autobooker machine
 */

/*--------------------------------------------------------
 *==>	i1000 open tty port
 *--------------------------------------------------------
 */

i1000_open_tty_port(ab_tty)
	char			 ab_tty[];
{
	int			 	 fd;


	if (DEBUG(AB_DBX_NO_AB_CONNECT))
		return(TRUE);

	if ((fd = open(ab_tty, O_RDWR, 0)) < 0)
	  {
		ERROR(' ', "", "Error opening autobooker port");
		return(AB_NULL_FD);
	  }

	if (!i1200_initialize_tty_port(fd))
	  {
		ERROR(' ', "", "Error initializing autobooker port");
		return(AB_NULL_FD);
	  }

	return(fd);
}


/*--------------------------------------------------------
 *==>	i1100 close fd
 *--------------------------------------------------------
 */

i1100_close_fd(fd)
	int			 	 fd;
{

	if (DEBUG(AB_DBX_NO_AB_CONNECT))
		return(TRUE);

	close(fd);

	return(TRUE);
}


/*--------------------------------------------------------
 *==>	i1200 initialize tty port
 *--------------------------------------------------------
 */

i1200_initialize_tty_port(fd)
	int			 	 fd;
{
	struct sgttyb	 ttybs;
	struct tchars	 ttychars;
	int				 ttylocal;


/*
 *	flush i/o queues
 */
	if (ioctl(fd, TIOCFLUSH, (int *)&ttybs) == -1)
		return(FALSE);

/*
 *	get parameters
 */
	if (ioctl(fd, TIOCGETP, (char *)&ttybs) == -1)
		return(FALSE);

/*
 *	baud rate = 9600
 *	set erase and kill chars
 *	CBREAK --> return each character as soon as typed
 *	TANDEM --> automatic flow control
 */
	ttybs.sg_ispeed = B9600;
	ttybs.sg_ospeed = B9600;

	ttybs.sg_erase = -1;
	ttybs.sg_kill = -1;

	ttybs.sg_flags = CBREAK;
	ttybs.sg_flags |= TANDEM;

/*
 *	set parameters
 */
	if (ioctl(fd, TIOCSETP, (char *)&ttybs) == -1)
		return(FALSE);

/*
 *	get local mode
 */
	if (ioctl(fd, TIOCLGET, &ttylocal) == -1)
		return(FALSE);

/*
 *	only ^Q restarts output after ^S
 */
	ttylocal |= LDECCTQ;

/*
 *	set local mode
 */
	if (ioctl(fd, TIOCLSET, &ttylocal) == -1)
		return(FALSE);


	return(TRUE);
}


/*
 *========> messages
 */

/*--------------------------------------------------------
 *==>	i2000 get message
 *--------------------------------------------------------
 */

i2000_get_message(fd, ab_message)
	int				 fd;
	char			 ab_message[];
{
	int				 i;
	char			 c;


	memset(ab_message, '\0', AB_IN_MESSAGE_LEN + 1);

	if (DEBUG(AB_DBX_NO_AB_CONNECT))
	  {
		for (i = 0; i < AB_IN_MESSAGE_LEN - 2; i++)
	  	  {
			c = getchar();
			ab_message[i] = c;
			if (ab_message[i] == AB_END_MESSAGE_CHAR)
				break;
		  }
	  }
	else
	  {
		for (i = 0; i < AB_IN_MESSAGE_LEN; i++)
		  {
			if (read(fd, &ab_message[i], 1) <= 0)
				break;

			if (ab_message[i] == AB_END_MESSAGE_CHAR)
				break;
		  }
	  }

	return(TRUE);
}


/*--------------------------------------------------------
 *==>	i2100 send message
 *--------------------------------------------------------
 */

i2100_send_message(fd, line_number, message_number)
	int				 fd,
					 line_number,
					 message_number;
{
	char			 out_message[AB_OUT_MESSAGE_LEN + 2];

	
	sprintf(out_message, "%02d*%03d%c%c", 
				line_number, message_number, AB_CR_CHAR, AB_END_MESSAGE_CHAR);

	if (DEBUG(AB_DBX_MESSAGES))
		printf("--AB says: out message:  %02d*%03d\n", 
					line_number, message_number);

	if (DEBUG(AB_DBX_NO_AB_CONNECT))
		return(TRUE);

	if (write(fd, out_message, AB_OUT_MESSAGE_LEN) != AB_OUT_MESSAGE_LEN)
	  {
		ERROR(' ', "", "Error sending message to autobooker");
		return(FALSE);
	  }
	return(TRUE);
}

