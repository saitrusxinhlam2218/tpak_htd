/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: cnapi.c,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2000/03/06 14:08:31 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/CN_lib/cnapi.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: cnapi.c,v $	$Revision: 1.5 $"

static char rcsid[] = "$Id: cnapi.c,v 1.5 2000/03/06 14:08:31 taxiadm Exp $";

#include "cnapi.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>

extern int tcp_connect(char *, int );
extern int tcp_close( int );
extern int tcp_send( int, char * );
extern int tcp_read( int, char * );

CNargptr argroot = NULL;
int CNconnected = 0;
int CNsocket = 0;
int cAlloc = 0;

CNargptr CNstartNewSearch(void)
{
  CNargptr p, r;

  p = ( CNargptr ) malloc( sizeof( struct CNarg ) ); cAlloc++;

  p->requestId = 0;
  p->criteria = 0;
  p->linklist = NULL;
  p->status = 0;
  p->pathlength = 0;
  p->totdrivetime = 0;
  p->addrlist = NULL;
  p->ctime = 0;
  p->utime = 0;
  p->next = NULL;
  if (argroot == NULL) {
    argroot = p;
  } else {
    r = argroot;
    while (r->next != NULL) r = r->next;
    r->next = p;
  }
  return p;
}

int CNcleanup( CNargptr mydata )
{
  CNargptr p;
  CNlinkptr lp, lastlp;
  CNaddrptr ap, lastap;

  if ( mydata == NULL) return -1;

  for ( lp = mydata->linklist; lp != NULL; lp = lastlp ) {
    lastlp = lp->next;
    free( lp ); cAlloc--;
  }

  for ( ap = mydata->addrlist; ap != NULL; ap = lastap ) {
    lastap = ap->next;
    free( ap ); cAlloc--;
  }

  if ( mydata == argroot ) {
    p = argroot;
    argroot = mydata->next;
    free( p ); cAlloc --;
  } else {
    p - argroot;
    while ( p->next == NULL ? 0 : p->next != mydata )
      p = p->next;
    if ( ( p->next ) == mydata ) {
      p->next = mydata->next;
    } else {
      return -1;
    }
  }
  return 0;
}

int CNputLink( CNargptr mydata,
	       int      street,
	       char     *szKommun,
	       int      number,
	       int      order
	      )
{
  CNlinkptr lp, newLp;

  if ( mydata == NULL )
    return -1;

  newLp = ( CNlinkptr ) malloc( sizeof( struct CNlink ) ); cAlloc++;
  assert("newLp != NULL ", newLp != NULL);

  if ( newLp == NULL ) return -2;

  newLp->street = street;
  strcpy( newLp->szKommun, szKommun );
  newLp->number = number;
  newLp->order = order;
  newLp->next = NULL;

  if ( mydata->linklist == NULL ) {
    mydata->linklist = newLp;
  } else {
    lp = mydata->linklist;
    while ( lp->next != NULL ) lp = lp->next;
    lp->next = newLp;
  }
  return 0;
}

int CNconnect( int vrsn,
	       int mapsysid,
	       char host[],
	       int  port
	      )
{
  CNsocket = tcp_connect( host, port );
  CNconnected = (CNsocket >= 0);
  return( CNconnected );
}

int CNdisconnect( void )
{
  if ( !CNconnected ) return 1;
  tcp_close( CNsocket );
  return 0;
}

int CNGetCoords( CNargptr mydata, long flag )
{
  CNlinkptr      lp;
  char           szCommand[1000];
  char           szResult[1000];
  char           buffer[1000];
  char           sz[100];
  CNaddrptr      ap, r;
  char           *szToken;
  long           i;
  char           asz[10][50];

  if ( mydata == NULL ) return -1;

  flag = 0;
  
  sprintf( szCommand, "QR\t%ld\n", flag );
  for ( lp = mydata->linklist; lp != NULL; lp = lp->next ){
    sprintf( sz, "%ld\t%s\t%ld\t%ld\n", lp->street, lp->szKommun, lp->number, lp->order );
    strcat( szCommand, sz );
  }

  tcp_send( CNsocket, szCommand );

  if ( tcp_read( CNsocket, buffer ) < 0 ) return -1;

  if ( strcmp( buffer, "START\n" ) == 0 ) {
    szResult[0] = '\0';
    do {
      if ( tcp_read( CNsocket, buffer ) < 0 ) return -1;
      strcat( szResult, buffer );
      if ( strcmp( buffer, "STOP\n" ) == 0 ) break;
    } while ( -1 );
  } else {
    return -1;
  }

  szToken = strtok(szResult, "\t\n");
  for ( i = 0; i < 2; i++ ) {
    strcpy( asz[i], szToken );
    szToken = strtok( NULL, "\t\n" );
  }

  mydata->pathlength = atol(asz[0]);  /* temp. for holding X coord */
  mydata->totdrivetime = atol(asz[1]); /* temp. for holding Y coord */

  return 0;
}

int CNGetPath( CNargptr mydata, long flag )
{
  CNlinkptr      lp;
  char           szCommand[1000];
  char           szResult[1000];
  char           buffer[1000];
  char           sz[100];
  CNaddrptr      ap, r;
  char           *szToken;
  long           i;
  char           asz[10][50];

  if ( mydata == NULL ) return -1;

  sprintf( szCommand, "QP\t%ld\n", flag );
  for ( lp = mydata->linklist; lp != NULL; lp = lp->next ){
    sprintf( sz, "%ld\t%s\t%ld\t%ld\n", lp->street, lp->szKommun, lp->number, lp->order );
    strcat( szCommand, sz );
  }

  tcp_send( CNsocket, szCommand );

  if ( tcp_read( CNsocket, buffer ) < 0 ) return -1;

  if ( strcmp( buffer, "START\n" ) == 0 ) {
    szResult[0] = '\0';
    do {
      if ( tcp_read( CNsocket, buffer ) < 0 ) return -1;
      strcat( szResult, buffer );
      if ( strcmp( buffer, "STOP\n" ) == 0 ) break;
    } while ( -1 );
  } else {
    return -1;
  }

  szToken = strtok(szResult, "\t\n");
  for ( i = 0; i < 5; i++ ) {
    if ( szToken != NULL )
      strcpy( asz[i], szToken );
    else
      break;
    szToken = strtok( NULL, "\t\n" );
  }

  if ( strcmp( asz[0], "QP" ) != 0 ) return -1;
  if ( atol( asz[1] ) != 0 ) return -1;
  mydata->pathlength = atol(asz[2]);
  mydata->totdrivetime = atol(asz[3]);
  mydata->ctime = atol(asz[4]);

  while ( strcmp(szToken, "END QP") != 0) {
    for ( i = 0; i < 5; i++) {
      strcpy( asz[i], szToken );
      szToken = strtok( NULL, "\t\n" );
      if ( i == 0 && strcmp(szToken, "END QP") == 0) return 0;
    }

    ap = (CNaddrptr) malloc( sizeof( struct CNaddr ) ); cAlloc++;

    ap->meters = atol( asz[0] );
    ap->estdrivetime = atol(asz[1]);
    ap->turn = asz[2][0];
    strcpy( ap->streetname, asz[3]);
    ap->next = NULL;
    ap->addrstatus = 0;

    if ( mydata->addrlist == NULL) {
      mydata->addrlist = ap;
    } else {
      r = mydata->addrlist;
      while ( r->next != NULL ) r = r->next;
      r->next = ap;
    }
  }
  return 0;
}

int CNGetPathLength( CNargptr mydata ) {
  return CNGetPath( mydata, 0 );
}

int CNGetFullPath( CNargptr mydata ) {
  return CNGetPath( mydata, -1 );
}

int CNControl( int control )
{
  /** not used **/
  return( 0 );
}

    
  
