/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: socketsx.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1997/08/09 17:54:54 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/common/socketsx.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: socketsx.c,v 1.2 1997/08/09 17:54:54 taxiadm Exp $";

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <signal.h>
#include <setjmp.h>
#include <varargs.h>
#include <sys/stat.h>
#include "machdep.h"
#include "socketsx.h"
#ifndef SYSV
#include <sys/wait.h>
#include <sys/resource.h>
#endif


int NetStat = NETDOWN;
int s;
jmp_buf alrm_trap;



int cprintf(f,l,va_alist)
char * f;
int l;
va_dcl
{

	va_list args;
	FILE *fd;
	char * fmt;
	char buf[130];
	char tempbuf[130];
	extern int errno;
	extern char * sys_errlist[];

 
	fd = fopen("/dev/console","w");
	if (fd == NULL) return(0);
	va_start(args);
	fmt = va_arg(args,char *);
	vsprintf(buf,fmt,args);
	va_end(args);
	if (errno) 
	{
		sprintf(tempbuf,"-%s-",sys_errlist[errno]);
		strcat(buf,tempbuf);
		errno=0;
	}
	sprintf(tempbuf,"Socket: %s",buf);
	mad_error(' ',"",tempbuf,f,l);
	fprintf(fd,"%s\n",buf);
	fclose(fd);

  	fflush(stdout);

}

int s_fatal(f,l,va_alist)
char * f;
int l;
va_dcl
{
	cprintf(f,l,va_alist);
	exit (0);
}

/*******************************************************************************
	CloseConnection()
	clean exit and close of sockets
*********************************************************************************/

CloseConnection(r)
int r;
{
	shutdown(r,2);
	close(r);
}

/****************************************************************************
	death()
	Prevent zombie childern
*****************************************************************************/

death()
{
#ifndef SYSV

#ifdef FOO
	union wait status;
	while (wait3(&status,WNOHANG,0) > 0);
#endif
#else
	

#endif
}


set_socket_down(sig)
int sig;
{
	if (sig == SIGPIPE)
		s_fatal(__FILE__,__LINE__,"SIGPIPE signal caught - socket down -- killing server");
	else
		s_fatal(__FILE__,__LINE__,"In set_socket_down ... signal %d -- killing server",sig);

	NetStat=NETDOWN;
	CloseSocket();
}


CreateSocket(servername)
char * servername;
{
	struct sockaddr_in server;
	int sts;
	int intval=1;
	
	signal_init(set_socket_down);

	/* attempt to open socket */
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		s_fatal(__FILE__,__LINE__,"CreateSocket -- Can't open socket");

	/* figure out what my name should be */
	if (sts = getmyaddr(servername, &server))
		s_fatal(__FILE__,__LINE__,"CreateSocket -- what is my name anyway? == %d",sts);

	/* set reuse address so that bind doesn't think address is in use
	   if server is killed and immediately restarted */
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &intval, sizeof(int)) < 0)
		s_fatal(__FILE__,__LINE__,"CreateSocket -- setsockopt failed");
	/* assign a name to the socket */
	if (bind(s, &server, sizeof(server)) < 0)
	{
		CloseConnection(s);
		s_fatal(__FILE__,__LINE__,"CreateSocket -- bind failed");
	}
	/* start listening for requests */
	if (listen(s,5) < 0) 
		s_fatal(__FILE__,__LINE__,"CreateSocket -- listen failed");
	NetStat = NETUP;

}

static int getmyaddr(sname, sin)
char *sname; struct sockaddr_in *sin;
{
	struct servent *sp;
	struct hostent *hp;
	int server_port;

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
	bzero(sin, sizeof(*sin));
#endif
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = INADDR_ANY;
	sin->sin_port = server_port;

	
	return(0);
}

int MakeConnection()
{
	extern int errno;
	struct sockaddr_in from;
	int len = sizeof(from), r;

	while ((r = accept(s, &from, &len)) == -1)
		if (errno != 4)     /*EINTR*/
			s_fatal(__FILE__,__LINE__,"MakeConnection failed");
	return(r);
}

REQHDR * AllocRequest(size)
unsigned size;
{
	extern char * malloc();
	REQHDR * tmp;
	tmp=(REQHDR *)malloc(size);
	if (tmp==NULL) 
		s_fatal (__FILE__,__LINE__,"Memory allocation error");
	return(tmp);
}

FreeRequest(req)
REQHDR * req;
{
	free(req);
}

FillACKPacket(req)
REQHDR * req;
{
	req->size=0;
	req->action=TSUCCESS;
	req->rec_no=0;
	req->file=0;
	req->data[0]=0;
}

int Ssend(buffer,size)
char * buffer;
int size;
{
	REQHDR * req;
	int cc=0;

	if (size)
	{					/* safety valve for senders ignorance */
		if ((cc=send(s, buffer, size, 0)) != size) 
		{
			if (cc == -1)
			{
				if (errno != 4)
				{
					CloseConnection(s);
					s_fatal(__FILE__,__LINE__,"Ssend failed");
				}
			}
			cc=0;
		}
		else cc=1;
	}
	return(cc);
	
}


int Srecv(r, buffer, size)
int r;
char *buffer;
int size;
{

	int ret;
	
	if ((ret = recv(r, buffer, size,0)) < 1)
	{
		if (ret == -1)
		{
			if (errno != 4)
			{
				cprintf("Errno = %d, r = %d, buffer %s, size = %d",errno,r,buffer,size);
				CloseConnection(r);
				s_fatal(__FILE__,__LINE__,"Srecv failed");
			}
		}
		ret=0; 	

	}
	return(ret);

}
 
 
int CheckHandle(x)
int x;
{
                struct stat  stat;
 
                if (fstat(x,&stat) == -1)
                {
                        cprintf(__FILE__,__LINE__,"CheckHandle returns -1 with errno %d",errno);
                        if ((errno==EBADF)||(errno==EINVAL))
                        {
                                return(1);
                        }
                }
                return(0);
 
}


REQHDR *RcvRequest(r, ptype, psize)
int r;
int *ptype;
int *psize;
{

	REQHDR * req;
	int rsize;	/* amount to receive with the header payload */
	
	if (CheckHandle (r)) 
	{
		CloseConnection(r);
		s_fatal(__FILE__,__LINE__,"RecvRequest detects invalid socket handle.");
	}
	req = AllocRequest(MAXREQ);
	if (recv(r, req, TRANS_HDR_SIZE, 0) < TRANS_HDR_SIZE)
	{
		FreeRequest(req);
		if (*psize == -1)
		{
			if (errno != 4)
			{
				CloseConnection(r);
				s_fatal(__FILE__,__LINE__,"RecvRequest on header failed");
			}
		}
		return(NULL);
	}

	/* Determine how much data to receive with the header payload. */
	if ((req->size - TRANS_HDR_SIZE) < TRANS_DATA_SIZE)
		rsize = req->size - TRANS_HDR_SIZE;
	else
		rsize = TRANS_DATA_SIZE;

	if ((*psize = recv(r, req->data, rsize,0)) < rsize)
	{
		FreeRequest(req);
		if (*psize == -1)
		{
			if (errno != 4)
			{
				CloseConnection(r);
				s_fatal(__FILE__,__LINE__,"RecvRequest on first payload failed");
			}
		}
		return(NULL);
	}
	*psize += TRANS_HDR_SIZE;	/* for backward compatibility */
	*ptype = req->action;
	return(req);
}

SndResponse(r, req, reqsize)
int r;
REQHDR * req;
int reqsize;
{
	int cc;
	if (reqsize)					/* safety valve for senders ignorance */
	if ((cc=send(r, req, reqsize, 0)) != reqsize) 
	{
		FreeRequest(req);
		if (cc == -1)
		{
			if (errno != 4)
			{
				CloseConnection(r);
				s_fatal(__FILE__,__LINE__,"SndResponse failed");
			}
		}

	}
	FreeRequest(req);
}


char  n[40];
char  sx[40];


OpenSocket(rmtname,servername)
char * rmtname, * servername;
{
 	int sts;
	struct sockaddr_in server;
        
	strcpy (n,rmtname);
	strcpy (sx, servername);	
	signal_init(set_socket_down);
	/* get the address of the server */
	if (sts = getserveraddr(rmtname, servername, &server))
		s_fatal(__FILE__,__LINE__,"OpenQueue  -- where is the server anyway? == %d",sts);
	
	/* create a socket and establish a connection */
	if (((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) || connect(s, &server, sizeof(server)) != 0) 
		s_fatal(__FILE__,__LINE__,"OpenQueue -- can't connect to the server");
	NetStat = NETUP;
}

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
	sin->sin_port = server_port;
	sin->sin_family = hp->h_addrtype;
	return(0);

}

timeout()
{
        longjmp(alrm_trap);
}

int MakeRequest(type, req, reqsize)
int type;
REQHDR *req;
int reqsize;
{
       int retry = 5;
	int loadtry = 3;

        if (setjmp(alrm_trap) != 0)
        {
                if (--retry < 0)
		{
                	if (--loadtry > 0)
			{
             	                cprintf(__FILE__,__LINE__,"MakeRequest -- timeout and aborted--trying to reconnect....");
				OpenSocket(n,sx);
				
				retry = 3;
			}
			else s_fatal(__FILE__,__LINE__,"Connection can not be re-established!");
		}
        }
  	
	req->action = type;

	if (send(s, req, reqsize,0) != reqsize)
		if (errno != 4)
			s_fatal(__FILE__,__LINE__,"MakeRequest -- send failed");
        signal(SIGALRM,timeout);
        alarm(5);
     
	if (recv(s,req,MAXREQ,0) == -1)
		if (errno != 4)
			s_fatal(__FILE__,__LINE__,"MakeRequest -- receive failed");
        alarm(0);
	
}


signal_init(sig_handler)
int (*sig_handler)();
{
 
        signal(SIGHUP, SIG_IGN);                /* 01  Hang up */
        signal(SIGINT, SIG_IGN);                /* 02  Interrupt */
        signal(SIGILL, sig_handler);            /* 04  Illegal instruction */
        signal(SIGTRAP, SIG_DFL);               /* 05  Trace Trap */
        signal(SIGIOT, SIG_DFL);                /* 06  IOT instruction */
        signal(SIGEMT, SIG_DFL);                /* 07  EMT instruction */
        signal(SIGFPE, SIG_DFL);                /* 08  Floating Point Exception */
        signal(SIGKILL, SIG_DFL);               /* 09  Kill, can't be caught */
        signal(SIGBUS, sig_handler);            /* 10  Bus error */
        signal(SIGSEGV, sig_handler);           /* 11  Segmentation Violation */
        signal(SIGSYS, SIG_DFL);                /* 12  bad arg System call */
        signal(SIGPIPE, sig_handler);           /* 13  Pipe error */
        signal(SIGALRM, SIG_IGN);               /* 14  Alarm */
        signal(SIGTERM, sig_handler);           /* 15  software Terminate */
        signal(SIGUSR1, sig_handler);           /* 16  User defined signal 1 */
#ifdef SYSV
	signal(SIGPOLL,SIG_IGN);
#endif

        signal(SIGCLD, SIG_DFL);
}
/*
dumppak(req)
REQHDR * req;
{
 
	cprintf("In dump pak \n");

        cprintf("req->size = %d\n",req->size);
        cprintf("req->action = %lu\n",req->action);
        cprintf("req->file = %d\n",req->file);
        cprintf("req->rec_no = %d\n",req->rec_no);
        cprintf("req->data = %s\n",req->data);
}
 */  
/*
DumpPak(req)
REQHDR * req;
{
	printf("This is without net conversions\n");

	printf("req->req_payload_size = %d\n",req->req_payload_size);
	printf("req->req_mtype = %lu\n",req->req_mtype);
	printf("req->req_type = %d\n",req->req_type);
	printf("req->req_pid = %lu\n",req->req_pid);
	printf("req->req_status = %d\n",req->req_status);
	printf("req->req_payload = %s\n",req->req_payload);

	printf("This is with net conversions\n");
	printf("req->req_payload_size = %d\n",ntohs(req->req_payload_size));
	printf("req->req_mtype = %lu\n",ntohl(req->req_mtype));
	printf("req->req_type = %d\n",ntohs(req->req_type));
	printf("req->req_pid = %lu\n",ntohl(req->req_pid));
	printf("req->req_status = %d\n",ntohs(req->req_status));
	printf("req->req_payload = %s\n",req->req_payload);

	printf("long %d\n",sizeof(long));
	printf("int %d\n",sizeof(int));
	printf("HDRSIZE %d\n",HDRSIZE);
	printf("MAXREQ %d\n",MAXREQ);

}
*/


GetConnectName(r, name)
int r;
char * name;
{
	struct sockaddr_in sa;
	int len=sizeof(sa);

	if (getpeername(r, &sa, &len) == -1)
		s_fatal (__FILE__,__LINE__,"GetConnectName -- failed");
	strcpy(name,inet_ntoa(sa.sin_addr));
}

HandleConnection(r,ProcessRequest)
int r;
int (*ProcessRequest)();
{
	REQHDR * RcvRequest(), * req;
	int reqtype, reqsize;
	long curtime;
	int ret;
	char peername[20];


	GetConnectName(r,peername);
	time(&curtime);
        cprintf(__FILE__,__LINE__,"Spawned server process for %s at %s",peername,ctime(&curtime));
	signal(SIGPIPE,set_socket_down);		/* if net goes down, catch signal on write*/
	while (req = RcvRequest (r, &reqtype, &reqsize))
	{
		ProcessRequest(r,reqtype, req,reqsize);
		
		if (req->action != 200)
		{
			SndResponse(r, req, HDRSIZE+req->size);
		}
	}
	CloseConnection(r);
	exit(0);
}
/****************************************************************************
	CloseSocket()
	Clean exit 
	Signals should be sent here
****************************************************************************/
CloseSocket()
{
	CloseConnection(s);
}
