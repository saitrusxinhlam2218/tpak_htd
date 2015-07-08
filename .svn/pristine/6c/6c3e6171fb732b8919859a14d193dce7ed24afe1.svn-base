static char sz__FILE__[]="@(#)vpipe.c  10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  vpipe.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:36:42                      *
* @(#)  Last delta: 12/2/94 at 12:15:51                                               *
* @(#)  SCCS File: /taxi/sccs/s.vpipe.c                                                       *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS                                          *
*                                                                           *
*****************************************************************************/
#include <sys/types.h>
#include <fcntl.h>

#include "enhance_com.h"
#include "enh_err.h"

/*************************************************************************
  Open_VPipe, Close_Vpipe, Read_VPipe and Write_VPipe are
  used to transfere verified data over a pipe.

  Write_VPipe transmit the data with a <stx>,<size> and <etx> sequence,
  so that Read_VPipe can filter out any bad data received.

  The protocol is as follow:
       <stx>, ESC + 'B'.
      <size>, 5 decimal ASCII number + NULL (total 6)
              containing the total block size tranmittet.
      <data>, send as given, exept for ESC with is send
              as ESC + 'e'.
       <etx>, ESC + 'E'.
*************************************************************************/



#define ESC_CHR 27
#define ESC_ESC 'e'
#define ESC_BEGIN 'B'
#define ESC_END 'E'
#define MAX_VPIPES 10



typedef struct _vpipe
{
  int fd;
  int next;
  int size;
  int cachesize;
  char *cache;
} VPIPE;

VPIPE VHandle[MAX_VPIPES];

int Initialized=0;



 /**************************************************************************
  * Name: Open_VPipe(char *PipeName,char *LookAHead,int LAHSize)           *
  * Type:                                                                  *
  *                                                                        *
  * Purpose: Open the named pipe for verified read/write                   *
  *                                                                        *
  * Parameters: PipeName  : NULL terminated pipename.                      *
  *             LookAHead : Look-a-head buffer. Cache used by              *
  *                         Read_VPipe() to speed up operation.            *
  *             LAHSize   : Size of LookHead buffer.                       *
  *                         Must be greater than largest block             *
  *                         to read.                                       *
  *                                                                        *
  * Returnvalue: On success : VPipe handle (>=0)                           *
  *              On failure : -1                                           *
  **************************************************************************/

int Open_VPipe(PipeName,LookAHead,LAHSize)
char *PipeName,*LookAHead;
int LAHSize;
{
  int fd,i;

  if(!Initialized)                                          /* initialize VPIPE's table */
  {
    for(i=0; i<MAX_VPIPES; i++) VHandle[i].fd=0;
    Initialized=1;
  }
  for(i=0; i<MAX_VPIPES; i++) if(!VHandle[i].fd) break;     /* find free handle */
  if(i==MAX_VPIPES)
  {
    sprintf(szLogMsg,"Open_VPipe(): Too many open VPIPES, only %d allowed",MAX_VPIPES);
    ENH_ERROR(szLogMsg,NO_ERROR);
    return(-1);
  }
  if(LAHSize<10)                                            /* must be space for <stx><len> and <etx> */
  {
    sprintf(szLogMsg,"Open_VPipe(): LAHSize to small");
    ENH_ERROR(szLogMsg,NO_ERROR);
    return(-1);
  }
  fd=open(PipeName,O_RDWR | O_NDELAY);                      /* open pipe for read and write */
  if(fd<0)
  {
    sprintf(szLogMsg,"Open_VPipe(): Can't open pipe (%s)",PipeName);
    ENH_ERROR(szLogMsg,NO_ERROR);
    return(-1);
  }
  VHandle[i].fd=fd;                                         /* preset handle structure */
  VHandle[i].cache=LookAHead;
  VHandle[i].cachesize=LAHSize;
  VHandle[i].next=VHandle[i].size=0;
  return(i);
}



 /**************************************************************************
  * Name: Write_VPipe(int Handle,char *Data, int Len)                      *
  * Type:                                                                  *
  *                                                                        *
  * Purpose: Write datablock into verified pipe associated to Handle       *
  *                                                                        *
  * Parameters: Handle    : VPipe handle returned by Open_VPipe().         *
  *             Data      : Data to send through VPipe.                    *
  *             Len       : Size of Data.                                  *
  *                                                                        *
  * Returnvalue: On success : Len                                          *
  *              On failure : -1                                           *
  **************************************************************************/

int Write_VPipe(Handle,Data,Len)
int Handle,Len;
char *Data;
{
  char wrk[5000],*src,*dst,*end;
  VPIPE *hnd;
  int i;

  if(!Initialized)                                          /* initialize VPIPE's table */
  {
    for(i=0; i<MAX_VPIPES; i++) VHandle[i].fd=0;
    Initialized=1;
  }
  if(Handle<0 || Handle>=MAX_VPIPES)                        /* check handle */
  {
    sprintf(szLogMsg,"Write_VPipe(): Invalid VPipe handle (%d)",Handle);
    ENH_ERROR(szLogMsg,NO_ERROR);
    return(-1);
  }
  hnd= &VHandle[Handle];
  if(!hnd->fd)                                              /* check pipe_fd for handle */
  {
    sprintf(szLogMsg,"Write_VPipe(): Invalid VPipe handle (%d)",Handle);
    ENH_ERROR(szLogMsg,NO_ERROR);
    return(-1);
  }
  if(Len>4000)                                              /* check data len */
  {
    sprintf(szLogMsg,"Write_VPipe() blocksize is limmited to 4000 in VPipe handle (%d)",Handle);
    ENH_ERROR(szLogMsg,NO_ERROR);
    return(-1);
  }
  if(Len<0 || Len>hnd->cachesize-10)                        /* check data len */
  {
    sprintf(szLogMsg,"Write_VPipe() blocksize is limmited to %d in VPipe handle (%d)",hnd->cachesize-10,Handle);
    ENH_ERROR(szLogMsg,NO_ERROR);
    return(-1);
  }

  /* generate block to send, <stx>+<len>+<data>+<etx> */
  dst=wrk;
  dst[0]=ESC_CHR; dst[1]=ESC_BEGIN;                         /* make <stx> */
  dst+=8;                                                   /* skip <stx>+<len> */
  for(src=Data,end=src+Len; src<end; src++,dst++)           /* copy data changing all ESC_CHR's */
  {
    if((*dst= *src)==ESC_CHR) { dst++; *dst=ESC_ESC; }
  }
  dst[0]=ESC_CHR; dst[1]=ESC_END;                           /* make <etx> */
  dst+=2;
  sprintf(wrk+2,"%05d",dst-wrk);                            /* writeback <len> now known */
  if(write(hnd->fd,wrk,dst-wrk)<0)                          /* send block */
  {
    sprintf(szLogMsg,"Write_VPipe(): Error while writing %d bytes to VPipe handle (%d)",Len,Handle);
    ENH_ERROR(szLogMsg,NO_ERROR);
    return(-1);
  }
  return(Len);
}



 /**************************************************************************
  * Name: Close_VPipe(int Handle)                                          *
  * Type:                                                                  *
  *                                                                        *
  * Purpose: Close the pipe associated to Handle                           *
  *                                                                        *
  * Parameters: Handle    : VPipe handle returned by Open_VPipe().         *
  *                                                                        *
  * Returnvalue: On success : =0                                           *
  *              On failure : -1                                           *
  **************************************************************************/

int Close_VPipe(Handle)
int Handle;
{
  int i,r;

  if(!Initialized)                                          /* initialize VPIPE's table */
  {
    for(i=0; i<MAX_VPIPES; i++) VHandle[i].fd=0;
    Initialized=1;
  }
  if(Handle<0 || Handle>=MAX_VPIPES)                        /* check handle */
  {
    sprintf(szLogMsg,"Close_VPipe(): Invalid VPipe handle (%d)",Handle);
    ENH_ERROR(szLogMsg,NO_ERROR);
    return(-1);
  }
  if(!VHandle[Handle].fd)                                   /* check pipe_fd for handle */
  {
    sprintf(szLogMsg,"Close_VPipe(): Invalid VPipe handle (%d)",Handle);
    ENH_ERROR(szLogMsg,NO_ERROR);
    return(-1);
  }
  r=close(VHandle[Handle].fd);
  VHandle[Handle].fd=0;
  return(r);
}



 /**************************************************************************
  * Name: Read_VPipe(int Handle, char *Data)                               *
  * Type:                                                                  *
  *                                                                        *
  * Purpose: Read data from a verified pipe associated to Handle.          *
  *          Only non-corrupted data returned.                             *
  *                                                                        *
  * Parameters: Handle    : VPipe handle returned by Open_VPipe().         *
  *             Data      : Buffer to recieved data.                       *
  *                         Wont be overwritten on failure reads.          *
  *                                                                        *
  * Returnvalue: On success : length of recieved data                      *
  *              On failure : -1 (no completed data blocks)                *
  **************************************************************************/

int Read_VPipe(Handle,Data)
int Handle;
char *Data;
{
  int i,l;
  VPIPE *hnd;
  char *src,*dst,*end;

  if(!Initialized)                                          /* initialize VPIPE's table */
  {
    for(i=0; i<MAX_VPIPES; i++) VHandle[i].fd=0;
    Initialized=1;
  }
  if(Handle<0 || Handle>=MAX_VPIPES)                        /* check handle */
  {
    sprintf(szLogMsg,"Write_VPipe(): Invalid VPipe handle (%d)",Handle);
    ENH_ERROR(szLogMsg,NO_ERROR);
    return(-1);
  }
  hnd= &VHandle[Handle];
  if(!hnd->fd)                                              /* check pipe_fd for handle */
  {
    sprintf(szLogMsg,"Write_VPipe(): Invalid VPipe handle (%d)",Handle);
    ENH_ERROR(szLogMsg,NO_ERROR);
    return(-1);
  }
  if(hnd->next<hnd->cachesize)                              /* fill lookahead buffer from pipe */
  {
    i=read(hnd->fd,hnd->cache+hnd->next,hnd->cachesize-hnd->next);
    if(i>0) hnd->next+=i;
  }

  /* read and check data */
  if(hnd->next<8) return(-1);                               /* need at least <stx>+<len> */
  if(hnd->cache[0]!=ESC_CHR || hnd->cache[1]!=ESC_BEGIN)    /* buffer should allways start with a <stx> */
  {                                                         /* if not sync it to next ESC_CHR */
    src=hnd->cache;
    end=src+hnd->next;
    for(src++; src<end && *src!=ESC_CHR; src++);
    l=src-hnd->cache;
    memcpy(hnd->cache,src,hnd->next-l);
    hnd->next-=l;
    return(-1);
  }
  if(hnd->size==0)                                          /* get blocksize */
  {
    hnd->cache[7]=0; hnd->size=atoi(hnd->cache+2);
  }
  if(hnd->next<hnd->size) return(-1);                       /* need a full block to check it */
  src=hnd->cache;
  end=src+hnd->size-2;
  for(src+=8; src<end; src++)                               /* search for wrong ESC_CHR's */
  {
    if(*src==ESC_CHR)
    {
      if(src[1]==ESC_ESC) src++; else break;
    }
  }
  if(src==end && src[0]==ESC_CHR && src[1]==ESC_END)        /* if <etx> is found in the right place */
  {                                                         /* <data> should be OK */
    dst=Data;                                               /* copy it to caller */
    src=hnd->cache;
    end=src+hnd->size-2;
    for(src+=8,l=0; src<end; l++,src++,dst++)
    {
      if((*dst= *src)==ESC_CHR) { src++; *dst=ESC_CHR; }
    }
    memcpy(hnd->cache,end+2,hnd->next-hnd->size);           /* remove block from buffer */
    hnd->next-=hnd->size; hnd->size=0;
    return(l);
  }
  l=src-hnd->cache;
  memcpy(hnd->cache,src,hnd->next-l);                       /* bad <data>, remove it from buffer */
  hnd->next-=l; hnd->size=0;
  return(-1);
}
