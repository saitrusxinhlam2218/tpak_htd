/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: libtcp.c,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2002/03/25 15:53:33 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/CN_lib/libtcp.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: libtcp.c,v $	$Revision: 1.5 $"

static char rcsid[] = "$Id: libtcp.c,v 1.5 2002/03/25 15:53:33 jwelch Exp $";

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

static   char crlf[2] = { 13, 10 };

#define  LINE_LEN 80
#define  CONNECT_TIMEOUT 5

int timeout;
conn_timeout(void)
{
  timeout = 1;
}

int tcp_connect( char *host, int port )
{
  int cnt, sock, one = 1;
  struct sockaddr_in sin;
  struct hostent *hp;
  struct linger linger;
  if ((sock = socket( AF_INET, SOCK_STREAM, 0 )) < 0 ) {
    return -1;
  }

  linger.l_onoff = 1;
  linger.l_linger = 1;
  if ( setsockopt(sock, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger)) < 0)
    return -2;

  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  hp = gethostbyname(host);
  if ( hp == NULL )
    {
      printf("Error getting hostname\n");
      alarm(0);
      return -4;
    }
  bcopy(hp->h_addr, &(sin.sin_addr.s_addr), hp->h_length);
  sin.sin_port = htons(port);
  signal(SIGALRM, conn_timeout);
  timeout = 0;
  alarm(CONNECT_TIMEOUT);
  if (connect(sock, &sin, sizeof(sin)) < 0) {
    if (timeout) {
      alarm(0);
      return -4;
    }
    else {
      close(sock);
      alarm(0);
      return -3;
    }
  }
  else
    {
      alarm(0);
      return sock;
    }
}

tcp_close( int sock )
{
  shutdown( sock, 1);
}

tcp_read( int sock, char *buffer )
{
  int cnt;
  unsigned char indata;

  cnt = 0;

  do
    {
      read( sock, &indata, 1 );
      buffer[cnt++] = indata;
    } while ( indata != '\n' );
  buffer[cnt] = '\0';
  return cnt;
}

static int write_data( int sock, char *buffer, int n )
{
  int left, sent;

  left = n;
  while ( left > 0 ) {
    sent = write( sock, buffer, left );
    if ( sent <= 0 )
      return sent;
    left = left - sent;
    buffer = buffer + sent;
  }
  return ( n - left );
}

static void newline( int sock )
{
  write_data(sock, crlf, 2 );
}

static void text(int sock, char *s )
{
  write_data(sock, s, strlen( s ) );
}

void tcp_send( int sock, char *s )
{
  text( sock, s );
  newline( sock );
}
