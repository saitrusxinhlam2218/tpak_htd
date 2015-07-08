/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: cnapi.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1996/09/06 13:46:25 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/cnapi.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _CNAPI_H
#define _CNAPI_H
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define assert(TXT,X) if (!(X)) printf("Assert failure: "#TXT " "#X "\n");
typedef struct CNlink *CNlinkptr;
struct CNlink {
  int   street;
  char  szKommun[4];
  int   number;
  int   order;
  CNlinkptr  next;
};
typedef struct CNaddr *CNaddrptr;
struct CNaddr {
  char  streetname[40];
  char  numberinterval[20];
  char  szKommun[40];
  int   addrstatus;
  int   meters;
  char  turn;
  int   estdrivetime;
  CNaddrptr  next;
};
typedef struct CNarg *CNargptr;
struct CNarg {
  int    requestId;
  time_t  received;
  time_t  requestAt;
  time_t  started;
  time_t  answered;
  int     criteria;
  CNlinkptr  linklist;
  int      status;
  int      pathlength;
  int      totdrivetime;
  CNaddrptr  addrlist;
  int      ctime;
  int      utime;
  CNargptr  next;
};
CNargptr CNstartNewSearch(void);
int CNputLink( CNargptr,
	      int,
	      char *,
	      int,
	      int );
int CNcleanup(CNargptr);
int CNconnect( int,
	      int,
	      char *,
	      int );
int CNdisconnect(void);
int CNGetPathLength(CNargptr);
int CNGetFullPath(CNargptr);
int CNControl(int);
#endif
