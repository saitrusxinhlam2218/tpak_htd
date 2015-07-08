/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: tcp_open.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2002/01/07 16:31:35 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/async_dual/tcp_open.c,v $
*
*  Copyright (c) 1992 - Motorola Mobile Data Division, Seattle, WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: tcp_open.c,v $	$Revision: 1.3 $"

#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/file.h>
#include <sys/param.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>

#define ERROR       -1

void sig_timeout();

static int getserveraddr(hname, sname, sin)
char * hname, * sname;
struct sockaddr_in * sin;
{

        struct servent *sp;
        struct hostent *hp;
        int server_port;



        if ((hp = gethostbyname(hname)) == NULL) return(1);
        if (*sname == '~')
        {
                server_port = atoi(sname+1);
        }
        else
        {   
                if ((sp = getservbyname(sname,"tcp")) == NULL) return(2);
                server_port = sp->s_port;

        }
#ifndef SYSV
        bzero(sin,sizeof(*sin));
        bcopy(hp->h_addr,(char *)&sin->sin_addr,hp->h_length);
#else
        memcpy((char *)&sin->sin_addr,hp->h_addr,hp->h_length);
#endif
        sin->sin_port = htons(server_port);
        sin->sin_family = hp->h_addrtype;
        return(0);
 
}

int udp_open(service)
  char *service;
{
  int  sockfd;
  struct sockaddr_in serv_addr;

  if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
    perror("Problem with socket");

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  serv_addr.sin_port = htons(atoi(service+1));

  if ( bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 )
    perror("Problem with bind");

  return( sockfd );
  
}
 

int tcp_open(hostname,service,buffsiz,delay,retry)
char * hostname;
char * service;
int buffsiz;
int delay;
int retry;
{
        int sts;
        struct sockaddr_in server;
        int t;
        struct linger xlinger;
        int intval;
        int connected=0;

        /* get the address of the server */

        if (sts = getserveraddr(hostname,service, &server))
	{
                fprintf(stderr,"**TCP**  -- where is the server anyway? == %d",sts);
		return(ERROR);
	}

        /* create a socket and establish a connection */
        if ((t = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
                fprintf(stderr,"**TCP** -- can't create a socket");
		return(ERROR);
	}
	intval = sizeof(struct linger);
        if (getsockopt(t, SOL_SOCKET, SO_LINGER, &xlinger, &intval) < 0)
	{
                fprintf(stderr,"**TCP** -- setsockopt failed");
		return(ERROR);
	}
        intval = sizeof(struct linger);
        xlinger.l_onoff=0;
        xlinger.l_linger=0;
        if (setsockopt(t, SOL_SOCKET, SO_LINGER, &xlinger, intval) < 0)
	{
                fprintf(stderr,"**TCP** -- setsockopt failed");
		return(ERROR);
	}
        if (setsockopt(t, SOL_SOCKET, SO_REUSEADDR, &intval, sizeof(int)) < 0)
	{
                fprintf(stderr,"**TCP** -- setsockopt failed");
		return(ERROR);
	}
#ifdef FOO
	if (setsockopt(t, SOL_SOCKET, SO_SNDBUF, (char *) &buffsiz) < 0)
	  {
	    fprintf(stderr, "**TCP** -- setsockopt (SO_SNDBUF) failed");
	    return(ERROR);
	  }
	if (setsockopt(t, SOL_SOCKET, SO_RCVBUF, (char *) &buffsiz) < 0)
	  {
	    fprintf(stderr, "**TCP** -- setsockopt (SO_RCVBUF) failed");
	    return(ERROR);
	  }
#endif
        signal( SIGALRM, sig_timeout );
        while ((retry--)&&(!connected))
        {
          alarm(5);   // allow 5 seconds for a connection then move on
                if (connect(t, &server, sizeof(server)) != 0)
                {
                        switch (errno)
                        {
                                case EINTR    :
                                case ETIMEDOUT:
                                  alarm(0);
                                  return(-1);
                                  break;
                                case ECONNREFUSED :
                                case EINVAL:
                                  alarm(0);
                                  return(-1);
                                  break;

                                default :
                                  fprintf(stderr,"**TCP** -- errno %d",errno);
                                  alarm(0);
                                  return(-1);
					  
                        }
                }
                else
                  {
                    alarm(0);
                    connected =1;
                  }
        }
        if (connected)
        {
                return(t);
        }
	else
          {
            alarm(0);
            return(-1);
          }

}

void
sig_timeout()
{
   alarm(0);
   errno = ETIMEDOUT;
   signal(SIGALRM, sig_timeout);
}
