static char sz__FILE__[]="@(#)pi_msgq.c	10.1.2.1(ver) Date Release 12/2/94";
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  pi_msgq.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:46:09
 * @(#)  Last delta: 12/2/94 at 18:52:46
 * @(#)  SCCS File: /taxi/sccs/s.pi_msgq.c
 *                                                                           *
 *****************************************************************************/

/**************************************************************************
*
*   Msgq.c
*   Tom Kee
*   Modification History
*   ====================
*   11.22.89    Created
*
***************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include "msgq.h"
#include "msgcomms.h"
#include "requests.h"

#define MONITOR_MSGQ_KEY     0x55667788L      /* IPC msgget key for MONITOR MSGQ */
#define QTYPE	1L 

int msgqid;
struct msgbuf msgqrec;
struct msqid_ds msgqstat;
static int ourpid;
int ss_size = sizeof(REQUESTDEF) - MAXREQ;
int no_msg_q = 0;

REQUESTDEF tmpmsg;

setourpid(pid)
{
	ourpid=pid;
}

 
PutDummyMsgQ(pid) 
int pid; 
{ 
    REQUESTDEF msg; 
 
      msg.r_mtype=(long)ourpid;   
      msg.r_rtype=M_QUERYTOKEN;
      msg.r_pid=pid; 
      msg.r_size=0; 
      msg.r_seq=0; 
  
      chk(msgsnd(msgqid, &msg,ss_size,IPC_NOWAIT));        
}
 

int OpenMsgQ()
{

    msgqid = msgget(MONITOR_MSGQ_KEY, 0666);
    if (msgqid == -1) 
    {
	no_msg_q=1;
	return(1);
    }
    ourpid=getpid();
    ss_size = sizeof(REQUESTDEF) - MAXREQ;
    return(0);

}

 
CreateMsgQ()
{

    msgqid = msgget(MONITOR_MSGQ_KEY, 0666|IPC_EXCL|IPC_CREAT);
    if (msgqid == -1)
    {
        msgqid = msgget(MONITOR_MSGQ_KEY, 0666);
        msgctl(msgqid, IPC_RMID, &msgqstat);
        msgqid = msgget(MONITOR_MSGQ_KEY,  IPC_CREAT|0666);
    	if (msgqid == -1)
    	{
      	  	 printf("Can't create message queue %ld\n",MONITOR_MSGQ_KEY );
       		 exit(1);
   	}
 
    }
    msgqid = msgget(MONITOR_MSGQ_KEY,  IPC_CREAT|0666);
    if (msgqid == -1)
    {
        printf("Can't create message queue %ld\n",MONITOR_MSGQ_KEY );
        exit(1);
    }
    ourpid=getpid();
    ss_size = sizeof(REQUESTDEF) - MAXREQ;
}


static chk(sts)
  int sts;
{
    extern int errno;
    if (sts == -1) 
    {
       printf("Call failed, errno=%d\n",errno);
    }
}

  
SendMsgQ(mtype, msg)
int mtype;
REQUESTDEF * msg;
{
    msg->r_mtype=(long)mtype;
    msg->r_rtype=M_QUERYTOKEN;
    msg->r_pid=ourpid;
    msg->r_size=ss_size;
    msg->r_seq=0; 
    
    chk(msgsnd(msgqid, msg,ss_size,IPC_NOWAIT));

}




int RecvMsgQ(mtype, msgx)
int mtype;
REQUESTDEF  *msgx;
{
    
    int len;
    long t;

    t=(long)mtype;

    if ((msgrcv(msgqid,msgx, sizeof(REQUESTDEF), t, IPC_NOWAIT)) == -1) 
    {
	if (errno==ENOMSG) return(0);
	else 
	{
		if (errno == EINVAL) 
		{
			OpenMsgQ();
			return(0);
		}
		else
		{
			perror("RecvMsgQ");
			return(-1);
		}
	}
    }
    else return(1);
}


CheckMsgQ()
{
    int ret;
    REQUESTDEF tmpmsg;

    if (no_msg_q) return;
    if ((ret=RecvMsgQ(ourpid, &tmpmsg)) == -1)  
    {
        perror("CheckMsgQ (recv) ");
	return;
    }
    else 
    {
        if (!ret) return;
        else SendMsgQ(tmpmsg.r_pid,&tmpmsg);
   }
    
}

int getourpid()
{
	return(ourpid);
}

RemoveMsgQ()
{

	if (msgctl(msgqid, IPC_RMID, &msgqstat)==-1) 
		perror("RemoveMsgQ");
}

DumpQStat()
{
    char * ctime();
    
       if (msgctl(msgqid, IPC_STAT, &msgqstat)==-1)  
                       perror("DumpQStat"); 
		       

	printf("\n msgqstat.msg_perm.cuid %d\n", msgqstat.msg_perm.cuid );     
	printf(" msgqstat.msg_perm.cgid %d\n", msgqstat.msg_perm.cgid );       
        printf(" msgqstat.msg_perm.uid %d\n", msgqstat.msg_perm.uid );       
        printf(" msgqstat.msg_perm.gid %d\n", msgqstat.msg_perm.gid );       
        printf(" msgqstat.msg_perm.mode %X\n", msgqstat.msg_perm.mode );
         
        printf(" msgqstat.msg_cbytes %d\n", msgqstat.msg_cbytes );     /* current # bytes on q */
        printf(" msgqstat.msg_qnum %d\n", msgqstat.msg_qnum );       /* # of messages on q */
        printf(" msgqstat.msg_qbytes %d\n", msgqstat.msg_qbytes );     /* max # of bytes on q */
        printf(" msgqstat.msg_lspid %d\n", msgqstat.msg_lspid );      /* pid of last msgsnd */
        printf(" msgqstat.msg_lrpid %d\n", msgqstat.msg_lrpid);      /* pid of last msgrcv */
        printf(" msgqstat.msg_stime %s", ctime(&msgqstat.msg_stime) );      /* last msgsnd time */
        printf(" msgqstat.msg_rtime %s", ctime(&msgqstat.msg_rtime) );      /* last msgrcv time */
        printf(" msgqstat.msg_ctime %s", ctime(&msgqstat.msg_ctime) );      /* last change time */
}
